// BF_Mutex.h: interface for the CBF_Mutex class.
//
//////////////////////////////////////////////////////////////////////

/***********************************************************************************
  ������־��¼
  ������: ������
  �������� 2012-4-17 9:23:09 �ع�
  �汾: V1.00 
  ˵��: ���⼰�¼�
  CBF_MutexEventʹ��˵��:
   1���������
	m_pEvent.Lock();
	//Ҫ������
	m_poolDataList.push_back(data);
	//Ҫ���������
	m_pEvent.SetEvent();//�����¼�֪ͨ
	m_pEvent.UnLock();
   2���ȴ�֪ͨ������
	m_pEvent.Lock();
	if (m_poolDataList.size()>0)
	{
	     //Ҫ������
		data = m_poolDataList.front();
		m_poolDataList.pop_front();
		//Ҫ���������
		m_pEvent.UnLock();
		return true;
	}
	//�ȴ��¼� 
	m_pEvent.WaitEventTime(1000);

	if (m_poolDataList.size()>0)
	{
	    //Ҫ������
		data = m_poolDataList.front();
		m_poolDataList.pop_front();
		//Ҫ���������
		m_pEvent.UnLock();
		return true;
	}
	m_pEvent.UnLock();
 ***********************************************************************************/


#if !defined(AFX_BF_MUTEX_H__522B5203_1470_406D_8AFE_DE475FD78BCA__INCLUDED_)
#define AFX_BF_MUTEX_H__522B5203_1470_406D_8AFE_DE475FD78BCA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
//#include "BF_SimpleLog.h"



#ifdef KERNEL_EXPORTS
#define KERNEL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNEL_EXPORT __declspec(dllimport)
#else
#define KERNEL_EXPORT 
#endif
#endif


class KERNEL_EXPORT CBF_Mutex  
{
public:
	CBF_Mutex();
	virtual ~CBF_Mutex();
	// ������: TryLock
	// ���  : ������ 2012-4-10 16:59:16
	// ����  : virtual int 1�ɹ� -1���� 0���ɹ�
	// ����  : ���Լ�����
	virtual int TryLock();

	// ������: Lock
	// ���  : ������ 2012-4-10 16:59:59
	// ����  : virtual void 
	// ����  : ����
	virtual bool Lock();

	// ������: UnLock
	// ���  : ������ 2012-4-10 17:00:04
	// ����  : virtual void 
	// ����  : ����
	virtual void UnLock();

protected:
#if defined(_WINDOWS)
	HANDLE m_lock;
#else
	pthread_mutex_t m_lock;
#endif

//	CBF_SimpleLog m_pLog;
};



class KERNEL_EXPORT CBF_PMutex  
{
public:
	CBF_PMutex(CBF_Mutex *pmutex);
	virtual ~CBF_PMutex();
	
	bool   m_bIsLock;//�Ƿ�ɹ�����
private:
    CBF_Mutex *m_pmutex;

//	CBF_SimpleLog m_pLog;
};



class KERNEL_EXPORT CBF_MutexEvent 
{
public:
	
	/// ������: WaitEventTime
	/// ���  : ������ 2009-5-26 13:46:40
	/// ����  : void 
	/// ����  : unsigned int wait_ms
	/// ����  : �ȴ��¼�֪ͨ
	void WaitEventTime(unsigned int wait_ms);
	
	/// ������: SetEvent
	/// ���  : ������ 2009-5-26 13:46:12
	/// ����  : void 
	/// ����  : ����һ���¼�֪ͨ��ע����߳�ʱҪ����
	void SetEvent();
	
	/// ������: WaitEvent
	/// ���  : ������ 2009-5-26 13:46:53
	/// ����  : void 
	/// ����  : �ȴ��¼�֪ͨ
	void WaitEvent();
	
	virtual void Lock();
	virtual void UnLock();
    virtual int TryLock();
	
	
	CBF_MutexEvent();
	virtual ~CBF_MutexEvent();
	
private:
#ifdef  _WINDOWS
	HANDLE m_event;
	HANDLE m_lock;

#else
	pthread_cond_t m_event;
    pthread_mutex_t m_lock;
#endif
#ifdef LINUX
	sem_t m_sid;
#endif	
};


class  KERNEL_EXPORT CBF_SpinMutex 
{
public:	
	CBF_SpinMutex();
	virtual ~CBF_SpinMutex();	

	bool Lock();
	void UnLock();

private:
#ifdef  _WINDOWS
	HANDLE  m_SpinLock;
#else
	pthread_spinlock_t m_SpinLock;
#endif
};
class KERNEL_EXPORT CBF_PSpinMutex  
{
public:
	CBF_PSpinMutex(CBF_SpinMutex *pmutex);
	virtual ~CBF_PSpinMutex();

	bool   m_bIsLock;//�Ƿ�ɹ�����
private:
	CBF_SpinMutex *m_pmutex;

	//	CBF_SimpleLog m_pLog;
};
#endif // !defined(AFX_BF_MUTEX_H__522B5203_1470_406D_8AFE_DE475FD78BCA__INCLUDED_)
