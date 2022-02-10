// Db_Set.h: interface for the CDb_Set class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DB_SET_H__4B14D6AB_CE59_4B41_A502_27B567589F24__INCLUDED_)
#define AFX_DB_SET_H__4B14D6AB_CE59_4B41_A502_27B567589F24__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "BF_Mutex.h"
//数据库 设置 

//预警设置
typedef struct 
{
	unsigned short  nWarnLevel;//预警级数
	char sWarnName[41];//预警名称
	char sWarnColor[41];//预警颜色
	char sWarnSound[41];//预警声音
	unsigned int  nWarnSep;//预警间隔 秒
}S_MDB_WARN_LEVEL_INFO;

//风险合约设置
typedef struct 
{
	unsigned int nVarietyNo;//品种编号
	unsigned int nContractNo;//合约编号
	char   sContractCode[21];//合约代码
	char   sWarnItem[2];//预警项
	char   sCondition[2];//条件
	double dValue;//预警值
}S_MDB_RISKCONTRACT_SET;


//资金模板配置表
typedef struct 
{
	char   sFundTempletId[13];//模ID
	char   sFundTempletName[41];//模板名称
	unsigned short nWarnNo;//预警序号
	char   sFundModel[2];//资金指标
	char   sCondition[2];//条件
	double dValue;//预警值
	char   sRight[2];//权限
}S_MDB_FUND_TEMPLET;
//持仓模板配置表
typedef struct 
{
	char   sPosiTempletId[13];//模ID
	unsigned short nWarnNo;//预警序号
	char   sExchCode[2];//交易所代码
	char   sProdType[2];//品种类别
	unsigned int nVarietyNo;//品种编号
	char    sVarietyCode[21];//品种代码
	unsigned int nOption_desno;//期权标的合约编号
	char   sOption_destcode[21];//期权标的合约代码
	unsigned int nContractNo;//合约编号
	char   sContractCode[21];//合约代码
	char   sPosiModel[2];//持仓指标
	char   sCondition[2];//条件
	double dValue;//预警值
	char   sRight[2];//权限
}S_MDB_POSITION_TEMPLET;

//资产机构资金持仓监控表结构   资金监控表和持仓监控表结构相同，内容不同
typedef struct  
{
	char sTempletId[13];//模板ID
	unsigned short nWarnNo;//预警序号
	char sModel[2];  //资产指标
	char sCondition[2];//条件
	double dValue;//阀值
	char sRight[2];//交易权限
}S_MDB_PROD_MONITOR;

//资金账号模板对应表
typedef struct 
{
	char   sTxAcc[21];//资金账号
	char   sFundTempletId[13];//资金模板ID
	char   sPosiTempletId[13];//持仓模板ID
}S_MDB_CUST_TEMPLET_RELA;

//产品机构对应模板表
typedef struct 
{
	char   sProdOrg[13];//产品机构
	char   sFundTempletId[13];//资金模板ID
	char   sPosiTempletId[13];//持仓模板ID
}S_MDB_PRODORG_TEMPLET_RELA;
//模板表
typedef struct 
{
	char   sTempletId[13];//模板ID
	char   sTempletName[41];//模板名称
	char   sOrgCode[13];//所属机构
	char   sSubCanUse[2];//是否下级适用
}S_MDB_TEMPLET_INFO;


typedef struct 
{
	char sFareModeCode[14];//模板代码
	char sFareModeName[41];//模板名称
	char sFareType[2];//模板类型
	char sOrgCode[13];//所属机构
}S_MDB_FEE_TEMPLET;

typedef struct 
{
	char sFareModeCode[14];//模板代码
	char sProdType[2];//品种类型
	unsigned int nVarietyNo;//品种号
	char sVarietyCode[21];//品种代码
	unsigned int nContractNo;//合约号
	char sContractCode[21];//合约代码
	char sFareNo[4];//费用编号
	char sFareMode[2];//收费模式
	double dValue;//费率值
}S_MDB_FEE_TEMPLET_DETAIL;

typedef struct 
{
	char sTxAcc[21];
	char sProdType[2];//品种类型
	unsigned int nVarietyNo;//品种号
	char sVarietyCode[21];//品种代码
	unsigned int nContractNo;//合约号
	char sContractCode[21];//合约代码
	char sFareNo[4];//费用编号
	char sFareMode[2];//收费模式
	double dValue;//费率值
	char sInclude[2];//是否包含交易所
}S_MDB_CUST_FEE;


typedef struct 
{
	unsigned int nSysCode;//系统代码
	char  sSysName[41];//系统名称
	unsigned int  nSvrId;//系统注册到总线的公共服务号
	char  sOrgCode[17];//对应的机构代码
	char  sClearDate[9];//结算日期
	unsigned int   nClearFlag; //清算标志 0数据导入  1清算
	//0 不清算，导入柜台数据
	//1 清算       直接连交易所
	//	2 不清算   但连柜台的多个PG
	//	3 不清算       直接连交易所
}S_MDB_BROKERAGE_INFO; //经纪配置
typedef struct 
{
	unsigned int nSysCode;//系统代码
	char  sExchCode[2];//交易所代码
	char  sSeatNo[7];//席位代码
	unsigned int   nSvrId; //在总线上注册的服务号
}S_MDB_BROKERAGE_EXCHANGE; //经纪交易所配置
typedef struct 
{
	unsigned int nSysCode;//系统代码
	char  sBrokerId[11];//pg对应的brokerid
	unsigned int   nSvrId; //在总线上注册的服务号
}S_MDB_BROKERAGE_PG; //经纪PG配置
typedef struct  
{
	char sCurrency[2]; //币种
	double dRate;     //兑人民币的比率
}S_MDB_CURRENCY_RATE;

class  CTbl_WarnLevel
{
protected:
	
	CMemTableNew <S_MDB_WARN_LEVEL_INFO> m_table;
	CPkeyUInt<1>          m_pkey;               //预警级数
	CBF_Mutex                   m_mutex;//互斥锁
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
	int Insert(S_MDB_WARN_LEVEL_INFO info) //只有在初始化时使用，所以可以不加锁
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
	
	// 函数名: *Select
	// 编程  : 王明松 2017-8-3 10:20:35
	// 返回  : S_MDB_WARN_LEVEL_INFO 
	// 参数  : unsigned int warnlevel
	// 描述  : 根据预警级别查找级别设置
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

//风险合约设置主键
class CKey_RiskContractSet : public CIndex_Field_Base
{
public:
	unsigned int nVarietyNo;//品种编号
	unsigned int nContractNo;//合约编号
	char   sWarnItem[2];//预警项
	
	
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
	CBF_Mutex                   m_mutex;//互斥锁
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
	int Insert(S_MDB_RISKCONTRACT_SET info) //只有在初始化时使用，所以可以不加锁
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


//客户资金模板配置表主键
class CKey_Fund_Templet : public CIndex_Field_Base
{
public:
	char   sFundTempletId[13];//模ID
	unsigned short nWarnNo;//预警序号
	char   sFundModel[2];//资金指标
	
	
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
//客户资金模板配置order by 索引
class CIndex_FundTemplet_NoModel: public CIndex_Field_Base
{
public:
	unsigned short nWarnNo;//预警序号
	char   sFundModel[2];//资金指标
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
//客户资金模板配置表
class  CTbl_Fund_Templet
{
protected:
	
	CMemTableNew <S_MDB_FUND_TEMPLET> m_table;
	CKeyField<CKey_Fund_Templet>    m_key;   //主键 模板ID,预警序号，资金指标
//	CIndexCharF<13,1>                m_index;//模板ID的索引
	CIndexVarChar<1>                m_index;//模板ID的索引
	CBF_Mutex                   m_mutex;//互斥锁
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
	int Insert(S_MDB_FUND_TEMPLET info) //只有在初始化时使用，所以可以不加锁
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
	//取出资金模板的配置，按预警序号+资金指标排序
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

//客户持仓模板配置表主键
class CKey_Position_Templet : public CIndex_Field_Base
{
public:
	char   sPosiTempletId[13];//模ID
	unsigned short nWarnNo;//预警序号
	char   sProdType[2];//品种类别
	unsigned int nVarietyNo;//品种编号
	unsigned int nOption_desno;//期权标的合约编号
	unsigned int nContractNo;//合约编号
	char   sPosiModel[2];//持仓指标
	
	
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
//持仓模板索引，根据此索引查出合约的配置，按预警序号排序后比较得到持仓的风险
class CIndex_Position_Templet : public CIndex_Field_Base
{
public:
	char   sPosiTempletId[13];//模板ID
	char   sProdType[2];//品种类别
	unsigned int nVarietyNo;//品种编号
	unsigned int nOption_desno;//期权标的合约编号
	unsigned int nContractNo;//合约编号
	
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
//客户持仓模板配置order by 索引
class CIndex_PosiTemplet_NoModel: public CIndex_Field_Base
{
public:
	unsigned short nWarnNo;//预警序号
	char   sPosiModel[2];//持仓指标
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
//客户持仓模板配置表
class  CTbl_Position_Templet
{
protected:
	
	CMemTableNew <S_MDB_POSITION_TEMPLET> m_table;
	CKeyField<CKey_Position_Templet>    m_key;   //主键 模板ID+预警序号+品种类别+品种编号+标的合约+合约+资金指标
	CIndexField<CIndex_Position_Templet> m_index;//模板ID+品种类别+品种编号+标的合约+合约的索引
	CBF_Mutex                   m_mutex;//互斥锁
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
	//取出资金模板的配置，按预警序号+资金指标排序
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

//资产监控表主键
class CKey_Prod_Monitor : public CIndex_Field_Base
{
public:
	char sTempletId[13];//模板ID
	unsigned short nWarnNo;//预警序号
	char sModel[2];  //资产指标
	
	
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


//索引，order  by  预警序号+资产指标
class CIndex_Prod_Monitor : public CIndex_Field_Base
{
public:
	unsigned short nWarnNo;//预警序号
	char sModel[2];  //资产指标


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
//产品机构资金或持仓监控配置表
class  CTbl_Prod_Monitor
{
protected:
	
	CMemTableNew <S_MDB_PROD_MONITOR> m_table;
	CKeyField<CKey_Prod_Monitor>          m_key; 
	CIndexVarChar<1>                     m_index;//模板ID索引
	CBF_Mutex                   m_mutex;//互斥锁
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

//模板表
class  CTbl_Templet
{
protected:
	
	CMemTableNew <S_MDB_TEMPLET_INFO> m_table;
//	CPkeyCharF<13,1>                m_key;
	CPkeyCharV<1>                m_key;
	CBF_Mutex                   m_mutex;//互斥锁
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
	int Insert(S_MDB_TEMPLET_INFO info) //只有在初始化时使用，所以可以不加锁
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

//资金账号模板对应关系表
class  CTbl_Cust_Templet
{
protected:
	
	CMemTableNew <S_MDB_CUST_TEMPLET_RELA> m_table;
//	CPkeyCharF<21,1>                m_key;
	CPkeyCharV<1>                m_key;
	CBF_Mutex                   m_mutex;//互斥锁
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
	int Insert(S_MDB_CUST_TEMPLET_RELA info) //只有在初始化时使用，所以可以不加锁
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

//产品机构模板对应关系
class  CTblPordOrg_Templet
{
protected:

	CMemTableNew <S_MDB_PRODORG_TEMPLET_RELA> m_table;
	CPkeyCharV<1>                m_key;
	CBF_Mutex                   m_mutex;//互斥锁
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
	int Insert(S_MDB_PRODORG_TEMPLET_RELA info) //只有在初始化时使用，所以可以不加锁
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

//费率模板信息表
class  CTbl_Fee_Templet_Info
{
protected:
	
	CMemTableNew <S_MDB_FEE_TEMPLET> m_table;
// 	CPkeyCharF<14,1>               m_key;//模板ID主键
// 	CIndexCharF<13,1>            m_indexOrg;//机构索引
	CPkeyCharV<1>               m_key;//模板ID主键
	CIndexVarChar<1>            m_indexOrg;//机构索引
	CBF_Mutex                   m_mutex;//互斥锁
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



//费率模板配置表主键
class CKey_Fee_Templet_Detail : public CIndex_Field_Base
{
public:
	char sFareModeCode[14];//模板代码
	char sProdType[2];//品种类型
	unsigned int nVarietyNo;//品种号
	unsigned int nContractNo;//合约号
	char sFareNo[4];//费用编号
	char sFareMode[2];//收费模式
	
	
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
	char sFareModeCode[14];//模板代码
	char sProdType[2];//品种类型
	unsigned int nVarietyNo;//品种号
	unsigned int nContractNo;//合约号
	char sFareNo[4];//费用编号
	
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

//费率模板配置表
class  CTbl_Fee_Templet_Detail
{
protected:
	
	CMemTableNew <S_MDB_FEE_TEMPLET_DETAIL> m_table;
	CKeyField<CKey_Fee_Templet_Detail>    m_key;   //主键 模板ID+品种类别+品种编号+合约+费用编号+收费模式
	CIndexField<CIndex_Fee_Templet_Detail> m_index;//模板ID+品种类别+品种编号+合约+费用编号
	CBF_Mutex                   m_mutex;//互斥锁
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
	// 函数名: *Select
	// 编程  : 王明松 2018-2-7 13:54:46
	// 返回  : S_MDB_FEE_TEMPLET_DETAIL 
	// 参数  : const char *sFeeTempletId 费率模板id
	// 参数  : const char *sProdType     品种类型
	// 参数  : unsigned int nVariety     品种
	// 参数  : unsigned int nContractNo  合约
	// 参数  : const char *sFeeNo        费用编号
	// 参数  : const char *sFeeMode      收费模式
	// 描述  : 取费率模板配置
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


//客户特殊费率配置表主键
class CKey_Cust_Fee_Detail : public CIndex_Field_Base
{
public:
	char sTxAcc[21];//资金账号
	char sProdType[2];//品种类型
	unsigned int nVarietyNo;//品种号
	unsigned int nContractNo;//合约号
	char sFareNo[4];//费用编号
	char sFareMode[2];//收费模式

	
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
	char sProdType[2];//品种类型
	unsigned int nVarietyNo;//品种号
	unsigned int nContractNo;//合约号
	char sFareNo[4];//费用编号

	
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

//客户特殊费率配置表
class  CTbl_Cust_Fee_Detail
{
protected:
	
	CMemTableNew <S_MDB_CUST_FEE> m_table;
	CKeyField<CKey_Cust_Fee_Detail>    m_key;   //主键 资金账号+品种类别+品种编号+合约+费用编号+收费模式
	CIndexField<CIndex_Cust_Fee_Detail> m_index;//资金账号 +品种类别+品种编号+合约+费用编号

	CIndexVarChar<1>                   m_indexacc;//资金账号
	CBF_Mutex                   m_mutex;//互斥锁
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

	// 函数名: *Select
	// 编程  : 王明松 2018-2-7 13:54:46
	// 返回  : S_MDB_CUST_FEE 
	// 参数  : const char *stxacc 资金账号
	// 参数  : const char *sProdType     品种类型
	// 参数  : unsigned int nVariety     品种
	// 参数  : unsigned int nContractNo  合约
	// 参数  : const char *sFeeNo        费用编号
	// 参数  : const char *sFeeMode      收费模式
	// 描述  : 取费率模板配置
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

	// 函数名: DeleteByAcc
	// 编程  : 王明松 2018-4-2 15:03:40
	// 返回  : bool 
	// 参数  : char *txacc
	// 描述  : 删除资金账号的配置
	bool DeleteByAcc(char *txacc)
	{
		CInt iset;
		//找到ROWID
		if (!m_indexacc.Select(iset,txacc))
		{
			return true;
		}
		//删除资金账号索引
		m_indexacc.Delete(iset,txacc);
		//删除索引
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
		//删除主键
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
		//删除表内容
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


//经纪配置信息
class  CTbl_Brokerage
{
protected:

	CMemTableNew <S_MDB_BROKERAGE_INFO> m_table;
	CPkeyUInt<1>                m_key;
	CBF_Mutex                   m_mutex;//互斥锁
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
	int Insert(S_MDB_BROKERAGE_INFO info) //只有在初始化时使用，所以可以不加锁
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
	unsigned int nSysCode;//系统代码
	char  sExchCode[2];//交易所代码
	char  sSeatNo[7];//席位代码


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
	unsigned int nSysCode;//系统代码
	char  sExchCode[2];//交易所代码

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
//经纪对应交易所
class  CTbl_Brokerage_Exchange
{
protected:

	CMemTableNew <S_MDB_BROKERAGE_EXCHANGE> m_table;
	CKeyField<CKey_Brokerage_Exchange>    m_key;   //主键 经纪编号+交易所+席位号
	CIndexField<CIndex_Brokerage_Exchange> m_index; // 经纪编号+交易所
	CIndexUInt<1>                   m_indexSys;//经纪编号
	CBF_Mutex                   m_mutex;//互斥锁
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

	// 函数名: *Select
	// 编程  : 王明松 2018-2-7 13:54:46
	// 返回  : S_MDB_BROKERAGE_EXCHANGE 
	// 参数  : unsigned int nsyscode
	// 参数  :char *exchcode    品种类型
	// 参数  : char *seatno     品种
	// 描述  : 取经纪行对应交易所配置
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
	unsigned int nSysCode;//系统代码
	char  sBrokerId[11];//pg的brokerid


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

//经纪对应PG
class  CTbl_Brokerage_Pg
{
protected:

	CMemTableNew <S_MDB_BROKERAGE_PG> m_table;
	CKeyField<CKey_Brokerage_Pg>    m_key;   //主键 经纪编号+brokerid
	CIndexUInt<1>                   m_indexSys;//经纪编号
	CBF_Mutex                   m_mutex;//互斥锁
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

	// 函数名: *Select
	// 编程  : 王明松 2018-2-7 13:54:46
	// 返回  : S_MDB_BROKERAGE_EXCHANGE 
	// 参数  : unsigned int nsyscode
	// 参数  :char *exchcode    品种类型
	// 参数  : char *seatno     品种
	// 描述  : 取经纪行对应交易所配置
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
//人民币比率
class  CTbl_CurrencyRate
{
protected:

	CMemTableNew <S_MDB_CURRENCY_RATE> m_table;
	CPkeyCharV<1>                m_key;
	CBF_Mutex                   m_mutex;//互斥锁
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
	int Insert(S_MDB_CURRENCY_RATE info) //只有在初始化时使用，所以可以不加锁
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
	CTbl_WarnLevel           m_pWarnLevel;//预警级别表
	CTbl_RiskContract_Set    m_pRiskContractSet;//风险合约设置表
	
	
	CTbl_Prod_Monitor        m_pProdFundMonitor;//产品机构资金监控配置表
	CTbl_Prod_Monitor        m_pProdPosiMonitor;//产品机构持仓监控配置表

	CTbl_Templet             m_pFundTemplet;//客户资金模板信息
	CTbl_Templet             m_pPosiTemplet;//客户持仓模板信息
	CTbl_Fund_Templet        m_pFundTempletDetail;//客户资金模板配置表
	CTbl_Position_Templet    m_pPosiTempletDetail;//客户持仓模板配置表
	CTbl_Cust_Templet        m_pCustTemplet;//资金账号模板对应关系表

	CTbl_Fee_Templet_Info    m_pFeeTemplet;//费率模板信息
	CTbl_Fee_Templet_Detail  m_pFeeTempletDetail;//费率模板配置
	CTbl_Cust_Fee_Detail     m_pCustFeeDetail;//客户特殊费率设置表

	CTbl_Brokerage           m_pBrokerage;//经纪配置信息  
	CTbl_Brokerage_Exchange  m_pBrokerageExchange;//纪纪对应交易所
	CTbl_Brokerage_Pg        m_pBrokeragePg;//纪纪对应pg

	CTblPordOrg_Templet      m_pProdTemplet;//产品模板对应关系
	CTblPordOrg_Templet      m_pOrgTemplet;//机构模板对应关系
	CTbl_CurrencyRate        m_pCurrencyRate;//兑人民币比率设置

};

#endif // !defined(AFX_DB_SET_H__4B14D6AB_CE59_4B41_A502_27B567589F24__INCLUDED_)
