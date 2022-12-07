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
#include "BF_Mutex.h"
//������Ϣ  ÿ������һ��ʵ����



class CTbl_Subscribe
{
protected:

	CMemTableNew<S_SUBSCRIBE_>     m_table;
	CPkeyUInt<2>         m_key;//���ܺ�+key
	CBF_Mutex                      m_mutex;//������
public:
	CTbl_Subscribe()
	{
	}
	virtual ~CTbl_Subscribe()
	{

	}

};

class CSubScribeInfo  
{
protected:

    CMemTableNew<S_SUBSCRIBE_>     m_table;
    CPkeyUInt<2>         m_key;//���ܺ�+key

	void Clear();
public:

	// ������: IsSubscribe
	// ���  : ������ 2014-10-22 14:35:21
	// ����  : bool true����  falseδ����
	// ����  : int funcno
	// ����  : unsigned int key
	// ����  : �Ƿ���
	bool IsSubscribe(unsigned int funcno,unsigned int key);

	// ������: Subscribe
	// ���  : ������ 2014-10-22 10:55:12
	// ����  : bool TRUE�ɹ�  falseʧ��
	// ����  : const char *data  �������Ķ������ݣ��ֽ���Ϊ������LE
	// ����  : int datalen   �������ݳ���
	// ����  : char *msg   ʧ����Ϣ
	// ����  : ���鶩��
	bool Subscribe(const char *data,int datalen,char *msg);


	int Select(std::vector<S_SUBSCRIBE_*>& reslist);
	int Subscribe(unsigned int funcno, unsigned int key);
	int UnSubscribe(unsigned int funcno, unsigned int key);
	CSubScribeInfo();
	virtual ~CSubScribeInfo();
	
	CIErrlog   *m_pLog;
	
protected:
	CDrebEndian   m_pEndian;
	CBF_Mutex     m_mutex;
};

#endif // !defined(AFX_SUBSCRIBEINFO_H__26E08F03_B50A_4FC7_B2BA_F8A211CB2F81__INCLUDED_)
