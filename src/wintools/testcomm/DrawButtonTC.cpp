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
// DrawButtonTC.cpp : implementation file
//

#include "stdafx.h"
#include "DrawButtonTC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDrawButtonTC

CDrawButtonTC::CDrawButtonTC()
{
}

CDrawButtonTC::~CDrawButtonTC()
{
}


BEGIN_MESSAGE_MAP(CDrawButtonTC, CButton)
	//{{AFX_MSG_MAP(CDrawButtonTC)
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDrawButtonTC message handlers

//***********************************************************
//METHOD DECLARATION: CheckCondition
//***********************************************************
//Purpose           : �Ի��ư�ť��Դ
//
//Notes             : 
//
//***********************************************************
void CDrawButtonTC::LoadResource(UINT ControlID,LPCTSTR Title,UINT BitmapID)
{
	nBitmap = BitmapID;     //��ťλͼID
	nControl = ControlID;   //��ťID
	sTitle = Title;         //��ť���ݰ�������
	if (BitmapID!=NULL)
	{
		HICON hIcon = AfxGetApp()->LoadIcon(BitmapID);
		SetIcon(hIcon);
	}
	
	BlebHelp.Create(this);            
	BlebHelp.AddTool(nControl,sTitle);
	BlebHelp.Activate(TRUE);
}

//***********************************************************
//METHOD DECLARATION: CheckCondition
//***********************************************************
//Purpose           : �Ի��ư�ť
//
//Notes             : 
//
//***********************************************************

void CDrawButtonTC::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CRect ButRect;
	CRgn m_region;
	
	GetClientRect(&ButRect);

	//���ð�ť����
	CRect rect;
	rect.left = ButRect.left + 1;
	rect.top = ButRect.top + 1;
	rect.right = ButRect.left + ButRect.Width() - 1;
	rect.bottom = ButRect.top + ButRect.Height() - 1;

	CDC* m_pDC=GetDC();

	m_region.CreateEllipticRgnIndirect(&rect);

	
	if (m_region.PtInRegion(point) == TRUE) //������밴ť�����Ʊ߿�
	{
		m_pDC->DrawEdge(&ButRect,BDR_RAISEDINNER ,BF_RECT);
	}
	else                                   //����Ƴ���ť������߿�
	{	
		m_pDC->Draw3dRect(ButRect.left,ButRect.top,ButRect.right,ButRect.bottom,RGB(210,210,210),RGB(210,210,210));	
	}
	CButton::OnMouseMove(nFlags, point);
}
