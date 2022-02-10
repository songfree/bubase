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
#include "PoolData.h"


#define  PRIORITIES         3    //�������ȼ�   ����Ϊ0·��Ϊ1  ����Ϊ2  


typedef std::list<S_TRADE_DATA> TRADE_RDQUEUE;

typedef struct
{
	TRADE_RDQUEUE  datas;
	CBF_Mutex   m_mutex;
	CBF_MutexEvent m_pEvent;
} TRADE_RQUEUEDATA;



class  CSendQueue  
{
public:
	
	CSendQueue();
	virtual ~CSendQueue();
public:
	
	bool QueueData();
	int GetSize(int prio);


	/// ������: DeleteSend
	/// ���  : ������ 2009-10-26 15:47:34
	/// ����  : void 
	/// ����  : TRADE_RDQUEUE::iterator  data
	/// ����  : ɾ�����ͼ�¼(���ͳɹ����η���ʧ��)
	void DeleteSend(TRADE_RDQUEUE::iterator  data,int prio);

	/// ������: NeedSend
	/// ���  : ������ 2009-10-26 14:57:46
	/// ����  : bool 
	/// ����  : �Ƿ���Ҫ��������
	bool NeedSend();



	/// ������: PutSendMsg
	/// ���  : ������ 2009-10-26 15:48:11
	/// ����  : int 
	/// ����  : S_TRADE_DATA msg
	/// ����  : int prio
	/// ����  : ����һ��������Ϣ�����
	int PutSendMsg(S_TRADE_DATA msg,int prio);
	
	TRADE_RQUEUEDATA       m_qSendData[PRIORITIES];  //���Ͷ���

	
	

protected:

	CBF_Mutex  m_sendnumMutex;
	bool m_bIsNeedSend;     //�Ƿ���Ҫ��������
	volatile int  m_nNeedSendNum;
	int m_sendNum;
};

#endif // !defined(AFX_SENDQUEUE_H__20329B7E_8E2B_485E_9161_864775BD05D9__INCLUDED_)
