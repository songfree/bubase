// BF_Date_Time.h: interface for the CBF_Date_Time class.
//
//////////////////////////////////////////////////////////////////////
/***********************************************************************************
  编码日志记录
  编码者: 王明松
  编码日期 2012-4-1 10:49:33 重构
  版本: V1.00 
  说明: 实现日期时间等的操作方法
 ***********************************************************************************/

#if !defined(AFX_BF_DATE_TIME_H__759A17FB_CA08_4C15_B07E_CD66109BFA3A__INCLUDED_)
#define AFX_BF_DATE_TIME_H__759A17FB_CA08_4C15_B07E_CD66109BFA3A__INCLUDED_

// MS VC++ 10.0 _MSC_VER = 1600  Visual C++ 2010
// MS VC++ 9.0  _MSC_VER = 1500  Visual C++ 2008
// MS VC++ 8.0  _MSC_VER = 1400  Visual C++ 2005
// MS VC++ 7.1  _MSC_VER = 1310
// MS VC++ 7.0  _MSC_VER = 1300
// MS VC++ 6.0  _MSC_VER = 1200
// MS VC++ 5.0  _MSC_VER = 1100 

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "BF_Time_Value.h"


#ifdef KERNEL_EXPORTS
#define KERNEL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNEL_EXPORT __declspec(dllimport)
#else
#define KERNEL_EXPORT 
#endif
#endif

//针对3200年前的日期处理，因3200不是闰年




class KERNEL_EXPORT CBF_Date_Time  
{
public:
	CBF_Date_Time();
	virtual ~CBF_Date_Time();

public:

	// 函数名: GetRemainSecCurDate
	// 编程  : 王明松 2012-5-14 11:12:43
	// 返回  : unsigned int 
	// 描述  : 当天还剩下的秒数
	unsigned int GetRemainSecCurDate();

	// 函数名: GetTickUS
	// 编程  : 王明松 2012-4-9 9:30:58
	// 返回  : INT64_ 
	// 描述  : 获取微秒时间，不更新本类时间   %lld %llu   %I64d %I64u
	static INT64_ GetTickUS();

	// 函数名: GetTickCount
	// 编程  : 王明松 2012-4-9 9:23:34
	// 返回  : unsigned long 
	// 描述  : 获取毫秒时间  不更新本类时间
	static unsigned long GetTickCount();
	

	// 函数名: ToCharTime8
	// 编程  : 王明松 2012-4-6 15:12:49
	// 返回  : void 
	// 参数  : char *t
	// 描述  : 转为字符时间hh:mm:ss
	void ToCharTime8(char *t);

	// 函数名: ToCharTime
	// 编程  : 王明松 2012-4-6 15:12:46
	// 返回  : void 
	// 参数  : char *hhmmss
	// 描述  : 转为字符时间hhmmss
	void ToCharTime(char *hhmmss);

	// 函数名: ToCharDate10
	// 编程  : 王明松 2012-4-6 15:12:42
	// 返回  : void 
	// 参数  : char *d
	// 描述  : 转换为字符日期YYYY-MM-DD
	void ToCharDate10(char *d);

	// 函数名: ToCharDate
	// 编程  : 王明松 2012-4-6 15:12:39
	// 返回  : void 
	// 参数  : char *yyyymmdd
	// 描述  : 转换为字符日期YYYYMMDD
	void ToCharDate(char *yyyymmdd);


	// 函数名: ToStringDT17
	// 编程  : 王明松 2012-4-17 10:23:06
	// 返回  : std::string 
	// 描述  : 转换为日期时间 YYYYMMDD hh:mm:ss
	std::string ToStringDT17();

	// 函数名: ToStringT8
	// 编程  : 王明松 2012-4-6 14:25:20
	// 返回  : std::string 
	// 描述  : 转换为字符时间 HH:MM:SS
	std::string ToStringT8();

	// 函数名: ToStringT6
	// 编程  : 王明松 2012-4-6 14:25:02
	// 返回  : std::string 
	// 描述  : 转换为字符时间 HHMMSS
	std::string ToStringT6();

	// 函数名: ToStringD10
	// 编程  : 王明松 2012-4-6 14:24:48
	// 返回  : std::string 
	// 描述  : 转换为字符日期YYYY-MM-DD
	std::string ToStringD10();

	// 函数名: ToStringD8
	// 编程  : 王明松 2012-4-6 14:24:31
	// 返回  : std::string 
	// 描述  : 转换为字符日期YYYYMMDD
	std::string ToStringD8();

	// 函数名: Set
	// 编程  : 王明松 2012-4-6 14:24:16
	// 返回  : void 
	// 参数  : const char *yyyymmdd
	// 描述  : 设置日期
	void Set(const char *yyyymmdd);

	// 函数名: Set
	// 编程  : 王明松 2012-4-6 13:50:24
	// 返回  : void 
	// 参数  : unsigned long year
	// 参数  : unsigned long month
	// 参数  : unsigned long day
	// 描述  : 设置日期
	void Set(unsigned long year,unsigned long month,unsigned long day);

	// 函数名: IsLeapYear
	// 编程  : 王明松 2012-4-5 16:24:05
	// 返回  : int 1是闰年 0否
	// 参数  : long year
	// 描述  : 是否闰年
	int IsLeapYear(long year);

	// 函数名: GetDaysBeginYear
	// 编程  : 王明松 2012-4-5 16:24:26
	// 返回  : long  
	// 参数  : long year
	// 参数  : long month
	// 参数  : long day
	// 描述  : 得到日期是此年的第几天
	long GetDaysBeginYear(long year,long month,long day);
	long GetDaysBeginYear();

	// 函数名: GetDaysThisYear
	// 编程  : 王明松 2012-4-5 16:25:03
	// 返回  : long 
	// 参数  : long year
	// 描述  : 得到此年有多少天
	long GetDaysThisYear(long year);

	// 函数名: GetWeek
	// 编程  : 王明松 2012-4-5 16:25:18
	// 返回  : long 
	// 描述  : 得到星期
	long GetWeek();

	// 函数名: GetUs
	// 编程  : 王明松 2012-4-5 16:25:29
	// 返回  : long 
	// 描述  : 得到微秒
	long GetUs();

	// 函数名: GetSec
	// 编程  : 王明松 2012-4-5 16:25:42
	// 返回  : long 
	// 描述  : 得到秒
	long GetSec();

	// 函数名: GetMinute
	// 编程  : 王明松 2012-4-5 16:25:49
	// 返回  : long 
	// 描述  : 得到分钟
	long GetMinute();

	// 函数名: GetHour
	// 编程  : 王明松 2012-4-5 16:25:58
	// 返回  : long 
	// 描述  : 得到小时
	long GetHour();

	// 函数名: GetMonth
	// 编程  : 王明松 2012-4-5 16:26:06
	// 返回  : long 
	// 描述  : 得到月
	long GetMonth();

	// 函数名: GetDay
	// 编程  : 王明松 2012-4-5 16:26:15
	// 返回  : long 
	// 描述  : 得到日
	long GetDay();

	// 函数名: GetYear
	// 编程  : 王明松 2012-4-5 16:26:23
	// 返回  : long 
	// 描述  : 得到年
	long GetYear();

	// 函数名: operator<
	// 编程  : 王明松 2012-3-2 10:04:03
	// 返回  : bool 
	// 参数  : const CBF_Date_Time& dd
	// 描述  : 判断日期是否小于dd所指的日期
	bool operator<(const CBF_Date_Time& dd) ;


	// 函数名: operator=
	// 编程  : 王明松 2012-4-6 15:39:34
	// 返回  : void 
	// 参数  : const CBF_Date_Time& dd
	// 描述  : 赋值
	void operator=(const CBF_Date_Time& dd) ;
	
	// 函数名: operator>
	// 编程  : 王明松 2012-3-2 10:04:10
	// 返回  : bool 
	// 参数  : const CBF_Date_Time& dd
	// 描述  : 判断日期是否大于dd所指的日期
    bool operator>(const CBF_Date_Time& dd) ;
	
	// 函数名: operator-
	// 编程  : 王明松 2012-3-2 10:04:14
	// 返回  : long 
	// 参数  : CBF_Date_Time dd
	// 描述  : 判断和dd日期之间的天数，如20110406 -20110401 之间的差为5
	long operator-(CBF_Date_Time dd) ;

	// 函数名: operator--
	// 编程  : 王明松 2012-3-2 10:04:14
	// 返回  : CBF_Date_Time& 
	// 参数  : int i
	// 描述  : 得到前一天日期
	CBF_Date_Time& operator--(int i) ;
	
	// 函数名: operator++
	// 编程  : 王明松 2012-3-2 10:04:14
	// 返回  : CBF_Date_Time& 
	// 参数  : int i
	// 描述  : 得到后一天日期
	CBF_Date_Time& operator++(int i) ;

	// 函数名: &operator+=
	// 编程  : 王明松 2012-3-2 10:11:06
	// 返回  : string yyyymmdd
	// 参数  : unsigned int  dd
	// 描述  : 将本日期加上dd天后得到日期，本日期变化
	std::string operator+=(unsigned int  dd) ;
	
	// 函数名: &operator-=
	// 编程  : 王明松 2012-3-2 10:11:32
	// 返回  : string yyyymmdd
	// 参数  : unsigned int  dd
	// 描述  : 将本日期减上dd天后得到日期，本日期变化
	std::string operator-=(unsigned int  dd) ;


	// 函数名: Update
	// 编程  : 王明松 2012-4-1 16:33:09
	// 返回  : void 
	// 描述  : 更新日期时间
	void Update();
	void Update(const CBF_Time_Value &tv);

	// 函数名: Gettimeofday
	// 编程  : 王明松 2012-4-1 16:33:26
	// 返回  : CBF_Time_Value 
	// 描述  : 取时间
	static CBF_Time_Value Gettimeofday();

	static void GetTickNS(UINT64_ &us,int &ns);
private:

	// 函数名: GetMonthDay
	// 编程  : 王明松 2012-4-6 16:51:24
	// 返回  : int 
	// 参数  : long year
	// 参数  : long month
	// 描述  : 取得月份天数
	int GetMonthDay(long year,long month);

	// 函数名: NormalMonth
	// 编程  : 王明松 2012-4-6 16:51:36
	// 返回  : void 
	// 描述  : 处理月份变动
	void NormalMonth();

	// 函数名: Normalize
	// 编程  : 王明松 2012-4-6 14:15:39
	// 返回  : void 
	// 描述  : 更新了日期，重新计算星期等
	void Normalize();

	// 函数名: GetLocalTime
	// 编程  : 王明松 2012-4-1 16:33:55
	// 返回  : tm* 
	// 参数  : time_t* t
	// 参数  : tm* result
	// 描述  : 取本地日期时间
	tm* BFGetLocalTime(const time_t* t, tm* result);

	long m_day;
	long m_month;
	long m_year;
	long m_hour;
	long m_minute;
	long m_second;
	long m_microsec;
	long m_wday;

	
};

#endif // !defined(AFX_BF_DATE_TIME_H__759A17FB_CA08_4C15_B07E_CD66109BFA3A__INCLUDED_)
