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

	
	

	// ������: OrderMsg
	// ���  : ������ 2018-6-1 16:46:18
	// ����  : int 
	// ����  : S_TRADE_DATA &data
	// ����  : ί����Ϣ����
	int OrderMsg(S_TRADE_DATA &data);

	

	// ������: QueryCustOrder
	// ���  : ������ 2018-6-1 16:47:10
	// ����  : int 
	// ����  : S_TRADE_DATA &data
	// ����  : ��ѯ�ͻ�����
	int QueryCustOrder(S_TRADE_DATA &data);

	

	// ������: RcvQuotation
	// ���  : ������ 2018-3-6 16:16:08
	// ����  : int 
	// ����  : S_TRADE_DATA &data
	// ����  : �������鴦��
	int RcvQuotation(S_TRADE_DATA &data);
	RiskDll();
	virtual ~RiskDll();

	virtual bool Init(CBF_Xml *xml,CIClientLink *pBusLink,CMemDb *pMemDb,	CXdp *pXdp,	CIErrlog *pLog,CBF_BufferPool *pMemPool,CDbApi *pSqlCmd,CIExchLink *pExchLink,int index);

	CBF_Date_Time m_pDate;
	char m_sErrMsg[256];

	CKernelPub   m_pKernelPub;//�ʽ�ֲֵȵĹ���������

	S_TRADE_DATA  m_pSendData;

	INT64_   m_nBeginTime;//��ʼʱ��
	INT64_   m_nEndTime;  //��������

	
protected:
	int      m_nCallSerial;//�첽��ˮ

	bool ReadContract();
	bool ReadVariety();

	int GetNextSerial();
	int GetCurSerial();

	// ������: SendBus
	// ���  : ������ 2018-10-24 16:16:08
	// ����  : int 
	// ����  : CXdp *xdp  xd����
	// ����  : int svrid  ���շ���������ע��ķ����
	// ����  : int funcno  ������
	// ����  : int serial �첽��ˮ
	// ����  : short cmd ����������
	// ����  : �������鴦��
	int SendBus(CXdp *xdp,int svrid,int funcno,int serial,short cmd);


	void Send2CustPushMsg(S_MDB_TXACC_LOCK *acc, const char *pushdata,int pushdatalen,int funcno);
	void Send2OperPushMsg(S_MDB_TXACC_LOCK *acc,const char *pushdata,int pushdatalen,int funcno);


	void SendMoneyIOPush(S_MDB_TXACC_LOCK *acclock ,S_MDB_FUND_FLOW *info);
	void SendDonePush(S_MDB_TXACC_LOCK *acclock ,S_MDB_DONE_INFO *info);
	void SendOrderPush(S_MDB_TXACC_LOCK *acclock ,S_MDB_ORDER_INFO *info);

	void GenAcc(const char *oldacc, int syscode,int destlen, char *txacc);

	void Send2OperProdOrgRiskMsg(char *orgcode, char traderight,const char *riskdata,int riskdatalen,int funcno);

	

	bool ComputeProd(const char *compdate);
	bool ComputeOrg(const char *compdate);
	bool ComputeOneOrg(const char* compdate, char* orgcode, const char* orgname, const char* risktype, std::vector<S_MDB_CUST_FUNDACC_INFO*> orgcustlist);
	

	bool SyncDone(S_MDB_DONE_INFO *done);
	bool SyncOrder(S_MDB_ORDER_INFO *order);

	bool CalcRiskInfo(char *txacc,S_MDB_CONTRACT_INFO *coninfo,double newprice,S_MDB_CUST_FUND &fund);

	bool UpdateFeeTemplet();


	bool ReadOneCustInfo(const char *txacc);

	// ������: ReadInfoFromDbByExch
	// ���  : ������ 2018-3-30 16:00:03
	// ����  : bool 
	// ����  : char *exch
	// ����  : ��������������Ϣ
	bool ReadInfoFromDbByExch(char *exch);

	// ������: ReadInfoFromDbByMarket
	// ���  : ������ 2018-3-30 15:59:39
	// ����  : bool 
	// ����  : char *market
	// ����  : ���г�������Ϣ
	bool ReadInfoFromDbByMarket(char *market);

	// ������: ResetCustInfo
	// ���  : ������ 2018-3-30 15:59:22
	// ����  : bool 
	// ����  : CTbl_Char_Key &custlist
	// ����  : ���ÿͻ���Ϣ
	bool ResetCustInfo(CTbl_Char_Key &custlist);

	// ������: ChgDateProcByExchange
	// ���  : ������ 2018-3-30 15:59:07
	// ����  : bool 
	// ����  : char *exchange
	// ����  : char *curdate ��ǰ������
	// ����  : ���������д���
	bool ChgDateProcByExchange(char *exchange,char *curdate);

	// ������: ChgDateProcByMarket
	// ���  : ������ 2018-3-30 15:58:50
	// ����  : bool 
	// ����  : char *market
	// ����  : char *curdate ��ǰ������
	// ����  : �г����д���
	bool ChgDateProcByMarket(char *market,char *curdate);

	bool DeleteMoneyIO(CTbl_Char_Key &custlist,char *curdate);

	// ������: UpdateOrder2Db
	// ���  : ������ 2018-6-1 16:48:07
	// ����  : bool 
	// ����  : S_MDB_ORDER_INFO *porder
	// ����  : ����ί�������ݿ�
	bool UpdateOrder2Db(S_MDB_ORDER_INFO *porder);

	// ������: ComputeNewOrder
	// ���  : ������ 2018-2-28 10:12:41
	// ����  : bool 
	// ����  : S_MDB_TXACC_LOCK *lockinfo
	// ����  : S_MDB_CUST_FUND *fundinfo
	// ����  : S_MDB_ORDER_INFO *info   ������Ϣ�������� ��֤�� ��� Ҫ��ǰ���
	// ����  : S_MDB_CONTRACT_INFO *pcinfo 
	// ����  : char *errmsg
	// ����  : ����һ���µı���  �����ʽ𼰳ֲ�
	bool ComputeNewOrder(S_MDB_TXACC_LOCK *lockinfo,S_MDB_CUST_FUND *fundinfo,S_MDB_ORDER_INFO *info,S_MDB_CONTRACT_INFO *pcinfo ,char *errmsg);

	// ������: ComputeNewDone
	// ���  : ������ 2018-2-28 10:17:35
	// ����  : bool 
	// ����  : S_MDB_TXACC_LOCK *lockinfo
	// ����  : S_MDB_CUST_FUND *fundinfo
	// ����  : S_MDB_DONE_INFO *info
	// ����  : S_MDB_ORDER_INFO *po
	// ����  : S_MDB_CONTRACT_INFO *pc
	// ����  : char *errmsg
	// ����  : ����һ���µĳɽ� �����ʽ𼰳ֲ�
	bool ComputeNewDone(S_MDB_TXACC_LOCK *lockinfo,S_MDB_CUST_FUND *fundinfo,S_MDB_DONE_INFO *info,S_MDB_ORDER_INFO *po,S_MDB_CONTRACT_INFO *pc,char *errmsg);


	// ������: Send2OperRiskMsg
	// ���  : ������ 2018-6-1 16:49:45
	// ����  : void 
	// ����  : S_MDB_TXACC_LOCK *acc
	// ����  : char traderight
	// ����  : const char *riskdata
	// ����  : int riskdatalen
	// ����  : int funcno
	// ����  : ���͸�����Ա������Ϣ
	void Send2OperRiskMsg(S_MDB_TXACC_LOCK *acc, char traderight,const char *riskdata,int riskdatalen,int funcno);

	// ������: Send2CustRiskMsg
	// ���  : ������ 2018-6-1 16:50:04
	// ����  : void 
	// ����  : S_MDB_TXACC_LOCK *acc
	// ����  : char traderight
	// ����  : const char *riskdata
	// ����  : int riskdatalen
	// ����  : int funcno
	// ����  : ���͸��ͻ�������Ϣ
	void Send2CustRiskMsg(S_MDB_TXACC_LOCK *acc,char traderight,const char *riskdata,int riskdatalen,int funcno);

	void Send2PgSync(S_MDB_TXACC_LOCK *acc,const char *riskdata,int riskdatalen,int funcno);

	// ������: SendRiskMsg
	// ���  : ������ 2018-6-1 16:50:19
	// ����  : void 
	// ����  : S_MDB_TXACC_LOCK *acc
	// ����  : S_MDB_CUST_FUND *fund
	// ����  : char traderight
	// ����  : ���ͷ�����Ϣ
	void SendRiskMsg(S_MDB_TXACC_LOCK *acc,S_MDB_CUST_FUND *fund,char traderight);

	void AnsData(S_TRADE_DATA &data);
	
	char m_sSql[8000];

	// ������: ComputeSurplus
	// ���  : ������ 2018-3-6 16:17:53
	// ����  : void 
	// ����  : S_MDB_QUOTATION info
	// ����  : S_MDB_CONTRACT_INFO *pc
	// ����  : S_MDB_CUST_POSITION *posi
	// ����  : ����һ���ֲֵ�ӯ��
	void ComputeSurplus(S_MDB_QUOTATION info,S_MDB_CONTRACT_INFO *pc,S_MDB_CUST_POSITION *posi);

	// ������: ProcRisk
	// ���  : ������ 2018-3-6 16:17:20
	// ����  : void 
	// ����  : S_MDB_QUOTATION info
	// ����  : S_MDB_CONTRACT_INFO *pc
	// ����  : ������������ĳֲ�ӯ��
	void ProcRisk(S_MDB_QUOTATION info,S_MDB_CONTRACT_INFO *pc);
	
	// ������: ProcRiskContract
	// ���  : ������ 2018-3-6 16:16:44
	// ����  : void 
	// ����  : S_MDB_QUOTATION qinfo
	// ����  : S_MDB_CONTRACT_INFO *pc
	// ����  : ������պ�Լ
	void ProcRiskContract(S_MDB_QUOTATION qinfo,S_MDB_CONTRACT_INFO *pc);

	// ������: UpdateSetllePrice
	// ���  : ������ 2018-3-6 16:16:23
	// ����  : void 
	// ����  : S_MDB_QUOTATION qinfo
	// ����  : �������ݿ�����
	void UpdateSetllePrice(S_MDB_QUOTATION qinfo);



	// ������: GetAccList
	// ���  : ������ 2018-3-6 17:12:50
	// ����  : bool 
	// ����  : std::vector<int> &acclist
	// ����  : ȡ���и���ӯ���Ŀͻ��б�
	bool GetAccList(std::vector<int> &acclist);


	// ������: ComputeCustRisk
	// ���  : ������ 2018-3-6 17:12:39
	// ����  : void 
	// ����  : int rowid
	// ����  : ����һ���ͻ��ķ���
	void ComputeCustRisk(int rowid);


	// ������: ComputeCustPosiRisk
	// ���  : ������ 2018-3-12 16:16:28
	// ����  : void 
	// ����  : int rowid
	// ����  : ����һ���ͻ��ĳֲַ���
	void ComputeCustPosiRisk(int rowid);

	

	// ������: ProcessDone
	// ���  : ������ 2018-6-1 16:52:36
	// ����  : int 
	// ����  : S_MDB_CUST_FUNDACC_INFO *pcustacc
	// ����  : S_MDB_ORDER_INFO *po
	// ����  : S_MDB_CONTRACT_INFO *pc
	// ����  : S_MDB_EXCH_INFO *pexch
	// ����  : S_MDB_DONE_INFO &info
	// ����  : ����ɽ�
	int ProcessDone(S_MDB_CUST_FUNDACC_INFO *pcustacc ,S_MDB_ORDER_INFO *po,S_MDB_CONTRACT_INFO *pc,S_MDB_EXCH_INFO *pexch,S_MDB_DONE_INFO &info);


	//���������ʽ�
	bool UpdateFundLock(const char *txacc,char *currency,const char fundflag,const  char *fundcode,double amt,bool isadd,int &lockrowid);

public:
	void MsgCustRiskCalc(int lockrowid);
	

	// ������: Order
	// ���  : ������ 2018-3-6 16:16:08
	// ����  : int 
	// ����  : S_TRADE_DATA &data
	// ����  : ί��
	int Order(S_TRADE_DATA &data);

	
};
//ע�⣺����Ķ�����ÿ�������涼Ҫ�У���������ݴ˷�����ȡ�����ʵ��
extern "C" 
{ 
    DLL_EXPORT RiskDll * getInstance() ; 
}
#endif // !defined(AFX_RISKDLL_H__82682589_B43E_4DCC_B644_EC0A4A5D19DB__INCLUDED_)
