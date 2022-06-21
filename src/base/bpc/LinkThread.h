// LinkThread.h: interface for the CLinkThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LINKTHREAD_H__B0F78DC0_7B0B_4189_AE4D_6111A689B1A7__INCLUDED_)
#define AFX_LINKTHREAD_H__B0F78DC0_7B0B_4189_AE4D_6111A689B1A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "BF_Thread.h"
#include "BF_LogClient.h"
#include "BF_SocketTcp.h"
#include "GResource.h"
#include "SocketMgr.h"
#include "BF_BufferPool.h"
#include "PoolData.h"
#include "FuncTbl.h"
#include "DrebEndian.h"
#include "BF_Timer.h"
#include "BF_ProcessMgr.h"

class CLinkThread : public CBF_Thread  
{
public:
	void OnBpuIsReg(int index, S_BPC_RSMSG& rcvdata);

	CLinkThread();
	virtual ~CLinkThread();
	
	// 函数名: ResetData
	// 编程  : 王明松 2010-3-12 15:59:52
	// 返回  : bool 
	// 参数  : int index
	// 参数  : S_FPC_RSMSG *data
	// 描述  : 重置数据
	bool ResetData(int index,S_BPC_RSMSG *data);

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

	// 函数名: OnConnect
	// 编程  : 王明松 2010-8-13 14:56:53
	// 返回  : void 
	// 参数  : int conindex
	// 描述  : 连接dreb
	void OnConnect(int conindex);

	// 函数名: OnConnected
	// 编程  : 王明松 2010-8-13 14:57:06
	// 返回  : void 
	// 参数  : int conindex
	// 描述  : 已连接上dreb,发送注册信息
	void OnConnected(int conindex);

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
	// 描述  : 接收数据并处理
	void OnRecv(int conindex);

	/// 函数名: OnClose
	/// 编程  : 王明松 2009-5-21 13:18:27
	/// 返回  : void 
	/// 参数  : int conindex 连接的下标
	/// 参数  : const char *msg 关闭连接原因
	/// 描述  : 关闭客户端或服务端连接
	void OnClose(int conindex,const char *msg,const char *filename,int fileline);
	
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

	/// 函数名: GetMaxfdAddSelect
	/// 编程  : 王明松 2009-5-21 11:51:00
	/// 返回  : int 
	/// 参数  : int &maxfd
	/// 描述  : 获取最大的socket并将socket加入到select集合
	int GetMaxfdAddSelect(int &maxfd);

	// 函数名: Run
	// 编程  : 王明松 2010-3-12 13:00:15
	// 返回  : virtual int 
	// 描述  : 接收发送线程主方法
	virtual int Run();
	virtual bool InitThreadInstance();
	virtual void ExitThreadInstance();


	// 函数名: SetGlobalVar
	// 编程  : 王明松 2010-8-13 14:57:46
	// 返回  : bool 
	// 参数  : CGResource *res
	// 参数  : CPoolData *pooldata
	// 参数  : CPoolBuffer *mempool
	// 参数  : CSocketMgr *sockmgr
	// 参数  : CFuncTbl *tbl
	// 描述  : 设置全局参数
	bool SetGlobalVar(CGResource *res,CPoolDataMsg *pooldata,CBF_BufferPool *mempool,CSocketMgr *sockmgr,CFuncTbl *tbl);

	int m_nEnd;     //结束的连接序号，不包括此序号
	int m_nBegin;  //开始的连接序号
	int m_nScanBegin;

	// 函数名: StopTimer
	// 编程  : 王明松 2013-6-24 11:41:55
	// 返回  : void 
	// 描述  : 停止定时器
	void StopTimer();
	
	// 函数名: StartTimer
	// 编程  : 王明松 2013-6-24 11:41:58
	// 返回  : void 
	// 描述  : 启动定时器
	void StartTimer();
	
	// 函数名: OnTimer
	// 编程  : 王明松 2013-3-6 16:43:23
	// 返回  : static int 
	// 参数  : unsigned int event
	// 参数  : void *p
	// 描述  : 定时器回调方法
	static int OnTimer(unsigned int eventid,void *p);

	// 函数名: PingTimer
	// 编程  : 王明松 2013-6-24 11:34:04
	// 返回  : void 
	// 描述  : 定时器，心跳检测
	void PingTimer();

protected:
	CIErrlog *m_pLog;

	CBF_Des        m_pDes;

	CBF_Timer m_pTimer;//定时器
	bool      m_bIsPingTimer;
	
	CGResource  *m_pRes;
	CSocketMgr  *m_pSockMgr;
	CBF_BufferPool *m_pMemPool;//内存分配池;
	CPoolDataMsg   *m_pPoolData;//处理数据队列
	CFuncTbl    *m_pFuncTbl;//功能表
	CDrebEndian  m_pDrebEndian;
	S_BPC_RSMSG  m_pDataBuf;//发送数据结构
	S_BPC_RSMSG  m_pDataRcv;//接收数据结构
	CBF_SocketTcp  m_tcpSocket;
	
	CBF_ProcessMgr  m_pMgrBpu;    //启动bpu

	bool m_bIsInit;
	char m_errMsg[256];
	bool m_bIsRunning;
	
	fd_set m_rset;
	fd_set m_wset;
#if defined(_WINDOWS)
	fd_set m_eset;
#endif
	char  m_headMsg[8192];

};

#endif // !defined(AFX_LINKTHREAD_H__B0F78DC0_7B0B_4189_AE4D_6111A689B1A7__INCLUDED_)
