// testcommDlg.cpp : implementation file
//

#include "stdafx.h"
#include "testcomm.h"
#include "testcommDlg.h"

// #include "Encode.h"
// #include "zip.h"

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
// CTestcommDlg dialog

CTestcommDlg::CTestcommDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestcommDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTestcommDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	memset(&m_commpara,0,sizeof(S_COMM_PARA));
	memset(m_errMsg,0,sizeof(m_errMsg));
	m_bIsResult = true;
}

void CTestcommDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestcommDlg)
	DDX_Control(pDX, IDC_CHECK_CONNECT, m_chkConnect);
	DDX_Control(pDX, IDC_EDIT_LOG_LEVEL, m_edtLevel);
	DDX_Control(pDX, IDC_COMBO_PWDFLAG, m_combPwdFlag);
	DDX_Control(pDX, IDC_COMBO_ZIP, m_combZip);
	DDX_Control(pDX, IDC_EDIT_MINTIME, m_edtMinTime);
	DDX_Control(pDX, IDC_EDIT_MAXTIME, m_edtMaxTime);
	DDX_Control(pDX, IDC_PROGRESS, m_progress);
	DDX_Control(pDX, IDC_CHECK_FILL, m_chkFill);
	DDX_Control(pDX, IDC_COMBO_TYPE, m_combType);
	DDX_Control(pDX, IDC_COMBO_SEL_PACK, m_combSelectPack);
	DDX_Control(pDX, IDC_COMBO_SEL_HEAD, m_combSelectHead);
	DDX_Control(pDX, IDC_COMBO_ALIGN, m_combAlign);
	DDX_Control(pDX, IDC_CHECK_INCLUDE, m_chkInclude);
	DDX_Control(pDX, IDC_RADIO_ADD_PACK, m_rdoPack);
	DDX_Control(pDX, IDC_RADIO_ADD_HEAD, m_rdoHead);
	DDX_Control(pDX, IDC_RADIO_MODI_PACK, m_rdoModiPack);
	DDX_Control(pDX, IDC_RADIO_MODI_HEAD, m_rdoModiHead);
	DDX_Control(pDX, IDC_RADIO_INVAR, m_rdoInvar);
	DDX_Control(pDX, IDC_RADIO_INVARSEG, m_rdoInvarSeg);
	DDX_Control(pDX, IDC_RADIO_SEG, m_rdoSeg);
	DDX_Control(pDX, IDC_RADIO_XML, m_rdoXml);
	DDX_Control(pDX, IDC_BUTTON_SEND, m_btnSend);
	DDX_Control(pDX, IDC_BUTTON_SAVE_TREE, m_btnSaveTree);
	DDX_Control(pDX, IDC_BUTTON_SAVE_PARA, m_btnSavePara);
	DDX_Control(pDX, IDC_BUTTON_MODI_ADD, m_btnModiAdd);
	DDX_Control(pDX, IDC_BUTTON_MODI, m_btnModi);
	DDX_Control(pDX, IDC_BUTTON_DISCONNECT, m_btnDisconnect);
	DDX_Control(pDX, IDC_BUTTON_DELETE, m_btnDelete);
	DDX_Control(pDX, IDC_BUTTON_CONNECT, m_btnConnect);
	DDX_Control(pDX, IDC_BUTTON_CLEAR_TREE, m_btnClearTree);
	DDX_Control(pDX, IDC_BUTTON_ADD, m_btnAdd);
	DDX_Control(pDX, IDC_TREE_ANS, m_treeAns);
	DDX_Control(pDX, IDC_LIST_PACK, m_lstPack);
	DDX_Control(pDX, IDC_LIST_HEAD, m_lstHead);
	DDX_Control(pDX, IDC_EDIT_VALUE, m_edtValue);
	DDX_Control(pDX, IDC_EDIT_TIMEOUT, m_edtTimeOut);
	DDX_Control(pDX, IDC_EDIT_TIME, m_edtTime);
	DDX_Control(pDX, IDC_EDIT_THREAD_NUM, m_edtThreadNum);
	DDX_Control(pDX, IDC_EDIT_SEND_NUM, m_edtSendNum);
	DDX_Control(pDX, IDC_EDIT_SEG, m_edtSeg);
	DDX_Control(pDX, IDC_EDIT_PORT, m_edtPort);
	DDX_Control(pDX, IDC_EDIT_NAME, m_edtName);
	DDX_Control(pDX, IDC_EDIT_LENGTH, m_edtLength);
	DDX_Control(pDX, IDC_EDIT_DEC, m_edtDec);
	DDX_Control(pDX, IDC_EDIT_FUNCNAME, m_edtFuncName);
	DDX_Control(pDX, IDC_EDIT_FUNCNO, m_edtFuncNo);
	DDX_Control(pDX, IDC_EDIT_HEAD, m_edtHead);
	DDX_Control(pDX, IDC_EDIT_IP, m_edtIp);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTestcommDlg, CDialog)
	//{{AFX_MSG_MAP(CTestcommDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_SAVE_PARA, OnButtonSavePara)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_TREE, OnButtonClearTree)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, OnButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_DISCONNECT, OnButtonDisconnect)
	ON_BN_CLICKED(IDC_BUTTON_MODI, OnButtonModi)
	ON_BN_CLICKED(IDC_BUTTON_MODI_ADD, OnButtonModiAdd)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_TREE, OnButtonSaveTree)
	ON_BN_CLICKED(IDC_BUTTON_SEND, OnButtonSend)
	ON_BN_CLICKED(IDC_RADIO_ADD_HEAD, OnRadioAddHead)
	ON_BN_CLICKED(IDC_RADIO_ADD_PACK, OnRadioAddPack)
	ON_BN_CLICKED(IDC_RADIO_MODI_HEAD, OnRadioModiHead)
	ON_BN_CLICKED(IDC_RADIO_MODI_PACK, OnRadioModiPack)
	ON_BN_CLICKED(IDC_RADIO_INVAR, OnRadioInvar)
	ON_BN_CLICKED(IDC_RADIO_INVARSEG, OnRadioInvarseg)
	ON_BN_CLICKED(IDC_RADIO_SEG, OnRadioSeg)
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, OnSelchangeComboType)
	ON_CBN_SELCHANGE(IDC_COMBO_SEL_HEAD, OnSelchangeComboSelHead)
	ON_CBN_SELCHANGE(IDC_COMBO_SEL_PACK, OnSelchangeComboSelPack)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_HEAD, OnItemchangedListHead)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PACK, OnItemchangedListPack)
	ON_BN_CLICKED(IDC_RADIO_XML, OnRadioXml)
	ON_EN_CHANGE(IDC_EDIT_SEND_NUM, OnChangeEditSendNum)
	ON_BN_CLICKED(IDC_CHECK_CONNECT, OnCheckConnect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestcommDlg message handlers

BOOL CTestcommDlg::OnInitDialog()
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

	m_edtIp.SetLimitText(17);
	m_edtPort.SetLimitText(6);
	m_edtTimeOut.SetLimitText(7);
	m_edtHead.SetLimitText(2);
	m_edtName.SetLimitText(40);
	m_edtFuncNo.SetLimitText(10);
	m_edtFuncName.SetLimitText(100);
	m_edtValue.SetLimitText(1000);
	m_edtSeg.SetLimitText(2);
	m_edtDec.SetLimitText(1);
	m_edtLength.SetLimitText(3);
	m_edtThreadNum.SetLimitText(3);
	m_edtSendNum.SetLimitText(8);
	m_edtLength.SetWindowText("0");
	m_edtDec.SetWindowText("0");

	//m_btnSend.LoadResource(IDC_BUTTON_SEND,"��������������",NULL);
	//m_btnSaveTree.LoadResource(IDC_BUTTON_SAVE_TREE,"��������Ӧ����",NULL);
	//m_btnSavePara.LoadResource(IDC_BUTTON_SAVE_PARA,"����ͨѶ����",NULL);
	//m_btnModiAdd.LoadResource(IDC_BUTTON_MODI_ADD,"���Ӱ�ͷ���ݻ��������",NULL);
	//m_btnModi.LoadResource(IDC_BUTTON_MODI,"�޸İ�ͷ���ݻ��������",NULL);
	//m_btnDisconnect.LoadResource(IDC_BUTTON_DISCONNECT,"�Ͽ���������",NULL);
	//m_btnDelete.LoadResource(IDC_BUTTON_DELETE,"ɾ����ͷ���ݻ��������",NULL);
	//m_btnConnect.LoadResource(IDC_BUTTON_CONNECT,"���ӷ���",NULL);
	//m_btnClearTree.LoadResource(IDC_BUTTON_CLEAR_TREE,"�������Ӧ����",NULL);
	//m_btnAdd.LoadResource(IDC_BUTTON_ADD,"���Ӱ�ͷ����幦��",NULL);

	m_combZip.ResetContent();
	m_combZip.FormatComboBox();
	m_combZip.SetColumnWidth(15, 40);
	m_combZip.SetColumnAlignStyle(DT_LEFT, DT_LEFT);
	m_combZip.AddRow("0","�����ܲ�ѹ��");
	m_combZip.AddRow("2","ѹ��");

	m_combZip.SetCurSel(0);
	m_combZip.EnableWindow(false);

	m_combPwdFlag.ResetContent();
	m_combPwdFlag.FormatComboBox();
	m_combPwdFlag.SetColumnWidth(15, 40);
	m_combPwdFlag.SetColumnAlignStyle(DT_LEFT, DT_LEFT);
	m_combPwdFlag.AddRow("0","����δ����");
	m_combPwdFlag.AddRow("1","�����Ѽ���");
	m_combPwdFlag.SetCurSel(0);
	m_combPwdFlag.EnableWindow(false);

	m_combType.ResetContent();
	m_combType.FormatComboBox();
	m_combType.SetColumnWidth(15, 40);
	m_combType.SetColumnAlignStyle(DT_LEFT, DT_LEFT);
	m_combType.AddRow("0","�ַ�����");
	m_combType.AddRow("1","��������");
	m_combType.AddRow("2","����");
	m_combType.SetCurSel(0);

	m_combAlign.ResetContent();
	m_combAlign.FormatComboBox();
	m_combAlign.SetColumnWidth(15, 40);
	m_combAlign.SetColumnAlignStyle(DT_LEFT, DT_LEFT);	
	m_combAlign.AddRow("0","�����");
	m_combAlign.AddRow("1","�Ҷ���");
	m_combAlign.SetCurSel(0);

	m_combSelectPack.ResetContent();
	m_combSelectPack.FormatComboBox();
	m_combSelectPack.SetColumnWidth(60, 100);
	m_combSelectPack.SetColumnAlignStyle(DT_LEFT, DT_LEFT);	

	m_combSelectHead.ResetContent();
	m_combSelectHead.FormatComboBox();
	m_combSelectHead.SetColumnWidth(60, 100);
	m_combSelectHead.SetColumnAlignStyle(DT_LEFT, DT_LEFT);	
	
	m_edtName.EnableWindow(false);
	m_edtValue.EnableWindow(false);
	m_edtLength.EnableWindow(false);
	m_edtDec.EnableWindow(false);
	m_edtSeg.EnableWindow(false);
	m_rdoPack.SetCheck(1);
	m_rdoInvar.SetCheck(1);
	m_edtSendNum.SetWindowText("1");
	m_edtThreadNum.SetWindowText("1");
	m_edtSeg.SetWindowText("|");
	m_btnModiAdd.EnableWindow(false);
	m_btnModi.EnableWindow(false);
	m_btnDelete.EnableWindow(false);
	m_combType.EnableWindow(false);
	m_combAlign.EnableWindow(false);
	
//	m_lstHead.SetRowColors(RGB(9,24,227),RGB(255,255,255));
	m_lstHead.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_HEADERDRAGDROP);

	
	m_lstHead.InsertColumn(0, "���", LVCFMT_LEFT, 38);
	m_lstHead.InsertColumn(1, "�ֶ���", LVCFMT_LEFT, 90);
	m_lstHead.InsertColumn(2, "�ֶ�ֵ", LVCFMT_LEFT, 138);
	m_lstHead.InsertColumn(3, "����", LVCFMT_LEFT, 38);
	m_lstHead.InsertColumn(4, "����", LVCFMT_LEFT, 38);
	m_lstHead.InsertColumn(5, "С��", LVCFMT_LEFT, 38);
	m_lstHead.InsertColumn(6, "����", LVCFMT_LEFT, 38);
//	m_lstHead.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	
	m_lstPack.InsertColumn(0, "���", LVCFMT_LEFT, 38);
	m_lstPack.InsertColumn(1, "�ֶ���", LVCFMT_LEFT, 90);
	m_lstPack.InsertColumn(2, "�ֶ�ֵ", LVCFMT_LEFT, 138);
	m_lstPack.InsertColumn(3, "����", LVCFMT_LEFT, 38);
	m_lstPack.InsertColumn(4, "����", LVCFMT_LEFT, 38);
	m_lstPack.InsertColumn(5, "С��", LVCFMT_LEFT, 38);
	m_lstPack.InsertColumn(6, "����", LVCFMT_LEFT, 38);
//	m_lstPack.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
//	m_lstPack.SetRowColors(RGB(9,24,227),RGB(255,255,255));
	m_lstPack.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_HEADERDRAGDROP);

	m_edtLevel.SetWindowText("4");
	m_pTool.MakeDir("cfg");

	m_pLog.SetLogPara(LOG_DEBUG,"","testcomm.inf");
	
	//��ʼͨѶ����
	if (m_commparaxml.fromFile("testcomm.xml")<0)
	{
		m_commparaxml.newPackage();
		m_commparaxml.setPackNameValue("IP","127.0.0.1");
		m_commparaxml.setPackNameValue("PORT","8000");
		m_commparaxml.setPackNameValue("TIMEOUT","5000");
		m_commparaxml.setPackNameValue("COMMHEAD","5");
		m_commparaxml.setPackNameValue("ISINCLUDE","0");
		m_commparaxml.toFile("testcomm.xml");
	}

	string ss;
	m_commparaxml.getPackValue("IP",ss);
	m_edtIp.SetWindowText(ss.c_str());
	m_commparaxml.getPackValue("PORT",ss);
	m_edtPort.SetWindowText(ss.c_str());
	m_commparaxml.getPackValue("TIMEOUT",ss);
	m_edtTimeOut.SetWindowText(ss.c_str());
	m_commparaxml.getPackValue("COMMHEAD",ss);
	m_edtHead.SetWindowText(ss.c_str());
	m_commpara.isinclude = m_commparaxml.getPackValueI("ISINCLUDE");
	if (m_commpara.isinclude == 1)
	{
		m_chkInclude.SetCheck(1);
	}
	else
	{
		m_chkInclude.SetCheck(0);
	}

	//��ʼ��ͷ����ѡ��
	InitPackSelect();
	InitHeadSelect();

	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestcommDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTestcommDlg::OnPaint() 
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
HCURSOR CTestcommDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CTestcommDlg::OnButtonSavePara() 
{
	// TODO: Add your control notification handler code here
	GetCommPara();
	m_commparaxml.setPackNameValue("IP",m_commpara.ip);
	m_commparaxml.setPackNameValue("PORT",m_commpara.port);
	m_commparaxml.setPackNameValue("TIMEOUT",m_commpara.timeout);
	m_commparaxml.setPackNameValue("COMMHEAD",m_commpara.headlen);
	m_commparaxml.setPackNameValue("ISINCLUDE",m_commpara.isinclude);
	if (m_commparaxml.toFile("testcomm.xml")<0)
	{
		sprintf(m_errMsg,"�������ʧ��[%s]",m_commparaxml.m_errMsg);
		AfxMessageBox(m_errMsg);
	}
	else
	{
		AfxMessageBox("��������ɹ�");
	}
}

void CTestcommDlg::OnButtonAdd() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	int ret;
	int funcNo;
	CString csFuncNo;
	CString csFuncName;
	char fullname[300];
	char errmsg[100];
	memset(fullname,0,sizeof(fullname));
	m_edtFuncNo.GetWindowText(csFuncNo);
	m_edtFuncName.GetWindowText(csFuncName);
	if (m_rdoPack.GetCheck()==1)
	{
		funcNo = atoi(csFuncNo);
		if (funcNo<=0)
		{
			MessageBox("���ܺŲ���ΪС�ڵ���0��", "����", MB_ICONSTOP);
			m_edtFuncNo.SetFocus();
			m_edtFuncNo.SetWindowText("");
			return;
		}
		csFuncName.TrimLeft();
		csFuncName.TrimRight();
		if (csFuncName.GetLength()<1)
		{
			MessageBox("�������Ʋ���Ϊ�գ�", "����", MB_ICONSTOP);
			m_edtFuncName.SetFocus();
			return;
		}
		//���ù��ܺ��Ƿ��Ѿ�����
		for(int i=0;i< m_allpackno.size();i++)
		{
			if(funcNo==m_allpackno[i])
			{
				MessageBox("���ܺ��Ѿ����б��У�", "����", MB_ICONSTOP);
				return;
			}
		}
		sprintf(fullname,"Cfg/PACK%d.xml",funcNo);
		//����һ���µ�xml��
		ret = m_sendData.m_packxml.newPackage();
		if (ret<0)
		{
			MessageBox("������xml��ʧ��", "����", MB_ICONSTOP);
			return;
		}
		m_sendData.m_packxml.setHeadNameValue("������",funcNo);
		m_sendData.m_packxml.gotoPack();
		m_sendData.m_packxml.m_xml.getParentItem();
		if (!m_sendData.m_packxml.m_xml.setAttribute("name",csFuncName.GetBuffer(csFuncName.GetLength())))
		{
			MessageBox("������ʧ��", "����", MB_ICONSTOP);
			return;
		}
		ret = m_sendData.m_packxml.toFile(fullname);
		if (ret<0)
		{
			MessageBox("д�����ļ�ʧ��", "����", MB_ICONSTOP);
			return;
		}
		sprintf(errmsg,"%d",funcNo);
		m_combSelectPack.AddRow(errmsg,csFuncName.GetBuffer(csFuncName.GetLength()));
		m_allpackno.push_back(funcNo);
		m_edtFuncName.SetWindowText("");
		sprintf(errmsg,"�����¹���[%d]�ɹ�",funcNo);
		AfxMessageBox(errmsg);
	}
	else
	{
		funcNo = atoi(csFuncNo);
		if (funcNo<=0)
		{
			MessageBox("���ܺŲ���ΪС��0��", "����", MB_ICONSTOP);
			m_edtFuncNo.SetFocus();
			m_edtFuncNo.SetWindowText("");
			return;
		}
		csFuncName.TrimLeft();
		csFuncName.TrimRight();
		if (csFuncName.GetLength()<1)
		{
			MessageBox("�������Ʋ���Ϊ�գ�", "����", MB_ICONSTOP);
			m_edtFuncName.SetFocus();
			return;
		}
		//���ù��ܺ��Ƿ��Ѿ�����
		for(int i=0;i< m_allheadno.size();i++)
		{
			if(funcNo==m_allheadno[i])
			{
				MessageBox("���ܺ��Ѿ����б��У�", "����", MB_ICONSTOP);
				return;
			}
		}
		sprintf(fullname,"Cfg/HEAD%d.xml",funcNo);
		//����һ���µ�xml��
		ret = m_sendData.m_headxml.newPackage();
		if (ret<0)
		{
			MessageBox("������xml��ʧ��", "����", MB_ICONSTOP);
			return;
		}
		
		m_sendData.m_headxml.gotoPack();
		m_sendData.m_headxml.m_xml.getParentItem();
		if (!m_sendData.m_headxml.m_xml.setAttribute("name",csFuncName.GetBuffer(csFuncName.GetLength())))
		{
			MessageBox("������ʧ��", "����", MB_ICONSTOP);
			return;
		}
		ret = m_sendData.m_headxml.toFile(fullname);
		if (ret<0)
		{
			MessageBox("д�����ļ�ʧ��", "����", MB_ICONSTOP);
			return;
		}
		sprintf(errmsg,"%d",funcNo);
		m_combSelectHead.AddRow(errmsg,csFuncName.GetBuffer(csFuncName.GetLength()));
		m_allheadno.push_back(funcNo);
		m_edtFuncName.SetWindowText("");
		
		sprintf(errmsg,"�����¹���[%d]�ɹ�",funcNo);
		AfxMessageBox(errmsg);
	}
}

void CTestcommDlg::OnButtonClearTree() 
{
	// TODO: Add your control notification handler code here
	if(MessageBox("��ȷ��Ҫ�����������", "", MB_YESNOCANCEL) == IDYES)
		m_treeAns.DeleteAllItems();
}

void CTestcommDlg::OnButtonConnect() 
{
	// TODO: Add your control notification handler code here
	GetCommPara();
	m_tcp.Create(AF_INET);
	if (!m_tcp.ConnectServer(m_commpara.ip,m_commpara.port))
	{
		sprintf(m_errMsg,"����[%s][%d]ʧ��",m_commpara.ip,m_commpara.port);
		AfxMessageBox(m_errMsg);
	}
	else
	{
		sprintf(m_errMsg,"����[%s][%d]�ɹ�",m_commpara.ip,m_commpara.port);
		AfxMessageBox(m_errMsg);
	}
}

void CTestcommDlg::OnButtonDelete() 
{
	// TODO: Add your control notification handler code here
	CString csname;
	char filename[100];
	memset(filename,0,sizeof(filename));
	m_edtName.GetWindowText(csname);
	csname.TrimLeft();
	csname.TrimRight();
	int ret;
	if (csname.GetLength()<1)
	{
		MessageBox("���Ʋ���Ϊ��!", "����", MB_ICONSTOP);
		m_edtName.SetFocus();
		return;
	}

	if (m_rdoModiPack.GetCheck()==1)
	{
		m_sendData.m_packxml.gotoPack();
		CNode *tmpnode = m_sendData.m_packxml.m_xml.getFirstChildItem(csname.GetBuffer(csname.GetLength()));
		if (tmpnode==NULL)
		{
			MessageBox("û�д��ֶΣ�����ɾ��!", "����", MB_ICONSTOP);
			m_edtName.SetFocus();
			return;
		}
		m_sendData.m_packxml.m_xml.deleteNode(tmpnode);
		strcpy(filename,m_combSelectPack.GetCurColumnItem(0));
		ret = atoi(filename);
		sprintf(filename,"Cfg/PACK%d.xml",ret);
		m_sendData.m_packxml.toFile(filename);
		UpdatePackFromFile(filename);
	}
	else
	{
		m_sendData.m_headxml.gotoPack();
		CNode *tmpnode = m_sendData.m_headxml.m_xml.getFirstChildItem(csname.GetBuffer(csname.GetLength()));
		if (tmpnode==NULL)
		{
			MessageBox("û�д��ֶΣ�����ɾ��!", "����", MB_ICONSTOP);
			m_edtName.SetFocus();
			return;
		}
		m_sendData.m_headxml.m_xml.deleteNode(tmpnode);
		strcpy(filename,m_combSelectHead.GetCurColumnItem(0));
		ret = atoi(filename);
		sprintf(filename,"Cfg/HEAD%d.xml",ret);
		m_sendData.m_headxml.toFile(filename);
		UpdateHeadFromFile(filename);
	}
}

void CTestcommDlg::OnButtonDisconnect() 
{
	// TODO: Add your control notification handler code here
	if (m_tcp.GetConnectStatus())
	{
		m_tcp.Close();
		sprintf(m_errMsg,"��[%s][%d]�Ͽ��ɹ�",m_commpara.ip,m_commpara.port);
		AfxMessageBox(m_errMsg);
	}
	
}

void CTestcommDlg::OnButtonModi() 
{
	// TODO: Add your control notification handler code here
	
}

void CTestcommDlg::OnButtonModiAdd() 
{
	// TODO: Add your control notification handler code here
	CString csname;
	CString csvalue;
	CString csdec;
	CString cslength;
	int nSelectedIndex;
	char stype[10];
	char salign[10];
	char filename[100];
	int ltype;
	int lalign;
	int ldec;
	int llength;
	int ret;
	char cdec[10];
	char serial[10];
	int nLastRow;
	int nItem;
	char tmpchar[8192];
	memset(tmpchar,0,sizeof(tmpchar));
	memset(filename,0,sizeof(filename));
	memset(serial,0,sizeof(serial));
	memset(stype,0,sizeof(stype));
	memset(salign,0,sizeof(salign));
	memset(cdec,0,sizeof(cdec));
	m_edtName.GetWindowText(csname);
	m_edtValue.GetWindowText(csvalue);
	m_edtDec.GetWindowText(csdec);
	m_edtLength.GetWindowText(cslength);
	csname.TrimLeft();
	csname.TrimRight();
	csvalue.TrimLeft();
	csvalue.TrimRight();
	cslength.TrimLeft();
	cslength.TrimRight();

	if (csname.GetLength()<1)
	{
		MessageBox("���Ʋ���Ϊ��!", "����", MB_ICONSTOP);
		m_edtName.SetFocus();
		return;
	}
	ret = csvalue.GetLength();
	if (csvalue.GetLength()<1)
	{
		MessageBox("�ֶ�ֵ����Ϊ��!", "����", MB_ICONSTOP);
		m_edtValue.SetFocus();
		return;
	}
	nSelectedIndex = m_combType.GetCurSel();
	if (nSelectedIndex<0)
	{
		MessageBox("��û��ѡ���ֶ�����!", "����", MB_ICONSTOP);
		m_combType.SetFocus();
		return;
	}
	strcpy(tmpchar,csvalue.GetBuffer(csvalue.GetLength()));
	strcpy(stype,m_combType.GetCurColumnItem(0));
	ltype = atoi(stype);
	
	if  (m_rdoXml.GetCheck()!=1)
	{


		if (ltype == 1)
		{
			
			if (!IsDouble(tmpchar))
			{
				MessageBox("�ֶ�ֵ��double��!", "����", MB_ICONSTOP);
				m_edtValue.SetFocus();
				return;
			}
			csdec.TrimLeft();
			csdec.TrimRight();
			if (csdec.GetLength()<1)
			{
				MessageBox("С��λ����Ϊ��!", "����", MB_ICONSTOP);
				m_edtDec.SetFocus();
				return;
			}
			ldec = atoi(csdec);
		}
		else
		{
			if (ltype == 2)
			{
				CNode aa;
				if (!aa.IsNumber(tmpchar))
				{
					MessageBox("�ֶ�ֵ������!", "����", MB_ICONSTOP);
					m_edtValue.SetFocus();
					return;
				}
			}
			ldec = 0 ;
		}
		
		sprintf(cdec,"%d",ldec);
		nSelectedIndex = m_combAlign.GetCurSel();
		if (nSelectedIndex<0)
		{
			MessageBox("��û��ѡ����뷽ʽ!", "����", MB_ICONSTOP);
			m_combAlign.SetFocus();
			return;
		}
		strcpy(salign,m_combAlign.GetCurColumnItem(0));
		lalign = atoi(salign);
		if (cslength.GetLength()<1)
		{
			MessageBox("�ֶγ��Ȳ���Ϊ��!", "����", MB_ICONSTOP);
			m_edtLength.SetFocus();
			return;
		}

		llength = atoi(cslength);
	}
	if (m_rdoModiPack.GetCheck()==1)
	{
		string ss;
		ret = m_sendData.m_packxml.getPackValue(csname.GetBuffer(csname.GetLength()),ss);
		if (ret==0)
		{
			ret = MessageBox("���д��ֶΣ��Ƿ��޸�?","��ʾ",MB_YESNO);
			if (ret==IDYES)
			{
				
			}
			else
			{
				return;
			}
		}
		m_sendData.m_packxml.setPackNameValue(csname.GetBuffer(csname.GetLength()),csvalue.GetBuffer(csvalue.GetLength()));
		m_sendData.m_packxml.m_xml.setAttribute("type",stype);
		m_sendData.m_packxml.m_xml.setAttribute("dec",cdec);
		m_sendData.m_packxml.m_xml.setAttribute("length",cslength.GetBuffer(cslength.GetLength()));
		m_sendData.m_packxml.m_xml.setAttribute("align",salign);
		strcpy(filename,m_combSelectPack.GetCurColumnItem(0));
		ret = atoi(filename);
		sprintf(filename,"Cfg/PACK%d.xml",ret);
		m_sendData.m_packxml.toFile(filename);
		UpdatePackFromFile(filename);
	}
	else
	{
		string ss;
		ret = m_sendData.m_headxml.getPackValue(csname.GetBuffer(csname.GetLength()),ss);
		if (ret==0)
		{
			ret = MessageBox("���д��ֶΣ��Ƿ��޸�?","��ʾ",MB_YESNO);
			if (ret==IDYES)
			{
				
			}
			else
			{
				return;
			}
		}
		m_sendData.m_headxml.setPackNameValue(csname.GetBuffer(csname.GetLength()),csvalue.GetBuffer(csvalue.GetLength()));
		m_sendData.m_headxml.m_xml.setAttribute("type",stype);
		m_sendData.m_headxml.m_xml.setAttribute("dec",cdec);
		m_sendData.m_headxml.m_xml.setAttribute("length",cslength.GetBuffer(cslength.GetLength()));
		m_sendData.m_headxml.m_xml.setAttribute("align",salign);
		strcpy(filename,m_combSelectHead.GetCurColumnItem(0));
		ret = atoi(filename);
		sprintf(filename,"Cfg/HEAD%d.xml",ret);
		m_sendData.m_headxml.toFile(filename);
		UpdateHeadFromFile(filename);
	}

}

void CTestcommDlg::OnButtonSaveTree() 
{
	// TODO: Add your control notification handler code here
	S_REQANS s_reqans;
	for (int i=0;i<m_sendData.m_reqans.size();i++)
	{
		s_reqans = m_sendData.m_reqans[i];
		if (s_reqans.raflag == 0)
		{
			sprintf(m_errMsg,"���� ���߳�[%d] ʱ��[%s] ����[%d]",s_reqans.threadNo,s_reqans.cutime,s_reqans.bufferlen);
			m_pLog.LogBin(LOG_DEBUG,__FILE__,__LINE__,m_errMsg,s_reqans.buffer,s_reqans.bufferlen);
//			m_publog.Write_Packet(s_reqans.buffer,"testcomm.inf",s_reqans.bufferlen,m_errMsg);
		}
		else
		{
			sprintf(m_errMsg,"Ӧ�� ���߳�[%d] ʱ��[%s] ����[%d]",s_reqans.threadNo,s_reqans.cutime,s_reqans.bufferlen);
			m_pLog.LogBin(LOG_DEBUG,__FILE__,__LINE__,m_errMsg,s_reqans.buffer,s_reqans.bufferlen);
//			m_publog.Write_Packet(s_reqans.buffer,"testcomm.inf",s_reqans.bufferlen,m_errMsg);
		}
	}
	AfxMessageBox("������ testcomm.inf �ļ��ɹ�!");
}

void CTestcommDlg::OnButtonSend() 
{
	// TODO: Add your control notification handler code here
	
	int sendNum=0;
	int i;
	int threadNum=0;
	char segchar[30];
	memset(segchar,0,sizeof(segchar));
	CString cssendNum;
	CString csthreadNum;
	CString csseg;
	int packtype=0;
	int nItem;
	bool isfill;
	long allusetime=0;
	int begintime ;
	int endtime;
	int ans_errno=0;
	int ans_connect_err=0;
	int ans_send_err=0;
	int ans_success=0;
	int ans_recv_err=0;
	int nSendNum=0;
	int maxtime=0;
	int mintime=999999;
	bool isansPack;
	bool ismuanspack;
	int ansPackNum;
	
	S_REQANS s_reqans;
	HTREEITEM	hRoot, hHead, hPack;
	CXmlPack xmlpack;
	char tmpchar[20];
	string ss;
	int ret;
	char filename[300];

	UpdateData();
	m_edtSendNum.GetWindowText(cssendNum);
	m_edtThreadNum.GetWindowText(csthreadNum);
	m_edtSeg.GetWindowText(csseg);
	csseg.TrimLeft();
	csseg.TrimRight();
	sendNum = atoi(cssendNum);
	m_edtLevel.GetWindowText(cssendNum);
	cssendNum.TrimLeft();
	cssendNum.TrimRight();
	m_sendData.m_nLogLevel = atoi(cssendNum);
// 	if (sendNum<1 || sendNum >10000)
// 	{
// 		m_edtSendNum.SetFocus();
// 		MessageBox("���ʹ�����1-10000֮��!", "����", MB_ICONSTOP);
// 		return ;
// 	}
	threadNum = atoi(csthreadNum);
	if (threadNum<1 || threadNum >200)
	{
		m_edtThreadNum.SetFocus();
		MessageBox("�߳�����1-200֮��!", "����", MB_ICONSTOP);
		return ;
	}
	
	if (m_rdoInvar.GetCheck()==1)
	{
		packtype = 0;
	}
	else if (m_rdoInvarSeg.GetCheck()==1)
	{
		packtype = 1;
	}
	else if  (m_rdoXml.GetCheck()==1)
	{
		packtype = 3;
	}
	else 
	{
		packtype = 2;
	}
	//������ 0����1�������ָ�2�ָ�3xml
	if (packtype==1 || packtype==2)
	{
		if (csseg.GetLength()<1)
		{
			m_edtSeg.SetFocus();
			MessageBox("�ָ�������Ϊ��!", "����", MB_ICONSTOP);
			return ;
		}
	}
	strcpy(segchar,csseg.GetBuffer(csseg.GetLength()));
	
	nItem = m_combSelectHead.GetCurSel();
	if (packtype !=3)
	{
		if (nItem<0)
		{
			MessageBox("��û��ѡ���ͷ!", "����", MB_ICONSTOP);
			return ;
		}
	}
	nItem = m_combSelectPack.GetCurSel();
	if (nItem<0)
	{
		MessageBox("��û��ѡ�����!", "����", MB_ICONSTOP);
		return ;
	}
	GetCommPara();
	
	if (m_chkFill.GetCheck()==1)
	{
		isfill = true;
	}
	else
	{
		isfill = false;
	}
	m_sendData.SetParameter(m_commpara.headlen,m_commpara.isinclude,isfill,packtype,segchar);
	m_sendData.m_reqans.clear();
	m_sendData.m_nSuccessNum = 0;
	m_sendData.m_nFaildNum = 0;
	m_edtTime.SetWindowText("");
	if (threadNum*sendNum<101)
	{
		m_bIsResult = true;
	}
	else
	{
		m_bIsResult = false;
	}
	m_sendData.m_bIsExit = false;
	m_sendData.m_bIsBegin = false;
	for (i=0;i<threadNum;i++)
	{
		m_send2server[i].m_bIsResult = m_bIsResult;
		m_send2server[i].SetParameter(packtype,sendNum,true,&m_sendData,&m_commpara,i);
		m_send2server[i].m_nSuccessNum = 0;
		m_send2server[i].m_nFaildNum = 0;
		m_send2server[i].m_nSendNum = 0;
		m_send2server[i].m_nSendErrorNum = 0;
		m_send2server[i].m_nConnectErrorNum = 0;
		m_send2server[i].m_nRecvErrorNum = 0;
		m_send2server[i].CreateThread();
	}
	int stopnum=0;
	m_progress.SetRange(0,100);
	m_progress.SetPos(1);
	int processid=0;
	

	if (sendNum <10000)
	{
		m_sendData.m_bIsBegin = true;
		while (stopnum < threadNum)
		{
			stopnum = 0;
			for (int j=0;j<threadNum;j++)
			{
				if (m_send2server[j].m_isRunning)
				{
					stopnum = 0;
					processid++;
					if (processid>=99)
					{
						processid = 1;
					}
					m_progress.SetPos(processid);
					break;
				}
				stopnum++;
			}
			SLEEP(10);
		}
	}
	else
	{
		m_sendData.m_bIsBegin = true;
		begintime = m_publog.GetTickCount();
		endtime = m_publog.GetTickCount();
		while (endtime-begintime <=sendNum)
		{
			endtime = m_publog.GetTickCount();
			processid = (int)((endtime-begintime)*100)/sendNum;
			m_progress.SetPos(processid);
			stopnum = 0;
			for (int j=0;j<threadNum;j++)
			{
				if (m_send2server[j].m_isRunning)
				{
					stopnum = 0;
					processid++;
					if (processid>=99)
					{
						processid = 1;
					}
					m_progress.SetPos(processid);
					break;
				}
				stopnum++;
			}
			if (stopnum >= threadNum)
			{
				break;
			}
			SLEEP(10);
		}
		m_sendData.m_bIsExit = true;
		stopnum = 0;
		while (stopnum < threadNum)
		{
			stopnum = 0;
			for (int j=0;j<threadNum;j++)
			{
				if (m_send2server[j].m_isRunning)
				{
					stopnum = 0;
					break;
				}
				stopnum++;
			}
			SLEEP(10);
		}
		SLEEP(100);
	}
	
	m_progress.SetPos(100);
	//д��Ϣ��
	
	for (int k=0;k<threadNum;k++)
	{
		allusetime += m_send2server[k].m_usetime;
	}
	sprintf(m_errMsg,"%ld",allusetime);
	m_edtTime.SetWindowText(m_errMsg);

	
	for (i=0;i<m_sendData.m_reqans.size();i++)
	{
		s_reqans = m_sendData.m_reqans[i];
		if (s_reqans.raflag == 0)
		{
			isansPack = false;
			ansPackNum = 0;
			if (m_bIsResult)
			{
				sprintf(m_errMsg,"���� ���߳�[%d] ʱ��[%s] ����[%d]",s_reqans.threadNo,s_reqans.cutime,s_reqans.bufferlen);
				hRoot = m_treeAns.InsertItem(m_errMsg);
				if (m_rdoXml.GetCheck()==1)
				{
					s_reqans.buffer[s_reqans.bufferlen]=0;
					
					if (!xmlpack.fromBuf(s_reqans.buffer))
					{
						ss = (string)"["+(string)s_reqans.buffer+(string)"]";
						hHead = m_treeAns.InsertItem(ss.c_str(),hRoot);
						m_treeAns.SelectItem(hRoot);
						m_treeAns.Expand(hRoot,TVE_EXPAND);
					}
					else
					{
						hPack = m_treeAns.InsertItem("PACK",hRoot);
						showXmlPack(&xmlpack,&m_treeAns,hPack);
					}
				}
				else
				{
					for (int kk=0;kk<s_reqans.bufferlen;kk++)
					{
						if (s_reqans.buffer[kk]==0)
						{
							s_reqans.buffer[kk] = '*';
						}
					}
					s_reqans.buffer[s_reqans.bufferlen]=0;
					ss = (string)"["+(string)s_reqans.buffer+(string)"]";
					hHead = m_treeAns.InsertItem(ss.c_str(),hRoot);
					m_treeAns.SelectItem(hRoot);
					m_treeAns.Expand(hRoot,TVE_EXPAND);
				}
			}
			
		}
		else
		{
			if (isansPack)
			{
				ismuanspack = true;
				ansPackNum++;
			}
			else
			{
				ismuanspack = false;
				ansPackNum = 0;
			}
			isansPack = true; 
			if (threadNum>1)
			{
				ismuanspack = false;
			}
			if (m_bIsResult)
			{
				if (!ismuanspack)
				{
					sprintf(m_errMsg,"Ӧ�� ���߳�[%d] ʱ��[%s] ����[%d]",s_reqans.threadNo,s_reqans.cutime,s_reqans.bufferlen);
					hRoot = m_treeAns.InsertItem(m_errMsg);
				}
				if (m_rdoXml.GetCheck()==1)
				{
					s_reqans.buffer[s_reqans.bufferlen]=0;
					if (!xmlpack.fromBuf(s_reqans.buffer))
					{
						ss = (string)"["+(string)s_reqans.buffer+(string)"]";
						hHead = m_treeAns.InsertItem(ss.c_str(),hRoot);
						m_treeAns.SelectItem(hRoot);
						m_treeAns.Expand(hRoot,TVE_EXPAND);
					}
					else
					{
						sprintf(tmpchar,"PACK%d",ansPackNum);
						hPack = m_treeAns.InsertItem(tmpchar,hRoot);
						showXmlPack(&xmlpack,&m_treeAns,hPack);
					}
				}
				else
				{
					for (int kk=0;kk<s_reqans.bufferlen;kk++)
					{
						if (s_reqans.buffer[kk]==0)
						{
							s_reqans.buffer[kk] = '*';
						}
					}
					s_reqans.buffer[s_reqans.bufferlen]=0;
					ss = (string)"["+(string)s_reqans.buffer+(string)"]";
					hHead = m_treeAns.InsertItem(ss.c_str(),hRoot);
					m_treeAns.SelectItem(hRoot);
					m_treeAns.Expand(hRoot,TVE_EXPAND);
				}
				

			}
			
			if (s_reqans.raflag == 1)
			{
				if (!ismuanspack)
				{
					ans_success++;
				}
				
				if (s_reqans.usetime>maxtime)
				{
					maxtime = s_reqans.usetime;
				}
				if (s_reqans.usetime<mintime)
				{
					mintime = s_reqans.usetime;
				}
			}
			else
			{
				ans_errno++;
			}
		}
	}
		
	

	if (m_rdoXml.GetCheck() !=1)
	{
		memset(filename,0,sizeof(filename));
		strcpy(filename,m_combSelectHead.GetCurColumnItem(0));
		ret = atoi(filename);
		sprintf(filename,"Cfg/HEAD%d.xml",ret);
		m_sendData.m_headxml.toFile(filename);
		UpdateHeadFromFile(filename);

	}
	
	memset(filename,0,sizeof(filename));
	strcpy(filename,m_combSelectPack.GetCurColumnItem(0));
	ret = atoi(filename);
	sprintf(filename,"Cfg/PACK%d.xml",ret);
	m_sendData.m_packxml.toFile(filename);
	UpdatePackFromFile(filename);
	sprintf(m_errMsg,"%d",maxtime);
	m_edtMaxTime.SetWindowText(m_errMsg);
	sprintf(m_errMsg,"%d",mintime);
	m_edtMinTime.SetWindowText(m_errMsg);
	if (m_bIsResult)
	{
		sprintf(m_errMsg,"�������! �ɹ�[%d]�� ʧ��[%d]�� �ʱ��[%d] ���ʱ��[%d]!",\
			ans_success,ans_errno,maxtime,mintime);
	}
	else
	{
		ans_errno=0;
		ans_success = 0;
		ans_connect_err =0;
		ans_send_err =0;
		for (int j=0;j<threadNum;j++)
		{

			ans_errno = ans_errno + m_send2server[j].m_nFaildNum;
			ans_success = ans_success + m_send2server[j].m_nSuccessNum;
			nSendNum = nSendNum +m_send2server[j].m_nSendNum;
			ans_send_err = ans_send_err + m_send2server[j].m_nSendErrorNum;
			ans_connect_err = ans_connect_err + m_send2server[j].m_nConnectErrorNum;	
			ans_recv_err = ans_recv_err +m_send2server[j].m_nRecvErrorNum;
		}
		
		sprintf(m_errMsg,"%d",ans_success);
		m_edtMaxTime.SetWindowText(m_errMsg);
		sprintf(m_errMsg,"%d",ans_errno);
		m_edtMinTime.SetWindowText(m_errMsg);
		sprintf(m_errMsg,"�������! �ɹ�[%d]�� ʧ��[%d]�� �ܷ���[%d]�� ��������ʧ��[%d]�ʷ���ʧ��[%d]�ʽ���ʧ��[%d]��!",\
			ans_success,ans_errno,nSendNum,ans_connect_err,ans_send_err,ans_recv_err);
	}
	AfxMessageBox(m_errMsg);
	UpdateData();
	
}

void CTestcommDlg::GetCommPara()
{
	CString csValue;
	UpdateData(false);
	memset(&m_commpara,0,sizeof(S_COMM_PARA));
	m_edtIp.GetWindowText(csValue);
	strcpy(m_commpara.ip,csValue.GetBuffer(csValue.GetLength()));
	m_edtPort.GetWindowText(csValue);
	m_commpara.port = atoi(csValue);
	m_edtHead.GetWindowText(csValue);
	m_commpara.headlen = atoi(csValue);
	m_edtTimeOut.GetWindowText(csValue);
	m_commpara.timeout = atoi(csValue);
	if (m_chkInclude.GetCheck() == 1)
	{
		m_commpara.isinclude = 1;
	}
	else
	{
		m_commpara.isinclude = 0;
	}
	if (m_chkConnect.GetCheck() == 1)
	{
		m_commpara.shortconnect = false;
	}
	else
	{
		m_commpara.shortconnect = true;
	}
	m_commpara.zip = atoi(m_combZip.GetCurColumnItem(0));
	m_commpara.pwdflag = atoi(m_combPwdFlag.GetCurColumnItem(0));
}

void CTestcommDlg::InitPackSelect()
{
	int ret;
	CFileFind finder;
	CString   strFile;
	m_allpackno.clear();
	char fullfilename[300];
	//��������ͨ���
	CString strWildcard(".\\Cfg");
	strWildcard += _T("\\PACK*.xml");
	BOOL bWorking = finder.FindFile(strWildcard);
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		strFile=finder.GetFileName();
		sprintf(fullfilename,"Cfg/%s",strFile.GetBuffer(strFile.GetLength()));
		ret = m_sendData.m_packxml.fromFile(fullfilename);
		if (ret<0)
		{
			continue;
		}
		m_sendData.m_packxml.m_xml.getRoot();
		m_sendData.m_packxml.m_xml.getFirstChildItem("package");
		string fname;
		if (!m_sendData.m_packxml.m_xml.getAttribute("name",fname))
		{
			continue;
		}
		strFile.MakeUpper();
		strFile.TrimRight(".XML");
		strFile.TrimLeft("PACK");
		int nFunc=atoi(strFile);
		if(nFunc==0)
		{
			continue;
		}
		m_allpackno.push_back(nFunc);
		m_combSelectPack.AddRow(strFile.GetBuffer(strFile.GetLength()),fname.c_str());
	}
	finder.Close();
}

void CTestcommDlg::InitHeadSelect()
{
	int ret;
	CFileFind finder;
	CString   strFile;
	m_allheadno.clear();
	char fullfilename[300];
	//��������ͨ���
	CString strWildcard(".\\Cfg");
	strWildcard += _T("\\HEAD*.xml");
	BOOL bWorking = finder.FindFile(strWildcard);
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		strFile=finder.GetFileName();
		sprintf(fullfilename,"Cfg/%s",strFile.GetBuffer(strFile.GetLength()));
		ret = m_sendData.m_headxml.fromFile(fullfilename);
		if (ret<0)
		{
			continue;
		}
		m_sendData.m_headxml.m_xml.getRoot();
		m_sendData.m_headxml.m_xml.getFirstChildItem("package");
		string fname;
		if (!m_sendData.m_headxml.m_xml.getAttribute("name",fname))
		{
			continue;
		}
		strFile.MakeUpper();
		strFile.TrimRight(".XML");
		strFile.TrimLeft("HEAD");
		int nFunc=atoi(strFile);
		m_allheadno.push_back(nFunc);
		m_combSelectHead.AddRow(strFile.GetBuffer(strFile.GetLength()),fname.c_str());
	}
	finder.Close();
}

void CTestcommDlg::OnRadioAddHead() 
{
	// TODO: Add your control notification handler code here
	if (m_rdoHead.GetCheck()==1)
	{
		m_edtFuncNo.EnableWindow(true);
		m_edtFuncName.EnableWindow(true);
		m_btnAdd.EnableWindow(true);

		m_edtName.EnableWindow(false);
		m_edtValue.EnableWindow(false);
		m_edtLength.EnableWindow(false);
		m_edtDec.EnableWindow(false);
		m_btnModiAdd.EnableWindow(false);
		m_btnModi.EnableWindow(false);
		m_btnDelete.EnableWindow(false);
		m_combType.EnableWindow(false);
		m_combAlign.EnableWindow(false);

		m_edtFuncNo.SetFocus();
	}
}

void CTestcommDlg::OnRadioAddPack() 
{
	// TODO: Add your control notification handler code here
	if (m_rdoPack.GetCheck()==1)
	{
		m_edtFuncNo.EnableWindow(true);
		m_edtFuncName.EnableWindow(true);
		m_btnAdd.EnableWindow(true);
		
		m_edtName.EnableWindow(false);
		m_edtValue.EnableWindow(false);
		m_edtLength.EnableWindow(false);
		m_edtDec.EnableWindow(false);
		m_btnModiAdd.EnableWindow(false);
		m_btnModi.EnableWindow(false);
		m_btnDelete.EnableWindow(false);
		m_combType.EnableWindow(false);
		m_combAlign.EnableWindow(false);

		m_edtFuncNo.SetFocus();
	}
}

void CTestcommDlg::OnRadioModiHead() 
{
	// TODO: Add your control notification handler code here
	if (m_rdoModiHead.GetCheck()==1)
	{
		if (m_combSelectHead.GetCurSel()<0)
		{
			m_edtFuncNo.EnableWindow(false);
			m_edtFuncName.EnableWindow(false);
			m_btnAdd.EnableWindow(false);
			
			m_edtName.EnableWindow(false);
			m_edtValue.EnableWindow(false);
			m_edtLength.EnableWindow(false);
			m_edtDec.EnableWindow(false);
			m_btnModiAdd.EnableWindow(false);
			m_btnModi.EnableWindow(false);
			m_btnDelete.EnableWindow(false);
			m_combType.EnableWindow(false);
			m_combAlign.EnableWindow(false);
			MessageBox("��û��ѡ���ͷ!", "����", MB_ICONSTOP);
		}
		else
		{
			m_edtFuncNo.EnableWindow(false);
			m_edtFuncName.EnableWindow(false);
			m_btnAdd.EnableWindow(false);
			
			m_edtName.EnableWindow(true);
			m_edtValue.EnableWindow(true);
			m_btnModiAdd.EnableWindow(true);
			m_btnModi.EnableWindow(true);
			m_btnDelete.EnableWindow(true);

			m_edtName.SetFocus();
			m_edtName.SetSel(0,m_edtName.GetWindowTextLength());

			if (m_rdoInvar.GetCheck()==1 || m_rdoInvarSeg.GetCheck()==1)//����
			{
//				m_edtLength.SetWindowText("0");
				m_edtLength.EnableWindow(true);
//				m_combType.SetCurSel(0);
				m_combType.EnableWindow(true);
//				m_edtDec.SetWindowText("0");
				m_edtDec.EnableWindow(true);
				m_combAlign.EnableWindow(true);
			}
			else if  (m_rdoXml.GetCheck()==1) //xml
			{
//				m_edtLength.SetWindowText("0");
				m_edtLength.EnableWindow(false);
//				m_combType.SetCurSel(0);
				m_combType.EnableWindow(false);
//				m_edtDec.SetWindowText("0");
				m_edtDec.EnableWindow(false);
				m_combAlign.EnableWindow(false);
			}
			else 
			{
				//�ָ�
//				m_edtLength.SetWindowText("0");
				m_edtLength.EnableWindow(false);
//				m_combType.SetCurSel(0);
				m_combType.EnableWindow(true);
//				m_edtDec.SetWindowText("0");
				m_edtDec.EnableWindow(true);
				m_combAlign.EnableWindow(false);
			}

			
		}
		
	}
}

void CTestcommDlg::OnRadioModiPack() 
{
	// TODO: Add your control notification handler code here
	if (m_rdoModiPack.GetCheck()==1)
	{
		if (m_combSelectPack.GetCurSel()<0)
		{
			m_edtFuncNo.EnableWindow(false);
			m_edtFuncName.EnableWindow(false);
			m_btnAdd.EnableWindow(false);
			
			m_edtName.EnableWindow(false);
			m_edtValue.EnableWindow(false);
			m_edtLength.EnableWindow(false);
			m_edtDec.EnableWindow(false);
			m_btnModiAdd.EnableWindow(false);
			m_btnModi.EnableWindow(false);
			m_btnDelete.EnableWindow(false);
			m_combType.EnableWindow(false);
			m_combAlign.EnableWindow(false);
			MessageBox("��û��ѡ�����!", "����", MB_ICONSTOP);
		}
		else
		{
			m_edtFuncNo.EnableWindow(false);
			m_edtFuncName.EnableWindow(false);
			m_btnAdd.EnableWindow(false);
			
			m_edtName.EnableWindow(true);
			m_edtValue.EnableWindow(true);
			m_btnModiAdd.EnableWindow(true);
			m_btnModi.EnableWindow(true);
			m_btnDelete.EnableWindow(true);

			
			m_edtName.SetFocus();
			m_edtName.SetSel(0,m_edtName.GetWindowTextLength());
			if (m_rdoInvar.GetCheck()==1 || m_rdoInvarSeg.GetCheck()==1)//����
			{
//				m_edtLength.SetWindowText("0");
				m_edtLength.EnableWindow(true);
//				m_combType.SetCurSel(0);
				m_combType.EnableWindow(true);
//				m_edtDec.SetWindowText("0");
				m_edtDec.EnableWindow(true);
				m_combAlign.EnableWindow(true);
			}
			else if  (m_rdoXml.GetCheck()==1) //xml
			{
//				m_edtLength.SetWindowText("0");
				m_edtLength.EnableWindow(false);
//				m_combType.SetCurSel(0);
				m_combType.EnableWindow(false);
//				m_edtDec.SetWindowText("0");
				m_edtDec.EnableWindow(false);
				m_combAlign.EnableWindow(false);
			}
			else 
			{
				//�ָ�
//				m_edtLength.SetWindowText("0");
				m_edtLength.EnableWindow(false);
//				m_combType.SetCurSel(0);
				m_combType.EnableWindow(true);
//				m_edtDec.SetWindowText("0");
				m_edtDec.EnableWindow(true);
				m_combAlign.EnableWindow(false);
			}
		}
		
	}
}

void CTestcommDlg::OnRadioInvar() 
{
	// TODO: Add your control notification handler code here
	m_edtSeg.EnableWindow(false);
	m_combSelectHead.EnableWindow(true);
	m_combPwdFlag.EnableWindow(false);
	m_combZip.EnableWindow(false);
	m_chkFill.EnableWindow(true);

//	m_edtLength.SetWindowText("0");
	m_edtLength.EnableWindow(true);
	m_combAlign.EnableWindow(true);
//    m_combType.SetCurSel(0);
	m_combType.EnableWindow(true);
//	m_edtDec.SetWindowText("2");
	m_edtDec.EnableWindow(true);
}

void CTestcommDlg::OnRadioInvarseg() 
{
	// TODO: Add your control notification handler code here
	m_edtSeg.EnableWindow(true);
	m_combSelectHead.EnableWindow(true);
	m_edtSeg.SetSel(0,1);
	m_edtSeg.SetFocus();
	m_combPwdFlag.EnableWindow(false);
	m_combZip.EnableWindow(false);
	m_chkFill.EnableWindow(true);

//	m_edtLength.SetWindowText("0");
	m_edtLength.EnableWindow(true);
	m_combAlign.EnableWindow(true);
//    m_combType.SetCurSel(0);
	m_combType.EnableWindow(true);
//	m_edtDec.SetWindowText("2");
	m_edtDec.EnableWindow(true);
}

void CTestcommDlg::OnRadioSeg() 
{
	// TODO: Add your control notification handler code here
	m_edtSeg.EnableWindow(true);
	m_edtSeg.SetSel(0,1);
	m_edtSeg.SetFocus();
	m_combSelectHead.EnableWindow(true);
	m_combPwdFlag.EnableWindow(false);
	m_combZip.EnableWindow(false);
	m_chkFill.EnableWindow(true);

//	m_edtLength.SetWindowText("0");
	m_edtLength.EnableWindow(false);
	m_combAlign.EnableWindow(false);
//   m_combType.SetCurSel(0);
	m_combType.EnableWindow(true);
//	m_edtDec.SetWindowText("2");
	m_edtDec.EnableWindow(true);
	
}

void CTestcommDlg::OnSelchangeComboType() 
{
	// TODO: Add your control notification handler code here
	char value[40];
	memset(value,0,sizeof(value));
	strcpy(value,m_combType.GetCurColumnItem(0));
	if (atoi(value)==1)
	{
		m_edtDec.EnableWindow(true);
		m_edtDec.SetFocus();
		m_edtDec.SetSel(0,m_edtDec.GetWindowTextLength());
		m_combAlign.SetCurSel(1);
	}
	else
	{
		if (atoi(value)==0)
		{
			m_combAlign.SetCurSel(0);
		}
		else
		{
			m_combAlign.SetCurSel(1);
		}
		m_edtDec.EnableWindow(false);
	}
}

void CTestcommDlg::OnSelchangeComboSelHead() 
{
	// TODO: Add your control notification handler code here
	char fullfilename[300];
	memset(fullfilename,0,sizeof(fullfilename));
	int funcNo = atoi(m_combSelectHead.GetCurColumnItem(0));
	sprintf(fullfilename,"Cfg/HEAD%d.xml",funcNo);
	UpdateHeadFromFile(fullfilename);
}

void CTestcommDlg::OnSelchangeComboSelPack() 
{
	// TODO: Add your control notification handler code here
	char fullfilename[300];
	memset(fullfilename,0,sizeof(fullfilename));
	int funcNo = atoi(m_combSelectPack.GetCurColumnItem(0));
	sprintf(fullfilename,"Cfg/PACK%d.xml",funcNo);
	UpdatePackFromFile(fullfilename);

}

void CTestcommDlg::UpdateHeadFromFile(char *filename)
{
	if (m_sendData.m_headxml.fromFile(filename)<0)
	{
		MessageBox(m_sendData.m_headxml.m_errMsg, "����", MB_ICONSTOP);
		return;
	}
	string name;
	string value;
	string sdec;
	string slength;
	string stype;
	string salign;
	char serial[10];
	memset(serial,0,sizeof(serial));
	int ltype;
	int lalign;
	CAttribute *att1=NULL;
	CAttribute *att2=NULL;
	int rec=0;
	int packtype=0;
	if (m_rdoInvar.GetCheck()==1)
	{
		packtype = 0;
	}
	else if (m_rdoInvarSeg.GetCheck()==1)
	{
		packtype = 1;
	}
	else if  (m_rdoXml.GetCheck()==1)
	{
		packtype = 3;
	}
	else 
	{
		packtype = 2;
	}
	m_lstHead.DeleteAllItems();
	m_sendData.m_headxml.gotoPack();
	CNode *tmpnode = m_sendData.m_headxml.m_xml.getFirstChildItem();
	while (tmpnode!=NULL)
	{
		rec++;
		tmpnode = tmpnode->getNextSibling();
	}
	m_sendData.m_headxml.gotoPack();
	tmpnode = m_sendData.m_headxml.m_xml.getFirstChildItem();
	while (tmpnode!=NULL)
	{
		name = tmpnode->getName();
		value = tmpnode->getValue();
		if (packtype != 3)
		{
			att1 =  tmpnode->getAttributes();
			if (att1==NULL)
			{
				MessageBox("XML���ʹ�����û������", "����", MB_ICONSTOP);
				return;
			}
			att2 =  att1->getNamedItem("dec");
			if (att2==NULL)
			{
				MessageBox("XML���ʹ�dec����û������", "����", MB_ICONSTOP);
				return;
			}
			sdec = att2->getValue();
			att2 = att1->getNamedItem("type");
			if (att2==NULL)
			{
				MessageBox("XML���ʹ�type����û������", "����", MB_ICONSTOP);
				return;
			}
			stype = att2->getValue();
			ltype = atoi(stype.c_str());
			att2 = att1->getNamedItem("length");
			if (att2==NULL)
			{
				MessageBox("XML���ʹ�length����û������", "����", MB_ICONSTOP);
				return;
			}
			slength = att2->getValue();
			att2 = att1->getNamedItem("align");
			if (att2==NULL)
			{
				MessageBox("XML���ʹ�align����û������", "����", MB_ICONSTOP);
				return;
			}
			salign = att2->getValue();
			lalign = atoi(salign.c_str());
			
		}
		else
		{
			slength= "0";
			stype = "0";
			sdec = "0";
			lalign = 0;

		}
		
		int nLastRow = m_lstHead.GetItemCount();
		sprintf(serial,"%d",nLastRow+1);
		int nItem = m_lstHead.InsertItem(nLastRow,serial);
		m_lstHead.SetItemText(nItem,1,name.c_str());
		m_lstHead.SetItemText(nItem,2,value.c_str());
		m_lstHead.SetItemText(nItem,3,slength.c_str());
		if (ltype==0)
		{
			m_lstHead.SetItemText(nItem,4,"�ַ�");
		}
		else if (ltype==1)
		{
			m_lstHead.SetItemText(nItem,4,"����");
		}
		else 
		{
			m_lstHead.SetItemText(nItem,4,"����");
		}
		m_lstHead.SetItemText(nItem,5,sdec.c_str());
		if (lalign==0)
		{
			m_lstHead.SetItemText(nItem,6,"��");
		}
		else
		{
			m_lstHead.SetItemText(nItem,6,"��");
		}
		
		rec--;
		tmpnode = tmpnode->getNextSibling();
	}
	sprintf(m_errMsg,"��ͷ���ݣ� [%s]",filename);
	GetDlgItem(IDC_STATIC_HEAD)->SetWindowText(m_errMsg);
}

void CTestcommDlg::UpdatePackFromFile(char *filename)
{
	if (m_sendData.m_packxml.fromFile(filename)<0)
	{
		MessageBox(m_sendData.m_packxml.m_errMsg, "����", MB_ICONSTOP);
		return;
	}
	string name;
	string value;
	string sdec;
	string slength;
	string stype;
	string salign;
	char serial[10];
	memset(serial,0,sizeof(serial));
	int ltype;
	int lalign;
	CAttribute *att1=NULL;
	CAttribute *att2=NULL;
	int rec=0;
	int packtype=0;
	if (m_rdoInvar.GetCheck()==1)
	{
		packtype = 0;
	}
	else if (m_rdoInvarSeg.GetCheck()==1)
	{
		packtype = 1;
	}
	else if  (m_rdoXml.GetCheck()==1)
	{
		packtype = 3;
	}
	else 
	{
		packtype = 2;
	}
	m_lstPack.DeleteAllItems();
	m_sendData.m_packxml.gotoPack();
	CNode *tmpnode = m_sendData.m_packxml.m_xml.getFirstChildItem();
	while (tmpnode!=NULL)
	{
		rec++;
		tmpnode = tmpnode->getNextSibling();
	}
	m_sendData.m_packxml.gotoPack();
	tmpnode = m_sendData.m_packxml.m_xml.getFirstChildItem();
	while (tmpnode!=NULL)
	{
		name = tmpnode->getName();
		value = tmpnode->getValue();
		if (packtype !=3)
		{
			att1 =  tmpnode->getAttributes();
			if (att1==NULL)
			{
				MessageBox("XML���ʹ�����û������", "����", MB_ICONSTOP);
				return;
			}
			att2 =  att1->getNamedItem("dec");
			if (att2==NULL)
			{
				MessageBox("XML���ʹ�dec����û������", "����", MB_ICONSTOP);
				return;
			}
			sdec = att2->getValue();
			att2 = att1->getNamedItem("type");
			if (att2==NULL)
			{
				MessageBox("XML���ʹ�type����û������", "����", MB_ICONSTOP);
				return;
			}
			stype = att2->getValue();
			ltype = atoi(stype.c_str());
			att2 = att1->getNamedItem("length");
			if (att2==NULL)
			{
				MessageBox("XML���ʹ�length����û������", "����", MB_ICONSTOP);
				return;
			}
			slength = att2->getValue();
			att2 = att1->getNamedItem("align");
			if (att2==NULL)
			{
				MessageBox("XML���ʹ�align����û������", "����", MB_ICONSTOP);
				return;
			}
			salign = att2->getValue();
			lalign = atoi(salign.c_str());
		
		}
		else
		{
			slength= "0";
			stype = "0";
			sdec = "0";
			lalign = 0;
		}
		//sprintf(serial,"%d",rec);
		int nLastRow = m_lstPack.GetItemCount();
		sprintf(serial,"%d",nLastRow+1);
		int nItem = m_lstPack.InsertItem(nLastRow,serial);
		m_lstPack.SetItemText(nItem,1,name.c_str());
		m_lstPack.SetItemText(nItem,2,value.c_str());
		m_lstPack.SetItemText(nItem,3,slength.c_str());
		if (ltype==0)
		{
			m_lstPack.SetItemText(nItem,4,"�ַ�");
		}
		else if (ltype==1)
		{
			m_lstPack.SetItemText(nItem,4,"����");
		}
		else 
		{
			m_lstPack.SetItemText(nItem,4,"����");
		}
		m_lstPack.SetItemText(nItem,5,sdec.c_str());
		if (lalign==0)
		{
			m_lstPack.SetItemText(nItem,6,"��");
		}
		else
		{
			m_lstPack.SetItemText(nItem,6,"��");
		}
		rec--;
		tmpnode = tmpnode->getNextSibling();
	}
	sprintf(m_errMsg,"�������ݣ� [%s]",filename);
	GetDlgItem(IDC_STATIC_PACK)->SetWindowText(m_errMsg);
}

void CTestcommDlg::OnItemchangedListHead(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	*pResult = 0;

	int nItem = pNMListView->iItem;
	char szTmp[501];
	memset(szTmp,0,sizeof(szTmp));
	m_lstHead.GetItemText(nItem, 1, szTmp, 500);
	m_pTool.LRtrim(szTmp);
	m_edtName.SetWindowText(szTmp);
	
	m_lstHead.GetItemText(nItem, 2, szTmp, 500);
	m_pTool.LRtrim(szTmp);
	m_edtValue.SetWindowText(szTmp);
	
	m_lstHead.GetItemText(nItem, 3, szTmp, 500);
	m_pTool.LRtrim(szTmp);
	m_edtLength.SetWindowText(szTmp);

	m_lstHead.GetItemText(nItem, 4, szTmp, 500);
	m_pTool.LRtrim(szTmp);
	if (strcmp(szTmp,"�ַ�")==0)
	{
		m_combType.SetCurSel(0);
		m_edtDec.EnableWindow(false);
	}
	else if (strcmp(szTmp,"����")==0)
	{
		m_combType.SetCurSel(1);
		m_edtDec.EnableWindow(true);
	}
	else if (strcmp(szTmp,"����")==0)
	{
		m_combType.SetCurSel(2);
		m_edtDec.EnableWindow(false);
	}
	
	m_lstHead.GetItemText(nItem, 5, szTmp, 500);
	m_pTool.LRtrim(szTmp);
	m_edtDec.SetWindowText(szTmp);

	m_lstHead.GetItemText(nItem, 6, szTmp, 500);
	m_pTool.LRtrim(szTmp);
	if (strcmp(szTmp,"��")==0)
	{
		m_combAlign.SetCurSel(0);
	}
	else if (strcmp(szTmp,"��")==0)
	{
		m_combAlign.SetCurSel(1);
	}
	GetDlgItem(IDC_RADIO_MODI_HEAD)->PostMessage(BM_CLICK);

	
}

void CTestcommDlg::OnItemchangedListPack(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	*pResult = 0;

	int nItem = pNMListView->iItem;
	char szTmp[8192];
	memset(szTmp,0,sizeof(szTmp));
	m_lstPack.GetItemText(nItem, 1, szTmp, 4096);
	m_pTool.LRtrim(szTmp);
	m_edtName.SetWindowText(szTmp);
	
	memset(szTmp,0,sizeof(szTmp));
	m_lstPack.GetItemText(nItem, 2, szTmp, 4096);
//	m_publog.trim(szTmp);
	m_edtValue.SetWindowText(szTmp);
	int ret = strlen(szTmp);
	memset(szTmp,0,sizeof(szTmp));
	m_lstPack.GetItemText(nItem, 3, szTmp, 4096);
	m_pTool.LRtrim(szTmp);
	m_edtLength.SetWindowText(szTmp);
	
	memset(szTmp,0,sizeof(szTmp));
	m_lstPack.GetItemText(nItem, 4, szTmp, 4096);
	m_pTool.LRtrim(szTmp);
	if (strcmp(szTmp,"�ַ�")==0)
	{
		m_combType.SetCurSel(0);
		m_edtDec.EnableWindow(false);
	}
	else if (strcmp(szTmp,"����")==0)
	{
		m_combType.SetCurSel(1);
		m_edtDec.EnableWindow(true);
	}
	else if (strcmp(szTmp,"����")==0)
	{
		m_combType.SetCurSel(2);
		m_edtDec.EnableWindow(false);
	}
	
	memset(szTmp,0,sizeof(szTmp));
	m_lstPack.GetItemText(nItem, 5, szTmp, 4096);
	m_pTool.LRtrim(szTmp);
	m_edtDec.SetWindowText(szTmp);
	
	memset(szTmp,0,sizeof(szTmp));
	m_lstPack.GetItemText(nItem, 6, szTmp, 4096);
	m_pTool.LRtrim(szTmp);
	if (strcmp(szTmp,"��")==0)
	{
		m_combAlign.SetCurSel(0);
	}
	else if (strcmp(szTmp,"��")==0)
	{
		m_combAlign.SetCurSel(1);
	}

	GetDlgItem(IDC_RADIO_MODI_PACK)->PostMessage(BM_CLICK);


}
BOOL CTestcommDlg:: PreTranslateMessage( MSG* pMsg )
{
	return CDialog::PreTranslateMessage(pMsg);
}

void CTestcommDlg::OnRadioXml() 
{
	// TODO: Add your control notification handler code here
	m_edtSeg.EnableWindow(false);
	m_combSelectHead.EnableWindow(false);
	m_combPwdFlag.EnableWindow(true);
	m_combZip.EnableWindow(true);
	m_chkFill.EnableWindow(false);

//	m_edtLength.SetWindowText("0");
    m_edtLength.EnableWindow(false);
    m_combType.SetCurSel(0);
	m_combType.EnableWindow(false);
//	m_edtDec.SetWindowText("0");
	m_edtDec.EnableWindow(false);
	m_combAlign.EnableWindow(false);
	m_edtValue.SetWindowText("");
	m_edtName.SetWindowText("");
}

int CTestcommDlg::showXmlPack(CXmlPack *xmlpack,CTreeCtrl *pTreeCtrl, HTREEITEM hParent)
{
	CNode *aa = NULL;
	xmlpack->gotoHead();
	aa = xmlpack->m_xml.getParentItem();
	if (aa == NULL)
	{
		return -1;
	}
	showXmlPackNode(aa, xmlpack,pTreeCtrl,hParent);
	return 0;

// 	HTREEITEM	hHead; 
// 	HTREEITEM	hPack;
// 	hHead = pTreeCtrl->InsertItem("head", hParent);
// 	xmlpack->gotoHead();
// 	aa = xmlpack->m_xml.getFirstChildItem();
// 	while (aa!=NULL)
// 	{
// // 		string name = aa->getName();
// // 		string value = aa->getValue();
// // 		char *tmpchar = (char *)malloc(name.length()+value.length()+3);
// // 		memset(tmpchar,0,name.length()+value.length()+3);
// // 		sprintf(tmpchar,"%s:%s",name.c_str(),value.c_str()) ;
// // 		pTreeCtrl->InsertItem(tmpchar,hHead);
// // 		free(tmpchar);
// 		showXmlPackNode(aa, xmlpack,pTreeCtrl,hHead);
// 		aa = xmlpack->m_xml.getNextSibling();
// 	}
// 	hPack = pTreeCtrl->InsertItem("pack", hParent);
// 	xmlpack->gotoPack();
// 	aa = xmlpack->m_xml.getFirstChildItem();
// 	while (aa!=NULL)
// 	{
// // 		string name = aa->getName();
// // 		string value = aa->getValue();
// // 		char *tmpchar = (char *)malloc(name.length()+value.length()+3);
// // 		memset(tmpchar,0,name.length()+value.length()+3);
// // 		sprintf(tmpchar,"%s:%s",name.c_str(),value.c_str()) ;
// // 		pTreeCtrl->InsertItem(tmpchar,hPack);
// // 		free(tmpchar);
// 		showXmlPackNode(aa, xmlpack,pTreeCtrl,hPack);
// 
// 		aa = xmlpack->m_xml.getNextSibling();
// 	}
	return 0;
}

int CTestcommDlg::showXmlPackNode(CNode *node, CXmlPack *xmlpack,CTreeCtrl *pTreeCtrl, HTREEITEM hParent)
{
	xmlpack->m_xml.setCurrentNode(node);
	HTREEITEM	hPack;
	HTREEITEM	hPackNext;
	hPack = pTreeCtrl->InsertItem((node->getName()+":"+node->getValue()).c_str(),hParent);
	CNode *tmpnode = xmlpack->m_xml.getFirstChildItem();
	while (tmpnode != NULL)
	{
		showXmlPackNode(tmpnode, xmlpack,pTreeCtrl,hPack);
		tmpnode = xmlpack->m_xml.getNextSibling();
	}
	xmlpack->m_xml.setCurrentNode(node);
	return 0;
}

void CTestcommDlg::OnChangeEditSendNum() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	int sendNum=0;
	CString cssendNum;
	UpdateData();
	m_edtSendNum.GetWindowText(cssendNum);
	sendNum = atoi(cssendNum);
	if (sendNum >10000)
	{
		GetDlgItem(IDC_STATIC_SENDNUM)->SetWindowText("����ʱ��");
	}
	else
	{
		GetDlgItem(IDC_STATIC_SENDNUM)->SetWindowText("���ʹ���");
	}
}

void CTestcommDlg::OnCheckConnect() 
{
	// TODO: Add your control notification handler code here
	if (m_chkConnect.GetCheck() == 1)
	{
		GetDlgItem(IDC_CHECK_CONNECT)->SetWindowText("������");
	}
	else
	{
		GetDlgItem(IDC_CHECK_CONNECT)->SetWindowText("������");
	}
}
bool CTestcommDlg::IsDouble(char *str)
{
	int len;
	int sop=0;
	int ret=0;
	len = strlen(str);
	if (len<1) return true;
	for (int i=0;i<len;i++)
	{
		if (str[i]=='-' && i==0)
		{
			continue;
		}
		if (str[i]=='.')
		{
			ret++;
			if (ret>1)
				return false;
		}
		if ((str[i]<'0' || str[i]>'9') && str[i]!='.')
		{
			return false;
		}
		if (str[i]=='.')
		{
			if (i==0 || i==len-1)
			{
				return false;
			}
			sop++;
		}
	}
	if (sop>1)
	{
		return false;
	}
	return true;	
}