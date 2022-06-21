// OpenApi.h: interface for the COpenApi class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPENAPI_H__90C341F9_1F6D_4055_A3E0_2801AF81E54D__INCLUDED_)
#define AFX_OPENAPI_H__90C341F9_1F6D_4055_A3E0_2801AF81E54D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ICertApiBase.h"
#include "public.h"

#include <openssl/md5.h>   
#include <openssl/rsa.h>    
#include <openssl/objects.h>  
#include <openssl/x509.h>  
#include <openssl/err.h>  
#include <openssl/pem.h>  
#include <openssl/pkcs12.h>   
#include <openssl/ssl.h>  
#include <openssl/evp.h>




#ifdef _WINDOWS

#else
#define WINAPI
#endif

#pragma comment(lib, "bf_kerneld.lib") 
#pragma comment(lib, "libeay32.lib") 
#pragma comment(lib, "ssleay32.lib") 
#pragma message("Automatically linking with   bf_kerneld.lib ssleay32.lib libeay32.lib ")


//使用rsa算法   pfx文件

class DLL_EXPORT COpenApi : public CICertApiBase  
{
public:
	COpenApi();
	virtual ~COpenApi();


	// 函数名: InitCertApi
	// 编程  : 王明松 2013-5-24 11:05:29
	// 返回  : virtual void 
	// 参数  : const char *certpath  证书目录 最后不带"/"
	// 参数  : char *certpwd  证书密码
	// 描述  : 初始化证书api
	virtual bool InitCertApi(const char *certpath,char *certpwd) ;
	
	// 函数名: UnInitCertApi
	// 编程  : 王明松 2013-5-24 11:05:29
	// 返回  : virtual void 
	// 描述  : 退出证书API
	virtual bool UnInitCertApi() ;
	
	// 函数名: Sign
	// 编程  : 王明松 2013-5-24 11:07:02
	// 返回  : virtual bool 
	// 参数  : const char *signstr  要签名的字符串
	// 参数  : unsigned short signstrlen 要签名的字符串长度
	// 参数  : char *signres   签名结果
	// 参数  : unsigned short &signreslen   签名结果长度
	// 描述  : 签名，用本机的私钥签名
	virtual bool Sign(const char *signstr,unsigned short signstrlen,char *signres,unsigned short &signreslen);
	
	
	// 函数名: GetPubicKey
	// 编程  : 王明松 2013-5-25 11:22:50
	// 返回  : virtual bool 
	// 参数  : char *signkeybuf   签名公钥
	// 参数  : unsigned short &signkeybuflen 签名公钥长度
	// 参数  : char *encryptkkeybuf  加密公钥
	// 参数  : unsigned short &encryptkkeybuflen 加密公钥长度
	// 描述  : 取本机证书的公钥
	virtual bool GetPubicKey(char *signkeybuf,unsigned short &signkeybuflen,char *encryptkeybuf,unsigned short &encryptkkeybuflen);
	
	// 函数名: CheckSign
	// 编程  : 王明松 2013-5-30 10:43:56
	// 返回  : virtual bool 
	// 参数  : const char *publickey   签名公钥
	// 参数  : unsigned short publickeylen   签名公钥长度
	// 参数  : const char *signstr      签名的原始字符串
	// 参数  : unsigned short signstrlen  签名的原始字符串长度
	// 参数  : const char *signres   签名结果  注意要有结束符 0
	// 参数  : int signreslen 签名结果长度
	// 描述  : 校验签名，并校验证书是否一个根证书链的
	virtual bool CheckSign(const char *publickey,unsigned short publickeylen,const char *signstr,unsigned short signstrlen,const char *signres,unsigned short signreslen);
	
	
	// 函数名: Encrypt
	// 编程  : 王明松 2013-5-25 11:27:12
	// 返回  : virtual bool 
	// 参数  : const char *publickey  加密公钥
	// 参数  : unsigned short publickeylen 加密公钥长度
	// 参数  : char *data   明文->加密数据
	// 参数  : unsigned short &datalen   加密数长度，加密前为要加密的数据长度，加密后为加密码后的数据长度
	// 描述  : 加密，用对方的公钥进行加密。仅在会话建立时使用。交易时使用会话密钥进行DES加密
	virtual bool Encrypt(const char *publickey,unsigned short publickeylen,char *data,unsigned short &datalen);
	
	
	// 函数名: UnEncrypt
	// 编程  : 王明松 2013-5-25 11:28:33
	// 返回  : virtual bool 
	// 参数  : char *data   加密数据->解密数据
	// 参数  : unsigned short  &datalen 加密数据长度，解密前为加密的数据长度，解密后为数据明文长度
	// 描述  : 用本机的私钥进行解密
	virtual bool UnEncrypt(char *data,unsigned short  &datalen);
	
	
	
	// 函数名: GetCertSerial
	// 编程  : 王明松 2013-6-14 15:51:14
	// 返回  : virtual bool 
	// 参数  : const char *pubkey
	// 参数  : unsigned short pubkeylen
	// 参数  : char *serial
	// 参数  : int &seriallen
	// 描述  : 获取证唯一序号
	virtual bool GetCertSerial(const char *pubkey,unsigned short pubkeylen,char *serial,unsigned short &seriallen);


	// 函数名: GetErrMsg
	// 编程  : 王明松 2014-4-10 15:44:35
	// 返回  : virtual std::string 
	// 描述  : 
	virtual std::string GetErrMsg();
protected:
	
	char               m_errMsg[256];
    char               m_sPublicKey[4096];
	int                m_nPublicKeyLen;

	EVP_PKEY          *m_pPrivateKey;//PKCS12的证书私钥
	EVP_MD_CTX         m_md_ctx;
	
	char               m_sMethod[10];//签名等所用算法类型   "RSA"和SM2
	char               m_sHashMethod[10]; //hash算法类型，当为RSA才起作用，为"SHA-1" "SHA-256"
	char               m_sCertPwd[33];  //证书密码 
	char               m_sCertFilePath[300];//证书文件(含路径)
};

//注意：下面的定义在每个类里面都要有，主程序根据此方法获取本类的实例
extern "C" 
{ 
    DLL_EXPORT COpenApi * getInstance() ; 
}

#endif // !defined(AFX_OPENAPI_H__90C341F9_1F6D_4055_A3E0_2801AF81E54D__INCLUDED_)
