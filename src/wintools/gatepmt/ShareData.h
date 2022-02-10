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
#include "BF_Xml.h"
#include "IVarDefine.h"

struct SERVER_PARA
{
	unsigned int timeout;//超时
	unsigned int timesep;//发送间隔
	unsigned int runtime;//运行时间
	unsigned int connectNum;//连接次数
	unsigned int loglevel;//日志级别
	unsigned int rrtime;//记录超过此响应时间的记录
};

typedef struct _THREAD_DATA_PARA
{
	std::string sApiXml;//网关api配置文件
	unsigned int   nServiceNo;//发压交易
	unsigned int   nLoginTx;//登录交易
	unsigned int   nThreadNum;//启动线程数
	unsigned int   nStartTime;//启动时间点

};

class CShareData  
{
public:
	CShareData();
	virtual ~CShareData();

public:
	int GetSerial();

	// 函数名: InitVar
	// 编程  : 王明松 2010-9-30 15:25:08
	// 返回  : bool 
	// 参数  : char *varxml
	// 描述  : 初始变量
	bool InitVar(char *varxml);
	void Clear();

	// 函数名: Run
	// 编程  : 王明松 2010-9-30 15:24:52
	// 返回  : void 
	// 参数  : bool isstop
	// 参数  : int sendnum     发送请求数
	// 参数  : int sendbyte    发送字节数
	// 参数  : int rcvnum      接收应答数
	// 参数  : int rcvbyte     接收字节数
	// 参数  : int errnum      出错数
	// 参数  : int resptime    响应时间
	// 参数  : int rsppacknum  响应包数目
	// 描述  : 报告运行状况
	void Run(bool isstop,int sendnum,int sendbyte,int rcvnum,int rcvbyte,int errnum, INT64_ resptime,int rsppacknum);


	void RunPush(int rcvbyte,int rcvnum);

	int m_nAllRunTime;//总运行时间
	int m_nCurRunTime;//当前运行时间
	int m_nTotalThreadNum;//总线程数
	int m_nSendNum;//发送笔数
	int m_nRcvNum;//接收笔数
	int m_nErrNum;//出错笔数
	INT64_ m_nMinRespTime;//最小响应时间
	INT64_ m_nMaxRespTime;//最大响应时间
	INT64_ m_nAvRespTime;//平均响应时间
	int m_nCurNumSec;//当前每秒笔数
	int m_nAvNumSec;//平增每秒笔数

	int m_nRcvPushNum;//接收推送报文数
	int m_nRcvAnsPackNum;//接收到的应答报文总数

	INT64_ m_nSendByte;//发送字节数
	INT64_ m_nRcvByte;//接收字节数

	int m_nTotalRespTIme;//总响应时间

	bool m_bIsExit;//是否退出
	bool m_bIsRun;//是否开始发送

 	int  m_nRunTime;//开始发送的时间点,即开始测试的时间点

	unsigned int m_nSerial;//发送流水

	SERVER_PARA m_pServerPara;//服务器参数


	CBF_Mutex m_runMutex;//run方法的互斥变量

	CBF_Mutex m_serialMutex;//流水互斥变量
	std::vector<CIVarDefine *>m_varList;//变量列表
protected:
	
	CBF_Xml   m_varxml;




};

#endif // !defined(AFX_SHAREDATA_H__1120841C_B2D8_4915_9C85_8F7B8B53711C__INCLUDED_)
