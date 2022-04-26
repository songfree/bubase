// ProcThread.cpp: implementation of the CProcThread class.
//
//////////////////////////////////////////////////////////////////////

#include "ProcThread.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
typedef CIProcBase *(*NEWPROCINSTANCE)();

#define  ERR_TRADEKERNEL_CATCH            41   //��̬�������쳣����
#define  ERR_TRADEKERNEL_NOTFUNC          42   //�޴˽���

CProcThread::CProcThread()
{
	m_bNeedConnectDb =false;
	m_bAutoCommit = false;
	m_pSqlCmd = NULL; //���ݿ�����
	m_pExchLink = NULL;//���������ӵ���
	m_pClientLink = NULL; //�ͻ������ӵ���
	m_pMemPool = NULL;//�ڴ滺���
	m_pMemDb = NULL;//�ڴ��
	m_pLog = NULL;
	m_pRcvQueue = NULL;
	m_bIsRunning = false;
	g_nSvrHostStatus = NULL;
}

CProcThread::~CProcThread()
{
	m_pSqlCmd = NULL; //���ݿ�����
	m_pExchLink = NULL;//���������ӵ���
	m_pClientLink = NULL; //�ͻ������ӵ���
	m_pMemPool = NULL;//�ڴ滺���
	m_pMemDb = NULL;//�ڴ��
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
		    //����rpcӦ��ص�   ��ΪrpcӦ��ʱ������requestid���ҵ�����Ϣ���ص�
			if (1 == m_sData.pData->sDBHead.cRaflag)
			{

			}
//			m_sData.pData->sDBHead.s_Sinfo.s_nGateIndex = m_nIndex;
			// songfree 20180510 modi ����b_Cinfo���洦���߳����
			m_sData.pData->sDBHead.b_Cinfo.b_cIndex = m_nIndex;
			
			func= m_pFuncList.Find(m_sData.pData->sDBHead.d_Dinfo.d_nServiceNo);
			if (func == NULL)
			{
				//Ӧ���ȥ���޴˽���
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"not found txcode[%d]",m_sData.pData->sDBHead.d_Dinfo.d_nServiceNo);
#else
				m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"��[%d]����",m_sData.pData->sDBHead.d_Dinfo.d_nServiceNo);
#endif
				if (m_sData.src >1) //����Ӧ�𣬶�ʱ��������
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
				//����Ӧ���ȥ
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"[%d] call execption",funcno);
#else
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"[%d]���׵���ҵ��̬�����",funcno);
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
			sprintf(errmsg,"�򿪶�̬��[%s]ʧ��",dllpathname);
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
			sprintf(errmsg,"�򿪶�̬��[%s]ʧ��",dllpathname);
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
			sprintf(errmsg,"�򿪶�̬��[%s]ʧ��",dllpathname);
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
			sprintf(errmsg,"�򿪶�̬��[%s]ʧ��[%s]",dllpathname,pdllerror);
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
	//��ҵ��̬������
	int nUseDb=0;
	if (m_pXml->GetNodeAttribute("package/head/business/database",false,"use",false,nUseDb) == NULL)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"no [package/head/business/database] node attribute use");
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ڵ�[package/head/business/database]������use������");
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
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ڵ�[package/head/business/database]������commit������");
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
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ڵ�[package/head/business/database/databaseserver]������");
#endif
			return false;
		}
		if (m_pXml->GetNodeValueByPath("package/head/business/database/databaseuser",false,dbuser,false) == NULL)
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xml file no [package/head/business/database/databaseuser] node");
#else
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ڵ�[package/head/business/database/databaseuser] ������");
#endif
			return false;
		}
		if (m_pXml->GetNodeValueByPath("package/head/business/database/userpasswd",false,dbpwd,true) == NULL)
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xml file no [package/head/business/database/userpasswd] node");
#else
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ڵ�[package/head/business/database/userpasswd]������");
#endif
			return false;
		}
		if (m_pXml->GetNodeValueByPath("package/head/business/database/passwdtype",false,dbpwdflag,false) == NULL)
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xml file no [package/head/business/database/passwdtype] node");
#else
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ڵ� [package/head/business/database/passwdtype] ������");
#endif
			return false;
		}
		if (m_pXml->GetNodeValueByPath("package/head/business/database/databasetype",false,dbtype,false) == NULL)
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xml file no [package/head/business/database/databasetype] node");
#else
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ڵ�[package/head/business/database/databasetype]������");
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
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ݿ�������ܳ���");
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
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ݿ�������ܳ��� %s",errmsg);
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
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ݿ�����ʵ����ʧ�ܣ�����ռ�ʧ��");
#endif
			return false;
		}
		if (!m_pSqlCmd->Connect(dbname,dbuser,tmpchar,atoi(dbtype.c_str())))
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"connect database error %d %s",m_pSqlCmd->m_dbconn.m_nErrCode,m_pSqlCmd->m_dbconn.m_sErrInfo.c_str());
#else
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"�������ݿ�� %d %s",m_pSqlCmd->m_dbconn.m_nErrCode,m_pSqlCmd->m_dbconn.m_sErrInfo.c_str());
#endif
			return false;
		}
	}
		
	//��ҵ��̬�⣬��ȡ������
	
	CXmlNode *dllnode =  m_pXml->GetNodeByPath("/package/head/business/DLL",false);
	if (dllnode == NULL)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xml file no [/package/head/business/DLL] node");
		printf("xml file no [/package/head/business/DLL] node! \n");
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XML������[/package/head/business/DLL]�ڵ�");
		printf("XML������[/package/head/business/DLL]�ڵ�! \n");
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
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XML������[/package/head/business/DLL/%s]�ڵ������method",tmpndoe->GetNodeName(false).c_str());
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
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XML������[/package/head/business/DLL/%s]�ڵ������dllpath",tmpndoe->GetNodeName(false).c_str());
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
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"��ҵ��̬��[%s]ʧ��",tmp.procdllname.c_str());
			printf("��ҵ��̬��[%s]ʧ��\n",tmp.procdllname.c_str());
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
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ҵ��̬��[%s]ʵ����ʧ��",tmp.procdllname.c_str());
			printf("ҵ��̬��[%s]ʵ����ʧ��\n",tmp.procdllname.c_str());
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
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ҵ��̬��[%s]ʵ����ʧ��",tmp.procdllname.c_str());
			printf("ҵ��̬��[%s]ʵ����ʧ��\n",tmp.procdllname.c_str());
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
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ҵ��̬��[%s]Initʧ��",tmp.procdllname.c_str());
				printf("ҵ��̬��[%s]Initʧ��\n",tmp.procdllname.c_str());
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
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"����ҵ��̬��%s��ʼ���쳣����",tmp.procdllname.c_str());
			printf("����ҵ��̬��[%s]��ʼ���쳣����\n",tmp.procdllname.c_str());
#endif
			return false;
		}
		m_alldllclass.push_back(tmp);
		//ȡ�ö�̬����Ľ�����
		S_TBL_PROCFUNC *tmpfunc =  tmp.iProcBase->m_pFuncTbl.First();
		while (tmpfunc != NULL)
		{
			tmpfunc->pProcInstance = tmp.iProcBase;
			S_TBL_PROCFUNC * tmpp = m_pFuncList.Find(tmpfunc->nFuncNo);
			if (tmpp == NULL)
			{
				m_pFuncList.Insert(*tmpfunc);
			}
			else  //���ظ��Ľ����룬�Ƚϰ汾���������µİ汾
			{
				if (tmpfunc->dVersion > tmpp->dVersion) //����
				{
#ifdef _ENGLISH_
					m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"library[%s] txcode [%d] than [%s]  new��update",\
						tmpfunc->sDllName,tmpfunc->nFuncNo,tmpp->sDllName);
#else
					m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"��̬��[%s]�еĽ�����[%d]��[%s]��̬���а汾�ߣ�����",\
						tmpfunc->sDllName,tmpfunc->nFuncNo,tmpp->sDllName);
#endif
					strcpy(tmpp->sFuncName,tmpfunc->sFuncName);
					tmpp->dVersion = tmpfunc->dVersion;
					strcpy(tmpp->sAuthor,tmpfunc->sAuthor);
					tmpp->nLevel = tmpfunc->nLevel;
					tmpp->pProcInstance = tmpfunc->pProcInstance;
					strcpy(tmpp->sDllName,tmpfunc->sDllName);
				}
				else //����
				{
#ifdef _ENGLISH_
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"library[%s] txcode [%d] than [%s]  old ",\
							tmpfunc->sDllName,tmpfunc->nFuncNo,tmpp->sDllName);
#else
					m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"��̬��[%s]�еĽ�����[%d]��[%s]��̬���а汾�ͣ�������",\
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
