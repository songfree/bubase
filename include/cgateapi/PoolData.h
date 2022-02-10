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

typedef std::deque<CGATE_COMMSTRU> LIST_POOLDATA;//连接池连接列表

class  CGATEAPI_EXPORT CPoolData  
{
public:

	void Clear();

	// 函数名: GetSize
	// 编程  : 王明松 2013-5-29 9:12:08
	// 返回  : int  
	// 描述  : 取得队列里数据
	int GetSize();

	// 函数名: GetData
	// 编程  : 王明松 2013-5-29 9:13:04
	// 返回  : bool 
	// 参数  : CGATE_COMMSTRU &data
	// 参数  : int timeout 超时时间 毫秒,windows不能小于1秒
	// 描述  : 取得数据
	bool GetData(CGATE_COMMSTRU &data,int timeout);

	// 函数名: PushData
	// 编程  : 王明松 2013-5-29 9:13:14
	// 返回  : int 
	// 参数  : S_CGATEAPI_SMSG data
	// 描述  : 放入一条数据
	int PushData(CGATE_COMMSTRU data);

	CPoolData();
	virtual ~CPoolData();

protected:

	LIST_POOLDATA m_poolDataList;//处理数据列表  
	CBF_Mutex m_mutex;//取连接互斥量
	CBF_MutexEvent m_pEvent;
};

#endif // !defined(AFX_POOLDATA_H__81F66324_E658_42D0_9F21_D391079F3ADB__INCLUDED_)
