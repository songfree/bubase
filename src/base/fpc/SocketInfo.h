// SocketInfo.h: interface for the CSocketInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOCKETINFO_H__6195E011_BD18_493A_BAA5_561125709E5C__INCLUDED_)
#define AFX_SOCKETINFO_H__6195E011_BD18_493A_BAA5_561125709E5C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "BF_SocketTcp.h"
#include "BF_Mutex.h"
#include "BF_SimpleLog.h"
#include "BF_BufferPool.h"
#include "SendQueue.h"
#include "BfDefine.h"
#include "GResource.h"


class  CSocketInfo  
{
public:
	int SendMsg(S_BF_RSMSG_FPC msg);
	void Reverse(unsigned char *buf, int len);

	void Init(CGResource *res,CBF_BufferPool *pool);


	// 函数名: NeedSend
	// 编程  : 王明松 2010-3-12 13:55:22
	// 返回  : bool 
	// 描述  : 是否有数据要发送
	bool NeedSend();

	void SetAccept(SOCKET_HANDLE sock,const char *address);

	/// 函数名: SetClient
	/// 编程  : 王明松 2009-7-13 15:44:09
	/// 返回  : void 
	/// 描述  : 设置连接为客户端
	void SetClient();

	// 函数名: SetBuClient
	// 编程  : 王明松 2010-3-18 11:53:23
	// 返回  : void 
	// 描述  : 设置连接BU连接
	void SetBuClient();

	/// 函数名: GetRecvData
	/// 编程  : 王明松 2009-7-3 15:53:27
	/// 返回  : int 数据长度 小于0连接关闭 等于0数据不完整，大于0接收到的数据长度
	/// 参数  : S_BF_RSMSG *msg
	/// 描述  : 获取接收到的数据
	int GetRecvData(S_BF_RSMSG_FPC *msg);


	

	/// 函数名: OnSend
	/// 编程  : 王明松 2009-7-3 15:35:30
	/// 返回  : int 小于0发送失败 >0发送的数据长度 0无数据发送
	/// 描述  : 发送数据
	int OnSend();

	/// 函数名: OnRecv
	/// 编程  : 王明松 2009-7-3 15:35:23
	/// 返回  : int 若返回小于0则表示连接关闭，小于通讯头则还要继续收，大于通讯头可以取数据进行处理了
	/// 描述  : 接收数据
	int OnRecv();
	

	/// 函数名: Listen
	/// 编程  : 王明松 2009-7-3 12:48:10
	/// 返回  : bool 
	/// 参数  : int port
	/// 参数  : bool blockflag=false  阻塞方式 true为阻塞
	/// 参数  : int queuenum=5
	/// 描述  : 开始侦听，并置内部参数
	bool Listen(int port,bool blockflag=false,int queuenum=5);

	/// 函数名: IsFree
	/// 编程  : 王明松 2009-7-3 10:18:17
	/// 返回  : bool 
	/// 描述  : accept时取此连接是否可以accept
	bool IsFree();

	/// 函数名: OnShutdown
	/// 编程  : 王明松 2009-7-3 10:17:25
	/// 返回  : void 
	/// 描述  : 强行关闭连接，置各项参数为初始值
	void OnShutdown();

	/// 函数名: OnClose
	/// 编程  : 王明松 2009-7-3 10:16:06
	/// 返回  : void 
	/// 参数  : const char *msg
	/// 描述  : 关闭连接，根据连接类型置各项参数
	void OnClose(const char *msg);

	/// 函数名: IsCanSelect
	/// 编程  : 王明松 2009-7-3 10:14:49
	/// 返回  : bool 
	/// 描述  : 此连接是否可以select操作
	bool IsCanSelect();

	CSocketInfo();
	virtual ~CSocketInfo();



//心跳参数
	int            m_nBeatheartcount;        //发送心跳次数
	bool           m_bIsBeat;                //本通讯平台是否要发心跳

	char           m_sAddress[33]; //客户端IP,连接过来的IP
//socket公共参数
	SOCKET_HANDLE  m_sock;                   //socket id
	int            m_nType;                  //SOCKET类型
	int            m_nPort;                  //侦听端口

	bool           m_bNeedread;              //是否需要读数据

    char           m_cCreateDate[9];         //连接建立日期
	int            m_nConntime;              //连接建立时间

	int            m_nReadtime;              //上次读数据时间
	int            m_nSendtime;              //上次发送数据时间 

	char           m_cCloseDate[9];         //连接关闭日期
	int            m_nCloseTime;             //关闭时间
	int            m_nPingTime;              //onping时间
	
	int            m_nPid;                   //连接上来的fpu的进程ID


//发送接收数据	
	char *m_sRcvBuffer;            //接收数据缓冲   
	unsigned int  m_nRcvBufLen;                //接收的数据长度  可多次接收一个数据包

	char *m_sSendBuffer;            //发送数据缓冲   
	unsigned int  m_nSendBufLen;                //发送的数据长度  可多次接收一个数据包

	int           m_nQueueTime;                //队列里数据超过此时间则删除不发送，单位秒

	int  m_index;

	
	CSendQueue       m_pSendQueue;//发送队列
	bool      m_bIsBuGetFlag;//BU连接正常，是否可分发请求

protected :
	int           m_sendNum;                   //连接发送次数



	/// 函数名: PutSendMsg
	/// 编程  : 王明松 2009-7-3 15:35:48
	/// 返回  : int 
	/// 参数  : S_DRTP_RSMSG msg
	/// 描述  : 放入一个要发送的数据至队列并置需发送标志为true
	int PutSendMsg(S_BF_RSMSG_FPC msg);


	void ResetInfo();
	void ClearQueue();
	

	char   m_errMsg[256];

	CBF_SocketTcp m_tcpSocket;
	CBF_Tools  m_pTool;
	CIErrlog *m_log;	
	CBF_Date_Time m_publog;

	CBF_BufferPool *m_pMemPool;
	CGResource  *m_gRes;

	
	int     m_nLogLevel;
	string  m_sLogFile;
	CBF_Mutex  m_mutex;
 	
};



#endif // !defined(AFX_SOCKETINFO_H__6195E011_BD18_493A_BAA5_561125709E5C__INCLUDED_)
