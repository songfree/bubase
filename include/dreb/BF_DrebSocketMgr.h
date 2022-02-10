// BF_DrebSocketMgr.h: interface for the CBF_DrebSocketMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BF_DREBSOCKETMGR_H__225694FE_34CF_44C1_BD8B_8E8ECCD3D84C__INCLUDED_)
#define AFX_BF_DREBSOCKETMGR_H__225694FE_34CF_44C1_BD8B_8E8ECCD3D84C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BF_DrebSocketInfo.h"
#include "BF_DrebResource.h"
#include "BF_BufferPool.h"
#include "BF_CommDataLog.h"
#include "BF_LogClient.h"



#ifdef DREBAPI_EXPORTS
#define DREBAPI_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define DREBAPI_EXPORT __declspec(dllimport)
#else
#define DREBAPI_EXPORT 
#endif
#endif

class DREBAPI_EXPORT CBF_DrebSocketMgr  
{
public:
	CBF_DrebSocketMgr();
	virtual ~CBF_DrebSocketMgr();
	// 函数名: AffirmIndex
	// 编程  : 王明松 2009-10-26 15:51:31
	// 返回  : bool 
	// 参数  : int index
	// 描述  : 索引是否正确
	bool AffirmIndex(int index);
	
	// 函数名: at
	// 编程  : 王明松 2009-10-26 15:50:10
	// 返回  : CSocketInfo * 
	// 参数  : int index
	// 描述  : 返回指定索引的连接
	CBF_DrebSocketInfo * at(int index);
	
	// 函数名: Init
	// 编程  : 王明松 2009-10-26 15:49:49
	// 返回  : bool 
	// 描述  : 连接池初始
	bool Init(CBF_DrebResource *res,CBF_BufferPool *pool,CIErrlog *perrlog,CBF_CommDataLog *pdreb2bpclog,CBF_CommDataLog *pbpc2dreblog);

	char m_errMsg[256];
	std::vector <CBF_DrebSocketInfo *>m_socketList;

};

#endif // !defined(AFX_BF_DREBSOCKETMGR_H__225694FE_34CF_44C1_BD8B_8E8ECCD3D84C__INCLUDED_)
