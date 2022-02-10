// TestDateTime.h: interface for the CTestDateTime class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTDATETIME_H__91C09EC7_7C77_4DBC_8CCC_01F4BA1E0869__INCLUDED_)
#define AFX_TESTDATETIME_H__91C09EC7_7C77_4DBC_8CCC_01F4BA1E0869__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BF_Date_Time.h"

class CTestDateTime  
{
public:
	void Test()
	{
		int tlong=100000000;
		CBF_Date_Time dd;
		CBF_Date_Time tt;
		//测试Set
		printf("---测试Set方法 2012 22 40\n");
		dd.Set(2012,22,40);
		printf("测试设置日期 %s  week is %ld \n",(dd.ToStringD8()).c_str(),dd.GetWeek());
		printf("---测试Set方法 20120201\n");
		dd.Set("20120201");
		printf("测试设置日期 %s  week is %d \n",dd.ToStringD8().c_str(),dd.GetWeek());
		
		printf("---测试日期相减 当前-20120201\n");
		tlong = tt-dd;
		printf("测试-  日期:%s %s  时间差%d\n",dd.ToStringD8().c_str(),tt.ToStringD8().c_str(),tlong);
		if (dd > tt)
		{
			printf("测试>  %s > %s\n",dd.ToStringD8().c_str(),tt.ToStringD8().c_str());
		}
		if (dd < tt)
		{
			printf("测试< %s < %s\n",dd.ToStringD8().c_str(),tt.ToStringD8().c_str());
		}
		tt = dd;
		printf("---测试日期= 当前=20120201\n");
		printf("测试= 日期:%s %s\n",tt.ToStringD8().c_str(),dd.ToStringD8().c_str());
		
		printf("测试+= 日期:%s  %s\n",dd.ToStringD8().c_str(),(dd+=5).c_str());
		printf("测试-= 日期:%s  %s\n",dd.ToStringD8().c_str(),(dd-=5).c_str());
		printf("测试+= 日期:%s  %s\n",dd.ToStringD8().c_str(),(dd+=5).c_str());
		printf("测试++前日期:%s  \n",dd.ToStringD8().c_str());
		
		printf("---测试日期++ 366天 \n");
		for (int j=0;j<366;j++)
		{
			printf("测试++ 日期:%s  \n",(dd++).ToStringD8().c_str());
		}
		printf("测试--366 前日期:%s  \n",dd.ToStringD8().c_str());
		for (int k=0;k<366;k++)
		{
			printf("测试-- 日期:%s  \n",(dd--).ToStringD8().c_str());
		}
	}
	CTestDateTime()
	{

	}
	virtual ~CTestDateTime()
	{

	}

};

#endif // !defined(AFX_TESTDATETIME_H__91C09EC7_7C77_4DBC_8CCC_01F4BA1E0869__INCLUDED_)
