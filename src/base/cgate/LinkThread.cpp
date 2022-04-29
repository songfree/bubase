// LinkThread.cpp: implementation of the CLinkThread class.
//
//////////////////////////////////////////////////////////////////////

#include "LinkThread.h"
#include "dreberrcode.h"
#include "Bpc_Timer.h"

#define  DESENCRYPTKEY    "rd402wms"
#define  DESENCRYPTKEY3   "wms13711songfree"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern char g_VERSION[20];
extern CBpc_Timer      g_pBpcTime;



#ifdef _WINDOWS
#define   INT64PFM  "%I64d"
#else
#define   INT64PFM  "%lld"
#endif

#define  CHECKFLAG_UNKNOW   0   //连接未知
#define  CHECKFLAG_STEP1    1    //接接客户端发送签名及公钥
#define  CHECKFLAG_STEP2    2    //接收客户端验证及登录
#define  CHECKFLAG_NORMAL   5    //正常连接，登录后置为正常连接

CLinkThread::CLinkThread()
{
	m_bIsRunning = false;
	m_pRes = NULL;
	m_pMemPool = NULL;
	m_bIsInit = false;
	m_nBegin=0;
	m_nScanBegin = 0;
	m_nEnd=1;
	m_sThreadName = "CLinkThread";
	CBF_Tools::MakeDir("data");
	m_bIsPingTimer = false;

	bzero(&(m_pDataBuf.data->head),DREBHEADLEN);
	m_pSendData = NULL;
}

CLinkThread::~CLinkThread()
{

}

std::string CLinkThread::GetBpcMsgType(int msgtype)
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
std::string CLinkThread::GetDrebCmdType(int cmdtype)
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
bool CLinkThread::Terminate()
{
	m_bIsRunning = false;
	return true;
}

bool CLinkThread::SetGlobalVar(CGateResource *res,CPoolData *pooldata,CBF_BufferPool *mempool,CSocketMgr *sockmgr,CSendData *senddata,CAIO_Work *aio)
{
	m_pRes = res;
	m_pSockMgr = sockmgr;
	m_pMemPool = mempool;
	m_pPoolData = pooldata;
	m_pSendData = senddata;
	
	m_pAio = aio;
	m_pLog  = &(m_pRes->g_pLog);
	m_bIsInit = true;

	m_pDesZip.SetLogPara(m_pLog);

	
	m_pDrebLog = &(m_pRes->g_pDrebLog);
	

	m_pTimer.Init(100,true);
	//节点路由定时器
	m_pTimer.SetTimer(0,5000,&CLinkThread::OnTimer,this); //设置定时器 

	return true;
}
int CLinkThread::Run()
{
	struct timeval tv;
	int result;
	unsigned int i;
	int maxfd=0;
	//int lasttime=time(NULL);
	//int lasttime2=time(NULL);
	m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"启动dreb线程");
	while(!m_pRes->g_bIsExit)
	{
		FD_ZERO(&m_rset);
		FD_ZERO(&m_wset);
#if defined(_WINDOWS)
		FD_ZERO(&m_eset);
#endif
		
		m_nScanBegin = m_nBegin;
		//取得最大的socket并将socket加入到fd_set集合
		GetMaxfdAddSelect(maxfd);
		if (maxfd  <=0)
		{
			for (i=0; i<m_pRes->g_vDrebLinkInfo.size() ;i++)
			{
				OnConnect(i);
			}
			SLEEP(5);
//			m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"无dreb连接 %d",tv.tv_sec);
			continue;
		}
		tv.tv_sec = 1;
		tv.tv_usec = 0;
#if defined(_WINDOWS)
		result = select(maxfd+1, &m_rset, &m_wset, &m_eset, &tv);
#else
		result = select(maxfd+1, &m_rset, &m_wset, NULL, &tv);
#endif
		
		if (result != 0) //有事件发生,处理
		{
			OnEvent();
		}
		if (m_bIsPingTimer)
		{
//			m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"监控NOEVENT");
			OnNoEvent();
			if (m_pRes->g_nUseMonitor == 1)
			{
				OnMonitor();
			}
			
			m_bIsPingTimer = false;
		}

	}
	m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"停止dreb线程");
	return 0;
}
int CLinkThread::GetMaxfdAddSelect(int &maxfd)
{
	int ret=0;
	maxfd = 0;
	for (unsigned int i=0;i< m_pRes->g_vDrebLinkInfo.size() ;i++)
	{
		if (!m_pSockMgr->at(i)->IsCanSelect())
		{
			continue;
		}
		else
		{
			//需要读数据
			if (m_pSockMgr->at(i)->NeedRead())
			{
				FD_SET(m_pSockMgr->at(i)->m_sock, &m_rset);//加入到读select集合
			}
			//需要发送数据或主动连接时
			if (m_pSockMgr->at(i)->NeedSend() || m_pSockMgr->at(i)->NeedConnect())
			{
				FD_SET(m_pSockMgr->at(i)->m_sock, &m_wset);//加入到写select集合
			}
#if defined(_WINDOWS)
			if (m_pSockMgr->at(i)->NeedConnect())
			{
				FD_SET(m_pSockMgr->at(i)->m_sock, &m_eset);//加入到连接异常集合
			}
#endif
			//得到最大的socket 
			if (m_pSockMgr->at(i)->m_sock > maxfd)
			{
				maxfd=m_pSockMgr->at(i)->m_sock; 
			}
			ret++;
		}
	}
	return ret;
}
void CLinkThread::OnNoEvent()
{
	unsigned int j;
	int tmptime;

	for (j=0; j< m_pRes->g_vDrebLinkInfo.size(); j++)
	{
		if (m_pSockMgr->at(j)->m_sock != INVALID_SOCKET && m_pSockMgr->at(j)->m_nType == SOCK_DREB)
		{
			tmptime = time(NULL) - m_pSockMgr->at(j)->m_nReadtime;
			if ( tmptime > m_pRes->g_nHeartRun && tmptime < m_pRes->g_nDisconnectTime)
			{
				OnPing(j);
			}
			else if (tmptime > m_pRes->g_nDisconnectTime)
			{
				m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"连接index[%d] 当前时间[%d] 连接最新读时间[%d] 配置断开时间[%d] 连接长时间未用，断开",\
					j,time(NULL),m_pSockMgr->at(j)->m_nReadtime,m_pRes->g_nDisconnectTime);
				OnClose(j,"连接长时间未用，断开",__FILE__,__LINE__);
			}
		}
		if (m_pRes->g_bIsExit)
		{
			return;
		}
		//若是数据总线节点连接，且没有连上则去连接
		if (m_pSockMgr->at(j)->m_nType == SOCK_DREB )
		{
			if (m_pSockMgr->at(j)->m_sock == INVALID_SOCKET)
			{
				OnConnect(j);
			}
		}
	}
	
}

void CLinkThread::OnEvent()
{
	unsigned int i;
	for (i= 0; i< m_pRes->g_vDrebLinkInfo.size() ; i++)
	{
		if (m_pSockMgr->at(i)->m_sock == INVALID_SOCKET)//未连接
		{
			if (m_pSockMgr->at(i)->m_nType == SOCK_DREB   ) //若是数据总线节点连接，则去连接
			{
				OnConnect(i);
			}
			continue;
		}
		else
		{
			//响应读事件
			OnReadEvent(i);
			if (m_pSockMgr->at(i)->m_sock != INVALID_SOCKET)//未连接
			{
				//响应写事件
				OnWriteEvent(i);
			}
		}
	}
}

void CLinkThread::OnReadEvent(int conindex)
{
	int ret;
	ret = FD_ISSET(m_pSockMgr->at(conindex)->m_sock,&m_rset);
	if (ret != 0) //有读事件
	{
		OnRecv(conindex);
	}
	else //此连接无读事件
	{
		if (m_pSockMgr->at(conindex)->m_sock != INVALID_SOCKET)
		{
			//没有检查通过
			if (!m_pSockMgr->at(conindex)->m_bChecked)
			{
				//数据总线节点连接
				if (m_pSockMgr->at(conindex)->m_nType == SOCK_DREB) 
				{
					if (time(NULL)-m_pSockMgr->at(conindex)->m_nReadtime >= m_pRes->g_nHeartRun)
					{
						if (m_pSockMgr->at(conindex)->NeedConnect())
						{
							m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"尝试连接到%s:%d未成功",m_pSockMgr->at(conindex)->m_sDrebIp.c_str(),m_pSockMgr->at(conindex)->m_nDrebPort);
							closesocket(m_pSockMgr->at(conindex)->m_sock);
							m_pSockMgr->at(conindex)->m_sock = INVALID_SOCKET;
						}
					}
				}// end if (m_pSockMgr->at(conindex)->m_nType == BPCSOCK_TODREB) //数据总线节点连接
				else if (m_pSockMgr->at(conindex)->m_nType == SOCK_UNKNOW)
				{
					if (time(NULL)-m_pSockMgr->at(conindex)->m_nReadtime >= m_pRes->g_nHeartRun)
					{
						OnClose(conindex,"连接非法",__FILE__,__LINE__);
					} 
				}
				else 
				{
					if (time(NULL)-m_pSockMgr->at(conindex)->m_nReadtime >= m_pRes->g_nHeartRun)
					{
						OnClose(conindex,"协商超时",__FILE__,__LINE__);
					} 
				}
			}
		}// end if (m_pSockMgr->at(conindex)->m_sock != INVALID_SOCKET)
	}// end if (ret == 1) //有读事件
}

void CLinkThread::OnWriteEvent(int conindex)
{
	
	if (m_pSockMgr->at(conindex)->m_sock == INVALID_SOCKET)
	{
		return;
	}
#if defined(_WINDOWS)
	int ret;
	ret = FD_ISSET(m_pSockMgr->at(conindex)->m_sock, &m_eset);
	if (ret != 0)
	{
		//主动连接其它的DREB
		if (m_pSockMgr->at(conindex)->m_nType == SOCK_DREB)
		{
			m_pLog->LogMp(2,__FILE__,__LINE__,"尝试连接到%s:%d未成功",m_pSockMgr->at(conindex)->m_sDrebIp.c_str(),m_pSockMgr->at(conindex)->m_nDrebPort);
			closesocket(m_pSockMgr->at(conindex)->m_sock);
			m_pSockMgr->at(conindex)->m_sock = INVALID_SOCKET;
		}
		else 
		{
			OnClose(conindex,"Other connect fail",__FILE__,__LINE__);
		}
	}
	else if (FD_ISSET(m_pSockMgr->at(conindex)->m_sock, &m_wset) !=0 )
	{
		if (m_pSockMgr->at(conindex)->NeedConnect())
		{
			if (m_pSockMgr->at(conindex)->m_nType == SOCK_DREB ) 
			{
				OnConnected(conindex);//去连接数据总线节点
			}
		}
		else if (m_pSockMgr->at(conindex)->m_bChecked)
		{
			OnSend(conindex);//发送数据
		}
	}
	else  if (m_pSockMgr->at(conindex)->m_bChecked)//socket无写事件
	{
		OnSend(conindex);//发送数据
	}
#else
	
	int ok;
	int oklen;
	if (FD_ISSET(m_pSockMgr->at(conindex)->m_sock, &m_wset) !=0)
	{
		if (m_pSockMgr->at(conindex)->NeedConnect())
		{
			oklen=sizeof(ok);
#if defined(HP_UX)
			getsockopt(m_pSockMgr->at(conindex)->m_sock,SOL_SOCKET,SO_ERROR,(char *)&ok,&oklen);
#else
			getsockopt(m_pSockMgr->at(conindex)->m_sock,SOL_SOCKET,SO_ERROR,(char *)&ok,(socklen_t *)&oklen);
#endif
			if (ok == 0)
			{
				if (m_pSockMgr->at(conindex)->m_nType == SOCK_DREB)
				{
					OnConnected(conindex);//去连接数据总线节点
				}
			}
			else
			{
				if (m_pSockMgr->at(conindex)->m_nType == SOCK_DREB)
				{
					m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"尝试连接到%s:%d未成功",m_pSockMgr->at(conindex)->m_sDrebIp.c_str(),m_pSockMgr->at(conindex)->m_nDrebPort);
					closesocket(m_pSockMgr->at(conindex)->m_sock);
					m_pSockMgr->at(conindex)->m_sock = INVALID_SOCKET;
				}
			} // end if (ok == 0)
		}
		else if (m_pSockMgr->at(conindex)->m_bChecked)// else if (g_connInfoList[conindex]->m_bNeedconnect)
		{
			OnSend(conindex);//发送数据
		} // end if (g_connInfoList[conindex]->m_bNeedconnect)
	}
	else  if (m_pSockMgr->at(conindex)->m_bChecked)
	{
		OnSend(conindex);//发送数据
	}
#endif

				
}


void CLinkThread::OnPing(int conindex)
{
	if (time(NULL) - m_pSockMgr->at(conindex)->m_nPingTime < BEATHEARTTICK) //小于5秒不用发ping
	{
		return ;
	}
	if (m_pSockMgr->at(conindex)->m_nType == SOCK_DREB)
	{
		m_pSockMgr->at(conindex)->m_nPingTime = time(NULL);
		if (!ResetData(conindex,&m_pDataBuf))
		{
			return;
		}
		m_pDataBuf.data->head.cRaflag = 0;
		m_pDataBuf.data->head.cZip = 0;
		m_pDataBuf.data->head.nLen = 0;
		m_pDataBuf.data->head.cCmd = CMD_PING;
		//发送ping数据包
		m_pSockMgr->at(conindex)->Send(m_pDataBuf);
	}
	
}

void CLinkThread::OnClose(int conindex,const char *msg,const char *filename,int fileline)
{
	if (m_pSockMgr->at(conindex)->m_sock == INVALID_SOCKET)
	{
		return;
	}
	//将socket关闭
	m_pSockMgr->at(conindex)->OnClose(msg,filename,fileline);
}


void CLinkThread::OnSend(int conindex)
{
	m_pSockMgr->at(conindex)->OnSend();
}
void CLinkThread::OnConnect(int conindex)
{
	if (m_pSockMgr->at(conindex)->m_sock != INVALID_SOCKET) //连接成功
	{
		return ;
	}
	//去连接
	if (time(NULL) - m_pSockMgr->at(conindex)->m_nConntime < 5 ) //在10秒内不用重连
	{
		return;
	}

	m_pSockMgr->at(conindex)->m_nConntime = time(NULL);
	m_tcpSocket.Create(AF_INET,false);//异步方式
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"尝试连接到数据总线节点%s:%d:%d",m_pSockMgr->at(conindex)->m_sDrebIp.c_str(),m_pSockMgr->at(conindex)->m_nDrebPort,m_pSockMgr->at(conindex)->m_sock);
	if (m_tcpSocket.ConnectServer((char *)m_pSockMgr->at(conindex)->m_sDrebIp.c_str(),m_pSockMgr->at(conindex)->m_nDrebPort))
	{
		
		m_pSockMgr->at(conindex)->m_nReadtime = time(NULL);
		m_pSockMgr->at(conindex)->m_sock = m_tcpSocket.GetSocket();
	}
	else
	{
		if ((m_tcpSocket.GetErrorNo() == EWOULDBLOCK) || (m_tcpSocket.GetErrorNo() == EINPROGRESS))
		{
			m_pSockMgr->at(conindex)->m_nReadtime = time(NULL);
			m_pSockMgr->at(conindex)->m_sock = m_tcpSocket.GetSocket();
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"等待建立到%s:%d的连接",m_pSockMgr->at(conindex)->m_sDrebIp.c_str(),m_pSockMgr->at(conindex)->m_nDrebPort);
			
			return;
		}
		else
		{
			m_pSockMgr->at(conindex)->m_sock = INVALID_SOCKET;
		}
	}
	return;
	
}
void CLinkThread::OnConnected(int conindex)
{
	if (m_pSockMgr->at(conindex)->m_sock != INVALID_SOCKET) //连接成功
	{
		//可以发送数据了
		m_pSockMgr->at(conindex)->m_bNeedread = true;
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"建立到%s:%d的连接成功",m_pSockMgr->at(conindex)->m_sDrebIp.c_str(),m_pSockMgr->at(conindex)->m_nDrebPort);
		//连接成功后发送CMD_CONNECT消息，确认连接的类型
		m_pSockMgr->at(conindex)->m_bNeedConnect = false;
		//发送连接消息
		m_pSockMgr->at(conindex)->m_nPingTime = time(NULL);
		if (!ResetData(conindex,&m_pDataBuf))
		{
			return;
		}
		
		m_pDataBuf.data->head.cRaflag = 0;
		m_pDataBuf.data->head.cZip = 0;
		m_pDataBuf.data->head.cCmd = CMD_CONNECT;
		m_pDataBuf.data->head.cDrebAffirm = 0;
	
		
		m_pDataBuf.data->head.nLen = sizeof(S_CONNECT_DATA);
		S_CONNECT_DATA *data = (S_CONNECT_DATA *)m_pDataBuf.data->buffer;
		data->cConnectType = 1;
		data->nSvrMainId = m_pRes->g_nSvrMainId;
		data->cSvrPrivateId = m_pRes->g_nSvrPrivateId;
		sprintf(data->sVersion,"%s",g_VERSION);
		m_pDrebEndian.Endian2Comm((unsigned char *)&(data->nSvrMainId),sizeof(data->nSvrMainId));
		
		if (m_pSockMgr->at(conindex)->Send(m_pDataBuf,true) == 0)
		{
			m_pSockMgr->at(conindex)->m_bChecked = true;
		}
		return ;
	}
	return;
	
}

void CLinkThread::OnRecv(int conindex)
{
	int ret=0;
	char tmpchar[200];
	ret = m_pSockMgr->at(conindex)->OnRecv();	
	if (ret<=0)
	{
		sprintf(tmpchar,"%s 远端关闭，接收数据不完整 ret=[%d]",m_pSockMgr->at(conindex)->m_sAddress,ret);
		OnClose(conindex,tmpchar,__FILE__,__LINE__);
		return;
	}
	//开始接收处理
	if (m_pSockMgr->at(conindex)->m_nRcvBufLen >= DREBHEADLEN)
	{
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"开始接收处理  数据长度%d",m_pSockMgr->at(conindex)->m_nRcvBufLen );
	}
	while (m_pSockMgr->at(conindex)->m_nRcvBufLen >= DREBHEADLEN)
	{
		if (!ResetData(conindex,&m_pDataRcv))
		{
			return;
		}
		ret = m_pSockMgr->at(conindex)->GetRecvData(&m_pDataRcv);
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"index[%d] GetRecvData 返回[%d]",conindex,ret);
		if (ret >0 ) //有数据
		{
			m_pDataRcv.nIndex = conindex;
			//接收dreb过来的数据
			if (m_pDataRcv.data->head.cCmd == CMD_PING || m_pDataRcv.data->head.cCmd == CMD_CONNECT) //响应PING应答
			{
				if (m_pDataRcv.data->head.cRaflag == 0) //请求
				{
					m_pDataRcv.data->head.cRaflag = 1;
					m_pSockMgr->at(conindex)->Send(m_pDataRcv);
					m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"index[%d]收到CMD_PING,返回应答",conindex);
				}
				else //应答，什么都不用做
				{
					if ( m_pDataRcv.data->head.cCmd == CMD_CONNECT)
					{
						m_pSockMgr->at(conindex)->m_nDrebId = m_pDataRcv.data->head.a_Ainfo.a_nNodeId;
						m_pSockMgr->at(conindex)->m_nDrebPrivateId = m_pDataRcv.data->head.a_Ainfo.a_cNodePrivateId;
						m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"index[%d]收到CMD_CONNECT应答",conindex);
					}
					else
					{
						m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"index[%d]收到CMD_PING应答",conindex);
					}
					
					m_pMemPool->PoolFree(m_pDataRcv.data);
					m_pDataRcv.data = NULL;
				}
				m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"index[%d]开始接收处理  数据长度%d",conindex,m_pSockMgr->at(conindex)->m_nRcvBufLen );
				continue; ;
			}
			m_pDataRcv.nIndex = conindex;
			ProcessDreb(m_pDataRcv);
			m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"index[%d]处理完一条数据  缓冲数据长度[%d]",conindex,m_pSockMgr->at(conindex)->m_nRcvBufLen);

		}
		else  //ret <=0出错或未接收完整
		{
			if (ret <0)
			{
				OnClose(conindex,"CRC校验错",__FILE__,__LINE__);
			}
			m_pMemPool->PoolFree(m_pDataRcv.data);
			m_pDataRcv.data = NULL;
			m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"退出接收处理  缓冲数据长度[%d]",m_pSockMgr->at(conindex)->m_nRcvBufLen);
			break;
		}
	}
	
	return ;
}


bool CLinkThread::ResetData(int index,S_GATEDREB_RSMSG *data)
{
	data->data = (PCOMMSTRU )m_pMemPool->PoolMalloc();
	if (data->data == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"分配消息空间出错");
		m_pSockMgr->at(index)->OnClose("分配消息空间出错",__FILE__,__LINE__);
		return false;
	}
	data->offset = 0;
	data->rtime = time(NULL);
	data->nIndex = index;
	return true;
}


void CLinkThread::ProcessDreb(S_GATEDREB_RSMSG &rcvdata)
{
	
	if (rcvdata.data->head.cCmd != CMD_PING && rcvdata.data->head.cCmd != CMD_CONNECT && rcvdata.data->head.cCmd != CMD_REGSERVICE)
	{
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"从DREB[%d %d %d]收到的消息 DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 源[%d %d] 业务数据长度[%d]",\
			rcvdata.data->head.s_Sinfo.s_nNodeId,rcvdata.data->head.s_Sinfo.s_cNodePrivateId,rcvdata.nIndex,\
			GetDrebCmdType(rcvdata.data->head.cCmd).c_str(),rcvdata.data->head.cNextFlag,\
			rcvdata.data->head.cRaflag,rcvdata.data->head.d_Dinfo.d_nServiceNo,rcvdata.data->head.s_Sinfo.s_nNodeId,\
			rcvdata.data->head.s_Sinfo.s_cNodePrivateId,rcvdata.data->head.s_Sinfo.s_nDrebSerial,\
			rcvdata.data->head.s_Sinfo.s_nSerial,rcvdata.data->head.s_Sinfo.s_nHook,rcvdata.data->head.nLen);
	}
	else
	{
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"接收到DREB[%d]的消息 DREB命令[%s]  业务数据长度[%d]",\
			rcvdata.nIndex,GetDrebCmdType(rcvdata.data->head.cCmd).c_str(),rcvdata.data->head.nLen);
	}

	if (!UnzipBuf(rcvdata.data))
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解密出错");
		m_pMemPool->PoolFree(rcvdata.data);
		rcvdata.data = NULL;
		return;
	}
	if (rcvdata.data->head.cCmd != CMD_PING && rcvdata.data->head.cCmd != CMD_CONNECT && rcvdata.data->head.cCmd != CMD_REGSERVICE)
	{
		m_pDrebLog->LogDreb(LOG_DEBUG,rcvdata.data,false);
	}
	if (rcvdata.data->head.cCmd == CMD_MONITOR_BPC)
	{
		//m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"CMD_MONITOR_BPC [%d]",rcvdata.data->head.d_Dinfo.d_nServiceNo);
		CallBpcLocalFunc(rcvdata);
		return;
	}
	S_CGATE_SMSG data;
	bzero(&data,sizeof(S_CGATE_SMSG));
	data.data.head.nLen = rcvdata.data->head.nLen;
	if (data.data.head.nLen>0 && data.data.head.nLen<DREBDATASIZE)
	{
		memcpy(data.data.buffer,rcvdata.data->buffer,data.data.head.nLen);
	}
	else if (data.data.head.nLen >=DREBDATASIZE)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据非法 %d",data.data.head.nLen);
		m_pMemPool->PoolFree(rcvdata.data);
		rcvdata.data = NULL;
		return;
	}
	data.s_nDrebSerial = rcvdata.data->head.s_Sinfo.s_nDrebSerial;
	data.txcode = rcvdata.data->head.d_Dinfo.d_nServiceNo;
	if (rcvdata.data->head.cRaflag == 0) //请求
	{
		data.data.head.nRetCode = 0;
		data.data.head.stComm.cCmd = rcvdata.data->head.cCmd;
		data.data.head.stComm.cMsgType = MSG_PUSH;
		data.data.head.stComm.cNextFlag = rcvdata.data->head.cNextFlag;
		data.data.head.stComm.cRaflag = rcvdata.data->head.cRaflag;
		data.data.head.stComm.cZip = rcvdata.data->head.cZip;
		data.data.head.stComm.nCrc = 0;
		data.data.head.stDest.cNodePrivateId = rcvdata.data->head.s_Sinfo.s_cNodePrivateId;
		data.data.head.stDest.cSvrPrivateId = rcvdata.data->head.s_Sinfo.s_cSvrPrivateId;
		data.data.head.stDest.nNodeId = rcvdata.data->head.s_Sinfo.s_nNodeId;
		
		data.d_nNodeId = rcvdata.data->head.s_Sinfo.s_nNodeId;
		data.d_cNodePrivateId = rcvdata.data->head.s_Sinfo.s_cNodePrivateId;
		data.s_nDrebSerial = rcvdata.data->head.s_Sinfo.s_nDrebSerial;
		data.rtime = time(NULL);

// 		PS_GATE_SERIAL gserial = (PS_GATE_SERIAL)&(rcvdata.data->head.s_Sinfo.s_nSerial);//存放的是连接索引+请求流水
// 		
// 
// 		data.data.head.stDest.nSerial = gserial->index;//连接索引

		data.data.head.stDest.nServiceNo = rcvdata.data->head.d_Dinfo.d_nServiceNo;
		data.data.head.stDest.nSvrMainId = rcvdata.data->head.s_Sinfo.s_nSvrMainId;
		data.data.head.stNext.n_nNextNo = 0;
		data.data.head.stNext.n_nNextOffset = 0;

		data.data.head.stDest.nSerial = rcvdata.data->head.s_Sinfo.s_nSerial;
		data.index = rcvdata.data->head.s_Sinfo.s_nGateIndex;
		data.timestamp = rcvdata.data->head.s_Sinfo.s_nHook;
		

// #define  CMD_DPCALL      8  //数据总线节点同步调用 要求最终处理完成后应答
// #define  CMD_DPACALL     9  //数据总线节点异步调用 要求接收到的数据总线节点发送至服务后确认应答
// #define  CMD_DPBC        10  //数据总线节点广播，即将信息发给指定数据总线节点上所有注册的服务,要求数据总线节点应答
// #define  CMD_DPABC       11  //数据总线节点广播，即将信息发给指定数据总线节点上所有注册的服务,不要求数据总线节点应答
// #define  CMD_DPPUSH      12 //数据总线节点推送，无须应答
// #define  CMD_DPPOST      13 //数据总线节点投递，要求接收到的数据总线节点应答		
		if (rcvdata.data->head.cCmd == CMD_DPBC || rcvdata.data->head.cCmd == CMD_DPABC ) //广播公共流
		{
			if (m_pRes->g_nBroadCastMode != 0)
			{
				if (m_pRes->g_nBroadCastMode == 1)
				{
					//禁止发送
					if (m_pRes->g_lBCFuncList.Find(rcvdata.data->head.d_Dinfo.d_nServiceNo))
					{
						m_pMemPool->PoolFree(rcvdata.data);
						rcvdata.data = NULL;
						return;
					}
				}
				else
				{	
					//允许发送
					if (!m_pRes->g_lBCFuncList.Find(rcvdata.data->head.d_Dinfo.d_nServiceNo))
					{
						m_pMemPool->PoolFree(rcvdata.data);
						rcvdata.data = NULL;
						return;
					}
				}
			}

			//查找行情是否是最新的，若不是直接丢弃，若是直更新行情表，将数据丢入队列
			data.isBC = 1;
			data.index = 0;
			data.timestamp = 0;
			//第三级别为行情信息

			//后台送过来的交易码9999999为交易行情
			if ( 9999999 == rcvdata.data->head.d_Dinfo.d_nServiceNo)
			{
				//判断行情是否为最新
				//rcvdata.data->head.s_Sinfo.s_nHook 为合约编号
				//rcvdata.data->head.s_Sinfo.s_nSerial 为行情日期
				//rcvdata.data->head.s_Sinfo.s_nGateIndex 为行情序号
				data.nVerietyId = rcvdata.data->head.s_Sinfo.s_nHook;
				ProcessQuota(rcvdata,data);
			}
			else
			{ 
				if ( 9999998 == rcvdata.data->head.d_Dinfo.d_nServiceNo)
				{
					data.nVerietyId = rcvdata.data->head.s_Sinfo.s_nHook;
				}
				m_pSendData->PushData(data,2);
				m_pMemPool->PoolFree(rcvdata.data);
				rcvdata.data = NULL;
			}
			return;
		}
		else  if (rcvdata.data->head.cCmd == CMD_DPCALL || rcvdata.data->head.cCmd == CMD_DPACALL || \
			rcvdata.data->head.cCmd == CMD_DPPUSH || rcvdata.data->head.cCmd == CMD_DPPOST )
		//私有流，指定公告，成交回报，委托回报
		{
			if (data.index == 0)
			{
				m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"无效的请求 index[%d] cCmd[%d]",data.index,data.data.head.stComm.cCmd);
				m_pMemPool->PoolFree(rcvdata.data);
				rcvdata.data = NULL;
				return;
			}
			//源的nGateIndex保存用户登录的连接索引   s_nHook保存连接的时间戳
			data.isBC = 0;
			//第二级别为回报信息  
			m_pSendData->PushData(data,1);
//			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"发送队列数据目[%d]",m_pSendData->GetSize());

			m_pMemPool->PoolFree(rcvdata.data);
			rcvdata.data = NULL;
			return;

		}
		m_pMemPool->PoolFree(rcvdata.data);
		
		rcvdata.data = NULL;
		return;	
	}
	else  //是DREB的应答
	{
		if (rcvdata.data->head.cCmd == CMD_MONITOR_BPC)
		{
			m_pMemPool->PoolFree(rcvdata.data);
			rcvdata.data = NULL;
			return;
		}
//		PS_GATE_SERIAL gserial = (PS_GATE_SERIAL)&(rcvdata.data->head.s_Sinfo.s_nSerial);

		data.data.head.nRetCode = rcvdata.data->head.a_Ainfo.a_nRetCode;
		data.data.head.stComm.cCmd = rcvdata.data->head.cCmd;
		data.data.head.stComm.cMsgType = MSG_GATEREQ;
		data.data.head.stComm.cNextFlag = rcvdata.data->head.cNextFlag;
		data.data.head.stComm.cRaflag = rcvdata.data->head.cRaflag;
		data.data.head.stComm.cZip = rcvdata.data->head.cZip;
		data.data.head.stComm.nCrc = 0;
		data.data.head.stDest.cNodePrivateId = rcvdata.data->head.d_Dinfo.d_cNodePrivateId;
		data.data.head.stDest.cSvrPrivateId = rcvdata.data->head.d_Dinfo.d_cSvrPrivateId;
		data.data.head.stDest.nNodeId = rcvdata.data->head.d_Dinfo.d_nNodeId;
		data.data.head.stDest.nSerial = rcvdata.data->head.s_Sinfo.s_nSerial;
		data.data.head.stDest.nServiceNo = rcvdata.data->head.d_Dinfo.d_nServiceNo;
		data.data.head.stDest.nSvrMainId = rcvdata.data->head.d_Dinfo.d_nSvrMainId;
		data.data.head.stNext.n_nNextNo = rcvdata.data->head.n_Ninfo.n_nNextNo;
		data.data.head.stNext.n_nNextOffset = rcvdata.data->head.n_Ninfo.n_nNextOffset;
		
		data.d_nNodeId = rcvdata.data->head.s_Sinfo.s_nNodeId;
		data.d_cNodePrivateId = rcvdata.data->head.s_Sinfo.s_cNodePrivateId;
		data.s_nDrebSerial = rcvdata.data->head.s_Sinfo.s_nDrebSerial;
		data.rtime = time(NULL);

		data.index = rcvdata.data->head.s_Sinfo.s_nGateIndex;
		data.timestamp = rcvdata.data->head.s_Sinfo.s_nHook;
		if (data.index == 0 && rcvdata.data->head.s_Sinfo.s_nHook == 0)
		{
			m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"无效的应答");
			m_pMemPool->PoolFree(rcvdata.data);
			rcvdata.data = NULL;
			return;
		}
		data.isBC = 0;
		//最优先响应应答数据
     	//统一由一个线程加密并放入发送队列
		m_pSendData->PushData(data,0);
//		m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"发送队列数目[%d]",m_pSendData->GetSize());
	

		m_pMemPool->PoolFree(rcvdata.data);
		rcvdata.data = NULL;
		return;
		
	}
	m_pMemPool->PoolFree(rcvdata.data);
	rcvdata.data = NULL;
	return;
}


void CLinkThread::CallBpcLocalFunc(S_GATEDREB_RSMSG &rcvdata)
{
// #define  BPCMONI_INFO       11     //bpc信息   节点 bpu组
// #define  BPCMONI_HOST       12     //主机信息  CPU 碰盘 内存
// #define  BPCMONI_BPU        13     //路由信息  服务、bpu数
// #define  BPCMONI_REPORT     14     //运行统计信息  处理请求数 队列数  
// #define  BPCMONI_SETBPU     15     //设置BPU数目
// #define  BPCMONI_SETMODE    16     //设置bpc运行模式
// #define  BPCMONI_STOPBPU    17     //停止BPU
// #define  BPCMONI_STARTBPU   18     //启动BPU
	//DREB过来的调用
	switch (rcvdata.data->head.d_Dinfo.d_nServiceNo)
	{
		case BPCMONI_INFO:
			MonitorInfo(rcvdata);
			break;
		case BPCMONI_HOST:
			MonitorHost(rcvdata);
			break;
		case BPCMONI_BPU:
			MonitorBpu(rcvdata);
			break;
		case BPCMONI_REPORT:
			MonitorReport(rcvdata);
			break;
		case BPCMONI_DOWNLOAD:
			Download(rcvdata);
			break;
		case BPCMONI_UPLOAD:
			Upload(rcvdata);
			break;
		default:
			rcvdata.data->head.cRaflag = 1;
			rcvdata.data->head.cZip = 0;
			rcvdata.data->head.a_Ainfo.a_nRetCode = ERR_FUNCNOTFUND;
			rcvdata.data->head.nLen = 0;
			m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
			break;
	}
	

}


void CLinkThread::LogDrebHead(int loglevel, DREB_HEAD head, const char *msg, const char *filename, int fileline)
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
	m_pLog->LogMp(loglevel,filename,fileline,m_headMsg,msg);
}

bool CLinkThread::UnzipBuf(PCOMMSTRU data)
{
	//0明文 1des加密密钥长度16 2压缩 3DES加密 4DES加密并压缩
	if (data->head.nLen == 0)
	{
		return true;
	}
	if (data->head.cZip >0 && data->head.cZip <=4)
	{
		char zipDataBuf[BPUDATASIZE];
		bzero(zipDataBuf,sizeof(zipDataBuf));
		int ddlen;
		switch (data->head.cZip)
		{
			case 1:
				ddlen = data->head.nLen;
				if (m_pDes.UncryptString(data->buffer,ddlen,DESENCRYPTKEY)<1)
				{
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解密出错");
					return false;
				}
				data->head.nLen = ddlen;
				data->head.cZip =0;
				break;
			case 2:	
				break;
			case 3:
				ddlen = data->head.nLen ;
				if (m_pDes.UncryptString(data->buffer,ddlen,DESENCRYPTKEY3)<1)
				{
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解密出错");
					return false;
				}
				data->head.nLen = ddlen;
				data->head.cZip =0;
				break;
			case 4:
				ddlen = data->head.nLen ;
				if (m_pDes.UncryptString(data->buffer,ddlen,DESENCRYPTKEY3)<1)
				{
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解密出错");
					return false;
				}
				data->head.nLen = ddlen;
				data->head.cZip =2;
				break;
			default:
				return false;
		}
	}
	
	return true;
}
void CLinkThread::StartTimer()
{
	m_pTimer.Start();
}

void CLinkThread::StopTimer()
{
	m_pTimer.Stop();
	m_pTimer.Join();
}
int CLinkThread::OnTimer(unsigned int eventid, void *p)
{
	CLinkThread *pp = (CLinkThread *)p;
	if (eventid == 0)
	{
		pp->PingTimer();
	}
	return 0;
}
void CLinkThread::PingTimer()
{
	//OnNoEvent();
//	m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"监控定时器");
	m_bIsPingTimer = true;
}

void CLinkThread::OnMonitor()
{
	//监控信息
	int i;
	CBF_Xml        pXml;
	pXml.SetNodeValueByPath("Monitor/公共节点",false,(int)m_pRes->g_nSvrMainId);
	pXml.SetNodeValueByPath("Monitor/私有节点",false,(int)m_pRes->g_nSvrPrivateId);
	pXml.SetNodeValueByPath("Monitor/节点类型",false,2);//类bpc
	pXml.SetNodeValueByPath("Monitor/报告类型",false,0);//正常
	pXml.SetNodeValueByPath("Monitor/启动时间",false,m_pRes->m_sStartDate,false);
	
	int nConntotal=0;
	int nConnnum = m_pAio->GetConnectPoolInfo(nConntotal);

	pXml.SetNodeAttribute("Monitor/连接池",false,"最大连接数",false,nConntotal );
	pXml.SetNodeAttribute("Monitor/连接池",false,"空闲连接数",false,nConnnum );
	pXml.SetNodeAttribute("Monitor/连接池",false,"客户连接数",false,nConntotal-nConnnum-1 );
	char extdata[300];
	sprintf(extdata,"连接池最大连接数:%d 空闲连接数%d 客户连接数%d",nConntotal,nConnnum,nConntotal-nConnnum-1);
	m_pMemPool->PoolInfo(nConntotal,nConnnum);
	pXml.SetNodeAttribute("Monitor/内存缓冲池",false,"总分配",false,nConntotal );
	pXml.SetNodeAttribute("Monitor/内存缓冲池",false,"未使用",false,nConnnum );
	pXml.SetNodeAttribute("Monitor/内存缓冲池",false,"每块大小",false,(int)DREBBUFSIZE );

	
	//取主机信息
	if (m_pRes->g_nMonitorHost == 1)
	{
		S_MONITOR_HOST hostinfo;
		V_MonitorDiskInfo  diskinfo;
		g_pBpcTime.GetHostInfo(hostinfo,diskinfo);
		pXml.SetNodeValueByPath("Monitor/主机资源/CPU",false,(int)hostinfo.nCpuRate);
		pXml.SetNodeAttribute("Monitor/主机资源/内存",false,"total",false,(int)hostinfo.nTotalMemory);
		pXml.SetNodeAttribute("Monitor/主机资源/内存",false,"used",false,(int)hostinfo.nUsedMemory);
		for (i=0 ; i< diskinfo.size() ; i++)
		{
			CXmlNode *disknode = pXml.AddNodeByPath("Monitor/主机资源/磁盘",false,"磁盘",false,"");
			if (disknode != NULL)
			{
				disknode->SetAttribute("drive",false,diskinfo[i].sDiskName,true);
				disknode->SetAttribute("total",false,(int)diskinfo[i].nTotalSpace);
				disknode->SetAttribute("used",false,(int)diskinfo[i].nUsedSpace);
			}
		}
	}
	
	//连接信息
	for (i=0 ; i < m_pRes->g_vDrebLinkInfo.size() ; i++)
	{
		CXmlNode *connnode = pXml.AddNodeByPath("Monitor/连接信息",false,"连接",false,"");
		if (connnode == NULL)
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"AddNodeByPath 出错");
			continue;
		}
		connnode->SetAttribute("ip",false,m_pRes->g_vDrebLinkInfo[i].sIp);
		connnode->SetAttribute("port",false,(int)m_pRes->g_vDrebLinkInfo[i].nPort);
		connnode->SetAttribute("index",false,i);
		connnode->SetAttribute("公共节点",false,m_pSockMgr->at(i)->m_nDrebId);
		connnode->SetAttribute("私有节点",false,m_pSockMgr->at(i)->m_nDrebPrivateId);
		if (m_pSockMgr->at(i)->m_sock == INVALID_SOCKET)
		{
			connnode->SetAttribute("status",false,"未连接");
		}
		else if (m_pSockMgr->at(i)->m_bChecked)
		{
			connnode->SetAttribute("status",false,"正常");
		}
		else
		{
			connnode->SetAttribute("status",false,"正在连接");
		}
	}
	pXml.SetNodeValueByPath("Monitor/扩展信息/cgate",false,extdata,false);
	pXml.SetNodeAttribute("Monitor/扩展信息/cgate",false,"status",false,0);

	S_GATEDREB_RSMSG drebdata;
	bzero(&drebdata,sizeof(S_GATEDREB_RSMSG));
	drebdata.data = (PCOMMSTRU)m_pMemPool->PoolMalloc();
	if (drebdata.data == NULL) 
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"PoolMalloc出错");
		return;
	}
	
	drebdata.rtime = time(NULL);
	bzero(drebdata.data,sizeof(COMMSTRU));
	int len = DREBDATASIZE;
	if (!pXml.ToBuffer(drebdata.data->buffer,len))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ToBuffer出错");
		m_pMemPool->PoolFree(drebdata.data);
		drebdata.data = NULL;
		return;
	}
	m_pLog->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"监控数据内容",drebdata.data->buffer,len);
	
	//报文头
	drebdata.data->head.nLen = len;
	drebdata.data->head.cCmd = CMD_DPPUSH;
	drebdata.data->head.cZip = 0;
	drebdata.data->head.cDrebAffirm = 0;
	drebdata.data->head.cNextFlag = 0; //是否是取后续包
	drebdata.data->head.cRaflag = 0;//请求应答标志
	
	drebdata.data->head.d_Dinfo.d_cNodePrivateId = m_pRes->g_nMonitorDrebPrivateId;
	drebdata.data->head.d_Dinfo.d_cSvrPrivateId = m_pRes->g_nMonitorSvrPrivateId;
	drebdata.data->head.d_Dinfo.d_nNodeId = m_pRes->g_nMonitorDrebId;
	drebdata.data->head.d_Dinfo.d_nServiceNo = m_pRes->g_nMonitorTxCode;
	drebdata.data->head.d_Dinfo.d_nSvrMainId = m_pRes->g_nMonitorSvrId;
		
	//放入队列，由消息线程来处理
	m_pPoolData->PushData(drebdata);
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"队列数[%d] 总分配内存块[%d] 未使用内存块[%d]",\
		m_pPoolData->GetSize(),nConntotal,nConnnum);

	return;
}

void CLinkThread::MonitorInfo(S_GATEDREB_RSMSG &rcvdata)
{
	int i;
	CBF_Xml xmlpack;
	xmlpack.SetNodeValueByPath("Monitor/BPC公共节点",false,(int)m_pRes->g_nSvrMainId);
	xmlpack.SetNodeValueByPath("Monitor/BPC私有节点",false,(int)m_pRes->g_nSvrPrivateId);
	xmlpack.SetNodeValueByPath("Monitor/启动时间",false,m_pRes->m_sStartDate,false);
	int nConntotal=0;
	int nConnnum = m_pAio->GetConnectPoolInfo(nConntotal);
	
	xmlpack.SetNodeAttribute("Monitor/连接池",false,"最大连接数",false,nConntotal );
	xmlpack.SetNodeAttribute("Monitor/连接池",false,"空闲连接数",false,nConnnum );
	xmlpack.SetNodeAttribute("Monitor/连接池",false,"客户连接数",false,nConntotal-nConnnum-1 );
	
	m_pMemPool->PoolInfo(nConntotal,nConnnum);
	xmlpack.SetNodeAttribute("Monitor/内存缓冲池",false,"总分配",false,nConntotal );
	xmlpack.SetNodeAttribute("Monitor/内存缓冲池",false,"未使用",false,nConnnum );
	xmlpack.SetNodeAttribute("Monitor/内存缓冲池",false,"每块大小",false,(int)DREBBUFSIZE );
	//连接信息
	for (i=0 ; i < m_pRes->g_vDrebLinkInfo.size() ; i++)
	{
		CXmlNode *connnode = xmlpack.AddNodeByPath("Monitor/连接信息",false,"连接",false,"");
		if (connnode == NULL)
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"AddNodeByPath 出错");
			continue;
		}
		connnode->SetAttribute("ip",false,m_pRes->g_vDrebLinkInfo[i].sIp);
		connnode->SetAttribute("port",false,(int)m_pRes->g_vDrebLinkInfo[i].nPort);
		connnode->SetAttribute("index",false,i);
		connnode->SetAttribute("公共节点",false,m_pSockMgr->at(i)->m_nDrebId);
		connnode->SetAttribute("私有节点",false,m_pSockMgr->at(i)->m_nDrebPrivateId);
		if (m_pSockMgr->at(i)->m_sock == INVALID_SOCKET)
		{
			connnode->SetAttribute("status",false,"未连接");
		}
		else if (m_pSockMgr->at(i)->m_bChecked)
		{
			connnode->SetAttribute("status",false,"正常");
		}
		else
		{
			connnode->SetAttribute("status",false,"正在连接");
		}
	}
	
	//BPU信息

	
	int len = DREBDATASIZE;
	if (!xmlpack.ToBuffer(rcvdata.data->buffer,len))
	{
		sprintf(rcvdata.data->buffer,"缓冲不足，请下载cgateinfo.txt查看");
		rcvdata.data->head.nLen = strlen(rcvdata.data->buffer);
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"缓冲不足");
		xmlpack.ToFile("cgateinfo.txt");
	}
	else
	{
		rcvdata.data->head.nLen = len;
	}
	rcvdata.data->head.cRaflag = 1;
	rcvdata.data->head.cZip = 0;
	rcvdata.data->head.a_Ainfo.a_nRetCode = SUCCESS;
	m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
}

void CLinkThread::MonitorHost(S_GATEDREB_RSMSG &rcvdata)
{
	unsigned int i;
	CBF_Xml xmlpack;
	xmlpack.SetNodeValueByPath("Monitor/BPC公共节点",false,(int)m_pRes->g_nSvrMainId);
	xmlpack.SetNodeValueByPath("Monitor/BPC私有节点",false,(int)m_pRes->g_nSvrPrivateId);
	xmlpack.SetNodeValueByPath("Monitor/启动时间",false,m_pRes->m_sStartDate,false);
	int nConntotal=0;
	int nConnnum = m_pAio->GetConnectPoolInfo(nConntotal);
	
	xmlpack.SetNodeAttribute("Monitor/连接池",false,"最大连接数",false,nConntotal );
	xmlpack.SetNodeAttribute("Monitor/连接池",false,"空闲连接数",false,nConnnum );
	xmlpack.SetNodeAttribute("Monitor/连接池",false,"客户连接数",false,nConntotal-nConnnum-1 );
	
	m_pMemPool->PoolInfo(nConntotal,nConnnum);
	xmlpack.SetNodeAttribute("Monitor/内存缓冲池",false,"总分配",false,nConntotal );
	xmlpack.SetNodeAttribute("Monitor/内存缓冲池",false,"未使用",false,nConnnum );
	xmlpack.SetNodeAttribute("Monitor/内存缓冲池",false,"每块大小",false,(int)DREBBUFSIZE );


	//取主机信息
	if (m_pRes->g_nMonitorHost == 1)
	{
		S_MONITOR_HOST hostinfo;
		V_MonitorDiskInfo  diskinfo;
		g_pBpcTime.GetHostInfo(hostinfo,diskinfo);
		xmlpack.SetNodeValueByPath("Monitor/主机资源/CPU",false,(int)hostinfo.nCpuRate);
		xmlpack.SetNodeAttribute("Monitor/主机资源/内存",false,"total",false,(int)hostinfo.nTotalMemory);
		xmlpack.SetNodeAttribute("Monitor/主机资源/内存",false,"used",false,(int)hostinfo.nUsedMemory);
		for (i=0 ; i< diskinfo.size() ; i++)
		{
			CXmlNode *disknode = xmlpack.AddNodeByPath("Monitor/主机资源/磁盘",false,"磁盘",false,"");
			if (disknode != NULL)
			{
				disknode->SetAttribute("drive",false,diskinfo[i].sDiskName,true);
				disknode->SetAttribute("total",false,(int)diskinfo[i].nTotalSpace);
				disknode->SetAttribute("used",false,(int)diskinfo[i].nUsedSpace);
			}
		}
	}
	int len = DREBDATASIZE;
	if (!xmlpack.ToBuffer(rcvdata.data->buffer,len))
	{
		sprintf(rcvdata.data->buffer,"缓冲不足，请下载cgatehost.txt查看");
		rcvdata.data->head.nLen = strlen(rcvdata.data->buffer);
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"缓冲不足");
		xmlpack.ToFile("cgatehost.txt");
	}
	else
	{
		rcvdata.data->head.nLen = len;
	}
	rcvdata.data->head.cRaflag = 1;
	rcvdata.data->head.cZip = 0;
	rcvdata.data->head.a_Ainfo.a_nRetCode = SUCCESS;
	m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
}


void CLinkThread::MonitorReport(S_GATEDREB_RSMSG &rcvdata)
{
	CBF_Xml xmlpack;
	xmlpack.SetNodeValueByPath("Monitor/BPC公共节点",false,(int)m_pRes->g_nSvrMainId);
	xmlpack.SetNodeValueByPath("Monitor/BPC私有节点",false,(int)m_pRes->g_nSvrPrivateId);
	xmlpack.SetNodeValueByPath("Monitor/启动时间",false,m_pRes->m_sStartDate,false);
	int nConntotal=0;
	int nConnnum = m_pAio->GetConnectPoolInfo(nConntotal);
	
	xmlpack.SetNodeAttribute("Monitor/连接池",false,"最大连接数",false,nConntotal );
	xmlpack.SetNodeAttribute("Monitor/连接池",false,"空闲连接数",false,nConnnum );
	xmlpack.SetNodeAttribute("Monitor/连接池",false,"客户连接数",false,nConntotal-nConnnum-1 );
	
	m_pMemPool->PoolInfo(nConntotal,nConnnum);
	xmlpack.SetNodeAttribute("Monitor/内存缓冲池",false,"总分配",false,nConntotal );
	xmlpack.SetNodeAttribute("Monitor/内存缓冲池",false,"未使用",false,nConnnum );
	xmlpack.SetNodeAttribute("Monitor/内存缓冲池",false,"每块大小",false,(int)DREBBUFSIZE );
	
	char tmpchar[100];
	sprintf(tmpchar,INT64PFM,m_pRes->g_nSendByteLen);
	xmlpack.SetNodeValueByPath("Monitor/发送字节数",false,tmpchar,false);
	sprintf(tmpchar,INT64PFM,m_pRes->g_nRecvByteLen);
	xmlpack.SetNodeValueByPath("Monitor/接收字节数",false,tmpchar,false);
	int len = DREBDATASIZE;
	if (!xmlpack.ToBuffer(rcvdata.data->buffer,len))
	{
		sprintf(rcvdata.data->buffer,"缓冲不足，请下载cgatereport.txt查看");
		rcvdata.data->head.nLen = strlen(rcvdata.data->buffer);
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"缓冲不足");
		xmlpack.ToFile("cgatereport.txt");
	}
	else
	{
		rcvdata.data->head.nLen = len;
	}
	rcvdata.data->head.cRaflag = 1;
	rcvdata.data->head.cZip = 0;
	rcvdata.data->head.a_Ainfo.a_nRetCode = SUCCESS;
	m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
}

void CLinkThread::MonitorBpu(S_GATEDREB_RSMSG &rcvdata)
{
	int i;
	CBF_Xml xmlpack;
	xmlpack.SetNodeValueByPath("Monitor/BPC公共节点",false,(int)m_pRes->g_nSvrMainId);
	xmlpack.SetNodeValueByPath("Monitor/BPC私有节点",false,(int)m_pRes->g_nSvrPrivateId);
	xmlpack.SetNodeValueByPath("Monitor/启动时间",false,m_pRes->m_sStartDate,false);
	int nConntotal=0;
	int nConnnum =0;
	m_pMemPool->PoolInfo(nConntotal,nConnnum);
	xmlpack.SetNodeAttribute("Monitor/内存缓冲池",false,"总分配",false,nConntotal );
	xmlpack.SetNodeAttribute("Monitor/内存缓冲池",false,"未使用",false,nConnnum );
	xmlpack.SetNodeAttribute("Monitor/内存缓冲池",false,"每块大小",false,(int)DREBBUFSIZE );

	nConnnum = m_pAio->GetConnectPoolInfo(nConntotal);
	xmlpack.SetNodeAttribute("Monitor/连接池",false,"最大连接数",false,nConntotal );
	xmlpack.SetNodeAttribute("Monitor/连接池",false,"空闲连接数",false,nConnnum );
	xmlpack.SetNodeAttribute("Monitor/连接池",false,"客户连接数",false,nConntotal-nConnnum-1 );
	
	

	xmlpack.SetNodeValueByPath("Monitor/连接列表",false,"",false);
	char tmpchar[100];
	for ( i=1 ; i<nConntotal ; i++ )
	{
		PSOCKET_POOL_DATA info = m_pAio->GetSocketByIndex(i);
		if (info == NULL)
		{
			continue;
		}
		if (info->s_hSocket != INVALID_SOCKET)
		{
			sprintf(tmpchar,"%s",inet_ntoa(info->s_pSocketAddr.sin_addr));
			CXmlNode *disknode = xmlpack.AddNodeByPath("Monitor/连接列表",false,tmpchar,false,"");
			if (disknode != NULL)
			{
				disknode->SetAttribute("index",false,i);
				disknode->SetAttribute("socket",false,(int)info->s_hSocket);
				disknode->SetAttribute("flag",false,(int)info->s_cCheckFlag);
			}
		}
	}
	int len = DREBDATASIZE;
	if (!xmlpack.ToBuffer(rcvdata.data->buffer,len))
	{
		sprintf(rcvdata.data->buffer,"缓冲不足，请下载clientlist.txt查看");
		rcvdata.data->head.nLen = strlen(rcvdata.data->buffer);
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"缓冲不足");
		xmlpack.ToFile("clientlist.txt");
	}
	else
	{
		rcvdata.data->head.nLen = len;
	}
	rcvdata.data->head.cRaflag = 1;
	rcvdata.data->head.cZip = 0;
	rcvdata.data->head.a_Ainfo.a_nRetCode = SUCCESS;
	m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
}

void CLinkThread::Download(S_GATEDREB_RSMSG &rcvdata)
{
	char databuf[DREBDATASIZE];
	
	if (rcvdata.data->head.cZip == 2)
	{
		memcpy(databuf,rcvdata.data->buffer,rcvdata.data->head.nLen);
		unsigned int dlen = DREBDATASIZE;
		if (!CBF_Tools::Uncompress((unsigned char *)rcvdata.data->buffer,dlen,(unsigned char *)databuf,rcvdata.data->head.nLen))
		{
			sprintf(rcvdata.data->buffer,"数据解压缩出错");
			rcvdata.data->head.nLen = strlen(rcvdata.data->buffer);
			rcvdata.data->head.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
			rcvdata.data->head.cRaflag = 1;
			rcvdata.data->head.cZip = 0;
			m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
			return ;
		}
	}
	S_BPC_DUFILE sfile;
	if (rcvdata.data->head.nLen  < sizeof(S_BPC_DUFILE))
	{
		m_pLog->LogBin(LOG_ERROR,__FILE__,__LINE__,"请求数据非下载数据",\
			rcvdata.data->buffer,rcvdata.data->head.nLen);
		sprintf(rcvdata.data->buffer,"请求数据非下载数据");
		rcvdata.data->head.nLen = strlen(rcvdata.data->buffer);
		rcvdata.data->head.a_Ainfo.a_nRetCode = ERR_MONITOR_DOWNLOAD;
		rcvdata.data->head.cRaflag = 1;
		rcvdata.data->head.cZip = 0;
		m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
		return ;
	}
	memcpy(&sfile,rcvdata.data->buffer,sizeof(S_BPC_DUFILE));
	struct_stat fstat;
	FSTAT(sfile.sFileName,&fstat);
	if (fstat.st_size <= 0)
	{
		sprintf(rcvdata.data->buffer,"文件[%s]不存在",sfile.sFileName);
		rcvdata.data->head.nLen = strlen(rcvdata.data->buffer);
		rcvdata.data->head.a_Ainfo.a_nRetCode = ERR_MONITOR_DOWNLOAD;
		rcvdata.data->head.cRaflag = 1;
		rcvdata.data->head.cZip = 0;
		m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
		return ;
	}
	int offset = atol(sfile.sFileOffset);
	if (offset >= fstat.st_size)
	{
		sprintf(rcvdata.data->buffer,"文件偏移量[%d]已大于等于文件大小",offset);
		rcvdata.data->head.nLen = strlen(rcvdata.data->buffer);
		rcvdata.data->head.a_Ainfo.a_nRetCode = ERR_MONITOR_DOWNLOAD;
		rcvdata.data->head.cRaflag = 1;
		rcvdata.data->head.cZip = 0;
		m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
		return ;
	}
	sprintf(sfile.sFileSize,"%011ld",fstat.st_size);
	FILE *fp = fopen(sfile.sFileName,"rb");
	if (fp == NULL)
	{
		sprintf(rcvdata.data->buffer,"文件[%s]不存在",sfile.sFileName);
		rcvdata.data->head.nLen = strlen(rcvdata.data->buffer);
		rcvdata.data->head.a_Ainfo.a_nRetCode = ERR_MONITOR_DOWNLOAD;
		rcvdata.data->head.cRaflag = 1;
		rcvdata.data->head.cZip = 0;
		m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
		return ;
	}
	fseek(fp,offset,1);
	int getlen = fread(rcvdata.data->buffer+sizeof(S_BPC_DUFILE),1,50*1024,fp);
	fclose(fp);
	sprintf(sfile.sFileOffset,"%011ld",getlen + offset);
	if ((getlen + offset) >= fstat.st_size)
	{
		sfile.cEndFlag = '1';
	}
	else
	{
		sfile.cEndFlag = '0';
	}
	memcpy(rcvdata.data->buffer,&sfile,sizeof(S_BPC_DUFILE));
	rcvdata.data->head.nLen = sizeof(S_BPC_DUFILE)+getlen;
	
	rcvdata.data->head.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.data->head.cRaflag = 1;
	rcvdata.data->head.cZip = 0;
	m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);

}

void CLinkThread::Upload(S_GATEDREB_RSMSG &rcvdata)
{
	char databuf[DREBDATASIZE];
	if (rcvdata.data->head.cZip == 2)
	{
		memcpy(databuf,rcvdata.data->buffer,rcvdata.data->head.nLen);
		unsigned int dlen = DREBDATASIZE;
		if (!CBF_Tools::Uncompress((unsigned char *)rcvdata.data->buffer,dlen,(unsigned char *)databuf,rcvdata.data->head.nLen))
		{
			sprintf(rcvdata.data->buffer,"数据解压缩出错");
			rcvdata.data->head.nLen = strlen(rcvdata.data->buffer);
			rcvdata.data->head.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
			rcvdata.data->head.cRaflag = 1;
			rcvdata.data->head.cZip = 0;
			m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
			return ;
		}
	}
	S_BPC_DUFILE sfile;
	if (rcvdata.data->head.nLen  <= sizeof(S_BPC_DUFILE))
	{
		m_pLog->LogBin(LOG_ERROR,__FILE__,__LINE__,"请求数据非上传数据",\
			rcvdata.data->buffer,rcvdata.data->head.nLen);
		sprintf(rcvdata.data->buffer,"请求数据非上传数据");
		rcvdata.data->head.nLen = strlen(rcvdata.data->buffer);
		rcvdata.data->head.a_Ainfo.a_nRetCode = ERR_MONITOR_UPLOAD;
		rcvdata.data->head.cRaflag = 1;
		rcvdata.data->head.cZip = 0;
		m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
		return ;
	}
	memcpy(&sfile,rcvdata.data->buffer,sizeof(S_BPC_DUFILE));
	m_pLog->LogBin(LOG_DEBUG,__FILE__,__LINE__,"上传文件",rcvdata.data->buffer,sizeof(S_BPC_DUFILE));
	FILE *fp=NULL;
	if (sfile.cBeginFlag == '1')
	{
		char filenamepath[301];
		sprintf(filenamepath,"%s",sfile.sFileName);
		int j= strlen(filenamepath)-1;
		for (;j>0;j--)
		{
			if ((filenamepath[j] == '/') ||  (filenamepath[j] == '\\'))
			{
				filenamepath[j] =0;
				CBF_Tools::MakeDir(filenamepath);
				break;
			}
		}
		fp = fopen(sfile.sFileName,"wb");
	}
	else
	{
		fp = fopen(sfile.sFileName,"ab");
	}
	if (fp == NULL)
	{
		sprintf(rcvdata.data->buffer,"文件[%s]打开出错",sfile.sFileName);
		rcvdata.data->head.nLen = strlen(rcvdata.data->buffer);
		rcvdata.data->head.a_Ainfo.a_nRetCode = ERR_MONITOR_UPLOAD;
		rcvdata.data->head.cRaflag = 1;
		rcvdata.data->head.cZip = 0;
		m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
		return ;
	}
	fwrite(rcvdata.data->buffer+sizeof(S_BPC_DUFILE),1,rcvdata.data->head.nLen-sizeof(S_BPC_DUFILE),fp);
	fclose(fp);
	if (sfile.cEndFlag == '1')
	{
		struct_stat fstat;
		FSTAT(sfile.sFileName,&fstat);
		if (atol(sfile.sFileSize) != fstat.st_size)
		{
			sprintf(rcvdata.data->buffer,"上传文件[%s]失败，大小[%d]不等于[%s]",sfile.sFileName,fstat.st_size,sfile.sFileSize);
			rcvdata.data->head.nLen = strlen(rcvdata.data->buffer);
			rcvdata.data->head.a_Ainfo.a_nRetCode = ERR_MONITOR_UPLOAD;
			rcvdata.data->head.cRaflag = 1;
			rcvdata.data->head.cZip = 0;
		m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
			return ;
		}
		sprintf(rcvdata.data->buffer,"上传文件[%s]成功",sfile.sFileName);
		rcvdata.data->head.nLen = strlen(rcvdata.data->buffer);
		rcvdata.data->head.a_Ainfo.a_nRetCode = SUCCESS;
		rcvdata.data->head.cRaflag = 1;
		rcvdata.data->head.cZip = 0;
		m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
		return ;
	}
	else
	{
		sprintf(rcvdata.data->buffer,"写入文件[%s]成功",sfile.sFileName);
		rcvdata.data->head.nLen = strlen(rcvdata.data->buffer);
		rcvdata.data->head.a_Ainfo.a_nRetCode = SUCCESS;
		rcvdata.data->head.cRaflag = 1;
		rcvdata.data->head.cZip = 0;
		m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
		return ;
	}
}

void CLinkThread::ProcessQuota(S_GATEDREB_RSMSG &rcvdata, S_CGATE_SMSG &data)
{
	S_CGATE_QUOTATION quo;
	quo.nVerietyId = rcvdata.data->head.s_Sinfo.s_nHook;
	quo.nDate = rcvdata.data->head.s_Sinfo.s_nSerial;
	//quo.nSerial = rcvdata.data->head.n_Ninfo.n_nNextNo;
	//n_nNextNo是short类型，还是用回s_nGateIndex字段吧
	quo.nSerial =  rcvdata.data->head.s_Sinfo.s_nGateIndex;
	
	if (m_pQuotation.IsNewQuotation(quo) >0 )
	{
		//放入队列，待发送
		m_pSendData->PushData(data,2);
		
	}
	else
	{
		m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"行情[%d %d %d]不是最新，丢弃",quo.nVerietyId,quo.nDate,quo.nSerial);
	}
	m_pMemPool->PoolFree(rcvdata.data);
	rcvdata.data = NULL;
}
