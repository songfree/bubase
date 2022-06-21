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

	// 函数名: CreateIndex
	// 编程  : 王明松 2010-7-6 10:22:23
	// 返回  : void 
	// 描述  : 建索引，在xml不变且大量查找时可建
	void CreateIndex();

	// 函数名: AddNodeByPath
	// 编程  : 王明松 2010-7-6 10:22:23
	// 返回  : CXmlNode * 
	// 参数  : const char *xmlpathname /xx/yy/或/xx/yy或xx/yy/或xx/yy的格式
	// 参数  : bool isTransSign   xmlpathname是否需转义符转换
	// 参数  : const char *nodename    节点名称
	// 参数  : bool isTransName        节点名称是否需转义符转换
	// 参数  : const char *nodevalue   节点值
	// 参数  : bool isTransValue=true  节点值是否需转义符转换
	// 描述  : 增加一个新节点，若xmlpathname不唯一返回NULL
	CXmlNode * AddNodeByPath(const char *xmlpathname,bool isTransSign,const char *nodename,bool isTransName,const char *nodevalue,bool isTransValue=true);
	CXmlNode * AddNodeByPath(const char *xmlpathname,bool isTransSign,const char *nodename,bool isTransName,int nodevalue);
	CXmlNode * AddNodeByPath(const char *xmlpathname,bool isTransSign,const char *nodename,bool isTransName,long nodevalue);
	CXmlNode * AddNodeByPath(const char *xmlpathname,bool isTransSign,const char *nodename,bool isTransName,double nodevalue,int dec=2);

	// 函数名: GetNodeAttribute
	// 编程  : 王明松 2010-6-29 9:16:04
	// 返回  : CXmlAttribute * 返回属性指针,若有多个节点返回NULL
	// 参数  : const char *xmlpathname  /xx/yy/或/xx/yy或xx/yy/或xx/yy的格式
	// 参数  : bool isTransSign         xmlpathname是否需转义符转换
	// 参数  : const char *attrname     属性名称
	// 参数  : bool isTransAttrName     属性名称是否需转义符转换
	// 参数  : string &value            属性值
	// 参数  : bool isTransValue=true   属性值是否需转义符转换
	// 描述  : 取得属性值
	CXmlAttribute * GetNodeAttribute(const char *xmlpathname,bool isTransSign,const char *attrname,bool isTransAttrName,std::string &value,bool isTransValue=true);
	
	// 函数名: GetNodeAttribute
	// 编程  : 王明松 2010-6-29 9:16:04
	// 返回  : CXmlAttribute * 返回属性指针,若有多个节点返回NULL
	// 参数  : const char *xmlpathname  /xx/yy/或/xx/yy或xx/yy/或xx/yy的格式
	// 参数  : bool isTransSign         xmlpathname是否需转义符转换
	// 参数  : const char *attrname     属性名称
	// 参数  : bool isTransAttrName     属性名称是否需转义符转换
	// 参数  : int &value               属性值
	// 描述  : 取得属性值
	CXmlAttribute * GetNodeAttribute(const char *xmlpathname,bool isTransSign,const char *attrname,bool isTransAttrName,int &value);

	CXmlAttribute * GetNodeAttribute(const char *xmlpathname,bool isTransSign,const char *attrname,bool isTransAttrName,long &value);
	
	// 函数名: GetNodeAttribute
	// 编程  : 王明松 2010-6-29 9:16:04
	// 返回  : CXmlAttribute * 返回属性指针,若有多个节点返回NULL
	// 参数  : const char *xmlpathname  /xx/yy/或/xx/yy或xx/yy/或xx/yy的格式
	// 参数  : bool isTransSign         xmlpathname是否需转义符转换
	// 参数  : const char *attrname     属性名称
	// 参数  : bool isTransAttrName     属性名称是否需转义符转换
	// 参数  : double &value               属性值
	// 描述  : 取得属性值
	CXmlAttribute * GetNodeAttribute(const char *xmlpathname,bool isTransSign,const char *attrname,bool isTransAttrName,double &value);
	
	// 函数名: SetNodeAttribute
	// 编程  : 王明松 2010-6-29 9:14:11
	// 返回  : CXmlAttribute *   返回属性指针,若有多个节点返回NULL
	// 参数  : const char *xmlpathname /xx/yy/或/xx/yy或xx/yy/或xx/yy的格式
	// 参数  : bool isTransSign   xmlpathname是否需转义符转换
	// 参数  : const char *attrname   属性名称
	// 参数  : bool isTransAttrName    属性名称是否需转义符转换 
	// 参数  : const char *value       属性值
	// 参数  : bool isTransValue=true  属性值是否需转义符转换
	// 描述  : 设置节点属性，若无节点则增加新节点
	CXmlAttribute * SetNodeAttribute(const char *xmlpathname,bool isTransSign,const char *attrname,bool isTransAttrName,const char *value,bool isTransValue=true);
	
	// 函数名: SetNodeAttribute
	// 编程  : 王明松 2010-6-29 9:14:11
	// 返回  : CXmlAttribute *   返回属性指针,若有多个节点返回NULL
	// 参数  : const char *xmlpathname /xx/yy/或/xx/yy或xx/yy/或xx/yy的格式
	// 参数  : bool isTransSign   xmlpathname是否需转义符转换
	// 参数  : const char *attrname   属性名称
	// 参数  : bool isTransAttrName    属性名称是否需转义符转换 
	// 参数  : int value       属性值
	// 描述  : 设置节点属性，若无节点则增加新节点
	CXmlAttribute * SetNodeAttribute(const char *xmlpathname,bool isTransSign,const char *attrname,bool isTransAttrName,int value);

	CXmlAttribute * SetNodeAttribute(const char *xmlpathname,bool isTransSign,const char *attrname,bool isTransAttrName,long value);
	
	// 函数名: SetNodeAttribute
	// 编程  : 王明松 2010-6-29 9:14:11
	// 返回  : CXmlAttribute *   返回属性指针,若有多个节点返回NULL
	// 参数  : const char *xmlpathname /xx/yy/或/xx/yy或xx/yy/或xx/yy的格式
	// 参数  : bool isTransSign   xmlpathname是否需转义符转换
	// 参数  : const char *attrname   属性名称
	// 参数  : bool isTransAttrName    属性名称是否需转义符转换 
	// 参数  : double value       属性值
	// 参数  : intdouble dec       小数位数
	// 描述  : 设置节点属性，若无节点则增加新节点
	CXmlAttribute * SetNodeAttribute(const char *xmlpathname,bool isTransSign,const char *attrname,bool isTransAttrName,double value,int dec=2);

	// 函数名: ToBuffer
	// 编程  : 王明松 2010-6-14 11:22:14
	// 返回  : bool 
	// 参数  : char *buffer
	// 描述  : 生成XML数据，注意缓冲要够大，可以先调GetXmlDataLen来获知大小
	bool ToBuffer(char *buffer);
	
	// 函数名: SetNodeValueByPath
	// 编程  : 王明松 2010-6-23 15:11:46
	// 返回  : CXmlNode * 若节点有多个返回NULL
	// 参数  : const char *xmlpathname /xx/yy/或/xx/yy或xx/yy/或xx/yy的格式
	// 参数  : bool isTransSign xmlpathname是否需转义符转换
	// 参数  : double *value  节点值
	// 参数  : int dec=2 节点值的小数位数
	// 描述  : 设置节点值，节点不存在则新增
	CXmlNode * SetNodeValueByPath(const char *xmlpathname, bool isTransSign,double value,int dec=2);

	// 函数名: SetNodeValueByPath
	// 编程  : 王明松 2010-6-23 15:11:46
	// 返回  : CXmlNode * 若节点有多个返回NULL
	// 参数  : const char *xmlpathname /xx/yy/或/xx/yy或xx/yy/或xx/yy的格式
	// 参数  : bool isTransSign xmlpathname是否需转义符转换
	// 参数  : const char *value  节点值
	// 参数  : bool isTransValue=true 节点值是否需转义符转换
	// 描述  : 设置节点值，节点不存在则新增
	CXmlNode * SetNodeValueByPath(const char *xmlpathname, bool isTransSign,int value);

	CXmlNode * SetNodeValueByPath(const char *xmlpathname, bool isTransSign,long value);

	// 函数名: SetNodeValueByPath
	// 编程  : 王明松 2010-6-23 15:11:46
	// 返回  : CXmlNode * 若节点有多个返回NULL
	// 参数  : const char *xmlpathname /xx/yy/或/xx/yy或xx/yy/或xx/yy的格式
	// 参数  : bool isTransSign xmlpathname是否需转义符转换
	// 参数  : const char *value  节点值
	// 参数  : bool isTransValue=true 节点值是否需转义符转换
	// 描述  : 设置节点值，节点不存在则新增
	CXmlNode * SetNodeValueByPath(const char *xmlpathname,bool isTransSign,const char *value,bool isTransValue=true);

	// 函数名: GetNodeValueByPath
	// 编程  : 王明松 2010-6-23 15:09:31
	// 返回  : CXmlNode * 若节点有多个或不存在返回NULL
	// 参数  : const char *xmlpathname  /xx/yy/或/xx/yy或xx/yy/或xx/yy的格式
	// 参数  : bool isTransSign   xmlpathname是否需转义符转换
	// 参数  : double &value    节点值
	// 描述  : 取得节点值，若有多个节点返回false
	CXmlNode * GetNodeValueByPath(const char *xmlpathname,bool isTransSign,double &value);

	// 函数名: GetNodeValueByPath
	// 编程  : 王明松 2010-6-23 15:09:31
	// 返回  : CXmlNode * 若节点有多个或不存在返回NULL
	// 参数  : const char *xmlpathname  /xx/yy/或/xx/yy或xx/yy/或xx/yy的格式
	// 参数  : bool isTransSign   xmlpathname是否需转义符转换
	// 参数  : int &value    节点值
	// 描述  : 取得节点值，若有多个节点返回false
	CXmlNode * GetNodeValueByPath(const char *xmlpathname,bool isTransSign,int &value);

	CXmlNode * GetNodeValueByPath(const char *xmlpathname,bool isTransSign,long &value);

	// 函数名: GetNodeValueByPath
	// 编程  : 王明松 2010-6-23 15:09:31
	// 返回  : CXmlNode * 若节点有多个或不存在返回NULL
	// 参数  : const char *xmlpathname  /xx/yy/或/xx/yy或xx/yy/或xx/yy的格式
	// 参数  : bool isTransSign   xmlpathname是否需转义符转换
	// 参数  : std::string &value    节点值
	// 参数  : bool isTransValue=true   节点值是否需转义符转换
	// 描述  : 取得节点值，若有多个节点返回false
	CXmlNode * GetNodeValueByPath(const char *xmlpathname,bool isTransSign,std::string &value,bool isTransValue=true);

	// 函数名: GetRoot
	// 编程  : 王明松 2010-6-23 9:10:24
	// 返回  : CXmlNode * 
	// 描述  : 取得根节点，注意根节点无名称及值。它的下属节点才是实际XML内容
	CXmlNode * GetRoot();

	// 函数名: XmlCopy
	// 编程  : 王明松 2010-6-23 9:11:05
	// 返回  : bool 
	// 参数  : CBFXml *srcxml
	// 描述  : XML复制，将srcxml的内容完全复制到本XML类中
	bool XmlCopy(CBF_Xml *srcxml);

	// 函数名: GetNodeByPath
	// 编程  : 王明松 2010-6-22 15:40:36
	// 返回  : CXmlNode * 不存在或多个返回NULL
	// 参数  : const char *xmlpathname 可以为/xx/yy/或/xx/yy或xx/yy/或xx/yy的格式
	// 参数  : bool isTransSign=true
	// 描述  : 根据节点路径查找节点,若有多个则返回NULL
	CXmlNode * GetNodeByPath(const char *xmlpathname,bool isTransSign=true);

	// 函数名: DeleteAllNode
	// 编程  : 王明松 2010-6-15 13:30:44
	// 返回  : void 
	// 描述  : 删除所有节点
	void DeleteAllNode();
	

	// 函数名: GetXmlDataLen
	// 编程  : 王明松 2010-6-14 11:22:00
	// 返回  : int 
	// 描述  : 取得XML的数据长度
	int GetXmlDataLen();

	// 函数名: ToBuffer
	// 编程  : 王明松 2010-6-14 11:22:14
	// 返回  : bool 
	// 参数  : char *buffer
	// 参数  : int &buflen  传入buffer的缓冲大小，传出xml数据长度
	// 描述  : 生成XML数据，若缓冲不够大则出错
	bool ToBuffer(char *buffer,int &buflen);

	// 函数名: ToFile
	// 编程  : 王明松 2010-6-14 11:23:03
	// 返回  : bool 
	// 参数  : const char *filename
	// 描述  : 生成XML数据至文件
	bool ToFile(const char *filename);

	// 函数名: ToString
	// 编程  : 王明松 2010-6-14 11:23:18
	// 返回  : bool 
	// 参数  : string &xmlstring
	// 描述  : 生成XML
	bool ToString(std::string &xmlstring);


	// 函数名: FromFile
	// 编程  : 王明松 2010-6-14 11:26:54
	// 返回  : bool 
	// 参数  : const char *xmlfile
	// 参数  : int layer=0  0为DOM解析 大于0为SAX解析，为解析层次
	// 描述  : 从文件中解析XML
	bool FromFile(const char *xmlfile,int layer=0);

	// 函数名: FromBuffer
	// 编程  : 王明松 2010-6-14 11:27:53
	// 返回  : bool 
	// 参数  : const char *xmlbuf
	// 参数  : int layer=0  0为DOM解析 大于0为SAX解析，为解析层次
	// 描述  : 从数据缓冲解析XML,xmlbuf要有结束符0
	bool FromBuffer(const char *xmlbuf,int layer=0);


	CBF_Xml();
	virtual ~CBF_Xml();

protected:

	// 函数名: GetNodeName
	// 编程  : 王明松 2010-6-22 15:42:32
	// 返回  : int 
	// 参数  : std::string xmlpathname  节点路径可以为/xx/yy/或/xx/yy或xx/yy/或xx/yy的格式
	// 参数  : vector<std::string> &namelist
	// 描述  : 取得节点名称列表
	int GetNodeName(std::string xmlpathname, std::vector<std::string> *namelist);

	CXmlNode m_pRootNode; //根节点，固定为空

	CXmlParse m_parseXml;  //解析类
};
#undef  BFXML_EXPORT
#endif // !defined(AFX_BFXML_H__50F338A7_6795_462B_8771_84225CFCEBB1__INCLUDED_)
