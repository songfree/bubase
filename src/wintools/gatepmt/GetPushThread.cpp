// GetPushThread.cpp: implementation of the CGetPushThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "drebpmt.h"
#include "GetPushThread.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGetPushThread::CGetPushThread()
{
	m_pShareData=NULL; //��������ݲ���
	m_pGateLink=NULL;//����api��
	m_log=NULL;
	m_bisRun = false;
}

CGetPushThread::~CGetPushThread()
{
	m_pShareData=NULL; //��������ݲ���
	m_pGateLink=NULL;//����api��
	m_log=NULL;
}
bool CGetPushThread::Terminate()
{
	return(true);
}

int CGetPushThread::Run()
{
	if (m_pShareData == NULL)
	{
		m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"%d m_pShareDataδ����",m_nID);
		return -1;
	}
	int ret;
	m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"%d �㲥�����߳�����",m_nID);
	CGATE_COMMSTRU data;
	while (!m_pShareData->m_bIsExit && m_bisRun)
	{
		ret = m_pGateLink->GetReqData(&data,m_pShareData->m_pServerPara.timeout/1000);
		if (ret >0 )
		{
			m_log->LogBin(LOG_DEBUG, __FILE__, __LINE__, "�յ����ع�������������", data.buffer,data.head.nLen);
			m_pShareData->RunPush(data.head.nLen+CGATEHEADLEN,1);
		}
		else if (ret <0)
		{
			SLEEP(10);	
		}
	}
	m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"%d �㲥�����߳�ֹͣ",m_nID);
	return 0;
}
