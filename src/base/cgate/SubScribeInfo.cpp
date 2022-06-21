// SubScribeInfo.cpp: implementation of the CSubScribeInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "SubScribeInfo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSubScribeInfo::CSubScribeInfo()
{
	m_pLog = NULL;
}

CSubScribeInfo::~CSubScribeInfo()
{
	m_listVariety.Clear();
	m_listSubscribeKey.Clear();
}

bool CSubScribeInfo::Subscribe(const char *data,int datalen, char *msg)
{
	if (datalen < sizeof(S_GATE_SUBSCRIBE))
	{
		sprintf(msg,"数据长度不符");
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"订阅报文不符 %s",msg);
		return false;
	}
	PS_GATE_SUBSCRIBE sub = (PS_GATE_SUBSCRIBE)data;
	
	if (sub->flag <0 || sub->flag >3)
	{
		sprintf(msg,"订阅标志不符 %d",sub->flag);
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"%s",msg);
		return false;
	}
	switch (sub->flag)
	{
		case 0:
			m_listVariety.Clear();
			m_pLog->LogMp(LOG_PROMPT, __FILE__, __LINE__, "取消行情订阅");
			return true;
		case 3:
		    m_listSubscribeKey.Clear();
			m_pLog->LogMp(LOG_PROMPT, __FILE__, __LINE__, "取消回报类订阅");
			return true;
        case 1:
            m_listVariety.Clear();
            break;
        case 2:
            m_listSubscribeKey.Clear();
            break;
		default:
			return true;
	}  
	m_pEndian.Endian2LocalHost((unsigned char *)&(sub->datanum),sizeof(sub->datanum));
	
	int nQuoDataNum = sub->datanum;
	if (datalen != sizeof(S_GATE_SUBSCRIBE)+(nQuoDataNum -1)*sizeof(sub->variety))
	{
		sprintf(msg,"数据长度[%d]不符 合约数为[%d]",datalen, nQuoDataNum);
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"%s",msg);
		return false;
	}
	m_pEndian.Endian2LocalHost((unsigned char *)&(sub->variety),sizeof(sub->variety));
	CBF_PMutex pmutex(&m_mutex);
    if (sub->flag == 1)
    {
		m_listVariety.Add(sub->variety);
    }
    else
    {
        m_listSubscribeKey.Add(sub->variety);
    }
	

	char *pp = (char *)data +sizeof(S_GATE_SUBSCRIBE);
	for (int i=0 ; i< nQuoDataNum -1 ; i++)
	{
		int *pint = (int *)pp;
		m_pEndian.Endian2LocalHost((unsigned char *)pint,sizeof(int));
		if (sub->flag == 1)
		{
			m_listVariety.Add(*pint);
		}
		else
		{
			m_listSubscribeKey.Add(*pint);
		}
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
	CBF_PMutex pp(&m_mutex);
	return m_listVariety.Find(varietycode);
}
bool CSubScribeInfo::IsSubscribe(int key)
{
	CBF_PMutex pp(&m_mutex);
	return m_listSubscribeKey.Find(key);
}
