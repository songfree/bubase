// InitTrade.h: interface for the CInitTrade class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INITTRADE_H__CDA96C9F_53BB_4389_A85C_ED688807B76C__INCLUDED_)
#define AFX_INITTRADE_H__CDA96C9F_53BB_4389_A85C_ED688807B76C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "IInitial.h"
#include "KernelPub.h"

#ifdef _WINDOWS
#ifdef _DEBUG
#pragma comment(lib, "bf_kerneld.lib") 
#pragma comment(lib, "bf_dbapid.lib") 
#pragma comment(lib, "busidlld.lib")
#pragma message("Automatically linking with   bf_kerneld.lib bf_dbapid.lib busidlld.lib")
#else
#pragma comment(lib, "bf_kernel.lib") 
#pragma comment(lib, "bf_dbapi.lib") 
#pragma comment(lib, "busidll.lib")
#pragma message("Automatically linking with   bf_kernel.lib bf_dbapi.lib busidll.lib")
#endif

#endif


class DLL_EXPORT CInitTrade : public CIInitial  
{
public:
	

	// 函数名: ReadQuotation
	// 编程  : 王明松 2018-3-6 16:16:33
	// 返回  : bool 
	// 描述  : 从数据库表读行情
	bool ReadQuotation();

	// 函数名: ReadFundFlow
	// 编程  : 王明松 2018-3-6 16:16:33
	// 返回  : bool 
	// 描述  : 从数据库表读出入金
	bool ReadFundFlow();

	// 函数名: ReadDoneMatch
	// 编程  : 王明松 2018-3-6 16:16:33
	// 返回  : bool 
	// 描述  : 从数据库表读成交
	bool ReadDoneMatch();

	// 函数名: ReadOrder
	// 编程  : 王明松 2018-3-6 16:16:33
	// 返回  : bool 
	// 描述  : 从数据库表读委托
	bool ReadOrder();

	// 函数名: ReadCustStorage
	// 编程  : 王明松 2018-3-6 16:16:33
	// 返回  : bool 
	// 描述  : 从数据库表读库存
	bool ReadCustStorage();

	// 函数名: ReadCustPosition
	// 编程  : 王明松 2018-3-6 16:16:33
	// 返回  : bool 
	// 描述  : 从数据库表读持仓
	bool ReadCustPosition();

	// 函数名: ReadCustFundDetail
	// 编程  : 王明松 2018-3-6 16:16:33
	// 返回  : bool 
	// 描述  : 从数据库表读资金详细
	bool ReadCustFundDetail();

	// 函数名: ReadCustFund
	// 编程  : 王明松 2018-3-6 16:16:33
	// 返回  : bool 
	// 描述  : 从数据库表读资金
	bool ReadCustFund();

	// 函数名: ReadExchTradeOnline
	// 编程  : 王明松 2018-3-6 16:16:33
	// 返回  : bool 
	// 描述  : 从数据库表读交易所交易员在线
	bool ReadExchTradeOnline();

	// 函数名: ReadTradeCode
	// 编程  : 王明松 2018-3-6 16:16:33
	// 返回  : bool 
	// 描述  : 从数据库表读交易编码
	bool ReadTradeCode();

	// 函数名: ReadCustOnline
	// 编程  : 王明松 2018-3-6 16:16:33
	// 返回  : bool 
	// 描述  : 从数据库表读客户在线
	bool ReadCustOnline();

	// 函数名: InitTrader
	// 编程  : 王明松 2018-3-6 16:16:33
	// 描述  : 初始化交易信息
	bool InitTrader();

	// 函数名: ReadCustFeeDetail
	// 编程  : 王明松 2018-3-6 16:16:33
	// 返回  : bool 
	// 描述  : 从数据库读客户特殊费率
	bool ReadCustFeeDetail();

	// 函数名: ReadFeeTempletDetail
	// 编程  : 王明松 2018-3-6 16:16:33
	// 返回  : bool 
	// 描述  : 从数据库读费率模板详细设置
	bool ReadFeeTempletDetail();

	// 函数名: ReadFeeTemplet
	// 编程  : 王明松 2018-3-6 16:16:33
	// 返回  : bool 
	// 描述  : 从数据库读费率模板
	bool ReadFeeTemplet();

	bool ReadCustPosiDiff();

	// 函数名: ReadTxaccTemplet
	// 编程  : 王明松 2018-3-6 16:16:33
	// 返回  : bool 
	// 描述  : 从数据库读客户对应的风险模板
	bool ReadTxaccTemplet();

	// 函数名: ReadPosiTempletDetail
	// 编程  : 王明松 2018-3-6 16:16:33
	// 返回  : bool 
	// 描述  : 从数据库读持仓模板明细
	bool ReadPosiTempletDetail();

	// 函数名: ReadFundTempletDetail
	// 编程  : 王明松 2018-3-6 16:16:33
	// 返回  : bool 
	// 描述  : 从数据库读资金模板明细
	bool ReadFundTempletDetail();

	// 函数名: ReadPosiTemplet
	// 编程  : 王明松 2018-3-6 16:16:33
	// 返回  : bool 
	// 描述  : 从数据库读持仓模板
	bool ReadPosiTemplet();

	// 函数名: ReadPosiTemplet
	// 编程  : 王明松 2018-3-6 16:16:33
	// 返回  : bool 
	// 描述  : 从数据库读资金模板
	bool ReadFundTemplet();

	// 函数名: ReadProdPosiMonitor
	// 编程  : 王明松 2018-3-6 16:16:33
	// 返回  : bool 
	// 描述  : 从数据库读产品持仓模板
	bool ReadProdPosiMonitor();

	// 函数名: ReadProdFundMonitor
	// 编程  : 王明松 2018-3-6 16:16:33
	// 返回  : bool 
	// 描述  : 从数据库读产品资金模板
	bool ReadProdFundMonitor();

	// 函数名: ReadRiskContractSet
	// 编程  : 王明松 2018-3-6 16:16:33
	// 返回  : bool 
	// 描述  : 从数据库读风险合约设置
	bool ReadRiskContractSet();

	// 函数名: ReadRiskContractSet
	// 编程  : 王明松 2018-3-6 16:16:33
	// 返回  : bool 
	// 描述  : 从数据库读产品机构对应模板
	bool ReadProdOrgTemplet();
	// 函数名: ReadWarnLevel
	// 编程  : 王明松 2018-3-6 16:16:33
	// 返回  : bool 
	// 描述  : 从数据库读预警设置
	bool ReadWarnLevel();

	// 函数名: ReadOptionModel
	// 编程  : 王明松 2018-3-6 16:16:33
	// 返回  : bool 
	// 描述  : 从数据库表读取期权价格模型
	bool ReadOptionModel();

	// 函数名: InitSet
	// 编程  : 王明松 2018-3-6 16:16:33
	// 返回  : bool 
	// 描述  : 初始化设置
	bool InitSet();


	// 函数名: ReadFundAcc
	// 编程  : 王明松 2018-5-31 16:00:28
	// 返回  : bool 
	// 描述  : 读资金账号
	bool ReadFundAcc();

	// 函数名: ReadCustom
	// 编程  : 王明松 2018-5-31 16:00:44
	// 返回  : bool 
	// 描述  : 读客户信息
	bool ReadCustom();

	// 函数名: InitCust
	// 编程  : 王明松 2018-5-31 16:00:55
	// 返回  : bool 
	// 描述  : 初始化客户信息
	bool InitCust();
	

	// 函数名: InitBaseInfo
	// 编程  : 王明松 2018-5-31 16:01:08
	// 返回  : bool 
	// 描述  : 初始化基本信息
	bool InitBaseInfo();

	// 函数名: ReadVariety
	// 编程  : 王明松 2018-5-31 16:01:31
	// 返回  : bool 
	// 描述  : 从数据库读品种
	bool ReadVariety();

	// 函数名: ReadContract
	// 编程  : 王明松 2018-5-31 16:01:43
	// 返回  : bool 
	// 描述  : 从数据库读合约
	bool ReadContract();

	// 函数名: ReadMarket
	// 编程  : 王明松 2018-5-31 16:01:56
	// 返回  : bool 
	// 描述  : 从数据库读市场信息
	bool ReadMarket();

	// 函数名: ReadExchange
	// 编程  : 王明松 2018-5-31 16:02:09
	// 返回  : bool 
	// 描述  : 从数据库读交易所
	bool ReadExchange();

	// 函数名: ReadProd
	// 编程  : 王明松 2018-5-31 16:02:22
	// 返回  : bool 
	// 描述  : 从数据库读产品
	bool ReadProd();

	// 函数名: ReadOperTxacc
	// 编程  : 王明松 2018-5-31 16:02:33
	// 返回  : bool 
	// 描述  : 从数据库读操作员资金账号对应
	bool ReadOperTxacc();

	// 函数名: ReadProdTxacc
	// 编程  : 王明松 2018-5-31 16:02:55
	// 返回  : bool 
	// 描述  : 从数据库读产品对应资金账号
	bool ReadProdTxacc();

	// 函数名: ReadOperProd
	// 编程  : 王明松 2018-5-31 16:03:11
	// 返回  : bool 
	// 描述  : 从数据库读操作员对应产品
	bool ReadOperProd();

	// 函数名: ReadContractLimit
	// 编程  : 王明松 2018-5-31 16:03:27
	// 返回  : bool 
	// 描述  : 从数据库读冷门合约开仓设置
	bool ReadContractLimit();

	// 函数名: ReadSettlePrice
	// 编程  : 王明松 2018-5-31 16:05:15
	// 返回  : bool 
	// 描述  : 从数据库读合约结算价
	bool ReadSettlePrice();

	// 函数名: ReadSpecDate
	// 编程  : 王明松 2018-5-31 16:05:34
	// 返回  : bool 
	// 描述  : 从数据库读节假日配置表
	bool ReadSpecDate();


	// 函数名: InitPublic
	// 编程  : 王明松 2018-5-31 16:05:50
	// 返回  : bool 
	// 描述  : 初始化公共信息
	bool InitPublic();

	// 函数名: ReadPara
	// 编程  : 王明松 2018-5-31 16:06:01
	// 返回  : bool 
	// 描述  : 从数据库读系统参数
	bool ReadPara();

	// 函数名: ReadOper
	// 编程  : 王明松 2018-5-31 16:06:15
	// 返回  : bool 
	// 描述  : 从数据库读操作员信息
	bool ReadOper();

	// 函数名: ReadOrgan
	// 编程  : 王明松 2018-5-31 16:06:29
	// 返回  : bool 
	// 描述  : 从数据库读机构信息
	bool ReadOrgan();

	// 函数名: ReadOperOnline
	// 编程  : 王明松 2018-5-31 16:06:41
	// 返回  : bool 
	// 描述  : 从数据库读操作员在线信息
	bool ReadOperOnline();

	// 函数名: ReadBrokerageInfo
	// 编程  : 王明松 2018-5-31 16:06:41
	// 返回  : bool 
	// 描述  : 从数据库读经纪信息
	bool ReadBrokerageInfo();
	

	// 函数名: ConnectLocalDb
	// 编程  : 王明松 2018-5-31 16:06:54
	// 返回  : bool 
	// 描述  : 连接本地数据库
	bool ConnectLocalDb();
	
	// 函数名: ProcCustFund
	// 编程  : 王明松 2018-3-6 16:16:33
	// 返回  : bool 
	// 描述  : 处理客户的大边保证金、成交、报单、出入金
	bool ProcCustFund();

	// 函数名: ProcOneCust
	// 编程  : 王明松 2018-3-6 16:16:33
	// 返回  : bool 
	// 参数  : int rid  资金账户锁的rowid，根据此rowid可找到对应的资金账户表、资金表、客户信息表等
	// 描述  : 处理一个客户的持仓、成交、报单、出入金
	bool ProcOneCust(int rid);

	CInitTrade();
	virtual ~CInitTrade();
	
	virtual bool Init(CIErrlog *log,CBF_Xml *xml,CMemDb *mdb);

	CKernelPub  m_pKernelPub;
protected:

	CInt     m_keyCustRowid;
	S_MDB_CUST_FUNDACC_INFO *m_pTmpCustTxacc;
	char     m_sSql[500];
	char     m_sErrMsg[256];

	CBF_Date_Time m_pDate;
public:
	
};
//注意：下面的定义在每个类里面都要有，主程序根据此方法获取本类的实例
extern "C" 
{ 
    DLL_EXPORT CInitTrade * getInstance() ; 
}

#endif // !defined(AFX_INITTRADE_H__CDA96C9F_53BB_4389_A85C_ED688807B76C__INCLUDED_)
