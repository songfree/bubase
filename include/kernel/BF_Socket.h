// BF_Socket.h: interface for the CBF_Socket class.
//
//////////////////////////////////////////////////////////////////////

/***********************************************************************************
  编码日志记录
  编码者: 王明松
  编码日期 2012-4-27 10:11:31 重构
  版本: V1.00 
  说明: SOCKET定义的头文件
 ***********************************************************************************/


#if !defined(AFX_BF_SOCKET_H__7904AA5C_3525_445F_B38C_3B4C0EFB679E__INCLUDED_)
#define AFX_BF_SOCKET_H__7904AA5C_3525_445F_B38C_3B4C0EFB679E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "public.h"

#define MAX_FRAME_SIZE       8192        
#define UNIX_BLOCK_SIZE      8192
#define TCP_BLOCK_SIZE       8192        //默认的接收缓冲大小
#define MAX_MSGLEN           512
#define UDP_BLOCK_SIZE       8192        //默认的接收缓冲大小


#ifdef SOLARIS
#define INADDR_NONE  0
#endif

#if defined(_WINDOWS)
#define FD_SETSIZE 10240
#include <winsock2.h>  
#include <wsipx.h>
#include <io.h>
typedef SOCKET SOCKET_HANDLE;
#define EWOULDBLOCK WSAEWOULDBLOCK
#define EINTR WSAEINTR
#define EINPROGRESS WSAEINPROGRESS

#pragma comment(lib, "WS2_32.lib") 
#pragma message("Automatically linking with WS2_32.lib")
#endif

#if defined(AIX) || defined(LINUX) || defined(SOLARIS) || defined(HP_UX)
typedef int SOCKET_HANDLE;
#define SOCKET_ERROR            (-1)
#define	INVALID_SOCKET (-1)
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr_in *PSOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct sockaddr *PSOCKADDR;
typedef fd_set FD_SET;
#if defined(LINUX) || defined(SOLARIS)
#define	SO_REUSEPORT	15
#endif
// #include <sys/socket.h>
// #include <errno.h>
// #include <unistd.h>
// #include <netdb.h>
// #include <arpa/inet.h>
// #include <sys/time.h>
// #include <sys/ioctl.h>
// 
// #if !defined(HP_UX)
// #include <sys/select.h>
// #endif
// #ifdef SOLARIS
// #include <sys/filio.h>
// #endif

#endif

#if defined(_WINDOWS) //|| defined(LINUX) || defined(AIX) || defined(HP_UX)
struct sockaddr_un
{
	short int sun_family; /* AF_UNIX */
	char sun_path[108]; /* path name (gag) */
};
#endif

#if defined(LINUX) || defined(AIX) || defined(HP_UX)
#define SOCKADDRLEN	socklen_t
#else
#define SOCKADDRLEN	int
#endif

#endif // !defined(AFX_BF_SOCKET_H__7904AA5C_3525_445F_B38C_3B4C0EFB679E__INCLUDED_)
