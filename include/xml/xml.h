

/***********************************************************************************
  ������־��¼
  ������: ������
  �������� 2008-3-12 13:26:20
  �汾: V2.06 
  ˵��: ������xml���ݰ������ٶ��Ż�
 ***********************************************************************************/


#ifndef CXML_H_HEADER_INCLUDED_C123C05E
#define CXML_H_HEADER_INCLUDED_C123C05E

#include "public.h"
#include "attribute.h"
#include "node.h"
#include "xmlanalysis.h"



#ifdef XML_EXPORTS
#define XML_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define XML_EXPORT __declspec(dllimport)
#else
#define XML_EXPORT 
#endif
#endif
///ת���
///&quot; "
///&lt; <
///&gt; >
///&amp; &
///&apos; '
class XML_EXPORT CXML
{
public:

    CXML();
	CXML(char *filename);
    virtual ~CXML();

	CNode *m_firstNode;///�������׽ڵ�ָ��

	CXMLAnalysis m_xmlAnalysis;//xml�����ݽ�����
	
private:
    CNode *m_curNode;  ///��ǰ�ڵ�ָ��


	//// ������: GetXmlDatalen
	//// ���  : ������ 2008-6-19 17:38:50
	//// ����  : int 
	//// ����  : CNode *pnode
	//// ����  : int &xmldatalen
	//// ����  : int tier �ڵ�Ĳ��
	//// ����  : ��ĳһ�ڵ��xml���ݳ���
	int GetXmlDatalen(CNode *pnode,int &xmldatalen, int tier);
public:

	/// ������: ReplaceNode
	/// ���  : ������ 2009-6-18 9:40:45
	/// ����  : bool 
	/// ����  : CNode *newnode
	/// ����  : CNode *oldnode
	/// ����  : ���½ڵ��滻���ɽڵ㣬���ڵ���ǰ������deletepoint��newnode�Ͽ������ڵ������delete���ɽڵ㣬������ڴ�й¶
	bool ReplaceNode(CNode *newnode,CNode *oldnode);

	/// ������: AddNode
	/// ���  : ������ 2009-1-20 9:58:08
	/// ����  : bool 
	/// ����  : CNode *anode
	/// ����  : bool first true�ӵ��׽ڵ� flase�ӵ�β�ڵ�
	/// ����  : �ڵ�ǰ�ڵ����Ӵ���Ľڵ㣬ע��˽ڵ���ٽ��ڵ�(����)������
	bool AddNode(CNode *anode,bool first=false);

	/// ������: deleteNodePoint
	/// ���  : ������ 2009-1-20 10:25:46
	/// ����  : bool 
	/// ����  : CNode *pnode
	/// ����  : ���ڵ��xml�ڵ�������ȥ�����ڵ㲻����
	bool deleteNodePoint(CNode *pnode);


	/// ������: GetXmlDatalen
	/// ���  : ������ 2008-6-19 17:38:21
	/// ����  : int 
	/// ����  : ȡxml���ݵĳ���
	int GetXmlDatalen();
	

	/// ������: XmlCopyNode
	/// ���  : ������ 2008-6-19 17:38:32
	/// ����  : bool 
	/// ����  : CNode *srcnode
	/// ����  : bool iscopysrc=true  true����srcnode
	/// ����  : �ڵ�֮����и���
	bool XmlCopyNode(CNode *srcnode,bool iscopysrc=true);

	/// ������: XmlCopy
	/// ���  : ������ 2008-5-23 11:43:56
	/// ����  : bool 
	/// ����  : CXML* srcxml
	/// ����  : ��Ŀ��srcxml�������copy����xml��,srcxml����
	bool XmlCopy(CXML* srcxml);


	/// ������: XmlPointCopy
	/// ���  : ������ 2009-1-20 9:28:20
	/// ����  : bool 
	/// ����  : CXML *srcxml
	/// ����  : ��Ŀ��srcxml��ָ�븴�Ƹ���xml��srcxml��Ϊ��
	bool XmlPointCopy(CXML *srcxml);

	/// ������: toString
	/// ���  : ������ 2008-3-12 13:25:43
	/// ����  : bool 
	/// ����  : std::string &buffer
	/// ����  : ��xml���ݰ��ŵ�sting��ȥ�������ô˷���
	bool toString(std::string &buffer);


	/// ������: getTransSign
	/// ���  : ������ 2007-7-6 12:23:17
	/// ����  : int 
	/// ����  : string &value
	/// ����  : ����ת�����valueת������ֵ
	int getTransSign(std::string &value);

	/// ������: putTransSign
	/// ���  : ������ 2007-7-6 12:23:21
	/// ����  : int 
	/// ����  : string &value
	/// ����  : ������<>�ȵ�valueת�ɴ���ת�����value
	int putTransSign(std::string &value);

	/// ������: getLastSibling
	/// ���  : ������ 2007-2-6 10:55:00
	/// ����  : CNode * 
	/// ����  : ��ȡ��ǰ�ڵ����һ�ٽ��ڵ�
	CNode * getLastSibling();

	/// ������: getEndChildItem
	/// ���  : ������ 2007-2-6 10:54:44
	/// ����  : CNode * 
	/// ����  : ��ȡ��ǰ�ڵ�����һ���ӽڵ�
	CNode * getEndChildItem();

	/// ������: fromFile
	/// ���  : ������ 2006-6-23 19:43:47
	/// ����  : int 
	/// ����  : char *filename
	/// ����  : ���ļ���ȡXML���ݱ�
	int fromFile(char *filename);

	/// ������: fromBuffer
	/// ���  : ������ 2006-6-23 19:44:15
	/// ����  : bool 
	/// ����  : char * buffer
	/// ����  : �����ݻ������л��XML���ݱ�
    bool fromBuffer(char * buffer);

	/// ������: toBuffer
	/// ���  : ������ 2006-6-23 19:44:26
	/// ����  : bool 
	/// ����  : char *buffer
	/// ����  : ������������������ϳ�XML���ݱ�����buffer���Ȳ������ܻ�崵�,��������
	bool toBuffer(char *buffer);
	
	/// ������: toBuffer
	/// ���  : ������ 2008-6-19 17:37:43
	/// ����  : bool 
	/// ����  : char *buffer
	/// ����  : int &bufsize buffer�Ĵ�С������С��������false
	/// ����  : ������������������ϳ�XML���ݱ�
	bool toBuffer(char *buffer,int &bufsize);

	/// ������: toFile
	/// ���  : ������ 2006-6-23 19:44:46
	/// ����  : bool 
	/// ����  : char *filename
	/// ����  : ������������������ϳ�XML���ݱ�������ļ�
	bool toFile(char *filename);

	/// ������: nodeToBuffer
	/// ���  : ������ 2006-6-23 19:45:16
	/// ����  : int 
	/// ����  : CNode *pnode
	/// ����  : char *buffer
	/// ����  : int tier
	/// ����  : ��ָ���ڵ�������ϳ�XML���ݱ�
    int nodeToBuffer(CNode *pnode,char *buffer,int &xmldatalen,int tier);

	/// ������: analysisXML
	/// ���  : ������ 2006-6-23 19:45:32
	/// ����  : bool 
	/// ����  : const char *buffer
	/// ����  : int bufferlen
	/// ����  : ����XML���ݱ���ø���������
    bool analysisXML(const char *buffer,int bufferlen);

	/// ������: *getRoot
	/// ���  : ������ 2006-6-23 19:45:45
	/// ����  : CNode 
	/// ����  : ȡ�ø��������ǰ�ڵ�Ϊ���ڵ�
    CNode *getRoot();

	/// ������: *getFirstChildItem
	/// ���  : ������ 2006-6-23 19:45:56
	/// ����  : CNode 
	/// ����  : ȡ�õ�ǰ�ڵ�ĵ�һ���ӽڵ�,��ǰ�ڵ�Ϊ�˽ڵ�
    CNode *getFirstChildItem();

	/// ������: *getFirstChildItem
	/// ���  : ������ 2006-6-23 19:46:12
	/// ����  : CNode 
	/// ����  : const string item
	/// ����  : ȡ�õ�ǰ�ڵ���ӽڵ����ָ��������Ľڵ�,��ǰ�ڵ�Ϊ�˽ڵ�
    CNode *getFirstChildItem(const std::string item);


	/// ������: *getFirstItem
	/// ���  : ������ 2006-6-23 19:46:24
	/// ����  : CNode 
	/// ����  : ȡ�õ�ǰ�ڵ�ĵ�һ���ڵ㣬��ǰ�ڵ㲻�仯
    CNode *getFirstItem();

	/// ������: *getFirstItem
	/// ���  : ������ 2006-6-23 19:46:42
	/// ����  : CNode 
	/// ����  : const string item
	/// ����  : ȡ�õ�ǰ�ڵ��ָ����������ӽڵ�,��ǰ�ڵ�Ϊ�˽ڵ�
    CNode *getFirstItem(const std::string item);

    ///##ModelId=3EDC16E6021D
    ///##Documentation
    ///

	/// ������: *getNextSibling
	/// ���  : ������ 2006-6-23 19:46:53
	/// ����  : CNode 
	/// ����  : ȡ�õ�ǰ�ڵ���ٽ��ڵ�,��ǰ�ڵ�Ϊ�˽ڵ�
    CNode *getNextSibling();

	/// ������: *getNextSibling
	/// ���  : ������ 2006-6-23 19:47:21
	/// ����  : CNode 
	/// ����  : const string item
	/// ����  : ȡ�õ�ǰ�ڵ���ٽ��ڵ�Ϊָ���ڵ����Ľڵ�,��ǰ�ڵ�Ϊ�˽ڵ�
    CNode *getNextSibling(const std::string item);

	/// ������: *addItem
	/// ���  : ������ 2006-6-23 19:48:29
	/// ����  : CNode 
	/// ����  : const string name
	/// ����  : const string value
	/// ����  : �ڵ�ǰ�ڵ�������һ���ӽڵ�,��ǰ�ڵ㲻�仯
    CNode *addItem(const std::string name, const std::string value);

	/// ������: *addItemN
	/// ���  : ������ 2007-7-6 12:43:36
	/// ����  : CNode 
	/// ����  : const string name
	/// ����  : const string value
	/// ����  : �ڵ�ǰ�ڵ�������һ���ӽڵ�,��ǰ�ڵ㲻�仯,����ת��
	CNode *addItemN(const std::string name, const std::string value);

	/// ������: *addItem
	/// ���  : ������ 2006-6-23 19:48:40
	/// ����  : CNode 
	/// ����  : const string name
	/// ����  : const string value
	/// ����  : int i
	/// ����  : �ڵ�ǰ�ڵ�������һ���ӽڵ�,��ǰ�ڵ㲻�仯
    CNode *addItem(const std::string name, const std::string value,int i);

	/// ������: editItem
	/// ���  : ������ 2006-6-23 19:48:50
	/// ����  : bool 
	/// ����  : const string value
	/// ����  : �޸�ָ��������Ľڵ�,��ת���ת��
    bool editItem(const std::string value);


	/// ������: editItemN
	/// ���  : ������ 2007-7-5 23:12:32
	/// ����  : bool 
	/// ����  : const string value
	/// ����  : �޸�ָ��������Ľڵ�ֵ������ת���ת��
	bool editItemN(const std::string value);
	

	/// ������: editItem
	/// ���  : ������ 2006-6-23 19:49:02
	/// ����  : bool 
	/// ����  : const string value
	/// ����  : int i
	/// ����  : �޸�ָ��������Ľڵ�
    bool editItem(const std::string value,int i);


	/// ������: deleteItem
	/// ���  : ������ 2006-6-23 19:49:09
	/// ����  : bool 
	/// ����  : ɾ����ǰ�ڵ㣬��ǰ�ڵ�˽ڵ㸸�ڵ�
    bool deleteItem();

	/// ������: deleteNode
	/// ���  : ������ 2006-6-23 19:49:36
	/// ����  : bool 
	/// ����  : CNode *pnode
	/// ����  : ɾ��ָ���ڵ㼰�����ڵ�
    bool deleteNode(CNode *pnode);

	/// ������: setAttribute
	/// ���  : ������ 2006-6-23 19:49:52
	/// ����  : bool 
	/// ����  : const string name
	/// ����  : const string value
	/// ����  : ���õ�ǰ�ڵ������ֵ
    bool setAttribute(const std::string name,const std::string value);

	/// ������: getAttribute
	/// ���  : ������ 2006-6-23 19:50:00
	/// ����  : bool 
	/// ����  : const string name
	/// ����  : string &value
	/// ����  : ��ȡ��ǰ�ڵ������ֵ
    bool getAttribute(const std::string name,std::string &value);

	/// ������: *getAttributes
	/// ���  : ������ 2006-6-23 19:50:09
	/// ����  : CAttribute 
	/// ����  : ��ȡ��ǰ�ڵ������
	CAttribute *getAttributes();

	/// ������: delAttribute
	/// ���  : ������ 2006-6-23 19:50:19
	/// ����  : bool 
	/// ����  : const string name
	/// ����  : ɾ����ǰ�ڵ������
    bool delAttribute(const std::string name);

	/// ������: delAttribute
	/// ���  : ������ 2006-6-23 19:50:28
	/// ����  : bool 
	/// ����  : CNode *pNode
	/// ����  : ɾ��ָ���ڵ������
	bool delAttribute(CNode *pNode);
 
	/// ������: *getParentItem
	/// ���  : ������ 2006-6-23 19:42:59
	/// ����  : CNode 
	/// ����  : ��ø��׽ڵ�,��ǰ�ڵ�ı�
	CNode *getParentItem();

	/// ������: *getCurrentNode
	/// ���  : ������ 2006-6-23 19:42:47
	/// ����  : CNode 
	/// ����  : ��õ�ǰ�ڵ�
	CNode *getCurrentNode();


	/// ������: *setCurrentNode
	/// ���  : ������ 2006-6-23 19:42:28
	/// ����  : CNode 
	/// ����  : CNode *pNode
	/// ����  : ���õ�ǰ�ڵ�
	CNode *setCurrentNode(CNode *pNode);

	/// ������: getNodeName
	/// ���  : ������ 2006-6-23 19:53:28
	/// ����  : string 
	/// ����  : ��õ�ǰ�ڵ���
	std::string getNodeName();

	/// ������: getNodeValue
	/// ���  : ������ 2006-6-23 19:53:32
	/// ����  : string 
	/// ����  : ��õ�ǰ�ڵ�ֵ
	std::string getNodeValue();


	/// ������: toBufferOld
	/// ���  : ������ 2008-11-25 16:10:11
	/// ����  : bool 
	/// ����  : char *buffer
	/// ����  : û��</>����ʽ��xml����
	bool toBufferOld(char *buffer);

	/// ������: nodeToBufferOld
	/// ���  : ������ 2008-11-25 16:18:21
	/// ����  : int 
	/// ����  : CNode *pnode
	/// ����  : char *buffer
	/// ����  : int &xmldatalen
	/// ����  : int tier
	/// ����  : û��</>����ʽ��xml����
	int nodeToBufferOld(CNode *pnode,char *buffer,int &xmldatalen,int tier);

	//��λ��ǰ�ڵ㵽���ڵ�
	CNode *LocateRoot();

	//���ݽڵ����ֶ�λ���ò�ĵ�һ���ڵ�
	CNode *LocateFirstByName(char *pString);

	
	char m_errMsg[1025];
private:
	char m_buf[1000];
	char *GetCol(char *src, int index);
	CNode *CompLayerNode(CNode *pFirst,const std::string item);
};

#undef XML_EXPORT
#endif /* CXML_H_HEADER_INCLUDED_C123C05E */
