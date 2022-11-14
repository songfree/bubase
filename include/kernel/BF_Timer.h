// BF_Timer.h: interface for the CBF_Timer class.
//
//////////////////////////////////////////////////////////////////////

/***********************************************************************************
  ������־��¼
  ������: ������
  �������� 2012-4-11 17:06:08 �ع�
  �汾: V1.00 
  ˵��: ��ʱ���࣬ͨ����ʱ���߳̽��ж�ʱ,ʹ���������£�
class testtimer
{
public:
	CBF_Timer tmer;
	CBF_Date_Time tDate;
	
	testtimer()
	{
		tmer.Init(10,true); //��ʼ��ʱ������
		tmer.SetTimer(0,1000,&testtimer::OnTimer,this); //���ö�ʱ��
		tmer.SetTimer(1,5000,&testtimer::OnTimer,this);//���ö�ʱ��
		tmer.Start();//������ʱ��
	}
	virtual ~testtimer()
	{
		tmer.KillTimer(0); //ɾ��һ����ʱ
		tmer.Stop();//ֹͣ��ʱ��
	}
	void Stop()
	{
		tmer.Stop();
	}
	
	void join()
	{
		tmer.Join();
	}
	static int OnTimer(unsigned int event,void *p) //��ʱ���ص�����
	{
		testtimer *pp = (testtimer *)p;
		pp->tDate.Update();
		char sdate[10];
		char stime[9];
		bzero(sdate,sizeof(sdate));
		bzero(stime,sizeof(stime));
		pp->tDate.ToCharDate(sdate);
		pp->tDate.ToCharTime(stime);
		printf("%d %s %s \n",event,sdate,stime);
		return 0;
	}
};

 ***********************************************************************************/
#if !defined(AFX_BF_TIMER_H__7D335AB3_46FA_40BE_8CF9_8348AAE23018__INCLUDED_)
#define AFX_BF_TIMER_H__7D335AB3_46FA_40BE_8CF9_8348AAE23018__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "BF_Thread.h"
#include "BF_Date_Time.h"
//#include "BF_SimpleLog.h"
#include "BF_Mutex.h"


#ifdef KERNEL_EXPORTS
#define KERNEL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNEL_EXPORT __declspec(dllimport)
#else
#define KERNEL_EXPORT 
#endif
#endif

#ifdef _WINDOWS
#pragma comment(lib, "WS2_32.lib") 
#pragma message("Automatically linking with WS2_32.lib")
#endif


typedef  int(*TIME_CALL_BACK)(unsigned int eventid,void *p);

class KERNEL_EXPORT CBF_Timer   : public CBF_Thread
{
public:

	// ������: Init
	// ���  : ������ 2012-4-16 11:56:23
	// ����  : void 
	// ����  : unsigned long tick ��ʱ���ms  ��select��ms windows����СΪ10
	// ����  : bool accuracy=false   �Ƿ�ȷ��ʱ
	// ����  : ��ʼ����ʱ���ľ�ȷ�Լ���ʱ���
	void Init(unsigned long tick,bool accuracy=false, int cpucores = 0);

	// ������: Stop
	// ���  : ������ 2012-4-16 11:46:07
	// ����  : void 
	// ����  : ֹͣ��ʱ���̣߳����ԵĶ�ʱ��ֹͣ��������ɾ����ʱ��
	void Stop();

	// ������: Start
	// ���  : ������ 2012-4-16 11:45:28
	// ����  : bool 
	// ����  : ������ʱ���̣߳�֮����Ե���SetTimer�趨һ����ʱ��
	bool Start();

	// ������: OnTimer
	// ���  : ������ 2012-4-11 17:44:12
	// ����  : virtual int 
	// ����  : unsigned int event ��ʱ��ID
	// ����  : ��ʱ���ص��麯�����̳�ʱʹ�á�Ҫע���̰߳�ȫ
	virtual int OnTimer(unsigned int event);

	// ������: SetTimer
	// ���  : ������ 2012-4-11 17:44:42
	// ����  : bool 
	// ����  : unsigned int timerid ��ʱ��ID
	// ����  : unsigned long tms     ��ʱ����
	// ����  : TIME_CALL_BACK cb   �ص�����
	// ����  : void *p   �ص��������ڵ���ָ��
	// ����  : unsigned short timeflag ��ʱ������ 0һ����  1ѭ���� 2��һ�γ�ʱ����
	// ����  : ����һ����ʱ��
	bool SetTimer(unsigned int timerid,unsigned long tms,TIME_CALL_BACK cb,void *p,unsigned short timeflag=1);

	// ������: KillTimer
	// ���  : ������ 2012-4-11 17:45:22
	// ����  : bool 
	// ����  : unsigned int timerid ��ʱ��ID
	// ����  : ɾ��һ����ʱ����ע�ⲻ���ڻص���������ñ�����
	bool KillTimer(unsigned int timerid);

	CBF_Timer();
	virtual ~CBF_Timer();
	//CBF_SimpleLog  m_pLog;
protected:

	// ������: ProcessTimer
	// ���  : ������ 2012-4-16 17:33:45
	// ����  : void 
	// ����  : ����ʱ�¼�
	void ProcessTimer();

	// ������: InitThreadInstance
	// ���  : ������ 2012-4-9 12:00:54
	// ����  : virtual bool 
	// ����  : �̳߳�ʼ�����߳�����ʱ����һ��
	virtual bool InitThreadInstance();
	
	// ������: ExitThreadInstance
	// ���  : ������ 2012-4-9 12:01:05
	// ����  : virtual void 
	// ����  : �˳��߳�Ҫ���Ĺ���,�߳��˳�ʱ����һ��
	virtual void ExitThreadInstance();
	
	// ������: Run
	// ���  : ������ 2012-4-9 12:01:23
	// ����  : virtual int 
	// ����  : �߳����з��������˷������أ����߳��Զ���ֹ
	virtual int Run(); 

	typedef struct  
	{
		unsigned int    nEventId;//��ʱ��ID
		unsigned long   nTimeOut;//��ʱ����ʱʱ�� ms
		INT64_          nTimeCount;//��ʱ��ʱ����� ms
		long            nCallNum;//�ص�����
		unsigned short  nTimeFlag;//��ʱ������  0 һ����  1ѭ����   2��һ�γ�ʱ����
		TIME_CALL_BACK  pCB;//�ص�����
		void            *pC; //�ص�����������ʵ��ָ��
		bool            bIsUse;//�˶�ʱ���Ƿ�ʹ��
	}BF_TIMER_PARA;
	
	typedef std::list<BF_TIMER_PARA> LIST_TIMER;
	
	LIST_TIMER m_lTimer;
	
	unsigned long   m_nTimerTick;  //��ʱ��select ��ʱ�� ms
	bool            m_bTimerAccuracy;//�Ƿ�ȷ�Ķ�ʱ ��ȷ�Ķ�ʱ��Ҫ��cpu��Դ
	bool            m_bIsExit;//��ʱ��ֹͣ��־
	
	CBF_Mutex       m_mutex;  //����
	CBF_Date_Time   m_pDateTime;
#ifdef _WINDOWS
	typedef SOCKET SOCKET_HANDLE;
	SOCKET_HANDLE m_sock;
#endif
	int             m_nCpucore;
};


class KERNEL_EXPORT CBF_TimerUs   : public CBF_Thread
{
public:

	// ������: Init
	// ���  : ������ 2012-4-16 11:56:23
	// ����  : void 
	// ����  : unsigned long tick ��ʱ���us  ��select��us windows����СΪ10ms
	// ����  : bool accuracy=false   �Ƿ�ȷ��ʱ
	// ����  : ��ʼ����ʱ���ľ�ȷ�Լ���ʱ���
	void Init(unsigned long tick,bool accuracy=false);

	// ������: Stop
	// ���  : ������ 2012-4-16 11:46:07
	// ����  : void 
	// ����  : ֹͣ��ʱ���̣߳����ԵĶ�ʱ��ֹͣ��������ɾ����ʱ��
	void Stop();

	// ������: Start
	// ���  : ������ 2012-4-16 11:45:28
	// ����  : bool 
	// ����  : ������ʱ���̣߳�֮����Ե���SetTimer�趨һ����ʱ��
	bool Start();

	// ������: OnTimer
	// ���  : ������ 2012-4-11 17:44:12
	// ����  : virtual int 
	// ����  : unsigned int event ��ʱ��ID
	// ����  : ��ʱ���ص��麯�����̳�ʱʹ�á�Ҫע���̰߳�ȫ
	virtual int OnTimer(unsigned int event);

	// ������: SetTimer
	// ���  : ������ 2012-4-11 17:44:42
	// ����  : bool 
	// ����  : unsigned int timerid ��ʱ��ID
	// ����  : unsigned long tus     ��ʱ΢��
	// ����  : TIME_CALL_BACK cb   �ص�����
	// ����  : void *p   �ص��������ڵ���ָ��
	// ����  : unsigned short timeflag ��ʱ������ 0һ����  1ѭ���� 2һ�γ�ʱ����
	// ����  : ����һ����ʱ��
	bool SetTimer(unsigned int timerid,unsigned long tus,TIME_CALL_BACK cb,void *p,unsigned short timeflag=1);

	// ������: KillTimer
	// ���  : ������ 2012-4-11 17:45:22
	// ����  : bool 
	// ����  : unsigned int timerid ��ʱ��ID
	// ����  : ɾ��һ����ʱ����ע�ⲻ���ڻص���������ñ�����
	bool KillTimer(unsigned int timerid);

	CBF_TimerUs();
	virtual ~CBF_TimerUs();

protected:

	// ������: ProcessTimer
	// ���  : ������ 2012-4-16 17:33:45
	// ����  : void 
	// ����  : ����ʱ�¼�
	void ProcessTimer();

	// ������: InitThreadInstance
	// ���  : ������ 2012-4-9 12:00:54
	// ����  : virtual bool 
	// ����  : �̳߳�ʼ�����߳�����ʱ����һ��
	virtual bool InitThreadInstance();

	// ������: ExitThreadInstance
	// ���  : ������ 2012-4-9 12:01:05
	// ����  : virtual void 
	// ����  : �˳��߳�Ҫ���Ĺ���,�߳��˳�ʱ����һ��
	virtual void ExitThreadInstance();

	// ������: Run
	// ���  : ������ 2012-4-9 12:01:23
	// ����  : virtual int 
	// ����  : �߳����з��������˷������أ����߳��Զ���ֹ
	virtual int Run(); 

	typedef struct  
	{
		unsigned int    nEventId;//��ʱ��ID
		INT64_          nTimeOut;//��ʱ����ʱʱ�� US
		INT64_          nTimeCount;//��ʱ��ʱ����� 
		long            nCallNum;//�ص�����
		unsigned short  nTimeFlag;//��ʱ������  0 һ����  1ѭ���� 2��һ�γ�ʱ����nTimeOut >100000ms
		TIME_CALL_BACK  pCB;//�ص�����
		void            *pC; //�ص�����������ʵ��ָ��
		bool            bIsUse;//�˶�ʱ���Ƿ�ʹ��
	}BF_TIMER_PARA_US;

	typedef std::list<BF_TIMER_PARA_US> LIST_TIMER_US;

	LIST_TIMER_US m_lTimer;

	unsigned long   m_nTimerTick;  //��ʱ��select ��ʱ��us
	bool            m_bTimerAccuracy;//�Ƿ�ȷ�Ķ�ʱ ��ȷ�Ķ�ʱ��Ҫ��cpu��Դ
	bool            m_bIsExit;//��ʱ��ֹͣ��־
	CBF_Mutex       m_mutex;  //����
	CBF_Date_Time   m_pDateTime;
#ifdef _WINDOWS
	typedef SOCKET SOCKET_HANDLE;
	SOCKET_HANDLE m_sock;
#endif
};

#endif // !defined(AFX_BF_TIMER_H__7D335AB3_46FA_40BE_8CF9_8348AAE23018__INCLUDED_)
