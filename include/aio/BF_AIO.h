// BF_AIO.h: interface for the CBF_AIO class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BF_AIO_H__2C090C2E_4D96_4548_A785_0731D797AED0__INCLUDED_)
#define AFX_BF_AIO_H__2C090C2E_4D96_4548_A785_0731D797AED0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BF_AIO_Base.h"
#include "BF_AIO_Pool.h"
#include "BF_AIO_Thread.h"
#include "BF_AIO_WorkBase.h"
#include "BF_SocketTcp.h"
#include "BF_Date_Time.h"
#include "BF_AIO_Resource.h"
#include "BF_Timer.h"
#include "IErrLog.h"
#ifdef LINUX
#include <sys/epoll.h>
#endif

#ifdef _WINDOWS
#include "mswsock.h"
#pragma comment(lib, "Mswsock.lib") 
#pragma message("Automatically linking with Mswsock.lib")
#endif

#ifdef _WINDOWS
#ifdef _DEBUG
#pragma comment(lib, "bf_kerneld.lib") 
#pragma message("Automatically linking with  bf_kerneld.lib")
#else
#pragma comment(lib, "bf_kernel.lib") 
#pragma message("Automatically linking with  bf_kernel.lib")
#endif
#endif

#ifdef AIO_EXPORTS
#define AIO_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define AIO_EXPORT __declspec(dllimport)
#else
#define AIO_EXPORT 
#endif
#endif



class AIO_EXPORT CBF_AIO : public CBF_AIO_Base  
{
public:
	

	// 函数名: GetErrMsg
	// 编程  : 王明松 2012-5-22 10:21:06
	// 返回  : std::string 
	// 描述  : 取得出错信息
	std::string GetErrMsg();
	

	// 函数名: GetSocketErrNo
	// 编程  : 王明松 2012-5-24 15:03:40
	// 返回  : int  
	// 描述  : 取得iocp发送出错时的ERRNO
	virtual int  GetSocketErrNo();

	// 函数名: Send
	// 编程  : 王明松 2012-5-21 16:52:24
	// 返回  : bool 
	// 参数  : PSOCKET_SEND_DATA senddata
	// 参数  : bool isimmediate=false	 是否立即发送
	// 参数  : bool islock=true	  是否加锁  
	// 描述  : 发送数据
	virtual bool Send(SOCKET_SEND_DATA senddata,bool isimmediate=false,bool islock=true);
	
	// 函数名: AddAio
	// 编程  : 王明松 2012-5-21 10:03:52
	// 返回  : bool 
	// 参数  : PSOCKET_POOL_DATA info
	// 描述  : 将连接信息加入到AIO中
    virtual bool AddAio(PSOCKET_POOL_DATA info);

	// 函数名: Close
	// 编程  : 王明松 2012-5-22 11:16:09
	// 返回  : virtual bool 
	// 参数  : PSOCKET_POOL_DATA info
	// 描述  : 应用主动关闭连接
	virtual bool Close(PSOCKET_POOL_DATA info);
	
	// 函数名: Connect
	// 编程  : 王明松 2012-5-21 11:56:45
	// 返回  : bool 
	// 参数  : char *ip  连接的IP
	// 参数  : short port  端口
	// 参数  : unsigned  short  sockettype   连接类型
	// 参数  : unsigned int destindex        若是vc的话要指定关联的连接
	// 参数  : unsigned int &index           AIO返回此连接索引
	// 描述  : 应用要进行连接，OnConnectedBack返回是否成功连上
	virtual bool Connect(char *ip,short port,unsigned  short  sockettype, unsigned int destindex ,unsigned int &index);


	// 函数名: ScanPool
	// 编程  : 王明松 2012-5-21 11:52:10
	// 返回  : void 
	// 描述  : 检查连接，是否需要发送Ping及超时断开
	void ScanPool();

	// 函数名: Stop
	// 编程  : 王明松 2012-5-21 11:48:32
	// 返回  : bool 
	// 描述  : 停止IO模型
	bool Stop();

	// 函数名: Start
	// 编程  : 王明松 2012-5-21 11:48:17
	// 返回  : bool 
	// 描述  : 启动IO模型
	bool Start();

	
	
	CBF_AIO();
	virtual ~CBF_AIO();

	
public:
	
	// 函数名: SetParameter
	// 编程  : 王明松 2012-5-21 11:38:06
	// 返回  : void 
	// 参数  : unsigned int iothreadnum   IO处理的线程数
	// 参数  : unsigned int maxfd         IO处理最大的连接数
	// 参数  : CBF_AIO_WorkBase *workcallback    IO处理完成后的回调类
	// 描述  : 设置AIO运行参数
	void SetParameter(CBF_AIO_WorkBase *workcallback,CBF_AIO_Resource *res);
	
	

protected:
	static int OnTimer(unsigned int eventid,void *p);

	char m_sErrMsg[256];
	int  m_nSocketErrno;

	CIErrlog          *m_pLog;
	CBF_SocketTcp     m_pSocketTcp;
	CBF_Date_Time     m_pDate;
	CBF_AIO_Pool      m_pAioPool;
	
	CBF_Timer         m_pTimer;

	CBF_AIO_WorkBase  *m_pAioWork;
	CBF_AIO_Resource  *m_pAioRes;
	CBF_AIO_Thread    m_pAioThread[20];//IO处理线程数，最多20个

};

#endif // !defined(AFX_BF_AIO_H__2C090C2E_4D96_4548_A785_0731D797AED0__INCLUDED_)
