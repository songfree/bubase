// NextTbl.h: interface for the CNextTbl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NEXTTBL_H__786AAE62_7EC4_42CD_AFFA_3902DA17B08A__INCLUDED_)
#define AFX_NEXTTBL_H__786AAE62_7EC4_42CD_AFFA_3902DA17B08A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MdbBase.h"
#include "BF_Mutex.h"

#include "GResource.h"

typedef struct 
{
	int      nNodeId;        //�������߽ڵ���
	int      nNodePrivateId; //�������߽ڵ�˽�����
	int      nDrebSerial;    //�������߽ڵ��������ˮ����־Ψһ���������ݰ�
	unsigned int  nFileSerial;//�ļ���ˮ
	int      nTime;//����ʱ��
}S_NEXT_TBL;     //��������Ϣ��

class CNextTbl  
{
public:
	CGResource *m_pRes;

	// ������: DeleteTime
	// ���  : ������ 2013-4-16 15:39:39
	// ����  : int 
	// ����  : unsigned int stime  ��ʱ��
	// ����  : ɾ��ʱ�䳬ʱ�ļ�¼
	int DeleteTime(unsigned int stime);
	
	CNextTbl();
	virtual ~CNextTbl();

	// ������: Next
	// ���  : ������ 2013-4-16 15:40:31
	// ����  : bool 
	// ����  : S_NEXT_TBL &fn
	// ����  : ��һ����¼
	bool Next(S_NEXT_TBL &fn);
	
	// ������: First
	// ���  : ������ 2013-4-16 15:40:19
	// ����  : bool 
	// ����  : S_NEXT_TBL &fn
	// ����  : ��һ����¼
	bool First(S_NEXT_TBL &fn);

	// ������: Select
	// ���  : ������ 2013-4-16 15:40:45
	// ����  : bool 
	// ����  : int nodeid
	// ����  : int privateid
	// ����  : int serial
	// ����  : S_NEXT_TBL &fn
	// ����  : ���Ҽ�¼
	bool Select(int nodeid,int privateid,int serial,S_NEXT_TBL &fn);


	// ������: Delete
	// ���  : ������ 2013-4-16 15:41:05
	// ����  : bool 
	// ����  : int nodeid
	// ����  : int privateid
	// ����  : int serial
	// ����  : ɾ����¼
	bool Delete(int nodeid,int privateid,int serial);
	

	// ������: Update
	// ���  : ������ 2013-4-16 15:41:16
	// ����  : bool 
	// ����  : S_NEXT_TBL fn
	// ����  : ���¼�¼
	bool Update(S_NEXT_TBL fn);


	// ������: Insert
	// ���  : ������ 2013-4-16 15:41:27
	// ����  : bool 
	// ����  : S_NEXT_TBL fn
	// ����  : �����¼
	bool Insert(S_NEXT_TBL fn);
protected:
	
	CMemTable <S_NEXT_TBL> m_table;               //���� �ڴ��
	CIndexInt<3>           m_pkey;                //�ڵ�ID���ڵ�˽��ID���ڵ���ˮ��Ψһ����
	CIndexInt<1>           m_indextime;           //ʱ������
	CBF_Mutex m_mutex;	
};



#endif // !defined(AFX_NEXTTBL_H__786AAE62_7EC4_42CD_AFFA_3902DA17B08A__INCLUDED_)
