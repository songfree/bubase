// SendQueue.h: interface for the CSendQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SENDQUEUE_H__20329B7E_8E2B_485E_9161_864775BD05D9__INCLUDED_)
#define AFX_SENDQUEUE_H__20329B7E_8E2B_485E_9161_864775BD05D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GateHead.h"
#include "BF_Mutex.h"

#ifdef CGATEAPI_EXPORTS
#define CGATEAPI_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define CGATEAPI_EXPORT __declspec(dllimport)
#else
#define CGATEAPI_EXPORT 
#endif
#endif

#ifdef CGATEAPI_EXPORTS
#define CGATEAPI_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define CGATEAPI_EXPORT __declspec(dllimport)
#else
#define CGATEAPI_EXPORT 
#endif
#endif

typedef std::list<S_CGATEAPI_SMSG> CGATE_SQUEUE;

typedef struct
{
	CGATE_SQUEUE   datas;
	CBF_Mutex      m_mutex;
} CGATE_SQUEUEDATA;

class CGATEAPI_EXPORT CSendQueue  
{
public:
	
	CSendQueue();
	virtual ~CSendQueue();
public:


	/// ������: DeleteSend
	/// ���  : ������ 2009-10-26 15:47:34
	/// ����  : void 
	/// ����  : RDQUEUE::iterator  data
	/// ����  : ɾ�����ͼ�¼(���ͳɹ����η���ʧ��)
	void DeleteSend(CGATE_SQUEUE::iterator  data);

	/// ������: NeedSend
	/// ���  : ������ 2009-10-26 14:57:46
	/// ����  : bool 
	/// ����  : �Ƿ���Ҫ��������
	bool NeedSend();



	/// ������: PutSendMsg
	/// ���  : ������ 2009-10-26 15:48:11
	/// ����  : int 
	/// ����  : S_DREB_RSMSG msg
	/// ����  : ����һ��������Ϣ�����
	int PutSendMsg(S_CGATEAPI_SMSG msg);

	
	CGATE_SQUEUEDATA       m_qSendData;  //���Ͷ���
protected:

	

	bool m_bIsNeedSend;     //�Ƿ���Ҫ��������
	int  m_nNeedSendNum;
	int m_sendNum;
};

#endif // !defined(AFX_SENDQUEUE_H__20329B7E_8E2B_485E_9161_864775BD05D9__INCLUDED_)
