#if !defined(AFX_RESULTVIEW_H__36C91A68_C645_48C6_8057_3B1F4706D0A9__INCLUDED_)
#define AFX_RESULTVIEW_H__36C91A68_C645_48C6_8057_3B1F4706D0A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ResultView.h : header file
//
#include "resource.h"
#include "Xdp.h"
/////////////////////////////////////////////////////////////////////////////
// CResultView dialog

class CResultView : public CDialog
{
// Construction
public:
	char *m_pDataPoint;
	int  m_nDataLen;
	CXdp  *m_pXdp;
	CResultView(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CResultView)
	enum { IDD = IDD_DIALOG_RESULT };
	CButton	m_btnTitle;
	CRichEditCtrl	m_redt_result;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResultView)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CResultView)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESULTVIEW_H__36C91A68_C645_48C6_8057_3B1F4706D0A9__INCLUDED_)
