/// PackConvert.h: interface for the CPackConvert class.
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PACKCONVERT_H__51B7C0FB_36B7_4755_ACDB_00086C6FB6C6__INCLUDED_)
#define AFX_PACKCONVERT_H__51B7C0FB_36B7_4755_ACDB_00086C6FB6C6__INCLUDED_

#include "XmlPack.h"
//#include "pubstrtime.h"
#include "InterfacePack.h"
#include "BF_Tools.h"
#include "BF_SimpleLog.h"
#include "BF_Slist.h"

#if _MSC_VER > 1000
#pragma once
#endif /// _MSC_VER > 1000

#ifdef DPTOOL_EXPORTS
#define DPTOOL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define DPTOOL_EXPORT __declspec(dllimport)
#else
#define DPTOOL_EXPORT 
#endif
#endif

class DPTOOL_EXPORT CPackConvert  
{
public:

	///初始化，xml配置文件
	int Init(char *file);
	CPackConvert();
	CPackConvert(char *file);
	virtual ~CPackConvert();
	char m_errMsg[256];
	char m_logfile[41];///日志文件名
	CInterfacePack m_interfaceConf;///接口定义类
	bool m_isInit;///是否初始化

	char m_buf[1300]; ///buf内容
	int m_buflen;///buf长度
	int m_resConv;///应答包的包类型
protected:

	CBF_SimpleLog m_publog;
	
public:


	/// 函数名: Xml2AnsExt
	/// 编程  : 王明松 2006-8-14 9:53:37
	/// 返回  : bool 
	/// 参数  : int txcode
	/// 参数  : CXmlPack *xmlpack
	/// 参数  : void *buf
	/// 参数  : int &buflen
	/// 描述  : 根据返回码的值选择xmlpack包中的应答内容(定义的包头)
	bool Xml2AnsExt(int txcode, CXmlPack *xmlpack, char *buf, int &buflen);

	/// 函数名: Xml2AnsExt
	/// 编程  : 王明松 2006-11-14 9:53:37
	/// 返回  : bool 
	/// 参数  : int txcode
	/// 参数  : CXmlPack *xmlpack
	/// 参数  : void *buf
	/// 参数  : int &buflen
	/// 参数  : int type
	/// 描述  : 根据type的值选择xmlpack包中的应答内容(定义的包头)
	bool Xml2AnsExt(int txcode, CXmlPack *xmlpack, char *buf, int &buflen, int type);
	
		/// 函数名: Xml2Ans
	/// 编程  : 王明松 2006-6-8 16:23:08
	/// 返回  : bool 
	/// 参数  : int txcode
	/// 参数  : CXmlPack *xmlpack
	/// 参数  : void *buf
	/// 参数  : int &buflen
	/// 描述  : 根据返回码转换内部的xmlpack应答给外部
	bool Xml2Ans(int txcode,CXmlPack *xmlpack,void *buf,int &buflen);

	/// 函数名: Xml2Ans
	/// 编程  : 王明松 2006-11-14 16:23:08
	/// 返回  : bool 
	/// 参数  : int txcode
	/// 参数  : CXmlPack *xmlpack
	/// 参数  : void *buf
	/// 参数  : int &buflen
	/// 参数  : int type
	/// 描述  : 根据type的值选择xmlpack包中的应答内容应答给外部
	bool Xml2Ans(int txcode,CXmlPack *xmlpack,void *buf,int &buflen, int type);

	/// 函数名: Xml2Req
	/// 编程  : 王明松 2006-6-8 16:20:08
	/// 返回  : bool 
	/// 参数  : int txcode
	/// 参数  : CXmlPack *xmlpack 内部请求xml
	/// 参数  : void *buf 数据缓冲
	/// 参数  : int &buflen  数据缓冲长度
	/// 描述  : 转换内部xmlpack格式为外部请求
	bool Xml2Req(int txcode,CXmlPack *xmlpack,void *buf,int &buflen);

	/// 函数名: Conv2XmlAns
	/// 编程  : 王明松 2006-6-8 16:16:05
	/// 返回  : bool 
	/// 参数  : int txcode 交易码
	/// 参数  : void *buf 数据缓冲
	/// 参数  : int buflen 数据缓冲长度
	/// 参数  : CXmlPack *xmlpack 结果xml
	/// 描述  : 转换外部的应答为内部xmlpack格式
	bool Conv2XmlAns(int txcode,void *buf,int buflen,CXmlPack *xmlpack);

	/// 函数名: Conv2XmlReq
	/// 编程  : 王明松 2006-6-8 16:12:38
	/// 返回  : bool 
	/// 参数  : int txcode 交易码
	/// 参数  : void *buf   数据缓冲
	/// 参数  : int buflen  数据缓冲长度
	/// 参数  : CXmlPack *xmlpack 结果xml
	/// 描述  : 转换外部请求为内部xmlpack格式
	bool Conv2XmlReq(int &txcode,void *buf,int buflen,CXmlPack *xmlpack);

	/// 函数名: getResAttr
	/// 编程  : 王明松 2006-8-7 19:20:33
	/// 返回  : bool 
	/// 参数  : int txcode  交易码，必填
	/// 参数  : int retcode  返回码，如果此交易只有一个返回包，则可不填
	/// 参数  : int &fileattr  0通讯包  1写到文件
	/// 描述  : 根据交易码及返回码获取返回的类型属性 
	bool getResAttr(int txcode,int retcode,int &fileattr);

private:
	/// 函数名: conv2BufS
	/// 编程  : 王明松 2006-8-14 9:32:22
	/// 返回  : bool 
	/// 参数  : int txcode
	/// 参数  : S_PACKHEAD_INFO *headinfo
	/// 参数  : CXmlPack *xmlpack
	/// 参数  : char *buf
	/// 参数  : int &buflen
	/// 描述  : 将包头的配置项转换至定长缓冲
	bool conv2BufS(int txcode,S_PACKHEAD_INFO *headinfo,CXmlPack *xmlpack,char *buf,int &buflen);

	

	/// 函数名: setValueToBufL
	/// 编程  : 王明松 2006-8-3 17:17:38
	/// 返回  : bool 
	/// 参数  : CXmlPack *xmlpack
	/// 参数  : S_FUNC_FIELD_INFO *fieldconf
	/// 参数  : Cpubstrtime *publog
	/// 描述  : 根据配置设置字段值至有分隔的类中
	bool setValueToBufL(CXmlPack *xmlpack, S_FUNC_FIELD_INFO *fieldconf, CBF_Slist *publog);


	/// 函数名: setValueToBuf
	/// 编程  : 王明松 2006-8-3 17:16:53
	/// 返回  : bool 
	/// 参数  : CXmlPack *xmlpack
	/// 参数  : S_FUNC_FIELD_INFO *fieldconf
	/// 参数  : char *buf
	/// 描述  : 根据配置设置字段值至定长无分隔的BUF中
	bool setValueToBufS(CXmlPack *xmlpack,S_FUNC_FIELD_INFO *fieldconf,char *buf);

	/// 函数名: getValueFromBuf2
	/// 编程  : 王明松 2006-8-3 16:59:09
	/// 返回  : bool 
	/// 参数  : Cpubstrtime *publog
	/// 参数  : S_FUNC_FIELD_INFO *fieldconf
	/// 参数  : string value
	/// 描述  : 根据转换类型及配置信息从分隔符数据缓冲获取
	bool getValueFromBufL(CBF_Slist *publog,S_FUNC_FIELD_INFO *fieldconf,std::string &value);

	/// 函数名: getValueFromBuf1
	/// 编程  : 王明松 2006-8-3 16:59:06
	/// 返回  : bool 
	/// 参数  : char *buf
	/// 参数  : int buflen
	/// 参数  : S_FUNC_FIELD_INFO *fieldconf
	/// 参数  : string value
	/// 描述  : 根据转换类型及配置信息从定长数据缓冲获取 值
	bool getValueFromBufS(char *buf,int buflen,S_FUNC_FIELD_INFO *fieldconf,std::string &value);

	/// 函数名: getTransValue
	/// 编程  : 王明松 2006-8-3 13:32:52
	/// 返回  : string 
	/// 参数  : string transname
	/// 参数  : char *buf 
	/// 参数  : int flag  0定长 1分隔
	/// 参数  : int fieldtype 0-char 1-integer 2-double
	/// 描述  : 从数据缓冲中根据定义取出值,加减乘除
	std::string getTransValue(std::string transname,char *buf,int flag,int fieldtype);

	/// 函数名: getTransValue
	/// 编程  : 王明松 2006-6-12 17:38:53
	/// 返回  : string 
	/// 参数  : string transname
	/// 参数  : CXmlPack *xmlpack
	/// 参数  : int fieldtype 0-char 1-integer 2-double
	/// 描述  : 从xml中的节段中根据定义取出值,加减乘除
	std::string getTransValue(std::string transname,CXmlPack *xmlpack,int fieldtype);

	bool IsDouble(char *str);


};
#undef DPTOOL_EXPORT
#endif /// !defined(AFX_PACKCONVERT_H__51B7C0FB_36B7_4755_ACDB_00086C6FB6C6__INCLUDED_)
