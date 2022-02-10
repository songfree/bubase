// testcommDlg.h : header file
//

#if !defined(AFX_TESTCOMMDLG_H__2F0DE078_E825_4388_803C_22556B3F2255__INCLUDED_)
#define AFX_TESTCOMMDLG_H__2F0DE078_E825_4388_803C_22556B3F2255__INCLUDED_


#include "MultiColumnComboBox.h"
#include "DrawButtonTC.h"
#include "BF_Date_Time.h"
#include "xmlpack.h"
#include "SendData.h"
#include "Send2Server.h"
//#include "CJLibrary.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////////////////////////////////////
// CTestcommDlg dialog

class CTestcommDlg : public CDialog
{
// Construction
public:
	CTestcommDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CTestcommDlg)
	enum { IDD = IDD_TESTCOMM_DIALOG };
	CButton	m_chkConnect;
	CEdit	m_edtLevel;
	CMultiColumnComboBox	m_combPwdFlag;
	CMultiColumnComboBox	m_combZip;
	CEdit	m_edtMinTime;
	CEdit	m_edtMaxTime;
	CProgressCtrl	m_progress;
	CButton	m_chkFill;
	CMultiColumnComboBox	m_combType;
	CMultiColumnComboBox	m_combSelectPack;
	CMultiColumnComboBox	m_combSelectHead;
	CMultiColumnComboBox	m_combAlign;
	CButton	m_chkInclude;
	CButton	m_rdoPack;
	CButton	m_rdoHead;
	CButton	m_rdoModiPack;
	CButton	m_rdoModiHead;
	CButton	m_rdoInvar;
	CButton	m_rdoInvarSeg;
	CButton	m_rdoSeg;
	CButton m_rdoXml;

	CButton	m_btnSend;
	CButton	m_btnSaveTree;
	CButton	m_btnSavePara;
	CButton	m_btnModiAdd;
	CButton	m_btnModi;
	CButton	m_btnDisconnect;
	CButton	m_btnDelete;
	CButton	m_btnConnect;
	CButton	m_btnClearTree;
	CButton	m_btnAdd;

	//CDrawButtonTC	m_btnSend;
	//CDrawButtonTC	m_btnSaveTree;
	//CDrawButtonTC	m_btnSavePara;
	//CDrawButtonTC	m_btnModiAdd;
	//CDrawButtonTC	m_btnModi;
	//CDrawButtonTC	m_btnDisconnect;
	//CDrawButtonTC	m_btnDelete;
	//CDrawButtonTC	m_btnConnect;
	//CDrawButtonTC	m_btnClearTree;
	//CDrawButtonTC	m_btnAdd;

	CTreeCtrl	m_treeAns;
//	CCJListCtrl	m_lstPack;
	CListCtrl   m_lstPack;
//	CCJListCtrl	m_lstHead;
	CListCtrl	m_lstHead;
	CEdit	m_edtValue;
	CEdit	m_edtTimeOut;
	CEdit	m_edtTime;
	CEdit	m_edtThreadNum;
	CEdit	m_edtSendNum;
	CEdit	m_edtSeg;
	CEdit	m_edtPort;
	CEdit	m_edtName;
	CEdit	m_edtLength;
	CEdit	m_edtDec;
	CEdit	m_edtFuncName;
	CEdit	m_edtFuncNo;
	CEdit	m_edtHead;
	CEdit	m_edtIp;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestcommDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL
public:
	int showXmlPackNode(CNode *node, CXmlPack *xmlpack,CTreeCtrl *pTreeCtrl, HTREEITEM hParent);
	int showXmlPack(CXmlPack *xmlpack,CTreeCtrl *pTreeCtrl, HTREEITEM hParent);

	bool IsDouble(char *str);

	// 函数名: UpdatePackFromFile
	// 编程  : 王明松 2007-2-6 16:58:36
	// 返回  : void 
	// 参数  : char *filename
	// 描述  : 从文件更新包体列表
	void UpdatePackFromFile(char *filename);

	// 函数名: UpdateHeadFromFile
	// 编程  : 王明松 2007-2-6 16:57:05
	// 返回  : void 
	// 参数  : char *filename
	// 描述  : 从文件更新包头列表
	void UpdateHeadFromFile(char *filename);

	// 函数名: InitHeadSelect
	// 编程  : 王明松 2007-2-6 16:56:45
	// 返回  : void 
	// 描述  : 初始包头选择
	void InitHeadSelect();

	// 函数名: InitPackSelect
	// 编程  : 王明松 2007-2-6 16:56:40
	// 返回  : void 
	// 描述  : 初始包体选择
	void InitPackSelect();

	// 函数名: GetCommPara
	// 编程  : 王明松 2007-2-6 16:56:31
	// 返回  : void 
	// 描述  : 取通讯参数
	void GetCommPara();
	CXmlPack m_commparaxml;//参数xml
	CSendData m_sendData; //发送数据类
//	CXmlPack m_headxml;//包头xml
//	CXmlPack m_packxml;//包体xml
	S_COMM_PARA m_commpara;
	CBF_Date_Time m_publog;
	CBF_Tools     m_pTool;
	CBF_SimpleLog  m_pLog;

	vector<int> m_allheadno;
	vector<int> m_allpackno;
	char m_errMsg[256];
	
	bool m_bIsResult;//是否显示结果
	CSend2Server m_send2server[200];
	CBF_SocketTcp m_tcp;
	//vector <S_REQANS> m_reqans;
// Implementation
protected:
	BOOL PreTranslateMessage( MSG* pMsg );
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTestcommDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonSavePara();
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonClearTree();
	afx_msg void OnButtonConnect();
	afx_msg void OnButtonDelete();
	afx_msg void OnButtonDisconnect();
	afx_msg void OnButtonModi();
	afx_msg void OnButtonModiAdd();
	afx_msg void OnButtonSaveTree();
	afx_msg void OnButtonSend();
	afx_msg void OnRadioAddHead();
	afx_msg void OnRadioAddPack();
	afx_msg void OnRadioModiHead();
	afx_msg void OnRadioModiPack();
	afx_msg void OnRadioInvar();
	afx_msg void OnRadioInvarseg();
	afx_msg void OnRadioSeg();
	afx_msg void OnSelchangeComboType();
	afx_msg void OnSelchangeComboSelHead();
	afx_msg void OnSelchangeComboSelPack();
	afx_msg void OnItemchangedListHead(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedListPack(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRadioXml();
	afx_msg void OnChangeEditSendNum();
	afx_msg void OnCheckConnect();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTCOMMDLG_H__2F0DE078_E825_4388_803C_22556B3F2255__INCLUDED_)
