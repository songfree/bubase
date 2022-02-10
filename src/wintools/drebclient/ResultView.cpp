// ResultView.cpp : implementation file
//

#include "stdafx.h"
#include "drebclient.h"
#include "ResultView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CResultView dialog


CResultView::CResultView(CWnd* pParent /*=NULL*/)
	: CDialog(CResultView::IDD, pParent)
{
	//{{AFX_DATA_INIT(CResultView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pDataPoint = NULL;
}


void CResultView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CResultView)
	DDX_Control(pDX, IDC_STATIC_TITLE, m_btnTitle);
	DDX_Control(pDX, IDC_RICHEDIT_RESULT, m_redt_result);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CResultView, CDialog)
	//{{AFX_MSG_MAP(CResultView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResultView message handlers

void CResultView::OnOK() 
{
	// TODO: Add extra validation here
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
	
	
	FILE *fp = fopen(ss.GetBuffer(ss.GetLength()),"wb");
	if (fp == NULL)
	{
		MessageBox("打开文件失败","错误");
		return ;
	}
	fwrite(m_pDataPoint,1,m_nDataLen,fp);
	fclose(fp);
	
	//将工作路径设置回去
	SetCurrentDirectory(filename);
	UpdateData(false);
	//CDialog::OnOK();
}

BOOL CResultView::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if (m_pDataPoint != NULL)
	{
		if (m_pXdp->m_pXdpFmt.GetFieldNum()>0)
		{
			char errmsg[256];
			if (m_pXdp->FromBuffer(m_pDataPoint,m_nDataLen,errmsg))
			{
				char xdpdata[16384*2];
				m_pXdp->PrintXdp(xdpdata);
				m_redt_result.SetWindowText(xdpdata);
				char msg[40];
				sprintf(msg,"XDP数据长度%d",m_nDataLen);
				m_btnTitle.SetWindowText(msg);
			}
			else
			{
				m_redt_result.SetWindowText(m_pDataPoint);
				char msg[40];
				sprintf(msg,"数据长度%d",m_nDataLen);
				m_btnTitle.SetWindowText(msg);
			}
		}
		else
		{
			m_redt_result.SetWindowText(m_pDataPoint);
			char msg[40];
			sprintf(msg,"数据长度%d",m_nDataLen);
			m_btnTitle.SetWindowText(msg);
		}
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
