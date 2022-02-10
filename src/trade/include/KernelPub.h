// KernelPub.h: interface for the CKernelPub class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KERNELPUB_H__1E64E06C_1D5F_41AB_8FB8_A16C19C39070__INCLUDED_)
#define AFX_KERNELPUB_H__1E64E06C_1D5F_41AB_8FB8_A16C19C39070__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MemDb.h"
#include "IErrLog.h"
#include "BF_Date_Time.h"

#ifdef KERNELPUB_EXPORTS
#define KERNELPUB_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNELPUB_EXPORT __declspec(dllimport)
#else
#define KERNELPUB_EXPORT 
#endif
#endif

class KERNELPUB_EXPORT CKernelPub  
{
public:
	CKernelPub();
	virtual ~CKernelPub();
	
	CMemDb         *m_pMemDb;//内存表
	CIErrlog       *m_pLog;

	// 函数名: ProcPosition
	// 编程  : 王明松 2018-3-6 16:16:33
	// 返回  : bool 
	// 参数  : S_MDB_TXACC_LOCK *acclock
	// 参数  : std::vector<S_MDB_CUST_POSITION *> &posilist
	// 描述  : 处理一个客户的持仓，生成大边净边信息，若是导入的客户，将大边保证金记入资金详细
	bool ProcPosition(S_MDB_TXACC_LOCK *acclock,std::vector<S_MDB_CUST_POSITION *> &posilist);
	
	
	
	// 函数名: ProcDone
	// 编程  : 王明松 2018-3-6 16:16:33
	// 返回  : bool 
	// 参数  : S_MDB_TXACC_LOCK *acclock
	// 参数  : std::vector<S_MDB_DONE_INFO *> &donelist
	// 描述  : 处理一个客户的成交，逐笔计算资金持仓保证金，不处理冻结解冻
	bool ProcDone(S_MDB_TXACC_LOCK *acclock,std::vector<S_MDB_DONE_INFO *> &donelist);
	
	// 函数名: ProcOrder
	// 编程  : 王明松 2018-3-6 16:16:33
	// 返回  : bool 
	// 参数  : S_MDB_TXACC_LOCK *acclock
	// 参数  : std::vector<S_MDB_ORDER_INFO *> &orderlist
	// 描述  : 处理一个客户的报单，逐笔计算资金冻结证金  对剩余量进行计算
	bool ProcOrder(S_MDB_TXACC_LOCK *acclock,std::vector<S_MDB_ORDER_INFO *> &orderlist);
	
	// 函数名: ProcFundFlow
	// 编程  : 王明松 2018-3-6 16:16:33
	// 返回  : bool 
	// 参数  : S_MDB_TXACC_LOCK *acclock
	// 参数  : std::vector<S_MDB_ORDER_INFO *> &orderlist
	// 描述  : 处理一个客户的出入金，计算资金
	bool ProcFundFlow(S_MDB_TXACC_LOCK *acclock,std::vector<S_MDB_FUND_FLOW *> &flowlist);
	
	// 函数名: GetMarginValue
	// 编程  : 王明松 2018-3-21 17:26:13
	// 返回  : bool 
	// 参数  : S_MDB_CONTRACT_INFO *pc
	// 参数  : S_MDB_CUST_FUNDACC_INFO *pcust
	// 参数  : int flag  0期货买开卖开保证金  1期权卖开保证金
	// 参数  : int num
	// 参数  : double amt
	// 输出参数  : double &exmargin  交易所保证金
	// 输出参数  : double &membermargin   会员保证金
	// 描述  : 取保证金
	bool GetMarginValue(S_MDB_CONTRACT_INFO *pc, S_MDB_CUST_FUNDACC_INFO *pcust,int flag ,int num, double amt, double &exmargin, double &membermargin);

	int GetRiskLevel(S_MDB_CUST_FUND *fund,char *traderight);

	// 函数名: GetPosiRiskLevel
	// 编程  : 王明松 2018-6-1 16:48:44
	// 返回  : int 
	// 参数  : S_MDB_TXACC_LOCK *acclock
	// 参数  : S_MDB_CUST_TEMPLET_RELA *templet
	// 参数  : S_MDB_CUST_POSITION *custposi
	// 参数  : char *traderight
	// 参数  : char *warnmsg
	// 描述  : 取持仓风险级别
	int GetPosiRiskLevel(S_MDB_TXACC_LOCK *acclock,S_MDB_CUST_TEMPLET_RELA *templet,S_MDB_CUST_POSITION *custposi,char *traderight,char *warnmsg);

	int GetProdRiskLevel(char *prodid,double unitvalue,double risk,char *traderight,char *warnmsg);
	int GetProdRiskLevel(char *prodid,double marginrate,double posivalue,double posidiffvalue,char *traderight,char *warnmsg);

	int GetOrgRiskLevel(char *orgcode,double unitvalue,double risk,char *traderight,char *warnmsg);
	int GetOrgRiskLevel(char *orgcode,double marginrate,double posivalue,double posidiffvalue,char *traderight,char *warnmsg);

	// 函数名: GetTradeFeeValue
	// 编程  : 王明松 2018-3-21 14:08:49
	// 返回  : bool 
	// 参数  : S_MDB_CONTRACT_INFO *pc    合约信息
	// 参数  : S_MDB_CUST_FUNDACC_INFO *pcust  资金账号信息
	// 参数  : int flag   买卖开平标志  1买入开仓 2卖出平仓 3平今仓
	// 参数  : int num   数量(手)
	// 参数  : double amt  金额
	// 输出参数  : double &exfee  交易所手续费
	// 输出参数  : double &memberfee   会员手续费
	// 描述  : 取交易手续费
	bool GetTradeFeeValue(S_MDB_CONTRACT_INFO *pc ,S_MDB_CUST_FUNDACC_INFO *pcust,int flag,int num,double amt,double &exfee,double &memberfee);

	// 函数名: CheckRight
	// 编程  : 王明松 2018-6-1 16:49:32
	// 返回  : bool 
	// 参数  : char *custorg
	// 参数  : char *operorg
	// 描述  : 检查权限
	bool CheckRight(char *custorg,char *operorg);

	// 函数名: CheckOperProdRight
	// 编程  : 王明松 2018-6-1 16:49:01
	// 返回  : bool 
	// 参数  : const char *oper
	// 参数  : const char *prod
	// 描述  : 检查操作员产品权限
	bool CheckOperProdRight(const char *oper, const char *prod);

	// 函数名: CheckOperAccRight
	// 编程  : 王明松 2018-6-1 16:49:14
	// 返回  : bool 
	// 参数  : const char *oper
	// 参数  : const char *acc
	// 描述  : 检查操作员资金账号权限
	bool CheckOperAccRight(const char *oper,const char *acc);

	bool CheckOperOrgRight(const char *oper,const char *org);
	
	// 函数名: UpdateFund
	// 编程  : 王明松 2018-2-23 13:03:25
	// 返回  : bool 
	// 参数  : S_MDB_TXACC_LOCK *lockinfo
	// 参数  : S_MDB_CUST_FUND *fundinfo
	// 参数  : const char fundflag   资金类型
	// 参数  : const  char *fundcode  资金编号
	// 参数  : double amt  金额   正负区分
	// 参数  : bool isadd   true 和原来的值直接相加，false使用最新值替换原有值。
	// 参数  : bool islock   是否加锁
	// 描述  : 更新资金
	bool UpdateFund(S_MDB_TXACC_LOCK *info,S_MDB_CUST_FUND *fund,const char fundflag,const  char *fundcode,double amt,bool isadd,bool islock=false);

	// 函数名: GetLastTradeDate
	// 编程  : 王明松 2018-5-31 19:41:40
	// 返回  : int <0失败  >0交易日
	// 参数  : const char *market  市场
	// 参数  : const char *excode  交易所
	// 参数  : const char *curdate  当前交易日
	// 参数  : int lastn   前5个交易日，为6。即最后一个交易日的前的个交易日
	// 描述  : 取上几个交易日
	int GetLastTradeDate(const char *market,const char *excode,const char *curdate,int lastn);
	
	// 函数名: GetMarginMode
	// 编程  : 王明松 2018-5-31 19:41:40
	// 返回  : int <0失败  0正常双边  1 净边   2大边 3非保证金 非期货
	// 参数  : S_MDB_CONTRACT_INFO *pc  市场
	// 描述  : 根据合约取保证金模式
	int GetMarginMode(S_MDB_CONTRACT_INFO *pc);
	
	// 函数名: ProcOrder
	// 编程  : 王明松 2018-3-6 16:16:33
	// 返回  : int  0成功 其它失败  >0返回码
	// 参数  : S_MDB_TXACC_LOCK *lockinfo  客户锁
	// 参数  : S_MDB_CUST_FUND *fundinfo   资金表
	// 参数  : S_MDB_ORDER_INFO *info      报单信息
	// 参数  : S_MDB_CONTRACT_INFO *pc     合约信息
	// 参数  : bool checkfund=false  是否检查资金及仓位
	// 描述  : 处理一笔报单，计算资金冻结证金
	int ComputeNewOrder(S_MDB_TXACC_LOCK *lockinfo,S_MDB_CUST_FUND *fundinfo,S_MDB_ORDER_INFO *info,S_MDB_CONTRACT_INFO *pc,bool checkfund=false);

	bool ComputeNewDone(S_MDB_TXACC_LOCK *lockinfo,S_MDB_CUST_FUND *fundinfo,S_MDB_DONE_INFO *info,S_MDB_ORDER_INFO *po,S_MDB_CONTRACT_INFO *pc);

	int ProceeOneCancelOrder(S_MDB_TXACC_LOCK *pAcclock,S_MDB_ORDER_INFO *po,int nCancelNum );
protected:

	CBF_Date_Time m_pDate;

	// 函数名: ComputeOneDone
	// 编程  : 王明松 2018-2-28 10:17:35
	// 返回  : bool 
	// 参数  : S_MDB_TXACC_LOCK *lockinfo
	// 参数  : S_MDB_CUST_FUND *fundinfo
	// 参数  : S_MDB_DONE_INFO *info
	// 参数  : S_MDB_CONTRACT_INFO *pc
	// 描述  : 计算数据库里的成交 更新资金及持仓  不处理解冻
	bool ComputeOneDone(S_MDB_TXACC_LOCK *lockinfo,S_MDB_CUST_FUND *fundinfo,S_MDB_DONE_INFO *info,S_MDB_CONTRACT_INFO *pc);

	// 函数名: ComputeOneOrder
	// 编程  : 王明松 2018-2-28 10:12:41
	// 返回  : bool 
	// 参数  : S_MDB_TXACC_LOCK *lockinfo
	// 参数  : S_MDB_CUST_FUND *fundinfo
	// 参数  : S_MDB_ORDER_INFO *info
	// 参数  : S_MDB_CONTRACT_INFO *pc
	// 描述  : 计算数据库里的报单  更新资金及持仓 只对剩余量进行计算。
	bool ComputeOneOrder(S_MDB_TXACC_LOCK *lockinfo,S_MDB_CUST_FUND *fundinfo,S_MDB_ORDER_INFO *info,S_MDB_CONTRACT_INFO *pc);

	S_MDB_BIG_MARGIN *AddBigMarginNull(char *txacc,int nvariety);

	bool UpdatePosition(S_MDB_TXACC_LOCK *lockinfo,S_MDB_CUST_FUND *fundinfo,S_MDB_DONE_INFO *pd,S_MDB_CUST_POSITION *ps,S_MDB_CONTRACT_INFO *pc,bool isinit=false);
public:
	bool GetSettlePrice(int contractid, double &settleprice);

	// 函数名: ProcDoneNetMargin
	// 编程  : 王明松 2018-2-28 10:12:41
	// 返回  : int  0成功
	// 参数  : S_MDB_TXACC_LOCK *lockinfo
	// 参数  : S_MDB_CUST_FUND *fundinfo
	// 参数  : S_MDB_DONE_INFO *pd    成交
	// 参数  : S_MDB_ORDER_INFO *po  当为初始化处理时，可为NULL
	// 参数  : S_MDB_CONTRACT_INFO *pc
	// 参数  : bool  bisinit 是否初始化时的处理
	// 描述  : 处理成交的净边资金
	int ProcDoneNetMargin(S_MDB_TXACC_LOCK *lockinfo,S_MDB_CUST_FUND *fundinfo,S_MDB_DONE_INFO *pd,S_MDB_ORDER_INFO *po,S_MDB_CONTRACT_INFO *pc,bool bisinit=false);

	// 函数名: ProcOrderNetMargin
	// 编程  : 王明松 2018-2-28 10:12:41
	// 返回  : int  0成功
	// 参数  : S_MDB_TXACC_LOCK *lockinfo
	// 参数  : S_MDB_CUST_FUND *fundinfo
	// 参数  : S_MDB_ORDER_INFO *po
	// 参数  : S_MDB_CONTRACT_INFO *pc
	// 参数  : bool  bisinit 是否初始化时的处理 初始化时不检查资金  
	// 参数  : bool  checkfund 是否检查资金足够 风控发起的报单要检查资金  柜台同步过来的外报单不需检查资金
	// 描述  : 处理报单的净边资金
	int ProcOrderNetMargin(S_MDB_TXACC_LOCK *lockinfo,S_MDB_CUST_FUND *fundinfo,S_MDB_ORDER_INFO *po,S_MDB_CONTRACT_INFO *pc,bool bisinit=false,bool checkfund=false);

	// 函数名: ProcCancelOrderNetMargin
	// 编程  : 王明松 2018-2-28 10:12:41
	// 返回  : int  0成功
	// 参数  : S_MDB_TXACC_LOCK *lockinfo
	// 参数  : S_MDB_CUST_FUND *fundinfo
	// 参数  : S_MDB_ORDER_INFO *po
	// 参数  : S_MDB_CONTRACT_INFO *pc
	// 描述  : 处理撤单的净边资金 撤单回报的处理
	int ProcCancelOrderNetMargin(S_MDB_TXACC_LOCK *lockinfo,S_MDB_CUST_FUND *fundinfo,S_MDB_ORDER_INFO *po,S_MDB_CONTRACT_INFO *pc);

	int GetFeeMargin(S_MDB_ORDER_INFO *porder,S_MDB_CONTRACT_INFO *pc, S_MDB_CUST_FUNDACC_INFO *pcust, double &exfee, double &memberfee,double &exmargin,double &membermargin);
};

#endif // !defined(AFX_KERNELPUB_H__1E64E06C_1D5F_41AB_8FB8_A16C19C39070__INCLUDED_)
