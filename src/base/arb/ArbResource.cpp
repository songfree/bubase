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
	g_lArbTxList.push_back(APP_QUERY_HOST);//����ѯ��
	g_lArbTxList.push_back(APP_PUSH_STATUS);//����״̬֪ͨ
	if (m_pXml.GetNodeValueByPath("/package/head/public/xdpfile",false,m_sXdpDefineXml,false) == NULL)
	{
		m_errMsg = "[/package/head/public/xdpfile]δ����";
		return false;
	}
	if (m_pXml.GetNodeValueByPath("/package/head/public/changetime",false,m_nSwitchTime) == NULL)
	{
		m_errMsg = "[/package/head/public/changetime]δ����";
		return false;
	}
	if (m_pXml.GetNodeValueByPath("/package/head/public/changesep",false,m_nSwitchSeg) == NULL)
	{
		m_errMsg = "[/package/head/public/changesep]δ����";
		m_nSwitchSeg = 5;
	}
	return true;
}
