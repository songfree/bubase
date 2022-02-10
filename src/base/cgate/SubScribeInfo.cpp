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
		sprintf(msg,"数据长度不符");
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"订阅报文不符 %s",msg);
		return false;
	}
	PS_GATE_SUBSCRIBE sub = (PS_GATE_SUBSCRIBE)data;
	
	if (sub->flag <0 || sub->flag >2)
	{
		sprintf(msg,"订阅标志不符 %d",sub->flag);
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"%s",msg);
		return false;
	}
	m_listVariety.Clear();
	m_nQuoSubcribeFlag = sub->flag;
	if (0 == sub->flag )
	{
		m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"取消订阅");
		return true;
	}
	m_pEndian.Endian2LocalHost((unsigned char *)&(sub->datanum),sizeof(sub->datanum));
	if (2 == sub->flag)
	{
		if (sub->datanum <0 || sub->datanum > 999)
		{
			sprintf(msg,"市场代码[%d]不符",sub->datanum);
			m_nQuoSubcribeFlag = 0;
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"%s",msg);
			return false;
		}
		m_nQuoDataNum = sub->datanum;
		m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"按市场订阅的市场代码 %d",m_nQuoDataNum);
		return true;
	}
	m_nQuoDataNum = sub->datanum;
	if (datalen != sizeof(S_GATE_SUBSCRIBE)+(m_nQuoDataNum-1)*sizeof(sub->variety))
	{
		sprintf(msg,"数据长度[%d]不符 合约数为[%d]",datalen,m_nQuoDataNum);
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
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"按合约订阅的合约代码 %s",dinfo);
	}
	return true;
}

bool CSubScribeInfo::IsQuoSubscribe(int varietycode)
{
	if (m_nQuoSubcribeFlag == 0)
	{
		return false;
	}
	if (m_nQuoSubcribeFlag == 1)  //按编号订阅
	{
		return m_listVariety.Find(varietycode);
	}
	if (m_nQuoSubcribeFlag == 2) //按市场订阅
	{
		//前三位为市场代码 后6位为合约代码
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
