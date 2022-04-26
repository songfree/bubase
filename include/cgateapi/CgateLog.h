// CgateLog.h: interface for the CCgateLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CGATELOG_H__003786D3_693C_4B1E_8FA3_44BE7C2F4DD0__INCLUDED_)
#define AFX_CGATELOG_H__003786D3_693C_4B1E_8FA3_44BE7C2F4DD0__INCLUDED_
#include "BF_Date_Time.h"
#include "GateHead.h"
#include "BF_CommDataLog.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifdef CGATEAPI_EXPORTS
#define CGATEAPI_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define CGATEAPI_EXPORT __declspec(dllimport)
#else
#define CGATEAPI_EXPORT 
#endif
#endif

class CGATEAPI_EXPORT CCgateLog
{
public:
	bool IsStoped();

	// 函数名: StartLog
	// 编程  : 王明松 2015-11-16 11:41:00
	// 返回  : bool 
	// 描述  : 启动日志线程
	bool StartLog();

	// 函数名: StopLog
	// 编程  : 王明松 2015-11-16 11:40:50
	// 返回  : void 
	// 描述  : 停止日志线程
	void StopLog();

	// 函数名: SetLoglevel
	// 编程  : 王明松 2015-11-16 11:40:35
	// 返回  : void 
	// 参数  : int loglevel
	// 描述  : 设置日志级别
	void SetLogLevel(int loglevel);

	CCgateLog();
	virtual ~CCgateLog();

	// 函数名: LogDreb
	// 编程  : 王明松 2013-5-6 16:00:38
	// 返回  : void 
	// 参数  : int level
	// 参数  : PCOMMSTRU data
	// 描述  : 写入dreb的日志
	void LogCgate(int level ,PCGATE_COMMSTRU data,int index,const char *key=NULL);

	// 函数名: SetLogPara
	// 编程  : 王明松 2013-5-6 16:00:11
	// 返回  : void 
	// 参数  : int logsize
	// 参数  : int level
	// 参数  : const char *logpath
	// 参数  : const char *iologname 正常日志名
	// 参数  : const char *errlogname 出错日志名
	// 描述  : 设置参数
	void SetLogPara(int logsize,int level ,const char *logpath,const char *cgatelogname);

protected:
	int m_nLogLevel;
	CBF_CommDataLog m_pLogGate;

};

#endif // !defined(AFX_CGATELOG_H__003786D3_693C_4B1E_8FA3_44BE7C2F4DD0__INCLUDED_)
