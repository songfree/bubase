// XmlAttrIndex.h: interface for the CXmlAttrIndex class.
//
//////////////////////////////////////////////////////////////////////

/***********************************************************************************
  编码日志记录
  编码者: 王明松
  编码日期 20109-4-26 9:48:52
  版本: V1.00 
  说明: XML属性索引类，对多个属性建立索引，加速查找
 ***********************************************************************************/


#if !defined(AFX_XMLATTRINDEX_H__EE14B112_03A2_4194_87FD_C16D1B05D2D8__INCLUDED_)
#define AFX_XMLATTRINDEX_H__EE14B112_03A2_4194_87FD_C16D1B05D2D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "MdbBase.h"
#include "public.h"
#include "XmlHead.h"

class CXmlAttribute;

#ifdef KERNEL_EXPORTS
#define KERNEL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNEL_EXPORT __declspec(dllimport)
#else
#define KERNEL_EXPORT 
#endif
#endif

class KERNEL_EXPORT CXmlAttrIndex  
{
public:
	CXmlAttrIndex();
	virtual ~CXmlAttrIndex();
protected:
	
	struct S_TBL_ATTR_INDEX
	{   
		std::string    sAttrName; 
		CXmlAttribute     *pXmlAttr;
	};
	
	CMemTableNew <S_TBL_ATTR_INDEX> m_table;
//	CIndexCharF<NODENAMELEN,1>                  m_pkey;     //节点名称的索引
	CIndexVarChar<1>                  m_pkey;     //节点名称的索引
public:
		
	// 函数名: Clear
	// 编程  : 王明松 2010-6-7 15:13:35
	// 返回  : void 
	// 描述  : 清除索引
	void Clear();
	
	// 函数名: SelectAttribute
	// 编程  : 王明松 2010-6-7 15:11:03
	// 返回  : CAttribute* 
	// 参数  : const char *attrname   属性名称是转义后的
	// 描述  : 根据名称查找一个属性 属性名称是转义后的
	CXmlAttribute* SelectAttribute(const char *attrname);
	
	// 函数名: Delete
	// 编程  : 王明松 2010-6-7 15:10:43
	// 返回  : bool false无此索引
	// 参数  : CXmlAttribute *attr
	// 描述  : 根据属性名称删除属性索引
	bool Delete(CXmlAttribute *attr);
	
	// 函数名: Insert
	// 编程  : 王明松 2010-6-7 15:10:31
	// 返回  : bool false已有此索引
	// 参数  : CXmlAttribute *attr
	// 描述  : 写入一个属性至索引
	bool Insert(CXmlAttribute *attr);
};
#undef  KERNEL_EXPORT
#endif // !defined(AFX_XMLATTRINDEX_H__EE14B112_03A2_4194_87FD_C16D1B05D2D8__INCLUDED_)
