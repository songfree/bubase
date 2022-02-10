// Db_Custom.h: interface for the CDb_Custom class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DB_CUSTOM_H__F4B3ABE2_EC4D_4E00_AF69_0AA5BB64790F__INCLUDED_)
#define AFX_DB_CUSTOM_H__F4B3ABE2_EC4D_4E00_AF69_0AA5BB64790F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//���ݿ�ͻ�����  �ͻ���Ϣ���ʽ��˻���Ϣ


typedef struct 
{
	char sCustNo[13];//�ͻ����
	char sCustName[41];//�ͻ�����
	char sCustType[2];//�ͻ����
	char sCertType[2];//֤������
	char sCertNo[41];//֤������
	char sMobile[21];//�ֻ�
	char sChannleOpen[61];//��ͨ����
	char sPkiCertNo[41];//֤�����к�
	char sPkiCertDate[9];//֤����Ч��
	char sStatus[2];//�ͻ�״̬
	double dLastRisk;//���շ���
	double dCurRisk;//���շ���
	double dNetValue;//��λ��ֵ
	double dPosiTotal;//�ֲ��ܼ�ֵ
	double dMarginRate;//��֤��ռ��
	double dContractRate;//��Լ����ռ��
}S_MDB_CUSTINFO;
typedef struct 
{
	char sTxAcc[21];//�ʽ��˺�
	char sCustNo[13];//�ͻ����
	char sAcctype[2];//�˻�����
	char sCustSimpleName[41];//�ͻ����
	char sCustName[41];//�ͻ����
	char sSignOrg[13];//�������� ��������  ����
	char sMgrOrg[13];//�����������   �����ͻ���
	char sFutureFlag[2];//�ڻ�ҵ��ǩԼ��־
	char sAccLevel[3];//�˻�����
	char sExchFareModel[14];//����������ģ��
	char sMemberFareModel[14];//��Ա����ģ��
	unsigned int nSysId;//�������͹�˾
	char sCurrency[2];//����
	char sFsFeeModel[14];//fs������ģ��
	char sParentTxAcc[21];//���ʽ��˺�
}S_MDB_CUST_FUNDACC_INFO;

typedef struct 
{
	int  nFundRowid[20];//�ʽ��ROWID �±�Ϊ����
	char sTxAcc[21];//�ʽ��˺�
	int  nLockRowid;//�ڱ����е�rowid
	int  nCustRowid;//�ͻ���rowid
	int  nFundAccRowid;//�ʽ��˺ű�rowid
	CBF_Mutex  *mutex;//������
}S_MDB_TXACC_LOCK;

//�ͻ�
class  CTbl_Cust  
{
protected:
	
	CMemTableNew <S_MDB_CUSTINFO>   m_table;
//	CPkeyCharF<13,1>             m_pkey;//�ͻ���ŵ�key
	CPkeyCharV<1>             m_pkey;//�ͻ���ŵ�key
//	CPkeyCharV<2>             m_key_cert;//֤�����ͣ�֤�������KEY
//	CPkeyCharV<1>             m_key_phone;//�ֻ������key
	CBF_Mutex                 m_mutex;//������
public:
	CTbl_Cust()
	{
		
	}
	virtual ~CTbl_Cust()
	{
		
	}
	
	// ������: Insert
	// ���  : ������ 2017-8-3 10:21:03
	// ����  : int 
	// ����  : S_MDB_CUSTINFO cust
	// ����  : д��ͻ��� �ͻ���š��ֻ��š�֤������Ψһ����
	int Insert(S_MDB_CUSTINFO cust)  //ֻ���ڳ�ʼ��ʱʹ�ã����Կ��Բ�����
	{
		if (m_pkey.Find(cust.sCustNo))
		{
			return -1;
		}
// 		if (m_key_phone.Find(cust.sMobile))
// 		{
// 			return -1;
// 		}
// 		if (m_key_cert.Find(cust.sCertType,cust.sCertNo))
// 		{
// 			return -1;
// 		}
		int rid = m_table.Add(cust);
		m_pkey.Add(rid,m_table.m_table[rid].sCustNo);
//		m_key_phone.Add(rid,m_table.m_table[rid].sMobile);
//		m_key_cert.Add(rid,m_table.m_table[rid].sCertType,m_table.m_table[rid].sCertNo);
		return rid;
	}

	// ������: *SelectByNo
	// ���  : ������ 2017-8-3 10:21:58
	// ����  : S_CUST_INFO 
	// ����  : char *custno
	// ����  : ���ݿͻ���Ų�ѯ�ͻ���Ϣ
	S_MDB_CUSTINFO *SelectByNo(char *custno)
	{
		int rid;
		CBF_PMutex pp(&m_mutex);
		if (!m_pkey.GetRowId(rid,custno))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}
	int GetRowId(char *custno)
	{
		int rid;
		CBF_PMutex pp(&m_mutex);
		if (!m_pkey.GetRowId(rid,custno))
		{
			return -1;
		}
		return rid;
	}

	// ������: *SelectByPhone
	// ���  : ������ 2017-8-3 10:22:17
	// ����  : S_CUST_INFO 
	// ����  : char *phone
	// ����  : ���ݿͻ��ֻ��Ų�ѯ�ͻ���Ϣ
// 	S_MDB_CUSTINFO *SelectByPhone(char *phone)
// 	{
// 		int rid;
// 		CBF_PMutex pp(&m_mutex);
// 		if (!m_key_phone.GetRowId(rid,phone))
// 		{
// 			return NULL;
// 		}
// 		return &(m_table.m_table[rid]);
// 	}

	// ������: *SelectByCert
	// ���  : ������ 2017-8-3 10:22:41
	// ����  : S_CUST_INFO 
	// ����  : char *certtype
	// ����  : char *certno
	// ����  : ����֤�����͡�֤�������ѯ�ͻ���Ϣ
// 	S_MDB_CUSTINFO *SelectByCert(char *certtype,char *certno)
// 	{
// 		int rid;
// 		CBF_PMutex pp(&m_mutex);
// 		if (!m_key_cert.GetRowId(rid,certtype,certno))
// 		{
// 			return NULL;
// 		}
// 		return &(m_table.m_table[rid]);
// 	}
	S_MDB_CUSTINFO *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
};
//�ͻ��ʽ��˺�
class CTbl_Cust_TX_Acc
{
protected:
	
	CMemTableNew <S_MDB_CUST_FUNDACC_INFO>   m_table;
// 	CPkeyCharF<21,1>               m_pkey;//�ʽ��˺�
// 	CIndexCharF<13,1>            m_index_accorg;//������������
// 	CIndexCharF<13,1>            m_index_mgrorg;//�����������
// 	CIndexCharF<21,1>            m_index_parent;//���ʽ��˺�����
// 	CIndexCharF<13,1>            m_index_custno;//�ͻ�������
	CPkeyCharV<1>               m_pkey;//�ʽ��˺�
	CIndexVarChar<1>            m_index_accorg;//������������
	CIndexVarChar<1>            m_index_mgrorg;//�����������
	CIndexVarChar<1>            m_index_parent;//���ʽ��˺�����
	CIndexVarChar<1>            m_index_custno;//�ͻ�������
	CBF_Mutex                   m_mutex;//������
public:
	CTbl_Cust_TX_Acc()
	{
		
	}
	virtual ~CTbl_Cust_TX_Acc()
	{
		
	}
	void Clear()
	{
		m_pkey.Clear();//�ʽ��˺�
		m_index_accorg.Clear();//������������
		m_index_mgrorg.Clear();//�����������
		m_index_parent.Clear();//���ʽ��˺�����
		m_index_custno.Clear();//�ͻ�������
		m_table.Clear();
	}
	// ������: Insert
	// ���  : ������ 2017-8-3 10:31:29
	// ����  : bool 
	// ����  : S_MDB_CUST_FUNDACC_INFO custacc
	// ����  : д�뽻���˻��������˻����ͻ����/������/ϯλ��������/ϯλ/���ױ�������Ψһ���������ױ�������
	int Insert(S_MDB_CUST_FUNDACC_INFO custacc)  
	{
		CBF_PMutex pp(&m_mutex);
		if (m_pkey.Find(custacc.sTxAcc))
		{
			return -1;
		}
		int rid = m_table.Add(custacc);
		m_pkey.Add(rid,m_table.m_table[rid].sTxAcc);
		m_index_parent.Add(rid,m_table.m_table[rid].sParentTxAcc);
		m_index_accorg.Add(rid,m_table.m_table[rid].sSignOrg);
		m_index_mgrorg.Add(rid,m_table.m_table[rid].sMgrOrg);
		m_index_custno.Add(rid,m_table.m_table[rid].sCustNo);
		return rid;
	}
	int Update(S_MDB_CUST_FUNDACC_INFO custacc)  
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		if (m_pkey.GetRowId(rid,custacc.sTxAcc))
		{
			strcpy(m_table.m_table[rid].sFsFeeModel,custacc.sFsFeeModel);
			strcpy(m_table.m_table[rid].sExchFareModel,custacc.sExchFareModel);
			strcpy(m_table.m_table[rid].sMemberFareModel,custacc.sMemberFareModel);
			strcpy(m_table.m_table[rid].sFutureFlag,custacc.sFutureFlag);
			return rid;
		}
		rid = m_table.Add(custacc);
		m_pkey.Add(rid,m_table.m_table[rid].sTxAcc);
		m_index_parent.Add(rid,m_table.m_table[rid].sParentTxAcc);
		m_index_accorg.Add(rid,m_table.m_table[rid].sSignOrg);
		m_index_mgrorg.Add(rid,m_table.m_table[rid].sMgrOrg);
		m_index_custno.Add(rid,m_table.m_table[rid].sCustNo);
		return rid;
	}
	// ������: *SelectByAcc
	// ���  : ������ 2017-8-3 10:33:22
	// ����  : S_MDB_CUST_FUNDACC_INFO 
	// ����  : char *txacc
	// ����  : ���ݽ����˻���ѯ�����˻���Ϣ
	S_MDB_CUST_FUNDACC_INFO *SelectByAcc(char *txacc)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		if (!m_pkey.GetRowId(rid,txacc))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
		
	}
	int GetRowId(char *txacc)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		if (!m_pkey.GetRowId(rid,txacc))
		{
			return -1;
		}
		return rid;
	}
	bool SelectAll(std::vector<S_MDB_CUST_FUNDACC_INFO *> &acclist)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		bool bRet = m_pkey.First(rid);
		if (!bRet)
		{
			return false;
		}
		while (bRet)
		{
			acclist.push_back(&(m_table.m_table[rid]));
			bRet = m_pkey.Next(rid);
		}
		return true;

	}
	bool SelectAll(CInt &acclist)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		bool bRet = m_pkey.First(rid);
		if (!bRet)
		{
			return false;
		}
		while (bRet)
		{
			acclist.Add(rid);
			bRet = m_pkey.Next(rid);
		}
		return true;

	}
	int GetRowidByAcc(char *txacc)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		if (!m_pkey.GetRowId(rid,txacc))
		{
			return -1;
		}
		return rid;
	}
	// ������: SelectByAccOrg
	// ���  : ������ 2017-8-4 11:13:45
	// ����  : bool 
	// ����  : char *org
	// ����  : std::vector<S_MDB_CUST_FUNDACC_INFO *> &acclist
	// ����  : ����ǩԼ�����µ����пͻ�
	bool SelectByAccOrg(char *org,std::vector<S_MDB_CUST_FUNDACC_INFO *> &acclist)
	{
		CBF_PMutex pp(&m_mutex);
		std::vector<int> iset;
		if (!m_index_accorg.Select(iset,org))
		{
			return false;
		}
		for (int i=0; i< iset.size(); i++)
		{
			acclist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
	}
	bool SelectByAccOrg(char *org,CInt &acclist)
	{
		CBF_PMutex pp(&m_mutex);
		CInt iset;
		if (! m_index_accorg.Select(iset,org))
		{
			return false;
		}
		int rid;
		bool bret = iset.First(rid);
		while (bret)
		{
			acclist.Add(rid);
			bret = iset.Next(rid);
		}
		return true;

	}
	// ������: SelectByCust
	// ���  : ������ 2017-8-4 11:13:45
	// ����  : bool 
	// ����  : char *custno
	// ����  : std::vector<S_MDB_CUST_FUNDACC_INFO *> &acclist
	// ����  : ����ǩԼ�����µ����пͻ�
	bool SelectByCust(char *custno,std::vector<S_MDB_CUST_FUNDACC_INFO *> &acclist)
	{
		CBF_PMutex pp(&m_mutex);
		std::vector<int> iset;
		if (!m_index_custno.Select(iset,custno))
		{
			return false;
		}
		for (int i=0; i< iset.size(); i++)
		{
			acclist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
	}

	// ������: SelectByMgrOrg
	// ���  : ������ 2017-8-4 11:14:10
	// ����  : bool 
	// ����  : char *org
	// ����  : std::vector<S_MDB_CUST_FUNDACC_INFO *> &acclist
	// ����  : ���Ҵ�������µ����пͻ�
	bool SelectByMgrOrg(char *org,std::vector<S_MDB_CUST_FUNDACC_INFO *> &acclist)
	{
		CBF_PMutex pp(&m_mutex);
		std::vector<int> iset;
		if (!m_index_mgrorg.Select(iset,org))
		{
			return false;
		}
		for (int i=0; i< iset.size(); i++)
		{
			acclist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
	}
	bool SelectByMgrOrg(char *org,CInt &acclist)
	{
		CBF_PMutex pp(&m_mutex);
		CInt iset;
		if (!m_index_mgrorg.Select(iset,org))
		{
			return false;
		}
		int rid;
		bool bret = iset.First(rid);
		while (bret)
		{
			acclist.Add(rid);
			bret = iset.Next(rid);
		}
		return true;
	}
	S_MDB_CUST_FUNDACC_INFO *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
};


class CTbl_TX_Acc_Lock
{
protected:
	
	CMemTableNew <S_MDB_TXACC_LOCK>   m_table;
//	CPkeyCharF<21,1>               m_pkey;//�ʽ��˺�
	CPkeyCharV<1>               m_pkey;//�ʽ��˺�
	CBF_Mutex                   m_mutex;//������
public:
	CTbl_TX_Acc_Lock()
	{
		
	}
	virtual ~CTbl_TX_Acc_Lock()
	{
		int rid;
		bool bret = m_pkey.First(rid);
		while (bret)
		{
			delete m_table.m_table[rid].mutex;
			m_table.m_table[rid].mutex = NULL;
			bret = m_pkey.Next(rid);
		}
	}

	int Insert(S_MDB_TXACC_LOCK acclock)  
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		if (m_pkey.GetRowId(rid,acclock.sTxAcc))
		{
//			m_table.m_table[rid].nFundRowid = acclock.nFundRowid;
			return rid;
		}
		acclock.mutex = new CBF_Mutex();
		rid = m_table.Add(acclock);
		m_pkey.Add(rid,m_table.m_table[rid].sTxAcc);
		m_table.m_table[rid].nLockRowid=rid;
		return rid;
	}

	// ������: *SelectByAcc
	// ���  : ������ 2017-8-3 10:33:22
	// ����  : S_MDB_TXACC_LOCK 
	// ����  : char *txacc
	// ����  : ���ݽ����˻���ѯ�����˻���Ϣ
	S_MDB_TXACC_LOCK *SelectByAcc(char *txacc)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		if (!m_pkey.GetRowId(rid,txacc))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
		
	}

	S_MDB_TXACC_LOCK *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
};

class CDb_Custom  
{
public:
	CDb_Custom()
	{

	}
	virtual ~CDb_Custom()
	{

	}
	CTbl_Cust         m_pCustom;//�ͻ���Ϣ��
	CTbl_Cust_TX_Acc  m_pCustFundAcc;//�ʽ��˺���Ϣ��
	
};

#endif // !defined(AFX_DB_CUSTOM_H__F4B3ABE2_EC4D_4E00_AF69_0AA5BB64790F__INCLUDED_)
