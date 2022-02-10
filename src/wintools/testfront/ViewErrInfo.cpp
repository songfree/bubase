// ViewErrInfo.cpp : implementation file
//

#include "stdafx.h"
#include "testfront.h"
#include "ViewErrInfo.h"

#include "public.h"
#include "BF_Date_Time.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewErrInfo dialog


CViewErrInfo::CViewErrInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CViewErrInfo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CViewErrInfo)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CViewErrInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewErrInfo)
	DDX_Control(pDX, IDC_RICHEDIT_ERR, m_redtErr);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CViewErrInfo, CDialog)
	//{{AFX_MSG_MAP(CViewErrInfo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewErrInfo message handlers

BOOL CViewErrInfo::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	int ret;
	char  *filebuf=NULL;
	int filelen;
	CBF_Date_Time  pDate;
	char filename[400];
	pDate.Update();
	sprintf(filename,"log\\%s%s",pDate.ToStringD8().c_str(),"TestFront.log");
	struct_stat f_stat;
	FSTAT(filename,&f_stat);
	filelen = f_stat.st_size;
	if (filelen>0)
	{
		FILE *fp = fopen(filename,"rb");
		if (fp != NULL)
		{
			filebuf = (char *)malloc(filelen+1);
			if (filebuf != NULL)
			{
				memset(filebuf,0,filelen+1);
				ret = fread(filebuf,1,filelen,fp);
				if (ret>0)
				{
					filebuf[ret]=0;
				}
				m_redtErr.SetWindowText(filebuf);
				free(filebuf);
				filebuf = NULL;

			}
			fclose(fp);
			fp = NULL;
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
