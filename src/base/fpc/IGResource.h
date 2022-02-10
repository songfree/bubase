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

    void *g_para1;//全局参数，由具体的一个线程进行分配释放，然后多线程共享
	void *g_para2;//全局参数，由具体的一个线程进行分配释放，然后多线程共享
	void *g_para3;//全局参数，由具体的一个线程进行分配释放，然后多线程共享
	
	CXmlPack m_xmlconf;
	
	CIGResource()
	{
		
	}
	virtual ~CIGResource()
	{
		
	}

	/// 函数名: SetGPara1
	/// 编程  : 王明松 2008-12-30 9:42:48
	/// 返回  : virtual bool 
	/// 参数  : bool setflag=true
	/// 描述  : 设置全局变量1,若已设置则返回false
	virtual bool SetGPara1(bool setflag=true)=0;


	/// 函数名: isSetGPara2
	/// 编程  : 王明松 2008-12-30 9:36:10
	/// 返回  : virtual bool 
	/// 参数  : bool setflag=true
	/// 描述  : 设置全局变量2,若已设置则返回false
	virtual bool SetGPara2(bool setflag=true)=0;


	/// 函数名: isSetGPara3
	/// 编程  : 王明松 2008-12-30 9:36:13
	/// 返回  : virtual bool 
	/// 参数  : bool setflag=true
	/// 描述  : 设置全局变量3,若已设置则返回false
	virtual bool SetGPara3(bool setflag=true)=0;

	/// 函数名: GetThreadShareSerial
	/// 编程  : 王明松 2008-12-30 9:36:13
	/// 返回  : virtual long 
	/// 描述  : 取得唯一的流水号
	virtual long GetThreadShareSerial()=0;
};

#endif // !defined(AFX_IGRESOURCE_H__D0010637_39E6_4E4E_94DD_5F537ABA6764__INCLUDED_)
