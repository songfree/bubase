// Db_Set.h: interface for the CDb_Set class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DB_SET_H__4B14D6AB_CE59_4B41_A502_27B567589F24__INCLUDED_)
#define AFX_DB_SET_H__4B14D6AB_CE59_4B41_A502_27B567589F24__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "BF_Mutex.h"
//���ݿ� ���� 

//Ԥ������
typedef struct 
{
	unsigned short  nWarnLevel;//Ԥ������
	char sWarnName[41];//Ԥ������
	char sWarnColor[41];//Ԥ����ɫ
	char sWarnSound[41];//Ԥ������
	unsigned int  nWarnSep;//Ԥ����� ��
}S_MDB_WARN_LEVEL_INFO;

//���պ�Լ����
typedef struct 
{
	unsigned int nVarietyNo;//Ʒ�ֱ��
	unsigned int nContractNo;//��Լ���
	char   sContractCode[21];//��Լ����
	char   sWarnItem[2];//Ԥ����
	char   sCondition[2];//����
	double dValue;//Ԥ��ֵ
}S_MDB_RISKCONTRACT_SET;


//�ʽ�ģ�����ñ�
typedef struct 
{
	char   sFundTempletId[13];//ģID
	char   sFundTempletName[41];//ģ������
	unsigned short nWarnNo;//Ԥ�����
	char   sFundModel[2];//�ʽ�ָ��
	char   sCondition[2];//����
	double dValue;//Ԥ��ֵ
	char   sRight[2];//Ȩ��
}S_MDB_FUND_TEMPLET;
//�ֲ�ģ�����ñ�
typedef struct 
{
	char   sPosiTempletId[13];//ģID
	unsigned short nWarnNo;//Ԥ�����
	char   sExchCode[2];//����������
	char   sProdType[2];//Ʒ�����
	unsigned int nVarietyNo;//Ʒ�ֱ��
	char    sVarietyCode[21];//Ʒ�ִ���
	unsigned int nOption_desno;//��Ȩ��ĺ�Լ���
	char   sOption_destcode[21];//��Ȩ��ĺ�Լ����
	unsigned int nContractNo;//��Լ���
	char   sContractCode[21];//��Լ����
	char   sPosiModel[2];//�ֲ�ָ��
	char   sCondition[2];//����
	double dValue;//Ԥ��ֵ
	char   sRight[2];//Ȩ��
}S_MDB_POSITION_TEMPLET;

//�ʲ������ʽ�ֲּ�ر�ṹ   �ʽ��ر�ͳֲּ�ر�ṹ��ͬ�����ݲ�ͬ
typedef struct  
{
	char sTempletId[13];//ģ��ID
	unsigned short nWarnNo;//Ԥ�����
	char sModel[2];  //�ʲ�ָ��
	char sCondition[2];//����
	double dValue;//��ֵ
	char sRight[2];//����Ȩ��
}S_MDB_PROD_MONITOR;

//�ʽ��˺�ģ���Ӧ��
typedef struct 
{
	char   sTxAcc[21];//�ʽ��˺�
	char   sFundTempletId[13];//�ʽ�ģ��ID
	char   sPosiTempletId[13];//�ֲ�ģ��ID
}S_MDB_CUST_TEMPLET_RELA;

//��Ʒ������Ӧģ���
typedef struct 
{
	char   sProdOrg[13];//��Ʒ����
	char   sFundTempletId[13];//�ʽ�ģ��ID
	char   sPosiTempletId[13];//�ֲ�ģ��ID
}S_MDB_PRODORG_TEMPLET_RELA;
//ģ���
typedef struct 
{
	char   sTempletId[13];//ģ��ID
	char   sTempletName[41];//ģ������
	char   sOrgCode[13];//��������
	char   sSubCanUse[2];//�Ƿ��¼�����
}S_MDB_TEMPLET_INFO;


typedef struct 
{
	char sFareModeCode[14];//ģ�����
	char sFareModeName[41];//ģ������
	char sFareType[2];//ģ������
	char sOrgCode[13];//��������
}S_MDB_FEE_TEMPLET;

typedef struct 
{
	char sFareModeCode[14];//ģ�����
	char sProdType[2];//Ʒ������
	unsigned int nVarietyNo;//Ʒ�ֺ�
	char sVarietyCode[21];//Ʒ�ִ���
	unsigned int nContractNo;//��Լ��
	char sContractCode[21];//��Լ����
	char sFareNo[4];//���ñ��
	char sFareMode[2];//�շ�ģʽ
	double dValue;//����ֵ
}S_MDB_FEE_TEMPLET_DETAIL;

typedef struct 
{
	char sTxAcc[21];
	char sProdType[2];//Ʒ������
	unsigned int nVarietyNo;//Ʒ�ֺ�
	char sVarietyCode[21];//Ʒ�ִ���
	unsigned int nContractNo;//��Լ��
	char sContractCode[21];//��Լ����
	char sFareNo[4];//���ñ��
	char sFareMode[2];//�շ�ģʽ
	double dValue;//����ֵ
	char sInclude[2];//�Ƿ����������
}S_MDB_CUST_FEE;


typedef struct 
{
	unsigned int nSysCode;//ϵͳ����
	char  sSysName[41];//ϵͳ����
	unsigned int  nSvrId;//ϵͳע�ᵽ���ߵĹ��������
	char  sOrgCode[17];//��Ӧ�Ļ�������
	char  sClearDate[9];//��������
	unsigned int   nClearFlag; //�����־ 0���ݵ���  1����
	//0 �����㣬�����̨����
	//1 ����       ֱ����������
	//	2 ������   ������̨�Ķ��PG
	//	3 ������       ֱ����������
}S_MDB_BROKERAGE_INFO; //��������
typedef struct 
{
	unsigned int nSysCode;//ϵͳ����
	char  sExchCode[2];//����������
	char  sSeatNo[7];//ϯλ����
	unsigned int   nSvrId; //��������ע��ķ����
}S_MDB_BROKERAGE_EXCHANGE; //���ͽ���������
typedef struct 
{
	unsigned int nSysCode;//ϵͳ����
	char  sBrokerId[11];//pg��Ӧ��brokerid
	unsigned int   nSvrId; //��������ע��ķ����
}S_MDB_BROKERAGE_PG; //����PG����
typedef struct  
{
	char sCurrency[2]; //����
	double dRate;     //������ҵı���
}S_MDB_CURRENCY_RATE;

class  CTbl_WarnLevel
{
protected:
	
	CMemTableNew <S_MDB_WARN_LEVEL_INFO> m_table;
	CPkeyUInt<1>          m_pkey;               //Ԥ������
	CBF_Mutex                   m_mutex;//������
public:
	CTbl_WarnLevel()
	{
		
	}
	virtual ~CTbl_WarnLevel()
	{
		
	}
	void Clear()
	{
		m_pkey.Clear();
		m_table.Clear();
	}
	int Insert(S_MDB_WARN_LEVEL_INFO info) //ֻ���ڳ�ʼ��ʱʹ�ã����Կ��Բ�����
	{
		CBF_PMutex pp(&m_mutex);
		if (m_pkey.Find((unsigned int)info.nWarnLevel))
		{
			return -1;
		}
		int rid=m_table.Add(info);
		m_pkey.Add(rid,(unsigned int)m_table.m_table[rid].nWarnLevel);
		return rid;
	}
	
	// ������: *Select
	// ���  : ������ 2017-8-3 10:20:35
	// ����  : S_MDB_WARN_LEVEL_INFO 
	// ����  : unsigned int warnlevel
	// ����  : ����Ԥ��������Ҽ�������
	S_MDB_WARN_LEVEL_INFO *Select(unsigned int warnlevel)
	{
		int rid;
		CBF_PMutex pp(&m_mutex);
		if (!m_pkey.Select(rid,warnlevel))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}
	S_MDB_WARN_LEVEL_INFO *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
};

//���պ�Լ��������
class CKey_RiskContractSet : public CIndex_Field_Base
{
public:
	unsigned int nVarietyNo;//Ʒ�ֱ��
	unsigned int nContractNo;//��Լ���
	char   sWarnItem[2];//Ԥ����
	
	
	virtual bool operator<(const CKey_RiskContractSet& field) const
	{
		int ret;
		if (nVarietyNo < field.nVarietyNo)
		{
			return true;
		}
		else if (nVarietyNo == field.nVarietyNo)
		{
			if  (nContractNo < field.nContractNo)
			{
				return true;
			}
			else if  (nContractNo == field.nContractNo)
			{
				if (sWarnItem[0]<field.sWarnItem[0])
				{
					return true;
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

class  CTbl_RiskContract_Set
{
protected:
	
	CMemTableNew <S_MDB_RISKCONTRACT_SET> m_table;
	CKeyField<CKey_RiskContractSet>    m_key;
	CIndexUInt<2>                      m_index;             
	CBF_Mutex                   m_mutex;//������
public:
	CTbl_RiskContract_Set()
	{
		
	}
	virtual ~CTbl_RiskContract_Set()
	{
		
	}
	void Clear()
	{
		m_key.Clear();
		m_table.Clear();
		m_index.Clear();
	}
	int Insert(S_MDB_RISKCONTRACT_SET info) //ֻ���ڳ�ʼ��ʱʹ�ã����Կ��Բ�����
	{
		CBF_PMutex pp(&m_mutex);
		CKey_RiskContractSet aa;
		aa.nContractNo = info.nContractNo;
		aa.nVarietyNo = info.nVarietyNo;
		strcpy(aa.sWarnItem,info.sWarnItem);
		if (m_key.Find(aa))
		{
			return -1;
		}
		int rid=m_table.Add(info);
		aa.m_nRowId = rid;
		m_key.Add(aa);
		m_index.Add(rid,m_table.m_table[rid].nVarietyNo,m_table.m_table[rid].nContractNo);
		return rid;
	}
	S_MDB_RISKCONTRACT_SET *Select(unsigned int nVarietyNo,unsigned int nContractNo,char *sWarnItem)
	{
		CBF_PMutex pp(&m_mutex);
		CKey_RiskContractSet aa;
		aa.nContractNo = nContractNo;
		aa.nVarietyNo = nVarietyNo;
		strcpy(aa.sWarnItem,sWarnItem);
		if (!m_key.Find(aa))
		{
			return NULL;
		}
		return &(m_table.m_table[aa.m_nRowId]);
	}
	bool Select(unsigned int nVarietyNo,unsigned int nContractNo,std::vector<S_MDB_RISKCONTRACT_SET *> &reslist)
	{
		CBF_PMutex pp(&m_mutex);
		std::vector<int> iset;
		if (!m_index.Select(iset,nVarietyNo,nContractNo))
		{
			return false;
		}
		for (int i=0; i<iset.size(); i++)
		{
			reslist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
	}
	S_MDB_RISKCONTRACT_SET *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
};


//�ͻ��ʽ�ģ�����ñ�����
class CKey_Fund_Templet : public CIndex_Field_Base
{
public:
	char   sFundTempletId[13];//ģID
	unsigned short nWarnNo;//Ԥ�����
	char   sFundModel[2];//�ʽ�ָ��
	
	
	virtual bool operator<(const CKey_Fund_Templet& field) const
	{
		int ret=strcmp(sFundTempletId,field.sFundTempletId);
		if (ret<0)
		{
			return true;
		}
		else if (ret == 0)
		{
			if  (nWarnNo < field.nWarnNo)
			{
				return true;
			}
			else if  (nWarnNo == field.nWarnNo)
			{
				if (sFundModel[0]<field.sFundModel[0])
				{
					return true;
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
//�ͻ��ʽ�ģ������order by ����
class CIndex_FundTemplet_NoModel: public CIndex_Field_Base
{
public:
	unsigned short nWarnNo;//Ԥ�����
	char   sFundModel[2];//�ʽ�ָ��
	virtual bool operator<(const CIndex_FundTemplet_NoModel& field) const
	{
		if  (nWarnNo < field.nWarnNo)
		{
			return true;
		}
		else if  (nWarnNo == field.nWarnNo)
		{
			if (sFundModel[0] < field.sFundModel[0])
			{
				return true;
			}
		}
		return false;
	}
};
//�ͻ��ʽ�ģ�����ñ�
class  CTbl_Fund_Templet
{
protected:
	
	CMemTableNew <S_MDB_FUND_TEMPLET> m_table;
	CKeyField<CKey_Fund_Templet>    m_key;   //���� ģ��ID,Ԥ����ţ��ʽ�ָ��
//	CIndexCharF<13,1>                m_index;//ģ��ID������
	CIndexVarChar<1>                m_index;//ģ��ID������
	CBF_Mutex                   m_mutex;//������
public:
	CTbl_Fund_Templet()
	{
		
	}
	virtual ~CTbl_Fund_Templet()
	{
		
	}
	void Clear()
	{
		m_key.Clear();
		m_table.Clear();
		m_index.Clear();
	}
	int Insert(S_MDB_FUND_TEMPLET info) //ֻ���ڳ�ʼ��ʱʹ�ã����Կ��Բ�����
	{
		CBF_PMutex pp(&m_mutex);
		CKey_Fund_Templet aa;
		strcpy(aa.sFundTempletId,info.sFundTempletId);
		aa.nWarnNo = info.nWarnNo;
		strcpy(aa.sFundModel,info.sFundModel);
		if (m_key.Find(aa))
		{
			return -1;
		}
		int rid=m_table.Add(info);
		aa.m_nRowId = rid;
		m_key.Add(aa);
		m_index.Add(rid,m_table.m_table[rid].sFundTempletId);
		return rid;
	}
	S_MDB_FUND_TEMPLET *Select(const char *sFundTempletId,unsigned short nWarnNo,const char *sFundModel)
	{
		CBF_PMutex pp(&m_mutex);
		CKey_Fund_Templet aa;
		strcpy(aa.sFundTempletId,sFundTempletId);
		aa.nWarnNo = nWarnNo;
		strcpy(aa.sFundModel,sFundModel);
		if (!m_key.Find(aa))
		{
			return NULL;
		}
		return &(m_table.m_table[aa.m_nRowId]);
	}
	//ȡ���ʽ�ģ������ã���Ԥ�����+�ʽ�ָ������
	bool Select(const char *sFundTempletId,std::vector<S_MDB_FUND_TEMPLET *> &reslist)
	{
		CIndexField<CIndex_FundTemplet_NoModel> index;
		CIndex_FundTemplet_NoModel aa;
		std::vector<int> iset;
		if (!m_index.Select(iset,(char *)sFundTempletId))
		{
			return false;
		}
		for (int i=0; i<iset.size(); i++)
		{
			aa.m_nRowId = iset[i];
			strcpy(aa.sFundModel,m_table.m_table[iset[i]].sFundModel);
			aa.nWarnNo = m_table.m_table[iset[i]].nWarnNo;
			index.Add(aa);
		}
		bool bret = index.First(aa);
		while (bret)
		{
			reslist.push_back(&(m_table.m_table[aa.m_nRowId]));
			bret = index.Next(aa);
		}
		return true;
	}
	S_MDB_FUND_TEMPLET *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
};

//�ͻ��ֲ�ģ�����ñ�����
class CKey_Position_Templet : public CIndex_Field_Base
{
public:
	char   sPosiTempletId[13];//ģID
	unsigned short nWarnNo;//Ԥ�����
	char   sProdType[2];//Ʒ�����
	unsigned int nVarietyNo;//Ʒ�ֱ��
	unsigned int nOption_desno;//��Ȩ��ĺ�Լ���
	unsigned int nContractNo;//��Լ���
	char   sPosiModel[2];//�ֲ�ָ��
	
	
	virtual bool operator<(const CKey_Position_Templet& field) const
	{
		int ret=strcmp(sPosiTempletId,field.sPosiTempletId);
		if (ret<0)
		{
			return true;
		}
		else if (ret == 0)
		{
			if  (nWarnNo < field.nWarnNo)
			{
				return true;
			}
			else if  (nWarnNo == field.nWarnNo)
			{
				if (sProdType[0] < field.sProdType[0])
				{
					return true;
				}
				else if (sProdType[0] == field.sProdType[0])
				{
					if (nVarietyNo< field.nVarietyNo)
					{
						return true;
					}
					else if (nVarietyNo == field.nVarietyNo)
					{
						if (nOption_desno<field.nOption_desno)
						{
							return true;
						}
						else if (nOption_desno == field.nOption_desno)
						{
							if (nContractNo <field.nContractNo)
							{
								return true;
							}
							else if (nContractNo == field.nContractNo)
							{
								if (sPosiModel[0] < field.sPosiModel[0])
								{
									return true;
								}
							}
						}
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
//�ֲ�ģ�����������ݴ����������Լ�����ã���Ԥ����������Ƚϵõ��ֲֵķ���
class CIndex_Position_Templet : public CIndex_Field_Base
{
public:
	char   sPosiTempletId[13];//ģ��ID
	char   sProdType[2];//Ʒ�����
	unsigned int nVarietyNo;//Ʒ�ֱ��
	unsigned int nOption_desno;//��Ȩ��ĺ�Լ���
	unsigned int nContractNo;//��Լ���
	
	virtual bool operator<(const CIndex_Position_Templet& field) const
	{
		int ret=strcmp(sPosiTempletId,field.sPosiTempletId);
		if (ret<0)
		{
			return true;
		}
		else if (ret == 0)
		{
			if (sProdType[0] < field.sProdType[0])
			{
				return true;
			}
			else if (sProdType[0] == field.sProdType[0])
			{
				if (nVarietyNo< field.nVarietyNo)
				{
					return true;
				}
				else if (nVarietyNo == field.nVarietyNo)
				{
					if (nOption_desno<field.nOption_desno)
					{
						return true;
					}
					else if (nOption_desno == field.nOption_desno)
					{
						if (nContractNo <field.nContractNo)
						{
							return true;
						}
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
//�ͻ��ֲ�ģ������order by ����
class CIndex_PosiTemplet_NoModel: public CIndex_Field_Base
{
public:
	unsigned short nWarnNo;//Ԥ�����
	char   sPosiModel[2];//�ֲ�ָ��
	virtual bool operator<(const CIndex_PosiTemplet_NoModel& field) const
	{
		if  (nWarnNo < field.nWarnNo)
		{
			return true;
		}
		else if  (nWarnNo == field.nWarnNo)
		{
			if (sPosiModel[0] < field.sPosiModel[0] )
			{
				return true;
			}
		}
		return false;
	}
};
//�ͻ��ֲ�ģ�����ñ�
class  CTbl_Position_Templet
{
protected:
	
	CMemTableNew <S_MDB_POSITION_TEMPLET> m_table;
	CKeyField<CKey_Position_Templet>    m_key;   //���� ģ��ID+Ԥ�����+Ʒ�����+Ʒ�ֱ��+��ĺ�Լ+��Լ+�ʽ�ָ��
	CIndexField<CIndex_Position_Templet> m_index;//ģ��ID+Ʒ�����+Ʒ�ֱ��+��ĺ�Լ+��Լ������
	CBF_Mutex                   m_mutex;//������
public:
	CTbl_Position_Templet()
	{
		
	}
	virtual ~CTbl_Position_Templet()
	{
		
	}
	void Clear()
	{
		m_key.Clear();
		m_table.Clear();
		m_index.Clear();
	}
	int Insert(S_MDB_POSITION_TEMPLET info) 
	{
		CBF_PMutex pp(&m_mutex);
		CKey_Position_Templet aa;
		CIndex_Position_Templet bb;
		strcpy(aa.sPosiTempletId,info.sPosiTempletId);
		aa.nWarnNo = info.nWarnNo;
		strcpy(aa.sProdType,info.sProdType);
		aa.nVarietyNo = info.nVarietyNo;
		aa.nOption_desno = info.nOption_desno;
		aa.nContractNo = info.nContractNo;
		strcpy(aa.sPosiModel,info.sPosiModel);
		if (m_key.Find(aa))
		{
			return -1;
		}
		strcpy(bb.sPosiTempletId,info.sPosiTempletId);
		strcpy(bb.sProdType,info.sProdType);
		bb.nVarietyNo = info.nVarietyNo;
		bb.nOption_desno = info.nOption_desno;
		bb.nContractNo = info.nContractNo;
		
		int rid=m_table.Add(info);
		aa.m_nRowId = rid;
		bb.m_nRowId = rid;
		m_key.Add(aa);
		m_index.Add(bb);
		return rid;
	}
	S_MDB_POSITION_TEMPLET *Select(const char *sPosiTempletId,unsigned short nWarnNo,const char *sProdType,unsigned int nVarietyNo,unsigned int nOption_desno,unsigned int nContractNo,const char *sPosiModel)
	{
		CBF_PMutex pp(&m_mutex);
		CKey_Position_Templet aa;
		strcpy(aa.sPosiTempletId,sPosiTempletId);
		aa.nWarnNo = nWarnNo;
		strcpy(aa.sProdType,sProdType);
		aa.nVarietyNo = nVarietyNo;
		aa.nOption_desno = nOption_desno;
		aa.nContractNo = nContractNo;
		strcpy(aa.sPosiModel,sPosiModel);
		if (!m_key.Find(aa))
		{
			return NULL;
		}
		return &(m_table.m_table[aa.m_nRowId]);
	}
	//ȡ���ʽ�ģ������ã���Ԥ�����+�ʽ�ָ������
	bool Select(const char *sPosiTempletId,const char *sProdType,unsigned int nVarietyNo,unsigned int nOption_desno,unsigned int nContractNo,std::vector<S_MDB_POSITION_TEMPLET *> &reslist)
	{
		CIndexField<CIndex_PosiTemplet_NoModel> index;
		CIndex_Position_Templet bb;
		

		strcpy(bb.sPosiTempletId,sPosiTempletId);
		strcpy(bb.sProdType,sProdType);
		bb.nVarietyNo = nVarietyNo;
		bb.nOption_desno = nOption_desno;
		bb.nContractNo = nContractNo;

		std::vector<int> iset;
		bool bret = m_index.Find(bb);
		if (!bret)
		{
			return false;
		}
		CIndex_PosiTemplet_NoModel aa;
		while (bret)
		{
			aa.m_nRowId = bb.m_nRowId;
			strcpy(aa.sPosiModel,m_table.m_table[bb.m_nRowId].sPosiModel);
			aa.nWarnNo = m_table.m_table[bb.m_nRowId].nWarnNo;
			index.Add(aa);
			bret = m_index.Next(bb);
		}
		bret = index.First(aa);
		while (bret)
		{
			reslist.push_back(&(m_table.m_table[aa.m_nRowId]));
			bret = index.Next(aa);
		}
		return true;
	}
	S_MDB_POSITION_TEMPLET *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
};

//�ʲ���ر�����
class CKey_Prod_Monitor : public CIndex_Field_Base
{
public:
	char sTempletId[13];//ģ��ID
	unsigned short nWarnNo;//Ԥ�����
	char sModel[2];  //�ʲ�ָ��
	
	
	virtual bool operator<(const CKey_Prod_Monitor& field) const
	{
		int ret=strcmp(sTempletId,field.sTempletId);
		if (ret<0)
		{
			return true;
		}
		else if (ret == 0)
		{
			if  (nWarnNo < field.nWarnNo)
			{
				return true;
			}
			else if  (nWarnNo == field.nWarnNo)
			{
				if (sModel[0] < field.sModel[0])
				{
					return true;
				}
			}
		}
		
		return false;
	}
};


//������order  by  Ԥ�����+�ʲ�ָ��
class CIndex_Prod_Monitor : public CIndex_Field_Base
{
public:
	unsigned short nWarnNo;//Ԥ�����
	char sModel[2];  //�ʲ�ָ��


	virtual bool operator<(const CIndex_Prod_Monitor& field) const
	{
		if  (nWarnNo < field.nWarnNo)
		{
			return true;
		}
		else if  (nWarnNo == field.nWarnNo)
		{
			if (sModel[0] < field.sModel[0])
			{
				return true;
			}
		}
		return false;
	}
};
//��Ʒ�����ʽ��ֲּ�����ñ�
class  CTbl_Prod_Monitor
{
protected:
	
	CMemTableNew <S_MDB_PROD_MONITOR> m_table;
	CKeyField<CKey_Prod_Monitor>          m_key; 
	CIndexVarChar<1>                     m_index;//ģ��ID����
	CBF_Mutex                   m_mutex;//������
public:
	CTbl_Prod_Monitor()
	{
		
	}
	virtual ~CTbl_Prod_Monitor()
	{
		
	}
	void Clear()
	{
		m_key.Clear();
		m_table.Clear();
	}
	int Insert(S_MDB_PROD_MONITOR info) 
	{
		CBF_PMutex pp(&m_mutex);
		CKey_Prod_Monitor aa;
		strcpy(aa.sTempletId,info.sTempletId);
		aa.nWarnNo = info.nWarnNo;
		strcpy(aa.sModel,info.sModel);
		if (m_key.Find(aa))
		{
			return -1;
		}
		int rid=m_table.Add(info);
		aa.m_nRowId =rid;
		m_key.Add(aa);
		m_index.Add(rid,m_table.m_table[rid].sTempletId);
		return rid;
	}
	//bool Select(std::vector<S_MDB_PROD_MONITOR *> &reslist)
	//{
	//	CKey_Prod_Monitor aa;
	//	CBF_PMutex pp(&m_mutex);
	//	bool bret = m_key.First(aa);
	//	if (!bret)
	//	{
	//		return false;
	//	}
	//	while (bret)
	//	{
	//		reslist.push_back(&(m_table.m_table[aa.m_nRowId]));
	//		bret = m_key.Next(aa);
	//	}
	//	return true;
	//}
	bool Select(char *templetid,std::vector<S_MDB_PROD_MONITOR *> &reslist)
	{
		CInt iset;
		if (!m_index.Select(iset,templetid))
		{
			return false;
		}
		CKeyField<CIndex_Prod_Monitor>   index;
		CIndex_Prod_Monitor bb;
		int rid;
		bool bRet = iset.First(rid);
		while (bRet)
		{
			bb.m_nRowId =rid;
			bb.nWarnNo = m_table.m_table[rid].nWarnNo;
			strcpy(bb.sModel,m_table.m_table[rid].sModel);
			index.Add(bb);
			bRet = iset.Next(rid);
		}
		bRet = index.First(bb);
		while(bRet)
		{
			reslist.push_back(&(m_table.m_table[bb.m_nRowId]));
			bRet = index.Next(bb);
		}
		return true;
	}
	S_MDB_PROD_MONITOR *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
};

//ģ���
class  CTbl_Templet
{
protected:
	
	CMemTableNew <S_MDB_TEMPLET_INFO> m_table;
//	CPkeyCharF<13,1>                m_key;
	CPkeyCharV<1>                m_key;
	CBF_Mutex                   m_mutex;//������
public:
	CTbl_Templet()
	{
		
	}
	virtual ~CTbl_Templet()
	{
		
	}
	void Clear()
	{
		m_key.Clear();
		m_table.Clear();
	}
	int Insert(S_MDB_TEMPLET_INFO info) //ֻ���ڳ�ʼ��ʱʹ�ã����Կ��Բ�����
	{
		CBF_PMutex pp(&m_mutex);
		if (m_key.Find(info.sTempletId))
		{
			return -1;
		}
		int rid=m_table.Add(info);
		m_key.Add(rid,m_table.m_table[rid].sTempletId);
		return rid;
	}

	S_MDB_TEMPLET_INFO *Select(const char *templetid)
	{
		int rid;
		CBF_PMutex pp(&m_mutex);
		if (!m_key.Select(rid,(char *)templetid))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}
	S_MDB_TEMPLET_INFO *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
};

//�ʽ��˺�ģ���Ӧ��ϵ��
class  CTbl_Cust_Templet
{
protected:
	
	CMemTableNew <S_MDB_CUST_TEMPLET_RELA> m_table;
//	CPkeyCharF<21,1>                m_key;
	CPkeyCharV<1>                m_key;
	CBF_Mutex                   m_mutex;//������
public:
	CTbl_Cust_Templet()
	{
		
	}
	virtual ~CTbl_Cust_Templet()
	{
		
	}
	void Clear()
	{
		m_key.Clear();
		m_table.Clear();
	}
	int Insert(S_MDB_CUST_TEMPLET_RELA info) //ֻ���ڳ�ʼ��ʱʹ�ã����Կ��Բ�����
	{
		CBF_PMutex pp(&m_mutex);
		if (m_key.Find(info.sTxAcc))
		{
			return -1;
		}
		int rid=m_table.Add(info);
		m_key.Add(rid,m_table.m_table[rid].sTxAcc);
		return rid;
	}
	
	S_MDB_CUST_TEMPLET_RELA *Select(const char *txacc)
	{
		int rid;
		CBF_PMutex pp(&m_mutex);
		if (!m_key.Select(rid,(char *)txacc))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}
	S_MDB_CUST_TEMPLET_RELA *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
};

//��Ʒ����ģ���Ӧ��ϵ
class  CTblPordOrg_Templet
{
protected:

	CMemTableNew <S_MDB_PRODORG_TEMPLET_RELA> m_table;
	CPkeyCharV<1>                m_key;
	CBF_Mutex                   m_mutex;//������
public:
	CTblPordOrg_Templet()
	{

	}
	virtual ~CTblPordOrg_Templet()
	{

	}
	void Clear()
	{
		m_key.Clear();
		m_table.Clear();
	}
	int Insert(S_MDB_PRODORG_TEMPLET_RELA info) //ֻ���ڳ�ʼ��ʱʹ�ã����Կ��Բ�����
	{
		CBF_PMutex pp(&m_mutex);
		if (m_key.Find(info.sProdOrg))
		{
			return -1;
		}
		int rid=m_table.Add(info);
		m_key.Add(rid,m_table.m_table[rid].sProdOrg);
		return rid;
	}

	S_MDB_PRODORG_TEMPLET_RELA *Select(const char *prodorg)
	{
		int rid;
		CBF_PMutex pp(&m_mutex);
		if (!m_key.Select(rid,(char *)prodorg))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}
	S_MDB_PRODORG_TEMPLET_RELA *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
};

//����ģ����Ϣ��
class  CTbl_Fee_Templet_Info
{
protected:
	
	CMemTableNew <S_MDB_FEE_TEMPLET> m_table;
// 	CPkeyCharF<14,1>               m_key;//ģ��ID����
// 	CIndexCharF<13,1>            m_indexOrg;//��������
	CPkeyCharV<1>               m_key;//ģ��ID����
	CIndexVarChar<1>            m_indexOrg;//��������
	CBF_Mutex                   m_mutex;//������
public:
	CTbl_Fee_Templet_Info()
	{
		
	}
	virtual ~CTbl_Fee_Templet_Info()
	{
		
	}
	void Clear()
	{
		m_key.Clear();
		m_table.Clear();
		m_indexOrg.Clear();
	}
	int Insert(S_MDB_FEE_TEMPLET info) 
	{
		CBF_PMutex pp(&m_mutex);
		if (m_key.Find(info.sFareModeCode))
		{
			return -1;
		}
		int rid=m_table.Add(info);
		m_key.Add(rid,m_table.m_table[rid].sFareModeCode);
		m_indexOrg.Add(rid,m_table.m_table[rid].sOrgCode);
		return rid;
	}
	S_MDB_FEE_TEMPLET *Select(const char *feeid)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		if (!m_key.Select(rid,(char *)feeid))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}
	bool Select(const char *orgcode,std::vector<S_MDB_FEE_TEMPLET *>reslist)
	{
		CBF_PMutex pp(&m_mutex);
		std::vector<int>iset;
		if (!m_indexOrg.Select(iset,(char *)orgcode))
		{
			return false;
		}
		for (int i=0; i<iset.size(); i++)
		{
			reslist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
	}
	S_MDB_FEE_TEMPLET *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
};



//����ģ�����ñ�����
class CKey_Fee_Templet_Detail : public CIndex_Field_Base
{
public:
	char sFareModeCode[14];//ģ�����
	char sProdType[2];//Ʒ������
	unsigned int nVarietyNo;//Ʒ�ֺ�
	unsigned int nContractNo;//��Լ��
	char sFareNo[4];//���ñ��
	char sFareMode[2];//�շ�ģʽ
	
	
	virtual bool operator<(const CKey_Fee_Templet_Detail& field) const
	{
		int ret=strcmp(sFareModeCode,field.sFareModeCode);
		if (ret<0)
		{
			return true;
		}
		else if (ret == 0)
		{
			if  (sProdType[0] < field.sProdType[0])
			{
				return true;
			}
			else if  (sProdType[0] == field.sProdType[0])
			{
				if (nVarietyNo< field.nVarietyNo)
				{
					return true;
				}
				else if (nVarietyNo == field.nVarietyNo)
				{
					if (nContractNo <field.nContractNo)
					{
						return true;
					}
					else if (nContractNo == field.nContractNo)
					{
						ret = strcmp(sFareNo,field.sFareNo);
						if (ret <0)
						{
							return true;
						}
						else if (ret == 0)
						{
							if (sFareMode[0] < field.sFareMode[0])
							{
								return true;
							}
						}
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

class CIndex_Fee_Templet_Detail : public CIndex_Field_Base
{
public:
	char sFareModeCode[14];//ģ�����
	char sProdType[2];//Ʒ������
	unsigned int nVarietyNo;//Ʒ�ֺ�
	unsigned int nContractNo;//��Լ��
	char sFareNo[4];//���ñ��
	
	virtual bool operator<(const CIndex_Fee_Templet_Detail& field) const
	{
		int ret=strcmp(sFareModeCode,field.sFareModeCode);
		if (ret<0)
		{
			return true;
		}
		else if (ret == 0)
		{
			if  (sProdType[0] < field.sProdType[0])
			{
				return true;
			}
			else if  (sProdType[0] == field.sProdType[0])
			{
				if (nVarietyNo< field.nVarietyNo)
				{
					return true;
				}
				else if (nVarietyNo == field.nVarietyNo)
				{
					if (nContractNo <field.nContractNo)
					{
						return true;
					}
					else if (nContractNo == field.nContractNo)
					{
						ret = strcmp(sFareNo,field.sFareNo);
						if (ret <0)
						{
							return true;
						}
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

//����ģ�����ñ�
class  CTbl_Fee_Templet_Detail
{
protected:
	
	CMemTableNew <S_MDB_FEE_TEMPLET_DETAIL> m_table;
	CKeyField<CKey_Fee_Templet_Detail>    m_key;   //���� ģ��ID+Ʒ�����+Ʒ�ֱ��+��Լ+���ñ��+�շ�ģʽ
	CIndexField<CIndex_Fee_Templet_Detail> m_index;//ģ��ID+Ʒ�����+Ʒ�ֱ��+��Լ+���ñ��
	CBF_Mutex                   m_mutex;//������
public:
	CTbl_Fee_Templet_Detail()
	{
		
	}
	virtual ~CTbl_Fee_Templet_Detail()
	{
		
	}
	void Clear()
	{
		m_key.Clear();
		m_table.Clear();
		m_index.Clear();
	}
	int Insert(S_MDB_FEE_TEMPLET_DETAIL info) 
	{
		CBF_PMutex pp(&m_mutex);
		CKey_Fee_Templet_Detail aa;
		CIndex_Fee_Templet_Detail bb;
		strcpy(aa.sFareModeCode,info.sFareModeCode);
		strcpy(aa.sProdType,info.sProdType);
		aa.nVarietyNo = info.nVarietyNo;
		aa.nContractNo = info.nContractNo;
		strcpy(aa.sFareNo,info.sFareNo);
		strcpy(aa.sFareMode,info.sFareMode);
		
		if (m_key.Find(aa))
		{
			return -1;
		}
		strcpy(bb.sFareModeCode,info.sFareModeCode);
		strcpy(bb.sProdType,info.sProdType);
		bb.nVarietyNo = info.nVarietyNo;
		bb.nContractNo = info.nContractNo;
		strcpy(bb.sFareNo,info.sFareNo);
		
		int rid=m_table.Add(info);
		aa.m_nRowId = rid;
		bb.m_nRowId = rid;
		m_key.Add(aa);
		m_index.Add(bb);
		return rid;
	}
	int Update(S_MDB_FEE_TEMPLET_DETAIL info)
	{
		CBF_PMutex pp(&m_mutex);
		CKey_Fee_Templet_Detail aa;
		CIndex_Fee_Templet_Detail bb;
		strcpy(aa.sFareModeCode,info.sFareModeCode);
		strcpy(aa.sProdType,info.sProdType);
		aa.nVarietyNo = info.nVarietyNo;
		aa.nContractNo = info.nContractNo;
		strcpy(aa.sFareNo,info.sFareNo);
		strcpy(aa.sFareMode,info.sFareMode);
		
		if (m_key.Find(aa))
		{
			m_table.m_table[aa.m_nRowId].dValue = info.dValue;
			return aa.m_nRowId;
		}
		strcpy(bb.sFareModeCode,info.sFareModeCode);
		strcpy(bb.sProdType,info.sProdType);
		bb.nVarietyNo = info.nVarietyNo;
		bb.nContractNo = info.nContractNo;
		strcpy(bb.sFareNo,info.sFareNo);
		
		int rid=m_table.Add(info);
		aa.m_nRowId = rid;
		bb.m_nRowId = rid;
		m_key.Add(aa);
		m_index.Add(bb);
		return rid;

	}
	// ������: *Select
	// ���  : ������ 2018-2-7 13:54:46
	// ����  : S_MDB_FEE_TEMPLET_DETAIL 
	// ����  : const char *sFeeTempletId ����ģ��id
	// ����  : const char *sProdType     Ʒ������
	// ����  : unsigned int nVariety     Ʒ��
	// ����  : unsigned int nContractNo  ��Լ
	// ����  : const char *sFeeNo        ���ñ��
	// ����  : const char *sFeeMode      �շ�ģʽ
	// ����  : ȡ����ģ������
	S_MDB_FEE_TEMPLET_DETAIL *Select(const char *sFeeTempletId,const char *sProdType,unsigned int nVariety,unsigned int nContractNo,const char *sFeeNo,const char *sFeeMode)
	{
		CBF_PMutex pp(&m_mutex);
		CKey_Fee_Templet_Detail aa;
		strcpy(aa.sFareModeCode,sFeeTempletId);
		strcpy(aa.sProdType,sProdType);
		aa.nVarietyNo = nVariety;
		aa.nContractNo = nContractNo;
		strcpy(aa.sFareNo,sFeeNo);
		strcpy(aa.sFareMode,sFeeMode);
		if (!m_key.Find(aa))
		{
			return NULL;
		}
		return &(m_table.m_table[aa.m_nRowId]);
	}
	
	bool Select(const char *sFeeTempletId,const char *sProdType,unsigned int nVariety,unsigned int nContractNo,const char *sFeeNo,std::vector<S_MDB_FEE_TEMPLET_DETAIL *> &reslist)
	{
		CBF_PMutex pp(&m_mutex);
		CIndex_Fee_Templet_Detail bb;
		strcpy(bb.sFareModeCode,sFeeTempletId);
		strcpy(bb.sProdType,sProdType);
		bb.nVarietyNo = nVariety;
		bb.nContractNo = nContractNo;
		strcpy(bb.sFareNo,sFeeNo);

		std::vector<int> iset;
		bool bret = m_index.Find(bb);
		if (!bret)
		{
			return false;
		}
		while (bret)
		{
			reslist.push_back(&(m_table.m_table[bb.m_nRowId]));
			bret = m_index.Next(bb);
		}
		return true;
	}
	S_MDB_FEE_TEMPLET_DETAIL *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
};


//�ͻ�����������ñ�����
class CKey_Cust_Fee_Detail : public CIndex_Field_Base
{
public:
	char sTxAcc[21];//�ʽ��˺�
	char sProdType[2];//Ʒ������
	unsigned int nVarietyNo;//Ʒ�ֺ�
	unsigned int nContractNo;//��Լ��
	char sFareNo[4];//���ñ��
	char sFareMode[2];//�շ�ģʽ

	
	virtual bool operator<(const CKey_Cust_Fee_Detail& field) const
	{
		int ret=strcmp(sTxAcc,field.sTxAcc);
		if (ret<0)
		{
			return true;
		}
		else if (ret == 0)
		{
			if  (sProdType[0] < field.sProdType[0])
			{
				return true;
			}
			else if  (sProdType[0] == field.sProdType[0])
			{
				if (nVarietyNo< field.nVarietyNo)
				{
					return true;
				}
				else if (nVarietyNo == field.nVarietyNo)
				{
					if (nContractNo <field.nContractNo)
					{
						return true;
					}
					else if (nContractNo == field.nContractNo)
					{
						ret = strcmp(sFareNo,field.sFareNo);
						if (ret <0)
						{
							return true;
						}
						else if (ret == 0)
						{
							if (sFareMode[0] < field.sFareMode[0])
							{
								return true;
							}
						}
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

class CIndex_Cust_Fee_Detail : public CIndex_Field_Base
{
public:
	char sTxAcc[21];
	char sProdType[2];//Ʒ������
	unsigned int nVarietyNo;//Ʒ�ֺ�
	unsigned int nContractNo;//��Լ��
	char sFareNo[4];//���ñ��

	
	virtual bool operator<(const CIndex_Cust_Fee_Detail& field) const
	{
		int ret=strcmp(sTxAcc,field.sTxAcc);
		if (ret<0)
		{
			return true;
		}
		else if (ret == 0)
		{
			if  (sProdType[0] < field.sProdType[0])
			{
				return true;
			}
			else if  (sProdType[0] == field.sProdType[0])
			{
				if (nVarietyNo< field.nVarietyNo)
				{
					return true;
				}
				else if (nVarietyNo == field.nVarietyNo)
				{
					if (nContractNo <field.nContractNo)
					{
						return true;
					}
					else if (nContractNo == field.nContractNo)
					{
						ret = strcmp(sFareNo,field.sFareNo);
						if (ret <0)
						{
							return true;
						}
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

//�ͻ�����������ñ�
class  CTbl_Cust_Fee_Detail
{
protected:
	
	CMemTableNew <S_MDB_CUST_FEE> m_table;
	CKeyField<CKey_Cust_Fee_Detail>    m_key;   //���� �ʽ��˺�+Ʒ�����+Ʒ�ֱ��+��Լ+���ñ��+�շ�ģʽ
	CIndexField<CIndex_Cust_Fee_Detail> m_index;//�ʽ��˺� +Ʒ�����+Ʒ�ֱ��+��Լ+���ñ��

	CIndexVarChar<1>                   m_indexacc;//�ʽ��˺�
	CBF_Mutex                   m_mutex;//������
public:
	CTbl_Cust_Fee_Detail()
	{
		
	}
	virtual ~CTbl_Cust_Fee_Detail()
	{
		
	}
	void Clear()
	{
		m_key.Clear();
		m_index.Clear();
		m_indexacc.Clear();
		m_table.Clear();
	}
	int Insert(S_MDB_CUST_FEE info) 
	{
		CBF_PMutex pp(&m_mutex);
		CKey_Cust_Fee_Detail aa;
		CIndex_Cust_Fee_Detail bb;
		strcpy(aa.sTxAcc,info.sTxAcc);
		strcpy(aa.sProdType,info.sProdType);
		aa.nVarietyNo = info.nVarietyNo;
		aa.nContractNo = info.nContractNo;
		strcpy(aa.sFareNo,info.sFareNo);
		strcpy(aa.sFareMode,info.sFareMode);
		
		if (m_key.Find(aa))
		{
			return -1;
		}
		strcpy(bb.sTxAcc,info.sTxAcc);
		strcpy(bb.sProdType,info.sProdType);
		bb.nVarietyNo = info.nVarietyNo;
		bb.nContractNo = info.nContractNo;
		strcpy(bb.sFareNo,info.sFareNo);
		
		int rid=m_table.Add(info);
		aa.m_nRowId = rid;
		bb.m_nRowId = rid;
		m_key.Add(aa);
		m_index.Add(bb);
		m_indexacc.Add(rid,m_table.m_table[rid].sTxAcc);
		return rid;
	}

	// ������: *Select
	// ���  : ������ 2018-2-7 13:54:46
	// ����  : S_MDB_CUST_FEE 
	// ����  : const char *stxacc �ʽ��˺�
	// ����  : const char *sProdType     Ʒ������
	// ����  : unsigned int nVariety     Ʒ��
	// ����  : unsigned int nContractNo  ��Լ
	// ����  : const char *sFeeNo        ���ñ��
	// ����  : const char *sFeeMode      �շ�ģʽ
	// ����  : ȡ����ģ������
	S_MDB_CUST_FEE *Select(const char *stxacc,const char *sProdType,unsigned int nVariety,unsigned int nContractNo,const char *sFeeNo,const char *sFeeMode)
	{
		CBF_PMutex pp(&m_mutex);
		CKey_Cust_Fee_Detail aa;
		strcpy(aa.sTxAcc,stxacc);
		strcpy(aa.sProdType,sProdType);
		aa.nVarietyNo = nVariety;
		aa.nContractNo = nContractNo;
		strcpy(aa.sFareNo,sFeeNo);
		strcpy(aa.sFareMode,sFeeMode);
		if (!m_key.Find(aa))
		{
			return NULL;
		}
		return &(m_table.m_table[aa.m_nRowId]);
	}
	
	bool Select(const char *stxacc,const char *sProdType,unsigned int nVariety,unsigned int nContractNo,const char *sFeeNo,std::vector<S_MDB_CUST_FEE *> &reslist)
	{
		CIndex_Cust_Fee_Detail bb;
		strcpy(bb.sTxAcc,stxacc);
		strcpy(bb.sProdType,sProdType);
		bb.nVarietyNo = nVariety;
		bb.nContractNo = nContractNo;
		strcpy(bb.sFareNo,sFeeNo);

		std::vector<int> iset;
		bool bret = m_index.Find(bb);
		if (!bret)
		{
			return false;
		}
		while (bret)
		{
			reslist.push_back(&(m_table.m_table[bb.m_nRowId]));
			bret = m_index.Next(bb);
		}
		return true;
	}
	S_MDB_CUST_FEE *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}

	// ������: DeleteByAcc
	// ���  : ������ 2018-4-2 15:03:40
	// ����  : bool 
	// ����  : char *txacc
	// ����  : ɾ���ʽ��˺ŵ�����
	bool DeleteByAcc(char *txacc)
	{
		CInt iset;
		//�ҵ�ROWID
		if (!m_indexacc.Select(iset,txacc))
		{
			return true;
		}
		//ɾ���ʽ��˺�����
		m_indexacc.Delete(iset,txacc);
		//ɾ������
		CIndex_Cust_Fee_Detail bb;
		bool bret = m_index.First(bb);
		while (bret)
		{
			if (iset.Find(bb.m_nRowId))
			{
				m_index.EraseOne();
				bret = m_index.First(bb);
			}
			else
			{
				bret = m_index.Next(bb);
			}
			
		}
		//ɾ������
		CKey_Cust_Fee_Detail aa;
		bret = m_key.First(aa);
		while (bret)
		{
			if (iset.Find(aa.m_nRowId))
			{
				m_key.EraseOne();
				bret = m_key.First(aa);
			}
			else
			{
				bret = m_key.Next(aa);
			}
			
		}
		//ɾ��������
		int rid;
		bret = iset.First(rid);
		while (bret)
		{
			m_table.Delete(rid);
			bret = iset.Next(rid);
		}
		
		return true;
	}
};


//����������Ϣ
class  CTbl_Brokerage
{
protected:

	CMemTableNew <S_MDB_BROKERAGE_INFO> m_table;
	CPkeyUInt<1>                m_key;
	CBF_Mutex                   m_mutex;//������
public:
	CTbl_Brokerage()
	{

	}
	virtual ~CTbl_Brokerage()
	{

	}
	void Clear()
	{
		m_key.Clear();
		m_table.Clear();
	}
	int Insert(S_MDB_BROKERAGE_INFO info) //ֻ���ڳ�ʼ��ʱʹ�ã����Կ��Բ�����
	{
		CBF_PMutex pp(&m_mutex);
		if (m_key.Find(info.nSysCode))
		{
			return -1;
		}
		int rid=m_table.Add(info);
		m_key.Add(rid,m_table.m_table[rid].nSysCode);
		return rid;
	}

	S_MDB_BROKERAGE_INFO *Select(unsigned int brokerageid)
	{
		int rid;
		CBF_PMutex pp(&m_mutex);
		if (!m_key.Select(rid,brokerageid))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}
	S_MDB_BROKERAGE_INFO *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
};


class CKey_Brokerage_Exchange : public CIndex_Field_Base
{
public:
	unsigned int nSysCode;//ϵͳ����
	char  sExchCode[2];//����������
	char  sSeatNo[7];//ϯλ����


	virtual bool operator<(const CKey_Brokerage_Exchange& field) const
	{
		if (nSysCode<field.nSysCode)
		{
			return true;
		}
		else if (nSysCode == field.nSysCode)
		{
			if  (sExchCode[0] < field.sExchCode[0])
			{
				return true;
			}
			else if  (sExchCode[0] == field.sExchCode[0])
			{
				if (strcmp(sSeatNo,field.sSeatNo) <0)
				{
					return true;
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
class CIndex_Brokerage_Exchange : public CIndex_Field_Base
{
public:
	unsigned int nSysCode;//ϵͳ����
	char  sExchCode[2];//����������

	virtual bool operator<(const CIndex_Brokerage_Exchange& field) const
	{
		if (nSysCode<field.nSysCode)
		{
			return true;
		}
		else if (nSysCode == field.nSysCode)
		{
			if  (sExchCode[0] < field.sExchCode[0])
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
//���Ͷ�Ӧ������
class  CTbl_Brokerage_Exchange
{
protected:

	CMemTableNew <S_MDB_BROKERAGE_EXCHANGE> m_table;
	CKeyField<CKey_Brokerage_Exchange>    m_key;   //���� ���ͱ��+������+ϯλ��
	CIndexField<CIndex_Brokerage_Exchange> m_index; // ���ͱ��+������
	CIndexUInt<1>                   m_indexSys;//���ͱ��
	CBF_Mutex                   m_mutex;//������
public:
	CTbl_Brokerage_Exchange()
	{

	}
	virtual ~CTbl_Brokerage_Exchange()
	{

	}
	void Clear()
	{
		m_key.Clear();
		m_index.Clear();
		m_indexSys.Clear();
		m_table.Clear();
	}
	int Insert(S_MDB_BROKERAGE_EXCHANGE info) 
	{
		CBF_PMutex pp(&m_mutex);
		CKey_Brokerage_Exchange aa;
		CIndex_Brokerage_Exchange bb;

		strcpy(aa.sExchCode,info.sExchCode);
		strcpy(aa.sSeatNo,info.sSeatNo);
		aa.nSysCode = info.nSysCode;
		

		if (m_key.Find(aa))
		{
			return -1;
		}
		strcpy(bb.sExchCode,info.sExchCode);
		bb.nSysCode = info.nSysCode;

		int rid=m_table.Add(info);
		aa.m_nRowId = rid;
		bb.m_nRowId = rid;
		m_key.Add(aa);
		m_index.Add(bb);
		m_indexSys.Add(rid,m_table.m_table[rid].nSysCode);
		return rid;
	}

	// ������: *Select
	// ���  : ������ 2018-2-7 13:54:46
	// ����  : S_MDB_BROKERAGE_EXCHANGE 
	// ����  : unsigned int nsyscode
	// ����  :char *exchcode    Ʒ������
	// ����  : char *seatno     Ʒ��
	// ����  : ȡ�����ж�Ӧ����������
	S_MDB_BROKERAGE_EXCHANGE *Select(unsigned int nsyscode,char *exchcode,char *seatno)
	{
		CBF_PMutex pp(&m_mutex);
		CKey_Brokerage_Exchange aa;
		strcpy(aa.sExchCode,exchcode);
		strcpy(aa.sSeatNo,seatno);
		aa.nSysCode = nsyscode;
		if (!m_key.Find(aa))
		{
			return NULL;
		}
		return &(m_table.m_table[aa.m_nRowId]);
	}

	bool Select(unsigned int nsyscode,std::vector<S_MDB_BROKERAGE_EXCHANGE *> &reslist)
	{
		std::vector<int> iset;
		if (!m_indexSys.Select(iset,nsyscode))
		{
			return false;
		}
		for (int i=0; i<iset.size(); i++)
		{
			reslist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
	}
	bool Select(unsigned int nsyscode,char *exchcode,std::vector<S_MDB_BROKERAGE_EXCHANGE *> &reslist)
	{
		CIndex_Brokerage_Exchange bb;
		strcpy(bb.sExchCode,exchcode);
		bb.nSysCode = nsyscode;
		

		std::vector<int> iset;
		bool bret = m_index.Find(bb);
		if (!bret)
		{
			return false;
		}
		while (bret)
		{
			reslist.push_back(&(m_table.m_table[bb.m_nRowId]));
			bret = m_index.Next(bb);
		}
		return true;
	}
	S_MDB_BROKERAGE_EXCHANGE *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}

};

class CKey_Brokerage_Pg : public CIndex_Field_Base
{
public:
	unsigned int nSysCode;//ϵͳ����
	char  sBrokerId[11];//pg��brokerid


	virtual bool operator<(const CKey_Brokerage_Pg& field) const
	{
		if (nSysCode<field.nSysCode)
		{
			return true;
		}
		else if (nSysCode == field.nSysCode)
		{
			if (strcmp(sBrokerId,field.sBrokerId) <0)
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

//���Ͷ�ӦPG
class  CTbl_Brokerage_Pg
{
protected:

	CMemTableNew <S_MDB_BROKERAGE_PG> m_table;
	CKeyField<CKey_Brokerage_Pg>    m_key;   //���� ���ͱ��+brokerid
	CIndexUInt<1>                   m_indexSys;//���ͱ��
	CBF_Mutex                   m_mutex;//������
public:
	CTbl_Brokerage_Pg()
	{

	}
	virtual ~CTbl_Brokerage_Pg()
	{

	}
	void Clear()
	{
		m_key.Clear();
		m_indexSys.Clear();
		m_table.Clear();
	}
	int Insert(S_MDB_BROKERAGE_PG info) 
	{
		CBF_PMutex pp(&m_mutex);
		CKey_Brokerage_Pg aa;

		strcpy(aa.sBrokerId,info.sBrokerId);
		aa.nSysCode = info.nSysCode;


		if (m_key.Find(aa))
		{
			return -1;
		}
		aa.nSysCode = info.nSysCode;

		int rid=m_table.Add(info);
		aa.m_nRowId = rid;
		m_key.Add(aa);
		m_indexSys.Add(rid,m_table.m_table[rid].nSysCode);
		return rid;
	}

	// ������: *Select
	// ���  : ������ 2018-2-7 13:54:46
	// ����  : S_MDB_BROKERAGE_EXCHANGE 
	// ����  : unsigned int nsyscode
	// ����  :char *exchcode    Ʒ������
	// ����  : char *seatno     Ʒ��
	// ����  : ȡ�����ж�Ӧ����������
	S_MDB_BROKERAGE_PG *Select(unsigned int nsyscode,char *sbrokerid)
	{
		CBF_PMutex pp(&m_mutex);
		CKey_Brokerage_Pg aa;
		strcpy(aa.sBrokerId,sbrokerid);
		aa.nSysCode = nsyscode;
		if (!m_key.Find(aa))
		{
			return NULL;
		}
		return &(m_table.m_table[aa.m_nRowId]);
	}

	bool Select(unsigned int nsyscode,std::vector<S_MDB_BROKERAGE_PG *> &reslist)
	{
		std::vector<int> iset;
		if (!m_indexSys.Select(iset,nsyscode))
		{
			return false;
		}
		for (int i=0; i<iset.size(); i++)
		{
			reslist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
	}
	S_MDB_BROKERAGE_PG *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}

};
//����ұ���
class  CTbl_CurrencyRate
{
protected:

	CMemTableNew <S_MDB_CURRENCY_RATE> m_table;
	CPkeyCharV<1>                m_key;
	CBF_Mutex                   m_mutex;//������
public:
	CTbl_CurrencyRate()
	{

	}
	virtual ~CTbl_CurrencyRate()
	{

	}
	void Clear()
	{
		m_key.Clear();
		m_table.Clear();
	}
	int Insert(S_MDB_CURRENCY_RATE info) //ֻ���ڳ�ʼ��ʱʹ�ã����Կ��Բ�����
	{
		CBF_PMutex pp(&m_mutex);
		if (m_key.Find(info.sCurrency))
		{
			return -1;
		}
		int rid=m_table.Add(info);
		m_key.Add(rid,m_table.m_table[rid].sCurrency);
		return rid;
	}

	S_MDB_CURRENCY_RATE *Select(const char *currency)
	{
		int rid;
		CBF_PMutex pp(&m_mutex);
		if (!m_key.Select(rid,(char *)currency))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}
	S_MDB_CURRENCY_RATE *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
};


class CDb_Set  
{
public:
	CDb_Set()
	{

	}
	virtual ~CDb_Set()
	{

	}
	CTbl_WarnLevel           m_pWarnLevel;//Ԥ�������
	CTbl_RiskContract_Set    m_pRiskContractSet;//���պ�Լ���ñ�
	
	
	CTbl_Prod_Monitor        m_pProdFundMonitor;//��Ʒ�����ʽ������ñ�
	CTbl_Prod_Monitor        m_pProdPosiMonitor;//��Ʒ�����ֲּ�����ñ�

	CTbl_Templet             m_pFundTemplet;//�ͻ��ʽ�ģ����Ϣ
	CTbl_Templet             m_pPosiTemplet;//�ͻ��ֲ�ģ����Ϣ
	CTbl_Fund_Templet        m_pFundTempletDetail;//�ͻ��ʽ�ģ�����ñ�
	CTbl_Position_Templet    m_pPosiTempletDetail;//�ͻ��ֲ�ģ�����ñ�
	CTbl_Cust_Templet        m_pCustTemplet;//�ʽ��˺�ģ���Ӧ��ϵ��

	CTbl_Fee_Templet_Info    m_pFeeTemplet;//����ģ����Ϣ
	CTbl_Fee_Templet_Detail  m_pFeeTempletDetail;//����ģ������
	CTbl_Cust_Fee_Detail     m_pCustFeeDetail;//�ͻ�����������ñ�

	CTbl_Brokerage           m_pBrokerage;//����������Ϣ  
	CTbl_Brokerage_Exchange  m_pBrokerageExchange;//�ͼͶ�Ӧ������
	CTbl_Brokerage_Pg        m_pBrokeragePg;//�ͼͶ�Ӧpg

	CTblPordOrg_Templet      m_pProdTemplet;//��Ʒģ���Ӧ��ϵ
	CTblPordOrg_Templet      m_pOrgTemplet;//����ģ���Ӧ��ϵ
	CTbl_CurrencyRate        m_pCurrencyRate;//������ұ�������

};

#endif // !defined(AFX_DB_SET_H__4B14D6AB_CE59_4B41_A502_27B567589F24__INCLUDED_)
