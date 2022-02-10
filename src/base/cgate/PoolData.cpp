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

}

int CPoolData::PushData(S_GATEDREB_RSMSG data)
{
	m_pEvent.Lock();
	m_poolDataList.push_back(data);
	m_pEvent.SetEvent();
	m_pEvent.UnLock();
	return 0;
}

bool CPoolData::GetData(S_GATEDREB_RSMSG &data)
{
	if (m_pRes == NULL)
	{
		return false;
	}
	if (m_pRes->g_bIsExit)
	{
		return false;
	}
	m_pEvent.Lock();
//	CPMUTEX pp(&m_mutex);
	
	if (m_poolDataList.size()>0)
	{
		data = m_poolDataList.front();
		m_poolDataList.pop_front();
		m_pEvent.UnLock();
		return true;
	}
	
	
#ifdef _WINDOWS
	m_pEvent.WaitEventTime(1000);
#else
	m_pEvent.WaitEventTime(1000);
#endif
	if (m_pRes->g_bIsExit)
	{
		m_pEvent.UnLock();
		return false;
	}

	if (m_poolDataList.size()>0)
	{
		data = m_poolDataList.front();
		m_poolDataList.pop_front();
		m_pEvent.UnLock();
		return true;
	}
	
	m_pEvent.UnLock();
	return false;
}

int CPoolData::GetSize()
{
	CBF_PMutex pp(&m_mutex);
	return m_poolDataList.size();
}
