// GlobalVar.cpp: implementation of the CGlobalVar class.
//
//////////////////////////////////////////////////////////////////////

#include "GlobalVar.h"
#include "BF_Xml.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGlobalVar::CGlobalVar()
{
	memset(g_curentpath,0,sizeof(g_curentpath));
	memset(g_modulename,0,sizeof(g_modulename));
	memset(g_modulepath,0,sizeof(g_modulepath));
	memset(m_errMsg,0,sizeof(m_errMsg));
	g_bIsExit = false;

//	g_nModuleThreadNum = 1;
	g_nDrebRouteTime = 5;
	g_nServiceRouteTime = 15;
	g_nSvrDeleteTime=300;      //����Ͽ���ɾ���������ʱ��
	g_nServiceDeleteTime=60;  //������·�ɹ���ʱ��
	m_pShmpub = NULL;

	g_nDisconnectTime  = 600 ;  //�ͻ���600��û���ݾͶϿ�

	g_nMaxLogSize = 0;
	g_nMaxDataLogSize =0;
	g_nRegOtherDreb = 1;
	g_nPrintRoute =0;

	m_bIsRecTime = 1;
	
	g_nMaxRouteStep = 16;
}

CGlobalVar::~CGlobalVar()
{
	g_pLog.StopLog();
	g_pDataLog.StopLog();
	g_pRouteLog.StopLog();
}

bool CGlobalVar::Init(const char *confile)
{
	g_sConfFileName = confile;
	CBF_Xml  xmlconf;
	if (!xmlconf.FromFile(confile))
	{
		sprintf(m_errMsg,"�����ļ�%s�����ڻ��xml�ļ�",confile);
		return false;
	}
	
	if (xmlconf.GetNodeValueByPath("package/head/loglevel",false,g_nLogLevel)==NULL)
	{
		sprintf(m_errMsg,"�ڵ�[package/head/loglevel]û������");
		return false;
	}
	
	if (xmlconf.GetNodeValueByPath("package/head/logsize",false,g_nMaxLogSize)==NULL)
	{
		sprintf(m_errMsg,"�ڵ�[package/head/logsize]û������");
		return false;
	}
	if (xmlconf.GetNodeValueByPath("package/head/logfile",false,g_sLogFileName)==NULL)
	{
		sprintf(m_errMsg,"�ڵ�[package/head/logfile]û������");
		return false;
	}
	if (xmlconf.GetNodeValueByPath("package/head/logdir",false,g_sLogFilePath)==NULL)
	{
		g_sLogFilePath = "";
	}
	if (xmlconf.GetNodeValueByPath("package/head/crc",false,g_nCrcFlag) == NULL)
	{
		
		g_nCrcFlag=0;
	}

	//��ֹͣ��־�߳�
	if (!g_pLog.IsStopedThread())
	{
		g_pLog.StopLog();
	}
	g_pLog.SetMaxLogSize(g_nMaxLogSize);
	g_pLog.SetLogPara(g_nLogLevel,g_sLogFilePath.c_str(),g_sLogFileName.c_str());
	//������־�߳�
	g_pLog.StartLog();

	
	if (xmlconf.GetNodeValueByPath("package/head/routeprint",false,g_nPrintRoute)==NULL)
	{
		sprintf(m_errMsg,"�ڵ�[package/head/routeprint]û������,Ĭ�ϲ���ӡ");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
		g_nPrintRoute =0;
	}
	

	if (xmlconf.GetNodeValueByPath("package/head/ListenPort",false,g_nPort)==NULL)
	{
		sprintf(m_errMsg,"�ڵ�[package/head/ListenPort]�����˿�û������");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
		return false;
	}
	if (xmlconf.GetNodeValueByPath("package/head/nodeid",false,g_nDrebId)==NULL)
	{
		sprintf(m_errMsg,"�ڵ�[package/head/nodeid]û������");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
		return false;
	}
	if (g_nDrebId<1 || g_nDrebId >65534)
	{
		sprintf(m_errMsg,"�����ڵ�����ò���,ȡֵΪ1-65534");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
		return false;
	}
	if (xmlconf.GetNodeValueByPath("package/head/nodepid",false,g_nDrebPrivateId)==NULL)
	{
		sprintf(m_errMsg,"�ڵ�[package/head/nodepid]û������");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
		return false;
	}
	if (g_nDrebPrivateId<1 || g_nDrebPrivateId>254)
	{
		sprintf(m_errMsg,"˽�нڵ�����ò���,ȡֵΪ1-254");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
		return false;
	}
	if (xmlconf.GetNodeValueByPath("package/head/dataloglevel",false,g_nDataLogLevel)==NULL)
	{
		sprintf(m_errMsg,"�ڵ�[package/head/dataloglevel]û������");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
		return false;
	}
	if (xmlconf.GetNodeValueByPath("package/head/datalogsize",false,g_nMaxDataLogSize)==NULL)
	{
		sprintf(m_errMsg,"�ڵ�[package/head/datalogsize]û������");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
		return false;
	}

	if (xmlconf.GetNodeValueByPath("package/head/routestep",false,g_nMaxRouteStep)==NULL)
	{
		sprintf(m_errMsg,"�ڵ�[package/head/routestep]û�����ã�Ĭ��Ϊ16");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
		g_nMaxRouteStep=16;
	}

	char tmpchar1[100];
	char tmpchar2[100];
	sprintf(tmpchar1,"DREB%d-%d.dat",g_nDrebId,g_nDrebPrivateId);
	sprintf(tmpchar2,"BPC%d-%d.dat",g_nDrebId,g_nDrebPrivateId);
	g_pDataLog.SetLogPara(g_nMaxDataLogSize,g_nDataLogLevel,g_sLogFilePath.c_str(),tmpchar1,tmpchar2);
	//�����߳�
	g_pDataLog.StartLog();



	if (g_nPrintRoute == 1)
	{
		//��ֹͣ��־�߳�
		if (!g_pRouteLog.IsStopedThread())
		{
			g_pRouteLog.StopLog();
		}
		char tmpstr[100];
		sprintf(tmpstr,"dreb%d_%d.route.log",g_nDrebId,g_nDrebPrivateId);
		g_pRouteLog.SetMaxLogSize(g_nMaxLogSize);
		g_pRouteLog.SetLogPara(g_nLogLevel,g_sLogFilePath.c_str(),tmpstr);
		//������־�߳�
		g_pRouteLog.StartLog();
	}
	if (xmlconf.GetNodeValueByPath("package/head/iscompress",false,g_nZip)==NULL)
	{
		sprintf(m_errMsg,"�ڵ�[package/head/iscompress]û������");
		g_nZip = 0;
		g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_errMsg);
	}
	if (xmlconf.GetNodeValueByPath("package/head/maxconnectnum",false,g_nMaxConnCount)==NULL)
	{
		sprintf(m_errMsg,"�ڵ�[package/head/maxconnectnum]û������");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
		return false;
	}
	if (g_nMaxConnCount < 20 || g_nMaxConnCount >640)
	{
		sprintf(m_errMsg,"������������ò�����ȡֵΪ20-640");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
		return false;
	}
	
	if (xmlconf.GetNodeValueByPath("package/head/heartcheck",false,g_nBeatHeart) == NULL)
	{
		sprintf(m_errMsg,"�ڵ�[package/head/heartcheck]û������");
		g_nBeatHeart = 0;
		g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_errMsg);
	}
	int tmpint;
	if (xmlconf.GetNodeValueByPath("package/head/muthread",false,tmpint) == NULL)
	{
		sprintf(m_errMsg,"�ڵ�[package/head/muthread]û������");
		tmpint = 0;
		g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_errMsg);
	}
	if (tmpint == 1)
	{
		g_bMsgProcThread = true;
		sprintf(m_errMsg,"���ô����̣߳����պʹ��������߳�");
//		printf("%s\n",m_errMsg);
		g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_errMsg);
	}
	else
	{
		g_bMsgProcThread = false;
		sprintf(m_errMsg,"�����ô����̣߳�һ�����մ����̼߳���");
//		printf("%s\n",m_errMsg);
		g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_errMsg);
	}

	if (xmlconf.GetNodeValueByPath("package/head/regotherdreb",false,g_nRegOtherDreb)==NULL)
	{
		sprintf(m_errMsg,"�ڵ�[package/head/regotherdreb]û������,Ĭ��Ϊע�����");
		g_nRegOtherDreb = 1;
		g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_errMsg);
	}
	else
	{
		if (g_nRegOtherDreb !=0 )
		{
			g_nRegOtherDreb = 1;
		}
	}
	
	if (xmlconf.GetNodeValueByPath("package/head/svrroutetime",false,g_nServiceRouteTime) == NULL)
	{
		sprintf(m_errMsg,"�ڵ�[package/head/svrroutetime]û������,Ĭ��Ϊ15��");
		printf("%s\n",m_errMsg);
		g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_errMsg);
		g_nServiceRouteTime=15;//Ĭ��Ϊ15��
	}
	if (xmlconf.GetNodeValueByPath("package/head/noderoutetime",false,g_nDrebRouteTime) == NULL)
	{
		sprintf(m_errMsg,"�ڵ�[package/head/noderoutetime]û������,Ĭ��Ϊ15��");
		printf("%s\n",m_errMsg);
		g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_errMsg);
		g_nDrebRouteTime=5;//Ĭ��Ϊ5��
	}

	if (xmlconf.GetNodeValueByPath("package/head/svrdelaytime",false,g_nSvrDeleteTime) == NULL)
	{
		sprintf(m_errMsg,"�ڵ�[package/head/svrdelaytime]û������,Ĭ��Ϊ300��");
		printf("%s\n",m_errMsg);
		g_nSvrDeleteTime=300;//Ĭ��Ϊ300��
		g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_errMsg);
	}
	
	if (xmlconf.GetNodeValueByPath("package/head/clientclosetime",false,g_nDisconnectTime) == NULL)
	{
		sprintf(m_errMsg,"�ڵ�[package/head/clientclosetime]û������,Ĭ��Ϊ300��");
		printf("%s\n",m_errMsg);
		g_nDisconnectTime=300;//Ĭ��Ϊ300��
		g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_errMsg);
	}

	if (xmlconf.GetNodeValueByPath("package/head/funcdelaytime",false,g_nServiceDeleteTime) == NULL)
	{
		sprintf(m_errMsg,"�ڵ�[package/head/funcdelaytime]û������,Ĭ��Ϊ60��");
		printf("%s\n",m_errMsg);
		g_nServiceDeleteTime=60;//Ĭ��Ϊ60��
		g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_errMsg);
	}

	if (xmlconf.GetNodeValueByPath("package/head/closeheartnum",false,g_nHeartCount) == NULL)
	{
		sprintf(m_errMsg,"�ڵ�[package/head/closeheartnum]û������");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
		return false;
	}
	if (g_nHeartCount <2 || g_nHeartCount >10)
	{
		sprintf(m_errMsg,"�Ͽ������������ò�����ȡֵΪ2-10");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
		return false;
	}
	if (xmlconf.GetNodeValueByPath("package/head/sendwindownum",false,g_nBufferLimit) == NULL)
	{
		sprintf(m_errMsg,"�ڵ�[package/head/sendwindownum]û������");
		g_nBufferLimit =0;
		g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_errMsg);
	}
	
	if (g_nBufferLimit>0 && g_nBufferLimit <100)
	{
		sprintf(m_errMsg,"���ͻ���������ò�����ȡֵΪ0����С100");
		g_nBufferLimit =0;
		g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_errMsg);
	}
	g_nUseMonitor=0;
	if (xmlconf.GetNodeAttribute("package/head/monitor",false,"use",false,g_nUseMonitor) == NULL)
	{
		sprintf(m_errMsg,"��package/head/�������");
		g_nUseMonitor = 0;
		g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_errMsg);
	}
	if (g_nUseMonitor == 1)
	{
		if (xmlconf.GetNodeAttribute("package/head/monitor",false,"host",false,g_nMonitorHost) == NULL)
		{
			sprintf(m_errMsg,"��package/head/monitor");
			g_nMonitorHost = 1;
			g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_errMsg);
		}
		if (xmlconf.GetNodeValueByPath("package/head/monitor/drebid",false,g_nMonitorDrebId) == NULL)
		{
			sprintf(m_errMsg,"�ڵ�[package/head/monitor/drebid]û������");
			g_nMonitorDrebId = 0;
			g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_errMsg);
		}
		if (xmlconf.GetNodeValueByPath("package/head/monitor/drebpid",false,g_nMonitorDrebPrivateId) == NULL)
		{
			sprintf(m_errMsg,"�ڵ�[package/head/monitor/drebpid]û������");
			g_nMonitorDrebPrivateId = 0;
			g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_errMsg);
		}
		if (xmlconf.GetNodeValueByPath("package/head/monitor/svrid",false,g_nMonitorSvrId) == NULL)
		{
			sprintf(m_errMsg,"�ڵ�[package/head/monitor/svrid]û������");
			g_nMonitorSvrId = 0;
			g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_errMsg);
		}
		if (xmlconf.GetNodeValueByPath("package/head/monitor/svrpid",false,g_nMonitorSvrPrivateId) == NULL)
		{
			sprintf(m_errMsg,"�ڵ�[package/head/monitor/svrpid]û������");
			g_nMonitorSvrPrivateId = 0;
			g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_errMsg);
		}
		if (xmlconf.GetNodeValueByPath("package/head/monitor/txcode",false,g_nMonitorTxCode) == NULL)
		{
			sprintf(m_errMsg,"�ڵ�[package/head/monitor/txcode]û������");
			g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
			return false;
		}
	}

// 	if (xmlconf.GetNodeValueByPath("package/head/���շ����߳���",false,g_nModuleThreadNum) == NULL)
// 	{
// 		sprintf(m_errMsg,"�ڵ�[package/head/���շ����߳���]û������");
// 		return false;
// 	}
// 	if (g_nModuleThreadNum<1 || g_nModuleThreadNum>10)
// 	{
// 		sprintf(m_errMsg,"���շ����߳������ò�����ȡֵΪ1-10");
// 		return false;
// 	}
	g_pDate.Update();
	CBF_Tools::StringCopy(g_pDrebStatInfo.m_sStartDate,17,g_pDate.ToStringDT17().c_str());

	int uplink=0;
	if (xmlconf.GetNodeAttribute("package/pack",false,"drebnum",false,uplink) == NULL)
	{
		sprintf(m_errMsg,"���������߼���");
		g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_errMsg);
		return true;
	}
	if (uplink<1)
	{
		sprintf(m_errMsg,"���������߼���");
		g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_errMsg);
		return true;
	}
	S_CENTERINFO info;
	CXmlNode *tmpnode = xmlconf.GetNodeByPath("package/pack",false);
	if (tmpnode == NULL)
	{
		sprintf(m_errMsg,"��package/pack�ڵ�");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
		return false;
	}
	tmpnode = (CXmlNode *)tmpnode->GetFirstChild();
	int itemcount=0;
	std::string stemp;
	while (tmpnode != NULL)
	{
		info.sname = tmpnode->GetNodeName();
		if (tmpnode->GetAttribute("ip",false,info.sIp) == NULL)
		{
			sprintf(m_errMsg,"%s ��ipû������",info.sname.c_str());
			g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
			return false;
		}

		if (tmpnode->GetAttribute("port",false,stemp) == NULL)
		{
			sprintf(m_errMsg,"%s ��portû������",info.sname.c_str());
			g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
			return false;
		}
		info.nPort = atoi(stemp.c_str());
		if (tmpnode->GetAttribute("bandwidth",false,stemp) == NULL)
		{
			sprintf(m_errMsg,"%s ��bandwidthû������",info.sname.c_str());
			g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
			return false;
		}
		info.nSpeed = atoi(stemp.c_str());
		itemcount++;
		g_pCenterList.push_back(info);
		if (itemcount>=uplink)
		{
			break;
		}
		tmpnode = (CXmlNode *)tmpnode->getNextSibling();
	}
	
	return true;
}

int CGlobalVar::getTick()
{
//	CBF_PMutex pp(&m_mutex);
	m_pShmpub->nSerial_get++;
	if (m_pShmpub->nSerial_get<=0 || m_pShmpub->nSerial_get >999999999)
	{
		m_pShmpub->nSerial_get = 1;
	}
	return m_pShmpub->nSerial_get;
}
void CGlobalVar::PutTime()
{
	if (1 == m_bIsRecTime)
	{
		m_vTime.push_back(CBF_Date_Time::GetTickUS());
	}
	
}

void CGlobalVar::LogTime()
{
	for (unsigned int ret=0; ret < m_vTime.size() ; ret ++)
	{
#ifdef WINDOWS
		g_pLog.LogMp(LOG_INFO,__FILE__,__LINE__,"��%d�� ʱ��[%I64d]",ret,m_vTime[ret]);
#else
		g_pLog.LogMp(LOG_INFO,__FILE__,__LINE__,"��%d�� ʱ��[%lld]",ret,m_vTime[ret]);
#endif
	}
	m_vTime.clear();
}
