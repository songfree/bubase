// BF_Thread.cpp: implementation of the CBF_Thread class.
//
//////////////////////////////////////////////////////////////////////

#include "BF_Thread.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBF_Thread::CBF_Thread(const char *threadname)
{
	m_sThreadName = threadname;
	m_hThread = (THREAD_HANDLE)0;
	m_nThreadId = 0;
}

CBF_Thread::~CBF_Thread()
{

}
#ifdef _WINDOWS
DWORD WINAPI CBF_Thread::_ThreadProc(LPVOID pParam)
#else
void* CBF_Thread::_ThreadProc(void* pParam)
#endif
{
	CBF_Thread* pThread = (CBF_Thread*) pParam;
	pThread->m_tThreadBeginTime = pThread->m_pDateTime.Gettimeofday();
#ifdef _WINDOWS
	srand((unsigned int)pThread->m_pDateTime.GetTickCount());
#endif
	if(pThread->InitThreadInstance())
	{
		
		try
		{
#ifndef _WINDOWS	
			pThread->m_nThreadId = pthread_self();
#endif
			pThread->Run();
		}
		catch(...)
		{
            char logname[350];
			pThread->m_pDateTime.Update();
			sprintf(logname,"%sBF_Thread.log",pThread->m_pDateTime.ToStringD8().c_str());
			FILE *fp=NULL;
			fp = fopen(logname,"a");
			if (fp != NULL)
			{
				if (pThread->m_sThreadName.length()<1)
				{
					fprintf(fp,"%s BF_Thread.cpp ID=%ld name=%s ThreadProc Exception \n",\
						pThread->m_pDateTime.ToStringT6().c_str(),pThread->m_nThreadId,\
						"未知,继承线程类未置线程名称");
				}
				else
				{
					fprintf(fp,"%s BF_Thread.cpp ID=%ld name=%s ThreadProc Exception \n",\
						pThread->m_pDateTime.ToStringT6().c_str(),pThread->m_nThreadId,\
						pThread->m_sThreadName.c_str());
				}
				fclose(fp);
				fp = NULL;
			}
		}
		pThread->ExitThreadInstance();
	}
	pThread->m_hThread = (THREAD_HANDLE)0;
	pThread->m_tThreadEndTime = pThread->m_pDateTime.Gettimeofday();
	pThread->m_nThreadId = 0;
	return NULL;
}

bool CBF_Thread::InitThreadInstance()
{
	return true;
}

void CBF_Thread::ExitThreadInstance()
{
	
}

bool CBF_Thread::Join()
{
	THREAD_HANDLE hThread = GetThisThreadHandle();
	if(hThread == (THREAD_HANDLE)0)
	{
		return true;
	}
#ifdef _WINDOWS
	return (WaitForSingleObject(hThread,INFINITE) != 0);
#else
	return (pthread_join(hThread, NULL) == 0);

#endif
}

THREAD_HANDLE CBF_Thread::GetThisThreadHandle()
{
	return m_hThread;
}

bool CBF_Thread::CreateThread()
{
	//判断重复启动多次
	if (m_hThread != (THREAD_HANDLE)0)
	{
		return true;//线程已经启动了
	}

	bool ret = true;
#ifdef _WINDOWS
	m_hThread = ::CreateThread(NULL,0,&CBF_Thread::_ThreadProc,this,0,&m_nThreadId);
	
	if(m_hThread == NULL)
	{
		ret = false;
	}
#else	
	pthread_attr_t attr;
	pthread_attr_init (&attr);
	pthread_attr_setscope (&attr, PTHREAD_SCOPE_SYSTEM);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
	if((pthread_create(&m_hThread,&attr,_ThreadProc, this)) == 0)
	{
		ret=true;//创建成功
	}
    else
	{
        m_hThread = (THREAD_HANDLE)0;
		ret = false;
	}
	pthread_attr_destroy(&attr);
#endif
	return ret;
}

bool CBF_Thread::IsStoped()
{
	if(m_hThread == (THREAD_HANDLE)0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

CBF_Time_Value CBF_Thread::GetThreadRunTime()
{
	return m_tThreadEndTime - m_tThreadBeginTime;
}

THREAD_ID CBF_Thread::GetThisThreadId()
{
	return m_nThreadId;
}
