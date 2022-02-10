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
//��ʼ�����࣬��ʼ����̬��̳д˻���ʵ�ֳ�ʼ����
//������������ʱ���򿪶�̬�⣬����Init������
//��ʼ����̬�⽫�ڴ��������ݽ��г�ʼ��
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

	// ������: Init
	// ���  : ������ 2017-8-9 9:46:56
	// ����  : virtual bool
	// ����  : CIErrlog *log  д��־��ָ��
	// ����  : CBF_Xml *xml  xml�����ļ�
	// ����  : CMemDb *mdb   �ڴ��
	// ����  : ��̬��Ҫʵ�ֵĳ�ʼ������
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
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ڵ�[package/head/initial/database]������use������");
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
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ڵ�[package/head/initial/database/databaseserver]������");
#endif
				return false;
			}
			if (m_pXml->GetNodeValueByPath("package/head/initial/database/databaseuser",false,dbuser,false) == NULL)
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xml file no [package/head/initial/database/databaseuser] node");
#else
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ڵ�[package/head/initial/database/databaseuser] ������");
#endif
				return false;
			}
			if (m_pXml->GetNodeValueByPath("package/head/initial/database/userpasswd",false,dbpwd,true) == NULL)
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xml file no [package/head/initial/database/userpasswd] node");
#else
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ڵ�[package/head/initial/database/userpasswd]������");
#endif
				return false;
			}
			if (m_pXml->GetNodeValueByPath("package/head/initial/database/passwdtype",false,dbpwdflag,false) == NULL)
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xml file no [package/head/initial/database/passwdtype] node");
#else
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ڵ� [package/head/initial/database/passwdtype] ������");
#endif
				return false;
			}
			if (m_pXml->GetNodeValueByPath("package/head/initial/database/databasetype",false,dbtype,false) == NULL)
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xml file no [package/head/initial/database/databasetype] node");
#else
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ڵ�[package/head/initial/database/databasetype]������");
#endif
				return false;
			}
//			m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"��ʼ�������ݿ�");

			strcpy(tmpchar,dbpwd.c_str());
			if (atoi(dbpwdflag.c_str()) ==1 )
			{
				int len = strlen(tmpchar);
				if (m_pEncrypt.UncryptString(tmpchar,len,DBPWDKEY)<0)
				{
#ifdef _ENGLISH_
					m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"database user password uncrypt error");
#else
					m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ݿ�������ܳ���");
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
					m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ݿ�������ܳ��� %s",errmsg);
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
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ݿ�����ʵ����ʧ�ܣ�����ռ�ʧ��");
#endif
				return false;
			}
			if (!m_pSqlCmd->Connect(dbname,dbuser,tmpchar,atoi(dbtype.c_str())))
			{
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"connect database error %d %s",m_pSqlCmd->m_dbconn.m_nErrCode,m_pSqlCmd->m_dbconn.m_sErrInfo.c_str());
#else
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"�������ݿ�� %d %s",m_pSqlCmd->m_dbconn.m_nErrCode,m_pSqlCmd->m_dbconn.m_sErrInfo.c_str());
#endif
				return false;
			}
//			m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"�������ݿ�ɹ�");
		}
		else
		{
//			m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"δ����ʹ�����ݿ�");
		}
		return true;
	}

protected:
	CMemDb    *m_pMemDb;//�ڴ��
	CBF_Xml   *m_pXml;  //xml����
	CIErrlog  *m_pLog;  //��־
	CDbApi    *m_pSqlCmd;//���ݿ�

	CBF_Des    m_pEncrypt;//des�ӽ�����
	bool      m_bNeedConnectDb;
};

#endif // !defined(AFX_IINITIAL_H__0C474664_A8FF_4462_891C_8E66AE593D19__INCLUDED_)
