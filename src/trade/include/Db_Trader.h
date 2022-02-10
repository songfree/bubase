// Db_Trader.h: interface for the CDb_Trader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DB_TRADER_H__6EECD875_4F24_4FFA_9024_3DEBCD57D140__INCLUDED_)
#define AFX_DB_TRADER_H__6EECD875_4F24_4FFA_9024_3DEBCD57D140__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//���ݿ� ����  ί�С��ɽ�������𡢻ر��������

//�ͻ����߱�
typedef struct
{
	char sCustNo[21];//�ͻ����
	char sTermType[11];//��¼����
	char sLoginIp[21];//��¼IP
	char sOnlineStatus[2];//����״̬
	char sOnlineTime[18];//��¼ʱ��
	int  nDrebId;//���߹����ڵ��
	int  nDrebPid;//����˽�нڵ��
	int  nSvrId;//���������
	int  nSvrPid;//˽�з����
	int  nGateIndex; //�����������
	long nTimeStamp;//��¼ʱ���
}S_MDB_CUST_ONLINE;


typedef struct 
{
	char sTxAcc[21];//�ʽ��˺�
	char sExCode[2];//����������
	char sSeatNo[7];//ϯλ����
	char sTradeCode[21];//���ױ���
}S_MDB_CUST_TRADECODE;

//����������Ա��¼��Ϣ
typedef struct 
{
	char sExchCode[2]; //����������
	char sSeatNo[7];//ϯλ����
	char sTraderId[17];//����Ա����
	char sLoginStatus[2];//��¼״̬ 0δ����δ��¼ 	1�������ѵ�¼ 2������δ��¼
	char sLoginTime[18];//��¼ʱ��yymmdd hhmmss
	char sLogoutTime[18];//¼��ʱ��yymmdd hhmmss
	char sPara[201]; //ͨѶ����
}S_MDB_EXCH_TRADER_INFO;


typedef struct 
{
	char sTxAcc[21];//�ʽ��˺�
	char sCurrency[2];//����
	double dLast_bal;//���ս��
	double dBalance;//���ս��
	double dAvailFund;//���տ���
	double dGetFund;//���տ���
	double dBaseMargin;//������֤��
	double dIncomeExpense;//�ո�����
	double dFrozeFund;//�����ʽ�
	double dMargin;//��֤��
	double dFee;//����
	double dReserve;//������
	double dSurplus;//ӯ��
	double dFloatSurplus;//����ӯ��
// 	double dInteIntegral;//��Ϣ����
// 	double dPuniIntegral;//��Ϣ����
	double dIncrInte;//��Ϣ
	double dArap;//Ӧ��Ӧ��
	int    nRisk_level;//���ռ���
	double dRisk;//���ն�
	char sTradeRight[2];//����Ȩ��
	char   sRiskDesc[81];//����˵��
	char   sRiskFlag[2];//���ձ�־ '1' �������� 0ȡ������
	unsigned int nRiskdate;//������������
	unsigned int nRisktime;//��������ʱ��
}S_MDB_CUST_FUND;//�ʽ��


typedef struct 
{
	char   sTxAcc[21];//�ʽ��˺�
	char   sCurrency[2];//����
	char   sTime[9];//ʱ��  hhmmss
	int    nRisk_level;//���ռ���
	double dRisk;//���ն�
}S_MDB_CUST_RISK_TICK;//���շ��ձ�

typedef struct
{
	char sTxAcc[21];//�ʽ��˺�
	char sCurrency[2];//����
	char sFundType[2];//�ʽ�����
	char sFundCode[4];//�ʽ���
	double dFundValue;//ֵ
}S_MDB_CUST_FUND_DETAIL;//�ʽ���ϸ��

typedef struct 
{
	char sTxAcc[21];
	unsigned int  nContractNo;//��Լ���
	char sContractCode[21];//��Լ����
	char sSeatNo[7];//ϯλ����
	char sTradeCode[21];//���ױ���
	char sShFlag[2];//Ͷ����־
	int  nLastPB;//������� ��ͷ�ֲ�
	int  nLastPS;//��������  ��ͷ�ֲ�
	int  nCurPB;//��ǰ���
	int  nCurPS;//��ǰ�ղ�
	int  nCurCanUsePB;//��ǰ���ö��
	int  nCurCanUsePS;//��ǰ���ÿղ�
	int  nDayPB;//���쿪��ֲֳ�
	int  nDayPS;//���쿪�ղֲֳ�
	int  nDAY_COV_PB_Froze;//����ƽ��ֶ���
	int  nDAY_COV_PB;//����ƽ���
	int  nDAY_COV_PS_Froze;//����ƽ�ղֶ���
	int  nDAY_COV_PS;//����ƽ�ղ�
	int  nDAY_DELI_PB_Froze;//���콻�ն��
	int  nDAY_DELI_PB;//���콻�ն��
	int  nDAY_DELI_PS_Froze;//���콻�տղֶ���
	int  nDAY_DELI_PS;//���콻�տղ�
	int  nDAY_MIDD_PB;//�����ֶ��
	int  nDAY_MIDD_PS;//�����ֿղ�
	double dPB_OAvp;//��ֿ��־���    �ֻ���Ʊ���ֳɱ�����
	double dPS_OAvp;//�ղֿ��־���
	double dPB_Avp;//��ֲֳ־���         �ֻ���Ʊ�ɱ�����(ͬ��Ʊ������ӯ����ɱ����ͣ���������ɱ�����)
	double dPS_Avp;//�ղֲֳ־���
	double dPB_Margin;//��ֱֲֳ�֤��    �ֻ���Ʊ�ֲֳɱ�
	double dPS_Margin;//�ղֱֲֳ�֤��
	double dDAY_PB_SURPLUS;//���ն�ͷӯ��
	double dDAY_PS_SURPLUS;//���տ�ͷӯ��
	double dLAST_SETTLE_PRICE;//���ս����
	double dSETTLE_PRICE;//�����
	int    nVarietyId;//Ʒ�ֱ��
}S_MDB_CUST_POSITION;

//����
typedef struct
{
	char sTxAcc[21]; //�����˺�
	char sProdCode[7];//Ʒ�ִ���
	int  nLastBal;//�������
	int  nLastCanUse;//���տ���
	int  nCurBal;//��ǰ���
	int  nCurCanUse;//��ǰ����
	int  nCurCanGet;//��ǰ����
	int  nOrderSellQty;//ί����������
	int  nBackSellQty;//�ر���������
	int  nBackBuyQty;//�ر���������
	int  nDayDeposit;//���մ������
	double  dDayDepositReal;//���մ���ʵ��
	int    nDayDraw;//�����������
	double dDayDrawReal;//�������ʵ��
	double dDiffStorage;//����������
	double dUnclearDiffStor;//δ����������
	double dOutDiff;//�����������
	int   nLastTakeFroz;//���������������
	int   nDayTakeFroz;//���������������
	int   nImpawnOut;//��������
	int   nImpawnIn;//��Ȩ����
	int   nDayImpawnOut;//���ճ�������
	int   nDayImpawnIn;//������Ȩ����
	int   nDayInCancel;//��Ѻ����(��Ȩ��)
	int   nDayOutCancel;//��Ѻ����(���ʷ�)
	int   nLastLongFroz;//���ճ��ڶ�������
	int   nDayLongFroz;//���ճ��ڶ�������
	int   nLentQty;//�������
	int   nBorrowQty;//��������
	int   nDayLent;//���ս��
	int   nDayBorrow;//���ս���
	int   nDayShiftOut;//���ջ���
	int   nDayShiftIn;//���ջ���
	int   nTotalDeposit;//�������
	int   nTotalTake;//�������
	int   nStorAccu;//������
}S_MDB_STOR_INFO;


//������
typedef struct
{
	char sTxAcc[21];//�����˺�
	char sExchCode[2];//����������
	char sSeatNo[7];//ϯλ����
	char sTxCode[21];//���ױ���
	unsigned int  nContractNo;//��Լ���
	char  sContractCode[21];//��Լ����
	char sClientSerial[19];//�ͻ�����ˮ
	char sLocalSerial[38];//���ر�����
	char sSystemNo[21];//������������
	int    nOrderNum;//ί������
	double dOrderPrice;//ί�м۸�
	int    nRemainNum;//ʣ������   �ɽ��ر�����Ĵ�����
	char   sEoFlag[2];//��ƽ��־
	char   sBsFlag[2];//������־
	char   sShFlag[2];//Ͷ����־
	char   sTxDate[9]; //�������� yyyymmdd
	double dAmt;//���
	double dFrozAmt;//������  �ڻ�Ϊ��֤�� �ֻ�֤ȯΪ��� ����  
	double dExFee;//���׷���   ��������
	char   sTxType[2];//��������
	char   sCmdType[2];//ָ������
	char   sCmdAttr[2];//ָ������
	char   sForceFlag[2];//ǿƽ��־
	char   sOrderStatus[2];//ί��״̬ o �ѱ��� p ���ֳɽ�	c  ȫ���ɽ�	d  �ѳ���  s  ϵͳ����	1  �����걨	2  ��Ч 3  ���ɲ���
	char   sCancelFlag[2];//������־
	char   sAnsTime[17];//Ӧ��ʱ��   yyyymmdd hhmmss
	char   sOrderChannel[3];//ί����Դ��������
	char   sAccOrg[13];//�˻���������
	char   sTxOrg[13]; //��������(����)
	char   sOrderOper[21];//ί�н���Ա/����Ա/�ͻ���
	char   sCancelChannel[3];//������Դ ����
	char   sCancelOper[21];//��������Ա
	char   sCancelTime[17];//����ʱ�� yyyymmdd hhmmss
	char   sOrderRsp[201];//������Ӧ
	double dFrozUseamt;//�����������
	double dFrozOutamt;//�����������
	int    nCancelNum;//��������
	int    nSyncFlag;//ͬ����־  0 ����ͬ��  1����ͬ��  2����ͬ��  3�ɽ�ͬ�� 4�����ɽ�ͬ�� 5�ر�ͬ��
	double dDoneAvp;//�ɽ�����
	int    nVarietyId;//Ʒ�ֱ��
}S_MDB_ORDER_INFO;


//�ɽ���
typedef struct
{
	char sDoneNo[21];//�ɽ����
	char sSystemNo[21];//ϵͳ������
	char sExchCode[2];//����������
	char sSeatNo[7];//ϯλ����
	char sTxCode[21];//���ױ���
	char sLocalNo[38];//���ر�����
	char sTxAcc[21];//�����˺�
	char sDoneDate[9];//�ɽ�����
	char sDoneTime[17];//�ɽ�ʱ��
	unsigned int  nContractNo;//��Լ���
	char sContractCode[21];//��Լ����
	double dDonePrice;//�ɽ���
	int   nDoneNum;//�ɽ�����  (��)
	char   sBsFlag[2];//������־
	char   sShFlag[2];//Ͷ����־
	char   sEoFlag[2];//��ƽ��־
	char   sTxType[2];//��������
	char   sCmdType[2];//ָ������
	char   sCmdAttr[2];//�ɽ�����
	double  dAmt;//���׽��           -���� +����   ����������
	double  dExFee;//���������׷���   ����
	double  dMemberFee;//��Ա���׷��� ����
	double  dExMargin;//��������֤��  ����
	double  dMemberMargin;//��Ա��֤�� ����
	double  dFloatSurplus;//����ӯ��
	double  dCovSurplus;//ƽ��ӯ��
	char    sTxOrg[13];//��������
	char    sAccOrg[13];//ǩԼ����
	char    sForceFlag[2];//ǿƽ��־
	char    sOrderChannel[3];//ί������
	char    sOrderOper[21];//ί�в���Ա���ͻ�
	char    sSystemDate[9];//������Ȼ��
	double  dUnfrozeAmt;//�ⶳ���   ��֤����� ����
	double  dUnfrozeFee;//�ⶳ����  ����
	int    nSyncFlag;//ͬ����־  0 ����ͬ��   3�ɽ�ͬ��
	int    nVarietyId;
}S_MDB_DONE_INFO;


//������
typedef struct
{
	char sTxDate[9];//��������
	char sTxAcc[21];//�����˻�
	char sTxSerial[31];//������ˮ
	char sBankAcc[24];//�����˺�
	char sBankName[41];//��������
	char sCurrency[2];//����
	double  dAmt;//���
	char  sFundFlag[2];//�ʽ���
	double dLastAmt;//����ǰ����
	double dCurAmt;//���������
}S_MDB_FUND_FLOW;


typedef struct 
{
	unsigned int  nContractNo;//��Լ���
	char sContractCode[21];//��Լ����
	char sContractName[41];//��Լ����
	unsigned int  nDate;//��������
	char sTime[18];//����ʱ��
	unsigned int  nQuoSerial;//�������
	double dLastSetllePrice;//�����
	double dLastClosePrice;//������
	double dOpenPrice;//����
	double dHighPrice;//��߼�
	double dLowPrice;//��ͼ�
	double dNewPrice;//���¼�
	double dClosePrice;//���̼�
	double dSettlePrice;//�����
	double dBuyPrice1;//���1
	double dBuyPrice2;//���2
	double dBuyPrice3;//���3
	double dBuyPrice4;//���4
	double dBuyPrice5;//���5
	double dBuyPrice6;//���6
	double dBuyPrice7;//���7
	double dBuyPrice8;//���8
	double dBuyPrice9;//���9
	double dBuyPrice10;//���10
	int    nBuy1;//����1
	int    nBuy2;//����2
	int    nBuy3;//����3
	int    nBuy4;//����4
	int    nBuy5;//����5
	int    nBuy6;//����6
	int    nBuy7;//����7
	int    nBuy8;//����8
	int    nBuy9;//����9
	int    nBuy10;//����10
	double dSellPrice1;//����1
	double dSellPrice2;//����2
	double dSellPrice3;//����3
	double dSellPrice4;//����4
	double dSellPrice5;//����5
	double dSellPrice6;//����6
	double dSellPrice7;//����7
	double dSellPrice8;//����8
	double dSellPrice9;//����9
	double dSellPrice10;//����10
	int    nSell1;//����1
	int    nSell2;//����2
	int    nSell3;//����3
	int    nSell4;//����4
	int    nSell5;//����5
	int    nSell6;//����6
	int    nSell7;//����7
	int    nSell8;//����8
	int    nSell9;//����9
	int    nSell10;//����10
	unsigned int    nDoneQty;//�ɽ���  ��
	double dDoneWeight;//�ɽ�����
	unsigned int    nPosition;//�ֲ���
	double dUpLimit;//��ͣ��
	double dDownLimit;//��ͣ��
	double dUpDown;//�ǵ�
	double dUpDownRate;//�ǵ���
	double dDownAmt;//�ɽ����
	double dAvp;//����
}S_MDB_QUOTATION;

//���պ�Լ
typedef struct 
{
	unsigned int nContractNo;//��Լ���
	char   sContractCode[21];//��Լ����
	double dNewPrice;//���¼�
	double dUpDown;//�ǵ�
	double dUpDownRate;//�ǵ���
	double dBuyPrice;//���
	double dSellPrice;//����
	double dHighPrice;//���
	double dLowPrice;//���
	int    nDoneQty;//�ɽ���
	char   sRiskDesc[81];//����˵��
	char   sRiskFlag[2];//���ձ�־ '1' �������� 0ȡ������
	unsigned int nRiskdate;//������������
	unsigned int nRisktime;//��������ʱ��
}S_MDB_RISKCONTRACT_INFO;

//�ͻ��ֲַ���
typedef struct 
{
	char sTxAcc[21];//�����˻�
	unsigned int nContractNo;//��Լ���
	char   sContractCode[21];//��Լ����
	char   sShFlag[2];//Ͷ����־
	int    nBuyQty;//�����
	int    nSellQty;//������
	unsigned short  nWarnLevel;//Ԥ������
	char   sRiskDesc[81];//����˵��
	char   sRiskFlag[2];//���ձ�־ '1' �������� 0ȡ������
	unsigned int nRiskdate;//������������
	unsigned int nRisktime;//��������ʱ��
}S_MDB_CUST_POSI_RISK;
typedef struct  
{
	char sOper[17];//����Ա
	char sSubcribeType[2];//�������� '0' ����
}S_MDB_SUBSCRIBE;

typedef struct  
{
	char sDate[9];//��������
	char sProdCode[13];//��Ʒ���
	int  nRiskLevel;//���ռ��𣬱������
	double dRisk;//���ն�
	double dUnitValue;//��λ��ֵ
	double dBal;//��Ȩ��
	double dMargin;//�ܱ�֤��
	char sRight[2];//����Ȩ��
	char   sRiskDesc[81];//����˵��
	char   sRiskFlag[2];//���ձ�־ '1' �������� 0ȡ������
	unsigned int nRiskdate;//������������
	unsigned int nRisktime;//��������ʱ��
}S_MDB_PROD_FUND_RISK;//��Ʒ�ʽ���ձ�

typedef struct  
{
	char sDate[9];//��������
	char sProdCode[13];//��Ʒ���
	int  nRiskLevel;//���ռ��𣬱������
	double dMarginRate;//��֤��ռ��
	double dPosiValueRate;//�ֲּ�ֵ��
	double dPosiDiffValueRate;//�ֲ������ֵ��
	double dT_Bal;//��Ȩ��
	double dT_Buy;//����ֲּ�ֵ
	double dT_Sell;//�����ֲּ�ֵ
	char sRight[2];//����Ȩ��
	char   sRiskDesc[81];//����˵��
	char   sRiskFlag[2];//���ձ�־ '1' �������� 0ȡ������
	unsigned int nRiskdate;//������������
	unsigned int nRisktime;//��������ʱ��
}S_MDB_PROD_POSI_RISK;//��Ʒ�ֲַ��ձ�

typedef struct  
{
	char sDate[9];//��������
	char sOrgCode[13];//��������
	char sRiskType[2];//������� 0��������  1�������
	int  nRiskLevel;//���ռ��𣬱������
	double dRisk;//���ն�
	double dUnitValue;//��λ��ֵ
	double dBal;//��Ȩ��
	double dMargin;//�ܱ�֤��
	char sRight[2];//����Ȩ��
	char   sRiskDesc[81];//����˵��
	char   sRiskFlag[2];//���ձ�־ '1' �������� 0ȡ������
	unsigned int nRiskdate;//������������
	unsigned int nRisktime;//��������ʱ��
}S_MDB_ORG_FUND_RISK;//�����ʽ���ձ�

typedef struct  
{
	char sDate[9];//��������
	char sOrgCode[13];//��������
	char sRiskType[2];//������� 0��������  1�������
	int  nRiskLevel;//���ռ��𣬱������
	double dMarginRate;//��֤��ռ��
	double dPosiValueRate;//�ֲּ�ֵ��
	double dPosiDiffValueRate;//�ֲ������ֵ��
	double dT_Bal;//��Ȩ��
	double dT_Buy;//����ֲּ�ֵ
	double dT_Sell;//�����ֲּ�ֵ
	char sRight[2];//����Ȩ��
	char   sRiskDesc[81];//����˵��
	char   sRiskFlag[2];//���ձ�־ '1' �������� 0ȡ������
	unsigned int nRiskdate;//������������
	unsigned int nRisktime;//��������ʱ��
}S_MDB_ORG_POSI_RISK;//�����ֲַ��ձ�


typedef struct  
{
	char sTxAcc[21];//�����˻�
	int nVarietyId;//Ʒ�ֱ��
	int      nBuyPosi;//�����
	int      nSellPosi;//�ղ���
	double   dBuyMargin;    //��ֱ�֤��
	double   dSellMargin;   //�ղֱ�֤��
	int      nMarginFlag;   //��֤���߾��߷��� 1��� 2�ղ�
	int      nBuyQty;//����ʣ������
	int      nSellQty;//����ʣ������
	double   dBuyFroze;     //�����򶳽�
	double   dSellFroze;    //����������
	int      nFrozeFlag;    //���᷽��    1��� 2�ղ�
	double      dFrozeAmt;     //����������   ����+���У��õ������ټ�ȥ�ֲֽ����õ��ö�����
	char     sBailMode;     //��֤��ģʽ  BAIL_MODE_BOTH�� ���ߴ��
}S_MDB_BIG_MARGIN;//��߱�֤���

//�ֲ������
typedef struct 
{
	char sTxDate[9];//��������
	char sTxAcc[21];//�ʽ��˺�
	char sSeatNo[7];//ϯλ��
	char sShFlag[2];//Ͷ����־
	char sBsFlag[2];//������־
	int  nContractNo;//��Լ���
	char sContractCode[21];//��Լ����
	int  nDiffQty;//��������
	char sSerial[31];//��ˮ��
}S_MDB_DIFF_POSI; 

//�ͻ�����
class CTbl_Cust_Online
{
protected:
	
	CMemTableNew <S_MDB_CUST_ONLINE>      m_table;
// 	CPkeyCharF<21,2>               m_pkey;//�ͻ�+����
// 	CIndexCharF<2,1>            m_index_loginstatus; //����¼״̬������
// 	CIndexCharF<21,1>            m_index_cust; //�ͻ�
	CPkeyCharV<2>               m_pkey;//�ͻ�+����
	CIndexVarChar<1>            m_index_loginstatus; //����¼״̬������
	CIndexVarChar<1>            m_index_cust; //�ͻ�
	CBF_Mutex                   m_mutex;//������
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

	// ������: Select
	// ���  : ������ 2018-1-29 15:25:03
	// ����  : bool 
	// ����  : char *oper
	// ����  : std::vector<S_MDB_OPER_ONLINE *> &onlinelist
	// ����  : ���ݿͻ���ȡ���ͻ�������Ϣ
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
	// ������: SelectByStatus
	// ���  : ������ 2018-1-29 15:25:03
	// ����  : bool 
	// ����  : char *status ״̬
	// ����  : std::vector<S_MDB_OPER_ONLINE *> &onlinelist
	// ����  : ���ݲ���Աȡ������Ա������Ϣ
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

	// ������: Update
	// ���  : ������ 2018-1-29 15:41:53
	// ����  : int  -1���� >0 rid 
	// ����  : S_MDB_OPER_ONLINE online
	// ����  : �������߱�û�����ӣ��и��¡��м���
	int Update(S_MDB_CUST_ONLINE online)
	{
		int rid;
		CBF_PMutex pp(&m_mutex);
		if (m_pkey.GetRowId(rid,online.sCustNo,online.sTermType))
		{
			//����
			if (strcmp(online.sOnlineStatus,m_table.m_table[rid].sOnlineStatus)!=0) //״̬��ͬ
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
				//��ɾ������������
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
// 	CPkeyCharF<21,3>               m_pkey;//�ʽ��˺�+������+ϯλ
// 	CIndexCharF<21,1>            m_index_txacc; //���ʽ��˺ŵ�����
// 	CIndexCharF<21,3>            m_index_tradecode; //������+ϯλ+���ױ���
// 	CIndexCharF<2,1>            m_index_exch; //��������������
	CPkeyCharV<3>               m_pkey;//�ʽ��˺�+������+ϯλ
	CIndexVarChar<1>            m_index_txacc; //���ʽ��˺ŵ�����
	CIndexVarChar<3>            m_index_tradecode; //������+ϯλ+���ױ���
	CIndexVarChar<1>            m_index_exch; //��������������
	CBF_Mutex                   m_mutex;//������
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

	// ������: Select
	// ���  : ������ 2018-1-29 15:25:03
	// ����  : bool 
	// ����  : char *txacc
	// ����  : std::vector<S_MDB_CUST_TRADECODE *> &reslist
	// ����  : ���ݲ���Աȡ������Ա������Ϣ
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
	// ������: Select
	// ���  : ������ 2018-1-30 13:47:52
	// ����  : bool 
	// ����  : char *excode
	// ����  : char *seatno
	// ����  : char *txcode
	// ����  : std::vector<S_MDB_CUST_TRADECODE *> &reslist
	// ����  : ���ݽ�������ϯλ���ױ���ȡ�ʽ��˺�
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


//������ϯλ����Ա���߱�  
class CTbl_Exch_Trader
{
protected:
	
	CMemTableNew <S_MDB_EXCH_TRADER_INFO>   m_table;
// 	CPkeyCharF<17,3>               m_pkey;//���������롢ϯλ���롢����Ա����
// 	CIndexCharF<7,3>            m_index_exch_seat_status; //��������ϯλ����¼״̬
// 	CIndexCharF<7,2>            m_index_exch_seat; //��������ϯλ
	CPkeyCharV<3>               m_pkey;//���������롢ϯλ���롢����Ա����
	CIndexVarChar<3>            m_index_exch_seat_status; //��������ϯλ����¼״̬
	CIndexVarChar<2>            m_index_exch_seat; //��������ϯλ
	CBF_Mutex                   m_mutex;//������
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

	// ������: Select
	// ���  : ������ 2018-1-30 16:30:25
	// ����  : bool 
	// ����  : char *exchcode
	// ����  : char *seatno
	// ����  : char *status
	// ����  : std::vector<S_MDB_EXCH_TRADER_INFO *> &exlist
	// ����  : ���ݽ�������ϯλ��״̬�����Ҽ�¼
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

	// ������: Select
	// ���  : ������ 2018-1-30 16:31:06
	// ����  : bool 
	// ����  : char *exchcode
	// ����  : char *seatno
	// ����  : std::vector<S_MDB_EXCH_TRADER_INFO *> &exlist
	// ����  : ���ݽ�������ϯλ�����Ҽ�¼
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


//�ʽ��  S_MDB_CUST_FUND
class CTbl_Fund
{
protected:
	
	CMemTableNew <S_MDB_CUST_FUND>  m_table;
//	CPkeyCharF<21,1>               m_pkey;//�����˺�
	CPkeyCharV<2>               m_pkey;//�����˺� ����
	CIndexVarChar<1>            m_indexAcc;//�����˺�
	CBF_Mutex                   m_mutex;//������
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

//����tick��  S_MDB_CUST_RISK_TICK
class CTbl_Risk_Tick
{
protected:

	CMemTableNew <S_MDB_CUST_RISK_TICK>  m_table;
	//	CPkeyCharF<21,1>               m_pkey;//�����˺�
	CPkeyCharV<3>               m_pkey;//�����˺� ���� ʱ��
	CIndexVarChar<1>            m_indexAcc;//�����˺�
	CIndexVarChar<2>            m_indexAccCurrency;//�����˺� ����
	CBF_Mutex                   m_mutex;//������
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

//�ʽ���ϸ��
class CTbl_FundDetail
{
protected:
	
	CMemTableNew <S_MDB_CUST_FUND_DETAIL>  m_table;
// 	CPkeyCharF<21,3>               m_pkey;//�ʽ��˺�+�ʽ�����+�ʽ���
// 	CIndexCharF<21,1>            m_indexAcc;//�ʽ��˺�
// 	CIndexCharF<21,2>            m_indexAccFundtype;//�ʽ��˺�+�ʽ�����
	CPkeyCharV<4>               m_pkey;//�ʽ��˺�+����+�ʽ�����+�ʽ���
	CIndexVarChar<1>            m_indexAcc;//�ʽ��˺�
	CIndexVarChar<2>            m_indexAccCurr;//�ʽ��˺�+����
	CIndexVarChar<3>            m_indexAccFundtype;//�ʽ��˺�+����+�ʽ�����
	CBF_Mutex                   m_mutex;//������
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


//�ֱֲ���ʽ��˺�+��Լ��ŵ�����
class Index_Position_Txacc_contract : public CIndex_Field_Base
{
public:
	char  sTxAcc[21];//�ʽ��˺�
	int   nContractNo;//��Լ���


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
//�ֱֲ���ʽ��˺�+��Լ��ŵ�����
class Index_Position_Txacc_Variety : public CIndex_Field_Base
{
public:
	char  sTxAcc[21];//�ʽ��˺�
	int   nVarietyId;//Ʒ�ֱ��


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
//�ֱֲ���ʽ��˺�+ϯλ+��Լ���_Ͷ����־������
class Index_Position_Txacc_seat_contract_shflag : public CIndex_Field_Base
{
public:
	char  sTxAcc[21];//�ʽ��˺�
	char  sSeatNo[7];//ϯλ����
	int   nContractNo;//��Լ���
	char  sShFlag[2];//Ͷ����־
	
	
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
//�ֱֲ� S_MDB_CUST_POSITION
class CTbl_Position
{
protected:
	
	CMemTableNew <S_MDB_CUST_POSITION>  m_table;
	CKeyField<Index_Position_Txacc_seat_contract_shflag> m_keyno;  //�ʽ��˺�+ϯλ+��Լ���+Ͷ����־
	CIndexField<Index_Position_Txacc_contract>   m_index_AccNo; //�ʽ��˺�+��Լ���
	CIndexField<Index_Position_Txacc_Variety>   m_index_AccVariety; //�ʽ��˺�+Ʒ�ֱ��
// 	CPkeyCharF<21,4>                            m_keycode;//�ʽ��˺�+ϯλ+��Լ����+Ͷ����־
// 	CIndexCharF<21,2>            m_index_acccode;//�ʽ��˺�+��Լ����
// 	CIndexCharF<21,1>            m_index_acc;//�ʽ��˺�
// 	CIndexCharF<21,1>            m_index_contractcode;//��Լ����

	CPkeyCharV<4>                            m_keycode;//�ʽ��˺�+ϯλ+��Լ����+Ͷ����־
	CIndexVarChar<2>            m_index_acccode;//�ʽ��˺�+��Լ����
	CIndexVarChar<1>            m_index_acc;//�ʽ��˺�
	CIndexVarChar<1>            m_index_contractcode;//��Լ����
	CIndexInt<1>                m_index_contractno;//��Լ���
	CBF_Mutex                   m_mutex;//������
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
		//���ֲ�ѯ������ʵ��һ���ĸ�Ч�ʸ���
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
		//���ֲ�ѯ������ʵ��һ���ĸ�Ч�ʸ���
		int rid;
		bool bret = m_index_acc.Find(rid,txacc);
		if (!bret)
		{
			return false;
		}
		while (bret)
		{
			m_table.m_table[rid].nLastPB=0;//������� ��ͷ�ֲ�
			m_table.m_table[rid].nLastPS=0;//��������  ��ͷ�ֲ�
			m_table.m_table[rid].nCurPB=0;//��ǰ���
			m_table.m_table[rid].nCurPS=0;//��ǰ�ղ�
			m_table.m_table[rid].nCurCanUsePB=0;//��ǰ���ö��
			m_table.m_table[rid].nCurCanUsePS=0;//��ǰ���ÿղ�
			m_table.m_table[rid].nDayPB=0;//���쿪���
			m_table.m_table[rid].nDayPS=0;//���쿪�ղ�
			m_table.m_table[rid].nDAY_COV_PB_Froze=0;//����ƽ��ֶ���
			m_table.m_table[rid].nDAY_COV_PB=0;//����ƽ���
			m_table.m_table[rid].nDAY_COV_PS_Froze=0;//����ƽ�ղֶ���
			m_table.m_table[rid].nDAY_COV_PS=0;//����ƽ�ղ�
			m_table.m_table[rid].nDAY_DELI_PB_Froze=0;//���콻�ն��
			m_table.m_table[rid].nDAY_DELI_PB=0;//���콻�ն��
			m_table.m_table[rid].nDAY_DELI_PS_Froze=0;//���콻�տղֶ���
			m_table.m_table[rid].nDAY_DELI_PS=0;//���콻�տղ�
			m_table.m_table[rid].nDAY_MIDD_PB=0;//�����ֶ��
			m_table.m_table[rid].nDAY_MIDD_PS=0;//�����ֿղ�
			m_table.m_table[rid].dPB_OAvp=0.0;//��ֿ��־���    �ֻ���Ʊ���ֳɱ�����
			m_table.m_table[rid].dPS_OAvp=0.0;//�ղֿ��־���
			m_table.m_table[rid].dPB_Avp=0.0;//��ֲֳ־���         �ֻ���Ʊ�ɱ�����(ͬ��Ʊ������ӯ����ɱ����ͣ���������ɱ�����)
			m_table.m_table[rid].dPS_Avp=0.0;//�ղֲֳ־���
			m_table.m_table[rid].dPB_Margin=0.0;//��ֱֲֳ�֤��    �ֻ���Ʊ�ֲֳɱ�
			m_table.m_table[rid].dPS_Margin=0.0;//�ղֱֲֳ�֤��
			m_table.m_table[rid].dDAY_PB_SURPLUS=0.0;//���ն�ͷӯ��
			m_table.m_table[rid].dDAY_PS_SURPLUS=0.0;//���տ�ͷӯ��
			m_table.m_table[rid].dLAST_SETTLE_PRICE=0.0;//���ս����
			m_table.m_table[rid].dSETTLE_PRICE=0.0;//�����
			
			bret = m_index_acc.Next(rid);
		}
		return true;
	}
	bool SelectByAccContract(char *txacc,int contractno,std::vector<S_MDB_CUST_POSITION *> &posilist)
	{
		CBF_PMutex pp(&m_mutex);
		//���ֲ�ѯ������ʵ��һ���ĸ�Ч�ʸ���
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
		//���ֲ�ѯ������ʵ��һ���ĸ�Ч�ʸ���
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
		//���ֲ�ѯ������ʵ��һ���ĸ�Ч�ʸ���
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


//����  S_MDB_STOR_INFO
class CTbl_Storage
{
protected:
	
	CMemTableNew <S_MDB_STOR_INFO>  m_table;
// 	CPkeyCharF<21,2>               m_pkey;//�����˺�+Ʒ�ִ���
// 	CIndexCharF<21,1>            m_index_acc;//�����˺�
// 	CIndexCharF<7,1>            m_index_prodcode;//Ʒ�ִ���

	CPkeyCharV<2>               m_pkey;//�����˺�+Ʒ�ִ���
	CIndexVarChar<1>            m_index_acc;//�����˺�
	CIndexVarChar<1>            m_index_prodcode;//Ʒ�ִ���
	CBF_Mutex                   m_mutex;//������
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

//������ S_ORDER_INFO
class CTbl_Order
{
protected:
	
	CMemTableNew <S_MDB_ORDER_INFO>  m_table;
// 	CPkeyCharF<38,1>               m_pkey;//���ر����ż�
// 	CIndexCharF<21,1>            m_index_acc;//�����˺�
// 	CIndexCharF<19,1>            m_index_frontserial;//ǰ����ˮ��
// 	CIndexCharF<21,2>            m_index_operfrontserial;//����Ա+ǰ����ˮ��
// 	CIndexCharF<21,1>            m_index_oper;//����Ա�ͻ�
// 	CIndexCharF<21,1>            m_index_systemno;//ϵͳ������
// 	CIndexCharF<9,2>            m_index_date_exch;//��������+����������

	CPkeyCharV<1>               m_pkey;//���ر����ż�
	CIndexVarChar<1>            m_index_acc;//�����˺�
	CIndexVarChar<1>            m_index_frontserial;//ǰ����ˮ��
	CIndexVarChar<2>            m_index_operfrontserial;//����Ա+ǰ����ˮ��
	CIndexVarChar<1>            m_index_oper;//����Ա�ͻ�
	CIndexVarChar<2>            m_index_systemno;//����������+ϵͳ������
	CIndexVarChar<2>            m_index_date_exch;//��������+����������
	CBF_Mutex                   m_mutex;//������
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

	// ������: Update
	// ���  : ������ 2017-8-7 15:58:37
	// ����  : bool 
	// ����  : int rid
	// ����  : char *excode  ����������
	// ����  : char *systemno ϵͳ������
	// ����  : �޸������ļ�ֵ������ֱ���޸�ָ�룬�����ȵ��ñ����������ؽ�ϵͳ������������Ȼ����ȥ�޸�ָ��
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

	// ������: Update
	// ���  : ������ 2017-8-8 9:17:43
	// ����  : int 
	// ����  : char *localno
	// ����  : char *excode  ����������
	// ����  : char *systemno ϵͳ������
	// ����  : ���ݱ��ر������޸�ϵͳ�����ţ�������rowid  ��Ϊϵͳ������������������ֱ���޸�ָ���ֵ
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

	// ������: SelectByOper
	// ���  : ������ 2018-3-16 10:52:59
	// ����  : bool 
	// ����  : char *oper
	// ����  : std::vector<S_MDB_ORDER_INFO *> &orderlist
	// ����  : ���ݲ���Ա/�ͻ���ѯ���������ͻ�����ˮ��С��������
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
	// ������: Delete
	// ���  : ������ 2017-8-8 10:48:53
	// ����  : int 
	// ����  : char *txdate
	// ����  : char *exchcode
	// ����  : ɾ���������ڼ�����������ı���
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


//�ɽ���  S_DONE_INFO
class CTbl_Done
{
protected:
	
	CMemTableNew <S_MDB_DONE_INFO>     m_table;

	CPkeyCharV<3>               m_pkey;//����������+������ʶ+�ɽ����
	CIndexVarChar<1>            m_index_acc;//�����˺�
	CIndexVarChar<1>            m_index_localno;//���ر�����
	CIndexVarChar<1>            m_index_systemno;//ϵͳ������
	CIndexVarChar<2>            m_index_date_exch;//��������+����������
	CIndexVarChar<1>            m_index_oper;//��������Ա/�ͻ�
	CBF_Mutex                   m_mutex;//������
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

	// ������: Delete
	// ���  : ������ 2017-8-8 10:48:53
	// ����  : int 
	// ����  : char *txdate
	// ����  : char *exchcode
	// ����  : ɾ���������ڼ�����������ĳɽ�
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

//������ S_FUND_FLOW
class CTbl_Fund_Flow
{
protected:
	
	CMemTableNew <S_MDB_FUND_FLOW>     m_table;
// 	CPkeyCharF<31,2>               m_pkey;//��������+��ˮ��
// 	CIndexCharF<21,1>            m_index_txacc;//�����˺�
// 	CIndexCharF<21,2>            m_index_acc_inout;//�����˺�+�������
	CPkeyCharV<2>               m_pkey;//��������+��ˮ��
	CIndexVarChar<1>            m_index_txacc;//�����˺�
	CIndexVarChar<2>            m_index_acc_inout;//�����˺�+�������
	CIndexVarChar<2>            m_index_txaccdate;//�����˺�+����
	CBF_Mutex                   m_mutex;//������
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

//�����  S_MDB_QUOTATION �������µ�����
class CTbl_Quotation
{
protected:
	
	CMemTableNew <S_MDB_QUOTATION>  m_table;
	CPkeyInt<1>                 m_keyno;//��Լ���
//	CPkeyCharF<21,1>               m_keycode;//��Լ����
	CPkeyCharV<1>               m_keycode;//��Լ����
	CBF_Mutex                   m_mutex;//������
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

	// ������: Update
	// ���  : ������ 2018-3-5 9:49:37
	// ����  : bool 
	// ����  : S_MDB_QUOTATION &info
	// ����  : ���ݺ�Լ��š��������ڡ������������������
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
		//�Ƚ��������ڼ�������ţ��Ƿ�������
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
//���պ�Լ��Ϣ
class  CTbl_RiskContractInfo
{
protected:
	
	CMemTableNew <S_MDB_RISKCONTRACT_INFO> m_table;
	CPkeyUInt<1>                     m_key;//��Լ���
	CBF_Mutex                   m_mutex;//������
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
	int Insert(S_MDB_RISKCONTRACT_INFO info) //ֻ���ڳ�ʼ��ʱʹ�ã����Կ��Բ�����
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

	// ������: Select
	// ���  : ������ 2018-3-5 21:11:59
	// ����  : int 
	// ����  : std::vector<S_MDB_RISKCONTRACT_INFO *> &reslist
	// ����  : ȡ�����պ�Լ
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


//�ͻ��ֲַ���
class  CTbl_Cust_Posi_Risk
{
protected:
	
	CMemTableNew <S_MDB_CUST_POSI_RISK> m_table;
//	CPkeyCharF<21,1>                     m_key;//�ʽ��˺�
	CPkeyCharV<2>                     m_key;//�ʽ��˺�+��Լ
	CIndexVarChar<1>                  m_index;//�ʽ��˺�
	CBF_Mutex                   m_mutex;//������
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
	// ������: Select
	// ���  : ������ 2018-3-5 21:11:59
	// ����  : int 
	// ����  : std::vector<S_MDB_RISKCONTRACT_INFO *> &reslist
	// ����  : ȡ���ͻ��ֲַ���
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
// 	CPkeyCharF<17,2>                     m_key;//����Ա+��������
// 	CIndexCharF<2,1>                    m_index;//��������

	CPkeyCharV<2>                     m_key;//����Ա+��������
	CIndexVarChar<1>                    m_index;//��������
	CBF_Mutex                   m_mutex;//������
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

//��Ʒ�ʽ���ձ�
class  CTbl_Prod_Fund_Risk
{
protected:
	
	CMemTableNew <S_MDB_PROD_FUND_RISK> m_table;
//	CPkeyCharF<13,1>                     m_key;//��Ʒ���
	CPkeyCharV<1>                     m_key;//��Ʒ���
	CBF_Mutex                   m_mutex;//������
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

//��Ʒ�ֲַ��ձ�
class  CTbl_Prod_Posi_Risk
{
protected:
	
	CMemTableNew <S_MDB_PROD_POSI_RISK> m_table;
//	CPkeyCharF<13,1>                     m_key;//��Ʒ���
	CPkeyCharV<1>                     m_key;//��Ʒ���
	CBF_Mutex                   m_mutex;//������
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

//�����ʽ���ձ�
class  CTbl_Org_Fund_Risk
{
protected:

	CMemTableNew <S_MDB_ORG_FUND_RISK> m_table;
	CPkeyCharV<2>                     m_key;//��������+��������
	CIndexVarChar<1>                  m_index;//��������
	CBF_Mutex                   m_mutex;//������
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

//�����ֲַ��ձ�
class  CTbl_Org_Posi_Risk
{
protected:

	CMemTableNew <S_MDB_ORG_POSI_RISK> m_table;
	CPkeyCharV<2>                     m_key;//��������+��������
	CIndexVarChar<1>                  m_index;//��������
	CBF_Mutex                   m_mutex;//������
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

//��߱�֤���
class  CTbl_Big_Margin
{
protected:

	CMemTableNew <S_MDB_BIG_MARGIN> m_table;
	CKeyField<Index_Position_Txacc_Variety>   m_key; //�ʽ��˺�+Ʒ�ֱ��
	CIndexCharV                               m_index;// �ʽ��˺�
	CBF_Mutex                   m_mutex;//������
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


//�ֲ������  S_MDB_DIFF_POSI
class CTbl_Diff_Posi
{
protected:

	CMemTableNew <S_MDB_DIFF_POSI>  m_table;
	CPkeyCharV<2>               m_pkey;//���� ��ˮ��
	CIndexVarChar<1>            m_indexAcc;//�����˺�
	CBF_Mutex                   m_mutex;//������
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
	CTbl_Cust_Online     m_pCustOnline;//�ͻ����߱�
	CTbl_Cust_Tradecode  m_pTradecode;//�ʽ��˺Ž��ױ���󶨱�
	CTbl_Exch_Trader     m_pExchTrader;//������ϯλ����Ա���߱�

	CTbl_Fund            m_pFund; //�ʽ��
	CTbl_FundDetail      m_pFundDetail;//�ʽ���ϸ��
	CTbl_Position        m_pPosition;//�ֱֲ�
	CTbl_Storage         m_pStore;//����

	CTbl_Order           m_pOrder;//������
	CTbl_Done            m_pDone;//�ɽ���
	CTbl_Fund_Flow       m_pFundFlow;//�����
	CTbl_Quotation       m_pQuotation;//ʵʱ�����
	
	CTbl_RiskContractInfo    m_pRiskContractInfo;//���պ�Լ��Ϣ��
	CTbl_Cust_Posi_Risk      m_pCustPosiRisk;//�ͻ��ֲַ��ձ�

	//���ı�
	CTbl_Oper_Subcribe       m_pSubcribe;//����Ա���ı�
	CTbl_Prod_Fund_Risk      m_pProdFundRisk;//��Ʒ�ʽ����
	CTbl_Prod_Posi_Risk      m_pProdPosiRisk;//��Ʒ�ֲַ���
	
	CTbl_Big_Margin          m_pBigNetMargin;//��߾��߱�֤���

	CTbl_Diff_Posi           m_pDiffPosi;//�ֲ������

	CTbl_Risk_Tick           m_pRiskTick;//�ռ���ձ�

	CTbl_Org_Fund_Risk      m_pOrgFundRisk;//�����ʽ���ձ�
	CTbl_Org_Posi_Risk      m_pOrgPosiRisk;//�����ֲַ��ձ�
};

#endif // !defined(AFX_DB_TRADER_H__6EECD875_4F24_4FFA_9024_3DEBCD57D140__INCLUDED_)
