// VarConfig.cpp : implementation file
//

#include "stdafx.h"
#include "drebpmt.h"
#include "VarConfig.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVarConfig dialog


CVarConfig::CVarConfig(CWnd* pParent /*=NULL*/)
	: CDialog(CVarConfig::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVarConfig)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_currNode = NULL;
}


void CVarConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVarConfig)
	DDX_Control(pDX, IDC_BUTTON_DELVALUE, m_btnDelValue);
	DDX_Control(pDX, IDC_BUTTON_DEL, m_btnDel);
	DDX_Control(pDX, IDC_BUTTON_ADDVALUE, m_btnAddValue);
	DDX_Control(pDX, IDC_LIST_VARVALUE, m_lstVarValue);
	DDX_Control(pDX, IDC_LIST_VAR, m_lstVar);
	DDX_Control(pDX, IDC_EDIT_VARNAME, m_edtVarName);
	DDX_Control(pDX, IDC_EDIT_VARLEN, m_edtVarLen);
	DDX_Control(pDX, IDC_EDIT_VALUE, m_edtValue);
	DDX_Control(pDX, IDC_EDIT_INITVALUE, m_edtInitValue);
	DDX_Control(pDX, IDC_EDIT_FMT, m_edtFmt);
	DDX_Control(pDX, IDC_COMBO_TYPE, m_combType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVarConfig, CDialog)
	//{{AFX_MSG_MAP(CVarConfig)
	ON_BN_CLICKED(IDC_BUTTON_ADDVALUE, OnButtonAddvalue)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_DELVALUE, OnButtonDelvalue)
	ON_NOTIFY(NM_CLICK, IDC_LIST_VAR, OnClickListVar)
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, OnSelchangeComboType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVarConfig message handlers

void CVarConfig::OnButtonAddvalue() 
{
	// TODO: Add your control notification handler code here
	if (m_edtValue.GetWindowTextLength()>0)
	{
		CString ss;
		char tmpchar[40];
		m_edtValue.GetWindowText(ss);
		int recNum = m_lstVarValue.GetItemCount();
		
		sprintf(tmpchar,"%d",recNum+1);
		int nItem = m_lstVarValue.InsertItem(recNum,tmpchar);
		m_lstVarValue.SetItemText(nItem,1,ss);

// 		m_xmlpack.m_xml.setCurrentNode(m_currNode);
// 		m_xmlpack.m_xml.addItem(ss.GetBuffer(ss.GetLength()),"");
		if (m_currNode != NULL)
		{
			m_currNode->AddChildNode(ss.GetBuffer(ss.GetLength()));
		}
		m_xmlpack.ToFile("vardefine.xml");
		m_edtValue.SetFocus();
		m_edtValue.SetWindowText("");
	}
	else
	{
		MessageBox("���������ֵ","��ʾ");
		m_edtValue.SetFocus();
	}
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
void CVarConfig::OnButtonDel() 
{
	// TODO: Add your control notification handler code here
	CString ss;
	int ret;
	int nItem;
	char tmpchar[500];
	char sSql[500];
	int funcNo;
	ret = m_lstVar.GetSelectedCount();
	if   (ret < 1)  
	{
        MessageBox("��ѡ������һ����¼","��ʾ");
		m_lstVarValue.SetFocus();
		return;
	}
	std::list<MyStruct>list_iItem;
	POSITION pos = m_lstVar.GetFirstSelectedItemPosition();
	if   (pos   ==   NULL)  
	{
		MessageBox("��ѡ������һ����¼","��ʾ");
		m_lstVarValue.SetFocus();
		return;
	}
	else  
	{  
		while   (pos)  
		{  
			nItem   =   m_lstVar.GetNextSelectedItem(pos);
			MyStruct d;
			d.ID = nItem;
			list_iItem.push_back(d);
			
		}
		std::list<MyStruct>::iterator it;
		list_iItem.sort();
		int aa;
		for(it =list_iItem.begin();it !=list_iItem.end();it++)
		{
			memset(tmpchar,0,sizeof(tmpchar));
			m_lstVar.GetItemText(it->ID, 1, tmpchar, 300);
			m_lstVar.DeleteItem(it->ID);
			
			CXmlNode *headnode = m_xmlpack.GetNodeByPath("/package/head",false);
			headnode->DeleteChildNode(tmpchar);
		}
		m_xmlpack.ToFile("vardefine.xml");
	}
}

void CVarConfig::OnButtonDelvalue() 
{
	// TODO: Add your control notification handler code here
	CString ss;
	int ret;
	int nItem;
	char tmpchar[500];
	char sSql[500];
	int funcNo;
	ret = m_lstVarValue.GetSelectedCount();
	if   (ret < 1)  
	{
        MessageBox("��ѡ������һ����¼","��ʾ");
		m_lstVarValue.SetFocus();
		return;
	}
	std::list<MyStruct>list_iItem;
	POSITION pos = m_lstVarValue.GetFirstSelectedItemPosition();
	if   (pos   ==   NULL)  
	{
		MessageBox("��ѡ������һ����¼","��ʾ");
		m_lstVarValue.SetFocus();
		return;
	}
	else  
	{  
		while   (pos)  
		{  
			nItem   =   m_lstVarValue.GetNextSelectedItem(pos);
			MyStruct d;
			d.ID = nItem;
			list_iItem.push_back(d);
			
		}
		std::list<MyStruct>::iterator it;
		list_iItem.sort();
		int aa;
		for(it =list_iItem.begin();it !=list_iItem.end();it++)
		{
			memset(tmpchar,0,sizeof(tmpchar));
			m_lstVarValue.GetItemText(it->ID, 1, tmpchar, 300);
			m_lstVarValue.DeleteItem(it->ID);
			
			if (m_currNode != NULL)
			{
				m_currNode->DeleteChildNode(tmpchar);
				m_xmlpack.ToFile("vardefine.xml");
			}
		}
	}
}

void CVarConfig::OnClickListVar(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
	char nodepathname[400];
	char tmpchar[400];
	char tmpchar2[400];
	char vartype[100];
	char varlen[100];
	memset(varlen,0,sizeof(varlen));
	memset(vartype,0,sizeof(vartype));
	memset(tmpchar,0,sizeof(tmpchar));
	POSITION pos = m_lstVar.GetFirstSelectedItemPosition();
	if   (pos   ==   NULL)  
	{
		MessageBox("��ѡ������һ����¼","��ʾ");
		m_lstVar.SetFocus();
		return;
	}
	int nItem   =   m_lstVar.GetNextSelectedItem(pos);
	m_lstVar.GetItemText(nItem, 1, tmpchar, 300);
	m_lstVar.GetItemText(nItem, 2, vartype, 99);
	m_lstVar.GetItemText(nItem, 3, varlen, 99);
	if (atoi(vartype)>0)
	{
		m_lstVarValue.DeleteAllItems();
		m_edtValue.EnableWindow(true);
		m_edtValue.SetWindowText("");
		m_edtValue.SetLimitText(atoi(varlen));
		m_btnAddValue.EnableWindow(true);
		m_btnDel.EnableWindow(true);
		sprintf(tmpchar2,"�б����[%s]��ֵ",tmpchar);
		GetDlgItem(IDC_STATIC_LISTVALUE)->SetWindowText(tmpchar2);

		sprintf(nodepathname,"/package/head/%s",tmpchar);

		m_currNode = m_xmlpack.GetNodeByPath(nodepathname);
		if (m_currNode != NULL)
		{
			CXmlNode *childnode = (CXmlNode *)m_currNode->GetFirstChild();
			while (childnode != NULL)
			{
				int recNum = m_lstVarValue.GetItemCount();
				sprintf(tmpchar2,"%d",recNum+1);
				nItem = m_lstVarValue.InsertItem(recNum,tmpchar2);
				m_lstVarValue.SetItemText(nItem,1,childnode->GetNodeName().c_str());
				childnode = (CXmlNode *)childnode->getNextSibling();
			}
		}
	}
	else
	{
		m_lstVarValue.DeleteAllItems();
		GetDlgItem(IDC_STATIC_LISTVALUE)->SetWindowText("�б����ֵ");
		m_edtValue.EnableWindow(false);
		m_edtValue.SetWindowText("");
		m_btnAddValue.EnableWindow(false);
		m_btnDel.EnableWindow(false);
	}
}

BOOL CVarConfig::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_combType.ResetContent();
	m_combType.FormatComboBox();
	m_combType.SetColumnWidth(40, 40);
	m_combType.SetColumnAlignStyle(DT_LEFT, DT_LEFT);	
	m_combType.AddRow("0","��ˮ�������Զ���1");
	m_combType.AddRow("1","�б��������");
	m_combType.AddRow("2","�б�˳�����");
	m_combType.SetCurSel(0);
	m_edtVarLen.SetWindowText("8");
	m_edtInitValue.SetWindowText("1");
	m_edtFmt.SetWindowText("%08I64d");
	m_btnAddValue.EnableWindow(false);
	m_btnDel.EnableWindow(false);
	m_edtValue.EnableWindow(false);
	
	m_lstVar.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_HEADERDRAGDROP);
	m_lstVar.InsertColumn(0, "���", LVCFMT_CENTER, 50);
	m_lstVar.InsertColumn(1, "��������", LVCFMT_CENTER, 200);
	m_lstVar.InsertColumn(2, "��������", LVCFMT_CENTER, 80);
	m_lstVar.InsertColumn(3, "��������", LVCFMT_CENTER, 80);
	m_lstVar.InsertColumn(4, "������ʽ", LVCFMT_CENTER, 100);
	m_lstVar.InsertColumn(5, "������ʼֵ", LVCFMT_CENTER, 100);
	m_lstVarValue.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_HEADERDRAGDROP);
	m_lstVarValue.InsertColumn(0, "���", LVCFMT_CENTER, 50);
	m_lstVarValue.InsertColumn(1, "����ֵ", LVCFMT_CENTER, 330);
	if (!m_xmlpack.FromFile("vardefine.xml"))
	{
		
	}
	m_currNode = NULL;
	string type;
	string len;
	string fmt;
	string initvalue;
	char tmpchar[40];

	CXmlNode *headnode = m_xmlpack.SetNodeValueByPath("/package/head",false,"",false);
	if (headnode == NULL)
	{
		return TRUE;
	}
	CXmlNode *tmpnode = (CXmlNode *)headnode->GetFirstChild();

	while (tmpnode != NULL)
	{
		if (tmpnode->GetAttribute("type",false,type) == NULL)
		{
			tmpnode = (CXmlNode *)tmpnode->getNextSibling();
			continue;
		}
		if (tmpnode->GetAttribute("len",false,len) == NULL)
		{
			tmpnode = (CXmlNode *)tmpnode->getNextSibling();
			continue;
		}
		if (type.compare("0") == 0)
		{
			if (tmpnode->GetAttribute("fmt",false,fmt) == NULL)
			{
				tmpnode = (CXmlNode *)tmpnode->getNextSibling();
				continue;
			}
			if (tmpnode->GetAttribute("initvalue",false,initvalue) == NULL)
			{
				tmpnode = (CXmlNode *)tmpnode->getNextSibling();
				continue;
			}
			int recNum = m_lstVar.GetItemCount();
			
			sprintf(tmpchar,"%d",recNum+1);
			int nItem = m_lstVar.InsertItem(recNum,tmpchar);
			m_lstVar.SetItemText(nItem,1,tmpnode->GetNodeName().c_str());
			m_lstVar.SetItemText(nItem,2,type.c_str());
			m_lstVar.SetItemText(nItem,3,len.c_str());
			m_lstVar.SetItemText(nItem,4,fmt.c_str());
			m_lstVar.SetItemText(nItem,5,initvalue.c_str());
			
			tmpnode = (CXmlNode *)tmpnode->getNextSibling();
		}
		else
		{
			int recNum = m_lstVar.GetItemCount();
			
			sprintf(tmpchar,"%d",recNum+1);
			int nItem = m_lstVar.InsertItem(recNum,tmpchar);
			m_lstVar.SetItemText(nItem,1,tmpnode->GetNodeName().c_str());
			m_lstVar.SetItemText(nItem,2,type.c_str());
			m_lstVar.SetItemText(nItem,3,len.c_str());
			
			tmpnode = (CXmlNode *)tmpnode->getNextSibling();
		}
		
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CVarConfig::OnOK() 
{
	// TODO: Add extra validation here
	CString csName;
	CString csLen;
	CString csFmt;
	CString csInitData;

	char nodepathname[400];
	
	m_edtVarName.GetWindowText(csName);
	m_edtVarLen.GetWindowText(csLen);
	m_edtFmt.GetWindowText(csFmt);
	m_edtInitValue.GetWindowText(csInitData);
	if (m_edtVarName.GetWindowTextLength()<1)
	{
		MessageBox("�������������","��ʾ");
		m_edtVarName.SetFocus();
		return ;
	}
	if (csName.GetLength()  > atoi(csLen.GetBuffer(csLen.GetLength()))-2)
	{
		MessageBox("�������Ƴ��Ȳ��ܴ��ڱ�������-2","��ʾ");
		m_edtVarName.SetFocus();
		return ;
	}
	if (m_edtVarLen.GetWindowTextLength()<1)
	{
		MessageBox("�������������","��ʾ");
		m_edtVarLen.SetFocus();
		return ;
	}
	if (atoi(m_combType.GetCurColumnItem(0))==0)
	{
		if (m_edtFmt.GetWindowTextLength()<1)
		{
			MessageBox("�����������ʽ��","��ʾ");
			m_edtFmt.SetFocus();
			return ;
		}
		if (m_edtInitValue.GetWindowTextLength()<1)
		{
			MessageBox("�����������ʼֵ","��ʾ");
			m_edtInitValue.SetFocus();
			return ;
		}
	}
	string stemp;
	sprintf(nodepathname,"/package/head/%s",csName.GetBuffer(csName.GetLength()));
	if (m_xmlpack.GetNodeByPath(nodepathname) != NULL)
	{
		MessageBox("�ñ����Ѵ���","��ʾ");
		m_edtVarName.SetFocus();
		return ;
	}
	
	CXmlNode *addnode = m_xmlpack.SetNodeValueByPath(nodepathname,true,"");
	addnode->SetAttribute("type",false,m_combType.GetCurColumnItem(0));
	addnode->SetAttribute("len",false,csLen.GetBuffer(csLen.GetLength()));
	addnode->SetAttribute("fmt",false,csFmt.GetBuffer(csLen.GetLength()));
	addnode->SetAttribute("initvalue",false,csInitData.GetBuffer(csLen.GetLength()));
	
	m_lstVarValue.DeleteAllItems();
	m_btnAddValue.EnableWindow(false);
	m_btnDel.EnableWindow(false);
	m_edtValue.EnableWindow(false);
	m_xmlpack.ToFile("vardefine.xml");
	
	int recNum = m_lstVar.GetItemCount();
	char tmpchar[100];
	sprintf(tmpchar,"%d",recNum+1);
	int nItem = m_lstVar.InsertItem(recNum,tmpchar);
	m_lstVar.SetItemText(nItem,1,csName);
	m_lstVar.SetItemText(nItem,2,m_combType.GetCurColumnItem(0));
	m_lstVar.SetItemText(nItem,3,csLen);
	m_lstVar.SetItemText(nItem,4,csFmt);
	m_lstVar.SetItemText(nItem,5,csInitData);
//	CDialog::OnOK();
}

void CVarConfig::OnSelchangeComboType() 
{
	// TODO: Add your control notification handler code here
	if (atoi(m_combType.GetCurColumnItem(0))>0)
	{
		m_edtFmt.EnableWindow(false);
		m_edtInitValue.EnableWindow(false);
		m_edtFmt.SetWindowText("");
		m_edtInitValue.SetWindowText("");
	}
	else
	{
		m_edtFmt.EnableWindow(true);
		m_edtInitValue.EnableWindow(true);
		m_edtInitValue.SetWindowText("1");
		m_edtFmt.SetWindowText("%08I64d");
	}
}
