// Bpc_Timer.h: interface for the CBpc_Timer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DREB_TIMER_H__B7578973_40CD_4B03_8ECC_A560B1B70441__INCLUDED_)
#define AFX_DREB_TIMER_H__B7578973_40CD_4B03_8ECC_A560B1B70441__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "BF_Timer.h"
#include "GResource.h"
#include "DrebPubDefine.h"
#include "NextTbl.h"


typedef struct 
{
	UINT64_  g_nRcvBytes;//接收字节数
	UINT64_  g_nRcvPackNum;//接收数据包数
	UINT64_  g_nSendBytes;//发送字节数
	UINT64_  g_nSendPackNum;//发送数据包数

}S_BPCSTAT_INFO;



class CBpc_Timer  
{
public:
	void DeleteNextFile();
	void ReportSend(unsigned int sendbytes,unsigned int sendpack=1);
	void ReportRcv(unsigned int rcvbyte,unsigned int packnum=1);
	bool IsStoped();


	// 函数名: Init
	// 编程  : 王明松 2013-3-8 10:19:07
	// 返回  : bool 
	// 参数  : CGlobalVar *pVar
	// 描述  : 初始化
	bool Init(CGResource *pVar);
	CBpc_Timer();
	virtual ~CBpc_Timer();


	// 函数名: StopTimer
	// 编程  : 王明松 2013-3-8 10:19:16
	// 返回  : bool 
	// 描述  : 停止定时器
	bool StopTimer();

	// 函数名: StartTimer
	// 编程  : 王明松 2013-3-8 10:19:20
	// 返回  : bool 
	// 描述  : 启动定时器
	bool StartTimer();


	// 函数名: ComputeStat
	// 编程  : 王明松 2013-3-8 10:19:28
	// 返回  : void 
	// 描述  : 定时器取统计数据计算
	void ComputeStat();

	// 函数名: OnTimer
	// 编程  : 王明松 2013-3-6 16:43:23
	// 返回  : static int 
	// 参数  : unsigned int event
	// 参数  : void *p
	// 描述  : 定时器回调方法
	static int OnTimer(unsigned int event,void *p);

 	S_BPCSTAT_INFO  m_sBpcInfo;
 	S_BPCSTAT_INFO  m_sOldBpcInfo;
 	S_BPCSTAT_INFO  m_sResultBpcInfo;

	CNextTbl  m_pNextTbl;

	std::string      m_sNxtFilePath;

protected:
	CBF_Timer m_pTimer;//定时器
	CGResource *m_pRes;
	CBF_Mutex        m_mutex;

};

#endif // !defined(AFX_DREB_TIMER_H__B7578973_40CD_4B03_8ECC_A560B1B70441__INCLUDED_)
