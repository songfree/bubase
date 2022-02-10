/// ClientApi.h: interface for the CClientApi class.
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLIENTAPI_H__C8ADEC7B_210F_41AE_9191_D0D8EE3277ED__INCLUDED_)
#define AFX_CLIENTAPI_H__C8ADEC7B_210F_41AE_9191_D0D8EE3277ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif /// _MSC_VER > 1000

#include "IClientApi.h"
#include "XmlPack.h"
#include "XmlPackArray.h"
#include "BF_SimpleLog.h"
#include "BF_SocketTcp.h"
#include "BF_Date_Time.h"
#include "BF_Tools.h"


#ifdef TUXEDO
#include "atmi.h"
#endif


#ifdef WIN32
#ifdef _DEBUG
	#pragma comment(lib, "bf_oldxmld.lib") 
	#pragma comment(lib, "bf_kerneld.lib") 
	#pragma message("Automatically linking with  bf_kerneld.lib bf_oldxmld.lib")
#else
	#pragma comment(lib, "bf_oldxml.lib") 
	#pragma comment(lib, "bf_kernel.lib") 
	#pragma message("Automatically linking with  bf_kernel.lib bf_oldxml.lib")
#endif
#endif
///�˰汾����tuxedo���֣�ֻҪ��link�����  wtuxws32.lib�����ڱ���ѡ�������TUXEDOȻ�����±������֧��TUXEDO�İ汾

typedef struct _GLOBAL_VARS_API
{
	char g_sIp[50];///ǰ��IP
	int  g_nPort;///�����˿�
	char g_logFileName[201];///��־�ļ���
	int  g_timeOut;///socket���ճ�ʱʱ��
	int  g_trace;///��־���ټ���
	int  g_tpCallTime;///TUXEDO�����ʱʱ��
	unsigned int  g_rcvHead;///ͨѶ��ͷ����
	bool  g_bIsInclHead;///ͨѶ��ͷ�Ƿ��������
	bool  g_bIsTuxedo;///�Ƿ�ʹ��tuxedo
	int   g_bIsZipEncrypt;///�Ƿ����ѹ��1����2ѹ��0��
	bool  g_bIsPwdEncrypt;///�����Ǽ�����set
	bool  g_bIsFileTrans;///�Ƿ�ʹ���ļ�����
}G_VARS_API;
#ifdef API_EXPORTS
#define API_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define API_EXPORT __declspec(dllimport)
#else
#define API_EXPORT 
#endif
#endif
class API_EXPORT CClientApi  : public CIClientApi
{
public:
	CClientApi();
	virtual ~CClientApi();

	CXmlPack m_xmlconf;///�����ļ�����
	CBF_SimpleLog m_log;///д��־ 
	CBF_SocketTcp m_tcpSocket;///tcp
//	Cpubstrtime m_publog;
	CBF_Date_Time m_publog;
	CBF_Tools     m_pTool;

	
	
	bool m_bIsInit;///�Ƿ��ʼ��
	
	G_VARS_API g_vars;
	
	bool m_isServiceName;
public:
	virtual void Close();

	/// ������: TcpInit
	/// ���  : ������ 2008-10-10 15:58:18
	/// ����  : bool 
	/// ����  : socket���ӳ�ʼ��
	bool TcpInit();
	

	/// ������: CallService
	/// ���  : ������ 2008-7-3 9:22:38
	/// ����  : int <0ʧ�� =0�ɹ�
	/// ����  : const char *bizinfobuf
	/// ����  : string &bizinfoansbuf  Ӧ�����ݻ���
	/// ����  : ����bizinfo��ʽ�����ݣ���bizinfoת��xml��������̨
	virtual int CallService(const char *bizinfobuf,std::string &bizinfoansbuf);

	/// ������: SetTuxedoPara
	/// ���  : ������ 2008-7-3 9:18:42
	/// ����  : int <0ʧ�� =0�ɹ�
	/// ����  : bool shortconnect=true
	/// ����  : int tuxedobuflen=8192
	/// ����  : bool tuxedoalias=false tuxedo������ͨ��������+��������Ϊ����
	/// ����  : ����tuxedo�����Ӳ��������ݻ��峤�ȣ���������ñ����������õĻ���Ĭ��Ϊ�����ӣ�����Ϊ8192
	virtual int SetTuxedoPara(bool shortconnect=true,int tuxedobuflen=8192, bool tuxedoalias=false);


	/// ������: GetFile
	/// ���  : ������ 2008-6-27 14:57:47
	/// ����  : int <0ʧ�� =0�ɹ�
	/// ����  : const char *srcfilenamepath  �����ϵ��ļ�
	/// ����  : const char * destfilenamepath ���ر�����ļ�
	/// ����  : char *szmsg
	/// ����  : ȡ�ļ�
	virtual int GetFile(const char *srcfilenamepath,const char * destfilenamepath,char *szmsg);

	/// ������: SetTuxedoPara
	/// ���  : ������ 2008-6-27 14:58:18
	/// ����  : int int <0ʧ�� =0�ɹ�
	/// ����  : const char *wsnaddr  tuxedo��wsnaddr ///192.168.22.216L17090
	/// ����  : const char *serviceName  tuxedo������
	/// ����  : int zip �Ƿ������ѹ��  0�������벻ѹ�� 1���� 2ѹ��3DES����4DES���ܲ�ѹ��5zlibѹ��
	/// ����  : int pwdflag �����Ƿ���� (��Ϊ1��ʾxml���������Ѽ��ܣ���api���ټ���)
	/// ����  : char *logfile ��־�ļ���
	/// ����  : int loglevel ��־����
	/// ����  : bool istransfile = true �������Ƿ�ͨ���������ļ�
	/// ����  : ����tuxedo��ͨѶ����
	virtual int SetTuxedoPara(const char *wsnaddr,const char *serviceName,int zip ,int pwdflag,char *logfile,int loglevel,bool istransfile = true);
	


	/// ������: SetCommPara
	/// ���  : ������ 2008-6-27 14:58:34
	/// ����  : int int <0ʧ�� =0�ɹ�
	/// ����  : const char *ip ip��ַ
	/// ����  : int port  �����˿�
	/// ����  : int zip  �Ƿ������ѹ��  0�������벻ѹ�� 1���� 2ѹ��3DES����4DES���ܲ�ѹ��5zlibѹ��
	/// ����  : int pwdflag  �����Ƿ���� (��Ϊ1��ʾxml���������Ѽ��ܣ���api���ټ���)
	/// ����  : int timeout   ��ʱʱ��
	/// ����  : int headlen   ͨѶ����ͷ����
	/// ����  : int inchead   �Ƿ����ͨѶ����ͷ
	/// ����  : const char *logfile  ��־�ļ���
	/// ����  : int loglevel  ��־����
	/// ����  : bool istransfile = true �������Ƿ�ͨ���������ļ�
	/// ����  : bool shortconnect=false ��������
	/// ����  : ����socketͨѶ�Ĳ���
	virtual int SetCommPara(const char *ip, int port, int zip, int pwdflag, int timeout, int headlen, int inchead,const char *logfile,int loglevel,bool istransfile = true,bool shortconnect=false);
	
	/// ������: SendFile
	/// ���  : ������ 2008-5-29 8:37:16
	/// ����  : int int <0ʧ�� =0�ɹ�
	/// ����  : const char *srcfilenamepath Ҫ���͵��ļ���(����path)
	/// ����  : const char * destfilenamepath ��̨���ļ���(����path)
	/// ����  : char *szmsg
	/// ����  : �����ļ�����̨
	virtual int SendFile(const char *srcfilenamepath,const char * destfilenamepath,char *szmsg);
	

	/// ������: CallServiceFile
	/// ���  : ������ 2007-12-6 19:15:11
	/// ����  : int int <0ʧ�� =0�ɹ�
	/// ����  : const char *reqfilename
	/// ����  : char *ansfilename
	/// ����  : ���ú󷵻ض�������ļ���
	virtual int CallServiceFile(CXmlPack &apack,const char *reqfilename,char *ansfilename);

	

	/// ������: CallService
	/// ���  : ������ 2007-10-15 14:29:06
	/// ����  : int <0ʧ��
	/// ����  : CXmlPack &xmlpack ���󡢵���Ӧ��
	/// ����  : CXmlPackArray &muxmlpack ���Ӧ��
	/// ����  : ���ú�̨����xmlpackΪ����xmpack��muxmlpackΪӦ��
	virtual int CallService(CXmlPack &xmlpack, CXmlPackArray &muxmlpack);

	

	/// ������: Init
	/// ���  : ������ 2007-10-15 14:28:26
	/// ����  : bool 
	/// ����  : char *file �����ļ��� ��linux�´�·��
	/// ����  : ��ʼ��,�������ļ�������
	virtual bool Init(char *file);

	virtual int Xml2Biz(CXmlPack *xmlpack,std::string &bizbuf);
	virtual int Biz2Xml(const char *bizinfobuf ,CXmlPack &xmlpack);
protected:


#ifdef TUXEDO
	TPINIT * g_tpinitbuf;
#endif
	char  m_wsnaddr[80];
	bool  m_bIsShortConnect;///�Ƿ������
	int   m_TuxedoBufLen;///���峤��
	
	char m_service[60];///tuxedo������
	bool m_bIsTpInit ;///tuxedo�Ƿ��ʼ��
	bool m_bIsTcpInit;//socket�Ƿ�����


	int m_txCode;///������
	bool m_tuxedoAlias;///�Ƿ����tuxedo����


	/// ������: GetFileName
	/// ���  : ������ 2007-12-3 10:27:16
	/// ����  : int <0ʧ�� =0�ɹ�
	/// ����  : char *filename
	/// ����  : ���ɶ�������ļ���
	int GetFileName(char *filename);

	/// ������: WriteToFile
	/// ���  : ������ 2007-12-3 10:27:00
	/// ����  : int <0ʧ�� =0�ɹ� 
	/// ����  : FILE *fp
	/// ����  : char *buf
	/// ����  : int buflen
	/// ����  : ��Ӧ��xml����д���ļ�
	int WriteToFile(FILE *fp,char *buf,int buflen);

	/// ������: TuxedoInit
	/// ���  : ������ 2007-12-3 10:26:50
	/// ����  : bool 
	/// ����  : bool isinit
	/// ����  : tuxedo��ʼ��
	bool TuxedoInit(bool isinit);

	/// ������: TcpReceive
	/// ���  : ������ 2007-10-15 14:29:44
	/// ����  : int <0ʧ�� =0�ɹ�
	/// ����  : char *buf
	/// ����  : int &buflen
	/// ����  : socket����
	int TcpReceive(char *buf,int &buflen);
	
	/// ������: TcpSend
	/// ���  : ������ 2007-10-15 14:29:40
	/// ����  : int <0ʧ�� =0�ɹ�
	/// ����  : char *buf
	/// ����  : int buflen
	/// ����  : socket����
	int TcpSend(char *buf,int buflen);
	
	/// ������: GetService
	/// ���  : ������ 2007-10-15 14:29:36
	/// ����  : bool 
	/// ����  : int txcode
	/// ����  : ���ݽ�����ȡtuxedo�ķ�����
	bool GetService(int txcode);

	/// ������: Send2Server
	/// ���  : ������ 2007-10-15 14:28:53
	/// ����  : int <0ʧ�� =0�ɹ�
	/// ����  : char *sendbuf
	/// ����  : int buflen
	/// ����  : char *rcvbuf
	/// ����  : int &rcvlen
	/// ����  : ���͵���̨
	int Send2Server(char *sendbuf,int buflen,char *rcvbuf,int &rcvlen);

	/// ������: GetMuPack
	/// ���  : ������ 2008-6-27 17:16:42
	/// ����  : int <0ʧ�� =0�ɹ�
	/// ����  : const char *filename
	/// ����  : CXmlPackArray &muxmlpack
	/// ����  : int mupackans �Ƿ��packӦ�� 1��������
	/// ����  : bool first= false ��һ���Ƿ�ŵ�muxmlpack��
	/// ����  : ������ļ��е����ݶ�ȡ��muxmlpack��
	int GetMuPack(const char *filename,CXmlPackArray &muxmlpack,int mupackans,bool first= false);

	/// ������: GetFileSerial
	/// ���  : ������ 2008-6-27 17:17:18
	/// ����  : int 
	/// ����  : char *serial
	/// ����  : ȡΨһ�ļ���
	int GetFileSerial(char *serial);
};
extern "C" 
{
	API_EXPORT CClientApi * getInstanceApi();
	
}
#undef  API_EXPORT
#endif /// !defined(AFX_CLIENTAPI_H__C8ADEC7B_210F_41AE_9191_D0D8EE3277ED__INCLUDED_)
