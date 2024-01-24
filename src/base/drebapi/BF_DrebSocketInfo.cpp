// BF_DrebSocketInfo.cpp: implementation of the CBF_DrebSocketInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "BF_DrebSocketInfo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define  DESENCRYPTKEY    "rd402wms"
#define  DESENCRYPTKEY3   "wms13711songfree"

CBF_DrebSocketInfo::CBF_DrebSocketInfo()
{
	ResetInfo();
	m_pMemPool = NULL;
	m_gRes = NULL;
	m_nType = BPCSOCK_UNKNOW;
	m_nCloseTime = 0;
	m_nPid = -1;
	m_sendNum = 10;
	m_bNeedConnect = false;
	m_nBpcHeadLen = sizeof(S_BPC_HEAD);
	m_nDrebHeadLen = DREBHEADLEN;
}

CBF_DrebSocketInfo::~CBF_DrebSocketInfo()
{
	if (m_sock != INVALID_SOCKET)
	{
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
	}
	RDQUEUE::iterator prq,prn;
	for (prn=m_pSendQueue.m_qSendData.datas.begin();prn!=m_pSendQueue.m_qSendData.datas.end();)
	{
		prq = prn;
		prn++;
		m_pMemPool->PoolFree(prq->sMsgBuf);
		m_pSendQueue.DeleteSend(prq);	
	}
}
std::string CBF_DrebSocketInfo::GetBpcMsgType(int msgtype)
{
	switch (msgtype)
	{
	case MSG_REQ:        // 0      //接收请求,bpc发给BPU的请求
		return "MSG_REQ";
	case MSG_FREE:        //1      //BPU发送空闲请求
		return "MSG_FREE";
	case MSG_GETSERIAL:   //4      //BPU取共享唯一流水
		return "MSG_GETSERIAL";
	case MSG_SETGPARA1:  //5      //BPU发送SETGPARA1
		return "MSG_SETGPARA1";
	case MSG_SETGPARA2:   //6      //BPU发送SETGPARA2
		return "MSG_SETGPARA2";
	case MSG_SETGPARA3:   //7      //BPU发送SETGPARA3
		return "MSG_SETGPARA3";	
	case MSG_EXTCALL:      //10      //BPU发送外调请求	
		return "MSG_EXTCALL";
	case MSG_BPCCALL:      //11      //BPU发送PU组调用请求
		return "MSG_BPCCALL";
	case MSG_BPUREG:       //12      //BPU发送注册请求
		return "MSG_BPUREG";
	case MSG_BPUISREG:     //13      //BPU发送是否注册请求
		return "MSG_BPUISREG";
	case MSG_GETNEXT:      //15      //dreb过来的取后续包的消息
		return "MSG_GETNEXT";
	case MSG_BPCMONITOR:      //16      //主动的监控报告，定时发送
		return "MSG_BPCMONITOR";
	case MSG_GETBPCNEXT:      //17      //dreb过来的取在BPC缓存的后续包的消息
		return "MSG_GETBPCNEXT";
	case MSG_CONNECTDREB:      //18      //dreb过来的已连接DREB成功的消息，可在这消息里进行注册等(主备询问，注册)
		return "MSG_CONNECTDREB";
	case MSG_MONITOR_SAP:      //19      //dreb过来的请求监控消息，DREB命令字为CMD_MONITOR_BPC,具体的监控项在service_no里，详见DrebPubDefine.h里的BPCMONI_INFO那里
		return "MSG_MONITOR_SAP";
	case MSG_DREBANS:      //20      //DREB过来的应答信息
		return "MSG_DREBANS";
	default:
		return "";
	}
}
std::string CBF_DrebSocketInfo::GetDrebCmdType(int cmdtype)
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
	case  CMD_UNREGSERVICE:      //14 //取消交易注册
		return "CMD_UNREGSERVICE";
	case  CMD_MONITOR_DREB:     //15 //数据总线节点监控
		return "CMD_MONITOR_DREB";
	case  CMD_MONITOR_BPC:     //16 //数据总线节点监控
		return "CMD_MONITOR_BPC";
	default:
		return "";
	}
}

void CBF_DrebSocketInfo::Init(CBF_DrebResource *res,CBF_BufferPool *pool,CIErrlog *perrlog,CBF_CommDataLog *pdreb2bpclog,CBF_CommDataLog *pbpc2dreblog)
{
	m_pMemPool = pool;
	m_gRes = res;
	m_log = perrlog;
	m_pDreb2BpcDataLog = pdreb2bpclog;
	m_pBpc2DrebDataLog = pbpc2dreblog;
	m_pSendQueue.m_nQueueSize = m_gRes->g_nQueueSize;
	ResetInfo();
	m_nCloseTime = 0;
	m_pDrebEndian.SetCommEndian(true);//使用主机序
}
void CBF_DrebSocketInfo::ResetInfo()
{
	
	memset(m_cCreateDate,0,sizeof(m_cCreateDate));
	memset(m_cCloseDate,0,sizeof(m_cCloseDate));
	memset(&m_head,0,sizeof(DREB_HEAD));
	//socket公共参数
	m_sock = INVALID_SOCKET;                   //socket id
	m_bChecked = false;
	m_bNeedread=false;              //是否需要读数据
	m_nReadtime= time(NULL);              //上次读数据时间
	m_nConntime=m_nReadtime;              //连接建立时间
	m_nSendtime=m_nReadtime;              //上次发送数据时间 
	m_nPingTime = 0;
	//发送接收数据	
	
	memset(m_sRcvBuffer,0,sizeof(m_sRcvBuffer));            //接收数据缓冲   
	m_nRcvBufLen=0;                //接收的数据长度  可多次接收一个数据包
	
	memset(m_cCreateDate,0,sizeof(m_cCreateDate));//连接建立日期
	m_nConntime = 0;
	memset(m_cCloseDate,0,sizeof(m_cCloseDate));//连接关闭日期
	m_nCloseTime = 0;
}

void CBF_DrebSocketInfo::OnClose(const char *msg,const char *filename,int fileline)
{
	if (m_sock == INVALID_SOCKET)
	{
		return;
	}
	CBF_PMutex pp(&m_mutex);
	m_datetime.Update();
	m_datetime.ToCharDate(m_cCloseDate);
#ifdef _ENGLISH_
	m_log->LogMp(LOG_DEBUG,filename,fileline,"%s,close DREB [%s:%d %d] index[%d] type[%d] sock[%d]",\
		msg,m_sDrebIp.c_str(),m_nDrebPort,m_sock,m_index,m_nType,m_sock);
#else
	m_log->LogMp(LOG_DEBUG,filename,fileline,"%s,关闭DREB连接[%s:%d %d] index[%d] type[%d] sock[%d]",\
		msg,m_sDrebIp.c_str(),m_nDrebPort,m_sock,m_index,m_nType,m_sock);
#endif
	
	closesocket(m_sock);
	m_sock = INVALID_SOCKET;
	ResetInfo();
	m_nCloseTime = time(NULL);
	
	m_bNeedConnect = true; //需要连接
	//将发送一半的数据删除掉，留下完整的放在队列里
	CheckFirstData();
}

void CBF_DrebSocketInfo::OnShutdown()
{
	closesocket(m_sock);
	ResetInfo();
	m_nCloseTime = 0;
}
bool CBF_DrebSocketInfo::IsCanSelect()
{
	if (m_sock != INVALID_SOCKET)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CBF_DrebSocketInfo::Listen(int port,bool blockflag,int queuenum)
{
	if (!m_tcpSocket.Create())
	{
		CBF_Tools::StringCopy(m_errMsg,255,m_tcpSocket.GetErrorMsg().c_str());
		return false;
	}
	m_nPort = port;
	if (!m_tcpSocket.Listen(m_nPort,queuenum))
	{
#ifdef _ENGLISH_
		sprintf(m_errMsg,"Listen Port [%d] fail, queue size %d",m_nPort,queuenum);
#else
		sprintf(m_errMsg,"侦听端口[%d]失败,侦听队列大小%d",m_nPort,queuenum);
#endif
		return false;
	}
	if (!blockflag)
	{
		m_tcpSocket.SetBlockFlag(false);
	}
	m_nType   =  BPCSOCK_LISTEN;      //侦听
	m_sock = m_tcpSocket.GetSocket();
#ifndef _WINDOWS
	fcntl(m_sock,F_SETFD,25);
#endif
	m_bNeedread = true;
	
	return true;
}

bool CBF_DrebSocketInfo::IsFree()
{
	if (m_sock != INVALID_SOCKET)
	{
		return false;
	}
	if (m_nType == BPCSOCK_LISTEN)//是侦听
	{
		return false;
	}
	return true;
}
int CBF_DrebSocketInfo::OnRecv()
{
	int ret;
	m_tcpSocket.AttachSocket(m_sock,"");
	if (m_nRcvBufLen >=0 && m_nRcvBufLen< BPCBUFSIZE)
	{
		//包头未接收完
		ret = m_tcpSocket.Receive(m_sRcvBuffer+m_nRcvBufLen,BPCBUFSIZE-m_nRcvBufLen);
		if (ret <= 0)
		{
			return ret;
		}
		m_nRcvBufLen+=ret;
		m_nReadtime =time(NULL);
		return ret;

	}
	return 0;
}

int CBF_DrebSocketInfo::OnSend()
{
	int ret;
	if (m_sock == INVALID_SOCKET)
	{
#ifdef _ENGLISH_
		m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"connect closed");
#else
		m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"连接已断开");
#endif
		return -2;
	}
	if (!m_pSendQueue.NeedSend())
	{
		return -3;
	}
	CBF_PMutex pp(&m_mutex);
	RDQUEUE::iterator prq,prn;
	unsigned int atimeout= time(NULL);
	int sendNum=0;
	for (prn=m_pSendQueue.m_qSendData.datas.begin();prn!=m_pSendQueue.m_qSendData.datas.end();)
	{
		sendNum++;
		if (m_sendNum >0 && sendNum > m_sendNum) //如果本次已发送数超过定义，重新开始
		{
			return sendNum;
		}
		prq = prn;
		prn++;
		if (atimeout - prq->nRtime > 300)
		{
#ifdef _ENGLISH_
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__," index[%d] DREB[%d %d] src[%d %d %d] msg timeout,  datalen[%d]",\
				m_index,m_nDrebId,m_nDrebPrivateId,prq->s_nNodeId,prq->s_cNodePrivateId,prq->s_nDrebSerial,ret);
#else
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"从队列里发送给index[%d] DREB[%d %d] 源[%d %d %d] 的消息 超时5分钟丢弃  数据长度[%d]",\
				m_index,m_nDrebId,m_nDrebPrivateId,prq->s_nNodeId,prq->s_cNodePrivateId,prq->s_nDrebSerial,ret);
#endif
			m_pMemPool->PoolFree(prq->sMsgBuf);
			m_pSendQueue.DeleteSend(prq);	
			continue;
		}
		m_tcpSocket.AttachSocket(m_sock,"");

		ret = m_tcpSocket.Send((char *)&(prq->sMsgBuf->sDBHead)+(prq->nTotalLen-prq->nSendLen),prq->nSendLen);
		if (ret == prq->nSendLen)
		{
			//统计数据
			m_gRes->ReportSend(prq->nSendLen);
#ifdef _ENGLISH_
			m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"index[%d] DREB[%d %d] src[%d %d %d] send msg success datalen[%d]",\
				m_index,m_nDrebId,m_nDrebPrivateId,prq->s_nNodeId,prq->s_cNodePrivateId,prq->s_nDrebSerial,ret);
#else
			m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"从队列里发送给index[%d] DREB[%d %d] 源[%d %d %d] 的消息成功 数据长度[%d]",\
				m_index,m_nDrebId,m_nDrebPrivateId,prq->s_nNodeId,prq->s_cNodePrivateId,prq->s_nDrebSerial,ret);
#endif
			m_pMemPool->PoolFree(prq->sMsgBuf);
			// 处理成功,停止
			m_pSendQueue.DeleteSend(prq);	
			m_nSendtime = time(NULL);
			
		}
		else //停止
		{
			if (ret >0)
			{
				m_gRes->ReportSend(ret);
				prq->nSendLen = prq->nSendLen -ret;
#ifdef _ENGLISH_
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"index[%d] DREB[%d %d] src[%d %d %d] send part success datalen[%d]",\
					m_index,m_nDrebId,m_nDrebPrivateId,prq->s_nNodeId,prq->s_cNodePrivateId,prq->s_nDrebSerial,ret);
#else
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"从队列里发送给index[%d] DREB[%d %d] 源[%d %d %d] 的消息部分成功 数据长度[%d]",\
					m_index,m_nDrebId,m_nDrebPrivateId,prq->s_nNodeId,prq->s_cNodePrivateId,prq->s_nDrebSerial,ret);
#endif
			}
			else
			{
				//下次再发，直到成功或连接关闭
#ifdef _ENGLISH_
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"index[%d] DREB[%d %d] src[%d %d %d] send fail, ret[%d]",\
					m_index,m_nDrebId,m_nDrebPrivateId,prq->s_nNodeId,prq->s_cNodePrivateId,prq->s_nDrebSerial,ret);
#else
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"从队列里发送给index[%d] DREB[%d %d] 源[%d %d %d] 的消息失败 ret[%d]",\
					m_index,m_nDrebId,m_nDrebPrivateId,prq->s_nNodeId,prq->s_cNodePrivateId,prq->s_nDrebSerial,ret);
#endif
			}
			//发送不成功，下次再发，还是从这里再发
			return 0;
		}
	}
	return sendNum;
}


int CBF_DrebSocketInfo::SendMsg(S_BPC_RSMSG &msg,bool sendimmediate)
{
	int ret=0;
	CBF_PMutex pp(&m_mutex);
	m_tcpSocket.AttachSocket(m_sock,"");
	//将DREBHEAD中的数据放入MSG中，供发送时写日志进行跟踪
	msg.s_nNodeId = msg.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId;
	msg.s_nDrebSerial = msg.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial;
	msg.s_cNodePrivateId = msg.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId;

	msg.nTotalLen = msg.sMsgBuf->sDBHead.nLen+DREBHEADLEN;
	msg.nSendLen = msg.nTotalLen;
	if (msg.sMsgBuf->sDBHead.cCmd != CMD_PING && msg.sMsgBuf->sDBHead.cCmd != CMD_CONNECT && msg.sMsgBuf->sDBHead.cCmd != CMD_REGSERVICE)
	{
		if (m_log->isWrite(LOG_DEBUG+1))
		{
#ifdef _ENGLISH_
			m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"send to DREB[%d] msg DREB[%s] RA[%d] txcode[%d]  flag[%d %d %d] src[%d %d] cZip[%d] datalen[%d]",\
				m_index,GetDrebCmdType(msg.sMsgBuf->sDBHead.cCmd).c_str(),\
				msg.sMsgBuf->sDBHead.cRaflag,msg.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,msg.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
				msg.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,msg.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
				msg.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,msg.sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
				msg.sMsgBuf->sDBHead.cZip,msg.sMsgBuf->sDBHead.nLen);
#else
			m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"发送给DREB[%d]的消息 DREB命令[%s] RA标志[%d] 交易码[%d]  标识[%d %d %d] 源[%d %d] cZip[%d] 业务数据长度[%d]",\
				m_index,GetDrebCmdType(msg.sMsgBuf->sDBHead.cCmd).c_str(),\
				msg.sMsgBuf->sDBHead.cRaflag,msg.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,msg.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
				msg.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,msg.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
				msg.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,msg.sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
				msg.sMsgBuf->sDBHead.cZip,msg.sMsgBuf->sDBHead.nLen);
#endif
		}
		
	}
	else
	{
		if (m_log->isWrite(LOG_DEBUG+2))
		{
#ifdef _ENGLISH_
			m_log->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"send to DREB[%d]  DREB[%s]  datalen[%d]",\
				m_index,GetDrebCmdType(msg.sMsgBuf->sDBHead.cCmd).c_str(),msg.sMsgBuf->sDBHead.nLen);
#else
			m_log->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"发送给DREB[%d]的消息 DREB命令[%s]  业务数据长度[%d]",\
				m_index,GetDrebCmdType(msg.sMsgBuf->sDBHead.cCmd).c_str(),msg.sMsgBuf->sDBHead.nLen);
#endif
		}
	}
	if (1 == msg.sMsgBuf->sDBHead.cRaflag)
	{
		//写入DREB主动发过来的日志
		m_pDreb2BpcDataLog->LogDreb(LOG_PROMPT,msg.sMsgBuf,false);
	}
	else
	{
		//写入BPC主动向外发的日志
		m_pBpc2DrebDataLog->LogDreb(LOG_PROMPT,msg.sMsgBuf,false);
	}
	if (m_gRes->g_nCrcFlag == 1)
	{
		m_pDrebEndian.Endian2CommGenCrc(&(msg.sMsgBuf->sDBHead));
	}
	else
	{
		m_pDrebEndian.Endian2Comm(&(msg.sMsgBuf->sDBHead));
	}
	//发送队列若有数据，则放入发送队列，按顺序发送
	if (sendimmediate)
	{
		if (m_sock == INVALID_SOCKET)
		{
#ifdef _ENGLISH_
			m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"connect closed，push queue datalen[%d] flag[%d %d %d] ",msg.nTotalLen,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
#else
			m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"连接已断开，放入DREB队列 数据长度[%d] 标识[%d %d %d] ",msg.nTotalLen,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
#endif
			PutSendMsg(msg);
			return 1;
		}
		ret = m_tcpSocket.Send((char *)&(msg.sMsgBuf->sDBHead),msg.nTotalLen);
		if (ret == msg.nTotalLen)
		{
#ifdef _ENGLISH_
			m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"send finish index[%d] flag[%d %d %d] ",\
				m_index,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
#else
			m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"发送完成 index[%d] 标识[%d %d %d] ",\
				m_index,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
#endif
			//统计数据
			m_gRes->ReportSend(msg.nTotalLen);
			m_pMemPool->PoolFree(msg.sMsgBuf);
			msg.sMsgBuf = NULL;
			m_nSendtime = time(NULL);
			return 0;
			
		}
		else //停止
		{
			if (ret >0)
			{
				msg.nSendLen = msg.nTotalLen - ret;
			}
#ifdef _ENGLISH_
			m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"warn,send to DREB[%d] failed and push queue, retcode[%d] msg[%s] push len [%d] datalen[%d] flag[%d %d %d] ",\
				m_index,ret,m_tcpSocket.GetErrorMsg().c_str(),msg.nSendLen,msg.nTotalLen,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
#else
			m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"警告 发送给DREB[%d]的消息出错,放入队列 ret返回[%d] msg[%s] 放入长度[%d] 数据长度[%d] 标识[%d %d %d] ",\
				m_index,ret,m_tcpSocket.GetErrorMsg().c_str(),msg.nSendLen,msg.nTotalLen,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
#endif
			//只要发送出错就放到队列，如果队列里有数据可能就会有问题
			if (!m_pSendQueue.NeedSend())
			{
				if (PutSendMsg(msg) < 0)
				{
					return 100;
				}
				return 1;
			}
			else
			{
#ifdef _ENGLISH_
				OnClose("send fail,queue has data",__FILE__,__LINE__);
#else
				OnClose("直接在连接发送数据不成功，但连接队列里仍有数据",__FILE__,__LINE__);
#endif
				return -1;
			}
			
		}
	}
	else
	{
		if (NeedSend()) //连接正常
		{
			if (msg.sMsgBuf->sBpcHead.cMsgType != MSG_BPCMONITOR)
			{
#ifdef _ENGLISH_
				m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"queue has data,push DREB queue  index[%d] datalen[%d] flag[%d %d %d] cMsgType[%d] txcode[%d]",\
					m_index,msg.nTotalLen,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial,msg.sMsgBuf->sBpcHead.cMsgType,msg.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo);
#else
				m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"发送队列已有数据，放入DREB队列 index[%d] 数据长度[%d] 标识[%d %d %d] cMsgType[%d] txcode[%d]",\
					m_index,msg.nTotalLen,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial,msg.sMsgBuf->sBpcHead.cMsgType,msg.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo);
#endif
                if (PutSendMsg(msg) < 0)
                {
                    return 100;
                }
				return 1;
			}
		}
		else  //直接发送
		{
			if (m_sock == INVALID_SOCKET )
			{
#ifdef _ENGLISH_
				m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"connect closed,push DREB queue, index[%d] datalen[%d] flag[%d %d %d] ",\
					m_index,msg.nTotalLen,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
#else
				m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"连接已断开，放入DREB队列 index[%d] 数据长度[%d] 标识[%d %d %d] ",\
					m_index,msg.nTotalLen,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
#endif
                if (PutSendMsg(msg) < 0)
                {
                    return 100;
                }
				return 1;
			}
			if (!m_bChecked)
			{
#ifdef _ENGLISH_
				m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"connect not affirm,push DREB queue, index[%d] datalen[%d] flag[%d %d %d] ",\
					m_index,msg.nTotalLen,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
#else
				m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"连接未验证通过，放入DREB队列 index[%d] 数据长度[%d] 标识[%d %d %d] ",\
					m_index,msg.nTotalLen,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
#endif
                if (PutSendMsg(msg) < 0)
                {
                    return 100;
                }
				return 1;
			}
			ret = m_tcpSocket.Send((char *)&(msg.sMsgBuf->sDBHead),msg.nTotalLen);
			if (ret == msg.nTotalLen)
			{
#ifdef _ENGLISH_
				m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"send finish, index[%d] flag[%d %d %d] ",\
					m_index,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
#else
				m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"发送完成 index[%d] 标识[%d %d %d] ",\
					m_index,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
#endif
				//统计数据
				m_gRes->ReportSend(msg.nTotalLen);
				m_pMemPool->PoolFree(msg.sMsgBuf);
				msg.sMsgBuf = NULL;
				m_nSendtime = time(NULL);
				return 0;
				
			}
			else //停止
			{
				if (ret >0)
				{
					msg.nSendLen = msg.nTotalLen - ret;
				}
#ifdef _ENGLISH_
				m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"warn,send to DREB[%d] failed,push queue, ret[%d] msg[%s] pushlen[%d] datalen[%d] flag[%d %d %d] ",\
					m_index,ret,m_tcpSocket.GetErrorMsg().c_str(),msg.nSendLen,msg.nTotalLen,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
#else
				m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"警告 发送给DREB[%d]的消息出错,放入队列 ret返回[%d] msg[%s] 放入长度[%d] 数据长度[%d] 标识[%d %d %d] ",\
					m_index,ret,m_tcpSocket.GetErrorMsg().c_str(),msg.nSendLen,msg.nTotalLen,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
#endif
				//只要发送出错就放入队列
                if (PutSendMsg(msg) < 0)
                {
                    return 100;
                }
				return 2;
			}
		}
	}
	return ret;
}

int CBF_DrebSocketInfo::PutSendMsg(S_BPC_RSMSG msg)
{
	int ret;
	
	ret = m_pSendQueue.PutSendMsg(msg);
	if (ret <0 )
	{
#ifdef _ENGLISH_
		m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"queue already full,discard, index[%d] flag[%d %d %d] queuesize[%d]",\
				m_index,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial, m_pSendQueue.m_nQueueSize);
#else
		m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"队列已满，丢弃 index[%d] 标识[%d %d %d]  队列大小[%d]",\
				m_index,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial, m_pSendQueue.m_nQueueSize);
#endif
		m_pMemPool->PoolFree(msg.sMsgBuf);
		msg.sMsgBuf = NULL;
		return ret;
	}
	return 0;
}



int CBF_DrebSocketInfo::GetRecvData(S_BPC_RSMSG *msg)
{
	int txlen=0;
	char tmpchar[100];
	memset(tmpchar,0,sizeof(tmpchar));
	if (msg->sMsgBuf == NULL)
	{
		return -100;
	}
	//收到的数据大于包头
	if (m_nRcvBufLen >= DREBHEADLEN )
	{
		//进行字节序处理并校验CRC
		memcpy(&(msg->sMsgBuf->sDBHead),m_sRcvBuffer,DREBHEADLEN);
		if (m_gRes->g_nCrcFlag ==1 )
		{
			if (!m_pDrebEndian.Endian2LocalHostChkCrc(&(msg->sMsgBuf->sDBHead)) )
			{
#ifdef _ENGLISH_
				OnClose("CRC error",__FILE__,__LINE__);
#else
				OnClose("错误 CRC错",__FILE__,__LINE__);
#endif
				return -1;
			}
		}
		else
		{
			m_pDrebEndian.Endian2LocalHost(&(msg->sMsgBuf->sDBHead)) ;
		}
		
		if (m_nRcvBufLen < msg->sMsgBuf->sDBHead.nLen + DREBHEADLEN)
		{
			//说明数据没有接收完整
			return 0;
		}
		if (msg->sMsgBuf->sDBHead.nLen >=0 && msg->sMsgBuf->sDBHead.nLen <= DREBDATASIZE )
		{
			memcpy(msg->sMsgBuf->sBuffer,m_sRcvBuffer+DREBHEADLEN,msg->sMsgBuf->sDBHead.nLen);
		}
		else
		{
#ifdef _ENGLISH_
			OnClose("connect not DREB",__FILE__,__LINE__);
#else
			OnClose("错误 连接非DREB",__FILE__,__LINE__);
#endif
			return -1;
		}
		
		msg->sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN+msg->sMsgBuf->sDBHead.nLen;
		msg->sMsgBuf->sBpcHead.nBpuIndex = 0;
		msg->sMsgBuf->sBpcHead.nConnectTime = m_nConntime;
		msg->sMsgBuf->sBpcHead.nIndex = m_index; 
		msg->sMsgBuf->sBpcHead.cMsgType = MSG_REQ;
		msg->sMsgBuf->sBpcHead.nRtime = time(NULL);
		
		//统计数据
		m_gRes->ReportRcv(msg->sMsgBuf->sBpcHead.nBpcLen);
		if (msg->sMsgBuf->sDBHead.cCmd != CMD_PING && msg->sMsgBuf->sDBHead.cCmd != CMD_CONNECT && msg->sMsgBuf->sDBHead.cCmd != CMD_REGSERVICE)
		{
#ifdef _ENGLISH_
			m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"index[%d] DREB[%d %d] reveive: DREB[%d %d] msg DREB[%s] next[%d] RA[%d] txcode[%d]  flag[%d %d %d] src[%d %d] cZip[%d] dest[%d %d %d %d] datalen[%d]",\
				m_index,m_nDrebId,m_nDrebPrivateId,msg->sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,msg->sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,\
				GetDrebCmdType(msg->sMsgBuf->sDBHead.cCmd).c_str(),msg->sMsgBuf->sDBHead.cNextFlag,\
				msg->sMsgBuf->sDBHead.cRaflag,msg->sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,msg->sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
				msg->sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,msg->sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
				msg->sMsgBuf->sDBHead.s_Sinfo.s_nSerial,msg->sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
				msg->sMsgBuf->sDBHead.cZip,msg->sMsgBuf->sDBHead.d_Dinfo.d_nNodeId,\
				msg->sMsgBuf->sDBHead.d_Dinfo.d_cNodePrivateId,msg->sMsgBuf->sDBHead.d_Dinfo.d_nSvrMainId,\
				msg->sMsgBuf->sDBHead.d_Dinfo.d_cSvrPrivateId,msg->sMsgBuf->sDBHead.nLen);
#else
			m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"从index[%d] DREB[%d %d] 收到源DREB[%d %d]收到的消息 DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 源[%d %d] cZip[%d] 目标[%d %d %d %d] 业务数据长度[%d]",\
				m_index,m_nDrebId,m_nDrebPrivateId,msg->sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,msg->sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,\
				GetDrebCmdType(msg->sMsgBuf->sDBHead.cCmd).c_str(),msg->sMsgBuf->sDBHead.cNextFlag,\
				msg->sMsgBuf->sDBHead.cRaflag,msg->sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,msg->sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
				msg->sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,msg->sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
				msg->sMsgBuf->sDBHead.s_Sinfo.s_nSerial,msg->sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
				msg->sMsgBuf->sDBHead.cZip,msg->sMsgBuf->sDBHead.d_Dinfo.d_nNodeId,\
				msg->sMsgBuf->sDBHead.d_Dinfo.d_cNodePrivateId,msg->sMsgBuf->sDBHead.d_Dinfo.d_nSvrMainId,\
				msg->sMsgBuf->sDBHead.d_Dinfo.d_cSvrPrivateId,msg->sMsgBuf->sDBHead.nLen);
#endif
		}
		else
		{
#ifdef _ENGLISH_
			m_log->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"index[%d] DREB[%d %d] receive  DREB[%s]  datalen[%d]",\
				m_index,m_nDrebId,m_nDrebPrivateId,GetDrebCmdType(msg->sMsgBuf->sDBHead.cCmd).c_str(),msg->sMsgBuf->sDBHead.nLen);
#else
			m_log->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"接收到index[%d] DREB[%d %d]的消息 DREB命令[%s]  业务数据长度[%d]",\
				m_index,m_nDrebId,m_nDrebPrivateId,GetDrebCmdType(msg->sMsgBuf->sDBHead.cCmd).c_str(),msg->sMsgBuf->sDBHead.nLen);
#endif
		}
		//若是应答且为不可靠推送或全域广播消息，直接丢弃
		if (msg->sMsgBuf->sDBHead.cRaflag == 1 && (msg->sMsgBuf->sDBHead.cCmd == CMD_DPPUSH || msg->sMsgBuf->sDBHead.cCmd == CMD_DPABC))
		{
//				m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"CMD_DPPUSH或CMD_DPABC的应答，丢弃");
		}
		else
		{
			if (1 == msg->sMsgBuf->sDBHead.cRaflag)
			{
				//写入BPC主动向外发的日志
				m_pBpc2DrebDataLog->LogDreb(LOG_PROMPT,msg->sMsgBuf,false);
			}
			else
			{
				//写入DREB主动发过来的日志
				m_pDreb2BpcDataLog->LogDreb(LOG_PROMPT,msg->sMsgBuf,false);
			}
		}
		

		//收多了，将原包重新给m_sRcvBuffer
		for (int i=0; i<m_nRcvBufLen-DREBHEADLEN-msg->sMsgBuf->sDBHead.nLen; i++)
		{
			m_sRcvBuffer[i]=m_sRcvBuffer[i+DREBHEADLEN+msg->sMsgBuf->sDBHead.nLen];
		}
		m_nRcvBufLen=m_nRcvBufLen - DREBHEADLEN-msg->sMsgBuf->sDBHead.nLen;
		
		return DREBHEADLEN+msg->sMsgBuf->sDBHead.nLen;
	}
	else if (m_nRcvBufLen>0)
	{
#ifdef _ENGLISH_
		m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"warn, index[%d] DREB[%d %d] type[%s] receive part data %d",\
			m_index,m_nDrebId,m_nDrebPrivateId,"BPCSOCK_DREB",m_nRcvBufLen);
#else
		m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"警告 index[%d] DREB[%d %d] type[%s] 收到数据%d,但不完整",\
			m_index,m_nDrebId,m_nDrebPrivateId,"BPCSOCK_DREB",m_nRcvBufLen);
#endif
	}
	
	return 0;
}

bool CBF_DrebSocketInfo::NeedConnect()
{
	return m_bNeedConnect;
}

bool CBF_DrebSocketInfo::NeedRead()
{
	return m_bNeedread;
}

bool CBF_DrebSocketInfo::NeedSend()
{
	if (m_sock == INVALID_SOCKET)
	{
		return false;
	}
	return m_pSendQueue.NeedSend();
}

bool CBF_DrebSocketInfo::UnzipBuf(BPCCOMMSTRU &data)
{
	//0明文 1des加密密钥长度16 2压缩 3DES加密 4DES加密并压缩
	if (data.sDBHead.nLen == 0)
	{
		data.sDBHead.cZip = 0;
		return true;
	}
	if (data.sDBHead.cZip >0 && data.sDBHead.cZip <=4)
	{
		char zipDataBuf[BPUDATASIZE];
		bzero(zipDataBuf,sizeof(zipDataBuf));
		unsigned int dlen = BPUDATASIZE;
		int ddlen;
		switch (data.sDBHead.cZip)
		{
			case 1:
				ddlen = data.sDBHead.nLen;
				if (m_pDes.Uncryptchar(data.sBuffer,ddlen,DESENCRYPTKEY)<=0)
				{
					m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Uncryptchar error");
					return false;
				}
				data.sDBHead.nLen = ddlen;
				data.sDBHead.cZip =0;
				break;
			case 2:	
				memcpy(zipDataBuf,data.sBuffer,data.sDBHead.nLen);
				if (!CBF_Tools::Uncompress((unsigned char *)(data.sBuffer),dlen,(unsigned char *)zipDataBuf,data.sDBHead.nLen))
				{
					m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Uncompress error");
					return false;
				}
				data.sDBHead.nLen = dlen;
				data.sDBHead.cZip =0;
				break;
			case 3:
				ddlen = data.sDBHead.nLen;
				if (m_pDes.Uncryptchar(data.sBuffer,ddlen,DESENCRYPTKEY3)<=0)
				{
					m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Uncryptchar error");
					return false;
				}
				data.sDBHead.nLen = ddlen;
				data.sDBHead.cZip =0;
				break;
			case 4:
				ddlen = data.sDBHead.nLen;
//				m_log->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"解密前数据",data.sBuffer,data.sDBHead.nLen);
				if (m_pDes.Uncryptchar(data.sBuffer,ddlen,DESENCRYPTKEY)<=0)
				{
					m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Uncryptchar error");
					return false;
				}
				data.sDBHead.nLen = ddlen;
//				m_log->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"解密后数据",data.sBuffer,data.sDBHead.nLen);
				memcpy(zipDataBuf,data.sBuffer,data.sDBHead.nLen);
				if (!CBF_Tools::Uncompress((unsigned char *)(data.sBuffer),dlen,(unsigned char *)zipDataBuf,data.sDBHead.nLen))
				{
					m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Uncompress error");
					return false;
				}
				data.sDBHead.nLen = dlen;
//				m_log->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"解压后数据",data.sBuffer,data.sDBHead.nLen);
				data.sDBHead.cZip =0;
				break;
			default:
				return false;
		}
	}
	
	return true;
}
bool CBF_DrebSocketInfo::ZipBuf(PBPCCOMMSTRU data,int zipflag)
{
	//0明文 1des加密密钥长度16 2压缩 3DES加密 4DES加密并压缩
	if (data->sDBHead.cZip != 0 || data->sDBHead.nLen<20)
	{
		return true;
	}
	char zipDataBuf[BPUDATASIZE];
	bzero(zipDataBuf,sizeof(zipDataBuf));
	unsigned int dlen = BPUDATASIZE;
	int ddlen;
	switch (zipflag)
	{
		case 1:
			ddlen = data->sDBHead.nLen;
			if (m_pDes.Encryptchar(data->sBuffer,ddlen,DESENCRYPTKEY)<=0)
			{
				m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Encryptchar error");
				return false;
			}
			data->sDBHead.nLen = ddlen;
			data->sDBHead.cZip =1;
			break;
		case 2:	
			memcpy(zipDataBuf,data->sBuffer,data->sDBHead.nLen);
			if (!CBF_Tools::Compress((unsigned char *)(data->sBuffer),dlen,(unsigned char *)(zipDataBuf),data->sDBHead.nLen))
			{
				m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"Compress error");
				return false;
			}
			data->sDBHead.nLen = dlen;
			data->sDBHead.cZip =2;
			break;
		case 3:
			ddlen = data->sDBHead.nLen;
			if (m_pDes.Encryptchar(data->sBuffer,ddlen,DESENCRYPTKEY3)<=0)
			{
				m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Encryptchar error");
				return false;
			}
			data->sDBHead.nLen = ddlen;
			data->sDBHead.cZip =3;
			break;
		case 4:
			memcpy(zipDataBuf,data->sBuffer,data->sDBHead.nLen);
//			m_log->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"压缩前数据",data->sBuffer,data->sDBHead.nLen);
			if (!CBF_Tools::Compress((unsigned char *)data->sBuffer,dlen,(unsigned char *)(zipDataBuf),data->sDBHead.nLen))
			{
				m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"Compress error");
				return false;
			}
			data->sDBHead.nLen = dlen;
			ddlen = dlen;
//			m_log->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"加密前数据",data->sBuffer,data->sDBHead.nLen);
			if (m_pDes.Encryptchar(data->sBuffer,ddlen,DESENCRYPTKEY)<=0)
			{
				m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Encryptchar error");
				return false;
			}
			data->sDBHead.nLen = ddlen;
//			m_log->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"加密后数据",data->sBuffer,data->sDBHead.nLen);
			data->sDBHead.cZip =4;
			break;
		default:
			data->sDBHead.cZip =0;
			return true;
	}
	return true;
}
void CBF_DrebSocketInfo::CheckFirstData()
{
	RDQUEUE::iterator prq,prn;
	for (prn=m_pSendQueue.m_qSendData.datas.begin(); prn!=m_pSendQueue.m_qSendData.datas.end() ; )
	{
		if (prn->nTotalLen != prn->nTotalLen)
		{
#ifdef _ENGLISH_
			m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"error, the first send part data,delete it %d",prn->index);
#else
			m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"错误 队列中第一个数据包发送部分数据，重连后删除 %d",prn->index);
#endif
			m_pSendQueue.DeleteSend(prn);
		}
		break;
	}
}
