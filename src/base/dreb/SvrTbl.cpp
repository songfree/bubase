// SvrTbl.cpp: implementation of the CSvrTbl class.
//
//////////////////////////////////////////////////////////////////////

#include "SvrTbl.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSvrTbl::CSvrTbl()
{

}

CSvrTbl::~CSvrTbl()
{
	int id;
	bool bRet;
	bRet = m_pkey.First(id);
	while (bRet)
	{
		if (m_table.m_table[id].pSendQueue != NULL)
		{
			delete m_table.m_table[id].pSendQueue;
			m_table.m_table[id].pSendQueue = NULL;
		}
		bRet = m_pkey.Next(id);
	}
}

bool CSvrTbl::Insert(S_SVR_ROUTE svr)
{
	int id;
	//通过主键查找，不存在则增加
	if (!m_pkey.Find(id,svr.nSvrMainId,svr.cSvrPrivateId))
	{
//		CBF_PMutex pp(&m_mutex);
		id = m_table.Add(svr);//增加到表
		m_pkey.Add(id,svr.nSvrMainId,svr.cSvrPrivateId);//增加主键
		m_index_node.Add(id,svr.nSvrMainId);//增加索引
		return true;
	}
	return false;
}

bool CSvrTbl::SelectBySvr(int svrid,vector<S_SVR_ROUTE> &svr)
{
	CInt iset;
	if (!m_index_node.Select(iset,svrid))
	{
		return false;
	}
	int id;
	bool bRet;
	bRet = iset.First(id);
	while (bRet)
	{
		svr.push_back(m_table.m_table[id]);
		bRet = iset.Next(id);
	}
	return true;
}
bool CSvrTbl::SelectBySvr(unsigned int svrid,vector<S_SVR_ROUTE *> &svr)
{
	CInt iset;
	if (!m_index_node.Select(iset,svrid))
	{
		return false;
	}
	int id;
	bool bRet;
	bRet = iset.First(id);
	while (bRet)
	{
		if (m_table.m_table[id].nIndex != 0) //服务未关闭才放入
		{
			svr.push_back(&(m_table.m_table[id]));
		}
		bRet = iset.Next(id);
	}
	return true;
}
bool CSvrTbl::SelectPrivateid(int svrid, int privateid, S_SVR_ROUTE &svr)
{
	CInt iset;
	if (!m_pkey.Select(iset,svrid,privateid))
	{
		return false;
	}
	int id;
	iset.First(id);
	svr = m_table.m_table[id];
	return true;
}
S_SVR_ROUTE * CSvrTbl::SelectPrivateid(unsigned int svrid, int privateid)
{
	CInt iset;
	if (!m_pkey.Select(iset,svrid,privateid))
	{
		return NULL;
	}
	int id;
	iset.First(id);
	if (m_table.m_table[id].nIndex == 0)
	{
		return NULL;
	}
	return &(m_table.m_table[id]);
}
bool CSvrTbl::Delete(int svrid, int privateid)
{
	CInt iset;
	if (!m_pkey.Select(iset,svrid,privateid))
	{
		return false;
	}
	int id;
	iset.First(id);
//	CBF_PMutex pp(&m_mutex);
	m_pkey.Delete(iset,svrid,privateid);
	m_index_node.Delete(iset,svrid);
	m_table.Delete(id);
	return true;
}

bool CSvrTbl::Update(S_SVR_ROUTE svr)
{
	CInt iset;
	if (!m_pkey.Select(iset,svr.nSvrMainId,svr.cSvrPrivateId))
	{
		return false;
	}
	int id=0;
	iset.First(id);
//	CBF_PMutex pp(&m_mutex);

	m_table.m_table[id].nIndex = svr.nIndex;
	m_table.m_table[id].bIsClose = svr.bIsClose;
	m_table.m_table[id].nCloseTime = svr.nCloseTime;
	if (m_table.m_table[id].pSendQueue == NULL)
	{
		m_table.m_table[id].pSendQueue = svr.pSendQueue;
	}
	return true;

}

bool CSvrTbl::OnClose(int svrid, int privateid)
{
	CInt iset;
	if (!m_pkey.Select(iset,svrid,privateid))
	{
		return false;
	}
	int id;
	iset.First(id);
//	CBF_PMutex pp(&m_mutex);
	m_table.m_table[id].bIsClose = true;
	m_table.m_table[id].nIndex = 0;
	m_table.m_table[id].nCloseTime = time(NULL);
	return true;
}

bool CSvrTbl::UnRegister(int svrid, int privateid)
{
	CInt iset;
	if (!m_pkey.Select(iset,svrid,privateid))
	{
		return false;
	}
	int id=0;
	iset.First(id);
//	CBF_PMutex pp(&m_mutex);
	if (m_table.m_table[id].pSendQueue != NULL)
	{
		delete m_table.m_table[id].pSendQueue;
		m_table.m_table[id].pSendQueue = NULL;
	}
	m_pkey.Delete(iset,svrid,privateid);
	m_index_node.Delete(iset,svrid);
	m_table.Delete(id);
	return true;
}
bool CSvrTbl::First(S_SVR_ROUTE &svr)
{
	return m_table.First(svr);
}

bool CSvrTbl::Next(S_SVR_ROUTE &svr)
{
	return m_table.Next(svr);
}