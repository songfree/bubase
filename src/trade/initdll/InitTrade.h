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
	

	// ������: ReadQuotation
	// ���  : ������ 2018-3-6 16:16:33
	// ����  : bool 
	// ����  : �����ݿ�������
	bool ReadQuotation();

	// ������: ReadFundFlow
	// ���  : ������ 2018-3-6 16:16:33
	// ����  : bool 
	// ����  : �����ݿ��������
	bool ReadFundFlow();

	// ������: ReadDoneMatch
	// ���  : ������ 2018-3-6 16:16:33
	// ����  : bool 
	// ����  : �����ݿ����ɽ�
	bool ReadDoneMatch();

	// ������: ReadOrder
	// ���  : ������ 2018-3-6 16:16:33
	// ����  : bool 
	// ����  : �����ݿ���ί��
	bool ReadOrder();

	// ������: ReadCustStorage
	// ���  : ������ 2018-3-6 16:16:33
	// ����  : bool 
	// ����  : �����ݿ������
	bool ReadCustStorage();

	// ������: ReadCustPosition
	// ���  : ������ 2018-3-6 16:16:33
	// ����  : bool 
	// ����  : �����ݿ����ֲ�
	bool ReadCustPosition();

	// ������: ReadCustFundDetail
	// ���  : ������ 2018-3-6 16:16:33
	// ����  : bool 
	// ����  : �����ݿ����ʽ���ϸ
	bool ReadCustFundDetail();

	// ������: ReadCustFund
	// ���  : ������ 2018-3-6 16:16:33
	// ����  : bool 
	// ����  : �����ݿ����ʽ�
	bool ReadCustFund();

	// ������: ReadExchTradeOnline
	// ���  : ������ 2018-3-6 16:16:33
	// ����  : bool 
	// ����  : �����ݿ�������������Ա����
	bool ReadExchTradeOnline();

	// ������: ReadTradeCode
	// ���  : ������ 2018-3-6 16:16:33
	// ����  : bool 
	// ����  : �����ݿ������ױ���
	bool ReadTradeCode();

	// ������: ReadCustOnline
	// ���  : ������ 2018-3-6 16:16:33
	// ����  : bool 
	// ����  : �����ݿ����ͻ�����
	bool ReadCustOnline();

	// ������: InitTrader
	// ���  : ������ 2018-3-6 16:16:33
	// ����  : ��ʼ��������Ϣ
	bool InitTrader();

	// ������: ReadCustFeeDetail
	// ���  : ������ 2018-3-6 16:16:33
	// ����  : bool 
	// ����  : �����ݿ���ͻ��������
	bool ReadCustFeeDetail();

	// ������: ReadFeeTempletDetail
	// ���  : ������ 2018-3-6 16:16:33
	// ����  : bool 
	// ����  : �����ݿ������ģ����ϸ����
	bool ReadFeeTempletDetail();

	// ������: ReadFeeTemplet
	// ���  : ������ 2018-3-6 16:16:33
	// ����  : bool 
	// ����  : �����ݿ������ģ��
	bool ReadFeeTemplet();

	bool ReadCustPosiDiff();

	// ������: ReadTxaccTemplet
	// ���  : ������ 2018-3-6 16:16:33
	// ����  : bool 
	// ����  : �����ݿ���ͻ���Ӧ�ķ���ģ��
	bool ReadTxaccTemplet();

	// ������: ReadPosiTempletDetail
	// ���  : ������ 2018-3-6 16:16:33
	// ����  : bool 
	// ����  : �����ݿ���ֲ�ģ����ϸ
	bool ReadPosiTempletDetail();

	// ������: ReadFundTempletDetail
	// ���  : ������ 2018-3-6 16:16:33
	// ����  : bool 
	// ����  : �����ݿ���ʽ�ģ����ϸ
	bool ReadFundTempletDetail();

	// ������: ReadPosiTemplet
	// ���  : ������ 2018-3-6 16:16:33
	// ����  : bool 
	// ����  : �����ݿ���ֲ�ģ��
	bool ReadPosiTemplet();

	// ������: ReadPosiTemplet
	// ���  : ������ 2018-3-6 16:16:33
	// ����  : bool 
	// ����  : �����ݿ���ʽ�ģ��
	bool ReadFundTemplet();

	// ������: ReadProdPosiMonitor
	// ���  : ������ 2018-3-6 16:16:33
	// ����  : bool 
	// ����  : �����ݿ����Ʒ�ֲ�ģ��
	bool ReadProdPosiMonitor();

	// ������: ReadProdFundMonitor
	// ���  : ������ 2018-3-6 16:16:33
	// ����  : bool 
	// ����  : �����ݿ����Ʒ�ʽ�ģ��
	bool ReadProdFundMonitor();

	// ������: ReadRiskContractSet
	// ���  : ������ 2018-3-6 16:16:33
	// ����  : bool 
	// ����  : �����ݿ�����պ�Լ����
	bool ReadRiskContractSet();

	// ������: ReadRiskContractSet
	// ���  : ������ 2018-3-6 16:16:33
	// ����  : bool 
	// ����  : �����ݿ����Ʒ������Ӧģ��
	bool ReadProdOrgTemplet();
	// ������: ReadWarnLevel
	// ���  : ������ 2018-3-6 16:16:33
	// ����  : bool 
	// ����  : �����ݿ��Ԥ������
	bool ReadWarnLevel();

	// ������: ReadOptionModel
	// ���  : ������ 2018-3-6 16:16:33
	// ����  : bool 
	// ����  : �����ݿ���ȡ��Ȩ�۸�ģ��
	bool ReadOptionModel();

	// ������: InitSet
	// ���  : ������ 2018-3-6 16:16:33
	// ����  : bool 
	// ����  : ��ʼ������
	bool InitSet();


	// ������: ReadFundAcc
	// ���  : ������ 2018-5-31 16:00:28
	// ����  : bool 
	// ����  : ���ʽ��˺�
	bool ReadFundAcc();

	// ������: ReadCustom
	// ���  : ������ 2018-5-31 16:00:44
	// ����  : bool 
	// ����  : ���ͻ���Ϣ
	bool ReadCustom();

	// ������: InitCust
	// ���  : ������ 2018-5-31 16:00:55
	// ����  : bool 
	// ����  : ��ʼ���ͻ���Ϣ
	bool InitCust();
	

	// ������: InitBaseInfo
	// ���  : ������ 2018-5-31 16:01:08
	// ����  : bool 
	// ����  : ��ʼ��������Ϣ
	bool InitBaseInfo();

	// ������: ReadVariety
	// ���  : ������ 2018-5-31 16:01:31
	// ����  : bool 
	// ����  : �����ݿ��Ʒ��
	bool ReadVariety();

	// ������: ReadContract
	// ���  : ������ 2018-5-31 16:01:43
	// ����  : bool 
	// ����  : �����ݿ����Լ
	bool ReadContract();

	// ������: ReadMarket
	// ���  : ������ 2018-5-31 16:01:56
	// ����  : bool 
	// ����  : �����ݿ���г���Ϣ
	bool ReadMarket();

	// ������: ReadExchange
	// ���  : ������ 2018-5-31 16:02:09
	// ����  : bool 
	// ����  : �����ݿ��������
	bool ReadExchange();

	// ������: ReadProd
	// ���  : ������ 2018-5-31 16:02:22
	// ����  : bool 
	// ����  : �����ݿ����Ʒ
	bool ReadProd();

	// ������: ReadOperTxacc
	// ���  : ������ 2018-5-31 16:02:33
	// ����  : bool 
	// ����  : �����ݿ������Ա�ʽ��˺Ŷ�Ӧ
	bool ReadOperTxacc();

	// ������: ReadProdTxacc
	// ���  : ������ 2018-5-31 16:02:55
	// ����  : bool 
	// ����  : �����ݿ����Ʒ��Ӧ�ʽ��˺�
	bool ReadProdTxacc();

	// ������: ReadOperProd
	// ���  : ������ 2018-5-31 16:03:11
	// ����  : bool 
	// ����  : �����ݿ������Ա��Ӧ��Ʒ
	bool ReadOperProd();

	// ������: ReadContractLimit
	// ���  : ������ 2018-5-31 16:03:27
	// ����  : bool 
	// ����  : �����ݿ�����ź�Լ��������
	bool ReadContractLimit();

	// ������: ReadSettlePrice
	// ���  : ������ 2018-5-31 16:05:15
	// ����  : bool 
	// ����  : �����ݿ����Լ�����
	bool ReadSettlePrice();

	// ������: ReadSpecDate
	// ���  : ������ 2018-5-31 16:05:34
	// ����  : bool 
	// ����  : �����ݿ���ڼ������ñ�
	bool ReadSpecDate();


	// ������: InitPublic
	// ���  : ������ 2018-5-31 16:05:50
	// ����  : bool 
	// ����  : ��ʼ��������Ϣ
	bool InitPublic();

	// ������: ReadPara
	// ���  : ������ 2018-5-31 16:06:01
	// ����  : bool 
	// ����  : �����ݿ��ϵͳ����
	bool ReadPara();

	// ������: ReadOper
	// ���  : ������ 2018-5-31 16:06:15
	// ����  : bool 
	// ����  : �����ݿ������Ա��Ϣ
	bool ReadOper();

	// ������: ReadOrgan
	// ���  : ������ 2018-5-31 16:06:29
	// ����  : bool 
	// ����  : �����ݿ��������Ϣ
	bool ReadOrgan();

	// ������: ReadOperOnline
	// ���  : ������ 2018-5-31 16:06:41
	// ����  : bool 
	// ����  : �����ݿ������Ա������Ϣ
	bool ReadOperOnline();

	// ������: ReadBrokerageInfo
	// ���  : ������ 2018-5-31 16:06:41
	// ����  : bool 
	// ����  : �����ݿ��������Ϣ
	bool ReadBrokerageInfo();
	

	// ������: ConnectLocalDb
	// ���  : ������ 2018-5-31 16:06:54
	// ����  : bool 
	// ����  : ���ӱ������ݿ�
	bool ConnectLocalDb();
	
	// ������: ProcCustFund
	// ���  : ������ 2018-3-6 16:16:33
	// ����  : bool 
	// ����  : ����ͻ��Ĵ�߱�֤�𡢳ɽ��������������
	bool ProcCustFund();

	// ������: ProcOneCust
	// ���  : ������ 2018-3-6 16:16:33
	// ����  : bool 
	// ����  : int rid  �ʽ��˻�����rowid�����ݴ�rowid���ҵ���Ӧ���ʽ��˻����ʽ���ͻ���Ϣ���
	// ����  : ����һ���ͻ��ĳֲ֡��ɽ��������������
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
//ע�⣺����Ķ�����ÿ�������涼Ҫ�У���������ݴ˷�����ȡ�����ʵ��
extern "C" 
{ 
    DLL_EXPORT CInitTrade * getInstance() ; 
}

#endif // !defined(AFX_INITTRADE_H__CDA96C9F_53BB_4389_A85C_ED688807B76C__INCLUDED_)
