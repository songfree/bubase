// XmlAttributes.h: interface for the CXmlAttributes class.
//
//////////////////////////////////////////////////////////////////////

/***********************************************************************************
  编码日志记录
  编码者: 王明松
  编码日期 2009-4-26 9:46:21
  版本: V1.00 
  说明: XML属性操作类，此类包含多个XML属性，提供XML属性操作方法 
 ***********************************************************************************/


#if !defined(AFX_XMLATTRIBUTES_H__CAA9058B_7E53_40F1_AAC4_4557FC758B83__INCLUDED_)
#define AFX_XMLATTRIBUTES_H__CAA9058B_7E53_40F1_AAC4_4557FC758B83__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "XmlAttribute.h"
#include "XmlAttrIndex.h"

#ifdef KERNEL_EXPORTS
#define KERNEL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNEL_EXPORT __declspec(dllimport)
#else
#define KERNEL_EXPORT 
#endif
#endif

class KERNEL_EXPORT CXmlAttributes  
{
public:
	bool GetUseIndexFlag();
	bool CreateIndex();

	// 函数名: GetEndAttribute
	// 编程  : 王明松 2010-6-12 15:16:34
	// 返回  : CXmlAttribute * 
	// 描述  : 取得最后一个属性
	CXmlAttribute * GetEndAttribute();

	// 函数名: GetFirstAttribute
	// 编程  : 王明松 2010-6-12 15:16:38
	// 返回  : CXmlAttribute * 
	// 描述  : 取得第一个属性
	CXmlAttribute * GetFirstAttribute();

	// 函数名: DeleteAttribute
	// 编程  : 王明松 2010-6-8 11:10:41
	// 返回  : bool 
	// 参数  : string attrname
	// 参数  : bool isTransSign=true
	// 描述  : 删除属性及属性索引
	bool DeleteAttribute(std::string attrname,bool isTransSign=true);

	// 函数名: Clear
	// 编程  : 王明松 2010-6-8 11:09:56
	// 返回  : void 
	// 描述  : 清除所有属性，包括索引
	void Clear();

	// 函数名: GetAttribute
	// 编程  : 王明松 2010-6-8 11:04:49
	// 返回  : CXmlAttribute * 若找到则返回属性，否则返回NULL
	// 参数  : string attrname  属性名称
	// 参数  : bool isTransSign 名称是否进行转义符转换
	// 参数  : string &attrvalue 属性值
	// 参数  : bool isTransValue=true 属性值是否进行转义符转换
	// 描述  : 根据属性名称查找属性值 
	CXmlAttribute * GetAttribute(std::string attrname,bool isTransSign,std::string &attrvalue,bool isTransValue=true);

	// 函数名: GetAttribute
	// 编程  : 王明松 2010-6-8 11:04:46
	// 返回  : CXmlAttribute * 若找到则返回属性，否则返回NULL
	// 参数  : std::string attrname 属性名称
	// 参数  : bool isTransSign=true 名称是否进行转义符转换
	// 描述  : 根据属性名称查找属性
	CXmlAttribute * GetAttribute(std::string attrname,bool isTransSign=true);

	// 函数名: SetAttribute
	// 编程  : 王明松 2010-6-8 10:36:50
	// 返回  : CXmlAttribute * 
	// 参数  : string attrname  属性名称
	// 参数  : bool isTransSign  名称是否进行转义符转换
	// 参数  : string attrvlaue   属性值
	// 参数  : bool isTransValue=true  属性值是否进行转义符转换
	// 描述  : 增加一个属性 若已有此名称的属性则设置值返回，否则新增并加入索引返回
	CXmlAttribute * SetAttribute(std::string attrname,bool isTransSign,std::string attrvlaue,bool isTransValue=true);

	CXmlAttribute * SetAttribute(std::string attrname,bool isTransSign,int attrvlaue);

	CXmlAttribute * SetAttribute(std::string attrname,bool isTransSign,long attrvlaue);

	CXmlAttribute * SetAttribute(std::string attrname,bool isTransSign,double attrvlaue,int dec=2);

	CXmlAttribute * AddAttribute(std::string attrname,bool isTransSign,std::string attrvlaue,bool isTransValue=true);
	
	CXmlAttributes();
	virtual ~CXmlAttributes();

protected:
	// 函数名: DeleteAllAttribute
	// 编程  : 王明松 2010-6-10 11:03:28
	// 返回  : void 
	// 描述  : 删除所有属性
	void DeleteAllAttribute();

	// 函数名: DeleteAttributePoint
	// 编程  : 王明松 2010-6-8 11:10:06
	// 返回  : void 
	// 参数  : CXmlAttribute *attr
	// 描述  : 删除属性的指针链接，然后就可以delete了
	void DeleteAttributePoint(CXmlAttribute *attr);

	CXmlAttribute *m_pFirstAttribute;//第一个属性
	CXmlAttribute *m_pEndAttribute;//最后一个属性
	CXmlAttrIndex *m_index;//属性的索引

	bool  m_bIsUseIndex;
	
};
#undef  KERNEL_EXPORT
#endif // !defined(AFX_XMLATTRIBUTES_H__CAA9058B_7E53_40F1_AAC4_4557FC758B83__INCLUDED_)
