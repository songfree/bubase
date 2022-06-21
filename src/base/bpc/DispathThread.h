// DispathThread.h: interface for the CDispathThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DISPATHTHREAD_H__A40E0A0F_9F10_4C87_A4C3_E6D568746DFD__INCLUDED_)
#define AFX_DISPATHTHREAD_H__A40E0A0F_9F10_4C87_A4C3_E6D568746DFD__INCLUDED_
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


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDispathThread : public CBF_Thread  
{
public:
	CDispathThread();
	virtual ~CDispathThread();
public:

	bool SetGlobalVar(CGResource *res,CPoolData *pooldata,CBF_BufferPool *mempool,CSocketMgr *sockmgr,CFuncTbl *tbl);
	int m_nBegin;
	int m_nEnd;
	int m_nBpugroupindex;

	int m_nCurBpuIndex;
protected:
	
	void DispatchBpu(S_BPC_RSMSG &rcvdata);
	void Dispatch();
	
	void LogDrebHead(int loglevel, DREB_HEAD head, const char *msg, const char *filename, int fileline);
		
	char m_headMsg[8192];
	
	CIErrlog *m_pLog;
	CBF_Date_Time m_datetime;
	
	CGResource  *m_pRes;
	CSocketMgr  *m_pSockMgr;
	CBF_BufferPool *m_pMemPool;//�ڴ�����;
	CPoolData   *m_pPoolData;//�������ݶ���
	CFuncTbl    *m_pFuncTbl;
	
	CBpcEndian   m_pBpcEndian;
	CDrebEndian  m_pDrebEndian;
	
	S_BPC_RSMSG  m_pDataBuf;//�������ݽṹ
	
	int m_nDispatchNum;
	
	
	
private:
	// ������: Run
	// ���  : ������ 2010-3-12 13:00:15
	// ����  : virtual int 
	// ����  : ���շ����߳�������
	virtual int Run();
	virtual bool InitThreadInstance();
	virtual void ExitThreadInstance();
};

#endif // !defined(AFX_DISPATHTHREAD_H__A40E0A0F_9F10_4C87_A4C3_E6D568746DFD__INCLUDED_)
