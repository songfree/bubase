// RegBuPathDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RegBuPath.h"
#include "RegBuPathDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


const LPCTSTR szStartWithOpReg2 = _T("SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment");
#define WORKPATH(WorkDir) GetCurrentDirectory(sizeof(WorkDir),WorkDir);

/////////////////////////////////////////////////////////////////////////////
// CRegBuPathDlg dialog

CRegBuPathDlg::CRegBuPathDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRegBuPathDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRegBuPathDlg)
	m_csDir = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRegBuPathDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRegBuPathDlg)
	DDX_Control(pDX, IDC_EDIT_DIR, m_edtDir);
	DDX_Text(pDX, IDC_EDIT_DIR, m_csDir);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRegBuPathDlg, CDialog)
	//{{AFX_MSG_MAP(CRegBuPathDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_BROW, OnButtonBrow)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegBuPathDlg message handlers

BOOL CRegBuPathDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	char curPath[8192];
	
	
	memset(curPath,0,sizeof(curPath));
	WORKPATH(curPath);
	m_edtDir.SetWindowText(curPath);

// 	char szKeyValue[8192] ;
// 	char szKeyName[8192] ;
// 	char curPath[8192];
// 	
// 	char strText[8192];
// 	char szErrMsg[8192];
// 	memset(szKeyName,0,sizeof(szKeyName));
// 	memset(szKeyValue,0,sizeof(szKeyValue));
// 	memset(strText,0,sizeof(strText));
// 	memset(szErrMsg,0,sizeof(szErrMsg));
// 	memset(curPath,0,sizeof(curPath));
// 
// 	unsigned long lDataLen;
// 	unsigned long lType = 0;
// 	WORKPATH(curPath);
// 	sprintf(strText,"确定BUBASE框架路径:[%s] ?，不正确可能导致运行出错",curPath);
// 	int ret = MessageBox(strText,"选择",MB_YESNO);
// 	if (ret !=IDYES)
// 	{
// 		OnCancel();
// 		return true;
// 	}
// 	HKEY hregkey=NULL;
// 	
// 	sprintf(strText,"请选择VC6的运行环境还是VC2010的环境，VC6为YES，否则为VC2010");
// 	ret = MessageBox(strText,"选择",MB_YESNO);
// 	if (ret ==IDYES)
// 	{
// 		sprintf(szKeyValue,"%s\\%s;%s\\%s;%s\\%s",curPath,"bin",curPath,"lib",curPath,"libvc");
// 	}
// 	else
// 	{
// 		sprintf(szKeyValue,"%s\\%s;%s\\%s;%s\\%s",curPath,"bin10",curPath,"lib10",curPath,"libvc10");
// 	}
// 	
// 
// 	try
// 	{
// 		//m_publog.errlog("ms",__FILE__,__LINE__,-1,"打开注册键值");
// 		ret = ::RegOpenKeyEx(HKEY_CURRENT_USER, "Environment", 0, KEY_ALL_ACCESS, &hregkey);
// 		if (ERROR_SUCCESS != ret)
// 		{
// 			//m_publog.errlog("ms",__FILE__,__LINE__,-1,"RegOpenKeyEx error");
// 			OnCancel();
// 			return false;
// 		} 
// 	}
// 	catch(...)
// 	{
// 		//m_publog.errlog("ms",__FILE__,__LINE__,-1,"打开注册键值出错");
// 	}
// 	
// 	
// 	
// 	//注册PATH
// 	memset(strText,0,sizeof(strText));
// 	strcpy(szKeyName,"PATH");
// 	try
// 	{
// 		lDataLen = 8100;
// 		ret = QueryRegValue(HKEY_CURRENT_USER, "Environment", szKeyName, lType, 
// 			(unsigned char*)(LPCSTR)strText, lDataLen, szErrMsg, sizeof(szErrMsg));
// 		if (ERROR_SUCCESS != ret)
// 		{
// 			sprintf(strText,"查询注册表出错 %d %s",ret,szErrMsg);
// 			//m_publog.errlog("ms",__FILE__,__LINE__,-1,"RegOpenKeyEx error");
// 			MessageBox(strText);
// 			OnCancel();
// 			return false;
// 		} 
// 	}
// 	catch(...)
// 	{
// 		//m_publog.errlog("ms",__FILE__,__LINE__,ret,"查询注册键值出错");
// 		OnCancel();
// 		return false;
// 	}
// 	if (strlen(strText)>0 )
// 	{
// 		sprintf(szErrMsg,"%s;%s",strText,szKeyValue);
// 		strcpy(szKeyValue,szErrMsg);
// 	}
// 	else
// 	{
// 		MessageBox("查询注册表 HKEY_CURRENT_USER Environment 键值出错");
// 		OnCancel();
// 		return false;
// 	}
// 	try
// 	{
// 		
// 		if(::RegSetValueEx(hregkey,	szKeyName, NULL, REG_SZ,
// 			(CONST BYTE *)szKeyValue, strlen(szKeyValue)) != ERROR_SUCCESS)
// 		{
// 			RegCloseKey(hregkey);
// 			//m_publog.errlog("ms",__FILE__,__LINE__,-1,"RegSetValueEx error");
// 			MessageBox("RegSetValueEx error");
// 			return false;
// 		}
// 	}
// 	catch(...)
// 	{
// 		//m_publog.errlog("ms",__FILE__,__LINE__,-1,szValueName);
// 		//m_publog.errlog("ms",__FILE__,__LINE__,-1,g_szAppName);
// 		//m_publog.errlog("ms",__FILE__,__LINE__,-1,"重置注册键值出错");
// 	}
// 
// 
// 	unsigned long dwReturnValue;
// 	SendMessageTimeout(HWND_BROADCAST,   WM_SETTINGCHANGE,   0,   
// 		(LPARAM)   "Environment",   SMTO_ABORTIFHUNG,   
//           5000,   &dwReturnValue);   
// 	MessageBox("注册框架路径成功","提示");
// 	OnCancel();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRegBuPathDlg::OnPaint() 
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
HCURSOR CRegBuPathDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

int CRegBuPathDlg::QueryRegValue(HKEY__ *ReRootKey, LPCTSTR ReSubKey, TCHAR *ReValueName, DWORD &dwType, unsigned char *content, unsigned long &dwLength, char *ErrMsg, int iErrMsgLen)
{
	HKEY hKey=NULL;
	if(RegOpenKeyEx(ReRootKey,ReSubKey,0,KEY_READ,&hKey)==ERROR_SUCCESS)
	{
		//		publog.errlog("ms",__FILE__,__LINE__,0,(char *)ReSubKey);
		//		publog.errlog("ms",__FILE__,__LINE__,0,ReValueName);
		int ret;
		ret = RegQueryValueEx(hKey,ReValueName,NULL,&dwType,(unsigned char *)content,&dwLength);
		if(ret != ERROR_SUCCESS)
		{
			strncpy(ErrMsg, "错误：无法查询有关的注册表信息", iErrMsgLen);
			ErrMsg[iErrMsgLen - 1] = 0;
			return ret;
		}
		RegCloseKey(hKey);
	}
	else
	{
		strncpy(ErrMsg, "错误：无法打开有关的hKEY", iErrMsgLen);
		ErrMsg[iErrMsgLen - 1] = 0;
		return -1;
	}
	return 0;
}

void CRegBuPathDlg::OnOK() 
{
	// TODO: Add extra validation here
	char szKeyValue[8192] ;
	char szKeyName[8192] ;
	char curPath[8192];
	
	char strText[8192];
	char szErrMsg[8192];
	memset(szKeyName,0,sizeof(szKeyName));
	memset(szKeyValue,0,sizeof(szKeyValue));
	memset(strText,0,sizeof(strText));
	memset(szErrMsg,0,sizeof(szErrMsg));
	memset(curPath,0,sizeof(curPath));

	unsigned long lDataLen;
	unsigned long lType = 0;
	UpdateData();
	strcpy(curPath,m_csDir.GetBuffer(m_csDir.GetLength()));
	sprintf(strText,"确定BUBASE框架路径:[%s] ?，不正确可能导致运行出错",curPath);
	int ret = MessageBox(strText,"选择",MB_YESNO);
	if (ret !=IDYES)
	{
		return ;
	}
	HKEY hregkey=NULL;
	
	sprintf(szKeyValue,"%s\\%s;%s\\%s;%s\\%s",curPath,"bin",curPath,"lib",curPath,"libvc");
	try
	{
		//m_publog.errlog("ms",__FILE__,__LINE__,-1,"打开注册键值");
		ret = ::RegOpenKeyEx(HKEY_CURRENT_USER, "Environment", 0, KEY_ALL_ACCESS, &hregkey);
		if (ERROR_SUCCESS != ret)
		{
			//m_publog.errlog("ms",__FILE__,__LINE__,-1,"RegOpenKeyEx error");
			OnCancel();
			return ;
		} 
	}
	catch(...)
	{
		//m_publog.errlog("ms",__FILE__,__LINE__,-1,"打开注册键值出错");
	}
	
	
	
	//注册PATH
	memset(strText,0,sizeof(strText));
	strcpy(szKeyName,"PATH");
	try
	{
		lDataLen = 8100;
		ret = QueryRegValue(HKEY_CURRENT_USER, "Environment", szKeyName, lType, 
			(unsigned char*)(LPCSTR)strText, lDataLen, szErrMsg, sizeof(szErrMsg));
		if (ERROR_SUCCESS != ret)
		{
			sprintf(strText,"查询注册表出错 %d %s",ret,szErrMsg);
			//m_publog.errlog("ms",__FILE__,__LINE__,-1,"RegOpenKeyEx error");
			MessageBox(strText);
			return ;
		} 
	}
	catch(...)
	{
		//m_publog.errlog("ms",__FILE__,__LINE__,ret,"查询注册键值出错");
		return ;
	}
	if (strlen(strText)>0 )
	{
		sprintf(szErrMsg,"%s;%s",strText,szKeyValue);
		strcpy(szKeyValue,szErrMsg);
	}
	else
	{
		MessageBox("查询注册表 HKEY_CURRENT_USER Environment 键值出错");
		return ;
	}
	try
	{
		
		if(::RegSetValueEx(hregkey,	szKeyName, NULL, REG_SZ,
			(CONST BYTE *)szKeyValue, strlen(szKeyValue)) != ERROR_SUCCESS)
		{
			RegCloseKey(hregkey);
			//m_publog.errlog("ms",__FILE__,__LINE__,-1,"RegSetValueEx error");
			MessageBox("RegSetValueEx error");
			return ;
		}
	}
	catch(...)
	{
		//m_publog.errlog("ms",__FILE__,__LINE__,-1,szValueName);
		//m_publog.errlog("ms",__FILE__,__LINE__,-1,g_szAppName);
		//m_publog.errlog("ms",__FILE__,__LINE__,-1,"重置注册键值出错");
	}

// MS VC++ 14.0 _MSC_VER = 1900 (Visual Studio 2015)
// MS VC++ 12.0 _MSC_VER = 1800 (VisualStudio 2013)
// MS VC++ 11.0 _MSC_VER = 1700 (VisualStudio 2012)
// MS VC++ 10.0 _MSC_VER = 1600(VisualStudio 2010)
// MS VC++ 9.0 _MSC_VER = 1500(VisualStudio 2008)
// MS VC++ 8.0 _MSC_VER = 1400(VisualStudio 2005)
// MS VC++ 7.1 _MSC_VER = 1310(VisualStudio 2003)
// MS VC++ 7.0 _MSC_VER = 1300(VisualStudio .NET)
// MS VC++ 6.0 _MSC_VER = 1200(VisualStudio 98)
// MS VC++ 5.0 _MSC_VER = 1100(VisualStudio 97)
#if (_MSC_VER > 1200)
	{
		//unsigned long dwReturnValue;
		
		PDWORD_PTR dwReturnValue;
		SendMessageTimeout(HWND_BROADCAST,   WM_SETTINGCHANGE,   0,   
			(LPARAM)   "Environment",   SMTO_ABORTIFHUNG,   
          5000,   dwReturnValue);   
	}
#else
	{
		unsigned long dwReturnValue;
		
		SendMessageTimeout(HWND_BROADCAST,   WM_SETTINGCHANGE,   0,   
			(LPARAM)   "Environment",   SMTO_ABORTIFHUNG,   
          5000,   &dwReturnValue);   
	}
#endif
	MessageBox("注册框架路径成功","提示");
	CDialog::OnOK();
}

void CRegBuPathDlg::OnButtonBrow() 
{
	// TODO: Add your control notification handler code here
	char szPath[400];     //存放选择的目录路径 
    CString str;
	
    memset(szPath,0, sizeof(szPath));   
	WORKPATH(szPath);
    BROWSEINFO bi;   
    bi.hwndOwner = m_hWnd;   
    bi.pidlRoot = NULL;   
    bi.pszDisplayName = szPath;   
    bi.lpszTitle = "请选择框架目录：";   
    bi.ulFlags = 0;   
    bi.lpfn = NULL;   
    bi.lParam = 0;   
    bi.iImage = 0;   
    //弹出选择目录对话框
    LPITEMIDLIST lp = SHBrowseForFolder(&bi);   
	
    if(lp && SHGetPathFromIDList(lp, szPath))   
    {
        m_edtDir.SetWindowText(szPath);
    }
    
	
}
