// BF_SocketTcp.cpp: implementation of the CBF_SocketTcp class.
//
//////////////////////////////////////////////////////////////////////

#include "BF_SocketTcp.h"
#include "BF_Date_Time.h"


/**
对Tcp连结的初始化
*/
class  CTcpInit
{
public:
	/**构造函数
	*对Windows系统下的Tcp连结的初始化
	*/
	CTcpInit();
   ~CTcpInit();
};


static CTcpInit _TCP_INIT;
CTcpInit::CTcpInit()
{
#ifdef _WINDOWS
    WORD wVersionRequested = MAKEWORD( 2, 2 );
	WSADATA wsaData;

	if (WSAStartup( wVersionRequested, &wsaData) != 0)
	{
		printf("can not init socket!\n");
	}
#endif
}

CTcpInit::~CTcpInit()
{
#ifdef _WINDOWS
   WSACleanup();
#endif
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBF_SocketTcp::CBF_SocketTcp(int addressFamily)
{
	m_nAddressFamily=addressFamily;
	m_bConnected = false;
	m_nSocketType = BF_SOCKET_ATTACH_UNKNOW;
	m_nErrno = 0;
	m_socket = INVALID_SOCKET ;
	memset(m_sErrMsg,0,sizeof(m_sErrMsg));
	memset(m_sAddress,0,sizeof(m_sAddress));
	m_nSendBlockSize =  (m_nAddressFamily == AF_INET?TCP_BLOCK_SIZE:UNIX_BLOCK_SIZE); 
	m_nPort = 0;    //BF_SOCKET_CLIENT:服务端侦听端口  BF_SOCKET_SERVER:侦听端口        
	
}

CBF_SocketTcp::~CBF_SocketTcp()
{	
	if (m_nSocketType != BF_SOCKET_ATTACH)
	{
		Close();
	}
}

bool CBF_SocketTcp::Create(int AddressFamily, bool blocked)
{
    m_nAddressFamily=AddressFamily;
#if defined(_WINDOWS)
	if (AF_UNIX == m_nAddressFamily)
	{
		return false;
	}
#endif
	//修改针对ipv6的支持
    switch(m_nAddressFamily)
	{
		case AF_INET:
		case AF_INET6:
		case AF_UNSPEC:	
		case AF_UNIX:
			if ((m_socket = socket(m_nAddressFamily,SOCK_STREAM,0)) <0 )  //IPPROTO_IP
			{
				GetLastErrorInfo();
				return false;
			}
			break;
		default:
			return false;
	}
	SetBlockFlag(blocked);
	return true; 
}

void CBF_SocketTcp::GetLastErrorInfo()
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

SOCKET_HANDLE CBF_SocketTcp::GetSocket()
{
	return m_socket;
}

int CBF_SocketTcp::GetErrorNo()
{
	return m_nErrno;
}

std::string CBF_SocketTcp::GetErrorMsg()
{
	return m_sErrMsg;
}
int CBF_SocketTcp::GetSocketMode()
{
	return m_nAddressFamily;
}

unsigned int CBF_SocketTcp::GetSocketType()
{
	return m_nSocketType;
}

std::string CBF_SocketTcp::GetAddress()
{
	return m_sAddress;
}

void CBF_SocketTcp::Close()
{
	if (m_socket != INVALID_SOCKET )
	{
#if defined(_WINDOWS)
		closesocket(m_socket);
#endif
		
#if defined(AIX) || defined(LINUX) || defined(SOLARIS) || defined(HP_UX) 
		close(m_socket);
#endif
		m_socket=INVALID_SOCKET;
		if (m_nSocketType == BF_SOCKET_CLIENT )
		{
			m_bConnected = false;
		}
	}
	
}

unsigned short CBF_SocketTcp::GetPort()
{
	return m_nPort;
}

bool CBF_SocketTcp::Listen(unsigned short port, unsigned int queuenum, const char* localip)
{
	long blockflag=0;
	long reuseflag=1;
	struct sockaddr_in iaddr;
	struct sockaddr_un uaddr;
	int ret;
	m_nSocketType = BF_SOCKET_SERVER;
	m_nPort = port;
	
	SetBlockFlag(false);

	switch( m_nAddressFamily)
	{
		case AF_INET:
		case AF_INET6:
		case AF_UNSPEC:	   //增加ipv6的支持
			memset( &iaddr, 0,sizeof(iaddr) );
			iaddr.sin_family = m_nAddressFamily;
			if (localip == NULL)
			{
				iaddr.sin_addr.s_addr = htonl(INADDR_ANY);
			}
			else
			{
				inet_pton(m_nAddressFamily, localip, &iaddr.sin_addr);
			}
			iaddr.sin_port = htons(port);
#if defined(_WINDOWS)
			setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&reuseflag, sizeof(reuseflag));
			ioctlsocket(m_socket,FIONBIO,(unsigned long*)&blockflag);
#endif
		
#if defined(AIX) || defined(LINUX) || defined(SOLARIS) || defined(HP_UX) 
			setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &reuseflag, sizeof(reuseflag));
			setsockopt(m_socket, SOL_SOCKET, SO_REUSEPORT, &reuseflag, sizeof(reuseflag));
// 			struct linger so_linger;
// 			so_linger.l_onoff = 1;
// 			so_linger.l_linger = 0;
// 			setsockopt(m_socket,SOL_SOCKET,SO_LINGER,&so_linger,sizeof(so_linger));
			ioctl(m_socket,FIONBIO,(unsigned int*)&blockflag);
#endif
			ret = bind(m_socket, (struct sockaddr*)&iaddr, sizeof(iaddr));
			if(ret<0) 
			{
				GetLastErrorInfo();
				return false;
			}
			break;
		case AF_UNIX:
			memset( &uaddr, 0,sizeof(uaddr) );
			uaddr.sun_family = AF_UNIX;
			sprintf(uaddr.sun_path,"/tmp/tcpsocket%d",port);
			strcpy(m_sAddress,uaddr.sun_path);
#if defined(AIX) || defined(LINUX) || defined(SOLARIS) || defined(HP_UX) 
			unlink(uaddr.sun_path);
#endif
#if defined(_WINDOWS)
			ioctlsocket(m_socket,FIONBIO,(unsigned long*)&blockflag);
#endif
			
#if defined(AIX) || defined(LINUX) || defined(SOLARIS) || defined(HP_UX) 
			ioctl(m_socket,FIONBIO,(unsigned int*)&blockflag);
#endif
			ret = bind(m_socket, (struct sockaddr*)&uaddr, sizeof(uaddr));
			if(ret<0) 
			{
				GetLastErrorInfo();
				return false;
			}
			break;
		default:
			return false;
	}
	
	if( listen( m_socket, queuenum ) < 0 ) 
	{
		GetLastErrorInfo();
		return false;
	}
	return true;
}

SOCKET_HANDLE CBF_SocketTcp::Accept(char *lpaddress, bool blocked)
{
	SOCKET_HANDLE tempsocket=INVALID_SOCKET;
	int keepflag=1;
#if defined(_WINDOWS)
	int addrlen;
#endif
	
#if defined(SCO_UNIX)
	int addrlen;
#else
#if defined(AIX) || defined(LINUX) || defined(SOLARIS) || defined(HP_UX) 
	socklen_t addrlen;
#endif
#endif
	
	
	if(m_nAddressFamily==AF_UNIX) 
	{
		struct sockaddr_un addr;
		addrlen = sizeof(addr);
		tempsocket = accept( m_socket, (struct sockaddr*)&addr, &addrlen);
		if(tempsocket < 0)
		{
			GetLastErrorInfo();
			return INVALID_SOCKET;
		}
		strcpy(lpaddress,"127.0.0.1");
	}
	else		  //增加对ipv6的支持
	{
		struct sockaddr fromaddr;
		addrlen = sizeof(fromaddr);
		tempsocket = accept(m_socket,&fromaddr, &addrlen);
		if (tempsocket < 0)
		{
			GetLastErrorInfo();
			return INVALID_SOCKET;
		}
		void* numeric_addr = NULL;
		char addr_buff[66];
		int port;
		if (AF_INET == fromaddr.sa_family)
		{
			//numeric_addr = &((struct sockaddr_in*)&fromaddr)->sin_addr;
			port = ntohs(((struct sockaddr_in*)&fromaddr)->sin_port);
			sprintf(lpaddress, "%s:%d", inet_ntoa(((struct sockaddr_in*)&fromaddr)->sin_addr), port);
		}
		else if (AF_INET6 == fromaddr.sa_family)
		{
			numeric_addr = &((struct sockaddr_in6*)&fromaddr)->sin6_addr;
			port = ntohs(((struct sockaddr_in6*)&fromaddr)->sin6_port);
			inet_ntop(AF_INET6, numeric_addr, addr_buff, 65);
			sprintf(lpaddress, "%s:%d", addr_buff, port);
		}
		else
		{
			sprintf(lpaddress, "127.0.0.1");
		}
		
	}
	int blockflag;
	if (blocked)
	{
		blockflag=0;
	}
	else
	{
		blockflag=1;
	}
#if defined(_WINDOWS)
	ioctlsocket(tempsocket,FIONBIO,(unsigned long*)&blockflag);
#endif
#if defined(AIX) || defined(LINUX) || defined(SOLARIS) || defined(HP_UX)
	ioctl(tempsocket,FIONBIO,(unsigned int*)&blockflag);
#endif

//	SetBlockFlag(blocked);
	setsockopt(tempsocket,SOL_SOCKET,SO_KEEPALIVE, (char *)&keepflag, sizeof(keepflag));
	setsockopt(tempsocket, IPPROTO_TCP, TCP_NODELAY, (char*)&keepflag, sizeof(keepflag));
	return tempsocket;
}
SOCKET_HANDLE CBF_SocketTcp::Accept(SOCKADDR_IN &iaddr, bool blocked)
{
	SOCKET_HANDLE tempsocket=INVALID_SOCKET;
	int keepflag=1;
#if defined(_WINDOWS)
	int addrlen;
#endif
	
#if defined(SCO_UNIX)
	int addrlen;
#else
#if defined(AIX) || defined(LINUX) || defined(SOLARIS) || defined(HP_UX) 
	socklen_t addrlen;
#endif
#endif
	
	if(m_nAddressFamily == AF_INET) 
	{
		iaddr.sin_family=m_nAddressFamily;
		addrlen = sizeof(iaddr);
		tempsocket = accept( m_socket, (struct sockaddr*)&iaddr, &addrlen);
		if(tempsocket < 0)
		{
			GetLastErrorInfo();
			return INVALID_SOCKET;
		}
	}
	else
	{
		if(m_nAddressFamily==AF_UNIX) 
		{
			struct sockaddr_un addr;
			addrlen = sizeof(addr);
			tempsocket = accept( m_socket, (struct sockaddr*)&addr, &addrlen);
			if(tempsocket < 0)
			{
				GetLastErrorInfo();
				return INVALID_SOCKET;
			}
			iaddr.sin_port = m_nPort;
#if defined(LINUX)
			
			iaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
			//iaddr.sin_addr = inet_addr("127.0.0.1");
#endif
			iaddr.sin_family = AF_UNIX;
		}
	}
	int blockflag;
	if (blocked)
	{
		blockflag=0;
	}
	else
	{
		blockflag=1;
	}
#if defined(_WINDOWS)
	ioctlsocket(tempsocket,FIONBIO,(unsigned long*)&blockflag);
#endif
#if defined(AIX) || defined(LINUX) || defined(SOLARIS) || defined(HP_UX)
	ioctl(tempsocket,FIONBIO,(unsigned int*)&blockflag);
#endif
//	SetBlockFlag(blocked);
	setsockopt(tempsocket,SOL_SOCKET,SO_KEEPALIVE, (char *)&keepflag, sizeof(keepflag));
	setsockopt(tempsocket, IPPROTO_TCP, TCP_NODELAY, (char*)&keepflag, sizeof(keepflag));
	return tempsocket;
}
void CBF_SocketTcp::AttachSocket(SOCKET_HANDLE socket, const char *clientip)
{
	m_socket = socket;
	strcpy(m_sAddress,clientip);
	m_nSocketType = BF_SOCKET_ATTACH;
}
void CBF_SocketTcp::AttachBindSocket(SOCKET_HANDLE socket, const char *clientip)
{
	m_socket = socket;
	strcpy(m_sAddress,clientip);
	m_nSocketType = BF_SOCKET_ATTACHBIND;
}
void CBF_SocketTcp::SetBlockFlag(bool isblock)
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

bool CBF_SocketTcp::ConnectServer(const char *host, unsigned short port, unsigned int tms)
{
	if (m_socket == INVALID_SOCKET )
	{
		return false;//请先Create
	}
	m_nSocketType = BF_SOCKET_CLIENT;
	strcpy(m_sAddress,host);
	m_nPort = port;
	m_bConnected = false;
	if (0 == tms ) //阻塞式连接
	{
		return Connect();
	}
	//超时非阻塞连接
	SetBlockFlag(false);
	if (Connect()) //连接成功，这种情况会有，但一般不在广域网出现,局域网或本机是可能的
	{
		return true;
	}
	//根据socket出错代码来判断
	if ((m_nErrno == EWOULDBLOCK) || (m_nErrno == EINPROGRESS))
	{
		if(!CheckConnect(tms))
		{
			Close();
			m_bConnected = false;
			return false;
		}
		m_bConnected = true;
		return m_bConnected;
	} //end if ((m_nErrno == EWOULDBLOCK) || (m_nErrno == EINPROGRESS))
	//出错了，没连接上
	Close();
	m_bConnected = false;
	return false;
}

bool CBF_SocketTcp::Connect()
{
	struct hostent ht;
	unsigned int inaddr;
	struct sockaddr_in iaddr;
	struct sockaddr_in6 iaddr6;
	struct sockaddr_un uaddr;
	int flag=1;
	m_bConnected = false;
	switch(m_nAddressFamily)
	{
		case AF_INET:
			memset(&iaddr, 0, sizeof(iaddr));
			iaddr.sin_family = AF_INET;
			if ((inaddr=inet_addr(m_sAddress)) == INADDR_NONE ) 
			{
				if (gethostbyname(m_sAddress) == NULL)
				{
					GetLastErrorInfo();
					return false;
				}
				memcpy(&ht,gethostbyname(m_sAddress),sizeof(ht));
				memcpy(&(iaddr.sin_addr),ht.h_addr,ht.h_length);
				iaddr.sin_family=ht.h_addrtype;
			}
			else
			{
#if defined(_WINDOWS)
				iaddr.sin_addr.S_un.S_addr = inaddr;
#endif
			
#if defined(AIX) || defined(LINUX) || defined(SOLARIS) || defined(HP_UX) 
				iaddr.sin_addr.s_addr = inaddr;
#endif		
			}
			iaddr.sin_port = htons(m_nPort);
		
			if (connect(m_socket, (struct sockaddr*)&iaddr, sizeof (iaddr)) <0) 
			{
				GetLastErrorInfo();
				return false;
			}
		
			break;
		case AF_UNSPEC:
		case AF_INET6:
			memset(&iaddr6, 0, sizeof(iaddr6));
			iaddr6.sin6_family = AF_INET6;
			if (inet_pton(AF_INET6, m_sAddress, (void*)&iaddr6.sin6_addr) == INADDR_NONE)
			{
				struct addrinfo hints, * res;
				bzero(&hints, sizeof(hints));
				hints.ai_family = AF_INET6;
				hints.ai_socktype = SOCK_STREAM;
				hints.ai_protocol = IPPROTO_IP;
				hints.ai_flags = 0;
				if (getaddrinfo(m_sAddress,NULL,&hints,&res) !=0 )
				{
					GetLastErrorInfo();
					return false;
				}
				memcpy(&iaddr6, &res->ai_addr, res->ai_addrlen);
				iaddr6.sin6_port = htons(m_nPort);
				iaddr6.sin6_family = AF_INET6;
			}
			else
			{
				iaddr6.sin6_port = htons(m_nPort);
			}
			if (connect(m_socket, (struct sockaddr*)&iaddr6, sizeof(iaddr6)) < 0)
			{
				GetLastErrorInfo();
				return false;
			}

			break;
		case AF_UNIX:
			memset(&uaddr, 0, sizeof(uaddr));
			uaddr.sun_family = AF_UNIX;
			sprintf(uaddr.sun_path,"/tmp/tcpsocket%d",m_nPort);
			if (connect(m_socket, (struct sockaddr*)&uaddr, sizeof (uaddr)) <0) 
			{
				GetLastErrorInfo();
				return false;
			}
			break;
		default:
			GetLastErrorInfo();
			return false;
	}
	setsockopt(m_socket, SOL_SOCKET,  SO_KEEPALIVE, (char *)&flag, sizeof(flag));
	setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag));
	m_bConnected = true;
	return m_bConnected;
}

bool CBF_SocketTcp::Connect(SOCKADDR_IN iaddr)
{
	struct sockaddr_un uaddr;
	int flag=1;
	m_bConnected = false;
	switch(m_nAddressFamily)
	{
	     case AF_INET6:
		case AF_INET:
			if (connect(m_socket, (struct sockaddr*)&iaddr, sizeof (SOCKADDR_IN)) <0) 
			{
				GetLastErrorInfo();
				return false;
			}
			break;
		case AF_UNIX:
			memset(&uaddr, 0, sizeof(uaddr));
			uaddr.sun_family = AF_UNIX;
			sprintf(uaddr.sun_path,"/tmp/tcpsocket%d",iaddr.sin_port);
			if (connect(m_socket, (struct sockaddr*)&uaddr, sizeof (uaddr)) <0) 
			{
				GetLastErrorInfo();
				return false;
			}
			break;
		default:
			GetLastErrorInfo();
			return false;
	}
	setsockopt(m_socket, SOL_SOCKET,  SO_KEEPALIVE, (char *)&flag, sizeof(flag));
	setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag));
	m_bConnected = true;
	return m_bConnected;
}

bool CBF_SocketTcp::CheckConnect(unsigned int tms)
{
	int flag=1;
	//在超时间时间内检查连接是否正常
	struct timeval tv;
	int result;
	fd_set rset;
	fd_set wset;
	int timeout = tms;
	
#if defined(_WINDOWS)
	int ret;
	fd_set eset;
	tv.tv_sec = timeout/1000;
	tv.tv_usec = timeout%1000*1000;
#endif
	if (0 ==  tms) //只检查一次
	{
		FD_ZERO(&rset);
		FD_ZERO(&wset);
#if defined(_WINDOWS)
		FD_ZERO(&eset);
		FD_SET(m_socket, &eset);
#endif
		if (m_socket  <=0) //此种情况一般不会出现
		{
			return false;
		}
		FD_SET(m_socket, &rset);//加入到读select集合
		FD_SET(m_socket, &wset);//加入到读select集合

#if defined(_WINDOWS)
		result = select(m_socket+1, &rset, &wset, &eset, NULL);
#else
		result = select(m_socket+1, &rset, &wset, NULL, NULL);
#endif
		if (result == 0) //无事件发生
		{
			return false;
		}
		//socket有事件发生，要根据具体情况来判断
		//检查是否可写
		if (m_socket == INVALID_SOCKET) //此种情况应该不可出现
		{
			return false;
		}
#if defined(_WINDOWS)
		//WINDOWS下的处理
		ret = FD_ISSET(m_socket, &eset);
		if (ret != 0)
		{
			return false;
		}
		else if (FD_ISSET(m_socket, &wset) !=0 )
		{
			//连接成功
			m_bConnected = true;
			setsockopt(m_socket,SOL_SOCKET,SO_KEEPALIVE, (char *)&flag, sizeof(flag));
			setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag));
			return true;
		}
#else
		//UNIX下的处理
		int ok=1;
		int oklen;
		if (FD_ISSET(m_socket, &wset) !=0)
		{
			oklen=sizeof(ok);
#if defined(HP_UX)
			getsockopt(m_socket,SOL_SOCKET,SO_ERROR,(char *)&ok,(socklen_t *)&oklen);
#else	
			getsockopt(m_socket,SOL_SOCKET,SO_ERROR,(char *)&ok,(socklen_t *)&oklen);
#endif
			if (ok == 0)
			{
				//连接成功
				m_bConnected = true;
				setsockopt(m_socket,SOL_SOCKET,SO_KEEPALIVE, (char *)&flag, sizeof(flag));
				return true;
			}
			else
			{
				//连接未成功
				Close();
				m_bConnected = false;
				return false;
			} 
		}  //end  if (FD_ISSET(m_socket, &wset) !=0)
		return false;
#endif  
	} // if (0 ==  tms) //只检查一次

	//进入超时检查
	unsigned long btime = CBF_Date_Time::GetTickCount();//进入超时的开始时间
	//在超时时间内检查
	while( timeout >0 )
	{
		FD_ZERO(&rset);
		FD_ZERO(&wset);
#if defined(_WINDOWS)
		FD_ZERO(&eset);
		FD_SET(m_socket, &eset);
#endif
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
		FD_SET(m_socket, &wset);//加入到读select集合
#if defined(_WINDOWS)
		result = select(m_socket+1, &rset, &wset, &eset, &tv);
#else
		result = select(m_socket+1, &rset, &wset, NULL, &tv);
#endif
		if (result == 0) //无事件发生
		{
			timeout = tms - (CBF_Date_Time::GetTickCount()-btime);
			continue;
		}
		//socket有事件发生，要根据具体情况来判断
		//检查是否可写
		if (m_socket == INVALID_SOCKET) //此种情况应该不可出现
		{
			m_bConnected = false;
			return false;
		}
#if defined(_WINDOWS)
		//WINDOWS下的处理
		ret = FD_ISSET(m_socket, &eset);
		if (ret != 0)
		{
			//m_log.LogMp(2,__FILE__,__LINE__,"尝试连接到%s:%d未成功 index=%d",m_pSocketMgr->at(conindex)->m_s_center.sAddress,m_pSocketMgr->at(conindex)->m_s_center.nPort,conindex);
			Close();
			return false;
		}
		else if (FD_ISSET(m_socket, &wset) !=0 )
		{
			//连接成功
			m_bConnected = true;
			setsockopt(m_socket,SOL_SOCKET,SO_KEEPALIVE, (char *)&flag, sizeof(flag));
			return true;
		}
#else
		//UNIX下的处理
		int ok=1;
		int oklen;
		if (FD_ISSET(m_socket, &wset) !=0)
		{
			oklen=sizeof(ok);
#if defined(HP_UX)
			getsockopt(m_socket,SOL_SOCKET,SO_ERROR,(char *)&ok,(socklen_t *)&oklen);
#else	
			getsockopt(m_socket,SOL_SOCKET,SO_ERROR,(char *)&ok,(socklen_t *)&oklen);
#endif
			if (ok == 0)
			{
				//连接成功
				m_bConnected = true;
				setsockopt(m_socket,SOL_SOCKET,SO_KEEPALIVE, (char *)&flag, sizeof(flag));
				return true;
			}
			else
			{
				//连接未成功
				Close();
				m_bConnected = false;
				return false;
			} 
		}  //end  if (FD_ISSET(m_socket, &wset) !=0)
#endif  
		//socket不可写，继续超时判断
		timeout = tms - (CBF_Date_Time::GetTickCount()-btime);
		continue;
	} //end while( timeout >0 )
	Close();
	return false;
}

int CBF_SocketTcp::CheckReadEvent(unsigned int tms)
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
		if (ret <= 0) //无事件发生，0无事件 <0连接已断
		{
 			if (ret <0 )
 			{
 				GetLastErrorInfo();
				return -1;
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

int CBF_SocketTcp::CheckWriteEvent(unsigned int tms)
{
	int ret;
	fd_set wset;
	struct timeval tv;
	FD_ZERO(&wset);
	int timeout = tms;
#if defined(_WINDOWS)
	tv.tv_sec = 0;
	tv.tv_usec =1;
#endif

	if (0 == tms )
	{
		FD_SET(m_socket, &wset);//加入到写select集合
#if defined(_WINDOWS)
		ret = select(m_socket+1, NULL, &wset, NULL, &tv);
#else
		ret = select(m_socket+1, NULL, &wset, NULL, &tv);
#endif
		if (0 == ret) //无事件发生,处理心跳
		{
			return 0;
		}
 		if ( ret <0 )
 		{
 			GetLastErrorInfo();
 			return ret;
 		}
 		ret = FD_ISSET(m_socket,&wset);
 		if (ret != 0) //可写
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
		FD_ZERO(&wset);
#ifndef _WINDOWS
		tv.tv_sec = timeout/1000;
		tv.tv_usec = timeout%1000*1000;
#endif
		if (m_socket  <=0) //此种情况一般不会出现
		{
			timeout = tms - (CBF_Date_Time::GetTickCount()-btime);
			continue;
		}
		FD_SET(m_socket, &wset);//加入到写select集合
#if defined(_WINDOWS)
		ret = select(m_socket+1, NULL, &wset, NULL, &tv);
#else
		ret = select(m_socket+1, NULL, &wset, NULL, &tv);
#endif
		if (ret == 0) //无事件发生
		{
			timeout = tms - (CBF_Date_Time::GetTickCount()-btime);
			continue;
		}
 		if ( ret <0 )
 		{
 			GetLastErrorInfo();
 			return ret;
 		}
 		ret = FD_ISSET(m_socket,&wset);
 		if (ret != 0) //可写
 		{
 			return 1;
 		}
 		timeout = tms - (CBF_Date_Time::GetTickCount()-btime);
	}
	return 0;
}

int CBF_SocketTcp::GetReceiveDataLen()
{
	int datalen=0;
	int ret = CheckReadEvent(0);
	if (ret == 1)
	{
#if defined(_WINDOWS)
		ioctlsocket(m_socket,FIONREAD,(unsigned long*)&datalen);
#endif
#if defined(AIX) || defined(LINUX) || defined(SOLARIS) || defined(HP_UX)
		ioctl(m_socket,FIONREAD,(unsigned int*)&datalen);
#endif
		if (datalen == 0)
		{
			return -1;
		}
		return datalen;
	}
	if (ret <0)
	{
		return -2;
	}
	return ret;
}

int CBF_SocketTcp::Receive(char *buf, unsigned int &wantlen, unsigned int tms)
{
	if (m_socket == INVALID_SOCKET) 
	{
		return -1;
	}
	if(wantlen == 0)
	{
		return 0;
	}
	int result;
	int RecvLen = 0;
	int curgetlen;
	int timeout = tms;
	unsigned long  btime  = CBF_Date_Time::GetTickCount();

	curgetlen= wantlen-RecvLen; //剩下没收的长度
	while( RecvLen < (int)wantlen )
	{
		//检查是否可读
		result = CheckReadEvent(timeout);
		if (result<0)
		{
			wantlen = RecvLen;
			return -1;  //连接已断开,返回收到的数据
		}
		if (result == 0) //无数据可读
		{
			//在超时时间内无数据可读，可以退出了
			wantlen = RecvLen;
			return RecvLen; 
		}
		//收
		result = recv(m_socket, buf+RecvLen, curgetlen, 0);
		if (result < 0 )
		{
			GetLastErrorInfo();
			//不管是阻塞还是非阻塞，EINTR是遇到系统中断 EWOULDBLOCK是操作系统阻塞  ==0 HP_IA64会有
			if (m_nErrno == EWOULDBLOCK  || m_nErrno == EINTR ||  m_nErrno == 0) 
			{
				result = 0;
			}
			else //连接出错了，可关闭
			{
				wantlen = RecvLen;
				return -1;
			}
		}
		
		RecvLen+=result;
		if (RecvLen == wantlen) //收到指定长度的数据了
		{
			return RecvLen;
		}
		
		curgetlen= wantlen-RecvLen; //剩下没收的长度
		//下次检查可读时的超时
		timeout = tms - (CBF_Date_Time::GetTickCount() - btime);
	}
	return RecvLen;
}

int CBF_SocketTcp::Receive(char *buf, unsigned int bufsize)
{
	int recvlen;
	if ( 0 == bufsize ) 
	{
		return 0;
	}
	recvlen=recv(m_socket,buf,bufsize,0);
	if(recvlen <=0 ) 
	{
		GetLastErrorInfo();
		if (recvlen <0)
		{
			//不管是阻塞还是非阻塞，EINTR是遇到系统中断 EWOULDBLOCK是操作系统阻塞  ==0 HP_IA64会有
			if (m_nErrno == EWOULDBLOCK  || m_nErrno == EINTR ||  m_nErrno == 0) 
			{
				return 0;
			}
			return -1;
		}
		return 0;
	}
	return recvlen;
}

int CBF_SocketTcp::SendFrame(const char *databuf, unsigned int &datalen)
{
	if(m_socket == INVALID_SOCKET) 
	{
		return 0;
	}
	if( 0 == datalen) 
	{
		return 0;
	}
	int nleft=datalen;

	int nwriten;
	int everysendlen=0;//每次发送长度
	int offset=0; //位置
	while( nleft > 0 )
	{
		if( nleft >= (int)m_nSendBlockSize ) //发送数据比较大，分开发
		{
			everysendlen = m_nSendBlockSize;
		}
		else
		{
			everysendlen=nleft;
		}
		nwriten = send(m_socket,databuf+offset,everysendlen,0);
		if (nwriten < 0) 
		{
			GetLastErrorInfo();
#ifdef AIX
			if (m_nErrno == EWOULDBLOCK || m_nErrno == 2 || m_nErrno ==0 )
#elif defined(HP_UX)
			if (m_nErrno == EWOULDBLOCK || m_nErrno == 16 || m_nErrno == 0 || m_nErrno == 2 || m_nErrno == 251)
#else
			if (m_nErrno == EWOULDBLOCK)
#endif			
			{
				//不能发送，可能是对方缓冲区满
				//返回已发送的数据长度
				datalen = datalen - nleft;
				return datalen;	
			}
			datalen = datalen - nleft;
			return -1;
		}
		if (nwriten == 0) //不能发送，可能是缓冲区满
		{
			//返回已发送的数据长度
			datalen = datalen - nleft;
			return datalen;	
		}
		nleft -= nwriten;
		offset += nwriten;
	}
	datalen = datalen - nleft;
	return datalen;	
		
}

int CBF_SocketTcp::Send(const char *databuf, unsigned int datalen)
{
	if(m_socket == INVALID_SOCKET) 
	{
		return 0;
	}
	if( 0 == datalen) 
	{
		return 0;
	}
	int sendlen;
	sendlen = send(m_socket,databuf,datalen,0);
	if (sendlen < 0) 
	{
		GetLastErrorInfo();
#ifdef AIX
		if (m_nErrno == EWOULDBLOCK || m_nErrno == 2 || m_nErrno ==0 )
#elif defined(HP_UX)
		if (m_nErrno == EWOULDBLOCK || m_nErrno == 16 || m_nErrno == 0 || m_nErrno == 2 || m_nErrno == 251)
#else
		if (m_nErrno == EWOULDBLOCK)
#endif			
		{
			//不能发送，可能是对方缓冲区满
			return 0;	
		}
		//有问题了，可能要关闭连接
		return -1;
	}
	if (sendlen == 0)
	{
		//不能发送，可能是缓冲区满
		return 0;
	}
	return sendlen;
}

bool CBF_SocketTcp::GetConnectStatus()
{
	return m_bConnected;
}
