// DrebApiML.cpp: implementation of the CDrebApiML class.
//
//////////////////////////////////////////////////////////////////////

#include "DrebApiML.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDrebApiML::CDrebApiML()
{

}

CDrebApiML::~CDrebApiML()
{
	int id;
	bool bRet;
	bRet = m_key.First(id);
	while (bRet)
	{
		if (m_table.m_table[id] != NULL)
		{
			delete m_table.m_table[id];
			m_table.m_table[id] = NULL;
		}
		bRet = m_key.Next(id);
	}
}

int CDrebApiML::Insert(CBF_DrebServerApi *point)
{
	int id = m_table.Add(point);
	CInt iset;
	if (!m_key.Select(iset,id))
	{
		m_key.Add(id,id);
	}
	else
	{
		m_key.Delete(id);
		m_key.Add(id,id);
	}
	return id;
}

bool CDrebApiML::Delete(int id)
{
	CInt iset;
	if (!m_key.Select(iset,id))
	{
		return false;
	}
	m_key.Delete(id);
	m_table.Delete(id);
	return true;
}

CBF_DrebServerApi * CDrebApiML::Find(int id)
{
	CInt iset;
	if (!m_key.Select(iset,id))
	{
		return NULL;
	}
	return m_table.m_table[id];
}
