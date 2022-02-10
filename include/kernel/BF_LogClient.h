// BF_LogClient.h: interface for the CBF_LogClient class.
//
//////////////////////////////////////////////////////////////////////

/***********************************************************************************
  ������־��¼
  ������: ������
  �������� 2012-5-11 14:09:58
  �汾: V1.00 
  ˵��: ��־���������õĲ�����������������־����������ֱ��д��־�ļ���ͨ���߳�ʵ�֡�
        ֧�ֶ��̵߳���
 ***********************************************************************************/


#if !defined(AFX_BF_LOGCLIENT_H__9EB53084_7DA6_41C6_A461_C8DBBAC75974__INCLUDED_)
#define AFX_BF_LOGCLIENT_H__9EB53084_7DA6_41C6_A461_C8DBBAC75974__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IErrLog.h"
#include "BF_Date_Time.h"
#include "BF_Thread.h"
#include "BF_Mutex.h"
#include "BF_BufferPool.h"
#include "BF_Timer.h"
#include "BF_SocketTcp.h"
#include "BF_Queue.h"
#include "BF_SimpleLog.h"



#define    LOG_QUEUEMAXNUM    1000   //��־������������־��  
#define    LOG_BUFFERSIZE     65534   //��־�����С
    
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
	char            loglevel;     //��־����
	char            filename[41]; //�����ļ���
	unsigned short  fileline;     //�ļ���
	long            logus;        //����logmpʱ��
	char            wtype;        //0-LOGMP  1-LOGBIN
	int             threadid;     //�߳�id
	unsigned short  datalen;
	char            data[1];
}__PACKED__ S_LOG_DATA;

#define  S_LOG_HEADLEN   sizeof(S_LOG_DATA)-1    //��־ͷ����



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


#ifdef KERNEL_EXPORTS
#define KERNEL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNEL_EXPORT __declspec(dllimport)
#else
#define KERNEL_EXPORT 
#endif
#endif

 


class KERNEL_EXPORT CBF_LogClient : public CIErrlog,CBF_Thread
{
public:
	

	// ������: SetLogLevel
	// ���  : ������ 2015-11-12 11:12:05
	// ����  : void 
	// ����  : int loglevel
	// ����  : ������־����
	void SetLogLevel(int loglevel);

	// ������: GetLogLevel
	// ���  : ������ 2015-11-12 11:11:48
	// ����  : int 
	// ����  : ȡ��־����
	int GetLogLevel();


	// ������: IsStopedThread
	// ���  : ������ 2014-12-15 10:25:29
	// ����  : bool 
	// ����  : �߳��Ƿ�ֹͣ
	bool IsStopedThread();

	// ������: StopLog
	// ���  : ������ 2014-11-12 11:07:10
	// ����  : void 
	// ����  : ֹͣ��־ 
	void StopLog();
	

	// ������: StartLog
	// ���  : ������ 2012-5-14 15:05:28
	// ����  : bool 
	// ����  : ��ʼ��־��������־�߳�
	bool StartLog();


	CBF_LogClient();
	virtual ~CBF_LogClient();

	// ������: isWrite
	// ���  : ������ 2009-6-29 14:28:34
	// ����  : int 
	// ����  : int level ��־���� 
	// ����  : �Ƿ�Ҫд����־
	virtual bool isWrite(int level);
	
	// ������: SetLogPara
	// ���  : ������ 2009-6-29 14:28:34
	// ����  : int 
	// ����  : int level ��־���� 
	// ����  : const char *logfilepath  ��־�ļ�·��  ��Ϊ����־��������Ϊip@port@mode
	// ����  : const char *logfilename  ��־�ļ���    ��Ϊ����־��������Ϊ����
	// ����  : int id=-1
	// ����  : ������־����
	virtual void SetLogPara(int level,const char *logfilepath,const char *logfilename,int id=-1);
	
	/// ������: GetLogPara
	/// ���  : ������ 2012-4-25 11:21:05
	/// ����  : virtual void 
	/// ����  : int &level ��־����
	/// ����  : std::string &logfilepath ��־�ļ�·��
	/// ����  : std::string &logfilename ��־�ļ���
	/// ����  : int &id ���̻��߳�ID
	/// ����  : ȡ��־����
	virtual void GetLogPara(int &level,std::string &logfilepath, std::string &logfilename,int &id);
	
	/// ������: LogMp
	/// ���  : ������ 2009-5-16 17:28:24
	/// ����  : int 
	/// ����  : int level
	/// ����  : const char *filename
	/// ����  : int line
	/// ����  : const char *fmt
	/// ����  : ...
	/// ����  : д����־  Ҫȷ���ִ����ܳ���LOG_BUFFERSZIE
	virtual int LogMp(int level, const char *filename, int line,const char *fmt,...);
	
	/// ������: LogBin
	/// ���  : ������ 2007-7-10 11:27:52
	/// ����  : int -1ʧ�� 0�ɹ�
	/// ����  : int level  ��־����
	/// ����  : const char *filename  Դ�ļ���
	/// ����  : int line              Դ�ļ���
	/// ����  : const char *title  ����
	/// ����  : char *msg   ����
	/// ����  : int msglen  ����ע��  ����ҪС��8192-title����-8���������Ĳ�д��
	/// ����  : д���������������־�ļ�,'\0'��*����  ��msglen����LOG_BUFFERSZIE��С���ȥ
	virtual int LogBin(int level, const char *filename, int line,const char *title,char *msg,int msglen);



	// ������: SetMaxLogSize
	// ���  : ������ 2014-11-12 14:04:26
	// ����  : virtual void 
	// ����  : int size   ��С
	// ����  : ������־�ļ���С��С��1024������  �˷�������SetLogParaǰ����
	virtual void SetMaxLogSize(int size);
	
protected:


	// ������: RunServer
	// ���  : ������ 2014-11-13 9:20:04
	// ����  : int 
	// ����  : ��־��������ʽ�̷߳���
	int RunServer();

	// ������: RunLocal
	// ���  : ������ 2014-11-13 9:20:08
	// ����  : int 
	// ����  : ������־��ʽ�̷߳���
	int RunLocal();

	// ������: OpenLogFile
	// ���  : ������ 2012-5-14 14:52:47
	// ����  : bool 
	// ����  : ����־�ļ�
	bool OpenLogFile();
	
	
	// ������: WriteLog
	// ���  : ������ 2012-5-11 17:09:54
	// ����  : bool 
	// ����  : char *logdata
	// ����  : д������־�ļ�
	bool WriteLog(char *logdata);

	/// ������: GetName
	/// ���  : ������ 2007-7-10 10:52:10
	/// ����  : void 
	/// ����  : char *filename  
	/// ����  : ��ȡ�ļ����ƣ�����·�����ļ���
	int GetName(char *filename,char **pfn);

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
	
protected:
	void OnClose();

	// ������: OnSend
	// ���  : ������ 2014-11-13 10:48:48
	// ����  : void 
	// ����  : ���͸���־��������־��Ϣ(�Ӷ�����ȡ)
	void OnSend();

	// ������: OnRecv
	// ���  : ������ 2014-11-13 10:48:27
	// ����  : void 
	// ����  : ������־���������ݲ�������Ҫ������
	void OnRecv();

	// ������: OnPing
	// ���  : ������ 2014-11-13 9:56:32
	// ����  : void 
	// ����  : ��ʱ�����־���������ӣ�����һ��ʱ�������ݶ�д�������������������ҳ���һ��ʱ��û�ж�������Ͽ�
	void OnPing();

	// ������: OnEvent
	// ���  : ������ 2014-11-13 9:54:20
	// ����  : void 
	// ����  : ��־�����������ж����д���¼�����
	void OnEvent();


	// ������: OnNoEvent
	// ���  : ������ 2014-11-13 9:54:17
	// ����  : void 
	// ����  : ��־�����������޶��¼����Ҳ���д(�����Ӳ���д����־�������д�������ʱ��Ҫ���д���)
	void OnNoEvent();

	// ������: OnConnect
	// ���  : ������ 2014-11-13 9:54:06
	// ����  : void 
	// ����  : ������־������
	void OnConnect();

	
	
	static  int DateChange(unsigned int eventid,void *p);

	char           m_sLogFilePathName[500];//��ǰ����־�ļ�

	int            m_nLogLevel; //��־����
	int            m_nId;
	std::string    m_sLogPath;  //�ļ�·����Զ�̷���˵�IP�˿�ģʽ
	std::string    m_sLogName;
	char           m_sIp[30];//Զ����־����ip
	unsigned int   m_nPort;//Զ����־����˿�
	

    unsigned int   m_nMaxLogSize;//��־�ļ���С 0������һֱд
	unsigned int   m_nCurLogSize;//��ǰ��־��С
	
	std::string    m_sCurDate;//��ǰ����,����д��ʱ����ʱҪ�Ƚ�����
	bool           m_bIsCheckDate;//�Ƿ�������
	bool           m_bIsSetPara;
	bool           m_bIsLocal;
	bool           m_bIsExit;

	CBF_Stack<char *>  m_pQueue;
	CBF_Date_Time  m_pDate;
	CBF_BufferPool m_pMemPool;
	CBF_Timer      m_pTimer;
	FILE       *   m_pFp;
	
	CBF_SocketTcp  m_pTcpSocket;

	SOCKET_HANDLE  m_sock;
	fd_set m_rset;
	fd_set m_wset;
#if defined(_WINDOWS)
	fd_set m_eset;
#endif

	unsigned int m_nConntime; //����ʱ��
	unsigned int m_nReadtime; //��������ʱ��
	unsigned int m_nWriteTime;//д����ʱ��
	unsigned int m_nPingTime;//����ʱ��
	bool         m_bIsPingTimer;//��ʱ�����־
	bool         m_bIsConnect;//�Ƿ�������
	CBF_SimpleLog m_log;
	
	char         m_sRcvBuffer[8192];
	unsigned int m_nRcvLen;
	char         m_sSendBuffer[8192];
	unsigned int m_nSendLen;

//	CBF_Mutex    m_mutex;
private:
	
};

#endif // !defined(AFX_BF_LOGCLIENT_H__9EB53084_7DA6_41C6_A461_C8DBBAC75974__INCLUDED_)
