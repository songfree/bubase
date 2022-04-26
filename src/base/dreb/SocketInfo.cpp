// SocketInfo.cpp: implementation of the CSocketInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "SocketInfo.h"
#include "BF_Zip.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CSocketInfo::CSocketInfo()
{
	memset(m_sAddress,0,sizeof(m_sAddress));         //请求连接上来的IP
	memset(m_sSendBuffer,0,sizeof(m_sSendBuffer)); 
	
	ResetInfo();
	m_pMemPool = NULL;
	m_pStatInfo = NULL;
	m_nCloseTime = 0;
	m_nQueueTime = 60;
	m_pSendQueue = NULL;
	m_sendNum = 10;
	m_pRes = NULL;
}

CSocketInfo::~CSocketInfo()
{
	if (m_sock != INVALID_SOCKET)
	{
		closesocket(m_sock);
	}
	m_pMemPool = NULL;
	m_pRes = NULL;
	m_pStatInfo = NULL;
}

void CSocketInfo::SetGlobalVar(CGlobalVar *pVar,CBF_BufferPool *pool,int deltime)
{
	m_pRes = pVar;
	m_pMemPool = pool;
	m_pStatInfo = &pVar->g_pDrebStatInfo;
	m_nLogLevel = m_pRes->g_nLogLevel;
	m_sLogFile = m_pRes->g_sLogFileName;
	m_nQueueTime  = deltime;
	m_log = &(m_pRes->g_pLog);
	

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

void CSocketInfo::OnClose(const char *msg,const char *filename,int fileline)
{
	CBF_PMutex pp(&m_mutex);
	m_datetime.Update();
	m_datetime.ToCharDate(m_cCloseDate);
	if (m_nType == SOCK_CLI || m_nType == SOCK_UNKNOW)
	{
		//清空队列
		m_log->LogMp(LOG_ERROR_GENERAL,filename,fileline,"%s,关闭连接[%s:%d] index[%d] type[%s]",msg,m_sAddress,m_sock,m_index,GetSockType(m_nType).c_str());
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
		ClearQueue();
		ResetInfo();
	}
	else 
	{
		m_log->LogMp(LOG_ERROR_GENERAL,filename,fileline,"%s,关闭连接[%s:%d] index[%d] type[%s]",msg,m_sAddress,m_sock,m_index,GetSockType(m_nType).c_str());
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
		//检查队列中第一个数据包是否发送部分，若是则删除，重连后发送完整数据包
		CheckFirstData();

		//20170205 songfree add 还是把队列里的数据清掉吧，以后可以考虑把数据放回分配队列
		ClearQueue();
		//20170205 songfree add end 

		ResetSInfo();
		if (m_nType == SOCK_TODREB) //是主动建立连接
		{
			m_s_center.bNeedConnect = true; //需要连接
		}
		m_pSendQueue = NULL;

	}
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
	if (m_nType == SOCK_TODREB ) //和其它通讯平台的连接
	{
		return false;
	}
	return true;
}

void CSocketInfo::ClearQueue()
{
	if (m_pSendQueue== NULL)
	{
		return;
	}
	RDQUEUE::iterator prn;

	while (m_pSendQueue->m_qSendData.datas.size()>0)	
	{
		prn=m_pSendQueue->m_qSendData.datas.begin();
		if (prn == m_pSendQueue->m_qSendData.datas.end())
		{
			break;
		}
		
		if ((*prn)->message.head.cCmd != CMD_PING && (*prn)->message.head.cCmd != CMD_CONNECT && (*prn)->message.head.cCmd != CMD_REGSERVICE)
		{
			m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"清空队列 index[%d] type[%s]的消息 DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 业务数据长度[%d]",\
				m_index,GetSockType(m_nType).c_str(),\
				GetDrebCmdType((*prn)->message.head.cCmd).c_str(),(*prn)->message.head.cNextFlag,\
				(*prn)->message.head.cRaflag,(*prn)->message.head.d_Dinfo.d_nServiceNo,(*prn)->message.head.s_Sinfo.s_nNodeId,\
				(*prn)->message.head.s_Sinfo.s_cNodePrivateId,(*prn)->message.head.s_Sinfo.s_nDrebSerial,(*prn)->message.head.nLen);
		}
		else
		{
			m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"清空队列 index[%d] type[%s]的消息 DREB命令[%s]  业务数据长度[%d]",\
				m_index,GetSockType(m_nType).c_str(),GetDrebCmdType((*prn)->message.head.cCmd).c_str(),(*prn)->message.head.nLen);
		}
		m_pRes->g_pDataLog.LogDreb(LOG_ERROR_FAULT,&((*prn)->message),true);
		m_pMemPool->PoolFree((*prn));
		m_pSendQueue->DeleteSend(prn);
	}
	
}

bool CSocketInfo::Listen(int port,bool blockflag,int queuenum)
{
//	if (!m_tcpSocket.Create(AF_UNIX,blockflag))
	if (!m_tcpSocket.Create(AF_INET,blockflag))
	{
		CBF_Tools::StringCopy(m_errMsg,255,m_tcpSocket.GetErrorMsg().c_str());
		return false;
	}
	m_s_listen.nPort = port;
	if (!m_tcpSocket.Listen(m_s_listen.nPort,queuenum))
	{
		sprintf(m_errMsg,"侦听端口[%d]失败,侦听队列大小%d",port,queuenum);
		return false;
	}
	
	m_nType   =  SOCK_LISTEN;      //侦听
	m_sock = m_tcpSocket.GetSocket();

	m_bChecked = true;
	m_bNeedread = true;

	return true;
}

bool CSocketInfo::ListenIpc(int port,bool blockflag,int queuenum)
{
	if (!m_tcpSocket.Create(AF_UNIX,blockflag))
	{
		CBF_Tools::StringCopy(m_errMsg,255,m_tcpSocket.GetErrorMsg().c_str());
		return false;
	}
	m_s_listen.nPort = port;
	if (!m_tcpSocket.Listen(m_s_listen.nPort,queuenum))
	{
		sprintf(m_errMsg,"侦听端口[%d]失败,侦听队列大小%d",port,queuenum);
		return false;
	}
	
	m_nType   =  SOCK_LISTEN;      //侦听
	m_sock = m_tcpSocket.GetSocket();
	
	m_bChecked = true;
	m_bNeedread = true;
	
	return true;
}

void CSocketInfo::ResetInfo()
{
	memset(m_sAddress,0,sizeof(m_sAddress));
	memset(&m_s_center,0,sizeof(DREB_CENTER));
	memset(&m_s_listen,0,sizeof(DREB_LISTEN));
	memset(&m_s_conn,0,sizeof(DREB_CONN));
	memset(&m_s_srv,0,sizeof(DREB_SRV));

	//心跳参数
	m_nBeatheartcount=0;        //发送心跳次数
	//socket公共参数
	m_sock = INVALID_SOCKET;                   //socket id
	m_nType = SOCK_UNKNOW;                  //socket type SOCK_UNKNOW SOCK_CLI SOCK_DREB SOCK_SRV SOCK_TODREB
	m_bChecked=false;               //是否检查认证过，即确认过连接类型
	m_bNeedread=false;              //是否需要读数据
	
	m_nReadtime= 0;             //上次读数据时间
//	m_nConntime=0;              //连接建立时间
	m_nSendtime=0;              //上次发送数据时间 
	m_nPingTime = 0;
	
	m_nSend_packs=0;          //自连接建立起发送的数据包
	m_nRcv_packs=0;           //自连接建立起接收的数据包
	m_nSendBytes=0;           //自连接建立起发送的字节数
	m_nRcvBytes=0;            //自连接建立起接收的字节数
	
	//发送接收数据	
	memset(m_sRcvBuffer,0,sizeof(m_sRcvBuffer));            //接收数据缓冲   
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
	if (m_nRcvBufLen >=0 && m_nRcvBufLen<DREBBUFSIZE)
	{
		//包头未接收完
		ret = m_tcpSocket.Receive(m_sRcvBuffer+m_nRcvBufLen,DREBBUFSIZE-m_nRcvBufLen);
		if (ret <= 0)
		{
			return ret;
		}
		m_nRcvBufLen+=ret;
		m_nReadtime =time(NULL);
		m_nPingTime = m_nReadtime;
		//记录统计信息
		m_nRcvBytes+=ret;
		return ret;
	}
	return 0;
}

int CSocketInfo::OnSend()
{
	int ret;
	if (m_pSendQueue == NULL)
	{
		m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"连接index=%d type=%d 发送队列为NULL",m_index,m_nType);
		return -1;
	}
	if (m_sock == INVALID_SOCKET)
	{
		m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"连接已断开");
		return -2;
	}
	if (!m_pSendQueue->NeedSend())
	{
		return -3;
	}
	CBF_PMutex pp(&m_mutex);
	RDQUEUE::iterator prq,prn;
	int sendNum=0;
    if (m_pSendQueue == NULL)
    {
        m_log->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "连接index=%d type=%d 发送队列为NULL", m_index, m_nType);
        return -1;
    }
	for (prn=m_pSendQueue->m_qSendData.datas.begin();prn!=m_pSendQueue->m_qSendData.datas.end();)
	{
		prq = prn;
		prn++;
		if ( time(NULL) - (*prq)->msghead.rtime > m_nQueueTime && (*prq)->msghead.nTotalLen == (*prq)->msghead.nSendLen )
		{
			m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"请求在队列超时 index[%d] type[%s]的消息 s_nDrebSerial[%d]  数据长度[%d]",\
					m_index,GetSockType(m_nType).c_str(),(*prq)->msghead.serial,(*prq)->msghead.nTotalLen);
			m_pMemPool->PoolFree((*prq));
			m_pSendQueue->DeleteSend(prq);	
			continue;
		}
		if ((*prq)->msghead.nSendLen <=0)
		{
			m_pMemPool->PoolFree((*prq));
			m_pSendQueue->DeleteSend(prq);	
			continue;
		}
		m_tcpSocket.AttachSocket(m_sock,"");
		ret = m_tcpSocket.Send((char *)&((*prq)->message)+((*prq)->msghead.nTotalLen-(*prq)->msghead.nSendLen),(*prq)->msghead.nSendLen);
		if (ret == (*prq)->msghead.nSendLen)
		{
			//统计信息
			m_pStatInfo->ReportSendByte(m_nType,ret);
			m_pStatInfo->ReportSendPack(m_nType,1);
			
			m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"发送成功 index[%d] type[%s]的消息 s_nDrebSerial[%d]  数据长度[%d]",\
					m_index,GetSockType(m_nType).c_str(),(*prq)->msghead.serial,(*prq)->msghead.nSendLen);
			m_pMemPool->PoolFree((*prq));
			// 处理成功,停止
			m_pSendQueue->DeleteSend(prq);	
			m_nSendtime = time(NULL);
			
		}
		else 
		{
			if (ret >0)
			{
				(*prq)->msghead.nSendLen = (*prq)->msghead.nSendLen -ret;
				m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"发送部分数据 [%d]，准备重发 index[%d] type[%s]的消息 s_nDrebSerial[%d]  数据长度[%d]",\
					ret,m_index,GetSockType(m_nType).c_str(),(*prq)->msghead.serial,(*prq)->msghead.nSendLen);
				
			}
			else
			{
				m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"发送失败，准备重发 index[%d] type[%s]的消息 s_nDrebSerial[%d]  数据长度[%d]",\
					m_index,GetSockType(m_nType).c_str(),(*prq)->msghead.serial,(*prq)->msghead.nSendLen);
			}
			return sendNum;
		}
	}
	

	return sendNum;
}
int CSocketInfo::SendMsg(S_DREB_RSMSG *msg,bool sendimmediate)
{
	int ret;
	if (m_sock == INVALID_SOCKET)
	{
		m_pMemPool->PoolFree(msg);
		m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"连接 [%d] 已关闭",m_index);
		return -1;
	}
	if (m_nType == SOCK_CLI || m_nType == SOCK_UNKNOW)
	{
		//比较连接时间戳
		if (msg->message.head.s_Sinfo.s_nTimestamp != m_nConntime)
		{
			if (msg->message.head.cCmd != CMD_PING && msg->message.head.cCmd != CMD_CONNECT && msg->message.head.cCmd != CMD_REGSERVICE)
			{
				m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"连接时间戳不符，删除 index[%d] type[%s]的消息 DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 业务数据长度[%d]",\
					m_index,GetSockType(m_nType).c_str(),\
					GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
					msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
					msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,msg->message.head.nLen);
			}
			else
			{
				m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"连接时间戳不符，删除 index[%d] type[%s]的消息 DREB命令[%s]  业务数据长度[%d]",\
					m_index,GetSockType(m_nType).c_str(),GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.nLen);
			}
			m_pRes->g_pDataLog.LogDreb(LOG_ERROR_FAULT,&(msg->message),true);
			m_pMemPool->PoolFree(msg);
			//记录日志
			return -1;
		}
	}
	
	if (msg->message.head.nLen > 200 && msg->message.head.cZip == 0 ) //有数据且未压缩
	{
		if (m_pRes->g_nZip == 1)
		{
#ifndef  WIN64
			if (msg->message.head.cCmd == CMD_DPCALL || msg->message.head.cCmd == CMD_DPACALL || \
				msg->message.head.cCmd == CMD_DPPOST || msg->message.head.cCmd == CMD_DPPUSH || \
				msg->message.head.cCmd == CMD_DPBC   || msg->message.head.cCmd == CMD_DPABC || \
				msg->message.head.cCmd == CMD_MONITOR_DREB || msg->message.head.cCmd == CMD_MONITOR_BPC )
			{  //当为以上命令字时进行压缩
				
				unsigned int dlen = sizeof(m_sSendBuffer)-1;
				if (!CBF_Tools::Compress((unsigned char *)m_sSendBuffer,dlen,(unsigned char *)(msg->message.buffer),msg->message.head.nLen))
				{
					m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"数据压缩失败");
					m_pRes->g_pDataLog.LogDreb(LOG_ERROR_FAULT,&(msg->message),true);
					m_pMemPool->PoolFree(msg);
					return -101;
				}
				else
				{
					m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"数据压缩成功 %d->%d",msg->message.head.nLen,dlen);
					msg->message.head.nLen = dlen;
					memcpy(msg->message.buffer,m_sSendBuffer,dlen);
					msg->message.head.cZip = 2;
				}
				
			}
#endif
		}
		
	}
	if (msg->message.head.cCmd != CMD_PING && msg->message.head.cCmd != CMD_CONNECT && msg->message.head.cCmd != CMD_REGSERVICE)
	{
		m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"发送 index[%d] type[%s]的消息 DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] cZip[%d] 业务数据长度[%d]",\
			m_index,GetSockType(m_nType).c_str(),\
			GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
			msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
			msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
			msg->message.head.cZip,msg->message.head.nLen);
	}
	else
	{
		m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"发送 index[%d] type[%s]的消息 DREB命令[%s]  业务数据长度[%d]",\
			m_index,GetSockType(m_nType).c_str(),GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.nLen);
	}
	m_pRes->g_pDataLog.LogDreb(LOG_DEBUG,&(msg->message),false);

	msg->msghead.nTotalLen = msg->message.head.nLen+DREBHEADLEN;
	msg->msghead.nSendLen = msg->msghead.nTotalLen;
	msg->msghead.serial =    msg->message.head.s_Sinfo.s_nDrebSerial;

	if (m_pRes->g_nCrcFlag == 1)
	{
		m_pDrebEndian.Endian2CommGenCrc(&(msg->message.head));
	}
	else
	{
		m_pDrebEndian.Endian2Comm(&(msg->message.head));
	}

	m_tcpSocket.AttachSocket(m_sock,"");
	CBF_PMutex pp(&m_mutex);

	if (sendimmediate)
	{

		ret = m_tcpSocket.Send((char *)&(msg->message),msg->msghead.nTotalLen);
		if (ret == msg->msghead.nTotalLen)
		{
			
			//统计信息
			m_pStatInfo->ReportSendByte(m_nType,ret);
			m_pStatInfo->ReportSendPack(m_nType,1);
			m_pMemPool->PoolFree(msg);
			// 处理成功,停止
			m_nSendtime = time(NULL);
			m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"发送s_nDrebSerial[%d]完成",msg->msghead.serial);
		}
		else 
		{
			if (ret > 0)
			{
				msg->msghead.nSendLen = msg->msghead.nTotalLen - ret;
				m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"发送部分成功 index[%d] type[%s]的消息 s_nDrebSerial[%d]  数据长度[%d]",\
					m_index,GetSockType(m_nType).c_str(),msg->msghead.serial,msg->msghead.nTotalLen);
			}
			else
			{
				m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"发送失败 index[%d] type[%s]的消息 s_nDrebSerial[%d]  数据长度[%d]",\
					m_index,GetSockType(m_nType).c_str(),msg->msghead.serial,msg->msghead.nTotalLen);
			}
			msg->msghead.sendnum = 0;
			//如果队列里有数据可能就会有问题
			if (!NeedSend())
			{
				PutSendMsg(msg);
			}
			else
			{
				OnClose("直接在连接发送数据不成功，但连接队列里仍有数据",__FILE__,__LINE__);
			}
		}
		return 0;
	}
	else
	{
		if (NeedSend())
		{
			m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"发送队列已有数据，放入队列 数据长度[%d]",msg->msghead.nTotalLen);
			PutSendMsg(msg);
		}
		else  //直接发送
		{
			
			ret = m_tcpSocket.Send((char *)&(msg->message),msg->msghead.nTotalLen);
			if (ret == msg->msghead.nTotalLen)
			{
				//统计信息
				m_pStatInfo->ReportSendByte(m_nType,ret);
				m_pStatInfo->ReportSendPack(m_nType,1);
				m_pMemPool->PoolFree(msg);
				// 处理成功,停止
				m_nSendtime = time(NULL);
				m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"发送s_nDrebSerial[%d]完成",msg->msghead.serial);
				
			}
			else 
			{
				if (ret > 0)
				{
					msg->msghead.nSendLen = msg->msghead.nTotalLen - ret;
				}
				m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"发送 index[%d] type[%s]的消息 s_nDrebSerial[%d]  数据长度[%d]",\
					m_index,GetSockType(m_nType).c_str(),msg->msghead.serial,msg->msghead.nTotalLen);
				msg->msghead.sendnum = 0;
				PutSendMsg(msg);
			}
			
// 			if (msg->message.head.cRaflag == 0)
//			{
//				m_pRes->PutTime();
//			}	
		}
	
	}
	
	return 0;	
}
int CSocketInfo::PutSendMsg(S_DREB_RSMSG *msg)
{
	if (m_pSendQueue == NULL)
	{
		return -2;
	}
	int ret;
	ret = m_pSendQueue->PutSendMsg(msg);
	if (ret<0)
	{
		m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"队列满丢弃 index[%d] type[%s]的消息 DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] cZip[%d] 业务数据长度[%d]",\
			m_index,GetSockType(m_nType).c_str(),\
			GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
			msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
			msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
			msg->message.head.cZip,msg->message.head.nLen);
		m_pMemPool->PoolFree(msg);
		msg = NULL;
		return -1;
	}
	return 0;
}



int CSocketInfo::GetRecvData(S_DREB_RSMSG *msg)
{
	//收到的数据大于包头
	if (m_nRcvBufLen >= DREBHEADLEN )
	{
		//记录开始时间，微秒
		
//		m_pRes->PutTime();
		//进行字节序处理并校验CRC
		memcpy(&(msg->message.head),m_sRcvBuffer,DREBHEADLEN);

		if (m_pRes->g_nCrcFlag == 1)
		{
			if (!m_pDrebEndian.Endian2LocalHostChkCrc(&(msg->message.head)))
			{
				//OnClose("CRC错误");
				return -1;
			}
		}
		else
		{
			m_pDrebEndian.Endian2LocalHost(&(msg->message.head));
		}
		if (msg->message.head.nLen <0 || msg->message.head.nLen> DREBDATASIZE)
		{
			m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"index[%d] 报文头[%d]长度不符",m_index,msg->message.head.nLen);
			return -1;
		}
		if (m_nRcvBufLen < msg->message.head.nLen + DREBHEADLEN)
		{
			//说明数据没有接收完整
			return 0;
		}
		if (msg->message.head.nLen>0)
		{
			memcpy(msg->message.buffer,m_sRcvBuffer+DREBHEADLEN,msg->message.head.nLen);
		}
		//收多了，将原包重新给m_sRcvBuffer
		m_nRcvBufLen = m_nRcvBufLen-DREBHEADLEN-msg->message.head.nLen;
		for (int i=0; i<m_nRcvBufLen; i++)
		{
			m_sRcvBuffer[i]=m_sRcvBuffer[i+DREBHEADLEN+msg->message.head.nLen];
		}
		
//		m_pRes->PutTime();
		
		//记录通讯平台统计信息
		m_nRcv_packs++;
		m_pStatInfo->ReportRcvByte(m_nType,DREBHEADLEN+msg->message.head.nLen);
		m_pStatInfo->ReportRcvPack(m_nType,1);

		if (m_nType == SOCK_CLI || m_nType == SOCK_SRV ) //将通讯平台信息赋值
		{
			if (msg->message.head.cRaflag == 0 )
			{
				msg->message.head.s_Sinfo.s_nDrebSerial = m_pRes->getTick();//通讯平台流水，标志唯一请求
				//20180328 songfree modi 判断是否为转移，若是不用重新赋值
				if (msg->message.head.a_Ainfo.a_cNodePrivateId != 100)
				{
					msg->message.head.s_Sinfo.s_cNodePrivateId = m_pRes->g_nDrebPrivateId;
					msg->message.head.s_Sinfo.s_nNodeId = m_pRes->g_nDrebId;
					msg->message.head.s_Sinfo.s_cSvrPrivateId = m_s_srv.cSvrPrivateId;
					msg->message.head.s_Sinfo.s_nSvrMainId = m_s_srv.nSvrMainId;
					msg->message.head.s_Sinfo.s_nIndex = m_index;
					msg->message.head.s_Sinfo.s_nTimestamp = m_nConntime;
				}
				
				
				//用此字段作为路由路径计数，超过16丢弃
				msg->message.head.a_Ainfo.a_nNodeId =0;
			}
			else
			{
				msg->message.head.a_Ainfo.a_nNodeId = m_pRes->g_nDrebId;
				msg->message.head.a_Ainfo.a_cNodePrivateId = m_pRes->g_nDrebPrivateId;
			}
		}
		

		//记入接收日志
		if (msg->message.head.cCmd != CMD_PING && msg->message.head.cCmd != CMD_CONNECT && msg->message.head.cCmd != CMD_REGSERVICE)
		{
			if (m_log->isWrite(LOG_DEBUG))
			{
				m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"接收数据 index[%d] type[%s]的消息 DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] cZip[%d] 业务数据长度[%d]",\
					m_index,GetSockType(m_nType).c_str(),\
					GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
					msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
					msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
					msg->message.head.cZip,msg->message.head.nLen);
			}
			
		}
		else
		{
			if (m_log->isWrite(LOG_DEBUG+1))
			{
				m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"接收数据 index[%d] type[%s]的消息 DREB命令[%s]  业务数据长度[%d]",\
					m_index,GetSockType(m_nType).c_str(),GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.nLen);
			}
		}
		
		if (msg->message.head.cRaflag == 1 && (msg->message.head.cCmd == CMD_DPABC || msg->message.head.cCmd == CMD_DPPUSH))
		{
			m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"CMD_DPABC或CMD_DPPUSH的应答,丢弃");
		}
		else
		{

			m_pRes->g_pDataLog.LogDreb(LOG_DEBUG,&(msg->message),false);
		}
		
// 		if (msg->message.head.cRaflag == 0)
// 		{
// 			m_pRes->PutTime();
// 		}
		
		return msg->message.head.nLen+DREBHEADLEN;
	}
	return 0;
}


void CSocketInfo::SetToDreb(const char *ip, int port,int brate)
{
	memset(&m_s_center,0,sizeof(DREB_CENTER));
	strcpy(m_sAddress,ip);
	strcpy(m_s_center.sAddress,ip);
	m_s_center.bNeedConnect = true;
	m_s_center.nPort = port;
	m_s_center.nbandwidth = brate;

	m_nType = SOCK_TODREB;
	m_nConntime = 0;
	m_nReadtime = 0;
	m_nSendtime = 0;
	m_nCloseTime = 0;
	m_bChecked = false;
	m_bNeedread = false;
	m_nBeatheartcount =0;
}


void CSocketInfo::SetClient()
{
	m_nType = SOCK_CLI;
	m_bChecked = true;
	m_bNeedread = true;
	m_nSendtime = time(NULL);
	m_nReadtime = time(NULL);
	m_nBeatheartcount =0;
}

void CSocketInfo::SetDreb(unsigned short NodeId, char cNodePrivateId,int bdrate,CSendQueue *sque)
{
	m_nType = SOCK_DREB;
	m_bChecked = true;
	m_bNeedread = true;
	m_nSendtime = time(NULL);
	m_nReadtime = time(NULL);
	m_s_conn.nNodeId = NodeId;
	m_s_conn.cNodePrivateId = cNodePrivateId;
	m_s_conn.nbandwidth = bdrate;
	m_nBeatheartcount =0;
	if (m_pSendQueue != NULL)
	{
		delete m_pSendQueue;
		m_pSendQueue = NULL;
	}
	m_pSendQueue = sque;
}

void CSocketInfo::SetSvr(unsigned int nSvrMainId, char cSvrPrivateId, CSendQueue *sque)
{
	m_nType = SOCK_SRV;
	m_bChecked = true;
	m_bNeedread = true;
	m_nSendtime = time(NULL);
	m_nReadtime = time(NULL);
	m_s_srv.cSvrPrivateId = cSvrPrivateId;
	m_s_srv.nSvrMainId = nSvrMainId;
	m_nBeatheartcount =0;
	if (m_pSendQueue != NULL)
	{
		delete m_pSendQueue;
		m_pSendQueue = NULL;
	}
	m_pSendQueue = sque;
	m_pSendQueue->m_pMemPool = m_pMemPool;
}

void CSocketInfo::SetAccept(SOCKET_HANDLE sock,const char *address)
{
	ResetInfo();
	m_nType = SOCK_UNKNOW;
	m_sock = sock;
	m_bChecked = false;
	m_bNeedread = true;
	m_nConntime = time(NULL);
	m_nSendtime = time(NULL);
	m_nReadtime = time(NULL);
	strcpy(m_sAddress,address);
	m_datetime.Update();
	m_datetime.ToCharDate(m_cCreateDate);
	m_nBeatheartcount =0;
	if (m_pSendQueue == NULL)
	{
		m_pSendQueue = new CSendQueue();
		m_pSendQueue->m_pMemPool = m_pMemPool;
	}
}
void CSocketInfo::SetConnectDreb(SOCKET_HANDLE sock,int conntime,CSendQueue *sque)
{
	m_nType = SOCK_TODREB;
	m_sock = sock;
	m_bChecked = true;
	m_bNeedread = true;
	m_datetime.Update();
	m_datetime.ToCharDate(m_cCreateDate);
	m_nBeatheartcount = 0;
	if (m_pSendQueue != NULL)
	{
		delete m_pSendQueue;
		m_pSendQueue = NULL;
	}
	m_pSendQueue = sque;
	m_pSendQueue->m_pMemPool = m_pMemPool;
	
}

bool CSocketInfo::NeedSend()
{
	if (m_pSendQueue != NULL)
	{
		return m_pSendQueue->NeedSend();
	}
	else
	{
		return false;
	}
}


bool CSocketInfo::NeedConnect()
{
	return m_s_center.bNeedConnect;
}

bool CSocketInfo::NeedRead()
{
	return m_bNeedread;
}

void CSocketInfo::ResetSInfo()
{
	//心跳参数
	m_nBeatheartcount=0;        //发送心跳次数
	
	//socket公共参数
	m_sock = INVALID_SOCKET;                   //socket id
	if (m_nType != SOCK_TODREB)
	{
		m_nType = SOCK_UNKNOW;                  //socket type SOCK_UNKNOW SOCK_CLI SOCK_DREB SOCK_SRV SOCK_TODREB
	}
	m_bChecked=false;               //是否检查认证过，即确认过连接类型
	m_bNeedread=false;              //是否需要读数据
	m_nReadtime= 0;             //上次读数据时间
	m_nConntime=0;              //连接建立时间
	m_nSendtime=0;              //上次发送数据时间 
	m_nSend_packs=0;          //自连接建立起发送的数据包
	m_nRcv_packs=0;           //自连接建立起接收的数据包
	m_nSendBytes=0;           //自连接建立起发送的字节数
	m_nRcvBytes=0;            //自连接建立起接收的字节数
	//发送接收数据	
	memset(m_sRcvBuffer,0,sizeof(m_sRcvBuffer));            //接收数据缓冲   
	m_nRcvBufLen=0;                //接收的数据长度  可多次接收一个数据包
	memset(m_cCreateDate,0,sizeof(m_cCreateDate));//连接建立日期
	m_nConntime = 0;
	memset(m_cCloseDate,0,sizeof(m_cCloseDate));//连接关闭日期
	m_nCloseTime = 0;
	
	m_sendNum = 10;
}

std::string CSocketInfo::GetSockType(int ntype)
{
// #define SOCK_UNKNOW   1      //未知
// #define SOCK_CLI      2      //客户端
// #define SOCK_DREB     3      //其它数据总线节点到本数据总线节点的连接
// #define SOCK_SRV      4      //注册的服务端
// #define SOCK_TODREB   5      //和其它数据总线节点节点的连接
// #define SOCK_SLOT	  6      //注册的通讯槽
// #define SOCK_LISTEN	  7      //侦听端口
	char tmpchar[30];
	bzero(tmpchar,sizeof(tmpchar));
	switch (ntype)
	{
		case 1:
			sprintf(tmpchar,"0-SOCK_UNKNOW");
			break;
		case 2:
			sprintf(tmpchar,"1-SOCK_CLI");
			break;
		case 3:
			sprintf(tmpchar,"3-SOCK_DREB");
			break;
		case 4:
			sprintf(tmpchar,"4-SOCK_SRV");
			break;
		case 5:
			sprintf(tmpchar,"5-SOCK_TODREB");
			break;
		case 6:
			sprintf(tmpchar,"6-SOCK_SLOT");
			break;
		case 7:
			sprintf(tmpchar,"7-SOCK_LISTEN");
			break;
		default:
			break;
	}
	return std::string(tmpchar);
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
	case  CMD_DPBC_UNREGTX:     //17 //取消服务的交易注册广播
		return "CMD_DPBC_UNREGTX";
	default:
		return "";
	}
}
void CSocketInfo::CheckFirstData()
{
	if (m_pSendQueue == NULL)
	{
		return;
	}
	RDQUEUE::iterator prq,prn;
	for (prn=m_pSendQueue->m_qSendData.datas.begin(); prn!=m_pSendQueue->m_qSendData.datas.end() ; )
	{
		prq = prn;
		prn++;
		if ((*prq)->msghead.nSendLen != (*prq)->msghead.nTotalLen)
		{
			m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"错误 队列中第一个数据包发送部分数据，重连后删除 %d",\
				(*prq)->msghead.index);
			m_pSendQueue->DeleteSend(prq);
		}
		break;
	}
}