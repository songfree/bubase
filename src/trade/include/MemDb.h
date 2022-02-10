// MemDb.h: interface for the CMemDb class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMDB_H__2EBC663A_63AF_4C36_92E0_2671FE9B4780__INCLUDED_)
#define AFX_MEMDB_H__2EBC663A_63AF_4C36_92E0_2671FE9B4780__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "MdbBase.h"
#include "BF_Mutex.h"
#include "Db_Public.h"
#include "Db_Set.h"
#include "Db_Trader.h"
#include "Db_Custom.h"
#include "Db_BaseInfo.h"
#include "CodeTable.h"
#include "BF_Queue.h"




class CMemDb  
{
public:
	CMemDb()
	{
		m_bIsNewQuotation = false;
	}
	virtual ~CMemDb()
	{

	}
	CDb_Public      m_pDbPublic;//�������ݿ⣬����������Ա��
	CDb_Custom      m_pDbCust;//�ͻ���Ϣ
	CDb_BaseInfo    m_pDbBaseinfo;//������Ϣ
	CDb_Set         m_pDbSet;//������Ϣ
	CDb_Trader      m_pDbTrader;//�������

	bool    m_bDataInit;//�����Ƿ��ʼ��
	
	bool    m_bIsNewQuotation;//�Ƿ����µ����飬�������ռ���

	std::list<S_MDB_DONE_INFO> m_pUnProcessDoneMatch;//δ����ĳɽ����Ҳ���ί�к�
	CBF_Mutex  m_pUnProcessDoneMatchMutex;//δ����ɽ�������

protected:
	//�����ʽ��˺���Ϣ�������ʽ��˺���Ϣ��rowid���ṩ����𡢱������ɽ��Ľӿڣ����������ĳ���ͻ���������������,��ֹͬһ�ͻ���������Ӱ��
	CTbl_TX_Acc_Lock  m_pTxAccLock;//�ʽ��˺�����

	

public:
	
	CInt   m_pSurplusLockRowId;//��ż����ӯ�����ʽ��˺��������rowid����ʱ���ݴ˱����ҵ��ʽ��˺ţ�������նȼ����ռ���
	CBF_Mutex  m_pSurplusLockRowIdLock;//���CInt   m_pSurplusLockRowId����
	


	// ������: InsertAccLock
	// ���  : ������ 2018-3-8 13:51:20
	// ����  : int 
	// ����  : S_MDB_TXACC_LOCK info
	// ����  : д��ͻ�����
	int InsertAccLock(S_MDB_TXACC_LOCK info)
	{
		return m_pTxAccLock.Insert(info);
	}

	// ������: *Select
	// ���  : ������ 2018-3-8 13:51:35
	// ����  : S_MDB_TXACC_LOCK 
	// ����  : char *txacc
	// ����  : �����ʽ��˺�ȡ������
	S_MDB_TXACC_LOCK *SelectLock(char *txacc)
	{
		S_MDB_TXACC_LOCK *info =   m_pTxAccLock.SelectByAcc(txacc);
		if (info != NULL)
		{
			int rid;
			if (info->nFundAccRowid<0)
			{
				rid= m_pDbCust.m_pCustFundAcc.GetRowId(txacc);
				if (rid<0)
				{
					return NULL;
				}
				info->nFundAccRowid = rid;
				rid = m_pDbCust.m_pCustom.GetRowId(m_pDbCust.m_pCustFundAcc.At(rid)->sCustNo);
				if (rid<0)
				{
					return NULL;
				}
				info->nCustRowid = rid;
				//ȡ�ʽ���rowid
				std::vector<int>iset;
				if (m_pDbTrader.m_pFund.Select(txacc,iset))
				{
					S_MDB_CUST_FUND *pfund=NULL;
					for (int i=0;i<iset.size(); i++)
					{
						pfund = m_pDbTrader.m_pFund.At(iset[i]);
						if (pfund != NULL)
						{
							info->nFundRowid[atoi(pfund->sCurrency)] = iset[i];
						}
					}
				}
			}
			return info;
		}
		return NULL;
	}
	S_MDB_TXACC_LOCK *SelectLock(int rowid)
	{
		S_MDB_TXACC_LOCK *info =m_pTxAccLock.At(rowid);
		int rid;
		if (info->nFundAccRowid<0)
		{
			rid= m_pDbCust.m_pCustFundAcc.GetRowId(info->sTxAcc);
			if (rid<0)
			{
				return NULL;
			}
			info->nFundAccRowid = rid;
			rid = m_pDbCust.m_pCustom.GetRowId(m_pDbCust.m_pCustFundAcc.At(rid)->sCustNo);
			if (rid<0)
			{
				return NULL;
			}
			info->nCustRowid = rid;
			//ȡ�ʽ���rowid
			std::vector<int>iset;
			if (m_pDbTrader.m_pFund.Select(info->sTxAcc,iset))
			{
				S_MDB_CUST_FUND *pfund=NULL;
				for (int i=0;i<iset.size(); i++)
				{
					pfund = m_pDbTrader.m_pFund.At(iset[i]);
					if (pfund != NULL)
					{
						info->nFundRowid[atoi(pfund->sCurrency)] = iset[i];
					}
				}
			}
		}
		return info;
	}
	S_MDB_CUST_FUND *GetFund(S_MDB_TXACC_LOCK *acclock,char *currency)
	{
		int ncur = atoi(currency);
		if (ncur<0 || ncur>19)
		{
			return NULL;
		}
		int rowid = acclock->nFundRowid[ncur];
		return m_pDbTrader.m_pFund.At(rowid);
	}

};

#endif // !defined(AFX_MEMDB_H__2EBC663A_63AF_4C36_92E0_2671FE9B4780__INCLUDED_)
