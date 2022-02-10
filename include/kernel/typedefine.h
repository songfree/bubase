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
		time_t tv_sec;//√Î
		long tv_nsec;//ƒ…√Î
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



#endif // !defined(AFX_TYPEDEFINE_H__08250AC9_FAC7_4943_A67E_CD714F67AF61__INCLUDED_)
