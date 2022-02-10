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
#include "GResource.h"
#include "SocketMgr.h"
#include "BF_BufferPool.h"
#include "PoolData.h"
#include "FuncTbl.h"
#include "dreberrcode.h"

class CMsgThread : public CBF_Thread  
{
public:
	
	
	CMsgThread();
	virtual ~CMsgThread();

	

	bool SetGlobalVar(CGResource *res,CPoolDataMsg *pooldata,CBF_BufferPool *mempool,CSocketMgr *sockmgr,CFuncTbl *tbl);

protected:
	void Dispatch2BpuGroupGetNext(S_BPC_RSMSG &rcvdata);
	void Dispatch2BpuGroup(S_BPC_RSMSG &rcvdata);
	void DispatchMonitor(S_BPC_RSMSG &rcvdata);
	void DispatchBpuReg(S_BPC_RSMSG &rcvdata);
	void RegisterDreb();

	void DispatchTrans(S_BPC_RSMSG &rcvdata);
	void DispatchExtCall(S_BPC_RSMSG &rcvdata);

	void Dispatch();

	void LogDrebHead(int loglevel, DREB_HEAD head, const char *msg, const char *filename, int fileline);
	std::string GetDrebCmdType(int cmdtype);

	int GetDrebIndex(int nodeid,int nodepid);

	char m_headMsg[8192];

	CIErrlog *m_pLog;
	CBF_Date_Time m_datetime;
	
	CGResource  *m_pRes;
	CSocketMgr  *m_pSockMgr;
	CBF_BufferPool *m_pMemPool;//�ڴ�����;
	CPoolDataMsg   *m_pPoolData;//�������ݶ���
	CFuncTbl    *m_pFuncTbl;
	
	CBpcEndian   m_pBpcEndian;
	CDrebEndian  m_pDrebEndian;

	S_BPC_RSMSG  m_pDataBuf;//�������ݽṹ

	int m_nDispatchNum;

	int m_nBegin;
	int m_nEnd;

private:
	// ������: Run
	// ���  : ������ 2010-3-12 13:00:15
	// ����  : virtual int 
	// ����  : ���շ����߳�������
	virtual int Run();
	virtual bool InitThreadInstance();
	virtual void ExitThreadInstance();
};

#endif // !defined(AFX_MSGTHREAD_H__D0B49BB3_617E_41A8_8E96_203DB815C048__INCLUDED_)
