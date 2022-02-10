// XmlAttrIndex.cpp: implementation of the CXmlAttrIndex class.
//
//////////////////////////////////////////////////////////////////////

#include "XmlAttrIndex.h"
#include "XmlAttribute.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXmlAttrIndex::CXmlAttrIndex()
{

}

CXmlAttrIndex::~CXmlAttrIndex()
{

}
bool CXmlAttrIndex::Insert(CXmlAttribute *attr)
{
	int id;
	S_TBL_ATTR_INDEX tmptbl;
	tmptbl.pXmlAttr = attr;
	//	tmptbl.sAttrName = attr->GetAttrName(false);
	if (m_pkey.Find(id,(char *)attr->GetAttrName(false).c_str())) //已有此属性
	{
		return false;
	}
	id = m_table.Add(tmptbl);
	m_pkey.Add(id,(char *)m_table.m_table[id].sAttrName.c_str());
	return true;
}

bool CXmlAttrIndex::Delete(CXmlAttribute *attr)
{
	CInt iset;
	if (!m_pkey.Select(iset,(char *)attr->GetAttrName(false).c_str()))
	{
		return false;
	}
	int id;
	iset.First(id);
	m_pkey.Delete((char *)attr->GetAttrName(false).c_str());
	m_table.Delete(id);
	return true;
}

CXmlAttribute* CXmlAttrIndex::SelectAttribute(const char *attrname)
{
	CInt iset;
	if (!m_pkey.Select(iset,(char *)attrname))
	{
		return NULL;
	}
	int id;
	iset.First(id);
	return m_table.m_table[id].pXmlAttr;
}

void CXmlAttrIndex::Clear()
{
	m_pkey.Clear();
	m_table.Clear();
}
