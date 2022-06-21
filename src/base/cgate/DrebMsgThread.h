// DrebMsgThread.h: interface for the CDrebMsgThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DREBMSGTHREAD_H__52F83741_0772_4053_96E3_452305F6E404__INCLUDED_)
#define AFX_DREBMSGTHREAD_H__52F83741_0772_4053_96E3_452305F6E404__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DrebMsgProcBase.h"
#include "SendData.h"
#include "AIO_Work.h"
#include "PoolData.h"

class CDrebMsgThread : public CDrebMsgProcBase  
{
public:
	
	
	CDrebMsgThread();
	virtual ~CDrebMsgThread();

	CIErrlog  *m_pLog;

	virtual  bool Init(CIBF_DrebServer* bus,CBF_DrebResource *res);
	bool SetGlobalVar(CPoolData* pooldata, CSendData* senddata, CAIO_Work* aio);

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
	CSendData* m_pSendData;//���׶���   ���߹����ģ�Ҫ�����ͻ��˵���Ϣ������˶���
    CPoolData* m_pPoolData;//�������ݶ���	  �����ؼ��ָ��ʱ������Ϣ����˶��У�����Ϣ�߳�ȡ�˶���ȡ����������
    CAIO_Work* m_pAio;	   //aio�Ĺ������߳��࣬�ɻ�ȡaio����Ϣ�����ָ��ɵ��ô����ȡ��
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

};

#endif // !defined(AFX_DREBMSGTHREAD_H__52F83741_0772_4053_96E3_452305F6E404__INCLUDED_)
