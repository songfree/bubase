// AppWizardDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AppWizard.h"
#include "AppWizardDlg.h"

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
// CAppWizardDlg dialog

CAppWizardDlg::CAppWizardDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAppWizardDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAppWizardDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAppWizardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAppWizardDlg)
	DDX_Control(pDX, IDC_COMBO_TYPE, m_combType);
	DDX_Control(pDX, IDC_EDIT_DLL_PATH, m_edtDPath);
	DDX_Control(pDX, IDC_EDIT_DLL_NAME, m_edtDName);
	DDX_Control(pDX, IDC_EDIT_CLASS_NAME, m_edtCName);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAppWizardDlg, CDialog)
	//{{AFX_MSG_MAP(CAppWizardDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_BROW, OnButtonBrow)
	ON_EN_KILLFOCUS(IDC_EDIT_DLL_NAME, OnKillfocusEditDllName)
	ON_EN_CHANGE(IDC_EDIT_DLL_NAME, OnChangeEditDllName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAppWizardDlg message handlers

BOOL CAppWizardDlg::OnInitDialog()
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
	char dllpath[500];
	memset(dllpath,0,sizeof(dllpath));
	GetCurrentDirectory(sizeof(dllpath)-1,dllpath);
	m_edtDPath.SetWindowText(dllpath);

	m_combType.ResetContent();
	m_combType.FormatComboBox(2);
	m_combType.SetColumnWidth(50,180);
	m_combType.SetColumnAlignStyle(DT_LEFT, DT_LEFT,DT_LEFT);
	m_combType.AddRow("1","业务原子动态库v5");
	m_combType.SetCurSel(1);
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAppWizardDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CAppWizardDlg::OnPaint() 
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
HCURSOR CAppWizardDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

int CAppWizardDlg::GetFmtBuffer(char *filename, char *fmtbuf,int &fmtlen)
{
	int ret;
	FILE *fp;
	char filepathname[500];
#ifdef WIN32
	fp = fopen(filename,"r");
#else
	sprintf(filepathname,"%s/fmt/%s",getenv("HOME"),filename);
	fp = fopen(filepathname,"r");
#endif
	if (NULL == fp)
	{
		strcpy(fmtbuf,"");
		return -1;
	}
	ret = fread(fmtbuf,1,8000,fp);
	fclose(fp);
	fmtbuf[ret]=0;
	fmtlen = ret;
	return ret;

}

void CAppWizardDlg::OnOK() 
{
	// TODO: Add extra validation here
	int ret;
	char dllname[100];
	CString ss;
	char dllpath[300];
	char filepathname[500];
	char hbuffer[8192];
	char cbuffer[8192];
	char wbuffer[8192];
	char dbuffer[200];
	char curdate[11];
	FILE *fp;
	memset(hbuffer,0,sizeof(hbuffer));
	memset(cbuffer,0,sizeof(cbuffer));
	memset(wbuffer,0,sizeof(wbuffer));
	memset(dllname,0,sizeof(dllname));
	memset(dllpath,0,sizeof(dllpath));
	memset(filepathname,0,sizeof(filepathname));
	memset(curdate,0,sizeof(curdate));
	if (m_edtDName.GetWindowTextLength() <1)
	{
		m_edtDName.SetFocus();
		MessageBox("动态库名称不能为空!");
		return ;
	}
	if (m_edtDName.GetWindowTextLength() > 20 )
	{
		m_edtDName.SetFocus();
		MessageBox("动态库名称太长!");
		return ;
	}
	m_edtDName.GetWindowText(ss);
	strcpy(dllname,ss.GetBuffer(ss.GetLength()));
	m_edtDPath.GetWindowText(ss);
	strcpy(dllpath,ss.GetBuffer(ss.GetLength()));
	trim(dllname);
	if (strlen(dllname)<1)
	{
		m_edtDName.SetFocus();
		MessageBox("动态库名称不能有空格!");
		return ;
	}
	ret = strlen(dllname);
	if (dllname[0] == '_')
	{
		m_edtDName.SetFocus();
		MessageBox("动态库名称只能为字母+下划线,下划线不能为第一个字母!");
		return ;
	}
	for (int i=0;i<ret;i++)
	{
		if (dllname[i]<65 )
		{
			m_edtDName.SetFocus();
			MessageBox("动态库名称只能为字母+下划线!");
			return ;
		}
		if (dllname[i]>90 && dllname[i]<97) 
		{
			m_edtDName.SetFocus();
			MessageBox("动态库名称只能为字母+下划线!");
			return ;
		}
		if (dllname[i]>122 ) 
		{
			m_edtDName.SetFocus();
			MessageBox("动态库名称只能为字母+下划线!");
			return ;
		}
	}
	
	//取模板文件
	int apptype = atoi(m_combType.GetCurColumnItem(0));
	if (apptype == 1)
	{
		ret = GetFmtBuffer(".\\dot\\templet.cpp",cbuffer,ret);
		if (ret<0)
		{
			MessageBox("模板不存在，请重新安装开发框架");
			return ;
		}
		ret = GetFmtBuffer(".\\dot\\templet.h",hbuffer,ret);
		if (ret<0)
		{
			MessageBox("模板不存在，请重新安装开发框架");
			return ;
		}
	}
	
	sprintf(wbuffer,"您确定要根据模板在%s下生成%s的动态库代码文件",dllpath,dllname);
	ret =MessageBox(wbuffer,"选择",MB_YESNO);
	if (ret!=IDYES)
	{
		return;
	}
	time_t t;
    time(&t);
	struct tm *tt;
	tt=localtime(&t);
	sprintf(curdate,"%4d%02d%02d",\
			1900+tt->tm_year,tt->tm_mon+1,tt->tm_mday);

	//生成cpp文件
	sprintf(filepathname,"%s\\%s.cpp",dllpath,dllname);
	if (apptype == 1)
	{
		sprintf(wbuffer,cbuffer,dllname,curdate,dllname,dllname,dllname,dllname,dllname,dllname,dllname,dllname,dllname,dllname,dllname);
	}
	else
	{
		sprintf(wbuffer,cbuffer,dllname,curdate,dllname,dllname,dllname,dllname,dllname,dllname,dllname,dllname,dllname,dllname);
	}
	
	fp = fopen(filepathname,"w");
	if (fp==NULL)
	{
		MessageBox("生成cpp文件失败");
		return;
	}
	fwrite(wbuffer,1,strlen(wbuffer),fp);
	fclose(fp);

	//生成h文件
	sprintf(filepathname,"%s\\%s.h",dllpath,dllname);


	sprintf(dbuffer,"BUBASE_AUTO_APPWIZARD_%s_%ld_INCLUDED_",curdate,GetTickCount());
	sprintf(wbuffer,hbuffer,dllname,curdate,dllname,dllname,dbuffer,dbuffer,dllname,dllname,dllname,dllname,dbuffer);

	fp = fopen(filepathname,"w");
	if (fp==NULL)
	{
		MessageBox("生成h文件失败");
		return;
	}
	fwrite(wbuffer,1,strlen(wbuffer),fp);
	fclose(fp);

	//生成makefile文件
	if (apptype == 1)
	{
		ret = GetFmtBuffer(".\\dot\\Makefile.dot",hbuffer,ret);
	}
	if (ret<0)
	{
		MessageBox("makefile模板不存在，请重新安装开发框架");
		return ;
	}
	sprintf(filepathname,"%s\\%s",dllpath,"Makefile");

	
	sprintf(wbuffer,hbuffer,dllname,dllname,dllname,dllname);
	fp = fopen(filepathname,"w");
	if (fp==NULL)
	{
		MessageBox("生成makefile文件失败");
		return;
	}
	fwrite(wbuffer,1,strlen(wbuffer),fp);
	fclose(fp);

	if (apptype == 1)
	{
		ret = GetFmtBuffer(".\\dot\\mkaix.dot",hbuffer,ret);
	}
	if (ret<0)
	{
		MessageBox("makefile-aix模板不存在，请重新安装开发框架");
		return ;
	}
	sprintf(filepathname,"%s\\%s",dllpath,"mk.aix");
	
	
	sprintf(wbuffer,hbuffer,dllname,dllname,dllname,dllname);
	fp = fopen(filepathname,"w");
	if (fp==NULL)
	{
		MessageBox("生成makefile文件失败");
		return;
	}
	fwrite(wbuffer,1,strlen(wbuffer),fp);
	fclose(fp);

	if (apptype == 1)
	{
		ret = GetFmtBuffer(".\\dot\\mkaix64.dot",hbuffer,ret);
	}
	if (ret<0)
	{
		MessageBox("makefile-aix64模板不存在，请重新安装开发框架");
		return ;
	}
	sprintf(filepathname,"%s\\%s",dllpath,"mk.aix64");
	
	
	sprintf(wbuffer,hbuffer,dllname,dllname,dllname,dllname);
	fp = fopen(filepathname,"w");
	if (fp==NULL)
	{
		MessageBox("生成makefile文件失败");
		return;
	}
	fwrite(wbuffer,1,strlen(wbuffer),fp);
	fclose(fp);

	if (apptype == 1)
	{
		ret = GetFmtBuffer(".\\dot\\mkhpgcc.dot",hbuffer,ret);
	}
	if (ret<0)
	{
		MessageBox("makefile-hpgcc模板不存在，请重新安装开发框架");
		return ;
	}
	sprintf(filepathname,"%s\\%s",dllpath,"mk.hpgcc");
	
	
	sprintf(wbuffer,hbuffer,dllname,dllname,dllname,dllname);
	fp = fopen(filepathname,"w");
	if (fp==NULL)
	{
		MessageBox("生成makefile文件失败");
		return;
	}
	fwrite(wbuffer,1,strlen(wbuffer),fp);
	fclose(fp);
	//生成vc的工程文件
	if (apptype == 1)
	{
		ret = GetFmtBuffer(".\\dot\\bumodulevcdsp.dot",hbuffer,ret);
	}
	if (ret<0)
	{
		MessageBox("vc工程模板不存在，请重新安装开发框架");
		return ;
	}
	sprintf(filepathname,"%s\\%s.dsp",dllpath,dllname);
	CString udllname;
	udllname = dllname;
	udllname.MakeUpper();
	
	sprintf(wbuffer,hbuffer,dllname,dllname,dllname,dllname,\
		dllname,dllname,dllname,dllname,udllname,dllname,udllname,dllname,dllname,dllname,dllname);
	fp = fopen(filepathname,"w");
	if (fp==NULL)
	{
		MessageBox("生成vc工程文件dsp失败");
		return;
	}
	fwrite(wbuffer,1,strlen(wbuffer),fp);
	fclose(fp);


	sprintf(filepathname,"在%s下生成%s.cpp、%s.h、Makefile及VC工程文件成功!",dllpath,dllname,dllname);
	MessageBox(filepathname);
	return;

	//CDialog::OnOK();
}

void CAppWizardDlg::OnButtonBrow() 
{
	// TODO: Add your control notification handler code here
	LPMALLOC pMalloc;
	if (::SHGetMalloc(&pMalloc) == NOERROR)
	{
		BROWSEINFO bi;
		char pszBuffer[MAX_PATH];
		char dllpath[500];
		memset(pszBuffer,0,sizeof(pszBuffer));
		memset(dllpath,0,sizeof(dllpath));
		GetCurrentDirectory(sizeof(dllpath)-1,dllpath);
		strcpy(pszBuffer,dllpath);

		LPITEMIDLIST pidl;
		bi.hwndOwner = GetSafeHwnd();
		bi.pidlRoot = NULL;
		bi.pszDisplayName = pszBuffer;
		bi.lpszTitle = _T("选择业务功能源代码目录");
		bi.ulFlags = BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;
		bi.lpfn = NULL;
		bi.lParam = 0;
		if ((pidl = ::SHBrowseForFolder(&bi)) != NULL)
		{
			if (::SHGetPathFromIDList(pidl, pszBuffer))
			{
				m_edtDPath.SetWindowText(pszBuffer);
			}
			pMalloc->Free(pidl);
		}
		pMalloc->Release();
	} 
}

void CAppWizardDlg::OnKillfocusEditDllName() 
{
	// TODO: Add your control notification handler code here
	CString ss;
	m_edtDName.GetWindowText(ss);
	m_edtCName.SetWindowText("C"+ss);
}

void CAppWizardDlg::OnChangeEditDllName() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString ss;
	m_edtDName.GetWindowText(ss);
	m_edtCName.SetWindowText("C"+ss);
}

int CAppWizardDlg::trim(char *s)
{

	int i;
	char tm[300],*str;
	str=s;
	for(i=0;*str!=0;str++)
	{
		if(*str!=' '&&*str!=10&&*str!=13)
		{
			tm[i++]=*str;
		}
	}
	tm[i]=0;
	strcpy(s,tm);
	return(0);
}
