// RegBuPathDlg.h : header file
//

#if !defined(AFX_REGBUPATHDLG_H__EC19AB51_B671_48E4_953D_9A53C3BFFE74__INCLUDED_)
#define AFX_REGBUPATHDLG_H__EC19AB51_B671_48E4_953D_9A53C3BFFE74__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CRegBuPathDlg dialog

class CRegBuPathDlg : public CDialog
{
// Construction
public:
	int QueryRegValue(struct HKEY__*ReRootKey,LPCTSTR ReSubKey,TCHAR *ReValueName, DWORD &dwType,unsigned char *content, unsigned long &dwLength, char *ErrMsg, int iErrMsgLen);
	CRegBuPathDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CRegBuPathDlg)
	enum { IDD = IDD_REGBUPATH_DIALOG };
	CEdit	m_edtDir;
	CString	m_csDir;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegBuPathDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CRegBuPathDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	afx_msg void OnButtonBrow();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGBUPATHDLG_H__EC19AB51_B671_48E4_953D_9A53C3BFFE74__INCLUDED_)
