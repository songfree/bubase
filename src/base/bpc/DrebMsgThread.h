// DrebMsgThread.h: interface for the CDrebMsgThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DREBMSGTHREAD_H__52F83741_0772_4053_96E3_452305F6E404__INCLUDED_)
#define AFX_DREBMSGTHREAD_H__52F83741_0772_4053_96E3_452305F6E404__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DrebMsgProcBase.h"
#include "BF_ProcessMgr.h"
#include "PoolData.h"
#include "FuncTbl.h"
#include "SocketMgr.h"

class CDrebMsgThread : public CDrebMsgProcBase  
{
public:
	
	
	CDrebMsgThread();
	virtual ~CDrebMsgThread();

	CIErrlog  *m_pLog;

	virtual  bool Init(CIBF_DrebServer* bus,CBF_DrebResource *res);
	bool SetGlobalVar(CPoolDataMsg* pooldata, CFuncTbl* tbl, CSocketMgr* sockmgr);
	
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
	CBF_ProcessMgr  m_pMgrBpu;    //启动bpu
    CPoolDataMsg* m_pPoolData;//处理数据队列
    CFuncTbl* m_pFuncTbl;//功能表
	CSocketMgr * m_pSockMgr;//bpu连接管理

	CBF_Des        m_pDes;
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
	//取bpu后续包
	virtual void OnGetBpuNextPack(S_BPC_RSMSG& rcvdata);
    //取bpc后续包
    virtual void OnGetBpcNextPack(S_BPC_RSMSG& rcvdata);
	//bpc监控信息
	virtual void MonitorInfo(S_BPC_RSMSG& rcvdata);
	//监控bpu信息
	virtual void MonitorBpu(S_BPC_RSMSG& rcvdata);
	//报告信息
	virtual void MonitorReport(S_BPC_RSMSG& rcvdata);
	virtual void OnMsgReportBpc(S_BPC_RSMSG& rcvdata);

	//解密解压数据
	virtual bool UnzipBuf(BPCCOMMSTRU& data);
	//设置bpu参数
	virtual void MonitorSetBpu(S_BPC_RSMSG& rcvdata);
	//设置模式
	virtual void MonitorSetMode(S_BPC_RSMSG& rcvdata);
	//停止bpu
	virtual void MonitorStopBpu(S_BPC_RSMSG& rcvdata);
	//启动bpu
	virtual void MonitorStartBpu(S_BPC_RSMSG& rcvdata);
};

#endif // !defined(AFX_DREBMSGTHREAD_H__52F83741_0772_4053_96E3_452305F6E404__INCLUDED_)
