// KVNode.h: interface for the CKVNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KVNODE_H__356784D1_5EFA_488D_A15E_AF1F8B40856D__INCLUDED_)
#define AFX_KVNODE_H__356784D1_5EFA_488D_A15E_AF1F8B40856D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "public.h"
#include "BF_Slist.h"
#include "BF_SimpleLog.h"

#define  KVTYPE_POP            0    //最上面的一层
#define  KVTYPE_NAMEVALUE          1    //名称:值的形式  =分隔的数据

#define  KVTYPE_ARRAYLIST      20    //数据列表   
#define  KVTYPE_HASHTABLE      21    //哈希表
#define  KVTYPE_DATALIST       22    //"∧"分隔的不是数据列表和哈希表

#define  KVTYPE_DATA           3    // "｜","ˇ" 分隔的最低层值

#define  ONESEPSTRING   "｜"


#ifdef KVDATA_EXPORTS
#define KVDATA_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KVDATA_EXPORT __declspec(dllimport)
#else
#define KVDATA_EXPORT 
#endif
#endif

class KVDATA_EXPORT CKVNode  
{
public:

	// 函数名: ParseNode
	// 编程  : 王明松 2013-12-11 10:16:31
	// 返回  : bool 
	// 描述  : 解析节点
	bool ParseNode();

public:

	// 函数名: ToStringOne
	// 编程  : 王明松 2013-12-11 14:45:11
	// 返回  : bool 
	// 参数  : std::string &data
	// 描述  : 转成|分隔的值
	bool ToStringOne(std::string &data,int &col,int &line);

	// 函数名: ToString
	// 编程  : 王明松 2013-12-11 11:44:34
	// 返回  : bool 
	// 参数  : std::string &data
	// 描述  : 转成kv的格式字符串
	bool ToString(std::string &data);

	// 函数名: GetNodeType
	// 编程  : 王明松 2013-12-11 10:16:42
	// 返回  : int 
	// 参数  : std::string value
	// 描述  : 根据最后几个字符判断节点的类型
	int GetNodeType(std::string svalue);

	// 函数名: AddChildNode
	// 编程  : 王明松 2013-12-11 10:16:46
	// 返回  : void 
	// 参数  : CKVNode *addnode
	// 描述  : 增加子节点
	void AddChildNode(CKVNode *addnode);

	void DeleteAllChild();

	CKVNode();
	virtual ~CKVNode();


	std::string    m_sKvName;       //名称
	std::string    m_sKvValue;      //值
	unsigned short m_nKvType;       //表明此节点的类型
	unsigned short m_nLevel;        //此节点的层次

	int            m_nDataCount;      //数据个数

	CKVNode *m_parentNode;///父节点指针
    CKVNode *m_nextNode; ///临近下节点指针
	CKVNode *m_lastNode; ///临近上节点指针
    CKVNode *m_firstChildNode;  ///第一个子节点指针
	CKVNode *m_endChildNode;   ///最后一个子节点指针
protected:
	CBF_SimpleLog  m_pLog;
	
	bool ToStringOne(std::string &data);
	
	void DeletePoint();
};

#endif // !defined(AFX_KVNODE_H__356784D1_5EFA_488D_A15E_AF1F8B40856D__INCLUDED_)


