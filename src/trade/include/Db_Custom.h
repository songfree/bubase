// Db_Custom.h: interface for the CDb_Custom class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DB_CUSTOM_H__F4B3ABE2_EC4D_4E00_AF69_0AA5BB64790F__INCLUDED_)
#define AFX_DB_CUSTOM_H__F4B3ABE2_EC4D_4E00_AF69_0AA5BB64790F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//数据库客户部分  客户信息、资金账户信息


typedef struct 
{
	char sCustNo[13];//客户编号
	char sCustName[41];//客户名称
	char sCustType[2];//客户类别
	char sCertType[2];//证件类型
	char sCertNo[41];//证件号码
	char sMobile[21];//手机
	char sTradePwd[33];//交易密码
	char sFundPwd[33];//资金密码
	char sLastModiPwdTime[18];//上次修改密码时间
	int  nPwdFaildNum;//密码失败次数
	char sChannleOpen[61];//开通渠道
	char sPkiCertNo[41];//证书序列号
	char sPkiCertDate[9];//证书有效期
	char sStatus[2];//客户状态
	char sLastPwdFaildDate[9];	//上次密码失败日期
	int  nLastPwdFaildTime;//上次密码失败时间
}S_MDB_CUSTINFO;
typedef struct 
{
	char sTxAcc[21];//交易账号
	char sCustNo[13];//客户编号
	char sAcctype[2];//账户类型
	char sAccName[41];//账户名称
	char sCustSimpleName[41];//客户简称
	char sSignOrg[13];//开户机构 所属机构  银行
	char sMgrOrg[13];//所属代理机构   代理经纪机构
	char sFutureFlag[2];//期货业务签约标志
	char sAccLevel[3];//账户级别
	char sExchFareModel[14];//交易所费率模板
	char sMemberFareModel[14];//会员费率模板
	char sAffirmFlag[2];//日结单确认标志
	char sAffirmDate[9];//日结单确认日期
	char sAddDateTime[18];//创建日期时间
	char sAddTerm[3];//创建来源
	char sAddOper[17];//创建操作员
    char sModiDateTime[18];//修改日期时间
    char sModiTerm[3];//修改来源
    char sModiOper[17];//修改操作员
	unsigned int nSysId;//所属经纪公司
	char sCurrency[2];//币种
	char sParentTxAcc[21];//父资金账号
	char sAccStatus[2];//账户状态
}S_MDB_TRADEACC_INFO;



typedef struct 
{
	int  nFundRowid;//资金表ROWID   一个交易账户只对应一个币种
	char sTxAcc[21];//资金账号
	int  nLockRowid;//在本表中的rowid
	int  nCustRowid;//客户表rowid
	int  nTradeAccRowid;//交易账号表rowid
	CBF_Mutex  *mutex;//互斥锁
}S_MDB_TXACC_LOCK;

//客户
class  CTbl_Cust  
{
protected:
	
	CMemTableNew <S_MDB_CUSTINFO>   m_table;
	CPkeyCharVUnordered<1>             m_pkey;//客户编号的key
	CBF_Mutex                 m_mutex;//互斥锁
public:
	CTbl_Cust()
	{
		
	}
	virtual ~CTbl_Cust()
	{
		
	}
	
	// 函数名: Insert
	// 编程  : 王明松 2017-8-3 10:21:03
	// 返回  : int 
	// 参数  : S_MDB_CUSTINFO cust
	// 描述  : 写入客户表 客户编号、手机号、证件三个唯一索引
	int Insert(S_MDB_CUSTINFO cust)  //只有在初始化时使用，所以可以不加锁
	{
		CBF_PMutex pp(&m_mutex);
		if (m_pkey.Find(cust.sCustNo))
		{
			return -1;
		}
		int rid = m_table.Add(cust);
		m_pkey.Add(rid,m_table.m_table[rid].sCustNo);
		return rid;
	}

	// 函数名: *SelectByNo
	// 编程  : 王明松 2017-8-3 10:21:58
	// 返回  : S_CUST_INFO 
	// 参数  : char *custno
	// 描述  : 根据客户编号查询客户信息
	S_MDB_CUSTINFO *SelectByNo(char *custno)
	{
		int rid;
		CBF_PMutex pp(&m_mutex);
		if (!m_pkey.GetRowId(rid,custno))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}
	int GetRowId(char *custno)
	{
		int rid;
		CBF_PMutex pp(&m_mutex);
		if (!m_pkey.GetRowId(rid,custno))
		{
			return -1;
		}
		return rid;
	}

	S_MDB_CUSTINFO *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
};
//交易账号表
class CTbl_Trade_Acc
{
protected:
	
	CMemTableNew <S_MDB_TRADEACC_INFO>   m_table;
	CPkeyCharVUnordered<1>               m_pkey;//交易账号
	CIndexVarChar<1>            m_index_accorg;//所属机构索引
	CIndexVarChar<1>            m_index_mgrorg;//代理机构索引
	CIndexVarChar<1>            m_index_parent;//父资金账号索引
	CIndexVarChar<1>            m_index_custno;//客户号索引
	CBF_Mutex                   m_mutex;//互斥锁
public:
	CTbl_Trade_Acc()
	{
		
	}
	virtual ~CTbl_Trade_Acc()
	{
		
	}
	void Clear()
	{
		CBF_PMutex pp(&m_mutex);
		m_pkey.Clear();//资金账号
		m_index_accorg.Clear();//所属机构索引
		m_index_mgrorg.Clear();//代理机构索引
		m_index_parent.Clear();//父资金账号索引
		m_index_custno.Clear();//客户号索引
		m_table.Clear();
	}
	// 函数名: Insert
	// 编程  : 王明松 2017-8-3 10:31:29
	// 返回  : bool 
	// 参数  : S_MDB_CUST_FUNDACC_INFO custacc
	// 描述  : 写入交易账户表，交易账户、客户编号/交易所/席位、交易所/席位/交易编码三个唯一索引，交易编码索引
	int Insert(S_MDB_TRADEACC_INFO custacc)
	{
		CBF_PMutex pp(&m_mutex);
		if (m_pkey.Find(custacc.sTxAcc))
		{
			return -1;
		}
		int rid = m_table.Add(custacc);
		m_pkey.Add(rid,m_table.m_table[rid].sTxAcc);
		m_index_parent.Add(rid,m_table.m_table[rid].sParentTxAcc);
		m_index_accorg.Add(rid,m_table.m_table[rid].sSignOrg);
		m_index_mgrorg.Add(rid,m_table.m_table[rid].sMgrOrg);
		m_index_custno.Add(rid,m_table.m_table[rid].sCustNo);
		return rid;
	}
	int Update(S_MDB_TRADEACC_INFO custacc)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		if (m_pkey.GetRowId(rid,custacc.sTxAcc))
		{
			strcpy(m_table.m_table[rid].sExchFareModel,custacc.sExchFareModel);
			strcpy(m_table.m_table[rid].sMemberFareModel,custacc.sMemberFareModel);
			strcpy(m_table.m_table[rid].sFutureFlag,custacc.sFutureFlag);
			return rid;
		}
		rid = m_table.Add(custacc);
		m_pkey.Add(rid,m_table.m_table[rid].sTxAcc);
		m_index_parent.Add(rid,m_table.m_table[rid].sParentTxAcc);
		m_index_accorg.Add(rid,m_table.m_table[rid].sSignOrg);
		m_index_mgrorg.Add(rid,m_table.m_table[rid].sMgrOrg);
		m_index_custno.Add(rid,m_table.m_table[rid].sCustNo);
		return rid;
	}
	// 函数名: *SelectByAcc
	// 编程  : 王明松 2017-8-3 10:33:22
	// 返回  : S_MDB_CUST_FUNDACC_INFO 
	// 参数  : char *txacc
	// 描述  : 根据交易账户查询交易账户信息
	S_MDB_TRADEACC_INFO*SelectByAcc(char *txacc)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		if (!m_pkey.GetRowId(rid,txacc))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
		
	}
	int GetRowId(char *txacc)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		if (!m_pkey.GetRowId(rid,txacc))
		{
			return -1;
		}
		return rid;
	}
	bool SelectAll(std::vector<S_MDB_TRADEACC_INFO*> &acclist)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		bool bRet = m_pkey.First(rid);
		if (!bRet)
		{
			return false;
		}
		while (bRet)
		{
			acclist.push_back(&(m_table.m_table[rid]));
			bRet = m_pkey.Next(rid);
		}
		return true;

	}
	bool SelectAll(CInt &acclist)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		bool bRet = m_pkey.First(rid);
		if (!bRet)
		{
			return false;
		}
		while (bRet)
		{
			acclist.Add(rid);
			bRet = m_pkey.Next(rid);
		}
		return true;

	}
	int GetRowidByAcc(char *txacc)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		if (!m_pkey.GetRowId(rid,txacc))
		{
			return -1;
		}
		return rid;
	}
	// 函数名: SelectByAccOrg
	// 编程  : 王明松 2017-8-4 11:13:45
	// 返回  : bool 
	// 参数  : char *org
	// 参数  : std::vector<S_MDB_CUST_FUNDACC_INFO *> &acclist
	// 描述  : 查找签约机构下的所有客户
	bool SelectByAccOrg(char *org,std::vector<S_MDB_TRADEACC_INFO*> &acclist)
	{
		CBF_PMutex pp(&m_mutex);
		std::vector<int> iset;
		if (!m_index_accorg.Select(iset,org))
		{
			return false;
		}
		for (int i=0; i< iset.size(); i++)
		{
			acclist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
	}
	bool SelectByAccOrg(char *org,CInt &acclist)
	{
		CBF_PMutex pp(&m_mutex);
		CInt iset;
		if (! m_index_accorg.Select(iset,org))
		{
			return false;
		}
		int rid;
		bool bret = iset.First(rid);
		while (bret)
		{
			acclist.Add(rid);
			bret = iset.Next(rid);
		}
		return true;

	}
	// 函数名: SelectByCust
	// 编程  : 王明松 2017-8-4 11:13:45
	// 返回  : bool 
	// 参数  : char *custno
	// 参数  : std::vector<S_MDB_CUST_FUNDACC_INFO *> &acclist
	// 描述  : 查找签约机构下的所有客户
	bool SelectByCust(char *custno,std::vector<S_MDB_TRADEACC_INFO*> &acclist)
	{
		CBF_PMutex pp(&m_mutex);
		std::vector<int> iset;
		if (!m_index_custno.Select(iset,custno))
		{
			return false;
		}
		for (int i=0; i< iset.size(); i++)
		{
			acclist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
	}

	// 函数名: SelectByMgrOrg
	// 编程  : 王明松 2017-8-4 11:14:10
	// 返回  : bool 
	// 参数  : char *org
	// 参数  : std::vector<S_MDB_CUST_FUNDACC_INFO *> &acclist
	// 描述  : 查找代理机构下的所有客户
	bool SelectByMgrOrg(char *org,std::vector<S_MDB_TRADEACC_INFO*> &acclist)
	{
		CBF_PMutex pp(&m_mutex);
		std::vector<int> iset;
		if (!m_index_mgrorg.Select(iset,org))
		{
			return false;
		}
		for (int i=0; i< iset.size(); i++)
		{
			acclist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
	}
	bool SelectByMgrOrg(char *org,CInt &acclist)
	{
		CBF_PMutex pp(&m_mutex);
		CInt iset;
		if (!m_index_mgrorg.Select(iset,org))
		{
			return false;
		}
		int rid;
		bool bret = iset.First(rid);
		while (bret)
		{
			acclist.Add(rid);
			bret = iset.Next(rid);
		}
		return true;
	}
	S_MDB_TRADEACC_INFO *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
};


class CTbl_TX_Acc_Lock
{
protected:
	
	CMemTableNew <S_MDB_TXACC_LOCK>   m_table;
	CPkeyCharVUnordered<1>               m_pkey;//资金账号
	CBF_Mutex                   m_mutex;//互斥锁
public:
	CTbl_TX_Acc_Lock()
	{
		
	}
	virtual ~CTbl_TX_Acc_Lock()
	{
		int rid;
		bool bret = m_pkey.First(rid);
		while (bret)
		{
			delete m_table.m_table[rid].mutex;
			m_table.m_table[rid].mutex = NULL;
			bret = m_pkey.Next(rid);
		}
	}

	int Insert(S_MDB_TXACC_LOCK acclock)  
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		if (m_pkey.GetRowId(rid,acclock.sTxAcc))
		{
//			m_table.m_table[rid].nFundRowid = acclock.nFundRowid;
			return rid;
		}
		acclock.mutex = new CBF_Mutex();
		rid = m_table.Add(acclock);
		m_pkey.Add(rid,m_table.m_table[rid].sTxAcc);
		m_table.m_table[rid].nLockRowid=rid;
		return rid;
	}

	// 函数名: *SelectByAcc
	// 编程  : 王明松 2017-8-3 10:33:22
	// 返回  : S_MDB_TXACC_LOCK 
	// 参数  : char *txacc
	// 描述  : 根据交易账户查询交易账户信息
	S_MDB_TXACC_LOCK *SelectByAcc(char *txacc)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		if (!m_pkey.GetRowId(rid,txacc))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
		
	}

	S_MDB_TXACC_LOCK *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
};

class CDb_Custom  
{
public:
	CDb_Custom()
	{

	}
	virtual ~CDb_Custom()
	{

	}
	CTbl_Cust         m_pCustom;//客户信息表
	CTbl_Trade_Acc    m_pTradeAcc;//交易账号信息表
	
};

#endif // !defined(AFX_DB_CUSTOM_H__F4B3ABE2_EC4D_4E00_AF69_0AA5BB64790F__INCLUDED_)
