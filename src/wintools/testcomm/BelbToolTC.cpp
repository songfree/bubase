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
// BelbToolTC.cpp : implementation file
//

#include "stdafx.h"
#include "BelbToolTC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBelbToolTC

CBelbToolTC::CBelbToolTC()
{
}

CBelbToolTC::~CBelbToolTC()
{
}


BEGIN_MESSAGE_MAP(CBelbToolTC, CToolTipCtrl)
	//{{AFX_MSG_MAP(CBelbToolTC)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBelbToolTC message handlers

//***********************************************************
//METHOD DECLARATION: OnTimer
//***********************************************************
//Purpose           : ¼¤»îÆøÅÝ°ïÖú
//Notes             : 
//
//***********************************************************
BOOL CBelbToolTC::AddTool(UINT nID, LPCTSTR lpszText)
{
	TOOLINFO HelpInf;

	memset(&HelpInf, 0, sizeof(TOOLINFO));

	HelpInf.lpszText = ( LPSTR )lpszText;
	HelpInf.uFlags = TTF_IDISHWND|TTF_SUBCLASS;
	HelpInf.uId = (UINT)GetParent()->GetDlgItem(nID)->m_hWnd;
	HelpInf.hwnd = GetOwner()->GetSafeHwnd();
	return (BOOL)SendMessage(TTM_ADDTOOL, 0, (LPARAM)&HelpInf );
}