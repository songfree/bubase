// TestThread.h: interface for the CTestThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTTHREAD_H__6548AE8E_CEA9_4163_8A7B_4BFF89F00B2B__INCLUDED_)
#define AFX_TESTTHREAD_H__6548AE8E_CEA9_4163_8A7B_4BFF89F00B2B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "public.h"
#include "BF_Thread.h"
#include "FrontApi.h"
#include "ShareData.h"
#include "BF_SimpleLog.h"
#include "IVarDefine.h"
typedef struct _SVAROFFSET
{
	int begin;
	int end;
	CIVarDefine *pVarPoint;
};
class CTestThread : public CBF_Thread  
{
public:

	/// ������: ResetBuf
	/// ���  : ������ 2009-7-18 12:42:05
	/// ����  : void 
	/// ����  : �����������ݣ�ȡ����ֵ
	void ResetBuf();
	

	/// ������: GetVarOffset
	/// ���  : ������ 2009-7-18 12:42:27
	/// ����  : int 
	/// ����  : CIVarDefine *var
	/// ����  : const char *databuf
	/// ����  : int datalen
	/// ����  : vector<_SVAROFFSET>&result
	/// ����  : ��ȡ����ı����ڷ��������е�λ��
	int GetVarOffset(CIVarDefine *var,const char *databuf,int datalen,vector<_SVAROFFSET>&result);

	/// ������: ThreadInit
	/// ���  : ������ 2009-7-17 17:46:14
	/// ����  : int 
	/// ����  : �����̳߳�ʼ��
	int ThreadInit();
	
	CTestThread();
	virtual ~CTestThread();

	bool SetData(const char *data,unsigned int datalen,CShareData *pData,unsigned int begintime,int conntype);

	virtual bool Init();
	
	CShareData *m_pShareData; //��������ݲ���
	
	int m_nID;//�̵߳�ID

protected:
	CBF_SimpleLog m_log;
	CBF_Date_Time m_pDate;
	CFrontApi m_pFrontApi;//socket api

	int m_nBeginTime;//���߳̿�ʼ����ʱ��,�����˵�����뵥λ
	char *m_pSendData;//���͵Ľű���������
	int m_nSendDataLen;//���͵Ľű����ݳ���
	int m_nConnType;//�������� 0������1������

	vector<_SVAROFFSET>m_varList;

private:
	/// ������: Run
	/// ���  : ������ 2007-3-30 9:54:53
	/// ����  : virtual int 
	/// ����  : �̵߳���������
	virtual int Run();
	
	/// ������: Terminate
	/// ���  : ������ 2007-3-30 9:54:58
	/// ����  : virtual bool 
	/// ����  : ��ֹ�̴߳���,���߳��ڽ���ʱ�Զ�����
	virtual bool Terminate();
};

#endif // !defined(AFX_TESTTHREAD_H__6548AE8E_CEA9_4163_8A7B_4BFF89F00B2B__INCLUDED_)
