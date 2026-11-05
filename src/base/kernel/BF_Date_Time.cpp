// BF_Date_Time.cpp: implementation of the CBF_Date_Time class.
//
//////////////////////////////////////////////////////////////////////

#include "BF_Date_Time.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBF_Date_Time::CBF_Date_Time()
{
	Update();
}

CBF_Date_Time::~CBF_Date_Time()
{

}
CBF_Time_Value CBF_Date_Time::Gettimeofday()
{
#if defined (_WINDOWS)
#if defined (LACKS_GETSYSTEMTIMEASFILETIME) //缺少GetSystemTimeAsFileTime
	SYSTEMTIME tsys;
	FILETIME   tfile;
	::GetSystemTime(&tsys);
	::SystemTimeToFileTime(&tsys, &tfile);
	return CBF_Time_Value(tfile);
#else  // if defined (LACKS_GETSYSTEMTIMEASFILETIME)
	FILETIME   tfile;
	::GetSystemTimeAsFileTime(&tfile);
	return CBF_Time_Value(tfile);
#endif  //end  if defined (LACKS_GETSYSTEMTIMEASFILETIME)

#elif defined (AIX)
	int result = 0;
	timeval tv;
	timebasestruct_t tb;
	::read_real_time(&tb, TIMEBASE_SZ);
	::time_base_to_time(&tb, TIMEBASE_SZ);
	tv.tv_sec = tb.tb_high;
	tv.tv_usec = tb.tb_low / 1000L;
#else
	timeval tv;
	int result = 0;
	result = ::gettimeofday(&tv, NULL);

#endif //end  if defined (_WINDOWS)

#ifndef _WINDOWS
	if (result == -1)
	{
		return CBF_Time_Value((time_t)-1);
	}
	else
	{
		return CBF_Time_Value(tv);
	}
#endif // !defined (_WINDOWS)
}

void CBF_Date_Time::Update(const CBF_Time_Value &tv)
{
#if defined(_WINDOWS) && defined(WINCE)
	// CE doesn't do localtime().
	FILETIME file_time = tv;
	FILETIME local_file_time;
	SYSTEMTIME sys_time;
	::FileTimeToLocalFileTime (&file_time, &local_file_time);
	::FileTimeToSystemTime (&local_file_time, &sys_time);
	this->m_day = sys_time.wDay;
	this->m_month = sys_time.wMonth;
	this->m_year = sys_time.wYear;
	this->m_hour = sys_time.wHour;
	this->m_minute = sys_time.wMinute;
	this->m_second = sys_time.wSecond;
	this->m_microsec = sys_time.wMilliseconds * 1000;
	this->m_wday = sys_time.wDayOfWeek;
#else

	time_t timett = tv.GetCurSec();
	struct tm tm_time;
	//基于线程安全的调用
	BFGetLocalTime(&timett, &tm_time);


	this->m_day = tm_time.tm_mday;
	this->m_month = tm_time.tm_mon + 1;    // localtime's months are 0-11
	this->m_year = tm_time.tm_year + 1900; // localtime reports years since 1900
	this->m_hour = tm_time.tm_hour;
	this->m_minute = tm_time.tm_min;
	this->m_second = tm_time.tm_sec;
	this->m_microsec = tv.GetCurUs();
	this->m_wday = tm_time.tm_wday;
#endif 
}

void CBF_Date_Time::Update()
{
#ifdef _WINDOWS
	SYSTEMTIME stm;
	GetLocalTime(&stm);
	m_year = stm.wYear;
	m_month = stm.wMonth;
	m_day = stm.wDay;
	m_hour = stm.wHour;
	m_minute = stm.wMinute;
	m_second = stm.wSecond;
	m_microsec = stm.wMilliseconds;
	m_wday =  stm.wDayOfWeek;
#else
	Update(Gettimeofday());
#endif
}

tm* CBF_Date_Time::BFGetLocalTime(const time_t* t,tm* result) 
{
#ifdef _WINDOWS
	tm* res = ::localtime(t);
	memcpy(result,res,sizeof(tm));
#else
//	result = std::localtime(t);
	localtime_r(t,result);
#endif
	return result;
}

bool CBF_Date_Time::operator<(const CBF_Date_Time& dd) 
{
	if (m_year < dd.m_year)
	{
		return true;
	}
	else if (m_year == dd.m_year)
	{
		if (m_month < dd.m_month)
		{
			return true;
		}
		else if (m_month == dd.m_month)
		{
			if (m_day < dd.m_day)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	return false;
}

bool CBF_Date_Time::operator>(const CBF_Date_Time& dd) 
{
	if (m_year > dd.m_year)
	{
		return true;
	}
	else if (m_year == dd.m_year)
	{
		if (m_month > dd.m_month)
		{
			return true;
		}
		else if (m_month == dd.m_month)
		{
			if (m_day > dd.m_day)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	return false;
}
long CBF_Date_Time::operator-(CBF_Date_Time dd)
{
	int days=0;
	if (m_year == dd.GetYear())
	{
		days = GetDaysBeginYear() - dd.GetDaysBeginYear(); 
		return days;
	}
	else if (m_year > dd.GetYear()) //大于
	{
		for (int i=dd.GetYear();i<m_year;i++)
		{
			days+=GetDaysThisYear(i); //sample:dd 2001-this2012 11年的总共天数
		}
		days+=GetDaysBeginYear();  //加上2012年的天数
		days-=dd.GetDaysBeginYear();//减去2001的天数
		return days;
	}
	else //小于
	{
		for (int i=GetYear();i<dd.GetYear();i++)
		{
			days+=GetDaysThisYear(i);
		}
		days = 0-days; 
		days+=dd.GetDaysBeginYear();
		days-=GetDaysBeginYear();
		return days;
	}
}
void CBF_Date_Time::operator=(const CBF_Date_Time& dd)
{
	m_year = dd.m_year;
	m_month = dd.m_month;
	m_day = dd.m_day;
	m_hour = dd.m_hour;
	m_minute = dd.m_minute;
	m_second = dd.m_second;
	m_microsec = dd.m_microsec;
	m_wday = dd.m_wday;
}

std::string CBF_Date_Time::operator+=(unsigned int  dd)
{
	m_day+=dd;
	Normalize();
	return ToStringD8();
}
std::string CBF_Date_Time::operator-=(unsigned int  dd)
{
	m_day-=dd;
	Normalize();
	return ToStringD8();
}

long CBF_Date_Time::GetYear()
{
	return m_year;
}

long CBF_Date_Time::GetDay()
{
	return m_day;
}

long CBF_Date_Time::GetMonth()
{
	return m_month;
}

long CBF_Date_Time::GetHour()
{
	return m_hour;
}

long CBF_Date_Time::GetMinute()
{
	return m_minute;
}

long CBF_Date_Time::GetSec()
{
	return m_second;
}

long CBF_Date_Time::GetUs()
{
	return m_microsec;
}

long CBF_Date_Time::GetWeek()
{
	return m_wday;
}

long CBF_Date_Time::GetDaysThisYear(long year)
{
	if (IsLeapYear(year))
	{
		return 366;
	}
	else
	{
		return 365;
	}
}
long CBF_Date_Time::GetDaysBeginYear()
{
	return GetDaysBeginYear(m_year,m_month,m_day);
}
long CBF_Date_Time::GetDaysBeginYear(long year,long month,long day)
{
	int i=0;
	int days=0;
	for (i=1;i<month;i++)
	{
		switch(i)
		{
			case 1:
			case 3:
			case 5:
			case 7:
			case 8:
			case 10:
			case 12:
				days+=31;
				break;
			case 2:
				days+=28+IsLeapYear(year);
				break;
			case 4:
			case 6:
			case 9:
			case 11:
				days+=30; 
				break;
			default:
				days+=0;
		}
	}
	return days+day;
}

int CBF_Date_Time::IsLeapYear(long year)
{
	if (year%4 == 0 && year%100 != 0 || year%400 == 0)
	{
		return 1;//闰年
	}
	else
	{
		return 0;
	}
}

void CBF_Date_Time::Set(unsigned long year, unsigned long month, unsigned long day)
{
// 	
// 	if (year >= 3200)
// 	{
// 		return;
// 	}
// 	if (month > 12)
// 	{
// 		return;
// 	}
// 	if (day > 31)
// 	{
// 		return;
// 	}
	m_day = day;
	m_year = year;
	m_month = month;
	Normalize();
}

void CBF_Date_Time::Set(const char *yyyymmdd)
{
	char tmpstr[5];
	long year;
	long month;
	long day;
	bzero(tmpstr,sizeof(tmpstr));
	memcpy(tmpstr,yyyymmdd,4);
	year = atol(tmpstr);
	bzero(tmpstr,sizeof(tmpstr));
	memcpy(tmpstr,yyyymmdd+4,2);
	month = atol(tmpstr);
	bzero(tmpstr,sizeof(tmpstr));
	memcpy(tmpstr,yyyymmdd+6,2);
	day = atol(tmpstr);
	Set(year,month,day);
}

std::string CBF_Date_Time::ToStringD8()
{
	char sdate[9];
	sprintf(sdate,"%04ld%02ld%02ld",m_year,m_month,m_day);
	return sdate;
}
std::string CBF_Date_Time::ToStringDT17()
{
	char sdate[18];
	sprintf(sdate,"%04ld%02ld%02ld %02ld:%02ld:%02ld",m_year,m_month,m_day,m_hour,m_minute,m_second);
	return sdate;
}
std::string CBF_Date_Time::ToStringD10()
{
	char sdate[11];
	sprintf(sdate,"%04ld-%02ld-%02ld",m_year,m_month,m_day);
	return sdate;
}


std::string CBF_Date_Time::ToStringT6()
{
	char stime[7];
	sprintf(stime,"%02ld%02ld%02ld",m_hour,m_minute,m_second);
	return stime;
}

std::string CBF_Date_Time::ToStringT8()
{
	char stime[9];
	sprintf(stime,"%02ld:%02ld:%02ld",m_hour,m_minute,m_second);
	return stime;
}

void CBF_Date_Time::Normalize()
{
	long days=0;
	long dnum;
	//月是否合法
	NormalMonth();
	//日是否合法
	dnum = GetMonthDay(m_year,m_month);
	while (true)
	{
		if (m_day > 0 && m_day <= dnum)
		{
			break;//正常了，可以返回
		}
		//要做处理
		if (m_day >0)
		{
			m_day-=dnum;
			m_month++;
			NormalMonth();
			dnum = GetMonthDay(m_year,m_month);//取下一个月的天数
		}
		else if (0 == m_day) //日为0
		{
			m_month--;
			NormalMonth();
			m_day = GetMonthDay(m_year,m_month);
			dnum = m_day;
		}
		else //日小于0
		{
			m_month--;
			NormalMonth();
			dnum = GetMonthDay(m_year,m_month);
			m_day+=dnum;
		}
	}
	//计算星期
	if (m_year>=2012)
	{
		for (int i=2012;i<m_year;i++)
		{
			days +=GetDaysThisYear(i);
		}
		days += GetDaysBeginYear(m_year,m_month,m_day);
		m_wday = (days-1)%7;
		
	}
	else //小于2012年
	{
		days += GetDaysBeginYear(m_year,m_month,m_day);
		for (int i=m_year+1;i<2012;i++)
		{
			days +=GetDaysThisYear(i);
		}
		m_wday = (days-2)%7;
	}
}

void CBF_Date_Time::ToCharDate(char *yyyymmdd)
{
	sprintf(yyyymmdd,"%04ld%02ld%02ld",m_year,m_month,m_day);
}

void CBF_Date_Time::ToCharDate10(char *d)
{
	sprintf(d,"%04ld-%02ld-%02ld",m_year,m_month,m_day);
}

void CBF_Date_Time::ToCharTime(char *hhmmss)
{
	sprintf(hhmmss,"%02ld%02ld%02ld",m_hour,m_minute,m_second);
}

void CBF_Date_Time::ToCharTime8(char *t)
{
	sprintf(t,"%02ld:%02ld:%02ld",m_hour,m_minute,m_second);
}

void CBF_Date_Time::NormalMonth()
{
	long ynum;
	long mnum;
	if (m_month <1 )
	{
		ynum = (0-m_month)/12; 
		mnum = (0-m_month)%12;
		m_year-=ynum; 
		if (0 == mnum)
		{
			m_year--;
			m_month = 12;
		}
		else
		{
			m_month = mnum;
		}
		
	}
	if (m_month >12)
	{
		ynum = (m_month)/12; 
		mnum = (m_month)%12;
		m_year+=ynum; 
		if (0 == mnum)
		{
			m_year--;
			m_month = 12;
		}
		else
		{
			m_month = mnum;
		}
	}
}

int CBF_Date_Time::GetMonthDay(long year,long month)
{
	switch(month)
	{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			return 31;
		case 2:
			return 28+IsLeapYear(year);
			
		case 4:
		case 6:
		case 9:
		case 11:
			return 30;
			break;
		default:
			return 0;
	}
}
CBF_Date_Time& CBF_Date_Time::operator--(int i)
{
	m_day--;
	Normalize();
	return *this;
}
CBF_Date_Time& CBF_Date_Time::operator++(int i)
{
	m_day++;
	Normalize();
	return *this;
}

unsigned long CBF_Date_Time::GetTickCount()
{
#ifdef _WINDOWS
	return ::GetTickCount();
#else
// 	struct timeval CurrentTime;
// 	::gettimeofday(&CurrentTime, NULL);
// 	return ((CurrentTime.tv_sec * 1000) + (CurrentTime.tv_usec / 1000));   
	return Gettimeofday().GetMsec();
#endif
}

INT64_ CBF_Date_Time::GetTickUS()
{
	
#if defined (_WINDOWS)

	FILETIME   tfile;
#if defined (LACKS_GETSYSTEMTIMEASFILETIME) //缺少GetSystemTimeAsFileTime
	SYSTEMTIME tsys;
	::GetSystemTime(&tsys);
	::SystemTimeToFileTime(&tsys, &tfile);
#else  // if defined (LACKS_GETSYSTEMTIMEASFILETIME)
	::GetSystemTimeAsFileTime(&tfile);
#endif  //end  if defined (LACKS_GETSYSTEMTIMEASFILETIME)
	ULARGE_INTEGER _100ns;
	_100ns.LowPart = tfile.dwLowDateTime;
	_100ns.HighPart = tfile.dwHighDateTime;
	
	_100ns.QuadPart -= CBF_Time_Value::FILETIME_to_timval_skew;
	return (INT64_)_100ns.QuadPart/10+(_100ns.QuadPart % (10000 * 1000)) / 10;
	
#elif defined (AIX)
	int result = 0;
	timeval tv;
	timebasestruct_t tb;
	::read_real_time(&tb, TIMEBASE_SZ);
	::time_base_to_time(&tb, TIMEBASE_SZ);
	tv.tv_sec = tb.tb_high;
	tv.tv_usec = tb.tb_low / 1000L;
	return (INT64_)tv.tv_sec*1000000+tv.tv_usec;
#else
	timeval tv;
	int result = 0;
	result = ::gettimeofday(&tv, NULL);
	return (INT64_)tv.tv_sec*1000000+tv.tv_usec;
	
#endif //end  if defined (_WINDOWS)

//	return Gettimeofday().GetUs();
}

unsigned int CBF_Date_Time::GetRemainSecCurDate()
{
	return 24*60*60-m_hour*60*60-m_minute*60 - m_second;
}

void CBF_Date_Time::GetTickNS(UINT64_& us,int& ns)
{
#ifdef LINUX
	struct timespec  time_start;
    clock_gettime(CLOCK_REALTIME, &time_start);
	us = (UINT64_)time_start.tv_sec*1000000+ time_start.tv_nsec/1000;
	ns = time_start.tv_nsec%1000;
#else
	ns=0;
	us = GetTickUS();
#endif
}
