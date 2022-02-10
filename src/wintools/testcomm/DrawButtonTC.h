//***********************************************************
//               stock index system
//
//          SHANGHAI FUDAN KINGSSTAR COMPUTER CO。,LTD
//                        Department of Finance
//
// File Name        : BelbToolTC.h 
// modelNumber      : 
// modelName        : 自绘按钮
// FunctionIntroduce:
// Revision History : V1.0
// Author           : 张玉泉     Remark:
// ChangeHistory    :
// ---------  -----------  -----------------------
// 20-10-02 张玉泉    CREATE
// *********************************************************
#if !defined(AFX_DRAWBUTTONTC_H__0FF659BA_89BB_49C0_933A_68FDF7EC3F54__INCLUDED_)
#define AFX_DRAWBUTTONTC_H__0FF659BA_89BB_49C0_933A_68FDF7EC3F54__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DrawButtonTC.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDrawButtonTC window
#include "BelbToolTC.h"
class CDrawButtonTC : public CButton
{
// Construction
public:
	CDrawButtonTC();

// Attributes
public:
	UINT nBitmap;                 //位图资源ID
	UINT nControl;                //按钮ID
	CString sTitle;               //气泡帮助内容
	CBelbToolTC BlebHelp;         //气泡帮助控件
// Operations
public:
	//装载气泡帮助资源
	void LoadResource(UINT ControlID, LPCTSTR Title, UINT BitmapID);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDrawButtonTC)
	public:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDrawButtonTC();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDrawButtonTC)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DRAWBUTTONTC_H__0FF659BA_89BB_49C0_933A_68FDF7EC3F54__INCLUDED_)
