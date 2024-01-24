// GlobalVar.h: interface for the CGlobalVar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLOBALVAR_H__C0EB044C_DC92_47A0_B81E_4B1EC2FC1710__INCLUDED_)
#define AFX_GLOBALVAR_H__C0EB044C_DC92_47A0_B81E_4B1EC2FC1710__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "BF_Mutex.h"
#include "DrebStatInfo.h"
#include "BF_Date_Time.h"
#include "BF_Tools.h"
#include "BF_LogClient.h"
#include "BF_CommDataLog.h"

//�������߽ṹ
typedef struct 
{
	std::string sname;  //������������
	std::string sIp;    //���ӵ���������IP
	unsigned int    nPort;  //�˿�
	unsigned int    nSpeed; //���ʣ�·��ѡ��ʱһ������ 
}S_CENTERINFO;

#ifdef __cplusplus
extern "C" {
#endif
	
	//�ֽڶ���
#ifdef _WINDOWS
#define __PACKED__ 
#pragma pack(push,1)
#else
#ifdef __GNUC__
#define __PACKED__	__attribute__ ((packed))
	//#pragma pack(push,1)
#else
#define __PACKED__
#pragma options align=packed
#endif
#endif

typedef struct
{
	long  nSerial_get;       //Ψһ��ˮ
	int   serial;     //����
}__PACKED__ S_SHM_PUBLIC;
//ȡ���ֽڶ���
#ifdef _WINDOWS
#pragma pack(pop)
#else
#ifdef __GNUC__
//#pragma pack(pop)
#else
#pragma options align=reset
#endif
#endif
#undef __PACKED__

#ifdef __cplusplus
}
#endif

typedef std::vector <S_CENTERINFO> S_DREBINFO;



class CGlobalVar  
{
public:
	CGlobalVar();
	virtual ~CGlobalVar();

public:
	
	bool g_bIsExit;            //�Ƿ��˳�
	std::string  g_sConfFileName ; //�����ļ���
	char m_errMsg[256];        //������Ϣ

	char g_curentpath[256];    //��������Ŀ¼
	char g_modulename[256];    //��������
	char g_modulepath[256];    //��������Ŀ¼
	
public:
	int  g_nPort;              //�����˿�
	int  g_nDrebId;            //�����ڵ��
	int  g_nDrebPrivateId;     //˽�нڵ��

	int g_nRegOtherDreb;       //�Ƿ�����������ע��
	int g_nZip;                //�Ƿ�ѹ��
	int g_nMaxConnCount;       //�������������Ҫ����640
	int g_nBeatHeart;          //�Ƿ�������1��0��
	int g_nHeartCount;         //�����������ڷ���beatheart������δ�յ���Ӧ�󣬽������ر����ӣ�ȡֵ1��20
	int g_nBufferLimit;        //�����ϵķ��ͻ������0Ϊ�����ƣ���С��������100
	int g_nMsgProcThread;      //�Ƿ񵥶������߳� �����߳�ͨ����Ϣ���н���  >0Ϊ�����߳���

	int g_nSvrDeleteTime;      //����Ͽ���ɾ���������ʱ��
	int g_nServiceDeleteTime;  //������·�ɹ���ʱ��

	int g_nDisconnectTime;     //��Կͻ���
	
	int g_nServiceRouteTime;   //���ͷ���·�ɹ��ܵ�ʱ����
	int g_nDrebRouteTime;      //DREB�ڵ��·��ʱ��

	int g_nMaxQueue;  //���ͻ�����ʱ�������������ĸ���
//	int g_nModuleThreadNum;    //�����̵߳ĸ���

	int g_nLogLevel;           //��־����
	int g_nDataLogLevel;       //data��־����
	int g_nMaxLogSize;         //�ֽ���<1024������
	int g_nMaxDataLogSize;         //�ֽ���<1024������  

	int g_nCpuCore;//�󶨵�cpu����

	int g_nCrcFlag; //crcУ���־  0��У�鲻����  1У�� ����
	std::string g_sLogFileName;   //��־�ļ���
	std::string g_sLogFilePath;   //��־�ļ�Ŀ¼
	
	S_DREBINFO g_pCenterList;  //���ӵ�ͨѶƽ̨�б�

	int   g_nUseMonitor;       //�Ƿ���
	int   g_nMonitorHost;      //�Ƿ񱨸�������Դ
	int   g_nMonitorDrebId;            //�����ڵ��
	int   g_nMonitorDrebPrivateId;     //˽�нڵ��
	int   g_nMonitorSvrId;            //��������ڵ��
	int   g_nMonitorSvrPrivateId;     //˽�з���ڵ��
	int   g_nMonitorTxCode;           //��ؽ�����
	
	CDrebStatInfo  g_pDrebStatInfo;
	CBF_Date_Time  g_pDate;

	CBF_LogClient      g_pLog;     //��־

	CBF_LogClient      g_pRouteLog;     //·����־

	int  g_nPrintRoute;//�Ƿ��¼·����Ϣ����־

	int  g_nSupportSubscribe;//�Ƿ�֧�ֹ㲥���ģ�ֻ�е�û��ָ������ʱ���ſ��˲���������֧�ֶ���ʱ���㲥û��ָ�������򷢸����з��񣬷��򷢸������˵ķ���

	CBF_CommDataLog    g_pDataLog; //�����Ʊ�����־

	int  g_nMaxRouteStep;//���·�ɲ���
	
	int  m_bIsRecTime;
public:
	void LogTime();
	/// ������: Init
	/// ���  : ������ 2009-5-16 16:03:39
	/// ����  : bool 
	/// ����  : ���ó�ʼ��
	bool Init(const char *confile);

	int getTick();

	S_SHM_PUBLIC *m_pShmpub;//�����ڴ��е�����
	
	void PutTime();

	std::vector<S_MONITOR_INFO> m_vBcDest;//�����ڵ�Ĺ㲥ת����ָ�����߽ڵ��

protected:
	
	std::vector<INT64_> m_vTime;
	
	CBF_Mutex m_mutex;
};

#endif // !defined(AFX_GLOBALVAR_H__C0EB044C_DC92_47A0_B81E_4B1EC2FC1710__INCLUDED_)
