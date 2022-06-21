// SocketInfo.cpp: implementation of the CSocketInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "SocketInfo.h"
#include "Bpc_Timer.h"


extern CBpc_Timer  g_pBpcTime;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define  DESENCRYPTKEY    "rd402wms"
#define  DESENCRYPTKEY3   "wms13711songfree"

CSocketInfo::CSocketInfo()
{
	memset(m_sAddress,0,sizeof(m_sAddress));         //请求连接上来的IP
	
	ResetInfo();
	m_pMemPool = NULL;
	m_gRes = NULL;
	m_nType = BPCSOCK_UNKNOW;
	m_nCloseTime = 0;
	m_nPid = -1;
	m_sendNum = 10;
	m_bNeedConnect = false;
	m_nBpuGroupIndex = 0;
	m_nBpcHeadLen = sizeof(S_BPC_HEAD);
	m_nDrebHeadLen = DREBHEADLEN;
	m_log= NULL;
	m_DrebIsIpc = false;
	
}

CSocketInfo::~CSocketInfo()
{
	if (m_sock != INVALID_SOCKET)
	{
		closesocket(m_sock);
	}
	m_pMemPool = NULL;
	m_gRes = NULL;
	m_log= NULL;
}
std::string CSocketInfo::GetBpcMsgType(int msgtype)
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
	default:
		return "";
	}
}
std::string CSocketInfo::GetDrebCmdType(int cmdtype)
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
	case  CMD_MONITOR_BPC:     //16 //数据总线节点监控
		return "CMD_MONITOR_BPC";
	default:
		return "";
	}
}

void CSocketInfo::Init(CGResource *res,CBF_BufferPool *pool)
{
	m_pMemPool = pool;
	m_gRes = res;

	m_log= m_gRes->m_log;
	ResetInfo();
	m_nCloseTime = 0;
	m_pDrebEndian.SetCommEndian(true);//使用主机序
}
void CSocketInfo::ResetInfo()
{
	memset(m_sAddress,0,sizeof(m_sAddress));
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
	m_bIsBuGetFlag = 0;
	m_sBpuGroupName = "";
	m_nBpuGroupIndex = 0;
}

void CSocketInfo::OnClose(const char *msg,const char *filename,int fileline)
{
	CBF_PMutex pp(&m_mutex);
	m_datetime.Update();
	m_datetime.ToCharDate(m_cCloseDate);
	
	//清空队列
	m_log->LogMp(LOG_DEBUG,filename,fileline,"%s,关闭连接[%s:%d] index[%d] type[%d] sock[%d]",\
		msg,m_sAddress,m_sock,m_index,m_nType,m_sock);
	
	closesocket(m_sock);
	m_sock = INVALID_SOCKET;
	if (m_sBpuGroupName.length()>0)
	{
		if (m_nBpuGroupIndex<m_gRes->g_vBpuGroupInfo.size())
		{
			m_gRes->g_vBpuGroupInfo[m_nBpuGroupIndex].g_nStartBpuNum--;
		}
	}
	ResetInfo();
	m_nCloseTime = time(NULL);
	if (m_nType == BPCSOCK_DREB) //是主动建立连接
	{
		m_bNeedConnect = true; //需要连接
		CheckFirstData();

	}
	else
	{
		m_bNeedConnect = false;
		m_pSendQueue.Clear();
		
	}
	
}

void CSocketInfo::OnShutdown()
{
	closesocket(m_sock);
	//清空队列
	ResetInfo();
	m_nCloseTime = 0;
}
bool CSocketInfo::IsCanSelect()
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

bool CSocketInfo::Listen(int port,bool blockflag,int queuenum)
{
	if (!m_tcpSocket.Create(AF_INET,false))
	{
		CBF_Tools::StringCopy(m_errMsg,255,m_tcpSocket.GetErrorMsg().c_str());
		return false;
	}
	m_nPort = port;
	if (!m_tcpSocket.Listen(m_nPort,queuenum))
	{
		sprintf(m_errMsg,"侦听端口[%d]失败,侦听队列大小%d",m_nPort,queuenum);
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

bool CSocketInfo::ListenIpc(int port,bool blockflag,int queuenum)
{
	if (!m_tcpSocket.Create(AF_UNIX,false))
	{
		CBF_Tools::StringCopy(m_errMsg,255,m_tcpSocket.GetErrorMsg().c_str());
		return false;
	}
	m_nPort = port;
	if (!m_tcpSocket.Listen(m_nPort,queuenum))
	{
		sprintf(m_errMsg,"侦听端口[%d]失败,侦听队列大小%d",m_nPort,queuenum);
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
bool CSocketInfo::IsFree()
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
int CSocketInfo::OnRecv()
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
//		if (m_nType == BPCSOCK_DREB) //dreb收到的数据
//		{
//			m_gRes->PutTime();
//        }
		return ret;

	}
	return 0;
}

int CSocketInfo::OnSend()
{
	int ret;
	if (m_sock == INVALID_SOCKET)
	{
		m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"连接已断开");
		return -2;
	}
	if (!m_pSendQueue.NeedSend())
	{
		return -3;
	}
	CBF_PMutex pp(&m_mutex);
	RDQUEUE::iterator prq,prn;
	unsigned int atimeout;
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
		m_tcpSocket.AttachSocket(m_sock,"");
		
		if (m_nType != BPCSOCK_DREB )
		{
			ret = m_tcpSocket.Send((char *)prq->sMsgBuf+(prq->nTotalLen-prq->nSendLen),prq->nSendLen);
			if (ret == prq->nSendLen)
			{
				g_pBpcTime.ReportSend(prq->nSendLen);
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送给BPU[%s %d] 源[%d %d %d] 的消息成功 数据长度[%d]",\
					m_sBpuGroupName.c_str(),m_index,prq->s_nNodeId,prq->s_cNodePrivateId,prq->s_nDrebSerial,ret);
				m_pMemPool->PoolFree(prq->sMsgBuf);
				// 处理成功,停止
				m_pSendQueue.DeleteSend(prq);	
				m_nSendtime = time(NULL);
			}
			else //发送不成功
			{
				if (ret >0)
				{
					g_pBpcTime.ReportSend(ret);
					prq->nSendLen = prq->nSendLen -ret;
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送给BPU[%s %d] 源[%d %d %d] 的消息部分成功 数据长度[%d]",\
						m_sBpuGroupName.c_str(),m_index,prq->s_nNodeId,prq->s_cNodePrivateId,prq->s_nDrebSerial,ret);
				}
				//下次再发，只到发送完成或连接关闭
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送给BPU[%s %d] 源[%d %d %d] 的消息失败 ret[%d]",\
					m_sBpuGroupName.c_str(),m_index,prq->s_nNodeId,prq->s_cNodePrivateId,prq->s_nDrebSerial,ret);
				//发送不成功，下次再发，还是从这里再发
				return 0;

			}
			
		}
		else   //发给DREB的数据
		{
			ret = m_tcpSocket.Send((char *)&(prq->sMsgBuf->sDBHead)+(prq->nTotalLen-prq->nSendLen),prq->nSendLen);
			if (ret == prq->nSendLen)
			{
				//统计数据
				g_pBpcTime.ReportSend(prq->nSendLen);
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送给index[%d] DREB[%d %d] 源[%d %d %d] 的消息成功 数据长度[%d]",\
					m_index,m_nDrebId,m_nDrebPrivateId,prq->s_nNodeId,prq->s_cNodePrivateId,prq->s_nDrebSerial,ret);
				m_pMemPool->PoolFree(prq->sMsgBuf);
				// 处理成功,停止
				m_pSendQueue.DeleteSend(prq);	
				m_nSendtime = time(NULL);
				
			}
			else //停止
			{
				if (ret >0)
				{
					g_pBpcTime.ReportSend(ret);
					prq->nSendLen = prq->nSendLen -ret;
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送给index[%d] DREB[%d %d] 源[%d %d %d] 的消息部分成功 数据长度[%d]",\
						m_index,m_nDrebId,m_nDrebPrivateId,prq->s_nNodeId,prq->s_cNodePrivateId,prq->s_nDrebSerial,ret);
				}
				else
				{
					//下次再发，直到成功或连接关闭
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送给index[%d] DREB[%d %d] 源[%d %d %d] 的消息失败 ret[%d]",\
						m_index,m_nDrebId,m_nDrebPrivateId,prq->s_nNodeId,prq->s_cNodePrivateId,prq->s_nDrebSerial,ret);
				}
				//发送不成功，下次再发，还是从这里再发
				return 0;
			}
		}
		
	}
	

	return sendNum;
}


int CSocketInfo::SendMsg(S_BPC_RSMSG &msg,bool sendimmediate)
{
	int ret=0;
	CBF_PMutex pp(&m_mutex);
	m_tcpSocket.AttachSocket(m_sock,"");
	//将DREBHEAD中的数据放入MSG中，供发送时写日志进行跟踪
	msg.s_nNodeId = msg.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId;
	msg.s_nDrebSerial = msg.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial;
	msg.s_cNodePrivateId = msg.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId;
	if (m_nType != BPCSOCK_DREB )
	{
		//此bpu组要求明文发送
		if (m_gRes->g_vBpuGroupInfo[m_nBpuGroupIndex].g_bIsUnencrypt)
		{
			if (msg.sMsgBuf->sDBHead.nLen >0)
			{
				if (!UnzipBuf(*(msg.sMsgBuf)))
				{
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"错误 解压缩解密出错 发送给BPU[%s %d]的消息[%s]  DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 源[%d %d] cZip[%d] 业务数据长度[%d] ",\
						m_sBpuGroupName.c_str(),m_index,GetBpcMsgType(msg.sMsgBuf->sBpcHead.cMsgType).c_str(),\
						GetDrebCmdType(msg.sMsgBuf->sDBHead.cCmd).c_str(),msg.sMsgBuf->sDBHead.cNextFlag,\
						msg.sMsgBuf->sDBHead.cRaflag,msg.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,msg.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
						msg.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,msg.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
						msg.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,msg.sMsgBuf->sDBHead.s_Sinfo.s_nHook,msg.sMsgBuf->sDBHead.cZip,msg.sMsgBuf->sDBHead.nLen);
// 					m_pMemPool->PoolFree(msg.sMsgBuf);
// 					msg.sMsgBuf = NULL;
					return -101;
				}
			}
			
		}
		msg.sMsgBuf->sBpcHead.nBpcLen = msg.sMsgBuf->sDBHead.nLen +DREBHEADLEN;
		msg.nTotalLen = msg.sMsgBuf->sDBHead.nLen+BPCHEADLEN+DREBHEADLEN;
		msg.nSendLen = msg.nTotalLen;
		if (msg.sMsgBuf->sBpcHead.cMsgType != MSG_FREE && msg.sMsgBuf->sBpcHead.cMsgType != MSG_BPUISREG && msg.sMsgBuf->sBpcHead.cMsgType != MSG_BPUREG)
		{
			m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"发送给BPU[%s %d]的消息[%s]  DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 源[%d %d] cZip[%d] 业务数据长度[%d]",\
				m_sBpuGroupName.c_str(),m_index,GetBpcMsgType(msg.sMsgBuf->sBpcHead.cMsgType).c_str(),\
				GetDrebCmdType(msg.sMsgBuf->sDBHead.cCmd).c_str(),msg.sMsgBuf->sDBHead.cNextFlag,\
				msg.sMsgBuf->sDBHead.cRaflag,msg.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,msg.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
				msg.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,msg.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
				msg.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,msg.sMsgBuf->sDBHead.s_Sinfo.s_nHook,msg.sMsgBuf->sDBHead.cZip,msg.sMsgBuf->sDBHead.nLen);
		}
		else
		{
			m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"发送给BPU[%s %d]的消息[%s] 业务数据长度[%d]",\
				m_sBpuGroupName.c_str(),m_index,GetBpcMsgType(msg.sMsgBuf->sBpcHead.cMsgType).c_str(),msg.sMsgBuf->sDBHead.nLen);
		}
		//转换成通讯字节序,这里为主机字节序

		if (m_gRes->g_vBpuGroupInfo[m_nBpuGroupIndex].g_nUseBigEndian == 1) //java版的bpu使用网络序时
		{
			m_pBpcEndian.SetCommEndian(false);
		}
		else
		{
			m_pBpcEndian.SetCommEndian(true);
		}
		
		m_pBpcEndian.Endian2Comm(msg.sMsgBuf);
		m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"准备发给BPU");
		//发送队列若有数据，则放入发送队列，按顺序发送
		if (NeedSend())
		{
			m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"发送队列已有数据，放入BPU队列 index[%d] 数据长度[%d] 标识[%d %d %d] ",\
				m_index,msg.nTotalLen,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
			PutSendMsg(msg);
			return 0;
		}
		else  //直接发送
		{
			if (m_sock == INVALID_SOCKET)
			{
				m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"连接已断开，放入BPU队列 index[%d] 数据长度[%d] 标识[%d %d %d] ",\
					m_index,msg.nTotalLen,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
				PutSendMsg(msg);
				return -2;
			}
			ret = m_tcpSocket.Send((char *)(msg.sMsgBuf),msg.nTotalLen);
			if (ret == msg.nTotalLen)
			{
				m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"发送给BPU完成 index[%d] 标识[%d %d %d] ",\
					m_index,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
				g_pBpcTime.ReportSend(msg.nTotalLen);
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
// 				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"错误 发送给BPU[%d]的消息出错,放入队列 ret返回[%d] msg[%s] 放入长度[%d]",\
// 					m_index,ret,m_tcpSocket.GetErrorMsg().c_str(),msg.nSendLen);
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"错误 发送给BPU index[%d]的消息出错,放入队列 ret返回[%d] msg[%s] 放入长度[%d] 数据长度[%d] 标识[%d %d %d] ",\
					m_index,ret,m_tcpSocket.GetErrorMsg().c_str(),msg.nSendLen,msg.nTotalLen,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
				PutSendMsg(msg);
				return 0;
			}
		}
		return 0;
		
	}
	else   //发给DREB的数据
	{
		msg.nTotalLen = msg.sMsgBuf->sDBHead.nLen+DREBHEADLEN;
		msg.nSendLen = msg.nTotalLen;
		if (msg.sMsgBuf->sDBHead.cCmd != CMD_PING && msg.sMsgBuf->sDBHead.cCmd != CMD_CONNECT && msg.sMsgBuf->sDBHead.cCmd != CMD_REGSERVICE)
		{
			m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"发送给DREB[%d]的消息 DREB命令[%s] RA标志[%d] 交易码[%d]  标识[%d %d %d] 源[%d %d] cZip[%d] 业务数据长度[%d]",\
				m_index,GetDrebCmdType(msg.sMsgBuf->sDBHead.cCmd).c_str(),\
				msg.sMsgBuf->sDBHead.cRaflag,msg.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,msg.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
				msg.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,msg.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
				msg.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,msg.sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
				msg.sMsgBuf->sDBHead.cZip,msg.sMsgBuf->sDBHead.nLen);
		}
		else
		{
			m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"发送给DREB[%d]的消息 DREB命令[%s]  业务数据长度[%d]",\
				m_index,GetDrebCmdType(msg.sMsgBuf->sDBHead.cCmd).c_str(),msg.sMsgBuf->sDBHead.nLen);
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
				m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"连接已断开，放入DREB队列 数据长度[%d] 标识[%d %d %d] ",msg.nTotalLen,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
				PutSendMsg(msg);
				return -2;
			}
			ret = m_tcpSocket.Send((char *)&(msg.sMsgBuf->sDBHead),msg.nTotalLen);
			if (ret == msg.nTotalLen)
			{
				m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"发送完成 index[%d] 标识[%d %d %d] ",\
					m_index,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
				//统计数据
				g_pBpcTime.ReportSend(msg.nTotalLen);
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
// 				m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"警告 发送给DREB[%d]的消息出错 放入队列 ret返回[%d] msg[%s] 放入长度[%d]",\
// 					m_index,ret,m_tcpSocket.GetErrorMsg().c_str(),msg.nSendLen);
				m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"警告 发送给DREB[%d]的消息出错,放入队列 ret返回[%d] msg[%s] 放入长度[%d] 数据长度[%d] 标识[%d %d %d] ",\
					m_index,ret,m_tcpSocket.GetErrorMsg().c_str(),msg.nSendLen,msg.nTotalLen,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
				//只要发送出错就放到队列，如果队列里有数据可能就会有问题
				if (!m_pSendQueue.NeedSend())
				{
					PutSendMsg(msg);
				}
				else
				{
					OnClose("直接在连接发送数据不成功，但连接队列里仍有数据",__FILE__,__LINE__);
				}
				return 0;
			}
			
			
		}
		else
		{
			if (NeedSend()) //连接正常
			{
				m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"发送队列已有数据，放入DREB队列 index[%d] 数据长度[%d] 标识[%d %d %d] ",\
					m_index,msg.nTotalLen,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
				if (msg.sMsgBuf->sBpcHead.cMsgType != MSG_BPCMONITOR)
				{
					PutSendMsg(msg);
					return 0;
				}
				
			}
			else  //直接发送
			{
				if (m_sock == INVALID_SOCKET)
				{
					m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"连接已断开，放入DREB队列 index[%d] 数据长度[%d] 标识[%d %d %d] ",\
						m_index,msg.nTotalLen,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
					PutSendMsg(msg);
					return -2;
				}
				ret = m_tcpSocket.Send((char *)&(msg.sMsgBuf->sDBHead),msg.nTotalLen);
				if (ret == msg.nTotalLen)
				{
					m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"发送完成 index[%d] 标识[%d %d %d] ",\
						m_index,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
					//统计数据
					g_pBpcTime.ReportSend(msg.nTotalLen);
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
// 					m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"警告 发送给DREB[%d]的消息出错 放入队列 ret返回[%d] msg[%s] 放入长度[%d]",\
// 						m_index,ret,m_tcpSocket.GetErrorMsg().c_str(),msg.nSendLen);
					m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"警告 发送给DREB[%d]的消息出错,放入队列 ret返回[%d] msg[%s] 放入长度[%d] 数据长度[%d] 标识[%d %d %d] ",\
						m_index,ret,m_tcpSocket.GetErrorMsg().c_str(),msg.nSendLen,msg.nTotalLen,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
					//只要发送出错就放入队列
					PutSendMsg(msg);
					return 0;
				}
			}
		}
		
			
	}
	return ret;
}

int CSocketInfo::PutSendMsg(S_BPC_RSMSG msg)
{
	int ret;
	ret = m_pSendQueue.PutSendMsg(msg);
	if (ret<0)
	{
		m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"队列满 丢弃 DREB[%d]的消息 DREB命令[%s] RA标志[%d] 交易码[%d]  标识[%d %d %d] 源[%d %d] cZip[%d] 业务数据长度[%d]",\
			m_index,GetDrebCmdType(msg.sMsgBuf->sDBHead.cCmd).c_str(),\
			msg.sMsgBuf->sDBHead.cRaflag,msg.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,msg.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
			msg.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,msg.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
			msg.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,msg.sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
				msg.sMsgBuf->sDBHead.cZip,msg.sMsgBuf->sDBHead.nLen);
		m_pMemPool->PoolFree(msg.sMsgBuf);
		msg.sMsgBuf = NULL;
		return -1;
	}
	return 0;
}



int CSocketInfo::GetRecvData(S_BPC_RSMSG *msg)
{
	int txlen=0;
	char tmpchar[100];
	memset(tmpchar,0,sizeof(tmpchar));
	if (msg->sMsgBuf == NULL)
	{
		return -100;
	}
	if (m_nType == BPCSOCK_DREB) //dreb收到的数据
	{
		//收到的数据大于包头
		if (m_nRcvBufLen >= DREBHEADLEN )
		{
//			m_gRes->PutTime();
			//进行字节序处理并校验CRC
			memcpy(&(msg->sMsgBuf->sDBHead),m_sRcvBuffer,DREBHEADLEN);
			if (m_gRes->g_nCrcFlag == 1)
			{
				if (!m_pDrebEndian.Endian2LocalHostChkCrc(&(msg->sMsgBuf->sDBHead)) )
				{
					OnClose("错误 CRC错",__FILE__,__LINE__);
					return -1;
				}
			}
			else
			{
				m_pDrebEndian.Endian2LocalHost(&(msg->sMsgBuf->sDBHead));

			}
			if (m_nRcvBufLen < msg->sMsgBuf->sDBHead.nLen + DREBHEADLEN)
			{
				//说明数据没有接收完整
				return 0;
			}
			if (msg->sMsgBuf->sDBHead.nLen >=0 && msg->sMsgBuf->sDBHead.nLen <= BPUDATASIZE ) 
			{
				if (msg->sMsgBuf->sDBHead.nLen >0)
				{
					memcpy(msg->sMsgBuf->sBuffer,m_sRcvBuffer+DREBHEADLEN,msg->sMsgBuf->sDBHead.nLen);
				}
			}
			else
			{
				OnClose("错误 连接非DREB",__FILE__,__LINE__);
				return -1;
			}
//			m_gRes->PutTime();
			
			msg->sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN+msg->sMsgBuf->sDBHead.nLen;
			msg->sMsgBuf->sBpcHead.nBpuIndex = 0;
			msg->sMsgBuf->sBpcHead.nConnectTime = m_nConntime;
			msg->sMsgBuf->sBpcHead.nIndex = m_index; 
			msg->sMsgBuf->sBpcHead.cMsgType = MSG_REQ;
			msg->sMsgBuf->sBpcHead.nRtime = time(NULL);
			
			//统计数据
			g_pBpcTime.ReportRcv(msg->sMsgBuf->sBpcHead.nBpcLen);
			if (msg->sMsgBuf->sDBHead.cCmd != CMD_PING && msg->sMsgBuf->sDBHead.cCmd != CMD_CONNECT && msg->sMsgBuf->sDBHead.cCmd != CMD_REGSERVICE)
			{
				if (m_log->isWrite(LOG_DEBUG))
				{
					m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"从index[%d] DREB[%d %d] 收到源DREB[%d %d]收到的消息 DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 源[%d %d] cZip[%d] 业务数据长度[%d]",\
						m_index,m_nDrebId,m_nDrebPrivateId,msg->sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,msg->sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,\
						GetDrebCmdType(msg->sMsgBuf->sDBHead.cCmd).c_str(),msg->sMsgBuf->sDBHead.cNextFlag,\
						msg->sMsgBuf->sDBHead.cRaflag,msg->sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,msg->sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
						msg->sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,msg->sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
						msg->sMsgBuf->sDBHead.s_Sinfo.s_nSerial,msg->sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
						msg->sMsgBuf->sDBHead.cZip,msg->sMsgBuf->sDBHead.nLen);
				}
				
			}
			else
			{
				if (m_log->isWrite(LOG_DEBUG+1))
				{
					m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"接收到index[%d] DREB[%d %d]的消息 DREB命令[%s]  业务数据长度[%d]",\
						m_index,m_nDrebId,m_nDrebPrivateId,GetDrebCmdType(msg->sMsgBuf->sDBHead.cCmd).c_str(),msg->sMsgBuf->sDBHead.nLen);
				}
			}
			//若是应答且为不可靠推送或全域广播消息，直接丢弃
			if (msg->sMsgBuf->sDBHead.cRaflag == 1 && (msg->sMsgBuf->sDBHead.cCmd == CMD_DPPUSH || msg->sMsgBuf->sDBHead.cCmd == CMD_DPABC))
			{
//				m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"CMD_DPPUSH或CMD_DPABC的应答，丢弃");
			}
			else
			{

			}
			

			//收多了，将原包重新给m_sRcvBuffer
			m_nRcvBufLen=m_nRcvBufLen - DREBHEADLEN-msg->sMsgBuf->sDBHead.nLen;
			for (int i=0; i<m_nRcvBufLen; i++)
			{
				m_sRcvBuffer[i]=m_sRcvBuffer[i+DREBHEADLEN+msg->sMsgBuf->sDBHead.nLen];
			}
			
//			m_gRes->PutTime();
			return DREBHEADLEN+msg->sMsgBuf->sDBHead.nLen;
		}
		else if (m_nRcvBufLen>0)
		{
			m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"警告 index[%d] DREB[%d %d] type[%s] 收到数据%d,但不完整",\
				m_index,m_nDrebId,m_nDrebPrivateId,"BPCSOCK_DREB",m_nRcvBufLen);
		}
	}
	else  // if (m_nType == BPCSOCK_BPU || m_nType == BPCSOCK_CLI) //收BPU数据
	{
	    m_log->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"BPU[%s %d %d] type[%s] 收到数据%d",\
			m_sBpuGroupName.c_str(),m_index,m_nBpuGroupIndex,"BPU_SOCKET",m_nRcvBufLen);
		//收到的数据大于包头
		if (m_nRcvBufLen >= BPCHEADLEN+DREBHEADLEN)
		{
			memcpy(&(msg->sMsgBuf->sBpcHead),m_sRcvBuffer,BPCHEADLEN);
			if (msg->sMsgBuf->sBpcHead.nMsgFlag != LMSGID)
			{
				m_pBpcEndian.SetCommEndian(false);//对方使用网络序
			}
			else
			{
				m_pBpcEndian.SetCommEndian(true);
			}
			memcpy(&(msg->sMsgBuf->sDBHead),m_sRcvBuffer+BPCHEADLEN,DREBHEADLEN);
			//字节序处理
			m_pBpcEndian.Endian2LocalHost(msg->sMsgBuf);

			//以sDBHead.nLen为准来算长度
			if (m_nRcvBufLen < BPCHEADLEN+DREBHEADLEN+msg->sMsgBuf->sDBHead.nLen)
			{
				if (m_nRcvBufLen >= BPCBUFSIZE || msg->sMsgBuf->sBpcHead.nBpcLen>BPUDATASIZE+DREBHEADLEN || msg->sMsgBuf->sDBHead.nLen>BPUDATASIZE)
				{
					OnClose("错误 连接非BPU,报文头过大",__FILE__,__LINE__);
					return -1;
				}
				m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"警告 BPU[%s  %d] 数据没有接收完整[%d] < HEADLEN[%d]+nLen[%d]",\
					m_sBpuGroupName.c_str(),m_index,m_nRcvBufLen,BPCHEADLEN+DREBHEADLEN,msg->sMsgBuf->sDBHead.nLen);
				//说明数据没有接收完整
				return 0;
			}
			if (msg->sMsgBuf->sDBHead.nLen>0 && msg->sMsgBuf->sDBHead.nLen<BPUDATASIZE)
			{
				memcpy(msg->sMsgBuf->sBuffer,m_sRcvBuffer+(BPCHEADLEN+DREBHEADLEN),msg->sMsgBuf->sDBHead.nLen);
				msg->sMsgBuf->sBuffer[msg->sMsgBuf->sDBHead.nLen]=0;
			}
			else if (msg->sMsgBuf->sDBHead.nLen >= BPUDATASIZE)
			{
				OnClose("错误 连接非BPU,报文体过大",__FILE__,__LINE__);
				return -1;
			}
			if (msg->sMsgBuf->sBpcHead.cMsgType != MSG_FREE && msg->sMsgBuf->sBpcHead.cMsgType != MSG_BPUISREG && msg->sMsgBuf->sBpcHead.cMsgType != MSG_BPUREG)
			{
				m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"接收到BPU[%s %d]的消息[%s]  DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 源[%d %d] cZip[%d] 业务数据长度[%d]",\
					m_sBpuGroupName.c_str(),m_index,GetBpcMsgType(msg->sMsgBuf->sBpcHead.cMsgType).c_str(),\
					GetDrebCmdType(msg->sMsgBuf->sDBHead.cCmd).c_str(),msg->sMsgBuf->sDBHead.cNextFlag,\
					msg->sMsgBuf->sDBHead.cRaflag,msg->sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,\
					msg->sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
					msg->sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,msg->sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
					msg->sMsgBuf->sDBHead.s_Sinfo.s_nSerial,msg->sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
					msg->sMsgBuf->sDBHead.cZip,msg->sMsgBuf->sDBHead.nLen);
			}
			else
			{
				m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"接收到BPU[%s %d]的消息[%s] 业务数据长度[%d]",\
					m_sBpuGroupName.c_str(),m_index,GetBpcMsgType(msg->sMsgBuf->sBpcHead.cMsgType).c_str(),msg->sMsgBuf->sDBHead.nLen);
			}
			CBF_Tools::StringCopy(msg->sBpuGroupName,20,m_sBpuGroupName.c_str());
			msg->nBpuGroupIndex = m_nBpuGroupIndex;
			//若是请求的话将接收到请求的连接index放到bpc头的nBpuIndex中
			if (msg->sMsgBuf->sDBHead.cRaflag == 0) 
			{
				msg->sMsgBuf->sBpcHead.nBpuIndex = m_index;//只有在请求时才要更新，因为有内调，内调根据此值返回给调用的bpu
				//m_gRes->m_pBpcCallDataLog.LogBpc(LOG_PROMPT,msg->sMsgBuf,false);
			}
			else
			{
				//m_gRes->m_pDrebDataLog.LogBpc(LOG_PROMPT,msg->sMsgBuf,false);
			}
			//g_pBpcLog.LogBpc(LOG_PROMPT,msg->sMsgBuf,false);
			//收多了，将原包重新给m_sRcvBuffer
			for (int i=0; i<m_nRcvBufLen-BPCHEADLEN-DREBHEADLEN-msg->sMsgBuf->sDBHead.nLen; i++)
			{
				m_sRcvBuffer[i]=m_sRcvBuffer[i+BPCHEADLEN+DREBHEADLEN+msg->sMsgBuf->sDBHead.nLen];
			}
			m_nRcvBufLen = m_nRcvBufLen - BPCHEADLEN -DREBHEADLEN-msg->sMsgBuf->sDBHead.nLen;
			
			
			return BPCHEADLEN+DREBHEADLEN+msg->sMsgBuf->sDBHead.nLen;
		}
		else if (m_nRcvBufLen>0)
		{
			m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"警告 BPU[%s %d] type[%s] 收到数据%d,但不完整",\
				m_sBpuGroupName.c_str(),m_index,"BPCSOCK_BPU",m_nRcvBufLen);
		}
	}
	return 0;
}

bool CSocketInfo::NeedConnect()
{
	return m_bNeedConnect;
}

bool CSocketInfo::NeedRead()
{
	return m_bNeedread;
}

bool CSocketInfo::NeedSend()
{
	return m_pSendQueue.NeedSend();
}
void CSocketInfo::SetAccept(SOCKET_HANDLE sock,const char *address)
{
	ResetInfo();
	m_sock = sock;
	m_bChecked = false;
	m_bNeedread = true;
	m_nConntime = time(NULL);
	strcpy(m_sAddress,address);
	m_datetime.Update();
	m_datetime.ToCharDate(m_cCreateDate);
	m_nType = BPCSOCK_BPU;
}

bool CSocketInfo::UnzipBuf(BPCCOMMSTRU &data)
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
					m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解密出错");
					return false;
				}
				data.sDBHead.nLen = ddlen;
				data.sDBHead.cZip =0;
				break;
			case 2:	
				memcpy(zipDataBuf,data.sBuffer,data.sDBHead.nLen);
				if (!CBF_Tools::Uncompress((unsigned char *)(data.sBuffer),dlen,(unsigned char *)zipDataBuf,data.sDBHead.nLen))
				{
					m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解压缩出错");
					return false;
				}
				data.sDBHead.nLen = dlen;
				data.sDBHead.cZip =0;
				break;
			case 3:
				ddlen = data.sDBHead.nLen;
				if (m_pDes.Uncryptchar(data.sBuffer,ddlen,DESENCRYPTKEY3)<=0)
				{
					m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解密出错");
					return false;
				}
				data.sDBHead.nLen = ddlen;
				data.sDBHead.cZip =0;
				break;
			case 4:
				ddlen = data.sDBHead.nLen;
				m_log->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"解密前数据",data.sBuffer,data.sDBHead.nLen);
				if (m_pDes.Uncryptchar(data.sBuffer,ddlen,DESENCRYPTKEY)<=0)
				{
					m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解密出错");
					return false;
				}
				data.sDBHead.nLen = ddlen;
				m_log->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"解密后数据",data.sBuffer,data.sDBHead.nLen);
				memcpy(zipDataBuf,data.sBuffer,data.sDBHead.nLen);
				if (!CBF_Tools::Uncompress((unsigned char *)(data.sBuffer),dlen,(unsigned char *)zipDataBuf,data.sDBHead.nLen))
				{
					m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解压缩出错");
					return false;
				}
				data.sDBHead.nLen = dlen;
				m_log->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"解压后数据",data.sBuffer,data.sDBHead.nLen);
				data.sDBHead.cZip =0;
				break;
			default:
				return false;
		}
	}
	
	return true;
}
bool CSocketInfo::ZipBuf(PBPCCOMMSTRU data,int zipflag)
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
				m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"加密出错");
				return false;
			}
			data->sDBHead.nLen = ddlen;
			data->sDBHead.cZip =1;
			break;
		case 2:	
			memcpy(zipDataBuf,data->sBuffer,data->sDBHead.nLen);
			if (!CBF_Tools::Compress((unsigned char *)(data->sBuffer),dlen,(unsigned char *)(zipDataBuf),data->sDBHead.nLen))
			{
				m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"数据压缩失败");
				return false;
			}
			data->sDBHead.nLen = dlen;
			data->sDBHead.cZip =2;
			break;
		case 3:
			ddlen = data->sDBHead.nLen;
			if (m_pDes.Encryptchar(data->sBuffer,ddlen,DESENCRYPTKEY3)<=0)
			{
				m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"加密出错");
				return false;
			}
			data->sDBHead.nLen = ddlen;
			data->sDBHead.cZip =3;
			break;
		case 4:
			memcpy(zipDataBuf,data->sBuffer,data->sDBHead.nLen);
			m_log->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"压缩前数据",data->sBuffer,data->sDBHead.nLen);
			if (!CBF_Tools::Compress((unsigned char *)data->sBuffer,dlen,(unsigned char *)(zipDataBuf),data->sDBHead.nLen))
			{
				m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"数据压缩失败");
				return false;
			}
			data->sDBHead.nLen = dlen;
			ddlen = dlen;
			m_log->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"加密前数据",data->sBuffer,data->sDBHead.nLen);
			if (m_pDes.Encryptchar(data->sBuffer,ddlen,DESENCRYPTKEY)<=0)
			{
				m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"加密出错");
				return false;
			}
			data->sDBHead.nLen = ddlen;
			m_log->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"加密后数据",data->sBuffer,data->sDBHead.nLen);
			data->sDBHead.cZip =4;
			break;
		default:
			data->sDBHead.cZip =0;
			return true;
	}
	return true;
}
void CSocketInfo::CheckFirstData()
{
	RDQUEUE::iterator prq,prn;
	for (prn=m_pSendQueue.m_qSendData.datas.begin(); prn!=m_pSendQueue.m_qSendData.datas.end() ; )
	{
		if (prn->nTotalLen != prn->nTotalLen)
		{
			m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"错误 队列中第一个数据包发送部分数据，重连后删除 %d",prn->index);
			m_pSendQueue.DeleteSend(prn);
		}
		break;
	}
}

void CSocketInfo::SetBpuFreeFlag(bool isfree)
{
	CBF_PMutex pp(&m_mutexFreeFlag);
	m_bIsBuGetFlag = isfree;
	return;
}

bool CSocketInfo::IsBpuFree()
{
 	return m_bIsBuGetFlag;
}
