// BF_SocketUdp.h: interface for the CBF_SocketUdp class.
//
//////////////////////////////////////////////////////////////////////

/***********************************************************************************
  编码日志记录
  编码者: 王明松
  编码日期 2012-4-17 14:07:47 重构
  版本: V1.00 
  说明: Udp通讯类  此类要么是服务要么是客户，不能同时
 ***********************************************************************************/


#if !defined(AFX_BF_SOCKETUDP_H__67745822_F0AF_45DC_97C6_22F4E0255E5D__INCLUDED_)
#define AFX_BF_SOCKETUDP_H__67745822_F0AF_45DC_97C6_22F4E0255E5D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BF_Socket.h"
#include "public.h"

#ifdef KERNEL_EXPORTS
#define KERNEL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNEL_EXPORT __declspec(dllimport)
#else
#define KERNEL_EXPORT 
#endif
#endif

class KERNEL_EXPORT CBF_SocketUdp  
{
public:
	// 函数名: CheckReadEvent
	// 编程  : 王明松 2012-4-23 16:29:23
	// 返回  : int <0出错了，可能连接已断开 0不可读 1可读
	// 参数  : unsigned int tms  超时ms
	// 描述  : 检查socket是否可读
	int CheckReadEvent(unsigned int tms);
	
	// 函数名: GetSockAddr
	// 编程  : 王明松 2012-4-27 13:25:41
	// 返回  : sockaddr_in & 
	// 描述  : 取得接收到的客户端地址信息或作为客户端设置的主机信息
	sockaddr_in & GetSockAddr();

	// 函数名: GetSockAddr
	// 编程  : 王明松 2012-4-27 13:26:25
	// 返回  : void 
	// 参数  : sockaddr_in &addr
	// 描述  : 取得接收到的客户端地址信息或作为客户端设置的主机信息
	void GetSockAddr(sockaddr_in &addr);


	// 函数名: GetHostString
	// 编程  : 王明松 2012-4-27 13:26:34
	// 返回  : std::string 
	// 描述  : 取得作为客户端设置的主机信息
	std::string GetHostString();

	// 函数名: GetPort
	// 编程  : 王明松 2012-4-27 13:26:50
	// 返回  : unsigned int 
	// 描述  : 取得UDP端口
	unsigned int GetPort();

	// 函数名: IsServer
	// 编程  : 王明松 2012-4-27 13:26:59
	// 返回  : bool 
	// 描述  : 本实例是服务端还是客户端
	bool IsServer();

	// 函数名: Close
	// 编程  : 王明松 2012-4-27 13:16:47
	// 返回  : void 
	// 描述  : 关闭socket
	void Close();

	// 函数名: BindClient
	// 编程  : 王明松 2012-4-27 13:04:42
	// 返回  : bool 
	// 参数  : const char *sip 服务的IP，若长度<7则为广播，否则发给指定的IP
	// 参数  : unsigned int port  服务的UDP端口
	// 描述  : 绑定要发送的服务的服务端口信息，将服务端的地址放入m_sSockAddr
	bool BindClient(const char *sip,unsigned int port);

	// 函数名: RecvFrom
	// 编程  : 王明松 2012-4-27 12:24:56
	// 返回  : int =0无数据  -1出错了  >0收到的数据长度
	// 参数  : char *databuf       数据
	// 参数  : unsigned int buflen  数据长度
	// 参数  : sockaddr_in &addr  发送方的地址
	// 描述  : 接收发来的数据
	int RecvFrom(char *databuf,unsigned int buflen,sockaddr_in &addr);


	// 函数名: SendTo
	// 编程  : 王明松 2012-4-27 13:08:23
	// 返回  : int <0发送失败, >0发送成功的数据长度 =0未发送成功，可能对方缓冲满
	// 参数  : sockaddr_in addr  发给目的地的地址
	// 参数  : const char *databuf
	// 参数  : unsigned int buflen
	// 描述  : 发送数据
	int SendTo(sockaddr_in addr,const char *databuf,unsigned int buflen);

	// 函数名: SendTo
	// 编程  : 王明松 2012-4-27 13:08:23
	// 返回  : int <0发送失败, >0发送成功的数据长度 =0未发送成功，可能对方缓冲满
	// 参数  : const char *databuf
	// 参数  : unsigned int buflen
	// 描述  : 发送数据给服务端，调用之前必须BindClient
	int SendTo(const char *databuf,unsigned int buflen);

	// 函数名: BindServer
	// 编程  : 王明松 2012-4-27 11:20:32
	// 返回  : bool 
	// 参数  : unsigned int port   端口
	// 参数  : unsigned int bufsize  单位k
	// 描述  : 绑定服务端，设置接收缓冲大小
	bool BindServer(unsigned int port,unsigned int bufsize=1024);

	// 函数名: GetLastErrorInfo
	// 编程  : 王明松 2012-4-23 11:04:55
	// 返回  : void 
	// 描述  : 获取出错信息至m_errno及m_errMsg中
	void GetLastErrorInfo();

	// 函数名: SetBlockFlag
	// 编程  : 王明松 2009-5-8 10:27:12
	// 返回  : void 
	// 参数  : bool isblock  true阻塞模式  false非阻塞模式 
	// 描述  : 设置socket的阻塞模式  
	void SetBlockFlag(bool isblock);

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
	// 编程  : 王明松 2012-4-27 10:34:54
	// 返回  : bool 
	// 描述  : 建立一个Socket udp就不用AF_UNIX了，没必要
	bool Create();

	CBF_SocketUdp();
	virtual ~CBF_SocketUdp();

protected:
	
	
	SOCKET_HANDLE   m_socket;  //socket连接标识
	int             m_nErrno; //socket errno
	char            m_sErrMsg[300]; //出错信息
	int             m_nAddressFamily; //通讯模式，固定为AF_INET

	char            m_sAddress[32];//要发送的服务端的地址信息,由BindClient设置
	unsigned short  m_nPort;       //UDP端口
	
	sockaddr_in     m_sSockAddr; //地址信息 客户端由BindClient设置
	bool            m_bIsServer;//是否服务端

};

#endif // !defined(AFX_BF_SOCKETUDP_H__67745822_F0AF_45DC_97C6_22F4E0255E5D__INCLUDED_)
