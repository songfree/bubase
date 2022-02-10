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
	ResetInfo();
	m_pMemPool = NULL;
	m_nType = SOCK_UNKNOW;
	m_nCloseTime = 0;
	m_nQueueTime = 60;
	m_sendNum = 10;
	m_nPid = -1;
	m_log = NULL;
	m_nStatus = SOCK_STATUS_UNLOGIN;
	
}

CSocketInfo::~CSocketInfo()
{
	if (m_sock != INVALID_SOCKET)
	{
		OnShutdown();
	}

	TRADE_RDQUEUE::iterator prq,prn;
	for (int i=0; i< PRIORITIES ; i++)
	{
		for (prn=m_pSendQueue.m_qSendData[i].datas.begin();prn!=m_pSendQueue.m_qSendData[i].datas.end();)
		{
			
			prq = prn;
			prn++;
			m_pMemPool->PoolFree(prq->pData);
			// 处理成功,停止
			m_pSendQueue.DeleteSend(prq,i);	
			
		}
	}

}

void CSocketInfo::Init(CTradeResource *res,CBF_BufferPool *pool)
{
	ResetInfo();
	m_pMemPool = pool;
	m_gRes = res;
	m_log = m_gRes->m_log;
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
#ifdef _ENGLISH_
	m_log->LogMp(LOG_PROMPT,__FILE__,__LINE__,"%s,close socket %s:%d index[%d]",msg,m_sAddress,m_sock,m_index);
#else
	m_log->LogMp(LOG_PROMPT,__FILE__,__LINE__,"%s,关闭连接%s:%d index[%d]",msg,m_sAddress,m_sock,m_index);
#endif

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
	TRADE_RDQUEUE::iterator prn,prq;
	for (int i=0; i< PRIORITIES ; i++)
	{
		for (prn=m_pSendQueue.m_qSendData[i].datas.begin(); prn !=  m_pSendQueue.m_qSendData[i].datas.end();)
		{
			prq = prn;
			prn++;
#ifdef _ENGLISH_
			m_log->LogBin(LOG_ERROR,__FILE__,__LINE__,"clear queue data",prq->pData->sBuffer,prq->nWantlen);
#else
			m_log->LogBin(LOG_ERROR,__FILE__,__LINE__,"清空队列时待发送的数据",prq->pData->sBuffer,prq->nWantlen);
#endif
			m_pMemPool->PoolFree(prq->pData);
			m_pSendQueue.DeleteSend(prq,i);
		}
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
#ifdef _ENGLISH_
		sprintf(m_errMsg,"Listen [%d] error ,queue size:%d",m_nPort,queuenum);
#else
		sprintf(m_errMsg,"侦听端口[%d]失败,侦听队列大小%d",m_nPort,queuenum);
#endif
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

bool CSocketInfo::ListenIpc(int port,bool blockflag,int queuenum)
{
	if (!m_tcpSocket.Create(AF_UNIX,blockflag))
	{
		strcpy(m_errMsg,m_tcpSocket.GetErrorMsg().c_str());
		return false;
	}
	m_nPort = port;
	if (!m_tcpSocket.Listen(m_nPort,queuenum))
	{
#ifdef _ENGLISH_
		sprintf(m_errMsg,"Listen [%d] error ,queue size:%d",m_nPort,queuenum);
#else
		sprintf(m_errMsg,"侦听端口[%d]失败,侦听队列大小%d",m_nPort,queuenum);
#endif
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
//	m_bIsBuGetFlag = false;
	m_nStatus = SOCK_STATUS_UNLOGIN;
	
	m_nReadtime= 0;              //上次读数据时间
	m_nConntime=0;              //连接建立时间
	m_nSendtime=0;              //上次发送数据时间 
	m_nPingTime = 0;
	
	//发送接收数据	
	
	memset(m_sRcvBuffer,0,sizeof(BPCBUFSIZE));            //接收数据缓冲
	memset(m_sSendBuffer,0,sizeof(BPCBUFSIZE));            //发送数据缓冲
	m_nRcvBufLen=0;                //接收的数据长度  可多次接收一个数据包
	
	m_nConntime = 0;
	m_nCloseTime = 0;
	m_sendNum = 10;

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

	}
	return m_nRcvBufLen;
}

int CSocketInfo::OnSend()
{
	
	
	int ret;
	if (m_sock == INVALID_SOCKET)
	{
		m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"connect closed");
		return -2;
	}
	if (!m_pSendQueue.NeedSend())
	{
		return -3;
	}
	CBF_PMutex pp(&m_mutex);
	TRADE_RDQUEUE::iterator prq,prn;
	
	int sendNum=0;
	for (int i=0; i< PRIORITIES ; i++)
	{

		for (prn=m_pSendQueue.m_qSendData[i].datas.begin();prn!=m_pSendQueue.m_qSendData[i].datas.end();)
		{
			sendNum++;
			if (m_sendNum >0 && sendNum > m_sendNum) //如果本次已发送数超过定义，重新开始
			{
				return sendNum;
			}
			prq = prn;
			prn++;
			
			if (prq->pData == NULL)
			{
				m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"数据为NULL");
				continue;
			}
			m_tcpSocket.AttachSocket(m_sock,"");
#ifdef _ENGLISH_
			m_log->LogBin(LOG_DEBUG+2,__FILE__,__LINE__,"send far-end data",prq->pData->sBuffer+prq->nSendlen,prq->nWantlen-prq->nSendlen);
#else			
			m_log->LogBin(LOG_DEBUG+2,__FILE__,__LINE__,"发送给客户端的数据",prq->pData->sBuffer+prq->nSendlen,prq->nWantlen-prq->nSendlen);
#endif			

			ret = m_tcpSocket.Send((char *)&(prq->pData->sDBHead)+(prq->nWantlen-prq->nSendlen),prq->nSendlen);
			if (ret == prq->nSendlen)
			
			{
				m_pMemPool->PoolFree(prq->pData);
				// 处理成功,停止
				m_pSendQueue.DeleteSend(prq,i);	
				m_nSendtime = time(NULL);
			}
			else //停止发送
			{
				if (ret >0)
				{
					prq->nSendlen= prq->nSendlen -ret;
				}
				return ret;
			}
		}
		
	}
	if (sendNum>0)
	{
#ifdef _ENGLISH_
		m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"index[%d] ip[%s] can write，send data qty[%d]",m_index,m_sAddress,sendNum);
#else
		m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"连接index[%d] ip[%s]可写，发送数据数[%d]",m_index,m_sAddress,sendNum);
#endif
	}
	
	return sendNum;
}

int CSocketInfo::PutSendMsg(S_TRADE_DATA msg,int prio)
{
	return m_pSendQueue.PutSendMsg(msg,prio);
}



int CSocketInfo::GetRecvData(S_TRADE_DATA *msg)
{
	int txlen=0;

	//收到的数据大于包头
	if (m_nRcvBufLen >= DREBHEADLEN )
	{
		//进行字节序处理并校验CRC
		memcpy(&(msg->pData->sDBHead),m_sRcvBuffer,DREBHEADLEN);
		
		m_pDrebEndian.Endian2LocalHost(&(msg->pData->sDBHead)) ;
		
		
		if (m_nRcvBufLen < msg->pData->sDBHead.nLen + DREBHEADLEN)
		{
			//说明数据没有接收完整
			return 0;
		}
		if (msg->pData->sDBHead.nLen >=0 && msg->pData->sDBHead.nLen <= DREBDATASIZE )
		{
			memcpy(msg->pData->sBuffer,m_sRcvBuffer+DREBHEADLEN,msg->pData->sDBHead.nLen);
			msg->pData->sBuffer[msg->pData->sDBHead.nLen]=0;
		}
		else
		{
#ifdef _ENGLISH_
			OnClose("error,illegal connect");
#else
			OnClose("错误 连接非法");
#endif
			return -1;
		}
		
		msg->pData->sBpcHead.nBpcLen = DREBHEADLEN+msg->pData->sDBHead.nLen;
		msg->pData->sBpcHead.nBpuIndex = 1;
		msg->pData->sBpcHead.nConnectTime = m_nConntime;
		msg->pData->sDBHead.s_Sinfo.s_nTimestamp = m_nConntime;
		msg->pData->sBpcHead.nIndex = m_index; 
		msg->pData->sBpcHead.cMsgType = MSG_REQ;
		msg->pData->sBpcHead.nRtime = time(NULL);
		msg->index = m_index;
		msg->src = 1;

		
		//收多了，将原包重新给m_sRcvBuffer
		for (int i=0; i<m_nRcvBufLen-DREBHEADLEN-msg->pData->sDBHead.nLen; i++)
		{
			m_sRcvBuffer[i]=m_sRcvBuffer[i+DREBHEADLEN+msg->pData->sDBHead.nLen];
		}
		m_nRcvBufLen=m_nRcvBufLen - DREBHEADLEN-msg->pData->sDBHead.nLen;
		
		return DREBHEADLEN+msg->pData->sDBHead.nLen;
	}
	else if (m_nRcvBufLen>0)
	{
#ifdef _ENGLISH_
		m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"warn, index[%d] recv part data:%d",\
			m_index,m_nRcvBufLen);
#else
		m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"警告 index[%d] 收到数据%d,但不完整",\
			m_index,m_nRcvBufLen);
#endif
	}
	return 0;
}

void CSocketInfo::SetClient()
{
	m_nType = SOCK_CLI;
	m_bNeedread = true;
	m_nSendtime = time(NULL);
	m_nReadtime = time(NULL);
}
void CSocketInfo::SetAccept(SOCKET_HANDLE sock,const char *address)
{
	ResetInfo();
	m_nType = SOCK_UNKNOW;
	m_sock = sock;
	m_bNeedread = false;
	m_nConntime = m_publog.GetTickCount();
	strcpy(m_sAddress,address);
	m_publog.Update();
	m_publog.ToCharDate(m_cCreateDate);
}
bool CSocketInfo::NeedSend()
{
	if (m_sock == INVALID_SOCKET)
	{
		return false;
	}
	return m_pSendQueue.NeedSend();
}


void CSocketInfo::Reverse(unsigned char *buf, int len)
{
	int i;
	unsigned char buf1[10];
	if (htonl(1) == 1)
	{
		for(i=0;i<len;i++)
		{
			buf1[i] = buf[len-i-1];
		}
		memcpy(buf,buf1,len);
	}
}

int CSocketInfo::SendMsg(S_TRADE_DATA msg,bool sendimmediate)
{
	int ret=0;
	CBF_PMutex pp(&m_mutex);
	if (m_sock == INVALID_SOCKET)
	{
		m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"connect index[%d] closed",m_index);
		return -1;
	}
	m_tcpSocket.AttachSocket(m_sock,"");
	if (msg.pData == NULL)
	{
		m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"data is NULL");
		return -1;
	}
	//判断连接时间戳
	if (msg.pData->sBpcHead.nConnectTime != m_nConntime)
	{
		m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"连接时间戳不符");
		return -1;
	}
	
	msg.nWantlen = msg.pData->sDBHead.nLen+DREBHEADLEN;
	
	m_pDrebEndian.Endian2Comm(&(msg.pData->sDBHead));

	//发送队列若有数据，则放入发送队列，按顺序发送
	if (sendimmediate)
	{
		if (m_sock == INVALID_SOCKET)
		{
#ifdef _ENGLISH_
			m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"connect closed，push DREB queue datalen[%d] ",msg.nWantlen);
#else
			m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"连接已断开，放入DREB队列 数据长度[%d] ",msg.nWantlen);
#endif
			PutSendMsg(msg,0);
			return 1;
		}
		ret = m_tcpSocket.Send((char *)&(msg.pData->sDBHead),msg.nWantlen);
		if (ret == msg.nWantlen)
		{
			m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"send complete, index[%d]",m_index);
			//统计数据
			m_pMemPool->PoolFree(msg.pData);
			msg.pData = NULL;
			m_nSendtime = time(NULL);
			return 0;
			
		}
		else //停止
		{
			if (ret >0)
			{
				msg.nSendlen = msg.nWantlen - ret;
			}
#ifdef _ENGLISH_
			m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"warn, send error,push queue, retcode[%d] msg[%s] len[%d] datalen[%d] ",\
				m_index,ret,m_tcpSocket.GetErrorMsg().c_str(),msg.nSendlen,msg.nWantlen);
#else
			m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"警告 发送消息出错,放入队列 ret返回[%d] msg[%s] 放入长度[%d] 数据长度[%d] ",\
				m_index,ret,m_tcpSocket.GetErrorMsg().c_str(),msg.nSendlen,msg.nWantlen);
#endif
			//只要发送出错就放到队列，如果队列里有数据可能就会有问题
			if (!m_pSendQueue.NeedSend())
			{
				PutSendMsg(msg,0);
				return 1;
			}
			else
			{
#ifdef _ENGLISH_
				OnClose("queue has data,send error");
#else
				OnClose("直接在连接发送数据不成功，但连接队列里仍有数据");
#endif
				return -1;
			}
			
		}
	}
	else
	{
		if (NeedSend()) //连接正常
		{
#ifdef _ENGLISH_
			m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"queue has data,pushback, index[%d] datalen[%d]  ",\
				m_index,msg.nWantlen);
#else
			m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"发送队列已有数据，放入队列 index[%d] 数据长度[%d]  ",\
				m_index,msg.nWantlen);
#endif
			PutSendMsg(msg,0);
			return 1;
		}
		else  //直接发送
		{
			if (m_sock == INVALID_SOCKET )
			{
#ifdef _ENGLISH_
				m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"connect closed, index[%d] datalen[%d] ",\
					m_index,msg.nWantlen);
#else
				m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"连接已断开，index[%d] 数据长度[%d] ",\
					m_index,msg.nWantlen);
#endif
				return -1;
			}
			ret = m_tcpSocket.Send((char *)&(msg.pData->sDBHead),msg.nWantlen);
			if (ret == msg.nWantlen)
			{
				m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"send complete, index[%d] ",\
					m_index);
				m_pMemPool->PoolFree(msg.pData);
				msg.pData = NULL;
				m_nSendtime = time(NULL);
				return 0;
				
			}
			else //停止
			{
				if (ret >0)
				{
					msg.nSendlen = msg.nWantlen - ret;
				}
#ifdef _ENGLISH_
				m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"send  DREB[%d] error, retcode[%d] msg[%s] len[%d] datalen[%d] ",\
					m_index,ret,m_tcpSocket.GetErrorMsg().c_str(),msg.nSendlen,msg.nWantlen);
#else
				m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"警告 发送给DREB[%d]的消息出错,放入队列 ret返回[%d] msg[%s] 放入长度[%d] 数据长度[%d] ",\
					m_index,ret,m_tcpSocket.GetErrorMsg().c_str(),msg.nSendlen,msg.nWantlen);
#endif
				//只要发送出错就放入队列
				PutSendMsg(msg,0);
				return 2;
			}
		}
	}
	return ret;
}

bool CSocketInfo::ResetData(S_TRADE_DATA &data)
{
	
	data.index = m_index;
	data.rTime = time(NULL);
	data.nSendlen =0;
	data.nWantlen = 0;
	data.src=1;
	data.pData = (PBPCCOMMSTRU)m_pMemPool->PoolMalloc();
	if (data.pData == NULL)
	{
		m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"PoolMalloc error");
		return false;
	}
	return true;
}

void CSocketInfo::PoolFree(void *p)
{
	m_pMemPool->PoolFree(p);
}
