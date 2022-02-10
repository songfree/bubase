// PoolModule.h: interface for the CPoolModule class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POOLMODULE_H__DB9F06B7_888C_42E9_9755_AB2847BE09C0__INCLUDED_)
#define AFX_POOLMODULE_H__DB9F06B7_888C_42E9_9755_AB2847BE09C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "BF_Thread.h"
#include "IErrLog.h"
#include "BF_SocketTcp.h"
#include "TradeResource.h"
#include "SocketMgr.h"
#include "BF_BufferPool.h"
#include "PoolData.h"
#include "BF_Timer.h"
#include "BpcHead.h"
#include "SendQueue.h"



class  CPoolModule : public CBF_Thread  
{
public:
	
	CPoolModule();
	virtual ~CPoolModule();


	// ������: Run
	// ���  : ������ 2010-3-12 13:00:15
	// ����  : virtual int 
	// ����  : ���շ����߳�������
	virtual int Run();
	virtual bool Terminate();

	CIErrlog   *m_log;
	CTradeResource  *m_pRes;
	CSocketMgr  m_pSockMgr;
	CBF_BufferPool *m_pMemPool;//�ڴ�����;
	CPoolDataMsg   *m_pRcvQueue;//���ն���
	
	
	
	// ������: OnTimer
	// ���  : ������ 2013-3-6 16:43:23
	// ����  : static int 
	// ����  : unsigned int event
	// ����  : void *p
	// ����  : ��ʱ���ص�����
	static int OnTimer(unsigned int eventid,void *p);
	void PingTimer();

public:

	// ������: StartFront
	// ���  : ������ 2016-3-23 14:14:02
	// ����  : bool 
	// ����  : ����select�߳�
	bool StartFront();


	void Monitor();


	// ������: StopFront
	// ���  : ������ 2010-3-12 12:59:33
	// ����  : void 
	// ����  : ֹͣselect
	void StopFront();

	// ������: InitFront
	// ���  : ������ 2010-3-12 12:59:42
	// ����  : bool 
	// ����  : CTradeResource *res
	// ����  : ��ʼ������ǰ��
	bool InitFront(CTradeResource *res,CPoolDataMsg *que,CIErrlog *log);


	int GetMaxfdAddSelect(int &maxfd);

	/// ������: OnWriteEvent
	/// ���  : ������ 2009-5-21 15:17:24
	/// ����  : void 
	/// ����  : int conindex
	/// ����  : ��Ӧselectд�¼�
	void OnWriteEvent(int conindex);
	
	/// ������: OnReadEvent
	/// ���  : ������ 2009-5-21 15:17:03
	/// ����  : void 
	/// ����  : int conindex
	/// ����  : ��Ӧselect���¼�
	void OnReadEvent(int conindex);
	
	/// ������: OnAccept
	/// ���  : ������ 2009-5-21 15:16:41
	/// ����  : void 
	/// ����  : ��������
	void OnAccept(int conindex);
	
	/// ������: OnSend
	/// ���  : ������ 2009-5-21 15:16:33
	/// ����  : void 
	/// ����  : int conindex
	/// ����  : ��������
	void OnSend(int conindex);

	// ������: OnRecv
	// ���  : ������ 2010-3-12 14:06:55
	// ����  : void 
	// ����  : int conindex
	// ����  : �������ݲ��������
	void OnRecv(int conindex);
	
	
	
	/// ������: OnClose
	/// ���  : ������ 2009-5-21 13:18:27
	/// ����  : void 
	/// ����  : int conindex ���ӵ��±�
	/// ����  : const char *msg �ر�����ԭ��
	/// ����  : �رտͻ��˻���������
	void OnClose(int conindex,const char *msg);
	
	/// ������: OnPing
	/// ���  : ������ 2009-5-21 13:17:22
	/// ����  : void 
	/// ����  : int conindex ���ӵ��±�
	/// ����  : ��������
	void OnPing(int conindex);
	
	
	/// ������: OnEvent
	/// ���  : ������ 2009-5-21 13:16:19
	/// ����  : void 
	/// ����  : select�����¼��Ĵ���,������ݡ�������
	void OnEvent();
	
	/// ������: OnNoEvent
	/// ���  : ������ 2009-5-21 13:16:35
	/// ����  : void 
	/// ����  : select�����¼��Ĵ���������
	void OnNoEvent();

	bool ResetData(int index,S_TRADE_DATA *data);
protected:
	S_TRADE_DATA   m_pDataRcv;

	CBF_SocketTcp  m_tcpSocket;

	bool m_bIsInit;
	char m_errMsg[256];
	bool m_bIsRunning;
	
	fd_set m_rset;
	fd_set m_wset;
#if defined(_WINDOWS)
	fd_set m_eset;
#endif
	CBF_Timer m_pTimer;//��ʱ��
	bool      m_bIsPingTimer;
};

#endif // !defined(AFX_POOLMODULE_H__DB9F06B7_888C_42E9_9755_AB2847BE09C0__INCLUDED_)
