// XmlNodeIndex.h: interface for the CXmlNodeIndex class.
//
//////////////////////////////////////////////////////////////////////

/***********************************************************************************
  ������־��¼
  ������: ������
  �������� 2009-4-26 9:50:32
  �汾: V1.00 
  ˵��: XML�ڵ������࣬�ӿ�ڵ�Ĳ����ٶ�
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
//	CIndexCharF<NODENAMELEN,1>   m_index_nodename;     //�ڵ����Ƶ�����
	CIndexVarChar<1>   m_index_nodename;     //�ڵ����Ƶ�����
	CIndexInt<1>                 m_pkey;               //�ڵ���ŵ�key
public:
		
	// ������: *SelectNode
	// ���  : ������ 2010-6-4 17:24:23
	// ����  : CXmlNode *
	// ����  : int nodeid  �ڵ���ţ���m_nParentIndex
	// ����  : ���ݽڵ���Ų��ҽڵ�
	CXmlNode *SelectNode(int nodeid);
	
	// ������: SelectNode
	// ���  : ������ 2010-6-4 17:24:07
	// ����  : bool 
	// ����  : const char *nodename
	// ����  : vector<CXmlNode *> &nodelist
	// ����  : ���ݽڵ����Ʋ��ҽڵ�
	bool SelectNode(const char *nodename,vector<CXmlNode *> &nodelist);
	
	// ������: Delete
	// ���  : ������ 2010-6-4 17:23:41
	// ����  : bool 
	// ����  : CXmlNode *node
	// ����  : ɾ���ڵ�����
	bool Delete(CXmlNode *node);
	
	// ������: Clear
	// ���  : ������ 2010-6-4 17:23:29
	// ����  : void 
	// ����  : ����ڵ�����
	void Clear();
	
	// ������: Insert
	// ���  : ������ 2010-6-4 17:23:07
	// ����  : void 
	// ����  : CXmlNode *node
	// ����  : ����һ���ڵ�����
	void Insert(CXmlNode *node);
};
#undef  KERNEL_EXPORT
#endif // !defined(AFX_XMLNODEINDEX_H__71F0C6C9_EE1C_4B39_97A2_C3EE24BD4BDA__INCLUDED_)
