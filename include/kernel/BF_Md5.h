// BF_Md5.h: interface for the CBF_Md5 class.
//
//////////////////////////////////////////////////////////////////////

/***********************************************************************************
  编码日志记录
  编码者: 王明松
  编码日期 2012-4-17 14:18:22 重构
  版本: V1.00 
  说明: MD5算法
 ***********************************************************************************/


#if !defined(AFX_BF_MD5_H__5D56F139_BA63_4CAA_B29F_04B893858170__INCLUDED_)
#define AFX_BF_MD5_H__5D56F139_BA63_4CAA_B29F_04B893858170__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "typedefine.h"



#ifdef KERNEL_EXPORTS
#define KERNEL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNEL_EXPORT __declspec(dllimport)
#else
#define KERNEL_EXPORT 
#endif
#endif

#define MD5_BUFFER_SIZE 1024   //生成MD5的最大长度

class KERNEL_EXPORT CBF_Md5  
{

public:
	CBF_Md5();
	CBF_Md5(const void *input, size_t length);
	CBF_Md5(const string &str);
	CBF_Md5(ifstream &in);
	virtual ~CBF_Md5();


	// 函数名: Update
	// 编程  : 王明松 2012-4-17 14:39:13
	// 返回  : void 
	// 参数  : const void *input
	// 参数  : size_t length
	// 描述  : 更新要做MD5的数据
	void Update(const void *input, size_t length);

	// 函数名: Update
	// 编程  : 王明松 2012-4-17 14:39:31
	// 返回  : void 
	// 参数  : const string &str
	// 描述  : 更新要做MD5的数据
	void Update(const string &str);

	// 函数名: Update
	// 编程  : 王明松 2012-4-17 14:39:39
	// 返回  : void 
	// 参数  : ifstream &in
	// 描述  : 更新要做MD5的数据
	void Update(ifstream &in);

	// 函数名: digest
	// 编程  : 王明松 2012-4-17 14:36:53
	// 返回  : const byte*  16位长度char 以\0结束
	// 描述  : 得到Update后的数据的MD5,16位长度
	const byte* Digest();

	// 函数名: ToString
	// 编程  : 王明松 2012-4-17 14:34:32
	// 返回  : string 
	// 描述  : 得到Update后的数据的MD5并转换为可显示字符
	std::string ToString();

	// 函数名: Reset
	// 编程  : 王明松 2012-4-17 14:34:17
	// 返回  : void 
	// 描述  : 重置之前的Update数据
	void Reset();
private:
	void Update(const byte *input, size_t length);
	void Final();
	void Transform(const byte block[64]);
	void Encode(const UINT32 *input, byte *output, size_t length);
	void Decode(const byte *input, UINT32 *output, size_t length);
	std::string BytesToHexString(const byte *input, size_t length);
	

private:
	UINT32 m_state[4];	/* state (ABCD) */
	UINT32 m_count[2];	/* number of bits, modulo 2^64 (low-order word first) */
	byte m_buffer[64];	/* input buffer */
	byte m_digest[17];	/* message digest */
	bool m_finished;		/* calculate finished ? */
	
	static const byte PADDING[64];	/* padding for calculate */
	static const char HEX[16];

};

#endif // !defined(AFX_BF_MD5_H__5D56F139_BA63_4CAA_B29F_04B893858170__INCLUDED_)
