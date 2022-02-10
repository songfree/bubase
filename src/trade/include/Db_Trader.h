// Db_Trader.h: interface for the CDb_Trader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DB_TRADER_H__6EECD875_4F24_4FFA_9024_3DEBCD57D140__INCLUDED_)
#define AFX_DB_TRADER_H__6EECD875_4F24_4FFA_9024_3DEBCD57D140__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//数据库 交易  委托、成交、出入金、回报、行情等

//客户在线表
typedef struct
{
	char sCustNo[21];//客户编号
	char sTermType[11];//登录渠道
	char sLoginIp[21];//登录IP
	char sOnlineStatus[2];//在线状态
	char sOnlineTime[18];//登录时间
	int  nDrebId;//总线公共节点号
	int  nDrebPid;//总线私有节点号
	int  nSvrId;//公共服务号
	int  nSvrPid;//私有服务号
	int  nGateIndex; //网关连接序号
	long nTimeStamp;//登录时间戳
}S_MDB_CUST_ONLINE;


typedef struct 
{
	char sTxAcc[21];//资金账号
	char sExCode[2];//交易所代码
	char sSeatNo[7];//席位代码
	char sTradeCode[21];//交易编码
}S_MDB_CUST_TRADECODE;

//交易所交易员登录信息
typedef struct 
{
	char sExchCode[2]; //交易所代码
	char sSeatNo[7];//席位代码
	char sTraderId[17];//交易员代码
	char sLoginStatus[2];//登录状态 0未连接未登录 	1已连接已登录 2已连接未登录
	char sLoginTime[18];//等录时间yymmdd hhmmss
	char sLogoutTime[18];//录出时间yymmdd hhmmss
	char sPara[201]; //通讯参数
}S_MDB_EXCH_TRADER_INFO;


typedef struct 
{
	char sTxAcc[21];//资金账号
	char sCurrency[2];//币种
	double dLast_bal;//上日结存
	double dBalance;//当日结存
	double dAvailFund;//当日可用
	double dGetFund;//当日可提
	double dBaseMargin;//基础保证金
	double dIncomeExpense;//收付款项
	double dFrozeFund;//冻结资金
	double dMargin;//保证金
	double dFee;//费用
	double dReserve;//备付金
	double dSurplus;//盈亏
	double dFloatSurplus;//浮动盈亏
// 	double dInteIntegral;//利息积数
// 	double dPuniIntegral;//罚息积数
	double dIncrInte;//利息
	double dArap;//应收应付
	int    nRisk_level;//风险级别
	double dRisk;//风险度
	char sTradeRight[2];//交易权限
	char   sRiskDesc[81];//风险说明
	char   sRiskFlag[2];//风险标志 '1' 发生风险 0取消风险
	unsigned int nRiskdate;//发生风险日期
	unsigned int nRisktime;//发生风险时间
}S_MDB_CUST_FUND;//资金表


typedef struct 
{
	char   sTxAcc[21];//资金账号
	char   sCurrency[2];//币种
	char   sTime[9];//时间  hhmmss
	int    nRisk_level;//风险级别
	double dRisk;//风险度
}S_MDB_CUST_RISK_TICK;//当日风险表

typedef struct
{
	char sTxAcc[21];//资金账号
	char sCurrency[2];//币种
	char sFundType[2];//资金类型
	char sFundCode[4];//资金编号
	double dFundValue;//值
}S_MDB_CUST_FUND_DETAIL;//资金明细表

typedef struct 
{
	char sTxAcc[21];
	unsigned int  nContractNo;//合约编号
	char sContractCode[21];//合约代码
	char sSeatNo[7];//席位代码
	char sTradeCode[21];//交易编码
	char sShFlag[2];//投保标志
	int  nLastPB;//上日买持 多头持仓
	int  nLastPS;//上日卖持  空头持仓
	int  nCurPB;//当前多仓
	int  nCurPS;//当前空仓
	int  nCurCanUsePB;//当前可用多仓
	int  nCurCanUsePS;//当前可用空仓
	int  nDayPB;//当天开多仓持仓
	int  nDayPS;//当天开空仓持仓
	int  nDAY_COV_PB_Froze;//当天平多仓冻结
	int  nDAY_COV_PB;//当天平多仓
	int  nDAY_COV_PS_Froze;//当天平空仓冻结
	int  nDAY_COV_PS;//当天平空仓
	int  nDAY_DELI_PB_Froze;//当天交收多仓
	int  nDAY_DELI_PB;//当天交收多仓
	int  nDAY_DELI_PS_Froze;//当天交收空仓冻结
	int  nDAY_DELI_PS;//当天交收空仓
	int  nDAY_MIDD_PB;//中立仓多仓
	int  nDAY_MIDD_PS;//中立仓空仓
	double dPB_OAvp;//多仓开仓均价    现货股票开仓成本均价
	double dPS_OAvp;//空仓开仓均价
	double dPB_Avp;//多仓持仓均价         现货股票成本均价(同股票，卖出盈利后成本降低，卖出亏损成本升高)
	double dPS_Avp;//空仓持仓均价
	double dPB_Margin;//多仓持仓保证金    现货股票持仓成本
	double dPS_Margin;//空仓持仓保证金
	double dDAY_PB_SURPLUS;//当日多头盈亏
	double dDAY_PS_SURPLUS;//当日空头盈亏
	double dLAST_SETTLE_PRICE;//上日结算价
	double dSETTLE_PRICE;//结算价
	int    nVarietyId;//品种编号
}S_MDB_CUST_POSITION;

//库存表
typedef struct
{
	char sTxAcc[21]; //交易账号
	char sProdCode[7];//品种代码
	int  nLastBal;//上日余额
	int  nLastCanUse;//上日可用
	int  nCurBal;//当前余额
	int  nCurCanUse;//当前可用
	int  nCurCanGet;//当前可提
	int  nOrderSellQty;//委托卖出数量
	int  nBackSellQty;//回报卖出数量
	int  nBackBuyQty;//回报买入数量
	int  nDayDeposit;//当日存入标重
	double  dDayDepositReal;//当日存入实重
	int    nDayDraw;//当日提出标重
	double dDayDrawReal;//当日提出实重
	double dDiffStorage;//入库溢短总量
	double dUnclearDiffStor;//未清算入库溢短
	double dOutDiff;//出库溢短总量
	int   nLastTakeFroz;//上日提货冻结数量
	int   nDayTakeFroz;//当日提货冻结数量
	int   nImpawnOut;//出质数量
	int   nImpawnIn;//质权数量
	int   nDayImpawnOut;//当日出质数量
	int   nDayImpawnIn;//当日质权数量
	int   nDayInCancel;//质押撤销(质权方)
	int   nDayOutCancel;//质押撤销(出质方)
	int   nLastLongFroz;//上日长期冻结数量
	int   nDayLongFroz;//当日长期冻结数量
	int   nLentQty;//借出数量
	int   nBorrowQty;//借入数量
	int   nDayLent;//当日借出
	int   nDayBorrow;//当日借入
	int   nDayShiftOut;//当日划出
	int   nDayShiftIn;//当日划入
	int   nTotalDeposit;//入库总量
	int   nTotalTake;//提货总量
	int   nStorAccu;//库存积数
}S_MDB_STOR_INFO;


//报单表
typedef struct
{
	char sTxAcc[21];//交易账号
	char sExchCode[2];//交易所代码
	char sSeatNo[7];//席位代码
	char sTxCode[21];//交易编码
	unsigned int  nContractNo;//合约编号
	char  sContractCode[21];//合约代码
	char sClientSerial[19];//客户端流水
	char sLocalSerial[38];//本地报单号
	char sSystemNo[21];//交易所报单号
	int    nOrderNum;//委托数量
	double dOrderPrice;//委托价格
	int    nRemainNum;//剩余数量   成交回报后更改此数量
	char   sEoFlag[2];//开平标志
	char   sBsFlag[2];//买卖标志
	char   sShFlag[2];//投保标志
	char   sTxDate[9]; //交易日期 yyyymmdd
	double dAmt;//金额
	double dFrozAmt;//冻结金额  期货为保证金 现货证券为金额 正数  
	double dExFee;//交易费用   负数减少
	char   sTxType[2];//交易类型
	char   sCmdType[2];//指令类型
	char   sCmdAttr[2];//指令属性
	char   sForceFlag[2];//强平标志
	char   sOrderStatus[2];//委托状态 o 已报入 p 部分成交	c  全部成交	d  已撤销  s  系统撤销	1  正在申报	2  无效 3  部成部撤
	char   sCancelFlag[2];//撤单标志
	char   sAnsTime[17];//应答时间   yyyymmdd hhmmss
	char   sOrderChannel[3];//委托来源，即渠道
	char   sAccOrg[13];//账户所属机构
	char   sTxOrg[13]; //交易网点(机构)
	char   sOrderOper[21];//委托交易员/操作员/客户号
	char   sCancelChannel[3];//撤销来源 渠道
	char   sCancelOper[21];//撤销操作员
	char   sCancelTime[17];//撤销时间 yyyymmdd hhmmss
	char   sOrderRsp[201];//报单响应
	double dFrozUseamt;//报单冻结可用
	double dFrozOutamt;//报单冻结可提
	int    nCancelNum;//撤单数量
	int    nSyncFlag;//同步标志  0 无须同步  1报入同步  2撤单同步  3成交同步 4撤单成交同步 5回报同步
	double dDoneAvp;//成交均价
	int    nVarietyId;//品种编号
}S_MDB_ORDER_INFO;


//成交表
typedef struct
{
	char sDoneNo[21];//成交编号
	char sSystemNo[21];//系统报单号
	char sExchCode[2];//交易所代码
	char sSeatNo[7];//席位代码
	char sTxCode[21];//交易编码
	char sLocalNo[38];//本地报单号
	char sTxAcc[21];//交易账号
	char sDoneDate[9];//成交日期
	char sDoneTime[17];//成交时间
	unsigned int  nContractNo;//合约编号
	char sContractCode[21];//合约代码
	double dDonePrice;//成交价
	int   nDoneNum;//成交数量  (手)
	char   sBsFlag[2];//买卖标志
	char   sShFlag[2];//投保标志
	char   sEoFlag[2];//开平标志
	char   sTxType[2];//交易类型
	char   sCmdType[2];//指令类型
	char   sCmdAttr[2];//成交属性
	double  dAmt;//交易金额           -买入 +卖出   不含手续费
	double  dExFee;//交易所交易费用   负数
	double  dMemberFee;//会员交易费用 负数
	double  dExMargin;//交易所保证金  正数
	double  dMemberMargin;//会员保证金 正数
	double  dFloatSurplus;//浮动盈亏
	double  dCovSurplus;//平仓盈亏
	char    sTxOrg[13];//交易网点
	char    sAccOrg[13];//签约网点
	char    sForceFlag[2];//强平标志
	char    sOrderChannel[3];//委托渠道
	char    sOrderOper[21];//委托操作员、客户
	char    sSystemDate[9];//发生自然日
	double  dUnfrozeAmt;//解冻金额   保证金或金额 负数
	double  dUnfrozeFee;//解冻费用  正数
	int    nSyncFlag;//同步标志  0 无须同步   3成交同步
	int    nVarietyId;
}S_MDB_DONE_INFO;


//出入金表
typedef struct
{
	char sTxDate[9];//交易日期
	char sTxAcc[21];//交易账户
	char sTxSerial[31];//交易流水
	char sBankAcc[24];//银行账号
	char sBankName[41];//银行名称
	char sCurrency[2];//币种
	double  dAmt;//金额
	char  sFundFlag[2];//资金方向
	double dLastAmt;//发生前可用
	double dCurAmt;//发生后可用
}S_MDB_FUND_FLOW;


typedef struct 
{
	unsigned int  nContractNo;//合约编号
	char sContractCode[21];//合约代码
	char sContractName[41];//合约名称
	unsigned int  nDate;//行情日期
	char sTime[18];//行情时间
	unsigned int  nQuoSerial;//行情序号
	double dLastSetllePrice;//昨结算
	double dLastClosePrice;//昨收盘
	double dOpenPrice;//开盘
	double dHighPrice;//最高价
	double dLowPrice;//最低价
	double dNewPrice;//最新价
	double dClosePrice;//收盘价
	double dSettlePrice;//结算价
	double dBuyPrice1;//买价1
	double dBuyPrice2;//买价2
	double dBuyPrice3;//买价3
	double dBuyPrice4;//买价4
	double dBuyPrice5;//买价5
	double dBuyPrice6;//买价6
	double dBuyPrice7;//买价7
	double dBuyPrice8;//买价8
	double dBuyPrice9;//买价9
	double dBuyPrice10;//买价10
	int    nBuy1;//买量1
	int    nBuy2;//买量2
	int    nBuy3;//买量3
	int    nBuy4;//买量4
	int    nBuy5;//买量5
	int    nBuy6;//买量6
	int    nBuy7;//买量7
	int    nBuy8;//买量8
	int    nBuy9;//买量9
	int    nBuy10;//买量10
	double dSellPrice1;//卖价1
	double dSellPrice2;//卖价2
	double dSellPrice3;//卖价3
	double dSellPrice4;//卖价4
	double dSellPrice5;//卖价5
	double dSellPrice6;//卖价6
	double dSellPrice7;//卖价7
	double dSellPrice8;//卖价8
	double dSellPrice9;//卖价9
	double dSellPrice10;//卖价10
	int    nSell1;//卖量1
	int    nSell2;//卖量2
	int    nSell3;//卖量3
	int    nSell4;//卖量4
	int    nSell5;//卖量5
	int    nSell6;//卖量6
	int    nSell7;//卖量7
	int    nSell8;//卖量8
	int    nSell9;//卖量9
	int    nSell10;//卖量10
	unsigned int    nDoneQty;//成交量  手
	double dDoneWeight;//成交重量
	unsigned int    nPosition;//持仓量
	double dUpLimit;//涨停板
	double dDownLimit;//跌停板
	double dUpDown;//涨跌
	double dUpDownRate;//涨跌幅
	double dDownAmt;//成交金额
	double dAvp;//均价
}S_MDB_QUOTATION;

//风险合约
typedef struct 
{
	unsigned int nContractNo;//合约编号
	char   sContractCode[21];//合约代码
	double dNewPrice;//最新价
	double dUpDown;//涨跌
	double dUpDownRate;//涨跌幅
	double dBuyPrice;//买价
	double dSellPrice;//卖价
	double dHighPrice;//最高
	double dLowPrice;//最低
	int    nDoneQty;//成交量
	char   sRiskDesc[81];//风险说明
	char   sRiskFlag[2];//风险标志 '1' 发生风险 0取消风险
	unsigned int nRiskdate;//发生风险日期
	unsigned int nRisktime;//发生风险时间
}S_MDB_RISKCONTRACT_INFO;

//客户持仓风险
typedef struct 
{
	char sTxAcc[21];//交易账户
	unsigned int nContractNo;//合约编号
	char   sContractCode[21];//合约代码
	char   sShFlag[2];//投保标志
	int    nBuyQty;//买持量
	int    nSellQty;//卖持量
	unsigned short  nWarnLevel;//预警级数
	char   sRiskDesc[81];//风险说明
	char   sRiskFlag[2];//风险标志 '1' 发生风险 0取消风险
	unsigned int nRiskdate;//发生风险日期
	unsigned int nRisktime;//发生风险时间
}S_MDB_CUST_POSI_RISK;
typedef struct  
{
	char sOper[17];//操作员
	char sSubcribeType[2];//订阅类型 '0' 风险
}S_MDB_SUBSCRIBE;

typedef struct  
{
	char sDate[9];//计算日期
	char sProdCode[13];//产品编号
	int  nRiskLevel;//风险级别，报警序号
	double dRisk;//风险度
	double dUnitValue;//单位净值
	double dBal;//总权益
	double dMargin;//总保证金
	char sRight[2];//交易权限
	char   sRiskDesc[81];//风险说明
	char   sRiskFlag[2];//风险标志 '1' 发生风险 0取消风险
	unsigned int nRiskdate;//发生风险日期
	unsigned int nRisktime;//发生风险时间
}S_MDB_PROD_FUND_RISK;//产品资金风险表

typedef struct  
{
	char sDate[9];//计算日期
	char sProdCode[13];//产品编号
	int  nRiskLevel;//风险级别，报警序号
	double dMarginRate;//保证金占比
	double dPosiValueRate;//持仓价值比
	double dPosiDiffValueRate;//持仓轧差价值比
	double dT_Bal;//总权益
	double dT_Buy;//总买持仓价值
	double dT_Sell;//总卖持仓价值
	char sRight[2];//交易权限
	char   sRiskDesc[81];//风险说明
	char   sRiskFlag[2];//风险标志 '1' 发生风险 0取消风险
	unsigned int nRiskdate;//发生风险日期
	unsigned int nRisktime;//发生风险时间
}S_MDB_PROD_POSI_RISK;//产品持仓风险表

typedef struct  
{
	char sDate[9];//计算日期
	char sOrgCode[13];//机构代码
	char sRiskType[2];//风险类别 0所属机构  1代理机构
	int  nRiskLevel;//风险级别，报警序号
	double dRisk;//风险度
	double dUnitValue;//单位净值
	double dBal;//总权益
	double dMargin;//总保证金
	char sRight[2];//交易权限
	char   sRiskDesc[81];//风险说明
	char   sRiskFlag[2];//风险标志 '1' 发生风险 0取消风险
	unsigned int nRiskdate;//发生风险日期
	unsigned int nRisktime;//发生风险时间
}S_MDB_ORG_FUND_RISK;//机构资金风险表

typedef struct  
{
	char sDate[9];//计算日期
	char sOrgCode[13];//机构代码
	char sRiskType[2];//风险类别 0所属机构  1代理机构
	int  nRiskLevel;//风险级别，报警序号
	double dMarginRate;//保证金占比
	double dPosiValueRate;//持仓价值比
	double dPosiDiffValueRate;//持仓轧差价值比
	double dT_Bal;//总权益
	double dT_Buy;//总买持仓价值
	double dT_Sell;//总卖持仓价值
	char sRight[2];//交易权限
	char   sRiskDesc[81];//风险说明
	char   sRiskFlag[2];//风险标志 '1' 发生风险 0取消风险
	unsigned int nRiskdate;//发生风险日期
	unsigned int nRisktime;//发生风险时间
}S_MDB_ORG_POSI_RISK;//机构持仓风险表


typedef struct  
{
	char sTxAcc[21];//交易账户
	int nVarietyId;//品种编号
	int      nBuyPosi;//多仓量
	int      nSellPosi;//空仓量
	double   dBuyMargin;    //多仓保证金
	double   dSellMargin;   //空仓保证金
	int      nMarginFlag;   //保证金大边净边方向 1多仓 2空仓
	int      nBuyQty;//报单剩余买量
	int      nSellQty;//报单剩余卖量
	double   dBuyFroze;     //报单买冻结
	double   dSellFroze;    //报单卖冻结
	int      nFrozeFlag;    //冻结方向    1多仓 2空仓
	double      dFrozeAmt;     //报单冻结金额   冻结+持有，得到方向，再减去持仓金额，即得到该冻结金额
	char     sBailMode;     //保证金模式  BAIL_MODE_BOTH等 净边大边
}S_MDB_BIG_MARGIN;//大边保证金表

//持仓轧差表
typedef struct 
{
	char sTxDate[9];//交易日期
	char sTxAcc[21];//资金账号
	char sSeatNo[7];//席位号
	char sShFlag[2];//投保标志
	char sBsFlag[2];//买卖标志
	int  nContractNo;//合约编号
	char sContractCode[21];//合约代码
	int  nDiffQty;//轧差数量
	char sSerial[31];//流水号
}S_MDB_DIFF_POSI; 

//客户在线
class CTbl_Cust_Online
{
protected:
	
	CMemTableNew <S_MDB_CUST_ONLINE>      m_table;
// 	CPkeyCharF<21,2>               m_pkey;//客户+渠道
// 	CIndexCharF<2,1>            m_index_loginstatus; //按登录状态的索引
// 	CIndexCharF<21,1>            m_index_cust; //客户
	CPkeyCharV<2>               m_pkey;//客户+渠道
	CIndexVarChar<1>            m_index_loginstatus; //按登录状态的索引
	CIndexVarChar<1>            m_index_cust; //客户
	CBF_Mutex                   m_mutex;//互斥锁
public:
	CTbl_Cust_Online()
	{
		
	}
	virtual ~CTbl_Cust_Online()
	{
		
	}
	void Clear()
	{
		m_pkey.Clear();
		m_index_cust.Clear();
		m_index_loginstatus.Clear();
		m_table.Clear();
	}
	int Insert(S_MDB_CUST_ONLINE online)
	{
		if (m_pkey.Find(online.sCustNo,online.sTermType))
		{
			return -1;
		}
		int rid = m_table.Add(online);
		m_index_loginstatus.Add(rid,m_table.m_table[rid].sCustNo,m_table.m_table[rid].sTermType);
		m_index_cust.Add(rid,m_table.m_table[rid].sCustNo);
		return rid;
	}
	S_MDB_CUST_ONLINE *Select(char *cust,char *channel)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		if (!m_pkey.GetRowId(rid,cust,channel))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}

	// 函数名: Select
	// 编程  : 王明松 2018-1-29 15:25:03
	// 返回  : bool 
	// 参数  : char *oper
	// 参数  : std::vector<S_MDB_OPER_ONLINE *> &onlinelist
	// 描述  : 根据客户号取出客户在线信息
	bool Select(char *cust,std::vector<S_MDB_CUST_ONLINE *> &onlinelist)
	{
		CBF_PMutex pp(&m_mutex);
		std::vector<int> iset;
		if (!m_index_cust.Select(iset,cust))
		{
			return false;
		}
		for (int i=0; i<iset.size(); i++)
		{
			onlinelist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
	}
	// 函数名: SelectByStatus
	// 编程  : 王明松 2018-1-29 15:25:03
	// 返回  : bool 
	// 参数  : char *status 状态
	// 参数  : std::vector<S_MDB_OPER_ONLINE *> &onlinelist
	// 描述  : 根据操作员取出操作员在线信息
	bool SelectByStatus(char *status,std::vector<S_MDB_CUST_ONLINE *> &onlinelist)
	{
		CBF_PMutex pp(&m_mutex);
		std::vector<int> iset;
		if (!m_index_loginstatus.Select(iset,status))
		{
			return false;
		}
		for (int i=0; i<iset.size(); i++)
		{
			onlinelist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
	}

	// 函数名: Update
	// 编程  : 王明松 2018-1-29 15:41:53
	// 返回  : int  -1出错 >0 rid 
	// 参数  : S_MDB_OPER_ONLINE online
	// 描述  : 更新在线表，没有增加，有更新。有加锁
	int Update(S_MDB_CUST_ONLINE online)
	{
		int rid;
		CBF_PMutex pp(&m_mutex);
		if (m_pkey.GetRowId(rid,online.sCustNo,online.sTermType))
		{
			//更新
			if (strcmp(online.sOnlineStatus,m_table.m_table[rid].sOnlineStatus)!=0) //状态不同
			{
				m_table.m_table[rid].nDrebId = online.nDrebId;
				m_table.m_table[rid].nDrebPid = online.nDrebPid;
				m_table.m_table[rid].nGateIndex = online.nGateIndex;
				m_table.m_table[rid].nSvrId = online.nSvrId;
				m_table.m_table[rid].nSvrPid = online.nSvrPid;
				m_table.m_table[rid].nTimeStamp = online.nTimeStamp;
				strcpy(m_table.m_table[rid].sLoginIp,online.sLoginIp);
				strcpy(m_table.m_table[rid].sOnlineTime,online.sOnlineTime);
				strcpy(m_table.m_table[rid].sOnlineStatus,online.sOnlineStatus);
				//先删除索引再增加
				m_index_loginstatus.Clear();
				int rowid;
				bool bRet = m_pkey.First(rowid);
				while (bRet)
				{
					m_index_loginstatus.Add(rowid,m_table.m_table[rowid].sOnlineStatus);
					bRet = m_pkey.Next(rowid);
				}

			}
			else
			{
				m_table.m_table[rid].nDrebId = online.nDrebId;
				m_table.m_table[rid].nDrebPid = online.nDrebPid;
				m_table.m_table[rid].nGateIndex = online.nGateIndex;
				m_table.m_table[rid].nSvrId = online.nSvrId;
				m_table.m_table[rid].nSvrPid = online.nSvrPid;
				m_table.m_table[rid].nTimeStamp = online.nTimeStamp;
				strcpy(m_table.m_table[rid].sLoginIp,online.sLoginIp);
				strcpy(m_table.m_table[rid].sOnlineTime,online.sOnlineTime);
			}
		}
		else
		{
			rid = m_table.Add(online);
			m_index_loginstatus.Add(rid,m_table.m_table[rid].sCustNo,m_table.m_table[rid].sTermType);
			m_index_cust.Add(rid,m_table.m_table[rid].sCustNo);
		}
		return rid;
	}
	S_MDB_CUST_ONLINE *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
};
class CTbl_Cust_Tradecode
{
protected:
	
	CMemTableNew <S_MDB_CUST_TRADECODE>      m_table;
// 	CPkeyCharF<21,3>               m_pkey;//资金账号+交易所+席位
// 	CIndexCharF<21,1>            m_index_txacc; //按资金账号的索引
// 	CIndexCharF<21,3>            m_index_tradecode; //交易所+席位+交易编码
// 	CIndexCharF<2,1>            m_index_exch; //按交易所的索引
	CPkeyCharV<3>               m_pkey;//资金账号+交易所+席位
	CIndexVarChar<1>            m_index_txacc; //按资金账号的索引
	CIndexVarChar<3>            m_index_tradecode; //交易所+席位+交易编码
	CIndexVarChar<1>            m_index_exch; //按交易所的索引
	CBF_Mutex                   m_mutex;//互斥锁
public:
	CTbl_Cust_Tradecode()
	{
		
	}
	virtual ~CTbl_Cust_Tradecode()
	{
		
	}
	void Clear()
	{
		m_pkey.Clear();
		m_index_txacc.Clear();
		m_index_tradecode.Clear();
		m_index_exch.Clear();
		m_table.Clear();
	}
	int Insert(S_MDB_CUST_TRADECODE info)
	{
		if (m_pkey.Find(info.sTxAcc,info.sExCode,info.sSeatNo))
		{
			return -1;
		}
		int rid = m_table.Add(info);
		m_index_tradecode.Add(rid,m_table.m_table[rid].sExCode,m_table.m_table[rid].sSeatNo,m_table.m_table[rid].sTradeCode);
		m_index_txacc.Add(rid,m_table.m_table[rid].sTxAcc);
		m_index_exch.Add(rid,m_table.m_table[rid].sExCode);
		m_pkey.Add(rid,m_table.m_table[rid].sTxAcc,m_table.m_table[rid].sExCode,m_table.m_table[rid].sSeatNo);
		return rid;
	}
	S_MDB_CUST_TRADECODE *Select(char *txacc,char *excode,char *seatno)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		if (!m_pkey.GetRowId(rid,txacc,excode,seatno))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}

	// 函数名: Select
	// 编程  : 王明松 2018-1-29 15:25:03
	// 返回  : bool 
	// 参数  : char *txacc
	// 参数  : std::vector<S_MDB_CUST_TRADECODE *> &reslist
	// 描述  : 根据操作员取出操作员在线信息
	bool Select(char *txacc,std::vector<S_MDB_CUST_TRADECODE *> &reslist)
	{
		CBF_PMutex pp(&m_mutex);
		std::vector<int> iset;
		if (!m_index_txacc.Select(iset,txacc))
		{
			return false;
		}
		for (int i=0; i<iset.size(); i++)
		{
			reslist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
	}
	bool SelectByExch(char *exch,std::vector<S_MDB_CUST_TRADECODE *> &reslist)
	{
		CBF_PMutex pp(&m_mutex);
		std::vector<int> iset;
		if (!m_index_exch.Select(iset,exch))
		{
			return false;
		}
		for (int i=0; i<iset.size(); i++)
		{
			reslist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
	}
	// 函数名: Select
	// 编程  : 王明松 2018-1-30 13:47:52
	// 返回  : bool 
	// 参数  : char *excode
	// 参数  : char *seatno
	// 参数  : char *txcode
	// 参数  : std::vector<S_MDB_CUST_TRADECODE *> &reslist
	// 描述  : 根据交易所、席位交易编码取资金账号
	bool Select(char *excode,char *seatno,char *txcode,std::vector<S_MDB_CUST_TRADECODE *> &reslist)
	{
		CBF_PMutex pp(&m_mutex);
		std::vector<int> iset;
		if (!m_index_tradecode.Select(iset,excode,seatno,txcode))
		{
			return false;
		}
		for (int i=0; i<iset.size(); i++)
		{
			reslist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
	}

	S_MDB_CUST_TRADECODE *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
};


//交易所席位交易员在线表  
class CTbl_Exch_Trader
{
protected:
	
	CMemTableNew <S_MDB_EXCH_TRADER_INFO>   m_table;
// 	CPkeyCharF<17,3>               m_pkey;//交易所代码、席位代码、交易员代码
// 	CIndexCharF<7,3>            m_index_exch_seat_status; //交易所、席位、登录状态
// 	CIndexCharF<7,2>            m_index_exch_seat; //交易所、席位
	CPkeyCharV<3>               m_pkey;//交易所代码、席位代码、交易员代码
	CIndexVarChar<3>            m_index_exch_seat_status; //交易所、席位、登录状态
	CIndexVarChar<2>            m_index_exch_seat; //交易所、席位
	CBF_Mutex                   m_mutex;//互斥锁
public:
	CTbl_Exch_Trader()
	{
		
	}
	virtual ~CTbl_Exch_Trader()
	{
		
	}
	int Insert(S_MDB_EXCH_TRADER_INFO info)
	{
		CBF_PMutex pp(&m_mutex);
		if (m_pkey.Find(info.sExchCode,info.sSeatNo,info.sTraderId))
		{
			return -1;
		}
		int rid = m_table.Add(info);
		m_index_exch_seat_status.Add(rid,m_table.m_table[rid].sExchCode,m_table.m_table[rid].sSeatNo,m_table.m_table[rid].sLoginStatus);
		m_index_exch_seat.Add(rid,m_table.m_table[rid].sExchCode,m_table.m_table[rid].sSeatNo);
		return rid;
	}
	S_MDB_EXCH_TRADER_INFO *Select(char *exchcode,char *seatno,char *tradeid)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		if (!m_pkey.GetRowId(rid,exchcode,seatno,tradeid))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}

	// 函数名: Select
	// 编程  : 王明松 2018-1-30 16:30:25
	// 返回  : bool 
	// 参数  : char *exchcode
	// 参数  : char *seatno
	// 参数  : char *status
	// 参数  : std::vector<S_MDB_EXCH_TRADER_INFO *> &exlist
	// 描述  : 根据交易所、席位、状态来查找记录
	bool Select(char *exchcode,char *seatno,char *status,std::vector<S_MDB_EXCH_TRADER_INFO *> &exlist)
	{
		CBF_PMutex pp(&m_mutex);
		std::vector<int> iset;
		if (!m_index_exch_seat_status.Select(iset,exchcode,seatno,status))
		{
			return false;
		}
		for (int i=0; i< iset.size(); i++)
		{
			exlist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
	}

	// 函数名: Select
	// 编程  : 王明松 2018-1-30 16:31:06
	// 返回  : bool 
	// 参数  : char *exchcode
	// 参数  : char *seatno
	// 参数  : std::vector<S_MDB_EXCH_TRADER_INFO *> &exlist
	// 描述  : 根据交易所、席位来查找记录
	bool Select(char *exchcode,char *seatno,std::vector<S_MDB_EXCH_TRADER_INFO *> &exlist)
	{
		CBF_PMutex pp(&m_mutex);
		std::vector<int> iset;
		if (!m_index_exch_seat.Select(iset,exchcode,seatno))
		{
			return false;
		}
		for (int i=0; i< iset.size(); i++)
		{
			exlist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
	}
	S_MDB_EXCH_TRADER_INFO *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
};


//资金表  S_MDB_CUST_FUND
class CTbl_Fund
{
protected:
	
	CMemTableNew <S_MDB_CUST_FUND>  m_table;
//	CPkeyCharF<21,1>               m_pkey;//交易账号
	CPkeyCharV<2>               m_pkey;//交易账号 币种
	CIndexVarChar<1>            m_indexAcc;//交易账号
	CBF_Mutex                   m_mutex;//互斥锁
public:
	CTbl_Fund()
	{
		
	}
	virtual ~CTbl_Fund()
	{
		
	}
	int Insert(S_MDB_CUST_FUND fund)
	{
		CBF_PMutex pp(&m_mutex);
		if (m_pkey.Find(fund.sTxAcc,fund.sCurrency))
		{
			return -1;
		}
		int rid = m_table.Add(fund);
		m_pkey.Add(rid,m_table.m_table[rid].sTxAcc,m_table.m_table[rid].sCurrency);
		m_indexAcc.Add(rid,m_table.m_table[rid].sTxAcc);
		return rid;
	}
	bool Select(char *txacc,std::vector<S_MDB_CUST_FUND *> &fundlist)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		std::vector<int> iset;
		if (!m_indexAcc.Select(iset,txacc))
		{
			return false;
		}
		for (int i=0; i<iset.size(); i++)
		{
			fundlist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
	}
	bool Select(char *txacc,std::vector<int> &rowidlist)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		std::vector<int> iset;
		if (!m_indexAcc.Select(rowidlist,txacc))
		{
			return false;
		}
		return true;
	}
	S_MDB_CUST_FUND *Select(char *txacc,char *currency)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		if (!m_pkey.Select(rid,txacc,currency))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}
	S_MDB_CUST_FUND *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
	bool Delete(char *txacc,char *currency)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		if (!m_pkey.Select(rid,txacc,currency))
		{
			return false;
		}
		CInt iset;
		iset.Add(rid);
		m_indexAcc.Delete(iset);
		m_pkey.Delete(txacc,currency);
		m_table.Delete(rid);
		return true;
	}
	bool Delete(int rowid)
	{
		CBF_PMutex pp(&m_mutex);
		S_MDB_CUST_FUND *info =  At(rowid);
		if (info == NULL)
		{
			return false;
		}
		CInt iset;
		iset.Add(rowid);
		m_indexAcc.Delete(iset);
		m_pkey.Delete(info->sTxAcc,info->sCurrency);
		m_table.Delete(rowid);
		return true;
	}
};

//风险tick表  S_MDB_CUST_RISK_TICK
class CTbl_Risk_Tick
{
protected:

	CMemTableNew <S_MDB_CUST_RISK_TICK>  m_table;
	//	CPkeyCharF<21,1>               m_pkey;//交易账号
	CPkeyCharV<3>               m_pkey;//交易账号 币种 时间
	CIndexVarChar<1>            m_indexAcc;//交易账号
	CIndexVarChar<2>            m_indexAccCurrency;//交易账号 币种
	CBF_Mutex                   m_mutex;//互斥锁
public:
	CTbl_Risk_Tick()
	{

	}
	virtual ~CTbl_Risk_Tick()
	{

	}
	int Insert(S_MDB_CUST_RISK_TICK fund)
	{
		CBF_PMutex pp(&m_mutex);
		if (m_pkey.Find(fund.sTxAcc,fund.sCurrency,fund.sTime))
		{
			return -1;
		}
		int rid = m_table.Add(fund);
		m_pkey.Add(rid,m_table.m_table[rid].sTxAcc,m_table.m_table[rid].sCurrency,m_table.m_table[rid].sTime);
		m_indexAcc.Add(rid,m_table.m_table[rid].sTxAcc);
		m_indexAccCurrency.Add(rid,m_table.m_table[rid].sTxAcc,m_table.m_table[rid].sCurrency);
		return rid;
	}
	bool Select(char *txacc,std::vector<S_MDB_CUST_RISK_TICK *> &fundlist)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		std::vector<int> iset;
		if (!m_indexAcc.Select(iset,txacc))
		{
			return false;
		}
		for (int i=0; i<iset.size(); i++)
		{
			fundlist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
	}
	bool Select(char *txacc,std::vector<int> &rowidlist)
	{
		CBF_PMutex pp(&m_mutex);
//		int rid;
//		std::vector<int> iset;
		if (!m_indexAcc.Select(rowidlist,txacc))
		{
			return false;
		}
		return true;
	}
	bool Select(char *txacc,char *currency,std::vector<S_MDB_CUST_RISK_TICK *> &fundlist)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		std::vector<int> iset;
		if (!m_indexAccCurrency.Select(iset,txacc,currency))
		{
			return false;
		}
		for (int i=0; i<iset.size(); i++)
		{
			fundlist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
	}
	bool Select(char *txacc,char *currency,std::vector<int> &rowidlist)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		std::vector<int> iset;
		if (!m_indexAccCurrency.Select(rowidlist,txacc,currency))
		{
			return false;
		}
		return true;
	}

	S_MDB_CUST_RISK_TICK *Select(char *txacc,char *currency,char *stime)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		if (!m_pkey.Select(rid,txacc,currency,stime))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}
	S_MDB_CUST_RISK_TICK *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
	bool Delete(char *txacc)
	{
		CBF_PMutex pp(&m_mutex);
		CInt iset;
		if (!m_indexAcc.Select(iset,txacc))
		{
			return true;
		}
		m_indexAcc.Delete(iset);
		m_indexAccCurrency.Delete(iset);
		int rid;
		bool bret = iset.First(rid);
		while (bret)
		{
			m_pkey.Delete(m_table.m_table[rid].sTxAcc,m_table.m_table[rid].sCurrency,m_table.m_table[rid].sTime);
			m_table.Delete(rid);
		}
		return true;
	}
	bool Delete(char *txacc,char *currency,char *stime)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		if (!m_pkey.Select(rid,txacc,currency,stime))
		{
			return false;
		}
		CInt iset;
		iset.Add(rid);
		m_indexAcc.Delete(iset);
		m_indexAccCurrency.Delete(iset);
		m_pkey.Delete(txacc,currency,stime);
		m_table.Delete(rid);
		return true;
	}
	bool Delete(int rowid)
	{
		CBF_PMutex pp(&m_mutex);
		S_MDB_CUST_RISK_TICK *info =  At(rowid);
		if (info == NULL)
		{
			return false;
		}
		CInt iset;
		iset.Add(rowid);
		m_indexAcc.Delete(iset);
		m_indexAccCurrency.Delete(iset);
		m_pkey.Delete(info->sTxAcc,info->sCurrency,info->sTime);
		m_table.Delete(rowid);
		return true;
	}
	void Reset(char *txacc)
	{
		CBF_PMutex pp(&m_mutex);
		CInt iset;
		if (!m_indexAcc.Select(iset,txacc))
		{
			return ;
		}
		m_indexAcc.Delete(iset);
		m_indexAccCurrency.Delete(iset);
		int rowid;
		int bret = iset.First(rowid);
		while (bret)
		{
			m_pkey.Delete(m_table.m_table[rowid].sTxAcc,m_table.m_table[rowid].sCurrency,m_table.m_table[rowid].sTime);
			m_table.Delete(rowid);
			 bret = iset.Next(rowid);
		}
		return ;
	}
};

//资金明细表
class CTbl_FundDetail
{
protected:
	
	CMemTableNew <S_MDB_CUST_FUND_DETAIL>  m_table;
// 	CPkeyCharF<21,3>               m_pkey;//资金账号+资金类型+资金编号
// 	CIndexCharF<21,1>            m_indexAcc;//资金账号
// 	CIndexCharF<21,2>            m_indexAccFundtype;//资金账号+资金类型
	CPkeyCharV<4>               m_pkey;//资金账号+币种+资金类型+资金编号
	CIndexVarChar<1>            m_indexAcc;//资金账号
	CIndexVarChar<2>            m_indexAccCurr;//资金账号+币种
	CIndexVarChar<3>            m_indexAccFundtype;//资金账号+币种+资金类型
	CBF_Mutex                   m_mutex;//互斥锁
public:
	CTbl_FundDetail()
	{
		
	}
	virtual ~CTbl_FundDetail()
	{
		
	}
	int Insert(S_MDB_CUST_FUND_DETAIL fund)
	{
		CBF_PMutex pp(&m_mutex);
		if (m_pkey.Find(fund.sTxAcc,fund.sCurrency,fund.sFundType,fund.sFundCode))
		{
			return -1;
		}
		int rid = m_table.Add(fund);
		m_pkey.Add(rid,m_table.m_table[rid].sTxAcc,m_table.m_table[rid].sCurrency,m_table.m_table[rid].sFundType,m_table.m_table[rid].sFundCode);
		m_indexAcc.Add(rid,m_table.m_table[rid].sTxAcc);
		m_indexAccCurr.Add(rid,m_table.m_table[rid].sTxAcc,m_table.m_table[rid].sCurrency);
		m_indexAccFundtype.Add(rid,m_table.m_table[rid].sTxAcc,m_table.m_table[rid].sCurrency,m_table.m_table[rid].sFundType);
		return rid;
	}
	S_MDB_CUST_FUND_DETAIL *Select(char *txacc,char *currency,char *fundtype,char *fundcode)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		if (!m_pkey.Select(rid,txacc,currency,fundtype,fundcode))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}
	S_MDB_CUST_FUND_DETAIL *Select(char *txacc,char *currency,char fundtype,char *fundcode)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		char sfundtype[2];
		sfundtype[0]= fundtype;
		sfundtype[1]= 0;
		if (!m_pkey.Select(rid,txacc,currency,sfundtype,fundcode))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}
	bool Select(char *txacc,char *currency,std::vector<S_MDB_CUST_FUND_DETAIL *> &reslist)
	{
		CBF_PMutex pp(&m_mutex);
		std::vector<int>iset;
		if (!m_indexAccCurr.Select(iset,txacc,currency))
		{
			return false;
		}
		for (int i=0;i<iset.size(); i++)
		{
			reslist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
	}
	bool Select(char *txacc,std::vector<S_MDB_CUST_FUND_DETAIL *> &reslist)
	{
		CBF_PMutex pp(&m_mutex);
		std::vector<int>iset;
		if (!m_indexAcc.Select(iset,txacc))
		{
			return false;
		}
		for (int i=0;i<iset.size(); i++)
		{
			reslist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
	}
	bool Reset(char *txacc)
	{
		CBF_PMutex pp(&m_mutex);
		std::vector<int>iset;
		if (!m_indexAcc.Select(iset,txacc))
		{
			return false;
		}
		for (int i=0;i<iset.size(); i++)
		{
			m_table.m_table[iset[i]].dFundValue = 0.0;
		}
		return true;
	}
	bool Select(char *txacc,char *currency,char *fundtype,std::vector<S_MDB_CUST_FUND_DETAIL *> &reslist)
	{
		CBF_PMutex pp(&m_mutex);
		std::vector<int>iset;
		if (!m_indexAccFundtype.Select(iset,txacc,currency,fundtype))
		{
			return false;
		}
		for (int i=0;i<iset.size(); i++)
		{
			reslist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
	}
	S_MDB_CUST_FUND_DETAIL *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
};


//持仓表的资金账号+合约编号的索引
class Index_Position_Txacc_contract : public CIndex_Field_Base
{
public:
	char  sTxAcc[21];//资金账号
	int   nContractNo;//合约编号


	virtual bool operator<(const Index_Position_Txacc_contract& field) const
	{
		int ret = strcmp(sTxAcc,field.sTxAcc);
		if (ret < 0)
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
//持仓表的资金账号+合约编号的索引
class Index_Position_Txacc_Variety : public CIndex_Field_Base
{
public:
	char  sTxAcc[21];//资金账号
	int   nVarietyId;//品种编号


	virtual bool operator<(const Index_Position_Txacc_Variety& field) const
	{
		int ret = strcmp(sTxAcc,field.sTxAcc);
		if (ret < 0)
		{
			return true;
		}
		else if (ret == 0)
		{
			if  (nVarietyId < field.nVarietyId)
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
//持仓表的资金账号+席位+合约编号_投保标志的索引
class Index_Position_Txacc_seat_contract_shflag : public CIndex_Field_Base
{
public:
	char  sTxAcc[21];//资金账号
	char  sSeatNo[7];//席位代码
	int   nContractNo;//合约编号
	char  sShFlag[2];//投保标志
	
	
	virtual bool operator<(const Index_Position_Txacc_seat_contract_shflag& field) const
	{
		int ret = strcmp(sTxAcc,field.sTxAcc);
		if (ret < 0)
		{
			return true;
		}
		else if (ret == 0)
		{
			int ret2= strcmp(sSeatNo,field.sSeatNo);
			if (ret2<0)
			{
				return true;
			}
			else if (ret2 == 0)
			{
				if (sShFlag[0] < field.sShFlag[0])
				{
					return true;
				}
				else if (sShFlag[0] == field.sShFlag[0])
				{
					if  (nContractNo < field.nContractNo)
					{
						return true;
					}
				}
			}
			return false;
		}
		else
		{
			return false;
		}
	}
};
//持仓表 S_MDB_CUST_POSITION
class CTbl_Position
{
protected:
	
	CMemTableNew <S_MDB_CUST_POSITION>  m_table;
	CKeyField<Index_Position_Txacc_seat_contract_shflag> m_keyno;  //资金账号+席位+合约编号+投保标志
	CIndexField<Index_Position_Txacc_contract>   m_index_AccNo; //资金账号+合约编号
	CIndexField<Index_Position_Txacc_Variety>   m_index_AccVariety; //资金账号+品种编号
// 	CPkeyCharF<21,4>                            m_keycode;//资金账号+席位+合约代码+投保标志
// 	CIndexCharF<21,2>            m_index_acccode;//资金账号+合约代码
// 	CIndexCharF<21,1>            m_index_acc;//资金账号
// 	CIndexCharF<21,1>            m_index_contractcode;//合约代码

	CPkeyCharV<4>                            m_keycode;//资金账号+席位+合约代码+投保标志
	CIndexVarChar<2>            m_index_acccode;//资金账号+合约代码
	CIndexVarChar<1>            m_index_acc;//资金账号
	CIndexVarChar<1>            m_index_contractcode;//合约代码
	CIndexInt<1>                m_index_contractno;//合约编号
	CBF_Mutex                   m_mutex;//互斥锁
public:
	CTbl_Position()
	{
		
	}
	virtual ~CTbl_Position()
	{
		
	}	
	int Insert(S_MDB_CUST_POSITION posi)
	{
		CBF_PMutex pp(&m_mutex);
		if (m_keycode.Find(posi.sTxAcc,posi.sSeatNo,posi.sShFlag,posi.sContractCode))
		{
			return -1;
		}
		Index_Position_Txacc_seat_contract_shflag aa;
		strcpy(aa.sTxAcc,posi.sTxAcc);
		strcpy(aa.sSeatNo,posi.sSeatNo);
		strcpy(aa.sShFlag,posi.sShFlag);
		aa.nContractNo = posi.nContractNo;
		if (m_keyno.Find(aa))
		{
			return -1;
		}
		int rid = m_table.Add(posi);
		aa.m_nRowId = rid;

		Index_Position_Txacc_contract bb;
		strcpy(bb.sTxAcc,posi.sTxAcc);
		bb.nContractNo = posi.nContractNo;
		bb.m_nRowId = rid;
		Index_Position_Txacc_Variety cc;
		strcpy(bb.sTxAcc,posi.sTxAcc);
		cc.nVarietyId = posi.nVarietyId;
		cc.m_nRowId = rid;

		m_keyno.Add(aa);
		m_keycode.Add(rid,m_table.m_table[rid].sTxAcc,m_table.m_table[rid].sSeatNo,m_table.m_table[rid].sShFlag,m_table.m_table[rid].sContractCode);
		m_index_AccNo.Add(bb);
		m_index_AccVariety.Add(cc);
		m_index_acccode.Add(rid,m_table.m_table[rid].sTxAcc,m_table.m_table[rid].sContractCode);
		m_index_acc.Add(rid,m_table.m_table[rid].sTxAcc);
		m_index_contractno.Add(rid,m_table.m_table[rid].nContractNo);
		m_index_contractcode.Add(rid,m_table.m_table[rid].sContractCode);
		return rid;
	}
	S_MDB_CUST_POSITION *Select(char *txacc,char *seatno,char *shflag,char *contractcode)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		if (!m_keycode.Select(rid,txacc,seatno,shflag,contractcode))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}
	S_MDB_CUST_POSITION *Select(char *txacc,char *seatno,char *shflag,int contractno)
	{
		CBF_PMutex pp(&m_mutex);
		Index_Position_Txacc_seat_contract_shflag aa;
		strcpy(aa.sTxAcc,txacc);
		strcpy(aa.sSeatNo,seatno);
		strcpy(aa.sShFlag,shflag);
		aa.nContractNo = contractno;

		if (!m_keyno.Find(aa))
		{
			return NULL;
		}
		return &(m_table.m_table[aa.m_nRowId]);
	}
	bool SelectByAcc(char *txacc,std::vector<S_MDB_CUST_POSITION *> &posilist)
	{
		CBF_PMutex pp(&m_mutex);
		//两种查询，可以实测一下哪个效率更高
		int rid;
		bool bret = m_index_acc.Find(rid,txacc);
		if (!bret)
		{
			return false;
		}
		while (bret)
		{
			posilist.push_back(&(m_table.m_table[rid]));
			bret = m_index_acc.Next(rid);
		}
		
// 		std::vector<int> iset;
// 		if (!m_index_acc.Select(iset,txacc))
// 		{
// 			return false;
// 		}
// 		for (int i=0; i<iset.size() ;i++)
// 		{
// 			posilist.push_back(&(m_table.m_table[iset[i]]));
// 		}
		return true;
	}
	bool SelectByAccOrderByVariety(char *txacc,std::vector<S_MDB_CUST_POSITION *> &posilist)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		std::vector<int> iset;
		if (!m_index_acc.Select(iset,txacc))
		{
			return false;
		}
		CIndexInt<1> orderby;
		for (int i=0; i<iset.size(); i++)
		{
			orderby.Add(iset[i],m_table.m_table[iset[i]].nVarietyId);
		}
		bool bret = orderby.First(rid);
		while (bret)
		{
			posilist.push_back(&(m_table.m_table[rid]));
			bret = orderby.Next(rid);
		}
		return true;
	}
	bool Reset(char *txacc)
	{
		CBF_PMutex pp(&m_mutex);
		//两种查询，可以实测一下哪个效率更高
		int rid;
		bool bret = m_index_acc.Find(rid,txacc);
		if (!bret)
		{
			return false;
		}
		while (bret)
		{
			m_table.m_table[rid].nLastPB=0;//上日买持 多头持仓
			m_table.m_table[rid].nLastPS=0;//上日卖持  空头持仓
			m_table.m_table[rid].nCurPB=0;//当前多仓
			m_table.m_table[rid].nCurPS=0;//当前空仓
			m_table.m_table[rid].nCurCanUsePB=0;//当前可用多仓
			m_table.m_table[rid].nCurCanUsePS=0;//当前可用空仓
			m_table.m_table[rid].nDayPB=0;//当天开多仓
			m_table.m_table[rid].nDayPS=0;//当天开空仓
			m_table.m_table[rid].nDAY_COV_PB_Froze=0;//当天平多仓冻结
			m_table.m_table[rid].nDAY_COV_PB=0;//当天平多仓
			m_table.m_table[rid].nDAY_COV_PS_Froze=0;//当天平空仓冻结
			m_table.m_table[rid].nDAY_COV_PS=0;//当天平空仓
			m_table.m_table[rid].nDAY_DELI_PB_Froze=0;//当天交收多仓
			m_table.m_table[rid].nDAY_DELI_PB=0;//当天交收多仓
			m_table.m_table[rid].nDAY_DELI_PS_Froze=0;//当天交收空仓冻结
			m_table.m_table[rid].nDAY_DELI_PS=0;//当天交收空仓
			m_table.m_table[rid].nDAY_MIDD_PB=0;//中立仓多仓
			m_table.m_table[rid].nDAY_MIDD_PS=0;//中立仓空仓
			m_table.m_table[rid].dPB_OAvp=0.0;//多仓开仓均价    现货股票开仓成本均价
			m_table.m_table[rid].dPS_OAvp=0.0;//空仓开仓均价
			m_table.m_table[rid].dPB_Avp=0.0;//多仓持仓均价         现货股票成本均价(同股票，卖出盈利后成本降低，卖出亏损成本升高)
			m_table.m_table[rid].dPS_Avp=0.0;//空仓持仓均价
			m_table.m_table[rid].dPB_Margin=0.0;//多仓持仓保证金    现货股票持仓成本
			m_table.m_table[rid].dPS_Margin=0.0;//空仓持仓保证金
			m_table.m_table[rid].dDAY_PB_SURPLUS=0.0;//当日多头盈亏
			m_table.m_table[rid].dDAY_PS_SURPLUS=0.0;//当日空头盈亏
			m_table.m_table[rid].dLAST_SETTLE_PRICE=0.0;//上日结算价
			m_table.m_table[rid].dSETTLE_PRICE=0.0;//结算价
			
			bret = m_index_acc.Next(rid);
		}
		return true;
	}
	bool SelectByAccContract(char *txacc,int contractno,std::vector<S_MDB_CUST_POSITION *> &posilist)
	{
		CBF_PMutex pp(&m_mutex);
		//两种查询，可以实测一下哪个效率更高
		int rid;
		Index_Position_Txacc_contract bb;
		strcpy(bb.sTxAcc,txacc);
		bb.nContractNo = contractno;

		bool bret = m_index_AccNo.Find(bb);
		if (!bret)
		{
			return false;
		}
		while (bret)
		{
			posilist.push_back(&(m_table.m_table[bb.m_nRowId]));
			bret = m_index_AccNo.Next(bb);
		}
		return true;
	}
	bool SelectByAccVariety(char *txacc,int varietyid,std::vector<S_MDB_CUST_POSITION *> &posilist)
	{
		CBF_PMutex pp(&m_mutex);
		//两种查询，可以实测一下哪个效率更高
		int rid;
		Index_Position_Txacc_Variety bb;
		strcpy(bb.sTxAcc,txacc);
		bb.nVarietyId = varietyid;

		bool bret = m_index_AccVariety.Find(bb);
		if (!bret)
		{
			return false;
		}
		while (bret)
		{
			posilist.push_back(&(m_table.m_table[bb.m_nRowId]));
			bret = m_index_AccVariety.Next(bb);
		}
		return true;
	}
	bool SelectByAccContract(char *txacc,char *contractcode,std::vector<S_MDB_CUST_POSITION *> &posilist)
	{
		CBF_PMutex pp(&m_mutex);
		//两种查询，可以实测一下哪个效率更高
		int rid;
		bool bret = m_index_acccode.Find(rid,txacc);
		if (!bret)
		{
			return false;
		}
		while (bret)
		{
			posilist.push_back(&(m_table.m_table[rid]));
			bret = m_index_acc.Next(rid);
		}
		return true;
	}
	bool SelectByContract(int contractno,std::vector<S_MDB_CUST_POSITION *> &posilist)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		bool bret = m_index_contractno.Find(rid,contractno);
		if (!bret)
		{
			return false;
		}
		while (bret)
		{
			posilist.push_back(&(m_table.m_table[rid]));
			bret = m_index_contractno.Next(rid);
		}
		return true;
	}
	bool SelectByContract(char *contractcode,std::vector<S_MDB_CUST_POSITION *> &posilist)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		bool bret = m_index_contractcode.Find(rid,contractcode);
		if (!bret)
		{
			return false;
		}
		while (bret)
		{
			posilist.push_back(&(m_table.m_table[rid]));
			bret = m_index_contractcode.Next(rid);
		}
		return true;
	}
	S_MDB_CUST_POSITION *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
};


//库存表  S_MDB_STOR_INFO
class CTbl_Storage
{
protected:
	
	CMemTableNew <S_MDB_STOR_INFO>  m_table;
// 	CPkeyCharF<21,2>               m_pkey;//交易账号+品种代码
// 	CIndexCharF<21,1>            m_index_acc;//交易账号
// 	CIndexCharF<7,1>            m_index_prodcode;//品种代码

	CPkeyCharV<2>               m_pkey;//交易账号+品种代码
	CIndexVarChar<1>            m_index_acc;//交易账号
	CIndexVarChar<1>            m_index_prodcode;//品种代码
	CBF_Mutex                   m_mutex;//互斥锁
public:
	CTbl_Storage()
	{
		
	}
	virtual ~CTbl_Storage()
	{
		
	}	
	int Insert(S_MDB_STOR_INFO stor)
	{
		CBF_PMutex pp(&m_mutex);
		if (m_pkey.Find(stor.sTxAcc,stor.sProdCode))
		{
			return -1;
		}
		int rid = m_table.Add(stor);
		m_pkey.Add(rid,m_table.m_table[rid].sTxAcc,m_table.m_table[rid].sProdCode);
		m_index_acc.Add(rid,m_table.m_table[rid].sTxAcc);
		m_index_prodcode.Add(rid,m_table.m_table[rid].sProdCode);
		return rid;
	}
	S_MDB_STOR_INFO *Select(char *txacc,char *prodcode)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		if (!m_pkey.Select(rid,txacc,prodcode))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}
	bool SelectByAcc(char *txacc,std::vector<S_MDB_STOR_INFO *> &storlist)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		bool bret = m_index_acc.Find(rid,txacc);
		if (!bret)
		{
			return false;
		}
		while (bret)
		{
			storlist.push_back(&(m_table.m_table[rid]));
			bret = m_index_acc.Next(rid);
		}
		return true;
	}
	bool SelectByVariety(char *prodcode,std::vector<S_MDB_STOR_INFO *> &storlist)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		bool bret = m_index_prodcode.Find(rid,prodcode);
		if (!bret)
		{
			return false;
		}
		while (bret)
		{
			storlist.push_back(&(m_table.m_table[rid]));
			bret = m_index_prodcode.Next(rid);
		}
		return true;
	}

	S_MDB_STOR_INFO *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
};

//报单表 S_ORDER_INFO
class CTbl_Order
{
protected:
	
	CMemTableNew <S_MDB_ORDER_INFO>  m_table;
// 	CPkeyCharF<38,1>               m_pkey;//本地报单号键
// 	CIndexCharF<21,1>            m_index_acc;//交易账号
// 	CIndexCharF<19,1>            m_index_frontserial;//前端流水号
// 	CIndexCharF<21,2>            m_index_operfrontserial;//操作员+前端流水号
// 	CIndexCharF<21,1>            m_index_oper;//操作员客户
// 	CIndexCharF<21,1>            m_index_systemno;//系统报单号
// 	CIndexCharF<9,2>            m_index_date_exch;//交易日期+交易所代码

	CPkeyCharV<1>               m_pkey;//本地报单号键
	CIndexVarChar<1>            m_index_acc;//交易账号
	CIndexVarChar<1>            m_index_frontserial;//前端流水号
	CIndexVarChar<2>            m_index_operfrontserial;//操作员+前端流水号
	CIndexVarChar<1>            m_index_oper;//操作员客户
	CIndexVarChar<2>            m_index_systemno;//交易所代码+系统报单号
	CIndexVarChar<2>            m_index_date_exch;//交易日期+交易所代码
	CBF_Mutex                   m_mutex;//互斥锁
public:
	CTbl_Order()
	{
		
	}
	virtual ~CTbl_Order()
	{
		
	}	
	int Insert(S_MDB_ORDER_INFO order)
	{
		CBF_PMutex pp(&m_mutex);
		if (m_pkey.Find(order.sLocalSerial))
		{
			return -1;
		}
		int rid = m_table.Add(order);
		m_pkey.Add(rid,m_table.m_table[rid].sLocalSerial);
		m_index_acc.Add(rid,m_table.m_table[rid].sTxAcc);
		m_index_frontserial.Add(rid,m_table.m_table[rid].sClientSerial);
		m_index_systemno.Add(rid,m_table.m_table[rid].sExchCode,m_table.m_table[rid].sSystemNo);
		m_index_date_exch.Add(rid,m_table.m_table[rid].sTxDate,m_table.m_table[rid].sExchCode);
		m_index_operfrontserial.Add(rid,m_table.m_table[rid].sOrderOper,m_table.m_table[rid].sClientSerial);
		m_index_oper.Add(rid,m_table.m_table[rid].sOrderOper);
		return rid;
	}
	S_MDB_ORDER_INFO *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}

	// 函数名: Update
	// 编程  : 王明松 2017-8-7 15:58:37
	// 返回  : bool 
	// 参数  : int rid
	// 参数  : char *excode  交易所代码
	// 参数  : char *systemno 系统报单号
	// 描述  : 修改索引的键值，不能直接修改指针，必须先调用本方法，以重建系统报单号索引，然后再去修改指针
	bool Update(int rid,char *excode,char *systemno)
	{
		S_MDB_ORDER_INFO *tmp = At(rid);
		if (tmp == NULL)
		{
			return false;
		}
		CInt iset;
		iset.Add(rid);
		CBF_PMutex pp(&m_mutex);
		m_index_systemno.Delete(iset,excode,systemno);
		strcpy(m_table.m_table[rid].sExchCode,excode);
		strcpy(m_table.m_table[rid].sSystemNo,systemno);
		m_index_systemno.Add(rid,m_table.m_table[rid].sExchCode,m_table.m_table[rid].sSystemNo);
		return true;
	}

	// 函数名: Update
	// 编程  : 王明松 2017-8-8 9:17:43
	// 返回  : int 
	// 参数  : char *localno
	// 参数  : char *excode  交易所代码
	// 参数  : char *systemno 系统报单号
	// 描述  : 根据本地报单号修改系统报单号，并返回rowid  因为系统报单号是索引，不能直接修改指针的值
	int Update(char *localno,char *excode,char *systemno)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		if (!m_pkey.Select(rid,localno))
		{
			return -1;
		}
		CInt iset;
		iset.Add(rid);
		m_index_systemno.Delete(iset,excode,systemno);
		strcpy(m_table.m_table[rid].sExchCode,excode);
		strcpy(m_table.m_table[rid].sSystemNo,systemno);
		m_index_systemno.Add(rid,m_table.m_table[rid].sExchCode,m_table.m_table[rid].sSystemNo);
		return rid;
	}
	S_MDB_ORDER_INFO *Select(char *localno)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		if (!m_pkey.Select(rid,localno))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}

	bool SelectByClient(char *cs,std::vector<S_MDB_ORDER_INFO *> &orderlist)
	{
		CBF_PMutex pp(&m_mutex);
		std::vector<int> iset;
		if (!m_index_frontserial.Select(iset,cs))
		{
			return false;
		}
		for (int i=0; i<iset.size(); i++)
		{
			orderlist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
	}

	// 函数名: SelectByOper
	// 编程  : 王明松 2018-3-16 10:52:59
	// 返回  : bool 
	// 参数  : char *oper
	// 参数  : std::vector<S_MDB_ORDER_INFO *> &orderlist
	// 描述  : 根据操作员/客户查询报单，按客户端流水从小到大排序
	bool SelectByOper(char *oper,std::vector<S_MDB_ORDER_INFO *> &orderlist)
	{
		CBF_PMutex pp(&m_mutex);
		std::vector<int> iset;
		if (!m_index_oper.Select(iset,oper))
		{
			return false;
		}
		CIndexCharF<19,1> order;
		for (int i=0; i<iset.size(); i++)
		{
			order.Add(iset[i],m_table.m_table[iset[i]].sClientSerial);
		}
		int rid;
		bool bret = order.First(rid);
		while (bret)
		{
			orderlist.push_back(&(m_table.m_table[rid]));
			bret = order.Next(rid);
		}
		return true;
	}
	bool SelectByClient(char *oper,char *cserial,std::vector<S_MDB_ORDER_INFO *> &orderlist)
	{
		CBF_PMutex pp(&m_mutex);
		std::vector<int> iset;
		if (!m_index_operfrontserial.Select(iset,oper,cserial))
		{
			return false;
		}
		for (int i=0; i<iset.size(); i++)
		{
			orderlist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
	}
	bool SelectByAcc(char *acc,std::vector<S_MDB_ORDER_INFO *> &orderlist)
	{
		CBF_PMutex pp(&m_mutex);
		std::vector<int> iset;
		if (!m_index_acc.Select(iset,acc))
		{
			return false;
		}
		for (int i=0; i<iset.size(); i++)
		{
			orderlist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
	}
	bool SelectBySystemno(char *excode,char *systemno,std::vector<S_MDB_ORDER_INFO *> &orderlist)
	{
		CBF_PMutex pp(&m_mutex);
		std::vector<int> iset;
		if (!m_index_acc.Select(iset,excode,systemno))
		{
			return false;
		}
		for (int i=0; i<iset.size(); i++)
		{
			orderlist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
	}
	// 函数名: Delete
	// 编程  : 王明松 2017-8-8 10:48:53
	// 返回  : int 
	// 参数  : char *txdate
	// 参数  : char *exchcode
	// 描述  : 删除交易日期及交易所代码的报单
	int Delete(char *txdate,char *exchcode)
	{
		CBF_PMutex pp(&m_mutex);
		CInt iset;
		if (!m_index_date_exch.Select(iset,txdate,exchcode))
		{
			return 0;
		}
		m_index_acc.Delete(iset);
		m_index_date_exch.Delete(iset);
		m_index_frontserial.Delete(iset);
		m_index_systemno.Delete(iset);
		m_index_operfrontserial.Delete(iset);
		m_index_oper.Delete(iset);
		
		int rid;
		bool bret = iset.First(rid);
		while (bret)
		{
			m_pkey.Delete(m_table.m_table[rid].sLocalSerial);
			m_table.Delete(rid);
			bret = iset.Next(rid);
		}
		return iset.Size();
	}
	S_MDB_ORDER_INFO *First()
	{
		return m_table.First();
	}
	S_MDB_ORDER_INFO *Next()
	{
		return m_table.Next();
	}
	S_MDB_ORDER_INFO *KeyFirst()
	{
		int id;
		if (!m_pkey.First(id))
		{
			return NULL;
		}
		return &(m_table.m_table[id]);
	}
	S_MDB_ORDER_INFO *KeyNext()
	{
		int id;
		if (!m_pkey.Next(id))
		{
			return NULL;
		}
		return &(m_table.m_table[id]);
	}
	
};


//成交表  S_DONE_INFO
class CTbl_Done
{
protected:
	
	CMemTableNew <S_MDB_DONE_INFO>     m_table;

	CPkeyCharV<3>               m_pkey;//交易所代码+买卖标识+成交编号
	CIndexVarChar<1>            m_index_acc;//交易账号
	CIndexVarChar<1>            m_index_localno;//本地报单号
	CIndexVarChar<1>            m_index_systemno;//系统报单号
	CIndexVarChar<2>            m_index_date_exch;//交易日期+交易所代码
	CIndexVarChar<1>            m_index_oper;//报单操作员/客户
	CBF_Mutex                   m_mutex;//互斥锁
public:
	CTbl_Done()
	{
		
	}
	virtual ~CTbl_Done()
	{
		
	}
	int Insert(S_MDB_DONE_INFO done)
	{
		CBF_PMutex pp(&m_mutex);
		if (m_pkey.Find(done.sExchCode,done.sBsFlag,done.sDoneNo))
		{
			return -1;
		}
		int rid;
		rid = m_table.Add(done);
		m_pkey.Add(rid,m_table.m_table[rid].sExchCode,m_table.m_table[rid].sBsFlag,m_table.m_table[rid].sDoneNo);
		m_index_systemno.Add(rid,m_table.m_table[rid].sSystemNo);
		m_index_localno.Add(rid,m_table.m_table[rid].sLocalNo);
		m_index_acc.Add(rid,m_table.m_table[rid].sTxAcc);
		m_index_date_exch.Add(rid,m_table.m_table[rid].sDoneDate,m_table.m_table[rid].sExchCode);
		m_index_oper.Add(rid,m_table.m_table[rid].sOrderOper);
		return rid;
	}
	S_MDB_DONE_INFO *Select(char *excode,char *bsflag,char *doneno)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		if (!m_pkey.Select(rid,excode,bsflag,doneno))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}
	bool SelectByAcc(char *acc,std::vector<S_MDB_DONE_INFO *> &donelist)
	{
		CBF_PMutex pp(&m_mutex);
		std::vector<int> iset;
		if (!m_index_acc.Select(iset,acc))
		{
			return false;
		}
		for (int i=0; i< iset.size(); i++)
		{
			donelist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
	}
	bool SelectByLocalno(char *localno,std::vector<S_MDB_DONE_INFO *> &donelist)
	{
		CBF_PMutex pp(&m_mutex);
		std::vector<int> iset;
		if (!m_index_localno.Select(iset,localno))
		{
			return false;
		}
		for (int i=0; i< iset.size(); i++)
		{
			donelist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
	}
	bool SelectBySystemno(char *systemno,std::vector<S_MDB_DONE_INFO *> &donelist)
	{
		CBF_PMutex pp(&m_mutex);
		std::vector<int> iset;
		if (!m_index_systemno.Select(iset,systemno))
		{
			return false;
		}
		for (int i=0; i< iset.size(); i++)
		{
			donelist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
	}
	bool SelectByOper(char *oper,std::vector<S_MDB_DONE_INFO *> &donelist)
	{
		CBF_PMutex pp(&m_mutex);
		std::vector<int> iset;
		if (!m_index_oper.Select(iset,oper))
		{
			return false;
		}
		for (int i=0; i< iset.size(); i++)
		{
			donelist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
	}

	// 函数名: Delete
	// 编程  : 王明松 2017-8-8 10:48:53
	// 返回  : int 
	// 参数  : char *txdate
	// 参数  : char *exchcode
	// 描述  : 删除交易日期及交易所代码的成交
	int Delete(char *txdate,char *exchcode)
	{
		CBF_PMutex pp(&m_mutex);
		CInt iset;
		if (!m_index_date_exch.Select(iset,txdate,exchcode))
		{
			return 0;
		}
		m_index_date_exch.Delete(iset);
		m_index_localno.Delete(iset);
		m_index_systemno.Delete(iset);
		m_index_acc.Delete(iset);
		m_index_oper.Delete(iset);
		int rid;
		bool bret = iset.First(rid);
		while (bret)
		{
			m_pkey.Delete(m_table.m_table[rid].sExchCode,m_table.m_table[rid].sBsFlag,m_table.m_table[rid].sDoneNo);
			m_table.Delete(rid);
			bret = iset.Next(rid);
		}
		return iset.Size();
	}
	S_MDB_DONE_INFO *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
	S_MDB_DONE_INFO *First()
	{
		return m_table.First();
	}
	S_MDB_DONE_INFO *Next()
	{
		return m_table.Next();
	}
};

//出入金表 S_FUND_FLOW
class CTbl_Fund_Flow
{
protected:
	
	CMemTableNew <S_MDB_FUND_FLOW>     m_table;
// 	CPkeyCharF<31,2>               m_pkey;//交易日期+流水号
// 	CIndexCharF<21,1>            m_index_txacc;//交易账号
// 	CIndexCharF<21,2>            m_index_acc_inout;//交易账号+出入金方向
	CPkeyCharV<2>               m_pkey;//交易日期+流水号
	CIndexVarChar<1>            m_index_txacc;//交易账号
	CIndexVarChar<2>            m_index_acc_inout;//交易账号+出入金方向
	CIndexVarChar<2>            m_index_txaccdate;//交易账号+日期
	CBF_Mutex                   m_mutex;//互斥锁
public:
	CTbl_Fund_Flow()
	{
		
	}
	virtual ~CTbl_Fund_Flow()
	{
		
	}
	int Insert(S_MDB_FUND_FLOW fundflow)
	{
		CBF_PMutex pp(&m_mutex);
		if (m_pkey.Find(fundflow.sTxDate,fundflow.sTxSerial))
		{
			return -1;
		}
		int rid = m_table.Add(fundflow);
		m_pkey.Add(rid,m_table.m_table[rid].sTxDate,m_table.m_table[rid].sTxSerial);
		m_index_txacc.Add(rid,m_table.m_table[rid].sTxAcc);
		m_index_acc_inout.Add(rid,m_table.m_table[rid].sTxAcc,m_table.m_table[rid].sFundFlag);
		m_index_txaccdate.Add(rid,m_table.m_table[rid].sTxAcc,m_table.m_table[rid].sTxDate);
		return rid;
	}
	S_MDB_FUND_FLOW *Select(char *txdate,char *serial)
	{
		int rid;
		CBF_PMutex pp(&m_mutex);
		if (!m_pkey.Select(rid,txdate,serial))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}
	bool SelectByDateAccInout(char *acc,char *inout,std::vector<S_MDB_FUND_FLOW *> &flowlist)
	{
		CBF_PMutex pp(&m_mutex);
		std::vector<int> iset;
		if (!m_index_acc_inout.Select(iset,acc,inout))
		{
			return false;
		}
		for (int i=0;i<iset.size(); i++)
		{
			flowlist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
	}
	bool SelectByAcc(char *acc,std::vector<S_MDB_FUND_FLOW *> &flowlist)
	{
		CBF_PMutex pp(&m_mutex);
		std::vector<int> iset;
		if (!m_index_txacc.Select(iset,acc))
		{
			return false;
		}
		for (int i=0;i<iset.size(); i++)
		{
			flowlist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
	}
 	int Delete(char *txacc,char *txdate)
 	{
 		CBF_PMutex pp(&m_mutex);
 		CInt iset;
 		if (!m_index_txaccdate.Select(iset,txacc,txdate))
 		{
 			return 0;
 		}
 		m_index_txacc.Delete(iset);
 		m_index_acc_inout.Delete(iset);
 		m_index_txaccdate.Delete(iset);
 		int rid;
 		bool bret = iset.First(rid);
 		while (bret)
 		{
			m_pkey.Delete(m_table.m_table[rid].sTxDate,m_table.m_table[rid].sTxSerial);
 			m_table.Delete(rid);
 			bret = iset.Next(rid);
 		}
 		return iset.Size();
 	}
	S_MDB_FUND_FLOW *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
};

//行情表  S_MDB_QUOTATION 保留最新的行情
class CTbl_Quotation
{
protected:
	
	CMemTableNew <S_MDB_QUOTATION>  m_table;
	CPkeyInt<1>                 m_keyno;//合约编号
//	CPkeyCharF<21,1>               m_keycode;//合约代码
	CPkeyCharV<1>               m_keycode;//合约代码
	CBF_Mutex                   m_mutex;//互斥锁
public:
	CTbl_Quotation()
	{
		
	}
	virtual ~CTbl_Quotation()
	{
		
	}
	int Insert(S_MDB_QUOTATION quo)
	{
		CBF_PMutex pp(&m_mutex);
		if (m_keyno.Find(quo.nContractNo))
		{
			return -1;
		}
		if (m_keycode.Find(quo.sContractCode))
		{
			return -1;
		}
		int rid = m_table.Add(quo);
		m_keyno.Add(rid,m_table.m_table[rid].nContractNo);
		m_keycode.Add(rid,m_table.m_table[rid].sContractCode);
		return rid;
	}
	S_MDB_QUOTATION *Select(char *contractcode)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		if (!m_keycode.Select(rid,contractcode))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}
	S_MDB_QUOTATION *Select(int contractno)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		if (!m_keyno.Select(rid,contractno))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}
	bool Select(std::vector<S_MDB_QUOTATION *> &reslist)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		bool bret = m_keyno.First(rid);
		if (!bret)
		{
			return false;
		}
		while (bret)
		{
			reslist.push_back(&(m_table.m_table[rid]));
			bret = m_keyno.Next(rid);
		}
		return true;
	}
	S_MDB_QUOTATION *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}

	// 函数名: Update
	// 编程  : 王明松 2018-3-5 9:49:37
	// 返回  : bool 
	// 参数  : S_MDB_QUOTATION &info
	// 描述  : 根据合约编号、行情日期、行情序号来更新行情
	bool Update(S_MDB_QUOTATION &info)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		if (!m_keyno.Select(rid,info.nContractNo))
		{
			int rid = m_table.Add(info);
			m_keyno.Add(rid,m_table.m_table[rid].nContractNo);
			m_keycode.Add(rid,m_table.m_table[rid].sContractCode);
			return true;
		}
		//比较行情日期及行情序号，是否新行情
		if (info.nDate > m_table.m_table[rid].nDate || (info.nDate == m_table.m_table[rid].nDate && info.nQuoSerial > m_table.m_table[rid].nQuoSerial))
		{
			m_table.m_table[rid].nQuoSerial = info.nQuoSerial;
			m_table.m_table[rid].nDate = info.nDate;
			
			m_table.m_table[rid].dLastSetllePrice = info.dLastSetllePrice;
			m_table.m_table[rid].dLastClosePrice= info.dLastClosePrice;
			m_table.m_table[rid].dOpenPrice= info.dOpenPrice;
			m_table.m_table[rid].dHighPrice= info.dHighPrice;
			m_table.m_table[rid].dLowPrice= info.dLowPrice;
			m_table.m_table[rid].dNewPrice= info.dNewPrice;
			m_table.m_table[rid].dClosePrice= info.dClosePrice;
			m_table.m_table[rid].dSettlePrice= info.dSettlePrice;
			m_table.m_table[rid].dBuyPrice1= info.dBuyPrice1;
			m_table.m_table[rid].dBuyPrice2= info.dBuyPrice2;
			m_table.m_table[rid].dBuyPrice3= info.dBuyPrice3;
			m_table.m_table[rid].dBuyPrice4= info.dBuyPrice4;
			m_table.m_table[rid].dBuyPrice5= info.dBuyPrice5;
			m_table.m_table[rid].dBuyPrice6= info.dBuyPrice6;
			m_table.m_table[rid].dBuyPrice7= info.dBuyPrice7;
			m_table.m_table[rid].dBuyPrice8= info.dBuyPrice8;
			m_table.m_table[rid].dBuyPrice9= info.dBuyPrice9;
			m_table.m_table[rid].dBuyPrice10= info.dBuyPrice10;
			m_table.m_table[rid].nBuy1= info.nBuy1;
			m_table.m_table[rid].nBuy2= info.nBuy2;
			m_table.m_table[rid].nBuy3= info.nBuy3;
			m_table.m_table[rid].nBuy4= info.nBuy4;
			m_table.m_table[rid].nBuy5= info.nBuy5;
			m_table.m_table[rid].nBuy6= info.nBuy6;
			m_table.m_table[rid].nBuy7= info.nBuy7;
			m_table.m_table[rid].nBuy8= info.nBuy8;
			m_table.m_table[rid].nBuy9= info.nBuy9;
			m_table.m_table[rid].nBuy10= info.nBuy10;
			m_table.m_table[rid].dSellPrice1= info.dSellPrice1;
			m_table.m_table[rid].dSellPrice2= info.dSellPrice2;
			m_table.m_table[rid].dSellPrice3= info.dSellPrice3;
			m_table.m_table[rid].dSellPrice4= info.dSellPrice4;
			m_table.m_table[rid].dSellPrice5= info.dSellPrice5;
			m_table.m_table[rid].dSellPrice6= info.dSellPrice6;
			m_table.m_table[rid].dSellPrice7= info.dSellPrice7;
			m_table.m_table[rid].dSellPrice8= info.dSellPrice8;
			m_table.m_table[rid].dSellPrice9= info.dSellPrice9;
			m_table.m_table[rid].dSellPrice10= info.dSellPrice10;
			m_table.m_table[rid].nSell1= info.nSell1;
			m_table.m_table[rid].nSell2= info.nSell2;
			m_table.m_table[rid].nSell3= info.nSell3;
			m_table.m_table[rid].nSell4= info.nSell4;
			m_table.m_table[rid].nSell5= info.nSell5;
			m_table.m_table[rid].nSell6= info.nSell6;
			m_table.m_table[rid].nSell7= info.nSell7;
			m_table.m_table[rid].nSell8= info.nSell8;
			m_table.m_table[rid].nSell9= info.nSell9;
			m_table.m_table[rid].nSell10= info.nSell10;
			m_table.m_table[rid].nDoneQty= info.nDoneQty;
			m_table.m_table[rid].dDoneWeight= info.dDoneWeight;
			m_table.m_table[rid].nPosition= info.nPosition;
			m_table.m_table[rid].dUpLimit= info.dUpLimit;
			m_table.m_table[rid].dDownLimit= info.dDownLimit;
			m_table.m_table[rid].dUpDown= info.dUpDown;
			m_table.m_table[rid].dUpDownRate= info.dUpDownRate;
			m_table.m_table[rid].dDownAmt= info.dDownAmt;
			m_table.m_table[rid].dAvp= info.dAvp;
			return true;
		}
		return false;
	}
	
};
//风险合约信息
class  CTbl_RiskContractInfo
{
protected:
	
	CMemTableNew <S_MDB_RISKCONTRACT_INFO> m_table;
	CPkeyUInt<1>                     m_key;//合约编号
	CBF_Mutex                   m_mutex;//互斥锁
public:
	CTbl_RiskContractInfo()
	{
		
	}
	virtual ~CTbl_RiskContractInfo()
	{
		
	}
	void Clear()
	{
		m_key.Clear();
		m_table.Clear();
	}
	int Insert(S_MDB_RISKCONTRACT_INFO info) //只有在初始化时使用，所以可以不加锁
	{
		CBF_PMutex pp(&m_mutex);
		if (m_key.Find(info.nContractNo))
		{
			return -1;
		}
		int rid=m_table.Add(info);
		m_key.Add(rid,m_table.m_table[rid].nContractNo);
		return rid;
	}
	int Update(S_MDB_RISKCONTRACT_INFO info) 
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		if (m_key.Select(rid,info.nContractNo))
		{
			strcpy(m_table.m_table[rid].sRiskFlag,info.sRiskFlag);
			m_table.m_table[rid].dNewPrice =info.dNewPrice;
			m_table.m_table[rid].dUpDown =info.dUpDown;
			m_table.m_table[rid].dUpDownRate =info.dUpDownRate;
			m_table.m_table[rid].dBuyPrice =info.dBuyPrice;
			m_table.m_table[rid].dSellPrice =info.dSellPrice;
			m_table.m_table[rid].dHighPrice =info.dHighPrice;
			m_table.m_table[rid].dLowPrice =info.dLowPrice;
			m_table.m_table[rid].nDoneQty =info.nDoneQty;
			strcpy(m_table.m_table[rid].sRiskDesc,info.sRiskDesc);
			m_table.m_table[rid].nRiskdate = info.nRiskdate;
			m_table.m_table[rid].nRisktime = info.nRisktime;
			return rid;
		}
		rid=m_table.Add(info);
		m_key.Add(rid,info.nContractNo);
		return rid;
	}
	S_MDB_RISKCONTRACT_INFO *Select(unsigned int nContractNo)
	{
		int rid;
		CBF_PMutex pp(&m_mutex);
		if (!m_key.Select(rid,nContractNo))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}

	// 函数名: Select
	// 编程  : 王明松 2018-3-5 21:11:59
	// 返回  : int 
	// 参数  : std::vector<S_MDB_RISKCONTRACT_INFO *> &reslist
	// 描述  : 取出风险合约
	int Select(std::vector<S_MDB_RISKCONTRACT_INFO *> &reslist)
	{
		int rid;
		CBF_PMutex pp(&m_mutex);
		bool bRet = m_key.First(rid);
		while (bRet)
		{
			if (m_table.m_table[rid].sRiskFlag[0] =='1')
			{
				reslist.push_back(&(m_table.m_table[rid]));
			}
			bRet = m_key.Next(rid);
		}
		return reslist.size();
	}
	S_MDB_RISKCONTRACT_INFO *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
};


//客户持仓风险
class  CTbl_Cust_Posi_Risk
{
protected:
	
	CMemTableNew <S_MDB_CUST_POSI_RISK> m_table;
//	CPkeyCharF<21,1>                     m_key;//资金账号
	CPkeyCharV<2>                     m_key;//资金账号+合约
	CIndexVarChar<1>                  m_index;//资金账号
	CBF_Mutex                   m_mutex;//互斥锁
public:
	CTbl_Cust_Posi_Risk()
	{
		
	}
	virtual ~CTbl_Cust_Posi_Risk()
	{
		
	}
	void Clear()
	{
		m_key.Clear();
		m_table.Clear();
		m_index.Clear();
	}
	int Insert(S_MDB_CUST_POSI_RISK info) 
	{
		CBF_PMutex pp(&m_mutex);
		if (m_key.Find(info.sTxAcc,info.sContractCode))
		{
			return -1;
		}
		int rid=m_table.Add(info);
		m_key.Add(rid,m_table.m_table[rid].sTxAcc,m_table.m_table[rid].sContractCode);
		m_index.Add(rid,m_table.m_table[rid].sTxAcc);
		return rid;
	}
	bool Delete(char *txacc)
	{
		CBF_PMutex pp(&m_mutex);
		CInt iset;
		if (!m_index.Select(iset,txacc))
		{
			return true;
		}
		m_index.Delete(iset);
		
		int rid;
		bool bret = iset.First(rid);
		while (bret)
		{
			m_key.Delete(m_table.m_table[rid].sTxAcc,m_table.m_table[rid].sContractCode);
			m_table.Delete(rid);
		}
		return true;
	}
	int Update(S_MDB_CUST_POSI_RISK info) 
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		if (m_key.Select(rid,info.sTxAcc,info.sContractCode))
		{
			strcpy(m_table.m_table[rid].sRiskFlag,info.sRiskFlag);
			m_table.m_table[rid].nWarnLevel =info.nWarnLevel;
			m_table.m_table[rid].nBuyQty =info.nBuyQty;
			m_table.m_table[rid].nSellQty =info.nSellQty;
			strcpy(m_table.m_table[rid].sShFlag,info.sShFlag);
			m_table.m_table[rid].nContractNo =info.nContractNo;
			strcpy(m_table.m_table[rid].sContractCode,info.sContractCode);
			strcpy(m_table.m_table[rid].sRiskDesc,info.sRiskDesc);
			return rid;
		}
		rid=m_table.Add(info);
		m_key.Add(rid,m_table.m_table[rid].sTxAcc,m_table.m_table[rid].sContractCode);
		m_index.Add(rid,m_table.m_table[rid].sTxAcc);
		return rid;
	}
	S_MDB_CUST_POSI_RISK *Select(char *txacc,char *contractcode)
	{
		int rid;
		CBF_PMutex pp(&m_mutex);
		if (!m_key.Select(rid,txacc,contractcode))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}
	int Select(char *txacc,std::vector<S_MDB_CUST_POSI_RISK *> &reslist)
	{
		int rid;
		CBF_PMutex pp(&m_mutex);
		bool bRet = m_index.First(rid);
		while (bRet)
		{
			if (m_table.m_table[rid].sRiskFlag[0] =='1')
			{
				reslist.push_back(&(m_table.m_table[rid]));
			}
			bRet = m_index.Next(rid);
		}
		return reslist.size();
	}
	// 函数名: Select
	// 编程  : 王明松 2018-3-5 21:11:59
	// 返回  : int 
	// 参数  : std::vector<S_MDB_RISKCONTRACT_INFO *> &reslist
	// 描述  : 取出客户持仓风险
	int Select(std::vector<S_MDB_CUST_POSI_RISK *> &reslist)
	{
		int rid;
		CBF_PMutex pp(&m_mutex);
		bool bRet = m_key.First(rid);
		while (bRet)
		{
			if (m_table.m_table[rid].sRiskFlag[0] =='1')
			{
				reslist.push_back(&(m_table.m_table[rid]));
			}
			bRet = m_key.Next(rid);
		}
		return reslist.size();
	}
	S_MDB_CUST_POSI_RISK *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
};

class  CTbl_Oper_Subcribe
{
protected:
	
	CMemTableNew <S_MDB_SUBSCRIBE> m_table;
// 	CPkeyCharF<17,2>                     m_key;//操作员+订阅类型
// 	CIndexCharF<2,1>                    m_index;//订阅类型

	CPkeyCharV<2>                     m_key;//操作员+订阅类型
	CIndexVarChar<1>                    m_index;//订阅类型
	CBF_Mutex                   m_mutex;//互斥锁
public:
	CTbl_Oper_Subcribe()
	{
		
	}
	virtual ~CTbl_Oper_Subcribe()
	{
		
	}
	void Clear()
	{
		m_key.Clear();
		m_table.Clear();
		m_index.Clear();
	}
	int Insert(S_MDB_SUBSCRIBE info) 
	{
		CBF_PMutex pp(&m_mutex);
		if (m_key.Find(info.sOper,info.sSubcribeType))
		{
			return -1;
		}
		int rid=m_table.Add(info);
		m_key.Add(rid,m_table.m_table[rid].sOper,m_table.m_table[rid].sSubcribeType);
		m_index.Add(rid,m_table.m_table[rid].sSubcribeType);
		return rid;
	}
	
	S_MDB_SUBSCRIBE *Select(char *oper,char *subtype)
	{
		int rid;
		CBF_PMutex pp(&m_mutex);
		if (!m_key.Select(rid,oper,subtype))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}
	int Select(char *subtype,std::vector<S_MDB_SUBSCRIBE *> &reslist)
	{
		CBF_PMutex pp(&m_mutex);
		std::vector<int> iset;
		if (!m_index.Select(iset,subtype))
		{
			return false;
		}
		for (int i=0;i<iset.size(); i++)
		{
			reslist.push_back(&(m_table.m_table[iset[i]]));
		}
		return reslist.size();
	}
	S_MDB_SUBSCRIBE *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
};

//产品资金风险表
class  CTbl_Prod_Fund_Risk
{
protected:
	
	CMemTableNew <S_MDB_PROD_FUND_RISK> m_table;
//	CPkeyCharF<13,1>                     m_key;//产品编号
	CPkeyCharV<1>                     m_key;//产品编号
	CBF_Mutex                   m_mutex;//互斥锁
public:
	CTbl_Prod_Fund_Risk()
	{
		
	}
	virtual ~CTbl_Prod_Fund_Risk()
	{
		
	}
	void Clear()
	{
		m_key.Clear();
		m_table.Clear();
	}
	int Insert(S_MDB_PROD_FUND_RISK info) 
	{
		CBF_PMutex pp(&m_mutex);
		if (m_key.Find(info.sProdCode))
		{
			return -1;
		}
		int rid=m_table.Add(info);
		m_key.Add(rid,m_table.m_table[rid].sProdCode);
		return rid;
	}
	int Update(S_MDB_PROD_FUND_RISK info) 
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		if (m_key.Select(rid,info.sProdCode))
		{
			strcpy(m_table.m_table[rid].sRiskFlag,info.sRiskFlag);
			m_table.m_table[rid].nRiskLevel =info.nRiskLevel;
			m_table.m_table[rid].dRisk =info.dRisk;
			m_table.m_table[rid].dUnitValue =info.dUnitValue;
			m_table.m_table[rid].dBal = info.dBal;
			m_table.m_table[rid].dMargin = info.dMargin;
			strcpy(m_table.m_table[rid].sRight,info.sRight);
			
			strcpy(m_table.m_table[rid].sRiskDesc,info.sRiskDesc);
			m_table.m_table[rid].nRiskdate = info.nRiskdate;
			m_table.m_table[rid].nRisktime = info.nRisktime;
			return rid;
		}
		rid=m_table.Add(info);
		m_key.Add(rid,m_table.m_table[rid].sProdCode);
		return rid;
	}
	S_MDB_PROD_FUND_RISK *Select(char *sProdCode)
	{
		int rid;
		CBF_PMutex pp(&m_mutex);
		if (!m_key.Select(rid,sProdCode))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}
	

	S_MDB_PROD_FUND_RISK *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
};

//产品持仓风险表
class  CTbl_Prod_Posi_Risk
{
protected:
	
	CMemTableNew <S_MDB_PROD_POSI_RISK> m_table;
//	CPkeyCharF<13,1>                     m_key;//产品编号
	CPkeyCharV<1>                     m_key;//产品编号
	CBF_Mutex                   m_mutex;//互斥锁
public:
	CTbl_Prod_Posi_Risk()
	{
		
	}
	virtual ~CTbl_Prod_Posi_Risk()
	{
		
	}
	void Clear()
	{
		m_key.Clear();
		m_table.Clear();
	}
	int Insert(S_MDB_PROD_POSI_RISK info) 
	{
		CBF_PMutex pp(&m_mutex);
		if (m_key.Find(info.sProdCode))
		{
			return -1;
		}
		int rid=m_table.Add(info);
		m_key.Add(rid,m_table.m_table[rid].sProdCode);
		return rid;
	}
	int Update(S_MDB_PROD_POSI_RISK info) 
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		if (m_key.Select(rid,info.sProdCode))
		{
			strcpy(m_table.m_table[rid].sRiskFlag,info.sRiskFlag);
			m_table.m_table[rid].nRiskLevel =info.nRiskLevel;
			m_table.m_table[rid].dMarginRate =info.dMarginRate;
			m_table.m_table[rid].dPosiValueRate =info.dPosiValueRate;
			m_table.m_table[rid].dPosiDiffValueRate =info.dPosiDiffValueRate;
			m_table.m_table[rid].dT_Bal = info.dT_Bal;
			m_table.m_table[rid].dT_Buy = info.dT_Buy;
			m_table.m_table[rid].dT_Sell = info.dT_Sell;
			strcpy(m_table.m_table[rid].sRight,info.sRight);
			
			strcpy(m_table.m_table[rid].sRiskDesc,info.sRiskDesc);
			return rid;
		}
		rid=m_table.Add(info);
		m_key.Add(rid,m_table.m_table[rid].sProdCode);
		return rid;
	}
	S_MDB_PROD_POSI_RISK *Select(char *sProdCode)
	{
		int rid;
		CBF_PMutex pp(&m_mutex);
		if (!m_key.Select(rid,sProdCode))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}
	
	
	S_MDB_PROD_POSI_RISK *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
};

//机构资金风险表
class  CTbl_Org_Fund_Risk
{
protected:

	CMemTableNew <S_MDB_ORG_FUND_RISK> m_table;
	CPkeyCharV<2>                     m_key;//机构代码+风险类型
	CIndexVarChar<1>                  m_index;//机构代码
	CBF_Mutex                   m_mutex;//互斥锁
public:
	CTbl_Org_Fund_Risk()
	{

	}
	virtual ~CTbl_Org_Fund_Risk()
	{

	}
	void Clear()
	{
		m_key.Clear();
		m_table.Clear();
	}
	int Insert(S_MDB_ORG_FUND_RISK info) 
	{
		CBF_PMutex pp(&m_mutex);
		if (m_key.Find(info.sOrgCode,info.sRiskType))
		{
			return -1;
		}
		int rid=m_table.Add(info);
		m_key.Add(rid,m_table.m_table[rid].sOrgCode,m_table.m_table[rid].sRiskType);
		m_index.Add(rid,m_table.m_table[rid].sOrgCode);
		return rid;
	}
	int Update(S_MDB_ORG_FUND_RISK info) 
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		if (m_key.Select(rid,info.sOrgCode,info.sRiskType))
		{
			strcpy(m_table.m_table[rid].sRiskFlag,info.sRiskFlag);
			m_table.m_table[rid].nRiskLevel =info.nRiskLevel;
			m_table.m_table[rid].dRisk =info.dRisk;
			m_table.m_table[rid].dUnitValue =info.dUnitValue;
			strcpy(m_table.m_table[rid].sRight,info.sRight);
			strcpy(m_table.m_table[rid].sRiskDesc,info.sRiskDesc);
			return rid;
		}
		rid=m_table.Add(info);
		m_key.Add(rid,m_table.m_table[rid].sOrgCode,m_table.m_table[rid].sRiskType);
		m_index.Add(rid,m_table.m_table[rid].sOrgCode);
		return rid;
	}
	S_MDB_ORG_FUND_RISK *Select(char *sOrgCode,char *risktype)
	{
		int rid;
		CBF_PMutex pp(&m_mutex);
		if (!m_key.Select(rid,sOrgCode,risktype))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}
	bool Select(char *sOrgCode,std::vector<S_MDB_ORG_FUND_RISK *> &reslist)
	{
		CBF_PMutex pp(&m_mutex);
		std::vector<int> iset;
		if (!m_index.Select(iset,sOrgCode))
		{
			return false;
		}
		for (int i=0 ; i< iset.size(); i++)
		{
			reslist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
	}

	S_MDB_ORG_FUND_RISK *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
};

//机构持仓风险表
class  CTbl_Org_Posi_Risk
{
protected:

	CMemTableNew <S_MDB_ORG_POSI_RISK> m_table;
	CPkeyCharV<2>                     m_key;//机构代码+风险类型
	CIndexVarChar<1>                  m_index;//机构代码
	CBF_Mutex                   m_mutex;//互斥锁
public:
	CTbl_Org_Posi_Risk()
	{

	}
	virtual ~CTbl_Org_Posi_Risk()
	{

	}
	void Clear()
	{
		m_key.Clear();
		m_table.Clear();
	}
	int Insert(S_MDB_ORG_POSI_RISK info) 
	{
		CBF_PMutex pp(&m_mutex);
		if (m_key.Find(info.sOrgCode,info.sRiskType))
		{
			return -1;
		}
		int rid=m_table.Add(info);
		m_key.Add(rid,m_table.m_table[rid].sOrgCode,m_table.m_table[rid].sRiskType);
		m_index.Add(rid,m_table.m_table[rid].sOrgCode);
		return rid;
	}
	int Update(S_MDB_ORG_POSI_RISK info) 
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		if (m_key.Select(rid,info.sOrgCode,info.sRiskType))
		{
			strcpy(m_table.m_table[rid].sRiskFlag,info.sRiskFlag);
			m_table.m_table[rid].nRiskLevel =info.nRiskLevel;
			m_table.m_table[rid].dMarginRate =info.dMarginRate;
			m_table.m_table[rid].dPosiValueRate =info.dPosiValueRate;
			m_table.m_table[rid].dPosiDiffValueRate =info.dPosiDiffValueRate;

			strcpy(m_table.m_table[rid].sRight,info.sRight);

			strcpy(m_table.m_table[rid].sRiskDesc,info.sRiskDesc);
			return rid;
		}
		rid=m_table.Add(info);
		m_key.Add(rid,m_table.m_table[rid].sOrgCode,m_table.m_table[rid].sRiskType);
		m_index.Add(rid,m_table.m_table[rid].sOrgCode);
		return rid;
	}
	S_MDB_ORG_POSI_RISK *Select(char *sProdCode,char *risktype)
	{
		int rid;
		CBF_PMutex pp(&m_mutex);
		if (!m_key.Select(rid,sProdCode,risktype))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}
	bool Select(char *sOrgCode,std::vector<S_MDB_ORG_POSI_RISK *> &reslist)
	{
		CBF_PMutex pp(&m_mutex);
		std::vector<int> iset;
		if (!m_index.Select(iset,sOrgCode))
		{
			return false;
		}
		for (int i=0 ; i< iset.size(); i++)
		{
			reslist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
	}

	S_MDB_ORG_POSI_RISK *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
};

//大边保证金表
class  CTbl_Big_Margin
{
protected:

	CMemTableNew <S_MDB_BIG_MARGIN> m_table;
	CKeyField<Index_Position_Txacc_Variety>   m_key; //资金账号+品种编号
	CIndexCharV                               m_index;// 资金账号
	CBF_Mutex                   m_mutex;//互斥锁
public:
	CTbl_Big_Margin()
	{

	}
	virtual ~CTbl_Big_Margin()
	{

	}
	void Clear()
	{
		m_index.Clear();
		m_key.Clear();
		m_table.Clear();
	}
	int Insert(S_MDB_BIG_MARGIN info)
	{
		CBF_PMutex pp(&m_mutex);
		Index_Position_Txacc_Variety bb;
		strcpy(bb.sTxAcc,info.sTxAcc);
		bb.nVarietyId = info.nVarietyId;
		if (m_key.Find(bb))
		{
			return -1;
		}
		int rid = m_table.Add(info);
		bb.m_nRowId = rid;
		m_key.Add(bb);
		m_index.Add(rid,m_table.m_table[rid].sTxAcc);
		return rid;
	}
	S_MDB_BIG_MARGIN *Select(char *txacc,int varietyid)
	{
		CBF_PMutex pp(&m_mutex);
		Index_Position_Txacc_Variety bb;
		strcpy(bb.sTxAcc,txacc);
		bb.nVarietyId = varietyid;
		if (!m_key.Find(bb))
		{
			return NULL;
		}
		return &(m_table.m_table[bb.m_nRowId]);
	}
	bool Select(char *txacc,std::vector<S_MDB_BIG_MARGIN *> reslist)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		bool bret = m_index.Find(rid,txacc);
		if (!bret)
		{
			return false;
		}
		while (bret)
		{
			reslist.push_back(&(m_table.m_table[rid]));
			bret = m_index.Next(rid);
		}
		return true;
	}
	S_MDB_BIG_MARGIN *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
	
};


//持仓轧差表  S_MDB_DIFF_POSI
class CTbl_Diff_Posi
{
protected:

	CMemTableNew <S_MDB_DIFF_POSI>  m_table;
	CPkeyCharV<2>               m_pkey;//日期 流水号
	CIndexVarChar<1>            m_indexAcc;//交易账号
	CBF_Mutex                   m_mutex;//互斥锁
public:
	CTbl_Diff_Posi()
	{

	}
	virtual ~CTbl_Diff_Posi()
	{

	}
	int Insert(S_MDB_DIFF_POSI info)
	{
		CBF_PMutex pp(&m_mutex);
		if (m_pkey.Find(info.sTxDate,info.sSerial))
		{
			return -1;
		}
		int rid = m_table.Add(info);
		m_pkey.Add(rid,m_table.m_table[rid].sTxDate,m_table.m_table[rid].sSerial);
		m_indexAcc.Add(rid,m_table.m_table[rid].sTxAcc);
		return rid;
	}
	bool Select(char *txacc,std::vector<S_MDB_DIFF_POSI *> &reslist)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		std::vector<int> iset;
		if (!m_indexAcc.Select(iset,txacc))
		{
			return false;
		}
		for (int i=0; i<iset.size(); i++)
		{
			reslist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
	}
	bool Select(char *txacc,std::vector<int> &rowidlist)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		if (!m_indexAcc.Select(rowidlist,txacc))
		{
			return false;
		}
		return true;
	}
	S_MDB_DIFF_POSI *Select(char *txdate,char *serial)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		if (!m_pkey.Select(rid,txdate,serial))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}
	S_MDB_DIFF_POSI *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
	bool Delete(char *txdate,char *serial)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		if (!m_pkey.Select(rid,txdate,serial))
		{
			return false;
		}
		CInt iset;
		iset.Add(rid);
		m_indexAcc.Delete(iset);
		m_pkey.Delete(txdate,serial);
		m_table.Delete(rid);
		return true;
	}
	bool Delete(int rowid)
	{
		CBF_PMutex pp(&m_mutex);
		S_MDB_DIFF_POSI *info =  At(rowid);
		if (info == NULL)
		{
			return false;
		}
		CInt iset;
		iset.Add(rowid);
		m_indexAcc.Delete(iset);
		m_pkey.Delete(info->sTxDate,info->sSerial);
		m_table.Delete(rowid);
		return true;
	}
};

class CDb_Trader  
{
public:
	CDb_Trader()
	{

	}
	virtual ~CDb_Trader()
	{

	}
	CTbl_Cust_Online     m_pCustOnline;//客户在线表
	CTbl_Cust_Tradecode  m_pTradecode;//资金账号交易编码绑定表
	CTbl_Exch_Trader     m_pExchTrader;//交易所席位交易员在线表

	CTbl_Fund            m_pFund; //资金表
	CTbl_FundDetail      m_pFundDetail;//资金明细表
	CTbl_Position        m_pPosition;//持仓表
	CTbl_Storage         m_pStore;//库存表

	CTbl_Order           m_pOrder;//报单表
	CTbl_Done            m_pDone;//成交表
	CTbl_Fund_Flow       m_pFundFlow;//出入金
	CTbl_Quotation       m_pQuotation;//实时行情表
	
	CTbl_RiskContractInfo    m_pRiskContractInfo;//风险合约信息表
	CTbl_Cust_Posi_Risk      m_pCustPosiRisk;//客户持仓风险表

	//订阅表
	CTbl_Oper_Subcribe       m_pSubcribe;//操作员订阅表
	CTbl_Prod_Fund_Risk      m_pProdFundRisk;//产品资金风险
	CTbl_Prod_Posi_Risk      m_pProdPosiRisk;//产品持仓风险
	
	CTbl_Big_Margin          m_pBigNetMargin;//大边净边保证金表

	CTbl_Diff_Posi           m_pDiffPosi;//持仓轧差表

	CTbl_Risk_Tick           m_pRiskTick;//日间风险表

	CTbl_Org_Fund_Risk      m_pOrgFundRisk;//机构资金风险表
	CTbl_Org_Posi_Risk      m_pOrgPosiRisk;//机构持仓风险表
};

#endif // !defined(AFX_DB_TRADER_H__6EECD875_4F24_4FFA_9024_3DEBCD57D140__INCLUDED_)
