// Dreb_Timer.h: interface for the CDreb_Timer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DREB_TIMER_H__B7578973_40CD_4B03_8ECC_A560B1B70440__INCLUDED_)
#define AFX_DREB_TIMER_H__B7578973_40CD_4B03_8ECC_A560B1B70440__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "BF_Timer.h"
#include "BF_HostInfo.h"
#include "GlobalVar.h"
#include "DrebPubDefine.h"


typedef vector<S_MONITOR_DISK > V_MonitorDiskInfo;

class CDreb_Timer  
{
public:
	bool IsStoped();

	// 函数名: GetHostInfo
	// 编程  : 王明松 2013-3-8 10:18:55
	// 返回  : void 
	// 参数  : S_MONITOR_HOST &hostinfo
	// 参数  : vector<S_MONITOR_DISK>  &diskinfo
	// 描述  : 应用取主机信息
	void GetHostInfo(S_MONITOR_HOST &hostinfo,V_MonitorDiskInfo  &diskinfo);

	// 函数名: Init
	// 编程  : 王明松 2013-3-8 10:19:07
	// 返回  : bool 
	// 参数  : CGlobalVar *pVar
	// 描述  : 初始化
	bool Init(CGlobalVar *pVar);
	CDreb_Timer();
	virtual ~CDreb_Timer();


	// 函数名: StopTimer
	// 编程  : 王明松 2013-3-8 10:19:16
	// 返回  : bool 
	// 描述  : 停止定时器
	bool StopTimer();

	// 函数名: StartTimer
	// 编程  : 王明松 2013-3-8 10:19:20
	// 返回  : bool 
	// 描述  : 启动定时器
	bool StartTimer();

	// 函数名: GetHostInfo
	// 编程  : 王明松 2013-3-8 10:19:24
	// 返回  : void 
	// 描述  : 定时器取主机信息
	void GetHostInfo();

	// 函数名: ComputeStat
	// 编程  : 王明松 2013-3-8 10:19:28
	// 返回  : void 
	// 描述  : 定时器取统计数据计算
	void ComputeStat();

	// 函数名: OnTimer
	// 编程  : 王明松 2013-3-6 16:43:23
	// 返回  : static int 
	// 参数  : unsigned int event
	// 参数  : void *p
	// 描述  : 定时器回调方法
	static int OnTimer(unsigned int event,void *p);

	S_DREBSTAT_INFO  m_sDrebInfo;
	S_DREBSTAT_INFO  m_sOldDrebInfo;
	S_DREBSTAT_INFO  m_sResultDrebInfo;

protected:
	CBF_Timer m_pTimer;//定时器
	CBF_HostInfo m_pHostInfo;
	CGlobalVar *m_pRes;


	S_MONITOR_HOST   m_sHostInfo;
	V_DiskInfo       m_vdinfo;
	

};

#endif // !defined(AFX_DREB_TIMER_H__B7578973_40CD_4B03_8ECC_A560B1B70440__INCLUDED_)
