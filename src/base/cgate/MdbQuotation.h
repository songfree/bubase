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
	// 参数  : S_CGATE_QUOTATION &quo
	// 描述  : 判断行情的变化情况
	int IsNewQuotation(S_CGATE_QUOTATION &quo);


protected:
	CMemTable <S_CGATE_QUOTATION> m_table;        //行情最新内存表
	
	CPkeyInt<1>             m_pkey;   //合约编号
	
};

#endif // !defined(AFX_MDBQUOTATION_H__FDF3F654_CC36_408B_8B39_A4333C739AFB__INCLUDED_)
