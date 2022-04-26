// ArbMain.h: interface for the CArbMain class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRADEMAIN_H__D1862398_E67A_4017_AC64_51E13AE7BEF3__INCLUDED_)
#define AFX_TRADEMAIN_H__D1862398_E67A_4017_AC64_51E13AE7BEF3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TradeResource.h"
#include "DrebMsgThread.h"
#include "BF_DrebServer.h"
#include "PoolModule.h"
#include "ProcThread.h"
#include "IInitial.h"

typedef struct
{
	CIInitial              *initPoint;//初始化动态库的类指针
	DLLHANDLE              initdllhandle;//动态库句柄
	std::string            initdllname;//动态库名称
	std::string            initInstance;//实例的函数名
	std::string            initdllpath;//动态库路径，绝对
}S_INITITAL_DLL;

typedef struct
{
	CIExchLink             *exchPoint;//初始化动态库的类指针
	DLLHANDLE              exchdllhandle;//动态库句柄
	std::string            exchdllname;//动态库名称
	std::string            exchInstance;//实例的函数名
	std::string            exchdllpath;//动态库路径，绝对
}S_EXCHANGE_DLL;

typedef struct 
{
	int sepms;//定时间隔
	int txcode;//交易码
}S_TIMER_CONF;

typedef struct
{
    int request_id;//rpc请求id
    int rpctime;//rpc请求时间
	std::function<int(S_TRADE_DATA& reqdata, S_TRADE_DATA& ansdata)> func;
	S_TRADE_DATA data;//请求报文数据
}S_RPC_DATA;
//合约表
class CTbl_RpcData
{
protected:

    CMemTableNew <S_RPC_DATA>  m_table;
    CPkeyInt<1>                 m_keyId;//请求ID
    CBF_Mutex                   m_mutex;//互斥锁
public:

	CBF_BufferPool* m_pMemPool;//内存分配池;
    CTbl_RpcData()
    {

    }
    virtual ~CTbl_RpcData()
    {

    }
    void Clear()
    {
        m_keyId.Clear();
        m_table.Clear();
    }
    int Insert(S_RPC_DATA info)
    {
        if (m_keyId.Find(info.request_id))
        {
            return -1;
        }
        int rid = m_table.Add(info);
        m_keyId.Add(rid, m_table.m_table[rid].request_id);
        return rid;
    }
    S_RPC_DATA* Select(int requestid)
    {
        int rid;
        CBF_PMutex pp(&m_mutex);
        if (!m_keyId.Select(rid, requestid))
        {
            return NULL;
        }
        return &(m_table.m_table[rid]);
    }
    bool Select(int requestid, S_RPC_DATA &data,bool delflag=true)
    {
        int rid;
        CBF_PMutex pp(&m_mutex);
        if (!m_keyId.Select(rid, requestid))
        {
            return false;
        }
		// memcpy(&data,&(m_table.m_table[rid]),sizeof(S_RPC_DATA));
		data.request_id = m_table.m_table[rid].request_id;
		data.rpctime = m_table.m_table[rid].rpctime;
		data.data.pData = m_table.m_table[rid].data.pData;
		data.func = m_table.m_table[rid].func;
		data.data.index = m_table.m_table[rid].data.index;
		data.data.nSendlen = m_table.m_table[rid].data.nSendlen;
		data.data.nWantlen = m_table.m_table[rid].data.nWantlen;
		data.data.rTime = m_table.m_table[rid].data.rTime;
		data.data.src = m_table.m_table[rid].data.src;
		if (delflag)
		{
            m_keyId.Delete(rid);
            m_table.Delete(rid);
		}
        return true;
    }

    S_RPC_DATA* At(int rowid)
    {
        if (rowid<0 || rowid > m_table.AllSize() - 1)
        {
            return NULL;
        }
        return &(m_table.m_table[rowid]);
    }
    int Delete(int requestid)
    {
        int rid;
        CBF_PMutex pp(&m_mutex);
        if (!m_keyId.Select(rid, requestid))
        {
            return -1;
        }

		m_keyId.Delete(requestid);
		m_table.Delete(rid);
        return 0;
    }
    int Delete()
    {
        int rid;
        CBF_PMutex pp(&m_mutex);
		CInt iset;
        bool bret = m_keyId.First(rid);
		while (bret)
		{
			if (m_table.m_table[rid].rpctime - time(NULL) > 3600) //1小时
			{
				iset.Add(rid);
			}
			bret = m_keyId.Next(rid);
		}
        m_keyId.Delete(iset);
		bret = iset.First(rid);
		while (bret)
		{
			if (m_pMemPool != NULL && m_table.m_table[rid].data.pData !=NULL)
			{
				m_pMemPool->PoolFree(m_table.m_table[rid].data.pData);
				m_table.m_table[rid].data.pData = NULL;
			}
			
			m_table.Delete(rid);
			bret = iset.Next(rid);
		}
        
        return 0;
    }
	int Size()
	{
		return m_keyId.Size();
	}
};

class CTradeMain  : public CIClientLink
{
public:
    // 函数名: AnsData
   // 编程  : 王明松 2017-7-20 9:28:49
   // 返回  : virtual int 
   // 参数  : S_TRADE_DATA &data  src=1时data.pData->sBpcHead.nIndex为socket连接序号
   // 参数  : char nextflag  0无后续包  1 有后续包  10最后一个数据包
   // 描述  : 应答数据  数据格式为总线头+数据  src=0表示总线  src=1表示socket直连
    virtual int AnsData(S_TRADE_DATA& data, char nextflag);

    // 函数名: RpcRequest
    // 编程  : 王明松 2017-7-20 9:28:49
    // 返回  : virtual int 
    // 参数  : S_TRADE_DATA &data  src=1时data.pData->sBpcHead.nIndex为socket连接序号
    // 参数  : char nextflag  0无后续包  1 有后续包  10最后一个数据包
    // 描述  : 应答数据  数据格式为总线头+数据  src=0表示总线  src=1表示socket直连
    virtual int RpcRequest(S_TRADE_DATA& data, int requestid, std::function<int(S_TRADE_DATA& reqdata, S_TRADE_DATA& ansdata)> func);

	int proctimer(int id);
	static int OnTimer(unsigned int event, void *p);


	// 函数名: GetAnsData
	// 编程  : 王明松 2017-8-14 9:00:10
	// 返回  : virtual int 
	// 参数  : int index
	// 参数  : S_TRADE_DATA &data
	// 参数  : int tms
	// 描述  : 取得应答
	virtual int GetAnsData(int index,S_TRADE_DATA &data,int tms);


	// 函数名: PushRcvQueue
	// 编程  : 王明松 2017-8-14 8:59:42
	// 返回  : virtual int 
	// 参数  : S_TRADE_DATA &data
	// 描述  : 放入请求队列
	virtual int PushRcvQueue(S_TRADE_DATA &data);


	// 函数名: PushAnsQueue
	// 编程  : 王明松 2017-8-14 9:40:00
	// 返回  : virtual int 
	// 参数  : S_TRADE_DATA &data   通过b_Cinfo.b_nIndex来对应是哪个请求 b_Cinfo.b_cIndex标识是哪个处理线程
	// 描述  : 放入应答队列
	virtual int PushAnsQueue(S_TRADE_DATA &data);

	// 函数名: Send2Client
	// 编程  : 王明松 2017-8-14 9:40:12
	// 返回  : virtual int 
	// 参数  : S_TRADE_DATA &data
	// 描述  : 发送给客户端或总线
	virtual int Send2Client(S_TRADE_DATA &data);


	// 函数名:  PoolMalloc
	// 编程  : 王明松 2018-4-13 15:50:23
	// 返回  : virtual int 
	// 参数  : S_TRADE_DATA &data
	// 描述  : 分配空间
	virtual int  PoolMalloc(S_TRADE_DATA &data);

	// 函数名: PoolFree
	// 编程  : 王明松 2018-4-13 15:50:27
	// 返回  : virtual void 
	// 参数  : S_TRADE_DATA &data
	// 描述  : 释放空间
	virtual void PoolFree(S_TRADE_DATA &data);

    //取递增的流水号
    virtual int  GetSerial();
    //取递增的本地号
    virtual UINT64_ GetLocalNo();

	void Monitor();
	void Stop();
	bool Start();
	bool Init(const char *confile);
	CTradeMain();
	virtual ~CTradeMain();

	
	CIErrlog         *m_pLog;
	
	CBF_DrebServer   m_pDrebApi;//api
	CDrebMsgThread   m_pDrebMsgThread;//总线消息线程
	CPoolModule      m_pListenThread;//侦听线程
	CTradeResource   m_pRes;   //配置
	CMemDb           m_pMemDb;//内存表

	CTbl_RpcData     m_pRpcData;//rpc调用的请求数据

	CProcThread *    m_lProcthread[50];//处理线程组
protected:
	S_BPC_RSMSG      m_drebdata;

	CPoolDataMsg     m_pRcvQueue;

	CBF_Timer        m_pTimer;//定时器
	CXdp             m_pXdp;
	
	
	bool              m_bIsInit;

	S_INITITAL_DLL    m_sInitDll;//初始化动态库信息
	S_EXCHANGE_DLL    m_sExchDll;//交易所动态库
	
	std::vector<S_TIMER_CONF> m_pTimerConf;//定时器配置

	CPubData   g_pPubData;//公共数据类

protected:
	bool OpenDll(DLLHANDLE *pDll, const char *dllname, const char *dllpath);

	char m_sSerialDataFile[256];
};

#endif // !defined(AFX_TRADEMAIN_H__D1862398_E67A_4017_AC64_51E13AE7BEF3__INCLUDED_)
