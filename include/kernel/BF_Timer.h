// BF_Timer.h: interface for the CBF_Timer class.
//
//////////////////////////////////////////////////////////////////////

/***********************************************************************************
  编码日志记录
  编码者: 王明松
  编码日期 2012-4-11 17:06:08 重构
  版本: V1.00 
  说明: 定时器类，通过定时器线程进行定时,使用例子如下：
class testtimer
{
public:
	CBF_Timer tmer;
	CBF_Date_Time tDate;
	
	testtimer()
	{
		tmer.Init(10,true); //初始定时器参数
		tmer.SetTimer(0,1000,&testtimer::OnTimer,this); //设置定时器
		tmer.SetTimer(1,5000,&testtimer::OnTimer,this);//设置定时器
		tmer.Start();//启动定时器
	}
	virtual ~testtimer()
	{
		tmer.KillTimer(0); //删除一个定时
		tmer.Stop();//停止定时器
	}
	void Stop()
	{
		tmer.Stop();
	}
	
	void join()
	{
		tmer.Join();
	}
	static int OnTimer(unsigned int event,void *p) //定时器回调函数
	{
		testtimer *pp = (testtimer *)p;
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

 ***********************************************************************************/
#if !defined(AFX_BF_TIMER_H__7D335AB3_46FA_40BE_8CF9_8348AAE23018__INCLUDED_)
#define AFX_BF_TIMER_H__7D335AB3_46FA_40BE_8CF9_8348AAE23018__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "BF_Thread.h"
#include "BF_Date_Time.h"
//#include "BF_SimpleLog.h"
#include "BF_Mutex.h"


#ifdef KERNEL_EXPORTS
#define KERNEL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNEL_EXPORT __declspec(dllimport)
#else
#define KERNEL_EXPORT 
#endif
#endif

#ifdef _WINDOWS
#pragma comment(lib, "WS2_32.lib") 
#pragma message("Automatically linking with WS2_32.lib")
#endif


typedef  int(*TIME_CALL_BACK)(unsigned int eventid,void *p);

class KERNEL_EXPORT CBF_Timer   : public CBF_Thread
{
public:

	// 函数名: Init
	// 编程  : 王明松 2012-4-16 11:56:23
	// 返回  : void 
	// 参数  : unsigned long tick 定时间隔ms  即select的ms windows下最小为10
	// 参数  : bool accuracy=false   是否精确定时
	// 描述  : 初始化定时器的精确性及定时间隔
	void Init(unsigned long tick,bool accuracy=false, int cpucores = 0);

	// 函数名: Stop
	// 编程  : 王明松 2012-4-16 11:46:07
	// 返回  : void 
	// 描述  : 停止定时器线程，所以的定时器停止，但并不删除定时器
	void Stop();

	// 函数名: Start
	// 编程  : 王明松 2012-4-16 11:45:28
	// 返回  : bool 
	// 描述  : 启动定时器线程，之后可以调用SetTimer设定一个定时器
	bool Start();

	// 函数名: OnTimer
	// 编程  : 王明松 2012-4-11 17:44:12
	// 返回  : virtual int 
	// 参数  : unsigned int event 定时器ID
	// 描述  : 定时器回调虚函数，继承时使用。要注意线程安全
	virtual int OnTimer(unsigned int event);

	// 函数名: SetTimer
	// 编程  : 王明松 2012-4-11 17:44:42
	// 返回  : bool 
	// 参数  : unsigned int timerid 定时器ID
	// 参数  : unsigned long tms     定时毫秒
	// 参数  : TIME_CALL_BACK cb   回调方法
	// 参数  : void *p   回调方法所在的类指针
	// 参数  : unsigned short timeflag 定时器类型 0一次型  1循环型 2：一次长时间型
	// 描述  : 设置一个定时器
	bool SetTimer(unsigned int timerid,unsigned long tms,TIME_CALL_BACK cb,void *p,unsigned short timeflag=1);

	// 函数名: KillTimer
	// 编程  : 王明松 2012-4-11 17:45:22
	// 返回  : bool 
	// 参数  : unsigned int timerid 定时器ID
	// 描述  : 删除一个定时器，注意不能在回调函数里调用本方法
	bool KillTimer(unsigned int timerid);

	CBF_Timer();
	virtual ~CBF_Timer();
	//CBF_SimpleLog  m_pLog;
protected:

	// 函数名: ProcessTimer
	// 编程  : 王明松 2012-4-16 17:33:45
	// 返回  : void 
	// 描述  : 处理定时事件
	void ProcessTimer();

	// 函数名: InitThreadInstance
	// 编程  : 王明松 2012-4-9 12:00:54
	// 返回  : virtual bool 
	// 描述  : 线程初始化，线程启动时调用一次
	virtual bool InitThreadInstance();
	
	// 函数名: ExitThreadInstance
	// 编程  : 王明松 2012-4-9 12:01:05
	// 返回  : virtual void 
	// 描述  : 退出线程要做的工作,线程退出时调用一次
	virtual void ExitThreadInstance();
	
	// 函数名: Run
	// 编程  : 王明松 2012-4-9 12:01:23
	// 返回  : virtual int 
	// 描述  : 线程运行方法，若此方法返回，则线程自动终止
	virtual int Run(); 

	typedef struct  
	{
		unsigned int    nEventId;//定时器ID
		unsigned long   nTimeOut;//定时器定时时间 ms
		INT64_          nTimeCount;//定时器时间记数 ms
		long            nCallNum;//回调次数
		unsigned short  nTimeFlag;//定时器类型  0 一次型  1循环型   2：一次长时间型
		TIME_CALL_BACK  pCB;//回调函数
		void            *pC; //回调函数所在类实例指针
		bool            bIsUse;//此定时器是否使用
	}BF_TIMER_PARA;
	
	typedef std::list<BF_TIMER_PARA> LIST_TIMER;
	
	LIST_TIMER m_lTimer;
	
	unsigned long   m_nTimerTick;  //定时器select 的时间 ms
	bool            m_bTimerAccuracy;//是否精确的定时 精确的定时需要耗cpu资源
	bool            m_bIsExit;//定时器停止标志
	
	CBF_Mutex       m_mutex;  //互斥
	CBF_Date_Time   m_pDateTime;
#ifdef _WINDOWS
	typedef SOCKET SOCKET_HANDLE;
	SOCKET_HANDLE m_sock;
#endif
	int             m_nCpucore;
};


class KERNEL_EXPORT CBF_TimerUs   : public CBF_Thread
{
public:

	// 函数名: Init
	// 编程  : 王明松 2012-4-16 11:56:23
	// 返回  : void 
	// 参数  : unsigned long tick 定时间隔us  即select的us windows下最小为10ms
	// 参数  : bool accuracy=false   是否精确定时
	// 描述  : 初始化定时器的精确性及定时间隔
	void Init(unsigned long tick,bool accuracy=false);

	// 函数名: Stop
	// 编程  : 王明松 2012-4-16 11:46:07
	// 返回  : void 
	// 描述  : 停止定时器线程，所以的定时器停止，但并不删除定时器
	void Stop();

	// 函数名: Start
	// 编程  : 王明松 2012-4-16 11:45:28
	// 返回  : bool 
	// 描述  : 启动定时器线程，之后可以调用SetTimer设定一个定时器
	bool Start();

	// 函数名: OnTimer
	// 编程  : 王明松 2012-4-11 17:44:12
	// 返回  : virtual int 
	// 参数  : unsigned int event 定时器ID
	// 描述  : 定时器回调虚函数，继承时使用。要注意线程安全
	virtual int OnTimer(unsigned int event);

	// 函数名: SetTimer
	// 编程  : 王明松 2012-4-11 17:44:42
	// 返回  : bool 
	// 参数  : unsigned int timerid 定时器ID
	// 参数  : unsigned long tus     定时微秒
	// 参数  : TIME_CALL_BACK cb   回调方法
	// 参数  : void *p   回调方法所在的类指针
	// 参数  : unsigned short timeflag 定时器类型 0一次型  1循环型 2一次长时间型
	// 描述  : 设置一个定时器
	bool SetTimer(unsigned int timerid,unsigned long tus,TIME_CALL_BACK cb,void *p,unsigned short timeflag=1);

	// 函数名: KillTimer
	// 编程  : 王明松 2012-4-11 17:45:22
	// 返回  : bool 
	// 参数  : unsigned int timerid 定时器ID
	// 描述  : 删除一个定时器，注意不能在回调函数里调用本方法
	bool KillTimer(unsigned int timerid);

	CBF_TimerUs();
	virtual ~CBF_TimerUs();

protected:

	// 函数名: ProcessTimer
	// 编程  : 王明松 2012-4-16 17:33:45
	// 返回  : void 
	// 描述  : 处理定时事件
	void ProcessTimer();

	// 函数名: InitThreadInstance
	// 编程  : 王明松 2012-4-9 12:00:54
	// 返回  : virtual bool 
	// 描述  : 线程初始化，线程启动时调用一次
	virtual bool InitThreadInstance();

	// 函数名: ExitThreadInstance
	// 编程  : 王明松 2012-4-9 12:01:05
	// 返回  : virtual void 
	// 描述  : 退出线程要做的工作,线程退出时调用一次
	virtual void ExitThreadInstance();

	// 函数名: Run
	// 编程  : 王明松 2012-4-9 12:01:23
	// 返回  : virtual int 
	// 描述  : 线程运行方法，若此方法返回，则线程自动终止
	virtual int Run(); 

	typedef struct  
	{
		unsigned int    nEventId;//定时器ID
		INT64_          nTimeOut;//定时器定时时间 US
		INT64_          nTimeCount;//定时器时间记数 
		long            nCallNum;//回调次数
		unsigned short  nTimeFlag;//定时器类型  0 一次型  1循环型 2：一次长时间型nTimeOut >100000ms
		TIME_CALL_BACK  pCB;//回调函数
		void            *pC; //回调函数所在类实例指针
		bool            bIsUse;//此定时器是否使用
	}BF_TIMER_PARA_US;

	typedef std::list<BF_TIMER_PARA_US> LIST_TIMER_US;

	LIST_TIMER_US m_lTimer;

	unsigned long   m_nTimerTick;  //定时器select 的时间us
	bool            m_bTimerAccuracy;//是否精确的定时 精确的定时需要耗cpu资源
	bool            m_bIsExit;//定时器停止标志
	CBF_Mutex       m_mutex;  //互斥
	CBF_Date_Time   m_pDateTime;
#ifdef _WINDOWS
	typedef SOCKET SOCKET_HANDLE;
	SOCKET_HANDLE m_sock;
#endif
};

#endif // !defined(AFX_BF_TIMER_H__7D335AB3_46FA_40BE_8CF9_8348AAE23018__INCLUDED_)
