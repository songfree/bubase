// GwRun.cpp: implementation of the CGwRun class.
//
//////////////////////////////////////////////////////////////////////

#include "GwRun.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGwRun::CGwRun()
{
	bzero(m_sErrMsg,sizeof(m_sErrMsg));
}

CGwRun::~CGwRun()
{

}

bool CGwRun::Init(const char *confile)
{
	if (!g_pRes.Init(confile))
	{
		sprintf(m_sErrMsg,"初始化配置出错");
		return false;
	}
	m_pWorkThread.SetGlobalVar(&g_pRes,&m_pSockMgr);

	//初始化连接池 dreb+侦听+1000连接
	if (!m_pSockMgr.Init(&g_pRes))
	{
		sprintf(m_sErrMsg,"%s",m_pSockMgr.m_errMsg);
		g_pRes.g_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	//初始化侦听
	printf("开始端口侦听:");
	for (int i=g_pRes.g_vDrebLinkInfo.size(); i<g_pRes.g_vDrebLinkInfo.size()+g_pRes.m_gwList.size() ;i++)
	{
		if (!m_pSockMgr.at(i)->Listen(false,30))
		{
			sprintf(m_sErrMsg,"初始化失败:建立%d sokcet侦听失败",m_pSockMgr.at(i)->m_nPort);
			printf("%s \n",m_sErrMsg);
			g_pRes.g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
		printf("%d ",m_pSockMgr.at(i)->m_nPort);
	}
	printf("\n");
	

	return true;
}

void CGwRun::Monitor()
{
	if (m_pWorkThread.IsStoped())
	{
		g_pRes.g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"工作线程已停止,重启");
		m_pWorkThread.CreateThread();
	}
}

bool CGwRun::Start()
{
	//启动线程
	g_pRes.g_bIsExit = false;
	m_pWorkThread.CreateThread();
	m_pWorkThread.StartTimer();

	return true;
}

std::string CGwRun::GetErrMsg()
{
	return m_sErrMsg;
}

void CGwRun::Stop()
{
	g_pRes.g_bIsExit = true;
	m_pWorkThread.StopTimer();
	m_pWorkThread.Join();
	
	
}
