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
	sprintf(m_sSql,"select ORG_CODE,ORG_NAME,ORG_TYPE,ORG_LEVEL,UP1_ORG,UP_ORG,CITY,SETTLE_ACCOUNT_NO from sys_org_info order by ORG_CODE");
	
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
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Table sys_org_info no data");
#else
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"机构表无数据");
#endif
			return false;
		}
		while (bret)
		{
			CBF_Tools::StringCopy(info.sOrgCode,sizeof(info.sOrgCode)-1,m_pSqlCmd->Field(0).asString());
			CBF_Tools::StringCopy(info.sOrgName,sizeof(info.sOrgName) - 1,m_pSqlCmd->Field(1).asString());
			CBF_Tools::StringCopy(info.sOrgType, sizeof(info.sOrgType) - 1,m_pSqlCmd->Field(2).asString());
			CBF_Tools::StringCopy(info.sOrgLevel, sizeof(info.sOrgLevel) - 1,m_pSqlCmd->Field(3).asString());
			CBF_Tools::StringCopy(info.sUp1Org, sizeof(info.sUp1Org) - 1,m_pSqlCmd->Field(4).asString());
			CBF_Tools::StringCopy(info.sUpOrg, sizeof(info.sUpOrg) - 1,m_pSqlCmd->Field(5).asString());
			CBF_Tools::StringCopy(info.sOrgCity, sizeof(info.sOrgCity) - 1,m_pSqlCmd->Field(6).asString());
			CBF_Tools::StringCopy(info.sAccNo, sizeof(info.sAccNo) - 1,m_pSqlCmd->Field(7).asString());
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
	sprintf(m_sSql,"select OPER_CODE,OPER_NAME,OPER_PHONE,OPER_ORG,OPER_TYPE from sys_oper_info order by OPER_CODE");
	
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
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Table sys_oper_info no data");
#else
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"操作员表无数据");
#endif
			return false;
		}
		while (bret)
		{
			CBF_Tools::StringCopy(info.sOperCode, sizeof(info.sOperCode) - 1,m_pSqlCmd->Field(0).asString());
			CBF_Tools::StringCopy(info.sOperName, sizeof(info.sOperName) - 1,m_pSqlCmd->Field(1).asString());
			CBF_Tools::StringCopy(info.sOperPhone, sizeof(info.sOperPhone) - 1,m_pSqlCmd->Field(2).asString());
			CBF_Tools::StringCopy(info.sOrgCode, sizeof(info.sOrgCode) - 1,m_pSqlCmd->Field(3).asString());
			CBF_Tools::StringCopy(info.sOperType, sizeof(info.sOperType) - 1,m_pSqlCmd->Field(4).asString());
			
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
	sprintf(m_sSql,"select PARA_CODE,PARA_NAME,PARA_VALUE,VALUE_TYPE from sys_system_prara order by PARA_CODE");
	
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
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Table sys_system_prara no data");
#else
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"系统参数表无数据");
#endif
			return false;
		}
		while (bret)
		{
			CBF_Tools::StringCopy(info.sParaCode, sizeof(info.sParaCode) - 1, m_pSqlCmd->Field(0).asString());
			CBF_Tools::StringCopy(info.sParaName, sizeof(info.sParaName) - 1, m_pSqlCmd->Field(1).asString());
			CBF_Tools::StringCopy(info.sParaValue, sizeof(info.sParaValue) - 1, m_pSqlCmd->Field(2).asString());
			CBF_Tools::StringCopy(info.sValueType, sizeof(info.sValueType) - 1, m_pSqlCmd->Field(3).asString());
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
	sprintf(m_sSql,"select a.OPER_CODE,a.CHANNEL,a.LOGIN_IP,a.ONLINE_STATUS,a.ONLINE_TIME,a.NODEID,a.NODEPID,a.SVRID,a.SVRPID,a.GATEINDEX,a.TIMESTAMP from sys_oper_online a order by OPER_CODE");
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
			CBF_Tools::StringCopy(info.sOperCode, sizeof(info.sOperCode) - 1, m_pSqlCmd->Field(0).asString());
			CBF_Tools::StringCopy(info.sTermType, sizeof(info.sTermType) - 1, m_pSqlCmd->Field(1).asString());
			CBF_Tools::StringCopy(info.sLoginIp, sizeof(info.sLoginIp) - 1, m_pSqlCmd->Field(2).asString());
			CBF_Tools::StringCopy(info.sOnlineStatus, sizeof(info.sOnlineStatus) - 1, m_pSqlCmd->Field(3).asString());

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
//码表

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
    m_pLog->LogMp(LOG_INFO, __FILE__, __LINE__, "ReadMarket");
    if (!ReadMarket())
    {
        return false;
    }
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"ReadExchange");
	if (!ReadExchange())
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
    m_pLog->LogMp(LOG_INFO, __FILE__, __LINE__, "ReadCodeTable");
    if (!ReadCodeTable())
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
	sprintf(m_sSql,"select VARIETY_NO,VARIETY_CODE,VARIETY_NAME,EXCH_CODE,PROD_TYPE,MIN_CHG_PRICE,MAX_HAND,MIN_HAND,EXCH_UNIT,PRICE_UINT,DELI_DAYS,CURRENCY,BAIL_MODE from sys_variety_info order by VARIETY_NO");
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
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Table sys_variety_info no data");
#else
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"品种表无数据");
#endif
			return false;
		}
		while (bret)
		{
			info.nVarietyId = m_pSqlCmd->Field(0).asLong();
			CBF_Tools::StringCopy(info.sVarietyCode, sizeof(info.sVarietyCode) - 1, m_pSqlCmd->Field(1).asString());
			CBF_Tools::StringCopy(info.sVarietyName, sizeof(info.sVarietyName) - 1, m_pSqlCmd->Field(2).asString());
			CBF_Tools::StringCopy(info.sExcode, sizeof(info.sExcode) - 1, m_pSqlCmd->Field(3).asString());
			CBF_Tools::StringCopy(info.sProd_type, sizeof(info.sProd_type) - 1, m_pSqlCmd->Field(4).asString());
			info.dMinChgPrice = m_pSqlCmd->Field(5).asDouble();
			info.nMaxHand = m_pSqlCmd->Field(6).asLong();
			info.nMinHand = m_pSqlCmd->Field(7).asLong();
			info.dTxUnit = m_pSqlCmd->Field(8).asDouble();
			CBF_Tools::StringCopy(info.sOrderUnit, sizeof(info.sOrderUnit) - 1, m_pSqlCmd->Field(9).asString());
			info.nDeliDays = m_pSqlCmd->Field(11).asLong();
			CBF_Tools::StringCopy(info.sCurrency, sizeof(info.sCurrency) - 1, m_pSqlCmd->Field(11).asString());
			CBF_Tools::StringCopy(info.sBailMode, sizeof(info.sBailMode) - 1, m_pSqlCmd->Field(12).asString());
			
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
	S_MDB_BOND_INFO bond;
	S_MDB_BOND_INFO *pb=NULL;
	//读债券扩展信息
    sprintf(m_sSql, "select BOND_CODE,BOND_TYPE,ISSUE_DATE,LIST_DATE,ISSUE_PRICE,START_DATE,MTR_DATE,COUPON,REMAIN_MATURITY,PAY_FREQ,MARKET_SCOPE,TERM_MATURITY,ISSUE_ORG,EXEC_DATE,EXEC_PRICE,INIT_VALUE,INIT_SPREAD,COUPON_TYPE,INIT_COUPON,FIRST_PAY_DATE,DV01,ACCR_INTE,NEW_VALUE,APPR_VALUE,DAY_COUNT from sys_bond_ext_info order by BOND_CODE");
    try
    {
        if (!m_pSqlCmd->setCommandText(m_sSql))
        {
            m_pLog->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "setCommandText error <%s>", m_sSql);
            return false;
        }
        if (!m_pSqlCmd->Execute())
        {
            m_pLog->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "Execute <%s> error %s", m_sSql, m_pSqlCmd->GetCmdErrInfo());
            return false;
        }

        bret = m_pSqlCmd->FetchNext();
        while (bret)
        {
			CBF_Tools::StringCopy(bond.sBondCode, sizeof(bond.sBondCode) - 1, m_pSqlCmd->Field(0).asString());
			bond.nBondType = m_pSqlCmd->Field(1).asLong();
			CBF_Tools::StringCopy(bond.sIssueDate, sizeof(bond.sIssueDate) - 1, m_pSqlCmd->Field(2).asString());
			CBF_Tools::StringCopy(bond.sListDate, sizeof(bond.sListDate) - 1, m_pSqlCmd->Field(3).asString());
			bond.dIssuePrice = m_pSqlCmd->Field(4).asDouble();
			CBF_Tools::StringCopy(bond.sStartDate, sizeof(bond.sStartDate) - 1, m_pSqlCmd->Field(5).asString());
			CBF_Tools::StringCopy(bond.sMtrDate, sizeof(bond.sMtrDate) - 1, m_pSqlCmd->Field(6).asString());
			bond.dCoupon = m_pSqlCmd->Field(7).asDouble();
			bond.dRemainMaturity = m_pSqlCmd->Field(8).asDouble();
			bond.nPayFreq = m_pSqlCmd->Field(9).asLong();
			bond.dMarketScope = m_pSqlCmd->Field(10).asDouble();
			CBF_Tools::StringCopy(bond.sTermMaturity, sizeof(bond.sTermMaturity) - 1, m_pSqlCmd->Field(11).asString());
			CBF_Tools::StringCopy(bond.sIssueOrg, sizeof(bond.sIssueOrg) - 1, m_pSqlCmd->Field(12).asString());
			CBF_Tools::StringCopy(bond.sExecDate, sizeof(bond.sExecDate) - 1, m_pSqlCmd->Field(13).asString());
			bond.dExecPrice = m_pSqlCmd->Field(14).asDouble();
			bond.dInitValue = m_pSqlCmd->Field(15).asDouble();
			bond.dInitSpread = m_pSqlCmd->Field(16).asDouble();
			bond.nCouponType = m_pSqlCmd->Field(17).asLong();
			bond.dInitCoupon = m_pSqlCmd->Field(18).asDouble();
			CBF_Tools::StringCopy(bond.sFirstPayDate, sizeof(bond.sFirstPayDate) - 1, m_pSqlCmd->Field(19).asString());
			bond.dDv01 = m_pSqlCmd->Field(20).asDouble();
			bond.dAccrInte = m_pSqlCmd->Field(21).asDouble();
			bond.dNewValue = m_pSqlCmd->Field(22).asDouble();
			bond.dApprValue = m_pSqlCmd->Field(23).asDouble();
			bond.nDayCount = m_pSqlCmd->Field(24).asLong();
            if (m_pMemDb->m_pDbBaseinfo.m_pBond.Insert(bond) < 0)
            {
#ifdef _ENGLISH_
                m_pLog->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "bond [%s] repeat", bond.sBondCode);
#else
                m_pLog->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "债券[%s]重复", bond.sBondCode);
#endif
                return false;
            }
            bret = m_pSqlCmd->FetchNext();
		}
        
	}
    catch (...)
    {
#ifdef _ENGLISH_
        m_pLog->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "Database operator Exception!");
#else
        m_pLog->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "数据库操作异常");
#endif
        return false;
    }
	sprintf(m_sSql,"select a.SECU_NO,a.SECU_CODE,a.SECU_NAME,a.EX_CODE,a.PROD_TYPE,a.PROD_CODE,a.PROD_NO,a.MIN_CHG_PRICE,a.IST0,a.ISBAIL,a.BAIL_RATE,a.UL_LIMIT,a.UPPER_LIMIT_VALUE,a.LOWER_LIMIT_VALUE,a.ACTIVE_FLAG,a.TX_STATUS,a.REF_PRICE,a.RCV_RATE,a.BUSI_MODE,a.MAX_HAND,a.MIN_HAND,a.EX_UNIT,a.PRICE_UNIT,a.ORDER_CMD,a.ORDER_ATTR,a.DUE_DATE,a.LAST_DATE,a.DELI_DAYS,a.IS_BIGMARGIN,a.ISCALL,a.VARIETY_NO from sys_secu_info a order by a.SECU_NO");
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
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Table sys_secu_info no data");
#else
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"合约表无数据");
#endif
			return false;
		}
		while (bret)
		{
			info.nContractId = m_pSqlCmd->Field(0).asLong();
			CBF_Tools::StringCopy(info.sContractCode, sizeof(info.sContractCode) - 1,m_pSqlCmd->Field(1).asString());
			CBF_Tools::StringCopy(info.sContractName, sizeof(info.sContractName) - 1, m_pSqlCmd->Field(2).asString());
			CBF_Tools::StringCopy(info.sExcode, sizeof(info.sExcode) - 1, m_pSqlCmd->Field(3).asString());
			CBF_Tools::StringCopy(info.sProd_type, sizeof(info.sProd_type) - 1, m_pSqlCmd->Field(4).asString());
			CBF_Tools::StringCopy(info.sProd_code, sizeof(info.sProd_code) - 1, m_pSqlCmd->Field(5).asString());
			info.nProdId = m_pSqlCmd->Field(6).asLong();
			info.dMinChgPrice = m_pSqlCmd->Field(7).asDouble();
			CBF_Tools::StringCopy(info.sIsT0, sizeof(info.sIsT0) - 1, m_pSqlCmd->Field(8).asString());
			CBF_Tools::StringCopy(info.sIsMargin, sizeof(info.sIsMargin) - 1, m_pSqlCmd->Field(9).asString());
			info.dMarginRate = m_pSqlCmd->Field(10).asDouble();
			CBF_Tools::StringCopy(info.sIsUpDownLimit, sizeof(info.sIsUpDownLimit) - 1, m_pSqlCmd->Field(11).asString());
			info.dUpRate = m_pSqlCmd->Field(12).asDouble();
			info.dDownRate = m_pSqlCmd->Field(13).asDouble();
			CBF_Tools::StringCopy(info.sActiveFlag, sizeof(info.sActiveFlag) - 1, m_pSqlCmd->Field(14).asString());
			CBF_Tools::StringCopy(info.sStatus, sizeof(info.sStatus) - 1, m_pSqlCmd->Field(15).asString());
			info.dRefPrice = m_pSqlCmd->Field(16).asDouble();
			info.dSellGetRate = m_pSqlCmd->Field(17).asDouble();
			CBF_Tools::StringCopy(info.sDeliMode, sizeof(info.sDeliMode) - 1, m_pSqlCmd->Field(18).asString());
			info.nMaxHand = m_pSqlCmd->Field(19).asLong();
			info.nMinHand = m_pSqlCmd->Field(20).asLong();
			info.dTxUnit = m_pSqlCmd->Field(21).asDouble();
			CBF_Tools::StringCopy(info.sOrderUnit, sizeof(info.sOrderUnit) - 1, m_pSqlCmd->Field(22).asString());
			CBF_Tools::StringCopy(info.sOrderCmd, sizeof(info.sOrderCmd) - 1, m_pSqlCmd->Field(23).asString());
			CBF_Tools::StringCopy(info.sDoneAttr, sizeof(info.sDoneAttr) - 1, m_pSqlCmd->Field(24).asString());
			CBF_Tools::StringCopy(info.sDueDate, sizeof(info.sDueDate) - 1, m_pSqlCmd->Field(25).asString());
			CBF_Tools::StringCopy(info.sLastDate, sizeof(info.sLastDate) - 1, m_pSqlCmd->Field(26).asString());
			info.nDeliDays = m_pSqlCmd->Field(27).asLong();
			CBF_Tools::StringCopy(info.sIsBigMargin, sizeof(info.sIsBigMargin) - 1, m_pSqlCmd->Field(28).asString());
			CBF_Tools::StringCopy(info.sIsOptionCall, sizeof(info.sIsOptionCall) - 1, m_pSqlCmd->Field(29).asString());
			info.nVariety = m_pSqlCmd->Field(30).asLong();
			//取债券扩展信息的rowid
			info.nBondRowid = m_pMemDb->m_pDbBaseinfo.m_pBond.GetRowId(info.sContractCode);

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
					info.nVariety = pv->nVarietyId;
					break;
				case PROD_TYPE_OPTION:
				case PROD_TYPE_OPTION_SEC:
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
					info.nVariety = pv->nVarietyId;
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
	sprintf(m_sSql,"select EX_CODE,EX_NAME,EX_TYPE,EX_TRADEDATE,IS_SEAT_TRADE,COMM_PARA,CLEAR_DATE,CLEAR_STATUS,ONLY_CHGDATE from sys_ex_info order by EX_CODE");
	
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
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Table sys_ex_info no data");
#else
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"交易所表无数据");
#endif
			return false;
		}
		while (bret)
		{
			CBF_Tools::StringCopy(info.sExCode, sizeof(info.sExCode) - 1, m_pSqlCmd->Field(0).asString());
			CBF_Tools::StringCopy(info.sExName, sizeof(info.sExName) - 1, m_pSqlCmd->Field(1).asString());
			CBF_Tools::StringCopy(info.sMarket, sizeof(info.sMarket) - 1, m_pSqlCmd->Field(2).asString());
			CBF_Tools::StringCopy(info.sTradeDate, sizeof(info.sTradeDate) - 1, m_pSqlCmd->Field(3).asString());
			CBF_Tools::StringCopy(info.sIsSeatTrade, sizeof(info.sIsSeatTrade) - 1, m_pSqlCmd->Field(4).asString());
			CBF_Tools::StringCopy(info.sCommPara, sizeof(info.sCommPara) - 1, m_pSqlCmd->Field(5).asString());
			CBF_Tools::StringCopy(info.sClearDate, sizeof(info.sClearDate) - 1, m_pSqlCmd->Field(6).asString());
			CBF_Tools::StringCopy(info.sClearStatus, sizeof(info.sClearStatus) - 1, m_pSqlCmd->Field(7).asString());
			CBF_Tools::StringCopy(info.sOnlyChgDate, sizeof(info.sOnlyChgDate) - 1, m_pSqlCmd->Field(8).asString());
			//取对应的市场表rowid
			info.nMarketRowid = m_pMemDb->m_pDbBaseinfo.m_pMarket.GetRowId(info.sMarket);
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
	sprintf(m_sSql,"select MARKET_CODE,MARKET_NAME,TX_DATE,CLEAR_DATE,CLEAR_STATUS,ONLY_CHGDATE from sys_ex_market_info  order by MARKET_CODE");
	
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
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Table sys_ex_market_info no data");
#else
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"市场表表无数据");
#endif
			return false;
		}
		while (bret)
		{
			CBF_Tools::StringCopy(info.sMarket, sizeof(info.sMarket) - 1, m_pSqlCmd->Field(0).asString());
			CBF_Tools::StringCopy(info.sMarketName, sizeof(info.sMarketName) - 1, m_pSqlCmd->Field(1).asString());
			CBF_Tools::StringCopy(info.sTradeDate, sizeof(info.sTradeDate) - 1, m_pSqlCmd->Field(2).asString());
			CBF_Tools::StringCopy(info.sClearDate, sizeof(info.sClearDate) - 1, m_pSqlCmd->Field(3).asString());
			CBF_Tools::StringCopy(info.sClearStatus, sizeof(info.sClearStatus) - 1, m_pSqlCmd->Field(4).asString());
			CBF_Tools::StringCopy(info.sOnlyChgDate, sizeof(info.sOnlyChgDate) - 1, m_pSqlCmd->Field(5).asString());
		
			if (m_pMemDb->m_pDbBaseinfo.m_pMarket.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"sys_ex_market_info [%s] repeat",info.sMarket);
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
	sprintf(m_sSql,"select a.FINA_PROD_NO,a.FINA_PROD_NAME,a.LINKMAN,a.PHONE,a.ADDR,a.ORG_CODE,a.PUBLISH_UNIT from sys_prod_info a ");
	
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
			CBF_Tools::StringCopy(info.sFinaProdCode, sizeof(info.sFinaProdCode) - 1, m_pSqlCmd->Field(0).asString());
			CBF_Tools::StringCopy(info.sFinaProdName, sizeof(info.sFinaProdName) - 1, m_pSqlCmd->Field(1).asString());
			CBF_Tools::StringCopy(info.sLinkMan, sizeof(info.sLinkMan) - 1, m_pSqlCmd->Field(2).asString());
			CBF_Tools::StringCopy(info.sPhone, sizeof(info.sPhone) - 1, m_pSqlCmd->Field(3).asString());
			CBF_Tools::StringCopy(info.sAddr, sizeof(info.sAddr) - 1, m_pSqlCmd->Field(4).asString());
			CBF_Tools::StringCopy(info.sOrgCode, sizeof(info.sOrgCode) - 1, m_pSqlCmd->Field(5).asString());
			info.nPublishUit = m_pSqlCmd->Field(6).asLong();
			if (m_pMemDb->m_pDbBaseinfo.m_pProd.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"sys_prod_info [%s] repeat",info.sFinaProdCode);
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
	sprintf(m_sSql,"select a.OPER_CODE,a.TX_ACC from sys_oper_tradeacc a ");
	
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
			CBF_Tools::StringCopy(info.sOperCode, sizeof(info.sOperCode) - 1, m_pSqlCmd->Field(0).asString());
			CBF_Tools::StringCopy(info.sTxAcc, sizeof(info.sTxAcc) - 1, m_pSqlCmd->Field(1).asString());
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
	sprintf(m_sSql,"select a.PROD_NO,a.TX_ACC from sys_prod_tradeacc a ");
	
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
			CBF_Tools::StringCopy(info.sProdNo, sizeof(info.sProdNo) - 1, m_pSqlCmd->Field(0).asString());
			CBF_Tools::StringCopy(info.sTxAcc, sizeof(info.sTxAcc) - 1, m_pSqlCmd->Field(1).asString());
			if (m_pMemDb->m_pDbBaseinfo.m_pProdCust.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"sys_prod_tradeacc [%s %s] repeat",info.sProdNo,info.sTxAcc);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"产品交易账号对应表 [%s %s]重复",info.sProdNo,info.sTxAcc);
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
	sprintf(m_sSql,"select a.OPER_CODE,a.FINA_PROD_ID from sys_oper_prod a ");
	
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
			CBF_Tools::StringCopy(info.sOper, sizeof(info.sOper) - 1, m_pSqlCmd->Field(0).asString());
			CBF_Tools::StringCopy(info.sProdNo, sizeof(info.sProdNo) - 1, m_pSqlCmd->Field(1).asString());
			if (m_pMemDb->m_pDbBaseinfo.m_pOperProd.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"sys_oper_prod [%s %s] repeat",info.sOper,info.sProdNo);
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
	sprintf(m_sSql,"select a.TX_ACC,a.SECU_CODE,a.FORCERATE from risk_nonactive_secu_set a ");
	
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
			CBF_Tools::StringCopy(info.sTxAcc, sizeof(info.sTxAcc) - 1, m_pSqlCmd->Field(0).asString());
			CBF_Tools::StringCopy(info.sContractCode,sizeof(info.sContractCode)-1,m_pSqlCmd->Field(0).asString());
			info.dForceRate = m_pSqlCmd->Field(2).asDouble();
			if (m_pMemDb->m_pDbBaseinfo.m_pCustContractLimit.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"risk_nonactive_secu_set [%s %s] repeat",info.sTxAcc,info.sContractCode);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"冷门合约开仓限制表 [%s %s]重复",info.sTxAcc,info.sContractCode);
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
	sprintf(m_sSql,"select a.EXCH_DATE,a.CONTRACT_NO,a.CONTRACT_CODE,a.SETTLE_PRICE,a.HOLD_POSI,a.CLOSE_PRICE,a.PRICE_CHANGE,a.SIMPLE_RETURN,a.LOG_RETURN,a.HIGH,a.LOW,a.OPEN from sys_secu_settle_price a,sys_secu_info b ,sys_ex_info c where a.CONTRACT_NO = b.SECU_NO and b.EX_CODE=c.EX_CODE and c.CLEAR_DATE=a.EXCH_DATE");
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
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"sql[%s] Table sys_secu_settle_price no data",m_sSql);
#else
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"合约结算价表无数据 sql[%s] ",m_sSql);
#endif
			//return false;
		}
		while (bret)
		{
			CBF_Tools::StringCopy(info.sClearDate, sizeof(info.sClearDate) - 1, m_pSqlCmd->Field(0).asString());
			info.nContractNo = m_pSqlCmd->Field(1).asLong();
			CBF_Tools::StringCopy(info.sContractCode, sizeof(info.sContractCode) - 1, m_pSqlCmd->Field(2).asString());
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
	sprintf(m_sSql,"select SPEC_DATE,MARKET_CODE,EX_CODE,DATE_TYPE from sys_spec_date where SPEC_DATE >=:1  ");
	
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
			m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"Table sys_spec_date no data");
#else
			m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"节假日表无数据 ");
#endif
			return true;
		}
		while (bret)
		{
			CBF_Tools::StringCopy(info.sSpecDate, sizeof(info.sSpecDate) - 1, m_pSqlCmd->Field(0).asString());
			CBF_Tools::StringCopy(info.sMarket, sizeof(info.sMarket) - 1, m_pSqlCmd->Field(1).asString());
			CBF_Tools::StringCopy(info.sExchCode, sizeof(info.sExchCode) - 1, m_pSqlCmd->Field(2).asString());
			CBF_Tools::StringCopy(info.sDateType, sizeof(info.sDateType) - 1, m_pSqlCmd->Field(3).asString());

			if (m_pMemDb->m_pDbBaseinfo.m_pSpecDate.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"sys_spec_date [%s] repeat",info.sSpecDate);
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
// 	CTbl_Cust_TX_Acc  m_pCustFundAcc;//交易账号信息表
	//客户信息表
	if (!ReadCustom())
	{
		return false;
	}
	//交易账号信息表
	if (!ReadTradeAcc())
	{
		return false;
	}
	return true;
}

bool CInitTrade::ReadCustom()
{
	bool bret;
	
	S_MDB_CUSTINFO info;
	sprintf(m_sSql,"select CUST_NO,CUST_NAME,CUST_TYPE,CUST_CERT,CUST_CERT_NO,CUST_PHONE,TRADE_PWD,FUND_PWD,LAST_MODIPWDTIME,PWD_FAILDNUM,TXCHANNLE,PKI_CERT_SERIAL,PKI_CERT_DATE,CUST_STATUS,LAST_PWDFAILDATE,LAST_PWDFAILTIME from sys_custom_info order by CUST_NO");
	
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
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Table sys_custom_info no data");
#else
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"客户表CUST_BASIC_INFO无数据 ");
#endif
			return false;
		}
		while (bret)
		{
			CBF_Tools::StringCopy(info.sCustNo, sizeof(info.sCustNo) - 1, m_pSqlCmd->Field(0).asString());
			CBF_Tools::StringCopy(info.sCustName, sizeof(info.sCustName) - 1, m_pSqlCmd->Field(1).asString());
			CBF_Tools::StringCopy(info.sCustType, sizeof(info.sCustType) - 1, m_pSqlCmd->Field(2).asString());
			CBF_Tools::StringCopy(info.sCertType, sizeof(info.sCertType) - 1, m_pSqlCmd->Field(3).asString());
			if (strlen(info.sCertType)<1) //若没有证件类型，默认身份证
			{
				info.sCertType[0] = CERT_TYPE_IDCARD;
				info.sCertType[1] =0; 
			}
			CBF_Tools::StringCopy(info.sCertNo, sizeof(info.sCertNo) - 1, m_pSqlCmd->Field(4).asString());
			if (strlen(info.sCertNo)<1)//若没有证件号，用客户号
			{
				strcpy(info.sCertNo,info.sCustNo);
			}
			CBF_Tools::StringCopy(info.sMobile, sizeof(info.sMobile) - 1, m_pSqlCmd->Field(5).asString());
			if (strlen(info.sMobile)<1)//若没有手机号，用客户号
			{
				strcpy(info.sMobile,info.sCustNo);
			}
			CBF_Tools::StringCopy(info.sTradePwd, sizeof(info.sTradePwd) - 1, m_pSqlCmd->Field(6).asString());
			CBF_Tools::StringCopy(info.sFundPwd, sizeof(info.sFundPwd) - 1, m_pSqlCmd->Field(7).asString());
			CBF_Tools::StringCopy(info.sLastModiPwdTime, sizeof(info.sLastModiPwdTime) - 1, m_pSqlCmd->Field(8).asString());
			info.nPwdFaildNum = m_pSqlCmd->Field(9).asLong();
			CBF_Tools::StringCopy(info.sChannleOpen, sizeof(info.sChannleOpen) - 1, m_pSqlCmd->Field(10).asString());
			CBF_Tools::StringCopy(info.sPkiCertNo, sizeof(info.sPkiCertNo) - 1, m_pSqlCmd->Field(11).asString());
			CBF_Tools::StringCopy(info.sPkiCertDate, sizeof(info.sPkiCertDate) - 1, m_pSqlCmd->Field(12).asString());
			CBF_Tools::StringCopy(info.sStatus, sizeof(info.sStatus) - 1, m_pSqlCmd->Field(13).asString());
			CBF_Tools::StringCopy(info.sLastPwdFaildDate, sizeof(info.sLastPwdFaildDate) - 1, m_pSqlCmd->Field(14).asString());
			info.nLastPwdFaildTime = m_pSqlCmd->Field(15).asLong();

			if (m_pMemDb->m_pDbCust.m_pCustom.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"sys_custom_info [%s] repeat",info.sCustNo);
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

bool CInitTrade::ReadTradeAcc()
{
	bool bret;
	int rid;
	S_MDB_TRADEACC_INFO info;
	bzero(&info,sizeof(S_MDB_TRADEACC_INFO));
	sprintf(m_sSql,"select TX_ACC,CUST_NO,ACC_TYPE,ACC_NAME,ACC_SIMPLENAME,SIGN_ORG,MANAGER_ORG,FUTURES_FLAG,ACC_LEVEL,EX_FEE,MEMBER_FEE,AFFIRM_FLAG,AFFIRM_DATE,ADD_DATETIME,ADD_TERM,ADD_OPER,MODI_DATETIME,MODI_TERM,MODI_OPER,BROKERAGE_CODE,CURRENCY,PARENT_TXACC,ACC_STATUS from sys_tradeacc_info where ACC_STATUS='0' ");
	
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
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Table sys_tradeacc_info no data");
#else
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"资金账号sys_tradeacc_info表无数据 ");
#endif
			return false;
		}
		while (bret)
		{
			CBF_Tools::StringCopy(info.sTxAcc, sizeof(info.sTxAcc) - 1, m_pSqlCmd->Field(0).asString());
			CBF_Tools::StringCopy(info.sCustNo, sizeof(info.sCustNo) - 1, m_pSqlCmd->Field(1).asString());
			CBF_Tools::StringCopy(info.sAcctype, sizeof(info.sAcctype) - 1, m_pSqlCmd->Field(2).asString());
			CBF_Tools::StringCopy(info.sAccName, sizeof(info.sAccName) - 1, m_pSqlCmd->Field(3).asString());
			CBF_Tools::StringCopy(info.sCustSimpleName, sizeof(info.sCustSimpleName) - 1, m_pSqlCmd->Field(4).asString());
			CBF_Tools::StringCopy(info.sSignOrg, sizeof(info.sSignOrg) - 1, m_pSqlCmd->Field(5).asString());
			CBF_Tools::StringCopy(info.sMgrOrg, sizeof(info.sMgrOrg) - 1, m_pSqlCmd->Field(6).asString());
			CBF_Tools::StringCopy(info.sFutureFlag, sizeof(info.sFutureFlag) - 1, m_pSqlCmd->Field(7).asString());
			CBF_Tools::StringCopy(info.sAccLevel, sizeof(info.sAccLevel) - 1, m_pSqlCmd->Field(8).asString());
			CBF_Tools::StringCopy(info.sExchFareModel, sizeof(info.sExchFareModel) - 1, m_pSqlCmd->Field(9).asString());
			CBF_Tools::StringCopy(info.sMemberFareModel, sizeof(info.sMemberFareModel) - 1, m_pSqlCmd->Field(10).asString());
			CBF_Tools::StringCopy(info.sAffirmFlag, sizeof(info.sAffirmFlag) - 1, m_pSqlCmd->Field(11).asString());
			CBF_Tools::StringCopy(info.sAffirmDate, sizeof(info.sAffirmDate) - 1, m_pSqlCmd->Field(12).asString());
			CBF_Tools::StringCopy(info.sAddDateTime, sizeof(info.sAddDateTime) - 1, m_pSqlCmd->Field(13).asString());
			CBF_Tools::StringCopy(info.sAddTerm, sizeof(info.sAddTerm) - 1, m_pSqlCmd->Field(14).asString());
			CBF_Tools::StringCopy(info.sAddOper, sizeof(info.sAddOper) - 1, m_pSqlCmd->Field(15).asString());
            CBF_Tools::StringCopy(info.sModiDateTime, sizeof(info.sModiDateTime) - 1, m_pSqlCmd->Field(16).asString());
            CBF_Tools::StringCopy(info.sModiTerm, sizeof(info.sModiTerm) - 1, m_pSqlCmd->Field(17).asString());
            CBF_Tools::StringCopy(info.sModiOper, sizeof(info.sModiOper) - 1, m_pSqlCmd->Field(18).asString());
			info.nSysId = m_pSqlCmd->Field(19).asLong();
			CBF_Tools::StringCopy(info.sCurrency, sizeof(info.sCurrency) - 1, m_pSqlCmd->Field(20).asString());
			CBF_Tools::StringCopy(info.sParentTxAcc, sizeof(info.sParentTxAcc) - 1, m_pSqlCmd->Field(21).asString());
			CBF_Tools::StringCopy(info.sAccStatus, sizeof(info.sAccStatus) - 1, m_pSqlCmd->Field(22).asString());

			rid = m_pMemDb->m_pDbCust.m_pTradeAcc.Insert(info);
			if (rid <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"sys_tradeacc_info [%s] repeat",info.sTxAcc);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"交易账号sys_tradeacc_info表 [%s]重复",info.sTxAcc);
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

	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"ReadProdOrgTemplet");
	if (!ReadProdOrgTemplet())
	{
		return false;
	}
    m_pLog->LogMp(LOG_INFO, __FILE__, __LINE__, "ReadBondBwg");
    if (!ReadBondBwg())
    {
        return false;
    }
    m_pLog->LogMp(LOG_INFO, __FILE__, __LINE__, "ReadPriceLimit");
    if (!ReadPriceLimit())
    {
        return false;
    }
	return true;
}
bool CInitTrade::ReadProdOrgTemplet()
{
	bool bret;

	S_MDB_PRODORG_TEMPLET_RELA info;
	sprintf(m_sSql,"select ORG_CODE,FUND_TEMPLET_ID,POSI_TEMPLET_ID from  risk_organ_templet_rela");
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
			m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"Table risk_organ_templet_rela no data");
#else
			m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"机构模板表risk_organ_templet_rela 表无数据 ");
#endif
			//return false;
		}
		while (bret)
		{
			
			CBF_Tools::StringCopy(info.sProdOrg, sizeof(info.sProdOrg) - 1, m_pSqlCmd->Field(0).asString());
			CBF_Tools::StringCopy(info.sFundTempletId, sizeof(info.sFundTempletId) - 1, m_pSqlCmd->Field(1).asString());
			CBF_Tools::StringCopy(info.sPosiTempletId, sizeof(info.sPosiTempletId) - 1, m_pSqlCmd->Field(2).asString());
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
	sprintf(m_sSql,"select FINA_PROD_NO,FUND_TEMPLET_ID,POSI_TEMPLET_ID from  risk_prod_templet_rela");
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
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Table risk_prod_templet_rela no data");
#else
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"产品模板表 risk_prod_templet_rela 表无数据 ");
#endif
			//return false;
		}
		while (bret)
		{

			CBF_Tools::StringCopy(info.sProdOrg, sizeof(info.sProdOrg) - 1, m_pSqlCmd->Field(0).asString());
			CBF_Tools::StringCopy(info.sFundTempletId, sizeof(info.sProdOrg) - 1, m_pSqlCmd->Field(1).asString());
			CBF_Tools::StringCopy(info.sPosiTempletId, sizeof(info.sProdOrg) - 1, m_pSqlCmd->Field(2).asString());
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
	//读汇率配置表  	 计算外币资产用
	S_MDB_CURRENCY_RATE rate;
	sprintf(m_sSql,"select CURRENCY,RATE from  risk_currency_rate");
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
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Table risk_currency_rate no data");
#else
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"汇率配置表 risk_currency_rate 表无数据 ");
#endif
			//return false;
		}
		while (bret)
		{

			CBF_Tools::StringCopy(rate.sCurrency, sizeof(rate.sCurrency) - 1, m_pSqlCmd->Field(0).asString());
			rate.dRate =m_pSqlCmd->Field(1).asDouble();
			
			if (m_pMemDb->m_pDbSet.m_pCurrencyRate.Insert(rate) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"risk_currency_rate [%s] repeat",rate.sCurrency);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"汇率配置表 risk_currency_rate [%s]重复",rate.sCurrency);
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
	sprintf(m_sSql,"select WARN_NO,WARN_NAME,WARN_COLOR,WARN_SOUND,WARN_SEP from risk_early_warning_level order by WARN_NO");
	
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
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Table risk_early_warning_level no data");
#else
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"预警级别risk_early_warning_level 表无数据 ");
#endif
			return false;
		}
		while (bret)
		{
			info.nWarnLevel = m_pSqlCmd->Field(0).asLong();
			CBF_Tools::StringCopy(info.sWarnName, sizeof(info.sWarnName) - 1, m_pSqlCmd->Field(1).asString());
			CBF_Tools::StringCopy(info.sWarnColor, sizeof(info.sWarnColor) - 1, m_pSqlCmd->Field(2).asString());
			CBF_Tools::StringCopy(info.sWarnSound, sizeof(info.sWarnSound) - 1, m_pSqlCmd->Field(3).asString());
			info.nWarnSep= m_pSqlCmd->Field(4).asLong();
		
			if (m_pMemDb->m_pDbSet.m_pWarnLevel.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"risk_early_warning_level [%d] repeat",info.nWarnLevel);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"预警级别risk_early_warning_level表 [%d]重复",info.nWarnLevel);
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
	sprintf(m_sSql,"select VARIETY_NO,SECU_NO,SECU_CODE,WARN_ITEM,RISK_CONDITION,WARN_VALUE from risk_risk_secu_set ");
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
			CBF_Tools::StringCopy(info.sContractCode, sizeof(info.sContractCode) - 1, m_pSqlCmd->Field(2).asString());
			CBF_Tools::StringCopy(info.sWarnItem, sizeof(info.sWarnItem) - 1, m_pSqlCmd->Field(3).asString());
			CBF_Tools::StringCopy(info.sCondition, sizeof(info.sCondition) - 1, m_pSqlCmd->Field(4).asString());
			info.dValue = m_pSqlCmd->Field(5).asDouble();
			
			
			if (m_pMemDb->m_pDbSet.m_pRiskContractSet.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"risk_risk_secu_set [%d] repeat",info.nVarietyNo);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"风险合约设置risk_risk_secu_set表 [%d]重复",info.nVarietyNo);
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
	sprintf(m_sSql,"select TEMPLET_ID,WARN_NO,FUND_MODEL,FUND_CONDITION,FUND_VALUE,TRADE_RIGHT from risk_prod_fund_model_set ");
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
			CBF_Tools::StringCopy(info.sTempletId, sizeof(info.sTempletId) - 1, m_pSqlCmd->Field(0).asString());
			info.nWarnNo = m_pSqlCmd->Field(1).asLong();
			CBF_Tools::StringCopy(info.sModel, sizeof(info.sModel) - 1, m_pSqlCmd->Field(2).asString());
			CBF_Tools::StringCopy(info.sCondition, sizeof(info.sCondition) - 1, m_pSqlCmd->Field(3).asString());
			info.dValue = m_pSqlCmd->Field(4).asDouble();
			CBF_Tools::StringCopy(info.sRight, sizeof(info.sRight) - 1, m_pSqlCmd->Field(5).asString());
			
			if (m_pMemDb->m_pDbSet.m_pProdFundMonitor.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"risk_prod_fund_model_set [%d] repeat",info.nWarnNo);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"产品资金监控risk_prod_fund_model_set [%d]重复",info.nWarnNo);
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
	sprintf(m_sSql,"select TEMPLET_ID,WARN_NO,POSI_MODEL,POSI_CONDITION,POSI_VALUE,TRADE_RIGHT from risk_prod_posi_model_set ");
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
			CBF_Tools::StringCopy(info.sTempletId, sizeof(info.sTempletId) - 1, m_pSqlCmd->Field(0).asString());
			info.nWarnNo = m_pSqlCmd->Field(1).asLong();
			CBF_Tools::StringCopy(info.sModel, sizeof(info.sModel) - 1, m_pSqlCmd->Field(2).asString());
			CBF_Tools::StringCopy(info.sCondition, sizeof(info.sCondition) - 1, m_pSqlCmd->Field(3).asString());
			info.dValue = m_pSqlCmd->Field(4).asDouble();
			CBF_Tools::StringCopy(info.sRight, sizeof(info.sRight) - 1, m_pSqlCmd->Field(5).asString());
			
			if (m_pMemDb->m_pDbSet.m_pProdPosiMonitor.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"risk_prod_posi_model_set [%d] repeat",info.nWarnNo);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"产品持仓监控risk_prod_posi_model_set [%d]重复",info.nWarnNo);
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
	sprintf(m_sSql,"select TEMPLET_ID,TEMPLET_NAME,ORG_CODE,SUB_CAN_USE from risk_fund_templet ");
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
			CBF_Tools::StringCopy(info.sTempletId , sizeof(info.sTempletId) - 1, m_pSqlCmd->Field(0).asString());
			CBF_Tools::StringCopy(info.sTempletName, sizeof(info.sTempletName) - 1, m_pSqlCmd->Field(1).asString());
			CBF_Tools::StringCopy(info.sOrgCode, sizeof(info.sOrgCode) - 1, m_pSqlCmd->Field(2).asString());
			CBF_Tools::StringCopy(info.sSubCanUse, sizeof(info.sSubCanUse) - 1, m_pSqlCmd->Field(3).asString());
			if (m_pMemDb->m_pDbSet.m_pFundTemplet.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"risk_fund_templet [%s] repeat",info.sTempletId);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"资金模板表risk_fund_templet [%s]重复",info.sTempletId);
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
	sprintf(m_sSql,"select TEMPLET_ID,TEMPLET_NAME,ORG_CODE,SUB_CAN_USE from risk_posi_templet ");
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
			CBF_Tools::StringCopy(info.sTempletId , sizeof(info.sTempletId) - 1, m_pSqlCmd->Field(0).asString());
			CBF_Tools::StringCopy(info.sTempletName, sizeof(info.sTempletName) - 1, m_pSqlCmd->Field(1).asString());
            CBF_Tools::StringCopy(info.sOrgCode, sizeof(info.sOrgCode) - 1, m_pSqlCmd->Field(2).asString());
            CBF_Tools::StringCopy(info.sSubCanUse, sizeof(info.sSubCanUse) - 1, m_pSqlCmd->Field(3).asString());
			if (m_pMemDb->m_pDbSet.m_pPosiTemplet.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"risk_posi_templet [%s] repeat",info.sTempletId);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"持仓模板表 risk_posi_templet [%s]重复",info.sTempletId);
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
	sprintf(m_sSql,"select a.TEMPLET_ID,b.TEMPLET_NAME,a.WARN_NO,a.FUND_MODEL,a.FUND_CONDITION,a.FUND_VALUE,a.TRADE_RIGHT from risk_tradeacc_fund_model_set a,risk_fund_templet b  where a.TEMPLET_ID=b.TEMPLET_ID ");
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
			CBF_Tools::StringCopy(info.sFundTempletId , sizeof(info.sFundTempletId) - 1, m_pSqlCmd->Field(0).asString());
			CBF_Tools::StringCopy(info.sFundTempletName, sizeof(info.sFundTempletName) - 1, m_pSqlCmd->Field(1).asString());
			info.nWarnNo = m_pSqlCmd->Field(2).asLong();
			CBF_Tools::StringCopy(info.sFundModel, sizeof(info.sFundModel) - 1, m_pSqlCmd->Field(3).asString());
			CBF_Tools::StringCopy(info.sCondition, sizeof(info.sCondition) - 1, m_pSqlCmd->Field(4).asString());
			info.dValue = m_pSqlCmd->Field(5).asDouble();
			CBF_Tools::StringCopy(info.sRight, sizeof(info.sRight) - 1, m_pSqlCmd->Field(6).asString());
			if (m_pMemDb->m_pDbSet.m_pFundTempletDetail.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"risk_tradeacc_fund_model_set [%s] repeat",info.sFundTempletId);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"客户资金模板设置 risk_tradeacc_fund_model_set [%s]重复",info.sFundTempletId);
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
	sprintf(m_sSql,"select a.TEMPLET_ID,b.TEMPLET_NAME,a.WARN_NO,a.EX_CODE,a.PROD_TYPE,a.VARIETY_NO,a.VARIETY_CODE,a.OPTION_DESTNO,a.OPTION_DESTCODE,a.SECU_NO,a.SECU_CODE,a.POSI_MODEL,a.POSI_CONDITION,a.POSI_VALUE,a.TRADE_RIGHT from risk_tradeacc_posi_model_set a,risk_posi_templet b where a.TEMPLET_ID=b.TEMPLET_ID    ");
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
			CBF_Tools::StringCopy(info.sPosiTempletId , sizeof(info.sPosiTempletId) - 1, m_pSqlCmd->Field(0).asString());
			CBF_Tools::StringCopy(info.sPosiTempletName, sizeof(info.sPosiTempletName) - 1, m_pSqlCmd->Field(1).asString());
			info.nWarnNo = m_pSqlCmd->Field(2).asLong();
			CBF_Tools::StringCopy(info.sExchCode, sizeof(info.sExchCode) - 1, m_pSqlCmd->Field(3).asString());
			CBF_Tools::StringCopy(info.sProdType, sizeof(info.sProdType) - 1, m_pSqlCmd->Field(4).asString());
			info.nVarietyNo = m_pSqlCmd->Field(5).asLong();
			CBF_Tools::StringCopy(info.sVarietyCode, sizeof(info.sVarietyCode) - 1, m_pSqlCmd->Field(6).asString());
			info.nOption_desno = m_pSqlCmd->Field(7).asLong();
			CBF_Tools::StringCopy(info.sOption_destcode, sizeof(info.sOption_destcode) - 1, m_pSqlCmd->Field(8).asString());
			info.nContractNo = m_pSqlCmd->Field(9).asLong();
			CBF_Tools::StringCopy(info.sContractCode, sizeof(info.sContractCode) - 1, m_pSqlCmd->Field(10).asString());
			CBF_Tools::StringCopy(info.sPosiModel, sizeof(info.sPosiModel) - 1, m_pSqlCmd->Field(11).asString());
			CBF_Tools::StringCopy(info.sCondition, sizeof(info.sCondition) - 1, m_pSqlCmd->Field(12).asString());
			info.dValue = m_pSqlCmd->Field(13).asDouble();
			CBF_Tools::StringCopy(info.sRight, sizeof(info.sRight) - 1, m_pSqlCmd->Field(14).asString());
			if (m_pMemDb->m_pDbSet.m_pPosiTempletDetail.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"risk_tradeacc_posi_model_set [%s] repeat",info.sPosiTempletId);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"客户持仓模板设置 risk_tradeacc_posi_model_set [%s]重复",info.sPosiTempletId);
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
	sprintf(m_sSql,"select a.TX_ACC,a.FUND_TEMPLET_ID,a.POSI_TEMPLET_ID from risk_tradeacc_templet_rela a ");
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
			CBF_Tools::StringCopy(info.sTxAcc , sizeof(info.sTxAcc) - 1, m_pSqlCmd->Field(0).asString());
			CBF_Tools::StringCopy(info.sFundTempletId, sizeof(info.sFundTempletId) - 1, m_pSqlCmd->Field(1).asString());
			CBF_Tools::StringCopy(info.sPosiTempletId, sizeof(info.sPosiTempletId) - 1, m_pSqlCmd->Field(2).asString());
			
			if (m_pMemDb->m_pDbSet.m_pCustTemplet.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"risk_tradeacc_templet_rela [%s] repeat",info.sTxAcc);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"交易账号模板对应 risk_tradeacc_templet_rela [%s]重复",info.sTxAcc);
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
	sprintf(m_sSql,"select FEE_MODEL_CODE,MODEL_NAME,MODEL_TYPE,ORG,CR_CHANNEL,CR_DT,CR_OPER,MODI_CHANNEL,MODI_DT,MODI_OPER,SUB_CAN_USE from t_fee_templet_def ");
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
			CBF_Tools::StringCopy(info.sFareModeCode , sizeof(info.sFareModeCode) - 1, m_pSqlCmd->Field(0).asString());
			CBF_Tools::StringCopy(info.sFareModeName, sizeof(info.sFareModeName) - 1, m_pSqlCmd->Field(1).asString());
			CBF_Tools::StringCopy(info.sFareType, sizeof(info.sFareType) - 1, m_pSqlCmd->Field(2).asString());
			CBF_Tools::StringCopy(info.sOrgCode, sizeof(info.sOrgCode) - 1, m_pSqlCmd->Field(3).asString());
            CBF_Tools::StringCopy(info.sCR_Channel, sizeof(info.sCR_Channel) - 1, m_pSqlCmd->Field(4).asString());
            CBF_Tools::StringCopy(info.sCR_Date, sizeof(info.sCR_Date) - 1, m_pSqlCmd->Field(5).asString());
            CBF_Tools::StringCopy(info.sCR_Oper, sizeof(info.sCR_Oper) - 1, m_pSqlCmd->Field(6).asString());
            CBF_Tools::StringCopy(info.sModi_Channel, sizeof(info.sModi_Channel) - 1, m_pSqlCmd->Field(7).asString());
            CBF_Tools::StringCopy(info.sModi_Date, sizeof(info.sModi_Date) - 1, m_pSqlCmd->Field(8).asString());
            CBF_Tools::StringCopy(info.sModi_Oper, sizeof(info.sModi_Oper) - 1, m_pSqlCmd->Field(9).asString());
			CBF_Tools::StringCopy(info.sSubCanUse, sizeof(info.sSubCanUse) - 1, m_pSqlCmd->Field(10).asString());
			if (m_pMemDb->m_pDbSet.m_pFeeTemplet.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"t_fee_templet_def [%s] repeat",info.sFareModeCode);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"费率模板表 t_fee_templet_def [%s]重复",info.sFareModeCode);
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
	sprintf(m_sSql,"select a.FEE_MODEL_CODE,a.PROD_TYPE,a.VARIETY_NO,a.VARIETY_CODE,a.SECU_NO,a.SECU_CODE,a.FEE_NO,a.FEE_MODE,a.FEE_VALUE from t_feerate_detail a ");
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
			CBF_Tools::StringCopy(info.sFareModeCode , sizeof(info.sFareModeCode) - 1, m_pSqlCmd->Field(0).asString());
			CBF_Tools::StringCopy(info.sProdType, sizeof(info.sProdType) - 1, m_pSqlCmd->Field(1).asString());
			info.nVarietyNo  = m_pSqlCmd->Field(2).asLong();
			CBF_Tools::StringCopy(info.sVarietyCode, sizeof(info.sVarietyCode) - 1, m_pSqlCmd->Field(3).asString());
			info.nContractNo  = m_pSqlCmd->Field(4).asLong();
			CBF_Tools::StringCopy(info.sContractCode, sizeof(info.sContractCode) - 1, m_pSqlCmd->Field(5).asString());
			CBF_Tools::StringCopy(info.sFareNo, sizeof(info.sFareNo) - 1, m_pSqlCmd->Field(6).asString());
			CBF_Tools::StringCopy(info.sFareMode, sizeof(info.sFareMode) - 1, m_pSqlCmd->Field(7).asString());
			info.dValue = m_pSqlCmd->Field(8).asDouble();

			if (m_pMemDb->m_pDbSet.m_pFeeTempletDetail.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"t_feerate_detail [%s] repeat",info.sFareModeCode);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"费率模板设置表 t_feerate_detail [%s]重复",info.sFareModeCode);
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
	sprintf(m_sSql,"select a.TX_ACC,a.PROD_TYPE,a.VARIETY_NO,a.VARIETY_CODE,a.SECU_NO,a.SECU_CODE,a.FEE_NO,a.FEE_MODE,a.FEE_VALUE,a.EX_INCLUDE from t_special_feerate a ");
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
			CBF_Tools::StringCopy(info.sTxAcc , sizeof(info.sTxAcc) - 1, m_pSqlCmd->Field(0).asString());
			CBF_Tools::StringCopy(info.sProdType, sizeof(info.sProdType) - 1, m_pSqlCmd->Field(1).asString());
			info.nVarietyNo  = m_pSqlCmd->Field(2).asLong();
			CBF_Tools::StringCopy(info.sVarietyCode, sizeof(info.sVarietyCode) - 1, m_pSqlCmd->Field(3).asString());
			info.nContractNo  = m_pSqlCmd->Field(4).asLong();
			CBF_Tools::StringCopy(info.sContractCode, sizeof(info.sContractCode) - 1, m_pSqlCmd->Field(5).asString());
			CBF_Tools::StringCopy(info.sFareNo, sizeof(info.sFareNo) - 1, m_pSqlCmd->Field(6).asString());
			CBF_Tools::StringCopy(info.sFareMode, sizeof(info.sFareMode) - 1, m_pSqlCmd->Field(7).asString());
			info.dValue = m_pSqlCmd->Field(8).asDouble();
			CBF_Tools::StringCopy(info.sInclude, sizeof(info.sInclude) - 1, m_pSqlCmd->Field(9).asString());

			if (m_pMemDb->m_pDbSet.m_pCustFeeDetail.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"t_special_feerate [%s] repeat",info.sTxAcc);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"客户特殊费率设置表 t_special_feerate [%s]重复",info.sTxAcc);
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
bool CInitTrade::ReadBondBwg()
{
    bool bret;

    S_MDB_BOND_BWG_INFO info;
    sprintf(m_sSql, "select a.COUNTERPARTY_ACC,a.BWG_FLAG,a.COUNTERPARTY_ORG,a.COUNTERPARTY_NAME,a.ADD_DATETIME,a.ADD_OPER from risk_bond_bwg_info a ");
    try
    {
        if (!m_pSqlCmd->setCommandText(m_sSql))
        {
            m_pLog->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "setCommandText error <%s>", m_sSql);
            return false;
        }
        if (!m_pSqlCmd->Execute())
        {
            m_pLog->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "Execute <%s> error %s", m_sSql, m_pSqlCmd->GetCmdErrInfo());
            return false;
        }

        bret = m_pSqlCmd->FetchNext();
        while (bret)
        {
            CBF_Tools::StringCopy(info.sMatchAcc, sizeof(info.sMatchAcc) - 1, m_pSqlCmd->Field(0).asString());
            CBF_Tools::StringCopy(info.sBwgFlag, sizeof(info.sBwgFlag) - 1, m_pSqlCmd->Field(1).asString());
            CBF_Tools::StringCopy(info.sMatchOrg, sizeof(info.sMatchOrg) - 1, m_pSqlCmd->Field(2).asString());
            CBF_Tools::StringCopy(info.sMatchName, sizeof(info.sMatchName) - 1, m_pSqlCmd->Field(3).asString());
            CBF_Tools::StringCopy(info.sAddDateTime, sizeof(info.sAddDateTime) - 1, m_pSqlCmd->Field(4).asString());
            CBF_Tools::StringCopy(info.sAddOper, sizeof(info.sAddOper) - 1, m_pSqlCmd->Field(5).asString());

            if (m_pMemDb->m_pDbSet.m_pBondBwgInfo.Insert(info) < 0)
            {
#ifdef _ENGLISH_
                m_pLog->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "risk_bond_bwg_info [%s] repeat", info.sMatchAcc);
#else
                m_pLog->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "黑白灰名单表 risk_bond_bwg_info [%s]重复", info.sMatchAcc);
#endif
                return false;
            }
            bret = m_pSqlCmd->FetchNext();
        }
    }
    catch (...)
    {
#ifdef _ENGLISH_
        m_pLog->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "Database operator Exception!");
#else
        m_pLog->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "数据库操作异常");
#endif
        return false;
    }
    return true;
}

bool CInitTrade::ReadPriceLimit()
{
    bool bret;

    S_MDB_PRICE_LIMIT info;
    sprintf(m_sSql, "select a.PROD_TYPE,a.SECU_NO,a.SECU_CODE,a.LIMIT_TYPE,a.UP_LIMIT,a.DOWN_LIMIT from risk_price_limit a ");
    try
    {
        if (!m_pSqlCmd->setCommandText(m_sSql))
        {
            m_pLog->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "setCommandText error <%s>", m_sSql);
            return false;
        }
        if (!m_pSqlCmd->Execute())
        {
            m_pLog->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "Execute <%s> error %s", m_sSql, m_pSqlCmd->GetCmdErrInfo());
            return false;
        }

        bret = m_pSqlCmd->FetchNext();
        while (bret)
        {
            CBF_Tools::StringCopy(info.sProdType, sizeof(info.sProdType) - 1, m_pSqlCmd->Field(0).asString());
            info.nSecuNo =  m_pSqlCmd->Field(1).asLong();
            CBF_Tools::StringCopy(info.sSecuCode, sizeof(info.sSecuCode) - 1, m_pSqlCmd->Field(2).asString());
            CBF_Tools::StringCopy(info.sLimitType, sizeof(info.sLimitType) - 1, m_pSqlCmd->Field(3).asString());
            info.dUpLimit = m_pSqlCmd->Field(4).asDouble();
            info.dDownLimit=m_pSqlCmd->Field(5).asDouble();
            
            if (m_pMemDb->m_pDbSet.m_pPriceLimit.Insert(info) < 0)
            {
#ifdef _ENGLISH_
                m_pLog->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "risk_price_limit [%s %s] repeat", info.sProdType, info.sSecuCode);
#else
                m_pLog->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "价格限定表 risk_price_limit [%s %s]重复", info.sProdType, info.sSecuCode);
#endif
                return false;
            }
            bret = m_pSqlCmd->FetchNext();
        }
    }
    catch (...)
    {
#ifdef _ENGLISH_
        m_pLog->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "Database operator Exception!");
#else
        m_pLog->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "数据库操作异常");
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
	sprintf(m_sSql,"select a.CUST_NO,a.CHANNLE,a.LOGIN_IP,a.LOGIN_STATUS,a.LOGIN_TIME,a.LOGIN_DREBID,a.LOGIN_DREBPID,a.LOGIN_SVRID,a.LOGIN_SVRPID,a.LOGIN_GATEINDEX,LOGIN_TIMESTAMP from sys_cust_online_info a ");
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
			CBF_Tools::StringCopy(info.sCustNo , sizeof(info.sCustNo) - 1, m_pSqlCmd->Field(0).asString());
			CBF_Tools::StringCopy(info.sTermType, sizeof(info.sTermType) - 1, m_pSqlCmd->Field(1).asString());
			CBF_Tools::StringCopy(info.sLoginIp, sizeof(info.sLoginIp) - 1, m_pSqlCmd->Field(2).asString());
			CBF_Tools::StringCopy(info.sOnlineStatus, sizeof(info.sOnlineStatus) - 1, m_pSqlCmd->Field(3).asString());
			CBF_Tools::StringCopy(info.sOnlineTime, sizeof(info.sOnlineTime) - 1, m_pSqlCmd->Field(4).asString());
			info.nDrebId  = m_pSqlCmd->Field(5).asLong();
			info.nDrebPid  = m_pSqlCmd->Field(6).asLong();
			info.nSvrId  = m_pSqlCmd->Field(7).asLong();
			info.nSvrPid  = m_pSqlCmd->Field(8).asLong();
			info.nGateIndex  = m_pSqlCmd->Field(9).asLong();
			info.nTimeStamp  = m_pSqlCmd->Field(10).asLong();
			
			if (m_pMemDb->m_pDbTrader.m_pCustOnline.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"sys_cust_online_info [%s] repeat",info.sCustNo);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"客户在线表 sys_cust_online_info [%s]重复",info.sCustNo);
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
	sprintf(m_sSql,"select a.TX_ACC,a.EX_CODE,a.SEAT_NO,a.TX_CODE,a.EXT_INFO,a.COMM_INFO  from t_tradeacc_excode_info a ");
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
			CBF_Tools::StringCopy(info.sTxAcc , sizeof(info.sTxAcc) - 1, m_pSqlCmd->Field(0).asString());
			CBF_Tools::StringCopy(info.sExCode, sizeof(info.sExCode) - 1, m_pSqlCmd->Field(1).asString());
			CBF_Tools::StringCopy(info.sSeatNo, sizeof(info.sSeatNo) - 1, m_pSqlCmd->Field(2).asString());
			CBF_Tools::StringCopy(info.sTradeCode, sizeof(info.sTradeCode) - 1, m_pSqlCmd->Field(3).asString());
			CBF_Tools::StringCopy(info.sExtInfo, sizeof(info.sExtInfo) - 1, m_pSqlCmd->Field(4).asString());
			CBF_Tools::StringCopy(info.sCommInfo, sizeof(info.sCommInfo) - 1, m_pSqlCmd->Field(5).asString());
			
			if (m_pMemDb->m_pDbTrader.m_pTradecode.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"t_tradeacc_excode_info [%s] repeat",info.sTxAcc);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"资金账号交易编码对应 t_tradeacc_excode_info [%s]重复",info.sTxAcc);
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
	sprintf(m_sSql,"select a.EX_CODE,a.SEAT_NO,a.TRADER,a.LOGIN_STATUS,a.LOGIN_TIME,a.LOGOUT_TIME,a.COMM_PARA,a.REMARK  from t_ex_trader_info a ");
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
			CBF_Tools::StringCopy(info.sExchCode , sizeof(info.sExchCode) - 1, m_pSqlCmd->Field(0).asString());
			CBF_Tools::StringCopy(info.sSeatNo, sizeof(info.sSeatNo) - 1, m_pSqlCmd->Field(1).asString());
			CBF_Tools::StringCopy(info.sTraderId, sizeof(info.sTraderId) - 1, m_pSqlCmd->Field(2).asString());
			CBF_Tools::StringCopy(info.sLoginStatus, sizeof(info.sLoginStatus) - 1, m_pSqlCmd->Field(3).asString());
			CBF_Tools::StringCopy(info.sLoginTime, sizeof(info.sLoginTime) - 1, m_pSqlCmd->Field(4).asString());
			CBF_Tools::StringCopy(info.sLogoutTime, sizeof(info.sLogoutTime) - 1, m_pSqlCmd->Field(5).asString());
			CBF_Tools::StringCopy(info.sPara, sizeof(info.sPara) - 1, m_pSqlCmd->Field(6).asString());
			CBF_Tools::StringCopy(info.sRemark, sizeof(info.sRemark) - 1, m_pSqlCmd->Field(7).asString());
			if (m_pMemDb->m_pDbTrader.m_pExchTrader.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"t_ex_trader_info [%s] repeat",info.sExchCode);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"交易所席位交易员在线 t_ex_trader_info [%s]重复",info.sExchCode);
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
	acclock.nTradeAccRowid =-1;
	acclock.nFundRowid=-1;
	int i;

	
	sprintf(m_sSql,"select TX_ACC,CURRENCY,LAST_BAL,BALANCE,AVAIL_FUND,GET_FUND,BASE_MARGIN,INCOME_EXPENSE,FROZE_FUND,MARGIN,FEE,RESERVE,SURPLUS,FLOAT_SURPLUS,INCR_INTE,ARAP,RISK_LEVEL,RISK,TRADE_RIGHT from t_tradeacc_fund");
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
			//bzero(&info,sizeof(S_MDB_CUST_FUND));
			CBF_Tools::StringCopy(info.sTxAcc , sizeof(info.sTxAcc) - 1, m_pSqlCmd->Field(0).asString());
			CBF_Tools::StringCopy(info.sCurrency, sizeof(info.sCurrency) - 1, m_pSqlCmd->Field(1).asString());
			
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
			CBF_Tools::StringCopy(info.sTradeRight, sizeof(info.sTradeRight) - 1, m_pSqlCmd->Field(18).asString());
			rid = m_pMemDb->m_pDbTrader.m_pFund.Insert(info);
			if (rid <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"t_tradeacc_fund [%s] repeat",info.sTxAcc);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"资金表 t_tradeacc_fund [%s]重复",info.sTxAcc);
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
	sprintf(m_sSql,"select TX_ACC,FUND_TYPE,FUND_CODE,FUND_VALUE,CURRENCY from t_tradeacc_fund_detail ");
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
			CBF_Tools::StringCopy(info.sTxAcc , sizeof(info.sTxAcc) - 1, m_pSqlCmd->Field(0).asString());
			CBF_Tools::StringCopy(info.sFundType, sizeof(info.sFundType) - 1, m_pSqlCmd->Field(1).asString());
			CBF_Tools::StringCopy(info.sFundCode, sizeof(info.sFundCode) - 1, m_pSqlCmd->Field(2).asString());
			info.dFundValue =m_pSqlCmd->Field(3).asDouble();
			CBF_Tools::StringCopy(info.sCurrency, sizeof(info.sCurrency) - 1, m_pSqlCmd->Field(4).asString());
			if (m_pMemDb->m_pDbTrader.m_pFundDetail.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"t_tradeacc_fund_detail [%s] repeat",info.sTxAcc);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"资金详细表 t_tradeacc_fund_detail [%s]重复",info.sTxAcc);
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
	sprintf(m_sSql,"select TX_ACC,SECU_NO,SECU_CODE,SECU_NAME,SEAT_ID,TX_CODE,SH_FLAG,LAST_PB,LAST_PS,CUR_PB,CUR_PS,CUR_CANUSE_PB,CUR_CANUSE_PS,DAY_PB,DAY_PS,DAY_COV_PB_FROZ,DAY_COV_PB,DAY_COV_PS_FROZ,DAY_COV_PS,PB_OPEN_AVG_PRICE,PS_OPEN_AVG_PRICE,PB_AVG_PRICE,PS_AVG_PRICE,PB_MARGIN,PS_MARGIN,DAY_PB_SURPLUS,DAY_PS_SURPLUS,LAST_SETTLE_PRICE,DAY_SETTLE_PRICE,VARIETY from t_position");
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
			CBF_Tools::StringCopy(info.sTxAcc , sizeof(info.sTxAcc) - 1, m_pSqlCmd->Field(0).asString());
			info.nContractNo = m_pSqlCmd->Field(1).asLong();
			CBF_Tools::StringCopy(info.sContractCode, sizeof(info.sContractCode) - 1, m_pSqlCmd->Field(2).asString());
			CBF_Tools::StringCopy(info.sContractName, sizeof(info.sContractName) - 1, m_pSqlCmd->Field(3).asString());
			CBF_Tools::StringCopy(info.sSeatNo, sizeof(info.sSeatNo) - 1, m_pSqlCmd->Field(4).asString());
			CBF_Tools::StringCopy(info.sTradeCode, sizeof(info.sTradeCode) - 1, m_pSqlCmd->Field(5).asString());
			CBF_Tools::StringCopy(info.sShFlag, sizeof(info.sShFlag) - 1, m_pSqlCmd->Field(6).asString());
			info.nLastPB =m_pSqlCmd->Field(7).asNumeric();
			info.nLastPS =m_pSqlCmd->Field(8).asNumeric();
			info.nCurPB =m_pSqlCmd->Field(9).asNumeric();
			info.nCurPS =m_pSqlCmd->Field(10).asNumeric();
			info.nCurCanUsePB =m_pSqlCmd->Field(11).asNumeric();
			info.nCurCanUsePS =m_pSqlCmd->Field(12).asNumeric();
			info.nDayPB =m_pSqlCmd->Field(13).asNumeric();
			info.nDayPS =m_pSqlCmd->Field(14).asNumeric();
			info.nDAY_COV_PB_Froze =m_pSqlCmd->Field(15).asNumeric();
			info.nDAY_COV_PB =m_pSqlCmd->Field(16).asNumeric();
			info.nDAY_COV_PS_Froze =m_pSqlCmd->Field(17).asNumeric();
			info.nDAY_COV_PS = m_pSqlCmd->Field(18).asNumeric();
			
			info.dPB_OAvp =m_pSqlCmd->Field(19).asDouble();
			info.dPS_OAvp =m_pSqlCmd->Field(20).asDouble();
			info.dPB_Avp =m_pSqlCmd->Field(21).asDouble();
			info.dPS_Avp =m_pSqlCmd->Field(22).asDouble();
			info.dPB_Margin =m_pSqlCmd->Field(23).asDouble();
			info.dPS_Margin =m_pSqlCmd->Field(24).asDouble();
			info.dDAY_PB_SURPLUS =m_pSqlCmd->Field(25).asDouble();
			info.dDAY_PS_SURPLUS =m_pSqlCmd->Field(26).asDouble();
			info.dLAST_SETTLE_PRICE = m_pSqlCmd->Field(27).asDouble();
			info.dSETTLE_PRICE = m_pSqlCmd->Field(28).asDouble();
			info.nVarietyId = m_pSqlCmd->Field(29).asLong();
			if (m_pMemDb->m_pDbTrader.m_pPosition.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"t_position [%s] repeat",info.sTxAcc);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"持仓表 t_position [%s]重复",info.sTxAcc);
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
	sprintf(m_sSql,"select TX_ACC,EX_CODE,SEAT_NO,TX_CODE,SECU_NO,SECU_CODE,SECU_NAME,CLIENT_SERIAL,LOCAL_NO,SYSTEM_NO,ORDER_VOL,ORDER_PRICE,REMAIN_VOL,EO_FLAG,BS_FLAG,SH_FLAG,TXDATE,AMT,FROZ_AMT,EX_FEE,TX_TYPE,CMD_TYPE,CMD_ATTR,FORCE_FLAG,ORDER_STATUS,CANCEL_FLAG,ANS_TIME,ORDER_CHANNEL,ACC_ORG,TX_ORG,ORDER_OPER,CANCEL_CHANNEL,CANCEL_OPER,CANCEL_TIME,ORDER_RSP,FROZ_USEAMT,FROZ_OUTAMT,CANCEL_VOL,DONE_AVP,VARIETY,RULE_ALGO_CODE,CLEAR_SPEED,DELIVERY_TYPE,CLEAR_METHOD,MATCH_ACC from t_order_info order by SYSTEM_NO");
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
			CBF_Tools::StringCopy(info.sTxAcc , sizeof(info.sTxAcc) - 1, m_pSqlCmd->Field(0).asString());
			CBF_Tools::StringCopy(info.sExchCode, sizeof(info.sExchCode) - 1, m_pSqlCmd->Field(1).asString());
			CBF_Tools::StringCopy(info.sSeatNo, sizeof(info.sSeatNo) - 1, m_pSqlCmd->Field(2).asString());
			CBF_Tools::StringCopy(info.sTxCode, sizeof(info.sTxCode) - 1, m_pSqlCmd->Field(3).asString());
			info.nContractNo = m_pSqlCmd->Field(4).asLong();
			CBF_Tools::StringCopy(info.sContractCode, sizeof(info.sContractCode) - 1, m_pSqlCmd->Field(5).asString());
			CBF_Tools::StringCopy(info.sContractName, sizeof(info.sContractName) - 1, m_pSqlCmd->Field(6).asString());

			CBF_Tools::StringCopy(info.sClientSerial, sizeof(info.sClientSerial) - 1, m_pSqlCmd->Field(7).asString());
			CBF_Tools::StringCopy(info.sLocalSerial, sizeof(info.sLocalSerial) - 1, m_pSqlCmd->Field(8).asString());
			CBF_Tools::StringCopy(info.sSystemNo, sizeof(info.sSystemNo) - 1, m_pSqlCmd->Field(9).asString());
			info.nOrderNum =m_pSqlCmd->Field(10).asLong();
			info.dOrderPrice =m_pSqlCmd->Field(11).asDouble();
			info.nRemainNum =m_pSqlCmd->Field(12).asLong();
			CBF_Tools::StringCopy(info.sEoFlag , sizeof(info.sEoFlag) - 1, m_pSqlCmd->Field(13).asString());
			CBF_Tools::StringCopy(info.sBsFlag , sizeof(info.sBsFlag) - 1, m_pSqlCmd->Field(14).asString());
			CBF_Tools::StringCopy(info.sShFlag , sizeof(info.sShFlag) - 1, m_pSqlCmd->Field(15).asString());
			CBF_Tools::StringCopy(info.sTxDate , sizeof(info.sTxDate) - 1, m_pSqlCmd->Field(16).asString());
			info.dAmt =m_pSqlCmd->Field(17).asDouble();
			info.dFrozAmt =m_pSqlCmd->Field(18).asDouble();
			info.dExFee =m_pSqlCmd->Field(19).asDouble();
			CBF_Tools::StringCopy(info.sTxType , sizeof(info.sTxType) - 1, m_pSqlCmd->Field(20).asString());
			CBF_Tools::StringCopy(info.sCmdType , sizeof(info.sCmdType) - 1, m_pSqlCmd->Field(21).asString());
			CBF_Tools::StringCopy(info.sCmdAttr, sizeof(info.sCmdAttr) - 1, m_pSqlCmd->Field(22).asString());
			CBF_Tools::StringCopy(info.sForceFlag , sizeof(info.sForceFlag) - 1, m_pSqlCmd->Field(23).asString());
			CBF_Tools::StringCopy(info.sOrderStatus , sizeof(info.sOrderStatus) - 1, m_pSqlCmd->Field(24).asString());
			CBF_Tools::StringCopy(info.sCancelFlag , sizeof(info.sCancelFlag) - 1, m_pSqlCmd->Field(25).asString());
			CBF_Tools::StringCopy(info.sAnsTime , sizeof(info.sAnsTime) - 1, m_pSqlCmd->Field(26).asString());
			CBF_Tools::StringCopy(info.sOrderChannel , sizeof(info.sOrderChannel) - 1, m_pSqlCmd->Field(27).asString());
			CBF_Tools::StringCopy(info.sAccOrg , sizeof(info.sAccOrg) - 1, m_pSqlCmd->Field(28).asString());
			CBF_Tools::StringCopy(info.sTxOrg , sizeof(info.sTxOrg) - 1, m_pSqlCmd->Field(29).asString());
			CBF_Tools::StringCopy(info.sOrderOper , sizeof(info.sOrderOper) - 1, m_pSqlCmd->Field(30).asString());
			CBF_Tools::StringCopy(info.sCancelChannel , sizeof(info.sCancelChannel) - 1, m_pSqlCmd->Field(31).asString());
			CBF_Tools::StringCopy(info.sCancelOper , sizeof(info.sCancelOper) - 1, m_pSqlCmd->Field(32).asString());
			CBF_Tools::StringCopy(info.sCancelTime , sizeof(info.sCancelTime) - 1, m_pSqlCmd->Field(33).asString());
			CBF_Tools::StringCopy(info.sOrderRsp , sizeof(info.sOrderRsp) - 1, m_pSqlCmd->Field(34).asString());
			info.dFrozUseamt = m_pSqlCmd->Field(35).asDouble();
			info.dFrozOutamt = m_pSqlCmd->Field(36).asDouble();
			info.nCancelNum = m_pSqlCmd->Field(37).asLong();
			info.dDoneAvp = m_pSqlCmd->Field(38).asDouble();
			info.nVarietyId = m_pSqlCmd->Field(39).asLong();
			CBF_Tools::StringCopy(info.sRuleAlgoCode, sizeof(info.sRuleAlgoCode) - 1, m_pSqlCmd->Field(40).asString());
            info.nCancelNum = m_pSqlCmd->Field(41).asLong();
            info.dDoneAvp = m_pSqlCmd->Field(42).asDouble();
            info.nClearSpeed = m_pSqlCmd->Field(43).asLong();
			CBF_Tools::StringCopy(info.sMatchAcc, sizeof(info.sMatchAcc) - 1, m_pSqlCmd->Field(44).asString());
			if (m_pMemDb->m_pDbTrader.m_pOrder.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"t_order_info [%s] repeat",info.sTxAcc);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"报单表 t_order_info [%s]重复",info.sTxAcc);
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
	sprintf(m_sSql,"select DONE_NO,SYSTEM_NO,EX_CODE,SEAT_NO,TX_CODE,LOCAL_NO,TX_ACC,DONE_DATE,DONE_TIME,SECU_NO,SECU_CODE,SECU_NAME,DONE_PRICE,DONE_QTY,BS_FLAG,SH_FLAG,EO_FLAG,ORDER_PRICE,TX_TYPE,CMD_TYPE,CMD_ATTR,AMT,EX_FEE,MEMBER_FEE,EX_MARGIN,MEMBER_MARGIN,FLOAT_SURPLUS,COV_SURPLUS,TX_ORG,ACC_ORG,FORCE_FLAG,ORDER_CHANNEL,ORDER_OPER,SYSTEM_DATE,VARIETY,RULE_ALGO_CODE,CLEAR_SPEED from t_match_info order by DONE_NO");
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
			CBF_Tools::StringCopy(info.sDoneNo , sizeof(info.sDoneNo) - 1, m_pSqlCmd->Field(0).asString());
			CBF_Tools::StringCopy(info.sSystemNo, sizeof(info.sSystemNo) - 1, m_pSqlCmd->Field(1).asString());
			CBF_Tools::StringCopy(info.sExchCode, sizeof(info.sExchCode) - 1, m_pSqlCmd->Field(2).asString());
			CBF_Tools::StringCopy(info.sSeatNo, sizeof(info.sSeatNo) - 1, m_pSqlCmd->Field(3).asString());
			CBF_Tools::StringCopy(info.sTxCode, sizeof(info.sTxCode) - 1, m_pSqlCmd->Field(4).asString());
			CBF_Tools::StringCopy(info.sLocalNo, sizeof(info.sLocalNo) - 1, m_pSqlCmd->Field(5).asString());
			CBF_Tools::StringCopy(info.sTxAcc, sizeof(info.sTxAcc) - 1, m_pSqlCmd->Field(6).asString());
			CBF_Tools::StringCopy(info.sDoneDate, sizeof(info.sDoneDate) - 1, m_pSqlCmd->Field(7).asString());
			CBF_Tools::StringCopy(info.sDoneTime, sizeof(info.sDoneTime) - 1, m_pSqlCmd->Field(8).asString());
			info.nContractNo =m_pSqlCmd->Field(9).asLong();
			CBF_Tools::StringCopy(info.sContractCode, sizeof(info.sContractCode) - 1, m_pSqlCmd->Field(10).asString());
			CBF_Tools::StringCopy(info.sContractName, sizeof(info.sContractName) - 1, m_pSqlCmd->Field(11).asString());
			info.dDonePrice =m_pSqlCmd->Field(12).asDouble();
			info.nDoneNum = m_pSqlCmd->Field(13).asLong();
			CBF_Tools::StringCopy(info.sBsFlag , sizeof(info.sBsFlag) - 1, m_pSqlCmd->Field(14).asString());
			CBF_Tools::StringCopy(info.sShFlag , sizeof(info.sShFlag) - 1, m_pSqlCmd->Field(15).asString());
			CBF_Tools::StringCopy(info.sEoFlag , sizeof(info.sEoFlag) - 1, m_pSqlCmd->Field(16).asString());
			info.dOrderPrice = m_pSqlCmd->Field(17).asDouble();
			CBF_Tools::StringCopy(info.sTxType , sizeof(info.sTxType) - 1, m_pSqlCmd->Field(18).asString());
			CBF_Tools::StringCopy(info.sCmdType , sizeof(info.sCmdType) - 1, m_pSqlCmd->Field(19).asString());
			CBF_Tools::StringCopy(info.sCmdAttr , sizeof(info.sCmdAttr) - 1, m_pSqlCmd->Field(20).asString());
			info.dAmt =m_pSqlCmd->Field(21).asDouble();
			info.dExFee =m_pSqlCmd->Field(22).asDouble();
			info.dMemberFee =m_pSqlCmd->Field(23).asDouble();
			info.dExMargin =m_pSqlCmd->Field(24).asDouble();
			info.dMemberMargin =m_pSqlCmd->Field(25).asDouble();
			info.dFloatSurplus =m_pSqlCmd->Field(26).asDouble();
			info.dCovSurplus =m_pSqlCmd->Field(27).asDouble();
			CBF_Tools::StringCopy(info.sTxOrg , sizeof(info.sCmdAttr) - 1, m_pSqlCmd->Field(28).asString());
			CBF_Tools::StringCopy(info.sAccOrg , sizeof(info.sCmdAttr) - 1, m_pSqlCmd->Field(29).asString());
			CBF_Tools::StringCopy(info.sForceFlag , sizeof(info.sCmdAttr) - 1, m_pSqlCmd->Field(30).asString());
			CBF_Tools::StringCopy(info.sOrderChannel , sizeof(info.sCmdAttr) - 1, m_pSqlCmd->Field(31).asString());
			CBF_Tools::StringCopy(info.sOrderOper , sizeof(info.sCmdAttr) - 1, m_pSqlCmd->Field(32).asString());
			CBF_Tools::StringCopy(info.sSystemDate , sizeof(info.sCmdAttr) - 1, m_pSqlCmd->Field(33).asString());
			info.nVarietyId = m_pSqlCmd->Field(34).asLong();
			CBF_Tools::StringCopy(info.sRuleAlgoCode, sizeof(info.sCmdAttr) - 1, m_pSqlCmd->Field(35).asString());
			info.nClearSpeed = m_pSqlCmd->Field(36).asLong();
			if (m_pMemDb->m_pDbTrader.m_pDone.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"t_match_info [%s] repeat",info.sTxAcc);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"成交表 t_match_info [%s]重复",info.sTxAcc);
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
	sprintf(m_sSql,"select TX_DATE,TX_ACC,TX_SERIAL,BANK_ACC,BANK_NAME,CURRENCY,AMT,FUND_FLAG,LAST_AMT,CUR_AMT from t_tradeacc_fund_flow where IS_REC='1' ");
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
			CBF_Tools::StringCopy(info.sTxDate , sizeof(info.sTxDate) - 1, m_pSqlCmd->Field(0).asString());
			CBF_Tools::StringCopy(info.sTxAcc, sizeof(info.sTxAcc) - 1, m_pSqlCmd->Field(1).asString());
			CBF_Tools::StringCopy(info.sTxSerial, sizeof(info.sTxSerial) - 1, m_pSqlCmd->Field(2).asString());
			CBF_Tools::StringCopy(info.sBankAcc, sizeof(info.sBankAcc) - 1, m_pSqlCmd->Field(3).asString());
			CBF_Tools::StringCopy(info.sBankName, sizeof(info.sBankName) - 1, m_pSqlCmd->Field(4).asString());
			CBF_Tools::StringCopy(info.sCurrency, sizeof(info.sCurrency) - 1, m_pSqlCmd->Field(5).asString());
			info.dAmt = m_pSqlCmd->Field(6).asDouble();
			CBF_Tools::StringCopy(info.sFundFlag, sizeof(info.sFundFlag) - 1, m_pSqlCmd->Field(7).asString());
			info.dLastAmt =m_pSqlCmd->Field(8).asDouble();
			info.dCurAmt =m_pSqlCmd->Field(9).asDouble();
			if (m_pMemDb->m_pDbTrader.m_pFundFlow.Insert(info) <0 )
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"t_tradeacc_fund_flow [%s] repeat",info.sTxAcc);
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"出入金表 t_tradeacc_fund_flow [%s]重复",info.sTxAcc);
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
	
	sprintf(m_sSql,"select SECU_NO,SECU_CODE,EXCH_DATE,SETTLE_PRICE,VOLATILITY,THEORETICAL_PRICE,DELTA,GAMMA,VEGA,THETA,RHO,IMPLIED_VOLATILITY_BS,IMPLIED_VOLATILITY_GARCH from risk_option_price_model  order by EXCH_DATE desc ");
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
			m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"Table risk_option_price_model no data");
#else
			m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"期权价格模型表risk_option_price_model表无数据 ");
#endif
			return true;
		}
		while (bret)
		{
			info.nContractNo = m_pSqlCmd->Field(0).asLong();
			CBF_Tools::StringCopy(info.sContractCode, sizeof(info.sContractCode) - 1, m_pSqlCmd->Field(1).asString());
			CBF_Tools::StringCopy(info.sSettleDate, sizeof(info.sSettleDate) - 1, m_pSqlCmd->Field(2).asString());
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

bool CInitTrade::ReadCodeTable()
{
    bool bret;
    int rid;
    S_MDB_CODE_TABLE info;

    sprintf(m_sSql, "select CODE_TYPE,FLAG,CODE_ID,CODE_DESC,VALUE_TYPE from sys_code_table  order by CODE_TYPE desc ");
    try
    {
        if (!m_pSqlCmd->setCommandText(m_sSql))
        {
            m_pLog->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "setCommandText error <%s>", m_sSql);
            return false;
        }
        if (!m_pSqlCmd->Execute())
        {
            m_pLog->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "Execute <%s> error %s", m_sSql, m_pSqlCmd->GetCmdErrInfo());
            return false;
        }

        bret = m_pSqlCmd->FetchNext();
        if (!bret)
        {
#ifdef _ENGLISH_
            m_pLog->LogMp(LOG_WARNNING, __FILE__, __LINE__, "Table sys_code_table no data");
#else
            m_pLog->LogMp(LOG_WARNNING, __FILE__, __LINE__, "码表 sys_code_table表无数据 ");
#endif
            return false;
        }
        while (bret)
        {
			CBF_Tools::StringCopy(info.sCodeType, sizeof(info.sCodeType) - 1, m_pSqlCmd->Field(0).asString());
            CBF_Tools::StringCopy(info.sCharFlag, sizeof(info.sCharFlag) - 1, m_pSqlCmd->Field(1).asString());
            CBF_Tools::StringCopy(info.sCodeId, sizeof(info.sCodeId) - 1, m_pSqlCmd->Field(2).asString());
			CBF_Tools::StringCopy(info.sCodeDesc, sizeof(info.sCodeDesc) - 1, m_pSqlCmd->Field(3).asString());
            info.nValueType = m_pSqlCmd->Field(4).asLong();
            rid = m_pMemDb->m_pDbBaseinfo.m_pCodeTable.Insert(info);
            bret = m_pSqlCmd->FetchNext();
        }
    }
    catch (...)
    {
#ifdef _ENGLISH_
        m_pLog->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "Database operator Exception!");
#else
        m_pLog->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "数据库操作异常");
#endif
        return false;
    }
    return true;
}
