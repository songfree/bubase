// CgateLog.h: interface for the CCgateLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CGATELOG_H__003786D3_693C_4B1E_8FA3_44BE7C2F4DD0__INCLUDED_)
#define AFX_CGATELOG_H__003786D3_693C_4B1E_8FA3_44BE7C2F4DD0__INCLUDED_
#include "BF_Date_Time.h"
#include "GateHead.h"
#include "BF_CommDataLog.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifdef CGATEAPI_EXPORTS
#define CGATEAPI_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define CGATEAPI_EXPORT __declspec(dllimport)
#else
#define CGATEAPI_EXPORT 
#endif
#endif

class CGATEAPI_EXPORT CCgateLog
{
public:
	bool IsStoped();

	// ������: StartLog
	// ���  : ������ 2015-11-16 11:41:00
	// ����  : bool 
	// ����  : ������־�߳�
	bool StartLog();

	// ������: StopLog
	// ���  : ������ 2015-11-16 11:40:50
	// ����  : void 
	// ����  : ֹͣ��־�߳�
	void StopLog();

	// ������: SetLoglevel
	// ���  : ������ 2015-11-16 11:40:35
	// ����  : void 
	// ����  : int loglevel
	// ����  : ������־����
	void SetLogLevel(int loglevel);

	CCgateLog();
	virtual ~CCgateLog();

	// ������: LogDreb
	// ���  : ������ 2013-5-6 16:00:38
	// ����  : void 
	// ����  : int level
	// ����  : PCOMMSTRU data
	// ����  : д��dreb����־
	void LogCgate(int level ,PCGATE_COMMSTRU data,int index,const char *key=NULL);

	// ������: SetLogPara
	// ���  : ������ 2013-5-6 16:00:11
	// ����  : void 
	// ����  : int logsize
	// ����  : int level
	// ����  : const char *logpath
	// ����  : const char *iologname ������־��
	// ����  : const char *errlogname ������־��
	// ����  : ���ò���
	void SetLogPara(int logsize,int level ,const char *logpath,const char *cgatelogname);

protected:
	int m_nLogLevel;
	CBF_CommDataLog m_pLogGate;

};

#endif // !defined(AFX_CGATELOG_H__003786D3_693C_4B1E_8FA3_44BE7C2F4DD0__INCLUDED_)
