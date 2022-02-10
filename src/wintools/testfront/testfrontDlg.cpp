// testfrontDlg.cpp : implementation file
//

#include "stdafx.h"
#include "testfront.h"
#include "testfrontDlg.h"
#include "ViewErrInfo.h"
#include "VarConfig.h"


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
// CTestfrontDlg dialog

CTestfrontDlg::CTestfrontDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestfrontDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTestfrontDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);


	for (int i=0;i<MAXTHREADNUM;i++)
	{
		m_pThread[i] = NULL;
	}
	m_curThreadNum= 0;
	m_time_auto = 1000;
	m_lastAvr = 0;
	m_curtime = 1;
	m_nMaxAvr = 0;
}

void CTestfrontDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestfrontDlg)
	DDX_Control(pDX, IDC_EDIT_MAXAVNUMSEC, m_edtMaxAvNumSec);
	DDX_Control(pDX, IDC_EDIT_AVR_SENDBYTE, m_edtAvrSendRate);
	DDX_Control(pDX, IDC_EDIT_AVR_RCVBYTE, m_edtAvrRcvRate);
	DDX_Control(pDX, IDC_EDIT_SENDBYTE, m_edtSendByte);
	DDX_Control(pDX, IDC_EDIT_RCVBYTE, m_edtRcvByte);
	DDX_Control(pDX, IDC_BUTTON_RESTAR, m_btnRestar);
	DDX_Control(pDX, IDC_EDIT_TRYNUM, m_edtTryNum);
	DDX_Control(pDX, IDC_CHECK_AUTO, m_chkAuto);
	DDX_Control(pDX, IDC_EDIT_DATETIME, m_edtDateTime);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDC_LIST_SC, m_lstScript);
	DDX_Control(pDX, IDC_IPADDRESS, m_ipctr);
	DDX_Control(pDX, IDC_EDIT_TIMESEP, m_edtTimeSep);
	DDX_Control(pDX, IDC_EDIT_TIMEOUT, m_edtTimeout);
	DDX_Control(pDX, IDC_EDIT_THREADNUM, m_edtThreadNum);
	DDX_Control(pDX, IDC_EDIT_SENDNUM, m_edtSendNum);
	DDX_Control(pDX, IDC_EDIT_SCFILE, m_edtFile);
	DDX_Control(pDX, IDC_EDIT_RUNTIME, m_edtRunTime);
	DDX_Control(pDX, IDC_EDIT_RCVNUM, m_edtRcvNum);
	DDX_Control(pDX, IDC_EDIT_PORT, m_edtPort);
	DDX_Control(pDX, IDC_EDIT_MINRESP, m_edtMinResp);
	DDX_Control(pDX, IDC_EDIT_MAXRESP, m_edtMaxResp);
	DDX_Control(pDX, IDC_EDIT_LEVEL, m_edtLevel);
	DDX_Control(pDX, IDC_EDIT_HEAD, m_edtHead);
	DDX_Control(pDX, IDC_EDIT_ERRNUM, m_edtErrNum);
	DDX_Control(pDX, IDC_EDIT_CURTIME, m_edtCurTime);
	DDX_Control(pDX, IDC_EDIT_CURNUMSEC, m_edtCurNumSec);
	DDX_Control(pDX, IDC_EDIT_BEGINTIME, m_edtBeginTime);
	DDX_Control(pDX, IDC_EDIT_AVRESP, m_edtAvResp);
	DDX_Control(pDX, IDC_EDIT_AVNUMSEC, m_edtAvNumSec);
	DDX_Control(pDX, IDC_EDIT_ALLTIME, m_edtAllTime);
	DDX_Control(pDX, IDC_EDIT_ALLTHREAD, m_edtAllThread);
	DDX_Control(pDX, IDC_COMBO_CONNECT, m_combConnect);
	DDX_Control(pDX, IDC_BUTTON_STOP, m_btnStop);
	DDX_Control(pDX, IDC_BUTTON_FILE, m_btnFile);
	DDX_Control(pDX, IDC_BUTTON_DEL, m_btnDel);
	DDX_Control(pDX, IDC_BUTTON_ADD, m_btnAdd);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTestfrontDlg, CDialog)
	//{{AFX_MSG_MAP(CTestfrontDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_FILE, OnButtonFile)
	ON_BN_CLICKED(IDC_BUTTON_STOP, OnButtonStop)
	ON_NOTIFY(NM_CLICK, IDC_LIST_SC, OnClickListSc)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_SC, OnDblclkListSc)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_RESTAR, OnButtonRestar)
	ON_BN_CLICKED(IDC_BUTTON_VIEW, OnButtonView)
	ON_BN_CLICKED(IDC_BUTTON_SETVAR, OnButtonSetvar)
	ON_BN_CLICKED(IDC_BUTTON_SAVELIST, OnButtonSavelist)
	ON_BN_CLICKED(IDC_BUTTON_OPENLIST, OnButtonOpenlist)
	ON_BN_CLICKED(IDC_BUTTON_SAVETEST, OnButtonSavetest)
	ON_BN_CLICKED(IDC_BUTTON_OPENTEST, OnButtonOpentest)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestfrontDlg message handlers

BOOL CTestfrontDlg::OnInitDialog()
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
	AfxInitRichEdit();
	
	if (!Init())
	{
		return false;
	}
// 	CTestThread dl;
// 	vector<_SVAROFFSET>res;
// 	dl.GetVarOffset("tt","dkajfjf<tt>afafsdfa<tt>ddd",26,res);


	m_log.SetLogPara(5,"","TestFront.log");
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestfrontDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTestfrontDlg::OnPaint() 
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
HCURSOR CTestfrontDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CTestfrontDlg::OnOK() 
{
	// TODO: Add extra validation here
	int sciNum = m_lstScript.GetItemCount();
	if (sciNum <1)
	{
		MessageBox("无脚本配置","提示");
		m_lstScript.SetFocus();
		return;
	}
	m_pShareData.Clear();
	//将参数保存至ShareData的SERVER_PARA结构中
	SaveParameter();
	

	//读取脚本给各线程
	FILE *fp=NULL;
	int i;
	char tmpchar[500];
	char filename[400];
	char sendData[8192];
	int sendDatalen;
	int conntype;
	int threadnum;
	int begintime;
	m_pShareData.m_nTotalThreadNum = 0;
	int itemCount = m_lstScript.GetItemCount();
	//释放原来的线程
	FreeThread();
	for ( i=0;i<itemCount;i++)
	{
		memset(tmpchar,0,sizeof(tmpchar));
		m_lstScript.GetItemText(i, 4, tmpchar, 300);
		m_pShareData.m_nTotalThreadNum+=atoi(tmpchar);
	}
	if (m_pShareData.m_nTotalThreadNum > MAXTHREADNUM)
	{
		sprintf(tmpchar,"总线程数大于规定的%d数目，请减小线程数目",MAXTHREADNUM);
		MessageBox(tmpchar,"提示");
		m_lstScript.SetFocus();
		return;
	}
	m_pShareData.m_bIsExit = false;
	m_pShareData.m_bIsRun = false;
	m_curThreadNum = 0;
	for ( i=0;i<itemCount;i++)
	{
		memset(filename,0,sizeof(filename));
		m_lstScript.GetItemText(i, 1, filename, 300);//文件名
		fp = fopen(filename,"rb");
		if (fp == NULL)
		{
			sprintf(tmpchar,"打开脚本文件[%s]失败",filename);
			MessageBox(tmpchar,"提示");
			m_lstScript.SetFocus();
			m_pShareData.m_bIsExit = true;
			return;
		}
		memset(sendData,0,sizeof(sendData));
		sendDatalen = fread(sendData,1,8191,fp);
		fclose(fp);

		memset(tmpchar,0,sizeof(tmpchar));
		m_lstScript.GetItemText(i, 2, tmpchar, 10);//连接类型
		conntype = atoi(tmpchar);
		memset(tmpchar,0,sizeof(tmpchar));
		m_lstScript.GetItemText(i, 3, tmpchar, 40);//开始时间
		begintime = atoi(tmpchar)*1000;
		memset(tmpchar,0,sizeof(tmpchar));
		m_lstScript.GetItemText(i, 4, tmpchar, 40);//线程数
		threadnum = atoi(tmpchar);
		for (int j=m_curThreadNum;j<m_curThreadNum+threadnum;j++)
		{
			m_pThread[j]= new CTestThread();
			if (m_pThread[j] == NULL)
			{
				sprintf(tmpchar,"资源不足");
				MessageBox(tmpchar,"提示");
				m_lstScript.SetFocus();
				m_pShareData.m_bIsExit = true;
				FreeThread();
				return;
			}
			m_pThread[j]->m_nID = j;
			m_pThread[j]->SetData(sendData,sendDatalen,&m_pShareData,begintime,conntype);

			if (m_pThread[j]->ThreadInit()<0)
			{
				MessageBox("ThreadInit失败");
				FreeThread();
				return ;
			}
			

		}
		m_curThreadNum +=threadnum;

	}
	m_publog.Update();
	sprintf(tmpchar,"log\\%s%s",m_publog.ToStringD8().c_str(),"TestFront.log");
	UNLINK(tmpchar);
	m_log.SetLogPara(m_pShareData.m_pServerPara.loglevel,"","TestFront.log");
	for (int jj=0;jj<m_curThreadNum;jj++)
	{
		m_pThread[jj]->CreateThread();//启动线程
	}
	m_pShareData.m_nTotalThreadNum = m_curThreadNum;
	m_pShareData.m_bIsRun = true;
	m_pShareData.m_nRunTime = m_publog.GetTickCount();
	m_lastAvr =0;
	m_nMaxAvr = 0;
	if (m_chkAuto.GetCheck()==1)//自动重启停止的线程
	{
		m_btnRestar.EnableWindow(false);
		m_timerID2 = SetTimer(ONTIMER2,2,NULL);//定时器
	}
	else
	{
		m_btnRestar.EnableWindow(true);
	}
	m_timerID1 = SetTimer(ONTIMER1,m_time_auto,NULL);//定时器
	if (m_pShareData.m_pServerPara.runtime != 0)
	{
		ConvSecTime(m_pShareData.m_pServerPara.runtime/1000,tmpchar);
	//sprintf(tmpchar,"%ld毫秒",m_pShareData.m_pServerPara.runtime);
	}
	else
	{
		sprintf(tmpchar,"未限时");
	}
	m_edtAllTime.SetWindowText(tmpchar);
	m_btnStop.EnableWindow(true);
	m_btnOk.EnableWindow(false);

	//CDialog::OnOK();
}
void CTestfrontDlg::ConvSecTime(int ssec,char *time)
{
	int hour;
	int minute;
	int sec;
	hour = (int)ssec/3600;
	minute = (int)(ssec - hour*3600)/60;
	sec = ssec - hour*3600 - minute*60;
	sprintf(time,"%02d:%02d:%02d",hour,minute,sec);
	return;
}
void CTestfrontDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	if (!m_pShareData.m_bIsExit)
	{
		int ret = MessageBox("线程正在运行，你确定退出？","确定",MB_YESNO);
		if (ret != IDYES)
		{
			return;
		}
	}




	m_log.LogMp(LOG_ERROR,__FILE__,__LINE__,"退出测试，去掉运行定时器");
	KillTimer(m_timerID2);
	m_pShareData.m_bIsExit = true;

	m_log.LogMp(LOG_ERROR,__FILE__,__LINE__,"退出测试，等待所有线程停止");
	//判断所有线程是否全停了
	int j;
	for (j=0;j<m_curThreadNum;j++)
	{
		if (m_pThread[j]!=NULL)
		{
			m_pThread[j]->Join();
		}
		
	}
	m_log.LogMp(LOG_ERROR,__FILE__,__LINE__,"退出测试，释放线程");
	SLEEP(1000);
	KillTimer(m_timerID1);
	FreeThread();
	m_log.LogMp(LOG_ERROR,__FILE__,__LINE__,"退出测试完成");

	CDialog::OnCancel();
}

void CTestfrontDlg::OnButtonAdd() 
{
	// TODO: Add your control notification handler code here
	CString filename;
	CString begintime;
	CString threadnum;
	
	if (m_edtFile.GetWindowTextLength()<1)
	{
		m_edtFile.SetFocus();
		MessageBox("请输入或选择脚本","提示",MB_OK);
		return ;
	}
	m_edtFile.GetWindowText(filename);
	//文件长度
	struct_stat f_stat;
	FSTAT(filename,&f_stat);
	int filelen=f_stat.st_size;//文件长度
	if (filelen >=8192)
	{
		m_edtFile.SetFocus();
		MessageBox("脚本文件不能超过8K","提示",MB_OK);
		return ;
	}
	if (filelen <1)
	{
		m_edtFile.SetFocus();
		MessageBox("文件不存在或为空文件","提示",MB_OK);
		return ;
	}
	if (m_edtBeginTime.GetWindowTextLength()<1)
	{
		m_edtBeginTime.SetFocus();
		MessageBox("请输入起始时间","提示",MB_OK);
		return ;
	}
	m_edtBeginTime.GetWindowText(begintime);
	if (m_edtThreadNum.GetWindowTextLength()<1)
	{
		m_edtThreadNum.SetFocus();
		MessageBox("请输入线程数","提示",MB_OK);
		return ;
	}
	m_edtThreadNum.GetWindowText(threadnum);
	
	int recNum = m_lstScript.GetItemCount();
	char tmpchar[40];
	sprintf(tmpchar,"%d",recNum+1);
	int nItem = m_lstScript.InsertItem(recNum,tmpchar);

	m_lstScript.SetItemText(nItem,1,filename);
	m_lstScript.SetItemText(nItem,2,m_combConnect.GetCurColumnItem(0));
	m_lstScript.SetItemText(nItem,3,begintime);
	m_lstScript.SetItemText(nItem,4,threadnum);
}

struct MyStruct 
{
	int ID;
	bool operator < (MyStruct item)  /*注意重载〈*/
    {
        if(ID<item.ID) //倒序
		{
            return false;
		}
		else
		{
			return true;
		}
    };
};

void CTestfrontDlg::OnButtonDel() 
{
	// TODO: Add your control notification handler code here
	CString ss;
	int ret;
	int nItem;
	char tmpchar[50];
	char sSql[500];
	int funcNo;
	ret = m_lstScript.GetSelectedCount();
	if   (ret < 1)  
	{
        MessageBox("请选中至少一条记录","提示");
		m_lstScript.SetFocus();
		return;
	}
	std::list<MyStruct>list_iItem;
	POSITION pos = m_lstScript.GetFirstSelectedItemPosition();
	if   (pos   ==   NULL)  
	{
		MessageBox("请选中至少一条记录","提示");
		m_lstScript.SetFocus();
		return;
	}
	else  
	{  
		while   (pos)  
		{  
			nItem   =   m_lstScript.GetNextSelectedItem(pos);
			MyStruct d;
			d.ID = nItem;
			list_iItem.push_back(d);
			
		}
		std::list<MyStruct>::iterator it;
		list_iItem.sort();
		int aa;
		for(it =list_iItem.begin();it !=list_iItem.end();it++)
		{
			m_lstScript.DeleteItem(it->ID);
		}
	}
}

void CTestfrontDlg::OnButtonFile() 
{
	// TODO: Add your control notification handler code here
	char filename[400];
	memset(filename,0,sizeof(filename));
	WORKPATH(filename);
	CString ss;
	int ret;

	CFileDialog   dlg(true,"","*.*",OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"ALL Files (*.*)|",NULL);
	ret = dlg.DoModal();
	ss = dlg.GetPathName();

	if (ret !=IDOK)
	{
		return;
	}
	
	m_edtFile.SetWindowText(ss);
	//将工作路径设置回去
	SetCurrentDirectory(filename);
	UpdateData(false);
	m_edtThreadNum.SetFocus();
}

void CTestfrontDlg::OnButtonStop() 
{
	// TODO: Add your control notification handler code here

//	StopThread();

	m_log.LogMp(LOG_ERROR,__FILE__,__LINE__,"停止测试，去掉运行定时器");
	KillTimer(m_timerID2);
	m_pShareData.m_bIsExit = true;
	m_pShareData.m_bIsRun =false;
	KillTimer(m_timerID1);
	m_log.LogMp(LOG_ERROR,__FILE__,__LINE__,"停止测试，等待所有线程停止");
    //判断所有线程是否全停掉
	int j;
	for (j=0;j<m_curThreadNum;j++)
	{
		m_pThread[j]->Join();
	}
	m_btnOk.EnableWindow(true);
	m_btnStop.EnableWindow(false);
	SLEEP(1000);
	autoMonitor();
	
	m_log.LogMp(LOG_ERROR,__FILE__,__LINE__,"停止测试，准备保存初始变量");
	//保存初始变量
	SaveVarInitData();
	m_log.LogMp(LOG_ERROR,__FILE__,__LINE__,"停止测试，释放线程");
	//释放线程
	FreeThread();
	m_log.LogMp(LOG_ERROR,__FILE__,__LINE__,"停止测试完成");

		
}

void CTestfrontDlg::OnClickListSc(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CTestfrontDlg::OnDblclkListSc(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

bool CTestfrontDlg::Init()
{
	m_combConnect.ResetContent();
	m_combConnect.FormatComboBox();
	m_combConnect.SetColumnWidth(40, 40);
	m_combConnect.SetColumnAlignStyle(DT_LEFT, DT_LEFT);	
	m_combConnect.AddRow("0","短连接");
	m_combConnect.AddRow("1","长连接");
	m_combConnect.SetCurSel(0);
	
	m_edtPort.SetLimitText(6);
	m_edtHead.SetLimitText(2);
	m_edtTimeout.SetLimitText(6);
	m_edtTimeSep.SetLimitText(6);
	m_edtLevel.SetLimitText(2);
	m_edtRunTime.SetLimitText(4);
	m_edtBeginTime.SetLimitText(4);
	m_edtThreadNum.SetLimitText(5);	
	m_edtTryNum.SetLimitText(3);

	LoadCfg();
	m_edtBeginTime.SetWindowText("0");
	m_edtThreadNum.SetWindowText("1");


	m_lstScript.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_HEADERDRAGDROP);
	m_lstScript.InsertColumn(0, "序号", LVCFMT_CENTER, 50);
	m_lstScript.InsertColumn(1, "脚本文件", LVCFMT_CENTER, 420);
	m_lstScript.InsertColumn(2, "连接类型", LVCFMT_CENTER, 100);
	m_lstScript.InsertColumn(3, "起始时间", LVCFMT_CENTER, 100);
	m_lstScript.InsertColumn(4, "线程数", LVCFMT_CENTER, 100);

	m_btnStop.EnableWindow(false);
	m_bisMax = false;
	for (int jj=0;jj<MAXTHREADNUM;jj++)
	{
		m_pThread[jj] = NULL;//启动线程
	}
	return true;
}

bool CTestfrontDlg::SaveParameter()
{
	CString ss;
	m_ipctr.GetWindowText(ss);
	
	SERVER_PARA para;
	memset(&para,0,sizeof(SERVER_PARA));
	strcpy(para.sip,ss.GetBuffer(ss.GetLength()));
	::WritePrivateProfileString("COMMON", "ADDR", ss, ".\\TestFront.dat");

	m_edtPort.GetWindowText(ss);
	::WritePrivateProfileString("COMMON", "PORT",ss, ".\\TestFront.dat");
	para.port = atoi(ss);

	m_edtTimeout.GetWindowText(ss);
	::WritePrivateProfileString("COMMON", "TIMEOUT", ss, ".\\TestFront.dat");

	para.timeout = atoi(ss);
	m_edtHead.GetWindowText(ss);
	::WritePrivateProfileString("COMMON", "HEADLEN", ss,".\\TestFront.dat");

	para.headlen = atoi(ss);
	m_edtTimeSep.GetWindowText(ss);
	::WritePrivateProfileString("COMMON", "TIMESEP", ss, ".\\TestFront.dat");
	para.timesep = atoi(ss);
	m_edtLevel.GetWindowText(ss);
	::WritePrivateProfileString("COMMON", "LOGLEVEL",ss, ".\\TestFront.dat");

	para.loglevel = atoi(ss);
	 
	m_edtRunTime.GetWindowText(ss);
	::WritePrivateProfileString("COMMON", "RUNTIME", ss, ".\\TestFront.dat");
	para.runtime = atoi(ss)*60000;

	m_edtTryNum.GetWindowText(ss);
	::WritePrivateProfileString("COMMON", "TRYNUM", ss, ".\\TestFront.dat");
	m_pShareData.m_nConnNum = atoi(ss);

	if (m_chkAuto.GetCheck() == 1)
	{
		::WritePrivateProfileString("COMMON", "AUTORESTAR", "1", ".\\TestFront.dat");
	}
	else
	{
		::WritePrivateProfileString("COMMON", "AUTORESTAR", "0", ".\\TestFront.dat");
	}
	memcpy(&m_pShareData.m_pServerPara,&para,sizeof(SERVER_PARA));

	m_pShareData.InitVar("vardefine.xml");
	return true;
}

void CTestfrontDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	OnCancel();
	//CDialog::OnClose();
}
#if _MSC_VER > 1200
void CTestfrontDlg::OnTimer(UINT_PTR nIDEvent) 
#else
void CTestfrontDlg::OnTimer(UINT nIDEvent) 
#endif
{
	// TODO: Add your message handler code here and/or call default
	switch (nIDEvent)
	{
		case ONTIMER1:
			autoMonitor();
			break;
		case ONTIMER2:
			OnButtonRestar();
			break;
		case ONTIMER3:
			IsStopAll();
			break;
		default:
			break;
	}
	CDialog::OnTimer(nIDEvent);
}

void CTestfrontDlg::autoMonitor()
{
	char tmpchar[40];
	m_publog.Update();
	sprintf(tmpchar,"%s %s",m_publog.ToStringD10().c_str(),m_publog.ToStringT8().c_str());
	m_edtDateTime.SetWindowText(tmpchar);

	int curtime = m_publog.GetTickCount()-m_pShareData.m_nRunTime;
	ConvSecTime(curtime/1000,tmpchar);
	//sprintf(tmpchar,"%ld毫秒",curtime);
	m_edtCurTime.SetWindowText(tmpchar);//当前时间

	sprintf(tmpchar,"%ld",m_pShareData.m_nTotalThreadNum);
	m_edtAllThread.SetWindowText(tmpchar);//总线程数
	sprintf(tmpchar,"%ld",m_pShareData.m_nSendNum);
	m_edtSendNum.SetWindowText(tmpchar);//发送数
	sprintf(tmpchar,"%ld",m_pShareData.m_nRcvNum);
	m_edtRcvNum.SetWindowText(tmpchar);//接收数

	sprintf(tmpchar,"%ld",m_pShareData.m_nErrNum);
	m_edtErrNum.SetWindowText(tmpchar);//出错数

	sprintf(tmpchar,"%ld毫秒",m_pShareData.m_nMinRespTime);
	m_edtMinResp.SetWindowText(tmpchar);//最小响应时间

	sprintf(tmpchar,"%ld毫秒",m_pShareData.m_nMaxRespTime);
	m_edtMaxResp.SetWindowText(tmpchar);//最大响应时间

	sprintf(tmpchar,"%I64d",m_pShareData.m_nSendByte);
	m_edtSendByte.SetWindowText(tmpchar);//发送字节数

	sprintf(tmpchar,"%I64d",m_pShareData.m_nRcvByte);
	m_edtRcvByte.SetWindowText(tmpchar);//接收字节数

	if (m_pShareData.m_nRcvNum>0)
	{
		sprintf(tmpchar,"%.2lf毫秒",1.00*m_pShareData.m_nTotalRespTIme/m_pShareData.m_nRcvNum);
		m_edtAvResp.SetWindowText(tmpchar);//平均响应时间
	}
	
	if (curtime>0)
	{
		sprintf(tmpchar,"%.2lf字节/秒",1000.00*m_pShareData.m_nSendByte/curtime);
		m_edtAvrSendRate.SetWindowText(tmpchar);//平均发送速率
		sprintf(tmpchar,"%.2lf字节/秒",1000.00*m_pShareData.m_nRcvByte/curtime);
		m_edtAvrRcvRate.SetWindowText(tmpchar);//平均收速率
	}

	

	int curavr= m_pShareData.m_nRcvNum-m_lastAvr;
	if (m_bisMax)
	{
		if (curavr > m_nMaxAvr)
		{
			m_nMaxAvr = curavr;
		}
	}
	m_bisMax = true;
	sprintf(tmpchar,"%d",m_nMaxAvr);
	m_edtMaxAvNumSec.SetWindowText(tmpchar);//峰值每秒笔数
	sprintf(tmpchar,"%d",curavr);
	m_edtCurNumSec.SetWindowText(tmpchar);//当前每秒笔数
	m_curtime++;
	m_lastAvr = m_pShareData.m_nRcvNum;

	int tt = ((m_publog.GetTickCount()-m_pShareData.m_nRunTime)/1000);
	if (tt !=0)
	{
		sprintf(tmpchar,"%.4lf",1.00*m_pShareData.m_nRcvNum/tt);
		m_edtAvNumSec.SetWindowText(tmpchar);//平均每秒笔数
	}
	
	if (!m_pShareData.m_bIsExit && m_pShareData.m_bIsRun) //若正在运行测试则判断是否要退出
	{
		if (m_pShareData.m_nTotalThreadNum<1)
		{
			GetDlgItem(IDC_BUTTON_STOP)->PostMessage(BM_CLICK);
			//OnButtonStop();
		}
		if (m_pShareData.m_pServerPara.runtime>0)
		{
			if (m_publog.GetTickCount()- m_pShareData.m_nRunTime > m_pShareData.m_pServerPara.runtime)
			{
				GetDlgItem(IDC_BUTTON_STOP)->PostMessage(BM_CLICK);
				//OnButtonStop();
			}
		}
	}
	
	
}

void CTestfrontDlg::Clear()
{
	for (int j=0;j<MAXTHREADNUM;j++)
	{
		if (m_pThread[j]!=NULL)
		{
			delete m_pThread[j];
			m_pThread[j] = NULL;
		}
	}
}

void CTestfrontDlg::OnButtonRestar() 
{
	// TODO: Add your control notification handler code here
	for (int j=0;j<m_curThreadNum;j++)
	{
		if (m_pThread[j]!=NULL)
		{
			if (m_pThread[j]->IsStoped())
			{
				m_pShareData.m_nTotalThreadNum++;
				m_pThread[j]->CreateThread();
			}
		}
	}
}

void CTestfrontDlg::LoadCfg()
{
	m_ipctr.SetWindowText("198.88.88.203");
	m_edtPort.SetWindowText("9001");
	m_edtHead.SetWindowText("5");
	m_edtTimeout.SetWindowText("60000");
	m_edtTimeSep.SetWindowText("0");
	m_edtLevel.SetWindowText("4");
	m_edtRunTime.SetWindowText("5");
	char szTmp[100];
	
	::GetPrivateProfileString("COMMON", "ADDR", "198.88.88.203", szTmp, 29, ".\\TestFront.dat");
	m_ipctr.SetWindowText(szTmp);
	
	::GetPrivateProfileString("COMMON", "PORT", "9001", szTmp, 7, ".\\TestFront.dat");
	m_edtPort.SetWindowText(szTmp);

	
	::GetPrivateProfileString("COMMON", "TIMEOUT", "60000", szTmp, 8, ".\\TestFront.dat");
	m_edtTimeout.SetWindowText(szTmp);
	
	::GetPrivateProfileString("COMMON", "HEADLEN", "5", szTmp, 3, ".\\TestFront.dat");
	m_edtHead.SetWindowText(szTmp);
	
	::GetPrivateProfileString("COMMON", "LOGLEVEL", "4", szTmp, 3, ".\\TestFront.dat");
	m_edtLevel.SetWindowText(szTmp);
	
	::GetPrivateProfileString("COMMON", "TIMESEP", "0", szTmp, 8, ".\\TestFront.dat");
	m_edtTimeSep.SetWindowText(szTmp);
	
	::GetPrivateProfileString("COMMON", "RUNTIME", "5", szTmp, 5, ".\\TestFront.dat");
	m_edtRunTime.SetWindowText(szTmp);

	::GetPrivateProfileString("COMMON", "TRYNUM", "5", szTmp, 4, ".\\TestFront.dat");
	m_edtTryNum.SetWindowText(szTmp);

	::GetPrivateProfileString("COMMON", "AUTORESTAR", "0", szTmp, 4, ".\\TestFront.dat");
	if (atoi(szTmp) == 1)
	{
		m_chkAuto.SetCheck(1);
		m_btnRestar.EnableWindow(false);
	}
	else
	{
		m_chkAuto.SetCheck(0);
		m_btnRestar.EnableWindow(true);
	}

}

void CTestfrontDlg::OnButtonView() 
{
	// TODO: Add your control notification handler code here
	CViewErrInfo dlg;
	dlg.DoModal();
}

void CTestfrontDlg::OnButtonSetvar() 
{
	// TODO: Add your control notification handler code here
	CVarConfig dlg;
	dlg.DoModal();
}

void CTestfrontDlg::OnButtonSavelist() 
{
	// TODO: Add your control notification handler code here
	int i;
	char tmpchar[500];
	FILE *fp;
	char filename[300];
	char savefilename[400];
	int  conntype;
	int  begintime;
	int  threadnum;
	int itemCount = m_lstScript.GetItemCount();
	CXmlPack xmlpack;
	CString ss;
	int ret;
	SaveParameter();
	if (itemCount>0)
	{
		
		memset(savefilename,0,sizeof(savefilename));
		WORKPATH(savefilename);
		
		
		CFileDialog   dlg(false,"","*.tfl",OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"ALL Files (*.*)|",NULL);
		ret = dlg.DoModal();
		ss = dlg.GetPathName();
		
		if (ret !=IDOK)
		{
			return;
		}
		//将工作路径设置回去
		SetCurrentDirectory(savefilename);
		strcpy(savefilename,ss.GetBuffer(ss.GetLength()));
	}
	else
	{
		return;
	}
	xmlpack.gotoPack();
	for ( i=0;i<itemCount;i++)
	{
		memset(filename,0,sizeof(filename));
		m_lstScript.GetItemText(i, 1, filename, 300);//文件名
		fp = fopen(filename,"rb");
		if (fp == NULL)
		{
			sprintf(tmpchar,"打开脚本文件[%s]失败",filename);
			MessageBox(tmpchar,"提示");
			m_lstScript.SetFocus();
			m_pShareData.m_bIsExit = true;
			return;
		}
		memset(tmpchar,0,sizeof(tmpchar));
		m_lstScript.GetItemText(i, 2, tmpchar, 10);//连接类型
		conntype = atoi(tmpchar);
		memset(tmpchar,0,sizeof(tmpchar));
		m_lstScript.GetItemText(i, 3, tmpchar, 40);//开始时间
		begintime = atoi(tmpchar)*1000;
		memset(tmpchar,0,sizeof(tmpchar));
		m_lstScript.GetItemText(i, 4, tmpchar, 40);//线程数
		threadnum = atoi(tmpchar);

		
		CNode *addnode = xmlpack.m_xml.addItem("LIST","");
		if (addnode!= NULL)
		{
			addnode->SetAttribute("文件名",filename,false);
			addnode->SetAttribute("连接类型",conntype);
			addnode->SetAttribute("开始时间",begintime);
			addnode->SetAttribute("线程数",threadnum);
		}
	}
	xmlpack.toFile(savefilename);
}

void CTestfrontDlg::OnButtonOpenlist() 
{
	// TODO: Add your control notification handler code here
	int i;
	char tmpchar[500];
	FILE *fp;
	char filename[300];
	char savefilename[400];
	int  conntype;
	int  begintime;
	int  threadnum;

	CXmlPack xmlpack;
	CString ss;
	int ret;
	
	memset(savefilename,0,sizeof(savefilename));
	WORKPATH(savefilename);
	
	
	CFileDialog   dlg(true,"","*.tfl",OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"ALL Files (*.*)|",NULL);
	ret = dlg.DoModal();
	ss = dlg.GetPathName();
	
	if (ret !=IDOK)
	{
		return;
	}
	//将工作路径设置回去
	SetCurrentDirectory(savefilename);
	strcpy(savefilename,ss.GetBuffer(ss.GetLength()));
	m_lstScript.DeleteAllItems();

	if (xmlpack.fromFile(savefilename)<0)
	{
		MessageBox("文件非法","提示");
		return;
	}
	xmlpack.gotoPack();
	int recNum;
	int nItem ;
	string sss;
	CNode *tmpnode = xmlpack.m_xml.getFirstChildItem();
	while (tmpnode != NULL)
	{
		recNum = m_lstScript.GetItemCount();
		
		sprintf(tmpchar,"%d",recNum+1);
		nItem = m_lstScript.InsertItem(recNum,tmpchar);
		tmpnode->GetAttributeValue("文件名",sss);
		m_lstScript.SetItemText(nItem,1,sss.c_str());
		tmpnode->GetAttributeValue("连接类型",sss);
		m_lstScript.SetItemText(nItem,2,sss.c_str());
		tmpnode->GetAttributeValue("开始时间",sss);
		m_lstScript.SetItemText(nItem,3,sss.c_str());
		tmpnode->GetAttributeValue("线程数",sss);
		m_lstScript.SetItemText(nItem,4,sss.c_str());
		tmpnode = tmpnode->getNextSibling();
	}
}

void CTestfrontDlg::OnButtonSavetest() 
{
	// TODO: Add your control notification handler code here
	int i;
	char tmpchar[500];
	FILE *fp;
	char filename[300];
	char savefilename[400];

	CXmlPack xmlpack;
	CString ss;
	int ret;
	
	memset(savefilename,0,sizeof(savefilename));
	WORKPATH(savefilename);
	
	
	CFileDialog   dlg(false,"","*.trs",OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"ALL Files (*.*)|",NULL);
	ret = dlg.DoModal();
	ss = dlg.GetPathName();
	
	if (ret !=IDOK)
	{
		return;
	}
	//将工作路径设置回去
	SetCurrentDirectory(savefilename);
	strcpy(savefilename,ss.GetBuffer(ss.GetLength()));

	xmlpack.gotoPack();
	CNode *curnode= xmlpack.m_xml.getCurrentNode();
	m_edtDateTime.GetWindowText(ss);//测试时间
	curnode->SetAttribute("测试时间",ss.GetBuffer(ss.GetLength()));

	m_ipctr.GetWindowText(ss);//服务器IP
	xmlpack.setPackNameValue("服务器IP",ss.GetBuffer(ss.GetLength()));

	m_edtPort.GetWindowText(ss);//服务器端口
	xmlpack.setPackNameValue("服务器端口",ss.GetBuffer(ss.GetLength()));

	m_edtTimeout.GetWindowText(ss);//接收超时(毫秒)
	xmlpack.setPackNameValue("接收超时(毫秒)",ss.GetBuffer(ss.GetLength()));

	m_edtTimeSep.GetWindowText(ss);//发送间隔
	xmlpack.setPackNameValue("发送间隔(毫秒)",ss.GetBuffer(ss.GetLength()));


	m_edtRunTime.GetWindowText(ss);//运行时间
	xmlpack.setPackNameValue("运行时间(分)",ss.GetBuffer(ss.GetLength()));


	m_edtAllThread.GetWindowText(ss);//总线程数
	xmlpack.setPackNameValue("总线程数",ss.GetBuffer(ss.GetLength()));

	m_edtSendNum.GetWindowText(ss);//发送数
	xmlpack.setPackNameValue("发送数",ss.GetBuffer(ss.GetLength()));
	
	m_edtRcvNum.GetWindowText(ss);//发送数
	xmlpack.setPackNameValue("接收数",ss.GetBuffer(ss.GetLength()));

	m_edtErrNum.GetWindowText(ss);//出错数
	xmlpack.setPackNameValue("出错数",ss.GetBuffer(ss.GetLength()));
	
	m_edtMinResp.GetWindowText(ss);//最小响应时间
	xmlpack.setPackNameValue("最小响应时间",ss.GetBuffer(ss.GetLength()));

	m_edtMaxResp.GetWindowText(ss);//最大响应时间
	xmlpack.setPackNameValue("最大响应时间",ss.GetBuffer(ss.GetLength()));
	
	m_edtSendByte.GetWindowText(ss);//发送字节数
	xmlpack.setPackNameValue("发送字节数",ss.GetBuffer(ss.GetLength()));

	m_edtRcvByte.GetWindowText(ss);//接收字节数
	xmlpack.setPackNameValue("接收字节数",ss.GetBuffer(ss.GetLength()));
	
	m_edtAvResp.GetWindowText(ss);//平均响应时间
	xmlpack.setPackNameValue("平均响应时间",ss.GetBuffer(ss.GetLength()));
	
	
	m_edtAvrSendRate.GetWindowText(ss);//平均发送速率
	xmlpack.setPackNameValue("平均发送速率",ss.GetBuffer(ss.GetLength()));


	m_edtAvrRcvRate.GetWindowText(ss);//平均收速率
	xmlpack.setPackNameValue("平均收速率",ss.GetBuffer(ss.GetLength()));

	m_edtMaxAvNumSec.GetWindowText(ss);//峰值每秒笔数
	xmlpack.setPackNameValue("峰值每秒笔数",ss.GetBuffer(ss.GetLength()));


	m_edtCurNumSec.GetWindowText(ss);//当前每秒笔数
	xmlpack.setPackNameValue("当前每秒笔数",ss.GetBuffer(ss.GetLength()));

	m_edtAvNumSec.GetWindowText(ss);//平均每秒笔数
	xmlpack.setPackNameValue("平均每秒笔数",ss.GetBuffer(ss.GetLength()));

	

	xmlpack.toFile(savefilename);

}

void CTestfrontDlg::OnButtonOpentest() 
{
	// TODO: Add your control notification handler code here
	int i;
	char tmpchar[500];
	FILE *fp;
	char filename[300];
	char savefilename[400];
	
	CXmlPack xmlpack;
	CString ss;
	int ret;
	
	memset(savefilename,0,sizeof(savefilename));
	WORKPATH(savefilename);
	
	
	CFileDialog   dlg(true,"","*.trs",OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"ALL Files (*.*)|",NULL);
	ret = dlg.DoModal();
	ss = dlg.GetPathName();
	
	if (ret !=IDOK)
	{
		return;
	}
	//将工作路径设置回去
	SetCurrentDirectory(savefilename);
	strcpy(savefilename,ss.GetBuffer(ss.GetLength()));
	if (xmlpack.fromFile(savefilename)<0)
	{
		MessageBox("文件格式非法");
		return;
	}

	xmlpack.gotoPack();

	m_edtDateTime.SetWindowText(xmlpack.getPackValueS("测试时间").c_str());

	m_ipctr.SetWindowText(xmlpack.getPackValueS("服务器IP").c_str());

	m_edtPort.SetWindowText(xmlpack.getPackValueS("服务器端口").c_str());

	m_edtTimeout.SetWindowText(xmlpack.getPackValueS("接收超时(毫秒)").c_str());
	
	m_edtTimeSep.SetWindowText(xmlpack.getPackValueS("发送间隔(毫秒)").c_str());

	m_edtRunTime.SetWindowText(xmlpack.getPackValueS("运行时间(分)").c_str());
	m_edtAllTime.SetWindowText(xmlpack.getPackValueS("运行时间(分)").c_str());
	
	m_edtAllThread.SetWindowText(xmlpack.getPackValueS("总线程数").c_str());
	
	m_edtSendNum.SetWindowText(xmlpack.getPackValueS("发送数").c_str());

	m_edtRcvNum.SetWindowText(xmlpack.getPackValueS("接收数").c_str());

	m_edtErrNum.SetWindowText(xmlpack.getPackValueS("出错数").c_str());
	
	m_edtMinResp.SetWindowText(xmlpack.getPackValueS("最小响应时间").c_str());

	m_edtMaxResp.SetWindowText(xmlpack.getPackValueS("最大响应时间").c_str());
	
	m_edtSendByte.SetWindowText(xmlpack.getPackValueS("发送字节数").c_str());

	m_edtRcvByte.SetWindowText(xmlpack.getPackValueS("接收字节数").c_str());
	
	m_edtAvResp.SetWindowText(xmlpack.getPackValueS("平均响应时间").c_str());
	
	m_edtAvrSendRate.SetWindowText(xmlpack.getPackValueS("平均发送速率").c_str());


	m_edtAvrRcvRate.SetWindowText(xmlpack.getPackValueS("平均收速率").c_str());
	
	m_edtMaxAvNumSec.SetWindowText(xmlpack.getPackValueS("峰值每秒笔数").c_str());
	
	m_edtCurNumSec.SetWindowText(xmlpack.getPackValueS("当前每秒笔数").c_str());
	
	m_edtAvNumSec.SetWindowText(xmlpack.getPackValueS("平均每秒笔数").c_str());
	
	
}

void CTestfrontDlg::FreeThread()
{
	if (m_pShareData.m_nTotalThreadNum>0)
	{
		for (int jjj=0;jjj<m_pShareData.m_nTotalThreadNum;jjj++)
		{
			if (m_pThread[jjj]!=NULL)
			{
				delete m_pThread[jjj] ;
				m_pThread[jjj] = NULL;
			}
		}
	}
}

void CTestfrontDlg::SaveVarInitData()
{
	CBF_PMutex pp(&m_mutex);
	CXmlPack xmlpack;
	xmlpack.fromFile("vardefine.xml");
	xmlpack.gotoHead();
	char tmpchar[40];
	//将变量的初始值置回
	for (int i=0;i<m_pShareData.m_varList.size();i++)
	{
		if (m_pShareData.m_varList[i]->GetVarType()==0)	
		{
			CNode *tmpnode = xmlpack.m_xml.getFirstChildItem(m_pShareData.m_varList[i]->GetVarName());
			if (tmpnode!=NULL)
			{
				sprintf(tmpchar,"%I64d",m_pShareData.m_varList[i]->GetVarValue());
				tmpnode->SetAttribute("initvalue",tmpchar);
			}
		}
		
	}
	xmlpack.toFile("vardefine.xml");
}

void CTestfrontDlg::StopThread()
{
	KillTimer(m_timerID2);
	m_pShareData.m_bIsExit = true;
	//启动定时器才判断线程是否全停掉
	m_timerID3 = SetTimer(ONTIMER3,m_time_auto,NULL);//定时器
}

void CTestfrontDlg::IsStopAll()
{
	//判断所有线程是否全停掉
	int j;
	int num=0;
	for (j=0;j<m_curThreadNum;j++)
	{
		if (m_pThread[j]->IsStoped())
		{
			num++;
		}
	}
	if (num<m_curThreadNum) //没有全停掉
	{
		return;
	}
	KillTimer(m_timerID3);
	m_btnOk.EnableWindow(true);
	m_btnStop.EnableWindow(false);
	SLEEP(1000);
	KillTimer(m_timerID1);
	
	//保存初始变量
	SaveVarInitData();
	//释放线程
	FreeThread();
}
