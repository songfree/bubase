// cgatetoolDlg.h : header file
//

#if !defined(AFX_CGATETOOLDLG_H__E10E8F31_BE96_48A1_8538_76039E6DB5B2__INCLUDED_)
#define AFX_CGATETOOLDLG_H__E10E8F31_BE96_48A1_8538_76039E6DB5B2__INCLUDED_
#include "public.h"
#include "BF_Des.h"
#include "BF_Tools.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CCgatetoolDlg dialog

class CCgatetoolDlg : public CDialog
{
// Construction
public:
	CCgatetoolDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CCgatetoolDlg)
	enum { IDD = IDD_CGATETOOL_DIALOG };
	CComboBox	m_comb_mode;
	CButton	m_btnSave;
	CRichEditCtrl	m_redt_data;
	CEdit	m_edtKey;
	CEdit	m_edtDataFile;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCgatetoolDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

	char  m_sDataOld[65536];
	int   m_nDataOldLen;

	char  m_sDataNew[65536*2];
	int   m_nDataNewLen;

	char  m_sKey[17];

	CBF_Des  m_pDes;

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CCgatetoolDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	afx_msg void OnButtonBrow();
	afx_msg void OnButtonSave();
	afx_msg void OnButtonSerial();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CGATETOOLDLG_H__E10E8F31_BE96_48A1_8538_76039E6DB5B2__INCLUDED_)
