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

	// 函数名: Start
	// 编程  : 王明松 2015-4-23 15:52:29
	// 返回  : virtual bool 
	// 描述  : 启动处理线程
	virtual bool Start();
	
	// 函数名: Stop
	// 编程  : 王明松 2015-4-23 15:52:43
	// 返回  : virtual void 
	// 描述  : 停止处理线程
	virtual void Stop();

    CGateResource *g_pGateRes;
protected:
    CMdbQuotation m_pQuotation;//存放最新的行情序号

	CSendData* m_pSendData;//交易队列   总线过来的，要发给客户端的消息，放入此队列
    CPoolData* m_pPoolData;//处理数据队列	  若重载监控指令时，将消息放入此队列，由消息线程取此队列取出发到总线
    CAIO_Work* m_pAio;	   //aio的工作线线程类，可获取aio的信息，监控指令可调用此类获取。
	virtual int Run();

    // 函数名: ProcessDrebData
    // 编程  : 王明松 2015-4-23 15:53:34
    // 返回  : virtual void 
    // 参数  : S_BPC_RSMSG &rcvdata
    // 描述  : 处理从总线接收过来的数据	 如果用spi回调，则这里就是回调的入口，api收到数据后会调用此接口
    virtual void ProcessDrebData(S_BPC_RSMSG& rcvdata)
    {
        if (rcvdata.sMsgBuf->sDBHead.cZip != 0)
        {
            //这里如果是压缩的话，直接返回，由cgateapi去解压缩
            if (!UnzipBuf(*rcvdata.sMsgBuf))
            {
                sprintf(rcvdata.sMsgBuf->sBuffer, "数据解压缩出错");
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
        case MSG_GETBPCNEXT://在bpc上取后续包
            OnGetBpcNextPack(rcvdata);
            break;
        case MSG_GETNEXT://在BPU上取后续包，要发给BPU处理
            OnGetBpuNextPack(rcvdata);
            break;
        case MSG_REQ://DREB上收到的请求
            OnMsgRequest(rcvdata);
            break;
        case MSG_MONITOR_SAP://DREB上收到的监控指令
            OnMsgMonitor(rcvdata);
            break;
        case MSG_CONNECTDREB://DREB返回连接成功，可以在这里进行注册交易等
            OnMsgConnectBack(rcvdata);
            break;
        case MSG_DREBANS://DREB返回外调等的应答
            OnMsgDrebAns(rcvdata);
            break;
        case MSG_BPCMONITOR://主动上报监控信息
            OnMsgReportBpc(rcvdata);
            break;
        default:
            m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
            rcvdata.sMsgBuf = NULL;
            break;
        }
        return;
    }

	// 函数名: OnMsgConnectBack
	// 编程  : 王明松 2015-4-23 15:55:29
	// 返回  : virtual void 
	// 参数  : S_BPC_RSMSG &rcvdata
	// 描述  : 处理连接总线成功的响应
	virtual void OnMsgConnectBack(S_BPC_RSMSG &rcvdata);

	// 函数名: OnMsgDrebAns
	// 编程  : 王明松 2015-4-23 15:55:58
	// 返回  : virtual void 
	// 参数  : S_BPC_RSMSG &rcvdata
	// 描述  : 响应总线过来的应答数据
	virtual void OnMsgDrebAns(S_BPC_RSMSG &rcvdata);

    // 函数名: OnMsgDrebAns
    // 编程  : 王明松 2015-4-23 15:55:58
    // 返回  : virtual void 
    // 参数  : S_BPC_RSMSG &rcvdata
    // 描述  : 处理总线过来的数据，广播，推送等
	virtual void OnMsgRequest(S_BPC_RSMSG& rcvdata);

	virtual void OnMsgReportBpc(S_BPC_RSMSG& rcvdata);

	bool UnzipBuf(BPCCOMMSTRU& data);

	CBF_Des        m_pDes;  //DES类


};

#endif // !defined(AFX_DREBMSGTHREAD_H__52F83741_0772_4053_96E3_452305F6E404__INCLUDED_)
