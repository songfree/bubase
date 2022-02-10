// XmlAttributes.cpp: implementation of the CXmlAttributes class.
//
//////////////////////////////////////////////////////////////////////

#include "XmlAttributes.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXmlAttributes::CXmlAttributes()
{
	m_pFirstAttribute = NULL;
	m_pEndAttribute = NULL;
	m_bIsUseIndex = false;
	m_index = NULL;
}

CXmlAttributes::~CXmlAttributes()
{
	Clear();
	DeleteAllAttribute();
}
CXmlAttribute * CXmlAttributes::AddAttribute(std::string attrname, bool isTransSign, std::string attrvlaue, bool isTransValue)
{
	std::string aname= attrname;
	if (isTransSign)
	{
		XmlputTransSign(aname);
	}
	CXmlAttribute *pattr=new CXmlAttribute();
	pattr->SetAttrName(attrname,isTransSign);
	pattr->SetAttrValue(attrvlaue,isTransValue);
	
	if (m_pEndAttribute != NULL) //不是最后属性	
	{
		m_pEndAttribute->SetNextAttribute(pattr);//原最后属性的下属性为此属性
		pattr->SetLastAttribute(m_pEndAttribute); //此属性的临近上属性为原最后一属性
		m_pEndAttribute = pattr;
	}
	else
	{
		m_pFirstAttribute = pattr;
		m_pEndAttribute = pattr;
	}
	return pattr;
}

CXmlAttribute * CXmlAttributes::SetAttribute(std::string attrname, bool isTransSign, std::string attrvlaue, bool isTransValue)
{
	std::string aname= attrname;
	if (isTransSign)
	{
		XmlputTransSign(aname);
	}
	CXmlAttribute *pattr=NULL;
	if (m_bIsUseIndex)
	{
		pattr = m_index->SelectAttribute(aname.c_str());
		if (pattr != NULL)
		{
			pattr->SetAttrValue(attrvlaue,isTransValue);
			return pattr;
		}
	}
	else
	{
		pattr = m_pFirstAttribute;
		while (pattr != NULL)
		{
			if (aname.compare(pattr->GetAttrName(false))==0)
			{
				pattr->SetAttrValue(attrvlaue,isTransValue);
				return pattr;
			}
			pattr = pattr->getNextSibling();
		}
	}
	
	pattr = new CXmlAttribute();
	pattr->SetAttrName(attrname,isTransSign);
	pattr->SetAttrValue(attrvlaue,isTransValue);
	
	if (m_pEndAttribute != NULL) //不是最后属性	
	{
		m_pEndAttribute->SetNextAttribute(pattr);//原最后属性的下属性为此属性
		pattr->SetLastAttribute(m_pEndAttribute); //此属性的临近上属性为原最后一属性
		m_pEndAttribute = pattr;
	}
	else
	{
		m_pFirstAttribute = pattr;
		m_pEndAttribute = pattr;
	}
	if (m_bIsUseIndex)
	{
		m_index->Insert(pattr);
	}
	return pattr;

}

CXmlAttribute * CXmlAttributes::SetAttribute(std::string attrname, bool isTransSign, int attrvlaue)
{
	char svalue[30];
	sprintf(svalue,"%d",attrvlaue);
	return SetAttribute(attrname,isTransSign,svalue,false);
	
}
CXmlAttribute * CXmlAttributes::SetAttribute(std::string attrname, bool isTransSign, long attrvlaue)
{
	char svalue[30];
	sprintf(svalue,"%ld",attrvlaue);
	return SetAttribute(attrname,isTransSign,svalue,false);
	
}
CXmlAttribute * CXmlAttributes::SetAttribute(std::string attrname, bool isTransSign, double attrvlaue,int dec)
{
	char svalue[30];
	char sfmat[20];
	sprintf(sfmat,"%s.%d%s","%",dec,"f");
	sprintf(svalue,sfmat,attrvlaue);
	return SetAttribute(attrname,isTransSign,svalue,false);
	
}
CXmlAttribute * CXmlAttributes::GetAttribute(std::string attrname, bool isTransSign)
{
	std::string aname= attrname;
	if (isTransSign)
	{
		XmlputTransSign(aname);
	}
	CXmlAttribute *pattr = NULL;
	if (m_bIsUseIndex)
	{
		pattr = m_index->SelectAttribute(aname.c_str());
		if (pattr != NULL)
		{
			return pattr;
		}
	}
	else
	{
		pattr = m_pFirstAttribute;
		while (pattr != NULL)
		{
			if (aname.compare(pattr->GetAttrName(false))==0)
			{
				return pattr;
			}
			pattr = pattr->getNextSibling();
		}
	}
	return NULL;
}

CXmlAttribute * CXmlAttributes::GetAttribute(std::string attrname, bool isTransSign, std::string &attrvalue, bool isTransValue)
{
	std::string aname= attrname;
	if (isTransSign)
	{
		XmlputTransSign(aname);
	}
	CXmlAttribute *pattr=NULL;
	if (m_bIsUseIndex)
	{
		pattr = m_index->SelectAttribute(aname.c_str());
		if (pattr == NULL)
		{
			return NULL;
		}
		attrvalue = pattr->GetAttrValue(isTransValue);
		return pattr;
	}
	else
	{
		pattr = m_pFirstAttribute;
		while (pattr != NULL)
		{
			if (aname.compare(pattr->GetAttrName(false))==0)
			{
				attrvalue = pattr->GetAttrValue(isTransValue);
				return pattr;
			}
			pattr = pattr->getNextSibling();
		}
		return NULL;
	}
	return NULL;
}

void CXmlAttributes::Clear()
{
	if (m_index != NULL)
	{
		m_index->Clear();
	}
	DeleteAllAttribute();
}

void CXmlAttributes::DeleteAttributePoint(CXmlAttribute *attr)
{
	if (attr == m_pFirstAttribute && attr == m_pEndAttribute)
	{
		m_pFirstAttribute = NULL;
		m_pEndAttribute = NULL;
	}
	else if (attr == m_pFirstAttribute)
	{
		m_pFirstAttribute = attr->getNextSibling();
	}
	else if (attr == m_pEndAttribute)
	{
		m_pEndAttribute = attr->getLastSibling();
	}
	attr->DeletePoint();
}

bool CXmlAttributes::DeleteAttribute(std::string attrname, bool isTransSign)
{
	std::string aname= attrname;
	if (isTransSign)
	{
		XmlputTransSign(aname);
	}
	CXmlAttribute *pattr =NULL;
	if (m_bIsUseIndex)
	{
		pattr= m_index->SelectAttribute(aname.c_str());
		if (pattr == NULL)
		{
			return false;
		}
		m_index->Delete(pattr);
		DeleteAttributePoint(pattr);
		delete pattr;
		pattr = NULL;
		return true;
	}
	else
	{
		pattr = m_pFirstAttribute;
		while (pattr != NULL)
		{
			if (aname.compare(pattr->GetAttrName(false))==0)
			{
				DeleteAttributePoint(pattr);
				delete pattr;
				pattr = NULL;
				return true;
			}
			pattr = pattr->getNextSibling();
		}
		return false;
	}
	return false;
}

void CXmlAttributes::DeleteAllAttribute()
{
	CXmlAttribute *tmppoint;
	while (m_pFirstAttribute != NULL)
	{
		tmppoint = m_pFirstAttribute;
		m_pFirstAttribute = m_pFirstAttribute->getNextSibling();
		DeleteAttributePoint(tmppoint);
		delete tmppoint;
		tmppoint = NULL;
	}
	m_pFirstAttribute = NULL;
	m_pEndAttribute = NULL;
}

CXmlAttribute * CXmlAttributes::GetFirstAttribute()
{
	return m_pFirstAttribute;
}

CXmlAttribute * CXmlAttributes::GetEndAttribute()
{
	return m_pEndAttribute;
}

bool CXmlAttributes::CreateIndex()
{
	bool bres;
	m_bIsUseIndex = true;
	if (m_index == NULL)
	{
		m_index = new CXmlAttrIndex();
	}
	else
	{
		m_index->Clear();
	}
	
	CXmlAttribute *pattr = m_pFirstAttribute;
	while (pattr != NULL)
	{
		m_index->Insert(pattr);//重复的不做处理，通过索引找不到了
		pattr = pattr->getNextSibling();
	}
	return true;
}

bool CXmlAttributes::GetUseIndexFlag()
{
	return m_bIsUseIndex;
}
