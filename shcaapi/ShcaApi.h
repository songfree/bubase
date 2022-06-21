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


	// ������: 
	// ���  : ������ 2016-2-18 10:44:50
	// ����  : long 
	// ����  : HSE *hSE
	// ����  : unsigned long privatekeydevicetype   �洢˽Կ�豸����  2�ļ�
	// ����  : char *privatekeydeviceparameter      �洢˽Կ�豸����  Ϊ2ʱΪ֤���ļ���
	// ����  : char *privatekeypassword             ֤������ (˽Կ��������)
	// ����  : unsigned long privatekeytimeout      ˽Կ��ʱʱ��
	// ����  : unsigned long certchaindevicetype    �洢��֤���豸������ 2�ļ�
	// ����  : char *certchaindeviceparameter       �洢��֤���豸�Ĳ���  Ϊ2ʱΪ֤���ļ���
	// ����  : char *certchainpassword              ��֤������  ��Ϊic��ʱ��Ч
	// ����  : ��ʼ��
typedef long (*SEH_InitialSessionPtr)(HSE *hSE, unsigned long privatekeydevicetype,  char *privatekeydeviceparameter, char *privatekeypassword,  unsigned long privatekeytimeout,  unsigned long certchaindevicetype,  char *certchaindeviceparameter,  char *certchainpassword );
typedef long (*SEH_InitialSessionExPtr)(HSE *hSE, unsigned char * privatekey, unsigned short privatekeylen, char *privatekeypassword, unsigned char *rootcert, unsigned short rootcertlen);

	// ������: 
	// ���  : ������ 2016-2-18 10:49:11
	// ����  : long 
	// ����  : ClearSessionPtr
	// ����  : �����������
typedef long (*SEH_ClearSessionPtr)(HSE hSE);


	// ������: 
	// ���  : ������ 2016-2-18 10:50:56
	// ����  : typedef long 
	// ����  : HSE hSE
	// ����  : unsigned char *origindata   ԭʼ���ݿ�
	// ����  : unsigned long length        ԭʼ���ݳ���
    // ����  : unsigned short signmethod   ǩ������  1: MD2   2: MD5   3: SHA1
	// ����  : unsigned char *signeddata   ǩ����� 
	// �����������  : unsigned long *signedlength  ǩ���������
	// ����  : ǩ��(��˽Կǩ�����Զ��ñ����Ĺ�Կ��ǩ)
typedef long (*SEH_SignDataPtr)(HSE hSE, unsigned char *origindata, unsigned long length, unsigned short signmethod, unsigned char *signeddata, unsigned long *signedlength);

	// ������: 
	// ���  : ������ 2016-2-18 10:51:30
	// ����  : typedef long 
	// ����  : HSE hSE
	// ����  : unsigned char *origindata   ԭʼ���ݿ�
	// ����  : unsigned long originlength  ԭʼ���ݳ���
	// ����  : unsigned short signmethod   ǩ������  1: MD2   2: MD5   3: SHA1
	// ����  : unsigned char *signeddata   ǩ�����
	// ����  : unsigned long signedlength  ǩ���������
	// ����  : unsigned char *certificate  ��Կ֤��
	// ����  : unsigned short certlength   ��Կ֤�鳤��
	// ����  : �öԶ˵Ĺ�Կ��֤ǩ��
typedef long (*SEH_VerifySignDataPtr)(HSE hSE, unsigned char *origindata, unsigned long originlength, unsigned short signmethod, unsigned char *signeddata, unsigned long signedlength, unsigned char *certificate, unsigned short certlength);


	// ������: 
	// ���  : ������ 2016-2-18 10:52:08
	// ����  : typedef long 
	// ����  : HSE hSE
	// ����  : unsigned char *data   ����
	// ����  : unsigned long length  ���ݳ���
	// ����  : unsigned short method  ժҪ����  1: MD2   2: MD5   3: SHA1
	// ����  : unsigned char *digest  ժҪ����
	// ����  : unsigned long  *digestlength  ժҪ����
	// ����  : ժҪ
typedef long (*SEH_DigestPtr)(HSE hSE, unsigned char *data, unsigned long length, unsigned short method, unsigned char *digest, unsigned long  *digestlength);


	// ������: 
	// ���  : ������ 2016-2-18 10:52:19
	// ����  : typedef long 
	// ����  : HSE hSE
	// ����  : unsigned short envolpetype   �������� 1��������ŷ�   2��������ŷ�
	// ����  : unsigned char *indata        ��������
	// ����  : unsigned long inlength       �������ݳ���
	// ����  : unsigned char *outdata       �������
	// ����  : unsigned long *outlength     ������ݳ���
	// ����  : unsigned char *certificate   ֤������   ��������ʱ����  Ϊ�Զ˵Ĺ�Կ
	// ����  : unsigned short certlength    ֤�鳤��
	// ����  : �����ŷ���ܽ��ܣ�����ʹ�öԶ˵Ĺ�Կ������ʹ�ñ��˵�˽Կ
typedef long (*SEH_EnvelopePtr)(HSE hSE, unsigned short envolpetype, unsigned char *indata, unsigned long inlength, unsigned char *outdata, unsigned long *outlength, unsigned char *certificate, unsigned short certlength);
typedef long (*SEH_EnvelopeExPtr)(HSE hSE, unsigned short envolpetype, unsigned char *indata, unsigned long inlength, unsigned char *outdata, unsigned long *outlength, unsigned char *certificate, unsigned short certlength, int AlgID);


	// ������: 
	// ���  : ������ 2016-2-18 10:52:50
	// ����  : typedef long 
	// ����  : HSE hSE
	// ����  : unsigned long devicetype  �豸���� 2Ϊ�ļ�
	// ����  : char *deviceparameter     �豸���� ֤���ļ���
	// ����  : char *password            ֤������
	// ����  : unsigned char *certificate    ֤������
	// �����������  : unsigned short *certificatelength    ֤�����ݳ���
	// ����  : ȡ֤������  ֤���Ϊ��Կ֤���˽Կ֤��
typedef long (*SEH_GetSelfCertificatePtr)(HSE hSE, unsigned long devicetype, char *deviceparameter, char *password, unsigned char *certificate, unsigned short *certificatelength);


	// ������: 
	// ���  : ������ 2016-2-18 11:22:34
	// ����  : typedef long 
	// ����  : HSE hSE
	// ����  : unsigned char *certificate   ֤������
	// ����  : unsigned short length        ֤�鳤��
	// ����  : ��֤֤����Ч�ԣ�������Ч�ڣ�caǩ�����������þ����Ƿ���֤���غ�������ca������
typedef long (*SEH_VerifyCertificatePtr)(HSE hSE, unsigned char *certificate, unsigned short length);

	// ������: 
	// ���  : ������ 2016-2-18 11:24:12
	// ����  : typedef long 
	// ����  : WINAPI *SEH_GetCertificatePtr)(HSE hSE
	// ����  : char *SerialNo  ֤�����к�  16���Ʊ�ʾ
	// ����  : char *URL       ֤���ѯURL   ��ʽ: IP : PORT
	// ����  : unsigned char *certificate   ֤������
	// �����������  : unsigned short *certlen      ֤�����ݳ���
	// ����  : �������ȡ֤��
typedef long (*SEH_GetCertificatePtr)(HSE hSE, char *SerialNo, char *URL, unsigned char *certificate, unsigned short *certlen);


	// ������: 
	// ���  : ������ 2016-2-18 11:26:10
	// ����  : typedef long 
	// ����  : HSE hSE
	// ����  : unsigned char *certificate   ֤������
	// ����  : unsigned short length        ֤�鳤��
	// ����  : ������֤֤����Ч�ԣ�������Ч�ڣ�caǩ����Ȼ��ocsp
typedef long (*SEH_VerifyCertificateOnlinePtr)(HSE hSE, unsigned char *certificate, unsigned short length);

	// ������: 
	// ���  : ������ 2016-2-18 13:05:29
	// ����  : typedef long 
	// ����  :  HSE hSE
	// ����  : unsigned long device
	// ����  : char *deviceparameter
	// ����  : char *oldpassword  ������
	// ����  : char *newpassword  ������
	// ����  : ���Ĵ洢���ʵ�����
typedef long (*SEH_ChangePasswordPtr)(HSE hSE, unsigned long device, char *deviceparameter, char *oldpassword, char *newpassword);

//typedef long (*SEH_GetCertInfoPtr)(HSE hSE, unsigned char *DerCert, unsigned short DerCertLen, CERT_INFO *pCertInfo);

	// ������: 
	// ���  : ������ 2016-2-18 10:56:15
	// ����  : typedef long 
	// ����  : HSE hSE
	// ����  : unsigned char *DerCert      �������֤���DER���봮����ʼ��ַ
	// ����  : unsigned short DerCertLen   �������֤���DER���봮�ĳ���
	// ����  : unsigned short ItemNo  ��Ŀ id
	// ����  : unsigned char *ItemValue  ��Ŀֵ
	// �����������  : unsigned short *ItemLength  ��Ŀֵ����
	// ����  : ��ȡ֤��ϸĿ   
typedef long (*SEH_GetCertDetailPtr)(HSE hSE, unsigned char *DerCert, unsigned short DerCertLen, unsigned short ItemNo, unsigned char *ItemValue, unsigned short *ItemLength);


	// ������: 
	// ���  : ������ 2016-2-18 11:49:34
	// ����  : typedef long 
	// ����  : HSE hSE
	// ����  : unsigned char *indata    ����
	// ����  : unsigned long indatalen  ���ݳ���
	// ����  : unsigned char *pemdata   ���PEM����
	// �����������  : unsigned long *pemlen    PEM���ݳ���
	// ����  : PEM����
typedef long (*SEH_PEMEncodePtr)(HSE hSE, unsigned char *indata, unsigned long indatalen, unsigned char *pemdata, unsigned long *pemlen);

	// ������: 
	// ���  : ������ 2016-2-18 11:49:39
	// ����  : typedef long 
	// ����  : HSE hSE
	// ����  : unsigned char *pemdata  ����PEM����
	// ����  : unsigned long pemlen    pem���ݳ���
	// ����  : unsigned char *outdata   �������
	// �����������  : unsigned long *outlen    ���ݳ���
	// ����  : PEM����
typedef long (*SEH_PEMDecodePtr)(HSE hSE, unsigned char *pemdata, unsigned long pemlen, unsigned char *outdata, unsigned long *outlen);


	// ������: 
	// ���  : ������ 2016-2-18 11:55:19
	// ����  : typedef long 
	// ����  : HSE hSE
    // ����  : long mode   Bit 0 = 1:��֤����Ч��ʱ���CA�ĺ�������.Bit 0 = 0: ����֤������.Bit1 = 1:OCSP��֤֤��ʱ��֤����Ҳ��ocsp��֤.Bit1 = 0: OCSP��֤֤��ʱ��֤���������Ƿ�ϳ���֤.Bit 1 - Bit 31: ����.ȱʡ���ò���������
	// ����  : ��������  ��������һЩ������ѡ��.  ����֤֤��ʱ�Ƿ���֤������
typedef long (*SEH_SetConfigurationPtr)(HSE hSE, long mode);



	// ������: 
	// ���  : ������ 2016-2-18 11:57:39
	// ����  : typedef long 
	// ����  : HSE hSE
	// ����  : unsigned char *randombytes   ���������������
	// ����  : int randombyteslen           Ҫ���������������
	// ����  : ���������Կ
typedef long (*SEH_GenRandomBytesPtr)(HSE hSE, unsigned char *randombytes, int randombyteslen);

	// ������: 
	// ���  : ������ 2016-2-18 11:58:17
	// ����  : typedef long 
	// ����  : HSE hSE
	// ����  : unsigned char *indata   Ҫ���ܵ�����
	// ����  : int indatalen           Ҫ���ܵ����ݳ���
	// ����  : unsigned char *outdata   ��������
	// �����������  : int *outdatalen          �������ݳ���
	// ����  : unsigned char *sdbikey   ������Կ  16�ֽڳ���
	// ����  : �öԳ��㷨��SDBI������Ӧ������
typedef long (*SEH_EncryptDataPtr)(HSE hSE, unsigned char *indata, int indatalen, unsigned char *outdata, int *outdatalen, unsigned char *sdbikey);

	// ������: 
	// ���  : ������ 2016-2-18 12:41:38
	// ����  : typedef long 
	// ����  : HSE hSE
	// ����  : unsigned char *indata  ��������
	// ����  : int indatalen          �������ݳ���
	// ����  : unsigned char *outdata   ����
	// �����������  : int *outdatalen          ���ݳ���
	// ����  : unsigned char *sdbikey  ������Կ  16�ֽڳ���
	// ����  : �öԳ��㷨��SDBI������Ӧ������
typedef long (*SEH_DecryptDataPtr)(HSE hSE, unsigned char *indata, int indatalen, unsigned char *outdata, int *outdatalen, unsigned char *sdbikey);
typedef long (*SEH_EncryptDataExPtr)(HSE hSE, unsigned char *indata, int indatalen, unsigned char *outdata, int *outdatalen, unsigned char *key, int keylen, int AlgID);
typedef long (*SEH_DecryptDataExPtr)(HSE hSE, unsigned char *indata, int indatalen, unsigned char *outdata, int *outdatalen, unsigned char *key, int keylen, int AlgID);

	// ������: 
	// ���  : ������ 2016-2-18 12:43:31
	// ����  : typedef long 
	// ����  : HSE hSE
	// ����  : unsigned char * pcDERCert  Der�����֤��
	// ����  : unsigned long lCertSize    ֤�鳤��
	// ����  : char * pcPassword          ֤������
	// ����  : unsigned char * pcP12Buffer    PKCS12��ʽ����
	// �����������  : unsigned long * plP12Size      PKCS12��ʽ���ݳ���
	// ����  : ��֤�飬˽Կ�����pkcs12��ʽ
typedef long (*SEH_EncodePKCS12Ptr)(HSE hSE,unsigned char * pcDERCert,unsigned long lCertSize,char * pcPassword,unsigned char * pcP12Buffer,unsigned long * plP12Size);


	// ������: 
	// ���  : ������ 2016-2-18 12:46:22
	// ����  : typedef long 
	// ����  : HSE hSE
	// ����  : unsigned char *cert  �������֤���DER���봮����ʼ��ַ
	// ����  : unsigned short certlen    ����
	// ����  : char *uniqueid             Ψһ��ʶ
	// �����������  : unsigned short *idlen    ����Ϊ�����С ���ΪΨһ��ʶ����
	// ����  : ȡ֤��Ψһ��ʶ
typedef long (*SEH_GetCertUniqueIDPtr)(HSE hSE, unsigned char *cert, unsigned short certlen, char *uniqueid, unsigned short *idlen);

	// ������: 
	// ���  : ������ 2016-2-18 12:51:38
	// ����  : typedef long 
	// ����  : HSE hSE
	// ����  : unsigned char *cert    ֤������
	// ����  : unsigned short certlen   ֤�����ݴ�С
	// ����  : char *oid      Sheca�������չ��id
	// ����  : char *info     ��Ϣ����
	// ����  : unsigned short *infolen   ��Ϣ����
	// ����  : ȡ֤����չ����Ϣ
typedef long (*SEH_GetCertInfoByOIDPtr)(HSE hSE, unsigned char *cert, unsigned short certlen, char *oid, char *info, unsigned short *infolen);


	// ������: 
	// ���  : ������ 2016-2-18 12:53:11
	// ����  : typedef long 
	// ����  : WINAPI *SEH_ShowVersionPtr)(HSE hSE
	// ����  : long *plModualNo   ģ���
	// ����  : long *plVersion    �汾��
	// ����  : ��ȡAPI��ǰ�汾��Ϣ
typedef long (*SEH_ShowVersionPtr)(HSE hSE, long *plModualNo, long *plVersion);


	// ������: 
	// ���  : ������ 2016-2-18 12:54:09
	// ����  : typedef long 
	// ����  : HSE hSE
	// ����  : unsigned char *cert   ֤������
	// ����  : unsigned short certlen  ֤�����ݳ���
	// ����  : long *datanum    ��Ч����
	// ����  : ��ȡ֤��ʣ�����Ч����
typedef long (*SEH_GetCertValidDatePtr)(HSE hSE, unsigned char *cert, unsigned short certlen, long *datanum);

	// ������: 
	// ���  : ������ 2016-2-18 12:55:15
	// ����  : typedef long 
	// ����  : HSE hSE
	// ����  : unsigned char *cert
	// ����  : unsigned short certlen
	// ����  : unsigned long *lCertClass  �������֤��  0x00000103 ����EMAIL֤��	 0x00000102 ���˴���ǩ��֤��	 0x00000105 ��λ���֤��		 0x00000203 ��λEAIL֤��   0x00000202 ��λ����ǩ��֤��	 0x00000204 ���������֤��   0x00000304 WEB������֤��  0x00000301 
	// ����  : ȡ֤�鼶��
typedef long (*SEH_GetCertClassPtr)(HSE hSE, unsigned char *cert, unsigned short certlen, unsigned long *lCertClass);

	// ������: 
	// ���  : ������ 2016-2-18 13:01:10
	// ����  : typedef long 
	// ����  : WINAPI *SEH_GetCertUsagePtr)(HSE hSE
	// ����  : unsigned char *cert
	// ����  : unsigned short certlen
	// ����  : unsigned long *lKeyUsage
	// ����  : ȡ֤����;
typedef long (*SEH_GetCertUsagePtr)(HSE hSE, unsigned char *cert, unsigned short certlen, unsigned long *lKeyUsage);

typedef long (*SEH_GetDeviceListPtr)(HSE hSE, int *totalDevNum, unsigned long *devType);
typedef long (*SEH_GetDeviceInfoPtr)(HSE hSE, unsigned long devType, DEVICE_INFO *pdevInfo);
typedef long (*SEH_GetDeviceTypePtr)(HSE hSE, unsigned short ReaderID, unsigned short CardType, unsigned long *devType);

 

typedef struct _SHCA_ENTRYPOINTS
{
    SEH_InitialSessionPtr                SEH_InitialSession; //��ʼ��
    SEH_ClearSessionPtr                  SEH_ClearSession;//�������
	SEH_SignDataPtr                      SEH_SignData;    //ǩ��
	SEH_VerifySignDataPtr                SEH_VerifySignData;//У��ǩ��
	SEH_DigestPtr                        SEH_Digest; //ժҪ
	SEH_EnvelopePtr                      SEH_Envelope;   //�����ŷ�
	SEH_GetSelfCertificatePtr            SEH_GetSelfCertificate;//ȡ֤��
	SEH_VerifyCertificatePtr             SEH_VerifyCertificate;//У��֤��
	SEH_ChangePasswordPtr                SEH_ChangePassword;//������
	SEH_GetCertDetailPtr                 SEH_GetCertDetail;//ȡ֤��ϸĿ
	SEH_PEMEncodePtr                     SEH_PEMEncode;//pem����
	SEH_PEMDecodePtr                     SEH_PEMDecode;//pem����
	SEH_SetConfigurationPtr              SEH_SetConfiguration;//���ò���
	SEH_GenRandomBytesPtr                SEH_GenRandomBytes;//���������Կ
	SEH_EncryptDataPtr                   SEH_EncryptData;//�ԳƼ���
	SEH_DecryptDataPtr                   SEH_DecryptData;//�Գƽ���
	SEH_EncodePKCS12Ptr                  SEH_EncodePKCS12;//��֤�飬˽Կ�����pkcs12��ʽ
	SEH_GetCertUniqueIDPtr               SEH_GetCertUniqueID;//ȡ֤��Ψһ��ʶ
	SEH_GetCertInfoByOIDPtr              SEH_GetCertInfoByOID;//ȡ֤����չ����Ϣ
	SEH_ShowVersionPtr                   SEH_ShowVersion;//��ȡapi�汾
	SEH_GetCertValidDatePtr              SEH_GetCertValidDate;//ȡ֤����Ч����
	SEH_GetCertClassPtr                  SEH_GetCertClass;//ȡ֤�ռ���
	SEH_GetCertUsagePtr                  SEH_GetCertUsage;//ȡ֤����;
}SHCA_ENTRYPOINTS, *LPSHCA_ENTRYPOINTS;


class DLL_EXPORT CShcaApi : public CICertApiBase  
{
public:
	CShcaApi();
	virtual ~CShcaApi();

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
	SHCA_ENTRYPOINTS   m_pShcaPoint;
	HSE                m_hSE;               //�Ϻ�CA
	DLLHANDLE          m_pDllHandle;
	char               m_errMsg[256];

	char               m_sPublicKey[4096];
	unsigned short     m_nPublicKeyLen;
	

	char               m_sCertPwd[33];  //֤������ 
	char               m_sCertFilePath[300];//˽Կ֤���ļ�(��·��) UserKey.key
	char               m_sPubFilePath[300]; //��Կ֤���ļ�(��·��) UserCert.der
	char               m_sRootFilePath[300];//֤�����ļ�(��·��)   RootCert.der
};

//ע�⣺����Ķ�����ÿ�������涼Ҫ�У���������ݴ˷�����ȡ�����ʵ��
extern "C" 
{ 

	// ������: * getInstance
	// ���  : ������ 2016-2-18 16:25:38
	// ����  : DLL_EXPORT CShcaApi 
	// ����  : �õ�һ��apiʵ��
    DLL_EXPORT CShcaApi * getInstance() ;

	// ������: InitCertApi
	// ���  : ������ 2013-5-24 11:05:29
	// ����  : int  >=0�ɹ� <0ʧ��   �ɹ���Ϊʵ������ţ��´�ʹ��Ҫ����Ŵ���
	// ����  : const char *certpath  ֤��Ŀ¼ ��󲻴�"/"
	// ����  : char *certpwd  ֤������
	// ����  : int *apipoint  apiʵ������ţ����߳�ʹ��ʱÿ���߳�һ��ʵ��
	// ����  : ��ʼ��֤��api
	DLL_EXPORT int SHCAAPI_InitCertApi(const char *certpath,const char *certpwd,int *apipoint) ;
	
	// ������: UnInitCertApi
	// ���  : ������ 2013-5-24 11:05:29
	// ����  : int   0�ɹ�  
	// ����  : �˳�֤��API
	DLL_EXPORT int SHCAAPI_UnInitCertApi(int apipoint) ;
	
	// ������: Sign
	// ���  : ������ 2013-5-24 11:07:02
	// ����  : int   0�ɹ�  
	// ����  : int apipoint    api��ʵ�����
	// ����  : const char *signstr  Ҫǩ�����ַ���
	// ����  : unsigned short signstrlen Ҫǩ�����ַ�������
	// ����  : char *signres   ǩ�����
	// ����  : unsigned short *signreslen   ǩ���������
	// ����  : ǩ�����ñ�����˽Կǩ��
	DLL_EXPORT int SHCAAPI_Sign(int apipoint,const char *signstr,unsigned short signstrlen,char *signres,unsigned short *signreslen);
	
	
	// ������: GetPubicKey
	// ���  : ������ 2013-5-25 11:22:50
	// ����  : int   0�ɹ� 
	// ����  : int apipoint    api��ʵ�����
	// ����  : char *signkeybuf   ǩ����Կ
	// ����  : unsigned short *signkeybuflen ǩ����Կ����
	// ����  : char *encryptkkeybuf  ���ܹ�Կ
	// ����  : unsigned short *encryptkkeybuflen ���ܹ�Կ����
	// ����  : ȡ����֤��Ĺ�Կ
	DLL_EXPORT int SHCAAPI_GetPubicKey(int apipoint,char *signkeybuf,unsigned short *signkeybuflen,char *encryptkeybuf,unsigned short *encryptkkeybuflen);
	
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
	DLL_EXPORT int SHCAAPI_CheckSign(int apipoint,const char *publickey,unsigned short publickeylen,const char *signstr,unsigned short signstrlen,const char *signres,unsigned short signreslen);
	
	
	// ������: Encrypt
	// ���  : ������ 2013-5-25 11:27:12
	// ����  : int   0�ɹ�  
	// ����  : int apipoint    api��ʵ�����
	// ����  : const char *publickey  ���ܹ�Կ
	// ����  : unsigned short publickeylen ���ܹ�Կ����
	// ����  : char *data   ����->��������
	// ����  : unsigned short *datalen   ���������ȣ�����ǰΪҪ���ܵ����ݳ��ȣ����ܺ�Ϊ�����������ݳ���
	// ����  : ���ܣ��öԷ��Ĺ�Կ���м��ܡ����ڻỰ����ʱʹ�á�����ʱʹ�ûỰ��Կ����DES����
	DLL_EXPORT int SHCAAPI_Encrypt(int apipoint,const char *publickey,unsigned short publickeylen,char *data,unsigned short *datalen);
	
	
	// ������: UnEncrypt
	// ���  : ������ 2013-5-25 11:28:33
	// ����  : int   0�ɹ� 
	// ����  : int apipoint    api��ʵ�����
	// ����  : char *data   ��������->��������
	// ����  : unsigned short  *datalen �������ݳ��ȣ�����ǰΪ���ܵ����ݳ��ȣ����ܺ�Ϊ�������ĳ���
	// ����  : �ñ�����˽Կ���н���
	DLL_EXPORT int SHCAAPI_UnEncrypt(int apipoint,char *data,unsigned short  *datalen);
	
	
	
	// ������: GetCertSerial
	// ���  : ������ 2013-6-14 15:51:14
	// ����  : int   0�ɹ� 
	// ����  : int apipoint    api��ʵ�����
	// ����  : const char *pubkey
	// ����  : unsigned short pubkeylen
	// ����  : char *serial
	// ����  : int *seriallen
	// ����  : ��ȡ֤Ψһ���
	DLL_EXPORT int SHCAAPI_GetCertSerial(int apipoint,const char *pubkey,unsigned short pubkeylen,char *serial,unsigned short *seriallen);
	
	
	// ������: GetErrMsg
	// ���  : ������ 2014-4-10 15:44:35
	// ����  : int  0�ɹ�
	// ����  : int apipoint    api��ʵ�����
	// ����  : ȡ������Ϣ
	DLL_EXPORT int SHCAAPI_GetErrMsg(int apipoint,char *errmsg);
}

#endif // !defined(AFX_SHCAAPI_H__CD835A24_5D17_4EE4_BECA_C6E625B4A11E__INCLUDED_)
