// BF_DrebSocketMgr.cpp: implementation of the CBF_DrebSocketMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "BF_DrebSocketMgr.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBF_DrebSocketMgr::CBF_DrebSocketMgr()
{
	memset(m_errMsg,0,sizeof(m_errMsg));
}

CBF_DrebSocketMgr::~CBF_DrebSocketMgr()
{
	for (int i=0;i<m_socketList.size();i++)
	{
		if (m_socketList[i] != NULL)
		{
			delete m_socketList[i];
			m_socketList[i] = NULL;
		}
	}
}

bool CBF_DrebSocketMgr::Init(CBF_DrebResource *res,CBF_BufferPool *pool,CIErrlog *perrlog,CBF_CommDataLog *pdreb2bpclog,CBF_CommDataLog *pbpc2dreblog)
{
	int num = res->g_vDrebLinkInfo.size();
	//连接总线的连接
	for (int i=0;i<num;i++)
	{
		CBF_DrebSocketInfo *tmpsocket = new CBF_DrebSocketInfo();
		if (tmpsocket == NULL)
		{
			sprintf(m_errMsg,"内存不足new CBF_DrebSocketInfo失败");
			return false;
		}
		tmpsocket->m_index = i;
		tmpsocket->Init(res,pool,perrlog,pdreb2bpclog,pbpc2dreblog);
		tmpsocket->m_nType = BPCSOCK_DREB;
		tmpsocket->m_bNeedConnect = true;
		tmpsocket->m_bNeedread = false;
		tmpsocket->m_sDrebIp = res->g_vDrebLinkInfo[i].sIp;
		tmpsocket->m_nDrebPort = res->g_vDrebLinkInfo[i].nPort;
		m_socketList.push_back(tmpsocket);
	}
	return true;
}

CBF_DrebSocketInfo * CBF_DrebSocketMgr::at(int index)
{
	if (index<0 || index > m_socketList.size()-1 )
	{
		return NULL;
	}
	return m_socketList[index];
}

bool CBF_DrebSocketMgr::AffirmIndex(int index)
{
	if (index <0 || index > m_socketList.size()-1)
	{
		return false;
	}
	return true;
}
