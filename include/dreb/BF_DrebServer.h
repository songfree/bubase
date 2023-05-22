// BF_DrebServer.h: interface for the CBF_DrebServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BF_DREBSERVER_H__8810FE91_B2B1_4312_9C27_4C97F9219A30__INCLUDED_)
#define AFX_BF_DREBSERVER_H__8810FE91_B2B1_4312_9C27_4C97F9219A30__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "BF_Thread.h"
#include "DrebPubDefine.h"
//#include "BF_Queue.h"
#include "BpcHead.h"
#include "BF_BufferPool.h"
#include "DrebEndian.h"
#include "BF_Timer.h"
#include "BF_SocketTcp.h"
#include "BF_Des.h"
#include "BF_CommDataLog.h"
#include "BF_LogClient.h"
#include "BF_DrebResource.h"
#include "BF_DrebSocketMgr.h"
#include "BF_HostInfo.h"
#include "dreberrcode.h"
#include "BF_Xml.h"
#include "DrebMsgProcBase.h"
#include "IBF_DrebServer.h"
#include <unordered_map>

#ifdef DREBAPI_EXPORTS
#define DREBAPI_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define DREBAPI_EXPORT __declspec(dllimport)
#else
#define DREBAPI_EXPORT 
#endif
#endif

typedef struct _BC_SERIAL
{
	int node_id;
	int node_pid;
	int serial;
	int timestamp;
}S_BC_SERIAL;

class CTbl_BcSerial
{
protected:

    CMemTableNew <S_BC_SERIAL>  m_table;
	CPkeyIntUnordered<3>                 m_keyId;//����ID
    CBF_Mutex                   m_mutex;//������
public:
	int  m_nDeleteTime;	 //����ɾ��ʱ�� ��
    CBF_BufferPool* m_pMemPool;//�ڴ�����;
	CTbl_BcSerial()
    {
		m_nDeleteTime = 60;
    }
    virtual ~CTbl_BcSerial()
    {

    }
    void Clear()
    {
        m_keyId.Clear();
        m_table.Clear();
    }
    int Insert(S_BC_SERIAL info)
    {
		CBF_PMutex pp(&m_mutex);
        if (m_keyId.Find(info.node_id,info.node_pid,info.serial))
        {
            return -1;
        }
        int rid = m_table.Add(info);
        m_keyId.Add(rid, m_table.m_table[rid].node_id, m_table.m_table[rid].node_pid, m_table.m_table[rid].serial);
        return rid;
    }
	S_BC_SERIAL* Select(int nodeid,int nodepid,int serial)
    {
        int rid;
        CBF_PMutex pp(&m_mutex);
        if (!m_keyId.Select(rid, nodeid, nodepid,serial))
        {
            return NULL;
        }
        return &(m_table.m_table[rid]);
    }

	S_BC_SERIAL* At(int rowid)
    {
        if (rowid<0 || rowid > m_table.AllSize() - 1)
        {
            return NULL;
        }
        return &(m_table.m_table[rowid]);
    }
    
    int Delete()
    {
        int rid;
        CBF_PMutex pp(&m_mutex);
        CInt iset;
        bool bret = m_keyId.First(rid);
        while (bret)
        {
            if (time(NULL) - m_table.m_table[rid].timestamp > m_nDeleteTime) //����1���ӵģ�ɾ��
            {
                iset.Add(rid);
            }
            bret = m_keyId.Next(rid);
        }
        //m_keyId.Delete(iset);
        bret = iset.First(rid);
        while (bret)
        {
			m_keyId.Delete(m_table.m_table[rid].node_id, m_table.m_table[rid].node_pid, m_table.m_table[rid].serial);
            m_table.Delete(rid);
            bret = iset.Next(rid);
        }

        return 0;
    }
    int Size()
    {
		CBF_PMutex pp(&m_mutex);
        return m_keyId.Size();
    }
};

class DREBAPI_EXPORT CBF_DrebServer : public CIBF_DrebServer,CBF_Thread
{
public:

    // ������: Subscribe
    // ���  : ������ 20220614 14:09:01
    // ����  : void 
    // ����  : int index        dreb��������
    // ����  : CInt *funclist   �㲥�����б�
    // ����  : �����ӵ�DREB�Ϲ㲥������Ϣ�����������ϵĹ㲥��Ϣ�����㲥������û��ָ���˹㲥�ķ������ʱ���򰴴˶���������
    virtual void Subscribe(int index, std::vector<int>* funclist);


    // ������: UnSubscribe
    // ���  : ������ 20220614 14:09:09
    // ����  : void 
    // ����  : ȡ����dreb�Ϲ㲥����
    virtual void UnSubscribe();

	virtual int GetDataLogLevel();

	// ������: GetErrMsg
	// ���  : ������ 2015-7-21 10:28:00
	// ����  : std::string 
	// ����  : ȡ�ó�����Ϣ
	virtual std::string GetErrMsg();

	// ������: SetDataLogLevel
	// ���  : ������ 2015-7-15 15:01:38
	// ����  : void 
	// ����  : int loglevel
	// ����  : ���ñ�����־���𣬼�ʱ��Ч
	virtual void SetDataLogLevel(int loglevel);


	// ������: UnzipBuf
	// ���  : ������ 2014-12-15 8:47:49
	// ����  : bool 
	// ����  : BPCCOMMSTRU &data
	// ����  : �⿪�������ݣ���Լ��ָ��
	virtual bool UnzipBuf(BPCCOMMSTRU &data);

	// ������: GetLogPoint
	// ���  : ������ 2014-12-12 10:31:28
	// ����  : CIErrlog * 
	// ����  : ������־��ָ�룬��ͨ����ָ��д����־��Ϣ
	virtual CIErrlog * GetLogPoint();

	// ������: GetDrebInfo
	// ���  : ������ 2014-12-10 16:35:34
	// ����  : bool 
	// ����  : int index  ��������
	// ����  : char *sip  ip��ַ
	// ����  : int &port  �˿�
	// ����  : int &drebid   �����ڵ��
	// ����  : int &privateid  ˽�нڵ��
	// ����  : char *status  ״̬   "δ����"  "����"   "��������"
	// ����  : ��ȡDREB����������Ϣ
	virtual bool GetDrebInfo(int index,char *sip,int &port,int &drebid,int &privateid,char *status);

	// ������: GetBufferPoolInfo
	// ���  : ������ 2014-12-10 16:30:51
	// ����  : void 
	// ����  : int &total    �ܹ������˶��ٿ�
	// ����  : int &unused   ���ж��ٿ�δʹ��
	// ����  : int &bufsize  ÿ���С
	// ����  : ��ȡ�ڴ滺�����Ϣ
	virtual void GetBufferPoolInfo(int &total, int &unused,int &bufsize);

	// ������: RegisterDreb
	// ���  : ������ 2013-4-8 15:10:22
	// ����  : void 
	// ����  : int index        dreb��������
	// ����  : CInt *funclist   �����б�
	// ����  : �����ӵ�DREB�Ϸ�ע����Ϣ,��������ע�ᵽ������,�������԰�������·�ɣ����㲥���ܴ����ƣ��ɵ��ö��������ע��
	virtual void RegisterDreb(int index, std::vector<int> *funclist);


	// ������: UnRegisterDreb
	// ���  : ������ 2014-12-12 10:07:43
	// ����  : void 
	// ����  : ȡ����dreb��ע��Ľ���
	virtual void UnRegisterDreb();

	// ������: GetMsgData
	// ���  : ������ 2014-12-10 15:06:39
	// ����  : bool true�ɹ� 
	// ����  : S_BPC_RSMSG &rdata  ��Ϣ
	// ����  : unsigned int waitetms=1000  û����Ϣ�ĵȴ�ʱ��
	// ����  : bool get_front=true  true��ͷȡ,false��βȡ 
	// ����  : �Ӷ�����ȡ��Ϣ���д���ע��Ҫ����poolfree�ͷ�rdata��Ŀռ�
	virtual bool GetMsgData(S_BPC_RSMSG &rdata,unsigned int waitetms=1000,bool get_front=true);

	// ������: SendMsg
	// ���  : ������ 2014-12-10 15:00:22
	// ����  : int  -1����ʧ�� 0���ͳɹ�  >0�������
	// ����  : S_BPC_RSMSG &sdata
	// ����  : ��DREB�������� ע��bpchead���index��д������Ӧ��ԭ�����ɣ����������һ���Ƚϴ��ֵ�������ñ�apiѡ��һ�����ӷ���
	//          ע���ڱ�������������ͷ�sdata�ռ�
	//          a_cNodePrivateId����ʶ�Ƿ�ת�ƽ��ף��������յ��жϴ˱�ʶΪ100ʱ������s_info��������
	virtual int SendMsg(S_BPC_RSMSG &sdata);

	int GetIndex();

	// ������: Stop
	// ���  : ������ 2014-12-10 14:15:10
	// ����  : void 
	// ����  : ֹͣapi�߳�
	virtual void Stop();

	// ������: Start
	// ���  : ������ 2014-12-10 14:14:58
	// ����  : bool 
	// ����  : ����api�߳�
	virtual bool Start();


	// ������: MonitorThread
	// ���  : ������ 2014-12-15 8:48:34
	// ����  : void 
	// ����  : ����߳��Ƿ�ֹͣ����ͣ��������
	virtual void MonitorThread();

	// ������: Init
	// ���  : ������ 2014-12-5 15:52:02
	// ����  : bool 
	// ����  : CBF_DrebResource *res
	// ����  :,CDrebMsgProcBase* spi=NULL   ���лص�ʱ��api�߳�ֱ�ӻص�������������
	// ����  : ��api��ʼ��
	virtual bool Init(CBF_DrebResource *res,CDrebMsgProcBase* spi=NULL);

	// ������: PoolFree
	// ���  : ������ 2014-12-5 11:32:37
	// ����  : void 
	// ����  : void *ppoint
	// ����  : �ͷſռ�
	virtual void PoolFree(void *ppoint);

	// ������: PoolMalloc
	// ���  : ������ 2014-12-5 11:32:41
	// ����  : void * 
	// ����  : ����һ���ռ�
	virtual void * PoolMalloc();

	virtual CBF_BufferPool *GetBufferPool();

	// ������: GetHostInfo
	// ���  : ������ 2014-12-10 14:27:21
	// ����  : void 
	// ����  : S_MONITOR_HOST &host
	// ����  : vector<S_MONITOR_DISK>&disk
	// ����  : ��ȡ����cpu���ڴ桢������Ϣ��ǰ������Դ�ı�־g_nMonitorHostΪ1
	virtual void GetHostInfo(S_MONITOR_HOST &host, std::vector<S_MONITOR_DISK>&disk);

	CBF_DrebServer();
	virtual ~CBF_DrebServer();

	CDrebMsgProcBase* m_spi;//�ص���ָ��
private:
	// ������: Run
	// ���  : ������ 2010-3-12 13:00:15
	// ����  : virtual int 
	// ����  : ���շ����߳�������
	virtual int Run();
	virtual bool InitThreadInstance();
	virtual void ExitThreadInstance();

	// ������: GetMaxfdAddSelect
	// ���  : ������ 2009-5-21 11:51:00
	// ����  : int 
	// ����  : int &maxfd
	// ����  : ��ȡ����socket����socket���뵽select����
	int GetMaxfdAddSelect(int &maxfd);

	// ������: OnConnect
	// ���  : ������ 2010-8-13 14:56:53
	// ����  : void 
	// ����  : int conindex
	// ����  : ����dreb
	void OnConnect(int conindex);
	
	// ������: OnConnected
	// ���  : ������ 2010-8-13 14:57:06
	// ����  : void 
	// ����  : int conindex
	// ����  : ��������dreb,����ע����Ϣ
	void OnConnected(int conindex);

	// ������: OnEvent
	// ���  : ������ 2009-5-21 13:16:19
	// ����  : void 
	// ����  : select�����¼��Ĵ���,������ݡ�������
	void OnEvent();
	
	// ������: OnNoEvent
	// ���  : ������ 2009-5-21 13:16:35
	// ����  : void 
	// ����  : select�����¼��Ĵ���������
	void OnNoEvent();

	// ������: OnMonitor
	// ���  : ������ 2013-11-25 14:15:07
	// ����  : void 
	// ����  : �������ͼ�ر���
	void OnMonitor();

	// ������: ResetData
	// ���  : ������ 2010-3-12 15:59:52
	// ����  : bool 
	// ����  : int index
	// ����  : S_FPC_RSMSG *data
	// ����  : ��������
	bool ReMallocData(int index,S_BPC_RSMSG *data);

	// ������: OnWriteEvent
	// ���  : ������ 2009-5-21 15:17:24
	// ����  : void 
	// ����  : int conindex
	// ����  : ��Ӧselectд�¼�
	void OnWriteEvent(int conindex);
	
	// ������: OnReadEvent
	// ���  : ������ 2009-5-21 15:17:03
	// ����  : void 
	// ����  : int conindex
	// ����  : ��Ӧselect���¼�
	void OnReadEvent(int conindex);

	// ������: OnPing
	// ���  : ������ 2009-5-21 13:17:22
	// ����  : void 
	// ����  : int conindex ���ӵ��±�
	// ����  : ��������
	void OnPing(int conindex);

	// ������: OnClose
	// ���  : ������ 2009-5-21 13:18:27
	// ����  : void 
	// ����  : int conindex ���ӵ��±�
	// ����  : const char *msg �ر�����ԭ��
	// ����  : �رտͻ��˻���������
	void OnClose(int conindex,const char *msg,const char *filename,int fileline);

	// ������: OnRecv
	// ���  : ������ 2010-3-12 14:06:55
	// ����  : void 
	// ����  : int conindex
	// ����  : �������ݲ�����
	void OnRecv(int conindex);

	// ������: OnSend
	// ���  : ������ 2009-5-21 15:16:33
	// ����  : void 
	// ����  : int conindex
	// ����  : ��������
	void OnSend(int conindex);

	// ������: ProcessDreb
	// ���  : ������ 2010-8-13 14:54:46
	// ����  : void 
	// ����  : S_BPC_RSMSG &rcvdata
	// ����  : ����DREB����������
	void ProcessDreb(S_BPC_RSMSG &rcvdata);

	// ������: OnTimer
	// ���  : ������ 2013-3-6 16:43:23
	// ����  : static int 
	// ����  : unsigned int event
	// ����  : void *p
	// ����  : ��ʱ���ص�����
	static int OnTimer(unsigned int event,void *p);

	// ������: GetHostInfo
	// ���  : ������ 2013-3-8 10:19:24
	// ����  : void 
	// ����  : ��ʱ��ȡ������Ϣ
	void GetHostInfo();
	
	// ������: ComputeStat
	// ���  : ������ 2013-3-8 10:19:28
	// ����  : void 
	// ����  : ��ʱ��ȡͳ�����ݼ���
	void ComputeStat();


	unsigned int m_nBegin; //socket��ʼ���
	unsigned int m_nEnd;   //socket�������

	//std::atomic<unsigned int> m_nCurIndex;  //����ʱ��һ����������ѡ�����
	unsigned int m_nCurIndex;  //����ʱ��һ����������ѡ�����
protected: 

	CTbl_BcSerial m_bcSerial;//��¼�㲥����Ϣ������Ϊdreb node_id node_pid derbserial
	bool m_bIsPingTimer;      //�Ƿ�Ҫ���PING
	bool m_bIsInit;          //�Ƿ����Init����
	char m_errMsg[256];      //������Ϣ
	bool m_bIsRunning;       //ֹͣ�����̱߳�־  1���� 0ֹͣ

	CBF_Des        m_pDes;  //DES��
	CBF_Timer      m_pTimer;//��ʱ��
	CDrebEndian    m_pDrebEndian;//�ֽ���
	S_BPC_RSMSG    m_pDataBuf;//�������ݽṹ
	S_BPC_RSMSG    m_pDataRcv;//�������ݽṹ
	CBF_SocketTcp  m_tcpSocket;
	//ע��S_BPC_RSMSG�����PBPCCOMMSTRU��һ���ɳ��������ڴ�ռ䣬��Ҫ�ͷš�
	CBF_Stack<S_BPC_RSMSG>  m_qRcvQueue;  //�������յ����߳���Ҫ�ⲿ��������ݶ���   S_BPC_RSMSGΪ���շ�����Ϣ�ṹ

	CBF_BufferPool          m_pMemPool;//�ڴ滺��أ����н��գ����͵����ݿռ���ɴ��������������ɴ����ͷš� 
	CBF_CommDataLog         m_pDrebDataLog;//��¼�����߹���������Ӧ������ߵı�����־��
	CBF_CommDataLog         m_pBpcCallDataLog;//��¼�������ߵ�����Ӧ�����API�ı�����־��
	CBF_LogClient           m_pLog;    //��־��
	CBF_DrebSocketMgr       m_pSockMgr;//���ӹ�����
	CBF_HostInfo            m_pHostInfo;

	CBF_DrebResource       *m_pRes;//ȫ�ֵĲ����� ��������init���뱾API

	

	S_MONITOR_HOST          m_sHostInfo;//cpu�ڴ���Ϣ 
	V_DiskInfo              m_vdinfo;   //������Ϣ
private:
	

	fd_set m_rset;
	fd_set m_wset;
#if defined(_WINDOWS)
	fd_set m_eset;
#endif

	CBF_Mutex  m_pHostMutex;
	CBF_Mutex  m_pindexmutex;
};

#endif // !defined(AFX_BF_DREBSERVER_H__8810FE91_B2B1_4312_9C27_4C97F9219A30__INCLUDED_)
