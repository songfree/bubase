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

	// ������: Run
	// ���  : ������ 2010-3-12 13:00:15
	// ����  : virtual int 
	// ����  : ���շ����߳�������
	virtual int Run();

	// ������: InitThreadInstance
	// ���  : ������ 2012-4-9 12:00:54
	// ����  : virtual bool 
	// ����  : �̳߳�ʼ�����߳�����ʱ����һ��
	virtual bool InitThreadInstance();
	
	// ������: ExitThreadInstance
	// ���  : ������ 2012-4-9 12:01:05
	// ����  : virtual void 
	// ����  : �˳��߳�Ҫ���Ĺ���,�߳��˳�ʱ����һ��
	virtual void ExitThreadInstance();

protected:


	// ������: Dispatch
	// ���  : ������ 2013-6-14 19:02:42
	// ����  : void 
	// ����  : ���ɽ���
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
	CBF_BufferPool *m_pMemPool;//�ڴ�����;
	CPoolData   *m_pPoolData;//�������ݶ���

	S_BPC_RSMSG  m_pDataBuf;//�������ݽṹ
};

#endif // !defined(AFX_MSGTHREAD_H__D0B49BB3_617E_41A8_8E96_203DB815C048__INCLUDED_)
