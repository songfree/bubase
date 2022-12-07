// MsgProcThread.cpp: implementation of the CMsgProcThread class.
//
//////////////////////////////////////////////////////////////////////

#include "MsgProcThread.h"
#include "GlobalVar.h"
#include "SocketMgr.h"
#include "BF_BufferPool.h"
#include "MemDB.h"
#include "dreberrcode.h"
#include "Dreb_Timer.h"

extern char g_VERSION[20];
extern CDreb_Timer g_pDrebTimer;

#ifdef _WINDOWS
#define   INT64PFM  "%I64d"
#else
#define   INT64PFM  "%lld"
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



CMsgProcThread::CMsgProcThread()
{
	memset(m_errMsg,0,sizeof(m_errMsg));
	m_pRes=NULL;
	m_pMemDb=NULL;
	m_pSocketMgr=NULL;
	m_pMemPool=NULL;
	m_pMsgQueue = NULL;
//	memset(&m_pRsMsgHead,0,sizeof(DREB_HEAD));
	m_nIndex =0;
}

CMsgProcThread::~CMsgProcThread()
{
	m_pRes=NULL;
	m_pMemDb=NULL;
	m_pSocketMgr=NULL;
	m_pMemPool=NULL;
	m_pMsgQueue = NULL;
}
void CMsgProcThread::ExitThreadInstance()
{
	return ;
}
bool CMsgProcThread::InitThreadInstance()
{
	return true;
}
int CMsgProcThread::Run()
{
	S_DREB_RSMSG *data=NULL;
	m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"启动消息处理线程 index[%d] g_bIsExit[%d]",m_nIndex,m_pRes->g_bIsExit);
	while (!m_pRes->g_bIsExit)
	{
		data = m_pMsgQueue->GetData();
		if (data != NULL)
		{
			Run(data);
		}
	}
	m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"停止消息处理线程 index[%d] g_bIsExit[%d]",m_nIndex,m_pRes->g_bIsExit);
	return 0;
}
std::string CMsgProcThread::GetDrebCmdType(int cmdtype)
{
	switch (cmdtype)
	{
		
	case  CMD_ROUTER:      //1  //数据总线节点发送路由通知的命令字
		return "CMD_ROUTER";
	case  CMD_DEL_NODE:    //2  //数据总线节点之间取消数据总线节点的命令字,只发送至主动连接的数据总线节点
		return "CMD_DEL_NODE";
	case  CMD_DEL_SVR:     //3  //服务端取消注册服务的命令字
		return "CMD_DEL_SVR";
	case  CMD_PING:        //4  //心跳请求的命令字
		return "CMD_PING";
	case  CMD_CONNECT:     //5  //连接注册
		return "CMD_CONNECT";
	case  CMD_SERVICE:     //6  //服务路由
		return "CMD_SERVICE";
	case  CMD_REGSERVICE:  //7  //注册服务
		return "CMD_REGSERVICE";			
	case  CMD_DPCALL:      //8  //数据总线节点同步调用 要求最终处理完成后应答
		return "CMD_DPCALL";
	case  CMD_DPACALL:     //9  //数据总线节点异步调用 要求接收到的数据总线节点发送至服务后确认应答
		return "CMD_DPACALL";
	case  CMD_DPBC:        //10  //数据总线节点广播，即将信息发给指定数据总线节点上所有注册的服务,要求数据总线节点应答
		return "CMD_DPBC";
	case  CMD_DPABC:       //11  //数据总线节点广播，即将信息发给指定数据总线节点上所有注册的服务,不要求数据总线节点应答
		return "CMD_DPABC";			
	case  CMD_DPPUSH:      //12 //数据总线节点推送，无须应答
		return "CMD_DPPUSH";
	case  CMD_DPPOST:      //13 //数据总线节点投递，要求接收到的数据总线节点应答
		return "CMD_DPPOST";
	case  CMD_UNREGSERVICE:     //14 //取消交易注册
		return "CMD_UNREGSERVICE";
	case  CMD_MONITOR_DREB:     //15 //数据总线节点监控
		return "CMD_MONITOR_DREB";
	case  CMD_MONITOR_BPC:     //16 //BPC节点监控
		return "CMD_MONITOR_BPC";
	case  CMD_DPBC_UNREGTX:     //17 //取消服务的交易注册广播
		return "CMD_DPBC_UNREGTX";
	default:
		return "";
	}
}
std::string CMsgProcThread::GetDrebSockType(int socktype)
{
	switch (socktype)
	{
		case  SOCK_CLI:      //2  //客户端
			return "SOCK_CLI";
		case SOCK_DREB:     //3      //其它数据总线节点到本数据总线节点的连接
			return "SOCK_DREB";
		case SOCK_SRV:      //4      //注册的服务端
			return "SOCK_SRV";
		case SOCK_TODREB:   //5      //和其它数据总线节点节点的连接
			return "SOCK_TODREB";
		case SOCK_SLOT:	  //6      //注册的通讯槽
			return "SOCK_SLOT";
		case SOCK_LISTEN:	  //7      //侦听端口
			return "SOCK_LISTEN";
		default:
			return "";
	}
}

std::string CMsgProcThread::GetIndexFlag(int index)
{
	if (!m_pSocketMgr->AffirmIndex(index))
	{
		return "";
	}
	char tmpchar[40];
	switch (m_pSocketMgr->at(index)->m_nType)
	{
		case SOCK_SRV:
			sprintf(tmpchar,"SVR=%d,%d",m_pSocketMgr->at(index)->m_s_srv.nSvrMainId,m_pSocketMgr->at(index)->m_s_srv.cSvrPrivateId);
			return tmpchar;
		case SOCK_TODREB:
			sprintf(tmpchar,"DREB=%d,%d",m_pSocketMgr->at(index)->m_s_center.nNodeId,m_pSocketMgr->at(index)->m_s_center.cNodePrivateId);
			return tmpchar;
		case SOCK_DREB:
			sprintf(tmpchar,"DREB=%d,%d",m_pSocketMgr->at(index)->m_s_conn.nNodeId,m_pSocketMgr->at(index)->m_s_conn.cNodePrivateId);
			return tmpchar;
		default:
			return "";
	}
}

int CMsgProcThread::Run(S_DREB_RSMSG *msg)
{
	//处理消息
	switch (msg->msghead.msgtype)
	{
		case MSG_READ://从socket收到的数据，根据命令字分别进行处理
			m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"处理一条MSG_READ消息");
			OnMsgRead(msg);
			break;
		case MSG_CLOSE://连接断开的消息
			m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"处理一条MSG_CLOSE消息");
			OnMsgClose(msg);
			break;
		case MSG_ROUTE://发送dreb节点路由的消息
			m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"处理一条MSG_ROUTE消息");
			OnMsgRoute(msg);
			break;
		case MSG_SERVICE://发送交易路由的消息
			m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"处理一条MSG_SERVICE消息");
			OnMsgServiceRoute(msg);
			break;
		case MSG_MONITOR://发送监控信息的消息
			m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"处理一条MSG_MONITOR消息");
			OnMsgMonitor(msg);
			break;
		default:
			m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"消息命令字不符 %d",msg->msghead.msgtype);
			m_pMemPool->PoolFree(msg);
			break;
	}
	return 0;
}


void CMsgProcThread::OnMsgClose(S_DREB_RSMSG *msg)
{
	if (m_pSocketMgr->at(msg->msghead.index)->m_nConntime != msg->msghead.connecttime)
	{
		m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"关闭连接标识不符 index=%d %d!=%d",msg->msghead.index,msg->msghead.connecttime,m_pSocketMgr->at(msg->msghead.index)->m_nConntime);
		m_pMemPool->PoolFree(msg);
		return;
	}
	switch (m_pSocketMgr->at(msg->msghead.index)->m_nType)
	{
		case SOCK_SRV:
			//关闭服务
			if (!m_pMemDb->CloseSvr(m_pSocketMgr->at(msg->msghead.index)->m_s_srv.nSvrMainId,m_pSocketMgr->at(msg->msghead.index)->m_s_srv.cSvrPrivateId))
			{
				m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"关闭时服务[%d %d]不存在",\
					m_pSocketMgr->at(msg->msghead.index)->m_s_srv.nSvrMainId,\
					m_pSocketMgr->at(msg->msghead.index)->m_s_srv.cSvrPrivateId);
			}
			//取消本地服务功能表
			m_pMemDb->UnRegisterService(m_pRes->g_nDrebId,m_pRes->g_nDrebPrivateId,m_pSocketMgr->at(msg->msghead.index)->m_s_srv.nSvrMainId,m_pSocketMgr->at(msg->msghead.index)->m_s_srv.cSvrPrivateId);
			//取消广播订阅
			m_pMemDb->m_subscribe.UnSubscribe(msg->msghead.index);
			m_pSocketMgr->at(msg->msghead.index)->OnClose("断开连接",__FILE__,__LINE__);
			if (m_pRes->g_nRegOtherDreb == 1) //向其它总线注册
			{
				OnMsgServiceRoute(msg);
			}
			return;
		case SOCK_TODREB:
			if (!m_pMemDb->CloseDreb(m_pSocketMgr->at(msg->msghead.index)->m_s_center.nNodeId,m_pSocketMgr->at(msg->msghead.index)->m_s_center.cNodePrivateId,msg->msghead.index))
			{
				m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"关闭本端主动连接的数据总线节点[%d %d]不存在",\
					m_pSocketMgr->at(msg->msghead.index)->m_s_center.nNodeId,\
					m_pSocketMgr->at(msg->msghead.index)->m_s_center.cNodePrivateId);
			}
			//删除此连接过来的交易路由
			m_pMemDb->DeleteServiceByIndex(msg->msghead.index);	
			m_pSocketMgr->at(msg->msghead.index)->OnClose("断开连接",__FILE__,__LINE__);
			OnMsgRoute(msg);
			return;
		case SOCK_DREB:
			if (!m_pMemDb->CloseDreb(m_pSocketMgr->at(msg->msghead.index)->m_s_conn.nNodeId,m_pSocketMgr->at(msg->msghead.index)->m_s_conn.cNodePrivateId,msg->msghead.index))
			{
				m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"关闭对端主动连接的数据总线节点[%d %d]不存在",\
					m_pSocketMgr->at(msg->msghead.index)->m_s_center.nNodeId,\
					m_pSocketMgr->at(msg->msghead.index)->m_s_center.cNodePrivateId);
			}
			//删除此连接过来的交易路由
			m_pMemDb->DeleteServiceByIndex(msg->msghead.index);
			m_pSocketMgr->at(msg->msghead.index)->OnClose("断开连接",__FILE__,__LINE__);
			OnMsgRoute(msg);
			return;
		case SOCK_LISTEN:
			break;
		default:
			m_pSocketMgr->at(msg->msghead.index)->OnClose("断开连接",__FILE__,__LINE__);
			break;

	}
	m_pMemPool->PoolFree(msg);
	return;
}


void CMsgProcThread::OnMsgRoute(S_DREB_RSMSG *msg)
{
	S_DREB_ROUTE *data=NULL;
	std::vector<S_DREB_ROUTE *>rtlist;
	std::vector<S_DREB_ROUTE *> dp;

	std::string msginfo;
	char tmpchar[500];
	char msghead[500];
	//取所有的连接的DREB信息，不含本节点
	m_pMemDb->GetAllDreb(dp);
	if (dp.size()<1)
	{
		m_pMemPool->PoolFree(msg);
		m_log->LogMp(LOG_PROMPT,__FILE__,__LINE__,"无额外的总线连接");
		return;
	}
	m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"主动发送ROUTE");
	S_DREB_RSMSG *rsmsg = NULL;
	for (unsigned int i=0;i<dp.size();i++)
	{
		data = dp[i];
		if (!data->bIsClose)  //连接未关闭
		{
			rtlist.clear();
			//根据dreb连接的索引取得要到这个连接上发送的路由信息
			m_pMemDb->GetRouteByIndex(data->nIndex,rtlist);
			if (m_pRes->g_nPrintRoute == 1)
			{
				unsigned short nodeid;
				int   privateid;
				if (m_pSocketMgr->at(dp[i]->nIndex)->m_nType == SOCK_TODREB) //本dreb主动连接的
				{
					nodeid = m_pSocketMgr->at(dp[i]->nIndex)->m_s_center.nNodeId;
					privateid = m_pSocketMgr->at(dp[i]->nIndex)->m_s_center.cNodePrivateId;
				}
				else //对方dreb连接过来的
				{
					nodeid = m_pSocketMgr->at(dp[i]->nIndex)->m_s_conn.nNodeId;
					privateid = m_pSocketMgr->at(dp[i]->nIndex)->m_s_conn.cNodePrivateId;
				}
				msginfo="";
				
				bzero(tmpchar,sizeof(tmpchar));
				
				bzero(msghead,sizeof(msghead));
				sprintf(msghead,"发给index[%d] dreb[%d %d]的节点路由信息",dp[i]->nIndex,nodeid,privateid);
				for (unsigned int j=0;j<rtlist.size();j++)
				{
					sprintf(tmpchar,"\n   DREB[%d %d] STEP[%d] ",\
						rtlist[j]->nNodeId,rtlist[j]->cNodePrivateId,\
						rtlist[j]->nStep);
					msginfo = msginfo + tmpchar;
				}
				msginfo = msginfo + "\n";
				m_pRes->g_pRouteLog.LogBin(LOG_INFO,__FILE__,__LINE__,msghead,(char *)msginfo.c_str(),msginfo.length());
			}
			
			if (rtlist.size()>0)
			{
				//处理路由发送
				rsmsg = (S_DREB_RSMSG *) m_pMemPool->PoolMalloc();
				if (rsmsg == NULL)
				{
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"内存不足，分配数据总线节点数据空间出错");
					m_pMemPool->PoolFree(msg);
					return;
				}
				rsmsg->msghead.connecttime = m_pSocketMgr->at(data->nIndex)->m_nConntime;
				rsmsg->msghead.index = data->nIndex;
				bzero(&(rsmsg->message.head),sizeof(DREB_HEAD));
				rsmsg->message.head.cRaflag = 0;
				rsmsg->message.head.cNextFlag = 0;
				rsmsg->message.head.cZip = 0;
				rsmsg->message.head.nLen = 0;
				rsmsg->message.head.cCmd = CMD_ROUTER;
				rsmsg->message.head.s_Sinfo.s_nIndex = data->nIndex;
				rsmsg->message.head.s_Sinfo.s_cNodePrivateId = m_pRes->g_nDrebPrivateId;	
				rsmsg->message.head.s_Sinfo.s_nNodeId = m_pRes->g_nDrebId;
				rsmsg->message.head.s_Sinfo.s_nTimestamp = m_pSocketMgr->at(data->nIndex)->m_nConntime;
				if (rtlist.size()*sizeof(S_ROUTE_MSG)> DREBDATASIZE)
				{
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"路由表太大，程序出错");
					m_pMemPool->PoolFree(rsmsg);
					m_pMemPool->PoolFree(msg);
					return;
				}
				rsmsg->message.head.nLen = rtlist.size()*sizeof(S_ROUTE_MSG); 
				for (unsigned k=0;k<rtlist.size();k++)
				{
					S_ROUTE_MSG *tbl = (S_ROUTE_MSG *)(rsmsg->message.buffer+k*sizeof(S_ROUTE_MSG));
					tbl->nNodeId = rtlist[k]->nNodeId;
					m_pDrebEndian.Endian2Comm((unsigned char *)&(tbl->nNodeId),sizeof(tbl->nNodeId));
					tbl->cNodePrivateId = rtlist[k]->cNodePrivateId;
					tbl->nStep = rtlist[k]->nStep;
					tbl->nBandWidth = rtlist[k]->nbandwidth;
					m_pDrebEndian.Endian2Comm((unsigned char *)&(tbl->nBandWidth),sizeof(tbl->nBandWidth));
					m_log->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"发送给index=%d nNodeId=%d cNodePrivateId=%d的路由nNodeId=%d cNodePrivateId=%d nStep=%d",\
						data->nIndex,data->nNodeId,data->cNodePrivateId,tbl->nNodeId,tbl->cNodePrivateId,tbl->nStep);
				}
				if (m_pSocketMgr->at(data->nIndex)->SendMsg(rsmsg)!=0)
				{
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送CMD_ROUTER出错 %d",data->nIndex);
				}
				
			}
			else //没有数据也要发送一个空的
			{
				//处理路由发送
				rsmsg = (S_DREB_RSMSG *) m_pMemPool->PoolMalloc();
				if (rsmsg == NULL)
				{
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"内存不足，分配数据总线节点数据空间出错");
					m_pMemPool->PoolFree(msg);
					return;
				}
				bzero(&(rsmsg->message.head), sizeof(DREB_HEAD));
				rsmsg->message.head.cRaflag = 0;
				rsmsg->message.head.cZip = 0;
				rsmsg->message.head.cNextFlag = 0;
				rsmsg->message.head.nLen = 0;
				rsmsg->message.head.cCmd = CMD_ROUTER;
				rsmsg->message.head.s_Sinfo.s_nIndex = data->nIndex;
				rsmsg->message.head.s_Sinfo.s_cNodePrivateId = m_pRes->g_nDrebPrivateId;	
				rsmsg->message.head.s_Sinfo.s_nNodeId = m_pRes->g_nDrebId;
				rsmsg->message.head.s_Sinfo.s_nTimestamp = m_pSocketMgr->at(data->nIndex)->m_nConntime;
				if (m_pSocketMgr->at(data->nIndex)->SendMsg(rsmsg)!=0)
				{
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送CMD_ROUTER出错 %d",data->nIndex);
				}
			}
		} //end if (!data.bIsClose)
	}
	m_pMemPool->PoolFree(msg);
	return ;
}

void CMsgProcThread::OnMsgServiceRoute(S_DREB_RSMSG *msg)
{
	std::vector<S_SERVICE_ROUTE *> rtlist;
	std::vector<S_DREB_ROUTE *> dp;
	//取所有的DREB信息，包括本端
	m_pMemDb->GetAllDreb(dp);
	if (dp.size()<1)
	{
		m_pMemPool->PoolFree(msg);
		m_log->LogMp(LOG_PROMPT,__FILE__,__LINE__,"无额外的总线连接");
		return;
	}
	
	unsigned short nodeid;
	int   privateid;
	std::string msginfo;
	char tmpchar[500];
	char msghead[500];


	int index;
	unsigned short wnodeid;
	int wpid;
	unsigned int wsvr;
	int wpsvr;
	for (unsigned int i=0;i<dp.size();i++)
	{
		if (!dp[i]->bIsClose)
		{
			rtlist.clear();
			//为了减少数据量，把连接对应的dreb从列表中去掉
			if (m_pSocketMgr->at(dp[i]->nIndex)->m_nType == SOCK_TODREB) //本dreb主动连接的
			{
				nodeid = m_pSocketMgr->at(dp[i]->nIndex)->m_s_center.nNodeId;
				privateid = m_pSocketMgr->at(dp[i]->nIndex)->m_s_center.cNodePrivateId;
			}
			else //对方dreb连接过来的
			{
				nodeid = m_pSocketMgr->at(dp[i]->nIndex)->m_s_conn.nNodeId;
				privateid = m_pSocketMgr->at(dp[i]->nIndex)->m_s_conn.cNodePrivateId;
			}
			if (!m_pMemDb->GetRouteServiceByIndex(nodeid,privateid,dp[i]->nIndex,rtlist))
			{
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"GetRouteServiceByIndex出错");
				continue;
			}
			if (m_pRes->g_nPrintRoute == 1)
			{
				msginfo="";
				bzero(tmpchar,sizeof(tmpchar));
				bzero(msghead,sizeof(msghead));
				sprintf(msghead,"发给index[%d] dreb[%d %d]的交易路由信息",dp[i]->nIndex,nodeid,privateid);
				index=-1;
				wnodeid=0;
				wpid=-1;
				wsvr=0;
				wpsvr=-1;
				for (unsigned int j=0;j<rtlist.size();j++)
				{
					if (index != rtlist[j]->nIndex || wnodeid!= rtlist[j]->nNodeId ||\
						wpid!=rtlist[j]->cNodePrivateId || wsvr != rtlist[j]->nSvrMainId ||\
						wpsvr != rtlist[j]->cSvrPrivateId)
					{
						index = rtlist[j]->nIndex;
						wnodeid = rtlist[j]->nNodeId;
						wpid =rtlist[j]->cNodePrivateId;
						wsvr = rtlist[j]->nSvrMainId;
						wpsvr = rtlist[j]->cSvrPrivateId;
						sprintf(tmpchar,"\n   INDEX[%d] DREB[%d %d] SVR[%d %d] STEP[%d] FUNCNO=",\
							index,rtlist[j]->nNodeId,rtlist[j]->cNodePrivateId,rtlist[j]->nSvrMainId,\
							rtlist[j]->cSvrPrivateId,rtlist[j]->nStep);
						msginfo = msginfo + tmpchar;
					}
					sprintf(tmpchar," %d",rtlist[j]->nFuncNo);
					msginfo = msginfo + tmpchar;
					
				}
				msginfo = msginfo + "\n";
				m_pRes->g_pRouteLog.LogBin(LOG_INFO,__FILE__,__LINE__,msghead,(char *)msginfo.c_str(),msginfo.length());
			}
			m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"主动发送SERVICE ROUTE index[%d] 交易数[%d]",dp[i]->nIndex,rtlist.size());
			SendService(dp[i]->nIndex,rtlist);
		}
	}
	m_pMemPool->PoolFree(msg);
	return;
}
void CMsgProcThread::OnMsgRead(S_DREB_RSMSG *msg)
{
	
	char cmd = msg->message.head.cCmd;
	int index;
	unsigned short s_nNodeId;
	char s_cNodePrivateId;
	if (m_log->isWrite(LOG_DEBUG))
    {
		index = msg->msghead.index;
        s_nNodeId = msg->message.head.s_Sinfo.s_nNodeId;
		s_cNodePrivateId = msg->message.head.s_Sinfo.s_cNodePrivateId;
		m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"数据总线节点命令[%s] index[%d] 来自DREB[%d %d]",\
			GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->msghead.index,s_nNodeId,\
			s_cNodePrivateId);
	}
	
	switch (msg->message.head.cCmd)
	{
		case CMD_PING:  //心跳
			AnsMsg(msg,SUCCESS,"应答心跳"); 
			break;
		case CMD_CONNECT:  //连接
			OnCmdConnect(msg); //处理连接的认证等
			break;
		case CMD_ROUTER:  //路由通知
			OnCmdRoute(msg);
			break;
		case CMD_SERVICE:  //服务功能通知
			OnCmdService(msg);
			break;	
		case CMD_REGSERVICE:
			OnCmdServiceReg(msg);//服务功能号注册
			break;
		case CMD_UNREGSERVICE:
			OnCmdUnServiceReg(msg);//服务功能号取消注册
			break;
		case CMD_DEL_NODE: //通讯节点取消
			OnCmdDelNode(msg);
			break;
		case CMD_DEL_SVR:  //服务注销
			OnCmdDelSvr(msg);
			break;
		case CMD_DPCALL:   //同步调用 最终服务应答
			OnCmdDpCall(msg);
			break;
		case CMD_DPACALL:  //异步调用，数据总线节点应答
			OnCmdDpACall(msg);	
			break;
		case CMD_DPBC:     //广播有应答
			OnCmdDpBc(msg);
			break;
		case CMD_DPABC:    //广播无应答
			OnCmdDpABc(msg);
			break;
		case CMD_DPPOST:    //投递，数据总线节点应答
			OnCmdDpPost(msg);
			break;
		case CMD_DPPUSH:    //推送，无应答
			OnCmdDpPush(msg);
			break;
		case CMD_MONITOR_DREB://DREB监控功能
			OnCmdMonitorDreb(msg);
			break;
		case CMD_MONITOR_BPC://BPC监控功能
			OnCmdMonitorBpc(msg);
			break;
		case CMD_DPBC_UNREGTX://广播取消服务的交易注册
			OnCmdUnReqTx(msg);
			break;
		case CMD_SUBSCRIBE:		//服务订阅广播
			OnCmdSubscribe(msg);
			break;
        case CMD_UNSUBSCRIBE:		//服务取消订阅广播
            OnCmdUnSubscribe(msg);
			break;
		default:
			m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"数据总线节点命令字不符 %d index[%d] 来自DREB[%d %d]",\
				msg->message.head.cCmd,msg->msghead.index,msg->message.head.s_Sinfo.s_nNodeId,\
				msg->message.head.s_Sinfo.s_cNodePrivateId);
			m_pMemPool->PoolFree(msg);
			return;
	}
	if (m_log->isWrite(LOG_DEBUG))
	{
		m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"数据总线节点命令[%s] 处理完成 index[%d] 来自DREB[%d %d ]",\
			GetDrebCmdType(cmd).c_str(),msg->msghead.index,s_nNodeId,\
			s_cNodePrivateId);
	}
	return ;
}


void CMsgProcThread::AcceptClient(S_DREB_RSMSG *msg)
{
	m_pSocketMgr->at(msg->msghead.index)->SetClient();
	//应答回去
	AnsConnect(msg,SUCCESS,"客户端连接");
}

void CMsgProcThread::AcceptSvr(S_DREB_RSMSG *msg)
{
	int index = msg->msghead.index;
	S_CONNECT_DATA *conData = (S_CONNECT_DATA *)msg->message.buffer;
	if (conData->nSvrMainId<1)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"index[%d] 服务号%d不符",msg->msghead.index,conData->nSvrMainId);
		AnsConnect(msg,ERR_SVRID,"服务号不符");
		m_pSocketMgr->at(index)->OnClose("服务号不符",__FILE__,__LINE__);
		return;
	}
	if (conData->cSvrPrivateId<1)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"index[%d] 服务私有号%d不符",msg->msghead.index,conData->cSvrPrivateId);
		AnsConnect(msg,ERR_PRIVATEID,"服务私有号不符");
		m_pSocketMgr->at(index)->OnClose("服务私有号不符",__FILE__,__LINE__);
		return;
	}
	CSendQueue *psque=NULL;
	//注册服务
	if (!m_pMemDb->RegisterSvr(conData->nSvrMainId,conData->cSvrPrivateId,msg->msghead.index,&psque))
	{
		sprintf(m_errMsg,"index[%d] 服务[%d %d]已注册",msg->msghead.index,conData->nSvrMainId,conData->cSvrPrivateId);
		AnsConnect(msg,ERR_REGISTER,m_errMsg);
		m_pSocketMgr->at(index)->OnClose(m_errMsg,__FILE__,__LINE__);
		return;
	}
	m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"index[%d] 服务[%d %d]对应的队列地址[%d]",\
		msg->msghead.index,conData->nSvrMainId,conData->cSvrPrivateId,psque);

	m_pSocketMgr->at(msg->msghead.index)->SetSvr(conData->nSvrMainId,conData->cSvrPrivateId,psque);
	//20130304 songfree add 服务连接时，将服务加入到交易路由表，供按服务路由，交易码默认为0
	S_SERVICE_ROUTE rt;
	bzero(&rt,sizeof(S_SERVICE_ROUTE));
	rt.cNodePrivateId = m_pRes->g_nDrebPrivateId;
	rt.cSvrPrivateId = conData->cSvrPrivateId;
	rt.nFuncNo = 0;
	//nIndex为服务在总线上连接的index
	rt.nIndex = msg->msghead.index;
	rt.nNodeId = m_pRes->g_nDrebId;
	rt.nStep = 0;
	rt.nSvrMainId = conData->nSvrMainId;
	rt.nUpdateTime = time(NULL);
	m_pMemDb->AddServiceRoute(rt);
	//ADD END

	m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"index[%d] 接受服务节点[%d %d]连接成功",\
		msg->msghead.index,conData->nSvrMainId,conData->cSvrPrivateId);
	AnsConnect(msg,SUCCESS,"服务端连接");
	return;

}

void CMsgProcThread::AcceptDreb(S_DREB_RSMSG *msg)
{
	int index = msg->msghead.index;
	S_CONNECT_DATA *conData = (S_CONNECT_DATA *)msg->message.buffer;
	if (conData->nNodeId<1)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"index[%d] 数据总线节点ID[%d]不符",msg->msghead.index,conData->nNodeId);
		AnsConnect(msg,ERR_DREBID,"数据总线节点ID不符");
		m_pSocketMgr->at(index)->OnClose("数据总线节点ID不符",__FILE__,__LINE__);
		return;
	}
	if (conData->cNodePrivateId<1)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"index[%d] 数据总线节点私有ID[%d]不符",msg->msghead.index,conData->cNodePrivateId);
		AnsConnect(msg,ERR_DREBPRIVATEID,"数据总线节点ID不符");
		m_pSocketMgr->at(index)->OnClose("数据总线节点私有ID不符",__FILE__,__LINE__);
		return;
	}
	if (m_pRes->g_nDrebId == conData->nNodeId  && m_pRes->g_nDrebPrivateId == conData->cNodePrivateId )
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"index[%d] 要注册的DREB节点和本节点相同[%d %d]",\
			msg->msghead.index,conData->nNodeId,conData->cNodePrivateId);
		AnsConnect(msg,ERR_DREBNODE,"要注册的DREB节点和本节点相同");
		m_pSocketMgr->at(index)->OnClose("要注册的DREB节点和本节点相同",__FILE__,__LINE__);
		return;
	}
	CSendQueue *psque;
	if (!m_pMemDb->RegisterDreb(conData->nNodeId,conData->cNodePrivateId,index,conData->nBandWidth,&psque))
	{
		sprintf(m_errMsg,"index[%d] 数据总线节点[%d %d]已注册",msg->msghead.index,conData->nNodeId,conData->cNodePrivateId);
		AnsConnect(msg,ERR_DREBREGISTER,m_errMsg);
		m_pSocketMgr->at(index)->OnClose(m_errMsg,__FILE__,__LINE__);
		return;
	}
	
	m_pSocketMgr->at(index)->SetDreb(conData->nNodeId,conData->cNodePrivateId,conData->nBandWidth,psque);
	m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"index[%d] 接受数据总线节点nodeid=%d privateid=%d index=%d 连接成功",\
		msg->msghead.index,conData->nNodeId,conData->cNodePrivateId,index);
	AnsConnect(msg,SUCCESS,"数据总线节点连接成功");	
	return;
}

void CMsgProcThread::OnCmdConnect(S_DREB_RSMSG *msg)
{
	int index = msg->msghead.index;
	if (m_pSocketMgr->at(index)->m_bChecked)
	{
		m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"连接已确认");
		m_pMemPool->PoolFree(msg);
		return;
	}

	if (msg->message.head.cRaflag == 0) //客户端、服务端、通讯节点发来连接请求
	{
		if (msg->message.head.nLen < sizeof(S_CONNECT_DATA))
		{
			m_pSocketMgr->at(index)->OnClose("连接消息长度不符",__FILE__,__LINE__);
			m_pMemPool->PoolFree(msg);
			return ;
		}
		S_CONNECT_DATA *conData = (S_CONNECT_DATA *)(msg->message.buffer);
		m_pDrebEndian.Endian2Comm((unsigned char *)&(conData->nNodeId),sizeof(conData->nNodeId));
		m_pDrebEndian.Endian2Comm((unsigned char *)&(conData->nSvrMainId),sizeof(conData->nSvrMainId));
		switch (conData->cConnectType)
		{
			case 0://客户端
				AcceptClient(msg);
				break;
			case 1: //服务端
				AcceptSvr(msg);
				break;
			case 2://数据总线节点
				AcceptDreb(msg);
				break;
			default:
				m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"index[%d] 连接类型错误 %d",msg->msghead.index,conData->cConnectType);
				AnsConnect(msg,ERR_VERSION,"连接类型错误");
				m_pSocketMgr->at(index)->OnClose("连接类型错误",__FILE__,__LINE__);
				break;	
		}
		return;
	}
	else //应答,主动连接的
	{
		if (m_pSocketMgr->at(index)->m_nType != SOCK_TODREB)
		{
			m_log->LogMp(LOG_PROMPT,__FILE__,__LINE__,"index[%d] 对端[%d]的应答，丢弃",\
				msg->msghead.index,m_pSocketMgr->at(index)->m_nType);
			m_pMemPool->PoolFree(msg);
			return;
		}
		if (msg->message.head.a_Ainfo.a_nRetCode != SUCCESS)
		{
			m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"index[%d] 对端dreb返回 %d",msg->msghead.index,msg->message.head.a_Ainfo.a_nRetCode);
			m_pSocketMgr->at(index)->OnClose("对端DREB拒绝连接",__FILE__,__LINE__);
			m_pMemPool->PoolFree(msg);
			return;
		}
		CSendQueue *psque;
		if (!m_pMemDb->RegisterDreb(msg->message.head.a_Ainfo.a_nNodeId,msg->message.head.a_Ainfo.a_cNodePrivateId,msg->msghead.index,m_pSocketMgr->at(msg->msghead.index)->m_s_center.nbandwidth,&psque))
		{
			m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"index[%d] 注册数据总线节点失败[%d %d]",\
				msg->msghead.index,msg->message.head.a_Ainfo.a_nNodeId,msg->message.head.a_Ainfo.a_cNodePrivateId);
			m_pSocketMgr->at(index)->OnClose("注册数据总线节点失败",__FILE__,__LINE__);
			m_pMemPool->PoolFree(msg);
			return;
		}
		m_pSocketMgr->at(index)->m_s_center.cNodePrivateId = msg->message.head.a_Ainfo.a_cNodePrivateId;
		m_pSocketMgr->at(index)->m_s_center.nNodeId = msg->message.head.a_Ainfo.a_nNodeId;
		m_pSocketMgr->at(index)->SetConnectDreb(msg->msghead.socket,msg->msghead.connecttime,psque);
		m_pMemPool->PoolFree(msg);
		return;
	}
	
}
void CMsgProcThread::OnCmdRoute(S_DREB_RSMSG *msg)
{
	std::vector<S_DREB_ROUTE>rtlist;
	S_DREB_ROUTE data;
	bzero(&data,sizeof(S_DREB_ROUTE));
	int rtnum = msg->message.head.nLen/sizeof(S_ROUTE_MSG);

	unsigned short nodeid;
	int  privateid;
	if (m_pSocketMgr->at(msg->msghead.index)->m_nType == SOCK_TODREB)
	{
		nodeid= m_pSocketMgr->at(msg->msghead.index)->m_s_center.nNodeId;
		privateid = m_pSocketMgr->at(msg->msghead.index)->m_s_center.cNodePrivateId;
	}
	else
	{
		nodeid= m_pSocketMgr->at(msg->msghead.index)->m_s_conn.nNodeId;
		privateid = m_pSocketMgr->at(msg->msghead.index)->m_s_conn.cNodePrivateId;
	}
	std::string msginfo;
	char tmpchar[400];
	bzero(tmpchar,sizeof(tmpchar));
	//先删除此连接序号上的所有总线路由
	m_pMemDb->DeleteRouteByIndex(msg->msghead.index);
	
	for (int i=0;i<rtnum;i++)
	{
		S_ROUTE_MSG *rt = (S_ROUTE_MSG *)(msg->message.buffer+i*sizeof(S_ROUTE_MSG));
		data.cNodePrivateId = rt->cNodePrivateId;
		m_pDrebEndian.Endian2Comm((unsigned char *)&(rt->nNodeId),sizeof(rt->nNodeId));
		data.nNodeId = rt->nNodeId;
		data.nStep = rt->nStep+1;
		data.nIndex = msg->msghead.index;
		m_pDrebEndian.Endian2Comm((unsigned char *)&(rt->nBandWidth),sizeof(rt->nBandWidth));
		if (m_pSocketMgr->at(msg->msghead.index)->m_nType == SOCK_TODREB && \
			m_pSocketMgr->at(msg->msghead.index)->m_s_center.nbandwidth < rt->nBandWidth)
		{
			data.nbandwidth = m_pSocketMgr->at(msg->msghead.index)->m_s_center.nbandwidth;
		}
		else if (m_pSocketMgr->at(msg->msghead.index)->m_nType == SOCK_DREB && \
			m_pSocketMgr->at(msg->msghead.index)->m_s_conn.nbandwidth < rt->nBandWidth)
		{
			data.nbandwidth = m_pSocketMgr->at(msg->msghead.index)->m_s_conn.nbandwidth;
		}
		else
		{
			data.nbandwidth = rt->nBandWidth;
		}
		if (m_pRes->g_nPrintRoute == 1)
		{
			sprintf(tmpchar,"\n   DREB[%d %d] STEP[%d]",data.nNodeId,data.cNodePrivateId,data.nStep);
			msginfo = msginfo +tmpchar;
		}

		//songfree 20210126 修改最大步进为配置
		if (data.nStep <= m_pRes->g_nMaxRouteStep )//步进大于16的认为是循环路由，去掉
		{			
			if (data.nNodeId == m_pRes->g_nDrebId && data.cNodePrivateId == m_pRes->g_nDrebPrivateId)
			{
				//是本节点的路由，去掉
			}
			else
			{
				rtlist.push_back(data);
			}
		}
	}
	m_log->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"收到index=%d 过来的路由总数[%d]",msg->msghead.index,rtlist.size());
	//增加路由,如果在直连的dreb中则不用加入路由表
	if (m_pRes->g_nPrintRoute == 1)
	{
		msginfo = msginfo +"\n";
		sprintf(tmpchar,"收到index[%d] DREB[%d %d] 的节点路由",msg->msghead.index,nodeid,privateid);
		m_pRes->g_pRouteLog.LogBin(LOG_INFO,__FILE__,__LINE__,tmpchar,(char *)msginfo.c_str(),msginfo.length());
	}
	m_pMemDb->AddRoute(msg->msghead.index,rtlist);
	PrintRoute();
	m_pMemPool->PoolFree(msg);

}

void CMsgProcThread::OnCmdDelSvr(S_DREB_RSMSG *msg)
{
	if (m_pSocketMgr->at(msg->msghead.index)->m_nType != SOCK_SRV)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"OnCmdDelSvr 连接类型不符 %d",m_pSocketMgr->at(msg->msghead.index)->m_nType);
		m_pMemPool->PoolFree(msg);
		return;
	}
	//取消本地服务表
	m_pMemDb->UnRegisterSvr(m_pSocketMgr->at(msg->msghead.index)->m_s_srv.nSvrMainId,m_pSocketMgr->at(msg->msghead.index)->m_s_srv.cSvrPrivateId);
	//取消本地服务功能表
	m_pMemDb->UnRegisterService(m_pRes->g_nDrebId,m_pRes->g_nDrebPrivateId,\
		m_pSocketMgr->at(msg->msghead.index)->m_s_srv.nSvrMainId,m_pSocketMgr->at(msg->msghead.index)->m_s_srv.cSvrPrivateId);
	m_pSocketMgr->at(msg->msghead.index)->OnClose("注销服务",__FILE__,__LINE__);
 	m_pSocketMgr->at(msg->msghead.index)->OnShutdown();
	m_pMemPool->PoolFree(msg);
}

void CMsgProcThread::OnCmdDelNode(S_DREB_RSMSG *msg)
{
	if (m_pSocketMgr->at(msg->msghead.index)->m_nType != SOCK_DREB)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"OnCmdDelNode 连接类型不符 %d",m_pSocketMgr->at(msg->msghead.index)->m_nType);
		m_pMemPool->PoolFree(msg);
		return;
	}
	m_pMemDb->UnRegisterDreb(m_pSocketMgr->at(msg->msghead.index)->m_s_conn.nNodeId,\
		m_pSocketMgr->at(msg->msghead.index)->m_s_conn.cNodePrivateId,msg->msghead.index);
	
	m_pSocketMgr->at(msg->msghead.index)->OnShutdown();
	m_pMemPool->PoolFree(msg);
}


bool CMsgProcThread::AnsMsg(S_DREB_RSMSG *msg, unsigned short errcode,const char *errmsg)
{
	if (errcode != SUCCESS)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,errmsg);
	}
	else
	{
		m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,errmsg);
	}
	if (msg->msghead.msgtype == MSG_MONITOR)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送监控通知出错 %d",msg->message.head.cCmd);
		m_pMemPool->PoolFree(msg);
		return true;
	}
	if (msg->message.head.cCmd == CMD_DPPUSH || msg->message.head.cCmd == CMD_DPABC)
	{
		m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"DPPUSH和DPABC无需应答 %d",msg->message.head.cCmd);
		m_pMemPool->PoolFree(msg);
		return true;
	}
	msg->message.head.cRaflag = 1;
	msg->message.head.cDrebAffirm = 1;
	msg->message.head.cZip = 0;
	msg->message.head.nLen = 0;
	
	msg->message.head.a_Ainfo.a_cNodePrivateId = m_pRes->g_nDrebPrivateId;	
	msg->message.head.a_Ainfo.a_nNodeId = m_pRes->g_nDrebId;
	msg->message.head.a_Ainfo.a_nRetCode = errcode;

	m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"应答 %d",msg->msghead.index);

	if (m_pSocketMgr->at(msg->msghead.index)->SendMsg(msg)!=0)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送应答出错");
	}
	return true;
}

void CMsgProcThread::OnCmdDpCall(S_DREB_RSMSG *msg)
{
	if (msg->message.head.cRaflag == 0)
	{
// #ifdef _WINDOWS
// 		m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"开始转发 %I64d",CBF_Date_Time::GetTickUS());
// #else
// 		m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"开始转发 %lld",CBF_Date_Time::GetTickUS());
// #endif
		TransMsgRequest(msg,false);
// #ifdef _WINDOWS
// 		m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"转发完成 %I64d",CBF_Date_Time::GetTickUS());
// #else
// 		m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"转发完成 %lld",CBF_Date_Time::GetTickUS());
// #endif
		
	}
	else
	{
// #ifdef _WINDOWS
// 		m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"开始应答 %I64d",CBF_Date_Time::GetTickUS());
// #else
// 		m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"开始应答 %lld",CBF_Date_Time::GetTickUS());
// #endif
		TransMsgAns(msg);
// #ifdef _WINDOWS
// 		m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"应答完成 %I64d",CBF_Date_Time::GetTickUS());
// #else
// 		m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"应答完成 %lld",CBF_Date_Time::GetTickUS());
// #endif
	}
	return;
}

void CMsgProcThread::OnCmdDpACall(S_DREB_RSMSG *msg)
{
	if (msg->message.head.cRaflag == 0)
	{
		TransMsgRequest(msg,true);
	}
	else
	{
		TransMsgAns(msg);
	}
}
void CMsgProcThread::OnCmdDpPush(S_DREB_RSMSG *msg)
{
	if (msg->message.head.cRaflag == 0)
	{
		TransMsgRequest(msg,false);
	}
	else
	{
		TransMsgAns(msg);
	}
}

void CMsgProcThread::OnCmdDpPost(S_DREB_RSMSG *msg)
{
	if (msg->message.head.cRaflag == 0)
	{
		TransMsgRequest(msg,true);
	}
	else
	{
		TransMsgAns(msg);
	}
}
void CMsgProcThread::OnCmdDpBc(S_DREB_RSMSG *msg)
{
	if (msg->message.head.cRaflag == 0)
	{
		TransDestBc(msg);
		TransBroadCast(msg,true);
	}
	else
	{
		TransMsgAns(msg);
	}
}

void CMsgProcThread::OnCmdDpABc(S_DREB_RSMSG *msg)
{
	if (msg->message.head.cRaflag == 0)
	{
		TransDestBc(msg);
		TransBroadCast(msg,false);
	}
	else
	{
		TransMsgAns(msg);
	}
}



bool CMsgProcThread::SetGlobalVar(CGlobalVar *pVar, CSocketMgr *sockmgr, CMemDB *pMemDb, CBF_BufferPool *pool,CPoolData *msgqueue)
{
	m_pRes = pVar;
	m_log = &(m_pRes->g_pLog);
	m_pMemDb=pMemDb;
	m_pSocketMgr=sockmgr;
	m_pMemPool=pool;
	m_pMsgQueue = msgqueue;
	
//	m_pLogRoute.SetLogPara(m_pRes->g_nLogLevel+1,m_pRes->g_sLogFilePath.c_str(),"drebroute.log");
	return true;
}
bool CMsgProcThread::AnsConnect(S_DREB_RSMSG *msg, unsigned short errcode,const char *errmsg)
{
	if (errcode != SUCCESS)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,errmsg);
	}
	else
	{
		m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,errmsg);
	}
	msg->message.head.cRaflag = 1;
	msg->message.head.cDrebAffirm = 1;
	msg->message.head.cZip = 0;
	msg->message.head.nLen = 0;
	msg->message.head.cNextFlag = 0;
	
	msg->message.head.a_Ainfo.a_cNodePrivateId = m_pRes->g_nDrebPrivateId;	
	msg->message.head.a_Ainfo.a_nNodeId = m_pRes->g_nDrebId;
	msg->message.head.a_Ainfo.a_nRetCode = errcode;
	
	msg->message.head.d_Dinfo.d_cNodePrivateId = msg->message.head.s_Sinfo.s_cNodePrivateId;
	msg->message.head.d_Dinfo.d_cSvrPrivateId = msg->message.head.s_Sinfo.s_cSvrPrivateId;
	msg->message.head.d_Dinfo.d_nNodeId = msg->message.head.s_Sinfo.s_nNodeId;
	msg->message.head.d_Dinfo.d_nSvrMainId = msg->message.head.s_Sinfo.s_nSvrMainId;

	msg->message.head.s_Sinfo.s_nNodeId = m_pRes->g_nDrebId;
	msg->message.head.s_Sinfo.s_cNodePrivateId = m_pRes->g_nDrebPrivateId;
	msg->message.head.s_Sinfo.s_nIndex = msg->msghead.index;
	msg->message.head.s_Sinfo.s_nTimestamp = msg->msghead.connecttime;
	msg->message.head.s_Sinfo.s_nSvrMainId = 0;
	msg->message.head.s_Sinfo.s_cSvrPrivateId = 0;

	if (m_pSocketMgr->at(msg->msghead.index)->SendMsg(msg,true)!=0)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送应答出错");
		return false;
	}
	return true;
}


void CMsgProcThread::SendService(int index, std::vector<S_SERVICE_ROUTE *> rtlist)
{
	S_DREB_RSMSG *rsmsg=NULL;
	if (rtlist.size()<1)
	{
		rsmsg = (S_DREB_RSMSG *) m_pMemPool->PoolMalloc();
		if (rsmsg == NULL)
		{
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"内存不足，分配数据总线节点数据空间出错");
			return;
		}
		bzero(&(rsmsg->message.head), sizeof(DREB_HEAD));
		rsmsg->msghead.connecttime = m_pSocketMgr->at(index)->m_nConntime;
		rsmsg->msghead.index = index;
		rsmsg->message.head.cRaflag = 0;
		rsmsg->message.head.cNextFlag = 0;
		rsmsg->message.head.cZip = 0;
		rsmsg->message.head.cCmd = CMD_SERVICE;
		rsmsg->message.head.s_Sinfo.s_nIndex = index;
		rsmsg->message.head.s_Sinfo.s_cNodePrivateId = m_pRes->g_nDrebPrivateId;	
		rsmsg->message.head.s_Sinfo.s_nNodeId = m_pRes->g_nDrebId;
		rsmsg->message.head.s_Sinfo.s_nTimestamp = m_pSocketMgr->at(index)->m_nConntime;
		rsmsg->message.head.nLen = 0;
		if (m_pSocketMgr->at(index)->SendMsg(rsmsg)!=0)
		{
			if (m_pSocketMgr->at(index)->m_nType == SOCK_DREB)
			{
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送服务路由SOCK_DREB出错");
			}
			else if (m_pSocketMgr->at(index)->m_nType == SOCK_TODREB)
			{
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送服务路由SOCK_TODREB 出错");
			}
			
		}
		return;
	}
	bool isnext=false;
	int len=0;
	int allfuncnum=0;
	S_ROUTESVR_MSG *prs=NULL;
	rsmsg = (S_DREB_RSMSG *) m_pMemPool->PoolMalloc();
	if (rsmsg == NULL)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"内存不足，分配数据总线节点数据空间出错");
		return;
	}
	bzero(&(rsmsg->message.head), sizeof(DREB_HEAD));
	rsmsg->msghead.connecttime = m_pSocketMgr->at(index)->m_nConntime;
	rsmsg->msghead.index = index;
	rsmsg->message.head.cRaflag = 0;
	rsmsg->message.head.cNextFlag = 0;
	rsmsg->message.head.cZip = 0;
	rsmsg->message.head.cCmd = CMD_SERVICE;
	rsmsg->message.head.s_Sinfo.s_nIndex = index;
	rsmsg->message.head.s_Sinfo.s_cNodePrivateId = m_pRes->g_nDrebPrivateId;	
	rsmsg->message.head.s_Sinfo.s_nNodeId = m_pRes->g_nDrebId;
	rsmsg->message.head.s_Sinfo.s_nTimestamp = m_pSocketMgr->at(index)->m_nConntime;
	rsmsg->message.head.nLen = 0;
	bool isbegin = false;
	for (unsigned int i=0;i<rtlist.size();i++)
	{
		if (rtlist[i]->nStep >= m_pRes->g_nMaxRouteStep-1) //步进大于最大的步进-1，不用再发了， 发了也没用
		{
			continue;
		}
		if (isbegin) //一个新包开始
		{
			prs = (S_ROUTESVR_MSG *)(rsmsg->message.buffer+len);
			prs->cNodePrivateId = rtlist[i]->cNodePrivateId;
			prs->nNodeId = rtlist[i]->nNodeId;
			m_pDrebEndian.Endian2Comm((unsigned char *)&(prs->nNodeId),sizeof(prs->nNodeId));
			prs->nSvrMainId = rtlist[i]->nSvrMainId;
			m_pDrebEndian.Endian2Comm((unsigned char *)&(prs->nSvrMainId),sizeof(prs->nSvrMainId));
			prs->cSvrPrivateId = rtlist[i]->cSvrPrivateId;
			prs->nFuncNo = rtlist[i]->nFuncNo;
			m_pDrebEndian.Endian2Comm((unsigned char *)&(prs->nFuncNo),sizeof(prs->nFuncNo));
			prs->nFuncNum = 1;
			allfuncnum++;
			prs->cStep = rtlist[i]->nStep;
			len = sizeof(S_ROUTESVR_MSG);
			isbegin = false;
		}
		else
		{
			if (i==0)
			{
				//第一个数据包
				prs = (S_ROUTESVR_MSG *)rsmsg->message.buffer;
				prs->cNodePrivateId = rtlist[i]->cNodePrivateId;
				prs->nNodeId = rtlist[i]->nNodeId;
				m_pDrebEndian.Endian2Comm((unsigned char *)&(prs->nNodeId),sizeof(prs->nNodeId));
				prs->nSvrMainId = rtlist[i]->nSvrMainId;
				m_pDrebEndian.Endian2Comm((unsigned char *)&(prs->nSvrMainId),sizeof(prs->nSvrMainId));
				prs->cSvrPrivateId = rtlist[i]->cSvrPrivateId;
				prs->nFuncNo = rtlist[i]->nFuncNo;
				m_pDrebEndian.Endian2Comm((unsigned char *)&(prs->nFuncNo),sizeof(prs->nFuncNo));
				prs->nFuncNum = 1;
				allfuncnum++;
				prs->cStep = rtlist[i]->nStep;
				len = sizeof(S_ROUTESVR_MSG);
			}
			else
			{
				if (rtlist[i]->nNodeId == rtlist[i-1]->nNodeId && rtlist[i]->cNodePrivateId == rtlist[i-1]->cNodePrivateId \
					&& rtlist[i]->nSvrMainId == rtlist[i-1]->nSvrMainId && rtlist[i]->cSvrPrivateId == rtlist[i-1]->cSvrPrivateId)
				{ //还是上一个服务
					//20160108moid HP_UX
// 					unsigned int *func = (unsigned int *)(rsmsg->message.buffer+len);
// 					*func = rtlist[i].nFuncNo;
					memcpy(rsmsg->message.buffer+len,&(rtlist[i]->nFuncNo),sizeof(unsigned int));
					m_pDrebEndian.Endian2Comm((unsigned char *)(rsmsg->message.buffer+len),sizeof(unsigned int));
					//20160108MODI END 
					prs->nFuncNum++;
					allfuncnum++;
					len +=sizeof(unsigned int);
				}
				else //换了一个服务了
				{
					//对上一个服务的交易数进行字节序处理
					m_pDrebEndian.Endian2Comm((unsigned char *)&(prs->nFuncNum),sizeof(prs->nFuncNum));
					//下一个服务继续
					prs = (S_ROUTESVR_MSG *)(rsmsg->message.buffer+len);
					prs->cNodePrivateId = rtlist[i]->cNodePrivateId;
					prs->nNodeId = rtlist[i]->nNodeId;
					m_pDrebEndian.Endian2Comm((unsigned char *)&(prs->nNodeId),sizeof(prs->nNodeId));
					prs->nSvrMainId = rtlist[i]->nSvrMainId;
					m_pDrebEndian.Endian2Comm((unsigned char *)&(prs->nSvrMainId),sizeof(prs->nSvrMainId));
					prs->cSvrPrivateId = rtlist[i]->cSvrPrivateId;
					prs->nFuncNo = rtlist[i]->nFuncNo;
					m_pDrebEndian.Endian2Comm((unsigned char *)&(prs->nFuncNo),sizeof(prs->nFuncNo));
					prs->nFuncNum = 1;
					allfuncnum++;
					prs->cStep = rtlist[i]->nStep;
					len += sizeof(S_ROUTESVR_MSG);
				}
			}
		}
		
		if (i == rtlist.size()-1) //最后一个服务了
		{
			rsmsg->message.head.nLen = len;
			
			m_log->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"index=%d 发送service路由数目%d 数据长度%d",index,allfuncnum,len);
			m_pDrebEndian.Endian2Comm((unsigned char *)&(prs->nFuncNum),sizeof(prs->nFuncNum));
			if (m_pSocketMgr->at(index)->SendMsg(rsmsg)!=0)
			{
				if (m_pSocketMgr->at(index)->m_nType == SOCK_DREB)
				{
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送服务路由SOCK_DREB出错");
				}
				else if (m_pSocketMgr->at(index)->m_nType == SOCK_TODREB)
				{
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送服务路由SOCK_TODREB 出错");
				}
			}
			return ;
		}
		
		if (len >= DREBDATASIZE-100) //服务够多，要多包发送
		{
			rsmsg->message.head.nLen = len;
			rsmsg->message.head.cNextFlag = 1;
			isnext = true;
			m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"分数据包发送交易路由 index=%d 发送service路由数目%d 数据长度%d",index,allfuncnum,len);
			m_pDrebEndian.Endian2Comm((unsigned char *)&(prs->nFuncNum),sizeof(prs->nFuncNum));
			if (m_pSocketMgr->at(index)->SendMsg(rsmsg)!=0)
			{
				if (m_pSocketMgr->at(index)->m_nType == SOCK_DREB)
				{
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送服务路由SOCK_DREB出错");
				}
				else if (m_pSocketMgr->at(index)->m_nType == SOCK_TODREB)
				{
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送服务路由SOCK_TODREB 出错");
				}
			}
			rsmsg = (S_DREB_RSMSG *) m_pMemPool->PoolMalloc();
			if (rsmsg == NULL)
			{
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"内存不足，分配数据总线节点数据空间出错");
				return;
			}
			bzero(&(rsmsg->message.head), sizeof(DREB_HEAD));
			rsmsg->msghead.connecttime = m_pSocketMgr->at(index)->m_nConntime;
			rsmsg->msghead.index = index;
			rsmsg->message.head.cRaflag = 0;
			if (isnext) //有后续包 
			{
				rsmsg->message.head.cNextFlag = 1;
			}
			else
			{
				rsmsg->message.head.cNextFlag = 0;
			}
			
			rsmsg->message.head.cZip = 0;
			rsmsg->message.head.cCmd = CMD_SERVICE;
			rsmsg->message.head.s_Sinfo.s_nIndex = index;
			rsmsg->message.head.s_Sinfo.s_cNodePrivateId = m_pRes->g_nDrebPrivateId;	
			rsmsg->message.head.s_Sinfo.s_nNodeId = m_pRes->g_nDrebId;
			rsmsg->message.head.s_Sinfo.s_nTimestamp = m_pSocketMgr->at(index)->m_nConntime;
			rsmsg->message.head.nLen = 0;
			len = 0;
			isbegin = true;  //表示从新一个数据包开始
			allfuncnum =0;
		}
	} // end for (int i=0;i<rtlist.size();i++)
	m_pMemPool->PoolFree(rsmsg);
	return;
}


// 函数名: CMsgProcThread::OnCmdService
// 编程  : 王明松 2013-6-24 16:21:33
// 返回  : void 
// 参数  : S_DREB_RSMSG *msg
// 描述  : 接收到的服务路由，更新路由表
void CMsgProcThread::OnCmdService(S_DREB_RSMSG *msg)
{
	int snum=0;
	PCOMMSTRU databuf = &(msg->message);
	S_ROUTESVR_MSG *prs=NULL;
	S_SERVICE_ROUTE data;
	int nFuncNum=0;
	unsigned int  bufferlen=0;

	std::string routemsg;
	char tmpchar[300];
	std::vector<S_SERVICE_ROUTE *>servicelist;
	unsigned short nodeid;
	int  privateid;
	if (m_pSocketMgr->at(msg->msghead.index)->m_nType == SOCK_TODREB)
	{
		nodeid= m_pSocketMgr->at(msg->msghead.index)->m_s_center.nNodeId;
		privateid = m_pSocketMgr->at(msg->msghead.index)->m_s_center.cNodePrivateId;
	}
	else
	{
		nodeid= m_pSocketMgr->at(msg->msghead.index)->m_s_conn.nNodeId;
		privateid = m_pSocketMgr->at(msg->msghead.index)->m_s_conn.cNodePrivateId;
	}
	PrintServiceRoute();
	//20210126 songfree add 当对端送过来的路由，表示没有此节点没有功能注册，删除原来的
	if (databuf->head.cNextFlag == 0 && databuf->head.nLen == 0 ) 
	{
		//表示对端送来的是一个空的，没有交易
		m_pMemDb->DeleteServiceByIndex(msg->msghead.index);
		return ;
	}
	//20210126 songfree add end

	//先更新一下服务功能路由，删除过期的功能
	if (databuf->head.cNextFlag == 0) //只有一个数据包的交易路由
	{
		m_pMemDb->UpdateRouteServiceByIndex(msg->msghead.index);
	}
	m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,\
		"收到连接index=%d %s 发过来的service路由数据长度%d",msg->msghead.index,\
		GetIndexFlag(msg->msghead.index).c_str(),databuf->head.nLen);
	while (bufferlen<databuf->head.nLen)
	{
		if (databuf->head.nLen-bufferlen <sizeof(S_ROUTESVR_MSG))
		{
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"服务路由不符");
			break;
		}
		prs = (S_ROUTESVR_MSG *)(databuf->buffer+bufferlen);
		m_pDrebEndian.Endian2LocalHost((unsigned char *)&(prs->nNodeId),sizeof(prs->nNodeId));
		m_pDrebEndian.Endian2LocalHost((unsigned char *)&(prs->nSvrMainId),sizeof(prs->nSvrMainId));
		m_pDrebEndian.Endian2LocalHost((unsigned char *)&(prs->nFuncNo),sizeof(prs->nFuncNo));
		m_pDrebEndian.Endian2LocalHost((unsigned char *)&(prs->nFuncNum),sizeof(prs->nFuncNum));
		if (prs->nFuncNum*sizeof(unsigned int) > (databuf->head.nLen-bufferlen-11))
		{
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"收到连接index=%d 发过来的service路由信息中服务功能数目不符 %d",\
				msg->msghead.index,prs->nFuncNum);
			m_pMemPool->PoolFree(msg);
			return;
		}
		if (prs->nFuncNum<1)
		{
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"收到连接index=%d 发过来的service路由信息中服务功能数目不符 %d",\
				msg->msghead.index,prs->nFuncNum);
			m_pMemPool->PoolFree(msg);
			return ;
		}
		m_log->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"收到连接index=%d 发过来的service路由数目%d",msg->msghead.index,prs->nFuncNum);

		data.nNodeId = prs->nNodeId;
		data.nSvrMainId = prs->nSvrMainId;
		data.nFuncNo = prs->nFuncNo;
		data.cSvrPrivateId = prs->cSvrPrivateId;
		data.cNodePrivateId = prs->cNodePrivateId;
		data.nIndex = msg->msghead.index;
		data.nStep = prs->cStep+1;
		if (m_pSocketMgr->at(data.nIndex)->m_nType == SOCK_TODREB)
		{
			data.nbandwidth = m_pSocketMgr->at(data.nIndex)->m_s_center.nbandwidth;
		}
		else if (m_pSocketMgr->at(data.nIndex)->m_nType == SOCK_DREB)
		{
			data.nbandwidth = m_pSocketMgr->at(data.nIndex)->m_s_conn.nbandwidth;
		}
		
		if (m_pRes->g_nPrintRoute == 1)
		{
			sprintf(tmpchar,"\n   DREB[%d %d] SVR[%d %d] STEP[%d] FUNCNO[%d",\
				data.nNodeId,data.cNodePrivateId,data.nSvrMainId,data.cSvrPrivateId,data.nStep,data.nFuncNo);
			routemsg = routemsg+tmpchar;
		}
		nFuncNum++;
		m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"收到index=%d的service路由nNodeId=%d cNodePrivateId=%d nSvrMainId=%d cSvrPrivateId=%d nStep=%d nFuncNo=%d",\
			data.nIndex,data.nNodeId,data.cNodePrivateId,data.nSvrMainId,data.cSvrPrivateId,data.nStep,data.nFuncNo);
		//songfree 20210126 修改最大步进为配置
		if (data.nStep<= m_pRes->g_nMaxRouteStep)//步进大于16的认为是循环路由，去掉
		{
			if (data.nNodeId == m_pRes->g_nDrebId && data.cNodePrivateId == m_pRes->g_nDrebPrivateId)
			{
				//是本节点的路由，去掉
			}
			else
			{
				m_pMemDb->AddServiceRoute(data);
			}
			
		}
		snum = prs->nFuncNum -1;
		bufferlen +=sizeof(S_ROUTESVR_MSG);;

		//20161014 若服务是本地的，不用增加了
		if (prs->nNodeId == m_pRes->g_nDrebId && prs->cNodePrivateId == m_pRes->g_nDrebPrivateId)
		{
			m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"丢掉本节点的路由  index=%d的service路由nNodeId=%d cNodePrivateId=%d nSvrMainId=%d cSvrPrivateId=%d nStep=%d nFuncNo=%d",\
			data.nIndex,data.nNodeId,data.cNodePrivateId,data.nSvrMainId,data.cSvrPrivateId,data.nStep,data.nFuncNo);
			continue;
		}

		for (int i=0;i <snum ; i++)
		{
			memcpy(&(data.nFuncNo),databuf->buffer+bufferlen,sizeof(unsigned int ));
			m_pDrebEndian.Endian2LocalHost((unsigned char *)&(data.nFuncNo),sizeof(unsigned int ));
			bufferlen +=sizeof(unsigned int );
			if (m_pRes->g_nPrintRoute == 1)
			{
				sprintf(tmpchar,",%d",data.nFuncNo);
				routemsg = routemsg+tmpchar;
			}
			//songfree 20210126 修改最大步进为配置
			if (data.nStep<=m_pRes->g_nMaxRouteStep)//步进大于16的认为是循环路由，去掉
			{
				if (data.nNodeId == m_pRes->g_nDrebId && data.cNodePrivateId == m_pRes->g_nDrebPrivateId)
				{
					//是本节点的路由，去掉
				}
				else
				{
					if (!m_pMemDb->AddServiceRoute(data))
					{
						m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"增加或更新路由出错 收到index=%d的service路由nNodeId=%d cNodePrivateId=%d nSvrMainId=%d cSvrPrivateId=%d nStep=%d nFuncNo=%d",\
							data.nIndex,data.nNodeId,data.cNodePrivateId,data.nSvrMainId,data.cSvrPrivateId,data.nStep,data.nFuncNo);
						m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"增加或更新路由出错");
					}
				}
			}
			nFuncNum++;
		}
		if (m_pRes->g_nPrintRoute == 1)
		{
			sprintf(tmpchar,"]\n");
			routemsg = routemsg+tmpchar;
		}
	}
	if (databuf->head.cNextFlag == 0)
	{
		m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"INDEX=%d %s 有%d个服务功能号路由",\
			msg->msghead.index,GetIndexFlag(msg->msghead.index).c_str(),nFuncNum);
	}
	else
	{
		m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"INDEX=%d %s 后续有%d个服务功能号路由",\
			msg->msghead.index,GetIndexFlag(msg->msghead.index).c_str(),nFuncNum);
	}
	
	if (m_pRes->g_nPrintRoute == 1)
	{
		sprintf(tmpchar,"收到index[%d] DREB[%d %d] 的交易路由",msg->msghead.index,nodeid,privateid);
		m_pRes->g_pRouteLog.LogBin(LOG_INFO,__FILE__,__LINE__,tmpchar,(char *)routemsg.c_str(),routemsg.length());
	}

	m_pMemPool->PoolFree(msg);
	PrintServiceRoute();
	return ;
}	

void CMsgProcThread::TransMsgAns(S_DREB_RSMSG *msg)
{
	if (msg->message.head.s_Sinfo.s_nNodeId<1)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"请求时的DREB节点不符 %d",msg->message.head.s_Sinfo.s_nNodeId);
		m_pMemPool->PoolFree(msg);
		return ;
	}
	if (msg->message.head.s_Sinfo.s_nNodeId == m_pRes->g_nDrebId && \
		msg->message.head.s_Sinfo.s_cNodePrivateId == m_pRes->g_nDrebPrivateId) //就是本数据总线节点了
	{
		//转发至连接
		if (m_pSocketMgr->at(msg->message.head.s_Sinfo.s_nIndex)->m_nType == SOCK_CLI && \
			m_pSocketMgr->at(msg->message.head.s_Sinfo.s_nIndex)->m_nConntime != msg->message.head.s_Sinfo.s_nTimestamp)
		{
			//客户端连接已断开，不用应答了
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"应答时客户端连接已断开[%d %s] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d] 业务数据长度[%d]",\
				msg->message.head.s_Sinfo.s_nIndex,GetIndexFlag(msg->message.head.s_Sinfo.s_nIndex).c_str(),\
				GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
				msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
				msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
				msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		        msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
			m_pMemPool->PoolFree(msg);
			return;
		}
		if (m_pSocketMgr->at(msg->message.head.s_Sinfo.s_nIndex)->m_nType == SOCK_CLI)
		{    //给客户端的应答
			msg->msghead.connecttime = msg->message.head.s_Sinfo.s_nTimestamp;
			if (m_log->isWrite(LOG_DEBUG))
			{
				m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"将数据应答给客户端[%d %s] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d] 业务数据长度[%d]",\
					msg->message.head.s_Sinfo.s_nIndex,GetIndexFlag(msg->message.head.s_Sinfo.s_nIndex).c_str(),\
					GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
					msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
					msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
					msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
					msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
			}

			
			if (m_pSocketMgr->at(msg->message.head.s_Sinfo.s_nIndex)->SendMsg(msg)!=0)
			{
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送失败[%d %s] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d] 业务数据长度[%d]",\
					msg->message.head.s_Sinfo.s_nIndex,GetIndexFlag(msg->message.head.s_Sinfo.s_nIndex).c_str(),\
					GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
					msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
					msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
					msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		            msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
				return;
			}
			
		}
		else //给服务的应答
		{
			S_SVR_ROUTE svr;
			if (!m_pMemDb->SelectSvr(msg->message.head.s_Sinfo.s_nSvrMainId,msg->message.head.s_Sinfo.s_cSvrPrivateId,svr))
			{
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"应答时服务端服务已注销[%d %s] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d] 业务数据长度[%d]",\
					msg->message.head.s_Sinfo.s_nIndex,GetIndexFlag(msg->message.head.s_Sinfo.s_nIndex).c_str(),\
					GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
					msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
					msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
					msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		            msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
				m_pMemPool->PoolFree(msg);
				return;
			}
			if (m_log->isWrite(LOG_DEBUG))
			{
				m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"将数据应答给服务端[%d %s] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d] 业务数据长度[%d]",\
					msg->message.head.s_Sinfo.s_nIndex,GetIndexFlag(msg->message.head.s_Sinfo.s_nIndex).c_str(),\
					GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
					msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
					msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
					msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		            msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
			}
			
			//转发至服务

			if (m_pSocketMgr->at(msg->message.head.s_Sinfo.s_nIndex)->SendMsg(msg)!=0)
			{
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送失败");
				return;
			}
		}
		return ;
		
	}
	//通过路由转发
	S_DREB_ROUTE dreb;
	if (msg->message.head.s_Sinfo.s_cNodePrivateId <1)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"应答时数据总线节点私有节点[%d] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d] 业务数据长度[%d]",\
			msg->message.head.s_Sinfo.s_cNodePrivateId,\
			GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
			msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
					msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
					msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		            msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
		m_pMemPool->PoolFree(msg);
		return;
	}
	else
	{
		if (!m_pMemDb->SelectRoute(msg->message.head.s_Sinfo.s_nNodeId,msg->message.head.s_Sinfo.s_cNodePrivateId,dreb))
		{
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"应答时数据总线节点路由不存在[%d %d] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d] 业务数据长度[%d]",\
				msg->message.head.s_Sinfo.s_nNodeId,msg->message.head.s_Sinfo.s_cNodePrivateId,\
				GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
				msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
					msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
					msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		            msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
			m_pMemPool->PoolFree(msg);
			return;
		}
	}
	if (m_log->isWrite(LOG_DEBUG))
	{
		m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"将数据应答给路由转发[%d %d] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d] 业务数据长度[%d]",\
			dreb.nIndex,GetIndexFlag(dreb.nIndex).c_str(),\
			GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
			msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
			msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
			msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
			msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
	
	}
	
	if (m_pSocketMgr->at(dreb.nIndex)->SendMsg(msg)!=0)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送失败");
		return;
	}
	return;	
}

void CMsgProcThread::OnCmdServiceReg(S_DREB_RSMSG *msg)
{
	int snum=0;
	PCOMMSTRU databuf = &(msg->message);
	S_SERVICEREG_MSG *prs=NULL;
	S_SERVICE_ROUTE data;

	prs = (S_SERVICEREG_MSG *)(databuf->buffer);
	m_pDrebEndian.Endian2LocalHost((unsigned char *)&(prs->nSvrMainId),sizeof(prs->nSvrMainId));
	m_pDrebEndian.Endian2LocalHost((unsigned char *)&(prs->nFuncNo),sizeof(prs->nFuncNo));
	m_pDrebEndian.Endian2LocalHost((unsigned char *)&(prs->nFuncNum),sizeof(prs->nFuncNum));
	
	m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"服务[%d %d]注册   交易个数[%d] len=[%d]",\
		prs->nSvrMainId,prs->cSvrPrivateId,prs->nFuncNum,msg->message.head.nLen);
	if (prs->nFuncNum <1)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"服务[%d %d]注册 交易个数[%d]不符",prs->nSvrMainId,prs->cSvrPrivateId,prs->nFuncNum);
		m_pMemPool->PoolFree(msg);
		return ;
	}
	if (msg->message.head.nLen <sizeof(S_SERVICEREG_MSG) + (prs->nFuncNum -1)*sizeof(unsigned int ))
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"服务[%d %d]注册 报文长度[%d]不符 应为[%d]",\
			prs->nSvrMainId,prs->cSvrPrivateId,prs->nFuncNum,msg->message.head.nLen,\
			sizeof(S_SERVICEREG_MSG) + (prs->nFuncNum -1)*sizeof(unsigned int));
		m_pMemPool->PoolFree(msg);
		return ;
	}

	data.nNodeId = m_pRes->g_nDrebId;
	data.nSvrMainId = prs->nSvrMainId;
	data.nFuncNo = prs->nFuncNo;
	data.cSvrPrivateId = prs->cSvrPrivateId;
	data.cNodePrivateId = m_pRes->g_nDrebPrivateId;
	data.nStep = 0;
	//20160112modi begin 在注册时将服务的连接序号放进交易路由表
	//data.nIndex=0;
	data.nIndex = msg->msghead.index;
	data.nbandwidth = 10240000;
	//20160112modi end

	S_SVR_ROUTE svr;
	if (!m_pMemDb->SelectSvr(data.nSvrMainId,data.cSvrPrivateId,svr))
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"功能注册时的服务[%d %d]在服务表中不存在",data.nSvrMainId,data.cSvrPrivateId);
		m_pMemPool->PoolFree(msg);
		return ;
	}
	if (svr.nIndex != data.nIndex)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"功能注册时的服务[%d %d]的连接序号[%d]和注册服务的连接序号[%d]不一到长",\
			data.nSvrMainId,data.cSvrPrivateId,data.nIndex,svr.nIndex);
		m_pMemPool->PoolFree(msg);
		return ;
	}
    //20130304 songfree add 当有交易注册时，删除连接时写入路由表的服务路由 
    if (prs->nFuncNum > 0)
    {
        m_log->LogMp(LOG_DEBUG + 1, __FILE__, __LINE__, "m_serviceTbl Delete   begin");
        m_pMemDb->m_serviceTbl.Delete(data.nIndex, m_pRes->g_nDrebId, m_pRes->g_nDrebPrivateId, data.nSvrMainId, data.cSvrPrivateId, 0);
        m_log->LogMp(LOG_DEBUG + 1, __FILE__, __LINE__, "m_serviceTbl Delete   end");
        m_log->LogMp(LOG_DEBUG + 1, __FILE__, __LINE__, "将交易[%d]加入交易表", data.nFuncNo);
        m_pMemDb->AddServiceRoute(data);
        m_log->LogMp(LOG_DEBUG + 1, __FILE__, __LINE__, "将交易[%d]加入交易表完成", data.nFuncNo);
    }
	//add end
	
	
	snum = prs->nFuncNum -1;
	
	int nFuncNum =  prs->nFuncNum;
	for (int i=0; i< snum; i++)
	{
		//20160107修改  data.nFuncNo = *func;在HP_UX下死掉
		memcpy(&(data.nFuncNo),databuf->buffer+sizeof(S_SERVICEREG_MSG)+i*sizeof(unsigned int),sizeof(unsigned int));
		m_log->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"Endian2LocalHost begin" );
		m_pDrebEndian.Endian2LocalHost((unsigned char *)&(data.nFuncNo),sizeof(unsigned int ));
		m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"将交易[%d]加入交易表",data.nFuncNo );
		m_pMemDb->AddServiceRoute(data);
		m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"将交易[%d]加入交易表完成",data.nFuncNo );
	}

	m_log->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"服务%d-%d有功能%d个注册",data.nSvrMainId,data.cSvrPrivateId,nFuncNum);
	if (m_pRes->g_nRegOtherDreb == 1) //向其它总线注册
	{
		OnMsgServiceRoute(msg);
	}
	return ;
}

void CMsgProcThread::OnCmdUnServiceReg(S_DREB_RSMSG *msg)
{
	PCOMMSTRU databuf = &(msg->message);
	S_SERVICEREG_MSG *prs=NULL;
	S_SERVICE_ROUTE data;

	prs = (S_SERVICEREG_MSG *)(databuf->buffer);
	m_pDrebEndian.Endian2LocalHost((unsigned char *)&(prs->nSvrMainId),sizeof(prs->nSvrMainId));
	m_pDrebEndian.Endian2LocalHost((unsigned char *)&(prs->nFuncNo),sizeof(prs->nFuncNo));
	m_pDrebEndian.Endian2LocalHost((unsigned char *)&(prs->nFuncNum),sizeof(prs->nFuncNum));
	data.nNodeId = m_pRes->g_nDrebId;
	data.nSvrMainId = prs->nSvrMainId;
	data.nFuncNo = prs->nFuncNo;
	data.cSvrPrivateId = prs->cSvrPrivateId;
	data.cNodePrivateId = m_pRes->g_nDrebPrivateId;
	data.nStep = 0;
	data.nIndex = 0;
	S_SVR_ROUTE svr;
	if (!m_pMemDb->SelectSvr(data.nSvrMainId,data.cSvrPrivateId,svr))
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"取消功能注册时的服务[%d %d]在服务表中不存在",data.nSvrMainId,data.cSvrPrivateId);
		m_pMemPool->PoolFree(msg);
		return ;
	}
	//取消注册
	m_pMemDb->UnRegisterService(data.nNodeId,data.cNodePrivateId,data.nSvrMainId,data.cSvrPrivateId);

	if (!svr.bIsClose)
	{
		//20130304 songfree add 服务连接时，将服务加入到交易路由表，供按服务路由，交易码默认为0
		S_SERVICE_ROUTE rt;
		bzero(&rt,sizeof(S_SERVICE_ROUTE));
		rt.cNodePrivateId = m_pRes->g_nDrebPrivateId;
		rt.cSvrPrivateId = data.cSvrPrivateId;
		rt.nFuncNo = 0;
		rt.nIndex = svr.nIndex;
		rt.nNodeId = m_pRes->g_nDrebId;
		rt.nStep = 0;
		rt.nSvrMainId = data.nSvrMainId;
		rt.nUpdateTime = time(NULL);
		m_pMemDb->AddServiceRoute(rt);
	//ADD END
	}
	
	return ;
}
void CMsgProcThread::OnCmdMonitorDreb(S_DREB_RSMSG *msg)
{
	if (msg->message.head.cRaflag == 1) //应答
	{
		TransMsgAns(msg);
		return;
	}
	if (msg->message.head.d_Dinfo.d_nNodeId >0) // 发给指定的dreb节点的，判断是否通过路由转发
	{
		if (msg->message.head.d_Dinfo.d_nNodeId == m_pRes->g_nDrebId && msg->message.head.d_Dinfo.d_cNodePrivateId == m_pRes->g_nDrebPrivateId)
		{

		}
		else //不是本节点的请球，路由转发
		{
			S_DREB_ROUTE dreb;
			if (!m_pMemDb->SelectRoute(msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,dreb))
			{
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"路由[%d %d]不存在，可能是数据总线节点未注册",msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId);
				AnsMsg(msg,ERR_DREBROUTE,"路由不存在，可能是数据总线节点未注册");
				//WriteErrLog(8,msg->message);
				return;
			}
		
			if (dreb.nIndex <1)
			{
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"怎么会出现路由指向本身呢");
				m_pMemPool->PoolFree(msg);
				return ;
			}
			if (m_log->isWrite(LOG_DEBUG))
			{
				m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"将数据发给路由处理nIndex=%d %s nCmd=%d %s cRaFlag=%d cZip=%d s_nHook=%d s_nSerial=%d nLen=%d ",\
					dreb.nIndex,GetIndexFlag(dreb.nIndex).c_str(),msg->message.head.cCmd,\
					GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cRaflag,msg->message.head.cZip,\
				msg->message.head.s_Sinfo.s_nHook,msg->message.head.s_Sinfo.s_nSerial,msg->message.head.nLen);
			}
			
			if (!dreb.bIsClose)
			{
				m_pSocketMgr->at(dreb.nIndex)->SendMsg(msg);
			}
			else if (dreb.pSendQueue->PutSendMsg(msg)!=0)
			{
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"入发送队列失败");
				return;
			}
			return;
		}
		
	}
// #define  DREBMONI_INFO          1     //dreb信息 当前节点、当前节点服务
// #define  DREBMONI_HOST          2     //主机信息 CPU 碰盘 内存
// #define  DREBMONI_CURTXLIST     3     //dreb信息 当前节点交易列表
// #define  DREBMONI_ROUTEDREB     4     //dreb路由信息 节点路由表
// #define  DREBMONI_ROUTESERVICE  5     //dreb路由信息 服务交易路由表
// #define  DREBMONI_REPORT        6     //dreb运行统计信息
// #define  DREBMONI_CONNINFO      7     //dreb连接信息
// #define  DREBMONI_REPORTSTART   8     //dreb开始统计信息 pack/s byte/s
// #define  DREBMONI_REPORTSTOP    9     //dreb停止统计信息
	switch (msg->message.head.d_Dinfo.d_nServiceNo)
	{
		case DREBMONI_INFO:
			Monitor_info(msg);
			break;
		case DREBMONI_HOST:
			Monitor_Host(msg);
			break;
		case DREBMONI_CURTXLIST:
			Monitor_CurTxList(msg);
			break;
		case DREBMONI_ROUTEDREB:
			Monitor_RouteDreb(msg);
			break;
		case DREBMONI_ROUTESERVICE:
			Monitor_RouteService(msg);
			break;
		case DREBMONI_REPORT:
			Monitor_Report(msg);
			break;
		case DREBMONI_CONNINFO:
			Monitor_ConnInfo(msg);
			break;
		case DREBMONI_REPORTSTART:
			Monitor_ReportStart(msg);
			break;
		case DREBMONI_REPORTSTOP:
			Monitor_ReportStop(msg);
			break;
		default:
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"无此监控交易类型 %d",msg->message.head.d_Dinfo.d_nServiceNo);
			AnsMsg(msg,ERR_FUNCNOTFUND,"无此监控交易类型");
			//WriteErrLog(8,msg->message);
			break;
	}
	return;
}

void CMsgProcThread::OnCmdMonitorBpc(S_DREB_RSMSG *msg)
{
	if (msg->message.head.cRaflag == 1) //应答
	{
		TransMsgAns(msg);
		return;
	}
	if (msg->message.head.d_Dinfo.d_nSvrMainId<1)
	{
		//服务未填写
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"未指定公共服务号[%d]",msg->message.head.d_Dinfo.d_nSvrMainId);
		AnsMsg(msg,ERR_SVRNOTREGISTER,"未指定公共服务号");
		return ;
	}
	bool bRet;
	if (msg->message.head.d_Dinfo.d_nNodeId>0)
	{
		//首先判断是否本总线节点
		if (msg->message.head.d_Dinfo.d_nNodeId == m_pRes->g_nDrebId)
		{
			//公共节点相同
			if (0 == msg->message.head.d_Dinfo.d_cNodePrivateId || msg->message.head.d_Dinfo.d_cNodePrivateId == m_pRes->g_nDrebPrivateId)
			{
				S_SERVICE_ROUTE rt;
				//公共节点相同，私有节点为0或就是本节点
				//本地连接的服务
				if (0 == msg->message.head.d_Dinfo.d_cNodePrivateId )
				{
					bRet = m_pMemDb->SelectLocalBySvr(msg->message.head.d_Dinfo.d_nSvrMainId,rt);
				}
				else
				{
					bRet = m_pMemDb->SelectLocalBySvr(msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cNodePrivateId,rt);
				}
				if (!bRet)
				{
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"路由错误 根据总线节点-服务路由，找不到服务 [%d %d %d %d] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d]  业务数据长度[%d]",\
						msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
						msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
						GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
						msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
						msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
						msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		        msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
					AnsMsg(msg,ERR_SVRNOTREGISTER,"根据总线节点-服务路由，找不到服务");
					return ;
				}
				//发给指定服务
				//转发至服务
				if (m_log->isWrite(LOG_DEBUG))
				{
					m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"将数据发给本地服务处理 [%d %s] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d]  业务数据长度[%d]",\
						rt.nIndex,GetIndexFlag(rt.nIndex).c_str(),\
						GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
						msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
						msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
						msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
						msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
				}
				
				m_pSocketMgr->at(rt.nIndex)->SendMsg(msg);

				return ;
			}
			//公共节点相同但私有节点不相同，所以通过下面的路由方式
		}
		
		S_DREB_ROUTE  dreb;
		//不是本节点，要通过路由转发
		if (0 == msg->message.head.d_Dinfo.d_cNodePrivateId)
		{
			bRet =m_pMemDb->SelectRoute(msg->message.head.d_Dinfo.d_nNodeId,dreb);
		}
		else
		{
			bRet =m_pMemDb->SelectRoute(msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,dreb);
		}
		if (!bRet)
		{
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"路由错误 找不到总线节点[%d %d]的路由",\
				msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId);
			AnsMsg(msg,ERR_DREBROUTE,"路由错误");
			return;
		}
		if (0 == dreb.nStep)
		{
			//怎么可能呢，若是本节点应该在上面才对
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"路由错误 [%d %d %d %d] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d]  业务数据长度[%d]",\
				msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
				msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
				GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
				msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
				msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
				msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		        msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"路由错误 总线节点路由出错 当前节点[%d %d] 路由目标节点[%d %d] nStep[%d]",\
				m_pRes->g_nDrebId,m_pRes->g_nDrebPrivateId,dreb.nNodeId,dreb.cNodePrivateId,dreb.nStep);
			AnsMsg(msg,ERR_DREBROUTE,"路由错误");
			return ;
		}
		if (m_log->isWrite(LOG_DEBUG))
		{
			m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"将数据发给路由处理 [%d %s] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d]  业务数据长度[%d]",\
				dreb.nIndex,GetIndexFlag(dreb.nIndex).c_str(),\
				GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
				msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
				msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
				msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		        msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
		}
		
		if (!dreb.bIsClose)
		{
			m_pSocketMgr->at(dreb.nIndex)->SendMsg(msg);
		}
		else 
		{
			if (m_log->isWrite(LOG_WARNNING))
			{
				m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"路由连接已断开，暂缓发送[%d %d %d %d] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d]  业务数据长度[%d]",\
					msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
					msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
					GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
					msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
					msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
					msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		        msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
			}
			
			if (dreb.pSendQueue->PutSendMsg(msg) !=0 )
			{
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"入发送队列失败");
			}
		}
		return ;
	}
	else
	{
		RouteMsgRequestBySvr(msg,false);
	}
}

//DREBMONI_INFO
void CMsgProcThread::Monitor_info(S_DREB_RSMSG *msg)
{
	msg->message.head.cRaflag = 1;
	msg->message.head.cDrebAffirm = 0;
	msg->message.head.cZip = 0;
	msg->message.head.nLen = 0;
	msg->message.head.cNextFlag = 0;
	
	msg->message.head.a_Ainfo.a_cNodePrivateId = m_pRes->g_nDrebPrivateId;	
	msg->message.head.a_Ainfo.a_nNodeId = m_pRes->g_nDrebId;
	msg->message.head.a_Ainfo.a_nRetCode = SUCCESS;

	CBF_Xml xmlpack;
	xmlpack.SetNodeValueByPath("Monitor/公共节点",false,m_pRes->g_nDrebId);
	xmlpack.SetNodeValueByPath("Monitor/私有节点",false,m_pRes->g_nDrebPrivateId);
	xmlpack.SetNodeValueByPath("Monitor/启动时间",false,m_pRes->g_pDrebStatInfo.m_sStartDate,false);
	//取所有注册的服务，在本dreb上注册的
	std::vector<S_SVR_ROUTE>svrlist;
	m_pMemDb->GetAllSvr(svrlist);
	for (unsigned int i=0 ; i<svrlist.size() ; i++)
	{
		CXmlNode *svrnode = xmlpack.AddNodeByPath("Monitor/服务列表",false,"服务",false,"");
		if (svrnode != NULL)
		{
			svrnode->SetAttribute("公共服务号",false,(int)svrlist[i].nSvrMainId);
			svrnode->SetAttribute("私有服务号",false,(int)svrlist[i].cSvrPrivateId);
			svrnode->SetAttribute("状态",false,(int)svrlist[i].bIsClose);
			svrnode->SetAttribute("连接",false,(int)svrlist[i].nIndex);
		}
	}
	int len = DREBDATASIZE;
	if (!xmlpack.ToBuffer(msg->message.buffer,len))
	{
		sprintf(msg->message.buffer,"缓冲不足，请下载dreb.txt查看");
		msg->message.head.nLen = strlen(msg->message.buffer);
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"缓冲不足");
		xmlpack.ToFile("dreb.txt");
	}
	else
	{
		msg->message.head.nLen = len;
	}

	
	if (m_pSocketMgr->at(msg->msghead.index)->SendMsg(msg)!=0)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送应答出错");
	}
	return ;
	
}
//DREBMONI_HOST
void CMsgProcThread::Monitor_Host(S_DREB_RSMSG *msg)
{
	msg->message.head.cRaflag = 1;
	msg->message.head.cDrebAffirm = 0;
	msg->message.head.cZip = 0;
	msg->message.head.nLen = 0;
	msg->message.head.cNextFlag = 0;
	
	msg->message.head.a_Ainfo.a_cNodePrivateId = m_pRes->g_nDrebPrivateId;	
	msg->message.head.a_Ainfo.a_nNodeId = m_pRes->g_nDrebId;
	msg->message.head.a_Ainfo.a_nRetCode = SUCCESS;
	
	CBF_Xml xmlpack;
	xmlpack.SetNodeValueByPath("Monitor/公共节点",false,m_pRes->g_nDrebId);
	xmlpack.SetNodeValueByPath("Monitor/私有节点",false,m_pRes->g_nDrebPrivateId);
	xmlpack.SetNodeValueByPath("Monitor/启动时间",false,m_pRes->g_pDrebStatInfo.m_sStartDate,false);

	int total;
	int unused;
	m_pMemPool->PoolInfo(total,unused);
	xmlpack.SetNodeAttribute("Monitor/内存缓冲池",false,"总分配",false,total );
	xmlpack.SetNodeAttribute("Monitor/内存缓冲池",false,"未使用",false,unused );
	xmlpack.SetNodeAttribute("Monitor/内存缓冲池",false,"每块大小",false,(int)(sizeof(DREBQUEUEMSG_HEAD)+DREBBUFSIZE+1) );
	//取主机信息
	S_MONITOR_HOST hf;
	V_MonitorDiskInfo di;
	g_pDrebTimer.GetHostInfo(hf,di);
	
	xmlpack.SetNodeValueByPath("Monitor/主机资源/CPU",false,(int)hf.nCpuRate);
	

	xmlpack.SetNodeValueByPath("Monitor/主机资源/内存",false,"",false);
	xmlpack.SetNodeAttribute("Monitor/主机资源/内存",false,"总大小M",false,(int)hf.nTotalMemory);
	xmlpack.SetNodeAttribute("Monitor/主机资源/内存",false,"使用M",false,(int)hf.nUsedMemory);
	
	for (unsigned int i=0 ; i<di.size() ; i++)
	{
		CXmlNode *disknode = xmlpack.AddNodeByPath("Monitor/主机资源/磁盘列表",false,"磁盘",false,"");
		if (disknode != NULL)
		{
			disknode->SetAttribute("名称",false,di[i].sDiskName);
			disknode->SetAttribute("总大小M",false,(int)di[i].nTotalSpace);
			disknode->SetAttribute("使用大小M",false,(int)di[i].nUsedSpace);
		}
	}
	int len = DREBDATASIZE;
	if (!xmlpack.ToBuffer(msg->message.buffer,len))
	{
		sprintf(msg->message.buffer,"缓冲不足，请下载host.txt查看");
		msg->message.head.nLen = strlen(msg->message.buffer);
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"缓冲不足");
		xmlpack.ToFile("host.txt");
	}
	else
	{
		msg->message.head.nLen = len;
	}

	if (m_pSocketMgr->at(msg->msghead.index)->SendMsg(msg)!=0)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送应答出错");
	}
	
	return ;
	
}
//DREBMONI_CURTXLIST
void CMsgProcThread::Monitor_CurTxList(S_DREB_RSMSG *msg)
{
	msg->message.head.cRaflag = 1;
	msg->message.head.cDrebAffirm = 0;
	msg->message.head.cZip = 0;
	msg->message.head.nLen = 0;
	msg->message.head.cNextFlag = 0;
	
	msg->message.head.a_Ainfo.a_cNodePrivateId = m_pRes->g_nDrebPrivateId;	
	msg->message.head.a_Ainfo.a_nNodeId = m_pRes->g_nDrebId;
	msg->message.head.a_Ainfo.a_nRetCode = SUCCESS;
	

	//取所有交易
	std::vector<S_SERVICE_ROUTE *> slist;
	m_pMemDb->GetAllServiceOrder(slist);
	CBF_Xml monixml;
	monixml.SetNodeValueByPath("Monitor/公共节点",false,m_pRes->g_nDrebId);
	monixml.SetNodeValueByPath("Monitor/私有节点",false,m_pRes->g_nDrebPrivateId);
	monixml.SetNodeValueByPath("Monitor/启动时间",false,m_pRes->g_pDrebStatInfo.m_sStartDate,false);
	monixml.SetNodeValueByPath("Monitor/服务列表",false,"",false);
	char tmpchar[30];
	int nSvrMainId=0;
	int cSvrPrivateId=0;


	char svrname[40];
	char txnodename[100];
	bzero(svrname,sizeof(svrname));
	bzero(txnodename,sizeof(txnodename));
	for (unsigned int i=0 ; i<slist.size() ; i++)
	{
		if (slist[i]->nStep  != 0) //去掉非本地的交易信息
		{
			continue;
		}
		if (nSvrMainId != slist[i]->nSvrMainId || cSvrPrivateId != slist[i]->cSvrPrivateId)
		{
			//一个新的服务
			sprintf(svrname,"%d_%d",slist[i]->nSvrMainId,slist[i]->cSvrPrivateId);
			monixml.AddNodeByPath("Monitor/服务列表",false,svrname,false,"",false);
			nSvrMainId = slist[i]->nSvrMainId ;
			cSvrPrivateId = slist[i]->cSvrPrivateId;
		}
		sprintf(txnodename,"Monitor/服务列表/%s",svrname);
		sprintf(tmpchar,"%d",slist[i]->nFuncNo);
		monixml.AddNodeByPath(txnodename,false,tmpchar,false,"",false);
		
	}
	int ret = DREBDATASIZE;
	if (!monixml.ToBuffer(msg->message.buffer,ret))
	{
		sprintf(msg->message.buffer,"交易数%d太多，缓冲不足，请下载curtxlist.txt查看",slist.size());
		msg->message.head.nLen = strlen(msg->message.buffer);
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"交易数太多，缓冲不足");
		monixml.ToFile("curtxlist.txt");

	}
	else
	{
		msg->message.head.nLen = ret;
	}
	if (m_pSocketMgr->at(msg->msghead.index)->SendMsg(msg)!=0)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送应答出错");
	}
	
	return ;
	
}
//DREBMONI_ROUTEDREB
void CMsgProcThread::Monitor_RouteDreb(S_DREB_RSMSG *msg)
{
	msg->message.head.cRaflag = 1;
	msg->message.head.cDrebAffirm = 0;
	msg->message.head.cZip = 0;
	msg->message.head.nLen = 0;
	msg->message.head.cNextFlag = 0;
	
	msg->message.head.a_Ainfo.a_cNodePrivateId = m_pRes->g_nDrebPrivateId;	
	msg->message.head.a_Ainfo.a_nNodeId = m_pRes->g_nDrebId;
	msg->message.head.a_Ainfo.a_nRetCode = SUCCESS;
	
	CBF_Xml xmlpack;
	xmlpack.SetNodeValueByPath("Monitor/公共节点",false,m_pRes->g_nDrebId);
	xmlpack.SetNodeValueByPath("Monitor/私有节点",false,m_pRes->g_nDrebPrivateId);
	xmlpack.SetNodeValueByPath("Monitor/启动时间",false,m_pRes->g_pDrebStatInfo.m_sStartDate,false);
	xmlpack.SetNodeValueByPath("Monitor/路由表",false,"",false);
	unsigned int i;
	std::vector<S_DREB_ROUTE *>dreblist;
	
	//取所有直连的节点信息
	m_pMemDb->GetAllDreb(dreblist);

	//取所有非直连的DREB路由
	m_pMemDb->GetRouteList(dreblist);

	for (i=0 ; i<dreblist.size() ; i++)
	{
		CXmlNode *routenode = xmlpack.AddNodeByPath("Monitor/路由表",false,"路由",false,"");
		if (routenode != NULL)
		{
			routenode->SetAttribute("公共节点",false,(int)dreblist[i]->nNodeId);
			routenode->SetAttribute("私有节点",false,(int)dreblist[i]->cNodePrivateId);
			routenode->SetAttribute("连接",false,(int)dreblist[i]->nIndex);
			routenode->SetAttribute("步进",false,(int)dreblist[i]->nStep);
			routenode->SetAttribute("带宽",false,(int)dreblist[i]->nbandwidth);
		}
	}


	int ret = DREBDATASIZE;
	if (!xmlpack.ToBuffer(msg->message.buffer,ret))
	{
		sprintf(msg->message.buffer,"缓冲不足，请下载drebroute.txt查看");
		msg->message.head.nLen = strlen(msg->message.buffer);
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"缓冲不足");
		xmlpack.ToFile("drebroute.txt");
		
	}
	else
	{
		msg->message.head.nLen = ret;
	}
	if (m_pSocketMgr->at(msg->msghead.index)->SendMsg(msg)!=0)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送应答出错");
	}
	
	return ;
	
}
//DREBMONI_ROUTESERVICE
void CMsgProcThread::Monitor_RouteService(S_DREB_RSMSG *msg)
{
	msg->message.head.cRaflag = 1;
	msg->message.head.cDrebAffirm = 0;
	msg->message.head.cZip = 0;
	msg->message.head.nLen = 0;
	msg->message.head.cNextFlag = 0;
	
	msg->message.head.a_Ainfo.a_cNodePrivateId = m_pRes->g_nDrebPrivateId;	
	msg->message.head.a_Ainfo.a_nNodeId = m_pRes->g_nDrebId;
	msg->message.head.a_Ainfo.a_nRetCode = SUCCESS;
	
	CBF_Xml xmlpack;
	xmlpack.SetNodeValueByPath("Monitor/公共节点",false,m_pRes->g_nDrebId);
	xmlpack.SetNodeValueByPath("Monitor/私有节点",false,m_pRes->g_nDrebPrivateId);
	xmlpack.SetNodeValueByPath("Monitor/启动时间",false,m_pRes->g_pDrebStatInfo.m_sStartDate,false);
//	xmlpack.SetNodeValueByPath("Monitor/交易路由表",false,"",false);
	
	unsigned int i;
	std::string routemsg;
	char tmpchar[DREBBUFSIZE];
	//取所有交易
	std::vector<S_SERVICE_ROUTE *> slist;
	m_pMemDb->GetAllServiceOrder(slist);

	sprintf(tmpchar,"\n  DREB SERVICE LIST,  TOTAL SERVICE:%d \n",slist.size());
	routemsg = routemsg+tmpchar;
	int index;
	int nodeid;
	int nodeprivateid;
	int svrid;
	int svrprivateid;
	
	for (i=0;i<slist.size();i++)
	{
		if (i == 0)
		{
			index = slist[i]->nIndex;
			nodeid = slist[i]->nNodeId;
			nodeprivateid = slist[i]->cNodePrivateId;
			svrid = slist[i]->nSvrMainId;
			svrprivateid = slist[i]->cSvrPrivateId;
			sprintf(tmpchar,"    INDEX=%d DREB[%d %d] SVR[%d %d] STEP=%d FUNC=",index,nodeid,nodeprivateid,svrid,svrprivateid,slist[i]->nStep);
			routemsg= routemsg+tmpchar;
		}
		if (index != slist[i]->nIndex || nodeid != slist[i]->nNodeId  || nodeprivateid != slist[i]->cNodePrivateId \
			|| svrid != slist[i]->nSvrMainId || svrprivateid != slist[i]->cSvrPrivateId)
		{
			index = slist[i]->nIndex;
			nodeid = slist[i]->nNodeId;
			nodeprivateid = slist[i]->cNodePrivateId;
			svrid = slist[i]->nSvrMainId;
			svrprivateid = slist[i]->cSvrPrivateId;
			sprintf(tmpchar,"\n    INDEX=%d DREB[%d %d] SVR[%d %d] STEP=%d FUNC=",index,nodeid,nodeprivateid,svrid,svrprivateid,slist[i]->nStep);
			routemsg= routemsg+tmpchar;
		}
		//sprintf(tmpchar," %d-i%d",servicelist[i]->nFuncNo,servicelist[i]->nIndex);
		sprintf(tmpchar," %d",slist[i]->nFuncNo);
		routemsg= routemsg+tmpchar;
	}
	routemsg= routemsg+"\n";

	xmlpack.AddNodeByPath("Monitor/",false,"交易路由表",false,routemsg.c_str(),false);

	int ret = DREBDATASIZE;
	if (!xmlpack.ToBuffer(msg->message.buffer,ret))
	{
		sprintf(msg->message.buffer,"缓冲不足,请下载txroute.txt文件查看");
		msg->message.head.nLen = strlen(msg->message.buffer);
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"缓冲不足");
		xmlpack.ToFile("txroute.txt");
		
	}
	else
	{
		msg->message.head.nLen = ret;
	}
	if (m_pSocketMgr->at(msg->msghead.index)->SendMsg(msg)!=0)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送应答出错");
	}
	return;
	
}
//DREBMONI_REPORT
void CMsgProcThread::Monitor_Report(S_DREB_RSMSG *msg)
{
	msg->message.head.cRaflag = 1;
	msg->message.head.cDrebAffirm = 0;
	msg->message.head.cZip = 0;
	msg->message.head.nLen = 0;
	msg->message.head.cNextFlag = 0;
	
	msg->message.head.a_Ainfo.a_cNodePrivateId = m_pRes->g_nDrebPrivateId;	
	msg->message.head.a_Ainfo.a_nNodeId = m_pRes->g_nDrebId;
	msg->message.head.a_Ainfo.a_nRetCode = SUCCESS;
	

	CBF_Xml xmlpack;
	xmlpack.SetNodeValueByPath("Monitor/公共节点",false,m_pRes->g_nDrebId);
	xmlpack.SetNodeValueByPath("Monitor/私有节点",false,m_pRes->g_nDrebPrivateId);
	xmlpack.SetNodeValueByPath("Monitor/启动时间",false,m_pRes->g_pDrebStatInfo.m_sStartDate,false);
	xmlpack.SetNodeValueByPath("Monitor/运行统计",false,"",false);

	xmlpack.AddNodeByPath("Monitor/运行统计",false,"运行日期",false,m_pRes->g_pDrebStatInfo.m_sStartDate,false);
	char tmpchar[100];
	sprintf(tmpchar,INT64PFM,g_pDrebTimer.m_sResultDrebInfo.m_nRcvPackClient);
	xmlpack.AddNodeByPath("Monitor/运行统计/平均每秒",false,"接收客户端数据包数",false,tmpchar,false);

	sprintf(tmpchar,INT64PFM,g_pDrebTimer.m_sResultDrebInfo.m_nRcvByteClient);
	xmlpack.AddNodeByPath("Monitor/运行统计/平均每秒",false,"接收客户端字节数",false,tmpchar,false);

	sprintf(tmpchar,INT64PFM,g_pDrebTimer.m_sResultDrebInfo.m_nAnsPackClient);
	xmlpack.AddNodeByPath("Monitor/运行统计/平均每秒",false,"应答客户端数据包数",false,tmpchar,false);

	sprintf(tmpchar,INT64PFM,g_pDrebTimer.m_sResultDrebInfo.m_nAnsByteClient);
	xmlpack.AddNodeByPath("Monitor/运行统计/平均每秒",false,"应答客户端字节数",false,tmpchar,false);

	sprintf(tmpchar,INT64PFM,g_pDrebTimer.m_sResultDrebInfo.m_nTransOutNum);
	xmlpack.AddNodeByPath("Monitor/运行统计/平均每秒",false,"转出数据包数",false,tmpchar,false);

	sprintf(tmpchar,INT64PFM,g_pDrebTimer.m_sResultDrebInfo.m_nTransOutByte);
	xmlpack.AddNodeByPath("Monitor/运行统计/平均每秒",false,"转出数据字节数",false,tmpchar,false);

	sprintf(tmpchar,INT64PFM,g_pDrebTimer.m_sResultDrebInfo.m_nTransInNum);
	xmlpack.AddNodeByPath("Monitor/运行统计/平均每秒",false,"转入数据包数",false,tmpchar,false);

	sprintf(tmpchar,INT64PFM,g_pDrebTimer.m_sResultDrebInfo.m_nTransInByte);
	xmlpack.AddNodeByPath("Monitor/运行统计/平均每秒",false,"转入数据字节数",false,tmpchar,false);

	sprintf(tmpchar,INT64PFM,g_pDrebTimer.m_sResultDrebInfo.m_nCallSrvNum);
	xmlpack.AddNodeByPath("Monitor/运行统计/平均每秒",false,"调用服务次数",false,tmpchar,false);

	sprintf(tmpchar,INT64PFM,g_pDrebTimer.m_sResultDrebInfo.m_nCallSrvByte);
	xmlpack.AddNodeByPath("Monitor/运行统计/平均每秒",false,"调用服务字节数",false,tmpchar,false);

	sprintf(tmpchar,INT64PFM,g_pDrebTimer.m_sResultDrebInfo.m_nSrvAnsNum);
	xmlpack.AddNodeByPath("Monitor/运行统计/平均每秒",false,"服务应答数",false,tmpchar,false);

	sprintf(tmpchar,INT64PFM,g_pDrebTimer.m_sResultDrebInfo.m_nSrvAnsByte);
	xmlpack.AddNodeByPath("Monitor/运行统计/平均每秒",false,"服务应答字节数",false,tmpchar,false);



	sprintf(tmpchar,INT64PFM,m_pRes->g_pDrebStatInfo.m_drebinfo.m_nRcvPackClient);
	xmlpack.AddNodeByPath("Monitor/运行统计/累计",false,"接收客户端数据包数",false,tmpchar,false);
	
	sprintf(tmpchar,INT64PFM,m_pRes->g_pDrebStatInfo.m_drebinfo.m_nRcvByteClient);
	xmlpack.AddNodeByPath("Monitor/运行统计/累计",false,"接收客户端字节数",false,tmpchar,false);
	
	sprintf(tmpchar,INT64PFM,m_pRes->g_pDrebStatInfo.m_drebinfo.m_nAnsPackClient);
	xmlpack.AddNodeByPath("Monitor/运行统计/累计",false,"应答客户端数据包数",false,tmpchar,false);
	
	sprintf(tmpchar,INT64PFM,m_pRes->g_pDrebStatInfo.m_drebinfo.m_nAnsByteClient);
	xmlpack.AddNodeByPath("Monitor/运行统计/累计",false,"应答客户端字节数",false,tmpchar,false);
	
	sprintf(tmpchar,INT64PFM,m_pRes->g_pDrebStatInfo.m_drebinfo.m_nTransOutNum);
	xmlpack.AddNodeByPath("Monitor/运行统计/累计",false,"转出数据包数",false,tmpchar,false);
	
	sprintf(tmpchar,INT64PFM,m_pRes->g_pDrebStatInfo.m_drebinfo.m_nTransOutByte);
	xmlpack.AddNodeByPath("Monitor/运行统计/累计",false,"转出数据字节数",false,tmpchar,false);
	
	sprintf(tmpchar,INT64PFM,m_pRes->g_pDrebStatInfo.m_drebinfo.m_nTransInNum);
	xmlpack.AddNodeByPath("Monitor/运行统计/累计",false,"转入数据包数",false,tmpchar,false);
	
	sprintf(tmpchar,INT64PFM,m_pRes->g_pDrebStatInfo.m_drebinfo.m_nTransInByte);
	xmlpack.AddNodeByPath("Monitor/运行统计/累计",false,"转入数据字节数",false,tmpchar,false);
	
	sprintf(tmpchar,INT64PFM,m_pRes->g_pDrebStatInfo.m_drebinfo.m_nCallSrvNum);
	xmlpack.AddNodeByPath("Monitor/运行统计/累计",false,"调用服务次数",false,tmpchar,false);
	
	sprintf(tmpchar,INT64PFM,m_pRes->g_pDrebStatInfo.m_drebinfo.m_nCallSrvByte);
	xmlpack.AddNodeByPath("Monitor/运行统计/累计",false,"调用服务字节数",false,tmpchar,false);
	
	sprintf(tmpchar,INT64PFM,m_pRes->g_pDrebStatInfo.m_drebinfo.m_nSrvAnsNum);
	xmlpack.AddNodeByPath("Monitor/运行统计/累计",false,"服务应答数",false,tmpchar,false);
	
	sprintf(tmpchar,INT64PFM,m_pRes->g_pDrebStatInfo.m_drebinfo.m_nSrvAnsByte);
	xmlpack.AddNodeByPath("Monitor/运行统计/累计",false,"服务应答字节数",false,tmpchar,false);

	int ret = DREBDATASIZE;
	if (!xmlpack.ToBuffer(msg->message.buffer,ret))
	{
		sprintf(msg->message.buffer,"缓冲不足,请下载drebstat.txt文件查看");
		msg->message.head.nLen = strlen(msg->message.buffer);
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"缓冲不足");
		xmlpack.ToFile("drebstat.txt");
		
	}
	else
	{
		msg->message.head.nLen = ret;
	}
	if (m_pSocketMgr->at(msg->msghead.index)->SendMsg(msg)!=0)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送应答出错");
	}

	return ;
	
}
//DREBMONI_CONNINFO
void CMsgProcThread::Monitor_ConnInfo(S_DREB_RSMSG *msg)
{
	msg->message.head.cRaflag = 1;
	msg->message.head.cDrebAffirm = 0;
	msg->message.head.cZip = 0;
	msg->message.head.nLen = 0;
	msg->message.head.cNextFlag = 0;
	
	msg->message.head.a_Ainfo.a_cNodePrivateId = m_pRes->g_nDrebPrivateId;	
	msg->message.head.a_Ainfo.a_nNodeId = m_pRes->g_nDrebId;
	msg->message.head.a_Ainfo.a_nRetCode = SUCCESS;
	
	CBF_Xml xmlpack;
	xmlpack.SetNodeValueByPath("Monitor/公共节点",false,m_pRes->g_nDrebId);
	xmlpack.SetNodeValueByPath("Monitor/私有节点",false,m_pRes->g_nDrebPrivateId);
	xmlpack.SetNodeValueByPath("Monitor/启动时间",false,m_pRes->g_pDrebStatInfo.m_sStartDate,false);
	xmlpack.SetNodeValueByPath("Monitor/连接信息",false,"",false);
	unsigned int i;
	int svrconnnum=0;
	int drebnum=0;
	int cliconnnum=0;
	for ( i=m_pRes->g_pCenterList.size()+1 ; i<m_pRes->g_nMaxConnCount ; i++)
	{
		if (m_pSocketMgr->at(i)->m_sock != INVALID_SOCKET && m_pSocketMgr->at(i)->m_bChecked)
		{
			if (m_pSocketMgr->at(i)->m_nType == SOCK_DREB)
			{
				drebnum++;
			}
			if (m_pSocketMgr->at(i)->m_nType == SOCK_SRV)
			{
				svrconnnum++;
			}
			if (m_pSocketMgr->at(i)->m_nType == SOCK_CLI)
			{
				cliconnnum++;
			}
		}
	}
	xmlpack.SetNodeAttribute("Monitor/连接信息",false,"连接过来的DREB数",false,drebnum);
	xmlpack.SetNodeAttribute("Monitor/连接信息",false,"客户端数目",false,cliconnnum);
	xmlpack.SetNodeAttribute("Monitor/连接信息",false,"服务数目",false,svrconnnum);
	for ( i=0 ; i<m_pRes->g_pCenterList.size() ; i++)
	{
		CXmlNode *tmpnode = xmlpack.AddNodeByPath("Monitor/连接信息",false,"DREB连接",false,"",false);
		if (tmpnode != NULL)
		{
			tmpnode->SetAttribute("ip",false,m_pSocketMgr->at(i+1)->m_s_center.sAddress,false);
			tmpnode->SetAttribute("port",false,(int)m_pSocketMgr->at(i+1)->m_s_center.nPort);
			if (m_pSocketMgr->at(i+1)->m_s_center.bNeedConnect)
			{
				tmpnode->SetAttribute("连接状态",false,"需要连接",false);
			}
			else
			{
				tmpnode->SetAttribute("连接状态",false,"正常",false);
			}
			tmpnode->SetAttribute("对端公共节点",false,(int)m_pSocketMgr->at(i+1)->m_s_center.nNodeId);
			tmpnode->SetAttribute("对端私有节点",false,(int)m_pSocketMgr->at(i+1)->m_s_center.cNodePrivateId);
			tmpnode->SetAttribute("带宽",false,m_pSocketMgr->at(i+1)->m_s_center.nbandwidth);
		}
	}
	
	int ret = DREBDATASIZE;
	if (!xmlpack.ToBuffer(msg->message.buffer,ret))
	{
		sprintf(msg->message.buffer,"缓冲不足");
		msg->message.head.nLen = strlen(msg->message.buffer);
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"缓冲不足");
		xmlpack.ToFile("connstatus.txt");
		
	}
	else
	{
		msg->message.head.nLen = ret;
	}
	if (m_pSocketMgr->at(msg->msghead.index)->SendMsg(msg)!=0)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送应答出错");
	}
	
	return ;
	
}

void CMsgProcThread::Monitor_ReportStart(S_DREB_RSMSG *msg)
{
	msg->message.head.cRaflag = 1;
	msg->message.head.cDrebAffirm = 0;
	msg->message.head.cZip = 0;
	msg->message.head.nLen = 0;
	msg->message.head.cNextFlag = 0;
	
	msg->message.head.a_Ainfo.a_cNodePrivateId = m_pRes->g_nDrebPrivateId;	
	msg->message.head.a_Ainfo.a_nNodeId = m_pRes->g_nDrebId;
	msg->message.head.a_Ainfo.a_nRetCode = SUCCESS;
	
	if (g_pDrebTimer.IsStoped())
	{
		if (g_pDrebTimer.StartTimer())
		{
			//已启动DREB监控定时器
			sprintf(msg->message.buffer,"启动DREB监控定时器成功");
			msg->message.head.nLen = strlen(msg->message.buffer);
		}
		else
		{
			msg->message.head.a_Ainfo.a_nRetCode = ERR_START_DREBTIMER;
			sprintf(msg->message.buffer,"启动DREB监控定时器失败");
			msg->message.head.nLen = strlen(msg->message.buffer);
		}
	}
	else
	{
		msg->message.head.a_Ainfo.a_nRetCode = ERR_START_DREBTIMER;
		sprintf(msg->message.buffer,"DREB监控定时器已经运行,无法再启");
		msg->message.head.nLen = strlen(msg->message.buffer);
	}
	
	if (m_pSocketMgr->at(msg->msghead.index)->SendMsg(msg)!=0)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送应答出错");
	}
	
	return ;
	
}

void CMsgProcThread::Monitor_ReportStop(S_DREB_RSMSG *msg)
{
	msg->message.head.cRaflag = 1;
	msg->message.head.cDrebAffirm = 0;
	msg->message.head.cZip = 0;
	msg->message.head.nLen = 0;
	msg->message.head.cNextFlag = 0;
	
	msg->message.head.a_Ainfo.a_cNodePrivateId = m_pRes->g_nDrebPrivateId;	
	msg->message.head.a_Ainfo.a_nNodeId = m_pRes->g_nDrebId;
	msg->message.head.a_Ainfo.a_nRetCode = SUCCESS;
	
	if (g_pDrebTimer.IsStoped())
	{
		msg->message.head.a_Ainfo.a_nRetCode = ERR_STOP_DREBTIMER;
		sprintf(msg->message.buffer,"DREB监控定时器已经停止,无法再停");
		msg->message.head.nLen = strlen(msg->message.buffer);
	}
	else
	{
		if (g_pDrebTimer.StopTimer())
		{
			//已启动DREB监控定时器
			sprintf(msg->message.buffer,"停止DREB监控定时器成功");
			msg->message.head.nLen = strlen(msg->message.buffer);
		}
		else
		{
			msg->message.head.a_Ainfo.a_nRetCode = ERR_STOP_DREBTIMER;
			sprintf(msg->message.buffer,"停止DREB监控定时器失败");
			msg->message.head.nLen = strlen(msg->message.buffer);
		}
	}
	
	if (m_pSocketMgr->at(msg->msghead.index)->SendMsg(msg)!=0)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送应答出错");
	}
	return ;
	
}
void CMsgProcThread::LogDrebHead(int loglevel, DREB_HEAD head, const char *msg, const char *filename, int fileline)
{
	bzero(m_headMsg,sizeof(m_headMsg));
	sprintf(m_headMsg,"%s DREBHEAD信息 cZip=%d cCmd=%s cRaflag=%d cNextFlag=%d cDrebAffirm=%d s_nNodeId=%d \
	   s_cNodePrivateId=%d s_nSvrMainId=%d s_cSvrPrivateId=%d s_nHook=%d s_nSerial=%d \
	   s_nDrebSerial=%d s_nIndex=%d d_nNodeId=%d d_cNodePrivateId=%d d_nSvrMainId=%d \
	   d_cSvrPrivateId=%d d_nServiceNo=%d a_nNodeId=%d a_cNodePrivateId=%d a_nRetCode=%d \
	   n_nNextNo=%d n_nNextOffset=%d b_nSerial=%d b_cIndex=%d nLen=%d","%s",head.cZip,GetDrebCmdType(head.cCmd).c_str(),\
	   head.cRaflag,head.cNextFlag,head.cDrebAffirm,head.s_Sinfo.s_nNodeId,head.s_Sinfo.s_cNodePrivateId,\
	   head.s_Sinfo.s_nSvrMainId,head.s_Sinfo.s_cSvrPrivateId,head.s_Sinfo.s_nHook,head.s_Sinfo.s_nSerial,\
	   head.s_Sinfo.s_nDrebSerial,head.s_Sinfo.s_nIndex,head.d_Dinfo.d_nNodeId,head.d_Dinfo.d_cNodePrivateId,\
	   head.d_Dinfo.d_nSvrMainId,head.d_Dinfo.d_cSvrPrivateId,head.d_Dinfo.d_nServiceNo,\
	   head.a_Ainfo.a_nNodeId,head.a_Ainfo.a_cNodePrivateId,head.a_Ainfo.a_nRetCode,\
	   head.n_Ninfo.n_nNextNo,head.n_Ninfo.n_nNextOffset,head.b_Cinfo.b_nSerial,\
	   head.b_Cinfo.b_cIndex,head.nLen);
	m_log->LogMp(loglevel,filename,fileline,m_headMsg,msg);
}
void CMsgProcThread::TransBroadCast(S_DREB_RSMSG *msg, bool isaffirm)
{
	bool issendaffirm=false;//是否已发送应答
	bool baffrim=false; //是否由本节发送应答
	if (isaffirm && msg->message.head.s_Sinfo.s_nNodeId == m_pRes->g_nDrebId && \
		msg->message.head.s_Sinfo.s_cNodePrivateId == m_pRes->g_nDrebPrivateId)
	{
		baffrim = true;
	}
	S_DREB_RSMSG *data=NULL;
	if (msg->message.head.d_Dinfo.d_nNodeId<1)
	{
		//发送给所有的通讯平台
		std::vector<S_DREB_ROUTE *>sendtlist;
		if (!m_pMemDb->GetAllRouteBc(sendtlist))
		{
			m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"TransBroadCast 取所有要发广播的DREB节点信息出错");
			m_pMemPool->PoolFree(msg);
			return;
		}
		for (unsigned int i=0;i<sendtlist.size();i++)
		{
			data = (S_DREB_RSMSG *) m_pMemPool->PoolMalloc();
			if (data == NULL)
			{
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"内存不足，分配数据总线节点数据空间出错");
				m_pMemPool->PoolFree(msg);
				return;
			}
			memcpy(data,msg,sizeof(S_DREB_RSMSG));
			data->message.head.d_Dinfo.d_nNodeId = sendtlist[i]->nNodeId;
			data->message.head.d_Dinfo.d_cNodePrivateId = sendtlist[i]->cNodePrivateId;
			if (m_log->isWrite(LOG_DEBUG+1))
			{
				m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"发给总线 [%d %d %d %d] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d] 业务数据长度[%d]",\
					data->message.head.d_Dinfo.d_nNodeId,data->message.head.d_Dinfo.d_cNodePrivateId,\
					data->message.head.d_Dinfo.d_nSvrMainId,data->message.head.d_Dinfo.d_cSvrPrivateId,\
					GetDrebCmdType(data->message.head.cCmd).c_str(),data->message.head.cNextFlag,\
					data->message.head.cRaflag,data->message.head.d_Dinfo.d_nServiceNo,data->message.head.s_Sinfo.s_nNodeId,\
					data->message.head.s_Sinfo.s_cNodePrivateId,data->message.head.s_Sinfo.s_nDrebSerial,\
					data->message.head.d_Dinfo.d_nNodeId, data->message.head.d_Dinfo.d_cNodePrivateId,\
					data->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,data->message.head.nLen);
			}
			
			if (m_pSocketMgr->at(sendtlist[i]->nIndex)->SendMsg(data)!=0)
			{
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"TransBroadCast 发送失败");
				m_pMemPool->PoolFree(msg);
				return;
			}
		}

	}
	if (msg->message.head.d_Dinfo.d_nNodeId == m_pRes->g_nDrebId)  //节点相同
	{
		if (msg->message.head.d_Dinfo.d_cNodePrivateId <1 || msg->message.head.d_Dinfo.d_cNodePrivateId == m_pRes->g_nDrebPrivateId) //就是本数据总线节点了
		{
			//发给私有节点相同或不指定私有节点
			if (msg->message.head.d_Dinfo.d_nSvrMainId<1) //没有指定服务
			{
				if (0 != m_pRes->g_nSupportSubscribe)
				{
					 std::vector<S_SUBSCRIBE> subcribelist;
					 if (m_pMemDb->m_subscribe.GetIndex(msg->message.head.d_Dinfo.d_nServiceNo, subcribelist))
					 {
                         S_DREB_RSMSG* data = NULL;
                         for (unsigned int i = 0; i < subcribelist.size(); i++)
                         {
                             data = (S_DREB_RSMSG*)m_pMemPool->PoolMalloc();
                             if (data == NULL)
                             {
                                 m_log->LogMp(LOG_ERROR, __FILE__, __LINE__, "内存不足，分配数据总线节点数据空间出错");
                                 m_pMemPool->PoolFree(msg);
                                 return;
                             }
                             memcpy(data, msg, sizeof(S_DREB_RSMSG));
                          
							 if (m_log->isWrite(LOG_DEBUG + 1))
							 {
								 m_log->LogMp(LOG_DEBUG + 1, __FILE__, __LINE__, "发给服务 [%d %d ]订阅的广播信息 DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d] 业务数据长度[%d]", \
									 subcribelist[i].nSvrMainId, subcribelist[i].cSvrPrivateId, \
									GetDrebCmdType(data->message.head.cCmd).c_str(), data->message.head.cNextFlag, \
									data->message.head.cRaflag, data->message.head.d_Dinfo.d_nServiceNo, data->message.head.s_Sinfo.s_nNodeId, \
									data->message.head.s_Sinfo.s_cNodePrivateId, data->message.head.s_Sinfo.s_nDrebSerial, \
									data->message.head.d_Dinfo.d_nNodeId, data->message.head.d_Dinfo.d_cNodePrivateId, \
									data->message.head.d_Dinfo.d_nSvrMainId, data->message.head.d_Dinfo.d_cSvrPrivateId, data->message.head.nLen);
							 }

							 m_pSocketMgr->at(subcribelist[i].nIndex)->SendMsg(data);
                            
                         }
						 //发给订阅了广播的服务
					 }
				}
				else
				{
                    //发送给所有的服务
					std::vector<S_SVR_ROUTE>svrlist;
                    if (!m_pMemDb->GetAllSvr(svrlist))
                    {
                        m_pMemPool->PoolFree(msg);
                        m_log->LogMp(LOG_ERROR, __FILE__, __LINE__, "TransBroadCast GetAllSvr失败");
                        return;
                    }
                    S_DREB_RSMSG* data = NULL;
                    for (unsigned int i = 0; i < svrlist.size(); i++)
                    {
                        data = (S_DREB_RSMSG*)m_pMemPool->PoolMalloc();
                        if (data == NULL)
                        {
                            m_log->LogMp(LOG_ERROR, __FILE__, __LINE__, "内存不足，分配数据总线节点数据空间出错");
                            m_pMemPool->PoolFree(msg);
                            return;
                        }
                        memcpy(data, msg, sizeof(S_DREB_RSMSG));
                        if (!svrlist[i].bIsClose)
                        {
                            if (m_log->isWrite(LOG_DEBUG + 1))
                            {
                                m_log->LogMp(LOG_DEBUG + 1, __FILE__, __LINE__, "发给服务 [%d %d ] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d] 业务数据长度[%d]", \
                                    svrlist[i].nSvrMainId, svrlist[i].cSvrPrivateId, \
                                    GetDrebCmdType(data->message.head.cCmd).c_str(), data->message.head.cNextFlag, \
                                    data->message.head.cRaflag, data->message.head.d_Dinfo.d_nServiceNo, data->message.head.s_Sinfo.s_nNodeId, \
                                    data->message.head.s_Sinfo.s_cNodePrivateId, data->message.head.s_Sinfo.s_nDrebSerial, \
									data->message.head.d_Dinfo.d_nNodeId, data->message.head.d_Dinfo.d_cNodePrivateId, \
									data->message.head.d_Dinfo.d_nSvrMainId, data->message.head.d_Dinfo.d_cSvrPrivateId, data->message.head.nLen);
                            }

                            m_pSocketMgr->at(svrlist[i].nIndex)->SendMsg(data);
                        }
                        else
                        {
                            m_log->LogMp(LOG_ERROR, __FILE__, __LINE__, "服务连接已断开，暂缓发送[%d %d %d %d] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 服务[%d %d] 目标[%d %d %d %d] 业务数据长度[%d]", \
                                data->message.head.d_Dinfo.d_nNodeId, data->message.head.d_Dinfo.d_cNodePrivateId, \
                                data->message.head.d_Dinfo.d_nSvrMainId, data->message.head.d_Dinfo.d_cSvrPrivateId, \
                                GetDrebCmdType(data->message.head.cCmd).c_str(), data->message.head.cNextFlag, \
                                data->message.head.cRaflag, data->message.head.d_Dinfo.d_nServiceNo, data->message.head.s_Sinfo.s_nNodeId, \
                                data->message.head.s_Sinfo.s_cNodePrivateId, msg->message.head.s_Sinfo.s_nDrebSerial, \
                                svrlist[i].nSvrMainId, svrlist[i].cSvrPrivateId, \
								data->message.head.d_Dinfo.d_nNodeId, data->message.head.d_Dinfo.d_cNodePrivateId, \
								data->message.head.d_Dinfo.d_nSvrMainId, data->message.head.d_Dinfo.d_cSvrPrivateId, data->message.head.nLen);
                            if (svrlist[i].pSendQueue->PutSendMsg(data) != 0)
                            {
                                m_log->LogMp(LOG_ERROR, __FILE__, __LINE__, "入发送队列失败");
                                m_pMemPool->PoolFree(msg);
                                return;
                            }

                        }
                    }
					//发送给所有服务返回
				}
				
				if (baffrim && !issendaffirm)
				{
					AnsMsg(msg,SUCCESS,"SUCCESS");
					issendaffirm = true;
				}
				else
				{
					m_pMemPool->PoolFree(msg);
				}
				//发送给所有服务或订阅了此广播的返回
				return;
			}
			S_SVR_ROUTE svr;
			//指定了公共服务号
			if (msg->message.head.d_Dinfo.d_cSvrPrivateId <1) //未指定私有服务号
			{
				if (!m_pMemDb->SelectASvr(msg->message.head.d_Dinfo.d_nSvrMainId,svr))
				{
					//无此服务
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"无此服务[%d %d %d %d] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d] 业务数据长度[%d]",\
						msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
						msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
						GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
						msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
						msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
						msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
						msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
					AnsMsg(msg,ERR_SVRNOTREGISTER,"无此服务");
					return ;
				}
			}
			else
			{
				if (!m_pMemDb->SelectSvr(msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,svr))
				{
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"无此服务[%d %d %d %d] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d] 业务数据长度[%d]",\
						msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
						msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
						GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
						msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
						msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
						msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
						msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
					AnsMsg(msg,ERR_SVRNOTREGISTER,"无此服务");
					return;
				}
			}
			
			data = (S_DREB_RSMSG *) m_pMemPool->PoolMalloc();
			if (data == NULL)
			{
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"内存不足，分配数据总线节点数据空间出错");
				m_pMemPool->PoolFree(msg);
				return;
			}
			memcpy(&(data->msghead),&(msg->msghead),sizeof(DREBQUEUEMSG_HEAD));
			memcpy(&(data->message),&(msg->message),sizeof(COMMSTRU));
			//转发至服务
			if (!svr.bIsClose)
			{
				if (m_log->isWrite(LOG_DEBUG+1))
				{
					m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"发给服务 [%d %d ] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d] 业务数据长度[%d]",\
						svr.nSvrMainId,svr.cSvrPrivateId,\
						GetDrebCmdType(data->message.head.cCmd).c_str(),data->message.head.cNextFlag,\
						data->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,data->message.head.s_Sinfo.s_nNodeId,\
						data->message.head.s_Sinfo.s_cNodePrivateId,data->message.head.s_Sinfo.s_nDrebSerial,\
						data->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
						data->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,data->message.head.nLen);
				}
				
				m_pSocketMgr->at(svr.nIndex)->SendMsg(data);
			}
			else 
			{
				if (m_log->isWrite(LOG_WARNNING))
				{
					m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"服务连接已断开，暂缓发送[%d %d %d %d] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d] 业务数据长度[%d]",\
						data->message.head.d_Dinfo.d_nNodeId,data->message.head.d_Dinfo.d_cNodePrivateId,\
						data->message.head.d_Dinfo.d_nSvrMainId,data->message.head.d_Dinfo.d_cSvrPrivateId,\
						GetDrebCmdType(data->message.head.cCmd).c_str(),data->message.head.cNextFlag,\
						data->message.head.cRaflag,data->message.head.d_Dinfo.d_nServiceNo,data->message.head.s_Sinfo.s_nNodeId,\
						data->message.head.s_Sinfo.s_cNodePrivateId,data->message.head.s_Sinfo.s_nDrebSerial,\
						data->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
						data->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,data->message.head.nLen);
				}
				
				if (svr.pSendQueue->PutSendMsg(data)!=0)
				{
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"入发送队列失败");
				}
			}
			if (baffrim && !issendaffirm)
			{
				AnsMsg(msg,SUCCESS,"SUCCESS");
				issendaffirm = true;
			}
			else
			{
				m_pMemPool->PoolFree(msg);
			}
			//发送给指定了服务返回
			return ;
		} // end 	if (msg->message.head.d_Dinfo.d_cNodePrivateId <1 || msg->message.head.d_Dinfo.d_cNodePrivateId == m_pRes->g_nDrebPrivateId) //就是本数据总线节点了
		else 
		{
			//不是本数据节点，公共节点号相同但私有不同，进入下面路由转发
			
		}
	}
	else if (msg->message.head.d_Dinfo.d_nNodeId <1)  //没有指定节点
	{
		if (msg->message.head.d_Dinfo.d_cNodePrivateId <1 || msg->message.head.d_Dinfo.d_cNodePrivateId == m_pRes->g_nDrebPrivateId) //就是本数据总线节点了
		{
			//发给私有节点相同或不指定私有节点
			if (msg->message.head.d_Dinfo.d_nSvrMainId<1) //没有指定服务
			{
                if (0 != m_pRes->g_nSupportSubscribe)
                {
                    std::vector<S_SUBSCRIBE> subcribelist;
                    if (m_pMemDb->m_subscribe.GetIndex(msg->message.head.d_Dinfo.d_nServiceNo, subcribelist))
                    {
                        S_DREB_RSMSG* data = NULL;
                        for (unsigned int i = 0; i < subcribelist.size(); i++)
                        {
                            data = (S_DREB_RSMSG*)m_pMemPool->PoolMalloc();
                            if (data == NULL)
                            {
                                m_log->LogMp(LOG_ERROR, __FILE__, __LINE__, "内存不足，分配数据总线节点数据空间出错");
                                m_pMemPool->PoolFree(msg);
                                return;
                            }
                            memcpy(data, msg, sizeof(S_DREB_RSMSG));

                            if (m_log->isWrite(LOG_DEBUG + 1))
                            {
                                m_log->LogMp(LOG_DEBUG + 1, __FILE__, __LINE__, "发给服务 [%d %d ]订阅的广播信息 DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d] 业务数据长度[%d]", \
                                    subcribelist[i].nSvrMainId, subcribelist[i].cSvrPrivateId, \
                                    GetDrebCmdType(data->message.head.cCmd).c_str(), data->message.head.cNextFlag, \
                                    data->message.head.cRaflag, data->message.head.d_Dinfo.d_nServiceNo, data->message.head.s_Sinfo.s_nNodeId, \
                                    data->message.head.s_Sinfo.s_cNodePrivateId, data->message.head.s_Sinfo.s_nDrebSerial, \
									data->message.head.d_Dinfo.d_nNodeId, data->message.head.d_Dinfo.d_cNodePrivateId, \
									data->message.head.d_Dinfo.d_nSvrMainId, data->message.head.d_Dinfo.d_cSvrPrivateId, data->message.head.nLen);
                            }

                            m_pSocketMgr->at(subcribelist[i].nIndex)->SendMsg(data);

                        }
                        //发给订阅了广播的服务
                    }
                }
                else
                {
					//发送给所有的服务
					std::vector<S_SVR_ROUTE>svrlist;
					if (!m_pMemDb->GetAllSvr(svrlist))
					{
						m_pMemPool->PoolFree(msg);
						m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"TransBroadCast GetAllSvr失败");
						return;
					}
					S_DREB_RSMSG *data= NULL;
					for (unsigned int i=0;i<svrlist.size();i++)
					{
						data  = (S_DREB_RSMSG *) m_pMemPool->PoolMalloc();
						if (data == NULL)
						{
							m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"内存不足，分配数据总线节点数据空间出错");
							m_pMemPool->PoolFree(msg);
							return;
						}
						memcpy(data,msg,sizeof(S_DREB_RSMSG));
						if (!svrlist[i].bIsClose)
						{
							if (m_log->isWrite(LOG_DEBUG+1))
							{
								m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"发给服务 [%d %d ] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d] 业务数据长度[%d]",\
									svrlist[i].nSvrMainId,svrlist[i].cSvrPrivateId,\
									GetDrebCmdType(data->message.head.cCmd).c_str(),data->message.head.cNextFlag,\
									data->message.head.cRaflag,data->message.head.d_Dinfo.d_nServiceNo,data->message.head.s_Sinfo.s_nNodeId,\
									data->message.head.s_Sinfo.s_cNodePrivateId,data->message.head.s_Sinfo.s_nDrebSerial,\
									data->message.head.d_Dinfo.d_nNodeId, data->message.head.d_Dinfo.d_cNodePrivateId,\
									data->message.head.d_Dinfo.d_nSvrMainId, data->message.head.d_Dinfo.d_cSvrPrivateId,data->message.head.nLen);
							}
						
							m_pSocketMgr->at(svrlist[i].nIndex)->SendMsg(data);
						}
						else
						{
							m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"服务连接已断开，暂缓发送[%d %d %d %d] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 服务[%d %d] 目标[%d %d %d %d] 业务数据长度[%d]",\
								data->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
								data->message.head.d_Dinfo.d_nSvrMainId,data->message.head.d_Dinfo.d_cSvrPrivateId,\
								GetDrebCmdType(data->message.head.cCmd).c_str(),data->message.head.cNextFlag,\
								data->message.head.cRaflag,data->message.head.d_Dinfo.d_nServiceNo,data->message.head.s_Sinfo.s_nNodeId,\
								data->message.head.s_Sinfo.s_cNodePrivateId,data->message.head.s_Sinfo.s_nDrebSerial,\
								svrlist[i].nSvrMainId,svrlist[i].cSvrPrivateId,\
								data->message.head.d_Dinfo.d_nNodeId, data->message.head.d_Dinfo.d_cNodePrivateId,\
								data->message.head.d_Dinfo.d_nSvrMainId, data->message.head.d_Dinfo.d_cSvrPrivateId,data->message.head.nLen);
							if (svrlist[i].pSendQueue->PutSendMsg(data)!=0)
							{
								m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"入发送队列失败");
								m_pMemPool->PoolFree(msg);
								return;
							}
						
						}
					}
					//发送给所有服务返回
				}
				if (baffrim && !issendaffirm)
				{
					AnsMsg(msg,SUCCESS,"SUCCESS");
					issendaffirm = true;
				}
				else
				{
					m_pMemPool->PoolFree(msg);
				}
				//发送给所有服务或订阅了此广播的返回
				return;
			}
			S_SVR_ROUTE svr;
			//指定了公共服务号
			if (msg->message.head.d_Dinfo.d_cSvrPrivateId <1) //未指定私有服务号
			{
				if (!m_pMemDb->SelectASvr(msg->message.head.d_Dinfo.d_nSvrMainId,svr))
				{
					//无此服务
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"无此服务[%d %d %d %d] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d] 业务数据长度[%d]",\
						msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
						msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
						GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
						msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
						msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
						msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
						msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
					m_pMemPool->PoolFree(msg);
					return ;
				}
			}
			else
			{
				if (!m_pMemDb->SelectSvr(msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,svr))
				{
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"无此服务[%d %d %d %d] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d] 业务数据长度[%d]",\
						msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
						msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
						GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
						msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
						msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
						msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
						msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
					m_pMemPool->PoolFree(msg);
					return;
				}
			}
			
			data = (S_DREB_RSMSG *) m_pMemPool->PoolMalloc();
			if (data == NULL)
			{
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"内存不足，分配数据总线节点数据空间出错");
				m_pMemPool->PoolFree(msg);
				return;
			}
			memcpy(&(data->msghead),&(msg->msghead),sizeof(DREBQUEUEMSG_HEAD));
			memcpy(&(data->message),&(msg->message),sizeof(COMMSTRU));
			//转发至服务
			if (!svr.bIsClose)
			{
				if (m_log->isWrite(LOG_DEBUG+1))
				{
					m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"发给服务 [%d %d ] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d] 业务数据长度[%d]",\
						svr.nSvrMainId,svr.cSvrPrivateId,\
						GetDrebCmdType(data->message.head.cCmd).c_str(),data->message.head.cNextFlag,\
						data->message.head.cRaflag,data->message.head.d_Dinfo.d_nServiceNo,data->message.head.s_Sinfo.s_nNodeId,\
						data->message.head.s_Sinfo.s_cNodePrivateId,data->message.head.s_Sinfo.s_nDrebSerial,\
						data->message.head.d_Dinfo.d_nNodeId, data->message.head.d_Dinfo.d_cNodePrivateId,\
						data->message.head.d_Dinfo.d_nSvrMainId, data->message.head.d_Dinfo.d_cSvrPrivateId,data->message.head.nLen);
				}
				
				m_pSocketMgr->at(svr.nIndex)->SendMsg(data);
			}
			else 
			{
				if (m_log->isWrite(LOG_WARNNING))
				{
					m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"服务连接已断开，暂缓发送[%d %d %d %d] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d] 业务数据长度[%d]",\
						data->message.head.d_Dinfo.d_nNodeId,data->message.head.d_Dinfo.d_cNodePrivateId,\
						data->message.head.d_Dinfo.d_nSvrMainId,data->message.head.d_Dinfo.d_cSvrPrivateId,\
						GetDrebCmdType(data->message.head.cCmd).c_str(),data->message.head.cNextFlag,\
						data->message.head.cRaflag,data->message.head.d_Dinfo.d_nServiceNo,data->message.head.s_Sinfo.s_nNodeId,\
						data->message.head.s_Sinfo.s_cNodePrivateId,data->message.head.s_Sinfo.s_nDrebSerial,\
						data->message.head.d_Dinfo.d_nNodeId, data->message.head.d_Dinfo.d_cNodePrivateId,\
						data->message.head.d_Dinfo.d_nSvrMainId, data->message.head.d_Dinfo.d_cSvrPrivateId,data->message.head.nLen);
				}
				
				if (svr.pSendQueue->PutSendMsg(data)!=0)
				{
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"入发送队列失败");
				}
			}
			if (baffrim && !issendaffirm)
			{
				AnsMsg(msg,SUCCESS,"SUCCESS");
				issendaffirm = true;
			}
			else
			{
				m_pMemPool->PoolFree(msg);
			}
			return ;
		} // end if (msg->message.head.d_Dinfo.d_cNodePrivateId <1 || msg->message.head.d_Dinfo.d_cNodePrivateId == m_pRes->g_nDrebPrivateId) //就是本数据总线节点了
		//指定了私有节点，因为前面已转发给各dreb节点，所有这里直接返回
		m_log->LogMp(LOG_PROMPT,__FILE__,__LINE__,"公共节点不相同且未指定公共节点，但指定了私有节点，不为本节点处理，本节点处理结束");
		m_pMemPool->PoolFree(msg);
		return ;
	}
	//通过路由转发
	S_DREB_ROUTE dreb;
	if (msg->message.head.d_Dinfo.d_cNodePrivateId <1)
	{
		if (!m_pMemDb->SelectRoute(msg->message.head.d_Dinfo.d_nNodeId,dreb))
		{
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"路由不存在，可能是数据总线节点未注册[%d %d %d %d] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d] 业务数据长度[%d]",\
				msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
				msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
				GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
				msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
				msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
				msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		        msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
			AnsMsg(msg,ERR_DREBROUTE,"路由不存在，可能是数据总线节点未注册");	
			return;
		}
	}
	else
	{
		if (!m_pMemDb->SelectRoute(msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,dreb))
		{
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"路由不存在，可能是数据总线节点未注册[%d %d %d %d] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d] 业务数据长度[%d]",\
				msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
				msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
				GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
				msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
				msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
				msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		        msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
				
			AnsMsg(msg,ERR_DREBROUTE,"路由不存在，可能是数据总线节点未注册");
			return;
		}
	}
	if (dreb.nIndex <1)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"怎么会出现路由指向本身呢[%d %d %d %d] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d] 业务数据长度[%d]",\
			msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
			msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
			GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
			msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
			msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
			msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		        msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
		m_pMemPool->PoolFree(msg);		
		return ;
	}
	//songfree modi 20160607  
// 	if (baffrim && !issendaffirm)
// 	{
		data = (S_DREB_RSMSG *) m_pMemPool->PoolMalloc();
		if (data == NULL)
		{
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"内存不足，分配数据总线节点数据空间出错");
			m_pMemPool->PoolFree(msg);
			return;
		}
		memcpy(&(data->msghead),&(msg->msghead),sizeof(DREBQUEUEMSG_HEAD));
		memcpy(&(data->message),&(msg->message),sizeof(COMMSTRU));
//	}
	//songfree modi 20160607  end 
	if (!dreb.bIsClose)
	{
		if (m_log->isWrite(LOG_DEBUG+1))
		{
			m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"发给总线 [%d %d] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d] 业务数据长度[%d]",\
				dreb.nNodeId,dreb.cNodePrivateId,\
				GetDrebCmdType(data->message.head.cCmd).c_str(),data->message.head.cNextFlag,\
				data->message.head.cRaflag,data->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
				data->message.head.s_Sinfo.s_cNodePrivateId,data->message.head.s_Sinfo.s_nDrebSerial,\
				msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		        msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,data->message.head.nLen);
		}
		
		m_pSocketMgr->at(dreb.nIndex)->SendMsg(data);
	}
	else 
	{
		if (m_log->isWrite(LOG_WARNNING))
		{
			m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"路由连接已断开，暂缓发送[%d %d %d %d] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d] 业务数据长度[%d]",\
				msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
				msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
				GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
				msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
				msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
				msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		        msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
		}
		
		if (dreb.pSendQueue->PutSendMsg(msg)!=0)
		{
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"入发送队列失败");
		}
	}
	if (baffrim && !issendaffirm)
	{
		AnsMsg(msg,SUCCESS,"SUCCESS");
		issendaffirm = true;
	}
	else
	{
		m_pMemPool->PoolFree(msg);
	}
	return ;
}


void CMsgProcThread::TransMsgRequest(S_DREB_RSMSG *msg, bool isaffirm)
{
	RouteMsgRequest(msg,isaffirm);
	return;
}


void CMsgProcThread::OnMsgMonitor(S_DREB_RSMSG *msg)
{
	int i;
	CBF_Xml        pXml;
	pXml.SetNodeValueByPath("Monitor/公共节点",false,m_pRes->g_nDrebId);
	pXml.SetNodeValueByPath("Monitor/私有节点",false,m_pRes->g_nDrebPrivateId);
	pXml.SetNodeValueByPath("Monitor/节点类型",false,0);//dreb
	pXml.SetNodeValueByPath("Monitor/报告类型",false,0);//正常
	pXml.SetNodeValueByPath("Monitor/启动时间",false,m_pRes->g_pDrebStatInfo.m_sStartDate,false);
	
	int total=0;
	int used=0;
	m_pMemPool->PoolInfo(total,used);
	pXml.SetNodeAttribute("Monitor/内存缓冲池",false,"总分配",false,total );
	pXml.SetNodeAttribute("Monitor/内存缓冲池",false,"未使用",false,used );
	pXml.SetNodeAttribute("Monitor/内存缓冲池",false,"每块大小",false,(int)(DREBBUFSIZE+sizeof(DREBQUEUEMSG_HEAD)+1));

	if (m_pRes->g_nMonitorHost == 1)
	{
		S_MONITOR_HOST hostinfo;
		V_MonitorDiskInfo  diskinfo;
		g_pDrebTimer.GetHostInfo(hostinfo,diskinfo);
		pXml.SetNodeValueByPath("Monitor/主机资源/CPU",false,(int)hostinfo.nCpuRate);
		pXml.SetNodeAttribute("Monitor/主机资源/内存",false,"total",false,(int)hostinfo.nTotalMemory);
		pXml.SetNodeAttribute("Monitor/主机资源/内存",false,"used",false,(int)hostinfo.nUsedMemory);
		for (i=0 ; i< diskinfo.size() ; i++)
		{
			CXmlNode *disknode = pXml.AddNodeByPath("Monitor/主机资源/磁盘",false,"磁盘",false,"");
			if (disknode != NULL)
			{
				disknode->SetAttribute("drive",false,diskinfo[i].sDiskName,true);
				disknode->SetAttribute("total",false,(int)diskinfo[i].nTotalSpace);
				disknode->SetAttribute("used",false,(int)diskinfo[i].nUsedSpace);
			}
		}
	}
	//连接信息
	for (i=0 ; i < m_pRes->g_pCenterList.size() ; i++)
	{
		CXmlNode *connnode = pXml.AddNodeByPath("Monitor/连接信息",false,m_pRes->g_pCenterList[i].sname.c_str(),false,"");
		if (connnode == NULL)
		{
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"AddNodeByPath 出错");
			continue;
		}
		connnode->SetAttribute("ip",false,m_pRes->g_pCenterList[i].sIp);
		connnode->SetAttribute("port",false,(int)m_pRes->g_pCenterList[i].nPort);
		connnode->SetAttribute("index",false,i+1);
		connnode->SetAttribute("公共节点",false,m_pSocketMgr->at(i+1)->m_s_center.nNodeId);
		connnode->SetAttribute("私有节点",false,m_pSocketMgr->at(i+1)->m_s_center.cNodePrivateId);
		if (m_pSocketMgr->at(i+1)->m_sock == INVALID_SOCKET)
		{
			connnode->SetAttribute("status",false,"未连接");
		}
		else if (m_pSocketMgr->at(i+1)->m_bChecked)
		{
			connnode->SetAttribute("status",false,"正常");
		}
		else
		{
			connnode->SetAttribute("status",false,"正在连接");
		}
	}
	//DREB信息
// 	<DREB信息>
//     <DREBLIST name="直接连接的dreb列表">
// 		  <DREB index="1" id="" pid=""  />
// 		  <DREB index="1" id="" pid=""  />
// 		  <DREB index="1" id="" pid=""  />
// 		  <DREB index="1" id="" pid=""  />
//     </DREBLIST>
//     <SVRLIST name="注册的服务列表">
// 		  <SVR index="1" id="" pid="" status="" />
// 		  <SVR index="1" id="" pid="" status="" />
//     </SVRLIST>
//  </DREB信息>
	//dreb连接列表
	pXml.AddNodeByPath("Monitor/DREB信息",false,"DREBLIST",false,"");
	for (i=1 ; i< m_pRes->g_nMaxConnCount ; i++)
	{
		if (m_pSocketMgr->at(i)->m_nType == SOCK_DREB)
		{
			CXmlNode *ddnode = pXml.AddNodeByPath("Monitor/DREB信息/DREBLIST",false,"SOCK_DREB",false,"");
			if (ddnode == NULL)
			{
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"AddNodeByPath 出错");
				continue;
			}
			ddnode->SetAttribute("index",false,i);
			ddnode->SetAttribute("id",false,(int)m_pSocketMgr->at(i)->m_s_conn.nNodeId);
			ddnode->SetAttribute("pid",false,(int)m_pSocketMgr->at(i)->m_s_conn.cNodePrivateId);
		}
	}
	//dreb服务列表
	pXml.AddNodeByPath("Monitor/DREB信息",false,"SVRLIST",false,"");
	std::vector<S_SVR_ROUTE>svrlist;
	m_pMemDb->GetAllSvr(svrlist);
	for (i=0 ; i<svrlist.size() ; i++)
	{
		CXmlNode *svrnode = pXml.AddNodeByPath("Monitor/DREB信息/SVRLIST",false,"SVR",false,"");
		if (svrnode == NULL)
		{
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"AddNodeByPath 出错");
			continue;
		}
		svrnode->SetAttribute("index",false,(int)svrlist[i].nIndex);
		svrnode->SetAttribute("id",false,(int)svrlist[i].nSvrMainId);
		svrnode->SetAttribute("pid",false,(int)svrlist[i].cSvrPrivateId);
		if (svrlist[i].bIsClose)
		{
			svrnode->SetAttribute("status",false,"断开",false);
		}
		else
		{
			svrnode->SetAttribute("status",false,"正常",false);
		}
	}
	int len = DREBDATASIZE;
	if (!pXml.ToBuffer(msg->message.buffer,len))
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"ToBuffer出错");
		m_pMemPool->PoolFree(msg);
		return;
	}
	msg->message.head.nLen = len;

	m_log->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"监控通知",msg->message.buffer,msg->message.head.nLen);

	m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"监控通知 内存分配[%d] 未使用[%d]",total,used);
	msg->message.head.cRaflag = 0;
	msg->message.head.cDrebAffirm = 0;
	msg->message.head.cZip = 0;
	msg->message.head.cNextFlag = 0;

	msg->message.head.cCmd = CMD_DPPUSH;
	msg->message.head.d_Dinfo.d_nNodeId = m_pRes->g_nMonitorDrebId;
	msg->message.head.d_Dinfo.d_cNodePrivateId = m_pRes->g_nMonitorDrebPrivateId;
	msg->message.head.d_Dinfo.d_nSvrMainId = m_pRes->g_nMonitorSvrId;
	msg->message.head.d_Dinfo.d_cSvrPrivateId = m_pRes->g_nMonitorSvrPrivateId;
	msg->message.head.d_Dinfo.d_nServiceNo = m_pRes->g_nMonitorTxCode;
	TransMsgRequest(msg,false);

}

void CMsgProcThread::OnCmdUnReqTx(S_DREB_RSMSG *msg)
{
	
	bool baffrim=false; //是否由本节发送应答
	
	S_DREB_RSMSG *data=NULL;
	if (msg->message.head.d_Dinfo.d_nNodeId<1)
	{
		//发送给所有的通讯平台
		std::vector<S_DREB_ROUTE *>sendtlist;
		if (!m_pMemDb->GetAllRouteBc(sendtlist))
		{
			m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"OnCmdUnReqTx 取所有要发广播的DREB节点信息出错");
			m_pMemPool->PoolFree(msg);
			return;
		}
		for (unsigned int i=0;i<sendtlist.size();i++)
		{
			data = (S_DREB_RSMSG *) m_pMemPool->PoolMalloc();
			if (data == NULL)
			{
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"内存不足，分配数据总线节点数据空间出错");
				m_pMemPool->PoolFree(msg);
				return;
			}
			memcpy(data,msg,sizeof(S_DREB_RSMSG));
			data->message.head.d_Dinfo.d_nNodeId = sendtlist[i]->nNodeId;
			data->message.head.d_Dinfo.d_cNodePrivateId = sendtlist[i]->cNodePrivateId;
			if (m_pSocketMgr->at(sendtlist[i]->nIndex)->SendMsg(data)!=0)
			{
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"OnCmdUnReqTx 发送失败");
				m_pMemPool->PoolFree(msg);
				return;
			}
		}
	}
	if (msg->message.head.d_Dinfo.d_nNodeId == m_pRes->g_nDrebId || msg->message.head.d_Dinfo.d_nNodeId <1)  //节点相同
	{
		S_SERVICE_ROUTE data;
		//取消服务的交易注册
		data.nNodeId = m_pRes->g_nDrebId;
		data.nSvrMainId = msg->message.head.d_Dinfo.d_nSvrMainId;
		data.nFuncNo = msg->message.head.d_Dinfo.d_nServiceNo;
		data.cSvrPrivateId = msg->message.head.d_Dinfo.d_cSvrPrivateId;
		data.cNodePrivateId = m_pRes->g_nDrebPrivateId;
		data.nStep = 0;
		data.nIndex = 0;
		S_SVR_ROUTE svr;
		if (!m_pMemDb->SelectSvr(data.nSvrMainId,data.cSvrPrivateId,svr))
		{
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"取消功能注册时的服务[%d %d]在服务表中不存在",data.nSvrMainId,data.cSvrPrivateId);
			m_pMemPool->PoolFree(msg);
			return ;
		}
		m_log->LogMp(LOG_PROMPT,__FILE__,__LINE__,"取消服务[%d %d] 的交易注册",data.nSvrMainId,data.cSvrPrivateId);
		//取消注册
		m_pMemDb->UnRegisterService(data.nNodeId,data.cNodePrivateId,data.nSvrMainId,data.cSvrPrivateId);
		
		if (!svr.bIsClose)
		{
	    //20130304 songfree add 服务连接时，将服务加入到交易路由表，供按服务路由，交易码默认为0
			S_SERVICE_ROUTE rt;
			bzero(&rt,sizeof(S_SERVICE_ROUTE));
			rt.cNodePrivateId = m_pRes->g_nDrebPrivateId;
			rt.cSvrPrivateId = data.cSvrPrivateId;
			rt.nFuncNo = 0;
			rt.nIndex = svr.nIndex;
			rt.nNodeId = m_pRes->g_nDrebId;
			rt.nStep = 0;
			rt.nSvrMainId = data.nSvrMainId;
			rt.nUpdateTime = time(NULL);
			m_pMemDb->AddServiceRoute(rt);
		//ADD END
		}

	}
	m_pMemPool->PoolFree(msg);	
	return ;
}

void CMsgProcThread::RouteMsgRequest(S_DREB_RSMSG *msg,bool isaffirm)
{
	if (msg->message.head.d_Dinfo.d_nNodeId>0)
	{
		 RouteMsgRequestByDreb(msg,isaffirm);
		 return;
	}
	else if (msg->message.head.d_Dinfo.d_nSvrMainId > 0) 
	{
		 RouteMsgRequestBySvr(msg,isaffirm);
		 return;
	}
	else
	{
		RouteMsgRequestByFunc(msg,isaffirm);
		return;
	}
}

void CMsgProcThread::RouteMsgRequestByDreb(S_DREB_RSMSG *msg,bool isaffirm)
{
	bool bRet;
	//首先判断是否本总线节点
	if (msg->message.head.d_Dinfo.d_nNodeId == m_pRes->g_nDrebId)
	{
		//公共节点相同
		if (0 == msg->message.head.d_Dinfo.d_cNodePrivateId || msg->message.head.d_Dinfo.d_cNodePrivateId == m_pRes->g_nDrebPrivateId)
		{
			S_SERVICE_ROUTE rt;
			//公共节点相同，私有节点为0或就是本节点
			if (msg->message.head.d_Dinfo.d_nSvrMainId >0)  //查找本地的服务
			{
				//本地连接的服务
				if (0 == msg->message.head.d_Dinfo.d_cSvrPrivateId )
				{
					if (msg->message.head.d_Dinfo.d_nServiceNo>0)
					{
						//20181019 按总线+交易码+服务查找	
						bRet = m_pMemDb->SelectRouteByDrebFuncSvr(msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.d_Dinfo.d_nSvrMainId,rt);
					}
					else
					{
						bRet = m_pMemDb->SelectLocalBySvr(msg->message.head.d_Dinfo.d_nSvrMainId,rt);
					}
					
				}
				else
				{
					bRet = m_pMemDb->SelectLocalBySvr(msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,rt);
				}
				if (!bRet)
				{
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"路由错误 根据总线节点-服务路由，找不到服务 [%d %d %d %d] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d] 业务数据长度[%d]",\
						msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
						msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
						GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
						msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
						msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
						msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		                msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
					AnsMsg(msg,ERR_SVRNOTREGISTER,"根据总线节点-服务路由，找不到服务");
					return ;
				}
			}
			else  //根据交易码来查找
			{
				
				if (!m_pMemDb->SelectLocalByFunc(msg->message.head.d_Dinfo.d_nServiceNo,rt))
				{
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"路由错误 根据总线节点路由，找不到交易码 [%d %d %d %d] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d] 业务数据长度[%d]",\
						msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
						msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
						GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
						msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
						msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
						msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		                msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
					AnsMsg(msg,ERR_FUNCNOTFUND,"总线节点路由无交易码");
					return ;
				}

			}
			
			S_DREB_RSMSG *data=NULL;
			if (isaffirm) //要确认应答
			{
				data = (S_DREB_RSMSG *)m_pMemPool->PoolMalloc();
				if (data == NULL)
				{
					m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"内存分配出错");
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"内存分配出错[%d %d %d %d] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d] 业务数据长度[%d]",\
						msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
						msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
						GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
						msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
						msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
						msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		                msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
					AnsMsg(msg,ERR_SYSTEM,"内存分配出错");
					return;
				}
				
				memcpy(&(data->msghead),&(msg->msghead),sizeof(DREBQUEUEMSG_HEAD));
				memcpy(&(data->message),&(msg->message),sizeof(COMMSTRU));
				//以上为要确认应答，先复制一份
			}
			//发给指定服务
			//转发至服务
			if (m_log->isWrite(LOG_DEBUG))
			{
				m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"将数据发给本地服务处理 [%d %s] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d] 业务数据长度[%d]",\
					rt.nIndex,GetIndexFlag(rt.nIndex).c_str(),\
					GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
					msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
					msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
					msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		        msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
			}
			if (m_pSocketMgr->at(rt.nIndex) != NULL)
			{
				m_pSocketMgr->at(rt.nIndex)->SendMsg(msg);
			}
			else
			{
				m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"本地服务[%d %d]已断开",rt.nSvrMainId,rt.cSvrPrivateId);
			}
			if (isaffirm)
			{
				AnsMsg(data,SUCCESS,"已转发至服务");
			}
			return ;
		}
		//公共节点相同但私有节点不相同，所以通过下面的路由方式
	}
	
	S_DREB_ROUTE  dreb;
	//不是本节点，要通过路由转发
	if (0 == msg->message.head.d_Dinfo.d_cNodePrivateId)
	{
		bRet =m_pMemDb->SelectRoute(msg->message.head.d_Dinfo.d_nNodeId,dreb);
	}
	else
	{
		bRet =m_pMemDb->SelectRoute(msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,dreb);
	}
	if (!bRet)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"路由错误 找不到总线节点[%d %d]的路由",\
			msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId);
		AnsMsg(msg,ERR_DREBROUTE,"路由错误");
		return;
	}
	if (0 == dreb.nStep)
	{
		//怎么可能呢，若是本节点应该在上面才对
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"路由错误 [%d %d %d %d] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d] 业务数据长度[%d]",\
			msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
			msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
			GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
			msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
			msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
			msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		        msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"路由错误 总线节点路由出错 当前节点[%d %d] 路由目标节点[%d %d] nStep[%d]",\
			m_pRes->g_nDrebId,m_pRes->g_nDrebPrivateId,dreb.nNodeId,dreb.cNodePrivateId,dreb.nStep);
		AnsMsg(msg,ERR_DREBROUTE,"路由错误");
		return ;
	}
	//用此字段作为路由路径计数，超过16丢弃
	msg->message.head.a_Ainfo.a_nNodeId++;
	if (dreb.nNodeId == msg->message.head.s_Sinfo.s_nNodeId && dreb.cNodePrivateId == msg->message.head.s_Sinfo.s_cNodePrivateId)	
	{
		//发回接收请求的总线，这样造成循环，直接丢弃
		m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"交易未注册");
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"交易未注册[%d %d %d %d] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d] 业务数据长度[%d]",\
			msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
			msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
			GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
			msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
			msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
			msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
			msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
		AnsMsg(msg,ERR_FUNCNOTFUND,"交易未注册");
		return;
	}
	//songfree 20210126 修改最大步进为配置
	if (msg->message.head.a_Ainfo.a_nNodeId >=m_pRes->g_nMaxRouteStep)
	{
		//发回接收请求的总线，这样造成循环，直接丢弃
		m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"路由循环错误，总线可能断开");
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"路由循环错误，总线可能断开[%d %d %d %d] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d] 业务数据长度[%d]",\
			msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
			msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
			GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
			msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
			msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
			msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
			msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
		AnsMsg(msg,ERR_FUNCNOTFUND,"路由循环错误，总线可能断开");
		return;
	}
	if (m_log->isWrite(LOG_DEBUG))
	{
		m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"将数据发给路由处理 [%d %s] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d] 业务数据长度[%d]",\
			dreb.nIndex,GetIndexFlag(dreb.nIndex).c_str(),\
			GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
			msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
			msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
			msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
			msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
	}
	
	if (!dreb.bIsClose)
	{
		m_pSocketMgr->at(dreb.nIndex)->SendMsg(msg);
	}
	else 
	{
		if (m_log->isWrite(LOG_WARNNING))
		{
			m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"路由连接已断开，暂缓发送[%d %d %d %d] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d] 业务数据长度[%d]",\
				msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
				msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
				GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
				msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
				msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
				msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		    msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
		}
		
		if (dreb.pSendQueue->PutSendMsg(msg) !=0 )
		{
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"入发送队列失败");
		}
	}
	return ;
}

void CMsgProcThread::RouteMsgRequestBySvr(S_DREB_RSMSG *msg,bool isaffirm)
{
	bool bRet;
	S_DREB_ROUTE rt;
	if (0 == msg->message.head.d_Dinfo.d_cSvrPrivateId)
	{
		if (msg->message.head.d_Dinfo.d_nServiceNo>0)
		{
			//20181019增加按交易码和服务号路由
			//即服务没有注册交易码的不会发，用于服务主备
			bRet = m_pMemDb->SelectRouteByFuncSvr(msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.d_Dinfo.d_nSvrMainId,rt);
			if (!bRet) // 20190311没有找到，按服务号查
			{
				bRet = m_pMemDb->SelectRouteBySvr(msg->message.head.d_Dinfo.d_nSvrMainId,rt);
			}
		}
		else //没有填交易码，按服务发送
		{
			bRet = m_pMemDb->SelectRouteBySvr(msg->message.head.d_Dinfo.d_nSvrMainId,rt);
		}
	}
	else
	{
		bRet = m_pMemDb->SelectRouteBySvr(msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,rt);
	}
	if (!bRet)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"根据服务路由 找不到服务 [%d %d %d %d] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d] 业务数据长度[%d]",\
			msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
			msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
			GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
			msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
			msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
			msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		    msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
			AnsMsg(msg,ERR_SVRNOTREGISTER,"服务未注册");
		return;
	}
	//用此字段作为路由路径计数，超过16丢弃
	msg->message.head.a_Ainfo.a_nNodeId++;
	//songfree 20210126 修改最大步进为配置
	if (msg->message.head.a_Ainfo.a_nNodeId >=m_pRes->g_nMaxRouteStep)
	{
		//发回接收请求的总线，这样造成循环，直接丢弃
		m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"路由循环错误，服务可能断开");
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"路由循环错误，服务可能断开[%d %d %d %d] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d] 业务数据长度[%d]",\
			msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
			msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
			GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
			msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
			msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
			msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
			msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
		AnsMsg(msg,ERR_FUNCNOTFUND,"路由循环错误，服务可能断开");
		return;
	}

	S_DREB_RSMSG *data=NULL;
	if (isaffirm) //要确认应答
	{
		data = (S_DREB_RSMSG *)m_pMemPool->PoolMalloc();
		if (data == NULL)
		{
			m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"内存分配出错");
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"内存分配出错[%d %d %d %d] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d] 业务数据长度[%d]",\
				msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
				msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
				GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
				msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
				msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
				msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		        msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
			AnsMsg(msg,ERR_SYSTEM,"内存分配出错");
			return;
		}
		
		memcpy(&(data->msghead),&(msg->msghead),sizeof(DREBQUEUEMSG_HEAD));
		memcpy(&(data->message),&(msg->message),sizeof(COMMSTRU));
		//以上为要确认应答，先复制一份
	}
	//转发至服务
	if (m_log->isWrite(LOG_DEBUG))
	{
		m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"将数据发给服务处理 [%d %s] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d] 业务数据长度[%d]",\
			rt.nIndex,GetIndexFlag(rt.nIndex).c_str(),\
			GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
			msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
			msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
			msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
	}
	if (m_pSocketMgr->at(rt.nIndex) != NULL)
	{
		m_pSocketMgr->at(rt.nIndex)->SendMsg(msg);
	}
	else
	{
		m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"连接已断开");
	}
	if (isaffirm)
	{
		AnsMsg(data,SUCCESS,"已转发至服务");
	}
	return ;
}

void CMsgProcThread::RouteMsgRequestByFunc(S_DREB_RSMSG *msg,bool isaffirm)
{
	bool bRet;
	S_DREB_ROUTE rt;
//	m_pRes->PutTime();
	bRet = m_pMemDb->SelectRouteByFunc(msg->message.head.d_Dinfo.d_nServiceNo,rt);
	if (!bRet)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"根据交易路由 找不到此交易 [%d %d %d %d] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d] 业务数据长度[%d]",\
			msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
			msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
			GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
			msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
			msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
			msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		        msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
		AnsMsg(msg,ERR_FUNCNOTFUND,"交易码未注册");
		return;
	}
	//用此字段作为路由路径计数，超过16丢弃
	msg->message.head.a_Ainfo.a_nNodeId++;

	if (rt.nStep >0) //说明不是本地
	{
		if (rt.nNodeId == msg->message.head.s_Sinfo.s_nNodeId && rt.cNodePrivateId == msg->message.head.s_Sinfo.s_cNodePrivateId)	
		{
			//发回接收请求的总线，这样造成循环，直接丢弃
			m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"交易未注册");
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"交易未注册[%d %d %d %d] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d] 业务数据长度[%d]",\
				msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
				msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
				GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
				msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
				msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
				msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
				msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
			AnsMsg(msg,ERR_FUNCNOTFUND,"交易未注册");
			return;
		}
		//songfree 20210126 修改最大步进为配置
		if (msg->message.head.a_Ainfo.a_nNodeId >=m_pRes->g_nMaxRouteStep)
		{
			//发回接收请求的总线，这样造成循环，直接丢弃
			m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"路由循环错误，交易未注册");
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"路由循环错误，交易未注册[%d %d %d %d] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d] 业务数据长度[%d]",\
				msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
				msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
				GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
				msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
				msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
				msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
				msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
			AnsMsg(msg,ERR_FUNCNOTFUND,"路由循环错误，交易未注册");
			return;
		}
	}
//	m_pRes->PutTime();
	S_DREB_RSMSG *data=NULL;
	if (isaffirm) //要确认应答
	{
		data = (S_DREB_RSMSG *)m_pMemPool->PoolMalloc();
		if (data == NULL)
		{
			m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"内存分配出错");
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"内存分配出错[%d %d %d %d] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d] 业务数据长度[%d]",\
				msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
				msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
				GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
				msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
				msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
				msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		        msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
			AnsMsg(msg,ERR_SYSTEM,"内存分配出错");
			return;
		}
		
		memcpy(&(data->msghead),&(msg->msghead),sizeof(DREBQUEUEMSG_HEAD));
		memcpy(&(data->message),&(msg->message),sizeof(COMMSTRU));
		//以上为要确认应答，先复制一份
	}
	//转发至服务或总线路由
	if (m_log->isWrite(LOG_DEBUG))
	{
		m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"将数据发给[%d %s] 处理 DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 目标[%d %d %d %d] 业务数据长度[%d]",\
			rt.nIndex,GetIndexFlag(rt.nIndex).c_str(),\
			GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
			msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
			msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
			msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		        msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
	}
	if (m_pSocketMgr->at(rt.nIndex) != NULL)
	{
		m_pSocketMgr->at(rt.nIndex)->SendMsg(msg);
	}
	else
	{
		m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"连接已断开");
	}
	
//	m_pRes->PutTime();
	if (isaffirm)
	{
		AnsMsg(data,SUCCESS,"已转发");
	}
	return ;
}

void CMsgProcThread::PrintRoute()
{
	if (m_pRes->g_nPrintRoute != 1)
	{
		return;
	}
	unsigned int i;
	std::string routemsg;
	char tmpchar[400];
	std::vector<S_DREB_ROUTE *> dp;
	sprintf(tmpchar,"\n  DREB LOCAL INFO\n");
	routemsg = routemsg+tmpchar;
	sprintf(tmpchar,"    nIndex=%d nStep=%d nNodeId=%d cNodePrivateId=%d nbandwidth=%d\n",\
		0,0,m_pRes->g_nDrebId,m_pRes->g_nDrebPrivateId,1024000);
	routemsg = routemsg+tmpchar;
	m_pMemDb->GetAllDreb(dp);
	sprintf(tmpchar,"  DREB CONNECT LIST\n");
	routemsg = routemsg+tmpchar;
	for (i=0;i<dp.size();i++)
	{
		sprintf(tmpchar,"    nIndex=%d nStep=%d nNodeId=%d cNodePrivateId=%d bIsClose=%d nbandwidth=%d\n",\
			dp[i]->nIndex,dp[i]->nStep,dp[i]->nNodeId,dp[i]->cNodePrivateId,dp[i]->bIsClose,dp[i]->nbandwidth);
		routemsg = routemsg+tmpchar;
	}
	std::vector<S_SVR_ROUTE>svrlist;
	m_pMemDb->GetAllSvr(svrlist);
	sprintf(tmpchar,"  DREB LOCAL SERVICE LIST\n");
	routemsg = routemsg+tmpchar;
	for (i=0;i<svrlist.size();i++)
	{
		sprintf(tmpchar,"    nSvrMainId=%d cSvrPrivateId=%d bIsClose=%d\n",\
			svrlist[i].nSvrMainId,svrlist[i].cSvrPrivateId,svrlist[i].bIsClose);
		routemsg = routemsg+tmpchar;
	}
	
	std::vector<S_DREB_ROUTE *> rt;
	m_pMemDb->GetRouteList(rt);
	sprintf(tmpchar,"  DREB ROUTE LIST\n");
	routemsg = routemsg+tmpchar;
	for (i=0;i<rt.size();i++)
	{
		sprintf(tmpchar,"    nIndex=%d nStep=%d nNodeId=%d cNodePrivateId=%d nbandwidth=%d\n",\
			rt[i]->nIndex,rt[i]->nStep,rt[i]->nNodeId,rt[i]->cNodePrivateId,rt[i]->nbandwidth);
		routemsg = routemsg+tmpchar;
	}
	m_pRes->g_pRouteLog.LogBin(LOG_INFO,__FILE__,__LINE__,"节点服务及节点路由信息",\
		(char *)routemsg.c_str(),routemsg.length());
}

void CMsgProcThread::PrintServiceRoute()
{
	if (m_pRes->g_nPrintRoute != 1)
	{
		return;
	}
	unsigned int i;
	std::string routemsg;
	char tmpchar[DREBBUFSIZE];

	std::vector<S_SERVICE_ROUTE *>servicelist;
	m_pMemDb->GetAllServiceOrder(servicelist);
	sprintf(tmpchar,"\n  DREB SERVICE LIST,  TOTAL SERVICE:%d \n",servicelist.size());
	routemsg = routemsg+tmpchar;
	int index;
	int nodeid;
	int nodeprivateid;
	int svrid;
	int svrprivateid;
	
	for (i=0;i<servicelist.size();i++)
	{
		if (i == 0)
		{
			index = servicelist[i]->nIndex;
			nodeid = servicelist[i]->nNodeId;
			nodeprivateid = servicelist[i]->cNodePrivateId;
			svrid = servicelist[i]->nSvrMainId;
			svrprivateid = servicelist[i]->cSvrPrivateId;
			sprintf(tmpchar,"    INDEX=%d DREB[%d %d] SVR[%d %d] STEP=%d FUNC=",index,nodeid,nodeprivateid,svrid,svrprivateid,servicelist[i]->nStep);
			routemsg= routemsg+tmpchar;
		}
		if (index != servicelist[i]->nIndex || nodeid != servicelist[i]->nNodeId  || nodeprivateid != servicelist[i]->cNodePrivateId \
			|| svrid != servicelist[i]->nSvrMainId || svrprivateid != servicelist[i]->cSvrPrivateId)
		{
			index = servicelist[i]->nIndex;
			nodeid = servicelist[i]->nNodeId;
			nodeprivateid = servicelist[i]->cNodePrivateId;
			svrid = servicelist[i]->nSvrMainId;
			svrprivateid = servicelist[i]->cSvrPrivateId;
			sprintf(tmpchar,"\n    INDEX=%d DREB[%d %d] SVR[%d %d] STEP=%d FUNC=",index,nodeid,nodeprivateid,svrid,svrprivateid,servicelist[i]->nStep);
			routemsg= routemsg+tmpchar;
		}
		//sprintf(tmpchar," %d-i%d",servicelist[i]->nFuncNo,servicelist[i]->nIndex);
		sprintf(tmpchar," %d",servicelist[i]->nFuncNo);
		routemsg= routemsg+tmpchar;
	}
	routemsg= routemsg+"\n";
	m_pRes->g_pRouteLog.LogBin(LOG_INFO,__FILE__,__LINE__,"交易路由信息",(char *)routemsg.c_str(),routemsg.length());
}


void CMsgProcThread::OnCmdSubscribe(S_DREB_RSMSG* msg)
{
    int snum = 0;
    PCOMMSTRU databuf = &(msg->message);
    S_SERVICEREG_MSG* prs = NULL;
	S_SUBSCRIBE data;

    prs = (S_SERVICEREG_MSG*)(databuf->buffer);
    m_pDrebEndian.Endian2LocalHost((unsigned char*)&(prs->nSvrMainId), sizeof(prs->nSvrMainId));
    m_pDrebEndian.Endian2LocalHost((unsigned char*)&(prs->nFuncNo), sizeof(prs->nFuncNo));
    m_pDrebEndian.Endian2LocalHost((unsigned char*)&(prs->nFuncNum), sizeof(prs->nFuncNum));

    m_log->LogMp(LOG_DEBUG, __FILE__, __LINE__, "服务[%d %d]广播订阅   广播个数[%d] len=[%d]", \
        prs->nSvrMainId, prs->cSvrPrivateId, prs->nFuncNum, msg->message.head.nLen);
    if (prs->nFuncNum < 1)
    {
        m_log->LogMp(LOG_ERROR, __FILE__, __LINE__, "服务[%d %d]广播订阅 广播个数[%d]为0，取消订阅", prs->nSvrMainId, prs->cSvrPrivateId, prs->nFuncNum);
		m_pMemDb->m_subscribe.UnSubscribe(msg->msghead.index);
		m_pMemPool->PoolFree(msg);
        return;
    }
    if (msg->message.head.nLen < sizeof(S_SERVICEREG_MSG) + (prs->nFuncNum - 1) * sizeof(unsigned int))
    {
        m_log->LogMp(LOG_ERROR, __FILE__, __LINE__, "服务[%d %d]广播订阅 报文长度[%d]不符 应为[%d]", \
            prs->nSvrMainId, prs->cSvrPrivateId, prs->nFuncNum, msg->message.head.nLen, \
            sizeof(S_SERVICEREG_MSG) + (prs->nFuncNum - 1) * sizeof(unsigned int));
		m_pMemPool->PoolFree(msg);
        return;
    }
    data.nSvrMainId = prs->nSvrMainId;
    data.nFuncNo = prs->nFuncNo;
    data.cSvrPrivateId = prs->cSvrPrivateId;
    data.nIndex = msg->msghead.index;

	std::vector<unsigned int> subscribelist;
	subscribelist.push_back(data.nFuncNo);
    snum = prs->nFuncNum - 1;

    int nFuncNum = prs->nFuncNum;
    for (int i = 0; i < snum; i++)
    {
        memcpy(&(data.nFuncNo), databuf->buffer + sizeof(S_SERVICEREG_MSG) + i * sizeof(unsigned int), sizeof(unsigned int));
        m_pDrebEndian.Endian2LocalHost((unsigned char*)&(data.nFuncNo), sizeof(unsigned int));
		subscribelist.push_back(data.nFuncNo);
    }

    m_log->LogMp(LOG_DEBUG + 2, __FILE__, __LINE__, "服务%d-%d有%d个广播订阅", data.nSvrMainId, data.cSvrPrivateId, nFuncNum);
	m_pMemDb->m_subscribe.Subscribe(data.nIndex,subscribelist, data.nSvrMainId, data.cSvrPrivateId);
	m_pMemPool->PoolFree(msg);
    return;
}
void CMsgProcThread::OnCmdUnSubscribe(S_DREB_RSMSG* msg)
{
    m_log->LogMp(LOG_ERROR, __FILE__, __LINE__, "index[%d] 取消广播订阅", msg->msghead.index);
    m_pMemDb->m_subscribe.UnSubscribe(msg->msghead.index);
    m_pMemPool->PoolFree(msg);
    return;
}

void CMsgProcThread::TransDestBc(S_DREB_RSMSG* msg)
{
	//m_log->LogMp(LOG_DEBUG, __FILE__, __LINE__, "TransDestBc 开始 m_vBcDest size[%d] dest[%d %d %d %d] local[%d %d]", m_pRes->m_vBcDest.size(),\
	//	msg->message.head.d_Dinfo.d_nNodeId, msg->message.head.d_Dinfo.d_cNodePrivateId, msg->message.head.d_Dinfo.d_nSvrMainId, msg->message.head.d_Dinfo.d_cSvrPrivateId, \
	//	m_pRes->g_nDrebId,m_pRes->g_nDrebPrivateId);
	if (m_pRes->m_vBcDest.size()>0 && \
		msg->message.head.d_Dinfo.d_nNodeId == m_pRes->g_nDrebId && \
		msg->message.head.d_Dinfo.d_cNodePrivateId == m_pRes->g_nDrebPrivateId && \
		msg->message.head.d_Dinfo.d_nSvrMainId == 0 && \
		msg->message.head.d_Dinfo.d_cSvrPrivateId == 0)
	{
		S_DREB_RSMSG* data = NULL;
		S_DREB_ROUTE dreb;
		//为发给本节点的广播，查看是否有配置，若有则转发出去
		for (unsigned int i = 0; i < m_pRes->m_vBcDest.size(); i++)
		{
			if (m_pMemDb->SelectRoute(m_pRes->m_vBcDest[i].nNodeId, m_pRes->m_vBcDest[i].cNodePrivateId, dreb))
			{
                data = (S_DREB_RSMSG*)m_pMemPool->PoolMalloc();
                if (data == NULL)
                {
                    m_log->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "TransDestBc 内存分配出错");
                    return;
                }
                memcpy(&(data->msghead), &(msg->msghead), sizeof(DREBQUEUEMSG_HEAD));
                memcpy(&(data->message), &(msg->message), sizeof(COMMSTRU)); 
                data->message.head.d_Dinfo.d_nNodeId = m_pRes->m_vBcDest[i].nNodeId;
                data->message.head.d_Dinfo.d_cNodePrivateId = m_pRes->m_vBcDest[i].cNodePrivateId;
                if (m_pSocketMgr->at(dreb.nIndex)->SendMsg(data) != 0)
                {
                    m_log->LogMp(LOG_ERROR, __FILE__, __LINE__, "TransDestBc 发送失败");
                }
				//else
				//{
				//	m_log->LogMp(LOG_DEBUG, __FILE__, __LINE__, "广播转发总线节点[%d %d]完成", m_pRes->m_vBcDest[i].nNodeId, m_pRes->m_vBcDest[i].cNodePrivateId);
				//}
			}
			//else
			//{
			//	m_log->LogMp(LOG_WARNNING, __FILE__, __LINE__, "无广播转发的总线节点[%d %d]路由", m_pRes->m_vBcDest[i].nNodeId, m_pRes->m_vBcDest[i].cNodePrivateId);
			//}
		}
	}
	return;
}