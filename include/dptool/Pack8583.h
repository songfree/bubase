// Pack8583.h: interface for the CPack8583 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PACK8583_H__E88F88EC_1F39_4740_84AA_A258292576C3__INCLUDED_)
#define AFX_PACK8583_H__E88F88EC_1F39_4740_84AA_A258292576C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "XmlPack.h"

#ifdef PACK8583_EXPORTS
    #define CONVERTLIB_API __declspec(dllexport)
#else
    #ifdef WIN_DLL
        #define CONVERTLIB_API __declspec(dllimport)
    #else
        #define CONVERTLIB_API 
    #endif
#endif

typedef struct _struct8583
{
	int idx;
	string value;
};
typedef struct _struct8583def
{
	int idx;
	CNode *pfield;
};
typedef std::list<_struct8583> L_PACK8583;
typedef std::vector<_struct8583def> V_pack8583def;

class  CONVERTLIB_API CPack8583  
{
public:
	CPack8583();
	CPack8583(char *xmldefine);
	virtual ~CPack8583();

	CXmlPack m_xml8583;//8583包定义文件
	
	char m_packHead[512];//8583位图,最长512位
	char m_packBuffer[8193];
	int  m_bitLen;//位图长度
	int  m_fieldNum;//字段个数
	bool m_bIsLoad;//是否装载8583配置成功
	char m_errMsg[512];//错误信息

private:
	L_PACK8583 m_vpack8583;//8583包数据

	V_pack8583def m_vpack8583def;//8583包定义	
public:
	bool LoadPackDef(char *xmlfile);

	// 函数名: GetPackBuffer
	// 编程  : 王明松 2007-9-8 18:04:08
	// 返回  : bool 
	// 参数  : char *buffer  8583包数据
	// 参数  : int &buflen   8583包数据长度
	// 描述  : 获取8583包的内容
	bool GetPackBuffer(char *buffer,int &buflen);

	// 函数名: SetFieldValue
	// 编程  : 王明松 2007-9-8 18:01:36
	// 返回  : bool 
	// 参数  : const char *name  位图名称
	// 参数  : const double value 值
	// 描述  : 设置字段值
	bool SetFieldValue(const char *name,const double value);

	// 函数名: SetFieldValue
	// 编程  : 王明松 2007-9-8 18:01:32
	// 返回  : bool 
	// 参数  : const char *name 位图名称
	// 参数  : const int value 值
	// 描述  : 设置字段值
	bool SetFieldValue(const char *name,const int value);


	// 函数名: SetFieldValue
	// 编程  : 王明松 2007-9-8 18:01:28
	// 返回  : bool 
	// 参数  : const char *name 位图名称
	// 参数  : const string value 值
	// 描述  : 设置字段值
	bool SetFieldValue(const char *name,const string value);


	// 函数名: SetFieldValue
	// 编程  : 王明松 2007-9-8 18:01:25
	// 返回  : bool 
	// 参数  : const char *name 位图名称
	// 参数  : const char *value 值
	// 描述  : 设置字段值
	bool SetFieldValue(const char *name,const char *value);


	// 函数名: SetFieldValue
	// 编程  : 王明松 2007-9-8 18:01:21
	// 返回  : bool 
	// 参数  : const int idx
	// 参数  : const double value
	// 描述  : 设置字段值
	bool SetFieldValue(const int idx,const double value);

	// 函数名: SetFieldValue
	// 编程  : 王明松 2007-9-8 18:00:22
	// 返回  : bool 
	// 参数  : const int idx
	// 参数  : const int value
	// 描述  : 设置字段值
	bool SetFieldValue(const int idx,const int value);

	// 函数名: SetFieldValue
	// 编程  : 王明松 2007-9-8 17:51:06
	// 返回  : bool 
	// 参数  : const int idx 位置序号
	// 参数  : const string value 值
	// 描述  : 设置字段值
	bool SetFieldValue(const int idx,const string value);

	// 函数名: SetFieldValue
	// 编程  : 王明松 2007-9-8 17:49:38
	// 返回  : bool 
	// 参数  : const int idx  位置序号
	// 参数  : const char *value  值
	// 描述  : 设置字段值 
	bool SetFieldValue(const int idx,const char *value);
	void New8583();

};
#undef DPTOOL_EXPORT
#endif // !defined(AFX_PACK8583_H__E88F88EC_1F39_4740_84AA_A258292576C3__INCLUDED_)
