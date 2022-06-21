// GResource.h: interface for the CGResource class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRESOURCE_H__A7724768_0CB1_4D54_B228_60D1463D2AED__INCLUDED_)
#define AFX_GRESOURCE_H__A7724768_0CB1_4D54_B228_60D1463D2AED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "BF_Mutex.h"
#include "XmlPack.h"
#include "BF_LogClient.h"
#include "BF_Date_Time.h"
#include "IGResource.h"
#include "BF_Tools.h"

//配置文件范例
/*
<?xml version="1.0" encoding="GB2312"?>
<package>
	<head>
		<公共配置>
			<日志级别 name="日志跟踪级别0严重错误1重要错误2一般错误3警告4提示警告5调试信息">5</日志级别>
			<日志文件 name="记录日志信息的文件名,前加日期">test.log</日志文件>
		</公共配置>
		<前置配置>
			<LISTENPORT name="对外提供服务的侦听端口">9001</LISTENPORT>
			<处理线程数 name="启动的处理线程数，根据并发数来配置，最小为ACCEPT线程数">40</处理线程数>
			<通讯报文头长度 name="通讯报文头长度,最小为4">5</通讯报文头长度>
			<通讯报文头是否包含自身 name="通讯报文头是否包含本身的长度0不包含">0</通讯报文头是否包含自身>
			<最大连接数 name="可以同时接受socket连接的最大数，0不限制最小100">0</最大连接数>
			<未使用断开时间 name="如果在连接池时间超过此设置则断开，单位秒，最小60">600</未使用断开时间>
			<心跳时间 name="超时此时间若连接未使用，主动发送心跳"></心跳时间>
			<动态库路径 name="前置使用的动态库的路径，若不填或错误的路径将按PATH或LIBPATH来打开">D:\工作\bubase开发框架\lib</动态库路径>
			<测试回路 name="不使用处理动态库，直接将请求原封不动的返回,生产请置为0">1</测试回路>
			<连接池接收数据 name="连接池线程接收数据，然后交给处理线程1，否则由处理线程去接收数据0">1</连接池接收数据>
			<处理动态库名称 name="前置处理线程动态库的名称及实例方法名" method="getProcessInstance">dllrunfrontd.dll</处理动态库名称>
		</前置配置>
		<服务配置 name="前置接入配置，IP接入的IP地址，trans为1表示要进行包转换，包转换配置文件为filename的值，ID为接入序号，IP和ID不能够重复,若不配置此项表示所以均可接入，是否包转换由前面的是否包转换来决定。">
			<? <名称 IP="192.168.22.47" Port="7200" trans="1" filename="D:\工作\BUBASE开发框架\etc\fjsms.xml" id="1"/>?>
			<?<名称 IP="127.0.0.2" Port="7200" trans="1" filename="D:\工作\BUBASE开发框架\etc\fjsms.xml" id="0"/>?> 
		</服务配置>
	</head>
	<pack>
	</pack>
</package>

*/
#define  MAXPROCESSTHREAD 400  //最大处理线程数


#ifdef WIN32
#define INT64  __int64 
#else
#define INT64  long long 
#endif

typedef struct 
{
	char  sip[21];
	int   nLen;
}S_IP_HEAD; 

class  CGResource  : public CIGResource 
{
public:
	
	CGResource();
	virtual ~CGResource();

	

public:
	bool g_bToExit;//是否退出程序
	
	int g_nPort;//侦听端口 默认为9001
	int g_nLoglevel;//日志级别 默认为5
	std::string g_sLogFileName;//日志文件名
	std::string g_sLogFilePath;//日志目录
	int g_nLogSize;

	int  g_nProcessThreadNum;//处理线程数,默认为10
	int g_nMaxConnectNum;//最大连接数 0为无限制，默认
	int g_nDisconnectTime;//未使用断开时间，如果一个连接在此时间内一直没有使用，则将此连接断开。单位秒默认600秒即10分钟
	int g_nDispatchTime;//分配最大时间

	int g_nQueueDelayTime;//队列超时时间
	int g_nHeadNum;//通讯报文头长度，默认为5
	unsigned int g_nHeartRun;//心跳时间,当发现连接超过此时间未用时，主动发送心跳


	unsigned long  g_processNum;//处理请求数
	unsigned long  g_processSucessNum;//处理成功请求数

	bool g_bIsMonitor;//是否监控信息


	int g_nBuPort;//bu侦听端口
	std::string g_sBuPrgName;//bu程序名
	std::string g_sBuPrgPath;//bu程序的路径
	std::string g_sBuRunPath;//bu运行目录
	std::string g_sBuPrgPara;//bu的启动参数
	bool g_bIsAutoLoadBu;//是否自助装载BU

	CBF_LogClient m_log;//日志类
public:
	int GetHeadLenByIp(const char  *ip);

	// 函数名: WriteBack
	// 编程  : 王明松 2010-3-16 14:58:20
	// 返回  : void 
	// 描述  : 写回配置文件
	void WriteBack();

	std::string m_frontParaFileName;//前置配置文件

	


	/// 函数名: PutProcessNum
	/// 编程  : 王明松 2009-1-6 10:20:11
	/// 返回  : int 
	/// 描述  : 放置处理请求数
	int PutProcessNum();

	/// 函数名: PutProcessSucessNum
	/// 编程  : 王明松 2009-1-6 10:20:14
	/// 返回  : int 
	/// 描述  : 放置成功处理请求数
	int PutProcessSucessNum();

	/// 函数名: GetGenerateSerial
	/// 编程  : 王明松 2008-9-5 13:18:17
	/// 返回  : INT64 
	/// 描述  : 获取连接的次数
	unsigned long  GetGenerateSerial();

	/// 函数名: SetListen
	/// 编程  : 王明松 2008-9-5 9:22:36
	/// 返回  : bool 
	/// 描述  : 将侦听标志置为true,若已侦听则返回false,否则将侦听标志置为true并返回true
	bool SetListen();


	/// 函数名: SetInitOnce
	/// 编程  : 王明松 2008-9-4 9:17:41
	/// 返回  : bool true要初始，false无需初始
	/// 描述  : 将初始化标志置为true,若已初始则返回false,否则将初始标志置为true并返回true
	bool SetInitOnce();


	/// 函数名: OpenDll
	/// 编程  : 王明松 2008-9-4 9:15:14
	/// 返回  : bool 
	/// 参数  : DLLHANDLE *pDll 动态库地址
	/// 参数  : const char *dllname 动态库名称
	/// 参数  : const char *dllpath 动态库路径
	/// 描述  : 打开动态库，传出动态库地址
	bool OpenDll(DLLHANDLE *pDll,const char *dllname,const char *dllpath);

	/// 函数名: GetMsg
	/// 编程  : 王明松 2008-9-2 14:29:00
	/// 返回  : string 
	/// 描述  : 取得出错信息
	std::string GetMsg();

	/// 函数名: Init
	/// 编程  : 王明松 2008-9-2 11:55:26
	/// 返回  : bool 
	/// 参数  : char *confile
	/// 描述  : 全局参数初始化
	virtual bool Init(char *confile);


	/// 函数名: GetSerial
	/// 编程  : 王明松 2008-9-2 11:47:48
	/// 返回  : unsigned long 
	/// 描述  : 取得唯一连接序号
	unsigned long GetSerial();

	/// 函数名: SetGPara1
	/// 编程  : 王明松 2008-12-30 9:42:48
	/// 返回  : virtual bool 
	/// 参数  : bool setflag=true
	/// 描述  : 设置全局变量1,若已设置则返回false
	virtual bool SetGPara1(bool setflag=true);
	
	
	/// 函数名: isSetGPara2
	/// 编程  : 王明松 2008-12-30 9:36:10
	/// 返回  : virtual bool 
	/// 参数  : bool setflag=true
	/// 描述  : 设置全局变量2,若已设置则返回false
	virtual bool SetGPara2(bool setflag=true);
	
	
	/// 函数名: isSetGPara3
	/// 编程  : 王明松 2008-12-30 9:36:13
	/// 返回  : virtual bool 
	/// 参数  : bool setflag=true
	/// 描述  : 设置全局变量3,若已设置则返回false
	virtual bool SetGPara3(bool setflag=true);

	/// 函数名: GetThreadShareSerial
	/// 编程  : 王明松 2008-12-30 9:36:13
	/// 返回  : virtual long 
	/// 描述  : 取得唯一的流水号
	virtual long GetThreadShareSerial();

protected:
	std::vector<S_IP_HEAD> m_ip_head_list;//ip地址对应的通讯头额外长度
	unsigned long m_serial_connect;//连接序号
	CBF_Mutex m_mutex;//互斥变量
	std::string m_errMsg;//错误信息
	

	CBF_Date_Time m_publog;
	CBF_Tools     m_pTool;
	bool g_bIsInitOnce;//是否初始化一次，即在一个线程里进行初始化后，将此标志置为1，其它线程判断此标志是否进行初始化
	bool g_bIsListen;//是否侦听，只能调用一次

	unsigned long m_nMaxSerial;//最大的流水
	unsigned long m_nSerialNum;//流水次数

	CBF_Mutex m_mutex_putprocess;//互斥变量
	CBF_Mutex m_mutex_putprocesssucess;//互斥变量

	//全局变量
	CBF_Mutex m_mutex_setgpara1;
	CBF_Mutex m_mutex_setgpara2;
	CBF_Mutex m_mutex_setgpara3;
	bool   m_bisSetGPara1;
	bool   m_bisSetGPara2;
	bool   m_bisSetGPara3;


	CBF_Mutex m_mutex_getSerial;//取唯一流水号
	long   m_nSerial_get;//流水序号
	long   m_nSerial_getMax;//最大流水序号
};

#endif // !defined(AFX_GRESOURCE_H__A7724768_0CB1_4D54_B228_60D1463D2AED__INCLUDED_)
