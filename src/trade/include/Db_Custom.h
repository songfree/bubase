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
	char sChannleOpen[61];//开通渠道
	char sPkiCertNo[41];//证书序列号
	char sPkiCertDate[9];//证书有效期
	char sStatus[2];//客户状态
	double dLastRisk;//上日风险
	double dCurRisk;//当日风险
	double dNetValue;//单位净值
	double dPosiTotal;//持仓总价值
	double dMarginRate;//保证金占比
	double dContractRate;//合约轧差占比
}S_MDB_CUSTINFO;
typedef struct 
{
	char sTxAcc[21];//资金账号
	char sCustNo[13];//客户编号
	char sAcctype[2];//账户类型
	char sCustSimpleName[41];//客户简称
	char sCustName[41];//客户简称
	char sSignOrg[13];//开户机构 所属机构  银行
	char sMgrOrg[13];//所属代理机构   代理经纪机构
	char sFutureFlag[2];//期货业务签约标志
	char sAccLevel[3];//账户级别
	char sExchFareModel[14];//交易所费率模板
	char sMemberFareModel[14];//会员费率模板
	unsigned int nSysId;//所属经纪公司
	char sCurrency[2];//币种
	char sFsFeeModel[14];//fs手续费模板
	char sParentTxAcc[21];//父资金账号
}S_MDB_CUST_FUNDACC_INFO;

typedef struct 
{
	int  nFundRowid[20];//资金表ROWID 下标为币种
	char sTxAcc[21];//资金账号
	int  nLockRowid;//在本表中的rowid
	int  nCustRowid;//客户表rowid
	int  nFundAccRowid;//资金账号表rowid
	CBF_Mutex  *mutex;//互斥锁
}S_MDB_TXACC_LOCK;

//客户
class  CTbl_Cust  
{
protected:
	
	CMemTableNew <S_MDB_CUSTINFO>   m_table;
//	CPkeyCharF<13,1>             m_pkey;//客户编号的key
	CPkeyCharV<1>             m_pkey;//客户编号的key
//	CPkeyCharV<2>             m_key_cert;//证件类型，证件号码的KEY
//	CPkeyCharV<1>             m_key_phone;//手机号码的key
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
		if (m_pkey.Find(cust.sCustNo))
		{
			return -1;
		}
// 		if (m_key_phone.Find(cust.sMobile))
// 		{
// 			return -1;
// 		}
// 		if (m_key_cert.Find(cust.sCertType,cust.sCertNo))
// 		{
// 			return -1;
// 		}
		int rid = m_table.Add(cust);
		m_pkey.Add(rid,m_table.m_table[rid].sCustNo);
//		m_key_phone.Add(rid,m_table.m_table[rid].sMobile);
//		m_key_cert.Add(rid,m_table.m_table[rid].sCertType,m_table.m_table[rid].sCertNo);
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

	// 函数名: *SelectByPhone
	// 编程  : 王明松 2017-8-3 10:22:17
	// 返回  : S_CUST_INFO 
	// 参数  : char *phone
	// 描述  : 根据客户手机号查询客户信息
// 	S_MDB_CUSTINFO *SelectByPhone(char *phone)
// 	{
// 		int rid;
// 		CBF_PMutex pp(&m_mutex);
// 		if (!m_key_phone.GetRowId(rid,phone))
// 		{
// 			return NULL;
// 		}
// 		return &(m_table.m_table[rid]);
// 	}

	// 函数名: *SelectByCert
	// 编程  : 王明松 2017-8-3 10:22:41
	// 返回  : S_CUST_INFO 
	// 参数  : char *certtype
	// 参数  : char *certno
	// 描述  : 根据证件类型、证件号码查询客户信息
// 	S_MDB_CUSTINFO *SelectByCert(char *certtype,char *certno)
// 	{
// 		int rid;
// 		CBF_PMutex pp(&m_mutex);
// 		if (!m_key_cert.GetRowId(rid,certtype,certno))
// 		{
// 			return NULL;
// 		}
// 		return &(m_table.m_table[rid]);
// 	}
	S_MDB_CUSTINFO *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
};
//客户资金账号
class CTbl_Cust_TX_Acc
{
protected:
	
	CMemTableNew <S_MDB_CUST_FUNDACC_INFO>   m_table;
// 	CPkeyCharF<21,1>               m_pkey;//资金账号
// 	CIndexCharF<13,1>            m_index_accorg;//所属机构索引
// 	CIndexCharF<13,1>            m_index_mgrorg;//代理机构索引
// 	CIndexCharF<21,1>            m_index_parent;//父资金账号索引
// 	CIndexCharF<13,1>            m_index_custno;//客户号索引
	CPkeyCharV<1>               m_pkey;//资金账号
	CIndexVarChar<1>            m_index_accorg;//所属机构索引
	CIndexVarChar<1>            m_index_mgrorg;//代理机构索引
	CIndexVarChar<1>            m_index_parent;//父资金账号索引
	CIndexVarChar<1>            m_index_custno;//客户号索引
	CBF_Mutex                   m_mutex;//互斥锁
public:
	CTbl_Cust_TX_Acc()
	{
		
	}
	virtual ~CTbl_Cust_TX_Acc()
	{
		
	}
	void Clear()
	{
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
	int Insert(S_MDB_CUST_FUNDACC_INFO custacc)  
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
	int Update(S_MDB_CUST_FUNDACC_INFO custacc)  
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		if (m_pkey.GetRowId(rid,custacc.sTxAcc))
		{
			strcpy(m_table.m_table[rid].sFsFeeModel,custacc.sFsFeeModel);
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
	S_MDB_CUST_FUNDACC_INFO *SelectByAcc(char *txacc)
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
	bool SelectAll(std::vector<S_MDB_CUST_FUNDACC_INFO *> &acclist)
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
	bool SelectByAccOrg(char *org,std::vector<S_MDB_CUST_FUNDACC_INFO *> &acclist)
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
	bool SelectByCust(char *custno,std::vector<S_MDB_CUST_FUNDACC_INFO *> &acclist)
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
	bool SelectByMgrOrg(char *org,std::vector<S_MDB_CUST_FUNDACC_INFO *> &acclist)
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
	S_MDB_CUST_FUNDACC_INFO *At(int rowid)
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
//	CPkeyCharF<21,1>               m_pkey;//资金账号
	CPkeyCharV<1>               m_pkey;//资金账号
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
	CTbl_Cust_TX_Acc  m_pCustFundAcc;//资金账号信息表
	
};

#endif // !defined(AFX_DB_CUSTOM_H__F4B3ABE2_EC4D_4E00_AF69_0AA5BB64790F__INCLUDED_)
