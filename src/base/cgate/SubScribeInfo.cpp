// SubScribeInfo.cpp: implementation of the CSubScribeInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "SubScribeInfo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSubScribeInfo::CSubScribeInfo()
{
	m_nQuoSubcribeFlag =0;
	m_pLog = NULL;
}

CSubScribeInfo::~CSubScribeInfo()
{
	m_listVariety.Clear();
}

bool CSubScribeInfo::QuoSubscribe(const char *data,int datalen, char *msg)
{
	if (datalen < sizeof(S_GATE_SUBSCRIBE))
	{
		sprintf(msg,"���ݳ��Ȳ���");
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ı��Ĳ��� %s",msg);
		return false;
	}
	PS_GATE_SUBSCRIBE sub = (PS_GATE_SUBSCRIBE)data;
	
	if (sub->flag <0 || sub->flag >2)
	{
		sprintf(msg,"���ı�־���� %d",sub->flag);
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"%s",msg);
		return false;
	}
	m_listVariety.Clear();
	m_nQuoSubcribeFlag = sub->flag;
	if (0 == sub->flag )
	{
		m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"ȡ������");
		return true;
	}
	m_pEndian.Endian2LocalHost((unsigned char *)&(sub->datanum),sizeof(sub->datanum));
	if (2 == sub->flag)
	{
		if (sub->datanum <0 || sub->datanum > 999)
		{
			sprintf(msg,"�г�����[%d]����",sub->datanum);
			m_nQuoSubcribeFlag = 0;
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"%s",msg);
			return false;
		}
		m_nQuoDataNum = sub->datanum;
		m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"���г����ĵ��г����� %d",m_nQuoDataNum);
		return true;
	}
	m_nQuoDataNum = sub->datanum;
	if (datalen != sizeof(S_GATE_SUBSCRIBE)+(m_nQuoDataNum-1)*sizeof(sub->variety))
	{
		sprintf(msg,"���ݳ���[%d]���� ��Լ��Ϊ[%d]",datalen,m_nQuoDataNum);
		m_nQuoSubcribeFlag = 0;
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"%s",msg);
		return false;
	}
	m_pEndian.Endian2LocalHost((unsigned char *)&(sub->variety),sizeof(sub->variety));
	m_listVariety.Add(sub->variety);

	char *pp = (char *)data +sizeof(S_GATE_SUBSCRIBE);
	for (int i=0 ; i<m_nQuoDataNum-1 ; i++)
	{
		int *pint = (int *)pp;
		m_pEndian.Endian2LocalHost((unsigned char *)pint,sizeof(int));
		m_listVariety.Add(*pint);
		pp+=4;
	}
	if (m_pLog->isWrite(LOG_DEBUG+1))
	{
		char dinfo[8192];
		char tmpchar[20];
		bzero(dinfo,sizeof(dinfo));
		bzero(tmpchar,sizeof(tmpchar));
		int id;
		bool bret = m_listVariety.First(id);
		while (bret)
		{
			sprintf(tmpchar," %d",id);
			strcat(dinfo,tmpchar);
			bret = m_listVariety.Next(id);
		}
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"����Լ���ĵĺ�Լ���� %s",dinfo);
	}
	return true;
}

bool CSubScribeInfo::IsQuoSubscribe(int varietycode)
{
	if (m_nQuoSubcribeFlag == 0)
	{
		return false;
	}
	if (m_nQuoSubcribeFlag == 1)  //����Ŷ���
	{
		return m_listVariety.Find(varietycode);
	}
	if (m_nQuoSubcribeFlag == 2) //���г�����
	{
		//ǰ��λΪ�г����� ��6λΪ��Լ����
		if (varietycode > m_nQuoDataNum *1000000 && varietycode <(m_nQuoDataNum+1)*10000000 )
		{
			return true;
		}
		else
		{
			return false;
		}

	}
	return false;
}
