// BF_AIO.cpp: implementation of the CBF_AIO class.
//
//////////////////////////////////////////////////////////////////////

#include "BF_AIO.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBF_AIO::CBF_AIO()
{
	m_pAioWork = NULL;
	m_pAioRes=NULL;
	bzero(m_sErrMsg,sizeof(m_sErrMsg));
	m_pLog = NULL;
}

CBF_AIO::~CBF_AIO()
{
	m_pAioWork=NULL;
	m_pAioRes=NULL;
}
void CBF_AIO::SetParameter(CBF_AIO_WorkBase *workcallback,CBF_AIO_Resource *res)
{
	m_pAioWork = workcallback;
	m_pAioRes = res;
	m_pAioWork->SetParameter(this,&m_pAioPool);

	m_pAioPool.SetMaxNum(m_pAioRes->g_nMaxConnectNum);
	//当最大连接数大于2000时，先分配2000个连接
	if (m_pAioRes->g_nMaxConnectNum >2000)
	{
		//当连接数超时2000且小于g_nMaxConnectNum时，由m_pAioPool继续分配
		m_pAioPool.Init(2000);
	}
	else
	{
		//小于2000则直接分配到最大
		m_pAioPool.Init(m_pAioRes->g_nMaxConnectNum);
	}
	m_pLog = &(m_pAioRes->g_pLog);
}



bool CBF_AIO::Start()
{
	int ret;
	if (m_pLog == NULL)
	{
		return false;
	}
	m_pAioRes->g_bIsExit = false;
	if (!m_pSocketTcp.Create(AF_INET,false))
	{
		sprintf(m_sErrMsg,"侦听端口Create失败");
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		m_pAioRes->g_bIsExit = true;
		return false;
	}
	if (!m_pSocketTcp.Listen(m_pAioRes->g_nPort))
	{
		sprintf(m_sErrMsg,"侦听端口%d Listen失败",m_pAioRes->g_nPort);
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		m_pAioRes->g_bIsExit = true;
		return false;
	}


	PSOCKET_POOL_DATA info = m_pAioPool.PoolMalloc();
	if (info == NULL)
	{
		sprintf(m_sErrMsg,"分配连接空间失败");
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		m_pAioRes->g_bIsExit = true;
		return false;
	}
	info->s_hSocket = m_pSocketTcp.GetSocket();
	info->s_cOperType = OP_LISTEN;
	info->s_cSocketType = SOCK_LISTEN;
	info->s_cSocketStatus = SOCKSTATUS_R; //只用读就行了
	//将侦听的连接索引放入全局，供各线程使用
	m_pAioRes->g_nListenIndex = info->s_nIndex;
	
	m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"侦听 socket[%d] index[%d]",info->s_hSocket,info->s_nIndex);

	if (m_pAioRes->g_nRunMode == MODEIOCP )
	{
#if defined(_WINDOWS)
		//IOCP模式
		m_pAioRes->g_hIocp = ::CreateIoCompletionPort( INVALID_HANDLE_VALUE, 0, 0, 0 );
		if (m_pAioRes->g_hIocp == NULL)
		{
			sprintf(m_sErrMsg,"创建完成端口失败");
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			m_pAioRes->g_bIsExit = true;
			return false;
		}
		CreateIoCompletionPort((HANDLE)info->s_hSocket, m_pAioRes->g_hIocp, NULL, 0 );

		PSOCKET_POOL_DATA infotmp=NULL;
		//发送多个AcceptEx请求,但不收数据，每个IO线程一个acceptex
		CBF_SocketTcp ptcp;
		for (int k=0 ; k<m_pAioRes->g_nIoThreadNum ; k++)
		{
			ptcp.Create(AF_INET,false);//非阻塞
			//分配一个新的空的连接信息
			infotmp = m_pAioPool.PoolMalloc();
			if (infotmp == NULL)
			{
				sprintf(m_sErrMsg,"分配连接空间失败");
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
				m_pAioRes->g_bIsExit = true;
				return false;
			}
			infotmp->s_hSocket = ptcp.GetSocket();

			infotmp->s_cOperType = OP_ACCEPT;
			infotmp->s_cSocketType = SOCK_ACCEPT;
			infotmp->s_cSocketStatus = SOCKSTATUS_R; //只用读就行了
			infotmp->s_hSocket = ptcp.GetSocket();
			DWORD dwBytes;
			m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"index=%d 开始AcceptEx ",infotmp->s_nIndex);

			bool bret = AcceptEx(info->s_hSocket,infotmp->s_hSocket,infotmp->s_pIoBuffer,0,sizeof(sockaddr_in)+16, sizeof(sockaddr_in)+16,&dwBytes, (LPOVERLAPPED)infotmp);
			if (!bret) 
			{
				int iocperrno = WSAGetLastError();
				if  (iocperrno != ERROR_IO_PENDING)
				{
					sprintf(m_sErrMsg,"AcceptEx failed with error: %u\n", iocperrno);
					m_pAioPool.CloseFree(info);
					m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
					m_pAioRes->g_bIsExit = true;
					return false;
				}
			}
			ptcp.AttachSocket(INVALID_SOCKET,"");
		}
#else
		sprintf(m_sErrMsg,"IOCP只能在windows下运行\n");
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		m_pAioRes->g_bIsExit = true;
		return false;
#endif

	}
	else if (m_pAioRes->g_nRunMode == MODEEPOLL )
	{
#if defined(LINUX)
		//EPOLL模式
		
		m_pAioRes->g_nEpoll = epoll_create(m_pAioRes->g_nMaxConnectNum);
		if (m_pAioRes->g_nEpoll <0 )
		{
			sprintf(m_sErrMsg,"epoll_create失败");
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			m_pAioRes->g_bIsExit = true;
			return false;
		}
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"创建epoll handle[%d]",m_pAioRes->g_nEpoll);
		//加入到EPOLL
		struct epoll_event ev;
		ev.events = EPOLLPRI|EPOLLERR|EPOLLIN|EPOLLOUT|EPOLLHUP;
		ev.data.ptr = info;
// 	    EPOLLIN ：表示对应的文件描述符可以读；
// 		EPOLLOUT：表示对应的文件描述符可以写；
// 		EPOLLPRI： 表示对应的文件描述符有紧急的数可读；
// 		EPOLLERR：表示对应的文件描述符发生错误；
// 		EPOLLHUP：表示对应的文件描述符被挂断；
//      EPOLLET：    ET的epoll工作模式；
		
        if (epoll_ctl(m_pAioRes->g_nEpoll, EPOLL_CTL_ADD, ((PSOCKET_POOL_DATA)(ev.data.ptr))->s_hSocket, &ev)<0)
		{
			sprintf(m_sErrMsg,"将侦听端口加入epoll失败 [%d]",((PSOCKET_POOL_DATA)(ev.data.ptr))->s_hSocket);
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			m_pAioRes->g_bIsExit = true;
			return false;
		}
        m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"加入 epoll socket:[%d]",info->s_hSocket);
#else
		sprintf(m_sErrMsg,"EPOLL只能在linux下运行\n");
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		m_pAioRes->g_bIsExit = true;
		return false;
#endif
	}
	else if (m_pAioRes->g_nRunMode == MODESELECT )
	{
		int id;
		//在每个io线程里，自动根据线程ID平均分配连接进行select处理
		m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"总共%d个select线程，将%d连接平均分配 ",m_pAioRes->g_nIoThreadNum,m_pAioRes->g_nMaxConnectNum);
		for (int j=0;j < m_pAioRes->g_nMaxConnectNum ; j++)
		{
			id = j%m_pAioRes->g_nIoThreadNum;
			m_pAioThread[id].AddSocketIndex(j);
		}
	}
	else
	{
		m_pAioRes->g_bIsExit = true;
		m_pSocketTcp.Close();
		sprintf(m_sErrMsg,"运行模式不符 %d\n",m_pAioRes->g_nRunMode);
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	int i;
    //IO工作线程的初始
	for (i=0 ; i< m_pAioRes->g_nIoThreadNum ; i++)
	{
		m_pAioThread[i].SetParameter(&m_pAioPool,m_pAioWork,m_pAioRes,i+1,m_pLog);
//		m_pAioThread[i].ListIndex();
		m_pAioThread[i].CreateThread();//启动IO线程
	}

	//启动定时器

 	m_pAioThread[0].m_pTimer.Init(100);
 	m_pAioThread[0].m_pTimer.SetTimer(0,5000,&CBF_AIO::OnTimer,this); //每隔5秒检查一次
 	m_pAioThread[0].m_pTimer.Start();
	return true;
}

bool CBF_AIO::Stop()
{
	m_pAioRes->g_bIsExit = true;
	m_pSocketTcp.Close();
 	//m_pAioThread[0].m_pTimer.KillTimer(0);
	m_pAioThread[0].m_pTimer.Stop();
	m_pAioThread[0].m_pTimer.Join();

 	m_pTimer.Stop();
	int i;
	if (m_pAioRes->g_nRunMode == MODEIOCP)
	{
#if defined(_WINDOWS)
		for (i=0; i<m_pAioRes->g_nIoThreadNum ; i++)  //通知IO线程停止
		{
			PostQueuedCompletionStatus( m_pAioRes->g_hIocp, 0, 1, 0 );
		}
#endif
	}
	for (i=0 ; i< m_pAioRes->g_nIoThreadNum ; i++)
	{
		m_pAioThread[i].Join();
	}
	m_pTimer.Join();
	return true;
}

void CBF_AIO::ScanPool()
{
	//查看连接
	int nSize;
	int nUse;
	m_pAioPool.PoolInfo(nSize,nUse);
	PSOCKET_POOL_DATA data ;
	m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"开始扫描连接池");
	for (int i=1; i<nSize ;i++)
	{
		data = m_pAioPool.GetDataByIndex(i);
		if (data->s_hSocket != INVALID_SOCKET && (data->s_cSocketType != SOCK_LISTEN && data->s_cSocketType != OP_ACCEPT)) 
		{
			//iocp不用检查acceptex的连接情况，它在等客户连接上来
			if (m_pAioRes->g_nRunMode == MODEIOCP )
			{
				if (data->s_cOperType == OP_ACCEPT)
				{
					continue;
				}
			}
			
			if (data->s_cHBFlag == HEARTBEAT_RECV)
			{
				if ((time(NULL) - data->s_nReadTime) > data->s_nHBCloseTime)
				{
					m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"来自[%s]的连接index[%d]socket[%d]连接长时间未用，断开 rtime[%d] closetime[%d]",\
						inet_ntoa(data->s_pSocketAddr.sin_addr),data->s_nIndex,data->s_hSocket,data->s_nReadTime,data->s_nHBCloseTime);
					m_pAioWork->OnClose(data,"连接长时间未用，断开");
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
					m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"来自[%s]的连接index[%d]socket[%d]连接长时间未用，断开 wtime[%d] closetime[%d]",\
						inet_ntoa(data->s_pSocketAddr.sin_addr),data->s_nIndex,data->s_hSocket,data->s_nWriteTime,data->s_nHBCloseTime);
					m_pAioWork->OnClose(data,"连接长时间未用，断开");
					continue;
				}
				else if ((time(NULL) - data->s_nWriteTime) > data->s_nHBTime)
				{
					m_pAioWork->OnPing(data);
				}
			}
		}
	}
	m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"扫描连接池完成");
}

bool CBF_AIO::Send(SOCKET_SEND_DATA senddata,bool isimmediate)
{
	int ret=0;
	senddata.s_nSendLen = 0;
	PSOCKET_POOL_DATA info = m_pAioPool.GetDataByIndex(senddata.s_nIndex);
	if (info == NULL || info->s_hSocket == INVALID_SOCKET)
	{
		senddata.s_nRet = -1;
		sprintf(m_sErrMsg,"发送失败，连接已关闭或指定的连接索引[%d]不存在 标识[%d]",senddata.s_nIndex,senddata.s_nHook);
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	CBF_PMutex pp(&info->s_mutex);
	if (senddata.s_nTimestamp != 0)
	{
		if (info->s_nTimestamp != senddata.s_nTimestamp)
		{
			senddata.s_nRet = -1;
			sprintf(m_sErrMsg,"发送失败，连接时间戳不符 连接索引[%d] 标识[%d]",senddata.s_nIndex,senddata.s_nHook);
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
	}
	if (info->s_hSocket == INVALID_SOCKET)
	{
		senddata.s_nRet = -1;
		sprintf(m_sErrMsg,"发送失败，连接已关闭 连接索引[%d] 标识[%d]",senddata.s_nIndex,senddata.s_nHook);
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	
	senddata.s_nTime = time(NULL);
	CBF_SocketTcp pTcpSocket;
	if (isimmediate) //立即发送，登录验证等
	{
		pTcpSocket.AttachSocket(info->s_hSocket,inet_ntoa(info->s_pSocketAddr.sin_addr));
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"开始发送数据 %d",senddata.s_nTotalLen);
		ret = pTcpSocket.Send(senddata.s_sSendBuffer,senddata.s_nTotalLen);
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"发送Send 返回 %d",ret);
		if (ret == senddata.s_nTotalLen)
		{
			//监控数据
//			m_pAioRes->g_nSendByteLen +=ret;
			senddata.s_nRet = ret;
			info->s_nWriteTime = time(NULL);
//			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"发送完成 index[%d] 标识[%d] length[%d]",info->s_nIndex,senddata.s_nHook,ret);
			return true;
		}
		else
		{
			senddata.s_nRet = ret;
			if (ret >0)
			{
				senddata.s_nSendLen +=ret;	
			}
			if (info->s_pSendQueue.size() > 0)
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"怎可能，发送连接验证消息时队列里已有数据 index[%d] 标识[%d] length[%d]",info->s_nIndex,senddata.s_nHook,ret);
				return false;
			}
			else //放入队列，暂缓发磅
			{
				senddata.s_nTime = time(NULL);
				info->s_pSendQueue.push_back(senddata);
				m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"放入队列，下次再发 index[%d] 标识[%d] 总线流水[%d] 交易码[%d] 本次发送length[%d] 剩余长度[%d] 交易队列数[%d] 行情队列数[%d]",\
					info->s_nIndex,senddata.s_nHook,senddata.s_serial,senddata.s_nTxCode,ret,senddata.s_nTotalLen-senddata.s_nSendLen,info->s_pSendQueue.size(),info->s_pQuoteSendQueue.size());
				//根据不同的模型进行处理
				if (m_pAioRes->g_nRunMode == MODESELECT ) //select模型，socket是读写都要检查
				{
					info->s_cSocketStatus = SOCKSTATUS_RW;
				}
				else if (m_pAioRes->g_nRunMode == MODEEPOLL)
				{
#if defined(LINUX)
					//要查看状态
					struct epoll_event ev;
					ev.data.ptr = info;
					m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"将index[%d] socket[%d] g_nEpoll[%d] 加入epoll 可写",info->s_nIndex,info->s_hSocket,m_pAioRes->g_nEpoll);
					ev.events = EPOLLPRI|EPOLLERR|EPOLLIN|EPOLLHUP|EPOLLOUT;//要检查是否可写，当可写时再发
					int ret = epoll_ctl(m_pAioRes->g_nEpoll, EPOLL_CTL_MOD,info->s_hSocket, &ev);
					if (ret != 0)
					{
						m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"epoll_ctl ret[%d] errno[%d] 失败",ret,errno);
					
					}
// 					else
// 					{
// 						m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"epoll_ctl ret[%d] 成功",ret);
// 					}
					
#endif
				}
				else 
				{
#if defined(_WINDOWS)
					
					//发送一个发数据的请求
					///此处有错，注意指针不符
					info->s_cOperType = OP_WRITE;
					info->s_pSendData = &(senddata);
					WSABUF buf;
					buf.buf = senddata.s_sSendBuffer+senddata.s_nSendLen;
					buf.len = senddata.s_nTotalLen - senddata.s_nSendLen;
					DWORD dwSend = 0; 
					DWORD dwFlags = 0;
					ret = WSASend(info->s_hSocket,&buf,1,&dwSend,dwFlags,(LPOVERLAPPED)info,NULL);
#endif 
				}
			}
		}
		return true;
	}
	
	//从总线收过来的数据
	

	int delaytime=0;
	int deletenum=0;//删除条数
	int delaynum=0;//过期条数
	//要判断队列是否有数据
	if (info->s_pSendQueue.size() > 0 || info->s_pQuoteSendQueue.size() >0 )
	{
		senddata.s_nTime = time(NULL);
		if (senddata.s_nTimestamp != 0)
		{
			info->s_pSendQueue.push_back(senddata);
			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"放入队列，下次再发 index[%d] 标识[%d] 总线流水[%d] 交易码[%d] 本次发送length[%d] 剩余长度[%d]  交易队列大小[%d] 行情队列大小[%d]",\
				info->s_nIndex,senddata.s_nHook,senddata.s_serial,senddata.s_nTxCode,0,senddata.s_nTotalLen-senddata.s_nSendLen,info->s_pSendQueue.size(),info->s_pQuoteSendQueue.size());
		}
		else
		{
			info->s_pQuoteSendQueue.push_back(senddata);
			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"放入行情队列，下次再发 index[%d] 标识[%d] 总线流水[%d] 交易码[%d] 本次发送length[%d] 剩余长度[%d]，交易队列大小[%d] 行情队列大小[%d]",\
				info->s_nIndex,senddata.s_nHook,senddata.s_serial,senddata.s_nTxCode,0,senddata.s_nTotalLen-senddata.s_nSendLen,info->s_pSendQueue.size(),info->s_pQuoteSendQueue.size());
			//移除过期的行情数据
			SOCKSENDQUEUE::iterator prq,prn;
			bool bMoved = false;
			int  quoteNum = info->s_pQuoteSendQueue.size();
			for (prn=info->s_pQuoteSendQueue.begin() ; prn!=info->s_pQuoteSendQueue.end() ; )
			{
				prq = prn;
				prn++;
				if (info->s_nSendQueueFlag == 2 && bMoved==false)
				{
					quoteNum--;
					bMoved = true;
					continue;//行情发送部分，看后面的数据是否过期
				}
				if (quoteNum > m_pAioRes->g_nQuoteQueueNum )
				{
					//移除行情
					info->s_pQuoteSendQueue.erase(prq);//从队列删除
					quoteNum--;
					deletenum++;
					continue;
				}
				//队列数小于保留数，查看过期时间
				delaytime = time(NULL) - prq->s_nTime ;
				//行情在队列里超过配置的秒超时则丢弃
				if ( delaytime > m_pAioRes->g_nQuoteQueueDeleteTime )
				{
					delaynum++;
					info->s_pQuoteSendQueue.erase(prq);//从队列删除
					continue;	
				}
			}
			if (delaynum >0 || deletenum >0)
			{
				m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"来自[%s]的连接index[%d] 标识[%d] socket[%d] 发送行情队列  移除数据数目[%d] 过期数目[%d] 过期时间[%d]  交易队列大小[%d] 行情队列大小[%d] ",\
					inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,prq->s_nHook,\
					info->s_hSocket,deletenum,delaynum,m_pAioRes->g_nQuoteQueueDeleteTime,\
					info->s_pSendQueue.size(),info->s_pQuoteSendQueue.size());
			}
			
		}
		//根据不同的模型进行处理
		if (m_pAioRes->g_nRunMode == MODESELECT ) //select模型，socket是读写都要检查
		{
			info->s_cSocketStatus = SOCKSTATUS_RW;
		}
		else if (m_pAioRes->g_nRunMode == MODEEPOLL)
		{
#if defined(LINUX)
			//要查看状态
			struct epoll_event ev;
			ev.data.ptr = info;
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"将index[%d] socket[%d] g_nEpoll[%d] 加入epoll 可写",info->s_nIndex,info->s_hSocket,m_pAioRes->g_nEpoll);
			ev.events = EPOLLPRI|EPOLLERR|EPOLLIN|EPOLLOUT|EPOLLHUP;//要检查是否可写，当可写时再发
			int ret = epoll_ctl(m_pAioRes->g_nEpoll, EPOLL_CTL_MOD, info->s_hSocket, &ev);
			if (ret != 0)
			{
				m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"epoll_ctl ret[%d] errno[%d] 失败",ret,errno);
		
			}
// 			else
// 			{
// 				m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"epoll_ctl ret[%d] 成功",ret);
// 			}
#endif
		}
		else 
		{
#if defined(_WINDOWS)
			//发送一个发数据的请求
			//此处指针有错，注意修改

			info->s_cOperType = OP_WRITE;
			info->s_pSendData = &senddata;
			WSABUF buf;
			buf.buf = senddata.s_sSendBuffer+senddata.s_nSendLen;
			buf.len = senddata.s_nTotalLen - senddata.s_nSendLen;
			DWORD dwSend = 0; 
			DWORD dwFlags = 0;
			ret = WSASend(info->s_hSocket,&buf,1,&dwSend,dwFlags,(LPOVERLAPPED)info,NULL);
#endif 
		}
	}
	else  //直接发送
	{
		info->s_nSendQueueFlag = 0;
//		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"开始发送  连接索引[%d] 标识[%d]",senddata.s_nIndex,senddata.s_nHook);
		pTcpSocket.AttachSocket(info->s_hSocket,inet_ntoa(info->s_pSocketAddr.sin_addr));
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"开始发送数据 长度[%d]  socket[%d] ip[%s]",senddata.s_nTotalLen,info->s_hSocket,inet_ntoa(info->s_pSocketAddr.sin_addr));
		ret = pTcpSocket.Send(senddata.s_sSendBuffer,senddata.s_nTotalLen);
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"发送Send 返回 %d",ret);
		if (ret == senddata.s_nTotalLen)
		{
			//监控数据
//			m_pAioRes->g_nSendByteLen +=ret;
			senddata.s_nRet = ret;
			info->s_nWriteTime = time(NULL);
//			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"发送完成 index[%d] 标识[%d] length[%d]",info->s_nIndex,senddata.s_nHook,ret);
			return true;
		}
		else
		{
			senddata.s_nRet = ret;
			if(ret > 0)
			{
				senddata.s_nSendLen +=ret;
				info->s_nWriteTime = time(NULL);
			}
			senddata.s_nTime = time(NULL);
			if (senddata.s_nTimestamp != 0)
			{				
				info->s_pSendQueue.push_back(senddata);
				m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"放入队列，下次再发 index[%d] 标识[%d] 总线流水[%d] 交易码[%d] 本次发送length[%d] 剩余长度[%d] 交易队列大小[%d] 行情队列大小[%d]",\
					info->s_nIndex,senddata.s_nHook,senddata.s_serial,senddata.s_nTxCode,ret,\
					senddata.s_nTotalLen-senddata.s_nSendLen,info->s_pSendQueue.size(),\
					info->s_pQuoteSendQueue.size());
				if (senddata.s_nSendLen>0)
				{
					info->s_nSendQueueFlag = 1;//发送部分数据
				}
			}
			else
			{
				info->s_pQuoteSendQueue.push_back(senddata);
				m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"放入行情队列，下次再发 index[%d] 标识[%d] 总线流水[%d] 交易码[%d] 本次发送length[%d] 剩余长度[%d] 交易队列大小[%d] 行情队列大小[%d]",\
					info->s_nIndex,senddata.s_nHook,senddata.s_serial,senddata.s_nTxCode,ret,senddata.s_nTotalLen-senddata.s_nSendLen,info->s_pSendQueue.size(),info->s_pQuoteSendQueue.size());
				if (senddata.s_nSendLen>0)
				{
					info->s_nSendQueueFlag = 2;//行情发送部分数据
				}
			}
			
			//根据不同的模型进行处理
			if (m_pAioRes->g_nRunMode == MODESELECT ) //select模型，socket是读写都要检查
			{
				info->s_cSocketStatus = SOCKSTATUS_RW;
			}
			else if (m_pAioRes->g_nRunMode == MODEEPOLL)
			{
#if defined(LINUX)
				//要查看状态
				struct epoll_event ev;
				ev.data.ptr = info;
				m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"将index[%d] socket[%d] g_nEpoll[%d] 加入epoll 可写",info->s_nIndex,info->s_hSocket,m_pAioRes->g_nEpoll);
				ev.events = EPOLLPRI|EPOLLERR|EPOLLIN|EPOLLOUT|EPOLLHUP;//要检查是否可写，当可写时再发
//				ev.events = EPOLLOUT;//要检查是否可写，当可写时再发
				int ret = epoll_ctl(m_pAioRes->g_nEpoll, EPOLL_CTL_MOD,info->s_hSocket, &ev);
				if (ret != 0)
				{
					m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"epoll_ctl ret[%d] errno[%d] 失败",ret,errno);

				}
// 				else
// 				{
// 					m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"epoll_ctl ret[%d] 成功",ret);
// 				}
#endif
			}
			else 
			{
#if defined(_WINDOWS)
				
				//发送一个发数据的请求
				//此处有错，注意修改
				info->s_cOperType = OP_WRITE;
				info->s_pSendData = &senddata;
				WSABUF buf;
				buf.buf = senddata.s_sSendBuffer+senddata.s_nSendLen;
				buf.len = senddata.s_nTotalLen - senddata.s_nSendLen;
				DWORD dwSend = 0; 
				DWORD dwFlags = 0;
				ret = WSASend(info->s_hSocket,&buf,1,&dwSend,dwFlags,(LPOVERLAPPED)info,NULL);
#endif 
			}
		}
	}
	return true;
}

bool CBF_AIO::AddAio(PSOCKET_POOL_DATA info)
{
	if (m_pAioRes->g_nRunMode == MODESELECT ) //select模型，socket是读写都要检查
	{
		//SELECT模型的标志由应用来置
//		info->s_cSocketStatus = SOCKSTATUS_R;
	}
	else if (m_pAioRes->g_nRunMode == MODEEPOLL )
	{
#if defined(LINUX)
		struct epoll_event ev;
		ev.data.ptr = info;
		ev.events = EPOLLPRI|EPOLLERR|EPOLLIN|EPOLLHUP;
        int ret = epoll_ctl(m_pAioRes->g_nEpoll, EPOLL_CTL_ADD, info->s_hSocket, &ev); 
		if (ret != 0)
		{
			m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"epoll_ctl add ret[%d] errno[%d] 失败",ret,errno);
			
		}
		else
		{
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"将连接 index[%d] socket[%d]加入epoll",info->s_nIndex,info->s_hSocket);
		}
		
#else
		return false;
#endif
	}
	else
	{
#if defined(_WINDOWS)
		int ret;
		CreateIoCompletionPort((HANDLE)info->s_hSocket, m_pAioRes->g_hIocp, NULL, 0 );
		//发送一个接收数据的请求
		info->s_cOperType = OP_READ;
		WSABUF buf;
		buf.buf = info->s_pIoBuffer;
		buf.len = info->s_nIoBufferLen;
		DWORD dwRecv = 0; 
        DWORD dwFlags = 0;
		WSARecv(info->s_hSocket,&buf,1,&dwRecv,&dwFlags,(LPOVERLAPPED)info,NULL);
#else
		return false;
#endif 
	}

	return true;
}
bool CBF_AIO::Close(PSOCKET_POOL_DATA info)
{
	int ret;
	if (info == NULL)
	{
		return false;
	}
//	CBF_SocketTcp pTcpSocket;
	CBF_PMutex pp(&info->s_mutex);
	if (m_pAioRes->g_nRunMode == MODESELECT ) 
	{
		m_pAioPool.CloseFree(info);
	}
	else if (m_pAioRes->g_nRunMode == MODEEPOLL )
	{
#if defined(LINUX)
		struct epoll_event ev;
		ev.data.ptr = info;
        ret = epoll_ctl(m_pAioRes->g_nEpoll, EPOLL_CTL_DEL, info->s_hSocket , &ev); 
		if (ret != 0)
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"epoll_ctl del ret[%d] errno[%d] socket[%d] index[%d] 失败",\
				ret,errno,info->s_hSocket,info->s_nIndex);
		}
		m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"关闭连接 index[%d]",info->s_nIndex);
		m_pAioPool.CloseFree(info);
		
#endif
	}
	else
	{
#if defined(_WINDOWS)
		m_pAioPool.CloseFree(info);
#endif 
	}
	return true;
}
bool CBF_AIO::Connect(char *ip,short port,unsigned  short  sockettype, unsigned int destindex ,unsigned int &index)
{
	int ret;
	CBF_SocketTcp pSocketTcp;
	if (!pSocketTcp.Create(AF_INET,false))
	{
		sprintf(m_sErrMsg,"Create失败");
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	if (m_pAioRes->g_nRunMode == MODEIOCP )
	{
#ifdef _WINDOWS
		SOCKADDR_IN temp;
		temp.sin_family = AF_INET;
		temp.sin_port = htons(0);
		temp.sin_addr.s_addr = htonl(ADDR_ANY);
		ret = bind(pSocketTcp.GetSocket(), (struct sockaddr*)&temp, sizeof(temp));
		if(ret<0) 
		{
			m_nSocketErrno = GET_LAST_SOCK_ERROR();
			return false;
		}
#endif
	}

	bool bret  = pSocketTcp.ConnectServer(ip,port,0);
	
	PSOCKET_POOL_DATA info = m_pAioPool.PoolMalloc();
	if (info == NULL)
	{
		sprintf(m_sErrMsg,"分配连接空间失败");
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	unsigned int inaddr;
	memset(&(info->s_pSocketAddr), 0, sizeof(info->s_pSocketAddr));
	info->s_pSocketAddr.sin_family = AF_INET;
	inaddr=inet_addr(ip);

#if defined(_WINDOWS)
	info->s_pSocketAddr.sin_addr.S_un.S_addr = inaddr;
#endif
		
#if defined(AIX) || defined(LINUX) || defined(SOLARIS) || defined(HP_UX) 
	info->s_pSocketAddr.sin_addr.s_addr = inaddr;
#endif		
	info->s_pSocketAddr.sin_port = htons(port);
	
	info->s_hSocket = pSocketTcp.GetSocket();
	info->s_cOperType   = OP_CONNECT;
	info->s_cSocketType = sockettype;
	info->s_nDestIndex = destindex;
	index = info->s_nIndex;
	if (bret)
	{
		//连接成功
		info->s_cSocketStatus = SOCKSTATUS_C;//连接的socket
	}
	//未成功要检查
	info->s_cSocketStatus = SOCKSTATUS_C;//连接的socket

	return bret;
}


std::string CBF_AIO::GetErrMsg()
{
	return m_sErrMsg;
}
int  CBF_AIO::GetSocketErrNo()
{
	return m_nSocketErrno;
}

int CBF_AIO::OnTimer(unsigned int eventid, void *p)
{
	CBF_AIO *pp = (CBF_AIO *)p;
	pp->ScanPool();
	return 0;
}
