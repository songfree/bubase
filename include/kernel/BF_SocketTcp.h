// BF_SocketTcp.h: interface for the CBF_SocketTcp class.
//
//////////////////////////////////////////////////////////////////////

/***********************************************************************************
  编码日志记录
  编码者: 王明松
  编码日期 2012-4-17 14:07:33 重构
  版本: V1.00 
  说明: tcp通讯类  编译时要加入 -D_REENTRANT
 ***********************************************************************************/


#if !defined(AFX_BF_SOCKETTCP_H__99507094_1B43_494E_9D1C_573498387EC3__INCLUDED_)
#define AFX_BF_SOCKETTCP_H__99507094_1B43_494E_9D1C_573498387EC3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "BF_Socket.h"




#ifdef KERNEL_EXPORTS
#define KERNEL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNEL_EXPORT __declspec(dllimport)
#else
#define KERNEL_EXPORT 
#endif
#endif

#define  BF_SOCKET_CLIENT        0    //作为客户端    
#define  BF_SOCKET_SERVER        1    //作为服务端的侦听
#define  BF_SOCKET_ATTACHBIND    2    //作为服务端接受连接到的客户端，本类绑定，析构关闭socket
#define  BF_SOCKET_ATTACH        3    //作为服务端接受连接到的客户端，本类不绑定
#define  BF_SOCKET_ATTACH_UNKNOW 9    //未知，初始状态

class KERNEL_EXPORT CBF_SocketTcp  
{
public:

	// 函数名: GetConnectStatus
	// 编程  : 王明松 2012-5-11 16:11:15
	// 返回  : bool 
	// 描述  : 返回连接的状态
	bool GetConnectStatus();

	// 函数名: Send
	// 编程  : 王明松 2012-4-24 12:48:35
	// 返回  : int <0发送失败，要关闭连接了   >=0发送的数据长度  ==0可能阻塞等原因造成，但无需关闭连接，重发即可
	// 参数  : const char *databuf  要发送的数据
	// 参数  : unsigned int datalen  要发送的数据长度
	// 描述  : 发送指定长度数据，若数据太大，可能在网络层会分帧发送，造成速度慢等问题
	int Send(const char *databuf,unsigned int datalen);

	// 函数名: SendFrame
	// 编程  : 王明松 2012-4-24 12:40:35
	// 返回  : int <0发送失败，要关闭连接了,可能发送成功一部分，datalen存已发送数据长度 >=0发送的数据长度 若和要发送的长度不等可能阻塞等原因造成，但无需关闭连接，重发剩下的即可
	// 参数  : const char *databuf   要发送的数据
	// 参数  : unsigned int &datalen   要发送的数据长度/已发送数据长度
	// 描述  : 按帧发送数据，目前定义最大为8192，超过此值分开发送
	int SendFrame(const char *databuf,unsigned int &datalen);

	// 函数名: Receive
	// 编程  : 王明松 2012-4-24 11:19:51
	// 返回  : int <0接收失败,可以关闭连接了 =0未收到任何数据  >0收到的数据长度
	// 参数  : char *buf
	// 参数  : unsigned int bufsize  缓冲大小
	// 描述  : 接收数据至缓冲，注意在Receive之前要检查是否有读事件发生，如果有才能调用，因为对方主动关的时候，recv返回仍是0，此时此函数返回-1
	int Receive(char *buf,unsigned int bufsize);

	// 函数名: Receive
	// 编程  : 王明松 2012-4-24 11:08:27
	// 返回  : int <0接收失败,可以关闭连接了,可能有收到数据   =0超时未收到任何数据   >0收到的数据长度
	// 输出参数  : char *buf 接收数据
	// 参数  : unsigned int &wantlen  输入为要收的数据长度，输出为实际收到的数据长度
	// 参数  : unsigned int tms  超时时间ms
	// 描述  : 在规定时间内收取指定长度的数据
	int Receive(char *buf,unsigned int &wantlen,unsigned int tms);

	// 函数名: GetReceiveDataLen
	// 编程  : 王明松 2012-4-23 16:57:09
	// 返回  : int =0 无数据可读 -1有数据可读，但数据长度为0，连接已断开 -2出错了，连接已断开
	// 描述  : 得到socket上要收的数据长度，>0有数据但不准确
	int GetReceiveDataLen();

	// 函数名: CheckWriteEvent
	// 编程  : 王明松 2012-4-23 16:29:54
	// 返回  : int <0出错了，可能连接已断开 0不可写 1可写
	// 参数  : unsigned int tms  超时ms
	// 描述  : 检查socket是否可写
	int CheckWriteEvent(unsigned int tms);

	// 函数名: CheckReadEvent
	// 编程  : 王明松 2012-4-23 16:29:23
	// 返回  : int <0出错了，可能连接已断开 0不可读 1可读
	// 参数  : unsigned int tms  超时ms
	// 描述  : 检查socket是否可读
	int CheckReadEvent(unsigned int tms);

	// 函数名: CheckConnect
	// 编程  : 王明松 2012-4-23 16:06:51
	// 返回  : bool 
	// 参数  : unsigned int tms  超时ms
	// 描述  : 检查非阻塞方式连接是否建立，调用此方法前要判断m_nErrNo是否为 EWOULDBLOCK或EINPROGRESS
	bool CheckConnect(unsigned int tms);

	// 函数名: ConnectServer
	// 编程  : 王明松 2012-4-23 15:06:25
	// 返回  : bool 
	// 参数  : const char *host      服务IP
	// 参数  : unsigned short port   服务端口
	// 参数  : unsigned int tms=0   =0阻塞式连接，  >0非阻塞式连接超时ms,主动设置为非阻塞方式 注意在WINDOWS下最小为10ms
	// 描述  : 作为客户端连接服务端  超时为0时不会主动设置socket为阻塞方式，要在Create指定或调用SetBlockFlag
	bool ConnectServer(const char *host ,unsigned short port,unsigned int tms=0);


	// 函数名: Connect
	// 编程  : 王明松 2012-5-17 11:35:22
	// 返回  : bool 
	// 参数  : SOCKADDR_IN iaddr  服务地址及端口
	// 描述  : 作为客户端连接服务端  不会主动设置socket为阻塞方式，要在Create指定或调用SetBlockFlag
	bool Connect(SOCKADDR_IN iaddr);

	// 函数名: SetBlockFlag
	// 编程  : 王明松 2009-5-8 10:27:12
	// 返回  : void 
	// 参数  : bool isblock  true阻塞模式  false非阻塞模式 
	// 描述  : 设置socket的阻塞模式  
	void SetBlockFlag(bool isblock);

	// 函数名: AttachBindSocket
	// 编程  : 王明松 2012-4-23 12:55:42
	// 返回  : void 
	// 参数  : SOCKET_HANDLE socket
	// 参数  : const char *clientip
	// 描述  : 为本类配置一个socket连接，使之可以收发数据等，析构时关闭连接
	void AttachBindSocket(SOCKET_HANDLE socket,const char *clientip);

	// 函数名: AttachSocket
	// 编程  : 王明松 2012-4-23 12:55:42
	// 返回  : void 
	// 参数  : SOCKET_HANDLE socket
	// 参数  : const char *clientip
	// 描述  : 为本类配置一个socket连接，使之可以收发数据等
	void AttachSocket(SOCKET_HANDLE socket,const char *clientip);

	// 函数名: Accept
	// 编程  : 王明松 2005-3-20 10:52:28
	// 返回  : int >0 socket连接标识  <0accept出错，可能连接已关闭
	// 输出参数  : char* lpaddress  //连接过来的客户端IP
	// 输入参数  : bool blocked=true  true阻塞模式 false非阻塞模式
	// 描述  : 接受远程的连接请求并置连接的阻塞模式
	SOCKET_HANDLE Accept(char* lpaddress,bool blocked=true);


	// 函数名: Accept
	// 编程  : 王明松 2012-5-22 10:55:11
	// 返回  : SOCKET_HANDLE >0 socket连接标识  <0accept出错，可能连接已关闭
	// 参数  : SOCKADDR_IN &iaddr   客户端的IP及端口   若是AF_UNIX则为127.0.0.1及服务侦听端口
	// 参数  : bool blocked=true    true阻塞模式 false非阻塞模式
	// 描述  : 接受远程的连接请求并置连接的阻塞模式 
	SOCKET_HANDLE Accept(SOCKADDR_IN &iaddr, bool blocked=true);

	// 函数名: Listen
	// 编程  : 王明松 2012-4-23 12:31:42
	// 返回  : bool 
	// 参数  : unsigned short port
	// 参数  : unsigned int queuenum=5
	// 描述  : 开始服务端侦听，必先Create
	bool Listen(unsigned short port,unsigned int queuenum=5,const char *localip=NULL);
	

	// 函数名: Close
	// 编程  : 王明松 2012-4-23 12:29:17
	// 返回  : void 
	// 描述  : 关闭连接 若是BF_SOCKET_CLIENT则m_bConnected=false
	void Close();

	// 函数名: GetPort
	// 编程  : 王明松 2012-4-23 12:28:55
	// 返回  : unsigned short 
	// 描述  : 取端口信息 BF_SOCKET_CLIENT:服务端侦听端口  BF_SOCKET_SERVER:侦听端口        
	unsigned short GetPort();

	// 函数名: GetAddress
	// 编程  : 王明松 2012-4-23 12:27:19
	// 返回  : std::string 
	// 描述  : 取IP地址信息 BF_SOCKET_CLIENT:服务端IP   BF_SOCKET_SERVER:127.0.0.1  BF_SOCKET_ATTACH/BF_SOCKET_ATTACHBIND: 客户端连接过来的IP
	std::string GetAddress();

	// 函数名: GetSocketType
	// 编程  : 王明松 2012-4-23 11:44:39
	// 返回  : unsigned int 
	// 描述  : 获取socket类型 BF_SOCKET_CLIENT  BF_SOCKET_SERVER BF_SOCKET_ATTACH BF_SOCKET_ATTACHBIND
	unsigned int GetSocketType();
	
	// 函数名: GetSocketMode
	// 编程  : 王明松 2012-4-23 11:39:05
	// 返回  : int 
	// 描述  : 返回通讯模式AF_INET或AF_UNIX
	int GetSocketMode();

	// 函数名: GetErrorMsg
	// 编程  : 王明松 2012-4-23 11:16:14
	// 返回  : std::string 
	// 描述  : 返回socket错误信息
	std::string GetErrorMsg();

	// 函数名: GetErrorNo
	// 编程  : 王明松 2012-4-23 11:15:06
	// 返回  : int  返回h_errno(WINDOWS)或errno(UNIX)
	// 描述  : 取得错误号
	int GetErrorNo();

	// 函数名: GetSocket
	// 编程  : 王明松 2012-4-23 11:13:44
	// 返回  : SOCKET_HANDLE 
	// 描述  : 返回本类的SOCKET
	SOCKET_HANDLE GetSocket();

	// 函数名: Create
	// 编程  : 王明松 2012-4-23 10:45:51
	// 返回  : bool 
	// 参数  : int AddressFamily=AF_INET  通讯模式 AF_INET  AF_UNIX
	// 参数  : bool blocked=true          true阻塞模式false非阻塞模式
	// 描述  : 建立socket
	bool Create(int AddressFamily=AF_INET,bool blocked=true);

	CBF_SocketTcp(int addressFamily=AF_INET);
	virtual ~CBF_SocketTcp();

protected:

	
	SOCKET_HANDLE   m_socket;  //socket连接标识
	int             m_nErrno; //socket errno
	char            m_sErrMsg[300]; //出错信息
	int             m_nAddressFamily; //通讯模式，是网络模式还是UNIX DOMAIN sock模式
	
	unsigned int    m_nSocketType;   //socket类型  BF_SOCKET_CLIENT BF_SOCKET_SERVER BF_SOCKET_ATTACH BF_SOCKET_ATTACHBIND
	char            m_sAddress[80];//BF_SOCKET_CLIENT:服务端IP   BF_SOCKET_SERVER:127.0.0.1  BF_SOCKET_ATTACH/BF_SOCKET_ATTACHBIND: 客户端连接过来的IP
	unsigned short  m_nPort;    //BF_SOCKET_CLIENT:服务端侦听端口  BF_SOCKET_SERVER:侦听端口        
	bool            m_bConnected;  //若客户端BF_SOCKET_CLIENT是否连接成功    
	
protected:

	// 函数名: Connect
	// 编程  : 王明松 2012-4-23 14:58:05
	// 返回  : bool 
	// 描述  : 连接服务端 使用m_sAddress及m_nPort
	bool Connect();

	// 函数名: GetLastErrorInfo
	// 编程  : 王明松 2012-4-23 11:04:55
	// 返回  : void 
	// 描述  : 获取出错信息至m_errno及m_errMsg中
	void GetLastErrorInfo();

	unsigned int  m_nSendBlockSize;//每次发送数据块的大小
};

#endif // !defined(AFX_BF_SOCKETTCP_H__99507094_1B43_494E_9D1C_573498387EC3__INCLUDED_)
