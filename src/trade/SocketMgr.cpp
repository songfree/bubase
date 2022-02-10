// SocketMgr.cpp: implementation of the CSocketMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "SocketMgr.h"
#include "BF_BufferPool.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CSocketMgr::CSocketMgr()
{
	memset(m_errMsg,0,sizeof(m_errMsg));
}

CSocketMgr::~CSocketMgr()
{
	for (int i=0;i<m_socketList.size();i++)
	{
		delete m_socketList[i];
		m_socketList[i] = NULL;
	}
}

bool CSocketMgr::Init(CTradeResource *res,CBF_BufferPool *pool)
{
	for (int i=0;i<res->g_nMaxConnectNum;i++)
	{
		CSocketInfo *tmpsocket = new CSocketInfo();
		if (tmpsocket == NULL)
		{
			sprintf(m_errMsg,"new CSocketInfo failed");
			return false;
		}
		tmpsocket->m_index = i;
		tmpsocket->Init(res,pool);
		m_socketList.push_back(tmpsocket);
	}
	return true;
}

CSocketInfo * CSocketMgr::at(int index)
{
	if (index<0 || index > m_socketList.size()-1 )
	{
		return NULL;
	}
	return m_socketList[index];
}

bool CSocketMgr::AffirmIndex(int index)
{
	if (index <0 || index > m_socketList.size()-1)
	{
		return false;
	}
	return true;
}
