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
#include "BF_Xml.h"
#include "BF_LogClient.h"
#include "BF_Tools.h"
#include "IGRes.h"
#include "BpcHead.h"
#include "BF_CommDataLog.h"
#include "BF_Queue.h"
#include "BF_DrebResource.h"

//配置文件范例
/*<?xml version="1.0" encoding="GB2312"?>
<package>
   <head>
      <公共配置>
         <日志级别 remark="日志跟踪级别0严重错误1重要错误2一般错误3警告4提示警告5调试信息">6</日志级别>
         <DATA日志级别 remark="日志跟踪级别0严重错误1重要错误2一般错误3警告4提示警告5调试信息">5</DATA日志级别>
         <日志文件 remark="记录日志信息的文件名,前加日期">bfbpc.log</日志文件>
         <日志目录 remark="日志存放的目录"></日志目录>
      </公共配置>
      <DREB配置>
            <DREB1 ip="127.0.0.1" port="9011" bandwidth="100" remark="配置数据总线的参数" />
			<DREB1 ip="127.0.0.1" port="9012" bandwidth="100" remark="配置数据总线的参数" />
      </DREB配置>
       <监控配置 use="1" host="1" remark="use为主动报告选项，0不报告 host为主机资源报告选择0不报告">
        <监控公共节点号>0</监控公共节点号>
        <监控私有节点号>0</监控私有节点号>
        <监控公共服务号>0</监控公共服务号>
        <监控私有服务号>0</监控私有服务号>
        <监控报告交易码>99001</监控报告交易码>
     </监控配置>
      <BPC配置>
         <公共服务号 remark="在dreb上注册的服务号">100</公共服务号>
         <私有服务号 remark="在dreb上注册的私有服务号">1</私有服务号>
         <最大流水计数 remark="Bpu共享的流水最大值，超过则从1重新开始">99999999</最大流水计数>
         <本次流水开始值 remark="Bpu共享的流水起始值">1</本次流水开始值>
         <请求分配超时时间 remark="请求分配给BU的超时时间，秒">10</请求分配超时时间>
         <连接过期时间 remark="当连接长时间未有数据时断开,秒">600</连接过期时间>
         <DREB心跳时间 remark="和DREB的心条时间,秒">60</DREB心跳时间>
         <交易注册模式 remark="0表示动态注册，1表示静态注册 2表示不注册">0</交易注册模式>
         <交易注册文件 remark="静态注册时的文件名">txlist.xml</交易注册文件>
      </BPC配置>
      
</package>
*/

#ifdef WIN32
#define INT64  __int64 
#else
#define INT64  long long 
#endif

#define  DISPATCHMODE_FREE  0
#define  DISPATCHMODE_ALL   1

#define  REGISTERMODE_DYNAMIC   0   //动态注册  BPU向BPC注册，BPC向总线注册
#define  REGISTERMODE_STATIC    1   //静态注册  BPC向总线注册，BPU不注册
#define  REGISTERMODE_NONE      2   //不注册    BPC不向总线注册，BPU向BPC注册


class CPoolData;
typedef struct 
{
	std::string g_sBpuGroupName;//组名称
	std::string g_sBuPrgName;//bpu程序名
	std::string g_sBuPrgPath;//bpu程序的路径
	std::string g_sBuRunPath;//bpu运行目录
	std::string g_sBuPrgPara;//bpu的启动参数
	bool g_bIsAutoLoadBu;//是否自助装载BPU
	int  g_nBpuNum;//可启动的bpu数目
	int  g_nStartBpuNum;//已启动的bpu数目
	bool g_bIsReg; //是否注册
	bool g_bIsAnsPing;//是否响应心跳
	bool g_bIsUnencrypt;//是否发送明文
	int  g_nDispatchMode;//分配模式 0表示空闲分配，1表示BPU数只能为1，一直分配
	int  g_nBeginSocketIndex;//此bpu组开始的socket index
	int  g_nEndSocketIndex;  //此bpu组结束的socket index，不含此
	int  g_nJavaBpuPid;//JAVA版本的BPU的进程ID
	int  g_nUseBigEndian;//JAVA版本的BPU，发送网络序

	CPoolData *pdispathQueue;//分派队列 每个bpu组一个
	int       bpugroupindex; //bpu组索引 
}S_BPUGROUP_INFO;

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

typedef struct 
{
	char      bpugroupname[21];//bpu组名
	int       bpugroupindex; //bpu组索引 
}S_BPUGROUP_TBL;
class  CGResource  : public CIGResource,public CBF_DrebResource 
{
public:
	
	CGResource();
	virtual ~CGResource();

public:
	S_SHM_PUBLIC  *m_pSerial;
	bool g_bToExit;//是否退出程序
	int    g_nPort;//侦听端口 默认为9001

 	std::string m_sBpcParaFileName;//配置文件名称

	int g_nDispatchTime;//消息分配的最大时间

	int g_nRegisterMode;//交易注册模式   0表示动态注册，1表示静态注册 2表示不注册
	std::string g_sRegisterFile;//交易注册文件
	unsigned long  g_processNum;//处理请求数
	unsigned long  g_processSucessNum;//处理成功请求数

	std::vector<S_BPUGROUP_INFO> g_vBpuGroupInfo; //记录bpu组的信息
	int  m_bIsRecTime;
	std::vector<int>  g_lBCFuncList;//广播配置 需要向总线订阅的广播
public:

	void Stop();

	// 函数名: PushData
	// 编程  : 王明松 2015-11-4 14:29:07
	// 返回  : int 
	// 参数  : unsigned int bpugroypindex
	// 参数  : S_BPC_RSMSG data
	// 参数  : int prio
	// 描述  : 根据BPU组index将数据放入队列
	int PushData(unsigned int bpugroypindex,S_BPC_RSMSG data,int prio);

	std::string GetNextFileName(unsigned int &serial);

	// 函数名: WriteBack
	// 编程  : 王明松 2010-3-16 14:58:20
	// 返回  : void 
	// 描述  : 写回配置文件
	void WriteBack();

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
	virtual bool Init(char *confile, CIErrlog *log);


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

	CIErrlog *m_log;//日志类

	void LogTime();
//	void PutTime(const char *filename,int fileline);
	void PutTime();

protected:
	
	std::vector<INT64_> m_vTime;

	unsigned long m_serial_connect;//连接序号
	CBF_Mutex m_mutex;//互斥变量
	std::string m_errMsg;//错误信息
	
	CBF_Date_Time m_datetime;

	bool g_bIsInitOnce;//是否初始化一次，即在一个线程里进行初始化后，将此标志置为1，其它线程判断此标志是否进行初始化
	bool g_bIsListen;//是否侦听，只能调用一次

	unsigned long m_nMaxSerial;//最大的流水
	unsigned long m_nSerialNum;//流水次数

	unsigned int m_nNextFileSerial;//后续包文件流水

	CBF_Mutex m_mutex_putprocess;//互斥变量
	CBF_Mutex m_mutex_putprocesssucess;//互斥变量

	//全局变量
	CBF_Mutex m_mutex_setgpara1;
	CBF_Mutex m_mutex_setgpara2;
	CBF_Mutex m_mutex_setgpara3;
	bool   m_bisSetGPara1;
	bool   m_bisSetGPara2;
	bool   m_bisSetGPara3;


	CBF_Mutex m_mutex_getnextfile;

	CBF_Mutex m_mutex_getSerial;//取唯一流水号

//	long   m_nSerial_get;//流水序号
	
	long   m_nSerial_getMax;//最大流水序号


	CMemTable <S_BPUGROUP_TBL> m_table;               //组名表
	CPkeyCharF<21,1>           m_pkey;                //组名
};

#endif // !defined(AFX_GRESOURCE_H__A7724768_0CB1_4D54_B228_60D1463D2AED__INCLUDED_)
