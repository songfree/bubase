#ifndef D_XML_CNODE_H_HEADER_INCLUDED_C1238B96
#define D_XML_CNODE_H_HEADER_INCLUDED_C1238B96

#include "attribute.h"
#include "public.h"

#ifdef XML_EXPORTS
#define XML_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define XML_EXPORT __declspec(dllimport)
#else
#define XML_EXPORT 
#endif
#endif

class XML_EXPORT CNode
{
public:


	/// ������: setNodeValue
	/// ���  : ������ 2008-7-11 10:00:02
	/// ����  : void 
	/// �������  : double value
	/// ����  : ���ýڵ�ֵ
	void setNodeValue(double value);

	/// ������: setNodeValue
	/// ���  : ������ 2008-7-11 9:59:58
	/// ����  : void 
	/// �������  : long value
	/// ����  : ���ýڵ�ֵ
	void setNodeValue(long value);

	/// ������: setNodeValue
	/// ���  : ������ 2008-7-11 9:59:46
	/// ����  : void 
	/// �������  : int value
	/// ����  : ���ýڵ�ֵ
	void setNodeValue(int value);

	/// ������: setNodeName
	/// ���  : ������ 2008-7-11 9:59:41
	/// ����  : void 
	/// �������  : double name
	/// ����  : ���ýڵ�����
	void setNodeName(double name);

	/// ������: setNodeName
	/// ���  : ������ 2008-7-11 9:59:34
	/// ����  : void 
	/// �������  : long name
	/// ����  : ���ýڵ�����
	void setNodeName(long name);

	/// ������: setNodeName
	/// ���  : ������ 2008-7-11 9:59:23
	/// ����  : void 
	/// �������  : int name
	/// ����  : ���ýڵ�����
	void setNodeName(int name);

	/// ������: setNodeValue
	/// ���  : ������ 2008-7-11 9:54:41
	/// ����  : void 
	/// �������  : string value
	/// �������  : bool isTransSign  �Ƿ�ת���ת��
	/// ����  : ���ýڵ�ֵ
	void setNodeValue(std::string value,bool isTransSign=true);

	/// ������: setNodeName
	/// ���  : ������ 2008-7-11 9:54:38
	/// ����  : void 
	/// �������  : string name
	/// �������  : bool isTransSign �Ƿ�ת���ת��
	/// ����  : ���ýڵ�����
	void setNodeName(std::string name,bool isTransSign=true);

	/// ������: getTransSign
	/// ���  : ������ 2008-7-11 9:54:03
	/// ����  : int 
	/// �����������  : std::string &value
	/// ����  : ȥ��ת���
	int getTransSign(std::string &value);


	/// ������: putTransSign
	/// ���  : ������ 2008-7-11 9:54:07
	/// ����  : int 
	/// �����������  : std::string &value
	/// ����  : ����ת���
	int putTransSign(std::string &value);

	/// ������: IsDouble
	/// ���  : ������ 2008-6-16 12:58:04
	/// ����  : bool 
	/// �������  : const char *str
	/// ����  : �Ƿ񸡵�ֵ
	bool IsDouble(const char *str);

	/// ������: IsNumber
	/// ���  : ������ 2008-6-16 12:58:01
	/// ����  : bool 
	/// �������  : const char *str
	/// ����  : �Ƿ�����
	bool IsNumber(const char *str);

	/// ������: GetAttributeValue
	/// ���  : ������ 2008-6-16 12:52:43
	/// ����  : int 
	/// �������  : std::string name
	/// �������  : double &value
	/// ����  : ȡ������ֵ
	int GetAttributeValue(std::string name,double &value);


	/// ������: GetAttributeValue
	/// ���  : ������ 2008-6-16 12:52:43
	/// ����  : int 
	/// �������  : std::string name
	/// �������  : long &value
	/// ����  : ȡ������ֵ
	int GetAttributeValue(std::string name,long &value);

	/// ������: GetAttributeValue
	/// ���  : ������ 2008-6-16 12:52:43
	/// ����  : int 
	/// �������  : std::string name
	/// �������  : int &value
	/// ����  : ȡ������ֵ
	int GetAttributeValue(std::string name,int &value);

	/// ������: GetAttributeValue
	/// ���  : ������ 2008-6-16 12:52:43
	/// ����  : int 
	/// �������  : std::string name
	/// �������  : std::string &value
	/// ����  : ȡ������ֵ������ת���ת��
	int GetAttributeValue(std::string name,std::string &value);

	/// ������: *SetAttribute
	/// ���  : ������ 2008-5-23 14:27:36
	/// ����  : CAttribute 
	/// �������  : string name
	/// �������  : double value
	/// ����  : ���ýڵ������
	CAttribute *SetAttribute(std::string name, double value);

	/// ������: *SetAttribute
	/// ���  : ������ 2008-5-23 14:27:36
	/// ����  : CAttribute 
	/// �������  : string name
	/// �������  : long value
	/// ����  : ���ýڵ������
	CAttribute *SetAttribute(std::string name, long value);

	/// ������: *SetAttribute
	/// ���  : ������ 2008-5-23 14:27:36
	/// ����  : CAttribute 
	/// �������  : string name
	/// �������  : int value
	/// ����  : ���ýڵ������
	CAttribute *SetAttribute(std::string name, int value);

	/// ������: *SetAttribute
	/// ���  : ������ 2008-5-23 14:27:36
	/// ����  : CAttribute 
	/// �������  : string name
	/// �������  : string value
	/// �������  : bool isTransSign �Ƿ�ת���ת��
	/// ����  : ���ýڵ�����ԣ�����д������޸ķ�������
	CAttribute *SetAttribute(std::string name,std::string value,bool isTransSign=false);


	/// ������: *SetAttribute
	/// ���  : ������ 2008-5-23 14:27:36
	/// ����  : CAttribute 
	/// �������  : string name
	/// �������  : double value
	/// ����  : ���ýڵ������
	CAttribute *SetNewAttribute(std::string name, double value);
	
	/// ������: *SetAttribute
	/// ���  : ������ 2008-5-23 14:27:36
	/// ����  : CAttribute 
	/// �������  : string name
	/// �������  : long value
	/// ����  : ���ýڵ������
	CAttribute *SetNewAttribute(std::string name, long value);
	
	/// ������: *SetAttribute
	/// ���  : ������ 2008-5-23 14:27:36
	/// ����  : CAttribute 
	/// �������  : string name
	/// �������  : int value
	/// ����  : ���ýڵ������
	CAttribute *SetNewAttribute(std::string name, int value);
	
	/// ������: *SetAttribute
	/// ���  : ������ 2008-5-23 14:27:36
	/// ����  : CAttribute 
	/// �������  : string name
	/// �������  : string value
	/// �������  : bool isTransSign �Ƿ�ת���ת��
	/// ����  : ����һ���µ����ԣ������ж�
	CAttribute *SetNewAttribute(std::string name,std::string value,bool isTransSign=false);

	/// ������: getEndChild
	/// ���  : ������ 2007-2-6 10:47:17
	/// ����  : CNode * 
	/// ����  : ������һ���ӽڵ�
	CNode * getEndChild();

	/// ������: getLastSibling
	/// ���  : ������ 2007-2-6 10:45:59
	/// ����  : CNode * 
	/// ����  : ��ȡ��һ�ٽ��ڵ�
	CNode * getLastSibling();

	CAttribute *getAttributes(std::string attrname);
    CNode();
    virtual ~CNode();

	/// ������: getValue
	/// ���  : ������ 2006-6-23 19:37:42
	/// ����  : string 
	/// ����  : ��ýڵ�ֵ����ת��ת��
	std::string getValue();

	/// ������: getName
	/// ���  : ������ 2006-6-23 19:37:53
	/// ����  : string 
	/// ����  : ��ýڵ����� ��ת��ת��
	std::string getName();

	/// ������: *getAttributes
	/// ���  : ������ 2006-6-23 19:38:03
	/// ����  : CAttribute 
	/// ����  : ��ýڵ�������
	CAttribute *getAttributes();

	/// ������: *getNextSibling
	/// ���  : ������ 2006-6-23 19:38:21
	/// ����  : CNode 
	/// ����  : ��ȡ��һ�ٽ��ڵ�
	CNode *getNextSibling();

	/// ������: *getFirstChild
	/// ���  : ������ 2006-6-23 19:38:33
	/// ����  : CNode 
	/// ����  : ��ȡ��һ���ӽڵ�
	CNode *getFirstChild();

	/// ������: *getParentNode
	/// ���  : ������ 2006-6-23 19:38:51
	/// ����  : CNode 
	/// ����  : ��ȡ���ڵ�
	CNode *getParentNode();

    std::string m_name; ///�ڵ�����

    std::string m_value; ///�ڵ�ֵ

    CAttribute *m_attribute;///�ڵ�����

    CNode *m_parentNode;///���ڵ�ָ��

    CNode *m_nextNode; ///�ٽ��½ڵ�ָ��

    CNode *m_firstChildNode;///��һ���ӽڵ�ָ��

	CNode *m_endChildNode;///���һ���ӽڵ�ָ��

	CNode *m_lastNode; ///�ٽ��Ͻڵ�ָ��


	

};
#undef XML_EXPORT


#endif /* D_XML_CNODE_H_HEADER_INCLUDED_C1238B96 */
