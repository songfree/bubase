// XmlParse.h: interface for the CXmlParse class.
//
//////////////////////////////////////////////////////////////////////

/***********************************************************************************
  ������־��¼
  ������: ������
  �������� 2009-4-26 9:51:10
  �汾: V1.00 
  ˵��: XML������
 ***********************************************************************************/


#if !defined(AFX_XMLPARSE_H__5EAB1EF5_B256_4EF3_8992_D9B75BFBFEC9__INCLUDED_)
#define AFX_XMLPARSE_H__5EAB1EF5_B256_4EF3_8992_D9B75BFBFEC9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "IXmlParse.h"
#include "BF_Date_Time.h"

#ifdef KERNEL_EXPORTS
#define KERNEL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNEL_EXPORT __declspec(dllimport)
#else
#define KERNEL_EXPORT 
#endif
#endif

class KERNEL_EXPORT CXmlParse  : public CIXmlParse
{
public:
	
	// ������: ParseXml
	// ���  : ������ 2010-6-3 10:01:26
	// ����  : bool 
	// ����  : std::string xmlbuf
	// ����  : CIXmlNode *node
	// ����  : int layer=0   0ΪDOMģʽ >0ΪSAXģʽ
	// ����  : ����XML�����ڵ�
	bool ParseXml(std::string xmlbuf,CIXmlNode *parentNode,int layer=0);

	CXmlParse();
	virtual ~CXmlParse();

protected:


	// ������: ParseNodeNameAttrDom
	// ���  : ������ 2010-7-1 11:26:47
	// ����  : CIXmlNode * 
	// ����  : CIXmlNode *parentnode
	// ����  : char *nameattr
	// ����  : bool isEndNode
	// ����  : �����ڵ���������
	CIXmlNode * ParseNodeNameAttrDom(CIXmlNode *parentnode,char *nameattr,bool isEndNode);
	

	// ������: FindEndNodeDom
	// ���  : ������ 2010-7-1 11:27:05
	// ����  : bool 
	// ����  : int bOffset
	// ����  : int eOffset
	// ����  : std::string nodename
	// ����  : int &vend
	// ����  : int &nextnodebegin
	// ����  : ���ҽڵ�Ľ���λ��
	bool FindEndNodeDom(int bOffset,int eOffset,std::string nodename,int &vend,int &nextnodebegin);


	// ������: ParseXmlSax
	// ���  : ������ 2010-7-1 11:27:23
	// ����  : bool 
	// ����  : std::string xmlbuf
	// ����  : CIXmlNode *parentNode
	// ����  : int layer=1
	// ����  : ��SAXģʽ����XML
	bool ParseXmlSax(std::string xmlbuf,CIXmlNode *parentNode,int layer=1);

	// ������: ParseXmlDom
	// ���  : ������ 2010-7-1 11:27:38
	// ����  : bool 
	// ����  : std::string xmlbuf
	// ����  : CIXmlNode *parentNode
	// ����  : ��DOMģʽ����XML
	bool ParseXmlDom(std::string xmlbuf,CIXmlNode *parentNode);

	// ������: ParseXmlDom
	// ���  : ������ 2010-7-1 11:27:56
	// ����  : bool 
	// ����  : int beginoffset
	// ����  : int endoffset
	// ����  : CIXmlNode *parentNode
	// ����  : ��DOMģʽ����XML�ĵݹ鷽��
	bool ParseXmlDom(int beginoffset, int endoffset, CIXmlNode *parentNode);

	bool ParseXmlDom2(int &beginoffset, int endoffset, CIXmlNode *parentNode);

	// ������: GetNodeName
	// ���  : ������ 2010-7-1 11:28:22
	// ����  : bool 
	// ����  : std::string nameattr
	// ����  : std::string &nodename
	// ����  : ȡ�ýڵ�����
	bool GetNodeName(std::string nameattr,std::string &nodename);

	// ������: ParseNodeAttribute
	// ���  : ������ 2010-6-12 16:06:45
	// ����  : int -1���ԷǷ�
	// ����  : std::string sattr  �����ַ���
	// ����  : CIXmlNode *addnode  �ڵ�
	// ����  : ��������
	int ParseNodeAttribute(std::string sattr,CIXmlNode *addnode);


	// ������: FindEndNode
	// ���  : ������ 2010-6-12 16:03:20
	// ����  : bool true�ҵ���
	// ����  : string xmlbuf  xml�ַ���
	// ����  : int bOffset  �ַ�����ʼλ��
	// ����  : std::string nodename   �ڵ�����
	// ����  : int &vend         �ڵ�ֵ�Ľ���λ��
	// ����  : int &nextnodebegin         �ڵ�ֵ�Ľ���λ��
	// ����  : ͨ��������ַ�������ʼλ�ã����Ҷ�Ӧ�Ľڵ����λ��
	bool FindEndNode(std::string xmlbuf,int bOffset,std::string nodename,int &vend,int &nextnodebegin);


	// ������: ParseNodeNameAttr
	// ���  : ������ 2010-6-12 16:01:16
	// ����  : CIXmlNode *   ���ӵĽڵ�
	// ����  : CIXmlNode *parentnode  ���ڵ�
	// ����  : std::string nameattr  ���������ַ���(������<>),��<xx>��<xx/>�е�xx
	// ����  : bool isEndNode        �Ƿ���/>����
	// ����  : �����ڵ����Ƽ����ԣ������汾ע�͵�
	CIXmlNode * ParseNodeNameAttr(CIXmlNode *parentnode,std::string nameattr,bool isEndNode);
	
	/// ������: LogMp
	/// ���  : ������ 2009-1-24 8:20:56
	/// ����  : int 
	/// ����  : int level
	/// ����  : const char *filename
	/// ����  : int line
	/// ����  : const char *fmt
	/// ����  : ...
	/// ����  : д��־
	int LogMp(int level, const char *filename, int line,const char *data,int datalen,const char *fmt,...);

	void GetName(char *filename);
	char m_errMsg[256];

	char *m_sXmlBuf;
	int   m_nXmlBufLen;

	int            m_nLogLevel; //��־����
	int            m_nId;
	std::string    m_sLogName;
	CBF_Date_Time  m_pDate;
	std::string    m_sCurDate;//��ǰ����
};
#undef  KERNEL_EXPORT
#endif // !defined(AFX_XMLPARSE_H__5EAB1EF5_B256_4EF3_8992_D9B75BFBFEC9__INCLUDED_)

