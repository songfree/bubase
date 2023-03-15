// DrebMsgThread.h: interface for the CDrebMsgThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DREBMSGTHREAD_H__52F83741_0772_4053_96E3_452305F6E404__INCLUDED_)
#define AFX_DREBMSGTHREAD_H__52F83741_0772_4053_96E3_452305F6E404__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DrebMsgProcBase.h"
#include "BF_ProcessMgr.h"
#include "PoolData.h"
#include "FuncTbl.h"
#include "SocketMgr.h"

class CDrebMsgThread : public CDrebMsgProcBase  
{
public:
	
	
	CDrebMsgThread();
	virtual ~CDrebMsgThread();

	CIErrlog  *m_pLog;

	virtual  bool Init(CIBF_DrebServer* bus,CBF_DrebResource *res);
	bool SetGlobalVar(CPoolDataMsg* pooldata, CFuncTbl* tbl, CSocketMgr* sockmgr);
	
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

	
	std::vector<int> *m_lBcRegister;//Ҫ�����߶��ĵĹ㲥���ܺ�
protected:
	CBF_ProcessMgr  m_pMgrBpu;    //����bpu
    CPoolDataMsg* m_pPoolData;//�������ݶ���
    CFuncTbl* m_pFuncTbl;//���ܱ�
	CSocketMgr * m_pSockMgr;//bpu���ӹ���

	CBF_Des        m_pDes;
	virtual int Run();

    
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

    // ������: OnMsgDrebAns
    // ���  : ������ 2015-4-23 15:55:58
    // ����  : virtual void 
    // ����  : S_BPC_RSMSG &rcvdata
    // ����  : �������߹��������ݣ��㲥�����͵�
	virtual void OnMsgRequest(S_BPC_RSMSG& rcvdata);
	//ȡbpu������
	virtual void OnGetBpuNextPack(S_BPC_RSMSG& rcvdata);
    //ȡbpc������
    virtual void OnGetBpcNextPack(S_BPC_RSMSG& rcvdata);
	//bpc�����Ϣ
	virtual void MonitorInfo(S_BPC_RSMSG& rcvdata);
	//���bpu��Ϣ
	virtual void MonitorBpu(S_BPC_RSMSG& rcvdata);
	//������Ϣ
	virtual void MonitorReport(S_BPC_RSMSG& rcvdata);
	virtual void OnMsgReportBpc(S_BPC_RSMSG& rcvdata);

	//���ܽ�ѹ����
	virtual bool UnzipBuf(BPCCOMMSTRU& data);
	//����bpu����
	virtual void MonitorSetBpu(S_BPC_RSMSG& rcvdata);
	//����ģʽ
	virtual void MonitorSetMode(S_BPC_RSMSG& rcvdata);
	//ֹͣbpu
	virtual void MonitorStopBpu(S_BPC_RSMSG& rcvdata);
	//����bpu
	virtual void MonitorStartBpu(S_BPC_RSMSG& rcvdata);
};

#endif // !defined(AFX_DREBMSGTHREAD_H__52F83741_0772_4053_96E3_452305F6E404__INCLUDED_)
