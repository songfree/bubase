// XmlAttribute.h: interface for the CXmlAttribute class.
//
//////////////////////////////////////////////////////////////////////

/***********************************************************************************
  ������־��¼
  ������: ������
  �������� 2009-4-26 9:47:34
  �汾: V1.00 
  ˵��: XML������ ��Ӧһ��XML����
 ***********************************************************************************/


#if !defined(AFX_XMLATTRIBUTE_H__401DD803_E06B_4702_9E04_74A53D92094E__INCLUDED_)
#define AFX_XMLATTRIBUTE_H__401DD803_E06B_4702_9E04_74A53D92094E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "XmlTransSign.h"
#ifdef KERNEL_EXPORTS
#define KERNEL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNEL_EXPORT __declspec(dllimport)
#else
#define KERNEL_EXPORT 
#endif
#endif


class KERNEL_EXPORT CXmlAttribute  
{
public:
	CXmlAttribute();
	virtual ~CXmlAttribute();

public:

	// ������: DeletePoint
	// ���  : ������ 2010-6-10 10:18:27
	// ����  : void 
	// ����  : ������������ɾ������������
	void DeletePoint();

	// ������: SetLastAttribute
	// ���  : ������ 2010-6-8 11:35:02
	// ����  : void 
	// ����  : CXmlAttribute *lastattr
	// ����  : ���������������һ������
	void SetLastAttribute(CXmlAttribute *lastattr);
	
	// ������: SetNextAttribute
	// ���  : ������ 2010-6-8 10:53:40
	// ����  : void
	// ����  : CXmlAttribute *nextattr
	// ����  : ���������������һ������
	void SetNextAttribute(CXmlAttribute *nextattr);
	
	// ������: SetAttrValue
	// ���  : ������ 2010-6-7 14:54:49
	// ����  : void 
	// ����  : std::string value
	// ����  : bool isTransSign=true ת���ת��
	// ����  : ��������ֵ
	void SetAttrValue(std::string value,bool isTransSign=true);
	
	// ������: SetAttrName
	// ���  : ������ 2010-6-7 14:54:40
	// ����  : void 
	// ����  : std::string attrname
	// ����  : bool isTransSign=true ת���ת��
	// ����  : ������������
	void SetAttrName(std::string attrname,bool isTransSign=true);
	
	// ������: GetAttrValue
	// ���  : ������ 2010-6-7 14:50:31
	// ����  : std::string 
	// ����  : bool isTransSign=true ת���ת��
	// ����  : ȡ������ֵ
	std::string GetAttrValue(bool isTransSign=true);
	
	// ������: GetAttrName
	// ���  : ������ 2010-6-7 14:48:04
	// ����  : std::string 
	// ����  : bool isTransSign=true ת���ת��
	// ����  : ȡ����������
	std::string GetAttrName(bool isTransSign=true);
	
	// ������: getNextSibling
	// ���  : ������ 2010-6-7 14:47:07
	// ����  : CAttribute * 
	// ����  : ��ȡ��һ������
	CXmlAttribute * getNextSibling();

	// ������: getLastSibling
	// ���  : ������ 2010-6-8 11:15:16
	// ����  : CXmlAttribute * 
	// ����  : ��ȡ��һ������
	CXmlAttribute * getLastSibling();


protected:
	std::string m_sAttrValue;
	std::string m_sAttrName;
	
	CXmlAttribute *m_next;///��һ����ָ��
	CXmlAttribute *m_last;///��һ����ָ��
	

};
#undef  KERNEL_EXPORT
#endif // !defined(AFX_XMLATTRIBUTE_H__401DD803_E06B_4702_9E04_74A53D92094E__INCLUDED_)
