// ArbMain.cpp: implementation of the CArbMain class.
//
//////////////////////////////////////////////////////////////////////

#include "ArbMain.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CArbMain::CArbMain()
{
	m_pLog = NULL;
	m_bIsInit = false;
}

CArbMain::~CArbMain()
{

}

bool CArbMain::Init(const char *confile)
{
	m_bIsInit = false;
	//配置初始
	if (!m_pRes.Init(confile))
	{
		printf("初始化出错  %s\n",m_pRes.m_errMsg.c_str());
		return false;
	}
	//api初始化
	if (!m_pDrebApi.Init(&m_pRes))
	{
		printf("数据总线初始化出错! \n");
		return false;
	}
	m_pLog = m_pDrebApi.GetLogPoint();
	
	//初始线程
	if (!m_pDrebMsgThread.Init(&m_pDrebApi,&m_pRes))
	{
		m_pDrebApi.Stop();
		printf("消息处理线程初始化出错! \n");
		return false;
	}
	m_bIsInit = true;
	return true;
}

bool CArbMain::Start()
{
	if (!m_bIsInit)
	{
		printf("未初始化 \n");
		return false;
	}
	//启动api线程
	m_pDrebApi.Start();

	//启动消息处理线程
	m_pDrebMsgThread.Start();
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"启动仲裁机完成!");
	return true;
}

void CArbMain::Stop()
{
	m_pDrebApi.Stop();
	m_pDrebMsgThread.Stop();
}

void CArbMain::Monitor()
{
	m_pDrebApi.MonitorThread();
	if (m_pDrebMsgThread.IsStoped())
	{
		m_pDrebMsgThread.Start();
	}
}
