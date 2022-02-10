/// IClientApi.h: interface for the CIClientApi class.
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICLIENTAPI_H__A9EB780E_A8D1_4491_8911_1D6845DBEBAB__INCLUDED_)
#define AFX_ICLIENTAPI_H__A9EB780E_A8D1_4491_8911_1D6845DBEBAB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif /// _MSC_VER > 1000

#include <stdio.h>
#include <string.h>
#include <string>

class CXmlPack;
class CXmlPackArray;

class  CIClientApi  
{
public:
	char m_errMsg[256];///������Ϣ

	CIClientApi()
	{

	}
	virtual ~CIClientApi()
	{

	}
	virtual void Close()=0;
	/// ������: CallService
	/// ���  : ������ 2008-7-3 9:22:38
	/// ����  : int <0ʧ�� =0�ɹ�
	/// ����  : const char *bizinfobuf
	/// ����  : string &bizinfoansbuf  Ӧ�����ݻ���
	/// ����  : ����bizinfo��ʽ�����ݣ���bizinfoת��xml��������̨
	virtual int CallService(const char *bizinfobuf,std::string &bizinfoansbuf)=0;


	/// ������: SetTuxedoPara
	/// ���  : ������ 2008-7-3 9:18:42
	/// ����  : int <0ʧ�� =0�ɹ�
	/// ����  : bool shortconnect=true
	/// ����  : int tuxedobuflen=8192
	/// ����  : bool tuxedoalias=false
	/// ����  : ����tuxedo�����Ӳ��������ݻ��峤�ȣ���������ñ����������õĻ���Ĭ��Ϊ�����ӣ�����Ϊ8192
	virtual int SetTuxedoPara(bool shortconnect=true,int tuxedobuflen=8192, bool tuxedoalias=false)=0;



	/// ������: GetFile
	/// ���  : ������ 2008-6-27 14:57:47
	/// ����  : int <0ʧ�� =0�ɹ�
	/// ����  : const char *srcfilenamepath  �����ϵ��ļ�
	/// ����  : const char * destfilenamepath ���ر�����ļ�
	/// ����  : char *szmsg
	/// ����  : ȡ�ļ�
	virtual int GetFile(const char *srcfilenamepath,const char * destfilenamepath,char *szmsg)=0;


	/// ������: SetTuxedoPara
	/// ���  : ������ 2008-6-27 14:58:18
	/// ����  : int int <0ʧ�� =0�ɹ�
	/// ����  : const char *wsnaddr  tuxedo��wsnaddr ///192.168.22.216L17090
	/// ����  : const char *serviceName  tuxedo������
	/// ����  : int zip �Ƿ������ѹ��  0�������벻ѹ�� 1���� 2ѹ��
	/// ����  : int pwdflag �����Ƿ���� (��Ϊ1��ʾxml���������Ѽ��ܣ���api���ټ���)
	/// ����  : char *logfile ��־�ļ���
	/// ����  : int loglevel ��־����
	/// ����  : bool istransfile = true �������Ƿ�ͨ���������ļ�
	/// ����  : ����tuxedo��ͨѶ����
	virtual int SetTuxedoPara(const char *wsnaddr,const char *serviceName,int zip ,int pwdflag,char *logfile,int loglevel,bool istransfile = true)=0;

	


	/// ������: SetCommPara
	/// ���  : ������ 2008-6-27 14:58:34
	/// ����  : int int <0ʧ�� =0�ɹ�
	/// ����  : const char *ip ip��ַ
	/// ����  : int port  �����˿�
	/// ����  : int zip  �Ƿ������ѹ��  0�������벻ѹ�� 1���� 2ѹ��
	/// ����  : int pwdflag  �����Ƿ���� (��Ϊ1��ʾxml���������Ѽ��ܣ���api���ټ���)
	/// ����  : int timeout   ��ʱʱ��
	/// ����  : int headlen   ͨѶ����ͷ����
	/// ����  : int inchead   �Ƿ����ͨѶ����ͷ
	/// ����  : const char *logfile  ��־�ļ���
	/// ����  : int loglevel  ��־����
	/// ����  : bool istransfile = true �������Ƿ�ͨ���������ļ�
	/// ����  : ����socketͨѶ�Ĳ���
	virtual int SetCommPara(const char *ip, int port, int zip, int pwdflag, int timeout, int headlen, int inchead,const char *logfile,int loglevel,bool istransfile = true,bool shortconnect=false)=0;

	
	/// ������: SendFile
	/// ���  : ������ 2008-5-29 8:37:16
	/// ����  : int int <0ʧ�� =0�ɹ�
	/// ����  : const char *srcfilenamepath Ҫ���͵��ļ���(����path)
	/// ����  : const char * destfilenamepath ��̨���ļ���(����path)
	/// ����  : char *szmsg
	/// ����  : �����ļ�����̨
	virtual int SendFile(const char *srcfilenamepath,const char * destfilenamepath,char *szmsg)=0;

	/// ������: CallServiceFile
	/// ���  : ������ 2007-12-6 19:15:11
	/// ����  : int int <0ʧ�� =0�ɹ�
	/// ����  : const char *reqfilename
	/// ����  : char *ansfilename
	/// ����  : ���ú󷵻ض�������ļ���
	virtual int CallServiceFile(CXmlPack &apack,const char *reqfilename,char *ansfilename)=0;

	/// ������: CallService
	/// ���  : ������ 2007-10-15 14:29:06
	/// ����  : int <0ʧ��
	/// ����  : CXmlPack &xmlpack ���󡢵���Ӧ��
	/// ����  : CXmlPackArray &muxmlpack ���Ӧ��
	/// ����  : ���ú�̨����xmlpackΪ����xmpack��muxmlpackΪӦ��
	virtual int CallService(CXmlPack &xmlpack, CXmlPackArray &muxmlpack)=0;

	/// ������: Init
	/// ���  : ������ 2007-10-15 14:28:26
	/// ����  : bool 
	/// ����  : char *file �����ļ��� ��linux�´�·��
	/// ����  : ��ʼ��,�������ļ�������
	virtual bool Init(char *file)=0;

	virtual int Xml2Biz(CXmlPack *xmlpack,std::string &bizbuf)=0;

	virtual int Biz2Xml(const char *bizinfobuf ,CXmlPack &xmlpack)=0;



};

#endif /// !defined(AFX_ICLIENTAPI_H__A9EB780E_A8D1_4491_8911_1D6845DBEBAB__INCLUDED_)
