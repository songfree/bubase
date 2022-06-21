// BpcRun.h: interface for the CBpcRun class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BPCRUN_H__1D668C64_8EE2_4658_B96B_197FF10850A5__INCLUDED_)
#define AFX_BPCRUN_H__1D668C64_8EE2_4658_B96B_197FF10850A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "BF_Tools.h"
#include "GResource.h"
#include "SocketMgr.h"
#include "LinkThread.h"
#include "FuncTbl.h"
#include "MsgThread.h"

#include "PuThread.h"
#include "DispathThread.h"
#include "BF_ProcessMgr.h"
#include "DrebMsgThread.h"
#include "BF_DrebServer.h"

class CBpcRun  
{
public:

	// 函数名: Monitor
	// 编程  : 王明松 2010-8-5 15:06:40
	// 返回  : void 
	// 描述  : 监控bpc
	void Monitor();

	// 函数名: Stop
	// 编程  : 王明松 2010-8-5 15:06:32
	// 返回  : void 
	// 描述  : 停止bpc
	void Stop();

	// 函数名: Init
	// 编程  : 王明松 2010-8-5 15:06:23
	// 返回  : bool 
	// 参数  : const char *confile
	// 描述  : bpc初始化
	bool Init(const char *confile);
	CBpcRun();
	virtual ~CBpcRun();

public:
	CGResource m_pRes;
	CIErrlog    *m_pLog;

	CBF_DrebServer	m_pDrebApi;//总线api
	CDrebMsgThread  m_pDrebSpi;//总线回调类
	CPoolDataMsg   m_pPoolData;//消息处理线程队列
	CMsgThread   m_pMsgThread;  //消息处理线程   所以收到的数据统一交给此线程处理

	CFuncTbl     m_pFuncTbl;   //功能列表

	
	CBF_BufferPool  *m_pMemPool; //数据缓冲池
	CBF_ProcessMgr  m_pMgrBpu;    //启动bpu

	CLinkThread  m_pBpuLinkThread;  //BPU连接线程
	CPuThread    m_pBuThread; //pu接收线程
	CSocketMgr m_pSockMgr;     //连接管理类 针对bpu的连接

	std::vector<CDispathThread *>  m_pDispath;//每个bpu组一个分派线程
	
	CBF_Xml      m_pBpuPidXml;

};

#endif // !defined(AFX_BPCRUN_H__1D668C64_8EE2_4658_B96B_197FF10850A5__INCLUDED_)
