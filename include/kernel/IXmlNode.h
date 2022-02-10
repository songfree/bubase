// IXmlNode.h: interface for the CIXmlNode class.
//
//////////////////////////////////////////////////////////////////////

/***********************************************************************************
  ������־��¼
  ������: ������
  �������� 2009-4-26 9:42:21
  �汾: V1.00 
  ˵��: XML�ڵ�ĳ���ӿ���
 ***********************************************************************************/


#if !defined(AFX_IXMLNODE_H__CE5BA1F4_01BF_4F84_B3E0_BAED797371C2__INCLUDED_)
#define AFX_IXMLNODE_H__CE5BA1F4_01BF_4F84_B3E0_BAED797371C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"

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

class KERNEL_EXPORT CIXmlNode  
{
public:
	CIXmlNode()
	{

	}
	virtual ~CIXmlNode()
	{

	};

	// ������: * NewNode
	// ���  : ������ 2010-6-15 13:19:07
	// ����  : virtual CIXmlNode 
	// ����  : ����һ���µĽڵ�
	virtual CIXmlNode * NewNode()=0;

	// ������: AddAttribute
	// ���  : ������ 2010-6-8 10:36:50
	// ����  : CXmlAttribute * 
	// ����  : string attrname  ��������
	// ����  : bool isTransSign  �����Ƿ����ת���ת��
	// ����  : string attrvlaue   ����ֵ
	// ����  : bool isTransValue=true  ����ֵ�Ƿ����ת���ת��
	// ����  : ����һ������ �����д����Ƶ�����������ֵ���أ�����������������������
	virtual CXmlAttribute * AddAttribute(std::string attrname,bool isTransSign,std::string attrvlaue,bool isTransValue=true)=0;

	// ������: GetParentNode
	// ���  : ������ 2010-6-8 16:22:14
	// ����  : CIXmlNode * 
	// ����  : ȡ�ø��ڵ�
	virtual CIXmlNode * GetParentNode()=0;
	
	// ������: getNextSibling
	// ���  : ������ 2010-6-8 16:22:26
	// ����  : CIXmlNode * 
	// ����  : ȡ���ٽ��½ڵ�
	virtual CIXmlNode * getNextSibling()=0;
	
	// ������: getLastSibling
	// ���  : ������ 2010-6-8 16:22:37
	// ����  : CIXmlNode * 
	// ����  : ȡ���ٽ��Ͻڵ�
	virtual CIXmlNode * getLastSibling()=0;
	
	// ������: GetEndChild
	// ���  : ������ 2010-6-8 16:22:49
	// ����  : CIXmlNode * 
	// ����  : ȡ�����һ���ӽڵ�
	virtual CIXmlNode * GetEndChild()=0;
	
	// ������: GetFirstChild
	// ���  : ������ 2010-6-8 16:23:00
	// ����  : CIXmlNode * 
	// ����  : ȡ����ǰһ���ӽڵ�
	virtual CIXmlNode * GetFirstChild()=0;
	
	// ������: GetNodeType
	// ���  : ������ 2010-6-4 16:39:31
	// ����  : int 
	// ����  : ȡ�ýڵ�����
	virtual int GetNodeType()=0;
	
	// ������: SetNodeType
	// ���  : ������ 2010-6-4 16:39:17
	// ����  : void 
	// ����  : int type
	// ����  : ���ýڵ�����
	virtual void SetNodeType(int type)=0;
	
	// ������: GetNodeValue
	// ���  : ������ 2010-6-4 15:57:32
	// ����  : std::string 
	// ����  : bool isTransSign=true true����ת���ת�� false��ת
	// ����  : ȡ�ýڵ�ֵ
	virtual std::string GetNodeValue(bool isTransSign=true)=0;
	
	// ������: GetNodeName
	// ���  : ������ 2010-6-4 15:57:41
	// ����  : std::string 
	// ����  : bool isTransSign=true  true����ת���ת�� false��ת
	// ����  : ȡ�ýڵ�����
	virtual std::string GetNodeName(bool isTransSign=true)=0;
	
	
	// ������: SetNodeValue
	// ���  : ������ 2010-6-9 12:01:05
	// ����  : void 
	// ����  : std::string value
	// ����  : bool isTransSign=true
	// ����  : ���ýڵ�ֵ
	virtual void SetNodeValue(std::string value,bool isTransSign=true)=0;

	
	// ������: SetNodeName
	// ���  : ������ 2010-6-9 12:01:18
	// ����  : void 
	// ����  : std::string nodename
	// ����  : bool isTransSign=true
	// ����  : ���ýڵ����ƣ�ע������ڵ��Ѿ�����XML�����򲻿������������ƣ���Ϊ����Ҫ��������
	virtual void SetNodeName(std::string nodename,bool isTransSign=true)=0;
	
	// ������: ParseChildNode
	// ���  : ������ 2010-6-2 16:37:04
	// ����  : bool 
	// ����  : int lay=0   �ӽڵ�Ĳ�Σ�0ȫ���⿪��>0�⿪���  1ֻ�⿪���ڵ���ӽڵ� 
	// ����  : �����ӽڵ�
	virtual bool ParseChildNode(int layer=0)=0;
	
	// ������: AddChildNode
	// ���  : ������ 2010-6-4 15:14:22
	// ����  : int   ���ش˽ڵ���ӽڵ���
	// ����  : CXmlNode *anode    Ҫ����Ľڵ� ע��ڵ�����Ҫ�У���������������������
	// ����  : bool first=false   true���ӵ�ǰ�棬false���ӵ����
	// ����  : ����һ���ӽڵ�,ע��ڵ�anode�Ľڵ����ͼ��ڵ�����Ҫ��֮ǰ���ú�
	virtual int AddChildNode(CIXmlNode *anode,bool first=false)=0;


	// ������: SetParseClass
	// ���  : ������ 2010-6-17 13:35:52
	// ����  : virtual void 
	// ����  : void *parse
	// ����  : ���ý�����ָ��
	virtual void SetParseClass(void *parse)=0;


	// ������: SetParseChilded
	// ���  : ������ 2010-6-22 11:56:42
	// ����  : virtual void 
	// ����  : ���ô˽ڵ��ѽ����½ڵ�
	virtual void SetParseChilded()=0;

};

#endif // !defined(AFX_IXMLNODE_H__CE5BA1F4_01BF_4F84_B3E0_BAED797371C2__INCLUDED_)
