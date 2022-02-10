// GResource.cpp: implementation of the CGResource class.
//
//////////////////////////////////////////////////////////////////////

#include "GResource.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGResource::CGResource()
{
	g_para1 = NULL;
	g_para2 = NULL;
	g_para3 = NULL;

	g_bToExit = false;
	m_serial_connect = 0;
	g_nPort = 9001;//�����˿� Ĭ��Ϊ9001
	g_nLoglevel = 5;//��־���� Ĭ��Ϊ5
	g_nLogSize=0;
	
	g_nProcessThreadNum = 10;//�����߳���,Ĭ��Ϊ10

	
	
	g_nMaxConnectNum = 0;//��������� 0Ϊ�����ƣ�Ĭ��
	g_nDisconnectTime = 600;//δʹ�öϿ�ʱ�䣬���һ�������ڴ�ʱ����һֱû��ʹ�ã��򽫴����ӶϿ�����λ��Ĭ��600�뼴10����
	g_nDispatchTime = 10;
	g_nHeadNum = 5;//ͨѶ����ͷ���ȣ�Ĭ��Ϊ5
	g_nHeartRun = 600;//����ʱ��  

	g_nQueueDelayTime= 10;//���г�ʱ��

	g_bIsInitOnce = false; //�Ƿ��ʼ��һ�Σ�����һ���߳�����г�ʼ���󣬽��˱�־��Ϊ1�������߳��жϴ˱�־�Ƿ���г�ʼ��
	g_bIsListen = false;//ֻ������һ�Σ�������󽫴˱�־��Ϊtrue,�����߳̽���������

	m_nSerialNum = 1;
	int ap = sizeof(m_serial_connect)*8-1; 
	m_nMaxSerial = (unsigned long)pow(2.0,ap);

	g_processSucessNum=0;
	g_processNum = 0;
	
	g_bIsMonitor = false;

	m_bisSetGPara1 = false;
	m_bisSetGPara2 = false;
	m_bisSetGPara3 = false;
	m_nSerial_get = 0;
	m_nSerial_getMax = 99999999;

}

CGResource::~CGResource()
{
	m_log.StopLog();
}

unsigned long CGResource::GetSerial()
{
//	CPMUTEX pp(&m_mutex);
	if (m_serial_connect >= m_nMaxSerial)
	{
		m_serial_connect = 1;
		if (m_nSerialNum >= m_nMaxSerial)
		{
			m_nSerialNum = 1; 
		}
		else
		{
			m_nSerialNum++;
		}
	}
	else
	{
		
		m_serial_connect++;
	}
	return m_serial_connect;
}

bool CGResource::Init(char *confile)
{
	m_frontParaFileName = confile;
	if (confile == NULL)
	{
		m_errMsg = "�����ļ�Ϊ��";
		return false;
	}
	if (m_xmlconf.fromFile(confile)<0)
	{
		printf("�����ļ��Ƿ�,�����ļ�[%s]\n",confile);
		m_errMsg = "���������ļ��Ƿ�";
		return false;
	}

	
	if (m_xmlconf.gotoHead()<0)
	{
		m_errMsg = "��ʼ�������XML�ڵ�headΪNULL";
		return false;
	}

	int ret=0;
	std::string stmp;
	CNode *pNode =NULL;
	CNode *pChildNode =NULL;
	pNode = m_xmlconf.m_xml.getFirstChildItem("public");
	if (pNode == NULL)
	{
		m_errMsg = "�����ļ���,head�ڵ�����[public]�ڵ�";
		return false;
	}
	pChildNode = m_xmlconf.m_xml.getFirstChildItem("loglevel");
	if (pChildNode == NULL)
	{
		m_errMsg = "�����ļ���,head�ڵ��µ�public�ڵ���[loglevel]�ڵ�";
		return false;
	}
	g_nLoglevel = atoi(pChildNode->getValue().c_str());
	if (g_nLoglevel <0 )
	{
		g_nLoglevel = LOG_DEBUG;//���ô���Ϊ5
	}
	m_xmlconf.m_xml.setCurrentNode(pNode);
	pChildNode = m_xmlconf.m_xml.getFirstChildItem("logfile");
	if (pChildNode == NULL)
	{
		m_errMsg = "�����ļ���,head�ڵ��µ�public�ڵ���[logfile]�ڵ�";
//		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ļ��� %s",(char *)m_errMsg.c_str());
		return false;
	}
	
	g_sLogFileName = pChildNode->getValue();
	if (g_sLogFileName.length()<1)
	{
		m_errMsg = "�����ļ���,head�ڵ��µ�public�ڵ���[��־�ļ�]�ڵ�û������";
//		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ļ��� %s",(char *)m_errMsg.c_str());
		return false;
	}
	m_xmlconf.m_xml.setCurrentNode(pNode);
	pChildNode = m_xmlconf.m_xml.getFirstChildItem("logdir");
	if (pChildNode == NULL)
	{
		m_errMsg = "�����ļ���,head�ڵ��µ�public�ڵ���[logdir]�ڵ�";
//		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ļ��� %s",(char *)m_errMsg.c_str());
		return false;
	}
	g_sLogFilePath = pChildNode->getValue();

	m_xmlconf.m_xml.setCurrentNode(pNode);
	pChildNode = m_xmlconf.m_xml.getFirstChildItem("logsize");
	if (pChildNode == NULL)
	{
		m_errMsg = "�����ļ���,head�ڵ��µ�public�ڵ���[logsize]�ڵ�";
//		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ļ��� %s",(char *)m_errMsg.c_str());
		g_nLogSize =0;
	}
	else
	{
		g_nLogSize = atoi(pChildNode->getValue().c_str());
		if (g_nLogSize <1024)
		{
			g_nLogSize=0;
		}
	}
	m_log.SetMaxLogSize(g_nLogSize);
	m_log.SetLogPara(g_nLoglevel,g_sLogFilePath.c_str(),g_sLogFileName.c_str());
	m_log.StartLog();


	m_xmlconf.gotoHead();
	pNode = m_xmlconf.m_xml.getFirstChildItem("front");
	if (pNode == NULL)
	{
		m_errMsg = "�����ļ���,head�ڵ�����[front]�ڵ�";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ļ��� %s",(char *)m_errMsg.c_str());
		return false;
	}
	pChildNode = m_xmlconf.m_xml.getFirstChildItem("listenport");
	if (pChildNode == NULL)
	{
		m_errMsg = "�����ļ���,head�ڵ��µ�front�ڵ���[listenport]�ڵ�";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ļ��� %s",(char *)m_errMsg.c_str());
		return false;
	}
	g_nPort = atoi(pChildNode->getValue().c_str());
	if (g_nPort <= 1024)
	{
		m_errMsg = "�����ļ���,head�ڵ��µ�front�ڵ�[listenport]�ڵ����ò��Ϸ�";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ļ��� %s",(char *)m_errMsg.c_str());
		return false;
	}
	
	m_xmlconf.m_xml.setCurrentNode(pNode);
	pChildNode = m_xmlconf.m_xml.getFirstChildItem("dispatchtime");
	if (pChildNode == NULL)
	{
		g_nDispatchTime = 10;
	}
	else
	{
		g_nDispatchTime = atol(pChildNode->getValue().c_str());
	}

	m_xmlconf.m_xml.setCurrentNode(pNode);
	pChildNode = m_xmlconf.m_xml.getFirstChildItem("reqduetime");
	if (pChildNode == NULL)
	{
		g_nQueueDelayTime= 10;
	}
	else
	{
		g_nQueueDelayTime = atol(pChildNode->getValue().c_str());
	}

	m_xmlconf.m_xml.setCurrentNode(pNode);
	pChildNode = m_xmlconf.m_xml.getFirstChildItem("monitormemuse");
	if (pChildNode == NULL)
	{
		g_bIsMonitor = false;
	}
	else
	{ 
		if (atol(pChildNode->getValue().c_str()) == 1)
		{
			g_bIsMonitor = true;
		}
		else
		{
			g_bIsMonitor = false;
		}
	}

	m_xmlconf.m_xml.setCurrentNode(pNode);
	pChildNode = m_xmlconf.m_xml.getFirstChildItem("maxserial");
	if (pChildNode == NULL)
	{
		m_nSerial_getMax = 99999999;
	}
	else
	{
		m_nSerial_getMax = atol(pChildNode->getValue().c_str());
	}
	m_xmlconf.m_xml.setCurrentNode(pNode);
	pChildNode = m_xmlconf.m_xml.getFirstChildItem("curserial");
	if (pChildNode == NULL)
	{
		m_nSerial_get = 1;
	}
	else
	{
		m_nSerial_get = atol(pChildNode->getValue().c_str());
	}

	
	m_xmlconf.m_xml.setCurrentNode(pNode);
	pChildNode = m_xmlconf.m_xml.getFirstChildItem("commheadlen");
	if (pChildNode == NULL)
	{
		m_errMsg = "�����ļ���,head�ڵ��µ�front�ڵ���[commheadlen]�ڵ�";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ļ��� %s",(char *)m_errMsg.c_str());
		return false;
	}
	g_nHeadNum = atoi(pChildNode->getValue().c_str());
	if (g_nHeadNum < 1)
	{
		g_nHeadNum = 0;
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ļ� %s","ͨѶ����ͷ����ȡĬ����Сֵ0");
	}
	m_xmlconf.m_xml.setCurrentNode(pNode);
	pChildNode = m_xmlconf.m_xml.getFirstChildItem("clienthead");
	if (pChildNode != NULL)
	{
		CNode *tmpnode = pChildNode->getFirstChild();
		while (tmpnode!= NULL)
		{
			S_IP_HEAD tmphead;
			memset(&tmphead,0,sizeof(tmphead));
			strcpy(tmphead.sip,tmpnode->getName().c_str());
			tmphead.nLen = atoi(tmpnode->getValue().c_str());
			m_ip_head_list.push_back(tmphead);
			tmpnode = tmpnode->getNextSibling();
		}
	}

	m_xmlconf.m_xml.setCurrentNode(pNode);
	pChildNode = m_xmlconf.m_xml.getFirstChildItem("hearttime");
	if (pChildNode != NULL)
	{
		ret = atoi(pChildNode->getValue().c_str());
		if (ret <5)
		{
			g_nHeartRun = 5;
		}
		else
		{
			g_nHeartRun = ret;
		}
	}

	m_xmlconf.m_xml.setCurrentNode(pNode);
	pChildNode = m_xmlconf.m_xml.getFirstChildItem("maxconnectnum");
	if (pChildNode == NULL)
	{
		m_errMsg = "�����ļ���,head�ڵ��µ�front�ڵ���[maxconnectnum]�ڵ�";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ļ��� %s",(char *)m_errMsg.c_str());
		return false;
	}
	g_nMaxConnectNum = atoi(pChildNode->getValue().c_str());
	if (g_nMaxConnectNum <100 )
	{
		g_nMaxConnectNum = 100;
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ļ� %s","���������ȡĬ����Сֵ100");
	}
	m_xmlconf.m_xml.setCurrentNode(pNode);
	pChildNode = m_xmlconf.m_xml.getFirstChildItem("disconnecttime");
	if (pChildNode == NULL)
	{
		m_errMsg = "�����ļ���,head�ڵ��µ�front�ڵ���[disconnecttime]�ڵ�";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ļ��� %s",(char *)m_errMsg.c_str());
		return false;
	}
	g_nDisconnectTime = atoi(pChildNode->getValue().c_str());
	if (g_nDisconnectTime < 6 )
	{
		g_nDisconnectTime = 6;
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ļ� %s","δʹ�öϿ�ʱ��ȡĬ����Сֵ6");
	}
	
	m_xmlconf.gotoHead();
	pNode = m_xmlconf.m_xml.getFirstChildItem("fpu");
	if (pNode == NULL)
	{
		m_errMsg = "�����ļ���,head�ڵ�����[fpu]�ڵ�";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ļ��� %s",(char *)m_errMsg.c_str());
		return false;
	}
	m_xmlconf.m_xml.setCurrentNode(pNode);
	pChildNode = m_xmlconf.m_xml.getFirstChildItem("fpuport");
	if (pChildNode == NULL)
	{
		m_errMsg = "�����ļ���,head�ڵ��µ�fpu�ڵ���[fpuport]�ڵ�";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ļ��� %s",(char *)m_errMsg.c_str());
		return false;
	}
	g_nBuPort = atoi(pChildNode->getValue().c_str());
	if (g_nBuPort <1024 || g_nBuPort == g_nPort)
	{
		m_errMsg = "�����ļ���,head�ڵ��µ�fpu�ڵ�[fpuport]�ڵ����ò��Ϸ�";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ļ��� %s",(char *)m_errMsg.c_str());
		return false;
	}
	m_xmlconf.m_xml.setCurrentNode(pNode);
	pChildNode = m_xmlconf.m_xml.getFirstChildItem("startnum");
	if (pChildNode == NULL)
	{
		m_errMsg = "�����ļ���,head�ڵ��µ�fpu�ڵ�����[startnum]�ڵ�";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ļ��� %s",(char *)m_errMsg.c_str());
		return false;
	}
	g_nProcessThreadNum = atoi(pChildNode->getValue().c_str());
	
	m_xmlconf.m_xml.setCurrentNode(pNode);
	pChildNode = m_xmlconf.m_xml.getFirstChildItem("autoloadfpu");
	if (pChildNode == NULL)
	{
		m_errMsg = "�����ļ���,head�ڵ��µ�fpu�ڵ���[autoloadfpu]�ڵ�";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ļ��� %s",(char *)m_errMsg.c_str());
		return false;
	}
	string ss;
	pChildNode->GetAttributeValue("use",ss);
	if (atoi(ss.c_str())== 1)
	{
		g_bIsAutoLoadBu = true;
	}
	else
	{
		g_bIsAutoLoadBu = false;
	}
	if (g_bIsAutoLoadBu)
	{
		m_xmlconf.m_xml.setCurrentNode(pChildNode);
		pNode = m_xmlconf.m_xml.getFirstChildItem("fpuprgname");
		if (pNode == NULL)
		{
			m_errMsg = "�����ļ���,head�ڵ��µ�fpu�ڵ��µ�autoloadfpu��[fpuprgname]�ڵ�";
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ļ��� %s",(char *)m_errMsg.c_str());
			return false;
		}
		g_sBuPrgName = pNode->getValue();
		
		m_xmlconf.m_xml.setCurrentNode(pChildNode);
		pNode = m_xmlconf.m_xml.getFirstChildItem("fpuprgpath");
		if (pNode == NULL)
		{
			m_errMsg = "�����ļ���,head�ڵ��µ�fpu�ڵ��µ�autoloadfpu��[fpuprgpath]�ڵ�";
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ļ��� %s",(char *)m_errMsg.c_str());
			return false;
		}
		g_sBuPrgPath = pNode->getValue();
		
		m_xmlconf.m_xml.setCurrentNode(pChildNode);
		pNode = m_xmlconf.m_xml.getFirstChildItem("fpurunpath");
		if (pNode == NULL)
		{
			m_errMsg = "�����ļ���,head�ڵ��µ�fpu�ڵ��µ�autoloadfpu��[fpurunpath]�ڵ�";
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ļ��� %s",(char *)m_errMsg.c_str());
			return false;
		}
		g_sBuRunPath = pNode->getValue();

		m_xmlconf.m_xml.setCurrentNode(pChildNode);
		pNode = m_xmlconf.m_xml.getFirstChildItem("fpupara");
		if (pNode == NULL)
		{
			m_errMsg = "�����ļ���,head�ڵ��µ�fpu�ڵ��µ�autoloadfpu��[fpupara]�ڵ�";
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ļ��� %s",(char *)m_errMsg.c_str());
			return false;
		}
		g_sBuPrgPara = pNode->getValue();
	}

	char tmpchar[40];
	sprintf(tmpchar,"%d",g_nLoglevel);
	m_errMsg = "";
	m_errMsg = m_errMsg+ "��־����:" + tmpchar ; 
	m_errMsg = m_errMsg + " ��־�ļ�:" + g_sLogFileName ;
	sprintf(tmpchar,"%d",g_nPort);
	m_errMsg = m_errMsg + " �����˿�:" + tmpchar ;
	sprintf(tmpchar,"%d",g_nHeadNum);
	m_errMsg = m_errMsg + " ͨѶ����ͷ����:" + tmpchar ;
		

	sprintf(tmpchar,"%d",g_nMaxConnectNum);
	m_errMsg = m_errMsg +" ���������:" + tmpchar ;
	sprintf(tmpchar,"%d",g_nDisconnectTime);
	m_errMsg = m_errMsg +" δʹ�öϿ�ʱ��(��):" + tmpchar ;
	sprintf(tmpchar,"%d",g_nHeartRun);
	m_errMsg = m_errMsg +" ����ʱ��(��):" + tmpchar ;
	

	m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ǰ�����в��� %s" ,(char *)m_errMsg.c_str());

	return true;
}

std::string CGResource::GetMsg()
{
	return m_errMsg;
}

bool CGResource::OpenDll(DLLHANDLE *pDll, const char *dllname,const char *dllpath)
{
	bool bIsNotPath=false;
	char dllpathname[801];
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
		sprintf(dllpathname,"%s/%s",dllpath,dllname);
	}
	
	m_pTool.LRtrim(dllpathname);
	
	*pDll = LOADDLL(dllpathname);
#ifdef WIN32
	if (ISNOTOPEN(*pDll))
	{
		if (bIsNotPath)
		{
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�򿪶�̬��ʧ�� %s",dllpathname);
			return false;
		}
		sprintf(dllpathname,"%s",dllname);
		m_pTool.LRtrim(dllpathname);
		*pDll = LOADDLL(dllpathname);
		if (ISNOTOPEN(*pDll))
		{
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�򿪶�̬��ʧ�� %s",dllpathname);
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
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�򿪶�̬��ʧ�� %s",dllpathname);
			return false;
		}
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�򿪶�̬��ʧ�� %s %s",dllpathname,pdllerror);
		sprintf(dllpathname,"%s",dllname);
		m_pTool.LRtrim(dllpathname);
		*pDll = LOADDLL(dllpathname);
		pdllerror = dlerror();
		if (pdllerror!=NULL)
		{
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�򿪶�̬��ʧ�� %s %s",dllpathname,pdllerror);
			return false;
		}
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�򿪶�̬��ɹ� %s",dllpathname);
		
	}
#endif	
	return true;
}


bool CGResource::SetInitOnce()
{
	CBF_PMutex pp(&m_mutex);
	if (!g_bIsInitOnce)
	{
		g_bIsInitOnce = true;
		return g_bIsInitOnce;
	}
	else
	{
		return false;
	}
}


bool CGResource::SetListen()
{
	CBF_PMutex pp(&m_mutex);
	if (!g_bIsListen)
	{
		g_bIsListen = true;
		return g_bIsListen;
	}
	else
	{
		return false;
	}
}

unsigned long CGResource::GetGenerateSerial()
{
	return m_serial_connect*m_nSerialNum;
}


int CGResource::PutProcessNum()
{
	if (g_bIsMonitor)
	{
//		CPMUTEX pp(&m_mutex_putprocess);
		if (g_processNum >= m_nMaxSerial)
		{
			g_processNum = 1; 
		}
		else
		{
			g_processNum++;
		}
	}
	return 0;
}
int CGResource::PutProcessSucessNum()
{
	if (g_bIsMonitor)
	{
//		CPMUTEX pp(&m_mutex_putprocesssucess);
		if (g_processSucessNum >= m_nMaxSerial)
		{
			g_processSucessNum = 1; 
		}
		else
		{
			g_processSucessNum++;
		}
	}
	return 0;
}
/// ������: SetGPara1
/// ���  : ������ 2008-12-30 9:42:48
/// ����  : virtual bool 
/// ����  : bool setflag=true
/// ����  : ����ȫ�ֱ���1,���������򷵻�false
bool CGResource::SetGPara1(bool setflag)
{
	CBF_PMutex pp(&m_mutex_setgpara1);
	if (m_bisSetGPara1)//������
	{
		return false;
	}
	m_bisSetGPara1 = setflag;
	return true;
}


/// ������: isSetGPara2
/// ���  : ������ 2008-12-30 9:36:10
/// ����  : virtual bool 
/// ����  : bool setflag=true
/// ����  : ����ȫ�ֱ���2,���������򷵻�false
bool CGResource::SetGPara2(bool setflag)
{
	CBF_PMutex pp(&m_mutex_setgpara2);
	if (m_bisSetGPara2)//������
	{
		return false;
	}
	m_bisSetGPara2 = setflag;
	return true;
}

/// ������: isSetGPara3
/// ���  : ������ 2008-12-30 9:36:13
/// ����  : virtual bool 
/// ����  : bool setflag=true
/// ����  : ����ȫ�ֱ���3,���������򷵻�false
bool CGResource::SetGPara3(bool setflag)
{
	CBF_PMutex pp(&m_mutex_setgpara3);
	if (m_bisSetGPara3)//������
	{
		return false;
	}
	m_bisSetGPara3 = setflag;
	return true;
}
long CGResource::GetThreadShareSerial()
{
	CBF_PMutex pp(&m_mutex_setgpara3);
	if (m_nSerial_get>=m_nSerial_getMax)//������
	{
		m_nSerial_get = 1;
	}
	else
	{
		m_nSerial_get++;
	}
	return m_nSerial_get;
	
}

void CGResource::WriteBack()
{
	m_xmlconf.gotoHead();
	CNode *tmpnode =m_xmlconf.m_xml.getFirstChildItem("ǰ������");
	
	if (tmpnode != NULL)
	{
		char tmpchar[100];
		sprintf(tmpchar,"%ld",m_nSerial_get);
		tmpnode = m_xmlconf.m_xml.getFirstChildItem("������ˮ��ʼֵ");
		if (tmpnode != NULL)
		{
			tmpnode->setNodeValue(tmpchar);
		}
		else
		{
			m_xmlconf.m_xml.addItem("������ˮ��ʼֵ",tmpchar);
		}
		printf("������д�������ļ�[%s]\n",m_frontParaFileName.c_str());
		m_xmlconf.toFile((char *)m_frontParaFileName.c_str());
		
	}
}

int CGResource::GetHeadLenByIp(const char *ip)
{
	int nport=0;
	for (int i=0;i<m_ip_head_list.size();i++)
	{
		if (strcmp(m_ip_head_list[i].sip,"default") == 0)
		{
			nport = m_ip_head_list[i].nLen;
			continue;
		}
		if (strcmp(m_ip_head_list[i].sip,ip)==0)
		{
			return m_ip_head_list[i].nLen;
		}
	}
	return nport;
}
