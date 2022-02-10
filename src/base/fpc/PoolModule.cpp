// PoolModule.cpp: implementation of the CPoolModule class.
//
//////////////////////////////////////////////////////////////////////

#include "PoolModule.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPoolModule::CPoolModule()
{
	m_sThreadName = "CPoolModule";
	m_bIsInit = false;
	m_bIsRunning = false;
	memset(m_errMsg,0,sizeof(m_errMsg));
	memset(&m_pDataBuf,0,sizeof(S_BF_RSMSG_FPC));
	memset(&m_pDataRcv,0,sizeof(S_BF_RSMSG_FPC));
	
	m_pPara = NULL;
	m_log=NULL;
}

CPoolModule::~CPoolModule()
{
	m_pTimer.Stop();
}
bool CPoolModule::Terminate()
{
	m_bIsRunning = false;
	return true;
}
int CPoolModule::Run()
{
	struct timeval tv;
	int result;
	int i,j;
	int maxfd=0;
	int ret;
	int lasttime=time(NULL);
	while (m_bIsRunning)
	{
		FD_ZERO(&m_rset);
		FD_ZERO(&m_wset);
#if defined(_WINDOWS)
		FD_ZERO(&m_eset);
#endif
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		//取得最大的socket并将socket加入到fd_set集合
		ret = GetMaxfdAddSelect(maxfd);
		

		FD_SET(m_pSockMgr.at(0)->m_sock, &m_rset);//加入到读select集合
		if (m_pSockMgr.at(0)->m_sock > maxfd)
		{
			maxfd = m_pSockMgr.at(0)->m_sock;
		}
		FD_SET(m_pSockMgr.at(1)->m_sock, &m_rset);//加入到读select集合
		if (m_pSockMgr.at(1)->m_sock > maxfd)
		{
			maxfd = m_pSockMgr.at(1)->m_sock;
		}
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
		
		if (result != 0) //事件发生
		{
			OnEvent();
		}

		if (m_bIsPingTimer)
		{
			OnNoEvent();
			m_bIsPingTimer = false;
		}
		
		if (m_pRes.g_bToExit)
		{
			break;
		}

	}
	m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"退出接收发送线程");
	return 0;
}

bool CPoolModule::InitFront(char *confile,void *para)
{
	m_pPara = para;
	if (!m_pRes.Init(confile))
	{
		printf("前置初始化失败 %s\n",m_pRes.GetMsg().c_str());
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"前置初始化失败 %s",m_pRes.GetMsg().c_str());
		return false;
	}
	m_log = &(m_pRes.m_log);
	
	m_pPoolData.m_pRes = &m_pRes;
	//内存分配池大小及缓冲大小
	m_pMemPool.Init(100,BFBUFSIZE);
	//初始化连接池
	if (!m_pSockMgr.Init(&m_pRes,&m_pMemPool))
	{
		sprintf(m_errMsg,"初始化失败:%s",m_pSockMgr.m_errMsg);
		printf("%s \n",m_errMsg);
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
		m_bIsInit = false;
		return false;
	}
	if (!m_pSockMgr.at(0)->Listen(m_pRes.g_nPort,false,30))
	{
		sprintf(m_errMsg,"初始化失败:建立%d sokcet侦听失败",m_pRes.g_nPort);
		printf("%s \n",m_errMsg);
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
		m_bIsInit = false;
		return false;
	}
	if (!m_pSockMgr.at(1)->Listen(m_pRes.g_nBuPort,false,30))
	{
		sprintf(m_errMsg,"初始化失败:建立%d BUsokcet侦听失败",m_pRes.g_nBuPort);
		printf("%s \n",m_errMsg);
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
		m_bIsInit = false;
		return false;
	}
	if (!m_pProcessMgr.SetGlobalVar(&m_pRes,&m_pPoolData,&m_pMemPool,&m_pSockMgr,m_pPara))
	{
		sprintf(m_errMsg,"%s",m_pProcessMgr.GetErrMsg().c_str());
		printf("%s \n",m_errMsg);
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
		m_bIsInit = false;
		return false;
	}

	//初始并启动处理线程
	if (!m_pProcessMgr.InitPmgr())
	{
		sprintf(m_errMsg,"%s",m_pProcessMgr.GetErrMsg().c_str());
		printf("%s \n",m_errMsg);
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
		m_bIsInit = false;
		return false;
	}
	//启动接收发送线程
	m_bIsRunning = true;
	CreateThread();

	m_pTimer.Init(100,true);
	//节点路由定时器
	m_pTimer.SetTimer(0,5000,&CPoolModule::OnTimer,this); //设置定时器 
	m_pTimer.Start();

	SLEEP_SECONDS(2);
//	m_pProcessMgr.Monitor();//启动BU
	return true;
}

void CPoolModule::StopFront()
{
	printf("StopFront!\n");
	m_bIsRunning = false;
	m_pRes.g_bToExit = true;
	printf("开始停止处理线程!\n");
	m_pProcessMgr.Stop();
	m_pRes.m_log.StopLog();
	printf("处理线程已停止!\n");
}

int CPoolModule::GetMaxfdAddSelect(int &maxfd)
{
	int ret=0;
	maxfd = 0;

	for (int i=2;i<m_pRes.g_nMaxConnectNum+m_pRes.g_nProcessThreadNum;i++)
	{
		if (!m_pSockMgr.at(i)->IsCanSelect())
		{
			ret++;
		}
		else
		{
			if (m_pSockMgr.at(i)->m_sock != INVALID_SOCKET && m_pSockMgr.at(i)->m_bNeedread)
			{
				FD_SET(m_pSockMgr.at(i)->m_sock, &m_rset);//加入到读select集合
			}
			if (m_pSockMgr.at(i)->m_sock != INVALID_SOCKET && m_pSockMgr.at(i)->NeedSend())
			{
				FD_SET(m_pSockMgr.at(i)->m_sock, &m_wset);//加入到写select集合
			}
			if (m_pSockMgr.at(i)->m_sock > maxfd)
			{
				maxfd=m_pSockMgr.at(i)->m_sock; //得到最大的socket 
			}
		}
	}
	return ret;
}



void CPoolModule::OnNoEvent()
{
	int j;
	int tmptime;
	for (j=2;j<m_pRes.g_nMaxConnectNum+m_pRes.g_nProcessThreadNum;j++)
	{
		if (m_pSockMgr.at(j)->m_sock != INVALID_SOCKET && m_pSockMgr.at(j)->m_nType == SOCK_CLI)
		{
			tmptime = time(NULL) - m_pSockMgr.at(j)->m_nReadtime;
			if ( tmptime > m_pRes.g_nHeartRun && tmptime < m_pRes.g_nDisconnectTime)
			{
				OnPing(j);
			}
			else if (tmptime > m_pRes.g_nDisconnectTime)
			{
				OnClose(j,"连接长时间未用，断开");
			}
		}
		if (m_pSockMgr.at(j)->m_sock != INVALID_SOCKET && m_pSockMgr.at(j)->m_nType == SOCK_BU)
		{
			tmptime = time(NULL) - m_pSockMgr.at(j)->m_nReadtime;
			if (tmptime > m_pRes.g_nDisconnectTime)
			{
				OnClose(j,"连接长时间未用，断开");
			}
		}
		if (m_pRes.g_bToExit)
		{
			return;
		}
	}
}

void CPoolModule::OnEvent()
{
	int ret;
	int i;

	ret = FD_ISSET(m_pSockMgr.at(0)->m_sock,&m_rset);
	if (ret != 0) //如查是侦听端口有读的事件，则去调用accept
	{
		OnAccept(0);
	}
	ret = FD_ISSET(m_pSockMgr.at(1)->m_sock,&m_rset);
	if (ret != 0) //如查是侦听端口有读的事件，则去调用accept
	{
		OnAccept(1);
	}
	for (i= 2; i< m_pRes.g_nMaxConnectNum+m_pRes.g_nProcessThreadNum; i++)
	{
		if (m_pSockMgr.at(i)->m_sock == INVALID_SOCKET)//未连接
		{
			continue;
		}
		else
		{
			//响应读事件
			OnReadEvent(i);
			if (m_pSockMgr.at(i)->m_sock != INVALID_SOCKET)//未连接
			{
				//响应写事件
				OnWriteEvent(i);
			}
		}
		if (m_pRes.g_bToExit)
		{
			return;
		}
	}
}



void CPoolModule::OnReadEvent(int conindex)
{
	int ret;
	int tmptime;
	ret = FD_ISSET(m_pSockMgr.at(conindex)->m_sock,&m_rset);
	if (ret != 0) //有读事件
	{
		OnRecv(conindex);
	}
	else //此连接无读事件
	{
		if (m_pSockMgr.at(conindex)->m_sock != INVALID_SOCKET && m_pSockMgr.at(conindex)->m_nType == SOCK_CLI)
		{
			tmptime = time(NULL) - m_pSockMgr.at(conindex)->m_nReadtime;
			if ( tmptime > m_pRes.g_nHeartRun && tmptime < m_pRes.g_nDisconnectTime)
			{
				OnPing(conindex);
			}
			else if (tmptime > m_pRes.g_nDisconnectTime)
			{
				OnClose(conindex,"连接长时间未用，断开");
			}
		}
		if (m_pSockMgr.at(conindex)->m_sock != INVALID_SOCKET && m_pSockMgr.at(conindex)->m_nType == SOCK_BU)
		{
			tmptime = time(NULL) - m_pSockMgr.at(conindex)->m_nReadtime;
			if (tmptime > m_pRes.g_nDisconnectTime)
			{
				OnClose(conindex,"连接长时间未用，断开");
			}
		}
	}// end if (ret == 1) //有读事件
}

void CPoolModule::OnWriteEvent(int conindex)
{
	int ret;
#if defined(_WINDOWS)
	if (m_pSockMgr.at(conindex)->m_sock == INVALID_SOCKET)
	{
		return;
	}
	ret = FD_ISSET(m_pSockMgr.at(conindex)->m_sock, &m_eset);
	if (ret != 0)
	{
		OnClose(conindex,"写socket失败");
	}
	else if (FD_ISSET(m_pSockMgr.at(conindex)->m_sock, &m_wset) !=0 )
	{
		OnSend(conindex);//发送数据
	}
	else  
	{
		OnSend(conindex);//发送数据
	}
#else
	if (m_pSockMgr.at(conindex)->m_sock == INVALID_SOCKET)
	{
		return;
	}
	int ok;
	int oklen;
	if (FD_ISSET(m_pSockMgr.at(conindex)->m_sock, &m_wset) !=0)
	{
		OnSend(conindex);//发送数据
	}
	else  
	{
		OnSend(conindex);//发送数据
	}
#endif

				
}


void CPoolModule::OnPing(int conindex)
{
	if (time(NULL) - m_pSockMgr.at(conindex)->m_nPingTime < BEATHEARTTICK) //小于5秒不用发ping
	{
		return ;
	}
	m_pSockMgr.at(conindex)->m_nPingTime = time(NULL);
	if (!ResetData(conindex,&m_pDataBuf))
	{
		return;
	}
	strcpy(m_pDataBuf.msgbuf.message->buffer,"xt");
	m_pDataBuf.msgbuf.head.len = 2;
	//发送ping数据包
	m_pSockMgr.at(conindex)->SendMsg(m_pDataBuf);
	
	
}

void CPoolModule::OnClose(int conindex,const char *msg)
{
	if (m_pSockMgr.at(conindex)->m_sock == INVALID_SOCKET)
	{
		return;
	}
//	m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"关闭连接%s:%d:%d %d %s",m_pSockMgr.at(conindex)->m_sAddress,\
		m_pSockMgr.at(conindex)->m_sock,conindex,m_pSockMgr.at(conindex)->m_nType,msg);
	//将socket关闭
	m_pSockMgr.at(conindex)->OnClose(msg);
}


void CPoolModule::OnSend(int conindex)
{
	m_pSockMgr.at(conindex)->OnSend();
}

void CPoolModule::OnAccept(int conindex)
{
	char address[32];
	SOCKET_HANDLE tsock=INVALID_SOCKET;
	int i;
	bool havefree=false;
	m_tcpSocket.AttachSocket(m_pSockMgr.at(conindex)->m_sock,"");
	tsock= m_tcpSocket.Accept(address,false);
	if (tsock != INVALID_SOCKET)
	{
		if (conindex == 0) //客户端连接
		{
			for (i=2; i<m_pRes.g_nMaxConnectNum; i++)
			{
				if ( m_pSockMgr.at(i)->m_sock == INVALID_SOCKET )
				{
					havefree = true;
					m_pSockMgr.at(i)->SetAccept(tsock,address);
					m_pSockMgr.at(i)->SetClient();
					break;
				}
			}
		}
		else //BU连接
		{
			for (i=m_pRes.g_nMaxConnectNum; i<m_pRes.g_nMaxConnectNum+m_pRes.g_nProcessThreadNum; i++)
			{
				if ( m_pSockMgr.at(i)->m_sock == INVALID_SOCKET )
				{
					havefree = true;
					m_pSockMgr.at(i)->SetAccept(tsock,address);
					m_pSockMgr.at(i)->SetBuClient();
					break;
				}
			}
		}
		
		if (havefree)
		{
			if (conindex == 0) 
			{
				m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"接收从%s的连接%d:%d",address,tsock,i);
			}
			else
			{
				m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"接收从%s的BU连接%d:%d",address,tsock,i);
			}
		}
		else if (conindex == 0) //客户端连接
		{
			m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"达到最大连接管理，从%s的连接%d被拒绝",address,tsock);
			closesocket(tsock);
		}
		else
		{
			m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"BU连接已满，从%s的连接%d被拒绝",address,tsock);
			closesocket(tsock);
		}
	}
}

void CPoolModule::OnRecv(int conindex)
{
	int ret=0;
	int i=0;
	
	ret = m_pSockMgr.at(conindex)->OnRecv();	
	if (ret<=0)
	{
		OnClose(conindex,"远端关闭，接收数据不完整");
		m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"远端关闭，接收数据不完整");
		return;
	}

	//	printf("OnRecv %d\n",ret);

	do 
	{
		if (!ResetData(conindex,&m_pDataRcv))
		{
			return;
		}
		ret = m_pSockMgr.at(conindex)->GetRecvData(&m_pDataRcv);
		m_log->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"GetRecvData返回 %d",ret);
		if (ret>0)
		{
			if (m_pSockMgr.at(conindex)->m_nType == SOCK_BU) //接收FPU的数据
			{
				m_pDataRcv.msgbuf.head.buindex = conindex;
				m_log->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"BU头信息 index=%d conntime=%d msgtype=%d",conindex,\
					m_pDataRcv.msgbuf.head.connecttime,m_pDataRcv.msgbuf.head.msgtype);
				m_log->LogBin(LOG_DEBUG+2,__FILE__,__LINE__,"收到BU发来的数据",m_pDataRcv.msgbuf.message->buffer,m_pDataRcv.msgbuf.head.len);
				ProcessFpu(m_pDataRcv);
			}
			else  //接收的请求数据  通过队列交给处理线程来分发给FPU
			{
				m_log->LogBin(LOG_DEBUG+2,__FILE__,__LINE__,"收到请求数据",m_pDataRcv.msgbuf.message->buffer,m_pDataRcv.msgbuf.head.len);
				m_pDataRcv.msgbuf.head.rtime = time(NULL);
				m_pPoolData.PushData(m_pDataRcv);
			}
			
		}
		else
		{
			m_pMemPool.PoolFree(m_pDataRcv.msgbuf.message);
			m_pDataRcv.msgbuf.message = NULL;
			break;
		}
	} while (ret <=0 );

	
	return ;
}


bool CPoolModule::ResetData(int index,S_BF_RSMSG_FPC *data)
{
	memset(data,0,sizeof(S_BF_RSMSG_FPC));
	data->msgbuf.head.connecttime = m_pSockMgr.at(index)->m_nConntime;
	data->msgbuf.head.index = index;
	data->msgbuf.head.rtime = time(NULL);
	data->msgbuf.message = (COMMSTRU_FPC *)m_pMemPool.PoolMalloc();
	if (data->msgbuf.message == NULL)
	{
		m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"分配消息空间出错");
		m_pSockMgr.at(index)->OnClose("分配消息空间出错");
		return false;
	}
	return true;
}

void CPoolModule::Monitor()
{
	if (IsStoped())
	{
		printf("重启CPoolModule线程 %d\n",m_bIsRunning);
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"重启CPoolModule线程 %d",m_bIsRunning);
		m_bIsRunning = true;
		CreateThread();
	}
	m_pProcessMgr.Monitor();
	if (m_pRes.g_bIsMonitor)
	{
		int allsize=0;
		int unuse=0;
		m_pMemPool.PoolInfo(allsize,unuse);
		m_log->LogMp(LOG_PROMPT,__FILE__,__LINE__,"内存缓冲数[%d]  空闲数[%d] 队列数[%d]",allsize,unuse,m_pPoolData.GetSize());
	}
	
}

void CPoolModule::ProcessFpu(S_BF_RSMSG_FPC &rcvdata)
{
	switch (rcvdata.msgbuf.head.msgtype)
	{
	case MSG_FREE:
		DispatchBuFree(rcvdata);
		break;
	case MSG_BUANS:
		DispatchBuAns(rcvdata);
		//m_pPoolData.PushData(rcvdata);
		break;
	case MSG_BUANSFREE:
		DispatchBuAnsFree(rcvdata);
		break;
	case MSG_GETSERIAL:
		GetSerial(rcvdata);
		break;
	default:
		m_pMemPool.PoolFree(rcvdata.msgbuf.message);
		rcvdata.msgbuf.message = NULL;
		break;
	}
	return ;
}
void CPoolModule::DispatchBuAns(S_BF_RSMSG_FPC &rcvdata)
{
	if (m_pSockMgr.AffirmIndex(rcvdata.msgbuf.head.index))
	{
		if (m_pSockMgr.at(rcvdata.msgbuf.head.index)->m_sock != INVALID_SOCKET )
		{
			m_pSockMgr.at(rcvdata.msgbuf.head.index)->SendMsg(rcvdata);
			return;
		}
	}
	m_pMemPool.PoolFree(rcvdata.msgbuf.message);
	rcvdata.msgbuf.message = NULL;
	return;
}

void CPoolModule::DispatchBuFree(S_BF_RSMSG_FPC &rcvdata)
{
	if (m_pSockMgr.AffirmIndex(rcvdata.msgbuf.head.buindex))
	{
		m_pSockMgr.at(rcvdata.msgbuf.head.buindex)->m_bIsBuGetFlag = true;
		m_pSockMgr.at(rcvdata.msgbuf.head.buindex)->m_nPid = rcvdata.msgbuf.head.rtime;
	}
	
	m_pMemPool.PoolFree(rcvdata.msgbuf.message);
	rcvdata.msgbuf.message = NULL;
}

void CPoolModule::DispatchBuAnsFree(S_BF_RSMSG_FPC &rcvdata)
{
	if (m_pSockMgr.AffirmIndex(rcvdata.msgbuf.head.buindex))
	{
		m_pSockMgr.at(rcvdata.msgbuf.head.buindex)->m_bIsBuGetFlag = true;
	}
	if (m_pSockMgr.AffirmIndex(rcvdata.msgbuf.head.index))
	{
		if (m_pSockMgr.at(rcvdata.msgbuf.head.index)->m_sock != INVALID_SOCKET )
		{
			m_pSockMgr.at(rcvdata.msgbuf.head.index)->SendMsg(rcvdata);
			return;
		}
	}
	m_pMemPool.PoolFree(rcvdata.msgbuf.message);
	rcvdata.msgbuf.message = NULL;
	return;
}

void CPoolModule::GetSerial(S_BF_RSMSG_FPC &rcvdata)
{
	sprintf(rcvdata.msgbuf.message->buffer,"%ld",m_pRes.GetThreadShareSerial());
	rcvdata.msgbuf.head.len = strlen(m_pDataRcv.msgbuf.message->buffer);
	m_pSockMgr.at(rcvdata.msgbuf.head.index)->SendMsg(rcvdata);
	
}
int CPoolModule::OnTimer(unsigned int eventid, void *p)
{
	CPoolModule *pp = (CPoolModule *)p;
	if (eventid == 0)
	{
		pp->PingTimer();
	}
	return 0;
}
void CPoolModule::PingTimer()
{
	//OnNoEvent();
	m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"定时器");
	m_bIsPingTimer = true;
}
