#ifndef D_XML_CNODE_H_HEADER_INCLUDED_C1238B96
#define D_XML_CNODE_H_HEADER_INCLUDED_C1238B96

#include "attribute.h"
#include "public.h"

#ifdef XML_EXPORTS
#define XML_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define XML_EXPORT __declspec(dllimport)
#else
#define XML_EXPORT 
#endif
#endif

class XML_EXPORT CNode
{
public:


	/// 函数名: setNodeValue
	/// 编程  : 王明松 2008-7-11 10:00:02
	/// 返回  : void 
	/// 输入参数  : double value
	/// 描述  : 设置节点值
	void setNodeValue(double value);

	/// 函数名: setNodeValue
	/// 编程  : 王明松 2008-7-11 9:59:58
	/// 返回  : void 
	/// 输入参数  : long value
	/// 描述  : 设置节点值
	void setNodeValue(long value);

	/// 函数名: setNodeValue
	/// 编程  : 王明松 2008-7-11 9:59:46
	/// 返回  : void 
	/// 输入参数  : int value
	/// 描述  : 设置节点值
	void setNodeValue(int value);

	/// 函数名: setNodeName
	/// 编程  : 王明松 2008-7-11 9:59:41
	/// 返回  : void 
	/// 输入参数  : double name
	/// 描述  : 设置节点名称
	void setNodeName(double name);

	/// 函数名: setNodeName
	/// 编程  : 王明松 2008-7-11 9:59:34
	/// 返回  : void 
	/// 输入参数  : long name
	/// 描述  : 设置节点名称
	void setNodeName(long name);

	/// 函数名: setNodeName
	/// 编程  : 王明松 2008-7-11 9:59:23
	/// 返回  : void 
	/// 输入参数  : int name
	/// 描述  : 设置节点名称
	void setNodeName(int name);

	/// 函数名: setNodeValue
	/// 编程  : 王明松 2008-7-11 9:54:41
	/// 返回  : void 
	/// 输入参数  : string value
	/// 输入参数  : bool isTransSign  是否转义符转换
	/// 描述  : 设置节点值
	void setNodeValue(std::string value,bool isTransSign=true);

	/// 函数名: setNodeName
	/// 编程  : 王明松 2008-7-11 9:54:38
	/// 返回  : void 
	/// 输入参数  : string name
	/// 输入参数  : bool isTransSign 是否转义符转换
	/// 描述  : 设置节点名称
	void setNodeName(std::string name,bool isTransSign=true);

	/// 函数名: getTransSign
	/// 编程  : 王明松 2008-7-11 9:54:03
	/// 返回  : int 
	/// 输入输出参数  : std::string &value
	/// 描述  : 去除转义符
	int getTransSign(std::string &value);


	/// 函数名: putTransSign
	/// 编程  : 王明松 2008-7-11 9:54:07
	/// 返回  : int 
	/// 输入输出参数  : std::string &value
	/// 描述  : 加入转议符
	int putTransSign(std::string &value);

	/// 函数名: IsDouble
	/// 编程  : 王明松 2008-6-16 12:58:04
	/// 返回  : bool 
	/// 输入参数  : const char *str
	/// 描述  : 是否浮点值
	bool IsDouble(const char *str);

	/// 函数名: IsNumber
	/// 编程  : 王明松 2008-6-16 12:58:01
	/// 返回  : bool 
	/// 输入参数  : const char *str
	/// 描述  : 是否数字
	bool IsNumber(const char *str);

	/// 函数名: GetAttributeValue
	/// 编程  : 王明松 2008-6-16 12:52:43
	/// 返回  : int 
	/// 输入参数  : std::string name
	/// 输出参数  : double &value
	/// 描述  : 取得属性值
	int GetAttributeValue(std::string name,double &value);


	/// 函数名: GetAttributeValue
	/// 编程  : 王明松 2008-6-16 12:52:43
	/// 返回  : int 
	/// 输入参数  : std::string name
	/// 输出参数  : long &value
	/// 描述  : 取得属性值
	int GetAttributeValue(std::string name,long &value);

	/// 函数名: GetAttributeValue
	/// 编程  : 王明松 2008-6-16 12:52:43
	/// 返回  : int 
	/// 输入参数  : std::string name
	/// 输出参数  : int &value
	/// 描述  : 取得属性值
	int GetAttributeValue(std::string name,int &value);

	/// 函数名: GetAttributeValue
	/// 编程  : 王明松 2008-6-16 12:52:43
	/// 返回  : int 
	/// 输入参数  : std::string name
	/// 输出参数  : std::string &value
	/// 描述  : 取得属性值，不做转义符转换
	int GetAttributeValue(std::string name,std::string &value);

	/// 函数名: *SetAttribute
	/// 编程  : 王明松 2008-5-23 14:27:36
	/// 返回  : CAttribute 
	/// 输入参数  : string name
	/// 输入参数  : double value
	/// 描述  : 设置节点的属性
	CAttribute *SetAttribute(std::string name, double value);

	/// 函数名: *SetAttribute
	/// 编程  : 王明松 2008-5-23 14:27:36
	/// 返回  : CAttribute 
	/// 输入参数  : string name
	/// 输入参数  : long value
	/// 描述  : 设置节点的属性
	CAttribute *SetAttribute(std::string name, long value);

	/// 函数名: *SetAttribute
	/// 编程  : 王明松 2008-5-23 14:27:36
	/// 返回  : CAttribute 
	/// 输入参数  : string name
	/// 输入参数  : int value
	/// 描述  : 设置节点的属性
	CAttribute *SetAttribute(std::string name, int value);

	/// 函数名: *SetAttribute
	/// 编程  : 王明松 2008-5-23 14:27:36
	/// 返回  : CAttribute 
	/// 输入参数  : string name
	/// 输入参数  : string value
	/// 输入参数  : bool isTransSign 是否转义符转换
	/// 描述  : 设置节点的属性，如果有此属性修改否则新增
	CAttribute *SetAttribute(std::string name,std::string value,bool isTransSign=false);


	/// 函数名: *SetAttribute
	/// 编程  : 王明松 2008-5-23 14:27:36
	/// 返回  : CAttribute 
	/// 输入参数  : string name
	/// 输入参数  : double value
	/// 描述  : 设置节点的属性
	CAttribute *SetNewAttribute(std::string name, double value);
	
	/// 函数名: *SetAttribute
	/// 编程  : 王明松 2008-5-23 14:27:36
	/// 返回  : CAttribute 
	/// 输入参数  : string name
	/// 输入参数  : long value
	/// 描述  : 设置节点的属性
	CAttribute *SetNewAttribute(std::string name, long value);
	
	/// 函数名: *SetAttribute
	/// 编程  : 王明松 2008-5-23 14:27:36
	/// 返回  : CAttribute 
	/// 输入参数  : string name
	/// 输入参数  : int value
	/// 描述  : 设置节点的属性
	CAttribute *SetNewAttribute(std::string name, int value);
	
	/// 函数名: *SetAttribute
	/// 编程  : 王明松 2008-5-23 14:27:36
	/// 返回  : CAttribute 
	/// 输入参数  : string name
	/// 输入参数  : string value
	/// 输入参数  : bool isTransSign 是否转义符转换
	/// 描述  : 设置一个新的属性，不做判断
	CAttribute *SetNewAttribute(std::string name,std::string value,bool isTransSign=false);

	/// 函数名: getEndChild
	/// 编程  : 王明松 2007-2-6 10:47:17
	/// 返回  : CNode * 
	/// 描述  : 获得最后一个子节点
	CNode * getEndChild();

	/// 函数名: getLastSibling
	/// 编程  : 王明松 2007-2-6 10:45:59
	/// 返回  : CNode * 
	/// 描述  : 获取上一临近节点
	CNode * getLastSibling();

	CAttribute *getAttributes(std::string attrname);
    CNode();
    virtual ~CNode();

	/// 函数名: getValue
	/// 编程  : 王明松 2006-6-23 19:37:42
	/// 返回  : string 
	/// 描述  : 获得节点值，做转义转换
	std::string getValue();

	/// 函数名: getName
	/// 编程  : 王明松 2006-6-23 19:37:53
	/// 返回  : string 
	/// 描述  : 获得节点名称 做转义转换
	std::string getName();

	/// 函数名: *getAttributes
	/// 编程  : 王明松 2006-6-23 19:38:03
	/// 返回  : CAttribute 
	/// 描述  : 获得节点属性链
	CAttribute *getAttributes();

	/// 函数名: *getNextSibling
	/// 编程  : 王明松 2006-6-23 19:38:21
	/// 返回  : CNode 
	/// 描述  : 获取下一临近节点
	CNode *getNextSibling();

	/// 函数名: *getFirstChild
	/// 编程  : 王明松 2006-6-23 19:38:33
	/// 返回  : CNode 
	/// 描述  : 获取第一个子节点
	CNode *getFirstChild();

	/// 函数名: *getParentNode
	/// 编程  : 王明松 2006-6-23 19:38:51
	/// 返回  : CNode 
	/// 描述  : 获取父节点
	CNode *getParentNode();

    std::string m_name; ///节点名称

    std::string m_value; ///节点值

    CAttribute *m_attribute;///节点属性

    CNode *m_parentNode;///父节点指针

    CNode *m_nextNode; ///临近下节点指针

    CNode *m_firstChildNode;///第一个子节点指针

	CNode *m_endChildNode;///最后一个子节点指针

	CNode *m_lastNode; ///临近上节点指针


	

};
#undef XML_EXPORT


#endif /* D_XML_CNODE_H_HEADER_INCLUDED_C1238B96 */
