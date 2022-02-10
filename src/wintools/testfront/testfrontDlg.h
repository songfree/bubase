// testfrontDlg.h : header file
//

#if !defined(AFX_TESTFRONTDLG_H__856C3E7F_5443_432F_A38E_30ACF0EC9ED8__INCLUDED_)
#define AFX_TESTFRONTDLG_H__856C3E7F_5443_432F_A38E_30ACF0EC9ED8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "public.h"
#include "MultiColumnComboBox.h"
#include "ShareData.h"
#include "TestThread.h"
#include "BF_Date_Time.h"
#include "BF_Tools.h"

#define  MAXTHREADNUM  100000

#define  ONTIMER1 24
#define  ONTIMER2 25
#define  ONTIMER3 26

/////////////////////////////////////////////////////////////////////////////
// CTestfrontDlg dialog

class CTestfrontDlg : public CDialog
{
// Construction
public:
	void IsStopAll();
	void StopThread();
	void SaveVarInitData();
	void FreeThread();
	void LoadCfg();
	void Clear();
	void autoMonitor();
	bool SaveParameter();
	bool Init();
	void ConvSecTime(int ssec,char *time);
	CTestfrontDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CTestfrontDlg)
	enum { IDD = IDD_TESTFRONT_DIALOG };
	CEdit	m_edtMaxAvNumSec;
	CEdit	m_edtAvrSendRate;
	CEdit	m_edtAvrRcvRate;
	CEdit	m_edtSendByte;
	CEdit	m_edtRcvByte;
	CButton	m_btnRestar;
	CEdit	m_edtTryNum;
	CButton	m_chkAuto;
	CEdit	m_edtDateTime;
	CButton	m_btnOk;
	CListCtrl	m_lstScript;
	CIPAddressCtrl	m_ipctr;
	CEdit	m_edtTimeSep;
	CEdit	m_edtTimeout;
	CEdit	m_edtThreadNum;
	CEdit	m_edtSendNum;
	CEdit	m_edtFile;
	CEdit	m_edtRunTime;
	CEdit	m_edtRcvNum;
	CEdit	m_edtPort;
	CEdit	m_edtMinResp;
	CEdit	m_edtMaxResp;
	CEdit	m_edtLevel;
	CEdit	m_edtHead;
	CEdit	m_edtErrNum;
	CEdit	m_edtCurTime;
	CEdit	m_edtCurNumSec;
	CEdit	m_edtBeginTime;
	CEdit	m_edtAvResp;
	CEdit	m_edtAvNumSec;
	CEdit	m_edtAllTime;
	CEdit	m_edtAllThread;
	CMultiColumnComboBox m_combConnect;
	CButton	m_btnStop;
	CButton	m_btnFile;
	CButton	m_btnDel;
	CButton	m_btnAdd;
	//}}AFX_DATA
	UINT m_timerID1;//定时器1
	UINT  m_timerID2;//定时器2
	UINT  m_timerID3;
	int m_time_auto;

	CTestThread *m_pThread[MAXTHREADNUM];
	CShareData m_pShareData;

	CBF_Date_Time m_publog;
	CBF_SimpleLog   m_log;
	CBF_Mutex m_mutex;

	int m_curThreadNum;
	int m_lastAvr;
	int m_curtime;
	int m_nMaxAvr;

	bool m_bisMax;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestfrontDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTestfrontDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonDel();
	afx_msg void OnButtonFile();
	afx_msg void OnButtonStop();
	afx_msg void OnClickListSc(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListSc(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClose();
#if _MSC_VER > 1200
    afx_msg void OnTimer(UINT_PTR nIDEvent);
#else
	afx_msg void OnTimer(UINT nIDEvent);
#endif
	
	afx_msg void OnButtonRestar();
	afx_msg void OnButtonView();
	afx_msg void OnButtonSetvar();
	afx_msg void OnButtonSavelist();
	afx_msg void OnButtonOpenlist();
	afx_msg void OnButtonSavetest();
	afx_msg void OnButtonOpentest();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTFRONTDLG_H__856C3E7F_5443_432F_A38E_30ACF0EC9ED8__INCLUDED_)
