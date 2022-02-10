#if !defined(AFX_VIEWERRINFO_H__A8F001C3_CE25_4377_B6EF_8C9B7B045E40__INCLUDED_)
#define AFX_VIEWERRINFO_H__A8F001C3_CE25_4377_B6EF_8C9B7B045E40__INCLUDED_

#include "BF_Date_Time.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ViewErrInfo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CViewErrInfo dialog

class CViewErrInfo : public CDialog
{
// Construction
public:
	CViewErrInfo(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CViewErrInfo)
	enum { IDD = IDD_DIALOG_ERRINFO };
	CRichEditCtrl	m_redtErr;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewErrInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CBF_Date_Time m_pDate;
	// Generated message map functions
	//{{AFX_MSG(CViewErrInfo)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWERRINFO_H__A8F001C3_CE25_4377_B6EF_8C9B7B045E40__INCLUDED_)
