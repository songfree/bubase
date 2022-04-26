// ProcThread.cpp: implementation of the CProcThread class.
//
//////////////////////////////////////////////////////////////////////

#include "ProcThread.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
typedef CIProcBase *(*NEWPROCINSTANCE)();

#define  ERR_TRADEKERNEL_CATCH            41   //动态库运行异常出错
#define  ERR_TRADEKERNEL_NOTFUNC          42   //无此交易

CProcThread::CProcThread()
{
	m_bNeedConnectDb =false;
	m_bAutoCommit = false;
	m_pSqlCmd = NULL; //数据库连接
	m_pExchLink = NULL;//交易所连接调用
	m_pClientLink = NULL; //客户端连接调用
	m_pMemPool = NULL;//内存缓冲池
	m_pMemDb = NULL;//内存表
	m_pLog = NULL;
	m_pRcvQueue = NULL;
	m_bIsRunning = false;
	g_nSvrHostStatus = NULL;
}

CProcThread::~CProcThread()
{
	m_pSqlCmd = NULL; //数据库连接
	m_pExchLink = NULL;//交易所连接调用
	m_pClientLink = NULL; //客户端连接调用
	m_pMemPool = NULL;//内存缓冲池
	m_pMemDb = NULL;//内存表
	m_pLog = NULL;
	g_nSvrHostStatus = NULL;
	for (int i=0; i<m_alldllclass.size(); i++)
	{
		if (m_alldllclass[i].iProcBase != NULL)
		{
			delete m_alldllclass[i].iProcBase;
			m_alldllclass[i].iProcBase = NULL;
			FREEDLL(m_alldllclass[i].procdllhandle);
			m_alldllclass[i].procdllhandle = NULL;
		}
	}
}
int CProcThread::Run()
{
	int ret;
	S_TBL_PROCFUNC *func=NULL;
	int funcno;
	while (m_bIsRunning)
	{
		if (m_pRcvQueue->GetData(m_sData))
		{
		    //增加rpc应答回调   当为rpc应答时，根据requestid查找调用信息，回调
			if (1 == m_sData.pData->sDBHead.cRaflag)
			{

			}
//			m_sData.pData->sDBHead.s_Sinfo.s_nGateIndex = m_nIndex;
			// songfree 20180510 modi 改用b_Cinfo保存处理线程序号
			m_sData.pData->sDBHead.b_Cinfo.b_cIndex = m_nIndex;
			
			func= m_pFuncList.Find(m_sData.pData->sDBHead.d_Dinfo.d_nServiceNo);
			if (func == NULL)
			{
				//应答回去，无此交易
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"not found txcode[%d]",m_sData.pData->sDBHead.d_Dinfo.d_nServiceNo);
#else
				m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"无[%d]交易",m_sData.pData->sDBHead.d_Dinfo.d_nServiceNo);
#endif
				if (m_sData.src >1) //不用应答，定时器发出的
				{
					m_pMemPool->PoolFree(m_sData.pData);
					m_sData.pData = NULL;
					continue;
				}
				m_sData.pData->sDBHead.a_Ainfo.a_nRetCode = ERR_TRADEKERNEL_NOTFUNC;
				m_sData.pData->sDBHead.cRaflag = 1;
				m_pClientLink->Send2Client(m_sData);
				continue;
			}
			try
			{
				if (m_bNeedConnectDb)
				{
					if (m_bAutoCommit)
					{
						m_pSqlCmd->Commit();
					}
				}
				funcno = m_sData.pData->sDBHead.d_Dinfo.d_nServiceNo;
				ret = (func->pProcInstance->*(func->pFunc))(m_sData);
				if (m_bNeedConnectDb)
				{
					if (m_bAutoCommit)
					{
						if (ret !=0)
						{
							m_pSqlCmd->Rollback();
						}
						else
						{
							m_pSqlCmd->Commit();
						}
					}
				}
			}
			catch (...)
			{
				//出错应答回去
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"[%d] call execption",funcno);
#else
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"[%d]交易调用业务动态库出错",funcno);
#endif
				if (m_sData.pData != NULL)
				{
					m_sData.pData->sDBHead.a_Ainfo.a_nRetCode = ERR_TRADEKERNEL_CATCH;
					m_sData.pData->sDBHead.cRaflag = 1;
					m_pClientLink->Send2Client(m_sData);
				}
				continue;
			}
		}
	}
	return 0;
}
bool CProcThread::OpenDll(DLLHANDLE *pDll, const char *dllname, const char *dllpath)
{
	char errmsg[300];
	bool bIsNotPath=false;
	char dllpathname[401];
	memset(dllpathname,0,sizeof(dllpathname));
	if (strlen(dllpath) <1)
	{
		bIsNotPath=true;
	}
	else
	{
		bIsNotPath=false;
	}
	if (bIsNotPath)
	{
		sprintf(dllpathname,"%s",dllname);
	}
	else
	{
#ifdef _WINDOWS
		sprintf(dllpathname,"%s\\%s",dllpath,dllname);
#else
		sprintf(dllpathname,"%s/%s",dllpath,dllname);
#endif
	}
	
	CBF_Tools::LRtrim(dllpathname);
	
	*pDll = LOADDLL(dllpathname);
#ifdef WIN32
	if (ISNOTOPEN(*pDll))
	{
		if (bIsNotPath)
		{
#ifdef _ENGLISH_
			sprintf(errmsg,"open library[%s] error",dllpathname);
#else
			sprintf(errmsg,"打开动态库[%s]失败",dllpathname);
#endif
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,errmsg);
			return false;
		}
		sprintf(dllpathname,"%s",dllname);
		CBF_Tools::LRtrim(dllpathname);
		*pDll = LOADDLL(dllpathname);
		if (ISNOTOPEN(*pDll))
		{
#ifdef _ENGLISH_
			sprintf(errmsg,"open library[%s] error",dllpathname);
#else
			sprintf(errmsg,"打开动态库[%s]失败",dllpathname);
#endif
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,errmsg);
			return false;
		}
	}
#else
	char *pdllerror;
	pdllerror = dlerror();
	if (pdllerror!=NULL)
	{	
		if (bIsNotPath)
		{
#ifdef _ENGLISH_
			sprintf(errmsg,"open library[%s] error",dllpathname);
#else
			sprintf(errmsg,"打开动态库[%s]失败",dllpathname);
#endif
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,errmsg);
			return false;
		}
		sprintf(dllpathname,"%s",dllname);
		CBF_Tools::LRtrim(dllpathname);
		*pDll = LOADDLL(dllpathname);
		pdllerror = dlerror();
		if (pdllerror!=NULL)
		{
#ifdef _ENGLISH_
			sprintf(errmsg,"open library[%s] error:%s",dllpathname,pdllerror);
#else
			sprintf(errmsg,"打开动态库[%s]失败[%s]",dllpathname,pdllerror);
#endif
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,errmsg);
			return false;
		}
		
	}
#endif	
	return true;
}


bool CProcThread::Init(CBF_Xml *xml)
{
	m_pXml = xml;
	//读业务动态库配置
	int nUseDb=0;
	if (m_pXml->GetNodeAttribute("package/head/business/database",false,"use",false,nUseDb) == NULL)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"no [package/head/business/database] node attribute use");
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"节点[package/head/business/database]的属性use不存在");
#endif
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
	if (m_pXml->GetNodeAttribute("package/head/business/database",false,"commit",false,nUseDb) == NULL)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"no [package/head/business/database] node attribute commit");
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"节点[package/head/business/database]的属性commit不存在");
#endif
		return false;
	}
	if (nUseDb == 1)
	{
		m_bAutoCommit = true;
	}
	else
	{
		m_bAutoCommit = false;
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
		if (m_pXml->GetNodeValueByPath("package/head/business/database/databaseserver",false,dbname,false) == NULL)
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xml file no [package/head/business/database/databaseserver] node");
#else
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"节点[package/head/business/database/databaseserver]不存在");
#endif
			return false;
		}
		if (m_pXml->GetNodeValueByPath("package/head/business/database/databaseuser",false,dbuser,false) == NULL)
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xml file no [package/head/business/database/databaseuser] node");
#else
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"节点[package/head/business/database/databaseuser] 不存在");
#endif
			return false;
		}
		if (m_pXml->GetNodeValueByPath("package/head/business/database/userpasswd",false,dbpwd,true) == NULL)
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xml file no [package/head/business/database/userpasswd] node");
#else
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"节点[package/head/business/database/userpasswd]不存在");
#endif
			return false;
		}
		if (m_pXml->GetNodeValueByPath("package/head/business/database/passwdtype",false,dbpwdflag,false) == NULL)
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xml file no [package/head/business/database/passwdtype] node");
#else
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"节点 [package/head/business/database/passwdtype] 不存在");
#endif
			return false;
		}
		if (m_pXml->GetNodeValueByPath("package/head/business/database/databasetype",false,dbtype,false) == NULL)
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xml file no [package/head/business/database/databasetype] node");
#else
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"节点[package/head/business/database/databasetype]不存在");
#endif
			return false;
		}

		strcpy(tmpchar,dbpwd.c_str());
		if (atoi(dbpwdflag.c_str()) ==1 )
		{
			int len = strlen(tmpchar);
			if (m_pEncrypt.UncryptString(tmpchar,len,DBPWDKEY)<0)
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"database user password uncrypt error");
#else
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"数据库密码解密出错");
#endif
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
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"database user password uncrypt error %s",errmsg);
#else
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"数据库密码解密出错 %s",errmsg);
#endif
				return false;
			}
		}
		m_pSqlCmd = new CDbApi();
		if (m_pSqlCmd == NULL)
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"new CDbApi error");
#else
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库连接实例化失败，分配空间失败");
#endif
			return false;
		}
		if (!m_pSqlCmd->Connect(dbname,dbuser,tmpchar,atoi(dbtype.c_str())))
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"connect database error %d %s",m_pSqlCmd->m_dbconn.m_nErrCode,m_pSqlCmd->m_dbconn.m_sErrInfo.c_str());
#else
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"连接数据库错 %d %s",m_pSqlCmd->m_dbconn.m_nErrCode,m_pSqlCmd->m_dbconn.m_sErrInfo.c_str());
#endif
			return false;
		}
	}
		
	//打开业务动态库，读取交易码
	
	CXmlNode *dllnode =  m_pXml->GetNodeByPath("/package/head/business/DLL",false);
	if (dllnode == NULL)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xml file no [/package/head/business/DLL] node");
		printf("xml file no [/package/head/business/DLL] node! \n");
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XML配置无[/package/head/business/DLL]节点");
		printf("XML配置无[/package/head/business/DLL]节点! \n");
#endif
		return false;
	}
	S_PROCBASE_DLL tmp;
	CXmlNode *tmpndoe = (CXmlNode *)dllnode->GetFirstChild();
	while (tmpndoe != NULL)
	{
		tmp.procdllname = tmpndoe->GetNodeValue(false);
		CXmlAttribute *attr = tmpndoe->GetAttribute("method",false);
		if (attr == NULL)
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xml file no [/package/head/business/DLL/%s] node attribute method",tmpndoe->GetNodeName(false).c_str());
#else
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XML配置无[/package/head/business/DLL/%s]节点的属性method",tmpndoe->GetNodeName(false).c_str());
#endif
			return false;
		}
		tmp.procInstance = attr->GetAttrValue(false);
		attr = tmpndoe->GetAttribute("dllpath",false);
		if (attr == NULL)
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xml file no [/package/head/business/DLL/%s] node attribute dllpath",tmpndoe->GetNodeName(false).c_str());
#else
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XML配置无[/package/head/business/DLL/%s]节点的属性dllpath",tmpndoe->GetNodeName(false).c_str());
#endif
			return false;
		}
		tmp.procdllpath = attr->GetAttrValue();
		if (!OpenDll(&(tmp.procdllhandle),tmp.procdllname.c_str(),tmp.procdllpath.c_str()))
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"open library [%s] error",tmp.procdllname.c_str());
			printf("open library [%s] error\n",tmp.procdllname.c_str());
#else
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"打开业务动态库[%s]失败",tmp.procdllname.c_str());
			printf("打开业务动态库[%s]失败\n",tmp.procdllname.c_str());
#endif
			return false;
		}
		NEWPROCINSTANCE baseinstance = (NEWPROCINSTANCE)GETINSTANCE(tmp.procdllhandle,tmp.procInstance.c_str());
		if (NULL == baseinstance)
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"library [%s] instance error",tmp.procdllname.c_str());
			printf("library [%s] instance error\n",tmp.procdllname.c_str());
#else
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"业务动态库[%s]实例化失败",tmp.procdllname.c_str());
			printf("业务动态库[%s]实例化失败\n",tmp.procdllname.c_str());
#endif
			return false;
		}
		tmp.iProcBase =(CIProcBase *)baseinstance(); 
		if (NULL == tmp.iProcBase )
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"library [%s] instance error",tmp.procdllname.c_str());
			printf("library [%s] instance error\n",tmp.procdllname.c_str());
#else
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"业务动态库[%s]实例化失败",tmp.procdllname.c_str());
			printf("业务动态库[%s]实例化失败\n",tmp.procdllname.c_str());
#endif
			return false;
		}
		
		try
		{
// 			tmp.iProcBase->m_pClientLink = m_pClientLink;
// 			tmp.iProcBase->m_pExchLink = m_pExchLink;
// 			tmp.iProcBase->m_pMemDb = m_pMemDb;
//			tmp.iProcBase->m_pMemPool = m_pMemPool;
// 			tmp.iProcBase->m_pSqlCmd = m_pSqlCmd;
// 			tmp.iProcBase->m_pXdp = &m_pXdp;
//			tmp.iProcBase->m_pLog = m_pLog;
//			tmp.iProcBase->m_nIndex = m_nIndex;
			tmp.iProcBase->g_nSvrHostStatus = g_nSvrHostStatus;
			tmp.iProcBase->g_pPubData = g_pPubData;
			if (!tmp.iProcBase->Init(m_pXml,m_pClientLink,m_pMemDb,&m_pXdp,m_pLog,m_pMemPool,m_pSqlCmd,m_pExchLink,m_nIndex))
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"library [%s] init error",tmp.procdllname.c_str());
				printf("library [%s] init error\n",tmp.procdllname.c_str());
#else
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"业务动态库[%s]Init失败",tmp.procdllname.c_str());
				printf("业务动态库[%s]Init失败\n",tmp.procdllname.c_str());
#endif
				return false;
			}
		}
		catch (...)
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"library [%s] init execption",tmp.procdllname.c_str());
			printf("library [%s] init execption\n",tmp.procdllname.c_str());
#else
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"调用业务动态库%s初始化异常出错",tmp.procdllname.c_str());
			printf("调用业务动态库[%s]初始化异常出错\n",tmp.procdllname.c_str());
#endif
			return false;
		}
		m_alldllclass.push_back(tmp);
		//取得动态库里的交易码
		S_TBL_PROCFUNC *tmpfunc =  tmp.iProcBase->m_pFuncTbl.First();
		while (tmpfunc != NULL)
		{
			tmpfunc->pProcInstance = tmp.iProcBase;
			S_TBL_PROCFUNC * tmpp = m_pFuncList.Find(tmpfunc->nFuncNo);
			if (tmpp == NULL)
			{
				m_pFuncList.Insert(*tmpfunc);
			}
			else  //有重复的交易码，比较版本，保留最新的版本
			{
				if (tmpfunc->dVersion > tmpp->dVersion) //更新
				{
#ifdef _ENGLISH_
					m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"library[%s] txcode [%d] than [%s]  new，update",\
						tmpfunc->sDllName,tmpfunc->nFuncNo,tmpp->sDllName);
#else
					m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"动态库[%s]中的交易码[%d]比[%s]动态库中版本高，更新",\
						tmpfunc->sDllName,tmpfunc->nFuncNo,tmpp->sDllName);
#endif
					strcpy(tmpp->sFuncName,tmpfunc->sFuncName);
					tmpp->dVersion = tmpfunc->dVersion;
					strcpy(tmpp->sAuthor,tmpfunc->sAuthor);
					tmpp->nLevel = tmpfunc->nLevel;
					tmpp->pProcInstance = tmpfunc->pProcInstance;
					strcpy(tmpp->sDllName,tmpfunc->sDllName);
				}
				else //不变
				{
#ifdef _ENGLISH_
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"library[%s] txcode [%d] than [%s]  old ",\
							tmpfunc->sDllName,tmpfunc->nFuncNo,tmpp->sDllName);
#else
					m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"动态库[%s]中的交易码[%d]比[%s]动态库中版本低，不更新",\
						tmpfunc->sDllName,tmpfunc->nFuncNo,tmpp->sDllName);
#endif
				}
			}
			tmpfunc = tmp.iProcBase->m_pFuncTbl.Next();
		}
		tmpndoe = (CXmlNode *)tmpndoe->getNextSibling();
	}
	return true;
}

bool CProcThread::Start()
{
	m_bIsRunning = true;
	return CreateThread();
}

void CProcThread::Stop()
{
	m_bIsRunning = false;
	
}
