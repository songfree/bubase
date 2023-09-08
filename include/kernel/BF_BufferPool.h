// BF_BufferPool.h: interface for the CBF_BufferPool class.
//
//////////////////////////////////////////////////////////////////////

/***********************************************************************************
  ������־��¼
  ������: ������
  �������� 2012-4-17 10:30:20 �ع�
  �汾: V1.00 
  ˵��: ������࣬�ڴ󲢷���ʱ������ͨ����������ڴ����Ч��
 ***********************************************************************************/


#if !defined(AFX_BF_BUFFERPOOL_H__FA0DD54E_962B_4184_92E0_6E9D4E28B465__INCLUDED_)
#define AFX_BF_BUFFERPOOL_H__FA0DD54E_962B_4184_92E0_6E9D4E28B465__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "public.h"
#include "BF_Mutex.h"
#include "MdbBase.h"

#ifdef KERNEL_EXPORTS
#define KERNEL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNEL_EXPORT __declspec(dllimport)
#else
#define KERNEL_EXPORT 
#endif
#endif



class KERNEL_EXPORT CBF_BufferPool  
{
public:
	typedef std::stack<void *> POOLSTACK;

	CBF_BufferPool(int mallocsize=8192);
	virtual ~CBF_BufferPool();

	unsigned int  m_nMaxFreeSize;//�����и�������Ϊ0ʱ�����ƣ���ʵ�ʿ��д��ڴ�ֵʱ���ͷŴ��ڵ�

public:
	
	/// ������: *PoolMalloc
	/// ���  : ������ 2009-5-18 17:21:24
	/// ����  : void *
	/// ����  : ����һ���ڴ�
	void *PoolMalloc();
	
	/// ������: Init
	/// ���  : ������ 2009-5-18 17:22:59
	/// ����  : bool 
	/// ����  : int size  ����������
	/// ����  : int mallocsize  ��������С
	/// ����  : int maxfreesize  �����л��������������ڴ˵��ͷ� ����0Ϊsize��100��
	/// ����  : ��ʼ����ȷ��һ�η�����ڴ��С.
	bool Init(int size,int mallocsize=8192,int maxfreesize=0);
	
	/// ������: PoolFree
	/// ���  : ������ 2009-5-18 17:21:38
	/// ����  : void 
	/// ����  : void *trunck
	/// ����  : ���ڴ�����ջ
	void PoolFree(void *trunck);
	
	/// ������: PoolInfo
	/// ���  : ������ 2009-5-18 17:22:30
	/// ����  : void 
	/// ����  : int &total
	/// ����  : int &unused
	/// ����  : �����ܹ�������ڴ�����δʹ����
	void PoolInfo(int &total,int &unused);
	
	/// ������: PoolInuse
	/// ���  : ������ 2009-5-18 17:22:09
	/// ����  : int  
	/// ����  : ������ʹ�õ��ڴ���
	int  PoolInuse();
protected:
	int    m_size;   //�ڴ��Ĵ�С
	int    m_nInitSize;//��ʼ���ĳ��Ӵ�С
	int    m_total;  //�ܹ�������ڴ�����

	void *m_poolmalloc;
	
	POOLSTACK m_stack;//��ſ��еĻ����ַ
	CBF_Mutex m_mutex;

//	POOLSTACK m_pTotalBuffer;

	
	CMemTableNew <void *> m_pTotalBuffer_new;  //������еĻ����ַ
	CPkeyUint64<1> m_pKeyAddr;//���еĻ����ַ������
};

#endif // !defined(AFX_BF_BUFFERPOOL_H__FA0DD54E_962B_4184_92E0_6E9D4E28B465__INCLUDED_)
