// GateWay.cpp: implementation of the CGateWay class.
//
//////////////////////////////////////////////////////////////////////

#include "GateWay.h"
#include "Bpc_Timer.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern CBpc_Timer  g_pBpcTime;

CGateWay::CGateWay()
{

}

CGateWay::~CGateWay()
{

}

bool CGateWay::Init(const char *confile)
{
	printf("%s\n",confile);
	if (!g_pRes.Init(confile))
	{
		sprintf(m_sErrMsg,"初始化配置文件[%s]出错",confile);
		return false;
	}
	
	//初始化 dreb连接缓冲池
	m_pMemPool.Init(100,DREBBUFSIZE);
	m_pPoolData.m_pRes = &g_pRes;
	m_pLog = &(g_pRes.g_pLog);
	m_pSendData.m_pRes = &g_pRes;

	//初始化dreb连接信息
	if (!m_pSockMgr.Init(&g_pRes,&m_pMemPool))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"%s",m_pSockMgr.m_errMsg);
		return false;
	}
	//设置参数
	g_pDrebLink.SetGlobalVar(&g_pRes,&m_pPoolData,&m_pMemPool,&m_pSockMgr,&m_pSendData,&g_pAioWork);

	g_pDrebLink.m_nBegin = 0;
	g_pDrebLink.m_nEnd = g_pRes.g_vDrebLinkInfo.size();

	//设置IO参数
	g_pAioRun.SetParameter(&g_pAioWork,&g_pRes);
	
	//设置消息线程参数
	m_pMsgThread.SetGlobalVar(&g_pRes,&m_pPoolData,&m_pMemPool,&m_pSockMgr);
	
	g_pAioWork.SetGlobalVar(&g_pRes,&m_pPoolData,&m_pMemPool,&m_pSendData);


	for (int i=0;i<g_pRes.g_nSendQueThreadNum+1;i++)
	{
		m_pSendThread[i].m_nIndex= i;
		m_pSendThread[i].SetGlobalVar(&g_pRes,&m_pSendData,&g_pAioWork);
	}	
	
	

	return true;
}

bool CGateWay::Start()
{
	//启动IO线程
	if (!g_pAioRun.Start())
	{
		sprintf(m_sErrMsg,"启动IO线程出错[%s]",g_pAioRun.GetErrMsg().c_str());
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	SLEEP_SECONDS(1);
	//启动dreb接收线程
	if (!g_pDrebLink.CreateThread())
	{
		sprintf(m_sErrMsg,"启动DREB接收线程出错");
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	//启动消息处理线程
	if (!m_pMsgThread.CreateThread())
	{
		sprintf(m_sErrMsg,"启动消息处理线程出错");
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	//启动客户端发送处理线程
	for (int i=0;i<g_pRes.g_nSendQueThreadNum+1;i++)
	{

		if (!m_pSendThread[i].CreateThread())
		{
			sprintf(m_sErrMsg,"启动客户端发送处理线程出错[%d]",i);
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
	}

	//启动定时器
	if (g_pRes.g_nUseMonitor == 1)
	{
		if (g_pRes.g_nMonitorHost == 1)
		{
			g_pBpcTime.Init(&g_pRes);
			g_pBpcTime.StartTimer();
		}
	} 

	g_pDrebLink.StartTimer();
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"启动网关成功");
	return true;
}

void CGateWay::Stop()
{
	g_pRes.g_bIsExit = true;
	g_pAioRun.Stop();
	m_pMsgThread.Join();
	g_pDrebLink.StopTimer();
	g_pDrebLink.Join();
	m_pLog->LogMp(LOG_INFO, __FILE__, __LINE__, "停止网关1");
	for (int i=0;i<g_pRes.g_nSendQueThreadNum+1;i++)
	{
		m_pSendThread[i].Join();
	}
	g_pBpcTime.StopTimer();
	m_pLog->LogMp(LOG_INFO, __FILE__, __LINE__, "停止网关2");
	SLEEP_SECONDS(5);

}

std::string CGateWay::GetErrMsg()
{
	return m_sErrMsg;
}

void CGateWay::Monitor()
{
	if (g_pRes.g_pLog.IsStopedThread())
	{
		g_pRes.g_pLog.StartLog();
	}
	if (g_pRes.g_pDrebLog.IsStoped())
	{
		g_pRes.g_pDrebLog.StartLog();
		m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"重启DREB日志线程");
	}
	if (g_pRes.g_pGateLog.IsStoped())
	{
		g_pRes.g_pGateLog.StartLog();
		m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"重启CGATE日志线程");
	}
	if (g_pDrebLink.IsStoped())
	{
		g_pDrebLink.CreateThread();
		m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"重启DREB线程");
	}
	if (m_pMsgThread.IsStoped())
	{
		m_pMsgThread.CreateThread();
		m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"重启消息处理线程");
	}
	for (int i=0;i<g_pRes.g_nSendQueThreadNum+1;i++)
	{
		if (m_pSendThread[i].IsStoped())
		{
			m_pSendThread[i].CreateThread();
			m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"重启发送客户端处理线程 %d",i);
		}
	}
	

}
