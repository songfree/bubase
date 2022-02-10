// GResource.h: interface for the CGResource class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRESOURCE_H__A7724768_0CB1_4D54_B228_60D1463D2AED__INCLUDED_)
#define AFX_GRESOURCE_H__A7724768_0CB1_4D54_B228_60D1463D2AED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "BF_Mutex.h"
#include "XmlPack.h"
#include "BF_SimpleLog.h"
#include "BF_Date_Time.h"
#include "IGResource.h"
#include "IPcLink.h"
#include "BF_Tools.h"



#ifdef _WINDOWS
#define INT64  __int64 
#else
#define INT64  long long 
#endif


class CBuRes  : public CIGResource 
{
public:
	
	CBuRes();
	virtual ~CBuRes();

	

public:
	bool g_bToExit;//�Ƿ��˳�����
	int g_nPort;//�����˿� Ĭ��Ϊ9001
	int g_nLoglevel;//��־���� Ĭ��Ϊ5
	std::string g_sLogFileName;//��־�ļ���
	std::string g_sLogFilePath;//��־ֹ¼
	std::string g_sBccIp;//bcc��IP��ַ
	
	int g_IsZipEncrypt;//������ѹ����־
	int g_nXmlNodeMode;//xml�ڵ����� 0֧������ 1��֧��<aa /> 

	bool g_bIsTxMainFunc;//����ͳһ���
	std::string g_sMainFunc;//����ͳһ��ڹ��ܺ�

	int g_nDisconnectTime;//���ӶϿ�ʱ�䣬���ڴ�ʱ����û���յ�����


	std::string m_errMsg;
	CBF_SimpleLog m_log;
	CBF_Tools m_publog;


public:
	CIPcLink *m_pLink;
	void WriteBack();

	

	/// ������: OpenDll
	/// ���  : ������ 2008-9-4 9:15:14
	/// ����  : bool 
	/// ����  : DLLHANDLE *pDll ��̬���ַ
	/// ����  : const char *dllname ��̬������
	/// ����  : const char *dllpath ��̬��·��
	/// ����  : �򿪶�̬�⣬������̬���ַ
	bool OpenDll(DLLHANDLE *pDll,const char *dllname,const char *dllpath);

	/// ������: GetMsg
	/// ���  : ������ 2008-9-2 14:29:00
	/// ����  : string 
	/// ����  : ȡ�ó�����Ϣ
	std::string GetMsg();

	/// ������: Init
	/// ���  : ������ 2008-9-2 11:55:26
	/// ����  : bool 
	/// ����  : char *confile
	/// ����  : ȫ�ֲ�����ʼ��
	virtual bool Init(char *confile);


	/// ������: GetSerial
	/// ���  : ������ 2008-9-2 11:47:48
	/// ����  : unsigned long 
	/// ����  : ȡ��Ψһ�������
	unsigned long GetSerial();

	/// ������: SetGPara1
	/// ���  : ������ 2008-12-30 9:42:48
	/// ����  : virtual bool 
	/// ����  : bool setflag=true
	/// ����  : ����ȫ�ֱ���1,���������򷵻�false
	virtual bool SetGPara1(bool setflag=true);
	
	
	/// ������: isSetGPara2
	/// ���  : ������ 2008-12-30 9:36:10
	/// ����  : virtual bool 
	/// ����  : bool setflag=true
	/// ����  : ����ȫ�ֱ���2,���������򷵻�false
	virtual bool SetGPara2(bool setflag=true);
	
	
	/// ������: isSetGPara3
	/// ���  : ������ 2008-12-30 9:36:13
	/// ����  : virtual bool 
	/// ����  : bool setflag=true
	/// ����  : ����ȫ�ֱ���3,���������򷵻�false
	virtual bool SetGPara3(bool setflag=true);

	/// ������: GetThreadShareSerial
	/// ���  : ������ 2008-12-30 9:36:13
	/// ����  : virtual long 
	/// ����  : ȡ��Ψһ����ˮ��
	virtual long GetThreadShareSerial();

protected:
	
};

#endif // !defined(AFX_GRESOURCE_H__A7724768_0CB1_4D54_B228_60D1463D2AED__INCLUDED_)
