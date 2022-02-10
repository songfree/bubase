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

#define ERR_NOTSETPARA      -1000                 ///���Ӳ���δ����
#define ERR_NOTSETCONN      -1001                 ///δ�������ݿ�����
#define ERR_NOTSETSQLTEXT   -1002                 ///δ�������ݿ�����


#ifdef DBAPI_EXPORTS
#define DBAPI_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define DBAPI_EXPORT __declspec(dllimport)
#else
#define DBAPI_EXPORT 
#endif
#endif

//���ݿ�������
class CSqlData;

class DBAPI_EXPORT CDbConnection 
{

public:
	CDbConnection();
	virtual ~CDbConnection();

protected:
	std::string m_sDbName;//����Ϊ������ tnsname��
	std::string m_sDbUser;//���ݿ��û���
	std::string m_sDbUserPwd;//���ݿ��û�����

	bool m_bIsSetDbPara;
	bool m_bIsConnection;//�Ƿ��������ݿ�
	
public:

	// ������: ConnectDb
	// ���  : ������ 2009-5-13 8:42:05
	// ����  : bool 
	// ����  : CDbConnection &extcon
	// ����  : ʹ���ⲿ�������������ݿ⣬������������ݿ⸴�Ƹ�extcon���������ݿ�
	bool ConnectDb(CDbConnection &extcon);

	// ������: Rollback
	// ���  : ������ 2009-5-8 13:20:58
	// ����  : bool 
	// ����  : �ع�����
	bool Rollback();

	// ������: Commit
	// ���  : ������ 2009-5-8 13:20:49
	// ����  : bool 
	// ����  : �ύ����
	bool Commit();

	// ������: setAutoCommit
	// ���  : ������ 2009-5-8 13:02:21
	// ����  : bool false����
	// ����  : bool autocommit=false  true�Զ��ύ false���������ύ 
	// ����  : �������ݿ��Զ��ύ��־
	bool setAutoCommit(bool autocommit=false);

	// ������: DisConnect
	// ���  : ������ 2009-5-8 13:02:11
	// ����  : bool false����
	// ����  : �Ͽ����ݿ�����
	bool DisConnect();

	// ������: IsConnect
	// ���  : ������ 2009-5-8 13:01:59
	// ����  : bool 
	// ����  : ���ݿ��Ƿ�����
	bool IsConnect();

	// ������: ConnectDb
	// ���  : ������ 2009-5-8 13:01:51
	// ����  : bool 
	// ����  : �������ݿ⣬�����ύΪ�ֶ��ύ
	bool ConnectDb();

	// ������: SetDbPara
	// ���  : ������ 2009-5-8 11:42:50
	// ����  : bool 
	// ����  : string dbname ���ݿ������
	// ����  : string dbuser ���ݿ��û�
	// ����  : string dbuserpwd ���ݿ��û�����
	// ����  : int dbtype ���ݿ�����
	// ����  : �������ݿ����Ӳ���
	bool SetDbPara(std::string dbname,std::string dbuser,std::string dbuserpwd,int dbtype);

	void SetMysqlCharset(char* charset);

	std::string m_sErrInfo;//������Ϣ
	int m_nErrCode;//�������

	char m_clientVersion[30];
	char m_serverVersion[30];

	int m_nDbType;//���ݿ����� 0-oracel 1-sybase 2-sqlserver 3-infomix 4-db2 5-odbc 6-mysql 

	SAConnection m_dbConnection;//sqlapi

};

//ִ��sql���ݼ���
class DBAPI_EXPORT CSqlCommand  
{
public:

	CSqlCommand();
	virtual ~CSqlCommand();
	
	SACommand m_sqlapiCmd;
	
	std::string m_sErrInfo;//������Ϣ
	int m_nErrCode;//�������

protected:
	bool m_bIsSetConnection;//�Ƿ��������ݿ�����
	bool m_bIsSetCommandText;//�Ƿ�������sql���

	char m_ErrInfoMsg[200];

public:

	// ������: GetErrInfo
	// ���  : ������ 2009-5-8 14:36:35
	// ����  : char *    %d:%s �������:������Ϣ
	// ����  : ȡ�ó�����Ϣ 
	char * GetErrInfo();

	// ������: Field
	// ���  : ������ 2009-5-8 14:30:24
	// ����  : SAField & 
	// ����  : const char *sField
	// ����  : �����ֶ�����ȡ���ֶΣ�ע��Ҫtry catch
	SAField & Field(const char *sField);

	// ������: Field
	// ���  : ������ 2009-5-8 14:29:44
	// ����  : SAField & 
	// ����  : int nField
	// ����  : �����ֶ����ȡ���ֶ�(��0��ʼ)��ע��Ҫtry catch
	SAField & Field(int nField);

	// ������: FieldCount
	// ���  : ������ 2009-5-8 14:22:22
	// ����  : int 0���ֶ� -1���� ����0Ϊ�ֶ���
	// ����  : ȡ�ü�¼�е��ܹ��ֶ���
	int FieldCount();

	// ������: &Param
	// ���  : ������ 2009-5-8 14:21:00
	// ����  : SAParam 
	// ����  : const char *sParamByName
	// ����  :  ���ݲ�����ȡ�ò�����Ȼ��ȡ����ֵ�����ò�������
	SAParam &Param(const char *sParamByName);

	// ������: &Param
	// ���  : ������ 2009-5-8 14:20:14
	// ����  : SAParam 
	// ����  : int nParamByID
	// ����  : ����IDȡ�ò�����Ȼ��ȡ����ֵ�����ò�������
	SAParam &Param(int nParamByID);

	// ������: ParamCount
	// ���  : ������ 2009-5-8 14:20:04
	// ����  : int 
	// ����  : ȡ�ò�������
	int ParamCount();

	// ������: FetchNext
	// ���  : ������ 2009-5-8 14:12:57
	// ����  : bool 
	// ����  : �ӽ����ȡ��һ����¼
	bool FetchNext();

	// ������: RowsAffected
	// ���  : ������ 2009-5-8 14:09:52
	// ����  : long -1ִ�д� 0������ ��СΪ��¼��
	// ����  : ִ��Ӱ��ļ�¼�� ��update��insert��¼��
	long RowsAffected();

	// ������: isResultSet
	// ���  : ������ 2009-5-8 14:09:32
	// ����  : bool 
	// ����  : ִ�н���Ƿ��н����
	bool isResultSet();
	

	// ������: Execute
	// ���  : ������ 2009-5-8 14:04:14
	// ����  : bool 
	// ����  : const char *sqlbuf
	// ����  : ִ���޲�����sql��䣬
	bool Execute(const char *sqlbuf);

	// ������: Execute
	// ���  : ������ 2009-5-8 14:03:26
	// ����  : bool 
	// ����  : ִ��SetCommandText�����ò������sql���
	bool Execute();

	// ������: SetCommandText
	// ���  : ������ 2009-5-8 13:50:19
	// ����  : bool 
	// ����  : const char *sqlbuf
	// ����  : int cmdType=0  ִ������ 0 sql 1�洢���� 2 sqlstmtraw ����δ֪
	// ����  : ����sqlִ�����
	bool SetCommandText(const char *sqlbuf,int cmdType=0);
	

	// ������: SetConnection
	// ���  : ������ 2009-5-8 13:35:09
	// ����  : void 
	// ����  : CDbConnection *con
	// ����  : �������ݿ�����
	void SetConnection(CDbConnection *con);
	
	

};


//һ��sql���ݼ�һ���������ӵ���
class DBAPI_EXPORT CDbApi  
{
public:
	CDbApi();
	virtual ~CDbApi();

protected:
	
	int  m_nLastUseTime;

public:

	// ������: LoadResultSql
	// ���  : ������ 2009-5-8 15:33:51
	// ����  : CSqlData 
	// ����  : const char *sqlbuf
	// ����  : ����CSqlData���͵Ľ��������
	CSqlData LoadResultSql(const char *sqlbuf);

	// ������: LoadResultSql
	// ���  : ������ 2009-5-13 8:43:50
	// ����  : CSqlData 
	// ����  : ��ǰ��setcommandtext֮��ִ��sql�������ʹ��CSqlData�෵��
	CSqlData LoadResultSql();

	// ������: GetCmdErrInfo
	// ���  : ������ 2009-5-8 15:33:36
	// ����  : char * 
	// ����  : ȡִ��sql�ĳ�����Ϣ
	char * GetCmdErrInfo();

	// ������: Field
	// ���  : ������ 2009-5-8 14:30:24
	// ����  : SAField & 
	// ����  : const char *sField
	// ����  : �����ֶ�����ȡ���ֶΣ�ע��Ҫtry catch
	SAField & Field(const char *sField);
	
	// ������: Field
	// ���  : ������ 2009-5-8 14:29:44
	// ����  : SAField & 
	// ����  : int nField
	// ����  : �����ֶ����ȡ���ֶ�(��0��ʼ)��ע��Ҫtry catch
	SAField & Field(int nField);
	
	// ������: FieldCount
	// ���  : ������ 2009-5-8 14:22:22
	// ����  : int 0���ֶ� -1���� ����0Ϊ�ֶ���
	// ����  : ȡ�ü�¼�е��ܹ��ֶ���
	int FieldCount();
	
	// ������: &Param
	// ���  : ������ 2009-5-8 14:21:00
	// ����  : SAParam 
	// ����  : const char *sParamByName
	// ����  :  ���ݲ�����ȡ�ò�����Ȼ��ȡ����ֵ�����ò�������
	SAParam &Param(const char *sParamByName);
	
	// ������: &Param
	// ���  : ������ 2009-5-8 14:20:14
	// ����  : SAParam 
	// ����  : int nParamByID
	// ����  : ����IDȡ�ò�����Ȼ��ȡ����ֵ�����ò�������
	SAParam &Param(int nParamByID);
	
	// ������: ParamCount
	// ���  : ������ 2009-5-8 14:20:04
	// ����  : int 
	// ����  : ȡ�ò�������
	int ParamCount();
	
	// ������: FetchNext
	// ���  : ������ 2009-5-8 14:12:57
	// ����  : bool 
	// ����  : �ӽ����ȡ��һ����¼
	bool FetchNext();
	
	// ������: RowsAffected
	// ���  : ������ 2009-5-8 14:09:52
	// ����  : long -1ִ�д� 0������ ��СΪ��¼��
	// ����  : ִ��Ӱ��ļ�¼�� ��update��insert��¼��
	long RowsAffected();
	
	// ������: isResultSet
	// ���  : ������ 2009-5-8 14:09:32
	// ����  : bool 
	// ����  : ִ�н���Ƿ��н����
	bool isResultSet();
	
	
	// ������: Execute
	// ���  : ������ 2009-5-8 14:04:14
	// ����  : bool 
	// ����  : const char *sqlbuf
	// ����  : ִ���޲�����sql��䣬
	bool Execute(const char *sqlbuf);
	
	// ������: Execute
	// ���  : ������ 2009-5-8 14:03:26
	// ����  : bool 
	// ����  : ִ��SetCommandText�����ò������sql���
	bool Execute();
	
	// ������: SetCommandText
	// ���  : ������ 2009-5-8 13:50:19
	// ����  : bool 
	// ����  : const char *sqlbuf
	// ����  : int cmdType=0  ִ������ 0 sql 1�洢���� 2 sqlstmtraw ����δ֪
	// ����  : ����sqlִ�����
	bool setCommandText(const char *sqlbuf,int cmdType=0);

	// ������: setAutoCommit
	// ���  : ������ 2009-5-8 14:56:03
	// ����  : bool 
	// ����  : bool autocommit=false
	// ����  : ���������ύ��ʽ
	bool setAutoCommit(bool autocommit=false);

	// ������: Rollback
	// ���  : ������ 2009-5-8 14:55:45
	// ����  : bool 
	// ����  : �ع�����
	bool Rollback();

	// ������: Commit
	// ���  : ������ 2009-5-8 14:54:45
	// ����  : bool 
	// ����  : �����ύ��ע��:�����ύ��������ϴ��ύ��ع�������
	bool Commit();

	// ������: ReConnect
	// ���  : ������ 2009-5-8 14:54:33
	// ����  : bool 
	// ����  : �������ݿ�
	bool ReConnect();

	// ������: IsConnect
	// ���  : ������ 2009-5-8 14:54:24
	// ����  : bool 
	// ����  : �Ƿ��������ݿ�
	bool IsConnect();

	// ������: DisConnect
	// ���  : ������ 2009-5-8 14:54:13
	// ����  : bool 
	// ����  : �Ͽ����ݿ�����
	bool DisConnect();

	// ������: Connect
	// ���  : ������ 2009-5-8 14:54:02
	// ����  : bool 
	// ����  : string dbname   ip:port@name or tnsname
	// ����  : string dbuser
	// ����  : string dbuserpwd
	// ����  : int dbtype 0-oracl 1-sybase 2-sqlserver 3-infomi 4-db2 5-odbc 6-mysql
	// ����  : �������ݿ�
	bool Connect(string dbname,string dbuser,string dbuserpwd,int dbtype);

	void SetMysqlCharset(char *charset);

	CDbConnection m_dbconn;//���ݿ�����
	CSqlCommand   m_sqlcmd;//sql

};



#endif // !defined(AFX_DBAPI_H__73E8A8B9_DB6A_4D5B_84B3_3E05BD563D35__INCLUDED_)
