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
	/// ������: GetThreadShareSerial
	/// ���  : ������ 2008-12-30 9:36:13
	/// ����  : virtual long 
	/// ����  : ȡ��Ψһ����ˮ��
	virtual long GetThreadShareSerial()=0;
};

#endif // !defined(AFX_IBULINK_H__AE095518_D92C_44F8_96E3_8906B03B2ABB__INCLUDED_)
