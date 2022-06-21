// IErrLog.h: interface for the CIErrlog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IERRLOG_H__5435B2B8_48F0_43B6_B609_5663A3879846__INCLUDED_)
#define AFX_IERRLOG_H__5435B2B8_48F0_43B6_B609_5663A3879846__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define LOG_DEBUG 5   ///������Ϣ
#define LOG_PROMPT 4  ///��ʾ����
#define LOG_WARNNING 3  ///����
#define LOG_ERROR_GENERAL 2  ///һ�����
#define LOG_ERROR 1        ///��Ҫ����
#define LOG_ERROR_FAULT 0  ///���ش�
#define LOG_INFO    -1     ///����ֹͣ�ȱ���д����־����Ϣ

#include <stdio.h>
#include <string>


#ifdef KERNEL_EXPORTS
#define KERNEL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNEL_EXPORT __declspec(dllimport)
#else
#define KERNEL_EXPORT 
#endif
#endif

class KERNEL_EXPORT CIErrlog  
{
public:
	CIErrlog()
	{

	}
	virtual ~CIErrlog()
	{

	}
	// ������: isWrite
	// ���  : ������ 2009-6-29 14:28:34
	// ����  : int 
	// ����  : int level ��־���� 
	// ����  : �Ƿ�Ҫд����־
	virtual bool isWrite(int level)=0;

	// ������: SetLogPara
	// ���  : ������ 2009-6-29 14:28:34
	// ����  : int 
	// ����  : int level ��־���� 
	// ����  : const char *logfilepath  ��־�ļ�·��
	// ����  : const char *logfilename  ��־�ļ���
	// ����  : int id=-1  ���̻��߳�ID
	// ����  : ������־����
	virtual void SetLogPara(int level,const char *logfilepath,const char *logfilename,int id=-1)=0;


	// ������: GetLogPara
	// ���  : ������ 2012-4-25 11:21:05
	// ����  : virtual void 
	// ����  : int &level ��־����
	// ����  : std::string &logfilepath ��־�ļ�·��
	// ����  : std::string &logfilename ��־�ļ���
	// ����  : int &id ���̻��߳�ID
	// ����  : ȡ��־����
	virtual void GetLogPara(int &level,std::string &logfilepath, std::string &logfilename,int &id)=0;

	/// ������: LogMp
	/// ���  : ������ 2009-5-16 17:28:24
	/// ����  : int 
	/// ����  : int level
	/// ����  : const char *filename
	/// ����  : int line
	/// ����  : const char *fmt
	/// ����  : ...
	/// ����  : д����־
	virtual int LogMp(int level, const char *filename, int line,const char *fmt,...)=0;

	/// ������: LogBin
	/// ���  : ������ 2007-7-10 11:27:52
	/// ����  : int 
	/// ����  : int level
	/// ����  : const char *filename
	/// ����  : int line
	/// ����  : const char *title
	/// ����  : char *msg
	/// ����  : int msglen
	/// ����  : д���������������־�ļ� ע��msglen�Ĵ�С
	virtual int LogBin(int level, const char *filename, int line,const char *title,char *msg,int msglen)=0;


	// ������: SetMaxLogSize
	// ���  : ������ 2014-11-11 15:19:01
	// ����  : virtual void 
	// ����  : int size  �ֽ���
	// ����  : ������־�ļ�������С��0Ϊ�����ƣ��ﵽ��С���л��ļ�
	virtual void SetMaxLogSize(int size)=0;

};

#endif // !defined(AFX_IERRLOG_H__5435B2B8_48F0_43B6_B609_5663A3879846__INCLUDED_)
