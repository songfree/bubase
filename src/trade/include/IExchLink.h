// IExchLink.h: interface for the CIExchLink class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IEXCHLINK_H__D49449A0_4511_4E9B_A39F_5906E6FEDE4A__INCLUDED_)
#define AFX_IEXCHLINK_H__D49449A0_4511_4E9B_A39F_5906E6FEDE4A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Xdp.h"
#include "IClientLink.h"
#include "BF_Xml.h"
#include "MemDb.h"
#include "IErrLog.h"
#include "BF_BufferPool.h"

class CIExchLink  
{
public:


	// ������: Send2Exch
	// ���  : ������ 2017-7-20 9:33:01
	// ����  : virtual int 
	// ����  : S_TRADE_DATA &data  
	// ����  : CXdp *xdp   xdp����
	// ����  : ������������ע�������Ҫ����Ӧ��ģ�Ҫ��¼����ͷ��Ӧ��ʱ�ٰѱ���ͷ������
	virtual int Send2Exch(S_TRADE_DATA &data,CXdp *xdp)=0;


	// ������: Init
	// ���  : ������ 2017-8-10 15:41:08
	// ����  : virtual bool 
	// ����  : CBF_Xml *xml
	// ����  : ��ʼ��
	virtual bool Init(CBF_Xml *xml,CIClientLink *pBusLink,CMemDb *pMemDb,	CXdp *pXdp,	CIErrlog *pLog,CBF_BufferPool *pMemPool)=0;

	// ������: Start
	// ���  : ������ 2017-8-10 15:39:45
	// ����  : virtual bool 
	// ����  : �������������̨����
	virtual bool Start()=0;


	// ������: Stop
	// ���  : ������ 2017-8-10 15:40:09
	// ����  : virtual bool 
	// ����  : ֹͣ
	virtual bool Stop()=0;


	// ������: Join
	// ���  : ������ 2017-8-10 15:40:21
	// ����  : virtual bool 
	// ����  : �ȴ��߳�ֹͣ
	virtual bool Join()=0;

public:
	CIExchLink()
	{
		m_pMemDb = NULL;
		m_pLog = NULL;
		m_pMemPool = NULL;
	}
	virtual ~CIExchLink()
	{
		
	}
protected:
	CIClientLink *m_pBusLink;//�������ߵĻ���ָ��
	CMemDb       *m_pMemDb;
	CXdp          m_pXdp;
	CIErrlog      *m_pLog;
	CBF_BufferPool *m_pMemPool;//�ڴ滺���

};

#endif // !defined(AFX_IEXCHLINK_H__D49449A0_4511_4E9B_A39F_5906E6FEDE4A__INCLUDED_)
