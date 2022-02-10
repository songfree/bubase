// TestTimer.h: interface for the CTestTimer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTTIMER_H__CBDE24CC_EC03_406B_941B_1845E1272098__INCLUDED_)
#define AFX_TESTTIMER_H__CBDE24CC_EC03_406B_941B_1845E1272098__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BF_Timer.h"
#include "BF_Tools.h"
//测试定时器

class CTestTimer  
{
public:
	CBF_Timer tmer;
	CBF_Date_Time tDate;
	
	CTestTimer()
	{
		
	}
	virtual ~CTestTimer()
	{
		tmer.Stop();
	}
	void Test()
	{
		char schar[20];
		bzero(schar,sizeof(schar));
begin:
		CBF_Tools::GetConsoleEnter("请输入定时器定时精度间隔ms",4,schar);
		if (atoi(schar)<1)
		{	
			goto begin;
		}
		tmer.Init(atoi(schar),true);
		int nCount=0;
		while (1)
		{
			CBF_Tools::GetConsoleEnter("请输入一个定时时间ms",6,schar);
			if (atoi(schar)<1)
			{	
				printf("\n定时时间不符!");
				continue;;
			}
			tmer.SetTimer(nCount,atoi(schar),&OnTimer,this);
			printf("已设置一个定时[%d-%s]",nCount,schar);
			CBF_Tools::GetConsoleEnter("定时设置完成?(y/n)",1,schar);
			if (schar[0] == 'y' || schar[0] == 'Y')
			{
				break;
			}
			nCount++;
			continue;
		}
		tmer.Start();
		CBF_Tools::GetConsoleEnter("是否退出测试(y/n)",1,schar);
		if (schar[0] == 'y' || schar[0] == 'Y')
		{
			Stop();
		}
	}
	void Stop()
	{
		tmer.Stop();
	}
	
	void join()
	{
		tmer.Join();
	}
	static int OnTimer(unsigned int event,void *p)
	{
		CTestTimer *pp = (CTestTimer *)p;
		pp->tDate.Update();
		char sdate[10];
		char stime[9];
		bzero(sdate,sizeof(sdate));
		bzero(stime,sizeof(stime));
		pp->tDate.ToCharDate(sdate);
		pp->tDate.ToCharTime(stime);
		printf("%d %s %s \n",event,sdate,stime);
		return 0;
	}

};

#endif // !defined(AFX_TESTTIMER_H__CBDE24CC_EC03_406B_941B_1845E1272098__INCLUDED_)
