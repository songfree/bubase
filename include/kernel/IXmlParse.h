// IXmlParse.h: interface for the CIXmlParse class.
//
//////////////////////////////////////////////////////////////////////

/***********************************************************************************
  编码日志记录
  编码者: 王明松
  编码日期 2009-4-26 9:42:47
  版本: V1.00 
  说明: xml解析类的抽象接口类
 ***********************************************************************************/


#if !defined(AFX_IXMLPARSE_H__DF104931_39B6_4585_A90D_26C398C146A7__INCLUDED_)
#define AFX_IXMLPARSE_H__DF104931_39B6_4585_A90D_26C398C146A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CIXmlNode;

#ifdef KERNEL_EXPORTS
#define KERNEL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNEL_EXPORT __declspec(dllimport)
#else
#define KERNEL_EXPORT 
#endif
#endif

class KERNEL_EXPORT CIXmlParse  
{
public:
	CIXmlParse()
	{

	}
	virtual ~CIXmlParse()
	{

	}
	virtual bool ParseXml(std::string xmlbuf,CIXmlNode *parentNode,int layer=0)=0;

};

#endif // !defined(AFX_IXMLPARSE_H__DF104931_39B6_4585_A90D_26C398C146A7__INCLUDED_)
