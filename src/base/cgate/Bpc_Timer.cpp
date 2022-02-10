// Bpc_Timer.cpp: implementation of the CBpc_Timer class.
//
//////////////////////////////////////////////////////////////////////

#include "Bpc_Timer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBpc_Timer::CBpc_Timer()
{
	m_pRes = NULL;
}

CBpc_Timer::~CBpc_Timer()
{
	m_pRes = NULL;
}



void CBpc_Timer::GetHostInfo()
{
	CBF_PMutex pp(&m_mutex);
	m_sHostInfo.nCpuRate = m_pHostInfo.GetCpu();
	S_MemoryInf smInfo;
	m_pHostInfo.GetEms(smInfo);
	m_sHostInfo.nTotalMemory = smInfo.EmsTotal;
	m_sHostInfo.nUsedMemory = smInfo.EmsUse;
	m_vdinfo.clear();
	m_pHostInfo.GetDisk(m_vdinfo);
	m_sHostInfo.nDiskNum = m_vdinfo.size();
	
}

bool CBpc_Timer::StartTimer()
{
	return m_pTimer.Start();
}

bool CBpc_Timer::StopTimer()
{
	m_pTimer.Stop();
	m_pTimer.Join();
	return true;
}

int CBpc_Timer::OnTimer(unsigned int event, void *p)
{
	CBpc_Timer *pp = (CBpc_Timer *)p;
	if (event == 0)
	{
	}
	else if (event == 1 )
	{
		pp->GetHostInfo();
	}
	return 0;
}

bool CBpc_Timer::Init(CGateResource *pVar)
{
	m_pRes = pVar;
	
	m_pHostInfo.m_log  = &(m_pRes->g_pLog);
	m_pTimer.Init(100,true);
	//取主机资源信息的定时器
	m_pTimer.SetTimer(1,5000,&CBpc_Timer::OnTimer,this);

	return true;
}

void CBpc_Timer::GetHostInfo(S_MONITOR_HOST &hostinfo, V_MonitorDiskInfo  &diskinfo)
{
	CBF_PMutex ppp(&m_mutex);
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

bool CBpc_Timer::IsStoped()
{
	return m_pTimer.IsStoped();
}

