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
//ע�⣺����Ķ�����ÿ�������涼Ҫ�У���������ݴ˷�����ȡ�����ʵ��
//֤���ļ���Ϊkey.pfx

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
// ������: InitCertApi
// ���  : ������ 2013-5-24 11:05:29
// ����  : virtual void 
// ����  : ��ʼ��֤��api
bool COpenApi::InitCertApi(const char *certpath,char *certpwd)
{
	//ȡ����֤�鹫Կ������ÿ�ζ�ȡ��
	
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
    //��ʼ��                                                                        
    SSLeay_add_all_algorithms();    
	OpenSSL_add_all_ciphers();
	
    ERR_load_crypto_strings();                                                        
    //��ȡ������Ϣ֤�鲢�ֽ����Կ��֤��   
	fp = fopen(m_sCertFilePath, "rb");
    if (fp == NULL)
	{    
		sprintf(m_errMsg,"��ȡ֤���ļ�[%s]�����ļ�������",m_sCertFilePath);
        return false;                                                                 
    }                                                                                 
    d2i_PKCS12_fp(fp, &p12);                                                    
    fclose (fp); 
	fp =NULL;
    if (p12 == NULL) 
	{                                                                       
		sprintf(m_errMsg,"��ȡ֤���ļ�[%s]������PKCS#12 file",m_sCertFilePath);
        return false;                                                                 
    }
	nRet = PKCS12_parse(p12, m_sCertPwd, &m_pPrivateKey, &x509, &ca);
    if (nRet != 1) 
	{                    
        sprintf(m_errMsg,"����PKCS#12֤���ļ�[%s]����",m_sCertFilePath);                                              
        PKCS12_free(p12);                                                             
        return false;                                                                 
    }                                                                                 
    PKCS12_free(p12);  
	p12 = NULL;
    if (m_pPrivateKey == NULL) 
	{                                                               
        sprintf(m_errMsg,"����PKCS#12֤���ļ�[%s]����,˽ԿΪ��",m_sCertFilePath);                                               
        return false;                                                                 
    }                                                                                 
    //ǩ����ʼ����     ʹ��sha256ժҪ�㷨                                                                 
    EVP_SignInit(&m_md_ctx, EVP_sha256());
	//��ȡ��Կ���ɱ�����
	EVP_PKEY *publickey = X509_get_pubkey(x509);
	unsigned char *key = (unsigned char *)m_sPublicKey;
	m_nPublicKeyLen = i2d_PUBKEY(publickey, &key);

	m_sPublicKey[m_nPublicKeyLen]=0;
	
	X509_free(x509); 
	EVP_PKEY_free(publickey);
	return true;
}

// ������: UnInitCertApi
// ���  : ������ 2013-5-24 11:05:29
// ����  : virtual void 
// ����  : �˳�֤��API
bool COpenApi::UnInitCertApi()
{
	if (m_pPrivateKey != NULL)
	{
		EVP_PKEY_free(m_pPrivateKey);
		m_pPrivateKey = NULL;
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
bool COpenApi::Sign(const char *signstr,unsigned short signstrlen,char *signres,unsigned short &signreslen)
{
	char *ssign=NULL;
	int sdatalen = signstrlen;
	//ǩ����ʼ����     ʹ��sha256ժҪ�㷨
	EVP_SignInit(&m_md_ctx, EVP_sha256());  
	
    int nRet = EVP_SignUpdate (&m_md_ctx, signstr, signstrlen);    
	if (nRet != 1)
	{
		sprintf(m_errMsg,"EVP_SignUpdate���� %d",nRet);
		return false;
	}
    unsigned int reslen = 128;                                                                    
    nRet = EVP_SignFinal(&m_md_ctx, (unsigned char *)signres, &reslen, m_pPrivateKey);                           
    if (nRet != 1) 
	{                                                                   
		sprintf(m_errMsg,"ǩ��EVP_SignFinal���� %d",nRet);                                                                            
        return false;                                                                 
    }   
	signreslen = reslen;
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
bool COpenApi::GetPubicKey(char *signkeybuf,unsigned short &signkeybuflen,char *encryptkeybuf,unsigned short &encryptkkeybuflen)
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
bool COpenApi::CheckSign(const char *publickey,unsigned short publickeylen,const char *signstr,unsigned short signstrlen,const char *signres,unsigned short signreslen)
{
	EVP_PKEY *certpubkey = NULL;
	const unsigned char *pkey = (unsigned char *)publickey;
	long llen = publickeylen;
	//�ӻ����ж�ȡ��Կ
	if (d2i_PUBKEY(&certpubkey,&pkey,llen) == NULL)
	{
		sprintf(m_errMsg,"����d2i_PUBKEY�ӻ����ȡ��Կʧ��");
		return false;
	}
	int nRet;                                                         
    EVP_MD_CTX     md_ctx;                                                            
    unsigned int sig_len = signreslen;  
	//ժҪ�㷨Ϊsha256
    EVP_VerifyInit(&md_ctx, EVP_sha256());                                              
    EVP_VerifyUpdate(&md_ctx, signstr, signstrlen);  
	//��֤ǩ��
    nRet = EVP_VerifyFinal(&md_ctx, (const unsigned char *)signres, sig_len, certpubkey);          
    EVP_PKEY_free (certpubkey);                                                             
    if (nRet != 1) 
	{                                                                   
        sprintf(m_errMsg,"����EVP_VerifyFinal��֤ǩ��ʧ�� %d",nRet);                                                               
        return false;                                                                 
    }  
	//��֤��֤���


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
bool COpenApi::Encrypt(const char *publickey,unsigned short publickeylen,char *data,unsigned short &datalen)
{
	//תΪRSA˽Կ
	RSA *rsa=NULL;
	EVP_PKEY *certpubkey = NULL;
	const unsigned char *pkey = (unsigned char *)publickey;
	long llen = publickeylen;
	//�ӻ����ж�ȡ��Կ
	if (d2i_PUBKEY(&certpubkey,&pkey,llen) == NULL)
	{
		sprintf(m_errMsg,"����d2i_PUBKEY�ӻ����ȡ��Կʧ��");
		return false;
	}
	rsa = EVP_PKEY_get1_RSA(certpubkey);
//	rsa=d2i_RSAPublicKey(NULL,(const unsigned char**)&(certpubkey->pkey.ptr),publickeylen);
	if (rsa == NULL)
	{
		sprintf(m_errMsg,"ȡ��RSA��Կ����EVP_PKEY_get1_RSA����");
		return false;
	}
	unsigned char resbuffer[8192];
	bzero(resbuffer,sizeof(resbuffer));
	unsigned char tmpbuffer[129];
	int   tmplen =0;
	//��117���ֽ�Ϊһ����Ԫ���м���
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
			sprintf(m_errMsg,"����RSA_public_encrypt���ܳ��� %d",nRet);
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


// ������: UnEncrypt
// ���  : ������ 2013-5-25 11:28:33
// ����  : virtual bool 
// ����  : char *data   ��������->��������
// ����  : unsigned short  &datalen �������ݳ��ȣ�����ǰΪ���ܵ����ݳ��ȣ����ܺ�Ϊ�������ĳ���
// ����  : �ñ�����˽Կ���н���
bool COpenApi::UnEncrypt(char *data,unsigned short  &datalen)
{
	int nRet;
	unsigned char *res=NULL;
	int  reslen;
	

	//תΪRSA˽Կ
	RSA *rsa=NULL;
	rsa = EVP_PKEY_get1_RSA(m_pPrivateKey);
	if (rsa == NULL)
	{
		sprintf(m_errMsg,"ȡ��RSA˽Կ����EVP_PKEY_get1_RSA����");
		return false;
	}
	unsigned char resbuffer[8192];
	bzero(resbuffer,sizeof(resbuffer));
	unsigned char tmpbuffer[129];
	int   tmplen =0;
	//��128���ֽ�Ϊһ����Ԫ���м���
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
			sprintf(m_errMsg,"����RSA_private_decrypt���ܳ��� %d",nRet);
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



// ������: GetCertSerial
// ���  : ������ 2013-6-14 15:51:14
// ����  : virtual bool 
// ����  : const char *pubkey
// ����  : unsigned short pubkeylen
// ����  : char *serial
// ����  : int &seriallen
// ����  : ��ȡ֤Ψһ���
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