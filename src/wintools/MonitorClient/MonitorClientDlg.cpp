
// MonitorClientDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MonitorClient.h"
#include "MonitorClientDlg.h"
#include "afxdialogex.h"
#include "BF_Des.h"
#include "BF_3Des.h"
#include "SetDatabase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define  DBPWDKEY  "rd402wms"
#define  DBPWD3DESKEY   "rd402wms20092017Ylink123"


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMonitorClientDlg 对话框



CMonitorClientDlg::CMonitorClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MONITORCLIENT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMonitorClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_BEGIN, m_edtBegin);
	DDX_Control(pDX, IDC_EDIT_END, m_edtEnd);
	DDX_Control(pDX, IDC_EDIT_SEP, m_edtSep);
	DDX_Control(pDX, IDC_LIST_APP_NODE, m_lstAppNode);
	DDX_Control(pDX, IDC_LIST_CPU_RAM, m_lstCpuRam);
	DDX_Control(pDX, IDC_LIST_DISK, m_lstDisk);
	DDX_Control(pDX, IDC_LIST_PC_NODE, m_lstPcNode);
	DDX_Control(pDX, IDC_BUTTON_STOP, m_btnStop);
	DDX_Control(pDX, IDC_BUTTON_SET, m_btnSet);
	DDX_Control(pDX, IDOK, m_btnOk);
}

BEGIN_MESSAGE_MAP(CMonitorClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CMonitorClientDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CMonitorClientDlg::OnBnClickedButtonStop)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_SET, &CMonitorClientDlg::OnBnClickedButtonSet)
END_MESSAGE_MAP()


// CMonitorClientDlg 消息处理程序

BOOL CMonitorClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	m_bIsPlaying = false;
	m_music_open.lpstrElementName= "err.wav";
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	m_picstatus_app = true;
	m_picstatus_pc = true;
	// TODO: 在此添加额外的初始化代码
	m_pDbApi = NULL;
	m_edtBegin.SetWindowText("091000");
	m_edtEnd.SetWindowText("150000");
	m_edtSep.SetWindowText("5");
	m_edtSep.SetLimitText(1);
	m_edtBegin.SetLimitText(6);
	m_edtEnd.SetLimitText(6);

    m_lstPcNode.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP);
	m_lstPcNode.InsertColumn(0, "ID", LVCFMT_CENTER, 80);
	m_lstPcNode.InsertColumn(1, "服务器名称", LVCFMT_LEFT, 300);
	m_lstPcNode.InsertColumn(2, "IP地址", LVCFMT_CENTER, 240);
	m_lstPcNode.InsertColumn(3, "所在区域", LVCFMT_CENTER, 300);
	m_lstPcNode.InsertColumn(4, "内存警告线", LVCFMT_CENTER, 250);
	m_lstPcNode.InsertColumn(5, "CPU警告线", LVCFMT_CENTER, 250);
	m_lstPcNode.InsertColumn(6, "磁盘警告线", LVCFMT_CENTER, 250);

    m_lstCpuRam.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP);
	m_lstCpuRam.InsertColumn(0, "ID", LVCFMT_CENTER, 80);
	m_lstCpuRam.InsertColumn(1, "CPU使用率", LVCFMT_CENTER, 160);
	m_lstCpuRam.InsertColumn(2, "总内存M", LVCFMT_CENTER, 160);
	m_lstCpuRam.InsertColumn(3, "使用内存M", LVCFMT_CENTER, 160);
	m_lstCpuRam.InsertColumn(4, "内存使用率", LVCFMT_CENTER, 160);

    m_lstDisk.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP);
	m_lstDisk.InsertColumn(0, "ID", LVCFMT_CENTER, 80);
	m_lstDisk.InsertColumn(1, "磁盘名称", LVCFMT_LEFT, 160);
	m_lstDisk.InsertColumn(2, "总大小M", LVCFMT_CENTER, 160);
	m_lstDisk.InsertColumn(3, "已使用M", LVCFMT_CENTER, 160);
	m_lstDisk.InsertColumn(4, "使用率", LVCFMT_CENTER, 160);

    m_lstAppNode.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP);
	m_lstAppNode.InsertColumn(0, "应用编号", LVCFMT_CENTER, 130);
	m_lstAppNode.InsertColumn(1, "应用名称", LVCFMT_LEFT, 320);
	m_lstAppNode.InsertColumn(2, "应用类型", LVCFMT_CENTER, 160);
	m_lstAppNode.InsertColumn(3, "所在服务器", LVCFMT_LEFT, 350);
	m_lstAppNode.InsertColumn(4, "上报日期时间", LVCFMT_CENTER, 230);
	m_lstAppNode.InsertColumn(5, "状态", LVCFMT_CENTER, 160);
	m_lstAppNode.InsertColumn(6, "应用启动时间", LVCFMT_CENTER, 250);
    if (!m_statusbar.Create(this) ||
        !m_statusbar.SetIndicators(indicators,
            sizeof(indicators) / sizeof(UINT)))
    {
        TRACE0("Failed to create status bar\n");
        return false;      // fail to create
    }
    m_statusbar.SetPaneInfo(0, 0, SBPS_NORMAL, 1000);
    m_statusbar.SetPaneText(0, "  ");
    m_statusbar.SetPaneInfo(1, 0, SBPS_NORMAL, 1000);
    m_statusbar.SetPaneText(1, "  ");
    m_statusbar.SetPaneInfo(2, 0, SBPS_NORMAL, 500);
    char stmpchar[200];
    m_statusbar.SetPaneText(2, m_pDate.ToStringDT17().c_str());
    //让这个状态栏最终显示在对话框中
    RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
	m_timerID2=0;
	m_timerID1 = SetTimer(ONTIMER1, 1, NULL);//定时器

	m_btnStop.EnableWindow(FALSE);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMonitorClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMonitorClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMonitorClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMonitorClientDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CString csSep;
	CString csBegin;
	CString csEnd;
	if (m_edtSep.GetWindowTextLength() < 1)
	{
		MessageBox("请输入监控间隔s","错误");
		m_edtSep.SetFocus();
		return;
	}
    if (m_edtBegin.GetWindowTextLength() < 1)
    {
        MessageBox("请输入开始时间", "错误");
		m_edtBegin.SetFocus();
        return;
    }
    if (m_edtEnd.GetWindowTextLength() < 1)
    {
        MessageBox("请输入结束时间s", "错误");
		m_edtEnd.SetFocus();
        return;
    }
    if (!ConnectDb())
    {
        MessageBox("连接监控数据库出错", "错误");
        m_statusbar.SetPaneText(1, m_errMsg.c_str());
        UpdateData(TRUE);
        return;
    }

	m_edtSep.GetWindowText(csSep);
	m_edtBegin.GetWindowText(csBegin);
	m_edtEnd.GetWindowText(csEnd);

	m_nMonitorSep = atoi(csSep.GetBuffer());
	m_nBeginTime = atoi(csBegin.GetBuffer());
	m_nEndTime = atoi(csEnd.GetBuffer());
	m_timerID2 = SetTimer(ONTIMER2, m_nMonitorSep*1000, NULL);//定时器

	//CDialogEx::OnOK();
    m_btnOk.EnableWindow(FALSE);
    m_btnStop.EnableWindow(TRUE);
	m_edtSep.EnableWindow(FALSE);
	m_edtBegin.EnableWindow(FALSE);
	m_edtEnd.EnableWindow(FALSE);
	char tmpchar[300];
	m_pDate.Update();
	sprintf(tmpchar,"          启动监控 开始:%d 结束:%d 间隔:%d秒    启动时间-%s", m_nBeginTime,m_nEndTime,m_nMonitorSep,m_pDate.ToStringDT17().c_str());
	m_statusbar.SetPaneText(0, tmpchar);
	UpdateData(TRUE);
	GetPcNodeInfo();
	m_picstatus_app = true;
	m_picstatus_pc = true;
}


void CMonitorClientDlg::OnBnClickedButtonStop()
{
	// TODO: 在此添加控件通知处理程序代码
	KillTimer(m_timerID2);
	m_btnOk.EnableWindow(TRUE);
	m_btnStop.EnableWindow(FALSE);
    m_edtSep.EnableWindow(TRUE);
    m_edtBegin.EnableWindow(TRUE);
    m_edtEnd.EnableWindow(TRUE);
    char tmpchar[300];
    m_pDate.Update();
    sprintf(tmpchar, "          停止监控 停止时间-%s", m_pDate.ToStringDT17().c_str());
    m_statusbar.SetPaneText(0, tmpchar);
}


void CMonitorClientDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == m_timerID1)
	{
        m_pDate.Update();
        m_statusbar.SetPaneText(2, m_pDate.ToStringDT17().c_str());
        UpdateData(true);
	}
	else if (nIDEvent == m_timerID2)	//取监控信息
	{
		m_pDate.Update();
		unsigned int curtime= atoi(m_pDate.ToStringT6().c_str());
		if (curtime >= m_nBeginTime && curtime <= m_nEndTime)
		{
            char tmpchar[300];
            sprintf(tmpchar, "     正在监控");
            m_statusbar.SetPaneText(1, tmpchar);
			Monitor();
		}
		else
		{
            char tmpchar[300];
            sprintf(tmpchar, "     当前时间:%d 不在监控时间[%d-%d]范围", curtime,m_nBeginTime,m_nEndTime);
			m_statusbar.SetPaneText(1, tmpchar);
			GetDlgItem(IDC_BUTTON_STOP)->PostMessage(BM_CLICK);
		}
	}

	CDialogEx::OnTimer(nIDEvent);
}
void CMonitorClientDlg::Monitor()
{
	if (!m_pDbApi->IsConnect())
	{
		if (!m_pDbApi->ReConnect())
		{
            m_statusbar.SetPaneText(1,"重连数据库失败");
			return;
		}
	}
	GetAppNodeInfo();
	GetPcCpuMemDiskInfo();
}
bool CMonitorClientDlg::ConnectDb()
{
    char tmpchar[100];
    memset(tmpchar, 0, sizeof(tmpchar));
    std::string dbname;
    std::string dbuser;
    std::string dbpwd;
    std::string dbpwdflag;
    std::string dbtype;

	if (!m_pXml.FromFile("monitorclient.xml"))
	{
		m_errMsg = std::string("monitorclient.xml不存在或非法");
		return false;
	}
    if (m_pXml.GetNodeValueByPath("package/head/business/database/databaseserver", false, dbname, false) == NULL)
    {
        m_errMsg = std::string("节点[package/head/business/database/databaseserver]不存在");
        m_pLog.LogMp(LOG_ERROR, __FILE__, __LINE__, m_errMsg.c_str());
        return false;
    }
    if (m_pXml.GetNodeValueByPath("package/head/business/database/databaseuser", false, dbuser, false) == NULL)
    {
        m_errMsg = std::string("节点[package/head/business/database/databaseuser]不存在");
        m_pLog.LogMp(LOG_ERROR, __FILE__, __LINE__, m_errMsg.c_str());
        return false;
    }
    if (m_pXml.GetNodeValueByPath("package/head/business/database/userpasswd", false, dbpwd, true) == NULL)
    {
        m_errMsg = std::string("节点[package/head/business/database/userpasswd]不存在");
        m_pLog.LogMp(LOG_ERROR, __FILE__, __LINE__, m_errMsg.c_str());
        return false;
    }
    if (m_pXml.GetNodeValueByPath("package/head/business/database/passwdtype", false, dbpwdflag, false) == NULL)
    {
        m_errMsg = std::string("节点[package/head/business/database/passwdtype]不存在");
        m_pLog.LogMp(LOG_ERROR, __FILE__, __LINE__, m_errMsg.c_str());
        return false;
    }
    if (m_pXml.GetNodeValueByPath("package/head/business/database/databasetype", false, dbtype, false) == NULL)
    {
        m_errMsg = std::string("节点[package/head/business/database/databasetype]不存在");
        m_pLog.LogMp(LOG_ERROR, __FILE__, __LINE__, m_errMsg.c_str());
        return false;
    }
    m_pLog.LogMp(LOG_INFO, __FILE__, __LINE__, "开始连接数据库");

    strcpy(tmpchar, dbpwd.c_str());
    if (atoi(dbpwdflag.c_str()) == 1)
    {
		CBF_Des     pEncrypt;//des加解密类
        int len = strlen(tmpchar);
        if (pEncrypt.UncryptString(tmpchar, len, DBPWDKEY) < 0)
        {
            m_pLog.LogMp(LOG_ERROR, __FILE__, __LINE__, "数据库密码解密出错");
            return false;
        }
    }
    else if (atoi(dbpwdflag.c_str()) == 2)
    {
        int len = strlen(tmpchar);
        CBF_3Des des;
        char errmsg[200];
        if (!des.UnDes3Buffer(tmpchar, len, DBPWD3DESKEY, errmsg))
        {
            m_pLog.LogMp(LOG_ERROR, __FILE__, __LINE__, "数据库密码解密出错 %s", errmsg);
            return false;
        }
    }
	if (m_pDbApi == NULL)
	{
		m_pDbApi = new CDbApi();
	}
	else
	{
		m_pDbApi->DisConnect();
	}
    if (m_pDbApi == NULL)
    {
        m_errMsg = std::string("数据库连接实例化失败，分配空间失败");
        m_pLog.LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, m_errMsg.c_str());
        return false;
    }
	
    if (!m_pDbApi->Connect(dbname, dbuser, tmpchar, atoi(dbtype.c_str())))
    {
        m_errMsg = std::string("连接数据库错:") + m_pDbApi->m_dbconn.m_sErrInfo;
        m_pLog.LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "连接数据库错 %d %s", m_pDbApi->m_dbconn.m_nErrCode, m_pDbApi->m_dbconn.m_sErrInfo.c_str());
        return false;
    }
    m_pLog.LogMp(LOG_INFO, __FILE__, __LINE__, "连接数据库成功");
	m_errMsg = "连接数据库成功";
	return true;
}
void CMonitorClientDlg::GetPcNodeInfo()
{
    sprintf(m_sSql, "select pc_node_id,pc_node_name,pc_ip1,pc_area,pc_mem_warn,pc_cpu_warn,pc_disk_warn from BFM_PC_NODE order by pc_node_id");
    if (!m_pDbApi->Execute(m_sSql))
    {
        m_pLog.LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "查询数据库出错 %s", m_pDbApi->GetCmdErrInfo());
        m_statusbar.SetPaneText(1, m_pDbApi->GetCmdErrInfo());
        return;
    }
	S_PC_NODE_INFO info;
    m_vAppNodeInfo.clear();
    while (m_pDbApi->FetchNext())
    {
        bzero(&info, sizeof(S_PC_NODE_INFO));
        info.pc_node_id = m_pDbApi->Field(0).asLong();
        strcpy(info.pc_name, m_pDbApi->Field(1).asString());
        strcpy(info.sip, m_pDbApi->Field(2).asString());
		strcpy(info.area, m_pDbApi->Field(3).asString());
        info.warn_mem = m_pDbApi->Field(4).asLong();
		info.warn_cpu = m_pDbApi->Field(5).asLong();
        info.warn_disk = m_pDbApi->Field(6).asLong();
        m_vPcNodeInfo.push_back(info);
    }
	UpdatePcNode();
}
void CMonitorClientDlg::GetAppNodeInfo()
{
    sprintf(m_sSql, "select a.public_id,a.private_id,a.app_name,a.app_type,a.pc_node_id,a.rpt_date,a.rpt_time,a.node_status,a.start_datetime,b.pc_node_name from BFM_APP_NODE a,BFM_PC_NODE b where a.pc_node_id=b.pc_node_id order by a.public_id,a.private_id");
	if (!m_pDbApi->Execute(m_sSql))
	{
		m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"查询数据库出错 %s",m_pDbApi->GetCmdErrInfo());
		m_statusbar.SetPaneText(1, m_pDbApi->GetCmdErrInfo());
		return ;
	}
	S_APP_NODE_INFO info;
	m_vAppNodeInfo.clear();
	while (m_pDbApi->FetchNext())
	{
		bzero(&info,sizeof(S_APP_NODE_INFO));
		info.public_id = m_pDbApi->Field(0).asLong();
		info.private_id = m_pDbApi->Field(1).asLong();
		strcpy(info.app_name,m_pDbApi->Field(2).asString());
		info.app_type = m_pDbApi->Field(3).asLong();
		info.pc_node_id = m_pDbApi->Field(4).asLong();
		strcpy(info.rpt_date,m_pDbApi->Field(5).asString());
		info.rpt_time = m_pDbApi->Field(6).asLong();
		info.node_status = m_pDbApi->Field(7).asLong();
		strcpy(info.start_time,m_pDbApi->Field(8).asString());
		strcpy(info.pc_name,m_pDbApi->Field(9).asString());
		m_vAppNodeInfo.push_back(info);
	}
	UpdateAppNode();
}
void CMonitorClientDlg::GetPcCpuMemDiskInfo()
{
    sprintf(m_sSql, "select a.pc_total_ram,a.pc_used_ram,b.pc_node_cpu,c.pc_node_id,c.pc_node_name,c.pc_mem_warn,c.pc_cpu_warn from BFM_PC_NODE_RAM a,BFM_PC_NODE_CPU b,BFM_PC_NODE c where a.pc_node_id=b.pc_node_id and c.pc_node_id=a.pc_node_id");
    if (!m_pDbApi->Execute(m_sSql))
    {
        m_pLog.LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "查询数据库出错 %s", m_pDbApi->GetCmdErrInfo());
        m_statusbar.SetPaneText(1, m_pDbApi->GetCmdErrInfo());
        return;
    }
	S_PC_NODE_CPU_RAM_INFO info;
	m_vPcNodeCpuRamInfo.clear();
    while (m_pDbApi->FetchNext())
    {
        bzero(&info, sizeof(S_PC_NODE_CPU_RAM_INFO));
        info.totalram = m_pDbApi->Field(0).asLong();
        info.ramused = m_pDbApi->Field(1).asLong();
        info.cpurate = m_pDbApi->Field(2).asLong();
        info.pc_node_id = m_pDbApi->Field(3).asLong();
        strcpy(info.pc_name, m_pDbApi->Field(4).asString());
		info.warn_mem = m_pDbApi->Field(5).asLong();
		info.warn_cpu = m_pDbApi->Field(6).asLong();
		m_vPcNodeCpuRamInfo.push_back(info);
    }
    sprintf(m_sSql, "select a.pc_node_id,a.pc_disk_name,a.pc_disk_total,a.pc_disk_used,b.pc_node_name,b.pc_disk_warn from BFM_PC_NODE_DISK a,BFM_PC_NODE b where a.pc_node_id=b.pc_node_id");
    if (!m_pDbApi->Execute(m_sSql))
    {
        m_pLog.LogMp(LOG_ERROR_GENERAL, __FILE__, __LINE__, "查询数据库出错 %s", m_pDbApi->GetCmdErrInfo());
        m_statusbar.SetPaneText(1, m_pDbApi->GetCmdErrInfo());
        return;
    }
    S_PC_NODE_DISK_INFO diskinfo;
	m_vPcNodeDiskInfo.clear();
    while (m_pDbApi->FetchNext())
    {
        bzero(&diskinfo, sizeof(S_PC_NODE_DISK_INFO));
		diskinfo.pc_node_id = m_pDbApi->Field(0).asLong();
		strcpy(diskinfo.diskname,m_pDbApi->Field(1).asString());
		diskinfo.total = m_pDbApi->Field(2).asLong();
		diskinfo.used = m_pDbApi->Field(3).asLong();
        strcpy(diskinfo.pc_name, m_pDbApi->Field(4).asString());
		diskinfo.warn_disk = m_pDbApi->Field(5).asLong();
        m_vPcNodeDiskInfo.push_back(diskinfo);
    }
	UpdatePCpuMemDisk();
}
void CMonitorClientDlg::UpdatePCpuMemDisk()
{
    m_lstCpuRam.DeleteAllItems();
    int recNum;
    int nItem;
    char tmpchar[200];
    recNum = m_lstCpuRam.GetItemCount();
    bool iserror = false;
	unsigned int i=0;
	unsigned int j = 0;
	double dRate=0.0;
    for (i = 0; i < m_vPcNodeCpuRamInfo.size(); i++)
    {
        sprintf(tmpchar, "%d", m_vPcNodeCpuRamInfo[i].pc_node_id);
        nItem = m_lstCpuRam.InsertItem(recNum + i, tmpchar);
		sprintf(tmpchar, "%d", m_vPcNodeCpuRamInfo[i].cpurate);
		m_lstCpuRam.SetItemText(nItem, 1, tmpchar);
        if (m_vPcNodeCpuRamInfo[i].cpurate > m_vPcNodeCpuRamInfo[i].warn_cpu)
        {
            iserror = true;
        }
        sprintf(tmpchar, "%d", m_vPcNodeCpuRamInfo[i].totalram);
        m_lstCpuRam.SetItemText(nItem, 2, tmpchar);
        sprintf(tmpchar, "%d", m_vPcNodeCpuRamInfo[i].ramused);
        m_lstCpuRam.SetItemText(nItem, 3, tmpchar);
		dRate = 100.0 * m_vPcNodeCpuRamInfo[i].ramused / m_vPcNodeCpuRamInfo[i].totalram;
        sprintf(tmpchar, "%.2lf", dRate);
        m_lstCpuRam.SetItemText(nItem, 4, tmpchar);
		if (dRate > m_vPcNodeCpuRamInfo[i].warn_mem)
		{
			iserror = true;
		}
    }
	m_lstDisk.DeleteAllItems();
	recNum = m_lstDisk.GetItemCount();
    for (i = 0; i < m_vPcNodeDiskInfo.size(); i++)
    {
        sprintf(tmpchar, "%d", m_vPcNodeDiskInfo[i].pc_node_id);
        nItem = m_lstDisk.InsertItem(recNum + i, tmpchar);
        sprintf(tmpchar, "%s", m_vPcNodeDiskInfo[i].diskname);
		m_lstDisk.SetItemText(nItem, 1, tmpchar);
        sprintf(tmpchar, "%d", m_vPcNodeDiskInfo[i].total);
		m_lstDisk.SetItemText(nItem, 2, tmpchar);
        sprintf(tmpchar, "%d", m_vPcNodeDiskInfo[i].used);
		m_lstDisk.SetItemText(nItem, 3, tmpchar);
		dRate = 100.0 * m_vPcNodeDiskInfo[i].used / m_vPcNodeDiskInfo[i].total;
        sprintf(tmpchar, "%.2lf", dRate);
		m_lstDisk.SetItemText(nItem, 4, tmpchar);
        if (dRate > m_vPcNodeDiskInfo[i].warn_disk)
        {
            iserror = true;
        }

    }
    UpdateData(true);
    if (iserror)
    {
		m_picstatus_pc = false;
    }
	else
	{
		m_picstatus_pc = true;
	}
    autoDisplay();
}
void CMonitorClientDlg::UpdateAppNode()
{
	m_lstAppNode.DeleteAllItems();
    int recNum;
    int nItem;
    char tmpchar[200];
    recNum = m_lstAppNode.GetItemCount();
	bool iserror=false;
	
    for (unsigned int i = 0; i < m_vAppNodeInfo.size(); i++)
    {
        sprintf(tmpchar, "%d.%d", m_vAppNodeInfo[i].public_id, m_vAppNodeInfo[i].private_id);
        nItem = m_lstAppNode.InsertItem(recNum + i, tmpchar);
		m_lstAppNode.SetItemText(nItem, 1, m_vAppNodeInfo[i].app_name);
		if (m_vAppNodeInfo[i].app_type == 0)
		{
			sprintf(tmpchar, "总线");
		}
        else if (m_vAppNodeInfo[i].app_type == 1)
        {
            sprintf(tmpchar, "SAP");
        }
        else 
        {
            sprintf(tmpchar, "SRV");
        }
		m_lstAppNode.SetItemText(nItem, 2, tmpchar);
		sprintf(tmpchar,"%d-%s", m_vAppNodeInfo[i].pc_node_id, m_vAppNodeInfo[i].pc_name);
		m_lstAppNode.SetItemText(nItem, 3, tmpchar);
		sprintf(tmpchar, "%s %d", m_vAppNodeInfo[i].rpt_date, m_vAppNodeInfo[i].rpt_time);
		m_lstAppNode.SetItemText(nItem, 4, tmpchar);
		if (m_vAppNodeInfo[i].node_status == 0)
		{
			sprintf(tmpchar, "正常");
			COLORREF color = RGB(0,0,0);
			m_lstAppNode.SetItemText(nItem, 5, tmpchar);
			m_lstAppNode.SetItemColor(nItem,5, color);
		}
		else
		{
			sprintf(tmpchar, "%d - 异常", m_vAppNodeInfo[i].node_status);
			iserror = true;
            COLORREF color = RGB(255, 0, 0);
			m_lstAppNode.SetItemText(nItem, 5, tmpchar);
            m_lstAppNode.SetItemColor(nItem, 5, color);
		}
		
		m_lstAppNode.SetItemText(nItem, 6, m_vAppNodeInfo[i].start_time);

    }
    UpdateData(true);
	if (iserror)
	{
		m_picstatus_app = false;
	}
	else
	{
		m_picstatus_app = true;
	}
	autoDisplay();
}
void CMonitorClientDlg::UpdatePcNode()
{
    m_lstPcNode.DeleteAllItems();
    int recNum;
    int nItem;
    char tmpchar[200];
    recNum = m_lstPcNode.GetItemCount();
    bool iserror = false;
    for (unsigned int i = 0; i < m_vPcNodeInfo.size(); i++)
    {
        sprintf(tmpchar, "%d", m_vPcNodeInfo[i].pc_node_id);
        nItem = m_lstPcNode.InsertItem(recNum + i, tmpchar);
		m_lstPcNode.SetItemText(nItem, 1, m_vPcNodeInfo[i].pc_name);
        
		m_lstPcNode.SetItemText(nItem, 2, m_vPcNodeInfo[i].sip);
		m_lstPcNode.SetItemText(nItem, 3, m_vPcNodeInfo[i].area);
        sprintf(tmpchar, "%d", m_vPcNodeInfo[i].warn_mem);
		m_lstPcNode.SetItemText(nItem, 4, tmpchar);
		sprintf(tmpchar, "%d", m_vPcNodeInfo[i].warn_cpu);
		m_lstPcNode.SetItemText(nItem, 5, tmpchar);
		sprintf(tmpchar, "%d", m_vPcNodeInfo[i].warn_disk);
		m_lstPcNode.SetItemText(nItem, 6, tmpchar);

    }
    UpdateData(true);
}
void CMonitorClientDlg::autoDisplay()
{
	StopMusic();
    if (!m_picstatus_app)
    {
        HICON hIcontmp = AfxGetApp()->LoadIcon(IDI_ICON_ERR2);
        ((CStatic*)GetDlgItem(IDC_MONITOR_PIC))->SetIcon(hIcontmp);
		PlayMusic("err.wav");
    }
    else
    {
        HICON hIcontmp = AfxGetApp()->LoadIcon(IDI_ICON_OK1);
        ((CStatic*)GetDlgItem(IDC_MONITOR_PIC))->SetIcon(hIcontmp);
    }
    if (!m_picstatus_pc)
    {
        HICON hIcontmp = AfxGetApp()->LoadIcon(IDI_ICON_ERR1);
        ((CStatic*)GetDlgItem(IDC_MONITOR_PC))->SetIcon(hIcontmp);
		PlayMusic("err_pc.wav");
    }
    else
    {
        HICON hIcontmp = AfxGetApp()->LoadIcon(IDI_ICON_OK2);
        ((CStatic*)GetDlgItem(IDC_MONITOR_PC))->SetIcon(hIcontmp);
    }
}


void CMonitorClientDlg::OnBnClickedButtonSet()
{
	// TODO: 在此添加控件通知处理程序代码
	CSetDatabase dlg;
	int ret = dlg.DoModal();
    if (ret != IDOK)
    {
        return;
    }
}
bool CMonitorClientDlg::PlayMusic(const char* musicfile)
{
	if (m_bIsPlaying)
	{
		return m_bIsPlaying;
	}
	DWORD dwResult = 0;
	dwResult = mciSendCommand(0, MCI_OPEN, MCI_OPEN_ELEMENT, DWORD_PTR(&m_music_open));
	if (dwResult != 0)
	{
		m_statusbar.SetPaneText(1, "打开err.wav失败");
		UpdateData(TRUE);
	}
    MCI_PLAY_PARMS play; //定义播放结构体变量
	dwResult= mciSendCommand(m_music_open.wDeviceID, MCI_PLAY, MCI_NOTIFY, DWORD_PTR(&play));
    if (dwResult != 0)
    {
        m_statusbar.SetPaneText(1, "播放err.wav失败");
        UpdateData(TRUE);
    }
	m_bIsPlaying = true;
	return m_bIsPlaying;
}
void CMonitorClientDlg::StopMusic()
{
	if (m_bIsPlaying)
	{
        MCI_GENERIC_PARMS close;	
		DWORD dwResult = 0;
		dwResult = mciSendCommand(m_music_open.wDeviceID, MCI_CLOSE, 0, DWORD_PTR(&close));
        if (dwResult != 0)
        {
            m_statusbar.SetPaneText(1, "关闭err.wav播放失败");
            UpdateData(TRUE);
        }
		m_music_open.wDeviceID = 0;
	}
	m_bIsPlaying=false;
	return;
}
