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
	CBF_PMutex pp(&m_mutex);
	if (!m_pkey.Find(rt.nNodeId,rt.cNodePrivateId,rt.nIndex,rt.nStep))
	{
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
	int id;
	CBF_PMutex pp(&m_mutex);
	if (!m_pkey.Select(id,rt.nNodeId,rt.cNodePrivateId,rt.nIndex,rt.nStep))
	{
		return false;
	}
	m_table.m_table[id] = rt;
	return true;
}

bool CRouteTbl::DeleteByPk(S_DREB_ROUTE rt)
{
	int id;
	
	CBF_PMutex pp(&m_mutex);
	if (!m_pkey.Select(id,rt.nNodeId,rt.cNodePrivateId,rt.nIndex,rt.nStep))
	{
		return false;
	}
	CInt iset;
	iset.Add(id);
	m_pkey.Delete(rt.nNodeId,rt.cNodePrivateId,rt.nIndex,rt.nStep);
	m_index_node.Delete(iset,rt.nNodeId);
	m_index_index.Delete(iset,rt.nIndex);
	m_index_private.Delete(iset,rt.nNodeId,rt.cNodePrivateId);
	m_table.Delete(id);
	return true;
}

bool CRouteTbl::DeleteByIndex(int index)
{
	CInt iset;
	CBF_PMutex pp(&m_mutex);
	if (!m_index_index.Select(iset,index))
	{
		return false;
	}
	m_index_index.Delete(iset,index);
	int id;
	bool bret;
	bret = iset.First(id);
	while (bret)
	{
		m_pkey.Delete(m_table.m_table[id].nNodeId,m_table.m_table[id].cNodePrivateId,m_table.m_table[id].nIndex,m_table.m_table[id].nStep);
		m_index_node.Delete(iset,m_table.m_table[id].nNodeId);
		m_index_private.Delete(iset,m_table.m_table[id].nNodeId,m_table.m_table[id].cNodePrivateId);
		m_table.Delete(id);
		bret=iset.Next(id);
	}
	return true;
}

bool CRouteTbl::SelectByNode(int nodeid,std::vector<S_DREB_ROUTE *> &rt)
{
	CInt iset;
	CBF_PMutex pp(&m_mutex);
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

bool CRouteTbl::SelectByIndex(int index, std::vector<S_DREB_ROUTE *> &rt)
{
	CInt iset;
	CBF_PMutex pp(&m_mutex);
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

bool CRouteTbl::SelectByPrivateNode(int nodeid, int privateid, std::vector<S_DREB_ROUTE *> &rt)
{
	CInt iset;
	CBF_PMutex pp(&m_mutex);
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

bool CRouteTbl::UpdateRoute(S_DREB_ROUTE rt)
{
	CInt iset;
	CBF_PMutex pp(&m_mutex);
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

bool CRouteTbl::GetRouteByIndex(int index, std::vector<S_DREB_ROUTE *> &rtlist)
{
	bool bRet;
	int id;
	CBF_PMutex pp(&m_mutex);
	CInt iset;
	m_index_index.Select(iset,index);
	bRet = iset.First(id);
	if (!bRet)
	{
		return false;
	}
	while (bRet)
	{
		rtlist.push_back(&(m_table.m_table[id]));
		bRet = iset.Next(id);
	}
	return true;
}

bool  CRouteTbl::SelectById(std::vector<S_DREB_ROUTE*>& rtlist)
{
    int id;
	CBF_PMutex pp(&m_mutex);
	bool bret = m_index_private.First(id);
    if (!bret)
    {
        return false;
    }
	while (bret)
	{
		rtlist.push_back(&(m_table.m_table[id]));
		bret = m_index_private.Next(id);
	}
	return true;
}
