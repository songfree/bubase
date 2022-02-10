// ShareData.h: interface for the CShareData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHAREDATA_H__1120841C_B2D8_4915_9C85_8F7B8B53711C__INCLUDED_)
#define AFX_SHAREDATA_H__1120841C_B2D8_4915_9C85_8F7B8B53711C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "public.h"
#include "BF_Mutex.h"
#include "XmlPack.h"
#include "IVarDefine.h"

struct SERVER_PARA
{
	char sip[40];
	unsigned int port;
	unsigned int headlen;
	unsigned int timeout;
	unsigned int timesep;
	unsigned int loglevel;
	unsigned int runtime;
};

class CShareData  
{
public:
	CShareData();
	virtual ~CShareData();

public:
	bool InitVar(char *varxml);
	void Clear();
	void Run(bool isstop,int sendnum,int sendbyte,int rcvnum,int rcvbyte,int errnum,int resptime);

	int m_nAllRunTime;//总运行时间
	int m_nCurRunTime;//当前运行时间
	int m_nTotalThreadNum;//总线程数
	int m_nSendNum;//发送笔数
	int m_nRcvNum;//接收笔数
	int m_nErrNum;//出错笔数
	int m_nMinRespTime;//最小响应时间
	int m_nMaxRespTime;//最大响应时间
	int m_nAvRespTime;//平均响应时间
	int m_nCurNumSec;//当前每秒笔数
	int m_nAvNumSec;//平增每秒笔数

	__int64 m_nSendByte;//发送字节数
	__int64 m_nRcvByte;//接收字节数

	int m_nTotalRespTIme;//总响应时间

	bool m_bIsExit;//是否退出
	bool m_bIsRun;//是否开始发送
	int  m_nRunTime;//开始发送的时间点

	int m_nConnNum;//连接服务器的次数

	SERVER_PARA m_pServerPara;//服务器参数


	CBF_Mutex m_runMutex;//run方法的互斥变量

	std::vector<CIVarDefine *>m_varList;//变量列表
protected:
	CXmlPack m_varxml;




};

#endif // !defined(AFX_SHAREDATA_H__1120841C_B2D8_4915_9C85_8F7B8B53711C__INCLUDED_)
