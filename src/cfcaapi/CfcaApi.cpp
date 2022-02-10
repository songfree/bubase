// CfcaApi.cpp: implementation of the CCfcaApi class.
//
//////////////////////////////////////////////////////////////////////

#include "CfcaApi.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//注意：下面的定义在每个类里面都要有，主程序根据此方法获取本类的实例
//证书文件名为key.pfx

CCfcaApi * getInstance() 
{ 
    return new CCfcaApi(); 
}

CCfcaApi::CCfcaApi()
{
	m_pDllHandle = NULL;
	bzero(m_errMsg,sizeof(m_errMsg));
	bzero(&m_pCfcaPoint,sizeof(SADK_ENTRYPOINTS));
	bzero(m_sPublicKey,sizeof(m_sPublicKey));
	m_nPublicKeyLen = 0;
	bzero(m_sMethod,sizeof(m_sMethod));
	bzero(m_sHashMethod,sizeof(m_sHashMethod));
	bzero(m_sCertPwd,sizeof(m_sCertPwd));
	bzero(m_sCertFilePath,sizeof(m_sCertFilePath));
}

CCfcaApi::~CCfcaApi()
{
	if (m_pCfcaPoint.Uninitialize != NULL)
	{
		m_pCfcaPoint.Uninitialize();
	}
	bzero(&m_pCfcaPoint,sizeof(SADK_ENTRYPOINTS));
	if (m_pDllHandle!=NULL)
	{
		FREEDLL(m_pDllHandle);
		m_pDllHandle = NULL;
	}
}
// 函数名: InitCertApi
// 编程  : 王明松 2013-5-24 11:05:29
// 返回  : virtual void 
// 描述  : 初始化证书api
bool CCfcaApi::InitCertApi(const char *certpath,char *certpwd)
{
	std::string dllname;
#ifdef _WINDOWS
	dllname = "SADK.Standard.x86.dll";
	
#else
	dllname = "libSADK_Standard.so";
	
#endif	

	m_pDllHandle = LOADDLL(dllname.c_str());
	if (ISNOTOPEN(m_pDllHandle))
	{
		sprintf(m_errMsg,"打开动态库[%s]失败",dllname.c_str());
		return false;
	}
	m_pCfcaPoint.Initialize = (InitializePtr)GETINSTANCE(m_pDllHandle, "Initialize"); 
    m_pCfcaPoint.Uninitialize = (UninitializePtr)GETINSTANCE(m_pDllHandle, "Uninitialize"); 
    m_pCfcaPoint.SignData_PKCS1 = (SignData_PKCS1Ptr)GETINSTANCE(m_pDllHandle, "SignData_PKCS1"); 
    m_pCfcaPoint.SignData_PKCS7Detached = (SignData_PKCS7DetachedPtr)GETINSTANCE(m_pDllHandle, "SignData_PKCS7Detached"); 
    m_pCfcaPoint.SignData_PKCS7Attached = (SignData_PKCS7AttachedPtr)GETINSTANCE(m_pDllHandle, "SignData_PKCS7Attached"); 
    m_pCfcaPoint.SignFile_PKCS7Detached = (SignFile_PKCS7DetachedPtr)GETINSTANCE(m_pDllHandle, "SignFile_PKCS7Detached");
    m_pCfcaPoint.VerifyDataSignature_PKCS1 = (VerifyDataSignature_PKCS1Ptr)GETINSTANCE(m_pDllHandle, "VerifyDataSignature_PKCS1"); 
    m_pCfcaPoint.VerifyDataSignature_PKCS7Detached = (VerifyDataSignature_PKCS7DetachedPtr)GETINSTANCE(m_pDllHandle, "VerifyDataSignature_PKCS7Detached"); 
    m_pCfcaPoint.VerifyDataSignature_PKCS7Attached = (VerifyDataSignature_PKCS7AttachedPtr)GETINSTANCE(m_pDllHandle, "VerifyDataSignature_PKCS7Attached"); 
    m_pCfcaPoint.VerifyFileSignature_PKCS7Detached = (VerifyFileSignature_PKCS7DetachedPtr)GETINSTANCE(m_pDllHandle, "VerifyFileSignature_PKCS7Detached");
    m_pCfcaPoint.EncryptDataToCMSEnvelope = (EncryptDataToCMSEnvelopePtr)GETINSTANCE(m_pDllHandle, "EncryptDataToCMSEnvelope"); 
    m_pCfcaPoint.SignAndEncryptData = (SignAndEncryptDataPtr)GETINSTANCE(m_pDllHandle, "SignAndEncryptData");
    m_pCfcaPoint.DecryptDataFromCMSEnvelope = (DecryptDataFromCMSEnvelopePtr)GETINSTANCE(m_pDllHandle, "DecryptDataFromCMSEnvelope"); 
    m_pCfcaPoint.DecryptAndVerifyDataSignature = (DecryptAndVerifyDataSignaturePtr)GETINSTANCE(m_pDllHandle, "DecryptAndVerifyDataSignature");
    m_pCfcaPoint.GetCertificateInfo = (GetCertificateInfoPtr)GETINSTANCE(m_pDllHandle, "GetCertificateInfo"); 
    m_pCfcaPoint.GetPublicCertFromPFX = (GetPublicCertFromPFXPtr)GETINSTANCE(m_pDllHandle, "GetPublicCertFromPFX"); 
    m_pCfcaPoint.VerifyCertificate = (VerifyCertificatePtr)GETINSTANCE(m_pDllHandle, "VerifyCertificate"); 
    m_pCfcaPoint.FreeMemory = (FreeMemoryPtr)GETINSTANCE(m_pDllHandle, "FreeMemory"); 
	
    if (NULL == m_pCfcaPoint.Initialize ||
        NULL == m_pCfcaPoint.Uninitialize ||
        NULL == m_pCfcaPoint.SignData_PKCS1 ||
        NULL == m_pCfcaPoint.SignData_PKCS7Detached ||
        NULL == m_pCfcaPoint.SignData_PKCS7Attached || 
		NULL == m_pCfcaPoint.SignFile_PKCS7Detached ||
        NULL == m_pCfcaPoint.VerifyDataSignature_PKCS1 ||
        NULL == m_pCfcaPoint.VerifyDataSignature_PKCS7Detached ||
        NULL == m_pCfcaPoint.VerifyDataSignature_PKCS7Attached || 
        NULL == m_pCfcaPoint.VerifyFileSignature_PKCS7Detached ||
        NULL == m_pCfcaPoint.EncryptDataToCMSEnvelope ||
        NULL == m_pCfcaPoint.SignAndEncryptData ||
        NULL == m_pCfcaPoint.DecryptDataFromCMSEnvelope ||
        NULL == m_pCfcaPoint.DecryptAndVerifyDataSignature ||
        NULL == m_pCfcaPoint.VerifyCertificate ||
        NULL == m_pCfcaPoint.GetCertificateInfo ||
        NULL == m_pCfcaPoint.GetPublicCertFromPFX ||
        NULL == m_pCfcaPoint.FreeMemory)
    {
        sprintf(m_errMsg,"取动态库[%s]里的函数地址失败",dllname.c_str());
        return false;
    }
	if (m_pCfcaPoint.Initialize() != 0)
	{
		sprintf(m_errMsg,"动态库[%s]之证书初始化失败",dllname.c_str());
        return false;	
	}
	//取本地证书公钥，不用每次都取了
	
	strcpy(m_sMethod,"RSA");
#ifdef _WINDOWS
	sprintf(m_sCertFilePath,"%s\\key.pfx",certpath);
#else
	sprintf(m_sCertFilePath,"%s/key.pfx",certpath);
#endif
	strcpy(m_sCertPwd,certpwd);
//	strcpy(m_sHashMethod,"SHA-1");
	strcpy(m_sHashMethod,"SHA-256");
	char *pubkey=NULL;
	int nRet = m_pCfcaPoint.GetPublicCertFromPFX(m_sMethod,m_sCertFilePath,m_sCertPwd,&pubkey);
	if (nRet !=0)
	{
		sprintf(m_errMsg,"取公钥失败");
		if (pubkey != NULL)
		{
			m_pCfcaPoint.FreeMemory(pubkey);
		}
		pubkey = NULL;
		return false;
	}
	m_nPublicKeyLen = strlen(pubkey);

	if (m_nPublicKeyLen > sizeof(m_sPublicKey))
	{
		sprintf(m_errMsg,"公钥长度大于4096");
		if (pubkey != NULL)
		{
			m_pCfcaPoint.FreeMemory(pubkey);
		}
		pubkey = NULL;
		return false;
	}
	memcpy(m_sPublicKey,pubkey,m_nPublicKeyLen);
	m_sPublicKey[m_nPublicKeyLen] = 0;
	m_pCfcaPoint.FreeMemory(pubkey);
	pubkey = NULL;
	return true;
}

// 函数名: UnInitCertApi
// 编程  : 王明松 2013-5-24 11:05:29
// 返回  : virtual void 
// 描述  : 退出证书API
bool CCfcaApi::UnInitCertApi()
{
	if (m_pCfcaPoint.Uninitialize != NULL)
	{
		m_pCfcaPoint.Uninitialize();
		m_pCfcaPoint.Uninitialize = NULL;
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
bool CCfcaApi::Sign(const char *signstr,unsigned short signstrlen,char *signres,unsigned short &signreslen)
{
	char *ssign=NULL;
	int sdatalen = signstrlen;

	int nRet = m_pCfcaPoint.SignData_PKCS1(m_sMethod,(unsigned char *)signstr,sdatalen,m_sCertFilePath,m_sCertPwd,m_sHashMethod,&ssign);
	if (nRet != 0)
	{
		sprintf(m_errMsg,"PKCS#1签名失败");
		if (ssign != NULL)
		{
			m_pCfcaPoint.FreeMemory(ssign);
		}
		return false;
	}
	if (signreslen < strlen(ssign))
	{
		sprintf(m_errMsg,"签名空间不足");
		m_pCfcaPoint.FreeMemory(ssign);
		return false;
	}
	signreslen = strlen(ssign);
	memcpy(signres,ssign,signreslen);
	m_pCfcaPoint.FreeMemory(ssign);
	ssign = NULL;
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
bool CCfcaApi::GetPubicKey(char *signkeybuf,unsigned short &signkeybuflen,char *encryptkeybuf,unsigned short &encryptkkeybuflen)
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
bool CCfcaApi::CheckSign(const char *publickey,unsigned short publickeylen,const char *signstr,unsigned short signstrlen,const char *signres,unsigned short signreslen)
{
	int nRet = m_pCfcaPoint.VerifyDataSignature_PKCS1(m_sMethod,(unsigned char *)signstr,signstrlen,(char *)publickey,m_sHashMethod,(char *)signres);
	if (nRet != 0)
	{
		sprintf(m_errMsg,"签名验证失败");
		return false;
	}
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
bool CCfcaApi::Encrypt(const char *publickey,unsigned short publickeylen,char *data,unsigned short &datalen)
{
	char enmethod[10];
	strcpy(enmethod,"3DES");
	char *res=NULL;
	int nRet = m_pCfcaPoint.EncryptDataToCMSEnvelope(m_sMethod,(unsigned char *)data,datalen,(char *)publickey,enmethod,&res);
	if (nRet !=0 )
	{
		FILE *fp = fopen("cfcadata.dat","wb");
		if (fp != NULL)
		{
			fwrite(data,1,datalen,fp);
			fclose(fp);
		}
		fp = fopen("cfcapubkey.dat","wb");
		if (fp != NULL)
		{
			fwrite(publickey,1,publickeylen,fp);
			fclose(fp);
		}
		sprintf(m_errMsg,"数据加密失败 %d",nRet);
		if (res != NULL)
		{
			m_pCfcaPoint.FreeMemory(res);
		}
		res=NULL;
		return false;
	}
	datalen = strlen(res);
	memcpy(data,res,datalen);
	m_pCfcaPoint.FreeMemory(res);
	res = NULL;
	return true;
}


// 函数名: UnEncrypt
// 编程  : 王明松 2013-5-25 11:28:33
// 返回  : virtual bool 
// 参数  : char *data   加密数据->解密数据
// 参数  : unsigned short  &datalen 加密数据长度，解密前为加密的数据长度，解密后为数据明文长度
// 描述  : 用本机的私钥进行解密
bool CCfcaApi::UnEncrypt(char *data,unsigned short  &datalen)
{
	unsigned char *res=NULL;
	int  reslen;
	int nRet = m_pCfcaPoint.DecryptDataFromCMSEnvelope(m_sMethod,data,m_sCertFilePath,m_sCertPwd,&res,&reslen);
	if (nRet !=0 )
	{
		sprintf(m_errMsg,"数据解密失败");
		if (res != NULL)
		{
			m_pCfcaPoint.FreeMemory(res);
		}
		return false;
	}
	datalen = reslen;
	memcpy(data,res,datalen);
	m_pCfcaPoint.FreeMemory(res);
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
bool CCfcaApi::GetCertSerial(const char *pubkey,unsigned short pubkeylen,char *serial,unsigned short &seriallen)
{
	char certname[30];
	strcpy(certname,"SerialNumber");
	char *res = NULL;
	int nRet = m_pCfcaPoint.GetCertificateInfo((char *)pubkey,certname,&res);
	if (nRet !=0 )
	{
		sprintf(m_errMsg,"取证书序列号失败");
		if (res != NULL)
		{
			m_pCfcaPoint.FreeMemory(res);
		}
		res=NULL;
		return false;
	}
	seriallen = strlen(res);
	memcpy(serial,res,seriallen);
	serial[seriallen] =0;
	m_pCfcaPoint.FreeMemory(res);
	res = NULL;
	return true;
}

std::string CCfcaApi::GetErrMsg()
{
	return m_errMsg;
}