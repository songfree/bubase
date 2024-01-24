// FileXdp.cpp : implementation file
//

#include "stdafx.h"
#include "drebclient.h"
#include "FileXdp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileXdp dialog


CFileXdp::CFileXdp(CWnd* pParent /*=NULL*/)
	: CDialog(CFileXdp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFileXdp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CFileXdp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFileXdp)
	DDX_Control(pDX, IDC_EDIT_XDPVALUE, m_edtValue);
	DDX_Control(pDX, IDC_EDIT_XDPFIELD, m_edtFld);
	DDX_Control(pDX, IDC_EDIT_FILE, m_edtFile);
	DDX_Control(pDX, IDC_LIST_XDPDEFINE, m_lstXdpDefine);
	DDX_Control(pDX, IDC_LIST_XDP, m_lstXdp);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFileXdp, CDialog)
	//{{AFX_MSG_MAP(CFileXdp)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, OnButtonOpen)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_XDPDEFINE, OnDblclkListXdpdefine)
	ON_NOTIFY(NM_CLICK, IDC_LIST_XDP, OnClickListXdp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileXdp message handlers

BOOL CFileXdp::OnInitDialog() 
{
	
	CDialog::OnInitDialog();
	// TODO: Add extra initialization here
	m_lstXdpDefine.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_HEADERDRAGDROP);
	
	
	m_lstXdpDefine.InsertColumn(0, "序号", LVCFMT_LEFT, 80);
	m_lstXdpDefine.InsertColumn(1, "类型", LVCFMT_LEFT, 160);
	m_lstXdpDefine.InsertColumn(2, "名称", LVCFMT_LEFT, 160);
	m_lstXdpDefine.InsertColumn(3, "长度", LVCFMT_LEFT, 100);

	m_lstXdp.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_HEADERDRAGDROP);
	
	
	m_lstXdp.InsertColumn(0, "序号", LVCFMT_LEFT, 80);
	m_lstXdp.InsertColumn(1, "类型", LVCFMT_LEFT, 180);
	m_lstXdp.InsertColumn(2, "值", LVCFMT_LEFT, 500);

	char errmsg[256];
	if (!m_pXdp->InitXdp("xdp.xml",errmsg))
	{
		MessageBox(errmsg,"提示");
		CDialog::OnCancel();
		return FALSE;
	}
	m_nXdpDataLen = 0;
	bzero(m_xdpData,sizeof(m_xdpData));

	int num = m_pXdp->m_pXdpFmt.GetFieldNum();
	S_XDP_FIELD_DEFINE *fld;
	char tmpchar[20];
// #define  XDP_UNKNOW    0    //未知
// #define  XDP_USHORT    1    //无符号短整型
// #define  XDP_SHORT     2    //短整型
// #define  XDP_UINT      3    //无符号整型
// #define  XDP_INT       4    //整型
// #define  XDP_DOUBLE    5    //浮点型
// #define  XDP_CHAR      6    //固定长度字符串 最大255
// #define  XDP_VARCHAR   7    //变长字符串 最大255   最前面有一个字节长度  1+data
// #define  XDP_BINDATA   8    //二进制类型 最大4096  最前面有两个字节长度  2+data 
//#define  XDP_INT64     9    //64位整型 INT64_
//#define  XDP_UINT64    10    //无符号64位整型	UINT64_
	for (int i=0; i< num; i++)
	{
		fld = m_pXdp->m_pXdpFmt.At(i);
		int nLastRow = m_lstXdpDefine.GetItemCount();
		sprintf(tmpchar,"%d",i);
		int nItem = m_lstXdpDefine.InsertItem(nLastRow,tmpchar);
		switch (fld->f_type)
		{
			case XDP_UNKNOW:
				m_lstXdpDefine.SetItemText(nItem,1,"未知");
				break;
			case XDP_USHORT:
				m_lstXdpDefine.SetItemText(nItem,1,"无符号短整型");
				break;
			case XDP_SHORT:
				m_lstXdpDefine.SetItemText(nItem,1,"短整型");
				break;
			case XDP_UINT:
				m_lstXdpDefine.SetItemText(nItem,1,"无符号整型");
				break;
			case XDP_INT:
				m_lstXdpDefine.SetItemText(nItem,1,"整型");
				break;
            case XDP_UINT64:
                m_lstXdpDefine.SetItemText(nItem, 1, "64位无符号整型");
                break;
            case XDP_INT64:
                m_lstXdpDefine.SetItemText(nItem, 1, "64位整型");
                break;
			case XDP_DOUBLE:
				m_lstXdpDefine.SetItemText(nItem,1,"浮点型");
				break;
			case XDP_CHAR:
				m_lstXdpDefine.SetItemText(nItem,1,"固定长度字符串");
				break;
			case XDP_VARCHAR:
				m_lstXdpDefine.SetItemText(nItem,1,"变长字符串");
				break;
			case XDP_BINDATA:
				m_lstXdpDefine.SetItemText(nItem,1,"二进制类型");
				break;
			default:
				break;
		}
		
		m_lstXdpDefine.SetItemText(nItem,2,fld->f_name);
		sprintf(tmpchar,"%d",fld->f_length);
		m_lstXdpDefine.SetItemText(nItem,3,tmpchar);
		
	}
	m_pDataXdp.XdpCopy(m_pXdp);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFileXdp::OnOK() 
{
	// TODO: Add extra validation here
	char errmsg[256];
	m_nXdpDataLen = sizeof(m_pDataXdp);
	if (!m_pDataXdp.ToBuffer(m_xdpData,m_nXdpDataLen,errmsg))
	{
		m_nXdpDataLen =0;
		MessageBox("XDP ToBuffer 出错","错误");
		return;
	}
	CDialog::OnOK();
}

void CFileXdp::OnCancel() 
{
	// TODO: Add extra cleanup here
	m_nXdpDataLen =0;
	CDialog::OnCancel();
}

void CFileXdp::OnButtonAdd() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CString ss;
	m_edtFld.GetWindowText(ss);
	int index = atoi(ss.GetBuffer(ss.GetLength()));
	
	char fldvalue[4000];
	int fldvaluelen =0;
	m_edtValue.GetWindowText(ss);
	fldvaluelen = ss.GetLength();
	if (fldvaluelen<1)
	{
		MessageBox("请输入字段值","提示");
		m_edtValue.SetFocus();
		return;
	}
	memcpy(fldvalue,ss.GetBuffer(fldvaluelen),fldvaluelen);


	S_XDP_FIELD_DEFINE *fld= m_pDataXdp.m_pXdpFmt.At(index);
	if (fld == NULL)
	{
		MessageBox("字段索引不存在","提示");
		m_edtValue.SetFocus();
		return;
	}
	char errmsg[256];
	unsigned short pus;
	short ps;
	unsigned int pui;
	int  pi;
	double pd;
	INT64_ pin64;
	switch (fld->f_type)
	{
	case XDP_USHORT:
		pus = atoi(fldvalue);
		m_pDataXdp.SetFieldValue(fld->f_index,pus,errmsg);
		break;
	case XDP_SHORT:
		ps = atoi(fldvalue);
		m_pDataXdp.SetFieldValue(fld->f_index,ps,errmsg);
		break;
	case XDP_UINT:
		pui = atol(fldvalue);
		m_pDataXdp.SetFieldValue(fld->f_index,pui,errmsg);
		break;
	case XDP_INT:
		pi = atoi(fldvalue);
		m_pDataXdp.SetFieldValue(fld->f_index,pi,errmsg);
		break;
	case XDP_UINT64:
    case XDP_INT64:
		pin64 = atoll(fldvalue);
        m_pDataXdp.SetFieldValue(fld->f_index, pin64, errmsg);
        break;
	case XDP_DOUBLE:
		pd = atof(fldvalue);
		m_pDataXdp.SetFieldValue(fld->f_index,pd,errmsg);
		break;
	case XDP_CHAR:
	case XDP_VARCHAR:
	case XDP_BINDATA:
		if (fldvaluelen > fld->f_length)
		{
			MessageBox("字段长度不符","提示");
			m_edtValue.SetFocus();
			return;
		}
		m_pDataXdp.SetFieldValue(fld->f_index,fldvalue,fldvaluelen,errmsg);
		break;
	default:
			return ;
	}
	UpdateLstXdp(&m_pDataXdp);
	return;
}

void CFileXdp::OnButtonDelete() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CString ss;
	m_edtFld.GetWindowText(ss);
	int index = atoi(ss.GetBuffer(ss.GetLength()));
	S_XDP_FIELD_DEFINE *fld= m_pDataXdp.m_pXdpFmt.At(index);
	if (fld == NULL)
	{
		MessageBox("字段索引不存在","提示");
		m_edtValue.SetFocus();
		return;
	}
	m_pDataXdp.CancelField(index);
	UpdateLstXdp(&m_pDataXdp);
}

void CFileXdp::OnButtonSave() 
{
	// TODO: Add your control notification handler code here
	CString ss;
	m_edtFile.GetWindowText(ss);
	if (ss.GetLength()<1)
	{
		MessageBox("请输入xdp数据文件名","提示");
		m_edtFile.SetFocus();
		return;
	}
	struct_stat statfile;
	FSTAT(ss.GetBuffer(ss.GetLength()),&statfile);
	if (statfile.st_size >0 )
	{
		int ret = MessageBox("数据文件已存在，是否覆盖?","提示",MB_YESNO);
		if (ret!=IDYES)
		{
			m_edtFile.SetFocus();
			return ;
		}
		
	}
	char errmsg[256];
	m_nXdpDataLen = sizeof(m_pDataXdp);
	if (!m_pDataXdp.ToBuffer(m_xdpData,m_nXdpDataLen,errmsg))
	{
		m_nXdpDataLen =0;
		MessageBox("XDP ToBuffer 出错","错误");
		return;
	}
	FILE *fp = fopen(ss.GetBuffer(ss.GetLength()),"wb");
	if (fp == NULL)
	{
		MessageBox("打开数据文件失败","错误");
		m_edtFile.SetFocus();
		return ;
	}
	if (m_nXdpDataLen != fwrite(m_xdpData,1,m_nXdpDataLen,fp))
	{
		MessageBox("写数据文件失败","错误");
		m_edtFile.SetFocus();
		fclose(fp);
		return ;
	}
	fclose(fp);
	MessageBox("保存成功","提示");
	return;
}

void CFileXdp::OnButtonOpen() 
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
	struct_stat statfile;
	FSTAT(ss.GetBuffer(ss.GetLength()),&statfile);
	if (statfile.st_size <1 || statfile.st_size >=DREBDATASIZE)
	{
		MessageBox("数据文件不存在或数据太长","错误");
		m_edtFile.SetFocus();
		return ;
	}
	FILE *fp = fopen(ss.GetBuffer(ss.GetLength()),"rb");
	if (fp == NULL)
	{
		MessageBox("打开数据文件失败","错误");
		m_edtFile.SetFocus();
		return ;
	}
	memset(m_xdpData,0,sizeof(m_xdpData));
	m_nXdpDataLen = fread(m_xdpData,1,statfile.st_size,fp);
	fclose(fp);
	char errmsg[256];
	if (!m_pDataXdp.FromBuffer(m_xdpData,m_nXdpDataLen,errmsg))
	{
		MessageBox("数据文件非XDP","错误");
		m_edtFile.SetFocus();
		m_nXdpDataLen = 0;
		m_lstXdp.DeleteAllItems();
		return ;
	}
	UpdateLstXdp(&m_pDataXdp);
	
	//将工作路径设置回去
	SetCurrentDirectory(filename);
	UpdateData(false);
	
}

void CFileXdp::OnDblclkListXdpdefine(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	*pResult = 0;
	int nItem = pNMListView->iItem;
	char szTmp[501];
	memset(szTmp,0,sizeof(szTmp));
	m_lstXdpDefine.GetItemText(nItem, 0, szTmp, 500);
	
	m_edtFld.SetWindowText(szTmp);
}

void CFileXdp::OnClickListXdp(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	int nItem = pNMListView->iItem;
	char szTmp[1001];
	memset(szTmp,0,sizeof(szTmp));
	m_lstXdp.GetItemText(nItem, 0, szTmp, 500);
	m_edtFld.SetWindowText(szTmp);
	m_lstXdp.GetItemText(nItem, 2, szTmp, 1000);
	m_edtValue.SetWindowText(szTmp);
}

bool CFileXdp::UpdateLstXdp(CXdp *xdp)
{
	m_lstXdp.DeleteAllItems();
	char tmpchar[200];
	CXdpRecord *xr = xdp->GetXdpRecord(1);
	
	int datalen = 0;
	int curdatalen=0;
	char errmsg[400];
	char fielddata[8192];
	unsigned int fdatalen;
	
	unsigned short *pushort=NULL;
	short *pshort=NULL;
	unsigned int  *puint=NULL;
	int  *pint=NULL;
	double *pdvalue=NULL;
	INT64_ *pin64=NULL;
	int fieldtype;
	for (int i=0 ; i< xdp->m_pXdpFmt.GetFieldNum() ; i++)
	{
		if (!xr->IsFieldSet(i))
		{
			continue;
		}
		S_XDP_FIELD_DEFINE *field = xdp->m_pXdpFmt.At(i);
		if (field == NULL)
		{
			return false;
		}
		fdatalen = sizeof(fielddata);
		bzero(fielddata,sizeof(fielddata));
		if (!xr->GetFieldData(field->f_index,fielddata,fdatalen,fieldtype,errmsg))
		{
			return false;
		}
		int nLastRow = m_lstXdp.GetItemCount();
		sprintf(tmpchar,"%d",i);
		int nItem = m_lstXdp.InsertItem(nLastRow,tmpchar);
		switch (field->f_type)
		{
		case XDP_USHORT:
			m_lstXdp.SetItemText(nItem,1,field->f_name);
			pushort = (unsigned short *)fielddata;
			sprintf(tmpchar,"%d",*pushort);
			m_lstXdp.SetItemText(nItem,2,tmpchar);
			break;
		case XDP_SHORT:
			m_lstXdp.SetItemText(nItem,1,field->f_name);
			pshort = (short *)fielddata;
			sprintf(tmpchar,"%d",*pshort);
			m_lstXdp.SetItemText(nItem,2,tmpchar);
			break;
		case XDP_UINT:
			m_lstXdp.SetItemText(nItem,1,field->f_name);
			puint = (unsigned int *)fielddata;
			sprintf(tmpchar,"%d",*puint);
			m_lstXdp.SetItemText(nItem,2,tmpchar);
			break;
		case XDP_INT:
			m_lstXdp.SetItemText(nItem,1,field->f_name);
			pint = (int *)fielddata;
			sprintf(tmpchar,"%d",*pint);
			m_lstXdp.SetItemText(nItem,2,tmpchar);
			break;
		case XDP_DOUBLE:
			m_lstXdp.SetItemText(nItem,1,field->f_name);
			pdvalue = (double *)fielddata;
			sprintf(tmpchar,"%f",*pdvalue);
			m_lstXdp.SetItemText(nItem,2,tmpchar);
			break;
		case XDP_UINT64:
        case XDP_INT64:
            m_lstXdp.SetItemText(nItem, 1, field->f_name);
			pin64 = (INT64_*)fielddata;
#ifdef _WINDOWS
            sprintf(tmpchar, "%I64d", *pin64);
 #else
			sprintf(tmpchar, "%lld", *pin64);
 #endif
            m_lstXdp.SetItemText(nItem, 2, tmpchar);
            break;
		case XDP_CHAR:
			m_lstXdp.SetItemText(nItem,1,field->f_name);
			sprintf(tmpchar,"%s",fielddata);
			m_lstXdp.SetItemText(nItem,2,tmpchar);
			break;
		case XDP_VARCHAR:
			m_lstXdp.SetItemText(nItem,1,field->f_name);
			sprintf(tmpchar,"%s",fielddata);
			m_lstXdp.SetItemText(nItem,2,tmpchar);
			break;
		case XDP_BINDATA:
			m_lstXdp.SetItemText(nItem,1,field->f_name);
			//转成16进制
			
			break;
		default:
			return false;
		}
	}
	return true;
}
