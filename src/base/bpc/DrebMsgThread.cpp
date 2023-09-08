// DrebMsgThread.cpp: implementation of the CDrebMsgThread class.
//
//////////////////////////////////////////////////////////////////////

#include "DrebMsgThread.h"
#include "GResource.h"

#define  DESENCRYPTKEY    "rd402wms"
#define  DESENCRYPTKEY3   "wms13711songfree"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CDrebMsgThread::CDrebMsgThread()
{
	m_pLog = NULL;
    m_pPoolData = NULL;//处理数据队列
    m_pFuncTbl = NULL;//功能表
    m_pSockMgr = NULL;//bpu连接管理
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


bool CDrebMsgThread::SetGlobalVar(CPoolDataMsg* pooldata, CFuncTbl* tbl, CSocketMgr* sockmgr)
{
    m_pPoolData = pooldata;
    m_pFuncTbl = tbl;
    m_pSockMgr = sockmgr;
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
    m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "Recv DREB index[%d]connect rsp，subscribe broadcast", rcvdata.sMsgBuf->sBpcHead.nIndex);
#else
    m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "收到DREB index[%d]连接成功的响应 向总线订阅广播", rcvdata.sMsgBuf->sBpcHead.nIndex);
#endif
    std::vector<S_FUNCINFO_TBL> funclist;
    m_pFuncTbl->GetAllFunc(funclist);
    std::vector<int>txlist;
    for (int i = 0; i < funclist.size(); i++)
    {
        txlist.push_back(funclist[i].nFuncNo);
    }
    m_pDrebApi->RegisterDreb(rcvdata.sMsgBuf->sBpcHead.nIndex, &txlist);
	m_pDrebApi->Subscribe(rcvdata.sMsgBuf->sBpcHead.nIndex,m_lBcRegister);
	
}

// 函数名: OnMsgDrebAns
// 编程  : 王明松 2015-4-23 15:55:58
// 返回  : virtual void 
// 参数  : S_BPC_RSMSG &rcvdata
// 描述  : 响应总线过来的应答数据
void CDrebMsgThread::OnMsgDrebAns(S_BPC_RSMSG &rcvdata)
{
    switch (rcvdata.sMsgBuf->sDBHead.cCmd)
    {
    
    case CMD_DPCALL:
    case CMD_DPACALL:
    case CMD_DPBC:
    case CMD_DPABC:
    case CMD_DPPUSH:
    case CMD_DPPOST:
        //bpc外调，直接应答给bpu
        if (m_pSockMgr->AffirmIndex(rcvdata.sMsgBuf->sDBHead.b_Cinfo.b_cIndex))
        {
            //判断连接时间戳是否相同 ,b_Cinfo里的b_cIndex存放bpu连接index,b_nSerial存放bpu连接index时间戳
            if (rcvdata.sMsgBuf->sDBHead.b_Cinfo.b_nSerial == m_pSockMgr->at(rcvdata.sMsgBuf->sDBHead.b_Cinfo.b_cIndex)->m_nConntime)
            {
                rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN + rcvdata.sMsgBuf->sDBHead.nLen;
                rcvdata.sMsgBuf->sBpcHead.nBpuIndex = rcvdata.sMsgBuf->sDBHead.b_Cinfo.b_cIndex;
                rcvdata.sMsgBuf->sBpcHead.nConnectTime = m_pSockMgr->at(rcvdata.sMsgBuf->sDBHead.b_Cinfo.b_cIndex)->m_nConntime;
                //应答回到BPC，肯定是extcall了
                rcvdata.sMsgBuf->sBpcHead.cMsgType = MSG_EXTCALL;
                m_pSockMgr->at(rcvdata.sMsgBuf->sDBHead.b_Cinfo.b_cIndex)->SendMsg(rcvdata);
                return;
            }
            m_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, "BPU外调应答时连接时间戳不符 index[%d]的消息[%s]  DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 源[%d %d] czip[%d] 业务数据长度[%d]", \
                rcvdata.sMsgBuf->sDBHead.b_Cinfo.b_cIndex, CDrebMsgProcBase::GetBpcMsgType(rcvdata.sMsgBuf->sBpcHead.cMsgType).c_str(), \
                CDrebMsgProcBase::GetDrebCmdType(rcvdata.sMsgBuf->sDBHead.cCmd).c_str(), rcvdata.sMsgBuf->sDBHead.cNextFlag, \
                rcvdata.sMsgBuf->sDBHead.cRaflag, rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo, rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId, \
                rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId, rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial, \
                rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial, rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook, \
                rcvdata.sMsgBuf->sDBHead.cZip, rcvdata.sMsgBuf->sDBHead.nLen);
        }
        else
        {
            m_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, "BPU外调应答时连接index非法 index[%d]的消息[%s]  DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 源[%d %d] cZip[%d] 业务数据长度[%d]", \
                rcvdata.sMsgBuf->sDBHead.b_Cinfo.b_cIndex, CDrebMsgProcBase::GetBpcMsgType(rcvdata.sMsgBuf->sBpcHead.cMsgType).c_str(), \
                CDrebMsgProcBase::GetDrebCmdType(rcvdata.sMsgBuf->sDBHead.cCmd).c_str(), rcvdata.sMsgBuf->sDBHead.cNextFlag, \
                rcvdata.sMsgBuf->sDBHead.cRaflag, rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo, rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId, \
                rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId, rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial, \
                rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial, rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook, \
                rcvdata.sMsgBuf->sDBHead.cZip, rcvdata.sMsgBuf->sDBHead.nLen);
        }
        m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
        rcvdata.sMsgBuf = NULL;
        break;
    default:
        m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
        rcvdata.sMsgBuf = NULL;
        break;
    }
    return;
}

void CDrebMsgThread::OnMsgRequest(S_BPC_RSMSG& rcvdata)
{
    switch (rcvdata.sMsgBuf->sDBHead.cCmd)
    {
    case CMD_DPCALL:
    case CMD_DPACALL:
    case CMD_DPBC:
    case CMD_DPABC:
    case CMD_DPPUSH:
    case CMD_DPPOST:
        m_pLog->LogBin(LOG_DEBUG + 2, __FILE__, __LINE__, "收到请求数据", rcvdata.sMsgBuf->sBuffer, rcvdata.sMsgBuf->sDBHead.nLen);
//				m_pRes->PutTime();

// 				//测试代码
// 				rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
// 				sprintf(rcvdata.sMsgBuf->sBuffer,"测试bpc直接返回");
// 				rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
// 				rcvdata.sMsgBuf->sDBHead.cZip=0;
// 				m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
// 
// 				return;
                //放入队列，由消息线程取交易码信息，发给对应的BPU组
        m_pPoolData->PushData(rcvdata);
        break;
    default:
        m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
        rcvdata.sMsgBuf = NULL;
    }
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

void CDrebMsgThread::OnGetBpuNextPack(S_BPC_RSMSG& rcvdata)
{
    m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
    rcvdata.sMsgBuf = NULL;
}
void CDrebMsgThread::OnGetBpcNextPack(S_BPC_RSMSG& rcvdata)
{
    m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
    rcvdata.sMsgBuf = NULL;
}
void CDrebMsgThread::MonitorInfo(S_BPC_RSMSG& rcvdata)
{
    int i;
    CBF_Xml xmlpack;
    xmlpack.SetNodeValueByPath("Monitor/BPC公共节点", false, (int)m_pRes->g_nSvrMainId);
    xmlpack.SetNodeValueByPath("Monitor/BPC私有节点", false, (int)m_pRes->g_nSvrPrivateId);
    xmlpack.SetNodeValueByPath("Monitor/启动时间", false, m_pRes->g_sStartDateTime, false);
    char sip[40];
    char drebstatus[40];
    int  drebport;
    int  drebid;
    int  drebprivateid;

    //连接信息
    for (i = 0; i < m_pRes->g_vDrebLinkInfo.size(); i++)
    {
        CXmlNode* connnode = xmlpack.AddNodeByPath("Monitor/DREB连接信息", false, "连接", false, "");
        if (connnode == NULL)
        {
            m_pDrebApi->GetLogPoint()->LogMp(LOG_ERROR, __FILE__, __LINE__, "AddNodeByPath 出错");
            continue;
        }
        bzero(sip, sizeof(sip));
        bzero(drebstatus, sizeof(drebstatus));
        m_pDrebApi->GetDrebInfo(i, sip, drebport, drebid, drebprivateid, drebstatus);
        connnode->SetAttribute("ip", false, sip);
        connnode->SetAttribute("port", false, drebport);
        connnode->SetAttribute("index", false, i);
        connnode->SetAttribute("公共节点", false, drebid);
        connnode->SetAttribute("私有节点", false, drebprivateid);
        connnode->SetAttribute("status", false, drebstatus);

    }
    CGResource *res = (CGResource*)m_pRes;
    //BPU信息
    int k;
    int connNum = 0;
    xmlpack.AddNodeByPath("Monitor", false, "BPU列表", false, "");
    for (i = 0; i < res->g_vBpuGroupInfo.size(); i++)
    {
        connNum = 0;
        for (k = res->g_vBpuGroupInfo[i].g_nBeginSocketIndex; k < res->g_vBpuGroupInfo[i].g_nEndSocketIndex; k++)
        {
            if (m_pSockMgr->at(k)->m_sock != INVALID_SOCKET)
            {
                connNum++;
            }
        }
        CXmlNode* bpunode = xmlpack.AddNodeByPath("Monitor/BPU列表", false, res->g_vBpuGroupInfo[i].g_sBpuGroupName.c_str(), true, "");
        bpunode->SetAttribute("配置BPU数", false, res->g_vBpuGroupInfo[i].g_nBpuNum);
        bpunode->SetAttribute("实际BPU数", false, connNum);
        bpunode->SetAttribute("分配模式", false, res->g_vBpuGroupInfo[i].g_nDispatchMode);
        bpunode->SetAttribute("自动启动", false, res->g_vBpuGroupInfo[i].g_bIsAutoLoadBu);
        bpunode->SetAttribute("开始连接索引", false, res->g_vBpuGroupInfo[i].g_nBeginSocketIndex);
        bpunode->SetAttribute("结束连接索引", false, res->g_vBpuGroupInfo[i].g_nEndSocketIndex);
    }

    int len = DREBDATASIZE;
    if (!xmlpack.ToBuffer(rcvdata.sMsgBuf->sBuffer, len))
    {
        sprintf(rcvdata.sMsgBuf->sBuffer, "缓冲不足，请下载bpcinfo.txt查看");
        rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
        m_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, "缓冲不足");
        xmlpack.ToFile("bpcinfo.txt");
    }
    else
    {
        rcvdata.sMsgBuf->sDBHead.nLen = len;
    }

    rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
    rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
    rcvdata.sMsgBuf->sDBHead.cZip = 0;
    m_pDrebApi->SendMsg(rcvdata);
    return;
}
void CDrebMsgThread::MonitorBpu(S_BPC_RSMSG& rcvdata)
{
    int i, k;
    int connNum = 0;
    CBF_Xml xmlpack;
    xmlpack.SetNodeValueByPath("Monitor/BPC公共节点", false, (int)m_pRes->g_nSvrMainId);
    xmlpack.SetNodeValueByPath("Monitor/BPC私有节点", false, (int)m_pRes->g_nSvrPrivateId);
    xmlpack.SetNodeValueByPath("Monitor/启动时间", false, m_pRes->g_sStartDateTime, false);
    //BPU信息
    CGResource* res = (CGResource*)m_pRes;
    xmlpack.AddNodeByPath("Monitor", false, "BPU列表", false, "");
    for (i = 0; i < res->g_vBpuGroupInfo.size(); i++)
    {
        connNum = 0;
        for (k = res->g_vBpuGroupInfo[i].g_nBeginSocketIndex; k < res->g_vBpuGroupInfo[i].g_nEndSocketIndex; k++)
        {
            if (m_pSockMgr->at(k)->m_sock != INVALID_SOCKET)
            {
                connNum++;
            }
        }
        CXmlNode* bpunode = xmlpack.AddNodeByPath("Monitor/BPU列表", false, res->g_vBpuGroupInfo[i].g_sBpuGroupName.c_str(), true, "");
        if (bpunode != NULL)
        {
            bpunode->SetAttribute("组名", false, res->g_vBpuGroupInfo[i].g_sBpuGroupName);
            bpunode->SetAttribute("配置BPU数", false, res->g_vBpuGroupInfo[i].g_nBpuNum);
            bpunode->SetAttribute("实际BPU数", false, connNum);
            bpunode->SetAttribute("心跳应答", false, res->g_vBpuGroupInfo[i].g_bIsAnsPing);
            bpunode->SetAttribute("分配模式", false, res->g_vBpuGroupInfo[i].g_nDispatchMode);
            bpunode->SetAttribute("自动启动", false, res->g_vBpuGroupInfo[i].g_bIsAutoLoadBu);
            bpunode->SetAttribute("程序名", false, res->g_vBpuGroupInfo[i].g_sBuPrgName);
            bpunode->SetAttribute("程序目录", false, res->g_vBpuGroupInfo[i].g_sBuPrgPath);
            bpunode->SetAttribute("运行目录", false, res->g_vBpuGroupInfo[i].g_sBuRunPath);
            bpunode->SetAttribute("运行参数", false, res->g_vBpuGroupInfo[i].g_sBuPrgPara);
            bpunode->SetAttribute("开始连接索引", false, res->g_vBpuGroupInfo[i].g_nBeginSocketIndex);
            bpunode->SetAttribute("结束连接索引", false, res->g_vBpuGroupInfo[i].g_nEndSocketIndex);

        }
    }

    int len = DREBDATASIZE;
    if (!xmlpack.ToBuffer(rcvdata.sMsgBuf->sBuffer, len))
    {
        sprintf(rcvdata.sMsgBuf->sBuffer, "缓冲不足，请下载bpcbpu.txt查看");
        rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
        m_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, "缓冲不足");
        xmlpack.ToFile("bpcbpu.txt");
    }
    else
    {
        rcvdata.sMsgBuf->sDBHead.nLen = len;
    }

    rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
    rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
    rcvdata.sMsgBuf->sDBHead.cZip = 0;
    m_pDrebApi->SendMsg(rcvdata);
}
void CDrebMsgThread::OnMsgReportBpc(S_BPC_RSMSG& rcvdata)
{
    MonitorReport(rcvdata);
}
void CDrebMsgThread::MonitorReport(S_BPC_RSMSG& rcvdata)
{
    if (m_pRes->g_nUseMonitor != 1)
    {
        return;
    }
    //监控信息
    int i, k;
    CBF_Xml        pXml;
    if (!pXml.FromBuffer(rcvdata.sMsgBuf->sBuffer))
    {
        pXml.SetNodeValueByPath("Monitor/公共节点", false, (int)m_pRes->g_nSvrMainId);
        pXml.SetNodeValueByPath("Monitor/私有节点", false, (int)m_pRes->g_nSvrPrivateId);
        pXml.SetNodeValueByPath("Monitor/节点类型", false, 1);//bpc
        pXml.SetNodeValueByPath("Monitor/报告类型", false, 0);//正常
        pXml.SetNodeValueByPath("Monitor/启动时间", false, m_pRes->g_sStartDateTime, false);
        int total = 0;
        int used = 0;
        int bufsize = 0;
        m_pDrebApi->GetBufferPoolInfo(total, used, bufsize);
        pXml.SetNodeAttribute("Monitor/内存缓冲池", false, "总分配", false, total);
        pXml.SetNodeAttribute("Monitor/内存缓冲池", false, "未使用", false, used);
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
        char sip[40];
        char drebstatus[40];
        int  drebport;
        int  drebid;
        int  drebprivateid;

        //连接信息
        for (i = 0; i < m_pRes->g_vDrebLinkInfo.size(); i++)
        {
            CXmlNode* connnode = pXml.AddNodeByPath("Monitor/DREB连接信息", false, "连接", false, "");
            if (connnode == NULL)
            {
                m_pDrebApi->GetLogPoint()->LogMp(LOG_ERROR, __FILE__, __LINE__, "AddNodeByPath 出错");
                continue;
            }
            bzero(sip, sizeof(sip));
            bzero(drebstatus, sizeof(drebstatus));
            m_pDrebApi->GetDrebInfo(i, sip, drebport, drebid, drebprivateid, drebstatus);
            connnode->SetAttribute("ip", false, sip);
            connnode->SetAttribute("port", false, drebport);
            connnode->SetAttribute("index", false, i);
            connnode->SetAttribute("公共节点", false, drebid);
            connnode->SetAttribute("私有节点", false, drebprivateid);
            connnode->SetAttribute("status", false, drebstatus);

        }
    }
    CGResource* res = (CGResource*)m_pRes;
    int connNum = 0;
    //BPC信息
    pXml.AddNodeByPath("Monitor/BPU信息", false, "BPULIST", false, "");
    for (i = 0; i < res->g_vBpuGroupInfo.size(); i++)
    {
        connNum = 0;
        for (k = res->g_vBpuGroupInfo[i].g_nBeginSocketIndex; k < res->g_vBpuGroupInfo[i].g_nEndSocketIndex; k++)
        {
            if (m_pSockMgr->at(k)->m_sock != INVALID_SOCKET)
            {
                connNum++;
            }
        }
        CXmlNode* bpunode = pXml.AddNodeByPath("Monitor/BPU信息/BPULIST", false, res->g_vBpuGroupInfo[i].g_sBpuGroupName.c_str(), true, "");
        if (bpunode != NULL)
        {
            bpunode->SetAttribute("名称", false, res->g_vBpuGroupInfo[i].g_sBpuGroupName);
            bpunode->SetAttribute("数目", false, res->g_vBpuGroupInfo[i].g_nBpuNum);
            bpunode->SetAttribute("实际数", false, connNum);
            bpunode->SetAttribute("心跳应答", false, res->g_vBpuGroupInfo[i].g_bIsAnsPing);
            bpunode->SetAttribute("分配模式", false, res->g_vBpuGroupInfo[i].g_nDispatchMode);
            bpunode->SetAttribute("自动启动", false, res->g_vBpuGroupInfo[i].g_bIsAutoLoadBu);
            bpunode->SetAttribute("程序名", false, res->g_vBpuGroupInfo[i].g_sBuPrgName);
            bpunode->SetAttribute("程序目录", false, res->g_vBpuGroupInfo[i].g_sBuPrgPath);
            bpunode->SetAttribute("运行目录", false, res->g_vBpuGroupInfo[i].g_sBuRunPath);
            bpunode->SetAttribute("参数", false, res->g_vBpuGroupInfo[i].g_sBuPrgPara);
        }
    }

    S_BPC_RSMSG  mdata;//接收数据结构
//	bzero(&mdata,sizeof(S_BPC_RSMSG));
    mdata.sMsgBuf = (PBPCCOMMSTRU)m_pDrebApi->PoolMalloc();
    if (mdata.sMsgBuf == NULL)
    {
        m_pLog->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "分配消息空间出错");
        return;
    }
    int len = DREBDATASIZE;
    if (!pXml.ToBuffer(mdata.sMsgBuf->sBuffer, len))
    {
        m_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, "ToBuffer出错");
        m_pDrebApi->PoolFree(mdata.sMsgBuf);
        return;
    }
    mdata.sMsgBuf->sDBHead.nLen = len;

    m_pLog->LogBin(LOG_DEBUG + 1, __FILE__, __LINE__, "监控通知", mdata.sMsgBuf->sBuffer, mdata.sMsgBuf->sDBHead.nLen);


    mdata.sMsgBuf->sDBHead.cRaflag = 0;
    mdata.sMsgBuf->sDBHead.cDrebAffirm = 0;
    mdata.sMsgBuf->sDBHead.cZip = 0;
    mdata.sMsgBuf->sDBHead.cNextFlag = 0;

    mdata.sMsgBuf->sDBHead.cCmd = CMD_DPPUSH;
    mdata.sMsgBuf->sDBHead.d_Dinfo.d_nNodeId = m_pRes->g_nMonitorDrebId;
    mdata.sMsgBuf->sDBHead.d_Dinfo.d_cNodePrivateId = m_pRes->g_nMonitorDrebPrivateId;
    mdata.sMsgBuf->sDBHead.d_Dinfo.d_nSvrMainId = m_pRes->g_nMonitorSvrId;
    mdata.sMsgBuf->sDBHead.d_Dinfo.d_cSvrPrivateId = m_pRes->g_nMonitorSvrPrivateId;
    mdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo = m_pRes->g_nMonitorTxCode;
    mdata.sMsgBuf->sBpcHead.cMsgType = MSG_BPCMONITOR;
    mdata.nRtime = time(NULL);
    S_FUNCINFO_TBL fn;
    int flag = 0;
    fn = m_pFuncTbl->FindFunc(mdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo, flag);
    if (flag == 1)
    {
        fn.nCallNum++;
        mdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN + mdata.sMsgBuf->sDBHead.nLen;
        strcpy(mdata.sBpuGroupName, fn.sBpuGroupName);//本 BPC下面的监控服务
        //20151102 直接放到BPU对应的线程队列
        if (res->PushData(fn.nBpuGroupIndex, mdata, fn.cPrio) != 0)
        {
            m_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, "入BPU组队列出错 消息[%s]  DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 源[%d %d] cZip[%d] 业务数据长度[%d]", \
                CDrebMsgProcBase::GetBpcMsgType(mdata.sMsgBuf->sBpcHead.cMsgType).c_str(), \
                CDrebMsgProcBase::GetDrebCmdType(mdata.sMsgBuf->sDBHead.cCmd).c_str(), mdata.sMsgBuf->sDBHead.cNextFlag, \
                mdata.sMsgBuf->sDBHead.cRaflag, mdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo, mdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId, \
                mdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId, mdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial, \
                mdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial, mdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook, \
                mdata.sMsgBuf->sDBHead.cZip, mdata.sMsgBuf->sDBHead.nLen);
            m_pDrebApi->PoolFree(mdata.sMsgBuf);
            mdata.sMsgBuf = NULL;
        }
    }
    else
    {
        bzero(mdata.sBpuGroupName, sizeof(mdata.sBpuGroupName));
        m_pPoolData->PushData(mdata);
    }

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
            memcpy(zipDataBuf, data.sBuffer, data.sDBHead.nLen);
            if (!CBF_Tools::Uncompress((unsigned char*)(data.sBuffer), dlen, (unsigned char*)zipDataBuf, data.sDBHead.nLen))
            {
                m_pLog->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "解压缩出错");
                return false;
            }
            data.sDBHead.nLen = dlen;
            data.sDBHead.cZip = 0;
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
            m_pLog->LogBin(LOG_DEBUG + 1, __FILE__, __LINE__, "解密后数据", data.sBuffer, data.sDBHead.nLen);
            memcpy(zipDataBuf, data.sBuffer, data.sDBHead.nLen);
            if (!CBF_Tools::Uncompress((unsigned char*)(data.sBuffer), dlen, (unsigned char*)zipDataBuf, data.sDBHead.nLen))
            {
                m_pLog->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "解压缩出错");
                return false;
            }
            data.sDBHead.nLen = dlen;
            m_pLog->LogBin(LOG_DEBUG + 1, __FILE__, __LINE__, "解压后数据", data.sBuffer, data.sDBHead.nLen);
            data.sDBHead.cZip = 0;
            break;
        default:
            return false;
        }
    }

    return true;
}
void CDrebMsgThread::MonitorSetBpu(S_BPC_RSMSG& rcvdata)
{
    //设置BPU数目，启动模式，是否保存至配置文件
// 	<Monitor>
// 		<SetBpu mode="0" remark="0只更改内存数据1同时更新到配置文件">
//         <bpu1>
// 				<发送明文 remark="由BPU解压缩解密0,明文发给BPU1">0</发送明文>
// 				<心跳应答 remark="当为c/c++多bpu时，不用应答心跳消息">0</心跳应答>
// 				<分配模式 remark="0表示空闲分配，1表示BPU数只能为1，一直分配">0</分配模式>
// 				<可启动的BPU数 remark="即可以处理业务的单元数目，当请求分配模式为1时只能为1">1</可启动的BPU数>
// 				<AUTOLOADBPU use="0" remark="自动将BPU程序启动1，否则为0">
// 					<BPU程序名 remark="BU程序的名称，不带路径">bfBPUd.exe</BPU程序名>
// 					<BPU程序路径 remark="绝对或相对路径">D:\BUBASE\bin</BPU程序路径>
// 					<BPU运行目录 remark="绝对或相对目录">D:\BUBASE\bin</BPU运行目录>
// 					<BPU启动参数 remark="启动BPU所需的参数">D:\BUBASE\bin\bfBPU.xml</BPU启动参数>
// 				</AUTOLOADBPU>
//         </bpu1>
// 		</SetBpu>
// 	</Monitor>
    if (rcvdata.sMsgBuf->sDBHead.cZip != 0)
    {
        if (!UnzipBuf(*rcvdata.sMsgBuf))
        {
            sprintf(rcvdata.sMsgBuf->sBuffer, "数据解压缩出错");
            rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
            rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
            rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
            rcvdata.sMsgBuf->sDBHead.cZip = 0;
            m_pDrebApi->SendMsg(rcvdata);
            return;
        }
    }
    CBF_Xml  xmlpack;
    if (!xmlpack.FromBuffer(rcvdata.sMsgBuf->sBuffer))
    {
        sprintf(rcvdata.sMsgBuf->sBuffer, "请求数据非xml");
        rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
        rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
        rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
        rcvdata.sMsgBuf->sDBHead.cZip = 0;
        m_pDrebApi->SendMsg(rcvdata);
        return;
    }
    CXmlNode* setnode = xmlpack.GetNodeByPath("Monitor/SetBpu", false);
    if (setnode == NULL)
    {
        sprintf(rcvdata.sMsgBuf->sBuffer, "xml无[Monitor/SetBpu]节点");
        rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
        rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
        rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
        rcvdata.sMsgBuf->sDBHead.cZip = 0;
        m_pDrebApi->SendMsg(rcvdata);
        return;
    }
    int savemode = 0;
    if (xmlpack.GetNodeAttribute("Monitor/SetBpu", false, "mode", false, savemode) == NULL)
    {
        savemode = 0;//无属性默认为0
    }
    CGResource* res = (CGResource*)m_pRes;
    std::string bpugroupname;
    int i;
    int bpuindex = -1;
    char tmpchar[200];
    char tmpcharconf[200];
    bool isfind = false;
    std::string namevalue;
    if (savemode == 1)
    {
        CBF_Date_Time dt;
        sprintf(tmpchar, "%s.%s%s", res->m_sBpcParaFileName.c_str(), dt.ToStringD8().c_str(), dt.ToStringT6().c_str());
        res->m_xmlconf.ToFile(tmpchar);
    }

    
    CXmlNode* bpugnode = (CXmlNode*)setnode->GetFirstChild();
    while (bpugnode != NULL)
    {
        bpugroupname = bpugnode->GetNodeName();
        isfind = false;
        for (i = 0; i < res->g_vBpuGroupInfo.size(); i++)
        {
            if (bpugroupname.compare(res->g_vBpuGroupInfo[i].g_sBpuGroupName) == 0)
            {
                isfind = true;
                bpuindex = i;
                break;
            }
        }
        if (isfind)
        {
            //找到了
            sprintf(tmpchar, "Monitor/SetBpu/%s/发送明文", bpugroupname.c_str());
            if (xmlpack.GetNodeValueByPath(tmpchar, false, namevalue) != NULL)
            {
                if (savemode == 1) //更新配置文件
                {
                    sprintf(tmpcharconf, "package/head/BPU配置/BPU组/%s/发送明文", bpugroupname.c_str());
                    res->m_xmlconf.SetNodeValueByPath(tmpcharconf, false, namevalue.c_str());
                }
                //更新内存
                if (atoi(namevalue.c_str()) == 1)
                {
                    res->g_vBpuGroupInfo[bpuindex].g_bIsUnencrypt = true;
                }
                else
                {
                    res->g_vBpuGroupInfo[bpuindex].g_bIsUnencrypt = false;
                }
            }
            sprintf(tmpchar, "Monitor/SetBpu/%s/心跳应答", bpugroupname.c_str());
            if (xmlpack.GetNodeValueByPath(tmpchar, false, namevalue) != NULL)
            {
                if (savemode == 1) //更新配置文件
                {
                    sprintf(tmpcharconf, "package/head/BPU配置/BPU组/%s/心跳应答", bpugroupname.c_str());
                    res->m_xmlconf.SetNodeValueByPath(tmpcharconf, false, namevalue.c_str());
                }
                //更新内存
                if (atoi(namevalue.c_str()) == 1)
                {
                    res->g_vBpuGroupInfo[bpuindex].g_bIsAnsPing = true;
                }
                else
                {
                    res->g_vBpuGroupInfo[bpuindex].g_bIsAnsPing = false;
                }
            }
            sprintf(tmpchar, "Monitor/SetBpu/%s/分配模式", bpugroupname.c_str());
            if (xmlpack.GetNodeValueByPath(tmpchar, false, namevalue) != NULL)
            {
                if (savemode == 1) //更新配置文件
                {
                    sprintf(tmpcharconf, "package/head/BPU配置/BPU组/%s/分配模式", bpugroupname.c_str());
                    res->m_xmlconf.SetNodeValueByPath(tmpcharconf, false, namevalue.c_str());
                }
                //更新内存
                res->g_vBpuGroupInfo[bpuindex].g_nDispatchMode = atoi(namevalue.c_str());
            }
            sprintf(tmpchar, "Monitor/SetBpu/%s/可启动的BPU数", bpugroupname.c_str());
            if (xmlpack.GetNodeValueByPath(tmpchar, false, namevalue) != NULL)
            {
                if (res->g_vBpuGroupInfo[bpuindex].g_nDispatchMode == 1)
                {
                    namevalue = "1";
                }
                //更新内存
                res->g_vBpuGroupInfo[bpuindex].g_nBpuNum = atoi(namevalue.c_str());

                if (savemode == 1) //更新配置文件
                {
                    sprintf(tmpcharconf, "package/head/BPU配置/BPU组/%s/可启动的BPU数", bpugroupname.c_str());
                    res->m_xmlconf.SetNodeValueByPath(tmpcharconf, false, namevalue.c_str());
                }
            }

            sprintf(tmpchar, "Monitor/SetBpu/%s/AUTOLOADBPU", bpugroupname.c_str());
            if (xmlpack.GetNodeAttribute(tmpchar, false, "use", false, namevalue) != NULL)
            {
                if (atoi(namevalue.c_str()) == 1)
                {
                    res->g_vBpuGroupInfo[bpuindex].g_bIsAutoLoadBu = true;
                }
                else
                {
                    res->g_vBpuGroupInfo[bpuindex].g_bIsAutoLoadBu = false;
                }

                if (savemode == 1) //更新配置文件
                {
                    sprintf(tmpcharconf, "package/head/BPU配置/BPU组/%s/AUTOLOADBPU", bpugroupname.c_str());
                    res->m_xmlconf.SetNodeAttribute(tmpcharconf, false, "use", false, namevalue.c_str());
                }
                if (res->g_vBpuGroupInfo[bpuindex].g_bIsAutoLoadBu)//自动启动BPU
                {
                    sprintf(tmpchar, "Monitor/SetBpu/%s/AUTOLOADBPU/BPU程序名", bpugroupname.c_str());
                    if (xmlpack.GetNodeValueByPath(tmpchar, false, namevalue) != NULL)
                    {
                        res->g_vBpuGroupInfo[bpuindex].g_sBuPrgName = namevalue;
                        if (savemode == 1) //更新配置文件
                        {
                            sprintf(tmpcharconf, "package/head/BPU配置/BPU组/%s/AUTOLOADBPU/BPU程序名", bpugroupname.c_str());
                            res->m_xmlconf.SetNodeValueByPath(tmpcharconf, false, namevalue.c_str());
                        }
                    }
                    sprintf(tmpchar, "Monitor/SetBpu/%s/AUTOLOADBPU/BPU程序路径", bpugroupname.c_str());
                    if (xmlpack.GetNodeValueByPath(tmpchar, false, namevalue) != NULL)
                    {
                        res->g_vBpuGroupInfo[bpuindex].g_sBuPrgPath = namevalue;
                        if (savemode == 1) //更新配置文件
                        {
                            sprintf(tmpcharconf, "package/head/BPU配置/BPU组/%s/AUTOLOADBPU/BPU程序路径", bpugroupname.c_str());
                            res->m_xmlconf.SetNodeValueByPath(tmpcharconf, false, namevalue.c_str());
                        }
                    }
                    sprintf(tmpchar, "Monitor/SetBpu/%s/AUTOLOADBPU/BPU运行目录", bpugroupname.c_str());
                    if (xmlpack.GetNodeValueByPath(tmpchar, false, namevalue) != NULL)
                    {
                        res->g_vBpuGroupInfo[bpuindex].g_sBuRunPath = namevalue;
                        if (savemode == 1) //更新配置文件
                        {
                            sprintf(tmpcharconf, "package/head/BPU配置/BPU组/%s/AUTOLOADBPU/BPU运行目录", bpugroupname.c_str());
                            res->m_xmlconf.SetNodeValueByPath(tmpcharconf, false, namevalue.c_str());
                        }
                    }
                    sprintf(tmpchar, "Monitor/SetBpu/%s/AUTOLOADBPU/BPU启动参数", bpugroupname.c_str());
                    if (xmlpack.GetNodeValueByPath(tmpchar, false, namevalue) != NULL)
                    {
                        res->g_vBpuGroupInfo[bpuindex].g_sBuPrgPara = namevalue;
                        if (savemode == 1) //更新配置文件
                        {
                            sprintf(tmpcharconf, "package/head/BPU配置/BPU组/%s/AUTOLOADBPU/BPU启动参数", bpugroupname.c_str());
                            res->m_xmlconf.SetNodeValueByPath(tmpcharconf, false, namevalue.c_str());
                        }
                    }
                }
            }
            if (savemode == 1)
            {
                if (!res->m_xmlconf.ToFile(res->m_sBpcParaFileName.c_str()))
                {
                    m_pLog->LogMp(LOG_WARNNING, __FILE__, __LINE__, "更新配置文件[%s]失败", res->m_sBpcParaFileName.c_str());
                }
            }
        }
        else
        {
            m_pLog->LogMp(LOG_WARNNING, __FILE__, __LINE__, "bpu组[%s]在BPC配置中不存在", bpugroupname.c_str());
        }
        bpugnode = (CXmlNode*)bpugnode->getNextSibling();
    }
    sprintf(rcvdata.sMsgBuf->sBuffer, "设置参数成功");
    rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
    rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
    rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
    rcvdata.sMsgBuf->sDBHead.cZip = 0;
    m_pDrebApi->SendMsg(rcvdata);
    return;
}
void CDrebMsgThread::MonitorSetMode(S_BPC_RSMSG& rcvdata)
{
    if (rcvdata.sMsgBuf->sDBHead.cZip != 0)
    {
        if (!UnzipBuf(*rcvdata.sMsgBuf))
        {
            sprintf(rcvdata.sMsgBuf->sBuffer, "数据解压缩出错");
            rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
            rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
            rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
            rcvdata.sMsgBuf->sDBHead.cZip = 0;
            m_pDrebApi->SendMsg(rcvdata);
            return;
        }
    }
    CBF_Xml  xmlpack;
    if (!xmlpack.FromBuffer(rcvdata.sMsgBuf->sBuffer))
    {
        sprintf(rcvdata.sMsgBuf->sBuffer, "请求数据非xml");
        rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
        rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
        rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
        rcvdata.sMsgBuf->sDBHead.cZip = 0;
        m_pDrebApi->SendMsg(rcvdata);
        return;
    }
    CXmlNode* setnode = xmlpack.GetNodeByPath("Monitor/SetBpu", false);
    if (setnode == NULL)
    {
        sprintf(rcvdata.sMsgBuf->sBuffer, "xml无[Monitor/SetBpu]节点");
        rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
        rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
        rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
        rcvdata.sMsgBuf->sDBHead.cZip = 0;
        m_pDrebApi->SendMsg(rcvdata);
        return;
    }
    int savemode = 0;
    if (xmlpack.GetNodeAttribute("Monitor/SetMonitor", false, "mode", false, savemode) == NULL)
    {
        savemode = 0;//无属性默认为0
    }
    CGResource* res = (CGResource*)m_pRes;
    std::string bpugroupname;
    int i;

    char tmpchar[200];
    char tmpcharconf[200];
    bool isfind = false;
    std::string namevalue;
    if (savemode == 1)
    {
        CBF_Date_Time dt;
        sprintf(tmpchar, "%s.%s%s", res->m_sBpcParaFileName.c_str(), dt.ToStringD8().c_str(), dt.ToStringT6().c_str());
        res->m_xmlconf.ToFile(tmpchar);
    }
    // 	int   g_nUseMonitor;       //是否监控
    // 	int   g_nMonitorHost;      //是否报告主机资源
    // 	int   g_nMonitorDrebId;            //公共节点号
    // 	int   g_nMonitorDrebPrivateId;     //私有节点号
    // 	int   g_nMonitorSvrId;            //公共服务节点号
    // 	int   g_nMonitorSvrPrivateId;     //私有服务节点号
    // 	int   g_nMonitorTxCode;           //监控交易码
    if (xmlpack.GetNodeAttribute("Monitor/SetMonitor/监控配置", false, "use", false, namevalue) != NULL)
    {
        if (savemode == 1) //更新配置文件
        {
            sprintf(tmpcharconf, "package/head/监控配置");
            res->m_xmlconf.SetNodeAttribute(tmpcharconf, false, "use", false, namevalue.c_str(), false);
        }
        //更新内存
        m_pRes->g_nUseMonitor = atoi(namevalue.c_str());
    }
    if (xmlpack.GetNodeAttribute("Monitor/SetMonitor/监控配置", false, "host", false, namevalue) != NULL)
    {
        if (savemode == 1) //更新配置文件
        {
            sprintf(tmpcharconf, "package/head/监控配置");
            res->m_xmlconf.SetNodeAttribute(tmpcharconf, false, "host", false, namevalue.c_str(), false);
        }
        //更新内存
        m_pRes->g_nMonitorHost = atoi(namevalue.c_str());
    }
    if (xmlpack.GetNodeValueByPath("Monitor/SetMonitor/监控公共节点号", false, namevalue) != NULL)
    {
        if (savemode == 1) //更新配置文件
        {
            sprintf(tmpcharconf, "package/head/监控配置/监控公共节点号");
            res->m_xmlconf.SetNodeValueByPath(tmpcharconf, false, namevalue.c_str(), false);
        }
        //更新内存
        m_pRes->g_nMonitorDrebId = atoi(namevalue.c_str());
    }
    if (xmlpack.GetNodeValueByPath("Monitor/SetMonitor/监控私有节点号", false, namevalue) != NULL)
    {
        if (savemode == 1) //更新配置文件
        {
            sprintf(tmpcharconf, "package/head/监控配置/监控私有节点号");
            res->m_xmlconf.SetNodeValueByPath(tmpcharconf, false, namevalue.c_str(), false);
        }
        //更新内存
        m_pRes->g_nMonitorDrebPrivateId = atoi(namevalue.c_str());
    }
    if (xmlpack.GetNodeValueByPath("Monitor/SetMonitor/监控公共服务号", false, namevalue) != NULL)
    {
        if (savemode == 1) //更新配置文件
        {
            sprintf(tmpcharconf, "package/head/监控配置/监控公共服务号");
            res->m_xmlconf.SetNodeValueByPath(tmpcharconf, false, namevalue.c_str(), false);
        }
        //更新内存
        m_pRes->g_nMonitorSvrId = atoi(namevalue.c_str());
    }
    if (xmlpack.GetNodeValueByPath("Monitor/SetMonitor/监控私有服务号", false, namevalue) != NULL)
    {
        if (savemode == 1) //更新配置文件
        {
            sprintf(tmpcharconf, "package/head/监控配置/监控私有服务号");
            res->m_xmlconf.SetNodeValueByPath(tmpcharconf, false, namevalue.c_str(), false);
        }
        //更新内存
        m_pRes->g_nMonitorSvrPrivateId = atoi(namevalue.c_str());
    }
    if (xmlpack.GetNodeValueByPath("Monitor/SetMonitor/监控报告交易码", false, namevalue) != NULL)
    {
        if (savemode == 1) //更新配置文件
        {
            sprintf(tmpcharconf, "package/head/监控配置/监控报告交易码");
            res->m_xmlconf.SetNodeValueByPath(tmpcharconf, false, namevalue.c_str(), false);
        }
        //更新内存
        m_pRes->g_nMonitorTxCode = atoi(namevalue.c_str());
    }
    if (savemode == 1)
    {
        if (!res->m_xmlconf.ToFile(res->m_sBpcParaFileName.c_str()))
        {
            m_pLog->LogMp(LOG_WARNNING, __FILE__, __LINE__, "更新配置文件[%s]失败", res->m_sBpcParaFileName.c_str());
            sprintf(rcvdata.sMsgBuf->sBuffer, "更新配置文件[%s]失败", res->m_sBpcParaFileName.c_str());
            rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
            rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
            rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
            rcvdata.sMsgBuf->sDBHead.cZip = 0;
            m_pDrebApi->SendMsg(rcvdata);
            return;
        }
    }
    sprintf(rcvdata.sMsgBuf->sBuffer, "设置参数成功");
    rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
    rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
    rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
    rcvdata.sMsgBuf->sDBHead.cZip = 0;
    m_pDrebApi->SendMsg(rcvdata);
    return;
}
void CDrebMsgThread::MonitorStopBpu(S_BPC_RSMSG& rcvdata)
{
    if (rcvdata.sMsgBuf->sDBHead.cZip != 0)
    {
        if (!UnzipBuf(*rcvdata.sMsgBuf))
        {
            sprintf(rcvdata.sMsgBuf->sBuffer, "数据解压缩出错");
            rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
            rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
            rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
            rcvdata.sMsgBuf->sDBHead.cZip = 0;
            m_pDrebApi->SendMsg(rcvdata);
            return;
        }
    }
    CBF_Xml  xmlpack;
    if (!xmlpack.FromBuffer(rcvdata.sMsgBuf->sBuffer))
    {
        sprintf(rcvdata.sMsgBuf->sBuffer, "请求数据非xml");
        rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
        rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
        rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
        rcvdata.sMsgBuf->sDBHead.cZip = 0;
        m_pDrebApi->SendMsg(rcvdata);
        return;
    }
    CXmlNode* stopnode = xmlpack.GetNodeByPath("Monitor/StopBpu", false);
    if (stopnode == NULL)
    {
        sprintf(rcvdata.sMsgBuf->sBuffer, "xml无[Monitor/StopBpu]节点");
        rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
        rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
        rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
        rcvdata.sMsgBuf->sDBHead.cZip = 0;
        m_pDrebApi->SendMsg(rcvdata);
        return;
    }
    CGResource* res = (CGResource*)m_pRes;
    int i;
    int bpuindex = -1;
    std::string bpugroupname;
    int bpugnum = 0;
    S_PROCESS_NODE node;
    bool isfind = false;
    char errmsg[200];
    CXmlNode* bpunode = (CXmlNode*)stopnode->GetFirstChild();
    while (bpunode != NULL)
    {
        bpugroupname = bpunode->GetNodeName();
        isfind = false;
        for (i = 0; i < res->g_vBpuGroupInfo.size(); i++)
        {
            if (bpugroupname.compare(res->g_vBpuGroupInfo[i].g_sBpuGroupName) == 0)
            {
                isfind = true;
                bpuindex = i;
                break;
            }
        }
        if (isfind)
        {
            node.prgname = res->g_vBpuGroupInfo[bpuindex].g_sBuPrgName;
            node.prgpara = res->g_vBpuGroupInfo[bpuindex].g_sBuPrgPara;
            node.prgpath = res->g_vBpuGroupInfo[bpuindex].g_sBuPrgPath;
            node.runpath = res->g_vBpuGroupInfo[bpuindex].g_sBuRunPath;
            node.nodeid = res->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName;

            if (res->g_vBpuGroupInfo[bpuindex].g_nDispatchMode == 1) //java版本
            {
                m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "查找原BPU[%s]进程是否存在[%d]", \
                    res->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(), res->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid);
                if (res->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid > 0) //查看进程是否存在
                {
                    node.pid = res->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid;
                    if (m_pMgrBpu.FindProcess(&node, errmsg) == 0) //有此进程
                    {
                        m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "原BPU[%s]进程[%d]已存在，停止", \
                            res->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(), res->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid);
                        if (m_pMgrBpu.StopProcess(&node, errmsg) != 0)
                        {
                            m_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, "BPU[%s]进程[%d]停止失败", \
                                res->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(), res->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid);
                        }
                    }
                }
            }
            else
            {
                for (int j = res->g_vBpuGroupInfo[bpuindex].g_nBeginSocketIndex; j < res->g_vBpuGroupInfo[bpuindex].g_nEndSocketIndex; j++)
                {
                    if (m_pSockMgr->at(j)->m_sock != INVALID_SOCKET)
                    {
                        node.pid = m_pSockMgr->at(j)->m_nPid;
                        m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "停止BPU[%s]进程[%d]", \
                            res->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(), node.pid);
                        if (m_pMgrBpu.StopProcess(&node, errmsg) != 0)
                        {
                            m_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, "BPU[%s]进程[%d]停止失败", \
                                res->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(), node.pid);
                        }
                    }
                }
            }

        }
        bpunode = (CXmlNode*)bpunode->getNextSibling();
    }
    sprintf(rcvdata.sMsgBuf->sBuffer, "停止BPU成功");
    rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
    rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
    rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
    rcvdata.sMsgBuf->sDBHead.cZip = 0;
    m_pDrebApi->SendMsg(rcvdata);
    return;
}
void CDrebMsgThread::MonitorStartBpu(S_BPC_RSMSG& rcvdata)
{
    if (rcvdata.sMsgBuf->sDBHead.cZip != 0)
    {
        if (!UnzipBuf(*rcvdata.sMsgBuf))
        {
            sprintf(rcvdata.sMsgBuf->sBuffer, "数据解压缩出错");
            rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
            rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
            rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
            rcvdata.sMsgBuf->sDBHead.cZip = 0;
            m_pDrebApi->SendMsg(rcvdata);
            return;
        }
    }
    CBF_Xml  xmlpack;
    if (!xmlpack.FromBuffer(rcvdata.sMsgBuf->sBuffer))
    {
        sprintf(rcvdata.sMsgBuf->sBuffer, "请求数据非xml");
        rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
        rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
        rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
        rcvdata.sMsgBuf->sDBHead.cZip = 0;
        m_pDrebApi->SendMsg(rcvdata);
        return;
    }
    CXmlNode* stopnode = xmlpack.GetNodeByPath("Monitor/StartBpu", false);
    if (stopnode == NULL)
    {
        sprintf(rcvdata.sMsgBuf->sBuffer, "xml无[Monitor/StartBpu]节点");
        rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
        rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
        rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
        rcvdata.sMsgBuf->sDBHead.cZip = 0;
        m_pDrebApi->SendMsg(rcvdata);
        return;
    }
    CGResource* res = (CGResource*)m_pRes;
    int i;
    int bpuindex = -1;
    std::string bpugroupname;
    int bpugnum = 0;
    S_PROCESS_NODE node;
    bool isfind = false;
    int k;
    int connNum = 0;
    char errmsg[200];
    CXmlNode* bpunode = (CXmlNode*)stopnode->GetFirstChild();
    while (bpunode != NULL)
    {
        bpugroupname = bpunode->GetNodeName();
        isfind = false;
        for (i = 0; i < res->g_vBpuGroupInfo.size(); i++)
        {
            if (bpugroupname.compare(res->g_vBpuGroupInfo[i].g_sBpuGroupName) == 0)
            {
                isfind = true;
                bpuindex = i;
                break;
            }
        }
        if (isfind)
        {
            node.prgname = res->g_vBpuGroupInfo[bpuindex].g_sBuPrgName;
            node.prgpara = res->g_vBpuGroupInfo[bpuindex].g_sBuPrgPara;
            node.prgpath = res->g_vBpuGroupInfo[bpuindex].g_sBuPrgPath;
            node.runpath = res->g_vBpuGroupInfo[bpuindex].g_sBuRunPath;
            node.nodeid = res->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName;

            connNum = 0;
            for (k = res->g_vBpuGroupInfo[bpuindex].g_nBeginSocketIndex; k < res->g_vBpuGroupInfo[bpuindex].g_nEndSocketIndex; k++)
            {
                if (m_pSockMgr->at(k)->m_sock != INVALID_SOCKET)
                {
                    connNum++;
                }
            }
            if (!res->g_vBpuGroupInfo[bpuindex].g_bIsAutoLoadBu) //不是自动重启才进入启动
            {
                if (res->g_vBpuGroupInfo[bpuindex].g_nDispatchMode == 1) //java版本
                {
                    //					bpugnum = m_pRes->g_vBpuGroupInfo[bpuindex].g_nBpuNum - m_pRes->g_vBpuGroupInfo[bpuindex].g_nStartBpuNum;
                    bpugnum = res->g_vBpuGroupInfo[bpuindex].g_nBpuNum - connNum;
                    if (bpugnum > 0)
                    {
                        m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "查找原BPU[%s]进程是否存在[%d]", \
                            res->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(), res->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid);
                        if (res->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid > 0) //查看进程是否存在
                        {
                            node.pid = res->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid;
                            if (m_pMgrBpu.FindProcess(&node, errmsg) == 0) //有此进程
                            {
                                m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "原BPU[%s]进程[%d]已存在，停止", \
                                    res->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(), res->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid);
                                if (m_pMgrBpu.StopProcess(&node, errmsg) != 0)
                                {
                                    m_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, "BPU[%s]进程[%d]停止失败", \
                                        res->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(), res->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid);
                                }
                            }
                            m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "BPU[%s]重启", \
                                res->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str());
                            if (m_pMgrBpu.StopProcess(&node, errmsg) != 0)
                            {
                                m_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, "BPU[%s]进程[%d]停止失败", \
                                    res->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(), res->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid);
                            }
                        }
                    }
                }
                else
                {
                    //					bpugnum = m_pRes->g_vBpuGroupInfo[bpuindex].g_nBpuNum - m_pRes->g_vBpuGroupInfo[bpuindex].g_nStartBpuNum;
                    bpugnum = res->g_vBpuGroupInfo[bpuindex].g_nBpuNum - connNum;
                    for (int j = 0; j < bpugnum; j++)
                    {
                        if (m_pMgrBpu.StartProcess(&node, errmsg) != 0)
                        {
                            m_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, "BPU[%s]启动失败", \
                                res->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str());
                        }
                    }
                }
            }
        }
        bpunode = (CXmlNode*)bpunode->getNextSibling();
    }
    sprintf(rcvdata.sMsgBuf->sBuffer, "启动BPU成功");
    rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
    rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
    rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
    rcvdata.sMsgBuf->sDBHead.cZip = 0;
    m_pDrebApi->SendMsg(rcvdata);
    return;
}


