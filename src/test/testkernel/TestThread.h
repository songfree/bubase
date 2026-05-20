// TestThread.h: interface for the CTestThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTTHREAD_H__617C4D84_26D2_4A38_B900_9841179567E4__INCLUDED_)
#define AFX_TESTTHREAD_H__617C4D84_26D2_4A38_B900_9841179567E4__INCLUDED_

#include "BF_Thread.h"
#include "BF_Tools.h"
#include "BF_Mutex.h"
#include "BF_Queue.h"
#include "MdbBase.h"
#include "BF_LogClient.h"
#include <functional>
#include <atomic>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
CBF_QueueNoLock<UINT64_,10000> g_pFeeQueue;
extern  CBF_LogClient pLog;


//测试无锁队列
typedef struct
{
    UINT64_ nPuGime;
    UINT64_ nGettime;
}S_QUEUE_DATA;

CBF_QueueNoLock<S_QUEUE_DATA, 10000>g_testdelayqueu;
std::atomic<UINT64_>g_totaldelay;	//延迟总和
std::atomic<int>g_popnum;	 //pop数
std::atomic<int>g_pusherrnum;	 //push失败数
std::atomic<int>g_pushsucessnum;   //push成功数
int g_pushTotalNum = 8000000;



CBF_Mutex g_mutex;
void g_TestMutex()
{
	CBF_PMutex pp(&g_mutex);
	SLEEP(10);
}
CBF_MutexEvent g_event;
void g_TestSetEvent()
{
	g_event.Lock();
	g_event.SetEvent();
	g_event.UnLock();
	
}
void g_TestWaitEvent()
{
	int stime = 1000;
	g_event.Lock();
	unsigned long aa = CBF_Date_Time::GetTickCount();
	g_event.WaitEventTime(stime);
	printf("WaitEventTime %d = %d \n",stime,CBF_Date_Time::GetTickCount()-aa);
	g_event.UnLock();
}

typedef struct
{
	 UINT64_  serial;
}S_TEST_QUEUE_DATA;



class CTestT : public CBF_Thread
{
public:
	CTestT()
	{
		m_bIsRunning = false;
		m_nTestType = 0;
	}
	virtual ~CTestT()
	{
		
	}
	bool m_bIsRunning;
	int  m_nTestType;
	CBF_Date_Time m_pDate;
	UINT64_  m_serial=0;
	UINT64_  m_getserial = 0;
	int     m_index=0;
private:
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
		return;
	}
	
	// 函数名: Run
	// 编程  : 王明松 2012-4-9 12:01:23
	// 返回  : virtual int 
	// 描述  : 线程运行方法，若此方法返回，则线程自动终止
	virtual int Run()
	{
		while (m_bIsRunning)
		{
			S_QUEUE_DATA data;
			switch (m_nTestType)
			{
				case 0: //多线程日期测试
					m_pDate.Update();
					SLEEP(1);
					break;
				case 1:
					printf("线程[%d] 加锁\n",GetThisThreadId());
					g_TestMutex();
					printf("线程[%d] 解锁\n",GetThisThreadId());
					SLEEP(1);
					break;
				case 2:
					g_TestSetEvent();
					SLEEP(5);
					break;
				case 3:
					g_TestWaitEvent();
					break;
				case 4:
					if (!g_pFeeQueue.Push(m_serial))
					{
#ifdef _WINDOWS
						printf("index %d  push %I64d error\n", m_index, m_serial);
#else
						printf("index %d  push %lld error\n", m_index, m_serial);
#endif
					}
					else
					{
						//printf("index %d  push %I64d sucess\n", m_index, m_serial);
						//g_TestQueueData.PushPut(m_serial);
						g_pushsucessnum++;
						
					}
					m_serial++;
					break;
				case 5:
					if (g_pFeeQueue.Pop(m_getserial))
					{
						//printf("index %d  get %I64d\n", m_index, m_getserial);
						//g_TestQueueData.PushGet(m_getserial);
						g_popnum++;
					}
					else
					{
						//printf("index %d  no data\n", m_index);
#ifdef _WINDOWS
                        SLEEP(1);
#else
                        usleep(1);
#endif
					}
                    break;
                case 6:
					if (g_pushsucessnum.load() >= g_pushTotalNum)
					{
						break;
					}
                    data.nPuGime = CBF_Date_Time::GetTickUS();
                    if (!g_testdelayqueu.Push(data))
                    {
                        g_pusherrnum++;
                    }
					else
					{
						g_pushsucessnum++;
					}
                    break;
                case 7:
                    if (g_testdelayqueu.Pop(data))
                        //if (g_testMutexEvent.GetData(data))
                    {
                        //printf("队列延时 %d us\n",CBF_Date_Time::GetTickUS()-data.nPuGime);
                        //CBF_PMutex pp(&mutex);
                        //g_res.emplace_back((int)(CBF_Date_Time::GetTickUS() - data.nPuGime));
                        g_totaldelay += (CBF_Date_Time::GetTickUS() - data.nPuGime);
                        g_popnum++;
                    }
                    break;
				default:
					break;
			}
		}
		return 0;
	}
	
};



class CTestThread  
{
public:
	CTestThread()
	{
		m_nTCount = 0;
	}
	virtual ~CTestThread()
	{
		int i;
		for (i=0;i<m_nTCount;i++)
		{
			if (m_pTest[i] != NULL)
			{
				m_pTest[i]->m_bIsRunning = false;
				m_pTest[i]->Join();
				delete m_pTest[i];
				m_pTest[i]= NULL;
			}
		}

	}
	
    void Testfreequeue()
    {
        g_totaldelay = 0;	//延迟总和
        g_popnum = 0;	 //pop数
        g_pusherrnum = 0;	 //push失败数
        g_pushsucessnum = 0;   //push成功数
        int i;
        char stnum[11];
		int putnum = 2;
        bzero(stnum, sizeof(stnum));
        CBF_Tools::GetConsoleEnter("测试无锁队列\n请输入生产者线程数", 5, stnum);
		putnum = atoi(stnum);
        CBF_Tools::GetConsoleEnter("请输入消费者线程数", 5, stnum);
        m_nTCount = atoi(stnum);

        CBF_Tools::GetConsoleEnter("请输入测试时间ms", 10, stnum);
        m_nTestTime = atoi(stnum);
        if (m_nTestTime < 1)
        {
            return;
        }
		
		//输入的线程取
        for (i = 0; i < m_nTCount; i++)
        {
            m_pTest[putnum+i] = new CTestT();
            m_pTest[putnum+i]->m_bIsRunning = true;
			m_pTest[putnum+i]->m_index= putnum+i;
			m_pTest[putnum+i]->m_serial = (m_pTest[putnum+i]->m_index+1)*10000000000000000;
			m_pTest[putnum+i]->m_nTestType = 5;
        }
		//固定2个线程放
        for (i = 0; i < putnum; i++)
        {
            m_pTest[i] = new CTestT();
            m_pTest[i]->m_bIsRunning = true;
            m_pTest[i]->m_index = i;
            m_pTest[i]->m_serial = (m_pTest[i]->m_index + 1) * 10000000000000000;
            m_pTest[i]->m_nTestType = 4;

        }
       

        CBF_Tools::GetConsoleEnter("是否退出(y/n)", 11, stnum);
        if (stnum[0] == 'y' || stnum[0] == 'Y')
        {
            for (i = 0; i < m_nTCount+ putnum; i++)
            {
                m_pTest[i]->m_bIsRunning = false;
            }
            for (i = 0; i < m_nTCount+ putnum; i++)
            {
                m_pTest[i]->Join();
            }
            for (i = 0; i < m_nTCount+ putnum; i++)
            {
                delete m_pTest[i];
                m_pTest[i] = NULL;
            }
        }
        else
        {
            for (i = 0; i < m_nTCount; i++)
            {
				if (!m_pTest[putnum+i]->CreateThread())
				{
					printf("启动线程失败\n");
					return ;
				}
            }
            for (i = 0; i < putnum; i++)
            {
                if (!m_pTest[i]->CreateThread())
                {
                    printf("启动线程失败\n");
                    return;
                }
            }
			int lasttime = CBF_Date_Time::GetTickCount();
            int curtime = CBF_Date_Time::GetTickCount();
            while (curtime - lasttime <= m_nTestTime)
            {
				//INT64_ nvalue;
				//g_pFeeQueue.dequeue(nvalue);
                SLEEP(100);
                curtime = CBF_Date_Time::GetTickCount();
            }
            for (i = 0; i < putnum; i++)
            {
                m_pTest[i]->m_bIsRunning = false;
            }
            for (i = 0; i < putnum; i++)
            {
                m_pTest[i]->Join();
            }
			SLEEP(1000);
            for (i = putnum; i < m_nTCount+putnum; i++)
            {
                m_pTest[i]->m_bIsRunning = false;
            }
            for (i = putnum; i < m_nTCount+putnum; i++)
            {
                m_pTest[i]->Join();
            }
            for (i = 0; i < m_nTCount+ putnum; i++)
            {
                delete m_pTest[i];
                m_pTest[i] = NULL;
            }
			//比较测试结果
			if (g_pushsucessnum != g_popnum )
			{
				printf("测试结果出错 put %I64d  get %I64d  队列还有数据[%d]\n", g_pushsucessnum.load(), g_popnum.load(), g_pFeeQueue.size());
			}
			else
			{
				if (g_pFeeQueue.size() > 0)
				{
					printf("测试结果出错 put %I64d  get %I64d  队列里还有数据 %d \n", g_pushsucessnum.load(), g_popnum.load(), g_pFeeQueue.size());
					return; 
				}
				printf("测试结果正确 put %I64d  get %I64d\n", g_pushsucessnum.load(), g_popnum.load());
			}

        }

    }
    void Testfreequeuedelay()
    {
        g_totaldelay = 0;	//延迟总和
        g_popnum = 0;	 //pop数
        g_pusherrnum = 0;	 //push失败数
        g_pushsucessnum = 0;   //push成功数
        int i;
        char stnum[11];
        int putnum = 2;
        bzero(stnum, sizeof(stnum));
        CBF_Tools::GetConsoleEnter("测试无锁队列\n请输入生产者线程数", 5, stnum);
        putnum = atoi(stnum);
        CBF_Tools::GetConsoleEnter("请输入消费者线程数", 5, stnum);
        int popnum = atoi(stnum);

        CBF_Tools::GetConsoleEnter("请输入入队列数目", 10, stnum);
        g_pushTotalNum  = atol(stnum);
        if (g_pushTotalNum < 1)
        {
            return;
        }

        //输入的pop线程
        for (i = 0; i < popnum; i++)
        {
            m_pTest[putnum + i] = new CTestT();
            m_pTest[putnum + i]->m_bIsRunning = true;
            m_pTest[putnum + i]->m_index = putnum + i;
            m_pTest[putnum + i]->m_serial = (m_pTest[putnum + i]->m_index + 1) * 10000000000000000;
            m_pTest[putnum + i]->m_nTestType = 7;
        }
        //push线程
        for (i = 0; i < putnum; i++)
        {
            m_pTest[i] = new CTestT();
            m_pTest[i]->m_bIsRunning = true;
            m_pTest[i]->m_index = i;
            m_pTest[i]->m_serial = (m_pTest[i]->m_index + 1) * 10000000000000000;
            m_pTest[i]->m_nTestType = 6;

        }
        CBF_Tools::GetConsoleEnter("是否退出(y/n)", 11, stnum);
        if (stnum[0] == 'y' || stnum[0] == 'Y')
        {
            for (i = 0; i < m_nTCount + putnum; i++)
            {
                m_pTest[i]->m_bIsRunning = false;
            }
            for (i = 0; i < m_nTCount + putnum; i++)
            {
                m_pTest[i]->Join();
            }
            for (i = 0; i < m_nTCount + putnum; i++)
            {
                delete m_pTest[i];
                m_pTest[i] = NULL;
            }
        }
        else
        {
            for (i = 0; i < popnum; i++)
            {
                if (!m_pTest[putnum + i]->CreateThread())
                {
                    printf("启动线程失败\n");
                    return;
                }
            }
			UINT64_ btime = CBF_Date_Time::GetTickCount();
            for (i = 0; i < putnum; i++)
            {
                if (!m_pTest[i]->CreateThread())
                {
                    printf("启动线程失败\n");
                    return;
                }
            }
            while (g_pushsucessnum < g_pushTotalNum)
            {
                //INT64_ nvalue;
                //g_pFeeQueue.dequeue(nvalue);
                SLEEP(1);
                
            }
			UINT64_ etime = CBF_Date_Time::GetTickCount();
            for (i = 0; i < putnum; i++)
            {
                m_pTest[i]->m_bIsRunning = false;
            }
            for (i = 0; i < putnum; i++)
            {
                m_pTest[i]->Join();
            }
            SLEEP(1000);
            for (i = putnum; i < popnum + putnum; i++)
            {
                m_pTest[i]->m_bIsRunning = false;
            }
            for (i = putnum; i < popnum + putnum; i++)
            {
                m_pTest[i]->Join();
            }
            for (i = 0; i < popnum + putnum; i++)
            {
                delete m_pTest[i];
                m_pTest[i] = NULL;
            }
            //统计结果
		    printf("push线程[%d] pop线程[%d] 总耗时[%d]ms 总push数据[%ld]条  push成功[%d] push失败[%d]条 pop数据[%d]条 平均延时[%f]us  总延时[%ld]\n",\
				putnum,popnum,etime-btime,g_pushTotalNum,g_pushsucessnum.load(),g_pusherrnum.load(), g_popnum.load(), 1.00*g_totaldelay.load()/g_pushsucessnum.load(), g_totaldelay.load());
        }

    }
	void TestMutex()
	{
		int i;
		char stnum[11];
		bzero(stnum,sizeof(stnum));
		CBF_Tools::GetConsoleEnter("测试多个线程互斥\n请输入线程数",5,stnum);
		m_nTCount = atoi(stnum);
		CBF_Tools::GetConsoleEnter("请输入测试时间ms",10,stnum);
		m_nTestTime = atoi(stnum);
		if (m_nTestTime <1)
		{
			return;
		}
		for (i=0;i<m_nTCount;i++)
		{
			m_pTest[i] = new CTestT();
			m_pTest[i]->m_bIsRunning = true;
			m_pTest[i]->m_nTestType = 1;
			m_pTest[i]->CreateThread();
		}
		int lasttime =CBF_Date_Time::GetTickCount();

		CBF_Tools::GetConsoleEnter("是否退出(y/n)",11,stnum);
		if (stnum[0] == 'y' || stnum[0] == 'Y')
		{
			for (i=0;i<m_nTCount;i++)
			{
				m_pTest[i]->m_bIsRunning = false;
			}
			for (i=0;i<m_nTCount;i++)
			{
				m_pTest[i]->Join();
			}
			for (i=0;i<m_nTCount;i++)
			{
				delete m_pTest[i];
				m_pTest[i]= NULL;
			}
		}
		else
		{
			int curtime =  CBF_Date_Time::GetTickCount();
			while (curtime - lasttime <= m_nTestTime)
			{
				SLEEP(100);
				curtime =  CBF_Date_Time::GetTickCount();
			}
			for (i=0;i<m_nTCount;i++)
			{
				m_pTest[i]->m_bIsRunning = false;
			}
			for (i=0;i<m_nTCount;i++)
			{
				m_pTest[i]->Join();
			}
			for (i=0;i<m_nTCount;i++)
			{
				delete m_pTest[i];
				m_pTest[i]= NULL;
			}
		}

	}
	void TestEvent()
	{
// 		CBF_MutexEvent ss;
// 		char stime[10];
// 		bzero(stime,sizeof(stime));
// 		CBF_Tools::GetConsoleEnter("测试WaitEventTime,请输入超时时间(ms)",5,stime);
// 		ss.Lock();
// 		unsigned long aa = CBF_Date_Time::GetTickCount();
// 		ss.WaitEventTime(atoi(stime));
// 		printf("WaitEventTime %s = %d \n",stime,CBF_Date_Time::GetTickCount()-aa);
// 		ss.UnLock();
		int i;
		char stnum[11];
		bzero(stnum,sizeof(stnum));
		CBF_Tools::GetConsoleEnter("测试多个线程发送事件，多个线程接收事件通知\n请输入线程数",5,stnum);
		m_nTCount = atoi(stnum);
		CBF_Tools::GetConsoleEnter("请输入测试时间ms",10,stnum);
		m_nTestTime = atoi(stnum);
		if (m_nTestTime <1)
		{
			return;
		}
		int nsetevent = m_nTCount/2;
		for (i=0;i<nsetevent;i++)
		{
			m_pTest[i] = new CTestT();
			m_pTest[i]->m_bIsRunning = true;
			m_pTest[i]->m_nTestType = 2;
			m_pTest[i]->CreateThread();
		}
		for (i=nsetevent;i<m_nTCount;i++)
		{
			m_pTest[i] = new CTestT();
			m_pTest[i]->m_bIsRunning = true;
			m_pTest[i]->m_nTestType = 3;
			m_pTest[i]->CreateThread();
		}
		m_pTimer.Init(10);
		m_pTimer.SetTimer(1,m_nTestTime,&timeback,this,0);
		m_pTimer.Start();
		CBF_Tools::GetConsoleEnter("是否退出(y/n)",11,stnum);
		if (stnum[0] == 'y' || stnum[0] == 'Y')
		{
			for (i=0;i<m_nTCount;i++)
			{
				m_pTest[i]->m_bIsRunning = false;
			}
			for (i=0;i<m_nTCount;i++)
			{
				m_pTest[i]->Join();
			}
			for (i=0;i<m_nTCount;i++)
			{
				delete m_pTest[i];
				m_pTest[i]= NULL;
			}
		}

	}
	CTestT *m_pTest[10000];
	int m_nTCount;
	int m_nTestTime;
	CBF_Timer m_pTimer;
	void TestThreadDate()
	{
		int i;
		char stnum[11];
		bzero(stnum,sizeof(stnum));
		CBF_Tools::GetConsoleEnter("测试多线程取本地日期时间\n请输入线程数",5,stnum);
		m_nTCount = atoi(stnum);
		CBF_Tools::GetConsoleEnter("请输入测试时间ms",10,stnum);
		m_nTestTime = atoi(stnum);
		if (m_nTestTime <1)
		{
			return;
		}
		for (i=0;i<m_nTCount;i++)
		{
			m_pTest[i] = new CTestT();
			m_pTest[i]->m_bIsRunning = true;
			m_pTest[i]->m_nTestType = 0;
			m_pTest[i]->CreateThread();
		}
		m_pTimer.Init(10);
		m_pTimer.SetTimer(1,m_nTestTime,&timeback,this,0);
		m_pTimer.Start();
		int lasttime = CBF_Date_Time::GetTickCount();
		CBF_Tools::GetConsoleEnter("是否退出(y/n)",11,stnum);
		if (stnum[0] == 'y' || stnum[0] == 'Y')
		{
			for (i=0;i<m_nTCount;i++)
			{
				m_pTest[i]->m_bIsRunning = false;
			}
			for (i=0;i<m_nTCount;i++)
			{
				m_pTest[i]->Join();
			}
			for (i=0;i<m_nTCount;i++)
			{
				delete m_pTest[i];
				m_pTest[i]= NULL;
			}
		}
	
		

	}
	static int timeback(unsigned int id,void *p)
	{
		CTestThread *aa = (CTestThread *)p;
		for (int i=0;i<aa->m_nTCount;i++)
		{
			aa->m_pTest[i]->m_bIsRunning = false;
		}
		printf("定时回调完成,终止线程\n");
		return 0;
	}
	void Test()
	{
		char stnum[11];
		bzero(stnum,sizeof(stnum));
		CBF_Tools::GetConsoleEnter("请选择测试种类:\n1测试多线程互斥\n2测试多线程取本地日期时间\n3测试多线程消息事件\n请选择",11,stnum);
		switch (atoi(stnum))
		{
			case  1:
				TestMutex();
				break;
			case 2:
				TestThreadDate();
				break;
			case 3:
				TestEvent();
				break;
			default:
				break;
		}
	}
};

#endif // !defined(AFX_TESTTHREAD_H__617C4D84_26D2_4A38_B900_9841179567E4__INCLUDED_)
