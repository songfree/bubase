// GateWay.h: interface for the CGateWay class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GATEWAY_H__FCB4568A_F5CA_41F9_AB12_6F4E7ECAF5C1__INCLUDED_)
#define AFX_GATEWAY_H__FCB4568A_F5CA_41F9_AB12_6F4E7ECAF5C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "GateResource.h"
#include "LinkThread.h"
#include "SocketMgr.h"
#include "BF_AIO.h"
#include "AIO_Work.h"
#include "MsgThread.h"
#include "SendThread.h"

class CGateWay  
{
public:

	// 函数名: Monitor
	// 编程  : 王明松 2015-11-17 14:28:09
	// 返回  : void 
	// 描述  : 监控线程是否停止
	void Monitor();

	// 函数名: GetErrMsg
	// 编程  : 王明松 2015-11-17 14:28:27
	// 返回  : std::string 
	// 描述  : 取出错信息
	std::string GetErrMsg();

	// 函数名: Stop
	// 编程  : 王明松 2015-11-17 14:27:59
	// 返回  : void 
	// 描述  : 停止
	void Stop();

	// 函数名: Start
	// 编程  : 王明松 2015-11-17 14:27:55
	// 返回  : bool 
	// 描述  : 启动
	bool Start();

	// 函数名: Init
	// 编程  : 王明松 2015-11-17 14:27:42
	// 返回  : bool 
	// 参数  : const char *confile
	// 描述  : 初始化
	bool Init(const char *confile);

	CGateWay();
	virtual ~CGateWay();

	CGateResource  g_pRes;

	
protected:
	
	CBF_AIO        g_pAioRun;   //IO操作类   epoll  select 
	CAIO_Work      g_pAioWork;  //IO处理类

	CPoolData      m_pPoolData;  //客户请求队列  g_pAioWork放 m_pMsgThread取

	CMsgThread     m_pMsgThread;//客户消息处理线程   所有收到的数据统一交给此线程处理

	CLinkThread    g_pDrebLink; //dreb连接类
	CSocketMgr     m_pSockMgr; //dreb连接管理类
	CBF_BufferPool m_pMemPool; //数据缓冲池
	
	CSendData      m_pSendData;   //发送客户端数据队列   m_pSendThread取并发送 g_pAioWork放  g_pDrebLink放
	
	CSendThread    m_pSendThread[51];//发送客户端线程
	
	CIErrlog       *m_pLog; //日志指针
private:

	char m_sErrMsg[256];

};

#endif // !defined(AFX_GATEWAY_H__FCB4568A_F5CA_41F9_AB12_6F4E7ECAF5C1__INCLUDED_)
