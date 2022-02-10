// ShcaApi.cpp: implementation of the CShcaApi class.
//
//////////////////////////////////////////////////////////////////////

#include "ShcaApi.h"
#include "ShcaApiML.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//注意：下面的定义在每个类里面都要有，主程序根据此方法获取本类的实例
CShcaApiML g_shcaapi_list_point;



CShcaApi * getInstance() 
{ 
    return new CShcaApi(); 
}

CShcaApi::CShcaApi()
{
	bzero(m_sPubFilePath,sizeof(m_sPubFilePath));
	bzero(m_sCertFilePath,sizeof(m_sCertFilePath));
	bzero(m_sRootFilePath,sizeof(m_sRootFilePath));
	bzero(m_sPublicKey,sizeof(m_sPublicKey));
	bzero(m_sCertPwd,sizeof(m_sCertPwd));
	bzero(m_errMsg,sizeof(m_errMsg));
	m_hSE = NULL;
	m_nPublicKeyLen = 0;
}

CShcaApi::~CShcaApi()
{
	if (m_hSE != NULL)
	{
		UnInitCertApi();
	}
	bzero(&m_pShcaPoint,sizeof(SHCA_ENTRYPOINTS));
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
bool CShcaApi::InitCertApi(const char *certpath,char *certpwd)
{
	std::string dllname;
#ifdef _WINDOWS
	dllname = "SafeEngine.dll";
	
#else
	dllname = "libsafeengine.so";
	
#endif	

	m_pDllHandle = LOADDLL(dllname.c_str());
	if (ISNOTOPEN(m_pDllHandle))
	{
		sprintf(m_errMsg,"打开动态库[%s]失败",dllname.c_str());
		return false;
	}
	m_pShcaPoint.SEH_InitialSession = (SEH_InitialSessionPtr)GETINSTANCE(m_pDllHandle, "SEH_InitialSession"); 
	m_pShcaPoint.SEH_ClearSession = (SEH_ClearSessionPtr)GETINSTANCE(m_pDllHandle, "SEH_ClearSession");
	m_pShcaPoint.SEH_Envelope = (SEH_EnvelopePtr)GETINSTANCE(m_pDllHandle, "SEH_Envelope");
	m_pShcaPoint.SEH_GetCertDetail = (SEH_GetCertDetailPtr)GETINSTANCE(m_pDllHandle, "SEH_GetCertDetail");
	m_pShcaPoint.SEH_GetCertUniqueID = (SEH_GetCertUniqueIDPtr)GETINSTANCE(m_pDllHandle, "SEH_GetCertUniqueID");
	m_pShcaPoint.SEH_GetCertValidDate = (SEH_GetCertValidDatePtr)GETINSTANCE(m_pDllHandle, "SEH_GetCertValidDate");
	m_pShcaPoint.SEH_GetSelfCertificate = (SEH_GetSelfCertificatePtr)GETINSTANCE(m_pDllHandle, "SEH_GetSelfCertificate");
	m_pShcaPoint.SEH_SetConfiguration = (SEH_SetConfigurationPtr)GETINSTANCE(m_pDllHandle, "SEH_SetConfiguration");
	m_pShcaPoint.SEH_ShowVersion = (SEH_ShowVersionPtr)GETINSTANCE(m_pDllHandle, "SEH_ShowVersion");
	m_pShcaPoint.SEH_SignData = (SEH_SignDataPtr)GETINSTANCE(m_pDllHandle, "SEH_SignData");
	m_pShcaPoint.SEH_VerifyCertificate = (SEH_VerifyCertificatePtr)GETINSTANCE(m_pDllHandle, "SEH_VerifyCertificate");
	m_pShcaPoint.SEH_VerifySignData = (SEH_VerifySignDataPtr)GETINSTANCE(m_pDllHandle, "SEH_VerifySignData");
	

    if (NULL == m_pShcaPoint.SEH_InitialSession ||
        NULL == m_pShcaPoint.SEH_ClearSession ||
        NULL == m_pShcaPoint.SEH_Envelope ||
        NULL == m_pShcaPoint.SEH_GetCertDetail ||
        NULL == m_pShcaPoint.SEH_GetCertUniqueID || 
		NULL == m_pShcaPoint.SEH_GetCertValidDate ||
        NULL == m_pShcaPoint.SEH_GetSelfCertificate ||
        NULL == m_pShcaPoint.SEH_SetConfiguration ||
        NULL == m_pShcaPoint.SEH_ShowVersion || 
        NULL == m_pShcaPoint.SEH_SignData ||
        NULL == m_pShcaPoint.SEH_VerifyCertificate ||
        NULL == m_pShcaPoint.SEH_VerifySignData )
    {
        sprintf(m_errMsg,"取动态库[%s]里的函数地址失败",dllname.c_str());
        return false;
    }
#ifdef _WINDOWS
	sprintf(m_sPubFilePath,"%s\\UserCert.der",certpath);
	sprintf(m_sCertFilePath,"%s\\UserKey.key",certpath);
	sprintf(m_sRootFilePath,"%s\\RootCert.der",certpath);
#else
	sprintf(m_sPubFilePath,"%s/UserCert.der",certpath);
	sprintf(m_sCertFilePath,"%s/UserKey.key",certpath);
	sprintf(m_sRootFilePath,"%s/RootCert.der",certpath);
#endif
	strcpy(m_sCertPwd,certpwd);
	long lRet = m_pShcaPoint.SEH_InitialSession(&m_hSE,2,m_sCertFilePath,m_sCertPwd,0,2,m_sRootFilePath,"");
	if (lRet != SE_SUCCESS)
	{
		sprintf(m_errMsg,"动态库[%s]之证书初始化失败 返回码[%x] 证书文件[%s %s]",dllname.c_str(),lRet,m_sCertFilePath,m_sRootFilePath);
        return false;	
	}
 	//取本地证书公钥，不用每次都取了
	m_nPublicKeyLen = sizeof(m_sPublicKey);
	lRet =  m_pShcaPoint.SEH_GetSelfCertificate(m_hSE,2, m_sPubFilePath, m_sCertPwd, (unsigned char *)m_sPublicKey,&m_nPublicKeyLen);
	if (lRet != SE_SUCCESS)
	{
		sprintf(m_errMsg,"取公钥证书错<%x>",lRet);
		return false;
	}
	return true;
}

// 函数名: UnInitCertApi
// 编程  : 王明松 2013-5-24 11:05:29
// 返回  : virtual void 
// 描述  : 退出证书API
bool CShcaApi::UnInitCertApi()
{
	long lRet =  m_pShcaPoint.SEH_ClearSession(m_hSE);
	if (lRet != SE_SUCCESS)
	{
		sprintf(m_errMsg,"清除CA环境错<%x>",lRet);
		m_hSE = NULL;
		return false;
	}
	m_hSE = NULL;
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
bool CShcaApi::Sign(const char *signstr,unsigned short signstrlen,char *signres,unsigned short &signreslen)
{
	long lsignlen = signstrlen;
	unsigned long lsignreslen = signreslen;
	long lRet =  m_pShcaPoint.SEH_SignData(m_hSE, (unsigned char *)signstr,lsignlen,3,(unsigned char *)signres,&lsignreslen);
	if (lRet != SE_SUCCESS)
	{
		sprintf(m_errMsg,"SHA1签名失败[%x]",lRet);
		return false;
	}
	signreslen = lsignreslen;
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
bool CShcaApi::GetPubicKey(char *signkeybuf,unsigned short &signkeybuflen,char *encryptkeybuf,unsigned short &encryptkkeybuflen)
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
bool CShcaApi::CheckSign(const char *publickey,unsigned short publickeylen,const char *signstr,unsigned short signstrlen,const char *signres,unsigned short signreslen)
{
	long lRet =  m_pShcaPoint.SEH_VerifySignData(m_hSE,(unsigned char *)signstr,(unsigned long)signstrlen,3, (unsigned char *)signres,(unsigned long)signreslen,(unsigned char *)publickey,publickeylen);
	if (lRet != SE_SUCCESS)
	{
		sprintf(m_errMsg,"SHA1验证签名失败 [%x]",lRet);
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
bool CShcaApi::Encrypt(const char *publickey,unsigned short publickeylen,char *data,unsigned short &datalen)
{
	unsigned long  ldatalen = datalen;
	unsigned char odata[65534];
	unsigned long odatalen = sizeof(odata);
	long lRet =  m_pShcaPoint.SEH_Envelope(m_hSE, 1,(unsigned char *)data,ldatalen,odata,&odatalen,(unsigned char *)publickey,publickeylen);
	if (lRet != SE_SUCCESS)
	{
		sprintf(m_errMsg,"数字信封加密失败 [%x]",lRet);
		return false;	
	}
	memcpy(data,odata,odatalen);
	datalen = odatalen;
	return true;
}


// 函数名: UnEncrypt
// 编程  : 王明松 2013-5-25 11:28:33
// 返回  : virtual bool 
// 参数  : char *data   加密数据->解密数据
// 参数  : unsigned short  &datalen 加密数据长度，解密前为加密的数据长度，解密后为数据明文长度
// 描述  : 用本机的私钥进行解密
bool CShcaApi::UnEncrypt(char *data,unsigned short  &datalen)
{
	unsigned long  ldatalen = datalen;
	unsigned char odata[65534];
	unsigned long odatalen = sizeof(odata);
	long lRet = m_pShcaPoint.SEH_Envelope(m_hSE, 2,(unsigned char *)data,ldatalen,odata,&odatalen, NULL,NULL);
	if (lRet != SE_SUCCESS)
	{
		sprintf(m_errMsg,"数字信封解密失败 [%x]",lRet);
		return false;	
	}
	memcpy(data,odata,odatalen);
	datalen = odatalen;
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
bool CShcaApi::GetCertSerial(const char *pubkey,unsigned short pubkeylen,char *serial,unsigned short &seriallen)
{
	long lRet = m_pShcaPoint.SEH_GetCertUniqueID(m_hSE,(unsigned char *)pubkey,pubkeylen,serial,&seriallen);
	if (lRet != SE_SUCCESS)
	{
		sprintf(m_errMsg,"取证书唯一序号失败 [%x]",lRet);
		return false;
	}
	return true;
}

std::string CShcaApi::GetErrMsg()
{
	return m_errMsg;
}

// 函数名: InitCertApi
// 编程  : 王明松 2013-5-24 11:05:29
// 返回  : int  >=0成功 <0失败   成功则为实例的序号，下次使用要将序号传入
// 参数  : const char *certpath  证书目录 最后不带"/"
// 参数  : char *certpwd  证书密码
// 描述  : 初始化证书api
int SHCAAPI_InitCertApi(const char *certpath,const char *certpwd,int *apipoint)
{
	CShcaApi *tmpapi = getInstance();
	if (tmpapi == NULL)
	{
		return -1;
	}
	//将apiclass加入表，返回序号
	int ret = g_shcaapi_list_point.Insert(tmpapi);
	if (ret <0)
	{
		return -1;
	}
	*apipoint = ret;
	if (!tmpapi->InitCertApi(certpath,(char *)certpwd))
	{
		return -2;
	}
	return ret;
}

// 函数名: UnInitCertApi
// 编程  : 王明松 2013-5-24 11:05:29
// 返回  : int   0成功  
// 描述  : 退出证书API
int SHCAAPI_UnInitCertApi(int apipoint)
{
	CShcaApi *tmpapi = g_shcaapi_list_point.Find(apipoint);
	if (tmpapi != NULL)
	{
		tmpapi->UnInitCertApi();
		return 0;
	}
	return -1;
}

// 函数名: Sign
// 编程  : 王明松 2013-5-24 11:07:02
// 返回  : int   0成功  
// 参数  : int apipoint    api的实例序号
// 参数  : const char *signstr  要签名的字符串
// 参数  : unsigned short signstrlen 要签名的字符串长度
// 参数  : char *signres   签名结果
// 参数  : unsigned short *signreslen   签名结果长度
// 描述  : 签名，用本机的私钥签名
int SHCAAPI_Sign(int apipoint,const char *signstr,unsigned short signstrlen,char *signres,unsigned short *signreslen)
{
	CShcaApi *tmpapi = g_shcaapi_list_point.Find(apipoint);
	if (tmpapi != NULL)
	{
		unsigned short len = *signreslen;
		if (!tmpapi->Sign(signstr,signstrlen,signres,len))
		{
			return -2;
		}
		*signreslen = len;
		return 0;
	}
	return -1;
}


// 函数名: GetPubicKey
// 编程  : 王明松 2013-5-25 11:22:50
// 返回  : int   0成功 
// 参数  : int apipoint    api的实例序号
// 参数  : char *signkeybuf   签名公钥
// 参数  : unsigned short *signkeybuflen 签名公钥长度
// 参数  : char *encryptkkeybuf  加密公钥
// 参数  : unsigned short *encryptkkeybuflen 加密公钥长度
// 描述  : 取本机证书的公钥
int SHCAAPI_GetPubicKey(int apipoint,char *signkeybuf,unsigned short *signkeybuflen,char *encryptkeybuf,unsigned short *encryptkkeybuflen)
{
	CShcaApi *tmpapi = g_shcaapi_list_point.Find(apipoint);
	if (tmpapi != NULL)
	{
		unsigned short sklen =*signkeybuflen;
		unsigned short eklen =*encryptkkeybuflen;
		if (!tmpapi->GetPubicKey(signkeybuf,sklen,encryptkeybuf,eklen))
		{
			return -2;
		}
		*signkeybuflen = sklen;
		*encryptkkeybuflen= eklen;
		return 0;
	}
	return -1;
}

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
int SHCAAPI_CheckSign(int apipoint,const char *publickey,unsigned short publickeylen,const char *signstr,unsigned short signstrlen,const char *signres,unsigned short signreslen)
{
	CShcaApi *tmpapi = g_shcaapi_list_point.Find(apipoint);
	if (tmpapi != NULL)
	{
		if (!tmpapi->CheckSign(publickey,publickeylen,signstr,signstrlen,signres,signreslen))
		{
			return -2;
		}
		return 0;
	}
	return -1;
}


// 函数名: Encrypt
// 编程  : 王明松 2013-5-25 11:27:12
// 返回  : int   0成功  
// 参数  : int apipoint    api的实例序号
// 参数  : const char *publickey  加密公钥
// 参数  : unsigned short publickeylen 加密公钥长度
// 参数  : char *data   明文->加密数据
// 参数  : unsigned short *datalen   加密数长度，加密前为要加密的数据长度，加密后为加密码后的数据长度
// 描述  : 加密，用对方的公钥进行加密。仅在会话建立时使用。交易时使用会话密钥进行DES加密
int SHCAAPI_Encrypt(int apipoint,const char *publickey,unsigned short publickeylen,char *data,unsigned short *datalen)
{
	CShcaApi *tmpapi = g_shcaapi_list_point.Find(apipoint);
	if (tmpapi != NULL)
	{
		unsigned short len = *datalen;
		if (!tmpapi->Encrypt(publickey,publickeylen,data,len))
		{
			return -2;
		}
		*datalen = len;
		return 0;
	}
	return -1;
}


// 函数名: UnEncrypt
// 编程  : 王明松 2013-5-25 11:28:33
// 返回  : int   0成功 
// 参数  : int apipoint    api的实例序号
// 参数  : char *data   加密数据->解密数据
// 参数  : unsigned short  *datalen 加密数据长度，解密前为加密的数据长度，解密后为数据明文长度
// 描述  : 用本机的私钥进行解密
int SHCAAPI_UnEncrypt(int apipoint,char *data,unsigned short  *datalen)
{
	CShcaApi *tmpapi = g_shcaapi_list_point.Find(apipoint);
	if (tmpapi != NULL)
	{
		unsigned short len = *datalen;
		if (!tmpapi->UnEncrypt(data,len))
		{
			return -2;
		}
		*datalen = len;
		return 0;
	}
	return -1;
}



// 函数名: GetCertSerial
// 编程  : 王明松 2013-6-14 15:51:14
// 返回  : int   0成功 
// 参数  : int apipoint    api的实例序号
// 参数  : const char *pubkey
// 参数  : unsigned short pubkeylen
// 参数  : char *serial
// 参数  : int *seriallen
// 描述  : 获取证唯一序号
int SHCAAPI_GetCertSerial(int apipoint,const char *pubkey,unsigned short pubkeylen,char *serial,unsigned short *seriallen)
{
	CShcaApi *tmpapi = g_shcaapi_list_point.Find(apipoint);
	if (tmpapi != NULL)
	{
		unsigned short len = *seriallen;
		if (!tmpapi->GetCertSerial(pubkey,pubkeylen,serial,len))
		{
			return -2;
		}
		*seriallen = len;
		return 0;
	}
	return -1;
}


// 函数名: GetErrMsg
// 编程  : 王明松 2014-4-10 15:44:35
// 返回  : void 
// 参数  : int apipoint    api的实例序号
// 描述  : 取出错信息
int SHCAAPI_GetErrMsg(int apipoint,char *errmsg)
{
	CShcaApi *tmpapi = g_shcaapi_list_point.Find(apipoint);
	if (tmpapi != NULL)
	{
		std::string tmpstr = tmpapi->GetErrMsg();
		strcpy(errmsg,tmpstr.c_str());
		return 0;
	}
	return -1;
}
