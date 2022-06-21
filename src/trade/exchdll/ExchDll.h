// ExchDll.h: interface for the CExchDll class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXCHDLL_H__7100EC87_64FF_419A_ACB3_1AD52392DAE0__INCLUDED_)
#define AFX_EXCHDLL_H__7100EC87_64FF_419A_ACB3_1AD52392DAE0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _WINDOWS
#ifdef _DEBUG
#pragma comment(lib, "bf_kerneld.lib") 
#pragma comment(lib, "bf_xdpd.lib") 
#pragma message("Automatically linking with   bf_kerneld.lib  bf_xdpd.lib")
#else
#pragma comment(lib, "bf_kernel.lib") 
#pragma comment(lib, "bf_xdp.lib") 
#pragma message("Automatically linking with   bf_kernel.lib  bf_xdp.lib")
#endif

#endif


#include "IExchLink.h"


class DLL_EXPORT CExchDll : public CIExchLink  
{
public:
	CExchDll();
	virtual ~CExchDll();

	// 函数名: Send2Exch
	// 编程  : 王明松 2017-7-20 9:33:01
	// 返回  : virtual int 
	// 参数  : S_TRADE_DATA &data  
	// 参数  : CXdp *xdp   xdp报文
	// 描述  : 发给交易所 
	virtual int Send2Exch(S_TRADE_DATA &data,CXdp *xdp);
	
	
	// 函数名: Init
	// 编程  : 王明松 2017-8-10 15:41:08
	// 返回  : virtual bool 
	// 参数  : CBF_Xml *xml
	// 描述  : 初始化
	virtual bool Init(CBF_Xml *xml,CIClientLink *pBusLink,CMemDb *pMemDb,	CXdp *pXdp,	CIErrlog *pLog);
	
	// 函数名: Start
	// 编程  : 王明松 2017-8-10 15:39:45
	// 返回  : virtual bool 
	// 描述  : 启动交易所或柜台连接
	virtual bool Start();
	
	
	// 函数名: Stop
	// 编程  : 王明松 2017-8-10 15:40:09
	// 返回  : virtual bool 
	// 描述  : 停止
	virtual bool Stop();
	
	
	// 函数名: Join
	// 编程  : 王明松 2017-8-10 15:40:21
	// 返回  : virtual bool 
	// 描述  : 等待线程停止
	virtual bool Join();



	char m_errMsg[256];

};
extern "C" 
{ 
    DLL_EXPORT CExchDll * getInstance() ; 
}
#endif // !defined(AFX_EXCHDLL_H__7100EC87_64FF_419A_ACB3_1AD52392DAE0__INCLUDED_)
