// CallBu.cpp: implementation of the CCallBu class.
//
//////////////////////////////////////////////////////////////////////

#include "CallBu.h"
#include "dreberrcode.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
typedef CCallBu *(*NEWBULOGEINSTANCE)();
#define  DBPWDKEY  "rd402wms"
#define  DBPWD3DESKEY   "rd402wms20092017Ylink123"

CCallBu::CCallBu()
{
	m_bNeedConnectDb = false;
	
	
	m_nAutoCommit=0;   //自动提交数据库选项 1自动提交，其它否
	m_nAutoLoadDll=0;  //是否自动重载动态库
	m_nTimeTest=0;     //是否记录交易时间
	m_nTxUnit2Func=0;  //是否把原子转换为交易
	m_nTestLopBack=0;  //是否测试回路
	m_nFlowUse = 0;
	m_bIsSetPara = false;
	bzero(m_cMsg,sizeof(m_cMsg));
	m_pPcLink = NULL;
	m_pSqlCmd = NULL;
	m_pFlow = new CBuFlowControl();
	
}

CCallBu::~CCallBu()
{
	if (m_pSqlCmd != NULL)
	{
		delete m_pSqlCmd;
		m_pSqlCmd = NULL;	
	}
	if (m_pFlow != NULL)
	{
		delete m_pFlow;
		m_pFlow = NULL;	
	}
	
}
int CCallBu::Run(PBPCCOMMSTRU data)
{
	int ret; 


	if (m_nAutoLoadDll == 1)
	{
		if (InitAll())
		{
			AnsErr(ERR_BPU_INIT,__FILE__,__LINE__,"初始化BPU出错",data);
			return -1;
		}
	}
	//运行内部交易
	if (data->sDBHead.d_Dinfo.d_nServiceNo > 999999000 && data->sDBHead.d_Dinfo.d_nServiceNo < 999999999)
	{
		try
		{
			ret = RunLocalFunc(data);
		}
		catch (...)
		{
			AnsErr(ERR_BPU_CATCH,__FILE__,__LINE__,"BPU异常出错",data);
			return -1;
		}
		return 0;
	}


	//是否测试回路
	if (m_nTestLopBack == 1) //测试回路
	{
		char testfilename[200];
		memset(testfilename,0,sizeof(testfilename));
		if (m_sLopBackFilePath.length() >0)
		{
			sprintf(testfilename,"%s/test%d.xml",m_sLopBackFilePath.c_str(),data->sDBHead.d_Dinfo.d_nServiceNo);
		}
		else
		{
			sprintf(testfilename,"test%d.xml",data->sDBHead.d_Dinfo.d_nServiceNo);
		}
		m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"测试返回文件[%s]",testfilename);
//		m_pPcLink->AnsData(data);
		FILE *fp = fopen(testfilename,"rb");
		if (fp == NULL)
		{
			AnsErr(ERR_BPU_NOTFUNC,__FILE__,__LINE__,"测试回路文件不存在",data);
			return -1;
		}
		data->sDBHead.nLen = fread(data->sBuffer,1,BPUDATASIZE,fp);
		if (data->sDBHead.nLen<0)
		{
			AnsErr(ERR_BPU_NOTFUNC,__FILE__,__LINE__,"读测试回路文件出错",data);
			fclose(fp);
			return -1;
		}
		data->sBpcHead.nBpcLen = DREBHEADLEN+data->sDBHead.nLen;
		fclose(fp);
		data->sDBHead.cRaflag = 1;
		data->sDBHead.cNextFlag = 0;
		data->sDBHead.cZip = 0;
		m_pPcLink->AnsData(data);
		return 0;
	}
	
	//运行流程中的交易
	if (m_nFlowUse == 1)
	{
		if (m_pFlow->IsRunFlow(data->sDBHead.d_Dinfo.d_nServiceNo))
		{
			try
			{
				ret = m_pFlow->RunFlow(data);
				//流程是否提交在RunFlow里面进行

			}
			catch (...)
			{
				AnsErr(ERR_BPU_CATCH,__FILE__,__LINE__,"调用流程控制异常出错",data);
				return -1;
			}
		}
	}
	//运行动态库中的功能
	S_TBL_FUNC funcinfo;
	if (!m_pFuncMgr.m_pFuncTbl.Select(data->sDBHead.d_Dinfo.d_nServiceNo,funcinfo))
	{
		if (m_nTxUnit2Func == 1) //原子转换为功能
		{
			if (!m_pFuncMgr.m_pTxUnitTbl.Select(data->sDBHead.d_Dinfo.d_nServiceNo,funcinfo))
			{
				sprintf(m_cMsg,"无此交易和原子交易 %d",data->sDBHead.d_Dinfo.d_nServiceNo);
				AnsErr(ERR_BPU_NOTFUNC,__FILE__,__LINE__,m_cMsg,data);
				return -1;
			}
		}
		else
		{
			sprintf(m_cMsg,"无此交易 %d",data->sDBHead.d_Dinfo.d_nServiceNo);
			AnsErr(ERR_BPU_NOTFUNC,__FILE__,__LINE__,m_cMsg,data);
			return -1;
		}
	}
	try
	{
		if (m_bNeedConnectDb) //有连接数据库则需要判断数据提交
		{
			if (m_nAutoCommit == 1) //自动提交数据库
			{
				m_pSqlCmd->Commit(); 
			}
		}
		ret = (funcinfo.pBuInstance->*(funcinfo.pFunc))(data);
		if (m_bNeedConnectDb) //有连接数据库则需要判断数据提交
		{
			if (m_nAutoCommit == 1) //自动提交数据库
			{
				if (ret == 0)  //提交
				{
					m_pSqlCmd->Commit(); 
				}
				else           //回滚
				{
					m_pSqlCmd->Rollback();
				}
			}
		}
	}
	catch (...)
	{
		AnsErr(ERR_BPU_CATCH,__FILE__,__LINE__,"调用交易或原子功能异常出错",data);
		return -1;
	}
	return 0;
}

bool CCallBu::BuInit(const char *filename)
{
	m_sConfigXmlFile = filename;
	return InitAll();
}

bool CCallBu::InitLogClass()
{
	std::string sDllpath;
	std::string sDllname;
	std::string sMethod;
	int nLogLevel;
	std::string sLogFile;
	std::string sLogPath;
	bool bIsUseBuLog=false;

	if (m_pConfXml.GetNodeValueByPath("package/head/public/loglevel",false,nLogLevel) == NULL)
	{
		m_errMsg = std::string("节点[package/head/public/loglevel]不存在");
		return false;
	}

	if (m_pConfXml.GetNodeValueByPath("package/head/public/logdir",false,sLogPath,false) == NULL)
	{
		m_errMsg = std::string("节点[package/head/public/logdir]不存在");
		return false;
	}

	if (m_pConfXml.GetNodeValueByPath("package/head/public/logfile",false,sLogFile,false) == NULL)
	{
		m_errMsg = std::string("节点[package/head/public/logfile]不存在");
		return false;
	}
	
	m_pLog.SetLogPara(nLogLevel,sLogPath.c_str(),sLogFile.c_str());
	return true;
}

bool CCallBu::InitDatabase()
{
	int nUseDb=0;
	if (m_pConfXml.GetNodeAttribute("package/head/business/database",false,"use",false,nUseDb) == NULL)
	{
		m_errMsg = std::string("节点[package/head/business/database]的属性use不存在");
		m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg.c_str());
		return false;
	}
	if (nUseDb == 1)
	{
		m_bNeedConnectDb = true;
	}
	else
	{
		m_bNeedConnectDb = false;
	}
	if (m_bNeedConnectDb)
	{
		char tmpchar[100];
		memset(tmpchar,0,sizeof(tmpchar));
		std::string dbname;
		std::string dbuser;
		std::string dbpwd;
		std::string dbpwdflag;
		std::string dbtype;
		if (m_pConfXml.GetNodeAttribute("package/head/business/database",false,"commit",false,m_nAutoCommit) == NULL)
		{
			m_errMsg = std::string("节点[package/head/business/database]的属性commit不存在");
			m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg.c_str());
			return false;
		}
		if (m_pConfXml.GetNodeValueByPath("package/head/business/database/databaseserver",false,dbname,false) == NULL)
		{
			m_errMsg = std::string("节点[package/head/business/database/databaseserver]不存在");
			m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg.c_str());
			return false;
		}
		if (m_pConfXml.GetNodeValueByPath("package/head/business/database/databaseuser",false,dbuser,false) == NULL)
		{
			m_errMsg = std::string("节点[package/head/business/database/databaseuser]不存在");
			m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg.c_str());
			return false;
		}
		if (m_pConfXml.GetNodeValueByPath("package/head/business/database/userpasswd",false,dbpwd,true) == NULL)
		{
			m_errMsg = std::string("节点[package/head/business/database/userpasswd]不存在");
			m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg.c_str());
			return false;
		}
		if (m_pConfXml.GetNodeValueByPath("package/head/business/database/passwdtype",false,dbpwdflag,false) == NULL)
		{
			m_errMsg = std::string("节点[package/head/business/database/passwdtype]不存在");
			m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg.c_str());
			return false;
		}
		if (m_pConfXml.GetNodeValueByPath("package/head/business/database/databasetype",false,dbtype,false) == NULL)
		{
			m_errMsg = std::string("节点[package/head/business/database/databasetype]不存在");
			m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg.c_str());
			return false;
		}
		m_pLog.LogMp(LOG_INFO,__FILE__,__LINE__,"开始连接数据库");

		strcpy(tmpchar,dbpwd.c_str());
		if (atoi(dbpwdflag.c_str()) ==1 )
		{
//			m_pEncrypt.unencryptchar(tmpchar,strlen(tmpchar),DBPWDKEY); 
			int len = strlen(tmpchar);
			if (m_pEncrypt.UncryptString(tmpchar,len,DBPWDKEY)<0)
			{
				m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"数据库密码解密出错");
				return false;
			}
		}
		else if (atoi(dbpwdflag.c_str()) == 2 )
		{
			int len = strlen(tmpchar);
			CBF_3Des des;
			char errmsg[200];
			if (!des.UnDes3Buffer(tmpchar,len,DBPWD3DESKEY,errmsg))
			{
				m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"数据库密码解密出错 %s",errmsg);
				return false;
			}
		}
		m_pSqlCmd = new CDbApi();
		if (m_pSqlCmd == NULL)
		{
			m_errMsg = std::string("数据库连接实例化失败，分配空间失摆");
			m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,m_errMsg.c_str());
			return false;
		}
		if (!m_pSqlCmd->Connect(dbname,dbuser,tmpchar,atoi(dbtype.c_str())))
		{
			m_errMsg = std::string("连接数据库错:")+m_pSqlCmd->m_dbconn.m_sErrInfo;
			m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"连接数据库错 %d %s",m_pSqlCmd->m_dbconn.m_nErrCode,m_pSqlCmd->m_dbconn.m_sErrInfo.c_str());
			return false;
		}
		m_pLog.LogMp(LOG_INFO,__FILE__,__LINE__,"连接数据库成功");
	}
	else
	{
		m_pLog.LogMp(LOG_INFO,__FILE__,__LINE__,"未配置使用数据库");
	}
	return true;
}
bool CCallBu::InitBu()
{
	m_pFuncMgr.SetBuPara(&m_pConfXml,m_pPcLink,m_pSqlCmd,&m_pLog);
	return m_pFuncMgr.InitFuncInfo();
}
bool CCallBu::InitFlow()
{
	if (m_pConfXml.GetNodeAttribute("package/head/business/flowcontrol",false,"use",false,m_nFlowUse) == NULL)
	{
		return true;
	}
	if (m_nFlowUse != 1)
	{
		m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置不使用流程控制");
		return true;
	}
	if (!m_bNeedConnectDb)
	{
		m_errMsg = "流程控制需连接数据库,但未配置数据库连接";
		m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,m_errMsg.c_str());
		return false;
	}
	std::string sDllpath;
	std::string sDllname;
	std::string sMethod;
	
	if (m_pConfXml.GetNodeAttribute("package/head/business/flowcontrol",false,"dllpath",false,sDllpath,false) == NULL)
	{
		m_errMsg = std::string("节点[package/head/business/flowcontrol]的属性dllpath不存在");
		m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,m_errMsg.c_str());
		return false;
	}
	
	if (m_pConfXml.GetNodeAttribute("package/head/business/flowcontrol",false,"dllname",false,sDllname,false) == NULL)
	{
		m_errMsg = std::string("节点[package/head/business/flowcontrol]的属性dllname不存在");
		m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,m_errMsg.c_str());
		return false;
	}
	if (m_pConfXml.GetNodeAttribute("package/head/business/flowcontrol",false,"method",false,sMethod,false) == NULL)
	{
		m_errMsg = std::string("节点[package/head/business/flowcontrol]的属性method不存在");
		m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,m_errMsg.c_str());
		return false;
	}
	

	m_pFlow->SetBuPara(&m_pConfXml,m_pPcLink,m_pSqlCmd,&m_pLog);
	if (!m_pFlow->InitFlow())
	{
		m_errMsg = std::string("流程动态库")+sDllname+"初始化失败";
		m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,m_errMsg.c_str());
		return false;
	}
	return true;
}



bool CCallBu::InitAll()
{
	if (!m_pConfXml.FromFile(m_sConfigXmlFile.c_str()))
	{
		m_errMsg = std::string("初始化配置文件出错:")+m_sConfigXmlFile;
		return false;
	}
	CXmlNode *getNode = m_pConfXml.GetNodeByPath("/package/head/business/test",false);
	if (getNode!=NULL)
	{
		std::string svalue;
		if(m_pConfXml.GetNodeValueByPath("/package/head/business/test/autoloaddll",false,svalue,false)!=NULL)
		{
			if (atoi(svalue.c_str())==1)
			{
				m_nAutoLoadDll = 1;
			}
		}
		if(m_pConfXml.GetNodeValueByPath("/package/head/business/test/timetest",false,svalue,false)!=NULL)
		{
			if (atoi(svalue.c_str())==1)
			{
				m_nTimeTest = 1;
			}
		}
		if(m_pConfXml.GetNodeValueByPath("/package/head/business/test/txtrans",false,svalue,false)!=NULL)
		{
			//m_pLog.LogMp(LOG_DEBUG+1, __FILE__, __LINE__, "package/head/business/test/txtrans=%s", svalue.c_str());
			if (atoi(svalue.c_str())==1)
			{
				m_nTxUnit2Func = 1;
			}

		}
		if(m_pConfXml.GetNodeValueByPath("package/head/business/test/testback",false,svalue,false)!=NULL)
		{
			m_nTestLopBack = atoi(svalue.c_str());
			if (m_nTestLopBack == 1 )
			{
				if (m_pConfXml.GetNodeAttribute("package/head/business/test/testback",false,"testfilepath",false,m_sLopBackFilePath,false) == NULL)
				{
					m_sLopBackFilePath = "";
				}
			}
		}
	}
	else
	{
		m_pLog.LogMp(LOG_INFO, __FILE__, __LINE__, "xml  /package/head/business/test is null");
	}
	if (!InitLogClass())
	{
		return false;
	}
	//初始化数据库
	if (!InitDatabase())
	{
		return false;
	}
	
	//初始化BU
	if (!InitBu())
	{
		return false;
	}
	//初始化流程控制
	if (!InitFlow())
	{
		return false;
	}
	return true;
}

int CCallBu::RunLocalFunc(PBPCCOMMSTRU data)
{
	switch (data->sDBHead.d_Dinfo.d_nServiceNo)
	{
		case 999999001:  //监控，直接返回
//			ReturnSuccess(pXml,"运行正常");
			break;
		case 999999002:  //取文件
			GetFile(data);
			break;
		case 999999003:  //上传文件
			PutFile(data);
			break;	
		case 999999004:  //获取业务动态库信息
			GetDllInfo(data);
			break;
		case 999999005:  //获取提供服务的所有交易信息
			GetTxInfo(data);
			break;
		default:
//			ReturnError(pXml,__FILE__,__LINE__,"970","无此内部功能");
			sprintf(m_cMsg,"无此交易和原子交易 %d",data->sDBHead.d_Dinfo.d_nServiceNo);
			AnsErr(ERR_BPU_NOTFUNC,__FILE__,__LINE__,m_cMsg,data);
			break;
			
	}
	return 0;
}


int CCallBu::GetFile(PBPCCOMMSTRU data)
{
	std::string pathfilename;
	int  offset;
	int getlen;
	char *databuf=NULL;
	FILE *fp;
	int ret;
	struct_stat f_stat;

// 	if (pXml->GetNodeValueByPath("package/pack/文件名",false,pathfilename) == NULL)
// 	{
// 		ReturnError(pXml,__FILE__,__LINE__,ERR_INPUT,"取文件时[package/pack/文件名]不存在");
// 		return -1;
// 	}
// 	if (pXml->GetNodeValueByPath("package/pack/偏移量",false,offset) == NULL)
// 	{
// 		ReturnError(pXml,__FILE__,__LINE__,ERR_INPUT,"取文件时[package/pack/偏移量]不存在");
// 		return -1;
// 	}
// 	if (pXml->GetNodeValueByPath("package/pack/每次大小",false,getlen) == NULL)
// 	{
// 		ReturnError(pXml,__FILE__,__LINE__,ERR_INPUT,"取文件时[package/pack/每次大小]不存在");
// 		return -1;
// 	}
// 	if (getlen <1 || getlen > 65536)
// 	{
// 		ReturnError(pXml,__FILE__,__LINE__,ERR_INPUT,"取文件时[package/pack/每次大小]非法");
// 		return -1;
// 	}
// 	if (offset <1)
// 	{
// 		ReturnError(pXml,__FILE__,__LINE__,ERR_INPUT,"取文件时[package/pack/偏移量]非法");
// 		return -1;
// 	}
// 	FSTAT(pathfilename.c_str(),&f_stat);
// 
// 	fp = fopen(pathfilename.c_str(),"rb");
// 	if (fp==NULL)
// 	{
// 		m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"文件名不存在 %s",(char *)pathfilename.c_str());
// 		ReturnError(pXml,__FILE__,__LINE__,ERR_OPENFILE,"取文件时文件不存在");
// 		return -1;
// 	}
// 	
// 	if (offset>=f_stat.st_size)
// 	{
// 		m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"后续包偏移量不符 %d %s",offset,(char *)pathfilename.c_str());
// 		ReturnError(pXml,__FILE__,__LINE__,ERR_INPUT,"后续包偏移量不符");
// 		fclose(fp);
// 		return -1;
// 	}
// 	fseek(fp,offset,1);
// 	databuf = (char *)malloc(getlen+1);
// 	if (databuf == NULL)
// 	{
// 		ReturnError(pXml,__FILE__,__LINE__,ERR_MALLOC,"取文件时分配空间失败");
// 		fclose(fp);
// 		return -1;
// 	}
// 	ret = fread(databuf,1,getlen,fp);
// 	databuf[ret]=0;
// 	fclose(fp);
// 	if (ret +offset >= f_stat.st_size)
// 	{
// 		pXml->SetNodeAttribute("package/Response",false,"Next",false,"0",false);//无后续包
// 	}
// 	else
// 	{
// 		pXml->SetNodeAttribute("package/Response",false,"Next",false,"1",false);//有后续包
// 	}
// 	pXml->SetNodeValueByPath("package/pack/偏移量",false,offset+ret);
// 	pXml->SetNodeValueByPath("package/pack/文件大小",false,f_stat.st_size);
// 	pXml->SetCdata(databuf,ret);
// 	free(databuf);
// 	databuf = NULL;
// 	ReturnSuccess(pXml,"取文件操作完成");
	return 0;
}

int CCallBu::PutFile(PBPCCOMMSTRU data)
{
	char *databuf=NULL;
	int datalen;
	FILE *fp;
	int ret;
	struct_stat f_stat;
	std::string pathfilename;
	char filenamepath[300];
	int  filestartflag;
	int  fileendflag;
	int  filesize;
	
// 	if (pXml->GetNodeValueByPath("package/pack/文件名",false,pathfilename) == NULL)
// 	{
// 		ReturnError(pXml,__FILE__,__LINE__,ERR_INPUT,"传文件时[package/pack/文件名]不存在");
// 		return -1;
// 	}
// 	if (pXml->GetNodeValueByPath("package/pack/文件开始标志",false,filestartflag) == NULL)
// 	{
// 		ReturnError(pXml,__FILE__,__LINE__,ERR_INPUT,"传文件时[package/pack/文件开始标志]不存在");
// 		return -1;
// 	}
// 	if (pXml->GetNodeValueByPath("package/pack/文件结束标志",false,fileendflag) == NULL)
// 	{
// 		ReturnError(pXml,__FILE__,__LINE__,ERR_INPUT,"传文件时[package/pack/文件结束标志]不存在");
// 		return -1;
// 	}
// 	if (pathfilename.length()<1)
// 	{
// 		ReturnError(pXml,__FILE__,__LINE__,ERR_INPUT,"传文件时[package/pack/文件名]非法");
// 		return -1;
// 	}
// 	sprintf(filenamepath,"%s",pathfilename.c_str());
// 	int j= strlen(filenamepath)-1;
// 	for (;j>0;j--)
// 	{
// 		if ((filenamepath[j] == '/') ||  (filenamepath[j] == '\\'))
// 		{
// 			filenamepath[j] =0;
// 			m_publog.makedir(filenamepath);
// 			break;
// 		}
// 	}
// 	
// 	datalen = pXml->GetCDataLen();
// 	if (datalen<1)
// 	{
// 		ReturnError(pXml,__FILE__,__LINE__,ERR_INPUT,"传文件时XML[package/Cdata]非法");
// 		return -1;
// 	}
// 	datalen+=1;
// 	databuf = (char *)malloc(datalen);
// 	if (databuf == NULL)
// 	{
// 		ReturnError(pXml,__FILE__,__LINE__,ERR_MALLOC,"传文件时分配空间失败");
// 		return -1;
// 	}
// 	if (!pXml->GetCData(databuf,datalen))
// 	{
// 		ReturnError(pXml,__FILE__,__LINE__,ERR_INPUT,"取Cdata失败");
// 		return -1;
// 	}
// 	
// 	
// 	if (filestartflag == 1)
// 	{
// 		fp = fopen(pathfilename.c_str(),"wb");
// 	}
// 	else
// 	{
// 		fp = fopen(pathfilename.c_str(),"ab");
// 	}
// 	
// 	if (fp == NULL)
// 	{
// 		ReturnError(pXml,__FILE__,__LINE__,ERR_OPENFILE,"打开文件失败");
// 		return -1;
// 	}
// 	fwrite(databuf,1,datalen,fp);
// 	fclose(fp);
// 	
// 	if (fileendflag == 1)
// 	{
// 		if (pXml->GetNodeValueByPath("package/pack/文件大小",false,filesize) == NULL)
// 		{
// 			ReturnError(pXml,__FILE__,__LINE__,ERR_INPUT,"传文件时[package/pack/文件大小]不存在");
// 			return -1;
// 		}
// 		struct_stat f_stat;
// 		FSTAT(pathfilename.c_str(),&f_stat);
// 		if (f_stat.st_size != filesize)
// 		{
// 			ReturnError(pXml,__FILE__,__LINE__,ERR_WRITEFILE,"写入文件出错，文件大小不符");
// 			return -1;
// 		}
// 		ReturnSuccess(pXml,"上传文件成功");
// 		return 0;
// 	}
// 	ReturnSuccess(pXml,"写入文件成功");
	return 0;
}

int CCallBu::GetDllInfo(PBPCCOMMSTRU data)
{
	char tmpchar[30];
	S_TBL_FUNC   func;
	int rec=0;
// 	CXmlNode *txnode = pXml->AddNodeByPath("package/head/ResultSet",false,"动态库信息",false,"",false);
// 	if (txnode != NULL)
// 	{
// //		ReturnError(pXml,__FILE__,__LINE__,ERR_OPXML,"SetNodeValueByPath出错");
// 		return -1;
// 	}
// 

// 	bool bRes = m_pFuncMgr.m_pFuncTbl.First(func);
// 	while (bRes)
// 	{
// 		sprintf(tmpchar,"REC%d",rec);
// 		CXmlNode *recnode = txnode->AddChildNode(tmpchar,false);
// 		if (recnode == NULL)
// 		{
// //			ReturnError(pXml,__FILE__,__LINE__,ERR_OPXML,"AddChildNode出错");
// 			return -1;
// 		}
// 		recnode->SetAttribute("编号",false,func.nFuncNo);
// 		recnode->SetAttribute("名称",false,func.sFuncName.c_str());
// 		recnode->SetAttribute("版本",false,func.dVersion);
// 		recnode->SetAttribute("优先级",false,func.nLevel);
// 		if (func.bRepealFlag)
// 		{
// 			recnode->SetAttribute("冲正标志",false,"要冲正",false);
// 		}
// 		else
// 		{
// 			recnode->SetAttribute("冲正标志",false,"无需冲正",false);
// 		}
// 		recnode->SetAttribute("作者",false,func.sAuthor);
// 		recnode->SetAttribute("交易/原子",false,"交易");
// 		recnode->SetAttribute("动态库名称",false,func.sDllName.c_str());
// 		bRes = m_pFuncMgr.m_pFuncTbl.Next(func);
// 		rec++;
// 	}
// 
// 	bRes = m_pFuncMgr.m_pTxUnitTbl.First(func);
// 	while (bRes)
// 	{
// 		sprintf(tmpchar,"REC%d",rec);
// 		CXmlNode *recnode = txnode->AddChildNode(tmpchar,false);
// 		if (recnode == NULL)
// 		{
// 			ReturnError(pXml,__FILE__,__LINE__,ERR_OPXML,"AddChildNode出错");
// 			return -1;
// 		}
// 		recnode->SetAttribute("编号",false,func.nFuncNo);
// 		recnode->SetAttribute("名称",false,func.sFuncName.c_str());
// 		recnode->SetAttribute("版本",false,func.dVersion);
// 		recnode->SetAttribute("优先级",false,func.nLevel);
// 		if (func.bRepealFlag)
// 		{
// 			recnode->SetAttribute("冲正标志",false,"要冲正",false);
// 		}
// 		else
// 		{
// 			recnode->SetAttribute("冲正标志",false,"无需冲正",false);
// 		}
// 		
// 		recnode->SetAttribute("作者",false,func.sAuthor);
// 		recnode->SetAttribute("交易/原子",false,"原子");
// 		recnode->SetAttribute("动态库名称",false,func.sDllName.c_str());
// 		bRes = m_pFuncMgr.m_pTxUnitTbl.Next(func);
// 		rec++;
// 	}
	return rec;
}

int CCallBu::GetTxInfo(PBPCCOMMSTRU data)
{
	char tmpchar[30];
	std::vector<S_TBL_FLOWTX> txinfo;
	GetAllTxInfo(txinfo);
// 	CXmlNode *txnode = pXml->AddNodeByPath("package/head/ResultSet",false,"交易列表",false,"",false);
// 	if (txnode != NULL)
// 	{
// //		ReturnError(pXml,__FILE__,__LINE__,ERR_OPXML,"SetNodeValueByPath出错");
// 		return -1;
// 	}
// 
// 	for (int i=0;i<txinfo.size();i++)
// 	{
// 		sprintf(tmpchar,"REC%d",i);
// 		CXmlNode *recnode = txnode->AddChildNode(tmpchar,false);
// 		if (recnode == NULL)
// 		{
// //			ReturnError(pXml,__FILE__,__LINE__,ERR_OPXML,"AddChildNode出错");
// 			return -1;
// 		}
// 		recnode->SetAttribute("交易码",false,txinfo[i].nFuncNo);
// 		recnode->SetAttribute("交易名称",false,txinfo[i].sFuncName.c_str());
// 	}
// //	ReturnSuccess(pXml,"查询成功");
	return 0;
}

int CCallBu::GetAllTxInfo(std::vector<S_TBL_FLOWTX> &txinfo)
{
	S_TBL_FLOWTX tx;
	S_TBL_FUNC   func;
	txinfo.clear();
	
	if (m_nFlowUse)
	{
		m_pFlow->GetFlowFunc(txinfo);
	}
	bool bRes = m_pFuncMgr.m_pFuncTbl.First(func);
	while (bRes)
	{
		tx.nFuncNo = func.nFuncNo;
		tx.sFuncName= func.sFuncName;
		tx.cPrio = func.nLevel;
		tx.cServiceFlag = 1;
		txinfo.push_back(tx);
		bRes = m_pFuncMgr.m_pFuncTbl.Next(func);
	}
	if (m_nTxUnit2Func)
	{
		bRes = m_pFuncMgr.m_pTxUnitTbl.First(func);
		while (bRes)
		{
			tx.nFuncNo = func.nFuncNo;
			tx.sFuncName= func.sFuncName;
			tx.cPrio = func.nLevel;
			tx.cServiceFlag = 1;
			txinfo.push_back(tx);
			bRes = m_pFuncMgr.m_pTxUnitTbl.Next(func);
		}
	}
	//将内部功能加入进去

	return txinfo.size();
}

void CCallBu::SetPara(CIPcLink *pBulink)
{
	m_pPcLink = pBulink;
	m_bIsSetPara = true;
	
}

CBF_Xml * CCallBu::GetXmlConfig()
{
	return &m_pConfXml;
}

std::string & CCallBu::GetMsg()
{
	return m_errMsg;
}

CBF_SimpleLog * CCallBu::GetLogClass()
{
	return &m_pLog;
}

void CCallBu::AnsErr(int errcode, const char *file, int fileline,const char *msg, PBPCCOMMSTRU data)
{
	data->sDBHead.cRaflag = 1;
	data->sDBHead.cZip = 0;
	data->sDBHead.cNextFlag = 0;
	data->sDBHead.nLen = 0;
	data->sBpcHead.nBpcLen = DREBHEADLEN;
	data->sDBHead.a_Ainfo.a_nRetCode = errcode;
	m_pLog.LogMp(LOG_ERROR,file,fileline,"出错返回[%d %s]",errcode,msg);
	m_pPcLink->AnsData(data);
}
