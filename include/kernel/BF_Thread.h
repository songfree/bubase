// BF_Thread.h: interface for the CBF_Thread class.
//
//////////////////////////////////////////////////////////////////////

/***********************************************************************************
  编码日志记录
  编码者: 王明松
  编码日期 2012-4-17 10:32:30 重构
  版本: V1.00 
  说明: 线程基类,继承实现Run InitThreadInstance  ExitThreadInstance 方法 编译时加入 -D_REENTRANT
 ***********************************************************************************/


#if !defined(AFX_BF_THREAD_H__B1CE0594_0486_430D_817B_A62509845924__INCLUDED_)
#define AFX_BF_THREAD_H__B1CE0594_0486_430D_817B_A62509845924__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "BF_Date_Time.h"
#include "BF_Time_Value.h"	// Added by ClassView

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
typedef HANDLE         THREAD_HANDLE ;
typedef DWORD          THREAD_ID ;
#else

typedef pthread_t      THREAD_HANDLE ;
typedef pthread_t      THREAD_ID ;
#endif


class KERNEL_EXPORT CBF_Thread  
{
public:
	

	// 函数名: GetThreadRunTime
	// 编程  : 王明松 2012-4-26 10:38:05
	// 返回  : CBF_Time_Value 
	// 描述  : 取得本次线程运行时间
	CBF_Time_Value GetThreadRunTime();

	// 函数名: IsStoped
	// 编程  : 王明松 2012-4-26 10:38:23
	// 返回  : bool 
	// 描述  : 线程是否停止
	bool IsStoped();

	// 函数名: CreateThread
	// 编程  : 王明松 2012-4-9 12:00:05
	// 返回  : bool 
	// 描述  : 创建线程并运行
	bool CreateThread();

	// 函数名: GetThisThreadHandle
	// 编程  : 王明松 2012-4-9 12:00:25
	// 返回  : THREAD_HANDLE 
	// 描述  : 获取本线程句柄
	THREAD_HANDLE GetThisThreadHandle();


	// 函数名: GetThisThreadId
	// 编程  : 王明松 2012-4-26 10:41:13
	// 返回  : THREAD_ID 
	// 描述  : 取得本线程的ID
	THREAD_ID GetThisThreadId();

	// 函数名: Join
	// 编程  : 王明松 2012-4-9 12:00:37
	// 返回  : bool 
	// 描述  : 等待线程终止
	bool Join();
	
	
	CBF_Thread(const char *threadname="BF_Thread");
	virtual ~CBF_Thread();
	
	
	std::string    m_sThreadName; //线程名称

protected:
	
	CBF_Date_Time  m_pDateTime;
	CBF_Time_Value m_tThreadBeginTime;//线程开始时间
	CBF_Time_Value m_tThreadEndTime;  //线程结束时间

	THREAD_HANDLE  m_hThread;    //线程句柄
	THREAD_ID      m_nThreadId;  //线程ID


private:
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
	virtual int Run()=0; 

	// 函数名: _ThreadProc
	// 编程  : 王明松 2012-4-9 10:52:41
	// 返回  : static void 
	// 参数  : void *p 线程类指针
	// 描述  : 启动线程
protected:

#ifdef _WINDOWS
	static DWORD WINAPI _ThreadProc(LPVOID pParam);
#else
	static void* _ThreadProc(void* pParam);
#endif
};

#endif // !defined(AFX_BF_THREAD_H__B1CE0594_0486_430D_817B_A62509845924__INCLUDED_)
