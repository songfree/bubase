// IExchLink.h: interface for the CIExchLink class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IEXCHLINK_H__D49449A0_4511_4E9B_A39F_5906E6FEDE4A__INCLUDED_)
#define AFX_IEXCHLINK_H__D49449A0_4511_4E9B_A39F_5906E6FEDE4A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Xdp.h"
#include "IClientLink.h"
#include "BF_Xml.h"
#include "MemDb.h"
#include "IErrLog.h"
#include "BF_BufferPool.h"

class CIExchLink  
{
public:


	// 函数名: Send2Exch
	// 编程  : 王明松 2017-7-20 9:33:01
	// 返回  : virtual int 
	// 参数  : S_TRADE_DATA &data  
	// 参数  : CXdp *xdp   xdp报文
	// 描述  : 发给交易所，注意如果是要接收应答的，要记录报文头，应答时再把报文头带回来
	virtual int Send2Exch(S_TRADE_DATA &data,CXdp *xdp)=0;


	// 函数名: Init
	// 编程  : 王明松 2017-8-10 15:41:08
	// 返回  : virtual bool 
	// 参数  : CBF_Xml *xml
	// 描述  : 初始化
	virtual bool Init(CBF_Xml *xml,CIClientLink *pBusLink,CMemDb *pMemDb,	CXdp *pXdp,	CIErrlog *pLog,CBF_BufferPool *pMemPool)=0;

	// 函数名: Start
	// 编程  : 王明松 2017-8-10 15:39:45
	// 返回  : virtual bool 
	// 描述  : 启动交易所或柜台连接
	virtual bool Start()=0;


	// 函数名: Stop
	// 编程  : 王明松 2017-8-10 15:40:09
	// 返回  : virtual bool 
	// 描述  : 停止
	virtual bool Stop()=0;


	// 函数名: Join
	// 编程  : 王明松 2017-8-10 15:40:21
	// 返回  : virtual bool 
	// 描述  : 等待线程停止
	virtual bool Join()=0;

public:
	CIExchLink()
	{
		m_pMemDb = NULL;
		m_pLog = NULL;
		m_pMemPool = NULL;
	}
	virtual ~CIExchLink()
	{
		
	}
protected:
	CIClientLink *m_pBusLink;//发送总线的基类指针
	CMemDb       *m_pMemDb;
	CXdp          m_pXdp;
	CIErrlog      *m_pLog;
	CBF_BufferPool *m_pMemPool;//内存缓冲池

};

#endif // !defined(AFX_IEXCHLINK_H__D49449A0_4511_4E9B_A39F_5906E6FEDE4A__INCLUDED_)
