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

typedef struct
{
    int request_id;//rpc����id
    int rpctime;//rpc����ʱ��
	std::function<int(S_TRADE_DATA& reqdata, S_TRADE_DATA& ansdata)> func;
	S_TRADE_DATA data;//����������
}S_RPC_DATA;
//��Լ��
class CTbl_RpcData
{
protected:

    CMemTableNew <S_RPC_DATA>  m_table;
    CPkeyInt<1>                 m_keyId;//����ID
    CBF_Mutex                   m_mutex;//������
public:

	CBF_BufferPool* m_pMemPool;//�ڴ�����;
    CTbl_RpcData()
    {

    }
    virtual ~CTbl_RpcData()
    {

    }
    void Clear()
    {
        m_keyId.Clear();
        m_table.Clear();
    }
    int Insert(S_RPC_DATA info)
    {
        if (m_keyId.Find(info.request_id))
        {
            return -1;
        }
        int rid = m_table.Add(info);
        m_keyId.Add(rid, m_table.m_table[rid].request_id);
        return rid;
    }
    S_RPC_DATA* Select(int requestid)
    {
        int rid;
        CBF_PMutex pp(&m_mutex);
        if (!m_keyId.Select(rid, requestid))
        {
            return NULL;
        }
        return &(m_table.m_table[rid]);
    }
    bool Select(int requestid, S_RPC_DATA &data,bool delflag=true)
    {
        int rid;
        CBF_PMutex pp(&m_mutex);
        if (!m_keyId.Select(rid, requestid))
        {
            return false;
        }
		// memcpy(&data,&(m_table.m_table[rid]),sizeof(S_RPC_DATA));
		data.request_id = m_table.m_table[rid].request_id;
		data.rpctime = m_table.m_table[rid].rpctime;
		data.data.pData = m_table.m_table[rid].data.pData;
		data.func = m_table.m_table[rid].func;
		data.data.index = m_table.m_table[rid].data.index;
		data.data.nSendlen = m_table.m_table[rid].data.nSendlen;
		data.data.nWantlen = m_table.m_table[rid].data.nWantlen;
		data.data.rTime = m_table.m_table[rid].data.rTime;
		data.data.src = m_table.m_table[rid].data.src;
		if (delflag)
		{
            m_keyId.Delete(rid);
            m_table.Delete(rid);
		}
        return true;
    }

    S_RPC_DATA* At(int rowid)
    {
        if (rowid<0 || rowid > m_table.AllSize() - 1)
        {
            return NULL;
        }
        return &(m_table.m_table[rowid]);
    }
    int Delete(int requestid)
    {
        int rid;
        CBF_PMutex pp(&m_mutex);
        if (!m_keyId.Select(rid, requestid))
        {
            return -1;
        }

		m_keyId.Delete(requestid);
		m_table.Delete(rid);
        return 0;
    }
    int Delete()
    {
        int rid;
        CBF_PMutex pp(&m_mutex);
		CInt iset;
        bool bret = m_keyId.First(rid);
		while (bret)
		{
			if (m_table.m_table[rid].rpctime - time(NULL) > 3600) //1Сʱ
			{
				iset.Add(rid);
			}
			bret = m_keyId.Next(rid);
		}
        m_keyId.Delete(iset);
		bret = iset.First(rid);
		while (bret)
		{
			if (m_pMemPool != NULL && m_table.m_table[rid].data.pData !=NULL)
			{
				m_pMemPool->PoolFree(m_table.m_table[rid].data.pData);
				m_table.m_table[rid].data.pData = NULL;
			}
			
			m_table.Delete(rid);
			bret = iset.Next(rid);
		}
        
        return 0;
    }
	int Size()
	{
		return m_keyId.Size();
	}
};

class CTradeMain  : public CIClientLink
{
public:
    // ������: AnsData
   // ���  : ������ 2017-7-20 9:28:49
   // ����  : virtual int 
   // ����  : S_TRADE_DATA &data  src=1ʱdata.pData->sBpcHead.nIndexΪsocket�������
   // ����  : char nextflag  0�޺�����  1 �к�����  10���һ�����ݰ�
   // ����  : Ӧ������  ���ݸ�ʽΪ����ͷ+����  src=0��ʾ����  src=1��ʾsocketֱ��
    virtual int AnsData(S_TRADE_DATA& data, char nextflag);

    // ������: RpcRequest
    // ���  : ������ 2017-7-20 9:28:49
    // ����  : virtual int 
    // ����  : S_TRADE_DATA &data  src=1ʱdata.pData->sBpcHead.nIndexΪsocket�������
    // ����  : char nextflag  0�޺�����  1 �к�����  10���һ�����ݰ�
    // ����  : Ӧ������  ���ݸ�ʽΪ����ͷ+����  src=0��ʾ����  src=1��ʾsocketֱ��
    virtual int RpcRequest(S_TRADE_DATA& data, int requestid, std::function<int(S_TRADE_DATA& reqdata, S_TRADE_DATA& ansdata)> func);

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

    //ȡ��������ˮ��
    virtual int  GetSerial();
    //ȡ�����ı��غ�
    virtual UINT64_ GetLocalNo();

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

	CTbl_RpcData     m_pRpcData;//rpc���õ���������

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
