// InitTrade.cpp: implementation of the CInitTrade class.
//
//////////////////////////////////////////////////////////////////////

#include "InitTrade.h"
#include "CodeTable.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//注意：下面的定义在每个类里面都要有，主程序根据此方法获取本类的实例
CInitTrade * getInstance() 
{ 
    return new CInitTrade(); 
}
CInitTrade::CInitTrade()
{
	m_pSqlCmd = NULL;
}

CInitTrade::~CInitTrade()
{
	m_pMemDb = NULL;
	if (m_pSqlCmd != NULL)
	{
		m_pSqlCmd->DisConnect();
		delete m_pSqlCmd;
		m_pSqlCmd = NULL;
	}
}
bool CInitTrade::Init(CIErrlog *log,CBF_Xml *xml,CMemDb *mdb)
{
	m_pMemDb = mdb;
	m_pLog = log;
	m_pXml = xml;

	m_pKernelPub.m_pLog = log;
	m_pKernelPub.m_pMemDb = mdb;

	return true;

	//写内存表
	//连接本地数据库，从本地读交易所、合约信息、机构信息、参数、操作员信息
	if (!ConnectLocalDb())
	{
		return false;
	}
	if (!InitPublic())
	{
		return false;
	}
	if (!InitBaseInfo())
	{
		return false;
	}
	if (!InitCust())
	{
		return false;
	}
	if (!InitSet())
	{
		return false;
	}
	if (!InitTrader())
	{
		return false;
	}
	//根据出入金、报单、成交、重新计算客户资金，持仓、大边保证金
	if (!ProcCustFund())
	{
		return false;
	}
	m_pSqlCmd->DisConnect();
	m_pMemDb->m_bDataInit = true;
	return true;
}

bool CInitTrade::InitPublic()
{
	//机构
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"ReadOrgan");
	if (!ReadOrgan())
	{
		return false;
	}
	//操作员
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"ReadOper");
	if (!ReadOper())
	{
		return false;
	}
//	S_MDB_OPER_INFO *po2 = m_pMemDb->m_pDbPublic.m_pOper.Select("songfree2");
	//系统 参数
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"ReadPara");
	if (!ReadPara())
	{
		return false;
	}
	//操作员在线
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"ReadOperOnline");
	if (!ReadOperOnline())
	{
		return false;
	}
	return true;
}


bool CInitTrade::ConnectLocalDb()
{
	//这里不用判断use属性是否为1.
	char tmpchar[100];
	memset(tmpchar,0,sizeof(tmpchar));
	std::string dbname;
	std::string dbuser;
	std::string dbpwd;
	std::string dbpwdflag;
	std::string dbtype;
	bool bret;

	if (m_pXml->GetNodeValueByPath("package/head/business/database/databaseserver",false,dbname,false) == NULL)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xml file no [package/head/business/database/databaseserver] node");
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"节点[package/head/business/database/databaseserver]不存在");
#endif
		return false;
	}
	if (m_pXml->GetNodeValueByPath("package/head/business/database/databaseuser",false,dbuser,false) == NULL)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xml file no [package/head/business/database/databaseuser] node");
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"节点[package/head/business/database/databaseuser] 不存在");
#endif
		return false;
	}
	if (m_pXml->GetNodeValueByPath("package/head/business/database/userpasswd",false,dbpwd,true) == NULL)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xml file no [package/head/business/database/userpasswd] node");
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"节点[package/head/business/database/userpasswd]不存在");
#endif
		return false;
	}
	if (m_pXml->GetNodeValueByPath("package/head/business/database/passwdtype",false,dbpwdflag,false) == NULL)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xml file no [package/head/business/database/passwdtype] node");
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"节点 [package/head/business/database/passwdtype] 不存在");
#endif
		return false;
	}
	if (m_pXml->GetNodeValueByPath("package/head/business/database/databasetype",false,dbtype,false) == NULL)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xml file no [package/head/business/database/databasetype] node");
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"节点[package/head/business/database/databasetype]不存在");
#endif
		return false;
	}

	strcpy(tmpchar,dbpwd.c_str());
	if (atoi(dbpwdflag.c_str()) ==1 )
	{
		int len = strlen(tmpchar);
		if (m_pEncrypt.UncryptString(tmpchar,len,DBPWDKEY)<0)
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"database user password uncrypt error");
#else
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"数据库密码解密出错");
#endif
			return false;
		}
	}
	else if (atoi(dbpwdflag.c_str()) == 2 )
	{
		int len = strlen(tmpchar);
		CBF_3Des des;
		char errmsg[200];
		if (!des.UnDes3Buffer(tmpchar,len,DBPWD3DESKEY,errmsg))
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"database user password uncrypt error %s",errmsg);
#else
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"数据库密码解密出错 %s",errmsg);
#endif
			return false;
		}
	}
	m_pSqlCmd = new CDbApi();
	if (m_pSqlCmd == NULL)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"new CDbApi error");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库连接实例化失败，分配空间失败");
#endif
		return false;
	}
	if (!m_pSqlCmd->Connect(dbname,dbuser,tmpchar,atoi(dbtype.c_str())))
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"connect database error %d %s",m_pSqlCmd->m_dbconn.m_nErrCode,m_pSqlCmd->m_dbconn.m_sErrInfo.c_str());
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"连接数据库错 %d %s",m_pSqlCmd->m_dbconn.m_nErrCode,m_pSqlCmd->m_dbconn.m_sErrInfo.c_str());
#endif
		return false;
	}
	return true;
}

bool CInitTrade::ReadOrgan()
{
	bool bret;
	//机构表
	S_MDB_ORG_INFO info;
	sprintf(m_sSql,"select ORG_CODE,ORG_NAME,ORG_TYPE,ORG_LEVEL,UP1_ORG,UP_ORG,CITY,SETTLE_ACCOUNT_NO from ORG_INFO order by ORG_CODE");
	
	try
	{
		if (!m_pSqlCmd->setCommandText(m_sSql))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setCommandText error <%s>",m_sSql);
			return false;
		}
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}
		
		bool bret = m_pSqlCmd->FetchNext();
		if ( !bret )
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Table ORG_INFO no data");
#else
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"机构表无数据");
#endif
			return false;
		}
		while (bret)
		{
			strcpy(info.sOrgCode,m_pSqlCmd->Field(0).asString());
			strcpy(info.sOrgName,m_pSqlCmd->Field(1).asString());
			strcpy(info.sOrgType,m_pSqlCmd->Field(2).asString());
			strcpy(info.sOrgLevel,m_pSqlCmd->Field(3).asString());
			strcpy(info.sUp1Org,m_pSqlCmd->Field(4).asString());
			strcpy(info.sUpOrg,m_pSqlCmd->Field(5).asString());
			strcpy(info.sOrgCity,m_pSqlCmd->Field(6).asString());
			strcpy(info.sAccNo,m_pSqlCmd->Field(7).asString());
			if (m_pMemDb->m_pDbPublic.m_pOrg.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Org [%s] repeat",info.sOrgCode);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"机构[%s]重复",info.sOrgCode);
#endif
				return false;
			}
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}
	return true;
}

bool CInitTrade::ReadOper()
{
	bool bret;
	//操作员信息
	S_MDB_OPER_INFO info;
	sprintf(m_sSql,"select OPER_CODE,OPER_NAME,OPER_PHONE,OPER_ORG,OPER_TYPE from OPER_INFO order by OPER_CODE");
	
	try
	{
		if (!m_pSqlCmd->setCommandText(m_sSql))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setCommandText error <%s>",m_sSql);
			return false;
		}
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}
		
		bret = m_pSqlCmd->FetchNext();
		if ( !bret )
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Table OPER_INFO no data");
#else
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"操作员表无数据");
#endif
			return false;
		}
		while (bret)
		{
			strcpy(info.sOperCode,m_pSqlCmd->Field(0).asString());
			strcpy(info.sOperName,m_pSqlCmd->Field(1).asString());
			strcpy(info.sOperPhone,m_pSqlCmd->Field(2).asString());
			strcpy(info.sOrgCode,m_pSqlCmd->Field(3).asString());
			strcpy(info.sOperType,m_pSqlCmd->Field(4).asString());
			
			if (m_pMemDb->m_pDbPublic.m_pOper.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"OperCode [%s] repeat",info.sOperCode);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"操作员[%s]重复",info.sOperCode);
#endif
				return false;
			}
//			S_MDB_OPER_INFO *po = m_pMemDb->m_pDbPublic.m_pOper.Select(info.sOperCode);
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}
	
//	strcpy(info.sOperCode,"songfree2");
//	S_MDB_OPER_INFO *po2 = m_pMemDb->m_pDbPublic.m_pOper.Select(info.sOperCode);
	
	return true;
}

bool CInitTrade::ReadPara()
{
	bool bret;
	S_MDB_SYSTEM_PARA info;
	sprintf(m_sSql,"select PARA_CODE,PARA_NAME,PARA_VALUE,VALUE_TYPE from SYSTEM_PARA order by PARA_CODE");
	
	try
	{
		if (!m_pSqlCmd->setCommandText(m_sSql))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setCommandText error <%s>",m_sSql);
			return false;
		}
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}
		
		bret = m_pSqlCmd->FetchNext();
		if ( !bret )
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Table SYSTEM_PARA no data");
#else
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"系统参数表无数据");
#endif
			return false;
		}
		while (bret)
		{
			strcpy(info.sParaCode, m_pSqlCmd->Field(0).asString());
			strcpy(info.sParaName, m_pSqlCmd->Field(1).asString());
			strcpy(info.sParaValue, m_pSqlCmd->Field(2).asString());
			strcpy(info.sValueType, m_pSqlCmd->Field(3).asString());
			if (m_pMemDb->m_pDbPublic.m_pSysPara.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"参数 [%s] repeat",info.sParaCode);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"参数 [%s]重复",info.sParaCode);
#endif
				return false;
			}
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}
	return true;
}

bool CInitTrade::ReadOperOnline()
{
	S_MDB_OPER_ONLINE info;
	sprintf(m_sSql,"select a.OPER_CODE,a.CHANNEL,a.LOGIN_IP,a.ONLINE_STATUS,a.ONLINE_TIME,a.NODEID,a.NODEPID,a.SVRID,a.SVRPID,a.GATEINDEX,a.TIMESTAMP from OPER_ONLINE a order by OPER_CODE");
	try
	{
		m_pSqlCmd->setCommandText(m_sSql);
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}
		bool bret = m_pSqlCmd->FetchNext();
		while (bret)
		{
			strcpy(info.sOperCode,m_pSqlCmd->Field(0).asString());
			strcpy(info.sTermType,m_pSqlCmd->Field(1).asString());
			strcpy(info.sLoginIp,m_pSqlCmd->Field(2).asString());
			strcpy(info.sOnlineStatus,m_pSqlCmd->Field(3).asString());

			info.nDrebId = m_pSqlCmd->Field(5).asLong();
			info.nDrebPid = m_pSqlCmd->Field(6).asLong();
			info.nSvrId = m_pSqlCmd->Field(7).asLong();
			info.nSvrPid = m_pSqlCmd->Field(8).asLong();
			info.nGateIndex = m_pSqlCmd->Field(9).asLong();
			info.nTimeStamp = m_pSqlCmd->Field(10).asLong();
			if (info.sOnlineStatus[0] != LOGIN_STATUS_OK)
			{
				bret = m_pSqlCmd->FetchNext();
				continue;
			}
			if (m_pMemDb->m_pDbPublic.m_pOperOnline.Insert(info)<0)
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"操作员 [%s] 在线 Insert 出错",info.sOperCode);
				return false;
			}
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}

	//读取操作员订阅表
	S_MDB_SUBSCRIBE subinfo;
	sprintf(m_sSql,"select OPER_CODE,INFO_TYPE from SUBSCRIBE");
	try
	{
		m_pSqlCmd->setCommandText(m_sSql);
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}
		bool bret = m_pSqlCmd->FetchNext();
		while (bret)
		{
			strcpy(subinfo.sOper,m_pSqlCmd->Field(0).asString());
			strcpy(subinfo.sSubcribeType,m_pSqlCmd->Field(1).asString());
			
			if (m_pMemDb->m_pDbTrader.m_pSubcribe.Insert(subinfo)<0)
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"操作员订阅 [%s] Insert 出错",subinfo.sOper);
				return false;
			}
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}

	
	return true;
}


bool CInitTrade::InitBaseInfo()
{
// 	CTbl_Variety    m_pVariety;//品种表
// 	CTbl_Contract   m_pContract;//合约表
// 	CTbl_Exch_Info  m_pExchange;//交易所表
// 	CTbl_Market     m_pMarket;//市场表
// 	CTbl_Fina_Prod  m_pProd;//产品表
// 	CTbl_Oper_TxAcc m_pOperTxAcc;//操作员对应资金账号表
// 	
// 	CTbl_Prod_Cust  m_pProdCust;//产品对应资金账号表
// 	CTbl_Oper_Prod  m_pOperProd;//操作员对应产品
// 	
// 	CTbl_Cust_Contract_Limit m_pCustContractLimit;//冷门合约开仓限制表
// 	
// 	CTbl_SettlePrice  m_pSettlePrice;//合约结算价表，只保存最新的
// 	CTbl_SpecDate     m_pSpecDate;//节假日配置
// 	CTbl_Option_Model m_pOptionModel;//期权合约价格模型表

	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"ReadVariety");
	if (!ReadVariety())
	{
		return false;
	}
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"ReadContract");
	if (!ReadContract())
	{
		return false;
	}
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"ReadExchange");
	if (!ReadExchange())
	{
		return false;
	}
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"ReadMarket");
	if (!ReadMarket())
	{
		return false;
	}
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"ReadProd");
	if (!ReadProd())
	{
		return false;
	}
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"ReadOperTxacc");
	if (!ReadOperTxacc())
	{
		return false;
	}
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"ReadProdTxacc");
	if (!ReadProdTxacc())
	{
		return false;
	}
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"ReadOperProd");
	if (!ReadOperProd())
	{
		return false;
	}
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"ReadContractLimit");
	if (!ReadContractLimit())
	{
		return false;
	}
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"ReadSettlePrice");
	if (!ReadSettlePrice())
	{
		return false;
	}
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"ReadSpecDate");
	if (!ReadSpecDate())
	{
		return false;
	}
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"ReadOptionModel");
	if (!ReadOptionModel())
	{
		return false;
	}
	return true;
}
bool CInitTrade::ReadVariety()
{
	bool bret;
	//品种表
	S_MDB_VARIETY_INFO info;
	sprintf(m_sSql,"select VARIETY_NO,VARIETY_CODE,VARIETY_NAME,EXCH_CODE,PROD_TYPE,MIN_CHG_PRICE,MAX_HAND,MIN_HAND,EXCH_UNIT,PRICE_UINT,DELI_DAYS,CURRENCY,BAIL_MODE from VARIETY_INFO order by VARIETY_NO");
	try
	{
		if (!m_pSqlCmd->setCommandText(m_sSql))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setCommandText error <%s>",m_sSql);
			return false;
		}
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}
		
		bret = m_pSqlCmd->FetchNext();
		if ( !bret )
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Table VARIETY_INFO no data");
#else
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"品种表无数据");
#endif
			return false;
		}
		while (bret)
		{
			info.nVarietyId = m_pSqlCmd->Field(0).asLong();
			strcpy(info.sVarietyCode,m_pSqlCmd->Field(1).asString());
			strcpy(info.sVarietyName,m_pSqlCmd->Field(2).asString());
			strcpy(info.sExcode,m_pSqlCmd->Field(3).asString());
			strcpy(info.sProd_type,m_pSqlCmd->Field(4).asString());
			info.dMinChgPrice = m_pSqlCmd->Field(5).asDouble();
			info.nMaxHand = m_pSqlCmd->Field(6).asLong();
			info.nMinHand = m_pSqlCmd->Field(7).asLong();
			info.dTxUnit = m_pSqlCmd->Field(8).asDouble();
			strcpy(info.sOrderUnit,m_pSqlCmd->Field(9).asString());
			info.nDeliDays = m_pSqlCmd->Field(11).asLong();
			strcpy(info.sCurrency,m_pSqlCmd->Field(11).asString());
			strcpy(info.sBailMode,m_pSqlCmd->Field(12).asString());
			
			if (m_pMemDb->m_pDbBaseinfo.m_pVariety.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"variety [%d %s] repeat",info.nVarietyId,info.sVarietyCode);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"品种 [%d %s]重复",info.nVarietyId,info.sVarietyCode);
#endif
				return false;
			}
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}
	return true;
}


bool CInitTrade::ReadContract()
{
	bool bret;
	//合约表
	S_MDB_CONTRACT_INFO info;
	S_MDB_VARIETY_INFO *pv=NULL;
	S_MDB_CONTRACT_INFO *pc=NULL;
	sprintf(m_sSql,"select a.CONTRACT_NO,a.CONTRACT_CODE,a.CONTRACT_NAME,a.EX_CODE,a.PROD_TYPE,a.PROD_CODE,a.PROD_NO,a.MIN_CHG_PRICE,a.IST0,a.ISBAIL,a.BAIL_RATE,a.UL_LIMIT,a.UPPER_LIMIT_VALUE,a.LOWER_LIMIT_VALUE,a.ACTIVE_FLAG,a.TX_STATUS,a.REF_PRICE,a.RCV_RATE,a.BUSI_MODE,a.MAX_HAND,a.MIN_HAND,a.EX_UNIT,a.PRICE_UNIT,a.ORDER_CMD,a.ORDER_ATTR,a.DUE_DATE,a.LAST_DATE,a.DELI_DAYS,a.IS_BIGMARGIN,a.ISCALL,a.VARIETY_NO from CONTRACT_INFO a,VARIETY_INFO b where a.VARIETY_NO=b.VARIETY_NO order by a.CONTRACT_NO");
	try
	{
		if (!m_pSqlCmd->setCommandText(m_sSql))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setCommandText error <%s>",m_sSql);
			return false;
		}
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}
		
		bret = m_pSqlCmd->FetchNext();
		if ( !bret )
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Table CONTRACT_INFO no data");
#else
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"合约表无数据");
#endif
			return false;
		}
		while (bret)
		{
			info.nContractId = m_pSqlCmd->Field(0).asLong();
			strcpy(info.sContractCode,m_pSqlCmd->Field(1).asString());
			strcpy(info.sContractName,m_pSqlCmd->Field(2).asString());
			strcpy(info.sExcode,m_pSqlCmd->Field(3).asString());
			strcpy(info.sProd_type,m_pSqlCmd->Field(4).asString());
			strcpy(info.sProd_code,m_pSqlCmd->Field(5).asString());
			info.nProdId = m_pSqlCmd->Field(6).asLong();
			info.dMinChgPrice = m_pSqlCmd->Field(7).asDouble();
			strcpy(info.sIsT0,m_pSqlCmd->Field(8).asString());
			strcpy(info.sIsMargin,m_pSqlCmd->Field(9).asString());
			info.dMarginRate = m_pSqlCmd->Field(10).asDouble();
			strcpy(info.sIsUpDownLimit,m_pSqlCmd->Field(11).asString());
			info.dUpRate = m_pSqlCmd->Field(12).asDouble();
			info.dDownRate = m_pSqlCmd->Field(13).asDouble();
			strcpy(info.sActiveFlag,m_pSqlCmd->Field(14).asString());
			strcpy(info.sStatus,m_pSqlCmd->Field(15).asString());
			info.dRefPrice = m_pSqlCmd->Field(16).asDouble();
			info.dSellGetRate = m_pSqlCmd->Field(17).asDouble();
			strcpy(info.sDeliMode,m_pSqlCmd->Field(18).asString());
			info.nMaxHand = m_pSqlCmd->Field(19).asLong();
			info.nMinHand = m_pSqlCmd->Field(20).asLong();
			info.dTxUnit = m_pSqlCmd->Field(21).asDouble();
			strcpy(info.sOrderUnit,m_pSqlCmd->Field(22).asString());
			strcpy(info.sOrderCmd,m_pSqlCmd->Field(23).asString());
			strcpy(info.sDoneAttr,m_pSqlCmd->Field(24).asString());
			strcpy(info.sDueDate,m_pSqlCmd->Field(25).asString());
			strcpy(info.sLastDate,m_pSqlCmd->Field(26).asString());
			info.nDeliDays = m_pSqlCmd->Field(27).asLong();
			strcpy(info.sIsBigMargin,m_pSqlCmd->Field(28).asString());
			strcpy(info.sIsOptionCall,m_pSqlCmd->Field(29).asString());
			info.nVariety = m_pSqlCmd->Field(30).asLong();
			

			//去品种表取币种
			switch (info.sProd_type[0])
			{
				case PROD_TYPE_FUTURES:
				case PROD_TYPE_SEC:
				case PROD_TYPE_SPOT:
					pv = m_pMemDb->m_pDbBaseinfo.m_pVariety.Select(info.nProdId);
					if (pv == NULL)
					{
						m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"合约[%d %s]的品种[%d]在品种表不存在",info.nContractId,info.sContractCode,info.nProdId);
						bret = m_pSqlCmd->FetchNext();
						continue;
					}
					strcpy(info.sCurrency,pv->sCurrency);
					break;
				case PROD_TYPE_OPTION:
					pc = m_pMemDb->m_pDbBaseinfo.m_pContract.Select(info.nProdId);
					if (pc == NULL)
					{
						m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"期权合约[%d %s]的标的合约[%d]在合约表不存在",info.nContractId,info.sContractCode,info.nProdId);
						bret = m_pSqlCmd->FetchNext();
						continue;
					}
					pv = m_pMemDb->m_pDbBaseinfo.m_pVariety.Select(pc->nProdId);
					if (pv == NULL)
					{
						m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"期权合约[%d %s]的品种[%d]在品种表不存在",info.nContractId,info.sContractCode,pc->nProdId);
						bret = m_pSqlCmd->FetchNext();
						continue;
					}
					strcpy(info.sCurrency,pv->sCurrency);
					break;
				default:
					info.sCurrency[0]= CURRENCY_CNY;
					info.sCurrency[1]=0;
					break;
			}
			if (m_pMemDb->m_pDbBaseinfo.m_pContract.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Contract [%d %s] repeat",info.nContractId,info.sContractCode);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"合约[%d %s]重复",info.nContractId,info.sContractCode);
#endif
				return false;
			}
			
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}
	

	return true;
}

bool CInitTrade::ReadExchange()
{
	bool bret;
	//交易所信息
	S_MDB_EXCH_INFO info;
	sprintf(m_sSql,"select EX_CODE,EX_NAME,EX_TYPE,EX_TRADEDATE,IS_SEAT_TRADE,COMM_PARA,CLEAR_DATE,CLEAR_STATUS,ONLY_CHGDATE from EX_INFO order by EX_CODE");
	
	try
	{
		if (!m_pSqlCmd->setCommandText(m_sSql))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setCommandText error <%s>",m_sSql);
			return false;
		}
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}
		
		bret = m_pSqlCmd->FetchNext();
		if ( !bret )
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Table EX_INFO no data");
#else
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"交易所表无数据");
#endif
			return false;
		}
		while (bret)
		{
			strcpy(info.sExCode,m_pSqlCmd->Field(0).asString());
			strcpy(info.sExName,m_pSqlCmd->Field(1).asString());
			strcpy(info.sMarket,m_pSqlCmd->Field(2).asString());
			strcpy(info.sTradeDate,m_pSqlCmd->Field(3).asString());
			strcpy(info.sIsSeatTrade,m_pSqlCmd->Field(4).asString());
			strcpy(info.sCommPara,m_pSqlCmd->Field(5).asString());
			strcpy(info.sClearDate,m_pSqlCmd->Field(6).asString());
			strcpy(info.sClearStatus,m_pSqlCmd->Field(7).asString());
			strcpy(info.sOnlyChgDate,m_pSqlCmd->Field(8).asString());
			if (m_pMemDb->m_pDbBaseinfo.m_pExchange.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Exchange [%s] repeat",info.sExCode);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"交易所[%s]重复",info.sExCode);
#endif
				return false;
			}
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}
	return true;
}


bool CInitTrade::ReadMarket()
{
	bool bret;
	//市场信息
	S_MDB_MARKET_INFO info;
	sprintf(m_sSql,"select MARKET_CODE,MARKET_NAME,TX_DATE,CLEAR_DATE,CLEAR_STATUS,ONLY_CHGDATE from EX_MARKET_INFO  order by MARKET_CODE");
	
	try
	{
		if (!m_pSqlCmd->setCommandText(m_sSql))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setCommandText error <%s>",m_sSql);
			return false;
		}
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}
		
		bret = m_pSqlCmd->FetchNext();
		if ( !bret )
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Table EX_MARKET_INFO no data");
#else
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"市场表表无数据");
#endif
			return false;
		}
		while (bret)
		{
			strcpy(info.sMarket,m_pSqlCmd->Field(0).asString());
			strcpy(info.sMarketName,m_pSqlCmd->Field(1).asString());
			strcpy(info.sTradeDate,m_pSqlCmd->Field(2).asString());
			strcpy(info.sClearDate,m_pSqlCmd->Field(3).asString());
			strcpy(info.sClearStatus,m_pSqlCmd->Field(4).asString());
			strcpy(info.sOnlyChgDate,m_pSqlCmd->Field(5).asString());
		
			if (m_pMemDb->m_pDbBaseinfo.m_pMarket.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"EX_MARKET_INFO [%s] repeat",info.sMarket);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"市场表 [%s]重复",info.sMarket);
#endif
				return false;
			}
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}
	return true;
}

bool CInitTrade::ReadProd()
{
	bool bret;
	//产品信息
	S_MDB_FINA_PROD_INFO info;
	sprintf(m_sSql,"select a.FINA_PROD_NO,a.FINA_PROD_NAME,a.LINKMAN,a.PHONE,a.ADDR,a.ORG_CODE,a.PUBLISH_UNIT from FINA_PROD_INFO a ");
	
	try
	{
		if (!m_pSqlCmd->setCommandText(m_sSql))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setCommandText error <%s>",m_sSql);
			return false;
		}
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}
		
		bret = m_pSqlCmd->FetchNext();
// 		if ( !bret )
// 		{
// #ifdef _ENGLISH_
// 			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Table FINA_PROD_INFO no data");
// #else
// 			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"产品表无数据");
// #endif
// 			return false;
// 		}
		while (bret)
		{
			strcpy(info.sFinaProdCode,m_pSqlCmd->Field(0).asString());
			strcpy(info.sFinaProdName,m_pSqlCmd->Field(1).asString());
			strcpy(info.sLinkMan,m_pSqlCmd->Field(2).asString());
			strcpy(info.sPhone,m_pSqlCmd->Field(3).asString());
			strcpy(info.sAddr,m_pSqlCmd->Field(4).asString());
			strcpy(info.sOrgCode,m_pSqlCmd->Field(5).asString());
			info.nPublishUit = m_pSqlCmd->Field(6).asLong();
			if (m_pMemDb->m_pDbBaseinfo.m_pProd.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"FINA_PROD_INFO [%s] repeat",info.sFinaProdCode);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"产品表 [%s]重复",info.sFinaProdCode);
#endif
				return false;
			}
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}
	return true;
}

bool CInitTrade::ReadOperTxacc()
{
	bool bret;
	S_MDB_OPER_CUST_INFO info;
	sprintf(m_sSql,"select a.OPER_CODE,a.TX_ACC from OPER_CUST_LINK a ");
	
	try
	{
		if (!m_pSqlCmd->setCommandText(m_sSql))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setCommandText error <%s>",m_sSql);
			return false;
		}
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}
		
		bret = m_pSqlCmd->FetchNext();
		while (bret)
		{
			strcpy(info.sOperCode,m_pSqlCmd->Field(0).asString());
			strcpy(info.sTxAcc,m_pSqlCmd->Field(1).asString());
			if (m_pMemDb->m_pDbBaseinfo.m_pOperTxAcc.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"OPER_CUST_LINK [%s] repeat",info.sOperCode);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"交易员资金账号对应表 [%s]重复",info.sOperCode);
#endif
				return false;
			}
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}
	return true;
}

bool CInitTrade::ReadProdTxacc()
{
	bool bret;
	S_MDB_PROD_CUST info;
	sprintf(m_sSql,"select a.FINA_PROD_NO,a.CUST_NO,a.TX_ACC from FINA_PROD_CUST a ");
	
	try
	{
		if (!m_pSqlCmd->setCommandText(m_sSql))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setCommandText error <%s>",m_sSql);
			return false;
		}
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}
		
		bret = m_pSqlCmd->FetchNext();
		while (bret)
		{
			strcpy(info.sProdNo,m_pSqlCmd->Field(0).asString());
			strcpy(info.sCustNo,m_pSqlCmd->Field(1).asString());
			strcpy(info.sTxAcc,m_pSqlCmd->Field(2).asString());
			if (m_pMemDb->m_pDbBaseinfo.m_pProdCust.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"FINA_PROD_CUST [%s %s] repeat",info.sProdNo,info.sTxAcc);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"产品资金账号对应表 [%s %s]重复",info.sProdNo,info.sTxAcc);
#endif
				return false;
			}
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}
	return true;	
}

bool CInitTrade::ReadOperProd()
{
	bool bret;
	S_MDB_OPER_PROD info;
	sprintf(m_sSql,"select a.OPER_CODE,a.FINA_PROD_ID from FINA_PROD_OPER a ");
	
	try
	{
		if (!m_pSqlCmd->setCommandText(m_sSql))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setCommandText error <%s>",m_sSql);
			return false;
		}
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}
		
		bret = m_pSqlCmd->FetchNext();
		while (bret)
		{
			strcpy(info.sOper,m_pSqlCmd->Field(0).asString());
			strcpy(info.sProdNo,m_pSqlCmd->Field(1).asString());
			if (m_pMemDb->m_pDbBaseinfo.m_pOperProd.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"FINA_PROD_OPER [%s %s] repeat",info.sOper,info.sProdNo);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"操作员产品对应表 [%s %s]重复",info.sOper,info.sProdNo);
#endif
				return false;
			}
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}
	return true;	
}

bool CInitTrade::ReadContractLimit()
{
	bool bret;
	S_MDB_CONTRACT_LIMIT info;
	sprintf(m_sSql,"select a.TX_ACC,a.CONTRACT_NO,a.FORCERATE from NONACTIVE_CONTRACT_SET a ");
	
	try
	{
		if (!m_pSqlCmd->setCommandText(m_sSql))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setCommandText error <%s>",m_sSql);
			return false;
		}
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}
		
		bret = m_pSqlCmd->FetchNext();
		while (bret)
		{
			strcpy(info.sTxAcc,m_pSqlCmd->Field(0).asString());
			info.nContractNo = m_pSqlCmd->Field(1).asLong();
			info.dForceRate = m_pSqlCmd->Field(2).asDouble();
			if (m_pMemDb->m_pDbBaseinfo.m_pCustContractLimit.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"NONACTIVE_CONTRACT_SET [%s %d] repeat",info.sTxAcc,info.nContractNo);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"冷门合约开仓限制表 [%s %d]重复",info.sTxAcc,info.nContractNo);
#endif
				return false;
			}
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}
	return true;
}

bool CInitTrade::ReadSettlePrice()
{
	bool bret;
	//根据交易所表或市场表，取结算日期这天的结算价
	S_MDB_CONTRACT_SETTLE_PRICE info;
	//select a.EXCH_DATE,a.CONTRACT_NO,a.CONTRACT_CODE,a.SETTLE_PRICE,a.HOLD_POSI,a.CLOSE_PRICE,a.PRICE_CHANGE,a.SIMPLE_RETURN,a.LOG_RETURN,a.HIGH,a.LOW,a.OPEN from CONTRACT_SETTLE_PRICE a,CONTRACT_INFO b ,EX_INFO c where a.CONTRACT_NO = b.CONTRACT_NO and b.EX_CODE=c.EX_CODE and c.CLEAR_DATE=a.EXCH_DATE
	sprintf(m_sSql,"select a.EXCH_DATE,a.CONTRACT_NO,a.CONTRACT_CODE,a.SETTLE_PRICE,a.HOLD_POSI,a.CLOSE_PRICE,a.PRICE_CHANGE,a.SIMPLE_RETURN,a.LOG_RETURN,a.HIGH,a.LOW,a.OPEN from CONTRACT_SETTLE_PRICE a,CONTRACT_INFO b ,EX_INFO c where a.CONTRACT_NO = b.CONTRACT_NO and b.EX_CODE=c.EX_CODE and c.CLEAR_DATE=a.EXCH_DATE");
	
	try
	{
		if (!m_pSqlCmd->setCommandText(m_sSql))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setCommandText error <%s>",m_sSql);
			return false;
		}
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}
		bret = m_pSqlCmd->FetchNext();
		if ( !bret )
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"sql[%s] Table CONTRACT_SETTLE_PRICE no data",m_sSql);
#else
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"合约结算价表无数据 sql[%s] ",m_sSql);
#endif
			return false;
		}
		while (bret)
		{
			strcpy(info.sClearDate,m_pSqlCmd->Field(0).asString());
			info.nContractNo = m_pSqlCmd->Field(1).asLong();
			strcpy(info.sContractCode,m_pSqlCmd->Field(2).asString());
			info.dSettlePrice = m_pSqlCmd->Field(3).asDouble();
			info.nHoldPosi = m_pSqlCmd->Field(4).asLong();
			info.dClosePrice = m_pSqlCmd->Field(5).asDouble();
			info.dPriceChg = m_pSqlCmd->Field(6).asDouble();
			info.dSIMPLE_RETURN = m_pSqlCmd->Field(7).asDouble();
			info.dLOG_RETURN = m_pSqlCmd->Field(8).asDouble();
			info.dHighPrice = m_pSqlCmd->Field(9).asDouble();
			info.dLowPrice = m_pSqlCmd->Field(10).asDouble();
			info.dOpenPrice = m_pSqlCmd->Field(11).asDouble();

			if (m_pMemDb->m_pDbBaseinfo.m_pSettlePrice.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"CONTRACT_SETTLE_PRICE [%d %s] repeat",info.nContractNo,info.sClearDate);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"合约结算价表 [%d %s]重复",info.nContractNo,info.sClearDate);
#endif
				return false;
			}
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}
	return true;
}

bool CInitTrade::ReadSpecDate()
{
	bool bret;
	CBF_Date_Time pdate;
	S_MDB_SPECDATE info;
	sprintf(m_sSql,"select SPEC_DATE,MARKET_CODE,EX_CODE,DATE_TYPE from SPEC_DATE where SPEC_DATE >=:1  ");
	
	try
	{
		if (!m_pSqlCmd->setCommandText(m_sSql))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setCommandText error <%s>",m_sSql);
			return false;
		}
		m_pSqlCmd->Param(1).setAsString() = pdate.ToStringD8().c_str();
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}
		
		bret = m_pSqlCmd->FetchNext();
		if ( !bret )
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"Table SPEC_DATE no data");
#else
			m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"节假日表无数据 ");
#endif
			return true;
		}
		while (bret)
		{
			strcpy(info.sSpecDate,m_pSqlCmd->Field(0).asString());
			strcpy(info.sMarket,m_pSqlCmd->Field(1).asString());
			strcpy(info.sExchCode,m_pSqlCmd->Field(2).asString());
			strcpy(info.sDateType,m_pSqlCmd->Field(3).asString());

			if (m_pMemDb->m_pDbBaseinfo.m_pSpecDate.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"SPEC_DATE [%s] repeat",info.sSpecDate);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"节假日配置表 [%s]重复",info.sSpecDate);
#endif
				return false;
			}
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}
	return true;
}

bool CInitTrade::InitCust()
{
// 	CTbl_Cust         m_pCustom;//客户信息表
// 	CTbl_Cust_TX_Acc  m_pCustFundAcc;//资金账号信息表
	//客户信息表
	if (!ReadCustom())
	{
		return false;
	}
	//资金账号信息表
	if (!ReadFundAcc())
	{
		return false;
	}
	return true;
}

bool CInitTrade::ReadCustom()
{
	bool bret;
	
	S_MDB_CUSTINFO info;
	sprintf(m_sSql,"select CUST_NO,CUST_NAME,CUST_TYPE,CUST_CERT,CUST_CERT_NO,CUST_PHONE,TXCHANNLE,PKI_CERT_SERIAL,PKI_CERT_DATE,CUST_STATUS,LAST_RISK_LEVEL from CUST_BASIC_INFO order by CUST_NO");
	
	try
	{
		if (!m_pSqlCmd->setCommandText(m_sSql))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setCommandText error <%s>",m_sSql);
			return false;
		}
		
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}
		
		bret = m_pSqlCmd->FetchNext();
		if ( !bret )
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Table CUST_BASIC_INFO no data");
#else
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"客户表CUST_BASIC_INFO无数据 ");
#endif
			return false;
		}
		while (bret)
		{
			strcpy(info.sCustNo,m_pSqlCmd->Field(0).asString());
			strcpy(info.sCustName,m_pSqlCmd->Field(1).asString());
			strcpy(info.sCustType,m_pSqlCmd->Field(2).asString());
			strcpy(info.sCertType,m_pSqlCmd->Field(3).asString());
			if (strlen(info.sCertType)<1) //若没有证件类型，默认身份证
			{
				info.sCertType[0] = CERT_TYPE_IDCARD;
				info.sCertType[1] =0; 
			}
			strcpy(info.sCertNo,m_pSqlCmd->Field(4).asString());
			if (strlen(info.sCertNo)<1)//若没有证件号，用客户号
			{
				strcpy(info.sCertNo,info.sCustNo);
			}
			strcpy(info.sMobile,m_pSqlCmd->Field(5).asString());
			if (strlen(info.sMobile)<1)//若没有手机号，用客户号
			{
				strcpy(info.sMobile,info.sCustNo);
			}
			strcpy(info.sChannleOpen,m_pSqlCmd->Field(6).asString());
			strcpy(info.sPkiCertNo,m_pSqlCmd->Field(7).asString());
			strcpy(info.sPkiCertDate,m_pSqlCmd->Field(8).asString());
			strcpy(info.sStatus,m_pSqlCmd->Field(9).asString());
			info.dLastRisk = m_pSqlCmd->Field(10).asDouble();

			if (m_pMemDb->m_pDbCust.m_pCustom.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"custom [%s] repeat",info.sCustNo);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"客户表 [%s]重复",info.sCustNo);
#endif
				return false;
			}
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}
	return true;
}

bool CInitTrade::ReadFundAcc()
{
	bool bret;
	int rid;
	S_MDB_CUST_FUNDACC_INFO info;
	bzero(&info,sizeof(S_MDB_CUST_FUNDACC_INFO));
	sprintf(m_sSql,"select TX_ACC,CUST_NO,ACC_TYPE,ACC_SIMPLENAME,SIGN_ORG,MANAGER_ORG,FUTURES_FLAG,ACC_LEVEL,EX_FEE,MEMBER_FEE,BROKERAGE_CODE,CURRENCY,FS_FEE_MODEL,ACC_NAME from CUST_FUNDACC_INFO where ACC_STATUS='0' ");
	
	try
	{
		if (!m_pSqlCmd->setCommandText(m_sSql))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setCommandText error <%s>",m_sSql);
			return false;
		}
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}
		
		bret = m_pSqlCmd->FetchNext();
		if ( !bret )
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Table CUST_FUNDACC_INFO no data");
#else
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"资金账号CUST_FUNDACC_INFO表无数据 ");
#endif
			return false;
		}
		while (bret)
		{
			strcpy(info.sTxAcc,m_pSqlCmd->Field(0).asString());
			strcpy(info.sCustNo,m_pSqlCmd->Field(1).asString());
			strcpy(info.sAcctype,m_pSqlCmd->Field(2).asString());
			strcpy(info.sCustSimpleName,m_pSqlCmd->Field(3).asString());
			strcpy(info.sSignOrg,m_pSqlCmd->Field(4).asString());
			strcpy(info.sMgrOrg,m_pSqlCmd->Field(5).asString());
			strcpy(info.sFutureFlag,m_pSqlCmd->Field(6).asString());
			strcpy(info.sAccLevel,m_pSqlCmd->Field(7).asString());
			strcpy(info.sExchFareModel,m_pSqlCmd->Field(8).asString());
			strcpy(info.sMemberFareModel,m_pSqlCmd->Field(9).asString());
			info.nSysId = m_pSqlCmd->Field(10).asLong();
			strcpy(info.sCurrency,m_pSqlCmd->Field(11).asString());
			strcpy(info.sFsFeeModel,m_pSqlCmd->Field(12).asString());
			strcpy(info.sCustName,m_pSqlCmd->Field(13).asString());
			
			rid = m_pMemDb->m_pDbCust.m_pCustFundAcc.Insert(info);
			if (rid <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"CUST_FUNDACC_INFO [%s] repeat",info.sTxAcc);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"资金账号CUST_FUNDACC_INFO表 [%s]重复",info.sTxAcc);
#endif
				return false;
			}
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}
	return true;
}

bool CInitTrade::InitSet()
{
// 	CTbl_WarnLevel           m_pWarnLevel;//预警级别表
// 	CTbl_RiskContract_Set    m_pRiskContractSet;//风险合约设置表
// 	
// 	CTbl_Prod_Monitor        m_pProdFundMonitor;//产品资金监控表
// 	CTbl_Prod_Monitor        m_pProdPosiMonitor;//产品持仓监控表
// 	
// 	CTbl_Templet             m_pFundTemplet;//客户资金模板信息
// 	CTbl_Templet             m_pPosiTemplet;//客户持仓模板信息
// 	CTbl_Fund_Templet        m_pFundTempletDetail;//客户资金模板配置表
// 	CTbl_Position_Templet    m_pPosiTempletDetail;//客户持仓模板配置表
// 	CTbl_Cust_Templet        m_pCustTemplet;//资金账号模板对应关系表
// 	
// 	CTbl_Fee_Templet_Info    m_pFeeTemplet;//费率模板信息
// 	CTbl_Fee_Templet_Detail  m_pFeeTempletDetail;//费率模板配置
//	CTbl_Cust_Fee_Detail     m_pCustFeeDetail;//客户特殊费率设置表

	//CTbl_Brokerage           m_pBrokerage;//经纪配置信息  
	//CTbl_Brokerage_Exchange  m_pBrokerageExchange;//纪纪对应交易所
	//CTblPordOrg_Templet      m_pPordTemplet;//产品模板对应关系
	//CTblPordOrg_Templet      m_pOrgTemplet;//机构模板对应关系

	//预警级别表
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"ReadWarnLevel");
	if (!ReadWarnLevel())
	{
		return false;
	}
	//风险合约设置表
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"ReadRiskContractSet");
	if (!ReadRiskContractSet())
	{
		return false;
	}
	//产品资金监控表
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"ReadProdFundMonitor");
	if (!ReadProdFundMonitor())
	{
		return false;
	}
	//产品持仓监控表
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"ReadProdPosiMonitor");
	if (!ReadProdPosiMonitor())
	{
		return false;
	}
	//客户资金模板信息
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"ReadFundTemplet");
	if (!ReadFundTemplet())
	{
		return false;
	}
	//客户持仓模板信息
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"ReadPosiTemplet");
	if (!ReadPosiTemplet())
	{
		return false;
	}
	//客户资金模板配置表
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"ReadFundTempletDetail");
	if (!ReadFundTempletDetail())
	{
		return false;
	}
	//客户持仓模板配置表
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"ReadPosiTempletDetail");
	if (!ReadPosiTempletDetail())
	{
		return false;
	}
	//资金账号模板对应关系表
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"ReadTxaccTemplet");
	if (!ReadTxaccTemplet())
	{
		return false;
	}
	//费率模板信息
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"ReadFeeTemplet");
	if (!ReadFeeTemplet())
	{
		return false;
	}
	//费率模板配置
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"ReadFeeTempletDetail");
	if (!ReadFeeTempletDetail())
	{
		return false;
	}
	//客户特殊费率设置表
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"ReadCustFeeDetail");
	if (!ReadCustFeeDetail())
	{
		return false;
	}
	//经纪及交易所设置表
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"ReadBrokerageInfo");
	if (!ReadBrokerageInfo())
	{
		return false;
	}
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"ReadProdOrgTemplet");
	if (!ReadProdOrgTemplet())
	{
		return false;
	}
	return true;
}
bool CInitTrade::ReadProdOrgTemplet()
{
	bool bret;

	S_MDB_PRODORG_TEMPLET_RELA info;
	sprintf(m_sSql,"select ORG_CODE,FUND_TEMPLET_ID,POSI_TEMPLET_ID from  ORGAN_TEMPLET");
	try
	{
		if (!m_pSqlCmd->setCommandText(m_sSql))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setCommandText error <%s>",m_sSql);
			return false;
		}
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}

		bret = m_pSqlCmd->FetchNext();
		if ( !bret )
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"Table ORGAN_TEMPLET no data");
#else
			m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"机构模板表ORGAN_TEMPLET 表无数据 ");
#endif
			//return false;
		}
		while (bret)
		{
			
			strcpy(info.sProdOrg,m_pSqlCmd->Field(0).asString());
			strcpy(info.sFundTempletId,m_pSqlCmd->Field(1).asString());
			strcpy(info.sPosiTempletId,m_pSqlCmd->Field(2).asString());
			if (m_pMemDb->m_pDbSet.m_pOrgTemplet.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"ORGAN_TEMPLET [%s] repeat",info.sProdOrg);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"机构模板表 ORGAN_TEMPLET表 [%s]重复",info.sProdOrg);
#endif
				return false;
			}
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}
	sprintf(m_sSql,"select FINA_PROD_NO,FUND_TEMPLET_ID,POSI_TEMPLET_ID from  PROD_TEMPLET");
	try
	{
		if (!m_pSqlCmd->setCommandText(m_sSql))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setCommandText error <%s>",m_sSql);
			return false;
		}
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}

		bret = m_pSqlCmd->FetchNext();
		if ( !bret )
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Table PROD_TEMPLET no data");
#else
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"产品模板表 PROD_TEMPLET 表无数据 ");
#endif
			//return false;
		}
		while (bret)
		{

			strcpy(info.sProdOrg,m_pSqlCmd->Field(0).asString());
			strcpy(info.sFundTempletId,m_pSqlCmd->Field(1).asString());
			strcpy(info.sPosiTempletId,m_pSqlCmd->Field(2).asString());
			if (m_pMemDb->m_pDbSet.m_pProdTemplet.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"PROD_TEMPLET [%s] repeat",info.sProdOrg);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"产品模板表 PROD_TEMPLET表 [%s]重复",info.sProdOrg);
#endif
				return false;
			}
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}
	//读汇率配置表
	S_MDB_CURRENCY_RATE rate;
	sprintf(m_sSql,"select CURRENCY,RATE from  CURRENCY_RATE");
	try
	{
		if (!m_pSqlCmd->setCommandText(m_sSql))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setCommandText error <%s>",m_sSql);
			return false;
		}
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}

		bret = m_pSqlCmd->FetchNext();
		if ( !bret )
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Table CURRENCY_RATE no data");
#else
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"汇率配置表 CURRENCY_RATE 表无数据 ");
#endif
			//return false;
		}
		while (bret)
		{

			strcpy(rate.sCurrency,m_pSqlCmd->Field(0).asString());
			rate.dRate =m_pSqlCmd->Field(1).asDouble();
			
			if (m_pMemDb->m_pDbSet.m_pCurrencyRate.Insert(rate) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"CURRENCY_RATE [%s] repeat",rate.sCurrency);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"汇率配置表 CURRENCY_RATE [%s]重复",rate.sCurrency);
#endif
				return false;
			}
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}
	return true;
}

bool CInitTrade::ReadWarnLevel()
{
	bool bret;
	
	S_MDB_WARN_LEVEL_INFO info;
	sprintf(m_sSql,"select WARN_NO,WARN_NAME,WARN_COLOR,WARN_SOUND,WARN_SEP from RISK_EARLY_WARNING_LEVEL order by WARN_NO");
	
	try
	{
		if (!m_pSqlCmd->setCommandText(m_sSql))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setCommandText error <%s>",m_sSql);
			return false;
		}
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}
		
		bret = m_pSqlCmd->FetchNext();
		if ( !bret )
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Table RISK_EARLY_WARNING_LEVEL no data");
#else
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"预警级别RISK_EARLY_WARNING_LEVEL 表无数据 ");
#endif
			return false;
		}
		while (bret)
		{
			info.nWarnLevel = m_pSqlCmd->Field(0).asLong();
			strcpy(info.sWarnName,m_pSqlCmd->Field(1).asString());
			strcpy(info.sWarnColor,m_pSqlCmd->Field(2).asString());
			strcpy(info.sWarnSound,m_pSqlCmd->Field(3).asString());
			info.nWarnSep= m_pSqlCmd->Field(4).asLong();
		
			if (m_pMemDb->m_pDbSet.m_pWarnLevel.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"RISK_EARLY_WARNING_LEVEL [%d] repeat",info.nWarnLevel);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"预警级别RISK_EARLY_WARNING_LEVEL表 [%d]重复",info.nWarnLevel);
#endif
				return false;
			}
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}
	return true;
}

bool CInitTrade::ReadRiskContractSet()
{
	bool bret;
	
	S_MDB_RISKCONTRACT_SET info;
	sprintf(m_sSql,"select VARIETY_NO,CONTRACT_NO,CONTRACT_CODE,WARN_ITEM,RISK_CONDITION,WARN_VALUE from RISK_CONTRACT_SET ");
	
	try
	{
		if (!m_pSqlCmd->setCommandText(m_sSql))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setCommandText error <%s>",m_sSql);
			return false;
		}
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}
		
		bret = m_pSqlCmd->FetchNext();
		while (bret)
		{
			info.nVarietyNo = m_pSqlCmd->Field(0).asLong();
			info.nContractNo = m_pSqlCmd->Field(1).asLong();
			strcpy(info.sContractCode,m_pSqlCmd->Field(2).asString());
			strcpy(info.sWarnItem,m_pSqlCmd->Field(3).asString());
			strcpy(info.sCondition,m_pSqlCmd->Field(4).asString());
			info.dValue = m_pSqlCmd->Field(5).asDouble();
			
			
			if (m_pMemDb->m_pDbSet.m_pRiskContractSet.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"RISK_CONTRACT_SET [%d] repeat",info.nVarietyNo);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"风险合约设置RISK_CONTRACT_SET表 [%d]重复",info.nVarietyNo);
#endif
				return false;
			}
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}
	return true;
}

bool CInitTrade::ReadProdFundMonitor()
{
	bool bret;
	
	S_MDB_PROD_MONITOR info;
	sprintf(m_sSql,"select WARN_NO,FUND_MODEL,FUND_CONDITION,FUND_VALUE,TRADE_RIGHT,TEMPLET_ID from PROD_FUND_MONITOR ");
	try
	{
		if (!m_pSqlCmd->setCommandText(m_sSql))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setCommandText error <%s>",m_sSql);
			return false;
		}
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}
		
		bret = m_pSqlCmd->FetchNext();
		while (bret)
		{
			info.nWarnNo = m_pSqlCmd->Field(0).asLong();
			strcpy(info.sModel,m_pSqlCmd->Field(1).asString());
			strcpy(info.sCondition,m_pSqlCmd->Field(2).asString());
			info.dValue = m_pSqlCmd->Field(3).asDouble();
			strcpy(info.sRight,m_pSqlCmd->Field(4).asString());
			strcpy(info.sTempletId,m_pSqlCmd->Field(5).asString());
			if (m_pMemDb->m_pDbSet.m_pProdFundMonitor.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"PROD_FUND_MONITOR [%d] repeat",info.nWarnNo);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"产品资金监控PROD_FUND_MONITOR [%d]重复",info.nWarnNo);
#endif
				return false;
			}
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}
	return true;
}

bool CInitTrade::ReadProdPosiMonitor()
{
	bool bret;
	
	S_MDB_PROD_MONITOR info;
	sprintf(m_sSql,"select WARN_NO,POSI_MODEL,POSI_CONDITION,POSI_VALUE,TRADE_RIGHT,TEMPLET_ID from PROD_POSI_MONITOR ");
	try
	{
		if (!m_pSqlCmd->setCommandText(m_sSql))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setCommandText error <%s>",m_sSql);
			return false;
		}
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}
		
		bret = m_pSqlCmd->FetchNext();
		while (bret)
		{
			info.nWarnNo = m_pSqlCmd->Field(0).asLong();
			strcpy(info.sModel,m_pSqlCmd->Field(1).asString());
			strcpy(info.sCondition,m_pSqlCmd->Field(2).asString());
			info.dValue = m_pSqlCmd->Field(3).asDouble();
			strcpy(info.sRight,m_pSqlCmd->Field(4).asString());
			strcpy(info.sTempletId,m_pSqlCmd->Field(5).asString());
			if (m_pMemDb->m_pDbSet.m_pProdPosiMonitor.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"PROD_POSI_MONITOR [%d] repeat",info.nWarnNo);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"产品持仓监控PROD_POSI_MONITOR [%d]重复",info.nWarnNo);
#endif
				return false;
			}
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}
	return true;
}

bool CInitTrade::ReadFundTemplet()
{
	bool bret;
	
	S_MDB_TEMPLET_INFO info;
	sprintf(m_sSql,"select TEMPLET_ID,TEMPLET_NAME from CUST_FUND_MONITOR_MODEL ");
	try
	{
		if (!m_pSqlCmd->setCommandText(m_sSql))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setCommandText error <%s>",m_sSql);
			return false;
		}
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}
		
		bret = m_pSqlCmd->FetchNext();
		while (bret)
		{
			strcpy(info.sTempletId ,m_pSqlCmd->Field(0).asString());
			strcpy(info.sTempletName,m_pSqlCmd->Field(1).asString());
			
			if (m_pMemDb->m_pDbSet.m_pFundTemplet.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"CUST_FUND_MONITOR_MODEL [%s] repeat",info.sTempletId);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"资金模板表CUST_FUND_MONITOR_MODEL [%s]重复",info.sTempletId);
#endif
				return false;
			}
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}
	return true;
}

bool CInitTrade::ReadPosiTemplet()
{
	bool bret;
	
	S_MDB_TEMPLET_INFO info;
	sprintf(m_sSql,"select TEMPLET_ID,TEMPLET_NAME from CUST_POSI_MONITOR_MODEL ");
	try
	{
		if (!m_pSqlCmd->setCommandText(m_sSql))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setCommandText error <%s>",m_sSql);
			return false;
		}
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}
		
		bret = m_pSqlCmd->FetchNext();
		while (bret)
		{
			strcpy(info.sTempletId ,m_pSqlCmd->Field(0).asString());
			strcpy(info.sTempletName,m_pSqlCmd->Field(1).asString());
			
			if (m_pMemDb->m_pDbSet.m_pPosiTemplet.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"CUST_POSI_MONITOR_MODEL [%s] repeat",info.sTempletId);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"持仓模板表 CUST_POSI_MONITOR_MODEL [%s]重复",info.sTempletId);
#endif
				return false;
			}
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}
	return true;
}

bool CInitTrade::ReadFundTempletDetail()
{
	bool bret;
	
	S_MDB_FUND_TEMPLET info;
	sprintf(m_sSql,"select a.TEMPLET_ID,b.TEMPLET_NAME,a.WARN_NO,a.FUND_MODEL,a.FUND_CONDITION,a.FUND_VALUE,a.TRADE_RIGHT from CUST_FUND_MODEL_SET a,CUST_FUND_MONITOR_MODEL b  where a.TEMPLET_ID=b.TEMPLET_ID ");
	try
	{
		if (!m_pSqlCmd->setCommandText(m_sSql))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setCommandText error <%s>",m_sSql);
			return false;
		}
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}
		
		bret = m_pSqlCmd->FetchNext();
		while (bret)
		{
			strcpy(info.sFundTempletId ,m_pSqlCmd->Field(0).asString());
			strcpy(info.sFundTempletName,m_pSqlCmd->Field(1).asString());
			info.nWarnNo = m_pSqlCmd->Field(2).asLong();
			strcpy(info.sFundModel,m_pSqlCmd->Field(3).asString());
			strcpy(info.sCondition,m_pSqlCmd->Field(4).asString());
			info.dValue = m_pSqlCmd->Field(5).asDouble();
			strcpy(info.sRight,m_pSqlCmd->Field(6).asString());
			if (m_pMemDb->m_pDbSet.m_pFundTempletDetail.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"CUST_FUND_MODEL_SET [%s] repeat",info.sFundTempletId);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"客户资金模板设置 CUST_FUND_MODEL_SET [%s]重复",info.sFundTempletId);
#endif
				return false;
			}
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}
	return true;
}

bool CInitTrade::ReadPosiTempletDetail()
{
	bool bret;
	
	S_MDB_POSITION_TEMPLET info;
	sprintf(m_sSql,"select a.TEMPLET_ID,a.WARN_NO,a.EX_CODE,a.PROD_TYPE,a.VARIETY_NO,a.VARIETY_CODE,a.OPTION_DESTNO,a.OPTION_DESTCODE,a.CONTRACT_NO,a.CONTRACT_CODE,a.POSI_MODEL,a.POSI_CONDITION,a.POSI_VALUE,a.TRADE_RIGHT from CUST_POSI_MODEL_SET a   ");
	try
	{
		if (!m_pSqlCmd->setCommandText(m_sSql))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setCommandText error <%s>",m_sSql);
			return false;
		}
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}
		
		bret = m_pSqlCmd->FetchNext();
		while (bret)
		{
			strcpy(info.sPosiTempletId ,m_pSqlCmd->Field(0).asString());
			info.nWarnNo = m_pSqlCmd->Field(1).asLong();
			strcpy(info.sExchCode,m_pSqlCmd->Field(2).asString());
			strcpy(info.sProdType,m_pSqlCmd->Field(3).asString());
			info.nVarietyNo = m_pSqlCmd->Field(4).asLong();
			strcpy(info.sVarietyCode,m_pSqlCmd->Field(5).asString());
			info.nOption_desno = m_pSqlCmd->Field(6).asLong();
			strcpy(info.sOption_destcode,m_pSqlCmd->Field(7).asString());
			info.nContractNo = m_pSqlCmd->Field(8).asLong();
			strcpy(info.sContractCode,m_pSqlCmd->Field(9).asString());
			strcpy(info.sPosiModel,m_pSqlCmd->Field(10).asString());
			strcpy(info.sCondition,m_pSqlCmd->Field(11).asString());
			info.dValue = m_pSqlCmd->Field(12).asDouble();
			strcpy(info.sRight,m_pSqlCmd->Field(13).asString());
			if (m_pMemDb->m_pDbSet.m_pPosiTempletDetail.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"CUST_POSI_MODEL_SET [%s] repeat",info.sPosiTempletId);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"客户持仓模板设置 CUST_POSI_MODEL_SET [%s]重复",info.sPosiTempletId);
#endif
				return false;
			}
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}
	return true;
}

bool CInitTrade::ReadTxaccTemplet()
{
	bool bret;
	
	S_MDB_CUST_TEMPLET_RELA info;
	sprintf(m_sSql,"select a.TX_ACC,a.FUND_TEMPLET_ID,a.POSI_TEMPLET_ID from CUST_TEMPLET a ");
	try
	{
		if (!m_pSqlCmd->setCommandText(m_sSql))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setCommandText error <%s>",m_sSql);
			return false;
		}
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}
		
		bret = m_pSqlCmd->FetchNext();
		while (bret)
		{
			strcpy(info.sTxAcc ,m_pSqlCmd->Field(0).asString());
			strcpy(info.sFundTempletId,m_pSqlCmd->Field(1).asString());
			strcpy(info.sPosiTempletId,m_pSqlCmd->Field(2).asString());
			
			if (m_pMemDb->m_pDbSet.m_pCustTemplet.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"CUST_TEMPLET [%s] repeat",info.sTxAcc);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"资金账号模板对应 CUST_TEMPLET [%s]重复",info.sTxAcc);
#endif
				return false;
			}
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}
	return true;	
}

bool CInitTrade::ReadFeeTemplet()
{
	bool bret;
	
	S_MDB_FEE_TEMPLET info;
	sprintf(m_sSql,"select FEE_MODEL_CODE,MODEL_NAME,MODEL_TYPE,ORG from FEE_MODEL_DEF ");
	try
	{
		if (!m_pSqlCmd->setCommandText(m_sSql))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setCommandText error <%s>",m_sSql);
			return false;
		}
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}
		
		bret = m_pSqlCmd->FetchNext();
		while (bret)
		{
			strcpy(info.sFareModeCode ,m_pSqlCmd->Field(0).asString());
			strcpy(info.sFareModeName,m_pSqlCmd->Field(1).asString());
			strcpy(info.sFareType,m_pSqlCmd->Field(2).asString());
			strcpy(info.sOrgCode,m_pSqlCmd->Field(3).asString());
			if (m_pMemDb->m_pDbSet.m_pFeeTemplet.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"FEE_MODEL_DEF [%s] repeat",info.sFareModeCode);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"费率模板表 FEE_MODEL_DEF [%s]重复",info.sFareModeCode);
#endif
				return false;
			}
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}
	return true;	
}

bool CInitTrade::ReadFeeTempletDetail()
{
	bool bret;
	
	S_MDB_FEE_TEMPLET_DETAIL info;
	sprintf(m_sSql,"select a.FEE_MODEL_CODE,a.PROD_TYPE,a.VARIETY_NO,a.VARIETY_CODE,a.CONTRACT_NO,a.CONTRACT_CODE,a.FEE_NO,a.FEE_MODE,a.FEE_VALUE from FEE_MODEL_DETAIL a ");
	try
	{
		if (!m_pSqlCmd->setCommandText(m_sSql))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setCommandText error <%s>",m_sSql);
			return false;
		}
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}
		
		bret = m_pSqlCmd->FetchNext();
		while (bret)
		{
			strcpy(info.sFareModeCode ,m_pSqlCmd->Field(0).asString());
			strcpy(info.sProdType,m_pSqlCmd->Field(1).asString());
			info.nVarietyNo  = m_pSqlCmd->Field(2).asLong();
			strcpy(info.sVarietyCode,m_pSqlCmd->Field(3).asString());
			info.nContractNo  = m_pSqlCmd->Field(4).asLong();
			strcpy(info.sContractCode,m_pSqlCmd->Field(5).asString());
			strcpy(info.sFareNo,m_pSqlCmd->Field(6).asString());
			strcpy(info.sFareMode,m_pSqlCmd->Field(7).asString());
			info.dValue = m_pSqlCmd->Field(8).asDouble();

			if (m_pMemDb->m_pDbSet.m_pFeeTempletDetail.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"FEE_MODEL_DETAIL [%s] repeat",info.sFareModeCode);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"费率模板设置表 FEE_MODEL_DETAIL [%s]重复",info.sFareModeCode);
#endif
				return false;
			}
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}
	return true;
}

bool CInitTrade::ReadCustFeeDetail()
{
	bool bret;
	
	S_MDB_CUST_FEE info;
	sprintf(m_sSql,"select a.TX_ACC,a.PROD_TYPE,a.VARIETY_NO,a.VARIETY_CODE,a.CONTRACT_NO,a.CONTRACT_CODE,a.FEE_NO,a.FEE_MODE,a.FEE_VALUE,a.EX_INCLUDE from CUST_FEE_RATE a ");
	try
	{
		if (!m_pSqlCmd->setCommandText(m_sSql))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setCommandText error <%s>",m_sSql);
			return false;
		}
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}
		
		bret = m_pSqlCmd->FetchNext();
		while (bret)
		{
			strcpy(info.sTxAcc ,m_pSqlCmd->Field(0).asString());
			strcpy(info.sProdType,m_pSqlCmd->Field(1).asString());
			info.nVarietyNo  = m_pSqlCmd->Field(2).asLong();
			strcpy(info.sVarietyCode,m_pSqlCmd->Field(3).asString());
			info.nContractNo  = m_pSqlCmd->Field(4).asLong();
			strcpy(info.sContractCode,m_pSqlCmd->Field(5).asString());
			strcpy(info.sFareNo,m_pSqlCmd->Field(6).asString());
			strcpy(info.sFareMode,m_pSqlCmd->Field(7).asString());
			info.dValue = m_pSqlCmd->Field(8).asDouble();
			strcpy(info.sInclude,m_pSqlCmd->Field(9).asString());

			if (m_pMemDb->m_pDbSet.m_pCustFeeDetail.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"CUST_FEE_RATE [%s] repeat",info.sTxAcc);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"客户特殊费率设置表 CUST_FEE_RATE [%s]重复",info.sTxAcc);
#endif
				return false;
			}
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}
	return true;
}

bool CInitTrade::InitTrader()
{
// 	CTbl_Cust_Online     m_pCustOnline;//客户在线表
// 	CTbl_Cust_Tradecode  m_pTradecode;//资金账号交易编码绑定表
// 	CTbl_Exch_Trader     m_pExchTrader;//交易所席位交易员在线表
// 	
// 	CTbl_Fund            m_pFund; //资金表
// 	CTbl_FundDetail      m_pFundDetail;//资金明细表
// 	CTbl_Position        m_pPosition;//持仓表
// 	CTbl_Storage         m_pStore;//库存表
// 	
// 	CTbl_Order           m_pOrder;//报单表
// 	CTbl_Done            m_pDone;//成交表
// 	CTbl_Fund_Flow       m_pFundFlow;//出入金
// 	CTbl_Quotation       m_pQuotation;//实时行情表
// 	
// 	CTbl_RiskContractInfo    m_pRiskContractInfo;//风险合约信息表

	//客户在线表
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"ReadCustOnline");
	if (!ReadCustOnline())
	{
		return false;
	}
	//资金账号交易编码绑定表
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"ReadTradeCode");
	if (!ReadTradeCode())
	{
		return false;
	}
	//交易所席位交易员在线表
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"ReadExchTradeOnline");
	if (!ReadExchTradeOnline())
	{
		return false;
	}
	//资金表
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"ReadCustFund");
	if (!ReadCustFund())
	{
		return false;
	}
	//资金明细表
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"ReadCustFundDetail");
	if (!ReadCustFundDetail())
	{
		return false;
	}
	//持仓表
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"ReadCustPosition");
	if (!ReadCustPosition())
	{
		return false;
	}
	//库存表
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"ReadCustStorage");
	if (!ReadCustStorage())
	{
		return false;
	}
	//报单表
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"ReadOrder");
	if (!ReadOrder())
	{
		return false;
	}
	//成交表
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"ReadDoneMatch");
	if (!ReadDoneMatch())
	{
		return false;
	}
	//出入金
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"ReadFundFlow");
	if (!ReadFundFlow())
	{
		return false;
	}
	//持仓轧差
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"ReadCustPosiDiff");
	if (!ReadCustPosiDiff())
	{
		return false;
	}
	//实时行情表
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"ReadQuotation");
	if (!ReadQuotation())
	{
		return false;
	}
	return true;
}
 
bool CInitTrade::ReadCustOnline()
{
	bool bret;
	
	S_MDB_CUST_ONLINE info;
	sprintf(m_sSql,"select a.CUST_NO,a.CHANNLE,a.LOGIN_IP,a.LOGIN_STATUS,a.LOGIN_TIME,a.LOGIN_DREBID,a.LOGIN_DREBPID,a.LOGIN_SVRID,a.LOGIN_SVRPID,a.LOGIN_GATEINDEX,LOGIN_TIMESTAMP from CUST_ONLINE_INFO a ");
	try
	{
		if (!m_pSqlCmd->setCommandText(m_sSql))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setCommandText error <%s>",m_sSql);
			return false;
		}
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}
		
		bret = m_pSqlCmd->FetchNext();
		while (bret)
		{
			strcpy(info.sCustNo ,m_pSqlCmd->Field(0).asString());
			strcpy(info.sTermType,m_pSqlCmd->Field(1).asString());
			strcpy(info.sLoginIp,m_pSqlCmd->Field(2).asString());
			strcpy(info.sOnlineStatus,m_pSqlCmd->Field(3).asString());
			strcpy(info.sOnlineTime,m_pSqlCmd->Field(4).asString());
			info.nDrebId  = m_pSqlCmd->Field(5).asLong();
			info.nDrebPid  = m_pSqlCmd->Field(6).asLong();
			info.nSvrId  = m_pSqlCmd->Field(7).asLong();
			info.nSvrPid  = m_pSqlCmd->Field(8).asLong();
			info.nGateIndex  = m_pSqlCmd->Field(9).asLong();
			info.nTimeStamp  = m_pSqlCmd->Field(10).asLong();
			
			if (m_pMemDb->m_pDbTrader.m_pCustOnline.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"CUST_ONLINE_INFO [%s] repeat",info.sCustNo);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"客户在线表 CUST_ONLINE_INFO [%s]重复",info.sCustNo);
#endif
				return false;
			}
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}
	return true;
}

bool CInitTrade::ReadTradeCode()
{
	bool bret;
	
	S_MDB_CUST_TRADECODE info;
	sprintf(m_sSql,"select a.TX_ACC,a.EX_CODE,a.SEAT_NO,a.TX_CODE  from CUST_TRADE_INFO a ");
	try
	{
		if (!m_pSqlCmd->setCommandText(m_sSql))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setCommandText error <%s>",m_sSql);
			return false;
		}
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}
		
		bret = m_pSqlCmd->FetchNext();
		while (bret)
		{
			strcpy(info.sTxAcc ,m_pSqlCmd->Field(0).asString());
			strcpy(info.sExCode,m_pSqlCmd->Field(1).asString());
			strcpy(info.sSeatNo,m_pSqlCmd->Field(2).asString());
			strcpy(info.sTradeCode,m_pSqlCmd->Field(3).asString());
			
			if (m_pMemDb->m_pDbTrader.m_pTradecode.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"CUST_TRADE_INFO [%s] repeat",info.sTxAcc);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"资金账号交易编码对应 CUST_TRADE_INFO [%s]重复",info.sTxAcc);
#endif
				return false;
			}
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}
	return true;
}

bool CInitTrade::ReadExchTradeOnline()
{
	bool bret;
	
	S_MDB_EXCH_TRADER_INFO info;
	sprintf(m_sSql,"select a.EX_CODE,a.SEAT_ID,a.TRADER,a.LOGIN_STATUS,a.LOGIN_TIME,a.LOGOUT_TIME,a.COMM_PARA  from EX_SEAT_TRADE_INFO a ");
	try
	{
		if (!m_pSqlCmd->setCommandText(m_sSql))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setCommandText error <%s>",m_sSql);
			return false;
		}
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}
		
		bret = m_pSqlCmd->FetchNext();
		while (bret)
		{
			strcpy(info.sExchCode ,m_pSqlCmd->Field(0).asString());
			strcpy(info.sSeatNo,m_pSqlCmd->Field(1).asString());
			strcpy(info.sTraderId,m_pSqlCmd->Field(2).asString());
			strcpy(info.sLoginStatus,m_pSqlCmd->Field(3).asString());
			strcpy(info.sLoginTime,m_pSqlCmd->Field(4).asString());
			strcpy(info.sLogoutTime,m_pSqlCmd->Field(5).asString());
			strcpy(info.sPara,m_pSqlCmd->Field(6).asString());
			
			if (m_pMemDb->m_pDbTrader.m_pExchTrader.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"EX_SEAT_TRADE_INFO [%s] repeat",info.sExchCode);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"交易所席位交易员在线 EX_SEAT_TRADE_INFO [%s]重复",info.sExchCode);
#endif
				return false;
			}
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}
	return true;
}

bool CInitTrade::ReadCustFund()
{
	bool bret;
	//清算时，按市场清算，不能一个资金账号对应不同的市场，这样导致多次清算数据对不上
	int rid;
	S_MDB_CUST_FUND info;
	S_MDB_TXACC_LOCK  acclock;
	acclock.nCustRowid=-1;
	acclock.nFundAccRowid =-1;
	int i;
	for (i=0; i<10 ; i++)
	{
		acclock.nFundRowid[i] = -1;
	}
	
	
	sprintf(m_sSql,"select TX_ACC,CURRENCY,LAST_BAL,BALANCE,AVAIL_FUND,GET_FUND,BASE_MARGIN,INCOME_EXPENSE,FROZE_FUND,MARGIN,FEE,RESERVE,SURPLUS,FLOAT_SURPLUS,INCR_INTE,ARAP,RISK_LEVEL,RISK,TRADE_RIGHT from CUST_FUND");
	try
	{
		if (!m_pSqlCmd->setCommandText(m_sSql))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setCommandText error <%s>",m_sSql);
			return false;
		}
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}
		
		bret = m_pSqlCmd->FetchNext();
		while (bret)
		{
			bzero(&info,sizeof(S_MDB_CUST_FUND));
			strcpy(info.sTxAcc ,m_pSqlCmd->Field(0).asString());
			strcpy(info.sCurrency,m_pSqlCmd->Field(1).asString());
			
			info.dLast_bal =m_pSqlCmd->Field(2).asDouble();
			info.dBalance =m_pSqlCmd->Field(3).asDouble();
			info.dAvailFund =m_pSqlCmd->Field(4).asDouble();
			info.dGetFund =m_pSqlCmd->Field(5).asDouble();
			info.dBaseMargin =m_pSqlCmd->Field(6).asDouble();
			info.dIncomeExpense =m_pSqlCmd->Field(7).asDouble();
			info.dFrozeFund =m_pSqlCmd->Field(8).asDouble();
			info.dMargin =m_pSqlCmd->Field(9).asDouble();
			info.dFee =m_pSqlCmd->Field(10).asDouble();
			info.dReserve =m_pSqlCmd->Field(11).asDouble();
			info.dSurplus =m_pSqlCmd->Field(12).asDouble();
			info.dFloatSurplus =m_pSqlCmd->Field(13).asDouble();
			info.dIncrInte =m_pSqlCmd->Field(14).asDouble();
			info.dArap =m_pSqlCmd->Field(15).asDouble();
			//默认级别是10无风险，启动后计算
			info.nRisk_level =11;
			//info.nRisk_level =m_pSqlCmd->Field(16).asLong();
			info.dRisk =m_pSqlCmd->Field(17).asDouble();
			strcpy(info.sTradeRight,m_pSqlCmd->Field(18).asString());
			rid = m_pMemDb->m_pDbTrader.m_pFund.Insert(info);
			if (rid <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"CUST_FUND [%s] repeat",info.sTxAcc);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"资金表 CUST_FUND [%s]重复",info.sTxAcc);
#endif
				return false;
			}
			//资金表rowid先不加，统一在acclock里面取初始
			//acclock.nFundRowid[atoi(info.sCurrency)] = rid;
			strcpy(acclock.sTxAcc,info.sTxAcc);

			rid = m_pMemDb->InsertAccLock(acclock);
			
			//通知要进行风险计算 启动后所有的计算一遍
			m_pMemDb->m_pSurplusLockRowId.Add(rid);
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}
	return true;
}

bool CInitTrade::ReadCustFundDetail()
{
	bool bret;
	
	S_MDB_CUST_FUND_DETAIL info;
	sprintf(m_sSql,"select TX_ACC,FUND_TYPE,FUND_CODE,FUND_VALUE,CURRENCY from CUST_FUND_DETAIL ");
	try
	{
		if (!m_pSqlCmd->setCommandText(m_sSql))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setCommandText error <%s>",m_sSql);
			return false;
		}
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}
		
		bret = m_pSqlCmd->FetchNext();
		while (bret)
		{
			strcpy(info.sTxAcc ,m_pSqlCmd->Field(0).asString());
			strcpy(info.sFundType,m_pSqlCmd->Field(1).asString());
			strcpy(info.sFundCode,m_pSqlCmd->Field(2).asString());
			info.dFundValue =m_pSqlCmd->Field(3).asDouble();
			strcpy(info.sCurrency,m_pSqlCmd->Field(4).asString());
			if (m_pMemDb->m_pDbTrader.m_pFundDetail.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"CUST_FUND_DETAIL [%s] repeat",info.sTxAcc);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"资金详细表 CUST_FUND_DETAIL [%s]重复",info.sTxAcc);
#endif
				return false;
			}
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}
	return true;
}

bool CInitTrade::ReadCustPosition()
{
	bool bret;
	int rid;
	S_MDB_CUST_POSITION info;
	int num=0;
	sprintf(m_sSql,"select TX_ACC,CONTRACT_NO,CONTRACT_CODE,SEAT_ID,TX_CODE,TB_FLAG,LAST_PB,LAST_PS,CUR_PB,CUR_PS,CUR_CANUSE_PB,CUR_CANUSE_PS,DAY_PB,DAY_PS,DAY_COV_PB_FROZ,DAY_COV_PB,DAY_COV_PS_FROZ,DAY_COV_PS,DAY_DELI_PB_FROZ,DAY_DELI_PB,DAY_DELI_PS_FROZ,DAY_DELI_PS,DAY_MIDD_PB,DAY_MIDD_PS,PB_OPEN_AVG_PRICE,PS_OPEN_AVG_PRICE,PB_AVG_PRICE,PS_AVG_PRICE,PB_MARGIN,PS_MARGIN,DAY_PB_SURPLUS,DAY_PS_SURPLUS,LAST_SETTLE_PRICE,DAY_SETTLE_PRICE,VARIETY from CUST_POSI");
	try
	{
		if (!m_pSqlCmd->setCommandText(m_sSql))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setCommandText error <%s>",m_sSql);
			return false;
		}
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}
		S_MDB_CONTRACT_INFO *pc=NULL;
		bret = m_pSqlCmd->FetchNext();
		while (bret)
		{
			strcpy(info.sTxAcc ,m_pSqlCmd->Field(0).asString());
			info.nContractNo = m_pSqlCmd->Field(1).asLong();
			strcpy(info.sContractCode,m_pSqlCmd->Field(2).asString());
			strcpy(info.sSeatNo,m_pSqlCmd->Field(3).asString());
			strcpy(info.sTradeCode,m_pSqlCmd->Field(4).asString());
			strcpy(info.sShFlag,m_pSqlCmd->Field(5).asString());
			info.nLastPB =m_pSqlCmd->Field(6).asLong();
			info.nLastPS =m_pSqlCmd->Field(7).asLong();
			info.nCurPB =m_pSqlCmd->Field(8).asLong();
			info.nCurPS =m_pSqlCmd->Field(9).asLong();
			info.nCurCanUsePB =m_pSqlCmd->Field(10).asLong();
			info.nCurCanUsePS =m_pSqlCmd->Field(11).asLong();
			info.nDayPB =m_pSqlCmd->Field(12).asLong();
			info.nDayPS =m_pSqlCmd->Field(13).asLong();
			info.nDAY_COV_PB_Froze =m_pSqlCmd->Field(14).asLong();
			info.nDAY_COV_PB =m_pSqlCmd->Field(15).asLong();
			info.nDAY_COV_PS_Froze =m_pSqlCmd->Field(16).asLong();
			info.nDAY_COV_PS =m_pSqlCmd->Field(17).asLong();
			info.nDAY_DELI_PB_Froze =m_pSqlCmd->Field(18).asLong();
			info.nDAY_DELI_PB =m_pSqlCmd->Field(19).asLong();
			info.nDAY_DELI_PS_Froze =m_pSqlCmd->Field(20).asLong();
			info.nDAY_DELI_PS =m_pSqlCmd->Field(21).asLong();
			info.nDAY_MIDD_PB =m_pSqlCmd->Field(22).asLong();
			info.nDAY_MIDD_PS =m_pSqlCmd->Field(23).asLong();
			info.dPB_OAvp =m_pSqlCmd->Field(24).asDouble();
			info.dPS_OAvp =m_pSqlCmd->Field(25).asDouble();
			info.dPB_Avp =m_pSqlCmd->Field(26).asDouble();
			info.dPS_Avp =m_pSqlCmd->Field(27).asDouble();
			info.dPB_Margin =m_pSqlCmd->Field(28).asDouble();
			info.dPS_Margin =m_pSqlCmd->Field(29).asDouble();
			info.dDAY_PB_SURPLUS =m_pSqlCmd->Field(30).asDouble();
			info.dDAY_PS_SURPLUS =m_pSqlCmd->Field(31).asDouble();
			info.dLAST_SETTLE_PRICE = m_pSqlCmd->Field(32).asDouble();
			info.dSETTLE_PRICE = m_pSqlCmd->Field(33).asDouble();
			info.nVarietyId = m_pSqlCmd->Field(34).asLong();
			if (m_pMemDb->m_pDbTrader.m_pPosition.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"CUST_POSI [%s] repeat",info.sTxAcc);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"持仓表 CUST_POSI [%s]重复",info.sTxAcc);
#endif
				return false;
			}
			//将需要处理的客户加入
			S_MDB_TXACC_LOCK *acclock = m_pMemDb->SelectLock(info.sTxAcc);
			if (acclock != NULL)
			{
				m_keyCustRowid.Add(acclock->nLockRowid);
			}
			else
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"SelectLock 值为NULL [%s]",info.sTxAcc);
				return false;
			}
			bret = m_pSqlCmd->FetchNext();
			num++;
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"总共持仓[%d]条",num);
	return true;
}

bool CInitTrade::ReadCustStorage()
{
	return true;
}

bool CInitTrade::ReadOrder()
{
	int rid;
	bool bret;
	//委托另外同步至数据库，防止重启时得不到报单，成交回来时取不到对应的信息。
	S_MDB_ORDER_INFO info;
	S_MDB_CONTRACT_INFO *pc=NULL;
	sprintf(m_sSql,"select TX_ACC,EX_CODE,SEAT_NO,TX_CODE,CONTRACT_NO,CONTRACT_CODE,CLIENT_SERIAL,LOCAL_NO,SYSTEM_NO,ORDER_NUM,ORDER_PRICE,REMAIN_NUM,EO_FLAG,BS_FLAG,SH_FLAG,TXDATE,AMT,FROZ_AMT,EX_FARE,TX_TYPE,CMD_TYPE,CMD_ATTR,FORCE_FLAG,ORDER_STATUS,CANCEL_FLAG,ANS_TIME,ORDER_CHANNEL,ACC_ORG,TX_ORG,ORDER_OPER,CANCEL_CHANNEL,CANCEL_OPER,CANCEL_TIME,ORDER_RSP,FROZ_USEAMT,FROZ_OUTAMT,CANCEL_NUM,DONE_AVP,VARIETY from ORDER_INFO order by SYSTEM_NO");
	try
	{
		if (!m_pSqlCmd->setCommandText(m_sSql))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setCommandText error <%s>",m_sSql);
			return false;
		}
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}
		
		bret = m_pSqlCmd->FetchNext();
		while (bret)
		{
			strcpy(info.sTxAcc ,m_pSqlCmd->Field(0).asString());
			strcpy(info.sExchCode,m_pSqlCmd->Field(1).asString());
			strcpy(info.sSeatNo,m_pSqlCmd->Field(2).asString());
			strcpy(info.sTxCode,m_pSqlCmd->Field(3).asString());
			info.nContractNo = m_pSqlCmd->Field(4).asLong();
			strcpy(info.sContractCode,m_pSqlCmd->Field(5).asString());
			strcpy(info.sClientSerial,m_pSqlCmd->Field(6).asString());
			strcpy(info.sLocalSerial,m_pSqlCmd->Field(7).asString());
			strcpy(info.sSystemNo,m_pSqlCmd->Field(8).asString());
			info.nOrderNum =m_pSqlCmd->Field(9).asLong();
			info.dOrderPrice =m_pSqlCmd->Field(10).asDouble();
			info.nRemainNum =m_pSqlCmd->Field(11).asLong();
			strcpy(info.sEoFlag ,m_pSqlCmd->Field(12).asString());
			strcpy(info.sBsFlag ,m_pSqlCmd->Field(13).asString());
			strcpy(info.sShFlag ,m_pSqlCmd->Field(14).asString());
			strcpy(info.sTxDate ,m_pSqlCmd->Field(15).asString());
			info.dAmt =m_pSqlCmd->Field(16).asDouble();
			info.dFrozAmt =m_pSqlCmd->Field(17).asDouble();
			info.dExFee =m_pSqlCmd->Field(18).asDouble();
			strcpy(info.sTxType ,m_pSqlCmd->Field(19).asString());
			strcpy(info.sCmdType ,m_pSqlCmd->Field(20).asString());
			strcpy(info.sCmdAttr,m_pSqlCmd->Field(21).asString());
			strcpy(info.sForceFlag ,m_pSqlCmd->Field(22).asString());
			strcpy(info.sOrderStatus ,m_pSqlCmd->Field(23).asString());
			strcpy(info.sCancelFlag ,m_pSqlCmd->Field(24).asString());
			strcpy(info.sAnsTime ,m_pSqlCmd->Field(25).asString());
			strcpy(info.sOrderChannel ,m_pSqlCmd->Field(26).asString());
			strcpy(info.sAccOrg ,m_pSqlCmd->Field(27).asString());
			strcpy(info.sTxOrg ,m_pSqlCmd->Field(28).asString());
			strcpy(info.sOrderOper ,m_pSqlCmd->Field(29).asString());
			strcpy(info.sCancelChannel ,m_pSqlCmd->Field(30).asString());
			strcpy(info.sCancelOper ,m_pSqlCmd->Field(31).asString());
			strcpy(info.sCancelTime ,m_pSqlCmd->Field(32).asString());
			strcpy(info.sOrderRsp ,m_pSqlCmd->Field(33).asString());
			info.dFrozUseamt = m_pSqlCmd->Field(34).asDouble();
			info.dFrozOutamt = m_pSqlCmd->Field(35).asDouble();
			info.nCancelNum = m_pSqlCmd->Field(36).asLong();
			info.dDoneAvp = m_pSqlCmd->Field(37).asDouble();
			info.nVarietyId = m_pSqlCmd->Field(38).asLong();
			if (m_pMemDb->m_pDbTrader.m_pOrder.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"ORDER_INFO [%s] repeat",info.sTxAcc);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"报单表 ORDER_INFO [%s]重复",info.sTxAcc);
#endif
				return false;
			}
			S_MDB_TXACC_LOCK *acclock = m_pMemDb->SelectLock(info.sTxAcc);
			if (acclock != NULL)
			{
				m_keyCustRowid.Add(acclock->nLockRowid);
			}
			else
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"SelectLock 值为NULL [%s]",info.sTxAcc);
				return false;
			}
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}
	return true;
}

bool CInitTrade::ReadDoneMatch()
{
	int rid;
	bool bret;
	//内存里面要根据资金持仓+成交出入金重新计算资金等。
	S_MDB_DONE_INFO info;
	sprintf(m_sSql,"select DONE_NO,SYSTEM_NO,EX_CODE,TX_CODE,LOCAL_NO,TX_ACC,DONE_DATE,DONE_TIME,CONTRACT_NO,CONTRACT_CODE,DONE_PRICE,DONE_QTY,BS_FLAG,SH_FLAG,EO_FLAG,TX_TYPE,ORDER_TYPE,DONE_ATTR,AMT,EX_FEE,MEMBER_FEE,EX_MARGIN,MEMBER_MARGIN,FLOAT_SURPLUS,COV_SURPLUS,TX_ORG,ACC_ORG,FORCE_FLAG,ORDER_CHANNEL,ORDER_OPER,SYSTEM_DATE,SEAT_NO,VARIETY from DONE_INFO order by DONE_NO");
	try
	{
		if (!m_pSqlCmd->setCommandText(m_sSql))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setCommandText error <%s>",m_sSql);
			return false;
		}
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}
		
		bret = m_pSqlCmd->FetchNext();
		while (bret)
		{
			bzero(&info,sizeof(S_MDB_DONE_INFO));
			strcpy(info.sDoneNo ,m_pSqlCmd->Field(0).asString());
			strcpy(info.sSystemNo,m_pSqlCmd->Field(1).asString());
			strcpy(info.sExchCode,m_pSqlCmd->Field(2).asString());
			strcpy(info.sTxCode,m_pSqlCmd->Field(3).asString());
			strcpy(info.sLocalNo,m_pSqlCmd->Field(4).asString());
			strcpy(info.sTxAcc,m_pSqlCmd->Field(5).asString());
			strcpy(info.sDoneDate,m_pSqlCmd->Field(6).asString());
			strcpy(info.sDoneTime,m_pSqlCmd->Field(7).asString());
			info.nContractNo =m_pSqlCmd->Field(8).asLong();
			strcpy(info.sContractCode,m_pSqlCmd->Field(9).asString());
			info.dDonePrice =m_pSqlCmd->Field(10).asDouble();
			info.nDoneNum = m_pSqlCmd->Field(11).asLong();
			strcpy(info.sBsFlag ,m_pSqlCmd->Field(12).asString());
			strcpy(info.sShFlag ,m_pSqlCmd->Field(13).asString());
			strcpy(info.sEoFlag ,m_pSqlCmd->Field(14).asString());
			strcpy(info.sTxType ,m_pSqlCmd->Field(15).asString());
			strcpy(info.sCmdType ,m_pSqlCmd->Field(16).asString());
			strcpy(info.sCmdAttr ,m_pSqlCmd->Field(17).asString());
			info.dAmt =m_pSqlCmd->Field(18).asDouble();
			info.dExFee =m_pSqlCmd->Field(19).asDouble();
			info.dMemberFee =m_pSqlCmd->Field(20).asDouble();
			info.dExMargin =m_pSqlCmd->Field(21).asDouble();
			info.dMemberMargin =m_pSqlCmd->Field(22).asDouble();
			info.dFloatSurplus =m_pSqlCmd->Field(23).asDouble();
			info.dCovSurplus =m_pSqlCmd->Field(24).asDouble();
			strcpy(info.sTxOrg ,m_pSqlCmd->Field(25).asString());
			strcpy(info.sAccOrg ,m_pSqlCmd->Field(26).asString());
			strcpy(info.sForceFlag ,m_pSqlCmd->Field(27).asString());
			strcpy(info.sOrderChannel ,m_pSqlCmd->Field(28).asString());
			strcpy(info.sOrderOper ,m_pSqlCmd->Field(29).asString());
			strcpy(info.sSystemDate ,m_pSqlCmd->Field(30).asString());
			strcpy(info.sSeatNo ,m_pSqlCmd->Field(31).asString());
			info.nVarietyId = m_pSqlCmd->Field(32).asLong();
			if (m_pMemDb->m_pDbTrader.m_pDone.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"DONE_INFO [%s] repeat",info.sTxAcc);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"成交表 DONE_INFO [%s]重复",info.sTxAcc);
#endif
				return false;
			}
			S_MDB_TXACC_LOCK *acclock = m_pMemDb->SelectLock(info.sTxAcc);
			if (acclock != NULL)
			{
				m_keyCustRowid.Add(acclock->nLockRowid);
			}
			else
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"SelectLock 值为NULL [%s]",info.sTxAcc);
				return false;
			}
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}
	return true;
}

bool CInitTrade::ReadFundFlow()
{
	bool bret;
	int rid;
	S_MDB_FUND_FLOW info;
	sprintf(m_sSql,"select TX_DATE,TX_ACC,TX_SERIAL,BANK_ACC,BANK_NAME,CURRENCY,AMT,FUND_FLAG,LAST_AMT,CUR_AMT from FUND_FLOW where IS_REC='1' ");
	try
	{
		if (!m_pSqlCmd->setCommandText(m_sSql))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setCommandText error <%s>",m_sSql);
			return false;
		}
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}
		
		bret = m_pSqlCmd->FetchNext();
		while (bret)
		{
			strcpy(info.sTxDate ,m_pSqlCmd->Field(0).asString());
			strcpy(info.sTxAcc,m_pSqlCmd->Field(1).asString());
			strcpy(info.sTxSerial,m_pSqlCmd->Field(2).asString());
			strcpy(info.sBankAcc,m_pSqlCmd->Field(3).asString());
			strcpy(info.sBankName,m_pSqlCmd->Field(4).asString());
			strcpy(info.sCurrency,m_pSqlCmd->Field(5).asString());
			info.dAmt = m_pSqlCmd->Field(6).asDouble();
			strcpy(info.sFundFlag,m_pSqlCmd->Field(7).asString());
			info.dLastAmt =m_pSqlCmd->Field(8).asDouble();
			info.dCurAmt =m_pSqlCmd->Field(9).asDouble();
			if (m_pMemDb->m_pDbTrader.m_pFundFlow.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"FUND_FLOW [%s] repeat",info.sTxAcc);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"出入金表 FUND_FLOW [%s]重复",info.sTxAcc);
#endif
				return false;
			}
			S_MDB_TXACC_LOCK *acclock = m_pMemDb->SelectLock(info.sTxAcc);
			if (acclock != NULL)
			{
				m_keyCustRowid.Add(acclock->nLockRowid);
			}
			else
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"SelectLock 值为NULL [%s]",info.sTxAcc);
				return false;
			}
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}
	return true;
}

bool CInitTrade::ReadQuotation()
{
	return true;
}

bool CInitTrade::ProcCustFund()
{
	bool bret;
	int rid;
	bret = m_keyCustRowid.First(rid);
	while (bret)
	{
		if (!ProcOneCust(rid))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"ProcOneCust error");
			return false;
		}
		bret = m_keyCustRowid.Next(rid);
	}
	return true;
}

bool CInitTrade::ProcOneCust(int rid)
{
	S_MDB_TXACC_LOCK *info = m_pMemDb->SelectLock(rid);
	if (info == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"资金账号锁表的ROWID[%d]没有找到",rid);
		return false;
	}
	//首先出入金
	std::vector<S_MDB_FUND_FLOW *> flowlist;
	if (m_pMemDb->m_pDbTrader.m_pFundFlow.SelectByAcc(info->sTxAcc,flowlist))
	{
		//处理
		if (!m_pKernelPub.ProcFundFlow(info,flowlist))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"处理[%s]出入金流水失败",info->sTxAcc);
			return false;
		}
	}
	//持仓
	//判断是否大边，进行大边处理 导入的持仓的保证金，已经在资金表里，但资金明细表没有。这里要经过持仓计算，分别得到买保证金、卖保证金，记录资金明细表。
	std::vector<S_MDB_CUST_POSITION *> posilis;
	if (m_pMemDb->m_pDbTrader.m_pPosition.SelectByAccOrderByVariety(info->sTxAcc,posilis))
	{
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"开始处理[%s]持仓[%d]条",info->sTxAcc,posilis.size());
		if (!m_pKernelPub.ProcPosition(info,posilis))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"处理[%s]持仓失败",info->sTxAcc);
			return false;
		}
	}


	//成交
	std::vector<S_MDB_DONE_INFO *> donelist;
	if (m_pMemDb->m_pDbTrader.m_pDone.SelectByAcc(info->sTxAcc,donelist))
	{
		//处理
		if (!m_pKernelPub.ProcDone(info,donelist))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"处理[%s]成交失败",info->sTxAcc);
			return false;
		}
	}
	//报单
	std::vector<S_MDB_ORDER_INFO *> orderlist;
	if (m_pMemDb->m_pDbTrader.m_pOrder.SelectByAcc(info->sTxAcc,orderlist))
	{
		//处理
		if (!m_pKernelPub.ProcOrder(info,orderlist))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"处理[%s]报单失败",info->sTxAcc);
			return false;
		}
	}
	return true;
}

bool CInitTrade::ReadOptionModel()
{	
	bool bret;
	int rid;
	S_MDB_OPTION_CONTRACT_PRICE_MODEL info;
	
	sprintf(m_sSql,"select CONTRACT_NO,CONTRACT_CODE,EXCH_DATE,SETTLE_PRICE,VOLATILITY,THEORETICAL_PRICE,DELTA,GAMMA,VEGA,THETA,RHO,IMPLIED_VOLATILITY_BS,IMPLIED_VOLATILITY_GARCH from OPTION_CONTRACT_PRICE_MODEL  order by EXCH_DATE desc ");
	try
	{
		if (!m_pSqlCmd->setCommandText(m_sSql))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setCommandText error <%s>",m_sSql);
			return false;
		}
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}
		
		bret = m_pSqlCmd->FetchNext();
		if ( !bret )
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"Table OPTION_CONTRACT_PRICE_MODEL no data");
#else
			m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"期权价格模型表OPTION_CONTRACT_PRICE_MODEL表无数据 ");
#endif
			return true;
		}
		while (bret)
		{
			info.nContractNo = m_pSqlCmd->Field(0).asLong();
			strcpy(info.sContractCode,m_pSqlCmd->Field(1).asString());
			strcpy(info.sSettleDate,m_pSqlCmd->Field(2).asString());
			info.dSettlePrice = m_pSqlCmd->Field(3).asDouble();
			info.dHvDes = m_pSqlCmd->Field(4).asDouble();
			info.dPrice = m_pSqlCmd->Field(5).asDouble();
			info.dDelta = m_pSqlCmd->Field(6).asDouble();
			info.dGamma = m_pSqlCmd->Field(7).asDouble();
			info.dVega = m_pSqlCmd->Field(8).asDouble();
			info.dTheta = m_pSqlCmd->Field(9).asDouble();
			info.dRho = m_pSqlCmd->Field(10).asDouble();
			info.dBs = m_pSqlCmd->Field(11).asDouble();
			info.dGarch = m_pSqlCmd->Field(12).asDouble();
		
			rid = m_pMemDb->m_pDbBaseinfo.m_pOptionModel.Insert(info);
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}
	return true;
}
bool CInitTrade::ReadCustPosiDiff()
{
	bool bret;

	S_MDB_DIFF_POSI info;
	sprintf(m_sSql,"select TX_DATE,TX_ACC,SEAT_ID,SH_FLAG,BS_FLAG,CONTRACT_NO,CONTRACT_CODE,DIFF_QTY,TX_SERIAL from CUST_POSI_DIFF ");
	try
	{
		if (!m_pSqlCmd->setCommandText(m_sSql))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setCommandText error <%s>",m_sSql);
			return false;
		}
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}

		bret = m_pSqlCmd->FetchNext();
		while (bret)
		{
			strcpy(info.sTxDate ,m_pSqlCmd->Field(0).asString());
			strcpy(info.sTxAcc,m_pSqlCmd->Field(1).asString());
			strcpy(info.sSeatNo,m_pSqlCmd->Field(2).asString());
			strcpy(info.sShFlag,m_pSqlCmd->Field(3).asString());
			strcpy(info.sBsFlag,m_pSqlCmd->Field(4).asString());
			info.nContractNo  = m_pSqlCmd->Field(5).asLong();
			strcpy(info.sContractCode,m_pSqlCmd->Field(6).asString());
			info.nDiffQty  = m_pSqlCmd->Field(7).asLong();
			strcpy(info.sSerial,m_pSqlCmd->Field(8).asString());

			if (m_pMemDb->m_pDbTrader.m_pDiffPosi.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"CUST_POSI_DIFF [%s %s] repeat",info.sTxDate,info.sSerial);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"持仓轧差表 CUST_POSI_DIFF [%s %s]重复",info.sTxDate,info.sSerial);
#endif
				return false;
			}
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}
	return true;
}

bool CInitTrade::ReadBrokerageInfo()
{

	bool bret;

	S_MDB_BROKERAGE_INFO info;
	sprintf(m_sSql,"select BROKERAGE_CODE,NAME,PG_DREBSVRID,ORGAN_CODE,CLEAR_DATE,CLEAR_FLAG from BROKERAGE_INFO ");
	try
	{
		if (!m_pSqlCmd->setCommandText(m_sSql))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setCommandText error <%s>",m_sSql);
			return false;
		}
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}

		bret = m_pSqlCmd->FetchNext();
		while (bret)
		{
			info.nSysCode  = m_pSqlCmd->Field(0).asLong();
			strcpy(info.sSysName,m_pSqlCmd->Field(1).asString());
			info.nSvrId  = m_pSqlCmd->Field(2).asLong();
			strcpy(info.sOrgCode,m_pSqlCmd->Field(3).asString());
			strcpy(info.sClearDate,m_pSqlCmd->Field(4).asString());
			info.nClearFlag  = m_pSqlCmd->Field(5).asLong();
			if (m_pMemDb->m_pDbSet.m_pBrokerage.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"BROKERAGE_INFO [%d %s] repeat",info.nSysCode,info.sSysName);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"经纪信息 BROKERAGE_INFO [%d %s]重复",info.nSysCode,info.sSysName);
#endif
				return false;
			}
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}
	S_MDB_BROKERAGE_EXCHANGE exchinfo;
	sprintf(m_sSql,"select BROKERAGE_CODE,EXCH_CODE,SEAT_NO,DREBSVRID from BROKERAGE_EXCHANGE ");
	try
	{
		if (!m_pSqlCmd->setCommandText(m_sSql))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setCommandText error <%s>",m_sSql);
			return false;
		}
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}

		bret = m_pSqlCmd->FetchNext();
		while (bret)
		{
			exchinfo.nSysCode  = m_pSqlCmd->Field(0).asLong();
			strcpy(exchinfo.sExchCode,m_pSqlCmd->Field(1).asString());
			strcpy(exchinfo.sSeatNo,m_pSqlCmd->Field(2).asString());
			exchinfo.nSvrId = m_pSqlCmd->Field(3).asLong();
			
			if (m_pMemDb->m_pDbSet.m_pBrokerageExchange.Insert(exchinfo) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"BROKERAGE_EXCHANGE [%d %s %s] repeat",exchinfo.nSysCode,exchinfo.sExchCode,exchinfo.sSeatNo);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"经纪交易所信息 BROKERAGE_EXCHANGE [%d %s %s]重复",exchinfo.nSysCode,exchinfo.sExchCode,exchinfo.sSeatNo);
#endif
				return false;
			}
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}
	S_MDB_BROKERAGE_PG pginfo;
	sprintf(m_sSql,"select BROKERAGE_CODE,PG_BROKERID,DREBSVRID from BROKERAGE_PG ");
	try
	{
		if (!m_pSqlCmd->setCommandText(m_sSql))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setCommandText error <%s>",m_sSql);
			return false;
		}
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Execute <%s> error %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}

		bret = m_pSqlCmd->FetchNext();
		while (bret)
		{
			pginfo.nSysCode  = m_pSqlCmd->Field(0).asLong();
			strcpy(pginfo.sBrokerId,m_pSqlCmd->Field(1).asString());
			pginfo.nSvrId = m_pSqlCmd->Field(2).asLong();

			if (m_pMemDb->m_pDbSet.m_pBrokeragePg.Insert(pginfo) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"BROKERAGE_PG [%d %s] repeat",pginfo.nSysCode,pginfo.sBrokerId);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"经纪交易所信息 BROKERAGE_PG [%d %s]重复",pginfo.nSysCode,pginfo.sBrokerId);
#endif
				return false;
			}
			bret = m_pSqlCmd->FetchNext();
		}
	}
	catch (...)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Database operator Exception!");
#else
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据库操作异常");
#endif
		return false;
	}
	return true;
}