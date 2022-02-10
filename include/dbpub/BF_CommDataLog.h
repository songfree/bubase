// BF_CommDataLog.h: interface for the CBF_CommDataLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BF_COMMDATALOG_H__9F3DE176_FE0E_4392_93D4_725085721538__INCLUDED_)
#define AFX_BF_COMMDATALOG_H__9F3DE176_FE0E_4392_93D4_725085721538__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//通讯数据日志类，将dreb/bpc等通讯的报文记录到日志当中
//采用一个单独的线程来处理，日志文件大小，日切

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

//日志文件格式
//文件的前2字节表示字节序和日志类型
//第一字节   取值有B和L   B表示BIG_ENDIAN字节序  网络序或aix hp_ux所使用的字节序  L表示LITTLE_ENDIAN字节序 linux及windows使用的字节序
//第二字节   取值有0和1   0表示dreb数据日志   1表示bpc数据日志
//随后的数据见如下结构
// 
// typedef struct   bpc数据日志
// {
// 	unsigned int nTime;//日期时间  hhmmss
// 	unsigned int nUs;  //微秒
// 	unsigned int nId;  //进程ID
// 	BPCCOMMSTRU  data;
// }S_LOG_BPCDATA;
// 
// typedef struct   dreb数据日志
// {
// 	unsigned int nTime;//日期时间  hhmmss
// 	unsigned int nUs;  //微秒
// 	unsigned int nId;  //进程ID
// 	COMMSTRU  data;
// }S_LOG_DREBDATA;




class DBPUB_EXPORT CBF_CommDataLog  : public CBF_Thread
{
public:
	int GetLogLevel();

	// 函数名: SetLogLevel
	// 编程  : 王明松 2015-7-13 15:10:06
	// 返回  : void 
	// 参数  : int loglevel
	// 描述  : 设置日志级别
	void SetLogLevel(int loglevel);
	
	
	CBF_CommDataLog();
	virtual ~CBF_CommDataLog();

	// 函数名: LogDreb
	// 编程  : 王明松 2013-5-6 16:00:38
	// 返回  : int 
	// 参数  : int level
	// 参数  : PBPCCOMMSTRU data
	// 参数  : bool flag false正常日志  true出错日志
	// 描述  : 写入dreb的日志
	int LogDreb(int level ,PBPCCOMMSTRU data,bool flag=false);
	
	// 函数名: LogDreb
	// 编程  : 王明松 2013-5-6 16:00:38
	// 返回  : int 
	// 参数  : int level
	// 参数  : PCOMMSTRU data
	// 参数  : bool flag false正常日志  true出错日志
	// 描述  : 写入dreb的日志
	int LogDreb(int level ,PCOMMSTRU data,bool flag=false);
	
	// 函数名: SetLogPara
	// 编程  : 王明松 2013-5-6 16:00:11
	// 返回  : void 
	// 参数  : int maxlogsize   最大日志大小，小于1024不限制
	// 参数  : int level   日志级别
	// 参数  : const char *logpath    日志目录
	// 参数  : const char *dreblogname dreb正常日志名
	// 参数  : const char *bpclogname bpc正常日志名
	// 描述  : 设置参数
	void SetLogPara(int maxlogsize,int level ,const char *logpath,const char *dreblogname,const char *bpclogname);
	
	
	// 函数名: LogBpc
	// 编程  : 王明松 2013-5-6 16:00:20
	// 返回  : void 
	// 参数  : int level
	// 参数  : PBPCCOMMSTRU data
	// 参数  : bool flag  false正常日志  true出错日志
	// 描述  : 写入BPC的日志
	int LogBpc(int level,PBPCCOMMSTRU data,bool flag=false);


	// 函数名: StopLog
	// 编程  : 王明松 2014-11-12 11:07:10
	// 返回  : void 
	// 描述  : 停止日志 
	void StopLog();
	
	
	// 函数名: StartLog
	// 编程  : 王明松 2012-5-14 15:05:28
	// 返回  : bool 
	// 描述  : 开始日志，启动日志线程
	bool StartLog();

protected:

	FILE   *m_pBpcFile;  //bpc正常日志指针
	FILE   *m_pBpcErrFile;
	FILE   *m_pDrebFile;
	FILE   *m_pDrebErrFile;


	std::string    m_sLogPath;  //文件路径
	std::string    m_sDrebLogName;//dreb日志文件名
	std::string    m_sDrebErrLogName;//dreb出错日志文件名
	std::string    m_sBpcLogName;//bpc日志文件名
	std::string    m_sBpcErrLogName;//bpc出错日志文件名

	char           m_sDrebLogFilePathName[500];//当前的DREB日志文件
	char           m_sDrebErrLogFilePathName[500];//当前的DREB出错日志文件
	char           m_sBpcLogFilePathName[500];//当前的BPC日志文件
	char           m_sBpcErrLogFilePathName[500];//当前的BPC出错日志文件

	int            m_nLogLevel; //日志级别


	unsigned int   m_nMaxLogSize;//日志文件大小 0不限制一直写

	unsigned int   m_nCurDrebLogSize;//当前dreb日志大小
	unsigned int   m_nCurBpcLogSize;//当前BPC日志大小
	unsigned int   m_nCurDrebErrLogSize;//当前dreb错误日志大小
	unsigned int   m_nCurBpcErrLogSize;//当前BPC错误日志大小


	std::string    m_sCurDate;//当前日期,本地写入时日切时要比较日期


	CBF_Stack<char *>  m_pQueue;  //队列 第一个字节表示是dreb还是bpc，0DREB 1DREBERR 2BPC 3BPCERR,后面就是S_LOG_BPCDATA结构和S_LOGDREBDATA

	CBF_Date_Time  m_pDate;
	CBF_BufferPool m_pMemPool;
	CBF_Timer      m_pTimer;
	CBF_SimpleLog  m_pLog;

	bool           m_bIsExit;     //线程是否退出
	bool           m_bIsCheckDate;//是否检查日切
	bool           m_bIsSetPara;  //是否调用setlogpara
	bool           m_bIsBigEndian; //是否网络序


	// 函数名: int DateChange
	// 编程  : 王明松 2014-11-18 15:27:12
	// 返回  : static  
	// 参数  : unsigned int eventid
	// 参数  : void *p
	// 描述  : 定时器事件
	static  int DateChange(unsigned int eventid,void *p);

private:
	void ClearQueueAndClose();

	bool WriteBpcErrLog(char *logdata);
	bool WriteBpcLog(char *logdata);
	bool WriteDrebErrLog(char *logdata);
	bool WriteDrebLog(char *logdata);

	// 函数名: OpenDrebLogFile
	// 编程  : 王明松 2014-11-18 15:26:50
	// 返回  : bool 
	// 描述  : 打开日志文件
	bool OpenDrebLogFile();
	bool OpenDrebErrLogFile();
	bool OpenBpcLogFile();
	bool OpenBpcErrLogFile();


	// 函数名: WriteLog
	// 编程  : 王明松 2014-11-18 15:27:01
	// 返回  : bool 
	// 参数  : char *logdata
	// 描述  : 写入日志
	bool WriteLog(char *logdata);

	// 函数名: InitThreadInstance
	// 编程  : 王明松 2012-4-9 12:00:54
	// 返回  : virtual bool 
	// 描述  : 线程初始化，线程启动时调用一次
	virtual bool InitThreadInstance();
	
	// 函数名: ExitThreadInstance
	// 编程  : 王明松 2012-4-9 12:01:05
	// 返回  : virtual void 
	// 描述  : 退出线程要做的工作,线程退出时调用一次
	virtual void ExitThreadInstance();
	
	// 函数名: Run
	// 编程  : 王明松 2012-4-9 12:01:23
	// 返回  : virtual int 
	// 描述  : 线程运行方法，若此方法返回，则线程自动终止
	virtual int Run(); 
};

#endif // !defined(AFX_BF_COMMDATALOG_H__9F3DE176_FE0E_4392_93D4_725085721538__INCLUDED_)
