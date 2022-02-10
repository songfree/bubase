#ifndef __TCP_INIT_H_
#define __TCP_INIT_H_

#ifdef WIN32
//#include <winsock.h>
#include <winsock2.h>
#else
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#ifndef AF_INET
#define AF_UNSPEC       0               /* unspecified */
#define AF_UNIX         1               /* local to host (pipes, portals) */
#define AF_INET         2               /* internetwork: UDP, TCP, etc. */
#define AF_IMPLINK      3               /* arpanet imp addresses */
#define AF_PUP          4               /* pup protocols: e.g. BSP */
#define AF_CHAOS        5               /* mit CHAOS protocols */
#define AF_IPX          6               /* IPX and SPX */
#define AF_NS           6               /* XEROX NS protocols */
#define AF_ISO          7               /* ISO protocols */
#define AF_OSI          AF_ISO          /* OSI is ISO */
#define AF_ECMA         8               /* european computer manufacturers */
#define AF_DATAKIT      9               /* datakit protocols */
#define AF_CCITT        10              /* CCITT protocols, X.25 etc */
#define AF_SNA          11              /* IBM SNA */
#define AF_DECnet       12              /* DECnet */
#define AF_DLI          13              /* Direct data link interface */
#define AF_LAT          14              /* LAT */
#define AF_HYLINK       15              /* NSC Hyperchannel */
#define AF_APPLETALK    16              /* AppleTalk */
#define AF_NETBIOS      17              /* NetBios-style addresses */
#define AF_VOICEVIEW    18              /* VoiceView */
#define AF_FIREFOX      19              /* FireFox */
#define AF_UNKNOWN1     20              /* Somebody is using this! */
#define AF_BAN          21              /* Banyan */

#define AF_MAX          22

#endif

#ifndef PF_INET
   #define PF_UNSPEC       AF_UNSPEC
   #define PF_UNIX         AF_UNIX
   #define PF_INET         AF_INET
   #define PF_IMPLINK      AF_IMPLINK
   #define PF_PUP          AF_PUP
   #define PF_CHAOS        AF_CHAOS
   #define PF_NS           AF_NS
   #define PF_IPX          AF_IPX
   #define PF_ISO          AF_ISO
   #define PF_OSI          AF_OSI
   #define PF_ECMA         AF_ECMA
   #define PF_DATAKIT      AF_DATAKIT
   #define PF_CCITT        AF_CCITT
   #define PF_SNA          AF_SNA
   #define PF_DECnet       AF_DECnet
   #define PF_DLI          AF_DLI
   #define PF_LAT          AF_LAT
   #define PF_HYLINK       AF_HYLINK
   #define PF_APPLETALK    AF_APPLETALK
   #define PF_VOICEVIEW    AF_VOICEVIEW
   #define PF_FIREFOX      AF_FIREFOX
   #define PF_UNKNOWN1     AF_UNKNOWN1
   #define PF_BAN          AF_BAN

   #define PF_MAX          AF_MAX
#endif

#ifndef SOCK_STREAM
/*
 * Types
 */
#define SOCK_STREAM     1               /* stream socket */
#define SOCK_DGRAM      2               /* datagram socket */
#define SOCK_RAW        3               /* raw-protocol interface */
#define SOCK_RDM        4               /* reliably-delivered message */
#define SOCK_SEQPACKET  5               /* sequenced packet stream */

#endif

//在非Windows系统，我们使用close来关闭一个socket，而在Windows下，我们使用closesocket
//这里，我们统一成closesocket
#ifndef WIN32
#define closesocket close
#endif
/* ****** Updated by CHENYH at 2004-3-3 11:23:59 ****** 
#ifdef UNIX
#define closesocket close
#endif

#ifdef VOS
#define closesocket net_close
#endif
*/

//在非Windows系统，我们使用ioctl来设置一个socket，而在Windows下，我们使用ioctlsocket
//这里，我们统一成ioctlsocket
#ifndef WIN32
#define ioctlsocket ioctl
#endif


/* ****** Updated by CHENYH at 2004-4-19 11:40:07 ****** 
#ifdef VOS
#define ioctlsocket net_ioctl
#endif
*/

//在Windows下，所有的socket的错误号全部以WSA开始，我们将需要的那些统一成Unix的标准
#ifdef WIN32
#define EWOULDBLOCK WSAEWOULDBLOCK
#endif

//在VOS下，所有的socket的错误号全部以e$开始，我们将需要的那些统一成Unix的标准

#ifdef VOS
#define EINTR e$tcp_interrupted_operation
#define EWOULDBLOCK e$caller_must_wait
#endif


//在Windows下，使用h_error来获取最近的socket错误，而在其他系统下，直接使用errno。
//我们同意成GET_LAST_SOCK_ERROR()
#ifdef WIN32
#define GET_LAST_SOCK_ERROR() h_errno
#else
#define GET_LAST_SOCK_ERROR() errno
#endif


/* ****** Updated by CHENYH at 2004-4-14 10:31:52 ****** 
#ifdef WIN32
#define SOCKADDRLEN int
#else
#define SOCKADDRLEN socklen_t
#endif
*/
//在传送sockaddr时，使用的表示长度的类型不同 gyd 20020605 将 UNIX变为LINUX
#if defined(LINUX) || defined(SUNOS) || defined(AIX)
#define SOCKADDRLEN	socklen_t
#endif
#ifndef SOCKADDRLEN
#define SOCKADDRLEN int
#endif
/* ****** Updated by CHENYH at 2004-6-10 19:24:00 ****** 
#ifdef HP_UX
#define SOCKADDRLEN	int
#endif
#ifdef WIN32
#define SOCKADDRLEN	int
#endif
*/

#ifndef INVALID_SOCKET
#define	INVALID_SOCKET	(-1)
#endif

#ifdef BUPUB_EXPORTS
#define BUPUB_API __declspec(dllexport)
#else
#ifdef _WINDOWS
#define BUPUB_API __declspec(dllimport)
#else
#define BUPUB_API 
#endif
#endif

class  CTcpSocket2  
{
private:
#ifdef WIN32
   SOCKET m_socket;
#else
   int m_socket;
#endif
	int m_nAddressFamily;
   bool m_bConnected;   // 2005-11-7 12:29:20 CHENYH 为了标注本端口是否连接成功

public:
	bool IsConnected();
   // 函数名: CTcpSocket::CheckDataArrived
   // 编程  : 陈永华 2004-2-10 21:40:02
   // 描述  : 检查在一定时间内该连接上是否有数据被接收到
   // 返回  : int -1:端口处于被关闭状态；-2:在等待接收数据的时候出错；0:在指定的时间里，没有数据可被接收；1:有数据可被接收
   // 参数  : int timeout 最大等待时间，毫秒计
	int CheckDataArrived(int timeout);
   
	bool IsFree();

   // 函数名: CTcpSocket::Send
   // 编程  : 陈永华 2004-2-10 21:31:05
   // 描述  : 通过本链路发送数据
   // 返回  : int -1:链路没有正常建立，被关闭；-2:发送数据时候出错，被关闭；>=0: 发送指定长度数据成功，应该==nDesignedSize
   // 参数  : char *ptr :需要发送数据的缓存
   // 参数  : int nDesignedSize ：发送数据的指定长度
   int Send(char *ptr, int nDesignedSize); // 通过本链路发送数据 返回  : int -1:链路没有正常建立，被关闭；-2:发送数据时候出错，被关闭；>=0: 发送指定长度数据成功，应该==nDesignedSize

   // 函数名: CTcpSocket::Recv
   // 编程  : 陈永华 2004-2-10 21:15:05
   // 描述  : 通过本链路接收数据
   // 返回  : int >0:接收到的数据长度，即nDesignedSize；-1:被对方正常关闭；-10:输入的nDesignedSize<=0；-2:因为链路错误而被关闭；0:超时；-3:在接收数据的时候，出错而被关闭
   // 参数  : char *vptr ：接收数据的缓存
   // 参数  : int nDesignedSize ： 指定接收数据的长度
   // 参数  : int timeout : 等待接收数据的超时时间，毫秒计
	int Recv(char *vptr, int nDesignedSize, int init_timeout=0);  // 通过本链路接收数据 返回  : int -1:被对方正常关闭；-10:输入的nDesignedSize<=0；-2:因为链路错误而被关闭；0:超时；-3:在接收数据的时候，出错而被关闭
	bool GetPeerName(char *peername,unsigned short &peerport);
	bool Attach(int socket,int addressfamily);
	bool Accept(CTcpSocket2 &sock, char *remote=NULL);
	bool ConnectTcp(char* host, short port);
/* ****** Updated by CHENYH at 2006-3-10 15:50:33 ****** 
	bool Listen(short iPort);
*******************************************************/
	bool Listen(short iPort,char *ip=NULL);
	bool Create(int addressfamily=AF_INET,int type=SOCK_STREAM);
	int GetLastError(char *szMsgBuf,int MsgBLen=256);
	void Close();
	CTcpSocket2();
	CTcpSocket2(CTcpSocket2 &ts);
	virtual ~CTcpSocket2();

};

#undef BUPUB_API

#endif
