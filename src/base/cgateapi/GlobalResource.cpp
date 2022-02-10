// GlobalResource.cpp: implementation of the CGlobalResource class.
//
//////////////////////////////////////////////////////////////////////

#include "GlobalResource.h"
#include "XmlHead.h"
#include "BF_Des.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define  DBPWDKEY  "rd402wms"
typedef CGlobalResource *(*NEWBUMODULEINSTANCE)();

CGlobalResource::CGlobalResource()
{
	g_nTimeOutSend = 10; //发送数据超时时间 秒 当数据在队列里超过此时间后不发送
	g_nDataSendNum = 10; //数据发送次数
	g_nFlowControlSendBytes = 1024*1024*1; //1M
	g_nFlowControlSendTxNum = 100;
	g_nUseCert = 0;
	g_pCertClass = NULL;
	m_pDll = NULL;
	g_nHeartTime = 10; //心跳时间  隔多长时间未收到网关数据主动发送心跳
	g_nLogLevel = 5;//日志级别
	bzero(g_sCurPath,sizeof(g_sCurPath));
	g_nCrcFlag=0;
	g_nConnGateTimeOut = 5;
	g_nZipFlag = 1;
}

CGlobalResource::~CGlobalResource()
{
	if (m_pDll != NULL)
	{
		FREEDLL(m_pDll);
		m_pDll = NULL;
	}
	if (g_pCertClass != NULL)
	{
		delete g_pCertClass;
		g_pCertClass = NULL;
	}
	g_pLog.StopLog();
	g_pCgateLog.StopLog();
}

bool CGlobalResource::Init(const char *apixml)
{
	WORKPATH(g_sCurPath);
	if (!g_pXml.FromFile(apixml))
	{
		sprintf(m_sErrMsg,"配置[%s]文件不存在或非xml格式",apixml);
		return false;
	}
// <?xml version="1.0" encoding="GB2312"?>
// <网关API>
// <日志>
// <日志级别 remark="日志跟踪级别0严重错误1重要错误2一般错误3警告4提示警告5调试信息">5</日志级别>
// <日志文件名 remark="记录日志信息的文件名,前加日期">cgateapi.log</日志文件名>
// <日志目录 remark="存放日志的目录"></日志目录>
// </日志>
// <网关  心跳时间="30" remark="通讯时发送心跳的间隔 秒">
// <gate1  ip="" port="" remark="网关服务器的IP及端口" />
// <gate1  ip="" port="" remark="网关服务器的IP及端口" />
// </网关>
// <证书 use="0" dllpath="/home/songfree/lib" dllname="shcaapi.dll" method="getInstance" remark="是否使用证书及证书动态库所在的目录,method为实例化方法名">
// </证书>
// </网关API>
	g_pLog.StopLog();
	g_pCgateLog.StopLog();
	if (g_pXml.GetNodeValueByPath("/package/head/public/loglevel",false,g_nLogLevel) == NULL)
	{
		g_nLogLevel = 5;
	}
	if (g_pXml.GetNodeValueByPath("/package/head/public/logfile",false,g_sLogName,false) == NULL) 
	{
		sprintf(m_sErrMsg,"xml配置文件节点[/package/head/public/logfile]未配置");
		return false;
	}
	if (g_pXml.GetNodeValueByPath("//package/head/public/logdir",false,g_sLogPath,false) == NULL) 
	{
		sprintf(m_sErrMsg,"xml配置文件节点[/package/head/public/logdir]未配置");
		return false;
	}
	if (g_pXml.GetNodeValueByPath("/package/head/public/crc",false,g_nCrcFlag) == NULL)
	{
		g_nCrcFlag=0;
	}
	if (g_pXml.GetNodeValueByPath("/package/head/public/compress",false,g_nZipFlag) == NULL)
	{
		g_nZipFlag=1;
	}

	if (g_pXml.GetNodeValueByPath("/package/head/public/logsize",false,g_nMaxLogSize) == NULL)
	{
		g_nMaxLogSize = 0;
	}
	g_pLog.SetMaxLogSize(g_nMaxLogSize);
	g_pLog.SetLogPara(g_nLogLevel,g_sLogPath.c_str(),g_sLogName.c_str());
	g_pLog.StartLog();

	if (g_pXml.GetNodeValueByPath("/package/head/public/datalogsize",false,g_nMaxDataLogSize) == NULL)
	{
		g_nMaxLogSize = 0;
	}
	if (g_pXml.GetNodeValueByPath("/package/head/public/dataloglevel",false,g_nDataLogLevel) == NULL)
	{
		sprintf(m_sErrMsg,"xml配置文件节点[/package/head/public/dataloglevel]未配置");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	g_pCgateLog.SetLogPara(g_nMaxDataLogSize,g_nDataLogLevel,g_sLogPath.c_str(),"CGATE.dat");
	g_pCgateLog.StartLog();

	CXmlNode *gatenode = g_pXml.GetNodeByPath("/package/head/gate",false);
	if (gatenode == NULL)
	{
		sprintf(m_sErrMsg,"xml配置文件节点[/package/head/gate]未配置");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	std::string strtmp;
	if (gatenode->GetAttribute("hearttime",false,strtmp,false) == NULL)
	{
		sprintf(m_sErrMsg,"xml配置文件节点[/package/head/gate]的属性[hearttime]未配置");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	g_nHeartTime = atoi(strtmp.c_str());
	if (g_nHeartTime < 5)
	{
		g_nHeartTime = 5;
		sprintf(m_sErrMsg,"xml配置文件节点[/package/head/gate]的属性[hearttime]配置错误，取默认最小值5秒");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
	}
	if (gatenode->GetAttribute("disconnecttime",false,strtmp,false) == NULL)
	{
		sprintf(m_sErrMsg,"xml配置文件节点[/package/head/gate]的属性[disconnecttime]未配置");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	g_nCloseTime = atoi(strtmp.c_str());
	if (g_nCloseTime < g_nHeartTime+5)
	{
		g_nCloseTime = g_nHeartTime+5;
		g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"断开时间设置错，取默认值:[%d]心跳时间+5秒",g_nCloseTime);
	}
	
	if (gatenode->GetAttribute("timeout",false,strtmp,false) == NULL)
	{
		sprintf(m_sErrMsg,"xml配置文件节点[/package/head/gate]的属性[timeout]未配置，默认5秒");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		g_nConnGateTimeOut = 5;
	}
	else
	{
		g_nConnGateTimeOut = atoi(strtmp.c_str());
		if (g_nConnGateTimeOut < 3)
		{
			g_nConnGateTimeOut = 3;
		}
	}
	

	
	CXmlNode *gateipportnode = (CXmlNode *)gatenode->GetFirstChild();
	if (gateipportnode == NULL)
	{
		sprintf(m_sErrMsg,"xml配置文件节点[/package/head/gate]未配置网关IP及端口子节点");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	GATE_IPPORT oneip;
	g_lGateList.clear();
	while (gateipportnode != NULL)
	{
		if (gateipportnode->GetNodeType() != NODETYPE_VALUE && gateipportnode->GetNodeType() != NODETYPE_ATTR)
		{
			gateipportnode = (CXmlNode *)gateipportnode->getNextSibling();
			continue;
		}
		if (gateipportnode->GetAttribute("ip",false,oneip.sIp,false) == NULL)
		{
			sprintf(m_sErrMsg,"xml配置文件节点[/package/head/gate/%s]未配置[ip]属性",gateipportnode->GetNodeName().c_str());
			g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
		if (gateipportnode->GetAttribute("port",false,strtmp,false) == NULL)
		{
			sprintf(m_sErrMsg,"xml配置文件节点[/package/head/gate/%s]未配置[port]属性",gateipportnode->GetNodeName().c_str());
			g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
		oneip.nPort = atoi(strtmp.c_str());
		g_lGateList.push_back(oneip);
		gateipportnode = (CXmlNode *)gateipportnode->getNextSibling();
	}
	if (g_lGateList.size() <1 )
	{
		sprintf(m_sErrMsg,"xml配置文件节点[/package/head/gate]未配置网关IP及端口子节点");
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
				sprintf(m_sErrMsg,g_pCertClass->GetErrMsg().c_str());
				g_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,m_sErrMsg);
				return false;
			}
		}
		catch (...)
		{
			g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"调用认证动态库[%s]初始化异常出错",sdllname.c_str());
			return false;
		}
		
	}
	return true;
}

bool CGlobalResource::IsControlBytes()
{
	if (g_pApiFlowInfo.GetSpeedRateBytes() > g_nFlowControlSendBytes)
	{
		return true;
	}
	return false;
}

bool CGlobalResource::IsControlTx()
{
	if (g_pApiFlowInfo.GetSpeedRateTx() > g_nFlowControlSendTxNum)
	{
		return true;
	}
	return false;
}

std::string CGlobalResource::GetMsg()
{
	return m_sErrMsg;
}
bool CGlobalResource::OpenDll(DLLHANDLE *pDll, const char *dllname, const char *dllpath)
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