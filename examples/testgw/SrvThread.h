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
	// ������: InitThreadInstance
	// ���  : ������ 2012-4-9 12:00:54
	// ����  : virtual bool 
	// ����  : �̳߳�ʼ�����߳�����ʱ����һ��
	virtual bool InitThreadInstance();
	
	// ������: ExitThreadInstance
	// ���  : ������ 2012-4-9 12:01:05
	// ����  : virtual void 
	// ����  : �˳��߳�Ҫ���Ĺ���,�߳��˳�ʱ����һ��
	virtual void ExitThreadInstance();

};

#endif // !defined(AFX_SRVTHREAD_H__ADFE0EB7_9351_4482_9A33_AD5A1BCD4C06__INCLUDED_)
