// SubScribeInfo.h: interface for the CSubScribeInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SUBSCRIBEINFO_H__26E08F03_B50A_4FC7_B2BA_F8A211CB2F81__INCLUDED_)
#define AFX_SUBSCRIBEINFO_H__26E08F03_B50A_4FC7_B2BA_F8A211CB2F81__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "MdbBase.h"
#include "DrebEndian.h"
#include "GateHead.h"
#include "IErrLog.h"
//订阅信息  每个连接一个实例。

class CSubScribeInfo  
{
public:

	// 函数名: IsQuoSubscribe
	// 编程  : 王明松 2014-10-22 14:35:21
	// 返回  : bool true订阅  false未订阅
	// 参数  : int varietycode
	// 描述  : 判断此编号合约的行情是否订阅
	bool IsQuoSubscribe(int varietycode);

	// 函数名: Subscribe
	// 编程  : 王明松 2014-10-22 10:55:12
	// 返回  : bool TRUE成功  false失败
	// 参数  : const char *data  传过来的订阅数据，字节序为主机序LE
	// 参数  : int datalen   订阅数据长度
	// 参数  : char *msg   失败信息
	// 描述  : 行情订阅
	bool QuoSubscribe(const char *data,int datalen,char *msg);

	CSubScribeInfo();
	virtual ~CSubScribeInfo();
	
	CIErrlog   *m_pLog;
	
protected:
	int   m_nQuoSubcribeFlag;  //行情订阅标志 0 取消订阅   1按合约编号订阅   2按市场订阅
	unsigned int  m_nQuoDataNum; //当flag为1时，则为合约数量   当为2时，为市场代码
	CInt          m_listVariety; //订阅的行情合约列表

	CDrebEndian   m_pEndian;
};

#endif // !defined(AFX_SUBSCRIBEINFO_H__26E08F03_B50A_4FC7_B2BA_F8A211CB2F81__INCLUDED_)
