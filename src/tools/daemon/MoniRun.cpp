// MoniRun.cpp: implementation of the CMoniRun class.
//
//////////////////////////////////////////////////////////////////////

#include "MoniRun.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMoniRun::CMoniRun()
{
	bzero(m_sErrMsg,sizeof(m_sErrMsg));
}

CMoniRun::~CMoniRun()
{

}

bool CMoniRun::Init(const char *confile)
{
	if (!g_pRes.Init(confile))
	{
		sprintf(m_sErrMsg,"��ʼ�����ó���");
		return false;
	}
	m_pWorkThread.SetGlobalVar(&g_pRes,&m_pSockMgr);

	//��ʼ�����ӳ� dreb+����+200����
	if (!m_pSockMgr.Init(&g_pRes))
	{
		sprintf(m_sErrMsg,"%s",m_pSockMgr.m_errMsg);
		g_pRes.g_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	//��ʼ������
	printf("��ʼ�˿�����:");
	for (int i=g_pRes.g_vDrebLinkInfo.size(); i<g_pRes.g_vDrebLinkInfo.size()+1;i++)
	{
		if (!m_pSockMgr.at(i)->Listen(false,30))
		{
			sprintf(m_sErrMsg,"��ʼ��ʧ��:����%d sokcet����ʧ��",m_pSockMgr.at(i)->m_nPort);
			printf("%s \n",m_sErrMsg);
			g_pRes.g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
		printf("%d ",m_pSockMgr.at(i)->m_nPort);
	}
	printf("\n");
	

	return true;
}

void CMoniRun::Monitor()
{
	if (m_pWorkThread.IsStoped())
	{
		g_pRes.g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"�����߳���ֹͣ,����");
		m_pWorkThread.CreateThread();
	}

}

bool CMoniRun::Start()
{
	//�����߳�
	g_pRes.g_bIsExit = false;
	m_pWorkThread.CreateThread();
	m_pWorkThread.StartTimer();

//	SLEEP(500);
	m_pWorkThread.WriteFile();
	return true;
}

std::string CMoniRun::GetErrMsg()
{
	return m_sErrMsg;
}

void CMoniRun::Stop()
{
	g_pRes.g_bIsExit = true;
	m_pWorkThread.StopTimer();
	m_pWorkThread.Join();
	m_pWorkThread.WriteFile();
	
}
