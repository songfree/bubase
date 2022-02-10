// PoolData.h: interface for the CPoolData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POOLDATA_H__81F66324_E658_42D0_9F21_D391079F3ADB__INCLUDED_)
#define AFX_POOLDATA_H__81F66324_E658_42D0_9F21_D391079F3ADB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "BpcHead.h"
#include "BF_Mutex.h"
#include "TradeResource.h"
#include "IClientLink.h"


typedef std::deque<S_TRADE_DATA> LIST_POOLDATA;//连接池连接列表

class  CPoolData  
{
public:
	int GetSize();
	bool GetData(S_TRADE_DATA &data,int tms=1000);
	int PushData(S_TRADE_DATA data,int prio);
	CPoolData();
	virtual ~CPoolData();


	CTradeResource *m_pRes;
protected:
	LIST_POOLDATA m_poolDataList[3];//处理数据列表  
	CBF_Mutex m_mutex;//取连接互斥量
	CBF_MutexEvent m_pEvent;
};

class  CPoolDataMsg  
{
public:
	int GetSize();
	bool GetData(S_TRADE_DATA &data,int tms=1000);
	int PushData(S_TRADE_DATA data);
	CPoolDataMsg();
	virtual ~CPoolDataMsg();
	
	
	CTradeResource *m_pRes;
protected:
	LIST_POOLDATA m_poolDataList;//处理数据列表  
	CBF_Mutex m_mutex;//取连接互斥量
	CBF_MutexEvent m_pEvent;
};


#endif // !defined(AFX_POOLDATA_H__81F66324_E658_42D0_9F21_D391079F3ADB__INCLUDED_)
