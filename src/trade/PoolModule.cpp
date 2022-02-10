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
	m_log=NULL;
	m_pRcvQueue= NULL;
	
	m_pRes = NULL;
	m_bIsPingTimer=false;
	m_pMemPool = NULL;
}

CPoolModule::~CPoolModule()
{
	m_log=NULL;
	m_pRcvQueue= NULL;
	m_pRes = NULL;
	m_pMemPool = NULL;
	
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
#ifdef _ENGLISH_
	m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"Start listen thread");
#else
	m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"开始侦听线程");
#endif
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
	}
#ifdef _ENGLISH_
	m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"Stop listen thread");
#else
	m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"退出侦听线程");
#endif
	return 0;
}

bool CPoolModule::InitFront(CTradeResource *res,CPoolDataMsg *que,CIErrlog *log)
{
	m_pRes = res;
	m_log = log;
	m_pRcvQueue = que;

	//初始化连接池
	if (!m_pSockMgr.Init(m_pRes,m_pMemPool))
	{
#ifdef _ENGLISH_
		sprintf(m_errMsg,"init error:%244s",m_pSockMgr.m_errMsg);
#else
		sprintf(m_errMsg,"初始化失败:%244s",m_pSockMgr.m_errMsg);
#endif
		printf("%s \n",m_errMsg);
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
		m_bIsInit = false;
		return false;
	}
	if (!m_pSockMgr.at(0)->Listen(m_pRes->g_nListenPort,false,30))
	{
#ifdef _ENGLISH_
		sprintf(m_errMsg,"init error:listen socket %d faild",m_pRes->g_nListenPort);
#else
		sprintf(m_errMsg,"初始化失败:建立%d sokcet侦听失败",m_pRes->g_nListenPort);
#endif
		printf("%s \n",m_errMsg);
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
		m_bIsInit = false;
		return false;
	}
#ifdef UNIX
	if (!m_pSockMgr.at(1)->ListenIpc(m_pRes->g_nListenPort,false,30))
	{
#ifdef _ENGLISH_
		sprintf(m_errMsg,"init error:listen ipc %d faild",m_pRes->g_nListenPort);
#else
		sprintf(m_errMsg,"初始化失败:建立%d sokcet侦听失败",m_pRes->g_nListenPort);
#endif
		printf("%s \n",m_errMsg);
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
		m_bIsInit = false;
		return false;
	}
#endif
	m_pTimer.Init(100,true);
	//节点路由定时器
	m_pTimer.SetTimer(0,5000,&CPoolModule::OnTimer,this); //设置定时器 

	
	return true;
}

void CPoolModule::StopFront()
{
	m_bIsRunning = false;
	if (m_pRes != NULL)
	{
		m_pRes->g_bToExit = true;
	}
	m_pTimer.Stop();
//	printf("开始退出CPoolModule\n");
	Join();

}

int CPoolModule::GetMaxfdAddSelect(int &maxfd)
{
	int ret=0;
	maxfd = 0;

	for (int i=0;i<m_pRes->g_nMaxConnectNum;i++)
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
			if (m_pSockMgr.at(i)->NeedSend())
			{
				FD_SET(m_pSockMgr.at(i)->m_sock, &m_wset);//加入到写select集合
				//m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"连接index[%d]，检查是否可写",i);
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
	if (m_pRes == NULL)
	{
		return;
	}
	for (j=1;j<m_pRes->g_nMaxConnectNum;j++)
	{
		if (m_pRes == NULL)
		{
			return;
		}
		if (m_pSockMgr.at(j)->m_sock != INVALID_SOCKET && m_pSockMgr.at(j)->m_nType == SOCK_CLI)
		{
			tmptime = time(NULL) - m_pSockMgr.at(j)->m_nReadtime;
			if ( tmptime > m_pRes->g_nHeartRun && tmptime < m_pRes->g_nDisconnectTime)
			{
				OnPing(j);
			}
			else if (tmptime > m_pRes->g_nDisconnectTime)
			{
#ifdef _ENGLISH_
				OnClose(j,"connect unuse long time,clost it");
#else
				OnClose(j,"连接长时间未用，断开");
#endif
			}
		}
		
		if (m_pRes->g_bToExit)
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
	if (m_pSockMgr.at(1)->m_sock != INVALID_SOCKET && m_pSockMgr.at(1)->m_nType == SOCK_LISTEN)
	{
		ret = FD_ISSET(m_pSockMgr.at(1)->m_sock,&m_rset);
		if (ret != 0) //如查是侦听端口有读的事件，则去调用accept
		{
			OnAccept(1);
		}
	}
	for (i= 2; i< m_pRes->g_nMaxConnectNum; i++)
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
		if (m_pRes->g_bToExit)
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
			if ( tmptime > m_pRes->g_nHeartRun && tmptime < m_pRes->g_nDisconnectTime)
			{
				OnPing(conindex);
			}
			else if (tmptime > m_pRes->g_nDisconnectTime)
			{
#ifdef _ENGLISH_
				OnClose(conindex,"connect unuse long time,clost it");
#else
				OnClose(conindex,"连接长时间未用，断开");
#endif
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
#ifdef _ENGLISH_
		OnClose(conindex,"write socket error");
#else
		OnClose(conindex,"写socket失败");
#endif
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
	
	//这里要写心跳包
	S_TRADE_DATA data;
	if (!ResetData(conindex,&data))
	{
		return;
	}
	//前面四个字节的报文头
	strcpy(data.pData->sBuffer,"0000");
	data.nWantlen = 4;

	//发送ping数据包
	m_pSockMgr.at(conindex)->SendMsg(data,0);
	
	
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
		for (i=1; i<m_pRes->g_nMaxConnectNum; i++)
		{
			if ( m_pSockMgr.at(i)->m_sock == INVALID_SOCKET )
			{
				havefree = true;
				m_pSockMgr.at(i)->SetAccept(tsock,address);
				m_pSockMgr.at(i)->SetClient();
				break;
			}
		}
	
		if (havefree)
		{
#ifdef _ENGLISH_
			m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"Accept %s socket[%d] index[%d]",address,tsock,i);
#else
			m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"接收从%s的连接%d index[%d]",address,tsock,i);
#endif
		}
		else  //客户端连接
		{
#ifdef _ENGLISH_
			m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"at max connect [%d]，%s socket %d refuse",m_pRes->g_nMaxConnectNum,address,tsock);
#else
			m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"达到最大连接管理[%d]，从%s的连接%d被拒绝",m_pRes->g_nMaxConnectNum,address,tsock);
#endif
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
#ifdef _ENGLISH_
		OnClose(conindex,"far-end close");
		m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"far-end close");
#else
		OnClose(conindex,"远端关闭，接收数据不完整");
		m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"远端关闭，接收数据不完整");
#endif
		return;
	}

	//	printf("OnRecv %d\n",ret);
//	m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"接收数据 ret=%d",ret);
//	std::string gtpdata;
	do 
	{
		if (!ResetData(conindex,&m_pDataRcv))
		{
			return;
		}
		ret = m_pSockMgr.at(conindex)->GetRecvData(&m_pDataRcv);
		if (ret>0)
		{
#ifdef _ENGLISH_
			m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"index[%d] Recv data",conindex);
			m_log->LogBin(LOG_DEBUG,__FILE__,__LINE__,"Recv data",m_pDataRcv.pData->sBuffer,m_pDataRcv.nWantlen);
#else
			m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"index[%d] 收到请求数据",conindex);
			m_log->LogBin(LOG_DEBUG,__FILE__,__LINE__,"收到请求数据",m_pDataRcv.pData->sBuffer,m_pDataRcv.nWantlen);
#endif
			m_pDataRcv.rTime = time(NULL);
			//放入队列，待处理
			m_pRcvQueue->PushData(m_pDataRcv);

	
		}
		else
		{
			m_pMemPool->PoolFree(m_pDataRcv.pData);
			m_pDataRcv.pData = NULL;
			break;
		}
	} while (ret > 0 );

	
	return ;
}


bool CPoolModule::ResetData(int index,S_TRADE_DATA *data)
{
	data->nSendlen=0;
	data->nWantlen=0;
	data->src = 1;
	data->index = index;
	data->rTime = time(NULL);
	data->pData = (PBPCCOMMSTRU)m_pMemPool->PoolMalloc();
	if (data->pData == NULL)
	{
#ifdef _ENGLISH_
		m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"PoolMalloc error");
		m_pSockMgr.at(index)->OnClose("PoolMalloc error");
#else
		m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"分配消息空间出错");
		m_pSockMgr.at(index)->OnClose("分配消息空间出错");
#endif
		return false;
	}
	return true;
}

void CPoolModule::Monitor()
{
	if (IsStoped())
	{
#ifdef _ENGLISH_
		printf("Restart CPoolModule thread %d\n",m_bIsRunning);
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"Restart CPoolModule thread %d",m_bIsRunning);
#else
		printf("重启CPoolModule线程 %d\n",m_bIsRunning);
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"重启CPoolModule线程 %d",m_bIsRunning);
#endif
		m_bIsRunning = true;
		CreateThread();
	}
	if (m_pTimer.IsStoped())
	{
#ifdef _ENGLISH_
		printf("Restart timer thread %d\n",m_bIsRunning);
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"Restart timer thread %d",m_bIsRunning);
#else
		printf("重启定时器线程\n");
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"重启定时器线程");
#endif
		m_pTimer.Start();
	}
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
//	m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"定时器");
	m_bIsPingTimer = true;
}

bool CPoolModule::StartFront()
{
	//启动接收发送线程
	m_bIsRunning = true;
	CreateThread();
	
	
	m_pTimer.Start();
	
	SLEEP_SECONDS(2);
	return true;
}
