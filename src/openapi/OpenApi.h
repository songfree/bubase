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


//ʹ��rsa�㷨   pfx�ļ�

class DLL_EXPORT COpenApi : public CICertApiBase  
{
public:
	COpenApi();
	virtual ~COpenApi();


	// ������: InitCertApi
	// ���  : ������ 2013-5-24 11:05:29
	// ����  : virtual void 
	// ����  : const char *certpath  ֤��Ŀ¼ ��󲻴�"/"
	// ����  : char *certpwd  ֤������
	// ����  : ��ʼ��֤��api
	virtual bool InitCertApi(const char *certpath,char *certpwd) ;
	
	// ������: UnInitCertApi
	// ���  : ������ 2013-5-24 11:05:29
	// ����  : virtual void 
	// ����  : �˳�֤��API
	virtual bool UnInitCertApi() ;
	
	// ������: Sign
	// ���  : ������ 2013-5-24 11:07:02
	// ����  : virtual bool 
	// ����  : const char *signstr  Ҫǩ�����ַ���
	// ����  : unsigned short signstrlen Ҫǩ�����ַ�������
	// ����  : char *signres   ǩ�����
	// ����  : unsigned short &signreslen   ǩ���������
	// ����  : ǩ�����ñ�����˽Կǩ��
	virtual bool Sign(const char *signstr,unsigned short signstrlen,char *signres,unsigned short &signreslen);
	
	
	// ������: GetPubicKey
	// ���  : ������ 2013-5-25 11:22:50
	// ����  : virtual bool 
	// ����  : char *signkeybuf   ǩ����Կ
	// ����  : unsigned short &signkeybuflen ǩ����Կ����
	// ����  : char *encryptkkeybuf  ���ܹ�Կ
	// ����  : unsigned short &encryptkkeybuflen ���ܹ�Կ����
	// ����  : ȡ����֤��Ĺ�Կ
	virtual bool GetPubicKey(char *signkeybuf,unsigned short &signkeybuflen,char *encryptkeybuf,unsigned short &encryptkkeybuflen);
	
	// ������: CheckSign
	// ���  : ������ 2013-5-30 10:43:56
	// ����  : virtual bool 
	// ����  : const char *publickey   ǩ����Կ
	// ����  : unsigned short publickeylen   ǩ����Կ����
	// ����  : const char *signstr      ǩ����ԭʼ�ַ���
	// ����  : unsigned short signstrlen  ǩ����ԭʼ�ַ�������
	// ����  : const char *signres   ǩ�����  ע��Ҫ�н����� 0
	// ����  : int signreslen ǩ���������
	// ����  : У��ǩ������У��֤���Ƿ�һ����֤������
	virtual bool CheckSign(const char *publickey,unsigned short publickeylen,const char *signstr,unsigned short signstrlen,const char *signres,unsigned short signreslen);
	
	
	// ������: Encrypt
	// ���  : ������ 2013-5-25 11:27:12
	// ����  : virtual bool 
	// ����  : const char *publickey  ���ܹ�Կ
	// ����  : unsigned short publickeylen ���ܹ�Կ����
	// ����  : char *data   ����->��������
	// ����  : unsigned short &datalen   ���������ȣ�����ǰΪҪ���ܵ����ݳ��ȣ����ܺ�Ϊ�����������ݳ���
	// ����  : ���ܣ��öԷ��Ĺ�Կ���м��ܡ����ڻỰ����ʱʹ�á�����ʱʹ�ûỰ��Կ����DES����
	virtual bool Encrypt(const char *publickey,unsigned short publickeylen,char *data,unsigned short &datalen);
	
	
	// ������: UnEncrypt
	// ���  : ������ 2013-5-25 11:28:33
	// ����  : virtual bool 
	// ����  : char *data   ��������->��������
	// ����  : unsigned short  &datalen �������ݳ��ȣ�����ǰΪ���ܵ����ݳ��ȣ����ܺ�Ϊ�������ĳ���
	// ����  : �ñ�����˽Կ���н���
	virtual bool UnEncrypt(char *data,unsigned short  &datalen);
	
	
	
	// ������: GetCertSerial
	// ���  : ������ 2013-6-14 15:51:14
	// ����  : virtual bool 
	// ����  : const char *pubkey
	// ����  : unsigned short pubkeylen
	// ����  : char *serial
	// ����  : int &seriallen
	// ����  : ��ȡ֤Ψһ���
	virtual bool GetCertSerial(const char *pubkey,unsigned short pubkeylen,char *serial,unsigned short &seriallen);


	// ������: GetErrMsg
	// ���  : ������ 2014-4-10 15:44:35
	// ����  : virtual std::string 
	// ����  : 
	virtual std::string GetErrMsg();
protected:
	
	char               m_errMsg[256];
    char               m_sPublicKey[4096];
	int                m_nPublicKeyLen;

	EVP_PKEY          *m_pPrivateKey;//PKCS12��֤��˽Կ
	EVP_MD_CTX         m_md_ctx;
	
	char               m_sMethod[10];//ǩ���������㷨����   "RSA"��SM2
	char               m_sHashMethod[10]; //hash�㷨���ͣ���ΪRSA�������ã�Ϊ"SHA-1" "SHA-256"
	char               m_sCertPwd[33];  //֤������ 
	char               m_sCertFilePath[300];//֤���ļ�(��·��)
};

//ע�⣺����Ķ�����ÿ�������涼Ҫ�У���������ݴ˷�����ȡ�����ʵ��
extern "C" 
{ 
    DLL_EXPORT COpenApi * getInstance() ; 
}

#endif // !defined(AFX_OPENAPI_H__90C341F9_1F6D_4055_A3E0_2801AF81E54D__INCLUDED_)
