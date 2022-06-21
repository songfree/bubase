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

}

CSocketMgr::~CSocketMgr()
{
	for (int i=0;i<m_socketList.size();i++)
	{
		delete m_socketList[i];
		m_socketList[i] = NULL;
	}
}

bool CSocketMgr::Init(CGResource *res,CBF_BufferPool *pool)
{
	int num;
	//bpu侦听2+BPU连接accept=30，然后每个bpu组为100个连接
	num = 31+res->g_vBpuGroupInfo.size()*100;
	
	for (int i=0;i<num;i++)
	{
		CSocketInfo *tmpsocket = new CSocketInfo();
		if (tmpsocket == NULL)
		{
			sprintf(m_errMsg,"内存不足new CSocketInfo失败");
			return false;
		}
		tmpsocket->m_index = i;
		tmpsocket->Init(res,pool);
		m_socketList.push_back(tmpsocket);
		if (i == 0 || i==1 )
		{
			tmpsocket->m_nType = BPCSOCK_LISTEN;
		}
		else if (i < 31)
		{
			tmpsocket->m_nType = BPCSOCK_CLI;
		}
		else
		{
			tmpsocket->m_nType = BPCSOCK_BPU;
		}
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
