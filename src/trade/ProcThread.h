// ProcThread.h: interface for the CProcThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROCTHREAD_H__B593B31C_E1FF_4EF6_B64D_9DDD29C19C3A__INCLUDED_)
#define AFX_PROCTHREAD_H__B593B31C_E1FF_4EF6_B64D_9DDD29C19C3A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BF_Thread.h"
#include "DbApi.h"
#include "Xdp.h"
#include "IClientLink.h"
#include "IExchLink.h"
#include "IProcBase.h"
#include "BF_BufferPool.h"
#include "MemDb.h"
#include "IErrLog.h"
#include "PoolData.h"

//�����߳�
//���̴߳Ӷ���ȡ���ݣ����ݽ�������ҽ��׶�Ӧ�Ķ�̬����ĺ���ָ�룬���ô���

typedef struct
{
	CIProcBase             *iProcBase;//ҵ��̬�����ָ��
	DLLHANDLE              procdllhandle;//��̬����
	std::string            procdllname;//��̬������
	std::string            procInstance;//ʵ���ĺ�����
	std::string            procdllpath;//��̬��·��������
}S_PROCBASE_DLL;

class CProcThread : public CBF_Thread  
{
public:
	
	
	void Stop();
	bool Start();

	// ������: Init
	// ���  : ������ 2017-7-27 15:16:10
	// ����  : bool 
	// ����  : CBF_Xml *xml  �����ļ�����
	// ����  : ��ʼ������ȡҵ��̬�⣬ע�ύ��
	bool Init(CBF_Xml *xml);

	CProcThread();
	virtual ~CProcThread();

	CXdp            m_pXdp;//xdp����
	CIExchLink     *m_pExchLink;//���������ӵ���  ע��b_Cinfo.b_cIndex���洦���ߵ�index������Ӧ�𷵻ؾ�֪���ŵ��ĸ�Ӧ�����

	CIClientLink   *m_pClientLink; //�ͻ������ӵ���
	CMemDb         *m_pMemDb;//�ڴ��
	CIErrlog       *m_pLog;
	CBF_Xml        *m_pXml;

	CPoolDataMsg    *m_pRcvQueue;//�������
	CPoolDataMsg    m_pAnsQueue;//ͬ��Ӧ�����
	CProcFuncTbl        m_pFuncList;

	unsigned  int *g_nSvrHostStatus;//����״̬
	CPubData   *g_pPubData;//����������

	int    m_nIndex;
private:
	virtual int Run();

	
protected:

	bool           m_bIsRunning;
	S_TRADE_DATA   m_sData;

	bool OpenDll(DLLHANDLE *pDll, const char *dllname, const char *dllpath);

	CDbApi          *m_pSqlCmd; //���ݿ�����
	bool             m_bNeedConnectDb;
	bool             m_bAutoCommit;
	CBF_Des          m_pEncrypt;//des�ӽ�����

	std::vector<S_PROCBASE_DLL> m_alldllclass; //���涯̬����Ĵ���ʵ��

	

};

#endif // !defined(AFX_PROCTHREAD_H__B593B31C_E1FF_4EF6_B64D_9DDD29C19C3A__INCLUDED_)
