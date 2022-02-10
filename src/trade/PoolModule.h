// PoolModule.h: interface for the CPoolModule class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POOLMODULE_H__DB9F06B7_888C_42E9_9755_AB2847BE09C0__INCLUDED_)
#define AFX_POOLMODULE_H__DB9F06B7_888C_42E9_9755_AB2847BE09C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "BF_Thread.h"
#include "IErrLog.h"
#include "BF_SocketTcp.h"
#include "TradeResource.h"
#include "SocketMgr.h"
#include "BF_BufferPool.h"
#include "PoolData.h"
#include "BF_Timer.h"
#include "BpcHead.h"
#include "SendQueue.h"



class  CPoolModule : public CBF_Thread  
{
public:
	
	CPoolModule();
	virtual ~CPoolModule();


	// 函数名: Run
	// 编程  : 王明松 2010-3-12 13:00:15
	// 返回  : virtual int 
	// 描述  : 接收发送线程主方法
	virtual int Run();
	virtual bool Terminate();

	CIErrlog   *m_log;
	CTradeResource  *m_pRes;
	CSocketMgr  m_pSockMgr;
	CBF_BufferPool *m_pMemPool;//内存分配池;
	CPoolDataMsg   *m_pRcvQueue;//接收队列
	
	
	
	// 函数名: OnTimer
	// 编程  : 王明松 2013-3-6 16:43:23
	// 返回  : static int 
	// 参数  : unsigned int event
	// 参数  : void *p
	// 描述  : 定时器回调方法
	static int OnTimer(unsigned int eventid,void *p);
	void PingTimer();

public:

	// 函数名: StartFront
	// 编程  : 王明松 2016-3-23 14:14:02
	// 返回  : bool 
	// 描述  : 启动select线程
	bool StartFront();


	void Monitor();


	// 函数名: StopFront
	// 编程  : 王明松 2010-3-12 12:59:33
	// 返回  : void 
	// 描述  : 停止select
	void StopFront();

	// 函数名: InitFront
	// 编程  : 王明松 2010-3-12 12:59:42
	// 返回  : bool 
	// 参数  : CTradeResource *res
	// 描述  : 初始并启动前置
	bool InitFront(CTradeResource *res,CPoolDataMsg *que,CIErrlog *log);


	int GetMaxfdAddSelect(int &maxfd);

	/// 函数名: OnWriteEvent
	/// 编程  : 王明松 2009-5-21 15:17:24
	/// 返回  : void 
	/// 参数  : int conindex
	/// 描述  : 响应select写事件
	void OnWriteEvent(int conindex);
	
	/// 函数名: OnReadEvent
	/// 编程  : 王明松 2009-5-21 15:17:03
	/// 返回  : void 
	/// 参数  : int conindex
	/// 描述  : 响应select读事件
	void OnReadEvent(int conindex);
	
	/// 函数名: OnAccept
	/// 编程  : 王明松 2009-5-21 15:16:41
	/// 返回  : void 
	/// 描述  : 接收连接
	void OnAccept(int conindex);
	
	/// 函数名: OnSend
	/// 编程  : 王明松 2009-5-21 15:16:33
	/// 返回  : void 
	/// 参数  : int conindex
	/// 描述  : 发送数据
	void OnSend(int conindex);

	// 函数名: OnRecv
	// 编程  : 王明松 2010-3-12 14:06:55
	// 返回  : void 
	// 参数  : int conindex
	// 描述  : 接收数据并放入队列
	void OnRecv(int conindex);
	
	
	
	/// 函数名: OnClose
	/// 编程  : 王明松 2009-5-21 13:18:27
	/// 返回  : void 
	/// 参数  : int conindex 连接的下标
	/// 参数  : const char *msg 关闭连接原因
	/// 描述  : 关闭客户端或服务端连接
	void OnClose(int conindex,const char *msg);
	
	/// 函数名: OnPing
	/// 编程  : 王明松 2009-5-21 13:17:22
	/// 返回  : void 
	/// 参数  : int conindex 连接的下标
	/// 描述  : 发送心跳
	void OnPing(int conindex);
	
	
	/// 函数名: OnEvent
	/// 编程  : 王明松 2009-5-21 13:16:19
	/// 返回  : void 
	/// 描述  : select后有事件的处理,如读数据、发数据
	void OnEvent();
	
	/// 函数名: OnNoEvent
	/// 编程  : 王明松 2009-5-21 13:16:35
	/// 返回  : void 
	/// 描述  : select后无事件的处理，如心跳
	void OnNoEvent();

	bool ResetData(int index,S_TRADE_DATA *data);
protected:
	S_TRADE_DATA   m_pDataRcv;

	CBF_SocketTcp  m_tcpSocket;

	bool m_bIsInit;
	char m_errMsg[256];
	bool m_bIsRunning;
	
	fd_set m_rset;
	fd_set m_wset;
#if defined(_WINDOWS)
	fd_set m_eset;
#endif
	CBF_Timer m_pTimer;//定时器
	bool      m_bIsPingTimer;
};

#endif // !defined(AFX_POOLMODULE_H__DB9F06B7_888C_42E9_9755_AB2847BE09C0__INCLUDED_)
