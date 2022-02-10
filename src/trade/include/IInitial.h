// IInitial.h: interface for the CIInitial class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IINITIAL_H__0C474664_A8FF_4462_891C_8E66AE593D19__INCLUDED_)
#define AFX_IINITIAL_H__0C474664_A8FF_4462_891C_8E66AE593D19__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "MemDb.h"
#include "BF_Xml.h"
#include "IErrLog.h"
#include "DbApi.h"
#include "BF_3Des.h"

#define  DBPWDKEY   "rd402wms"
#define  DBPWD3DESKEY   "rd402wms20092017Ylink123" 
//初始化基类，初始化动态库继承此基类实现初始化。
//主程序在启动时，打开动态库，调用Init方法。
//初始化动态库将内存表里的数据进行初始。
class CIInitial  
{
public:
	CIInitial()
	{
		m_pMemDb = NULL;
		m_pXml = NULL;
		m_pLog = NULL;
		m_pSqlCmd = NULL;
		m_bNeedConnectDb = false;
	}
	virtual ~CIInitial()
	{
		m_pMemDb = NULL;
		if (m_pSqlCmd != NULL)
		{
			m_pSqlCmd->DisConnect();
			delete m_pSqlCmd;
			m_pSqlCmd = NULL;
		}
	}

	// 函数名: Init
	// 编程  : 王明松 2017-8-9 9:46:56
	// 返回  : virtual bool
	// 参数  : CIErrlog *log  写日志类指针
	// 参数  : CBF_Xml *xml  xml配置文件
	// 参数  : CMemDb *mdb   内存表
	// 描述  : 动态库要实现的初始化方法
	virtual bool Init(CIErrlog *log,CBF_Xml *xml,CMemDb *mdb)
	{
		m_pMemDb = mdb;
		m_pLog = log;
		m_pXml = xml;
		int nUseDb=0;
		if (m_pXml->GetNodeAttribute("package/head/initial/database",false,"use",false,nUseDb) == NULL)
		{
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"no [package/head/initial/database] node attribute use");
#else
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"节点[package/head/initial/database]的属性use不存在");
#endif
			return false;
		}
		if (nUseDb == 1)
		{
			m_bNeedConnectDb = true;
		}
		else
		{
			m_bNeedConnectDb = false;
		}
		if (m_bNeedConnectDb)
		{
			char tmpchar[100];
			memset(tmpchar,0,sizeof(tmpchar));
			std::string dbname;
			std::string dbuser;
			std::string dbpwd;
			std::string dbpwdflag;
			std::string dbtype;
			if (m_pXml->GetNodeValueByPath("package/head/initial/database/databaseserver",false,dbname,false) == NULL)
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xml file no [package/head/initial/database/databaseserver] node");
#else
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"节点[package/head/initial/database/databaseserver]不存在");
#endif
				return false;
			}
			if (m_pXml->GetNodeValueByPath("package/head/initial/database/databaseuser",false,dbuser,false) == NULL)
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xml file no [package/head/initial/database/databaseuser] node");
#else
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"节点[package/head/initial/database/databaseuser] 不存在");
#endif
				return false;
			}
			if (m_pXml->GetNodeValueByPath("package/head/initial/database/userpasswd",false,dbpwd,true) == NULL)
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xml file no [package/head/initial/database/userpasswd] node");
#else
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"节点[package/head/initial/database/userpasswd]不存在");
#endif
				return false;
			}
			if (m_pXml->GetNodeValueByPath("package/head/initial/database/passwdtype",false,dbpwdflag,false) == NULL)
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xml file no [package/head/initial/database/passwdtype] node");
#else
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"节点 [package/head/initial/database/passwdtype] 不存在");
#endif
				return false;
			}
			if (m_pXml->GetNodeValueByPath("package/head/initial/database/databasetype",false,dbtype,false) == NULL)
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xml file no [package/head/initial/database/databasetype] node");
#else
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"节点[package/head/initial/database/databasetype]不存在");
#endif
				return false;
			}
//			m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"开始连接数据库");

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
//			m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"连接数据库成功");
		}
		else
		{
//			m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"未配置使用数据库");
		}
		return true;
	}

protected:
	CMemDb    *m_pMemDb;//内存表
	CBF_Xml   *m_pXml;  //xml配置
	CIErrlog  *m_pLog;  //日志
	CDbApi    *m_pSqlCmd;//数据库

	CBF_Des    m_pEncrypt;//des加解密类
	bool      m_bNeedConnectDb;
};

#endif // !defined(AFX_IINITIAL_H__0C474664_A8FF_4462_891C_8E66AE593D19__INCLUDED_)
