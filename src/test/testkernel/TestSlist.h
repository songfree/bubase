// TestSlist.h: interface for the CTestSlist class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTSLIST_H__837B42BA_4F0C_4EF8_8E2F_3048E8FE27C0__INCLUDED_)
#define AFX_TESTSLIST_H__837B42BA_4F0C_4EF8_8E2F_3048E8FE27C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "BF_Slist.h"

class CTestSlist  
{
public:
	void Test()
	{
		m_pSlist.SetSeparateString("|");
		std::string ss = "aa||bb||cc||dd|| e";
		m_pSlist.FillSepString(ss);
		printf("数据=[%s]  ToString_0=[%s] \n",ss.c_str(),m_pSlist.ToString(0).c_str());
		printf("数据=[%s]  ToString_1=[%s] \n",ss.c_str(),m_pSlist.ToString(1).c_str());
		printf("数据=[%s]  ToString_2=[%s] \n",ss.c_str(),m_pSlist.ToString(2).c_str());
		printf("数据=[%s]  ToString_3=[%s] \n",ss.c_str(),m_pSlist.ToString(3).c_str());
		
		printf("数据=[%s]  ToString_3_1=[%s] \n",ss.c_str(),m_pSlist.ToStringException(1,3).c_str());
	}
	CTestSlist()
	{

	}
	virtual ~CTestSlist()
	{

	}
	
	CBF_Slist  m_pSlist;
};

#endif // !defined(AFX_TESTSLIST_H__837B42BA_4F0C_4EF8_8E2F_3048E8FE27C0__INCLUDED_)
