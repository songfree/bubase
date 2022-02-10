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

int CSendQueue::PutSendMsg(S_BPC_RSMSG msg)
{
	msg.sMsgBuf->sBpcHead.nRtime = time(NULL);
	m_qSendData.m_pEvent.Lock();
	if (m_qSendData.datas.size()<1000) //队列最大数为1000
	{
		m_qSendData.datas.push_back(msg);
		m_qSendData.m_pEvent.SetEvent();
		m_nNeedSendNum++;
		m_bIsNeedSend = true;
		m_qSendData.m_pEvent.UnLock();
		return 0;
	}
	m_qSendData.m_pEvent.UnLock();
	return -1;
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


void CSendQueue::Clear()
{
	m_qSendData.datas.clear();
}

