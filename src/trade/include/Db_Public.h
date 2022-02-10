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

//数据库公共部分  机构、操作员、参数等


typedef struct 
{
	char sOperCode[21];//操作员代码
	char sOperName[41];//操作员名称
	char sOperPhone[21];//操作员电话
	char sOrgCode[13];//所属机构
	char sOperType[2];//操作员类型
}S_MDB_OPER_INFO;


typedef struct 
{
	char sOrgCode[13];//机构代码
	char sOrgName[41];//机构名称
	char sOrgType[2];//机构类型
	char sOrgLevel[2];//机构级别
	char sUp1Org[13];//上一级机构
	char sUpOrg[201];//上级机构  ｜分隔
	char sOrgCity[5];//城市
	char sAccNo[24];//结算账号
}S_MDB_ORG_INFO;


typedef struct
{
	char sOperCode[21];//操作员代码
	char sTermType[11];//登录渠道
	char sLoginIp[21];//登录IP
	char sOnlineStatus[2];//在线状态
	char sOnlineTime[18];//登录时间
	int  nDrebId;//总线公共节点号
	int  nDrebPid;//总线私有节点号
	int  nSvrId;//公共服务号
	int  nSvrPid;//私有服务号
	int  nGateIndex; //网关连接序号
	long nTimeStamp;//登录时间戳
}S_MDB_OPER_ONLINE;

typedef struct
{
	char sParaCode[41];//参数代码
	char sParaName[41];//参数名称
	char sParaValue[41];//参数值
	char sValueType[2];//值类型
}S_MDB_SYSTEM_PARA;

//操作员
class  CTbl_Oper  
{
protected:
	
	CMemTableNew <S_MDB_OPER_INFO> m_table;
//	CPkeyCharF<21,1>          m_pkey;               //操作员代码
	CPkeyCharV<1>          m_pkey;               //操作员代码
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
	int Insert(S_MDB_OPER_INFO oper) //只有在初始化时使用，所以可以不加锁
	{
		if (m_pkey.Find(oper.sOperCode))
		{
			return -1;
		}
		int rid=m_table.Add(oper);
		m_pkey.Add(rid,m_table.m_table[rid].sOperCode);
		return rid;
	}
	
	// 函数名: *Select
	// 编程  : 王明松 2017-8-3 10:20:35
	// 返回  : S_MDB_OPER_INFO 
	// 参数  : const char *opercode
	// 描述  : 根据操作员代码查找操作员信息
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

//机构信息表  
class CTbl_Org
{
protected:
	
	CMemTableNew <S_MDB_ORG_INFO>      m_table;
// 	CPkeyCharF<13,1>               m_pkey;//机构代码
// 	CIndexCharF<13,1>            m_index_up1org; //按上一级机构进行的索引 即查找下一级机构
// 	CIndexCharF<201,1>            m_index_uporg;  //按上级机构进行的索引 即查找所有下级机构
	CPkeyCharV<1>               m_pkey;//机构代码
	CIndexVarChar<1>            m_index_up1org; //按上一级机构进行的索引 即查找下一级机构
	CIndexVarChar<1>            m_index_uporg;  //按上级机构进行的索引 即查找所有下级机构
	CBF_Mutex                   m_mutex;//互斥锁
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

	// 函数名: SelectByParentOrg
	// 编程  : 王明松 2017-8-4 9:35:07
	// 返回  : bool 
	// 参数  : char *orgcode
	// 参数  : std::vector<S_MDB_ORG_INFO *> &orglist
	// 描述  : 根据机构代码查询此机构的所有下一级机构(不是所有下级，是下一级)
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
	// 函数名: SelectByUpOrg
	// 编程  : 王明松 2017-8-4 9:36:23
	// 返回  : bool 
	// 参数  : char *uporg
	// 参数  : std::vector<S_ORG_INFO *> &orglist
	// 描述  : 根据机构组合，查找所有的下级机构，若为空即为所有机构
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
			if (strncmp(uporg,m_table.m_table[rid].sUpOrg,len) == 0)//前面相等，表示是其子机构
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
	

	// 函数名: GetAllChildOrg
	// 编程  : 王明松 2018-1-29 14:14:01
	// 返回  : int  int <0出错,无此机构  0成功  1成功是顶级机构
	// 参数  : char *org  机构代码
	// 参数  : std::vector<S_MDB_ORG_INFO *> &orglist
	// 参数  : bool isinclude=false  false只是下一级  true是所有下级
	// 描述  : 取得机构的下级机构
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
		if (m_table.m_table[rid].sOrgLevel[0] == '0') //顶级机构
		{
			ret = 1;
			if (isinclude) //返回所有下级，即所有机构
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
				if (!m_index_up1org.Select(iset,m_table.m_table[rid].sOrgCode)) //下一级机构
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
				bRet = m_index_uporg.FindFirst(rid,upOrg); //比较上级机构，找到前面相等的
				if (!bRet)
				{
					return -1;
				}
				while (bRet)
				{
					if (strncmp(upOrg,m_table.m_table[rid].sUpOrg,len) == 0)//前面相等，表示是其子机构
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

//系统参数
class  CTbl_SystemPara 
{
protected:
	
	CMemTableNew <S_MDB_SYSTEM_PARA> m_table;
//	CPkeyCharF<41,1>          m_pkey;               //参数代码
	CPkeyCharV<1>          m_pkey;               //参数代码
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
	int Insert(S_MDB_SYSTEM_PARA para) //只有在初始化时使用，所以可以不加锁
	{
		if (m_pkey.Find(para.sParaCode))
		{
			return -1;
		}
		int rid=m_table.Add(para);
		m_pkey.Add(rid,m_table.m_table[rid].sParaCode);
		return rid;
	}
	
	// 函数名: *Select
	// 编程  : 王明松 2017-8-3 10:20:35
	// 返回  : S_MDB_OPER_INFO 
	// 参数  : const char *opercode
	// 描述  : 根据操作员代码查找操作员信息
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

//操作员在线
class CTbl_Oper_Online
{
protected:
	
	CMemTableNew <S_MDB_OPER_ONLINE>      m_table;
// 	CPkeyCharF<21,2>               m_pkey;//操作员+渠道
// 	CIndexCharF<2,1>            m_index_loginstatus; //按登录状态的索引
// 	CIndexCharF<21,1>            m_index_oper; //操作员
	CPkeyCharV<2>               m_pkey;//操作员+渠道
	CIndexVarChar<1>            m_index_loginstatus; //按登录状态的索引
	CIndexVarChar<1>            m_index_oper; //操作员
	CBF_Mutex                   m_mutex;//互斥锁
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

	// 函数名: Select
	// 编程  : 王明松 2018-1-29 15:25:03
	// 返回  : bool 
	// 参数  : char *oper
	// 参数  : std::vector<S_MDB_OPER_ONLINE *> &onlinelist
	// 描述  : 根据操作员取出操作员在线信息
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
	// 函数名: SelectByStatus
	// 编程  : 王明松 2018-1-29 15:25:03
	// 返回  : bool 
	// 参数  : char *status 状态
	// 参数  : std::vector<S_MDB_OPER_ONLINE *> &onlinelist
	// 描述  : 根据操作员取出操作员在线信息
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

	// 函数名: Update
	// 编程  : 王明松 2018-1-29 15:41:53
	// 返回  : int  -1出错 >0 rid 
	// 参数  : S_MDB_OPER_ONLINE online
	// 描述  : 更新在线表，没有增加，有更新。有加锁
	int Update(S_MDB_OPER_ONLINE online)
	{
		int rid;
		CBF_PMutex pp(&m_mutex);
		if (m_pkey.GetRowId(rid,online.sOperCode,online.sTermType))
		{
			//更新
			if (strcmp(online.sOnlineStatus,m_table.m_table[rid].sOnlineStatus)!=0) //状态不同
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
				//先删除索引再增加
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
	CTbl_SystemPara   m_pSysPara;//系统参数
	CTbl_Oper         m_pOper;//操作员
	CTbl_Org          m_pOrg;//机构
	CTbl_Oper_Online  m_pOperOnline;//操作员在线 

};

#endif // !defined(AFX_DB_PUBLIC_H__78A43EE7_4192_47A9_8E6E_498B399BCEEB__INCLUDED_)
