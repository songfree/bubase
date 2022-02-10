#include "public.h"
#include "attribute.h"

CAttribute::CAttribute()
{
	m_next = NULL ;
}
CAttribute::~CAttribute()
{
	m_next = NULL;
}
std::string CAttribute::getName()
{
	return m_name;
}

std::string CAttribute::getValue()
{
	return m_value;
}
int CAttribute::getName(std::string &name)
{
	name = m_name;
	return 0;
}

int CAttribute::getValue(std::string &value)
{
	value = m_value;
	return 0;
}
CAttribute *CAttribute::getNamedItem(const std::string pAttrName)
{
	CAttribute *tmpNode = this;
	while (tmpNode!=NULL)
	{
		if (tmpNode->m_name.compare(pAttrName)==0)
		{
			return tmpNode;
		}
		tmpNode = tmpNode->m_next;
	}

	return NULL;
}

bool CAttribute::deleteNextAttribute()
{
	if (this==NULL) return true;
	CAttribute *cur;
	while (m_next != NULL)
	{
		cur = m_next;
		m_next  = m_next->m_next;
		if (cur!=NULL)
		{
			delete cur;
		}
		cur = NULL;
	}
	m_next = NULL ;
	return true;
}
CAttribute * CAttribute::getNextSibling()
{
	return this->m_next;
}
