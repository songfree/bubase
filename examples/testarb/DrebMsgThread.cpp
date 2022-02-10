// DrebMsgThread.cpp: implementation of the CDrebMsgThread class.
//
//////////////////////////////////////////////////////////////////////

#include "DrebMsgThread.h"
#include "ArbResource.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDrebMsgThread::CDrebMsgThread()
{
	m_bIsCheck = false;
	m_pLog = NULL;
}

CDrebMsgThread::~CDrebMsgThread()
{

}
bool CDrebMsgThread::Init(CBF_DrebServer   *bus,CBF_DrebResource *res)
{
	m_pDrebApi = bus;
	m_pRes = res;
	m_pLog = m_pDrebApi->GetLogPoint();
	m_pTime.Init(100,true);
	m_pTime.SetTimer(0,((CArbResource *)m_pRes)->m_nArbReportTime*1000,&CDrebMsgThread::OnTimer,this);
	m_pTime.SetTimer(1,5000,&CDrebMsgThread::OnTimer,this);
	m_pTime.SetTimer(2,5000,&CDrebMsgThread::OnTimer,this);
	

	char errmsg[200];
	bzero(errmsg,sizeof(errmsg));
	if (!m_pXdp.InitXdp(((CArbResource *)m_pRes)->m_sXdpDefineXml.c_str(),errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP报文初始化出错 %s",errmsg);
		return false;
	}
	if (!m_pSendXdp.XdpCopy(&m_pXdp))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP报文初始化XdpCopy出错 %s",errmsg);
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
	while (m_bIsRunning)
	{
		if (m_pDrebApi->GetMsgData(m_rMsgData))
		{
			ProcessDrebData(m_rMsgData);
		}
	}
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
	m_pDrebApi->GetLogPoint()->LogMp(LOG_DEBUG,__FILE__,__LINE__,"收到DREB连接成功的响应");
	//发起查询，服务在仲裁的状态，根据状态来注册
	char errmsg[4096];
	bzero(errmsg,sizeof(errmsg));
	m_pXdp.ResetData();
	if (!m_pXdp.SetFieldValue("uint3",m_pRes->g_nSvrMainId,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xdp设置主机状态出错 %s",errmsg);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (!m_pXdp.SetFieldValue("uint4",(unsigned int)(m_pRes->g_nSvrPrivateId),errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xdp设置返回码出错 %s",errmsg);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (m_pLog->isWrite(LOG_DEBUG+1))
	{
		m_pXdp.PrintXdp(errmsg);
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"应用查询主机状态报文 %s",errmsg);
	}
	unsigned int datalen = BPUDATASIZE;
	if (!m_pXdp.ToBuffer(rcvdata.sMsgBuf->sBuffer,datalen,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xdp ToBuffer出错 %s",errmsg);
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
	m_pDrebApi->GetLogPoint()->LogMp(LOG_DEBUG,__FILE__,__LINE__,"收到DREB的应答响应");
	if (rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo == APP_QUERY_HOST)
	{
		OnQueryArbAns(rcvdata);
		return;
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
		m_pDrebApi->GetLogPoint()->LogMp(LOG_ERROR,__FILE__,__LINE__,"无此功能 消息[%s]  DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 源[%d %d] cZip[%d] 业务数据长度[%d]",\
			GetBpcMsgType(rcvdata.sMsgBuf->sBpcHead.cMsgType).c_str(),\
			GetDrebCmdType(rcvdata.sMsgBuf->sDBHead.cCmd).c_str(),rcvdata.sMsgBuf->sDBHead.cNextFlag,\
			rcvdata.sMsgBuf->sDBHead.cRaflag,rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
			rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
			rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
			rcvdata.sMsgBuf->sDBHead.cZip,rcvdata.sMsgBuf->sDBHead.nLen);
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
	CArbResource *arbconf = (CArbResource *)m_pRes;
	if (arbconf->m_nSvrHostStatus == ARBSTATUS_UNKNOW)
	{
		//未知状态不用去发了
		return;
	}
	S_BPC_RSMSG data;
	data.sMsgBuf = (PBPCCOMMSTRU)m_pDrebApi->PoolMalloc();
	if (data.sMsgBuf == NULL)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送主备状态通知失败[%d %d] PoolMalloc error",m_pRes->g_nSvrMainId,m_pRes->g_nSvrPrivateId);
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
	if (!m_pSendXdp.SetFieldValue("uint9",((CArbResource *)m_pRes)->m_nSvrHostStatus,errmsg))
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
	m_pDrebApi->SendMsg(data); //发给仲裁通知
}

void CDrebMsgThread::OnQueryArbAns(S_BPC_RSMSG &rcvdata)
{
	//查询主备状态的报文
	char errmsg[4096];
	bzero(errmsg,sizeof(errmsg));
	if (!m_pXdp.FromBuffer(rcvdata.sMsgBuf->sBuffer,rcvdata.sMsgBuf->sDBHead.nLen,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"arb返回的报文不符，非XDP报文 %s",errmsg);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (m_pLog->isWrite(LOG_DEBUG+1))
	{
		m_pXdp.PrintXdp(errmsg);
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"arb返回应用查询报文 %s",errmsg);
	}
	unsigned int status=0;
	unsigned int retcode=0;

	if (!m_pXdp.GetFieldValue("uint9",status,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"arb返回应用查询报文不符，无uint9字段 %s",errmsg);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (!m_pXdp.GetFieldValue("uint10",retcode,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"arb返回应用查询报文不符，无uint10字段 %s",errmsg);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}	
	if (retcode != SUCCESS)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"arb返回应用查询 的返回码[%d]为不成功",retcode);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"arb返回应用查询 的应用状态[%d]",status);
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
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"arb返回应用查询 的应用状态[%d]不符",status);
	}
	m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
	rcvdata.sMsgBuf = NULL;
	m_bIsQueryAns = true;
	return;
	
}

void CDrebMsgThread::OnChangeHostStatus(unsigned int status)
{
	CArbResource *arbconf = (CArbResource *)m_pRes;
	if (status == ARBSTATUS_MASTER || status == ARBSTATUS_MASTER_AYN )
	{
		//为主机
		if (arbconf->m_nSvrHostStatus != ARBSTATUS_MASTER && arbconf->m_nSvrHostStatus != ARBSTATUS_MASTER_AYN)
		{
			//转为主机
			arbconf->m_nSvrHostStatus = ARBSTATUS_MASTER;
			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"本服务为主机");
			//注册交易
			for (int i=0; i< m_pRes->g_vDrebLinkInfo.size(); i++)
			{
				m_pDrebApi->RegisterDreb(i,&((CArbResource *)m_pRes)->g_lSvrTxList);
			}
		}
	}
	else if (status == ARBSTATUS_BACKUP || status == status == ARBSTATUS_BACKUP_AYN)
	{
		if (arbconf->m_nSvrHostStatus != ARBSTATUS_BACKUP && arbconf->m_nSvrHostStatus != ARBSTATUS_BACKUP_AYN)
		{
			//转为备机
			arbconf->m_nSvrHostStatus = ARBSTATUS_BACKUP;
			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"本服务为备机");
		}
	}
}

void CDrebMsgThread::OnSvrRequest(S_BPC_RSMSG &rcvdata)
{
	m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
	rcvdata.sMsgBuf = NULL;
	return;
}

void CDrebMsgThread::OnArbControl(S_BPC_RSMSG &rcvdata)
{
	char errmsg[4096];
	bzero(errmsg,sizeof(errmsg));
	if (!m_pXdp.FromBuffer(rcvdata.sMsgBuf->sBuffer,rcvdata.sMsgBuf->sDBHead.nLen,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"仲裁指定报文不符，非XDP报文 %s",errmsg);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (m_pLog->isWrite(LOG_DEBUG+1))
	{
		m_pXdp.PrintXdp(errmsg);
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"仲裁指定报文 %s",errmsg);
	}
	unsigned int id=0;
	unsigned int pid=0;
	unsigned int status=0;
	
	if (!m_pXdp.GetFieldValue("uint3",id,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"仲裁指定报文不符 无uint3字段 %s",errmsg);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	
	if (!m_pXdp.GetFieldValue("uint4",pid,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"仲裁指定报文不符 无uint4字段 %s",errmsg);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	
	if (!m_pXdp.GetFieldValue("uint9",status,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"仲裁指定报文不符 无uint9字段 %s",errmsg);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (id != m_pRes->g_nSvrMainId || pid != m_pRes->g_nSvrPrivateId)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"仲裁指定报文不符 服务[%d %d] 不是本服务",id,pid);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"服务指定的状态为[%d]",status);
	//改变状态
	OnChangeHostStatus(status);
	CArbResource *arbconf = (CArbResource *)m_pRes;
	if (!m_pXdp.SetFieldValue("uint9",arbconf->m_nSvrHostStatus,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP  SetFieldValue 出错");
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (!m_pXdp.SetFieldValue("uint10",(unsigned int)SUCCESS,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP  SetFieldValue 出错");
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (m_pLog->isWrite(LOG_DEBUG+1))
	{
		m_pXdp.PrintXdp(errmsg);
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"仲裁指定主机应答报文 %s",errmsg);
	}
	unsigned int datalen = BPUDATASIZE;
	if (!m_pXdp.ToBuffer(rcvdata.sMsgBuf->sBuffer,datalen,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xdp ToBuffer出错 %s",errmsg);
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
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"仲裁查询报文不符，非XDP报文 %s",errmsg);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (m_pLog->isWrite(LOG_DEBUG+1))
	{
		m_pXdp.PrintXdp(errmsg);
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"仲裁查询报文 %s",errmsg);
	}
	unsigned int id=0;
	unsigned int pid=0;
	unsigned int status=0;
	
	if (!m_pXdp.GetFieldValue("uint3",id,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"仲裁查询报文不符 无uint3字段 %s",errmsg);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	
	if (!m_pXdp.GetFieldValue("uint4",pid,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"仲裁查询报文不符 无uint4字段 %s",errmsg);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	
	
	if (id != m_pRes->g_nSvrMainId || pid != m_pRes->g_nSvrPrivateId)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"仲裁查询报文不符 服务[%d %d] 不是本服务",id,pid);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	
	CArbResource *arbconf = (CArbResource *)m_pRes;
	if (!m_pXdp.SetFieldValue("uint9",arbconf->m_nSvrHostStatus,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP  SetFieldValue 出错");
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (!m_pXdp.SetFieldValue("uint10",(unsigned int)SUCCESS,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP  SetFieldValue 出错");
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (m_pLog->isWrite(LOG_DEBUG+1))
	{
		m_pXdp.PrintXdp(errmsg);
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"仲裁查询应答报文 %s",errmsg);
	}
	unsigned int datalen = BPUDATASIZE;
	if (!m_pXdp.ToBuffer(rcvdata.sMsgBuf->sBuffer,datalen,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xdp ToBuffer出错 %s",errmsg);
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
	switch (((CArbResource *)m_pRes)->m_nSvrHostStatus)
	{
		case ARBSTATUS_UNKNOW:
			printf("当服务状态为 [未知]\n");

			break;
		case ARBSTATUS_MASTER:
			printf("当服务状态为 [主机]\n");
			break;
		case ARBSTATUS_MASTER_AYN:
			printf("当服务状态为 [主机同步]\n");
			break;
		case ARBSTATUS_BACKUP:
			printf("当服务状态为 [备机]\n");
			break;
		case ARBSTATUS_BACKUP_AYN:
			printf("当服务状态为 [备机同步]\n");
			break;
		default: 
			printf("当服务状态为错误状态 %d\n",((CArbResource *)m_pRes)->m_nSvrHostStatus);
			break;

	}
	
}

void CDrebMsgThread::SendQueryHostStatus()
{
	if (!m_bIsQueryAns)
	{
		S_BPC_RSMSG rcvdata;
		rcvdata.sMsgBuf = (PBPCCOMMSTRU) m_pDrebApi->PoolMalloc();
		if (rcvdata.sMsgBuf== NULL)
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"PoolMalloc出错 ");
			return;
		}
		//发起查询，服务在仲裁的状态，根据状态来注册
		char errmsg[4096];
		bzero(errmsg,sizeof(errmsg));
		m_pSendXdp.ResetData();
		if (!m_pSendXdp.SetFieldValue("uint3",m_pRes->g_nSvrMainId,errmsg))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xdp设置主机状态出错 %s",errmsg);
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			return;
		}
		if (!m_pSendXdp.SetFieldValue("uint4",(unsigned int)(m_pRes->g_nSvrPrivateId),errmsg))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xdp设置返回码出错 %s",errmsg);
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			return;
		}
		if (m_pLog->isWrite(LOG_DEBUG+1))
		{
			m_pSendXdp.PrintXdp(errmsg);
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"应用查询主机状态报文 %s",errmsg);
		}
		unsigned int datalen = BPUDATASIZE;
		if (!m_pSendXdp.ToBuffer(rcvdata.sMsgBuf->sBuffer,datalen,errmsg))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xdp ToBuffer出错 %s",errmsg);
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
