// ArbTbl.cpp: implementation of the CArbTbl class.
//
//////////////////////////////////////////////////////////////////////

#include "ArbTbl.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CArbTbl::CArbTbl()
{
	m_pLog = NULL;
}

CArbTbl::~CArbTbl()
{

}

ARB_S_APPINFO * CArbTbl::UpdateArb(unsigned int svr, char pid, char status,unsigned short drebid)
{
	int id;
	CInt iset;
	bool bRet;
	ARB_S_APPINFO arb;
	bzero(&arb,sizeof(ARB_S_APPINFO));
	if (!m_pkey.Select(iset,svr,pid)) //没有此服务
	{
		if (status == ARBSTATUS_UNKNOW) //若状态为未知，说明是来查询状态的
		{
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"服务[%d %d]查询主备",svr,pid);
			//取应用的信息，查找是主还是备
			if (!m_index_appid.Select(iset,svr))
			{
				//这是第一个服务，肯定是主了
				arb.cStatus = ARBSTATUS_MASTER;
			}
			else //查找现有的状态，根据时间来判断是主还是备
			{
				bRet = iset.First(id);
				while (bRet)
				{
					//检查状态
					if (m_table.m_table[id].cStatus == ARBSTATUS_MASTER \
						|| m_table.m_table[id].cStatus == ARBSTATUS_MASTER_AYN)
					{   //若有主则为备
						arb.cStatus = ARBSTATUS_BACKUP;
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"服务[%d %d]为主机，置[%d %d]为备",\
							m_table.m_table[id].nSvrMainId,m_table.m_table[id].cSvrPrivateId,svr,pid);
						break;
					}
					m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"服务[%d %d] 状态[%d] 为备机",\
							m_table.m_table[id].nSvrMainId,m_table.m_table[id].cSvrPrivateId,m_table.m_table[id].cStatus);
					bRet = iset.Next(id);
				}
				if (arb.cStatus == ARBSTATUS_UNKNOW) 
				{
					//状态不是备，则说明没有主机，则将其置为主
					arb.cStatus = ARBSTATUS_MASTER;
					m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"服务[%d]无主机，置[%d %d]为主",\
							svr,m_table.m_table[id].nSvrMainId,m_table.m_table[id].cSvrPrivateId);
				}
			}

			//增加
			arb.nNodeId = drebid;
			arb.nSvrMainId = svr;
			arb.cSvrPrivateId = pid;
			arb.nAddTime = time(NULL);
			arb.nQueryStatusTime = arb.nAddTime;
			arb.nUpdateTime = arb.nAddTime;
			arb.nLastSwtichTime = arb.nAddTime;
			id = m_table.Add(arb);
			
			m_index_appid.Add(id,arb.nSvrMainId);
			m_pkey.Add(id,arb.nSvrMainId,arb.cSvrPrivateId);
			return &(m_table.m_table[id]);
		}
		else  //服务的状态通知
		{
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"服务[%d %d]状态通知[%d]",svr,pid,status);
			//增加
			arb.nNodeId = drebid;
			arb.nSvrMainId = svr;
			arb.cSvrPrivateId = pid;
			arb.cStatus = status;
			arb.nAddTime = time(NULL);
			arb.nQueryStatusTime = arb.nAddTime;
			arb.nUpdateTime = arb.nAddTime;
			arb.nLastSwtichTime = arb.nAddTime;
			id = m_table.Add(arb);
			
			m_index_appid.Add(id,arb.nSvrMainId);
			m_pkey.Add(id,arb.nSvrMainId,arb.cSvrPrivateId);
			return &(m_table.m_table[id]);
		}
	}
	//有此服务的状态通知
	bRet = iset.First(id);
	if (!bRet)
	{
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"怎可能没有");
		return NULL;
	}
	if (status != ARBSTATUS_UNKNOW)
	{
		//状态通知
		if (status != m_table.m_table[id].cStatus)
		{
			m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"服务[%d %d]状态发生变化，由[%d]变为[%d]",\
				svr,pid,m_table.m_table[id].cStatus,status);
// 			if (m_table.m_table[id].cStatus != ARBSTATUS_MASTER && m_table.m_table[id].cStatus != ARBSTATUS_MASTER_AYN )
// 			{
// 				m_table.m_table[id].cStatus = status;
// 			}
			m_table.m_table[id].cStatus = status;
			
		}
		//更新
		m_table.m_table[id].nUpdateTime = time(NULL);
		return &(m_table.m_table[id]);
	}
	else //主备查询
	{
		//找到原来的记录
		int oid=0;
		iset.Clear();
		//取应用的信息，查找是主还是备
		m_index_appid.Select(iset,svr);//不用判断，至少有一条记录
		
		bRet = iset.First(oid);
		while (bRet)
		{
			//检查状态
			if (m_table.m_table[oid].cStatus == ARBSTATUS_MASTER \
				|| m_table.m_table[oid].cStatus == ARBSTATUS_MASTER_AYN)
			{   //若有主则为备
				if (oid == id) //就是同一个服务
				{
					m_table.m_table[id].nUpdateTime = time(NULL);
					m_table.m_table[id].nQueryStatusTime = m_table.m_table[id].nUpdateTime;
					m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"服务[%d %d] 状态[%d] 为主机，故状态不变",\
						m_table.m_table[oid].nSvrMainId,m_table.m_table[oid].cSvrPrivateId,\
						m_table.m_table[oid].cStatus);
					
					return &(m_table.m_table[id]);
				}
				else //不是同一服务，但另一服务为主，则此服务为备
				{
					m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"服务[%d %d] 状态[%d] 为主机，故服务[%d %d]为备机",\
						m_table.m_table[oid].nSvrMainId,m_table.m_table[oid].cSvrPrivateId,\
						m_table.m_table[oid].cStatus,m_table.m_table[id].nSvrMainId,m_table.m_table[id].cSvrPrivateId);
					m_table.m_table[id].cStatus = ARBSTATUS_BACKUP;
					return &(m_table.m_table[id]);
				}
				
			}
			m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"服务[%d %d] 状态[%d] 为备机",\
				m_table.m_table[oid].nSvrMainId,m_table.m_table[oid].cSvrPrivateId,m_table.m_table[oid].cStatus);
			bRet = iset.Next(oid);
		}
		
		//没有找到主机，则置本服务为主机
		m_table.m_table[id].cStatus = ARBSTATUS_MASTER;
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"服务[%d]无主机，置[%d %d]为主",\
			svr,m_table.m_table[id].nSvrMainId,m_table.m_table[id].cSvrPrivateId);
		m_table.m_table[id].nQueryStatusTime = time(NULL);
		m_table.m_table[id].nUpdateTime = time(NULL);
		return &(m_table.m_table[id]);
	}
	
}

bool CArbTbl::GetAppList(std::vector<ARB_S_APPINFO> &applist)
{
	bool bRet;
	int id;
	bRet = m_index_appid.First(id);
	if (!bRet)
	{
		return bRet;
	}
	while (bRet)
	{
		applist.push_back(m_table.m_table[id]);
		bRet = m_index_appid.Next(id);
	}
	return true;
}

bool CArbTbl::GetDistinctApp(std::vector<int> &applist)
{
	bool bRet;
	int id;
	CInt tmplist;
	bRet = m_index_appid.First(id);
	if (!bRet)
	{
		return bRet;
	}
	while (bRet)
	{
		tmplist.Add(m_table.m_table[id].nSvrMainId);
		bRet = m_index_appid.Next(id);
	}
	bRet = tmplist.First(id);
	while (bRet)
	{
		applist.push_back(id);
		bRet = tmplist.Next(id);
	}
	return true;
}

bool CArbTbl::GetAppInfo(unsigned int svrid,std::vector<ARB_S_APPINFO *> &applist)
{
	int id;
	CInt iset;
	bool bRet;
	if (!m_index_appid.Select(iset,svrid))
	{
		return false;
	}
	bRet = iset.First(id);
	if (!bRet)
	{
		return bRet;
	}
	while (bRet)
	{
		applist.push_back(&(m_table.m_table[id]));
		bRet = iset.Next(id);
	}
	return true;
}

ARB_S_APPINFO * CArbTbl::GetAppInfo(unsigned int svrid, char pid)
{
	int id;
	CInt iset;
	if (!m_pkey.Select(iset,svrid,pid))
	{
		return NULL;
	}
	if (!iset.First(id))
	{
		return NULL;
	}
	return &(m_table.m_table[id]);
}

void CArbTbl::DeleteApp(unsigned int svr, char pid)
{

}
