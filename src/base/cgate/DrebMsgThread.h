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
#include "GateResource.h"
#include "MdbQuotation.h"

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

    CGateResource *g_pGateRes;
protected:
    CMdbQuotation m_pQuotation;//������µ��������

	CSendData* m_pSendData;//���׶���   ���߹����ģ�Ҫ�����ͻ��˵���Ϣ������˶���
    CPoolData* m_pPoolData;//�������ݶ���	  �����ؼ��ָ��ʱ������Ϣ����˶��У�����Ϣ�߳�ȡ�˶���ȡ����������
    CAIO_Work* m_pAio;	   //aio�Ĺ������߳��࣬�ɻ�ȡaio����Ϣ�����ָ��ɵ��ô����ȡ��
	virtual int Run();

    // ������: ProcessDrebData
    // ���  : ������ 2015-4-23 15:53:34
    // ����  : virtual void 
    // ����  : S_BPC_RSMSG &rcvdata
    // ����  : ��������߽��չ���������	 �����spi�ص�����������ǻص�����ڣ�api�յ����ݺ����ô˽ӿ�
    virtual void ProcessDrebData(S_BPC_RSMSG& rcvdata)
    {
        if (rcvdata.sMsgBuf->sDBHead.cZip != 0)
        {
            //���������ѹ���Ļ���ֱ�ӷ��أ���cgateapiȥ��ѹ��
            if (!UnzipBuf(*rcvdata.sMsgBuf))
            {
                sprintf(rcvdata.sMsgBuf->sBuffer, "���ݽ�ѹ������");
                rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
                rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_DATA_ENCRYPTZIP;
                rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
                rcvdata.sMsgBuf->sDBHead.cZip = 0;
                m_pDrebApi->SendMsg(rcvdata);
                return;
            }
        }
        switch (rcvdata.sMsgBuf->sBpcHead.cMsgType)
        {
        case MSG_GETBPCNEXT://��bpc��ȡ������
            OnGetBpcNextPack(rcvdata);
            break;
        case MSG_GETNEXT://��BPU��ȡ��������Ҫ����BPU����
            OnGetBpuNextPack(rcvdata);
            break;
        case MSG_REQ://DREB���յ�������
            OnMsgRequest(rcvdata);
            break;
        case MSG_MONITOR_SAP://DREB���յ��ļ��ָ��
            OnMsgMonitor(rcvdata);
            break;
        case MSG_CONNECTDREB://DREB�������ӳɹ����������������ע�ύ�׵�
            OnMsgConnectBack(rcvdata);
            break;
        case MSG_DREBANS://DREB��������ȵ�Ӧ��
            OnMsgDrebAns(rcvdata);
            break;
        case MSG_BPCMONITOR://�����ϱ������Ϣ
            OnMsgReportBpc(rcvdata);
            break;
        default:
            m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
            rcvdata.sMsgBuf = NULL;
            break;
        }
        return;
    }

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

	virtual void OnMsgReportBpc(S_BPC_RSMSG& rcvdata);

	bool UnzipBuf(BPCCOMMSTRU& data);

	CBF_Des        m_pDes;  //DES��


};

#endif // !defined(AFX_DREBMSGTHREAD_H__52F83741_0772_4053_96E3_452305F6E404__INCLUDED_)
