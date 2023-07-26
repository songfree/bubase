// BF_Date_Time.h: interface for the CBF_Date_Time class.
//
//////////////////////////////////////////////////////////////////////
/***********************************************************************************
  ������־��¼
  ������: ������
  �������� 2012-4-1 10:49:33 �ع�
  �汾: V1.00 
  ˵��: ʵ������ʱ��ȵĲ�������
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

//���3200��ǰ�����ڴ�����3200��������




class KERNEL_EXPORT CBF_Date_Time  
{
public:
	CBF_Date_Time();
	virtual ~CBF_Date_Time();

public:

	// ������: GetRemainSecCurDate
	// ���  : ������ 2012-5-14 11:12:43
	// ����  : unsigned int 
	// ����  : ���컹ʣ�µ�����
	unsigned int GetRemainSecCurDate();

	// ������: GetTickUS
	// ���  : ������ 2012-4-9 9:30:58
	// ����  : INT64_ 
	// ����  : ��ȡ΢��ʱ�䣬�����±���ʱ��   %lld %llu   %I64d %I64u
	static INT64_ GetTickUS();

	// ������: GetTickCount
	// ���  : ������ 2012-4-9 9:23:34
	// ����  : unsigned long 
	// ����  : ��ȡ����ʱ��  �����±���ʱ��
	static unsigned long GetTickCount();
	

	// ������: ToCharTime8
	// ���  : ������ 2012-4-6 15:12:49
	// ����  : void 
	// ����  : char *t
	// ����  : תΪ�ַ�ʱ��hh:mm:ss
	void ToCharTime8(char *t);

	// ������: ToCharTime
	// ���  : ������ 2012-4-6 15:12:46
	// ����  : void 
	// ����  : char *hhmmss
	// ����  : תΪ�ַ�ʱ��hhmmss
	void ToCharTime(char *hhmmss);

	// ������: ToCharDate10
	// ���  : ������ 2012-4-6 15:12:42
	// ����  : void 
	// ����  : char *d
	// ����  : ת��Ϊ�ַ�����YYYY-MM-DD
	void ToCharDate10(char *d);

	// ������: ToCharDate
	// ���  : ������ 2012-4-6 15:12:39
	// ����  : void 
	// ����  : char *yyyymmdd
	// ����  : ת��Ϊ�ַ�����YYYYMMDD
	void ToCharDate(char *yyyymmdd);


	// ������: ToStringDT17
	// ���  : ������ 2012-4-17 10:23:06
	// ����  : std::string 
	// ����  : ת��Ϊ����ʱ�� YYYYMMDD hh:mm:ss
	std::string ToStringDT17();

	// ������: ToStringT8
	// ���  : ������ 2012-4-6 14:25:20
	// ����  : std::string 
	// ����  : ת��Ϊ�ַ�ʱ�� HH:MM:SS
	std::string ToStringT8();

	// ������: ToStringT6
	// ���  : ������ 2012-4-6 14:25:02
	// ����  : std::string 
	// ����  : ת��Ϊ�ַ�ʱ�� HHMMSS
	std::string ToStringT6();

	// ������: ToStringD10
	// ���  : ������ 2012-4-6 14:24:48
	// ����  : std::string 
	// ����  : ת��Ϊ�ַ�����YYYY-MM-DD
	std::string ToStringD10();

	// ������: ToStringD8
	// ���  : ������ 2012-4-6 14:24:31
	// ����  : std::string 
	// ����  : ת��Ϊ�ַ�����YYYYMMDD
	std::string ToStringD8();

	// ������: Set
	// ���  : ������ 2012-4-6 14:24:16
	// ����  : void 
	// ����  : const char *yyyymmdd
	// ����  : ��������
	void Set(const char *yyyymmdd);

	// ������: Set
	// ���  : ������ 2012-4-6 13:50:24
	// ����  : void 
	// ����  : unsigned long year
	// ����  : unsigned long month
	// ����  : unsigned long day
	// ����  : ��������
	void Set(unsigned long year,unsigned long month,unsigned long day);

	// ������: IsLeapYear
	// ���  : ������ 2012-4-5 16:24:05
	// ����  : int 1������ 0��
	// ����  : long year
	// ����  : �Ƿ�����
	int IsLeapYear(long year);

	// ������: GetDaysBeginYear
	// ���  : ������ 2012-4-5 16:24:26
	// ����  : long  
	// ����  : long year
	// ����  : long month
	// ����  : long day
	// ����  : �õ������Ǵ���ĵڼ���
	long GetDaysBeginYear(long year,long month,long day);
	long GetDaysBeginYear();

	// ������: GetDaysThisYear
	// ���  : ������ 2012-4-5 16:25:03
	// ����  : long 
	// ����  : long year
	// ����  : �õ������ж�����
	long GetDaysThisYear(long year);

	// ������: GetWeek
	// ���  : ������ 2012-4-5 16:25:18
	// ����  : long 
	// ����  : �õ�����
	long GetWeek();

	// ������: GetUs
	// ���  : ������ 2012-4-5 16:25:29
	// ����  : long 
	// ����  : �õ�΢��
	long GetUs();

	// ������: GetSec
	// ���  : ������ 2012-4-5 16:25:42
	// ����  : long 
	// ����  : �õ���
	long GetSec();

	// ������: GetMinute
	// ���  : ������ 2012-4-5 16:25:49
	// ����  : long 
	// ����  : �õ�����
	long GetMinute();

	// ������: GetHour
	// ���  : ������ 2012-4-5 16:25:58
	// ����  : long 
	// ����  : �õ�Сʱ
	long GetHour();

	// ������: GetMonth
	// ���  : ������ 2012-4-5 16:26:06
	// ����  : long 
	// ����  : �õ���
	long GetMonth();

	// ������: GetDay
	// ���  : ������ 2012-4-5 16:26:15
	// ����  : long 
	// ����  : �õ���
	long GetDay();

	// ������: GetYear
	// ���  : ������ 2012-4-5 16:26:23
	// ����  : long 
	// ����  : �õ���
	long GetYear();

	// ������: operator<
	// ���  : ������ 2012-3-2 10:04:03
	// ����  : bool 
	// ����  : const CBF_Date_Time& dd
	// ����  : �ж������Ƿ�С��dd��ָ������
	bool operator<(const CBF_Date_Time& dd) ;


	// ������: operator=
	// ���  : ������ 2012-4-6 15:39:34
	// ����  : void 
	// ����  : const CBF_Date_Time& dd
	// ����  : ��ֵ
	void operator=(const CBF_Date_Time& dd) ;
	
	// ������: operator>
	// ���  : ������ 2012-3-2 10:04:10
	// ����  : bool 
	// ����  : const CBF_Date_Time& dd
	// ����  : �ж������Ƿ����dd��ָ������
    bool operator>(const CBF_Date_Time& dd) ;
	
	// ������: operator-
	// ���  : ������ 2012-3-2 10:04:14
	// ����  : long 
	// ����  : CBF_Date_Time dd
	// ����  : �жϺ�dd����֮�����������20110406 -20110401 ֮��Ĳ�Ϊ5
	long operator-(CBF_Date_Time dd) ;

	// ������: operator--
	// ���  : ������ 2012-3-2 10:04:14
	// ����  : CBF_Date_Time& 
	// ����  : int i
	// ����  : �õ�ǰһ������
	CBF_Date_Time& operator--(int i) ;
	
	// ������: operator++
	// ���  : ������ 2012-3-2 10:04:14
	// ����  : CBF_Date_Time& 
	// ����  : int i
	// ����  : �õ���һ������
	CBF_Date_Time& operator++(int i) ;

	// ������: &operator+=
	// ���  : ������ 2012-3-2 10:11:06
	// ����  : string yyyymmdd
	// ����  : unsigned int  dd
	// ����  : �������ڼ���dd���õ����ڣ������ڱ仯
	std::string operator+=(unsigned int  dd) ;
	
	// ������: &operator-=
	// ���  : ������ 2012-3-2 10:11:32
	// ����  : string yyyymmdd
	// ����  : unsigned int  dd
	// ����  : �������ڼ���dd���õ����ڣ������ڱ仯
	std::string operator-=(unsigned int  dd) ;


	// ������: Update
	// ���  : ������ 2012-4-1 16:33:09
	// ����  : void 
	// ����  : ��������ʱ��
	void Update();
	void Update(const CBF_Time_Value &tv);

	// ������: Gettimeofday
	// ���  : ������ 2012-4-1 16:33:26
	// ����  : CBF_Time_Value 
	// ����  : ȡʱ��
	static CBF_Time_Value Gettimeofday();

	static void GetTickNS(UINT64_ &us,int &ns);
private:

	// ������: GetMonthDay
	// ���  : ������ 2012-4-6 16:51:24
	// ����  : int 
	// ����  : long year
	// ����  : long month
	// ����  : ȡ���·�����
	int GetMonthDay(long year,long month);

	// ������: NormalMonth
	// ���  : ������ 2012-4-6 16:51:36
	// ����  : void 
	// ����  : �����·ݱ䶯
	void NormalMonth();

	// ������: Normalize
	// ���  : ������ 2012-4-6 14:15:39
	// ����  : void 
	// ����  : ���������ڣ����¼������ڵ�
	void Normalize();

	// ������: GetLocalTime
	// ���  : ������ 2012-4-1 16:33:55
	// ����  : tm* 
	// ����  : time_t* t
	// ����  : tm* result
	// ����  : ȡ��������ʱ��
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
