// DrebTbl.cpp: implementation of the CDrebTbl class.
//
//////////////////////////////////////////////////////////////////////

#include "DrebTbl.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDrebTbl::CDrebTbl()
{

}

CDrebTbl::~CDrebTbl()
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
bool CDrebTbl::Insert(S_DREB_ROUTE dreb)
{
	int id;
	CBF_PMutex pp(&m_mutex);
	//通过主键查找，不存在则增加
	if (!m_pkey.Find(id,dreb.nNodeId,dreb.cNodePrivateId))
	{
		id = m_table.Add(dreb);//增加到表
		m_pkey.Add(id,dreb.nNodeId,dreb.cNodePrivateId);//增加主键
		m_index_node.Add(id,dreb.nNodeId);//增加索引
		m_index_index.Add(id,dreb.nIndex);
		return true;
	}
	return false;
}

bool CDrebTbl::SelectByNode(int nodeid, std::vector<S_DREB_ROUTE> &dreb)
{
	CInt iset;
	CBF_PMutex pp(&m_mutex);
	if (!m_index_node.Select(iset,nodeid))
	{
		return false;
	}
	int id;
	bool bRet;
	bRet = iset.First(id);
	while (bRet)
	{
		dreb.push_back(m_table.m_table[id]);
		bRet = iset.Next(id);
	}
	return true;
}

bool CDrebTbl::SelectByNode(int nodeid, std::vector<S_DREB_ROUTE *> &dreb)
{
	CInt iset;
	CBF_PMutex pp(&m_mutex);
	if (!m_index_node.Select(iset,nodeid))
	{
		return false;
	}
	int id;
	bool bRet;
	CIndexInt<2> index_step_bd;//步进及带宽的索引
	bRet = iset.First(id);
	while (bRet)
	{
		//将数据加入到索引
		if (!m_table.m_table[id].bIsClose) //若连接关闭则不用加入了
		{
			index_step_bd.Add(id,m_table.m_table[id].nStep,m_table.m_table[id].nbandwidth);
		}
		bRet = iset.Next(id);
	}
	
	unsigned int curstep=10000;
	unsigned int stepnum=0; //不同的步进计数
	bRet = index_step_bd.First(id);
	while (bRet)
	{
		//将当前的步进赋值
		if (curstep != m_table.m_table[id].nStep)
		{
			curstep = m_table.m_table[id].nStep;
			stepnum++;
		}
		if (stepnum == 1)
		{
			//最小步进的值放入
			dreb.push_back(&(m_table.m_table[id]));
		}
		else if (stepnum>1)
		{
			//只取最小步进的值，直接返回了
			return true;
		}
		bRet = index_step_bd.Next(id);
	}
	
	return true;
}
bool CDrebTbl::SelectPrivateid(int nodeid, int privateid, S_DREB_ROUTE &dreb)
{
	CInt iset;
	CBF_PMutex pp(&m_mutex);
	if (!m_pkey.Select(iset,nodeid,privateid))
	{
		return false;
	}
	int id;
	iset.First(id);
	dreb = m_table.m_table[id];
	return true;
}
S_DREB_ROUTE * CDrebTbl::SelectPrivateid(int nodeid, int privateid)
{
	CInt iset;
	CBF_PMutex pp(&m_mutex);
	if (!m_pkey.Select(iset,nodeid,privateid))
	{
		return NULL;
	}
	int id;
	iset.First(id);
	return  &(m_table.m_table[id]);
}
bool CDrebTbl::Delete(int nodeid, int privateid)
{
	CInt iset;
	CBF_PMutex pp(&m_mutex);
	if (!m_pkey.Select(iset,nodeid,privateid))
	{
		return false;
	}
	int id;
	iset.First(id);
	m_pkey.Delete(iset,nodeid,privateid);
	m_index_node.Delete(iset,nodeid);
	m_index_index.Delete(iset,m_table.m_table[id].nIndex);
	m_table.Delete(id);
	return true;
}

bool CDrebTbl::Update(S_DREB_ROUTE dreb)
{
	CInt iset;
	CBF_PMutex pp(&m_mutex);
	if (!m_pkey.Select(iset,dreb.nNodeId,dreb.cNodePrivateId))
	{
		return false;
	}
	int id;
	iset.First(id);
	if (m_table.m_table[id].nStep >= dreb.nStep) //
	{
		m_table.m_table[id].nStep = dreb.nStep;
		m_table.m_table[id].nIndex = dreb.nIndex;
		m_table.m_table[id].bIsClose = dreb.bIsClose;
		m_table.m_table[id].nbandwidth = dreb.nbandwidth;
		if (m_table.m_table[id].pSendQueue == NULL)
		{
			m_table.m_table[id].pSendQueue = dreb.pSendQueue;
		}
		return true;
	}
//	m_table.m_table[id] = dreb;
	return false;
}

bool CDrebTbl::OnClose(int nodeid, int privateid)
{
	CInt iset;
	CBF_PMutex pp(&m_mutex);
	if (!m_pkey.Select(iset,nodeid,privateid))
	{
		return false;
	}
	int id;
	iset.First(id);
	m_table.m_table[id].bIsClose = true;
	m_table.m_table[id].nStep = 100;
	m_table.m_table[id].nIndex = 0;
	m_table.m_table[id].nCloseTime = time(NULL);
	return true;
}

bool CDrebTbl::UnRegister(int nodeid, int privateid)
{

	return Delete(nodeid,privateid);
}

S_DREB_ROUTE * CDrebTbl::SelectByIndex(int index)
{
	CInt iset;
	CBF_PMutex pp(&m_mutex);
	if (!m_index_index.Select(iset,index))
	{
		return NULL;
	}
	int id;
	bool bRet;
	bRet = iset.First(id);
	//只会有一个
	// 	while (bRet)
	// 	{
	// 		dreb.push_back(m_table.m_table[id]);
	// 		bRet = iset.Next(id);
	// 	}
	return &(m_table.m_table[id]);
}
bool CDrebTbl::Select(std::vector<S_DREB_ROUTE*>& dreb)
{
	CBF_PMutex pp(&m_mutex);
	int rid;
	bool bret = m_pkey.First(rid);
	if (!bret)
	{
		return false;
	}
	while (bret)
	{
		dreb.push_back(&(m_table.m_table[rid]));
		bret = m_pkey.Next(rid);
	}
	return true;
}
bool CDrebTbl::SelectRouteByIndex(int index,std::vector<S_DREB_ROUTE*>& dreb)
{
    CBF_PMutex pp(&m_mutex);
	int id;
    bool bret = m_index_index.First(id);
	if (!bret)
	{
		return false;
	}
    while (bret)
    {
        //index不相同且连接正常的
        if (m_table.m_table[id].nIndex != index && !m_table.m_table[id].bIsClose)
        {
			dreb.push_back(&(m_table.m_table[id]));
        }
        bret = m_index_index.Next(id);
    }
    return true;
}