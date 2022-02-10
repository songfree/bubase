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

	// ������:  SignData_PKCS1Ptr
	// ���  : ������ 2014-4-10 16:53:19
	// ����  : int 
	// ����  : char* method            ǩ������ "SM2"��"RSA"
	// ����  : unsigned char* sdata    ǩ��ԭ������
	// ����  : int sdatalen            ǩ��ԭ�ĳ���
	// ����  : char* signcertfilepath  ǩ��֤���ļ���·��
	// ����  : char* certpwd           ǩ��֤������
	// ����  : char* hashmethod        ��ϣ�㷨 ����RSA���ã����� SHA-1/SHA-256����
	// ����  : char** signresult       ǩ�����
	// ����  : �����ݽ���PKCS#1ǩ��
typedef int  (WINAPI *SignData_PKCS1Ptr)(char* method, unsigned char* sdata, int sdatalen, char* signcertfilepath, char* certpwd, char* hashmethod, char** signresult);

// ������:  SignData_PKCS7AttachedPtr
// ���  : ������ 2014-4-10 16:53:19
// ����  : int 
// ����  : char* method            ǩ������ "SM2"��"RSA"
// ����  : unsigned char* sdata    ǩ��ԭ������
// ����  : int sdatalen            ǩ��ԭ�ĳ���
// ����  : char* signcertfilepath  ǩ��֤���ļ���·��
// ����  : char* certpwd           ǩ��֤������
// ����  : char* hashmethod        ��ϣ�㷨 ����RSA���ã����� SHA-1/SHA-256����
// ����  : char** signresult       ǩ�����
// ����  : �����ݽ���PKCS#7��ԭ��ǩ��
typedef int  (WINAPI *SignData_PKCS7AttachedPtr)(char* method, unsigned char* sdata, int sdatalen, char* signcertfilepath, char* certpwd, char* hashmethod, char** signresult);

// ������:  SignData_PKCS7AttachedPtr
// ���  : ������ 2014-4-10 16:53:19
// ����  : int 
// ����  : char* method            ǩ������ "SM2"��"RSA"
// ����  : unsigned char* sdata    ǩ��ԭ������
// ����  : int sdatalen            ǩ��ԭ�ĳ���
// ����  : char* signcertfilepath  ǩ��֤���ļ���·��
// ����  : char* certpwd           ǩ��֤������
// ����  : char* hashmethod        ��ϣ�㷨 ����RSA���ã����� SHA-1/SHA-256����
// ����  : char** signresult       ǩ�����
// ����  : �����ݽ���PKCS#7����ԭ��ǩ��
typedef int  (WINAPI *SignData_PKCS7DetachedPtr)(char* method, unsigned char* sdata, int sdatalen, char* signcertfilepath, char* certpwd, char* hashmethod, char** signresult);

typedef int  (WINAPI *SignFile_PKCS7DetachedPtr)(char*, char*, char*, char*, char*, char**);

// ������:  VerifyDataSignature_PKCS1Ptr
// ���  : ������ 2014-4-14 15:19:43
// ����  : int 
// ����  : char* method            ǩ������ "SM2"��"RSA"
// ����  : unsigned char* sdata    ǩ��ԭ������
// ����  : int sdatalen            ǩ��ԭ�ĳ���
// ����  : char* publickey         У�����ù�Կ
// ����  : char* hashmethod        ��ϣ�㷨 ����RSA���ã����� SHA-1/SHA-256����
// ����  : char* pkcssign          pkcsǩ�����
// ����  : ������ǩ��������֤
typedef int  (WINAPI *VerifyDataSignature_PKCS1Ptr)(char* method, unsigned char* sdata, int sdatalen, char* publickey, char* hashmethod, char* pkcssign);

// ������:  VerifyDataSignature_PKCS7DetachedPtr
// ���  : ������ 2014-4-14 15:19:43
// ����  : int 
// ����  : char* method            ǩ������ "SM2"��"RSA"
// ����  : unsigned char* sdata    ǩ��ԭ������
// ����  : int sdatalen            ǩ��ԭ�ĳ���
// ����  : char* publickey         У�����ù�Կ
// ����  : char* hashmethod        ��ϣ�㷨 ����RSA���ã����� SHA-1/SHA-256����
// ����  : char* pkcssign          pkcsǩ�����
// ����  : ������ǩ��������֤
typedef int  (WINAPI *VerifyDataSignature_PKCS7DetachedPtr)(char* method, unsigned char* sdata, int sdatalen, char* pkcssign, char** publickey);
typedef int  (WINAPI *VerifyDataSignature_PKCS7AttachedPtr)(char*, char*, char**, unsigned char**, int*);
typedef int  (WINAPI *VerifyFileSignature_PKCS7DetachedPtr)(char*, char*, char*, char**);


// ������:  EncryptDataToCMSEnvelopePtr
// ���  : ������ 2014-4-14 15:30:34
// ����  : int 
// ����  : char* method            ���� "SM2"��"RSA"
// ����  : unsigned char*  sdata   ��������
// ����  : int    sdatalen         �������ݳ���
// ����  : char*  publickey        ��Կ
// ����  : char*  encryptmethod    ���ܷ��� �ԳƼ����㷨�����롰RC4����3DES��
// ����  : char**  encryptresult   ���ܽ��
// ����  : ��Կ����
typedef int  (WINAPI *EncryptDataToCMSEnvelopePtr)(char* method, unsigned char* sdata, int sdatalen, char* publickey, char* encryptmethod, char** encryptresult);
typedef int  (WINAPI *SignAndEncryptDataPtr)(char*, unsigned char*, int, char*, char*, char*, char*, char*, char**);
// ������:  EncryptDataToCMSEnvelopePtr
// ���  : ������ 2014-4-14 15:30:34
// ����  : int 
// ����  : char* method            ���� "SM2"��"RSA"
// ����  : char*  sdata            ��������  
// ����  : char*  certfilepath     ֤���ļ�
// ����  : char*  certpwd          ֤������
// ����  : unsigned char**  resdata          ���Ľ��
// ����  : int *  resdatalen       ���Ľ������
// ����  : ���ݽ���
typedef int  (WINAPI *DecryptDataFromCMSEnvelopePtr)(char* method, char* sdata, char* certfilepath, char* certpwd, unsigned char** resdata, int* resdatalen);

typedef int  (WINAPI *DecryptAndVerifyDataSignaturePtr)(char*, char*, char*, char*, unsigned char**, int*);
typedef int  (WINAPI *VerifyCertificatePtr)(char*, int, char*, char*);

// ������:  GetCertificateInfoPtr
// ���  : ������ 2014-4-14 15:54:19
// ����  : int 
// ����  : char*   ֤������  ��Կ
// ����  : char*   ֤����Ϣ����  
// ����  : char**  ֤����Ϣ
// ����  : ȡ��֤����Ϣ
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


//ʹ��rsa�㷨   pfx�ļ�

class DLL_EXPORT CCfcaApi : public CICertApiBase  
{
public:
	CCfcaApi();
	virtual ~CCfcaApi();


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
	SADK_ENTRYPOINTS   m_pCfcaPoint;

	DLLHANDLE          m_pDllHandle;
	char               m_errMsg[256];
	char               m_sPublicKey[4096];
	int                m_nPublicKeyLen;
	
	char               m_sMethod[10];//ǩ���������㷨����   "RSA"��SM2
	char               m_sHashMethod[10]; //hash�㷨���ͣ���ΪRSA�������ã�Ϊ"SHA-1" "SHA-256"
	char               m_sCertPwd[33];  //֤������ 
	char               m_sCertFilePath[300];//֤���ļ�(��·��)
};

//ע�⣺����Ķ�����ÿ�������涼Ҫ�У���������ݴ˷�����ȡ�����ʵ��
extern "C" 
{ 
    DLL_EXPORT CCfcaApi * getInstance() ; 
}

#endif // !defined(AFX_CFCAAPI_H__90C341F9_1F6D_4055_A3E0_2801AF81E54D__INCLUDED_)
