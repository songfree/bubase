// DrebMsgThread.h: interface for the CDrebMsgThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DREBMSGTHREAD_H__52F83741_0772_4053_96E3_452305F6E404__INCLUDED_)
#define AFX_DREBMSGTHREAD_H__52F83741_0772_4053_96E3_452305F6E404__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DrebMsgProcBase.h"
#include "Xdp.h"
#include "BF_Timer.h"

class CDrebMsgThread : public CDrebMsgProcBase  
{
public:
	void SendQueryHostStatus();
	void PrintHostStatus();
	void OnArbQuery(S_BPC_RSMSG &rcvdata);
	void OnArbControl(S_BPC_RSMSG &rcvdata);
	void OnSvrRequest(S_BPC_RSMSG &rcvdata);
	void OnChangeHostStatus(unsigned int status);
	void OnQueryArbAns(S_BPC_RSMSG &rcvdata);
	void Send2Arb();

	bool  m_bIsQueryAns;
	
	CDrebMsgThread();
	virtual ~CDrebMsgThread();

	
	CXdp      m_pXdp;
	CXdp      m_pSendXdp;
	CBF_Timer m_pTime;
	bool      m_bIsCheck;
	CIErrlog  *m_pLog;

	virtual  bool Init(CBF_DrebServer   *bus,CBF_DrebResource *res);

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

	virtual int Run();
	
	

	// ������: OnMsgRequest
	// ���  : ������ 2015-4-23 15:54:38
	// ����  : virtual void 
	// ����  : S_BPC_RSMSG &rcvdata
	// ����  : �������߹�������������
	virtual void OnMsgRequest(S_BPC_RSMSG &rcvdata);

	

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

	


};

#endif // !defined(AFX_DREBMSGTHREAD_H__52F83741_0772_4053_96E3_452305F6E404__INCLUDED_)
