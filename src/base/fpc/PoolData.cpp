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

int CPoolData::PushData(S_BF_RSMSG_FPC data)
{
//	CPMUTEX pp(&m_mutex);
	m_pEvent.Lock();
	m_poolDataList.push_back(data);
	m_pEvent.SetEvent();
	m_pEvent.UnLock();
	return 0;
}
int CPoolData::GetSize()
{
	m_pEvent.Lock();
	int ret = m_poolDataList.size();
	m_pEvent.UnLock();
	return ret;
}
bool CPoolData::GetData(S_BF_RSMSG_FPC &data)
{
	if (m_pRes->g_bToExit)
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
	if (m_pRes->g_bToExit)
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
