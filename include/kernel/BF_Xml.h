// BFXml.h: interface for the CBFXml class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BFXML_H__50F338A7_6795_462B_8771_84225CFCEBB1__INCLUDED_)
#define AFX_BFXML_H__50F338A7_6795_462B_8771_84225CFCEBB1__INCLUDED_

	
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XmlNode.h"



#ifdef KERNEL_EXPORTS
#define KERNEL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNEL_EXPORT __declspec(dllimport)
#else
#define KERNEL_EXPORT 
#endif
#endif

class KERNEL_EXPORT CBF_Xml  
{
public:

	// ������: CreateIndex
	// ���  : ������ 2010-7-6 10:22:23
	// ����  : void 
	// ����  : ����������xml�����Ҵ�������ʱ�ɽ�
	void CreateIndex();

	// ������: AddNodeByPath
	// ���  : ������ 2010-7-6 10:22:23
	// ����  : CXmlNode * 
	// ����  : const char *xmlpathname /xx/yy/��/xx/yy��xx/yy/��xx/yy�ĸ�ʽ
	// ����  : bool isTransSign   xmlpathname�Ƿ���ת���ת��
	// ����  : const char *nodename    �ڵ�����
	// ����  : bool isTransName        �ڵ������Ƿ���ת���ת��
	// ����  : const char *nodevalue   �ڵ�ֵ
	// ����  : bool isTransValue=true  �ڵ�ֵ�Ƿ���ת���ת��
	// ����  : ����һ���½ڵ㣬��xmlpathname��Ψһ����NULL
	CXmlNode * AddNodeByPath(const char *xmlpathname,bool isTransSign,const char *nodename,bool isTransName,const char *nodevalue,bool isTransValue=true);
	CXmlNode * AddNodeByPath(const char *xmlpathname,bool isTransSign,const char *nodename,bool isTransName,int nodevalue);
	CXmlNode * AddNodeByPath(const char *xmlpathname,bool isTransSign,const char *nodename,bool isTransName,long nodevalue);
	CXmlNode * AddNodeByPath(const char *xmlpathname,bool isTransSign,const char *nodename,bool isTransName,double nodevalue,int dec=2);

	// ������: GetNodeAttribute
	// ���  : ������ 2010-6-29 9:16:04
	// ����  : CXmlAttribute * ��������ָ��,���ж���ڵ㷵��NULL
	// ����  : const char *xmlpathname  /xx/yy/��/xx/yy��xx/yy/��xx/yy�ĸ�ʽ
	// ����  : bool isTransSign         xmlpathname�Ƿ���ת���ת��
	// ����  : const char *attrname     ��������
	// ����  : bool isTransAttrName     ���������Ƿ���ת���ת��
	// ����  : string &value            ����ֵ
	// ����  : bool isTransValue=true   ����ֵ�Ƿ���ת���ת��
	// ����  : ȡ������ֵ
	CXmlAttribute * GetNodeAttribute(const char *xmlpathname,bool isTransSign,const char *attrname,bool isTransAttrName,std::string &value,bool isTransValue=true);
	
	// ������: GetNodeAttribute
	// ���  : ������ 2010-6-29 9:16:04
	// ����  : CXmlAttribute * ��������ָ��,���ж���ڵ㷵��NULL
	// ����  : const char *xmlpathname  /xx/yy/��/xx/yy��xx/yy/��xx/yy�ĸ�ʽ
	// ����  : bool isTransSign         xmlpathname�Ƿ���ת���ת��
	// ����  : const char *attrname     ��������
	// ����  : bool isTransAttrName     ���������Ƿ���ת���ת��
	// ����  : int &value               ����ֵ
	// ����  : ȡ������ֵ
	CXmlAttribute * GetNodeAttribute(const char *xmlpathname,bool isTransSign,const char *attrname,bool isTransAttrName,int &value);

	CXmlAttribute * GetNodeAttribute(const char *xmlpathname,bool isTransSign,const char *attrname,bool isTransAttrName,long &value);
	
	// ������: GetNodeAttribute
	// ���  : ������ 2010-6-29 9:16:04
	// ����  : CXmlAttribute * ��������ָ��,���ж���ڵ㷵��NULL
	// ����  : const char *xmlpathname  /xx/yy/��/xx/yy��xx/yy/��xx/yy�ĸ�ʽ
	// ����  : bool isTransSign         xmlpathname�Ƿ���ת���ת��
	// ����  : const char *attrname     ��������
	// ����  : bool isTransAttrName     ���������Ƿ���ת���ת��
	// ����  : double &value               ����ֵ
	// ����  : ȡ������ֵ
	CXmlAttribute * GetNodeAttribute(const char *xmlpathname,bool isTransSign,const char *attrname,bool isTransAttrName,double &value);
	
	// ������: SetNodeAttribute
	// ���  : ������ 2010-6-29 9:14:11
	// ����  : CXmlAttribute *   ��������ָ��,���ж���ڵ㷵��NULL
	// ����  : const char *xmlpathname /xx/yy/��/xx/yy��xx/yy/��xx/yy�ĸ�ʽ
	// ����  : bool isTransSign   xmlpathname�Ƿ���ת���ת��
	// ����  : const char *attrname   ��������
	// ����  : bool isTransAttrName    ���������Ƿ���ת���ת�� 
	// ����  : const char *value       ����ֵ
	// ����  : bool isTransValue=true  ����ֵ�Ƿ���ת���ת��
	// ����  : ���ýڵ����ԣ����޽ڵ��������½ڵ�
	CXmlAttribute * SetNodeAttribute(const char *xmlpathname,bool isTransSign,const char *attrname,bool isTransAttrName,const char *value,bool isTransValue=true);
	
	// ������: SetNodeAttribute
	// ���  : ������ 2010-6-29 9:14:11
	// ����  : CXmlAttribute *   ��������ָ��,���ж���ڵ㷵��NULL
	// ����  : const char *xmlpathname /xx/yy/��/xx/yy��xx/yy/��xx/yy�ĸ�ʽ
	// ����  : bool isTransSign   xmlpathname�Ƿ���ת���ת��
	// ����  : const char *attrname   ��������
	// ����  : bool isTransAttrName    ���������Ƿ���ת���ת�� 
	// ����  : int value       ����ֵ
	// ����  : ���ýڵ����ԣ����޽ڵ��������½ڵ�
	CXmlAttribute * SetNodeAttribute(const char *xmlpathname,bool isTransSign,const char *attrname,bool isTransAttrName,int value);

	CXmlAttribute * SetNodeAttribute(const char *xmlpathname,bool isTransSign,const char *attrname,bool isTransAttrName,long value);
	
	// ������: SetNodeAttribute
	// ���  : ������ 2010-6-29 9:14:11
	// ����  : CXmlAttribute *   ��������ָ��,���ж���ڵ㷵��NULL
	// ����  : const char *xmlpathname /xx/yy/��/xx/yy��xx/yy/��xx/yy�ĸ�ʽ
	// ����  : bool isTransSign   xmlpathname�Ƿ���ת���ת��
	// ����  : const char *attrname   ��������
	// ����  : bool isTransAttrName    ���������Ƿ���ת���ת�� 
	// ����  : double value       ����ֵ
	// ����  : intdouble dec       С��λ��
	// ����  : ���ýڵ����ԣ����޽ڵ��������½ڵ�
	CXmlAttribute * SetNodeAttribute(const char *xmlpathname,bool isTransSign,const char *attrname,bool isTransAttrName,double value,int dec=2);

	// ������: ToBuffer
	// ���  : ������ 2010-6-14 11:22:14
	// ����  : bool 
	// ����  : char *buffer
	// ����  : ����XML���ݣ�ע�⻺��Ҫ���󣬿����ȵ�GetXmlDataLen����֪��С
	bool ToBuffer(char *buffer);
	
	// ������: SetNodeValueByPath
	// ���  : ������ 2010-6-23 15:11:46
	// ����  : CXmlNode * ���ڵ��ж������NULL
	// ����  : const char *xmlpathname /xx/yy/��/xx/yy��xx/yy/��xx/yy�ĸ�ʽ
	// ����  : bool isTransSign xmlpathname�Ƿ���ת���ת��
	// ����  : double *value  �ڵ�ֵ
	// ����  : int dec=2 �ڵ�ֵ��С��λ��
	// ����  : ���ýڵ�ֵ���ڵ㲻����������
	CXmlNode * SetNodeValueByPath(const char *xmlpathname, bool isTransSign,double value,int dec=2);

	// ������: SetNodeValueByPath
	// ���  : ������ 2010-6-23 15:11:46
	// ����  : CXmlNode * ���ڵ��ж������NULL
	// ����  : const char *xmlpathname /xx/yy/��/xx/yy��xx/yy/��xx/yy�ĸ�ʽ
	// ����  : bool isTransSign xmlpathname�Ƿ���ת���ת��
	// ����  : const char *value  �ڵ�ֵ
	// ����  : bool isTransValue=true �ڵ�ֵ�Ƿ���ת���ת��
	// ����  : ���ýڵ�ֵ���ڵ㲻����������
	CXmlNode * SetNodeValueByPath(const char *xmlpathname, bool isTransSign,int value);

	CXmlNode * SetNodeValueByPath(const char *xmlpathname, bool isTransSign,long value);

	// ������: SetNodeValueByPath
	// ���  : ������ 2010-6-23 15:11:46
	// ����  : CXmlNode * ���ڵ��ж������NULL
	// ����  : const char *xmlpathname /xx/yy/��/xx/yy��xx/yy/��xx/yy�ĸ�ʽ
	// ����  : bool isTransSign xmlpathname�Ƿ���ת���ת��
	// ����  : const char *value  �ڵ�ֵ
	// ����  : bool isTransValue=true �ڵ�ֵ�Ƿ���ת���ת��
	// ����  : ���ýڵ�ֵ���ڵ㲻����������
	CXmlNode * SetNodeValueByPath(const char *xmlpathname,bool isTransSign,const char *value,bool isTransValue=true);

	// ������: GetNodeValueByPath
	// ���  : ������ 2010-6-23 15:09:31
	// ����  : CXmlNode * ���ڵ��ж���򲻴��ڷ���NULL
	// ����  : const char *xmlpathname  /xx/yy/��/xx/yy��xx/yy/��xx/yy�ĸ�ʽ
	// ����  : bool isTransSign   xmlpathname�Ƿ���ת���ת��
	// ����  : double &value    �ڵ�ֵ
	// ����  : ȡ�ýڵ�ֵ�����ж���ڵ㷵��false
	CXmlNode * GetNodeValueByPath(const char *xmlpathname,bool isTransSign,double &value);

	// ������: GetNodeValueByPath
	// ���  : ������ 2010-6-23 15:09:31
	// ����  : CXmlNode * ���ڵ��ж���򲻴��ڷ���NULL
	// ����  : const char *xmlpathname  /xx/yy/��/xx/yy��xx/yy/��xx/yy�ĸ�ʽ
	// ����  : bool isTransSign   xmlpathname�Ƿ���ת���ת��
	// ����  : int &value    �ڵ�ֵ
	// ����  : ȡ�ýڵ�ֵ�����ж���ڵ㷵��false
	CXmlNode * GetNodeValueByPath(const char *xmlpathname,bool isTransSign,int &value);

	CXmlNode * GetNodeValueByPath(const char *xmlpathname,bool isTransSign,long &value);

	// ������: GetNodeValueByPath
	// ���  : ������ 2010-6-23 15:09:31
	// ����  : CXmlNode * ���ڵ��ж���򲻴��ڷ���NULL
	// ����  : const char *xmlpathname  /xx/yy/��/xx/yy��xx/yy/��xx/yy�ĸ�ʽ
	// ����  : bool isTransSign   xmlpathname�Ƿ���ת���ת��
	// ����  : std::string &value    �ڵ�ֵ
	// ����  : bool isTransValue=true   �ڵ�ֵ�Ƿ���ת���ת��
	// ����  : ȡ�ýڵ�ֵ�����ж���ڵ㷵��false
	CXmlNode * GetNodeValueByPath(const char *xmlpathname,bool isTransSign,std::string &value,bool isTransValue=true);

	// ������: GetRoot
	// ���  : ������ 2010-6-23 9:10:24
	// ����  : CXmlNode * 
	// ����  : ȡ�ø��ڵ㣬ע����ڵ������Ƽ�ֵ�����������ڵ����ʵ��XML����
	CXmlNode * GetRoot();

	// ������: XmlCopy
	// ���  : ������ 2010-6-23 9:11:05
	// ����  : bool 
	// ����  : CBFXml *srcxml
	// ����  : XML���ƣ���srcxml��������ȫ���Ƶ���XML����
	bool XmlCopy(CBF_Xml *srcxml);

	// ������: GetNodeByPath
	// ���  : ������ 2010-6-22 15:40:36
	// ����  : CXmlNode * �����ڻ�������NULL
	// ����  : const char *xmlpathname ����Ϊ/xx/yy/��/xx/yy��xx/yy/��xx/yy�ĸ�ʽ
	// ����  : bool isTransSign=true
	// ����  : ���ݽڵ�·�����ҽڵ�,���ж���򷵻�NULL
	CXmlNode * GetNodeByPath(const char *xmlpathname,bool isTransSign=true);

	// ������: DeleteAllNode
	// ���  : ������ 2010-6-15 13:30:44
	// ����  : void 
	// ����  : ɾ�����нڵ�
	void DeleteAllNode();
	

	// ������: GetXmlDataLen
	// ���  : ������ 2010-6-14 11:22:00
	// ����  : int 
	// ����  : ȡ��XML�����ݳ���
	int GetXmlDataLen();

	// ������: ToBuffer
	// ���  : ������ 2010-6-14 11:22:14
	// ����  : bool 
	// ����  : char *buffer
	// ����  : int &buflen  ����buffer�Ļ����С������xml���ݳ���
	// ����  : ����XML���ݣ������岻���������
	bool ToBuffer(char *buffer,int &buflen);

	// ������: ToFile
	// ���  : ������ 2010-6-14 11:23:03
	// ����  : bool 
	// ����  : const char *filename
	// ����  : ����XML�������ļ�
	bool ToFile(const char *filename);

	// ������: ToString
	// ���  : ������ 2010-6-14 11:23:18
	// ����  : bool 
	// ����  : string &xmlstring
	// ����  : ����XML
	bool ToString(std::string &xmlstring);


	// ������: FromFile
	// ���  : ������ 2010-6-14 11:26:54
	// ����  : bool 
	// ����  : const char *xmlfile
	// ����  : int layer=0  0ΪDOM���� ����0ΪSAX������Ϊ�������
	// ����  : ���ļ��н���XML
	bool FromFile(const char *xmlfile,int layer=0);

	// ������: FromBuffer
	// ���  : ������ 2010-6-14 11:27:53
	// ����  : bool 
	// ����  : const char *xmlbuf
	// ����  : int layer=0  0ΪDOM���� ����0ΪSAX������Ϊ�������
	// ����  : �����ݻ������XML,xmlbufҪ�н�����0
	bool FromBuffer(const char *xmlbuf,int layer=0);


	CBF_Xml();
	virtual ~CBF_Xml();

protected:

	// ������: GetNodeName
	// ���  : ������ 2010-6-22 15:42:32
	// ����  : int 
	// ����  : std::string xmlpathname  �ڵ�·������Ϊ/xx/yy/��/xx/yy��xx/yy/��xx/yy�ĸ�ʽ
	// ����  : vector<std::string> &namelist
	// ����  : ȡ�ýڵ������б�
	int GetNodeName(std::string xmlpathname, std::vector<std::string> *namelist);

	CXmlNode m_pRootNode; //���ڵ㣬�̶�Ϊ��

	CXmlParse m_parseXml;  //������
};
#undef  BFXML_EXPORT
#endif // !defined(AFX_BFXML_H__50F338A7_6795_462B_8771_84225CFCEBB1__INCLUDED_)
