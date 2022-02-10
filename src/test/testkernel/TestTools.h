// TestTools.h: interface for the CTestTools class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTTOOLS_H__960D6402_0090_4B6E_831F_82DBDDB1ADC0__INCLUDED_)
#define AFX_TESTTOOLS_H__960D6402_0090_4B6E_831F_82DBDDB1ADC0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "BF_Tools.h"

class CTestTools  
{
public:
	void Test()
	{
		//测试BF_Tools
		std::string ss="     1 23456 7      ";
		CBF_Tools ptool;
		printf("测试trim [%s]\n",ss.c_str());
		ptool.Ltrim(ss);
		printf("Ltrim:[%s]\n",ss.c_str());
		
		ptool.Rtrim(ss);
		printf("Rtrim:[%s]\n",ss.c_str());
		
		double dd = 99.404444499999;
		printf("测试%.12f: Round.6 [%lf]\n",dd,ptool.Round(dd,6));
		
		char datastr[11];
		CBF_Tools::ToBinaryString(211,datastr);
		printf("转二进制字符串ToBinaryString %d = %sB\n",211,datastr);
		printf("二进制字符串转为整型StringToInteger %sB = %ld \n",datastr,CBF_Tools::StringToInteger(datastr,2));

	}
	CTestTools()
	{

	}
	virtual ~CTestTools()
	{

	}

};

#endif // !defined(AFX_TESTTOOLS_H__960D6402_0090_4B6E_831F_82DBDDB1ADC0__INCLUDED_)
