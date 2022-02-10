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
	//ͨ���������ң�������������
	if (!m_pkey.Find(id,dreb.nNodeId,dreb.cNodePrivateId))
	{
//		CBF_PMutex pp(&m_mutex);
		id = m_table.Add(dreb);//���ӵ���
		m_pkey.Add(id,dreb.nNodeId,dreb.cNodePrivateId);//��������
		m_index_node.Add(id,dreb.nNodeId);//��������
		m_index_index.Add(id,dreb.nIndex);
		return true;
	}
	return false;
}

bool CDrebTbl::SelectByNode(int nodeid, vector<S_DREB_ROUTE> &dreb)
{
	CInt iset;
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

bool CDrebTbl::SelectByNode(int nodeid, vector<S_DREB_ROUTE *> &dreb)
{
	CInt iset;
	if (!m_index_node.Select(iset,nodeid))
	{
		return false;
	}
	int id;
	bool bRet;
	CIndexInt<2> index_step_bd;//���������������
	bRet = iset.First(id);
	while (bRet)
	{
		//�����ݼ��뵽����
		if (!m_table.m_table[id].bIsClose) //�����ӹر����ü�����
		{
			index_step_bd.Add(id,m_table.m_table[id].nStep,m_table.m_table[id].nbandwidth);
		}
		bRet = iset.Next(id);
	}
	
	unsigned int curstep=10000;
	unsigned int stepnum=0; //��ͬ�Ĳ�������
	bRet = index_step_bd.First(id);
	while (bRet)
	{
		//����ǰ�Ĳ�����ֵ
		if (curstep != m_table.m_table[id].nStep)
		{
			curstep = m_table.m_table[id].nStep;
			stepnum++;
		}
		if (stepnum == 1)
		{
			//��С������ֵ����
			dreb.push_back(&(m_table.m_table[id]));
		}
		else if (stepnum>1)
		{
			//ֻȡ��С������ֵ��ֱ�ӷ�����
			return true;
		}
		bRet = index_step_bd.Next(id);
	}
	
	return true;
}
bool CDrebTbl::SelectPrivateid(int nodeid, int privateid, S_DREB_ROUTE &dreb)
{
	CInt iset;
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
	if (!m_pkey.Select(iset,nodeid,privateid))
	{
		return false;
	}
	int id;
	iset.First(id);
//	CBF_PMutex pp(&m_mutex);
	m_pkey.Delete(iset,nodeid,privateid);
	m_index_node.Delete(iset,nodeid);
	m_index_index.Delete(iset,m_table.m_table[id].nIndex);
	m_table.Delete(id);
	return true;
}

bool CDrebTbl::Update(S_DREB_ROUTE dreb)
{
	CInt iset;
	if (!m_pkey.Select(iset,dreb.nNodeId,dreb.cNodePrivateId))
	{
		return false;
	}
	int id;
	iset.First(id);
//	CBF_PMutex pp(&m_mutex);
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
	if (!m_pkey.Select(iset,nodeid,privateid))
	{
		return false;
	}
	int id;
	iset.First(id);
//	CBF_PMutex pp(&m_mutex);
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

bool CDrebTbl::SelectByIndex(int index, S_DREB_ROUTE &dreb)
{
	CInt iset;
	if (!m_index_index.Select(iset,index))
	{
		return false;
	}
	int id;
 	bool bRet;
 	bRet = iset.First(id);
	//ֻ����һ��
// 	while (bRet)
// 	{
// 		dreb.push_back(m_table.m_table[id]);
// 		bRet = iset.Next(id);
// 	}
	dreb = m_table.m_table[id];
	return true;
}
S_DREB_ROUTE * CDrebTbl::SelectByIndex(int index)
{
	CInt iset;
	if (!m_index_index.Select(iset,index))
	{
		return NULL;
	}
	int id;
	bool bRet;
	bRet = iset.First(id);
	//ֻ����һ��
	// 	while (bRet)
	// 	{
	// 		dreb.push_back(m_table.m_table[id]);
	// 		bRet = iset.Next(id);
	// 	}
	return &(m_table.m_table[id]);
}
S_DREB_ROUTE * CDrebTbl::First()
{
	int id;
	bool bRet = m_pkey.First(id);
	if (!bRet)
	{
		return NULL;
	}
	
	return &(m_table.m_table[id]);
}

S_DREB_ROUTE * CDrebTbl::Next()
{
	int id;
	bool bRet = m_pkey.Next(id);
	if (!bRet)
	{
		return NULL;
	}
	return &(m_table.m_table[id]);
}
