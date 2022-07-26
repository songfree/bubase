// BF_Queue.h: interface for the CBF_Queue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BF_QUEUE_H__2F2470E8_F0D3_4861_9482_D5EC196EEC7B__INCLUDED_)
#define AFX_BF_QUEUE_H__2F2470E8_F0D3_4861_9482_D5EC196EEC7B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000	

#include <functional>
#include <atomic>
#include "BF_Mutex.h"

#ifdef _WINDOWS
#define CAS(a_ptr, a_oldVal, a_newVal) InterlockedCompareExchange(a_ptr, a_newVal , a_oldVal)
#else
#define CAS(a_ptr, a_oldVal, a_newVal) __sync_bool_compare_and_swap(a_ptr, a_oldVal, a_newVal)

#endif


template <typename ELEM_T, UINT64_ Q_SIZE>
class CBF_QueueNoLock
{
public:

	CBF_QueueNoLock()
	{
		m_writeIndex=0;
		m_readIndex=0;
		m_maximumReadIndex=0;
		m_count=0;
	}
    virtual ~CBF_QueueNoLock()
	{
	}

	UINT64_ size()
	{
		return m_count;
	}
	//放入数据至无锁队列
    bool Push(const ELEM_T& a_data)
	{
		UINT64_ currentWriteIndex;        // 获取写指针的位置
		UINT64_ currentReadIndex;
        int curtms = CBF_Date_Time::GetTickCount();
        // 1. 获取可写入的位置
		bool bret=false;
        while (!bret)
        {
            currentWriteIndex = m_writeIndex;
            currentReadIndex = m_readIndex;
            if (countToIndex(currentWriteIndex + 1) == countToIndex(currentReadIndex))
            {
                //队列满
                return false;
            }
			UINT64_ res= CAS(&m_writeIndex, currentWriteIndex, (currentWriteIndex + 1));
#ifdef _WINDOWS
            if (res == currentWriteIndex)
            {
				break;
			}
#else
            if (res == 1)
			{
				break;
			}
#endif
            // 目的是为了获取一个能写入的位置
        } 
        // 获取写入位置后 currentWriteIndex 是一个临时变量，保存我们写入的位置
        m_thequeue[countToIndex(currentWriteIndex)] = a_data;  // 把数据更新到对应的位置

        // 2. 更新可读的位置，按着m_maximumReadIndex+1的操作
 		bret = false;
        while (!bret)
        {
			UINT64_ res = CAS(&m_maximumReadIndex, currentWriteIndex, (currentWriteIndex + 1));
#ifdef _WINDOWS
            if (res == currentWriteIndex)
            {
				break;
            }
#else
            if (res == 1)
            {
				break;
            }
#endif
			//走到下面是因为后面的在更新，所以要切换线程，等待做完再做
#ifdef _WINDOWS
			Sleep(0);//切换到同优先级的线程或高优先级的线程
#else
            sched_yield();      //先在当前线程所在的核心上遍历合适的任务线程，如果有的话，就切换到该线程 
#endif
        }

		m_count++;
        return true;
	}
	//从无锁队列取出一条数据
    bool Pop(ELEM_T& a_data)
	{
		UINT64_ currentMaximumReadIndex;
		UINT64_ currentReadIndex;
        while (true)
        {
            currentReadIndex = m_readIndex;
            currentMaximumReadIndex = m_maximumReadIndex;

            if (countToIndex(currentReadIndex) ==  countToIndex(currentMaximumReadIndex))      // 如果不为空，获取到读索引的位置
            {
                return false;
            }
            // retrieve the data from the queue
            a_data = m_thequeue[countToIndex(currentReadIndex)]; // 从临时位置读取的
			UINT64_ res = CAS(&m_readIndex, currentReadIndex, (currentReadIndex + 1));
#ifdef _WINDOWS
            if (res == currentReadIndex)
            {
                m_count--;
                return true;
            }
#else
            if (res == 1)
            {
                m_count--;
                return true;
            }
#endif
			//说明这个数据已经被别人读走了，重新开始
        } 
        return false;
	}
private:

    ELEM_T m_thequeue[Q_SIZE];	 //数组，存放数据
	std::atomic<UINT64_> m_count;
    volatile UINT64_ m_writeIndex;   //写的数据计数

    volatile UINT64_ m_readIndex;	   //读的数据计数

	volatile UINT64_ m_maximumReadIndex;	  //最大的数据记数，即队尾

	//根据计数，获取对应的数组下标
    inline UINT64_ countToIndex(UINT64_ a_count)  //
	{
		return a_count % Q_SIZE;
	}
};

template<typename Record>
class CBF_Stack
{
public:
    CBF_Stack()
    {

    }
    virtual ~CBF_Stack()
    {

    }

    // 函数名: PushData
    // 编程  : 王明松 2012-5-14 10:41:36
    // 返回  : void 
    // 参数  : Record data  数据
    // 参数  : bool insert_tail=true  true-放入尾部 false-放入首部
    // 描述  : 放入数据至队列 
    void PushData(Record data, bool insert_tail = true)
    {
        //CBF_PMutex pp(&m_mutex);//主要是有可能多个线程同时写入
        m_pEvent.Lock();
        if (insert_tail)
        {
            m_qLogList.push_back(data);
        }
        else
        {
            m_qLogList.push_front(data);
        }
        m_pEvent.SetEvent();
        m_pEvent.UnLock();
    }

    // 函数名: GetData
    // 编程  : 王明松 2012-5-14 10:43:01
    // 返回  : bool  true成功取到数据 false没有数据
    // 参数  : Record &data   数据
    // 参数  : unsigned int waitetms=1000  超时等待时间ms
    // 参数  : bool get_front=true  true-从首部取 false-从尾部取
    // 描述  : 从队列取出数据并在队列删除
    bool GetData(Record& data, unsigned int waitetms = 1000, bool get_front = true)
    {
        m_pEvent.Lock();
        if (m_qLogList.size() > 0)
        {
            if (get_front)
            {
                data = m_qLogList.front();
                m_qLogList.pop_front();
            }
            else
            {
                data = m_qLogList.back();
                m_qLogList.pop_back();
            }
            m_pEvent.UnLock();
            return true;
        }
#ifdef _WINDOWS
        if (waitetms < 10) //windows不能小于10毫秒
        {
            m_pEvent.WaitEventTime(10);
        }
        else
        {
            m_pEvent.WaitEventTime(waitetms);
        }
#else
        m_pEvent.WaitEventTime(waitetms);
#endif
        if (m_qLogList.size() > 0)
        {
            if (get_front)
            {
                data = m_qLogList.front();
                m_qLogList.pop_front();
            }
            else
            {
                data = m_qLogList.back();
                m_qLogList.pop_back();
            }
            m_pEvent.UnLock();
            return true;
        }
        m_pEvent.UnLock();
        return false;
    }


    // 函数名: pop_back
    // 编程  : 王明松 2012-5-16 9:01:10
    // 返回  : Record 
    // 描述  : 不是线程安全，析构时使用
    Record pop_back()
    {
        Record data = m_qLogList.back();
        m_qLogList.pop_back();
        return data;
    }
    // 函数名: pop_front
    // 编程  : 王明松 2012-5-16 9:01:10
    // 返回  : Record 
    // 描述  : 不是线程安全，析构时使用
    Record pop_front()
    {
        Record data = m_qLogList.front();
        m_qLogList.pop_front();
        return data;
    }
    int GetDataCount()
    {
        CBF_PMutex pp(&m_mutex);//主要是有可能多个线程同时写入
        return m_qLogList.size();
    }
private:
    typedef typename std::deque<Record> T_LOGQUEUE;//队列
    T_LOGQUEUE  m_qLogList;   //队列
    CBF_Mutex   m_mutex;      //push队列互斥量
    CBF_MutexEvent m_pEvent;  //取数据消息事件
};


template<typename Record, int PRIO>
class CBF_Queue
{
public:
    typedef typename  std::list<Record>     PRIOQUEUE;

    typedef typename  std::list<Record>::iterator   BF_QUEUE_ITER;

    typedef struct
    {
        PRIOQUEUE       datas;   //队列
        CBF_Mutex       m_mutex; //队列互斥量
//		CBF_MutexEvent  m_pEvent;//数据消息事件
    } RQUEUEDATA;

    CBF_Queue()
    {
        m_nDataCount = 0;
    }
    virtual ~CBF_Queue()
    {

    }

    // 函数名: Begin
    // 编程  : 王明松 2012-5-16 9:03:04
    // 返回  : BF_QUEUE_ITER 
    // 参数  : int prio=0
    // 描述  : 不是线程安全，析构时使用
    BF_QUEUE_ITER Begin(int prio = 0)
    {
        return m_qQueue[prio].datas.begin();
    }

    // 函数名: End
    // 编程  : 王明松 2012-5-16 9:02:52
    // 返回  : BF_QUEUE_ITER 
    // 参数  : int prio=0
    // 描述  : 不是线程安全，析构时使用
    BF_QUEUE_ITER End(int prio = 0)
    {
        return m_qQueue[prio].datas.end();
    }

    // 函数名: PushData
    // 编程  : 王明松 2012-5-16 9:03:20
    // 返回  : void 
    // 参数  : Record data
    // 参数  : int prio=0
    // 描述  : 放入数据，线程安全
    void PushData(Record data, int prio = 0)
    {
        if (prio > PRIO)
        {
            return;
        }
        CBF_PMutex pp(&m_qQueue[prio].m_mutex);//主要是有可能多个线程同时写入
        m_pEvent.Lock();
        //		m_qQueue[prio].m_pEvent.Lock();
        m_qQueue[prio].datas.push_back(data);
        m_pEvent.SetEvent();
        m_nDataCount++;
        //		m_qQueue[prio].m_pEvent.SetEvent();
        //		m_qQueue[prio].m_pEvent.UnLock();
        m_pEvent.UnLock();
    }

    // 函数名: DeleteData
    // 编程  : 王明松 2012-5-16 9:03:38
    // 返回  : void 
    // 参数  : void * data
    // 参数  : int prio=0
    // 描述  : 删除数据，线程安全，注意不要删错了，要BF_QUEUE_ITER d1=data,然后data++，再删除d1，
    void DeleteData(BF_QUEUE_ITER data, int prio = 0)
    {
        if (prio > PRIO)
        {
            return;
        }

        CBF_PMutex pp(&m_qQueue[prio].m_mutex);//主要是有可能多个线程同时写入
        m_qQueue[prio].datas.erase(data);
        m_nDataCount--;
    }

    // 函数名: GetData
    // 编程  : 王明松 2012-5-16 8:50:40
    // 返回  : int  -1没有数据   >=0为队列ID
    // 参数  : BF_QUEUE_ITER &data
    // 参数  : unsigned int waitetms=1000
    // 描述  : 取得队列中的一条数据,有多个队列就取优先级高的，处理完后可调用DeleteData从队列删除，线程安全
    int GetData(BF_QUEUE_ITER& data, unsigned int waitetms = 1000)
    {
        int i;
        m_pEvent.Lock();
        for (i = 0; i < PRIO; i++)
        {
            //			printf("GetData prio=%d\n",i);
            if (GetDataPrio(data, i) == 0)
            {
                return i;
            }
        }
        m_pEvent.WaitEventTime(waitetms);
        for (i = 0; i < PRIO; i++)
        {
            if (GetDataPrio(data, i) == 0)
            {
                return i;
            }
        }
        m_pEvent.UnLock();
        return -1;
    }
    int GetDataCount()
    {
        return m_nDataCount;
    }
public:
    RQUEUEDATA      m_qQueue[PRIO];

private:
    int GetDataPrio(BF_QUEUE_ITER& data, int prio)
    {
        //		CBF_PMutex pp(&m_qQueue[prio].m_mutex);//主要是有可能多个线程同时写入
        //		printf("prio=%d\n",prio);
        if ((data = Begin(prio)) != End(prio))
        {
            //			printf("have data prio=%d\n",prio);
            m_pEvent.UnLock();
            return 0;
        }
        //		printf("no data prio=%d\n",prio);
        return -1;
    }
    unsigned   int  m_nDataCount;//数据个数
    CBF_MutexEvent  m_pEvent;//数据消息事件

};




template<typename Record, int PRIO>
class CBF_Deque
{
public:
    typedef typename  std::deque<Record>     PRIODEQUE;
    typedef struct
    {
        PRIODEQUE       datas;   //队列
        CBF_Mutex       m_mutex; //队列互斥量
    } RDEQUEDATA;

    CBF_Deque()
    {

    }
    virtual ~CBF_Deque()
    {

    }



    // 函数名: PushData
    // 编程  : 王明松 2012-5-16 9:03:20
    // 返回  : int 
    // 参数  : Record data
    // 参数  : int prio=0
    // 描述  : 放入数据，线程安全
    int PushData(Record data, int prio = 0)
    {
        if (prio > PRIO || prio < 1)
        {
            return -1;
        }
        CBF_PMutex pp(&m_qDeque[prio].m_mutex);//主要是有可能多个线程同时写入
        m_pEvent.Lock();
        m_qDeque[prio].datas.push_back(data);
        m_pEvent.SetEvent();
        m_pEvent.UnLock();
        return 0;
    }



    // 函数名: GetData
    // 编程  : 王明松 2012-5-16 8:50:40
    // 返回  : int   <0无数据  >=0优先级  
    // 参数  : Record &data
    // 参数  : unsigned int waitetms=1000
    // 描述  : 取得队列中的一条数据
    int GetData(Record& data, unsigned int waitetms = 1000)
    {
        int i;
        m_pEvent.Lock();
        for (i = 0; i < PRIO; i++)
        {
            if (m_qDeque[i].datas.size() > 0)
            {
                data = m_qDeque[i].datas.front();
                m_qDeque[i].datas.pop_front();
                m_pEvent.UnLock();
                return i;
            }
        }
        m_pEvent.WaitEventTime(waitetms);
        for (i = 0; i < PRIO; i++)
        {
            if (m_qDeque[i].datas.size() > 0)
            {
                data = m_qDeque[i].datas.front();
                m_qDeque[i].datas.pop_front();
                m_pEvent.UnLock();
                return i;
            }
        }
        m_pEvent.UnLock();
        return -1;
    }
    int GetDataCount()
    {
        CBF_PMutex pp(&m_mutex);
        int datanum = 0;
        for (int i = 0; i < PRIO; i++)
        {
            datanum += m_qDeque[i].size();
        }
        return datanum;
    }
public:
    RDEQUEDATA      m_qDeque[PRIO];

private:

    CBF_MutexEvent  m_pEvent;//数据消息事件
    CBF_Mutex m_mutex;

};


#endif // !defined(AFX_BF_QUEUE_H__2F2470E8_F0D3_4861_9482_D5EC196EEC7B__INCLUDED_)
