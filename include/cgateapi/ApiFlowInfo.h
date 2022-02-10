// ApiFlowInfo.h: interface for the CApiFlowInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APIFLOWINFO_H__C698E6D2_B610_45A7_A4CE_9E6340335C3F__INCLUDED_)
#define AFX_APIFLOWINFO_H__C698E6D2_B610_45A7_A4CE_9E6340335C3F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "GateHead.h"

#ifdef CGATEAPI_EXPORTS
#define CGATEAPI_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define CGATEAPI_EXPORT __declspec(dllimport)
#else
#define CGATEAPI_EXPORT 
#endif
#endif

class CGATEAPI_EXPORT CApiFlowInfo  
{
public:

	// ������: GetSpeedRateTx
	// ���  : ������ 2013-5-27 17:36:55
	// ����  : unsigned int 
	// ����  : ��ȡ��ǰÿ�뷢�ͽ�����
	unsigned int GetSpeedRateTx();

	// ������: GetSpeedRateBytes
	// ���  : ������ 2013-5-27 17:36:39
	// ����  : unsigned int 
	// ����  : ��ȡ��ǰÿ�뷢���ֽ���
	unsigned int GetSpeedRateBytes();

	// ������: ReportTimer
	// ���  : ������ 2013-5-27 17:29:47
	// ����  : void 
	// ����  : ��ʱ�����ã�����ÿ�뷢���ֽ����ͷ��ͽ�����
	void ReportTimer();

	// ������: ReportStat
	// ���  : ������ 2013-5-27 16:12:20
	// ����  : void 
	// ����  : unsigned int sbytes
	// ����  : ���淢������
	void ReportStat(unsigned int sbytes);

	CApiFlowInfo();
	virtual ~CApiFlowInfo();


	
private:
	unsigned  int   m_nSendPackNum;             //�ܷ��ͽ�����
	unsigned  int   m_nSendPackBytes;           //�ܷ����ֽ���

	unsigned  int   m_nPackNumSpeedRate;        //ÿ�뷢�ͽ�����
	unsigned  int   m_nPackByteSpeedRate;       //ÿ�뷢���ֽ���

	unsigned  int   m_nOldSendPackNum;             //��һ���ܷ��ͽ�����
	unsigned  int   m_nOldSendPackBytes;           //��һ���ܷ����ֽ���

};

#endif // !defined(AFX_APIFLOWINFO_H__C698E6D2_B610_45A7_A4CE_9E6340335C3F__INCLUDED_)
