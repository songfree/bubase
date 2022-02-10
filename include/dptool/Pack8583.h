// Pack8583.h: interface for the CPack8583 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PACK8583_H__E88F88EC_1F39_4740_84AA_A258292576C3__INCLUDED_)
#define AFX_PACK8583_H__E88F88EC_1F39_4740_84AA_A258292576C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "XmlPack.h"

#ifdef PACK8583_EXPORTS
    #define CONVERTLIB_API __declspec(dllexport)
#else
    #ifdef WIN_DLL
        #define CONVERTLIB_API __declspec(dllimport)
    #else
        #define CONVERTLIB_API 
    #endif
#endif

typedef struct _struct8583
{
	int idx;
	string value;
};
typedef struct _struct8583def
{
	int idx;
	CNode *pfield;
};
typedef std::list<_struct8583> L_PACK8583;
typedef std::vector<_struct8583def> V_pack8583def;

class  CONVERTLIB_API CPack8583  
{
public:
	CPack8583();
	CPack8583(char *xmldefine);
	virtual ~CPack8583();

	CXmlPack m_xml8583;//8583�������ļ�
	
	char m_packHead[512];//8583λͼ,�512λ
	char m_packBuffer[8193];
	int  m_bitLen;//λͼ����
	int  m_fieldNum;//�ֶθ���
	bool m_bIsLoad;//�Ƿ�װ��8583���óɹ�
	char m_errMsg[512];//������Ϣ

private:
	L_PACK8583 m_vpack8583;//8583������

	V_pack8583def m_vpack8583def;//8583������	
public:
	bool LoadPackDef(char *xmlfile);

	// ������: GetPackBuffer
	// ���  : ������ 2007-9-8 18:04:08
	// ����  : bool 
	// ����  : char *buffer  8583������
	// ����  : int &buflen   8583�����ݳ���
	// ����  : ��ȡ8583��������
	bool GetPackBuffer(char *buffer,int &buflen);

	// ������: SetFieldValue
	// ���  : ������ 2007-9-8 18:01:36
	// ����  : bool 
	// ����  : const char *name  λͼ����
	// ����  : const double value ֵ
	// ����  : �����ֶ�ֵ
	bool SetFieldValue(const char *name,const double value);

	// ������: SetFieldValue
	// ���  : ������ 2007-9-8 18:01:32
	// ����  : bool 
	// ����  : const char *name λͼ����
	// ����  : const int value ֵ
	// ����  : �����ֶ�ֵ
	bool SetFieldValue(const char *name,const int value);


	// ������: SetFieldValue
	// ���  : ������ 2007-9-8 18:01:28
	// ����  : bool 
	// ����  : const char *name λͼ����
	// ����  : const string value ֵ
	// ����  : �����ֶ�ֵ
	bool SetFieldValue(const char *name,const string value);


	// ������: SetFieldValue
	// ���  : ������ 2007-9-8 18:01:25
	// ����  : bool 
	// ����  : const char *name λͼ����
	// ����  : const char *value ֵ
	// ����  : �����ֶ�ֵ
	bool SetFieldValue(const char *name,const char *value);


	// ������: SetFieldValue
	// ���  : ������ 2007-9-8 18:01:21
	// ����  : bool 
	// ����  : const int idx
	// ����  : const double value
	// ����  : �����ֶ�ֵ
	bool SetFieldValue(const int idx,const double value);

	// ������: SetFieldValue
	// ���  : ������ 2007-9-8 18:00:22
	// ����  : bool 
	// ����  : const int idx
	// ����  : const int value
	// ����  : �����ֶ�ֵ
	bool SetFieldValue(const int idx,const int value);

	// ������: SetFieldValue
	// ���  : ������ 2007-9-8 17:51:06
	// ����  : bool 
	// ����  : const int idx λ�����
	// ����  : const string value ֵ
	// ����  : �����ֶ�ֵ
	bool SetFieldValue(const int idx,const string value);

	// ������: SetFieldValue
	// ���  : ������ 2007-9-8 17:49:38
	// ����  : bool 
	// ����  : const int idx  λ�����
	// ����  : const char *value  ֵ
	// ����  : �����ֶ�ֵ 
	bool SetFieldValue(const int idx,const char *value);
	void New8583();

};
#undef DPTOOL_EXPORT
#endif // !defined(AFX_PACK8583_H__E88F88EC_1F39_4740_84AA_A258292576C3__INCLUDED_)
