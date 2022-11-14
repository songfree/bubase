// BF_Timer.cpp: implementation of the CBF_Timer class.
//
//////////////////////////////////////////////////////////////////////

#include "BF_Timer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CBF_Timer::CBF_Timer()
{
	m_bIsExit = true;
	m_nTimerTick = 1000;//1秒
	m_hThread = (THREAD_HANDLE)0;
#ifdef _WINDOWS
    WORD wVersionRequested = MAKEWORD( 2, 2 );
	WSADATA wsaData;
	if (WSAStartup( wVersionRequested, &wsaData) != 0)
	{
		printf("can not init socket!\n");
	}
//	m_sock = socket(AF_INET,SOCK_STREAM,0);
//	int blockflag=1;
//	ioctlsocket(m_sock,FIONBIO,(unsigned long*)&blockflag);
#endif
	//m_pLog.SetLogPara(LOG_DEBUG,"","timer.log");
	m_nCpucore=0;
}

CBF_Timer::~CBF_Timer()
{
#ifdef _WINDOWS
	if (m_sock != INVALID_SOCKET)
	{
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
	}
	WSACleanup();
#endif
}

bool CBF_Timer::SetTimer(unsigned int timerid,unsigned long tms,TIME_CALL_BACK cb,void *p,unsigned short timeflag)
{
	BF_TIMER_PARA st;
	st.nEventId = timerid;
	st.nTimeFlag = timeflag;
	st.nCallNum = 0;
	st.nTimeOut = tms;
	st.pCB = cb;
	st.nTimeCount = tms;
	if (st.nTimeFlag == 2) //超长定时模式
	{
		st.nTimeCount = CBF_Date_Time::GetTickCount();
	}
	st.bIsUse = true;
	st.pC = p;
	CBF_PMutex pp(&m_mutex);
	LIST_TIMER::iterator prn;
	for (prn=m_lTimer.begin();prn!=m_lTimer.end();prn++)
	{
		if (prn->nEventId == timerid)
		{
			prn->nTimeFlag = timeflag;
			prn->nCallNum = 0;
			prn->nTimeOut = tms;
			prn->pCB = cb;
			prn->nTimeCount = tms;
			if (prn->nTimeFlag == 2) //超长定时模式
			{
				prn->nTimeCount = CBF_Date_Time::GetTickCount();
			}
			prn->pC = p;
			prn->bIsUse = true;
			return true;
		}
	}
	m_lTimer.push_back(st);
	return true;
}

bool CBF_Timer::KillTimer(unsigned int timerid)
{
	CBF_PMutex pp(&m_mutex);
	LIST_TIMER::iterator prn;
	for (prn=m_lTimer.begin();prn!=m_lTimer.end();)
	{
		if (prn->nEventId == timerid)
		{
			//m_lTimer.erase(prn);
			prn->bIsUse = false;
			return true;
		}
		prn++;
	}
	return false;
}

int CBF_Timer::OnTimer(unsigned int event)
{
	return 0;
}
bool CBF_Timer::InitThreadInstance()
{
	return true;
}


void CBF_Timer::ExitThreadInstance()
{
	return;
}


int	CBF_Timer::Run()
{
	struct timeval tv;
#if defined(_WINDOWS)
	fd_set rset;
#endif
#ifndef _WINDOWS
    if (m_nCpucore > 0)
    {
        cpu_set_t cpu_set;
        CPU_ZERO(&cpu_set);
        CPU_SET(m_nCpucore, &cpu_set);
        int ret = pthread_setaffinity_np(pthread_self(), sizeof(cpu_set), &cpu_set);
        //if (ret < 0)
        //{
        //    //printf("bind core fail\n");
        //    m_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, "bind core %d fail", m_nCpucore);
        //}
        //else
        //{
        //    //printf("bind core success\n");
        //    m_pLog->LogMp(LOG_INFO, __FILE__, __LINE__, "bind core %d success", m_nCpucore);
        //}
    }
#endif
	tv.tv_sec = m_nTimerTick/1000;
	tv.tv_usec = m_nTimerTick%1000*1000;
//	printf("tv_sec=%ld tv_usec=%ld\n",tv.tv_sec,tv.tv_usec);
	while(!m_bIsExit)
	{
		
#if defined(_WINDOWS)
		FD_ZERO(&rset);
		FD_SET(m_sock, &rset);
		select(m_sock+1, &rset, NULL, NULL, &tv);
#else
		tv.tv_sec = m_nTimerTick/1000;
		tv.tv_usec = m_nTimerTick%1000*1000;
		select(0, 0, NULL, NULL, &tv);
#endif
		if (m_bIsExit)
		{
			return 0;
		}
		//超时后进行处理
		ProcessTimer();
	}
	return 0;
}
bool CBF_Timer::Start()
{
	if (!m_bIsExit) //正在运行
	{
		return false;	
	}
	if (!IsStoped()) //正在运行
	{
		return false;
	}
	
#ifdef _WINDOWS
	m_sock = socket(AF_INET,SOCK_STREAM,0);
	int blockflag=1;
	ioctlsocket(m_sock,FIONBIO,(unsigned long*)&blockflag);
#endif
	m_bIsExit = false;
	return CreateThread();	

}

void CBF_Timer::Stop()
{
	//发送消息，停止线程
	m_bIsExit = true;
#ifdef _WINDOWS
	if (m_sock != INVALID_SOCKET)
	{
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
	}
#endif
}

void CBF_Timer::Init(unsigned long tick, bool accuracy, int cpucores)
{
	m_nTimerTick = tick;
#ifdef _WINDOWS
	//windows下的定时精度为10ms
	if (m_nTimerTick <10)
	{
		m_nTimerTick = 10;
	}
#endif
	m_bTimerAccuracy = accuracy;  
	m_nCpucore = cpucores;
}

void CBF_Timer::ProcessTimer()
{
	unsigned long btime;
	unsigned long etime;
	CBF_PMutex pp(&m_mutex);
	LIST_TIMER::iterator prn,prq;
	
	if (m_bTimerAccuracy) //精确定时
	{
		btime  = m_pDateTime.GetTickCount();
	}
	for (prn=m_lTimer.begin();prn!=m_lTimer.end();)
	{
		if (m_bIsExit)
		{
			return ;
		}
		if (!prn->bIsUse)
		{
			prn++;
			continue;
		}
		prq = prn;
		prn++;
		if (prq->nTimeFlag == 2) //超长定时模式
		{
			INT64_  diftime = m_pDateTime.GetTickCount();
			if (diftime - prq->nTimeCount - prq->nTimeOut >= 0) //时间到了
			{
				try
				{
					prq->pCB(prq->nEventId,prq->pC);
				}
				catch (...)
				{
					printf("定时回调失败 id=%d\n",prq->nEventId);
					return;
				}
				//m_lTimer.erase(prq);
				prq->bIsUse = false;
			}
			else  //没有到时间的定时处理，
			{
			
			}
			
		}
		else
		{
			prq->nTimeCount = prq->nTimeCount-m_nTimerTick;
			if (prq->nTimeCount <=0) //时间到了，回调
			{
				try
				{
					prq->pCB(prq->nEventId,prq->pC);
					if (m_bTimerAccuracy) //精确定时
					{
						etime = m_pDateTime.GetTickCount();
					}
				}
				catch (...)
				{
					printf("定时回调失败 id=%d\n",prq->nEventId);
					return;
				}
				//m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"timer_id[%d] bIsUse[%d] nCallNum[%d]", prq->nEventId, prq->nTimeFlag, prq->nCallNum);
				//若是一次，则删除
				if (prq->nTimeFlag == 0)
				{
					//m_lTimer.erase(prq);
					prq->bIsUse = false;
				}
				else
				{
					if (m_bTimerAccuracy) //精确定时
					{
						prq->nTimeCount = prq->nTimeOut-(etime-btime);//减去处理所耗时间
					}
					else
					{
						prq->nTimeCount = prq->nTimeOut;
					}
					prq->nCallNum++;
				}
			}
			else  //没有到时间的定时处理，
			{
				if (m_bTimerAccuracy) //精确定时
				{
					etime = m_pDateTime.GetTickCount();
					prq->nTimeCount = prq->nTimeCount-(etime-btime);//减去处理所耗时间
				}
			}
		}
		
	}
		
}



CBF_TimerUs::CBF_TimerUs()
{
	m_bIsExit = true;
	m_nTimerTick = 1000;//1000us
	m_hThread = (THREAD_HANDLE)0;
#ifdef _WINDOWS
	WORD wVersionRequested = MAKEWORD( 2, 2 );
	WSADATA wsaData;
	if (WSAStartup( wVersionRequested, &wsaData) != 0)
	{
		printf("can not init socket!\n");
	}
	//	m_sock = socket(AF_INET,SOCK_STREAM,0);
	//	int blockflag=1;
	//	ioctlsocket(m_sock,FIONBIO,(unsigned long*)&blockflag);
#endif
}

CBF_TimerUs::~CBF_TimerUs()
{
#ifdef _WINDOWS
	if (m_sock != INVALID_SOCKET)
	{
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
	}
	WSACleanup();
#endif
}

bool CBF_TimerUs::SetTimer(unsigned int timerid,unsigned long tus,TIME_CALL_BACK cb,void *p,unsigned short timeflag)
{
	BF_TIMER_PARA_US st;
	st.nEventId = timerid;
	st.nTimeFlag = timeflag;
	st.nCallNum = 0;
	st.nTimeOut = tus;
	st.pCB = cb;
	st.nTimeCount = tus;
	st.pC = p;
	if (st.nTimeFlag == 2) //超长定时模式
	{
		st.nTimeCount = CBF_Date_Time::GetTickUS();
	}
	st.bIsUse = true;
		CBF_PMutex pp(&m_mutex);
	LIST_TIMER_US::iterator prn;
	for (prn=m_lTimer.begin();prn!=m_lTimer.end();prn++)
	{
		if (prn->nEventId == timerid)
		{
			prn->nTimeFlag = timeflag;
			prn->nCallNum = 0;
			prn->nTimeOut = tus;
			prn->pCB = cb;
			prn->nTimeCount = tus;
			prn->pC = p;
			if (prn->nTimeFlag == 2) //超长定时模式
			{
				prn->nTimeCount = CBF_Date_Time::GetTickUS();
			}
			prn->bIsUse = true;
			return true;
		}
	}

	m_lTimer.push_back(st);
	return true;
}

bool CBF_TimerUs::KillTimer(unsigned int timerid)
{
		CBF_PMutex pp(&m_mutex);
	LIST_TIMER_US::iterator prn;
	for (prn=m_lTimer.begin();prn!=m_lTimer.end();)
	{
		if (prn->nEventId == timerid)
		{
			//m_lTimer.erase(prn);
			prn->bIsUse = false;
			return true;
		}
		prn++;
	}
	return false;
}

int CBF_TimerUs::OnTimer(unsigned int event)
{
	return 0;
}
bool CBF_TimerUs::InitThreadInstance()
{
	return true;
}


void CBF_TimerUs::ExitThreadInstance()
{
	return;
}


int	CBF_TimerUs::Run()
{
	struct timeval tv;
#if defined(_WINDOWS)
	fd_set rset;
#endif
	tv.tv_sec = m_nTimerTick/1000000;
	tv.tv_usec = m_nTimerTick%1000000;
	//	printf("tv_sec=%ld tv_usec=%ld\n",tv.tv_sec,tv.tv_usec);
	while(!m_bIsExit)
	{

#if defined(_WINDOWS)
		FD_ZERO(&rset);
		FD_SET(m_sock, &rset);
		select(m_sock+1, &rset, NULL, NULL, &tv);
#else
		tv.tv_sec = m_nTimerTick/1000000;
		tv.tv_usec = m_nTimerTick%1000000;
		select(0, 0, NULL, NULL, &tv);
#endif
		if (m_bIsExit)
		{
			return 0;
		}
		//超时后进行处理
		ProcessTimer();
	}
	return 0;
}
bool CBF_TimerUs::Start()
{
	if (!m_bIsExit) //正在运行
	{
		return false;	
	}
	if (!IsStoped()) //正在运行
	{
		return false;
	}

#ifdef _WINDOWS
	m_sock = socket(AF_INET,SOCK_STREAM,0);
	int blockflag=1;
	ioctlsocket(m_sock,FIONBIO,(unsigned long*)&blockflag);
#endif
	m_bIsExit = false;
	return CreateThread();	

}

void CBF_TimerUs::Stop()
{
	//发送消息，停止线程
	m_bIsExit = true;
#ifdef _WINDOWS
	if (m_sock != INVALID_SOCKET)
	{
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
	}
#endif
}

void CBF_TimerUs::Init(unsigned long tick, bool accuracy)
{
	m_nTimerTick = tick;
#ifdef _WINDOWS
	//windows下的定时精度为10ms
	if (m_nTimerTick <10*1000)
	{
		m_nTimerTick = 10*1000;
	}
#endif
	m_bTimerAccuracy = accuracy;
}

void CBF_TimerUs::ProcessTimer()
{
	INT64_ btime;
	INT64_ etime;
		CBF_PMutex pp(&m_mutex);
	LIST_TIMER_US::iterator prn,prq;

	if (m_bTimerAccuracy) //精确定时
	{
		btime  = m_pDateTime.GetTickCount();
	}
	for (prn=m_lTimer.begin();prn!=m_lTimer.end();)
	{
		if (m_bIsExit)
		{
			return ;
		}
		if (!prn->bIsUse)
		{
			prn++;
			continue;
		}
		prq = prn;
		prn++;
		
		if (prq->nTimeFlag == 2) //超长定时模式
		{
			INT64_  diftime = m_pDateTime.GetTickUS();
			if ( diftime - prq->nTimeCount - prq->nTimeOut >= 0) //时间到了
			{
				try
				{
					prq->pCB(prq->nEventId,prq->pC);
				}
				catch (...)
				{
					printf("定时回调失败 id=%d\n",prq->nEventId);
					return;
				}
				//m_lTimer.erase(prq);
				prq->bIsUse = false;
			}
			else  //没有到时间的定时处理，
			{

			}

		}
		else
		{
			prq->nTimeCount = prq->nTimeCount-m_nTimerTick;
			//		printf("定时 id=%d 计时=%ld\n",prq->nEventId,prq->nTimeCount);	
			if (prq->nTimeCount <=0) //时间到了，回调
			{
				try
				{
					//				printf("定时回调 id=%d %d %ld \n",prq->nEventId,prq->nTimeCount,m_pDateTime.GetTickCount());	
					prq->pCB(prq->nEventId,prq->pC);
					if (m_bTimerAccuracy) //精确定时
					{
						etime = m_pDateTime.GetTickUS();
					}
				}
				catch (...)
				{
					printf("定时回调失败 id=%d\n",prq->nEventId);
					return;
				}
				//若是一次，则删除
				if (prq->nTimeFlag == 0)
				{
					//m_lTimer.erase(prq);
					prq->bIsUse = false;
				}
				else
				{
					if (m_bTimerAccuracy) //精确定时
					{
						prq->nTimeCount = prq->nTimeOut-(etime-btime);//减去处理所耗时间
					}
					else
					{
						prq->nTimeCount = prq->nTimeOut;
					}
					prq->nCallNum++;
				}
			}
			else  //没有到时间的定时处理，
			{

			}
		}
		
	}

}
