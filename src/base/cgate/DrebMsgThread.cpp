// DrebMsgThread.cpp: implementation of the CDrebMsgThread class.
//
//////////////////////////////////////////////////////////////////////

#include "DrebMsgThread.h"
#include "GateResource.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern int   g_pRunFlag;

#define  DESENCRYPTKEY    "rd402wms"
#define  DESENCRYPTKEY3   "wms13711songfree"

CDrebMsgThread::CDrebMsgThread()
{
	m_pLog = NULL;
    g_pGateRes = NULL;
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
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"Recv DREB index[%d]connect rsp，subscribe broadcast", rcvdata.sMsgBuf->sBpcHead.nIndex);
#else
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"收到DREB index[%d]连接成功的响应 向总线订阅广播", rcvdata.sMsgBuf->sBpcHead.nIndex);
#endif
    std::vector<int> lBcRegister;
    g_pGateRes->g_lBCFuncList.Select(lBcRegister);
	m_pDrebApi->Subscribe(rcvdata.sMsgBuf->sBpcHead.nIndex, &lBcRegister);
	
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

    data.data.head.stDest.nSerial = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial; 
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
        //行情时s_nSerial为行情日期，s_nHook为key(合约或证券id) s_nGateIndex为行情时间或序号
        //20221211 根据配置的广播级别，放入对应的发送队列，成交回报、报单回报等为1  行情等为2
        int level = 2;

        S_BC_INFO* bcinfo = g_pGateRes->g_lBCFuncList.Select(rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo);
        if (bcinfo != NULL)
        {
            level = bcinfo->nLevel;
        }
        m_pLog->LogMp(LOG_DEBUG+1, __FILE__, __LINE__, "广播 行情 func=%d date=%d code=%d time=%d", rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial, rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook, rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nGateIndex);
        //控制行情的发送，按交易时间或行情序号，后到的不发
        if (level == 2 && g_pGateRes->g_nFilterQuo == 1)
        {
            if (m_pQuotation.IsNewQuotation(rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nGateIndex) > 0)
            {
                //放入队列，待发送
                m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "广播 行情 func=%d date=%d code=%d time=%d", rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo, rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial, rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook, rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nGateIndex);
                data.nkey = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook;
                data.isBC = 1;
                data.index = 0;
                data.timestamp = 0;
                m_pSendData->PushData(data, level);
                m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
                rcvdata.sMsgBuf = NULL;
                
            }
            else
            {
                m_pLog->LogMp(LOG_PROMPT, __FILE__, __LINE__, "广播 行情 func=%d date=%d code=%d time=%d不是最新，丢弃", rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo, rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial, rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook, rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nGateIndex);
                m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
                rcvdata.sMsgBuf = NULL;
            }
            return;
        }
        else
        {
            //放入队列，待发送
            data.nkey = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook;
            data.isBC = 1;
            data.index = 0;
            data.timestamp = 0;
            m_pSendData->PushData(data, level);
            m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
            rcvdata.sMsgBuf = NULL;
            return;
        }
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

// 函数名: OnMsgReportBpc
    // 编程  : 王明松 2015-4-23 16:01:43
    // 返回  : virtual void 
    // 参数  : S_BPC_RSMSG &rcvdata
    // 描述  : 主动上报监控信息，发给总线
void CDrebMsgThread::OnMsgReportBpc(S_BPC_RSMSG& rcvdata)
{
    //监控信息
    CBF_Xml        pXml;

    if (!pXml.FromBuffer(rcvdata.sMsgBuf->sBuffer))
    {
        m_pDrebApi->GetLogPoint()->LogMp(LOG_ERROR, __FILE__, __LINE__, "API过来的非xml格式");
        m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
        rcvdata.sMsgBuf = NULL;
        return;
    }
    //监控信息
    int i;
    pXml.SetNodeValueByPath("Monitor/公共节点", false, (int)m_pRes->g_nSvrMainId);
    pXml.SetNodeValueByPath("Monitor/私有节点", false, (int)m_pRes->g_nSvrPrivateId);
    pXml.SetNodeValueByPath("Monitor/节点类型", false, 2);//类bpc
    pXml.SetNodeValueByPath("Monitor/报告类型", false, 0);//正常
    pXml.SetNodeValueByPath("Monitor/启动时间", false, m_pRes->g_sStartDateTime, false);

    int nConntotal = 0;
    int bufsize=0;
    int nConnnum = m_pAio->GetConnectPoolInfo(nConntotal);

    pXml.SetNodeAttribute("Monitor/连接池", false, "最大连接数", false, nConntotal);
    pXml.SetNodeAttribute("Monitor/连接池", false, "空闲连接数", false, nConnnum);
    pXml.SetNodeAttribute("Monitor/连接池", false, "客户连接数", false, nConntotal - nConnnum - 1);
    char extdata[300];
    sprintf(extdata, "连接池最大连接数:%d 空闲连接数%d 客户连接数%d", nConntotal, nConnnum, nConntotal - nConnnum - 1);
    m_pDrebApi->GetBufferPoolInfo(nConntotal, nConnnum, bufsize);
    pXml.SetNodeAttribute("Monitor/内存缓冲池", false, "总分配", false, nConntotal);
    pXml.SetNodeAttribute("Monitor/内存缓冲池", false, "未使用", false, nConnnum);
    pXml.SetNodeAttribute("Monitor/内存缓冲池", false, "每块大小", false, bufsize);

    if (m_pRes->g_nMonitorHost == 1)
    {
        S_MONITOR_HOST hostinfo;
        std::vector<S_MONITOR_DISK>  diskinfo;
        m_pDrebApi->GetHostInfo(hostinfo, diskinfo);
        pXml.SetNodeValueByPath("Monitor/主机资源/CPU", false, (int)hostinfo.nCpuRate);
        pXml.SetNodeAttribute("Monitor/主机资源/内存", false, "total", false, (int)hostinfo.nTotalMemory);
        pXml.SetNodeAttribute("Monitor/主机资源/内存", false, "used", false, (int)hostinfo.nUsedMemory);
        for (i = 0; i < diskinfo.size(); i++)
        {
            CXmlNode* disknode = pXml.AddNodeByPath("Monitor/主机资源/磁盘", false, "磁盘", false, "");
            if (disknode != NULL)
            {
                disknode->SetAttribute("drive", false, diskinfo[i].sDiskName, true);
                disknode->SetAttribute("total", false, (int)diskinfo[i].nTotalSpace);
                disknode->SetAttribute("used", false, (int)diskinfo[i].nUsedSpace);
            }
        }
    }
   
    //连接信息
    char sip[30];
    int port;
    int drebid;
    int drebpid;
    char status[10];

    for (i = 0; i < m_pRes->g_vDrebLinkInfo.size(); i++)
    {
        CXmlNode* connnode = pXml.AddNodeByPath("Monitor/连接信息", false, "连接", false, "");
        if (connnode == NULL)
        {
            m_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, "AddNodeByPath 出错");
            continue;
        }
        if (m_pDrebApi->GetDrebInfo(i, sip, port, drebid, drebpid, status))
        {
            connnode->SetAttribute("ip", false,sip);
            connnode->SetAttribute("port", false, port);
            connnode->SetAttribute("index", false, i);
            connnode->SetAttribute("公共节点", false, drebid);
            connnode->SetAttribute("私有节点", false, drebpid);
            connnode->SetAttribute("status", false, status);
        }
    }
    pXml.SetNodeValueByPath("Monitor/扩展信息/cgate", false, extdata, false);
    pXml.SetNodeAttribute("Monitor/扩展信息/cgate", false, "status", false, 0);
    int len = BPUDATASIZE;
    if (!pXml.ToBuffer(rcvdata.sMsgBuf->sBuffer, len))
    {
        m_pDrebApi->GetLogPoint()->LogMp(LOG_ERROR, __FILE__, __LINE__, "ToBuffer出错");
        m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
        rcvdata.sMsgBuf = NULL;
        return;
    }
    rcvdata.sMsgBuf->sDBHead.cCmd = CMD_DPPUSH;
    rcvdata.sMsgBuf->sDBHead.cZip = 0;
    rcvdata.sMsgBuf->sDBHead.cDrebAffirm = 0;
    rcvdata.sMsgBuf->sDBHead.cNextFlag = 0; //是否是取后续包
    rcvdata.sMsgBuf->sDBHead.cRaflag = 0;//请求应答标志

    rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_cNodePrivateId = m_pRes->g_nMonitorDrebPrivateId;
    rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_cSvrPrivateId = m_pRes->g_nMonitorSvrPrivateId;
    rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nNodeId = m_pRes->g_nMonitorDrebId;
    rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo = m_pRes->g_nMonitorTxCode;
    rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nSvrMainId = m_pRes->g_nMonitorSvrId;
    rcvdata.sMsgBuf->sDBHead.nLen = len;
    m_pDrebApi->GetLogPoint()->LogBin(LOG_DEBUG + 1, __FILE__, __LINE__, "监控通知", rcvdata.sMsgBuf->sBuffer, rcvdata.sMsgBuf->sDBHead.nLen);
    //发给总线	
    rcvdata.sMsgBuf->sBpcHead.nIndex = 100;
    m_pDrebApi->SendMsg(rcvdata);
    return;
}
bool CDrebMsgThread::UnzipBuf(BPCCOMMSTRU& data)
{
    //0明文 1des加密密钥长度16 2压缩 3DES加密 4DES加密并压缩
    if (data.sDBHead.nLen == 0)
    {
        data.sDBHead.cZip = 0;
        return true;
    }
    if (data.sDBHead.cZip > 0 && data.sDBHead.cZip <= 4)
    {
        char zipDataBuf[BPUDATASIZE];
        bzero(zipDataBuf, sizeof(zipDataBuf));
        unsigned int dlen = BPUDATASIZE;
        int ddlen;
        switch (data.sDBHead.cZip)
        {
        case 1:
            ddlen = data.sDBHead.nLen;
            if (m_pDes.UncryptString(data.sBuffer, ddlen, DESENCRYPTKEY) <= 0)
            {
                m_pLog->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "解密出错");
                return false;
            }
            data.sDBHead.nLen = ddlen;
            data.sDBHead.cZip = 0;
            break;
        case 2:
            break;
        case 3:
            ddlen = data.sDBHead.nLen;
            if (m_pDes.UncryptString(data.sBuffer, ddlen, DESENCRYPTKEY3) <= 0)
            {
                m_pLog->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "解密出错");
                return false;
            }
            data.sDBHead.nLen = ddlen;
            data.sDBHead.cZip = 0;
            break;
        case 4:
            ddlen = data.sDBHead.nLen;
            m_pLog->LogBin(LOG_DEBUG + 1, __FILE__, __LINE__, "解密前数据", data.sBuffer, data.sDBHead.nLen);
            if (m_pDes.UncryptString(data.sBuffer, ddlen, DESENCRYPTKEY) <= 0)
            {
                m_pLog->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "解密出错");
                return false;
            }
            data.sDBHead.nLen = ddlen;
            data.sDBHead.cZip = 2;
            break;
        default:
            return false;
        }
    }

    return true;
}