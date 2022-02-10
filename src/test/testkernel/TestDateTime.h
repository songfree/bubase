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
		//����Set
		printf("---����Set���� 2012 22 40\n");
		dd.Set(2012,22,40);
		printf("������������ %s  week is %ld \n",(dd.ToStringD8()).c_str(),dd.GetWeek());
		printf("---����Set���� 20120201\n");
		dd.Set("20120201");
		printf("������������ %s  week is %d \n",dd.ToStringD8().c_str(),dd.GetWeek());
		
		printf("---����������� ��ǰ-20120201\n");
		tlong = tt-dd;
		printf("����-  ����:%s %s  ʱ���%d\n",dd.ToStringD8().c_str(),tt.ToStringD8().c_str(),tlong);
		if (dd > tt)
		{
			printf("����>  %s > %s\n",dd.ToStringD8().c_str(),tt.ToStringD8().c_str());
		}
		if (dd < tt)
		{
			printf("����< %s < %s\n",dd.ToStringD8().c_str(),tt.ToStringD8().c_str());
		}
		tt = dd;
		printf("---��������= ��ǰ=20120201\n");
		printf("����= ����:%s %s\n",tt.ToStringD8().c_str(),dd.ToStringD8().c_str());
		
		printf("����+= ����:%s  %s\n",dd.ToStringD8().c_str(),(dd+=5).c_str());
		printf("����-= ����:%s  %s\n",dd.ToStringD8().c_str(),(dd-=5).c_str());
		printf("����+= ����:%s  %s\n",dd.ToStringD8().c_str(),(dd+=5).c_str());
		printf("����++ǰ����:%s  \n",dd.ToStringD8().c_str());
		
		printf("---��������++ 366�� \n");
		for (int j=0;j<366;j++)
		{
			printf("����++ ����:%s  \n",(dd++).ToStringD8().c_str());
		}
		printf("����--366 ǰ����:%s  \n",dd.ToStringD8().c_str());
		for (int k=0;k<366;k++)
		{
			printf("����-- ����:%s  \n",(dd--).ToStringD8().c_str());
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
