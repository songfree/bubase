// TestThread.h: interface for the CTestThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTTHREAD_H__6548AE8E_CEA9_4163_8A7B_4BFF89F00B2B__INCLUDED_)
#define AFX_TESTTHREAD_H__6548AE8E_CEA9_4163_8A7B_4BFF89F00B2B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "public.h"
#include "BF_Thread.h"
#include "FrontApi.h"
#include "ShareData.h"
#include "BF_SimpleLog.h"
#include "IVarDefine.h"
typedef struct _SVAROFFSET
{
	int begin;
	int end;
	CIVarDefine *pVarPoint;
};
class CTestThread : public CBF_Thread  
{
public:

	/// 函数名: ResetBuf
	/// 编程  : 王明松 2009-7-18 12:42:05
	/// 返回  : void 
	/// 描述  : 重新设置数据，取变量值
	void ResetBuf();
	

	/// 函数名: GetVarOffset
	/// 编程  : 王明松 2009-7-18 12:42:27
	/// 返回  : int 
	/// 参数  : CIVarDefine *var
	/// 参数  : const char *databuf
	/// 参数  : int datalen
	/// 参数  : vector<_SVAROFFSET>&result
	/// 描述  : 获取定义的变量在发送数据中的位置
	int GetVarOffset(CIVarDefine *var,const char *databuf,int datalen,vector<_SVAROFFSET>&result);

	/// 函数名: ThreadInit
	/// 编程  : 王明松 2009-7-17 17:46:14
	/// 返回  : int 
	/// 描述  : 测试线程初始化
	int ThreadInit();
	
	CTestThread();
	virtual ~CTestThread();

	bool SetData(const char *data,unsigned int datalen,CShareData *pData,unsigned int begintime,int conntype);

	virtual bool Init();
	
	CShareData *m_pShareData; //共享的数据参数
	
	int m_nID;//线程的ID

protected:
	CBF_SimpleLog m_log;
	CBF_Date_Time m_pDate;
	CFrontApi m_pFrontApi;//socket api

	int m_nBeginTime;//本线程开始运行时间,相对来说，毫秒单位
	char *m_pSendData;//发送的脚本数据内容
	int m_nSendDataLen;//发送的脚本数据长度
	int m_nConnType;//连接类型 0短连接1长连接

	vector<_SVAROFFSET>m_varList;

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

#endif // !defined(AFX_TESTTHREAD_H__6548AE8E_CEA9_4163_8A7B_4BFF89F00B2B__INCLUDED_)
