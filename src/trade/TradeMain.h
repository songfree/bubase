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

class CTradeMain  : public CIClientLink
{
public:
	
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
