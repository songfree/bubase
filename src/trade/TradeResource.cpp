// TradeResource.cpp: implementation of the CTradeResource class.
//
//////////////////////////////////////////////////////////////////////

#include "TradeResource.h"
#include "BF_Slist.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTradeResource::CTradeResource()
{
	g_bToExit = false;
	m_nSvrHostStatus = ARBSTATUS_UNKNOW;
	m_nTradeReportTime = 1; //定时上报为1秒
	m_log = NULL;
	g_nMaxConnectNum = 10;
	
	g_nListenPort = 9001;
	g_nHeartRun = 5;
	m_nArbMode = 0;
	g_nProcTheadNum=5;
	g_nStatus = -1;
	
}

CTradeResource::~CTradeResource()
{
	m_log = NULL;
}

bool CTradeResource::Init(const char *confile)
{
	//中文配置文件
	//bool bRet = CBF_DrebResource::Init_CN(confile);
	//英文配置文件
	bool bRet = CBF_DrebResource::Init(confile);
	if (!bRet)
	{
		return false;
	}
	if (m_pXml.GetNodeValueByPath("/package/head/public/xdpfile",false,m_sXdpDefineXml,false) == NULL)
	{
		m_errMsg = "file error,no [/package/head/public/xdpfile] node";
		return false;
	}
	if (m_pXml.GetNodeValueByPath("/package/head/public/sendarbtime",false,m_nTradeReportTime) == NULL)
	{
		m_errMsg = "file error,no [/package/head/public/sendarbtime] node";
		return false;
	}
	
	if (m_pXml.GetNodeValueByPath("/package/head/public/mastermode",false,m_nArbMode) == NULL)
	{
		m_errMsg = "file error,no  [/package/head/public/mastermode] node";
		return false;
	}
	if (m_nArbMode == 1)
	{
		g_lSvrTxList.push_back(ARB_SEND_CONTROL);//仲裁指定主机
		g_lSvrTxList.push_back(ARB_SEND_QUERY);//仲裁查询
		g_lArbTxList.push_back(ARB_SEND_CONTROL);//仲裁指定主机
		g_lArbTxList.push_back(ARB_SEND_QUERY);//仲裁查询
		
	}
	else
	{
		m_nSvrHostStatus = MASTERHOST;
	}
	
	
	if (m_pXml.GetNodeValueByPath("/package/head/public/listenport",false,g_nListenPort) == NULL)
	{
		m_errMsg = "file error,no [/package/head/public/listenport] node";
		return false;
	}
	if (g_nListenPort <1024 || g_nListenPort > 65534)
	{
		m_errMsg = "[/package/head/public/listenport] is illegal";
		return false;
	}
	if (m_pXml.GetNodeValueByPath("/package/head/public/maxconnect",false,g_nMaxConnectNum) == NULL)
	{
		m_errMsg = "file error,no  [/package/head/public/maxconnect] node";
		return false;
	}
	if (g_nMaxConnectNum <100)
	{
		g_nMaxConnectNum = 100;
	}
	if (m_pXml.GetNodeValueByPath("/package/head/business/busi_thread_num",false,g_nProcTheadNum) == NULL)
	{
		m_errMsg = "file error,no [/package/head/business/busi_thread_num] node";
		return false;
	}
	if (g_nProcTheadNum<1)
	{
		g_nProcTheadNum=1;
	}
	if (g_nProcTheadNum>50)
	{
		g_nProcTheadNum =50;
	}
	return true;
}

long CTradeResource::GetLocalNo()
{
	CBF_PMutex pp(&m_mutex);
	m_pShmpub->nSerial_get++;
	if (m_pShmpub->nSerial_get<0 || m_pShmpub->nSerial_get > 999999999)
	{
		m_pShmpub->nSerial_get=1;
		return m_pShmpub->nSerial_get;
	}
	else
	{
		return m_pShmpub->nSerial_get;
	}
}
int CTradeResource::GetSerial()
{
	CBF_PMutex pp(&m_mutex);
	m_pShmpub->serial++;
	if (m_pShmpub->serial<0 || m_pShmpub->serial > 999999999)
	{
		m_pShmpub->serial=1;
		return m_pShmpub->serial;
	}
	else
	{
		return m_pShmpub->serial;
	}
}