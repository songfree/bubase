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
#include "BF_Xml.h"
#include "BF_LogClient.h"
#include "BF_Tools.h"
#include "IGRes.h"
#include "BpcHead.h"
#include "BF_CommDataLog.h"
#include "BF_Queue.h"
#include "BF_DrebResource.h"

//�����ļ�����
/*<?xml version="1.0" encoding="GB2312"?>
<package>
   <head>
      <��������>
         <��־���� remark="��־���ټ���0���ش���1��Ҫ����2һ�����3����4��ʾ����5������Ϣ">6</��־����>
         <DATA��־���� remark="��־���ټ���0���ش���1��Ҫ����2һ�����3����4��ʾ����5������Ϣ">5</DATA��־����>
         <��־�ļ� remark="��¼��־��Ϣ���ļ���,ǰ������">bfbpc.log</��־�ļ�>
         <��־Ŀ¼ remark="��־��ŵ�Ŀ¼"></��־Ŀ¼>
      </��������>
      <DREB����>
            <DREB1 ip="127.0.0.1" port="9011" bandwidth="100" remark="�����������ߵĲ���" />
			<DREB1 ip="127.0.0.1" port="9012" bandwidth="100" remark="�����������ߵĲ���" />
      </DREB����>
       <������� use="1" host="1" remark="useΪ��������ѡ�0������ hostΪ������Դ����ѡ��0������">
        <��ع����ڵ��>0</��ع����ڵ��>
        <���˽�нڵ��>0</���˽�нڵ��>
        <��ع��������>0</��ع��������>
        <���˽�з����>0</���˽�з����>
        <��ر��潻����>99001</��ر��潻����>
     </�������>
      <BPC����>
         <��������� remark="��dreb��ע��ķ����">100</���������>
         <˽�з���� remark="��dreb��ע���˽�з����">1</˽�з����>
         <�����ˮ���� remark="Bpu�������ˮ���ֵ���������1���¿�ʼ">99999999</�����ˮ����>
         <������ˮ��ʼֵ remark="Bpu�������ˮ��ʼֵ">1</������ˮ��ʼֵ>
         <������䳬ʱʱ�� remark="��������BU�ĳ�ʱʱ�䣬��">10</������䳬ʱʱ��>
         <���ӹ���ʱ�� remark="�����ӳ�ʱ��δ������ʱ�Ͽ�,��">600</���ӹ���ʱ��>
         <DREB����ʱ�� remark="��DREB������ʱ��,��">60</DREB����ʱ��>
         <����ע��ģʽ remark="0��ʾ��̬ע�ᣬ1��ʾ��̬ע�� 2��ʾ��ע��">0</����ע��ģʽ>
         <����ע���ļ� remark="��̬ע��ʱ���ļ���">txlist.xml</����ע���ļ�>
      </BPC����>
      
</package>
*/

#ifdef WIN32
#define INT64  __int64 
#else
#define INT64  long long 
#endif

#define  DISPATCHMODE_FREE  0
#define  DISPATCHMODE_ALL   1

#define  REGISTERMODE_DYNAMIC   0   //��̬ע��  BPU��BPCע�ᣬBPC������ע��
#define  REGISTERMODE_STATIC    1   //��̬ע��  BPC������ע�ᣬBPU��ע��
#define  REGISTERMODE_NONE      2   //��ע��    BPC��������ע�ᣬBPU��BPCע��


class CPoolData;
typedef struct 
{
	std::string g_sBpuGroupName;//������
	std::string g_sBuPrgName;//bpu������
	std::string g_sBuPrgPath;//bpu�����·��
	std::string g_sBuRunPath;//bpu����Ŀ¼
	std::string g_sBuPrgPara;//bpu����������
	bool g_bIsAutoLoadBu;//�Ƿ�����װ��BPU
	int  g_nBpuNum;//��������bpu��Ŀ
	int  g_nStartBpuNum;//��������bpu��Ŀ
	bool g_bIsReg; //�Ƿ�ע��
	bool g_bIsAnsPing;//�Ƿ���Ӧ����
	bool g_bIsUnencrypt;//�Ƿ�������
	int  g_nDispatchMode;//����ģʽ 0��ʾ���з��䣬1��ʾBPU��ֻ��Ϊ1��һֱ����
	int  g_nBeginSocketIndex;//��bpu�鿪ʼ��socket index
	int  g_nEndSocketIndex;  //��bpu�������socket index��������
	int  g_nJavaBpuPid;//JAVA�汾��BPU�Ľ���ID
	int  g_nUseBigEndian;//JAVA�汾��BPU������������

	CPoolData *pdispathQueue;//���ɶ��� ÿ��bpu��һ��
	int       bpugroupindex; //bpu������ 
}S_BPUGROUP_INFO;

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

typedef struct 
{
	char      bpugroupname[21];//bpu����
	int       bpugroupindex; //bpu������ 
}S_BPUGROUP_TBL;
class  CGResource  : public CIGResource,public CBF_DrebResource 
{
public:
	
	CGResource();
	virtual ~CGResource();

public:
	S_SHM_PUBLIC  *m_pSerial;
	bool g_bToExit;//�Ƿ��˳�����
	int    g_nPort;//�����˿� Ĭ��Ϊ9001

 	std::string m_sBpcParaFileName;//�����ļ�����

	int g_nDispatchTime;//��Ϣ��������ʱ��

	int g_nRegisterMode;//����ע��ģʽ   0��ʾ��̬ע�ᣬ1��ʾ��̬ע�� 2��ʾ��ע��
	std::string g_sRegisterFile;//����ע���ļ�
	unsigned long  g_processNum;//����������
	unsigned long  g_processSucessNum;//����ɹ�������

	std::vector<S_BPUGROUP_INFO> g_vBpuGroupInfo; //��¼bpu�����Ϣ
	int  m_bIsRecTime;
	std::vector<int>  g_lBCFuncList;//�㲥���� ��Ҫ�����߶��ĵĹ㲥
public:

	void Stop();

	// ������: PushData
	// ���  : ������ 2015-11-4 14:29:07
	// ����  : int 
	// ����  : unsigned int bpugroypindex
	// ����  : S_BPC_RSMSG data
	// ����  : int prio
	// ����  : ����BPU��index�����ݷ������
	int PushData(unsigned int bpugroypindex,S_BPC_RSMSG data,int prio);

	std::string GetNextFileName(unsigned int &serial);

	// ������: WriteBack
	// ���  : ������ 2010-3-16 14:58:20
	// ����  : void 
	// ����  : д�������ļ�
	void WriteBack();

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
	virtual bool Init(char *confile, CIErrlog *log);


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

	CIErrlog *m_log;//��־��

	void LogTime();
//	void PutTime(const char *filename,int fileline);
	void PutTime();

protected:
	
	std::vector<INT64_> m_vTime;

	unsigned long m_serial_connect;//�������
	CBF_Mutex m_mutex;//�������
	std::string m_errMsg;//������Ϣ
	
	CBF_Date_Time m_datetime;

	bool g_bIsInitOnce;//�Ƿ��ʼ��һ�Σ�����һ���߳�����г�ʼ���󣬽��˱�־��Ϊ1�������߳��жϴ˱�־�Ƿ���г�ʼ��
	bool g_bIsListen;//�Ƿ�������ֻ�ܵ���һ��

	unsigned long m_nMaxSerial;//������ˮ
	unsigned long m_nSerialNum;//��ˮ����

	unsigned int m_nNextFileSerial;//�������ļ���ˮ

	CBF_Mutex m_mutex_putprocess;//�������
	CBF_Mutex m_mutex_putprocesssucess;//�������

	//ȫ�ֱ���
	CBF_Mutex m_mutex_setgpara1;
	CBF_Mutex m_mutex_setgpara2;
	CBF_Mutex m_mutex_setgpara3;
	bool   m_bisSetGPara1;
	bool   m_bisSetGPara2;
	bool   m_bisSetGPara3;


	CBF_Mutex m_mutex_getnextfile;

	CBF_Mutex m_mutex_getSerial;//ȡΨһ��ˮ��

//	long   m_nSerial_get;//��ˮ���
	
	long   m_nSerial_getMax;//�����ˮ���


	CMemTable <S_BPUGROUP_TBL> m_table;               //������
	CPkeyCharF<21,1>           m_pkey;                //����
};

#endif // !defined(AFX_GRESOURCE_H__A7724768_0CB1_4D54_B228_60D1463D2AED__INCLUDED_)
