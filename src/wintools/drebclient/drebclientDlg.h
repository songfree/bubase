// drebclientDlg.h : header file
//

#if !defined(AFX_DREBCLIENTDLG_H__31A0A3BD_EC0B_4FFA_BE1C_6DCD83AF44C8__INCLUDED_)
#define AFX_DREBCLIENTDLG_H__31A0A3BD_EC0B_4FFA_BE1C_6DCD83AF44C8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "MultiColumnComboBox.h"
#include "BF_Xml.h"
#include "DrebClient.h"
#include "BF_Tools.h"
#include "ResultView.h"
#include "BF_SimpleLog.h"
#include "BF_Date_Time.h"
#include "FileXdp.h"
#include "Xdp.h"

#define  ONTIMER1 24
#define  ONTIMER2 25

/////////////////////////////////////////////////////////////////////////////
// CDrebclientDlg dialog

class CDrebclientDlg : public CDialog
{
// Construction
public:
	int DpBcUnRegTx();
	void UpLoadFile(const char *localfilename, const char *remotefilename);
	void DownLoadFile(const char *localfilename,const char *remotefilename);
	void DownUpLoadFile();
	bool GetLenOffset(const char *buf,int &len,int &offset);
	void DisplayButton(HTREEITEM hti);
	void HideButton(HTREEITEM hti);
	void ShowPack(HTREEITEM hParent,DREB_HEAD head,const char *data,bool isans=false);
	int MonitorBpc();
	int Monitor();
	int DpPost();
	int DpPush();
	int DpAbc();
	int DpBc();
	int DpACall();
	int DpCall();
	int GetSendData();
	bool SetParaToFile(const char *filename);
	bool GetParaFromFile(const char *filename);
	CDrebclientDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CDrebclientDlg)
	enum { IDD = IDD_DREBCLIENT_DIALOG };
	CEdit	m_edtSerial;
	CEdit	m_edtIndex;
	CEdit	m_edtHook;
	CComboBox	m_combZip;
	CButton	m_btnGetNext;
	CTreeCtrl	m_treResultList;
	CIPAddressCtrl	m_ipAddress;
	CEdit	m_edtTimeOut;
	CEdit	m_edtSvrPrivateId;
	CEdit	m_edtSvrMainId;
	CEdit	m_edtServiceNo;
	CEdit	m_edtNodePrivateId;
	CEdit	m_edtNodeId;
	CEdit	m_edtDrebPort;
	CEdit	m_edtDataFile;
	CMultiColumnComboBox	m_combMessage;
	CButton	m_chkData;
	CButton	m_btnSend;
	CButton	m_btnFile;
	CButton	m_btnDisConnect;
	CRichEditCtrl	m_rEditData;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDrebclientDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CBF_Xml  m_pXml;
	CDrebClient  m_pClient;
	CStatusBar m_statusbar;
	CBF_Date_Time m_publog;
	CBF_SimpleLog  m_pLog;

	bool  m_bFromXdp;

	CDrebEndian    m_pDrebEndian;

	CXdp    m_pXdp;

	CFileXdp   m_xdpdlg;

	HTREEITEM	m_hRoot;
	char m_sDataMsg[256];

	int m_nSendNum;
	int m_nRcvNum;

	DREB_D_NODEINFO m_pDrebDest;
	DREB_HEAD       m_head;

	char   m_sDataFileName[40];
	int    m_nHook;
	int    m_nSerial;
	int    m_ngateindex;
	int    m_cZip;

	char  m_errMsg[256];

	char m_sSendData[DREBDATASIZE];
	char m_sRcvData[DREBDATASIZE];
	int  m_nDataLen;

	std::string m_sDrebIp;
	int         m_nDrebPort;
	int         m_nTimeOut;

	HICON m_hIcon;

	UINT m_timerID1;//¶¨Ê±Æ÷1
	

	// Generated message map functions
	//{{AFX_MSG(CDrebclientDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonDisconnect();
	afx_msg void OnButtonFile();
	afx_msg void OnButtonSend();
	afx_msg void OnCheckData();
	virtual void OnOK();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnButtonClear();
	afx_msg void OnButtonRcvnext();
	afx_msg void OnDblclkTreeResultlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnView();
	afx_msg void OnButtonXdp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DREBCLIENTDLG_H__31A0A3BD_EC0B_4FFA_BE1C_6DCD83AF44C8__INCLUDED_)
