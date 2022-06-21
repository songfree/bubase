// BpcRun.h: interface for the CBpcRun class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BPCRUN_H__1D668C64_8EE2_4658_B96B_197FF10850A5__INCLUDED_)
#define AFX_BPCRUN_H__1D668C64_8EE2_4658_B96B_197FF10850A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "BF_Tools.h"
#include "GResource.h"
#include "SocketMgr.h"
#include "LinkThread.h"
#include "FuncTbl.h"
#include "MsgThread.h"

#include "PuThread.h"
#include "DispathThread.h"
#include "BF_ProcessMgr.h"
#include "DrebMsgThread.h"
#include "BF_DrebServer.h"

class CBpcRun  
{
public:

	// ������: Monitor
	// ���  : ������ 2010-8-5 15:06:40
	// ����  : void 
	// ����  : ���bpc
	void Monitor();

	// ������: Stop
	// ���  : ������ 2010-8-5 15:06:32
	// ����  : void 
	// ����  : ֹͣbpc
	void Stop();

	// ������: Init
	// ���  : ������ 2010-8-5 15:06:23
	// ����  : bool 
	// ����  : const char *confile
	// ����  : bpc��ʼ��
	bool Init(const char *confile);
	CBpcRun();
	virtual ~CBpcRun();

public:
	CGResource m_pRes;
	CIErrlog    *m_pLog;

	CBF_DrebServer	m_pDrebApi;//����api
	CDrebMsgThread  m_pDrebSpi;//���߻ص���
	CPoolDataMsg   m_pPoolData;//��Ϣ�����̶߳���
	CMsgThread   m_pMsgThread;  //��Ϣ�����߳�   �����յ�������ͳһ�������̴߳���

	CFuncTbl     m_pFuncTbl;   //�����б�

	
	CBF_BufferPool  *m_pMemPool; //���ݻ����
	CBF_ProcessMgr  m_pMgrBpu;    //����bpu

	CLinkThread  m_pBpuLinkThread;  //BPU�����߳�
	CPuThread    m_pBuThread; //pu�����߳�
	CSocketMgr m_pSockMgr;     //���ӹ����� ���bpu������

	std::vector<CDispathThread *>  m_pDispath;//ÿ��bpu��һ�������߳�
	
	CBF_Xml      m_pBpuPidXml;

};

#endif // !defined(AFX_BPCRUN_H__1D668C64_8EE2_4658_B96B_197FF10850A5__INCLUDED_)
