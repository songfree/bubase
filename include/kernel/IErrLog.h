// IErrLog.h: interface for the CIErrlog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IERRLOG_H__5435B2B8_48F0_43B6_B609_5663A3879846__INCLUDED_)
#define AFX_IERRLOG_H__5435B2B8_48F0_43B6_B609_5663A3879846__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define LOG_DEBUG 5   ///调试信息
#define LOG_PROMPT 4  ///提示警告
#define LOG_WARNNING 3  ///警告
#define LOG_ERROR_GENERAL 2  ///一般错误
#define LOG_ERROR 1        ///重要错误
#define LOG_ERROR_FAULT 0  ///严重错
#define LOG_INFO    -1     ///启动停止等必须写入日志的信息

#include <stdio.h>
#include <string>


#ifdef KERNEL_EXPORTS
#define KERNEL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNEL_EXPORT __declspec(dllimport)
#else
#define KERNEL_EXPORT 
#endif
#endif

class KERNEL_EXPORT CIErrlog  
{
public:
	CIErrlog()
	{

	}
	virtual ~CIErrlog()
	{

	}
	// 函数名: isWrite
	// 编程  : 王明松 2009-6-29 14:28:34
	// 返回  : int 
	// 参数  : int level 日志级别 
	// 描述  : 是否要写入日志
	virtual bool isWrite(int level)=0;

	// 函数名: SetLogPara
	// 编程  : 王明松 2009-6-29 14:28:34
	// 返回  : int 
	// 参数  : int level 日志级别 
	// 参数  : const char *logfilepath  日志文件路径
	// 参数  : const char *logfilename  日志文件名
	// 参数  : int id=-1  进程或线程ID
	// 描述  : 设置日志参数
	virtual void SetLogPara(int level,const char *logfilepath,const char *logfilename,int id=-1)=0;


	// 函数名: GetLogPara
	// 编程  : 王明松 2012-4-25 11:21:05
	// 返回  : virtual void 
	// 参数  : int &level 日志级别
	// 参数  : std::string &logfilepath 日志文件路径
	// 参数  : std::string &logfilename 日志文件名
	// 参数  : int &id 进程或线程ID
	// 描述  : 取日志参数
	virtual void GetLogPara(int &level,std::string &logfilepath, std::string &logfilename,int &id)=0;

	/// 函数名: LogMp
	/// 编程  : 王明松 2009-5-16 17:28:24
	/// 返回  : int 
	/// 参数  : int level
	/// 参数  : const char *filename
	/// 参数  : int line
	/// 参数  : const char *fmt
	/// 参数  : ...
	/// 描述  : 写入日志
	virtual int LogMp(int level, const char *filename, int line,const char *fmt,...)=0;

	/// 函数名: LogBin
	/// 编程  : 王明松 2007-7-10 11:27:52
	/// 返回  : int 
	/// 参数  : int level
	/// 参数  : const char *filename
	/// 参数  : int line
	/// 参数  : const char *title
	/// 参数  : char *msg
	/// 参数  : int msglen
	/// 描述  : 写入二进制数据至日志文件 注意msglen的大小
	virtual int LogBin(int level, const char *filename, int line,const char *title,char *msg,int msglen)=0;


	// 函数名: SetMaxLogSize
	// 编程  : 王明松 2014-11-11 15:19:01
	// 返回  : virtual void 
	// 参数  : int size  字节数
	// 描述  : 设置日志文件的最大大小，0为不限制，达到大小后切换文件
	virtual void SetMaxLogSize(int size)=0;

};

#endif // !defined(AFX_IERRLOG_H__5435B2B8_48F0_43B6_B609_5663A3879846__INCLUDED_)
