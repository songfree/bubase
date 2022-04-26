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
#include "PoolData.h"
#include "BF_DrebServer.h"

class CDrebMsgThread : public CDrebMsgProcBase  
{
public:
	void OnHostProc();
	// 函数名: SendQueryHostStatus
	// 编程  : 王明松 2016-3-29 11:40:59
	// 返回  : void 
	// 描述  : 发送查询本机主备的请求
	void SendQueryHostStatus();

	void PrintHostStatus();

	// 函数名: OnArbQuery
	// 编程  : 王明松 2016-3-29 11:41:24
	// 返回  : void 
	// 参数  : S_BPC_RSMSG &rcvdata
	// 描述  : 处理仲裁的查询请求
	void OnArbQuery(S_BPC_RSMSG &rcvdata);

	// 函数名: OnArbControl
	// 编程  : 王明松 2016-3-29 11:41:50
	// 返回  : void 
	// 参数  : S_BPC_RSMSG &rcvdata
	// 描述  : 处理仲裁的控制命令，即主备切换
	void OnArbControl(S_BPC_RSMSG &rcvdata);

	// 函数名: OnSvrRequest
	// 编程  : 王明松 2016-3-29 11:42:16
	// 返回  : void 
	// 参数  : S_BPC_RSMSG &rcvdata
	// 描述  : 处理请求
	void OnSvrRequest(S_BPC_RSMSG &rcvdata);

	// 函数名: OnChangeHostStatus
	// 编程  : 王明松 2016-3-29 11:42:31
	// 返回  : void 
	// 参数  : unsigned int status
	// 描述  : 本机状态变换
	void OnChangeHostStatus(unsigned int status);

	// 函数名: OnQueryArbAns
	// 编程  : 王明松 2016-3-29 11:42:53
	// 返回  : void 
	// 参数  : S_BPC_RSMSG &rcvdata
	// 描述  : 处理仲裁应答给本机查询状态
	void OnQueryArbAns(S_BPC_RSMSG &rcvdata);

	// 函数名: Send2Arb
	// 编程  : 王明松 2016-3-29 11:43:35
	// 返回  : void 
	// 描述  : 发送给仲裁
	void Send2Arb();

	bool  m_bIsQueryAns;
	
	CDrebMsgThread();
	virtual ~CDrebMsgThread();


	CPoolDataMsg   *m_pRcvQueue;//

	CXdp      m_pXdp;
	CXdp      m_pSendXdp;
	CBF_Timer m_pTime;
	bool      m_bIsCheck;
	CIErrlog  *m_pLog;
	CIClientLink *m_pAnsProc;
	

	virtual  bool Init(CIBF_DrebServer* bus,CBF_DrebResource *res);

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
protected:

	

	static int OnTimer(unsigned int event,void *p);


	virtual void OnMsgReportBpc(S_BPC_RSMSG &rcvdata);

	virtual int Run();
	
	

	// 函数名: OnMsgRequest
	// 编程  : 王明松 2015-4-23 15:54:38
	// 返回  : virtual void 
	// 参数  : S_BPC_RSMSG &rcvdata
	// 描述  : 处理总线过来的请求数据
	virtual void OnMsgRequest(S_BPC_RSMSG &rcvdata);

	

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

	


};

#endif // !defined(AFX_DREBMSGTHREAD_H__52F83741_0772_4053_96E3_452305F6E404__INCLUDED_)
