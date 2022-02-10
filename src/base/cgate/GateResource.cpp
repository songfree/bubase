// GateResource.cpp: implementation of the CGateResource class.
//
//////////////////////////////////////////////////////////////////////

#include "GateResource.h"
#include "BF_Tools.h"
#include "XmlHead.h"
#include "BF_Des.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define  DBPWDKEY  "rd402wms"
typedef CGateResource *(*NEWBUMODULEINSTANCE)();

CGateResource::CGateResource()
{
	ReSetPara();
	m_pDll  = NULL;
	g_pCertClass = NULL;
	g_nUnCommpress = 0;
	g_nEncryptFlag = 1;
	bzero(m_sStartDate,sizeof(m_sStartDate));

	g_nQuoteQueueDeleteTime = 10;
	g_nSendQueThreadNum  = 2;
	g_nQuoSubScribe = 1;
	g_nBroadCastMode = 0;
}

CGateResource::~CGateResource()
{
	if (g_pCertClass != NULL)
	{
		delete g_pCertClass;
		g_pCertClass = NULL;
	}
	if (m_pDll != NULL)
	{
		FREEDLL(m_pDll);
		m_pDll = NULL;
	}
	g_pLog.StopLog();
	g_pDrebLog.StopLog();
	g_pGateLog.StopLog();

	
}
std::string CGateResource::GetMsg()
{
	return m_sErrMsg;
}
bool CGateResource::OpenDll(DLLHANDLE *pDll, const char *dllname, const char *dllpath)
{
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
			sprintf(m_sErrMsg,"打开动态库[%s]失败",dllpathname);
			g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
		sprintf(dllpathname,"%s",dllname);
		CBF_Tools::LRtrim(dllpathname);
		*pDll = LOADDLL(dllpathname);
		if (ISNOTOPEN(*pDll))
		{
			sprintf(m_sErrMsg,"打开动态库[%s]失败",dllpathname);
			g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
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
			sprintf(m_sErrMsg,"打开动态库[%s]失败",dllpathname);
			g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
		sprintf(dllpathname,"%s",dllname);
		CBF_Tools::LRtrim(dllpathname);
		*pDll = LOADDLL(dllpathname);
		pdllerror = dlerror();
		if (pdllerror!=NULL)
		{
			sprintf(m_sErrMsg,"打开动态库[%s]失败[%s]",dllpathname,pdllerror);
			g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
		
	}
#endif	
	return true;
}
bool CGateResource::Init(const char *confile)
{
// 	unsigned   int    g_nIoThreadNum;//IO线程数
// 	unsigned   int    g_nMaxConnectNum;//最大连接数
// 	unsigned   int    g_nRunMode;//运行模式   0-select模型  1-epoll模型(LINUX)   2-iocp模型(WINDOWS)
// 	unsigned   int    g_nPort;//IO模型的侦听端口
// 	int               g_nIocpAcceptNum;//iocp一次发AcceptEx的数目
// 	int g_nHeartRun;//心跳时间,当发现连接超过此时间未用时，主动发送心跳
// 	int g_nDisconnectTime;//未使用断开时间，如果一个连接在此时间内一直没有使用，则将此连接断开。单位秒默认600秒即10分钟
// 	int g_nDispatchTime;//分配最大时间
	if (!g_pXml.FromFile(confile))
	{
		sprintf(m_sErrMsg,"配置[%s]文件不存在或非xml格式",confile);
		return false;
	}
	//先停止日志线程
	g_pLog.StopLog();
	g_pDrebLog.StopLog();
	g_pGateLog.StopLog();

	if (g_pXml.GetNodeValueByPath("/package/head/public/loglevel",false,g_nLoglevel) == NULL)
	{
		g_nLoglevel = 5;
	}
	if (g_pXml.GetNodeValueByPath("/package/head/public/logfile",false,g_sLogFileName,false) == NULL) 
	{
		sprintf(m_sErrMsg,"xml配置文件节点[/package/head/public/logfile]未配置");
		return false;
	}
	if (g_pXml.GetNodeValueByPath("/package/head/public/logdir",false,g_sLogFilePath,false) == NULL) 
	{
		sprintf(m_sErrMsg,"xml配置文件节点[/package/head/public/logdir]未配置");
		return false;
	}
	if (g_pXml.GetNodeValueByPath("/package/head/public/CRC",false,g_nCrcFlag) == NULL)
	{
		
		g_nCrcFlag=0;
	}
	if (g_pXml.GetNodeValueByPath("/package/head/public/logsize",false,g_nMaxLogSize) == NULL) 
	{
		sprintf(m_sErrMsg,"xml配置文件节点[/package/head/public/logsize]未配置");
		return false;
	}
	if (g_nMaxLogSize<1024)
	{
		g_nMaxLogSize = 0;
	}
	
	g_pLog.SetMaxLogSize(g_nMaxLogSize);
	g_pLog.SetLogPara(g_nLoglevel,g_sLogFilePath.c_str(),g_sLogFileName.c_str());

	//启动日志线程
	g_pLog.StartLog();
	
	if (g_pXml.GetNodeValueByPath("/package/head/public/dataloglevel",false,g_nDataLoglevel) == NULL) 
	{
		sprintf(m_sErrMsg,"xml配置文件节点[/package/head/public/dataloglevel]未配置");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	if (g_pXml.GetNodeValueByPath("/package/head/public/datalogsize",false,g_nMaxDataLogSize) == NULL) 
	{
		sprintf(m_sErrMsg,"xml配置文件节点[/package/head/public/datalogsize]未配置");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	if (g_nMaxDataLogSize<1024)
	{
		g_nMaxDataLogSize = 0;
	}
	int tmpint=0;
	if (g_pXml.GetNodeValueByPath("/package/head/public/encrypt",false,g_nEncryptFlag) == NULL) 
	{
		sprintf(m_sErrMsg,"xml配置文件节点[/package/head/public/encrypt]未配置");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,"加密标志为%d",g_nEncryptFlag);
	if (g_pXml.GetNodeValueByPath("/package/head/public/compress",false,g_nZipFlag) == NULL) 
	{
		sprintf(m_sErrMsg,"xml配置文件节点[/package/head/public/compress]未配置");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,"压缩标志为%d",g_nZipFlag);
	if (g_pXml.GetNodeValueByPath("/package/head/public/uncompress",false,g_nUnCommpress) == NULL) 
	{
		sprintf(m_sErrMsg,"xml配置文件节点[/package/head/public/uncompress]未配置");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		g_nUnCommpress = 0;
	}
	
	if (g_pXml.GetNodeValueByPath("/package/head/public/quotduetime",false,g_nQuoteQueueDeleteTime) == NULL) 
	{
		sprintf(m_sErrMsg,"xml配置文件节点[/package/head/public/quotduetime]未配置 默认10秒 最小2秒");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		g_nQuoteQueueDeleteTime = 10;
	}
	if (g_nQuoteQueueDeleteTime < 2 )
	{
		g_nQuoteQueueDeleteTime =2;
	}

	if (g_pXml.GetNodeValueByPath("/package/head/public/quotnum",false,g_nQuoteQueueNum) == NULL) 
	{
		sprintf(m_sErrMsg,"xml配置文件节点[/package/head/public/quotnum]未配置 默认30个，最小10个");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		g_nQuoteQueueDeleteTime = 30;
	}
	if (g_nQuoteQueueNum < 10 )
	{
		g_nQuoteQueueNum = 10;
	}
	if (g_pXml.GetNodeValueByPath("/package/head/public/quotsubscribe",false,g_nQuoSubScribe) == NULL) 
	{
		sprintf(m_sErrMsg,"xml配置文件节点[/package/head/public/quotsubscribe]未配置 默认1支持");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		g_nQuoSubScribe = 1;
	}

	if (g_pXml.GetNodeValueByPath("/package/head/public/clientsendthreadnum",false,g_nSendQueThreadNum) == NULL) 
	{
		sprintf(m_sErrMsg,"xml配置文件节点[/package/head/public/clientsendthreadnum]未配置 默认1个，最大50个");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		g_nQuoteQueueDeleteTime = 1;
	}
	if (g_nQuoteQueueNum >50 )
	{
		g_nQuoteQueueNum = 50;
	}
	if (g_pXml.GetNodeValueByPath("/package/head/public/sendwindow",false,g_nSendOnceNum) == NULL) 
	{
		sprintf(m_sErrMsg,"xml配置文件节点[/package/head/public/sendwindow]未配置 默认16个，最大20个");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		g_nSendOnceNum = 16;
	}
	if (g_nSendOnceNum >20 )
	{
		g_nSendOnceNum = 20;
	}
	if (g_pXml.GetNodeValueByPath("/package/head/public/dispatchtime", false, g_nDispatchTime) == NULL)
	{
		g_nDispatchTime = 30;
		g_pLog.LogMp(LOG_ERROR, __FILE__, __LINE__, "消息分配时间默认30秒");
	}
	if (g_pXml.GetNodeValueByPath("/package/head/public/hearttime", false, g_nHeartRun) == NULL)
	{
		sprintf(m_sErrMsg, "xml配置文件节点[/package/head/public/hearttime]未配置");
		g_pLog.LogMp(LOG_ERROR, __FILE__, __LINE__, m_sErrMsg);
		return false;
	}
	if (g_pXml.GetNodeValueByPath("/package/head/public/disconnecttime", false, g_nDisconnectTime) == NULL)
	{
		sprintf(m_sErrMsg, "xml配置文件节点[/package/head/public/disconnecttime]未配置");
		g_pLog.LogMp(LOG_ERROR, __FILE__, __LINE__, m_sErrMsg);
		return false;
	}

	g_nUseMonitor=0;
	if (g_pXml.GetNodeAttribute("/package/head/monitor",false,"use",false,g_nUseMonitor) == NULL)
	{
		sprintf(m_sErrMsg,"无/package/head/monitor");
		g_nUseMonitor = 0;
	}
	if (g_nUseMonitor == 1)
	{
		if (g_pXml.GetNodeAttribute("/package/head/monitor",false,"host",false,g_nMonitorHost) == NULL)
		{
			sprintf(m_sErrMsg,"无/package/head/monitor");
			g_nMonitorHost = 1;
		}
		if (g_pXml.GetNodeValueByPath("/package/head/monitor/drebid",false,g_nMonitorDrebId) == NULL)
		{
			sprintf(m_sErrMsg,"节点[网关/monitor/drebid]没有配置");
			g_nMonitorDrebId = 0;
		}
		if (g_pXml.GetNodeValueByPath("/package/head/monitor/drebpid",false,g_nMonitorDrebPrivateId) == NULL)
		{
			sprintf(m_sErrMsg,"节点[网关/monitor/drebpid]没有配置");
			g_nMonitorDrebPrivateId = 0;
		}
		if (g_pXml.GetNodeValueByPath("/package/head/monitor/svrid",false,g_nMonitorSvrId) == NULL)
		{
			sprintf(m_sErrMsg,"节点[/package/head/monitor/svrid]没有配置");
			g_nMonitorSvrId = 0;
		}
		if (g_pXml.GetNodeValueByPath("/package/head/monitor/svrpid",false,g_nMonitorSvrPrivateId) == NULL)
		{
			sprintf(m_sErrMsg,"节点[/package/head/monitor/svrpid]没有配置");
			g_nMonitorSvrPrivateId = 0;
		}
		if (g_pXml.GetNodeValueByPath("/package/head/monitor/txcode",false,g_nMonitorTxCode) == NULL)
		{
			sprintf(m_sErrMsg,"节点[/package/head/monitor/txcode]没有配置");
			return false;
		}
	}
	
	if (g_pXml.GetNodeValueByPath("/package/head/iomode/runmode",false,tmpint) == NULL) 
	{
		sprintf(m_sErrMsg,"xml配置文件节点[/package/head/iomode/runmode]未配置");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	g_nRunMode = tmpint;
	if (g_pXml.GetNodeValueByPath("/package/head/iomode/listenport",false,tmpint) == NULL) 
	{
		sprintf(m_sErrMsg,"xml配置文件节点[/package/head/iomode/listenport]未配置");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	g_nPort = tmpint;
	if (g_pXml.GetNodeValueByPath("/package/head/iomode/maxconnectnum",false,tmpint) == NULL) 
	{
		sprintf(m_sErrMsg,"xml配置文件节点[/package/head/iomode/maxconnectnum]未配置");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	g_nMaxConnectNum = tmpint;
	if (g_pXml.GetNodeValueByPath("/package/head/iomode/iothread",false,tmpint) == NULL) 
	{
		sprintf(m_sErrMsg,"xml配置文件节点[/package/head/iomode/iothread]未配置");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	g_nIoThreadNum = tmpint;
	if (g_pXml.GetNodeValueByPath("/package/head/iomode/iocpacceptnum",false,tmpint) == NULL) 
	{
		sprintf(m_sErrMsg,"xml配置文件节点[/package/head/iomode/iocpacceptnum数]未配置");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		g_nIocpAcceptNum = 5;
	}
	g_nIocpAcceptNum = tmpint;
	if (g_pXml.GetNodeValueByPath("/package/head/bpc/svrid",false,tmpint) == NULL) 
	{
		sprintf(m_sErrMsg,"xml配置文件节点[/package/head/bpc/svrid]未配置");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	g_nSvrMainId = tmpint;
	if (g_pXml.GetNodeValueByPath("/package/head/bpc/svrpid",false,tmpint) == NULL) 
	{
		sprintf(m_sErrMsg,"xml配置文件节点[/package/head/bpc/svrpid]未配置");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	g_nSvrPrivateId = tmpint;
	
	char tmpchar1[100];
	char tmpchar2[100];
	sprintf(tmpchar1,"srv%d-%d.DREB2CGATE.dat",g_nSvrMainId,g_nSvrPrivateId);
	sprintf(tmpchar2,"srv%d-%d.BPC2BPU.dat",g_nSvrMainId,g_nSvrPrivateId);
	g_pDrebLog.SetLogPara(g_nMaxDataLogSize,g_nDataLoglevel,g_sLogFilePath.c_str(),tmpchar1,tmpchar2);

	sprintf(tmpchar1,"CGATE%d-%d.dat",g_nSvrMainId,g_nSvrPrivateId);
	g_pGateLog.SetLogPara(g_nMaxDataLogSize,g_nDataLoglevel,g_sLogFilePath.c_str(),tmpchar1);

	g_pDrebLog.StartLog();
	g_pGateLog.StartLog();

	CXmlNode *commnode = g_pXml.GetNodeByPath("/package/head/dreb",false);
	if (commnode == NULL)
	{
		sprintf(m_sErrMsg,"xml配置文件节点[/package/head/dreb]未配置");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	std::string strtmp;
	CXmlNode *drebnode = (CXmlNode *)commnode->GetFirstChild();
	if (drebnode == NULL)
	{
		sprintf(m_sErrMsg,"xml配置文件节点[/package/head/dreb]未配置总线信息");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	
	S_DREBLINK_INFO dinfo;
	g_vDrebLinkInfo.clear();
	while (drebnode != NULL)
	{
		if (drebnode->GetNodeType() != NODETYPE_VALUE && drebnode->GetNodeType() != NODETYPE_ATTR)
		{
			drebnode = (CXmlNode *)drebnode->getNextSibling();
			continue;
		}
		if (drebnode->GetAttribute("ip",false,dinfo.sIp,false) == NULL)
		{
			sprintf(m_sErrMsg,"xml配置文件节点[/package/head/dreb/%s]未配置[ip]属性",drebnode->GetNodeName().c_str());
			g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
		if (drebnode->GetAttribute("port",false,strtmp,false) == NULL)
		{
			sprintf(m_sErrMsg,"xml配置文件节点[/package/head/dreb/%s]未配置[port]属性",drebnode->GetNodeName().c_str());
			g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
		dinfo.nPort = atoi(strtmp.c_str());
		if (drebnode->GetAttribute("bandwidth",false,strtmp,false) == NULL)
		{
			sprintf(m_sErrMsg,"xml配置文件节点[/package/head/dreb/%s]未配置[bandwidth]属性",drebnode->GetNodeName().c_str());
			g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
		dinfo.nBandwidth = atoi(strtmp.c_str());
		g_vDrebLinkInfo.push_back(dinfo);
		drebnode = (CXmlNode *)drebnode->getNextSibling();
	}
	if (g_vDrebLinkInfo.size() <1 )
	{
		sprintf(m_sErrMsg,"xml配置文件节点[/package/head/dreb/list]未配置总节节点IP及端口子节点");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	//读取证书信息
	if (g_pXml.GetNodeAttribute("/package/head/pki",false,"use",false,g_nUseCert) == NULL) 
	{
		sprintf(m_sErrMsg,"xml配置文件节点[/package/head/pki]或其属性[use]未配置");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	//使用证书
	if (g_nUseCert == 1)
	{
		std::string sdllname;
		std::string sdllpath;
		std::string sdllmethod;
		std::string scertpath;
		std::string scertpwd;
		std::string scertpwdflag;
		char  certpwd[40];
		bzero(certpwd,sizeof(certpwd));
		if (g_pXml.GetNodeAttribute("/package/head/pki",false,"dllpath",false,sdllpath) == NULL) 
		{
			sprintf(m_sErrMsg,"xml配置文件节点[/package/head/pki]或其属性[dllpath]未配置");
			g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
		if (g_pXml.GetNodeAttribute("/package/head/pki",false,"dllname",false,sdllname) == NULL) 
		{
			sprintf(m_sErrMsg,"xml配置文件节点[/package/head/pki]或其属性[dllname]未配置");
			g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
		if (g_pXml.GetNodeAttribute("/package/head/pki",false,"method",false,sdllmethod) == NULL) 
		{
			sprintf(m_sErrMsg,"xml配置文件节点[/package/head/pki]或其属性[method]未配置");
			g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
		if (g_pXml.GetNodeAttribute("/package/head/pki",false,"certpath",false,scertpath) == NULL) 
		{
			sprintf(m_sErrMsg,"xml配置文件节点[/package/head/pki]或其属性[certpath]未配置");
			g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
		if (g_pXml.GetNodeAttribute("/package/head/pki",false,"certpwd",false,scertpwd) == NULL) 
		{
			sprintf(m_sErrMsg,"xml配置文件节点[/package/head/pki]或其属性[certpwd]未配置");
			g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
		if (g_pXml.GetNodeAttribute("/package/head/pki",false,"certpwdflag",false,scertpwdflag) == NULL) 
		{
			sprintf(m_sErrMsg,"xml配置文件节点[/package/head/pki]或其属性[certpwdflag]未配置");
			g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
		strcpy(certpwd,scertpwd.c_str());
		if (atoi(scertpwdflag.c_str()) ==1 )
		{
			CBF_Des pEncrypt;
			int len = strlen(certpwd);
			pEncrypt.UncryptString(certpwd,len,DBPWDKEY);
		}


		if (!OpenDll(&m_pDll,sdllname.c_str(),sdllpath.c_str()) )
		{
			sprintf(m_sErrMsg,"打开动态库[%s %s]失败",sdllpath.c_str(),sdllname.c_str());
			g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
		NEWBUMODULEINSTANCE baseinstance = (NEWBUMODULEINSTANCE)GETINSTANCE(m_pDll,sdllmethod.c_str());
		if (NULL == baseinstance)
		{
			sprintf(m_sErrMsg,"动态库[%s]实例化失败",sdllname.c_str());
			g_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
		
		g_pCertClass=(CICertApiBase *)baseinstance(); 
		if (g_pCertClass == NULL)
		{
			sprintf(m_sErrMsg,"动态库[%s]实例化失败",sdllname.c_str());
			g_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
		try
		{
			//认证动态库初始化
			if (!g_pCertClass->InitCertApi(scertpath.c_str(),certpwd)  )
			{
				sprintf(m_sErrMsg,"%s",g_pCertClass->GetErrMsg().c_str());
				g_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,m_sErrMsg);
				return false;
			}
		}
		catch (...)
		{
			sprintf(m_sErrMsg,"调用认证动态库[%s]初始化异常出错",sdllname.c_str());
			g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
		
	}

	//广播配置
	
	if (g_pXml.GetNodeAttribute("/package/head/broadcast",false,"use",false, g_nBroadCastMode) == NULL)
	{
		sprintf(m_sErrMsg,"/package/head/broadcast");
		g_nBroadCastMode = 0;
	}
	if (g_nBroadCastMode != 0)
	{
		CXmlNode *bcnode = g_pXml.GetNodeByPath("package/head/broadcast",false);
		if (bcnode != NULL)
		{
			CXmlNode *bcfuncnode = (CXmlNode *)bcnode->GetFirstChild();
			while (bcfuncnode != NULL)
			{
				g_lBCFuncList.Add(atoi(bcfuncnode->GetNodeValue().c_str()));
				bcfuncnode = (CXmlNode *)bcfuncnode->getNextSibling();
			}
		}
	}
	if (!SetRunMode(g_nRunMode))
	{
		sprintf(m_sErrMsg,"运行模式不符 [%d]",g_nRunMode);
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	SetIoThreadNum(g_nIoThreadNum);
	CBF_Date_Time pDate;
	std::string ss = pDate.ToStringDT17();
	sprintf(m_sStartDate,"%s",ss.c_str());
	return true;
}


