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
#include "IErrLog.h"

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
class CHostInfo  
{
public:
	CHostInfo();
	virtual ~CHostInfo();
	CIErrlog  *m_log;
private:
	
	CBF_Date_Time m_pDate;
	
public:
	int GetCpu();
	int GetEms(S_MemoryInf &smInfo);
	int GetDisk(V_DiskInfo &dinfo);
};

#endif // !defined(AFX_HOSTINFO_H__9B8DCC1A_16D6_4258_A084_60407F06CD6D__INCLUDED_)
