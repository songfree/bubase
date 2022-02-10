// ICertApiBase.h: interface for the CICertApiBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICERTAPIBASE_H__FD3750E3_C97F_4C6F_985E_9892B175B658__INCLUDED_)
#define AFX_ICERTAPIBASE_H__FD3750E3_C97F_4C6F_985E_9892B175B658__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"

class CICertApiBase  
{
public:
	CICertApiBase()
	{

	}
	virtual ~CICertApiBase()
	{

	}
	

	// ������: InitCertApi
	// ���  : ������ 2013-5-24 11:05:29
	// ����  : virtual void 
	// ����  : ��ʼ��֤��api
	virtual bool InitCertApi(const char *certpath,char *certpwd) =0;

	// ������: UnInitCertApi
	// ���  : ������ 2013-5-24 11:05:29
	// ����  : virtual void 
	// ����  : �˳�֤��API
	virtual bool UnInitCertApi() =0;

	// ������: Sign
	// ���  : ������ 2013-5-24 11:07:02
	// ����  : virtual bool 
	// ����  : const char *signstr  Ҫǩ�����ַ���
	// ����  : unsigned short signstrlen Ҫǩ�����ַ�������
	// ����  : char *signres   ǩ�����
	// ����  : unsigned short &signreslen   ǩ���������
	// ����  : ǩ�����ñ�����˽Կǩ��
	virtual bool Sign(const char *signstr,unsigned short signstrlen,char *signres,unsigned short &signreslen)=0;


	// ������: GetPubicKey
	// ���  : ������ 2013-5-25 11:22:50
	// ����  : virtual bool 
	// ����  : char *signkeybuf   ǩ����Կ
	// ����  : unsigned short &signkeybuflen ǩ����Կ����
	// ����  : char *encryptkkeybuf  ���ܹ�Կ
	// ����  : unsigned short &encryptkkeybuflen ���ܹ�Կ����
	// ����  : ȡ����֤��Ĺ�Կ
	virtual bool GetPubicKey(char *signkeybuf,unsigned short &signkeybuflen,char *encryptkeybuf,unsigned short &encryptkkeybuflen)=0;

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
	virtual bool CheckSign(const char *publickey,unsigned short publickeylen,const char *signstr,unsigned short signstrlen,const char *signres,unsigned short signreslen)=0;


	// ������: Encrypt
	// ���  : ������ 2013-5-25 11:27:12
	// ����  : virtual bool 
	// ����  : const char *publickey  ���ܹ�Կ
	// ����  : unsigned short publickeylen ���ܹ�Կ����
	// ����  : char *data   ����->��������
	// ����  : unsigned short &datalen   ���������ȣ�����ǰΪҪ���ܵ����ݳ��ȣ����ܺ�Ϊ�����������ݳ���
	// ����  : ���ܣ��öԷ��Ĺ�Կ���м��ܡ����ڻỰ����ʱʹ�á�����ʱʹ�ûỰ��Կ����DES����
	virtual bool Encrypt(const char *publickey,unsigned short publickeylen,char *data,unsigned short &datalen)=0;


	// ������: UnEncrypt
	// ���  : ������ 2013-5-25 11:28:33
	// ����  : virtual bool 
	// ����  : char *data   ��������->��������
	// ����  : unsigned short  &datalen �������ݳ��ȣ�����ǰΪ���ܵ����ݳ��ȣ����ܺ�Ϊ�������ĳ���
	// ����  : �ñ�����˽Կ���н���
	virtual bool UnEncrypt(char *data,unsigned short  &datalen)=0;



	// ������: GetCertSerial
	// ���  : ������ 2013-6-14 15:51:14
	// ����  : virtual bool 
	// ����  : const char *pubkey
	// ����  : unsigned short pubkeylen
	// ����  : char *serial
	// ����  : int &seriallen
	// ����  : ��ȡ֤Ψһ���
	virtual bool GetCertSerial(const char *pubkey,unsigned short pubkeylen,char *serial,unsigned short &seriallen)=0;


	// ������: GetErrMsg
	// ���  : ������ 2014-4-10 15:44:09
	// ����  : virtual std::string 
	// ����  : ��ȡ��ʼ��ʱ�ĳ�����Ϣ
	virtual std::string GetErrMsg()=0;
};

#endif // !defined(AFX_ICERTAPIBASE_H__FD3750E3_C97F_4C6F_985E_9892B175B658__INCLUDED_)
