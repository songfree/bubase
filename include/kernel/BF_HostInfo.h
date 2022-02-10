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

//��ȡ��������Դ��Ϣ�࣬��cpu��Ӳ�̡��ڴ�

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

	// ������: 
	// ���  : ������ 2014-11-14 10:04:18
	// ����  : __int64 CompareFileTime 
	// ����  :  FILETIME time1
	// ����  : FILETIME time2
	// ����  : GetCpuSpeedʹ��ȡ�ļ�ʱ���ķ���
	__int64 CompareFileTime ( FILETIME time1, FILETIME time2 )  ;

	// ������: GetCpuSpeed
	// ���  : ������ 2014-11-14 10:03:07
	// ����  : long 
	// ����  : double &tCpuUsed
	// ����  : ���windows2008��win7��ȡCPUʹ�÷���
	long GetCpuSpeed(double &tCpuUsed)  ;
#endif
	
public:
	// ������: SetLogPara
	// ���  : ������ 2009-11-14 9:55:23
	// ����  : void 
	// ����  : int level
	// ����  : const char *logfilepath
	// ����  : const char *logfilename
	// ����  : int id=-1
	// ����  : ����������Դ��־����
	void SetLogPara(int level,const char *logfilepath,const char *logfilename,int id=-1);

	// ������: GetCpu
	// ���  : ������ 2009-11-14 10:00:05
	// ����  : int   <0����   0-100��ֵ cpu��%ֵ
	// ����  : ����CPUʹ����
	int GetCpu();

	// ������: GetEms
	// ���  : ������ 2009-11-14 10:01:48
	// ����  : int 
	// ����  : S_MemoryInf &smInfo
	// ����  : ȡ���ڴ�ʹ�����
	int GetEms(S_MemoryInf &smInfo);

	// ������: GetDisk
	// ���  : ������ 2009-11-14 10:02:03
	// ����  : int 
	// ����  : V_DiskInfo &dinfo
	// ����  : ȡ�ô���ʹ�����
	int GetDisk(V_DiskInfo &dinfo);
};

#endif // !defined(AFX_HOSTINFO_H__9B8DCC1A_16D6_4258_A084_60407F06CD6D__INCLUDED_)
