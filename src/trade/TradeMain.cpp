// ArbMain.cpp: implementation of the CArbMain class.
//
//////////////////////////////////////////////////////////////////////

#include "TradeMain.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern int   g_pRunFlag;

typedef CTradeMain *(*NEWOFFERINGNSTANCE)();

CTradeMain::CTradeMain()
{
	m_pLog = NULL;
	m_bIsInit = false;
	m_sExchDll.exchPoint = NULL;
	m_sInitDll.initPoint = NULL;
	for (int i=0; i<50;i++)
	{
		m_lProcthread[i] = NULL;
	}
	
}

CTradeMain::~CTradeMain()
{
	
	if (m_sInitDll.initPoint != NULL)
	{
		delete m_sInitDll.initPoint;
		m_sInitDll.initPoint = NULL;
		FREEDLL(m_sInitDll.initdllhandle);
		m_sInitDll.initdllhandle = NULL;
	}
	if (m_sExchDll.exchPoint != NULL)
	{
		delete m_sExchDll.exchPoint;
		m_sExchDll.exchPoint = NULL;
		FREEDLL(m_sExchDll.exchdllhandle);
		m_sExchDll.exchdllhandle = NULL;
	}
	for (int i=0 ; i< 50; i++)
	{
		if (m_lProcthread[i]!= NULL)
		{
			delete m_lProcthread[i];
			m_lProcthread[i] = NULL;
		}
	}
	g_pPubData.WriteToFile(m_sSerialDataFile);
}

bool CTradeMain::Init(const char *confile)
{
	m_bIsInit = false;
	m_pRes.g_nStatus = 0;
	//���ó�ʼ
	if (!m_pRes.Init(confile))
	{
#ifdef _ENGLISH_
		printf("Initial error  %s\n",m_pRes.m_errMsg.c_str());
#else
		printf("��ʼ������  %s\n",m_pRes.m_errMsg.c_str());
#endif
		return false;
	}
	char errmsg[250];
	if (!m_pXdp.InitXdp(m_pRes.m_sXdpDefineXml.c_str(),errmsg))
	{
#ifdef _ENGLISH_
		printf("Initial xdp file %s error  %s\n",m_pRes.m_sXdpDefineXml.c_str(),errmsg);
#else
		printf("��ʼ��XDP����%s������%s�ļ�������\n",errmsg,m_pRes.m_sXdpDefineXml.c_str());
#endif
		return false;
	}
	m_pRcvQueue.m_pRes = &m_pRes;

	//api��ʼ��
	if (!m_pDrebApi.Init(&m_pRes))
	{
#ifdef _ENGLISH_
		printf("drebapi initial error! \n");
#else
		printf("�������߳�ʼ������! \n");
#endif
		return false;
	}
	//�ڴ��ָ��
	m_pListenThread.m_pMemPool = m_pDrebApi.GetBufferPool();

	m_pLog = m_pDrebApi.GetLogPoint();
	m_pRes.m_log = m_pLog;
	//��ʼ�߳�
	
	m_pDrebMsgThread.m_pRcvQueue = &m_pRcvQueue;
	m_pDrebMsgThread.m_pAnsProc = this;
	

	if (!m_pDrebMsgThread.Init(&m_pDrebApi,&m_pRes))
	{
#ifdef _ENGLISH_
		printf("drebapi msgthread initial error! \n");
#else
		printf("��Ϣ�����̳߳�ʼ������! \n");
#endif
		return false;
	}
	//���ó�ʼ���ݶ�̬�⣬�������ݳ�ʼ��
	CXmlNode *initnode =  m_pRes.m_pXml.GetNodeByPath("/package/head/initial/DLLNAME",false);
	if (initnode == NULL)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xml file no [/package/head/initial/DLLNAME] node");
		printf("xml file no [/package/head/initial/DLLNAME] node! \n");
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XML������[/package/head/initial/DLLNAME]�ڵ�");
		printf("XML������[/package/head/initial/DLLNAME]�ڵ�! \n");
#endif
		return false;
	}

	m_sInitDll.initdllname = initnode->GetNodeValue(false);
	CXmlAttribute *attr = initnode->GetAttribute("method",false);
	if (attr == NULL)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xml file no[/package/head/initial/DLLNAME]node attribute method");
		printf("xml file no [/package/head/initial/DLLNAME]node attribute method! \n");
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XML������[/package/head/initial/DLLNAME]�ڵ������method");
		printf("XML������[/package/head/initial/DLLNAME]�ڵ������method! \n");
#endif
		return false;
	}
	m_sInitDll.initInstance = attr->GetAttrValue(false);
	attr = initnode->GetAttribute("dllpath",false);
	if (attr == NULL)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xml file no[/package/head/initial/DLLNAME]node attribute dllpath");
		printf("XML������[/package/head/initial/DLLNAME]�ڵ������dllpath! \n");
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XML������[/package/head/initial/DLLNAME]�ڵ������dllpath");
		printf("XML������[/package/head/initial/DLLNAME]�ڵ������dllpath! \n");
#endif
		return false;
	}
	m_sInitDll.initdllpath = attr->GetAttrValue();
	if (!OpenDll(&(m_sInitDll.initdllhandle),m_sInitDll.initdllname.c_str(),m_sInitDll.initdllpath.c_str()))
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"open library [%s] error",m_sInitDll.initdllname.c_str());
		printf("open library [%s] error\n",m_sInitDll.initdllname.c_str());
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�򿪳�ʼ����̬��[%s]ʧ��",m_sInitDll.initdllname.c_str());
		printf("�򿪳�ʼ����̬��[%s]ʧ��\n",m_sInitDll.initdllname.c_str());
#endif
		return false;
	}
	NEWOFFERINGNSTANCE baseinstance = (NEWOFFERINGNSTANCE)GETINSTANCE(m_sInitDll.initdllhandle,m_sInitDll.initInstance.c_str());
	if (NULL == baseinstance)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"library [%s] instance error",m_sInitDll.initdllname.c_str());
		printf("library [%s] instance error\n",m_sInitDll.initdllname.c_str());
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"��ʼ����̬��[%s]ʵ����ʧ��",m_sInitDll.initdllname.c_str());
		printf("��ʼ����̬��[%s]ʵ����ʧ��\n",m_sInitDll.initdllname.c_str());
#endif
		return false;
	}
	m_sInitDll.initPoint =(CIInitial *)baseinstance(); 
	if (NULL == m_sInitDll.initPoint)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"library [%s] instance error",m_sInitDll.initdllname.c_str());
		printf("library [%s] instance error\n",m_sInitDll.initdllname.c_str());
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"��ʼ����̬��[%s]ʵ����ʧ��",m_sInitDll.initdllname.c_str());
		printf("��ʼ����̬��[%s]ʵ����ʧ��\n",m_sInitDll.initdllname.c_str());
#endif
		return false;
	}
	try
	{
		if (!m_sInitDll.initPoint->Init(m_pLog,&(m_pRes.m_pXml),&m_pMemDb))
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"library [%s] init error",m_sInitDll.initdllname.c_str());
			printf("library [%s] init error\n",m_sInitDll.initdllname.c_str());
#else
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"��ʼ����̬��[%s]Initʧ��",m_sInitDll.initdllname.c_str());
			printf("��ʼ����̬��[%s]Initʧ��\n",m_sInitDll.initdllname.c_str());
#endif
			return false;
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"library[%s] init exception",m_sInitDll.initdllname.c_str());
		printf("library[%s] init exception\n",m_sInitDll.initdllname.c_str());
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ó�ʼ����̬��%s��ʼ���쳣����",m_sInitDll.initdllname.c_str());
		printf("���ó�ʼ����̬��[%s]��ʼ���쳣����\n",m_sInitDll.initdllname.c_str());
#endif
		return false;
	}
#ifdef _WINDOWS
	sprintf(m_sSerialDataFile,"%s\\Serial.dat",m_pRes.g_sCurPath);
#else
	sprintf(m_sSerialDataFile,"%s/Serial.dat",m_pRes.g_sCurPath);
#endif
	//���ļ�ȡ��ˮ
	if (!g_pPubData.InitFromFile(m_sSerialDataFile))
	{
		m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"��ˮ�ļ�Serial.dat�����ڻ���Ȩ��");
	}
	//��ȡ�����������󱨵��ţ����±���
	S_MDB_ORDER_INFO *po = m_pMemDb.m_pDbTrader.m_pOrder.KeyFirst();
	int serial=0;
	while (po != NULL)
	{
		if (strlen(po->sLocalSerial) > 9 || strcmp(po->sOrderChannel,TERM_TYPE_SYSTEM) == 0 ) //���ر����ų��ȴ���9��������Ϊϵͳ�ģ���ʾ���Ǳ�ϵͳ�����
		{
			po = m_pMemDb.m_pDbTrader.m_pOrder.KeyNext();
		}
		else
		{
			if (serial < atoi(po->sLocalSerial))
			{
				serial =atoi(po->sLocalSerial);
			}
			po = m_pMemDb.m_pDbTrader.m_pOrder.KeyNext();
		}
	}
	g_pPubData.UpdateLocalSerial(serial); //���±��ر�����

	//��������̬�⣬���г�ʼ�� �����ﶯ̬�������ֱ�����ߡ�����̨��ֱ��������
	CXmlNode *exchnode =  m_pRes.m_pXml.GetNodeByPath("/package/head/exchange/DLLNAME",false);
	if (exchnode == NULL)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xml file no [/package/head/exchange/DLLNAME]node");
		printf("xml file no [/package/head/exchange/DLLNAME] node! \n");
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XML������[/package/head/exchange/DLLNAME]�ڵ�");
		printf("XML������[/package/head/exchange/DLLNAME]�ڵ�! \n");
#endif
		return false;
	}
	
	m_sExchDll.exchdllname = exchnode->GetNodeValue(false);
	attr = exchnode->GetAttribute("method",false);
	if (attr == NULL)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xml file no[/package/head/exchange/DLLNAME]node attribute method");
		printf("xml file no [/package/head/exchange/DLLNAME] node attribute method! \n");
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XML������[/package/head/exchange/DLLNAME]�ڵ������method");
		printf("XML������[/package/head/exchange/DLLNAME]�ڵ������method! \n");
#endif
		return false;
	}
	m_sExchDll.exchInstance = attr->GetAttrValue(false);
	attr = exchnode->GetAttribute("dllpath",false);
	if (attr == NULL)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xml file no[/package/head/exchange/DLLNAME]node attribute dllpath");
		printf("xml file no [/package/head/exchange/DLLNAME] node attribute dllpath! \n");
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XML������[/package/head/exchange/DLLNAME]�ڵ������dllpath");
		printf("XML������[/package/head/exchange/DLLNAME]�ڵ������dllpath! \n");
#endif
		return false;
	}
	m_sExchDll.exchdllpath = attr->GetAttrValue();
	if (!OpenDll(&(m_sExchDll.exchdllhandle),m_sExchDll.exchdllname.c_str(),m_sExchDll.exchdllpath.c_str()))
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"open exch library [%s] error",m_sExchDll.exchdllname.c_str());
		printf("open exch library[%s] error\n", m_sExchDll.exchdllname.c_str());
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�򿪽�������̬��[%s]ʧ��",m_sExchDll.exchdllname.c_str());
		printf("�򿪽�������̬��[%s]ʧ��\n", m_sExchDll.exchdllname.c_str());
#endif
		return false;
	}
	baseinstance = (NEWOFFERINGNSTANCE)GETINSTANCE(m_sExchDll.exchdllhandle,m_sExchDll.exchInstance.c_str());
	if (NULL == baseinstance)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"library [%s] instance error",m_sExchDll.exchdllname.c_str());
		printf("library[%s] instance error\n",m_sInitDll.initdllname.c_str());
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"��������̬��[%s]ʵ����ʧ��",m_sExchDll.exchdllname.c_str());
		printf("��������̬��[%s]ʵ����ʧ��\n",m_sExchDll.exchdllname.c_str());
#endif
		return false;
	}
	m_sExchDll.exchPoint =(CIExchLink *)baseinstance(); 
	if (NULL == m_sExchDll.exchPoint)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"library [%s] instance error",m_sExchDll.exchdllname.c_str());
		printf("library[%s] instance error\n",m_sInitDll.initdllname.c_str());
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"��������̬��[%s]ʵ����ʧ��",m_sExchDll.exchdllname.c_str());
		printf("��������̬��[%s]ʵ����ʧ��\n",m_sExchDll.exchdllname.c_str());
#endif
		return false;
	}
	try
	{
// 		m_sExchDll.exchPoint->m_pXdp.XdpCopy(&m_pXdp);
// 		m_sExchDll.exchPoint->m_pBusLink = this;
// 		m_sExchDll.exchPoint->m_pMemDb = &m_pMemDb;
// 		m_sExchDll.exchPoint->m_pLog = m_pLog;
// 		m_sExchDll.exchPoint->m_pMemPool = m_pListenThread.m_pMemPool;
		if (!m_sExchDll.exchPoint->Init(&(m_pRes.m_pXml),this,&m_pMemDb,&m_pXdp,m_pLog,m_pListenThread.m_pMemPool))
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"library [%s] Init error",m_sExchDll.exchdllname.c_str());
			printf("library[%s] Init error\n",m_sInitDll.initdllname.c_str());
#else
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"��������̬��[%s]Initʧ��",m_sExchDll.exchdllname.c_str());
			printf("��������̬��[%s]Initʧ��\n",m_sExchDll.exchdllname.c_str());
#endif
			return false;
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"library [%s] Init exception",m_sExchDll.exchdllname.c_str());
		printf("library[%s] Init exception\n",m_sInitDll.initdllname.c_str());
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ý�������̬��%s��ʼ���쳣����",m_sExchDll.exchdllname.c_str());
		printf("���ý�������̬��[%s]��ʼ���쳣����\n",m_sExchDll.exchdllname.c_str());
#endif
		return false;
	}
	
	int i;
	CProcThread *tmp=NULL;
	//��ʼ������̬��
	for (i=0 ; i<m_pRes.g_nProcTheadNum ; i++)
	{
		m_lProcthread[i] = new CProcThread();
		tmp = m_lProcthread[i];
		tmp->m_pMemDb = &m_pMemDb;
		tmp->m_pXdp.XdpCopy(&m_pXdp);//����xdp����
		tmp->m_pExchLink =   m_sExchDll.exchPoint;       //�������ӿڵ�����
		tmp->m_pClientLink = this;            //Ӧ��������
		tmp->m_pMemPool = m_pListenThread.m_pMemPool;
		tmp->m_pLog = m_pDrebApi.GetLogPoint();
		tmp->m_pRcvQueue = &m_pRcvQueue;
		tmp->m_nIndex = i;
		tmp->g_nSvrHostStatus = &(m_pRes.m_nSvrHostStatus);
		tmp->g_pPubData = &g_pPubData;
		if (!tmp->Init(&(m_pRes.m_pXml)))
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ProcThread init error");
			printf("ProcThread init error!\n");
#else
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�����̳߳�ʼ��ʧ��");
			printf("�����̳߳�ʼ��ʧ��\n");
#endif
			return false;
		}
//		m_lProcthread.push_back(tmp);
	}
	//����������빩����ע��
	S_TBL_PROCFUNC *funcinfo = m_lProcthread[0]->m_pFuncList.First();
	while (funcinfo != NULL)
	{
		m_pRes.g_lSvrTxList.push_back(funcinfo->nFuncNo);
		funcinfo = m_lProcthread[0]->m_pFuncList.Next();
	}
	
	
	if (!m_pListenThread.InitFront(&m_pRes,&m_pRcvQueue,m_pLog))
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"init listen error");
		printf("init listen error!\n");
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"��ʼ�������̳߳���");
		printf("��ʼ�������̳߳���\n");
#endif
		return false;
	}

	//��ʼ����ʱ��
	exchnode =  m_pRes.m_pXml.GetNodeByPath("/package/head/business/timer",false);
	if (exchnode == NULL)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xml file no[/package/head/business/timer] node");
		printf("xml file no [/package/head/business/timer] node! \n");
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XML������[/package/head/business/timer]�ڵ�");
		printf("XML������[/package/head/business/timer]�ڵ�! \n");
#endif
		return false;
	}
	exchnode = (CXmlNode *)exchnode->GetFirstChild();
	S_TIMER_CONF timeconf;
	while (exchnode != NULL)
	{
		attr = exchnode->GetAttribute("sep",false);
		if (attr ==NULL)
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xml file no [/package/head/business/timer/%s] node attribute sep",exchnode->GetNodeName(false).c_str());
			printf("xml file no [/package/head/business/timer/%s] node attribute sep! \n",exchnode->GetNodeName(false).c_str());
#else
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XML������[/package/head/business/timer/%s]�ڵ���sep����",exchnode->GetNodeName(false).c_str());
			printf("XML������[/package/head/business/timer/%s]�ڵ���sep����! \n",exchnode->GetNodeName(false).c_str());
#endif
			return false;
		}
		timeconf.sepms = atoi(attr->GetAttrValue(false).c_str());
		attr = exchnode->GetAttribute("txcode",false);
		if (attr ==NULL)
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xml file no [/package/head/business/timer/%s] node attribute txcode",exchnode->GetNodeName(false).c_str());
			printf("xml file no [/package/head/business/timer/%s] node attribute txcode! \n",exchnode->GetNodeName(false).c_str());
#else
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XML������[/package/head/business/timer/%s]�ڵ���txcode����",exchnode->GetNodeName(false).c_str());
			printf("XML������[/package/head/business/timer/%s]�ڵ���txcode����! \n",exchnode->GetNodeName(false).c_str());
#endif
			return false;
		}
		timeconf.txcode = atoi(attr->GetAttrValue(false).c_str());
		m_pTimerConf.push_back(timeconf);
		exchnode = (CXmlNode *)exchnode->getNextSibling();
	}
	m_pTimer.Init(100,true);
	for (i=0 ; i< m_pTimerConf.size(); i++)
	{
		m_pTimer.SetTimer(i,m_pTimerConf[i].sepms,&CTradeMain::OnTimer,this);
	}

	//��ʱд���ر���������
	m_pTimer.SetTimer(1000,5000,&CTradeMain::OnTimer,this);

	m_bIsInit = true;
	m_pRes.g_nStatus = 1;
	m_pRpcData.m_pMemPool = m_pListenThread.m_pMemPool;
	return true;
}

bool CTradeMain::Start()
{
	if (!m_bIsInit)
	{
		printf("δ��ʼ�� \n");
		return false;
	}
	//����api�߳�
	if (!m_pDrebApi.Start())
	{
#ifdef _ENGLISH_
		printf("Start DREB thread failed \n");
#else
		printf("���������߳�ʧ�� \n");
#endif
		return false;
	}
	//������Ϣ�����߳�
	if (!m_pDrebMsgThread.Start())
	{
#ifdef _ENGLISH_
		printf("Start DREB msgthread failed \n");
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"Start DREB msgthread failed!");
#else
		printf("����������Ϣ�����߳�ʧ�� \n");
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"����������Ϣ�����߳�ʧ��!");
#endif
		m_pDrebApi.Stop();
		return false;
	}

	if (!m_pListenThread.StartFront())
	{
#ifdef _ENGLISH_
		printf("Start listen thread failed \n");
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"Start listen thread failed!");
#else
		printf("���������߳�ʧ�� \n");
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���������߳�ʧ��!");
#endif
		
		m_pDrebApi.Stop();
		m_pDrebMsgThread.Stop();
		return false;
	}
	//��������������
	if (!m_sExchDll.exchPoint->Start())
	{
#ifdef _ENGLISH_
		printf("Start exchange thread failed \n");
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"Start exchange thread failed!");
#else
		printf("�����������߳�ʧ�� \n");
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�����������߳�ʧ��!");
#endif
		m_pDrebApi.Stop();
		m_pDrebMsgThread.Stop();
		m_pListenThread.StopFront();
		return false;
	}
	//���������߳�
	for (int i=0; i<m_pRes.g_nProcTheadNum; i++)
	{
		m_lProcthread[i]->Start();
	}
	
	if (m_pTimerConf.size()>0)
	{
		m_pTimer.Start();
	}
#ifdef _ENGLISH_
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"Start complete!");
#else
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"����tradekernel���!");
#endif
	m_pRes.g_nStatus = 3;
	return true;
}

void CTradeMain::Stop()
{
	
	m_pDrebMsgThread.Stop();
#ifdef _ENGLISH_
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"exit DREB msgthread!");
	m_pListenThread.StopFront();
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"exit listen thread!");
	m_pTimer.Stop();
	int i=0;
	for (i=0 ;i < m_pRes.g_nProcTheadNum ; i++)
	{
		m_lProcthread[i]->Stop();
	}
	m_sExchDll.exchPoint->Stop();
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"exit exchange thread!");
	m_pDrebApi.Stop();
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"exit DREBAPI thread!");
#else

	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"�˳�������Ϣ�����߳�!");
	m_pListenThread.StopFront();
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"�˳������߳�!");
	m_pTimer.Stop();
	int i=0;
	for (i=0 ;i <  m_pRes.g_nProcTheadNum ; i++)
	{
		m_lProcthread[i]->Stop();
	}
	m_sExchDll.exchPoint->Stop();

	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"�˳����̴����߳�!");
	m_pDrebApi.Stop();
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"�˳�����API�߳�!");
#endif
	m_pDrebMsgThread.Stop();
	m_pListenThread.StopFront();
	
	SLEEP_SECONDS(2);
}

void CTradeMain::Monitor()
{
	if (g_pRunFlag == 0)
	{
		return;
	}
 	m_pDrebApi.MonitorThread();
	if (m_pDrebMsgThread.IsStoped())
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"restart DREB msgthread!");
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"����������Ϣ�����߳�!");
#endif
		m_pDrebMsgThread.Start();
	}
	if (g_pRunFlag == 0)
	{
		return;
	}
	if (m_pListenThread.IsStoped())
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"restart listen thread!");
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���������߳�!");
#endif
		m_pListenThread.StartFront();
	}
	
	int i=0;
	for (i=0 ;i < m_pRes.g_nProcTheadNum ; i++)
	{
		if (m_lProcthread[i]->IsStoped())
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"restart proc thread [%d]!",i);
#else
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���������߳� %d!",i);
#endif
			m_lProcthread[i]->Start();
		}
	}
#ifdef _ENGLISH_
	switch (m_pRes.m_nSvrHostStatus)
	{
	case ARBSTATUS_UNKNOW:
		m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"node[%d %d] current status [unknow]\n",m_pRes.g_nSvrMainId,m_pRes.g_nSvrPrivateId);
		break;
	case ARBSTATUS_MASTER:
		m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"node[%d %d] current status [master]\n",m_pRes.g_nSvrMainId,m_pRes.g_nSvrPrivateId);
		break;
	case ARBSTATUS_MASTER_AYN:
		m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"node[%d %d] current status [master-ayn]\n",m_pRes.g_nSvrMainId,m_pRes.g_nSvrPrivateId);
		break;
	case ARBSTATUS_BACKUP:
		m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"node[%d %d] current status [backup]\n",m_pRes.g_nSvrMainId,m_pRes.g_nSvrPrivateId);
		break;
	case ARBSTATUS_BACKUP_AYN:
		m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"node[%d %d] current status [backup_ayn]\n",m_pRes.g_nSvrMainId,m_pRes.g_nSvrPrivateId);
		break;
	case MASTERHOST:
		break;
	default: 
		m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"node[%d %d] status error %d\n",m_pRes.g_nSvrMainId,m_pRes.g_nSvrPrivateId,m_pRes.m_nSvrHostStatus);
		break;
		
	}
#else
	switch (m_pRes.m_nSvrHostStatus)
	{
		case ARBSTATUS_UNKNOW:
			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"�ڵ�[%d %d]��ǰ����״̬Ϊ [δ֪]\n",m_pRes.g_nSvrMainId,m_pRes.g_nSvrPrivateId);
			break;
		case ARBSTATUS_MASTER:
			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"�ڵ�[%d %d]��ǰ����״̬Ϊ [����]\n",m_pRes.g_nSvrMainId,m_pRes.g_nSvrPrivateId);
			break;
		case ARBSTATUS_MASTER_AYN:
			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"�ڵ�[%d %d]��ǰ����״̬Ϊ [����ͬ��]\n",m_pRes.g_nSvrMainId,m_pRes.g_nSvrPrivateId);
			break;
		case ARBSTATUS_BACKUP:
			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"�ڵ�[%d %d]��ǰ����״̬Ϊ [����]\n",m_pRes.g_nSvrMainId,m_pRes.g_nSvrPrivateId);
			break;
		case ARBSTATUS_BACKUP_AYN:
			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"�ڵ�[%d %d]��ǰ����״̬Ϊ [����ͬ��]\n",m_pRes.g_nSvrMainId,m_pRes.g_nSvrPrivateId);
			break;
		case MASTERHOST:
			break;
		default: 
			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"�ڵ�[%d %d]��ǰ����״̬Ϊ����״̬ %d\n",m_pRes.g_nSvrMainId,m_pRes.g_nSvrPrivateId,m_pRes.m_nSvrHostStatus);
			break;
		
	}
#endif
 	//������Ϣͷ�������ڵ�ɾ��
	m_pRpcData.Delete();//ɾ�����ڵ�RPC���ݣ��ڴ��ڴ��ͷ�

	//��ӡ������Ŀ
	
	int total;
	int unused;
	int bufsize;
	m_pDrebApi.GetBufferPoolInfo(total,unused,bufsize);
#ifdef _ENGLISH_
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"MEMORY  total[%d] unuse[%d]  size[%d]",total,unused,bufsize);
#else
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"�����ڴ����  ����[%d] δʹ��[%d]  �����С[%d]",total,unused,bufsize);
#endif

}
bool CTradeMain::OpenDll(DLLHANDLE *pDll, const char *dllname, const char *dllpath)
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
			sprintf(errmsg,"open library[%s] failed",dllpathname);
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,errmsg);
#else
			sprintf(errmsg,"�򿪶�̬��[%s]ʧ��",dllpathname);
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,errmsg);
#endif
			return false;
		}
		sprintf(dllpathname,"%s",dllname);
		CBF_Tools::LRtrim(dllpathname);
		*pDll = LOADDLL(dllpathname);
		if (ISNOTOPEN(*pDll))
		{
#ifdef _ENGLISH_
			sprintf(errmsg,"open library[%s] failed",dllpathname);
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,errmsg);
#else
			sprintf(errmsg,"�򿪶�̬��[%s]ʧ��",dllpathname);
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,errmsg);
#endif
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
			sprintf(errmsg,"open library[%s] failed",dllpathname);
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,errmsg);
#else
			sprintf(errmsg,"�򿪶�̬��[%s]ʧ��",dllpathname);
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,errmsg);
#endif
			return false;
		}
		sprintf(dllpathname,"%s",dllname);
		CBF_Tools::LRtrim(dllpathname);
		*pDll = LOADDLL(dllpathname);
		pdllerror = dlerror();
		if (pdllerror!=NULL)
		{
#ifdef _ENGLISH_
			sprintf(errmsg,"open library[%s] failed [%s]",dllpathname,pdllerror);
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,errmsg);
#else
			sprintf(errmsg,"�򿪶�̬��[%s]ʧ��[%s]",dllpathname,pdllerror);
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,errmsg);
#endif
			return false;
		}
		
	}
#endif	
	return true;
}

int CTradeMain::Send2Client(S_TRADE_DATA &data)
{
	if (data.src == 0)
	{
		S_BPC_RSMSG msg;
		msg.sMsgBuf = data.pData;
		data.pData = NULL;
		if (msg.sMsgBuf == NULL)
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"Send2Client �������� ����ָ��ΪNULL");
			return -1;
		}
		return m_pDrebApi.SendMsg(msg);
	}
	else
	{
		if (data.pData == NULL)
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"Send2Client �������� ����ָ��ΪNULL");
			return -1;
		}
		if (m_pListenThread.m_pSockMgr.AffirmIndex(data.pData->sBpcHead.nIndex))
		{
			if (m_pListenThread.m_pSockMgr.at(data.pData->sBpcHead.nIndex)->SendMsg(data)<0)
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"index[%d] illegal",data.pData->sBpcHead.nIndex);
#else
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"index[%d]�Ƿ�",data.pData->sBpcHead.nIndex);
#endif
				m_pListenThread.m_pMemPool->PoolFree(data.pData);
				data.pData = NULL;
			}
			return -1;
		}
		else
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"index[%d] illegal",data.pData->sBpcHead.nIndex);
#else
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"index[%d]�Ƿ�",data.pData->sBpcHead.nIndex);
#endif
			m_pListenThread.m_pMemPool->PoolFree(data.pData);
			data.pData = NULL;
			return -1;
		}
	}
	return 0;
}
int CTradeMain::PushRcvQueue(S_TRADE_DATA &data)
{
	return m_pRcvQueue.PushData(data);
}
//����ʱ b_Cinfo.b_cIndex �����̵߳�ID��Ӧ�𷵻�ʱ���ݴ�ID�ŵ�Ӧ�������
int CTradeMain::PushAnsQueue(S_TRADE_DATA &data)
{
    if (data.pData == NULL)
    {
        return -1;
    }
	printf("dreb ans rpc_table size[%d] rpc_id[%d]\n", m_pRpcData.Size(), data.pData->sDBHead.s_Sinfo.s_nSerial);

	S_RPC_DATA rpcdata;
	if (m_pRpcData.Select(data.pData->sDBHead.s_Sinfo.s_nSerial, rpcdata))
	{
		 //��rpc����
		if (m_pRpcData.Delete(data.pData->sDBHead.s_Sinfo.s_nSerial)<0)
		{
			printf("rpc_table delete error size[%d] \n", m_pRpcData.Size());
		}
		else
		{
			printf("rpc_table delete sucess size[%d] \n", m_pRpcData.Size());
		}
		try
		{
			(rpcdata.func)(rpcdata.data, data);
		}
		catch (...)
		{
			m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "�ص����� requestid[%d]", data.pData->sDBHead.s_Sinfo.s_nSerial);
		}
		
		if (rpcdata.data.pData != NULL) 
		{
			printf("PoolFree \n");
            m_pListenThread.m_pMemPool->PoolFree(rpcdata.data.pData);
			rpcdata.data.pData = NULL;
		}
		return 0;

	}
	else
	{
		printf("rpc_id[%d] not found\n", data.pData->sDBHead.s_Sinfo.s_nSerial);
		m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "�������ˮ[%d]����rpc", data.pData->sDBHead.s_Sinfo.s_nSerial);
	}
	
	if (data.pData->sDBHead.b_Cinfo.b_cIndex <0 || data.pData->sDBHead.b_Cinfo.b_cIndex > m_pRes.g_nProcTheadNum-1)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"thread index s_nHook[%d] illegal",data.pData->sDBHead.b_Cinfo.b_cIndex );
#else
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�߳�����s_nHook[%d]����",data.pData->sDBHead.b_Cinfo.b_cIndex );
#endif
		m_pListenThread.m_pMemPool->PoolFree(data.pData);
		data.pData = NULL;
		return -1;
	}
	//ͬ������Ӧ�������߳��ڶ�������ȴ�Ӧ���������������߳�GetAnsData����ȡ��Ӧ��
	m_lProcthread[data.pData->sDBHead.b_Cinfo.b_cIndex]->m_pAnsQueue.PushData(data);
	return 0;	
}

int CTradeMain::GetAnsData(int index,S_TRADE_DATA &data,int tms)
{
	if (index <0 || index > m_pRes.g_nProcTheadNum-1)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"thread Index[%d] illegal",index);
#else
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�߳�����Index[%d]����",index);
#endif
		return -1;
	}
	if (!m_lProcthread[index]->m_pAnsQueue.GetData(data,tms))
	{
		return -1;
	}
	return 0;
}

int CTradeMain::OnTimer(unsigned int event, void *p)
{
	CTradeMain *pp = (CTradeMain *)p;
	return pp->proctimer(event);
}

int CTradeMain::proctimer(int id)
{
	if (id == 1000) //ʹ��1000ID����ʱ���汾�ر����ŵ�����
	{
		g_pPubData.WriteToFile(m_sSerialDataFile);
		
		return 0;
	}
	S_TRADE_DATA data;
	data.pData = (PBPCCOMMSTRU)m_pListenThread.m_pMemPool->PoolMalloc();
	if (data.pData == NULL)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"malloc msg data error");
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"��ʱ��������ڴ����");
#endif
		return -1;
	}
	data.src = 2;
	data.pData->sDBHead.d_Dinfo.d_nServiceNo = m_pTimerConf[id].txcode;
	//������ն���
	return m_pRcvQueue.PushData(data);
	
}

void CTradeMain::PoolFree(S_TRADE_DATA &data)
{
	m_pListenThread.m_pMemPool->PoolFree(data.pData);
	data.pData = NULL;
}

int CTradeMain::PoolMalloc(S_TRADE_DATA &data)
{
	data.pData = (PBPCCOMMSTRU)m_pListenThread.m_pMemPool->PoolMalloc();
	if (data.pData == NULL)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"malloc msg data error");
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�����ڴ����");
#endif
		return -1;
	}
	return 0;
}
// ������: AnsData
   // ���  : ������ 2017-7-20 9:28:49
   // ����  : virtual int 
   // ����  : S_TRADE_DATA &data  src=1ʱdata.pData->sBpcHead.nIndexΪsocket�������
   // ����  : char nextflag  0�޺�����  1 �к�����  10���һ�����ݰ�
   // ����  : Ӧ������  ���ݸ�ʽΪ����ͷ+����  src=0��ʾ����  src=1��ʾsocketֱ��
int CTradeMain::AnsData(S_TRADE_DATA& data, char nextflag)
{
	
    S_BPC_RSMSG msg;
    msg.sMsgBuf = data.pData;
	data.pData = NULL;
    if (msg.sMsgBuf == NULL)
    {
        m_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, "AnsData �������� ����ָ��ΪNULL");
        return -1;
    }
	msg.sMsgBuf->sDBHead.cNextFlag = nextflag;
	msg.sMsgBuf->sDBHead.cRaflag = 1;
    return m_pDrebApi.SendMsg(msg);

}


// ������: RpcRequest
// ���  : ������ 2017-7-20 9:28:49
// ����  : virtual int 
// ����  : S_TRADE_DATA &data  src=1ʱdata.pData->sBpcHead.nIndexΪsocket�������
// ����  : char nextflag  0�޺�����  1 �к�����  10���һ�����ݰ�
// ����  : Ӧ������  ���ݸ�ʽΪ����ͷ+����  src=0��ʾ����  src=1��ʾsocketֱ��
int CTradeMain::RpcRequest(S_TRADE_DATA& data, int requestid, std::function<int(S_TRADE_DATA& reqdata, S_TRADE_DATA& ansdata)> func)
{
	//��¼requestid�������ģ���Ӧ�������ʱ�������	RpcRequest��ֱ�ӽ����ķ�����ն��У��ɴ����߳�ȡ���ٸ���requestid�õ�ԭ�����ģ��ٵ��ûص���������
    if (data.pData == NULL)
    {
        m_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, "RpcRequest �������� ����ָ��ΪNULL");
        return -1;
    }
	data.pData->sDBHead.cCmd = CMD_DPCALL;
	data.pData->sDBHead.cRaflag = 0;
	data.pData->sDBHead.s_Sinfo.s_nSerial = requestid;
	data.pData->sBpcHead.nIndex = 100;	//��apiѡ���ĸ��������ӷ���
	S_RPC_DATA rpcdata;
	rpcdata.request_id = requestid;
	rpcdata.rpctime= time(NULL);
	rpcdata.func = func;
	//�����ڴ�
	rpcdata.data.pData = (PBPCCOMMSTRU)m_pListenThread.m_pMemPool->PoolMalloc();
    if (rpcdata.data.pData == NULL)
    {
#ifdef _ENGLISH_
        m_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, "malloc msg data error");
#else
        m_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, "�����ڴ����");
#endif
        return -1;
    }
	//����ԭ������
    memcpy(rpcdata.data.pData,data.pData,sizeof(BPCCOMMSTRU));
	if (m_pRpcData.Insert(rpcdata) < 0)
	{
		PoolFree(rpcdata.data);
		m_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, "RpcRequest requestid[%d]�ظ�", rpcdata.request_id);
		return -1;
	}
	printf("rpc_table size[%d] rpc_id[%d]\n", m_pRpcData.Size(), rpcdata.request_id);
    S_BPC_RSMSG msg;
    msg.sMsgBuf = data.pData;
    data.pData = NULL;
    return m_pDrebApi.SendMsg(msg);

}

//ȡ��������ˮ��
int   CTradeMain::GetSerial()
{
	return m_pRes.GetSerial();
}
//ȡ�����ı��غ�
UINT64_  CTradeMain::GetLocalNo()
{
	return g_pPubData.GetLocalSerial();
}
