// BpcLog.h: interface for the CBpcLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BPCLOG_H__46E1F84A_FC45_417C_BDF5_2AAB80089AC3__INCLUDED_)
#define AFX_BPCLOG_H__46E1F84A_FC45_417C_BDF5_2AAB80089AC3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BF_Date_Time.h"
#include "BpcHead.h"



#ifdef _WINDOWS
#ifdef _DEBUG
#pragma comment(lib, "bf_kerneld.lib") 
#pragma message("Automatically linking with   bf_kerneld.lib ")
#else
#pragma comment(lib, "bf_kernel.lib") 
#pragma message("Automatically linking with   bf_kernel.lib ")
#endif

#endif

#ifdef DBPUB_EXPORTS
#define DBPUB_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define DBPUB_EXPORT __declspec(dllimport)
#else
#define DBPUB_EXPORT 
#endif
#endif

//日志文件格式
//文件的前2字节表示字节序和日志类型
//第一字节   取值有B和L   B表示BIG_ENDIAN字节序  网络序或aix hp_ux所使用的字节序  L表示LITTLE_ENDIAN字节序 linux及windows使用的字节序
//第二字节   取值有0和1   0表示dreb数据日志   1表示bpc数据日志
//随后的数据见如下结构
// 
// typedef struct   bpc数据日志
// {
// 	unsigned int nTime;//日期时间  hhmmss
// 	unsigned int nUs;  //微秒
// 	unsigned int nId;  //进程ID
// 	BPCCOMMSTRU  data;
// }S_LOG_BPCDATA;
// 
// typedef struct   dreb数据日志
// {
// 	unsigned int nTime;//日期时间  hhmmss
// 	unsigned int nUs;  //微秒
// 	unsigned int nId;  //进程ID
// 	COMMSTRU  data;
// }S_LOG_DREBDATA;



class DBPUB_EXPORT CDrebBpcLog  
{
public:

	// 函数名: LogDreb
	// 编程  : 王明松 2013-5-6 16:00:38
	// 返回  : void 
	// 参数  : int level
	// 参数  : PBPCCOMMSTRU data
	// 参数  : bool flag false正常日志  true出错日志
	// 描述  : 写入dreb的日志
	void LogDreb(int level ,PBPCCOMMSTRU data,bool flag=false);

	// 函数名: LogDreb
	// 编程  : 王明松 2013-5-6 16:00:38
	// 返回  : void 
	// 参数  : int level
	// 参数  : PCOMMSTRU data
	// 参数  : bool flag false正常日志  true出错日志
	// 描述  : 写入dreb的日志
	void LogDreb(int level ,PCOMMSTRU data,bool flag=false);

	// 函数名: SetLogPara
	// 编程  : 王明松 2013-5-6 16:00:11
	// 返回  : void 
	// 参数  : int level
	// 参数  : const char *logpath
	// 参数  : const char *iologname 正常日志名
	// 参数  : const char *errlogname 出错日志名
	// 描述  : 设置参数
	void SetLogPara(int level ,const char *logpath,const char *dreblogname,const char *dreberrlogname,const char *bpclogname,const char *bpcerrlogname);


	// 函数名: LogBpc
	// 编程  : 王明松 2013-5-6 16:00:20
	// 返回  : void 
	// 参数  : int level
	// 参数  : PBPCCOMMSTRU data
	// 参数  : bool flag  false正常日志  true出错日志
	// 描述  : 写入BPC的日志
	void LogBpc(int level,PBPCCOMMSTRU data,bool flag=false);

	CDrebBpcLog();
	virtual ~CDrebBpcLog();

protected:
	std::string m_sLogPath;
	std::string m_sDrebLogName;
	std::string m_sDrebLogErrName;
	std::string m_sBpcLogName;
	std::string m_sBpcLogErrName;
	int         m_nLogLevel;
	CBF_Date_Time  m_pDate;
	int         m_nBpcHeadLen;

};

#endif // !defined(AFX_BPCLOG_H__46E1F84A_FC45_417C_BDF5_2AAB80089AC3__INCLUDED_)
