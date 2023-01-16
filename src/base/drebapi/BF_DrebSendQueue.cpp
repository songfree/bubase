// BF_DrebSendQueue.cpp: implementation of the CBF_DrebSendQueue class.
//
//////////////////////////////////////////////////////////////////////

#include "BF_DrebSendQueue.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBF_DrebSendQueue::CBF_DrebSendQueue()
{
	m_bIsNeedSend = false;
	m_nNeedSendNum = 0;
	m_sendNum = 10;
	m_nQueueSize =1000;
}

CBF_DrebSendQueue::~CBF_DrebSendQueue()
{

}

int CBF_DrebSendQueue::PutSendMsg(S_BPC_RSMSG msg)
{
	msg.sMsgBuf->sBpcHead.nRtime = time(NULL);
	msg.nRtime = time(NULL);
	m_qSendData.m_pEvent.Lock();
	if (m_qSendData.datas.size()<= m_nQueueSize) //队列最大数
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


bool CBF_DrebSendQueue::NeedSend()
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


void CBF_DrebSendQueue::DeleteSend(RDQUEUE::iterator data)
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


void CBF_DrebSendQueue::Clear()
{
	m_qSendData.datas.clear();
}
