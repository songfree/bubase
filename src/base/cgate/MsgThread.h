// MsgThread.h: interface for the CMsgThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MSGTHREAD_H__D0B49BB3_617E_41A8_8E96_203DB815C048__INCLUDED_)
#define AFX_MSGTHREAD_H__D0B49BB3_617E_41A8_8E96_203DB815C048__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "BF_Thread.h"
#include "IErrLog.h"
#include "BF_SocketTcp.h"
#include "GateResource.h"
#include "BF_DrebServer.h"
#include "BF_BufferPool.h"
#include "PoolData.h"
#include "BF_Des.h"
#include "BF_Tools.h"
#include "EncryptCommBuffer.h"
#include "BF_CommDataLog.h"

class CMsgThread : public CBF_Thread  
{
public:
	CMsgThread();
	virtual ~CMsgThread();

	

	bool SetGlobalVar(CGateResource *res,CPoolData *pooldata,CBF_BufferPool *mempool, CBF_DrebServer* drebapi);
private:
// 	bool ZipBuf(PCOMMSTRU data);
// 	bool UnzipBuf(COMMSTRU &data);

	// 函数名: Run
	// 编程  : 王明松 2010-3-12 13:00:15
	// 返回  : virtual int 
	// 描述  : 接收发送线程主方法
	virtual int Run();

	// 函数名: InitThreadInstance
	// 编程  : 王明松 2012-4-9 12:00:54
	// 返回  : virtual bool 
	// 描述  : 线程初始化，线程启动时调用一次
	virtual bool InitThreadInstance();
	
	// 函数名: ExitThreadInstance
	// 编程  : 王明松 2012-4-9 12:01:05
	// 返回  : virtual void 
	// 描述  : 退出线程要做的工作,线程退出时调用一次
	virtual void ExitThreadInstance();

protected:


	// 函数名: Dispatch
	// 编程  : 王明松 2013-6-14 19:02:42
	// 返回  : void 
	// 描述  : 分派交易
	void Dispatch();

	void LogDrebHead(int loglevel, DREB_HEAD head, const char *msg, const char *filename, int fileline);
	std::string GetDrebCmdType(int cmdtype);

	char m_headMsg[8192];

	CIErrlog        *m_pLog;

	CBF_Date_Time m_datetime;
	CBF_Des       m_pDes;
	CEncryptCommBuffer m_pDesZip;
	
	CGateResource  *m_pRes;
	CBF_DrebServer  *m_pDrebApi;
	CBF_BufferPool *m_pMemPool;//内存分配池;
	CPoolData   *m_pPoolData;//处理数据队列

	S_BPC_RSMSG  m_pDataBuf;//发送数据结构
};

#endif // !defined(AFX_MSGTHREAD_H__D0B49BB3_617E_41A8_8E96_203DB815C048__INCLUDED_)
