#if !defined(AFX_DREBLOGDATA_H__F126E429_EFAD_40C7_904B_AB1499D26E27__INCLUDED_)
#define AFX_DREBLOGDATA_H__F126E429_EFAD_40C7_904B_AB1499D26E27__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DrebLogData.h : header file
//
#include "drebpubdefine.h"
#include "EncryptCommBuffer.h"
#include "BF_SimpleLog.h"
#include "Xdp.h"
/////////////////////////////////////////////////////////////////////////////
// CDrebLogData dialog

class CDrebLogData : public CDialog
{
// Construction
public:
	CDrebLogData(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDrebLogData)
	enum { IDD = IDD_DIALOG_DATA };
	CEdit	m_edtZip;
	CEdit	m_edtDataLen;
	CRichEditCtrl	m_redtData;
	//}}AFX_DATA

	char m_sData[65534];
	int  m_nDataLen;

	DREB_HEAD    m_head; 

	COMMSTRU m_data;
	
	CBF_SimpleLog  m_pLog;
	CEncryptCommBuffer  m_pDesZip;
	CXdp  m_pXdp;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDrebLogData)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDrebLogData)
	virtual BOOL OnInitDialog();
	afx_msg void OnSave();
	afx_msg void OnSaveData();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DREBLOGDATA_H__F126E429_EFAD_40C7_904B_AB1499D26E27__INCLUDED_)
