// BF_Mutex.h: interface for the CBF_Mutex class.
//
//////////////////////////////////////////////////////////////////////

/***********************************************************************************
  编码日志记录
  编码者: 王明松
  编码日期 2012-4-17 9:23:09 重构
  版本: V1.00 
  说明: 互斥及事件
  CBF_MutexEvent使用说明:
   1：加入队列
	m_pEvent.Lock();
	//要做的事
	m_poolDataList.push_back(data);
	//要做的事完成
	m_pEvent.SetEvent();//设置事件通知
	m_pEvent.UnLock();
   2：等待通知，处理
	m_pEvent.Lock();
	if (m_poolDataList.size()>0)
	{
	     //要做的事
		data = m_poolDataList.front();
		m_poolDataList.pop_front();
		//要做的事完成
		m_pEvent.UnLock();
		return true;
	}
	//等待事件 
	m_pEvent.WaitEventTime(1000);

	if (m_poolDataList.size()>0)
	{
	    //要做的事
		data = m_poolDataList.front();
		m_poolDataList.pop_front();
		//要做的事完成
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
	// 函数名: TryLock
	// 编程  : 王明松 2012-4-10 16:59:16
	// 返回  : virtual int 1成功 -1错误 0不成功
	// 描述  : 尝试加锁，
	virtual int TryLock();

	// 函数名: Lock
	// 编程  : 王明松 2012-4-10 16:59:59
	// 返回  : virtual void 
	// 描述  : 加锁
	virtual bool Lock();

	// 函数名: UnLock
	// 编程  : 王明松 2012-4-10 17:00:04
	// 返回  : virtual void 
	// 描述  : 解锁
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
	
	bool   m_bIsLock;//是否成功加锁
private:
    CBF_Mutex *m_pmutex;

//	CBF_SimpleLog m_pLog;
};



class KERNEL_EXPORT CBF_MutexEvent 
{
public:
	
	/// 函数名: WaitEventTime
	/// 编程  : 王明松 2009-5-26 13:46:40
	/// 返回  : void 
	/// 参数  : unsigned int wait_ms
	/// 描述  : 等待事件通知
	void WaitEventTime(unsigned int wait_ms);
	
	/// 函数名: SetEvent
	/// 编程  : 王明松 2009-5-26 13:46:12
	/// 返回  : void 
	/// 描述  : 设置一个事件通知，注意多线程时要加锁
	void SetEvent();
	
	/// 函数名: WaitEvent
	/// 编程  : 王明松 2009-5-26 13:46:53
	/// 返回  : void 
	/// 描述  : 等待事件通知
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

	bool   m_bIsLock;//是否成功加锁
private:
	CBF_SpinMutex *m_pmutex;

	//	CBF_SimpleLog m_pLog;
};
#endif // !defined(AFX_BF_MUTEX_H__522B5203_1470_406D_8AFE_DE475FD78BCA__INCLUDED_)
