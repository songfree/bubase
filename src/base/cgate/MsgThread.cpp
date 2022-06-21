// MsgThread.cpp: implementation of the CMsgThread class.
//
//////////////////////////////////////////////////////////////////////

#include "MsgThread.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMsgThread::CMsgThread()
{
	m_pRes = NULL;
	m_pMemPool = NULL;
	m_pPoolData = NULL;
	m_sThreadName,"CMsgThread";
}

CMsgThread::~CMsgThread()
{

}
std::string CMsgThread::GetDrebCmdType(int cmdtype)
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
	case  CMD_MONITOR_DREB:     //15 //数据总线节点监控
		return "CMD_MONITOR_DREB";
	case  CMD_MONITOR_BPC:     //15 //数据总线节点监控
		return "CMD_MONITOR_BPC";
	default:
		return "";
	}
}
void CMsgThread::ExitThreadInstance()
{
	return ;
}
bool CMsgThread::InitThreadInstance()
{
	return true;
}
bool CMsgThread::SetGlobalVar(CGateResource *res,CPoolData *pooldata,CBF_BufferPool *mempool,CBF_DrebServer *drebapi)
{
	m_pRes = res;
	m_pDrebApi = drebapi;
	m_pMemPool = mempool;
	m_pPoolData = pooldata;

	m_pLog =m_pRes->g_pLog;
	m_pDesZip.SetLogPara(m_pLog);
	return true;
}
int CMsgThread::Run()
{
	m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"启动消息处理线程");
	while (!m_pRes->g_bIsExit)
	{
		if (m_pPoolData->GetData(m_pDataBuf))
		{
			//分发数据
			Dispatch();
		}
	}
	m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"停止消息处理线程");
	return 0;
}
void CMsgThread::Dispatch()
{
	m_pDataBuf.sMsgBuf->sBpcHead.nIndex = 100;
	LogDrebHead(LOG_DEBUG,m_pDataBuf.sMsgBuf->sDBHead,"发到总线的数据 ",__FILE__,__LINE__);
	m_pDrebApi->SendMsg(m_pDataBuf);
	
	return;
}

void CMsgThread::LogDrebHead(int loglevel, DREB_HEAD head, const char *msg, const char *filename, int fileline)
{
	//bzero(m_headMsg,sizeof(m_headMsg));
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
	m_pLog->LogMp(loglevel,filename,fileline,m_headMsg,msg);
}
