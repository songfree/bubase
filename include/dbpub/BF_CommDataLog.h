// BF_CommDataLog.h: interface for the CBF_CommDataLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BF_COMMDATALOG_H__9F3DE176_FE0E_4392_93D4_725085721538__INCLUDED_)
#define AFX_BF_COMMDATALOG_H__9F3DE176_FE0E_4392_93D4_725085721538__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//ͨѶ������־�࣬��dreb/bpc��ͨѶ�ı��ļ�¼����־����
//����һ���������߳���������־�ļ���С������

#include "BF_Date_Time.h"
#include "BpcHead.h"
#include "BF_Queue.h"
#include "BF_Timer.h"
#include "BF_BufferPool.h"
#include "BF_Thread.h"
#include "IErrLog.h"
#include "BF_Tools.h"
#include "BF_SimpleLog.h"

#ifdef _WINDOWS
#ifdef _DEBUG
#pragma comment(lib, "bf_kerneld.lib") 
#pragma message("Automatically linking with   bf_kerneld.lib ")
#else
#pragma comment(lib, "bf_kernel.lib") 
#pragma message("Automatically linking with   bf_kernel.lib ")
#endif

#endif

#ifdef DBPUB_EXPORTS
#define DBPUB_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define DBPUB_EXPORT __declspec(dllimport)
#else
#define DBPUB_EXPORT 
#endif
#endif

//��־�ļ���ʽ
//�ļ���ǰ2�ֽڱ�ʾ�ֽ������־����
//��һ�ֽ�   ȡֵ��B��L   B��ʾBIG_ENDIAN�ֽ���  �������aix hp_ux��ʹ�õ��ֽ���  L��ʾLITTLE_ENDIAN�ֽ��� linux��windowsʹ�õ��ֽ���
//�ڶ��ֽ�   ȡֵ��0��1   0��ʾdreb������־   1��ʾbpc������־
//�������ݼ����½ṹ
// 
// typedef struct   bpc������־
// {
// 	unsigned int nTime;//����ʱ��  hhmmss
// 	unsigned int nUs;  //΢��
// 	unsigned int nId;  //����ID
// 	BPCCOMMSTRU  data;
// }S_LOG_BPCDATA;
// 
// typedef struct   dreb������־
// {
// 	unsigned int nTime;//����ʱ��  hhmmss
// 	unsigned int nUs;  //΢��
// 	unsigned int nId;  //����ID
// 	COMMSTRU  data;
// }S_LOG_DREBDATA;




class DBPUB_EXPORT CBF_CommDataLog  : public CBF_Thread
{
public:
	int GetLogLevel();

	// ������: SetLogLevel
	// ���  : ������ 2015-7-13 15:10:06
	// ����  : void 
	// ����  : int loglevel
	// ����  : ������־����
	void SetLogLevel(int loglevel);
	
	
	CBF_CommDataLog();
	virtual ~CBF_CommDataLog();

	// ������: LogDreb
	// ���  : ������ 2013-5-6 16:00:38
	// ����  : int 
	// ����  : int level
	// ����  : PBPCCOMMSTRU data
	// ����  : bool flag false������־  true������־
	// ����  : д��dreb����־
	int LogDreb(int level ,PBPCCOMMSTRU data,bool flag=false);
	
	// ������: LogDreb
	// ���  : ������ 2013-5-6 16:00:38
	// ����  : int 
	// ����  : int level
	// ����  : PCOMMSTRU data
	// ����  : bool flag false������־  true������־
	// ����  : д��dreb����־
	int LogDreb(int level ,PCOMMSTRU data,bool flag=false);
	
	// ������: SetLogPara
	// ���  : ������ 2013-5-6 16:00:11
	// ����  : void 
	// ����  : int maxlogsize   �����־��С��С��1024������
	// ����  : int level   ��־����
	// ����  : const char *logpath    ��־Ŀ¼
	// ����  : const char *dreblogname dreb������־��
	// ����  : const char *bpclogname bpc������־��
	// ����  : ���ò���
	void SetLogPara(int maxlogsize,int level ,const char *logpath,const char *dreblogname,const char *bpclogname);
	
	
	// ������: LogBpc
	// ���  : ������ 2013-5-6 16:00:20
	// ����  : void 
	// ����  : int level
	// ����  : PBPCCOMMSTRU data
	// ����  : bool flag  false������־  true������־
	// ����  : д��BPC����־
	int LogBpc(int level,PBPCCOMMSTRU data,bool flag=false);


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

protected:

	FILE   *m_pBpcFile;  //bpc������־ָ��
	FILE   *m_pBpcErrFile;
	FILE   *m_pDrebFile;
	FILE   *m_pDrebErrFile;


	std::string    m_sLogPath;  //�ļ�·��
	std::string    m_sDrebLogName;//dreb��־�ļ���
	std::string    m_sDrebErrLogName;//dreb������־�ļ���
	std::string    m_sBpcLogName;//bpc��־�ļ���
	std::string    m_sBpcErrLogName;//bpc������־�ļ���

	char           m_sDrebLogFilePathName[500];//��ǰ��DREB��־�ļ�
	char           m_sDrebErrLogFilePathName[500];//��ǰ��DREB������־�ļ�
	char           m_sBpcLogFilePathName[500];//��ǰ��BPC��־�ļ�
	char           m_sBpcErrLogFilePathName[500];//��ǰ��BPC������־�ļ�

	int            m_nLogLevel; //��־����


	unsigned int   m_nMaxLogSize;//��־�ļ���С 0������һֱд

	unsigned int   m_nCurDrebLogSize;//��ǰdreb��־��С
	unsigned int   m_nCurBpcLogSize;//��ǰBPC��־��С
	unsigned int   m_nCurDrebErrLogSize;//��ǰdreb������־��С
	unsigned int   m_nCurBpcErrLogSize;//��ǰBPC������־��С


	std::string    m_sCurDate;//��ǰ����,����д��ʱ����ʱҪ�Ƚ�����


	CBF_Stack<char *>  m_pQueue;  //���� ��һ���ֽڱ�ʾ��dreb����bpc��0DREB 1DREBERR 2BPC 3BPCERR,�������S_LOG_BPCDATA�ṹ��S_LOGDREBDATA

	CBF_Date_Time  m_pDate;
	CBF_BufferPool m_pMemPool;
	CBF_Timer      m_pTimer;
	CBF_SimpleLog  m_pLog;

	bool           m_bIsExit;     //�߳��Ƿ��˳�
	bool           m_bIsCheckDate;//�Ƿ�������
	bool           m_bIsSetPara;  //�Ƿ����setlogpara
	bool           m_bIsBigEndian; //�Ƿ�������


	// ������: int DateChange
	// ���  : ������ 2014-11-18 15:27:12
	// ����  : static  
	// ����  : unsigned int eventid
	// ����  : void *p
	// ����  : ��ʱ���¼�
	static  int DateChange(unsigned int eventid,void *p);

private:
	void ClearQueueAndClose();

	bool WriteBpcErrLog(char *logdata);
	bool WriteBpcLog(char *logdata);
	bool WriteDrebErrLog(char *logdata);
	bool WriteDrebLog(char *logdata);

	// ������: OpenDrebLogFile
	// ���  : ������ 2014-11-18 15:26:50
	// ����  : bool 
	// ����  : ����־�ļ�
	bool OpenDrebLogFile();
	bool OpenDrebErrLogFile();
	bool OpenBpcLogFile();
	bool OpenBpcErrLogFile();


	// ������: WriteLog
	// ���  : ������ 2014-11-18 15:27:01
	// ����  : bool 
	// ����  : char *logdata
	// ����  : д����־
	bool WriteLog(char *logdata);

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
};

#endif // !defined(AFX_BF_COMMDATALOG_H__9F3DE176_FE0E_4392_93D4_725085721538__INCLUDED_)
