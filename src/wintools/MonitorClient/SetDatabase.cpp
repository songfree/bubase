// SetDatabase.cpp: 实现文件
//

#include "pch.h"
#include "MonitorClient.h"
#include "SetDatabase.h"
#include "afxdialogex.h"
#include "BF_Des.h"
#include "BF_3Des.h"

#define  DBPWDKEY  "rd402wms"
#define  DBPWD3DESKEY   "rd402wms20092017Ylink123"

// CSetDatabase 对话框

IMPLEMENT_DYNAMIC(CSetDatabase, CDialogEx)

CSetDatabase::CSetDatabase(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_SET, pParent)
{

}

CSetDatabase::~CSetDatabase()
{
}

void CSetDatabase::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_DBTYPE, m_combDbtype);
    DDX_Control(pDX, IDC_COMBO_PWDFLAG, m_combPwdFlag);
    DDX_Control(pDX, IDC_EDIT_NAME, m_edtDbName);
    DDX_Control(pDX, IDC_EDIT_PWD, m_edtPwd);
    DDX_Control(pDX, IDC_EDIT_USER, m_edtUser);
}


BEGIN_MESSAGE_MAP(CSetDatabase, CDialogEx)
    ON_BN_CLICKED(IDOK, &CSetDatabase::OnBnClickedOk)
END_MESSAGE_MAP()


// CSetDatabase 消息处理程序


BOOL CSetDatabase::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
    std::string dbname;
    std::string dbuser;
    std::string dbpwd;
    std::string dbpwdflag;
    std::string dbtype;
    m_combPwdFlag.AddString("0-明文");
    m_combPwdFlag.AddString("1-des");
    m_combPwdFlag.AddString("2-3des");

    m_combDbtype.AddString("0-oracl");
    m_combDbtype.AddString("1-sybase");
    m_combDbtype.AddString("2-sqlserver");
    m_combDbtype.AddString("3-infomi");
    m_combDbtype.AddString("4-db2");
    m_combDbtype.AddString("5-odbc");
    m_combDbtype.AddString("6-mysql");

    if (!m_pXml.FromFile("monitorclient.xml"))
    {
        return true;
    }
    if (m_pXml.GetNodeValueByPath("package/head/business/database/databaseserver", false, dbname, false) == NULL)
    {
        return true;
    }
    if (m_pXml.GetNodeValueByPath("package/head/business/database/databaseuser", false, dbuser, false) == NULL)
    {
        return true;
    }
    if (m_pXml.GetNodeValueByPath("package/head/business/database/userpasswd", false, dbpwd, true) == NULL)
    {
        return true;
    }
    if (m_pXml.GetNodeValueByPath("package/head/business/database/passwdtype", false, dbpwdflag, false) == NULL)
    {
        return true;
    }
    if (m_pXml.GetNodeValueByPath("package/head/business/database/databasetype", false, dbtype, false) == NULL)
    {
        return true;
    }
    char tmpchar[200];
    strcpy(tmpchar, dbpwd.c_str());
    if (atoi(dbpwdflag.c_str()) == 1)
    {
        CBF_Des     pEncrypt;//des加解密类
        int len = strlen(tmpchar);
        if (pEncrypt.UncryptString(tmpchar, len, DBPWDKEY) < 0)
        {
            return TRUE;
        }
    }
    else if (atoi(dbpwdflag.c_str()) == 2)
    {
        int len = strlen(tmpchar);
        CBF_3Des des;
        char errmsg[200];
        if (!des.UnDes3Buffer(tmpchar, len, DBPWD3DESKEY, errmsg))
        {
            return TRUE;
        }
    }

    m_edtDbName.SetWindowText(dbname.c_str());
    m_edtUser.SetWindowText(dbuser.c_str());
    m_edtPwd.SetWindowText(tmpchar);
    m_combDbtype.SetCurSel(atoi(dbtype.c_str()));
    m_combPwdFlag.SetCurSel(atoi(dbpwdflag.c_str()));
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CSetDatabase::OnBnClickedOk()
{
    // TODO: 在此添加控件通知处理程序代码
    CString csDbName;
    CString csDbUser;
    CString csDbPwd;
    CString csPwdFlag;
    CString csDbType;

    if (m_edtDbName.GetWindowTextLength() < 1)
    {
        MessageBox("请输入数据库名字","提示");
        m_edtDbName.SetFocus();
        return ;
    }
    if (m_edtUser.GetWindowTextLength() < 1)
    {
        MessageBox("请输入数据库用户", "提示");
        m_edtUser.SetFocus();
        return;
    }
    if (m_edtPwd.GetWindowTextLength() < 1)
    {
        MessageBox("请输入数据库密码", "提示");
        m_edtPwd.SetFocus();
        return;
    }
    m_edtDbName.GetWindowText(csDbName);
    m_edtUser.GetWindowText(csDbUser);
    m_edtPwd.GetWindowText(csDbPwd);
    m_combPwdFlag.GetWindowText(csPwdFlag);
    m_combDbtype.GetWindowText(csDbType);

    char sDbtype[2];
    char sPwdFlag[2];
    sDbtype[0]=csDbType.GetBuffer()[0];
    sDbtype[1]=0;
    sPwdFlag[0]=csPwdFlag.GetBuffer()[0];
    sPwdFlag[1]=0;
    char tmpchar[200];
    strcpy(tmpchar, csDbPwd.GetBuffer());
    if (atoi(sPwdFlag) == 1)
    {
        CBF_Des     pEncrypt;//des加解密类
        int len = strlen(tmpchar);
        if (pEncrypt.EncryptString(tmpchar, len, DBPWDKEY) < 0)
        {
            MessageBox("数据库密码加密出错", "提示");
            m_edtPwd.SetFocus();
            return;
        }
    }
    else  if (atoi(sPwdFlag) == 2)
    {
        int len = sizeof(tmpchar);
        CBF_3Des des;
        char errmsg[200];
        if (!des.Des3Buffer(tmpchar, len, DBPWD3DESKEY, errmsg))
        {
            MessageBox("数据库密码加密出错", "提示");
            m_edtPwd.SetFocus();
            return;
        }
    }
    if (m_pXml.SetNodeValueByPath("package/head/business/database/databaseserver", false, csDbName.GetBuffer(), false) == NULL)
    {
        MessageBox("SetNodeValueByPath出错", "提示");
        return;
    }
    if (m_pXml.SetNodeValueByPath("package/head/business/database/databaseuser", false, csDbUser.GetBuffer(), false) == NULL)
    {
        MessageBox("SetNodeValueByPath出错", "提示");
        return;
    }
    if (m_pXml.SetNodeValueByPath("package/head/business/database/userpasswd", false, tmpchar, true) == NULL)
    {
        MessageBox("SetNodeValueByPath出错", "提示");
        return;
    }
    if (m_pXml.SetNodeValueByPath("package/head/business/database/passwdtype", false, sPwdFlag, false) == NULL)
    {
        MessageBox("SetNodeValueByPath出错", "提示");
        return;
    }
    if (m_pXml.SetNodeValueByPath("package/head/business/database/databasetype", false, sDbtype, false) == NULL)
    {
        MessageBox("SetNodeValueByPath出错", "提示");
        return;
    }
    if (!m_pXml.ToFile("monitorclient.xml"))
    {
        MessageBox("ToFile出错", "提示");
        return;
    }
    CDialogEx::OnOK();
}
