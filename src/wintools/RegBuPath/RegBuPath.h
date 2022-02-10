// RegBuPath.h : main header file for the REGBUPATH application
//

#if !defined(AFX_REGBUPATH_H__69366469_02A8_4789_B283_F27C44BCE7F5__INCLUDED_)
#define AFX_REGBUPATH_H__69366469_02A8_4789_B283_F27C44BCE7F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CRegBuPathApp:
// See RegBuPath.cpp for the implementation of this class
//

class CRegBuPathApp : public CWinApp
{
public:
	CRegBuPathApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegBuPathApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CRegBuPathApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGBUPATH_H__69366469_02A8_4789_B283_F27C44BCE7F5__INCLUDED_)
