// DbApi.h: interface for the CDbApi class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DBAPI_H__73E8A8B9_DB6A_4D5B_84B3_3E05BD563D35__INCLUDED_)
#define AFX_DBAPI_H__73E8A8B9_DB6A_4D5B_84B3_3E05BD563D35__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"

#ifdef WIN32
#include "SQLAPI_win.h"
#ifdef _DEBUG
#pragma comment(lib, "sqlapid.lib") 
#pragma message("Automatically linking with sqlapid.lib")
#else
#pragma comment(lib, "sqlapi.lib") 
#pragma message("Automatically linking with sqlapi.lib")
#endif
#else
#include "SQLAPI_unix.h"
#endif
#include "SqlDataResult.h"	// Added by ClassView

#define ERR_NOTSETPARA      -1000                 ///连接参数未设置
#define ERR_NOTSETCONN      -1001                 ///未设置数据库连接
#define ERR_NOTSETSQLTEXT   -1002                 ///未设置数据库连接


#ifdef DBAPI_EXPORTS
#define DBAPI_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define DBAPI_EXPORT __declspec(dllimport)
#else
#define DBAPI_EXPORT 
#endif
#endif

//数据库连接类
class CSqlData;

class DBAPI_EXPORT CDbConnection 
{

public:
	CDbConnection();
	virtual ~CDbConnection();

protected:
	std::string m_sDbName;//数据为服务名 tnsname等
	std::string m_sDbUser;//数据库用户名
	std::string m_sDbUserPwd;//数据库用户密码

	bool m_bIsSetDbPara;
	bool m_bIsConnection;//是否连接数据库
	
public:

	// 函数名: ConnectDb
	// 编程  : 王明松 2009-5-13 8:42:05
	// 返回  : bool 
	// 参数  : CDbConnection &extcon
	// 描述  : 使用外部连接类连接数据库，即将本类的数据库复制给extcon并连接数据库
	bool ConnectDb(CDbConnection &extcon);

	// 函数名: Rollback
	// 编程  : 王明松 2009-5-8 13:20:58
	// 返回  : bool 
	// 描述  : 回滚事务
	bool Rollback();

	// 函数名: Commit
	// 编程  : 王明松 2009-5-8 13:20:49
	// 返回  : bool 
	// 描述  : 提交事务
	bool Commit();

	// 函数名: setAutoCommit
	// 编程  : 王明松 2009-5-8 13:02:21
	// 返回  : bool false出错
	// 参数  : bool autocommit=false  true自动提交 false主动代码提交 
	// 描述  : 设置数据库自动提交标志
	bool setAutoCommit(bool autocommit=false);

	// 函数名: DisConnect
	// 编程  : 王明松 2009-5-8 13:02:11
	// 返回  : bool false出错
	// 描述  : 断开数据库连接
	bool DisConnect();

	// 函数名: IsConnect
	// 编程  : 王明松 2009-5-8 13:01:59
	// 返回  : bool 
	// 描述  : 数据库是否连接
	bool IsConnect();

	// 函数名: ConnectDb
	// 编程  : 王明松 2009-5-8 13:01:51
	// 返回  : bool 
	// 描述  : 连接数据库，事务提交为手动提交
	bool ConnectDb();

	// 函数名: SetDbPara
	// 编程  : 王明松 2009-5-8 11:42:50
	// 返回  : bool 
	// 参数  : string dbname 数据库服务名
	// 参数  : string dbuser 数据库用户
	// 参数  : string dbuserpwd 数据库用户密码
	// 参数  : int dbtype 数据库类型
	// 描述  : 设置数据库连接参数
	bool SetDbPara(std::string dbname,std::string dbuser,std::string dbuserpwd,int dbtype);

	void SetMysqlCharset(char* charset);

	std::string m_sErrInfo;//出错信息
	int m_nErrCode;//出错代码

	char m_clientVersion[30];
	char m_serverVersion[30];

	int m_nDbType;//数据库类型 0-oracel 1-sybase 2-sqlserver 3-infomix 4-db2 5-odbc 6-mysql 

	SAConnection m_dbConnection;//sqlapi

};

//执行sql数据集类
class DBAPI_EXPORT CSqlCommand  
{
public:

	CSqlCommand();
	virtual ~CSqlCommand();
	
	SACommand m_sqlapiCmd;
	
	std::string m_sErrInfo;//出错信息
	int m_nErrCode;//出错代码

protected:
	bool m_bIsSetConnection;//是否设置数据库连接
	bool m_bIsSetCommandText;//是否设置了sql语句

	char m_ErrInfoMsg[200];

public:

	// 函数名: GetErrInfo
	// 编程  : 王明松 2009-5-8 14:36:35
	// 返回  : char *    %d:%s 出错代码:出错信息
	// 描述  : 取得出错信息 
	char * GetErrInfo();

	// 函数名: Field
	// 编程  : 王明松 2009-5-8 14:30:24
	// 返回  : SAField & 
	// 参数  : const char *sField
	// 描述  : 根据字段名称取得字段，注意要try catch
	SAField & Field(const char *sField);

	// 函数名: Field
	// 编程  : 王明松 2009-5-8 14:29:44
	// 返回  : SAField & 
	// 参数  : int nField
	// 描述  : 根据字段序号取得字段(从0开始)，注意要try catch
	SAField & Field(int nField);

	// 函数名: FieldCount
	// 编程  : 王明松 2009-5-8 14:22:22
	// 返回  : int 0无字段 -1出错 大于0为字段数
	// 描述  : 取得记录中的总共字段数
	int FieldCount();

	// 函数名: &Param
	// 编程  : 王明松 2009-5-8 14:21:00
	// 返回  : SAParam 
	// 参数  : const char *sParamByName
	// 描述  :  根据参数名取得参数，然后取参数值或设置参数内容
	SAParam &Param(const char *sParamByName);

	// 函数名: &Param
	// 编程  : 王明松 2009-5-8 14:20:14
	// 返回  : SAParam 
	// 参数  : int nParamByID
	// 描述  : 根据ID取得参数，然后取参数值或设置参数内容
	SAParam &Param(int nParamByID);

	// 函数名: ParamCount
	// 编程  : 王明松 2009-5-8 14:20:04
	// 返回  : int 
	// 描述  : 取得参数个数
	int ParamCount();

	// 函数名: FetchNext
	// 编程  : 王明松 2009-5-8 14:12:57
	// 返回  : bool 
	// 描述  : 从结果集取出一条记录
	bool FetchNext();

	// 函数名: RowsAffected
	// 编程  : 王明松 2009-5-8 14:09:52
	// 返回  : long -1执行错 0无数据 大小为记录数
	// 描述  : 执行影响的记录数 如update、insert记录数
	long RowsAffected();

	// 函数名: isResultSet
	// 编程  : 王明松 2009-5-8 14:09:32
	// 返回  : bool 
	// 描述  : 执行结果是否有结果集
	bool isResultSet();
	

	// 函数名: Execute
	// 编程  : 王明松 2009-5-8 14:04:14
	// 返回  : bool 
	// 参数  : const char *sqlbuf
	// 描述  : 执行无参数的sql语句，
	bool Execute(const char *sqlbuf);

	// 函数名: Execute
	// 编程  : 王明松 2009-5-8 14:03:26
	// 返回  : bool 
	// 描述  : 执行SetCommandText后并设置参数后的sql语句
	bool Execute();

	// 函数名: SetCommandText
	// 编程  : 王明松 2009-5-8 13:50:19
	// 返回  : bool 
	// 参数  : const char *sqlbuf
	// 参数  : int cmdType=0  执行类型 0 sql 1存储过程 2 sqlstmtraw 其它未知
	// 描述  : 设置sql执行语句
	bool SetCommandText(const char *sqlbuf,int cmdType=0);
	

	// 函数名: SetConnection
	// 编程  : 王明松 2009-5-8 13:35:09
	// 返回  : void 
	// 参数  : CDbConnection *con
	// 描述  : 设置数据库连接
	void SetConnection(CDbConnection *con);
	
	

};


//一个sql数据集一个数库连接的类
class DBAPI_EXPORT CDbApi  
{
public:
	CDbApi();
	virtual ~CDbApi();

protected:
	
	int  m_nLastUseTime;

public:

	// 函数名: LoadResultSql
	// 编程  : 王明松 2009-5-8 15:33:51
	// 返回  : CSqlData 
	// 参数  : const char *sqlbuf
	// 描述  : 返回CSqlData类型的结果集数据
	CSqlData LoadResultSql(const char *sqlbuf);

	// 函数名: LoadResultSql
	// 编程  : 王明松 2009-5-13 8:43:50
	// 返回  : CSqlData 
	// 描述  : 将前面setcommandtext之后执行sql并将结果使用CSqlData类返回
	CSqlData LoadResultSql();

	// 函数名: GetCmdErrInfo
	// 编程  : 王明松 2009-5-8 15:33:36
	// 返回  : char * 
	// 描述  : 取执行sql的出错信息
	char * GetCmdErrInfo();

	// 函数名: Field
	// 编程  : 王明松 2009-5-8 14:30:24
	// 返回  : SAField & 
	// 参数  : const char *sField
	// 描述  : 根据字段名称取得字段，注意要try catch
	SAField & Field(const char *sField);
	
	// 函数名: Field
	// 编程  : 王明松 2009-5-8 14:29:44
	// 返回  : SAField & 
	// 参数  : int nField
	// 描述  : 根据字段序号取得字段(从0开始)，注意要try catch
	SAField & Field(int nField);
	
	// 函数名: FieldCount
	// 编程  : 王明松 2009-5-8 14:22:22
	// 返回  : int 0无字段 -1出错 大于0为字段数
	// 描述  : 取得记录中的总共字段数
	int FieldCount();
	
	// 函数名: &Param
	// 编程  : 王明松 2009-5-8 14:21:00
	// 返回  : SAParam 
	// 参数  : const char *sParamByName
	// 描述  :  根据参数名取得参数，然后取参数值或设置参数内容
	SAParam &Param(const char *sParamByName);
	
	// 函数名: &Param
	// 编程  : 王明松 2009-5-8 14:20:14
	// 返回  : SAParam 
	// 参数  : int nParamByID
	// 描述  : 根据ID取得参数，然后取参数值或设置参数内容
	SAParam &Param(int nParamByID);
	
	// 函数名: ParamCount
	// 编程  : 王明松 2009-5-8 14:20:04
	// 返回  : int 
	// 描述  : 取得参数个数
	int ParamCount();
	
	// 函数名: FetchNext
	// 编程  : 王明松 2009-5-8 14:12:57
	// 返回  : bool 
	// 描述  : 从结果集取出一条记录
	bool FetchNext();
	
	// 函数名: RowsAffected
	// 编程  : 王明松 2009-5-8 14:09:52
	// 返回  : long -1执行错 0无数据 大小为记录数
	// 描述  : 执行影响的记录数 如update、insert记录数
	long RowsAffected();
	
	// 函数名: isResultSet
	// 编程  : 王明松 2009-5-8 14:09:32
	// 返回  : bool 
	// 描述  : 执行结果是否有结果集
	bool isResultSet();
	
	
	// 函数名: Execute
	// 编程  : 王明松 2009-5-8 14:04:14
	// 返回  : bool 
	// 参数  : const char *sqlbuf
	// 描述  : 执行无参数的sql语句，
	bool Execute(const char *sqlbuf);
	
	// 函数名: Execute
	// 编程  : 王明松 2009-5-8 14:03:26
	// 返回  : bool 
	// 描述  : 执行SetCommandText后并设置参数后的sql语句
	bool Execute();
	
	// 函数名: SetCommandText
	// 编程  : 王明松 2009-5-8 13:50:19
	// 返回  : bool 
	// 参数  : const char *sqlbuf
	// 参数  : int cmdType=0  执行类型 0 sql 1存储过程 2 sqlstmtraw 其它未知
	// 描述  : 设置sql执行语句
	bool setCommandText(const char *sqlbuf,int cmdType=0);

	// 函数名: setAutoCommit
	// 编程  : 王明松 2009-5-8 14:56:03
	// 返回  : bool 
	// 参数  : bool autocommit=false
	// 描述  : 设置事务提交方式
	bool setAutoCommit(bool autocommit=false);

	// 函数名: Rollback
	// 编程  : 王明松 2009-5-8 14:55:45
	// 返回  : bool 
	// 描述  : 回滚事务
	bool Rollback();

	// 函数名: Commit
	// 编程  : 王明松 2009-5-8 14:54:45
	// 返回  : bool 
	// 描述  : 事务提交，注意:本次提交的事务从上次提交或回滚后到现在
	bool Commit();

	// 函数名: ReConnect
	// 编程  : 王明松 2009-5-8 14:54:33
	// 返回  : bool 
	// 描述  : 重连数据库
	bool ReConnect();

	// 函数名: IsConnect
	// 编程  : 王明松 2009-5-8 14:54:24
	// 返回  : bool 
	// 描述  : 是否连接数据库
	bool IsConnect();

	// 函数名: DisConnect
	// 编程  : 王明松 2009-5-8 14:54:13
	// 返回  : bool 
	// 描述  : 断开数据库连接
	bool DisConnect();

	// 函数名: Connect
	// 编程  : 王明松 2009-5-8 14:54:02
	// 返回  : bool 
	// 参数  : string dbname   ip:port@name or tnsname
	// 参数  : string dbuser
	// 参数  : string dbuserpwd
	// 参数  : int dbtype 0-oracl 1-sybase 2-sqlserver 3-infomi 4-db2 5-odbc 6-mysql
	// 描述  : 连接数据库
	bool Connect(string dbname,string dbuser,string dbuserpwd,int dbtype);

	void SetMysqlCharset(char *charset);

	CDbConnection m_dbconn;//数据库连接
	CSqlCommand   m_sqlcmd;//sql

};



#endif // !defined(AFX_DBAPI_H__73E8A8B9_DB6A_4D5B_84B3_3E05BD563D35__INCLUDED_)
