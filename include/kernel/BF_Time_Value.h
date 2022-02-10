// BF_Time_Value.h: interface for the CBF_Time_Value class.
//
//////////////////////////////////////////////////////////////////////

/***********************************************************************************
  ������־��¼
  ������: ������
  �������� 2012-4-1 10:49:33 �ع�
  �汾: V1.00 
  ˵��: ���ļ���Ϊʱ������ֵ
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
	
	// ������: CBF_Time_Value
	// ���  : ������ 2012-3-30 15:23:31
	// ����  : ��
	// ����  : ���캯��,����set(0,0)
	CBF_Time_Value();

	// ������: CBF_Time_Value
	// ���  : ������ 2012-3-30 15:24:05
	// ����  : ��
	// ����  : time_t sec
	// ����  : long usec = 0
	// ����  : ���캯��,����set
	CBF_Time_Value(time_t sec, long usec = 0);
 	CBF_Time_Value(const struct timeval &t);
 	CBF_Time_Value(const timespec_t &t);
 
#if defined (_WINDOWS)
	// ������: CBF_Time_Value
	// ���  : ������ 2012-3-30 15:25:04
	// ����  : ��
	// ����  : const FILETIME &ft
	// ����  : windows�µ�ʹ��FILETIME����
 	CBF_Time_Value(const FILETIME &ft);
#endif 
	virtual ~CBF_Time_Value();

	// ������: Set
	// ���  : ������ 2012-3-30 15:26:45
	// ����  : void 
	// ����  : ����ʱ��ֵ
	void Set(time_t sec, long usec);
 	void Set(const timeval &t);
 	void Set(const timespec_t &t);
 
#if defined (_WINDOWS)

	// ������: Set
	// ���  : ������ 2012-3-30 15:27:25
	// ����  : void set 
	// ����  : const FILETIME &ft
	// ����  : windows�µ�ʹ��FILETIME����
 	void Set(const FILETIME &ft);
#endif /* _WINDOWS */


	// ������: SetMsec
	// ���  : ������ 2012-3-30 15:39:14
	// ����  : void 
	// ����  : ����������ʱ��
	void SetMsec(long);
	void SetMsec(int);  

	// ������: GetMsec
	// ���  : ������ 2012-3-30 15:39:35
	// ����  : void
	// ����  : �õ�ʱ�䣬��λ����
	unsigned long GetMsec(void) const;
    void GetMsec(INT64_ &ms);

	// ������: GetUs
	// ���  : ������ 2012-3-30 15:39:35
	// ����  : void
	// ����  : �õ�ʱ�䣬��λ΢��
	INT64_ GetUs(void) const;
    void GetUs(INT64_ &us);

	// ������: operator
	// ���  : ������ 2012-3-30 15:40:08
	// ����  :  
	// ����  : ����ת��
	operator timespec_t() const;
	operator timeval() const;
	operator const timeval *() const;
#if defined (_WINDOWS)
	operator FILETIME () const;
#endif /* _WINDOWS */


	// ������: GetCurSec
	// ���  : ������ 2012-3-30 15:41:03
	// ����  : time_t
	// ����  : void
	// ����  : �õ���ǰ��
	time_t GetCurSec(void) const;

	// ������: SetCurSec
	// ���  : ������ 2012-3-30 15:41:31
	// ����  : void 
	// ����  : time_t sec
	// ����  : ���õ�ǰ��
	void SetCurSec(time_t sec);

	// ������: GetCurUs
	// ���  : ������ 2012-3-30 15:41:41
	// ����  : long 
	// ����  : void
	// ����  : �õ���ǰ΢��
	long GetCurUs(void) const;

	// ������: SetCurUs
	// ���  : ������ 2012-3-30 15:41:50
	// ����  : void 
	// ����  : long usec
	// ����  : ���õ�ǰ΢��
	void SetCurUs(long usec);

	// ������: operator
	// ���  : ������ 2012-3-30 15:41:50
	// ����  : CBF_Time_Value 
	// ����  : ���ز���
	CBF_Time_Value &operator += (const CBF_Time_Value &tv);
	CBF_Time_Value &operator += (time_t tv);
	CBF_Time_Value &operator = (const CBF_Time_Value &tv);
	CBF_Time_Value &operator = (time_t tv);
	CBF_Time_Value &operator -= (const CBF_Time_Value &tv);
	CBF_Time_Value &operator -= (time_t tv);


	// ������: operator
	// ���  : ������ 2012-3-30 16:00:53
	// ����  : CBF_Time_Value operator++ 
	// ����  : int
	// ����  : ��΢����м�1�����ؼ�1֮ǰ��ֵ
	CBF_Time_Value operator++(int);

	// ������: &operator++
	// ���  : ������ 2012-3-30 16:00:15
	// ����  : CBF_Time_Value 
	// ����  : void
	// ����  : ��΢����м�1������
	CBF_Time_Value &operator++(void);

	// ������: operator--
	// ���  : ������ 2012-3-30 16:03:57
	// ����  : CBF_Time_Value 
	// ����  : int
	// ����  : ��΢����м�1�����ؼ�1֮ǰ��ֵ
	CBF_Time_Value operator--(int);

	// ������: &operator--
	// ���  : ������ 2012-3-30 16:04:18
	// ����  : CBF_Time_Value 
	// ����  : void
	// ����  : ��΢����м�1������
	CBF_Time_Value &operator--(void);

	//����һЩ��������
	//������ֵ���
	friend KERNEL_EXPORT CBF_Time_Value operator+(const CBF_Time_Value &tv1, const CBF_Time_Value &tv2);

	//������ֵ���
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

	// ������: Normalize
	// ���  : ������ 2012-3-30 16:17:00
	// ����  : void  
	// ����  : void
	// ����  : ��ʱ�����ͽ��д�����΢��С��0�����1��
	void Normalize(void);

	timeval m_tv_;

};

#endif // !defined(AFX_BF_TIME_VALUE_H__DB68626D_5167_4BBC_BF80_37843208A6EA__INCLUDED_)
