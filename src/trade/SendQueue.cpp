// SendQueue.cpp: implementation of the CSendQueue class.
//
//////////////////////////////////////////////////////////////////////

#include "SendQueue.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSendQueue::CSendQueue()
{
	m_bIsNeedSend = false;
	m_nNeedSendNum = 0;
	m_sendNum = 10;
}

CSendQueue::~CSendQueue()
{

}

int CSendQueue::PutSendMsg(S_TRADE_DATA msg,int prio)
{
	if (prio <0 || prio >2)
	{
		return -1;
	}
	m_qSendData[prio].m_pEvent.Lock();
	m_qSendData[prio].datas.push_back(msg);
	m_qSendData[prio].m_pEvent.SetEvent();
	m_qSendData[prio].m_pEvent.UnLock();
	return 0;
}


bool CSendQueue::NeedSend()
{
	for (int i=0; i< PRIORITIES ; i++)
	{
		m_qSendData[i].m_pEvent.Lock();
		if (m_qSendData[i].datas.size()>0)
		{
			m_qSendData[i].m_pEvent.UnLock();
			return true;
		}
		m_qSendData[i].m_pEvent.UnLock();
	}
	return false;

}


void CSendQueue::DeleteSend(TRADE_RDQUEUE::iterator data,int prio)
{
	m_qSendData[prio].m_mutex.Lock();
	m_qSendData[prio].datas.erase(data);
	m_qSendData[prio].m_mutex.UnLock();
	
}


int CSendQueue::GetSize(int prio)
{
	if (prio <0 || prio >2)
	{
		return -1;
	}
	m_qSendData[prio].m_pEvent.Lock();
	int ret = m_qSendData[prio].datas.size();
	m_qSendData[prio].m_pEvent.UnLock();
	return ret;
	
}

bool CSendQueue::QueueData()
{
	for (int i=0; i< PRIORITIES ; i++)
	{
		m_qSendData[i].m_pEvent.Lock();
		if (m_qSendData[i].datas.size()>0)
		{
			m_qSendData[i].m_pEvent.UnLock();
			return true;
		}
		m_qSendData[i].m_pEvent.UnLock();
	}
	return false;
}
