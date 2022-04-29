// IProcBase.h: interface for the CIProcBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPROCBASE_H__CFA6CB37_70F8_4291_BE24_D77ED7C3DC35__INCLUDED_)
#define AFX_IPROCBASE_H__CFA6CB37_70F8_4291_BE24_D77ED7C3DC35__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MemDb.h"
#include "DbApi.h"
#include "IClientLink.h"
#include "IExchLink.h"
#include "BF_BufferPool.h"

//处理业务动态库基类
class CIProcBase;

typedef  int(CIProcBase::*FuncPointer)(S_TRADE_DATA &data);

typedef struct _S_TBL_FUNC
{   
	int         nFuncNo; //功能号 
	FuncPointer pFunc;///函数指针
	char        sFuncName[81];///功能名称
	double      dVersion;///版本
	char        sAuthor[41];///作者
	int         nLevel;///优先级
	CIProcBase *pProcInstance;///处理类实例
	char        sDllName[201];///动态库名称
	unsigned int nCallNum;//调用次数
}S_TBL_PROCFUNC;


typedef struct  
{
	UINT64_  nLocalSerial;

}S_PUBLIC_SERIAL;


class CPubData
{
public:
	CPubData()
	{
		bzero(&m_sPublicSerial,sizeof(S_PUBLIC_SERIAL));
	}
	virtual ~CPubData()
	{

	}
	UINT64_ GetLocalSerial()
	{
		CBF_PMutex pp(&m_mutex);
		UINT64_ aa= m_sPublicSerial.nLocalSerial;
		 m_sPublicSerial.nLocalSerial++;
		 if (m_sPublicSerial.nLocalSerial> 999999999)
		 {
			m_sPublicSerial.nLocalSerial =1;
		 }
		 return aa;
	}
	int UpdateLocalSerial(UINT64_ serial)
	{
		CBF_PMutex pp(&m_mutex);
		if (serial> m_sPublicSerial.nLocalSerial)
		{
			m_sPublicSerial.nLocalSerial=serial;
		}
		else
		{
			if (m_sPublicSerial.nLocalSerial - serial >90000000)
			{
				m_sPublicSerial.nLocalSerial=serial;
			}
		}
		return 0;
	}
	bool InitFromFile(const char *datafile)
	{
		FILE *fp = fopen(datafile,"rb");
		if (fp == NULL)
		{
			return false;
		}
		int ret =fread((char *)&m_sPublicSerial,1,sizeof(S_PUBLIC_SERIAL),fp);
		if (ret != sizeof(S_PUBLIC_SERIAL))
		{
			return false;
		}
		return true;
	}
	void WriteToFile(const char *datafile)
	{
		FILE *fp = fopen(datafile,"wb");
		if (fp == NULL)
		{
			return ;
		}
		int ret = fwrite(&m_sPublicSerial,1,sizeof(S_PUBLIC_SERIAL),fp);
		fclose(fp);
		if (ret != sizeof(S_PUBLIC_SERIAL))
		{
			return ;
		}
		return;
	}
protected:
	CBF_Mutex m_mutex;
	S_PUBLIC_SERIAL m_sPublicSerial;//公共流水结构
};

class  CProcFuncTbl  
{
protected:
	
	CMemTableNew <S_TBL_PROCFUNC> m_table;
	CPkeyIntUnordered<1>            m_pkey;               //功能号的key
public:
	CProcFuncTbl()
	{
		
	}
	virtual ~CProcFuncTbl()
	{
		
	}
	void Clear()
	{
		m_pkey.Clear();
		m_table.Clear();
	}
	bool Insert(S_TBL_PROCFUNC funcinfo)
	{
		if (m_pkey.Find(funcinfo.nFuncNo))
		{
			return false;
		}
		int id = m_table.Add(funcinfo);//增加到表
		m_pkey.Add(id,funcinfo.nFuncNo);//增加主键
		return true;
	}
	bool Delete(int funcno)
	{
		CInt iset;
		if (!m_pkey.Select(iset,funcno))
		{
			return false;
		}
		int rid;
		iset.First(rid);
		m_table.Delete(rid);
		m_pkey.Delete(funcno);
		return true;
	}
	S_TBL_PROCFUNC *Find(int funcno)
	{
		int rid;
		if (!m_pkey.Select(rid,funcno))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}
	S_TBL_PROCFUNC *First()
	{
		return m_table.First();
	}
	S_TBL_PROCFUNC *Next()
	{
		return m_table.Next();
	}


};

class CIProcBase  
{
public:
	CIProcBase()
	{
		m_pXdp = NULL;//xdp报文
		m_pSqlCmd = NULL; //数据库连接
		m_pExchLink = NULL;//交易所连接调用
		m_pClientLink = NULL; //客户端连接调用
		m_pMemPool = NULL;//内存缓冲池
		m_pMemDb = NULL;//内存表
		m_pLog = NULL;
		g_nSvrHostStatus =  NULL;
		g_pPubData = NULL;
	}
	virtual ~CIProcBase()
	{
		m_pXdp = NULL;//xdp报文
		m_pSqlCmd = NULL; //数据库连接
		m_pExchLink = NULL;//交易所连接调用
		m_pClientLink = NULL; //客户端连接调用
		m_pMemPool = NULL;//内存缓冲池
		m_pMemDb = NULL;//内存表
		m_pLog = NULL;
		g_nSvrHostStatus =  NULL;
		g_pPubData = NULL;
	}
	void InitFuncInfo(int funcNo, FuncPointer pfunc, const char *funcName, double version, const char *author, int level)
	{
		S_TBL_PROCFUNC pFunc;
		bzero(&pFunc,sizeof(S_TBL_PROCFUNC));
		pFunc.nFuncNo = funcNo;
		pFunc.pFunc = pfunc;
		CBF_Tools::StringCopy(pFunc.sFuncName,80,funcName);
		pFunc.dVersion = version;
		CBF_Tools::StringCopy(pFunc.sAuthor,40,author);
		pFunc.nLevel = level;
		pFunc.pProcInstance = NULL;
		strcpy(pFunc.sDllName,"");
		m_pFuncTbl.Insert(pFunc);
	}
	virtual bool Init(CBF_Xml *xml,CIClientLink *pBusLink,CMemDb *pMemDb,	CXdp *pXdp,	CIErrlog *pLog,CBF_BufferPool *pMemPool,CDbApi *pSqlCmd,CIExchLink *pExchLink,int index)
	{
		m_pClientLink = pBusLink;
		//m_pXdp->XdpCopy(pXdp);
		m_pMemDb= pMemDb;
		m_pLog = pLog;
		m_pSqlCmd = pSqlCmd;
		m_pExchLink = pExchLink;
		//m_pMemPool = pMemPool;
		m_nIndex = index;
		return true;
	}


	// 函数名: GetAnsData
	// 编程  : 王明松 2017-8-31 9:02:29
	// 返回  : int  
	// 参数  : S_TRADE_DATA &data
	// 参数  : int tms
	// 描述  : 取得本处理线程的应答，同步等待，取到后要判断流水是不是本次请求的应答
	int  GetAnsData(S_TRADE_DATA &data,int tms)
	{
		return m_pClientLink->GetAnsData(m_nIndex,data,tms);
	}
	int Send2Exch(S_TRADE_DATA &data,CXdp *xdp)
	{
		//songfree 20180510 改为b_Cinfo.b_cIndex保存index
		data.pData->sDBHead.b_Cinfo.b_cIndex = m_nIndex;
		//data.pData->sDBHead.s_Sinfo.s_nGateIndex = m_nIndex;
		return m_pExchLink->Send2Exch(data,xdp);
	}

	CProcFuncTbl    m_pFuncTbl;//交易码表
	
	unsigned  int *g_nSvrHostStatus;//主备状态
	
	CPubData   *g_pPubData;//公共数据类
protected:
	CXdp           m_pSendXdp;//发送xdp报文，发给柜台/交易所等的报文
	CXdp           *m_pXdp;//xdp报文
	CDbApi         *m_pSqlCmd; //数据库连接
	CIExchLink     *m_pExchLink;//交易所连接调用
	CIClientLink   *m_pClientLink; //客户端连接调用
	CBF_BufferPool *m_pMemPool;//内存缓冲池
	CMemDb         *m_pMemDb;//内存表
	CIErrlog       *m_pLog;
	int             m_nIndex;//线程索引，标识是哪个线程

};

#endif // !defined(AFX_IPROCBASE_H__CFA6CB37_70F8_4291_BE24_D77ED7C3DC35__INCLUDED_)
