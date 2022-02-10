// BF_Slist.h: interface for the CBF_Slist class.
//
//////////////////////////////////////////////////////////////////////

/***********************************************************************************
  ������־��¼
  ������: ������
  �������� 2012-4-17 11:00:31 �ع�
  �汾: V1.00 
  ˵��: ����ָ�����������
 ***********************************************************************************/


#if !defined(AFX_BF_SLIST_H__34A62F79_4DB7_495B_9479_09886538A78D__INCLUDED_)
#define AFX_BF_SLIST_H__34A62F79_4DB7_495B_9479_09886538A78D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "public.h"
#include "BF_Tools.h"

#ifdef KERNEL_EXPORTS
#define KERNEL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNEL_EXPORT __declspec(dllimport)
#else
#define KERNEL_EXPORT 
#endif
#endif

#define  SLIST_BUFFER_SIZE   8192   //�ָ�����ָ���������󳤶�


class KERNEL_EXPORT CBF_Slist  
{
public:
	CBF_Slist(const char *sep="|");

	virtual ~CBF_Slist();
	
public:

	// ������: ToStringException
	// ���  : ������ 2012-4-18 14:38:20
	// ����  : std::string 
	// ����  : unsigned int index  Ҫȥ��������
	// ����  : unsigned int flag=1   ��־  1 a|b|c| 0 a|b|c 3 |a|b|c| 2 |a|b|c
	// ����  : ��������ɷָ����ֳɵ��ַ��� ȥ��ָ��������
	std::string ToStringException(unsigned int index,unsigned int flag=1);

	// ������: ToString
	// ���  : ������ 2012-4-18 12:27:31
	// ����  : std::string 
	// ����  : unsigned int flag ��־  1 a|b|c| 0 a|b|c 3 |a|b|c| 2 |a|b|c
	// ����  : ��������ɷָ����ֳɵ��ַ���
	std::string ToString(unsigned int flag=1);

	// ������: FillSepString
	// ���  : ������ 2012-4-18 9:34:18
	// ����  : int ���ݸ���
	// ����  : string datastring  �Էָ����ָ������� 1|2|3| 1|2|3
	// ����  : �����ָ�����
	int FillSepString(std::string datastring);

	// ������: SetSeparateString
	// ���  : ������ 2012-4-18 9:22:51
	// ����  : void 
	// ����  : char *sepstr
	// ����  : ���÷ָ���
	void SetSeparateString(const char *sepstr);
	
	/// ������: Clear
	/// ���  : ������ 2007-3-30 10:30:53
	/// ����  : void 
	/// ����  : ����Ѿ�װ���������
	void Clear();				

	/// ������: GetCount
	/// ���  : ������ 2007-3-30 10:31:14
	/// ����  : int 
	/// ����  : ��ȡ�ָ����ָ������ݸ���
	int GetCount(); 			

	/// ������: SetAt
	/// ���  : ������ 2007-3-30 10:31:34
	/// ����  : bool 
	/// ����  : std::string buffer
	/// ����  : int nIndex
	/// ����  : �޸ĵ�nIndex��������
	bool SetAt(std::string buffer,int nIndex);	

	/// ������: GetAt
	/// ���  : ������ 2008-4-21 10:24:21
	/// ����  : int 
	/// ����  : int nIndex
	/// ����  : std::string &data
	/// ����  : bool flag ȥ���ո��־
	/// ����  : �õ�ĳ������
	bool GetAt(int nIndex,std::string &data,bool flag=false); 

	/// ������: GetAt
	/// ���  : ������ 2008-4-21 10:26:02
	/// ����  : string 
	/// ����  : int nIndex
	/// ����  : bool flag ȥ���ո��־
	/// ����  : �õ�ĳ��������
	std::string GetAt(int nIndex,bool flag=false);		

	/// ������: Add
	/// ���  : ������ 2007-3-30 10:37:27
	/// ����  : int 
	/// ����  : string data
	/// ����  : ����������
	void Add(std::string data);		

	/// ������: Add
	/// ���  : ������ 2007-3-30 10:37:35
	/// ����  : void 
	/// ����  : double data
	/// ����  : int dec  ����С��λ
	/// ����  : ����������
	void Add(double data,int dec=2);

	/// ������: Add
	/// ���  : ������ 2007-3-30 10:38:06
	/// ����  : void 
	/// ����  : int data
	/// ����  : ����������
	void Add(int data);

	/// ������: Add
	/// ���  : ������ 2007-3-30 10:38:10
	/// ����  : void 
	/// ����  : long data
	/// ����  : ����������
	void Add(long data);

public:

	bool   m_bNotSepString;

	// ������: FillSepString
	// ���  : ������ 2012-4-18 9:34:18
	// ����  : int ���ݸ���
	// ����  : string datastring  �Ժ��ֻ�ȫ�Ƿָ����ָ�������
	// ����  : �����ָ�����
	int FillChnSepString(std::string datastring);

	// ������: fastDectGB
	// ���  : ������ 2013-12-9 9:02:27
	// ����  : bool 
	// ����  : const char * src
	// ����  : ����ַ����Ƿ��к���
	bool fastDectGB(const char * src);


	// ������: bufFillSP
	// ���  : ������ 2013-12-9 9:10:08
	// ����  : void 
	// ����  : const char* databuf
	// ����  : char *destbuf
	// ����  : ��ȫ�ǡ����ֺ����SPEP  �����߱��뱣֤destbuf���㹻�Ŀռ�ȥ��������
	void bufFillSP(const char* databuf,  char *destbuf);


	// ������: bufRemoveSP
	// ���  : ������ 2013-12-9 9:11:22
	// ����  : void  
	// ����  : const char* databuf
	// ����  : char *destbuf
	// ����  : ȥ��SPEP �����߱��뱣֤destbuf���㹻�Ŀռ�ȥ��������
	void  bufRemoveSP(const char* databuf,  char *destbuf);

private:
	std::string  m_sSepstr;///�ָ���

	typedef std::list <std::string> STRINGSLIST;

	STRINGSLIST m_buffer_list;
    int m_count; 
	
};

#endif // !defined(AFX_BF_SLIST_H__34A62F79_4DB7_495B_9479_09886538A78D__INCLUDED_)
