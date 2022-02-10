// XmlNodeIndex.h: interface for the CXmlNodeIndex class.
//
//////////////////////////////////////////////////////////////////////

/***********************************************************************************
  编码日志记录
  编码者: 王明松
  编码日期 2009-4-26 9:50:32
  版本: V1.00 
  说明: XML节点索引类，加快节点的查找速度
 ***********************************************************************************/


#if !defined(AFX_XMLNODEINDEX_H__71F0C6C9_EE1C_4B39_97A2_C3EE24BD4BDA__INCLUDED_)
#define AFX_XMLNODEINDEX_H__71F0C6C9_EE1C_4B39_97A2_C3EE24BD4BDA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MdbBase.h"
#include "public.h"
#include "XmlHead.h"

class CXmlNode;

#ifdef KERNEL_EXPORTS
#define KERNEL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNEL_EXPORT __declspec(dllimport)
#else
#define KERNEL_EXPORT 
#endif
#endif

class KERNEL_EXPORT CXmlNodeIndex  
{
public:
	CXmlNodeIndex();
	virtual ~CXmlNodeIndex();
protected:
	struct S_TBL_NODE_INDEX
	{   
		int            nNodeId;
		std::string    sNodeName; 
		CXmlNode      *pXmlNode;
	};
	CMemTableNew <S_TBL_NODE_INDEX> m_table;
//	CIndexCharF<NODENAMELEN,1>   m_index_nodename;     //节点名称的索引
	CIndexVarChar<1>   m_index_nodename;     //节点名称的索引
	CIndexInt<1>                 m_pkey;               //节点序号的key
public:
		
	// 函数名: *SelectNode
	// 编程  : 王明松 2010-6-4 17:24:23
	// 返回  : CXmlNode *
	// 参数  : int nodeid  节点序号，即m_nParentIndex
	// 描述  : 根据节点序号查找节点
	CXmlNode *SelectNode(int nodeid);
	
	// 函数名: SelectNode
	// 编程  : 王明松 2010-6-4 17:24:07
	// 返回  : bool 
	// 参数  : const char *nodename
	// 参数  : vector<CXmlNode *> &nodelist
	// 描述  : 根据节点名称查找节点
	bool SelectNode(const char *nodename,vector<CXmlNode *> &nodelist);
	
	// 函数名: Delete
	// 编程  : 王明松 2010-6-4 17:23:41
	// 返回  : bool 
	// 参数  : CXmlNode *node
	// 描述  : 删除节点索引
	bool Delete(CXmlNode *node);
	
	// 函数名: Clear
	// 编程  : 王明松 2010-6-4 17:23:29
	// 返回  : void 
	// 描述  : 清除节点索引
	void Clear();
	
	// 函数名: Insert
	// 编程  : 王明松 2010-6-4 17:23:07
	// 返回  : void 
	// 参数  : CXmlNode *node
	// 描述  : 插入一个节点索引
	void Insert(CXmlNode *node);
};
#undef  KERNEL_EXPORT
#endif // !defined(AFX_XMLNODEINDEX_H__71F0C6C9_EE1C_4B39_97A2_C3EE24BD4BDA__INCLUDED_)
