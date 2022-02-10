// TibRcvMgr.h: interface for the CTibRcvMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIBRCVMGR_H__094F4D81_016C_40FD_AA3B_842C7FAA8FDF__INCLUDED_)
#define AFX_TIBRCVMGR_H__094F4D81_016C_40FD_AA3B_842C7FAA8FDF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "public.h"
#include "BF_Xml.h"
#include "BF_SocketUdp.h"
#include "IErrLog.h"

#include "BF_Thread.h"
#include "TibHead.h"
#include "PortTbl.h"
#include "UdpSocketInfo.h"
#include "TibRcvThread.h"
#include "BF_BufferPool.h"
#include "TibRcvQueue.h"

//接收管理 可以启多个线程来接收
#ifdef TIB_EXPORTS
#define TIB_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define TIB_EXPORT __declspec(dllimport)
#else
#define TIB_EXPORT 
#endif
#endif

class TIB_EXPORT CTibRcvMgr  
{
public:

	// 函数名: GetQueueSize
	// 编程  : 王明松 2015-6-30 12:04:08
	// 返回  : int 
	// 描述  : 取接收队列大小
	int GetQueueSize();

	// 函数名: GetData
	// 编程  : 王明松 2015-6-30 12:03:44
	// 返回  : int <0无数据  0有数据
	// 参数  : TIBCOMMSTRU &data
	// 描述  : 从接收队列里取数据
	int GetData(TIBCOMMSTRU &data);

	// 函数名: Monitor
	// 编程  : 王明松 2015-6-18 17:00:56
	// 返回  : void 
	// 描述  : 监控线程是否停止，若停止则启动
	void Monitor();

	// 函数名: Stop
	// 编程  : 王明松 2015-6-18 17:00:40
	// 返回  : void 
	// 描述  : 停止接收线程
	void Stop();

	// 函数名: Start
	// 编程  : 王明松 2015-6-18 17:00:30
	// 返回  : bool 
	// 描述  : 启动接收线程
	bool Start();

	// 函数名: Init
	// 编程  : 王明松 2015-6-18 15:56:45
	// 返回  : bool 
	// 参数  : CPortTbl *portlist
	// 参数  : CIErrlog *errlog
	// 参数  : CBF_Xml *tibxml
	// 描述  : 初始化，读取配置参数
	bool Init(CPortTbl *portlist,CIErrlog *errlog,CBF_Xml *tibxml);

	CTibRcvMgr();
	virtual ~CTibRcvMgr();

private:
	

protected:
	bool     m_bIsInit;
	int      m_nRcvThreadNum;//接收线程数

	CPortTbl   *m_pListPort;//端口列表
	CIErrlog   *m_pLog;//日志类

	CBF_BufferPool  m_pMemPool;
	CTibRcvQueue    m_pQueue;//接收队列


	std::vector<CUdpSocketInfo *> m_pRcvInfo;//要接收的端口列表，CUdpSocketInfo的index对应本vector的索引
	std::vector<CTibRcvThread *>  m_pRcvThread;//接收线程列表
		

	
};

#endif // !defined(AFX_TIBRCVMGR_H__094F4D81_016C_40FD_AA3B_842C7FAA8FDF__INCLUDED_)
