// GtpProtocol.h: interface for the CGtpProtocol class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GTPPROTOCOL_H__90EBA403_F326_4D89_A996_9727CC0199BB__INCLUDED_)
#define AFX_GTPPROTOCOL_H__90EBA403_F326_4D89_A996_9727CC0199BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GtpData.h"
#include "BpcEndian.h"


#ifdef _WINDOWS
#ifdef _DEBUG
#pragma comment(lib, "bf_dbpubd.lib") 
#pragma comment(lib, "bf_kerneld.lib") 
#pragma message("Automatically linking with  bf_kerneld.lib bf_dbpubd.lib")
#else
#pragma comment(lib, "bf_kernel.lib") 
#pragma comment(lib, "bf_dbpub.lib") 
#pragma message("Automatically linking with  bf_kernel.lib bf_dbpub.lib")
#endif
#endif

//针对黄金交易所的报文协议类

#define  GTPTypeNone                0     //此报文不具有任何意义，一般用于心跳
#define  GTPTypeString              1     //报文体是正常的域数据内容
#define  GTPTypeBinary              2     //报文体是二进制数据
#define  GTPTypeCompressed          3     //报文体是压缩后的数据，需要解压缩后再处理
#define  GTPTypeEncrypt             4     //报文体是加密后的数据，需要解密后再处理

#define  SESSION_EXTHEAD_LEN        127   //扩展报文头长度
#define  GTPMSGMAXLEN               16384  //GTP最大报文长度

#ifdef __cplusplus
extern "C" {
#endif
	
	//字节对齐
#ifdef _WINDOWS
#define __PACKED__ 
#pragma pack(push,1)
#else
#ifdef __GNUC__
#define __PACKED__	__attribute__ ((packed))
	//#pragma pack(push,1)
#else
#define __PACKED__
#pragma options align=packed
#endif
#endif

//  魔数(4)+LOGID(4)+基本报头(4)+扩充报头(0-127)+报文体(GTP)

typedef struct 
{
	unsigned int   nRandNum;//魔数   用于在会话层读取报文时进行验证，当读入流发生错误时，可在底层及时识别
	unsigned int   nLogId;//日志ID   便于日志的筛选和排查问题  接收方对于与该请求对应的后续处理，通过logID进行识别。以报单请求：二级系统上送报单请求时，会随机生成一个logID发给交易所，交易所后续与该报单请求相对应后继消息（报单应答、报单回报、成交回报）都会带上该特征值
	unsigned char  cPackType;//报文类型   见上述宏定义
	unsigned char  cExtLength;//扩充报文长度
	unsigned short nBodyLen;   //信息正文长度
}__PACKED__ S_GTP_SESSION_HEAD;

typedef struct 
{
	S_GTP_SESSION_HEAD  head;         //会话头
	char                sExtInfo[SESSION_EXTHEAD_LEN];//扩充报文头
}__PACKED__ S_GTP_SESSION;


typedef struct 
{
	S_GTP_SESSION_HEAD  head;         //会话头
	char                sExtInfo[SESSION_EXTHEAD_LEN];//扩充报文头
	char                sGtpMsg[GTPMSGMAXLEN];
}__PACKED__ S_GTP_MSG;

//取消字节对齐
#ifdef _WINDOWS
#pragma pack(pop)
#else
#ifdef __GNUC__
//#pragma pack(pop)
#else
#pragma options align=reset
#endif
#endif
#undef __PACKED__

#ifdef __cplusplus
}
#endif




#ifdef GTP_EXPORTS
#define GTP_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define GTP_EXPORT __declspec(dllimport)
#else
#define GTP_EXPORT 
#endif
#endif

class GTP_EXPORT CGtpProtocol  
{
public:

	// 函数名: ToGtpDataString
	// 编程  : 王明松 2016-3-3 14:26:30
	// 返回  : bool 
	// 参数  : std::string gtpdata
	// 描述  : 只生成域的报文
	bool ToGtpDataString(std::string gtpdata);


	// 函数名: ToGtpHeadBuffer
	// 编程  : 王明松 2016-3-3 14:26:52
	// 返回  : bool 
	// 参数  : char *gtpbuffer
	// 参数  : int &buflen
	// 描述  : 生成GTP会话报文头    
	bool ToGtpHeadBuffer(char *gtpbuffer,int &buflen);

	// 函数名: FromGtpData
	// 编程  : 王明松 2016-3-3 14:12:54
	// 返回  : bool 
	// 参数  : const char *gtpdata   kv类的文本报文
	// 描述  : 从域文本解析 特别是当gtp协议里过来的报文是加密或压缩时，先调用FromGtpBuffer得到会话头和加密压缩报文，去解密解压后再调用本方法
	bool FromGtpData(const char *gtpdata);

	// 函数名: ToBuffer
	// 编程  : 王明松 2016-3-3 10:03:11
	// 返回  : bool true成功
	// 输出参数  : char *gtpbuffer  缓冲  
	// 输入输出参数  : int &buflen  输入为缓冲大小 输出为报文大小
	// 描述  : 生成gtp协议报文，包含域，生成的报文类型为GTPTypeString
	bool ToBuffer(char *gtpbuffer, int &buflen);

	// 函数名: FromBuffer
	// 编程  : 王明松 2016-3-3 10:02:07
	// 返回  : int  <0解析出错  0为无GTP域报文   1为域文本并解析  >1 GTP数据非域文本，返回为压缩或加密标志，见报文类型2 3 4
	// 输入参数  : const char *gtpbuffer   GTP协议报文  
	// 输入输出参数  : int buflen          输入为GTP缓冲数据长度，输出为当前的GTP报文长度
	// 描述  : 从缓冲解析GTP协议报文，若报文类型是GTPTypeString则解析，否则直接返回报文类型，后面根据buflen取出报文进行处理。
	int FromGtpBuffer(const char *gtpbuffer,int &buflen);

	// 函数名: GetGtpData
	// 编程  : 王明松 2016-3-3 10:00:44
	// 返回  : CGtpData * 
	// 描述  : 取GTP数据类指针，然后可以设置值取值等等
	CGtpData * GetGtpData();

	// 函数名: SetHeadInfo
	// 编程  : 王明松 2016-3-3 9:38:32
	// 返回  : void 
	// 参数  : unsigned int randnum   魔数
	// 参数  : unsigned int logid     日志ID
	// 参数  : char msgtype           报文类型
	// 描述  : 设置会话头信息
	void SetHeadInfo(unsigned int randnum, unsigned int logid,char  msgtype);

	// 函数名: SetExtHead
	// 编程  : 王明松 2016-3-3 9:40:28
	// 返回  : bool true成功
	// 参数  : int extlen   扩展报文头长度，不能大于127
	// 参数  : char *head   扩展报文头信息
	// 描述  : 设置扩展报文头信息
	bool SetExtHead(int extlen,char *head);

	// 函数名: SetBodyLen
	// 编程  : 王明松 2016-3-3 9:39:45
	// 返回  : void 
	// 参数  : unsigned short len  gtp长度(k=v,k1=v1等)
	// 描述  : 设置gtp消息长度
	void SetBodyLen(unsigned short len);

	// 函数名: NewGtp
	// 编程  : 王明松 2016-3-3 9:29:58
	// 返回  : void 
	// 参数  : unsigned int randnum   魔数
	// 参数  : unsigned int logid     日志ID
	// 参数  : char cPackType=GTPTypeString  报文类型
	// 描述  : 生成一个新的GTP数据包  扩展头默认没有
	void NewGtp(unsigned int randnum,unsigned int logid,char cPackType=GTPTypeString);

	CGtpProtocol();
	virtual ~CGtpProtocol();

protected:

	S_GTP_SESSION   m_pGtpSessionHead;//会话报文头

	CGtpData  m_pGtpData;//gtp报文类

	CBF_SimpleLog  m_pLog;
	CBpcEndian     m_pEndian;//字节序处理

};

#endif // !defined(AFX_GTPPROTOCOL_H__90EBA403_F326_4D89_A996_9727CC0199BB__INCLUDED_)
