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

bool CSocketMgr::Init(CGateResource *res,CBF_BufferPool *pool)
{
	int num;
	num = res->g_vDrebLinkInfo.size();
	for (int i=0;i<num;i++)
	{
		CSocketInfo *tmpsocket = new CSocketInfo();
		if (tmpsocket == NULL)
		{
			sprintf(m_errMsg,"ÄÚ´æ²»×ãnew CSocketInfoÊ§°Ü");
			return false;
		}
		tmpsocket->m_index = i;
		tmpsocket->Init(res,pool);
		tmpsocket->m_nType = SOCK_DREB;
		tmpsocket->m_bNeedConnect = true;
		tmpsocket->m_bNeedread = false;
		tmpsocket->m_sDrebIp = res->g_vDrebLinkInfo[i].sIp;
		tmpsocket->m_nDrebPort = res->g_vDrebLinkInfo[i].nPort;
		tmpsocket->m_bChecked = false;
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
