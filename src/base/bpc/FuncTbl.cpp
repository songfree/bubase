// FuncTbl.cpp: implementation of the CFuncTbl class.
//
//////////////////////////////////////////////////////////////////////

#include "FuncTbl.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFuncTbl::CFuncTbl()
{
	bzero(&m_null,sizeof(S_FUNCINFO_TBL));
	m_null.nCallNum = 0;
}

CFuncTbl::~CFuncTbl()
{

}
// ������: Next
// ���  : ������ 2010-7-27 11:26:34
// ����  : bool 
// ����  : S_FUNCINFO_TBL &fn
// ����  : ��һ������
bool CFuncTbl::Next(S_FUNCINFO_TBL &fn)
{
	bool bRet;
	int id;
	bRet = m_pkey.Next(id);
	if (!bRet)
	{
		return false;
	}
	fn = m_table.m_table[id];
	fn.cPrio = m_table.m_table[id].cPrio;
	fn.cServiceFlag = m_table.m_table[id].cServiceFlag;
	fn.nFuncNo = m_table.m_table[id].nFuncNo;
	strcpy(fn.sBpuGroupName,m_table.m_table[id].sBpuGroupName);
	strcpy(fn.sFuncName,m_table.m_table[id].sFuncName);
	return true;
}

// ������: First
// ���  : ������ 2010-7-27 11:26:23
// ����  : bool 
// ����  : S_FUNCINFO_TBL &fn
// ����  : ��һ������
bool CFuncTbl::First(S_FUNCINFO_TBL &fn)
{
	bool bRet;
	int id;
	bRet = m_pkey.First(id);
	if (!bRet)
	{
		return false;
	}
//	fn = m_table.m_table[id];
	fn.cPrio = m_table.m_table[id].cPrio;
	fn.cServiceFlag = m_table.m_table[id].cServiceFlag;
	fn.nFuncNo = m_table.m_table[id].nFuncNo;
	strcpy(fn.sBpuGroupName,m_table.m_table[id].sBpuGroupName);
	strcpy(fn.sFuncName,m_table.m_table[id].sFuncName);
	return bRet;
}

// ������: SelectByPk
// ���  : ������ 2010-7-27 11:26:15
// ����  : bool 
// ����  : unsigned int func
// ����  : S_FUNCINFO_TBL &fn
// ����  : ��ȡ����
bool CFuncTbl::Select(unsigned int func,S_FUNCINFO_TBL &fn)
{
	int id;
	CBF_PMutex pp(&m_mutex);
	if (!m_pkey.Select(id,func))
	{
		return false;
	}
	fn.cPrio = m_table.m_table[id].cPrio;
	fn.cServiceFlag = m_table.m_table[id].cServiceFlag;
	fn.nFuncNo = m_table.m_table[id].nFuncNo;
	strcpy(fn.sBpuGroupName,m_table.m_table[id].sBpuGroupName);
	strcpy(fn.sFuncName,m_table.m_table[id].sFuncName);
	return true;
}

// ������: Delete
// ���  : ������ 2010-7-27 11:26:06
// ����  : bool 
// ����  : unsigned int func
// ����  : ɾ��һ������
bool CFuncTbl::Delete(unsigned int func)
{
	CBF_PMutex pp(&m_mutex);
	int id;
	if (!m_pkey.Select(id,func))
	{
		return false;
	}
	CInt iset;
	iset.Add(id);
	m_pkey.Delete(func);
	m_indexgroup.Delete(iset,m_table.m_table[id].sBpuGroupName);
	m_table.Delete(id);
	return true;
}

// ������: Update
// ���  : ������ 2010-7-27 11:25:38
// ����  : bool 
// ����  : S_FUNCINFO_TBL fn
// ����  : ���¹���
bool CFuncTbl::Update(S_FUNCINFO_TBL fn)
{
	CBF_PMutex pp(&m_mutex);
	int id;
	if (!m_pkey.Select(id,fn.nFuncNo))
	{
		return false;
	}

	m_table.m_table[id].cPrio = fn.cPrio;
	m_table.m_table[id].cServiceFlag = fn.cServiceFlag ;
	m_table.m_table[id].nFuncNo =fn.nFuncNo;
	strcpy(m_table.m_table[id].sBpuGroupName ,fn.sBpuGroupName) ;
	strcpy(m_table.m_table[id].sFuncName ,fn.sFuncName);
	return true;
}

// ������: Insert
// ���  : ������ 2010-7-27 11:25:25
// ����  : bool 
// ����  : S_FUNCINFO_TBL fn
// ����  : insertһ������
bool CFuncTbl::Insert(S_FUNCINFO_TBL fn)
{
	int id;
	CBF_PMutex pp(&m_mutex);
	//ͨ���������ң�������������
	if (!m_pkey.Find(fn.nFuncNo))
	{
		id = m_table.Add(fn);//���ӵ���
		m_pkey.Add(id, m_table.m_table[id].nFuncNo);//��������
		m_indexgroup.Add(id,m_table.m_table[id].sBpuGroupName);
		return true;
	}
	return false;
}

S_FUNCINFO_TBL & CFuncTbl::FindFunc(unsigned int func,int &findflag)
{
	CBF_PMutex pp(&m_mutex);
	int id;
	if (!m_pkey.Select(id,func))
	{
		findflag = 0;
		return m_null;
	}
	findflag = 1;
	m_table.m_table[id].nCallNum++;
	return m_table.m_table[id];
}

int CFuncTbl::GetAllFunc(std::vector<S_FUNCINFO_TBL> &funclist)
{
	CBF_PMutex pp(&m_mutex);
	bool bRet;
	int id;
	bRet = m_pkey.First(id);
	if (!bRet)
	{
		return 0;
	}
	while (bRet)
	{
		funclist.push_back(m_table.m_table[id]);
		bRet = m_pkey.Next(id);
	}
	return funclist.size();
}
int CFuncTbl::GetAllFuncByGroup(std::vector<S_FUNCINFO_TBL> &funclist)
{
	CBF_PMutex pp(&m_mutex);
	bool bRet;
	int id;
	bRet = m_indexgroup.First(id);
	if (!bRet)
	{
		return 0;
	}
	while (bRet)
	{
		funclist.push_back(m_table.m_table[id]);
		bRet = m_indexgroup.Next(id);
	}
	return funclist.size();
}