// HostInfo.h: interface for the CHostInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HOSTINFO_H__9B8DCC1A_16D6_4258_A084_60407F06CD6D__INCLUDED_)
#define AFX_HOSTINFO_H__9B8DCC1A_16D6_4258_A084_60407F06CD6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "public.h"
#include "BF_Date_Time.h"
#include "BF_SimpleLog.h"

//获取主机的资源信息类，如cpu、硬盘、内存

typedef struct _MemoryInf
{
	int EmsTotal;
	int EmsUse;
	int EmsUseRadio;
}S_MemoryInf;
typedef struct _DiskInf
{
	char cIndex[256];
	int TotalSpace;
	int UsedSpace;
	double DiskUseRadio;
}S_DiskInf;

typedef vector<S_DiskInf > V_DiskInfo;

#ifdef KERNEL_EXPORTS
#define KERNEL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNEL_EXPORT __declspec(dllimport)
#else
#define KERNEL_EXPORT 
#endif
#endif


class KERNEL_EXPORT CBF_HostInfo  
{
public:
	CBF_HostInfo();
	virtual ~CBF_HostInfo();
	CBF_SimpleLog m_log;
private:
	
	CBF_Date_Time m_pDate;

#ifdef _WINDOWS

	// 函数名: 
	// 编程  : 王明松 2014-11-14 10:04:18
	// 返回  : __int64 CompareFileTime 
	// 参数  :  FILETIME time1
	// 参数  : FILETIME time2
	// 描述  : GetCpuSpeed使用取文件时间差的方法
	__int64 CompareFileTime ( FILETIME time1, FILETIME time2 )  ;

	// 函数名: GetCpuSpeed
	// 编程  : 王明松 2014-11-14 10:03:07
	// 返回  : long 
	// 参数  : double &tCpuUsed
	// 描述  : 针对windows2008和win7的取CPU使用方法
	long GetCpuSpeed(double &tCpuUsed)  ;
#endif
	
public:
	// 函数名: SetLogPara
	// 编程  : 王明松 2009-11-14 9:55:23
	// 返回  : void 
	// 参数  : int level
	// 参数  : const char *logfilepath
	// 参数  : const char *logfilename
	// 参数  : int id=-1
	// 描述  : 设置主机资源日志参数
	void SetLogPara(int level,const char *logfilepath,const char *logfilename,int id=-1);

	// 函数名: GetCpu
	// 编程  : 王明松 2009-11-14 10:00:05
	// 返回  : int   <0出错   0-100的值 cpu的%值
	// 描述  : 返回CPU使用率
	int GetCpu();

	// 函数名: GetEms
	// 编程  : 王明松 2009-11-14 10:01:48
	// 返回  : int 
	// 参数  : S_MemoryInf &smInfo
	// 描述  : 取得内存使用情况
	int GetEms(S_MemoryInf &smInfo);

	// 函数名: GetDisk
	// 编程  : 王明松 2009-11-14 10:02:03
	// 返回  : int 
	// 参数  : V_DiskInfo &dinfo
	// 描述  : 取得磁盘使用情况
	int GetDisk(V_DiskInfo &dinfo);
};

#endif // !defined(AFX_HOSTINFO_H__9B8DCC1A_16D6_4258_A084_60407F06CD6D__INCLUDED_)
