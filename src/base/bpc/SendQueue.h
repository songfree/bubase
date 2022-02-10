// SendQueue.h: interface for the CSendQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SENDQUEUE_H__20329B7E_8E2B_485E_9161_864775BD05D9__INCLUDED_)
#define AFX_SENDQUEUE_H__20329B7E_8E2B_485E_9161_864775BD05D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "BF_Mutex.h"
#include "BpcHead.h"


typedef std::list<S_BPC_RSMSG> RDQUEUE;

typedef struct
{
	RDQUEUE  datas;
	CBF_Mutex   m_mutex;
	CBF_MutexEvent m_pEvent;
} RQUEUEDATA;



class  CSendQueue  
{
public:
	
	CSendQueue();
	virtual ~CSendQueue();
public:

	// ������: Clear
	// ���  : ������ 2013-10-18 9:17:36
	// ����  : void 
	// ����  : ��ն���
	void Clear();

	/// ������: DeleteSend
	/// ���  : ������ 2009-10-26 15:47:34
	/// ����  : void 
	/// ����  : int prio
	/// ����  : RDQUEUE::iterator  data
	/// ����  : ɾ�����ͼ�¼(���ͳɹ����η���ʧ��)
	void DeleteSend(RDQUEUE::iterator  data);

	/// ������: NeedSend
	/// ���  : ������ 2009-10-26 14:57:46
	/// ����  : bool 
	/// ����  : �Ƿ���Ҫ��������
	bool NeedSend();



	/// ������: PutSendMsg
	/// ���  : ������ 2009-10-26 15:48:11
	/// ����  : int 
	/// ����  : S_BPC_RSMSG msg
	/// ����  : ����һ��������Ϣ�����
	int PutSendMsg(S_BPC_RSMSG msg);

	RQUEUEDATA       m_qSendData;  //���Ͷ���

protected:

	bool m_bIsNeedSend;     //�Ƿ���Ҫ��������
	int  m_nNeedSendNum;
	int m_sendNum;
};

#endif // !defined(AFX_SENDQUEUE_H__20329B7E_8E2B_485E_9161_864775BD05D9__INCLUDED_)
