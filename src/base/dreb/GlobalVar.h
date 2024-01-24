// GlobalVar.h: interface for the CGlobalVar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLOBALVAR_H__C0EB044C_DC92_47A0_B81E_4B1EC2FC1710__INCLUDED_)
#define AFX_GLOBALVAR_H__C0EB044C_DC92_47A0_B81E_4B1EC2FC1710__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "BF_Mutex.h"
#include "DrebStatInfo.h"
#include "BF_Date_Time.h"
#include "BF_Tools.h"
#include "BF_LogClient.h"
#include "BF_CommDataLog.h"

//数据总线结构
typedef struct 
{
	std::string sname;  //数据总线名称
	std::string sIp;    //连接的数据总线IP
	unsigned int    nPort;  //端口
	unsigned int    nSpeed; //速率，路由选择时一个条件 
}S_CENTERINFO;

#ifdef __cplusplus
extern "C" {
#endif
	
	//字节对齐
#ifdef _WINDOWS
#define __PACKED__ 
#pragma pack(push,1)
#else
#ifdef __GNUC__
#define __PACKED__	__attribute__ ((packed))
	//#pragma pack(push,1)
#else
#define __PACKED__
#pragma options align=packed
#endif
#endif

typedef struct
{
	long  nSerial_get;       //唯一流水
	int   serial;     //备用
}__PACKED__ S_SHM_PUBLIC;
//取消字节对齐
#ifdef _WINDOWS
#pragma pack(pop)
#else
#ifdef __GNUC__
//#pragma pack(pop)
#else
#pragma options align=reset
#endif
#endif
#undef __PACKED__

#ifdef __cplusplus
}
#endif

typedef std::vector <S_CENTERINFO> S_DREBINFO;



class CGlobalVar  
{
public:
	CGlobalVar();
	virtual ~CGlobalVar();

public:
	
	bool g_bIsExit;            //是否退出
	std::string  g_sConfFileName ; //配置文件名
	char m_errMsg[256];        //出错信息

	char g_curentpath[256];    //程序运行目录
	char g_modulename[256];    //程序名称
	char g_modulepath[256];    //程序所在目录
	
public:
	int  g_nPort;              //侦听端口
	int  g_nDrebId;            //公共节点号
	int  g_nDrebPrivateId;     //私有节点号

	int g_nRegOtherDreb;       //是否向其它总线注册
	int g_nZip;                //是否压缩
	int g_nMaxConnCount;       //最大连接数，不要大于640
	int g_nBeatHeart;          //是否有心跳1有0无
	int g_nHeartCount;         //心跳个数，在发出beatheart个心跳未收到响应后，将主动关闭连接，取值1到20
	int g_nBufferLimit;        //连接上的发送缓存包数0为不限制，最小的限制是100
	int g_nMsgProcThread;      //是否单独处理线程 处理线程通过消息队列进行  >0为处理线程数

	int g_nSvrDeleteTime;      //服务断开后删除服务队列时间
	int g_nServiceDeleteTime;  //服务功能路由过期时间

	int g_nDisconnectTime;     //针对客户端
	
	int g_nServiceRouteTime;   //发送服务路由功能的时间间隔
	int g_nDrebRouteTime;      //DREB节点的路由时间

	int g_nMaxQueue;  //发送缓冲满时，队列里的最大报文个数
//	int g_nModuleThreadNum;    //处理线程的个数

	int g_nLogLevel;           //日志级别
	int g_nDataLogLevel;       //data日志级别
	int g_nMaxLogSize;         //字节数<1024不限制
	int g_nMaxDataLogSize;         //字节数<1024不限制  

	int g_nCpuCore;//绑定的cpu核心

	int g_nCrcFlag; //crc校验标志  0不校验不生成  1校验 生成
	std::string g_sLogFileName;   //日志文件名
	std::string g_sLogFilePath;   //日志文件目录
	
	S_DREBINFO g_pCenterList;  //连接的通讯平台列表

	int   g_nUseMonitor;       //是否监控
	int   g_nMonitorHost;      //是否报告主机资源
	int   g_nMonitorDrebId;            //公共节点号
	int   g_nMonitorDrebPrivateId;     //私有节点号
	int   g_nMonitorSvrId;            //公共服务节点号
	int   g_nMonitorSvrPrivateId;     //私有服务节点号
	int   g_nMonitorTxCode;           //监控交易码
	
	CDrebStatInfo  g_pDrebStatInfo;
	CBF_Date_Time  g_pDate;

	CBF_LogClient      g_pLog;     //日志

	CBF_LogClient      g_pRouteLog;     //路由日志

	int  g_nPrintRoute;//是否记录路由信息至日志

	int  g_nSupportSubscribe;//是否支持广播订阅，只有当没有指定服务时，才看此参数。即不支持订阅时，广播没有指定服务则发给所有服务，否则发给订阅了的服务

	CBF_CommDataLog    g_pDataLog; //二进制报文日志

	int  g_nMaxRouteStep;//最大路由步进
	
	int  m_bIsRecTime;
public:
	void LogTime();
	/// 函数名: Init
	/// 编程  : 王明松 2009-5-16 16:03:39
	/// 返回  : bool 
	/// 描述  : 配置初始化
	bool Init(const char *confile);

	int getTick();

	S_SHM_PUBLIC *m_pShmpub;//共享内存中的数据
	
	void PutTime();

	std::vector<S_MONITOR_INFO> m_vBcDest;//将本节点的广播转发给指定总线节点的

protected:
	
	std::vector<INT64_> m_vTime;
	
	CBF_Mutex m_mutex;
};

#endif // !defined(AFX_GLOBALVAR_H__C0EB044C_DC92_47A0_B81E_4B1EC2FC1710__INCLUDED_)
