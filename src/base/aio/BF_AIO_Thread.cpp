// BF_AIO_Thread.cpp: implementation of the CBF_AIO_Thread class.
//
//////////////////////////////////////////////////////////////////////

#include "BF_AIO_Thread.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBF_AIO_Thread::CBF_AIO_Thread()
{
	m_pAioPool = NULL;
	m_pAioWork = NULL;
	m_pLog = NULL;
	m_pAioRes = NULL;
	bzero(m_sErrMsg,sizeof(m_sErrMsg));
	m_bIsScan = false;
}

CBF_AIO_Thread::~CBF_AIO_Thread()
{
	m_pAioPool = NULL;
	m_pAioWork = NULL;
	m_pLog = NULL;
	m_pAioRes = NULL;
}
// 函数名: InitThreadInstance
// 编程  : 王明松 2012-4-9 12:00:54
// 返回  : virtual bool 
// 描述  : 线程初始化，线程启动时调用一次
bool CBF_AIO_Thread::InitThreadInstance()
{
	return true;
}

// 函数名: ExitThreadInstance
// 编程  : 王明松 2012-4-9 12:01:05
// 返回  : virtual void 
// 描述  : 退出线程要做的工作,线程退出时调用一次
void CBF_AIO_Thread::ExitThreadInstance()
{

}

// 函数名: Run
// 编程  : 王明松 2012-4-9 12:01:23
// 返回  : virtual int 
// 描述  : 线程运行方法，若此方法返回，则线程自动终止
int CBF_AIO_Thread::Run()
{
	if (m_pAioPool == NULL || m_pAioRes == NULL || m_pAioWork == NULL || m_pLog == NULL)
	{
		
		return -1;
	}
	if (m_pAioRes->g_nRunMode == MODEEPOLL)
	{
		RunEpoll();
	}
	else if (m_pAioRes->g_nRunMode == MODEIOCP)
	{
		RunIocp();
	}
	else
	{
		RunSelect();
	}
	
	return 0;
}

void CBF_AIO_Thread::SetParameter(CBF_AIO_Pool *pool, CBF_AIO_WorkBase *work,CBF_AIO_Resource  *res,unsigned int index,CIErrlog  *log)
{
	m_pAioPool = pool;
	m_pAioWork = work;
	m_pAioRes = res;
	m_nIndex = index;
	m_pLog = log;
}

void CBF_AIO_Thread::RunSelect()
{
	struct timeval tv;
	int result;
	int maxfd=0;
	int ret;
	int index;
	m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"启动SELECT线程 %d",m_nIndex);
	while (!m_pAioRes->g_bIsExit)
	{
		FD_ZERO(&m_rset);
		FD_ZERO(&m_wset);
#if defined(_WINDOWS)
		FD_ZERO(&m_eset);
#endif

		tv.tv_sec = 1;
		tv.tv_usec = 0;
		//取得最大的socket并将socket加入到fd_set集合
		ret = SelectGetMaxfdAdd(maxfd);

		if (ret  < 1) //没有连接加入到本线程进行处理
		{
			SLEEP(5);
			continue;
		}
// 		if (ret >0)
// 		{
// 			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"本线程[%d]连接数[%d]",GetThreadID(),ret);
// 		}
#if defined(_WINDOWS)
		result = select(maxfd+1, &m_rset, &m_wset, &m_eset, &tv);
#else
		result = select(maxfd+1, &m_rset, &m_wset, NULL, &tv);
#endif
		if (result != 0) 
		{
//			m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"%d连接 有事件发生",ret);
			bool isdata = m_lSocketList.First(index);
			while (isdata)
			{
				PSOCKET_POOL_DATA info = m_pAioPool->GetDataByIndex(index);
				if (info != NULL)
				{
					SelectOnReadEvent(info);
					if (info->s_hSocket != INVALID_SOCKET)
					{
						SelectOnWriteEvent(info);
					}
				}
				//检查ret连接上的事件
				isdata = m_lSocketList.Next(index);
			}
		}
		else
		{
//			m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"%d连接 无事件发生",ret);
		}

		//扫描连接池
		if (m_bIsScan)
		{
			ScanPool();
			m_bIsScan = false;
		}
		
	}
	m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"停止SELECT线程 %d",m_nIndex);
	return ;
}

int CBF_AIO_Thread::SelectGetMaxfdAdd(int &maxfd)
{
	bool isadd=false;
	maxfd = 0;
	int num=0;
	int index;
	m_lSocketList.Clear();
	bool isdata = m_lIndexList.First(index);
	while (isdata)
	{
		isadd = false;
		//要加入到本线程的select中
		PSOCKET_POOL_DATA info = m_pAioPool->GetDataByIndex(index);
		if (info != NULL)
		{
			if (info->s_hSocket == INVALID_SOCKET)
			{
				isdata = m_lIndexList.Next(index);
				continue;
			}
			switch (info->s_cSocketStatus)
			{
				case  SOCKSTATUS_RW:
					FD_SET(info->s_hSocket, &m_rset);//加入到读select集合
					FD_SET(info->s_hSocket, &m_wset);//加入到写select集合
					isadd = true;
					m_pLog->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"加入select SOCKSTATUS_RW index=%d sock=%d",index,info->s_hSocket);
					break;
				case  SOCKSTATUS_R:
					FD_SET(info->s_hSocket, &m_rset);//加入到读select集合
					isadd = true;
					if (info->s_pSendQueue.size()>0 || info->s_pQuoteSendQueue.size()>0)
					{
						FD_SET(info->s_hSocket, &m_wset);//加入到写select集合
					}
					m_pLog->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"加入select SOCKSTATUS_R index=%d sock=%d",index,info->s_hSocket);
					break;
				case  SOCKSTATUS_C:
					FD_SET(info->s_hSocket, &m_wset);//加入到写select集合
	#if defined(_WINDOWS)
					FD_SET(info->s_hSocket, &m_eset);//加入到异常select集合
	#endif
					isadd = true;
					m_pLog->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"加入select SOCKSTATUS_C index=%d sock=%d",index,info->s_hSocket);
					break;
				case  SOCKSTATUS_W:
					FD_SET(info->s_hSocket, &m_wset);//加入到写select集合
					isadd = true;
					m_pLog->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"加入select SOCKSTATUS_W index=%d sock=%d",index,info->s_hSocket);
					break;
				default:
					m_pLog->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"加入select NULL index=%d sock=%d",index,info->s_hSocket);
					break;
				
			}
			if (isadd)
			{
//				m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"socket[%d] index[%d]",info->s_hSocket,info->s_nIndex);
				if (maxfd < info->s_hSocket)
				{
					maxfd = info->s_hSocket;
				}
				m_lSocketList.Add(info->s_nIndex);
				num++;
			}
		}
		else
		{
			//==NULL表示已到实际连接数最大值了
			break;
		}
		isdata = m_lIndexList.Next(index);
	}
	return num;
}

void CBF_AIO_Thread::SelectOnReadEvent(PSOCKET_POOL_DATA info)
{
	int ret;
	if (info->s_hSocket == INVALID_SOCKET)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"SOCKET已关闭");
		return;
	}
	ret = FD_ISSET(info->s_hSocket,&m_rset);
	if (ret != 0) //有读事件
	{
//		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"index=%d 有读事件",info->s_nIndex);
		if (info->s_cSocketType == SOCK_LISTEN)
		{
			m_pSocketTcp.AttachSocket(info->s_hSocket,inet_ntoa(info->s_pSocketAddr.sin_addr));
			SOCKADDR_IN s_pSocketAddr;
			SOCKET_HANDLE tmpsock = m_pSocketTcp.Accept(s_pSocketAddr,false);
			if (tmpsock != INVALID_SOCKET)
			{
				PSOCKET_POOL_DATA data = m_pAioPool->PoolMalloc();
				if (data == NULL)
				{
					int nsize=0;
					int nouse=0;
					m_pAioPool->PoolInfo(nsize,nouse);
					char msg[500];
					bzero(msg,sizeof(msg));
					m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"连接数已满[%d %d]来自连接[%s]的连接被拒绝",nsize,nouse,msg);
					closesocket(tmpsock);
					return;
				}
				data->s_hSocket = tmpsock;
				data->s_pSocketAddr = s_pSocketAddr;
				data->s_cSocketType = SOCK_CLIENT;
				m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"收到[%s %d]的连接 index[%d] sock[%d]",\
					inet_ntoa(data->s_pSocketAddr.sin_addr),data->s_pSocketAddr.sin_port,\
					data->s_nIndex,data->s_hSocket);
				OnAccept(data);
				return;
			}
			return;
		}
		m_pSocketTcp.AttachSocket(info->s_hSocket,inet_ntoa(info->s_pSocketAddr.sin_addr));
		ret = m_pSocketTcp.Receive(info->s_sRealBuffer+info->s_nRealBufferLen,DREBBUFSIZE - info->s_nRealBufferLen);
		if (ret <= 0)
		{
			//连接有读事件，但未收到数据，对方关闭连接
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"来自[%s]的连接index[%d]socket[%d] 对方主动关闭连接",\
				inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,info->s_hSocket);
			OnClose(info,__FILE__,__LINE__,"对方主动关闭连接");
			return ;
		}
		//监控数据
		m_pAioRes->g_nRecvByteLen +=ret;
		info->s_nRealBufferLen+=ret;
		info->s_nReadTime = time(NULL);
		//业务处理收到的数据
		OnRecvBack(info);
		return;
	}
// 	else
// 	{
// 		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"来自[%s]的连接index[%d]socket[%d] 无读消息",\
// 				inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,info->s_hSocket);
// 	}
}

void CBF_AIO_Thread::SelectOnWriteEvent(PSOCKET_POOL_DATA info)
{
	int ret;
	if (info->s_hSocket == INVALID_SOCKET)
	{
		return;
	}
#if defined(_WINDOWS)
	ret = FD_ISSET(info->s_hSocket, &m_eset);
	if (ret != 0)
	{
		//主动连接其它
		if (info->s_cSocketStatus  == SOCKSTATUS_C)
		{
			OnConnectBack(info,false);//连接失败
		}
		else 
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"来自[%s]的连接index[%d]socket[%d] windows连接异常出错",\
				inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,info->s_hSocket);
			OnClose(info,__FILE__,__LINE__,"windows连接异常出错");
		}
	}
	else if (FD_ISSET(info->s_hSocket, &m_wset) !=0 )
	{
		//主动连接其它
		if (info->s_cSocketStatus  == SOCKSTATUS_C)
		{
			OnConnectBack(info,true);//连接成功，通知应用
			return;
		}
		else //正常连接，可写
		{
			//那就发数据吧
			SelectSend(info);
		}
	}
#else
	
	int ok;
	int oklen;
	if (FD_ISSET(info->s_hSocket, &m_wset) !=0)
	{
		if (info->s_cSocketStatus  == SOCKSTATUS_C)
		{
			oklen=sizeof(ok);
#if defined(HP_UX)
			getsockopt(info->s_hSocket,SOL_SOCKET,SO_ERROR,(char *)&ok,&oklen);
#else	
			getsockopt(info->s_hSocket,SOL_SOCKET,SO_ERROR,(char *)&ok,(socklen_t *)&oklen);
#endif
			if (ok == 0)
			{
				OnConnectBack(info,true);//连接成功，通知应用
			}
			else
			{
				OnConnectBack(info,false);//连接失败，通知应用
			} // end if (ok == 0)
		}
		else  //正常连接，可写
		{
			//那就发数据吧
			SelectSend(info);
		}
	}

#endif
}

void CBF_AIO_Thread::SelectSend(PSOCKET_POOL_DATA info)
{
	int ret;
	
	int sendlen=0;
	
	int onceSendNum=0;
	//把缓冲里的数据全发出去，只到不能发
	SOCKSENDQUEUE::iterator prq,prn;
	CBF_PMutex pp(&info->s_mutex);
	m_pSocketTcp.AttachSocket(info->s_hSocket,"");
	m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"select 开始从队列发送数据");

	if (info->s_nSendQueueFlag == 1 || info->s_nSendQueueFlag == 0)
	{
		for (prn=info->s_pSendQueue.begin() ; prn!=info->s_pSendQueue.end() ; )
		{
			prq = prn;
			prn++;
			//一些判断
			if (prq->s_nIndex != info->s_nIndex || prq->s_nTimestamp != info->s_nTimestamp )
			{
				prq->s_nRet = -1;
				prq->s_nSendNum++;
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"来自[%s]的连接index[%d] 标识[%d] socket[%d] 发送队列里数据时间戳不符,移出",\
					inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,prq->s_nHook,info->s_hSocket);
				m_pAioWork->OnSendBack(prq,false);//返回给应用，由应用释放缓冲等
				info->s_pSendQueue.erase(prq);//从队列删除
				continue;
			}
			//发送数据
			//针对ET模式要修改，只到发送完
			
			while (prq->s_nSendLen < prq->s_nTotalLen )
			{
				ret = m_pSocketTcp.Send(prq->s_sSendBuffer+prq->s_nSendLen,prq->s_nTotalLen-prq->s_nSendLen);
				if (ret <= 0)
				{
					m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"来自[%s]的连接index[%d] 标识[%d] socket[%d] 发送失败[%d] 在队列里继续发",\
						inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,prq->s_nHook,info->s_hSocket,ret);
					//发送部分数据置标志
					if (prq->s_nSendLen > 0 )
					{
						info->s_nSendQueueFlag = 1;
					}
					else
					{
						info->s_nSendQueueFlag = 0;
					}
					return ;
				}
				else if (ret == (prq->s_nTotalLen-prq->s_nSendLen) ) //发完了
				{
					m_pAioRes->g_nSendByteLen +=ret;
					m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"来自[%s]的连接index[%d] 标识[%d] socket[%d] 从队列发送数据完成 发送数据长度[%d]",\
						inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,prq->s_nHook,info->s_hSocket,ret);
					m_pAioWork->OnSendBack(prq,true);//返回给应用，由应用释放缓冲等
					info->s_pSendQueue.erase(prq);//从队列删除
					//发送完整数据库置状态为优先发送优先队列数据
					info->s_nSendQueueFlag = 0;
				
					break;
					
				}
				prq->s_nSendLen +=ret;
				m_pAioRes->g_nSendByteLen +=ret;

				
			}
			onceSendNum++;
			if (onceSendNum >= m_pAioRes->g_nSendOnceNum)
			{
				return;
			}
		}  //end for 发送
	}
	
	int  delaytime;
	//开始发送行情队列，只有上面的队列发送完才进行
	for (prn=info->s_pQuoteSendQueue.begin() ; prn!=info->s_pQuoteSendQueue.end() ; )
	{
		prq = prn;
		prn++;
		//发送数据
		delaytime = time(NULL) - prq->s_nTime ;


		if (prq->s_nSendLen == 0)  //是完整的没有发送过的
		{
			//行情在队列里超过配置的秒超时则丢弃
			if ( delaytime > m_pAioRes->g_nQuoteQueueDeleteTime )
			{
				m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"来自[%s]的连接index[%d] 标识[%d] socket[%d] 发送行情队列数据时间过期 [%d > %d]秒,移出",\
					inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,prq->s_nHook,info->s_hSocket,delaytime,m_pAioRes->g_nQuoteQueueDeleteTime);
				m_pAioWork->OnSendBack(prq,false);//返回给应用，由应用释放缓冲等
				info->s_pQuoteSendQueue.erase(prq);//从队列删除
				continue;	
			}
		}
		
		//针对ET模式要修改，只到发送完
		
		while (prq->s_nSendLen < prq->s_nTotalLen )
		{
			ret = m_pSocketTcp.Send(prq->s_sSendBuffer+prq->s_nSendLen,prq->s_nTotalLen-prq->s_nSendLen);
			if (ret <= 0)
			{
				m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"来自[%s]的连接index[%d] 标识[%d] socket[%d] 发送行情失败[%d] 在队列里继续发",\
					inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,prq->s_nHook,info->s_hSocket,ret);
				//				struct epoll_event ev;
				// 				ev.data.ptr = info;
				// 				ev.events = EPOLLPRI|EPOLLERR|EPOLLIN|EPOLLOUT|EPOLLHUP;//要检查是否可写，当可写时再发
				// 				epoll_ctl(m_pAioRes->g_nEpoll, EPOLL_CTL_MOD, info->s_hSocket, &ev);
				//发送部分数据置标志
				if (prq->s_nSendLen > 0 )
				{
					info->s_nSendQueueFlag = 2;
				}
				else
				{
					info->s_nSendQueueFlag = 0;
				}
				return ;
			}
			else if (ret == (prq->s_nTotalLen-prq->s_nSendLen) ) //发完了
			{
				m_pAioRes->g_nSendByteLen +=ret;
				m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"来自[%s]的连接index[%d] 标识[%d] socket[%d] 从队列发送行情数据完成 发送数据长度[%d]",\
					inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,prq->s_nHook,info->s_hSocket,ret);
				m_pAioWork->OnSendBack(prq,true);//返回给应用，由应用释放缓冲等
				info->s_pQuoteSendQueue.erase(prq);//从队列删除
				//发送完整数据置状态为优先发送优先队列数据
				info->s_nSendQueueFlag = 0;
				
				break;
				
			}
			prq->s_nSendLen +=ret;
			m_pAioRes->g_nSendByteLen +=ret;
		}
		if (++onceSendNum >= m_pAioRes->g_nSendOnceNum)
		{
			return;
		}
	}  //end for 发送
	//发送完成，不用再检查可写
	info->s_cSocketStatus = SOCKSTATUS_R;


// 	for (prn=info->s_pSendQueue.begin() ; prn!=info->s_pSendQueue.end() ; )
// 	{
// 		prq = prn;
// 		prn++;
// 		//一些判断
// 		if ( prq->s_nIndex != info->s_nIndex || prq->s_nTimestamp != info->s_nTimestamp )
// 		{
// 			prq->s_nRet = -1;
// 			prq->s_nSendNum++;
// 			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"来自[%s]的连接index[%d]socket[%d] 发送队列里数据不符,移出",\
// 				inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,info->s_hSocket);
// 
// 			m_pAioWork->OnSendBack(prq,false);//返回给应用，由应用释放缓冲等
// 			info->s_pSendQueue.erase(prq);//从队列删除
// 			continue;
// 		}
// 		ret = SelectSend(prq);
// 		if (ret == prq->s_nTotalLen - prq->s_nSendLen)
// 		{
// 			prq->s_nRet = ret;
// 			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"发送完成 连接index[%d] 标识[%d] sock[%d] ip[%s] port[%d] 可写后发送数据[%d]",\
// 				info->s_nIndex,prq->s_nHook,info->s_hSocket,inet_ntoa(info->s_pSocketAddr.sin_addr),\
// 				info->s_pSocketAddr.sin_port,ret);
// 			m_pAioWork->OnSendBack(prq,false);//返回给应用，由应用释放缓冲等
// 			info->s_pSendQueue.erase(prq);//从队列删除
// 			
// 		}
// 		else if (ret >0 ) 
// 		{
// 			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"发送部分数据 连接index[%d] 标识[%d] sock[%d] ip[%s] port[%d] 可写后发送数据[%d]",\
// 				info->s_nIndex,prq->s_nHook,info->s_hSocket,inet_ntoa(info->s_pSocketAddr.sin_addr),\
// 				info->s_pSocketAddr.sin_port,ret);
// 		}
// 	}  //end for 发送
}

int CBF_AIO_Thread::SelectSend(SOCKSENDQUEUE::iterator prq)
{
	//发送数据
	int ret;
	int dlen=0;
	CBF_PMutex pp(&(m_pAioPool->GetDataByIndex(prq->s_nIndex)->s_mutex));
	while (prq->s_nSendLen > 0)
	{
		ret = m_pSocketTcp.Send(prq->s_sSendBuffer+prq->s_nSendLen,prq->s_nTotalLen-prq->s_nSendLen);
		if (ret < 0)
		{
			return -1;
		}
		if (ret == 0) //不能发了
		{
			return dlen;
		}
		dlen+=ret;
		prq->s_nSendLen +=ret;
		m_pAioRes->g_nSendByteLen +=ret;
	}
	return dlen;
	
}

void CBF_AIO_Thread::RunIocp()
{
#if defined(_WINDOWS)
	while (!m_pAioRes->g_bIsExit)
	{
		BOOL Status = 0;
        DWORD NumTransferred = 0;
		DWORD  perkey;
        PSOCKET_POOL_DATA info = NULL;
#if _MSC_VER > 1200
        Status = GetQueuedCompletionStatus( m_pAioRes->g_hIocp,&NumTransferred, (PULONG_PTR)&perkey, (LPOVERLAPPED *)&info,INFINITE );
#else
		Status = GetQueuedCompletionStatus( m_pAioRes->g_hIocp,&NumTransferred, (LPDWORD)&perkey, (LPOVERLAPPED *)&info,INFINITE );
#endif
        if (!Status)
        {
			if (info != NULL)
			{
				//说明完成端口返回了，但操作失败
				switch (info->s_cOperType)
				{
					case OP_READ:
						m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"来自[%s]的连接index[%d]socket[%d] 完成端口GetQueuedCompletionStatus失败",\
							inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,info->s_hSocket);
						OnClose(info,__FILE__,__LINE__,"完成端口GetQueuedCompletionStatus失败");
						break;
					case OP_WRITE:
						m_pAioWork->OnSendBack(info->s_pSendData,false);
						break;
					default:
						m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"来自[%s]的连接index[%d]socket[%d] 完成端口GetQueuedCompletionStatus失败",\
							inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,info->s_hSocket);
						OnClose(info,__FILE__,__LINE__,"完成端口GetQueuedCompletionStatus失败");
						break;
				}
			}
			continue;
        }
		if( NumTransferred == 0 && (info->s_cOperType == OP_READ || info->s_cOperType == OP_WRITE))
		{
			//连接被关闭
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"来自[%s]的连接index[%d]socket[%d] 连接被对方关闭",\
				inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,info->s_hSocket);
			OnClose(info,__FILE__,__LINE__,"连接被对方关闭");
			continue;

		}
        if ( 1 == perkey ) //收到退出的通知了
        {
			break;
        }
		WSABUF buf;
		DWORD dwRecv = 0; 
		DWORD dwFlags = 0;
		DWORD dwBytes;
		PSOCKET_POOL_DATA infotmp = NULL;

		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"连接index[%d] 有消息过来了 s_cOperType[%d]",info->s_nIndex,info->s_cOperType);
		switch (info->s_cOperType)
		{
			case OP_READ:
				//原先余留的数据加上新收的数据
				info->s_nRealBufferLen += info->s_nIoBufferLen;
				//在OnRecvBack里由应用取走数据，重置s_pIoBuffer指针及s_nIoBufferLen
				m_pAioWork->OnRecvBack(info);
				//继续发送一个接收数据的请求
				info->s_cOperType = OP_READ;
				
				buf.buf = info->s_pIoBuffer;
				buf.len = info->s_nIoBufferLen;
				info->s_nReadTime = time(NULL);
				WSARecv(info->s_hSocket,&buf,1,&dwRecv,&dwFlags,(LPOVERLAPPED)info,NULL);
				break;
			case OP_WRITE:
				//发送成功完数据，通知一下应用
				m_pAioWork->OnSendBack(info->s_pSendData,true);
				
				break;
			case OP_ACCEPT:
				//有连接过来了,通知应用进行检查等
				OnAccept(info);
				//再发一个Accept
				m_pSocketTcp.Create(AF_INET,false);//非阻塞
				//分配一个新的空的连接信息
				infotmp = m_pAioPool->PoolMalloc();
				if (infotmp == NULL)
				{
					m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"连接数已满，拒绝连接");
					//不发AcceptEx可能会导致后来的连接连不上了,统一在scanpool里做吧
					break;
				}
				infotmp->s_hSocket = m_pSocketTcp.GetSocket();
				infotmp->s_cSocketType = SOCK_ACCEPT;
				infotmp->s_cOperType = OP_ACCEPT;
				infotmp->s_cSocketStatus = SOCKSTATUS_UNKNOW;
				m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"index=%d 开始AcceptEx ",infotmp->s_nIndex);
				AcceptEx(m_pAioPool->GetDataByIndex(m_pAioRes->g_nListenIndex)->s_hSocket,infotmp->s_hSocket,infotmp->s_pIoBuffer,0,sizeof(sockaddr_in)+16, sizeof(sockaddr_in)+16,&dwBytes, (LPOVERLAPPED)infotmp);

				break;
			case OP_CONNECT:
				//主动连接，通知应用连接成功
				m_pAioWork->OnConnectBack(info,true);
				break;
			default:
				break;
		}
        
		
	}
#endif
}

void CBF_AIO_Thread::RunEpoll()
{
// 	    EPOLLIN ：表示对应的文件描述符可以读；
// 		EPOLLOUT：表示对应的文件描述符可以写；
// 		EPOLLPRI： 表示对应的文件描述符有紧急的数可读；
// 		
// 		EPOLLERR：表示对应的文件描述符发生错误；
// 		EPOLLHUP：表示对应的文件描述符被挂断；
//      EPOLLET：    ET的epoll工作模式；

#if defined(LINUX)
	int ret;
	int nfds;
	struct epoll_event ev;
	struct epoll_event eopollEvents[EPOLLMAXFD];
	m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"开始epoll线程 最大连接数%d",m_pAioRes->g_nMaxConnectNum);
	while(!m_pAioRes->g_bIsExit)
    {
        nfds = epoll_wait(m_pAioRes->g_nEpoll, eopollEvents, m_pAioRes->g_nMaxConnectNum, 1000);
		
        for(int i=0 ; i < nfds; i++)
        {
			PSOCKET_POOL_DATA data = (PSOCKET_POOL_DATA)eopollEvents[i].data.ptr;
			if (data->s_hSocket == INVALID_SOCKET)
			{
				continue;
			}
			if (eopollEvents[i].events & EPOLLIN || eopollEvents[i].events & EPOLLPRI ) //读事件
			{
				if (data->s_cOperType == OP_LISTEN && data->s_nIndex == 0) //侦听
				{
					while (true)
					{
						m_pSocketTcp.AttachSocket(data->s_hSocket,"");
						m_pSocketTcp.SetBlockFlag(false);
						SOCKADDR_IN        s_pSocketAddr;
						SOCKET_HANDLE tmpsocket = m_pSocketTcp.Accept(s_pSocketAddr,false);
						if (tmpsocket != INVALID_SOCKET)
						{
							PSOCKET_POOL_DATA info = m_pAioPool->PoolMalloc();
							if (info == NULL)
							{
								int nsize=0;
								int nouse=0;
								m_pAioPool->PoolInfo(nsize,nouse);
								char msg[500];
								bzero(msg,sizeof(msg));
								m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"连接数已满[%d %d]来自连接[%s]的连接被拒绝",nsize,nouse,msg);
								closesocket(tmpsocket);
								continue;
							}
							info->s_hSocket = tmpsocket;
							info->s_pSocketAddr = s_pSocketAddr;
							int nAll=0;
							int nFree=0;
							m_pAioPool->PoolInfo(nAll,nFree);
							m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"接受客户端来自连接[%s]的连接 index[%d] socket[%d]的连接  总连接数[%d] 空闲数[%d]",\
								inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,info->s_hSocket,nAll,nFree);

							OnAccept(info);
							continue;
						}
						
						break;
					}

				}
				else
				{
// 					m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"来自[%s]的连接index[%d]socket[%d]有读事件",\
// 						inet_ntoa(data->s_pSocketAddr.sin_addr),data->s_nIndex,data->s_hSocket);
					EpollRecv(data);
				}
				
			}
            if (eopollEvents[i].events & EPOLLOUT ) //写事件
			{
// 				m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"来自[%s]的连接index[%d]socket[%d]响应写事件",\
// 					inet_ntoa(data->s_pSocketAddr.sin_addr),data->s_nIndex,data->s_hSocket);
				if (data->s_hSocket != INVALID_SOCKET)
				{
					EpollSend(data);
				}
			}
			if (eopollEvents[i].events & EPOLLERR  || eopollEvents[i].events & EPOLLHUP  ) //关闭连接事件
			{
				if (data->s_hSocket != INVALID_SOCKET)
				{
// 					ev.data.ptr = data;
// 					ret = epoll_ctl(m_pAioRes->g_nEpoll, EPOLL_CTL_DEL, data->s_hSocket, &ev);
// 					if (ret != 0)
// 					{
// 						m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"来自[%s]的连接index[%d]socket[%d] EPOLL_CTL_DEL 失败[%d]",\
// 							inet_ntoa(data->s_pSocketAddr.sin_addr),data->s_nIndex,data->s_hSocket,ret);
// 					}
					m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"关闭连接 来自[%s]的连接index[%d]socket[%d] 被对方关闭",\
						inet_ntoa(data->s_pSocketAddr.sin_addr),data->s_nIndex,data->s_hSocket);
					OnClose(data,__FILE__,__LINE__,"关闭连接 连接被对方关闭");
				}
			}
        }
		//扫描连接池
		if (m_bIsScan)
		{
			ScanPool();
			m_bIsScan = false;
		}
    }
#endif
	m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"停止epoll线程");
}

void CBF_AIO_Thread::EpollRecv(PSOCKET_POOL_DATA info)
{
#if defined(LINUX)
	int ret;
	struct epoll_event ev;
	m_pSocketTcp.AttachSocket(info->s_hSocket,"");
	//不停的收，只到把缓冲区的数据收完
	
	//	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"接收数据 offset[%d]",info->s_nRealBufferLen);
	ret = m_pSocketTcp.Receive(info->s_sRealBuffer+info->s_nRealBufferLen,DREBBUFSIZE-info->s_nRealBufferLen);
	if (ret <= 0) 
	{
//   	ev.data.ptr = info;
//   	ret = epoll_ctl(m_pAioRes->g_nEpoll, EPOLL_CTL_DEL, info->s_hSocket, &ev);
// 		if (ret != 0)
// 		{
// 			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"来自[%s]的连接index[%d]socket[%d] EPOLL_CTL_DEL 失败[%d]",\
// 			inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,info->s_hSocket,ret);
// 		}
		m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"来自[%s]的连接index[%d]socket[%d]接收返回出错[%d] 数据长度[%d]",\
			inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,info->s_hSocket,ret,info->s_nRealBufferLen);
 		OnClose(info,__FILE__,__LINE__,"对方主动关闭连接");
		return;
	}
	info->s_nRealBufferLen+=ret;
	info->s_nReadTime = time(NULL);
	
	//监控数据
	m_pAioRes->g_nRecvByteLen +=ret;
//	m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"收到来自[%s]index[%d]socket[%d]的数据长度[%d]",\
//		inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,info->s_hSocket,info->s_nRealBufferLen);
	//调用消息处理
	m_pAioWork->OnRecvBack(info);
// 	ev.data.ptr = info;
// 	ev.events = EPOLLPRI|EPOLLERR|EPOLLIN|EPOLLHUP;//不用检查是否可写，只有当发送不了时才加入可写检查
//     epoll_ctl(m_pAioRes->g_nEpoll, EPOLL_CTL_MOD, info->s_hSocket, &ev);
#endif
	return;
}

void CBF_AIO_Thread::EpollSend(PSOCKET_POOL_DATA info)
{
#if defined(LINUX)
	int ret;
	int sendlen=0;

	int onceSendNum=0;
	//把缓冲里的数据全发出去，只到不能发
	SOCKSENDQUEUE::iterator prq,prn;
	CBF_PMutex pp(&info->s_mutex);
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"EpollSend开始");
	m_pSocketTcp.AttachSocket(info->s_hSocket,inet_ntoa(info->s_pSocketAddr.sin_addr));

	if (info->s_nSendQueueFlag == 1 || info->s_nSendQueueFlag == 0)
	{
		for (prn=info->s_pSendQueue.begin() ; prn!=info->s_pSendQueue.end() ; )
		{
			prq = prn;
			prn++;
			//一些判断
			if (prq->s_nIndex != info->s_nIndex || prq->s_nTimestamp != info->s_nTimestamp )
			{
				prq->s_nRet = -1;
				prq->s_nSendNum++;
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"来自[%s]的连接index[%d] 标识[%d] socket[%d] 发送队列里数据时间戳不符,移出",\
					inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,prq->s_nHook,info->s_hSocket);
				m_pAioWork->OnSendBack(prq,false);//返回给应用，由应用释放缓冲等
				info->s_pSendQueue.erase(prq);//从队列删除
				continue;
			}
			//发送数据
			//针对ET模式要修改，只到发送完
			
			while (prq->s_nSendLen < prq->s_nTotalLen )
			{
				ret = m_pSocketTcp.Send(prq->s_sSendBuffer+prq->s_nSendLen,prq->s_nTotalLen-prq->s_nSendLen);
				if (ret <= 0)
				{
					m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"来自[%s]的连接index[%d] 标识[%d] socket[%d] 发送失败[%d] 在队列里继续发",\
						inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,prq->s_nHook,info->s_hSocket,ret);
					//发送部分数据置标志
					if (prq->s_nSendLen > 0 )
					{
						info->s_nSendQueueFlag = 1;
					}
					else
					{
						info->s_nSendQueueFlag = 0;
					}
					return ;
				}
				else if (ret == (prq->s_nTotalLen-prq->s_nSendLen) ) //发完了
				{
					m_pAioRes->g_nSendByteLen +=ret;
					m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"来自[%s]的连接index[%d] 标识[%d] socket[%d] 从队列发送数据完成 发送数据长度[%d]",\
						inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,prq->s_nHook,info->s_hSocket,ret);
					m_pAioWork->OnSendBack(prq,true);//返回给应用，由应用释放缓冲等
					info->s_pSendQueue.erase(prq);//从队列删除
					//发送完整数据库置状态为优先发送优先队列数据
					info->s_nSendQueueFlag = 0;
				
					break;
					
				}
				prq->s_nSendLen +=ret;
				m_pAioRes->g_nSendByteLen +=ret;

				
			}
			onceSendNum++;
			if (onceSendNum >= m_pAioRes->g_nSendOnceNum)
			{
				return;
			}
		}  //end for 发送
	}
	
	int  delaytime;
	//开始发送行情队列，只有上面的队列发送完才进行
	for (prn=info->s_pQuoteSendQueue.begin() ; prn!=info->s_pQuoteSendQueue.end() ; )
	{
		prq = prn;
		prn++;
		//发送数据
		delaytime = time(NULL) - prq->s_nTime ;


		if (prq->s_nSendLen == 0)  //是完整的没有发送过的
		{
			//行情在队列里超过配置的秒超时则丢弃
			if ( delaytime > m_pAioRes->g_nQuoteQueueDeleteTime )
			{
				m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"来自[%s]的连接index[%d] 标识[%d] socket[%d] 发送行情队列数据时间过期 [%d > %d]秒,移出",\
					inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,prq->s_nHook,info->s_hSocket,delaytime,m_pAioRes->g_nQuoteQueueDeleteTime);
				m_pAioWork->OnSendBack(prq,false);//返回给应用，由应用释放缓冲等
				info->s_pQuoteSendQueue.erase(prq);//从队列删除
				continue;	
			}
		}
		
		//针对ET模式要修改，只到发送完
		
		while (prq->s_nSendLen < prq->s_nTotalLen )
		{
			ret = m_pSocketTcp.Send(prq->s_sSendBuffer+prq->s_nSendLen,prq->s_nTotalLen-prq->s_nSendLen);
			if (ret <= 0)
			{
				m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"来自[%s]的连接index[%d] 标识[%d] socket[%d] 发送行情失败[%d] 在队列里继续发",\
					inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,prq->s_nHook,info->s_hSocket,ret);
				//				struct epoll_event ev;
				// 				ev.data.ptr = info;
				// 				ev.events = EPOLLPRI|EPOLLERR|EPOLLIN|EPOLLOUT|EPOLLHUP;//要检查是否可写，当可写时再发
				// 				epoll_ctl(m_pAioRes->g_nEpoll, EPOLL_CTL_MOD, info->s_hSocket, &ev);
				//发送部分数据置标志
				if (prq->s_nSendLen > 0 )
				{
					info->s_nSendQueueFlag = 2;
				}
				else
				{
					info->s_nSendQueueFlag = 0;
				}
				return ;
			}
			else if (ret == (prq->s_nTotalLen-prq->s_nSendLen) ) //发完了
			{
				m_pAioRes->g_nSendByteLen +=ret;
				m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"来自[%s]的连接index[%d] 标识[%d] socket[%d] 从队列发送行情数据完成 发送数据长度[%d]",\
					inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,prq->s_nHook,info->s_hSocket,ret);
				m_pAioWork->OnSendBack(prq,true);//返回给应用，由应用释放缓冲等
				info->s_pQuoteSendQueue.erase(prq);//从队列删除
				//发送完整数据置状态为优先发送优先队列数据
				info->s_nSendQueueFlag = 0;
				
				break;
				
			}
			prq->s_nSendLen +=ret;
			m_pAioRes->g_nSendByteLen +=ret;
		}
		if (++onceSendNum >= m_pAioRes->g_nSendOnceNum)
		{
			return;
		}
	}  //end for 发送

	//全部发送完了，可以不用检查写
	struct epoll_event ev;
	ev.data.ptr = info;
	ev.events = EPOLLPRI|EPOLLERR|EPOLLIN|EPOLLHUP;//要检查是否可写，当可写时再发
	ret = epoll_ctl(m_pAioRes->g_nEpoll, EPOLL_CTL_MOD, info->s_hSocket, &ev);
	if (ret != 0)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"来自[%s]的连接index[%d]socket[%d] EPOLL_CTL_MOD 失败[%d]",\
			inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,info->s_hSocket,ret);
	}

#endif
	return;

}

void CBF_AIO_Thread::OnClose(PSOCKET_POOL_DATA info, const char *filename, int fileline, const char *msg)
{
	m_pLog->LogMp(LOG_PROMPT,filename,fileline,"关闭连接 index[%d]  来自[%s]  socket[%d] %s",\
		info->s_nIndex,inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_hSocket,msg);
	m_pAioWork->OnClose(info,msg);
}

void CBF_AIO_Thread::OnRecvBack(PSOCKET_POOL_DATA data)
{
	m_pAioWork->OnRecvBack(data);
}

void CBF_AIO_Thread::OnPing(PSOCKET_POOL_DATA data)
{
	m_pAioWork->OnPing(data);
}

// void CBF_AIO_Thread::OnSendBack(SOCKET_SEND_DATA data,bool bret)
// {
// 	m_pAioWork->OnSendBack(data,bret);
// }

void CBF_AIO_Thread::OnAccept(PSOCKET_POOL_DATA data)
{
	data->s_cHBFlag = HEARTBEAT_RECV;
	data->s_nReadTime = time(NULL);
	data->s_nWriteTime = data->s_nReadTime;
	data->s_nHBCloseTime = m_pAioRes->g_nDisconnectTime;
	data->s_nHBTime = m_pAioRes->g_nHeartRun;
	data->s_nTimestamp = data->s_nReadTime;
	data->s_cSocketStatus  = SOCKSTATUS_R;
	data->s_cSocketType = SOCK_CLIENT;

	//20151119 当iocp连接accept后置标志为读
	data->s_cOperType = OP_READ;

	m_pAioWork->OnAccept(data);
}

void CBF_AIO_Thread::OnConnectBack(PSOCKET_POOL_DATA data, bool bret)
{
	if (bret)
	{
		m_pAioWork->OnConnectBack(data,0);
	}
	else
	{
		m_pAioWork->OnConnectBack(data,1);
	}
	
}

void CBF_AIO_Thread::AddSocketIndex(int index)
{
	m_lIndexList.Add(index);
}

void CBF_AIO_Thread::ListIndex()
{
	int ret;
	printf("id[%d]   indexlist:",GetThreadID());
	bool isdata = m_lIndexList.First(ret);
	while (isdata)
	{
		printf("%d ",ret);
		isdata = m_lIndexList.Next(ret);
	}
	printf("END \n");
}

void CBF_AIO_Thread::ScanPool()
{
	//查看连接
	int nSize;
	int nUse;
	m_pAioPool->PoolInfo(nSize,nUse);
	PSOCKET_POOL_DATA data ;
	for (int i=1; i<nSize ;i++)
	{
		data = m_pAioPool->GetDataByIndex(i);
		if (data->s_hSocket != INVALID_SOCKET && (data->s_cSocketType != SOCK_LISTEN && data->s_cSocketType != OP_ACCEPT)) 
		{
			if (data->s_cHBFlag == HEARTBEAT_RECV)
			{
				if ((time(NULL) - data->s_nReadTime) > data->s_nHBCloseTime)
				{
					m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"来自[%s]的连接index[%d]socket[%d]连接长时间未用，断开 最后读时间[%d] 关闭时间[%d]",\
						inet_ntoa(data->s_pSocketAddr.sin_addr),data->s_nIndex,data->s_hSocket,data->s_nReadTime,data->s_nHBCloseTime);
					OnClose(data,__FILE__,__LINE__,"连接长时间未用，断开");
					continue;
				}
				else if ((time(NULL) - data->s_nReadTime) > data->s_nHBTime)
				{
					m_pAioWork->OnPing(data);
				}
			}
			else if (data->s_cHBFlag == HEARTBEAT_SEND)
			{
				if ((time(NULL) - data->s_nWriteTime) > data->s_nHBCloseTime)
				{
					m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"来自[%s]的连接index[%d]socket[%d]连接长时间未用，断开 最后写时间[%d] 关闭时间[%d]",\
						inet_ntoa(data->s_pSocketAddr.sin_addr),data->s_nIndex,data->s_hSocket,data->s_nWriteTime,data->s_nHBCloseTime);
					OnClose(data,__FILE__,__LINE__,"连接长时间未用，断开");
					continue;
				}
				else if ((time(NULL) - data->s_nWriteTime) > data->s_nHBTime)
				{
					m_pAioWork->OnPing(data);
				}
			}
		}
	}
}

int CBF_AIO_Thread::OnTimer(unsigned int eventid, void *p)
{
	CBF_AIO_Thread *pp = (CBF_AIO_Thread *)p;
	pp->SetScan();
	return 0;
}

void CBF_AIO_Thread::SetScan()
{
	m_bIsScan = true;
}
