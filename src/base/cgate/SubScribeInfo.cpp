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
	
	if (sub->flag <0 || sub->flag >1)
	{
		sprintf(msg,"订阅标志不符 %d",sub->flag);
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"%s",msg);
		return false;
	}
	switch (sub->flag)
	{
		case 0:
			Clear();
		    m_pLog->LogMp(LOG_PROMPT, __FILE__, __LINE__, "取消订阅");
			return true;
		case 1:
			m_pLog->LogMp(LOG_PROMPT, __FILE__, __LINE__, "订阅");
			break;
		default:
			return true;
	}  
	m_pEndian.Endian2LocalHost((unsigned char *)&(sub->datanum),sizeof(sub->datanum));

	if (datalen != sizeof(S_GATE_SUBSCRIBE)+(sub->datanum -1)*8)
	{
		sprintf(msg,"数据长度[%d]不符 合约数为[%d]",datalen, sub->datanum);
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"%s",msg);
		return false;
	}
	S_SUBSCRIBE_ *subitem= NULL;
	CBF_PMutex pmutex(&m_mutex);
	for (unsigned int i=0 ; i< sub->datanum; i++)
	{
		subitem = (S_SUBSCRIBE_*)(&sub->subinfo+(sub->datanum-1)*sizeof(S_SUBSCRIBE_));
        m_pEndian.Endian2LocalHost((unsigned char*)&(subitem->nServiceNo), sizeof(subitem->nServiceNo));
        m_pEndian.Endian2LocalHost((unsigned char*)&(subitem->nKey), sizeof(subitem->nKey));
        int rid = m_table.Add(*subitem);
        m_key.Add(rid, m_table.m_table[rid].nServiceNo, m_table.m_table[rid].nKey);
	}
	if (m_pLog->isWrite(LOG_DEBUG+1))
	{
		char dinfo[8192];
		char tmpchar[20];
		bzero(dinfo,sizeof(dinfo));
		bzero(tmpchar,sizeof(tmpchar));
		int id;
		bool bret = m_key.First(id);
		while (bret)
		{
			sprintf(tmpchar," %d-%d",m_table.m_table[id].nServiceNo,m_table.m_table[id].nKey);
			strcat(dinfo,tmpchar);
			bret = m_key.Next(id);
		}
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"订阅信息: %s",dinfo);
	}
	return true;
}
void CSubScribeInfo::Clear()
{
	CBF_PMutex pp(&m_mutex);
	m_table.Clear();
	m_key.Clear();
}
bool CSubScribeInfo::IsSubscribe(unsigned int funcno, unsigned int key)
{
	CBF_PMutex pp(&m_mutex);
	return m_key.Find(funcno,key);
}
