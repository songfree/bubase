// ArbMain.h: interface for the CArbMain class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARBMAIN_H__D1862398_E67A_4017_AC64_51E13AE7BEF3__INCLUDED_)
#define AFX_ARBMAIN_H__D1862398_E67A_4017_AC64_51E13AE7BEF3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ArbResource.h"
#include "DrebMsgThread.h"
#include "BF_DrebServer.h"


class CArbMain  
{
public:
	void Monitor();
	void Stop();
	bool Start();
	bool Init(const char *confile);
	CArbMain();
	virtual ~CArbMain();

	CArbResource     m_pRes;   //配置
	CBF_DrebServer   m_pDrebApi;//api
	CDrebMsgThread   m_pDrebMsgThread;//消息线程

	CIErrlog         *m_pLog;
	bool              m_bIsInit;
};

#endif // !defined(AFX_ARBMAIN_H__D1862398_E67A_4017_AC64_51E13AE7BEF3__INCLUDED_)
