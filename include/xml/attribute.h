#ifndef CATTRIBUTE_H_HEADER_INCLUDED_C1238FC1
#define CATTRIBUTE_H_HEADER_INCLUDED_C1238FC1

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

#define NODEVALUELEN    40960      ///最大的节点值
#define NODENAMELEN    1024      ///最大的节点名称
#define NODENAMEATTRLEN    40960      ///节点名称及属性的最大值

#define ITEMNAMELENGTH 40960      ///最大的节点名称
#define ITEMVALUELENGTH 40960   ///xml最大的节点值
//#define XMLDATALENGTH   1024000   ///输出到文件的最大缓冲
#define TAB "   "
#define SPACE 32
#define TABLESPACE 9
#define ENTER 10


class XML_EXPORT CAttribute
{
public:

	/// 函数名: deleteNextAttribute
	/// 编程  : 王明松 2006-6-23 19:30:25
	/// 返回  : bool 
	/// 描述  : 删除下属属性
	bool deleteNextAttribute();

	/// 函数名: getName
	/// 编程  : 王明松 2006-6-23 19:30:22
	/// 返回  : string 
	/// 描述  : 取得属性名称
	std::string getName();

	/// 函数名: getValue
	/// 编程  : 王明松 2006-6-23 19:30:19
	/// 返回  : string 
	/// 描述  : 取得属性值
	std::string getValue();

	/// 函数名: getName
	/// 编程  : 王明松 2006-6-23 19:29:50
	/// 返回  : int 
	/// 参数  : string name
	/// 描述  : 取得属性名
	int getName(std::string &name);

	/// 函数名: getValue
	/// 编程  : 王明松 2006-6-23 19:29:38
	/// 返回  : int 
	/// 参数  : string value
	/// 描述  : 取得属性值
	int getValue(std::string &value);

	/// 函数名: *getNamedItem
	/// 编程  : 王明松 2006-6-23 19:29:19
	/// 返回  : CAttribute 
	/// 参数  : const string pAttrName
	/// 描述  : 根根属性名称获取属性
	CAttribute *getNamedItem(const std::string pAttrName);

	/// 函数名: *getNextSibling
	/// 编程  : 王明松 2006-6-23 19:29:08
	/// 返回  : CAttribute 
	/// 描述  : 获取下一属性
	CAttribute *getNextSibling();
    CAttribute();
	virtual ~CAttribute();

    CAttribute *m_next;///下一属性指针
	std::string  m_name; ///属性名称
    std::string  m_value; ///属性值
 	
};
#undef XML_EXPORT


#endif /* CATTRIBUTE_H_HEADER_INCLUDED_C1238FC1 */
