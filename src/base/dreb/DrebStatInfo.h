// DrebStatInfo.h: interface for the CDrebStatInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DREBSTATINFO_H__F4320113_C67B_4C13_B218_E7E7B0E558DA__INCLUDED_)
#define AFX_DREBSTATINFO_H__F4320113_C67B_4C13_B218_E7E7B0E558DA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BF_Mutex.h"
#include "typedefine.h"
//统计信息
typedef struct 
{
	UINT64_   m_nRcvPackClient;//接收客户端包数
	UINT64_   m_nRcvByteClient;//接收客户端字节数
	UINT64_   m_nAnsPackClient;//应答客户端包数
	UINT64_   m_nAnsByteClient;//应答客户端字节数
	
	UINT64_   m_nTransOutNum; //数据转发包数
	UINT64_   m_nTransOutByte;//数据转发字节数
	UINT64_   m_nTransInNum;  //数据转发包数
	UINT64_   m_nTransInByte; //数据转发字节数
	
	UINT64_   m_nCallSrvNum;   //调用服务次数
    UINT64_   m_nCallSrvByte;  //调用服务字节数
	UINT64_   m_nSrvAnsNum;    //服务应答次数
    UINT64_   m_nSrvAnsByte;   //服务应答字节数

}S_DREBSTAT_INFO;

class CDrebStatInfo  
{
public:

	// 函数名: ReportSendByte
	// 编程  : 王明松 2012-3-6 10:35:46
	// 返回  : void 
	// 参数  : int ntype  连接类型
	// 参数  : int len    长度
	// 描述  : 报告发送长度
	void ReportSendByte(int ntype,int len);

	// 函数名: ReportSendPack
	// 编程  : 王明松 2012-3-6 10:36:19
	// 返回  : void 
	// 参数  : int ntype 连接类型
	// 参数  : int packnum
	// 描述  : 报告发送数据包数目
	void ReportSendPack(int ntype,int packnum);

	// 函数名: ReportRcvByte
	// 编程  : 王明松 2012-3-6 10:36:40
	// 返回  : void 
	// 参数  : int ntype 连接类型
	// 参数  : int len
	// 描述  : 报告接收字节数
	void ReportRcvByte(int ntype,int len);

	// 函数名: ReportRcvPack
	// 编程  : 王明松 2013-3-6 10:36:53
	// 返回  : void 
	// 参数  : int ntype 连接类型
	// 参数  : int packnum
	// 描述  : 报告接收数据包数
	void ReportRcvPack(int ntype,int packnum);

	CDrebStatInfo();
	virtual ~CDrebStatInfo();

	S_DREBSTAT_INFO m_drebinfo;
	
	int              m_nStartTime;           //启动时间
	char             m_sStartDate[20];           //启动日期
protected:
	CBF_Mutex m_mutex;

	
};

#endif // !defined(AFX_DREBSTATINFO_H__F4320113_C67B_4C13_B218_E7E7B0E558DA__INCLUDED_)
