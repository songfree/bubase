// ArbMain.h: interface for the CArbMain class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRADEMAIN_H__D1862398_E67A_4017_AC64_51E13AE7BEF3__INCLUDED_)
#define AFX_TRADEMAIN_H__D1862398_E67A_4017_AC64_51E13AE7BEF3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TradeResource.h"
#include "DrebMsgThread.h"
#include "BF_DrebServer.h"
#include "PoolModule.h"
#include "ProcThread.h"
#include "IInitial.h"

typedef struct
{
	CIInitial              *initPoint;//��ʼ����̬�����ָ��
	DLLHANDLE              initdllhandle;//��̬����
	std::string            initdllname;//��̬������
	std::string            initInstance;//ʵ���ĺ�����
	std::string            initdllpath;//��̬��·��������
}S_INITITAL_DLL;

typedef struct
{
	CIExchLink             *exchPoint;//��ʼ����̬�����ָ��
	DLLHANDLE              exchdllhandle;//��̬����
	std::string            exchdllname;//��̬������
	std::string            exchInstance;//ʵ���ĺ�����
	std::string            exchdllpath;//��̬��·��������
}S_EXCHANGE_DLL;

typedef struct 
{
	int sepms;//��ʱ���
	int txcode;//������
}S_TIMER_CONF;

class CTradeMain  : public CIClientLink
{
public:
	
	int proctimer(int id);
	static int OnTimer(unsigned int event, void *p);


	// ������: GetAnsData
	// ���  : ������ 2017-8-14 9:00:10
	// ����  : virtual int 
	// ����  : int index
	// ����  : S_TRADE_DATA &data
	// ����  : int tms
	// ����  : ȡ��Ӧ��
	virtual int GetAnsData(int index,S_TRADE_DATA &data,int tms);


	// ������: PushRcvQueue
	// ���  : ������ 2017-8-14 8:59:42
	// ����  : virtual int 
	// ����  : S_TRADE_DATA &data
	// ����  : �����������
	virtual int PushRcvQueue(S_TRADE_DATA &data);


	// ������: PushAnsQueue
	// ���  : ������ 2017-8-14 9:40:00
	// ����  : virtual int 
	// ����  : S_TRADE_DATA &data   ͨ��b_Cinfo.b_nIndex����Ӧ���ĸ����� b_Cinfo.b_cIndex��ʶ���ĸ������߳�
	// ����  : ����Ӧ�����
	virtual int PushAnsQueue(S_TRADE_DATA &data);

	// ������: Send2Client
	// ���  : ������ 2017-8-14 9:40:12
	// ����  : virtual int 
	// ����  : S_TRADE_DATA &data
	// ����  : ���͸��ͻ��˻�����
	virtual int Send2Client(S_TRADE_DATA &data);


	// ������:  PoolMalloc
	// ���  : ������ 2018-4-13 15:50:23
	// ����  : virtual int 
	// ����  : S_TRADE_DATA &data
	// ����  : ����ռ�
	virtual int  PoolMalloc(S_TRADE_DATA &data);

	// ������: PoolFree
	// ���  : ������ 2018-4-13 15:50:27
	// ����  : virtual void 
	// ����  : S_TRADE_DATA &data
	// ����  : �ͷſռ�
	virtual void PoolFree(S_TRADE_DATA &data);

	void Monitor();
	void Stop();
	bool Start();
	bool Init(const char *confile);
	CTradeMain();
	virtual ~CTradeMain();

	
	CIErrlog         *m_pLog;
	
	CBF_DrebServer   m_pDrebApi;//api
	CDrebMsgThread   m_pDrebMsgThread;//������Ϣ�߳�
	CPoolModule      m_pListenThread;//�����߳�
	CTradeResource   m_pRes;   //����
	CMemDb           m_pMemDb;//�ڴ��

	CProcThread *    m_lProcthread[50];//�����߳���
protected:
	S_BPC_RSMSG      m_drebdata;

	CPoolDataMsg     m_pRcvQueue;

	CBF_Timer        m_pTimer;//��ʱ��
	CXdp             m_pXdp;
	
	
	bool              m_bIsInit;

	S_INITITAL_DLL    m_sInitDll;//��ʼ����̬����Ϣ
	S_EXCHANGE_DLL    m_sExchDll;//��������̬��
	
	std::vector<S_TIMER_CONF> m_pTimerConf;//��ʱ������

	CPubData   g_pPubData;//����������

protected:
	bool OpenDll(DLLHANDLE *pDll, const char *dllname, const char *dllpath);

	char m_sSerialDataFile[256];
};

#endif // !defined(AFX_TRADEMAIN_H__D1862398_E67A_4017_AC64_51E13AE7BEF3__INCLUDED_)
