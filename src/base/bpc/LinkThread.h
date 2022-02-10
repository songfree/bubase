// LinkThread.h: interface for the CLinkThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LINKTHREAD_H__B0F78DC0_7B0B_4189_AE4D_6111A689B1A7__INCLUDED_)
#define AFX_LINKTHREAD_H__B0F78DC0_7B0B_4189_AE4D_6111A689B1A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "BF_Thread.h"
#include "BF_LogClient.h"
#include "BF_SocketTcp.h"
#include "GResource.h"
#include "SocketMgr.h"
#include "BF_BufferPool.h"
#include "PoolData.h"
#include "FuncTbl.h"
#include "DrebEndian.h"
#include "BF_Timer.h"
#include "BF_ProcessMgr.h"

class CLinkThread : public CBF_Thread  
{
public:
	void Upload(S_BPC_RSMSG &rcvdata);
	void Download(S_BPC_RSMSG &rcvdata);

	// ������: OnMonitor
	// ���  : ������ 2013-11-25 14:15:07
	// ����  : void 
	// ����  : �������ͼ�ر���
	void OnMonitor();

	// ������: OnBpuIsReg
	// ���  : ������ 2013-11-25 14:15:31
	// ����  : void 
	// ����  : int index
	// ����  : S_BPC_RSMSG &rcvdata
	// ����  : ��ӦBPU�Ƿ�Ҫע��
	void OnBpuIsReg(int index,S_BPC_RSMSG &rcvdata);

	// ������: MonitorStartBpu
	// ���  : ������ 2013-11-25 14:18:41
	// ����  : void 
	// ����  : S_BPC_RSMSG &rcvdata
	// ����  : �������BPU
	void MonitorStartBpu(S_BPC_RSMSG &rcvdata);

	// ������: MonitorStopBpu
	// ���  : ������ 2013-11-25 14:18:30
	// ����  : void 
	// ����  : S_BPC_RSMSG &rcvdata
	// ����  : ���ֹͣBPU
	void MonitorStopBpu(S_BPC_RSMSG &rcvdata);

	// ������: MonitorSetMode
	// ���  : ������ 2013-11-25 14:18:05
	// ����  : void 
	// ����  : S_BPC_RSMSG &rcvdata
	// ����  : ������ü��ģʽ
	void MonitorSetMode(S_BPC_RSMSG &rcvdata);

	// ������: MonitorSetBpu
	// ���  : ������ 2013-11-25 14:17:48
	// ����  : void 
	// ����  : S_BPC_RSMSG &rcvdata
	// ����  : �������BPU����
	void MonitorSetBpu(S_BPC_RSMSG &rcvdata);

	// ������: MonitorReport
	// ���  : ������ 2013-11-25 14:17:19
	// ����  : void 
	// ����  : S_BPC_RSMSG &rcvdata
	// ����  : ���BPU���ñ���
	void MonitorReport(S_BPC_RSMSG &rcvdata);

	// ������: MonitorBpu
	// ���  : ������ 2013-11-25 14:17:01
	// ����  : void 
	// ����  : S_BPC_RSMSG &rcvdata
	// ����  : ���BPU��Ϣ
	void MonitorBpu(S_BPC_RSMSG &rcvdata);

	// ������: MonitorHost
	// ���  : ������ 2013-11-25 14:16:44
	// ����  : void 
	// ����  : S_BPC_RSMSG &rcvdata
	// ����  : ���������Ϣ
	void MonitorHost(S_BPC_RSMSG &rcvdata);

	// ������: MonitorInfo
	// ���  : ������ 2013-11-25 14:16:20
	// ����  : void 
	// ����  : S_BPC_RSMSG &rcvdata
	// ����  : ���BPC��Ϣ
	void MonitorInfo(S_BPC_RSMSG &rcvdata);

	void LogDrebHead(int loglevel,DREB_HEAD head,const char *msg,const char *filename,int fileline);
	
	// ������: UnzipBuf
	// ���  : ������ 2013-5-2 16:07:55
	// ����  : bool 
	// ����  : BPCCOMMSTRU &data
	// ����  : ��cZipΪ1-4�Ľ��н��
	bool UnzipBuf(BPCCOMMSTRU &data);

	// ������: GetBpcNext
	// ���  : ������ 2013-4-8 15:09:37
	// ����  : void 
	// ����  : S_BPC_RSMSG &rcvdata
	// ����  : ȡ������
	void GetBpcNext(S_BPC_RSMSG &rcvdata);

	

	// ������: RegisterDreb
	// ���  : ������ 2013-4-8 15:10:22
	// ����  : void 
	// ����  : int index
	// ����  : �������Ϸ�ע����Ϣ
	void RegisterDreb(int index);

	// ������: CallBpcLocalFunc
	// ���  : ������ 2010-8-13 14:51:54
	// ����  : void 
	// ����  : S_BPC_RSMSG &rcvdata
	// ����  : ����BPC�ڲ�����
	void CallBpcLocalFunc(S_BPC_RSMSG &rcvdata);

	// ������: ProcessDreb
	// ���  : ������ 2010-8-13 14:54:46
	// ����  : void 
	// ����  : S_BPC_RSMSG &rcvdata
	// ����  : ����DREB����������
	void ProcessDreb(S_BPC_RSMSG &rcvdata);
	CLinkThread();
	virtual ~CLinkThread();
	
	// ������: ResetData
	// ���  : ������ 2010-3-12 15:59:52
	// ����  : bool 
	// ����  : int index
	// ����  : S_FPC_RSMSG *data
	// ����  : ��������
	bool ResetData(int index,S_BPC_RSMSG *data);

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

	// ������: OnConnect
	// ���  : ������ 2010-8-13 14:56:53
	// ����  : void 
	// ����  : int conindex
	// ����  : ����dreb
	void OnConnect(int conindex);

	// ������: OnConnected
	// ���  : ������ 2010-8-13 14:57:06
	// ����  : void 
	// ����  : int conindex
	// ����  : ��������dreb,����ע����Ϣ
	void OnConnected(int conindex);

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
	// ����  : �������ݲ�����
	void OnRecv(int conindex);

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

	// ������: Run
	// ���  : ������ 2010-3-12 13:00:15
	// ����  : virtual int 
	// ����  : ���շ����߳�������
	virtual int Run();
	virtual bool InitThreadInstance();
	virtual void ExitThreadInstance();


	// ������: SetGlobalVar
	// ���  : ������ 2010-8-13 14:57:46
	// ����  : bool 
	// ����  : CGResource *res
	// ����  : CPoolData *pooldata
	// ����  : CPoolBuffer *mempool
	// ����  : CSocketMgr *sockmgr
	// ����  : CFuncTbl *tbl
	// ����  : ����ȫ�ֲ���
	bool SetGlobalVar(CGResource *res,CPoolDataMsg *pooldata,CBF_BufferPool *mempool,CSocketMgr *sockmgr,CFuncTbl *tbl);

	int m_nEnd;     //������������ţ������������
	int m_nBegin;  //��ʼ���������
	int m_nScanBegin;

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
	
	// ������: OnTimer
	// ���  : ������ 2013-3-6 16:43:23
	// ����  : static int 
	// ����  : unsigned int event
	// ����  : void *p
	// ����  : ��ʱ���ص�����
	static int OnTimer(unsigned int eventid,void *p);

	// ������: PingTimer
	// ���  : ������ 2013-6-24 11:34:04
	// ����  : void 
	// ����  : ��ʱ�����������
	void PingTimer();

protected:
	CIErrlog *m_pLog;

	CBF_Des        m_pDes;

	CBF_Timer m_pTimer;//��ʱ��
	bool      m_bIsPingTimer;
	
	CGResource  *m_pRes;
	CSocketMgr  *m_pSockMgr;
	CBF_BufferPool *m_pMemPool;//�ڴ�����;
	CPoolDataMsg   *m_pPoolData;//�������ݶ���
	CFuncTbl    *m_pFuncTbl;//���ܱ�
	CDrebEndian  m_pDrebEndian;
	S_BPC_RSMSG  m_pDataBuf;//�������ݽṹ
	S_BPC_RSMSG  m_pDataRcv;//�������ݽṹ
	CBF_SocketTcp  m_tcpSocket;
	
	CBF_ProcessMgr  m_pMgrBpu;    //����bpu

	bool m_bIsInit;
	char m_errMsg[256];
	bool m_bIsRunning;
	
	fd_set m_rset;
	fd_set m_wset;
#if defined(_WINDOWS)
	fd_set m_eset;
#endif
	char  m_headMsg[8192];

};

#endif // !defined(AFX_LINKTHREAD_H__B0F78DC0_7B0B_4189_AE4D_6111A689B1A7__INCLUDED_)
