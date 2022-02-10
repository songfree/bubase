// cgatetool.h : main header file for the CGATETOOL application
//

#if !defined(AFX_CGATETOOL_H__C6866EBE_80A9_48D6_82B9_1F7E2FC23896__INCLUDED_)
#define AFX_CGATETOOL_H__C6866EBE_80A9_48D6_82B9_1F7E2FC23896__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CCgatetoolApp:
// See cgatetool.cpp for the implementation of this class
//

class CCgatetoolApp : public CWinApp
{
public:
	CCgatetoolApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCgatetoolApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CCgatetoolApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CGATETOOL_H__C6866EBE_80A9_48D6_82B9_1F7E2FC23896__INCLUDED_)
