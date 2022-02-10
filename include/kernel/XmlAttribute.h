// XmlAttribute.h: interface for the CXmlAttribute class.
//
//////////////////////////////////////////////////////////////////////

/***********************************************************************************
  编码日志记录
  编码者: 王明松
  编码日期 2009-4-26 9:47:34
  版本: V1.00 
  说明: XML属性类 对应一个XML属性
 ***********************************************************************************/


#if !defined(AFX_XMLATTRIBUTE_H__401DD803_E06B_4702_9E04_74A53D92094E__INCLUDED_)
#define AFX_XMLATTRIBUTE_H__401DD803_E06B_4702_9E04_74A53D92094E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "XmlTransSign.h"
#ifdef KERNEL_EXPORTS
#define KERNEL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNEL_EXPORT __declspec(dllimport)
#else
#define KERNEL_EXPORT 
#endif
#endif


class KERNEL_EXPORT CXmlAttribute  
{
public:
	CXmlAttribute();
	virtual ~CXmlAttribute();

public:

	// 函数名: DeletePoint
	// 编程  : 王明松 2010-6-10 10:18:27
	// 返回  : void 
	// 描述  : 从属性链表中删除此属性链接
	void DeletePoint();

	// 函数名: SetLastAttribute
	// 编程  : 王明松 2010-6-8 11:35:02
	// 返回  : void 
	// 参数  : CXmlAttribute *lastattr
	// 描述  : 设置属性链表的上一个属性
	void SetLastAttribute(CXmlAttribute *lastattr);
	
	// 函数名: SetNextAttribute
	// 编程  : 王明松 2010-6-8 10:53:40
	// 返回  : void
	// 参数  : CXmlAttribute *nextattr
	// 描述  : 设置属性链表的下一个属性
	void SetNextAttribute(CXmlAttribute *nextattr);
	
	// 函数名: SetAttrValue
	// 编程  : 王明松 2010-6-7 14:54:49
	// 返回  : void 
	// 参数  : std::string value
	// 参数  : bool isTransSign=true 转义符转换
	// 描述  : 设置属性值
	void SetAttrValue(std::string value,bool isTransSign=true);
	
	// 函数名: SetAttrName
	// 编程  : 王明松 2010-6-7 14:54:40
	// 返回  : void 
	// 参数  : std::string attrname
	// 参数  : bool isTransSign=true 转义符转换
	// 描述  : 设置属性名称
	void SetAttrName(std::string attrname,bool isTransSign=true);
	
	// 函数名: GetAttrValue
	// 编程  : 王明松 2010-6-7 14:50:31
	// 返回  : std::string 
	// 参数  : bool isTransSign=true 转义符转换
	// 描述  : 取得属性值
	std::string GetAttrValue(bool isTransSign=true);
	
	// 函数名: GetAttrName
	// 编程  : 王明松 2010-6-7 14:48:04
	// 返回  : std::string 
	// 参数  : bool isTransSign=true 转义符转换
	// 描述  : 取得属性名称
	std::string GetAttrName(bool isTransSign=true);
	
	// 函数名: getNextSibling
	// 编程  : 王明松 2010-6-7 14:47:07
	// 返回  : CAttribute * 
	// 描述  : 获取下一个属性
	CXmlAttribute * getNextSibling();

	// 函数名: getLastSibling
	// 编程  : 王明松 2010-6-8 11:15:16
	// 返回  : CXmlAttribute * 
	// 描述  : 获取上一个属性
	CXmlAttribute * getLastSibling();


protected:
	std::string m_sAttrValue;
	std::string m_sAttrName;
	
	CXmlAttribute *m_next;///下一属性指针
	CXmlAttribute *m_last;///上一属性指针
	

};
#undef  KERNEL_EXPORT
#endif // !defined(AFX_XMLATTRIBUTE_H__401DD803_E06B_4702_9E04_74A53D92094E__INCLUDED_)
