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

#include "ShareData.h"
#include "BF_SimpleLog.h"
#include "IVarDefine.h"
#include "GateLink.h"
#include "GetPushThread.h"



#ifdef _WINDOWS
#ifdef _DEBUG
#pragma comment(lib, "bf_cgateapid.lib") 
#pragma message("Automatically linking with bf_cgateapid.lib ")
#else
#pragma comment(lib, "bf_cgateapi.lib") 
#pragma message("Automatically linking with bf_cgateapi.lib ")
#endif

#endif

typedef struct _SVAROFFSET
{
	int begin;
	int end;
	CIVarDefine *pVarPoint;
};
class CTestThread : public CBF_Thread  
{
public:
	int GetSerial();
	bool Login();

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


	// 函数名: SetData
	// 编程  : 王明松 2010-9-30 15:07:16
	// 返回  : bool 
	// 参数  : const char *data
	// 参数  : unsigned int datalen
	// 参数  : const char *logindata
	// 参数  : unsigned int logindatalen
	// 参数  : CShareData *pData
	// 参数  : _THREAD_DATA_PARA para
	// 描述  : 设置共享数据及发送参数及发送数据
	bool SetData(const char *data,unsigned int datalen,const char *logindata,unsigned int logindatalen,CShareData *pData,_THREAD_DATA_PARA para);

	virtual bool Init();
	
	CShareData *m_pShareData; //共享的数据参数

	unsigned short  m_nSerial;

	// 函数名: ExitThreadInstance
	// 编程  : 王明松 2012-4-9 12:01:05
	// 返回  : virtual void 
	// 描述  : 退出线程要做的工作,线程退出时调用一次
	virtual void ExitThreadInstance();
	// 函数名: InitThreadInstance
	// 编程  : 王明松 2012-4-9 12:00:54
	// 返回  : virtual bool 
	// 描述  : 线程初始化，线程启动时调用一次
	virtual bool InitThreadInstance();



	
	int m_nID;//线程的ID

protected:
	CBF_SimpleLog m_log;
	
	char *m_pSendData;//发送的脚本数据内容
	int m_nSendDataLen;//发送的脚本数据长度

	char *m_pLoginData;//发送的脚本数据内容
	int m_nLoginDataLen;//发送的脚本数据长度

	_THREAD_DATA_PARA  m_pDataPara;//线程参数

	CGateLink          m_pGateLink;//网关api类

	CGetPushThread      m_pGetPush;

	CGATE_COMMSTRU     m_pCgateLogin;


	vector<_SVAROFFSET>m_varList;

private:
	/// 函数名: Run
	/// 编程  : 王明松 2007-3-30 9:54:53
	/// 返回  : virtual int 
	/// 描述  : 线程的主处理方法
	virtual int Run();
	

};

#endif // !defined(AFX_TESTTHREAD_H__6548AE8E_CEA9_4163_8A7B_4BFF89F00B2B__INCLUDED_)
