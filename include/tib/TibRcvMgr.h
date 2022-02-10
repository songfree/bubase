// TibRcvMgr.h: interface for the CTibRcvMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIBRCVMGR_H__094F4D81_016C_40FD_AA3B_842C7FAA8FDF__INCLUDED_)
#define AFX_TIBRCVMGR_H__094F4D81_016C_40FD_AA3B_842C7FAA8FDF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "public.h"
#include "BF_Xml.h"
#include "BF_SocketUdp.h"
#include "IErrLog.h"

#include "BF_Thread.h"
#include "TibHead.h"
#include "PortTbl.h"
#include "UdpSocketInfo.h"
#include "TibRcvThread.h"
#include "BF_BufferPool.h"
#include "TibRcvQueue.h"

//���չ��� ����������߳�������
#ifdef TIB_EXPORTS
#define TIB_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define TIB_EXPORT __declspec(dllimport)
#else
#define TIB_EXPORT 
#endif
#endif

class TIB_EXPORT CTibRcvMgr  
{
public:

	// ������: GetQueueSize
	// ���  : ������ 2015-6-30 12:04:08
	// ����  : int 
	// ����  : ȡ���ն��д�С
	int GetQueueSize();

	// ������: GetData
	// ���  : ������ 2015-6-30 12:03:44
	// ����  : int <0������  0������
	// ����  : TIBCOMMSTRU &data
	// ����  : �ӽ��ն�����ȡ����
	int GetData(TIBCOMMSTRU &data);

	// ������: Monitor
	// ���  : ������ 2015-6-18 17:00:56
	// ����  : void 
	// ����  : ����߳��Ƿ�ֹͣ����ֹͣ������
	void Monitor();

	// ������: Stop
	// ���  : ������ 2015-6-18 17:00:40
	// ����  : void 
	// ����  : ֹͣ�����߳�
	void Stop();

	// ������: Start
	// ���  : ������ 2015-6-18 17:00:30
	// ����  : bool 
	// ����  : ���������߳�
	bool Start();

	// ������: Init
	// ���  : ������ 2015-6-18 15:56:45
	// ����  : bool 
	// ����  : CPortTbl *portlist
	// ����  : CIErrlog *errlog
	// ����  : CBF_Xml *tibxml
	// ����  : ��ʼ������ȡ���ò���
	bool Init(CPortTbl *portlist,CIErrlog *errlog,CBF_Xml *tibxml);

	CTibRcvMgr();
	virtual ~CTibRcvMgr();

private:
	

protected:
	bool     m_bIsInit;
	int      m_nRcvThreadNum;//�����߳���

	CPortTbl   *m_pListPort;//�˿��б�
	CIErrlog   *m_pLog;//��־��

	CBF_BufferPool  m_pMemPool;
	CTibRcvQueue    m_pQueue;//���ն���


	std::vector<CUdpSocketInfo *> m_pRcvInfo;//Ҫ���յĶ˿��б�CUdpSocketInfo��index��Ӧ��vector������
	std::vector<CTibRcvThread *>  m_pRcvThread;//�����߳��б�
		

	
};

#endif // !defined(AFX_TIBRCVMGR_H__094F4D81_016C_40FD_AA3B_842C7FAA8FDF__INCLUDED_)
