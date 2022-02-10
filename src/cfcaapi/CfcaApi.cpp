// CfcaApi.cpp: implementation of the CCfcaApi class.
//
//////////////////////////////////////////////////////////////////////

#include "CfcaApi.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//ע�⣺����Ķ�����ÿ�������涼Ҫ�У���������ݴ˷�����ȡ�����ʵ��
//֤���ļ���Ϊkey.pfx

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
// ������: InitCertApi
// ���  : ������ 2013-5-24 11:05:29
// ����  : virtual void 
// ����  : ��ʼ��֤��api
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
		sprintf(m_errMsg,"�򿪶�̬��[%s]ʧ��",dllname.c_str());
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
        sprintf(m_errMsg,"ȡ��̬��[%s]��ĺ�����ַʧ��",dllname.c_str());
        return false;
    }
	if (m_pCfcaPoint.Initialize() != 0)
	{
		sprintf(m_errMsg,"��̬��[%s]֤֮���ʼ��ʧ��",dllname.c_str());
        return false;	
	}
	//ȡ����֤�鹫Կ������ÿ�ζ�ȡ��
	
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
		sprintf(m_errMsg,"ȡ��Կʧ��");
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
		sprintf(m_errMsg,"��Կ���ȴ���4096");
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

// ������: UnInitCertApi
// ���  : ������ 2013-5-24 11:05:29
// ����  : virtual void 
// ����  : �˳�֤��API
bool CCfcaApi::UnInitCertApi()
{
	if (m_pCfcaPoint.Uninitialize != NULL)
	{
		m_pCfcaPoint.Uninitialize();
		m_pCfcaPoint.Uninitialize = NULL;
	}
	return true;
}

// ������: Sign
// ���  : ������ 2013-5-24 11:07:02
// ����  : virtual bool 
// ����  : const char *signstr  Ҫǩ�����ַ���
// ����  : unsigned short signstrlen Ҫǩ�����ַ�������
// ����  : char *signres   ǩ�����
// ����  : unsigned short &signreslen   ǩ���������
// ����  : ǩ�����ñ�����˽Կǩ��
bool CCfcaApi::Sign(const char *signstr,unsigned short signstrlen,char *signres,unsigned short &signreslen)
{
	char *ssign=NULL;
	int sdatalen = signstrlen;

	int nRet = m_pCfcaPoint.SignData_PKCS1(m_sMethod,(unsigned char *)signstr,sdatalen,m_sCertFilePath,m_sCertPwd,m_sHashMethod,&ssign);
	if (nRet != 0)
	{
		sprintf(m_errMsg,"PKCS#1ǩ��ʧ��");
		if (ssign != NULL)
		{
			m_pCfcaPoint.FreeMemory(ssign);
		}
		return false;
	}
	if (signreslen < strlen(ssign))
	{
		sprintf(m_errMsg,"ǩ���ռ䲻��");
		m_pCfcaPoint.FreeMemory(ssign);
		return false;
	}
	signreslen = strlen(ssign);
	memcpy(signres,ssign,signreslen);
	m_pCfcaPoint.FreeMemory(ssign);
	ssign = NULL;
	return true;
}


// ������: GetPubicKey
// ���  : ������ 2013-5-25 11:22:50
// ����  : virtual bool 
// ����  : char *signkeybuf   ǩ����Կ
// ����  : unsigned short &signkeybuflen ǩ����Կ����
// ����  : char *encryptkkeybuf  ���ܹ�Կ
// ����  : unsigned short &encryptkkeybuflen ���ܹ�Կ����
// ����  : ȡ����֤��Ĺ�Կ
bool CCfcaApi::GetPubicKey(char *signkeybuf,unsigned short &signkeybuflen,char *encryptkeybuf,unsigned short &encryptkkeybuflen)
{
	if (m_nPublicKeyLen <=0)
	{
		sprintf(m_errMsg,"δ��ʼ��");
		return false;
	}
	if (m_nPublicKeyLen >= signkeybuflen)
	{
		sprintf(m_errMsg,"ǩ��֤��ռ䲻�� ���볤��%d ��Կ���� %d",signkeybuflen,m_nPublicKeyLen);
		return false;
	}
	if (m_nPublicKeyLen >= encryptkkeybuflen)
	{
		sprintf(m_errMsg,"����֤��ռ䲻�� ���볤��%d ��Կ���� %d",encryptkkeybuflen,m_nPublicKeyLen);
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

// ������: CheckSign
// ���  : ������ 2013-5-30 10:43:56
// ����  : virtual bool 
// ����  : const char *publickey   ǩ����Կ
// ����  : unsigned short publickeylen   ǩ����Կ����
// ����  : const char *signstr      ǩ����ԭʼ�ַ���
// ����  : unsigned short signstrlen  ǩ����ԭʼ�ַ�������
// ����  : const char *signres   ǩ�����
// ����  : int signreslen ǩ���������
// ����  : У��ǩ������У��֤���Ƿ�һ����֤������
bool CCfcaApi::CheckSign(const char *publickey,unsigned short publickeylen,const char *signstr,unsigned short signstrlen,const char *signres,unsigned short signreslen)
{
	int nRet = m_pCfcaPoint.VerifyDataSignature_PKCS1(m_sMethod,(unsigned char *)signstr,signstrlen,(char *)publickey,m_sHashMethod,(char *)signres);
	if (nRet != 0)
	{
		sprintf(m_errMsg,"ǩ����֤ʧ��");
		return false;
	}
	return true;
}


// ������: Encrypt
// ���  : ������ 2013-5-25 11:27:12
// ����  : virtual bool 
// ����  : const char *publickey  ���ܹ�Կ
// ����  : unsigned short publickeylen ���ܹ�Կ����
// ����  : char *data   ����->��������
// ����  : unsigned short &datalen   ���������ȣ�����ǰΪҪ���ܵ����ݳ��ȣ����ܺ�Ϊ�����������ݳ���
// ����  : ���ܣ��öԷ��Ĺ�Կ���м��ܡ����ڻỰ����ʱʹ�á�����ʱʹ�ûỰ��Կ����DES����
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
		sprintf(m_errMsg,"���ݼ���ʧ�� %d",nRet);
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


// ������: UnEncrypt
// ���  : ������ 2013-5-25 11:28:33
// ����  : virtual bool 
// ����  : char *data   ��������->��������
// ����  : unsigned short  &datalen �������ݳ��ȣ�����ǰΪ���ܵ����ݳ��ȣ����ܺ�Ϊ�������ĳ���
// ����  : �ñ�����˽Կ���н���
bool CCfcaApi::UnEncrypt(char *data,unsigned short  &datalen)
{
	unsigned char *res=NULL;
	int  reslen;
	int nRet = m_pCfcaPoint.DecryptDataFromCMSEnvelope(m_sMethod,data,m_sCertFilePath,m_sCertPwd,&res,&reslen);
	if (nRet !=0 )
	{
		sprintf(m_errMsg,"���ݽ���ʧ��");
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



// ������: GetCertSerial
// ���  : ������ 2013-6-14 15:51:14
// ����  : virtual bool 
// ����  : const char *pubkey
// ����  : unsigned short pubkeylen
// ����  : char *serial
// ����  : int &seriallen
// ����  : ��ȡ֤Ψһ���
bool CCfcaApi::GetCertSerial(const char *pubkey,unsigned short pubkeylen,char *serial,unsigned short &seriallen)
{
	char certname[30];
	strcpy(certname,"SerialNumber");
	char *res = NULL;
	int nRet = m_pCfcaPoint.GetCertificateInfo((char *)pubkey,certname,&res);
	if (nRet !=0 )
	{
		sprintf(m_errMsg,"ȡ֤�����к�ʧ��");
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