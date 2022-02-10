// Db_BaseInfo.h: interface for the CDb_BaseInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DB_BASEINFO_H__48668C3F_4A61_47F6_9292_CE6DD88F582B__INCLUDED_)
#define AFX_DB_BASEINFO_H__48668C3F_4A61_47F6_9292_CE6DD88F582B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//���ݿ�  ������Ϣ   ��Լ Ʒ�� ������ �г���

#include "CodeTable.h"

typedef struct 
{
	unsigned int nContractId;//��Լ���
	char   sContractCode[21];//��Լ����
	char   sContractName[41];//��Լ����
	char   sExcode[2];//����������
	char   sProd_type[2];//Ʒ������
	char   sProd_code[21];//����Ʒ����ĺ�Լ����
	unsigned  int    nProdId;//��ĺ�Լ��� Ʒ�ֱ��
	double     dMinChgPrice;//��С�䶯��λ
	char    sIsT0[2];//�Ƿ�T+0
	char   sIsMargin[2];//�Ƿ�֤��ģʽ
	double  dMarginRate;//��֤֤��
	char   sIsUpDownLimit[2];//�Ƿ��ǵ�ͣ����
	double  dUpRate;//��ͣ��
	double  dDownRate;//��ͣ��
	char   sActiveFlag[2];//�����־
	char   sStatus[2];//��Լ״̬
	double  dRefPrice;//�ο���
	double  dSellGetRate;//�����տ����
	char   sDeliMode[2];//����ģʽ
	unsigned int nMaxHand;//����걨����
	unsigned int nMinHand;//��С�걨����
	double   dTxUnit;//���׵�λ ��Լ����
	char    sOrderUnit[2];//���۵�λ
	char    sOrderCmd[13];//������֧�ֱ���ָ��   �޼� �м� 
	// 		��0��	ORDER_TYPE_LIMIT	�޼�
	// 		��1��	ORDER_TYPE_MARKET	�м�
	// 		��2��	ORDER_TYPE_MS_LOSS	�м�ֹ��
	// 		��3��	ORDER_TYPE_MS_PROFIT	�м�ֹӯ
	// 		��4��	ORDER_TYPE_LS_LOSS	�޼�ֹ��
	// 		��5��	ORDER_TYPE_LS_PROFIT	�޼�ֹӯ
	// 		��6��	ORDER_TYPE_M2L	�м�ת�޼�
	// 		��7��	ORDER_TYPE_LEVEL1_M	1���м�
	// 		��8��	ORDER_TYPE_LEVEL5_M	5���м�
	char    sDoneAttr[101];//������֧�ֱ�������
	char    sDueDate[9];//������
	char    sLastDate[9];//�������
	unsigned int  nDeliDays;//����������
	char    sIsBigMargin[2];//�ֲִ���ģʽ '0'˫�� '1'���� ����
	char    sIsOptionCall[2];//�Ƿ���Ȩ����
	int     nVariety;//Ʒ��    
	char    sCurrency[2];//����
}S_MDB_CONTRACT_INFO;

typedef struct 
{
	unsigned int nVarietyId;//Ʒ�ֱ��
	char   sVarietyCode[21];//Ʒ�ִ���
	char   sVarietyName[41];//Ʒ������
	char sExcode[2];//����������
	char sProd_type[2];//Ʒ������
	char sCurrency[2];//����
	double     dMinChgPrice;//��С�䶯��λ
	unsigned int nMaxHand;//����걨����
	unsigned int nMinHand;//��С�걨����
	double   dTxUnit;//���׵�λ ��Լ����
	char sOrderUnit[2];//���۵�λ
	unsigned int  nDeliDays;//����������
	char  sBailMode[2];//��֤����ȡ��ʽ
}S_MDB_VARIETY_INFO;



typedef struct
{
	char sExCode[2];//����������
	char sExName[41];//����������
	char sMarket[2];//�����г�
	char sTradeDate[9];//��������
	char sIsSeatTrade[2];//�Ƿ�ϯλ����
	char sCommPara[201];//ͨѶ����
	char sClearDate[9];//��������
	char sClearStatus[2];//����״̬ 
	char sOnlyChgDate[2];//�Ƿ���л�������
}S_MDB_EXCH_INFO;

typedef struct  
{
	char sMarket[2];//�г�
	char sMarketName[41];//�г�����
	char sTradeDate[9];//��������
	char sClearDate[9];//��������
	char sClearStatus[2];//����״̬
	char sOnlyChgDate[2];//�Ƿ���л�������
}S_MDB_MARKET_INFO;

//����Ա��Ӧ�ͻ�
typedef struct 
{
	char sOperCode[17];//����Ա����
	char sTxAcc[21];   //�ʽ��˺�
}S_MDB_OPER_CUST_INFO;

typedef struct 
{
	char sFinaProdCode[13];//��Ʒ���
	char sFinaProdName[41];//��Ʒ����
	char sLinkMan[21];//��ϵ��
	char sPhone[21];//�绰
	char sAddr[61];//��ַ
	char sOrgCode[13];//��������
	unsigned int nPublishUit;//���зݶ�
}S_MDB_FINA_PROD_INFO;

//��Ʒ��Ӧ�ʽ��˺�
typedef struct
{
	char  sProdNo[13];//��Ʒ���
	char  sCustNo[13];//�ͻ���
	char  sTxAcc[21];//�ʽ��˺�
}S_MDB_PROD_CUST;

//����Ա��Ӧ��Ʒ
typedef struct
{
	char  sOper[17];//����Ա
	char  sProdNo[13];//��Ʒ���
}S_MDB_OPER_PROD;

//���ź�Լ��������
typedef struct
{
	char  sTxAcc[21];//�ʽ��˺�
	unsigned int nContractNo;//��Լ���
	double   dForceRate;//���Ϸ���
}S_MDB_CONTRACT_LIMIT;

//���ı�
// typedef struct  
// {
// 	char sOper[17];//����Ա
// 	char sMsgType[2];//��Ϣ����
// }S_MDB_SUBSCRIBE;

typedef struct 
{
	char   sClearDate[9];//��������
	unsigned int    nContractNo;//��Լ���
	char   sContractCode[21];//��Լ����
	double dSettlePrice;//�����
	int    nHoldPosi;//�ֲ���
	double dClosePrice;//���̼�
	double dPriceChg;//�۸񲨶�
	double dSIMPLE_RETURN;//����������
	double dLOG_RETURN;//����������
	double dHighPrice;//��߼�
	double dLowPrice;//��ͼ�
	double dOpenPrice;//���̼�
}S_MDB_CONTRACT_SETTLE_PRICE;

//�ڼ������ñ�
typedef struct
{
	char sSpecDate[9];//����
	char sMarket[2];//�г�
	char sExchCode[2];//������
	char sDateType[2];//��������
}S_MDB_SPECDATE;


typedef struct
{
	int nContractNo;//��Ȩ��Լ���
	char sContractCode[21];//��Ȩ��Լ����
	char sSettleDate[9];//��������
	double dSettlePrice;//�����
	double dHvDes;//��ĺ�Լ�Ĳ�����
	double dPrice;//���ۼ۸�
	double dDelta;//delta
	double dGamma;
	double dVega;
	double dTheta;
	double dRho;
	double dBs;//bs����������
	double dGarch;
}S_MDB_OPTION_CONTRACT_PRICE_MODEL;

//��Լ���Ʒ������+��ĺ�Լ������
class Index_Contract_type_desno : public CIndex_Field_Base
{
public:
	char sProd_type;//Ʒ������
	unsigned  int    nProdId;//��ĺ�Լ���
	
	
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

//��Լ��
class CTbl_Contract
{
protected:
	
	CMemTableNew <S_MDB_CONTRACT_INFO>  m_table;
//	CPkeyCharF<21,1>               m_keyCode;//��Լ����
	CPkeyCharV<1>               m_keyCode;//��Լ����
	CPkeyUInt<1>                 m_keyNo;//��Լ���
//	CIndexCharF<2,1>            m_index_ex;//����-������
//	CIndexCharF<21,1>            m_index_destcode;//����-��ĺ�Լ����
//	CIndexCharF<2,1>            m_index_type;//����-Ʒ����� �ڻ� �ֻ� ��Ȩ ��Ʊ

	CIndexVarChar<1>            m_index_ex;//����-������
	CIndexVarChar<1>            m_index_destcode;//����-��ĺ�Լ����
	CIndexVarChar<1>            m_index_type;//����-Ʒ����� �ڻ� �ֻ� ��Ȩ ��Ʊ
	CIndexUInt<1>               m_index_destno;//����-��ĺ�Լ���
	
	CIndexField<Index_Contract_type_desno> m_index_type_destno;//���� Ʒ��+��ĺ�Լ���
	CBF_Mutex                   m_mutex;//������
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
	
	// ������: SelectByExch
	// ���  : ������ 2017-8-4 14:58:58
	// ����  : bool 
	// ����  : char *exch_code  NULL��Ϊ""��ʾȡ����
	// ����  : std::vector<S_MDB_CONTRACT_INFO *> &varlist
	// ����  : ȡ�ý�������������к�Լ
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

	// ������: SelectByDestno
	// ���  : ������ 2018-1-30 14:15:44
	// ����  : bool 
	// ����  : unsigned int destno ��ĺ�Լ���
	// ����  : std::vector<S_MDB_CONTRACT_INFO *> &varlist
	// ����  : ���ݱ�ĺ�Լ��Ų��Һ�Լ
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

	// ������: SelectByDestCode
	// ���  : ������ 2018-1-30 14:15:02
	// ����  : bool 
	// ����  : char *destcode ��ĺ�Լ����
	// ����  : std::vector<S_MDB_CONTRACT_INFO *> &varlist
	// ����  : ���ݱ�ĺ�Լ������Һ�Լ
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
	// ������: SelectByTypeDestno
	// ���  : ������ 2018-1-30 14:36:55
	// ����  : bool 
	// ����  : char *type ��Լ����
	// ����  : unsigned int destno ��ĺ�Լ
	// ����  : std::vector<S_MDB_CONTRACT_INFO *> &varlist
	// ����  : ���ݺ�Լ���ͼ���ĺ�Լ�Һ�Լ
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
	// ������: Delete
	// ���  : ������ 2019-5-16 10:48:53
	// ����  : int 
	// ����  : char *exchcode
	// ����  : ɾ���������ڼ�����������ı���
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

//��������Ϣ��
class CTbl_Exch_Info
{
protected:
	
	CMemTableNew <S_MDB_EXCH_INFO>   m_table;
//	CPkeyCharF<2,1>               m_pkey;//����������
//	CIndexCharF<2,1>            m_index_market;//����-�г�
	CPkeyCharV<1>               m_pkey;//����������
	CIndexVarChar<1>            m_index_market;//����-�г�
	CBF_Mutex                   m_mutex;//������
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
		if (onlychgdate == BOOL_TRUE) //������
		{
			if (strcmp(m_table.m_table[rid].sTradeDate,curdate) !=0)
			{
				return false;
			}
		}
		else
		{
			if (m_table.m_table[rid].sOnlyChgDate[0] == BOOL_TRUE) //ǰ������������
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
			m_table.m_table[rid].sClearStatus[0] = CLEAR_STATUS_DATEEND;//��״̬��Ϊ�������
		}
		else
		{
			m_table.m_table[rid].sClearStatus[0] = CLEAR_STATUS_CHG;//��״̬��Ϊ��������
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
//�г���Ϣ��
class CTbl_Market
{
protected:
	
	CMemTableNew <S_MDB_MARKET_INFO>   m_table;
	//CPkeyCharF<2,1>               m_pkey;//�г�����
	CPkeyCharV<1>               m_pkey;//�г�����
	CBF_Mutex                   m_mutex;//������
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
		if (onlychgdate == BOOL_TRUE) //������
		{
			if (strcmp(m_table.m_table[rid].sTradeDate,curdate) !=0)
			{
				return false;
			}
		}
		else
		{
			if (m_table.m_table[rid].sOnlyChgDate[0] == BOOL_TRUE) //ǰ������������
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
			m_table.m_table[rid].sClearStatus[0] = CLEAR_STATUS_DATEEND;//��״̬��Ϊ�������
		}
		else
		{
			m_table.m_table[rid].sClearStatus[0] = CLEAR_STATUS_CHG;//��״̬��Ϊ��������
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

//Ʒ�ֱ�
class CTbl_Variety
{
protected:
	
	CMemTableNew <S_MDB_VARIETY_INFO>  m_table;
// 	CPkeyCharF<21,1>               m_keyCode;//Ʒ�ִ���
// 	CIndexCharF<2,1>            m_index_ex;//����-������
// 	CIndexCharF<2,1>            m_index_type;//����-Ʒ����� �ڻ� �ֻ� ��Ȩ ��Ʊ
	CPkeyCharV<1>               m_keyCode;//Ʒ�ִ���
	CIndexVarChar<1>            m_index_ex;//����-������
	CIndexVarChar<1>            m_index_type;//����-Ʒ����� �ڻ� �ֻ� ��Ȩ ��Ʊ
	CPkeyUInt<1>                 m_keyNo;//Ʒ�ֱ��
	
	CBF_Mutex                   m_mutex;//������
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
	
	// ������: SelectByExch
	// ���  : ������ 2017-8-4 14:58:58
	// ����  : bool 
	// ����  : char *exch_code  NULL��Ϊ""��ʾȡ����
	// ����  : std::vector<S_MDB_VARIETY_INFO *> &varlist
	// ����  : ȡ�ý�������������к�Լ
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

//����Ա�ͻ���Ӧ��
class CTbl_Oper_TxAcc
{
protected:
	
	CMemTableNew <S_MDB_OPER_CUST_INFO>      m_table;
// 	CPkeyCharF<21,2>               m_pkey;//����Ա-�ͻ�
// 	CIndexCharF<17,1>            m_index_oper;//����Ա������
	CPkeyCharV<2>               m_pkey;//����Ա-�ͻ�
	CIndexVarChar<1>            m_index_oper;//����Ա������
	CBF_Mutex                   m_mutex;//������
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
	
	// ������: Select
	// ���  : ������ 2017-8-4 13:52:22
	// ����  : bool 
	// ����  : char *opercode
	// ����  : std::vector<S_MDB_OPER_CUST_INFO *> &oclist
	// ����  : ���ݲ���Ա��ѯ����Ա�����ͻ�
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

//��Ʒ��
class CTbl_Fina_Prod
{
protected:
	
	CMemTableNew <S_MDB_FINA_PROD_INFO>  m_table;
// 	CPkeyCharF<13,1>               m_key;//��Ʒ����
// 	CIndexCharF<13,1>            m_index_org;//����-��������
	CPkeyCharV<1>               m_key;//��Ʒ����
	CIndexVarChar<1>            m_index_org;//����-��������
	CBF_Mutex                   m_mutex;//������
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
	
	// ������: SelectByExch
	// ���  : ������ 2017-8-4 14:58:58
	// ����  : bool 
	// ����  : char *orgcode  
	// ����  : std::vector<S_MDB_FINA_PROD_INFO *> &varlist
	// ����  : ȡ�û�����������в�Ʒ
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

//��Ʒ��Ӧ�ʽ��˺ű�
class CTbl_Prod_Cust
{
protected:
	
	CMemTableNew <S_MDB_PROD_CUST>  m_table;
// 	CPkeyCharF<21,2>               m_key;//��Ʒ����+�ʽ��˺�
// 	CIndexCharF<13,1>            m_index;//��Ʒindex
	CPkeyCharV<2>               m_key;//��Ʒ����+�ʽ��˺�
	CIndexVarChar<1>            m_index;//��Ʒindex
	CBF_Mutex                   m_mutex;//������
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

//����Ա��Ӧ��Ʒ
class CTbl_Oper_Prod
{
protected:
	
	CMemTableNew <S_MDB_OPER_PROD>  m_table;
//	CPkeyCharF<17,2>               m_key;//����Ա+��Ʒ����
	CPkeyCharV<2>               m_key;//����Ա+��Ʒ����
	CBF_Mutex                   m_mutex;//������
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


//���ź�Լ�������Ʊ�����
class CKey_Cust_Contract_Limit : public CIndex_Field_Base
{
public:
	char sTxAcc[21];//�ʽ��˺�
	unsigned  int  nContractNo;//��Լ���
	
	
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
	CKeyField<CKey_Cust_Contract_Limit> m_key;//�ʽ��˺�+��Լ���
	CBF_Mutex                   m_mutex;//������
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

//���ı�
// class CTbl_Subscribe
// {
// protected:
// 	
// 	CMemTableNew <S_MDB_SUBSCRIBE>   m_table;
// 	CPkeyCharV<2>               m_pkey;//����Ա_��Ϣ����
// 	CBF_Mutex                   m_mutex;//������
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


//��Լ����۱� ֻ���浱ǰ���µĽ����
class CTbl_SettlePrice
{
protected:
	
	CMemTableNew <S_MDB_CONTRACT_SETTLE_PRICE>   m_table;
	CPkeyUInt<1>               m_pkey;//��Լ���
//	CPkeyCharF<21,1>               m_pkeycode;//��Լ����
	CPkeyCharV<1>               m_pkeycode;//��Լ����
	CBF_Mutex                   m_mutex;//������
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

//�������ñ�
class CTbl_SpecDate
{
protected:
	
	CMemTableNew <S_MDB_SPECDATE>   m_table;
// 	CPkeyCharF<9,3>               m_pkey;//�г�+������+����
// 	CIndexCharF<2,2>           m_index;//�г�+������
	CPkeyCharV<3>               m_pkey;//�г�+������+����
	CIndexVarChar<2>           m_index;//�г�+������
	CBF_Mutex                   m_mutex;//������
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

//��Ȩ�۸�ģ�ͱ�
class CTbl_Option_Model
{
protected:
	
	CMemTableNew <S_MDB_OPTION_CONTRACT_PRICE_MODEL>   m_table;
	CPkeyUInt<1>               m_pkey;//��Լ���
	CBF_Mutex                   m_mutex;//������
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
	CTbl_Variety    m_pVariety;//Ʒ�ֱ�
	CTbl_Contract   m_pContract;//��Լ��
	CTbl_Exch_Info  m_pExchange;//��������
	CTbl_Market     m_pMarket;//�г���
	CTbl_Fina_Prod  m_pProd;//��Ʒ��
	CTbl_Oper_TxAcc m_pOperTxAcc;//����Ա��Ӧ�ʽ��˺ű�

	CTbl_Prod_Cust  m_pProdCust;//��Ʒ��Ӧ�ʽ��˺ű�
	CTbl_Oper_Prod  m_pOperProd;//����Ա��Ӧ��Ʒ
	
	CTbl_Cust_Contract_Limit m_pCustContractLimit;//���ź�Լ�������Ʊ�

	CTbl_SettlePrice  m_pSettlePrice;//��Լ����۱�ֻ�������µ�
	CTbl_SpecDate     m_pSpecDate;//�ڼ�������
	CTbl_Option_Model m_pOptionModel;//��Ȩ��Լ�۸�ģ�ͱ�

};

#endif // !defined(AFX_DB_BASEINFO_H__48668C3F_4A61_47F6_9292_CE6DD88F582B__INCLUDED_)
