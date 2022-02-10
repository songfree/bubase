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
