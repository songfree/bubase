// drebpmtDlg.h : header file
//

#if !defined(AFX_DREBPMTDLG_H__BEA40F9C_AD7E_47D3_A083_EBAC14B625BD__INCLUDED_)
#define AFX_DREBPMTDLG_H__BEA40F9C_AD7E_47D3_A083_EBAC14B625BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "public.h"
#include "MultiColumnComboBox.h"
#include "ShareData.h"
#include "TestThread.h"
#include "BF_SimpleLog.h"
#include "BF_Date_Time.h"

#define  MAXTHREADNUM  10000

#define  ONTIMER1 24
#define  ONTIMER2 25

/////////////////////////////////////////////////////////////////////////////
// CDrebpmtDlg dialog

class CDrebpmtDlg : public CDialog
{
// Construction
public:
	void SaveVarInitData();
	void FreeThread();
	void LoadCfg();
	void Clear();
	void autoMonitor();
	bool SaveParameter();
	bool Init();

	void ConvSecTime(int ssec,char *time);

	UINT m_timerID1;//定时器1
	UINT  m_timerID2;//定时器2
	int m_time_auto;
	
	CTestThread *m_pThread[MAXTHREADNUM];
	CShareData m_pShareData;
	CBF_SimpleLog m_publog;
	CBF_Date_Time m_pDate;

	CBF_Mutex m_mutex;
	CStatusBar m_statusbar;

	int m_curThreadNum;
	int m_lastAvr;
	int m_curtime;
	int m_nMaxAvr;
	
	bool m_bisMax;
public:
	
	CDrebpmtDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CDrebpmtDlg)
	enum { IDD = IDD_DREBPMT_DIALOG };
	CEdit	m_edtXmlApi;
	CButton	m_btnApiXml;
	CEdit	m_edtLoginBody;
	CEdit	m_edtLoginTx;
	CEdit	m_edtRcvPushNum;  //接收推送数
	CEdit	m_edtRRTime;      
	CEdit	m_edtLogLevel;    //日志级别
	CButton	m_btnOk;
	CListCtrl	m_lstScript;
	CEdit	m_edtTryNum;
	CEdit	m_edtTotalThread;
	CEdit	m_edtTimeSep;
	CEdit	m_edtTimeOut;
	CEdit	m_edtThreadNum;
	CEdit	m_edtFuncNo;
	CEdit	m_edtSendNum;
	CEdit	m_edtSendByte;
	CEdit	m_edtFile;
	CEdit	m_edtRunTime;
	CEdit	m_edtRcvNum;
	CEdit	m_edtRcvByte;
	CEdit	m_edtMinResp;
	CEdit	m_edtMaxResp;
	CEdit	m_edtMaxNumSec;
	CEdit	m_edtErrNum;
	CEdit	m_edtCurTime;
	CEdit	m_edtCurNumSec;
	CEdit	m_edtBeginTime;
	CEdit	m_edtAvResp;
	CEdit	m_edtAvrSendRate;
	CEdit	m_edtAvrRcvRate;
	CEdit	m_edtAvNumSec;
	CEdit	m_edtAllTime;
	CEdit	m_edtAllThread;
	CButton	m_chkAuto;
	CButton	m_btnStop;
	CButton	m_btnRestar;
	CButton	m_btnFile;
	CButton	m_btnDel;
	CButton	m_btnAdd;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDrebpmtDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CDrebpmtDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	afx_msg void OnButtonView();
	afx_msg void OnButtonStop();
	afx_msg void OnButtonSetvar();
	afx_msg void OnButtonRestar();
	afx_msg void OnButtonSavetest();
	afx_msg void OnButtonOpentest();
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonDel();
	afx_msg void OnButtonSavelist();
	afx_msg void OnButtonOpenlist();
	afx_msg void OnButtonFile();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDblclkListSc(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnCancel();
	afx_msg void OnButtonFilelogin();
	afx_msg void OnButtonApifilename();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DREBPMTDLG_H__BEA40F9C_AD7E_47D3_A083_EBAC14B625BD__INCLUDED_)
