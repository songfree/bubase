// BF_SocketUdp.cpp: implementation of the CBF_SocketUdp class.
//
//////////////////////////////////////////////////////////////////////

#include "BF_SocketUdp.h"
#include "BF_Date_Time.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CBF_SocketUdp::CBF_SocketUdp()
{
	m_nAddressFamily=AF_INET;
	m_nErrno = 0;
	m_socket = INVALID_SOCKET ;
	bzero(m_sErrMsg,sizeof(m_sErrMsg));
	m_bIsServer = false;
	m_nPort =0;
	bzero(m_sAddress,sizeof(m_sAddress));
	bzero(&m_sSockAddr,sizeof(sockaddr_in));
}

CBF_SocketUdp::~CBF_SocketUdp()
{
	
}

bool CBF_SocketUdp::Create()
{
    m_nAddressFamily=AF_INET;
    if ((m_socket = socket(m_nAddressFamily,SOCK_DGRAM,0)) <0 ) 
	{
		GetLastErrorInfo();
		return false;
	}
	SetBlockFlag(false);

	int on=1;
	setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST, (char *)&on, sizeof(on));//广播模式
#if defined(_WINDOWS)
	setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
#endif
#if defined(AIX) || defined(LINUX) || defined(SOLARIS) || defined(HP_UX) 
	setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	setsockopt(m_socket, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on));
#endif

	return true; 
}
SOCKET_HANDLE CBF_SocketUdp::GetSocket()
{
	return m_socket;
}

int CBF_SocketUdp::GetErrorNo()
{
	return m_nErrno;
}

std::string CBF_SocketUdp::GetErrorMsg()
{
	return m_sErrMsg;
}
void CBF_SocketUdp::GetLastErrorInfo()
{
	m_nErrno = GET_LAST_SOCK_ERROR();

	switch(m_nErrno)
	{
#ifdef _WINDOWS
		case WSAEINTR:
			 sprintf(m_sErrMsg,"%d:Interrupted function call. ",m_nErrno);
			 break;
		case WSAEACCES:
			 sprintf(m_sErrMsg,"%d:Permission denied. ",m_nErrno);
			 break;
		case WSAEFAULT:
			 sprintf(m_sErrMsg,"%d:Bad address. ",m_nErrno);
			 break;
		case WSAEINVAL:
			 sprintf(m_sErrMsg,"%d:Invalid argument. ",m_nErrno);
			 break;
		case WSAEMFILE:
			 sprintf(m_sErrMsg,"%d:Too many open files. ",m_nErrno);
			 break;
		case WSAEWOULDBLOCK:
			 sprintf(m_sErrMsg,"%d:Resource temporarily unavailable. ",m_nErrno);
			 break;
		case WSAEINPROGRESS:
			 sprintf(m_sErrMsg,"%d:Operation now in progress. ",m_nErrno);
			 break;
		case WSAEALREADY:
			 sprintf(m_sErrMsg,"%d:Operation already in progress. ",m_nErrno);
			 break;
		case WSAENOTSOCK:
			 sprintf(m_sErrMsg,"%d:Socket operation on nonsocket. ",m_nErrno);
			 break;
		case WSAEDESTADDRREQ:
			 sprintf(m_sErrMsg,"%d:Destination address required. ",m_nErrno);
			 break;
		case WSAEMSGSIZE:
			sprintf(m_sErrMsg,"%d:Message too long. ",m_nErrno);
			break;
		case WSAEPROTOTYPE:
			sprintf(m_sErrMsg,"%d:Protocol wrong type for socket. ",m_nErrno);
			break;
		case WSAENOPROTOOPT:
			sprintf(m_sErrMsg,"%d:Bad protocol option. ",m_nErrno);
			break;
		case WSAEPROTONOSUPPORT:
			sprintf(m_sErrMsg,"%d:Protocol not supported. ",m_nErrno);
			break;
		case WSAESOCKTNOSUPPORT:
			sprintf(m_sErrMsg,"%d:Socket type not supported. ",m_nErrno);
			break;
		case WSAEOPNOTSUPP:
			sprintf(m_sErrMsg,"%d:Operation not supported. ",m_nErrno);
			break;
		case WSAEPFNOSUPPORT:
			sprintf(m_sErrMsg,"%d:Protocol family not supported. ",m_nErrno);
			break;
		case WSAEAFNOSUPPORT:
			sprintf(m_sErrMsg,"%d:Address family not supported by protocol family. ",m_nErrno);
			break;
		case WSAEADDRINUSE:
			sprintf(m_sErrMsg,"%d:Address already in use. ",m_nErrno);
			break;
		case WSAEADDRNOTAVAIL:
			sprintf(m_sErrMsg,"%d:Cannot assign requested address. ",m_nErrno);
			break;
		case WSAENETDOWN:
			sprintf(m_sErrMsg,"%d:Network is down. ",m_nErrno);
			break;
		case WSAENETUNREACH:
			sprintf(m_sErrMsg,"%d:Network is unreachable. ",m_nErrno);
			break;
		case WSAENETRESET:
			sprintf(m_sErrMsg,"%d:Network dropped connection on reset. ",m_nErrno);
			break;
		case WSAECONNABORTED:
			sprintf(m_sErrMsg,"%d:Software caused connection abort. ",m_nErrno);
			break;
		case WSAECONNRESET:
			sprintf(m_sErrMsg,"%d:Connection reset by peer. ",m_nErrno);
			break;
		case WSAENOBUFS:
			sprintf(m_sErrMsg,"%d:No buffer space available. ",m_nErrno);
			break;
		case WSAEISCONN:
			sprintf(m_sErrMsg,"%d:Socket is already connected. ",m_nErrno);
			break;
		case WSAENOTCONN:
			sprintf(m_sErrMsg,"%d:Socket is not connected. ",m_nErrno);
			break;
		case WSAESHUTDOWN:
			sprintf(m_sErrMsg,"%d:Cannot send after socket shutdown. ",m_nErrno);
			break;
		case WSAETIMEDOUT:
			sprintf(m_sErrMsg,"%d:Connection timed out. ",m_nErrno);
			break;
		case WSAECONNREFUSED:
			sprintf(m_sErrMsg,"%d:Connection refused. ",m_nErrno);
			break;
		case WSAEHOSTDOWN:
			sprintf(m_sErrMsg,"%d:Host is down. ",m_nErrno);
			break;
		case WSAEHOSTUNREACH:
			sprintf(m_sErrMsg,"%d:No route to host. ",m_nErrno);
			break;
		case WSAEPROCLIM:
			sprintf(m_sErrMsg,"%d:Too many processes. ",m_nErrno);
			break;
		case WSASYSNOTREADY:
			sprintf(m_sErrMsg,"%d:Network subsystem is unavailable. ",m_nErrno);
			break;
		case WSAVERNOTSUPPORTED:
			sprintf(m_sErrMsg,"%d:Winsock.dll version out of range. ",m_nErrno);
			break;
		case WSANOTINITIALISED:
			sprintf(m_sErrMsg,"%d:Successful WSAStartup not yet performed. ",m_nErrno);
			break;
		case WSAEDISCON:
			sprintf(m_sErrMsg,"%d:Graceful shutdown in progress. ",m_nErrno);
			break;
		case WSATYPE_NOT_FOUND:
			sprintf(m_sErrMsg,"%d:Class type not found. ",m_nErrno);
			break;
		case WSAHOST_NOT_FOUND:
			sprintf(m_sErrMsg,"%d:Host not found. ",m_nErrno);
			break;
		case WSATRY_AGAIN:
			sprintf(m_sErrMsg,"%d:Nonauthoritative host not found. ",m_nErrno);
			break;
		case WSANO_RECOVERY:
			sprintf(m_sErrMsg,"%d:This is a nonrecoverable error. ",m_nErrno);
			break;
		case WSANO_DATA:
			sprintf(m_sErrMsg,"%d:Valid name, no data record of requested type. ",m_nErrno);
			break;
		case WSA_INVALID_HANDLE:
			sprintf(m_sErrMsg,"%d:Specified event object handle is invalid. ",m_nErrno);
			break;
		case WSA_INVALID_PARAMETER:
			sprintf(m_sErrMsg,"%d:One or more parameters are invalid. ",m_nErrno);
			break;
		case WSA_IO_INCOMPLETE:
			sprintf(m_sErrMsg,"%d:Overlapped I/O event object not in signaled state. ",m_nErrno);
			break;
		case WSA_IO_PENDING:
			sprintf(m_sErrMsg,"%d:",m_nErrno);
			break;
		case WSA_NOT_ENOUGH_MEMORY:
			sprintf(m_sErrMsg,"%d:Insufficient memory available. ",m_nErrno);
			break;
		case WSA_OPERATION_ABORTED:
			sprintf(m_sErrMsg,"%d:Overlapped operation aborted. ",m_nErrno);
			break;
#ifdef WSAINVALIDPROCTABLE
		case WSAINVALIDPROCTABLE:
			sprintf(m_sErrMsg,"%d:Invalid procedure table from service provider. ",m_nErrno);
			break;
#endif
#ifdef WSAINVALIDPROVIDER
		case WSAINVALIDPROVIDER:
			sprintf(m_sErrMsg,"%d:Invalid service provider version number. ",m_nErrno);
			break;
#endif
#ifdef WSAPROVIDERFAILEDINIT
		case WSAPROVIDERFAILEDINIT:
			sprintf(m_sErrMsg,"%d:Unable to initialize a service provider. ",m_nErrno);
			break;
#endif
		case WSASYSCALLFAILURE:
			sprintf(m_sErrMsg,"%d:System call failure. ",m_nErrno);
			break;
#else  //UNIX
		case EPERM  : //1 
			sprintf(m_sErrMsg,"%d:Operation not permitted",m_nErrno); 
			break;
		case ENOENT  : //2 
			sprintf(m_sErrMsg,"%d:No such file or directory",m_nErrno); 
			break;
		case ESRCH  :  //3
			sprintf(m_sErrMsg,"%d:No such process",m_nErrno); 
			break;
		case EINTR  :  //4
			sprintf(m_sErrMsg,"%d:interrupted system call",m_nErrno); 
			break;
		case EIO  :  //5
			sprintf(m_sErrMsg,"%d:I/O error",m_nErrno); 
			break;
		case ENXIO  :  //6
			sprintf(m_sErrMsg,"%d:No such device or address",m_nErrno); 
			break;
		case E2BIG  :  //7
			sprintf(m_sErrMsg,"%d:Arg list too long",m_nErrno); 
			break;
		case ENOEXEC  ://8  
			sprintf(m_sErrMsg,"%d:Exec format error",m_nErrno); 
			break;
		case EBADF  : //9  
			sprintf(m_sErrMsg,"%d:Bad file descriptor",m_nErrno); 
			break;
		case ECHILD ://10  
			sprintf(m_sErrMsg,"%d:No child processes",m_nErrno); 
			break;
		case EAGAIN:  //11  No more processes 
			sprintf(m_sErrMsg,"%d:Resource temporarily unavailable	",m_nErrno); 
			break;
		case ENOMEM : //12  
			sprintf(m_sErrMsg,"%d:Not enough space",m_nErrno); 
			break;
		case EACCES : //13  
			sprintf(m_sErrMsg,"%d:Permission denied",m_nErrno); 
			break;
		case EFAULT :  //14
			sprintf(m_sErrMsg,"%d:Bad address",m_nErrno); 
			break;
		case ENOTBLK : //15
			sprintf(m_sErrMsg,"%d:Block device required",m_nErrno); 
			break;
		case EBUSY :   //16
			sprintf(m_sErrMsg,"%d:Resource busy",m_nErrno); 
			break;
		case EEXIST : //17  
			sprintf(m_sErrMsg,"%d:File exists",m_nErrno); 
			break;
		case EXDEV :  //18 Cross-device link
			sprintf(m_sErrMsg,"%d:Improper link",m_nErrno); 
			break;
		case ENODEV : //19 
			sprintf(m_sErrMsg,"%d:No such device",m_nErrno); 
			break;
		case ENOTDIR :  //20
			sprintf(m_sErrMsg,"%d:Not a directory",m_nErrno); 
			break;
		case EISDIR :  //21
			sprintf(m_sErrMsg,"%d:Is a directory",m_nErrno); 
			break;
		case 22 :  
			sprintf(m_sErrMsg,"%d:Invalid argument",m_nErrno); 
			break;
		case 23 :  
			sprintf(m_sErrMsg,"%d:Too many open files in system",m_nErrno); 
			break;
		case 24 :  
			sprintf(m_sErrMsg,"%d:Too many open files",m_nErrno); 
			break;
		case 25 :  
			sprintf(m_sErrMsg,"%d:Inappropriate I/O control operation",m_nErrno); 
			break;
		case 26 :  
			sprintf(m_sErrMsg,"%d:Text file busy",m_nErrno); 
			break;
		case 27 :  
			sprintf(m_sErrMsg,"%d:File too large",m_nErrno); 
			break;
		case 28 :  
			sprintf(m_sErrMsg,"%d:No space left on device",m_nErrno); 
			break;
		case 29 :  
			sprintf(m_sErrMsg,"%d:Invalid seek",m_nErrno); 
			break;
		case 30 :  
			sprintf(m_sErrMsg,"%d:Read only file system",m_nErrno); 
			break;
		case 31 :  
			sprintf(m_sErrMsg,"%d:Too many links",m_nErrno); 
			break;
		case 32 :  sprintf(m_sErrMsg,"%d:Broken pipe",m_nErrno); 
			break;
		case 33 :  
			sprintf(m_sErrMsg,"%d:Domain error within math function",m_nErrno); 
			break;
		case 34 :  
			sprintf(m_sErrMsg,"%d:Result too large",m_nErrno); 
			break;
		case 35 :  
			sprintf(m_sErrMsg,"%d:No message of desired type",m_nErrno); 
			break;
		case 36 :  
			sprintf(m_sErrMsg,"%d:Identifier removed",m_nErrno); 
			break;
		case 37 :  
			sprintf(m_sErrMsg,"%d:Channel number out of range",m_nErrno); 
			break;
		case 38 :  
			sprintf(m_sErrMsg,"%d:Level 2 not synchronized",m_nErrno); 
			break;
		case 39 :  
			sprintf(m_sErrMsg,"%d:Level 3 halted",m_nErrno); 
			break;
		case 40 :  
			sprintf(m_sErrMsg,"%d:Level 3 reset",m_nErrno); 
			break;
		case 41 :  
			sprintf(m_sErrMsg,"%d:Link number out of range",m_nErrno); 
			break;
		case 42 :  
			sprintf(m_sErrMsg,"%d:Protocol driver not attached",m_nErrno); 
			break;
		case 43 :  
			sprintf(m_sErrMsg,"%d:No CSI structure available",m_nErrno); 
			break;
		case 44 :  
			sprintf(m_sErrMsg,"%d:Level 2 halted",m_nErrno); 
			break;
		case 45 :  
			sprintf(m_sErrMsg,"%d:Resource deadlock avoided",m_nErrno); 
			break;
		case 46 :  
			sprintf(m_sErrMsg,"%d:Device not ready",m_nErrno); 
			break;
		case 47 :  
			sprintf(m_sErrMsg,"%d:Write-protected media",m_nErrno); 
			break;
		case 48 :  
			sprintf(m_sErrMsg,"%d:Unformatted media",m_nErrno); 
			break;
		case 49 :  
			sprintf(m_sErrMsg,"%d:No locks available",m_nErrno); 
			break;
		case 50 :  
			sprintf(m_sErrMsg,"%d:no connectioncase case",m_nErrno); 
			break;
		case 52 :  
			sprintf(m_sErrMsg,"%d:no filesystemcase case",m_nErrno); 
			break;
		case 53 :  
			sprintf(m_sErrMsg,"%d:old, currently unused AIX errno",m_nErrno); 
			break; 
		case 54 :  
			sprintf(m_sErrMsg,"%d:Operation would block",m_nErrno); 
			break;
		case 55 :  
			sprintf(m_sErrMsg,"%d:Operation now in progress",m_nErrno); 
			break;
		case 56 :  
			sprintf(m_sErrMsg,"%d:Operation already in progress",m_nErrno); 
			break;
		case 57 :  
			sprintf(m_sErrMsg,"errno=%d:Socket operation on non-socket",m_nErrno);
			break;
		case 58 :  
			sprintf(m_sErrMsg,"errno=%d:Destination address required",m_nErrno); 
			break;
		case 59 :  
			sprintf(m_sErrMsg,"errno=%d:Message too long",m_nErrno); 
			break;
		case 60 :  
			sprintf(m_sErrMsg,"errno=%d:Protocol wrong type for socket",m_nErrno); 
			break;
		case 61 :  
			sprintf(m_sErrMsg,"errno=%d:Protocol not available",m_nErrno); 
			break;
		case 62 :  
			sprintf(m_sErrMsg,"errno=%d:Protocol not supported",m_nErrno); 
			break;
		case 63 :  
			sprintf(m_sErrMsg,"errno=%d:Socket type not supported",m_nErrno);
			break;
		case 64 :  
			sprintf(m_sErrMsg,"errno=%d:Operation not supported on socket",m_nErrno);
			break;
		case 65 : 
			sprintf(m_sErrMsg,"errno=%d:Protocol family not supported",m_nErrno); 
			break;
		case 66 :  
			sprintf(m_sErrMsg,"errno=%d:Address family not supported by protocol family",m_nErrno);
			break;
		case 67 : 
			sprintf(m_sErrMsg,"errno=%d:Address already in use",m_nErrno); 
			break;
		case 68 : 
			sprintf(m_sErrMsg,"errno=%d:Can't assign requested address",m_nErrno);
			break;
		 /* operational errors */
		case 69 : 
			sprintf(m_sErrMsg,"errno=%d:Network is down",m_nErrno);
			break;
		case 70 : 
			sprintf(m_sErrMsg,"errno=%d:Network is unreachable",m_nErrno); 
			break;
		case 71 : 
			sprintf(m_sErrMsg,"errno=%d:Network dropped connection on reset",m_nErrno);
			break;
		case 72 : 
			sprintf(m_sErrMsg,"errno=%d:Software caused connection abort",m_nErrno); 
			break;
		case 73 : 
			sprintf(m_sErrMsg,"errno=%d:Connection reset by peer",m_nErrno); 
			break;
		case 74 : 
			sprintf(m_sErrMsg,"errno=%d:No buffer space available",m_nErrno); 
			break;
		case 75 : 
			sprintf(m_sErrMsg,"errno=%d:Socket is already connected",m_nErrno); 
			break;
		case 76 :
			sprintf(m_sErrMsg,"errno=%d:Socket is not connected",m_nErrno); 
			break;
		case 77 : 
			sprintf(m_sErrMsg,"errno=%d:Can't send after socket shutdown",m_nErrno); 
			break;
		case 78 : 
			sprintf(m_sErrMsg,"errno=%d:Connection timed out",m_nErrno); 
			break;
		case 79 : 
			sprintf(m_sErrMsg,"errno=%d:Connection refused",m_nErrno); 
			break;
		case 80 : 
			sprintf(m_sErrMsg,"errno=%d:Host is down",m_nErrno); 
			break;
		case 81 :  
			sprintf(m_sErrMsg,"errno=%d:No route to host",m_nErrno); 
			break;
		case 82	: 
			sprintf(m_sErrMsg,"%d:restart the system call",m_nErrno); 
			break;
		case 83	: 
			sprintf(m_sErrMsg,"%d:Too many processes",m_nErrno); 
			break;
		case 84	: 
			sprintf(m_sErrMsg,"%d:Too many users",m_nErrno); 
			break;
		case 85	:
			sprintf(m_sErrMsg,"%d:Too many levels of symbolic links",m_nErrno);
			break;
		case 86	:  
			sprintf(m_sErrMsg,"%d:File name too long",m_nErrno);
			break;
		case 87	:  
			sprintf(m_sErrMsg,"%d:Directory not empty",m_nErrno);
			break;
		case 88	: 
			sprintf(m_sErrMsg,"%d:Disc quota exceeded",m_nErrno); 
			break;
		case 89 :  
			sprintf(m_sErrMsg,"%d:Invalid file system control data ",m_nErrno); 
			break;
		case 93	: 
			sprintf(m_sErrMsg,"%d:Item is not local to host ",m_nErrno); 
			break;
		case 109 :
			sprintf(m_sErrMsg,"%d:Function not implemented  POSIX ",m_nErrno); 
			break;
		case 110 :
			sprintf(m_sErrMsg,"%d:media surface error ",m_nErrno); 
			break;
		case 111 :
			sprintf(m_sErrMsg,"%d:I/O completed, but needs relocation ",m_nErrno); 
			break;
		case 112 : 
			sprintf(m_sErrMsg,"%d:no attribute found ",m_nErrno); 
			break;
		case 113 :
			sprintf(m_sErrMsg,"%d:security authentication denied ",m_nErrno); 
			break;
		case 114 :
			sprintf(m_sErrMsg,"%d:not a trusted program ",m_nErrno); 
			break; 
		case 115 :
			sprintf(m_sErrMsg,"%d:Too many references: can't splice ",m_nErrno); 
			break;
		case 116 :
			sprintf(m_sErrMsg,"%d:Invalid wide character ",m_nErrno);
			break;
		case 117 :
			sprintf(m_sErrMsg,"%d:asynchronous i/o cancelled ",m_nErrno); 
			break;
		case 118 :
			sprintf(m_sErrMsg,"%d:temp out of streams resources ",m_nErrno); 
			break;
		case 119 : 
			sprintf(m_sErrMsg,"%d:I_STR ioctl timed out ",m_nErrno);
			break;
		case 120 :
			sprintf(m_sErrMsg,"%d:wrong message type at stream head ",m_nErrno); 
			break;
		case 121 :
			sprintf(m_sErrMsg,"%d:STREAMS protocol error ",m_nErrno); 
			break;
		case 122 :
			sprintf(m_sErrMsg,"%d:no message ready at stream head ",m_nErrno); 
			break;
		case 123 : 
			sprintf(m_sErrMsg,"%d:fd is not a stream ",m_nErrno); 
			break;
		case 124 : 
			sprintf(m_sErrMsg,"%d:POSIX threads unsupported value ",m_nErrno); 
			break;
		case 125 : 
			sprintf(m_sErrMsg,"%d:multihop is not allowed ",m_nErrno);
			break;
		case 126 : 
			sprintf(m_sErrMsg,"%d:the link has been severed ",m_nErrno); 
			break;
		case 127 : 
			sprintf(m_sErrMsg,"%d:value too large to be stored in data type ",m_nErrno); 
			break;
#endif
		default  : 
			sprintf(m_sErrMsg,"errno=%d:No error description",m_nErrno); 
			break;
	}
}
void CBF_SocketUdp::SetBlockFlag(bool isblock)
{
	int blockflag;
	if (isblock)
	{
		blockflag=0;
	}
	else
	{
		blockflag=1;
	}
#if defined(_WINDOWS)
	ioctlsocket(m_socket,FIONBIO,(unsigned long*)&blockflag);
#endif
#if defined(AIX) || defined(LINUX) || defined(SOLARIS) || defined(HP_UX)
	ioctl(m_socket,FIONBIO,(unsigned int*)&blockflag);
#endif
}


bool CBF_SocketUdp::BindServer(unsigned int port,unsigned int bufsize)
{
	m_nPort = port;
	struct sockaddr_in          servaddr; //IPv4套接口地址定义
	bzero(&servaddr, sizeof(servaddr)); //地址结构清零

	servaddr.sin_family = m_nAddressFamily; //IPv4协议
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);//内核指定地址
	servaddr.sin_port = htons(m_nPort); //端口
	
	int rcvbufsize = bufsize*1024;
	SOCKADDRLEN paramlen = sizeof(rcvbufsize);
	int ret = setsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, (const char *)&rcvbufsize, paramlen);
	if(ret != 0)
	{
		GetLastErrorInfo();
		return false;
	}
	//分配协议地址,绑定端口
	if (bind(m_socket, (struct sockaddr*)&servaddr, sizeof(servaddr))!=0)
//	if (bind(m_socket, (sockaddr *) &servaddr, sizeof(servaddr)) != 0)
	{
		GetLastErrorInfo();
		return false;
	}
	m_bIsServer = true;
	return true;
}

int CBF_SocketUdp::SendTo(const char *databuf,unsigned int buflen)
{
	return SendTo(m_sSockAddr,databuf,buflen);
}
int CBF_SocketUdp::SendTo(sockaddr_in addr,const char *databuf,unsigned int buflen)
{
	if (buflen < 1)
	{
		m_nErrno = 0;
		bzero(m_sErrMsg,sizeof(m_sErrMsg));
		return -2;
	}

	int nCount = ::sendto(m_socket, databuf, buflen, 0,(sockaddr *)&addr, sizeof(sockaddr_in));
	if (nCount == 0)
	{
#if !defined(HP_UX) && !defined(AIX)
		GetLastErrorInfo();
		return -1;
#endif			
	}
	
	if (nCount == -1)
	{
		GetLastErrorInfo();
		
#ifdef AIX
		if (m_nErrno == EWOULDBLOCK || m_nErrno == 2 || m_nErrno==0)
#elif defined(HP_UX)
			if (m_nErrno == EWOULDBLOCK || m_nErrno==16 || m_nErrno==0 || m_nErrno==2 || m_nErrno==251)
#else
				if (m_nErrno == EWOULDBLOCK)
#endif	
				{
					return 0;
				}
				else
				{
					return -1;
				}
	}
	return nCount;
}

int CBF_SocketUdp::RecvFrom(char *databuf, unsigned int buflen,sockaddr_in &addr)
{
	SOCKADDRLEN nFromLen = sizeof(sockaddr_in);
	int nCount = ::recvfrom(m_socket, databuf, buflen, 0, (sockaddr *)&addr, &nFromLen);
	
	if (nCount == 0)
	{
		return 0;
	}
	if (nCount == -1)
	{
		GetLastErrorInfo();
		if (m_nErrno == EWOULDBLOCK  || m_nErrno == EINTR ||  m_nErrno == 0) 
		{
			return 0;
		}
		else
		{
			return -1;
		}
	}
	return nCount;
}

bool CBF_SocketUdp::BindClient(const char *sip, unsigned int port)
{
//	struct hostent ht;
	unsigned int inaddr;
	bzero(&m_sSockAddr,sizeof(sockaddr_in));
	m_sSockAddr.sin_family = m_nAddressFamily;
	m_nPort = port;
	m_sSockAddr.sin_port = htons(m_nPort);
	strcpy(m_sAddress,sip);

	if (strlen(sip) < 8) //广播
	{
#if defined(_WINDOWS)
		m_sSockAddr.sin_addr.S_un.S_addr = INADDR_BROADCAST;
#endif
#if defined(AIX) || defined(LINUX) || defined(SOLARIS) || defined(HP_UX) 
		m_sSockAddr.sin_addr.s_addr = INADDR_ANY;
#endif
	}
	else //指定IP
	{
		inaddr=inet_addr(m_sAddress);
		if (inaddr == NULL)
		{
			GetLastErrorInfo();
			return false;
		}
#if defined(_WINDOWS)
		m_sSockAddr.sin_addr.S_un.S_addr = inaddr;
#endif
#if defined(AIX) || defined(LINUX) || defined(SOLARIS) || defined(HP_UX) 
		m_sSockAddr.sin_addr.s_addr = inaddr;
#endif	

	}
	m_bIsServer = false;
	return true;
}

void CBF_SocketUdp::Close()
{
	if (m_socket != INVALID_SOCKET )
	{
#if defined(_WINDOWS)
		closesocket(m_socket);
#endif
		
#if defined(AIX) || defined(LINUX) || defined(SOLARIS) || defined(HP_UX) 
		close(m_socket);
#endif
		m_nAddressFamily=AF_INET;
		m_nErrno = 0;
		m_socket = INVALID_SOCKET ;
		bzero(m_sErrMsg,sizeof(m_sErrMsg));
		m_bIsServer = false;
		bzero(m_sAddress,sizeof(m_sAddress));
		m_nPort = 0;
	}
	
}


bool CBF_SocketUdp::IsServer()
{
	return m_bIsServer;
}

unsigned int CBF_SocketUdp::GetPort()
{
	return m_nPort;
}

std::string CBF_SocketUdp::GetHostString()
{
	return m_sAddress;
}

void CBF_SocketUdp::GetSockAddr(sockaddr_in &addr)
{
	memcpy(&addr,&m_sSockAddr,sizeof(sockaddr_in));
}

sockaddr_in & CBF_SocketUdp::GetSockAddr()
{
	return m_sSockAddr;
}

int CBF_SocketUdp::CheckReadEvent(unsigned int tms)
{
	int ret;
	fd_set rset;
	struct timeval tv;
	FD_ZERO(&rset);
	int timeout = tms;
	if (0 == tms )
	{
#ifndef _WINDOWS
		tv.tv_sec = 0;
		tv.tv_usec = 1;
#endif
		FD_SET(m_socket, &rset);//加入到读select集合
#if defined(_WINDOWS)
		ret = select(m_socket+1, &rset, NULL, NULL, &tv);
#else
		ret = select(m_socket+1, &rset, NULL, NULL, &tv);
#endif
		if (ret == 0) //无事件发生，0无事件 <0连接已断
		{
			if (ret <0 )
			{
				GetLastErrorInfo();
			}
			return 0;
		}
		ret = FD_ISSET(m_socket,&rset);
		if (ret != 0) //有读事件
		{
			return 1;
		}
		return 0;
	}
	//进入超时检查
#if defined(_WINDOWS)
	tv.tv_sec = timeout/1000;
	tv.tv_usec = timeout%1000*1000;
#endif
	unsigned long btime = CBF_Date_Time::GetTickCount();//进入超时的开始时间
	//在超时时间内检查
	while( timeout >0 )
	{
		FD_ZERO(&rset);
#ifndef _WINDOWS
		tv.tv_sec = timeout/1000;
		tv.tv_usec = timeout%1000*1000;
#endif
		if (m_socket  <=0) //此种情况一般不会出现
		{
			timeout = tms - (CBF_Date_Time::GetTickCount()-btime);
			continue;
		}
		FD_SET(m_socket, &rset);//加入到读select集合
#if defined(_WINDOWS)
		ret = select(m_socket+1, &rset, NULL, NULL, &tv);
#else
		ret = select(m_socket+1, &rset, NULL, NULL, &tv);
#endif
		if (ret == 0) //无事件发生
		{
			timeout = tms - (CBF_Date_Time::GetTickCount()-btime);
			continue;
		}
		if (ret < 0) // <0连接已断
		{
			GetLastErrorInfo();
			return ret;
		}
		ret = FD_ISSET(m_socket,&rset);
		if (ret != 0) //有读事件
		{
			return 1;
		}
		timeout = tms - (CBF_Date_Time::GetTickCount()-btime);
	}
	return 0;
}