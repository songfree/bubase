// TibRcvThread.h: interface for the CTibRcvThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIBRCVTHREAD_H__063C6D75_972D_4D02_BC5E_5B4B06CAFAC7__INCLUDED_)
#define AFX_TIBRCVTHREAD_H__063C6D75_972D_4D02_BC5E_5B4B06CAFAC7__INCLUDED_


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
#include "BF_BufferPool.h"
#include "TibRcvQueue.h"

#ifdef TIB_EXPORTS
#define TIB_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define TIB_EXPORT __declspec(dllimport)
#else
#define TIB_EXPORT 
#endif
#endif

class TIB_EXPORT CTibRcvThread : public CBF_Thread  
{
public:

	// ������: Monitor
	// ���  : ������ 2015-6-26 16:19:25
	// ����  : void 
	// ����  : ����̣߳���ֹͣ������
	void Monitor();

	// ������: Stop
	// ���  : ������ 2015-6-26 16:19:19
	// ����  : void 
	// ����  : ֹͣ
	void Stop();

	// ������: Start
	// ���  : ������ 2015-6-26 16:19:12
	// ����  : bool 
	// ����  : ����
	bool Start();

	// ������: Init
	// ���  : ������ 2015-6-26 15:04:15
	// ����  : bool 
	// ����  : std::vector<CUdpSocketInfo *> *rcvinfo
	// ����  : CIErrlog *errlog
	// ����  : CBF_BufferPool *pool
	// ����  : ��ʼ��
	bool Init(std::vector<CUdpSocketInfo *> *rcvinfo, CIErrlog *errlog,CBF_BufferPool *pool,CTibRcvQueue *rcvqueue);

	// ������: AddIndex
	// ���  : ������ 2015-6-26 15:00:19
	// ����  : int 
	// ����  : int index
	// ����  : ����һ��������Ҫ���յ��б�����indexΪvector<CUdpSocketInfo *>���±�
	int AddIndex(int index);

	// ������: ClearIndex
	// ���  : ������ 2015-6-26 14:59:58
	// ����  : void 
	// ����  : ���Ҫ���յ����������б�
	void ClearIndex();
	CTibRcvThread();
	virtual ~CTibRcvThread();

private:
	virtual int Run();

	int GetMaxfdAddSelect(int &maxfd);

	void OnRecv(int conindex);
	
	void OnEvent();

	bool   m_bIsExit;
protected:

	std::vector<CUdpSocketInfo *> *m_pRcvInfo;//Ҫ���յĶ˿��б�
	std::vector<int> m_pRcvIndexList;//���߳���Ҫ���������б�,���m_pRcvInfo����±�

	CIErrlog *m_pLog;
	CBF_BufferPool *m_pMemPool;
	CTibRcvQueue    *m_pQueue;


	fd_set m_rset;
};

#endif // !defined(AFX_TIBRCVTHREAD_H__063C6D75_972D_4D02_BC5E_5B4B06CAFAC7__INCLUDED_)
