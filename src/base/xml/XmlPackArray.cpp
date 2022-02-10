// XmlPackArray.cpp: implementation of the CXmlPackArray class.
//
//////////////////////////////////////////////////////////////////////
#include "XmlPackArray.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXmlPackArray::CXmlPackArray()
{
}

CXmlPackArray::~CXmlPackArray()
{
	clear();
}
bool CXmlPackArray::push_back(CXmlPack *xmlpack)
{
	CXmlPack *pushtmp = new CXmlPack();
	pushtmp->XmlCopy(xmlpack);
	m_xmlarray.push_back(pushtmp);
	pushtmp = NULL ;
	return true;
}
CXmlPack *CXmlPackArray::push_back()
{
	CXmlPack *pushtmp = new CXmlPack();
	m_xmlarray.push_back(pushtmp);
	return pushtmp;
}
void CXmlPackArray::pop_back()
{
	CXmlPack *poptmp =NULL;
	poptmp = m_xmlarray.back();
	m_xmlarray.pop_back();
	if (poptmp != NULL)
	{
		delete poptmp;
		poptmp = NULL;
	}
	return;
}
bool CXmlPackArray::GetAt(int rec,CXmlPack *xmlpack)
{
	if (rec >= (int)m_xmlarray.size())
	{
		return false;
	}
	xmlpack = m_xmlarray[rec];
	return true;
}
CXmlPack *CXmlPackArray::GetAt(int rec)
{
	if (rec >= (int)m_xmlarray.size())
	{
		return NULL;
	}
	return  m_xmlarray[rec];
}
CXmlPack *CXmlPackArray::at(int rec)
{
	if (rec >= (int)m_xmlarray.size())
	{
		return NULL;
	}
	return  m_xmlarray[rec];
}
void CXmlPackArray::clear()
{
	CXmlPack *pusttmp = NULL;
	for (int i=0;i< (int)m_xmlarray.size();i++)
	{
		pusttmp  = m_xmlarray.at(i);
		delete pusttmp;
		pusttmp = NULL;
	}
	m_xmlarray.clear();
	
}


int CXmlPackArray::size()
{
	return m_xmlarray.size();
}

bool CXmlPackArray::PackArrayCopy(CXmlPackArray *muxmlpack)
{
	CXmlPack *pusttmp = NULL;
	for (int i=0;i<muxmlpack->size();i++)
	{
		pusttmp  = muxmlpack->at(i);
		m_xmlarray.push_back(pusttmp);
	}
	muxmlpack->ClearPoint();
	return true;
}

void CXmlPackArray::ClearPoint()
{
	m_xmlarray.clear();
}
