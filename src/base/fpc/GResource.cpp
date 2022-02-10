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
	g_nPort = 9001;//侦听端口 默认为9001
	g_nLoglevel = 5;//日志级别 默认为5
	g_nLogSize=0;
	
	g_nProcessThreadNum = 10;//处理线程数,默认为10

	
	
	g_nMaxConnectNum = 0;//最大连接数 0为无限制，默认
	g_nDisconnectTime = 600;//未使用断开时间，如果一个连接在此时间内一直没有使用，则将此连接断开。单位秒默认600秒即10分钟
	g_nDispatchTime = 10;
	g_nHeadNum = 5;//通讯报文头长度，默认为5
	g_nHeartRun = 600;//心跳时间  

	g_nQueueDelayTime= 10;//队列超时间

	g_bIsInitOnce = false; //是否初始化一次，即在一个线程里进行初始化后，将此标志置为1，其它线程判断此标志是否进行初始化
	g_bIsListen = false;//只能侦听一次，侦听完后将此标志置为true,其它线程将不用侦听

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
		m_errMsg = "传入文件为空";
		return false;
	}
	if (m_xmlconf.fromFile(confile)<0)
	{
		printf("传入文件非法,配置文件[%s]\n",confile);
		m_errMsg = "传入配置文件非法";
		return false;
	}

	
	if (m_xmlconf.gotoHead()<0)
	{
		m_errMsg = "初始化传入的XML节点head为NULL";
		return false;
	}

	int ret=0;
	std::string stmp;
	CNode *pNode =NULL;
	CNode *pChildNode =NULL;
	pNode = m_xmlconf.m_xml.getFirstChildItem("public");
	if (pNode == NULL)
	{
		m_errMsg = "配置文件错,head节点下无[public]节点";
		return false;
	}
	pChildNode = m_xmlconf.m_xml.getFirstChildItem("loglevel");
	if (pChildNode == NULL)
	{
		m_errMsg = "配置文件错,head节点下的public节点无[loglevel]节点";
		return false;
	}
	g_nLoglevel = atoi(pChildNode->getValue().c_str());
	if (g_nLoglevel <0 )
	{
		g_nLoglevel = LOG_DEBUG;//配置错则为5
	}
	m_xmlconf.m_xml.setCurrentNode(pNode);
	pChildNode = m_xmlconf.m_xml.getFirstChildItem("logfile");
	if (pChildNode == NULL)
	{
		m_errMsg = "配置文件错,head节点下的public节点无[logfile]节点";
//		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
		return false;
	}
	
	g_sLogFileName = pChildNode->getValue();
	if (g_sLogFileName.length()<1)
	{
		m_errMsg = "配置文件错,head节点下的public节点下[日志文件]节点没有配置";
//		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
		return false;
	}
	m_xmlconf.m_xml.setCurrentNode(pNode);
	pChildNode = m_xmlconf.m_xml.getFirstChildItem("logdir");
	if (pChildNode == NULL)
	{
		m_errMsg = "配置文件错,head节点下的public节点无[logdir]节点";
//		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
		return false;
	}
	g_sLogFilePath = pChildNode->getValue();

	m_xmlconf.m_xml.setCurrentNode(pNode);
	pChildNode = m_xmlconf.m_xml.getFirstChildItem("logsize");
	if (pChildNode == NULL)
	{
		m_errMsg = "配置文件错,head节点下的public节点无[logsize]节点";
//		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
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
		m_errMsg = "配置文件错,head节点下无[front]节点";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
		return false;
	}
	pChildNode = m_xmlconf.m_xml.getFirstChildItem("listenport");
	if (pChildNode == NULL)
	{
		m_errMsg = "配置文件错,head节点下的front节点无[listenport]节点";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
		return false;
	}
	g_nPort = atoi(pChildNode->getValue().c_str());
	if (g_nPort <= 1024)
	{
		m_errMsg = "配置文件错,head节点下的front节点[listenport]节点配置不合法";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
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
		m_errMsg = "配置文件错,head节点下的front节点无[commheadlen]节点";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
		return false;
	}
	g_nHeadNum = atoi(pChildNode->getValue().c_str());
	if (g_nHeadNum < 1)
	{
		g_nHeadNum = 0;
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件 %s","通讯报文头长度取默认最小值0");
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
		m_errMsg = "配置文件错,head节点下的front节点无[maxconnectnum]节点";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
		return false;
	}
	g_nMaxConnectNum = atoi(pChildNode->getValue().c_str());
	if (g_nMaxConnectNum <100 )
	{
		g_nMaxConnectNum = 100;
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件 %s","最大连接数取默认最小值100");
	}
	m_xmlconf.m_xml.setCurrentNode(pNode);
	pChildNode = m_xmlconf.m_xml.getFirstChildItem("disconnecttime");
	if (pChildNode == NULL)
	{
		m_errMsg = "配置文件错,head节点下的front节点无[disconnecttime]节点";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
		return false;
	}
	g_nDisconnectTime = atoi(pChildNode->getValue().c_str());
	if (g_nDisconnectTime < 6 )
	{
		g_nDisconnectTime = 6;
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件 %s","未使用断开时间取默认最小值6");
	}
	
	m_xmlconf.gotoHead();
	pNode = m_xmlconf.m_xml.getFirstChildItem("fpu");
	if (pNode == NULL)
	{
		m_errMsg = "配置文件错,head节点下无[fpu]节点";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
		return false;
	}
	m_xmlconf.m_xml.setCurrentNode(pNode);
	pChildNode = m_xmlconf.m_xml.getFirstChildItem("fpuport");
	if (pChildNode == NULL)
	{
		m_errMsg = "配置文件错,head节点下的fpu节点无[fpuport]节点";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
		return false;
	}
	g_nBuPort = atoi(pChildNode->getValue().c_str());
	if (g_nBuPort <1024 || g_nBuPort == g_nPort)
	{
		m_errMsg = "配置文件错,head节点下的fpu节点[fpuport]节点配置不合法";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
		return false;
	}
	m_xmlconf.m_xml.setCurrentNode(pNode);
	pChildNode = m_xmlconf.m_xml.getFirstChildItem("startnum");
	if (pChildNode == NULL)
	{
		m_errMsg = "配置文件错,head节点下的fpu节点下无[startnum]节点";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
		return false;
	}
	g_nProcessThreadNum = atoi(pChildNode->getValue().c_str());
	
	m_xmlconf.m_xml.setCurrentNode(pNode);
	pChildNode = m_xmlconf.m_xml.getFirstChildItem("autoloadfpu");
	if (pChildNode == NULL)
	{
		m_errMsg = "配置文件错,head节点下的fpu节点无[autoloadfpu]节点";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
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
			m_errMsg = "配置文件错,head节点下的fpu节点下的autoloadfpu无[fpuprgname]节点";
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
			return false;
		}
		g_sBuPrgName = pNode->getValue();
		
		m_xmlconf.m_xml.setCurrentNode(pChildNode);
		pNode = m_xmlconf.m_xml.getFirstChildItem("fpuprgpath");
		if (pNode == NULL)
		{
			m_errMsg = "配置文件错,head节点下的fpu节点下的autoloadfpu无[fpuprgpath]节点";
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
			return false;
		}
		g_sBuPrgPath = pNode->getValue();
		
		m_xmlconf.m_xml.setCurrentNode(pChildNode);
		pNode = m_xmlconf.m_xml.getFirstChildItem("fpurunpath");
		if (pNode == NULL)
		{
			m_errMsg = "配置文件错,head节点下的fpu节点下的autoloadfpu无[fpurunpath]节点";
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
			return false;
		}
		g_sBuRunPath = pNode->getValue();

		m_xmlconf.m_xml.setCurrentNode(pChildNode);
		pNode = m_xmlconf.m_xml.getFirstChildItem("fpupara");
		if (pNode == NULL)
		{
			m_errMsg = "配置文件错,head节点下的fpu节点下的autoloadfpu无[fpupara]节点";
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
			return false;
		}
		g_sBuPrgPara = pNode->getValue();
	}

	char tmpchar[40];
	sprintf(tmpchar,"%d",g_nLoglevel);
	m_errMsg = "";
	m_errMsg = m_errMsg+ "日志级别:" + tmpchar ; 
	m_errMsg = m_errMsg + " 日志文件:" + g_sLogFileName ;
	sprintf(tmpchar,"%d",g_nPort);
	m_errMsg = m_errMsg + " 侦听端口:" + tmpchar ;
	sprintf(tmpchar,"%d",g_nHeadNum);
	m_errMsg = m_errMsg + " 通讯报文头长度:" + tmpchar ;
		

	sprintf(tmpchar,"%d",g_nMaxConnectNum);
	m_errMsg = m_errMsg +" 最大连接数:" + tmpchar ;
	sprintf(tmpchar,"%d",g_nDisconnectTime);
	m_errMsg = m_errMsg +" 未使用断开时间(秒):" + tmpchar ;
	sprintf(tmpchar,"%d",g_nHeartRun);
	m_errMsg = m_errMsg +" 心跳时间(秒):" + tmpchar ;
	

	m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"前置运行参数 %s" ,(char *)m_errMsg.c_str());

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
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"打开动态库失败 %s",dllpathname);
			return false;
		}
		sprintf(dllpathname,"%s",dllname);
		m_pTool.LRtrim(dllpathname);
		*pDll = LOADDLL(dllpathname);
		if (ISNOTOPEN(*pDll))
		{
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"打开动态库失败 %s",dllpathname);
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
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"打开动态库失败 %s",dllpathname);
			return false;
		}
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"打开动态库失败 %s %s",dllpathname,pdllerror);
		sprintf(dllpathname,"%s",dllname);
		m_pTool.LRtrim(dllpathname);
		*pDll = LOADDLL(dllpathname);
		pdllerror = dlerror();
		if (pdllerror!=NULL)
		{
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"打开动态库失败 %s %s",dllpathname,pdllerror);
			return false;
		}
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"打开动态库成功 %s",dllpathname);
		
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
/// 函数名: SetGPara1
/// 编程  : 王明松 2008-12-30 9:42:48
/// 返回  : virtual bool 
/// 参数  : bool setflag=true
/// 描述  : 设置全局变量1,若已设置则返回false
bool CGResource::SetGPara1(bool setflag)
{
	CBF_PMutex pp(&m_mutex_setgpara1);
	if (m_bisSetGPara1)//已设置
	{
		return false;
	}
	m_bisSetGPara1 = setflag;
	return true;
}


/// 函数名: isSetGPara2
/// 编程  : 王明松 2008-12-30 9:36:10
/// 返回  : virtual bool 
/// 参数  : bool setflag=true
/// 描述  : 设置全局变量2,若已设置则返回false
bool CGResource::SetGPara2(bool setflag)
{
	CBF_PMutex pp(&m_mutex_setgpara2);
	if (m_bisSetGPara2)//已设置
	{
		return false;
	}
	m_bisSetGPara2 = setflag;
	return true;
}

/// 函数名: isSetGPara3
/// 编程  : 王明松 2008-12-30 9:36:13
/// 返回  : virtual bool 
/// 参数  : bool setflag=true
/// 描述  : 设置全局变量3,若已设置则返回false
bool CGResource::SetGPara3(bool setflag)
{
	CBF_PMutex pp(&m_mutex_setgpara3);
	if (m_bisSetGPara3)//已设置
	{
		return false;
	}
	m_bisSetGPara3 = setflag;
	return true;
}
long CGResource::GetThreadShareSerial()
{
	CBF_PMutex pp(&m_mutex_setgpara3);
	if (m_nSerial_get>=m_nSerial_getMax)//已设置
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
	CNode *tmpnode =m_xmlconf.m_xml.getFirstChildItem("前置配置");
	
	if (tmpnode != NULL)
	{
		char tmpchar[100];
		sprintf(tmpchar,"%ld",m_nSerial_get);
		tmpnode = m_xmlconf.m_xml.getFirstChildItem("本次流水开始值");
		if (tmpnode != NULL)
		{
			tmpnode->setNodeValue(tmpchar);
		}
		else
		{
			m_xmlconf.m_xml.addItem("本次流水开始值",tmpchar);
		}
		printf("将配置写回配置文件[%s]\n",m_frontParaFileName.c_str());
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
