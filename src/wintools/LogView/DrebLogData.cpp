// DrebLogData.cpp : implementation file
//

#include "stdafx.h"
#include "LogView.h"
#include "DrebLogData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDrebLogData dialog


CDrebLogData::CDrebLogData(CWnd* pParent /*=NULL*/)
	: CDialog(CDrebLogData::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDrebLogData)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDrebLogData::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDrebLogData)
	DDX_Control(pDX, IDC_EDIT_ZIP, m_edtZip);
	DDX_Control(pDX, IDC_EDIT_DATA_LEN, m_edtDataLen);
	DDX_Control(pDX, IDC_RICHEDIT_DATA, m_redtData);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDrebLogData, CDialog)
	//{{AFX_MSG_MAP(CDrebLogData)
	ON_BN_CLICKED(ID_SAVE, OnSave)
	ON_BN_CLICKED(ID_SAVE_DATA, OnSaveData)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDrebLogData message handlers

BOOL CDrebLogData::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_pLog.SetLogPara(LOG_DEBUG+1,"","logview.log");
	m_pDesZip.SetLogPara(&m_pLog);
	
	// TODO: Add extra initialization here
	if (m_nDataLen>0)
	{
		m_sData[m_nDataLen]=0;
		char tmpchar[40];
		sprintf(tmpchar,"%d",m_nDataLen);
		m_edtDataLen.SetWindowText(tmpchar);
		
		memcpy(&(m_data.head),&m_head,DREBHEADLEN);
		if (m_nDataLen>0)
		{
			memcpy(m_data.buffer,m_sData,m_nDataLen);
		}
		
		if (!m_pDesZip.DrebZipDesProcess(&m_data,0))
		{
			memcpy(&(m_data.head),&m_head,DREBHEADLEN);
			if (m_nDataLen>0)
			{
				memcpy(m_data.buffer,m_sData,m_nDataLen);
			}
			m_data.head.nLen = m_nDataLen;
			sprintf(tmpchar,"%d",m_data.head.nLen);
			m_edtDataLen.SetWindowText(tmpchar);
			sprintf(tmpchar,"%d",m_data.head.cZip);
			m_edtZip.SetWindowText(tmpchar);
			m_redtData.SetWindowText(m_data.buffer);

			return true;
		}
		m_data.buffer[m_data.head.nLen]=0;
		char errmsg[256];
		if (m_pXdp.InitXdp("xdp.xml",errmsg))
		{
			if (m_pXdp.FromBuffer(m_data.buffer,m_data.head.nLen,errmsg))
			{
				char xdpdata[DREBDATASIZE*2];
				m_pXdp.PrintXdp(xdpdata);
				sprintf(tmpchar,"%d",m_data.head.nLen);
				m_edtDataLen.SetWindowText(tmpchar);
				sprintf(tmpchar,"%d",m_data.head.cZip);
				m_edtZip.SetWindowText(tmpchar);
				m_redtData.SetWindowText(xdpdata);
				return TRUE; 
			}
			
		}
		
		sprintf(tmpchar,"%d",m_data.head.nLen);
		m_edtDataLen.SetWindowText(tmpchar);
		sprintf(tmpchar,"%d",m_data.head.cZip);
		m_edtZip.SetWindowText(tmpchar);
		m_redtData.SetWindowText(m_data.buffer);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDrebLogData::OnSave() 
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
	//将工作路径设置回去
	SetCurrentDirectory(filename);
	UpdateData(false);
	FILE *fp = fopen(savefilename.c_str(),"wb");
	if (fp == NULL)
	{
		MessageBox("打开文件出错","提示");
		return;
	}
	ret = fwrite((char *)&m_data,1,DREBHEADLEN+m_data.head.nLen,fp);
	if (ret != DREBHEADLEN+m_data.head.nLen)
	{
		MessageBox("写入文件出错","提示");
		fclose(fp);
		return;
	}
	fclose(fp);
	MessageBox("保存成功","提示");
}

void CDrebLogData::OnSaveData() 
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
	//将工作路径设置回去
	SetCurrentDirectory(filename);
	UpdateData(false);
	FILE *fp = fopen(savefilename.c_str(),"wb");
	if (fp == NULL)
	{
		MessageBox("打开文件出错","提示");
		return;
	}
	ret = fwrite(m_sData,1,m_nDataLen,fp);
	if (ret != m_nDataLen)
	{
		MessageBox("写入文件出错","提示");
		fclose(fp);
		return;
	}
	fclose(fp);
	MessageBox("保存成功","提示");
}
