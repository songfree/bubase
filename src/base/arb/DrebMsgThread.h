// DrebMsgThread.h: interface for the CDrebMsgThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DREBMSGTHREAD_H__52F83741_0772_4053_96E3_452305F6E404__INCLUDED_)
#define AFX_DREBMSGTHREAD_H__52F83741_0772_4053_96E3_452305F6E404__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DrebMsgProcBase.h"
#include "ArbTbl.h"
#include "Xdp.h"
#include "BF_Timer.h"

class CDrebMsgThread : public CDrebMsgProcBase  
{
public:
	void SetSwitchTime(std::vector<ARB_S_APPINFO *> &appinfo);

	// 函数名: SwitchHost
	// 编程  : 王明松 2015-12-30 16:53:37
	// 返回  : void 
	// 参数  : unsigned short drebid
	// 参数  : unsigned int svr  公共服务号
	// 参数  : char m2b          由主转备的私有服务号
	// 参数  : char b2m          由备转主的私有服务号
	// 描述  : 主机切换
	void SwitchHost(unsigned short drebid,unsigned int svr, char m2b,char b2m);

	// 函数名: SendQueryHostStatus
	// 编程  : 王明松 2015-12-30 16:53:23
	// 返回  : void 
	// 参数  : unsigned int svr 公共服务号
	// 参数  : char pid         私有服务号
	// 描述  : 发送应用主机状态查询
	void SendQueryHostStatus(unsigned int svr,char pid);

	// 函数名: CheckArbByApp
	// 编程  : 王明松 2015-12-30 15:21:15
	// 返回  : void 
	// 参数  : std::vector<ARB_S_APPINFO *> appinfo
	// 描述  : 检查一类应用的主备，发查询、切换等
	void CheckArbByApp(std::vector<ARB_S_APPINFO *> appinfo);

	// 函数名: CheckArb
	// 编程  : 王明松 2015-12-29 11:16:40
	// 返回  : void 
	// 描述  : 检查应用
	void CheckArb();

	// 函数名: OnCheck
	// 编程  : 王明松 2015-12-29 11:16:53
	// 返回  : void 
	// 描述  : 置检查应用标志
	void OnCheck();
	
	CDrebMsgThread();
	virtual ~CDrebMsgThread();

	CArbTbl   m_tArb;
	CXdp      m_pXdp;
	CBF_Timer m_pTime;
	bool      m_bIsCheck;
	CIErrlog  *m_pLog;

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

	void OnAppPushStatus(S_BPC_RSMSG &rcvdata);

	void OnAppQueryHost(S_BPC_RSMSG &rcvdata);

	virtual int Run();
	
	// 函数名: ProcessDrebData
	// 编程  : 王明松 2015-4-23 15:53:34
	// 返回  : virtual void 
	// 参数  : S_BPC_RSMSG &rcvdata
	virtual void ProcessDrebData(S_BPC_RSMSG &rcvdata);

	// 函数名: OnGetBpcNextPack
	// 编程  : 王明松 2015-4-23 15:53:52
	// 返回  : virtual void 
	// 参数  : S_BPC_RSMSG &rcvdata
	// 描述  : 取本地的后续包,(后续包存在本地)
	virtual void OnGetBpcNextPack(S_BPC_RSMSG &rcvdata);

	// 函数名: OnGetBpuNextPack
	// 编程  : 王明松 2015-4-23 15:54:14
	// 返回  : virtual void 
	// 参数  : S_BPC_RSMSG &rcvdata
	// 描述  : 取BPU上的后续包，要发给BPU处理
	virtual void OnGetBpuNextPack(S_BPC_RSMSG &rcvdata);

	// 函数名: OnMsgRequest
	// 编程  : 王明松 2015-4-23 15:54:38
	// 返回  : virtual void 
	// 参数  : S_BPC_RSMSG &rcvdata
	// 描述  : 处理总线过来的请求数据
	virtual void OnMsgRequest(S_BPC_RSMSG &rcvdata);

	// 函数名: OnMsgMonitor
	// 编程  : 王明松 2015-4-23 15:54:57
	// 返回  : virtual void 
	// 参数  : S_BPC_RSMSG &rcvdata
	// 描述  : 处理总线过来的监控指令
	virtual void OnMsgMonitor(S_BPC_RSMSG &rcvdata);

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

	// 函数名: OnMsgReportBpc
	// 编程  : 王明松 2015-4-23 16:01:43
	// 返回  : virtual void 
	// 参数  : S_BPC_RSMSG &rcvdata
	// 描述  : 主动上报监控信息，发给总线
	virtual void OnMsgReportBpc(S_BPC_RSMSG &rcvdata);

	//以下为监控功能
	virtual void MonitorInfo(S_BPC_RSMSG &rcvdata);

	virtual void MonitorHost(S_BPC_RSMSG &rcvdata);

	virtual void MonitorBpu(S_BPC_RSMSG &rcvdata);

	virtual void MonitorReport(S_BPC_RSMSG &rcvdata);


};

#endif // !defined(AFX_DREBMSGTHREAD_H__52F83741_0772_4053_96E3_452305F6E404__INCLUDED_)
