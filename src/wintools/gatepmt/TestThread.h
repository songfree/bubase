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

#include "ShareData.h"
#include "BF_SimpleLog.h"
#include "IVarDefine.h"
#include "GateLink.h"
#include "GetPushThread.h"



#ifdef _WINDOWS
#ifdef _DEBUG
#pragma comment(lib, "bf_cgateapid.lib") 
#pragma message("Automatically linking with bf_cgateapid.lib ")
#else
#pragma comment(lib, "bf_cgateapi.lib") 
#pragma message("Automatically linking with bf_cgateapi.lib ")
#endif

#endif

typedef struct _SVAROFFSET
{
	int begin;
	int end;
	CIVarDefine *pVarPoint;
};
class CTestThread : public CBF_Thread  
{
public:
	int GetSerial();
	bool Login();

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


	// ������: SetData
	// ���  : ������ 2010-9-30 15:07:16
	// ����  : bool 
	// ����  : const char *data
	// ����  : unsigned int datalen
	// ����  : const char *logindata
	// ����  : unsigned int logindatalen
	// ����  : CShareData *pData
	// ����  : _THREAD_DATA_PARA para
	// ����  : ���ù������ݼ����Ͳ�������������
	bool SetData(const char *data,unsigned int datalen,const char *logindata,unsigned int logindatalen,CShareData *pData,_THREAD_DATA_PARA para);

	virtual bool Init();
	
	CShareData *m_pShareData; //��������ݲ���

	unsigned short  m_nSerial;

	// ������: ExitThreadInstance
	// ���  : ������ 2012-4-9 12:01:05
	// ����  : virtual void 
	// ����  : �˳��߳�Ҫ���Ĺ���,�߳��˳�ʱ����һ��
	virtual void ExitThreadInstance();
	// ������: InitThreadInstance
	// ���  : ������ 2012-4-9 12:00:54
	// ����  : virtual bool 
	// ����  : �̳߳�ʼ�����߳�����ʱ����һ��
	virtual bool InitThreadInstance();



	
	int m_nID;//�̵߳�ID

protected:
	CBF_SimpleLog m_log;
	
	char *m_pSendData;//���͵Ľű���������
	int m_nSendDataLen;//���͵Ľű����ݳ���

	char *m_pLoginData;//���͵Ľű���������
	int m_nLoginDataLen;//���͵Ľű����ݳ���

	_THREAD_DATA_PARA  m_pDataPara;//�̲߳���

	CGateLink          m_pGateLink;//����api��

	CGetPushThread      m_pGetPush;

	CGATE_COMMSTRU     m_pCgateLogin;


	vector<_SVAROFFSET>m_varList;

private:
	/// ������: Run
	/// ���  : ������ 2007-3-30 9:54:53
	/// ����  : virtual int 
	/// ����  : �̵߳���������
	virtual int Run();
	

};

#endif // !defined(AFX_TESTTHREAD_H__6548AE8E_CEA9_4163_8A7B_4BFF89F00B2B__INCLUDED_)
