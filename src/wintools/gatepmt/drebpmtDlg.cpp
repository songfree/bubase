// drebpmtDlg.cpp : implementation file
//

#include "stdafx.h"
#include "drebpmt.h"
#include "drebpmtDlg.h"
#include "VarConfig.h"
#include "ViewErrInfo.h"
#include "BF_Date_Time.h"
#include "GateHead.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static UINT indicators[] =
{
	//����С���
	ID_SEPARATOR,           // status line indicator
	ID_SEPARATOR,           // status line indicator
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
// CDrebpmtDlg dialog

CDrebpmtDlg::CDrebpmtDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDrebpmtDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDrebpmtDlg)
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

void CDrebpmtDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDrebpmtDlg)
	DDX_Control(pDX, IDC_EDIT_API_XML, m_edtXmlApi);
	DDX_Control(pDX, IDC_BUTTON_APIFILENAME, m_btnApiXml);
	DDX_Control(pDX, IDC_EDIT_LOGINBODY, m_edtLoginBody);
	DDX_Control(pDX, IDC_EDIT_LOGINTX, m_edtLoginTx);
	DDX_Control(pDX, IDC_EDIT_RCVNUM_PUSH, m_edtRcvPushNum);
	DDX_Control(pDX, IDC_EDIT_RRTIME, m_edtRRTime);
	DDX_Control(pDX, IDC_EDIT_LOGLEVEL, m_edtLogLevel);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDC_LIST_SC, m_lstScript);
	DDX_Control(pDX, IDC_EDIT_TRYNUM, m_edtTryNum);
	DDX_Control(pDX, IDC_EDIT_TOTALTHREAD, m_edtTotalThread);
	DDX_Control(pDX, IDC_EDIT_TIMESEP, m_edtTimeSep);
	DDX_Control(pDX, IDC_EDIT_TIMEOUT, m_edtTimeOut);
	DDX_Control(pDX, IDC_EDIT_THREADNUM, m_edtThreadNum);
	DDX_Control(pDX, IDC_EDIT_SERVICEFUNC, m_edtFuncNo);
	DDX_Control(pDX, IDC_EDIT_SENDNUM, m_edtSendNum);
	DDX_Control(pDX, IDC_EDIT_SENDBYTE, m_edtSendByte);
	DDX_Control(pDX, IDC_EDIT_SCFILE, m_edtFile);
	DDX_Control(pDX, IDC_EDIT_RUNTIME, m_edtRunTime);
	DDX_Control(pDX, IDC_EDIT_RCVNUM, m_edtRcvNum);
	DDX_Control(pDX, IDC_EDIT_RCVBYTE, m_edtRcvByte);
	DDX_Control(pDX, IDC_EDIT_MINRESP, m_edtMinResp);
	DDX_Control(pDX, IDC_EDIT_MAXRESP, m_edtMaxResp);
	DDX_Control(pDX, IDC_EDIT_MAXNUMSEC, m_edtMaxNumSec);
	DDX_Control(pDX, IDC_EDIT_ERRNUM, m_edtErrNum);
	DDX_Control(pDX, IDC_EDIT_CURTIME, m_edtCurTime);
	DDX_Control(pDX, IDC_EDIT_CURNUMSEC, m_edtCurNumSec);
	DDX_Control(pDX, IDC_EDIT_BEGINTIME, m_edtBeginTime);
	DDX_Control(pDX, IDC_EDIT_AVRESP, m_edtAvResp);
	DDX_Control(pDX, IDC_EDIT_AVR_SENDBYTE, m_edtAvrSendRate);
	DDX_Control(pDX, IDC_EDIT_AVR_RCVBYTE, m_edtAvrRcvRate);
	DDX_Control(pDX, IDC_EDIT_AVNUMSEC, m_edtAvNumSec);
	DDX_Control(pDX, IDC_EDIT_ALLTIME, m_edtAllTime);
	DDX_Control(pDX, IDC_EDIT_ALLTHREAD, m_edtAllThread);
	DDX_Control(pDX, IDC_CHECK_AUTO, m_chkAuto);
	DDX_Control(pDX, IDC_BUTTON_STOP, m_btnStop);
	DDX_Control(pDX, IDC_BUTTON_RESTAR, m_btnRestar);
	DDX_Control(pDX, IDC_BUTTON_FILE, m_btnFile);
	DDX_Control(pDX, IDC_BUTTON_DEL, m_btnDel);
	DDX_Control(pDX, IDC_BUTTON_ADD, m_btnAdd);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDrebpmtDlg, CDialog)
	//{{AFX_MSG_MAP(CDrebpmtDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_VIEW, OnButtonView)
	ON_BN_CLICKED(IDC_BUTTON_STOP, OnButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_SETVAR, OnButtonSetvar)
	ON_BN_CLICKED(IDC_BUTTON_RESTAR, OnButtonRestar)
	ON_BN_CLICKED(IDC_BUTTON_SAVETEST, OnButtonSavetest)
	ON_BN_CLICKED(IDC_BUTTON_OPENTEST, OnButtonOpentest)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_SAVELIST, OnButtonSavelist)
	ON_BN_CLICKED(IDC_BUTTON_OPENLIST, OnButtonOpenlist)
	ON_BN_CLICKED(IDC_BUTTON_FILE, OnButtonFile)
	ON_WM_TIMER()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_SC, OnDblclkListSc)
	ON_BN_CLICKED(IDC_BUTTON_FILELOGIN, OnButtonFilelogin)
	ON_BN_CLICKED(IDC_BUTTON_APIFILENAME, OnButtonApifilename)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDrebpmtDlg message handlers

BOOL CDrebpmtDlg::OnInitDialog()
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
	if (!m_statusbar.Create(this) ||
		!m_statusbar.SetIndicators(indicators,
		sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_statusbar.SetPaneInfo(0,0,SBPS_STRETCH,80);
	m_statusbar.SetPaneText(0,"  ");
	m_statusbar.SetPaneInfo(1,1,SBPS_NORMAL,150);
	//�����״̬��������ʾ�ڶԻ�����
    RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,0);

	m_pDate.Update();
	m_statusbar.SetPaneText(1,m_pDate.ToStringDT17().c_str());
	if (!Init())
	{
		return false;
	}
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDrebpmtDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CDrebpmtDlg::OnPaint() 
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
HCURSOR CDrebpmtDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CDrebpmtDlg::OnOK() 
{
	// TODO: Add extra validation here
	int sciNum = m_lstScript.GetItemCount();
	if (sciNum <1)
	{
		MessageBox("�޽ű�����","��ʾ");
		m_lstScript.SetFocus();
		return;
	}
	m_pShareData.Clear();
	//������������ShareData��SERVER_PARA�ṹ��
	SaveParameter();
	
	_THREAD_DATA_PARA tpara;
	memset(&tpara,0,sizeof(_THREAD_DATA_PARA));

	//��ȡ�ű������߳�
	FILE *fp=NULL;
	int i;
	char tmpchar[500];
	char filename[400];

	char loginData[DREBBUFSIZE];
	int loginDatalen;

	char sendData[DREBBUFSIZE];
	int sendDatalen;
	m_pShareData.m_nTotalThreadNum = 0;

	int itemCount = m_lstScript.GetItemCount();
	//�ͷ�ԭ�����߳�
	//FreeThread();
	for ( i=0;i<itemCount;i++)
	{
		memset(tmpchar,0,sizeof(tmpchar));
		m_lstScript.GetItemText(i, 8, tmpchar, 300);
		m_pShareData.m_nTotalThreadNum+=atoi(tmpchar);
	}
	if (m_pShareData.m_nTotalThreadNum > MAXTHREADNUM)
	{
		sprintf(tmpchar,"���߳������ڹ涨��%d��Ŀ�����С�߳���Ŀ",MAXTHREADNUM);
		MessageBox(tmpchar,"��ʾ");
		m_lstScript.SetFocus();
		return;
	}
	m_pShareData.m_bIsExit = false;
	m_pShareData.m_bIsRun = false;
	m_curThreadNum = 0;
	for ( i=0;i<itemCount;i++)
	{
		memset(tmpchar,0,sizeof(tmpchar));
		m_lstScript.GetItemText(i, 1, tmpchar, 300);//���ص�ַ
		tpara.sApiXml = tmpchar;
		
		memset(tmpchar,0,sizeof(tmpchar));
		m_lstScript.GetItemText(i, 2, tmpchar, 10);//��¼������
		tpara.nLoginTx = atoi(tmpchar);


		memset(filename,0,sizeof(filename));
		m_lstScript.GetItemText(i, 3, filename, 300);//��¼�ļ���
		fp = fopen(filename,"rb");
		if (fp == NULL)
		{
			sprintf(tmpchar,"�򿪵�¼�ű��ļ�[%s]ʧ��",filename);
			MessageBox(tmpchar,"��ʾ");
			m_lstScript.SetFocus();
			m_pShareData.m_bIsExit = true;
			return;
		}
		memset(loginData,0,sizeof(loginData));
		loginDatalen = fread(loginData,1,CLIENTDATASIZE,fp);
		fclose(fp);

		memset(tmpchar,0,sizeof(tmpchar));
		m_lstScript.GetItemText(i, 4, tmpchar, 10);//������
		tpara.nServiceNo = atoi(tmpchar);


		memset(filename,0,sizeof(filename));
		m_lstScript.GetItemText(i, 5, filename, 300);//�ļ���
		fp = fopen(filename,"rb");
		if (fp == NULL)
		{
			sprintf(tmpchar,"�򿪷�ѹ�ű��ļ�[%s]ʧ��",filename);
			MessageBox(tmpchar,"��ʾ");
			m_lstScript.SetFocus();
			m_pShareData.m_bIsExit = true;
			return;
		}
		memset(sendData,0,sizeof(sendData));
		sendDatalen = fread(sendData,1,CLIENTDATASIZE,fp);
		fclose(fp);

		
		memset(tmpchar,0,sizeof(tmpchar));
		m_lstScript.GetItemText(i, 6, tmpchar, 40);//��ʼʱ��
		tpara.nStartTime = atoi(tmpchar)*1000;
		
		memset(tmpchar,0,sizeof(tmpchar));
		m_lstScript.GetItemText(i, 7, tmpchar, 40);//�߳���
		tpara.nThreadNum = atoi(tmpchar);


		for (int j=m_curThreadNum;j<m_curThreadNum+tpara.nThreadNum;j++)
		{
			m_pThread[j]= new CTestThread();
			if (m_pThread[j] == NULL)
			{
				sprintf(tmpchar,"��Դ����");
				MessageBox(tmpchar,"��ʾ");
				m_lstScript.SetFocus();
				m_pShareData.m_bIsExit = true;
				FreeThread();
				return;
			}
			m_pThread[j]->m_nID = j;
			m_pThread[j]->SetData(sendData,sendDatalen,loginData,loginDatalen,&m_pShareData,tpara);

			if (m_pThread[j]->ThreadInit()<0)
			{
				MessageBox("ThreadInitʧ��");
				FreeThread();
				return ;
			}
			

		}
		m_curThreadNum +=tpara.nThreadNum;

	}
    sprintf(tmpchar, "%d", m_curThreadNum);
    m_edtTotalThread.SetWindowText(tmpchar);

    m_pDate.Update();
    sprintf(tmpchar, "log\\%s%s", m_pDate.ToStringD8().c_str(), "gatepmt.log");
    UNLINK(tmpchar);
	for (int jj=0;jj<m_curThreadNum;jj++)
	{
		m_pThread[jj]->CreateThread();//�����߳�
		//m_pThread[jj]->m_pGetPush.CreateThread();
	}
	m_pShareData.m_nTotalThreadNum = m_curThreadNum;
	m_pShareData.m_bIsRun = true;
	m_pShareData.m_nRunTime = m_pDate.GetTickCount();
	m_lastAvr =0;
	m_nMaxAvr = 0;
	if (m_chkAuto.GetCheck()==1)//�Զ�����ֹͣ���߳�
	{
		m_btnRestar.EnableWindow(false);
		m_timerID2 = SetTimer(ONTIMER2,2,NULL);//��ʱ��
	}
	else
	{
		m_btnRestar.EnableWindow(true);
	}
	m_timerID1 = SetTimer(ONTIMER1,m_time_auto,NULL);//��ʱ��
	if (m_pShareData.m_pServerPara.runtime != 0)
	{
		ConvSecTime(m_pShareData.m_pServerPara.runtime/1000,tmpchar);
	}
	else
	{
		sprintf(tmpchar,"δ��ʱ");
	}
	m_edtAllTime.SetWindowText(tmpchar);
	m_btnStop.EnableWindow(true);
	m_btnOk.EnableWindow(false);
//	CDialog::OnOK();
}

void CDrebpmtDlg::OnButtonView() 
{
	// TODO: Add your control notification handler code here
	CViewErrInfo dlg;
	dlg.DoModal();
}

void CDrebpmtDlg::OnButtonStop() 
{
	// TODO: Add your control notification handler code here
	KillTimer(m_timerID2);
	m_pShareData.m_bIsExit = true;
	int j;
	for (j=0;j<m_curThreadNum;j++)
	{
		m_pThread[j]->Join();
	}
	m_btnOk.EnableWindow(true);
	m_btnStop.EnableWindow(false);
	SLEEP(1000);
	KillTimer(m_timerID1);
	
	//�ͷ��߳�
	//FreeThread();
	//�����ʼ����
	SaveVarInitData();
}

void CDrebpmtDlg::OnButtonSetvar() 
{
	// TODO: Add your control notification handler code here
	CVarConfig dlg;
	dlg.DoModal();
}

void CDrebpmtDlg::OnButtonRestar() 
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

void CDrebpmtDlg::OnButtonSavetest() 
{
	// TODO: Add your control notification handler code here
	int i;
	char tmpchar[500];
	FILE *fp;
	char filename[300];
	char savefilename[400];
	
	CBF_Xml xmlpack;
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
	//������·�����û�ȥ
	SetCurrentDirectory(savefilename);
	strcpy(savefilename,ss.GetBuffer(ss.GetLength()));
	

	ss = m_statusbar.GetPaneText(1);//����ʱ��

	xmlpack.SetNodeAttribute("/package/pack",false,"����ʱ��",false,ss.GetBuffer(ss.GetLength()));
	
	m_edtTimeOut.GetWindowText(ss);//���ճ�ʱ(����)
	xmlpack.SetNodeValueByPath("/package/pack/���ճ�ʱ(����)",false,ss.GetBuffer(ss.GetLength()));
	
	m_edtTimeSep.GetWindowText(ss);//���ͼ��
	xmlpack.SetNodeValueByPath("/package/pack/���ͼ��(����)",false,ss.GetBuffer(ss.GetLength()));
	
	
	m_edtRunTime.GetWindowText(ss);//����ʱ��
	xmlpack.SetNodeValueByPath("/package/pack/����ʱ��(��)",false,ss.GetBuffer(ss.GetLength()));
	
	m_edtAllThread.GetWindowText(ss);//���߳���
	xmlpack.SetNodeValueByPath("/package/pack/���߳���",false,ss.GetBuffer(ss.GetLength()));
	
	m_edtSendNum.GetWindowText(ss);//������
	xmlpack.SetNodeValueByPath("/package/pack/������",false,ss.GetBuffer(ss.GetLength()));
	
	m_edtRcvNum.GetWindowText(ss);//������
	xmlpack.SetNodeValueByPath("/package/pack/������",false,ss.GetBuffer(ss.GetLength()));
	
	m_edtErrNum.GetWindowText(ss);//������
	xmlpack.SetNodeValueByPath("/package/pack/������",false,ss.GetBuffer(ss.GetLength()));
	
	m_edtMinResp.GetWindowText(ss);//��С��Ӧʱ��
	xmlpack.SetNodeValueByPath("/package/pack/��С��Ӧʱ��",false,ss.GetBuffer(ss.GetLength()));
	
	m_edtMaxResp.GetWindowText(ss);//�����Ӧʱ��
	xmlpack.SetNodeValueByPath("/package/pack/�����Ӧʱ��",false,ss.GetBuffer(ss.GetLength()));
	
	m_edtSendByte.GetWindowText(ss);//�����ֽ���
	xmlpack.SetNodeValueByPath("/package/pack/�����ֽ���",false,ss.GetBuffer(ss.GetLength()));
	
	m_edtRcvByte.GetWindowText(ss);//�����ֽ���
	xmlpack.SetNodeValueByPath("/package/pack/�����ֽ���",false,ss.GetBuffer(ss.GetLength()));
	
	m_edtAvResp.GetWindowText(ss);//ƽ����Ӧʱ��
	xmlpack.SetNodeValueByPath("/package/pack/ƽ����Ӧʱ��",false,ss.GetBuffer(ss.GetLength()));
	
	m_edtAvrSendRate.GetWindowText(ss);//ƽ����������
	xmlpack.SetNodeValueByPath("/package/pack/ƽ����������",false,ss.GetBuffer(ss.GetLength()));
	
	m_edtAvrRcvRate.GetWindowText(ss);//ƽ��������
	xmlpack.SetNodeValueByPath("/package/pack/ƽ��������",false,ss.GetBuffer(ss.GetLength()));
	
	m_edtMaxNumSec.GetWindowText(ss);//��ֵÿ�����
	xmlpack.SetNodeValueByPath("/package/pack/��ֵÿ�����",false,ss.GetBuffer(ss.GetLength()));
	
	
	m_edtCurNumSec.GetWindowText(ss);//��ǰÿ�����
	xmlpack.SetNodeValueByPath("/package/pack/��ǰÿ�����",false,ss.GetBuffer(ss.GetLength()));
	
	m_edtAvNumSec.GetWindowText(ss);//ƽ��ÿ�����
	xmlpack.SetNodeValueByPath("/package/pack/ƽ��ÿ�����",false,ss.GetBuffer(ss.GetLength()));
	

	m_edtRcvPushNum.GetWindowText(ss);//���ͱ���
	xmlpack.SetNodeValueByPath("/package/pack/���ͱ���",false,ss.GetBuffer(ss.GetLength()));
	xmlpack.ToFile(savefilename);
}

void CDrebpmtDlg::OnButtonOpentest() 
{
	// TODO: Add your control notification handler code here
	int i;
	char tmpchar[500];
	FILE *fp;
	char filename[300];
	char savefilename[400];
	
	CBF_Xml xmlpack;
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
	//������·�����û�ȥ
	SetCurrentDirectory(savefilename);
	strcpy(savefilename,ss.GetBuffer(ss.GetLength()));
	if (!xmlpack.FromFile(savefilename))
	{
		MessageBox("�ļ���ʽ�Ƿ�");
		return;
	}
	std::string svalue;
	xmlpack.GetNodeValueByPath("/package/pack/���ճ�ʱ(����)",false,svalue);
	m_edtTimeOut.SetWindowText(svalue.c_str());
	xmlpack.GetNodeValueByPath("/package/pack/���ͼ��(����)",false,svalue);
	m_edtTimeSep.SetWindowText(svalue.c_str());
	xmlpack.GetNodeValueByPath("/package/pack/����ʱ��(��)",false,svalue);
	m_edtRunTime.SetWindowText(svalue.c_str());
	m_edtAllTime.SetWindowText(svalue.c_str());
	
	xmlpack.GetNodeValueByPath("/package/pack/���߳���",false,svalue);
	m_edtAllThread.SetWindowText(svalue.c_str());
	
	xmlpack.GetNodeValueByPath("/package/pack/������",false,svalue);
	m_edtSendNum.SetWindowText(svalue.c_str());
	xmlpack.GetNodeValueByPath("/package/pack/������",false,svalue);
	m_edtRcvNum.SetWindowText(svalue.c_str());
	xmlpack.GetNodeValueByPath("/package/pack/������",false,svalue);
	m_edtErrNum.SetWindowText(svalue.c_str());
	xmlpack.GetNodeValueByPath("/package/pack/��С��Ӧʱ��",false,svalue);
	m_edtMinResp.SetWindowText(svalue.c_str());
	xmlpack.GetNodeValueByPath("/package/pack/�����Ӧʱ��",false,svalue);
	m_edtMaxResp.SetWindowText(svalue.c_str());
	xmlpack.GetNodeValueByPath("/package/pack/�����ֽ���",false,svalue);
	m_edtSendByte.SetWindowText(svalue.c_str());
	xmlpack.GetNodeValueByPath("/package/pack/�����ֽ���",false,svalue);
	m_edtRcvByte.SetWindowText(svalue.c_str());
	xmlpack.GetNodeValueByPath("/package/pack/ƽ����Ӧʱ��",false,svalue);
	m_edtAvResp.SetWindowText(svalue.c_str());
	xmlpack.GetNodeValueByPath("/package/pack/ƽ����������",false,svalue);
	m_edtAvrSendRate.SetWindowText(svalue.c_str());
	
	xmlpack.GetNodeValueByPath("/package/pack/ƽ��������",false,svalue);
	m_edtAvrRcvRate.SetWindowText(svalue.c_str());
	xmlpack.GetNodeValueByPath("/package/pack/��ֵÿ�����",false,svalue);
	m_edtMaxNumSec.SetWindowText(svalue.c_str());
	xmlpack.GetNodeValueByPath("/package/pack/��ǰÿ�����",false,svalue);
	m_edtCurNumSec.SetWindowText(svalue.c_str());
	xmlpack.GetNodeValueByPath("/package/pack/ƽ��ÿ�����",false,svalue);
	m_edtAvNumSec.SetWindowText(svalue.c_str());
	xmlpack.GetNodeValueByPath("/package/pack/���ͱ���",false,svalue);
	m_edtRcvPushNum.SetWindowText(svalue.c_str());
}

void CDrebpmtDlg::OnButtonAdd() 
{
	// TODO: Add your control notification handler code here
	CString cspmfilename;
	CString begintime;
	CString threadnum;
	CString apixml;
	CString serviceno;
	CString cslogintx;
	CString csloginbody;
	
	char tmpchar[400];
	if (m_edtFile.GetWindowTextLength()<1)
	{
		m_edtFile.SetFocus();
		MessageBox("�������ѡ��ѹ�ű�","��ʾ",MB_OK);
		return ;
	}
	m_edtFile.GetWindowText(cspmfilename);
	//�ļ�����
	struct_stat f_stat;
	FSTAT(cspmfilename,&f_stat);
	int filelen=f_stat.st_size;//�ļ�����
	if (filelen >=CLIENTDATASIZE)
	{
		m_edtFile.SetFocus();
		sprintf(tmpchar,"��ѹ�ű��ļ���С���ܳ���%d",CLIENTDATASIZE);
		MessageBox(tmpchar,"��ʾ",MB_OK);
		return ;
	}
	if (filelen <1)
	{
		m_edtFile.SetFocus();
		MessageBox("��ѹ�ű��ļ������ڻ�Ϊ���ļ�","��ʾ",MB_OK);
		return ;
	}

	if (m_edtLoginBody.GetWindowTextLength()<1)
	{
		m_edtLoginBody.SetFocus();
		MessageBox("�������ѡ�����ص�¼�ű�","��ʾ",MB_OK);
		return ;
	}
	m_edtLoginBody.GetWindowText(csloginbody);
	//�ļ�����
	FSTAT(csloginbody,&f_stat);
	filelen=f_stat.st_size;//�ļ�����
	if (filelen >=CLIENTDATASIZE)
	{
		m_edtLoginBody.SetFocus();
		sprintf(tmpchar,"���ص�¼�ű��ļ���С���ܳ���%d",CLIENTDATASIZE);
		MessageBox(tmpchar,"��ʾ",MB_OK);
		return ;
	}
	if (filelen <1)
	{
		m_edtLoginBody.SetFocus();
		MessageBox("��¼�ű��ļ������ڻ�Ϊ���ļ�","��ʾ",MB_OK);
		return ;
	}
	if (m_edtBeginTime.GetWindowTextLength()<1)
	{
		m_edtBeginTime.SetFocus();
		MessageBox("��������ʼʱ��","��ʾ",MB_OK);
		return ;
	}
	m_edtBeginTime.GetWindowText(begintime);
	if (m_edtThreadNum.GetWindowTextLength()<1)
	{
		m_edtThreadNum.SetFocus();
		MessageBox("�������߳���","��ʾ",MB_OK);
		return ;
	}
	m_edtThreadNum.GetWindowText(threadnum);
	m_edtXmlApi.GetWindowText(apixml);
	if (m_edtXmlApi.GetWindowTextLength()<1)
	{
		m_edtXmlApi.SetFocus();
		MessageBox("�������ѡ������API������xml�ļ�","��ʾ",MB_OK);
		return ;
	}
	
	if (m_edtLoginTx.GetWindowTextLength()<1)
	{
		m_edtLoginTx.SetFocus();
		MessageBox("�������¼������","��ʾ",MB_OK);
		return ;
	}
	m_edtLoginTx.GetWindowText(cslogintx);

	if (m_edtFuncNo.GetWindowTextLength()<1)
	{
		m_edtFuncNo.SetFocus();
		MessageBox("�����뷢ѹ������","��ʾ",MB_OK);
		return ;
	}
	m_edtFuncNo.GetWindowText(serviceno);
	
	int recNum = m_lstScript.GetItemCount();
	
	sprintf(tmpchar,"%d",recNum+1);
	int nItem = m_lstScript.InsertItem(recNum,tmpchar);
	
	m_lstScript.SetItemText(nItem,1,apixml);
	m_lstScript.SetItemText(nItem,2,cslogintx);
	m_lstScript.SetItemText(nItem,3,csloginbody);
	m_lstScript.SetItemText(nItem,4,serviceno);
	m_lstScript.SetItemText(nItem,5,cspmfilename);
	m_lstScript.SetItemText(nItem,6,begintime);
	m_lstScript.SetItemText(nItem,7,threadnum);
	SaveParameter();
}
struct MyStruct 
{
	int ID;
	bool operator < (MyStruct item)  /*ע�����ء�*/
    {
        if(ID<item.ID) //����
		{
            return false;
		}
		else
		{
			return true;
		}
    };
};
void CDrebpmtDlg::OnButtonDel() 
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
        MessageBox("��ѡ������һ����¼","��ʾ");
		m_lstScript.SetFocus();
		return;
	}
	std::list<MyStruct>list_iItem;
	POSITION pos = m_lstScript.GetFirstSelectedItemPosition();
	if   (pos   ==   NULL)  
	{
		MessageBox("��ѡ������һ����¼","��ʾ");
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

void CDrebpmtDlg::OnButtonSavelist() 
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
	CBF_Xml xmlpack;
	CString ss;
	int ret;
	
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
		//������·�����û�ȥ
		SetCurrentDirectory(savefilename);
		strcpy(savefilename,ss.GetBuffer(ss.GetLength()));
	}
	else
	{
		return;
	}

	CXmlNode *packnode = xmlpack.SetNodeValueByPath("/package/pack",false,"");
	if (packnode == NULL)
	{
		return;
	}
	for ( i=0;i<itemCount;i++)
	{
		memset(filename,0,sizeof(filename));
		m_lstScript.GetItemText(i, 1, filename, 300);//�ļ���
		fp = fopen(filename,"rb");
		if (fp == NULL)
		{
			sprintf(tmpchar,"�򿪽ű��ļ�[%s]ʧ��",filename);
			MessageBox(tmpchar,"��ʾ");
			m_lstScript.SetFocus();
			m_pShareData.m_bIsExit = true;
			return;
		}
		CXmlNode *addnode = packnode->AddChildNode("LIST");
		if (addnode!= NULL)
		{

			addnode->SetAttribute("apixml",false,filename);

			memset(tmpchar,0,sizeof(tmpchar));
			m_lstScript.GetItemText(i, 2, tmpchar, 8);//��¼������
			addnode->SetAttribute("��¼������",false,tmpchar);

			memset(tmpchar,0,sizeof(tmpchar));
			m_lstScript.GetItemText(i, 3, tmpchar, 300);//��¼����
			addnode->SetAttribute("��¼����",false,tmpchar);

			memset(tmpchar,0,sizeof(tmpchar));
			m_lstScript.GetItemText(i, 4, tmpchar, 8);//��ѹ������
			addnode->SetAttribute("��ѹ������",false,tmpchar);

			memset(tmpchar,0,sizeof(tmpchar));
			m_lstScript.GetItemText(i, 5, tmpchar, 300);//��ѹ����
			addnode->SetAttribute("��ѹ����",false,tmpchar);

			memset(tmpchar,0,sizeof(tmpchar));
			m_lstScript.GetItemText(i, 6, tmpchar, 10);//��ʼʱ��
			addnode->SetAttribute("��ʼʱ��",false,tmpchar);

			memset(tmpchar,0,sizeof(tmpchar));
			m_lstScript.GetItemText(i, 7, tmpchar, 5);//�߳���
			addnode->SetAttribute("�߳���",false,tmpchar);

			
		}
	}
	xmlpack.ToFile(savefilename);
}

void CDrebpmtDlg::OnButtonOpenlist() 
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
	
	CBF_Xml xmlpack;
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
	//������·�����û�ȥ
	SetCurrentDirectory(savefilename);
	strcpy(savefilename,ss.GetBuffer(ss.GetLength()));
	m_lstScript.DeleteAllItems();
	
	if (!xmlpack.FromFile(savefilename))
	{
		MessageBox("�ļ��Ƿ�","��ʾ");
		return;
	}
	
	int recNum;
	int nItem ;
	string sss;
	CXmlNode *tmpnode = xmlpack.GetNodeByPath("/package/pack");
	if (tmpnode == NULL)
	{
		return;
	}
	tmpnode = (CXmlNode *)tmpnode->GetFirstChild();
	while (tmpnode != NULL)
	{
		recNum = m_lstScript.GetItemCount();
		
		sprintf(tmpchar,"%d",recNum+1);
		nItem = m_lstScript.InsertItem(recNum,tmpchar);

		tmpnode->GetAttribute("apixml",false,sss);
		m_lstScript.SetItemText(nItem,1,sss.c_str());
		
		tmpnode->GetAttribute("��¼������",false,sss);
		m_lstScript.SetItemText(nItem,2,sss.c_str());

		tmpnode->GetAttribute("��¼����",false,sss);
		m_lstScript.SetItemText(nItem,3,sss.c_str());


		tmpnode->GetAttribute("��ѹ������",false,sss);
		m_lstScript.SetItemText(nItem,4,sss.c_str());
		
		tmpnode->GetAttribute("��ѹ����",false,sss);
		m_lstScript.SetItemText(nItem,5,sss.c_str());

		tmpnode->GetAttribute("��ʼʱ��",false,sss);
		m_lstScript.SetItemText(nItem,6,sss.c_str());
		tmpnode->GetAttribute("�߳���",false,sss);
		m_lstScript.SetItemText(nItem,7,sss.c_str());

				
		tmpnode = (CXmlNode *)tmpnode->getNextSibling();
	}
}

void CDrebpmtDlg::OnButtonFile() 
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
	//������·�����û�ȥ
	SetCurrentDirectory(filename);
	UpdateData(false);
	m_edtThreadNum.SetFocus();
}

bool CDrebpmtDlg::Init()
{
	m_edtTimeOut.SetLimitText(6);
	m_edtRRTime.SetLimitText(6);
	m_edtTimeSep.SetLimitText(6);
	m_edtRunTime.SetLimitText(4);
	m_edtBeginTime.SetLimitText(4);
	m_edtThreadNum.SetLimitText(5);	
	m_edtTryNum.SetLimitText(3);
	
	m_edtFuncNo.SetLimitText(10);
	m_edtLogLevel.SetLimitText(2);

	
	LoadCfg();
	m_edtBeginTime.SetWindowText("0");
	m_edtThreadNum.SetWindowText("1");
	
	
	m_lstScript.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_HEADERDRAGDROP);
	m_lstScript.InsertColumn(0, "���", LVCFMT_CENTER, 50);
	m_lstScript.InsertColumn(1, "����API�ļ�", LVCFMT_CENTER, 300);
	m_lstScript.InsertColumn(2, "��¼����", LVCFMT_CENTER, 60);
	m_lstScript.InsertColumn(3, "��¼�ű�", LVCFMT_CENTER, 300);
	m_lstScript.InsertColumn(4, "��ѹ����", LVCFMT_CENTER, 60);
	m_lstScript.InsertColumn(5, "��ѹ�ű�", LVCFMT_CENTER, 300);
	m_lstScript.InsertColumn(6, "����ʱ��", LVCFMT_CENTER, 60);
	m_lstScript.InsertColumn(7, "�߳���", LVCFMT_CENTER, 60);


	m_btnStop.EnableWindow(false);
	m_bisMax = false;
	for (int jj=0;jj<MAXTHREADNUM;jj++)
	{
		m_pThread[jj] = NULL;//�����߳�
	}
	return true;

}
void CDrebpmtDlg::FreeThread()
{
	if (m_pShareData.m_nTotalThreadNum>0)
	{
		m_pShareData.m_bIsExit = true;
		for (int jjj=0;jjj<m_pShareData.m_nTotalThreadNum;jjj++)
		{
			if (m_pThread[jjj]!=NULL)
			{
				m_pThread[jjj]->Join();
				delete m_pThread[jjj] ;
				m_pThread[jjj] = NULL;
			}
		}
	}
}

void CDrebpmtDlg::SaveVarInitData()
{
	CBF_PMutex pp(&m_mutex);
	CBF_Xml   xmlpack;
 	if (!xmlpack.FromFile("vardefine.xml"))
	{
		return;
	}
	CXmlNode *headnode = xmlpack.GetNodeByPath("/package/head");
	if (headnode == NULL)
	{
		return;
	}
 	char tmpchar[40];
	int ret;
	vector<CXmlNode *> nodelist;
 	//�������ĳ�ʼֵ�û�
	for (int i=0;i<m_pShareData.m_varList.size();i++)
	{
		if (m_pShareData.m_varList[i]->GetVarType()==0)	
		{
			nodelist.clear();
			ret = headnode->GetChildNode(m_pShareData.m_varList[i]->GetVarName(),false,nodelist);
			if (nodelist.size() >0 )
			{
				CXmlNode *tmpnode = nodelist[0];
				sprintf(tmpchar,"%I64d",m_pShareData.m_varList[i]->GetVarValue());
				tmpnode->SetAttribute("initvalue",false,tmpchar);
			}
		}
		
	}
	xmlpack.ToFile("vardefine.xml");
}

void CDrebpmtDlg::LoadCfg()
{

	m_edtTimeOut.SetWindowText("10000");
	m_edtRRTime.SetWindowText("1000");
	m_edtTimeSep.SetWindowText("0");
	
	m_edtRunTime.SetWindowText("5");

	char szTmp[100];
	
	
	::GetPrivateProfileString("COMMON", "TIMEOUT", "60000", szTmp, 8, ".\\GatePMT.dat");
	m_edtTimeOut.SetWindowText(szTmp);

	::GetPrivateProfileString("COMMON", "RRTIME", "1000", szTmp, 8, ".\\GatePMT.dat");
	m_edtRRTime.SetWindowText(szTmp);
	
	::GetPrivateProfileString("COMMON", "LOGLEVEL", "4", szTmp, 2, ".\\GatePMT.dat");
	m_edtLogLevel.SetWindowText(szTmp);

	::GetPrivateProfileString("COMMON", "TIMESEP", "0", szTmp, 8, ".\\GatePMT.dat");
	m_edtTimeSep.SetWindowText(szTmp);
	
	::GetPrivateProfileString("COMMON", "RUNTIME", "5", szTmp, 5, ".\\GatePMT.dat");
	m_edtRunTime.SetWindowText(szTmp);
	
	::GetPrivateProfileString("COMMON", "TRYNUM", "5", szTmp, 4, ".\\GatePMT.dat");
	m_edtTryNum.SetWindowText(szTmp);


	::GetPrivateProfileString("COMMON", "LOGINTX", "8004001", szTmp, 10, ".\\GatePMT.dat");
	m_edtLoginTx.SetWindowText(szTmp);
	
	::GetPrivateProfileString("COMMON", "PMTTX", "9030", szTmp, 10, ".\\GatePMT.dat");
	m_edtFuncNo.SetWindowText(szTmp);
	
	
	::GetPrivateProfileString("COMMON", "AUTORESTAR", "0", szTmp, 4, ".\\GatePMT.dat");
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
bool CDrebpmtDlg::SaveParameter()
{
 	CString ss;
// 	m_ipctr.GetWindowText(ss);
	
	SERVER_PARA para;
	memset(&para,0,sizeof(SERVER_PARA));
// 
// 	::WritePrivateProfileString("COMMON", "ADDR", ss, ".\\GatePMT.dat");
// 	
// 	m_edtPort.GetWindowText(ss);
// 	::WritePrivateProfileString("COMMON", "PORT",ss, ".\\GatePMT.dat");

	
	m_edtTimeOut.GetWindowText(ss);
	::WritePrivateProfileString("COMMON", "TIMEOUT", ss, ".\\GatePMT.dat");
	para.timeout = atoi(ss);

	m_edtRRTime.GetWindowText(ss);
	::WritePrivateProfileString("COMMON", "RRTIME", ss, ".\\GatePMT.dat");
	para.rrtime = atoi(ss);

	
	m_edtLogLevel.GetWindowText(ss);
	::WritePrivateProfileString("COMMON", "LOGLEVEL", ss, ".\\GatePMT.dat");
	para.loglevel = atoi(ss);
	
	m_edtTimeSep.GetWindowText(ss);
	::WritePrivateProfileString("COMMON", "TIMESEP", ss, ".\\GatePMT.dat");
	para.timesep = atoi(ss);
	
	
	m_edtRunTime.GetWindowText(ss);
	::WritePrivateProfileString("COMMON", "RUNTIME", ss, ".\\GatePMT.dat");
	para.runtime = atoi(ss)*60000;
	
	m_edtTryNum.GetWindowText(ss);
	::WritePrivateProfileString("COMMON", "TRYNUM", ss, ".\\GatePMT.dat");
	para.connectNum = atoi(ss);
	
// 	m_edtNodeId.GetWindowText(ss);
// 	::WritePrivateProfileString("COMMON", "NODEID", ss, ".\\GatePMT.dat");
// 	
// 	m_edtNodePrivate.GetWindowText(ss);
// 	::WritePrivateProfileString("COMMON", "NODEPRIVATE", ss, ".\\GatePMT.dat");
// 	
// 	m_edtSvrId.GetWindowText(ss);
// 	::WritePrivateProfileString("COMMON", "SVRID", ss, ".\\GatePMT.dat");
// 	
// 	m_edtSvrPrivate.GetWindowText(ss);
// 	::WritePrivateProfileString("COMMON", "SVRPRIVATE", ss, ".\\GatePMT.dat");
// 	
// 	m_edtFuncNo.GetWindowText(ss);
// 	::WritePrivateProfileString("COMMON", "SERVICENO", ss, ".\\GatePMT.dat");
	
	if (m_chkAuto.GetCheck() == 1)
	{
		::WritePrivateProfileString("COMMON", "AUTORESTAR", "1", ".\\GatePMT.dat");
	}
	else
	{
		::WritePrivateProfileString("COMMON", "AUTORESTAR", "0", ".\\GatePMT.dat");
	}
	memcpy(&m_pShareData.m_pServerPara,&para,sizeof(SERVER_PARA));
	
	m_pShareData.InitVar("vardefine.xml");
	return true;
}
void CDrebpmtDlg::autoMonitor()
{
	char tmpchar[40];
	
	m_pDate.Update();
 	m_statusbar.SetPaneText(1,m_pDate.ToStringDT17().c_str());
	
	int curtime = CBF_Date_Time::GetTickCount()-m_pShareData.m_nRunTime;
	ConvSecTime(curtime/1000,tmpchar);

	m_edtCurTime.SetWindowText(tmpchar);//��ǰʱ��
	
	sprintf(tmpchar,"%ld",m_pShareData.m_nTotalThreadNum);
	m_edtAllThread.SetWindowText(tmpchar);//���߳���
	sprintf(tmpchar,"%ld",m_pShareData.m_nSendNum);
	m_edtSendNum.SetWindowText(tmpchar);//������
	sprintf(tmpchar,"%ld  %d",m_pShareData.m_nRcvNum,m_pShareData.m_nRcvAnsPackNum);
	m_edtRcvNum.SetWindowText(tmpchar);//������
	

	sprintf(tmpchar,"%ld",m_pShareData.m_nRcvPushNum);
	m_edtRcvPushNum.SetWindowText(tmpchar);//����������


	sprintf(tmpchar,"%ld",m_pShareData.m_nErrNum);
	m_edtErrNum.SetWindowText(tmpchar);//������
	
	sprintf(tmpchar,"%I64d����",m_pShareData.m_nMinRespTime);
	m_edtMinResp.SetWindowText(tmpchar);//��С��Ӧʱ��
	
	sprintf(tmpchar,"%I64d����",m_pShareData.m_nMaxRespTime);
	m_edtMaxResp.SetWindowText(tmpchar);//�����Ӧʱ��
	
	sprintf(tmpchar,"%I64d",m_pShareData.m_nSendByte);
	m_edtSendByte.SetWindowText(tmpchar);//�����ֽ���
	
	sprintf(tmpchar,"%I64d",m_pShareData.m_nRcvByte);
	m_edtRcvByte.SetWindowText(tmpchar);//�����ֽ���
	
	if (m_pShareData.m_nRcvNum>0)
	{
		sprintf(tmpchar,"%.2lf����",1.00*m_pShareData.m_nTotalRespTIme/m_pShareData.m_nRcvNum);
		m_edtAvResp.SetWindowText(tmpchar);//ƽ����Ӧʱ��
	}
	
	if (curtime>0)
	{
		sprintf(tmpchar,"%.2lf�ֽ�/��",1000.00*m_pShareData.m_nSendByte/curtime);
		m_edtAvrSendRate.SetWindowText(tmpchar);//ƽ����������
		sprintf(tmpchar,"%.2lf�ֽ�/��",1000.00*m_pShareData.m_nRcvByte/curtime);
		m_edtAvrRcvRate.SetWindowText(tmpchar);//ƽ��������
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
	m_edtMaxNumSec.SetWindowText(tmpchar);//��ֵÿ�����
	sprintf(tmpchar,"%d",curavr);
	m_edtCurNumSec.SetWindowText(tmpchar);//��ǰÿ�����
	m_curtime++;
	m_lastAvr = m_pShareData.m_nRcvNum;
	
	int tt = ((CBF_Date_Time::GetTickCount()-m_pShareData.m_nRunTime)/1000);
	if (tt !=0)
	{
		sprintf(tmpchar,"%.4lf",1.00*m_pShareData.m_nRcvNum/tt);
		m_edtAvNumSec.SetWindowText(tmpchar);//ƽ��ÿ�����
	}
	
	
	if (m_pShareData.m_nTotalThreadNum<1)
	{
		GetDlgItem(IDC_BUTTON_STOP)->PostMessage(BM_CLICK);
	}
	if (m_pShareData.m_pServerPara.runtime>0)
	{
		if (CBF_Date_Time::GetTickCount()- m_pShareData.m_nRunTime > m_pShareData.m_pServerPara.runtime)
		{
			GetDlgItem(IDC_BUTTON_STOP)->PostMessage(BM_CLICK);
		}
	}
	
}

void CDrebpmtDlg::Clear()
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

void CDrebpmtDlg::OnTimer(UINT_PTR nIDEvent) 
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
	default:
		break;
	}
	CDialog::OnTimer(nIDEvent);
}

void CDrebpmtDlg::OnDblclkListSc(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;

	char tmpchar[500];
	char filename[300];

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	int nItem = pNMListView->iItem;
	if (nItem<0)
	{
		return;
	}

	memset(tmpchar,0,sizeof(tmpchar));
	m_lstScript.GetItemText(nItem, 1, tmpchar, 300);
	m_edtXmlApi.SetWindowText(tmpchar);


	memset(tmpchar,0,sizeof(tmpchar));
	m_lstScript.GetItemText(nItem, 2, tmpchar, 8);//��¼������
	m_edtLoginTx.SetWindowText(tmpchar);

	memset(filename,0,sizeof(filename));
	m_lstScript.GetItemText(nItem, 3, filename, 300);//�ļ���
	m_edtLoginBody.SetWindowText(filename);

	memset(tmpchar,0,sizeof(tmpchar));
	m_lstScript.GetItemText(nItem, 4, tmpchar, 8);//��ѹ������
	m_edtFuncNo.SetWindowText(tmpchar);
	
	memset(filename,0,sizeof(filename));
	m_lstScript.GetItemText(nItem, 5, filename, 300);//��ѹ�ļ���
	m_edtFile.SetWindowText(filename);
	
	memset(tmpchar,0,sizeof(tmpchar));
	m_lstScript.GetItemText(nItem, 6, tmpchar, 40);//��ʼʱ��
	m_edtBeginTime.SetWindowText(tmpchar);

	memset(tmpchar,0,sizeof(tmpchar));
	m_lstScript.GetItemText(nItem, 7, tmpchar, 6);//�߳���
	m_edtThreadNum.SetWindowText(tmpchar);
	

	UpdateData(false);
}

void CDrebpmtDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	Clear();
	CDialog::OnCancel();
}
void CDrebpmtDlg::ConvSecTime(int ssec,char *time)
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

void CDrebpmtDlg::OnButtonFilelogin() 
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
	
	m_edtLoginBody.SetWindowText(ss);
	//������·�����û�ȥ
	SetCurrentDirectory(filename);
	UpdateData(false);
	m_edtThreadNum.SetFocus();
}

void CDrebpmtDlg::OnButtonApifilename() 
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
	
	m_edtXmlApi.SetWindowText(ss);
	//������·�����û�ȥ
	SetCurrentDirectory(filename);
	UpdateData(false);
	m_edtThreadNum.SetFocus();
}
