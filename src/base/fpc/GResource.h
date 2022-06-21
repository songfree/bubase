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
#include "BF_LogClient.h"
#include "BF_Date_Time.h"
#include "IGResource.h"
#include "BF_Tools.h"

//�����ļ�����
/*
<?xml version="1.0" encoding="GB2312"?>
<package>
	<head>
		<��������>
			<��־���� name="��־���ټ���0���ش���1��Ҫ����2һ�����3����4��ʾ����5������Ϣ">5</��־����>
			<��־�ļ� name="��¼��־��Ϣ���ļ���,ǰ������">test.log</��־�ļ�>
		</��������>
		<ǰ������>
			<LISTENPORT name="�����ṩ����������˿�">9001</LISTENPORT>
			<�����߳��� name="�����Ĵ����߳��������ݲ����������ã���СΪACCEPT�߳���">40</�����߳���>
			<ͨѶ����ͷ���� name="ͨѶ����ͷ����,��СΪ4">5</ͨѶ����ͷ����>
			<ͨѶ����ͷ�Ƿ�������� name="ͨѶ����ͷ�Ƿ��������ĳ���0������">0</ͨѶ����ͷ�Ƿ��������>
			<��������� name="����ͬʱ����socket���ӵ��������0��������С100">0</���������>
			<δʹ�öϿ�ʱ�� name="��������ӳ�ʱ�䳬����������Ͽ�����λ�룬��С60">600</δʹ�öϿ�ʱ��>
			<����ʱ�� name="��ʱ��ʱ��������δʹ�ã�������������"></����ʱ��>
			<��̬��·�� name="ǰ��ʹ�õĶ�̬���·���������������·������PATH��LIBPATH����">D:\����\bubase�������\lib</��̬��·��>
			<���Ի�· name="��ʹ�ô���̬�⣬ֱ�ӽ�����ԭ�ⲻ���ķ���,��������Ϊ0">1</���Ի�·>
			<���ӳؽ������� name="���ӳ��߳̽������ݣ�Ȼ�󽻸������߳�1�������ɴ����߳�ȥ��������0">1</���ӳؽ�������>
			<����̬������ name="ǰ�ô����̶߳�̬������Ƽ�ʵ��������" method="getProcessInstance">dllrunfrontd.dll</����̬������>
		</ǰ������>
		<�������� name="ǰ�ý������ã�IP�����IP��ַ��transΪ1��ʾҪ���а�ת������ת�������ļ�Ϊfilename��ֵ��IDΪ������ţ�IP��ID���ܹ��ظ�,�������ô����ʾ���Ծ��ɽ��룬�Ƿ��ת����ǰ����Ƿ��ת����������">
			<? <���� IP="192.168.22.47" Port="7200" trans="1" filename="D:\����\BUBASE�������\etc\fjsms.xml" id="1"/>?>
			<?<���� IP="127.0.0.2" Port="7200" trans="1" filename="D:\����\BUBASE�������\etc\fjsms.xml" id="0"/>?> 
		</��������>
	</head>
	<pack>
	</pack>
</package>

*/
#define  MAXPROCESSTHREAD 400  //������߳���


#ifdef WIN32
#define INT64  __int64 
#else
#define INT64  long long 
#endif

typedef struct 
{
	char  sip[21];
	int   nLen;
}S_IP_HEAD; 

class  CGResource  : public CIGResource 
{
public:
	
	CGResource();
	virtual ~CGResource();

	

public:
	bool g_bToExit;//�Ƿ��˳�����
	
	int g_nPort;//�����˿� Ĭ��Ϊ9001
	int g_nLoglevel;//��־���� Ĭ��Ϊ5
	std::string g_sLogFileName;//��־�ļ���
	std::string g_sLogFilePath;//��־Ŀ¼
	int g_nLogSize;

	int  g_nProcessThreadNum;//�����߳���,Ĭ��Ϊ10
	int g_nMaxConnectNum;//��������� 0Ϊ�����ƣ�Ĭ��
	int g_nDisconnectTime;//δʹ�öϿ�ʱ�䣬���һ�������ڴ�ʱ����һֱû��ʹ�ã��򽫴����ӶϿ�����λ��Ĭ��600�뼴10����
	int g_nDispatchTime;//�������ʱ��

	int g_nQueueDelayTime;//���г�ʱʱ��
	int g_nHeadNum;//ͨѶ����ͷ���ȣ�Ĭ��Ϊ5
	unsigned int g_nHeartRun;//����ʱ��,���������ӳ�����ʱ��δ��ʱ��������������


	unsigned long  g_processNum;//����������
	unsigned long  g_processSucessNum;//����ɹ�������

	bool g_bIsMonitor;//�Ƿ�����Ϣ


	int g_nBuPort;//bu�����˿�
	std::string g_sBuPrgName;//bu������
	std::string g_sBuPrgPath;//bu�����·��
	std::string g_sBuRunPath;//bu����Ŀ¼
	std::string g_sBuPrgPara;//bu����������
	bool g_bIsAutoLoadBu;//�Ƿ�����װ��BU

	CBF_LogClient m_log;//��־��
public:
	int GetHeadLenByIp(const char  *ip);

	// ������: WriteBack
	// ���  : ������ 2010-3-16 14:58:20
	// ����  : void 
	// ����  : д�������ļ�
	void WriteBack();

	std::string m_frontParaFileName;//ǰ�������ļ�

	


	/// ������: PutProcessNum
	/// ���  : ������ 2009-1-6 10:20:11
	/// ����  : int 
	/// ����  : ���ô���������
	int PutProcessNum();

	/// ������: PutProcessSucessNum
	/// ���  : ������ 2009-1-6 10:20:14
	/// ����  : int 
	/// ����  : ���óɹ�����������
	int PutProcessSucessNum();

	/// ������: GetGenerateSerial
	/// ���  : ������ 2008-9-5 13:18:17
	/// ����  : INT64 
	/// ����  : ��ȡ���ӵĴ���
	unsigned long  GetGenerateSerial();

	/// ������: SetListen
	/// ���  : ������ 2008-9-5 9:22:36
	/// ����  : bool 
	/// ����  : ��������־��Ϊtrue,���������򷵻�false,����������־��Ϊtrue������true
	bool SetListen();


	/// ������: SetInitOnce
	/// ���  : ������ 2008-9-4 9:17:41
	/// ����  : bool trueҪ��ʼ��false�����ʼ
	/// ����  : ����ʼ����־��Ϊtrue,���ѳ�ʼ�򷵻�false,���򽫳�ʼ��־��Ϊtrue������true
	bool SetInitOnce();


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
	std::vector<S_IP_HEAD> m_ip_head_list;//ip��ַ��Ӧ��ͨѶͷ���ⳤ��
	unsigned long m_serial_connect;//�������
	CBF_Mutex m_mutex;//�������
	std::string m_errMsg;//������Ϣ
	

	CBF_Date_Time m_publog;
	CBF_Tools     m_pTool;
	bool g_bIsInitOnce;//�Ƿ��ʼ��һ�Σ�����һ���߳�����г�ʼ���󣬽��˱�־��Ϊ1�������߳��жϴ˱�־�Ƿ���г�ʼ��
	bool g_bIsListen;//�Ƿ�������ֻ�ܵ���һ��

	unsigned long m_nMaxSerial;//������ˮ
	unsigned long m_nSerialNum;//��ˮ����

	CBF_Mutex m_mutex_putprocess;//�������
	CBF_Mutex m_mutex_putprocesssucess;//�������

	//ȫ�ֱ���
	CBF_Mutex m_mutex_setgpara1;
	CBF_Mutex m_mutex_setgpara2;
	CBF_Mutex m_mutex_setgpara3;
	bool   m_bisSetGPara1;
	bool   m_bisSetGPara2;
	bool   m_bisSetGPara3;


	CBF_Mutex m_mutex_getSerial;//ȡΨһ��ˮ��
	long   m_nSerial_get;//��ˮ���
	long   m_nSerial_getMax;//�����ˮ���
};

#endif // !defined(AFX_GRESOURCE_H__A7724768_0CB1_4D54_B228_60D1463D2AED__INCLUDED_)
