
// MonitorClientDlg.h: 头文件
//

#pragma once

#include "BF_Date_Time.h"
#include "BF_Timer.h"
#include "BF_Xml.h"
#include "DbApi.h"
#include "BF_SimpleLog.h"
#include "HwPlayer.h"
#include "ColorListCtrl.h"

#define  ONTIMER1 24   //显示时间
#define  ONTIMER2 25   //定时监控
#define  WM_USER_UPDATE  WM_USER+100

static UINT indicators[] =
{
    //设置小格格
    ID_SEPARATOR,           // status line indicator
    ID_SEPARATOR,
    ID_SEPARATOR,
};


typedef struct S_APP_NODE_INFO
{
    int  public_id;//序号
	int  private_id;//序号
	char app_name[200];
	int  app_type;
	int  pc_node_id;
	char  rpt_date[9];
	int   rpt_time;
	int  node_status;
	char start_time[21];
	char pc_name[200];
};

typedef struct S_PC_NODE_INFO
{
    int  pc_node_id;//序号
    char pc_name[200];
	char sip[33];
	char area[401];
	int warn_mem;
	int warn_cpu;
	int warn_disk;
};

typedef struct S_PC_NODE_DISK_INFO
{
    int  pc_node_id;//序号
    char pc_name[200];
    char diskname[201];
    int total;
    int used;
    int warn_disk;
};
typedef struct S_PC_NODE_CPU_RAM_INFO
{
    int  pc_node_id;//序号
    char pc_name[200];
    int cpurate;
    int totalram;
	int ramused;
    int warn_mem;
    int warn_cpu;
};


// CMonitorClientDlg 对话框
class CMonitorClientDlg : public CDialogEx
{
// 构造
public:
	CMonitorClientDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MONITORCLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	CStatusBar m_statusbar;
    CBF_Date_Time   m_pDate;
    CBF_Xml  m_pXml;
    CBF_SimpleLog  m_pLog;
	CDbApi         *m_pDbApi;

	UINT m_timerID1;//定时器1	 显示时间
	UINT m_timerID2;//定时器2	 监控

	unsigned int m_nMonitorSep;
	unsigned int m_nBeginTime;
	unsigned int m_nEndTime;
	std::string m_errMsg;
	std::string m_sDbUser;
	std::string m_nDbPwd;
	std::string m_nDbName;
	int         m_nDbType;
	void Monitor();
	bool ConnectDb();
	void GetAppNodeInfo();
	void GetPcNodeInfo();
	void GetPcCpuMemDiskInfo();
	char m_sSql[1000];

	std::vector<S_APP_NODE_INFO> m_vAppNodeInfo;
	std::vector<S_PC_NODE_INFO> m_vPcNodeInfo;
	std::vector<S_PC_NODE_DISK_INFO> m_vPcNodeDiskInfo;
	std::vector<S_PC_NODE_CPU_RAM_INFO> m_vPcNodeCpuRamInfo;

	void UpdateAppNode();
	void UpdatePcNode();
	void UpdatePCpuMemDisk();
	void autoDisplay();

    CHwPlayer m_playmusic;
    bool m_picstatus_app;
	bool m_picstatus_pc;

	MCI_OPEN_PARMS  m_music_open;
	bool m_bIsPlaying;
	bool PlayMusic(const char *musicfile);
	void StopMusic();
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	// 开始时间
	CEdit m_edtBegin;
	// 结束时间
	CEdit m_edtEnd;
	// 监控间隔
	CEdit m_edtSep;
	CColorListCtrl m_lstAppNode;
	//CListCtrl m_lstAppNode;
	// cpu及内存使用
	CListCtrl m_lstCpuRam;
	// 磁盘使用
	CListCtrl m_lstDisk;
	// 服务器信息
	CListCtrl m_lstPcNode;
	// 停止按钮
	CButton m_btnStop;
	// 设置数据库
	CButton m_btnSet;
	// 启动监控
	CButton m_btnOk;
	afx_msg void OnBnClickedButtonSet();
};
