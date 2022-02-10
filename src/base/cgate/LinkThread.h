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
#include "IErrLog.h"
#include "BF_SocketTcp.h"
#include "GateResource.h"
#include "SocketMgr.h"
#include "BF_BufferPool.h"
#include "PoolData.h"
#include "DrebEndian.h"
#include "BF_Des.h"
#include "EncryptCommBuffer.h"
#include "BF_CommDataLog.h"
#include "BF_Timer.h"
#include "SendData.h"
#include "AIO_Work.h"
#include "MdbQuotation.h"

class CLinkThread : public CBF_Thread  
{
public:

	// ������: ProcessQuota
	// ���  : ������ 2014-7-1 12:21:33
	// ����  : void 
	// ����  : S_GATEDREB_RSMSG &rcvdata
	// ����  : S_CGATE_SMSG &data
	// ����  : ��������
	void ProcessQuota(S_GATEDREB_RSMSG &rcvdata,S_CGATE_SMSG &data);

	void Upload(S_GATEDREB_RSMSG &rcvdata);
	void Download(S_GATEDREB_RSMSG &rcvdata);

	void MonitorBpu(S_GATEDREB_RSMSG &rcvdata);

	
	void MonitorReport(S_GATEDREB_RSMSG &rcvdata);
	void MonitorHost(S_GATEDREB_RSMSG &rcvdata);
	void MonitorInfo(S_GATEDREB_RSMSG &rcvdata);
	void OnMonitor();

	void LogDrebHead(int loglevel,DREB_HEAD head,const char *msg,const char *filename,int fileline);

	

	// ������: GetBpcMsgType
	// ���  : ������ 2013-4-8 15:09:48
	// ����  : std::string 
	// ����  : int msgtype
	// ����  : ȡBPC��Ϣ����˵��
	std::string GetBpcMsgType(int msgtype);

	// ������: GetDrebCmdType
	// ���  : ������ 2013-4-8 15:09:53
	// ����  : std::string 
	// ����  : int cmdtype
	// ����  : ȡDREB��������˵��
	std::string GetDrebCmdType(int cmdtype);


	
	// ������: CallBpcLocalFunc
	// ���  : ������ 2010-8-13 14:51:54
	// ����  : void 
	// ����  : S_BPC_RSMSG &rcvdata
	// ����  : ����BPC�ڲ�����
	void CallBpcLocalFunc(S_GATEDREB_RSMSG &rcvdata);

	// ������: ProcessDreb
	// ���  : ������ 2010-8-13 14:54:46
	// ����  : void 
	// ����  : S_BPC_RSMSG &rcvdata
	// ����  : ����DREB����������
	void ProcessDreb(S_GATEDREB_RSMSG &rcvdata);
	CLinkThread();
	virtual ~CLinkThread();


	
	// ������: ResetData
	// ���  : ������ 2010-3-12 15:59:52
	// ����  : bool 
	// ����  : int index
	// ����  : S_FPC_RSMSG *data
	// ����  : ��������
	bool ResetData(int index,S_GATEDREB_RSMSG *data);


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
	virtual bool Terminate();


	// ������: SetGlobalVar
	// ���  : ������ 2010-8-13 14:57:46
	// ����  : bool 
	// ����  : CGResource *res
	// ����  : CPoolData *pooldata
	// ����  : CPoolBuffer *mempool
	// ����  : CSocketMgr *sockmgr
	// ����  : ����ȫ�ֲ���
	bool SetGlobalVar(CGateResource *res,CPoolData *pooldata,CBF_BufferPool *mempool,CSocketMgr *sockmgr,CSendData *senddata,CAIO_Work *aio);

	int m_nEnd;     //������������ţ������������
	int m_nBegin;  //��ʼ���������
	int m_nScanBegin;

	// ������: UnzipBuf
	// ���  : ������ 2013-5-2 16:07:55
	// ����  : bool 
	// ����  : PCOMMSTRU data
	// ����  : ��cZipΪ1-4�Ľ��н��
	bool UnzipBuf(PCOMMSTRU data);


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
	CBF_Timer m_pTimer;//��ʱ��
	bool      m_bIsPingTimer;

	CBF_CommDataLog  *m_pDrebLog;
	CIErrlog         *m_pLog;
	CBF_Des       m_pDes;
	CSendData   *m_pSendData;//���׶���

	

	CEncryptCommBuffer m_pDesZip;
	
	CGateResource  *m_pRes;
	
	CSocketMgr  *m_pSockMgr;
	CBF_BufferPool *m_pMemPool;//�ڴ�����;
	CPoolData   *m_pPoolData;//�������ݶ���
	CAIO_Work   *m_pAio;
	
	CDrebEndian  m_pDrebEndian;
	S_GATEDREB_RSMSG  m_pDataBuf;//�������ݽṹ
	S_GATEDREB_RSMSG  m_pDataRcv;//�������ݽṹ
	CBF_SocketTcp  m_tcpSocket;

	CMdbQuotation  m_pQuotation;
	
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
