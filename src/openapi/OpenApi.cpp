// OpenApi.cpp: implementation of the COpenApi class.
//
//////////////////////////////////////////////////////////////////////

#include "OpenApi.h"

#ifdef WINDOWS
extern "C"
{
#include <applink.c>
};
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//注意：下面的定义在每个类里面都要有，主程序根据此方法获取本类的实例
//证书文件名为key.pfx

COpenApi * getInstance() 
{ 
    return new COpenApi(); 
}

COpenApi::COpenApi()
{
	
	bzero(m_errMsg,sizeof(m_errMsg));
	bzero(m_sPublicKey,sizeof(m_sPublicKey));
	m_nPublicKeyLen = 0;
	bzero(m_sMethod,sizeof(m_sMethod));
	bzero(m_sHashMethod,sizeof(m_sHashMethod));
	bzero(m_sCertPwd,sizeof(m_sCertPwd));
	bzero(m_sCertFilePath,sizeof(m_sCertFilePath));
	m_pPrivateKey = NULL;
}

COpenApi::~COpenApi()
{
	if (m_pPrivateKey != NULL)
	{
		EVP_PKEY_free(m_pPrivateKey);
		m_pPrivateKey = NULL;
	}

}
// 函数名: InitCertApi
// 编程  : 王明松 2013-5-24 11:05:29
// 返回  : virtual void 
// 描述  : 初始化证书api
bool COpenApi::InitCertApi(const char *certpath,char *certpwd)
{
	//取本地证书公钥，不用每次都取了
	
	strcpy(m_sMethod,"RSA");
#ifdef _WINDOWS
	sprintf(m_sCertFilePath,"%s\\key.pfx",certpath);
#else
	sprintf(m_sCertFilePath,"%s/key.pfx",certpath);
#endif
	strcpy(m_sCertPwd,certpwd);
	strcpy(m_sHashMethod,"SHA-256");
	int        nRet;                                                               
    FILE *     fp   = NULL;                                                           
    X509 *     x509 = NULL;                                                           
    PKCS12*     p12 = NULL;                                                           
    STACK_OF(X509) *ca = NULL;                                                        
    //初始化                                                                        
    SSLeay_add_all_algorithms();    
	OpenSSL_add_all_ciphers();
	
    ERR_load_crypto_strings();                                                        
    //读取个人信息证书并分解出密钥和证书   
	fp = fopen(m_sCertFilePath, "rb");
    if (fp == NULL)
	{    
		sprintf(m_errMsg,"读取证书文件[%s]出错，文件不存在",m_sCertFilePath);
        return false;                                                                 
    }                                                                                 
    d2i_PKCS12_fp(fp, &p12);                                                    
    fclose (fp); 
	fp =NULL;
    if (p12 == NULL) 
	{                                                                       
		sprintf(m_errMsg,"读取证书文件[%s]出错，非PKCS#12 file",m_sCertFilePath);
        return false;                                                                 
    }
	nRet = PKCS12_parse(p12, m_sCertPwd, &m_pPrivateKey, &x509, &ca);
    if (nRet != 1) 
	{                    
        sprintf(m_errMsg,"解析PKCS#12证书文件[%s]出错",m_sCertFilePath);                                              
        PKCS12_free(p12);                                                             
        return false;                                                                 
    }                                                                                 
    PKCS12_free(p12);  
	p12 = NULL;
    if (m_pPrivateKey == NULL) 
	{                                                               
        sprintf(m_errMsg,"解析PKCS#12证书文件[%s]出错,私钥为空",m_sCertFilePath);                                               
        return false;                                                                 
    }                                                                                 
    //签名初始设置     使用sha256摘要算法                                                                 
    EVP_SignInit(&m_md_ctx, EVP_sha256());
	//读取公钥，可备交换
	EVP_PKEY *publickey = X509_get_pubkey(x509);
	unsigned char *key = (unsigned char *)m_sPublicKey;
	m_nPublicKeyLen = i2d_PUBKEY(publickey, &key);

	m_sPublicKey[m_nPublicKeyLen]=0;
	
	X509_free(x509); 
	EVP_PKEY_free(publickey);
	return true;
}

// 函数名: UnInitCertApi
// 编程  : 王明松 2013-5-24 11:05:29
// 返回  : virtual void 
// 描述  : 退出证书API
bool COpenApi::UnInitCertApi()
{
	if (m_pPrivateKey != NULL)
	{
		EVP_PKEY_free(m_pPrivateKey);
		m_pPrivateKey = NULL;
	}
	return true;
}

// 函数名: Sign
// 编程  : 王明松 2013-5-24 11:07:02
// 返回  : virtual bool 
// 参数  : const char *signstr  要签名的字符串
// 参数  : unsigned short signstrlen 要签名的字符串长度
// 参数  : char *signres   签名结果
// 参数  : unsigned short &signreslen   签名结果长度
// 描述  : 签名，用本机的私钥签名
bool COpenApi::Sign(const char *signstr,unsigned short signstrlen,char *signres,unsigned short &signreslen)
{
	char *ssign=NULL;
	int sdatalen = signstrlen;
	//签名初始设置     使用sha256摘要算法
	EVP_SignInit(&m_md_ctx, EVP_sha256());  
	
    int nRet = EVP_SignUpdate (&m_md_ctx, signstr, signstrlen);    
	if (nRet != 1)
	{
		sprintf(m_errMsg,"EVP_SignUpdate出错 %d",nRet);
		return false;
	}
    unsigned int reslen = 128;                                                                    
    nRet = EVP_SignFinal(&m_md_ctx, (unsigned char *)signres, &reslen, m_pPrivateKey);                           
    if (nRet != 1) 
	{                                                                   
		sprintf(m_errMsg,"签名EVP_SignFinal出错 %d",nRet);                                                                            
        return false;                                                                 
    }   
	signreslen = reslen;
	return true;
}


// 函数名: GetPubicKey
// 编程  : 王明松 2013-5-25 11:22:50
// 返回  : virtual bool 
// 参数  : char *signkeybuf   签名公钥
// 参数  : unsigned short &signkeybuflen 签名公钥长度
// 参数  : char *encryptkkeybuf  加密公钥
// 参数  : unsigned short &encryptkkeybuflen 加密公钥长度
// 描述  : 取本机证书的公钥
bool COpenApi::GetPubicKey(char *signkeybuf,unsigned short &signkeybuflen,char *encryptkeybuf,unsigned short &encryptkkeybuflen)
{
	if (m_nPublicKeyLen <=0)
	{
		sprintf(m_errMsg,"未初始化");
		return false;
	}
	if (m_nPublicKeyLen >= signkeybuflen)
	{
		sprintf(m_errMsg,"签名证书空间不足 传入长度%d 公钥长度 %d",signkeybuflen,m_nPublicKeyLen);
		return false;
	}
	if (m_nPublicKeyLen >= encryptkkeybuflen)
	{
		sprintf(m_errMsg,"加密证书空间不足 传入长度%d 公钥长度 %d",encryptkkeybuflen,m_nPublicKeyLen);
		return false;
	}
	memcpy(signkeybuf,m_sPublicKey,m_nPublicKeyLen);
	signkeybuflen = m_nPublicKeyLen;
	signkeybuf[signkeybuflen]=0;

	memcpy(encryptkeybuf,m_sPublicKey,m_nPublicKeyLen);
	encryptkkeybuflen = m_nPublicKeyLen;
	encryptkeybuf[encryptkkeybuflen]=0;
	return true;
}

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
bool COpenApi::CheckSign(const char *publickey,unsigned short publickeylen,const char *signstr,unsigned short signstrlen,const char *signres,unsigned short signreslen)
{
	EVP_PKEY *certpubkey = NULL;
	const unsigned char *pkey = (unsigned char *)publickey;
	long llen = publickeylen;
	//从缓冲中读取公钥
	if (d2i_PUBKEY(&certpubkey,&pkey,llen) == NULL)
	{
		sprintf(m_errMsg,"调用d2i_PUBKEY从缓冲读取公钥失败");
		return false;
	}
	int nRet;                                                         
    EVP_MD_CTX     md_ctx;                                                            
    unsigned int sig_len = signreslen;  
	//摘要算法为sha256
    EVP_VerifyInit(&md_ctx, EVP_sha256());                                              
    EVP_VerifyUpdate(&md_ctx, signstr, signstrlen);  
	//验证签名
    nRet = EVP_VerifyFinal(&md_ctx, (const unsigned char *)signres, sig_len, certpubkey);          
    EVP_PKEY_free (certpubkey);                                                             
    if (nRet != 1) 
	{                                                                   
        sprintf(m_errMsg,"调用EVP_VerifyFinal验证签名失败 %d",nRet);                                                               
        return false;                                                                 
    }  
	//验证根证书等


 	return true;
}


// 函数名: Encrypt
// 编程  : 王明松 2013-5-25 11:27:12
// 返回  : virtual bool 
// 参数  : const char *publickey  加密公钥
// 参数  : unsigned short publickeylen 加密公钥长度
// 参数  : char *data   明文->加密数据
// 参数  : unsigned short &datalen   加密数长度，加密前为要加密的数据长度，加密后为加密码后的数据长度
// 描述  : 加密，用对方的公钥进行加密。仅在会话建立时使用。交易时使用会话密钥进行DES加密
bool COpenApi::Encrypt(const char *publickey,unsigned short publickeylen,char *data,unsigned short &datalen)
{
	//转为RSA私钥
	RSA *rsa=NULL;
	EVP_PKEY *certpubkey = NULL;
	const unsigned char *pkey = (unsigned char *)publickey;
	long llen = publickeylen;
	//从缓冲中读取公钥
	if (d2i_PUBKEY(&certpubkey,&pkey,llen) == NULL)
	{
		sprintf(m_errMsg,"调用d2i_PUBKEY从缓冲读取公钥失败");
		return false;
	}
	rsa = EVP_PKEY_get1_RSA(certpubkey);
//	rsa=d2i_RSAPublicKey(NULL,(const unsigned char**)&(certpubkey->pkey.ptr),publickeylen);
	if (rsa == NULL)
	{
		sprintf(m_errMsg,"取得RSA公钥调用EVP_PKEY_get1_RSA出错");
		return false;
	}
	unsigned char resbuffer[8192];
	bzero(resbuffer,sizeof(resbuffer));
	unsigned char tmpbuffer[129];
	int   tmplen =0;
	//按117个字节为一个单元进行加密
	int beginoffset=0;
	int destoffset=0;
	for (int i=0 ; i<datalen; )
	{
		if (datalen - beginoffset<118)
		{
			memcpy(tmpbuffer,data+beginoffset,datalen - beginoffset);
			tmplen = datalen - beginoffset;
		}
		else
		{
			memcpy(tmpbuffer,data+beginoffset,117);
			tmplen = 117;
		}
		beginoffset+=tmplen;
		int nRet = RSA_public_encrypt(tmplen,(const unsigned char *)tmpbuffer,resbuffer+destoffset,rsa,RSA_PKCS1_PADDING);
		if (nRet < 1)
		{
			sprintf(m_errMsg,"调用RSA_public_encrypt加密出错 %d",nRet);
			RSA_free(rsa);
			return false;
		}
		destoffset +=nRet;
		i = beginoffset;
	}
	datalen = destoffset;
	RSA_free(rsa);
	memcpy(data,resbuffer,datalen);
	data[datalen]=0;
	return true;
}


// 函数名: UnEncrypt
// 编程  : 王明松 2013-5-25 11:28:33
// 返回  : virtual bool 
// 参数  : char *data   加密数据->解密数据
// 参数  : unsigned short  &datalen 加密数据长度，解密前为加密的数据长度，解密后为数据明文长度
// 描述  : 用本机的私钥进行解密
bool COpenApi::UnEncrypt(char *data,unsigned short  &datalen)
{
	int nRet;
	unsigned char *res=NULL;
	int  reslen;
	

	//转为RSA私钥
	RSA *rsa=NULL;
	rsa = EVP_PKEY_get1_RSA(m_pPrivateKey);
	if (rsa == NULL)
	{
		sprintf(m_errMsg,"取得RSA私钥调用EVP_PKEY_get1_RSA出错");
		return false;
	}
	unsigned char resbuffer[8192];
	bzero(resbuffer,sizeof(resbuffer));
	unsigned char tmpbuffer[129];
	int   tmplen =0;
	//按128个字节为一个单元进行加密
	int beginoffset=0;
	int destoffset=0;
	for (int i=0 ; i<datalen; )
	{
		if (datalen - beginoffset<128)
		{
			memcpy(tmpbuffer,data+beginoffset,datalen - beginoffset);
			tmplen = datalen - beginoffset;
		}
		else
		{
			memcpy(tmpbuffer,data+beginoffset,128);
			tmplen = 128;
		}
		beginoffset+=tmplen;
		nRet = RSA_private_decrypt(tmplen,(const unsigned char *)tmpbuffer,resbuffer+destoffset,rsa,RSA_PKCS1_PADDING);
		if (nRet < 1)
		{
			sprintf(m_errMsg,"调用RSA_private_decrypt解密出错 %d",nRet);
			RSA_free(rsa);
			return false;
		}
		destoffset +=nRet;
		i = beginoffset;
	}
	datalen = destoffset;
	RSA_free(rsa);
	memcpy(data,resbuffer,datalen);
	data[datalen]=0;
	return true;
}



// 函数名: GetCertSerial
// 编程  : 王明松 2013-6-14 15:51:14
// 返回  : virtual bool 
// 参数  : const char *pubkey
// 参数  : unsigned short pubkeylen
// 参数  : char *serial
// 参数  : int &seriallen
// 描述  : 获取证唯一序号
bool COpenApi::GetCertSerial(const char *pubkey,unsigned short pubkeylen,char *serial,unsigned short &seriallen)
{
	char certname[30];
	strcpy(certname,"SerialNumber");
	char *res = NULL;
	
	res = NULL;
	return true;
}

std::string COpenApi::GetErrMsg()
{
	return m_errMsg;
}