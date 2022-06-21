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
	m_sBpcInfo.g_nRcvBytes=0;
	m_sBpcInfo.g_nRcvPackNum=0;
	m_sBpcInfo.g_nSendBytes=0;
	m_sBpcInfo.g_nSendPackNum=0;
}

CBpc_Timer::~CBpc_Timer()
{
	m_pRes = NULL;
}

void CBpc_Timer::ComputeStat()
{
  	//计算
	m_sResultBpcInfo.g_nRcvBytes = m_sBpcInfo.g_nRcvBytes  - m_sOldBpcInfo.g_nRcvBytes;
	m_sResultBpcInfo.g_nRcvPackNum =m_sBpcInfo.g_nRcvPackNum  - m_sOldBpcInfo.g_nRcvPackNum;
	m_sResultBpcInfo.g_nSendBytes =m_sBpcInfo.g_nSendBytes  - m_sOldBpcInfo.g_nSendBytes;
	m_sResultBpcInfo.g_nSendPackNum =m_sBpcInfo.g_nSendPackNum  - m_sOldBpcInfo.g_nSendPackNum;

	//保存到旧的结构
 	memcpy(&m_sOldBpcInfo,&(m_sBpcInfo),sizeof(S_BPCSTAT_INFO));

}

bool CBpc_Timer::StartTimer()
{
	return m_pTimer.Start();
}

bool CBpc_Timer::StopTimer()
{
	m_pTimer.Stop();
	return true;
}

int CBpc_Timer::OnTimer(unsigned int event, void *p)
{
	CBpc_Timer *pp = (CBpc_Timer *)p;
	if (event == 0)
	{
		//计算每秒接收发放
		pp->ComputeStat();
	}
	else if (event == 2)
	{
		pp->DeleteNextFile();
	}
	return 0;
}

bool CBpc_Timer::Init(CGResource *pVar)
{
	m_pRes = pVar;
	m_pNextTbl.m_pRes = m_pRes;

	m_pTimer.Init(100,true);
	//计算每秒数据量的定时器
	m_pTimer.SetTimer(0,1000,&CBpc_Timer::OnTimer,this); //设置定时器 
	//删除过期后续包定时器,每50分钟检查一次
	m_pTimer.SetTimer(2,3000000,&CBpc_Timer::OnTimer,this);
	return true;
}

bool CBpc_Timer::IsStoped()
{
	return m_pTimer.IsStoped();
}

void CBpc_Timer::ReportRcv(unsigned int rcvbyte, unsigned int packnum)
{
	m_sBpcInfo.g_nRcvBytes+=rcvbyte;
	m_sBpcInfo.g_nRcvPackNum+=packnum;
}

void CBpc_Timer::ReportSend(unsigned int sendbytes, unsigned int sendpack)
{
	m_sBpcInfo.g_nSendBytes+=sendbytes;
	m_sBpcInfo.g_nSendPackNum+=sendpack;
}

void CBpc_Timer::DeleteNextFile()
{
	CBF_Tools::RemoveOverdueFile(m_sNxtFilePath.c_str(),"nxt",1,true);
}
