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
	m_drebinfo.m_nRcvPackClient=0;//���տͻ��˰���
	m_drebinfo.m_nRcvByteClient=0;//���տͻ����ֽ���
	m_drebinfo.m_nAnsPackClient=0;//Ӧ��ͻ��˰���
	m_drebinfo.m_nAnsByteClient=0;//Ӧ��ͻ����ֽ���
	
	m_drebinfo.m_nTransOutNum=0; //����ת������
	m_drebinfo.m_nTransOutByte=0;//����ת���ֽ���
	m_drebinfo.m_nTransInNum=0;  //����ת������
	m_drebinfo.m_nTransInByte=0; //����ת���ֽ���
	
	m_drebinfo.m_nCallSrvNum=0;   //���÷������
    m_drebinfo.m_nCallSrvByte=0;  //���÷����ֽ���
	m_drebinfo.m_nSrvAnsNum=0;    //����Ӧ�����
    m_drebinfo.m_nSrvAnsByte=0;   //����Ӧ���ֽ���
	
	m_nStartTime=0;           //����ʱ��
	memset(m_sStartDate,0,sizeof(m_sStartDate));           //��������

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
