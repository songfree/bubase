// Bpc_Timer.h: interface for the CBpc_Timer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DREB_TIMER_H__B7578973_40CD_4B03_8ECC_A560B1B70441__INCLUDED_)
#define AFX_DREB_TIMER_H__B7578973_40CD_4B03_8ECC_A560B1B70441__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "BF_Timer.h"
#include "HostInfo.h"
#include "GResource.h"
#include "DrebPubDefine.h"
#include "NextTbl.h"

typedef vector<S_MONITOR_DISK > V_MonitorDiskInfo;
typedef struct 
{
	UINT64_  g_nRcvBytes;//�����ֽ���
	UINT64_  g_nRcvPackNum;//�������ݰ���
	UINT64_  g_nSendBytes;//�����ֽ���
	UINT64_  g_nSendPackNum;//�������ݰ���

}S_BPCSTAT_INFO;



class CBpc_Timer  
{
public:
	void DeleteNextFile();
	void ReportSend(unsigned int sendbytes,unsigned int sendpack=1);
	void ReportRcv(unsigned int rcvbyte,unsigned int packnum=1);
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
	bool Init(CGResource *pVar);
	CBpc_Timer();
	virtual ~CBpc_Timer();


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

 	S_BPCSTAT_INFO  m_sBpcInfo;
 	S_BPCSTAT_INFO  m_sOldBpcInfo;
 	S_BPCSTAT_INFO  m_sResultBpcInfo;

	CNextTbl  m_pNextTbl;

	std::string      m_sNxtFilePath;

protected:
	CBF_Timer m_pTimer;//��ʱ��
	CHostInfo m_pHostInfo;
	CGResource *m_pRes;
	

	S_MONITOR_HOST   m_sHostInfo;
	V_DiskInfo       m_vdinfo;
	
	CBF_Mutex        m_mutex;

	


};

#endif // !defined(AFX_DREB_TIMER_H__B7578973_40CD_4B03_8ECC_A560B1B70441__INCLUDED_)
