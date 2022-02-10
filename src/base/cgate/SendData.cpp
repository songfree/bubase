// SendData.cpp: implementation of the CSendData class.
//
//////////////////////////////////////////////////////////////////////

#include "SendData.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSendData::CSendData()
{

}

CSendData::~CSendData()
{

}

int CSendData::PushData(S_CGATE_SMSG data,int prio)
{
	if (prio<0 || prio>2)
	{
		return -1;
	}
	m_pEvent.Lock();
	data.rtime = time(NULL);
	data.prio = prio;
	m_poolDataList[prio].push_back(data);
	m_pEvent.SetEvent();
	m_pEvent.UnLock();
	return 0;
}

bool CSendData::GetData(S_CGATE_SMSG &data)
{
	if (m_pRes == NULL)
	{
		return false;
	}
	if (m_pRes->g_bIsExit)
	{
		return false;
	}
	int i=0;
	m_pEvent.Lock();
//	CPMUTEX pp(&m_mutex);
	for (i=0;i<3;i++)
	{
		if (m_poolDataList[i].size()>0)
		{
			data = m_poolDataList[i].front();
			m_poolDataList[i].pop_front();
			m_pEvent.UnLock();
			return true;
		}
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
	for (i=0;i<3;i++)
	{
		if (m_poolDataList[i].size()>0)
		{
			data = m_poolDataList[i].front();
			m_poolDataList[i].pop_front();
			m_pEvent.UnLock();
			return true;
		}
	}
	m_pEvent.UnLock();
	return false;
}

int CSendData::GetSize()
{
	CBF_PMutex pp(&m_mutex);
	int datanum=0;
	for (int i=0;i<2;i++)
	{
		datanum +=m_poolDataList[i].size();
	}
	return datanum;
}


