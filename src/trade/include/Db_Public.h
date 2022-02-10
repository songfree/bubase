// Db_Public.h: interface for the CDb_Public class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DB_PUBLIC_H__78A43EE7_4192_47A9_8E6E_498B399BCEEB__INCLUDED_)
#define AFX_DB_PUBLIC_H__78A43EE7_4192_47A9_8E6E_498B399BCEEB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MdbBase.h"
#include "BF_Mutex.h"

//���ݿ⹫������  ����������Ա��������


typedef struct 
{
	char sOperCode[21];//����Ա����
	char sOperName[41];//����Ա����
	char sOperPhone[21];//����Ա�绰
	char sOrgCode[13];//��������
	char sOperType[2];//����Ա����
}S_MDB_OPER_INFO;


typedef struct 
{
	char sOrgCode[13];//��������
	char sOrgName[41];//��������
	char sOrgType[2];//��������
	char sOrgLevel[2];//��������
	char sUp1Org[13];//��һ������
	char sUpOrg[201];//�ϼ�����  ���ָ�
	char sOrgCity[5];//����
	char sAccNo[24];//�����˺�
}S_MDB_ORG_INFO;


typedef struct
{
	char sOperCode[21];//����Ա����
	char sTermType[11];//��¼����
	char sLoginIp[21];//��¼IP
	char sOnlineStatus[2];//����״̬
	char sOnlineTime[18];//��¼ʱ��
	int  nDrebId;//���߹����ڵ��
	int  nDrebPid;//����˽�нڵ��
	int  nSvrId;//���������
	int  nSvrPid;//˽�з����
	int  nGateIndex; //�����������
	long nTimeStamp;//��¼ʱ���
}S_MDB_OPER_ONLINE;

typedef struct
{
	char sParaCode[41];//��������
	char sParaName[41];//��������
	char sParaValue[41];//����ֵ
	char sValueType[2];//ֵ����
}S_MDB_SYSTEM_PARA;

//����Ա
class  CTbl_Oper  
{
protected:
	
	CMemTableNew <S_MDB_OPER_INFO> m_table;
//	CPkeyCharF<21,1>          m_pkey;               //����Ա����
	CPkeyCharV<1>          m_pkey;               //����Ա����
public:
	CTbl_Oper()
	{
		
	}
	virtual ~CTbl_Oper()
	{
		
	}
	void Clear()
	{
		m_pkey.Clear();
		m_table.Clear();
	}
	int Insert(S_MDB_OPER_INFO oper) //ֻ���ڳ�ʼ��ʱʹ�ã����Կ��Բ�����
	{
		if (m_pkey.Find(oper.sOperCode))
		{
			return -1;
		}
		int rid=m_table.Add(oper);
		m_pkey.Add(rid,m_table.m_table[rid].sOperCode);
		return rid;
	}
	
	// ������: *Select
	// ���  : ������ 2017-8-3 10:20:35
	// ����  : S_MDB_OPER_INFO 
	// ����  : const char *opercode
	// ����  : ���ݲ���Ա������Ҳ���Ա��Ϣ
	S_MDB_OPER_INFO *Select(const char *opercode)
	{
		int rid;
		if (!m_pkey.GetRowId(rid,(char *)opercode))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}
	S_MDB_OPER_INFO *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
};

//������Ϣ��  
class CTbl_Org
{
protected:
	
	CMemTableNew <S_MDB_ORG_INFO>      m_table;
// 	CPkeyCharF<13,1>               m_pkey;//��������
// 	CIndexCharF<13,1>            m_index_up1org; //����һ���������е����� ��������һ������
// 	CIndexCharF<201,1>            m_index_uporg;  //���ϼ��������е����� �����������¼�����
	CPkeyCharV<1>               m_pkey;//��������
	CIndexVarChar<1>            m_index_up1org; //����һ���������е����� ��������һ������
	CIndexVarChar<1>            m_index_uporg;  //���ϼ��������е����� �����������¼�����
	CBF_Mutex                   m_mutex;//������
public:
	CTbl_Org()
	{
		
	}
	virtual ~CTbl_Org()
	{
		
	}
	void Clear()
	{
		m_pkey.Clear();
		m_index_uporg.Clear();
		m_index_up1org.Clear();
		m_table.Clear();
	}
	int Insert(S_MDB_ORG_INFO org)
	{
		if (m_pkey.Find(org.sOrgCode))
		{
			return -1;
		}
		int rid = m_table.Add(org);
		m_pkey.Add(rid,m_table.m_table[rid].sOrgCode);
		m_index_uporg.Add(rid,m_table.m_table[rid].sUpOrg);
		m_index_up1org.Add(rid,m_table.m_table[rid].sUp1Org);
		return rid;
	}
	S_MDB_ORG_INFO *Select(char *orgcode)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		if (!m_pkey.GetRowId(rid,orgcode))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}

	// ������: SelectByParentOrg
	// ���  : ������ 2017-8-4 9:35:07
	// ����  : bool 
	// ����  : char *orgcode
	// ����  : std::vector<S_MDB_ORG_INFO *> &orglist
	// ����  : ���ݻ��������ѯ�˻�����������һ������(���������¼�������һ��)
	bool SelectByParentOrg(char *orgcode,std::vector<S_MDB_ORG_INFO *> &orglist)
	{
		CBF_PMutex pp(&m_mutex);
		std::vector<int> iset;
		if (!m_index_up1org.Select(iset,orgcode))
		{
			return false;
		}
		for (int i=0; i< iset.size(); i++)
		{
			orglist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
	}
	bool GetAllOrg(std::vector<S_MDB_ORG_INFO *> &orglist)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		bool bRet = m_pkey.First(rid);
		while (bRet)
		{
			orglist.push_back(&(m_table.m_table[rid]));
			bRet = m_pkey.Next(rid);
		}
		return true;
	}
	// ������: SelectByUpOrg
	// ���  : ������ 2017-8-4 9:36:23
	// ����  : bool 
	// ����  : char *uporg
	// ����  : std::vector<S_ORG_INFO *> &orglist
	// ����  : ���ݻ�����ϣ��������е��¼���������Ϊ�ռ�Ϊ���л���
	bool SelectByUpOrg(char *uporg,std::vector<S_MDB_ORG_INFO *> &orglist)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		bool bret;
		if (uporg == NULL || strlen(uporg)<1)
		{
			bret = m_index_uporg.First(rid);
			while (bret)
			{
				orglist.push_back(&(m_table.m_table[rid]));
				bret = m_index_uporg.Next(rid);
			}
			return true;
		}
		int len = strlen(uporg);
		bret = m_index_uporg.FindFirst(rid,uporg);
		if (!bret)
		{
			return false;
		}
		while (bret)
		{
			if (strncmp(uporg,m_table.m_table[rid].sUpOrg,len) == 0)//ǰ����ȣ���ʾ�����ӻ���
			{
				orglist.push_back(&(m_table.m_table[rid]));
			}
			else
			{
				return true;
			}
			bret = m_index_uporg.Next(rid);
		}
		return true;
	}
	

	// ������: GetAllChildOrg
	// ���  : ������ 2018-1-29 14:14:01
	// ����  : int  int <0����,�޴˻���  0�ɹ�  1�ɹ��Ƕ�������
	// ����  : char *org  ��������
	// ����  : std::vector<S_MDB_ORG_INFO *> &orglist
	// ����  : bool isinclude=false  falseֻ����һ��  true�������¼�
	// ����  : ȡ�û������¼�����
	int  GetAllChildOrg(char *org,std::vector<S_MDB_ORG_INFO *> &orglist,bool isinclude=false)
	{
		CBF_PMutex pp(&m_mutex);
		int ret;
		int rid;
		bool bRet;
		char upOrg[201];

		if (!m_pkey.GetRowId(rid,org))
		{
			return -1;
		}
		if (m_table.m_table[rid].sOrgLevel[0] == '0') //��������
		{
			ret = 1;
			if (isinclude) //���������¼��������л���
			{
				bRet = m_pkey.First(rid);
				while (bRet)
				{
					orglist.push_back(&(m_table.m_table[rid]));
					bRet = m_pkey.Next(rid);
				}
			}
			else
			{
				std::vector<int> iset;
				if (!m_index_up1org.Select(iset,m_table.m_table[rid].sOrgCode)) //��һ������
				{
					return -1;
				}
				for (int i=0; i< iset.size(); i++)
				{
					orglist.push_back(&(m_table.m_table[iset[i]]));
				}
			}
			return ret;
		}
		else
		{
			ret = 0;
			if (isinclude)
			{
				strcpy(upOrg,m_table.m_table[rid].sUpOrg);
				strcat(upOrg,"|");
				strcat(upOrg,m_table.m_table[rid].sOrgCode);
				int len = strlen(upOrg);
				bRet = m_index_uporg.FindFirst(rid,upOrg); //�Ƚ��ϼ��������ҵ�ǰ����ȵ�
				if (!bRet)
				{
					return -1;
				}
				while (bRet)
				{
					if (strncmp(upOrg,m_table.m_table[rid].sUpOrg,len) == 0)//ǰ����ȣ���ʾ�����ӻ���
					{
						orglist.push_back(&(m_table.m_table[rid]));
					}
					else
					{
						return -1;
					}
					bRet = m_index_uporg.Next(rid);
				}
			}
			else
			{
				std::vector<int> iset;
				if (!m_index_up1org.Select(iset,m_table.m_table[rid].sOrgCode))
				{
					return -1;
				}
				for (int i=0; i< iset.size(); i++)
				{
					orglist.push_back(&(m_table.m_table[iset[i]]));
				}
			}
			return ret;
		}
	}
	S_MDB_ORG_INFO *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
};

//ϵͳ����
class  CTbl_SystemPara 
{
protected:
	
	CMemTableNew <S_MDB_SYSTEM_PARA> m_table;
//	CPkeyCharF<41,1>          m_pkey;               //��������
	CPkeyCharV<1>          m_pkey;               //��������
public:
	CTbl_SystemPara()
	{
		
	}
	virtual ~CTbl_SystemPara()
	{
		
	}
	void Clear()
	{
		m_pkey.Clear();
		m_table.Clear();
	}
	int Insert(S_MDB_SYSTEM_PARA para) //ֻ���ڳ�ʼ��ʱʹ�ã����Կ��Բ�����
	{
		if (m_pkey.Find(para.sParaCode))
		{
			return -1;
		}
		int rid=m_table.Add(para);
		m_pkey.Add(rid,m_table.m_table[rid].sParaCode);
		return rid;
	}
	
	// ������: *Select
	// ���  : ������ 2017-8-3 10:20:35
	// ����  : S_MDB_OPER_INFO 
	// ����  : const char *opercode
	// ����  : ���ݲ���Ա������Ҳ���Ա��Ϣ
	S_MDB_SYSTEM_PARA *Select(const char *paracode)
	{
		int rid;
		if (!m_pkey.GetRowId(rid,(char *)paracode))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}
	S_MDB_SYSTEM_PARA *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
};

//����Ա����
class CTbl_Oper_Online
{
protected:
	
	CMemTableNew <S_MDB_OPER_ONLINE>      m_table;
// 	CPkeyCharF<21,2>               m_pkey;//����Ա+����
// 	CIndexCharF<2,1>            m_index_loginstatus; //����¼״̬������
// 	CIndexCharF<21,1>            m_index_oper; //����Ա
	CPkeyCharV<2>               m_pkey;//����Ա+����
	CIndexVarChar<1>            m_index_loginstatus; //����¼״̬������
	CIndexVarChar<1>            m_index_oper; //����Ա
	CBF_Mutex                   m_mutex;//������
public:
	CTbl_Oper_Online()
	{
		
	}
	virtual ~CTbl_Oper_Online()
	{
		
	}
	void Clear()
	{
		m_pkey.Clear();
		m_index_loginstatus.Clear();
		m_index_oper.Clear();
		m_table.Clear();
	}
	int Insert(S_MDB_OPER_ONLINE operonline)
	{
		if (m_pkey.Find(operonline.sOperCode,operonline.sTermType))
		{
			return -1;
		}
		int rid = m_table.Add(operonline);
		m_pkey.Add(rid,m_table.m_table[rid].sOperCode,m_table.m_table[rid].sTermType);
		m_index_loginstatus.Add(rid,m_table.m_table[rid].sOnlineStatus);
		m_index_oper.Add(rid,m_table.m_table[rid].sOperCode);
		return rid;
	}
	S_MDB_OPER_ONLINE *Select(char *oper,char *channel)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		if (!m_pkey.GetRowId(rid,oper,channel))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}

	// ������: Select
	// ���  : ������ 2018-1-29 15:25:03
	// ����  : bool 
	// ����  : char *oper
	// ����  : std::vector<S_MDB_OPER_ONLINE *> &onlinelist
	// ����  : ���ݲ���Աȡ������Ա������Ϣ
	bool Select(char *oper,std::vector<S_MDB_OPER_ONLINE *> &onlinelist)
	{
		CBF_PMutex pp(&m_mutex);
		std::vector<int> iset;
		if (!m_index_oper.Select(iset,oper))
		{
			return false;
		}
		for (int i=0; i<iset.size(); i++)
		{
			onlinelist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
	}
	// ������: SelectByStatus
	// ���  : ������ 2018-1-29 15:25:03
	// ����  : bool 
	// ����  : char *status ״̬
	// ����  : std::vector<S_MDB_OPER_ONLINE *> &onlinelist
	// ����  : ���ݲ���Աȡ������Ա������Ϣ
	bool SelectByStatus(char *status,std::vector<S_MDB_OPER_ONLINE *> &onlinelist)
	{
		CBF_PMutex pp(&m_mutex);
		std::vector<int> iset;
		if (!m_index_loginstatus.Select(iset,status))
		{
			return false;
		}
		for (int i=0; i<iset.size(); i++)
		{
			onlinelist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
	}

	// ������: Update
	// ���  : ������ 2018-1-29 15:41:53
	// ����  : int  -1���� >0 rid 
	// ����  : S_MDB_OPER_ONLINE online
	// ����  : �������߱�û�����ӣ��и��¡��м���
	int Update(S_MDB_OPER_ONLINE online)
	{
		int rid;
		CBF_PMutex pp(&m_mutex);
		if (m_pkey.GetRowId(rid,online.sOperCode,online.sTermType))
		{
			//����
			if (strcmp(online.sOnlineStatus,m_table.m_table[rid].sOnlineStatus)!=0) //״̬��ͬ
			{
				m_table.m_table[rid].nDrebId = online.nDrebId;
				m_table.m_table[rid].nDrebPid = online.nDrebPid;
				m_table.m_table[rid].nGateIndex = online.nGateIndex;
				m_table.m_table[rid].nSvrId = online.nSvrId;
				m_table.m_table[rid].nSvrPid = online.nSvrPid;
				m_table.m_table[rid].nTimeStamp = online.nTimeStamp;
				strcpy(m_table.m_table[rid].sLoginIp,online.sLoginIp);
				strcpy(m_table.m_table[rid].sOnlineTime,online.sOnlineTime);
				strcpy(m_table.m_table[rid].sOnlineStatus,online.sOnlineStatus);
				//��ɾ������������
				m_index_loginstatus.Clear();
				int rowid;
				bool bRet = m_pkey.First(rowid);
				while (bRet)
				{
					m_index_loginstatus.Add(rowid,m_table.m_table[rowid].sOnlineStatus);
					bRet = m_pkey.Next(rowid);
				}

			}
			else
			{
				m_table.m_table[rid].nDrebId = online.nDrebId;
				m_table.m_table[rid].nDrebPid = online.nDrebPid;
				m_table.m_table[rid].nGateIndex = online.nGateIndex;
				m_table.m_table[rid].nSvrId = online.nSvrId;
				m_table.m_table[rid].nSvrPid = online.nSvrPid;
				m_table.m_table[rid].nTimeStamp = online.nTimeStamp;
				strcpy(m_table.m_table[rid].sLoginIp,online.sLoginIp);
				strcpy(m_table.m_table[rid].sOnlineTime,online.sOnlineTime);
			}
		}
		else
		{
			rid = m_table.Add(online);
			m_index_loginstatus.Add(rid,m_table.m_table[rid].sOperCode,m_table.m_table[rid].sTermType);
			m_index_oper.Add(rid,m_table.m_table[rid].sOperCode);
		}
		return rid;
	}
	S_MDB_OPER_ONLINE *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
};

class CDb_Public  
{
public:
	CDb_Public()
	{

	}
	virtual ~CDb_Public()
	{
		
	}
	CTbl_SystemPara   m_pSysPara;//ϵͳ����
	CTbl_Oper         m_pOper;//����Ա
	CTbl_Org          m_pOrg;//����
	CTbl_Oper_Online  m_pOperOnline;//����Ա���� 

};

#endif // !defined(AFX_DB_PUBLIC_H__78A43EE7_4192_47A9_8E6E_498B399BCEEB__INCLUDED_)
