// MoniRun.h: interface for the CGwRun class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MONIRUN_H__E3A7A7E0_2D18_4F18_8D6B_12F4F9FC22C6__INCLUDED_)
#define AFX_MONIRUN_H__E3A7A7E0_2D18_4F18_8D6B_12F4F9FC22C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "LinkThread.h"
#include "SocketMgr.h"


class CMoniRun  
{
public:

	// ������: Stop
	// ���  : ������ 2013-12-9 17:26:00
	// ����  : void 
	// ����  : ֹͣ����
	void Stop();

	// ������: GetErrMsg
	// ���  : ������ 2013-12-9 17:26:09
	// ����  : std::string 
	// ����  : ȡ������Ϣ
	std::string GetErrMsg();

	// ������: Start
	// ���  : ������ 2013-12-9 17:26:17
	// ����  : bool 
	// ����  : ��������
	bool Start();

	// ������: Monitor
	// ���  : ������ 2013-12-9 17:26:27
	// ����  : void 
	// ����  : �̼߳��
	void Monitor();

	// ������: Init
	// ���  : ������ 2013-12-9 17:26:40
	// ����  : bool 
	// ����  : const char *confile
	// ����  : ��ʼ��
	bool Init(const char *confile);

	CMoniRun();
	virtual ~CMoniRun();

	CResource  g_pRes;

protected:
	char    m_sErrMsg[256];


	CLinkThread   m_pWorkThread;
	CSocketMgr    m_pSockMgr;
	

};

#endif // !defined(AFX_GWRUN_H__E3A7A7E0_2D18_4F18_8D6B_12F4F9FC22C6__INCLUDED_)
