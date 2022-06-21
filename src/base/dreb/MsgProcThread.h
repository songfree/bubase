// MsgProcThread.h: interface for the CMsgProcThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MSGPROCTHREAD_H__56614FBC_0A67_43E7_A8B2_51D6CBB36441__INCLUDED_)
#define AFX_MSGPROCTHREAD_H__56614FBC_0A67_43E7_A8B2_51D6CBB36441__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//消息处理线程
#include "public.h"
#include "IErrLog.h"
#include "DrebPubDefine.h"
#include "BF_SocketTcp.h"
#include "DrebEndian.h"
#include "MemDB.h"
#include "GlobalVar.h"
#include "SocketMgr.h"
#include "BF_BufferPool.h"
#include "BF_Thread.h"
#include "PoolData.h"
#include "BF_Xml.h"




class CMsgProcThread : public CBF_Thread  
{
public:
	void PrintServiceRoute();
	void PrintRoute();
	
	

	// 函数名: OnCmdUnReqTx
	// 编程  : 王明松 2016-1-13 8:42:19
	// 返回  : void 
	// 参数  : S_DREB_RSMSG *msg
	// 描述  : 取消服务的交易注册
	void OnCmdUnReqTx(S_DREB_RSMSG *msg);

	void OnMsgMonitor(S_DREB_RSMSG *msg);

	void LogDrebHead(int loglevel, DREB_HEAD head, const char *msg, const char *filename, int fileline);

	// 函数名: Monitor_ReportStop
	// 编程  : 王明松 2013-3-6 16:44:17
	// 返回  : void 
	// 参数  : S_DREB_RSMSG *msg
	// 描述  : 停止监控报告
	void Monitor_ReportStop(S_DREB_RSMSG *msg);

	// 函数名: Monitor_ReportStart
	// 编程  : 王明松 2013-3-6 16:44:12
	// 返回  : void 
	// 参数  : S_DREB_RSMSG *msg
	// 描述  : 启动监控报告
	void Monitor_ReportStart(S_DREB_RSMSG *msg);

	// 函数名: Monitor_ConnInfo
	// 编程  : 王明松 2013-3-6 16:44:08
	// 返回  : void 
	// 参数  : S_DREB_RSMSG *msg
	// 描述  : 报告dreb连接信息
	void Monitor_ConnInfo(S_DREB_RSMSG *msg);

	// 函数名: Monitor_Report
	// 编程  : 王明松 2013-3-6 16:44:04
	// 返回  : void 
	// 参数  : S_DREB_RSMSG *msg
	// 描述  : 报告DREB数据转发信息
	void Monitor_Report(S_DREB_RSMSG *msg);

	// 函数名: Monitor_RouteService
	// 编程  : 王明松 2013-3-6 16:44:00
	// 返回  : void 
	// 参数  : S_DREB_RSMSG *msg
	// 描述  : 交易路由列表
	void Monitor_RouteService(S_DREB_RSMSG *msg);

	// 函数名: Monitor_RouteDreb
	// 编程  : 王明松 2013-3-6 16:43:56
	// 返回  : void 
	// 参数  : S_DREB_RSMSG *msg
	// 描述  : 节点路由表
	void Monitor_RouteDreb(S_DREB_RSMSG *msg);

	// 函数名: Monitor_CurTxList
	// 编程  : 王明松 2013-3-6 16:43:48
	// 返回  : void 
	// 参数  : S_DREB_RSMSG *msg
	// 描述  : 当前节点交易列表
	void Monitor_CurTxList(S_DREB_RSMSG *msg);

	// 函数名: Monitor_Host
	// 编程  : 王明松 2013-3-6 16:43:45
	// 返回  : void 
	// 参数  : S_DREB_RSMSG *msg
	// 描述  : 主机资源信息
	void Monitor_Host(S_DREB_RSMSG *msg);

	// 函数名: Monitor_info
	// 编程  : 王明松 2013-3-6 16:43:41
	// 返回  : void 
	// 参数  : S_DREB_RSMSG *msg
	// 描述  : DREB信息
	void Monitor_info(S_DREB_RSMSG *msg);

	

	// 函数名: OnCmdMonitorBpc
	// 编程  : 王明松 2013-3-4 15:20:41
	// 返回  : void 
	// 参数  : S_DREB_RSMSG *msg
	// 描述  : bpc监控交易
	void OnCmdMonitorBpc(S_DREB_RSMSG *msg);

	// 函数名: OnCmdMonitor
	// 编程  : 王明松 2013-3-4 15:20:14
	// 返回  : void 
	// 参数  : S_DREB_RSMSG *msg
	// 描述  : dreb监控交易
	void OnCmdMonitorDreb(S_DREB_RSMSG *msg);
	

	// 函数名: SetGlobalVar
	// 编程  : 王明松 2010-7-22 14:36:50
	// 返回  : bool 
	// 参数  : CGlobalVar *pVar
	// 参数  : CSocketMgr *sockmgr
	// 参数  : CMemDB *pMemDb
	// 参数  : CPoolBuffer *pool
	// 描述  : 设置全局参数
	bool SetGlobalVar(CGlobalVar *pVar,CSocketMgr *sockmgr,CMemDB *pMemDb,CBF_BufferPool *pool,CPoolData *msgqueue);

	/// 函数名: Run
	/// 编程  : 王明松 2009-8-3 10:44:00
	/// 返回  : int 
	/// 参数  : S_DREB_RSMSG *msg
	/// 描述  : 消息处理方法
	int Run(S_DREB_RSMSG *msg);


	CMsgProcThread();
	virtual ~CMsgProcThread();

	char m_errMsg[256];

	int  m_nIndex;

	std::string GetDrebCmdType(int cmdtype);
	std::string GetDrebSockType(int socktype);
	std::string GetIndexFlag(int index);

protected:

	// 函数名: TransBroadCast
	// 编程  : 王明松 2010-9-7 14:25:23
	// 返回  : void 
	// 参数  : S_DREB_RSMSG *msg
	// 参数  : bool isaffirm=true
	// 描述  : 广播信息
	void TransBroadCast(S_DREB_RSMSG *msg,bool isaffirm=true);


	// 函数名: TransMsgAns
	// 编程  : 王明松 2010-9-7 14:25:19
	// 返回  : void 
	// 参数  : S_DREB_RSMSG *msg
	// 描述  : 转发应答,通讯转发应答数据
	void TransMsgAns(S_DREB_RSMSG *msg);

	// 函数名: TransMsgRequest
	// 编程  : 王明松 2010-9-7 14:26:20
	// 返回  : void 
	// 参数  : S_DREB_RSMSG *msg
	// 参数  : bool isaffirm
	// 描述  : 转发请求,通讯转发请求数据
	void TransMsgRequest(S_DREB_RSMSG *msg,bool isaffirm);


	// 函数名: RouteMsgRequest
	// 编程  : 王明松 2016-1-13 8:43:09
	// 返回  : int 0 SUCCESS 非0出错码 
	// 参数  : S_DREB_RSMSG *msg
	// 参数  : bool isaffirm 是否确认应答
	// 描述  : 请求消息的路由转发
	void RouteMsgRequest(S_DREB_RSMSG *msg,bool isaffirm);


	// 函数名: AnsMsg
	// 编程  : 王明松 2010-9-7 14:27:03
	// 返回  : bool 
	// 参数  : S_DREB_RSMSG *msg
	// 参数  : unsigned short errcode  出错代码
	// 参数  : char *errmsg   出错信息
	// 描述  : 应答数据给请求方
	bool AnsMsg(S_DREB_RSMSG *msg,unsigned short errcode,const char *errmsg);

	// 函数名: OnCmdServiceReg
	// 编程  : 王明松 2010-9-7 14:24:19
	// 返回  : void 
	// 参数  : S_DREB_RSMSG *msg
	// 描述  : 本地的服务注册
	void OnCmdServiceReg(S_DREB_RSMSG *msg);



	// 函数名: OnCmdUnServiceReg
	// 编程  : 王明松 2014-12-12 9:56:02
	// 返回  : void 
	// 参数  : S_DREB_RSMSG *msg
	// 描述  : 取消交易注册
	void OnCmdUnServiceReg(S_DREB_RSMSG *msg);

	// 函数名: OnCmdService
	// 编程  : 王明松 2010-9-7 14:27:30
	// 返回  : void 
	// 参数  : S_DREB_RSMSG *msg
	// 描述  : 其它dreb上的服务路由
	void OnCmdService(S_DREB_RSMSG *msg);

	// 函数名: OnCmdDpPost
	// 编程  : 王明松 2010-9-7 14:28:09
	// 返回  : void 
	// 参数  : S_DREB_RSMSG *msg
	// 描述  : 处理DPPOST可靠推送消息
	void OnCmdDpPost(S_DREB_RSMSG *msg);

	// 函数名: OnCmdDpPush
	// 编程  : 王明松 2010-9-7 14:28:28
	// 返回  : void 
	// 参数  : S_DREB_RSMSG *msg
	// 描述  : 处理DPPUSH不可靠推送消息
	void OnCmdDpPush(S_DREB_RSMSG *msg);


	// 函数名: OnCmdDpABc
	// 编程  : 王明松 2010-9-7 14:28:49
	// 返回  : void 
	// 参数  : S_DREB_RSMSG *msg
	// 描述  : 处理DPABC无回应广播
	void OnCmdDpABc(S_DREB_RSMSG *msg);

	// 函数名: OnCmdDpBc
	// 编程  : 王明松 2010-9-7 14:29:39
	// 返回  : void 
	// 参数  : S_DREB_RSMSG *msg
	// 描述  : 处理DPBC有回应广播
	void OnCmdDpBc(S_DREB_RSMSG *msg);

	// 函数名: OnCmdDpACall
	// 编程  : 王明松 2010-9-7 14:29:53
	// 返回  : void 
	// 参数  : S_DREB_RSMSG *msg
	// 描述  : 处理DPACALL无回应调用消息
	void OnCmdDpACall(S_DREB_RSMSG *msg);

	// 函数名: OnCmdDpCall
	// 编程  : 王明松 2010-9-7 14:30:24
	// 返回  : void 
	// 参数  : S_DREB_RSMSG *msg
	// 描述  : 处理DPCALL有回应调用消息
	void OnCmdDpCall(S_DREB_RSMSG *msg);

	// 函数名: AnsConnect
	// 编程  : 王明松 2010-7-22 15:35:38
	// 返回  : bool 
	// 参数  : S_DREB_RSMSG *msg
	// 参数  : unsigned short errcode
	// 参数  : char *errmsg
	// 描述  : 应答给连接方
	bool AnsConnect(S_DREB_RSMSG *msg, unsigned short errcode,const char *errmsg);
	
	
	/// 函数名: OnCmdDelNode
	/// 编程  : 王明松 2009-8-3 10:45:39
	/// 返回  : void 
	/// 参数  : S_DREB_RSMSG *msg
	/// 描述  : 注销数据总线节点节点
	void OnCmdDelNode(S_DREB_RSMSG *msg);
	
	/// 函数名: OnCmdDelSvr
	/// 编程  : 王明松 2009-8-3 10:44:58
	/// 返回  : void 
	/// 参数  : S_DREB_RSMSG *msg
	/// 描述  : 注销服务
	void OnCmdDelSvr(S_DREB_RSMSG *msg);
	
	/// 函数名: OnCmdRoute
	/// 编程  : 王明松 2009-8-3 10:44:40
	/// 返回  : void 
	/// 参数  : S_DREB_RSMSG *msg
	/// 描述  : 路由通知
	void OnCmdRoute(S_DREB_RSMSG *msg);

	/// 函数名: AcceptDreb
	/// 编程  : 王明松 2009-8-3 10:44:29
	/// 返回  : void 
	/// 参数  : S_DREB_RSMSG *msg
	/// 描述  : 接受数据总线节点的级连
	void AcceptDreb(S_DREB_RSMSG *msg);
	
	/// 函数名: AcceptSvr
	/// 编程  : 王明松 2009-8-3 10:44:20
	/// 返回  : void 
	/// 参数  : S_DREB_RSMSG *msg
	/// 描述  : 接受服务的连接
	void AcceptSvr(S_DREB_RSMSG *msg);
	
	/// 函数名: AcceptClient
	/// 编程  : 王明松 2009-8-3 10:44:09
	/// 返回  : void 
	/// 参数  : S_DREB_RSMSG *msg
	/// 描述  : 接受客户端的连接
	void AcceptClient(S_DREB_RSMSG *msg);


	// 函数名: OnCmdConnect
	// 编程  : 王明松 2009-11-26 10:07:51
	// 返回  : void 
	// 参数  : S_DREB_RSMSG *msg
	// 描述  : 接收对端的连接注册，根据连接类型分别调用AcceptSvr、AcceptClient、AcceptDreb
	void OnCmdConnect(S_DREB_RSMSG *msg);


	// 函数名: SendService
	// 编程  : 王明松 2010-7-29 10:07:32
	// 返回  : void 
	// 参数  : int index dreb连接序号
	// 参数  : vector<S_SERVICE_ROUTE> rtlist
	// 描述  : 发送服务路由
	void SendService(int index, std::vector<S_SERVICE_ROUTE *> rtlist);

	// 函数名: OnMsgService
	// 编程  : 王明松 2010-7-26 15:40:13
	// 返回  : void 
	// 参数  : S_DREB_RSMSG *msg
	// 描述  : 发送服务列表通知
	void OnMsgServiceRoute(S_DREB_RSMSG *msg);

	// 函数名: OnMsgRoute
	// 编程  : 王明松 2009-10-30 9:16:23
	// 返回  : void 
	// 参数  : S_DREB_RSMSG *msg
	// 描述  : 处理发送路由消息
	void OnMsgRoute(S_DREB_RSMSG *msg);
	
	/// 函数名: OnMsgClose
	/// 编程  : 王明松 2009-5-25 14:47:12
	/// 返回  : void 
	/// 参数  : S_DREB_RSMSG *msg
	/// 描述  : 处理关闭连接消息
	void OnMsgClose(S_DREB_RSMSG *msg);
	
	
	/// 函数名: OnMsgRead
	/// 编程  : 王明松 2009-5-25 14:47:27
	/// 返回  : void 
	/// 参数  : S_DREB_RSMSG *msg
	/// 描述  : 处理接收到的数据，并根据数据类型进行处理
	void OnMsgRead(S_DREB_RSMSG *msg);
	
    /// 函数名: OnMsgRead
    /// 编程  : 王明松 2022-6-14 11:30:10
    /// 返回  : void 
    /// 参数  : S_DREB_RSMSG *msg
    /// 描述  : 处理服务订阅广播
	void OnCmdSubscribe(S_DREB_RSMSG* msg);

    /// 函数名: OnMsgRead
    /// 编程  : 王明松 2022-6-14 11:30:10
    /// 返回  : void 
    /// 参数  : S_DREB_RSMSG *msg
    /// 描述  : 处理服务取消订阅广播
	void OnCmdUnSubscribe(S_DREB_RSMSG* msg);

	virtual int Run();
	virtual bool InitThreadInstance();
	virtual void ExitThreadInstance();

protected:

	// 函数名: RouteMsgRequestByFunc
	// 编程  : 王明松 2016-1-13 8:48:37
	// 返回  : int 
	// 参数  : S_DREB_RSMSG *msg
	// 参数  : bool isaffirm  是否发送确认应答
	// 描述  : 通过交易路由
	void RouteMsgRequestByFunc(S_DREB_RSMSG *msg,bool isaffirm);

	// 函数名: RouteMsgRequestBySvr
	// 编程  : 王明松 2016-1-13 8:48:34
	// 返回  : int 
	// 参数  : S_DREB_RSMSG *msg
	// 参数  : bool isaffirm 是否发送确认应答
	// 描述  : 通过服务路由
	void RouteMsgRequestBySvr(S_DREB_RSMSG *msg,bool isaffirm);

	// 函数名: RouteMsgRequestByDreb
	// 编程  : 王明松 2016-1-13 8:48:30
	// 返回  : int 
	// 参数  : S_DREB_RSMSG *msg
	// 参数  : bool isaffirm 是否发送确认应答
	// 描述  : 通过总线节点路由
	void RouteMsgRequestByDreb(S_DREB_RSMSG *msg,bool isaffirm);

	
	
	
//	DREB_HEAD m_pRsMsgHead;
		
	CDrebEndian  m_pDrebEndian;

	CIErrlog      *m_log;

	//CBF_SimpleLog m_pLogRoute;

	CBF_Date_Time m_datetime;

	CBF_SocketTcp  m_tcpSocket;
	CGlobalVar     *m_pRes;
	CMemDB         *m_pMemDb;
	CSocketMgr     *m_pSocketMgr;
	CBF_BufferPool *m_pMemPool;
	CPoolData      *m_pMsgQueue;

	

	char  m_headMsg[8192];
};

#endif // !defined(AFX_MSGPROCTHREAD_H__56614FBC_0A67_43E7_A8B2_51D6CBB36441__INCLUDED_)
