#if !defined(AFX_VARCONFIG_H__08BD1369_D82D_424A_AF3E_B966B8E1484C__INCLUDED_)
#define AFX_VARCONFIG_H__08BD1369_D82D_424A_AF3E_B966B8E1484C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VarConfig.h : header file
//
#include "MultiColumnComboBox.h"
#include "BF_Xml.h"
/////////////////////////////////////////////////////////////////////////////
// CVarConfig dialog

class CVarConfig : public CDialog
{
// Construction
public:
	CVarConfig(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CVarConfig)
	enum { IDD = IDD_DIALOG_SETVAR };
	CButton	m_btnDelValue;
	CButton	m_btnDel;
	CButton	m_btnAddValue;
	CListCtrl	m_lstVarValue;
	CListCtrl	m_lstVar;
	CEdit	m_edtVarName;
	CEdit	m_edtVarLen;
	CEdit	m_edtValue;
	CEdit	m_edtInitValue;
	CEdit	m_edtFmt;
	CMultiColumnComboBox 	m_combType;
	//}}AFX_DATA

	CBF_Xml m_xmlpack;
	CXmlNode *m_currNode;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVarConfig)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CVarConfig)
	afx_msg void OnButtonAddvalue();
	afx_msg void OnButtonDel();
	afx_msg void OnButtonDelvalue();
	afx_msg void OnClickListVar(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeComboType();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VARCONFIG_H__08BD1369_D82D_424A_AF3E_B966B8E1484C__INCLUDED_)
