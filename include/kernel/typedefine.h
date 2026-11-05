// typedefine.h: interface for the Ctypedefine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TYPEDEFINE_H__08250AC9_FAC7_4943_A67E_CD714F67AF61__INCLUDED_)
#define AFX_TYPEDEFINE_H__08250AC9_FAC7_4943_A67E_CD714F67AF61__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



#if defined (_WINDOWS)  
	typedef __int64                  INT64_;
	typedef unsigned __int64         UINT64_;
    #define   ATOI64(val)                 _atoi64(val) ;
	typedef struct timespec_
	{
		time_t tv_sec;//秒
		long tv_nsec;//纳秒
	} _timespec_;
	typedef timespec_ timespec_t;
#else
	typedef long long                INT64_;
	typedef unsigned long long       UINT64_;
	
	typedef timespec timespec_t;
	
	#define   ATOI64(val)                  strtoll(val, NULL, 10);
#endif 

#ifdef _WINDOWS
#define   INT64PFM  "%I64d"
#else
#define   INT64PFM  "%lld"
#endif

typedef unsigned char byte;
typedef unsigned int  UINT32;


// ===================== 原子操作封装 =====================
// 用 __atomic 内建统一语义，替代原来的 CAS 宏两套分支
#if defined(_MSC_VER)
#  include <intrin.h>
static inline UINT64_ AtomicFetchAdd64(UINT64_* p, UINT64_ v)
{
    return (UINT64_)_InterlockedExchangeAdd64((volatile LONG64*)p, (LONG64)v);
}
static inline void AtomicStore64Release(UINT64_* p, UINT64_ v)
{
    _InterlockedExchange64((volatile LONG64*)p, (LONG64)v); // full barrier，x86 上等价 release
}
static inline UINT64_ AtomicLoad64Acquire(UINT64_* p)
{
    UINT64_ v = *(volatile UINT64_*)p;
    _ReadWriteBarrier();
    return v;
}
static inline bool CAS64(UINT64_* p, UINT64_ expected, UINT64_ desired)
{
    return (UINT64_)_InterlockedCompareExchange64(
        (volatile LONG64*)p, (LONG64)desired, (LONG64)expected) == expected;
}
#else
static inline UINT64_ AtomicFetchAdd64(UINT64_* p, UINT64_ v)
{
    return __atomic_fetch_add(p, v, __ATOMIC_RELAXED);
}
static inline void AtomicStore64Release(UINT64_* p, UINT64_ v)
{
    __atomic_store_n(p, v, __ATOMIC_RELEASE);
}
static inline UINT64_ AtomicLoad64Acquire(UINT64_* p)
{
    return __atomic_load_n(p, __ATOMIC_ACQUIRE);
}
static inline bool CAS64(UINT64_* p, UINT64_ expected, UINT64_ desired)
{
    return __atomic_compare_exchange_n(p, &expected, desired, false,
        __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE);
}
#endif

static inline void CpuRelax()
{
#if defined(_MSC_VER)
    _mm_pause();
#elif defined(__x86_64__) || defined(__i386__)
    __builtin_ia32_pause();
#else
    __asm__ __volatile__("" ::: "memory");
#endif
}

#define SHM_DATA_OFFSET 8   // 所有进程统一使用

#endif // !defined(AFX_TYPEDEFINE_H__08250AC9_FAC7_4943_A67E_CD714F67AF61__INCLUDED_)
