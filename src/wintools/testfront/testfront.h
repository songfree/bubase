// testfront.h : main header file for the TESTFRONT application
//

#if !defined(AFX_TESTFRONT_H__F9501C72_ACEB_4333_8808_B27599950DE8__INCLUDED_)
#define AFX_TESTFRONT_H__F9501C72_ACEB_4333_8808_B27599950DE8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CTestfrontApp:
// See testfront.cpp for the implementation of this class
//

class CTestfrontApp : public CWinApp
{
public:
	CTestfrontApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestfrontApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTestfrontApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTFRONT_H__F9501C72_ACEB_4333_8808_B27599950DE8__INCLUDED_)
