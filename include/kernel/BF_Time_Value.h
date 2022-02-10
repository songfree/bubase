// BF_Time_Value.h: interface for the CBF_Time_Value class.
//
//////////////////////////////////////////////////////////////////////

/***********************************************************************************
  编码日志记录
  编码者: 王明松
  编码日期 2012-4-1 10:49:33 重构
  版本: V1.00 
  说明: 本文件作为时间类型值
 ***********************************************************************************/


#if !defined(AFX_BF_TIME_VALUE_H__DB68626D_5167_4BBC_BF80_37843208A6EA__INCLUDED_)
#define AFX_BF_TIME_VALUE_H__DB68626D_5167_4BBC_BF80_37843208A6EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef KERNEL_EXPORTS
#define KERNEL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNEL_EXPORT __declspec(dllimport)
#else
#define KERNEL_EXPORT 
#endif
#endif

#include "public.h"
#include "typedefine.h"



class KERNEL_EXPORT CBF_Time_Value  
{
public:
	
	// 函数名: CBF_Time_Value
	// 编程  : 王明松 2012-3-30 15:23:31
	// 返回  : 无
	// 描述  : 构造函数,调用set(0,0)
	CBF_Time_Value();

	// 函数名: CBF_Time_Value
	// 编程  : 王明松 2012-3-30 15:24:05
	// 返回  : 无
	// 参数  : time_t sec
	// 参数  : long usec = 0
	// 描述  : 构造函数,调用set
	CBF_Time_Value(time_t sec, long usec = 0);
 	CBF_Time_Value(const struct timeval &t);
 	CBF_Time_Value(const timespec_t &t);
 
#if defined (_WINDOWS)
	// 函数名: CBF_Time_Value
	// 编程  : 王明松 2012-3-30 15:25:04
	// 返回  : 无
	// 参数  : const FILETIME &ft
	// 描述  : windows下的使用FILETIME构造
 	CBF_Time_Value(const FILETIME &ft);
#endif 
	virtual ~CBF_Time_Value();

	// 函数名: Set
	// 编程  : 王明松 2012-3-30 15:26:45
	// 返回  : void 
	// 描述  : 设置时间值
	void Set(time_t sec, long usec);
 	void Set(const timeval &t);
 	void Set(const timespec_t &t);
 
#if defined (_WINDOWS)

	// 函数名: Set
	// 编程  : 王明松 2012-3-30 15:27:25
	// 返回  : void set 
	// 参数  : const FILETIME &ft
	// 描述  : windows下的使用FILETIME设置
 	void Set(const FILETIME &ft);
#endif /* _WINDOWS */


	// 函数名: SetMsec
	// 编程  : 王明松 2012-3-30 15:39:14
	// 返回  : void 
	// 描述  : 按毫秒设置时间
	void SetMsec(long);
	void SetMsec(int);  

	// 函数名: GetMsec
	// 编程  : 王明松 2012-3-30 15:39:35
	// 参数  : void
	// 描述  : 得到时间，单位毫秒
	unsigned long GetMsec(void) const;
    void GetMsec(INT64_ &ms);

	// 函数名: GetUs
	// 编程  : 王明松 2012-3-30 15:39:35
	// 参数  : void
	// 描述  : 得到时间，单位微秒
	INT64_ GetUs(void) const;
    void GetUs(INT64_ &us);

	// 函数名: operator
	// 编程  : 王明松 2012-3-30 15:40:08
	// 返回  :  
	// 描述  : 类型转换
	operator timespec_t() const;
	operator timeval() const;
	operator const timeval *() const;
#if defined (_WINDOWS)
	operator FILETIME () const;
#endif /* _WINDOWS */


	// 函数名: GetCurSec
	// 编程  : 王明松 2012-3-30 15:41:03
	// 返回  : time_t
	// 参数  : void
	// 描述  : 得到当前秒
	time_t GetCurSec(void) const;

	// 函数名: SetCurSec
	// 编程  : 王明松 2012-3-30 15:41:31
	// 返回  : void 
	// 参数  : time_t sec
	// 描述  : 设置当前秒
	void SetCurSec(time_t sec);

	// 函数名: GetCurUs
	// 编程  : 王明松 2012-3-30 15:41:41
	// 返回  : long 
	// 参数  : void
	// 描述  : 得到当前微秒
	long GetCurUs(void) const;

	// 函数名: SetCurUs
	// 编程  : 王明松 2012-3-30 15:41:50
	// 返回  : void 
	// 参数  : long usec
	// 描述  : 设置当前微秒
	void SetCurUs(long usec);

	// 函数名: operator
	// 编程  : 王明松 2012-3-30 15:41:50
	// 返回  : CBF_Time_Value 
	// 描述  : 重载操作
	CBF_Time_Value &operator += (const CBF_Time_Value &tv);
	CBF_Time_Value &operator += (time_t tv);
	CBF_Time_Value &operator = (const CBF_Time_Value &tv);
	CBF_Time_Value &operator = (time_t tv);
	CBF_Time_Value &operator -= (const CBF_Time_Value &tv);
	CBF_Time_Value &operator -= (time_t tv);


	// 函数名: operator
	// 编程  : 王明松 2012-3-30 16:00:53
	// 返回  : CBF_Time_Value operator++ 
	// 参数  : int
	// 描述  : 对微秒进行加1并返回加1之前的值
	CBF_Time_Value operator++(int);

	// 函数名: &operator++
	// 编程  : 王明松 2012-3-30 16:00:15
	// 返回  : CBF_Time_Value 
	// 参数  : void
	// 描述  : 对微秒进行加1并返回
	CBF_Time_Value &operator++(void);

	// 函数名: operator--
	// 编程  : 王明松 2012-3-30 16:03:57
	// 返回  : CBF_Time_Value 
	// 参数  : int
	// 描述  : 对微秒进行减1并返回减1之前的值
	CBF_Time_Value operator--(int);

	// 函数名: &operator--
	// 编程  : 王明松 2012-3-30 16:04:18
	// 返回  : CBF_Time_Value 
	// 参数  : void
	// 描述  : 对微秒进行减1并返回
	CBF_Time_Value &operator--(void);

	//定义一些操作方法
	//两个类值相加
	friend KERNEL_EXPORT CBF_Time_Value operator+(const CBF_Time_Value &tv1, const CBF_Time_Value &tv2);

	//两个类值相减
	friend KERNEL_EXPORT CBF_Time_Value operator-(const CBF_Time_Value &tv1,const CBF_Time_Value &tv2);
	// if tv1 < tv2 return true
	friend KERNEL_EXPORT bool operator<(const CBF_Time_Value &tv1,const CBF_Time_Value &tv2);
	// if tv1 > tv2 return true
	friend KERNEL_EXPORT bool operator>(const CBF_Time_Value &tv1,const CBF_Time_Value &tv2);
	// if tv1 <= tv2 return true
	friend KERNEL_EXPORT bool operator<=(const CBF_Time_Value &tv1, const CBF_Time_Value &tv2);
	// if tv1 >= tv2 return true
	friend KERNEL_EXPORT bool operator>=(const CBF_Time_Value &tv1,const CBF_Time_Value &tv2);
	// if tv1 == tv2 return true
	friend KERNEL_EXPORT bool operator==(const CBF_Time_Value &tv1,const CBF_Time_Value &tv2);
	// if tv1 != tv2 return true
	friend KERNEL_EXPORT bool operator!=(const CBF_Time_Value &tv1,const CBF_Time_Value &tv2);

#if defined(_WINDOWS)
	static const DWORDLONG FILETIME_to_timval_skew;
#endif

private:

	// 函数名: Normalize
	// 编程  : 王明松 2012-3-30 16:17:00
	// 返回  : void  
	// 参数  : void
	// 描述  : 对时间类型进行处理，如微秒小于0或大于1秒
	void Normalize(void);

	timeval m_tv_;

};

#endif // !defined(AFX_BF_TIME_VALUE_H__DB68626D_5167_4BBC_BF80_37843208A6EA__INCLUDED_)
