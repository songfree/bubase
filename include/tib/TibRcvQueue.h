// TibRcvQueue.h: interface for the CTibRcvQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIBRCVQUEUE_H__4DF3B6DC_13D8_42B3_9B1D_E1221765B9BD__INCLUDED_)
#define AFX_TIBRCVQUEUE_H__4DF3B6DC_13D8_42B3_9B1D_E1221765B9BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TibHead.h"
#include "BF_Mutex.h"

#ifdef TIB_EXPORTS
#define TIB_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define TIB_EXPORT __declspec(dllimport)
#else
#define TIB_EXPORT 
#endif
#endif

typedef std::deque<TIBCOMMSTRU *> DEQUE_RCV;//接收队列

class TIB_EXPORT CTibRcvQueue  
{
public:
	CTibRcvQueue();
	virtual ~CTibRcvQueue();

	int  GetSize();
	bool GetData(TIBCOMMSTRU **data);
	int  PushData(TIBCOMMSTRU *data);

protected:

	DEQUE_RCV m_poolDataList;//  
	CBF_Mutex m_mutex;//取连接互斥量
	CBF_MutexEvent m_pEvent;

};

#endif // !defined(AFX_TIBRCVQUEUE_H__4DF3B6DC_13D8_42B3_9B1D_E1221765B9BD__INCLUDED_)
