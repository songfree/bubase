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
	//���ó�ʼ
	if (!m_pRes.Init(confile))
	{
		printf("��ʼ������  %s\n",m_pRes.m_errMsg.c_str());
		return false;
	}
	//api��ʼ��
	if (!m_pDrebApi.Init(&m_pRes))
	{
		printf("�������߳�ʼ������! \n");
		return false;
	}
	m_pLog = m_pDrebApi.GetLogPoint();
	
	//��ʼ�߳�
	if (!m_pDrebMsgThread.Init(&m_pDrebApi,&m_pRes))
	{
		m_pDrebApi.Stop();
		printf("��Ϣ�����̳߳�ʼ������! \n");
		return false;
	}
	m_bIsInit = true;
	return true;
}

bool CArbMain::Start()
{
	if (!m_bIsInit)
	{
		printf("δ��ʼ�� \n");
		return false;
	}
	//����api�߳�
	m_pDrebApi.Start();

	//������Ϣ�����߳�
	m_pDrebMsgThread.Start();
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"�����ٲû����!");
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
