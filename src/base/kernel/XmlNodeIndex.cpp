// XmlNodeIndex.cpp: implementation of the CXmlNodeIndex class.
//
//////////////////////////////////////////////////////////////////////

#include "XmlNodeIndex.h"
#include "XmlNode.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXmlNodeIndex::CXmlNodeIndex()
{

}

CXmlNodeIndex::~CXmlNodeIndex()
{

}
void CXmlNodeIndex::Insert(CXmlNode *node)
{
	int id;
	S_TBL_NODE_INDEX tmptbl;
	tmptbl.pXmlNode = node;
	id = m_table.Add(tmptbl);
	m_index_nodename.Add(id,(char *)m_table.m_table[id].sNodeName.c_str());
	m_pkey.Add(id,node->GetParentIndex());
	return ;
}

void CXmlNodeIndex::Clear()
{
	m_pkey.Clear();
	m_table.Clear();
	m_index_nodename.Clear();
}

bool CXmlNodeIndex::Delete(CXmlNode *node)
{
	if (node == NULL)
	{
		return false;
	}
	CInt iset;
	if (!m_pkey.Select(iset,node->GetParentIndex()))
	{
		return false;
	}
	int id;
	iset.First(id);
	m_pkey.Delete(node->GetParentIndex());
	m_index_nodename.Delete(iset,(char *)node->GetNodeName(false).c_str());
	m_table.Delete(id);
	return true;
}

bool CXmlNodeIndex::SelectNode(const char *nodename, std::vector<CXmlNode *> &nodelist)
{
	CInt iset;
	if (!m_index_nodename.Select(iset,(char *)nodename))
	{
		return false;
	}
	int id;
	bool bRet;
	bRet = iset.First(id);
	while (bRet)
	{
		nodelist.push_back(m_table.m_table[id].pXmlNode);
		bRet = iset.Next(id);
	}
	return true;
}

CXmlNode *CXmlNodeIndex::SelectNode(int nodeid)
{
	CInt iset;
	if (!m_pkey.Select(iset,nodeid))
	{
		return NULL;
	}
	int id;
	iset.First(id);
	return m_table.m_table[id].pXmlNode;
}
