// BF_Time_Value.cpp: implementation of the CBF_Time_Value class.
//
//////////////////////////////////////////////////////////////////////

#include "BF_Time_Value.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define BF_ONE_SECOND_IN_MSECS 1000
#define BF_ONE_SECOND_IN_USECS 1000000
#define BF_ONE_SECOND_IN_NSECS 1000000000

CBF_Time_Value::CBF_Time_Value()
{
	this->Set(0, 0);
}

CBF_Time_Value::~CBF_Time_Value()
{

}


#if defined (_WINDOWS)
	#define BF_UINT64_LITERAL(n) n ## ui64
	#define BF_INT64_LITERAL(n) n ## i64
	const DWORDLONG CBF_Time_Value::FILETIME_to_timval_skew = BF_INT64_LITERAL (0x19db1ded53e8000);


CBF_Time_Value::CBF_Time_Value(const FILETIME &file_time)
{
	this->Set(file_time);
}

void CBF_Time_Value::Set(const FILETIME &file_time)
{
  
	ULARGE_INTEGER _100ns;
	_100ns.LowPart = file_time.dwLowDateTime;
	_100ns.HighPart = file_time.dwHighDateTime;

	_100ns.QuadPart -= CBF_Time_Value::FILETIME_to_timval_skew;

	
	this->m_tv_.tv_sec = (long) (_100ns.QuadPart / (10000 * 1000));
	
	this->m_tv_.tv_usec = (long) ((_100ns.QuadPart % (10000 * 1000)) / 10);

	this->Normalize ();
}

// Returns the value of the object as a Win32 FILETIME.

CBF_Time_Value::operator FILETIME () const
{
	FILETIME file_time;
  

	ULARGE_INTEGER _100ns;
	_100ns.QuadPart = (((DWORDLONG) this->m_tv_.tv_sec * (10000 * 1000) +
					 this->m_tv_.tv_usec * 10) +
					 CBF_Time_Value::FILETIME_to_timval_skew);

	file_time.dwLowDateTime = _100ns.LowPart;
	file_time.dwHighDateTime = _100ns.HighPart;


  return file_time;
}

#endif /* _WINDOWS */


void CBF_Time_Value::Normalize(void)
{
	if (this->m_tv_.tv_usec >= BF_ONE_SECOND_IN_USECS)
	{
		do
		{
			++this->m_tv_.tv_sec;
			this->m_tv_.tv_usec -= BF_ONE_SECOND_IN_USECS;
		}
		while (this->m_tv_.tv_usec >= BF_ONE_SECOND_IN_USECS);
	}
	else if (this->m_tv_.tv_usec <= -BF_ONE_SECOND_IN_USECS)
	{
		do
		{
			--this->m_tv_.tv_sec;
			this->m_tv_.tv_usec += BF_ONE_SECOND_IN_USECS;
		}
		while (this->m_tv_.tv_usec <= -BF_ONE_SECOND_IN_USECS);
	}

	if (this->m_tv_.tv_sec >= 1 && this->m_tv_.tv_usec < 0)
	{
		--this->m_tv_.tv_sec;
		this->m_tv_.tv_usec += BF_ONE_SECOND_IN_USECS;
	}

}




CBF_Time_Value::CBF_Time_Value (const struct timeval &tv)
{
	this->Set(tv);
}

CBF_Time_Value::CBF_Time_Value(time_t sec, long usec)
{
    this->Set(sec, usec);
	this->Normalize ();
}

void CBF_Time_Value::Set(const timeval &tv)
{
	this->m_tv_.tv_sec = tv.tv_sec;
	this->m_tv_.tv_usec = tv.tv_usec;
	
	this->Normalize ();
}

void CBF_Time_Value::Set(time_t sec, long usec)
{
	this->m_tv_.tv_sec = sec;
	
	this->m_tv_.tv_usec = usec;
	
	this->Normalize ();
}

void CBF_Time_Value::Set(const timespec_t &tv)
{
	this->Set(tv.tv_sec,tv.tv_nsec / 1000); // Convert nanoseconds into microseconds.
}

//得到秒
time_t CBF_Time_Value::GetCurSec(void) const
{
	return this->m_tv_.tv_sec;
}
//设置时间，秒
void CBF_Time_Value::SetCurSec(time_t sec)
{
	this->m_tv_.tv_sec = (long)sec;
}
//得到时间，单位毫秒
unsigned long CBF_Time_Value::GetMsec(void) const
{
	time_t secs = this->m_tv_.tv_sec * 1000 + this->m_tv_.tv_usec / 1000;
	return (unsigned long)secs;
}
//得到时间，单位微秒
INT64_ CBF_Time_Value::GetUs(void) const
{
	return (INT64_)this->m_tv_.tv_sec * 1000000 + this->m_tv_.tv_usec;
}
//得到时间，单位微秒
void CBF_Time_Value::GetUs(INT64_ &us) 
{
	us = (INT64_)(this->m_tv_.tv_sec);
	us *= 1000000;
	us += (this->m_tv_.tv_usec);
}
//按毫秒设置时间
void CBF_Time_Value::SetMsec(long milliseconds)
{
	long secs = milliseconds / 1000;
	this->m_tv_.tv_sec = secs;
	this->m_tv_.tv_usec = (milliseconds - (secs * 1000)) * 1000;
}
//按毫秒设置时间
void CBF_Time_Value::SetMsec(int milliseconds)
{
	long secs = milliseconds / 1000;
	this->m_tv_.tv_sec = secs;
	this->m_tv_.tv_usec = (milliseconds - (secs * 1000)) * 1000;
}

//得到毫秒
void CBF_Time_Value::GetMsec(INT64_ &ms) 
{
	ms = (INT64_)(this->m_tv_.tv_sec);
	ms *= 1000;
	ms += (this->m_tv_.tv_usec / 1000);
}

//得到当前微秒
long CBF_Time_Value::GetCurUs(void) const
{
	return this->m_tv_.tv_usec;
}

// 设置当前微秒
void CBF_Time_Value::SetCurUs(long usec)
{
	this->m_tv_.tv_usec = usec;
}

CBF_Time_Value &CBF_Time_Value::operator++(void)
{
	this->SetCurUs(this->GetCurUs() + 1);
	this->Normalize();
	return *this;
}


CBF_Time_Value CBF_Time_Value::operator--(int)
{
	CBF_Time_Value tv (*this);
	--*this;
	return tv;
}

CBF_Time_Value &CBF_Time_Value::operator--(void)
{
	this->SetCurUs(this->GetCurUs() - 1);
	this->Normalize ();
	return *this;
}

// Returns the value of the object as a timespec_t.
CBF_Time_Value::operator timespec_t () const
{
	timespec_t tv;
	tv.tv_sec = this->GetCurSec();
	// Convert microseconds into nanoseconds.
	tv.tv_nsec = this->m_tv_.tv_usec * 1000;
	return tv;
}

// Initializes the ACE_Time_Value object from a timespec_t.
CBF_Time_Value::CBF_Time_Value (const timespec_t &tv)
{
	this->Set(tv);
}
// Add TV to this.
CBF_Time_Value &CBF_Time_Value::operator+=(const CBF_Time_Value &tv)
{
	this->SetCurSec(this->GetCurSec() + tv.GetCurSec());
	this->SetCurUs(this->GetCurUs() + tv.GetCurUs());
	this->Normalize();
	return *this;
}

CBF_Time_Value &CBF_Time_Value::operator+=(time_t tv)
{
	this->SetCurSec(this->GetCurSec() + tv);
	return *this;
}

CBF_Time_Value &CBF_Time_Value::operator=(const CBF_Time_Value &tv)
{
	this->SetCurSec(tv.GetCurSec());
	this->SetCurUs(tv.GetCurUs());
	return *this;
}

CBF_Time_Value &CBF_Time_Value::operator=(time_t tv)
{
	this->SetCurSec(tv);
	this->SetCurUs(0);
	return *this;
}

// Subtract TV to this.
CBF_Time_Value &CBF_Time_Value::operator-=(const CBF_Time_Value &tv)
{
	this->SetCurSec(this->GetCurSec() - tv.GetCurSec());
	this->SetCurUs(this->GetCurUs() - tv.GetCurUs());
	this->Normalize ();
	return *this;
}

CBF_Time_Value &CBF_Time_Value::operator-= (time_t tv)
{
	this->SetCurSec(this->GetCurSec() - tv);
	return *this;
}

CBF_Time_Value::operator timeval () const
{
	return this->m_tv_;
}

CBF_Time_Value::operator const timeval * () const
{
	return (const timeval *) &this->m_tv_;
}

// True if tv1 > tv2.
bool operator>(const CBF_Time_Value &tv1,const CBF_Time_Value &tv2)
{
	
	if (tv1.GetCurSec() > tv2.GetCurSec())
	{
		return true;
	}
	
	else if (tv1.GetCurSec() == tv2.GetCurSec()  && tv1.GetCurUs() > tv2.GetCurUs())
	{
		return true;
	}
	else
	{
		return false;
	}
}

// True if tv1 >= tv2.
bool operator>=(const CBF_Time_Value &tv1,const CBF_Time_Value &tv2)
{
	if (tv1.GetCurSec() > tv2.GetCurSec())
	{
		return true;
	}
	else if (tv1.GetCurSec() == tv2.GetCurSec()  && tv1.GetCurUs() >= tv2.GetCurUs())
	{
		return true;
	}
	else
	{
		return false;
	}
}
// True if tv1 < tv2.
bool operator<(const CBF_Time_Value &tv1,const CBF_Time_Value &tv2)
{
	return tv2 > tv1;
}

// True if tv1 >= tv2.
bool operator<=(const CBF_Time_Value &tv1,const CBF_Time_Value &tv2)
{
	return tv2 >= tv1;
}

// True if tv1 == tv2.
bool operator==(const CBF_Time_Value &tv1,const CBF_Time_Value &tv2)
{
	return tv1.GetCurSec() == tv2.GetCurSec()   && tv1.GetCurUs() == tv2.GetCurUs();
}

// True if tv1 != tv2.

bool operator!=(const CBF_Time_Value &tv1,const CBF_Time_Value &tv2)
{
	return !(tv1 == tv2);
}



CBF_Time_Value operator+(const CBF_Time_Value &tv1,const CBF_Time_Value &tv2)
{
	CBF_Time_Value sum (tv1);
	sum += tv2;
	return sum;
}


CBF_Time_Value operator-(const CBF_Time_Value &tv1,const CBF_Time_Value &tv2)
{
	CBF_Time_Value delta(tv1);
	delta -= tv2;
	return delta;
}

CBF_Time_Value CBF_Time_Value::operator++(int)
{
	CBF_Time_Value tv (*this);
	++*this;
	return tv;
}

