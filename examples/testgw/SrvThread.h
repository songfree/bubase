// SrvThread.h: interface for the CSrvThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SRVTHREAD_H__ADFE0EB7_9351_4482_9A33_AD5A1BCD4C06__INCLUDED_)
#define AFX_SRVTHREAD_H__ADFE0EB7_9351_4482_9A33_AD5A1BCD4C06__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BF_Thread.h"
#include "BF_SocketTcp.h"

#ifdef _WINDOWS
#ifdef _DEBUG
#pragma comment(lib, "bf_kerneld.lib") 
#pragma message("Automatically linking with  bf_kerneld.lib ")
#else
#pragma comment(lib, "bf_kernel.lib") 
#pragma message("Automatically linking with  bf_kernel.lib ")
#endif
#endif

class CSrvThread : public CBF_Thread  
{
public:
	bool Listen(unsigned short  port);
	CSrvThread();
	virtual ~CSrvThread();


	CBF_SocketTcp   m_pSocket;

	int RunSelect();

private:
	virtual int Run();
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

};

#endif // !defined(AFX_SRVTHREAD_H__ADFE0EB7_9351_4482_9A33_AD5A1BCD4C06__INCLUDED_)
