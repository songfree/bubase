// RouteTbl.cpp: implementation of the CRouteTbl class.
//
//////////////////////////////////////////////////////////////////////

#include "RouteTbl.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRouteTbl::CRouteTbl()
{

}

CRouteTbl::~CRouteTbl()
{

}

bool CRouteTbl::Insert(S_DREB_ROUTE rt)
{
	int id;
	//ͨ���������ң�������������
	if (!m_pkey.Find(id,rt.nNodeId,rt.cNodePrivateId,rt.nIndex,rt.nStep))
	{
//		CBF_PMutex pp(&m_mutex);
		id = m_table.Add(rt);//���ӵ���
		m_pkey.Add(id,rt.nNodeId,rt.cNodePrivateId,rt.nIndex,rt.nStep);//��������
		m_index_node.Add(id,rt.nNodeId);//��������
		m_index_index.Add(id,rt.nIndex);//��������
		m_index_private.Add(id,rt.nNodeId,rt.cNodePrivateId);//��������
		return true;
	}
	return false;
}

bool CRouteTbl::Update(S_DREB_ROUTE rt)
{
	CInt iset;
	if (!m_pkey.Select(iset,rt.nNodeId,rt.cNodePrivateId,rt.nIndex,rt.nStep))
	{
		return false;
	}
	int id;
	iset.First(id);
//	CBF_PMutex pp(&m_mutex);
	m_table.m_table[id] = rt;
	return false;
}

bool CRouteTbl::DeleteByPk(S_DREB_ROUTE rt)
{
	CInt iset;
	if (!m_pkey.Select(iset,rt.nNodeId,rt.cNodePrivateId,rt.nIndex,rt.nStep))
	{
		return false;
	}
	int id;
	iset.First(id);
//	CBF_PMutex pp(&m_mutex);
	m_pkey.Delete(iset,rt.nNodeId,rt.cNodePrivateId,rt.nIndex,rt.nStep);
	m_index_node.Delete(iset,rt.nNodeId);
	m_index_index.Delete(iset,rt.nIndex);
	m_index_private.Delete(iset,rt.nNodeId,rt.cNodePrivateId);
	m_table.Delete(id);
	return true;
}

bool CRouteTbl::DeleteByIndex(int index)
{
	CInt iset;
	if (!m_index_index.Select(iset,index))
	{
		return false;
	}
	m_index_index.Delete(iset,index);
	int id;
	bool bret;
	bret = iset.First(id);
//	CBF_PMutex pp(&m_mutex);
	while (bret)
	{
		m_pkey.Delete(iset,m_table.m_table[id].nNodeId,m_table.m_table[id].cNodePrivateId,m_table.m_table[id].nIndex,m_table.m_table[id].nStep);
		m_index_node.Delete(iset,m_table.m_table[id].nNodeId);
		m_index_private.Delete(iset,m_table.m_table[id].nNodeId,m_table.m_table[id].cNodePrivateId);
		m_table.Delete(id);
		bret=iset.Next(id);
	}
	return true;
}

// bool CRouteTbl::SelectByNode(int nodeid,vector<S_DREB_ROUTE> &rt)
// {
// 	CInt iset;
// 	if (!m_index_node.Select(iset,nodeid))
// 	{
// 		return false;
// 	}
// 	int id;
// 	bool bRet;
// 	bRet = iset.First(id);
// 	while (bRet)
// 	{
// 		rt.push_back(m_table.m_table[id]);
// 		bRet = iset.Next(id);
// 	}
// 	return true;
// }
bool CRouteTbl::SelectByNode(int nodeid,vector<S_DREB_ROUTE *> &rt)
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
		index_step_bd.Add(id,m_table.m_table[id].nStep,m_table.m_table[id].nbandwidth);
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
			rt.push_back(&(m_table.m_table[id]));
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
// bool CRouteTbl::SelectByIndex(int index,vector<S_ROUTE_TBL> &rt)
// {
// 	CInt iset;
// 	if (!m_index_index.Select(iset,index))
// 	{
// 		return false;
// 	}
// 	int id;
// 	bool bRet;
// 	bRet = iset.First(id);
// 	while (bRet)
// 	{
// 		rt.push_back(m_table.m_table[id]);
// 		bRet = iset.Next(id);
// 	}
// 	return true;
// }

bool CRouteTbl::SelectByIndex(int index,vector<S_DREB_ROUTE *> &rt)
{
	CInt iset;
	if (!m_index_index.Select(iset,index))
	{
		return false;
	}
	int id;
	bool bRet;
	bRet = iset.First(id);
	while (bRet)
	{
		rt.push_back(&(m_table.m_table[id]));
		bRet = iset.Next(id);
	}
	return true;
}

// 
// bool CRouteTbl::SelectByPrivateNode(int nodeid, int privateid, vector<S_ROUTE_TBL> &rt)
// {
// 	CInt iset;
// 	if (!m_index_private.Select(iset,nodeid,privateid))
// 	{
// 		return false;
// 	}
// 	int id;
// 	bool bRet;
// 	bRet = iset.First(id);
// 	while (bRet)
// 	{
// 		rt.push_back(m_table.m_table[id]);
// 		bRet = iset.Next(id);
// 	}
// 	return true;
// }
bool CRouteTbl::SelectByPrivateNode(int nodeid, int privateid, vector<S_DREB_ROUTE *> &rt)
{
	CInt iset;
	if (!m_index_private.Select(iset,nodeid,privateid))
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
		index_step_bd.Add(id,m_table.m_table[id].nStep,m_table.m_table[id].nbandwidth);
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
			rt.push_back(&(m_table.m_table[id]));
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
bool CRouteTbl::First(S_DREB_ROUTE &dreb)
{
	return m_table.First(dreb);
}

bool CRouteTbl::Next(S_DREB_ROUTE &dreb)
{
	return m_table.Next(dreb);
}

bool CRouteTbl::UpdateRoute(S_DREB_ROUTE rt)
{
	CInt iset;
	if (!m_index_private.Select(iset,rt.nNodeId,rt.cNodePrivateId))
	{
		return Insert(rt);
	}
	int id;
	bool bRet;
	bRet = iset.First(id);
	bool isFind=false;
	while (bRet)
	{
		//�����������ͬ���ٽ��бȽ�
		if (m_table.m_table[id].nIndex == rt.nIndex)
		{
			if (rt.nStep < m_table.m_table[id].nStep) 
			{
				//���²���
				m_table.m_table[id].nStep = rt.nStep;
			}
			return true;
		}
		bRet = iset.Next(id);
	}
	//û���ҵ���ͬ������ŵı���
	return Insert(rt);;
}

// bool CRouteTbl::GetRouteByIndex(int index,vector<S_ROUTE_TBL> &rtlist)
// {
// 	bool bRet;
// 	int id;
// 
// 	bRet = m_index_index.First(id);
// 	while (bRet)
// 	{
// 		if (m_table.m_table[id].nIndex == index)
// 		{
// 			bRet = m_index_index.Next(id);
// 			continue;
// 		}
// 		rtlist.push_back(m_table.m_table[id]);
// 		bRet = m_index_index.Next(id);
// 	}
// 	return true;
// }
bool CRouteTbl::GetRouteByIndex(int index,vector<S_DREB_ROUTE *> &rtlist)
{
	bool bRet;
	int id;
	
	bRet = m_index_index.First(id);
	while (bRet)
	{
		if (m_table.m_table[id].nIndex == index)
		{
			bRet = m_index_index.Next(id);
			continue;
		}
		rtlist.push_back(&(m_table.m_table[id]));
		bRet = m_index_index.Next(id);
	}
	return true;
}
// 
// bool CRouteTbl::NextById(S_ROUTE_TBL &dreb)
// {
// 	int id;
// 	if (!m_index_private.Next(id))
// 	{
// 		return false;
// 	}
// 	m_table.GetById(id,dreb);
// 	return true;
// }
// 
// 
// bool CRouteTbl::FirstById(S_ROUTE_TBL &dreb)
// {
// 	int id;
// 	if (!m_index_private.First(id))
// 	{
// 		return false;
// 	}
// 	m_table.GetById(id,dreb);
// 	return true;
// }

S_DREB_ROUTE * CRouteTbl::FirstById()
{
	int id;
	if (!m_index_private.First(id))
	{
		return NULL;
	}
	return &(m_table.m_table[id]);
}
S_DREB_ROUTE *CRouteTbl::NextById()
{
	int id;
	if (!m_index_private.Next(id))
	{
		return NULL;
	}
	return &(m_table.m_table[id]);
	
}
