// IXmlNode.h: interface for the CIXmlNode class.
//
//////////////////////////////////////////////////////////////////////

/***********************************************************************************
  编码日志记录
  编码者: 王明松
  编码日期 2009-4-26 9:42:21
  版本: V1.00 
  说明: XML节点的抽象接口类
 ***********************************************************************************/


#if !defined(AFX_IXMLNODE_H__CE5BA1F4_01BF_4F84_B3E0_BAED797371C2__INCLUDED_)
#define AFX_IXMLNODE_H__CE5BA1F4_01BF_4F84_B3E0_BAED797371C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"

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

class KERNEL_EXPORT CIXmlNode  
{
public:
	CIXmlNode()
	{

	}
	virtual ~CIXmlNode()
	{

	};

	// 函数名: * NewNode
	// 编程  : 王明松 2010-6-15 13:19:07
	// 返回  : virtual CIXmlNode 
	// 描述  : 生成一个新的节点
	virtual CIXmlNode * NewNode()=0;

	// 函数名: AddAttribute
	// 编程  : 王明松 2010-6-8 10:36:50
	// 返回  : CXmlAttribute * 
	// 参数  : string attrname  属性名称
	// 参数  : bool isTransSign  名称是否进行转义符转换
	// 参数  : string attrvlaue   属性值
	// 参数  : bool isTransValue=true  属性值是否进行转义符转换
	// 描述  : 增加一个属性 若已有此名称的属性则设置值返回，否则新增并加入索引返回
	virtual CXmlAttribute * AddAttribute(std::string attrname,bool isTransSign,std::string attrvlaue,bool isTransValue=true)=0;

	// 函数名: GetParentNode
	// 编程  : 王明松 2010-6-8 16:22:14
	// 返回  : CIXmlNode * 
	// 描述  : 取得父节点
	virtual CIXmlNode * GetParentNode()=0;
	
	// 函数名: getNextSibling
	// 编程  : 王明松 2010-6-8 16:22:26
	// 返回  : CIXmlNode * 
	// 描述  : 取得临近下节点
	virtual CIXmlNode * getNextSibling()=0;
	
	// 函数名: getLastSibling
	// 编程  : 王明松 2010-6-8 16:22:37
	// 返回  : CIXmlNode * 
	// 描述  : 取得临近上节点
	virtual CIXmlNode * getLastSibling()=0;
	
	// 函数名: GetEndChild
	// 编程  : 王明松 2010-6-8 16:22:49
	// 返回  : CIXmlNode * 
	// 描述  : 取得最后一个子节点
	virtual CIXmlNode * GetEndChild()=0;
	
	// 函数名: GetFirstChild
	// 编程  : 王明松 2010-6-8 16:23:00
	// 返回  : CIXmlNode * 
	// 描述  : 取得最前一个子节点
	virtual CIXmlNode * GetFirstChild()=0;
	
	// 函数名: GetNodeType
	// 编程  : 王明松 2010-6-4 16:39:31
	// 返回  : int 
	// 描述  : 取得节点类型
	virtual int GetNodeType()=0;
	
	// 函数名: SetNodeType
	// 编程  : 王明松 2010-6-4 16:39:17
	// 返回  : void 
	// 参数  : int type
	// 描述  : 设置节点类型
	virtual void SetNodeType(int type)=0;
	
	// 函数名: GetNodeValue
	// 编程  : 王明松 2010-6-4 15:57:32
	// 返回  : std::string 
	// 参数  : bool isTransSign=true true进行转义符转换 false不转
	// 描述  : 取得节点值
	virtual std::string GetNodeValue(bool isTransSign=true)=0;
	
	// 函数名: GetNodeName
	// 编程  : 王明松 2010-6-4 15:57:41
	// 返回  : std::string 
	// 参数  : bool isTransSign=true  true进行转义符转换 false不转
	// 描述  : 取得节点名称
	virtual std::string GetNodeName(bool isTransSign=true)=0;
	
	
	// 函数名: SetNodeValue
	// 编程  : 王明松 2010-6-9 12:01:05
	// 返回  : void 
	// 参数  : std::string value
	// 参数  : bool isTransSign=true
	// 描述  : 设置节点值
	virtual void SetNodeValue(std::string value,bool isTransSign=true)=0;

	
	// 函数名: SetNodeName
	// 编程  : 王明松 2010-6-9 12:01:18
	// 返回  : void 
	// 参数  : std::string nodename
	// 参数  : bool isTransSign=true
	// 描述  : 设置节点名称，注意如果节点已经加入XML链表，则不可以再重设名称，因为索引要重新设置
	virtual void SetNodeName(std::string nodename,bool isTransSign=true)=0;
	
	// 函数名: ParseChildNode
	// 编程  : 王明松 2010-6-2 16:37:04
	// 返回  : bool 
	// 参数  : int lay=0   子节点的层次，0全部解开，>0解开层次  1只解开本节点的子节点 
	// 描述  : 解析子节点
	virtual bool ParseChildNode(int layer=0)=0;
	
	// 函数名: AddChildNode
	// 编程  : 王明松 2010-6-4 15:14:22
	// 返回  : int   返回此节点的子节点数
	// 参数  : CXmlNode *anode    要加入的节点 注意节点名称要有，否则重新设置索引不符
	// 参数  : bool first=false   true增加到前面，false增加到最后
	// 描述  : 增加一个子节点,注意节点anode的节点类型及节点名称要在之前设置好
	virtual int AddChildNode(CIXmlNode *anode,bool first=false)=0;


	// 函数名: SetParseClass
	// 编程  : 王明松 2010-6-17 13:35:52
	// 返回  : virtual void 
	// 参数  : void *parse
	// 描述  : 设置解析类指针
	virtual void SetParseClass(void *parse)=0;


	// 函数名: SetParseChilded
	// 编程  : 王明松 2010-6-22 11:56:42
	// 返回  : virtual void 
	// 描述  : 设置此节点已解析下节点
	virtual void SetParseChilded()=0;

};

#endif // !defined(AFX_IXMLNODE_H__CE5BA1F4_01BF_4F84_B3E0_BAED797371C2__INCLUDED_)
