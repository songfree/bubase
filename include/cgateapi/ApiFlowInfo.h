// ApiFlowInfo.h: interface for the CApiFlowInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APIFLOWINFO_H__C698E6D2_B610_45A7_A4CE_9E6340335C3F__INCLUDED_)
#define AFX_APIFLOWINFO_H__C698E6D2_B610_45A7_A4CE_9E6340335C3F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "GateHead.h"

#ifdef CGATEAPI_EXPORTS
#define CGATEAPI_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define CGATEAPI_EXPORT __declspec(dllimport)
#else
#define CGATEAPI_EXPORT 
#endif
#endif

class CGATEAPI_EXPORT CApiFlowInfo  
{
public:

	// 函数名: GetSpeedRateTx
	// 编程  : 王明松 2013-5-27 17:36:55
	// 返回  : unsigned int 
	// 描述  : 获取当前每秒发送交易数
	unsigned int GetSpeedRateTx();

	// 函数名: GetSpeedRateBytes
	// 编程  : 王明松 2013-5-27 17:36:39
	// 返回  : unsigned int 
	// 描述  : 获取当前每秒发送字节数
	unsigned int GetSpeedRateBytes();

	// 函数名: ReportTimer
	// 编程  : 王明松 2013-5-27 17:29:47
	// 返回  : void 
	// 描述  : 定时器调用，计算每秒发送字节数和发送交易数
	void ReportTimer();

	// 函数名: ReportStat
	// 编程  : 王明松 2013-5-27 16:12:20
	// 返回  : void 
	// 参数  : unsigned int sbytes
	// 描述  : 报告发送数据
	void ReportStat(unsigned int sbytes);

	CApiFlowInfo();
	virtual ~CApiFlowInfo();


	
private:
	unsigned  int   m_nSendPackNum;             //总发送交易数
	unsigned  int   m_nSendPackBytes;           //总发送字节数

	unsigned  int   m_nPackNumSpeedRate;        //每秒发送交易数
	unsigned  int   m_nPackByteSpeedRate;       //每秒发送字节数

	unsigned  int   m_nOldSendPackNum;             //上一秒总发送交易数
	unsigned  int   m_nOldSendPackBytes;           //上一秒总发送字节数

};

#endif // !defined(AFX_APIFLOWINFO_H__C698E6D2_B610_45A7_A4CE_9E6340335C3F__INCLUDED_)
