// ICertApiBase.h: interface for the CICertApiBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICERTAPIBASE_H__FD3750E3_C97F_4C6F_985E_9892B175B658__INCLUDED_)
#define AFX_ICERTAPIBASE_H__FD3750E3_C97F_4C6F_985E_9892B175B658__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"

class CICertApiBase  
{
public:
	CICertApiBase()
	{

	}
	virtual ~CICertApiBase()
	{

	}
	

	// 函数名: InitCertApi
	// 编程  : 王明松 2013-5-24 11:05:29
	// 返回  : virtual void 
	// 描述  : 初始化证书api
	virtual bool InitCertApi(const char *certpath,char *certpwd) =0;

	// 函数名: UnInitCertApi
	// 编程  : 王明松 2013-5-24 11:05:29
	// 返回  : virtual void 
	// 描述  : 退出证书API
	virtual bool UnInitCertApi() =0;

	// 函数名: Sign
	// 编程  : 王明松 2013-5-24 11:07:02
	// 返回  : virtual bool 
	// 参数  : const char *signstr  要签名的字符串
	// 参数  : unsigned short signstrlen 要签名的字符串长度
	// 参数  : char *signres   签名结果
	// 参数  : unsigned short &signreslen   签名结果长度
	// 描述  : 签名，用本机的私钥签名
	virtual bool Sign(const char *signstr,unsigned short signstrlen,char *signres,unsigned short &signreslen)=0;


	// 函数名: GetPubicKey
	// 编程  : 王明松 2013-5-25 11:22:50
	// 返回  : virtual bool 
	// 参数  : char *signkeybuf   签名公钥
	// 参数  : unsigned short &signkeybuflen 签名公钥长度
	// 参数  : char *encryptkkeybuf  加密公钥
	// 参数  : unsigned short &encryptkkeybuflen 加密公钥长度
	// 描述  : 取本机证书的公钥
	virtual bool GetPubicKey(char *signkeybuf,unsigned short &signkeybuflen,char *encryptkeybuf,unsigned short &encryptkkeybuflen)=0;

	// 函数名: CheckSign
	// 编程  : 王明松 2013-5-30 10:43:56
	// 返回  : virtual bool 
	// 参数  : const char *publickey   签名公钥
	// 参数  : unsigned short publickeylen   签名公钥长度
	// 参数  : const char *signstr      签名的原始字符串
	// 参数  : unsigned short signstrlen  签名的原始字符串长度
	// 参数  : const char *signres   签名结果
	// 参数  : int signreslen 签名结果长度
	// 描述  : 校验签名，并校验证书是否一个根证书链的
	virtual bool CheckSign(const char *publickey,unsigned short publickeylen,const char *signstr,unsigned short signstrlen,const char *signres,unsigned short signreslen)=0;


	// 函数名: Encrypt
	// 编程  : 王明松 2013-5-25 11:27:12
	// 返回  : virtual bool 
	// 参数  : const char *publickey  加密公钥
	// 参数  : unsigned short publickeylen 加密公钥长度
	// 参数  : char *data   明文->加密数据
	// 参数  : unsigned short &datalen   加密数长度，加密前为要加密的数据长度，加密后为加密码后的数据长度
	// 描述  : 加密，用对方的公钥进行加密。仅在会话建立时使用。交易时使用会话密钥进行DES加密
	virtual bool Encrypt(const char *publickey,unsigned short publickeylen,char *data,unsigned short &datalen)=0;


	// 函数名: UnEncrypt
	// 编程  : 王明松 2013-5-25 11:28:33
	// 返回  : virtual bool 
	// 参数  : char *data   加密数据->解密数据
	// 参数  : unsigned short  &datalen 加密数据长度，解密前为加密的数据长度，解密后为数据明文长度
	// 描述  : 用本机的私钥进行解密
	virtual bool UnEncrypt(char *data,unsigned short  &datalen)=0;



	// 函数名: GetCertSerial
	// 编程  : 王明松 2013-6-14 15:51:14
	// 返回  : virtual bool 
	// 参数  : const char *pubkey
	// 参数  : unsigned short pubkeylen
	// 参数  : char *serial
	// 参数  : int &seriallen
	// 描述  : 获取证唯一序号
	virtual bool GetCertSerial(const char *pubkey,unsigned short pubkeylen,char *serial,unsigned short &seriallen)=0;


	// 函数名: GetErrMsg
	// 编程  : 王明松 2014-4-10 15:44:09
	// 返回  : virtual std::string 
	// 描述  : 获取初始化时的出错信息
	virtual std::string GetErrMsg()=0;
};

#endif // !defined(AFX_ICERTAPIBASE_H__FD3750E3_C97F_4C6F_985E_9892B175B658__INCLUDED_)
