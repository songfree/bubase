// ProcessMgr.h: interface for the CProcessMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROCESSMGR_H__09F6D209_AB88_4E50_A248_12B1F1BF6547__INCLUDED_)
#define AFX_PROCESSMGR_H__09F6D209_AB88_4E50_A248_12B1F1BF6547__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GResource.h"
#include "PoolData.h"
#include "BF_BufferPool.h"
#include "SocketMgr.h"
#include "BF_Thread.h"
#include "BF_ProcessMgr.h"

class   CProcessThread  : public CBF_Thread
{
public:
	
	CProcessThread();
	virtual ~CProcessThread();

public:
	void GetSerial();
	void DispatchBuAnsFree();
	void DispatchBuFree();
	void DispatchBuAns();
	void DispatchBu();
	void Dispatch();
	

	// 函数名: Monitor
	// 编程  : 王明松 2010-3-12 12:39:19
	// 返回  : virtual int 
	// 描述  : 查看所有线程，若停止则启动
	virtual int Monitor();
	

	// 函数名: Stop
	// 编程  : 王明松 2010-3-12 12:39:07
	// 返回  : virtual void 
	// 描述  : 停止所有处理线程
	virtual void Stop();
	
	/// 函数名: GetErrMsg
	/// 编程  : 王明松 2008-9-3 15:01:56
	/// 返回  : std::string 
	/// 描述  : 获取出错信息
	virtual std::string GetErrMsg();
	
	/// 函数名: Init
	/// 编程  : 王明松 2008-9-3 11:32:02
	/// 返回  : virtual bool 
	/// 描述  : 在设置参数后，对管理线程进行初始化，初始化处理线程并启动处理线程
	virtual bool InitPmgr();
	
	// 函数名: SetGlobalVar
	// 编程  : 王明松 2010-3-12 11:40:46
	// 返回  : bool 
	// 参数  : CGResource *res  全局变量
	// 参数  : CPoolData *pooldata  处理数据队列
	// 参数  : CPoolBuffer *mempool  内存分配
	// 参数  : CSocketMgr *sockmgr   连接管理
	// 参数  : void *para   全局共享变量 如共享内存
	// 描述  : 设置全局变量
	bool SetGlobalVar(CGResource *res,CPoolData *pooldata,CBF_BufferPool *mempool,CSocketMgr *sockmgr,void *para);

	
protected:

	/// 函数名: Run
	/// 编程  : 王明松 2007-3-30 9:54:53
	/// 返回  : virtual int 
	/// 描述  : 线程的主处理方法
	virtual int Run();
	
	/// 函数名: Terminate
	/// 编程  : 王明松 2007-3-30 9:54:58
	/// 返回  : virtual bool 
	/// 描述  : 中止线程处理,由线程在结束时自动调用
	virtual bool Terminate();

	
	CIErrlog      *m_log;//日志类
	CPoolData    *m_pPoolData;//处理数据队列
	CBF_BufferPool  *m_pMemPool;//内存分配
	CGResource *m_pRes;//全局资源类
	CSocketMgr *m_pSockMgr;//连接管理类

	void *m_pPara;//全局参数
	
	bool m_bIsInit;//是否初始化

	CBF_Mutex m_mutex;//互斥变量

	
	bool m_bIsSetGlobalVar;//
	std::string m_errMsg;//出错信息
	bool m_bIsRunning;

	CBF_ProcessMgr  m_procMgr;

	S_BF_RSMSG_FPC   m_pReqAnsData;   //请求应答数据

	char errmsg[200];


protected:

};

#endif // !defined(AFX_PROCESSMGR_H__09F6D209_AB88_4E50_A248_12B1F1BF6547__INCLUDED_)
