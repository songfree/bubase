// drebclient.h : main header file for the DREBCLIENT application
//

#if !defined(AFX_DREBCLIENT_H__77190A4D_8ADA_4376_8AA6_45F1F05C0B13__INCLUDED_)
#define AFX_DREBCLIENT_H__77190A4D_8ADA_4376_8AA6_45F1F05C0B13__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDrebclientApp:
// See drebclient.cpp for the implementation of this class
//

class CDrebclientApp : public CWinApp
{
public:
	CDrebclientApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDrebclientApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDrebclientApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DREBCLIENT_H__77190A4D_8ADA_4376_8AA6_45F1F05C0B13__INCLUDED_)
