// LogViewDlg.h : header file
//

#if !defined(AFX_LOGVIEWDLG_H__F4EB5086_C13F_41D0_8C9C_F44A0CF801E7__INCLUDED_)
#define AFX_LOGVIEWDLG_H__F4EB5086_C13F_41D0_8C9C_F44A0CF801E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CLogViewDlg dialog

#include "DrebBpcLog.h"
#include "DrebEndian.h"
#include "BpcEndian.h"
#include "MdbBase.h"


#ifdef __cplusplus
extern "C" {
#endif
	
	//�ֽڶ���
#ifdef _WINDOWS
#define __PACKED__ 
#pragma pack(push,1)
#else
#ifdef __GNUC__
#define __PACKED__	__attribute__ ((packed))
	//#pragma pack(push,1)
#else
#define __PACKED__
#pragma options align=packed
#endif
#endif
	

typedef struct   //bpc������־
{
	unsigned int nTime;//����ʱ��  hhmmss
	unsigned int nUs;  //΢��
	unsigned int nId;  //����ID
	S_BPC_HEAD    sBpcHead;        //bpchead
	DREB_HEAD     sDBHead;         //drebͷ
}__PACKED__ SL_LOG_BPCDATA;

typedef struct   //dreb������־
{
	unsigned int nTime;//����ʱ��  hhmmss
	unsigned int nUs;  //΢��
	unsigned int nId;  //����ID
	DREB_HEAD    head;  
}__PACKED__ SL_LOG_DREBDATA;


typedef struct 
{
	unsigned int id;
	SL_LOG_BPCDATA  head;
	unsigned int ndatabegin;
	unsigned int ndataend;
}__PACKED__ S_BPCHEADLIST;

typedef struct 
{
	unsigned int id;
	SL_LOG_DREBDATA  head;
	unsigned int ndatabegin;
	unsigned int ndataend;
}__PACKED__ S_DREBHEADLIST;




//ȡ���ֽڶ���
#ifdef _WINDOWS
#pragma pack(pop)
#else
#ifdef __GNUC__
//#pragma pack(pop)
#else
#pragma options align=reset
#endif
#endif
#undef __PACKED__

#ifdef __cplusplus
}
#endif


#define  DREBLOGHEADLEN sizeof(SL_LOG_DREBDATA)
#define  BPCLOGHEADLEN  sizeof(SL_LOG_BPCDATA)


typedef vector<S_DREBHEADLIST>  V_DREBHEADLIST;
typedef vector<S_BPCHEADLIST>   V_BPCHEADLIST;



class CLogViewDlg : public CDialog
{
// Construction
public:

	// ������: IsResult
	// ���  : ������ 2013-7-4 10:08:26
	// ����  : bool 
	// ����  : int id
	// ����  : ������Ļ����������ж�ID��Ӧ�������Ƿ���ʾ
	bool IsResult(int id);

	// ������: GetBpcMsgType
	// ���  : ������ 2013-7-4 10:09:05
	// ����  : std::string 
	// ����  : int msgtype
	// ����  : ȡBPC��Ϣ����
	std::string GetBpcMsgType(int msgtype);

	// ������: GetDrebCmdType
	// ���  : ������ 2013-7-4 10:09:10
	// ����  : std::string 
	// ����  : int cmdtype
	// ����  : ȡDREB��������
	std::string GetDrebCmdType(int cmdtype);


	// ������: DisplayDataItem
	// ���  : ������ 2013-7-4 10:08:08
	// ����  : void 
	// ����  : int id
	// ����  : ����ID��ʾһ������
	void DisplayDataItem(int id);

	// ������: DisplayHead
	// ���  : ������ 2013-7-4 10:07:53
	// ����  : void 
	// ����  : ��ʾ��ͷ
	void DisplayHead();

	// ������: DisplayData
	// ���  : ������ 2013-7-4 10:02:45
	// ����  : void 
	// ����  : CInt dataindex
	// ����  : ��ID�б���ʾ���ݣ�
	void DisplayData(CInt dataindex);

	// ������: DisplayData
	// ���  : ������ 2013-7-4 10:02:26
	// ����  : void 
	// ����  : ��ʾ���е�ͷ����
	void DisplayData();

	// ������: ReadBpcLogFile
	// ���  : ������ 2013-7-4 10:01:15
	// ����  : bool 
	// ����  : FILE *fp
	// ����  : ��BPC��־����ͷ�ŵ�m_vBpcHeadList��
	bool ReadBpcLogFile(FILE *fp);

	// ������: ReadDrebLogFile
	// ���  : ������ 2013-7-4 10:01:57
	// ����  : bool 
	// ����  : FILE *fp
	// ����  : ��DREB��־����ͷ�ŵ� m_vDrebHeadList��
	bool ReadDrebLogFile(FILE *fp);

	// ������: ReadFileData
	// ���  : ������ 2013-7-4 10:00:55
	// ����  : bool 
	// ����  : FILE *fp
	// ����  : ����־�ļ�
	bool ReadFileData(FILE *fp);
	CLogViewDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CLogViewDlg)
	enum { IDD = IDD_LOGVIEW_DIALOG };
	CEdit	m_edt_s_gateSerial;
	CButton	m_btn_next;
	CButton	m_btn_prev;
	CEdit	m_edt_s_svrPrivate;
	CEdit	m_edt_s_svr;
	CEdit	m_edt_zip;
	CEdit	m_edt_s_serial;
	CEdit	m_edt_s_nodePrivate;
	CEdit	m_edt_s_node;
	CEdit	m_edt_s_index;
	CEdit	m_edt_s_hook;
	CEdit	m_edt_s_drebSerial;
	CEdit	m_edt_raFlag;
	CEdit	m_edt_nextFlag;
	CEdit	m_edt_n_nextOffset;
	CEdit	m_edt_n_nextNo;
	CEdit	m_edt_len;
	CEdit	m_edt_index;
	CEdit	m_edt_etime;
	CEdit	m_edt_drebAffirm;
	CEdit	m_edt_d_svrPrivateId;
	CEdit	m_edt_d_svrMainId;
	CEdit	m_edt_d_serviceNo;
	CEdit	m_edt_d_nodePrivateId;
	CEdit	m_edt_d_nodeid;
	CEdit	m_edt_btime;
	CEdit	m_edt_bpuIndex;
	CEdit	m_edt_a_retCode;
	CEdit	m_edt_a_nodePrivateId;
	CEdit	m_edt_a_nodeId;
	CComboBox	m_comb_msgType;
	CComboBox	m_comb_msgFlag;
	CComboBox	m_comb_cmd;
	CButton	m_static_list;
	CListCtrl	m_lstLogHead;
	CString	m_csCmd;
	CString	m_csMsgFlag;
	CString	m_csMsgType;
	CString	m_cs_a_nodeid;
	CString	m_cs_a_nodepid;
	CString	m_cs_a_retCode;
	CString	m_cs_bpuindex;
	CString	m_cs_btime;
	CString	m_cs_d_nodeid;
	CString	m_cs_d_nodepid;
	CString	m_cs_d_serviceNo;
	CString	m_cs_d_svrid;
	CString	m_cs_d_svrpid;
	CString	m_cs_drebAffirm;
	CString	m_cs_etime;
	CString	m_cs_index;
	CString	m_cs_len;
	CString	m_cs_n_nextno;
	CString	m_cs_n_nextoffset;
	CString	m_cs_nextflag;
	CString	m_cs_raflag;
	CString	m_cs_s_drebserial;
	CString	m_cs_s_hook;
	CString	m_cs_s_index;
	CString	m_cs_s_nodeid;
	CString	m_cs_s_nodepid;
	CString	m_cs_s_serial;
	CString	m_cs_s_svrpid;
	CString	m_cs_s_svrid;
	CString	m_cs_zip;
	CString	m_cs_gateindex;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogViewDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void GetQueryData();
	HICON m_hIcon;

	unsigned    int  m_nCurDisplayIndex;//��ǰ��ʾ�����index
	V_DREBHEADLIST   m_vDrebHeadList; //dreb��־ͷ�б�
	V_BPCHEADLIST    m_vBpcHeadList;  //bpc��־ͷ�б�
	CDrebEndian      m_pDrebEndian;   //dreb�ֽ���ת����
	CBpcEndian       m_pBpcEndian;    //bpc�ֽ���ת����
	bool             m_bIsDrebLog;    //�Ƿ�Dreb��־ true�� false��bpc
	std::string      m_sLogFilePath;  //��־�ļ���

// 	CString          m_sBTime;
// 	CString          m_sETime;
// 	CString	         m_s_svrPrivate;
// 	CString	         m_s_svr;
// 	CString	         m_zip;
// 	CString	         m_s_serial;
// 	CString	         m_s_nodePrivate;
// 	CString	         m_s_node;
// 	CString	         m_s_index;
// 	CString	         m_s_hook;
// 	CString	         m_s_drebSerial;
// 	CString	         m_edt_raFlag;
// 	CString	         m_edt_nextFlag;
// 	CString	         m_edt_n_nextOffset;
// 	CString	         m_edt_n_nextNo;
// 	CString	         m_edt_len;
// 	CString	         m_edt_index;
// 	CString	         m_edt_drebAffirm;
// 	CString	         m_edt_d_svrPrivateId;
// 	CString	         m_edt_d_svrMainId;
// 	CString	         m_edt_d_serviceNo;
// 	CString	         m_edt_d_nodePrivateId;
// 	CString	         m_edt_d_nodeid;
// 	CString	         m_edt_bpuIndex;
// 	CString	         m_edt_a_retCode;
// 	CString	         m_edt_a_nodePrivateId;
// 	CString	         m_edt_a_nodeId;
// 	CString	         m_comb_msgType;
// 	CString	         m_comb_msgFlag;
// 	CString	         m_comb_cmd;
	// Generated message map functions
	//{{AFX_MSG(CLogViewDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonOpen();
	virtual void OnOK();
	afx_msg void OnButtonPrev();
	afx_msg void OnButtonNext();
	afx_msg void OnDblclkLISTHead(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGVIEWDLG_H__F4EB5086_C13F_41D0_8C9C_F44A0CF801E7__INCLUDED_)
