//***********************************************************
//               stock index system
//
//          SHANGHAI FUDAN KINGSSTAR COMPUTER CO¡£,LTD
//                        Department of Finance
//
// File Name        : BelbToolTC.h 
// modelNumber      : 
// modelName        : ×Ô»æ°´Å¥ÆøÅÝ°ïÖú
// FunctionIntroduce:
// Revision History : V1.0
// Author           : ÕÅÓñÈª     Remark:
// ChangeHistory    :
// ---------  -----------  -----------------------
// 20-10-02 ÕÅÓñÈª    CREATE
// *********************************************************
#if !defined(AFX_BELBTOOLTC_H__681684E3_0820_4D79_9283_84D7BF9A364A__INCLUDED_)
#define AFX_BELBTOOLTC_H__681684E3_0820_4D79_9283_84D7BF9A364A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BelbToolTC.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBelbToolTC window

class CBelbToolTC : public CToolTipCtrl
{
// Construction
public:
	CBelbToolTC();

// Attributes
public:

// Operations
public:
	BOOL AddTool(UINT nID, LPCTSTR lpszText); //¼¤»îÆøÅÝ°ïÖú
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBelbToolTC)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBelbToolTC();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBelbToolTC)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BELBTOOLTC_H__681684E3_0820_4D79_9283_84D7BF9A364A__INCLUDED_)
