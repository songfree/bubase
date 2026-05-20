// RiskDll.h: interface for the RiskDll class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RISKDLL_H__82682589_B43E_4DCC_B644_EC0A4A5D19DB__INCLUDED_)
#define AFX_RISKDLL_H__82682589_B43E_4DCC_B644_EC0A4A5D19DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IProcBase.h"
#include "KernelPub.h"
#include "Xdp2Struct.h"
#include "PubDb.h"
#include "PubMethod.h"

#ifdef _WINDOWS
#ifdef _DEBUG
#pragma comment(lib, "bf_kerneld.lib") 
#pragma comment(lib, "bf_dbapid.lib") 
#pragma comment(lib, "bf_xdpd.lib") 
#pragma message("Automatically linking with   bf_kerneld.lib bf_dbapid.lib bf_xdpd.lib")
#else
#pragma comment(lib, "bf_kernel.lib") 
#pragma comment(lib, "bf_dbapi.lib") 
#pragma comment(lib, "bf_xdp.lib") 
#pragma message("Automatically linking with   bf_kernel.lib bf_dbapi.lib bf_xdp.lib")
#endif

#endif

#ifdef KERNELPUB_EXPORTS
#define KERNELPUB_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNELPUB_EXPORT __declspec(dllimport)
#else
#define KERNELPUB_EXPORT 
#endif
#endif

class KERNELPUB_EXPORT RiskDll : public CIProcBase  
{
public:

	//处理外部发来的普通报单风控检查
	int OrderRiskCheck(S_TRADE_DATA& data);
	//处理外部发来的双边报价风控检查
	int BilateralOrderRiskCheck(S_TRADE_DATA& data);
	//处理外部发来的资金同步信息
	int ProcSyncFund(S_TRADE_DATA& data);
	//处理外部发来的持仓同步信息
	int ProcSyncPosi(S_TRADE_DATA& data);
	//处理外部发来的债券存续规模
    int ProcBondScope(S_TRADE_DATA& data);
	//处理外部发来的债券估值
    int ProcBondValue(S_TRADE_DATA& data);
	//处理外部发来的DV01数据
    int ProcBondDv01(S_TRADE_DATA& data);

	
	// 函数名: OrderMsg
	// 编程  : 王明松 2018-6-1 16:46:18
	// 返回  : int 
	// 参数  : S_TRADE_DATA &data
	// 描述  : 处理外部发来的报单通知,不校验资金仓位,默认为已报入且不调用外部接口
	int OrderMsg(S_TRADE_DATA &data);

	//处理外部发来的成交回报，计算资金持仓
	int DoneRtn(S_TRADE_DATA& data);
	//外理外部发来的撤单回报消息，处理撤单后的资金持仓
	int CancelOrderRtn(S_TRADE_DATA& data);

	// 函数名: QueryCustOrder
	// 编程  : 王明松 2018-6-1 16:47:10
	// 返回  : int 
	// 参数  : S_TRADE_DATA &data
	// 描述  : 查询客户报单
	int QueryCustOrder(S_TRADE_DATA &data);

	// 函数名: RcvQuotation
	// 编程  : 王明松 2018-3-6 16:16:08
	// 返回  : int 
	// 参数  : S_TRADE_DATA &data
	// 描述  : 接收行情处理
	int RcvQuotation(S_TRADE_DATA& data);
	//导入证券信息
	int ImportSecu(S_TRADE_DATA& data);

	RiskDll();
	virtual ~RiskDll();

	virtual bool Init(CBF_Xml *xml,CIClientLink *pBusLink,CMemDb *pMemDb,	CXdp *pXdp,	CIErrlog *pLog,CDbApi *pSqlCmd,CIExchLink *pExchLink,int index);

	CBF_Date_Time m_pDate;
	

	CKernelPub   m_pKernelPub;//资金持仓等的公共操作库

	S_TRADE_DATA  m_pSendData;

	INT64_   m_nBeginTime;//开始时间
	INT64_   m_nEndTime;  //结束日期

	//定时任务，计算风险
	int ComputeRisk(S_TRADE_DATA& data);
	//定时任务，同步至DB
	int Sync2Db(S_TRADE_DATA& data);
	//定时任务，处理未找到报单的成交
	int ProcessDoneNoOrder(S_TRADE_DATA& data);
protected:
	int      m_nCallSerial;//异步流水

	CPubDb   m_pPubDb;//数据库的公共操作
	CPubMethod m_pPubMethod;

    // 函数名: OrderCheck
    // 编程  : 王明松 2022-7-6 16:16:08
    // 返回  : int 
    // 参数  : S_MDB_ORDER_INFO &order	   报单信息
    // 参数  : S_ORDER_PUB &order_pub		 检查返回的账户、合约等信息指针
    // 描述  : 报单输入检查及风控检查
	int OrderCheck(S_MDB_ORDER_INFO & order,S_ORDER_PUB &order_pub);

    // 函数名: OrderRsp
    // 编程  : 王明松 2022-7-6 16:16:08
    // 返回  : int 
    // 参数  : S_TRADE_DATA &rpcdata	   原报单信息
    // 参数  : S_TRADE_DATA &ansdata	   报单响应信息
    // 描述  : 报单响应
    int OrderRsp(S_TRADE_DATA& rpcdata, S_TRADE_DATA& ansdata);

    // 函数名: GetPosiTotalValue
    // 编程  : 王明松 2022-7-7 10:55:08
    // 返回  : double 持仓总规模 
    // 参数  : const char* stxacc	交易账号
    // 描述  : 获取交易账号的持仓总规模  债券按面值，其余按最新价
	double GetPosiTotalValue(const char* stxacc);
	double GetProdPosiTotalValue(const char* Org);
	double GetOrgPosiTotalValue(const char* Org);

	int GetNextSerial();
	int GetCurSerial();

	// 函数名: SendBus
	// 编程  : 王明松 2018-10-24 16:16:08
	// 返回  : int 
	// 参数  : CXdp *xdp  xd报文
	// 参数  : int svrid  接收方在总线上注册的服务号
	// 参数  : int funcno  交易码
	// 参数  : int serial 异步流水
	// 参数  : short cmd 总线命令字
	// 描述  : 接收行情处理
	int SendBus(CXdp *xdp,int svrid,int funcno,int serial,short cmd);

	void SendMoneyIOPush(S_MDB_TXACC_LOCK *acclock ,S_MDB_FUND_FLOW *info);
	void SendDoneBroadcast(S_MDB_TXACC_LOCK *acclock ,S_MDB_DONE_INFO *info);
	void SendOrderBroadcast(S_MDB_TXACC_LOCK *acclock ,S_MDB_ORDER_INFO *info);

	void GenAcc(const char *oldacc, int syscode,int destlen, char *txacc);

	bool ComputeProd(const char *compdate);
	bool ComputeOrg(const char *compdate);
	bool ComputeOneOrg(const char* compdate, char* orgcode, const char* orgname, const char* risktype, std::vector<S_MDB_TRADEACC_INFO*> orgcustlist);
	
	//同步成交
	bool SyncDone(S_MDB_DONE_INFO *done);
	//同步报单
	bool SyncOrder(S_MDB_ORDER_INFO *order);
	//同步债券扩展信息，如存续规模，DV01等
	bool SyncBond(S_MDB_BOND_INFO* bond);

	bool CalcRiskInfo(char *txacc,S_MDB_CONTRACT_INFO *coninfo,double newprice,S_MDB_CUST_FUND &fund);

	bool ReadOneCustInfo(const char *txacc);

	// 函数名: ReadInfoFromDbByExch
	// 编程  : 王明松 2018-3-30 16:00:03
	// 返回  : bool 
	// 参数  : char *exch
	// 描述  : 按交易所读入信息
	bool ReadInfoFromDbByExch(char *exch);

	// 函数名: ReadInfoFromDbByMarket
	// 编程  : 王明松 2018-3-30 15:59:39
	// 返回  : bool 
	// 参数  : char *market
	// 描述  : 按市场读入信息
	bool ReadInfoFromDbByMarket(char *market);

	// 函数名: ResetCustInfo
	// 编程  : 王明松 2018-3-30 15:59:22
	// 返回  : bool 
	// 参数  : CTbl_Char_Key &custlist
	// 描述  : 重置客户信息
	bool ResetCustInfo(CTbl_Char_Key &custlist);

	// 函数名: ChgDateProcByExchange
	// 编程  : 王明松 2018-3-30 15:59:07
	// 返回  : bool 
	// 参数  : char *exchange
	// 参数  : char *curdate 当前交易日
	// 描述  : 交易所日切处理
	bool ChgDateProcByExchange(char *exchange,char *curdate);

	// 函数名: ChgDateProcByMarket
	// 编程  : 王明松 2018-3-30 15:58:50
	// 返回  : bool 
	// 参数  : char *market
	// 参数  : char *curdate 当前交易日
	// 描述  : 市场日切处理
	bool ChgDateProcByMarket(char *market,char *curdate);

	bool DeleteMoneyIO(CTbl_Char_Key &custlist,char *curdate);

	// 函数名: UpdateOrder2Db
	// 编程  : 王明松 2018-6-1 16:48:07
	// 返回  : bool 
	// 参数  : S_MDB_ORDER_INFO *porder
	// 描述  : 更新委托至数据库
	bool UpdateOrder2Db(S_MDB_ORDER_INFO *porder);

	// 函数名: ComputeNewOrder
	// 编程  : 王明松 2018-2-28 10:12:41
	// 返回  : bool 
	// 参数  : S_MDB_TXACC_LOCK *lockinfo
	// 参数  : S_MDB_CUST_FUND *fundinfo
	// 参数  : S_MDB_ORDER_INFO *info   报单信息，手续费 保证金 金额 要提前算好
	// 参数  : S_MDB_CONTRACT_INFO *pcinfo 
	// 参数  : char *errmsg
	// 描述  : 计算一个新的报单  更新资金及持仓
	bool ComputeNewOrder(S_MDB_TXACC_LOCK *lockinfo,S_MDB_CUST_FUND *fundinfo,S_MDB_ORDER_INFO *info,S_MDB_CONTRACT_INFO *pcinfo ,char *errmsg);

	// 函数名: ComputeNewDone
	// 编程  : 王明松 2018-2-28 10:17:35
	// 返回  : bool 
	// 参数  : S_MDB_TXACC_LOCK *lockinfo
	// 参数  : S_MDB_CUST_FUND *fundinfo
	// 参数  : S_MDB_DONE_INFO *info
	// 参数  : S_MDB_ORDER_INFO *po
	// 参数  : S_MDB_CONTRACT_INFO *pc
	// 参数  : char *errmsg
	// 描述  : 计算一个新的成交 更新资金及持仓
	bool ComputeNewDone(S_MDB_TXACC_LOCK *lockinfo,S_MDB_CUST_FUND *fundinfo,S_MDB_DONE_INFO *info,S_MDB_ORDER_INFO *po,S_MDB_CONTRACT_INFO *pc,char *errmsg);

	// 函数名: SendRiskMsg
	// 编程  : 王明松 2018-6-1 16:50:19
	// 返回  : void 
	// 参数  : S_MDB_TXACC_LOCK *acc
	// 参数  : S_MDB_CUST_FUND *fund
	// 参数  : char traderight
	// 描述  : 发送交易账户资金风险信息
	void SendRiskMsg(S_MDB_TXACC_LOCK *acc,S_MDB_CUST_FUND *fund,char traderight);

	void AnsData(S_TRADE_DATA &data);
	
	char m_sSql[8000];

	// 函数名: ComputeSurplus
	// 编程  : 王明松 2018-3-6 16:17:53
	// 返回  : void 
	// 参数  : S_MDB_QUOTATION info
	// 参数  : S_MDB_CONTRACT_INFO *pc
	// 参数  : S_MDB_CUST_POSITION *posi
	// 描述  : 计算一个持仓的盈亏
	void ComputeSurplus(S_MDB_QUOTATION info,S_MDB_CONTRACT_INFO *pc,S_MDB_CUST_POSITION *posi);

	// 函数名: ProcRisk
	// 编程  : 王明松 2018-3-6 16:17:20
	// 返回  : void 
	// 参数  : S_MDB_QUOTATION info
	// 参数  : S_MDB_CONTRACT_INFO *pc
	// 描述  : 计算最新行情的持仓盈亏
	void ProcRisk(S_MDB_QUOTATION info,S_MDB_CONTRACT_INFO *pc);
	
	// 函数名: ProcRiskContract
	// 编程  : 王明松 2018-3-6 16:16:44
	// 返回  : void 
	// 参数  : S_MDB_QUOTATION qinfo
	// 参数  : S_MDB_CONTRACT_INFO *pc
	// 描述  : 处理风险合约
	void ProcRiskContract(S_MDB_QUOTATION qinfo,S_MDB_CONTRACT_INFO *pc);

	// 函数名: UpdateSetllePrice
	// 编程  : 王明松 2018-3-6 16:16:23
	// 返回  : void 
	// 参数  : S_MDB_QUOTATION qinfo
	// 描述  : 更新数据库结算价
	void UpdateSetllePrice(S_MDB_QUOTATION qinfo);

	// 函数名: GetAccList
	// 编程  : 王明松 2018-3-6 17:12:50
	// 返回  : bool 
	// 参数  : std::vector<int> &acclist
	// 描述  : 取得有浮动盈亏的客户列表
	bool GetAccList(std::vector<int> &acclist);

	// 函数名: ComputeCustRisk
	// 编程  : 王明松 2018-3-6 17:12:39
	// 返回  : void 
	// 参数  : int rowid
	// 描述  : 计算一个客户的风险
	void ComputeCustRisk(int rowid);

	// 函数名: ComputeCustPosiRisk
	// 编程  : 王明松 2018-3-12 16:16:28
	// 返回  : void 
	// 参数  : int rowid
	// 描述  : 计算一个客户的持仓风险
	void ComputeCustPosiRisk(int rowid);

	// 函数名: ProcessDone
	// 编程  : 王明松 2018-6-1 16:52:36
	// 返回  : int 
	// 参数  : S_MDB_CUST_FUNDACC_INFO *pcustacc
	// 参数  : S_MDB_ORDER_INFO *po
	// 参数  : S_MDB_CONTRACT_INFO *pc
	// 参数  : S_MDB_EXCH_INFO *pexch
	// 参数  : S_MDB_DONE_INFO &info
	// 描述  : 处理成交
	int ProcessDone(S_MDB_TRADEACC_INFO *pcustacc ,S_MDB_ORDER_INFO *po,S_MDB_CONTRACT_INFO *pc,S_MDB_EXCH_INFO *pexch,S_MDB_DONE_INFO &info);


	//加锁更新资金
	bool UpdateFundLock(const char *txacc,char *currency,const char fundflag,const  char *fundcode,double amt,bool isadd,int &lockrowid);
	//检查黑白名单
	int CheckBwgMenu(S_MDB_ORDER_INFO *order,char *msg);
	//检查价格限定
	int CheckPriceLimit(S_MDB_ORDER_INFO* order, S_MDB_CONTRACT_INFO* pc, char* msg);
	//检查风险合约
	int CheckRiskContract(S_MDB_ORDER_INFO* order, S_MDB_CONTRACT_INFO* pc, char* msg);
public:
	void MsgCustRiskCalc(int lockrowid);
	

	// 函数名: Order
	// 编程  : 王明松 2018-3-6 16:16:08
	// 返回  : int 
	// 参数  : S_TRADE_DATA &data
	// 描述  : 接收外部的报单，进行资金仓位校验及处理，默认为未报，通过后调用外部的报单接口
	int Order(S_TRADE_DATA &data);
	//接收外部的双边报价，调用外部的双边报价接口
	int BilateralOrder(S_TRADE_DATA& data);
	//接收外部的撤单请求，调用外部的撤单接口
	int CancelOrder(S_TRADE_DATA& data);
	//对应Order的报单，收到外部系统的报单回报，进行回报的处理
    int OrderRtn(S_TRADE_DATA& data);

	//处理主机同步过来的报单信息
	int OrderSync(S_TRADE_DATA& data);


	int test1021(S_TRADE_DATA& data);
	int test1023(S_TRADE_DATA& data);

	int m_nRpcSerial;
};
//注意：下面的定义在每个类里面都要有，主程序根据此方法获取本类的实例
extern "C" 
{ 
	KERNELPUB_EXPORT RiskDll * getInstance() ;
}
#endif // !defined(AFX_RISKDLL_H__82682589_B43E_4DCC_B644_EC0A4A5D19DB__INCLUDED_)
