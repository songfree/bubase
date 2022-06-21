// DrebMsgThread.cpp: implementation of the CDrebMsgThread class.
//
//////////////////////////////////////////////////////////////////////

#include "DrebMsgThread.h"
#include "GateResource.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern int   g_pRunFlag;

CDrebMsgThread::CDrebMsgThread()
{
	m_pLog = NULL;
}

CDrebMsgThread::~CDrebMsgThread()
{
	m_pLog = NULL;
}
bool CDrebMsgThread::Init(CIBF_DrebServer * bus,CBF_DrebResource *res)
{
	m_pDrebApi = bus;
	m_pRes = res;
	m_pLog = m_pDrebApi->GetLogPoint();
	return true;
}


bool CDrebMsgThread::SetGlobalVar(CPoolData* pooldata,CSendData* senddata, CAIO_Work* aio)
{
    m_pPoolData = pooldata;
    m_pSendData = senddata;
    m_pAio = aio;
    
    return true;
}

int CDrebMsgThread::Run()
{
#ifdef _ENGLISH_
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"Start DREB msg thread");
#else
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"启动总线消息处理线程");
#endif
	while (m_bIsRunning)
	{
		if (m_pDrebApi == NULL)
		{
			m_bIsRunning = false;
			return -1;
		}
		if (m_pDrebApi->GetMsgData(m_rMsgData,500,true))
		{
			ProcessDrebData(m_rMsgData);
		}
		else
		{
			SLEEP(1);
		}
	}
#ifdef _ENGLISH_
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"Stop DREB msg thread");
#else
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"停止总线消息处理线程");
#endif
	return 0;
}


// 函数名: OnMsgConnectBack
// 编程  : 王明松 2015-4-23 15:55:29
// 返回  : virtual void 
// 参数  : S_BPC_RSMSG &rcvdata
// 描述  : 处理连接总线成功的响应
void CDrebMsgThread::OnMsgConnectBack(S_BPC_RSMSG &rcvdata)
{
	//发送注册等
#ifdef _ENGLISH_
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"Recv DREB connect rsp，subscribe broadcast");
#else
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"收到DREB连接成功的响应 向总线订阅广播");
#endif
	m_pDrebApi->Subscribe(rcvdata.index,m_lBcRegister);
	
}

// 函数名: OnMsgDrebAns
// 编程  : 王明松 2015-4-23 15:55:58
// 返回  : virtual void 
// 参数  : S_BPC_RSMSG &rcvdata
// 描述  : 响应总线过来的应答数据
void CDrebMsgThread::OnMsgDrebAns(S_BPC_RSMSG &rcvdata)
{
	//应答数据
    S_CGATE_SMSG data;
    bzero(&data.data.head, CGATEHEADLEN);
    data.data.head.nLen = rcvdata.sMsgBuf->sDBHead.nLen;
    if (data.data.head.nLen > 0 && data.data.head.nLen < DREBDATASIZE)
    {
        memcpy(data.data.buffer, rcvdata.sMsgBuf->sBuffer, data.data.head.nLen);
    }
    else if (data.data.head.nLen >= DREBDATASIZE)
    {
        m_pLog->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "数据非法 %d", data.data.head.nLen);
        m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
        rcvdata.sMsgBuf = NULL;
        return;
    }

    data.data.head.nRetCode = rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode;
    data.data.head.stComm.cCmd = rcvdata.sMsgBuf->sDBHead.cCmd;
    data.data.head.stComm.cMsgType = MSG_GATEREQ;
    data.data.head.stComm.cNextFlag = rcvdata.sMsgBuf->sDBHead.cNextFlag;
    data.data.head.stComm.cRaflag = rcvdata.sMsgBuf->sDBHead.cRaflag;
    data.data.head.stComm.cZip = rcvdata.sMsgBuf->sDBHead.cZip;
    data.data.head.stComm.nCrc = 0;
    data.data.head.stDest.cNodePrivateId = rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_cNodePrivateId;
    data.data.head.stDest.cSvrPrivateId = rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_cSvrPrivateId;
    data.data.head.stDest.nNodeId = rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nNodeId;
    data.data.head.stDest.nSerial = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial;
    data.data.head.stDest.nServiceNo = rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo;
    data.data.head.stDest.nSvrMainId = rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nSvrMainId;
    data.data.head.stNext.n_nNextNo = rcvdata.sMsgBuf->sDBHead.n_Ninfo.n_nNextNo;
    data.data.head.stNext.n_nNextOffset = rcvdata.sMsgBuf->sDBHead.n_Ninfo.n_nNextOffset;

    data.txcode = rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo;
    data.d_nNodeId = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId;
    data.d_cNodePrivateId = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId;
    data.s_nDrebSerial = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial;
    data.rtime = time(NULL);

    data.index = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nGateIndex;
    data.timestamp = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook;
    if (data.index == 0 && rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook == 0)
    {
        m_pLog->LogMp(LOG_WARNNING, __FILE__, __LINE__, "无效的应答");
        m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
        rcvdata.sMsgBuf = NULL;
        return;
    }
    data.isBC = 0;
    //最优先响应应答数据
    //统一由一个线程加密并放入发送队列
    m_pSendData->PushData(data, 0);
    //		m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"发送队列数目[%d]",m_pSendData->GetSize());
    m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
    rcvdata.sMsgBuf = NULL;
    return;
}

void CDrebMsgThread::OnMsgRequest(S_BPC_RSMSG& rcvdata)
{
    S_CGATE_SMSG data;
    bzero(&data.data.head, CGATEHEADLEN);
    data.data.head.nLen = rcvdata.sMsgBuf->sDBHead.nLen;
    if (data.data.head.nLen > 0 && data.data.head.nLen < DREBDATASIZE)
    {
        memcpy(data.data.buffer, rcvdata.sMsgBuf->sBuffer, data.data.head.nLen);
    }
    else if (data.data.head.nLen >= DREBDATASIZE)
    {
        m_pLog->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "数据非法 %d", data.data.head.nLen);
        m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
        rcvdata.sMsgBuf = NULL;
        return;
    }
    
    data.s_nDrebSerial = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial;
    data.txcode = rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo;

    data.data.head.nRetCode = 0;
    data.data.head.stComm.cCmd = rcvdata.sMsgBuf->sDBHead.cCmd;
    data.data.head.stComm.cMsgType = MSG_PUSH;
    data.data.head.stComm.cNextFlag = rcvdata.sMsgBuf->sDBHead.cNextFlag;
    data.data.head.stComm.cRaflag = rcvdata.sMsgBuf->sDBHead.cRaflag;
    data.data.head.stComm.cZip = rcvdata.sMsgBuf->sDBHead.cZip;
    data.data.head.stComm.nCrc = 0;
    data.data.head.stDest.cNodePrivateId = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId;
    data.data.head.stDest.cSvrPrivateId = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cSvrPrivateId;
    data.data.head.stDest.nNodeId = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId;

    data.d_nNodeId = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId;
    data.d_cNodePrivateId = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId;
    data.s_nDrebSerial = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial;
    data.rtime = time(NULL);
    data.data.head.stDest.nServiceNo = rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo;
    data.data.head.stDest.nSvrMainId = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSvrMainId;
    data.data.head.stNext.n_nNextNo = 0;
    data.data.head.stNext.n_nNextOffset = 0;

    data.data.head.stDest.nSerial = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial; //订阅模式下，填写交易账户的id (限定交易账户为整型数字)
    data.index = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nGateIndex;
    data.timestamp = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook;


    // #define  CMD_DPCALL      8  //数据总线节点同步调用 要求最终处理完成后应答
    // #define  CMD_DPACALL     9  //数据总线节点异步调用 要求接收到的数据总线节点发送至服务后确认应答
    // #define  CMD_DPBC        10  //数据总线节点广播，即将信息发给指定数据总线节点上所有注册的服务,要求数据总线节点应答
    // #define  CMD_DPABC       11  //数据总线节点广播，即将信息发给指定数据总线节点上所有注册的服务,不要求数据总线节点应答
    // #define  CMD_DPPUSH      12 //数据总线节点推送，无须应答
    // #define  CMD_DPPOST      13 //数据总线节点投递，要求接收到的数据总线节点应答		
    if (rcvdata.sMsgBuf->sDBHead.cCmd == CMD_DPBC || rcvdata.sMsgBuf->sDBHead.cCmd == CMD_DPABC) //广播公共流 订阅信息
    {
         //查找行情是否是最新的，若不是直接丢弃，若是直更新行情表，将数据丢入队列
        if (data.data.head.stDest.nSerial <= 0)  //公共的行情广播等
        {
            data.isBC = 1;
            data.index = 0;
            data.timestamp = 0;
            //第三级别为行情信息     不再判断行情是否最新，api已经去重了
            data.nVerietyId = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook;
            m_pSendData->PushData(data, 2);
            
        }
        else
        {
            data.isBC = 2;    //表示订阅的消息  发送线程要判断是否订阅
            data.index = 0;
            data.timestamp = 0;
            //第三级别为行情信息     不再判断行情是否最新，api已经去重了
            data.nVerietyId = 0;
            m_pSendData->PushData(data, 1);
        }
        m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
        rcvdata.sMsgBuf = NULL;
        return;
    }
    else  if (rcvdata.sMsgBuf->sDBHead.cCmd == CMD_DPCALL || rcvdata.sMsgBuf->sDBHead.cCmd == CMD_DPACALL || \
        rcvdata.sMsgBuf->sDBHead.cCmd == CMD_DPPUSH || rcvdata.sMsgBuf->sDBHead.cCmd == CMD_DPPOST)
        //私有流，指定公告，成交回报，委托回报  这里的回报是后台推送的，不是广播，如果是广播，要走到上面，另外加key的条件来发给指定的客户
    {
        if (data.index == 0)
        {
            m_pLog->LogMp(LOG_WARNNING, __FILE__, __LINE__, "无效的请求 index[%d] cCmd[%d]", data.index, data.data.head.stComm.cCmd);
            m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
            rcvdata.sMsgBuf = NULL;
            return;
        }
        //源的nGateIndex保存用户登录的连接索引   s_nHook保存连接的时间戳
        data.isBC = 0;
        //第二级别为回报信息  
        m_pSendData->PushData(data, 1);
        m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
        rcvdata.sMsgBuf = NULL;
        return;

    }
    m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
    rcvdata.sMsgBuf = NULL;
    return;

}
// 函数名: Start
// 编程  : 王明松 2015-4-23 15:52:29
// 返回  : virtual bool 
// 描述  : 启动处理线程
bool CDrebMsgThread::Start()
{
	if (NULL == m_pDrebApi || NULL == m_pRes)
	{
		return false;
	}
	m_bIsRunning = true;
	CreateThread();
	return true;
}

// 函数名: Stop
// 编程  : 王明松 2015-4-23 15:52:43
// 返回  : virtual void 
// 描述  : 停止处理线程
void CDrebMsgThread::Stop()
{
	m_bIsRunning = false;
	Join();
}
