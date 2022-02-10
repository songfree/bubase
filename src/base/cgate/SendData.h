// SendData.h: interface for the CSendData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SENDDATA_H__9AA28FCE_97D2_493E_8D62_D2AB8EC75101__INCLUDED_)
#define AFX_SENDDATA_H__9AA28FCE_97D2_493E_8D62_D2AB8EC75101__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "GateHead.h"
#include "BF_Mutex.h"
#include "GateResource.h"

typedef std::deque<S_CGATE_SMSG> LIST_SENDDATA;//连接池连接列表

class CSendData  
{
public:
	CSendData();
	virtual ~CSendData();
public:
	int GetSize();
	bool GetData(S_CGATE_SMSG &data);
	int PushData(S_CGATE_SMSG data,int prio);

	CGateResource *m_pRes;
protected:
	LIST_SENDDATA m_poolDataList[3];//处理数据列表  
	CBF_Mutex m_mutex;//取连接互斥量
	CBF_MutexEvent m_pEvent;
};

#endif // !defined(AFX_SENDDATA_H__9AA28FCE_97D2_493E_8D62_D2AB8EC75101__INCLUDED_)
