// DrebStatInfo.cpp: implementation of the CDrebStatInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "DrebStatInfo.h"
#include "DrebPubDefine.h"
#include "math.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDrebStatInfo::CDrebStatInfo()
{
	m_drebinfo.m_nRcvPackClient=0;//接收客户端包数
	m_drebinfo.m_nRcvByteClient=0;//接收客户端字节数
	m_drebinfo.m_nAnsPackClient=0;//应答客户端包数
	m_drebinfo.m_nAnsByteClient=0;//应答客户端字节数
	
	m_drebinfo.m_nTransOutNum=0; //数据转发包数
	m_drebinfo.m_nTransOutByte=0;//数据转发字节数
	m_drebinfo.m_nTransInNum=0;  //数据转发包数
	m_drebinfo.m_nTransInByte=0; //数据转发字节数
	
	m_drebinfo.m_nCallSrvNum=0;   //调用服务次数
    m_drebinfo.m_nCallSrvByte=0;  //调用服务字节数
	m_drebinfo.m_nSrvAnsNum=0;    //服务应答次数
    m_drebinfo.m_nSrvAnsByte=0;   //服务应答字节数
	
	m_nStartTime=0;           //启动时间
	memset(m_sStartDate,0,sizeof(m_sStartDate));           //启动日期

}

CDrebStatInfo::~CDrebStatInfo()
{

}

void CDrebStatInfo::ReportRcvPack(int ntype, int packnum)
{
	switch (ntype)
	{
		case SOCK_TODREB:
		case SOCK_DREB:
			m_drebinfo.m_nTransInNum+=packnum;
			break;
		case SOCK_CLI:
			m_drebinfo.m_nRcvPackClient = packnum;
			break;
		case SOCK_SRV:
			m_drebinfo.m_nSrvAnsNum+=packnum;
			break;
		default:
			break;
	}
}

void CDrebStatInfo::ReportRcvByte(int ntype, int len)
{
	switch (ntype)
	{
		case SOCK_DREB:
		case SOCK_TODREB:
			m_drebinfo.m_nTransInByte+=len;
			break;
		case SOCK_CLI:
			m_drebinfo.m_nRcvByteClient+=len;
			break;
		case SOCK_SRV:
			m_drebinfo.m_nSrvAnsByte+=len;
			break;
		default:
			break;
	}
}

void CDrebStatInfo::ReportSendPack(int ntype, int packnum)
{
	switch (ntype)
	{
		case SOCK_DREB:
		case SOCK_TODREB:
			m_drebinfo.m_nTransOutNum+=packnum;
			break;
		case SOCK_CLI:
			m_drebinfo.m_nAnsPackClient+=packnum;
			break;
		case SOCK_SRV:
			m_drebinfo.m_nCallSrvNum+=packnum;
			break;
		default:
			break;
	}
}

void CDrebStatInfo::ReportSendByte(int ntype, int len)
{
	switch (ntype)
	{
		case SOCK_TODREB:
		case SOCK_DREB:
			m_drebinfo.m_nTransOutByte+=len;
			break;
		case SOCK_CLI:
			m_drebinfo.m_nAnsByteClient+=len;
			break;
		case SOCK_SRV:
			m_drebinfo.m_nCallSrvByte+=len;
			break;
		default:
			break;
	}
}
