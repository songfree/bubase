// SubScribeInfo.h: interface for the CSubScribeInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SUBSCRIBEINFO_H__26E08F03_B50A_4FC7_B2BA_F8A211CB2F81__INCLUDED_)
#define AFX_SUBSCRIBEINFO_H__26E08F03_B50A_4FC7_B2BA_F8A211CB2F81__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "MdbBase.h"
#include "DrebEndian.h"
#include "GateHead.h"
#include "IErrLog.h"
//������Ϣ  ÿ������һ��ʵ����

class CSubScribeInfo  
{
public:

	// ������: IsQuoSubscribe
	// ���  : ������ 2014-10-22 14:35:21
	// ����  : bool true����  falseδ����
	// ����  : int varietycode
	// ����  : �жϴ˱�ź�Լ�������Ƿ���
	bool IsQuoSubscribe(int varietycode);

	// ������: Subscribe
	// ���  : ������ 2014-10-22 10:55:12
	// ����  : bool TRUE�ɹ�  falseʧ��
	// ����  : const char *data  �������Ķ������ݣ��ֽ���Ϊ������LE
	// ����  : int datalen   �������ݳ���
	// ����  : char *msg   ʧ����Ϣ
	// ����  : ���鶩��
	bool QuoSubscribe(const char *data,int datalen,char *msg);

	CSubScribeInfo();
	virtual ~CSubScribeInfo();
	
	CIErrlog   *m_pLog;
	
protected:
	int   m_nQuoSubcribeFlag;  //���鶩�ı�־ 0 ȡ������   1����Լ��Ŷ���   2���г�����
	unsigned int  m_nQuoDataNum; //��flagΪ1ʱ����Ϊ��Լ����   ��Ϊ2ʱ��Ϊ�г�����
	CInt          m_listVariety; //���ĵ������Լ�б�

	CDrebEndian   m_pEndian;
};

#endif // !defined(AFX_SUBSCRIBEINFO_H__26E08F03_B50A_4FC7_B2BA_F8A211CB2F81__INCLUDED_)
