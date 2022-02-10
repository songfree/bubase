// MdbQuotation.h: interface for the CMdbQuotation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MDBQUOTATION_H__FDF3F654_CC36_408B_8B39_A4333C739AFB__INCLUDED_)
#define AFX_MDBQUOTATION_H__FDF3F654_CC36_408B_8B39_A4333C739AFB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "MdbBase.h"

//������������µ�������Ϣ
typedef struct 
{
	int            nVerietyId;        //��Լ���    s_nHook
	int            nDate;             //��������    s_nSerial
	unsigned int   nSerial;           //�������    n_nNextNo
}S_CGATE_QUOTATION;   //�����

class CMdbQuotation  
{
public:
	CMdbQuotation();
	virtual ~CMdbQuotation();
	

	// ������: IsNewQuotation
	// ���  : ������ 2014-7-1 12:48:19
	// ����  : int <0���鲻�������ѹ��ڣ��ɶ���   >0���������� Ϊ������Ŀ
	// ����  : S_CGATE_QUOTATION &quo
	// ����  : �ж�����ı仯���
	int IsNewQuotation(S_CGATE_QUOTATION &quo);


protected:
	CMemTable <S_CGATE_QUOTATION> m_table;        //���������ڴ��
	
	CPkeyInt<1>             m_pkey;   //��Լ���
	
};

#endif // !defined(AFX_MDBQUOTATION_H__FDF3F654_CC36_408B_8B39_A4333C739AFB__INCLUDED_)
