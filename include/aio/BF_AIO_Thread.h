// BF_AIO_Thread.h: interface for the CBF_AIO_Thread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BF_AIO_THREAD_H__97E97381_77A3_4915_B4A2_7E59B6B9F252__INCLUDED_)
#define AFX_BF_AIO_THREAD_H__97E97381_77A3_4915_B4A2_7E59B6B9F252__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "BF_AIO_Pool.h"
#include "BF_Thread.h"
#include "BF_AIO_WorkBase.h"
#include "BF_AIO_Resource.h"
#include "MdbBase.h"
#include "BF_SocketTcp.h"
#include "IErrLog.h"
#include "BF_Timer.h"

#ifdef LINUX
#include <sys/epoll.h>
#endif

#ifdef _WINDOWS
#include "mswsock.h"
#pragma comment(lib, "Mswsock.lib") 
#pragma message("Automatically linking with Mswsock.lib")
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

class AIO_EXPORT CBF_AIO_Thread  : public CBF_Thread
{
public:
	void SetScan();
	static int OnTimer(unsigned int eventid,void *p);
	void ScanPool();

	// 函数名: ListIndex
	// 编程  : 王明松 2013-6-20 14:04:52
	// 返回  : void 
	// 描述  : 显示本线程管理的所有连接
	void ListIndex();

	// 函数名: AddSocketIndex
	// 编程  : 王明松 2013-6-20 14:04:57
	// 返回  : void 
	// 参数  : int index
	// 描述  : 加入本线程要管理的连接索引
	void AddSocketIndex(int index);

	// 函数名: SelectSend
	// 编程  : 王明松 2012-5-24 10:35:31
	// 返回  : void 
	// 参数  : PSOCKET_POOL_DATA info
	// 描述  : 发送数据，直到发完或连接不可写
	void SelectSend(PSOCKET_POOL_DATA info);

	// 函数名: EpollSend
	// 编程  : 王明松 2012-5-24 10:34:11
	// 返回  : void 
	// 参数  : PSOCKET_POOL_DATA info
	// 描述  : 发送数据，直到发完或连接不可写
	void EpollSend(PSOCKET_POOL_DATA info);

	// 函数名: EpollRecv
	// 编程  : 王明松 2012-5-24 10:34:16
	// 返回  : void 
	// 参数  : PSOCKET_POOL_DATA info
	// 描述  : EPOLL接收数据，直到没有数据
	void EpollRecv(PSOCKET_POOL_DATA info);

	// 函数名: SelectOnWriteEvent
	// 编程  : 王明松 2012-5-22 16:28:48
	// 返回  : void 
	// 参数  : PSOCKET_POOL_DATA info
	// 描述  : 响应select模型写事件
	void SelectOnWriteEvent(PSOCKET_POOL_DATA info);

	// 函数名: SelectOnReadEvent
	// 编程  : 王明松 2012-5-22 16:28:44
	// 返回  : void 
	// 参数  : PSOCKET_POOL_DATA info
	// 描述  : 响应select模型读事件
	void SelectOnReadEvent(PSOCKET_POOL_DATA info);

	// 函数名: SelectGetMaxfdAdd
	// 编程  : 王明松 2012-5-22 16:26:27
	// 返回  : int <1没有连接要片处理
	// 参数  : int maxfd    最大的socket
	// 描述  : select模型中，从连接池里取本线程要处理的连接加入到处理中，并得到最大的socket
	int SelectGetMaxfdAdd(int &maxfd);

	// 函数名: RunEpoll
	// 编程  : 王明松 2012-5-22 16:28:05
	// 返回  : void 
	// 描述  : 运行EPOLL模型
	void RunEpoll();

	// 函数名: RunIocp
	// 编程  : 王明松 2012-5-22 16:27:53
	// 返回  : void 
	// 描述  : 运行iocp模型
	void RunIocp();

	// 函数名: RunSelect
	// 编程  : 王明松 2012-5-22 16:27:32
	// 返回  : void 
	// 描述  : 运行select模型
	void RunSelect();


	// 函数名: SetParameter
	// 编程  : 王明松 2012-5-22 15:07:21
	// 返回  : void 
	// 参数  : CBF_AIO_Pool *pool
	// 参数  : CBF_AIO_WorkBase *work
	// 参数  : CBF_AIO_Resource  *res
	// 参数  : unsigned int index
	// 描述  : 设置IO线程参数
	void SetParameter(CBF_AIO_Pool *pool ,CBF_AIO_WorkBase *work,CBF_AIO_Resource  *res,unsigned int index,CIErrlog     *log);

	CBF_Timer         m_pTimer;

	CBF_AIO_Thread();
	virtual ~CBF_AIO_Thread();

	

private:
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

	

	bool              m_bIsScan;

	// 函数名: OnConnectBack
	// 编程  : 王明松 2013-6-5 17:45:47
	// 返回  : void 
	// 参数  : PSOCKET_POOL_DATA data
	// 参数  : bool bret  true连接成功  false失败
	// 描述  : 主动对外连接的回报，交由业务处理
	void OnConnectBack(PSOCKET_POOL_DATA data,bool bret);

	// 函数名: OnAccept
	// 编程  : 王明松 2013-6-5 17:45:12
	// 返回  : void 
	// 参数  : PSOCKET_POOL_DATA data
	// 描述  : 收到一个连接，交给业务进行处理，如状态，连接类型等
	void OnAccept(PSOCKET_POOL_DATA data);

// 	// 函数名: OnSendBack
// 	// 编程  : 王明松 2013-6-5 17:33:38
// 	// 返回  : void 
// 	// 参数  : PSOCKET_POOL_DATA data
// 	// 描述  : 当连接可写时，将队列的数据发送后回调给业务模块来处理
// 	void OnSendBack(SOCKET_SEND_DATA data,bool bret=true);

	// 函数名: OnPing
	// 编程  : 王明松 2013-6-5 17:33:34
	// 返回  : void 
	// 参数  : PSOCKET_POOL_DATA data
	// 描述  : 
	void OnPing(PSOCKET_POOL_DATA data);

	// 函数名: OnRecvBack
	// 编程  : 王明松 2013-6-5 17:31:43
	// 返回  : void 
	// 参数  : PSOCKET_POOL_DATA data
	// 描述  : 收到数据了，回调给业务模块来处理
	void OnRecvBack(PSOCKET_POOL_DATA data);

	// 函数名: OnClose
	// 编程  : 王明松 2013-6-5 17:31:23
	// 返回  : void 
	// 参数  : PSOCKET_POOL_DATA info
	// 参数  : const char *filename
	// 参数  : int fileline
	// 参数  : const char *msg
	// 描述  : 关闭连接
	void OnClose(PSOCKET_POOL_DATA info,const char *filename,int fileline,const char *msg);

	// 函数名: SelectSend
	// 编程  : 王明松 2013-6-5 14:46:22
	// 返回  : int  返回发送的数据  -1发送出错
	// 参数  : SOCKSENDQUEUE::iterator prq
	// 描述  : 发送数据
	int SelectSend(SOCKSENDQUEUE::iterator prq);

	CIErrlog     *m_pLog;
	CBF_AIO_WorkBase  *m_pAioWork;
	CBF_AIO_Pool      *m_pAioPool;
	CBF_AIO_Resource  *m_pAioRes;
	unsigned   int    m_nIndex;//本线程的索引，标识是第几个IO线程
	CBF_SocketTcp     m_pSocketTcp;

	fd_set m_rset;
	fd_set m_wset;
#if defined(_WINDOWS)
	fd_set m_eset;
#endif

	CInt   m_lSocketList;  //本次加入到select的列表

	CInt   m_lIndexList;   //本线程要处理的连接列表。
	char       m_sErrMsg[256];
};

#endif // !defined(AFX_BF_AIO_THREAD_H__97E97381_77A3_4915_B4A2_7E59B6B9F252__INCLUDED_)
