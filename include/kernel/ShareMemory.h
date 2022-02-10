/********************************************************************
	created:	2005/07/08
	created:	8:7:2005   19:44
	purpose:	 这里希望提供一种可以在多进程下，使用共享内存块的方法
            为了创建各个进程都使用的共享内存块，必须统一使用一个key_t
            即一个用0xXXXXXXXX表示的关键字，为了简化使用方法，这里就不用
            ftok来获得这个值了。

            CShareMemory共享内存资源使用方法：
            1. 本类是为了提供给一个在同一机器上的不同进程通过共享内存资源
            快速共享一些数据；
            2. 本类使用各个进程使用相同的KEY获得一个相同的共享内存资源；
            3. 获取方法采用Open(...)，得到各个进程访问共享内存映射内存地址shmptr；
            4. 在得到映射内存地址shmptr后，读写数据可以直接通过它进行读写；
            5. 为了避免在同一时刻同时操作共享数据，在必要的时候，可以对共享内存资源
            进行加锁；
            6. 在加锁或解锁的时候，则不能直接用内存映射地址，而是要通过本类的相关实例
            通过Lock()加锁或Unlock()解锁。
            7. 当需要讲共享内存资源从系统中删除，则必须要用本类实例调用Remove(...)，
            删除对应KEY的共享内存资源，如果本实例已经成功打开并且没有被关闭，那么就
            删除本实例的共享内存资源，这个时候就和semkey和shmkey无关。
*********************************************************************/


/// ShareMemory.h: interface for the CShareMemory class.
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHAREMEMORY_H__385E9967_54D4_4A8A_AD31_0445E3EF56CC__INCLUDED_)
#define AFX_SHAREMEMORY_H__385E9967_54D4_4A8A_AD31_0445E3EF56CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif /// _MSC_VER > 1000

#include "public.h"
#ifdef WIN32
#include <windows.h>
typedef int key_t;
typedef HANDLE SHMID;
#define INVALID_ID NULL
#else
#include <sys/shm.h>
typedef int SHMID;
#define INVALID_ID -1
#endif

#ifdef KERNEL_EXPORTS
#define KERNEL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNEL_EXPORT __declspec(dllimport)
#else
#define KERNEL_EXPORT 
#endif
#endif

class KERNEL_EXPORT CShareMemory  
{
public:
	int m_fileno;        /// 用于加锁的文件, 也就是基本文件
	SHMID m_shmid;       /// 共享内存ID
	int m_size;          /// 本共享内存长度
	void *m_address;     /// 共享内存空间在本进程中的地址
	char m_szmsg[256];   /// 在错误的时候，用于存放错误信息

public:
	/// 函数名: Unlock
    /// 编程  : wms 2005-7-9 0:12:45
    /// 描述  : 如果本资源是本进程加锁的，则进行解锁
    /// 返回  : bool: 解锁是否成功，返回false，则用GetMessage()得到错误信息 
	bool Unlock(int offset=0, int len = -1);
	
	/// 函数名: Lock
    /// 编程  : wms 2005-7-9 0:11:47
    /// 描述  : 对本资源进行加锁（阻塞模式）
    /// 返回  : bool: 是否加锁成功，返回false，则用GetMessage()得到错误信息
	bool Lock(int offset=0,int len=-1);


	/// 函数名: Remove
	/// 编程  : 王明松 2007-4-25 11:07:24
	/// 返回  : static bool 
	/// 参数  : const char *name 指定该共享内存的名字
	/// 描述  : 删除指定的共享内存资源
	static bool Remove(const char *name);

	/// 函数名: Open
	/// 编程  : 王明松 2007-4-25 11:08:51
	/// 返回  : void * NULL：打开失败；否则，返回成功打开的共享内存区域的进程映射地址。
	/// 参数  : const char *name 指定该共享内存的名字，当name==NULL时，不作检查或设置名字
	/// 参数  : int shmsize 创建或打开的共享内存大小
	/// 描述  : 用本类实例打开一个进程可保护共享内存区域  注意：在使用本方法的时候，建议设置正确的shmsize;
    ///             注意，这里返回的映射地址是由系统返回的，不要作初试化指定
	void * Open(const char *name, int shmsize);


	/// 函数名: myftok
	/// 编程  : 王明松 2007-4-25 11:07:57
	/// 返回  : static key_t 
	/// 参数  : const char *path
	/// 参数  : int ID
	/// 描述  : 获取关键字
	static key_t myftok(const char *path, int ID);



   /// 函数名: GetSHMPtr
   /// 编程  : wms 2005-7-10 15:13:07
   /// 描述  : 得到本共享内存映射到本进程的内存地址
   /// 返回  : void * ：本共享内存映射的地址，当没有使用Open打开或已经被关闭
   ///          的时候，返回为NULL。

	/// 函数名: GetSHMPtr
	/// 编程  : 王明松 2007-4-25 11:09:54
	/// 返回  : void * 
	/// 描述  : 
	void * GetSHMPtr();

   /// 函数名: GetMessage
   /// 编程  : wms 2005-7-10 15:11:38
   /// 描述  : 当操作本共享内存资源的时候，如果返回错误，可以通过本
   ///         方法得到错误信息
   /// 返回  : char * ：内部处理过程中，最后一次错误信息
	char * GetMessage();

   /// 函数名: Close
   /// 编程  : wms 2005-7-9 0:13:21
   /// 描述  : 关闭本共享资源，从此本进程就不该使用本资源了；
   ///          但是并不从系统中删除该资源
   /// 返回  : void 
	void Close();

   
   
   CShareMemory();
	virtual ~CShareMemory();

private:
	bool locktype(int ltype, unsigned int offset, unsigned int  len);


};
#undef KERNEL_EXPORT
#endif /// !defined(AFX_SHAREMEMORY_H__385E9967_54D4_4A8A_AD31_0445E3EF56CC__INCLUDED_)
