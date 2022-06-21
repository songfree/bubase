// XmlNode.h: interface for the CXmlNode class.
//
//////////////////////////////////////////////////////////////////////

/***********************************************************************************
  编码日志记录
  编码者: 王明松
  编码日期 2009-4-26 9:49:37
  版本: V1.00 
  说明: XML节点类，XML就是由多个节点组成，每个节点有节点属性和节点值组成
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
	// 函数名: * NewNode
	// 编程  : 王明松 2010-6-15 13:23:37
	// 返回  : virtual CIXmlNode 
	// 描述  : 生成一个新的节点
	virtual CIXmlNode * NewNode();	

    // 函数名: SetAttribute
	// 编程  : 王明松 2010-6-8 10:36:50
	// 返回  : CXmlAttribute * 
	// 参数  : string attrname  属性名称
	// 参数  : bool isTransSign  名称是否进行转义符转换
	// 参数  : string attrvlaue   属性值
	// 参数  : bool isTransValue=true  属性值是否进行转义符转换
	// 描述  : 增加一个属性 若已有此名称的属性则设置值返回，否则新增并加入索引返回
	virtual CXmlAttribute * SetAttribute(std::string attrname,bool isTransSign,std::string attrvlaue,bool isTransValue=true);

	virtual CXmlAttribute * SetAttribute(std::string attrname,bool isTransSign,int attrvlaue);

	virtual CXmlAttribute * SetAttribute(std::string attrname,bool isTransSign,long attrvlaue);

	virtual CXmlAttribute * SetAttribute(std::string attrname,bool isTransSign,double attrvlaue,int dec=2);


	// 函数名: * AddAttribute
	// 编程  : 王明松 2010-7-16 15:28:05
	// 返回  : virtual CXmlAttribute 
	// 参数  : std::string attrname
	// 参数  : bool isTransSign
	// 参数  : std::string attrvlaue
	// 参数  : bool isTransValue=true
	// 描述  : 增加一个属值，不做判断怎是增加
	virtual CXmlAttribute * AddAttribute(std::string attrname,bool isTransSign,std::string attrvlaue,bool isTransValue=true);

	// 函数名: GetParentNode
	// 编程  : 王明松 2010-6-8 16:22:14
	// 返回  : CXmlNode * 
	// 描述  : 取得父节点
	virtual CIXmlNode * GetParentNode();

	// 函数名: getNextSibling
	// 编程  : 王明松 2010-6-8 16:22:26
	// 返回  : CXmlNode * 
	// 描述  : 取得临近下节点
	virtual CIXmlNode * getNextSibling();

	// 函数名: getLastSibling
	// 编程  : 王明松 2010-6-8 16:22:37
	// 返回  : CXmlNode * 
	// 描述  : 取得临近上节点
	virtual CIXmlNode * getLastSibling();

	// 函数名: GetEndChild
	// 编程  : 王明松 2010-6-8 16:22:49
	// 返回  : CXmlNode * 
	// 描述  : 取得最后一个子节点
	virtual CIXmlNode * GetEndChild();

	// 函数名: GetFirstChild
	// 编程  : 王明松 2010-6-8 16:23:00
	// 返回  : CXmlNode * 
	// 描述  : 取得最前一个子节点
	virtual CIXmlNode * GetFirstChild();

	// 函数名: GetNodeType
	// 编程  : 王明松 2010-6-4 16:39:31
	// 返回  : int 
	// 描述  : 取得节点类型
	virtual int GetNodeType();

	// 函数名: SetNodeType
	// 编程  : 王明松 2010-6-4 16:39:17
	// 返回  : void 
	// 参数  : int type
	// 描述  : 设置节点类型
	virtual void SetNodeType(int type);

	// 函数名: GetNodeValue
	// 编程  : 王明松 2010-6-4 15:57:32
	// 返回  : std::string 
	// 参数  : bool isTransSign=true true进行转义符转换 false不转
	// 描述  : 取得节点值
	virtual std::string GetNodeValue(bool isTransSign=true);

	// 函数名: GetNodeName
	// 编程  : 王明松 2010-6-4 15:57:41
	// 返回  : std::string 
	// 参数  : bool isTransSign=true  true进行转义符转换 false不转
	// 描述  : 取得节点名称
	virtual std::string GetNodeName(bool isTransSign=true);


	// 函数名: SetNodeValue
	// 编程  : 王明松 2010-6-9 12:01:05
	// 返回  : void 
	// 参数  : std::string value
	// 参数  : bool isTransSign=true
	// 描述  : 设置节点值，若节点类型为NODETYPE_ATTR且value长度大于1则类型置为NODETYPE_VALUE
	virtual void SetNodeValue(std::string value,bool isTransSign=true);

	virtual void SetNodeValue(int value);

	virtual void SetNodeValue(long value);
	
	virtual void SetNodeValue(double value,int dec=2);

	// 函数名: SetNodeName
	// 编程  : 王明松 2010-6-9 12:01:18
	// 返回  : void 
	// 参数  : std::string nodename
	// 参数  : bool isTransSign=true
	// 描述  : 设置节点名称，注意如果节点已经加入XML链表，则不可以再重设名称，因为索引要重新设置
	virtual void SetNodeName(std::string nodename,bool isTransSign=true);

	// 函数名: ParseChildNode
	// 编程  : 王明松 2010-6-2 16:37:04
	// 返回  : bool 
	// 参数  : int lay=0   子节点的层次，0全部解开，>0解开层次  1只解开本节点的子节点 
	// 描述  : 解析子节点
	virtual bool ParseChildNode(int layer=0);

	// 函数名: AddChildNode
	// 编程  : 王明松 2010-6-4 15:14:22
	// 返回  : int   返回此节点的子节点数
	// 参数  : CIXmlNode *anode    要加入的节点 注意节点名称要有，否则重新设置索引不符
	// 参数  : bool first=false   true增加到前面，false增加到最后
	// 描述  : 增加一个子节点,注意节点anode的节点类型及节点名称要在之前设置好
	virtual int AddChildNode(CIXmlNode *anode,bool first=false);

	// 函数名: SetParseClass
	// 编程  : 王明松 2010-6-17 13:35:25
	// 返回  : virtual void 
	// 参数  : void *parse
	// 描述  : 设置解析类指针
	virtual void SetParseClass(void *parse);

	// 函数名: SetParseChilded
	// 编程  : 王明松 2010-6-22 11:56:42
	// 返回  : virtual void 
	// 描述  : 设置此节点已解析下节点
	virtual void SetParseChilded();
	

public:

	// 函数名: CreateNodeAttrIndex
	// 编程  : 王明松 2010-7-16 16:07:47
	// 返回  : void 
	// 描述  : 建立节点的所有索引 
	void CreateNodeAttrIndex();

	bool GetNodeExpandFlag();
	bool GetNodeIndexFlag();
	
	// 函数名: NodeCopy
	// 编程  : 王明松 2010-6-22 17:11:28
	// 返回  : bool 
	// 参数  : CXmlNode *srcnode  源节点
	// 描述  : 将源XML中的节点及下属节点复制到本节点中，替换本节点的名称及值(先删除本节点下属节点及属性)
	bool NodeCopy(CXmlNode *srcnode);

	// 函数名: GetEndAttribute
	// 编程  : 王明松 2010-6-12 15:16:34
	// 返回  : CXmlAttribute * 
	// 描述  : 取得最后一个属性
	CXmlAttribute * GetEndAttribute();
	
	// 函数名: GetFirstAttribute
	// 编程  : 王明松 2010-6-12 15:16:38
	// 返回  : CXmlAttribute * 
	// 描述  : 取得第一个属性
	CXmlAttribute * GetFirstAttribute();
	
	
	// 函数名: DeleteAttribute
	// 编程  : 王明松 2010-6-8 11:10:41
	// 返回  : bool 
	// 参数  : string attrname
	// 参数  : bool isTransSign=true
	// 描述  : 删除属性及属性索引
	bool DeleteAttribute(std::string attrname,bool isTransSign=true);
	
	// 函数名: ClearAllAttribute
	// 编程  : 王明松 2010-6-8 11:09:56
	// 返回  : void 
	// 描述  : 清除所有属性，包括索引
	void ClearAllAttribute();

	// 函数名: GetAttribute
	// 编程  : 王明松 2010-6-8 11:04:49
	// 返回  : CXmlAttribute * 若找到则返回属性，否则返回NULL
	// 参数  : string attrname  属性名称
	// 参数  : bool isTransSign 名称是否进行转义符转换
	// 参数  : string &attrvalue 属性值
	// 参数  : bool isTransValue=true 属性值是否进行转义符转换
	// 描述  : 根据属性名称查找属性值 
	CXmlAttribute * GetAttribute(std::string attrname,bool isTransSign,std::string &attrvalue,bool isTransValue=true);
	
	// 函数名: GetAttribute
	// 编程  : 王明松 2010-6-8 11:04:46
	// 返回  : CXmlAttribute * 若找到则返回属性，否则返回NULL
	// 参数  : std::string attrname 属性名称
	// 参数  : bool isTransSign=true 名称是否进行转义符转换
	// 描述  : 根据属性名称查找属性
	CXmlAttribute * GetAttribute(std::string attrname,bool isTransSign=true);

	

	// 函数名: ToString
	// 编程  : 王明松 2010-6-15 13:43:08
	// 返回  : int 
	// 参数  : std::string &xmlstring
	// 参数  : int layer=0
	// 描述  : 生成XML
	int ToString(std::string &xmlstring,int layer=0);
	
	// 函数名: GetNodeDataLen
	// 编程  : 王明松 2010-6-11 9:57:53
	// 返回  : int xml大小
	// 参数  : int &xmldatalen  xml大小
	// 参数  : int layer 节点层次，好在前面预留空格以对齐
	// 描述  : 取得XML数据的大小
	int GetNodeDataLen(int &xmldatalen,int layer);
	
	// 函数名: NodeToBuffer
	// 编程  : 王明松 2010-6-11 9:57:47
	// 返回  : int 
	// 参数  : char *buffer    数据缓冲，要足够大
	// 参数  : int &xmldatalen     数据缓冲已用大小
	// 参数  : int layer   节点层次，好在前面预留空格以对齐
	// 描述  : 将节点等生成数据，注意数据缓冲大小要足够，可调用GetNodeDataLen来知道大小来分配空间
	int NodeToBuffer(char *buffer,int &xmldatalen,int layer);

	// 函数名: NodeToFile
	// 编程  : 王明松 2010-6-11 9:57:47
	// 返回  : int 
	// 参数  : FILE *fp    已打开的文件指针
	// 参数  : int layer   节点层次，好在前面预留空格以对齐
	// 描述  : 将节点等生成数据存入文件
	int NodeToFile(FILE *fp,int layer);
	
	// 函数名: ReCreateIndex
	// 编程  : 王明松 2010-6-10 15:19:45
	// 返回  : void 
	// 描述  : 重建当前节点在父节点中的索引
	void ReCreateIndex();
	
	// 函数名: GetChildNode
	// 编程  : 王明松 2010-6-10 15:16:34
	// 返回  : int                满足条件的子节点数目
	// 参数  : string nodename    节点名称
	// 参数  : bool isTransSign   名称是否转义符转换
	// 参数  : vector<CXmlNode *> &nodelist  满足条件的子节点列表
	// 描述  : 根据名称查找子节点
	int GetChildNode(std::string nodename,bool isTransSign, std::vector<CXmlNode *> &nodelist);
	
	// 函数名: DeleteAllChild
	// 编程  : 王明松 2010-6-9 11:48:52
	// 返回  : void 
	// 描述  : 删除所有的子节点
	void DeleteAllChild();
	
	// 函数名: DeleteChildNode
	// 编程  : 王明松 2010-6-8 16:42:27
	// 返回  : bool 
	// 参数  : std::string nodename  子节点名称
	// 参数  : bool isTransSign=true  子节点名称是否转义符转换
	// 描述  : 根据节点名称删除子节点,有多个删多个
	bool DeleteChildNode(std::string nodename,bool isTransSign=true);
	
	// 函数名: DeletePoint
	// 编程  : 王明松 2010-6-4 16:47:52
	// 返回  : void 
	// 描述  : 将本节点从xml链表中去除，然后就可以delete了
	void DeletePoint();
	
	// 函数名: DeleteIndex
	// 编程  : 王明松 2010-6-8 16:25:17
	// 返回  : bool 
	// 参数  : CXmlNode *node
	// 描述  : 在本节点中删除子节点为node的索引，注意不是根据节点名来删除，是根据子节点的ID来删除
	bool DeleteIndex(CXmlNode *node);
	
	// 函数名: CreateIndex
	// 编程  : 王明松 2010-6-10 10:04:09
	// 返回  : bool fasel时childnode不为此节点的子节点或子节点的节点类型不符
	// 参数  : CXmlNode *childnode
	// 描述  : 建立索子节点childndoe的索引
	bool CreateIndex(CXmlNode *childnode);
	
	// 函数名: GetParentIndex
	// 编程  : 王明松 2010-6-4 15:47:54
	// 返回  : int 
	// 描述  : 取得本节点在父节点中的索引序号
	int GetParentIndex();
	
	// 函数名: SetParentIndex
	// 编程  : 王明松 2010-6-4 15:12:56
	// 返回  : void 
	// 参数  : int id  即父节点的索引序号，父节点按此进行唯一索引
	// 描述  : 设置此节点在父节点的索引序号，在解析xml时调用。使用者禁止使用
	void SetParentIndex(int id);
	
	// 函数名: AddChildNode
	// 编程  : 王明松 2010-6-10 9:58:31
	// 返回  : CXmlNode * 
	// 参数  : std::string nodename
	// 参数  : bool isTransSign=true
	// 描述  : 增加节点名为nodename的节点，节点类型为NODETYPE_ATTR
	CXmlNode * AddChildNode(std::string nodename,bool isTransSign=true);


	// 函数名: *GetChildNode
	// 编程  : 王明松 2017-9-20 14:59:32
	// 返回  : CXmlNode 
	// 参数  : std::string nodename
	// 参数  : bool isTransSign
	// 描述  : 取得指定名称的第一个子节点
	CXmlNode *GetChildNode(std::string nodename, bool isTransSign);
	
	CXmlNode();
	virtual ~CXmlNode();

	CXmlAttributes m_attribute;///节点属性

protected:

	// 函数名: ExpandValue
	// 编程  : 王明松 2010-6-4 14:23:18
	// 返回  : bool 需展开返回true
	// 描述  : 判断此节点的下属节点是否展开，若未展开则展开下属节点(仅限展开下属节点)
	bool ExpandValue();

	// 函数名: DeleteParentIndex
	// 编程  : 王明松 2010-6-4 16:57:40
	// 返回  : bool 
	// 描述  : 去掉本节点在父节点里的索引，即在父节点时按索引查找不到了
	bool DeleteParentIndex();
	
protected:
	
	std::string m_sNodeValue;  //节点值   <xx>m_sNodeName</xx>  保存的是xml文件中的值
	std::string m_sNodeName;   //节点名称

	bool m_bIsExpand; //子节点是否展开
	int m_nNodeType;  //节点类型

	int m_nParentIndex;   //本节点在父节点中的索引记数
	int m_nChildIndex;    //子节点索引记数
	int m_nChildNodeNum; //子节点数

	
	CXmlNodeIndex  *m_index;   //节点索引
	
	CIXmlParse *m_parseXml;
	
    CXmlNode *m_parentNode;///父节点指针
	
    CXmlNode *m_nextNode; ///临近下节点指针
	CXmlNode *m_lastNode; ///临近上节点指针
	
    CXmlNode *m_firstChildNode;  ///第一个子节点指针
	CXmlNode *m_endChildNode;   ///最后一个子节点指针

	bool      m_bIsUseIndex;    //是否使用索引

};
#undef  KERNEL_EXPORT
#endif // !defined(AFX_XMLNODE_H__FE9DD9EF_3AB1_403E_80C0_CB478F6B8B1F__INCLUDED_)
