// PoolModule.h: interface for the CPoolModule class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POOLMODULE_H__4CB367FB_FA90_4631_8E27_179703CAF20D__INCLUDED_)
#define AFX_POOLMODULE_H__4CB367FB_FA90_4631_8E27_179703CAF20D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "BF_Thread.h"
#include "IErrLog.h"
#include "DrebPubDefine.h"
#include "BF_SocketTcp.h"
#include "DrebEndian.h"
#include "PoolData.h"
#include "SocketMgr.h"
#include "GlobalVar.h"
#include "MemDB.h"
#include "MsgProcThread.h"
#include "BF_Timer.h"

class CPoolModule : public CBF_Thread  
{
public:

	// 函数名: OnMonitor
	// 编程  : 王明松 2013-9-2 9:39:06
	// 返回  : void 
	// 描述  : 定时监控
	void OnMonitor();
	
	// 函数名: GetDrebCmdType
	// 编程  : 王明松 2013-5-13 16:09:40
	// 返回  : std::string 
	// 参数  : int cmdtype
	// 描述  : 取dreb命令字
	std::string GetDrebCmdType(int cmdtype);

	void LogDrebHead(int loglevel, DREB_HEAD head, const char *msg, const char *filename, int fileline);


	// 函数名: OnServiceRoute
	// 编程  : 王明松 2013-5-13 16:09:54
	// 返回  : void 
	// 描述  : 服务路由
	void OnServiceRoute();

	// 函数名: PrintRoute
	// 编程  : 王明松 2010-7-22 11:07:38
	// 返回  : void 
	// 描述  : 打印路由
	void PrintRoute();

	// 函数名: OnMsgProc
	// 编程  : 王明松 2010-7-22 11:07:56
	// 返回  : void 
	// 参数  : S_DREB_RSMSG *msg
	// 描述  : 处理消息
	void OnMsgProc(S_DREB_RSMSG *msg);

	// 函数名: OnRoute
	// 编程  : 王明松 2010-7-22 11:07:43
	// 返回  : void 
	// 描述  : 主动发送路由通知
	void OnRoute();

	/// 函数名: OnRecv
	/// 编程  : 王明松 2009-5-21 15:17:35
	/// 返回  : void 
	/// 参数  : int conindex
	/// 描述  : 接收数据
	void OnRecv(int conindex);

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

	/// 函数名: OnConnect
	/// 编程  : 王明松 2009-5-21 15:16:12
	/// 返回  : void 
	/// 参数  : int conindex
	/// 描述  : 去主动连接
	void OnConnect(int conindex);


	// 函数名: OnConnected
	// 编程  : 王明松 2013-6-24 12:43:58
	// 返回  : void 
	// 参数  : int conindex
	// 描述  : 连接成功
	void OnConnected(int conindex);

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

	/// 函数名: Initial
	/// 编程  : 王明松 2009-5-19 10:28:44
	/// 返回  : bool 
	/// 描述  : 初始化
	bool SetGlobalVar(CGlobalVar *pVar, CSocketMgr *sockmgr, CMemDB *pMemDb, CBF_BufferPool *pool,CPoolData *msgqueue);

	CPoolModule();
	virtual ~CPoolModule();

	CIErrlog   *m_log;
	bool m_bIsInit;
	int m_nEnd;
	int m_nBegin;
	
	CBF_SocketTcp m_tcpSocket;
	CMsgProcThread *m_pMsgProc;//消息处理类

	// 函数名: OnTimer
	// 编程  : 王明松 2013-3-6 16:43:23
	// 返回  : static int 
	// 参数  : unsigned int event
	// 参数  : void *p
	// 描述  : 定时器回调方法
	static int OnTimer(unsigned int eventid,void *p);


	// 函数名: ServiceRouteTimer
	// 编程  : 王明松 2013-6-24 11:31:59
	// 返回  : void 
	// 描述  : 定时器，发送服务路由
	void ServiceRouteTimer();

	// 函数名: RouteTimer
	// 编程  : 王明松 2013-6-24 11:32:03
	// 返回  : void 
	// 描述  : 定时器，发送节点路由
	void RouteTimer();


	// 函数名: PingTimer
	// 编程  : 王明松 2013-6-24 11:34:04
	// 返回  : void 
	// 描述  : 定时器，心跳检测
	void PingTimer();


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

protected:
	bool      m_bIsPingTimer;
	bool      m_bIsRouteTimer;
	bool      m_bIsServiceTimer;

	CBF_Timer m_pTimer;//定时器

	int m_nDrebHeadLen;

	CDrebEndian  m_pDrebEndian;    //字节序

	CGlobalVar     *m_pRes;        //全局参数

	CSocketMgr     *m_pSocketMgr;  //连接管理
	CBF_BufferPool *m_pMemPool;  //内存缓冲池
	CPoolData      *m_pMsgQueue;//消息队列

	fd_set m_rset;
	fd_set m_wset;
#if defined(_WINDOWS)
	fd_set m_eset;
#endif
	char  m_headMsg[8192];

private:
	virtual int Run();
	virtual bool InitThreadInstance();
	virtual void ExitThreadInstance();
	
};

#endif // !defined(AFX_POOLMODULE_H__4CB367FB_FA90_4631_8E27_179703CAF20D__INCLUDED_)
