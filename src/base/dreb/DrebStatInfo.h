// DrebStatInfo.h: interface for the CDrebStatInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DREBSTATINFO_H__F4320113_C67B_4C13_B218_E7E7B0E558DA__INCLUDED_)
#define AFX_DREBSTATINFO_H__F4320113_C67B_4C13_B218_E7E7B0E558DA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BF_Mutex.h"
#include "typedefine.h"
//ͳ����Ϣ
typedef struct 
{
	UINT64_   m_nRcvPackClient;//���տͻ��˰���
	UINT64_   m_nRcvByteClient;//���տͻ����ֽ���
	UINT64_   m_nAnsPackClient;//Ӧ��ͻ��˰���
	UINT64_   m_nAnsByteClient;//Ӧ��ͻ����ֽ���
	
	UINT64_   m_nTransOutNum; //����ת������
	UINT64_   m_nTransOutByte;//����ת���ֽ���
	UINT64_   m_nTransInNum;  //����ת������
	UINT64_   m_nTransInByte; //����ת���ֽ���
	
	UINT64_   m_nCallSrvNum;   //���÷������
    UINT64_   m_nCallSrvByte;  //���÷����ֽ���
	UINT64_   m_nSrvAnsNum;    //����Ӧ�����
    UINT64_   m_nSrvAnsByte;   //����Ӧ���ֽ���

}S_DREBSTAT_INFO;

class CDrebStatInfo  
{
public:

	// ������: ReportSendByte
	// ���  : ������ 2012-3-6 10:35:46
	// ����  : void 
	// ����  : int ntype  ��������
	// ����  : int len    ����
	// ����  : ���淢�ͳ���
	void ReportSendByte(int ntype,int len);

	// ������: ReportSendPack
	// ���  : ������ 2012-3-6 10:36:19
	// ����  : void 
	// ����  : int ntype ��������
	// ����  : int packnum
	// ����  : ���淢�����ݰ���Ŀ
	void ReportSendPack(int ntype,int packnum);

	// ������: ReportRcvByte
	// ���  : ������ 2012-3-6 10:36:40
	// ����  : void 
	// ����  : int ntype ��������
	// ����  : int len
	// ����  : ��������ֽ���
	void ReportRcvByte(int ntype,int len);

	// ������: ReportRcvPack
	// ���  : ������ 2013-3-6 10:36:53
	// ����  : void 
	// ����  : int ntype ��������
	// ����  : int packnum
	// ����  : ����������ݰ���
	void ReportRcvPack(int ntype,int packnum);

	CDrebStatInfo();
	virtual ~CDrebStatInfo();

	S_DREBSTAT_INFO m_drebinfo;
	
	int              m_nStartTime;           //����ʱ��
	char             m_sStartDate[20];           //��������
protected:
	CBF_Mutex m_mutex;

	
};

#endif // !defined(AFX_DREBSTATINFO_H__F4320113_C67B_4C13_B218_E7E7B0E558DA__INCLUDED_)
