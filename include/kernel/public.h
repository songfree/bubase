// public.h: interface for the bf.
//
//////////////////////////////////////////////////////////////////////
///目前本文件支持不同的平台，用户可使用_WINDOWS表示microsoft windows的各个
///平台（使用VC 6.0以上编译），也可以使用UNIX表示各种不同的unix平台
///也可以使用VOS表示Stratus的VOS平台(使用g++编译）
///#define _WINDOWS
///#define UNIX
///#define VOS
///#define AIX
///#define LINUX
///#define HP_UX

#if !defined(AFX_PUBLIC_H__BDF683B5_7F3E_47BE_85AC_0007FB217CB1__INCLUDED_)
#define AFX_PUBLIC_H__BDF683B5_7F3E_47BE_85AC_0007FB217CB1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma  warning(disable:4819 4996)
#endif

/////////////////////////////////////////////////////////////////////////
///各个平台都需要包含的文件
/////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <errno.h>
#include <memory.h>
#include <assert.h>
#include <time.h>
#include <math.h>
#include <float.h>
#include <fstream>

/////////////////////////////////////////////////////////////////////////
///STL
/////////////////////////////////////////////////////////////////////////
#ifdef _WINDOWS
#pragma warning(disable:4786)
#endif
#include <algorithm>
#include <map>
#include <set>
#include <list>
#include <deque>
#include <vector>
#include <queue>
#include <string>
#include <stack>
//using namespace std;

/////////////////////////////////////////////////////////////////////////
///Unix所需的宏
/////////////////////////////////////////////////////////////////////////
#if  defined(HP_UX) || defined(AIX) || defined(LINUX) || defined(VOS)
#ifndef UNIX 
#define UNIX
#endif
#endif


/////////////////////////////////////////////////////////////////////////
///打开或者关闭调试模式
/////////////////////////////////////////////////////////////////////////
//#define DEBUG

/////////////////////////////////////////////////////////////////////////
///各个平台特有的头文件
/////////////////////////////////////////////////////////////////////////
#ifdef _WINDOWS
#define FD_SETSIZE 10240

#include <winsock2.h>
#include <io.h>
#include <process.h>
#include <sys/stat.h>
#include <conio.h>
#include <sys/timeb.h>
#include <time.h>
#include <direct.h>
#include <fcntl.h>
#include <sys/types.h>
#include <ws2tcpip.h>
#endif

#ifdef UNIX
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <pthread.h>
#include <sys/timeb.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/syscall.h>
#ifdef LINUX
#include <semaphore.h>
#include <signal.h>
#elif AIX
#include <sys/sem.h>
#else
#include <sys/semaphore.h>
#endif
#endif

#ifdef AIX
#include <strings.h>
#endif
#ifdef HP_UX
#include <strings.h>

/* HP-UX and Solaris */
#ifndef _REENTRANT
#   define _REENTRANT
#endif

#ifndef _POSIX_PTHREAD_SEMANTICS
#   define _POSIX_PTHREAD_SEMANTICS
#endif

//hp_ux 11.23
    //修改HP_UX的/usr/include/stdio.h增加如下注释的代码
    //#define __size_t size_t  
    //#define __va__list va_list  
//hp_ux 11.31
    //songfree add
    //增加如下代码
    #ifndef __size_t
        #if defined __GNUC__ && __GNUC__ >= 2
            typedef __SIZE_TYPE__ __size_t;
            #ifdef __USE_XOPEN
                typedef __SIZE_TYPE__ size_t;
            #endif
        #else
           #include <stddef.h>
           #ifndef __size_t
               #define __size_t size_t
           #endif
        #endif
    #else
    /* The GNU CC stddef.h version defines __size_t as empty.  We need a real
       definition.  */
        #undef __size_t
        #define __size_t size_t
    #endif
    //songfree add end

#endif

#ifdef VOS
#define $shortmap
#define $longmap
#define $memset memset
#include <voslib.h>
#include <c_utilities.h>
#include <errno.h>
#include <streams_utilities.h>
#include <error_codes.h>
#include <tcp_types.h>
#include <socket.h>
#include <tcp_socket.h>
#include <in.h>
#include <ioctl.h>
#define _TYPEDEF_DEV_T
#define _TYPEDEF_OFF_T
#include <sys/stat.h>
#endif


//**********************************************************************
//处理文件的区别
//**********************************************************************
//在Unix下，我们使用stat来获取文件状态
#ifdef UNIX
#define struct_stat struct stat
#define FSTAT    stat
#endif
//在Windows下，我们使用_stat来获取文件状态
#ifdef _WINDOWS
#define struct_stat struct _stat
#define FSTAT    _stat
#endif



//在Unix下，我们使用unlink来删除文件
#ifdef UNIX
#define UNLINK unlink
#define DLL_EXPORT 
#endif
//在Windows下，我们使用_unlink来删除文件
#ifdef _WINDOWS
#define UNLINK _unlink
#endif
///在Vos下，我们使用vos_unlink来删除文件
#ifdef VOS
#define UNLINK vos_unlink
#endif

/////////////////////////////////////////////////////////////////////////
///处理共享内存的区别
/////////////////////////////////////////////////////////////////////////
#ifdef UNIX
#ifndef SHM_FAILED
#define SHM_FAILED (char *)(-1)
#endif
#define ALLOC_SHARE_MEMORY(ptr_addr, id,size)  				\
	{								\
		int shmid = shmget(id,size, IPC_CREAT|0644);		\
		if(shmid<0) 						\
		{							\
			*ptr_addr=NULL;					\
		}							\
		else 							\
		{ 							\
			*ptr_addr=(char *)shmat(shmid,(char *)0,0);	\
			if (*ptr_addr == SHM_FAILED)			\
				*ptr_addr=NULL;				\
		}							\
	}
#define REUSE_SHARE_MEMORY(ptr_addr, id,size)  				\
	{								\
 		int shmid = shmget(id,size, 0644);			\
		if(shmid<0) 						\
		{							\
			*ptr_addr=NULL;					\
		}							\
		else 							\
		{ 							\
			*ptr_addr=(char *)shmat(shmid,(char *)0,0);	\
			if(*ptr_addr == SHM_FAILED)			\
				*ptr_addr=NULL;				\
		}							\
	}
#endif

#ifdef _WINDOWS

#ifdef EXPORTS
	#define DLL_EXPORT __declspec(dllexport)
#else
	#ifdef WIN_DLL
		#define DLL_EXPORT __declspec(dllimport)
	#else
		#define DLL_EXPORT 
	#endif
#endif


#define ALLOC_SHARE_MEMORY(ptr_addr,id,size)				\
	{								\
		HANDLE hfile,hmap;					\
		char buffer[80];					\
		sprintf(buffer,"\\mapping%d.mem",id);			\
		hfile=CreateFile(buffer,GENERIC_READ|GENERIC_WRITE,	\
			FILE_SHARE_READ|FILE_SHARE_WRITE,		\
			NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);	\
		if (hfile==INVALID_HANDLE_VALUE)			\
		{							\
			*ptr_addr=NULL;					\
		}							\
		else							\
		{							\
			hmap=CreateFileMapping(hfile,NULL,		\
				PAGE_READWRITE,0,size,NULL);		\
			if (hmap==NULL)					\
			{						\
				*ptr_addr=NULL;				\
			}						\
			else						\
			{						\
				*ptr_addr=(char *)MapViewOfFile(hmap,	\
					FILE_MAP_WRITE,0,0,size);	\
			}						\
		}							\
	}
#define REUSE_SHARE_MEMORY(ptr_addr,id,size)	ALLOC_SHARE_MEMORY(ptr_addr,id,size)
#endif

#ifdef VOS
#define ALLOC_SHARE_MEMORY(ptr_addr,id,size)	{*ptr_addr=NULL;}
#define REUSE_SHARE_MEMORY(ptr_addr,id,size)	{*ptr_addr=NULL;}
#endif

//在Windows下，使用h_error来获取最近的socket错误，而在其他系统下，直接使用errno。
//我们同意成GET_LAST_SOCK_ERROR()
#ifdef _WINDOWS
#define GET_LAST_SOCK_ERROR()  h_errno
#else
#define GET_LAST_SOCK_ERROR()  errno
#endif

/////////////////////////////////////////////////////////////////////////
///处理线程的区别
/////////////////////////////////////////////////////////////////////////


#ifdef _WINDOWS
#define CREATE_PTHREAD(ptrThreadHandle,ptrThreadId, ptrFunc, ptrArgument)   ( (*ptrThreadHandle = CreateThread(NULL,0, (LPTHREAD_START_ROUTINE)ptrFunc,ptrArgument,0,ptrThreadId))==NULL?0:1)
#endif

#ifdef UNIX
#define CREATE_PTHREAD(ptrThreadHandle,ptrThreadId, ptrFunc, ptrArgument)  (pthread_create(ptrThreadHandle, NULL,ptrFunc,ptrArgument)?0:1)
#endif


//////////////////////////////////////////////////////////////////////////
/// 线程函数的声明不同
//////////////////////////////////////////////////////////////////////////
#ifdef _WINDOWS
#define TheadEntry		\
		DWORD WINAPI _ThreadEntry(LPVOID pParam)
#else
#define TheadEntry		\
		void * _ThreadEntry(void *pParam)
#endif
 
//**********************************************************************
//处理临界区的区别
//**********************************************************************
#ifdef _WINDOWS
#define CRITICAL_VAR		CRITICAL_SECTION
#define INIT_CRITICAL_VAR(v)	InitializeCriticalSection(&(v))
#define ENTER_CRITICAL(v)	EnterCriticalSection(&(v))
#define LEAVE_CRITICAL(v)	LeaveCriticalSection(&(v))
#define DELETE_CRITICAL_VAR(v)	DeleteCriticalSection(&(v))
#endif

#ifdef UNIX
#define CRITICAL_VAR		pthread_mutex_t
#define INIT_CRITICAL_VAR(v)	pthread_mutex_init(&(v), NULL);
#define ENTER_CRITICAL(v)	pthread_mutex_lock(&(v))
#define LEAVE_CRITICAL(v)	pthread_mutex_unlock(&(v))
#define DELETE_CRITICAL_VAR(v)	pthread_mutex_destroy(&(v));
#endif


//**********************************************************************
//打开动态库的区别
//**********************************************************************
#ifdef _WINDOWS
typedef HMODULE DLLHANDLE;
#else
#include <dlfcn.h>
typedef void * DLLHANDLE;
#endif
#ifdef _WINDOWS
#define LOADDLL(a) LoadLibrary(a)
#define GETINSTANCE(a,b) GetProcAddress(a,b)
#define FREEDLL(a) FreeLibrary(a)
#define ISNOTOPEN(b) (b==NULL)
#else
//#define LOADDLL(a) dlopen(a,RTLD_NOW)
#define LOADDLL(a) dlopen(a,RTLD_LAZY | RTLD_GLOBAL)
#define GETINSTANCE(a,b) dlsym(a,b)
#define FREEDLL(a) dlclose(a)
#define ISNOTOPEN(b) (dlerror()!=NULL)
#endif

/////////////////////////////////////////////////////////////////////////
///各种常用函数的区别
/////////////////////////////////////////////////////////////////////////
///在Windows下，我们需要使用Sleep，而不用sleep。我们统一成SLEEP
#ifdef _WINDOWS
#define SLEEP(ms) Sleep(ms)
#elif HP_UX
#define SLEEP(ms) sleep(ms/1000)
#else
#define SLEEP(ms) usleep(ms*1000)
#endif



//在非Windows系统，我们使用close来关闭一个socket，而在Windows下，我们使用closesocket
//这里，我们统一成closesocket
#ifndef _WINDOWS
#define closesocket close
#endif

///Sleep 使用的参数不一样，这里说明了Sleep是以秒为级别的
#ifdef _WINDOWS
#define SLEEP_SECONDS(seconds) Sleep((seconds)*1000)
#else
#define SLEEP_SECONDS(seconds) sleep(seconds)
#endif

///获取当前线程号及进程号
#ifdef _WINDOWS 
#define GetThreadID() ::GetCurrentThreadId()
#define GetPid()      ::GetCurrentProcessId()
#else
//#define GetThreadID() ::pthread_self()
#define GetThreadID() ::syscall(SYS_gettid)
#define GetPid()      ::getpid()
#endif

///等待信号的改变
#ifdef _WINDOWS
#define WaitForObject(handle) (WaitForSingleObject(handle,INFINITE) != 0) 
#else
#define WaitForObject(handle) (pthread_join(handle, NULL) == 0)
#endif

///在Windows下，我们需要重新定义MAX和MIN宏
#ifdef _WINDOWS
#ifndef MAX
#define MAX(a,b) max(a,b)
#endif
#ifndef MIN
#define MIN(a,b) min(a,b)
#endif
#endif
#ifdef UNIX
#define MAX(a,b) (((a)>(b))?(a):(b))
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

///取得当前工作目录的区别
#ifdef _WINDOWS         //取得当前工作目录
#define WORKPATH(WorkDir) GetCurrentDirectory(sizeof(WorkDir),WorkDir);
#endif
#ifdef UNIX         //取得当前工作目录
#define WORKPATH(WorkDir) getcwd(WorkDir,sizeof(WorkDir));//strcpy(WorkDir,(char *)getenv("HOME"));
#endif

///简化memset
#define bzero(ptr, n) 	memset(ptr, 0, n)


#endif // !defined(AFX_PUBLIC_H__BDF683B5_7F3E_47BE_85AC_0007FB217CB1__INCLUDED_)
