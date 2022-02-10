// BF_LogClient.h: interface for the CBF_LogClient class.
//
//////////////////////////////////////////////////////////////////////

/***********************************************************************************
  编码日志记录
  编码者: 王明松
  编码日期 2012-5-11 14:09:58
  版本: V1.00 
  说明: 日志，根据设置的参数来决定是连接日志服务器还是直接写日志文件，通过线程实现。
        支持多线程调用
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



#define    LOG_QUEUEMAXNUM    1000   //日志队列里的最大日志数  
#define    LOG_BUFFERSIZE     65534   //日志缓冲大小
    
#ifdef __cplusplus
extern "C" {
#endif
	
	//字节对齐
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
	char            loglevel;     //日志级别
	char            filename[41]; //代码文件名
	unsigned short  fileline;     //文件行
	long            logus;        //调用logmp时间
	char            wtype;        //0-LOGMP  1-LOGBIN
	int             threadid;     //线程id
	unsigned short  datalen;
	char            data[1];
}__PACKED__ S_LOG_DATA;

#define  S_LOG_HEADLEN   sizeof(S_LOG_DATA)-1    //日志头长度



//取消字节对齐
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
	

	// 函数名: SetLogLevel
	// 编程  : 王明松 2015-11-12 11:12:05
	// 返回  : void 
	// 参数  : int loglevel
	// 描述  : 设置日志级别
	void SetLogLevel(int loglevel);

	// 函数名: GetLogLevel
	// 编程  : 王明松 2015-11-12 11:11:48
	// 返回  : int 
	// 描述  : 取日志级别
	int GetLogLevel();


	// 函数名: IsStopedThread
	// 编程  : 王明松 2014-12-15 10:25:29
	// 返回  : bool 
	// 描述  : 线程是否停止
	bool IsStopedThread();

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


	CBF_LogClient();
	virtual ~CBF_LogClient();

	// 函数名: isWrite
	// 编程  : 王明松 2009-6-29 14:28:34
	// 返回  : int 
	// 参数  : int level 日志级别 
	// 描述  : 是否要写入日志
	virtual bool isWrite(int level);
	
	// 函数名: SetLogPara
	// 编程  : 王明松 2009-6-29 14:28:34
	// 返回  : int 
	// 参数  : int level 日志级别 
	// 参数  : const char *logfilepath  日志文件路径  若为连日志服务器则为ip@port@mode
	// 参数  : const char *logfilename  日志文件名    若为连日志服务器则为类型
	// 参数  : int id=-1
	// 描述  : 设置日志参数
	virtual void SetLogPara(int level,const char *logfilepath,const char *logfilename,int id=-1);
	
	/// 函数名: GetLogPara
	/// 编程  : 王明松 2012-4-25 11:21:05
	/// 返回  : virtual void 
	/// 参数  : int &level 日志级别
	/// 参数  : std::string &logfilepath 日志文件路径
	/// 参数  : std::string &logfilename 日志文件名
	/// 参数  : int &id 进程或线程ID
	/// 描述  : 取日志参数
	virtual void GetLogPara(int &level,std::string &logfilepath, std::string &logfilename,int &id);
	
	/// 函数名: LogMp
	/// 编程  : 王明松 2009-5-16 17:28:24
	/// 返回  : int 
	/// 参数  : int level
	/// 参数  : const char *filename
	/// 参数  : int line
	/// 参数  : const char *fmt
	/// 参数  : ...
	/// 描述  : 写入日志  要确保字串不能超过LOG_BUFFERSZIE
	virtual int LogMp(int level, const char *filename, int line,const char *fmt,...);
	
	/// 函数名: LogBin
	/// 编程  : 王明松 2007-7-10 11:27:52
	/// 返回  : int -1失败 0成功
	/// 参数  : int level  日志级别
	/// 参数  : const char *filename  源文件名
	/// 参数  : int line              源文件行
	/// 参数  : const char *title  标题
	/// 参数  : char *msg   内容
	/// 参数  : int msglen  长度注意  长度要小于8192-title长度-8，否则多余的不写入
	/// 描述  : 写入二进制数据至日志文件,'\0'用*代替  若msglen超过LOG_BUFFERSZIE大小则截去
	virtual int LogBin(int level, const char *filename, int line,const char *title,char *msg,int msglen);



	// 函数名: SetMaxLogSize
	// 编程  : 王明松 2014-11-12 14:04:26
	// 返回  : virtual void 
	// 参数  : int size   大小
	// 描述  : 设置日志文件大小，小于1024不限制  此方法需在SetLogPara前调用
	virtual void SetMaxLogSize(int size);
	
protected:


	// 函数名: RunServer
	// 编程  : 王明松 2014-11-13 9:20:04
	// 返回  : int 
	// 描述  : 日志服务器方式线程方法
	int RunServer();

	// 函数名: RunLocal
	// 编程  : 王明松 2014-11-13 9:20:08
	// 返回  : int 
	// 描述  : 本地日志方式线程方法
	int RunLocal();

	// 函数名: OpenLogFile
	// 编程  : 王明松 2012-5-14 14:52:47
	// 返回  : bool 
	// 描述  : 打开日志文件
	bool OpenLogFile();
	
	
	// 函数名: WriteLog
	// 编程  : 王明松 2012-5-11 17:09:54
	// 返回  : bool 
	// 参数  : char *logdata
	// 描述  : 写本地日志文件
	bool WriteLog(char *logdata);

	/// 函数名: GetName
	/// 编程  : 王明松 2007-7-10 10:52:10
	/// 返回  : void 
	/// 参数  : char *filename  
	/// 描述  : 获取文件名称，不带路径的文件名
	int GetName(char *filename,char **pfn);

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
	
protected:
	void OnClose();

	// 函数名: OnSend
	// 编程  : 王明松 2014-11-13 10:48:48
	// 返回  : void 
	// 描述  : 发送给日志服务器日志信息(从队列里取)
	void OnSend();

	// 函数名: OnRecv
	// 编程  : 王明松 2014-11-13 10:48:27
	// 返回  : void 
	// 描述  : 接收日志服务器数据并处理，主要是心跳
	void OnRecv();

	// 函数名: OnPing
	// 编程  : 王明松 2014-11-13 9:56:32
	// 返回  : void 
	// 描述  : 定时检查日志服务器连接，超过一定时间无数据读写则发心跳，当发了心跳且超过一定时间没有读数据则断开
	void OnPing();

	// 函数名: OnEvent
	// 编程  : 王明松 2014-11-13 9:54:20
	// 返回  : void 
	// 描述  : 日志服务器连接有读或可写的事件发生
	void OnEvent();


	// 函数名: OnNoEvent
	// 编程  : 王明松 2014-11-13 9:54:17
	// 返回  : void 
	// 描述  : 日志服务器连接无读事件并且不可写(当连接不可写且日志队列里有大量数据时，要进行处理)
	void OnNoEvent();

	// 函数名: OnConnect
	// 编程  : 王明松 2014-11-13 9:54:06
	// 返回  : void 
	// 描述  : 连接日志服务器
	void OnConnect();

	
	
	static  int DateChange(unsigned int eventid,void *p);

	char           m_sLogFilePathName[500];//当前的日志文件

	int            m_nLogLevel; //日志级别
	int            m_nId;
	std::string    m_sLogPath;  //文件路径或远程服务端的IP端口模式
	std::string    m_sLogName;
	char           m_sIp[30];//远程日志服务ip
	unsigned int   m_nPort;//远程日志服务端口
	

    unsigned int   m_nMaxLogSize;//日志文件大小 0不限制一直写
	unsigned int   m_nCurLogSize;//当前日志大小
	
	std::string    m_sCurDate;//当前日期,本地写入时日切时要比较日期
	bool           m_bIsCheckDate;//是否检查日切
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

	unsigned int m_nConntime; //连接时间
	unsigned int m_nReadtime; //读到数据时间
	unsigned int m_nWriteTime;//写数据时间
	unsigned int m_nPingTime;//心跳时间
	bool         m_bIsPingTimer;//定时处理标志
	bool         m_bIsConnect;//是否在连接
	CBF_SimpleLog m_log;
	
	char         m_sRcvBuffer[8192];
	unsigned int m_nRcvLen;
	char         m_sSendBuffer[8192];
	unsigned int m_nSendLen;

//	CBF_Mutex    m_mutex;
private:
	
};

#endif // !defined(AFX_BF_LOGCLIENT_H__9EB53084_7DA6_41C6_A461_C8DBBAC75974__INCLUDED_)
