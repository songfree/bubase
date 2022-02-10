#if !defined(AFX_FILEXDP_H__47A1EB8E_9E7D_4AC9_9AC1_356997CA976E__INCLUDED_)
#define AFX_FILEXDP_H__47A1EB8E_9E7D_4AC9_9AC1_356997CA976E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileXdp.h : header file
//
#include "resource.h"
#include "Xdp.h"
/////////////////////////////////////////////////////////////////////////////
// CFileXdp dialog

class CFileXdp : public CDialog
{
// Construction
public:
	CFileXdp(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFileXdp)
	enum { IDD = IDD_DIALOG_XDP };
	CEdit	m_edtValue;
	CEdit	m_edtFld;
	CEdit	m_edtFile;
	CListCtrl	m_lstXdpDefine;
	CListCtrl	m_lstXdp;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileXdp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	bool UpdateLstXdp(CXdp *xdp);
	CXdp  *m_pXdp;
	char  m_xdpData[DREBDATASIZE];
	unsigned int   m_nXdpDataLen;


	CXdp  m_pDataXdp;

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFileXdp)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonDelete();
	afx_msg void OnButtonSave();
	afx_msg void OnButtonOpen();
	afx_msg void OnDblclkListXdpdefine(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickListXdp(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILEXDP_H__47A1EB8E_9E7D_4AC9_9AC1_356997CA976E__INCLUDED_)
