// BF_AIO_Resource.h: interface for the CBF_AIO_Resource class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BF_AIO_RESOURCE_H__DAB2456F_EB32_4EBC_8D6A_FA6A6EE9F1EB__INCLUDED_)
#define AFX_BF_AIO_RESOURCE_H__DAB2456F_EB32_4EBC_8D6A_FA6A6EE9F1EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "BF_Socket.h"
#include "BF_Mutex.h"
#include "BF_AIO_Head.h"
#include "typedefine.h"
#include "BF_Date_Time.h"
#include "BF_LogClient.h"

//0-select模型  1-epoll模型(LINUX)   2-iocp模型(WINDOWS)
#define  MODEIOCP    2     //iocp模型
#define  MODEEPOLL   1     //epoll模型
#define  MODESELECT  0     //select模型

#ifdef AIO_EXPORTS
#define AIO_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define AIO_EXPORT __declspec(dllimport)
#else
#define AIO_EXPORT 
#endif
#endif

class AIO_EXPORT CBF_AIO_Resource  
{
public:

	// 函数名: ReSetPara
	// 编程  : 王明松 2013-6-5 22:09:26
	// 返回  : void 
	// 描述  : 初始化参数的初始值,同构造函数
	void ReSetPara();

	// 函数名: SetIoThreadNum
	// 编程  : 王明松 2013-6-4 19:49:54
	// 返回  : void 
	// 参数  : unsigned iothreadnum
	// 描述  : 第二步，设置IO线程数  epoll只要一个线程  iocp为处理器个数，select根据需要设置
	void SetIoThreadNum(unsigned iothreadnum);

	// 函数名: SetRunMode
	// 编程  : 王明松 2013-6-4 19:47:09
	// 返回  : bool 
	// 参数  : unsigned int mode
	// 描述  : 第一步，设置运行模式 
	bool SetRunMode(unsigned int mode);

	CBF_AIO_Resource();
	virtual ~CBF_AIO_Resource();

	unsigned   int    g_nIoThreadNum;//IO线程数
	unsigned   int    g_nMaxConnectNum;//最大连接数
	bool              g_bIsExit;  //是否退出
	unsigned   int    g_nRunMode;//运行模式   0-select模型  1-epoll模型(LINUX)   2-iocp模型(WINDOWS)
	unsigned   int    g_nPort;//IO模型的侦听端口
#if defined(_WINDOWS)
	HANDLE            g_hIocp;  //IOCP的HANDLE
#endif
#if defined(LINUX)
	int               g_nEpoll;  //EPOlL
#endif
	unsigned  int     g_nListenIndex;
	UINT64_           g_nRecvByteLen;  //收到的数据长度
	UINT64_           g_nSendByteLen;  //发送的数据长度
    UINT64_           g_nStartTime;    //开始运行时间 毫秒
	int               g_nIocpAcceptNum;//iocp一次发AcceptEx的数目
	char              g_sCurPath[300];
	char              g_sModulePath[300];
	char              g_sModuleName[300];

	int               g_nQuoteQueueDeleteTime;      //行情队列过期时间 
	int               g_nQuoteQueueNum;//保留行情数据

	
	  
	int               g_nSendOnceNum;//EPOLL一次发送条数

	int g_nHeartRun;//心跳时间,当发现连接超过此时间未用时，主动发送心跳
	int g_nDisconnectTime;//未使用断开时间，如果一个连接在此时间内一直没有使用，则将此连接断开。单位秒默认600秒即10分钟
	int g_nDispatchTime;//分配最大时间


	int               g_nLoglevel;//日志级别 默认为5
	std::string       g_sLogFileName;//日志文件名
	std::string       g_sLogFilePath;//日志文件目录
	int               g_nMaxLogSize;//日志大小，<1024不限制
	CBF_LogClient     g_pLog;

	// 函数名: Init
	// 编程  : 王明松 2013-6-4 19:18:47
	// 返回  : virtual bool 
	// 参数  : const char *confile
	// 描述  : 需要实现的初始化类
	virtual bool Init(const char *confile)=0;

};

#endif // !defined(AFX_BF_AIO_RESOURCE_H__DAB2456F_EB32_4EBC_8D6A_FA6A6EE9F1EB__INCLUDED_)
