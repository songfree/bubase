// ShcaApi.h: interface for the CShcaApi class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHCAAPI_H__CD835A24_5D17_4EE4_BECA_C6E625B4A11E__INCLUDED_)
#define AFX_SHCAAPI_H__CD835A24_5D17_4EE4_BECA_C6E625B4A11E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ICertApiBase.h"
#include "public.h"
#include "SafeEngine.h"


	// 函数名: 
	// 编程  : 王明松 2016-2-18 10:44:50
	// 返回  : long 
	// 参数  : HSE *hSE
	// 参数  : unsigned long privatekeydevicetype   存储私钥设备类型  2文件
	// 参数  : char *privatekeydeviceparameter      存储私钥设备参数  为2时为证书文件名
	// 参数  : char *privatekeypassword             证书密码 (私钥保护密码)
	// 参数  : unsigned long privatekeytimeout      私钥超时时间
	// 参数  : unsigned long certchaindevicetype    存储根证书设备的类型 2文件
	// 参数  : char *certchaindeviceparameter       存储根证书设备的参数  为2时为证书文件名
	// 参数  : char *certchainpassword              根证书密码  仅为ic卡时有效
	// 描述  : 初始化
typedef long (*SEH_InitialSessionPtr)(HSE *hSE, unsigned long privatekeydevicetype,  char *privatekeydeviceparameter, char *privatekeypassword,  unsigned long privatekeytimeout,  unsigned long certchaindevicetype,  char *certchaindeviceparameter,  char *certchainpassword );
typedef long (*SEH_InitialSessionExPtr)(HSE *hSE, unsigned char * privatekey, unsigned short privatekeylen, char *privatekeypassword, unsigned char *rootcert, unsigned short rootcertlen);

	// 函数名: 
	// 编程  : 王明松 2016-2-18 10:49:11
	// 返回  : long 
	// 参数  : ClearSessionPtr
	// 描述  : 清除环境变量
typedef long (*SEH_ClearSessionPtr)(HSE hSE);


	// 函数名: 
	// 编程  : 王明松 2016-2-18 10:50:56
	// 返回  : typedef long 
	// 参数  : HSE hSE
	// 参数  : unsigned char *origindata   原始数据块
	// 参数  : unsigned long length        原始数据长度
    // 参数  : unsigned short signmethod   签名方法  1: MD2   2: MD5   3: SHA1
	// 参数  : unsigned char *signeddata   签名结果 
	// 输入输出参数  : unsigned long *signedlength  签名结果长度
	// 描述  : 签名(用私钥签名，对端用本机的公钥验签)
typedef long (*SEH_SignDataPtr)(HSE hSE, unsigned char *origindata, unsigned long length, unsigned short signmethod, unsigned char *signeddata, unsigned long *signedlength);

	// 函数名: 
	// 编程  : 王明松 2016-2-18 10:51:30
	// 返回  : typedef long 
	// 参数  : HSE hSE
	// 参数  : unsigned char *origindata   原始数据块
	// 参数  : unsigned long originlength  原始数据长度
	// 参数  : unsigned short signmethod   签名方法  1: MD2   2: MD5   3: SHA1
	// 参数  : unsigned char *signeddata   签名结果
	// 参数  : unsigned long signedlength  签名结果长度
	// 参数  : unsigned char *certificate  公钥证书
	// 参数  : unsigned short certlength   公钥证书长度
	// 描述  : 用对端的公钥验证签名
typedef long (*SEH_VerifySignDataPtr)(HSE hSE, unsigned char *origindata, unsigned long originlength, unsigned short signmethod, unsigned char *signeddata, unsigned long signedlength, unsigned char *certificate, unsigned short certlength);


	// 函数名: 
	// 编程  : 王明松 2016-2-18 10:52:08
	// 返回  : typedef long 
	// 参数  : HSE hSE
	// 参数  : unsigned char *data   数据
	// 参数  : unsigned long length  数据长度
	// 参数  : unsigned short method  摘要方法  1: MD2   2: MD5   3: SHA1
	// 参数  : unsigned char *digest  摘要内容
	// 参数  : unsigned long  *digestlength  摘要长度
	// 描述  : 摘要
typedef long (*SEH_DigestPtr)(HSE hSE, unsigned char *data, unsigned long length, unsigned short method, unsigned char *digest, unsigned long  *digestlength);


	// 函数名: 
	// 编程  : 王明松 2016-2-18 10:52:19
	// 返回  : typedef long 
	// 参数  : HSE hSE
	// 参数  : unsigned short envolpetype   操作类型 1组成数字信封   2拆解数字信封
	// 参数  : unsigned char *indata        输入数据
	// 参数  : unsigned long inlength       输入数据长度
	// 参数  : unsigned char *outdata       输出数据
	// 参数  : unsigned long *outlength     输出数据长度
	// 参数  : unsigned char *certificate   证书内容   仅当加密时有用  为对端的公钥
	// 参数  : unsigned short certlength    证书长度
	// 描述  : 数字信封加密解密，加密使用对端的公钥，解密使用本端的私钥
typedef long (*SEH_EnvelopePtr)(HSE hSE, unsigned short envolpetype, unsigned char *indata, unsigned long inlength, unsigned char *outdata, unsigned long *outlength, unsigned char *certificate, unsigned short certlength);
typedef long (*SEH_EnvelopeExPtr)(HSE hSE, unsigned short envolpetype, unsigned char *indata, unsigned long inlength, unsigned char *outdata, unsigned long *outlength, unsigned char *certificate, unsigned short certlength, int AlgID);


	// 函数名: 
	// 编程  : 王明松 2016-2-18 10:52:50
	// 返回  : typedef long 
	// 参数  : HSE hSE
	// 参数  : unsigned long devicetype  设备类型 2为文件
	// 参数  : char *deviceparameter     设备参数 证书文件名
	// 参数  : char *password            证书密码
	// 参数  : unsigned char *certificate    证书内容
	// 输入输出参数  : unsigned short *certificatelength    证书内容长度
	// 描述  : 取证书内容  证书可为公钥证书或私钥证书
typedef long (*SEH_GetSelfCertificatePtr)(HSE hSE, unsigned long devicetype, char *deviceparameter, char *password, unsigned char *certificate, unsigned short *certificatelength);


	// 函数名: 
	// 编程  : 王明松 2016-2-18 11:22:34
	// 返回  : typedef long 
	// 参数  : HSE hSE
	// 参数  : unsigned char *certificate   证书内容
	// 参数  : unsigned short length        证书长度
	// 描述  : 验证证书有效性，先验有效期，ca签发，根据配置决定是否验证本地黑名单，ca黑名单
typedef long (*SEH_VerifyCertificatePtr)(HSE hSE, unsigned char *certificate, unsigned short length);

	// 函数名: 
	// 编程  : 王明松 2016-2-18 11:24:12
	// 返回  : typedef long 
	// 参数  : WINAPI *SEH_GetCertificatePtr)(HSE hSE
	// 参数  : char *SerialNo  证书序列号  16进制表示
	// 参数  : char *URL       证书查询URL   格式: IP : PORT
	// 参数  : unsigned char *certificate   证书内容
	// 输入输出参数  : unsigned short *certlen      证书内容长度
	// 描述  : 从网络获取证书
typedef long (*SEH_GetCertificatePtr)(HSE hSE, char *SerialNo, char *URL, unsigned char *certificate, unsigned short *certlen);


	// 函数名: 
	// 编程  : 王明松 2016-2-18 11:26:10
	// 返回  : typedef long 
	// 参数  : HSE hSE
	// 参数  : unsigned char *certificate   证书内容
	// 参数  : unsigned short length        证书长度
	// 描述  : 在线验证证书有效性，先验有效期，ca签发，然后ocsp
typedef long (*SEH_VerifyCertificateOnlinePtr)(HSE hSE, unsigned char *certificate, unsigned short length);

	// 函数名: 
	// 编程  : 王明松 2016-2-18 13:05:29
	// 返回  : typedef long 
	// 参数  :  HSE hSE
	// 参数  : unsigned long device
	// 参数  : char *deviceparameter
	// 参数  : char *oldpassword  老密码
	// 参数  : char *newpassword  新密码
	// 描述  : 更改存储介质的密码
typedef long (*SEH_ChangePasswordPtr)(HSE hSE, unsigned long device, char *deviceparameter, char *oldpassword, char *newpassword);

//typedef long (*SEH_GetCertInfoPtr)(HSE hSE, unsigned char *DerCert, unsigned short DerCertLen, CERT_INFO *pCertInfo);

	// 函数名: 
	// 编程  : 王明松 2016-2-18 10:56:15
	// 返回  : typedef long 
	// 参数  : HSE hSE
	// 参数  : unsigned char *DerCert      待解码的证书的DER编码串的起始地址
	// 参数  : unsigned short DerCertLen   待解码的证书的DER编码串的长度
	// 参数  : unsigned short ItemNo  项目 id
	// 参数  : unsigned char *ItemValue  项目值
	// 输入输出参数  : unsigned short *ItemLength  项目值长度
	// 描述  : 获取证书细目   
typedef long (*SEH_GetCertDetailPtr)(HSE hSE, unsigned char *DerCert, unsigned short DerCertLen, unsigned short ItemNo, unsigned char *ItemValue, unsigned short *ItemLength);


	// 函数名: 
	// 编程  : 王明松 2016-2-18 11:49:34
	// 返回  : typedef long 
	// 参数  : HSE hSE
	// 参数  : unsigned char *indata    数据
	// 参数  : unsigned long indatalen  数据长度
	// 参数  : unsigned char *pemdata   输出PEM数据
	// 输入输出参数  : unsigned long *pemlen    PEM数据长度
	// 描述  : PEM编码
typedef long (*SEH_PEMEncodePtr)(HSE hSE, unsigned char *indata, unsigned long indatalen, unsigned char *pemdata, unsigned long *pemlen);

	// 函数名: 
	// 编程  : 王明松 2016-2-18 11:49:39
	// 返回  : typedef long 
	// 参数  : HSE hSE
	// 参数  : unsigned char *pemdata  输入PEM数据
	// 参数  : unsigned long pemlen    pem数据长度
	// 参数  : unsigned char *outdata   输出数据
	// 输入输出参数  : unsigned long *outlen    数据长度
	// 描述  : PEM解码
typedef long (*SEH_PEMDecodePtr)(HSE hSE, unsigned char *pemdata, unsigned long pemlen, unsigned char *outdata, unsigned long *outlen);


	// 函数名: 
	// 编程  : 王明松 2016-2-18 11:55:19
	// 返回  : typedef long 
	// 参数  : HSE hSE
    // 参数  : long mode   Bit 0 = 1:验证书有效性时检查CA的黑名单库.Bit 0 = 0: 不验证黑名单.Bit1 = 1:OCSP验证证书时对证书链也用ocsp验证.Bit1 = 0: OCSP验证证书时对证书链不做是否废除验证.Bit 1 - Bit 31: 保留.缺省设置不检查黑名单
	// 描述  : 设置配置  用来设置一些参数的选项.  如验证证书时是否验证黑名单
typedef long (*SEH_SetConfigurationPtr)(HSE hSE, long mode);



	// 函数名: 
	// 编程  : 王明松 2016-2-18 11:57:39
	// 返回  : typedef long 
	// 参数  : HSE hSE
	// 参数  : unsigned char *randombytes   产生的随机数数据
	// 参数  : int randombyteslen           要产生的随机数长度
	// 描述  : 产生随机密钥
typedef long (*SEH_GenRandomBytesPtr)(HSE hSE, unsigned char *randombytes, int randombyteslen);

	// 函数名: 
	// 编程  : 王明松 2016-2-18 11:58:17
	// 返回  : typedef long 
	// 参数  : HSE hSE
	// 参数  : unsigned char *indata   要加密的数据
	// 参数  : int indatalen           要加密的数据长度
	// 参数  : unsigned char *outdata   密文数据
	// 输入输出参数  : int *outdatalen          密文数据长度
	// 参数  : unsigned char *sdbikey   加密密钥  16字节长度
	// 描述  : 用对称算法（SDBI）加密应用数据
typedef long (*SEH_EncryptDataPtr)(HSE hSE, unsigned char *indata, int indatalen, unsigned char *outdata, int *outdatalen, unsigned char *sdbikey);

	// 函数名: 
	// 编程  : 王明松 2016-2-18 12:41:38
	// 返回  : typedef long 
	// 参数  : HSE hSE
	// 参数  : unsigned char *indata  密文数据
	// 参数  : int indatalen          密文数据长度
	// 参数  : unsigned char *outdata   数据
	// 输入输出参数  : int *outdatalen          数据长度
	// 参数  : unsigned char *sdbikey  加密密钥  16字节长度
	// 描述  : 用对称算法（SDBI）解密应用数据
typedef long (*SEH_DecryptDataPtr)(HSE hSE, unsigned char *indata, int indatalen, unsigned char *outdata, int *outdatalen, unsigned char *sdbikey);
typedef long (*SEH_EncryptDataExPtr)(HSE hSE, unsigned char *indata, int indatalen, unsigned char *outdata, int *outdatalen, unsigned char *key, int keylen, int AlgID);
typedef long (*SEH_DecryptDataExPtr)(HSE hSE, unsigned char *indata, int indatalen, unsigned char *outdata, int *outdatalen, unsigned char *key, int keylen, int AlgID);

	// 函数名: 
	// 编程  : 王明松 2016-2-18 12:43:31
	// 返回  : typedef long 
	// 参数  : HSE hSE
	// 参数  : unsigned char * pcDERCert  Der编码的证书
	// 参数  : unsigned long lCertSize    证书长度
	// 参数  : char * pcPassword          证书密码
	// 参数  : unsigned char * pcP12Buffer    PKCS12格式数据
	// 输入输出参数  : unsigned long * plP12Size      PKCS12格式数据长度
	// 描述  : 将证书，私钥编码成pkcs12格式
typedef long (*SEH_EncodePKCS12Ptr)(HSE hSE,unsigned char * pcDERCert,unsigned long lCertSize,char * pcPassword,unsigned char * pcP12Buffer,unsigned long * plP12Size);


	// 函数名: 
	// 编程  : 王明松 2016-2-18 12:46:22
	// 返回  : typedef long 
	// 参数  : HSE hSE
	// 参数  : unsigned char *cert  待解码的证书的DER编码串的起始地址
	// 参数  : unsigned short certlen    长度
	// 参数  : char *uniqueid             唯一标识
	// 输入输出参数  : unsigned short *idlen    输入为缓冲大小 输出为唯一标识长度
	// 描述  : 取证书唯一标识
typedef long (*SEH_GetCertUniqueIDPtr)(HSE hSE, unsigned char *cert, unsigned short certlen, char *uniqueid, unsigned short *idlen);

	// 函数名: 
	// 编程  : 王明松 2016-2-18 12:51:38
	// 返回  : typedef long 
	// 参数  : HSE hSE
	// 参数  : unsigned char *cert    证书内容
	// 参数  : unsigned short certlen   证书内容大小
	// 参数  : char *oid      Sheca定义的扩展项id
	// 参数  : char *info     信息内容
	// 参数  : unsigned short *infolen   信息长度
	// 描述  : 取证书扩展项信息
typedef long (*SEH_GetCertInfoByOIDPtr)(HSE hSE, unsigned char *cert, unsigned short certlen, char *oid, char *info, unsigned short *infolen);


	// 函数名: 
	// 编程  : 王明松 2016-2-18 12:53:11
	// 返回  : typedef long 
	// 参数  : WINAPI *SEH_ShowVersionPtr)(HSE hSE
	// 参数  : long *plModualNo   模块号
	// 参数  : long *plVersion    版本号
	// 描述  : 获取API当前版本信息
typedef long (*SEH_ShowVersionPtr)(HSE hSE, long *plModualNo, long *plVersion);


	// 函数名: 
	// 编程  : 王明松 2016-2-18 12:54:09
	// 返回  : typedef long 
	// 参数  : HSE hSE
	// 参数  : unsigned char *cert   证书内容
	// 参数  : unsigned short certlen  证书内容长度
	// 参数  : long *datanum    有效天数
	// 描述  : 获取证书剩余的有效天数
typedef long (*SEH_GetCertValidDatePtr)(HSE hSE, unsigned char *cert, unsigned short certlen, long *datanum);

	// 函数名: 
	// 编程  : 王明松 2016-2-18 12:55:15
	// 返回  : typedef long 
	// 参数  : HSE hSE
	// 参数  : unsigned char *cert
	// 参数  : unsigned short certlen
	// 参数  : unsigned long *lCertClass  个人身份证书  0x00000103 个人EMAIL证书	 0x00000102 个人代码签名证书	 0x00000105 单位身份证书		 0x00000203 单位EAIL证书   0x00000202 单位代码签名证书	 0x00000204 服务器身份证书   0x00000304 WEB服务器证书  0x00000301 
	// 描述  : 取证书级别
typedef long (*SEH_GetCertClassPtr)(HSE hSE, unsigned char *cert, unsigned short certlen, unsigned long *lCertClass);

	// 函数名: 
	// 编程  : 王明松 2016-2-18 13:01:10
	// 返回  : typedef long 
	// 参数  : WINAPI *SEH_GetCertUsagePtr)(HSE hSE
	// 参数  : unsigned char *cert
	// 参数  : unsigned short certlen
	// 参数  : unsigned long *lKeyUsage
	// 描述  : 取证书作途
typedef long (*SEH_GetCertUsagePtr)(HSE hSE, unsigned char *cert, unsigned short certlen, unsigned long *lKeyUsage);

typedef long (*SEH_GetDeviceListPtr)(HSE hSE, int *totalDevNum, unsigned long *devType);
typedef long (*SEH_GetDeviceInfoPtr)(HSE hSE, unsigned long devType, DEVICE_INFO *pdevInfo);
typedef long (*SEH_GetDeviceTypePtr)(HSE hSE, unsigned short ReaderID, unsigned short CardType, unsigned long *devType);

 

typedef struct _SHCA_ENTRYPOINTS
{
    SEH_InitialSessionPtr                SEH_InitialSession; //初始化
    SEH_ClearSessionPtr                  SEH_ClearSession;//清除环境
	SEH_SignDataPtr                      SEH_SignData;    //签名
	SEH_VerifySignDataPtr                SEH_VerifySignData;//校验签名
	SEH_DigestPtr                        SEH_Digest; //摘要
	SEH_EnvelopePtr                      SEH_Envelope;   //数据信封
	SEH_GetSelfCertificatePtr            SEH_GetSelfCertificate;//取证书
	SEH_VerifyCertificatePtr             SEH_VerifyCertificate;//校验证书
	SEH_ChangePasswordPtr                SEH_ChangePassword;//改密码
	SEH_GetCertDetailPtr                 SEH_GetCertDetail;//取证书细目
	SEH_PEMEncodePtr                     SEH_PEMEncode;//pem编码
	SEH_PEMDecodePtr                     SEH_PEMDecode;//pem解码
	SEH_SetConfigurationPtr              SEH_SetConfiguration;//配置参数
	SEH_GenRandomBytesPtr                SEH_GenRandomBytes;//产生随机密钥
	SEH_EncryptDataPtr                   SEH_EncryptData;//对称加密
	SEH_DecryptDataPtr                   SEH_DecryptData;//对称解密
	SEH_EncodePKCS12Ptr                  SEH_EncodePKCS12;//将证书，私钥编码成pkcs12格式
	SEH_GetCertUniqueIDPtr               SEH_GetCertUniqueID;//取证书唯一标识
	SEH_GetCertInfoByOIDPtr              SEH_GetCertInfoByOID;//取证书扩展项信息
	SEH_ShowVersionPtr                   SEH_ShowVersion;//获取api版本
	SEH_GetCertValidDatePtr              SEH_GetCertValidDate;//取证书有效天数
	SEH_GetCertClassPtr                  SEH_GetCertClass;//取证收级别
	SEH_GetCertUsagePtr                  SEH_GetCertUsage;//取证书用途
}SHCA_ENTRYPOINTS, *LPSHCA_ENTRYPOINTS;


class DLL_EXPORT CShcaApi : public CICertApiBase  
{
public:
	CShcaApi();
	virtual ~CShcaApi();

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
	SHCA_ENTRYPOINTS   m_pShcaPoint;
	HSE                m_hSE;               //上海CA
	DLLHANDLE          m_pDllHandle;
	char               m_errMsg[256];

	char               m_sPublicKey[4096];
	unsigned short     m_nPublicKeyLen;
	

	char               m_sCertPwd[33];  //证书密码 
	char               m_sCertFilePath[300];//私钥证书文件(含路径) UserKey.key
	char               m_sPubFilePath[300]; //公钥证书文件(含路径) UserCert.der
	char               m_sRootFilePath[300];//证书链文件(含路径)   RootCert.der
};

//注意：下面的定义在每个类里面都要有，主程序根据此方法获取本类的实例
extern "C" 
{ 

	// 函数名: * getInstance
	// 编程  : 王明松 2016-2-18 16:25:38
	// 返回  : DLL_EXPORT CShcaApi 
	// 描述  : 得到一个api实例
    DLL_EXPORT CShcaApi * getInstance() ;

	// 函数名: InitCertApi
	// 编程  : 王明松 2013-5-24 11:05:29
	// 返回  : int  >=0成功 <0失败   成功则为实例的序号，下次使用要将序号传入
	// 参数  : const char *certpath  证书目录 最后不带"/"
	// 参数  : char *certpwd  证书密码
	// 参数  : int *apipoint  api实例的序号，多线程使用时每个线程一个实例
	// 描述  : 初始化证书api
	DLL_EXPORT int SHCAAPI_InitCertApi(const char *certpath,const char *certpwd,int *apipoint) ;
	
	// 函数名: UnInitCertApi
	// 编程  : 王明松 2013-5-24 11:05:29
	// 返回  : int   0成功  
	// 描述  : 退出证书API
	DLL_EXPORT int SHCAAPI_UnInitCertApi(int apipoint) ;
	
	// 函数名: Sign
	// 编程  : 王明松 2013-5-24 11:07:02
	// 返回  : int   0成功  
	// 参数  : int apipoint    api的实例序号
	// 参数  : const char *signstr  要签名的字符串
	// 参数  : unsigned short signstrlen 要签名的字符串长度
	// 参数  : char *signres   签名结果
	// 参数  : unsigned short *signreslen   签名结果长度
	// 描述  : 签名，用本机的私钥签名
	DLL_EXPORT int SHCAAPI_Sign(int apipoint,const char *signstr,unsigned short signstrlen,char *signres,unsigned short *signreslen);
	
	
	// 函数名: GetPubicKey
	// 编程  : 王明松 2013-5-25 11:22:50
	// 返回  : int   0成功 
	// 参数  : int apipoint    api的实例序号
	// 参数  : char *signkeybuf   签名公钥
	// 参数  : unsigned short *signkeybuflen 签名公钥长度
	// 参数  : char *encryptkkeybuf  加密公钥
	// 参数  : unsigned short *encryptkkeybuflen 加密公钥长度
	// 描述  : 取本机证书的公钥
	DLL_EXPORT int SHCAAPI_GetPubicKey(int apipoint,char *signkeybuf,unsigned short *signkeybuflen,char *encryptkeybuf,unsigned short *encryptkkeybuflen);
	
	// 函数名: CheckSign
	// 编程  : 王明松 2013-5-30 10:43:56
	// 返回  : int   0成功 
	// 参数  : int apipoint    api的实例序号
	// 参数  : const char *publickey   签名公钥
	// 参数  : unsigned short publickeylen   签名公钥长度
	// 参数  : const char *signstr      签名的原始字符串
	// 参数  : unsigned short signstrlen  签名的原始字符串长度
	// 参数  : const char *signres   签名结果  注意要有结束符 0
	// 参数  : int signreslen 签名结果长度
	// 描述  : 校验签名，并校验证书是否一个根证书链的
	DLL_EXPORT int SHCAAPI_CheckSign(int apipoint,const char *publickey,unsigned short publickeylen,const char *signstr,unsigned short signstrlen,const char *signres,unsigned short signreslen);
	
	
	// 函数名: Encrypt
	// 编程  : 王明松 2013-5-25 11:27:12
	// 返回  : int   0成功  
	// 参数  : int apipoint    api的实例序号
	// 参数  : const char *publickey  加密公钥
	// 参数  : unsigned short publickeylen 加密公钥长度
	// 参数  : char *data   明文->加密数据
	// 参数  : unsigned short *datalen   加密数长度，加密前为要加密的数据长度，加密后为加密码后的数据长度
	// 描述  : 加密，用对方的公钥进行加密。仅在会话建立时使用。交易时使用会话密钥进行DES加密
	DLL_EXPORT int SHCAAPI_Encrypt(int apipoint,const char *publickey,unsigned short publickeylen,char *data,unsigned short *datalen);
	
	
	// 函数名: UnEncrypt
	// 编程  : 王明松 2013-5-25 11:28:33
	// 返回  : int   0成功 
	// 参数  : int apipoint    api的实例序号
	// 参数  : char *data   加密数据->解密数据
	// 参数  : unsigned short  *datalen 加密数据长度，解密前为加密的数据长度，解密后为数据明文长度
	// 描述  : 用本机的私钥进行解密
	DLL_EXPORT int SHCAAPI_UnEncrypt(int apipoint,char *data,unsigned short  *datalen);
	
	
	
	// 函数名: GetCertSerial
	// 编程  : 王明松 2013-6-14 15:51:14
	// 返回  : int   0成功 
	// 参数  : int apipoint    api的实例序号
	// 参数  : const char *pubkey
	// 参数  : unsigned short pubkeylen
	// 参数  : char *serial
	// 参数  : int *seriallen
	// 描述  : 获取证唯一序号
	DLL_EXPORT int SHCAAPI_GetCertSerial(int apipoint,const char *pubkey,unsigned short pubkeylen,char *serial,unsigned short *seriallen);
	
	
	// 函数名: GetErrMsg
	// 编程  : 王明松 2014-4-10 15:44:35
	// 返回  : int  0成功
	// 参数  : int apipoint    api的实例序号
	// 描述  : 取出错信息
	DLL_EXPORT int SHCAAPI_GetErrMsg(int apipoint,char *errmsg);
}

#endif // !defined(AFX_SHCAAPI_H__CD835A24_5D17_4EE4_BECA_C6E625B4A11E__INCLUDED_)
