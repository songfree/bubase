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
bool CDrebMsgThread::Init(CIBF_DrebServer* bus,CBF_DrebResource *res)
{
	m_pDrebApi = bus;
	m_pRes = res;
	m_tArb.m_pLog = m_pDrebApi->GetLogPoint();
	m_pLog = m_tArb.m_pLog;
	m_pTime.Init(100,true);
	m_pTime.SetTimer(0,1000,&CDrebMsgThread::OnTimer,this);
	char errmsg[200];
	bzero(errmsg,sizeof(errmsg));
	if (!m_pXdp.InitXdp(((CArbResource *)m_pRes)->m_sXdpDefineXml.c_str(),errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP报文初始化出错 %s",errmsg);
		return false;
	}
	return true;
}
int CDrebMsgThread::OnTimer(unsigned int event, void *p)
{
	CDrebMsgThread *pp = (CDrebMsgThread *)p;
	if (event == 0)
	{
		//置检查应用状态，以进行切换
		pp->OnCheck();
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
		if (m_bIsCheck)
		{
			//检查应用的状态，该切换就切换
			CheckArb();
			m_bIsCheck = false;
		}
	}
	return 0;
}
// 函数名: ProcessDrebData
// 编程  : 王明松 2015-4-23 15:53:34
// 返回  : virtual void 
// 参数  : S_BPC_RSMSG &rcvdata
// 描述  : 处理从总线接收过来的数据
void CDrebMsgThread::ProcessDrebData(S_BPC_RSMSG &rcvdata)
{
	if (rcvdata.sMsgBuf->sDBHead.cZip != 0)
	{
		if (!m_pDrebApi->UnzipBuf(*rcvdata.sMsgBuf))
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"数据解压缩出错");
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pDrebApi->SendMsg(rcvdata);
			return ;
		}
	}
	switch (rcvdata.sMsgBuf->sBpcHead.cMsgType)
	{
		case MSG_GETBPCNEXT://在bpc上取后续包
			OnGetBpcNextPack(rcvdata);
			break;
		case MSG_GETNEXT://在BPU上取后续包，要发给BPU处理
			OnGetBpuNextPack(rcvdata);
			break;
		case MSG_REQ://DREB上收到的请求
			OnMsgRequest(rcvdata);
			break;
		case MSG_MONITOR_SAP://DREB上收到的监控指令
			OnMsgMonitor(rcvdata);
			break;
		case MSG_CONNECTDREB://DREB返回连接成功，可以在这里进行注册交易等
			OnMsgConnectBack(rcvdata);
			break;
		case MSG_DREBANS://DREB返回外调等的应答
			OnMsgDrebAns(rcvdata);
			break;	
		case MSG_BPCMONITOR://主动上报监控信息
			OnMsgReportBpc(rcvdata);
			break;	
		default:
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			break;
	}
	return;
}

// 函数名: OnGetBpcNextPack
// 编程  : 王明松 2015-4-23 15:53:52
// 返回  : virtual void 
// 参数  : S_BPC_RSMSG &rcvdata
// 描述  : 取本地的后续包,(后续包存在本地)
void CDrebMsgThread::OnGetBpcNextPack(S_BPC_RSMSG &rcvdata)
{
	m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
	rcvdata.sMsgBuf = NULL;
}

// 函数名: OnGetBpuNextPack
// 编程  : 王明松 2015-4-23 15:54:14
// 返回  : virtual void 
// 参数  : S_BPC_RSMSG &rcvdata
// 描述  : 取BPU上的后续包，要发给BPU处理
void CDrebMsgThread::OnGetBpuNextPack(S_BPC_RSMSG &rcvdata)
{
	m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
	rcvdata.sMsgBuf = NULL;
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
	//根据不同的消息类型进行处理
	if (rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo != APP_QUERY_HOST && rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo != APP_PUSH_STATUS)
	{
// 		m_pDrebApi->GetLogPoint()->LogMp(LOG_ERROR,__FILE__,__LINE__,"不是仲裁交易 消息[%s]  DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 源[%d %d] cZip[%d] 业务数据长度[%d]",\
// 			GetBpcMsgType(rcvdata.sMsgBuf->sBpcHead.cMsgType).c_str(),\
// 			GetDrebCmdType(rcvdata.sMsgBuf->sDBHead.cCmd).c_str(),rcvdata.sMsgBuf->sDBHead.cNextFlag,\
// 			rcvdata.sMsgBuf->sDBHead.cRaflag,rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
// 			rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
// 			rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
// 			rcvdata.sMsgBuf->sDBHead.cZip,rcvdata.sMsgBuf->sDBHead.nLen);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	switch (rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo)
	{
		case APP_QUERY_HOST:
			OnAppQueryHost(rcvdata);//应用查询主备
			break;
		case APP_PUSH_STATUS: //应送推送状态通知
			OnAppPushStatus(rcvdata);
			break;
		default:
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			break;
	}

	
}

// 函数名: OnMsgMonitor
// 编程  : 王明松 2015-4-23 15:54:57
// 返回  : virtual void 
// 参数  : S_BPC_RSMSG &rcvdata
// 描述  : 处理总线过来的监控指令
void CDrebMsgThread::OnMsgMonitor(S_BPC_RSMSG &rcvdata)
{
	//DREB过来的调用
	switch (rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo)
	{
		case BPCMONI_INFO:
			MonitorInfo(rcvdata);
			break;
		case BPCMONI_HOST:
			MonitorHost(rcvdata);
			break;
		case BPCMONI_BPU:
			MonitorBpu(rcvdata);
			break;
		case BPCMONI_REPORT:
			MonitorReport(rcvdata);
			break;
		case BPCMONI_SETBPU:
			MonitorSetBpu(rcvdata);
			break;
		case BPCMONI_SETMODE:
			MonitorSetMode(rcvdata);
			break;
		case BPCMONI_STOPBPU:
			MonitorStopBpu(rcvdata);
			break;
		case BPCMONI_STARTBPU:
			MonitorStartBpu(rcvdata);
			break;
		case BPCMONI_DOWNLOAD:
			Download(rcvdata);
			break;
		case BPCMONI_UPLOAD:
			Upload(rcvdata);
			break;
		default:
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_FUNCNOTFUND;
			rcvdata.sMsgBuf->sDBHead.nLen = 0;
			rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
			m_pDrebApi->SendMsg(rcvdata);
			break;
	}
	return;
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
	//注册arb交易
	m_pDrebApi->RegisterDreb(rcvdata.sMsgBuf->sBpcHead.nIndex,&(((CArbResource *)m_pRes)->g_lArbTxList));
	m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
	rcvdata.sMsgBuf = NULL;
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
	char errmsg[4096];
	bzero(errmsg,sizeof(errmsg));
	if (rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo == ARB_SEND_QUERY)
	{
		if (rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode != SUCCESS)
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"查询应用[%d %d]状态 总线返回错 a_nRetCode[%d]",\
				rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nSvrMainId,rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_cSvrPrivateId,\
				rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode);
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			return;
		}
		if (!m_pXdp.FromBuffer(rcvdata.sMsgBuf->sBuffer,rcvdata.sMsgBuf->sDBHead.nLen,errmsg))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"应用返回报文不符，非XDP报文 %s",errmsg);
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			return;
		}
		if (m_pLog->isWrite(LOG_DEBUG+1))
		{
			m_pXdp.PrintXdp(errmsg);
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"应用返回状态查询报文 %s",errmsg);
		}
		unsigned int id=0;
		unsigned pid=0;
		unsigned status=0;
		unsigned int retcode=0;
		
		if (!m_pXdp.GetFieldValue("uint3",id,errmsg))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"应用返回状态查询报文不符，无uint3字段 %s",errmsg);
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			return;
		}
		
		if (!m_pXdp.GetFieldValue("uint4",pid,errmsg))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"应用返回状态查询报文不符，无uint4字段 %s",errmsg);
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			return;
		}
		
		if (!m_pXdp.GetFieldValue("uint9",status,errmsg))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"应用返回状态查询报文不符，无uint9字段 %s",errmsg);
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			return;
		}
		if (!m_pXdp.GetFieldValue("uint10",retcode,errmsg))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"应用返回状态查询报文不符，无uint10字段 %s",errmsg);
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			return;
		}
		
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"应用返回状态 app[%d %d] status[%d]",id,pid,status);
		if (status == ARBSTATUS_UNKNOW)
		{
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			return;
		}
		ARB_S_APPINFO *appinfo = m_tArb.UpdateArb(id,pid,status,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId);
		if (appinfo == NULL)
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"UpdateArb出错");
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			return;
		}
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo == ARB_SEND_CONTROL)
	{
		if (rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode != SUCCESS)
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"主备控制命令 应用[%d %d] 总线返回错 a_nRetCode[%d]",\
				rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nSvrMainId,rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_cSvrPrivateId,\
				rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode);
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			return;
		}
		if (!m_pXdp.FromBuffer(rcvdata.sMsgBuf->sBuffer,rcvdata.sMsgBuf->sDBHead.nLen,errmsg))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"应用返回报文不符，非XDP报文 %s",errmsg);
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			return;
		}
		if (m_pLog->isWrite(LOG_DEBUG+1))
		{
			m_pXdp.PrintXdp(errmsg);
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"应用返回指定主机报文 %s",errmsg);
		}
		unsigned int id=0;
		unsigned pid=0;
		unsigned status=0;
		unsigned int retcode=0;
		
		if (!m_pXdp.GetFieldValue("uint3",id,errmsg))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"应用返回指定主机报文不符，无uint3字段 %s",errmsg);
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			return;
		}
		
		if (!m_pXdp.GetFieldValue("uint4",pid,errmsg))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"应用返回指定主机报文不符，无uint4字段 %s",errmsg);
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			return;
		}
		
		if (!m_pXdp.GetFieldValue("uint9",status,errmsg))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"应用返回指定主机报文不符，无uint9字段 %s",errmsg);
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			return;
		}
		if (!m_pXdp.GetFieldValue("uint10",retcode,errmsg))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"应用返回指定主机报文不符，无uint10字段 %s",errmsg);
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			return;
		}
		
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"应用返回状态 app[%d %d] status[%d]",id,pid,status);
		ARB_S_APPINFO *appinfo = m_tArb.UpdateArb(id,pid,status,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId);
		if (appinfo == NULL)
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"UpdateArb出错");
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			return;
		}
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}

	m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
	rcvdata.sMsgBuf = NULL;
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
	//将应用信息组包，发给监控
	CXmlNode *arbnode =  pXml.AddNodeByPath("Monitor",false,"ARB信息",false,"");
	
	if (arbnode != NULL)
	{
		std::vector<ARB_S_APPINFO> applist;
		m_tArb.GetAppList(applist);
		char tmpchar [500];
		for (int i=0; i< applist.size(); i++)
		{
			bzero(tmpchar,sizeof(tmpchar));
			sprintf(tmpchar,"app[%d %d] status[%d] uptime[%d]",applist[i].nSvrMainId,applist[i].cSvrPrivateId,applist[i].cStatus,\
				applist[i].nUpdateTime);
			CXmlNode *tmpnode = arbnode->AddChildNode("ARBLIST");
			if (tmpnode != NULL)
			{
				tmpnode->SetNodeValue(tmpchar);
			}
		}
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
	
void CDrebMsgThread::MonitorInfo(S_BPC_RSMSG &rcvdata)
{
	int i;
	CBF_Xml xmlpack;
	xmlpack.SetNodeValueByPath("Monitor/BPC公共节点",false,(int)m_pRes->g_nSvrMainId);
	xmlpack.SetNodeValueByPath("Monitor/BPC私有节点",false,(int)m_pRes->g_nSvrPrivateId);
	xmlpack.SetNodeValueByPath("Monitor/启动时间",false,m_pRes->g_sStartDateTime,false);
	char sip[40];
	char drebstatus[40];
	int  drebport;
	int  drebid;
	int  drebprivateid;
	
	//连接信息
	for (i=0 ; i < m_pRes->g_vDrebLinkInfo.size() ; i++)
	{
		CXmlNode *connnode = xmlpack.AddNodeByPath("Monitor/DREB连接信息",false,"连接",false,"");
		if (connnode == NULL)
		{
			m_pDrebApi->GetLogPoint()->LogMp(LOG_ERROR,__FILE__,__LINE__,"AddNodeByPath 出错");
			continue;
		}
		bzero(sip,sizeof(sip));
		bzero(drebstatus,sizeof(drebstatus));
		m_pDrebApi->GetDrebInfo(i,sip,drebport,drebid,drebprivateid,drebstatus);
		connnode->SetAttribute("ip",false,sip);
		connnode->SetAttribute("port",false,drebport);
		connnode->SetAttribute("index",false,i);
		connnode->SetAttribute("公共节点",false,drebid);
		connnode->SetAttribute("私有节点",false,drebprivateid);
		connnode->SetAttribute("status",false,drebstatus);
		
	}

	int len = BPUDATASIZE;
	if (!xmlpack.ToBuffer(rcvdata.sMsgBuf->sBuffer,len))
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"缓冲不足，请下载bpcinfo.txt查看");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		m_pDrebApi->GetLogPoint()->LogMp(LOG_ERROR,__FILE__,__LINE__,"MonitorInfo 缓冲不足");
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
}
	
void CDrebMsgThread::MonitorHost(S_BPC_RSMSG &rcvdata)
{
	CBF_Xml xmlpack;
	xmlpack.SetNodeValueByPath("Monitor/BPC公共节点",false,(int)m_pRes->g_nSvrMainId);
	xmlpack.SetNodeValueByPath("Monitor/BPC私有节点",false,(int)m_pRes->g_nSvrPrivateId);
	xmlpack.SetNodeValueByPath("Monitor/启动时间",false,m_pRes->g_sStartDateTime,false);
	
	int total;
	int unused;
	int bufsize;
	m_pDrebApi->GetBufferPoolInfo(total,unused,bufsize);
	xmlpack.SetNodeAttribute("Monitor/内存缓冲池",false,"总分配",false,total );
	xmlpack.SetNodeAttribute("Monitor/内存缓冲池",false,"未使用",false,unused );
	xmlpack.SetNodeAttribute("Monitor/内存缓冲池",false,"每块大小",false,bufsize );
	//取主机信息
	S_MONITOR_HOST hf;
	vector<S_MONITOR_DISK> di;
	m_pDrebApi->GetHostInfo(hf,di);
	
	xmlpack.SetNodeValueByPath("Monitor/主机资源/CPU",false,(int)hf.nCpuRate);
	
	
	xmlpack.SetNodeValueByPath("Monitor/主机资源/内存",false,"",false);
	xmlpack.SetNodeAttribute("Monitor/主机资源/内存",false,"总大小M",false,(int)hf.nTotalMemory);
	xmlpack.SetNodeAttribute("Monitor/主机资源/内存",false,"使用M",false,(int)hf.nUsedMemory);
	
	for (int i=0 ; i<di.size() ; i++)
	{
		CXmlNode *disknode = xmlpack.AddNodeByPath("Monitor/主机资源/磁盘列表",false,"磁盘",false,"");
		if (disknode != NULL)
		{
			disknode->SetAttribute("名称",false,di[i].sDiskName);
			disknode->SetAttribute("总大小M",false,(int)di[i].nTotalSpace);
			disknode->SetAttribute("使用大小M",false,(int)di[i].nUsedSpace);
		}
	}
	
	int len = BPUDATASIZE;
	if (!xmlpack.ToBuffer(rcvdata.sMsgBuf->sBuffer,len))
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"缓冲不足，请下载bpchost.txt查看");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		m_pDrebApi->GetLogPoint()->LogMp(LOG_ERROR,__FILE__,__LINE__,"MonitorHost 缓冲不足");
		xmlpack.ToFile("bpchost.txt");
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

void CDrebMsgThread::MonitorBpu(S_BPC_RSMSG &rcvdata)
{
	
	int connNum=0;
	CBF_Xml xmlpack;
	xmlpack.SetNodeValueByPath("Monitor/BPC公共节点",false,(int)m_pRes->g_nSvrMainId);
	xmlpack.SetNodeValueByPath("Monitor/BPC私有节点",false,(int)m_pRes->g_nSvrPrivateId);
	xmlpack.SetNodeValueByPath("Monitor/启动时间",false,m_pRes->g_sStartDateTime,false);
	//BPU信息
	
	CXmlNode *arbnode =  xmlpack.AddNodeByPath("Monitor",false,"ARB信息",false,"");
	
	if (arbnode != NULL)
	{
		std::vector<ARB_S_APPINFO> applist;
		m_tArb.GetAppList(applist);
		char tmpchar [500];
		for (int i=0; i< applist.size(); i++)
		{
			bzero(tmpchar,sizeof(tmpchar));
			sprintf(tmpchar,"app[%d %d] status[%d] uptime[%d]",applist[i].nSvrMainId,applist[i].cSvrPrivateId,applist[i].cStatus,\
				applist[i].nUpdateTime);
			CXmlNode *tmpnode = arbnode->AddChildNode("ARBLIST");
			if (tmpnode != NULL)
			{
				tmpnode->SetNodeValue(tmpchar);
			}
		}
	}

	
	int len = BPUDATASIZE;
	if (!xmlpack.ToBuffer(rcvdata.sMsgBuf->sBuffer,len))
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"缓冲不足，请下载bpcbpu.txt查看");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		m_pDrebApi->GetLogPoint()->LogMp(LOG_ERROR,__FILE__,__LINE__,"MonitorBpu 缓冲不足");
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

void CDrebMsgThread::MonitorReport(S_BPC_RSMSG &rcvdata)
{
	CBF_Xml xmlpack;
	xmlpack.SetNodeValueByPath("Monitor/BPC公共节点",false,(int)m_pRes->g_nSvrMainId);
	xmlpack.SetNodeValueByPath("Monitor/BPC私有节点",false,(int)m_pRes->g_nSvrPrivateId);
	xmlpack.SetNodeValueByPath("Monitor/启动时间",false,m_pRes->g_sStartDateTime,false);
	char tmpchar[100];
	char nodename[200];


	int len = BPUDATASIZE;
	if (!xmlpack.ToBuffer(rcvdata.sMsgBuf->sBuffer,len))
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"缓冲不足，请下载bpureport.txt查看");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		m_pDrebApi->GetLogPoint()->LogMp(LOG_ERROR,__FILE__,__LINE__,"MonitorReport 缓冲不足");
		xmlpack.ToFile("bpureport.txt");
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

void CDrebMsgThread::OnAppQueryHost(S_BPC_RSMSG &rcvdata)
{
	char errmsg[4096];
	bzero(errmsg,sizeof(errmsg));
	if (!m_pXdp.FromBuffer(rcvdata.sMsgBuf->sBuffer,rcvdata.sMsgBuf->sDBHead.nLen,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"应用查询主机状态报文不符，非XDP报文 %s",errmsg);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (m_pLog->isWrite(LOG_DEBUG+1))
	{
		m_pXdp.PrintXdp(errmsg);
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"应用查询主机状态报文 %s",errmsg);
	}
	unsigned int id;
	unsigned int pid;
	
	if (!m_pXdp.GetFieldValue("uint3",id,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"应用查询主机状态报文不符，无uint3字段 %s",errmsg);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	
	if (!m_pXdp.GetFieldValue("uint4",pid,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"应用查询主机状态报文不符，无uint4字段 %s",errmsg);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	
	m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"应用查询主机状态 app[%d %d]",id,pid);

	ARB_S_APPINFO *appinfo = m_tArb.UpdateArb(id,pid,ARBSTATUS_UNKNOW,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId);
	if (appinfo == NULL)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"UpdateArb出错");
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (!m_pXdp.SetFieldValue("uint9",(unsigned int)(appinfo->cStatus),errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xdp设置主机状态出错 %s",errmsg);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (!m_pXdp.SetFieldValue("uint10",(unsigned int)SUCCESS,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xdp设置返回码出错 %s",errmsg);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (m_pLog->isWrite(LOG_DEBUG+1))
	{
		m_pXdp.PrintXdp(errmsg);
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"应用查询主机状态应答报文 %s",errmsg);
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

void CDrebMsgThread::OnAppPushStatus(S_BPC_RSMSG &rcvdata)
{
	char errmsg[4096];
	bzero(errmsg,sizeof(errmsg));
	if (!m_pXdp.FromBuffer(rcvdata.sMsgBuf->sBuffer,rcvdata.sMsgBuf->sDBHead.nLen,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"应用推送状态报文不符，非XDP报文 %s",errmsg);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (m_pLog->isWrite(LOG_DEBUG+1))
	{
		m_pXdp.PrintXdp(errmsg);
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"应用推送状态报文 %s",errmsg);
	}
	unsigned int id=0;
	unsigned int pid=0;
	unsigned int status=0;
	
	if (!m_pXdp.GetFieldValue("uint3",id,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"应用推送状态报文不符，无uint3字段 %s",errmsg);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	
	if (!m_pXdp.GetFieldValue("uint4",pid,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"应用推送状态报文不符，无uint4字段 %s",errmsg);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	
	if (!m_pXdp.GetFieldValue("uint9",status,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"应用推送状态报文不符，无uint9字段 %s",errmsg);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	

	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"应用推送状态 app[%d %d] status[%d]",id,pid,status);
	ARB_S_APPINFO *appinfo = m_tArb.UpdateArb(id,pid,status,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId);
	if (appinfo == NULL)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"UpdateArb出错");
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
	rcvdata.sMsgBuf = NULL;
	return;
	
}

void CDrebMsgThread::OnCheck()
{
	m_bIsCheck = true;	
}

void CDrebMsgThread::CheckArb()
{
	//取所有的应用，按每种应用进行扫描
	std::vector<int> applist;
	if (!m_tArb.GetDistinctApp(applist))
	{
		m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"GetDistinctApp出错 无应用连接");
		return ;
	}
	std::vector<ARB_S_APPINFO *> appinfo;
	int i;
	
	for (i=0; i<applist.size(); i++)
	{
		appinfo.clear();
		if (!m_tArb.GetAppInfo(applist[i],appinfo))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"没有[%d]的应用信息",applist[i]);
			continue;
		}
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"检查 应用[%d]的主备状态  应用数[%d]",applist[i],appinfo.size());
		CheckArbByApp(appinfo);
	}

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

void CDrebMsgThread::CheckArbByApp(std::vector<ARB_S_APPINFO *> appinfo)
{
	if (appinfo.size()<1)
	{
		return;
	}
	if (time(NULL) - appinfo[0]->nLastSwtichTime <((CArbResource *)m_pRes)->m_nSwitchSeg)
	{
		//刚切换，不用检查了
		return;
	}
	bool isSwitch=false;//是否主备切换
	bool isMaster=false;//是否有主机
	bool isBackup=false;//是否有备机
	int  nMasterId=-1;//主机的ID
	int  nBackupId=-1;//备机的ID  需要切为主的备机ID
	//检查更新时间 是否要发查询
	
	unsigned int curtime = time(NULL);
	unsigned int utime=curtime;
	for (int j=0 ; j<appinfo.size() ; j++)
	{
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"APP[%d %d]状态[%d]",\
			appinfo[j]->nSvrMainId,appinfo[j]->cSvrPrivateId,appinfo[j]->cStatus);
		if (appinfo[j]->cStatus == ARBSTATUS_MASTER || appinfo[j]->cStatus == ARBSTATUS_MASTER_AYN)
		{
			if (nMasterId >=0)
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"APP[%d]有多个主机 [%d %d] [%d %d]",\
					appinfo[nMasterId]->nSvrMainId,appinfo[nMasterId]->nSvrMainId,appinfo[nMasterId]->cSvrPrivateId,\
					appinfo[j]->nSvrMainId,appinfo[j]->cSvrPrivateId);
				m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"APP[%d]有多个主机,将[%d %d] 置为备机",appinfo[j]->nSvrMainId,appinfo[j]->cSvrPrivateId);
				SwitchHost(appinfo[j]->nNodeId,appinfo[j]->nSvrMainId,appinfo[j]->cSvrPrivateId,0);
				SetSwitchTime(appinfo);
				continue ;
			}
			//主机
			isMaster = true;
			nMasterId = j;
			//检查主机的最后更新时间
			if (curtime - appinfo[j]->nUpdateTime > ((CArbResource *)m_pRes)->m_nSwitchTime)
			{
				isSwitch = true; //需要切换
				//在循环里找到最新更新时间的备机，进行切换
			}
			else  //还不用切换
			{
				if (((CArbResource *)m_pRes)->m_nSwitchTime>2)
				{
					if (curtime - appinfo[j]->nUpdateTime > ((CArbResource *)m_pRes)->m_nSwitchTime-2)
					{
						//需要发送查询了
						appinfo[j]->nQueryStatusTime = curtime;
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"APP [%d %d] 超时进行查询",\
							appinfo[j]->nSvrMainId,appinfo[j]->cSvrPrivateId);
						SendQueryHostStatus(appinfo[j]->nSvrMainId,appinfo[j]->cSvrPrivateId);
						
					}
				}
				//继续循环
				continue ;
			}
			
		}
		if (appinfo[j]->cStatus != ARBSTATUS_MASTER && appinfo[j]->cStatus != ARBSTATUS_MASTER_AYN)
		{
			
			//若2分钟未更新，则删除
			if (curtime - appinfo[j]->nUpdateTime > 60*2)
			{
				m_tArb.DeleteApp(appinfo[j]->nSvrMainId,appinfo[j]->cSvrPrivateId);
			}
			else
			{
				if (appinfo[j]->cStatus == ARBSTATUS_UNKNOW)
				{
					continue;
				}
				//前面没有备机，置为备
				if (!isBackup)
				{
					isBackup = true;
					nBackupId = j;
					utime = appinfo[j]->nAddTime;
				}
				//找出更新时间在m_nSwitchTime-2之内并且加入时间最早的一个
				if (curtime - appinfo[j]->nUpdateTime < ((CArbResource *)m_pRes)->m_nSwitchTime-2)
				{
					//发现有一个备机的加入时间更早，则置为此备机
					if (utime > appinfo[j]->nAddTime)
					{
						utime = appinfo[j]->nAddTime;
						nBackupId = j;
					}
				}
			}
			
		}
	}
	
	if (isSwitch) //需切换
	{
		if (isBackup)//有备机
		{
			appinfo[nMasterId]->cStatus = ARBSTATUS_UNKNOW;
			appinfo[nBackupId]->cStatus = ARBSTATUS_MASTER;
			m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"服务[%d %d]切为ARBSTATUS_UNKNOW 服务[%d %d]切为主",\
				appinfo[nMasterId]->nSvrMainId,appinfo[nMasterId]->cSvrPrivateId,\
				appinfo[nBackupId]->nSvrMainId,appinfo[nBackupId]->cSvrPrivateId);
			m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"发送主机切换 [%d %d->%d]",\
				appinfo[nMasterId]->nSvrMainId,appinfo[nMasterId]->cSvrPrivateId,appinfo[nBackupId]->cSvrPrivateId);
			SwitchHost(appinfo[nMasterId]->nNodeId,appinfo[nMasterId]->nSvrMainId,appinfo[nMasterId]->cSvrPrivateId,appinfo[nBackupId]->cSvrPrivateId);
			SetSwitchTime(appinfo);
		}
		else
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"APP[%d] 主机[%d %d]要进行切换为ARBSTATUS_UNKNOW，但无备机进行切换",\
				appinfo[nMasterId]->nSvrMainId,appinfo[nMasterId]->nSvrMainId,appinfo[nMasterId]->cSvrPrivateId);
			appinfo[nMasterId]->cStatus = ARBSTATUS_UNKNOW;//置为未知
			SwitchHost(appinfo[nMasterId]->nNodeId,appinfo[nMasterId]->nSvrMainId,appinfo[nMasterId]->cSvrPrivateId,0);
			SetSwitchTime(appinfo);
			return ;
		}
	}
	else
	{
		//没有主机
		if (!isMaster)
		{
			if (isBackup) //有备机
			{
				appinfo[nBackupId]->cStatus = ARBSTATUS_MASTER;
				m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"没有主机，切换 [%d %d]为主机",\
					appinfo[nBackupId]->nSvrMainId,appinfo[nBackupId]->cSvrPrivateId);
				SwitchHost(appinfo[nBackupId]->nNodeId,appinfo[nBackupId]->nSvrMainId,0,appinfo[nBackupId]->cSvrPrivateId);
				SetSwitchTime(appinfo);
			}
			
		}
	}
}

void CDrebMsgThread::SendQueryHostStatus(unsigned int svr, char pid)
{
	S_BPC_RSMSG data;
	data.sMsgBuf = (PBPCCOMMSTRU)m_pDrebApi->PoolMalloc();
	if (data.sMsgBuf == NULL)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送主机状态查询失败 PoolMalloc error");
		return;
	}
	char errmsg[250];
	bzero(errmsg,sizeof(errmsg));
	bzero(data.sMsgBuf,sizeof(BPCCOMMSTRU));
	data.sMsgBuf->sBpcHead.nIndex = 100;
	data.sMsgBuf->sDBHead.cCmd = CMD_DPCALL;
	data.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo = ARB_SEND_QUERY;
	data.sMsgBuf->sDBHead.d_Dinfo.d_nSvrMainId = svr;
	data.sMsgBuf->sDBHead.d_Dinfo.d_cSvrPrivateId = pid;
	m_pXdp.ResetData();
	if (!m_pXdp.SetFieldValue("uint3",svr,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP SetFieldValue uint3 error: %s",errmsg);
		m_pDrebApi->PoolFree(data.sMsgBuf);
		return;
	}
	if (!m_pXdp.SetFieldValue("uint4",(unsigned int)pid,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP SetFieldValue uint4 error: %s",errmsg);
		m_pDrebApi->PoolFree(data.sMsgBuf);
		return;
	}
	unsigned int len = BPUDATASIZE;
	if (!m_pXdp.ToBuffer(data.sMsgBuf->sBuffer,len,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP ToBuffer error: %s",errmsg);
		m_pDrebApi->PoolFree(data.sMsgBuf);
		return;
	}
	data.sMsgBuf->sDBHead.nLen = len;
	m_pDrebApi->SendMsg(data);
}

void CDrebMsgThread::SwitchHost(unsigned short drebid,unsigned int svr, char m2b, char b2m)
{
	//发给主机切换为备机
	S_BPC_RSMSG data;
	char errmsg[250];
	unsigned int len = BPUDATASIZE;
	if (m2b >0)
	{
		data.sMsgBuf = (PBPCCOMMSTRU)m_pDrebApi->PoolMalloc();
		if (data.sMsgBuf == NULL)
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送主机切换失败[%d %d->%d] PoolMalloc error",svr,m2b,b2m);
			return;
		}
		
		bzero(errmsg,sizeof(errmsg));
		bzero(data.sMsgBuf,sizeof(BPCCOMMSTRU));
		data.sMsgBuf->sBpcHead.nIndex = 100;
		data.sMsgBuf->sDBHead.cCmd = CMD_DPCALL;
		data.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo = ARB_SEND_CONTROL;
		data.sMsgBuf->sDBHead.d_Dinfo.d_nNodeId = drebid;
		data.sMsgBuf->sDBHead.d_Dinfo.d_nSvrMainId = svr;
		data.sMsgBuf->sDBHead.d_Dinfo.d_cSvrPrivateId = m2b;
		m_pXdp.ResetData();
		if (!m_pXdp.SetFieldValue("uint3",svr,errmsg))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP SetFieldValue uint3 error: %s",errmsg);
			m_pDrebApi->PoolFree(data.sMsgBuf);
			return;
		}
		if (!m_pXdp.SetFieldValue("uint4",(unsigned int)m2b,errmsg))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP SetFieldValue uint4 error: %s",errmsg);
			m_pDrebApi->PoolFree(data.sMsgBuf);
			return;
		}
		if (!m_pXdp.SetFieldValue("uint9",(unsigned int)ARBSTATUS_BACKUP,errmsg))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP SetFieldValue uint9 error: %s",errmsg);
			m_pDrebApi->PoolFree(data.sMsgBuf);
			return;
		}
		
		if (!m_pXdp.ToBuffer(data.sMsgBuf->sBuffer,len,errmsg))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP ToBuffer error: %s",errmsg);
			m_pDrebApi->PoolFree(data.sMsgBuf);
			return;
		}
		data.sMsgBuf->sDBHead.nLen = len;
		m_pDrebApi->SendMsg(data); //发给主机切换为备机

		SLEEP(100);
		//发给总线取消主机的交易注册
		data.sMsgBuf = (PBPCCOMMSTRU)m_pDrebApi->PoolMalloc();
		if (data.sMsgBuf == NULL)
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送主机切换失败[%d %d->%d] PoolMalloc error",svr,m2b,b2m);
			return;
		}
		bzero(data.sMsgBuf,sizeof(BPCCOMMSTRU));
		data.sMsgBuf->sBpcHead.nIndex = 100;
		data.sMsgBuf->sDBHead.cCmd = CMD_DPBC_UNREGTX;
		data.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo = ARB_SEND_UNREG_SVR;
		data.sMsgBuf->sDBHead.d_Dinfo.d_nNodeId = drebid;
		data.sMsgBuf->sDBHead.d_Dinfo.d_nSvrMainId = svr;
		data.sMsgBuf->sDBHead.d_Dinfo.d_cSvrPrivateId = m2b;
		data.sMsgBuf->sDBHead.nLen = 0;
		m_pDrebApi->SendMsg(data);//发给总线取消主机的交易注册

	}
	//发给备机切换为主机
	if (b2m<1)
	{
		m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"控制[%d %d]为备机，不用发给备机切换为主机",svr,m2b);
		return;
	}

	data.sMsgBuf = (PBPCCOMMSTRU)m_pDrebApi->PoolMalloc();
	if (data.sMsgBuf == NULL)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送主机切换失败[%d %d->%d] PoolMalloc error",svr,m2b,b2m);
		return;
	}
	bzero(data.sMsgBuf,sizeof(BPCCOMMSTRU));
	data.sMsgBuf->sBpcHead.nIndex = 100;
	data.sMsgBuf->sDBHead.cCmd = CMD_DPCALL;
	data.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo = ARB_SEND_CONTROL;
	data.sMsgBuf->sDBHead.d_Dinfo.d_nNodeId = drebid;
	data.sMsgBuf->sDBHead.d_Dinfo.d_nSvrMainId = svr;
	data.sMsgBuf->sDBHead.d_Dinfo.d_cSvrPrivateId = b2m;
	m_pXdp.ResetData();
	if (!m_pXdp.SetFieldValue("uint3",svr,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP SetFieldValue uint3 error: %s",errmsg);
		m_pDrebApi->PoolFree(data.sMsgBuf);
		return;
	}
	if (!m_pXdp.SetFieldValue("uint4",(unsigned int)b2m,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP SetFieldValue uint4 error: %s",errmsg);
		m_pDrebApi->PoolFree(data.sMsgBuf);
		return;
	}
	if (!m_pXdp.SetFieldValue("uint9",(unsigned int)ARBSTATUS_MASTER,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP SetFieldValue uint9 error: %s",errmsg);
		m_pDrebApi->PoolFree(data.sMsgBuf);
		return;
	}
	len = BPUDATASIZE;
	if (!m_pXdp.ToBuffer(data.sMsgBuf->sBuffer,len,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP ToBuffer error: %s",errmsg);
		m_pDrebApi->PoolFree(data.sMsgBuf);
		return;
	}
	data.sMsgBuf->sDBHead.nLen = len;
	m_pDrebApi->SendMsg(data);//发给备机切换为主机
	return;
}

void CDrebMsgThread::SetSwitchTime(std::vector<ARB_S_APPINFO *> &appinfo)
{
	unsigned int curtime = time(NULL);
	for (int j=0 ; j<appinfo.size() ; j++)
	{
		appinfo[j]->nLastSwtichTime = curtime;
	}
}
