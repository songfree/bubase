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
	m_nSubscribFlag = 1;
}

CSubScribeInfo::~CSubScribeInfo()
{
}
bool CSubScribeInfo::SubscribeAll(bool allflag, char* msg)
{
	if (allflag)
	{
		m_nSubscribFlag = 2;
	}
	else
	{
		m_nSubscribFlag = 1;
	}
	return true;
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
	
	if (sub->flag <0 || sub->flag >2)
	{
		sprintf(msg,"订阅标志不符 %d",sub->flag);
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"%s",msg);
		return false;
	}
	//m_pLog->LogMp(LOG_PROMPT, __FILE__, __LINE__, "订阅标志:%d 订阅数:%d",sub->flag,sub->datanum);
	switch (sub->flag)
	{
		case 0:
		    //m_pLog->LogMp(LOG_PROMPT, __FILE__, __LINE__, "取消订阅");
			SubscribeAll(false, msg);
			break;
		case 1:
			//m_pLog->LogMp(LOG_PROMPT, __FILE__, __LINE__, "订阅");
			SubscribeAll(false, msg);
			break;
        case 2:
            //订阅所有
			return SubscribeAll(true,msg);
        case 3:
            //订阅所有
            return SubscribeAll(false,msg);
		default:
			return true;
	}  
	m_pEndian.Endian2LocalHost((unsigned char *)&(sub->datanum),sizeof(sub->datanum));
	if (datalen != sizeof(S_GATE_SUBSCRIBE)+(sub->datanum -1)*sizeof(S_SUBSCRIBE_))
	{
		sprintf(msg,"数据长度[%d]不符 合约数为[%d]",datalen, sub->datanum);
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"%s",msg);
		return false;
	}
	S_SUBSCRIBE_ *subitem= NULL;
	subitem = &sub->subinfo;
    m_pEndian.Endian2LocalHost((unsigned char*)&(subitem->nServiceNo), sizeof(subitem->nServiceNo));
    m_pEndian.Endian2LocalHost((unsigned char*)&(subitem->nKey), sizeof(subitem->nKey));
	if (sub->flag == 0)
	{
		if (UnSubscribe(subitem->nServiceNo,subitem->nKey)<0)
        {
			//m_pLog->LogMp(LOG_WARNNING, __FILE__, __LINE__, "取消订阅 [%d %d]失败，未订阅", subitem->nServiceNo, subitem->nKey);
        }
        else
        {
            //m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "取消订阅 [%d %d]完成", subitem->nServiceNo, subitem->nKey);
        }
	}
	else
	{
        if (Subscribe(subitem->nServiceNo, subitem->nKey) < 0)
        {
            //m_pLog->LogMp(LOG_WARNNING, __FILE__, __LINE__, "订阅 [%d %d]失败，已订阅", subitem->nServiceNo, subitem->nKey);
        }
        else
        {
            //m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "订阅 [%d %d]完成", subitem->nServiceNo, subitem->nKey);
        }
	}
	for (unsigned int i=1 ; i< sub->datanum; i++)
	{
		subitem = (S_SUBSCRIBE_*)(data+sizeof(S_GATE_SUBSCRIBE)+(i-1)*sizeof(S_SUBSCRIBE_));
        m_pEndian.Endian2LocalHost((unsigned char*)&(subitem->nServiceNo), sizeof(subitem->nServiceNo));
        m_pEndian.Endian2LocalHost((unsigned char*)&(subitem->nKey), sizeof(subitem->nKey));
        if (sub->flag == 0)
        {
            if (UnSubscribe(subitem->nServiceNo, subitem->nKey) < 0)
            {
                //m_pLog->LogMp(LOG_WARNNING, __FILE__, __LINE__, "取消订阅 [%d %d]失败，未订阅", subitem->nServiceNo, subitem->nKey);
            }
            else
            {
                //m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "取消订阅 [%d %d]完成", subitem->nServiceNo, subitem->nKey);
            }
        }
        else
        {
            if (Subscribe(subitem->nServiceNo, subitem->nKey) < 0)
            {
                //m_pLog->LogMp(LOG_WARNNING, __FILE__, __LINE__, "订阅 [%d %d]失败，已订阅", subitem->nServiceNo, subitem->nKey);
            }
			else
			{
				//m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "订阅 [%d %d]完成", subitem->nServiceNo, subitem->nKey);
			}
			
        }
	}
	if (m_pLog->isWrite(LOG_DEBUG+1))
	{
		std::string info;
		std::vector<S_SUBSCRIBE_ *>reslist;
		Select(reslist);
		for (unsigned int i = 0; i < reslist.size(); i++)
		{
			info = info +std::to_string(reslist[i]->nServiceNo)+"-"+ std::to_string(reslist[i]->nKey)+" ";
		}
		m_pLog->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"订阅信息", (char *)info.c_str(),info.length());
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
	if (m_nSubscribFlag == 2)
	{
		return true;
	}
	CBF_PMutex pp(&m_mutex);
	return m_key.Find(funcno,key);
}
int CSubScribeInfo::Subscribe(unsigned int funcno, unsigned int key)
{
	S_SUBSCRIBE_  sub;
	sub.nServiceNo = funcno;
	sub.nKey = key;
	CBF_PMutex pp(&m_mutex);
	if (!m_key.Find(funcno, key))
	{
	   int rid = m_table.Add(sub);
	   m_key.Add(rid,m_table.m_table[rid].nServiceNo, m_table.m_table[rid].nKey);
	   return rid;
	}
	return -1;

}
int CSubScribeInfo::UnSubscribe(unsigned int funcno, unsigned int key)
{
    CBF_PMutex pp(&m_mutex);
	int rid;
    if (!m_key.Select(rid,funcno, key))
    {
        return -1;
    }
	m_key.Delete(funcno, key);
	m_table.Delete(rid);
	return rid;
}
int  CSubScribeInfo::Select(std::vector<S_SUBSCRIBE_*>&reslist)
{
	CBF_PMutex pp(&m_mutex);
    int id;
    bool bret = m_key.First(id);
    while (bret)
    {
        reslist.push_back(&m_table.m_table[id]);
        bret = m_key.Next(id);
    }
	return reslist.size();
}
