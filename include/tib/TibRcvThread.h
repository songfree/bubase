// TibRcvThread.h: interface for the CTibRcvThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIBRCVTHREAD_H__063C6D75_972D_4D02_BC5E_5B4B06CAFAC7__INCLUDED_)
#define AFX_TIBRCVTHREAD_H__063C6D75_972D_4D02_BC5E_5B4B06CAFAC7__INCLUDED_


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
#include "BF_BufferPool.h"
#include "TibRcvQueue.h"

#ifdef TIB_EXPORTS
#define TIB_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define TIB_EXPORT __declspec(dllimport)
#else
#define TIB_EXPORT 
#endif
#endif

class TIB_EXPORT CTibRcvThread : public CBF_Thread  
{
public:

	// 函数名: Monitor
	// 编程  : 王明松 2015-6-26 16:19:25
	// 返回  : void 
	// 描述  : 监控线程，若停止则启动
	void Monitor();

	// 函数名: Stop
	// 编程  : 王明松 2015-6-26 16:19:19
	// 返回  : void 
	// 描述  : 停止
	void Stop();

	// 函数名: Start
	// 编程  : 王明松 2015-6-26 16:19:12
	// 返回  : bool 
	// 描述  : 启动
	bool Start();

	// 函数名: Init
	// 编程  : 王明松 2015-6-26 15:04:15
	// 返回  : bool 
	// 参数  : std::vector<CUdpSocketInfo *> *rcvinfo
	// 参数  : CIErrlog *errlog
	// 参数  : CBF_BufferPool *pool
	// 描述  : 初始化
	bool Init(std::vector<CUdpSocketInfo *> *rcvinfo, CIErrlog *errlog,CBF_BufferPool *pool,CTibRcvQueue *rcvqueue);

	// 函数名: AddIndex
	// 编程  : 王明松 2015-6-26 15:00:19
	// 返回  : int 
	// 参数  : int index
	// 描述  : 增加一个连接至要接收的列表，连接index为vector<CUdpSocketInfo *>的下标
	int AddIndex(int index);

	// 函数名: ClearIndex
	// 编程  : 王明松 2015-6-26 14:59:58
	// 返回  : void 
	// 描述  : 清空要接收的连接索引列表
	void ClearIndex();
	CTibRcvThread();
	virtual ~CTibRcvThread();

private:
	virtual int Run();

	int GetMaxfdAddSelect(int &maxfd);

	void OnRecv(int conindex);
	
	void OnEvent();

	bool   m_bIsExit;
protected:

	std::vector<CUdpSocketInfo *> *m_pRcvInfo;//要接收的端口列表
	std::vector<int> m_pRcvIndexList;//本线程需要接收连接列表,存放m_pRcvInfo里的下标

	CIErrlog *m_pLog;
	CBF_BufferPool *m_pMemPool;
	CTibRcvQueue    *m_pQueue;


	fd_set m_rset;
};

#endif // !defined(AFX_TIBRCVTHREAD_H__063C6D75_972D_4D02_BC5E_5B4B06CAFAC7__INCLUDED_)
