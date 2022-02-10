// TestQueue.h: interface for the CTestQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTQUEUE_H__BE43CFFB_9A7C_4F2B_A293_29F15479DE60__INCLUDED_)
#define AFX_TESTQUEUE_H__BE43CFFB_9A7C_4F2B_A293_29F15479DE60__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "BF_Queue.h"
#include "BF_Thread.h"

typedef struct 
{
	char data[10];
	int  index;
}TESTQUEUEDATA;

CBF_Queue<TESTQUEUEDATA,4> g_testqueue;
CBF_Stack<TESTQUEUEDATA> g_teststack;

#if defined(_WINDOWS) && _MSC_VER >1500
typedef std::list<TESTQUEUEDATA>::iterator DATA_ITER;
#else
typedef std::list<TESTQUEUEDATA>::iterator DATA_ITER;
#endif

bool         g_bIsExit;
unsigned int g_index;

class CTestQueueThread  : public CBF_Thread
{
public:
	CTestQueueThread()
	{
		m_nFlag =0;
	}
	virtual ~CTestQueueThread()
	{
		
	}
	// 函数名: InitThreadInstance
	// 编程  : 王明松 2012-4-9 12:00:54
	// 返回  : virtual bool 
	// 描述  : 线程初始化，线程启动时调用一次
	virtual bool InitThreadInstance()
	{
		return true;	
	}
	
	// 函数名: ExitThreadInstance
	// 编程  : 王明松 2012-4-9 12:01:05
	// 返回  : virtual void 
	// 描述  : 退出线程要做的工作,线程退出时调用一次
	virtual void ExitThreadInstance()
	{

	}
	
	// 函数名: Run
	// 编程  : 王明松 2012-4-9 12:01:23
	// 返回  : virtual int 
	// 描述  : 线程运行方法，若此方法返回，则线程自动终止
	virtual int Run()
	{
		while (!g_bIsExit)
		{
			TESTQUEUEDATA data;
			if (m_nFlag == 0)
			{
				data.index = g_index;
				g_index++;
				g_testqueue.PushData(data);
				SLEEP(1);
			}
			else
			{
				std::list<TESTQUEUEDATA>::iterator data;
//				printf("Begin GetData \n");
				int ret = g_testqueue.GetData(data);
				if (ret <0)
				{
					continue;
				}
//				printf("Begin DeleteData\n");
				g_testqueue.DeleteData(data,ret);
//				printf("DeleteData End\n");
				
			}
		}
		return 0;
	}
	int    m_nFlag;//0push  1get
	
};	


class CTestQueue  
{
public:
	CTestQueue()
	{
		g_bIsExit = false;
		g_index = 0;
	}
	virtual ~CTestQueue()
	{

	}
	void Test()
	{
		DATA_ITER dd ;
		if ((dd = g_testqueue.Begin()) != g_testqueue.End())
		{
			printf("数据个数%d\n",g_testqueue.GetDataCount());
		}
		TESTQUEUEDATA data;
		strcpy(data.data,"test");
		data.index = g_index;
		g_index++;
		g_testqueue.PushData(data);
		int ret = g_testqueue.GetData(dd);
		printf("数据个数%d\n",g_testqueue.GetDataCount());
		g_testqueue.DeleteData(dd,ret);
		printf("数据个数%d\n",g_testqueue.GetDataCount());
		
		aa[0].m_nFlag = 0;
		aa[0].m_sThreadName = "0";
		aa[1].m_nFlag = 0;
		aa[1].m_sThreadName = "1";
		aa[2].m_nFlag = 0;
		aa[2].m_sThreadName = "2";
		aa[3].m_nFlag = 1;
		aa[3].m_sThreadName = "3";
		aa[4].m_nFlag = 1;
		aa[4].m_sThreadName = "4";
		aa[5].m_nFlag = 1;
		aa[5].m_sThreadName = "5";
		g_bIsExit = false;
		aa[0].CreateThread();
		aa[1].CreateThread();
		aa[2].CreateThread();
		aa[3].CreateThread();
		aa[4].CreateThread();
		aa[5].CreateThread();
		while (true)
		{
			printf("数据个数%d\n",g_testqueue.GetDataCount());
			SLEEP(4000);
		}
	}
	CTestQueueThread aa[6];

	
};

#endif // !defined(AFX_TESTQUEUE_H__BE43CFFB_9A7C_4F2B_A293_29F15479DE60__INCLUDED_)
