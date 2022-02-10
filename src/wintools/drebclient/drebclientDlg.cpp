// drebclientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vcdrebclient.h"
#include "drebclientDlg.h"
#include "BF_SimpleLog.h"
#include "BF_Date_Time.h"
#include "BF_Tools.h"
#include "DownUpFile.h"
#include "BpcHead.h"

#include "Xdp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static UINT indicators[] =
{
	//设置小格格
	ID_SEPARATOR,           // status line indicator
	ID_SEPARATOR,  
	ID_SEPARATOR,   
	ID_SEPARATOR,   
	ID_SEPARATOR,   
	ID_SEPARATOR,  
	ID_SEPARATOR,
};

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
// CDrebclientDlg dialog

CDrebclientDlg::CDrebclientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDrebclientDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDrebclientDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	AfxInitRichEdit();
	memset(m_sSendData,0,sizeof(m_sSendData));
	memset(m_sRcvData,0,sizeof(m_sRcvData));
	m_nHook = 0;
	m_nSerial = 0;

	m_nSendNum=0;
	m_nRcvNum=0;
	strcpy(m_sDataFileName,"drebclient.dat");

	m_cZip = 0;
}

void CDrebclientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDrebclientDlg)
	DDX_Control(pDX, IDC_EDIT_SERIAL, m_edtSerial);
	DDX_Control(pDX, IDC_EDIT_INDEX, m_edtIndex);
	DDX_Control(pDX, IDC_EDIT_HOOK, m_edtHook);
	DDX_Control(pDX, IDC_COMBO_CZIP, m_combZip);
	DDX_Control(pDX, IDC_BUTTON_RCVNEXT, m_btnGetNext);
	DDX_Control(pDX, IDC_TREE_RESULTLIST, m_treResultList);
	DDX_Control(pDX, IDC_IPADDRESS_DREBIP, m_ipAddress);
	DDX_Control(pDX, IDC_EDIT_TIMEOUT, m_edtTimeOut);
	DDX_Control(pDX, IDC_EDIT_SVRPRIVATEID, m_edtSvrPrivateId);
	DDX_Control(pDX, IDC_EDIT_SVRMAINID, m_edtSvrMainId);
	DDX_Control(pDX, IDC_EDIT_SERVICE_NO, m_edtServiceNo);
	DDX_Control(pDX, IDC_EDIT_PRIVATEID, m_edtNodePrivateId);
	DDX_Control(pDX, IDC_EDIT_NODEID, m_edtNodeId);
	DDX_Control(pDX, IDC_EDIT_DREBPORT, m_edtDrebPort);
	DDX_Control(pDX, IDC_EDIT_DATAFILE, m_edtDataFile);
	DDX_Control(pDX, IDC_COMBO_MESSAGE, m_combMessage);
	DDX_Control(pDX, IDC_CHECK_DATA, m_chkData);
	DDX_Control(pDX, IDC_BUTTON_SEND, m_btnSend);
	DDX_Control(pDX, IDC_BUTTON_FILE, m_btnFile);
	DDX_Control(pDX, IDC_BUTTON_DISCONNECT, m_btnDisConnect);
	DDX_Control(pDX, IDC_RICHEDIT_DATA, m_rEditData);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDrebclientDlg, CDialog)
	//{{AFX_MSG_MAP(CDrebclientDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_DISCONNECT, OnButtonDisconnect)
	ON_BN_CLICKED(IDC_BUTTON_FILE, OnButtonFile)
	ON_BN_CLICKED(IDC_BUTTON_SEND, OnButtonSend)
	ON_BN_CLICKED(IDC_CHECK_DATA, OnCheckData)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, OnButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_RCVNEXT, OnButtonRcvnext)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_RESULTLIST, OnDblclkTreeResultlist)
	ON_BN_CLICKED(IDC_VIEW, OnView)
	ON_BN_CLICKED(IDC_BUTTON_XDP, OnButtonXdp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDrebclientDlg message handlers

BOOL CDrebclientDlg::OnInitDialog()
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
	
	m_bFromXdp = false;
	// TODO: Add extra initialization here
	m_combMessage.ResetContent();
	m_combMessage.FormatComboBox();
	m_combMessage.SetColumnWidth(60, 100);
	m_combMessage.SetColumnAlignStyle(DT_LEFT, DT_LEFT);

	m_combMessage.AddRow("8","CMD_DPCALL");
	m_combMessage.AddRow("9","CMD_DPACALL");
	m_combMessage.AddRow("10","CMD_DPBC");
	m_combMessage.AddRow("11","CMD_DPABC");
	m_combMessage.AddRow("12","CMD_DPPUSH");
	m_combMessage.AddRow("13","CMD_DPPOST");
	m_combMessage.AddRow("15","CMD_MONITOR_DREB");
	m_combMessage.AddRow("16","CMD_MONITOR_BPC");
	m_combMessage.AddRow("17","CMD_DPBC_UNREGTX");
	m_combMessage.SetCurSel(0);
	m_edtDataFile.EnableWindow(false);
	m_btnFile.EnableWindow(false);

	m_edtDrebPort.SetLimitText(6);
	m_edtTimeOut.SetLimitText(6);
	m_edtNodeId.SetLimitText(5);
	m_edtNodePrivateId.SetLimitText(3);
//	m_edtSvrMainId.SetLimitText(6);
	m_edtSvrPrivateId.SetLimitText(3);

	m_edtHook.SetLimitText(10);
	m_edtSerial.SetLimitText(10);
	m_edtIndex.SetLimitText(10);
	m_edtHook.SetWindowText("0");
	m_edtSerial.SetWindowText("0");
	m_edtIndex.SetWindowText("0");

	if (!m_statusbar.Create(this) ||
		!m_statusbar.SetIndicators(indicators,
		sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_statusbar.SetPaneInfo(0,0,SBPS_NORMAL,80);
	m_statusbar.SetPaneText(0,"  DREB连接-->");
	m_statusbar.SetPaneInfo(1,1,SBPS_STRETCH,260);
	m_statusbar.SetPaneText(1,"未连接");
	m_statusbar.SetPaneInfo(2,2,SBPS_NORMAL,70);
	m_statusbar.SetPaneText(2,"发送次数-->");
	m_statusbar.SetPaneInfo(3,3,SBPS_NORMAL,60);
	m_statusbar.SetPaneText(3,"0");
	m_statusbar.SetPaneInfo(4,4,SBPS_NORMAL,80);
	m_statusbar.SetPaneText(4,"接收应答数-->");
	m_statusbar.SetPaneInfo(5,5,SBPS_NORMAL,60);
	m_statusbar.SetPaneText(5,"0");
	m_statusbar.SetPaneInfo(6,6,SBPS_NORMAL,120);

	

	//让这个状态栏最终显示在对话框中
    RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,0);
	
	m_btnGetNext.EnableWindow(false);
	GetParaFromFile("drebclient.xml");
	
	m_combZip.InsertString(0,"0    明文");
	m_combZip.InsertString(1,"1    3DES加密");
	m_combZip.InsertString(2,"2    压缩");
	m_combZip.InsertString(3,"3    DES加密");
	m_combZip.InsertString(4,"4    DES加密并压缩");
	m_combZip.InsertString(5,"5    自定义1");
	m_combZip.InsertString(6,"6    自定义2");
	m_combZip.InsertString(7,"7    自定义3");
	m_combZip.InsertString(8,"8    自定义4");
	m_combZip.InsertString(9,"9    自定义5");
	m_combZip.SetCurSel(0);

	m_timerID1 = SetTimer(ONTIMER1,1,NULL);//定时器
	bzero(&m_head,DREBHEADLEN);


	m_pLog.SetLogPara(LOG_DEBUG,"","drebclient.log");
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDrebclientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CDrebclientDlg::OnPaint() 
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
HCURSOR CDrebclientDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CDrebclientDlg::OnButtonDisconnect() 
{
	// TODO: Add your control notification handler code here
	m_pClient.Close();
	sprintf(m_errMsg,"未连接");
	m_statusbar.SetPaneText(1,m_errMsg);
	m_nSendNum=0;
	m_nRcvNum=0;
	sprintf(m_errMsg,"%d",m_nSendNum);
	m_statusbar.SetPaneText(3,m_errMsg);
	sprintf(m_errMsg,"%d",m_nRcvNum);
	m_statusbar.SetPaneText(5,m_errMsg);
}

void CDrebclientDlg::OnButtonFile() 
{
	// TODO: Add your control notification handler code here
	char filename[400];
	memset(filename,0,sizeof(filename));
	WORKPATH(filename);
	CString ss;
	int ret;
	
	m_bFromXdp = false;
	CFileDialog   dlg(true,"","*.*",OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"ALL Files (*.*)|",NULL);
	ret = dlg.DoModal();
	ss = dlg.GetPathName();
	
	if (ret !=IDOK)
	{
		return;
	}
	
	m_edtDataFile.SetWindowText(ss);
	struct_stat statfile;
	FSTAT(ss.GetBuffer(ss.GetLength()),&statfile);
	if (statfile.st_size <1 || statfile.st_size >DREBDATASIZE)
	{
		MessageBox("数据文件不存在或数据太长","错误");
		m_btnFile.SetFocus();
		return ;
	}
	FILE *fp = fopen(ss.GetBuffer(ss.GetLength()),"rb");
	if (fp == NULL)
	{
		MessageBox("打开数据文件失败","错误");
		m_btnFile.SetFocus();
		return ;
	}
	memset(m_sSendData,0,sizeof(m_sSendData));
	m_nDataLen = fread(m_sSendData,1,statfile.st_size,fp);
	fclose(fp);
	m_rEditData.SetWindowText(m_sSendData);
	m_rEditData.EnableWindow(true);
	//将工作路径设置回去
	SetCurrentDirectory(filename);
	UpdateData(false);
	m_btnSend.SetFocus();
}

void CDrebclientDlg::OnButtonSend() 
{
	// TODO: Add your control notification handler code here
	CString ss;
	m_edtHook.GetWindowText(ss);
	m_nHook = atoi(ss.GetBuffer(ss.GetLength()));

	m_edtSerial.GetWindowText(ss);
	m_nSerial = atoi(ss.GetBuffer(ss.GetLength()));

	m_edtIndex.GetWindowText(ss);
	m_ngateindex = atoi(ss.GetBuffer(ss.GetLength()));

	SetParaToFile("drebclient.xml");
	m_pClient.Init(m_sDrebIp.c_str(),m_nDrebPort,m_nTimeOut);
	int cmdtype = atoi(m_combMessage.GetCurColumnItem(0));
	
	m_edtServiceNo.GetWindowText(ss);
	int txcode = atoi(ss.GetBuffer(ss.GetLength()));
	//下传下载文件
	if (cmdtype == CMD_MONITOR_BPC && ( txcode == BPCMONI_DOWNLOAD || txcode == BPCMONI_UPLOAD))
	{
		DownUpLoadFile();
		return;
	}

	
	if (GetSendData()<0)
	{
		return;
	}
	

	if (!m_pClient.IsConnect())
	{
		if (!m_pClient.Connect())
		{
			MessageBox(m_pClient.m_errMsg,"提示");
			sprintf(m_errMsg,"连接DREB[%s %d]失败 %s",m_sDrebIp.c_str(),m_nDrebPort,m_pClient.m_errMsg);
			m_statusbar.SetPaneText(1,m_errMsg);
			return;
		}
		sprintf(m_errMsg,"连接DREB[%s %d]成功",m_sDrebIp.c_str(),m_nDrebPort);
		m_statusbar.SetPaneText(1,m_errMsg);
	}
	m_pClient.GetDrebHead(m_head);
	m_head.cRaflag = 0;
	m_head.cZip = m_combZip.GetCurSel();
	
	m_head.cNextFlag = 0;
	m_head.cDrebAffirm = 0;
	m_head.cCmd = cmdtype;
	m_head.nLen = m_nDataLen;
	memcpy(&m_head.d_Dinfo,&m_pDrebDest,sizeof(DREB_D_NODEINFO));
	m_head.s_Sinfo.s_nHook = m_nHook;
	m_head.s_Sinfo.s_nSerial = m_nSerial;

	
	sprintf(m_sDataMsg,"请求 %s %d hook=%d serial=%d",m_combMessage.GetCurColumnItem(1),CBF_Date_Time::GetTickCount(),m_nHook,m_nSerial);
	m_hRoot = m_treResultList.InsertItem(m_sDataMsg);
	m_treResultList.SelectItem(m_hRoot);
	m_treResultList.Expand(m_hRoot,TVE_EXPAND);
	ShowPack(m_hRoot,m_head,m_sSendData);

	int curtime = CBF_Date_Time::GetTickCount();
	int nRet=0;
	switch (cmdtype)
	{
		case CMD_DPCALL:
			nRet = DpCall();
			break;
		case CMD_DPACALL:
			nRet = DpACall();
			break;
		case CMD_DPPUSH:
			nRet = DpPush();
			break;
		case CMD_DPPOST:
			nRet = DpPost();
			break;
		case CMD_DPBC:
			nRet = DpBc();
			break;
		case CMD_DPABC:
			nRet = DpAbc();
			break;
		case CMD_MONITOR_DREB:
			nRet = Monitor();
			break;
		case CMD_MONITOR_BPC:
			nRet = MonitorBpc();
			break;
		case CMD_DPBC_UNREGTX:
			nRet = DpBcUnRegTx();
			break;
		default:
			break;
	}
	if (nRet == 0)
	{
		sprintf(m_sDataMsg,"应答 %s %d hook=%d serial=%d  时间=%d毫秒",m_combMessage.GetCurColumnItem(1),CBF_Date_Time::GetTickCount(),m_nHook,m_nSerial,CBF_Date_Time::GetTickCount()-curtime);
		m_hRoot = m_treResultList.InsertItem(m_sDataMsg);
		m_treResultList.SelectItem(m_hRoot);
		m_treResultList.Expand(m_hRoot,TVE_EXPAND);
		ShowPack(m_hRoot,m_head,m_sRcvData,true);
	}
}

void CDrebclientDlg::OnCheckData() 
{
	// TODO: Add your control notification handler code here
	if (m_chkData.GetCheck() == 1)
	{
		m_rEditData.EnableWindow(false);
		m_edtDataFile.EnableWindow(true);
		m_btnFile.EnableWindow(true);
	}
	else
	{
		m_rEditData.EnableWindow(true);
		m_edtDataFile.EnableWindow(false);
		m_btnFile.EnableWindow(false);
	}
}

void CDrebclientDlg::OnOK() 
{
	// TODO: Add extra validation here

	SetParaToFile("drebclient.xml");
	m_pClient.Init(m_sDrebIp.c_str(),m_nDrebPort,m_nTimeOut);
	if (m_pClient.Connect())
	{
		sprintf(m_errMsg,"连接DREB[%s %d]成功",m_sDrebIp.c_str(),m_nDrebPort);
	}
	else
	{
		sprintf(m_errMsg,"连接DREB[%s %d]失败 %s",m_sDrebIp.c_str(),m_nDrebPort,m_pClient.m_errMsg);
	}
	m_statusbar.SetPaneText(1,m_errMsg);
//	CDialog::OnOK();
}

bool CDrebclientDlg::GetParaFromFile(const char *filename)
{
	std::string value;
	int msgtypeid;
	if (m_pXml.FromFile(filename))
	{
		if (m_pXml.GetNodeValueByPath("/package/head/DREB地址",false,value) != NULL)
		{
			m_ipAddress.SetWindowText(value.c_str());
		}
		if (m_pXml.GetNodeValueByPath("/package/head/DREB端口",false,value) != NULL)
		{
			m_edtDrebPort.SetWindowText(value.c_str());
		}
		if (m_pXml.GetNodeValueByPath("/package/head/DREB接收超时",false,value) != NULL)
		{
			m_edtTimeOut.SetWindowText(value.c_str());
		}
		if (m_pXml.GetNodeValueByPath("/package/head/NODEID",false,value) != NULL)
		{
			m_edtNodeId.SetWindowText(value.c_str());
		}
		if (m_pXml.GetNodeValueByPath("/package/head/NODEPRIVATEID",false,value) != NULL)
		{
			m_edtNodePrivateId.SetWindowText(value.c_str());
		}
		if (m_pXml.GetNodeValueByPath("/package/head/SVRMAINID",false,value) != NULL)
		{
			m_edtSvrMainId.SetWindowText(value.c_str());
		}
		if (m_pXml.GetNodeValueByPath("/package/head/SVRPRIVATEID",false,value) != NULL)
		{
			m_edtSvrPrivateId.SetWindowText(value.c_str());
		}
		if (m_pXml.GetNodeValueByPath("/package/head/SERVICENO",false,value) != NULL)
		{
			m_edtServiceNo.SetWindowText(value.c_str());
		}
		if (m_pXml.GetNodeValueByPath("/package/head/MSGTYPEID",false,msgtypeid) != NULL)
		{
			m_combMessage.SetCurSel(msgtypeid);
		}
	}
	return true;
}

bool CDrebclientDlg::SetParaToFile(const char *filename)
{
	CString value;
	m_ipAddress.GetWindowText(value);
	m_sDrebIp = value.GetBuffer(value.GetLength());
	m_pXml.SetNodeValueByPath("/package/head/DREB地址",false,value.GetBuffer(value.GetLength()));
	
	m_edtDrebPort.GetWindowText(value);
	m_nDrebPort = atoi(value.GetBuffer(value.GetLength()));
	m_pXml.SetNodeValueByPath("/package/head/DREB端口",false,value.GetBuffer(value.GetLength()));
	
	m_edtTimeOut.GetWindowText(value);
	m_nTimeOut = atoi(value.GetBuffer(value.GetLength()));
	m_pXml.SetNodeValueByPath("/package/head/DREB接收超时",false,value.GetBuffer(value.GetLength()));

	m_edtNodeId.GetWindowText(value);
	m_pDrebDest.d_nNodeId = atoi(value.GetBuffer(value.GetLength()));
	m_pXml.SetNodeValueByPath("/package/head/NODEID",false,value.GetBuffer(value.GetLength()));

	m_edtNodePrivateId.GetWindowText(value);
	m_pDrebDest.d_cNodePrivateId = atoi(value.GetBuffer(value.GetLength()));
	m_pXml.SetNodeValueByPath("/package/head/NODEPRIVATEID",false,value.GetBuffer(value.GetLength()));

	m_edtSvrMainId.GetWindowText(value);
	m_pDrebDest.d_nSvrMainId = atoi(value.GetBuffer(value.GetLength()));
	m_pXml.SetNodeValueByPath("/package/head/SVRMAINID",false,value.GetBuffer(value.GetLength()));

	m_edtSvrPrivateId.GetWindowText(value);
	m_pDrebDest.d_cSvrPrivateId = atoi(value.GetBuffer(value.GetLength()));
	m_pXml.SetNodeValueByPath("/package/head/SVRPRIVATEID",false,value.GetBuffer(value.GetLength()));

	m_edtServiceNo.GetWindowText(value);
	m_pDrebDest.d_nServiceNo = atoi(value.GetBuffer(value.GetLength()));
	m_pXml.SetNodeValueByPath("/package/head/SERVICENO",false,value.GetBuffer(value.GetLength()));

	int msgtypeid =	m_combMessage.GetCurSel();
	m_pXml.SetNodeValueByPath("/package/head/MSGTYPEID",false,msgtypeid);

	char tmpchar[200];
	memset(tmpchar,0,sizeof(tmpchar));
	m_combZip.GetWindowText(tmpchar,2);
	m_cZip = atoi(tmpchar);

	return m_pXml.ToFile(filename);
	
}
 
void CDrebclientDlg::OnTimer(UINT_PTR nIDEvent)
//void CDrebclientDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default

	switch (nIDEvent)
	{
		case ONTIMER1:
			m_publog.Update();
			m_statusbar.SetPaneText(6,m_publog.ToStringDT17().c_str());
			UpdateData(true);
			break;
		default:
			break;
	}
	CDialog::OnTimer(nIDEvent);
}

int CDrebclientDlg::GetSendData()
{
	CString value ;
	if (m_chkData.GetCheck() == 0)
	{
		if (m_bFromXdp)
		{
			if (m_xdpdlg.m_nXdpDataLen >0)
			{
				memcpy(m_sSendData,m_xdpdlg.m_xdpData,m_xdpdlg.m_nXdpDataLen );
				m_nDataLen = m_xdpdlg.m_nXdpDataLen;
				m_sSendData[m_nDataLen]=0;
			}
			return m_nDataLen;
		}
		if (m_rEditData.GetWindowTextLength() >= DREBDATASIZE)
		{
			MessageBox("输入数据太长","错误");
			m_rEditData.SetFocus();
			return -1;
		}
		
		m_rEditData.GetWindowText(value);
		m_nDataLen = value.GetLength();
		memset(m_sSendData,0,sizeof(m_sSendData));
		memcpy(m_sSendData,value.GetBuffer(value.GetLength()),m_nDataLen);
		return m_nDataLen;
	}
	else
	{
		
		m_edtDataFile.GetWindowText(value);
		if (value.GetLength()<1)
		{	
			MessageBox("请选择数据文件","错误");
			m_edtDataFile.SetFocus();
			return -1;
		}
		struct_stat statfile;
		FSTAT(value.GetBuffer(value.GetLength()),&statfile);
		if (statfile.st_size <1 || statfile.st_size >DREBDATASIZE)
		{
			MessageBox("数据文件不存在或数据太长","错误");
			m_edtDataFile.SetFocus();
			return -1;
		}
		FILE *fp = fopen(value.GetBuffer(value.GetLength()),"rb");
		if (fp == NULL)
		{
			MessageBox("打开数据文件失败","错误");
			m_edtDataFile.SetFocus();
			return -1;
		}
		memset(m_sSendData,0,sizeof(m_sSendData));
		m_nDataLen = fread(m_sSendData,1,statfile.st_size,fp);
		fclose(fp);
		return m_nDataLen;
	}
}

int CDrebclientDlg::DpCall()
{
	int ret;
	m_btnGetNext.EnableWindow(false);
	memset(m_sRcvData,0,sizeof(m_sRcvData));
	ret = m_pClient.DPCall(m_nHook,m_nSerial,m_pDrebDest,m_sSendData,m_nDataLen,m_cZip,m_head,m_sRcvData);
	if (ret == 0)
	{
		m_nSendNum++;
		sprintf(m_errMsg,"%d",m_nSendNum);
		m_statusbar.SetPaneText(3,m_errMsg);
		sprintf(m_errMsg,"%d",m_nRcvNum);
		m_statusbar.SetPaneText(5,m_errMsg);
		MessageBox("接收超时","提示");
		m_btnGetNext.EnableWindow(true);
		return -1;
	}
	else if (ret <0)
	{
		MessageBox(m_pClient.m_errMsg,"提示");
		return -1;
	}
	else
	{
		m_nSendNum++;
		m_nRcvNum++;
		sprintf(m_errMsg,"%d",m_nSendNum);
		m_statusbar.SetPaneText(3,m_errMsg);
		sprintf(m_errMsg,"%d",m_nRcvNum);
		m_statusbar.SetPaneText(5,m_errMsg);
		if (m_head.cNextFlag == 1)
		{
			m_btnGetNext.EnableWindow(true);
		}
		else if (m_head.cNextFlag == 10)
		{
			m_btnGetNext.EnableWindow(false);
		}
		return 0;
	}
	
}

int CDrebclientDlg::DpACall()
{
	int ret;
	m_btnGetNext.EnableWindow(false);
	ret = m_pClient.DPACall(m_nHook,m_nSerial,m_pDrebDest,m_sSendData,m_nDataLen,m_cZip);
	if (ret == 0)
	{
		m_nSendNum++;
		sprintf(m_errMsg,"%d",m_nSendNum);
		m_statusbar.SetPaneText(3,m_errMsg);
		sprintf(m_errMsg,"%d",m_nRcvNum);
		m_statusbar.SetPaneText(5,m_errMsg);
		MessageBox("接收超时","提示");
		m_btnGetNext.EnableWindow(true);
		return -1;
	}
	else if (ret <0)
	{
		MessageBox(m_pClient.m_errMsg,"提示");
		return -1;
	}
	else if (ret == 100)
	{
		m_nSendNum++;
		sprintf(m_errMsg,"%d",m_nSendNum);
		m_statusbar.SetPaneText(3,m_errMsg);
// 		sprintf(m_errMsg,"%d",m_nRcvNum);
// 		m_statusbar.SetPaneText(5,m_errMsg);
		m_btnGetNext.EnableWindow(true);
		sprintf(m_sDataMsg,"%s DREB 确认收到",m_combMessage.GetCurColumnItem(1));
		m_hRoot = m_treResultList.InsertItem(m_sDataMsg);
		m_treResultList.SelectItem(m_hRoot);
		m_treResultList.Expand(m_hRoot,TVE_EXPAND);
		return -1;
	}
	else
	{
		sprintf(m_errMsg,"DREB返回%d",ret);
		MessageBox(m_errMsg,"提示");
		return -1;
	}
	return 0;
}

int CDrebclientDlg::DpBc()
{
	int ret;
	m_btnGetNext.EnableWindow(false);
	ret = m_pClient.DPBC(m_nHook,m_nSerial,m_pDrebDest,m_sSendData,m_nDataLen,m_cZip);
	if (ret == 0)
	{
		m_nSendNum++;
		sprintf(m_errMsg,"%d",m_nSendNum);
		m_statusbar.SetPaneText(3,m_errMsg);
		sprintf(m_errMsg,"%d",m_nRcvNum);
		m_statusbar.SetPaneText(5,m_errMsg);
		MessageBox("接收超时","提示");
		m_btnGetNext.EnableWindow(true);
		return -1;
	}
	else if (ret <0)
	{
		MessageBox(m_pClient.m_errMsg,"提示");
		return -1;
	}
	else if (ret == 100)
	{
		m_nSendNum++;
		sprintf(m_errMsg,"%d",m_nSendNum);
		m_statusbar.SetPaneText(3,m_errMsg);
		// 		sprintf(m_errMsg,"%d",m_nRcvNum);
		// 		m_statusbar.SetPaneText(5,m_errMsg);
		m_btnGetNext.EnableWindow(true);
		sprintf(m_sDataMsg,"%s DREB 确认收到",m_combMessage.GetCurColumnItem(1));
		m_hRoot = m_treResultList.InsertItem(m_sDataMsg);
		m_treResultList.SelectItem(m_hRoot);
		m_treResultList.Expand(m_hRoot,TVE_EXPAND);
		return -1;
	}
	else
	{
		sprintf(m_errMsg,"DREB返回%d",ret);
		MessageBox(m_errMsg,"提示");
		return -1;
	}
	return 0;
}

int CDrebclientDlg::DpAbc()
{
	int ret;
	m_btnGetNext.EnableWindow(false);
	ret = m_pClient.DPABC(m_nHook,m_nSerial,m_ngateindex,m_pDrebDest,m_sSendData,m_nDataLen,m_cZip);
	if (ret == m_nDataLen)
	{
		m_nSendNum++;
		sprintf(m_errMsg,"%d",m_nSendNum);
		m_statusbar.SetPaneText(3,m_errMsg);
		// 		sprintf(m_errMsg,"%d",m_nRcvNum);
		// 		m_statusbar.SetPaneText(5,m_errMsg);
		m_btnGetNext.EnableWindow(true);
		sprintf(m_sDataMsg,"%s 发送成功",m_combMessage.GetCurColumnItem(1));
		m_hRoot = m_treResultList.InsertItem(m_sDataMsg);
		m_treResultList.SelectItem(m_hRoot);
		m_treResultList.Expand(m_hRoot,TVE_EXPAND);
		return -1;
	}
	else 
	{
		MessageBox(m_pClient.m_errMsg,"提示");
		return -1;
	}

}

int CDrebclientDlg::DpPush()
{
	int ret;
	m_btnGetNext.EnableWindow(false);
	ret = m_pClient.DPPush(m_nHook,m_nSerial,m_pDrebDest,m_sSendData,m_nDataLen,m_cZip);
	if (ret == m_nDataLen)
	{
		m_nSendNum++;
		sprintf(m_errMsg,"%d",m_nSendNum);
		m_statusbar.SetPaneText(3,m_errMsg);
		// 		sprintf(m_errMsg,"%d",m_nRcvNum);
		// 		m_statusbar.SetPaneText(5,m_errMsg);
		m_btnGetNext.EnableWindow(true);
		sprintf(m_sDataMsg,"%s 发送成功",m_combMessage.GetCurColumnItem(1));
		m_hRoot = m_treResultList.InsertItem(m_sDataMsg);
		m_treResultList.SelectItem(m_hRoot);
		m_treResultList.Expand(m_hRoot,TVE_EXPAND);
		return -1;
	}
	else 
	{
		MessageBox(m_pClient.m_errMsg,"提示");
		return -1;
	}
}

int CDrebclientDlg::DpPost()
{
	int ret;
	m_btnGetNext.EnableWindow(false);
	ret = m_pClient.DPPost(m_nHook,m_nSerial,m_pDrebDest,m_sSendData,m_nDataLen,m_cZip);
	if (ret == 0)
	{
		m_nSendNum++;
		sprintf(m_errMsg,"%d",m_nSendNum);
		m_statusbar.SetPaneText(3,m_errMsg);
		sprintf(m_errMsg,"%d",m_nRcvNum);
		m_statusbar.SetPaneText(5,m_errMsg);
		MessageBox("接收超时","提示");
		m_btnGetNext.EnableWindow(true);
		return -1;
	}
	else if (ret <0)
	{
		MessageBox(m_pClient.m_errMsg,"提示");
		return -1;
	}
	else if (ret == 100)
	{
		m_nSendNum++;
		sprintf(m_errMsg,"%d",m_nSendNum);
		m_statusbar.SetPaneText(3,m_errMsg);
		// 		sprintf(m_errMsg,"%d",m_nRcvNum);
		// 		m_statusbar.SetPaneText(5,m_errMsg);
		m_btnGetNext.EnableWindow(true);
		sprintf(m_sDataMsg,"%s DREB 确认收到",m_combMessage.GetCurColumnItem(1));
		m_hRoot = m_treResultList.InsertItem(m_sDataMsg);
		m_treResultList.SelectItem(m_hRoot);
		m_treResultList.Expand(m_hRoot,TVE_EXPAND);
		return -1;
	}
	else
	{
		sprintf(m_errMsg,"DREB返回%d",ret);
		MessageBox(m_errMsg,"提示");
		return -1;
	}
	return 0;
}

int CDrebclientDlg::Monitor()
{
	int ret;
	m_btnGetNext.EnableWindow(false);
	memset(m_sRcvData,0,sizeof(m_sRcvData));
	ret = m_pClient.Monitor_Dreb(m_nHook,m_nSerial,m_pDrebDest,m_sSendData,m_nDataLen,m_cZip,m_head,m_sRcvData);
	if (ret == 0)
	{
		m_nSendNum++;
		sprintf(m_errMsg,"%d",m_nSendNum);
		m_statusbar.SetPaneText(3,m_errMsg);
		sprintf(m_errMsg,"%d",m_nRcvNum);
		m_statusbar.SetPaneText(5,m_errMsg);
		MessageBox("接收超时","提示");
		m_btnGetNext.EnableWindow(true);
		return -1;
	}
	else if (ret <0)
	{
		MessageBox(m_pClient.m_errMsg,"提示");
		return -1;
	}
	else
	{
		m_nSendNum++;
		m_nRcvNum++;
		sprintf(m_errMsg,"%d",m_nSendNum);
		m_statusbar.SetPaneText(3,m_errMsg);
		sprintf(m_errMsg,"%d",m_nRcvNum);
		m_statusbar.SetPaneText(5,m_errMsg);
		return 0;
	}
	return 0;
}
int CDrebclientDlg::MonitorBpc()
{
	int ret;
	m_btnGetNext.EnableWindow(false);
	memset(m_sRcvData,0,sizeof(m_sRcvData));
	ret = m_pClient.Monitor_Bpc(m_nHook,m_nSerial,m_pDrebDest,m_sSendData,m_nDataLen,m_cZip,m_head,m_sRcvData);
	if (ret == 0)
	{
		m_nSendNum++;
		sprintf(m_errMsg,"%d",m_nSendNum);
		m_statusbar.SetPaneText(3,m_errMsg);
		sprintf(m_errMsg,"%d",m_nRcvNum);
		m_statusbar.SetPaneText(5,m_errMsg);
		MessageBox("接收超时","提示");
		m_btnGetNext.EnableWindow(true);
		return -1;
	}
	else if (ret <0)
	{
		MessageBox(m_pClient.m_errMsg,"提示");
		return -1;
	}
	else
	{
		m_nSendNum++;
		m_nRcvNum++;
		sprintf(m_errMsg,"%d",m_nSendNum);
		m_statusbar.SetPaneText(3,m_errMsg);
		sprintf(m_errMsg,"%d",m_nRcvNum);
		m_statusbar.SetPaneText(5,m_errMsg);
		return 0;
	}
	return 0;
}
void CDrebclientDlg::ShowPack(HTREEITEM hParent, DREB_HEAD head,const char *data,bool isans)
{
	FILE *fp;
	HTREEITEM hItem,hItemData,threeItem;
	DisplayButton(hParent);
	sprintf(m_sDataMsg,"cZip=%d",head.cZip);
	hItem = m_treResultList.InsertItem(m_sDataMsg,hParent);
	sprintf(m_sDataMsg,"cCmd=%d",head.cCmd);
	hItem = m_treResultList.InsertItem(m_sDataMsg,hParent);
	sprintf(m_sDataMsg,"cRaflag=%d",head.cRaflag);
	hItem = m_treResultList.InsertItem(m_sDataMsg,hParent);
	sprintf(m_sDataMsg,"cNextFlag=%d",head.cNextFlag);
	hItem = m_treResultList.InsertItem(m_sDataMsg,hParent);
	sprintf(m_sDataMsg,"cDrebAffirm=%d",head.cDrebAffirm);
	hItem = m_treResultList.InsertItem(m_sDataMsg,hParent);

	sprintf(m_sDataMsg,"DREB_S_NODEINFO");
	hItem = m_treResultList.InsertItem(m_sDataMsg,hParent);
	DisplayButton(hItem);
	sprintf(m_sDataMsg,"s_nNodeId=%d",head.s_Sinfo.s_nNodeId);
	hItemData = m_treResultList.InsertItem(m_sDataMsg,hItem);
	sprintf(m_sDataMsg,"s_cNodePrivateId=%d",head.s_Sinfo.s_cNodePrivateId);
	hItemData = m_treResultList.InsertItem(m_sDataMsg,hItem);
	sprintf(m_sDataMsg,"s_nSvrMainId=%d",head.s_Sinfo.s_nSvrMainId);
	hItemData = m_treResultList.InsertItem(m_sDataMsg,hItem);
	sprintf(m_sDataMsg,"s_cSvrPrivateId=%d",head.s_Sinfo.s_cSvrPrivateId);
	hItemData = m_treResultList.InsertItem(m_sDataMsg,hItem);
	sprintf(m_sDataMsg,"s_nHook=%d",head.s_Sinfo.s_nHook);
	hItemData = m_treResultList.InsertItem(m_sDataMsg,hItem);
	sprintf(m_sDataMsg,"s_nSerial=%d",head.s_Sinfo.s_nSerial);
	hItemData = m_treResultList.InsertItem(m_sDataMsg,hItem);
	sprintf(m_sDataMsg,"s_nDrebSerial=%d",head.s_Sinfo.s_nDrebSerial);
	hItemData = m_treResultList.InsertItem(m_sDataMsg,hItem);
	sprintf(m_sDataMsg,"s_nIndex=%d",head.s_Sinfo.s_nIndex);
	hItemData = m_treResultList.InsertItem(m_sDataMsg,hItem);
	sprintf(m_sDataMsg,"s_nTimestamp=%d",head.s_Sinfo.s_nTimestamp);
	hItemData = m_treResultList.InsertItem(m_sDataMsg,hItem);

	sprintf(m_sDataMsg,"DREB_D_NODEINFO");
	hItem = m_treResultList.InsertItem(m_sDataMsg,hParent);
	DisplayButton(hItem);
	sprintf(m_sDataMsg,"d_nNodeId=%d",head.d_Dinfo.d_nNodeId);
	hItemData = m_treResultList.InsertItem(m_sDataMsg,hItem);
	sprintf(m_sDataMsg,"d_cNodePrivateId=%d",head.d_Dinfo.d_cNodePrivateId);
	hItemData = m_treResultList.InsertItem(m_sDataMsg,hItem);
	sprintf(m_sDataMsg,"d_nSvrMainId=%d",head.d_Dinfo.d_nSvrMainId);
	hItemData = m_treResultList.InsertItem(m_sDataMsg,hItem);
	sprintf(m_sDataMsg,"d_cSvrPrivateId=%d",head.d_Dinfo.d_cSvrPrivateId);
	hItemData = m_treResultList.InsertItem(m_sDataMsg,hItem);
	sprintf(m_sDataMsg,"d_nServiceNo=%d",head.d_Dinfo.d_nServiceNo);
	hItemData = m_treResultList.InsertItem(m_sDataMsg,hItem);
	

	sprintf(m_sDataMsg,"DREB_A_NODEINFO");
	hItem = m_treResultList.InsertItem(m_sDataMsg,hParent);
	DisplayButton(hItem);
	sprintf(m_sDataMsg,"a_nNodeId=%d",head.a_Ainfo.a_nNodeId);
	hItemData = m_treResultList.InsertItem(m_sDataMsg,hItem);
	sprintf(m_sDataMsg,"a_cNodePrivateId=%d",head.a_Ainfo.a_cNodePrivateId);
	hItemData = m_treResultList.InsertItem(m_sDataMsg,hItem);
	sprintf(m_sDataMsg,"a_nRetCode=%d",head.a_Ainfo.a_nRetCode);
	hItemData = m_treResultList.InsertItem(m_sDataMsg,hItem);
	
	sprintf(m_sDataMsg,"DREB_NEXT_INFO");
	hItem = m_treResultList.InsertItem(m_sDataMsg,hParent);
	DisplayButton(hItem);
	sprintf(m_sDataMsg,"n_nNextNo=%d",head.n_Ninfo.n_nNextNo);
	hItemData = m_treResultList.InsertItem(m_sDataMsg,hItem);
	sprintf(m_sDataMsg,"n_nNextOffset=%d",head.n_Ninfo.n_nNextOffset);
	hItemData = m_treResultList.InsertItem(m_sDataMsg,hItem);

	sprintf(m_sDataMsg,"nLen=%d",head.nLen);
	hItem = m_treResultList.InsertItem(m_sDataMsg,hParent);

	int i;
// 	if (isans && head.a_Ainfo.a_nRetCode ==SUCCESS &&(head.cCmd == CMD_MONITOR_BPC || head.cCmd == CMD_MONITOR_DREB))
// 	{
// 		//显示结果
// 		if (head.cCmd == CMD_MONITOR_DREB)
// 		{
// 			S_MONITOR_INFO *mdrebinfo=NULL;
// 			S_MONITOR_SVR  *msvrinfo=NULL;
// 			S_MONITOR_HOST *mhostinfo=NULL;
// 			S_MONITOR_DISK *mdiskinfo=NULL;
// 			S_MONITOR_TXLIST *mtxlist=NULL;
// 			S_MONITOR_SERVICEINFO *msinfo=NULL;
// 			S_MONITOR_DREB *mdreb=NULL;
// 			S_MONITOR_ROUTEDREB *mrd=NULL;
// 			S_MONITOR_ROUTESERVICE *mrs=NULL;
// 			S_MONITOR_SERVICE      *msfunc=NULL;
// 			S_MONITOR_DREBREPORT *mdrp=NULL;
// 			S_MONITOR_CONNINFO   *mdcinfo=NULL;   
// 			int buflen=0;
// 			unsigned int *pNum;
// 			switch (head.d_Dinfo.d_nServiceNo)
// 			{
// 				case DREBMONI_INFO:
// 					if (head.nLen < sizeof(S_MONITOR_INFO))
// 					{
// 						m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"DREBMONI_INFO返回结果不符,nLen=%d",head.nLen);
// 						break;
// 					}
// 					buflen = sizeof(S_MONITOR_INFO);
// 					mdrebinfo = (S_MONITOR_INFO *)data;
// 					sprintf(m_sDataMsg,"DREBMONI_INFO");
// 					hItem = m_treResultList.InsertItem(m_sDataMsg,hParent);
// 					m_pDrebEndian.Endian2LocalHost((unsigned char *)&(mdrebinfo->nNodeId),sizeof(mdrebinfo->nNodeId));
// 					m_pDrebEndian.Endian2LocalHost((unsigned char *)&(mdrebinfo->nSvrNum),sizeof(mdrebinfo->nSvrNum));
// 					
// 					sprintf(m_sDataMsg,"nNodeId=%d",mdrebinfo->nNodeId);
// 					hItemData = m_treResultList.InsertItem(m_sDataMsg,hItem);
// 					sprintf(m_sDataMsg,"cNodePrivateId=%d",mdrebinfo->cNodePrivateId);
// 					hItemData = m_treResultList.InsertItem(m_sDataMsg,hItem);
// 					sprintf(m_sDataMsg,"nSvrNum=%d",mdrebinfo->nSvrNum);
// 					hItemData = m_treResultList.InsertItem(m_sDataMsg,hItem);
// 					msvrinfo = (S_MONITOR_SVR *)(data+sizeof(S_MONITOR_INFO));
// 					if (mdrebinfo->nSvrNum>0)
// 					{
// 						sprintf(m_sDataMsg,"SVRINFO");
// 						hItemData = m_treResultList.InsertItem(m_sDataMsg,hItem);
// 					}
// 					
// 					for (i=0 ; i< mdrebinfo->nSvrNum ; i++)
// 					{
// 						buflen+=sizeof(S_MONITOR_SVR);
// 						if (head.nLen < buflen)
// 						{
// 							m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"DREBMONI_INFO返回结果不符,nLen=%d buflen=%d",head.nLen,buflen);
// 							break;
// 						}
// 						m_pDrebEndian.Endian2LocalHost((unsigned char *)&(msvrinfo->nSvrMainId),sizeof(msvrinfo->nSvrMainId));
// 						sprintf(m_sDataMsg,"SVR%d: %d %d %d ",i+1,msvrinfo->nSvrMainId,msvrinfo->cSvrPrivateId,msvrinfo->cClose);
// 						threeItem = m_treResultList.InsertItem(m_sDataMsg,hItemData);
// 						msvrinfo++;
// 					}
// 					msvrinfo = NULL;
// 					break;
// 				case DREBMONI_HOST:
// 					if (head.nLen < sizeof(S_MONITOR_HOST))
// 					{
// 						m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"DREBMONI_HOST 返回结果不符,nLen=%d",head.nLen);
// 						break;
// 					}
// 					buflen = sizeof(S_MONITOR_HOST);
// 					mhostinfo = (S_MONITOR_HOST *)data;
// 					sprintf(m_sDataMsg,"HOST_INFO");
// 					hItem = m_treResultList.InsertItem(m_sDataMsg,hParent);
// 					m_pDrebEndian.Endian2LocalHost((unsigned char *)&(mhostinfo->nCpuRate),sizeof(mhostinfo->nCpuRate));
// 					m_pDrebEndian.Endian2LocalHost((unsigned char *)&(mhostinfo->nDiskNum),sizeof(mhostinfo->nDiskNum));
// 					m_pDrebEndian.Endian2LocalHost((unsigned char *)&(mhostinfo->nTotalMemory),sizeof(mhostinfo->nTotalMemory));
// 					m_pDrebEndian.Endian2LocalHost((unsigned char *)&(mhostinfo->nUsedMemory),sizeof(mhostinfo->nUsedMemory));
// 					
// 					sprintf(m_sDataMsg,"CPU使用率=%d%s",mhostinfo->nCpuRate,"%");
// 					hItemData = m_treResultList.InsertItem(m_sDataMsg,hItem);
// 					sprintf(m_sDataMsg,"总内存=%dM 使用内存=%dM 使用率=%d%s",mhostinfo->nTotalMemory,mhostinfo->nUsedMemory,mhostinfo->nUsedMemory*100/mhostinfo->nTotalMemory,"%");
// 					hItemData = m_treResultList.InsertItem(m_sDataMsg,hItem);
// 
// 					mdiskinfo = (S_MONITOR_DISK *)(data+sizeof(S_MONITOR_HOST));
// 					if (mhostinfo->nDiskNum>0)
// 					{
// 						sprintf(m_sDataMsg,"DISK_INFO %d",mhostinfo->nDiskNum);
// 						hItemData = m_treResultList.InsertItem(m_sDataMsg,hItem);
// 					}
// 					
// 					for (i=0 ; i< mhostinfo->nDiskNum ; i++)
// 					{
// 						buflen+=sizeof(S_MONITOR_DISK);
// 						if (head.nLen < buflen)
// 						{
// 							m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"DREBMONI_HOST 返回结果不符,nLen=%d buflen=%d",head.nLen,buflen);
// 							break;
// 						}
// 						m_pDrebEndian.Endian2LocalHost((unsigned char *)&(mdiskinfo->nTotalSpace),sizeof(mdiskinfo->nTotalSpace));
// 						m_pDrebEndian.Endian2LocalHost((unsigned char *)&(mdiskinfo->nUsedSpace),sizeof(mdiskinfo->nUsedSpace));
// 						if (mdiskinfo->nTotalSpace != 0)
// 						{
// 							sprintf(m_sDataMsg,"盘符名称: %s 总大小:%dM 使用大小:%dM 使用率:%d%s",mdiskinfo->sDiskName,mdiskinfo->nTotalSpace,mdiskinfo->nUsedSpace,mdiskinfo->nUsedSpace*100/mdiskinfo->nTotalSpace,"%");
// 						}
// 						else
// 						{
// 							sprintf(m_sDataMsg,"盘符名称: %s 总大小:%dM 使用大小:%dM",mdiskinfo->sDiskName,mdiskinfo->nTotalSpace,mdiskinfo->nUsedSpace);
// 						}
// 						
// 						threeItem = m_treResultList.InsertItem(m_sDataMsg,hItemData);
// 						mdiskinfo++;
// 					}
// 					mdiskinfo = NULL;
// 					break;
// 				case DREBMONI_CURTXLIST:
// 					if (head.nLen < (sizeof(S_MONITOR_TXLIST)-sizeof(S_MONITOR_SERVICEINFO)))
// 					{
// 						m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"DREBMONI_CURTXLIST 返回结果不符,nLen=%d",head.nLen);
// 						break;
// 					}
// 					buflen = sizeof(S_MONITOR_TXLIST)-sizeof(S_MONITOR_SERVICEINFO);
// 					mtxlist = (S_MONITOR_TXLIST *)data;
// 					sprintf(m_sDataMsg,"CURTX_INFO");
// 					hItem = m_treResultList.InsertItem(m_sDataMsg,hParent);
// 					m_pDrebEndian.Endian2LocalHost((unsigned char *)&(mtxlist->nNodeId),sizeof(mtxlist->nNodeId));
// 					m_pDrebEndian.Endian2LocalHost((unsigned char *)&(mtxlist->nTxNum),sizeof(mtxlist->nTxNum));
// 					sprintf(m_sDataMsg,"nNodeId=%d cNodePrivateId=%d ",mtxlist->nNodeId,mtxlist->cNodePrivateId);
// 					hItemData = m_treResultList.InsertItem(m_sDataMsg,hItem);
// 					sprintf(m_sDataMsg,"TX_INFO %d",mtxlist->nTxNum);
// 					hItemData = m_treResultList.InsertItem(m_sDataMsg,hItem);
// 					
// 					msinfo = (S_MONITOR_SERVICEINFO *)(data+sizeof(S_MONITOR_TXLIST)-sizeof(S_MONITOR_SERVICEINFO));
// 					for (i=0 ; i< mtxlist->nTxNum ; i++)
// 					{
// 						buflen+=sizeof(S_MONITOR_SERVICEINFO);
// 						if (head.nLen < buflen)
// 						{
// 							m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"DREBMONI_CURTXLIST 返回结果不符,nLen=%d buflen=%d",head.nLen,buflen);
// 							break;
// 						}
// 						m_pDrebEndian.Endian2LocalHost((unsigned char *)&(msinfo->nSvrMainId),sizeof(msinfo->nSvrMainId));
// 						m_pDrebEndian.Endian2LocalHost((unsigned char *)&(msinfo->nFuncNo),sizeof(msinfo->nFuncNo));
// 						m_pDrebEndian.Endian2LocalHost((unsigned char *)&(msinfo->nIndex),sizeof(msinfo->nIndex));
// 						m_pDrebEndian.Endian2LocalHost((unsigned char *)&(msinfo->nStep),sizeof(msinfo->nStep));
// 						sprintf(m_sDataMsg,"nSvrMainId:%d cSvrPrivateId:%d nFuncNo:%d nStep:%d nIndex:%d ",\
// 							msinfo->nSvrMainId,msinfo->cSvrPrivateId,msinfo->nFuncNo,msinfo->nStep,msinfo->nIndex);
// 						threeItem = m_treResultList.InsertItem(m_sDataMsg,hItemData);
// 						msinfo++;
// 					}
// 					
// 					break;
// 				case DREBMONI_ROUTEDREB:
// 					if (head.nLen < sizeof(mrd->nDrebNum))
// 					{
// 						m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"DREBMONI_ROUTEDREB 返回结果不符,nLen=%d",head.nLen);
// 						break;
// 					}
// 					buflen = sizeof(mrd->nDrebNum);
// 					mrd = (S_MONITOR_ROUTEDREB *)data;
// 					m_pDrebEndian.Endian2LocalHost((unsigned char *)&(mrd->nDrebNum),sizeof(mrd->nDrebNum));
// 					sprintf(m_sDataMsg,"DREBROUTE %d",mrd->nDrebNum);
// 					hItem = m_treResultList.InsertItem(m_sDataMsg,hParent);
// 					
// 					mdreb = (S_MONITOR_DREB *)(data+sizeof(mrd->nDrebNum));
// 					for (i=0 ; i< mrd->nDrebNum ; i++)
// 					{
// 						buflen+=sizeof(S_MONITOR_DREB);
// 						if (head.nLen < buflen)
// 						{
// 							m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"DREBMONI_ROUTEDREB 返回结果不符,nLen=%d buflen=%d",head.nLen,buflen);
// 							break;
// 						}
// 						m_pDrebEndian.Endian2LocalHost((unsigned char *)&(mdreb->nNodeId),sizeof(mdreb->nNodeId));
// 						m_pDrebEndian.Endian2LocalHost((unsigned char *)&(mdreb->nbandwidth),sizeof(mdreb->nbandwidth));
// 						m_pDrebEndian.Endian2LocalHost((unsigned char *)&(mdreb->nIndex),sizeof(mdreb->nIndex));
// 						m_pDrebEndian.Endian2LocalHost((unsigned char *)&(mdreb->nStep),sizeof(mdreb->nStep));
// 						sprintf(m_sDataMsg,"nNodeId:%d cNodePrivateId:%d nStep:%d nIndex:%d nbandwidth:%dk ",\
// 							mdreb->nNodeId,mdreb->cNodePrivateId,mdreb->nStep,mdreb->nIndex,mdreb->nbandwidth);
// 						hItemData = m_treResultList.InsertItem(m_sDataMsg,hItem);
// 						mdreb++;
// 					}
// 					
// 					break;
// 				case DREBMONI_ROUTESERVICE:
// 					if (head.nLen < sizeof(mrs->nServiceNum))
// 					{
// 						m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"DREBMONI_ROUTESERVICE 返回结果不符,nLen=%d",head.nLen);
// 						break;
// 					}
// 					buflen = sizeof(mrs->nServiceNum);
// 					mrs = (S_MONITOR_ROUTESERVICE *)data;
// 					m_pDrebEndian.Endian2LocalHost((unsigned char *)&(mrs->nServiceNum),sizeof(mrs->nServiceNum));
// 					sprintf(m_sDataMsg,"ROUTESERVICE_INFO %d",mrs->nServiceNum);
// 					hItem = m_treResultList.InsertItem(m_sDataMsg,hParent);
// 					
// 					msfunc = (S_MONITOR_SERVICE *)(data+sizeof(mrs->nServiceNum));
// 					for (i=0 ; i< mrs->nServiceNum ; i++)
// 					{
// 						buflen+=sizeof(S_MONITOR_SERVICE);
// 						if (head.nLen < buflen)
// 						{
// 							m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"DREBMONI_ROUTESERVICE 返回结果不符,nLen=%d buflen=%d",head.nLen,buflen);
// 							break;
// 						}
// 						m_pDrebEndian.Endian2LocalHost((unsigned char *)&(msfunc->nNodeId),sizeof(msfunc->nNodeId));
// 						m_pDrebEndian.Endian2LocalHost((unsigned char *)&(msfunc->nSvrMainId),sizeof(msfunc->nSvrMainId));
// 						m_pDrebEndian.Endian2LocalHost((unsigned char *)&(msfunc->nFuncNo),sizeof(msfunc->nFuncNo));
// 						m_pDrebEndian.Endian2LocalHost((unsigned char *)&(msfunc->nIndex),sizeof(msfunc->nIndex));
// 						m_pDrebEndian.Endian2LocalHost((unsigned char *)&(msfunc->nStep),sizeof(msfunc->nStep));
// 						sprintf(m_sDataMsg,"nNodeId:%d cNodePrivateId:%d nSvrMainId:%d cSvrPrivateId:%d nFuncNo:%d nStep:%d nIndex:%d ",\
// 							msfunc->nNodeId,msfunc->cNodePrivateId,msfunc->nSvrMainId,msfunc->cSvrPrivateId,msfunc->nFuncNo,msfunc->nStep,msfunc->nIndex);
// 						hItemData = m_treResultList.InsertItem(m_sDataMsg,hItem);
// 						msfunc++;
// 					}
// 					break;
// 				case DREBMONI_REPORT:
// 					if (head.nLen < sizeof(S_MONITOR_DREBREPORT)*2)
// 					{
// 						m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"DREBMONI_REPORT 返回结果不符,nLen=%d",head.nLen);
// 						break;
// 					}
// 					mdrp = (S_MONITOR_DREBREPORT *)data;
// 					sprintf(m_sDataMsg,"DREBMONI_REPORT");
// 					hItem = m_treResultList.InsertItem(m_sDataMsg,hParent);
// 					sprintf(m_sDataMsg,"启动时间 %s 每秒总流量=%I64d字节 每秒交易笔数=%I64d笔",mdrp->sStartDate,UINT64(atof(mdrp->sAnsByteClient)+atof(mdrp->sCallSrvByte)+\
// 						atof(mdrp->sRcvByteClient)+atof(mdrp->sSrvAnsByte)+atof(mdrp->sTransInByte)+\
// 						atof(mdrp->sTransOutByte)),UINT64(atof(mdrp->sAnsPackClient)+atof(mdrp->sCallSrvNum)+\
// 						atof(mdrp->sRcvPackClient)+atof(mdrp->sSrvAnsNum)+atof(mdrp->sTransInNum)+\
// 						atof(mdrp->sTransOutNum)) );
// 					hItemData = m_treResultList.InsertItem(m_sDataMsg,hItem);
// 					sprintf(m_sDataMsg,"每秒详细情况");
// 					hItemData = m_treResultList.InsertItem(m_sDataMsg,hItem);
// 					sprintf(m_sDataMsg,"AnsByteClient=%s CallSrvByte=%s RcvByteClient=%s SrvAnsByte=%s TransInByte=%s TransOutByte=%s",\
// 						mdrp->sAnsByteClient,mdrp->sCallSrvByte,\
// 						mdrp->sRcvByteClient,mdrp->sSrvAnsByte,mdrp->sTransInByte,\
// 						mdrp->sTransOutByte);
// 					threeItem = m_treResultList.InsertItem(m_sDataMsg,hItemData);
// 					sprintf(m_sDataMsg,"AnsPackClient=%s CallSrvNum=%s SrvAnsNum=%s TransInNum=%s TransOutNum=%s",\
// 						mdrp->sAnsPackClient,mdrp->sCallSrvNum,\
// 						mdrp->sRcvPackClient,mdrp->sSrvAnsNum,mdrp->sTransInNum,\
// 						mdrp->sTransOutNum);
// 					threeItem = m_treResultList.InsertItem(m_sDataMsg,hItemData);
// 
// 					mdrp++;
// 					sprintf(m_sDataMsg,"总共明细情况");
// 					hItemData = m_treResultList.InsertItem(m_sDataMsg,hItem);
// 					sprintf(m_sDataMsg,"AnsByteClient=%s CallSrvByte=%s RcvByteClient=%s SrvAnsByte=%s TransInByte=%s TransOutByte=%s",\
// 						mdrp->sAnsByteClient,mdrp->sCallSrvByte,\
// 						mdrp->sRcvByteClient,mdrp->sSrvAnsByte,mdrp->sTransInByte,\
// 						mdrp->sTransOutByte);
// 					threeItem = m_treResultList.InsertItem(m_sDataMsg,hItemData);
// 					sprintf(m_sDataMsg,"AnsPackClient=%s CallSrvNum=%s SrvAnsNum=%s TransInNum=%s TransOutNum=%s",\
// 						mdrp->sAnsPackClient,mdrp->sCallSrvNum,\
// 						mdrp->sRcvPackClient,mdrp->sSrvAnsNum,mdrp->sTransInNum,\
// 						mdrp->sTransOutNum);
// 					threeItem = m_treResultList.InsertItem(m_sDataMsg,hItemData);
// 					break;
// 				case DREBMONI_CONNINFO:
// 					if (head.nLen < sizeof(unsigned int))
// 					{
// 						m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"DREBMONI_REPORT 返回结果不符,nLen=%d",head.nLen);
// 						break;
// 					}
// 					pNum = (unsigned int *)data;
// 					if (head.nLen-sizeof(unsigned int) < sizeof(S_MONITOR_CONNINFO)*(*pNum))
// 					{
// 						m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"DREBMONI_REPORT 返回结果不符,nLen=%d",head.nLen);
// 						break;
// 					}
// 					
// 					mdcinfo = (S_MONITOR_CONNINFO *)(data+sizeof(unsigned int));
// 					m_pDrebEndian.Endian2LocalHost((unsigned char *)(pNum),sizeof(unsigned int));
// 					sprintf(m_sDataMsg,"连接信息 %d",*pNum);
// 					hItem = m_treResultList.InsertItem(m_sDataMsg,hParent);
// 					for (i=0; i< *pNum ; i++)
// 					{
// 						m_pDrebEndian.Endian2LocalHost((unsigned char *)(mdcinfo->nbandwidth),sizeof(mdcinfo->nbandwidth));
// 						m_pDrebEndian.Endian2LocalHost((unsigned char *)(mdcinfo->nNodeId),sizeof(mdcinfo->nNodeId));
// 						m_pDrebEndian.Endian2LocalHost((unsigned char *)(mdcinfo->nPort),sizeof(mdcinfo->nPort));
// 						if (mdcinfo->cStatus == 0)
// 						{
// 							sprintf(m_sDataMsg,"IP:%s 端口:%d  带宽:%dk 状态:%s 对端节点:%d %d ",mdcinfo->sIp,mdcinfo->nPort,\
// 								mdcinfo->nbandwidth,"正常",mdcinfo->nNodeId,mdcinfo->cNodePrivateId);
// 						}
// 						else
// 						{
// 							sprintf(m_sDataMsg,"IP:%s 端口:%d  带宽:%dk 状态:%s 对端节点:%d %d ",mdcinfo->sIp,mdcinfo->nPort,\
// 								mdcinfo->nbandwidth,"正在连接",mdcinfo->nNodeId,mdcinfo->cNodePrivateId);
// 						}
// 						hItemData = m_treResultList.InsertItem(m_sDataMsg,hItem);
// 						mdcinfo++;
// 						
// 					}
// 					//mdcinfo
// 
// 					break;
// 				case DREBMONI_REPORTSTART:
// 					break;
// 				case DREBMONI_REPORTSTOP:
// 					break;
// 				default:
// 					break;
// // #define  DREBMONI_INFO          1     //dreb信息 当前节点、当前节点服务
// // #define  DREBMONI_HOST          2     //主机信息 CPU 碰盘 内存
// // #define  DREBMONI_CURTXLIST     3     //dreb信息 当前节点交易列表
// // #define  DREBMONI_ROUTEDREB     4     //dreb路由信息 节点路由表
// // #define  DREBMONI_ROUTESERVICE  5     //dreb路由信息 服务交易路由表
// // #define  DREBMONI_REPORT        6     //dreb运行统计信息
// // #define  DREBMONI_CONNINFO      7     //dreb连接信息
// // #define  DREBMONI_REPORTSTART   8     //dreb开始统计信息 pack/s byte/s
// // #define  DREBMONI_REPORTSTOP    9     //dreb停止统计信息
// 
// 			}
// 		}
// 		else //  
// 		{
// 			switch (m_head.d_Dinfo.d_nServiceNo)
// 			{
// 				case DREBMONI_INFO:
// 					break;
// 				case DREBMONI_HOST:
// 					break;
// 			}
// 		}
// 		
// 		return;
// 	}

	//将数据写入到文件中
	if (isans && m_head.nLen>0)
	{
		struct_stat statfile;
		FSTAT(m_sDataFileName,&statfile);
		int offset=0;
		if (statfile.st_size <0 )
		{
			offset=0;
		}
		else
		{
			offset=statfile.st_size;
		}
//		sprintf(m_sDataMsg,"nLen=%d %d",m_head.nLen,offset);
		fp = fopen(m_sDataFileName,"wb");
		if (fp != NULL)
		{
			fwrite(m_sRcvData,1,m_head.nLen,fp);
			fclose(fp);
		}
	}
	else
	{
//		sprintf(m_sDataMsg,"nLen=%d",m_head.nLen);
	}
//	hItem = m_treResultList.InsertItem(m_sDataMsg,hParent);
}

void CDrebclientDlg::OnButtonClear() 
{
	// TODO: Add your control notification handler code here
	
// 	FILE *fp = fopen("tmp.dat","rb");
// 	struct_stat f_stat;
// 	FSTAT("tmp.dat",&f_stat);
// 	int ret=0;
// 	char sRcvBuffer[DREBBUFSIZE];
// 	unsigned char data[DREBBUFSIZE];
// 	unsigned int datalen;
// 	if (fp != NULL)
// 	{
// 		ret = fread(sRcvBuffer,1,f_stat.st_size+1,fp);
// 		fclose(fp);
// 	}
// 	if (ret >0)
// 	{
// 		datalen = sizeof(data);
// 		if (!CBF_Tools::Uncompress(data,datalen,(unsigned char *)sRcvBuffer,ret))
// 		{
// 			sprintf(m_errMsg,"数据解压缩失败");
// 			return;
// 		}
// 	}
	

	m_treResultList.DeleteAllItems();
}

void CDrebclientDlg::OnButtonRcvnext() 
{
	// TODO: Add your control notification handler code here
	memset(m_sRcvData,0,sizeof(m_sRcvData));
	unsigned int rcvlen = sizeof(m_sRcvData);
	int ret = m_pClient.ReceveData(m_head,(unsigned char *)m_sRcvData,rcvlen,m_nTimeOut);
	// >0收到的数据长度 =0在指定时间内未收到数据  <0失败 -100未连接 -1连接断开 -2通讯平台应答数据不符 -3CRC错误
	if (ret == 0 )
	{
		MessageBox("在指定时间未收到数据","提示");
		return ;
	}
	else if (ret <0)
	{
		MessageBox(m_pClient.m_errMsg,"提示");
		m_btnGetNext.EnableWindow(false);
		return ;
	}
	else if (ret >= DREBHEADLEN)
	{
		sprintf(m_sDataMsg,"应答 %s %d hook=%d serial=%d",m_combMessage.GetCurColumnItem(1),CBF_Date_Time::GetTickCount(),m_nHook,m_nSerial);
		m_hRoot = m_treResultList.InsertItem(m_sDataMsg);
		m_treResultList.SelectItem(m_hRoot);
		m_treResultList.Expand(m_hRoot,TVE_EXPAND);
		ShowPack(m_hRoot,m_head,m_sRcvData,true);
		m_nRcvNum++;
		sprintf(m_errMsg,"%d",m_nRcvNum);
		m_statusbar.SetPaneText(5,m_errMsg);
		if (m_head.cNextFlag == 0 || m_head.cNextFlag >=10)
		{
			m_btnGetNext.EnableWindow(false);
		}
		else
		{
			m_btnGetNext.EnableWindow(true);
		}
		return;
	}
	else
	{
		MessageBox("DREB应答数据不符","提示");
		return;
	}
}
void CDrebclientDlg::DisplayButton(HTREEITEM hti)
{
	TVITEM tvi;
	tvi.mask = TVIF_CHILDREN;
	tvi.hItem = hti;
	tvi.cChildren = 1;
	
	m_treResultList.SetItem(&tvi);
}
void CDrebclientDlg::HideButton(HTREEITEM hti)
{
	TVITEM tvi;
	tvi.mask = TVIF_CHILDREN;
	tvi.hItem = hti;
	tvi.cChildren = 0;
	
	m_treResultList.SetItem(&tvi);
}

void CDrebclientDlg::OnDblclkTreeResultlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
	HTREEITEM	hItem = m_treResultList.GetSelectedItem();
	CTreeCtrl &tree = m_treResultList;
	CString value = tree.GetItemText(hItem);
	HTREEITEM htiroot = tree.GetRootItem();
	HTREEITEM hti = hItem;
	int rec=0;
	while (hti !=NULL && hti!=htiroot)
	{
		hti = tree.GetParentItem(hti);
		rec++;	
	}
	if (rec !=2)
	{
		return;
	}
	int nlen;
	int noffset=0;
	if (strncmp(value.GetBuffer(value.GetLength()),"nLen=",5)==0)
	{
		if (GetLenOffset(value.GetBuffer(value.GetLength()),nlen,noffset))
		{
			FILE *fp = fopen(m_sDataFileName,"rb");
			if (fp == NULL)
			{
				return;
			}
			fseek(fp,noffset,1);
			memset(m_sRcvData,0,sizeof(m_sRcvData));
			m_nDataLen = fread(m_sRcvData,1,nlen,fp);
			
// 			CErrlog log;
// 			log.SetLogPara(5,"","data");
// 			log.LogBin(LOG_DEBUG,__FILE__,__LINE__,"data",m_sRcvData,m_nDataLen);

			fclose(fp);
			CResultView dlg;
			dlg.m_pXdp = &m_pXdp;
			dlg.m_pDataPoint = m_sRcvData;
			dlg.m_nDataLen = m_nDataLen;
			dlg.DoModal();
			dlg.m_nDataLen =0;
			dlg.m_pDataPoint = NULL;
		}
		return;
	}
}

bool CDrebclientDlg::GetLenOffset(const char *buf,int &len, int &offset)
{
	char slen[20];
	memset(slen,0,sizeof(slen));
	sscanf(buf,"%s %d",slen,&offset);
	for (int i=0;i<strlen(slen);i++)
	{
		if (slen[i]=='=')
		{
			len = atoi(slen+i+1);
			return true;
		}
	}
	return false;
}

void CDrebclientDlg::OnView() 
{
	// TODO: Add your control notification handler code here
	CResultView dlg;
	dlg.m_pDataPoint = m_sRcvData;
	dlg.m_nDataLen = m_head.nLen;
	dlg.DoModal();
	dlg.m_nDataLen =0;
	dlg.m_pDataPoint = NULL;
}

void CDrebclientDlg::DownUpLoadFile()
{
	CString ss;
	m_edtServiceNo.GetWindowText(ss);
	int txcode = atoi(ss.GetBuffer(ss.GetLength()));
	CDownUpFile  dlg;
	if (txcode == BPCMONI_DOWNLOAD)
	{
		dlg.m_nFileMode = 1;
	}
	else
	{
		dlg.m_nFileMode = 0;
	}
	int ret = dlg.DoModal();
	if (ret != IDOK)
	{
		return ;
	}
	CBF_Tools::LRtrim(dlg.m_sLocalFileName);
	CBF_Tools::LRtrim(dlg.m_sRemoteFileName);
	if (dlg.m_sLocalFileName.length()<1)
	{
		MessageBox("本地文件名为空","提示");
		return;
	}
	if (dlg.m_sRemoteFileName.length()<1)
	{
		MessageBox("远程文件名为空","提示");
		return;
	}
	
	if (dlg.m_nFileMode == 0)
	{
		UpLoadFile(dlg.m_sLocalFileName.c_str(),dlg.m_sRemoteFileName.c_str());	
	}
	else
	{
		DownLoadFile(dlg.m_sLocalFileName.c_str(),dlg.m_sRemoteFileName.c_str());	
	}
}

void CDrebclientDlg::DownLoadFile(const char *localfilename, const char *remotefilename)
{
	if (!m_pClient.IsConnect())
	{
		if (!m_pClient.Connect())
		{
			MessageBox(m_pClient.m_errMsg,"提示");
			sprintf(m_errMsg,"连接DREB[%s %d]失败 %s",m_sDrebIp.c_str(),m_nDrebPort,m_pClient.m_errMsg);
			m_statusbar.SetPaneText(1,m_errMsg);
			return;
		}
		sprintf(m_errMsg,"连接DREB[%s %d]成功",m_sDrebIp.c_str(),m_nDrebPort);
		m_statusbar.SetPaneText(1,m_errMsg);
	}
	
	m_pClient.GetDrebHead(m_head);
	m_head.cRaflag = 0;
	m_head.cZip = 0;
	
	m_head.cNextFlag = 0;
	m_head.cDrebAffirm = 0;
	m_head.cCmd = CMD_MONITOR_BPC;
	
	memcpy(&m_head.d_Dinfo,&m_pDrebDest,sizeof(DREB_D_NODEINFO));
	m_head.s_Sinfo.s_nHook = m_nHook;
	m_head.s_Sinfo.s_nSerial = m_nSerial;
	
	//文件头
	char tmpchar[100];
	bzero(tmpchar,sizeof(tmpchar));
	S_BPC_DUFILE sfile;
	bzero(&sfile,sizeof(S_BPC_DUFILE));
	memcpy(sfile.sFileName,remotefilename,strlen(remotefilename));
	sprintf(tmpchar,"%011ld",0);
	memcpy(sfile.sFileOffset,tmpchar,11);
	
	memcpy(m_sSendData,&sfile,sizeof(S_BPC_DUFILE));
	m_head.nLen = sizeof(S_BPC_DUFILE);
	m_nDataLen = sizeof(S_BPC_DUFILE);

	sprintf(m_sDataMsg,"请求 %s %d hook=%d serial=%d",m_combMessage.GetCurColumnItem(1),CBF_Date_Time::GetTickCount(),m_nHook,m_nSerial);
	m_hRoot = m_treResultList.InsertItem(m_sDataMsg);
	m_treResultList.SelectItem(m_hRoot);
	m_treResultList.Expand(m_hRoot,TVE_EXPAND);
	ShowPack(m_hRoot,m_head,m_sSendData);
	int curtime = CBF_Date_Time::GetTickCount();
	int nRet=0;
	int offset = 0;

	nRet = MonitorBpc();
	if (nRet != 0)
	{
		return ;
	}
	FILE *fp=NULL;
	fp = fopen(localfilename,"wb");
	if (fp == NULL)
	{
		MessageBox("打开本地文件失败","提示");
		return;
	}
	int filesize = 0;
	while (m_head.a_Ainfo.a_nRetCode == SUCCESS)
	{
		bzero(&sfile,sizeof(S_BPC_DUFILE));
		memcpy(&sfile,m_sRcvData,sizeof(S_BPC_DUFILE));
		fwrite(m_sRcvData+sizeof(S_BPC_DUFILE),1,m_head.nLen-sizeof(S_BPC_DUFILE),fp);
		offset = offset +m_head.nLen-sizeof(S_BPC_DUFILE);
		filesize = atol(sfile.sFileSize);
		if (sfile.cEndFlag == '1' ||  offset >= filesize)
		{
			sprintf(m_sDataMsg,"应答 %s %d hook=%d serial=%d  时间=%d毫秒",m_combMessage.GetCurColumnItem(1),CBF_Date_Time::GetTickCount(),m_nHook,m_nSerial,CBF_Date_Time::GetTickCount()-curtime);
			m_hRoot = m_treResultList.InsertItem(m_sDataMsg);
			m_treResultList.SelectItem(m_hRoot);
			m_treResultList.Expand(m_hRoot,TVE_EXPAND);
			ShowPack(m_hRoot,m_head,m_sRcvData,true);
			fclose(fp);
			return;
		}
		memcpy(m_sSendData,&sfile,sizeof(S_BPC_DUFILE));
		m_head.nLen = sizeof(S_BPC_DUFILE);
		m_nDataLen = sizeof(S_BPC_DUFILE);
		nRet = MonitorBpc();
		if (nRet != 0)
		{
			fclose(fp);
			return ;
		}
	}
	MessageBox("下载文件返回失败");
	sprintf(m_sDataMsg,"应答 %s %d hook=%d serial=%d  时间=%d毫秒",m_combMessage.GetCurColumnItem(1),CBF_Date_Time::GetTickCount(),m_nHook,m_nSerial,CBF_Date_Time::GetTickCount()-curtime);
	m_hRoot = m_treResultList.InsertItem(m_sDataMsg);
	m_treResultList.SelectItem(m_hRoot);
	m_treResultList.Expand(m_hRoot,TVE_EXPAND);
	ShowPack(m_hRoot,m_head,m_sRcvData,true);
	fclose(fp);
	return;
}

void CDrebclientDlg::UpLoadFile(const char *localfilename, const char *remotefilename)
{
	if (!m_pClient.IsConnect())
	{
		if (!m_pClient.Connect())
		{
			MessageBox(m_pClient.m_errMsg,"提示");
			sprintf(m_errMsg,"连接DREB[%s %d]失败 %s",m_sDrebIp.c_str(),m_nDrebPort,m_pClient.m_errMsg);
			m_statusbar.SetPaneText(1,m_errMsg);
			return;
		}
		sprintf(m_errMsg,"连接DREB[%s %d]成功",m_sDrebIp.c_str(),m_nDrebPort);
		m_statusbar.SetPaneText(1,m_errMsg);
	}
	m_pClient.GetDrebHead(m_head);
	m_head.cRaflag = 0;
	m_head.cZip = 0;
	
	m_head.cNextFlag = 0;
	m_head.cDrebAffirm = 0;
	m_head.cCmd = CMD_MONITOR_BPC;
	
	memcpy(&m_head.d_Dinfo,&m_pDrebDest,sizeof(DREB_D_NODEINFO));
	m_head.s_Sinfo.s_nHook = m_nHook;
	m_head.s_Sinfo.s_nSerial = m_nSerial;
	
	struct_stat fstat;
	FSTAT(localfilename,&fstat);
	if (fstat.st_size <1)
	{
		MessageBox("文件大小不符","提示");
		return;
	}
	//文件头
	char tmpchar[100];
	bzero(tmpchar,sizeof(tmpchar));
	S_BPC_DUFILE sfile;
	bzero(&sfile,sizeof(S_BPC_DUFILE));
	memcpy(sfile.sFileName,remotefilename,strlen(remotefilename));
	sprintf(tmpchar,"%011ld",fstat.st_size);
	memcpy(sfile.sFileSize,tmpchar,11);//文件大小
	int nSendLen = 0;
	sfile.cBeginFlag = '1';
	if (fstat.st_size <= 8*1024)
	{
		sfile.cEndFlag = '1';
	}
	memcpy(m_sSendData,&sfile,sizeof(S_BPC_DUFILE));
	FILE *fp=NULL;
	fp = fopen(localfilename,"rb");
	if (fp == NULL)
	{
		MessageBox("打开本地文件失败","提示");
		return;
	}
	m_nDataLen = fread(m_sSendData+sizeof(S_BPC_DUFILE),1,8*1024,fp);
	m_nDataLen = m_nDataLen+sizeof(S_BPC_DUFILE);
	m_head.nLen = m_nDataLen;

	sprintf(m_sDataMsg,"请求 %s %d hook=%d serial=%d",m_combMessage.GetCurColumnItem(1),CBF_Date_Time::GetTickCount(),m_nHook,m_nSerial);
	m_hRoot = m_treResultList.InsertItem(m_sDataMsg);
	m_treResultList.SelectItem(m_hRoot);
	m_treResultList.Expand(m_hRoot,TVE_EXPAND);
	ShowPack(m_hRoot,m_head,m_sSendData);
	int curtime = CBF_Date_Time::GetTickCount();
	int nRet;
	while (nSendLen < fstat.st_size)
	{
		nSendLen =nSendLen + m_nDataLen-sizeof(S_BPC_DUFILE);
		nRet = MonitorBpc();
		if (nRet != 0)
		{
			fclose(fp);
			return ;
		}
		if (m_head.a_Ainfo.a_nRetCode != SUCCESS)
		{
			fclose(fp);
			MessageBox("上传失败","提示");
			return;
		}
		if (nSendLen >= fstat.st_size) //发送完成
		{
			sprintf(m_sDataMsg,"应答 %s %d hook=%d serial=%d  时间=%d毫秒",m_combMessage.GetCurColumnItem(1),CBF_Date_Time::GetTickCount(),m_nHook,m_nSerial,CBF_Date_Time::GetTickCount()-curtime);
			m_hRoot = m_treResultList.InsertItem(m_sDataMsg);
			m_treResultList.SelectItem(m_hRoot);
			m_treResultList.Expand(m_hRoot,TVE_EXPAND);
			ShowPack(m_hRoot,m_head,m_sRcvData,true);
			fclose(fp);
			return;
		}
		memcpy(sfile.sFileName,remotefilename,strlen(remotefilename));
		sprintf(tmpchar,"%011ld",fstat.st_size);
		memcpy(sfile.sFileSize,tmpchar,11);//文件大小
		sfile.cBeginFlag = '0';
		m_nDataLen = fread(m_sSendData+sizeof(S_BPC_DUFILE),1,8*1024,fp);
		m_nDataLen +=sizeof(S_BPC_DUFILE);
		m_head.nLen = m_nDataLen;
		if (m_nDataLen<8*1024 || m_nDataLen+nSendLen-sizeof(S_BPC_DUFILE) >= fstat.st_size)
		{
			sfile.cEndFlag = '1';
		}
		else
		{
			sfile.cEndFlag = '0';
		}
		memcpy(m_sSendData,&sfile,sizeof(S_BPC_DUFILE));
	
	}
	MessageBox("下载文件返回失败");
	sprintf(m_sDataMsg,"应答 %s %d hook=%d serial=%d  时间=%d毫秒",m_combMessage.GetCurColumnItem(1),CBF_Date_Time::GetTickCount(),m_nHook,m_nSerial,CBF_Date_Time::GetTickCount()-curtime);
	m_hRoot = m_treResultList.InsertItem(m_sDataMsg);
	m_treResultList.SelectItem(m_hRoot);
	m_treResultList.Expand(m_hRoot,TVE_EXPAND);
	ShowPack(m_hRoot,m_head,m_sRcvData,true);
	fclose(fp);
	return;
}

int CDrebclientDlg::DpBcUnRegTx()
{
	int ret;
	m_btnGetNext.EnableWindow(false);
	ret = m_pClient.SendData(m_nHook,m_nSerial,0,CMD_DPBC_UNREGTX,m_pDrebDest,m_sSendData,m_nDataLen,m_cZip);
	//ret = m_pClient.DPABC(m_nHook,m_nSerial,m_pDrebDest,m_sSendData,m_nDataLen,m_cZip);
	if (ret == m_nDataLen)
	{
		m_nSendNum++;
		sprintf(m_errMsg,"%d",m_nSendNum);
		m_statusbar.SetPaneText(3,m_errMsg);
		// 		sprintf(m_errMsg,"%d",m_nRcvNum);
		// 		m_statusbar.SetPaneText(5,m_errMsg);
		m_btnGetNext.EnableWindow(true);
		sprintf(m_sDataMsg,"%s 发送成功",m_combMessage.GetCurColumnItem(1));
		m_hRoot = m_treResultList.InsertItem(m_sDataMsg);
		m_treResultList.SelectItem(m_hRoot);
		m_treResultList.Expand(m_hRoot,TVE_EXPAND);
		return -1;
	}
	else 
	{
		MessageBox(m_pClient.m_errMsg,"提示");
		return -1;
	}
}

void CDrebclientDlg::OnButtonXdp() 
{
	// TODO: Add your control notification handler code here
	char filename[400];
	memset(filename,0,sizeof(filename));
	WORKPATH(filename);
	CString ss;
	int ret;
	
	
	m_xdpdlg.m_pXdp = &m_pXdp;
	ret = m_xdpdlg.DoModal();
	
	
	if (ret !=IDOK)
	{
		return;
	}
	
	if (m_xdpdlg.m_nXdpDataLen >0)
	{
		memcpy(m_sSendData,m_xdpdlg.m_xdpData,m_xdpdlg.m_nXdpDataLen );
		m_nDataLen = m_xdpdlg.m_nXdpDataLen;
		m_sSendData[m_nDataLen]=0;
	}
	m_bFromXdp = true;
	m_rEditData.SetWindowText(m_sSendData);
	m_rEditData.EnableWindow(true);
	//将工作路径设置回去
	SetCurrentDirectory(filename);
	UpdateData(false);
	m_btnSend.SetFocus();
}
