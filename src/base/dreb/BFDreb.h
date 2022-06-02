// BFDreb.h: interface for the CBFDreb class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BFDREB_H__F7D3EEF5_1AE2_4B2D_A88B_9ED89B731E18__INCLUDED_)
#define AFX_BFDREB_H__F7D3EEF5_1AE2_4B2D_A88B_9ED89B731E18__INCLUDED_




#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BF_Thread.h"
#include "BF_SocketTcp.h"
#include "BF_Tools.h"
#include "PoolModule.h"
#include "GlobalVar.h"
#include "Dreb_Timer.h"
#include "PoolData.h"
#include "MsgProcThread.h"

extern CDreb_Timer g_pDrebTimer;


class CBFDreb
{
public:
	void Monitor();

	// ������: OnStop
	// ���  : ������ 2009-5-16 14:00:40
	// ����  : void 
	// ����  : ��ȫ��ֹͣ��־Ϊֹͣ
	void OnStop();

	// ������: Init
	// ���  : ������ 2009-5-16 13:58:33
	// ����  : bool 
	// ����  : �������߳�ʼ���������á���ʼ�����ӳ����顢��ʼ����������ջ����ʼ���ڴ����ݿ�(·�ɱ�)
	bool Init(const char *confile);


	CBFDreb();
	virtual ~CBFDreb();

	CGlobalVar g_Vars;//ȫ�ֱ���

protected:

	CBF_LogClient       *m_log;
	
	CPoolModule         g_pPoolModule;//�����߳�
	CMsgProcThread      g_pProcThread[5];//��Ϣ�����߳�
	CPoolData           g_pMsgQueue;  //��Ϣ����

	CSocketMgr          g_connInfoList;//���ӹ�����
	CBF_BufferPool      g_pMemPool;//�ڴ�����
	CMemDB              g_pMemDb; //�ڴ����ݱ�
	
	
};

#endif // !defined(AFX_BFDREB_H__F7D3EEF5_1AE2_4B2D_A88B_9ED89B731E18__INCLUDED_)
