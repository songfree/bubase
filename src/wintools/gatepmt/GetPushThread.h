// GetPushThread.h: interface for the CGetPushThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GETPUSHTHREAD_H__C36CF9D7_5884_4EA7_8A6C_1CD08FCE09A9__INCLUDED_)
#define AFX_GETPUSHTHREAD_H__C36CF9D7_5884_4EA7_8A6C_1CD08FCE09A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "public.h"
#include "BF_Thread.h"

#include "ShareData.h"
#include "BF_SimpleLog.h"
#include "IVarDefine.h"
#include "GateLink.h"

class CGetPushThread : public CBF_Thread  
{
public:
	CGetPushThread();
	virtual ~CGetPushThread();


	CShareData *m_pShareData; //��������ݲ���
	CGateLink  *m_pGateLink;//����api��
	CBF_SimpleLog *m_log;
	
	_THREAD_DATA_PARA  m_pDataPara;//�̲߳���

	int m_nID;//�̵߳�ID

	bool m_bisRun ;
protected:
	
	
	
private:

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
};

#endif // !defined(AFX_GETPUSHTHREAD_H__C36CF9D7_5884_4EA7_8A6C_1CD08FCE09A9__INCLUDED_)
