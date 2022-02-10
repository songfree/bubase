// BF_SimpleLog.h: interface for the CBF_SimpleLog class.
//
//////////////////////////////////////////////////////////////////////

/***********************************************************************************
  编码日志记录
  编码者: 王明松
  编码日期 2012-4-26 9:12:53 重构
  版本: V1.00 
  说明: 这是一个简单日志类 ，通过操作系统的文件机制来并发写入
 ***********************************************************************************/


#if !defined(AFX_BF_SIMPLELOG_H__BC40DB9B_FA26_4689_8DD7_DFD94B06E2D8__INCLUDED_)
#define AFX_BF_SIMPLELOG_H__BC40DB9B_FA26_4689_8DD7_DFD94B06E2D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IErrLog.h"
#include "BF_Date_Time.h"


#ifdef KERNEL_EXPORTS
#define KERNEL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNEL_EXPORT __declspec(dllimport)
#else
#define KERNEL_EXPORT 
#endif
#endif

class KERNEL_EXPORT CBF_SimpleLog : public CIErrlog  
{
public:
	CBF_SimpleLog();
	virtual ~CBF_SimpleLog();

	// 函数名: isWrite
	// 编程  : 王明松 2009-6-29 14:28:34
	// 返回  : int 
	// 参数  : int level 日志级别 
	// 描述  : 是否要写入日志
	virtual bool isWrite(int level);
	
	// 函数名: SetLogPara
	// 编程  : 王明松 2009-6-29 14:28:34
	// 返回  : int 
	// 参数  : int level 日志级别 
	// 参数  : const char *logfilepath  日志文件路径
	// 参数  : const char *logfilename  日志文件名
	// 参数  : int id=-1
	// 描述  : 设置日志参数
	virtual void SetLogPara(int level,const char *logfilepath,const char *logfilename,int id=-1);

	/// 函数名: GetLogPara
	/// 编程  : 王明松 2012-4-25 11:21:05
	/// 返回  : virtual void 
	/// 参数  : int &level 日志级别
	/// 参数  : std::string &logfilepath 日志文件路径
	/// 参数  : std::string &logfilename 日志文件名
	/// 参数  : int &id 进程或线程ID
	/// 描述  : 取日志参数
	virtual void GetLogPara(int &level,std::string &logfilepath, std::string &logfilename,int &id);
	
	/// 函数名: LogMp
	/// 编程  : 王明松 2009-5-16 17:28:24
	/// 返回  : int 
	/// 参数  : int level
	/// 参数  : const char *filename
	/// 参数  : int line
	/// 参数  : const char *fmt
	/// 参数  : ...
	/// 描述  : 写入日志
	virtual int LogMp(int level, const char *filename, int line,const char *fmt,...);
	
	/// 函数名: LogBin
	/// 编程  : 王明松 2007-7-10 11:27:52
	/// 返回  : int -1失败 0成功
	/// 参数  : int level  日志级别
	/// 参数  : const char *filename  源文件名
	/// 参数  : int line              源文件行
	/// 参数  : const char *title  标题
	/// 参数  : char *msg   内容
	/// 参数  : int msglen  长度
	/// 描述  : 写入二进制数据至日志文件,'\0'用*代替
	virtual int LogBin(int level, const char *filename, int line,const char *title,char *msg,int msglen);


	// 函数名: SetMaxLogSize
	// 编程  : 王明松 2014-11-12 14:02:18
	// 返回  : virtual void 
	// 参数  : int size  字节数   小于1024则不限制
	// 描述  : 设置日志文件大小
	virtual void SetMaxLogSize(int size);

	void  SetLogLevel(int level);
	int  GetLogLevel();

protected:
	/// 函数名: GetName
	/// 编程  : 王明松 2007-7-10 10:52:10
	/// 返回  : void 
	/// 参数  : char *filename  
	/// 描述  : 获取文件名称，不带路径的文件名
	void GetName(char *filename);

protected:
	int            m_nLogLevel; //日志级别
	int            m_nId;
	std::string    m_sLogPath;   
	std::string    m_sLogName;
	CBF_Date_Time  m_pDate;
	std::string    m_sCurDate;//当前日期
};

#endif // !defined(AFX_BF_SIMPLELOG_H__BC40DB9B_FA26_4689_8DD7_DFD94B06E2D8__INCLUDED_)
