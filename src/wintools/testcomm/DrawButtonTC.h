//***********************************************************
//               stock index system
//
//          SHANGHAI FUDAN KINGSSTAR COMPUTER CO��,LTD
//                        Department of Finance
//
// File Name        : BelbToolTC.h 
// modelNumber      : 
// modelName        : �Ի水ť
// FunctionIntroduce:
// Revision History : V1.0
// Author           : ����Ȫ     Remark:
// ChangeHistory    :
// ---------  -----------  -----------------------
// 20-10-02 ����Ȫ    CREATE
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
	UINT nBitmap;                 //λͼ��ԴID
	UINT nControl;                //��ťID
	CString sTitle;               //���ݰ�������
	CBelbToolTC BlebHelp;         //���ݰ����ؼ�
// Operations
public:
	//װ�����ݰ�����Դ
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
