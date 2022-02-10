// drebpmt.h : main header file for the DREBPMT application
//

#if !defined(AFX_DREBPMT_H__2C6C5EFB_2826_4D44_8D98_34EBAE5283ED__INCLUDED_)
#define AFX_DREBPMT_H__2C6C5EFB_2826_4D44_8D98_34EBAE5283ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDrebpmtApp:
// See drebpmt.cpp for the implementation of this class
//

class CDrebpmtApp : public CWinApp
{
public:
	CDrebpmtApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDrebpmtApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDrebpmtApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DREBPMT_H__2C6C5EFB_2826_4D44_8D98_34EBAE5283ED__INCLUDED_)
