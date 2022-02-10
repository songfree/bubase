// Db_BaseInfo.h: interface for the CDb_BaseInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DB_BASEINFO_H__48668C3F_4A61_47F6_9292_CE6DD88F582B__INCLUDED_)
#define AFX_DB_BASEINFO_H__48668C3F_4A61_47F6_9292_CE6DD88F582B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//数据库  基本信息   合约 品种 交易所 市场等

#include "CodeTable.h"

typedef struct 
{
	unsigned int nContractId;//合约编号
	char   sContractCode[21];//合约代码
	char   sContractName[41];//合约名称
	char   sExcode[2];//交易所代码
	char   sProd_type[2];//品种类型
	char   sProd_code[21];//交割品、标的合约代码
	unsigned  int    nProdId;//标的合约编号 品种编号
	double     dMinChgPrice;//最小变动价位
	char    sIsT0[2];//是否T+0
	char   sIsMargin[2];//是否保证金模式
	double  dMarginRate;//保证证率
	char   sIsUpDownLimit[2];//是否涨跌停限制
	double  dUpRate;//涨停率
	double  dDownRate;//跌停率
	char   sActiveFlag[2];//活跌标志
	char   sStatus[2];//合约状态
	double  dRefPrice;//参考价
	double  dSellGetRate;//卖方收款比例
	char   sDeliMode[2];//交收模式
	unsigned int nMaxHand;//最大申报手数
	unsigned int nMinHand;//最小申报手数
	double   dTxUnit;//交易单位 合约乘数
	char    sOrderUnit[2];//报价单位
	char    sOrderCmd[13];//交易所支持报单指令   限价 市价 
	// 		‘0’	ORDER_TYPE_LIMIT	限价
	// 		‘1’	ORDER_TYPE_MARKET	市价
	// 		‘2’	ORDER_TYPE_MS_LOSS	市价止损
	// 		‘3’	ORDER_TYPE_MS_PROFIT	市价止盈
	// 		‘4’	ORDER_TYPE_LS_LOSS	限价止损
	// 		‘5’	ORDER_TYPE_LS_PROFIT	限价止盈
	// 		‘6’	ORDER_TYPE_M2L	市价转限价
	// 		‘7’	ORDER_TYPE_LEVEL1_M	1档市价
	// 		‘8’	ORDER_TYPE_LEVEL5_M	5档市价
	char    sDoneAttr[101];//交易所支持报单属性
	char    sDueDate[9];//到期日
	char    sLastDate[9];//最后交易日
	unsigned int  nDeliDays;//交割间隔天数
	char    sIsBigMargin[2];//持仓处理模式 '0'双边 '1'单边 外盘
	char    sIsOptionCall[2];//是否期权看涨
	int     nVariety;//品种    
	char    sCurrency[2];//币种
}S_MDB_CONTRACT_INFO;

typedef struct 
{
	unsigned int nVarietyId;//品种编号
	char   sVarietyCode[21];//品种代码
	char   sVarietyName[41];//品种名称
	char sExcode[2];//交易所代码
	char sProd_type[2];//品种类型
	char sCurrency[2];//币种
	double     dMinChgPrice;//最小变动价位
	unsigned int nMaxHand;//最大申报手数
	unsigned int nMinHand;//最小申报手数
	double   dTxUnit;//交易单位 合约乘数
	char sOrderUnit[2];//报价单位
	unsigned int  nDeliDays;//交割间隔天数
	char  sBailMode[2];//保证金收取方式
}S_MDB_VARIETY_INFO;



typedef struct
{
	char sExCode[2];//交易所代码
	char sExName[41];//交易所名称
	char sMarket[2];//所属市场
	char sTradeDate[9];//交易日期
	char sIsSeatTrade[2];//是否席位交易
	char sCommPara[201];//通讯参数
	char sClearDate[9];//清算日期
	char sClearStatus[2];//清算状态 
	char sOnlyChgDate[2];//是否仅切换交易日
}S_MDB_EXCH_INFO;

typedef struct  
{
	char sMarket[2];//市场
	char sMarketName[41];//市场名称
	char sTradeDate[9];//交易日期
	char sClearDate[9];//清算日期
	char sClearStatus[2];//清算状态
	char sOnlyChgDate[2];//是否仅切换交易日
}S_MDB_MARKET_INFO;

//交易员对应客户
typedef struct 
{
	char sOperCode[17];//交易员代码
	char sTxAcc[21];   //资金账号
}S_MDB_OPER_CUST_INFO;

typedef struct 
{
	char sFinaProdCode[13];//产品编号
	char sFinaProdName[41];//产品名称
	char sLinkMan[21];//联系人
	char sPhone[21];//电话
	char sAddr[61];//地址
	char sOrgCode[13];//所属机构
	unsigned int nPublishUit;//发行份额
}S_MDB_FINA_PROD_INFO;

//产品对应资金账号
typedef struct
{
	char  sProdNo[13];//产品编号
	char  sCustNo[13];//客户号
	char  sTxAcc[21];//资金账号
}S_MDB_PROD_CUST;

//操作员对应产品
typedef struct
{
	char  sOper[17];//操作员
	char  sProdNo[13];//产品编号
}S_MDB_OPER_PROD;

//冷门合约禁开设置
typedef struct
{
	char  sTxAcc[21];//资金账号
	unsigned int nContractNo;//合约编号
	double   dForceRate;//禁合幅度
}S_MDB_CONTRACT_LIMIT;

//订阅表
// typedef struct  
// {
// 	char sOper[17];//操作员
// 	char sMsgType[2];//信息类型
// }S_MDB_SUBSCRIBE;

typedef struct 
{
	char   sClearDate[9];//结算日期
	unsigned int    nContractNo;//合约编号
	char   sContractCode[21];//合约代码
	double dSettlePrice;//结算价
	int    nHoldPosi;//持仓量
	double dClosePrice;//收盘价
	double dPriceChg;//价格波动
	double dSIMPLE_RETURN;//算数收益率
	double dLOG_RETURN;//对数收益率
	double dHighPrice;//最高价
	double dLowPrice;//最低价
	double dOpenPrice;//开盘价
}S_MDB_CONTRACT_SETTLE_PRICE;

//节假日配置表
typedef struct
{
	char sSpecDate[9];//日期
	char sMarket[2];//市场
	char sExchCode[2];//交易所
	char sDateType[2];//日期类型
}S_MDB_SPECDATE;


typedef struct
{
	int nContractNo;//期权合约编号
	char sContractCode[21];//期权合约代码
	char sSettleDate[9];//结算日期
	double dSettlePrice;//结算价
	double dHvDes;//标的合约的波动率
	double dPrice;//理论价格
	double dDelta;//delta
	double dGamma;
	double dVega;
	double dTheta;
	double dRho;
	double dBs;//bs隐含波动率
	double dGarch;
}S_MDB_OPTION_CONTRACT_PRICE_MODEL;

//合约表的品种类型+标的合约的索引
class Index_Contract_type_desno : public CIndex_Field_Base
{
public:
	char sProd_type;//品种类型
	unsigned  int    nProdId;//标的合约编号
	
	
	virtual bool operator<(const Index_Contract_type_desno& field) const
	{
		int ret;
		if (sProd_type < field.sProd_type)
		{
			return true;
		}
		else if (sProd_type == field.sProd_type)
		{
			if  (nProdId < field.nProdId)
			{
				return true;
			}
			return false;
		}
		else
		{
			return false;
		}
	}
};

//合约表
class CTbl_Contract
{
protected:
	
	CMemTableNew <S_MDB_CONTRACT_INFO>  m_table;
//	CPkeyCharF<21,1>               m_keyCode;//合约代码
	CPkeyCharV<1>               m_keyCode;//合约代码
	CPkeyUInt<1>                 m_keyNo;//合约编号
//	CIndexCharF<2,1>            m_index_ex;//索引-交易所
//	CIndexCharF<21,1>            m_index_destcode;//索引-标的合约代码
//	CIndexCharF<2,1>            m_index_type;//索引-品种类别 期货 现货 期权 股票

	CIndexVarChar<1>            m_index_ex;//索引-交易所
	CIndexVarChar<1>            m_index_destcode;//索引-标的合约代码
	CIndexVarChar<1>            m_index_type;//索引-品种类别 期货 现货 期权 股票
	CIndexUInt<1>               m_index_destno;//索引-标的合约编号
	
	CIndexField<Index_Contract_type_desno> m_index_type_destno;//索引 品种+标的合约编号
	CBF_Mutex                   m_mutex;//互斥锁
public:
	CTbl_Contract()
	{
		
	}
	virtual ~CTbl_Contract()
	{
		
	}
	void Clear()
	{
		m_keyCode.Clear();
		m_keyNo.Clear();
		m_index_ex.Clear();
		m_index_destcode.Clear();
		m_index_type.Clear();
		m_index_destno.Clear();
		m_index_type_destno.Clear();
		m_table.Clear();
	}
	int Insert(S_MDB_CONTRACT_INFO contract)
	{
		if (m_keyCode.Find(contract.sContractCode))
		{
			return -1;
		}
		if (m_keyNo.Find(contract.nContractId))
		{
			return -1;
		}
		int rid = m_table.Add(contract);
		m_keyCode.Add(rid,m_table.m_table[rid].sContractCode);
		m_keyNo.Add(rid,m_table.m_table[rid].nContractId);
		m_index_ex.Add(rid,m_table.m_table[rid].sExcode);
		m_index_destno.Add(rid,m_table.m_table[rid].nProdId);
		m_index_destcode.Add(rid,m_table.m_table[rid].sProd_code);
		m_index_type.Add(rid,m_table.m_table[rid].sProd_type);
		Index_Contract_type_desno aa;
		aa.m_nRowId = rid;
		aa.nProdId = contract.nProdId;
		aa.sProd_type = contract.sProd_type[0];
		m_index_type_destno.Add(aa);
		return rid;
	}
	S_MDB_CONTRACT_INFO *Select(unsigned int contractno)
	{
		int rid;
		CBF_PMutex pp(&m_mutex);
		if (!m_keyNo.Select(rid,contractno))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}
	S_MDB_CONTRACT_INFO *Select(char *contractcode)
	{
		int rid;
		CBF_PMutex pp(&m_mutex);
		if (!m_keyCode.Select(rid,contractcode))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}
	
	// 函数名: SelectByExch
	// 编程  : 王明松 2017-8-4 14:58:58
	// 返回  : bool 
	// 参数  : char *exch_code  NULL或为""表示取所有
	// 参数  : std::vector<S_MDB_CONTRACT_INFO *> &varlist
	// 描述  : 取得交易所下面的所有合约
	bool SelectByExch(char *exch_code,std::vector<S_MDB_CONTRACT_INFO *> &varlist)
	{
		int rid;
		bool bret;
		CBF_PMutex pp(&m_mutex);
		if (exch_code == NULL || strlen(exch_code)<1)
		{
			bret = m_index_ex.First(rid);
			while (bret)
			{
				varlist.push_back(&(m_table.m_table[rid]));
				bret = m_index_ex.Next(rid);
			}
			return true;
		}
		bret = m_index_ex.Find(rid,exch_code);
		if (!bret)
		{
			return false;
		}
		while (bret)
		{
			varlist.push_back(&(m_table.m_table[rid]));
			bret = m_index_ex.Next(rid);
		}
		return true;
	}
	bool SelectByType(char *type,std::vector<S_MDB_CONTRACT_INFO *> &varlist)
	{
		CBF_PMutex pp(&m_mutex);
		std::vector<int> iset;
		if (!m_index_type.Select(iset,type))
		{
			return false;
		}
		for (int i=0; i< iset.size(); i++)
		{
			varlist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
	
	}

	// 函数名: SelectByDestno
	// 编程  : 王明松 2018-1-30 14:15:44
	// 返回  : bool 
	// 参数  : unsigned int destno 标的合约编号
	// 参数  : std::vector<S_MDB_CONTRACT_INFO *> &varlist
	// 描述  : 根据标的合约编号查找合约
	bool SelectByDestno(unsigned int destno,std::vector<S_MDB_CONTRACT_INFO *> &varlist)
	{
		CBF_PMutex pp(&m_mutex);
		std::vector<int> iset;
		if (!m_index_destno.Select(iset,destno))
		{
			return false;
		}
		for (int i=0; i< iset.size(); i++)
		{
			varlist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
		
	}

	// 函数名: SelectByDestCode
	// 编程  : 王明松 2018-1-30 14:15:02
	// 返回  : bool 
	// 参数  : char *destcode 标的合约代码
	// 参数  : std::vector<S_MDB_CONTRACT_INFO *> &varlist
	// 描述  : 根据标的合约代码查找合约
	bool SelectByDestCode(char *destcode,std::vector<S_MDB_CONTRACT_INFO *> &varlist)
	{
		CBF_PMutex pp(&m_mutex);
		std::vector<int> iset;
		if (!m_index_destcode.Select(iset,destcode))
		{
			return false;
		}
		for (int i=0; i< iset.size(); i++)
		{
			varlist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
		
	}
	// 函数名: SelectByTypeDestno
	// 编程  : 王明松 2018-1-30 14:36:55
	// 返回  : bool 
	// 参数  : char *type 合约类型
	// 参数  : unsigned int destno 标的合约
	// 参数  : std::vector<S_MDB_CONTRACT_INFO *> &varlist
	// 描述  : 根据合约类型及标的合约找合约
	bool SelectByTypeDestno(char *type,unsigned int destno,std::vector<S_MDB_CONTRACT_INFO *> &varlist)
	{
		CBF_PMutex pp(&m_mutex);
		Index_Contract_type_desno aa;
		aa.nProdId = destno;
		aa.sProd_type = type[0];
		bool bRet =m_index_type_destno.Find(aa);
		if (!bRet)
		{
			return false;
		}
		while (bRet)
		{
			varlist.push_back(&(m_table.m_table[aa.m_nRowId]));
			bRet =m_index_type_destno.Next(aa);
		}
		return true;
		
	}
	S_MDB_CONTRACT_INFO *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
	// 函数名: Delete
	// 编程  : 王明松 2019-5-16 10:48:53
	// 返回  : int 
	// 参数  : char *exchcode
	// 描述  : 删除交易日期及交易所代码的报单
	int Delete(char *exchcode)
	{
		CBF_PMutex pp(&m_mutex);
		CInt iset;
		if (!m_index_ex.Select(iset,exchcode))
		{
			return 0;
		}
		m_index_ex.Delete(iset);
		m_index_destno.Delete(iset);
		m_index_destcode.Delete(iset);
		m_index_type.Delete(iset);
		
		Index_Contract_type_desno aa;
		int rid;
		bool bret = iset.First(rid);
		while (bret)
		{
			m_keyCode.Delete(m_table.m_table[rid].sContractCode);
			m_keyNo.Delete(m_table.m_table[rid].nContractId);
			aa.m_nRowId = rid;
			aa.nProdId = m_table.m_table[rid].nProdId;
			aa.sProd_type = m_table.m_table[rid].sProd_type[0];
			m_index_type_destno.Delete(aa);
			m_table.Delete(rid);
			bret = iset.Next(rid);
		}
		return iset.Size();
	}
};

//交易所信息表
class CTbl_Exch_Info
{
protected:
	
	CMemTableNew <S_MDB_EXCH_INFO>   m_table;
//	CPkeyCharF<2,1>               m_pkey;//交易所代码
//	CIndexCharF<2,1>            m_index_market;//索引-市场
	CPkeyCharV<1>               m_pkey;//交易所代码
	CIndexVarChar<1>            m_index_market;//索引-市场
	CBF_Mutex                   m_mutex;//互斥锁
public:
	CTbl_Exch_Info()
	{
		
	}
	virtual ~CTbl_Exch_Info()
	{
		
	}
	int Insert(S_MDB_EXCH_INFO exinfo)
	{
		if (m_pkey.Find(exinfo.sExCode))
		{
			return -1;
		}
		int rid= m_table.Add(exinfo);
		m_pkey.Add(rid,m_table.m_table[rid].sExCode);
		m_index_market.Add(rid,m_table.m_table[rid].sMarket);
		return rid;
	}
	S_MDB_EXCH_INFO *Select(char *exchcode)
	{
		int rid;
		CBF_PMutex pp(&m_mutex);
		if (!m_pkey.GetRowId(rid,exchcode))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}
	bool Select(std::vector<S_MDB_EXCH_INFO *> &exchlist)
	{
		int rid;
		CBF_PMutex pp(&m_mutex);
		bool bret =m_pkey.First(rid);
		if (!bret)
		{
			return false;
		}
		while (bret)
		{
			exchlist.push_back(&(m_table.m_table[rid]));
			bret = m_pkey.Next(rid);
		}
		return true;
	}
	bool UpdateTradeDate(char *exchcode,char *curdate,char *nextdate,char onlychgdate)
	{
		int rid;
		CBF_PMutex pp(&m_mutex);
		if (!m_pkey.GetRowId(rid,exchcode))
		{
			return false;
		}
		if (onlychgdate == BOOL_TRUE) //仅日切
		{
			if (strcmp(m_table.m_table[rid].sTradeDate,curdate) !=0)
			{
				return false;
			}
		}
		else
		{
			if (m_table.m_table[rid].sOnlyChgDate[0] == BOOL_TRUE) //前面已做仅日切
			{
				if (strcmp(m_table.m_table[rid].sTradeDate,nextdate) !=0)
				{
					return false;
				}
			}
			else
			{
				if (strcmp(m_table.m_table[rid].sTradeDate,curdate) !=0)
				{
					return false;
				}
			}
		}
	
		strcpy(m_table.m_table[rid].sTradeDate,nextdate);
		m_table.m_table[rid].sOnlyChgDate[0] = onlychgdate;
		if (onlychgdate == BOOL_TRUE)
		{
			m_table.m_table[rid].sClearStatus[0] = CLEAR_STATUS_DATEEND;//将状态置为日切完成
		}
		else
		{
			m_table.m_table[rid].sClearStatus[0] = CLEAR_STATUS_CHG;//将状态置为正在日切
		}
		
		return true;
	}
	bool SelectByMarket(char *market,std::vector<S_MDB_EXCH_INFO *> &reslist)
	{
		CBF_PMutex pp(&m_mutex);
		std::vector<int> iset;
		if (!m_index_market.Select(iset,market))
		{
			return false;
		}
		for (int i=0; i< iset.size(); i++)
		{
			reslist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
		
	}
	S_MDB_EXCH_INFO *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
	
};
//市场信息表
class CTbl_Market
{
protected:
	
	CMemTableNew <S_MDB_MARKET_INFO>   m_table;
	//CPkeyCharF<2,1>               m_pkey;//市场代码
	CPkeyCharV<1>               m_pkey;//市场代码
	CBF_Mutex                   m_mutex;//互斥锁
public:
	CTbl_Market()
	{
		
	}
	virtual ~CTbl_Market()
	{
		
	}
	int Insert(S_MDB_MARKET_INFO info)
	{
		if (m_pkey.Find(info.sMarket))
		{
			return -1;
		}
		int rid= m_table.Add(info);
		m_pkey.Add(rid,m_table.m_table[rid].sMarket);
		return rid;
	}
	S_MDB_MARKET_INFO *Select(char *market)
	{
		int rid;
		CBF_PMutex pp(&m_mutex);
		if (!m_pkey.GetRowId(rid,market))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}
	bool Select(std::vector<S_MDB_MARKET_INFO *> &marketlist)
	{
		int rid;
		CBF_PMutex pp(&m_mutex);
		bool bret =m_pkey.First(rid);
		if (!bret)
		{
			return false;
		}
		while (bret)
		{
			marketlist.push_back(&(m_table.m_table[rid]));
			bret = m_pkey.Next(rid);
		}
		return true;
	}
	bool UpdateTradeDate(char *market,char *curdate,char *nextdate,char onlychgdate)
	{
		int rid;
		CBF_PMutex pp(&m_mutex);
		if (!m_pkey.GetRowId(rid,market))
		{
			return false;
		}
		if (onlychgdate == BOOL_TRUE) //仅日切
		{
			if (strcmp(m_table.m_table[rid].sTradeDate,curdate) !=0)
			{
				return false;
			}
		}
		else
		{
			if (m_table.m_table[rid].sOnlyChgDate[0] == BOOL_TRUE) //前面已做仅日切
			{
				if (strcmp(m_table.m_table[rid].sTradeDate,nextdate) !=0)
				{
					return false;
				}
			}
			else
			{
				if (strcmp(m_table.m_table[rid].sTradeDate,curdate) !=0)
				{
					return false;
				}
			}
		}
		
		strcpy(m_table.m_table[rid].sTradeDate,nextdate);
		m_table.m_table[rid].sOnlyChgDate[0] = onlychgdate;
		if (onlychgdate == BOOL_TRUE)
		{
			m_table.m_table[rid].sClearStatus[0] = CLEAR_STATUS_DATEEND;//将状态置为日切完成
		}
		else
		{
			m_table.m_table[rid].sClearStatus[0] = CLEAR_STATUS_CHG;//将状态置为正在日切
		}
		
		return true;
	}
	S_MDB_MARKET_INFO *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
	
};

//品种表
class CTbl_Variety
{
protected:
	
	CMemTableNew <S_MDB_VARIETY_INFO>  m_table;
// 	CPkeyCharF<21,1>               m_keyCode;//品种代码
// 	CIndexCharF<2,1>            m_index_ex;//索引-交易所
// 	CIndexCharF<2,1>            m_index_type;//索引-品种类别 期货 现货 期权 股票
	CPkeyCharV<1>               m_keyCode;//品种代码
	CIndexVarChar<1>            m_index_ex;//索引-交易所
	CIndexVarChar<1>            m_index_type;//索引-品种类别 期货 现货 期权 股票
	CPkeyUInt<1>                 m_keyNo;//品种编号
	
	CBF_Mutex                   m_mutex;//互斥锁
public:
	CTbl_Variety()
	{
		
	}
	virtual ~CTbl_Variety()
	{
		
	}
	int Insert(S_MDB_VARIETY_INFO info)
	{
		if (m_keyCode.Find(info.sVarietyCode))
		{
			return -1;
		}
		if (m_keyNo.Find(info.nVarietyId))
		{
			return -1;
		}
		int rid = m_table.Add(info);
		m_keyCode.Add(rid,m_table.m_table[rid].sVarietyCode);
		m_keyNo.Add(rid,m_table.m_table[rid].nVarietyId);
		m_index_ex.Add(rid,m_table.m_table[rid].sExcode);
		m_index_type.Add(rid,m_table.m_table[rid].sProd_type);
		return rid;
	}
	S_MDB_VARIETY_INFO *Select(unsigned int varietno)
	{
		int rid;
		CBF_PMutex pp(&m_mutex);
		if (!m_keyNo.Select(rid,varietno))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}
	S_MDB_VARIETY_INFO *Select(char *varietycode)
	{
		int rid;
		CBF_PMutex pp(&m_mutex);
		if (!m_keyCode.Select(rid,varietycode))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}
	
	// 函数名: SelectByExch
	// 编程  : 王明松 2017-8-4 14:58:58
	// 返回  : bool 
	// 参数  : char *exch_code  NULL或为""表示取所有
	// 参数  : std::vector<S_MDB_VARIETY_INFO *> &varlist
	// 描述  : 取得交易所下面的所有合约
	bool SelectByExch(char *exch_code,std::vector<S_MDB_VARIETY_INFO *> &varlist)
	{
		int rid;
		bool bret;
		CBF_PMutex pp(&m_mutex);
		if (exch_code == NULL || strlen(exch_code)<1)
		{
			bret = m_index_ex.First(rid);
			while (bret)
			{
				varlist.push_back(&(m_table.m_table[rid]));
				bret = m_index_ex.Next(rid);
			}
			return true;
		}
		bret = m_index_ex.Find(rid,exch_code);
		if (!bret)
		{
			return false;
		}
		while (bret)
		{
			varlist.push_back(&(m_table.m_table[rid]));
			bret = m_index_ex.Next(rid);
		}
		return true;
	}
	bool SelectByType(char *type,std::vector<S_MDB_VARIETY_INFO *> &varlist)
	{
		CBF_PMutex pp(&m_mutex);
		std::vector<int> iset;
		if (!m_index_type.Select(iset,type))
		{
			return false;
		}
		for (int i=0; i< iset.size(); i++)
		{
			varlist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
	
	}
	S_MDB_VARIETY_INFO *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
};

//操作员客户对应表
class CTbl_Oper_TxAcc
{
protected:
	
	CMemTableNew <S_MDB_OPER_CUST_INFO>      m_table;
// 	CPkeyCharF<21,2>               m_pkey;//操作员-客户
// 	CIndexCharF<17,1>            m_index_oper;//操作员的索引
	CPkeyCharV<2>               m_pkey;//操作员-客户
	CIndexVarChar<1>            m_index_oper;//操作员的索引
	CBF_Mutex                   m_mutex;//互斥锁
public:
	CTbl_Oper_TxAcc()
	{
		
	}
	virtual ~CTbl_Oper_TxAcc()
	{
		
	}
	int Insert(S_MDB_OPER_CUST_INFO info)
	{
		if (m_pkey.Find(info.sOperCode,info.sTxAcc))
		{
			return -1;
		}
		int rid = m_table.Add(info);
		m_pkey.Add(rid,m_table.m_table[rid].sOperCode,m_table.m_table[rid].sTxAcc);
		m_index_oper.Add(rid,m_table.m_table[rid].sOperCode);
		return rid;
	}
	
	// 函数名: Select
	// 编程  : 王明松 2017-8-4 13:52:22
	// 返回  : bool 
	// 参数  : char *opercode
	// 参数  : std::vector<S_MDB_OPER_CUST_INFO *> &oclist
	// 描述  : 根据操作员查询操作员所带客户
	bool Select(char *opercode,std::vector<S_MDB_OPER_CUST_INFO *> &oclist)
	{
		int rid;
		CBF_PMutex pp(&m_mutex);
		if (!m_index_oper.Find(rid,opercode))
		{
			return false;
		}
		oclist.push_back(&(m_table.m_table[rid]));
		bool bret = m_index_oper.Next(rid);
		while (bret)
		{
			oclist.push_back(&(m_table.m_table[rid]));
			bret = m_index_oper.Next(rid);
		}
		return true;
	}
	S_MDB_OPER_CUST_INFO *Select(char *opercode,char *txacc)
	{
		int rid;
		CBF_PMutex pp(&m_mutex);
		if (!m_pkey.Select(rid,opercode,txacc))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}
	S_MDB_OPER_CUST_INFO *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
};

//产品表
class CTbl_Fina_Prod
{
protected:
	
	CMemTableNew <S_MDB_FINA_PROD_INFO>  m_table;
// 	CPkeyCharF<13,1>               m_key;//产品代码
// 	CIndexCharF<13,1>            m_index_org;//索引-所属机构
	CPkeyCharV<1>               m_key;//产品代码
	CIndexVarChar<1>            m_index_org;//索引-所属机构
	CBF_Mutex                   m_mutex;//互斥锁
public:
	CTbl_Fina_Prod()
	{
		
	}
	virtual ~CTbl_Fina_Prod()
	{
		
	}
	int Insert(S_MDB_FINA_PROD_INFO info)
	{
		if (m_key.Find(info.sFinaProdCode))
		{
			return -1;
		}
		
		int rid = m_table.Add(info);
		m_key.Add(rid,m_table.m_table[rid].sFinaProdCode);
		m_index_org.Add(rid,m_table.m_table[rid].sOrgCode);
		return rid;
	}
	
	S_MDB_FINA_PROD_INFO *Select(char *prodcode)
	{
		int rid;
		CBF_PMutex pp(&m_mutex);
		if (!m_key.Select(rid,prodcode))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}
	
	// 函数名: SelectByExch
	// 编程  : 王明松 2017-8-4 14:58:58
	// 返回  : bool 
	// 参数  : char *orgcode  
	// 参数  : std::vector<S_MDB_FINA_PROD_INFO *> &varlist
	// 描述  : 取得机构下面的所有产品
	bool SelectByExch(char *orgcode,std::vector<S_MDB_FINA_PROD_INFO *> &reslist)
	{
		int rid;
		bool bret;
		CBF_PMutex pp(&m_mutex);
		bret = m_index_org.Find(rid,orgcode);
		if (!bret)
		{
			return false;
		}
		while (bret)
		{
			reslist.push_back(&(m_table.m_table[rid]));
			bret = m_index_org.Next(rid);
		}
		return true;
	}

	S_MDB_FINA_PROD_INFO *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
	S_MDB_FINA_PROD_INFO *First()
	{
		return m_table.First();
	}
	S_MDB_FINA_PROD_INFO *Next()
	{
		return m_table.Next();
	}
};

//产品对应资金账号表
class CTbl_Prod_Cust
{
protected:
	
	CMemTableNew <S_MDB_PROD_CUST>  m_table;
// 	CPkeyCharF<21,2>               m_key;//产品代码+资金账号
// 	CIndexCharF<13,1>            m_index;//产品index
	CPkeyCharV<2>               m_key;//产品代码+资金账号
	CIndexVarChar<1>            m_index;//产品index
	CBF_Mutex                   m_mutex;//互斥锁
public:
	CTbl_Prod_Cust()
	{
		
	}
	virtual ~CTbl_Prod_Cust()
	{
		
	}
	int Insert(S_MDB_PROD_CUST info)
	{
		if (m_key.Find(info.sProdNo,info.sTxAcc))
		{
			return -1;
		}
		
		int rid = m_table.Add(info);
		m_key.Add(rid,m_table.m_table[rid].sProdNo,m_table.m_table[rid].sTxAcc);
		m_index.Add(rid,m_table.m_table[rid].sProdNo);
		return rid;
	}
	
	S_MDB_PROD_CUST *Select(char *prodcode,char *txacc)
	{
		int rid;
		CBF_PMutex pp(&m_mutex);
		if (!m_key.Select(rid,prodcode,txacc))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}
	bool SelectByProd(char *prodcode,std::vector<S_MDB_PROD_CUST *> &reslist)
	{
		CBF_PMutex pp(&m_mutex);
		std::vector<int> iset;
		if (!m_index.Select(iset,prodcode))
		{
			return false;
		}
		for (int i=0; i< iset.size(); i++)
		{
			reslist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
	}

	S_MDB_PROD_CUST *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
};

//操作员对应产品
class CTbl_Oper_Prod
{
protected:
	
	CMemTableNew <S_MDB_OPER_PROD>  m_table;
//	CPkeyCharF<17,2>               m_key;//操作员+产品代码
	CPkeyCharV<2>               m_key;//操作员+产品代码
	CBF_Mutex                   m_mutex;//互斥锁
public:
	CTbl_Oper_Prod()
	{
		
	}
	virtual ~CTbl_Oper_Prod()
	{
		
	}
	int Insert(S_MDB_OPER_PROD info)
	{
		if (m_key.Find(info.sOper,info.sProdNo))
		{
			return -1;
		}
		
		int rid = m_table.Add(info);
		m_key.Add(rid,m_table.m_table[rid].sOper,m_table.m_table[rid].sProdNo);
		return rid;
	}
	
	S_MDB_OPER_PROD *Select(char *oper,char *prod)
	{
		int rid;
		CBF_PMutex pp(&m_mutex);
		if (!m_key.Select(rid,oper,prod))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}
	
	
	S_MDB_OPER_PROD *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
};


//冷门合约开仓限制表主键
class CKey_Cust_Contract_Limit : public CIndex_Field_Base
{
public:
	char sTxAcc[21];//资金账号
	unsigned  int  nContractNo;//合约编号
	
	
	virtual bool operator<(const CKey_Cust_Contract_Limit& field) const
	{
		int ret = strcmp(sTxAcc,field.sTxAcc);
		if (ret<0)
		{
			return true;
		}
		else if (ret == 0)
		{
			if  (nContractNo < field.nContractNo)
			{
				return true;
			}
			return false;
		}
		else
		{
			return false;
		}
	}
};
class CTbl_Cust_Contract_Limit
{
protected:
	
	CMemTableNew <S_MDB_CONTRACT_LIMIT>  m_table;
	CKeyField<CKey_Cust_Contract_Limit> m_key;//资金账号+合约编号
	CBF_Mutex                   m_mutex;//互斥锁
public:
	CTbl_Cust_Contract_Limit()
	{
		
	}
	virtual ~CTbl_Cust_Contract_Limit()
	{
		
	}
	int Insert(S_MDB_CONTRACT_LIMIT info)
	{
		CKey_Cust_Contract_Limit aa;
		strcpy(aa.sTxAcc,info.sTxAcc);
		aa.nContractNo = info.nContractNo;
		if (m_key.Find(aa))
		{
			return -1;
		}
		
		int rid = m_table.Add(info);
		aa.m_nRowId = rid;
		m_key.Add(aa);
		return rid;
	}
	
	S_MDB_CONTRACT_LIMIT *Select(char *sTxacc,unsigned int nContractNo)
	{
		int rid;
		CBF_PMutex pp(&m_mutex);
		CKey_Cust_Contract_Limit aa;
		strcpy(aa.sTxAcc,sTxacc);
		aa.nContractNo = nContractNo;
		if (!m_key.Find(aa))
		{
			return NULL;
		}
		return &(m_table.m_table[aa.m_nRowId]);
	}
	
	
	S_MDB_CONTRACT_LIMIT *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
};

//订阅表
// class CTbl_Subscribe
// {
// protected:
// 	
// 	CMemTableNew <S_MDB_SUBSCRIBE>   m_table;
// 	CPkeyCharV<2>               m_pkey;//操作员_信息类型
// 	CBF_Mutex                   m_mutex;//互斥锁
// public:
// 	CTbl_Subscribe()
// 	{
// 		
// 	}
// 	virtual ~CTbl_Subscribe()
// 	{
// 		
// 	}
// 	int Insert(S_MDB_SUBSCRIBE info)
// 	{
// 		if (m_pkey.Find(info.sOper,info.sMsgType))
// 		{
// 			return -1;
// 		}
// 		int rid= m_table.Add(info);
// 		m_pkey.Add(rid,m_table.m_table[rid].sOper,m_table.m_table[rid].sMsgType);
// 		
// 		return rid;
// 	}
// 	S_MDB_SUBSCRIBE *Select(char *oper,char *msgtype)
// 	{
// 		int rid;
// 		CBF_PMutex pp(&m_mutex);
// 		if (!m_pkey.GetRowId(rid,oper,msgtype))
// 		{
// 			return NULL;
// 		}
// 		return &(m_table.m_table[rid]);
// 	}
// 
// 	S_MDB_SUBSCRIBE *At(int rowid)
// 	{
// 		if (rowid<0 || rowid > m_table.AllSize()-1)
// 		{
// 			return NULL;
// 		}
// 		return &(m_table.m_table[rowid]);
// 	}
// 	
// };


//合约结算价表 只保存当前最新的结算价
class CTbl_SettlePrice
{
protected:
	
	CMemTableNew <S_MDB_CONTRACT_SETTLE_PRICE>   m_table;
	CPkeyUInt<1>               m_pkey;//合约编号
//	CPkeyCharF<21,1>               m_pkeycode;//合约代码
	CPkeyCharV<1>               m_pkeycode;//合约代码
	CBF_Mutex                   m_mutex;//互斥锁
public:
	CTbl_SettlePrice()
	{
		
	}
	virtual ~CTbl_SettlePrice()
	{
		
	}
	int Insert(S_MDB_CONTRACT_SETTLE_PRICE info)
	{
		if (m_pkey.Find(info.nContractNo))
		{
			return -1;
		}
		if (m_pkeycode.Find(info.sContractCode))
		{
			return -1;
		}
		int rid= m_table.Add(info);
		m_pkey.Add(rid,m_table.m_table[rid].nContractNo);
		m_pkeycode.Add(rid,m_table.m_table[rid].sContractCode);
		return rid;
	}
	S_MDB_CONTRACT_SETTLE_PRICE *Select(unsigned int nContractNo)
	{
		int rid;
		CBF_PMutex pp(&m_mutex);
		if (!m_pkey.Select(rid,nContractNo))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}
	S_MDB_CONTRACT_SETTLE_PRICE *Select(char *sContractCode)
	{
		int rid;
		CBF_PMutex pp(&m_mutex);
		if (!m_pkeycode.Select(rid,sContractCode))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}
	S_MDB_CONTRACT_SETTLE_PRICE *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
	
};

//日期配置表
class CTbl_SpecDate
{
protected:
	
	CMemTableNew <S_MDB_SPECDATE>   m_table;
// 	CPkeyCharF<9,3>               m_pkey;//市场+交易所+日期
// 	CIndexCharF<2,2>           m_index;//市场+交易所
	CPkeyCharV<3>               m_pkey;//市场+交易所+日期
	CIndexVarChar<2>           m_index;//市场+交易所
	CBF_Mutex                   m_mutex;//互斥锁
public:
	CTbl_SpecDate()
	{
		
	}
	virtual ~CTbl_SpecDate()
	{
		
	}
	int Insert(S_MDB_SPECDATE info)
	{
		if (m_pkey.Find(info.sMarket,info.sExchCode,info.sSpecDate))
		{
			return -1;
		}
		int rid= m_table.Add(info);
		m_pkey.Add(rid,m_table.m_table[rid].sMarket,m_table.m_table[rid].sExchCode,m_table.m_table[rid].sSpecDate);
		m_index.Add(rid,m_table.m_table[rid].sMarket,m_table.m_table[rid].sExchCode);
		return rid;
	}
	S_MDB_SPECDATE *Select(char *sMarket,char *sExchCode,char *sSpecDate)
	{
		int rid;
		CBF_PMutex pp(&m_mutex);
		if (!m_pkey.Select(rid,sMarket,sExchCode,sSpecDate))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}
	bool Select(char *sMarket,char *sExchCode,std::vector<S_MDB_SPECDATE *>&reslist)
	{
		int rid;
		CBF_PMutex pp(&m_mutex);
		std::vector<int>iset;
		if (!m_index.Select(iset,sMarket,sExchCode))
		{
			return false;
		}
		for (int i=0;i<iset.size();i++)
		{
			reslist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
	}
	S_MDB_SPECDATE *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
	
};

//期权价格模型表
class CTbl_Option_Model
{
protected:
	
	CMemTableNew <S_MDB_OPTION_CONTRACT_PRICE_MODEL>   m_table;
	CPkeyUInt<1>               m_pkey;//合约编号
	CBF_Mutex                   m_mutex;//互斥锁
public:
	CTbl_Option_Model()
	{
		
	}
	virtual ~CTbl_Option_Model()
	{
		
	}
	int Insert(S_MDB_OPTION_CONTRACT_PRICE_MODEL info)
	{
		if (m_pkey.Find(info.nContractNo))
		{
			return -1;
		}

		int rid= m_table.Add(info);
		m_pkey.Add(rid,m_table.m_table[rid].nContractNo);
		return rid;
	}
	S_MDB_OPTION_CONTRACT_PRICE_MODEL *Select(unsigned int nContractNo)
	{
		int rid;
		CBF_PMutex pp(&m_mutex);
		if (!m_pkey.Select(rid,nContractNo))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}

	S_MDB_OPTION_CONTRACT_PRICE_MODEL *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
	
};

class CDb_BaseInfo  
{
public:
	CDb_BaseInfo()
	{

	}
	virtual ~CDb_BaseInfo()
	{

	}
	CTbl_Variety    m_pVariety;//品种表
	CTbl_Contract   m_pContract;//合约表
	CTbl_Exch_Info  m_pExchange;//交易所表
	CTbl_Market     m_pMarket;//市场表
	CTbl_Fina_Prod  m_pProd;//产品表
	CTbl_Oper_TxAcc m_pOperTxAcc;//操作员对应资金账号表

	CTbl_Prod_Cust  m_pProdCust;//产品对应资金账号表
	CTbl_Oper_Prod  m_pOperProd;//操作员对应产品
	
	CTbl_Cust_Contract_Limit m_pCustContractLimit;//冷门合约开仓限制表

	CTbl_SettlePrice  m_pSettlePrice;//合约结算价表，只保存最新的
	CTbl_SpecDate     m_pSpecDate;//节假日配置
	CTbl_Option_Model m_pOptionModel;//期权合约价格模型表

};

#endif // !defined(AFX_DB_BASEINFO_H__48668C3F_4A61_47F6_9292_CE6DD88F582B__INCLUDED_)
