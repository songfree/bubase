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
	//������������������
    bool Push(const ELEM_T& a_data)
	{
		UINT64_ currentWriteIndex;        // ��ȡдָ���λ��
		UINT64_ currentReadIndex;
        int curtms = CBF_Date_Time::GetTickCount();
        // 1. ��ȡ��д���λ��
		bool bret=false;
        while (!bret)
        {
            currentWriteIndex = m_writeIndex;
            currentReadIndex = m_readIndex;
            if (countToIndex(currentWriteIndex + 1) == countToIndex(currentReadIndex))
            {
                //������
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
            // Ŀ����Ϊ�˻�ȡһ����д���λ��
        } 
        // ��ȡд��λ�ú� currentWriteIndex ��һ����ʱ��������������д���λ��
        m_thequeue[countToIndex(currentWriteIndex)] = a_data;  // �����ݸ��µ���Ӧ��λ��

        // 2. ���¿ɶ���λ�ã�����m_maximumReadIndex+1�Ĳ���
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
			//�ߵ���������Ϊ������ڸ��£�����Ҫ�л��̣߳��ȴ���������
#ifdef _WINDOWS
			Sleep(0);//�л���ͬ���ȼ����̻߳�����ȼ����߳�
#else
            sched_yield();      //���ڵ�ǰ�߳����ڵĺ����ϱ������ʵ������̣߳�����еĻ������л������߳� 
#endif
        }

		m_count++;
        return true;
	}
	//����������ȡ��һ������
    bool Pop(ELEM_T& a_data)
	{
		UINT64_ currentMaximumReadIndex;
		UINT64_ currentReadIndex;
        while (true)
        {
            currentReadIndex = m_readIndex;
            currentMaximumReadIndex = m_maximumReadIndex;

            if (countToIndex(currentReadIndex) ==  countToIndex(currentMaximumReadIndex))      // �����Ϊ�գ���ȡ����������λ��
            {
                return false;
            }
            // retrieve the data from the queue
            a_data = m_thequeue[countToIndex(currentReadIndex)]; // ����ʱλ�ö�ȡ��
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
			//˵����������Ѿ������˶����ˣ����¿�ʼ
        } 
        return false;
	}
private:

    ELEM_T m_thequeue[Q_SIZE];	 //���飬�������
	std::atomic<UINT64_> m_count;
    volatile UINT64_ m_writeIndex;   //д�����ݼ���

    volatile UINT64_ m_readIndex;	   //�������ݼ���

	volatile UINT64_ m_maximumReadIndex;	  //�������ݼ���������β

	//���ݼ�������ȡ��Ӧ�������±�
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

    // ������: PushData
    // ���  : ������ 2012-5-14 10:41:36
    // ����  : void 
    // ����  : Record data  ����
    // ����  : bool insert_tail=true  true-����β�� false-�����ײ�
    // ����  : �������������� 
    void PushData(Record data, bool insert_tail = true)
    {
        //CBF_PMutex pp(&m_mutex);//��Ҫ���п��ܶ���߳�ͬʱд��
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

    // ������: GetData
    // ���  : ������ 2012-5-14 10:43:01
    // ����  : bool  true�ɹ�ȡ������ falseû������
    // ����  : Record &data   ����
    // ����  : unsigned int waitetms=1000  ��ʱ�ȴ�ʱ��ms
    // ����  : bool get_front=true  true-���ײ�ȡ false-��β��ȡ
    // ����  : �Ӷ���ȡ�����ݲ��ڶ���ɾ��
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
        if (waitetms < 10) //windows����С��10����
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


    // ������: pop_back
    // ���  : ������ 2012-5-16 9:01:10
    // ����  : Record 
    // ����  : �����̰߳�ȫ������ʱʹ��
    Record pop_back()
    {
        Record data = m_qLogList.back();
        m_qLogList.pop_back();
        return data;
    }
    // ������: pop_front
    // ���  : ������ 2012-5-16 9:01:10
    // ����  : Record 
    // ����  : �����̰߳�ȫ������ʱʹ��
    Record pop_front()
    {
        Record data = m_qLogList.front();
        m_qLogList.pop_front();
        return data;
    }
    int GetDataCount()
    {
        CBF_PMutex pp(&m_mutex);//��Ҫ���п��ܶ���߳�ͬʱд��
        return m_qLogList.size();
    }
private:
    typedef typename std::deque<Record> T_LOGQUEUE;//����
    T_LOGQUEUE  m_qLogList;   //����
    CBF_Mutex   m_mutex;      //push���л�����
    CBF_MutexEvent m_pEvent;  //ȡ������Ϣ�¼�
};


template<typename Record, int PRIO>
class CBF_Queue
{
public:
    typedef typename  std::list<Record>     PRIOQUEUE;

    typedef typename  std::list<Record>::iterator   BF_QUEUE_ITER;

    typedef struct
    {
        PRIOQUEUE       datas;   //����
        CBF_Mutex       m_mutex; //���л�����
//		CBF_MutexEvent  m_pEvent;//������Ϣ�¼�
    } RQUEUEDATA;

    CBF_Queue()
    {
        m_nDataCount = 0;
    }
    virtual ~CBF_Queue()
    {

    }

    // ������: Begin
    // ���  : ������ 2012-5-16 9:03:04
    // ����  : BF_QUEUE_ITER 
    // ����  : int prio=0
    // ����  : �����̰߳�ȫ������ʱʹ��
    BF_QUEUE_ITER Begin(int prio = 0)
    {
        return m_qQueue[prio].datas.begin();
    }

    // ������: End
    // ���  : ������ 2012-5-16 9:02:52
    // ����  : BF_QUEUE_ITER 
    // ����  : int prio=0
    // ����  : �����̰߳�ȫ������ʱʹ��
    BF_QUEUE_ITER End(int prio = 0)
    {
        return m_qQueue[prio].datas.end();
    }

    // ������: PushData
    // ���  : ������ 2012-5-16 9:03:20
    // ����  : void 
    // ����  : Record data
    // ����  : int prio=0
    // ����  : �������ݣ��̰߳�ȫ
    void PushData(Record data, int prio = 0)
    {
        if (prio > PRIO)
        {
            return;
        }
        CBF_PMutex pp(&m_qQueue[prio].m_mutex);//��Ҫ���п��ܶ���߳�ͬʱд��
        m_pEvent.Lock();
        //		m_qQueue[prio].m_pEvent.Lock();
        m_qQueue[prio].datas.push_back(data);
        m_pEvent.SetEvent();
        m_nDataCount++;
        //		m_qQueue[prio].m_pEvent.SetEvent();
        //		m_qQueue[prio].m_pEvent.UnLock();
        m_pEvent.UnLock();
    }

    // ������: DeleteData
    // ���  : ������ 2012-5-16 9:03:38
    // ����  : void 
    // ����  : void * data
    // ����  : int prio=0
    // ����  : ɾ�����ݣ��̰߳�ȫ��ע�ⲻҪɾ���ˣ�ҪBF_QUEUE_ITER d1=data,Ȼ��data++����ɾ��d1��
    void DeleteData(BF_QUEUE_ITER data, int prio = 0)
    {
        if (prio > PRIO)
        {
            return;
        }

        CBF_PMutex pp(&m_qQueue[prio].m_mutex);//��Ҫ���п��ܶ���߳�ͬʱд��
        m_qQueue[prio].datas.erase(data);
        m_nDataCount--;
    }

    // ������: GetData
    // ���  : ������ 2012-5-16 8:50:40
    // ����  : int  -1û������   >=0Ϊ����ID
    // ����  : BF_QUEUE_ITER &data
    // ����  : unsigned int waitetms=1000
    // ����  : ȡ�ö����е�һ������,�ж�����о�ȡ���ȼ��ߵģ��������ɵ���DeleteData�Ӷ���ɾ�����̰߳�ȫ
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
        //		CBF_PMutex pp(&m_qQueue[prio].m_mutex);//��Ҫ���п��ܶ���߳�ͬʱд��
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
    unsigned   int  m_nDataCount;//���ݸ���
    CBF_MutexEvent  m_pEvent;//������Ϣ�¼�

};




template<typename Record, int PRIO>
class CBF_Deque
{
public:
    typedef typename  std::deque<Record>     PRIODEQUE;
    typedef struct
    {
        PRIODEQUE       datas;   //����
        CBF_Mutex       m_mutex; //���л�����
    } RDEQUEDATA;

    CBF_Deque()
    {

    }
    virtual ~CBF_Deque()
    {

    }



    // ������: PushData
    // ���  : ������ 2012-5-16 9:03:20
    // ����  : int 
    // ����  : Record data
    // ����  : int prio=0
    // ����  : �������ݣ��̰߳�ȫ
    int PushData(Record data, int prio = 0)
    {
        if (prio > PRIO || prio < 1)
        {
            return -1;
        }
        CBF_PMutex pp(&m_qDeque[prio].m_mutex);//��Ҫ���п��ܶ���߳�ͬʱд��
        m_pEvent.Lock();
        m_qDeque[prio].datas.push_back(data);
        m_pEvent.SetEvent();
        m_pEvent.UnLock();
        return 0;
    }



    // ������: GetData
    // ���  : ������ 2012-5-16 8:50:40
    // ����  : int   <0������  >=0���ȼ�  
    // ����  : Record &data
    // ����  : unsigned int waitetms=1000
    // ����  : ȡ�ö����е�һ������
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

    CBF_MutexEvent  m_pEvent;//������Ϣ�¼�
    CBF_Mutex m_mutex;

};


#endif // !defined(AFX_BF_QUEUE_H__2F2470E8_F0D3_4861_9482_D5EC196EEC7B__INCLUDED_)
