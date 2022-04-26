// PoolModule.cpp: implementation of the CPoolModule class.
//
//////////////////////////////////////////////////////////////////////

#include "PoolModule.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern char g_VERSION[20];




CPoolModule::CPoolModule()
{
	m_sThreadName = "CPoolModule";
	m_bIsInit = false;
	m_nEnd = 0;
	m_nDrebHeadLen = sizeof(DREB_HEAD);
	m_nBegin=0;
	m_nEnd=1;
	m_pRes=NULL;
	m_pSocketMgr=NULL;
	m_pMemPool=NULL;
	m_pMsgProc = NULL;
	m_bIsPingTimer= false;
	m_bIsRouteTimer=false;
	m_bIsServiceTimer=false;

}

CPoolModule::~CPoolModule()
{
	m_pRes=NULL;
	m_pSocketMgr=NULL;
	m_pMemPool=NULL;
}
void CPoolModule::ExitThreadInstance()
{
	return ;
}
bool CPoolModule::InitThreadInstance()
{
	return true;
}
int CPoolModule::OnTimer(unsigned int eventid, void *p)
{
	CPoolModule *pp = (CPoolModule *)p;
	if (eventid == 0)
	{
		//节点路由
		pp->RouteTimer();
	}
	else if (eventid == 1 )
	{
		//服务路由
		pp->ServiceRouteTimer();
	}
	else if (eventid == 2)
	{
		pp->PingTimer();
	}

	return 0;
}
bool CPoolModule::SetGlobalVar(CGlobalVar *pVar, CSocketMgr *sockmgr, CMemDB *pMemDb, CBF_BufferPool *pool,CPoolData *msgqueue)
{
	m_pRes=pVar;
	m_log = &(m_pRes->g_pLog);
	//m_pMemDb=pMemDb;
	m_pSocketMgr=sockmgr;
	m_pMemPool=pool;
	m_pMsgQueue = msgqueue;
	

	m_pTimer.Init(100,true);
	//节点路由定时器
	m_pTimer.SetTimer(0,m_pRes->g_nDrebRouteTime*1000,&CPoolModule::OnTimer,this); //设置定时器 
	//服务路由定时器
	m_pTimer.SetTimer(1,m_pRes->g_nServiceRouteTime*1000,&CPoolModule::OnTimer,this);
	//PING定时器
	m_pTimer.SetTimer(2,BEATHEARTTICK*1000,&CPoolModule::OnTimer,this);

	return true;
}
int CPoolModule::Run()
{
	struct timeval tv;
	int result;
	int maxfd=0;
	int lasttime=time(NULL);
	int lasttime2=time(NULL);
	int curtime;
	m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"启动接收线程 %d",m_pRes->g_bIsExit);
	while (!m_pRes->g_bIsExit)
	{
		FD_ZERO(&m_rset);
		FD_ZERO(&m_wset);
#if defined(_WINDOWS)
		FD_ZERO(&m_eset);
#endif
		tv.tv_sec = 0;
		tv.tv_usec = 50;
		//取得最大的socket并将socket加入到fd_set集合
		GetMaxfdAddSelect(maxfd);
		if (maxfd  <=0)
		{
			SLEEP(10);
			continue;
		}
#if defined(_WINDOWS)
		result = select(maxfd+1, &m_rset, &m_wset, &m_eset, &tv);
#else
		result = select(maxfd+1, &m_rset, &m_wset, NULL, &tv);
#endif

		if (result != 0) 
		{
			//有事件发生，处理
			OnEvent();
		}
		if (m_bIsPingTimer)
		{
			OnNoEvent();
			OnMonitor();
			m_bIsPingTimer = false;

		}
		if (m_bIsRouteTimer)
		{
//			m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"主动发送ROUTE");
			OnRoute();
			m_bIsRouteTimer = false;
		}
		if (m_bIsServiceTimer)
		{
			if (m_pRes->g_nRegOtherDreb == 1) //注册
			{
//				m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"主动发送SERVICE ROUTE");
				OnServiceRoute();
			}
			m_bIsServiceTimer = false;
		}

	}

	m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"停止接收线程 %d",m_pRes->g_bIsExit);
	return 0;
}

int CPoolModule::GetMaxfdAddSelect(int &maxfd)
{
	int ret=0;
	maxfd = 0;
	for (int i=m_nBegin;i<m_nEnd;i++)
	{
		if (!m_pSocketMgr->at(i)->IsCanSelect())
		{
			ret++;
		}
		else
		{
			//需要读数据
			if (m_pSocketMgr->at(i)->NeedRead())
			{
				FD_SET(m_pSocketMgr->at(i)->m_sock, &m_rset);//加入到读select集合
			}
			//需要发送数据或主动连接时
			if (m_pSocketMgr->at(i)->NeedSend() || m_pSocketMgr->at(i)->NeedConnect())
			{
				FD_SET(m_pSocketMgr->at(i)->m_sock, &m_wset);//加入到写select集合
			}
#if defined(_WINDOWS)
			if (m_pSocketMgr->at(i)->NeedConnect())
			{
				FD_SET(m_pSocketMgr->at(i)->m_sock, &m_eset);//加入到连接异常集合
			}
#endif
			//得到最大的socket 
			if (m_pSocketMgr->at(i)->m_sock > maxfd)
			{
				maxfd=m_pSocketMgr->at(i)->m_sock; 
			}
		}
	}
	return ret;
}

void CPoolModule::OnNoEvent()
{
	int j;
	for (j=1; j<m_nEnd; j++)
	{
		if (m_pRes->g_bIsExit)
		{
			return;
		}
		if (m_pRes->g_nBeatHeart == 1) //心跳检测，针对服务和其它节点连过来的，是否主动发心跳检测
		{
			//连接正常需要读的情况
			if (m_pSocketMgr->at(j)->m_sock != INVALID_SOCKET && m_pSocketMgr->at(j)->m_bChecked && m_pSocketMgr->at(j)->NeedRead())
			{
				//服务和其它节点连过来
				if (m_pSocketMgr->at(j)->m_nType == SOCK_DREB || (m_pSocketMgr->at(j)->m_nType == SOCK_SRV  ))
				{
					//没有读到数据且时间超过心跳间隔
					if (time(NULL)-m_pSocketMgr->at(j)->m_nReadtime >= BEATHEARTTICK*(m_pSocketMgr->at(j)->m_nBeatheartcount+1)) //多长时间没有读到数据就发送ping或关闭
					{
						//发送心跳次数没有超过指定次数继续发心跳
						if (m_pSocketMgr->at(j)->m_nBeatheartcount < (m_pRes->g_nHeartCount-1))
						{
							m_pSocketMgr->at(j)->m_nBeatheartcount++;
							OnPing(j);//发送心跳
						}
						else
						{
							OnClose(j,"无心跳",__FILE__,__LINE__);
						}
					}
					else   //是否没有发送数据
					{
						//若在指定时间内没有发送数据就发送心跳
						if (time(NULL)-m_pSocketMgr->at(j)->m_nSendtime >= BEATHEARTTICK) 
						{
							OnPing(j);
						}
					}
				}  //end if (m_pSocketMgr->at(j)->m_nType == SOCK_DREB || (m_pSocketMgr->at(j)->m_nType == SOCK_SRV  ))
			}  // end if (m_pSocketMgr->at(j)->m_sock != INVALID_SOCKET && m_pSocketMgr->at(j)->m_bChecked && m_pSocketMgr->at(j)->m_bNeedread)
		}
		//若连接类型为数据总线节点互连
		if (m_pSocketMgr->at(j)->m_nType == SOCK_TODREB  && m_pSocketMgr->at(j)->m_sock != INVALID_SOCKET)
		{
			//连接没有检查通过
			if (!m_pSocketMgr->at(j)->m_bChecked) 
			{
				if (time(NULL)-m_pSocketMgr->at(j)->m_nReadtime >= m_pRes->g_nHeartCount*BEATHEARTTICK)
				{
					if (m_pSocketMgr->at(j)->m_s_center.bNeedConnect)
					{
						m_log->LogMp(2,__FILE__,__LINE__,"尝试连接到%s:%d未成功 index=%d",m_pSocketMgr->at(j)->m_s_center.sAddress,m_pSocketMgr->at(j)->m_s_center.nPort,j);
						closesocket(m_pSocketMgr->at(j)->m_sock);
						m_pSocketMgr->at(j)->m_sock = INVALID_SOCKET;
					}
					else  //不是SOCK_TODREB且要主动连接的情况
					{
						OnClose(j,"协商超时",__FILE__,__LINE__);
					}
				}
			}
		}
		if (m_pSocketMgr->at(j)->m_sock != INVALID_SOCKET &&  !m_pSocketMgr->at(j)->m_bChecked)
		{
			if (time(NULL)-m_pSocketMgr->at(j)->m_nReadtime >= m_pRes->g_nHeartCount*BEATHEARTTICK)
			{
				OnClose(j,"连接非法",__FILE__,__LINE__);
			} 
		}
		if (m_pSocketMgr->at(j)->m_sock != INVALID_SOCKET &&  m_pSocketMgr->at(j)->m_nType == SOCK_CLI)
		{
			if (time(NULL)-m_pSocketMgr->at(j)->m_nReadtime >= m_pRes->g_nDisconnectTime)
			{
				OnClose(j,"连接长时间未用，超时断开",__FILE__,__LINE__);
			} 
		}
		//若是数据总线节点连接，且没有连上则去连接
		if (m_pSocketMgr->at(j)->m_nType == SOCK_TODREB )
		{
			if (m_pSocketMgr->at(j)->m_sock == INVALID_SOCKET)
			{
				OnConnect(j);
			}
		}
	}
	
}

void CPoolModule::OnEvent()
{
	int ret;
	int i;
	if (m_nBegin == 0)
	{
		ret = FD_ISSET(m_pSocketMgr->at(0)->m_sock,&m_rset);
		if (ret != 0) //如查是侦听端口有读的事件，则去调用accept
		{
			OnAccept(0);
		}
	}
	for (i= 1; i<m_nEnd; i++)
    {
		if (m_pRes->g_bIsExit)
		{
			return;
		}
		if (m_pSocketMgr->at(i)->m_sock == INVALID_SOCKET)//未连接
		{
			if (m_pSocketMgr->at(i)->m_nType == SOCK_TODREB   ) //若是数据总线节点连接，则去连接
			{
				OnConnect(i);
			}
			continue;
		}
		else
		{
			//响应读事件
			OnReadEvent(i);
			if (m_pSocketMgr->at(i)->m_sock != INVALID_SOCKET)//未连接
			{
				//响应写事件
				OnWriteEvent(i);
			}
		}
	}
}



void CPoolModule::OnReadEvent(int conindex)
{
	int ret;
	ret = FD_ISSET(m_pSocketMgr->at(conindex)->m_sock,&m_rset);
	if (ret != 0) //有读事件
	{
		OnRecv(conindex);
	}
}

void CPoolModule::OnWriteEvent(int conindex)
{
	int ret;
	if (m_pSocketMgr->at(conindex)->m_sock == INVALID_SOCKET)
	{
		return;
	}
#if defined(_WINDOWS)
	ret = FD_ISSET(m_pSocketMgr->at(conindex)->m_sock, &m_eset);
	if (ret != 0)
	{
		//主动连接其它的DREB
		if (m_pSocketMgr->at(conindex)->m_nType == SOCK_TODREB)
		{
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"尝试连接到%s:%d未成功 index=%d",m_pSocketMgr->at(conindex)->m_s_center.sAddress,m_pSocketMgr->at(conindex)->m_s_center.nPort,conindex);
			closesocket(m_pSocketMgr->at(conindex)->m_sock);
			m_pSocketMgr->at(conindex)->m_sock = INVALID_SOCKET;
		}
		else 
		{
			OnClose(conindex,"Other connect fail",__FILE__,__LINE__);
		}
	}
	else if (FD_ISSET(m_pSocketMgr->at(conindex)->m_sock, &m_wset) !=0 )
	{
		if (m_pSocketMgr->at(conindex)->NeedConnect())
		{
			if (m_pSocketMgr->at(conindex)->m_nType == SOCK_TODREB ) 
			{
				OnConnected(conindex);
			}
		}
		else if (m_pSocketMgr->at(conindex)->m_bChecked)
		{
			OnSend(conindex);//发送数据
		}
	}
	else  if (m_pSocketMgr->at(conindex)->m_bChecked)//socket无写事件
	{
//		OnSend(conindex);//发送数据
	}
#else
	
	int ok;
	int oklen;
	if (FD_ISSET(m_pSocketMgr->at(conindex)->m_sock, &m_wset) !=0)
	{
		if (m_pSocketMgr->at(conindex)->NeedConnect())
		{
			oklen=sizeof(ok);
#if defined(HP_UX)
			getsockopt(m_pSocketMgr->at(conindex)->m_sock,SOL_SOCKET,SO_ERROR,(char *)&ok,&oklen);
#else	
			getsockopt(m_pSocketMgr->at(conindex)->m_sock,SOL_SOCKET,SO_ERROR,(char *)&ok,(socklen_t *)&oklen);
#endif
			if (ok == 0)
			{
				if (m_pSocketMgr->at(conindex)->m_nType == SOCK_TODREB)
				{
					OnConnected(conindex);
				}
			}
			else
			{
				if (m_pSocketMgr->at(conindex)->m_nType == SOCK_TODREB)
				{
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"尝试连接到%s:%d未成功 index=%d",m_pSocketMgr->at(conindex)->m_s_center.sAddress,m_pSocketMgr->at(conindex)->m_s_center.nPort,conindex);
					closesocket(m_pSocketMgr->at(conindex)->m_sock);
					m_pSocketMgr->at(conindex)->m_sock = INVALID_SOCKET;
				}
			} // end if (ok == 0)
		}
		else if (m_pSocketMgr->at(conindex)->m_bChecked)
		{
			OnSend(conindex);//发送数据
		} // end if (g_connInfoList[conindex]->m_bNeedconnect)
	}
	else  if (m_pSocketMgr->at(conindex)->m_bChecked)
	{
//		OnSend(conindex);//发送数据
	}
#endif

				
}

void CPoolModule::OnPing(int conindex)
{
	if (!m_pSocketMgr->at(conindex)->m_bChecked)//连接未确认，不用发心跳
	{
		return;
	}
// 	if (time(NULL) - m_pSocketMgr->at(conindex)->m_nPingTime < BEATHEARTTICK) //小于5秒不用发ping
// 	{
// 		return ;
// 	}

	m_pSocketMgr->at(conindex)->m_nPingTime = time(NULL);

	S_DREB_RSMSG *recvmessage = (S_DREB_RSMSG *)m_pMemPool->PoolMalloc();
	if (recvmessage == NULL)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"分配内存空间出错");
		return ;
	}
	bzero(&(recvmessage->message.head), sizeof(DREB_HEAD));
	recvmessage->msghead.msgtype=MSG_PING;
	recvmessage->msghead.index=conindex;
	recvmessage->msghead.connecttime=m_pSocketMgr->at(conindex)->m_nConntime;
	recvmessage->msghead.socket=m_pSocketMgr->at(conindex)->m_sock;
	recvmessage->msghead.sockettype = m_pSocketMgr->at(conindex)->m_nType;
	recvmessage->msghead.nTotalLen = 0;
	//发送ping数据包
	
	
	recvmessage->message.head.cRaflag = 0;
	recvmessage->message.head.cZip = 0;
	recvmessage->message.head.nLen = 0;
	recvmessage->message.head.cCmd = CMD_PING;
	recvmessage->message.head.s_Sinfo.s_nIndex = conindex;
	recvmessage->message.head.s_Sinfo.s_cNodePrivateId = m_pRes->g_nDrebPrivateId;	
	recvmessage->message.head.s_Sinfo.s_nNodeId = m_pRes->g_nDrebId;
	recvmessage->message.head.s_Sinfo.s_nTimestamp = recvmessage->msghead.connecttime;
	

	//20140515 如果多线程处理，此处SendMsg里应该要加互斥
	if (m_pSocketMgr->at(conindex)->SendMsg(recvmessage) <0 )
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送CMD_PING出错 %d",conindex);
	}
}

void CPoolModule::OnClose(int conindex,const char *msg,const char *filename,int fileline)
{
	if (m_pSocketMgr->at(conindex)->m_sock == INVALID_SOCKET)
	{
		return;
	}
	m_log->LogMp(LOG_INFO,filename,fileline,"关闭连接%s:%d index=%d type=%d %s",\
		m_pSocketMgr->at(conindex)->m_sAddress,m_pSocketMgr->at(conindex)->m_sock,\
		conindex,m_pSocketMgr->at(conindex)->m_nType,msg);
	if (m_pSocketMgr->at(conindex)->m_nType == SOCK_CLI || m_pSocketMgr->at(conindex)->m_nType== SOCK_SLOT \
		|| m_pSocketMgr->at(conindex)->m_nType == SOCK_UNKNOW)
	{
		//20140515 如果多线程处理，此处OnClose里应该要加互斥，防止发送线程正在发送
		m_pSocketMgr->at(conindex)->OnClose(msg,filename,fileline);
		return;
	}
	S_DREB_RSMSG *recvmessage = (S_DREB_RSMSG *)m_pMemPool->PoolMalloc();
	if (recvmessage == NULL)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"分配内存空间出错");
		return ;
	}
	bzero(&(recvmessage->message.head), sizeof(DREB_HEAD));
	recvmessage->msghead.index=conindex;
	recvmessage->msghead.msgtype=MSG_CLOSE;
	recvmessage->msghead.socket=m_pSocketMgr->at(conindex)->m_sock;
	recvmessage->msghead.connecttime=m_pSocketMgr->at(conindex)->m_nConntime;
	recvmessage->msghead.sockettype = m_pSocketMgr->at(conindex)->m_nType;
	recvmessage->msghead.nTotalLen = 0;
	
	if (recvmessage->msghead.sockettype == SOCK_TODREB)
	{
		recvmessage->msghead.closeinfo.d_nNodeId = m_pSocketMgr->at(conindex)->m_s_center.nNodeId;
		recvmessage->msghead.closeinfo.d_cNodePrivateId = m_pSocketMgr->at(conindex)->m_s_center.cNodePrivateId;
	}
	else if (recvmessage->msghead.sockettype == SOCK_DREB)
	{
		recvmessage->msghead.closeinfo.d_nNodeId = m_pSocketMgr->at(conindex)->m_s_conn.nNodeId;
		recvmessage->msghead.closeinfo.d_cNodePrivateId = m_pSocketMgr->at(conindex)->m_s_conn.cNodePrivateId;
	}
	else if (recvmessage->msghead.sockettype == SOCK_SRV)
	{
		recvmessage->msghead.closeinfo.d_cSvrPrivateId = m_pSocketMgr->at(conindex)->m_s_srv.cSvrPrivateId;
		recvmessage->msghead.closeinfo.d_nSvrMainId = m_pSocketMgr->at(conindex)->m_s_srv.nSvrMainId;
	}
	//通知消息处理线程，有些要处理
	OnMsgProc(recvmessage);
}
void CPoolModule::OnConnected(int conindex)
{
	//主动和dreb的连接
	if (m_pSocketMgr->at(conindex)->m_sock != INVALID_SOCKET) //连接成功
	{
		//可以发送数据了
		m_pSocketMgr->at(conindex)->m_bNeedread = true;
		m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"建立到%s:%d的连接成功 index=%d",m_pSocketMgr->at(conindex)->m_s_center.sAddress,m_pSocketMgr->at(conindex)->m_s_center.nPort,conindex);
		//连接成功后发送CMD_CONNECT消息，确认连接的类型
		m_pSocketMgr->at(conindex)->m_s_center.bNeedConnect = false;
		m_pSocketMgr->at(conindex)->m_nPingTime = time(NULL);
		//发送连接消息
		S_DREB_RSMSG *recvmessage = (S_DREB_RSMSG *)m_pMemPool->PoolMalloc();
		if (recvmessage == NULL)
		{
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"分配内存空间出错");
			return ;
		}
		bzero(&(recvmessage->message.head), sizeof(DREB_HEAD));
		recvmessage->msghead.msgtype = MSG_CONNECT;
		recvmessage->msghead.index=conindex;
		recvmessage->msghead.connecttime= m_pSocketMgr->at(conindex)->m_nConntime;
		recvmessage->msghead.socket=m_pSocketMgr->at(conindex)->m_sock;
		recvmessage->msghead.sockettype = m_pSocketMgr->at(conindex)->m_nType;
		
		if (m_pSocketMgr->at(conindex)->m_pSendQueue == NULL)
		{
			m_pSocketMgr->at(conindex)->m_pSendQueue = new CSendQueue();
			if (m_pSocketMgr->at(conindex)->m_pSendQueue == NULL)
			{
				m_pSocketMgr->at(conindex)->OnClose("分配发送队列空间出错",__FILE__,__LINE__);
				m_pMemPool->PoolFree(recvmessage);
				return;
			}
		}

		recvmessage->message.head.cRaflag = 0;
		recvmessage->message.head.cZip = 0;
		recvmessage->message.head.cCmd = CMD_CONNECT;
		recvmessage->message.head.cDrebAffirm = 0;
		recvmessage->message.head.s_Sinfo.s_nIndex = conindex;
		recvmessage->message.head.s_Sinfo.s_cNodePrivateId = m_pRes->g_nDrebPrivateId;	
		recvmessage->message.head.s_Sinfo.s_nNodeId = m_pRes->g_nDrebId;
		recvmessage->message.head.s_Sinfo.s_nTimestamp = recvmessage->msghead.connecttime;
		
		recvmessage->message.head.nLen = sizeof(S_CONNECT_DATA);
		S_CONNECT_DATA *data = (S_CONNECT_DATA *)recvmessage->message.buffer;
		data->cConnectType = 2;
		data->cNodePrivateId = m_pRes->g_nDrebPrivateId;
		data->nNodeId = m_pRes->g_nDrebId;
		data->nBandWidth = m_pSocketMgr->at(conindex)->m_s_center.nbandwidth;
		sprintf(data->sVersion,"%s",g_VERSION);
		
		m_pDrebEndian.Endian2Comm((unsigned char *)&(data->nNodeId),sizeof(data->nNodeId));
		m_pDrebEndian.Endian2Comm((unsigned char *)&(data->nBandWidth),sizeof(data->nBandWidth));
		
		//20140515 刚连接上，可以直接发，不用互斥也可以
		if (m_pSocketMgr->at(conindex)->SendMsg(recvmessage,true)< 0 )
		{
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送CMD_CONNECT出错 %d",conindex);
		}
		return;
	}
	m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"怎么可能，连接成功但SOCKET非法");
	return;
}

void CPoolModule::OnConnect(int conindex)
{
	//去连接
	if (time(NULL) - m_pSocketMgr->at(conindex)->m_nConntime < 5 ) //在10秒内不用重连
	{
		return;
	}
	int ret;
	m_pSocketMgr->at(conindex)->m_nConntime = time(NULL);
	m_tcpSocket.Create(AF_INET,false);//异步阻塞方式
	m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"尝试连接到数据总线节点%s:%d:%d index=%d",m_pSocketMgr->at(conindex)->m_s_center.sAddress,m_pSocketMgr->at(conindex)->m_s_center.nPort,m_pSocketMgr->at(conindex)->m_sock,conindex);
	if (m_tcpSocket.ConnectServer(m_pSocketMgr->at(conindex)->m_s_center.sAddress,m_pSocketMgr->at(conindex)->m_s_center.nPort))
	{
		
		m_pSocketMgr->at(conindex)->m_nReadtime = time(NULL);
		m_pSocketMgr->at(conindex)->m_sock = m_tcpSocket.GetSocket();
	}
	else
	{
		if ((m_tcpSocket.GetErrorNo() == EWOULDBLOCK) || (m_tcpSocket.GetErrorNo() == EINPROGRESS))
		{
			m_pSocketMgr->at(conindex)->m_nReadtime = time(NULL);
			m_pSocketMgr->at(conindex)->m_sock = m_tcpSocket.GetSocket();
			m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"等待建立到%s:%d的连接 index=%d",m_pSocketMgr->at(conindex)->m_s_center.sAddress,m_pSocketMgr->at(conindex)->m_s_center.nPort,conindex);
			
			return;
		}
		else
		{
			m_pSocketMgr->at(conindex)->m_sock = INVALID_SOCKET;
			m_tcpSocket.Close();
		}
	}
	return;

}

void CPoolModule::OnSend(int conindex)
{
	//20140515 如果多线程处理，此处OnSend里应该要加互斥，防止发送线程正在发送
	m_pSocketMgr->at(conindex)->OnSend();
}

void CPoolModule::OnAccept(int conindex)
{
	char address[32];
	SOCKET_HANDLE tsock=INVALID_SOCKET;
	int i;
	bool havefree=false;
	m_tcpSocket.AttachSocket(m_pSocketMgr->at(conindex)->m_sock,"127.0.0.1");
	tsock= m_tcpSocket.Accept(address,false);
	if (tsock != INVALID_SOCKET)
	{
		for (i=1; i<m_pRes->g_nMaxConnCount; i++)
		{
			//不是数据总线节点的连接且连接未使用
			if ((m_pSocketMgr->at(i)->m_sock == INVALID_SOCKET) && (m_pSocketMgr->at(i)->m_nType != SOCK_TODREB))
			{
				havefree = true;
				//设置连接的参数
				m_pSocketMgr->at(i)->SetAccept(tsock,address);//可读但未检查
				break;
			}
		}
		if (havefree)
		{
			m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"接收从%s的连接socket[%d] index[%d]",address,tsock,i);
		}
		else
		{
			m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"达到最大连接管理，从%s的连接socket[%d]被拒绝",address,tsock);
			closesocket(tsock);
		}
	}
}

void CPoolModule::OnRecv(int conindex)
{
	int ret=0;
	char tmpchar[200];
	//有读事件才进入到这里
	ret = m_pSocketMgr->at(conindex)->OnRecv();	
	if (ret <=0 )
	{
		//肯定是连接关闭了
		sprintf(tmpchar,"%s 远端关闭，接收数据不完整 ret=[%d]",m_pSocketMgr->at(conindex)->m_sAddress,ret);
		OnClose(conindex,tmpchar,__FILE__,__LINE__);
		return;
	}
	//有接收到数据心跳次数置为0
	m_pSocketMgr->at(conindex)->m_nBeatheartcount=0;
	//收到数据了，循环把收到的数据处理完
	while(m_pSocketMgr->at(conindex)->m_nRcvBufLen >= DREBHEADLEN)
	{
		S_DREB_RSMSG *recvmessage = (S_DREB_RSMSG *) m_pMemPool->PoolMalloc();
		if (recvmessage == NULL)
		{
			m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"分配消息空间时出错，内存不足");
			return ;
		}
		ret = m_pSocketMgr->at(conindex)->GetRecvData(recvmessage);
		if (ret < 0) 
		{
			m_pMemPool->PoolFree(recvmessage);
			OnClose(conindex,"CRC校验错",__FILE__,__LINE__);
//			m_pRes->LogTime();
			return;
		}
		else if (ret == 0) //数据未接收完整
		{
			m_pMemPool->PoolFree(recvmessage);
			m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"数据未接收完整 缓冲数据长度[%d]",m_pSocketMgr->at(conindex)->m_nRcvBufLen);
//			m_pRes->LogTime();
			return;
		}
		m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"开始处理数据 缓冲数据长度[%d]",m_pSocketMgr->at(conindex)->m_nRcvBufLen);
		if (recvmessage->message.head.cRaflag == 1 && (recvmessage->message.head.cCmd == CMD_DPABC || recvmessage->message.head.cCmd == CMD_DPPUSH))
		{
			m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"CMD_DPABC或CMD_DPPUSH的应答,丢弃");
			m_pMemPool->PoolFree(recvmessage);
//			m_pRes->LogTime();
			continue;
		}
		if (recvmessage->message.head.cRaflag == 1)
		{
			//一些应答，如cmd_connect等，不用给消息处理线程了
			// #define  CMD_ROUTER      1  //数据总线节点发送路由通知的命令字
			// #define  CMD_DEL_NODE    2  //数据总线节点之间取消数据总线节点的命令字,只发送至主动连接的数据总线节点
			// #define  CMD_DEL_SVR     3  //服务端取消注册服务的命令字
			// #define  CMD_PING        4  //心跳请求的命令字
			// #define  CMD_CONNECT     5  //连接注册
			// #define  CMD_SERVICE     6  //服务路由
			// #define  CMD_REGSERVICE  7  //注册服务
			if ( recvmessage->message.head.cCmd == CMD_ROUTER || recvmessage->message.head.cCmd == CMD_DEL_NODE || \
				recvmessage->message.head.cCmd == CMD_DEL_SVR || recvmessage->message.head.cCmd == CMD_PING || \
				recvmessage->message.head.cCmd == CMD_SERVICE || recvmessage->message.head.cCmd == CMD_REGSERVICE )
			{
				m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"应答无须处理[%s]",GetDrebCmdType(recvmessage->message.head.cCmd).c_str());
				m_pMemPool->PoolFree(recvmessage);
//				m_pRes->LogTime();
				continue;
			}
		}
		recvmessage->msghead.index = conindex;
		recvmessage->msghead.connecttime = m_pSocketMgr->at(conindex)->m_nConntime;
		recvmessage->msghead.msgtype = MSG_READ;
		recvmessage->msghead.socket = m_pSocketMgr->at(conindex)->m_sock;
		recvmessage->msghead.sockettype = m_pSocketMgr->at(conindex)->m_nType;
		m_log->LogMp(LOG_DEBUG + 1, __FILE__, __LINE__, "OnMsgProc MSG_READ cCmd[%s]", GetDrebCmdType(recvmessage->message.head.cCmd).c_str());
		OnMsgProc(recvmessage);
//		m_pRes->LogTime();
		m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"处理完一条数据 缓冲数据长度[%d]",m_pSocketMgr->at(conindex)->m_nRcvBufLen);
	}
	return ;
}


void CPoolModule::OnRoute()
{
	if (m_pRes->g_bIsExit)
	{
		return;
	}
	S_DREB_RSMSG *recvmessage = (S_DREB_RSMSG *) m_pMemPool->PoolMalloc();
	if (recvmessage == NULL)
	{
		m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"分配消息空间时出错，内存不足");
		return ;
	}
	recvmessage->msghead.msgtype=MSG_ROUTE;
	recvmessage->msghead.index=-1;
	OnMsgProc(recvmessage);
}

void CPoolModule::OnMsgProc(S_DREB_RSMSG *msg)
{
	if (m_pRes->g_bMsgProcThread)
	{
		m_pMsgQueue->PushData(msg);
	}
	else
	{
		m_pMsgProc->Run(msg);
	}
}

void CPoolModule::PrintRoute()
{
	
}

void CPoolModule::OnServiceRoute()
{
	if (m_pRes->g_bIsExit)
	{
		return;
	}
	S_DREB_RSMSG *recvmessage = (S_DREB_RSMSG *) m_pMemPool->PoolMalloc();
	if (recvmessage == NULL)
	{
		m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"分配消息空间时出错，内存不足");
		return ;
	}
	recvmessage->msghead.msgtype=MSG_SERVICE;
	recvmessage->msghead.index=-1;
	OnMsgProc(recvmessage);
}
std::string CPoolModule::GetDrebCmdType(int cmdtype)
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
void CPoolModule::LogDrebHead(int loglevel, DREB_HEAD head, const char *msg, const char *filename, int fileline)
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

void CPoolModule::RouteTimer()
{
	//通讯节点路由
	//OnRoute();
	m_bIsRouteTimer = true;
}

void CPoolModule::ServiceRouteTimer()
{
	//服务路由
	//OnServiceRoute();
	m_bIsServiceTimer = true;
}

void CPoolModule::PingTimer()
{
	//OnNoEvent();
	m_bIsPingTimer = true;
}

void CPoolModule::StartTimer()
{
	m_pTimer.Start();
}

void CPoolModule::StopTimer()
{
	m_pTimer.Stop();
}

void CPoolModule::OnMonitor()
{
	if (m_pRes->g_nUseMonitor != 1) 
	{
		return;
	}
	if (m_pRes->g_bIsExit)
	{
		return;
	}
	S_DREB_RSMSG *recvmessage = (S_DREB_RSMSG *) m_pMemPool->PoolMalloc();
	if (recvmessage == NULL)
	{
		m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"分配消息空间时出错，内存不足");
		return ;
	}
	bzero(&(recvmessage->message.head),sizeof(DREB_HEAD));
	recvmessage->msghead.msgtype=MSG_MONITOR;
	recvmessage->msghead.index=-1;
	OnMsgProc(recvmessage);
}
