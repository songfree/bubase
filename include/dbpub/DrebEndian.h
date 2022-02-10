// DrebEndian.h: interface for the CDrebEndian class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DREBENDIAN_H__86E9E605_0216_4F05_80FB_D0BBA4A5F9AE__INCLUDED_)
#define AFX_DREBENDIAN_H__86E9E605_0216_4F05_80FB_D0BBA4A5F9AE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "public.h"
#include "DrebPubDefine.h"

#ifdef DBPUB_EXPORTS
#define DBPUB_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define DBPUB_EXPORT __declspec(dllimport)
#else
#define DBPUB_EXPORT 
#endif
#endif

class DBPUB_EXPORT CDrebEndian  
{

public:
	// 函数名: CRC
	// 编程  : 王明松 2013-5-6 14:20:10
	// 返回  : unsigned int 
	// 参数  : void *pdata
	// 参数  : int datalen
	// 描述  : 产生CRC校验位
	unsigned int CRC(void *pdata,int datalen);

	// 函数名: SetCommEndian
	// 编程  : 王明松 2013-4-25 11:13:08
	// 返回  : void 
	// 参数  : bool islittle true是windows linux所使用的主机序  false为网络序
	// 描述  : 设置通讯时的字节序 
	void SetCommEndian(bool islittle);
	
	// 函数名: Endian2Comm
	// 编程  : 王明松 2013-4-25 10:41:58
	// 返回  : void 
	// 参数  : PDREB_HEAD data
	// 描述  : 通讯时，将字节序转为通讯时所用的字节序
	void Endian2Comm(PDREB_HEAD data);
	

	// 函数名: Endian2Comm
	// 编程  : 王明松 2013-5-6 14:42:15
	// 返回  : void 
	// 参数  : unsigned char *data
	// 参数  : int datalen
	// 描述  : 通讯时，将字节序转为通讯时所用的字节序
	void Endian2Comm(unsigned char *data,int datalen);


	// 函数名: Endian2CommGenCrc
	// 编程  : 王明松 2013-5-6 14:31:04
	// 返回  : void 
	// 参数  : PDREB_HEAD data
	// 描述  : 通讯时，将字节序转为通讯时所用的字节序，并生成CRC校验位
	void Endian2CommGenCrc(PDREB_HEAD data);
	
	// 函数名: Endian2LocalHost
	// 编程  : 王明松 2013-4-25 10:42:03
	// 返回  : void 
	// 参数  : PDREB_HEAD data
	// 描述  : 将通讯的主机字节序转为本机所使用的字节序
	void Endian2LocalHost(PDREB_HEAD data);


	// 函数名: Endian2LocalHostChkCrc
	// 编程  : 王明松 2013-5-6 14:21:46
	// 返回  : bool 
	// 参数  : PDREB_HEAD data
	// 描述  : 将通讯的主机字节序转为本机所使用的字节序，并检查CRC字段
	bool Endian2LocalHostChkCrc(PDREB_HEAD data);
	

	// 函数名: Endian2LocalHost
	// 编程  : 王明松 2013-5-6 14:42:02
	// 返回  : void 
	// 参数  : unsigned char *data
	// 参数  : int datalen
	// 描述  : 将通讯的主机字节序转为本机所使用的字节序
	void Endian2LocalHost(unsigned char *data,int datalen);
	

	// 函数名: Reverse
	// 编程  : 王明松 2013-5-6 14:41:45
	// 返回  : void 
	// 参数  : unsigned char *buf
	// 参数  : int len
	// 描述  : 进行反转
	void Reverse(unsigned char *buf, int len);

	CDrebEndian();
	virtual ~CDrebEndian();
	
protected:
	
	bool m_bIsLittle;//是否网络序 false为网络序 
	
	bool m_bUseLittleEndian;//通讯是否使用linux,windows主机字节序 false则为网络序
	
	

	
};

#endif // !defined(AFX_DREBENDIAN_H__86E9E605_0216_4F05_80FB_D0BBA4A5F9AE__INCLUDED_)
