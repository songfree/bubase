// BF_Mutex.cpp: implementation of the CBF_Mutex class.
//
//////////////////////////////////////////////////////////////////////

#include "BF_Mutex.h"
#include "BF_Date_Time.h"
#include "BF_SimpleLog.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBF_Mutex::CBF_Mutex()
{
#ifdef  _WINDOWS
	m_lock=CreateMutex(NULL, FALSE, NULL);
#else
	pthread_mutexattr_t mattr;
	if (pthread_mutexattr_init(&mattr) !=0 )
	{
		fprintf(stderr,"%s %d pthread_mutexattr_init error",__FILE__,__LINE__);
	}
	//PTHREAD_MUTEX_NORMAL  PTHREAD_MUTEX_ERRORCHECK PTHREAD_MUTEX_RECURSIVE 三种属性
	pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_RECURSIVE );
	if (pthread_mutex_init(&m_lock, &mattr) != 0)
	{
		fprintf(stderr,"%s %d pthread_mutex_init error",__FILE__,__LINE__);
	}
	pthread_mutexattr_destroy(&mattr);
#endif
}

CBF_Mutex::~CBF_Mutex()
{
#ifdef  _WINDOWS
	CloseHandle(m_lock);
#else
	pthread_mutex_destroy(&m_lock);
#endif
}
bool CBF_Mutex::Lock()
{
#ifdef  _WINDOWS
	if (WaitForSingleObject(m_lock, INFINITE) != WAIT_OBJECT_0 )
	{
		return false;
	}
#else
	try
	{
		if (pthread_mutex_lock(&m_lock) !=0 )
		{
			return false;
		}
	}
	catch (...)
	{
		return false;
	}
#endif
	return true;
}

void CBF_Mutex::UnLock()
{
#ifdef   _WINDOWS
	ReleaseMutex(m_lock);
#else
	try
	{
		if (pthread_mutex_unlock(&m_lock) !=0 )
		{
//			m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"pthread_mutex_unlock 返回不等0,解锁失败");
		}
		else
		{
//			m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"pthread_mutex_unlock 解锁成功");
		}
	}
	catch (...)
	{
//		m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"pthread_mutex_unlock 异常");
		return ;
	}
	
	
#endif
}

int CBF_Mutex::TryLock()
{
#ifdef  _WINDOWS
	switch (WaitForSingleObject(m_lock,0))
	{
		case WAIT_OBJECT_0:
			return(1);  // 成功
		case WAIT_FAILED: 
			return(-1);  // 错误
		case WAIT_ABANDONED:  
			// The specified object is a mutex object that was not released by 
			// the thread that owned the mutex object before the owning thread terminated. 
			// Ownership of the mutex object is granted to the calling thread, and the mutex is set to nonsignaled
		case WAIT_TIMEOUT:
		default:  
			return(0);   
	}
#else
	switch (pthread_mutex_trylock(&m_lock))
	{
		case 0:        // 上锁成功
			return(1);
		case EINVAL:   // 互斥变量参数错误
			return(-1);
		default:       // EBUSY：互斥体此前已经被上锁了
			return(0);
	}
#endif
}


//////////////////////////////////////////////////////////////////////
// CBF_PMutex Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBF_PMutex::CBF_PMutex(CBF_Mutex *pmutex)
{
	m_pmutex = pmutex;
	if (m_pmutex->Lock())
	{
		m_bIsLock = true;
	}
	else
	{
		m_bIsLock = false;
	}
}

CBF_PMutex::~CBF_PMutex()
{
	m_pmutex->UnLock();
	m_pmutex = NULL;
}


//////////////////////////////////////////////////////////////////////
// CBF_MutexEvent Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBF_MutexEvent::CBF_MutexEvent()
{
#ifdef _WINDOWS
	m_event = CreateEvent(NULL,FALSE,FALSE,NULL);
	m_lock=CreateMutex(NULL,FALSE,NULL);
#else
	pthread_mutexattr_t mattr;
	if (pthread_mutexattr_init(&mattr) !=0 )
	{
		fprintf(stderr,"%s %d pthread_mutexattr_init error",__FILE__,__LINE__);
	}
	pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_RECURSIVE );
	if (pthread_mutex_init(&m_lock, &mattr) != 0)
	{
		fprintf(stderr,"%s %d pthread_mutex_init error",__FILE__,__LINE__);
	}
	pthread_mutexattr_destroy(&mattr);

	if (pthread_cond_init(&m_event,NULL) !=0)
	{
		printf("pthread_cond_init error\n");
	}

#endif
#ifdef LINUX
	sem_init(&m_sid, 0, 1);
#endif	
}

CBF_MutexEvent::~CBF_MutexEvent()
{
#ifdef _WINDOWS
	CloseHandle(m_lock);
	CloseHandle(m_event);
#else
	pthread_mutex_destroy(&m_lock);
	if (pthread_cond_broadcast(&m_event) != 0)
	{
		 printf("pthread_cond_broadcast err\n");
	}
	pthread_cond_destroy(&m_event);
#endif
#ifdef LINUX
	sem_destroy(&m_sid);
#endif	
}

void CBF_MutexEvent::WaitEvent()
{
#ifdef _WINDOWS
	UnLock();
	WaitForSingleObject(m_event,INFINITE);
	Lock();
//#elif LINUX
//	sem_wait(&m_sid);
#else
	pthread_cond_wait(&m_event,&m_lock);
#endif
}



void CBF_MutexEvent::SetEvent()
{
#ifdef _WINDOWS
    ::SetEvent(m_event);
//#elif LINUX
//	//linux用信号sem_t
//	sem_post(&m_sid);
#else
    pthread_cond_signal(&m_event); 
#endif
}

void CBF_MutexEvent::WaitEventTime(unsigned int wait_ms)
{
	
#ifdef _WINDOWS
	UnLock();
	WaitForSingleObject(m_event,wait_ms);
	Lock();
//#elif LINUX
//	//linux用信号sem_t
//	int ret;
//	CBF_Time_Value curt = CBF_Date_Time::Gettimeofday();
//	CBF_Time_Value addt;
//	addt.SetCurUs(wait_ms*1000);
//	//	addt.Set(wait_ms/1000,wait_ms%1000*1000);
//	curt += addt ;
//	timespec_t mytime = (timespec_t)curt;
//	sem_timedwait(&m_sid,&mytime);
#else
	int ret;
	CBF_Time_Value curt = CBF_Date_Time::Gettimeofday();
	CBF_Time_Value addt;
	addt.SetCurUs(wait_ms*1000);
//	addt.Set(wait_ms/1000,wait_ms%1000*1000);
	curt += addt ;
	timespec_t mytime = (timespec_t)curt;
	ret = pthread_cond_timedwait(&m_event,&m_lock,&mytime);
#endif
}

void CBF_MutexEvent::Lock()
{
#ifdef  _WINDOWS
	WaitForSingleObject(m_lock, INFINITE);
#else
	pthread_mutex_lock(&m_lock);
#endif

}

void CBF_MutexEvent::UnLock()
{
#ifdef   _WINDOWS
	ReleaseMutex(m_lock);
#else
	pthread_mutex_unlock(&m_lock);
#endif
}




int CBF_MutexEvent::TryLock()
{
#ifdef  _WINDOWS
	switch (WaitForSingleObject(m_lock,0))
	{
		case WAIT_OBJECT_0:
			return(1);  // 成功
		case WAIT_FAILED: 
			return(-1);  // 错误
		case WAIT_ABANDONED:  
			// The specified object is a mutex object that was not released by 
			// the thread that owned the mutex object before the owning thread terminated. 
			// Ownership of the mutex object is granted to the calling thread, and the mutex is set to nonsignaled
		case WAIT_TIMEOUT:
		default:  
			return(0);   
	}
#else
	switch (pthread_mutex_trylock(&m_lock))
	{
		case 0:        // 上锁成功
			return(1);
		case EINVAL:   // 互斥变量参数错误
			return(-1);
		default:       // EBUSY：互斥体此前已经被上锁了
			return(0);
	}
#endif
}


CBF_SpinMutex::CBF_SpinMutex()
{		
#ifdef  _WINDOWS
	m_SpinLock=CreateMutex(NULL,FALSE,NULL);
#else
	pthread_spin_init(&m_SpinLock,0);
#endif
}


CBF_SpinMutex::~CBF_SpinMutex()
{
#ifdef  _WINDOWS
	CloseHandle(m_SpinLock);
#else
	pthread_spin_destroy(&m_SpinLock);
#endif
	
}	

bool CBF_SpinMutex::Lock()
{
#ifdef  _WINDOWS
	//WaitForSingleObject(m_SpinLock, INFINITE);
	if (WaitForSingleObject(m_SpinLock, INFINITE) != WAIT_OBJECT_0 )
	{
		return false;
	}
#else
	try
	{
		if (pthread_spin_lock(&m_SpinLock) !=0 )
		{
			return false;
		}
	}
	catch (...)
	{
		return false;
	}
#endif
	return true;
}	

void CBF_SpinMutex::UnLock()
{
#ifdef  _WINDOWS
	ReleaseMutex(m_SpinLock);
#else
	pthread_spin_unlock(&m_SpinLock);
#endif
}	

CBF_PSpinMutex::CBF_PSpinMutex(CBF_SpinMutex *pmutex)
{
	m_pmutex = pmutex;
	if (m_pmutex->Lock())
	{
		m_bIsLock = true;
	}
	else
	{
		m_bIsLock = false;
	}
}

CBF_PSpinMutex::~CBF_PSpinMutex()
{
	m_pmutex->UnLock();
	m_pmutex = NULL;
}
