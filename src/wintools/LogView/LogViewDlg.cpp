// LogViewDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LogView.h"
#include "LogViewDlg.h"
#include "public.h"
#include "DrebLogData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogViewDlg dialog

CLogViewDlg::CLogViewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLogViewDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLogViewDlg)
	m_csCmd = _T("");
	m_csMsgFlag = _T("");
	m_csMsgType = _T("");
	m_cs_a_nodeid = _T("");
	m_cs_a_nodepid = _T("");
	m_cs_a_retCode = _T("");
	m_cs_bpuindex = _T("");
	m_cs_btime = _T("");
	m_cs_d_nodeid = _T("");
	m_cs_d_nodepid = _T("");
	m_cs_d_serviceNo = _T("");
	m_cs_d_svrid = _T("");
	m_cs_d_svrpid = _T("");
	m_cs_drebAffirm = _T("");
	m_cs_etime = _T("");
	m_cs_index = _T("");
	m_cs_len = _T("");
	m_cs_n_nextno = _T("");
	m_cs_n_nextoffset = _T("");
	m_cs_nextflag = _T("");
	m_cs_raflag = _T("");
	m_cs_s_drebserial = _T("");
	m_cs_s_hook = _T("");
	m_cs_s_index = _T("");
	m_cs_s_nodeid = _T("");
	m_cs_s_nodepid = _T("");
	m_cs_s_serial = _T("");
	m_cs_s_svrpid = _T("");
	m_cs_s_svrid = _T("");
	m_cs_zip = _T("");
	m_cs_gateindex = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLogViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLogViewDlg)
	DDX_Control(pDX, IDC_EDIT_S_GATESERIAL, m_edt_s_gateSerial);
	DDX_Control(pDX, IDC_BUTTON_NEXT, m_btn_next);
	DDX_Control(pDX, IDC_BUTTON_PREV, m_btn_prev);
	DDX_Control(pDX, IDC_EDIT_S_SVRPRIVATE, m_edt_s_svrPrivate);
	DDX_Control(pDX, IDC_EDIT_S_SVR, m_edt_s_svr);
	DDX_Control(pDX, IDC_EDIT_ZIP, m_edt_zip);
	DDX_Control(pDX, IDC_EDIT_S_SERIAL, m_edt_s_serial);
	DDX_Control(pDX, IDC_EDIT_S_NODEPRIVATE, m_edt_s_nodePrivate);
	DDX_Control(pDX, IDC_EDIT_S_NODE, m_edt_s_node);
	DDX_Control(pDX, IDC_EDIT_S_INDEX, m_edt_s_index);
	DDX_Control(pDX, IDC_EDIT_S_HOOK, m_edt_s_hook);
	DDX_Control(pDX, IDC_EDIT_S_DREBSERIAL, m_edt_s_drebSerial);
	DDX_Control(pDX, IDC_EDIT_RAFLAG, m_edt_raFlag);
	DDX_Control(pDX, IDC_EDIT_NEXTFLAG, m_edt_nextFlag);
	DDX_Control(pDX, IDC_EDIT_N_NEXTOFFSET, m_edt_n_nextOffset);
	DDX_Control(pDX, IDC_EDIT_N_NEXTNO, m_edt_n_nextNo);
	DDX_Control(pDX, IDC_EDIT_LEN, m_edt_len);
	DDX_Control(pDX, IDC_EDIT_INDEX, m_edt_index);
	DDX_Control(pDX, IDC_EDIT_ETIME, m_edt_etime);
	DDX_Control(pDX, IDC_EDIT_DREBAFFIRM, m_edt_drebAffirm);
	DDX_Control(pDX, IDC_EDIT_D_SVRPRIVATEID, m_edt_d_svrPrivateId);
	DDX_Control(pDX, IDC_EDIT_D_SVRMAINID, m_edt_d_svrMainId);
	DDX_Control(pDX, IDC_EDIT_D_SERVICENO, m_edt_d_serviceNo);
	DDX_Control(pDX, IDC_EDIT_D_NODEPRIVATEID, m_edt_d_nodePrivateId);
	DDX_Control(pDX, IDC_EDIT_D_NODEID, m_edt_d_nodeid);
	DDX_Control(pDX, IDC_EDIT_BTIME, m_edt_btime);
	DDX_Control(pDX, IDC_EDIT_BPUINDEX, m_edt_bpuIndex);
	DDX_Control(pDX, IDC_EDIT_A_RETCODE, m_edt_a_retCode);
	DDX_Control(pDX, IDC_EDIT_A_NODEPRIVATEID, m_edt_a_nodePrivateId);
	DDX_Control(pDX, IDC_EDIT_A_NODEID, m_edt_a_nodeId);
	DDX_Control(pDX, IDC_COMBO_MSGTYPE, m_comb_msgType);
	DDX_Control(pDX, IDC_COMBO_MSGFLAG, m_comb_msgFlag);
	DDX_Control(pDX, IDC_COMBO_CMD, m_comb_cmd);
	DDX_Control(pDX, IDC_STATIC_LIST, m_static_list);
	DDX_Control(pDX, IDC_LIST_Head, m_lstLogHead);
	DDX_CBString(pDX, IDC_COMBO_CMD, m_csCmd);
	DDX_CBString(pDX, IDC_COMBO_MSGFLAG, m_csMsgFlag);
	DDX_CBString(pDX, IDC_COMBO_MSGTYPE, m_csMsgType);
	DDX_Text(pDX, IDC_EDIT_A_NODEID, m_cs_a_nodeid);
	DDX_Text(pDX, IDC_EDIT_A_NODEPRIVATEID, m_cs_a_nodepid);
	DDX_Text(pDX, IDC_EDIT_A_RETCODE, m_cs_a_retCode);
	DDX_Text(pDX, IDC_EDIT_BPUINDEX, m_cs_bpuindex);
	DDX_Text(pDX, IDC_EDIT_BTIME, m_cs_btime);
	DDV_MaxChars(pDX, m_cs_btime, 6);
	DDX_Text(pDX, IDC_EDIT_D_NODEID, m_cs_d_nodeid);
	DDX_Text(pDX, IDC_EDIT_D_NODEPRIVATEID, m_cs_d_nodepid);
	DDX_Text(pDX, IDC_EDIT_D_SERVICENO, m_cs_d_serviceNo);
	DDX_Text(pDX, IDC_EDIT_D_SVRMAINID, m_cs_d_svrid);
	DDX_Text(pDX, IDC_EDIT_D_SVRPRIVATEID, m_cs_d_svrpid);
	DDX_Text(pDX, IDC_EDIT_DREBAFFIRM, m_cs_drebAffirm);
	DDX_Text(pDX, IDC_EDIT_ETIME, m_cs_etime);
	DDV_MaxChars(pDX, m_cs_etime, 6);
	DDX_Text(pDX, IDC_EDIT_INDEX, m_cs_index);
	DDX_Text(pDX, IDC_EDIT_LEN, m_cs_len);
	DDX_Text(pDX, IDC_EDIT_N_NEXTNO, m_cs_n_nextno);
	DDX_Text(pDX, IDC_EDIT_N_NEXTOFFSET, m_cs_n_nextoffset);
	DDX_Text(pDX, IDC_EDIT_NEXTFLAG, m_cs_nextflag);
	DDX_Text(pDX, IDC_EDIT_RAFLAG, m_cs_raflag);
	DDX_Text(pDX, IDC_EDIT_S_DREBSERIAL, m_cs_s_drebserial);
	DDX_Text(pDX, IDC_EDIT_S_HOOK, m_cs_s_hook);
	DDX_Text(pDX, IDC_EDIT_S_INDEX, m_cs_s_index);
	DDX_Text(pDX, IDC_EDIT_S_NODE, m_cs_s_nodeid);
	DDX_Text(pDX, IDC_EDIT_S_NODEPRIVATE, m_cs_s_nodepid);
	DDX_Text(pDX, IDC_EDIT_S_SERIAL, m_cs_s_serial);
	DDX_Text(pDX, IDC_EDIT_S_SVRPRIVATE, m_cs_s_svrpid);
	DDX_Text(pDX, IDC_EDIT_S_SVR, m_cs_s_svrid);
	DDX_Text(pDX, IDC_EDIT_ZIP, m_cs_zip);
	DDX_Text(pDX, IDC_EDIT_S_GATESERIAL, m_cs_gateindex);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLogViewDlg, CDialog)
	//{{AFX_MSG_MAP(CLogViewDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_OPEN, OnButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_PREV, OnButtonPrev)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, OnButtonNext)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_Head, OnDblclkLISTHead)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogViewDlg message handlers

BOOL CLogViewDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	
	m_bIsDrebLog = true;

	m_comb_cmd.AddString("");
	m_comb_cmd.AddString("1 CMD_ROUTER");
	m_comb_cmd.AddString("2 CMD_DEL_NODE");
	m_comb_cmd.AddString("3 CMD_DEL_SVR");
	m_comb_cmd.AddString("4 CMD_PING");
	m_comb_cmd.AddString("5 CMD_CONNECT");
	m_comb_cmd.AddString("6 CMD_SERVICE");
	m_comb_cmd.AddString("7 CMD_REGSERVICE");
	m_comb_cmd.AddString("8 CMD_DPCALL");
	m_comb_cmd.AddString("9 CMD_DPACALL");
	m_comb_cmd.AddString("10 CMD_DPBC");
	m_comb_cmd.AddString("11 CMD_DPABC");
	m_comb_cmd.AddString("12 CMD_DPPUSH");
	m_comb_cmd.AddString("13 CMD_DPPOST");
	m_comb_cmd.AddString("15 CMD_MONITOR_DREB");
	m_comb_cmd.AddString("16 CMD_MONITOR_BPC");
	
	m_comb_msgFlag.AddString("");
	m_comb_msgFlag.AddString("0128");
	m_comb_msgFlag.AddString("2801");

	m_comb_msgType.AddString("");
	m_comb_msgType.AddString("0 MSG_REQ");
	m_comb_msgType.AddString("1 MSG_FREE");
	m_comb_msgType.AddString("4 MSG_GETSERIAL");
	m_comb_msgType.AddString("5 MSG_SETGPARA1");
	m_comb_msgType.AddString("6 MSG_SETGPARA2");
	m_comb_msgType.AddString("7 MSG_SETGPARA3");
	m_comb_msgType.AddString("9 MSG_TRANS");
	m_comb_msgType.AddString("10 MSG_EXTCALL");
	m_comb_msgType.AddString("11 MSG_BPCCALL");
	m_comb_msgType.AddString("12 MSG_BPUREG");
	m_comb_msgType.AddString("13 MSG_BPUISREG");
	m_comb_msgType.AddString("15 MSG_GETNEXT");

	m_btn_next.EnableWindow(false);
	m_btn_prev.EnableWindow(false);
	
	m_nCurDisplayIndex = 0;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CLogViewDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLogViewDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CLogViewDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CLogViewDlg::OnButtonOpen() 
{
	// TODO: Add your control notification handler code here
	char filename[400];
	memset(filename,0,sizeof(filename));
	WORKPATH(filename);
	CString ss;
	int ret;
	m_static_list.SetWindowText("");
	CFileDialog   dlg(true,"","*.*",OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"ALL Files (*.*)|",NULL);
	ret = dlg.DoModal();
	ss = dlg.GetPathName();
	
	if (ret !=IDOK)
	{
		return;
	}
	//将工作路径设置回去
	SetCurrentDirectory(filename);
	UpdateData(false);
	
	m_sLogFilePath = ss.GetBuffer(ss.GetLength());
	FILE *fp = fopen(ss.GetBuffer(ss.GetLength()),"rb");
	if (fp == NULL)
	{
		MessageBox("打开文件出错","错误");
		return;
	}
	char data[10];
	ret = fread(data,1,2,fp);
	if (ret != 2)
	{
		MessageBox("文件非法","错误");
		return;
	}
	if (data[0] != 'L' && data[0] != 'B')
	{
		MessageBox("文件非法，第一个字节应为B或L","错误");
		return;
	}
	if (data[0] == 'L')
	{
		m_pBpcEndian.SetCommEndian(true);
		m_pDrebEndian.SetCommEndian(true);
	}
	else
	{
		m_pBpcEndian.SetCommEndian(false);
		m_pDrebEndian.SetCommEndian(false);
	}
	
	if (data[1] != '0' && data[1] != '1')
	{
		MessageBox("文件非法，第二个字节应为0或1","错误");
		return;
	}
	if (data[1] == '0')
	{
		m_bIsDrebLog = true;
		m_edt_bpuIndex.EnableWindow(false);
		m_edt_index.EnableWindow(false);
		m_comb_msgFlag.EnableWindow(false);
		m_comb_msgType.EnableWindow(false);

	}
	else
	{
		m_bIsDrebLog = false;
		m_edt_bpuIndex.EnableWindow(true);
		m_edt_index.EnableWindow(true);
		m_comb_msgFlag.EnableWindow(true);
		m_comb_msgType.EnableWindow(true);
	}
	m_static_list.SetWindowText(m_sLogFilePath.c_str());
	//读文件，将头信息读到vector中，供查询及写数据
	if (!ReadFileData(fp))
	{
		MessageBox("解析日志文件出错","错误");
		fclose(fp);
		return;
	}
	fclose(fp);
	//显示表头
	DisplayHead();
	//显示数据
	DisplayData();
}

bool CLogViewDlg::ReadFileData(FILE *fp)
{
	if (m_bIsDrebLog)
	{
		return ReadDrebLogFile(fp);
	}
	else
	{
		return ReadBpcLogFile(fp);
	}
}

bool CLogViewDlg::ReadDrebLogFile(FILE *fp)
{
	int recNum=0;
	int ret;
	char tmpdata[DREBBUFSIZE];
	m_vDrebHeadList.clear();
	S_DREBHEADLIST dhead;
	
	int offset = 2;
	while (true)
	{
		bzero(&dhead,sizeof(S_DREBHEADLIST));
		dhead.id = recNum;
		ret = fread(&(dhead.head),1,DREBLOGHEADLEN,fp);
		if (ret != DREBLOGHEADLEN)
		{
			break;
		}
		
		m_pDrebEndian.Endian2LocalHost((unsigned char *)&(dhead.head.nId),sizeof(dhead.head.nId));
		m_pDrebEndian.Endian2LocalHost((unsigned char *)&(dhead.head.nTime),sizeof(dhead.head.nTime));
		m_pDrebEndian.Endian2LocalHost((unsigned char *)&(dhead.head.nUs),sizeof(dhead.head.nUs));
		m_pDrebEndian.Endian2LocalHost(&(dhead.head.head));
		offset +=DREBLOGHEADLEN;
		dhead.ndatabegin = offset;
		dhead.ndataend = offset+dhead.head.head.nLen;
		offset+=dhead.head.head.nLen;
		m_vDrebHeadList.push_back(dhead);
		recNum++;
		if (dhead.head.head.nLen>0)
		{
			ret = fread(tmpdata,1,dhead.head.head.nLen,fp);
			if (ret != dhead.head.head.nLen)
			{
				MessageBox("读日志文件的数据出错","错误");
				break;
			}
		}
		

	}
	return true;
}

bool CLogViewDlg::ReadBpcLogFile(FILE *fp)
{
	m_vBpcHeadList.clear();
	int recNum=0;
	int ret;
	char tmpdata[DREBBUFSIZE];
	m_vDrebHeadList.clear();
	S_BPCHEADLIST bhead;
	
	int offset = 2;
	while (true)
	{
		bzero(&bhead,BPCLOGHEADLEN);
		bhead.id = recNum;
		ret = fread(&(bhead.head),1,BPCLOGHEADLEN,fp);
		if (ret != BPCLOGHEADLEN)
		{
			break;
		}
		m_pBpcEndian.Endian2LocalHost((unsigned char *)&(bhead.head.nId),sizeof(bhead.head.nId));
		m_pBpcEndian.Endian2LocalHost((unsigned char *)&(bhead.head.nTime),sizeof(bhead.head.nTime));
		m_pBpcEndian.Endian2LocalHost((unsigned char *)&(bhead.head.nUs),sizeof(bhead.head.nUs));
		m_pBpcEndian.Endian2LocalHost((PBPCCOMMSTRU)&(bhead.head.sBpcHead));
		offset +=BPCLOGHEADLEN;
		bhead.ndatabegin = offset;
		bhead.ndataend = offset+bhead.head.sDBHead.nLen;
		offset+=bhead.head.sDBHead.nLen;
		m_vBpcHeadList.push_back(bhead);
		recNum++;
		if (bhead.head.sDBHead.nLen>0)
		{
			ret = fread(tmpdata,1,bhead.head.sDBHead.nLen,fp);
			if (ret != bhead.head.sDBHead.nLen)
			{
				MessageBox("读日志文件的数据出错","错误");
				break;
			}
		}

	}
	return true;
}
void CLogViewDlg::DisplayData()
{
	CInt  list;
	if (m_bIsDrebLog)
	{
		for ( int i=0;i<m_vDrebHeadList.size();i++)
		{
			if (i>500)
			{
				m_nCurDisplayIndex = i;
				m_btn_next.EnableWindow(true);
				break;
			}
			
			list.Add(i);
		}
	}
	else
	{
		for ( int i=0;i<m_vBpcHeadList.size();i++)
		{
			if (i>500)
			{
				m_nCurDisplayIndex = i;
				m_btn_next.EnableWindow(true);
				break;
			}
			list.Add(i);
		}
	}
	DisplayData(list);
	
}
void CLogViewDlg::DisplayData(CInt dataindex)
{
	bool bRet;
	int id;
	m_lstLogHead.DeleteAllItems();
	bRet = dataindex.First(id);
	while (bRet)
	{
		DisplayDataItem(id);
		bRet = dataindex.Next(id);
	}

}

void CLogViewDlg::DisplayHead()
{
	CHeaderCtrl* pHeaderCtrl = m_lstLogHead.GetHeaderCtrl();
	if (pHeaderCtrl != NULL)
	{
		int  nColumnCount = pHeaderCtrl->GetItemCount();
		for (int i=0; i<nColumnCount; i++)
		{
			m_lstLogHead.DeleteColumn(0);
		}
	} 
	
	if (m_bIsDrebLog)
	{
		
		m_lstLogHead.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_HEADERDRAGDROP);
		m_lstLogHead.InsertColumn(0, "序号", LVCFMT_CENTER, 40);
		m_lstLogHead.InsertColumn(1, "时间", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(2, "微秒", LVCFMT_CENTER, 50);
		m_lstLogHead.InsertColumn(3, "进程", LVCFMT_CENTER, 40);
		m_lstLogHead.InsertColumn(4, "cZip", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(5, "cCmd", LVCFMT_CENTER, 80);
		m_lstLogHead.InsertColumn(6, "cRaflag", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(7, "cNextFlag", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(8, "cDrebAffirm", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(9, "s_nNodeId", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(10, "s_cNodePrivateId", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(11, "s_nSvrMainId", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(12, "s_cSvrPrivateId", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(13, "s_nHook", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(14, "s_nSerial", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(15, "s_nDrebSerial", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(16, "s_nGateIndex", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(17, "s_nIndex", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(18, "d_nNodeId", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(19, "d_cNodePrivateId", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(20, "d_nSvrMainId", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(21, "d_cSvrPrivateId", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(22, "d_nServiceNo", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(23, "a_nNodeId", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(24, "a_cNodePrivateId", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(25, "a_nRetCode", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(26, "n_nNextNo", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(27, "n_nNextOffset", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(28, "b_cIndex", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(29, "b_nSerial", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(30, "nLen", LVCFMT_CENTER, 60);
	}
	else
	{
		m_lstLogHead.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_HEADERDRAGDROP);
		m_lstLogHead.InsertColumn(0, "序号", LVCFMT_CENTER, 40);
		m_lstLogHead.InsertColumn(1, "时间", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(2, "微秒", LVCFMT_CENTER, 50);
		m_lstLogHead.InsertColumn(3, "进程", LVCFMT_CENTER, 40);
		m_lstLogHead.InsertColumn(4, "cMsgType", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(5, "nMsgFlag", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(6, "nIndex", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(7, "nBpuIndex", LVCFMT_CENTER, 60);
		
		m_lstLogHead.InsertColumn(8, "cZip", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(9, "cCmd", LVCFMT_CENTER, 80);
		m_lstLogHead.InsertColumn(10, "cRaflag", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(11, "cNextFlag", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(12, "cDrebAffirm", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(13, "s_nNodeId", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(14, "s_cNodePrivateId", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(15, "s_nSvrMainId", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(16, "s_cSvrPrivateId", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(17, "s_nHook", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(18, "s_nSerial", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(19, "s_nDrebSerial", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(20, "s_nGateIndex", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(21, "s_nIndex", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(22, "d_nNodeId", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(23, "d_cNodePrivateId", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(24, "d_nSvrMainId", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(25, "d_cSvrPrivateId", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(26, "d_nServiceNo", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(27, "a_nNodeId", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(28, "a_cNodePrivateId", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(29, "a_nRetCode", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(30, "n_nNextNo", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(31, "n_nNextOffset", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(32, "b_cIndex", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(33, "b_nSerial", LVCFMT_CENTER, 60);
		m_lstLogHead.InsertColumn(34, "nLen", LVCFMT_CENTER, 60);
	}
}

void CLogViewDlg::DisplayDataItem(int id)
{
	int recNum;
	int nItem;
	char tmpchar[200];
	recNum = m_lstLogHead.GetItemCount();
	sprintf(tmpchar,"%d",id);
	nItem = m_lstLogHead.InsertItem(recNum,tmpchar);
	if (m_bIsDrebLog)
	{
		sprintf(tmpchar,"%06d",m_vDrebHeadList[id].head.nTime);
		m_lstLogHead.SetItemText(nItem,1,tmpchar);
		sprintf(tmpchar,"%d",m_vDrebHeadList[id].head.nUs);
		m_lstLogHead.SetItemText(nItem,2,tmpchar);
		sprintf(tmpchar,"%d",m_vDrebHeadList[id].head.nId);
		m_lstLogHead.SetItemText(nItem,3,tmpchar);
		sprintf(tmpchar,"%d",m_vDrebHeadList[id].head.head.cZip);
		m_lstLogHead.SetItemText(nItem,4,tmpchar);
		
		m_lstLogHead.SetItemText(nItem,5,GetDrebCmdType(m_vDrebHeadList[id].head.head.cCmd).c_str());
		sprintf(tmpchar,"%d",m_vDrebHeadList[id].head.head.cRaflag);
		m_lstLogHead.SetItemText(nItem,6,tmpchar);
		sprintf(tmpchar,"%d",m_vDrebHeadList[id].head.head.cNextFlag);
		m_lstLogHead.SetItemText(nItem,7,tmpchar);
		sprintf(tmpchar,"%d",m_vDrebHeadList[id].head.head.cDrebAffirm);
		m_lstLogHead.SetItemText(nItem,8,tmpchar);
		sprintf(tmpchar,"%d",m_vDrebHeadList[id].head.head.s_Sinfo.s_nNodeId);
		m_lstLogHead.SetItemText(nItem,9,tmpchar);
		sprintf(tmpchar,"%d",m_vDrebHeadList[id].head.head.s_Sinfo.s_cNodePrivateId);
		m_lstLogHead.SetItemText(nItem,10,tmpchar);
		sprintf(tmpchar,"%d",m_vDrebHeadList[id].head.head.s_Sinfo.s_nSvrMainId);
		m_lstLogHead.SetItemText(nItem,11,tmpchar);
		sprintf(tmpchar,"%d",m_vDrebHeadList[id].head.head.s_Sinfo.s_cSvrPrivateId);
		m_lstLogHead.SetItemText(nItem,12,tmpchar);
		sprintf(tmpchar,"%d",m_vDrebHeadList[id].head.head.s_Sinfo.s_nHook);
		m_lstLogHead.SetItemText(nItem,13,tmpchar);
		sprintf(tmpchar,"%d",m_vDrebHeadList[id].head.head.s_Sinfo.s_nSerial);
		m_lstLogHead.SetItemText(nItem,14,tmpchar);
		sprintf(tmpchar,"%d",m_vDrebHeadList[id].head.head.s_Sinfo.s_nDrebSerial);

		m_lstLogHead.SetItemText(nItem,15,tmpchar);
		sprintf(tmpchar,"%d",m_vDrebHeadList[id].head.head.s_Sinfo.s_nGateIndex);
		

		m_lstLogHead.SetItemText(nItem,16,tmpchar);
		sprintf(tmpchar,"%d",m_vDrebHeadList[id].head.head.s_Sinfo.s_nIndex);

		m_lstLogHead.SetItemText(nItem,17,tmpchar);
		sprintf(tmpchar,"%d",m_vDrebHeadList[id].head.head.d_Dinfo.d_nNodeId);
		m_lstLogHead.SetItemText(nItem,18,tmpchar);
		sprintf(tmpchar,"%d",m_vDrebHeadList[id].head.head.d_Dinfo.d_cNodePrivateId);
		m_lstLogHead.SetItemText(nItem,19,tmpchar);
		sprintf(tmpchar,"%d",m_vDrebHeadList[id].head.head.d_Dinfo.d_nSvrMainId);
		m_lstLogHead.SetItemText(nItem,20,tmpchar);
		sprintf(tmpchar,"%d",m_vDrebHeadList[id].head.head.d_Dinfo.d_cSvrPrivateId);
		m_lstLogHead.SetItemText(nItem,21,tmpchar);
		sprintf(tmpchar,"%d",m_vDrebHeadList[id].head.head.d_Dinfo.d_nServiceNo);
		m_lstLogHead.SetItemText(nItem,22,tmpchar);
		sprintf(tmpchar,"%d",m_vDrebHeadList[id].head.head.a_Ainfo.a_nNodeId);
		m_lstLogHead.SetItemText(nItem,23,tmpchar);
		sprintf(tmpchar,"%d",m_vDrebHeadList[id].head.head.a_Ainfo.a_cNodePrivateId);
		m_lstLogHead.SetItemText(nItem,24,tmpchar);
		sprintf(tmpchar,"%d",m_vDrebHeadList[id].head.head.a_Ainfo.a_nRetCode);
		m_lstLogHead.SetItemText(nItem,25,tmpchar);
		sprintf(tmpchar,"%d",m_vDrebHeadList[id].head.head.n_Ninfo.n_nNextNo);
		m_lstLogHead.SetItemText(nItem,26,tmpchar);
		sprintf(tmpchar,"%d",m_vDrebHeadList[id].head.head.n_Ninfo.n_nNextOffset);
		m_lstLogHead.SetItemText(nItem,27,tmpchar);
		sprintf(tmpchar,"%d",m_vDrebHeadList[id].head.head.b_Cinfo.b_cIndex);
		m_lstLogHead.SetItemText(nItem,28,tmpchar);
		sprintf(tmpchar,"%d",m_vDrebHeadList[id].head.head.b_Cinfo.b_nSerial);
		m_lstLogHead.SetItemText(nItem,29,tmpchar);
		sprintf(tmpchar,"%d",m_vDrebHeadList[id].head.head.nLen);
		m_lstLogHead.SetItemText(nItem,30,tmpchar);

	}
	else
	{
		sprintf(tmpchar,"%06d",m_vBpcHeadList[id].head.nTime);
		m_lstLogHead.SetItemText(nItem,1,tmpchar);
		sprintf(tmpchar,"%d",m_vBpcHeadList[id].head.nUs);
		m_lstLogHead.SetItemText(nItem,2,tmpchar);
		sprintf(tmpchar,"%d",m_vBpcHeadList[id].head.nId);
		m_lstLogHead.SetItemText(nItem,3,tmpchar);

		m_lstLogHead.SetItemText(nItem,4,GetBpcMsgType(m_vBpcHeadList[id].head.sBpcHead.cMsgType).c_str());
		sprintf(tmpchar,"%x",m_vBpcHeadList[id].head.sBpcHead.nMsgFlag);
		m_lstLogHead.SetItemText(nItem,5,tmpchar);
		sprintf(tmpchar,"%d",m_vBpcHeadList[id].head.sBpcHead.nIndex);
		m_lstLogHead.SetItemText(nItem,6,tmpchar);
		sprintf(tmpchar,"%d",m_vBpcHeadList[id].head.sBpcHead.nBpuIndex);
		m_lstLogHead.SetItemText(nItem,7,tmpchar);

		sprintf(tmpchar,"%d",m_vBpcHeadList[id].head.sDBHead.cZip);
		m_lstLogHead.SetItemText(nItem,8,tmpchar);
		m_lstLogHead.SetItemText(nItem,9,GetDrebCmdType(m_vBpcHeadList[id].head.sDBHead.cCmd).c_str());
		sprintf(tmpchar,"%d",m_vBpcHeadList[id].head.sDBHead.cRaflag);
		m_lstLogHead.SetItemText(nItem,10,tmpchar);
		sprintf(tmpchar,"%d",m_vBpcHeadList[id].head.sDBHead.cNextFlag);
		m_lstLogHead.SetItemText(nItem,11,tmpchar);
		sprintf(tmpchar,"%d",m_vBpcHeadList[id].head.sDBHead.cDrebAffirm);
		m_lstLogHead.SetItemText(nItem,12,tmpchar);
		sprintf(tmpchar,"%d",m_vBpcHeadList[id].head.sDBHead.s_Sinfo.s_nNodeId);
		m_lstLogHead.SetItemText(nItem,13,tmpchar);
		sprintf(tmpchar,"%d",m_vBpcHeadList[id].head.sDBHead.s_Sinfo.s_cNodePrivateId);
		m_lstLogHead.SetItemText(nItem,14,tmpchar);
		sprintf(tmpchar,"%d",m_vBpcHeadList[id].head.sDBHead.s_Sinfo.s_nSvrMainId);
		m_lstLogHead.SetItemText(nItem,15,tmpchar);
		sprintf(tmpchar,"%d",m_vBpcHeadList[id].head.sDBHead.s_Sinfo.s_cSvrPrivateId);
		m_lstLogHead.SetItemText(nItem,16,tmpchar);
		sprintf(tmpchar,"%d",m_vBpcHeadList[id].head.sDBHead.s_Sinfo.s_nHook);
		m_lstLogHead.SetItemText(nItem,17,tmpchar);
		sprintf(tmpchar,"%d",m_vBpcHeadList[id].head.sDBHead.s_Sinfo.s_nSerial);
		m_lstLogHead.SetItemText(nItem,18,tmpchar);
		sprintf(tmpchar,"%d",m_vBpcHeadList[id].head.sDBHead.s_Sinfo.s_nDrebSerial);
		m_lstLogHead.SetItemText(nItem,19,tmpchar);

		sprintf(tmpchar,"%d",m_vBpcHeadList[id].head.sDBHead.s_Sinfo.s_nGateIndex);
		m_lstLogHead.SetItemText(nItem,20,tmpchar);

		sprintf(tmpchar,"%d",m_vBpcHeadList[id].head.sDBHead.s_Sinfo.s_nIndex);
		m_lstLogHead.SetItemText(nItem,21,tmpchar);
		sprintf(tmpchar,"%d",m_vBpcHeadList[id].head.sDBHead.d_Dinfo.d_nNodeId);
		m_lstLogHead.SetItemText(nItem,22,tmpchar);
		sprintf(tmpchar,"%d",m_vBpcHeadList[id].head.sDBHead.d_Dinfo.d_cNodePrivateId);
		m_lstLogHead.SetItemText(nItem,23,tmpchar);
		sprintf(tmpchar,"%d",m_vBpcHeadList[id].head.sDBHead.d_Dinfo.d_nSvrMainId);
		m_lstLogHead.SetItemText(nItem,24,tmpchar);
		sprintf(tmpchar,"%d",m_vBpcHeadList[id].head.sDBHead.d_Dinfo.d_cSvrPrivateId);
		m_lstLogHead.SetItemText(nItem,25,tmpchar);
		sprintf(tmpchar,"%d",m_vBpcHeadList[id].head.sDBHead.d_Dinfo.d_nServiceNo);
		m_lstLogHead.SetItemText(nItem,26,tmpchar);
		sprintf(tmpchar,"%d",m_vBpcHeadList[id].head.sDBHead.a_Ainfo.a_nNodeId);
		m_lstLogHead.SetItemText(nItem,27,tmpchar);
		sprintf(tmpchar,"%d",m_vBpcHeadList[id].head.sDBHead.a_Ainfo.a_cNodePrivateId);
		m_lstLogHead.SetItemText(nItem,28,tmpchar);
		sprintf(tmpchar,"%d",m_vBpcHeadList[id].head.sDBHead.a_Ainfo.a_nRetCode);
		m_lstLogHead.SetItemText(nItem,29,tmpchar);
		sprintf(tmpchar,"%d",m_vBpcHeadList[id].head.sDBHead.n_Ninfo.n_nNextNo);
		m_lstLogHead.SetItemText(nItem,30,tmpchar);
		sprintf(tmpchar,"%d",m_vBpcHeadList[id].head.sDBHead.n_Ninfo.n_nNextOffset);
		m_lstLogHead.SetItemText(nItem,31,tmpchar);
		sprintf(tmpchar,"%d",m_vBpcHeadList[id].head.sDBHead.b_Cinfo.b_cIndex);
		m_lstLogHead.SetItemText(nItem,32,tmpchar);
		sprintf(tmpchar,"%d",m_vBpcHeadList[id].head.sDBHead.b_Cinfo.b_nSerial);
		m_lstLogHead.SetItemText(nItem,33,tmpchar);
		sprintf(tmpchar,"%d",m_vBpcHeadList[id].head.sDBHead.nLen);
		m_lstLogHead.SetItemText(nItem,34,tmpchar);
	}
}

std::string CLogViewDlg::GetBpcMsgType(int msgtype)
{
	switch (msgtype)
	{
	case MSG_REQ:        // 0      //接收请求,bpc发给BPU的请求
		return "MSG_REQ";
	case MSG_FREE:        //1      //BPU发送空闲请求
		return "MSG_FREE";
	case MSG_GETSERIAL:   //4      //BPU取共享唯一流水
		return "MSG_GETSERIAL";
	case MSG_SETGPARA1:  //5      //BPU发送SETGPARA1
		return "MSG_SETGPARA1";
	case MSG_SETGPARA2:   //6      //BPU发送SETGPARA2
		return "MSG_SETGPARA2";
	case MSG_SETGPARA3:   //7      //BPU发送SETGPARA3
		return "MSG_SETGPARA3";	
	case MSG_EXTCALL:      //10      //BPU发送外调请求	
		return "MSG_EXTCALL";
	case MSG_BPCCALL:      //11      //BPU发送PU组调用请求
		return "MSG_BPCCALL";
	case MSG_BPUREG:       //12      //BPU发送注册请求
		return "MSG_BPUREG";
	case MSG_BPUISREG:     //13      //BPU发送是否注册请求
		return "MSG_BPUISREG";
	case MSG_GETNEXT:      //15      //dreb过来的取后续包的消息
		return "MSG_GETNEXT";
	default:
		return "";
	}
}
std::string CLogViewDlg::GetDrebCmdType(int cmdtype)
{
	switch (cmdtype)
	{
		
	case  CMD_ROUTER:      //1  //数据总线节点发送路由通知的命令字
		return "CMD_ROUTER";
	case  CMD_DEL_NODE:    //2  //数据总线节点之间取消数据总线节点的命令字,只发送至主动连接的数据总线节点
		return "CMD_DEL_NODE";
	case  CMD_DEL_SVR:     //3  //服务端取消注册服务的命令字
		return "CMD_DEL_SVR";
	case  CMD_PING:        //4  //心跳请求的命令字
		return "CMD_PING";
	case  CMD_CONNECT:     //5  //连接注册
		return "CMD_CONNECT";
	case  CMD_SERVICE:     //6  //服务路由
		return "CMD_SERVICE";
	case  CMD_REGSERVICE:  //7  //注册服务
		return "CMD_REGSERVICE";			
	case  CMD_DPCALL:      //8  //数据总线节点同步调用 要求最终处理完成后应答
		return "CMD_DPCALL";
	case  CMD_DPACALL:     //9  //数据总线节点异步调用 要求接收到的数据总线节点发送至服务后确认应答
		return "CMD_DPACALL";
	case  CMD_DPBC:        //10  //数据总线节点广播，即将信息发给指定数据总线节点上所有注册的服务,要求数据总线节点应答
		return "CMD_DPBC";
	case  CMD_DPABC:       //11  //数据总线节点广播，即将信息发给指定数据总线节点上所有注册的服务,不要求数据总线节点应答
		return "CMD_DPABC";			
	case  CMD_DPPUSH:      //12 //数据总线节点推送，无须应答
		return "CMD_DPPUSH";
	case  CMD_DPPOST:      //13 //数据总线节点投递，要求接收到的数据总线节点应答
		return "CMD_DPPOST";			
	case  CMD_MONITOR_DREB:     //15 //数据总线节点监控
		return "CMD_MONITOR_DREB";
	case  CMD_MONITOR_BPC:     //15 //数据总线节点监控
		return "CMD_MONITOR_BPC";
	default:
		return "";
	}
}

void CLogViewDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(true);
	GetQueryData();	
	m_btn_next.EnableWindow(false);
	m_btn_prev.EnableWindow(false);
	CInt  idlist;
	if (m_bIsDrebLog)
	{
		for (int i=0;i<m_vDrebHeadList.size();i++)
		{
			if (IsResult(i))
			{
				idlist.Add(i);
			}
		}
	}
	else
	{
		for (int i=0;i<m_vBpcHeadList.size();i++)
		{
			if (IsResult(i))
			{
				idlist.Add(i);
			}
		}
	}
	DisplayData(idlist);
	

//	CDialog::OnOK();
}

bool CLogViewDlg::IsResult(int id)
{
	if (!m_bIsDrebLog)
	{
		if (m_csMsgFlag.GetLength()>0)
		{
			if (m_csMsgFlag.Compare("0128") == 0)
			{
				if (m_vBpcHeadList[id].head.sBpcHead.nMsgFlag != 296)
				{
					return false;
				}
			}
			else
			{
				if (m_vBpcHeadList[id].head.sBpcHead.nMsgFlag != 10241)
				{
					return false;
				}
			}
		}
		if (m_csMsgType.GetLength()>0)
		{
			if (m_vBpcHeadList[id].head.sBpcHead.cMsgType != atoi(m_csMsgType.GetBuffer(m_csMsgType.GetLength())) )
			{
				return false;
			}
		}
		if (m_cs_bpuindex.GetLength()>0)
		{
			if (m_vBpcHeadList[id].head.sBpcHead.nBpuIndex != atoi(m_cs_bpuindex.GetBuffer(m_cs_bpuindex.GetLength())) )
			{
				return false;
			}
		}
		if (m_cs_index.GetLength()>0)
		{
			if (m_vBpcHeadList[id].head.sBpcHead.nIndex != atoi(m_cs_index.GetBuffer(m_cs_index.GetLength())) )
			{
				return false;
			}
		}

		if (m_cs_btime.GetLength()>0)
		{
			if (m_vBpcHeadList[id].head.nTime < atoi(m_cs_btime.GetBuffer(m_cs_btime.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_etime.GetLength()>0)
		{
			if (m_vBpcHeadList[id].head.nTime > atoi(m_cs_etime.GetBuffer(m_cs_etime.GetLength())) )
			{
				return false;
			} 
		}

		if (m_cs_zip.GetLength()>0)
		{
			if (m_vBpcHeadList[id].head.sDBHead.cZip != atoi(m_cs_zip.GetBuffer(m_cs_zip.GetLength())) )
			{
				return false;
			} 
		}
		if (m_csCmd.GetLength()>0)
		{
			if (m_vBpcHeadList[id].head.sDBHead.cCmd != atoi(m_csCmd.GetBuffer(m_csCmd.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_raflag.GetLength()>0)
		{
			if (m_vBpcHeadList[id].head.sDBHead.cRaflag != atoi(m_cs_raflag.GetBuffer(m_cs_raflag.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_nextflag.GetLength()>0)
		{
			if (m_vBpcHeadList[id].head.sDBHead.cNextFlag != atoi(m_cs_nextflag.GetBuffer(m_cs_nextflag.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_drebAffirm.GetLength()>0)
		{
			if (m_vBpcHeadList[id].head.sDBHead.cDrebAffirm != atoi(m_cs_drebAffirm.GetBuffer(m_cs_drebAffirm.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_s_nodeid.GetLength()>0)
		{
			if (m_vBpcHeadList[id].head.sDBHead.s_Sinfo.s_nNodeId != atoi(m_cs_s_nodeid.GetBuffer(m_cs_s_nodeid.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_s_nodepid.GetLength()>0)
		{
			if (m_vBpcHeadList[id].head.sDBHead.s_Sinfo.s_cNodePrivateId != atoi(m_cs_s_nodepid.GetBuffer(m_cs_s_nodepid.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_s_svrid.GetLength()>0)
		{
			if (m_vBpcHeadList[id].head.sDBHead.s_Sinfo.s_nSvrMainId != atoi(m_cs_s_svrid.GetBuffer(m_cs_s_svrid.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_s_svrpid.GetLength()>0)
		{
			if (m_vBpcHeadList[id].head.sDBHead.s_Sinfo.s_cSvrPrivateId != atoi(m_cs_s_svrpid.GetBuffer(m_cs_s_svrpid.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_s_hook.GetLength()>0)
		{
			if (m_vBpcHeadList[id].head.sDBHead.s_Sinfo.s_nHook != atoi(m_cs_s_hook.GetBuffer(m_cs_s_hook.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_s_serial.GetLength()>0)
		{
			if (m_vBpcHeadList[id].head.sDBHead.s_Sinfo.s_nSerial != atoi(m_cs_s_serial.GetBuffer(m_cs_s_serial.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_s_drebserial.GetLength()>0)
		{
			if (m_vBpcHeadList[id].head.sDBHead.s_Sinfo.s_nDrebSerial != atoi(m_cs_s_drebserial.GetBuffer(m_cs_s_drebserial.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_gateindex.GetLength()>0)
		{
			if (m_vBpcHeadList[id].head.sDBHead.s_Sinfo.s_nGateIndex != atoi(m_cs_gateindex.GetBuffer(m_cs_gateindex.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_s_index.GetLength()>0)
		{
			if (m_vBpcHeadList[id].head.sDBHead.s_Sinfo.s_nIndex != atoi(m_cs_s_index.GetBuffer(m_cs_s_index.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_d_nodeid.GetLength()>0)
		{
			if (m_vBpcHeadList[id].head.sDBHead.d_Dinfo.d_nNodeId != atoi(m_cs_d_nodeid.GetBuffer(m_cs_d_nodeid.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_d_nodepid.GetLength()>0)
		{
			if (m_vBpcHeadList[id].head.sDBHead.d_Dinfo.d_cNodePrivateId != atoi(m_cs_d_nodepid.GetBuffer(m_cs_d_nodepid.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_d_svrid.GetLength()>0)
		{
			if (m_vBpcHeadList[id].head.sDBHead.d_Dinfo.d_nSvrMainId != atoi(m_cs_d_svrid.GetBuffer(m_cs_d_svrid.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_d_svrpid.GetLength()>0)
		{
			if (m_vBpcHeadList[id].head.sDBHead.d_Dinfo.d_cSvrPrivateId != atoi(m_cs_d_svrpid.GetBuffer(m_cs_d_svrpid.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_d_serviceNo.GetLength()>0)
		{
			if (m_vBpcHeadList[id].head.sDBHead.d_Dinfo.d_nServiceNo != atoi(m_cs_d_serviceNo.GetBuffer(m_cs_d_serviceNo.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_a_nodeid.GetLength()>0)
		{
			if (m_vBpcHeadList[id].head.sDBHead.a_Ainfo.a_nNodeId != atoi(m_cs_a_nodeid.GetBuffer(m_cs_a_nodeid.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_a_nodepid.GetLength()>0)
		{
			if (m_vBpcHeadList[id].head.sDBHead.a_Ainfo.a_cNodePrivateId != atoi(m_cs_a_nodepid.GetBuffer(m_cs_a_nodepid.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_a_retCode.GetLength()>0)
		{
			if (m_vBpcHeadList[id].head.sDBHead.a_Ainfo.a_nRetCode != atoi(m_cs_a_retCode.GetBuffer(m_cs_a_retCode.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_n_nextno.GetLength()>0)
		{
			if (m_vBpcHeadList[id].head.sDBHead.n_Ninfo.n_nNextNo != atoi(m_cs_n_nextno.GetBuffer(m_cs_n_nextno.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_n_nextoffset.GetLength()>0)
		{
			if (m_vBpcHeadList[id].head.sDBHead.n_Ninfo.n_nNextOffset != atoi(m_cs_n_nextoffset.GetBuffer(m_cs_n_nextoffset.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_len.GetLength()>0)
		{
			if (m_vBpcHeadList[id].head.sDBHead.nLen != atoi(m_cs_len.GetBuffer(m_cs_len.GetLength())) )
			{
				return false;
			} 
		}
	}
	else
	{
		if (m_cs_btime.GetLength()>0)
		{
			if (m_vDrebHeadList[id].head.nTime < atoi(m_cs_btime.GetBuffer(m_cs_btime.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_etime.GetLength()>0)
		{
			if (m_vDrebHeadList[id].head.nTime > atoi(m_cs_etime.GetBuffer(m_cs_etime.GetLength())) )
			{
				return false;
			} 
		}

		if (m_cs_zip.GetLength()>0)
		{
			if (m_vDrebHeadList[id].head.head.cZip != atoi(m_cs_zip.GetBuffer(m_cs_zip.GetLength())) )
			{
				return false;
			} 
		}
		if (m_csCmd.GetLength()>0)
		{
			if (m_vDrebHeadList[id].head.head.cCmd != atoi(m_csCmd.GetBuffer(m_csCmd.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_raflag.GetLength()>0)
		{
			if (m_vDrebHeadList[id].head.head.cRaflag != atoi(m_cs_raflag.GetBuffer(m_cs_raflag.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_nextflag.GetLength()>0)
		{
			if (m_vDrebHeadList[id].head.head.cNextFlag != atoi(m_cs_nextflag.GetBuffer(m_cs_nextflag.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_drebAffirm.GetLength()>0)
		{
			if (m_vDrebHeadList[id].head.head.cDrebAffirm != atoi(m_cs_drebAffirm.GetBuffer(m_cs_drebAffirm.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_s_nodeid.GetLength()>0)
		{
			if (m_vDrebHeadList[id].head.head.s_Sinfo.s_nNodeId != atoi(m_cs_s_nodeid.GetBuffer(m_cs_s_nodeid.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_s_nodepid.GetLength()>0)
		{
			if (m_vDrebHeadList[id].head.head.s_Sinfo.s_cNodePrivateId != atoi(m_cs_s_nodepid.GetBuffer(m_cs_s_nodepid.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_s_svrid.GetLength()>0)
		{
			if (m_vDrebHeadList[id].head.head.s_Sinfo.s_nSvrMainId != atoi(m_cs_s_svrid.GetBuffer(m_cs_s_svrid.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_s_svrpid.GetLength()>0)
		{
			if (m_vDrebHeadList[id].head.head.s_Sinfo.s_cSvrPrivateId != atoi(m_cs_s_svrpid.GetBuffer(m_cs_s_svrpid.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_s_hook.GetLength()>0)
		{
			if (m_vDrebHeadList[id].head.head.s_Sinfo.s_nHook != atoi(m_cs_s_hook.GetBuffer(m_cs_s_hook.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_s_serial.GetLength()>0)
		{
			if (m_vDrebHeadList[id].head.head.s_Sinfo.s_nSerial != atoi(m_cs_s_serial.GetBuffer(m_cs_s_serial.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_s_drebserial.GetLength()>0)
		{
			if (m_vDrebHeadList[id].head.head.s_Sinfo.s_nDrebSerial != atoi(m_cs_s_drebserial.GetBuffer(m_cs_s_drebserial.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_s_index.GetLength()>0)
		{
			if (m_vDrebHeadList[id].head.head.s_Sinfo.s_nIndex != atoi(m_cs_s_index.GetBuffer(m_cs_s_index.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_d_nodeid.GetLength()>0)
		{
			if (m_vDrebHeadList[id].head.head.d_Dinfo.d_nNodeId != atoi(m_cs_d_nodeid.GetBuffer(m_cs_d_nodeid.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_d_nodepid.GetLength()>0)
		{
			if (m_vDrebHeadList[id].head.head.d_Dinfo.d_cNodePrivateId != atoi(m_cs_d_nodepid.GetBuffer(m_cs_d_nodepid.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_d_svrid.GetLength()>0)
		{
			if (m_vDrebHeadList[id].head.head.d_Dinfo.d_nSvrMainId != atoi(m_cs_d_svrid.GetBuffer(m_cs_d_svrid.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_d_svrpid.GetLength()>0)
		{
			if (m_vDrebHeadList[id].head.head.d_Dinfo.d_cSvrPrivateId != atoi(m_cs_d_svrpid.GetBuffer(m_cs_d_svrpid.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_d_serviceNo.GetLength()>0)
		{
			if (m_vDrebHeadList[id].head.head.d_Dinfo.d_nServiceNo != atoi(m_cs_d_serviceNo.GetBuffer(m_cs_d_serviceNo.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_a_nodeid.GetLength()>0)
		{
			if (m_vDrebHeadList[id].head.head.a_Ainfo.a_nNodeId != atoi(m_cs_a_nodeid.GetBuffer(m_cs_a_nodeid.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_a_nodepid.GetLength()>0)
		{
			if (m_vDrebHeadList[id].head.head.a_Ainfo.a_cNodePrivateId != atoi(m_cs_a_nodepid.GetBuffer(m_cs_a_nodepid.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_a_retCode.GetLength()>0)
		{
			if (m_vDrebHeadList[id].head.head.a_Ainfo.a_nRetCode != atoi(m_cs_a_retCode.GetBuffer(m_cs_a_retCode.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_n_nextno.GetLength()>0)
		{
			if (m_vDrebHeadList[id].head.head.n_Ninfo.n_nNextNo != atoi(m_cs_n_nextno.GetBuffer(m_cs_n_nextno.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_n_nextoffset.GetLength()>0)
		{
			if (m_vDrebHeadList[id].head.head.n_Ninfo.n_nNextOffset != atoi(m_cs_n_nextoffset.GetBuffer(m_cs_n_nextoffset.GetLength())) )
			{
				return false;
			} 
		}
		if (m_cs_len.GetLength()>0)
		{
			if (m_vDrebHeadList[id].head.head.nLen != atoi(m_cs_len.GetBuffer(m_cs_len.GetLength())) )
			{
				return false;
			} 
		}
	}
	return true;
}

void CLogViewDlg::GetQueryData()
{
	//对下拉选择框，只取空格前面的值即可
	int ret;
	int i;
	char tmpchar[200];
	ret = m_csCmd.GetLength();
	bzero(tmpchar,sizeof(tmpchar));
	strcpy(tmpchar,m_csCmd.GetBuffer(ret));
	for (i=0;i<ret;i++)
	{
		if (tmpchar[i] == ' ')
		{
			tmpchar[i]=0;
			break;
		}
	}
	m_csCmd = tmpchar;

	ret = m_csMsgFlag.GetLength();
	bzero(tmpchar,sizeof(tmpchar));
	strcpy(tmpchar,m_csMsgFlag.GetBuffer(ret));
	for (i=0;i<ret;i++)
	{
		if (tmpchar[i] == ' ')
		{
			tmpchar[i]=0;
			break;
		}
	}
	m_csMsgFlag = tmpchar;

	ret = m_csMsgType.GetLength();
	bzero(tmpchar,sizeof(tmpchar));
	strcpy(tmpchar,m_csMsgType.GetBuffer(ret));
	for (i=0;i<ret;i++)
	{
		if (tmpchar[i] == ' ')
		{
			tmpchar[i]=0;
			break;
		}
	}
	m_csMsgType = tmpchar;

}

void CLogViewDlg::OnButtonPrev() 
{
	// TODO: Add your control notification handler code here
	int i;
	CInt  list;
	int num=0;
	if (m_bIsDrebLog)
	{
		for ( i=m_nCurDisplayIndex-1;i>=0;i--)
		{
			if (num>500)
			{
				m_nCurDisplayIndex = i;
				m_btn_prev.EnableWindow(true);
				break;
			}
			list.Add(i);
			num++;
		}
	}
	else
	{
		for ( i=m_nCurDisplayIndex-1;i>=0;i--)
		{
			if (num>500)
			{
				m_nCurDisplayIndex = i;
				m_btn_prev.EnableWindow(true);
				break;
			}
			list.Add(i);
			num++;
		}
	}
	if (num <500)
	{
		m_btn_prev.EnableWindow(false);
	}
	if (m_nCurDisplayIndex == num)
	{
		m_btn_prev.EnableWindow(false);
	}
	if (num>1)
	{
		m_btn_next.EnableWindow(true);
	}
	DisplayData(list);
}

void CLogViewDlg::OnButtonNext() 
{
	// TODO: Add your control notification handler code here
	int i;
	CInt  list;
	int num=0;
	if (m_bIsDrebLog)
	{
		for ( i=m_nCurDisplayIndex;i<m_vDrebHeadList.size();i++)
		{
			if (num>500)
			{
				m_nCurDisplayIndex = i;
				m_btn_next.EnableWindow(true);
				break;
			}
			list.Add(i);
			num++;
		}
	}
	else
	{
		for ( i=m_nCurDisplayIndex;i<m_vBpcHeadList.size();i++)
		{
			if (num>500)
			{
				m_nCurDisplayIndex = i;
				m_btn_next.EnableWindow(true);
				break;
			}
			list.Add(i);
			num++;
		}
	}
	if (num <500)
	{
		m_btn_next.EnableWindow(false);
	}
	if (num>1)
	{
		m_btn_prev.EnableWindow(true);
	}
	DisplayData(list);
	
	
}

void CLogViewDlg::OnDblclkLISTHead(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	int ret;
	int nItem = pNMListView->iItem;
	char szTmp[121];
	char szTmp2[300];
	memset(szTmp,0,sizeof(szTmp));
	memset(szTmp2,0,sizeof(szTmp2));
	
	if (nItem <0)
	{
		return;
	}
	
	m_lstLogHead.GetItemText(nItem, 0, szTmp, 40);
	int  headId = atoi(szTmp);
	if (headId<0)
	{
		return;
	}
	CDrebLogData dlg;
	FILE *fp=NULL;
	if (m_bIsDrebLog)
	{
		fp = fopen(m_sLogFilePath.c_str(),"rb");
		if (fp == NULL)
		{
			return;
		}
		memcpy(&(dlg.m_head),&(m_vDrebHeadList[headId].head.head),DREBHEADLEN);
		ret = fseek(fp,m_vDrebHeadList[headId].ndatabegin,1);
		dlg.m_nDataLen = fread(dlg.m_sData,1,m_vDrebHeadList[headId].ndataend-m_vDrebHeadList[headId].ndatabegin,fp);
		fclose(fp);

	}
	else
	{
		fp = fopen(m_sLogFilePath.c_str(),"rb");
		if (fp == NULL)
		{
			return;
		}
		memcpy(&(dlg.m_head),&(m_vBpcHeadList[headId].head.sDBHead),DREBHEADLEN);
		ret = fseek(fp,m_vBpcHeadList[headId].ndatabegin,1);
		dlg.m_nDataLen = fread(dlg.m_sData,1,m_vBpcHeadList[headId].ndataend-m_vBpcHeadList[headId].ndatabegin,fp);
		fclose(fp);
	}
	dlg.DoModal();
	
}
