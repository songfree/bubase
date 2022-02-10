// BF_SimpleLog.h: interface for the CBF_SimpleLog class.
//
//////////////////////////////////////////////////////////////////////

/***********************************************************************************
  ������־��¼
  ������: ������
  �������� 2012-4-26 9:12:53 �ع�
  �汾: V1.00 
  ˵��: ����һ������־�� ��ͨ������ϵͳ���ļ�����������д��
 ***********************************************************************************/


#if !defined(AFX_BF_SIMPLELOG_H__BC40DB9B_FA26_4689_8DD7_DFD94B06E2D8__INCLUDED_)
#define AFX_BF_SIMPLELOG_H__BC40DB9B_FA26_4689_8DD7_DFD94B06E2D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IErrLog.h"
#include "BF_Date_Time.h"


#ifdef KERNEL_EXPORTS
#define KERNEL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNEL_EXPORT __declspec(dllimport)
#else
#define KERNEL_EXPORT 
#endif
#endif

class KERNEL_EXPORT CBF_SimpleLog : public CIErrlog  
{
public:
	CBF_SimpleLog();
	virtual ~CBF_SimpleLog();

	// ������: isWrite
	// ���  : ������ 2009-6-29 14:28:34
	// ����  : int 
	// ����  : int level ��־���� 
	// ����  : �Ƿ�Ҫд����־
	virtual bool isWrite(int level);
	
	// ������: SetLogPara
	// ���  : ������ 2009-6-29 14:28:34
	// ����  : int 
	// ����  : int level ��־���� 
	// ����  : const char *logfilepath  ��־�ļ�·��
	// ����  : const char *logfilename  ��־�ļ���
	// ����  : int id=-1
	// ����  : ������־����
	virtual void SetLogPara(int level,const char *logfilepath,const char *logfilename,int id=-1);

	/// ������: GetLogPara
	/// ���  : ������ 2012-4-25 11:21:05
	/// ����  : virtual void 
	/// ����  : int &level ��־����
	/// ����  : std::string &logfilepath ��־�ļ�·��
	/// ����  : std::string &logfilename ��־�ļ���
	/// ����  : int &id ���̻��߳�ID
	/// ����  : ȡ��־����
	virtual void GetLogPara(int &level,std::string &logfilepath, std::string &logfilename,int &id);
	
	/// ������: LogMp
	/// ���  : ������ 2009-5-16 17:28:24
	/// ����  : int 
	/// ����  : int level
	/// ����  : const char *filename
	/// ����  : int line
	/// ����  : const char *fmt
	/// ����  : ...
	/// ����  : д����־
	virtual int LogMp(int level, const char *filename, int line,const char *fmt,...);
	
	/// ������: LogBin
	/// ���  : ������ 2007-7-10 11:27:52
	/// ����  : int -1ʧ�� 0�ɹ�
	/// ����  : int level  ��־����
	/// ����  : const char *filename  Դ�ļ���
	/// ����  : int line              Դ�ļ���
	/// ����  : const char *title  ����
	/// ����  : char *msg   ����
	/// ����  : int msglen  ����
	/// ����  : д���������������־�ļ�,'\0'��*����
	virtual int LogBin(int level, const char *filename, int line,const char *title,char *msg,int msglen);


	// ������: SetMaxLogSize
	// ���  : ������ 2014-11-12 14:02:18
	// ����  : virtual void 
	// ����  : int size  �ֽ���   С��1024������
	// ����  : ������־�ļ���С
	virtual void SetMaxLogSize(int size);

	void  SetLogLevel(int level);
	int  GetLogLevel();

protected:
	/// ������: GetName
	/// ���  : ������ 2007-7-10 10:52:10
	/// ����  : void 
	/// ����  : char *filename  
	/// ����  : ��ȡ�ļ����ƣ�����·�����ļ���
	void GetName(char *filename);

protected:
	int            m_nLogLevel; //��־����
	int            m_nId;
	std::string    m_sLogPath;   
	std::string    m_sLogName;
	CBF_Date_Time  m_pDate;
	std::string    m_sCurDate;//��ǰ����
};

#endif // !defined(AFX_BF_SIMPLELOG_H__BC40DB9B_FA26_4689_8DD7_DFD94B06E2D8__INCLUDED_)
