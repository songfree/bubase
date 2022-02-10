// BF_DrebResource.h: interface for the CBF_DrebResource class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BF_DREBRESOURCE_H__D94AB550_73E8_47AE_92EB_FB23D818A824__INCLUDED_)
#define AFX_BF_DREBRESOURCE_H__D94AB550_73E8_47AE_92EB_FB23D818A824__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "typedefine.h"
#include "BF_Xml.h"
#include "BF_Tools.h"
#include "IErrLog.h"
#include "BF_SimpleLog.h"
#include "BF_Mutex.h"

typedef struct
{
	std::string sIp;       //IP
	int         nPort;     //端口
	int         nBandwidth;//带宽
	bool        bIsIpc;
}S_DREBLINK_INFO;

typedef struct 
{
	UINT64_  g_nRcvBytes;//接收字节数
	UINT64_  g_nRcvPackNum;//接收数据包数
	UINT64_  g_nSendBytes;//发送字节数
	UINT64_  g_nSendPackNum;//发送数据包数
	
}S_DREBSTAT_INFO;  //连接总线的统计数据

#ifdef DREBAPI_EXPORTS
#define DREBAPI_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define DREBAPI_EXPORT __declspec(dllimport)
#else
#define DREBAPI_EXPORT 
#endif
#endif

class DREBAPI_EXPORT CBF_DrebResource 
{
public:
	

	CBF_DrebResource();
	virtual ~CBF_DrebResource();

	CBF_SimpleLog *log;

	//日志配置
	int g_nLoglevel;//日志级别 默认为LOG_DEBUG
	int g_nDrebLoglevel;//写入data下面的日志级别   默认为LOG_PROMPT
	std::string g_sLogFileName;//日志文件名
	std::string g_sLogFilePath;//日志文件目录
	unsigned int  g_nMaxLogSize;//日志最大大小 0不限制

	int           g_nCrcFlag; //crc校验标志  0不校验不生成  1校验 生成

	unsigned int  g_nEndianFlag; //针对调用方字节序处理标志,0不做处理，适合C/C++.  1使用网络序(SNEDMSG和getmsg)时使用网络序传输数据 适合JAVA调用(java不用处理字节序)

	//心跳连接配置
	int g_nDisconnectTime;//未使用断开时间，如果一个连接在此时间内一直没有使用，则将此连接断开。单位秒默认600秒即10分钟
	int g_nHeartRun;//心跳时间,当发现连接超过此时间未用时，主动发送心跳，默认为5秒 

	//主动发送监控的信息
	int   g_nUseMonitor;       //是否监控 默认为1
	int   g_nMonitorHost;      //是否报告主机资源 默认为1
	int   g_nMonitorDrebId;            //公共节点号 默认为0
	int   g_nMonitorDrebPrivateId;     //私有节点号 默认为0
	int   g_nMonitorSvrId;            //公共服务节点号 默认为0
	int   g_nMonitorSvrPrivateId;     //私有服务节点号 默认为0
	int   g_nMonitorTxCode;           //监控交易码     默认为99001

	//在总线上注册的公共服务号和私有服务号
	unsigned int   g_nSvrMainId;     //目标服务功能号 ,必须填写 默认为0
	unsigned int   g_nSvrPrivateId;  //目标私有序号 0表示公共 >0为每个服务的序号，必须填写 默认为0
	
	//要连接的dreb信息
	vector<S_DREBLINK_INFO> g_vDrebLinkInfo;  //要连接的dreb信息
	
	
	

	//以下三项为调用者在main里调用GetModuleNamePath取得运行程序的信息
	char g_sCurPath[300];     //当前目录
	char g_sModulePath[300];  //程序目录
	char g_sModuleName[300];  //程序名称

	//以下为本api运行时填写，供使用
	char g_sStartDateTime[20];  //不用填写，由程序运行填写
	
	S_DREBSTAT_INFO  g_DrebRcvSendSec;//每秒发送接收数据
	

	void ReportSend(unsigned int sendbytes,unsigned int sendpack=1);
	void ReportRcv(unsigned int rcvbyte,unsigned int packnum=1);
	void ComputeStat();

	std::string m_errMsg;//出错信息
	CBF_Xml     m_pXml;//xml配置


	virtual bool Init(const char *confile)
	{
		if (confile == NULL)
		{
			printf("file is null\n");
			m_errMsg = "file is null";
			return false;
		}
		
		if (!m_pXml.FromFile(confile))
		{
			printf("file [%s] error\n",confile);
			m_errMsg = "file error";
			return false;
		}
		CBF_Date_Time pdata;
		CBF_Tools::StringCopy(g_sStartDateTime,17,pdata.ToStringDT17().c_str());

		int ret=0;
		std::string stmp;
		
		if (m_pXml.GetNodeValueByPath("package/head/public/loglevel",false,g_nLoglevel) == NULL)
		{
			m_errMsg = "file error,no [package/head/public/loglevel] node";
			return false;
		}
		if (g_nLoglevel <0 )
		{
			g_nLoglevel = LOG_DEBUG;//配置错则为5
		}
		if (m_pXml.GetNodeValueByPath("package/head/public/dataloglevel",false,g_nDrebLoglevel) == NULL)
		{
			m_errMsg = "file error,no [package/head/public/dataloglevel] node";
			return false;
		}
		if (g_nDrebLoglevel <0 )
		{
			g_nDrebLoglevel = LOG_DEBUG;//配置错则为5
		}

		if (m_pXml.GetNodeValueByPath("package/head/public/logfile",false,g_sLogFileName) == NULL)
		{
			m_errMsg = "file error,no [package/head/public/logfile] node";
			return false;
		}
		if (m_pXml.GetNodeValueByPath("package/head/public/logdir",false,g_sLogFilePath) == NULL)
		{
			m_errMsg = "file error,no [package/head/public/logdir] node";
			g_sLogFilePath="";
		}
		if (m_pXml.GetNodeValueByPath("package/head/public/CRC",false,g_nCrcFlag) == NULL)
		{
			g_nCrcFlag=0;
		}
		if (m_pXml.GetNodeValueByPath("package/head/public/logsize",false,ret) == NULL)
		{
			m_errMsg = "file error,no [package/head/public/logsize] node";
			g_nMaxLogSize=0;
		}
		else
		{
			g_nMaxLogSize = ret;
		}

		if (m_pXml.GetNodeValueByPath("package/head/public/endian",false,ret) == NULL)
		{
			m_errMsg = "file error,no [package/head/public/endian] node";
			g_nEndianFlag=0;
		}
		else
		{
			g_nEndianFlag = ret;
		}
		if (g_nEndianFlag != 1)
		{
			g_nEndianFlag = 0;
		}
		
		if (m_pXml.GetNodeValueByPath("package/head/bpc/svrid",false,ret) == NULL)
		{
			m_errMsg = "file error,no [package/head/bpc/svrid] node";
			return false;
		}
		if (ret<1)
		{
			m_errMsg = "file error,[package/head/bpc/svrid] is illegal";
			return false;
		}
		g_nSvrMainId = ret;
		if (m_pXml.GetNodeValueByPath("package/head/bpc/svrpid",false,ret) == NULL)
		{
			m_errMsg = "file error,no [package/head/bpc/svrpid] node";
			return false;
		}
		if (ret<1 || ret>254)
		{
			m_errMsg = "file error [package/head/bpc/svrpid] is illegal";
			return false;
		}
		g_nSvrPrivateId = ret;


		if (m_pXml.GetNodeValueByPath("package/head/bpc/hearttime",false,g_nHeartRun) == NULL)
		{
			m_errMsg = "file error,no [package/head/bpc/hearttime] node";
			g_nHeartRun = 5;
		}
		if (m_pXml.GetNodeValueByPath("package/head/bpc/due_times",false,g_nDisconnectTime) == NULL)
		{
			m_errMsg = "file error,no [package/head/bpc/due_times] node";
			g_nDisconnectTime = 600;
		}
		g_nUseMonitor=0;
		if (m_pXml.GetNodeAttribute("package/head/monitor",false,"use",false,g_nUseMonitor) == NULL)
		{
			m_errMsg = "file error,no [package/head/monitor] node";
			g_nUseMonitor = 0;
		}
		if (g_nUseMonitor == 1)
		{
			if (m_pXml.GetNodeAttribute("package/head/monitor",false,"host",false,g_nMonitorHost) == NULL)
			{
				m_errMsg = "file error,no [package/head/monitor] host attribute";
				g_nMonitorHost = 1;
			}
			if (m_pXml.GetNodeValueByPath("package/head/monitor/drebid",false,g_nMonitorDrebId) == NULL)
			{
				m_errMsg = "file error,no [package/head/monitor/drebid] node";
				g_nMonitorDrebId = 0;
			}
			if (m_pXml.GetNodeValueByPath("package/head/monitor/drebpid",false,g_nMonitorDrebPrivateId) == NULL)
			{
				m_errMsg = "file error,no [package/head/monitor/drebpid] node";
				g_nMonitorDrebPrivateId = 0;
			}
			if (m_pXml.GetNodeValueByPath("package/head/monitor/svrid",false,g_nMonitorSvrId) == NULL)
			{
				m_errMsg = "file error,no [package/head/monitor/svrid] node";
				g_nMonitorSvrId = 0;
			}
			if (m_pXml.GetNodeValueByPath("package/head/monitor/svrpid",false,g_nMonitorSvrPrivateId) == NULL)
			{
				m_errMsg = "file error,no [package/head/monitor/svrpid] node";
				g_nMonitorSvrPrivateId = 0;
			}
			if (m_pXml.GetNodeValueByPath("package/head/monitor/txcode",false,g_nMonitorTxCode) == NULL)
			{
				m_errMsg = "file error,no [package/head/monitor/txcode] node";
				return false;
			}
		}
		CXmlNode *drebnode = m_pXml.GetNodeByPath("package/head/dreb",false);
		if (drebnode == NULL)
		{
			m_errMsg = "file error,no [package/head/dreb] node";
			return false;
		}
		CXmlNode *tmpnode = (CXmlNode *)drebnode->GetFirstChild();
		S_DREBLINK_INFO dreblinkinfo;
		while (tmpnode != NULL)
		{
			tmpnode->GetAttribute("ip",false,dreblinkinfo.sIp);
			tmpnode->GetAttribute("port",false,stmp);
			dreblinkinfo.nPort = atoi(stmp.c_str());
			tmpnode->GetAttribute("bandwidth",false,stmp);
			dreblinkinfo.nBandwidth = atoi(stmp.c_str());
			g_vDrebLinkInfo.push_back(dreblinkinfo);
			tmpnode = (CXmlNode *)tmpnode->getNextSibling();
		}
		return true;
	}
protected:
	CBF_Mutex        m_reportMutex;
	S_DREBSTAT_INFO  m_oldDrebStatInfo;//保存之前的接收发送信息
	S_DREBSTAT_INFO  g_DrebRcvSendStat;//发送接收累计数据
	
};

#endif // !defined(AFX_BF_DREBRESOURCE_H__D94AB550_73E8_47AE_92EB_FB23D818A824__INCLUDED_)
