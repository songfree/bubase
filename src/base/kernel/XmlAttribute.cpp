// XmlAttribute.cpp: implementation of the CXmlAttribute class.
//
//////////////////////////////////////////////////////////////////////

#include "XmlAttribute.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXmlAttribute::CXmlAttribute()
{
	m_next = NULL;
	m_last = NULL;
}

CXmlAttribute::~CXmlAttribute()
{
	DeletePoint();
}

CXmlAttribute * CXmlAttribute::getNextSibling()
{
	return this->m_next;
}
CXmlAttribute * CXmlAttribute::getLastSibling()
{
	return this->m_last;
}
std::string CXmlAttribute::GetAttrName(bool isTransSign)
{
	if (!isTransSign)
	{
		return m_sAttrName;
	}
	else
	{
		std::string ss = m_sAttrName;
		XmlgetTransSign(ss);
		return ss;
	}
}

std::string CXmlAttribute::GetAttrValue(bool isTransSign)
{
	if (!isTransSign)
	{
		return m_sAttrValue;
	}
	else
	{
		std::string ss = m_sAttrValue;
		XmlgetTransSign(ss);
		return ss;
	}
}

void CXmlAttribute::SetAttrName(std::string attrname, bool isTransSign)
{
	m_sAttrName= attrname;
	if (isTransSign)
	{
		XmlputTransSign(m_sAttrName);
	}
}

void CXmlAttribute::SetAttrValue(std::string value, bool isTransSign)
{
	m_sAttrValue= value;
	if (isTransSign)
	{
		XmlputTransSign(m_sAttrValue);
	}
}

void CXmlAttribute::SetNextAttribute(CXmlAttribute *nextattr)
{
	m_next = nextattr;
}



void CXmlAttribute::SetLastAttribute(CXmlAttribute *lastattr)
{
	m_last = lastattr;
}

void CXmlAttribute::DeletePoint()
{
	if (m_next == NULL && m_last == NULL) //ǰ�����Զ�Ϊ�գ�ʲô������
	{
		
	}
	else if (m_next == NULL)//�����һ������
	{
		m_last->SetNextAttribute(NULL);
		m_last = NULL;
	}
	else if (getLastSibling() == NULL) //����ǰһ������
	{
		m_next->SetLastAttribute(NULL);
		m_next = NULL;	
		
	}
	else //���м������
	{
		m_next->SetLastAttribute(m_last);
		m_last->SetNextAttribute(m_next);
		m_next = NULL;
		m_last = NULL;
	}
}
