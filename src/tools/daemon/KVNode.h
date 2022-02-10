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

#define  KVTYPE_POP            0    //�������һ��
#define  KVTYPE_NAMEVALUE          1    //����:ֵ����ʽ  =�ָ�������

#define  KVTYPE_ARRAYLIST      20    //�����б�   
#define  KVTYPE_HASHTABLE      21    //��ϣ��
#define  KVTYPE_DATALIST       22    //"��"�ָ��Ĳ��������б�͹�ϣ��

#define  KVTYPE_DATA           3    // "��","��" �ָ�����Ͳ�ֵ

#define  ONESEPSTRING   "��"

class CKVNode  
{
public:

	// ������: ParseNode
	// ���  : ������ 2013-12-11 10:16:31
	// ����  : bool 
	// ����  : �����ڵ�
	bool ParseNode();

public:

	// ������: ToStringOne
	// ���  : ������ 2013-12-11 14:45:11
	// ����  : bool 
	// ����  : std::string &data
	// ����  : ת��|�ָ���ֵ
	bool ToStringOne(std::string &data,int &col,int &line);

	// ������: ToString
	// ���  : ������ 2013-12-11 11:44:34
	// ����  : bool 
	// ����  : std::string &data
	// ����  : ת��kv�ĸ�ʽ�ַ���
	bool ToString(std::string &data);

	// ������: GetNodeType
	// ���  : ������ 2013-12-11 10:16:42
	// ����  : int 
	// ����  : std::string value
	// ����  : ������󼸸��ַ��жϽڵ������
	int GetNodeType(std::string svalue);

	// ������: AddChildNode
	// ���  : ������ 2013-12-11 10:16:46
	// ����  : void 
	// ����  : CKVNode *addnode
	// ����  : �����ӽڵ�
	void AddChildNode(CKVNode *addnode);

	void DeleteAllChild();

	CKVNode();
	virtual ~CKVNode();


	std::string    m_sKvName;       //����
	std::string    m_sKvValue;      //ֵ
	unsigned short m_nKvType;       //�����˽ڵ������
	unsigned short m_nLevel;        //�˽ڵ�Ĳ��

	int            m_nDataCount;      //���ݸ���

	CKVNode *m_parentNode;///���ڵ�ָ��
    CKVNode *m_nextNode; ///�ٽ��½ڵ�ָ��
	CKVNode *m_lastNode; ///�ٽ��Ͻڵ�ָ��
    CKVNode *m_firstChildNode;  ///��һ���ӽڵ�ָ��
	CKVNode *m_endChildNode;   ///���һ���ӽڵ�ָ��
protected:
	CBF_SimpleLog  m_pLog;
	
	bool ToStringOne(std::string &data);
	
	void DeletePoint();
};

#endif // !defined(AFX_KVNODE_H__356784D1_5EFA_488D_A15E_AF1F8B40856D__INCLUDED_)


