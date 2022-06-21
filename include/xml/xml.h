

/***********************************************************************************
  编码日志记录
  编码者: 王明松
  编码日期 2008-3-12 13:26:20
  版本: V2.06 
  说明: 对生成xml数据包进行速度优化
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
///转义符
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

	CNode *m_firstNode;///数据项首节点指针

	CXMLAnalysis m_xmlAnalysis;//xml的数据解析类
	
private:
    CNode *m_curNode;  ///当前节点指针


	//// 函数名: GetXmlDatalen
	//// 编程  : 王明松 2008-6-19 17:38:50
	//// 返回  : int 
	//// 参数  : CNode *pnode
	//// 参数  : int &xmldatalen
	//// 参数  : int tier 节点的层次
	//// 描述  : 获某一节点的xml数据长度
	int GetXmlDatalen(CNode *pnode,int &xmldatalen, int tier);
public:

	/// 函数名: ReplaceNode
	/// 编程  : 王明松 2009-6-18 9:40:45
	/// 返回  : bool 
	/// 参数  : CNode *newnode
	/// 参数  : CNode *oldnode
	/// 描述  : 用新节点替换掉旧节点，请在调用前将调用deletepoint将newnode断开，请在调用完后delete掉旧节点，否则会内存泄露
	bool ReplaceNode(CNode *newnode,CNode *oldnode);

	/// 函数名: AddNode
	/// 编程  : 王明松 2009-1-20 9:58:08
	/// 返回  : bool 
	/// 参数  : CNode *anode
	/// 参数  : bool first true加到首节点 flase加到尾节点
	/// 描述  : 在当前节点增加传入的节点，注意此节点的临近节点(上下)不加入
	bool AddNode(CNode *anode,bool first=false);

	/// 函数名: deleteNodePoint
	/// 编程  : 王明松 2009-1-20 10:25:46
	/// 返回  : bool 
	/// 参数  : CNode *pnode
	/// 描述  : 将节点从xml节点链表中去除，节点不销毁
	bool deleteNodePoint(CNode *pnode);


	/// 函数名: GetXmlDatalen
	/// 编程  : 王明松 2008-6-19 17:38:21
	/// 返回  : int 
	/// 描述  : 取xml数据的长度
	int GetXmlDatalen();
	

	/// 函数名: XmlCopyNode
	/// 编程  : 王明松 2008-6-19 17:38:32
	/// 返回  : bool 
	/// 参数  : CNode *srcnode
	/// 参数  : bool iscopysrc=true  true复制srcnode
	/// 描述  : 节点之间进行复制
	bool XmlCopyNode(CNode *srcnode,bool iscopysrc=true);

	/// 函数名: XmlCopy
	/// 编程  : 王明松 2008-5-23 11:43:56
	/// 返回  : bool 
	/// 参数  : CXML* srcxml
	/// 描述  : 将目标srcxml里的内容copy至本xml中,srcxml不变
	bool XmlCopy(CXML* srcxml);


	/// 函数名: XmlPointCopy
	/// 编程  : 王明松 2009-1-20 9:28:20
	/// 返回  : bool 
	/// 参数  : CXML *srcxml
	/// 描述  : 将目标srcxml的指针复制给本xml，srcxml置为空
	bool XmlPointCopy(CXML *srcxml);

	/// 函数名: toString
	/// 编程  : 王明松 2008-3-12 13:25:43
	/// 返回  : bool 
	/// 参数  : std::string &buffer
	/// 描述  : 将xml数据包放到sting中去，建议用此方法
	bool toString(std::string &buffer);


	/// 函数名: getTransSign
	/// 编程  : 王明松 2007-7-6 12:23:17
	/// 返回  : int 
	/// 参数  : string &value
	/// 描述  : 将有转义符的value转成正常值
	int getTransSign(std::string &value);

	/// 函数名: putTransSign
	/// 编程  : 王明松 2007-7-6 12:23:21
	/// 返回  : int 
	/// 参数  : string &value
	/// 描述  : 将含有<>等的value转成带有转义符的value
	int putTransSign(std::string &value);

	/// 函数名: getLastSibling
	/// 编程  : 王明松 2007-2-6 10:55:00
	/// 返回  : CNode * 
	/// 描述  : 获取当前节点的上一临近节点
	CNode * getLastSibling();

	/// 函数名: getEndChildItem
	/// 编程  : 王明松 2007-2-6 10:54:44
	/// 返回  : CNode * 
	/// 描述  : 获取当前节点的最后一个子节点
	CNode * getEndChildItem();

	/// 函数名: fromFile
	/// 编程  : 王明松 2006-6-23 19:43:47
	/// 返回  : int 
	/// 参数  : char *filename
	/// 描述  : 从文件获取XML数据报
	int fromFile(char *filename);

	/// 函数名: fromBuffer
	/// 编程  : 王明松 2006-6-23 19:44:15
	/// 返回  : bool 
	/// 参数  : char * buffer
	/// 描述  : 从数据缓冲区中获得XML数据报
    bool fromBuffer(char * buffer);

	/// 函数名: toBuffer
	/// 编程  : 王明松 2006-6-23 19:44:26
	/// 返回  : bool 
	/// 参数  : char *buffer
	/// 描述  : 将各个数据项重新组合成XML数据报，若buffer长度不够可能会宕掉,不建议用
	bool toBuffer(char *buffer);
	
	/// 函数名: toBuffer
	/// 编程  : 王明松 2008-6-19 17:37:43
	/// 返回  : bool 
	/// 参数  : char *buffer
	/// 参数  : int &bufsize buffer的大小，若大小不够返回false
	/// 描述  : 将各个数据项重新组合成XML数据报
	bool toBuffer(char *buffer,int &bufsize);

	/// 函数名: toFile
	/// 编程  : 王明松 2006-6-23 19:44:46
	/// 返回  : bool 
	/// 参数  : char *filename
	/// 描述  : 将各个数据项重新组合成XML数据报输出到文件
	bool toFile(char *filename);

	/// 函数名: nodeToBuffer
	/// 编程  : 王明松 2006-6-23 19:45:16
	/// 返回  : int 
	/// 参数  : CNode *pnode
	/// 参数  : char *buffer
	/// 参数  : int tier
	/// 描述  : 将指定节点重新组合成XML数据报
    int nodeToBuffer(CNode *pnode,char *buffer,int &xmldatalen,int tier);

	/// 函数名: analysisXML
	/// 编程  : 王明松 2006-6-23 19:45:32
	/// 返回  : bool 
	/// 参数  : const char *buffer
	/// 参数  : int bufferlen
	/// 描述  : 分析XML数据报获得各个数据项
    bool analysisXML(const char *buffer,int bufferlen);

	/// 函数名: *getRoot
	/// 编程  : 王明松 2006-6-23 19:45:45
	/// 返回  : CNode 
	/// 描述  : 取得根数据项、当前节点为根节点
    CNode *getRoot();

	/// 函数名: *getFirstChildItem
	/// 编程  : 王明松 2006-6-23 19:45:56
	/// 返回  : CNode 
	/// 描述  : 取得当前节点的第一个子节点,当前节点为此节点
    CNode *getFirstChildItem();

	/// 函数名: *getFirstChildItem
	/// 编程  : 王明松 2006-6-23 19:46:12
	/// 返回  : CNode 
	/// 参数  : const string item
	/// 描述  : 取得当前节点的子节点里的指定数据项的节点,当前节点为此节点
    CNode *getFirstChildItem(const std::string item);


	/// 函数名: *getFirstItem
	/// 编程  : 王明松 2006-6-23 19:46:24
	/// 返回  : CNode 
	/// 描述  : 取得当前节点的第一个节点，当前节点不变化
    CNode *getFirstItem();

	/// 函数名: *getFirstItem
	/// 编程  : 王明松 2006-6-23 19:46:42
	/// 返回  : CNode 
	/// 参数  : const string item
	/// 描述  : 取得当前节点的指定数据项的子节点,当前节点为此节点
    CNode *getFirstItem(const std::string item);

    ///##ModelId=3EDC16E6021D
    ///##Documentation
    ///

	/// 函数名: *getNextSibling
	/// 编程  : 王明松 2006-6-23 19:46:53
	/// 返回  : CNode 
	/// 描述  : 取得当前节点的临近节点,当前节点为此节点
    CNode *getNextSibling();

	/// 函数名: *getNextSibling
	/// 编程  : 王明松 2006-6-23 19:47:21
	/// 返回  : CNode 
	/// 参数  : const string item
	/// 描述  : 取得当前节点的临近节点为指定节点名的节点,当前节点为此节点
    CNode *getNextSibling(const std::string item);

	/// 函数名: *addItem
	/// 编程  : 王明松 2006-6-23 19:48:29
	/// 返回  : CNode 
	/// 参数  : const string name
	/// 参数  : const string value
	/// 描述  : 在当前节点上增加一个子节点,当前节点不变化
    CNode *addItem(const std::string name, const std::string value);

	/// 函数名: *addItemN
	/// 编程  : 王明松 2007-7-6 12:43:36
	/// 返回  : CNode 
	/// 参数  : const string name
	/// 参数  : const string value
	/// 描述  : 在当前节点上增加一个子节点,当前节点不变化,不做转义
	CNode *addItemN(const std::string name, const std::string value);

	/// 函数名: *addItem
	/// 编程  : 王明松 2006-6-23 19:48:40
	/// 返回  : CNode 
	/// 参数  : const string name
	/// 参数  : const string value
	/// 参数  : int i
	/// 描述  : 在当前节点上增加一个子节点,当前节点不变化
    CNode *addItem(const std::string name, const std::string value,int i);

	/// 函数名: editItem
	/// 编程  : 王明松 2006-6-23 19:48:50
	/// 返回  : bool 
	/// 参数  : const string value
	/// 描述  : 修改指定数据项的节点,做转义符转换
    bool editItem(const std::string value);


	/// 函数名: editItemN
	/// 编程  : 王明松 2007-7-5 23:12:32
	/// 返回  : bool 
	/// 参数  : const string value
	/// 描述  : 修改指定数据项的节点值，不带转义符转换
	bool editItemN(const std::string value);
	

	/// 函数名: editItem
	/// 编程  : 王明松 2006-6-23 19:49:02
	/// 返回  : bool 
	/// 参数  : const string value
	/// 参数  : int i
	/// 描述  : 修改指定数据项的节点
    bool editItem(const std::string value,int i);


	/// 函数名: deleteItem
	/// 编程  : 王明松 2006-6-23 19:49:09
	/// 返回  : bool 
	/// 描述  : 删除当前节点，当前节点此节点父节点
    bool deleteItem();

	/// 函数名: deleteNode
	/// 编程  : 王明松 2006-6-23 19:49:36
	/// 返回  : bool 
	/// 参数  : CNode *pnode
	/// 描述  : 删除指定节点及下属节点
    bool deleteNode(CNode *pnode);

	/// 函数名: setAttribute
	/// 编程  : 王明松 2006-6-23 19:49:52
	/// 返回  : bool 
	/// 参数  : const string name
	/// 参数  : const string value
	/// 描述  : 设置当前节点的属性值
    bool setAttribute(const std::string name,const std::string value);

	/// 函数名: getAttribute
	/// 编程  : 王明松 2006-6-23 19:50:00
	/// 返回  : bool 
	/// 参数  : const string name
	/// 参数  : string &value
	/// 描述  : 获取当前节点的属性值
    bool getAttribute(const std::string name,std::string &value);

	/// 函数名: *getAttributes
	/// 编程  : 王明松 2006-6-23 19:50:09
	/// 返回  : CAttribute 
	/// 描述  : 获取当前节点的属性
	CAttribute *getAttributes();

	/// 函数名: delAttribute
	/// 编程  : 王明松 2006-6-23 19:50:19
	/// 返回  : bool 
	/// 参数  : const string name
	/// 描述  : 删除当前节点的属性
    bool delAttribute(const std::string name);

	/// 函数名: delAttribute
	/// 编程  : 王明松 2006-6-23 19:50:28
	/// 返回  : bool 
	/// 参数  : CNode *pNode
	/// 描述  : 删除指定节点的属性
	bool delAttribute(CNode *pNode);
 
	/// 函数名: *getParentItem
	/// 编程  : 王明松 2006-6-23 19:42:59
	/// 返回  : CNode 
	/// 描述  : 获得父亲节点,当前节点改变
	CNode *getParentItem();

	/// 函数名: *getCurrentNode
	/// 编程  : 王明松 2006-6-23 19:42:47
	/// 返回  : CNode 
	/// 描述  : 获得当前节点
	CNode *getCurrentNode();


	/// 函数名: *setCurrentNode
	/// 编程  : 王明松 2006-6-23 19:42:28
	/// 返回  : CNode 
	/// 参数  : CNode *pNode
	/// 描述  : 设置当前节点
	CNode *setCurrentNode(CNode *pNode);

	/// 函数名: getNodeName
	/// 编程  : 王明松 2006-6-23 19:53:28
	/// 返回  : string 
	/// 描述  : 获得当前节点名
	std::string getNodeName();

	/// 函数名: getNodeValue
	/// 编程  : 王明松 2006-6-23 19:53:32
	/// 返回  : string 
	/// 描述  : 获得当前节点值
	std::string getNodeValue();


	/// 函数名: toBufferOld
	/// 编程  : 王明松 2008-11-25 16:10:11
	/// 返回  : bool 
	/// 参数  : char *buffer
	/// 描述  : 没有</>的形式的xml数据
	bool toBufferOld(char *buffer);

	/// 函数名: nodeToBufferOld
	/// 编程  : 王明松 2008-11-25 16:18:21
	/// 返回  : int 
	/// 参数  : CNode *pnode
	/// 参数  : char *buffer
	/// 参数  : int &xmldatalen
	/// 参数  : int tier
	/// 描述  : 没有</>的形式的xml数据
	int nodeToBufferOld(CNode *pnode,char *buffer,int &xmldatalen,int tier);

	//定位当前节点到根节点
	CNode *LocateRoot();

	//根据节点名字定位到该层的第一个节点
	CNode *LocateFirstByName(char *pString);

	
	char m_errMsg[1025];
private:
	char m_buf[1000];
	char *GetCol(char *src, int index);
	CNode *CompLayerNode(CNode *pFirst,const std::string item);
};

#undef XML_EXPORT
#endif /* CXML_H_HEADER_INCLUDED_C123C05E */
