// XdpFmt.h: interface for the CXdpFmt class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XDPFMT_H__9EEAE1E1_1E78_48AE_8715_9BB6553A1BD2__INCLUDED_)
#define AFX_XDPFMT_H__9EEAE1E1_1E78_48AE_8715_9BB6553A1BD2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BF_Xml.h"
#include "MdbBase.h"

#ifdef _WINDOWS
#ifdef _DEBUG
#pragma comment(lib, "bf_kerneld.lib") 
#pragma message("Automatically linking with   bf_kerneld.lib ")
#else
#pragma comment(lib, "bf_kernel.lib") 
#pragma message("Automatically linking with   bf_kernel.lib ")
#endif
#endif

//读取xml配置，生成单记录最大字节长度


#define  XDP_UNKNOW    0    //未知
#define  XDP_USHORT    1    //无符号短整型
#define  XDP_SHORT     2    //短整型
#define  XDP_UINT      3    //无符号整型
#define  XDP_INT       4    //整型
#define  XDP_DOUBLE    5    //浮点型
#define  XDP_CHAR      6    //固定长度字符串 最大255
#define  XDP_VARCHAR   7    //变长字符串 最大255   最前面有一个字节长度  1+data
#define  XDP_BINDATA   8    //二进制类型 最大4096  最前面有两个字节长度  2+data 


#define      XDP_BITMAPSIZE     16      //位图大小  16字节
#define      XDP_FIELD_NAMELEN  20       //字段名称长度
#define      XDP_FIELD_CHARLEN  255      //字符串最大长度
#define      XDP_FIELD_BINLEN   4096     //二进制数据最大长度



typedef struct 
{
	unsigned short  f_index;      //字段索引  从1开始
	std::string     f_name ;      //字段名称
	char            f_type;       //字段类型
	unsigned  short f_length;     //字段长度
	unsigned  short f_extlength;  //扩展长度，针对XDP_VARCHAR和XDP_BINDATA分别为1和2
	unsigned  int   f_offset;     //此字段在全值的偏移位置  设置值和解析时根据此位置填值和取值
} S_XDP_FIELD_DEFINE;

#ifdef XDP_EXPORTS
#define CXDP_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define CXDP_EXPORT __declspec(dllimport)
#else
#define CXDP_EXPORT 
#endif
#endif

class CXDP_EXPORT CXdpFmt  
{
public:

	// 函数名: XdpFmtCopy
	// 编程  : 王明松 2014-11-11 9:27:41
	// 返回  : bool true成功 false失败
	// 参数  : CXdpFmt *src  源fmt
	// 描述  : 从一个现有fmt复制xdp配置
	bool XdpFmtCopy(CXdpFmt *src);

	// 函数名: ResetFmt
	// 编程  : 王明松 2014-11-11 9:17:51
	// 返回  : void 
	// 描述  : 重置XDP格式，之后必须init或XdpFmtCopy
	void ResetFmt();

	// 函数名: char GetVersion
	// 编程  : 王明松 2014-11-11 9:06:49
	// 返回  : unsigned  
	// 描述  : 取得XDP版本
	unsigned  char GetVersion();

	// 函数名: GetFieldNum
	// 编程  : 王明松 2014-10-27 16:35:45
	// 返回  : int 
	// 描述  : 取得字段数
	int GetFieldNum();

	// 函数名: GetField
	// 编程  : 王明松 2014-10-27 16:12:17
	// 返回  : bool true找到    false无此字段
	// 参数  : const char *fieldname   字段名称
	// 参数  : S_XDP_FIELD_DEFINE &field
	// 描述  : 取得定义字段
//	bool GetField(const char *fieldname,S_XDP_FIELD_DEFINE &field);

	// 函数名: GetField
	// 编程  : 王明松 2014-10-27 16:12:21
	// 返回  : bool true找到    false无此字段
	// 参数  : unsigned short index  字段索引
	// 参数  : S_XDP_FIELD_DEFINE &field
	// 描述  : 取得定义字段
//	bool GetField(unsigned short index,S_XDP_FIELD_DEFINE &field);

	// 函数名: GetField
	// 编程  : 王明松 2014-10-27 16:12:21
	// 返回  : NULL无此字段 
	// 参数  : unsigned short index  字段索引
	// 描述  : 取得定义字段
	S_XDP_FIELD_DEFINE *GetField(const char *fieldname);
	S_XDP_FIELD_DEFINE *GetField(unsigned short index);

	S_XDP_FIELD_DEFINE *At(int index);

	// 函数名: GetMaxLength
	// 编程  : 王明松 2014-10-27 16:07:05
	// 返回  : unsigned int 
	// 描述  : 取得报文的最大长度，以此分配空间
	unsigned int GetMaxLength();

	// 函数名: Init
	// 编程  : 王明松 2014-10-23 14:24:13
	// 返回  : bool true成功 false失败
	// 参数  : const char *xmlfilepathname
	// 描述  : 初始化报文定义
	bool Init(const char *xmlfilepathname,char *errmsg);

	CXdpFmt();
	virtual ~CXdpFmt();

protected:

	// 函数名: AddFmt
	// 编程  : 王明松 2014-10-24 12:57:12
	// 返回  : bool 
	// 参数  : S_XDP_FIELD_DEFINE field
	// 描述  : 将配置项加入数据表
	bool AddFmt(S_XDP_FIELD_DEFINE field);

	// 函数名: GetType
	// 编程  : 王明松 2014-10-24 12:56:39
	// 返回  : unsigned short 类型
	// 参数  : const char *type
	// 描述  : 根据字段字符串取得字段类型
	unsigned short GetType(const char *type);

	
	unsigned short  m_nMaxLength;      //根据配置算出来的最大报文长度
	unsigned short  m_nFieldNum;       //字段数

	CMemTable <S_XDP_FIELD_DEFINE> m_table;        //字段配置表
	CPkeyInt<1>             m_index;              //字段索引
 	CPkeyCharF<20,1>        m_indexname;          //字段名称索引

	unsigned  char          m_cVersion;          //XDP版本

};

#endif // !defined(AFX_XDPFMT_H__9EEAE1E1_1E78_48AE_8715_9BB6553A1BD2__INCLUDED_)
