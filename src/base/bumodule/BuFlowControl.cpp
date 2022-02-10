// BuFlowControl.cpp: implementation of the CBuFlowControl class.
//
//////////////////////////////////////////////////////////////////////

#include "BuFlowControl.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBuFlowControl::CBuFlowControl()
{
	m_pConfXml = NULL;
	m_pCallBu = NULL;
	m_pSqlCmd = NULL;
	m_pLog = NULL;
	m_bIsSetPara =false;
}

CBuFlowControl::~CBuFlowControl()
{
	m_pConfXml = NULL;
	m_pCallBu = NULL;
	m_pSqlCmd = NULL;
	m_pLog = NULL;
}

void CBuFlowControl::SetBuPara(CBF_Xml *conxml,CIPcLink *callbu,CDbApi *sqldb,CBF_SimpleLog *bulog)
{
	m_pConfXml = conxml;
	m_pCallBu = callbu;
	m_pSqlCmd = sqldb;
	m_pLog = bulog;
	m_bIsSetPara = true;
}

bool CBuFlowControl::InitFlow()
{
	//读取应用配置

	//实例化各应用

	//读取交易配置



	return true;
}

bool CBuFlowControl::IsRunFlow(int nTxcode)
{
	return true;
}

int  CBuFlowControl::RunFlow(PBPCCOMMSTRU data)
{
	return 0;
}

int  CBuFlowControl::GetFlowFunc(vector<S_TBL_FLOWTX> &txlist)
{
	return 0;
}