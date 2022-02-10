// By Xiao Wu Guang

// MultiColumnComboBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMultiColumnComboBox window
#ifndef _MULTICOLUMNCOMBOBOX_H_
#define _MULTICOLUMNCOMBOBOX_H_
 
#define LEFTMARGIN_OF_DROPDOWNLIST		2
#define LEFTMARGIN_OF_CONTROL			1
#define RIGHTMARGIN_OF_DROPDOWNLIST		4 
#define SPACE_BETWEEN_COLUMN			1 

/////////////////////////////////////////////////////////////////////////////
// CComboBoxEdit window

class CComboBoxEdit : public CEdit
{
// Construction
public:
	CComboBoxEdit();

// Attributes
public:
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComboBoxEdit)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CComboBoxEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CComboBoxEdit)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

class CMultiColumnComboBox : public CComboBox
{
// Construction
public:
	CMultiColumnComboBox();
	
// Attributes
public:
	void	FormatComboBox(UINT TotalColumn = 2, UINT BoundColumn = 0, UINT ShowColumn = 1);
	void	SetColumnWidth(UINT ColumnWidth0, ...);
	void	SetColumnAlignStyle(UINT ColumnAlignStyle0, ...);

// Operations
public:
	void	ResetContent();
	int		AddRow(LPCTSTR lpszString0, ...);
	LPCSTR	GetCurColumnItem(UINT Column);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMultiColumnComboBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMultiColumnComboBox();

	// Generated message map functions
protected:
private:
	CComboBoxEdit	m_ComboBoxEdit;
	UINT			m_TotalColumn;
	UINT			m_BoundColumn;
	UINT			m_ShowColumn;
	UINT			m_TextItemHeight;
	CPen			m_LightGrayPen;
	UINT*			m_ColumnWidth;
	UINT*			m_ColumnAlignStyle;
	CStringList*	m_ColumnItemList;

	//{{AFX_MSG(CMultiColumnComboBox)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	LRESULT OnGetText(WPARAM wParam, LPARAM lParam);
	LRESULT OnSetText(WPARAM wParam, LPARAM lParam);
};

/////////////////////////////////////////////////////////////////////////////
#endif
