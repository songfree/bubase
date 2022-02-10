// SocketMgr.h: interface for the CSocketMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOCKETMGR_H__CB9B4146_5F17_46DC_A9DE_B470364E5A74__INCLUDED_)
#define AFX_SOCKETMGR_H__CB9B4146_5F17_46DC_A9DE_B470364E5A74__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "SocketInfo.h"

class  CSocketMgr  
{
public:

	/// ������: AffirmIndex
	/// ���  : ������ 2009-10-26 15:51:31
	/// ����  : bool 
	/// ����  : int index
	/// ����  : �����Ƿ���ȷ
	bool AffirmIndex(int index);

	/// ������: at
	/// ���  : ������ 2009-10-26 15:50:10
	/// ����  : CSocketInfo * 
	/// ����  : int index
	/// ����  : ����ָ������������
	CSocketInfo * at(int index);

	/// ������: Init
	/// ���  : ������ 2009-10-26 15:49:49
	/// ����  : bool 
	/// ����  : ���ӳس�ʼ
	bool Init(CTradeResource *res,CBF_BufferPool *pool);
	CSocketMgr();
	virtual ~CSocketMgr();


	char m_errMsg[256];
	std::vector <CSocketInfo *>m_socketList;
};

#endif // !defined(AFX_SOCKETMGR_H__CB9B4146_5F17_46DC_A9DE_B470364E5A74__INCLUDED_)
