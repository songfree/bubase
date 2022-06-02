// BFDreb.h: interface for the CBFDreb class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BFDREB_H__F7D3EEF5_1AE2_4B2D_A88B_9ED89B731E18__INCLUDED_)
#define AFX_BFDREB_H__F7D3EEF5_1AE2_4B2D_A88B_9ED89B731E18__INCLUDED_




#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BF_Thread.h"
#include "BF_SocketTcp.h"
#include "BF_Tools.h"
#include "PoolModule.h"
#include "GlobalVar.h"
#include "Dreb_Timer.h"
#include "PoolData.h"
#include "MsgProcThread.h"

extern CDreb_Timer g_pDrebTimer;


class CBFDreb
{
public:
	void Monitor();

	// 函数名: OnStop
	// 编程  : 王明松 2009-5-16 14:00:40
	// 返回  : void 
	// 描述  : 置全局停止标志为停止
	void OnStop();

	// 函数名: Init
	// 编程  : 王明松 2009-5-16 13:58:33
	// 返回  : bool 
	// 描述  : 数据总线初始化，读配置、初始化连接池数组、初始化接收数据栈、初始化内存数据库(路由表)
	bool Init(const char *confile);


	CBFDreb();
	virtual ~CBFDreb();

	CGlobalVar g_Vars;//全局变量

protected:

	CBF_LogClient       *m_log;
	
	CPoolModule         g_pPoolModule;//接收线程
	CMsgProcThread      g_pProcThread[5];//消息处理线程
	CPoolData           g_pMsgQueue;  //消息队列

	CSocketMgr          g_connInfoList;//连接管理类
	CBF_BufferPool      g_pMemPool;//内存分配池
	CMemDB              g_pMemDb; //内存数据表
	
	
};

#endif // !defined(AFX_BFDREB_H__F7D3EEF5_1AE2_4B2D_A88B_9ED89B731E18__INCLUDED_)
