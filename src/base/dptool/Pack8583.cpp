// Pack8583.cpp: implementation of the CPack8583 class.
//
//////////////////////////////////////////////////////////////////////

#include "Pack8583.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPack8583::CPack8583()
{
	m_bitLen = 0;
	memset(m_errMsg,0,sizeof(m_errMsg));
}
CPack8583::CPack8583(char *xmldefine)
{
	m_bitLen = 0;
	memset(m_errMsg,0,sizeof(m_errMsg));
	LoadPackDef(xmldefine);
}
CPack8583::~CPack8583()
{
	
}

void CPack8583::New8583()
{
	memset(m_packHead,0,sizeof(m_packHead));
	m_vpack8583.clear();
}

bool CPack8583::SetFieldValue(const int idx, const char *value)
{
	return true;
}

bool CPack8583::SetFieldValue(const int idx, const std::string value)
{
	return true;
}

bool CPack8583::SetFieldValue(const int idx, const int value)
{
	return true;
}

bool CPack8583::SetFieldValue(const int idx, const double value)
{
	return true;
}


bool CPack8583::SetFieldValue(const char *name, const char *value)
{
	return true;
}

bool CPack8583::SetFieldValue(const char *name, const std::string value)
{
	return true;
}

bool CPack8583::SetFieldValue(const char *name, const int value)
{
	return true;
}

bool CPack8583::SetFieldValue(const char *name, const double value)
{
	return true;
}

bool CPack8583::GetPackBuffer(char *buffer, int &buflen)
{
	return true;
}

bool CPack8583::LoadPackDef(char *xmlfile)
{
	if (m_xml8583.fromFile(xmlfile)<0)
	{
		m_bIsLoad = false;
		sprintf(m_errMsg,"装载定义文件失败，非xml或文件不存在");
		return false;
	}
	m_fieldNum = 0;
	m_vpack8583def.clear();
	m_vpack8583.clear();
	memset(m_packHead,0,sizeof(m_packHead));
	m_xml8583.getHeadValue("位图长度",m_bitLen);
	m_xml8583.gotoPack();
	//取出配置的字段数
	_struct8583def tmpdef;
	CNode *tmpnode = m_xml8583.m_xml.getFirstChildItem();
	while (tmpnode != NULL)
	{
		tmpdef.idx = m_fieldNum;
		tmpdef.pfield = tmpnode;
		m_vpack8583def.push_back(tmpdef);
		m_fieldNum++;
		tmpnode = m_xml8583.m_xml.getNextSibling();
	}
	m_bIsLoad = true;
		 
	return true;
}
