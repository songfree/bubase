#if !defined(AFX_DOWNUPFILE_H__2603EEA1_5FDF_4D74_B694_DDCD0C7C9F88__INCLUDED_)
#define AFX_DOWNUPFILE_H__2603EEA1_5FDF_4D74_B694_DDCD0C7C9F88__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DownUpFile.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CDownUpFile dialog

class CDownUpFile : public CDialog
{
// Construction
public:
	int  m_nFileMode; //文件模式 0上传 1下载
	std::string  m_sLocalFileName;
	std::string  m_sRemoteFileName;
public:
	CDownUpFile(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDownUpFile)
	enum { IDD = IDD_DIALOG_FILE };
	CEdit	m_edtRemark;
	CEdit	m_edtRemote;
	CButton	m_btnLocal;
	CEdit	m_edtLocal;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDownUpFile)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDownUpFile)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnButtonLocal();
	afx_msg void OnButtonRemote();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DOWNUPFILE_H__2603EEA1_5FDF_4D74_B694_DDCD0C7C9F88__INCLUDED_)
