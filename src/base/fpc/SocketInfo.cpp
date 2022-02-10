// SocketInfo.cpp: implementation of the CSocketInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "SocketInfo.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CSocketInfo::CSocketInfo()
{
	memset(m_sAddress,0,sizeof(m_sAddress));         //请求连接上来的IP
	m_sRcvBuffer = (char *)malloc(BFBUFSIZE);
	m_sSendBuffer = (char *)malloc(BFBUFSIZE);
	ResetInfo();
	m_pMemPool = NULL;
	m_nType = SOCK_UNKNOW;
	m_nCloseTime = 0;
	m_nQueueTime = 60;
	m_sendNum = 10;
	m_bIsBuGetFlag = false;
	m_nPid = -1;
	m_log = NULL;
	
}

CSocketInfo::~CSocketInfo()
{
	if (m_sock != INVALID_SOCKET)
	{
		OnShutdown();
	}
	m_pMemPool = NULL;
	free(m_sRcvBuffer);
	free(m_sSendBuffer);
}

void CSocketInfo::Init(CGResource *res,CBF_BufferPool *pool)
{
	m_pMemPool = pool;
	m_gRes = res;

	m_log = &(m_gRes->m_log);
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

void CSocketInfo::OnClose(const char *msg)
{
	CBF_PMutex pp(&m_mutex);
	m_publog.Update();
	m_publog.ToCharDate(m_cCloseDate);
	//清空队列
	m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"%s,关闭连接%s:%d:%d",msg,m_sAddress,m_sock,m_index);

	closesocket(m_sock);
	m_sock = INVALID_SOCKET;
	ClearQueue();
	ResetInfo();
	m_nCloseTime = time(NULL);
	
}

void CSocketInfo::OnShutdown()
{
	closesocket(m_sock);
	//清空队列
	ClearQueue();
	ResetInfo();
	m_nCloseTime = 0;
}

bool CSocketInfo::IsFree()
{
	if (m_sock != INVALID_SOCKET)
	{
		return false;
	}
	if (m_nType == SOCK_LISTEN)//是侦听
	{
		return false;
	}
	return true;
}

void CSocketInfo::ClearQueue()
{
	RDQUEUE::iterator prn;
	
	while (m_pSendQueue.m_qSendData.datas.size()>0)	
	{
		prn=m_pSendQueue.m_qSendData.datas.begin();
		if (prn == m_pSendQueue.m_qSendData.datas.end())
		{
			break;
		}
		m_log->LogBin(LOG_ERROR,__FILE__,__LINE__,"清空队列时待发送的数据",prn->msgbuf.message->buffer,prn->msgbuf.head.len);
		m_pMemPool->PoolFree(prn->msgbuf.message);
		m_pSendQueue.DeleteSend(prn);
	}
	
}

bool CSocketInfo::Listen(int port,bool blockflag,int queuenum)
{
	if (!m_tcpSocket.Create(AF_INET,blockflag))
	{
		strcpy(m_errMsg,m_tcpSocket.GetErrorMsg().c_str());
		return false;
	}
	m_nPort = port;
	if (!m_tcpSocket.Listen(m_nPort,queuenum))
	{
		sprintf(m_errMsg,"侦听端口[%d]失败,侦听队列大小%d",m_nPort,queuenum);
		return false;
	}

	m_nType   =  SOCK_LISTEN;      //侦听
	m_sock = m_tcpSocket.GetSocket();
#ifndef _WINDOWS
	fcntl(m_sock,F_SETFD,25);
#endif
	m_bNeedread = true;

	return true;
}

void CSocketInfo::ResetInfo()
{
	memset(m_sAddress,0,sizeof(m_sAddress));
	memset(m_cCreateDate,0,sizeof(m_cCreateDate));
	memset(m_cCloseDate,0,sizeof(m_cCloseDate));


	//心跳参数
	m_nBeatheartcount=0;        //发送心跳次数
	m_bIsBeat=false;                //本通讯平台是否要发心跳
	//socket公共参数
	m_sock = INVALID_SOCKET;                   //socket id
	m_nType = SOCK_UNKNOW;                  //socket type SOCK_UNKNOW SOCK_CLI SOCK_DRTP SOCK_SRV SOCK_CENTER
	m_bNeedread=false;              //是否需要读数据
	m_bIsBuGetFlag = false;
	
	m_nReadtime= 0;              //上次读数据时间
	m_nConntime=0;              //连接建立时间
	m_nSendtime=0;              //上次发送数据时间 
	m_nPingTime = 0;
	
	//发送接收数据	
	
	memset(m_sRcvBuffer,0,BFBUFSIZE);            //接收数据缓冲   
	m_nRcvBufLen=0;                //接收的数据长度  可多次接收一个数据包

	memset(m_cCreateDate,0,sizeof(m_cCreateDate));//连接建立日期
	m_nConntime = 0;
	memset(m_cCloseDate,0,sizeof(m_cCloseDate));//连接关闭日期
	m_nCloseTime = 0;

	m_sendNum = 10;

}

int CSocketInfo::OnRecv()
{
	int ret;
	m_tcpSocket.AttachSocket(m_sock,"");
	if (m_nRcvBufLen >=0 && m_nRcvBufLen< BFBUFSIZE)
	{
		//包头未接收完
		ret = m_tcpSocket.Receive(m_sRcvBuffer+m_nRcvBufLen,BFBUFSIZE-m_nRcvBufLen);
		if (ret <= 0)
		{
// 			if (m_tcpSocket.GetErrorNo() == EWOULDBLOCK)
// 			{
// 				return 0;
// 			}
			//OnClose("远端关闭，接收数据不完整");
			return ret;
		}
		m_nRcvBufLen+=ret;
		m_nReadtime =time(NULL);

	}
	return m_nRcvBufLen;
}

int CSocketInfo::OnSend()
{
	int length;
	char stmp[511];
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

		//比较连接时间戳
		if (m_nType == SOCK_CLI)
		{
			if (prq->msgbuf.head.connecttime != m_nConntime)
			{
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"连接时间戳不符，删除 %s index=%d sock=%d type=%d",m_sAddress,m_index,m_sock,m_nType);
				m_log->LogBin(LOG_ERROR,__FILE__,__LINE__,"连接时间戳不符，删除",prq->msgbuf.message->buffer,prq->msgbuf.head.len);
				m_pMemPool->PoolFree(prq->msgbuf.message);
				m_pSendQueue.DeleteSend(prq);					
				//记录日志
				return -1;
			}
		}
		
		
		if (prq->msgbuf.message == NULL)
		{
			m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"数据为NULL");
			continue;
		}
		if (time(NULL) - prq->msgbuf.head.rtime> m_nQueueTime)
		{
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"请求在队列超时，删除 %s index=%d sock=%d type=%d",m_sAddress,m_index,m_sock,m_nType);
			m_log->LogBin(LOG_ERROR,__FILE__,__LINE__,"请求在队列超时，删除",prq->msgbuf.message->buffer,prq->msgbuf.head.len);
			m_pMemPool->PoolFree(prq->msgbuf.message);
			m_pSendQueue.DeleteSend(prq);	
			continue;
		}
		m_tcpSocket.AttachSocket(m_sock,"");
		
		if (m_nType == SOCK_BU)
		{
			
			m_log->LogBin(LOG_DEBUG+2,__FILE__,__LINE__,"发送给BU的数据",prq->msgbuf.message->buffer+prq->nSendlen,prq->nWantlen-prq->nSendlen);
		}
		else if (m_nType == SOCK_CLI)
		{
			m_log->LogBin(LOG_DEBUG+2,__FILE__,__LINE__,"发送给客户端的数据",prq->msgbuf.message->buffer+prq->nSendlen,prq->nWantlen-prq->nSendlen);
		}
		ret = m_tcpSocket.Send(prq->msgbuf.message->buffer+prq->nSendlen,prq->nWantlen-prq->nSendlen);
		if (ret == prq->nWantlen-prq->nSendlen)
		{
			m_pMemPool->PoolFree(prq->msgbuf.message);
			// 处理成功,停止
			m_pSendQueue.DeleteSend(prq);	
			m_nSendtime = time(NULL);
		}
		else //停止发送
		{
			if (ret >0)
			{
				prq->nSendlen+=ret;
			}
			return ret;
		}
	}
	return sendNum;
}

int CSocketInfo::PutSendMsg(S_BF_RSMSG_FPC msg)
{
	int ret;
	ret = m_pSendQueue.PutSendMsg(msg);
	if (ret!=0)
	{
		return ret;
	}
	return ret;
}



int CSocketInfo::GetRecvData(S_BF_RSMSG_FPC *msg)
{
	int txlen=0;
	char tmpchar[100];
	memset(tmpchar,0,sizeof(tmpchar));
	
	if (m_nType == SOCK_CLI) //前置收到的数据
	{
		//收到的数据大于包头
		if (m_nRcvBufLen >= m_gRes->g_nHeadNum)
		{
			if (m_gRes->g_nHeadNum>0)
			{
				memcpy(tmpchar,m_sRcvBuffer,m_gRes->g_nHeadNum);
				tmpchar[m_gRes->g_nHeadNum]=0;
				m_pTool.LRtrim(tmpchar);
				if (!m_pTool.IsNumber(tmpchar))
				{
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"接收的通讯报文头非数字");
					OnClose("通讯报文头不符");
					return -1;
				}
				txlen = atoi(tmpchar);
				if (txlen<0)
				{
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"通讯报文头不符 %d",txlen);
					OnClose("通讯报文头不符");
					return 0;
				}
				if (txlen == 0)
				{
					for (int i=0; i<m_nRcvBufLen-txlen-m_gRes->g_nHeadNum; i++)
					{
						m_sRcvBuffer[i]=m_sRcvBuffer[i+txlen+m_gRes->g_nHeadNum];
					}
					m_nRcvBufLen-=m_gRes->g_nHeadNum+txlen;
					return 0;
				}
				txlen = txlen + m_gRes->GetHeadLenByIp(m_sAddress);
				m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"客户端IP%s 数据长度%d   已接收长度%d",m_sAddress,txlen,m_nRcvBufLen);
				//printf("GetRecvData headlen=%d rcvlen=%d\n",txlen,m_nRcvBufLen);
				if (m_nRcvBufLen < txlen+m_gRes->g_nHeadNum)
				{
					//说明数据没有接收完整
					return 0;
				}
				if (txlen >= BFBUFSIZE)
				{
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"长度超过缓冲区长度txlen=%d",txlen);
					OnClose("通讯报文头不符");
					return 0;
				}
			}
			
			//记录交易信息
//			m_log->LogSysBin(LOG_DEBUG+1,m_sRcvBuffer,txlen+m_gRes->g_nHeadNum);
			
			msg->msgbuf.head.len = txlen;
			if (msg->msgbuf.message == NULL)
			{
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"msg->msgbuf.message为NULL");
				return -1;
			}
			memcpy(msg->msgbuf.message,m_sRcvBuffer+m_gRes->g_nHeadNum,txlen);
			msg->msgbuf.message->buffer[msg->msgbuf.head.len]=0;
			//记入接收日志
			if (msg->msgbuf.head.len>0)
			{
				m_log->LogBin(LOG_DEBUG,__FILE__,__LINE__,"收到的交易数据",msg->msgbuf.message->buffer,msg->msgbuf.head.len);
				m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"请求头信息 index=%d conntime=%d msgtype=%d",msg->msgbuf.head.index,\
					msg->msgbuf.head.connecttime,msg->msgbuf.head.msgtype);
			}
			else
			{
				m_log->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"请求头信息 index=%d conntime=%d msgtype=%d",msg->msgbuf.head.index,\
					msg->msgbuf.head.connecttime,msg->msgbuf.head.msgtype);
			}
			m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"m_nRcvBufLen=%d txlen=%d  m_gRes->g_nHeadNum=%d ",m_nRcvBufLen,txlen,m_gRes->g_nHeadNum);
			if (m_nRcvBufLen < txlen+m_gRes->g_nHeadNum)
			{
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"数据未收全 %d<%d",m_nRcvBufLen,txlen+m_gRes->g_nHeadNum);
				return 0;
			}
			//收多了，将原包重新给m_sRcvBuffer
			for (int i=0; i<m_nRcvBufLen-txlen-m_gRes->g_nHeadNum; i++)
			{
				m_sRcvBuffer[i]=m_sRcvBuffer[i+txlen+m_gRes->g_nHeadNum];
			}
			m_nRcvBufLen-=m_gRes->g_nHeadNum+txlen;
			return m_gRes->g_nHeadNum+txlen;
		}
	}
	else if (m_nType == SOCK_BU) //收BU数据
	{
		//收到的数据大于包头
		if (m_nRcvBufLen >= sizeof(S_MSG_HEAD_FPC))
		{
			memcpy(&(msg->msgbuf.head),m_sRcvBuffer,sizeof(S_MSG_HEAD_FPC));
			Reverse((unsigned char *)&(msg->msgbuf.head.msgtype),sizeof(msg->msgbuf.head.msgtype));
			Reverse((unsigned char *)&(msg->msgbuf.head.connecttime),sizeof(msg->msgbuf.head.connecttime));
			Reverse((unsigned char *)&(msg->msgbuf.head.index),sizeof(msg->msgbuf.head.index));
			Reverse((unsigned char *)&(msg->msgbuf.head.len),sizeof(msg->msgbuf.head.len));
			if (m_nRcvBufLen < sizeof(S_MSG_HEAD_FPC)+msg->msgbuf.head.len)
			{
				if (m_nRcvBufLen >= BFBUFSIZE || msg->msgbuf.head.len>=BFBUFSIZE )
				{
					OnClose("连接非BU");
					return -1;
				}
				//说明数据没有接收完整
				return 0;
			}
			//记录交易信息
//			m_log->LogSysBin(LOG_DEBUG+1,m_sRcvBuffer,msg->msgbuf.head.len+sizeof(S_MSG_HEAD));
			if (msg->msgbuf.message == NULL)
			{
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"msg->msgbuf.message为NULL");
				return -1;
			}
			memcpy(msg->msgbuf.message,m_sRcvBuffer+sizeof(S_MSG_HEAD_FPC),msg->msgbuf.head.len);
			msg->msgbuf.message->buffer[msg->msgbuf.head.len]=0;
			//记入接收日志
			if (msg->msgbuf.head.len>0)
			{
				m_log->LogBin(LOG_DEBUG+2,__FILE__,__LINE__,"收到的BU数据",msg->msgbuf.message->buffer,msg->msgbuf.head.len);
				m_log->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"BU头信息 index=%d conntime=%d",msg->msgbuf.head.index,\
					msg->msgbuf.head.connecttime);
			}
			else
			{
				m_log->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"BU头信息 index=%d conntime=%d msgtype=%d",msg->msgbuf.head.index,\
					msg->msgbuf.head.connecttime,msg->msgbuf.head.msgtype);
			}
			//收多了，将原包重新给m_sRcvBuffer
			for (int i=0; i<m_nRcvBufLen-sizeof(S_MSG_HEAD_FPC)-msg->msgbuf.head.len; i++)
			{
				m_sRcvBuffer[i]=m_sRcvBuffer[i+sizeof(S_MSG_HEAD_FPC)+msg->msgbuf.head.len];
			}
			m_nRcvBufLen-=sizeof(S_MSG_HEAD_FPC)+msg->msgbuf.head.len;
			return sizeof(S_MSG_HEAD_FPC)+msg->msgbuf.head.len;
		}
	}
	return txlen;
}

void CSocketInfo::SetClient()
{
	m_nType = SOCK_CLI;
	m_bNeedread = true;
	m_nSendtime = time(NULL);
	m_nReadtime = time(NULL);
}
void CSocketInfo::SetBuClient()
{
	m_nType = SOCK_BU;
	m_bNeedread = true;
	m_bIsBuGetFlag = false;
	m_nSendtime = time(NULL);
	m_nReadtime = time(NULL);
}
void CSocketInfo::SetAccept(SOCKET_HANDLE sock,const char *address)
{
	ResetInfo();
	m_nType = SOCK_UNKNOW;
	m_sock = sock;
	m_bNeedread = false;
//	m_nConntime = time(NULL);
	m_nConntime = m_publog.GetTickCount();
	strcpy(m_sAddress,address);
	m_publog.Update();
	m_publog.ToCharDate(m_cCreateDate);
	m_tcpSocket.AttachSocket(m_sock,address);
	m_tcpSocket.SetBlockFlag(false);

}
bool CSocketInfo::NeedSend()
{
	return m_pSendQueue.NeedSend();
}


void CSocketInfo::Reverse(unsigned char *buf, int len)
{
	int i;
	unsigned char buf1[100];
	if (htonl(1) == 1)
	{
		for(i=0;i<len;i++)
		{
			buf1[i] = buf[len-i-1];
		}
		memcpy(buf,buf1,len);
	}
}

int CSocketInfo::SendMsg(S_BF_RSMSG_FPC msg)
{
	int ret;
	CBF_PMutex pp(&m_mutex);
	m_tcpSocket.AttachSocket(m_sock,"");
	if (msg.msgbuf.message == NULL)
	{
		m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"数据为NULL");
		return -1;
	}
	if (m_nType == SOCK_CLI) //前置收到的数据
	{
		if (msg.msgbuf.head.connecttime != m_nConntime)
		{
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"连接时间戳不符，删除 %s index=%d sock=%d type=%d",m_sAddress,m_index,m_sock,m_nType);
			m_log->LogBin(LOG_ERROR,__FILE__,__LINE__,"连接时间戳不符，删除",msg.msgbuf.message->buffer,msg.msgbuf.head.len);
			m_pMemPool->PoolFree(msg.msgbuf.message);
			return -1;
		}
		char stmp[511];
		//准备数据
		if (m_gRes->g_nHeadNum>0)
		{
			int tmplen = m_gRes->GetHeadLenByIp(m_sAddress);
			m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"客户端IP长度%d",tmplen);
			sprintf(stmp,"%s0%d%s","%",m_gRes->g_nHeadNum,"d");
			sprintf(m_sSendBuffer,stmp,msg.msgbuf.head.len-tmplen);
			if (msg.msgbuf.head.len>0)
			{
				memcpy(m_sSendBuffer+m_gRes->g_nHeadNum,msg.msgbuf.message->buffer,msg.msgbuf.head.len);
			}
			msg.nWantlen = m_gRes->g_nHeadNum + msg.msgbuf.head.len;
		}
		else
		{
			msg.nWantlen = msg.msgbuf.head.len;
			if (msg.nWantlen>0)
			{
				memcpy(m_sSendBuffer,msg.msgbuf.message->buffer,msg.msgbuf.head.len);
			}
			else
			{
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送数据长度为0  %d",msg.msgbuf.head.len);
				m_pMemPool->PoolFree(msg.msgbuf.message);
				return -1;
			}
		}
		if (NeedSend()) //队列中已有数据
		{
			m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"已有数据待发，直接入队列等待发送");
			memcpy(msg.msgbuf.message->buffer,m_sSendBuffer,msg.nWantlen);
			msg.nSendlen =0;
			PutSendMsg(msg);
			return 1;
		}

		ret = m_tcpSocket.Send(m_sSendBuffer,msg.nWantlen);
		m_log->LogBin(LOG_DEBUG,__FILE__,__LINE__,"应答数据",m_sSendBuffer,msg.nWantlen);	

		if (ret == msg.nWantlen)
		{
			m_pMemPool->PoolFree(msg.msgbuf.message);
			// 处理成功,停止
			m_nSendtime = time(NULL);
			return 0;
		}
		else //停止
		{
			//放入队列，下次再发
			char stmp[511];
			sprintf(stmp,"ret:%d %d %s",ret,GET_LAST_SOCK_ERROR(),m_tcpSocket.GetErrorMsg().c_str());
			m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"发送数据给客户端失败 放入队列再发 %s index=%d sock=%d errmsg=%s",m_sAddress,m_index,m_sock,stmp);
			if (ret >0)
			{
				msg.nSendlen = ret;
			}
			else
			{
				msg.nSendlen =0;
			}
			memcpy(msg.msgbuf.message->buffer,m_sSendBuffer,msg.nWantlen);
			PutSendMsg(msg);
			return 1;
	
		}
	}
	else if (m_nType == SOCK_BU)
	{
		msg.nWantlen = msg.msgbuf.head.len+sizeof(S_MSG_HEAD_FPC);
		memcpy(m_sSendBuffer,&(msg.msgbuf.head),sizeof(S_MSG_HEAD_FPC));
		if (msg.msgbuf.head.len>0)
		{
			memcpy(m_sSendBuffer+sizeof(S_MSG_HEAD_FPC),msg.msgbuf.message->buffer,msg.msgbuf.head.len);
		}
		S_MSG_HEAD_FPC *tmphead = (S_MSG_HEAD_FPC *)m_sSendBuffer;
		Reverse((unsigned char *)&(tmphead->msgtype),sizeof(tmphead->msgtype));
		Reverse((unsigned char *)&(tmphead->connecttime),sizeof(tmphead->connecttime));
		Reverse((unsigned char *)&(tmphead->index),sizeof(tmphead->index));
		Reverse((unsigned char *)&(tmphead->len),sizeof(tmphead->len));
		Reverse((unsigned char *)&(tmphead->buindex),sizeof(tmphead->buindex));
		Reverse((unsigned char *)&(tmphead->rtime),sizeof(tmphead->rtime));
		m_log->LogBin(LOG_DEBUG+2,__FILE__,__LINE__,"发送给BU的数据",msg.msgbuf.message->buffer,msg.nWantlen);
		if (NeedSend()) //队列中已有数据
		{
			m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"已有数据待发，直接入队列等待发送");
			memcpy(msg.msgbuf.message->buffer,m_sSendBuffer,msg.nWantlen);
			msg.nSendlen =0;
			PutSendMsg(msg);
			return 1;
		}
		ret = m_tcpSocket.Send(m_sSendBuffer,msg.nWantlen);
		if (ret == msg.nWantlen)
		{
			m_pMemPool->PoolFree(msg.msgbuf.message);
			// 处理成功,停止
			m_nSendtime = time(NULL);
			return 0;
		}
		else //停止
		{
			//放入队列，下次再发
			char stmp[511];
			sprintf(stmp,"ret:%d %d %s",ret,GET_LAST_SOCK_ERROR(),m_tcpSocket.GetErrorMsg().c_str());
			m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"发送数据给BU失败 放入队列再发 %s index=%d sock=%d errmsg=%s",m_sAddress,m_index,m_sock,stmp);
			if (ret >0)
			{
				msg.nSendlen = ret;
			}
			else
			{
				msg.nSendlen =0;
			}
			memcpy(msg.msgbuf.message->buffer,m_sSendBuffer,msg.nWantlen);
			PutSendMsg(msg);
			return 1;
		}
	}
	else
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"连接类型不符，即不是CLI也不是BU");
		return -1;
	}
	return 0;
}
