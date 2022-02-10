// GateWay.h: interface for the CGateWay class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GATEWAY_H__FCB4568A_F5CA_41F9_AB12_6F4E7ECAF5C1__INCLUDED_)
#define AFX_GATEWAY_H__FCB4568A_F5CA_41F9_AB12_6F4E7ECAF5C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "GateResource.h"
#include "LinkThread.h"
#include "SocketMgr.h"
#include "BF_AIO.h"
#include "AIO_Work.h"
#include "MsgThread.h"
#include "SendThread.h"

class CGateWay  
{
public:

	// ������: Monitor
	// ���  : ������ 2015-11-17 14:28:09
	// ����  : void 
	// ����  : ����߳��Ƿ�ֹͣ
	void Monitor();

	// ������: GetErrMsg
	// ���  : ������ 2015-11-17 14:28:27
	// ����  : std::string 
	// ����  : ȡ������Ϣ
	std::string GetErrMsg();

	// ������: Stop
	// ���  : ������ 2015-11-17 14:27:59
	// ����  : void 
	// ����  : ֹͣ
	void Stop();

	// ������: Start
	// ���  : ������ 2015-11-17 14:27:55
	// ����  : bool 
	// ����  : ����
	bool Start();

	// ������: Init
	// ���  : ������ 2015-11-17 14:27:42
	// ����  : bool 
	// ����  : const char *confile
	// ����  : ��ʼ��
	bool Init(const char *confile);

	CGateWay();
	virtual ~CGateWay();

	CGateResource  g_pRes;

	
protected:
	
	CBF_AIO        g_pAioRun;   //IO������   epoll  select 
	CAIO_Work      g_pAioWork;  //IO������

	CPoolData      m_pPoolData;  //�ͻ��������  g_pAioWork�� m_pMsgThreadȡ

	CMsgThread     m_pMsgThread;//�ͻ���Ϣ�����߳�   �����յ�������ͳһ�������̴߳���

	CLinkThread    g_pDrebLink; //dreb������
	CSocketMgr     m_pSockMgr; //dreb���ӹ�����
	CBF_BufferPool m_pMemPool; //���ݻ����
	
	CSendData      m_pSendData;   //���Ϳͻ������ݶ���   m_pSendThreadȡ������ g_pAioWork��  g_pDrebLink��
	
	CSendThread    m_pSendThread[51];//���Ϳͻ����߳�
	
	CIErrlog       *m_pLog; //��־ָ��
private:

	char m_sErrMsg[256];

};

#endif // !defined(AFX_GATEWAY_H__FCB4568A_F5CA_41F9_AB12_6F4E7ECAF5C1__INCLUDED_)
