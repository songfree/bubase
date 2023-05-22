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
	CPkeyIntUnordered<3>                 m_keyId;//请求ID
    CBF_Mutex                   m_mutex;//互斥锁
public:
	int  m_nDeleteTime;	 //过期删除时间 秒
    CBF_BufferPool* m_pMemPool;//内存分配池;
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
            if (time(NULL) - m_table.m_table[rid].timestamp > m_nDeleteTime) //大于1分钟的，删除
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

    // 函数名: Subscribe
    // 编程  : 王明松 20220614 14:09:01
    // 返回  : void 
    // 参数  : int index        dreb连接索引
    // 参数  : CInt *funclist   广播功能列表
    // 描述  : 在连接的DREB上广播订阅消息，订阅总线上的广播消息，当广播生产者没有指定此广播的服务对象时，则按此订阅来发送
    virtual void Subscribe(int index, std::vector<int>* funclist);


    // 函数名: UnSubscribe
    // 编程  : 王明松 20220614 14:09:09
    // 返回  : void 
    // 描述  : 取消在dreb上广播订阅
    virtual void UnSubscribe();

	virtual int GetDataLogLevel();

	// 函数名: GetErrMsg
	// 编程  : 王明松 2015-7-21 10:28:00
	// 返回  : std::string 
	// 描述  : 取得出错信息
	virtual std::string GetErrMsg();

	// 函数名: SetDataLogLevel
	// 编程  : 王明松 2015-7-15 15:01:38
	// 返回  : void 
	// 参数  : int loglevel
	// 描述  : 设置报文日志级别，即时生效
	virtual void SetDataLogLevel(int loglevel);


	// 函数名: UnzipBuf
	// 编程  : 王明松 2014-12-15 8:47:49
	// 返回  : bool 
	// 参数  : BPCCOMMSTRU &data
	// 描述  : 解开报文数据，针对监控指令
	virtual bool UnzipBuf(BPCCOMMSTRU &data);

	// 函数名: GetLogPoint
	// 编程  : 王明松 2014-12-12 10:31:28
	// 返回  : CIErrlog * 
	// 描述  : 返回日志类指针，可通过此指针写入日志信息
	virtual CIErrlog * GetLogPoint();

	// 函数名: GetDrebInfo
	// 编程  : 王明松 2014-12-10 16:35:34
	// 返回  : bool 
	// 参数  : int index  连接索引
	// 参数  : char *sip  ip地址
	// 参数  : int &port  端口
	// 参数  : int &drebid   公共节点号
	// 参数  : int &privateid  私有节点号
	// 参数  : char *status  状态   "未连接"  "正常"   "正在连接"
	// 描述  : 获取DREB总线连接信息
	virtual bool GetDrebInfo(int index,char *sip,int &port,int &drebid,int &privateid,char *status);

	// 函数名: GetBufferPoolInfo
	// 编程  : 王明松 2014-12-10 16:30:51
	// 返回  : void 
	// 参数  : int &total    总共分配了多少块
	// 参数  : int &unused   还有多少块未使用
	// 参数  : int &bufsize  每块大小
	// 描述  : 获取内存缓冲池信息
	virtual void GetBufferPoolInfo(int &total, int &unused,int &bufsize);

	// 函数名: RegisterDreb
	// 编程  : 王明松 2013-4-8 15:10:22
	// 返回  : void 
	// 参数  : int index        dreb连接索引
	// 参数  : CInt *funclist   交易列表
	// 描述  : 在连接的DREB上发注册消息,将交易码注册到总线上,这样可以按交易码路由，但广播不受此限制，可调用多次来进行注册
	virtual void RegisterDreb(int index, std::vector<int> *funclist);


	// 函数名: UnRegisterDreb
	// 编程  : 王明松 2014-12-12 10:07:43
	// 返回  : void 
	// 描述  : 取消在dreb上注册的交易
	virtual void UnRegisterDreb();

	// 函数名: GetMsgData
	// 编程  : 王明松 2014-12-10 15:06:39
	// 返回  : bool true成功 
	// 参数  : S_BPC_RSMSG &rdata  消息
	// 参数  : unsigned int waitetms=1000  没有消息的等待时间
	// 参数  : bool get_front=true  true从头取,false从尾取 
	// 描述  : 从队列里取消息进行处理，注意要调用poolfree释放rdata里的空间
	virtual bool GetMsgData(S_BPC_RSMSG &rdata,unsigned int waitetms=1000,bool get_front=true);

	// 函数名: SendMsg
	// 编程  : 王明松 2014-12-10 15:00:22
	// 返回  : int  -1发送失败 0发送成功  >0放入队列
	// 参数  : S_BPC_RSMSG &sdata
	// 描述  : 向DREB发送数据 注意bpchead里的index填写，若是应答原样即可，新请求可填一个比较大的值，这样让本api选择一个连接发送
	//          注意在本方法里会主动释放sdata空间
	//          a_cNodePrivateId来标识是否转移交易，当总线收到判断此标识为100时，不对s_info进行重置
	virtual int SendMsg(S_BPC_RSMSG &sdata);

	int GetIndex();

	// 函数名: Stop
	// 编程  : 王明松 2014-12-10 14:15:10
	// 返回  : void 
	// 描述  : 停止api线程
	virtual void Stop();

	// 函数名: Start
	// 编程  : 王明松 2014-12-10 14:14:58
	// 返回  : bool 
	// 描述  : 启动api线程
	virtual bool Start();


	// 函数名: MonitorThread
	// 编程  : 王明松 2014-12-15 8:48:34
	// 返回  : void 
	// 描述  : 监控线程是否停止，若停了则重启
	virtual void MonitorThread();

	// 函数名: Init
	// 编程  : 王明松 2014-12-5 15:52:02
	// 返回  : bool 
	// 参数  : CBF_DrebResource *res
	// 参数  :,CDrebMsgProcBase* spi=NULL   当有回调时，api线程直接回调，否则放入队列
	// 描述  : 本api初始化
	virtual bool Init(CBF_DrebResource *res,CDrebMsgProcBase* spi=NULL);

	// 函数名: PoolFree
	// 编程  : 王明松 2014-12-5 11:32:37
	// 返回  : void 
	// 参数  : void *ppoint
	// 描述  : 释放空间
	virtual void PoolFree(void *ppoint);

	// 函数名: PoolMalloc
	// 编程  : 王明松 2014-12-5 11:32:41
	// 返回  : void * 
	// 描述  : 分配一个空间
	virtual void * PoolMalloc();

	virtual CBF_BufferPool *GetBufferPool();

	// 函数名: GetHostInfo
	// 编程  : 王明松 2014-12-10 14:27:21
	// 返回  : void 
	// 参数  : S_MONITOR_HOST &host
	// 参数  : vector<S_MONITOR_DISK>&disk
	// 描述  : 获取主机cpu、内存、磁盘信息，前提是资源的标志g_nMonitorHost为1
	virtual void GetHostInfo(S_MONITOR_HOST &host, std::vector<S_MONITOR_DISK>&disk);

	CBF_DrebServer();
	virtual ~CBF_DrebServer();

	CDrebMsgProcBase* m_spi;//回调类指针
private:
	// 函数名: Run
	// 编程  : 王明松 2010-3-12 13:00:15
	// 返回  : virtual int 
	// 描述  : 接收发送线程主方法
	virtual int Run();
	virtual bool InitThreadInstance();
	virtual void ExitThreadInstance();

	// 函数名: GetMaxfdAddSelect
	// 编程  : 王明松 2009-5-21 11:51:00
	// 返回  : int 
	// 参数  : int &maxfd
	// 描述  : 获取最大的socket并将socket加入到select集合
	int GetMaxfdAddSelect(int &maxfd);

	// 函数名: OnConnect
	// 编程  : 王明松 2010-8-13 14:56:53
	// 返回  : void 
	// 参数  : int conindex
	// 描述  : 连接dreb
	void OnConnect(int conindex);
	
	// 函数名: OnConnected
	// 编程  : 王明松 2010-8-13 14:57:06
	// 返回  : void 
	// 参数  : int conindex
	// 描述  : 已连接上dreb,发送注册信息
	void OnConnected(int conindex);

	// 函数名: OnEvent
	// 编程  : 王明松 2009-5-21 13:16:19
	// 返回  : void 
	// 描述  : select后有事件的处理,如读数据、发数据
	void OnEvent();
	
	// 函数名: OnNoEvent
	// 编程  : 王明松 2009-5-21 13:16:35
	// 返回  : void 
	// 描述  : select后无事件的处理，如心跳
	void OnNoEvent();

	// 函数名: OnMonitor
	// 编程  : 王明松 2013-11-25 14:15:07
	// 返回  : void 
	// 描述  : 主动发送监控报告
	void OnMonitor();

	// 函数名: ResetData
	// 编程  : 王明松 2010-3-12 15:59:52
	// 返回  : bool 
	// 参数  : int index
	// 参数  : S_FPC_RSMSG *data
	// 描述  : 重置数据
	bool ReMallocData(int index,S_BPC_RSMSG *data);

	// 函数名: OnWriteEvent
	// 编程  : 王明松 2009-5-21 15:17:24
	// 返回  : void 
	// 参数  : int conindex
	// 描述  : 响应select写事件
	void OnWriteEvent(int conindex);
	
	// 函数名: OnReadEvent
	// 编程  : 王明松 2009-5-21 15:17:03
	// 返回  : void 
	// 参数  : int conindex
	// 描述  : 响应select读事件
	void OnReadEvent(int conindex);

	// 函数名: OnPing
	// 编程  : 王明松 2009-5-21 13:17:22
	// 返回  : void 
	// 参数  : int conindex 连接的下标
	// 描述  : 发送心跳
	void OnPing(int conindex);

	// 函数名: OnClose
	// 编程  : 王明松 2009-5-21 13:18:27
	// 返回  : void 
	// 参数  : int conindex 连接的下标
	// 参数  : const char *msg 关闭连接原因
	// 描述  : 关闭客户端或服务端连接
	void OnClose(int conindex,const char *msg,const char *filename,int fileline);

	// 函数名: OnRecv
	// 编程  : 王明松 2010-3-12 14:06:55
	// 返回  : void 
	// 参数  : int conindex
	// 描述  : 接收数据并处理
	void OnRecv(int conindex);

	// 函数名: OnSend
	// 编程  : 王明松 2009-5-21 15:16:33
	// 返回  : void 
	// 参数  : int conindex
	// 描述  : 发送数据
	void OnSend(int conindex);

	// 函数名: ProcessDreb
	// 编程  : 王明松 2010-8-13 14:54:46
	// 返回  : void 
	// 参数  : S_BPC_RSMSG &rcvdata
	// 描述  : 处理DREB过来的数据
	void ProcessDreb(S_BPC_RSMSG &rcvdata);

	// 函数名: OnTimer
	// 编程  : 王明松 2013-3-6 16:43:23
	// 返回  : static int 
	// 参数  : unsigned int event
	// 参数  : void *p
	// 描述  : 定时器回调方法
	static int OnTimer(unsigned int event,void *p);

	// 函数名: GetHostInfo
	// 编程  : 王明松 2013-3-8 10:19:24
	// 返回  : void 
	// 描述  : 定时器取主机信息
	void GetHostInfo();
	
	// 函数名: ComputeStat
	// 编程  : 王明松 2013-3-8 10:19:28
	// 返回  : void 
	// 描述  : 定时器取统计数据计算
	void ComputeStat();


	unsigned int m_nBegin; //socket开始序号
	unsigned int m_nEnd;   //socket结束序号

	//std::atomic<unsigned int> m_nCurIndex;  //发送时的一个连接索引选择变量
	unsigned int m_nCurIndex;  //发送时的一个连接索引选择变量
protected: 

	CTbl_BcSerial m_bcSerial;//记录广播的信息，主键为dreb node_id node_pid derbserial
	bool m_bIsPingTimer;      //是否要检查PING
	bool m_bIsInit;          //是否调用Init方法
	char m_errMsg[256];      //出错信息
	bool m_bIsRunning;       //停止运行线程标志  1运行 0停止

	CBF_Des        m_pDes;  //DES类
	CBF_Timer      m_pTimer;//定时器
	CDrebEndian    m_pDrebEndian;//字节序
	S_BPC_RSMSG    m_pDataBuf;//发送数据结构
	S_BPC_RSMSG    m_pDataRcv;//接收数据结构
	CBF_SocketTcp  m_tcpSocket;
	//注意S_BPC_RSMSG里面的PBPCCOMMSTRU是一个由程序分配的内存空间，需要释放。
	CBF_Stack<S_BPC_RSMSG>  m_qRcvQueue;  //从总线收到或本线程需要外部处理的数据队列   S_BPC_RSMSG为接收发送消息结构

	CBF_BufferPool          m_pMemPool;//内存缓冲池，所有接收，发送的数据空间皆由此类分配出来，并由此类释放。 
	CBF_CommDataLog         m_pDrebDataLog;//记录从总线过来的请求及应答给总线的报文日志类
	CBF_CommDataLog         m_pBpcCallDataLog;//记录发往总线的请求及应答给本API的报文日志类
	CBF_LogClient           m_pLog;    //日志类
	CBF_DrebSocketMgr       m_pSockMgr;//连接管理类
	CBF_HostInfo            m_pHostInfo;

	CBF_DrebResource       *m_pRes;//全局的参数类 需程序调用init传入本API

	

	S_MONITOR_HOST          m_sHostInfo;//cpu内存信息 
	V_DiskInfo              m_vdinfo;   //磁盘信息
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
