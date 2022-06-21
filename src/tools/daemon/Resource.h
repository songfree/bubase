// Resource.h: interface for the CResource class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESOURCE_H__FF782DDA_47F8_4B57_961C_BF9F48B676FC__INCLUDED_)
#define AFX_RESOURCE_H__FF782DDA_47F8_4B57_961C_BF9F48B676FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BF_Xml.h"
#include "BF_SimpleLog.h"
#include "BF_Date_Time.h"
#include "BF_Tools.h"
#include "MdbBase.h"
#include "MoniHead.h"
#include "BF_ProcessMgr.h"

#define  HEARTHEAD    8   //进程心跳的通讯头长度

class CResource  
{
public:
	CResource();
	virtual ~CResource();

	bool g_bIsExit;

	unsigned int   g_nSvrMainId;     //目标服务功能号 ,必须填写
	unsigned int   g_nSvrPrivateId;  //目标私有序号 0表示公共 >0为每个服务的序号，必须填写

	int   g_nUseMonitor;       //是否监控
	int   g_nMonitorHost;      //是否报告主机资源
	int   g_nMonitorDrebId;            //公共节点号
	int   g_nMonitorDrebPrivateId;     //私有节点号
	int   g_nMonitorSvrId;            //公共服务节点号
	int   g_nMonitorSvrPrivateId;     //私有服务节点号
	int   g_nMonitorTxCode;           //监控交易码
	
	char              g_sCurPath[300]; //当前运行目录
	char              g_sModulePath[300];//程序所在目录
	char              g_sModuleName[300];//程序名

	char              m_sStartDate[20];//启动日期
	
	int      g_nCrcFlag;//是否校验CRC

	int g_nLoglevel;//日志级别 默认为5
	std::string g_sLogFileName;//日志文件名
	std::string g_sLogFilePath;//日志文件目录
	int g_nHeartRun;//心跳时间,当发现连接超过此时间未用时，主动发送心跳
	int g_nDisconnectTime;//未使用断开时间，如果一个连接在此时间内一直没有使用，则将此连接断开。单位秒默认600秒即10分钟
	int g_nGwDisTime;  //转发连接未使用断开时间

	int g_nPort;   //侦听端口

	CBF_Xml  g_pXml; //xml解析类
	CBF_SimpleLog  g_pLog;
	CBF_Date_Time  m_pDate;

	CBF_Xml  g_pRunXml; //保存进程运行信息，如PID的xml文件
	char     m_sErrMsg[256]; //出错信息

	std::vector<S_DREBLINK_INFO> g_vDrebLinkInfo;//dreb连接列表
	std::vector<S_PROCESS_NODE>  g_vProcessInfo; //管理的进程信息
	
public:
	bool UpdateLastTimeById(std::string nodeid);

	// 函数名: Init
	// 编程  : 王明松 2013-12-10 9:46:25
	// 返回  : bool 
	// 参数  : const char *confile
	// 描述  : 读配置文件
	bool Init(const char *confile);


protected:
	
};

#endif // !defined(AFX_RESOURCE_H__FF782DDA_47F8_4B57_961C_BF9F48B676FC__INCLUDED_)
