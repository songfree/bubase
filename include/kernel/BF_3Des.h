// BF_3Des.h: interface for the CBF_3Des class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BF_3DES_H__1CC4AC38_7C5E_4D11_9515_6C76DBFE3769__INCLUDED_)
#define AFX_BF_3DES_H__1CC4AC38_7C5E_4D11_9515_6C76DBFE3769__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BF_Des.h"


#ifdef KERNEL_EXPORTS
#define KERNEL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNEL_EXPORT __declspec(dllimport)
#else
#define KERNEL_EXPORT 
#endif
#endif

//非标准的3DES算法 ，先用KEY

class KERNEL_EXPORT CBF_3Des  
{
public:

	// 函数名: UnDes3Buffer
	// 编程  : 王明松 2017-4-27 9:37:38
	// 返回  : bool 
	// 输入输出参数  : char *buffer   要解密的字符串 16倍数    解密后的字符串 
	// 参数  : int buflen     要解密的字符串长度   16的倍数
	// 参数  : const char *key   24个字符
	// 参数  : char *errmsg  出错信息
	// 描述  : 3DES解密
	bool UnDes3Buffer(char *buffer,int buflen,const char *key,char *errmsg);

	// 函数名: Des3Buffer
	// 编程  : 王明松 2017-4-27 9:37:43
	// 返回  : bool 
	// 输入输出参数  : char *buffer  要加密的字符串   3des密文16倍数
	// 参数  : int bufsize     缓冲区大小
	// 参数  : const char *key  24个字符
	// 参数  : char *errmsg  出错信息
	// 描述  : 3DES加密
	bool Des3Buffer(char *buffer,int bufsize,const char *key,char *errmsg);

	CBF_3Des();
	virtual ~CBF_3Des();


	CBF_DesEnctypt  m_pDes;

};

#endif // !defined(AFX_BF_3DES_H__1CC4AC38_7C5E_4D11_9515_6C76DBFE3769__INCLUDED_)
