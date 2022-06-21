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
	
	
	m_nAutoCommit=0;   //�Զ��ύ���ݿ�ѡ�� 1�Զ��ύ��������
	m_nAutoLoadDll=0;  //�Ƿ��Զ����ض�̬��
	m_nTimeTest=0;     //�Ƿ��¼����ʱ��
	m_nTxUnit2Func=0;  //�Ƿ��ԭ��ת��Ϊ����
	m_nTestLopBack=0;  //�Ƿ���Ի�·
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
			AnsErr(ERR_BPU_INIT,__FILE__,__LINE__,"��ʼ��BPU����",data);
			return -1;
		}
	}
	//�����ڲ�����
	if (data->sDBHead.d_Dinfo.d_nServiceNo > 999999000 && data->sDBHead.d_Dinfo.d_nServiceNo < 999999999)
	{
		try
		{
			ret = RunLocalFunc(data);
		}
		catch (...)
		{
			AnsErr(ERR_BPU_CATCH,__FILE__,__LINE__,"BPU�쳣����",data);
			return -1;
		}
		return 0;
	}


	//�Ƿ���Ի�·
	if (m_nTestLopBack == 1) //���Ի�·
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
		m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"���Է����ļ�[%s]",testfilename);
//		m_pPcLink->AnsData(data);
		FILE *fp = fopen(testfilename,"rb");
		if (fp == NULL)
		{
			AnsErr(ERR_BPU_NOTFUNC,__FILE__,__LINE__,"���Ի�·�ļ�������",data);
			return -1;
		}
		data->sDBHead.nLen = fread(data->sBuffer,1,BPUDATASIZE,fp);
		if (data->sDBHead.nLen<0)
		{
			AnsErr(ERR_BPU_NOTFUNC,__FILE__,__LINE__,"�����Ի�·�ļ�����",data);
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
	
	//���������еĽ���
	if (m_nFlowUse == 1)
	{
		if (m_pFlow->IsRunFlow(data->sDBHead.d_Dinfo.d_nServiceNo))
		{
			try
			{
				ret = m_pFlow->RunFlow(data);
				//�����Ƿ��ύ��RunFlow�������

			}
			catch (...)
			{
				AnsErr(ERR_BPU_CATCH,__FILE__,__LINE__,"�������̿����쳣����",data);
				return -1;
			}
		}
	}
	//���ж�̬���еĹ���
	S_TBL_FUNC funcinfo;
	if (!m_pFuncMgr.m_pFuncTbl.Select(data->sDBHead.d_Dinfo.d_nServiceNo,funcinfo))
	{
		if (m_nTxUnit2Func == 1) //ԭ��ת��Ϊ����
		{
			if (!m_pFuncMgr.m_pTxUnitTbl.Select(data->sDBHead.d_Dinfo.d_nServiceNo,funcinfo))
			{
				sprintf(m_cMsg,"�޴˽��׺�ԭ�ӽ��� %d",data->sDBHead.d_Dinfo.d_nServiceNo);
				AnsErr(ERR_BPU_NOTFUNC,__FILE__,__LINE__,m_cMsg,data);
				return -1;
			}
		}
		else
		{
			sprintf(m_cMsg,"�޴˽��� %d",data->sDBHead.d_Dinfo.d_nServiceNo);
			AnsErr(ERR_BPU_NOTFUNC,__FILE__,__LINE__,m_cMsg,data);
			return -1;
		}
	}
	try
	{
		if (m_bNeedConnectDb) //���������ݿ�����Ҫ�ж������ύ
		{
			if (m_nAutoCommit == 1) //�Զ��ύ���ݿ�
			{
				m_pSqlCmd->Commit(); 
			}
		}
		ret = (funcinfo.pBuInstance->*(funcinfo.pFunc))(data);
		if (m_bNeedConnectDb) //���������ݿ�����Ҫ�ж������ύ
		{
			if (m_nAutoCommit == 1) //�Զ��ύ���ݿ�
			{
				if (ret == 0)  //�ύ
				{
					m_pSqlCmd->Commit(); 
				}
				else           //�ع�
				{
					m_pSqlCmd->Rollback();
				}
			}
		}
	}
	catch (...)
	{
		AnsErr(ERR_BPU_CATCH,__FILE__,__LINE__,"���ý��׻�ԭ�ӹ����쳣����",data);
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
		m_errMsg = std::string("�ڵ�[package/head/public/loglevel]������");
		return false;
	}

	if (m_pConfXml.GetNodeValueByPath("package/head/public/logdir",false,sLogPath,false) == NULL)
	{
		m_errMsg = std::string("�ڵ�[package/head/public/logdir]������");
		return false;
	}

	if (m_pConfXml.GetNodeValueByPath("package/head/public/logfile",false,sLogFile,false) == NULL)
	{
		m_errMsg = std::string("�ڵ�[package/head/public/logfile]������");
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
		m_errMsg = std::string("�ڵ�[package/head/business/database]������use������");
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
			m_errMsg = std::string("�ڵ�[package/head/business/database]������commit������");
			m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg.c_str());
			return false;
		}
		if (m_pConfXml.GetNodeValueByPath("package/head/business/database/databaseserver",false,dbname,false) == NULL)
		{
			m_errMsg = std::string("�ڵ�[package/head/business/database/databaseserver]������");
			m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg.c_str());
			return false;
		}
		if (m_pConfXml.GetNodeValueByPath("package/head/business/database/databaseuser",false,dbuser,false) == NULL)
		{
			m_errMsg = std::string("�ڵ�[package/head/business/database/databaseuser]������");
			m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg.c_str());
			return false;
		}
		if (m_pConfXml.GetNodeValueByPath("package/head/business/database/userpasswd",false,dbpwd,true) == NULL)
		{
			m_errMsg = std::string("�ڵ�[package/head/business/database/userpasswd]������");
			m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg.c_str());
			return false;
		}
		if (m_pConfXml.GetNodeValueByPath("package/head/business/database/passwdtype",false,dbpwdflag,false) == NULL)
		{
			m_errMsg = std::string("�ڵ�[package/head/business/database/passwdtype]������");
			m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg.c_str());
			return false;
		}
		if (m_pConfXml.GetNodeValueByPath("package/head/business/database/databasetype",false,dbtype,false) == NULL)
		{
			m_errMsg = std::string("�ڵ�[package/head/business/database/databasetype]������");
			m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg.c_str());
			return false;
		}
		m_pLog.LogMp(LOG_INFO,__FILE__,__LINE__,"��ʼ�������ݿ�");

		strcpy(tmpchar,dbpwd.c_str());
		if (atoi(dbpwdflag.c_str()) ==1 )
		{
//			m_pEncrypt.unencryptchar(tmpchar,strlen(tmpchar),DBPWDKEY); 
			int len = strlen(tmpchar);
			if (m_pEncrypt.UncryptString(tmpchar,len,DBPWDKEY)<0)
			{
				m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"���ݿ�������ܳ���");
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
				m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"���ݿ�������ܳ��� %s",errmsg);
				return false;
			}
		}
		m_pSqlCmd = new CDbApi();
		if (m_pSqlCmd == NULL)
		{
			m_errMsg = std::string("���ݿ�����ʵ����ʧ�ܣ�����ռ�ʧ��");
			m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,m_errMsg.c_str());
			return false;
		}
		if (!m_pSqlCmd->Connect(dbname,dbuser,tmpchar,atoi(dbtype.c_str())))
		{
			m_errMsg = std::string("�������ݿ��:")+m_pSqlCmd->m_dbconn.m_sErrInfo;
			m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"�������ݿ�� %d %s",m_pSqlCmd->m_dbconn.m_nErrCode,m_pSqlCmd->m_dbconn.m_sErrInfo.c_str());
			return false;
		}
		m_pLog.LogMp(LOG_INFO,__FILE__,__LINE__,"�������ݿ�ɹ�");
	}
	else
	{
		m_pLog.LogMp(LOG_INFO,__FILE__,__LINE__,"δ����ʹ�����ݿ�");
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
		m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"���ò�ʹ�����̿���");
		return true;
	}
	if (!m_bNeedConnectDb)
	{
		m_errMsg = "���̿������������ݿ�,��δ�������ݿ�����";
		m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,m_errMsg.c_str());
		return false;
	}
	std::string sDllpath;
	std::string sDllname;
	std::string sMethod;
	
	if (m_pConfXml.GetNodeAttribute("package/head/business/flowcontrol",false,"dllpath",false,sDllpath,false) == NULL)
	{
		m_errMsg = std::string("�ڵ�[package/head/business/flowcontrol]������dllpath������");
		m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,m_errMsg.c_str());
		return false;
	}
	
	if (m_pConfXml.GetNodeAttribute("package/head/business/flowcontrol",false,"dllname",false,sDllname,false) == NULL)
	{
		m_errMsg = std::string("�ڵ�[package/head/business/flowcontrol]������dllname������");
		m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,m_errMsg.c_str());
		return false;
	}
	if (m_pConfXml.GetNodeAttribute("package/head/business/flowcontrol",false,"method",false,sMethod,false) == NULL)
	{
		m_errMsg = std::string("�ڵ�[package/head/business/flowcontrol]������method������");
		m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,m_errMsg.c_str());
		return false;
	}
	

	m_pFlow->SetBuPara(&m_pConfXml,m_pPcLink,m_pSqlCmd,&m_pLog);
	if (!m_pFlow->InitFlow())
	{
		m_errMsg = std::string("���̶�̬��")+sDllname+"��ʼ��ʧ��";
		m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,m_errMsg.c_str());
		return false;
	}
	return true;
}



bool CCallBu::InitAll()
{
	if (!m_pConfXml.FromFile(m_sConfigXmlFile.c_str()))
	{
		m_errMsg = std::string("��ʼ�������ļ�����:")+m_sConfigXmlFile;
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
	//��ʼ�����ݿ�
	if (!InitDatabase())
	{
		return false;
	}
	
	//��ʼ��BU
	if (!InitBu())
	{
		return false;
	}
	//��ʼ�����̿���
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
		case 999999001:  //��أ�ֱ�ӷ���
//			ReturnSuccess(pXml,"��������");
			break;
		case 999999002:  //ȡ�ļ�
			GetFile(data);
			break;
		case 999999003:  //�ϴ��ļ�
			PutFile(data);
			break;	
		case 999999004:  //��ȡҵ��̬����Ϣ
			GetDllInfo(data);
			break;
		case 999999005:  //��ȡ�ṩ��������н�����Ϣ
			GetTxInfo(data);
			break;
		default:
//			ReturnError(pXml,__FILE__,__LINE__,"970","�޴��ڲ�����");
			sprintf(m_cMsg,"�޴˽��׺�ԭ�ӽ��� %d",data->sDBHead.d_Dinfo.d_nServiceNo);
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

// 	if (pXml->GetNodeValueByPath("package/pack/�ļ���",false,pathfilename) == NULL)
// 	{
// 		ReturnError(pXml,__FILE__,__LINE__,ERR_INPUT,"ȡ�ļ�ʱ[package/pack/�ļ���]������");
// 		return -1;
// 	}
// 	if (pXml->GetNodeValueByPath("package/pack/ƫ����",false,offset) == NULL)
// 	{
// 		ReturnError(pXml,__FILE__,__LINE__,ERR_INPUT,"ȡ�ļ�ʱ[package/pack/ƫ����]������");
// 		return -1;
// 	}
// 	if (pXml->GetNodeValueByPath("package/pack/ÿ�δ�С",false,getlen) == NULL)
// 	{
// 		ReturnError(pXml,__FILE__,__LINE__,ERR_INPUT,"ȡ�ļ�ʱ[package/pack/ÿ�δ�С]������");
// 		return -1;
// 	}
// 	if (getlen <1 || getlen > 65536)
// 	{
// 		ReturnError(pXml,__FILE__,__LINE__,ERR_INPUT,"ȡ�ļ�ʱ[package/pack/ÿ�δ�С]�Ƿ�");
// 		return -1;
// 	}
// 	if (offset <1)
// 	{
// 		ReturnError(pXml,__FILE__,__LINE__,ERR_INPUT,"ȡ�ļ�ʱ[package/pack/ƫ����]�Ƿ�");
// 		return -1;
// 	}
// 	FSTAT(pathfilename.c_str(),&f_stat);
// 
// 	fp = fopen(pathfilename.c_str(),"rb");
// 	if (fp==NULL)
// 	{
// 		m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"�ļ��������� %s",(char *)pathfilename.c_str());
// 		ReturnError(pXml,__FILE__,__LINE__,ERR_OPENFILE,"ȡ�ļ�ʱ�ļ�������");
// 		return -1;
// 	}
// 	
// 	if (offset>=f_stat.st_size)
// 	{
// 		m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"������ƫ�������� %d %s",offset,(char *)pathfilename.c_str());
// 		ReturnError(pXml,__FILE__,__LINE__,ERR_INPUT,"������ƫ��������");
// 		fclose(fp);
// 		return -1;
// 	}
// 	fseek(fp,offset,1);
// 	databuf = (char *)malloc(getlen+1);
// 	if (databuf == NULL)
// 	{
// 		ReturnError(pXml,__FILE__,__LINE__,ERR_MALLOC,"ȡ�ļ�ʱ����ռ�ʧ��");
// 		fclose(fp);
// 		return -1;
// 	}
// 	ret = fread(databuf,1,getlen,fp);
// 	databuf[ret]=0;
// 	fclose(fp);
// 	if (ret +offset >= f_stat.st_size)
// 	{
// 		pXml->SetNodeAttribute("package/Response",false,"Next",false,"0",false);//�޺�����
// 	}
// 	else
// 	{
// 		pXml->SetNodeAttribute("package/Response",false,"Next",false,"1",false);//�к�����
// 	}
// 	pXml->SetNodeValueByPath("package/pack/ƫ����",false,offset+ret);
// 	pXml->SetNodeValueByPath("package/pack/�ļ���С",false,f_stat.st_size);
// 	pXml->SetCdata(databuf,ret);
// 	free(databuf);
// 	databuf = NULL;
// 	ReturnSuccess(pXml,"ȡ�ļ��������");
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
	
// 	if (pXml->GetNodeValueByPath("package/pack/�ļ���",false,pathfilename) == NULL)
// 	{
// 		ReturnError(pXml,__FILE__,__LINE__,ERR_INPUT,"���ļ�ʱ[package/pack/�ļ���]������");
// 		return -1;
// 	}
// 	if (pXml->GetNodeValueByPath("package/pack/�ļ���ʼ��־",false,filestartflag) == NULL)
// 	{
// 		ReturnError(pXml,__FILE__,__LINE__,ERR_INPUT,"���ļ�ʱ[package/pack/�ļ���ʼ��־]������");
// 		return -1;
// 	}
// 	if (pXml->GetNodeValueByPath("package/pack/�ļ�������־",false,fileendflag) == NULL)
// 	{
// 		ReturnError(pXml,__FILE__,__LINE__,ERR_INPUT,"���ļ�ʱ[package/pack/�ļ�������־]������");
// 		return -1;
// 	}
// 	if (pathfilename.length()<1)
// 	{
// 		ReturnError(pXml,__FILE__,__LINE__,ERR_INPUT,"���ļ�ʱ[package/pack/�ļ���]�Ƿ�");
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
// 		ReturnError(pXml,__FILE__,__LINE__,ERR_INPUT,"���ļ�ʱXML[package/Cdata]�Ƿ�");
// 		return -1;
// 	}
// 	datalen+=1;
// 	databuf = (char *)malloc(datalen);
// 	if (databuf == NULL)
// 	{
// 		ReturnError(pXml,__FILE__,__LINE__,ERR_MALLOC,"���ļ�ʱ����ռ�ʧ��");
// 		return -1;
// 	}
// 	if (!pXml->GetCData(databuf,datalen))
// 	{
// 		ReturnError(pXml,__FILE__,__LINE__,ERR_INPUT,"ȡCdataʧ��");
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
// 		ReturnError(pXml,__FILE__,__LINE__,ERR_OPENFILE,"���ļ�ʧ��");
// 		return -1;
// 	}
// 	fwrite(databuf,1,datalen,fp);
// 	fclose(fp);
// 	
// 	if (fileendflag == 1)
// 	{
// 		if (pXml->GetNodeValueByPath("package/pack/�ļ���С",false,filesize) == NULL)
// 		{
// 			ReturnError(pXml,__FILE__,__LINE__,ERR_INPUT,"���ļ�ʱ[package/pack/�ļ���С]������");
// 			return -1;
// 		}
// 		struct_stat f_stat;
// 		FSTAT(pathfilename.c_str(),&f_stat);
// 		if (f_stat.st_size != filesize)
// 		{
// 			ReturnError(pXml,__FILE__,__LINE__,ERR_WRITEFILE,"д���ļ������ļ���С����");
// 			return -1;
// 		}
// 		ReturnSuccess(pXml,"�ϴ��ļ��ɹ�");
// 		return 0;
// 	}
// 	ReturnSuccess(pXml,"д���ļ��ɹ�");
	return 0;
}

int CCallBu::GetDllInfo(PBPCCOMMSTRU data)
{
	char tmpchar[30];
	S_TBL_FUNC   func;
	int rec=0;
// 	CXmlNode *txnode = pXml->AddNodeByPath("package/head/ResultSet",false,"��̬����Ϣ",false,"",false);
// 	if (txnode != NULL)
// 	{
// //		ReturnError(pXml,__FILE__,__LINE__,ERR_OPXML,"SetNodeValueByPath����");
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
// //			ReturnError(pXml,__FILE__,__LINE__,ERR_OPXML,"AddChildNode����");
// 			return -1;
// 		}
// 		recnode->SetAttribute("���",false,func.nFuncNo);
// 		recnode->SetAttribute("����",false,func.sFuncName.c_str());
// 		recnode->SetAttribute("�汾",false,func.dVersion);
// 		recnode->SetAttribute("���ȼ�",false,func.nLevel);
// 		if (func.bRepealFlag)
// 		{
// 			recnode->SetAttribute("������־",false,"Ҫ����",false);
// 		}
// 		else
// 		{
// 			recnode->SetAttribute("������־",false,"�������",false);
// 		}
// 		recnode->SetAttribute("����",false,func.sAuthor);
// 		recnode->SetAttribute("����/ԭ��",false,"����");
// 		recnode->SetAttribute("��̬������",false,func.sDllName.c_str());
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
// 			ReturnError(pXml,__FILE__,__LINE__,ERR_OPXML,"AddChildNode����");
// 			return -1;
// 		}
// 		recnode->SetAttribute("���",false,func.nFuncNo);
// 		recnode->SetAttribute("����",false,func.sFuncName.c_str());
// 		recnode->SetAttribute("�汾",false,func.dVersion);
// 		recnode->SetAttribute("���ȼ�",false,func.nLevel);
// 		if (func.bRepealFlag)
// 		{
// 			recnode->SetAttribute("������־",false,"Ҫ����",false);
// 		}
// 		else
// 		{
// 			recnode->SetAttribute("������־",false,"�������",false);
// 		}
// 		
// 		recnode->SetAttribute("����",false,func.sAuthor);
// 		recnode->SetAttribute("����/ԭ��",false,"ԭ��");
// 		recnode->SetAttribute("��̬������",false,func.sDllName.c_str());
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
// 	CXmlNode *txnode = pXml->AddNodeByPath("package/head/ResultSet",false,"�����б�",false,"",false);
// 	if (txnode != NULL)
// 	{
// //		ReturnError(pXml,__FILE__,__LINE__,ERR_OPXML,"SetNodeValueByPath����");
// 		return -1;
// 	}
// 
// 	for (int i=0;i<txinfo.size();i++)
// 	{
// 		sprintf(tmpchar,"REC%d",i);
// 		CXmlNode *recnode = txnode->AddChildNode(tmpchar,false);
// 		if (recnode == NULL)
// 		{
// //			ReturnError(pXml,__FILE__,__LINE__,ERR_OPXML,"AddChildNode����");
// 			return -1;
// 		}
// 		recnode->SetAttribute("������",false,txinfo[i].nFuncNo);
// 		recnode->SetAttribute("��������",false,txinfo[i].sFuncName.c_str());
// 	}
// //	ReturnSuccess(pXml,"��ѯ�ɹ�");
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
	//���ڲ����ܼ����ȥ

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
	m_pLog.LogMp(LOG_ERROR,file,fileline,"������[%d %s]",errcode,msg);
	m_pPcLink->AnsData(data);
}
