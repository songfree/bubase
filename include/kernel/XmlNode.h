// XmlNode.h: interface for the CXmlNode class.
//
//////////////////////////////////////////////////////////////////////

/***********************************************************************************
  ������־��¼
  ������: ������
  �������� 2009-4-26 9:49:37
  �汾: V1.00 
  ˵��: XML�ڵ��࣬XML�����ɶ���ڵ���ɣ�ÿ���ڵ��нڵ����Ժͽڵ�ֵ���
 ***********************************************************************************/


#if !defined(AFX_XMLNODE_H__FE9DD9EF_3AB1_403E_80C0_CB478F6B8B1F__INCLUDED_)
#define AFX_XMLNODE_H__FE9DD9EF_3AB1_403E_80C0_CB478F6B8B1F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "XmlNodeIndex.h"
#include "XmlParse.h"
#include "XmlAttributes.h"
#include "XmlTransSign.h"
#include "IXmlNode.h"

#ifdef KERNEL_EXPORTS
#define KERNEL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNEL_EXPORT __declspec(dllimport)
#else
#define KERNEL_EXPORT 
#endif
#endif

class KERNEL_EXPORT CXmlNode  : public CIXmlNode
{
public:
	// ������: * NewNode
	// ���  : ������ 2010-6-15 13:23:37
	// ����  : virtual CIXmlNode 
	// ����  : ����һ���µĽڵ�
	virtual CIXmlNode * NewNode();	

    // ������: SetAttribute
	// ���  : ������ 2010-6-8 10:36:50
	// ����  : CXmlAttribute * 
	// ����  : string attrname  ��������
	// ����  : bool isTransSign  �����Ƿ����ת���ת��
	// ����  : string attrvlaue   ����ֵ
	// ����  : bool isTransValue=true  ����ֵ�Ƿ����ת���ת��
	// ����  : ����һ������ �����д����Ƶ�����������ֵ���أ�����������������������
	virtual CXmlAttribute * SetAttribute(std::string attrname,bool isTransSign,std::string attrvlaue,bool isTransValue=true);

	virtual CXmlAttribute * SetAttribute(std::string attrname,bool isTransSign,int attrvlaue);

	virtual CXmlAttribute * SetAttribute(std::string attrname,bool isTransSign,long attrvlaue);

	virtual CXmlAttribute * SetAttribute(std::string attrname,bool isTransSign,double attrvlaue,int dec=2);


	// ������: * AddAttribute
	// ���  : ������ 2010-7-16 15:28:05
	// ����  : virtual CXmlAttribute 
	// ����  : std::string attrname
	// ����  : bool isTransSign
	// ����  : std::string attrvlaue
	// ����  : bool isTransValue=true
	// ����  : ����һ����ֵ�������ж���������
	virtual CXmlAttribute * AddAttribute(std::string attrname,bool isTransSign,std::string attrvlaue,bool isTransValue=true);

	// ������: GetParentNode
	// ���  : ������ 2010-6-8 16:22:14
	// ����  : CXmlNode * 
	// ����  : ȡ�ø��ڵ�
	virtual CIXmlNode * GetParentNode();

	// ������: getNextSibling
	// ���  : ������ 2010-6-8 16:22:26
	// ����  : CXmlNode * 
	// ����  : ȡ���ٽ��½ڵ�
	virtual CIXmlNode * getNextSibling();

	// ������: getLastSibling
	// ���  : ������ 2010-6-8 16:22:37
	// ����  : CXmlNode * 
	// ����  : ȡ���ٽ��Ͻڵ�
	virtual CIXmlNode * getLastSibling();

	// ������: GetEndChild
	// ���  : ������ 2010-6-8 16:22:49
	// ����  : CXmlNode * 
	// ����  : ȡ�����һ���ӽڵ�
	virtual CIXmlNode * GetEndChild();

	// ������: GetFirstChild
	// ���  : ������ 2010-6-8 16:23:00
	// ����  : CXmlNode * 
	// ����  : ȡ����ǰһ���ӽڵ�
	virtual CIXmlNode * GetFirstChild();

	// ������: GetNodeType
	// ���  : ������ 2010-6-4 16:39:31
	// ����  : int 
	// ����  : ȡ�ýڵ�����
	virtual int GetNodeType();

	// ������: SetNodeType
	// ���  : ������ 2010-6-4 16:39:17
	// ����  : void 
	// ����  : int type
	// ����  : ���ýڵ�����
	virtual void SetNodeType(int type);

	// ������: GetNodeValue
	// ���  : ������ 2010-6-4 15:57:32
	// ����  : std::string 
	// ����  : bool isTransSign=true true����ת���ת�� false��ת
	// ����  : ȡ�ýڵ�ֵ
	virtual std::string GetNodeValue(bool isTransSign=true);

	// ������: GetNodeName
	// ���  : ������ 2010-6-4 15:57:41
	// ����  : std::string 
	// ����  : bool isTransSign=true  true����ת���ת�� false��ת
	// ����  : ȡ�ýڵ�����
	virtual std::string GetNodeName(bool isTransSign=true);


	// ������: SetNodeValue
	// ���  : ������ 2010-6-9 12:01:05
	// ����  : void 
	// ����  : std::string value
	// ����  : bool isTransSign=true
	// ����  : ���ýڵ�ֵ�����ڵ�����ΪNODETYPE_ATTR��value���ȴ���1��������ΪNODETYPE_VALUE
	virtual void SetNodeValue(std::string value,bool isTransSign=true);

	virtual void SetNodeValue(int value);

	virtual void SetNodeValue(long value);
	
	virtual void SetNodeValue(double value,int dec=2);

	// ������: SetNodeName
	// ���  : ������ 2010-6-9 12:01:18
	// ����  : void 
	// ����  : std::string nodename
	// ����  : bool isTransSign=true
	// ����  : ���ýڵ����ƣ�ע������ڵ��Ѿ�����XML�����򲻿������������ƣ���Ϊ����Ҫ��������
	virtual void SetNodeName(std::string nodename,bool isTransSign=true);

	// ������: ParseChildNode
	// ���  : ������ 2010-6-2 16:37:04
	// ����  : bool 
	// ����  : int lay=0   �ӽڵ�Ĳ�Σ�0ȫ���⿪��>0�⿪���  1ֻ�⿪���ڵ���ӽڵ� 
	// ����  : �����ӽڵ�
	virtual bool ParseChildNode(int layer=0);

	// ������: AddChildNode
	// ���  : ������ 2010-6-4 15:14:22
	// ����  : int   ���ش˽ڵ���ӽڵ���
	// ����  : CIXmlNode *anode    Ҫ����Ľڵ� ע��ڵ�����Ҫ�У���������������������
	// ����  : bool first=false   true���ӵ�ǰ�棬false���ӵ����
	// ����  : ����һ���ӽڵ�,ע��ڵ�anode�Ľڵ����ͼ��ڵ�����Ҫ��֮ǰ���ú�
	virtual int AddChildNode(CIXmlNode *anode,bool first=false);

	// ������: SetParseClass
	// ���  : ������ 2010-6-17 13:35:25
	// ����  : virtual void 
	// ����  : void *parse
	// ����  : ���ý�����ָ��
	virtual void SetParseClass(void *parse);

	// ������: SetParseChilded
	// ���  : ������ 2010-6-22 11:56:42
	// ����  : virtual void 
	// ����  : ���ô˽ڵ��ѽ����½ڵ�
	virtual void SetParseChilded();
	

public:

	// ������: CreateNodeAttrIndex
	// ���  : ������ 2010-7-16 16:07:47
	// ����  : void 
	// ����  : �����ڵ���������� 
	void CreateNodeAttrIndex();

	bool GetNodeExpandFlag();
	bool GetNodeIndexFlag();
	
	// ������: NodeCopy
	// ���  : ������ 2010-6-22 17:11:28
	// ����  : bool 
	// ����  : CXmlNode *srcnode  Դ�ڵ�
	// ����  : ��ԴXML�еĽڵ㼰�����ڵ㸴�Ƶ����ڵ��У��滻���ڵ�����Ƽ�ֵ(��ɾ�����ڵ������ڵ㼰����)
	bool NodeCopy(CXmlNode *srcnode);

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
	
	// ������: ClearAllAttribute
	// ���  : ������ 2010-6-8 11:09:56
	// ����  : void 
	// ����  : ����������ԣ���������
	void ClearAllAttribute();

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

	

	// ������: ToString
	// ���  : ������ 2010-6-15 13:43:08
	// ����  : int 
	// ����  : std::string &xmlstring
	// ����  : int layer=0
	// ����  : ����XML
	int ToString(std::string &xmlstring,int layer=0);
	
	// ������: GetNodeDataLen
	// ���  : ������ 2010-6-11 9:57:53
	// ����  : int xml��С
	// ����  : int &xmldatalen  xml��С
	// ����  : int layer �ڵ��Σ�����ǰ��Ԥ���ո��Զ���
	// ����  : ȡ��XML���ݵĴ�С
	int GetNodeDataLen(int &xmldatalen,int layer);
	
	// ������: NodeToBuffer
	// ���  : ������ 2010-6-11 9:57:47
	// ����  : int 
	// ����  : char *buffer    ���ݻ��壬Ҫ�㹻��
	// ����  : int &xmldatalen     ���ݻ������ô�С
	// ����  : int layer   �ڵ��Σ�����ǰ��Ԥ���ո��Զ���
	// ����  : ���ڵ���������ݣ�ע�����ݻ����СҪ�㹻���ɵ���GetNodeDataLen��֪����С������ռ�
	int NodeToBuffer(char *buffer,int &xmldatalen,int layer);

	// ������: NodeToFile
	// ���  : ������ 2010-6-11 9:57:47
	// ����  : int 
	// ����  : FILE *fp    �Ѵ򿪵��ļ�ָ��
	// ����  : int layer   �ڵ��Σ�����ǰ��Ԥ���ո��Զ���
	// ����  : ���ڵ���������ݴ����ļ�
	int NodeToFile(FILE *fp,int layer);
	
	// ������: ReCreateIndex
	// ���  : ������ 2010-6-10 15:19:45
	// ����  : void 
	// ����  : �ؽ���ǰ�ڵ��ڸ��ڵ��е�����
	void ReCreateIndex();
	
	// ������: GetChildNode
	// ���  : ������ 2010-6-10 15:16:34
	// ����  : int                �����������ӽڵ���Ŀ
	// ����  : string nodename    �ڵ�����
	// ����  : bool isTransSign   �����Ƿ�ת���ת��
	// ����  : vector<CXmlNode *> &nodelist  �����������ӽڵ��б�
	// ����  : �������Ʋ����ӽڵ�
	int GetChildNode(std::string nodename,bool isTransSign, std::vector<CXmlNode *> &nodelist);
	
	// ������: DeleteAllChild
	// ���  : ������ 2010-6-9 11:48:52
	// ����  : void 
	// ����  : ɾ�����е��ӽڵ�
	void DeleteAllChild();
	
	// ������: DeleteChildNode
	// ���  : ������ 2010-6-8 16:42:27
	// ����  : bool 
	// ����  : std::string nodename  �ӽڵ�����
	// ����  : bool isTransSign=true  �ӽڵ������Ƿ�ת���ת��
	// ����  : ���ݽڵ�����ɾ���ӽڵ�,�ж��ɾ���
	bool DeleteChildNode(std::string nodename,bool isTransSign=true);
	
	// ������: DeletePoint
	// ���  : ������ 2010-6-4 16:47:52
	// ����  : void 
	// ����  : �����ڵ��xml������ȥ����Ȼ��Ϳ���delete��
	void DeletePoint();
	
	// ������: DeleteIndex
	// ���  : ������ 2010-6-8 16:25:17
	// ����  : bool 
	// ����  : CXmlNode *node
	// ����  : �ڱ��ڵ���ɾ���ӽڵ�Ϊnode��������ע�ⲻ�Ǹ��ݽڵ�����ɾ�����Ǹ����ӽڵ��ID��ɾ��
	bool DeleteIndex(CXmlNode *node);
	
	// ������: CreateIndex
	// ���  : ������ 2010-6-10 10:04:09
	// ����  : bool faselʱchildnode��Ϊ�˽ڵ���ӽڵ���ӽڵ�Ľڵ����Ͳ���
	// ����  : CXmlNode *childnode
	// ����  : �������ӽڵ�childndoe������
	bool CreateIndex(CXmlNode *childnode);
	
	// ������: GetParentIndex
	// ���  : ������ 2010-6-4 15:47:54
	// ����  : int 
	// ����  : ȡ�ñ��ڵ��ڸ��ڵ��е��������
	int GetParentIndex();
	
	// ������: SetParentIndex
	// ���  : ������ 2010-6-4 15:12:56
	// ����  : void 
	// ����  : int id  �����ڵ��������ţ����ڵ㰴�˽���Ψһ����
	// ����  : ���ô˽ڵ��ڸ��ڵ��������ţ��ڽ���xmlʱ���á�ʹ���߽�ֹʹ��
	void SetParentIndex(int id);
	
	// ������: AddChildNode
	// ���  : ������ 2010-6-10 9:58:31
	// ����  : CXmlNode * 
	// ����  : std::string nodename
	// ����  : bool isTransSign=true
	// ����  : ���ӽڵ���Ϊnodename�Ľڵ㣬�ڵ�����ΪNODETYPE_ATTR
	CXmlNode * AddChildNode(std::string nodename,bool isTransSign=true);


	// ������: *GetChildNode
	// ���  : ������ 2017-9-20 14:59:32
	// ����  : CXmlNode 
	// ����  : std::string nodename
	// ����  : bool isTransSign
	// ����  : ȡ��ָ�����Ƶĵ�һ���ӽڵ�
	CXmlNode *GetChildNode(std::string nodename, bool isTransSign);
	
	CXmlNode();
	virtual ~CXmlNode();

	CXmlAttributes m_attribute;///�ڵ�����

protected:

	// ������: ExpandValue
	// ���  : ������ 2010-6-4 14:23:18
	// ����  : bool ��չ������true
	// ����  : �жϴ˽ڵ�������ڵ��Ƿ�չ������δչ����չ�������ڵ�(����չ�������ڵ�)
	bool ExpandValue();

	// ������: DeleteParentIndex
	// ���  : ������ 2010-6-4 16:57:40
	// ����  : bool 
	// ����  : ȥ�����ڵ��ڸ��ڵ�������������ڸ��ڵ�ʱ���������Ҳ�����
	bool DeleteParentIndex();
	
protected:
	
	std::string m_sNodeValue;  //�ڵ�ֵ   <xx>m_sNodeName</xx>  �������xml�ļ��е�ֵ
	std::string m_sNodeName;   //�ڵ�����

	bool m_bIsExpand; //�ӽڵ��Ƿ�չ��
	int m_nNodeType;  //�ڵ�����

	int m_nParentIndex;   //���ڵ��ڸ��ڵ��е���������
	int m_nChildIndex;    //�ӽڵ���������
	int m_nChildNodeNum; //�ӽڵ���

	
	CXmlNodeIndex  *m_index;   //�ڵ�����
	
	CIXmlParse *m_parseXml;
	
    CXmlNode *m_parentNode;///���ڵ�ָ��
	
    CXmlNode *m_nextNode; ///�ٽ��½ڵ�ָ��
	CXmlNode *m_lastNode; ///�ٽ��Ͻڵ�ָ��
	
    CXmlNode *m_firstChildNode;  ///��һ���ӽڵ�ָ��
	CXmlNode *m_endChildNode;   ///���һ���ӽڵ�ָ��

	bool      m_bIsUseIndex;    //�Ƿ�ʹ������

};
#undef  KERNEL_EXPORT
#endif // !defined(AFX_XMLNODE_H__FE9DD9EF_3AB1_403E_80C0_CB478F6B8B1F__INCLUDED_)
