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
	
	CMemDb         *m_pMemDb;//�ڴ��
	CIErrlog       *m_pLog;

	// ������: ProcPosition
	// ���  : ������ 2018-3-6 16:16:33
	// ����  : bool 
	// ����  : S_MDB_TXACC_LOCK *acclock
	// ����  : std::vector<S_MDB_CUST_POSITION *> &posilist
	// ����  : ����һ���ͻ��ĳֲ֣����ɴ�߾�����Ϣ�����ǵ���Ŀͻ�������߱�֤������ʽ���ϸ
	bool ProcPosition(S_MDB_TXACC_LOCK *acclock,std::vector<S_MDB_CUST_POSITION *> &posilist);
	
	
	
	// ������: ProcDone
	// ���  : ������ 2018-3-6 16:16:33
	// ����  : bool 
	// ����  : S_MDB_TXACC_LOCK *acclock
	// ����  : std::vector<S_MDB_DONE_INFO *> &donelist
	// ����  : ����һ���ͻ��ĳɽ�����ʼ����ʽ�ֱֲ�֤�𣬲�������ⶳ
	bool ProcDone(S_MDB_TXACC_LOCK *acclock,std::vector<S_MDB_DONE_INFO *> &donelist);
	
	// ������: ProcOrder
	// ���  : ������ 2018-3-6 16:16:33
	// ����  : bool 
	// ����  : S_MDB_TXACC_LOCK *acclock
	// ����  : std::vector<S_MDB_ORDER_INFO *> &orderlist
	// ����  : ����һ���ͻ��ı�������ʼ����ʽ𶳽�֤��  ��ʣ�������м���
	bool ProcOrder(S_MDB_TXACC_LOCK *acclock,std::vector<S_MDB_ORDER_INFO *> &orderlist);
	
	// ������: ProcFundFlow
	// ���  : ������ 2018-3-6 16:16:33
	// ����  : bool 
	// ����  : S_MDB_TXACC_LOCK *acclock
	// ����  : std::vector<S_MDB_ORDER_INFO *> &orderlist
	// ����  : ����һ���ͻ��ĳ���𣬼����ʽ�
	bool ProcFundFlow(S_MDB_TXACC_LOCK *acclock,std::vector<S_MDB_FUND_FLOW *> &flowlist);
	
	// ������: GetMarginValue
	// ���  : ������ 2018-3-21 17:26:13
	// ����  : bool 
	// ����  : S_MDB_CONTRACT_INFO *pc
	// ����  : S_MDB_CUST_FUNDACC_INFO *pcust
	// ����  : int flag  0�ڻ���������֤��  1��Ȩ������֤��
	// ����  : int num
	// ����  : double amt
	// �������  : double &exmargin  ��������֤��
	// �������  : double &membermargin   ��Ա��֤��
	// ����  : ȡ��֤��
	bool GetMarginValue(S_MDB_CONTRACT_INFO *pc, S_MDB_CUST_FUNDACC_INFO *pcust,int flag ,int num, double amt, double &exmargin, double &membermargin);

	int GetRiskLevel(S_MDB_CUST_FUND *fund,char *traderight);

	// ������: GetPosiRiskLevel
	// ���  : ������ 2018-6-1 16:48:44
	// ����  : int 
	// ����  : S_MDB_TXACC_LOCK *acclock
	// ����  : S_MDB_CUST_TEMPLET_RELA *templet
	// ����  : S_MDB_CUST_POSITION *custposi
	// ����  : char *traderight
	// ����  : char *warnmsg
	// ����  : ȡ�ֲַ��ռ���
	int GetPosiRiskLevel(S_MDB_TXACC_LOCK *acclock,S_MDB_CUST_TEMPLET_RELA *templet,S_MDB_CUST_POSITION *custposi,char *traderight,char *warnmsg);

	int GetProdRiskLevel(char *prodid,double unitvalue,double risk,char *traderight,char *warnmsg);
	int GetProdRiskLevel(char *prodid,double marginrate,double posivalue,double posidiffvalue,char *traderight,char *warnmsg);

	int GetOrgRiskLevel(char *orgcode,double unitvalue,double risk,char *traderight,char *warnmsg);
	int GetOrgRiskLevel(char *orgcode,double marginrate,double posivalue,double posidiffvalue,char *traderight,char *warnmsg);

	// ������: GetTradeFeeValue
	// ���  : ������ 2018-3-21 14:08:49
	// ����  : bool 
	// ����  : S_MDB_CONTRACT_INFO *pc    ��Լ��Ϣ
	// ����  : S_MDB_CUST_FUNDACC_INFO *pcust  �ʽ��˺���Ϣ
	// ����  : int flag   ������ƽ��־  1���뿪�� 2����ƽ�� 3ƽ���
	// ����  : int num   ����(��)
	// ����  : double amt  ���
	// �������  : double &exfee  ������������
	// �������  : double &memberfee   ��Ա������
	// ����  : ȡ����������
	bool GetTradeFeeValue(S_MDB_CONTRACT_INFO *pc ,S_MDB_CUST_FUNDACC_INFO *pcust,int flag,int num,double amt,double &exfee,double &memberfee);

	// ������: CheckRight
	// ���  : ������ 2018-6-1 16:49:32
	// ����  : bool 
	// ����  : char *custorg
	// ����  : char *operorg
	// ����  : ���Ȩ��
	bool CheckRight(char *custorg,char *operorg);

	// ������: CheckOperProdRight
	// ���  : ������ 2018-6-1 16:49:01
	// ����  : bool 
	// ����  : const char *oper
	// ����  : const char *prod
	// ����  : ������Ա��ƷȨ��
	bool CheckOperProdRight(const char *oper, const char *prod);

	// ������: CheckOperAccRight
	// ���  : ������ 2018-6-1 16:49:14
	// ����  : bool 
	// ����  : const char *oper
	// ����  : const char *acc
	// ����  : ������Ա�ʽ��˺�Ȩ��
	bool CheckOperAccRight(const char *oper,const char *acc);

	bool CheckOperOrgRight(const char *oper,const char *org);
	
	// ������: UpdateFund
	// ���  : ������ 2018-2-23 13:03:25
	// ����  : bool 
	// ����  : S_MDB_TXACC_LOCK *lockinfo
	// ����  : S_MDB_CUST_FUND *fundinfo
	// ����  : const char fundflag   �ʽ�����
	// ����  : const  char *fundcode  �ʽ���
	// ����  : double amt  ���   ��������
	// ����  : bool isadd   true ��ԭ����ֱֵ����ӣ�falseʹ������ֵ�滻ԭ��ֵ��
	// ����  : bool islock   �Ƿ����
	// ����  : �����ʽ�
	bool UpdateFund(S_MDB_TXACC_LOCK *info,S_MDB_CUST_FUND *fund,const char fundflag,const  char *fundcode,double amt,bool isadd,bool islock=false);

	// ������: GetLastTradeDate
	// ���  : ������ 2018-5-31 19:41:40
	// ����  : int <0ʧ��  >0������
	// ����  : const char *market  �г�
	// ����  : const char *excode  ������
	// ����  : const char *curdate  ��ǰ������
	// ����  : int lastn   ǰ5�������գ�Ϊ6�������һ�������յ�ǰ�ĸ�������
	// ����  : ȡ�ϼ���������
	int GetLastTradeDate(const char *market,const char *excode,const char *curdate,int lastn);
	
	// ������: GetMarginMode
	// ���  : ������ 2018-5-31 19:41:40
	// ����  : int <0ʧ��  0����˫��  1 ����   2��� 3�Ǳ�֤�� ���ڻ�
	// ����  : S_MDB_CONTRACT_INFO *pc  �г�
	// ����  : ���ݺ�Լȡ��֤��ģʽ
	int GetMarginMode(S_MDB_CONTRACT_INFO *pc);
	
	// ������: ProcOrder
	// ���  : ������ 2018-3-6 16:16:33
	// ����  : int  0�ɹ� ����ʧ��  >0������
	// ����  : S_MDB_TXACC_LOCK *lockinfo  �ͻ���
	// ����  : S_MDB_CUST_FUND *fundinfo   �ʽ��
	// ����  : S_MDB_ORDER_INFO *info      ������Ϣ
	// ����  : S_MDB_CONTRACT_INFO *pc     ��Լ��Ϣ
	// ����  : bool checkfund=false  �Ƿ����ʽ𼰲�λ
	// ����  : ����һ�ʱ����������ʽ𶳽�֤��
	int ComputeNewOrder(S_MDB_TXACC_LOCK *lockinfo,S_MDB_CUST_FUND *fundinfo,S_MDB_ORDER_INFO *info,S_MDB_CONTRACT_INFO *pc,bool checkfund=false);

	bool ComputeNewDone(S_MDB_TXACC_LOCK *lockinfo,S_MDB_CUST_FUND *fundinfo,S_MDB_DONE_INFO *info,S_MDB_ORDER_INFO *po,S_MDB_CONTRACT_INFO *pc);

	int ProceeOneCancelOrder(S_MDB_TXACC_LOCK *pAcclock,S_MDB_ORDER_INFO *po,int nCancelNum );
protected:

	CBF_Date_Time m_pDate;

	// ������: ComputeOneDone
	// ���  : ������ 2018-2-28 10:17:35
	// ����  : bool 
	// ����  : S_MDB_TXACC_LOCK *lockinfo
	// ����  : S_MDB_CUST_FUND *fundinfo
	// ����  : S_MDB_DONE_INFO *info
	// ����  : S_MDB_CONTRACT_INFO *pc
	// ����  : �������ݿ���ĳɽ� �����ʽ𼰳ֲ�  ������ⶳ
	bool ComputeOneDone(S_MDB_TXACC_LOCK *lockinfo,S_MDB_CUST_FUND *fundinfo,S_MDB_DONE_INFO *info,S_MDB_CONTRACT_INFO *pc);

	// ������: ComputeOneOrder
	// ���  : ������ 2018-2-28 10:12:41
	// ����  : bool 
	// ����  : S_MDB_TXACC_LOCK *lockinfo
	// ����  : S_MDB_CUST_FUND *fundinfo
	// ����  : S_MDB_ORDER_INFO *info
	// ����  : S_MDB_CONTRACT_INFO *pc
	// ����  : �������ݿ���ı���  �����ʽ𼰳ֲ� ֻ��ʣ�������м��㡣
	bool ComputeOneOrder(S_MDB_TXACC_LOCK *lockinfo,S_MDB_CUST_FUND *fundinfo,S_MDB_ORDER_INFO *info,S_MDB_CONTRACT_INFO *pc);

	S_MDB_BIG_MARGIN *AddBigMarginNull(char *txacc,int nvariety);

	bool UpdatePosition(S_MDB_TXACC_LOCK *lockinfo,S_MDB_CUST_FUND *fundinfo,S_MDB_DONE_INFO *pd,S_MDB_CUST_POSITION *ps,S_MDB_CONTRACT_INFO *pc,bool isinit=false);
public:
	bool GetSettlePrice(int contractid, double &settleprice);

	// ������: ProcDoneNetMargin
	// ���  : ������ 2018-2-28 10:12:41
	// ����  : int  0�ɹ�
	// ����  : S_MDB_TXACC_LOCK *lockinfo
	// ����  : S_MDB_CUST_FUND *fundinfo
	// ����  : S_MDB_DONE_INFO *pd    �ɽ�
	// ����  : S_MDB_ORDER_INFO *po  ��Ϊ��ʼ������ʱ����ΪNULL
	// ����  : S_MDB_CONTRACT_INFO *pc
	// ����  : bool  bisinit �Ƿ��ʼ��ʱ�Ĵ���
	// ����  : ����ɽ��ľ����ʽ�
	int ProcDoneNetMargin(S_MDB_TXACC_LOCK *lockinfo,S_MDB_CUST_FUND *fundinfo,S_MDB_DONE_INFO *pd,S_MDB_ORDER_INFO *po,S_MDB_CONTRACT_INFO *pc,bool bisinit=false);

	// ������: ProcOrderNetMargin
	// ���  : ������ 2018-2-28 10:12:41
	// ����  : int  0�ɹ�
	// ����  : S_MDB_TXACC_LOCK *lockinfo
	// ����  : S_MDB_CUST_FUND *fundinfo
	// ����  : S_MDB_ORDER_INFO *po
	// ����  : S_MDB_CONTRACT_INFO *pc
	// ����  : bool  bisinit �Ƿ��ʼ��ʱ�Ĵ��� ��ʼ��ʱ������ʽ�  
	// ����  : bool  checkfund �Ƿ����ʽ��㹻 ��ط���ı���Ҫ����ʽ�  ��̨ͬ���������ⱨ���������ʽ�
	// ����  : �������ľ����ʽ�
	int ProcOrderNetMargin(S_MDB_TXACC_LOCK *lockinfo,S_MDB_CUST_FUND *fundinfo,S_MDB_ORDER_INFO *po,S_MDB_CONTRACT_INFO *pc,bool bisinit=false,bool checkfund=false);

	// ������: ProcCancelOrderNetMargin
	// ���  : ������ 2018-2-28 10:12:41
	// ����  : int  0�ɹ�
	// ����  : S_MDB_TXACC_LOCK *lockinfo
	// ����  : S_MDB_CUST_FUND *fundinfo
	// ����  : S_MDB_ORDER_INFO *po
	// ����  : S_MDB_CONTRACT_INFO *pc
	// ����  : �������ľ����ʽ� �����ر��Ĵ���
	int ProcCancelOrderNetMargin(S_MDB_TXACC_LOCK *lockinfo,S_MDB_CUST_FUND *fundinfo,S_MDB_ORDER_INFO *po,S_MDB_CONTRACT_INFO *pc);

	int GetFeeMargin(S_MDB_ORDER_INFO *porder,S_MDB_CONTRACT_INFO *pc, S_MDB_CUST_FUNDACC_INFO *pcust, double &exfee, double &memberfee,double &exmargin,double &membermargin);
};

#endif // !defined(AFX_KERNELPUB_H__1E64E06C_1D5F_41AB_8FB8_A16C19C39070__INCLUDED_)
