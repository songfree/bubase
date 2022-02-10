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
#include "BF_SimpleLog.h"
#include "BF_Date_Time.h"
#include "IGResource.h"
#include "IPcLink.h"
#include "BF_Tools.h"



#ifdef _WINDOWS
#define INT64  __int64 
#else
#define INT64  long long 
#endif


class CBuRes  : public CIGResource 
{
public:
	
	CBuRes();
	virtual ~CBuRes();

	

public:
	bool g_bToExit;//是否退出程序
	int g_nPort;//侦听端口 默认为9001
	int g_nLoglevel;//日志级别 默认为5
	std::string g_sLogFileName;//日志文件名
	std::string g_sLogFilePath;//日志止录
	std::string g_sBccIp;//bcc的IP地址
	
	int g_IsZipEncrypt;//加密码压缩标志
	int g_nXmlNodeMode;//xml节点类型 0支持所有 1不支持<aa /> 

	bool g_bIsTxMainFunc;//交易统一入口
	std::string g_sMainFunc;//交易统一入口功能号

	int g_nDisconnectTime;//连接断开时间，即在此时间内没有收到请求


	std::string m_errMsg;
	CBF_SimpleLog m_log;
	CBF_Tools m_publog;


public:
	CIPcLink *m_pLink;
	void WriteBack();

	

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
	
};

#endif // !defined(AFX_GRESOURCE_H__A7724768_0CB1_4D54_B228_60D1463D2AED__INCLUDED_)
