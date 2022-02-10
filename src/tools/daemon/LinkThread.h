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
#include "BF_SimpleLog.h"
#include "BF_SocketTcp.h"
#include "BF_Timer.h"
#include "BF_Des.h"
#include "BF_Tools.h"
#include "BF_HostInfo.h"
#include "DrebEndian.h"
#include "SocketMgr.h"
#include "MoniHead.h"
#include "KvData.h"
#include "BF_ProcessMgr.h"

class CLinkThread : public CBF_Thread  
{
public:
	void OnCheckProcess();
	void WriteFile();
	int GetDrebIndex();

	// ������: GetHostInfo
	// ���  : ������ 2013-12-10 12:53:43
	// ����  : void 
	// ����  : ȡ������Դ��Ϣ
	void GetHostInfo();


	// ������: Upload
	// ���  : ������ 2013-12-10 12:53:35
	// ����  : void 
	// ����  : S_GWDREB_RSMSG &rcvdata
	// ����  : �ϴ��ļ�
	void Upload(S_GWDREB_RSMSG &rcvdata);

	// ������: Download
	// ���  : ������ 2013-12-10 12:53:27
	// ����  : void 
	// ����  : S_GWDREB_RSMSG &rcvdata
	// ����  : �����ļ�
	void Download(S_GWDREB_RSMSG &rcvdata);


	// ������: MonitorBpu
	// ���  : ������ 2013-12-10 12:53:13
	// ����  : void 
	// ����  : S_GWDREB_RSMSG &rcvdata
	// ����  : bpu��Ϣ
	void MonitorBpu(S_GWDREB_RSMSG &rcvdata);

	// ������: MonitorReport
	// ���  : ������ 2013-12-10 12:53:05
	// ����  : void 
	// ����  : S_GWDREB_RSMSG &rcvdata
	// ����  : ����
	void MonitorReport(S_GWDREB_RSMSG &rcvdata);

	// ������: MonitorHost
	// ���  : ������ 2013-12-10 12:52:56
	// ����  : void 
	// ����  : S_GWDREB_RSMSG &rcvdata
	// ����  : ������Ϣ
	void MonitorHost(S_GWDREB_RSMSG &rcvdata);

	// ������: MonitorInfo
	// ���  : ������ 2013-12-10 12:52:47
	// ����  : void 
	// ����  : S_GWDREB_RSMSG &rcvdata
	// ����  : ��Ϣ
	void MonitorInfo(S_GWDREB_RSMSG &rcvdata);

	// ������: MonitorStartBpu
	// ���  : ������ 2013-11-25 14:18:41
	// ����  : void 
	// ����  : S_BPC_RSMSG &rcvdata
	// ����  : �������BPU
	void MonitorStartBpu(S_GWDREB_RSMSG &rcvdata);
	
	// ������: MonitorStopBpu
	// ���  : ������ 2013-11-25 14:18:30
	// ����  : void 
	// ����  : S_BPC_RSMSG &rcvdata
	// ����  : ���ֹͣBPU
	void MonitorStopBpu(S_GWDREB_RSMSG &rcvdata);

	// ������: OnMonitor
	// ���  : ������ 2013-12-10 12:52:28
	// ����  : void 
	// ����  : �������ͼ����Ϣ
	void OnMonitor();

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
	void CallBpcLocalFunc(S_GWDREB_RSMSG &rcvdata);

	// ������: ProcessDreb
	// ���  : ������ 2010-8-13 14:54:46
	// ����  : void 
	// ����  : S_GWDREB_RSMSG &rcvdata
	// ����  : ����DREB����������
	void ProcessDreb(S_GWDREB_RSMSG &rcvdata);
	CLinkThread();
	virtual ~CLinkThread();


	
	// ������: ResetData
	// ���  : ������ 2010-3-12 15:59:52
	// ����  : bool 
	// ����  : int index
	// ����  : S_GWDREB_RSMSG *data
	// ����  : ��������
	bool ResetData(int index,S_GWDREB_RSMSG *data);


	void OnAccept(int conindex);

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
	// ����  : CResource *res
	// ����  : CSocketMgr *sockmgr
	// ����  : ����ȫ�ֲ���
	bool SetGlobalVar(CResource *res,CSocketMgr *sockmgr);


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
	
	CBF_SimpleLog m_pLog;
	CBF_Des       m_pDes;
	CResource  *m_pRes;
	CSocketMgr  *m_pSockMgr;
	CDrebEndian   m_pDrebEndian;

	CBF_HostInfo      m_pHost;
	CBF_ProcessMgr   m_pProcessMgr;
	
	S_GWDREB_RSMSG  m_pDataBuf;//�������ݽṹ
	S_GWDREB_RSMSG  m_pDataRcv;//�������ݽṹ
	CBF_SocketTcp   m_tcpSocket;
	CKvData         m_pKvList;
	
	bool m_bIsInit;
	char m_errMsg[256];
	bool m_bIsRunning;
	
	fd_set m_rset;
	fd_set m_wset;
#if defined(_WINDOWS)
	fd_set m_eset;
#endif
	char  m_headMsg[8192];
	char  m_sHeartBuffer[65535];
	S_MONITOR_HOST   m_sHostInfo;
	V_DiskInfo       m_vdinfo;

	unsigned int    m_nDrebIndex;

};

#endif // !defined(AFX_LINKTHREAD_H__B0F78DC0_7B0B_4189_AE4D_6111A689B1A7__INCLUDED_)
