// PuThread.h: interface for the CPuThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PUTHREAD_H__A9111D1A_9E48_48A4_9CB9_59AF93F9B6B7__INCLUDED_)
#define AFX_PUTHREAD_H__A9111D1A_9E48_48A4_9CB9_59AF93F9B6B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "public.h"
#include "BF_Thread.h"
#include "BF_SimpleLog.h"
#include "BF_SocketTcp.h"
#include "GResource.h"
#include "SocketMgr.h"
#include "BF_BufferPool.h"
#include "PoolData.h"
#include "FuncTbl.h"
#include "HostInfo.h"
#include "BpcEndian.h"
#include "DrebEndian.h"
#include "BF_Timer.h"

//���߳̽���bpu���ӵ����ݣ����д�������������ڵ�����msgthread�߳�������ַ�


class CPuThread : public CBF_Thread  
{
public:
	CPuThread();
	virtual ~CPuThread();
public:

	// ������: BpcNext
	// ���  : ������ 2013-5-15 15:27:58
	// ����  : void 
	// ����  : S_BPC_RSMSG &rcvdata
	// ����  : ȡ������
	void BpcNext(S_BPC_RSMSG &rcvdata);

	// ������: DispatchReq
	// ���  : ������ 2013-5-15 15:27:53
	// ����  : void 
	// ����  : S_BPC_RSMSG &rcvdata
	// ����  : ���������Ӧ��
	void DispatchReq(S_BPC_RSMSG &rcvdata);

	// ������: DispatchGetNext
	// ���  : ������ 2013-5-15 15:27:48
	// ����  : void 
	// ����  : S_BPC_RSMSG &rcvdata
	// ����  : ����ȡ��������Ӧ��
	void DispatchGetNext(S_BPC_RSMSG &rcvdata);

	// ������: DispatchTrans
	// ���  : ������ 2013-5-15 15:27:43
	// ����  : void 
	// ����  : S_BPC_RSMSG &rcvdata
	// ����  : ������Ϣ����
	void DispatchTrans(S_BPC_RSMSG &rcvdata);

	void LogDrebHead(int loglevel, DREB_HEAD head, const char *msg, const char *filename, int fileline);

	// ������: GetBpcMsgType
	// ���  : ������ 2013-4-9 10:29:32
	// ����  : std::string 
	// ����  : int msgtype
	// ����  : ȡBPC��Ϣ����˵��
	std::string GetBpcMsgType(int msgtype);

	// ������: GetDrebCmdType
	// ���  : ������ 2013-4-9 10:29:35
	// ����  : std::string 
	// ����  : int cmdtype
	// ����  : ȡdreb��������˵��
	std::string GetDrebCmdType(int cmdtype);


	// ������: DispatchExtCall
	// ���  : ������ 2010-8-13 14:52:22
	// ����  : void 
	// ����  : S_BPC_RSMSG &rcvdata
	// ����  : ����BPU���
	void DispatchExtCall(S_BPC_RSMSG &rcvdata);

	// ������: DispatchBpcCall
	// ���  : ������ 2010-8-13 14:52:32
	// ����  : void 
	// ����  : S_BPC_RSMSG &rcvdata
	// ����  : ����BPU���ñ�BPC����
	void DispatchBpcCall(S_BPC_RSMSG &rcvdata);


	// ������: DispatchBpuIsReg
	// ���  : ������ 2010-8-13 14:53:06
	// ����  : void 
	// ����  : S_BPC_RSMSG &rcvdata
	// ����  : ��ӦBPU�����Ƿ���Ҫע��
	void DispatchBpuIsReg(S_BPC_RSMSG &rcvdata);

	// ������: DispatchSetPara3
	// ���  : ������ 2010-8-13 14:54:03
	// ����  : void 
	// ����  : S_BPC_RSMSG &rcvdata
	// ����  : ����BPU���ò���3
	void DispatchSetPara3(S_BPC_RSMSG &rcvdata);

	// ������: DispatchSetPara2
	// ���  : ������ 2010-8-13 14:54:25
	// ����  : void 
	// ����  : S_BPC_RSMSG &rcvdata
	// ����  : ����BPU���ò���2
	void DispatchSetPara2(S_BPC_RSMSG &rcvdata);

	// ������: DispatchSetPara1
	// ���  : ������ 2010-8-13 14:54:32
	// ����  : void 
	// ����  : S_BPC_RSMSG &rcvdata
	// ����  : ����BPU���ò���1
	void DispatchSetPara1(S_BPC_RSMSG &rcvdata);

	// ������: GetSerial
	// ���  : ������ 2010-8-13 14:55:01
	// ����  : void 
	// ����  : S_BPC_RSMSG &rcvdata
	// ����  : ����BPUȡΨһ��ˮ
	void GetSerial(S_BPC_RSMSG &rcvdata);

	// ������: DispatchBuFree
	// ���  : ������ 2010-8-13 14:55:59
	// ����  : void 
	// ����  : S_BPC_RSMSG &rcvdata
	// ����  : ����bpu���Խ�����������
	void DispatchBuFree(S_BPC_RSMSG &rcvdata);

	// ������: ProcessBpu
	// ���  : ������ 2010-8-13 14:56:38
	// ����  : void 
	// ����  : S_BPC_RSMSG &rcvdata
	// ����  : ����BPU����������
	void ProcessBpu(S_BPC_RSMSG &rcvdata);
	
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

	int m_nEnd; //������������ţ������������
	int m_nBegin;//��ʼ���������
	
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

	CBF_Timer m_pTimer;//��ʱ��
	bool      m_bIsPingTimer;

	CIErrlog      *m_pLog;
//	CHostInfo     m_pHostInfo;
	
	CGResource  *m_pRes;
	CSocketMgr  *m_pSockMgr;
	CBF_BufferPool *m_pMemPool;//�ڴ�����;
	CPoolDataMsg   *m_pPoolData;//�������ݶ���
	CFuncTbl    *m_pFuncTbl;//���ܱ�
	CBpcEndian   m_pBpcEndian;
	CDrebEndian  m_pDrebEndian;


	S_BPC_RSMSG  m_pDataBuf;//�������ݽṹ
	S_BPC_RSMSG  m_pDataRcv;//�������ݽṹ
	CBF_SocketTcp  m_tcpSocket;
	
	bool m_bIsInit;
	char m_errMsg[256];
	bool m_bIsRunning;
	
	fd_set m_rset;
	fd_set m_wset;
#if defined(_WINDOWS)
	fd_set m_eset;
#endif
	char m_headMsg[8192];

private:
	// ������: Run
	// ���  : ������ 2010-3-12 13:00:15
	// ����  : virtual int 
	// ����  : ���շ����߳�������
	virtual int Run();
	virtual bool InitThreadInstance();
	virtual void ExitThreadInstance();
};

#endif // !defined(AFX_PUTHREAD_H__A9111D1A_9E48_48A4_9CB9_59AF93F9B6B7__INCLUDED_)
