// ShcaApi.cpp: implementation of the CShcaApi class.
//
//////////////////////////////////////////////////////////////////////

#include "ShcaApi.h"
#include "ShcaApiML.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//ע�⣺����Ķ�����ÿ�������涼Ҫ�У���������ݴ˷�����ȡ�����ʵ��
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
// ������: InitCertApi
// ���  : ������ 2013-5-24 11:05:29
// ����  : virtual void 
// ����  : ��ʼ��֤��api
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
		sprintf(m_errMsg,"�򿪶�̬��[%s]ʧ��",dllname.c_str());
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
        sprintf(m_errMsg,"ȡ��̬��[%s]��ĺ�����ַʧ��",dllname.c_str());
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
		sprintf(m_errMsg,"��̬��[%s]֤֮���ʼ��ʧ�� ������[%x] ֤���ļ�[%s %s]",dllname.c_str(),lRet,m_sCertFilePath,m_sRootFilePath);
        return false;	
	}
 	//ȡ����֤�鹫Կ������ÿ�ζ�ȡ��
	m_nPublicKeyLen = sizeof(m_sPublicKey);
	lRet =  m_pShcaPoint.SEH_GetSelfCertificate(m_hSE,2, m_sPubFilePath, m_sCertPwd, (unsigned char *)m_sPublicKey,&m_nPublicKeyLen);
	if (lRet != SE_SUCCESS)
	{
		sprintf(m_errMsg,"ȡ��Կ֤���<%x>",lRet);
		return false;
	}
	return true;
}

// ������: UnInitCertApi
// ���  : ������ 2013-5-24 11:05:29
// ����  : virtual void 
// ����  : �˳�֤��API
bool CShcaApi::UnInitCertApi()
{
	long lRet =  m_pShcaPoint.SEH_ClearSession(m_hSE);
	if (lRet != SE_SUCCESS)
	{
		sprintf(m_errMsg,"���CA������<%x>",lRet);
		m_hSE = NULL;
		return false;
	}
	m_hSE = NULL;
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
bool CShcaApi::Sign(const char *signstr,unsigned short signstrlen,char *signres,unsigned short &signreslen)
{
	long lsignlen = signstrlen;
	unsigned long lsignreslen = signreslen;
	long lRet =  m_pShcaPoint.SEH_SignData(m_hSE, (unsigned char *)signstr,lsignlen,3,(unsigned char *)signres,&lsignreslen);
	if (lRet != SE_SUCCESS)
	{
		sprintf(m_errMsg,"SHA1ǩ��ʧ��[%x]",lRet);
		return false;
	}
	signreslen = lsignreslen;
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
bool CShcaApi::GetPubicKey(char *signkeybuf,unsigned short &signkeybuflen,char *encryptkeybuf,unsigned short &encryptkkeybuflen)
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
bool CShcaApi::CheckSign(const char *publickey,unsigned short publickeylen,const char *signstr,unsigned short signstrlen,const char *signres,unsigned short signreslen)
{
	long lRet =  m_pShcaPoint.SEH_VerifySignData(m_hSE,(unsigned char *)signstr,(unsigned long)signstrlen,3, (unsigned char *)signres,(unsigned long)signreslen,(unsigned char *)publickey,publickeylen);
	if (lRet != SE_SUCCESS)
	{
		sprintf(m_errMsg,"SHA1��֤ǩ��ʧ�� [%x]",lRet);
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
bool CShcaApi::Encrypt(const char *publickey,unsigned short publickeylen,char *data,unsigned short &datalen)
{
	unsigned long  ldatalen = datalen;
	unsigned char odata[65534];
	unsigned long odatalen = sizeof(odata);
	long lRet =  m_pShcaPoint.SEH_Envelope(m_hSE, 1,(unsigned char *)data,ldatalen,odata,&odatalen,(unsigned char *)publickey,publickeylen);
	if (lRet != SE_SUCCESS)
	{
		sprintf(m_errMsg,"�����ŷ����ʧ�� [%x]",lRet);
		return false;	
	}
	memcpy(data,odata,odatalen);
	datalen = odatalen;
	return true;
}


// ������: UnEncrypt
// ���  : ������ 2013-5-25 11:28:33
// ����  : virtual bool 
// ����  : char *data   ��������->��������
// ����  : unsigned short  &datalen �������ݳ��ȣ�����ǰΪ���ܵ����ݳ��ȣ����ܺ�Ϊ�������ĳ���
// ����  : �ñ�����˽Կ���н���
bool CShcaApi::UnEncrypt(char *data,unsigned short  &datalen)
{
	unsigned long  ldatalen = datalen;
	unsigned char odata[65534];
	unsigned long odatalen = sizeof(odata);
	long lRet = m_pShcaPoint.SEH_Envelope(m_hSE, 2,(unsigned char *)data,ldatalen,odata,&odatalen, NULL,NULL);
	if (lRet != SE_SUCCESS)
	{
		sprintf(m_errMsg,"�����ŷ����ʧ�� [%x]",lRet);
		return false;	
	}
	memcpy(data,odata,odatalen);
	datalen = odatalen;
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
bool CShcaApi::GetCertSerial(const char *pubkey,unsigned short pubkeylen,char *serial,unsigned short &seriallen)
{
	long lRet = m_pShcaPoint.SEH_GetCertUniqueID(m_hSE,(unsigned char *)pubkey,pubkeylen,serial,&seriallen);
	if (lRet != SE_SUCCESS)
	{
		sprintf(m_errMsg,"ȡ֤��Ψһ���ʧ�� [%x]",lRet);
		return false;
	}
	return true;
}

std::string CShcaApi::GetErrMsg()
{
	return m_errMsg;
}

// ������: InitCertApi
// ���  : ������ 2013-5-24 11:05:29
// ����  : int  >=0�ɹ� <0ʧ��   �ɹ���Ϊʵ������ţ��´�ʹ��Ҫ����Ŵ���
// ����  : const char *certpath  ֤��Ŀ¼ ��󲻴�"/"
// ����  : char *certpwd  ֤������
// ����  : ��ʼ��֤��api
int SHCAAPI_InitCertApi(const char *certpath,const char *certpwd,int *apipoint)
{
	CShcaApi *tmpapi = getInstance();
	if (tmpapi == NULL)
	{
		return -1;
	}
	//��apiclass������������
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

// ������: UnInitCertApi
// ���  : ������ 2013-5-24 11:05:29
// ����  : int   0�ɹ�  
// ����  : �˳�֤��API
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

// ������: Sign
// ���  : ������ 2013-5-24 11:07:02
// ����  : int   0�ɹ�  
// ����  : int apipoint    api��ʵ�����
// ����  : const char *signstr  Ҫǩ�����ַ���
// ����  : unsigned short signstrlen Ҫǩ�����ַ�������
// ����  : char *signres   ǩ�����
// ����  : unsigned short *signreslen   ǩ���������
// ����  : ǩ�����ñ�����˽Կǩ��
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


// ������: GetPubicKey
// ���  : ������ 2013-5-25 11:22:50
// ����  : int   0�ɹ� 
// ����  : int apipoint    api��ʵ�����
// ����  : char *signkeybuf   ǩ����Կ
// ����  : unsigned short *signkeybuflen ǩ����Կ����
// ����  : char *encryptkkeybuf  ���ܹ�Կ
// ����  : unsigned short *encryptkkeybuflen ���ܹ�Կ����
// ����  : ȡ����֤��Ĺ�Կ
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

// ������: CheckSign
// ���  : ������ 2013-5-30 10:43:56
// ����  : int   0�ɹ� 
// ����  : int apipoint    api��ʵ�����
// ����  : const char *publickey   ǩ����Կ
// ����  : unsigned short publickeylen   ǩ����Կ����
// ����  : const char *signstr      ǩ����ԭʼ�ַ���
// ����  : unsigned short signstrlen  ǩ����ԭʼ�ַ�������
// ����  : const char *signres   ǩ�����  ע��Ҫ�н����� 0
// ����  : int signreslen ǩ���������
// ����  : У��ǩ������У��֤���Ƿ�һ����֤������
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


// ������: Encrypt
// ���  : ������ 2013-5-25 11:27:12
// ����  : int   0�ɹ�  
// ����  : int apipoint    api��ʵ�����
// ����  : const char *publickey  ���ܹ�Կ
// ����  : unsigned short publickeylen ���ܹ�Կ����
// ����  : char *data   ����->��������
// ����  : unsigned short *datalen   ���������ȣ�����ǰΪҪ���ܵ����ݳ��ȣ����ܺ�Ϊ�����������ݳ���
// ����  : ���ܣ��öԷ��Ĺ�Կ���м��ܡ����ڻỰ����ʱʹ�á�����ʱʹ�ûỰ��Կ����DES����
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


// ������: UnEncrypt
// ���  : ������ 2013-5-25 11:28:33
// ����  : int   0�ɹ� 
// ����  : int apipoint    api��ʵ�����
// ����  : char *data   ��������->��������
// ����  : unsigned short  *datalen �������ݳ��ȣ�����ǰΪ���ܵ����ݳ��ȣ����ܺ�Ϊ�������ĳ���
// ����  : �ñ�����˽Կ���н���
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



// ������: GetCertSerial
// ���  : ������ 2013-6-14 15:51:14
// ����  : int   0�ɹ� 
// ����  : int apipoint    api��ʵ�����
// ����  : const char *pubkey
// ����  : unsigned short pubkeylen
// ����  : char *serial
// ����  : int *seriallen
// ����  : ��ȡ֤Ψһ���
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


// ������: GetErrMsg
// ���  : ������ 2014-4-10 15:44:35
// ����  : void 
// ����  : int apipoint    api��ʵ�����
// ����  : ȡ������Ϣ
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
