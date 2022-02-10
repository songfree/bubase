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
	if (!m_pkey.Select(iset,svr,pid)) //û�д˷���
	{
		if (status == ARBSTATUS_UNKNOW) //��״̬Ϊδ֪��˵��������ѯ״̬��
		{
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"����[%d %d]��ѯ����",svr,pid);
			//ȡӦ�õ���Ϣ�������������Ǳ�
			if (!m_index_appid.Select(iset,svr))
			{
				//���ǵ�һ�����񣬿϶�������
				arb.cStatus = ARBSTATUS_MASTER;
			}
			else //�������е�״̬������ʱ�����ж��������Ǳ�
			{
				bRet = iset.First(id);
				while (bRet)
				{
					//���״̬
					if (m_table.m_table[id].cStatus == ARBSTATUS_MASTER \
						|| m_table.m_table[id].cStatus == ARBSTATUS_MASTER_AYN)
					{   //��������Ϊ��
						arb.cStatus = ARBSTATUS_BACKUP;
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"����[%d %d]Ϊ��������[%d %d]Ϊ��",\
							m_table.m_table[id].nSvrMainId,m_table.m_table[id].cSvrPrivateId,svr,pid);
						break;
					}
					m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"����[%d %d] ״̬[%d] Ϊ����",\
							m_table.m_table[id].nSvrMainId,m_table.m_table[id].cSvrPrivateId,m_table.m_table[id].cStatus);
					bRet = iset.Next(id);
				}
				if (arb.cStatus == ARBSTATUS_UNKNOW) 
				{
					//״̬���Ǳ�����˵��û��������������Ϊ��
					arb.cStatus = ARBSTATUS_MASTER;
					m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"����[%d]����������[%d %d]Ϊ��",\
							svr,m_table.m_table[id].nSvrMainId,m_table.m_table[id].cSvrPrivateId);
				}
			}

			//����
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
		else  //�����״̬֪ͨ
		{
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"����[%d %d]״̬֪ͨ[%d]",svr,pid,status);
			//����
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
	//�д˷����״̬֪ͨ
	bRet = iset.First(id);
	if (!bRet)
	{
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"������û��");
		return NULL;
	}
	if (status != ARBSTATUS_UNKNOW)
	{
		//״̬֪ͨ
		if (status != m_table.m_table[id].cStatus)
		{
			m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"����[%d %d]״̬�����仯����[%d]��Ϊ[%d]",\
				svr,pid,m_table.m_table[id].cStatus,status);
// 			if (m_table.m_table[id].cStatus != ARBSTATUS_MASTER && m_table.m_table[id].cStatus != ARBSTATUS_MASTER_AYN )
// 			{
// 				m_table.m_table[id].cStatus = status;
// 			}
			m_table.m_table[id].cStatus = status;
			
		}
		//����
		m_table.m_table[id].nUpdateTime = time(NULL);
		return &(m_table.m_table[id]);
	}
	else //������ѯ
	{
		//�ҵ�ԭ���ļ�¼
		int oid=0;
		iset.Clear();
		//ȡӦ�õ���Ϣ�������������Ǳ�
		m_index_appid.Select(iset,svr);//�����жϣ�������һ����¼
		
		bRet = iset.First(oid);
		while (bRet)
		{
			//���״̬
			if (m_table.m_table[oid].cStatus == ARBSTATUS_MASTER \
				|| m_table.m_table[oid].cStatus == ARBSTATUS_MASTER_AYN)
			{   //��������Ϊ��
				if (oid == id) //����ͬһ������
				{
					m_table.m_table[id].nUpdateTime = time(NULL);
					m_table.m_table[id].nQueryStatusTime = m_table.m_table[id].nUpdateTime;
					m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"����[%d %d] ״̬[%d] Ϊ��������״̬����",\
						m_table.m_table[oid].nSvrMainId,m_table.m_table[oid].cSvrPrivateId,\
						m_table.m_table[oid].cStatus);
					
					return &(m_table.m_table[id]);
				}
				else //����ͬһ���񣬵���һ����Ϊ������˷���Ϊ��
				{
					m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"����[%d %d] ״̬[%d] Ϊ�������ʷ���[%d %d]Ϊ����",\
						m_table.m_table[oid].nSvrMainId,m_table.m_table[oid].cSvrPrivateId,\
						m_table.m_table[oid].cStatus,m_table.m_table[id].nSvrMainId,m_table.m_table[id].cSvrPrivateId);
					m_table.m_table[id].cStatus = ARBSTATUS_BACKUP;
					return &(m_table.m_table[id]);
				}
				
			}
			m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"����[%d %d] ״̬[%d] Ϊ����",\
				m_table.m_table[oid].nSvrMainId,m_table.m_table[oid].cSvrPrivateId,m_table.m_table[oid].cStatus);
			bRet = iset.Next(oid);
		}
		
		//û���ҵ����������ñ�����Ϊ����
		m_table.m_table[id].cStatus = ARBSTATUS_MASTER;
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"����[%d]����������[%d %d]Ϊ��",\
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
