// SendQueue.cpp: implementation of the CSendQueue class.
//
//////////////////////////////////////////////////////////////////////

#include "SendQueue.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSendQueue::CSendQueue()
{
}

CSendQueue::~CSendQueue()
{

}

int CSendQueue::PutSendMsg(S_GATEDREB_RSMSG msg)
{
	CBF_PMutex pp(&(m_qSendData.m_mutex));
	msg.rtime = time(NULL);
	msg.offset = 0;
	msg.sendlen = msg.totallen;
//	m_qSendData.m_pEvent.Lock();
	if (m_qSendData.datas.size()<3000) //队列最大数为3000
	{
		m_qSendData.datas.push_back(msg);
		return 0;
	}
//	m_qSendData.m_pEvent.SetEvent();
//	m_qSendData.m_pEvent.UnLock();
	return -1;
}


bool CSendQueue::NeedSend()
{
	if (m_qSendData.datas.size()>0)
	{
		return true;
	}
	else
	{
		return false;
	}
}


void CSendQueue::DeleteSend(RDQUEUE::iterator data)
{
	CBF_PMutex pp(&(m_qSendData.m_mutex));
//	m_qSendData.m_mutex.Lock();
	m_qSendData.datas.erase(data);
//	m_qSendData.m_mutex.UnLock();
}

void CSendQueue::Clear()
{
	m_qSendData.datas.clear();
}