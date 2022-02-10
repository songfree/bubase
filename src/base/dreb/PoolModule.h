// PoolModule.h: interface for the CPoolModule class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POOLMODULE_H__4CB367FB_FA90_4631_8E27_179703CAF20D__INCLUDED_)
#define AFX_POOLMODULE_H__4CB367FB_FA90_4631_8E27_179703CAF20D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "BF_Thread.h"
#include "IErrLog.h"
#include "DrebPubDefine.h"
#include "BF_SocketTcp.h"
#include "DrebEndian.h"
#include "PoolData.h"
#include "SocketMgr.h"
#include "GlobalVar.h"
#include "MemDB.h"
#include "MsgProcThread.h"
#include "BF_Timer.h"

class CPoolModule : public CBF_Thread  
{
public:

	// ������: OnMonitor
	// ���  : ������ 2013-9-2 9:39:06
	// ����  : void 
	// ����  : ��ʱ���
	void OnMonitor();
	
	// ������: GetDrebCmdType
	// ���  : ������ 2013-5-13 16:09:40
	// ����  : std::string 
	// ����  : int cmdtype
	// ����  : ȡdreb������
	std::string GetDrebCmdType(int cmdtype);

	void LogDrebHead(int loglevel, DREB_HEAD head, const char *msg, const char *filename, int fileline);


	// ������: OnServiceRoute
	// ���  : ������ 2013-5-13 16:09:54
	// ����  : void 
	// ����  : ����·��
	void OnServiceRoute();

	// ������: PrintRoute
	// ���  : ������ 2010-7-22 11:07:38
	// ����  : void 
	// ����  : ��ӡ·��
	void PrintRoute();

	// ������: OnMsgProc
	// ���  : ������ 2010-7-22 11:07:56
	// ����  : void 
	// ����  : S_DREB_RSMSG *msg
	// ����  : ������Ϣ
	void OnMsgProc(S_DREB_RSMSG *msg);

	// ������: OnRoute
	// ���  : ������ 2010-7-22 11:07:43
	// ����  : void 
	// ����  : ��������·��֪ͨ
	void OnRoute();

	/// ������: OnRecv
	/// ���  : ������ 2009-5-21 15:17:35
	/// ����  : void 
	/// ����  : int conindex
	/// ����  : ��������
	void OnRecv(int conindex);

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

	/// ������: OnConnect
	/// ���  : ������ 2009-5-21 15:16:12
	/// ����  : void 
	/// ����  : int conindex
	/// ����  : ȥ��������
	void OnConnect(int conindex);


	// ������: OnConnected
	// ���  : ������ 2013-6-24 12:43:58
	// ����  : void 
	// ����  : int conindex
	// ����  : ���ӳɹ�
	void OnConnected(int conindex);

	/// ������: OnClose
	/// ���  : ������ 2009-5-21 13:18:27
	/// ����  : void 
	/// ����  : int conindex ���ӵ��±�
	/// ����  : const char *msg �ر�����ԭ��
	/// ����  : �رտͻ��˻���������
	void OnClose(int conindex,const char *msg,const char *filename,int fileline);

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


	/// ������: GetMaxfdAddSelect
	/// ���  : ������ 2009-5-21 11:51:00
	/// ����  : int 
	/// ����  : int &maxfd
	/// ����  : ��ȡ����socket����socket���뵽select����
	int GetMaxfdAddSelect(int &maxfd);

	/// ������: Initial
	/// ���  : ������ 2009-5-19 10:28:44
	/// ����  : bool 
	/// ����  : ��ʼ��
	bool SetGlobalVar(CGlobalVar *pVar, CSocketMgr *sockmgr, CMemDB *pMemDb, CBF_BufferPool *pool,CPoolData *msgqueue);

	CPoolModule();
	virtual ~CPoolModule();

	CIErrlog   *m_log;
	bool m_bIsInit;
	int m_nEnd;
	int m_nBegin;
	
	CBF_SocketTcp m_tcpSocket;
	CMsgProcThread *m_pMsgProc;//��Ϣ������

	// ������: OnTimer
	// ���  : ������ 2013-3-6 16:43:23
	// ����  : static int 
	// ����  : unsigned int event
	// ����  : void *p
	// ����  : ��ʱ���ص�����
	static int OnTimer(unsigned int eventid,void *p);


	// ������: ServiceRouteTimer
	// ���  : ������ 2013-6-24 11:31:59
	// ����  : void 
	// ����  : ��ʱ�������ͷ���·��
	void ServiceRouteTimer();

	// ������: RouteTimer
	// ���  : ������ 2013-6-24 11:32:03
	// ����  : void 
	// ����  : ��ʱ�������ͽڵ�·��
	void RouteTimer();


	// ������: PingTimer
	// ���  : ������ 2013-6-24 11:34:04
	// ����  : void 
	// ����  : ��ʱ�����������
	void PingTimer();


	// ������: StopTimer
	// ���  : ������ 2013-6-24 11:41:55
	// ����  : void 
	// ����  : ֹͣ��ʱ��
	void StopTimer();

	// ������: StartTimer
	// ���  : ������ 2013-6-24 11:41:58
	// ����  : void 
	// ����  : ������ʱ��
	void StartTimer();

protected:
	bool      m_bIsPingTimer;
	bool      m_bIsRouteTimer;
	bool      m_bIsServiceTimer;

	CBF_Timer m_pTimer;//��ʱ��

	int m_nDrebHeadLen;

	CDrebEndian  m_pDrebEndian;    //�ֽ���

	CGlobalVar     *m_pRes;        //ȫ�ֲ���

	CSocketMgr     *m_pSocketMgr;  //���ӹ���
	CBF_BufferPool *m_pMemPool;  //�ڴ滺���
	CPoolData      *m_pMsgQueue;//��Ϣ����

	fd_set m_rset;
	fd_set m_wset;
#if defined(_WINDOWS)
	fd_set m_eset;
#endif
	char  m_headMsg[8192];

private:
	virtual int Run();
	virtual bool InitThreadInstance();
	virtual void ExitThreadInstance();
	
};

#endif // !defined(AFX_POOLMODULE_H__4CB367FB_FA90_4631_8E27_179703CAF20D__INCLUDED_)
