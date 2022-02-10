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

	// ������: GetHostInfo
	// ���  : ������ 2013-3-8 10:18:55
	// ����  : void 
	// ����  : S_MONITOR_HOST &hostinfo
	// ����  : vector<S_MONITOR_DISK>  &diskinfo
	// ����  : Ӧ��ȡ������Ϣ
	void GetHostInfo(S_MONITOR_HOST &hostinfo,V_MonitorDiskInfo  &diskinfo);

	// ������: Init
	// ���  : ������ 2013-3-8 10:19:07
	// ����  : bool 
	// ����  : CGlobalVar *pVar
	// ����  : ��ʼ��
	bool Init(CGlobalVar *pVar);
	CDreb_Timer();
	virtual ~CDreb_Timer();


	// ������: StopTimer
	// ���  : ������ 2013-3-8 10:19:16
	// ����  : bool 
	// ����  : ֹͣ��ʱ��
	bool StopTimer();

	// ������: StartTimer
	// ���  : ������ 2013-3-8 10:19:20
	// ����  : bool 
	// ����  : ������ʱ��
	bool StartTimer();

	// ������: GetHostInfo
	// ���  : ������ 2013-3-8 10:19:24
	// ����  : void 
	// ����  : ��ʱ��ȡ������Ϣ
	void GetHostInfo();

	// ������: ComputeStat
	// ���  : ������ 2013-3-8 10:19:28
	// ����  : void 
	// ����  : ��ʱ��ȡͳ�����ݼ���
	void ComputeStat();

	// ������: OnTimer
	// ���  : ������ 2013-3-6 16:43:23
	// ����  : static int 
	// ����  : unsigned int event
	// ����  : void *p
	// ����  : ��ʱ���ص�����
	static int OnTimer(unsigned int event,void *p);

	S_DREBSTAT_INFO  m_sDrebInfo;
	S_DREBSTAT_INFO  m_sOldDrebInfo;
	S_DREBSTAT_INFO  m_sResultDrebInfo;

protected:
	CBF_Timer m_pTimer;//��ʱ��
	CBF_HostInfo m_pHostInfo;
	CGlobalVar *m_pRes;


	S_MONITOR_HOST   m_sHostInfo;
	V_DiskInfo       m_vdinfo;
	

};

#endif // !defined(AFX_DREB_TIMER_H__B7578973_40CD_4B03_8ECC_A560B1B70440__INCLUDED_)
