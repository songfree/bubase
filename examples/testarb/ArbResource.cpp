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
	m_nArbReportTime = 1; //��ʱ�ϱ�Ϊ1��
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
	g_lSvrTxList.push_back(ARB_SEND_CONTROL);//�ٲ�ָ������
	g_lSvrTxList.push_back(ARB_SEND_QUERY);//�ٲò�ѯ
	if (m_pXml.GetNodeValueByPath("/package/head/��������/XDP�����ļ�",false,m_sXdpDefineXml,false) == NULL)
	{
		m_errMsg = "[/package/head/��������/XDP�����ļ�]δ����";
		return false;
	}
	if (m_pXml.GetNodeValueByPath("/package/head/��������/�ϱ��ٲ�ʱ��",false,m_nArbReportTime) == NULL)
	{
		m_errMsg = "[/package/head/��������/�ϱ��ٲ�ʱ��]δ����";
		return false;
	}
	return true;
}
