// XmlAttributes.h: interface for the CXmlAttributes class.
//
//////////////////////////////////////////////////////////////////////

/***********************************************************************************
  ������־��¼
  ������: ������
  �������� 2009-4-26 9:46:21
  �汾: V1.00 
  ˵��: XML���Բ����࣬����������XML���ԣ��ṩXML���Բ������� 
 ***********************************************************************************/


#if !defined(AFX_XMLATTRIBUTES_H__CAA9058B_7E53_40F1_AAC4_4557FC758B83__INCLUDED_)
#define AFX_XMLATTRIBUTES_H__CAA9058B_7E53_40F1_AAC4_4557FC758B83__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "XmlAttribute.h"
#include "XmlAttrIndex.h"

#ifdef KERNEL_EXPORTS
#define KERNEL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNEL_EXPORT __declspec(dllimport)
#else
#define KERNEL_EXPORT 
#endif
#endif

class KERNEL_EXPORT CXmlAttributes  
{
public:
	bool GetUseIndexFlag();
	bool CreateIndex();

	// ������: GetEndAttribute
	// ���  : ������ 2010-6-12 15:16:34
	// ����  : CXmlAttribute * 
	// ����  : ȡ�����һ������
	CXmlAttribute * GetEndAttribute();

	// ������: GetFirstAttribute
	// ���  : ������ 2010-6-12 15:16:38
	// ����  : CXmlAttribute * 
	// ����  : ȡ�õ�һ������
	CXmlAttribute * GetFirstAttribute();

	// ������: DeleteAttribute
	// ���  : ������ 2010-6-8 11:10:41
	// ����  : bool 
	// ����  : string attrname
	// ����  : bool isTransSign=true
	// ����  : ɾ�����Լ���������
	bool DeleteAttribute(std::string attrname,bool isTransSign=true);

	// ������: Clear
	// ���  : ������ 2010-6-8 11:09:56
	// ����  : void 
	// ����  : ����������ԣ���������
	void Clear();

	// ������: GetAttribute
	// ���  : ������ 2010-6-8 11:04:49
	// ����  : CXmlAttribute * ���ҵ��򷵻����ԣ����򷵻�NULL
	// ����  : string attrname  ��������
	// ����  : bool isTransSign �����Ƿ����ת���ת��
	// ����  : string &attrvalue ����ֵ
	// ����  : bool isTransValue=true ����ֵ�Ƿ����ת���ת��
	// ����  : �����������Ʋ�������ֵ 
	CXmlAttribute * GetAttribute(std::string attrname,bool isTransSign,std::string &attrvalue,bool isTransValue=true);

	// ������: GetAttribute
	// ���  : ������ 2010-6-8 11:04:46
	// ����  : CXmlAttribute * ���ҵ��򷵻����ԣ����򷵻�NULL
	// ����  : std::string attrname ��������
	// ����  : bool isTransSign=true �����Ƿ����ת���ת��
	// ����  : �����������Ʋ�������
	CXmlAttribute * GetAttribute(std::string attrname,bool isTransSign=true);

	// ������: SetAttribute
	// ���  : ������ 2010-6-8 10:36:50
	// ����  : CXmlAttribute * 
	// ����  : string attrname  ��������
	// ����  : bool isTransSign  �����Ƿ����ת���ת��
	// ����  : string attrvlaue   ����ֵ
	// ����  : bool isTransValue=true  ����ֵ�Ƿ����ת���ת��
	// ����  : ����һ������ �����д����Ƶ�����������ֵ���أ�����������������������
	CXmlAttribute * SetAttribute(std::string attrname,bool isTransSign,std::string attrvlaue,bool isTransValue=true);

	CXmlAttribute * SetAttribute(std::string attrname,bool isTransSign,int attrvlaue);

	CXmlAttribute * SetAttribute(std::string attrname,bool isTransSign,long attrvlaue);

	CXmlAttribute * SetAttribute(std::string attrname,bool isTransSign,double attrvlaue,int dec=2);

	CXmlAttribute * AddAttribute(std::string attrname,bool isTransSign,std::string attrvlaue,bool isTransValue=true);
	
	CXmlAttributes();
	virtual ~CXmlAttributes();

protected:
	// ������: DeleteAllAttribute
	// ���  : ������ 2010-6-10 11:03:28
	// ����  : void 
	// ����  : ɾ����������
	void DeleteAllAttribute();

	// ������: DeleteAttributePoint
	// ���  : ������ 2010-6-8 11:10:06
	// ����  : void 
	// ����  : CXmlAttribute *attr
	// ����  : ɾ�����Ե�ָ�����ӣ�Ȼ��Ϳ���delete��
	void DeleteAttributePoint(CXmlAttribute *attr);

	CXmlAttribute *m_pFirstAttribute;//��һ������
	CXmlAttribute *m_pEndAttribute;//���һ������
	CXmlAttrIndex *m_index;//���Ե�����

	bool  m_bIsUseIndex;
	
};
#undef  KERNEL_EXPORT
#endif // !defined(AFX_XMLATTRIBUTES_H__CAA9058B_7E53_40F1_AAC4_4557FC758B83__INCLUDED_)
