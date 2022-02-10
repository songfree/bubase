// BF_AIO_Thread.h: interface for the CBF_AIO_Thread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BF_AIO_THREAD_H__97E97381_77A3_4915_B4A2_7E59B6B9F252__INCLUDED_)
#define AFX_BF_AIO_THREAD_H__97E97381_77A3_4915_B4A2_7E59B6B9F252__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "BF_AIO_Pool.h"
#include "BF_Thread.h"
#include "BF_AIO_WorkBase.h"
#include "BF_AIO_Resource.h"
#include "MdbBase.h"
#include "BF_SocketTcp.h"
#include "IErrLog.h"
#include "BF_Timer.h"

#ifdef LINUX
#include <sys/epoll.h>
#endif

#ifdef _WINDOWS
#include "mswsock.h"
#pragma comment(lib, "Mswsock.lib") 
#pragma message("Automatically linking with Mswsock.lib")
#endif

#ifdef AIO_EXPORTS
#define AIO_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define AIO_EXPORT __declspec(dllimport)
#else
#define AIO_EXPORT 
#endif
#endif

class AIO_EXPORT CBF_AIO_Thread  : public CBF_Thread
{
public:
	void SetScan();
	static int OnTimer(unsigned int eventid,void *p);
	void ScanPool();

	// ������: ListIndex
	// ���  : ������ 2013-6-20 14:04:52
	// ����  : void 
	// ����  : ��ʾ���̹߳������������
	void ListIndex();

	// ������: AddSocketIndex
	// ���  : ������ 2013-6-20 14:04:57
	// ����  : void 
	// ����  : int index
	// ����  : ���뱾�߳�Ҫ�������������
	void AddSocketIndex(int index);

	// ������: SelectSend
	// ���  : ������ 2012-5-24 10:35:31
	// ����  : void 
	// ����  : PSOCKET_POOL_DATA info
	// ����  : �������ݣ�ֱ����������Ӳ���д
	void SelectSend(PSOCKET_POOL_DATA info);

	// ������: EpollSend
	// ���  : ������ 2012-5-24 10:34:11
	// ����  : void 
	// ����  : PSOCKET_POOL_DATA info
	// ����  : �������ݣ�ֱ����������Ӳ���д
	void EpollSend(PSOCKET_POOL_DATA info);

	// ������: EpollRecv
	// ���  : ������ 2012-5-24 10:34:16
	// ����  : void 
	// ����  : PSOCKET_POOL_DATA info
	// ����  : EPOLL�������ݣ�ֱ��û������
	void EpollRecv(PSOCKET_POOL_DATA info);

	// ������: SelectOnWriteEvent
	// ���  : ������ 2012-5-22 16:28:48
	// ����  : void 
	// ����  : PSOCKET_POOL_DATA info
	// ����  : ��Ӧselectģ��д�¼�
	void SelectOnWriteEvent(PSOCKET_POOL_DATA info);

	// ������: SelectOnReadEvent
	// ���  : ������ 2012-5-22 16:28:44
	// ����  : void 
	// ����  : PSOCKET_POOL_DATA info
	// ����  : ��Ӧselectģ�Ͷ��¼�
	void SelectOnReadEvent(PSOCKET_POOL_DATA info);

	// ������: SelectGetMaxfdAdd
	// ���  : ������ 2012-5-22 16:26:27
	// ����  : int <1û������ҪƬ����
	// ����  : int maxfd    ����socket
	// ����  : selectģ���У������ӳ���ȡ���߳�Ҫ��������Ӽ��뵽�����У����õ�����socket
	int SelectGetMaxfdAdd(int &maxfd);

	// ������: RunEpoll
	// ���  : ������ 2012-5-22 16:28:05
	// ����  : void 
	// ����  : ����EPOLLģ��
	void RunEpoll();

	// ������: RunIocp
	// ���  : ������ 2012-5-22 16:27:53
	// ����  : void 
	// ����  : ����iocpģ��
	void RunIocp();

	// ������: RunSelect
	// ���  : ������ 2012-5-22 16:27:32
	// ����  : void 
	// ����  : ����selectģ��
	void RunSelect();


	// ������: SetParameter
	// ���  : ������ 2012-5-22 15:07:21
	// ����  : void 
	// ����  : CBF_AIO_Pool *pool
	// ����  : CBF_AIO_WorkBase *work
	// ����  : CBF_AIO_Resource  *res
	// ����  : unsigned int index
	// ����  : ����IO�̲߳���
	void SetParameter(CBF_AIO_Pool *pool ,CBF_AIO_WorkBase *work,CBF_AIO_Resource  *res,unsigned int index,CIErrlog     *log);

	CBF_Timer         m_pTimer;

	CBF_AIO_Thread();
	virtual ~CBF_AIO_Thread();

	

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
	virtual int Run(); 


protected:

	

	bool              m_bIsScan;

	// ������: OnConnectBack
	// ���  : ������ 2013-6-5 17:45:47
	// ����  : void 
	// ����  : PSOCKET_POOL_DATA data
	// ����  : bool bret  true���ӳɹ�  falseʧ��
	// ����  : �����������ӵĻر�������ҵ����
	void OnConnectBack(PSOCKET_POOL_DATA data,bool bret);

	// ������: OnAccept
	// ���  : ������ 2013-6-5 17:45:12
	// ����  : void 
	// ����  : PSOCKET_POOL_DATA data
	// ����  : �յ�һ�����ӣ�����ҵ����д�����״̬���������͵�
	void OnAccept(PSOCKET_POOL_DATA data);

// 	// ������: OnSendBack
// 	// ���  : ������ 2013-6-5 17:33:38
// 	// ����  : void 
// 	// ����  : PSOCKET_POOL_DATA data
// 	// ����  : �����ӿ�дʱ�������е����ݷ��ͺ�ص���ҵ��ģ��������
// 	void OnSendBack(SOCKET_SEND_DATA data,bool bret=true);

	// ������: OnPing
	// ���  : ������ 2013-6-5 17:33:34
	// ����  : void 
	// ����  : PSOCKET_POOL_DATA data
	// ����  : 
	void OnPing(PSOCKET_POOL_DATA data);

	// ������: OnRecvBack
	// ���  : ������ 2013-6-5 17:31:43
	// ����  : void 
	// ����  : PSOCKET_POOL_DATA data
	// ����  : �յ������ˣ��ص���ҵ��ģ��������
	void OnRecvBack(PSOCKET_POOL_DATA data);

	// ������: OnClose
	// ���  : ������ 2013-6-5 17:31:23
	// ����  : void 
	// ����  : PSOCKET_POOL_DATA info
	// ����  : const char *filename
	// ����  : int fileline
	// ����  : const char *msg
	// ����  : �ر�����
	void OnClose(PSOCKET_POOL_DATA info,const char *filename,int fileline,const char *msg);

	// ������: SelectSend
	// ���  : ������ 2013-6-5 14:46:22
	// ����  : int  ���ط��͵�����  -1���ͳ���
	// ����  : SOCKSENDQUEUE::iterator prq
	// ����  : ��������
	int SelectSend(SOCKSENDQUEUE::iterator prq);

	CIErrlog     *m_pLog;
	CBF_AIO_WorkBase  *m_pAioWork;
	CBF_AIO_Pool      *m_pAioPool;
	CBF_AIO_Resource  *m_pAioRes;
	unsigned   int    m_nIndex;//���̵߳���������ʶ�ǵڼ���IO�߳�
	CBF_SocketTcp     m_pSocketTcp;

	fd_set m_rset;
	fd_set m_wset;
#if defined(_WINDOWS)
	fd_set m_eset;
#endif

	CInt   m_lSocketList;  //���μ��뵽select���б�

	CInt   m_lIndexList;   //���߳�Ҫ����������б�
	char       m_sErrMsg[256];
};

#endif // !defined(AFX_BF_AIO_THREAD_H__97E97381_77A3_4915_B4A2_7E59B6B9F252__INCLUDED_)
