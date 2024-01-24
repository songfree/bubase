#pragma once
#include "BF_Xml.h"

// CSetDatabase 对话框

class CSetDatabase : public CDialogEx
{
	DECLARE_DYNAMIC(CSetDatabase)

public:
	CSetDatabase(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CSetDatabase();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SET };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	CBF_Xml m_pXml;
public:
	virtual BOOL OnInitDialog();
	// 数据库类型
	CComboBox m_combDbtype;
	// 密码加密标志
	CComboBox m_combPwdFlag;
	// 数据库名字
	CEdit m_edtDbName;
	// 数据库密码
	CEdit m_edtPwd;
	// 数据库用户
	CEdit m_edtUser;
	afx_msg void OnBnClickedOk();
};
