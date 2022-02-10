// BpcEndian.h: interface for the CBpcEndian class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BPCENDIANLITTLE_H__C11F5AA8_5EDB_48EC_9B30_07C176F7B137__INCLUDED_)
#define AFX_BPCENDIANLITTLE_H__C11F5AA8_5EDB_48EC_9B30_07C176F7B137__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BpcHead.h"

#ifdef DBPUB_EXPORTS
#define DBPUB_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define DBPUB_EXPORT __declspec(dllimport)
#else
#define DBPUB_EXPORT 
#endif
#endif


class DBPUB_EXPORT CBpcEndian  
{
public:

	// 函数名: SetCommEndian
	// 编程  : 王明松 2013-4-25 11:13:08
	// 返回  : void 
	// 参数  : bool islittle true是windows linux所使用的主机序  false为网络序
	// 描述  : 设置通讯时的字节序 
	void SetCommEndian(bool islittle);

	// 函数名: Endian2Comm
	// 编程  : 王明松 2013-4-25 10:41:58
	// 返回  : void 
	// 参数  : PBPCCOMMSTRU data
	// 描述  : 通讯时，将字节序转为通讯时所用的字节序
	void Endian2Comm(PBPCCOMMSTRU data);

	void Endian2Comm(unsigned char *data,int datalen);

	// 函数名: Endian2LocalHost
	// 编程  : 王明松 2013-4-25 10:42:03
	// 返回  : void 
	// 参数  : PBPCCOMMSTRU data
	// 描述  : 将通讯的主机字节序转为本机所使用的字节序
	void Endian2LocalHost(PBPCCOMMSTRU data);

	void Endian2LocalHost(unsigned char *data,int datalen);

	CBpcEndian();
	virtual ~CBpcEndian();
	
protected:

	bool m_bIsLittle;//是否网络序 false为网络序 

	bool m_bUseLittleEndian;//通讯是否使用linux,windows主机字节序 false则为网络序

	void Reverse(unsigned char *buf, int len);

};

#endif // !defined(AFX_BPCENDIANLITTLE_H__C11F5AA8_5EDB_48EC_9B30_07C176F7B137__INCLUDED_)
