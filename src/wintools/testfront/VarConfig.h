#if !defined(AFX_VARCONFIG_H__9C6BCE75_D7F5_4274_9E3D_575733FD64E9__INCLUDED_)
#define AFX_VARCONFIG_H__9C6BCE75_D7F5_4274_9E3D_575733FD64E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VarConfig.h : header file
//
#include "MultiColumnComboBox.h"
#include "XmlPack.h"
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
	CButton	m_combDel;
	CEdit	m_edtInitValue;
	CButton	m_combAddValue;
	CListCtrl	m_lstVarValue;
	CListCtrl	m_lstVar;
	CEdit	m_edtVarName;
	CEdit	m_edtVarLen;
	CEdit	m_edtValue;
	CEdit	m_edtFmt;
	CMultiColumnComboBox	m_combType;
	//}}AFX_DATA
	CXmlPack m_xmlpack;
	CNode *m_currNode;
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
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboType();
	afx_msg void OnClickListVar(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonAddvalue();
	afx_msg void OnButtonDelvalue();
	virtual void OnOK();
	afx_msg void OnButtonDel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VARCONFIG_H__9C6BCE75_D7F5_4274_9E3D_575733FD64E9__INCLUDED_)
