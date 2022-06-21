// CfcaApi.h: interface for the CCfcaApi class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CFCAAPI_H__90C341F9_1F6D_4055_A3E0_2801AF81E54D__INCLUDED_)
#define AFX_CFCAAPI_H__90C341F9_1F6D_4055_A3E0_2801AF81E54D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ICertApiBase.h"
#include "public.h"

#ifdef _WINDOWS

#else
#define WINAPI
#endif

typedef int  (WINAPI *InitializePtr)();
typedef int  (WINAPI *UninitializePtr)();

	// 函数名:  SignData_PKCS1Ptr
	// 编程  : 王明松 2014-4-10 16:53:19
	// 返回  : int 
	// 参数  : char* method            签名方法 "SM2"或"RSA"
	// 参数  : unsigned char* sdata    签名原文数据
	// 参数  : int sdatalen            签名原文长度
	// 参数  : char* signcertfilepath  签名证书文件及路径
	// 参数  : char* certpwd           签名证书密码
	// 参数  : char* hashmethod        哈希算法 仅对RSA有用，传入 SHA-1/SHA-256两种
	// 参数  : char** signresult       签名结果
	// 描述  : 对数据进行PKCS#1签名
typedef int  (WINAPI *SignData_PKCS1Ptr)(char* method, unsigned char* sdata, int sdatalen, char* signcertfilepath, char* certpwd, char* hashmethod, char** signresult);

// 函数名:  SignData_PKCS7AttachedPtr
// 编程  : 王明松 2014-4-10 16:53:19
// 返回  : int 
// 参数  : char* method            签名方法 "SM2"或"RSA"
// 参数  : unsigned char* sdata    签名原文数据
// 参数  : int sdatalen            签名原文长度
// 参数  : char* signcertfilepath  签名证书文件及路径
// 参数  : char* certpwd           签名证书密码
// 参数  : char* hashmethod        哈希算法 仅对RSA有用，传入 SHA-1/SHA-256两种
// 参数  : char** signresult       签名结果
// 描述  : 对数据进行PKCS#7带原文签名
typedef int  (WINAPI *SignData_PKCS7AttachedPtr)(char* method, unsigned char* sdata, int sdatalen, char* signcertfilepath, char* certpwd, char* hashmethod, char** signresult);

// 函数名:  SignData_PKCS7AttachedPtr
// 编程  : 王明松 2014-4-10 16:53:19
// 返回  : int 
// 参数  : char* method            签名方法 "SM2"或"RSA"
// 参数  : unsigned char* sdata    签名原文数据
// 参数  : int sdatalen            签名原文长度
// 参数  : char* signcertfilepath  签名证书文件及路径
// 参数  : char* certpwd           签名证书密码
// 参数  : char* hashmethod        哈希算法 仅对RSA有用，传入 SHA-1/SHA-256两种
// 参数  : char** signresult       签名结果
// 描述  : 对数据进行PKCS#7不带原文签名
typedef int  (WINAPI *SignData_PKCS7DetachedPtr)(char* method, unsigned char* sdata, int sdatalen, char* signcertfilepath, char* certpwd, char* hashmethod, char** signresult);

typedef int  (WINAPI *SignFile_PKCS7DetachedPtr)(char*, char*, char*, char*, char*, char**);

// 函数名:  VerifyDataSignature_PKCS1Ptr
// 编程  : 王明松 2014-4-14 15:19:43
// 返回  : int 
// 参数  : char* method            签名方法 "SM2"或"RSA"
// 参数  : unsigned char* sdata    签名原文数据
// 参数  : int sdatalen            签名原文长度
// 参数  : char* publickey         校验所用公钥
// 参数  : char* hashmethod        哈希算法 仅对RSA有用，传入 SHA-1/SHA-256两种
// 参数  : char* pkcssign          pkcs签名结果
// 描述  : 对数据签名进行验证
typedef int  (WINAPI *VerifyDataSignature_PKCS1Ptr)(char* method, unsigned char* sdata, int sdatalen, char* publickey, char* hashmethod, char* pkcssign);

// 函数名:  VerifyDataSignature_PKCS7DetachedPtr
// 编程  : 王明松 2014-4-14 15:19:43
// 返回  : int 
// 参数  : char* method            签名方法 "SM2"或"RSA"
// 参数  : unsigned char* sdata    签名原文数据
// 参数  : int sdatalen            签名原文长度
// 参数  : char* publickey         校验所用公钥
// 参数  : char* hashmethod        哈希算法 仅对RSA有用，传入 SHA-1/SHA-256两种
// 参数  : char* pkcssign          pkcs签名结果
// 描述  : 对数据签名进行验证
typedef int  (WINAPI *VerifyDataSignature_PKCS7DetachedPtr)(char* method, unsigned char* sdata, int sdatalen, char* pkcssign, char** publickey);
typedef int  (WINAPI *VerifyDataSignature_PKCS7AttachedPtr)(char*, char*, char**, unsigned char**, int*);
typedef int  (WINAPI *VerifyFileSignature_PKCS7DetachedPtr)(char*, char*, char*, char**);


// 函数名:  EncryptDataToCMSEnvelopePtr
// 编程  : 王明松 2014-4-14 15:30:34
// 返回  : int 
// 参数  : char* method            方法 "SM2"或"RSA"
// 参数  : unsigned char*  sdata   明文数据
// 参数  : int    sdatalen         明文数据长度
// 参数  : char*  publickey        公钥
// 参数  : char*  encryptmethod    加密方法 对称加密算法，传入“RC4”或“3DES”
// 参数  : char**  encryptresult   加密结果
// 描述  : 公钥加密
typedef int  (WINAPI *EncryptDataToCMSEnvelopePtr)(char* method, unsigned char* sdata, int sdatalen, char* publickey, char* encryptmethod, char** encryptresult);
typedef int  (WINAPI *SignAndEncryptDataPtr)(char*, unsigned char*, int, char*, char*, char*, char*, char*, char**);
// 函数名:  EncryptDataToCMSEnvelopePtr
// 编程  : 王明松 2014-4-14 15:30:34
// 返回  : int 
// 参数  : char* method            方法 "SM2"或"RSA"
// 参数  : char*  sdata            密文数据  
// 参数  : char*  certfilepath     证书文件
// 参数  : char*  certpwd          证书密码
// 参数  : unsigned char**  resdata          明文结果
// 参数  : int *  resdatalen       明文结果长度
// 描述  : 数据解密
typedef int  (WINAPI *DecryptDataFromCMSEnvelopePtr)(char* method, char* sdata, char* certfilepath, char* certpwd, unsigned char** resdata, int* resdatalen);

typedef int  (WINAPI *DecryptAndVerifyDataSignaturePtr)(char*, char*, char*, char*, unsigned char**, int*);
typedef int  (WINAPI *VerifyCertificatePtr)(char*, int, char*, char*);

// 函数名:  GetCertificateInfoPtr
// 编程  : 王明松 2014-4-14 15:54:19
// 返回  : int 
// 参数  : char*   证书内容  公钥
// 参数  : char*   证书信息名称  
// 参数  : char**  证书信息
// 描述  : 取得证书信息
typedef int  (WINAPI *GetCertificateInfoPtr)(char* publickey, char*  certname, char** certresult);
typedef int  (WINAPI *GetPublicCertFromPFXPtr) (char* method, char* certfilepath, char* certpwd, char** pubkeybuf);
typedef void (WINAPI *FreeMemoryPtr)(void* ptr);

typedef struct _SADK_ENTRYPOINTS
{
    InitializePtr                        Initialize;
    UninitializePtr                      Uninitialize;
    SignData_PKCS1Ptr                    SignData_PKCS1;
    SignData_PKCS7DetachedPtr            SignData_PKCS7Detached;
    SignData_PKCS7AttachedPtr            SignData_PKCS7Attached;
    SignFile_PKCS7DetachedPtr            SignFile_PKCS7Detached;
    VerifyDataSignature_PKCS1Ptr         VerifyDataSignature_PKCS1;
    VerifyDataSignature_PKCS7DetachedPtr VerifyDataSignature_PKCS7Detached;
    VerifyDataSignature_PKCS7AttachedPtr VerifyDataSignature_PKCS7Attached;
    VerifyFileSignature_PKCS7DetachedPtr VerifyFileSignature_PKCS7Detached;
    EncryptDataToCMSEnvelopePtr          EncryptDataToCMSEnvelope;
    SignAndEncryptDataPtr                SignAndEncryptData;
    DecryptDataFromCMSEnvelopePtr        DecryptDataFromCMSEnvelope;
    DecryptAndVerifyDataSignaturePtr     DecryptAndVerifyDataSignature;
    VerifyCertificatePtr                 VerifyCertificate;
    GetCertificateInfoPtr                GetCertificateInfo;
    FreeMemoryPtr                        FreeMemory;
    GetPublicCertFromPFXPtr              GetPublicCertFromPFX;
}SADK_ENTRYPOINTS, *LPSADK_ENTRYPOINTS;


//使用rsa算法   pfx文件

class DLL_EXPORT CCfcaApi : public CICertApiBase  
{
public:
	CCfcaApi();
	virtual ~CCfcaApi();


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
	SADK_ENTRYPOINTS   m_pCfcaPoint;

	DLLHANDLE          m_pDllHandle;
	char               m_errMsg[256];
	char               m_sPublicKey[4096];
	int                m_nPublicKeyLen;
	
	char               m_sMethod[10];//签名等所用算法类型   "RSA"和SM2
	char               m_sHashMethod[10]; //hash算法类型，当为RSA才起作用，为"SHA-1" "SHA-256"
	char               m_sCertPwd[33];  //证书密码 
	char               m_sCertFilePath[300];//证书文件(含路径)
};

//注意：下面的定义在每个类里面都要有，主程序根据此方法获取本类的实例
extern "C" 
{ 
    DLL_EXPORT CCfcaApi * getInstance() ; 
}

#endif // !defined(AFX_CFCAAPI_H__90C341F9_1F6D_4055_A3E0_2801AF81E54D__INCLUDED_)
