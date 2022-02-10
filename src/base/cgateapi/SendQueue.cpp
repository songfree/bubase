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

int CSendQueue::PutSendMsg(S_CGATEAPI_SMSG msg)
{
	msg.sendnum = 1;
	msg.rtime = time(NULL);
	CBF_PMutex pp(&(m_qSendData.m_mutex));
	m_qSendData.datas.push_back(msg);
	m_nNeedSendNum++;
	m_bIsNeedSend = true;
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


void CSendQueue::DeleteSend(CGATE_SQUEUE::iterator data)
{
	CBF_PMutex pp(&(m_qSendData.m_mutex));
	m_qSendData.datas.erase(data);
	m_nNeedSendNum--;
	if (m_nNeedSendNum<1)
	{
		m_bIsNeedSend = false;
	}
	else
	{
		m_bIsNeedSend = true;
	}
}
