// SendQueue.cpp: implementation of the CSendQueue class.
//
//////////////////////////////////////////////////////////////////////

#include "SendQueue.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSendQueue::CSendQueue()
{
	m_pMemPool = NULL;
}

CSendQueue::~CSendQueue()
{
	RDQUEUE::iterator prq,prn;

	for (prn=m_qSendData.datas.begin();prn!=m_qSendData.datas.end();)
	{
		prq = prn;
		prn++;
		m_pMemPool->PoolFree((*prq));
		DeleteSend(prq);	
	}
	m_pMemPool = NULL;
}

int CSendQueue::PutSendMsg(S_DREB_RSMSG *msg)
{
	msg->msghead.sendnum =0;
	msg->msghead.rtime = time(NULL);
//	CBF_PMutex pp(&m_qSendData.m_mutex);
	if (m_qSendData.datas.size()<3000) //队列最大数为3000
	{
		m_qSendData.datas.push_back(msg);
		return 0;
	}
	
	return -1;
}


bool CSendQueue::NeedSend()
{
	if (m_qSendData.datas.size() >0 )
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
//	CBF_PMutex pp(&m_qSendData.m_mutex);
	m_qSendData.datas.erase(data);
}
