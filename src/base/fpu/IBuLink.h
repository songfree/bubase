// IBuLink.h: interface for the CIBuLink class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IBULINK_H__AE095518_D92C_44F8_96E3_8906B03B2ABB__INCLUDED_)
#define AFX_IBULINK_H__AE095518_D92C_44F8_96E3_8906B03B2ABB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CIBuLink  
{
public:

	CIBuLink()
	{
		
	}
	virtual ~CIBuLink()
	{
		
	}
	/// 函数名: GetThreadShareSerial
	/// 编程  : 王明松 2008-12-30 9:36:13
	/// 返回  : virtual long 
	/// 描述  : 取得唯一的流水号
	virtual long GetThreadShareSerial()=0;
};

#endif // !defined(AFX_IBULINK_H__AE095518_D92C_44F8_96E3_8906B03B2ABB__INCLUDED_)
