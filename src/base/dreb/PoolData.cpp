// PoolData.cpp: implementation of the CPoolData class.
//
//////////////////////////////////////////////////////////////////////

#include "PoolData.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPoolData::CPoolData()
{
	m_pRes = NULL;
}

CPoolData::~CPoolData()
{
	m_pRes = NULL;
}

int CPoolData::PushData(S_DREB_RSMSG *data)
{
	m_pEvent.Lock();
	m_poolDataList.push_back(data);
	m_pEvent.SetEvent();
	m_pEvent.UnLock();
	return 0;
}

S_DREB_RSMSG *CPoolData::GetData()
{
//	CPMUTEX pp(&m_mutex);
	m_pEvent.Lock();
#ifdef _WINDOWS
	m_pEvent.WaitEventTime(1000);
#else
	m_pEvent.WaitEventTime(1000);
#endif
	if (m_pRes->g_bIsExit)
	{
		m_pEvent.UnLock();
		return NULL;
	}
	S_DREB_RSMSG *data = NULL;
	if (m_poolDataList.size()>0)
	{
		data = m_poolDataList.front();
		m_poolDataList.pop_front();
		m_pEvent.UnLock();
		return data;
	}
	m_pEvent.UnLock();
	return NULL;
}

int CPoolData::GetSize()
{
	CBF_PMutex pp(&m_mutex);
	return m_poolDataList.size();
}
