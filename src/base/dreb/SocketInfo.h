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
#include "DrebPubDefine.h"
#include "BF_Mutex.h"
#include "IErrLog.h"
#include "BF_Tools.h"
#include "BF_BufferPool.h"
#include "DrebStatInfo.h"
#include "SendQueue.h"
#include "DrebEndian.h"
#include "GlobalVar.h"

// #define SOCK_UNKNOW   1       //未知
// #define SOCK_CLI      2       //客户端
// #define SOCK_DREB     3       //其它数据总线到本数据总线的连接
// #define SOCK_SRV      4       //注册的服务端
// #define SOCK_CENTER   5       //和其它数据总线节点的连接
// #define SOCK_SLOT	 6       //注册的通讯槽 
// #define SOCK_LISTEN	 7       //侦听端口
 
typedef struct 
{
	char    sAddress[33]; //数据总线IP
	unsigned int nPort;   //数据总线端口
	unsigned short nNodeId;              //数据总线编号
	char           cNodePrivateId;       //数据总线私有序号
	bool           bNeedConnect;           //需要连接
	int            nbandwidth;     //带宽
}DREB_CENTER; //连接的数据总线参数

typedef struct 
{
	unsigned short nNodeId;              //数据总线编号
	char           cNodePrivateId;       //数据总线私有序号
	int            nbandwidth;     //带宽
}DREB_CONN; //连接过来的数据总线参数

typedef struct 
{
	unsigned int nPort;   //侦听端口
	
}DREB_LISTEN; //侦听参数

typedef struct 
{
	unsigned int   nSvrMainId;           //注册在数据总线上的主服务功能号   只有服务调用时才用填写
	char           cSvrPrivateId;        //注册在数据总线上的私有序号 0表示公共 >0为每个服务的序号，s_nSvrMainId+s_cSvrPrivateId 即为私有功能号   只有服务调用时才用填写
}DREB_SRV;

#define  WLOG_RCV   0
#define  WLOG_SND   1
#define  WLOG_ERR   2


class CSocketInfo  
{
public:

	// 函数名: GetDrebCmdType
	// 编程  : 王明松 2013-5-13 15:01:35
	// 返回  : std::string 
	// 参数  : int cmdtype
	// 描述  : 取得CMD命令字
	std::string GetDrebCmdType(int cmdtype);

	// 函数名: GetSockType
	// 编程  : 王明松 2013-5-13 14:18:01
	// 返回  : std::string 
	// 参数  : int ntype
	// 描述  : 取socket连接类型
	std::string GetSockType(int ntype);

	void ResetSInfo();

	// 函数名: NeedRead
	// 编程  : 王明松 2010-7-22 9:28:09
	// 返回  : bool 
	// 描述  : 连接是否需要读
	bool NeedRead();

	// 函数名: NeedConnect
	// 编程  : 王明松 2010-7-22 9:24:20
	// 返回  : bool 
	// 描述  : 是否需要主动进行连接，只有连接类型为SOCK_TODREB时才需要连接
	bool NeedConnect();

	// 函数名: NeedSend
	// 编程  : 王明松 2010-7-22 9:23:48
	// 返回  : bool 
	// 描述  : 是否需要发送数据，当队列有数据时才需要发送
	bool NeedSend();

	/// 函数名: SetConnectDreb
	/// 编程  : 王明松 2009-10-26 16:27:48
	/// 返回  : void 
	/// 参数  : SOCKET_HANDLE sock
	/// 参数  : int conntime
	/// 参数  : CSendQueue *sque
	/// 描述  : 设置连接为主动连接数据总线
	void SetConnectDreb(SOCKET_HANDLE sock,int conntime,CSendQueue *sque);

	/// 函数名: SetAccept
	/// 编程  : 王明松 2009-7-14 13:42:50
	/// 返回  : void 
	/// 参数  : SOCKET_HANDLE sock
	/// 参数  : const char *address
	/// 描述  : 设置连接为正在连接
	void SetAccept(SOCKET_HANDLE sock,const char *address);

	/// 函数名: SetSvr
	/// 编程  : 王明松 2009-7-13 15:44:42
	/// 返回  : void 
	/// 参数  : unsigned int   nSvrMainId
	/// 参数  : char   cSvrPrivateId
	/// 参数  : CSendQueue *sque
	/// 描述  : 设置连接为注册的服务连接
	void SetSvr(unsigned int   nSvrMainId,char   cSvrPrivateId,CSendQueue *sque);

	/// 函数名: SetDreb
	/// 编程  : 王明松 2009-7-13 15:44:23
	/// 返回  : void 
	/// 参数  : unsigned short NodeId
	/// 参数  : char  cNodePrivateId
	/// 参数  : int bdrate 
	/// 参数  : CSendQueue *sque
	/// 描述  : 设置连接为其它数据总线和本数据总线的连接
	void SetDreb(unsigned short NodeId,char  cNodePrivateId,int bdrate,CSendQueue *sque);


	/// 函数名: SetClient
	/// 编程  : 王明松 2009-7-13 15:44:09
	/// 返回  : void 
	/// 描述  : 设置连接为客户端
	void SetClient();


	/// 函数名: SetToDreb
	/// 编程  : 王明松 2009-7-13 15:43:54
	/// 返回  : void 
	/// 参数  : const char *ip
	/// 参数  : int port
	/// 描述  : 设置连接为和数据总线的连接
	void SetToDreb(const char *ip,int port,int brate);

	



	/// 函数名: GetRecvData
	/// 编程  : 王明松 2009-7-3 15:53:27
	/// 返回  : int 数据长度 小于0连接关闭 等于0数据不完整，大于0接收到的数据长度
	/// 参数  : S_DREB_RSMSG *msg
	/// 描述  : 获取接收到的数据
	int GetRecvData(S_DREB_RSMSG *msg);

	
	/// 函数名: SendMsg
	/// 编程  : 王明松 2009-7-3 15:35:48
	/// 返回  : int 
	/// 参数  : S_DREB_RSMSG *msg
	/// 描述  : 发送消息，不成功放入队列
	int SendMsg(S_DREB_RSMSG *msg,bool sendimmediate=false);

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

	bool ListenIpc(int port,bool blockflag=false,int queuenum=5);
	

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
	void OnClose(const char *msg,const char *filename,int fileline);

	/// 函数名: IsCanSelect
	/// 编程  : 王明松 2009-7-3 10:14:49
	/// 返回  : bool 
	/// 描述  : 此连接是否可以select操作
	bool IsCanSelect();

	// 函数名: SetGlobalVar
	// 编程  : 王明松 2009-7-3 16:18:43
	// 返回  : void 
	// 参数  : CGlobalVar *pVar 
	// 参数  : CPoolBuffer *pool 
	// 参数  : int deltime 数据在队列里超过此时间则删除
	// 描述  : 设置参数并初始化
	void SetGlobalVar(CGlobalVar *pVar,CBF_BufferPool *pool,int deltime);

	CSocketInfo();
	virtual ~CSocketInfo();


	DREB_CENTER    m_s_center;  //连接数据总线参数
	DREB_CONN      m_s_conn;    //连接过来本dreb的数据总线参数
	DREB_LISTEN    m_s_listen;  //侦听参数
	DREB_SRV       m_s_srv;     //注册的服务参数
//心跳参数
	int            m_nBeatheartcount;        //发送心跳次数
	
	char           m_sAddress[33]; //客户端IP,连接过来的IP
//socket公共参数
	SOCKET_HANDLE  m_sock;                   //socket id
	int            m_nType;                  //socket type SOCK_UNKNOW SOCK_CLI SOCK_DREB SOCK_SRV SOCK_TODREB
	bool           m_bChecked;               //是否检查认证过，即确认过连接类型
	
	bool           m_bNeedread;              //是否需要读数据

    char           m_cCreateDate[9];         //连接建立日期
	int            m_nConntime;              //连接建立时间

	int            m_nReadtime;              //上次读数据时间
	int            m_nSendtime;              //上次发送数据时间 

	char           m_cCloseDate[9];         //连接关闭日期
	int            m_nCloseTime;             //关闭时间
	int            m_nPingTime;              //onping时间
	
	unsigned int     m_nSend_packs;          //自连接建立起发送的数据包
	unsigned int     m_nRcv_packs;           //自连接建立起接收的数据包
	unsigned long    m_nSendBytes;           //自连接建立起发送的字节数
	unsigned long    m_nRcvBytes;            //自连接建立起接收的字节数

//发送接收数据	
	char m_sRcvBuffer[DREBBUFSIZE];            //接收数据缓冲   
	unsigned int  m_nRcvBufLen;                //接收的数据长度  可多次接收一个数据包

	int           m_nQueueTime;                //队列里数据超过此时间则删除不发送，单位秒

	int  m_index;                              //此连接在整个连接池中的序号

	
	CSendQueue       *m_pSendQueue;//发送队列

protected :
	int           m_sendNum;                   //连接发送次数

	void ResetInfo();
	void ClearQueue();
	/// 函数名: PutSendMsg
	/// 编程  : 王明松 2009-7-3 15:35:48
	/// 返回  : int 
	/// 参数  : S_DREB_RSMSG *msg
	/// 描述  : 放入一个要发送的数据至队列并置需发送标志为true
	int PutSendMsg(S_DREB_RSMSG *msg);

	void CheckFirstData();

	char   m_errMsg[256];

	char m_sSendBuffer[DREBBUFSIZE];            //临时数据压缩区   

	CBF_SocketTcp  m_tcpSocket;
	CIErrlog  *m_log;	
	CBF_Date_Time  m_datetime;
	CBF_BufferPool *m_pMemPool;
	CDrebStatInfo *m_pStatInfo;
	CGlobalVar    *m_pRes;
	
	int     m_nLogLevel;
	string  m_sLogFile;
	CBF_Mutex  m_mutex;

	CDrebEndian  m_pDrebEndian;

private:


};



#endif // !defined(AFX_SOCKETINFO_H__6195E011_BD18_493A_BAA5_561125709E5C__INCLUDED_)
