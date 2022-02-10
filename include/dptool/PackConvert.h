/// PackConvert.h: interface for the CPackConvert class.
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PACKCONVERT_H__51B7C0FB_36B7_4755_ACDB_00086C6FB6C6__INCLUDED_)
#define AFX_PACKCONVERT_H__51B7C0FB_36B7_4755_ACDB_00086C6FB6C6__INCLUDED_

#include "XmlPack.h"
//#include "pubstrtime.h"
#include "InterfacePack.h"
#include "BF_Tools.h"
#include "BF_SimpleLog.h"
#include "BF_Slist.h"

#if _MSC_VER > 1000
#pragma once
#endif /// _MSC_VER > 1000

#ifdef DPTOOL_EXPORTS
#define DPTOOL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define DPTOOL_EXPORT __declspec(dllimport)
#else
#define DPTOOL_EXPORT 
#endif
#endif

class DPTOOL_EXPORT CPackConvert  
{
public:

	///��ʼ����xml�����ļ�
	int Init(char *file);
	CPackConvert();
	CPackConvert(char *file);
	virtual ~CPackConvert();
	char m_errMsg[256];
	char m_logfile[41];///��־�ļ���
	CInterfacePack m_interfaceConf;///�ӿڶ�����
	bool m_isInit;///�Ƿ��ʼ��

	char m_buf[1300]; ///buf����
	int m_buflen;///buf����
	int m_resConv;///Ӧ����İ�����
protected:

	CBF_SimpleLog m_publog;
	
public:


	/// ������: Xml2AnsExt
	/// ���  : ������ 2006-8-14 9:53:37
	/// ����  : bool 
	/// ����  : int txcode
	/// ����  : CXmlPack *xmlpack
	/// ����  : void *buf
	/// ����  : int &buflen
	/// ����  : ���ݷ������ֵѡ��xmlpack���е�Ӧ������(����İ�ͷ)
	bool Xml2AnsExt(int txcode, CXmlPack *xmlpack, char *buf, int &buflen);

	/// ������: Xml2AnsExt
	/// ���  : ������ 2006-11-14 9:53:37
	/// ����  : bool 
	/// ����  : int txcode
	/// ����  : CXmlPack *xmlpack
	/// ����  : void *buf
	/// ����  : int &buflen
	/// ����  : int type
	/// ����  : ����type��ֵѡ��xmlpack���е�Ӧ������(����İ�ͷ)
	bool Xml2AnsExt(int txcode, CXmlPack *xmlpack, char *buf, int &buflen, int type);
	
		/// ������: Xml2Ans
	/// ���  : ������ 2006-6-8 16:23:08
	/// ����  : bool 
	/// ����  : int txcode
	/// ����  : CXmlPack *xmlpack
	/// ����  : void *buf
	/// ����  : int &buflen
	/// ����  : ���ݷ�����ת���ڲ���xmlpackӦ����ⲿ
	bool Xml2Ans(int txcode,CXmlPack *xmlpack,void *buf,int &buflen);

	/// ������: Xml2Ans
	/// ���  : ������ 2006-11-14 16:23:08
	/// ����  : bool 
	/// ����  : int txcode
	/// ����  : CXmlPack *xmlpack
	/// ����  : void *buf
	/// ����  : int &buflen
	/// ����  : int type
	/// ����  : ����type��ֵѡ��xmlpack���е�Ӧ������Ӧ����ⲿ
	bool Xml2Ans(int txcode,CXmlPack *xmlpack,void *buf,int &buflen, int type);

	/// ������: Xml2Req
	/// ���  : ������ 2006-6-8 16:20:08
	/// ����  : bool 
	/// ����  : int txcode
	/// ����  : CXmlPack *xmlpack �ڲ�����xml
	/// ����  : void *buf ���ݻ���
	/// ����  : int &buflen  ���ݻ��峤��
	/// ����  : ת���ڲ�xmlpack��ʽΪ�ⲿ����
	bool Xml2Req(int txcode,CXmlPack *xmlpack,void *buf,int &buflen);

	/// ������: Conv2XmlAns
	/// ���  : ������ 2006-6-8 16:16:05
	/// ����  : bool 
	/// ����  : int txcode ������
	/// ����  : void *buf ���ݻ���
	/// ����  : int buflen ���ݻ��峤��
	/// ����  : CXmlPack *xmlpack ���xml
	/// ����  : ת���ⲿ��Ӧ��Ϊ�ڲ�xmlpack��ʽ
	bool Conv2XmlAns(int txcode,void *buf,int buflen,CXmlPack *xmlpack);

	/// ������: Conv2XmlReq
	/// ���  : ������ 2006-6-8 16:12:38
	/// ����  : bool 
	/// ����  : int txcode ������
	/// ����  : void *buf   ���ݻ���
	/// ����  : int buflen  ���ݻ��峤��
	/// ����  : CXmlPack *xmlpack ���xml
	/// ����  : ת���ⲿ����Ϊ�ڲ�xmlpack��ʽ
	bool Conv2XmlReq(int &txcode,void *buf,int buflen,CXmlPack *xmlpack);

	/// ������: getResAttr
	/// ���  : ������ 2006-8-7 19:20:33
	/// ����  : bool 
	/// ����  : int txcode  �����룬����
	/// ����  : int retcode  �����룬����˽���ֻ��һ�����ذ�����ɲ���
	/// ����  : int &fileattr  0ͨѶ��  1д���ļ�
	/// ����  : ���ݽ����뼰�������ȡ���ص��������� 
	bool getResAttr(int txcode,int retcode,int &fileattr);

private:
	/// ������: conv2BufS
	/// ���  : ������ 2006-8-14 9:32:22
	/// ����  : bool 
	/// ����  : int txcode
	/// ����  : S_PACKHEAD_INFO *headinfo
	/// ����  : CXmlPack *xmlpack
	/// ����  : char *buf
	/// ����  : int &buflen
	/// ����  : ����ͷ��������ת������������
	bool conv2BufS(int txcode,S_PACKHEAD_INFO *headinfo,CXmlPack *xmlpack,char *buf,int &buflen);

	

	/// ������: setValueToBufL
	/// ���  : ������ 2006-8-3 17:17:38
	/// ����  : bool 
	/// ����  : CXmlPack *xmlpack
	/// ����  : S_FUNC_FIELD_INFO *fieldconf
	/// ����  : Cpubstrtime *publog
	/// ����  : �������������ֶ�ֵ���зָ�������
	bool setValueToBufL(CXmlPack *xmlpack, S_FUNC_FIELD_INFO *fieldconf, CBF_Slist *publog);


	/// ������: setValueToBuf
	/// ���  : ������ 2006-8-3 17:16:53
	/// ����  : bool 
	/// ����  : CXmlPack *xmlpack
	/// ����  : S_FUNC_FIELD_INFO *fieldconf
	/// ����  : char *buf
	/// ����  : �������������ֶ�ֵ�������޷ָ���BUF��
	bool setValueToBufS(CXmlPack *xmlpack,S_FUNC_FIELD_INFO *fieldconf,char *buf);

	/// ������: getValueFromBuf2
	/// ���  : ������ 2006-8-3 16:59:09
	/// ����  : bool 
	/// ����  : Cpubstrtime *publog
	/// ����  : S_FUNC_FIELD_INFO *fieldconf
	/// ����  : string value
	/// ����  : ����ת�����ͼ�������Ϣ�ӷָ������ݻ����ȡ
	bool getValueFromBufL(CBF_Slist *publog,S_FUNC_FIELD_INFO *fieldconf,std::string &value);

	/// ������: getValueFromBuf1
	/// ���  : ������ 2006-8-3 16:59:06
	/// ����  : bool 
	/// ����  : char *buf
	/// ����  : int buflen
	/// ����  : S_FUNC_FIELD_INFO *fieldconf
	/// ����  : string value
	/// ����  : ����ת�����ͼ�������Ϣ�Ӷ������ݻ����ȡ ֵ
	bool getValueFromBufS(char *buf,int buflen,S_FUNC_FIELD_INFO *fieldconf,std::string &value);

	/// ������: getTransValue
	/// ���  : ������ 2006-8-3 13:32:52
	/// ����  : string 
	/// ����  : string transname
	/// ����  : char *buf 
	/// ����  : int flag  0���� 1�ָ�
	/// ����  : int fieldtype 0-char 1-integer 2-double
	/// ����  : �����ݻ����и��ݶ���ȡ��ֵ,�Ӽ��˳�
	std::string getTransValue(std::string transname,char *buf,int flag,int fieldtype);

	/// ������: getTransValue
	/// ���  : ������ 2006-6-12 17:38:53
	/// ����  : string 
	/// ����  : string transname
	/// ����  : CXmlPack *xmlpack
	/// ����  : int fieldtype 0-char 1-integer 2-double
	/// ����  : ��xml�еĽڶ��и��ݶ���ȡ��ֵ,�Ӽ��˳�
	std::string getTransValue(std::string transname,CXmlPack *xmlpack,int fieldtype);

	bool IsDouble(char *str);


};
#undef DPTOOL_EXPORT
#endif /// !defined(AFX_PACKCONVERT_H__51B7C0FB_36B7_4755_ACDB_00086C6FB6C6__INCLUDED_)
