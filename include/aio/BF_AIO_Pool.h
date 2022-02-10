// BF_AIO_Pool.h: interface for the CBF_AIO_Pool class.
//
//////////////////////////////////////////////////////////////////////
/***********************************************************************************
  ������־��¼
  ������: ������
  �������� 2012-5-15 15:02:11
  �汾: V1.00 
  ˵��: ������һ�����ӳ��࣬������е����ӣ���ʼ����ʱ���Ƚ���ָ����С�����ӳ���������
         �����ӳز���ʱ�����������ӣ����ﵽָ��������ʱ������ʧ��
 ***********************************************************************************/
#if !defined(AFX_BF_AIO_POOL_H__DB10BB1F_05E0_48DE_87A3_01439B3B52D0__INCLUDED_)
#define AFX_BF_AIO_POOL_H__DB10BB1F_05E0_48DE_87A3_01439B3B52D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "BF_Socket.h"
#include "BF_Mutex.h"
#include "BF_AIO_Head.h"


#ifdef AIO_EXPORTS
#define AIO_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define AIO_EXPORT __declspec(dllimport)
#else
#define AIO_EXPORT 
#endif
#endif





class AIO_EXPORT CBF_AIO_Pool  
{
public:
	
	// ������: GetDataByIndex
	// ���  : ������ 2012-5-18 10:56:01
	// ����  : PSOCKET_POOL_DATA index�����ڷ���NULL
	// ����  : unsigned int index
	// ����  : ��������ȡ��������Ϣ
	PSOCKET_POOL_DATA GetDataByIndex(unsigned int index);
	
	// ������: PoolInfo
	// ���  : ������ 2012-5-15 16:21:59
	// ����  : void 
	// ����  : int &total   �ܵ����ӳ���Ŀ
	// ����  : int &unused  ������Ŀ
	// ����  : ���ӳ���Ϣ 
	void PoolInfo(int &total,int &unused);
	
	// ������: CloseFree
	// ���  : ������ 2012-5-15 16:21:28
	// ����  : void 
	// ����  : PSOCKET_POOL_DATA pp
	// ����  : �ر����ӣ��������ӷ�����еĳ���
	void CloseFree(PSOCKET_POOL_DATA pp);
	
	// ������: PoolMalloc
	// ���  : ������ 2012-5-15 16:13:50
	// ����  : PSOCKET_POOL_DATA 
	// ����  : ȡ��һ��δ������socket������Ϣ��������ӳ����п��򷵻أ�����NEW����
	PSOCKET_POOL_DATA PoolMalloc();
	
	// ������: Init
	// ���  : ������ 2012-5-15 15:51:51
	// ����  : bool 
	// ����  : unsigned int num  ������
	// ����  : ��ʼ������
	bool Init(unsigned int num);
	
	// ������: SetMaxNum
	// ���  : ������ 2012-5-15 15:41:53
	// ����  : void 
	// ����  : unsigned int max
	// ����  : ��������������
	void SetMaxNum(unsigned int max);

	// ������: RestData
	// ���  : ������ 2012-5-15 15:55:22
	// ����  : void 
	// ����  : PSOCKET_POOL_DATA pp
	// ����  : ����������ϢΪ��ʼֵ
	void ResetData(PSOCKET_POOL_DATA pp);
	
	CBF_AIO_Pool();
	virtual ~CBF_AIO_Pool();
	
private:
	
	CBF_Mutex    m_mutex;
	unsigned int m_nMaxNum;         //���socket��
	unsigned int m_nCurSocketIndex; //��ǰ��socket������
	
	std::vector<PSOCKET_POOL_DATA> m_vAllSocketInfo;  //������е���Ϣָ��
	std::deque<PSOCKET_POOL_DATA>  m_pFreeSocketInfo; //��ſ��е�socket��Ϣ


	PSOCKET_POOL_DATA m_pSocketData;
};

#endif // !defined(AFX_BF_AIO_POOL_H__DB10BB1F_05E0_48DE_87A3_01439B3B52D0__INCLUDED_)
