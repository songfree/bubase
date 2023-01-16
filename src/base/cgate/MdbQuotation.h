// MdbQuotation.h: interface for the CMdbQuotation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MDBQUOTATION_H__FDF3F654_CC36_408B_8B39_A4333C739AFB__INCLUDED_)
#define AFX_MDBQUOTATION_H__FDF3F654_CC36_408B_8B39_A4333C739AFB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "MdbBase.h"

//行情表，保存最新的行情信息
typedef struct 
{
	int            nServiceNo;         //行情交易码
	int            nVerietyId;        //合约编号    s_nHook
	int            nDate;             //行情日期    s_nSerial
	unsigned int   nSerial;           //行情序号    n_nNextNo
}S_CGATE_QUOTATION;   //行情表

class CMdbQuotation  
{
public:
	CMdbQuotation();
	virtual ~CMdbQuotation();
	

	// 函数名: IsNewQuotation
	// 编程  : 王明松 2014-7-1 12:48:19
	// 返回  : int <0行情不是最新已过期，可丢弃   >0行情是最新 为行情数目
	// 参数  : int funcno 交易码
	// 参数  : int ndate 交易日期
	// 参数  : int nkey  合约或证券id
	// 参数  : int ntime  行情时间或序号
	// 描述  : 判断行情的变化情况	 只有行情时间小于时不用再发
	int IsNewQuotation(int funcno,int ndate,int nkey ,unsigned int ntime);


protected:
	CMemTable <S_CGATE_QUOTATION> m_table;        //行情最新内存表
	
	CPkeyIntUnordered<2>             m_pkey;   //合约编号
	
};

#endif // !defined(AFX_MDBQUOTATION_H__FDF3F654_CC36_408B_8B39_A4333C739AFB__INCLUDED_)
