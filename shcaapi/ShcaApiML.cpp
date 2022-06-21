// ShcaApiML.cpp: implementation of the CDrebApiML class.
//
//////////////////////////////////////////////////////////////////////

#include "ShcaApiML.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShcaApiML::CShcaApiML()
{

}

CShcaApiML::~CShcaApiML()
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

int CShcaApiML::Insert(CShcaApi *point)
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

bool CShcaApiML::Delete(int id)
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

CShcaApi * CShcaApiML::Find(int id)
{
	CInt iset;
	if (!m_key.Select(iset,id))
	{
		return NULL;
	}
	return m_table.m_table[id];
}
