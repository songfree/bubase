// IVarDefine.cpp: implementation of the CIVarDefine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "drebpmt.h"
#include "IVarDefine.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIVarDefine::CIVarDefine()
{
	m_varValueNo = 0;
	m_lValue = 0;
	m_varType = 0;
	m_varLen =8;
	m_maxVarValue = (__int64)pow((double)10, m_varLen+1)-1;
}

CIVarDefine::~CIVarDefine()
{

}

__int64 CIVarDefine::GetVarValue()
{
	CBF_PMutex pp(&m_mutex);
	
	if (m_lValue>= m_maxVarValue)
	{
		m_lValue = 1;
	}
	else
	{
		m_lValue++;
	}
	return m_lValue;
}
std::string CIVarDefine::GetVarValueS()
{
	CBF_PMutex pp(&m_mutex);
	if (m_varType == 1) //Ëæ»úÑ¡Ôñ
	{
		srand(GetTickCount());
		m_varValueNo = rand() % (m_varValueList.size());
		//srand(time(0));
		//m_varValueNo = rand() % (maxfunc - minfunc + 1) + minfunc; 
		return m_varValueList[m_varValueNo];
	}
	else
	{
		if (m_varValueList.size()<1 )
		{
			return "";
		}
		if (m_varValueNo > m_varValueList.size()-1 || m_varValueNo<0)
		{
			return "";
		}
		if (m_varValueNo == m_varValueList.size()-1 )
		{
			m_varValueNo=0;
		}
		else
		{
			m_varValueNo++;
		}
		return m_varValueList[m_varValueNo];

	}
	return "";
}
void CIVarDefine::IntVarInit(std::string fmt, __int64 initvalue)
{
	m_fmt = fmt;
	m_lValue = initvalue;
}

std::string & CIVarDefine::GetVarName()
{
	return m_sVarName;
}

int CIVarDefine::GetVarType()
{
	return m_varType;
}

bool CIVarDefine::PushVarValue(std::string svalue)
{
	if (m_varType>0)
	{
		m_varValueList.push_back(svalue);
		return true;
	}
	else
	{
		return false;
	}
}

void CIVarDefine::SetVarPara(std::string name,int type,int len)
{
	m_sVarName = name;
	if (type<0 || type>2)
	{
		m_varType= 0;
	}
	else
	{	
		m_varType  = type;
	}
	m_varLen = len;
	m_maxVarValue = (__int64)pow((double)10, m_varLen+1)-1;
}

std::string & CIVarDefine::GetFmt()
{
	return m_fmt;
}

int CIVarDefine::GetVarLen()
{
	return m_varLen;
}
