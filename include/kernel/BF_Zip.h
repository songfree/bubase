// BF_Zip.h: interface for the CBF_Zip class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BF_ZIP_H__949008A6_9D40_493C_A3FE_CEF528ADBF23__INCLUDED_)
#define AFX_BF_ZIP_H__949008A6_9D40_493C_A3FE_CEF528ADBF23__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef KERNEL_EXPORTS
#define KERNEL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNEL_EXPORT __declspec(dllimport)
#else
#define KERNEL_EXPORT 
#endif
#endif

extern "C" 
{ 
	// 函数名: BF_Compress
	// 编程  : 王明松 2012-4-20 16:18:10
	// 返回  : bool  true：成功，false压缩失败。
	// 参数  : unsigned char * destbuf 压缩后的数据，
	// 参数  : unsigned int * destlen  压缩前为destbuf的大小，必须大于srbuflen，压缩后为压缩后的长度
	// 参数  : unsigned char * srbuf   压缩前的数据  
	// 参数  : int srbuflen            srbuf中数据的长度
	// 描述  : 压缩数据
	KERNEL_EXPORT bool  BF_Compress(unsigned char * destbuf,unsigned int * destlen,unsigned char * srbuf,int srbuflen);
	

	// 函数名: BF_Uncompress
	// 编程  : 王明松 2012-4-20 16:18:15
	// 返回  : bool  true：成功，false解压缩失败。
	// 参数  : unsigned char* destbuf   解压缩后的数据
	// 参数  : unsigned int * destlen   解压缩前为destbuf的大小，必须大于srbuflen，解压缩后为解压缩的长度
	// 参数  : unsigned char* srbuf     解压缩前的数据
	// 参数  : int srbuflen             srbuf中数据的长度
	// 描述  : 解压缩数据
	KERNEL_EXPORT bool  BF_Uncompress(unsigned char* destbuf, unsigned int * destlen, unsigned char* srbuf, int srbuflen);
}

#endif // !defined(AFX_BF_ZIP_H__949008A6_9D40_493C_A3FE_CEF528ADBF23__INCLUDED_)
