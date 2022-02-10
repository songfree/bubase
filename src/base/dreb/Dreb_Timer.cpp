// Dreb_Timer.cpp: implementation of the CDreb_Timer class.
//
//////////////////////////////////////////////////////////////////////

#include "Dreb_Timer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDreb_Timer::CDreb_Timer()
{
	m_pRes = NULL;
}

CDreb_Timer::~CDreb_Timer()
{
	m_pRes = NULL;
}

void CDreb_Timer::ComputeStat()
{
	//保存到旧的结构
	memcpy(&m_sOldDrebInfo,&(m_sDrebInfo),sizeof(S_DREBSTAT_INFO));
	//取最新的结构
	memcpy(&m_sDrebInfo,&(m_pRes->g_pDrebStatInfo.m_drebinfo),sizeof(S_DREBSTAT_INFO));
	//计算
	m_sResultDrebInfo.m_nAnsByteClient = m_sDrebInfo.m_nAnsByteClient  - m_sOldDrebInfo.m_nAnsByteClient;
	m_sResultDrebInfo.m_nAnsPackClient =m_sDrebInfo.m_nAnsPackClient  - m_sOldDrebInfo.m_nAnsPackClient;
	m_sResultDrebInfo.m_nCallSrvByte =m_sDrebInfo.m_nCallSrvByte  - m_sOldDrebInfo.m_nCallSrvByte;
	m_sResultDrebInfo.m_nCallSrvNum =m_sDrebInfo.m_nCallSrvNum  - m_sOldDrebInfo.m_nCallSrvNum;
	m_sResultDrebInfo.m_nRcvByteClient =m_sDrebInfo.m_nRcvByteClient  - m_sOldDrebInfo.m_nRcvByteClient;
	m_sResultDrebInfo.m_nRcvPackClient =m_sDrebInfo.m_nRcvPackClient  - m_sOldDrebInfo.m_nRcvPackClient;
	m_sResultDrebInfo.m_nSrvAnsByte =m_sDrebInfo.m_nSrvAnsByte  - m_sOldDrebInfo.m_nSrvAnsByte;
	m_sResultDrebInfo.m_nSrvAnsNum =m_sDrebInfo.m_nSrvAnsNum  - m_sOldDrebInfo.m_nSrvAnsNum;
	m_sResultDrebInfo.m_nTransInByte =m_sDrebInfo.m_nTransInByte  - m_sOldDrebInfo.m_nTransInByte;
	m_sResultDrebInfo.m_nTransInNum =m_sDrebInfo.m_nTransInNum  - m_sOldDrebInfo.m_nTransInNum;
	m_sResultDrebInfo.m_nTransOutByte =m_sDrebInfo.m_nTransOutByte  - m_sOldDrebInfo.m_nTransOutByte;
	m_sResultDrebInfo.m_nTransOutNum =m_sDrebInfo.m_nTransOutNum  - m_sOldDrebInfo.m_nTransOutNum;


}

void CDreb_Timer::GetHostInfo()
{
	m_sHostInfo.nCpuRate = m_pHostInfo.GetCpu();
	S_MemoryInf smInfo;
	m_pHostInfo.GetEms(smInfo);
	m_sHostInfo.nTotalMemory = smInfo.EmsTotal;
	m_sHostInfo.nUsedMemory = smInfo.EmsUse;
	m_vdinfo.clear();
	m_pHostInfo.GetDisk(m_vdinfo);
	m_sHostInfo.nDiskNum = m_vdinfo.size();
// 	for (int i=0 ; i<m_sHostInfo.nDiskNum ; i++)
// 	{
// 		printf("name:%s Total:%d used:%d\n",m_vdinfo[i].cIndex,m_vdinfo[i].TotalSpace,m_vdinfo[i].UsedSpace);
// 	}
	
}

bool CDreb_Timer::StartTimer()
{
	return m_pTimer.Start();
}

bool CDreb_Timer::StopTimer()
{
	m_pTimer.Stop();
	return true;
}

int CDreb_Timer::OnTimer(unsigned int event, void *p)
{
	CDreb_Timer *pp = (CDreb_Timer *)p;
	if (event == 0)
	{
		//计算每秒接收发放
		pp->ComputeStat();
	}
	else if (event == 1 )
	{
		pp->GetHostInfo();
	}
	return 0;
}

bool CDreb_Timer::Init(CGlobalVar *pVar)
{
	m_pRes = pVar;
//	m_pHostInfo.m_log = &(m_pRes->g_pLog);
	m_pTimer.Init(100,true);
	//计算每秒数据量的定时器
	m_pTimer.SetTimer(0,1000,&CDreb_Timer::OnTimer,this); //设置定时器 
	//取主机资源信息的定时器
	m_pTimer.SetTimer(1,5000,&CDreb_Timer::OnTimer,this);
	return true;
}

void CDreb_Timer::GetHostInfo(S_MONITOR_HOST &hostinfo, V_MonitorDiskInfo  &diskinfo)
{
	
	hostinfo.nCpuRate = m_sHostInfo.nCpuRate;
	hostinfo.nDiskNum = m_sHostInfo.nDiskNum;
	hostinfo.nTotalMemory = m_sHostInfo.nTotalMemory;
	hostinfo.nUsedMemory = m_sHostInfo.nUsedMemory;

	S_MONITOR_DISK pp;
	for (int i=0;i<hostinfo.nDiskNum;i++)
	{
		pp.nTotalSpace = m_vdinfo[i].TotalSpace;
		pp.nUsedSpace = m_vdinfo[i].UsedSpace;
		CBF_Tools::StringCopy(pp.sDiskName,199,m_vdinfo[i].cIndex);
//		printf("name:%s Total:%d used:%d\n",pp.sDiskName,pp.nTotalSpace,pp.nUsedSpace);
		diskinfo.push_back(pp);
	}
}

bool CDreb_Timer::IsStoped()
{
	return m_pTimer.IsStoped();
}
