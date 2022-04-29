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

//����ҵ��̬�����
class CIProcBase;

typedef  int(CIProcBase::*FuncPointer)(S_TRADE_DATA &data);

typedef struct _S_TBL_FUNC
{   
	int         nFuncNo; //���ܺ� 
	FuncPointer pFunc;///����ָ��
	char        sFuncName[81];///��������
	double      dVersion;///�汾
	char        sAuthor[41];///����
	int         nLevel;///���ȼ�
	CIProcBase *pProcInstance;///������ʵ��
	char        sDllName[201];///��̬������
	unsigned int nCallNum;//���ô���
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
	S_PUBLIC_SERIAL m_sPublicSerial;//������ˮ�ṹ
};

class  CProcFuncTbl  
{
protected:
	
	CMemTableNew <S_TBL_PROCFUNC> m_table;
	CPkeyIntUnordered<1>            m_pkey;               //���ܺŵ�key
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
		int id = m_table.Add(funcinfo);//���ӵ���
		m_pkey.Add(id,funcinfo.nFuncNo);//��������
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
		m_pXdp = NULL;//xdp����
		m_pSqlCmd = NULL; //���ݿ�����
		m_pExchLink = NULL;//���������ӵ���
		m_pClientLink = NULL; //�ͻ������ӵ���
		m_pMemPool = NULL;//�ڴ滺���
		m_pMemDb = NULL;//�ڴ��
		m_pLog = NULL;
		g_nSvrHostStatus =  NULL;
		g_pPubData = NULL;
	}
	virtual ~CIProcBase()
	{
		m_pXdp = NULL;//xdp����
		m_pSqlCmd = NULL; //���ݿ�����
		m_pExchLink = NULL;//���������ӵ���
		m_pClientLink = NULL; //�ͻ������ӵ���
		m_pMemPool = NULL;//�ڴ滺���
		m_pMemDb = NULL;//�ڴ��
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


	// ������: GetAnsData
	// ���  : ������ 2017-8-31 9:02:29
	// ����  : int  
	// ����  : S_TRADE_DATA &data
	// ����  : int tms
	// ����  : ȡ�ñ������̵߳�Ӧ��ͬ���ȴ���ȡ����Ҫ�ж���ˮ�ǲ��Ǳ��������Ӧ��
	int  GetAnsData(S_TRADE_DATA &data,int tms)
	{
		return m_pClientLink->GetAnsData(m_nIndex,data,tms);
	}
	int Send2Exch(S_TRADE_DATA &data,CXdp *xdp)
	{
		//songfree 20180510 ��Ϊb_Cinfo.b_cIndex����index
		data.pData->sDBHead.b_Cinfo.b_cIndex = m_nIndex;
		//data.pData->sDBHead.s_Sinfo.s_nGateIndex = m_nIndex;
		return m_pExchLink->Send2Exch(data,xdp);
	}

	CProcFuncTbl    m_pFuncTbl;//�������
	
	unsigned  int *g_nSvrHostStatus;//����״̬
	
	CPubData   *g_pPubData;//����������
protected:
	CXdp           m_pSendXdp;//����xdp���ģ�������̨/�������ȵı���
	CXdp           *m_pXdp;//xdp����
	CDbApi         *m_pSqlCmd; //���ݿ�����
	CIExchLink     *m_pExchLink;//���������ӵ���
	CIClientLink   *m_pClientLink; //�ͻ������ӵ���
	CBF_BufferPool *m_pMemPool;//�ڴ滺���
	CMemDb         *m_pMemDb;//�ڴ��
	CIErrlog       *m_pLog;
	int             m_nIndex;//�߳���������ʶ���ĸ��߳�

};

#endif // !defined(AFX_IPROCBASE_H__CFA6CB37_70F8_4291_BE24_D77ED7C3DC35__INCLUDED_)
