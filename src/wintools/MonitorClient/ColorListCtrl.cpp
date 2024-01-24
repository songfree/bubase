// Author:      SFr frydaysoft@gmx.de

#include "stdafx.h"
#include "ColorListCtrl.h"


#include "stdafx.h"
#include "ColorListCtrl.h"


// CColorListCtrl

IMPLEMENT_DYNAMIC(CColorListCtrl, CListCtrl)

CColorListCtrl::CColorListCtrl()
{

}

CColorListCtrl::~CColorListCtrl()
{
    POSITION   pos = m_lstItemColor.GetHeadPosition();
    while (pos)
    {
        delete[]m_lstItemColor.GetNext(pos);
    }
}


BEGIN_MESSAGE_MAP(CColorListCtrl, CListCtrl)
END_MESSAGE_MAP()


void   CColorListCtrl::PreSubclassWindow()
{
    ModifyStyle(0, LVS_OWNERDRAWFIXED | LVS_REPORT, 0);

    CListCtrl::PreSubclassWindow();
}

void   CColorListCtrl::DrawItem(LPDRAWITEMSTRUCT   lpDIS)
{
    CDC* pDC = CDC::FromHandle(lpDIS->hDC);
    int   nRows = m_lstItemColor.GetCount();
    COLORREF* clrCol = NULL;
    if (nRows && lpDIS->itemID < nRows)
    {
        clrCol = m_lstItemColor.GetAt(m_lstItemColor.FindIndex(lpDIS->itemID));
    }
    CRect   rcItem(lpDIS->rcItem);
    LVCOLUMN lvColumn;
    ZeroMemory(&lvColumn, sizeof(LVCOLUMN));
    lvColumn.mask = LVCF_FMT | LVCF_WIDTH;
    for (int nCol = 0; nCol < GetHeaderCtrl()->GetItemCount(); nCol++)
    {
        if (clrCol)
            pDC->SetTextColor(clrCol[nCol]);
        GetColumn(nCol, &lvColumn);
        UINT   uStyle = DT_SINGLELINE | DT_END_ELLIPSIS;
        if (lvColumn.fmt & LVCFMT_LEFT)
            uStyle |= DT_LEFT;
        if (lvColumn.fmt & LVCFMT_RIGHT)
            uStyle |= DT_RIGHT;
        if (lvColumn.fmt & LVCFMT_CENTER)
            uStyle |= DT_CENTER;
        rcItem.right = rcItem.left + lvColumn.cx;
        pDC->DrawText(GetItemText(lpDIS->itemID, nCol), rcItem, uStyle);
        rcItem.left += lvColumn.cx;
    }
}


COLORREF   CColorListCtrl::SetItemColor(int   nRow, int   nCol, COLORREF   clrItem)
{
    int   nCols = GetHeaderCtrl()->GetItemCount();
    int   nRows = GetItemCount();
    //if   (nCol   > =   nCols   ||   nRow   > =   nRows) 
    //return   COLORREF(-1); 

    for (int i = m_lstItemColor.GetCount(); i < nRow + 1; i++)
    {
        COLORREF* clrCol = new   COLORREF[nCols];
        ZeroMemory(clrCol, sizeof(clrCol));
        m_lstItemColor.AddTail(clrCol);
    }

    for (int i = 0; i < nRows; i++)
    {
        if (i == nRow)
        {
            COLORREF* clrCol = m_lstItemColor.GetAt(m_lstItemColor.FindIndex(nRow));
            COLORREF   clrRet = clrCol[nCol];
            clrCol[nCol] = clrItem;
            return   clrRet;
        }
    }

    return   COLORREF(-1);
}


COLORREF   CColorListCtrl::SetItemColor(LVITEM* plvItem, COLORREF   clrItem)
{
    if (!plvItem)
    {
        return   COLORREF(0);
    }
    int   nRow = plvItem->iItem;
    int   nCol = plvItem->iSubItem;
    return   SetItemColor(nCol, nRow, clrItem);

    return   COLORREF(-1);
}
//
//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif
//
//
///////////////////////////////////////////////////////////////////////////////
//// CColorListCtrl
//
//CColorListCtrl::CColorListCtrl(int colSize)
//{
//    m_columnSize=colSize;       // number of columns
//    m_fullColumnLines=TRUE;     // paint vert. lines | | |
//    m_withSingleRect=FALSE;     // paint single rect for all sub-items
//#ifdef _DEBUG
//    m_BigFocusRect=TRUE;        // focus-rect with small/big dots
//#endif
//#ifndef _DEBUG
//    m_BigFocusRect=FALSE;       // focus-rect with small/big dots
//#endif
//    IsCreated=FALSE;
//}
//
////***************************************************************
//CColorListCtrl::~CColorListCtrl()
//{
//}
//
////***************************************************************
//BEGIN_MESSAGE_MAP(CColorListCtrl, CListCtrl)
//	//{{AFX_MSG_MAP(CColorListCtrl)
//	ON_WM_PAINT()
//	ON_WM_DESTROY()
//	ON_WM_KILLFOCUS()
//	ON_WM_SETFOCUS()
//	ON_WM_SIZE()
//	//}}AFX_MSG_MAP
//END_MESSAGE_MAP()
//
///////////////////////////////////////////////////////////////////////////////
//// Behandlungsroutinen für Nachrichten CColorListCtrl 
//void CColorListCtrl::OnDestroy() 
//{
//	DeleteAllItems();	
//	CListCtrl::OnDestroy();
//}
//
////***************************************************************
//void CColorListCtrl::SetColumnType(BOOL bNew)
//{
//    m_fullColumnLines=bNew;     // paint no/vert. lines | | |
//    if (IsCreated) 
//    {
//        RedrawWindow();
//    }
//}
//void CColorListCtrl::SetRectType(BOOL bNew)
//{
//    m_withSingleRect=bNew;     // paint no/single rect for all sub-items
//    if (IsCreated)
//    {
//        RedrawWindow();
//    }
//}
//void CColorListCtrl::SetFocusType(BOOL bNew)
//{
//    m_BigFocusRect=bNew;       // focus-rect with small/big dots
//    if (IsCreated)
//    {
//        RedrawWindow();
//    }
//}
//
////***************************************************************
//CMyLParam*	CColorListCtrl::GetMyLParam(int Index)
//{
//    LVITEM	Item;
//	Item.mask		= LVIF_PARAM;
//	Item.iItem		= Index;
//	Item.iSubItem	= 0;
//	CListCtrl::GetItem(&Item);
//	return (CMyLParam*)Item.lParam;
//}
//
////***************************************************************
//CRect CColorListCtrl::GetFullCellRect(int nItem, BOOL outside)
//{
//	static CRect rectCell;
//    CListCtrl::GetItemRect(nItem, rectCell, LVIR_BOUNDS);
//    if (!outside)
//    {   //decrease 1 pixel
//        rectCell.left++;    rectCell.right--;
//        rectCell.top++;     rectCell.bottom--;
//    }
//	return rectCell;
//}
//
////***************************************************************
//CRect CColorListCtrl::GetCellRect(int nItem, int nColumn, BOOL outside)
//{
//	static CRect rectCell;
//
//	// Get the height
//    CListCtrl::GetItemRect(nItem, rectCell, LVIR_BOUNDS);
//    LV_COLUMN column;
//	column.mask = LVCF_WIDTH;        // Set the mask to only return the width
//
//    for (int x=0; x<nColumn; x++)
//    {
//        column.iSubItem=1;
//        CListCtrl::GetColumn(x, &column); // Get rect-edge (left/top)
//        rectCell.left+=column.cx;
//    }
//    column.iSubItem=1;
//    CListCtrl::GetColumn(nColumn, &column); // Get rect-edge (right/bottom)
//    rectCell.right = rectCell.left + column.cx;
//
//    if (!outside)
//    {   //decrease 1 pixel
//        rectCell.left++;    rectCell.right--;
//        rectCell.top++;     rectCell.bottom--;
//    }
//	return rectCell;
//}
//
////***************************************************************
//void CColorListCtrl::InvalidateFullItem(int nItem, BOOL bErase)
//{
//    CRect rcBounds=GetFullCellRect(nItem, TRUE);
//	InvalidateRect(rcBounds, bErase);
//
//}
//
////***************************************************************
//// create ListCtrl-head
//int CColorListCtrl::InitCtrl(CStringArray *phead, CByteArray *pProcent)
//{
//    if ( (!phead)||(!pProcent)) return 1;
//	LV_COLUMN lvc;
//	int cx=0;
//    int count = phead->GetSize(); 
//    if (count!=m_columnSize) return 2;
//    if (pProcent->GetSize() != count) return 3;
//    for (int x=0; x<count; x++) cx+=pProcent->GetAt(x);
//    if (cx!=100) return 4;
//
//	CRect rect_listbox;
//
//	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
//	GetWindowRect( rect_listbox );
//	cx=int((rect_listbox.right-rect_listbox.left)-20); //some Pixel for Scrollbar
//
//    //set head-columns in ListBox
//	lvc.iSubItem = 0;
//	lvc.fmt = LVCFMT_LEFT ;		
//	
//    for (int x=0; x<count; x++)
//    {
//        lvc.cx = int(cx*pProcent->GetAt(x)/100);
//	    lvc.pszText = (char*)LPCSTR(phead->GetAt(x));
//	    CListCtrl::InsertColumn(x,&lvc);
//    }
//    return 0;
//}
//
//
////***************************************************************
//// add an item
//int CColorListCtrl::AddItem(CStringArray *line, int pos)
//{
//    int ret_index=LISTCTRL_ERROR;
//    if (!line) return ret_index;
//
//	LV_ITEM lvi;
//    
//	//set List-properties
//	lvi.mask = LVIF_TEXT ; 
//	lvi.iSubItem = 0;
//    lvi.pszText = "";
//
//    int count=GetItemCount();
//    int columns=line->GetSize();
//
//    if (m_columnSize!=columns) return ret_index;
//
//    if (DWORD(pos)<=DWORD(count)) count=pos;
//	
//    lvi.iItem = count;
//    //lvi.lParam= count;
//	lvi.mask = LVIF_TEXT ; 
//	lvi.iSubItem = 0;   //refer to no Item
//    // insert an new line
//    ret_index=InsertItem(&lvi);
//    // set item text for additional columns		
//    for (int x=0; x<columns; x++)
//    {
//        CListCtrl::SetItemText(count,x,(char*)LPCSTR(line->GetAt(x)) );
//    }
//	lvi.mask = LVIF_STATE;
//	lvi.iSubItem = 1;
//	lvi.stateMask = 0xFFFFFFFF ; 
//	lvi.state = 0;
//
//	if (CListCtrl::SetItem(&lvi)) 
//    {
//        CMyLParam *p=new CMyLParam(m_columnSize); //init with: DEF_BACKGROUND, def_text
//
//        #ifdef TESTCOLOR
//            // only for TEST
//            if (ret_index<3)
//            {
//                p->SetBackColor(ITEM_COLOR(ret_index+BLUE),0);
//                p->SetTextColor(ITEM_COLOR(ret_index+RED ),0);
//            }
//            else
//            if (ret_index<=6)
//            {
//                p->SetBackColor(ITEM_COLOR(ret_index+BLUE),1);
//                p->SetTextColor(ITEM_COLOR(ret_index+RED ),1);
//            }
//            else
//            {
//                p->SetBackColor(ITEM_COLOR((ret_index+BLUE)&0xF),2);
//                p->SetTextColor(ITEM_COLOR((ret_index+RED )&0xF),2);
//            }
//            // end: only for TEST
//        #endif
//
//        CListCtrl::SetItemData( ret_index, DWORD(p) );
//        return ret_index;
//    }
//    return LISTCTRL_ERROR;
//}
//
////***************************************************************
//BOOL CColorListCtrl::DeleteAllItems()
//{
//	while (CListCtrl::GetItemCount())
//        CColorListCtrl::DeleteItem(0);
//
//    return TRUE;
//}
//
////***************************************************************
//BOOL CColorListCtrl::DeleteItem(int nItem )
//{
//    if (CListCtrl::GetItemCount()<=nItem) return FALSE;
//    CMyLParam *p=(CMyLParam*)CListCtrl::GetItemData(nItem);
//    if (p)
//    {
//        CListCtrl::SetItemData( nItem, p->GetData() );
//        delete p;
//        return CListCtrl::DeleteItem( nItem );
//    }
//    return TRUE;
//}
//
//
////***************************************************************
//DWORD CColorListCtrl::GetItemData( int nItem ) const
//{
//    CMyLParam *p=(CMyLParam*)CListCtrl::GetItemData(nItem);
//    if (p)
//    {
//        return p->GetData();
//        return TRUE;
//    }
//    return 0;
//}
//
////***************************************************************
//BOOL  CColorListCtrl::SetItemData( int nItem, DWORD dwData )
//{
//    CMyLParam *p=(CMyLParam*)CListCtrl::GetItemData(nItem);
//    if (p)
//    {
//        p->SetData(dwData);
//        return TRUE;
//    }
//    return FALSE;
//}
//
////***************************************************************
//BOOL CColorListCtrl::SetItemBackgndColor(ITEM_COLOR color, int Index, int iSub)	
//{
//    CMyLParam *p=GetMyLParam(Index);
//    if (!p) return FALSE;
//    if ( p->SetBackColor(color, iSub) )
//    {
//        RedrawItems( Index, Index );
//        return TRUE;
//    }
//    return FALSE;
//}
//
////***************************************************************
//BOOL CColorListCtrl::SetItemTextColor(ITEM_COLOR color, int Index, int iSub)	
//{
//    CMyLParam *p=GetMyLParam(Index);
//    if (!p) return FALSE;
//    if ( p->SetTextColor(color, iSub) )
//    {
//        RedrawItems( Index, Index );
//        return TRUE;
//    }
//    return FALSE;
//}
//
////***************************************************************
//COLORREF CColorListCtrl::GetItemBackgndColor(int Index, int iSub)
//{
//    CMyLParam *p=GetMyLParam(Index);
//    if (!p) return 0;
//    return p->GetBackColor(iSub);
//}
//
////***************************************************************
//COLORREF CColorListCtrl::GetItemTextColor(int Index, int iSub)
//{
//    CMyLParam *p=GetMyLParam(Index);
//    if (!p) return 0;
//    return p->GetTextColor(iSub);
//}
//
////***************************************************************
//void CColorListCtrl::OnPaint() 
//{
//	// First let the control do its default drawing.
//	const MSG *msg = GetCurrentMessage();
//	DefWindowProc( msg->message, msg->wParam, msg->lParam );
//
//    if (!m_fullColumnLines) return;
//
//	// Draw the lines only for LVS_REPORT mode
//	if( (GetStyle() & LVS_TYPEMASK) == LVS_REPORT )
//	{
//		// Get the number of columns
//		CClientDC dc(this );
//		CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
//		int nColumnCount = pHeader->GetItemCount();
//
//		// The bottom of the header corresponds to the top of the line 
//		RECT rect;
//		pHeader->GetClientRect( &rect );
//		int top = rect.bottom;
//
//		// Now get the client rect so we know the line length and
//		// when to stop
//		GetClientRect( &rect );
//
//		// The border of the column is offset by the horz scroll
//		int borderx = 0 - GetScrollPos( SB_HORZ );
//
//        CPen       *pOldPen;
//        CPen        pen;
//        CGdiObject *pOldBrush;
//        pen.CreatePen( PS_DOT, 0, GetColorRef(DEF_DESELTEXT) );
//        pOldPen  =dc.SelectObject(&pen);
//        pOldBrush=dc.SelectStockObject(NULL_BRUSH);
//
//		for( int i = 0; i < nColumnCount; i++ )
//		{
//			// Get the next border
//			borderx += GetColumnWidth( i );
//
//			// if next border is outside client area, break out
//			if( borderx >= rect.right ) break;
//
//			// Draw the line.
//			dc.MoveTo( borderx-1, top);
//			dc.LineTo( borderx-1, rect.bottom );
//		}
//        dc.SelectObject(pOldPen);
//        dc.SelectObject(pOldBrush);
//
//	}
//	// Do not call CListCtrl::OnPaint() for painting messages
//}
//
////***************************************************************
//void CColorListCtrl::RepaintSelectedItems()
//{
//	// Invalidate focused item so it can repaint 
//	int nItem = GetNextItem(-1, LVNI_FOCUSED);
//
//	if(nItem != -1)
//	{
//        InvalidateFullItem(nItem, FALSE);
//	}
//
//	// Invalidate selected items depending on LVS_SHOWSELALWAYS
//	if((GetStyle() & LVS_SHOWSELALWAYS))
//	{
//		for(nItem = GetNextItem(-1, LVNI_SELECTED);
//			nItem != -1; nItem = GetNextItem(nItem, LVNI_SELECTED))
//		{
//            InvalidateFullItem(nItem, FALSE);
//		}
//	}
//	UpdateWindow();
//}
//
////***************************************************************
//BOOL CColorListCtrl::SetSel(int pos, BOOL bSelect)
//{
//    // LVS_ES_FULLROWSELECT only works with newer version of Comctl32
//    // This style and some of the other new extended styles 
//    // only work with version 4.72 (or higer) of COMCTL32.DLL 
//
//    UINT nState=0;
//    if (bSelect) nState=LVIS_SELECTED|LVS_EX_FULLROWSELECT; else nState=0;
//    if (SetItemState(pos, nState, LVIS_SELECTED|LVS_EX_FULLROWSELECT))
//    {
//        if((GetStyle() & LVS_SHOWSELALWAYS))
//        {
//            InvalidateFullItem(pos, TRUE);
//        }
//        return TRUE;
//    }
//    return FALSE;
//}
////***************************************************************
//void CColorListCtrl::OnKillFocus(CWnd* pNewWnd) 
//{
//	SendMessage(WM_PAINT);
//    CListCtrl::OnKillFocus(pNewWnd);
//	// check if we are losing focus
//	if(pNewWnd != NULL && pNewWnd->GetParent() == this)
//		return;
//
//	// repaint items that should change appearance
//	if((GetStyle() & LVS_TYPEMASK) == LVS_REPORT)
//		RepaintSelectedItems();
//}
//
////***************************************************************
//void CColorListCtrl::OnSetFocus(CWnd* pOldWnd) 
//{
//	CListCtrl::OnSetFocus(pOldWnd);
//	
//	// check if we are getting focus
//	if(pOldWnd!=NULL && pOldWnd->GetParent()==this)
//		return;
//
//	// repaint items that should change appearance
//	if((GetStyle() & LVS_TYPEMASK)==LVS_REPORT)
//		RepaintSelectedItems();
//}
//
////***************************************************************
//void CColorListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
//{
//    if (!lpDrawItemStruct) return;
//    IsCreated=TRUE;
//	CDC*		pDC			= CDC::FromHandle(lpDrawItemStruct->hDC);
//	CMyLParam*	pMyLParam	= GetMyLParam(lpDrawItemStruct->itemID);
//	CRect		rcItem;
//
//    if (!pMyLParam) return;
//    int x=0;
//    typedef enum {R_NOTHINGS=0, R_FOCUS, R_SELECT, R_NORMAL, R_SEL_NOFOCUS} redrawmode;
//    redrawmode redraw=R_NORMAL;
//
//	// Get item image and state info
//	LV_ITEM lvi;
//	lvi.mask = LVIF_IMAGE | LVIF_STATE;
//	lvi.iItem = lpDrawItemStruct->itemID;
//	lvi.iSubItem = 0;
//	lvi.stateMask = 0xFFFF;		// get all state flags
//	GetItem(&lvi);
//
//	BOOL bHighlight =   (   (lvi.state & LVIS_DROPHILITED)
//                            ||
//				            (
//                                (lvi.state & LVIS_SELECTED) 
//                                && 
//                                (  ( GetFocus() == this )  || ( GetStyle() & LVS_SHOWSELALWAYS ) )
//					        )
//				        );
//
//    BOOL bLostFocus=(GetFocus()!=this);
//
//
//    if ( (lpDrawItemStruct->itemAction&ODA_FOCUS)==ODA_FOCUS)
//    {   //the control gains or loses input focus
//        if (bLostFocus) redraw=R_SEL_NOFOCUS;
//        else
//        if ((lvi.state&LVIS_FOCUSED)==LVIS_FOCUSED) redraw=R_FOCUS;
//        else 
//        {
//            if (bHighlight) redraw=R_SELECT;
//            else redraw=R_NORMAL;
//        }
//    }
//    else
//    if ((lpDrawItemStruct->itemAction&ODA_SELECT)==ODA_SELECT)
//    {   //the selection status has changed
//            if (bHighlight)
//            {
//                if (bLostFocus) redraw=R_SEL_NOFOCUS;
//                 else redraw=R_SELECT;
//            }
//            else redraw=R_NORMAL;
//    }
//    else
//    {   //redraw the item
//        if (bLostFocus)
//        {
//            if (bHighlight) redraw=R_SEL_NOFOCUS;
//             else
//              redraw=R_NORMAL;
//        }
//        else 
//        {
//            if ((lvi.state&LVIS_FOCUSED)==LVIS_FOCUSED) redraw=R_FOCUS;
//            else 
//            {
//                if (bHighlight) redraw=R_SELECT;
//                 else redraw=R_NORMAL;
//            }
//        }
//    }
//
//    CPen       *pOldPen;
//    CPen        pen;
//    CGdiObject *pOldBrush;
//
//    switch(redraw)
//    {
//
//        case R_FOCUS:
//            rcItem=GetFullCellRect(lpDrawItemStruct->itemID, TRUE);
//            pDC->FillSolidRect(rcItem, GetColorRef(DEF_SELBACKGROUND));
//            pDC->SetTextColor(GetColorRef(DEF_SELTEXT));
//            if (m_BigFocusRect)
//            {
//                pen.CreatePen( PS_DOT, 0, GetColorRef(DEF_SELTEXT) );
//                pOldPen  =pDC->SelectObject(&pen);
//                pOldBrush=pDC->SelectStockObject(NULL_BRUSH);
//                pDC->Rectangle(rcItem);
//                pDC->SelectObject(pOldPen);
//                pDC->SelectObject(pOldBrush);
//            }
//            else pDC->DrawFocusRect(rcItem);
//            for (x=0; x<m_columnSize; x++)
//            {
//                rcItem	= GetCellRect(lpDrawItemStruct->itemID, x, FALSE);
//                pDC->DrawText(CListCtrl::GetItemText(lpDrawItemStruct->itemID,x), rcItem, DT_SINGLELINE | DT_LEFT | DT_VCENTER );
//            }
//            break;
//        case R_SELECT:
//            rcItem=GetFullCellRect(lpDrawItemStruct->itemID, TRUE);
//            pDC->FillSolidRect(rcItem, GetColorRef(DEF_SELBACKGROUND));
//            pDC->SetTextColor(GetColorRef(DEF_SELTEXT));
//            for (x=0; x<m_columnSize; x++)
//            {
//                rcItem	= GetCellRect(lpDrawItemStruct->itemID, x, FALSE);
//                pDC->DrawText(CListCtrl::GetItemText(lpDrawItemStruct->itemID,x), rcItem, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
//            }
//            break;
//        case R_SEL_NOFOCUS:
//            rcItem=GetFullCellRect(lpDrawItemStruct->itemID, TRUE);
//            pDC->FillSolidRect(rcItem, GetColorRef(DEF_DESELBACKGROUND));
//            pDC->SetTextColor(pMyLParam->GetTextColor(DEF_DESELTEXT));
//            pen.CreatePen( PS_DOT, 0, GetColorRef(DEF_DESELBACKGROUND) );
//            pOldPen  =pDC->SelectObject(&pen);
//            pOldBrush=pDC->SelectStockObject(NULL_BRUSH);
//            if (!m_withSingleRect)
//                pDC->Rectangle(rcItem);
//            for (x=0; x<m_columnSize; x++)
//            {
//                rcItem	= GetCellRect(lpDrawItemStruct->itemID, x, FALSE);
//                pDC->DrawText(CListCtrl::GetItemText(lpDrawItemStruct->itemID,x), rcItem, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
//            }
//            pDC->SelectObject(pOldPen);
//            pDC->SelectObject(pOldBrush);
//            break;
//        case R_NORMAL:
//            for (x=0; x<m_columnSize; x++)
//            {
//	            rcItem	= GetCellRect(lpDrawItemStruct->itemID, x, TRUE);
//                pDC->FillSolidRect(rcItem, pMyLParam->GetBackColor(x));
//                pDC->SetTextColor(pMyLParam->GetTextColor(x));
//                if (m_withSingleRect)
//                {
//                    pOldBrush=pDC->SelectStockObject(NULL_BRUSH);
//                    pDC->Rectangle(rcItem);
//                    pDC->SelectObject(pOldBrush);
//                    
//                }
//                rcItem	= GetCellRect(lpDrawItemStruct->itemID, x, FALSE);
//                pDC->DrawText(CListCtrl::GetItemText(lpDrawItemStruct->itemID,x), rcItem, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
//            }
//            break;
//    }
//
//    return;
//}
//
//void CColorListCtrl::OnSize(UINT nType, int cx, int cy) 
//{
//	CListCtrl::OnSize(nType, cx, cy);
//    if (IsCreated) 
//    {
//        RedrawWindow();
//    }
//}
