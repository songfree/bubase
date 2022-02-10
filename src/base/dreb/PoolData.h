// PoolData.h: interface for the CPoolData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POOLDATA_H__81F66324_E658_42D0_9F21_D391079F3ADB__INCLUDED_)
#define AFX_POOLDATA_H__81F66324_E658_42D0_9F21_D391079F3ADB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "DrebPubDefine.h"
#include "BF_Mutex.h"
#include "GlobalVar.h"


typedef std::deque<S_DREB_RSMSG *> LIST_POOLDATA;//���ӳ������б�

class  CPoolData  
{
public:
	int GetSize();
	S_DREB_RSMSG *GetData();
	int PushData(S_DREB_RSMSG *data);
	CPoolData();
	virtual ~CPoolData();


	CGlobalVar *m_pRes;
protected:
	LIST_POOLDATA m_poolDataList;//���������б�  
	CBF_Mutex m_mutex;//ȡ���ӻ�����
	CBF_MutexEvent m_pEvent;
};

#endif // !defined(AFX_POOLDATA_H__81F66324_E658_42D0_9F21_D391079F3ADB__INCLUDED_)
