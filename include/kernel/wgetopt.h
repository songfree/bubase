// getopt.h: interface for the getopt class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GETOPT_H__478BD173_DDD3_4613_9AC8_D04E329F5868__INCLUDED_)
#define AFX_GETOPT_H__478BD173_DDD3_4613_9AC8_D04E329F5868__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//声明windows下的全局参数变量,getopt使用
#ifdef _WINDOWS  
extern "C"
{
/* For communication from `getopt' to the caller.
   When `getopt' finds an option that takes an argument,
   the argument value is returned here.
   Also, when `ordering' is RETURN_IN_ORDER,
   each non-option ARGV-element is returned here.  */

char *optarg;

/* Index in ARGV of the next element to be scanned.
   This is used for communication to and from the caller
   and for communication between successive calls to `getopt'.

   On entry to `getopt', zero means this is the first call; initialize.

   When `getopt' returns -1, this is the index of the first of the
   non-option elements that the caller should itself scan.

   Otherwise, `optind' communicates from one call to the next
   how much of ARGV has been scanned so far.  */

int optind;

/* Callers store zero here to inhibit the error message `getopt' prints
   for unrecognized options.  */

int opterr;

};

#endif

#endif // !defined(AFX_GETOPT_H__478BD173_DDD3_4613_9AC8_D04E329F5868__INCLUDED_)
