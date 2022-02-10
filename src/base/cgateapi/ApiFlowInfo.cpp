// ApiFlowInfo.cpp: implementation of the CApiFlowInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "ApiFlowInfo.h"
#include "BF_Tools.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CApiFlowInfo::CApiFlowInfo()
{
	m_nPackByteSpeedRate = 0;
	m_nPackNumSpeedRate = 0;
	m_nOldSendPackBytes = 0;
	m_nOldSendPackNum = 0;
	m_nSendPackNum = 0;
	m_nSendPackBytes = 0;
	CBF_Tools::MakeDir("log");
}

CApiFlowInfo::~CApiFlowInfo()
{

}

void CApiFlowInfo::ReportStat(unsigned int sbytes)
{

	m_nSendPackBytes+=sbytes;
	m_nSendPackNum++;
}

void CApiFlowInfo::ReportTimer()
{
	m_nPackByteSpeedRate = m_nSendPackBytes - m_nOldSendPackBytes;
	m_nPackNumSpeedRate = m_nPackNumSpeedRate - m_nOldSendPackNum;

	m_nOldSendPackNum = m_nSendPackNum;             //发送交易数
	m_nOldSendPackBytes = m_nSendPackBytes;           //发送字节数
}

unsigned int CApiFlowInfo::GetSpeedRateBytes()
{
	return m_nPackByteSpeedRate;
}

unsigned int CApiFlowInfo::GetSpeedRateTx()
{
	return m_nOldSendPackBytes;
}
