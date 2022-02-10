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
	CDb_Public      m_pDbPublic;//公共数据库，机构、操作员等
	CDb_Custom      m_pDbCust;//客户信息
	CDb_BaseInfo    m_pDbBaseinfo;//基本信息
	CDb_Set         m_pDbSet;//设置信息
	CDb_Trader      m_pDbTrader;//交易相关

	bool    m_bDataInit;//数据是否初始化
	
	bool    m_bIsNewQuotation;//是否有新的行情，触发风险计算

	std::list<S_MDB_DONE_INFO> m_pUnProcessDoneMatch;//未处理的成交，找不到委托号
	CBF_Mutex  m_pUnProcessDoneMatchMutex;//未处理成交队列锁

protected:
	//虚拟资金账号信息表，保存资金账号信息的rowid，提供出入金、报单、成交的接口，在里面针对某个客户加锁，类似行锁,防止同一客户并发数据影响
	CTbl_TX_Acc_Lock  m_pTxAccLock;//资金账号锁表

	

public:
	
	CInt   m_pSurplusLockRowId;//存放计算过盈亏的资金账号在锁表的rowid，定时根据此表来找到资金账号，计算风险度及风险级别。
	CBF_Mutex  m_pSurplusLockRowIdLock;//针对CInt   m_pSurplusLockRowId的锁
	


	// 函数名: InsertAccLock
	// 编程  : 王明松 2018-3-8 13:51:20
	// 返回  : int 
	// 参数  : S_MDB_TXACC_LOCK info
	// 描述  : 写入客户锁表
	int InsertAccLock(S_MDB_TXACC_LOCK info)
	{
		return m_pTxAccLock.Insert(info);
	}

	// 函数名: *Select
	// 编程  : 王明松 2018-3-8 13:51:35
	// 返回  : S_MDB_TXACC_LOCK 
	// 参数  : char *txacc
	// 描述  : 根据资金账号取得锁表
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
				//取资金表的rowid
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
			//取资金表的rowid
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
