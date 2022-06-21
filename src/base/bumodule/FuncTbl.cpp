// FuncTbl.cpp: implementation of the CFuncTbl class.
//
//////////////////////////////////////////////////////////////////////

#include "FuncTbl.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFuncTbl::CFuncTbl()
{
	memset(&m_pNull,0,sizeof(S_TBL_FUNC));
	m_pNull.nFuncNo = 0;
}

CFuncTbl::~CFuncTbl()
{
	m_pkey.Clear();
	m_table.Clear();
}

bool CFuncTbl::Insert(S_TBL_FUNC funcinfo)
{
	int id;
	//通过主键查找，不存在则增加
	if (!m_pkey.Find(funcinfo.nFuncNo))
	{
		id = m_table.Add(funcinfo);//增加到表
		m_pkey.Add(id,funcinfo.nFuncNo);//增加主键
		return true;
	}
	return false;
}

bool CFuncTbl::Update(S_TBL_FUNC funcinfo)
{
	int id;
	if (!m_pkey.Select(id,funcinfo.nFuncNo))
	{
		return false;
	}
	m_table.m_table[id] = funcinfo;
	return true;
}

bool CFuncTbl::Select(int nFuncNo, S_TBL_FUNC &funcinfo)
{
	int id;
	if (!m_pkey.Select(id,nFuncNo))
	{
		return false;
	}
	funcinfo = m_table.m_table[id];
	return true;
}
S_TBL_FUNC & CFuncTbl::Find(int nFuncNo)
{
	int id;
	if (!m_pkey.Select(id,nFuncNo))
	{
		return m_pNull;
	}
	return m_table.m_table[id];
	
}
bool CFuncTbl::First(S_TBL_FUNC &funcinfo)
{
	return m_table.First(funcinfo);
}

bool CFuncTbl::Next(S_TBL_FUNC &funcinfo)
{
	return m_table.Next(funcinfo);
}

void CFuncTbl::Clear()
{
	m_table.Clear();
	m_pkey.Clear();
}





CFuncTblOld::CFuncTblOld()
{
	memset(&m_pNull,0,sizeof(S_TBL_FUNC_OLD));
	m_pNull.nFuncNo = 0;
}

CFuncTblOld::~CFuncTblOld()
{
	m_pkey.Clear();
	m_table.Clear();
}

bool CFuncTblOld::Insert(S_TBL_FUNC_OLD funcinfo)
{
	int id;
	//通过主键查找，不存在则增加
	if (!m_pkey.Find(id,funcinfo.nFuncNo))
	{
		id = m_table.Add(funcinfo);//增加到表
		m_pkey.Add(id,funcinfo.nFuncNo);//增加主键
		return true;
	}
	return false;
}

bool CFuncTblOld::Update(S_TBL_FUNC_OLD funcinfo)
{
	CInt iset;
	if (!m_pkey.Select(iset,funcinfo.nFuncNo))
	{
		return false;
	}
	int id;
	iset.First(id);
	m_table.m_table[id] = funcinfo;
	return true;
}

bool CFuncTblOld::Select(int nFuncNo, S_TBL_FUNC_OLD &funcinfo)
{
	CInt iset;
	if (!m_pkey.Select(iset,nFuncNo))
	{
		return false;
	}
	int id;
	iset.First(id);
	funcinfo = m_table.m_table[id];
	return true;
}
S_TBL_FUNC_OLD & CFuncTblOld::Find(int nFuncNo)
{
	CInt iset;
	if (!m_pkey.Select(iset,nFuncNo))
	{
		return m_pNull;
	}
	int id;
	iset.First(id);
	return m_table.m_table[id];
	
}
bool CFuncTblOld::First(S_TBL_FUNC_OLD &funcinfo)
{
	return m_table.First(funcinfo);
}

bool CFuncTblOld::Next(S_TBL_FUNC_OLD &funcinfo)
{
	return m_table.Next(funcinfo);
}

void CFuncTblOld::Clear()
{
	m_table.Clear();
	m_pkey.Clear();
}
