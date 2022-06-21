// DrebMsgThread.h: interface for the CDrebMsgThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DREBMSGTHREAD_H__52F83741_0772_4053_96E3_452305F6E404__INCLUDED_)
#define AFX_DREBMSGTHREAD_H__52F83741_0772_4053_96E3_452305F6E404__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DrebMsgProcBase.h"
#include "ArbTbl.h"
#include "Xdp.h"
#include "BF_Timer.h"

class CDrebMsgThread : public CDrebMsgProcBase  
{
public:
	void SetSwitchTime(std::vector<ARB_S_APPINFO *> &appinfo);

	// ������: SwitchHost
	// ���  : ������ 2015-12-30 16:53:37
	// ����  : void 
	// ����  : unsigned short drebid
	// ����  : unsigned int svr  ���������
	// ����  : char m2b          ����ת����˽�з����
	// ����  : char b2m          �ɱ�ת����˽�з����
	// ����  : �����л�
	void SwitchHost(unsigned short drebid,unsigned int svr, char m2b,char b2m);

	// ������: SendQueryHostStatus
	// ���  : ������ 2015-12-30 16:53:23
	// ����  : void 
	// ����  : unsigned int svr ���������
	// ����  : char pid         ˽�з����
	// ����  : ����Ӧ������״̬��ѯ
	void SendQueryHostStatus(unsigned int svr,char pid);

	// ������: CheckArbByApp
	// ���  : ������ 2015-12-30 15:21:15
	// ����  : void 
	// ����  : std::vector<ARB_S_APPINFO *> appinfo
	// ����  : ���һ��Ӧ�õ�����������ѯ���л���
	void CheckArbByApp(std::vector<ARB_S_APPINFO *> appinfo);

	// ������: CheckArb
	// ���  : ������ 2015-12-29 11:16:40
	// ����  : void 
	// ����  : ���Ӧ��
	void CheckArb();

	// ������: OnCheck
	// ���  : ������ 2015-12-29 11:16:53
	// ����  : void 
	// ����  : �ü��Ӧ�ñ�־
	void OnCheck();
	
	CDrebMsgThread();
	virtual ~CDrebMsgThread();

	CArbTbl   m_tArb;
	CXdp      m_pXdp;
	CBF_Timer m_pTime;
	bool      m_bIsCheck;
	CIErrlog  *m_pLog;

	virtual  bool Init(CIBF_DrebServer* bus,CBF_DrebResource *res);

	// ������: Start
	// ���  : ������ 2015-4-23 15:52:29
	// ����  : virtual bool 
	// ����  : ���������߳�
	virtual bool Start();
	
	// ������: Stop
	// ���  : ������ 2015-4-23 15:52:43
	// ����  : virtual void 
	// ����  : ֹͣ�����߳�
	virtual void Stop();
protected:
	static int OnTimer(unsigned int event,void *p);

	void OnAppPushStatus(S_BPC_RSMSG &rcvdata);

	void OnAppQueryHost(S_BPC_RSMSG &rcvdata);

	virtual int Run();
	
	// ������: ProcessDrebData
	// ���  : ������ 2015-4-23 15:53:34
	// ����  : virtual void 
	// ����  : S_BPC_RSMSG &rcvdata
	virtual void ProcessDrebData(S_BPC_RSMSG &rcvdata);

	// ������: OnGetBpcNextPack
	// ���  : ������ 2015-4-23 15:53:52
	// ����  : virtual void 
	// ����  : S_BPC_RSMSG &rcvdata
	// ����  : ȡ���صĺ�����,(���������ڱ���)
	virtual void OnGetBpcNextPack(S_BPC_RSMSG &rcvdata);

	// ������: OnGetBpuNextPack
	// ���  : ������ 2015-4-23 15:54:14
	// ����  : virtual void 
	// ����  : S_BPC_RSMSG &rcvdata
	// ����  : ȡBPU�ϵĺ�������Ҫ����BPU����
	virtual void OnGetBpuNextPack(S_BPC_RSMSG &rcvdata);

	// ������: OnMsgRequest
	// ���  : ������ 2015-4-23 15:54:38
	// ����  : virtual void 
	// ����  : S_BPC_RSMSG &rcvdata
	// ����  : �������߹�������������
	virtual void OnMsgRequest(S_BPC_RSMSG &rcvdata);

	// ������: OnMsgMonitor
	// ���  : ������ 2015-4-23 15:54:57
	// ����  : virtual void 
	// ����  : S_BPC_RSMSG &rcvdata
	// ����  : �������߹����ļ��ָ��
	virtual void OnMsgMonitor(S_BPC_RSMSG &rcvdata);

	// ������: OnMsgConnectBack
	// ���  : ������ 2015-4-23 15:55:29
	// ����  : virtual void 
	// ����  : S_BPC_RSMSG &rcvdata
	// ����  : �����������߳ɹ�����Ӧ
	virtual void OnMsgConnectBack(S_BPC_RSMSG &rcvdata);

	// ������: OnMsgDrebAns
	// ���  : ������ 2015-4-23 15:55:58
	// ����  : virtual void 
	// ����  : S_BPC_RSMSG &rcvdata
	// ����  : ��Ӧ���߹�����Ӧ������
	virtual void OnMsgDrebAns(S_BPC_RSMSG &rcvdata);

	// ������: OnMsgReportBpc
	// ���  : ������ 2015-4-23 16:01:43
	// ����  : virtual void 
	// ����  : S_BPC_RSMSG &rcvdata
	// ����  : �����ϱ������Ϣ����������
	virtual void OnMsgReportBpc(S_BPC_RSMSG &rcvdata);

	//����Ϊ��ع���
	virtual void MonitorInfo(S_BPC_RSMSG &rcvdata);

	virtual void MonitorHost(S_BPC_RSMSG &rcvdata);

	virtual void MonitorBpu(S_BPC_RSMSG &rcvdata);

	virtual void MonitorReport(S_BPC_RSMSG &rcvdata);


};

#endif // !defined(AFX_DREBMSGTHREAD_H__52F83741_0772_4053_96E3_452305F6E404__INCLUDED_)
