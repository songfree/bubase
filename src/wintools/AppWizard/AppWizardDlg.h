// AppWizardDlg.h : header file
//

#if !defined(AFX_APPWIZARDDLG_H__89DEBF58_9A29_4F0B_930A_8C9383FAA4BA__INCLUDED_)
#define AFX_APPWIZARDDLG_H__89DEBF58_9A29_4F0B_930A_8C9383FAA4BA__INCLUDED_
#include "MultiColumnComboBox.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CAppWizardDlg dialog

class CAppWizardDlg : public CDialog
{
// Construction
public:
	int trim(char *s);
	int GetFmtBuffer(char *filename, char *fmtbuf,int &fmtlen);
	CAppWizardDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CAppWizardDlg)
	enum { IDD = IDD_APPWIZARD_DIALOG };
	CMultiColumnComboBox	m_combType;
	CEdit	m_edtDPath;
	CEdit	m_edtDName;
	CEdit	m_edtCName;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAppWizardDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CAppWizardDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	afx_msg void OnButtonBrow();
	afx_msg void OnKillfocusEditDllName();
	afx_msg void OnChangeEditDllName();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_APPWIZARDDLG_H__89DEBF58_9A29_4F0B_930A_8C9383FAA4BA__INCLUDED_)
