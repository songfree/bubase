// ISqlApi.h: interface for the CISqlApi class.
//
//////////////////////////////////////////////////////////////////////

/***********************************************************************************
  编码日志记录
  编码者: 王明松
  编码日期 2012-4-26 9:41:32
  版本: V1.00 
  说明: 数据库接口的抽象接口类，纯虚类
 ***********************************************************************************/



#if !defined(AFX_ISQLAPI_H__C493CE19_E7E4_407B_BEBE_8BC30BDCDA7A__INCLUDED_)
#define AFX_ISQLAPI_H__C493CE19_E7E4_407B_BEBE_8BC30BDCDA7A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef KERNEL_EXPORTS
#define KERNEL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNEL_EXPORT __declspec(dllimport)
#else
#define KERNEL_EXPORT 
#endif
#endif

class KERNEL_EXPORT CISqlApi  
{
public:
	CISqlApi()
	{

	}
	virtual ~CISqlApi()
	{

	}

};

#endif // !defined(AFX_ISQLAPI_H__C493CE19_E7E4_407B_BEBE_8BC30BDCDA7A__INCLUDED_)
