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
	// ������: AffirmIndex
	// ���  : ������ 2009-10-26 15:51:31
	// ����  : bool 
	// ����  : int index
	// ����  : �����Ƿ���ȷ
	bool AffirmIndex(int index);
	
	// ������: at
	// ���  : ������ 2009-10-26 15:50:10
	// ����  : CSocketInfo * 
	// ����  : int index
	// ����  : ����ָ������������
	CBF_DrebSocketInfo * at(int index);
	
	// ������: Init
	// ���  : ������ 2009-10-26 15:49:49
	// ����  : bool 
	// ����  : ���ӳس�ʼ
	bool Init(CBF_DrebResource *res,CBF_BufferPool *pool,CIErrlog *perrlog,CBF_CommDataLog *pdreb2bpclog,CBF_CommDataLog *pbpc2dreblog);

	char m_errMsg[256];
	std::vector <CBF_DrebSocketInfo *>m_socketList;

};

#endif // !defined(AFX_BF_DREBSOCKETMGR_H__225694FE_34CF_44C1_BD8B_8E8ECCD3D84C__INCLUDED_)
