// DrebMsgThread.cpp: implementation of the CDrebMsgThread class.
//
//////////////////////////////////////////////////////////////////////

#include "DrebMsgThread.h"
#include "TradeResource.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern int   g_pRunFlag;

CDrebMsgThread::CDrebMsgThread()
{
	m_bIsCheck = false;
	m_pLog = NULL;
	m_pAnsProc = NULL;
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
	m_pTime.Init(100,true);
	m_pTime.SetTimer(0,((CTradeResource *)m_pRes)->m_nTradeReportTime*1000,&CDrebMsgThread::OnTimer,this);
	m_pTime.SetTimer(1,5000,&CDrebMsgThread::OnTimer,this);
	m_pTime.SetTimer(2,5000,&CDrebMsgThread::OnTimer,this);
	

	char errmsg[200];
	bzero(errmsg,sizeof(errmsg));
	if (!m_pXdp.InitXdp(((CTradeResource *)m_pRes)->m_sXdpDefineXml.c_str(),errmsg))
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP Init error %s",errmsg);
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP报文初始化出错 %s",errmsg);
#endif
		return false;
	}
	if (!m_pSendXdp.XdpCopy(&m_pXdp))
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP XdpCopy error");
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP报文初始化XdpCopy出错 ");
#endif
		return false;
	}
	return true;
}
int CDrebMsgThread::OnTimer(unsigned int event, void *p)
{
	CDrebMsgThread *pp = (CDrebMsgThread *)p;
	if (event == 0)
	{
		//发送主备状态给仲裁
		pp->Send2Arb();
	}
	else if (event == 2)
	{
		pp->PrintHostStatus();
	}
	else if (event == 1)
	{
		pp->SendQueryHostStatus();
	}
	return 0;
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
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"Recv DREB connect rsp");
#else
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"收到DREB连接成功的响应");
#endif
	CTradeResource *res = (CTradeResource *)m_pRes;
	if (res->m_nArbMode != 1)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"Set Living mode");
#else
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"设置为负载均衡模式，无须主备仲裁");
#endif
		res->m_nSvrHostStatus = ARBSTATUS_MASTER;
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		//注册交易
		for (int i=0; i< m_pRes->g_vDrebLinkInfo.size(); i++)
		{
			m_pDrebApi->RegisterDreb(i,&((CTradeResource *)m_pRes)->g_lArbTxList);
		}
		return;
	}
	if (((CTradeResource *)m_pRes)->m_nArbMode != 1)
	{
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	//发起查询，服务在仲裁的状态，根据状态来注册
	char errmsg[4096];
	bzero(errmsg,sizeof(errmsg));
	m_pXdp.ResetData();
	if (!m_pXdp.SetFieldValue("uint3",m_pRes->g_nSvrMainId,errmsg))
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"Xdp Set error");
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xdp设置主机状态出错 %s",errmsg);
#endif
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (!m_pXdp.SetFieldValue("uint4",(unsigned int)(m_pRes->g_nSvrPrivateId),errmsg))
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"Xdp Set error");
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xdp设置返回码出错 %s",errmsg);
#endif
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (m_pLog->isWrite(LOG_DEBUG+1))
	{
		m_pXdp.PrintXdp(errmsg);
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"query host status: %s",errmsg);
#else
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"应用查询主机状态报文 %s",errmsg);
#endif
	}
	unsigned int datalen = BPUDATASIZE;
	if (!m_pXdp.ToBuffer(rcvdata.sMsgBuf->sBuffer,datalen,errmsg))
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ToBuffer error %s",errmsg);
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xdp ToBuffer出错 %s",errmsg);
#endif
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	rcvdata.sMsgBuf->sDBHead.nLen = datalen;
	
	rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 0;
	rcvdata.sMsgBuf->sDBHead.cCmd = CMD_DPCALL;
	rcvdata.sMsgBuf->sDBHead.cZip = 0;
	rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo = APP_QUERY_HOST;
	m_bIsQueryAns = false;//查询应答为false，当一直为false时要重新发起查询，防止仲裁问题导致服务不能起来
	m_pDrebApi->SendMsg(rcvdata);

	
}

// 函数名: OnMsgDrebAns
// 编程  : 王明松 2015-4-23 15:55:58
// 返回  : virtual void 
// 参数  : S_BPC_RSMSG &rcvdata
// 描述  : 响应总线过来的应答数据
void CDrebMsgThread::OnMsgDrebAns(S_BPC_RSMSG &rcvdata)
{
	//应答数据

	if (rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo == APP_QUERY_HOST)
	{
#ifdef _ENGLISH_
        m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "Recv DREB RSP");
#else
        m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "收到DREB的应答响应");
#endif
		OnQueryArbAns(rcvdata);
		return;
	}
	else
	{
		S_TRADE_DATA data;
		data.src = 0;
		data.rTime = time(NULL);
		data.index = rcvdata.index;
		data.pData = rcvdata.sMsgBuf;
		m_pAnsProc->PushAnsQueue(data);
//		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
	}
	

}

// 函数名: OnMsgRequest
// 编程  : 王明松 2015-4-23 15:54:38
// 返回  : virtual void 
// 参数  : S_BPC_RSMSG &rcvdata
// 描述  : 处理总线过来的请求数据
void CDrebMsgThread::OnMsgRequest(S_BPC_RSMSG &rcvdata)
{
	if (rcvdata.sMsgBuf->sDBHead.cRaflag !=0) //不为请求
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"not function msg[%s]  DREB[%s] next[%d] RA[%d] txcode[%d]  flag[%d %d %d] src[%d %d] cZip[%d] datalen[%d]",\
			GetBpcMsgType(rcvdata.sMsgBuf->sBpcHead.cMsgType).c_str(),\
			GetDrebCmdType(rcvdata.sMsgBuf->sDBHead.cCmd).c_str(),rcvdata.sMsgBuf->sDBHead.cNextFlag,\
			rcvdata.sMsgBuf->sDBHead.cRaflag,rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
			rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
			rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
			rcvdata.sMsgBuf->sDBHead.cZip,rcvdata.sMsgBuf->sDBHead.nLen);
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"无此功能 消息[%s]  DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 源[%d %d] cZip[%d] 业务数据长度[%d]",\
			GetBpcMsgType(rcvdata.sMsgBuf->sBpcHead.cMsgType).c_str(),\
			GetDrebCmdType(rcvdata.sMsgBuf->sDBHead.cCmd).c_str(),rcvdata.sMsgBuf->sDBHead.cNextFlag,\
			rcvdata.sMsgBuf->sDBHead.cRaflag,rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
			rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
			rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
			rcvdata.sMsgBuf->sDBHead.cZip,rcvdata.sMsgBuf->sDBHead.nLen);
#endif
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	switch (rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo)
	{
		case  ARB_SEND_CONTROL:
			OnArbControl(rcvdata);//指定主机
			break;
		case  ARB_SEND_QUERY://查询状态
			OnArbQuery(rcvdata);
			break;
		default:
			//服务的正常交易请求
			OnSvrRequest(rcvdata);
			break;
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
	if (m_pTime.IsStoped())
	{
		m_pTime.Start();
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
	if (!m_pTime.IsStoped())
	{
		m_pTime.Stop();
	}
	m_bIsRunning = false;
	Join();
}

void CDrebMsgThread::Send2Arb()
{
	CTradeResource *arbconf = (CTradeResource *)m_pRes;
	if (arbconf->m_nSvrHostStatus == ARBSTATUS_UNKNOW)
	{
		//未知状态不用去发了
		return;
	}
	if (arbconf->m_nArbMode != 1)
	{
		//非主备模式，不用发
		return;
	}
	S_BPC_RSMSG data;
	data.sMsgBuf = (PBPCCOMMSTRU)m_pDrebApi->PoolMalloc();
	if (data.sMsgBuf == NULL)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"SVR[%d %d] Send to ARB PoolMalloc error",m_pRes->g_nSvrMainId,m_pRes->g_nSvrPrivateId);
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送主备状态通知失败[%d %d] PoolMalloc error",m_pRes->g_nSvrMainId,m_pRes->g_nSvrPrivateId);
#endif
		return;
	}
	char errmsg[250];
	bzero(errmsg,sizeof(errmsg));
	bzero(data.sMsgBuf,sizeof(BPCCOMMSTRU));
	data.sMsgBuf->sBpcHead.nIndex = 100;
	data.sMsgBuf->sDBHead.cCmd = CMD_DPPUSH;
	data.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo = APP_PUSH_STATUS;
	data.sMsgBuf->sDBHead.d_Dinfo.d_nSvrMainId = 0;
	data.sMsgBuf->sDBHead.d_Dinfo.d_cSvrPrivateId = 0;
	
	if (!m_pSendXdp.SetFieldValue("uint3",m_pRes->g_nSvrMainId,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP SetFieldValue uint3 error: %s",errmsg);
		m_pDrebApi->PoolFree(data.sMsgBuf);
		return;
	}
	if (!m_pSendXdp.SetFieldValue("uint4",(unsigned int)m_pRes->g_nSvrPrivateId,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP SetFieldValue uint4 error: %s",errmsg);
		m_pDrebApi->PoolFree(data.sMsgBuf);
		return;
	}
	if (!m_pSendXdp.SetFieldValue("uint9",((CTradeResource *)m_pRes)->m_nSvrHostStatus,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP SetFieldValue uint9 error: %s",errmsg);
		m_pDrebApi->PoolFree(data.sMsgBuf);
		return;
	}
	unsigned int len = BPUDATASIZE;
	if (!m_pSendXdp.ToBuffer(data.sMsgBuf->sBuffer,len,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP ToBuffer error: %s",errmsg);
		m_pDrebApi->PoolFree(data.sMsgBuf);
		return;
	}
	data.sMsgBuf->sDBHead.nLen = len;
//	m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"发给仲裁通知");
	m_pDrebApi->SendMsg(data); //发给仲裁通知
}

void CDrebMsgThread::OnQueryArbAns(S_BPC_RSMSG &rcvdata)
{
	//查询主备状态的报文
	char errmsg[4096];
	bzero(errmsg,sizeof(errmsg));
	if (!m_pXdp.FromBuffer(rcvdata.sMsgBuf->sBuffer,rcvdata.sMsgBuf->sDBHead.nLen,errmsg))
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"arb rsp non XDP %s",errmsg);
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"arb返回的报文不符，非XDP报文 %s",errmsg);
#endif
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (m_pLog->isWrite(LOG_DEBUG+1))
	{
		m_pXdp.PrintXdp(errmsg);
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"arb rsp %s",errmsg);
#else
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"arb返回应用查询报文 %s",errmsg);
#endif
	}
	unsigned int status=0;
	unsigned int retcode=0;

	if (!m_pXdp.GetFieldValue("uint9",status,errmsg))
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"arb rsp ,no uint9 field %s",errmsg);
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"arb返回应用查询报文不符，无uint9字段 %s",errmsg);
#endif
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (!m_pXdp.GetFieldValue("uint10",retcode,errmsg))
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"arb rsp ,no uint10 field %s",errmsg);
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"arb返回应用查询报文不符，无uint10字段 %s",errmsg);
#endif
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}	
	if (retcode != SUCCESS)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"arb rsp ,ret[%d] error",retcode);
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"arb返回应用查询 的返回码[%d]为不成功",retcode);
#endif
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
#ifdef _ENGLISH_
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"arb rsp app status[%d]",status);
#else
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"arb返回应用查询 的应用状态[%d]",status);
#endif
	if (status == ARBSTATUS_MASTER || status == ARBSTATUS_MASTER_AYN )
	{
		//为主机
		OnChangeHostStatus(status);
	}
	else if (status == ARBSTATUS_BACKUP || status == status == ARBSTATUS_BACKUP_AYN)
	{
		OnChangeHostStatus(status);
	}
	else
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"arb rsp app status [%d] illegal",status);
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"arb返回应用查询 的应用状态[%d]不符",status);
#endif
	}
	m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
	rcvdata.sMsgBuf = NULL;
	m_bIsQueryAns = true;
	return;
	
}

void CDrebMsgThread::OnChangeHostStatus(unsigned int status)
{
	CTradeResource *arbconf = (CTradeResource *)m_pRes;
#ifdef _ENGLISH_
	m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"Recv  ARB  cmd status[%d], current status[%d]",status,arbconf->m_nSvrHostStatus);
#else
	m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"收到仲裁通知  指定状态[%d] 当前状态[%d]",status,arbconf->m_nSvrHostStatus);
#endif
	if (status == ARBSTATUS_MASTER || status == ARBSTATUS_MASTER_AYN )
	{
		//为主机
		if (arbconf->m_nSvrHostStatus != ARBSTATUS_MASTER && arbconf->m_nSvrHostStatus != ARBSTATUS_MASTER_AYN)
		{
			//转为主机
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"SVR is HOST ,begin start");
#else
			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"本服务为主机  开始启动api");
#endif
			//注册交易
			for (int i=0; i< m_pRes->g_vDrebLinkInfo.size(); i++)
			{
				m_pDrebApi->RegisterDreb(i,&((CTradeResource *)m_pRes)->g_lSvrTxList);
			}
			arbconf->m_nSvrHostStatus = ARBSTATUS_MASTER;
			//进行转为主机的处理，如连接交易所等
			OnHostProc();
		}
	}
	else if (status == ARBSTATUS_BACKUP || status  == ARBSTATUS_BACKUP_AYN)
	{
		if (arbconf->m_nSvrHostStatus != ARBSTATUS_BACKUP && arbconf->m_nSvrHostStatus != ARBSTATUS_BACKUP_AYN)
		{
			if (arbconf->m_nSvrHostStatus == ARBSTATUS_UNKNOW)
			{
				arbconf->m_nSvrHostStatus = ARBSTATUS_BACKUP;
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"SVR is backup");
#else
				m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"本服务为备机");
#endif
				return;
			}
			//转为备机
			for (int i=0; i< m_pRes->g_vDrebLinkInfo.size(); i++)
			{
				m_pDrebApi->UnRegisterDreb();
				m_pDrebApi->RegisterDreb(i,&((CTradeResource *)m_pRes)->g_lArbTxList);
			}
			arbconf->m_nSvrHostStatus = ARBSTATUS_BACKUP;
		}
	}
}

void CDrebMsgThread::OnSvrRequest(S_BPC_RSMSG &rcvdata)
{
	S_TRADE_DATA data;
	data.src = 0;   //表示是总线过来的
	data.pData = rcvdata.sMsgBuf;
	data.pData->sBpcHead.nBpuIndex = 0;//表示是总线过来的
	data.rTime = time(NULL);
// 	//去掉广播
// 	if (rcvdata.sMsgBuf->sDBHead.cCmd == CMD_DPABC || rcvdata.sMsgBuf->sDBHead.cCmd == CMD_DPBC)
// 	{
// 		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
// 		rcvdata.sMsgBuf = NULL;
// 		return;
// 	}

	//放入队列
	m_pRcvQueue->PushData(data);

	return;
}

void CDrebMsgThread::OnArbControl(S_BPC_RSMSG &rcvdata)
{
	char errmsg[4096];
	bzero(errmsg,sizeof(errmsg));
	if (!m_pXdp.FromBuffer(rcvdata.sMsgBuf->sBuffer,rcvdata.sMsgBuf->sDBHead.nLen,errmsg))
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ARB non XDP %s",errmsg);
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"仲裁指定报文不符，非XDP报文 %s",errmsg);
#endif
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (m_pLog->isWrite(LOG_DEBUG+1))
	{
		m_pXdp.PrintXdp(errmsg);
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"ARB Control XDP: %s",errmsg);
#else
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"仲裁指定报文 %s",errmsg);
#endif
	}
	unsigned int id=0;
	unsigned int pid=0;
	unsigned int status=0;
	
	if (!m_pXdp.GetFieldValue("uint3",id,errmsg))
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ARB Control XDP: %s",errmsg);
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"仲裁指定报文不符 无uint3字段 %s",errmsg);
#endif
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	
	if (!m_pXdp.GetFieldValue("uint4",pid,errmsg))
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ARB Control illegal no uint4 field %s",errmsg);
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"仲裁指定报文不符 无uint4字段 %s",errmsg);
#endif
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	
	if (!m_pXdp.GetFieldValue("uint9",status,errmsg))
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ARB Control illegal no uint9 field %s",errmsg);
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"仲裁指定报文不符 无uint9字段 %s",errmsg);
#endif
		
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (id != m_pRes->g_nSvrMainId || pid != m_pRes->g_nSvrPrivateId)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ARB Control illegal ,svr[%d %d] ",id,pid);
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"仲裁指定报文不符 服务[%d %d] 不是本服务",id,pid);
#endif
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
#ifdef _ENGLISH_
	m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"Arb Control status[%d]",status);
#else
	m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"ARB指定的状态为[%d]",status);
#endif
	//改变状态
	OnChangeHostStatus(status);
	CTradeResource *arbconf = (CTradeResource *)m_pRes;
	if (!m_pXdp.SetFieldValue("uint9",arbconf->m_nSvrHostStatus,errmsg))
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP  SetFieldValue error");
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP  SetFieldValue 出错");
#endif
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (!m_pXdp.SetFieldValue("uint10",(unsigned int)SUCCESS,errmsg))
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP  SetFieldValue error");
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP  SetFieldValue 出错");
#endif
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (m_pLog->isWrite(LOG_DEBUG+1))
	{
		m_pXdp.PrintXdp(errmsg);
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"answer to ARB: %s",errmsg);
#else
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"仲裁指定主机应答报文 %s",errmsg);
#endif
	}
	unsigned int datalen = BPUDATASIZE;
	if (!m_pXdp.ToBuffer(rcvdata.sMsgBuf->sBuffer,datalen,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xdp ToBuffer error %s",errmsg);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	rcvdata.sMsgBuf->sDBHead.nLen = datalen;
	
	rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
	rcvdata.sMsgBuf->sDBHead.cZip = 0;
	m_pDrebApi->SendMsg(rcvdata);
	


}

void CDrebMsgThread::OnArbQuery(S_BPC_RSMSG &rcvdata)
{
	char errmsg[4096];
	bzero(errmsg,sizeof(errmsg));
	if (!m_pXdp.FromBuffer(rcvdata.sMsgBuf->sBuffer,rcvdata.sMsgBuf->sDBHead.nLen,errmsg))
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ARB query illegal, FromBuffer error: %s",errmsg);
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"仲裁查询报文不符，非XDP报文 %s",errmsg);
#endif
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (m_pLog->isWrite(LOG_DEBUG+1))
	{
		m_pXdp.PrintXdp(errmsg);
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"ARB query data: %s",errmsg);
#else
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"仲裁查询报文 %s",errmsg);
#endif
	}
	unsigned int id=0;
	unsigned int pid=0;
	unsigned int status=0;
	
	if (!m_pXdp.GetFieldValue("uint3",id,errmsg))
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ARB query illegal ,no uint3 field");
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"仲裁查询报文不符 无uint3字段 %s",errmsg);
#endif
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	
	if (!m_pXdp.GetFieldValue("uint4",pid,errmsg))
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ARB query illegal ,no uint4 field");
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"仲裁查询报文不符 无uint4字段 %s",errmsg);
#endif
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	
	
	if (id != m_pRes->g_nSvrMainId || pid != m_pRes->g_nSvrPrivateId)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ARB query illegal ,svr[%d %d]",id,pid);
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"仲裁查询报文不符 服务[%d %d] 不是本服务",id,pid);
#endif
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	
	CTradeResource *arbconf = (CTradeResource *)m_pRes;
	if (!m_pXdp.SetFieldValue("uint9",arbconf->m_nSvrHostStatus,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP  SetFieldValue error");
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (!m_pXdp.SetFieldValue("uint10",(unsigned int)SUCCESS,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP  SetFieldValue error");
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (m_pLog->isWrite(LOG_DEBUG+1))
	{
		m_pXdp.PrintXdp(errmsg);
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"ARB query answer: %s",errmsg);
#else
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"仲裁查询应答报文 %s",errmsg);
#endif
	}
	unsigned int datalen = BPUDATASIZE;
	if (!m_pXdp.ToBuffer(rcvdata.sMsgBuf->sBuffer,datalen,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xdp ToBuffer error %s",errmsg);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	rcvdata.sMsgBuf->sDBHead.nLen = datalen;
	
	rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
	rcvdata.sMsgBuf->sDBHead.cZip = 0;
	m_pDrebApi->SendMsg(rcvdata);	
}

void CDrebMsgThread::PrintHostStatus()
{
#ifdef _ENGLISH_
	switch (((CTradeResource *)m_pRes)->m_nSvrHostStatus)
	{
	case ARBSTATUS_UNKNOW:
		printf("node[%d %d] current status [unknow]\n",m_pRes->g_nSvrMainId,m_pRes->g_nSvrPrivateId);
		
		break;
	case ARBSTATUS_MASTER:
		printf("node[%d %d] current status [master]\n",m_pRes->g_nSvrMainId,m_pRes->g_nSvrPrivateId);
		break;
	case ARBSTATUS_MASTER_AYN:
		printf("node[%d %d] current status [master-ayn]\n",m_pRes->g_nSvrMainId,m_pRes->g_nSvrPrivateId);
		break;
	case ARBSTATUS_BACKUP:
		printf("node[%d %d] current status [backup]\n",m_pRes->g_nSvrMainId,m_pRes->g_nSvrPrivateId);
		break;
	case ARBSTATUS_BACKUP_AYN:
		printf("node[%d %d] current status [backup_ayn]\n",m_pRes->g_nSvrMainId,m_pRes->g_nSvrPrivateId);
		break;
	case MASTERHOST:
		printf("node[%d %d] current status [LIVING]\n",m_pRes->g_nSvrMainId,m_pRes->g_nSvrPrivateId);
		break;
	default: 
		printf("node[%d %d] current status error %d\n",m_pRes->g_nSvrMainId,m_pRes->g_nSvrPrivateId,((CTradeResource *)m_pRes)->m_nSvrHostStatus);
		break;
		
	}
#else

	switch (((CTradeResource *)m_pRes)->m_nSvrHostStatus)
	{
		case ARBSTATUS_UNKNOW:
			printf("节点[%d %d]当前服务状态为 [未知]\n",m_pRes->g_nSvrMainId,m_pRes->g_nSvrPrivateId);

			break;
		case ARBSTATUS_MASTER:
			printf("节点[%d %d]当前服务状态为 [主机]\n",m_pRes->g_nSvrMainId,m_pRes->g_nSvrPrivateId);
			break;
		case ARBSTATUS_MASTER_AYN:
			printf("节点[%d %d]当前服务状态为 [主机同步]\n",m_pRes->g_nSvrMainId,m_pRes->g_nSvrPrivateId);
			break;
		case ARBSTATUS_BACKUP:
			printf("节点[%d %d]当前服务状态为 [备机]\n",m_pRes->g_nSvrMainId,m_pRes->g_nSvrPrivateId);
			break;
		case ARBSTATUS_BACKUP_AYN:
			printf("节点[%d %d]当前服务状态为 [备机同步]\n",m_pRes->g_nSvrMainId,m_pRes->g_nSvrPrivateId);
			break;
		case MASTERHOST:
			printf("节点[%d %d]当前服务状态为 [负载均衡主机]\n",m_pRes->g_nSvrMainId,m_pRes->g_nSvrPrivateId);
			break;
		default: 
			printf("节点[%d %d]当前服务状态为错误状态 %d\n",m_pRes->g_nSvrMainId,m_pRes->g_nSvrPrivateId,((CTradeResource *)m_pRes)->m_nSvrHostStatus);
			break;

	}
#endif	
}

void CDrebMsgThread::SendQueryHostStatus()
{
	if (((CTradeResource *)m_pRes)->m_nArbMode != 1)
	{
		return;
	}
	if (!m_bIsQueryAns)
	{
		S_BPC_RSMSG rcvdata;
		rcvdata.sMsgBuf = (PBPCCOMMSTRU) m_pDrebApi->PoolMalloc();
		if (rcvdata.sMsgBuf== NULL)
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"PoolMalloc error ");
			return;
		}
		//发起查询，服务在仲裁的状态，根据状态来注册
		char errmsg[4096];
		bzero(errmsg,sizeof(errmsg));
		m_pSendXdp.ResetData();
		if (!m_pSendXdp.SetFieldValue("uint3",m_pRes->g_nSvrMainId,errmsg))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xdp SetFieldValue error %s",errmsg);
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			return;
		}
		if (!m_pSendXdp.SetFieldValue("uint4",(unsigned int)(m_pRes->g_nSvrPrivateId),errmsg))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xdp SetFieldValue error %s",errmsg);
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			return;
		}
		if (m_pLog->isWrite(LOG_DEBUG+1))
		{
			m_pSendXdp.PrintXdp(errmsg);
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"app query data: %s",errmsg);
#else
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"应用查询主机状态报文 %s",errmsg);
#endif
		}
		unsigned int datalen = BPUDATASIZE;
		if (!m_pSendXdp.ToBuffer(rcvdata.sMsgBuf->sBuffer,datalen,errmsg))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xdp ToBuffer error %s",errmsg);
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			return;
		}
		rcvdata.sMsgBuf->sDBHead.nLen = datalen;
		
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 0;
		rcvdata.sMsgBuf->sDBHead.cCmd = CMD_DPCALL;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo = APP_QUERY_HOST;
		m_bIsQueryAns = false;//查询应答为false，当一直为false时要重新发起查询，防止仲裁问题导致服务不能起来
		m_pDrebApi->SendMsg(rcvdata);

	}
}

// 函数名: OnMsgReportBpc
// 编程  : 王明松 2015-4-23 16:01:43
// 返回  : virtual void 
// 参数  : S_BPC_RSMSG &rcvdata
// 描述  : 主动上报监控信息，发给总线
void CDrebMsgThread::OnMsgReportBpc(S_BPC_RSMSG &rcvdata)
{
	//监控信息
	CBF_Xml        pXml;
	
	if (!pXml.FromBuffer(rcvdata.sMsgBuf->sBuffer))
	{
		m_pDrebApi->GetLogPoint()->LogMp(LOG_ERROR,__FILE__,__LINE__,"API过来的非xml格式");
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	//报告报盘机的连接状态
// 	<扩展信息 name="子数据不能重名">
//         <DATA status="0">连接池最大连接数22 空闲连接数21 客户连接数0 内存缓冲池总分配100 未使用100 每块大小65534</DATA>
//         <DATA2 status="0">连接池最大连接数22 空闲连接数21 客户连接数0 内存缓冲池总分配100 未使用100 每块大小65534</DATA2>
// 	</扩展信息>
	
	int ret;
	char tmpchar[200];
	char tmpchar2[200];

	pXml.SetNodeValueByPath("/Monitor/节点类型",false,2);//类BPC

	pXml.SetNodeValueByPath("/Monitor/扩展信息",false,"",false);
	//每隔1秒检查连接的状态，未连接则尝试去连接

	sprintf(tmpchar,"/Monitor/扩展信息/%s/","tradekernel");
		

	switch (((CTradeResource *)m_pRes)->m_nSvrHostStatus)
	{
		case ARBSTATUS_UNKNOW:
			sprintf(tmpchar2,"主备未知，可能仲裁未启动或网络不通");
			pXml.SetNodeValueByPath(tmpchar,false,tmpchar2,false);
			pXml.SetNodeAttribute(tmpchar,false,"status",false,1);
			break;
		case ARBSTATUS_MASTER:
		case ARBSTATUS_MASTER_AYN:
			sprintf(tmpchar2,"主机状态");
			pXml.SetNodeValueByPath(tmpchar,false,tmpchar2,false);
			pXml.SetNodeAttribute(tmpchar,false,"status",false,0);//状态正常
			break;
		case ARBSTATUS_BACKUP:
		case ARBSTATUS_BACKUP_AYN:
			sprintf(tmpchar2,"备机状态");
			pXml.SetNodeValueByPath(tmpchar,false,tmpchar2,false);
			pXml.SetNodeAttribute(tmpchar,false,"status",false,0);//状态正常
			break;
		case MASTERHOST:
			sprintf(tmpchar2,"多主机状态");
			pXml.SetNodeValueByPath(tmpchar,false,tmpchar2,false);
			pXml.SetNodeAttribute(tmpchar,false,"status",false,0);//状态正常
			break;
		default: 
			printf("节点[%d %d]当前服务状态为错误状态 %d\n",m_pRes->g_nSvrMainId,m_pRes->g_nSvrPrivateId,((CTradeResource *)m_pRes)->m_nSvrHostStatus);
			break;
			
	}

	int len = BPUDATASIZE;
	if (!pXml.ToBuffer(rcvdata.sMsgBuf->sBuffer,len))
	{
		m_pDrebApi->GetLogPoint()->LogMp(LOG_ERROR,__FILE__,__LINE__,"ToBuffer出错");
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	rcvdata.sMsgBuf->sDBHead.nLen = len;
	m_pDrebApi->GetLogPoint()->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"监控通知",rcvdata.sMsgBuf->sBuffer,rcvdata.sMsgBuf->sDBHead.nLen);
	//发给总线	
	rcvdata.sMsgBuf->sBpcHead.nIndex = 100;
	m_pDrebApi->SendMsg(rcvdata);
	return;
}

void CDrebMsgThread::OnHostProc()
{
	//进行转为主机的处理，连接交易所等等
}
