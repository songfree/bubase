// DbApi.cpp: implementation of the CDbApi class.
//
//////////////////////////////////////////////////////////////////////

#include "DbApi.h"
#include "SqlDataResult.h"



#define CLIENT_MULTI_RESULTS    131072  /* Enable/disable multi-results */

//////////////////////////////////////////////////////////////////////
// CDbConnection Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDbConnection::CDbConnection()
{
	m_bIsSetDbPara = false;
	m_bIsConnection = false;
	m_nDbType = 0;
	m_nErrCode = 0;
	memset(m_clientVersion,0,sizeof(m_clientVersion));
	memset(m_serverVersion,0,sizeof(m_serverVersion));
}

CDbConnection::~CDbConnection()
{
	DisConnect();
}

bool CDbConnection::SetDbPara(std::string dbname, std::string dbuser, std::string dbuserpwd, int dbtype)
{
	m_sDbName = dbname;
	m_sDbUser = dbuser;
	m_sDbUserPwd = dbuserpwd;
	if (dbtype<0 || dbtype >6)
	{
		m_bIsSetDbPara = false;
		m_sErrInfo = "数据库类型不符:"+dbtype;
		return false;
	}
	m_nDbType = dbtype;
	m_bIsSetDbPara = true;
	return true;
}

bool CDbConnection::ConnectDb()
{
	if (!m_bIsSetDbPara)
	{
		m_sErrInfo = "数据库连接参数未设置";
		m_nErrCode = ERR_NOTSETPARA;
	}
	try 
	{
		switch (m_nDbType)
		{
		case 0://oracle
			m_dbConnection.setClient(SA_Oracle_Client);
			m_dbConnection.Connect(m_sDbName.c_str(),m_sDbUser.c_str(),m_sDbUserPwd.c_str(),SA_Oracle_Client);
			break;
		case 1:
			m_dbConnection.setClient(SA_Sybase_Client);
			m_dbConnection.Connect(m_sDbName.c_str(),m_sDbUser.c_str(),m_sDbUserPwd.c_str(),SA_Sybase_Client);
			break;
		case 2:
			m_dbConnection.setClient(SA_SQLServer_Client);
			m_dbConnection.Connect(m_sDbName.c_str(),m_sDbUser.c_str(),m_sDbUserPwd.c_str(),SA_SQLServer_Client);
			break;
		case 3:
			m_dbConnection.setClient(SA_Informix_Client);
			m_dbConnection.Connect(m_sDbName.c_str(),m_sDbUser.c_str(),m_sDbUserPwd.c_str(),SA_Informix_Client);
			break;
		case 4:
			m_dbConnection.setClient(SA_DB2_Client);
			m_dbConnection.Connect(m_sDbName.c_str(),m_sDbUser.c_str(),m_sDbUserPwd.c_str(),SA_DB2_Client);
			break;
		case 5:
			m_dbConnection.setClient(SA_ODBC_Client);
			m_dbConnection.Connect(m_sDbName.c_str(),m_sDbUser.c_str(),m_sDbUserPwd.c_str(),SA_ODBC_Client);
			break;
		case 6:
			m_dbConnection.setClient(SA_MySQL_Client);
			m_dbConnection.setOption(CLIENT_MULTI_RESULTS);
			m_dbConnection.Connect(m_sDbName.c_str(),m_sDbUser.c_str(),m_sDbUserPwd.c_str(),SA_MySQL_Client);
			
#ifdef _WINDOWS
			//mysql默认为gbk
			m_dbConnection.mysql_character((char *)"gbk");
#else
			char tmpchar[30];
			bzero(tmpchar,sizeof(tmpchar));
			if (getenv("MYSQL_CLIENT_CHARSET")!= NULL)
			{
				strcpy(tmpchar,getenv("MYSQL_CLIENT_CHARSET"));
				m_dbConnection.mysql_character(tmpchar);
			}
			else
			{
				//mysql默认为gbk
				m_dbConnection.mysql_character("gbk");
			}
			
#endif			
			break;
		default:
			m_sErrInfo = "数据库类型不符:"+m_nDbType;
			return false;
		}
		m_dbConnection.setAutoCommit(SA_AutoCommitOff);
		long nVersionClient = m_dbConnection.ClientVersion();
		short minor;
		short major;
		if(nVersionClient)
		{
			minor = (short)(nVersionClient & 0xFFFF);
			major = (short)(nVersionClient >> 16);
			sprintf(m_clientVersion,"Client version: %hd.%hd", major, minor);
		}
		else
		{
			sprintf(m_clientVersion,"Client version: unknown \n");
		}
		long nVersionServer = m_dbConnection.ServerVersion();
		minor = (short)(nVersionServer & 0xFFFF);
		major = (short)(nVersionServer >> 16);
		sprintf(m_serverVersion,"Server version: %hd.%hd", major, minor);
	}
	catch (SAException &err)
	{
		m_sErrInfo =err.ErrText();
		m_nErrCode=err.ErrNativeCode();
		m_bIsConnection = false; 
		return false;
	}
	m_bIsConnection = true;
	return true;
}

bool CDbConnection::IsConnect()
{
	if (!m_bIsConnection)
	{
		return false;
	}
	return m_dbConnection.isConnected();
}

bool CDbConnection::DisConnect()
{
	if (!m_bIsConnection)
	{
		return true;
	}
	try
	{
		m_dbConnection.Disconnect();
	}
	catch(SAException &err)
	{
		m_sErrInfo =err.ErrText();
		m_nErrCode=err.ErrNativeCode();
		m_bIsConnection = false; 
		return false;
	}
	return true;
	
}

bool CDbConnection::setAutoCommit(bool autocommit)
{
	try
	{
		if (autocommit)
		{
			m_dbConnection.setAutoCommit(SA_AutoCommitOn);
		}
		else
		{
			m_dbConnection.setAutoCommit(SA_AutoCommitOff);
		}
	}
	catch (SAException &err)
	{
		m_sErrInfo =err.ErrText();
		m_nErrCode=err.ErrNativeCode();
		m_bIsConnection = false; 
		return false;
	}
	return true;
	
}

bool CDbConnection::Commit()
{
	try
	{
		m_dbConnection.Commit();
	}
	catch (SAException &err)
	{
		m_sErrInfo =err.ErrText();
		m_nErrCode=err.ErrNativeCode();
		return false;
    }
	return true;
}

bool CDbConnection::Rollback()
{
	try
	{
		m_dbConnection.Rollback();
	}
	catch (SAException &err)
	{
		m_sErrInfo =err.ErrText();
		m_nErrCode=err.ErrNativeCode();
		return false;
    }
	return true;
}



//////////////////////////////////////////////////////////////////////
// CSqlCommand Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSqlCommand::CSqlCommand()
{
	m_bIsSetConnection = false;
	m_bIsSetCommandText = false;
	m_nErrCode = 0;
	memset(m_ErrInfoMsg,0,sizeof(m_ErrInfoMsg));
	
}

CSqlCommand::~CSqlCommand()
{
	
}

void CSqlCommand::SetConnection(CDbConnection *con)
{
	m_sqlapiCmd.setConnection(&con->m_dbConnection);
	m_bIsSetConnection = true;
}

bool CSqlCommand::SetCommandText(const char *sqlbuf,int cmdType)
{
	if (!m_bIsSetConnection)
	{
		m_sErrInfo = "未设置数据库连接";
		m_nErrCode = ERR_NOTSETPARA;
		return false;
	}
	try
	{
		switch (cmdType)
		{
			case 0:
				m_sqlapiCmd.setCommandText(sqlbuf,SA_CmdSQLStmt);
				break;
			case 1:
				m_sqlapiCmd.setCommandText(sqlbuf,SA_CmdStoredProc);
				break;
			case 2:
				m_sqlapiCmd.setCommandText(sqlbuf,SA_CmdSQLStmtRaw);
				break;
			default:
				m_sqlapiCmd.setCommandText(sqlbuf,SA_CmdUnknown);
				break;
		}
		
	}
	catch (SAException &err)
	{
		m_sErrInfo =err.ErrText();
		m_nErrCode=err.ErrNativeCode();
		return false;
    }
	m_bIsSetCommandText = true;
	return true;
}

bool CSqlCommand::Execute()
{
	if (!m_bIsSetCommandText)
	{
		m_sErrInfo = "未设置sql命令内容";
		m_nErrCode = ERR_NOTSETSQLTEXT;
		return false;
	}
	try
	{
		m_sqlapiCmd.Execute();
	}
	catch (SAException &err)
	{
		m_sErrInfo =err.ErrText();
		m_nErrCode=err.ErrNativeCode();
		return false;
    }
	m_bIsSetCommandText = false;
	return true;
}
bool CSqlCommand::Execute(const char *sqlbuf)
{
	if (!m_bIsSetConnection)
	{
		m_sErrInfo = "未设置数据库连接";
		m_nErrCode = ERR_NOTSETCONN;
		return false;
	}
	if (!SetCommandText(sqlbuf,3))
	{
		return false;
	}
	if (!Execute())
	{
		return false;
	}
	return true;
}

bool CSqlCommand::isResultSet()
{
	bool iset; 
	try
	{
		iset = m_sqlapiCmd.isResultSet();
	}
	catch (SAException &err)
	{
		m_sErrInfo =err.ErrText();
		m_nErrCode=err.ErrNativeCode();
		return false;
    }
	return iset;
}

long CSqlCommand::RowsAffected()
{
	int rec;
	try
	{
		rec = m_sqlapiCmd.RowsAffected();
	}
	catch (SAException &err)
	{
		m_sErrInfo =err.ErrText();
		m_nErrCode=err.ErrNativeCode();
		return -1;
    }
	return rec;
}

bool CSqlCommand::FetchNext()
{
	bool isfetch; 
	try
	{
		isfetch = m_sqlapiCmd.FetchNext();
	}
	catch (SAException &err)
	{
		m_sErrInfo =err.ErrText();
		m_nErrCode=err.ErrNativeCode();
		return false;
    }
	return isfetch;
}

int CSqlCommand::ParamCount()
{
	return m_sqlapiCmd.ParamCount();
}

SAParam &CSqlCommand::Param(int nParamByID)
{
	return m_sqlapiCmd.Param(nParamByID);
}
SAParam &CSqlCommand::Param(const char *sParamByName)
{
	return m_sqlapiCmd.Param(sParamByName);
}

int CSqlCommand::FieldCount()
{
	int rec;
	try
	{
		rec = m_sqlapiCmd.FieldCount();
	}
	catch (SAException &err)
	{
		m_sErrInfo =err.ErrText();
		m_nErrCode=err.ErrNativeCode();
		return -1;
    }
	return rec;
}

SAField & CSqlCommand::Field(int nField)
{
	return m_sqlapiCmd.Field(nField+1);
}
SAField & CSqlCommand::Field(const char *sField)
{
	return m_sqlapiCmd.Field(sField);
}

char * CSqlCommand::GetErrInfo()
{
	sprintf(m_ErrInfoMsg,"%d:%s",m_nErrCode,m_sErrInfo.c_str());
	return m_ErrInfoMsg;
}




//////////////////////////////////////////////////////////////////////
// CDbApi Construction/Destruction
//////////////////////////////////////////////////////////////////////


CDbApi::CDbApi()
{
	m_nLastUseTime=time(NULL);
}

CDbApi::~CDbApi()
{
	
}

bool CDbApi::Connect(std::string dbname, std::string dbuser, std::string dbuserpwd, int dbtype)
{
	bool isconn;
	isconn = m_dbconn.SetDbPara(dbname,dbuser,dbuserpwd,dbtype);
	if (!isconn)
	{
		return isconn;
	}
	isconn =  m_dbconn.ConnectDb();
	m_sqlcmd.SetConnection(&m_dbconn);
	return isconn;
}

bool CDbApi::DisConnect()
{
	return m_dbconn.DisConnect();
}

bool CDbApi::IsConnect()
{
	return m_dbconn.IsConnect();
}

bool CDbApi::ReConnect()
{
	return m_dbconn.ConnectDb();
}

bool CDbApi::Commit()
{
	return m_dbconn.Commit();
}

bool CDbApi::Rollback()
{
	return m_dbconn.Rollback();
}

bool CDbApi::setAutoCommit(bool autocommit)
{
	return m_dbconn.setAutoCommit(autocommit);
}

bool CDbApi::setCommandText(const char *sqlbuf,int cmdType)
{
	return m_sqlcmd.SetCommandText(sqlbuf,cmdType);
}

bool CDbApi::Execute()
{
	//判断连接是否超时
	if (time(NULL)-m_nLastUseTime>3600*4 || !IsConnect()) //超时4小时 或者连接断开
	{
		DisConnect();
		ReConnect();
	}
	m_nLastUseTime=time(NULL);
	return m_sqlcmd.Execute();
}
bool CDbApi::Execute(const char *sqlbuf)
{
	//判断连接是否超时
	if (time(NULL)-m_nLastUseTime>3600*4 || !IsConnect()) //超时4小时 或者连接断开
	{
		DisConnect();
		ReConnect();
	}
	m_nLastUseTime=time(NULL);
	return m_sqlcmd.Execute(sqlbuf);
}

bool CDbApi::isResultSet()
{
	return m_sqlcmd.isResultSet();
}

long CDbApi::RowsAffected()
{
	return m_sqlcmd.RowsAffected();
}

bool CDbApi::FetchNext()
{
	return m_sqlcmd.FetchNext();
}

int CDbApi::ParamCount()
{
	return m_sqlcmd.ParamCount();
}

SAParam &CDbApi::Param(int nParamByID)
{
	return m_sqlcmd.Param(nParamByID);
}
SAParam &CDbApi::Param(const char *sParamByName)
{
	return m_sqlcmd.Param(sParamByName);
}

int CDbApi::FieldCount()
{
	return m_sqlcmd.FieldCount();
}

SAField & CDbApi::Field(int nField)
{
	return m_sqlcmd.Field(nField);
}
SAField & CDbApi::Field(const char *sField)
{
	return m_sqlcmd.Field(sField);
}

char * CDbApi::GetCmdErrInfo()
{
	return m_sqlcmd.GetErrInfo();
}
void CDbApi::SetMysqlCharset(char *charset)
{
	m_dbconn.SetMysqlCharset(charset);
}
CSqlData CDbApi::LoadResultSql(const char *sqlbuf)
{
	CSqlAPIData sqldata;
	CSqlData result;
	char tmpchar[8192];
	char sSql[256];
	memset(tmpchar,0,sizeof(tmpchar));
	memset(sSql,0,sizeof(sSql));
	int ret ;
	bool iscount;
	int bcount;
	iscount=false;
	
	SANumeric aaa;
	SAString bbb;
	ret =0;
	bcount = 0;
	try
	{
		if (!Execute(sqlbuf))
		{
			result.DB_ErrorCode = m_sqlcmd.m_nErrCode;
			strcpy(result.DB_ErrorInfo ,m_sqlcmd.m_sErrInfo.c_str());
			result.setSqlCount(-1);
			return result;
		}
		
		ret = FieldCount();//取得字段数
		while (FetchNext())
		{
			for (int i=0;i<ret;i++)
			{
				S_RECORD ss;	
				ss.name = Field(i).Name();
				ss.value = Field(i).asString();
				sqldata.Add(ss);
			}
			result.put(sqldata);
			sqldata.Clear();
		}
			     
	}
	catch (SAException &err)
	{
		result.DB_ErrorCode = m_sqlcmd.m_nErrCode;
		strcpy(result.DB_ErrorInfo ,m_sqlcmd.m_sErrInfo.c_str());
		result.setSqlCount(-1);
		return result;
	}
	result.DB_ErrorCode = 0; 
    return result;
}

CSqlData CDbApi::LoadResultSql()
{
	CSqlAPIData sqldata;
	CSqlData result;
	char tmpchar[8192];
	char sSql[256];
	memset(tmpchar,0,sizeof(tmpchar));
	memset(sSql,0,sizeof(sSql));
	int ret ;
	bool iscount;
	int bcount;
	iscount=false;
	
	SANumeric aaa;
	SAString bbb;
	ret =0;
	bcount = 0;
	try
	{
		if (!Execute())
		{
			result.DB_ErrorCode = m_sqlcmd.m_nErrCode;
			strcpy(result.DB_ErrorInfo ,m_sqlcmd.m_sErrInfo.c_str());
			result.setSqlCount(-1);
			return result;
		}

		ret = FieldCount();//取得字段数
		while (FetchNext())
		{
			for (int i=0;i<ret;i++)
			{
				S_RECORD ss;	
				ss.name = Field(i).Name();
				ss.value = Field(i).asString();
				sqldata.Add(ss);
			}
			result.put(sqldata);
			sqldata.Clear();
		}
			     
	}
	catch (SAException &err)
	{
		result.DB_ErrorCode = m_sqlcmd.m_nErrCode;
		strcpy(result.DB_ErrorInfo ,m_sqlcmd.m_sErrInfo.c_str());
		result.setSqlCount(-1);
		return result;
	}
	result.DB_ErrorCode = 0; 
    return result;
}


bool CDbConnection::ConnectDb(CDbConnection &extcon)
{
	if (!extcon.SetDbPara(m_sDbName,m_sDbUser,m_sDbUserPwd,m_nDbType))
	{
		return false;
	}
	return extcon.ConnectDb();
}
void CDbConnection::SetMysqlCharset(char* charset)
{
	m_dbConnection.mysql_character(charset);
}