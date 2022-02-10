// FuncTbl.h: interface for the CFuncTbl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FUNCTBL_H__1BB79147_6A71_4944_AA41_5F4BBC375F08__INCLUDED_)
#define AFX_FUNCTBL_H__1BB79147_6A71_4944_AA41_5F4BBC375F08__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MdbBase.h"
#include "BF_Mutex.h"
#include "BpcHead.h"


typedef struct 
{
	unsigned int    nFuncNo;           //������
	char     sFuncName[41];         //��������
	char     sBpuGroupName[21];      //bpu������
	char            cPrio;             //�������ȼ�
	char            cServiceFlag;      //�����־ 0 BPC�ڲ����ܲ�����DREB��ע��2BPU�ڲ����ܲ�����DREB��ע��  1Ҫ��DREB��ע��
	unsigned int    nCallNum;          //���ô���
	unsigned int    nBpuGroupIndex;    //bpu���Ӧ�������������
}S_FUNCINFO_TBL;     //������Ϣ

class CFuncTbl  
{
public:

	// ������: GetAllFunc
	// ���  : ������ 2013-10-25 10:16:58
	// ����  : int 
	// ����  : vector<S_FUNCINFO_TBL> &funclist
	// ����  : ��ȡ���еĹ����б�������������
	int GetAllFunc(vector<S_FUNCINFO_TBL> &funclist);


	// ������: GetAllFuncByGroup
	// ���  : ������ 2013-11-2 15:32:11
	// ����  : int 
	// ����  : vector<S_FUNCINFO_TBL> &funclist
	// ����  : ��ȡ���еĹ����б���bpu������
	int GetAllFuncByGroup(vector<S_FUNCINFO_TBL> &funclist);

	// ������: FindFunc
	// ���  : ������ 2013-4-8 11:40:16
	// ����  : S_FUNCINFO_TBL & 
	// ����  : unsigned int func
	// ����  : int &findflag  ��־ 0û�ҵ����޴˹��� 1�ҵ���Щ����
	// ����  : ����һ�����ܣ������ع��ܵ�ַ���ù��ܵĵ��ü�����1
	S_FUNCINFO_TBL & FindFunc(unsigned int func,int &findflag);

	// ������: SelectByPk
	// ���  : ������ 2010-7-27 11:26:15
	// ����  : bool 
	// ����  : unsigned int func
	// ����  : S_FUNCINFO_TBL &fn
	// ����  : ��ȡ����,���ܵĵ��ü�������
	bool Select(unsigned int func,S_FUNCINFO_TBL &fn);

	CFuncTbl();
	virtual ~CFuncTbl();
	
	// ������: Insert
	// ���  : ������ 2010-7-27 11:25:25
	// ����  : bool 
	// ����  : S_FUNCINFO_TBL fn
	// ����  : insertһ������
	bool Insert(S_FUNCINFO_TBL fn);

	int Size()
	{
		return m_pkey.Size();
	}

protected:
	// ������: Next
	// ���  : ������ 2010-7-27 11:26:34
	// ����  : bool 
	// ����  : S_FUNCINFO_TBL &fn
	// ����  : ��һ������
	bool Next(S_FUNCINFO_TBL &fn);
	
	// ������: First
	// ���  : ������ 2010-7-27 11:26:23
	// ����  : bool 
	// ����  : S_FUNCINFO_TBL &fn
	// ����  : ��һ������
	bool First(S_FUNCINFO_TBL &fn);

	
	
	// ������: Delete
	// ���  : ������ 2010-7-27 11:26:06
	// ����  : bool 
	// ����  : unsigned int func
	// ����  : ɾ��һ������
	bool Delete(unsigned int func);
	
	// ������: Update
	// ���  : ������ 2010-7-27 11:25:38
	// ����  : bool 
	// ����  : S_FUNCINFO_TBL fn
	// ����  : ���¹���
	bool Update(S_FUNCINFO_TBL fn);


	CMemTable <S_FUNCINFO_TBL> m_table;        //���� �ڴ��
	CIndexInt<1>               m_pkey;           //���ܺţ�Ψһ����
	CIndexCharF<21,1>          m_indexgroup;     //��bpu��Ϊ����
	S_FUNCINFO_TBL             m_null;
	CBF_Mutex m_mutex;	
};

#endif // !defined(AFX_FUNCTBL_H__1BB79147_6A71_4944_AA41_5F4BBC375F08__INCLUDED_)
