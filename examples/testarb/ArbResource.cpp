// ArbResource.cpp: implementation of the CArbResource class.
//
//////////////////////////////////////////////////////////////////////

#include "ArbResource.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CArbResource::CArbResource()
{
	g_bToExit = false;
	m_nSvrHostStatus = ARBSTATUS_UNKNOW;
	m_nArbReportTime = 1; //定时上报为1秒
}

CArbResource::~CArbResource()
{

}

bool CArbResource::Init(const char *confile)
{
	bool bRet = CBF_DrebResource::Init(confile);
	if (!bRet)
	{
		return false;
	}
	g_lSvrTxList.push_back(ARB_SEND_CONTROL);//仲裁指定主机
	g_lSvrTxList.push_back(ARB_SEND_QUERY);//仲裁查询
	if (m_pXml.GetNodeValueByPath("/package/head/公共配置/XDP定义文件",false,m_sXdpDefineXml,false) == NULL)
	{
		m_errMsg = "[/package/head/公共配置/XDP定义文件]未配置";
		return false;
	}
	if (m_pXml.GetNodeValueByPath("/package/head/公共配置/上报仲裁时间",false,m_nArbReportTime) == NULL)
	{
		m_errMsg = "[/package/head/公共配置/上报仲裁时间]未配置";
		return false;
	}
	return true;
}
