// MoniRun.h: interface for the CGwRun class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MONIRUN_H__E3A7A7E0_2D18_4F18_8D6B_12F4F9FC22C6__INCLUDED_)
#define AFX_MONIRUN_H__E3A7A7E0_2D18_4F18_8D6B_12F4F9FC22C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "LinkThread.h"
#include "SocketMgr.h"


class CMoniRun  
{
public:

	// 函数名: Stop
	// 编程  : 王明松 2013-12-9 17:26:00
	// 返回  : void 
	// 描述  : 停止网关
	void Stop();

	// 函数名: GetErrMsg
	// 编程  : 王明松 2013-12-9 17:26:09
	// 返回  : std::string 
	// 描述  : 取出错信息
	std::string GetErrMsg();

	// 函数名: Start
	// 编程  : 王明松 2013-12-9 17:26:17
	// 返回  : bool 
	// 描述  : 启动网关
	bool Start();

	// 函数名: Monitor
	// 编程  : 王明松 2013-12-9 17:26:27
	// 返回  : void 
	// 描述  : 线程监控
	void Monitor();

	// 函数名: Init
	// 编程  : 王明松 2013-12-9 17:26:40
	// 返回  : bool 
	// 参数  : const char *confile
	// 描述  : 初始化
	bool Init(const char *confile);

	CMoniRun();
	virtual ~CMoniRun();

	CResource  g_pRes;

protected:
	char    m_sErrMsg[256];


	CLinkThread   m_pWorkThread;
	CSocketMgr    m_pSockMgr;
	

};

#endif // !defined(AFX_GWRUN_H__E3A7A7E0_2D18_4F18_8D6B_12F4F9FC22C6__INCLUDED_)
