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
	

	// ������: Monitor
	// ���  : ������ 2010-3-12 12:39:19
	// ����  : virtual int 
	// ����  : �鿴�����̣߳���ֹͣ������
	virtual int Monitor();
	

	// ������: Stop
	// ���  : ������ 2010-3-12 12:39:07
	// ����  : virtual void 
	// ����  : ֹͣ���д����߳�
	virtual void Stop();
	
	/// ������: GetErrMsg
	/// ���  : ������ 2008-9-3 15:01:56
	/// ����  : std::string 
	/// ����  : ��ȡ������Ϣ
	virtual std::string GetErrMsg();
	
	/// ������: Init
	/// ���  : ������ 2008-9-3 11:32:02
	/// ����  : virtual bool 
	/// ����  : �����ò����󣬶Թ����߳̽��г�ʼ������ʼ�������̲߳����������߳�
	virtual bool InitPmgr();
	
	// ������: SetGlobalVar
	// ���  : ������ 2010-3-12 11:40:46
	// ����  : bool 
	// ����  : CGResource *res  ȫ�ֱ���
	// ����  : CPoolData *pooldata  �������ݶ���
	// ����  : CPoolBuffer *mempool  �ڴ����
	// ����  : CSocketMgr *sockmgr   ���ӹ���
	// ����  : void *para   ȫ�ֹ������ �繲���ڴ�
	// ����  : ����ȫ�ֱ���
	bool SetGlobalVar(CGResource *res,CPoolData *pooldata,CBF_BufferPool *mempool,CSocketMgr *sockmgr,void *para);

	
protected:

	/// ������: Run
	/// ���  : ������ 2007-3-30 9:54:53
	/// ����  : virtual int 
	/// ����  : �̵߳���������
	virtual int Run();
	
	/// ������: Terminate
	/// ���  : ������ 2007-3-30 9:54:58
	/// ����  : virtual bool 
	/// ����  : ��ֹ�̴߳���,���߳��ڽ���ʱ�Զ�����
	virtual bool Terminate();

	
	CIErrlog      *m_log;//��־��
	CPoolData    *m_pPoolData;//�������ݶ���
	CBF_BufferPool  *m_pMemPool;//�ڴ����
	CGResource *m_pRes;//ȫ����Դ��
	CSocketMgr *m_pSockMgr;//���ӹ�����

	void *m_pPara;//ȫ�ֲ���
	
	bool m_bIsInit;//�Ƿ��ʼ��

	CBF_Mutex m_mutex;//�������

	
	bool m_bIsSetGlobalVar;//
	std::string m_errMsg;//������Ϣ
	bool m_bIsRunning;

	CBF_ProcessMgr  m_procMgr;

	S_BF_RSMSG_FPC   m_pReqAnsData;   //����Ӧ������

	char errmsg[200];


protected:

};

#endif // !defined(AFX_PROCESSMGR_H__09F6D209_AB88_4E50_A248_12B1F1BF6547__INCLUDED_)
