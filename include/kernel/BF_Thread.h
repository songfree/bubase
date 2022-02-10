// BF_Thread.h: interface for the CBF_Thread class.
//
//////////////////////////////////////////////////////////////////////

/***********************************************************************************
  ������־��¼
  ������: ������
  �������� 2012-4-17 10:32:30 �ع�
  �汾: V1.00 
  ˵��: �̻߳���,�̳�ʵ��Run InitThreadInstance  ExitThreadInstance ���� ����ʱ���� -D_REENTRANT
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
	

	// ������: GetThreadRunTime
	// ���  : ������ 2012-4-26 10:38:05
	// ����  : CBF_Time_Value 
	// ����  : ȡ�ñ����߳�����ʱ��
	CBF_Time_Value GetThreadRunTime();

	// ������: IsStoped
	// ���  : ������ 2012-4-26 10:38:23
	// ����  : bool 
	// ����  : �߳��Ƿ�ֹͣ
	bool IsStoped();

	// ������: CreateThread
	// ���  : ������ 2012-4-9 12:00:05
	// ����  : bool 
	// ����  : �����̲߳�����
	bool CreateThread();

	// ������: GetThisThreadHandle
	// ���  : ������ 2012-4-9 12:00:25
	// ����  : THREAD_HANDLE 
	// ����  : ��ȡ���߳̾��
	THREAD_HANDLE GetThisThreadHandle();


	// ������: GetThisThreadId
	// ���  : ������ 2012-4-26 10:41:13
	// ����  : THREAD_ID 
	// ����  : ȡ�ñ��̵߳�ID
	THREAD_ID GetThisThreadId();

	// ������: Join
	// ���  : ������ 2012-4-9 12:00:37
	// ����  : bool 
	// ����  : �ȴ��߳���ֹ
	bool Join();
	
	
	CBF_Thread(const char *threadname="BF_Thread");
	virtual ~CBF_Thread();
	
	
	std::string    m_sThreadName; //�߳�����

protected:
	
	CBF_Date_Time  m_pDateTime;
	CBF_Time_Value m_tThreadBeginTime;//�߳̿�ʼʱ��
	CBF_Time_Value m_tThreadEndTime;  //�߳̽���ʱ��

	THREAD_HANDLE  m_hThread;    //�߳̾��
	THREAD_ID      m_nThreadId;  //�߳�ID


private:
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

	// ������: Run
	// ���  : ������ 2012-4-9 12:01:23
	// ����  : virtual int 
	// ����  : �߳����з��������˷������أ����߳��Զ���ֹ
	virtual int Run()=0; 

	// ������: _ThreadProc
	// ���  : ������ 2012-4-9 10:52:41
	// ����  : static void 
	// ����  : void *p �߳���ָ��
	// ����  : �����߳�
protected:

#ifdef _WINDOWS
	static DWORD WINAPI _ThreadProc(LPVOID pParam);
#else
	static void* _ThreadProc(void* pParam);
#endif
};

#endif // !defined(AFX_BF_THREAD_H__B1CE0594_0486_430D_817B_A62509845924__INCLUDED_)
