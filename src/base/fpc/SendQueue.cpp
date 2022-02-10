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

int CSendQueue::PutSendMsg(S_BF_RSMSG_FPC msg)
{
	msg.msgbuf.head.rtime = time(NULL);
	m_qSendData.m_pEvent.Lock();
	m_qSendData.datas.push_back(msg);
	m_qSendData.m_pEvent.SetEvent();
	m_nNeedSendNum++;
	m_bIsNeedSend = true;
	m_qSendData.m_pEvent.UnLock();
	return 0;
}


bool CSendQueue::NeedSend()
{
	if (m_nNeedSendNum>0)
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
	m_qSendData.m_mutex.Lock();
	m_qSendData.datas.erase(data);
	m_nNeedSendNum--;
	m_qSendData.m_mutex.UnLock();
	if (m_nNeedSendNum<1)
	{
		m_bIsNeedSend = false;
	}
	else
	{
		m_bIsNeedSend = true;
	}
}

