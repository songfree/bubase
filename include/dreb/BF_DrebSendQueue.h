// BF_DrebSendQueue.h: interface for the CBF_DrebSendQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BF_DREBSENDQUEUE_H__ED7DF2D9_25FD_4088_8A89_EA3F81490F65__INCLUDED_)
#define AFX_BF_DREBSENDQUEUE_H__ED7DF2D9_25FD_4088_8A89_EA3F81490F65__INCLUDED_

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



#ifdef DREBAPI_EXPORTS
#define DREBAPI_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define DREBAPI_EXPORT __declspec(dllimport)
#else
#define DREBAPI_EXPORT 
#endif
#endif

class DREBAPI_EXPORT CBF_DrebSendQueue  
{
public:
	CBF_DrebSendQueue();
	virtual ~CBF_DrebSendQueue();
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
	/// ����  : int <0ʧ��  0�ɹ�
	/// ����  : S_BPC_RSMSG msg
	/// ����  : ����һ��������Ϣ�����
	int PutSendMsg(S_BPC_RSMSG msg);
	
	RQUEUEDATA       m_qSendData;  //���Ͷ���
	
protected:
	
	bool m_bIsNeedSend;     //�Ƿ���Ҫ��������
	int  m_nNeedSendNum;
	int m_sendNum;
};

#endif // !defined(AFX_BF_DREBSENDQUEUE_H__ED7DF2D9_25FD_4088_8A89_EA3F81490F65__INCLUDED_)
