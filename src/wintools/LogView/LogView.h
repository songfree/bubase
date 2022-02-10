// LogView.h : main header file for the LOGVIEW application
//

#if !defined(AFX_LOGVIEW_H__94E4DE2E_9EE2_43D1_9243_149A02C787BB__INCLUDED_)
#define AFX_LOGVIEW_H__94E4DE2E_9EE2_43D1_9243_149A02C787BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CLogViewApp:
// See LogView.cpp for the implementation of this class
//

class CLogViewApp : public CWinApp
{
public:
	CLogViewApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogViewApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CLogViewApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGVIEW_H__94E4DE2E_9EE2_43D1_9243_149A02C787BB__INCLUDED_)
