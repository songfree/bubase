// GetPushThread.h: interface for the CGetPushThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GETPUSHTHREAD_H__C36CF9D7_5884_4EA7_8A6C_1CD08FCE09A9__INCLUDED_)
#define AFX_GETPUSHTHREAD_H__C36CF9D7_5884_4EA7_8A6C_1CD08FCE09A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "public.h"
#include "BF_Thread.h"

#include "ShareData.h"
#include "BF_SimpleLog.h"
#include "IVarDefine.h"
#include "GateLink.h"

class CGetPushThread : public CBF_Thread  
{
public:
	CGetPushThread();
	virtual ~CGetPushThread();


	CShareData *m_pShareData; //共享的数据参数
	CGateLink  *m_pGateLink;//网关api类
	CBF_SimpleLog *m_log;
	
	_THREAD_DATA_PARA  m_pDataPara;//线程参数

	int m_nID;//线程的ID

	bool m_bisRun ;
protected:
	
	
	
private:

	/// 函数名: Run
	/// 编程  : 王明松 2007-3-30 9:54:53
	/// 返回  : virtual int 
	/// 描述  : 线程的主处理方法
	virtual int Run();
	
	/// 函数名: Terminate
	/// 编程  : 王明松 2007-3-30 9:54:58
	/// 返回  : virtual bool 
	/// 描述  : 中止线程处理,由线程在结束时自动调用
	virtual bool Terminate();
};

#endif // !defined(AFX_GETPUSHTHREAD_H__C36CF9D7_5884_4EA7_8A6C_1CD08FCE09A9__INCLUDED_)
