#ifndef CATTRIBUTE_H_HEADER_INCLUDED_C1238FC1
#define CATTRIBUTE_H_HEADER_INCLUDED_C1238FC1

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

#define NODEVALUELEN    40960      ///���Ľڵ�ֵ
#define NODENAMELEN    1024      ///���Ľڵ�����
#define NODENAMEATTRLEN    40960      ///�ڵ����Ƽ����Ե����ֵ

#define ITEMNAMELENGTH 40960      ///���Ľڵ�����
#define ITEMVALUELENGTH 40960   ///xml���Ľڵ�ֵ
//#define XMLDATALENGTH   1024000   ///������ļ�����󻺳�
#define TAB "   "
#define SPACE 32
#define TABLESPACE 9
#define ENTER 10


class XML_EXPORT CAttribute
{
public:

	/// ������: deleteNextAttribute
	/// ���  : ������ 2006-6-23 19:30:25
	/// ����  : bool 
	/// ����  : ɾ����������
	bool deleteNextAttribute();

	/// ������: getName
	/// ���  : ������ 2006-6-23 19:30:22
	/// ����  : string 
	/// ����  : ȡ����������
	std::string getName();

	/// ������: getValue
	/// ���  : ������ 2006-6-23 19:30:19
	/// ����  : string 
	/// ����  : ȡ������ֵ
	std::string getValue();

	/// ������: getName
	/// ���  : ������ 2006-6-23 19:29:50
	/// ����  : int 
	/// ����  : string name
	/// ����  : ȡ��������
	int getName(std::string &name);

	/// ������: getValue
	/// ���  : ������ 2006-6-23 19:29:38
	/// ����  : int 
	/// ����  : string value
	/// ����  : ȡ������ֵ
	int getValue(std::string &value);

	/// ������: *getNamedItem
	/// ���  : ������ 2006-6-23 19:29:19
	/// ����  : CAttribute 
	/// ����  : const string pAttrName
	/// ����  : �����������ƻ�ȡ����
	CAttribute *getNamedItem(const std::string pAttrName);

	/// ������: *getNextSibling
	/// ���  : ������ 2006-6-23 19:29:08
	/// ����  : CAttribute 
	/// ����  : ��ȡ��һ����
	CAttribute *getNextSibling();
    CAttribute();
	virtual ~CAttribute();

    CAttribute *m_next;///��һ����ָ��
	std::string  m_name; ///��������
    std::string  m_value; ///����ֵ
 	
};
#undef XML_EXPORT


#endif /* CATTRIBUTE_H_HEADER_INCLUDED_C1238FC1 */
