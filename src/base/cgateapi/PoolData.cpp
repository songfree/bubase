// PoolData.cpp: implementation of the CPoolData class.
//
//////////////////////////////////////////////////////////////////////

#include "PoolData.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPoolData::CPoolData()
{

}

CPoolData::~CPoolData()
{

}

int CPoolData::PushData(CGATE_COMMSTRU data)
{
	m_pEvent.Lock();
	m_poolDataList.push_back(data);
	m_pEvent.SetEvent();
	m_pEvent.UnLock();
	return 0;
}
void CPoolData::Clear()
{
	m_poolDataList.clear();
}
bool CPoolData::GetData(CGATE_COMMSTRU &data,int timeout)
{
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
	m_pEvent.WaitEventTime(timeout);
#else
	m_pEvent.WaitEventTime(timeout);
#endif

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
