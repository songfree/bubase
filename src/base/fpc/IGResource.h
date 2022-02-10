// IPoolMgr.h: interface for the CIPoolMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IGRESOURCE_H__D0010637_39E6_4E4E_94DD_5F537ABA6764__INCLUDED_)
#define AFX_IGRESOURCE_H__D0010637_39E6_4E4E_94DD_5F537ABA6764__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
class CXmlPack;

class CIGResource  
{
public:

    void *g_para1;//ȫ�ֲ������ɾ����һ���߳̽��з����ͷţ�Ȼ����̹߳���
	void *g_para2;//ȫ�ֲ������ɾ����һ���߳̽��з����ͷţ�Ȼ����̹߳���
	void *g_para3;//ȫ�ֲ������ɾ����һ���߳̽��з����ͷţ�Ȼ����̹߳���
	
	CXmlPack m_xmlconf;
	
	CIGResource()
	{
		
	}
	virtual ~CIGResource()
	{
		
	}

	/// ������: SetGPara1
	/// ���  : ������ 2008-12-30 9:42:48
	/// ����  : virtual bool 
	/// ����  : bool setflag=true
	/// ����  : ����ȫ�ֱ���1,���������򷵻�false
	virtual bool SetGPara1(bool setflag=true)=0;


	/// ������: isSetGPara2
	/// ���  : ������ 2008-12-30 9:36:10
	/// ����  : virtual bool 
	/// ����  : bool setflag=true
	/// ����  : ����ȫ�ֱ���2,���������򷵻�false
	virtual bool SetGPara2(bool setflag=true)=0;


	/// ������: isSetGPara3
	/// ���  : ������ 2008-12-30 9:36:13
	/// ����  : virtual bool 
	/// ����  : bool setflag=true
	/// ����  : ����ȫ�ֱ���3,���������򷵻�false
	virtual bool SetGPara3(bool setflag=true)=0;

	/// ������: GetThreadShareSerial
	/// ���  : ������ 2008-12-30 9:36:13
	/// ����  : virtual long 
	/// ����  : ȡ��Ψһ����ˮ��
	virtual long GetThreadShareSerial()=0;
};

#endif // !defined(AFX_IGRESOURCE_H__D0010637_39E6_4E4E_94DD_5F537ABA6764__INCLUDED_)
