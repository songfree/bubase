// testcomm.h : main header file for the TESTCOMM application
//

#if !defined(AFX_TESTCOMM_H__4EB24C43_97FF_45A4_83CF_74CC4B00FAFC__INCLUDED_)
#define AFX_TESTCOMM_H__4EB24C43_97FF_45A4_83CF_74CC4B00FAFC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CTestcommApp:
// See testcomm.cpp for the implementation of this class
//

class CTestcommApp : public CWinApp
{
public:
	CTestcommApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestcommApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTestcommApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTCOMM_H__4EB24C43_97FF_45A4_83CF_74CC4B00FAFC__INCLUDED_)
