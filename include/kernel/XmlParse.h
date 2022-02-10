// XmlParse.h: interface for the CXmlParse class.
//
//////////////////////////////////////////////////////////////////////

/***********************************************************************************
  编码日志记录
  编码者: 王明松
  编码日期 2009-4-26 9:51:10
  版本: V1.00 
  说明: XML解析类
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
	
	// 函数名: ParseXml
	// 编程  : 王明松 2010-6-3 10:01:26
	// 返回  : bool 
	// 参数  : std::string xmlbuf
	// 参数  : CIXmlNode *node
	// 参数  : int layer=0   0为DOM模式 >0为SAX模式
	// 描述  : 解析XML至父节点
	bool ParseXml(std::string xmlbuf,CIXmlNode *parentNode,int layer=0);

	CXmlParse();
	virtual ~CXmlParse();

protected:


	// 函数名: ParseNodeNameAttrDom
	// 编程  : 王明松 2010-7-1 11:26:47
	// 返回  : CIXmlNode * 
	// 参数  : CIXmlNode *parentnode
	// 参数  : char *nameattr
	// 参数  : bool isEndNode
	// 描述  : 解析节点名及属性
	CIXmlNode * ParseNodeNameAttrDom(CIXmlNode *parentnode,char *nameattr,bool isEndNode);
	

	// 函数名: FindEndNodeDom
	// 编程  : 王明松 2010-7-1 11:27:05
	// 返回  : bool 
	// 参数  : int bOffset
	// 参数  : int eOffset
	// 参数  : std::string nodename
	// 参数  : int &vend
	// 参数  : int &nextnodebegin
	// 描述  : 查找节点的结束位置
	bool FindEndNodeDom(int bOffset,int eOffset,std::string nodename,int &vend,int &nextnodebegin);


	// 函数名: ParseXmlSax
	// 编程  : 王明松 2010-7-1 11:27:23
	// 返回  : bool 
	// 参数  : std::string xmlbuf
	// 参数  : CIXmlNode *parentNode
	// 参数  : int layer=1
	// 描述  : 按SAX模式解析XML
	bool ParseXmlSax(std::string xmlbuf,CIXmlNode *parentNode,int layer=1);

	// 函数名: ParseXmlDom
	// 编程  : 王明松 2010-7-1 11:27:38
	// 返回  : bool 
	// 参数  : std::string xmlbuf
	// 参数  : CIXmlNode *parentNode
	// 描述  : 按DOM模式解析XML
	bool ParseXmlDom(std::string xmlbuf,CIXmlNode *parentNode);

	// 函数名: ParseXmlDom
	// 编程  : 王明松 2010-7-1 11:27:56
	// 返回  : bool 
	// 参数  : int beginoffset
	// 参数  : int endoffset
	// 参数  : CIXmlNode *parentNode
	// 描述  : 按DOM模式解析XML的递归方法
	bool ParseXmlDom(int beginoffset, int endoffset, CIXmlNode *parentNode);

	bool ParseXmlDom2(int &beginoffset, int endoffset, CIXmlNode *parentNode);

	// 函数名: GetNodeName
	// 编程  : 王明松 2010-7-1 11:28:22
	// 返回  : bool 
	// 参数  : std::string nameattr
	// 参数  : std::string &nodename
	// 描述  : 取得节点名称
	bool GetNodeName(std::string nameattr,std::string &nodename);

	// 函数名: ParseNodeAttribute
	// 编程  : 王明松 2010-6-12 16:06:45
	// 返回  : int -1属性非法
	// 参数  : std::string sattr  属性字符串
	// 参数  : CIXmlNode *addnode  节点
	// 描述  : 解析属性
	int ParseNodeAttribute(std::string sattr,CIXmlNode *addnode);


	// 函数名: FindEndNode
	// 编程  : 王明松 2010-6-12 16:03:20
	// 返回  : bool true找到了
	// 参数  : string xmlbuf  xml字符串
	// 参数  : int bOffset  字符串开始位置
	// 参数  : std::string nodename   节点名称
	// 参数  : int &vend         节点值的结束位置
	// 参数  : int &nextnodebegin         节点值的结束位置
	// 描述  : 通过传入的字符串及开始位置，查找对应的节点结束位置
	bool FindEndNode(std::string xmlbuf,int bOffset,std::string nodename,int &vend,int &nextnodebegin);


	// 函数名: ParseNodeNameAttr
	// 编程  : 王明松 2010-6-12 16:01:16
	// 返回  : CIXmlNode *   增加的节点
	// 参数  : CIXmlNode *parentnode  父节点
	// 参数  : std::string nameattr  名称属性字符串(不包括<>),即<xx>和<xx/>中的xx
	// 参数  : bool isEndNode        是否是/>结束
	// 描述  : 解析节点名称及属性，包括版本注释等
	CIXmlNode * ParseNodeNameAttr(CIXmlNode *parentnode,std::string nameattr,bool isEndNode);
	
	/// 函数名: LogMp
	/// 编程  : 王明松 2009-1-24 8:20:56
	/// 返回  : int 
	/// 参数  : int level
	/// 参数  : const char *filename
	/// 参数  : int line
	/// 参数  : const char *fmt
	/// 参数  : ...
	/// 描述  : 写日志
	int LogMp(int level, const char *filename, int line,const char *data,int datalen,const char *fmt,...);

	void GetName(char *filename);
	char m_errMsg[256];

	char *m_sXmlBuf;
	int   m_nXmlBufLen;

	int            m_nLogLevel; //日志级别
	int            m_nId;
	std::string    m_sLogName;
	CBF_Date_Time  m_pDate;
	std::string    m_sCurDate;//当前日期
};
#undef  KERNEL_EXPORT
#endif // !defined(AFX_XMLPARSE_H__5EAB1EF5_B256_4EF3_8992_D9B75BFBFEC9__INCLUDED_)

