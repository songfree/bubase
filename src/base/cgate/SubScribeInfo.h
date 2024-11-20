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
#include "BF_Mutex.h"
//订阅信息  每个连接一个实例。
class CSubScribeInfo  
{
protected:

    CMemTableNew<S_SUBSCRIBE_TBL>     m_table;
    CPkeyUIntUnordered<2>         m_key;//功能号+key

	void Clear();
public:

	// 函数名: IsSubscribe
	// 编程  : 王明松 2014-10-22 14:35:21
	// 返回  : bool true订阅  false未订阅
	// 参数  : int funcno
	// 参数  : unsigned int key
	// 描述  : 是否订阅
	bool IsSubscribe(unsigned int funcno,unsigned int key,int ndate,int ntime);

	// 函数名: Subscribe
	// 编程  : 王明松 2014-10-22 10:55:12
	// 返回  : bool TRUE成功  false失败
	// 参数  : const char *data  传过来的订阅数据，字节序为主机序LE
	// 参数  : int datalen   订阅数据长度
	// 参数  : char *msg   失败信息
	// 描述  : 行情订阅
	bool Subscribe(const char *data,int datalen,char *msg);
	bool SubscribeAll(bool allflag,char* msg);

	int Select(std::vector<S_SUBSCRIBE_TBL*>& reslist);
	int Subscribe(unsigned int funcno, unsigned int key);
	int UnSubscribe(unsigned int funcno, unsigned int key);
	CSubScribeInfo();
	virtual ~CSubScribeInfo();
	
	CIErrlog   *m_pLog;

	int    m_nSubscribFlag;		  //1按key funcno订阅  2订阅所有
	
protected:
	CDrebEndian   m_pEndian;
	CBF_Mutex     m_mutex;
};

#endif // !defined(AFX_SUBSCRIBEINFO_H__26E08F03_B50A_4FC7_B2BA_F8A211CB2F81__INCLUDED_)
