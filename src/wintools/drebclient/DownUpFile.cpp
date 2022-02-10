// DownUpFile.cpp : implementation file
//

#include "stdafx.h"
#include "drebclient.h"
#include "DownUpFile.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDownUpFile dialog


CDownUpFile::CDownUpFile(CWnd* pParent /*=NULL*/)
	: CDialog(CDownUpFile::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDownUpFile)
	//}}AFX_DATA_INIT
}


void CDownUpFile::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDownUpFile)
	DDX_Control(pDX, IDC_EDIT_REMARK, m_edtRemark);
	DDX_Control(pDX, IDC_EDIT_REMOTE, m_edtRemote);
	DDX_Control(pDX, IDC_BUTTON_LOCAL, m_btnLocal);
	DDX_Control(pDX, IDC_EDIT_LOCAL, m_edtLocal);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDownUpFile, CDialog)
	//{{AFX_MSG_MAP(CDownUpFile)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BUTTON_LOCAL, OnButtonLocal)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDownUpFile message handlers

void CDownUpFile::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL CDownUpFile::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	m_btnLocal.EnableWindow(true);
	if (m_nFileMode == 0)
	{
		//AfxGetApp()->GetMainWnd()->SetWindowText("�ϴ��ļ�");
		this->SetWindowText("�ϴ��ļ�");
		m_edtRemark.SetWindowText("Զ���ļ����������Ҳ��ɻָ�������������");
	}
	else
	{
		this->SetWindowText("�����ļ�");
		m_edtRemark.SetWindowText("�����ļ����������Ҳ��ɻָ���������ѡ��");
		//AfxGetApp()->GetMainWnd()->SetWindowText("�����ļ�");
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDownUpFile::OnOK() 
{
	// TODO: Add extra validation here
	CString ss;
	if (m_edtLocal.GetWindowTextLength()<1)
	{
		MessageBox("�������ѡ�񱾵��ļ�","��ʾ");
		return;
	}
	if (m_edtRemote.GetWindowTextLength()<1)
	{
		MessageBox("�������ѡ��Զ���ļ�","��ʾ");
		return;
	}
	if (m_edtRemote.GetWindowTextLength()>200)
	{
		MessageBox("Զ���ļ����Ȳ��ܴ���200","��ʾ");
		return;
	}
	m_edtLocal.GetWindowText(ss);
	m_sLocalFileName = ss.GetBuffer(ss.GetLength());
	m_edtRemote.GetWindowText(ss);
	m_sRemoteFileName = ss.GetBuffer(ss.GetLength());
	CDialog::OnOK();
}

void CDownUpFile::OnButtonLocal() 
{
	// TODO: Add your control notification handler code here
	char filename[400];
	memset(filename,0,sizeof(filename));
	WORKPATH(filename);
	CString ss;
	int ret;
	
	if (m_nFileMode == 0)
	{
		CFileDialog   dlg(true,"","*.*",OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"ALL Files (*.*)|",NULL);
		ret = dlg.DoModal();
		ss = dlg.GetPathName();
		
		if (ret !=IDOK)
		{
			return;
		}
		
		m_edtLocal.SetWindowText(ss);
		struct_stat statfile;
		FSTAT(ss.GetBuffer(ss.GetLength()),&statfile);
		if (statfile.st_size <1 )
		{
			MessageBox("�����ļ�������","����");
			m_btnLocal.SetFocus();
			return ;
		}
	}
	else
	{
		CFileDialog   dlg(false,"","*.*",OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"ALL Files (*.*)|",NULL);
		ret = dlg.DoModal();
		ss = dlg.GetPathName();
		
		if (ret !=IDOK)
		{
			return;
		}
		
		m_edtLocal.SetWindowText(ss);
	}

	
	//������·�����û�ȥ
	SetCurrentDirectory(filename);
	UpdateData(false);
	m_btnLocal.SetFocus();
}

void CDownUpFile::OnButtonRemote() 
{
	// TODO: Add your control notification handler code here
	
}
