// GateWay.cpp: implementation of the CGateWay class.
//
//////////////////////////////////////////////////////////////////////

#include "GateWay.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CGateWay::CGateWay()
{

}

CGateWay::~CGateWay()
{

}

bool CGateWay::Init(const char *confile)
{
	printf("%s\n",confile);
	//ͳһ��drebapi����־�༰�ڴ滺����
	m_pLog = m_pDrebapi.GetLogPoint();
	m_pMemPool = m_pDrebapi.GetBufferPool();
	if (!g_pRes.Init(confile,m_pLog))
	{
		sprintf(m_sErrMsg,"��ʼ�������ļ�[%s]����",confile);
		return false;
	}
    //��ʼ�� dreb���ӻ����
    m_pPoolData.m_pRes = &g_pRes;
    m_pSendData.m_pRes = &g_pRes;
	//drebapi��ʼ��
	m_pDrebSpi.SetGlobalVar(&m_pPoolData, &m_pSendData, &g_pAioWork);//���ò���
	m_pDrebSpi.Init(&m_pDrebapi, &g_pRes.g_pDrebRes);  //�ص��Ĳ���
	

	m_pDrebSpi.m_lBcRegister = &(g_pRes.g_lBCFuncList);//Ҫ���ĵĹ㲥��Ϣ
	m_pDrebapi.Init(&g_pRes.g_pDrebRes,&m_pDrebSpi);   //����api��ʼ������ص���

	//����IO����
	g_pAioRun.SetParameter(&g_pAioWork,&g_pRes);
	
	//������Ϣ�̲߳���
	m_pMsgThread.SetGlobalVar(&g_pRes,&m_pPoolData,m_pMemPool,&m_pDrebapi);
	g_pAioWork.SetGlobalVar(&g_pRes,&m_pPoolData,m_pMemPool,&m_pSendData);
	for (int i=0;i<g_pRes.g_nSendQueThreadNum+1;i++)
	{
		m_pSendThread[i].m_nIndex= i;
		m_pSendThread[i].SetGlobalVar(&g_pRes,&m_pSendData,&g_pAioWork);
	}	
	return true;
}

bool CGateWay::Start()
{
	
	//����IO�߳�
	if (!g_pAioRun.Start())
	{
		sprintf(m_sErrMsg,"����IO�̳߳���[%s]",g_pAioRun.GetErrMsg().c_str());
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	SLEEP_SECONDS(1);
	//����dreb�����߳�
	if (!m_pDrebapi.Start())
	{
		sprintf(m_sErrMsg,"����DREB api�̳߳���");
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	//������Ϣ�����߳�
	if (!m_pMsgThread.CreateThread())
	{
		sprintf(m_sErrMsg,"������Ϣ�����̳߳���");
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	//�����ͻ��˷��ʹ����߳�
	for (int i=0;i<g_pRes.g_nSendQueThreadNum+1;i++)
	{

		if (!m_pSendThread[i].CreateThread())
		{
			sprintf(m_sErrMsg,"�����ͻ��˷��ʹ����̳߳���[%d]",i);
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
	}
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"�������سɹ�");
	return true;
}

void CGateWay::Stop()
{
	g_pRes.g_bIsExit = true;
	g_pAioRun.Stop();
	m_pMsgThread.Join();
	m_pDrebapi.Stop();
	m_pLog->LogMp(LOG_INFO, __FILE__, __LINE__, "ֹͣ����1");
	for (int i=0;i<g_pRes.g_nSendQueThreadNum+1;i++)
	{
		m_pSendThread[i].Join();
	}
	m_pLog->LogMp(LOG_INFO, __FILE__, __LINE__, "ֹͣ����2");
	SLEEP_SECONDS(5);

}

std::string CGateWay::GetErrMsg()
{
	return m_sErrMsg;
}

void CGateWay::Monitor()
{
	if (g_pRes.g_pGateLog.IsStoped())
	{
		g_pRes.g_pGateLog.StartLog();
		m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"����CGATE��־�߳�");
	}
	if (m_pMsgThread.IsStoped())
	{
		m_pMsgThread.CreateThread();
		m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"������Ϣ�����߳�");
	}
	for (int i=0;i<g_pRes.g_nSendQueThreadNum+1;i++)
	{
		if (m_pSendThread[i].IsStoped())
		{
			m_pSendThread[i].CreateThread();
			m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"�������Ϳͻ��˴����߳� %d",i);
		}
	}

}
