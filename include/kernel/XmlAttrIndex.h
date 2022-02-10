// XmlAttrIndex.h: interface for the CXmlAttrIndex class.
//
//////////////////////////////////////////////////////////////////////

/***********************************************************************************
  ������־��¼
  ������: ������
  �������� 20109-4-26 9:48:52
  �汾: V1.00 
  ˵��: XML���������࣬�Զ�����Խ������������ٲ���
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
//	CIndexCharF<NODENAMELEN,1>                  m_pkey;     //�ڵ����Ƶ�����
	CIndexVarChar<1>                  m_pkey;     //�ڵ����Ƶ�����
public:
		
	// ������: Clear
	// ���  : ������ 2010-6-7 15:13:35
	// ����  : void 
	// ����  : �������
	void Clear();
	
	// ������: SelectAttribute
	// ���  : ������ 2010-6-7 15:11:03
	// ����  : CAttribute* 
	// ����  : const char *attrname   ����������ת����
	// ����  : �������Ʋ���һ������ ����������ת����
	CXmlAttribute* SelectAttribute(const char *attrname);
	
	// ������: Delete
	// ���  : ������ 2010-6-7 15:10:43
	// ����  : bool false�޴�����
	// ����  : CXmlAttribute *attr
	// ����  : ������������ɾ����������
	bool Delete(CXmlAttribute *attr);
	
	// ������: Insert
	// ���  : ������ 2010-6-7 15:10:31
	// ����  : bool false���д�����
	// ����  : CXmlAttribute *attr
	// ����  : д��һ������������
	bool Insert(CXmlAttribute *attr);
};
#undef  KERNEL_EXPORT
#endif // !defined(AFX_XMLATTRINDEX_H__EE14B112_03A2_4194_87FD_C16D1B05D2D8__INCLUDED_)
