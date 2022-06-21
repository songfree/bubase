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

	
	std::vector<int> *m_lBcRegister;//要向总线订阅的广播功能号
protected:
	CSendData* m_pSendData;//交易队列   总线过来的，要发给客户端的消息，放入此队列
    CPoolData* m_pPoolData;//处理数据队列	  若重载监控指令时，将消息放入此队列，由消息线程取此队列取出发到总线
    CAIO_Work* m_pAio;	   //aio的工作线线程类，可获取aio的信息，监控指令可调用此类获取。
	virtual int Run();

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

};

#endif // !defined(AFX_DREBMSGTHREAD_H__52F83741_0772_4053_96E3_452305F6E404__INCLUDED_)
