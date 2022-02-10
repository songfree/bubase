// PoolData.h: interface for the CPoolData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POOLDATA_H__81F66324_E658_42D0_9F21_D391079F3ADB__INCLUDED_)
#define AFX_POOLDATA_H__81F66324_E658_42D0_9F21_D391079F3ADB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "GateHead.h"
#include "BF_Mutex.h"
#include "GlobalResource.h"

#ifdef CGATEAPI_EXPORTS
#define CGATEAPI_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define CGATEAPI_EXPORT __declspec(dllimport)
#else
#define CGATEAPI_EXPORT 
#endif
#endif

typedef std::deque<CGATE_COMMSTRU> LIST_POOLDATA;//���ӳ������б�

class  CGATEAPI_EXPORT CPoolData  
{
public:

	void Clear();

	// ������: GetSize
	// ���  : ������ 2013-5-29 9:12:08
	// ����  : int  
	// ����  : ȡ�ö���������
	int GetSize();

	// ������: GetData
	// ���  : ������ 2013-5-29 9:13:04
	// ����  : bool 
	// ����  : CGATE_COMMSTRU &data
	// ����  : int timeout ��ʱʱ�� ����,windows����С��1��
	// ����  : ȡ������
	bool GetData(CGATE_COMMSTRU &data,int timeout);

	// ������: PushData
	// ���  : ������ 2013-5-29 9:13:14
	// ����  : int 
	// ����  : S_CGATEAPI_SMSG data
	// ����  : ����һ������
	int PushData(CGATE_COMMSTRU data);

	CPoolData();
	virtual ~CPoolData();

protected:

	LIST_POOLDATA m_poolDataList;//���������б�  
	CBF_Mutex m_mutex;//ȡ���ӻ�����
	CBF_MutexEvent m_pEvent;
};

#endif // !defined(AFX_POOLDATA_H__81F66324_E658_42D0_9F21_D391079F3ADB__INCLUDED_)
