// cgatetoolDlg.cpp : implementation file
//

#include "stdafx.h"
#include "cgatetool.h"
#include "cgatetoolDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



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

typedef struct 
{
	unsigned short  index;
	unsigned short  serial;
}__PACKED__ S_GATE_SERIAL,*PS_GATE_SERIAL;   //�ͻ������������Ϳͻ���������ˮ�ṹ



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
// CCgatetoolDlg dialog

CCgatetoolDlg::CCgatetoolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCgatetoolDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCgatetoolDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCgatetoolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCgatetoolDlg)
	DDX_Control(pDX, IDC_COMBO_MODE, m_comb_mode);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
	DDX_Control(pDX, IDC_RICHEDIT_DATA, m_redt_data);
	DDX_Control(pDX, IDC_EDIT_KEY, m_edtKey);
	DDX_Control(pDX, IDC_EDIT_DATAFILE, m_edtDataFile);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCgatetoolDlg, CDialog)
	//{{AFX_MSG_MAP(CCgatetoolDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_BROW, OnButtonBrow)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_SERIAL, OnButtonSerial)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCgatetoolDlg message handlers

BOOL CCgatetoolDlg::OnInitDialog()
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
	
	bzero(m_sDataOld,sizeof(m_sDataOld));
	bzero(m_sDataNew,sizeof(m_sDataNew));
	m_nDataOldLen=0;
	m_nDataNewLen=0;

	m_edtKey.SetLimitText(16);
	m_btnSave.EnableWindow(FALSE);

	m_comb_mode.AddString("1   DES����");
	m_comb_mode.AddString("2   DES����");
	m_comb_mode.AddString("3   ѹ��");
	m_comb_mode.AddString("4   ��ѹ��");
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCgatetoolDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CCgatetoolDlg::OnPaint() 
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
HCURSOR CCgatetoolDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CCgatetoolDlg::OnOK() 
{
	// TODO: Add extra validation here
	
//	CDialog::OnOK();

	UpdateData(true);
	CString ss;
	

	m_comb_mode.GetWindowText(ss);
	if (ss.GetLength()<1)
	{
		MessageBox("��ѡ�����ݴ���ģʽ","����");
		return;
	}
	int mode = atoi(ss.GetBuffer(1));
	
	if (mode == 1 || mode == 2 )
	{
		m_edtKey.GetWindowText(ss);
		m_btnSave.EnableWindow(FALSE);
		if (ss.GetLength() != 16)
		{
			MessageBox("��Կ����ӦΪ16λ�����벻��","����");
			return;
		}
		bzero(m_sKey,sizeof(m_sKey));
		memcpy(m_sKey,ss.GetBuffer(ss.GetLength()),16);
	}
	
	m_edtDataFile.GetWindowText(ss);
	if (ss.GetLength() <1)
	{
		MessageBox("������ԭʼ�����ļ�","����");
		return;
	}

	FILE *fp = fopen(ss.GetBuffer(ss.GetLength()),"rb");
	if (fp == NULL)
	{
		MessageBox("���ļ�����","����");
		return;
	}
	int len= sizeof(m_sDataOld)-1;
	m_nDataOldLen = fread(m_sDataOld,1,len,fp);
	fclose(fp);
	if (m_nDataOldLen == len)
	{
		MessageBox("�ļ����Ȳ���","����");
		return ;
	}
	unsigned int ulen;
	switch (mode)
	{
		case 2:
			if (m_pDes.UncryptString(m_sDataOld,m_nDataOldLen,m_sKey)<=0)
			{
				MessageBox("���ݽ���ʧ��","����");
				m_redt_data.SetWindowText("");
				return ;
			}
			m_redt_data.SetWindowText(m_sDataOld);
			UpdateData(FALSE);
			m_btnSave.EnableWindow(TRUE);
			MessageBox("���ݽ��ܳɹ��������水ť���洦����","��ʾ");
			break;
		case 1:
			if (m_pDes.EncryptString(m_sDataOld,m_nDataOldLen,m_sKey) <=0)
			{
				MessageBox("���ݼ���ʧ��","����");
				m_redt_data.SetWindowText("");
				return ;
			}
			m_redt_data.SetWindowText(m_sDataOld);
			UpdateData(FALSE);
			m_btnSave.EnableWindow(TRUE);
			MessageBox("���ݼ��ܳɹ��������水ť���洦����","��ʾ");
			break;
		case 3:
			memcpy(m_sDataNew,m_sDataOld,m_nDataOldLen);
			m_nDataNewLen = m_nDataOldLen;
			ulen = sizeof(m_sDataOld);
			if (!CBF_Tools::Compress((unsigned char *)m_sDataOld,ulen,(unsigned char *)m_sDataNew,m_nDataNewLen))
			{
				MessageBox("����ѹ��ʧ��","����");
				m_redt_data.SetWindowText("");
				return ;
			}
			m_nDataOldLen = ulen;
			m_redt_data.SetWindowText(m_sDataOld);
			UpdateData(FALSE);
			m_btnSave.EnableWindow(TRUE);
			MessageBox("����ѹ���ɹ��������水ť���洦����","��ʾ");
			break;  
		case 4:
			memcpy(m_sDataNew,m_sDataOld,m_nDataOldLen);
			m_nDataNewLen = m_nDataOldLen;
			ulen = sizeof(m_sDataOld);
			if (!CBF_Tools::Uncompress((unsigned char *)m_sDataOld,ulen,(unsigned char *)m_sDataNew,m_nDataNewLen))
			{
				MessageBox("���ݽ�ѹ��ʧ��","����");
				m_redt_data.SetWindowText("");
				return ;
			}
			m_nDataOldLen = ulen;
			m_redt_data.SetWindowText(m_sDataOld);
			UpdateData(FALSE);
			m_btnSave.EnableWindow(TRUE);
			MessageBox("���ݽ�ѹ���ɹ��������水ť���洦����","��ʾ");
			break; 
		default:
			MessageBox("���ݲ���ģʽ����","����");
			break;
	}
	

}

void CCgatetoolDlg::OnButtonBrow() 
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
	//������·�����û�ȥ
	SetCurrentDirectory(filename);
	UpdateData(false);
	
	
	FILE *fp = fopen(ss.GetBuffer(ss.GetLength()),"rb");
	if (fp == NULL)
	{
		MessageBox("���ļ�����","����");
		return;
	}
	int len= sizeof(m_sDataOld)-1;
	m_nDataOldLen = fread(m_sDataOld,1,len,fp);
	fclose(fp);
	if (m_nDataOldLen == len)
	{
		MessageBox("�ļ����Ȳ���","����");
		return ;
	}
	m_edtDataFile.SetWindowText(ss);
	UpdateData(FALSE);
	
}

void CCgatetoolDlg::OnButtonSave() 
{
	// TODO: Add your control notification handler code here
	char filename[400];
	memset(filename,0,sizeof(filename));
	WORKPATH(filename);
	CString ss;
	int ret;
	
	CFileDialog   dlg(false,"","*.*",OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"ALL Files (*.*)|",NULL);
	ret = dlg.DoModal();
	ss = dlg.GetPathName();
	
	if (ret !=IDOK)
	{
		return;
	}
	std::string savefilename;
	savefilename = ss.GetBuffer(ss.GetLength());
	//������·�����û�ȥ
	SetCurrentDirectory(filename);
	UpdateData(false);
	FILE *fp = fopen(savefilename.c_str(),"wb");
	if (fp == NULL)
	{
		MessageBox("���ļ�����","��ʾ");
		return;
	}
	ret = fwrite((char *)&m_sDataOld,1,m_nDataOldLen,fp);
	if (ret != m_nDataOldLen)
	{
		MessageBox("д���ļ�����","��ʾ");
		fclose(fp);
		return;
	}
	fclose(fp);
	MessageBox("����ɹ�","��ʾ");
}


void CCgatetoolDlg::OnButtonSerial() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	CString ss;
	m_edtKey.GetWindowText(ss);
	if (ss.GetLength()<1)
	{
		MessageBox("������drebͷ��s_nSerial�ֶ�ֵ","��ʾ");
		return ;
	}
	int  serial = atoi(ss.GetBuffer(ss.GetLength()));
	PS_GATE_SERIAL gserial = (PS_GATE_SERIAL)(&(serial));
	char tmpchar[200];
	sprintf(tmpchar,"��������s_nSvrMainId[%d] �������s_nHook[%d]",gserial->index,gserial->serial);
	MessageBox(tmpchar,"��ʾ");
	return ;
}
