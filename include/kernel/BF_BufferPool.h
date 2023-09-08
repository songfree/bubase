// BF_BufferPool.h: interface for the CBF_BufferPool class.
//
//////////////////////////////////////////////////////////////////////

/***********************************************************************************
  编码日志记录
  编码者: 王明松
  编码日期 2012-4-17 10:30:20 重构
  版本: V1.00 
  说明: 缓冲池类，在大并发量时，可以通过此类提高内存分配效率
 ***********************************************************************************/


#if !defined(AFX_BF_BUFFERPOOL_H__FA0DD54E_962B_4184_92E0_6E9D4E28B465__INCLUDED_)
#define AFX_BF_BUFFERPOOL_H__FA0DD54E_962B_4184_92E0_6E9D4E28B465__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "public.h"
#include "BF_Mutex.h"
#include "MdbBase.h"

#ifdef KERNEL_EXPORTS
#define KERNEL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNEL_EXPORT __declspec(dllimport)
#else
#define KERNEL_EXPORT 
#endif
#endif



class KERNEL_EXPORT CBF_BufferPool  
{
public:
	typedef std::stack<void *> POOLSTACK;

	CBF_BufferPool(int mallocsize=8192);
	virtual ~CBF_BufferPool();

	unsigned int  m_nMaxFreeSize;//最大空闲个数，当为0时不限制，当实际空闲大于此值时将释放大于的

public:
	
	/// 函数名: *PoolMalloc
	/// 编程  : 王明松 2009-5-18 17:21:24
	/// 返回  : void *
	/// 描述  : 分配一块内存
	void *PoolMalloc();
	
	/// 函数名: Init
	/// 编程  : 王明松 2009-5-18 17:22:59
	/// 返回  : bool 
	/// 参数  : int size  缓冲区个数
	/// 参数  : int mallocsize  缓冲区大小
	/// 参数  : int maxfreesize  最大空闲缓冲区个数，多于此的释放 等于0为size的100倍
	/// 描述  : 初始化，确定一次分配的内存大小.
	bool Init(int size,int mallocsize=8192,int maxfreesize=0);
	
	/// 函数名: PoolFree
	/// 编程  : 王明松 2009-5-18 17:21:38
	/// 返回  : void 
	/// 参数  : void *trunck
	/// 描述  : 将内存放入堆栈
	void PoolFree(void *trunck);
	
	/// 函数名: PoolInfo
	/// 编程  : 王明松 2009-5-18 17:22:30
	/// 返回  : void 
	/// 参数  : int &total
	/// 参数  : int &unused
	/// 描述  : 返回总共分配的内存数及未使用数
	void PoolInfo(int &total,int &unused);
	
	/// 函数名: PoolInuse
	/// 编程  : 王明松 2009-5-18 17:22:09
	/// 返回  : int  
	/// 描述  : 返回在使用的内存数
	int  PoolInuse();
protected:
	int    m_size;   //内存块的大小
	int    m_nInitSize;//初始化的池子大小
	int    m_total;  //总共分配的内存数量

	void *m_poolmalloc;
	
	POOLSTACK m_stack;//存放空闲的缓冲地址
	CBF_Mutex m_mutex;

//	POOLSTACK m_pTotalBuffer;

	
	CMemTableNew <void *> m_pTotalBuffer_new;  //存放所有的缓存地址
	CPkeyUint64<1> m_pKeyAddr;//所有的缓存地址的索引
};

#endif // !defined(AFX_BF_BUFFERPOOL_H__FA0DD54E_962B_4184_92E0_6E9D4E28B465__INCLUDED_)
