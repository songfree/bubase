// By Xiao Wu Guang

// MultiColumnComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "MultiColumnComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMultiColumnComboBox

CMultiColumnComboBox::CMultiColumnComboBox()
{
	m_TotalColumn		= 0;
	m_BoundColumn		= 0;
	m_TextItemHeight	= 12;
	m_LightGrayPen.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_3DLIGHT));
	m_ColumnWidth		= NULL;
	m_ColumnAlignStyle	= NULL;
	m_ColumnItemList	= NULL;
}

CMultiColumnComboBox::~CMultiColumnComboBox()
{
	if (m_TotalColumn != 0)
	{
		delete[] m_ColumnWidth;
		delete[] m_ColumnAlignStyle;
		delete[] m_ColumnItemList;
	}
	m_LightGrayPen.DeleteObject();
}

BEGIN_MESSAGE_MAP(CMultiColumnComboBox, CComboBox)
	//{{AFX_MSG_MAP(CMultiColumnComboBox)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_APP + WM_GETTEXT, OnGetText)
	ON_MESSAGE(WM_APP + WM_SETTEXT, OnSetText)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMultiColumnComboBox message handlers
void CMultiColumnComboBox::FormatComboBox(UINT TotalColumn, UINT BoundColumn, UINT ShowColumn)
{
	ASSERT(TotalColumn > 1);	//You can use CComboBox class instead of this class

	m_TotalColumn	= TotalColumn;
	m_BoundColumn	= BoundColumn;
	m_ShowColumn	= ShowColumn;

	m_ColumnWidth		= new UINT[m_TotalColumn];
	m_ColumnAlignStyle	= new UINT[m_TotalColumn];
	m_ColumnItemList	= new CStringList[m_TotalColumn];

	LONG	ComboBoxStyle = GetWindowLong(m_hWnd, GWL_STYLE) & 0x3;
	if (ComboBoxStyle != CBS_SIMPLE)
	{
		UINT TotalColumnWidth;
		UINT Index;
		UINT ComboBoxWidth = GetDroppedWidth() - LEFTMARGIN_OF_DROPDOWNLIST - SPACE_BETWEEN_COLUMN - 1;
		for (Index = 0, TotalColumnWidth = LEFTMARGIN_OF_DROPDOWNLIST + RIGHTMARGIN_OF_DROPDOWNLIST; Index < m_TotalColumn; Index++)
		{	//Assign default value 
			m_ColumnWidth[Index] = ComboBoxWidth;
			m_ColumnAlignStyle[Index] = DT_LEFT;
			TotalColumnWidth += (ComboBoxWidth + SPACE_BETWEEN_COLUMN);
		}

		SetDroppedWidth(TotalColumnWidth);
	}

	if (ComboBoxStyle == CBS_DROPDOWN)
		m_ComboBoxEdit.SubclassWindow(GetWindow(GW_CHILD)->m_hWnd);
	else
		if (ComboBoxStyle == CBS_SIMPLE)
			m_ComboBoxEdit.SubclassWindow((GetWindow(GW_CHILD)->GetWindow(GW_HWNDNEXT))->m_hWnd);

	CDC * DeviceContextPointer = GetWindowDC();
	m_TextItemHeight = (DeviceContextPointer->GetTextExtent(" ", 1)).cy - 1;
	SetItemHeight(-1, m_TextItemHeight);
	ReleaseDC(DeviceContextPointer);
}

void CMultiColumnComboBox::ResetContent()
{
	CComboBox::ResetContent();
	for (UINT ColumnIndex = 0; ColumnIndex < m_TotalColumn; ColumnIndex++)
		m_ColumnItemList[ColumnIndex].RemoveAll();
};

void CMultiColumnComboBox::SetColumnWidth(UINT ColumnWidth0, ...)
{	
	va_list ColumnWidthPointer;
	UINT TotalColumnWidth;

	ASSERT(m_TotalColumn > 1);	//call FormatComboBox first

	m_ColumnWidth[0] = ColumnWidth0;
	TotalColumnWidth = ColumnWidth0 + SPACE_BETWEEN_COLUMN + LEFTMARGIN_OF_DROPDOWNLIST + RIGHTMARGIN_OF_DROPDOWNLIST;

	va_start(ColumnWidthPointer, ColumnWidth0);
	for (UINT Index = 1; Index < m_TotalColumn; Index++)
	{
		m_ColumnWidth[Index] = va_arg(ColumnWidthPointer, UINT);
		if (m_ColumnWidth[Index] > 0)
			TotalColumnWidth += (m_ColumnWidth[Index] + SPACE_BETWEEN_COLUMN);
	}
	va_end(ColumnWidthPointer);

	if ((GetWindowLong(m_hWnd, GWL_STYLE) & 0x3) == CBS_SIMPLE)
	{
		CRect		WindowRectangle;
		GetWindowRect(WindowRectangle);
		SetWindowPos(NULL, 0, 0, TotalColumnWidth, WindowRectangle.bottom - WindowRectangle.top, SWP_NOMOVE | SWP_NOZORDER);
	}
	else
		SetDroppedWidth(TotalColumnWidth);
}

void CMultiColumnComboBox::SetColumnAlignStyle(UINT ColumnAlignStyle0, ...)
{	
	va_list ColumnAlignStylePointer;

	ASSERT(m_TotalColumn > 1);	//call FormatComboBox first

	m_ColumnAlignStyle[0] = ColumnAlignStyle0;

	va_start(ColumnAlignStylePointer, ColumnAlignStyle0);
	for (UINT Index = 1; Index < m_TotalColumn; Index++)
		m_ColumnAlignStyle[Index] = va_arg(ColumnAlignStylePointer, UINT);
	va_end(ColumnAlignStylePointer);
}

int CMultiColumnComboBox::AddRow(LPCTSTR lpszString0, ...)
{	
	va_list StringPointerPointer;
	UINT	Index, ItemIndex;
	LPCTSTR	StringPointer;

	ASSERT(m_TotalColumn > 1);	//call FormatComboBox first

	if (m_BoundColumn == 0)
		ItemIndex = CComboBox::AddString(lpszString0);
	else
	{
		va_start(StringPointerPointer, lpszString0);
		for (Index = 1; Index < m_TotalColumn; Index++)
		{
			StringPointer = va_arg(StringPointerPointer, LPCTSTR);
			if (m_BoundColumn == Index)
				ItemIndex = CComboBox::AddString(StringPointer);
		}
	}

	if (ItemIndex == 0)
		m_ColumnItemList[0].AddHead(lpszString0);
	else
		if (ItemIndex == (UINT) CComboBox::GetCount() - 1)
			m_ColumnItemList[0].AddTail(lpszString0);
		else
			m_ColumnItemList[0].InsertBefore(m_ColumnItemList[0].FindIndex(ItemIndex), lpszString0);

	va_start(StringPointerPointer, lpszString0);
	for (Index = 1; Index < m_TotalColumn; Index++)
	{
		StringPointer = va_arg(StringPointerPointer, LPCTSTR);

		if (ItemIndex == 0)
			m_ColumnItemList[Index].AddHead(StringPointer);
		else
			if (ItemIndex == (UINT) CComboBox::GetCount() - 1)
				m_ColumnItemList[Index].AddTail(StringPointer);
			else
				m_ColumnItemList[Index].InsertBefore(m_ColumnItemList[Index].FindIndex(ItemIndex), StringPointer);
	}
	va_end(StringPointerPointer);

	return ItemIndex;
}

LPCSTR	CMultiColumnComboBox::GetCurColumnItem(UINT Column)
{	
	int	ItemIndex;

	if ((ItemIndex = CComboBox::GetCurSel()) != CB_ERR && Column < m_TotalColumn)
		return m_ColumnItemList[Column].GetAt(m_ColumnItemList[Column].FindIndex(ItemIndex));
	else
		return NULL;
}

void CMultiColumnComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	ASSERT(m_TotalColumn > 1);	//You can use CComboBox class instead of this class

	CPen*		OldPenPointer;
	CDC*		DeviceContextPointer = CDC::FromHandle(lpDrawItemStruct->hDC);
	CWnd*		DropDownWindowPointer = GetParent();
	BOOL		IsHighLightText = lpDrawItemStruct->itemAction & ODA_FOCUS || lpDrawItemStruct->itemState & ODS_SELECTED;
	CRect		TextRectangle = lpDrawItemStruct->rcItem;
	
	DeviceContextPointer->SetTextColor(GetSysColor(IsWindowEnabled() ? (IsHighLightText ? COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT) : COLOR_WINDOWTEXT));
	DeviceContextPointer->SetBkColor(GetSysColor(IsWindowEnabled() ? (IsHighLightText ? COLOR_HIGHLIGHT : COLOR_WINDOW) : COLOR_3DFACE));
	DeviceContextPointer->FillRect(TextRectangle, &CBrush(DeviceContextPointer->GetBkColor()));

	if (IsHighLightText)
		DeviceContextPointer->DrawFocusRect(TextRectangle);

	if (lpDrawItemStruct->itemID != (UINT)-1)
	{
		if (DropDownWindowPointer != this)		//Draw items in drop down list
		{
			if (TextRectangle.top == 0 && (GetWindowLong(m_hWnd, GWL_STYLE) & 0x3) != CBS_SIMPLE)
			{	//Only "dropdwon" & "drop list" combo box can move their drop down window
				CRect		WindowRectangle;
				DropDownWindowPointer->GetWindowRect(WindowRectangle);

				if (WindowRectangle.right > GetSystemMetrics(SM_CXSCREEN))	//Move drop down window to left if its left side is out of screen
				{
					WindowRectangle.OffsetRect(GetSystemMetrics(SM_CXSCREEN) - WindowRectangle.right, 0);
					DropDownWindowPointer->MoveWindow(WindowRectangle);
  				}
			}

			if (!IsHighLightText)
			{
				OldPenPointer = DeviceContextPointer->SelectObject(&m_LightGrayPen);
				DeviceContextPointer->MoveTo(TextRectangle.left, TextRectangle.bottom - 1);
				DeviceContextPointer->LineTo(TextRectangle.right, TextRectangle.bottom - 1);
			}

			TextRectangle.left += LEFTMARGIN_OF_DROPDOWNLIST;
			TextRectangle.right = TextRectangle.left;

			for (UINT ColumnIndex = 0; ColumnIndex < m_TotalColumn; ColumnIndex++)
			{
				TextRectangle.left = TextRectangle.right;

				if (m_ColumnWidth[ColumnIndex] != 0)
				{
					if (ColumnIndex > 1 || (ColumnIndex == 1 && m_ColumnWidth[0] != 0))
					{	
						TextRectangle.left += SPACE_BETWEEN_COLUMN;
						if (!IsHighLightText)
						{	//draw vertical gray line
							DeviceContextPointer->MoveTo(TextRectangle.left - SPACE_BETWEEN_COLUMN / 2, TextRectangle.top);
							DeviceContextPointer->LineTo(TextRectangle.left - SPACE_BETWEEN_COLUMN / 2, TextRectangle.bottom);
						}
					}

					TextRectangle.right = TextRectangle.left + m_ColumnWidth[ColumnIndex];
					TextRectangle.left++;

					// if vertical scrool bar is visible
					if ((ColumnIndex == m_TotalColumn - 1 || (ColumnIndex == m_TotalColumn - 2 && m_ColumnWidth[m_TotalColumn - 1] == 0)) && (GetWindowLong(DropDownWindowPointer->m_hWnd, GWL_STYLE) & WS_VSCROLL))
						TextRectangle.right -= GetSystemMetrics(SM_CXVSCROLL);

					DeviceContextPointer->DrawText(m_ColumnItemList[ColumnIndex].GetAt(m_ColumnItemList[ColumnIndex].FindIndex(lpDrawItemStruct->itemID)), -1, TextRectangle,
						m_ColumnAlignStyle[ColumnIndex] | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER);
				}
			}

			if (!IsHighLightText)
				DeviceContextPointer->SelectObject(OldPenPointer);
		}
		else												//Draw item in edit box control/static text control
		{
			TextRectangle.left += LEFTMARGIN_OF_CONTROL;

			if (m_ColumnWidth[m_BoundColumn] > 0)
				DeviceContextPointer->DrawText(m_ColumnItemList[m_BoundColumn].GetAt(m_ColumnItemList[m_BoundColumn].FindIndex(lpDrawItemStruct->itemID)), -1, TextRectangle,
					m_ColumnAlignStyle[m_BoundColumn] | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER);
			else											//Bound column is hidden
			{
				//Seek a column that its width is not 0
//				for (UINT ColumnIndex = 0; !m_ColumnWidth[ColumnIndex] && ColumnIndex < m_TotalColumn; ColumnIndex++);

//				DeviceContextPointer->DrawText(m_ColumnItemList[ColumnIndex].GetAt(m_ColumnItemList[ColumnIndex].FindIndex(lpDrawItemStruct->itemID)), -1, TextRectangle,
//					m_ColumnAlignStyle[ColumnIndex] | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER);
				DeviceContextPointer->DrawText(m_ColumnItemList[m_ShowColumn].GetAt(m_ColumnItemList[m_ShowColumn].FindIndex(lpDrawItemStruct->itemID)), -1, TextRectangle,
					m_ColumnAlignStyle[m_ShowColumn] | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER);
			}
		}
	}
}

void CMultiColumnComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	lpMeasureItemStruct->itemHeight = m_TextItemHeight;
}

LRESULT CMultiColumnComboBox::OnSetText(WPARAM wParam, LPARAM lParam)
{
	UINT		ShowColumnItemIndex, BoundColumnItemCount;

	BoundColumnItemCount = m_ColumnItemList[m_BoundColumn].GetCount();
	for (ShowColumnItemIndex = 0; ShowColumnItemIndex < BoundColumnItemCount && m_ColumnItemList[m_BoundColumn].GetAt(m_ColumnItemList[m_BoundColumn].FindIndex(ShowColumnItemIndex)) != (LPCTSTR) lParam; ShowColumnItemIndex++);

	if (ShowColumnItemIndex < BoundColumnItemCount)
		return (LRESULT) (LPCTSTR) (m_ColumnItemList[m_ShowColumn].GetAt(m_ColumnItemList[m_ShowColumn].FindIndex(ShowColumnItemIndex)));
	else
		return 0;
}

LRESULT CMultiColumnComboBox::OnGetText(WPARAM wParam, LPARAM lParam)
{
	UINT		ShowColumnItemIndex, BoundColumnItemCount;

	BoundColumnItemCount = m_ColumnItemList[m_ShowColumn].GetCount();

//case sensitive:
//	for (ShowColumnItemIndex = 0; ShowColumnItemIndex < BoundColumnItemCount && m_ColumnItemList[m_ShowColumn].GetAt(m_ColumnItemList[m_ShowColumn].FindIndex(ShowColumnItemIndex)) != (LPCTSTR) lParam; ShowColumnItemIndex++);
//case insensitive:
	for (ShowColumnItemIndex = 0; ShowColumnItemIndex < BoundColumnItemCount && (m_ColumnItemList[m_ShowColumn].GetAt(m_ColumnItemList[m_ShowColumn].FindIndex(ShowColumnItemIndex))).CompareNoCase((LPCTSTR) lParam) != 0; ShowColumnItemIndex++);

	if (ShowColumnItemIndex < BoundColumnItemCount)
	{
		memcpy((void *) (LPCTSTR) lParam, m_ColumnItemList[m_BoundColumn].GetAt(m_ColumnItemList[m_BoundColumn].FindIndex(ShowColumnItemIndex)), wParam);
		return 1; 
	}
	else
		return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CComboBoxEdit

CComboBoxEdit::CComboBoxEdit()
{
}

CComboBoxEdit::~CComboBoxEdit()
{
}

BEGIN_MESSAGE_MAP(CComboBoxEdit, CEdit)
	//{{AFX_MSG_MAP(CComboBoxEdit)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComboBoxEdit message handlers

LRESULT CComboBoxEdit::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	switch (message)
	{
		case WM_GETTEXTLENGTH:
			{
				char	StringBuffer[255];
				UINT	ShowItemLength;

				GetLine(0, StringBuffer, 255);
				for (ShowItemLength = 0; StringBuffer[ShowItemLength] != 0; ShowItemLength++);
				if (GetParent()->SendMessage(WM_APP + WM_GETTEXT, 255, (LPARAM) StringBuffer) != 0)
				{
					UINT	BoundItemLength;
					for (BoundItemLength = 0; StringBuffer[BoundItemLength] != 0; BoundItemLength++);
					return max(BoundItemLength + 1, ShowItemLength + 1);
				}
				else
					return ShowItemLength + 1;
			}
			break;
		case WM_GETTEXT:
			CEdit::WindowProc(message, wParam, lParam);			//Get text in edit control
			return GetParent()->SendMessage(WM_APP + WM_GETTEXT, wParam, lParam);	//replace it
			break;
		case WM_SETTEXT:		//replace text, then set text
			return CEdit::WindowProc(message, wParam, (LPARAM) GetParent()->SendMessage(WM_APP + WM_SETTEXT, wParam, lParam));
			break;
		default:
			return CEdit::WindowProc(message, wParam, lParam);
			break;
	}
}

void CComboBoxEdit::OnPaint() 
{
	CPaintDC	DeviceContext(this); // device context for painting
	// TODO: Add your message handler code here

	char		StringBuffer[255];

	GetLine(0, StringBuffer, 255);
	DeviceContext.SelectObject(::GetStockObject(DEFAULT_GUI_FONT));

	if (!IsWindowEnabled())
		DeviceContext.SetBkColor(GetSysColor(COLOR_3DFACE));

	DeviceContext.FillRect(&(DeviceContext.m_ps.rcPaint), &CBrush(DeviceContext.GetBkColor()));
	DeviceContext.DrawText(StringBuffer, &(DeviceContext.m_ps.rcPaint), DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER | DT_BOTTOM);
	
	// Do not call CEdit::OnPaint() for painting messages
}
