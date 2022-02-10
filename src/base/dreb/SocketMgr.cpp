// SocketMgr.cpp: implementation of the CSocketMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "SocketMgr.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CSocketMgr::CSocketMgr()
{
	memset(m_errMsg,0,sizeof(m_errMsg));
	m_pRes=NULL;
	m_pMemPool=NULL;
}

CSocketMgr::~CSocketMgr()
{
	for (int i=0;i<m_socketList.size();i++)
	{
		delete m_socketList[i];
		m_socketList[i] = NULL;
	}
	m_pRes=NULL;
	m_pMemPool=NULL;
}

bool CSocketMgr::Init(int socketCount)
{
	for (int i=0;i<socketCount;i++)
	{
		CSocketInfo *tmpsocket = new CSocketInfo();
		if (tmpsocket == NULL)
		{
			sprintf(m_errMsg,"ÄÚ´æ²»×ãnew CSocketInfoÊ§°Ü");
			return false;
		}
		tmpsocket->m_index = i;
		tmpsocket->SetGlobalVar(m_pRes,m_pMemPool,300000);
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

bool CSocketMgr::SetGlobalVar(CGlobalVar *pVar, CBF_BufferPool *pPoolbuffer)
{
	m_pRes = pVar;
	m_pMemPool = pPoolbuffer;
	return true;
}
