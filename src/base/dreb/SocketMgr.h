// SocketMgr.h: interface for the CSocketMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOCKETMGR_H__CB9B4146_5F17_46DC_A9DE_B470364E5A74__INCLUDED_)
#define AFX_SOCKETMGR_H__CB9B4146_5F17_46DC_A9DE_B470364E5A74__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "SocketInfo.h"
#include "GlobalVar.h"
#include "BF_BufferPool.h"

class CSocketMgr  
{
public:
	bool SetGlobalVar(CGlobalVar *pVar,CBF_BufferPool *pPoolbuffer);

	/// 函数名: AffirmIndex
	/// 编程  : 王明松 2009-10-26 15:51:31
	/// 返回  : bool 
	/// 参数  : int index
	/// 描述  : 索引是否正确
	bool AffirmIndex(int index);

	/// 函数名: at
	/// 编程  : 王明松 2009-10-26 15:50:10
	/// 返回  : CSocketInfo * 
	/// 参数  : int index
	/// 描述  : 返回指定索引的连接
	CSocketInfo * at(int index);

	/// 函数名: Init
	/// 编程  : 王明松 2009-10-26 15:49:49
	/// 返回  : bool 
	/// 参数  : int socketCount  最大的连接数
	/// 描述  : 连接池初始
	bool Init(int socketCount);
	CSocketMgr();
	virtual ~CSocketMgr();


	char m_errMsg[256];
	std::vector <CSocketInfo *>m_socketList;

protected:

	CGlobalVar *m_pRes;
	CBF_BufferPool *m_pMemPool;

};

#endif // !defined(AFX_SOCKETMGR_H__CB9B4146_5F17_46DC_A9DE_B470364E5A74__INCLUDED_)
