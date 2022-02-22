// LinkThread.cpp: implementation of the CLinkThread class.
//
//////////////////////////////////////////////////////////////////////

#include "LinkThread.h"
#include "dreberrcode.h"
#include "Bpc_Timer.h"
#include "BF_Xml.h"

extern CBpc_Timer      g_pBpcTime;
#define  DESENCRYPTKEY    "rd402wms"
#define  DESENCRYPTKEY3   "wms13711songfree"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern char g_VERSION[20];
CLinkThread::CLinkThread()
{
	m_bIsRunning = false;
	m_pRes = NULL;
	m_pSockMgr = NULL;
	m_pMemPool = NULL;
	m_pPoolData = NULL;
	m_pFuncTbl = NULL;
	m_bIsInit = false;
	m_nBegin=0;
	m_nScanBegin = 0;
	m_nEnd=1;
	m_sThreadName = "CLinkThread";
	m_bIsPingTimer = false;
	m_pLog = NULL;
}

CLinkThread::~CLinkThread()
{
	m_pRes = NULL;
	m_pSockMgr = NULL;
	m_pMemPool = NULL;
	m_pPoolData = NULL;
	m_pFuncTbl = NULL;
	m_pLog = NULL;
}

void CLinkThread::ExitThreadInstance()
{
	return ;
}
bool CLinkThread::InitThreadInstance()
{
	return true;
}

bool CLinkThread::SetGlobalVar(CGResource *res,CPoolDataMsg *pooldata,CBF_BufferPool *mempool,CSocketMgr *sockmgr,CFuncTbl *tbl)
{
	m_pRes = res;
	m_pSockMgr = sockmgr;
	m_pMemPool = mempool;
	m_pPoolData = pooldata;
	m_pFuncTbl = tbl;

	m_pLog = &(m_pRes->m_log);
	m_bIsInit = true;

	m_pTimer.Init(100,true);
	//节点路由定时器
	m_pTimer.SetTimer(0,5000,&CLinkThread::OnTimer,this); //设置定时器 
	
	

	return true;
}
int CLinkThread::Run()
{
	struct timeval tv;
	int result;
	int i,j;
	int maxfd=0;
	int ret;
	int lasttime=time(NULL);
	int lasttime2=time(NULL);
	while(!m_pRes->g_bToExit)
	{
		FD_ZERO(&m_rset);
		FD_ZERO(&m_wset);
#if defined(_WINDOWS)
		FD_ZERO(&m_eset);
#endif
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		m_nScanBegin = m_nBegin;
		//取得最大的socket并将socket加入到fd_set集合
		ret = GetMaxfdAddSelect(maxfd);
		
		if (m_nBegin == 0) //若有侦听socket则加入读集合
		{
			FD_SET(m_pSockMgr->at(0)->m_sock, &m_rset);//加入到读select集合
			if (m_pSockMgr->at(0)->m_sock > maxfd)
			{
				maxfd = m_pSockMgr->at(0)->m_sock;
			}
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
		
		if (result != 0) //事件发生,处理
		{
			OnEvent();
		}

		if (m_bIsPingTimer)
		{
			OnNoEvent();
			OnMonitor();
			m_bIsPingTimer = false;
		}

	}
	m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"停止侦听线程");
	return 0;
}
int CLinkThread::GetMaxfdAddSelect(int &maxfd)
{
	int ret=0;
	maxfd = 0;
	m_nScanBegin = m_nBegin;
	if (m_nBegin == 0) //如果有侦听则从1开始
	{
		m_nScanBegin = 1;
	}
	for (int i=m_nScanBegin;i<m_nEnd;i++)
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
	int j;
	int tmptime;
	if (m_nBegin == 0)
	{
		m_nScanBegin = 2;//略过侦听端口
	}
	for (j=m_nScanBegin; j<m_nEnd; j++)
	{
		if (m_pSockMgr->at(j)->m_sock != INVALID_SOCKET && m_pSockMgr->at(j)->m_nType == BPCSOCK_DREB)
		{
			tmptime = time(NULL) - m_pSockMgr->at(j)->m_nReadtime;
			if ( tmptime > m_pRes->g_nHeartRun && tmptime < m_pRes->g_nDisconnectTime)
			{
				OnPing(j);
			}
			else if (tmptime > m_pRes->g_nDisconnectTime)
			{
				OnClose(j,"连接长时间未用，断开",__FILE__,__LINE__);
			}
		}
		if (m_pSockMgr->at(j)->m_sock != INVALID_SOCKET && m_pSockMgr->at(j)->m_nType == BPCSOCK_BPU)
		{
			tmptime = time(NULL) - m_pSockMgr->at(j)->m_nReadtime;
			if (tmptime > m_pRes->g_nDisconnectTime)
			{
				OnClose(j,"连接长时间未用，断开",__FILE__,__LINE__);
			}
		}
		if (m_pSockMgr->at(j)->m_sock != INVALID_SOCKET)
		{
			//没有检查通过
			if (!m_pSockMgr->at(j)->m_bChecked)
			{
				//数据总线节点连接
				if (m_pSockMgr->at(j)->m_nType == BPCSOCK_DREB) 
				{
					if (time(NULL)-m_pSockMgr->at(j)->m_nReadtime >= m_pRes->g_nHeartRun)
					{
						if (m_pSockMgr->at(j)->NeedConnect())
						{
							m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"尝试连接到%s:%d未成功",m_pSockMgr->at(j)->m_sDrebIp.c_str(),m_pSockMgr->at(j)->m_nDrebPort);
							closesocket(m_pSockMgr->at(j)->m_sock);
							m_pSockMgr->at(j)->m_sock = INVALID_SOCKET;
						}
					}
				}// end if (m_pSockMgr->at(conindex)->m_nType == BPCSOCK_TODREB) //数据总线节点连接
				else if (m_pSockMgr->at(j)->m_nType == BPCSOCK_UNKNOW)
				{
					if (time(NULL)-m_pSockMgr->at(j)->m_nReadtime >= m_pRes->g_nHeartRun)
					{
						OnClose(j,"连接非法",__FILE__,__LINE__);
					} 
				}
				else 
				{
					if (time(NULL)-m_pSockMgr->at(j)->m_nReadtime >= m_pRes->g_nHeartRun)
					{
						OnClose(j,"协商超时",__FILE__,__LINE__);
					} 
				}
			}
		}// end if (m_pSockMgr->at(conindex)->m_sock != INVALID_SOCKET)
		if (m_pRes->g_bToExit)
		{
			return;
		}
		//若是数据总线节点连接，且没有连上则去连接
		if (m_pSockMgr->at(j)->m_nType == BPCSOCK_DREB )
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
	int ret;
	int i;
	m_nScanBegin = m_nBegin;
	if (m_nBegin == 0)
	{
		ret = FD_ISSET(m_pSockMgr->at(0)->m_sock,&m_rset);
		if (ret != 0) //如查是侦听端口有读的事件，则去调用accept
		{
			OnAccept(0);
		}
#ifdef UNIX

		ret = FD_ISSET(m_pSockMgr->at(1)->m_sock,&m_rset);
		if (ret != 0) //如查是侦听端口有读的事件，则去调用accept
		{
			OnAccept(1);
		}
		
#endif
		m_nScanBegin = m_nBegin + 2;
	}

	for (i= m_nScanBegin; i<m_nEnd; i++)
	{
		if (m_pSockMgr->at(i)->m_sock == INVALID_SOCKET)//未连接
		{
			if (m_pSockMgr->at(i)->m_nType == BPCSOCK_DREB   ) //若是数据总线节点连接，则去连接
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
}

void CLinkThread::OnWriteEvent(int conindex)
{
	int ret;
	if (m_pSockMgr->at(conindex)->m_sock == INVALID_SOCKET)
	{
		return;
	}
#if defined(_WINDOWS)
	ret = FD_ISSET(m_pSockMgr->at(conindex)->m_sock, &m_eset);
	if (ret != 0)
	{
		//主动连接其它的DREB
		if (m_pSockMgr->at(conindex)->m_nType == BPCSOCK_DREB)
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
			if (m_pSockMgr->at(conindex)->m_nType == BPCSOCK_DREB ) 
			{
				OnConnected(conindex);//去连接数据总线节点
			}
		}
		else if (m_pSockMgr->at(conindex)->m_bChecked)
		{
			OnSend(conindex);//发送数据
		}
	}
	else  if (m_pSockMgr->at(conindex)->m_bChecked) //socket无写事件
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
				if (m_pSockMgr->at(conindex)->m_nType == BPCSOCK_DREB)
				{
					OnConnected(conindex);//去连接数据总线节点
				}
			}
			else
			{
				if (m_pSockMgr->at(conindex)->m_nType == BPCSOCK_DREB)
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
	if (m_pSockMgr->at(conindex)->m_nType == BPCSOCK_DREB)
	{
		m_pSockMgr->at(conindex)->m_nPingTime = time(NULL);
		if (!ResetData(conindex,&m_pDataBuf))
		{
			return;
		}
		m_pDataBuf.sMsgBuf->sDBHead.cRaflag = 0;
		m_pDataBuf.sMsgBuf->sDBHead.cZip = 0;
		m_pDataBuf.sMsgBuf->sDBHead.nLen = 0;
		m_pDataBuf.sMsgBuf->sDBHead.cCmd = CMD_PING;
		m_pDataBuf.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
		//发送ping数据包
		m_pSockMgr->at(conindex)->SendMsg(m_pDataBuf);
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
	int ret;
	m_pSockMgr->at(conindex)->m_bChecked = false;

	m_pSockMgr->at(conindex)->m_nConntime = time(NULL);


//	m_tcpSocket.Create(AF_UNIX,false);//异步方式
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
		
		m_pDataBuf.sMsgBuf->sDBHead.cRaflag = 0;
		m_pDataBuf.sMsgBuf->sDBHead.cZip = 0;
		m_pDataBuf.sMsgBuf->sDBHead.cCmd = CMD_CONNECT;
		m_pDataBuf.sMsgBuf->sDBHead.cDrebAffirm = 0;
	
		
		m_pDataBuf.sMsgBuf->sDBHead.nLen = sizeof(S_CONNECT_DATA);
		S_CONNECT_DATA *data = (S_CONNECT_DATA *)m_pDataBuf.sMsgBuf->sBuffer;
		data->cConnectType = 1;

		data->nSvrMainId = m_pRes->g_nSvrMainId;
		data->cSvrPrivateId = m_pRes->g_nSvrPrivateId;
		sprintf(data->sVersion,"%s",g_VERSION);
		m_pDrebEndian.Endian2Comm((unsigned char *)&(data->nSvrMainId),sizeof(data->nSvrMainId));
		m_pDataBuf.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN+m_pDataBuf.sMsgBuf->sDBHead.nLen;
		m_pSockMgr->at(conindex)->SendMsg(m_pDataBuf,true);
		m_pSockMgr->at(conindex)->m_bChecked = true;
		return ;
	}
	return;
	
}
void CLinkThread::OnAccept(int conindex)
{
	char address[40];
	bzero(address,sizeof(address));
	SOCKET_HANDLE tsock=INVALID_SOCKET;
	int i;
	bool havefree=false;
	m_tcpSocket.AttachSocket(m_pSockMgr->at(conindex)->m_sock,"");
	tsock= m_tcpSocket.Accept(address,false);
	int num;
	num = 31;
	
	if (tsock != INVALID_SOCKET)
	{

		for (i=m_pRes->g_vDrebLinkInfo.size()+2; i<num; i++)
		{
			if ( m_pSockMgr->at(i)->m_sock == INVALID_SOCKET )
			{
				havefree = true;
				m_pSockMgr->at(i)->SetAccept(tsock,address);
				m_pSockMgr->at(i)->m_nType = SOCK_CLI;
				break;
			}
		}
		if (havefree)
		{
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"接收从%s的BPU连接socket[%d] index[%d]",address,tsock,i);
		}
		else
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"BPU连接过多，从%s的连接socket%d被拒绝",address,tsock);
			closesocket(tsock);
		}
	}
}

void CLinkThread::OnRecv(int conindex)
{
	int ret=0;
	int i=0;
	
	ret = m_pSockMgr->at(conindex)->OnRecv();	
	if (ret<=0)
	{
		OnClose(conindex,"远端关闭，接收数据不完整",__FILE__,__LINE__);
		return;
	}
	while (m_pSockMgr->at(conindex)->m_nRcvBufLen>0)
	{
//		m_pRes->PutTime();
		if (!ResetData(conindex,&m_pDataRcv))
		{
			return;
		}
		ret = m_pSockMgr->at(conindex)->GetRecvData(&m_pDataRcv);
		if (ret>0)
		{
			if (m_pSockMgr->at(conindex)->m_nType == BPCSOCK_DREB)
			{
				//接收dreb过来的数据
				if (m_pDataRcv.sMsgBuf->sDBHead.cCmd == CMD_PING) //响应PING应答
				{
					m_pDataRcv.sMsgBuf->sDBHead.cRaflag = 1;
					m_pSockMgr->at(conindex)->SendMsg(m_pDataRcv);
//					m_pRes->LogTime();
					return ;
				}
				//若是应答且为不可靠推送或全域广播消息，直接丢弃
				if (m_pDataRcv.sMsgBuf->sDBHead.cRaflag == 1 && (m_pDataRcv.sMsgBuf->sDBHead.cCmd == CMD_DPPUSH || m_pDataRcv.sMsgBuf->sDBHead.cCmd == CMD_DPABC))
				{
					m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"CMD_DPPUSH或CMD_DPABC的应答，丢弃");
					m_pMemPool->PoolFree(m_pDataRcv.sMsgBuf);
					m_pDataRcv.sMsgBuf = NULL;
//					m_pRes->LogTime();
					continue;
				}
				ProcessDreb(m_pDataRcv);
//				m_pRes->PutTime();
//				m_pRes->LogTime();
			}
			else //bpu连接过来的bpuisreq注册消息
			{
				if (m_pDataRcv.sMsgBuf->sBpcHead.cMsgType != MSG_BPUISREG)
				{
					OnClose(conindex,"BPU连接应先发MSG_BPUISREG消息",__FILE__,__LINE__);
//					m_pRes->LogTime();
					return ;
				}
				OnBpuIsReg(conindex,m_pDataRcv);
//				m_pRes->LogTime();
			}
			
		}
		else
		{
			m_pMemPool->PoolFree(m_pDataRcv.sMsgBuf);
			m_pDataRcv.sMsgBuf = NULL;
			break;
		}
	}
	
	return ;
}


bool CLinkThread::ResetData(int index,S_BPC_RSMSG *data)
{
	data->sMsgBuf = (PBPCCOMMSTRU )m_pMemPool->PoolMalloc();
	if (data->sMsgBuf == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"分配消息空间出错");
		m_pSockMgr->at(index)->OnClose("分配消息空间出错",__FILE__,__LINE__);
		return false;
	}
	data->sMsgBuf->sBpcHead.nConnectTime= m_pSockMgr->at(index)->m_nConntime;
	data->sMsgBuf->sBpcHead.nIndex = index;
	data->sMsgBuf->sBpcHead.nRtime = time(NULL);
	
	return true;
}


void CLinkThread::ProcessDreb(S_BPC_RSMSG &rcvdata)
{
	int flag=0;
	if (rcvdata.sMsgBuf->sDBHead.cRaflag == 0) //请求
	{
		S_FUNCINFO_TBL fn;
		if (rcvdata.sMsgBuf->sDBHead.cNextFlag == 2 ) //取后续数据
		{
			GetBpcNext(rcvdata);
			return;
		}
		if (rcvdata.sMsgBuf->sDBHead.cNextFlag == 3) //bpu取后续包
		{
			m_pLog->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"收到BPU取后续包请求数据");
			rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN+rcvdata.sMsgBuf->sDBHead.nLen;
			rcvdata.sMsgBuf->sBpcHead.cMsgType = MSG_GETNEXT;
			m_pPoolData->PushData(rcvdata);
			return ;
		}
		switch (rcvdata.sMsgBuf->sDBHead.cCmd)
		{
			case CMD_PING:
				rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
				rcvdata.sMsgBuf->sDBHead.nLen = 0;
				rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
				m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
				break;
			case CMD_DPCALL:
			case CMD_DPACALL:
			case CMD_DPBC:
			case CMD_DPABC:
			case CMD_DPPUSH:
			case CMD_DPPOST:
				m_pLog->LogBin(LOG_DEBUG+2,__FILE__,__LINE__,"收到请求数据",rcvdata.sMsgBuf->sBuffer,rcvdata.sMsgBuf->sDBHead.nLen);
//				m_pRes->PutTime();

// 				//测试代码
// 				rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
// 				sprintf(rcvdata.sMsgBuf->sBuffer,"测试bpc直接返回");
// 				rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
// 				rcvdata.sMsgBuf->sDBHead.cZip=0;
// 				m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
// 
// 				return;
				//放入队列，由消息线程取交易码信息，发给对应的BPU组
				m_pPoolData->PushData(rcvdata);
				break;	
			case CMD_MONITOR_BPC:
				CallBpcLocalFunc(rcvdata);
				break;	
			default:
				m_pMemPool->PoolFree(rcvdata.sMsgBuf);
				rcvdata.sMsgBuf = NULL;
		}
		return;
		
	}
	else  //是DREB的应答
	{
		switch (rcvdata.sMsgBuf->sDBHead.cCmd)
		{
			case CMD_PING:
				m_pMemPool->PoolFree(rcvdata.sMsgBuf);
				rcvdata.sMsgBuf = NULL;
				break;
			case CMD_CONNECT:
				if (rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode != SUCCESS)
				{
					m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"服务连接失败 %d",rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode);
					m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->OnClose("DREB返回服务连接失败",__FILE__,__LINE__);
					m_pMemPool->PoolFree(rcvdata.sMsgBuf);
					rcvdata.sMsgBuf = NULL;
					return;
				}
				m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->m_bChecked = true;
				m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->m_nDrebId = rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nNodeId;
				m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->m_nDrebPrivateId = rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_cNodePrivateId;
				
				memcpy(&(m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->m_head),&(rcvdata.sMsgBuf->sDBHead),DREBHEADLEN);
				if (m_pRes->g_nRegisterMode != REGISTERMODE_NONE)
				{
					RegisterDreb(rcvdata.sMsgBuf->sBpcHead.nIndex);
				}
				
				m_pMemPool->PoolFree(rcvdata.sMsgBuf);
				rcvdata.sMsgBuf = NULL;
				
				break;
			case CMD_DEL_SVR:
				m_pMemPool->PoolFree(rcvdata.sMsgBuf);
				rcvdata.sMsgBuf = NULL;
				break;
			case CMD_REGSERVICE:
				m_pMemPool->PoolFree(rcvdata.sMsgBuf);
				rcvdata.sMsgBuf = NULL;
				break;	
			case CMD_DPCALL:
			case CMD_DPACALL:
			case CMD_DPBC:
			case CMD_DPABC:
			case CMD_DPPUSH:
			case CMD_DPPOST:
				//bpc外调，直接应答给bpu
				if (m_pSockMgr->AffirmIndex(rcvdata.sMsgBuf->sDBHead.b_Cinfo.b_cIndex))
				{
					//判断连接时间戳是否相同 ,b_Cinfo里的b_cIndex存放bpu连接index,b_nSerial存放bpu连接index时间戳
					if (rcvdata.sMsgBuf->sDBHead.b_Cinfo.b_nSerial == m_pSockMgr->at(rcvdata.sMsgBuf->sDBHead.b_Cinfo.b_cIndex)->m_nConntime)
					{
						rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN + rcvdata.sMsgBuf->sDBHead.nLen;
						rcvdata.sMsgBuf->sBpcHead.nBpuIndex = rcvdata.sMsgBuf->sDBHead.b_Cinfo.b_cIndex;
						rcvdata.sMsgBuf->sBpcHead.nConnectTime = m_pSockMgr->at(rcvdata.sMsgBuf->sDBHead.b_Cinfo.b_cIndex)->m_nConntime;
						//应答回到BPC，肯定是extcall了
						rcvdata.sMsgBuf->sBpcHead.cMsgType = MSG_EXTCALL;
						m_pSockMgr->at(rcvdata.sMsgBuf->sDBHead.b_Cinfo.b_cIndex)->SendMsg(rcvdata);
						return;
					}
					m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"BPU外调应答时连接时间戳不符 index[%d]的消息[%s]  DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 源[%d %d] czip[%d] 业务数据长度[%d]",\
						rcvdata.sMsgBuf->sDBHead.b_Cinfo.b_cIndex,m_pRes->GetBpcMsgType(rcvdata.sMsgBuf->sBpcHead.cMsgType).c_str(),\
						m_pRes->GetDrebCmdType(rcvdata.sMsgBuf->sDBHead.cCmd).c_str(),rcvdata.sMsgBuf->sDBHead.cNextFlag,\
						rcvdata.sMsgBuf->sDBHead.cRaflag,rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
							rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
							rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
							rcvdata.sMsgBuf->sDBHead.cZip,rcvdata.sMsgBuf->sDBHead.nLen);
					m_pRes->m_pBpcCallDataLog.LogDreb(LOG_ERROR,rcvdata.sMsgBuf,true);
//					g_pBpcLog.LogBpc(LOG_ERROR,rcvdata.sMsgBuf,true);
				}
				else
				{
					m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"BPU外调应答时连接index非法 index[%d]的消息[%s]  DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 源[%d %d] cZip[%d] 业务数据长度[%d]",\
						rcvdata.sMsgBuf->sDBHead.b_Cinfo.b_cIndex,m_pRes->GetBpcMsgType(rcvdata.sMsgBuf->sBpcHead.cMsgType).c_str(),\
						m_pRes->GetDrebCmdType(rcvdata.sMsgBuf->sDBHead.cCmd).c_str(),rcvdata.sMsgBuf->sDBHead.cNextFlag,\
						rcvdata.sMsgBuf->sDBHead.cRaflag,rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
						rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
						rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
						rcvdata.sMsgBuf->sDBHead.cZip,rcvdata.sMsgBuf->sDBHead.nLen);
					m_pRes->m_pBpcCallDataLog.LogDreb(LOG_ERROR,rcvdata.sMsgBuf,true);
//					g_pBpcLog.LogBpc(LOG_ERROR,rcvdata.sMsgBuf,true);
				}
				m_pMemPool->PoolFree(rcvdata.sMsgBuf);
				rcvdata.sMsgBuf = NULL;
				break;	
			default:
				m_pMemPool->PoolFree(rcvdata.sMsgBuf);
				rcvdata.sMsgBuf = NULL;
				break;
		}
		return;
	}
}


void CLinkThread::CallBpcLocalFunc(S_BPC_RSMSG &rcvdata)
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
	switch (rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo)
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
		case BPCMONI_SETBPU:
			MonitorSetBpu(rcvdata);
			break;
		case BPCMONI_SETMODE:
			MonitorSetMode(rcvdata);
			break;
		case BPCMONI_STOPBPU:
			MonitorStopBpu(rcvdata);
			break;
		case BPCMONI_STARTBPU:
			MonitorStartBpu(rcvdata);
			break;
		case BPCMONI_DOWNLOAD:
			Download(rcvdata);
			break;
		case BPCMONI_UPLOAD:
			Upload(rcvdata);
			break;
		default:
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_FUNCNOTFUND;
			rcvdata.sMsgBuf->sDBHead.nLen = 0;
			rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
			m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
			break;
	}
	

}
void CLinkThread::RegisterDreb(int index)
{
	S_BPC_RSMSG rcvdata;
	S_FUNCINFO_TBL fc;
	S_SERVICEREG_MSG *data=NULL;
	bool bRet;
	int  offset;
	unsigned int *func=NULL;
	if (m_pSockMgr->at(index)->m_sock == INVALID_SOCKET || !m_pSockMgr->at(index)->m_bChecked )
	{
		return;
	}
	rcvdata.sMsgBuf = (PBPCCOMMSTRU )m_pMemPool->PoolMalloc();
	if (rcvdata.sMsgBuf == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"分配消息空间出错");
		return ;
	}
	//
	memset(rcvdata.sMsgBuf,0,sizeof(BPCCOMMSTRU));
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"开始向DREB注册服务功能 %s-%d",m_pSockMgr->at(index)->m_sDrebIp.c_str(),m_pSockMgr->at(index)->m_nDrebPort);
	rcvdata.sMsgBuf->sDBHead.cCmd = CMD_REGSERVICE;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 0;
	rcvdata.sMsgBuf->sDBHead.cNextFlag = 0;
	rcvdata.sMsgBuf->sDBHead.cZip = 0;

	vector<S_FUNCINFO_TBL>vfunclist;
	if (m_pFuncTbl->GetAllFunc(vfunclist)<1)
	{
		m_pMemPool->PoolFree(rcvdata.sMsgBuf);
		m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"没有注册功能");
		rcvdata.sMsgBuf = NULL;
		return;
	}
	
	for (int i=0 ; i< vfunclist.size() ; i++)
	{
		if (i == 0)
		{
			data = (S_SERVICEREG_MSG *)rcvdata.sMsgBuf->sBuffer;
			data->nSvrMainId = m_pRes->g_nSvrMainId;
			m_pDrebEndian.Endian2Comm((unsigned char *)&(data->nSvrMainId),sizeof(data->nSvrMainId));
			data->cSvrPrivateId = m_pRes->g_nSvrPrivateId;
			data->nFuncNum = 1;
			data->nFuncNo = vfunclist[i].nFuncNo;
			m_pDrebEndian.Endian2Comm((unsigned char *)&(data->nFuncNo),sizeof(data->nFuncNo));
			offset = sizeof(S_SERVICEREG_MSG);
		}
		else
		{
			func = (unsigned int *)(rcvdata.sMsgBuf->sBuffer+offset);
			*func = vfunclist[i].nFuncNo;
			m_pDrebEndian.Endian2Comm((unsigned char *)func,sizeof(unsigned int));
			data->nFuncNum++;
			offset += sizeof(unsigned int);
			if (offset> BPUDATASIZE-30)
			{
				m_pMemPool->PoolFree(rcvdata.sMsgBuf);
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"注册功能太多,一个数据包无法在DREB上注册，请减少功能数");
				rcvdata.sMsgBuf = NULL;
				return;
			}
		}
	}
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"总共有%d个功能注册",data->nFuncNum);
	m_pDrebEndian.Endian2Comm((unsigned char *)&(data->nFuncNum),sizeof(data->nFuncNum));
	rcvdata.sMsgBuf->sDBHead.nLen = offset;
	rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN + rcvdata.sMsgBuf->sDBHead.nLen;

	m_pLog->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"连接注册发送的数据 长度=%d",rcvdata.sMsgBuf->sDBHead.nLen);

	m_pSockMgr->at(index)->SendMsg(rcvdata);
}

void CLinkThread::GetBpcNext(S_BPC_RSMSG &rcvdata)
{
	char sNextFile[400];
#ifdef _WINDOWS
	sprintf(sNextFile,"%s\\nxt\\%ld.nxt",m_pRes->g_sCurPath,rcvdata.sMsgBuf->sDBHead.n_Ninfo.n_nNextNo);
#else
	sprintf(sNextFile,"%s/nxt/%ld.nxt",m_pRes->g_sCurPath,rcvdata.sMsgBuf->sDBHead.n_Ninfo.n_nNextNo);
#endif
	FILE *fp = fopen(sNextFile,"rb");
	if (fp == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"后续包文件[%s]不存在",sNextFile);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_NEXTFILENOT;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cNextFlag = 0;
		rcvdata.sMsgBuf->sDBHead.nLen = 0;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
		return;
	}
	fseek(fp,rcvdata.sMsgBuf->sDBHead.n_Ninfo.n_nNextOffset,1);
	int ret =  fread(rcvdata.sMsgBuf->sBuffer,1,BPUDATASIZE,fp);
	fclose(fp);
	struct_stat fs;
	FSTAT(sNextFile,&fs);
	
	rcvdata.sMsgBuf->sDBHead.nLen = ret;
	rcvdata.sMsgBuf->sDBHead.n_Ninfo.n_nNextOffset+=ret;
	if (ret < BPUDATASIZE)
	{
		rcvdata.sMsgBuf->sDBHead.cNextFlag = 0; //没有后续的了
	}
	else
	{
		if (rcvdata.sMsgBuf->sDBHead.n_Ninfo.n_nNextOffset == fs.st_size) //正好是尾
		{
			rcvdata.sMsgBuf->sDBHead.cNextFlag = 0;//没有后续的了
		}
		else
		{
			rcvdata.sMsgBuf->sDBHead.cNextFlag = 2;//有后续
		}
	}

	rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
	rcvdata.sMsgBuf->sDBHead.cZip = 0;
	m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
	return;
}

void CLinkThread::LogDrebHead(int loglevel, DREB_HEAD head, const char *msg, const char *filename, int fileline)
{
	bzero(m_headMsg,sizeof(m_headMsg));
	sprintf(m_headMsg,"%s DREBHEAD信息 cZip=%d cCmd=%s cRaflag=%d cNextFlag=%d cDrebAffirm=%d s_nNodeId=%d \
		s_cNodePrivateId=%d s_nSvrMainId=%d s_cSvrPrivateId=%d s_nHook=%d s_nSerial=%d \
		s_nDrebSerial=%d s_nIndex=%d d_nNodeId=%d d_cNodePrivateId=%d d_nSvrMainId=%d \
		d_cSvrPrivateId=%d d_nServiceNo=%d a_nNodeId=%d a_cNodePrivateId=%d a_nRetCode=%d \
		n_nNextNo=%d n_nNextOffset=%d b_nSerial=%d b_cIndex=%d nLen=%d","%s",head.cZip,m_pRes->GetDrebCmdType(head.cCmd).c_str(),\
		head.cRaflag,head.cNextFlag,head.cDrebAffirm,head.s_Sinfo.s_nNodeId,head.s_Sinfo.s_cNodePrivateId,\
		head.s_Sinfo.s_nSvrMainId,head.s_Sinfo.s_cSvrPrivateId,head.s_Sinfo.s_nHook,head.s_Sinfo.s_nSerial,\
		head.s_Sinfo.s_nDrebSerial,head.s_Sinfo.s_nIndex,head.d_Dinfo.d_nNodeId,head.d_Dinfo.d_cNodePrivateId,\
		head.d_Dinfo.d_nSvrMainId,head.d_Dinfo.d_cSvrPrivateId,head.d_Dinfo.d_nServiceNo,\
		head.a_Ainfo.a_nNodeId,head.a_Ainfo.a_cNodePrivateId,head.a_Ainfo.a_nRetCode,\
		head.n_Ninfo.n_nNextNo,head.n_Ninfo.n_nNextOffset,head.b_Cinfo.b_nSerial,\
		head.b_Cinfo.b_cIndex,head.nLen);
	m_pLog->LogMp(loglevel,filename,fileline,m_headMsg,msg);
}

void CLinkThread::MonitorInfo(S_BPC_RSMSG &rcvdata)
{
	int i;
	CBF_Xml xmlpack;
	xmlpack.SetNodeValueByPath("Monitor/BPC公共节点",false,(int)m_pRes->g_nSvrMainId);
	xmlpack.SetNodeValueByPath("Monitor/BPC私有节点",false,(int)m_pRes->g_nSvrPrivateId);
	xmlpack.SetNodeValueByPath("Monitor/启动时间",false,m_pRes->g_sStartDateTime,false);
	//连接信息
	for (i=0 ; i < m_pRes->g_vDrebLinkInfo.size() ; i++)
	{
		CXmlNode *connnode = xmlpack.AddNodeByPath("Monitor/DREB连接信息",false,"连接",false,"");
		if (connnode == NULL)
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"AddNodeByPath 出错");
			continue;
		}
		connnode->SetAttribute("ip",false,m_pRes->g_vDrebLinkInfo[i].sIp);
		connnode->SetAttribute("port",false,(int)m_pRes->g_vDrebLinkInfo[i].nPort);
		connnode->SetAttribute("index",false,i+2);
		connnode->SetAttribute("公共节点",false,m_pSockMgr->at(i+2)->m_nDrebId);
		connnode->SetAttribute("私有节点",false,m_pSockMgr->at(i+2)->m_nDrebPrivateId);
		if (m_pSockMgr->at(i+2)->m_sock == INVALID_SOCKET)
		{
			connnode->SetAttribute("status",false,"未连接");
		}
		else if (m_pSockMgr->at(i+2)->m_bChecked)
		{
			connnode->SetAttribute("status",false,"正常");
		}
		else
		{
			connnode->SetAttribute("status",false,"正在连接");
		}
	}
	
	//BPU信息
	int k;
	int connNum=0;
	xmlpack.AddNodeByPath("Monitor",false,"BPU列表",false,"");
	for (i=0 ; i<m_pRes->g_vBpuGroupInfo.size(); i++)
	{
		connNum=0;
		for (k=m_pRes->g_vBpuGroupInfo[i].g_nBeginSocketIndex ; k < m_pRes->g_vBpuGroupInfo[i].g_nEndSocketIndex; k++)
		{
			if (m_pSockMgr->at(k)->m_sock != INVALID_SOCKET)
			{
				connNum++;
			}
		}
		CXmlNode *bpunode = xmlpack.AddNodeByPath("Monitor/BPU列表",false,m_pRes->g_vBpuGroupInfo[i].g_sBpuGroupName.c_str(),true,"");
		bpunode->SetAttribute("配置BPU数",false,m_pRes->g_vBpuGroupInfo[i].g_nBpuNum);
		bpunode->SetAttribute("实际BPU数",false,connNum);
		bpunode->SetAttribute("分配模式",false,m_pRes->g_vBpuGroupInfo[i].g_nDispatchMode);
		bpunode->SetAttribute("自动启动",false,m_pRes->g_vBpuGroupInfo[i].g_bIsAutoLoadBu);
		bpunode->SetAttribute("开始连接索引",false,m_pRes->g_vBpuGroupInfo[i].g_nBeginSocketIndex);
		bpunode->SetAttribute("结束连接索引",false,m_pRes->g_vBpuGroupInfo[i].g_nEndSocketIndex);
	}

	int len = DREBDATASIZE;
	if (!xmlpack.ToBuffer(rcvdata.sMsgBuf->sBuffer,len))
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"缓冲不足，请下载bpcinfo.txt查看");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"缓冲不足");
		xmlpack.ToFile("bpcinfo.txt");
	}
	else
	{
		rcvdata.sMsgBuf->sDBHead.nLen = len;
	}

	rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
	rcvdata.sMsgBuf->sDBHead.cZip = 0;
	m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
}

void CLinkThread::MonitorHost(S_BPC_RSMSG &rcvdata)
{
	CBF_Xml xmlpack;
	xmlpack.SetNodeValueByPath("Monitor/BPC公共节点",false,(int)m_pRes->g_nSvrMainId);
	xmlpack.SetNodeValueByPath("Monitor/BPC私有节点",false,(int)m_pRes->g_nSvrPrivateId);
	xmlpack.SetNodeValueByPath("Monitor/启动时间",false,m_pRes->g_sStartDateTime,false);
	
	int total;
	int unused;
	m_pMemPool->PoolInfo(total,unused);
	xmlpack.SetNodeAttribute("Monitor/内存缓冲池",false,"总分配",false,total );
	xmlpack.SetNodeAttribute("Monitor/内存缓冲池",false,"未使用",false,unused );
	xmlpack.SetNodeAttribute("Monitor/内存缓冲池",false,"每块大小",false,(int)BPCBUFSIZE );
	//取主机信息
	S_MONITOR_HOST hf;
	V_MonitorDiskInfo di;
	g_pBpcTime.GetHostInfo(hf,di);
	
	xmlpack.SetNodeValueByPath("Monitor/主机资源/CPU",false,(int)hf.nCpuRate);
	
	
	xmlpack.SetNodeValueByPath("Monitor/主机资源/内存",false,"",false);
	xmlpack.SetNodeAttribute("Monitor/主机资源/内存",false,"总大小M",false,(int)hf.nTotalMemory);
	xmlpack.SetNodeAttribute("Monitor/主机资源/内存",false,"使用M",false,(int)hf.nUsedMemory);
	
	for (int i=0 ; i<di.size() ; i++)
	{
		CXmlNode *disknode = xmlpack.AddNodeByPath("Monitor/主机资源/磁盘列表",false,"磁盘",false,"");
		if (disknode != NULL)
		{
			disknode->SetAttribute("名称",false,di[i].sDiskName);
			disknode->SetAttribute("总大小M",false,(int)di[i].nTotalSpace);
			disknode->SetAttribute("使用大小M",false,(int)di[i].nUsedSpace);
		}
	}
	
	int len = DREBDATASIZE;
	if (!xmlpack.ToBuffer(rcvdata.sMsgBuf->sBuffer,len))
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"缓冲不足，请下载bpchost.txt查看");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"缓冲不足");
		xmlpack.ToFile("bpchost.txt");
	}
	else
	{
		rcvdata.sMsgBuf->sDBHead.nLen = len;
	}
	
	rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
	rcvdata.sMsgBuf->sDBHead.cZip = 0;
	m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
}

void CLinkThread::MonitorBpu(S_BPC_RSMSG &rcvdata)
{
	int i,k;
	int connNum=0;
	CBF_Xml xmlpack;
	xmlpack.SetNodeValueByPath("Monitor/BPC公共节点",false,(int)m_pRes->g_nSvrMainId);
	xmlpack.SetNodeValueByPath("Monitor/BPC私有节点",false,(int)m_pRes->g_nSvrPrivateId);
	xmlpack.SetNodeValueByPath("Monitor/启动时间",false,m_pRes->g_sStartDateTime,false);
	//BPU信息
	xmlpack.AddNodeByPath("Monitor",false,"BPU列表",false,"");
	for (i=0 ; i<m_pRes->g_vBpuGroupInfo.size(); i++)
	{
		connNum=0;
		for (k=m_pRes->g_vBpuGroupInfo[i].g_nBeginSocketIndex ; k < m_pRes->g_vBpuGroupInfo[i].g_nEndSocketIndex; k++)
		{
			if (m_pSockMgr->at(k)->m_sock != INVALID_SOCKET)
			{
				connNum++;
			}
		}
		CXmlNode *bpunode = xmlpack.AddNodeByPath("Monitor/BPU列表",false,m_pRes->g_vBpuGroupInfo[i].g_sBpuGroupName.c_str(),true,"");
		if (bpunode != NULL)
		{
			bpunode->SetAttribute("组名",false,m_pRes->g_vBpuGroupInfo[i].g_sBpuGroupName);
			bpunode->SetAttribute("配置BPU数",false,m_pRes->g_vBpuGroupInfo[i].g_nBpuNum);
			bpunode->SetAttribute("实际BPU数",false,connNum);
			bpunode->SetAttribute("心跳应答",false,m_pRes->g_vBpuGroupInfo[i].g_bIsAnsPing);
			bpunode->SetAttribute("分配模式",false,m_pRes->g_vBpuGroupInfo[i].g_nDispatchMode);
			bpunode->SetAttribute("自动启动",false,m_pRes->g_vBpuGroupInfo[i].g_bIsAutoLoadBu);
			bpunode->SetAttribute("程序名",false,m_pRes->g_vBpuGroupInfo[i].g_sBuPrgName);
			bpunode->SetAttribute("程序目录",false,m_pRes->g_vBpuGroupInfo[i].g_sBuPrgPath);
			bpunode->SetAttribute("运行目录",false,m_pRes->g_vBpuGroupInfo[i].g_sBuRunPath);
			bpunode->SetAttribute("运行参数",false,m_pRes->g_vBpuGroupInfo[i].g_sBuPrgPara);
			bpunode->SetAttribute("开始连接索引",false,m_pRes->g_vBpuGroupInfo[i].g_nBeginSocketIndex);
			bpunode->SetAttribute("结束连接索引",false,m_pRes->g_vBpuGroupInfo[i].g_nEndSocketIndex);
			
		}
	}
	
	int len = DREBDATASIZE;
	if (!xmlpack.ToBuffer(rcvdata.sMsgBuf->sBuffer,len))
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"缓冲不足，请下载bpcbpu.txt查看");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"缓冲不足");
		xmlpack.ToFile("bpcbpu.txt");
	}
	else
	{
		rcvdata.sMsgBuf->sDBHead.nLen = len;
	}
	
	rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
	rcvdata.sMsgBuf->sDBHead.cZip = 0;
	m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
}

void CLinkThread::MonitorReport(S_BPC_RSMSG &rcvdata)
{
	CBF_Xml xmlpack;
	xmlpack.SetNodeValueByPath("Monitor/BPC公共节点",false,(int)m_pRes->g_nSvrMainId);
	xmlpack.SetNodeValueByPath("Monitor/BPC私有节点",false,(int)m_pRes->g_nSvrPrivateId);
	xmlpack.SetNodeValueByPath("Monitor/启动时间",false,m_pRes->g_sStartDateTime,false);
	char tmpchar[100];
	char nodename[200];
	vector<S_FUNCINFO_TBL> funclist;
	m_pFuncTbl->GetAllFuncByGroup(funclist);
	
	xmlpack.SetNodeValueByPath("Monitor/BPU交易/交易数",false,(int)funclist.size());

	std::string bpugroup;
	int txnum=0;
	bzero(nodename,sizeof(nodename));
	for (int i=0 ; i<funclist.size() ; i++)
	{
		if (bpugroup.compare(funclist[i].sBpuGroupName) !=0)
		{
			if (strlen(nodename) >0)
			{
				xmlpack.SetNodeAttribute(nodename,false,"交易数",false,txnum);
			}
			sprintf(nodename,"Monitor/BPU交易/%s",funclist[i].sBpuGroupName);
			bpugroup = funclist[i].sBpuGroupName;
			txnum = 0;
		}
		txnum++;
		sprintf(tmpchar,"%d",funclist[i].nFuncNo);
		CXmlNode *bpunode = xmlpack.AddNodeByPath(nodename,false,tmpchar,false,"",false);
		if (bpunode != NULL)
		{
			bpunode->SetAttribute("名称",false,funclist[i].sFuncName);
			bpunode->SetAttribute("优先级",false,(int)funclist[i].cPrio);
			bpunode->SetAttribute("调用次数",false,(int)funclist[i].nCallNum);
		}
	}
	xmlpack.SetNodeAttribute(nodename,false,"交易数",false,txnum);

	int len = DREBDATASIZE;
	if (!xmlpack.ToBuffer(rcvdata.sMsgBuf->sBuffer,len))
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"缓冲不足，请下载bpureport.txt查看");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"缓冲不足");
		xmlpack.ToFile("bpureport.txt");
	}
	else
	{
		rcvdata.sMsgBuf->sDBHead.nLen = len;
	}
	rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
	rcvdata.sMsgBuf->sDBHead.cZip = 0;
	m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
}

void CLinkThread::MonitorSetBpu(S_BPC_RSMSG &rcvdata)
{
	//设置BPU数目，启动模式，是否保存至配置文件
// 	<Monitor>
// 		<SetBpu mode="0" remark="0只更改内存数据1同时更新到配置文件">
//         <bpu1>
// 				<发送明文 remark="由BPU解压缩解密0,明文发给BPU1">0</发送明文>
// 				<心跳应答 remark="当为c/c++多bpu时，不用应答心跳消息">0</心跳应答>
// 				<分配模式 remark="0表示空闲分配，1表示BPU数只能为1，一直分配">0</分配模式>
// 				<可启动的BPU数 remark="即可以处理业务的单元数目，当请求分配模式为1时只能为1">1</可启动的BPU数>
// 				<AUTOLOADBPU use="0" remark="自动将BPU程序启动1，否则为0">
// 					<BPU程序名 remark="BU程序的名称，不带路径">bfBPUd.exe</BPU程序名>
// 					<BPU程序路径 remark="绝对或相对路径">D:\BUBASE\bin</BPU程序路径>
// 					<BPU运行目录 remark="绝对或相对目录">D:\BUBASE\bin</BPU运行目录>
// 					<BPU启动参数 remark="启动BPU所需的参数">D:\BUBASE\bin\bfBPU.xml</BPU启动参数>
// 				</AUTOLOADBPU>
//         </bpu1>
// 		</SetBpu>
// 	</Monitor>
	if (rcvdata.sMsgBuf->sDBHead.cZip != 0)
	{
		if (!UnzipBuf(*rcvdata.sMsgBuf))
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"数据解压缩出错");
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
			return ;
		}
	}
	CBF_Xml  xmlpack;
	if (!xmlpack.FromBuffer(rcvdata.sMsgBuf->sBuffer))
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"请求数据非xml");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
		return ;
	}
	CXmlNode *setnode = xmlpack.GetNodeByPath("Monitor/SetBpu",false);
	if (setnode == NULL)
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"xml无[Monitor/SetBpu]节点");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
		return ;
	}
	int savemode=0;
	if (xmlpack.GetNodeAttribute("Monitor/SetBpu",false,"mode",false,savemode) == NULL)
	{
		savemode =0;//无属性默认为0
	}
	std::string bpugroupname;
	int i;
	int bpuindex=-1;
	char tmpchar[200];
	char tmpcharconf[200];
	bool isfind=false;
	std::string namevalue;
	if (savemode == 1)
	{
		CBF_Date_Time dt;
		sprintf(tmpchar,"%s.%s%s",m_pRes->m_sBpcParaFileName.c_str(),dt.ToStringD8().c_str(),dt.ToStringT6().c_str());
		m_pRes->m_xmlconf.ToFile(tmpchar);
	}
	
	
	CXmlNode *bpugnode = (CXmlNode *)setnode->GetFirstChild();
	while (bpugnode != NULL)
	{
		bpugroupname = bpugnode->GetNodeName();
		isfind=false;
		for (i=0; i< m_pRes->g_vBpuGroupInfo.size() ; i++)
		{
			if (bpugroupname.compare(m_pRes->g_vBpuGroupInfo[i].g_sBpuGroupName) == 0)
			{
				isfind = true;
				bpuindex = i;
				break;
			}
		}
		if (isfind)
		{
			//找到了
			sprintf(tmpchar,"Monitor/SetBpu/%s/发送明文",bpugroupname.c_str());
			if (xmlpack.GetNodeValueByPath(tmpchar,false,namevalue) != NULL)
			{
				if (savemode == 1) //更新配置文件
				{
					sprintf(tmpcharconf,"package/head/BPU配置/BPU组/%s/发送明文",bpugroupname.c_str());
					m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str());
				}
				//更新内存
				if (atoi(namevalue.c_str()) == 1)
				{
					m_pRes->g_vBpuGroupInfo[bpuindex].g_bIsUnencrypt  = true;
				}
				else
				{
					m_pRes->g_vBpuGroupInfo[bpuindex].g_bIsUnencrypt  = false;
				}
			}
			sprintf(tmpchar,"Monitor/SetBpu/%s/心跳应答",bpugroupname.c_str());
			if (xmlpack.GetNodeValueByPath(tmpchar,false,namevalue) != NULL)
			{
				if (savemode == 1) //更新配置文件
				{
					sprintf(tmpcharconf,"package/head/BPU配置/BPU组/%s/心跳应答",bpugroupname.c_str());
					m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str());
				}
				//更新内存
				if (atoi(namevalue.c_str()) == 1)
				{
					m_pRes->g_vBpuGroupInfo[bpuindex].g_bIsAnsPing  = true;
				}
				else
				{
					m_pRes->g_vBpuGroupInfo[bpuindex].g_bIsAnsPing  = false;
				}
			}
			sprintf(tmpchar,"Monitor/SetBpu/%s/分配模式",bpugroupname.c_str());
			if (xmlpack.GetNodeValueByPath(tmpchar,false,namevalue) != NULL)
			{
				if (savemode == 1) //更新配置文件
				{
					sprintf(tmpcharconf,"package/head/BPU配置/BPU组/%s/分配模式",bpugroupname.c_str());
					m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str());
				}
				//更新内存
				m_pRes->g_vBpuGroupInfo[bpuindex].g_nDispatchMode = atoi(namevalue.c_str());
			}
			sprintf(tmpchar,"Monitor/SetBpu/%s/可启动的BPU数",bpugroupname.c_str());
			if (xmlpack.GetNodeValueByPath(tmpchar,false,namevalue) != NULL)
			{
				if (m_pRes->g_vBpuGroupInfo[bpuindex].g_nDispatchMode == 1)
				{
					namevalue = "1";
				}
				//更新内存
				m_pRes->g_vBpuGroupInfo[bpuindex].g_nBpuNum  = atoi(namevalue.c_str());
				
				if (savemode == 1) //更新配置文件
				{
					sprintf(tmpcharconf,"package/head/BPU配置/BPU组/%s/可启动的BPU数",bpugroupname.c_str());
					m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str());
				}
			}
			
			sprintf(tmpchar,"Monitor/SetBpu/%s/AUTOLOADBPU",bpugroupname.c_str());
			if (xmlpack.GetNodeAttribute(tmpchar,false,"use",false,namevalue) != NULL)
			{
				if (atoi(namevalue.c_str()) == 1)
				{
					m_pRes->g_vBpuGroupInfo[bpuindex].g_bIsAutoLoadBu = true;
				}
				else
				{
					m_pRes->g_vBpuGroupInfo[bpuindex].g_bIsAutoLoadBu = false;
				}

				if (savemode == 1) //更新配置文件
				{
					sprintf(tmpcharconf,"package/head/BPU配置/BPU组/%s/AUTOLOADBPU",bpugroupname.c_str());
					m_pRes->m_xmlconf.SetNodeAttribute(tmpcharconf,false,"use",false,namevalue.c_str());
				}
				if (m_pRes->g_vBpuGroupInfo[bpuindex].g_bIsAutoLoadBu)//自动启动BPU
				{
					sprintf(tmpchar,"Monitor/SetBpu/%s/AUTOLOADBPU/BPU程序名",bpugroupname.c_str());
					if (xmlpack.GetNodeValueByPath(tmpchar,false,namevalue) != NULL)
					{
						m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuPrgName = namevalue;
						if (savemode == 1) //更新配置文件
						{
							sprintf(tmpcharconf,"package/head/BPU配置/BPU组/%s/AUTOLOADBPU/BPU程序名",bpugroupname.c_str());
							m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str());
						}
					}
					sprintf(tmpchar,"Monitor/SetBpu/%s/AUTOLOADBPU/BPU程序路径",bpugroupname.c_str());
					if (xmlpack.GetNodeValueByPath(tmpchar,false,namevalue) != NULL)
					{
						m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuPrgPath = namevalue;
						if (savemode == 1) //更新配置文件
						{
							sprintf(tmpcharconf,"package/head/BPU配置/BPU组/%s/AUTOLOADBPU/BPU程序路径",bpugroupname.c_str());
							m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str());
						}
					}
					sprintf(tmpchar,"Monitor/SetBpu/%s/AUTOLOADBPU/BPU运行目录",bpugroupname.c_str());
					if (xmlpack.GetNodeValueByPath(tmpchar,false,namevalue) != NULL)
					{
						m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuRunPath = namevalue;
						if (savemode == 1) //更新配置文件
						{
							sprintf(tmpcharconf,"package/head/BPU配置/BPU组/%s/AUTOLOADBPU/BPU运行目录",bpugroupname.c_str());
							m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str());
						}
					}
					sprintf(tmpchar,"Monitor/SetBpu/%s/AUTOLOADBPU/BPU启动参数",bpugroupname.c_str());
					if (xmlpack.GetNodeValueByPath(tmpchar,false,namevalue) != NULL)
					{
						m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuPrgPara = namevalue;
						if (savemode == 1) //更新配置文件
						{
							sprintf(tmpcharconf,"package/head/BPU配置/BPU组/%s/AUTOLOADBPU/BPU启动参数",bpugroupname.c_str());
							m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str());
						}
					}
				}
			}
			if (savemode == 1)
			{
				if (!m_pRes->m_xmlconf.ToFile(m_pRes->m_sBpcParaFileName.c_str()))
				{
					m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"更新配置文件[%s]失败",m_pRes->m_sBpcParaFileName.c_str());
				}
			}
		}
		else
		{
			m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"bpu组[%s]在BPC配置中不存在",bpugroupname.c_str());
		}
		bpugnode = (CXmlNode *)bpugnode->getNextSibling();
	}
	sprintf(rcvdata.sMsgBuf->sBuffer,"设置参数成功");
	rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
	rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
	rcvdata.sMsgBuf->sDBHead.cZip = 0;
	m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
	return ;
}

void CLinkThread::MonitorSetMode(S_BPC_RSMSG &rcvdata)
{
	if (rcvdata.sMsgBuf->sDBHead.cZip != 0)
	{
		if (!UnzipBuf(*rcvdata.sMsgBuf))
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"数据解压缩出错");
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
			return ;
		}
	}
	CBF_Xml  xmlpack;
	if (!xmlpack.FromBuffer(rcvdata.sMsgBuf->sBuffer))
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"请求数据非xml");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
		return ;
	}
	CXmlNode *setnode = xmlpack.GetNodeByPath("Monitor/SetBpu",false);
	if (setnode == NULL)
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"xml无[Monitor/SetBpu]节点");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
		return ;
	}
	int savemode=0;
	if (xmlpack.GetNodeAttribute("Monitor/SetMonitor",false,"mode",false,savemode) == NULL)
	{
		savemode =0;//无属性默认为0
	}
	std::string bpugroupname;
	int i;
	
	char tmpchar[200];
	char tmpcharconf[200];
	bool isfind=false;
	std::string namevalue;
	if (savemode == 1)
	{
		CBF_Date_Time dt;
		sprintf(tmpchar,"%s.%s%s",m_pRes->m_sBpcParaFileName.c_str(),dt.ToStringD8().c_str(),dt.ToStringT6().c_str());
		m_pRes->m_xmlconf.ToFile(tmpchar);
	}
// 	int   g_nUseMonitor;       //是否监控
// 	int   g_nMonitorHost;      //是否报告主机资源
// 	int   g_nMonitorDrebId;            //公共节点号
// 	int   g_nMonitorDrebPrivateId;     //私有节点号
// 	int   g_nMonitorSvrId;            //公共服务节点号
// 	int   g_nMonitorSvrPrivateId;     //私有服务节点号
// 	int   g_nMonitorTxCode;           //监控交易码
	if (xmlpack.GetNodeAttribute("Monitor/SetMonitor/监控配置",false,"use",false,namevalue) != NULL)
	{
		if (savemode == 1) //更新配置文件
		{
			sprintf(tmpcharconf,"package/head/监控配置");
			m_pRes->m_xmlconf.SetNodeAttribute(tmpcharconf,false,"use",false,namevalue.c_str(),false);
		}
		//更新内存
		m_pRes->g_nUseMonitor = atoi(namevalue.c_str());
	}
	if (xmlpack.GetNodeAttribute("Monitor/SetMonitor/监控配置",false,"host",false,namevalue) != NULL)
	{
		if (savemode == 1) //更新配置文件
		{
			sprintf(tmpcharconf,"package/head/监控配置");
			m_pRes->m_xmlconf.SetNodeAttribute(tmpcharconf,false,"host",false,namevalue.c_str(),false);
		}
		//更新内存
		m_pRes->g_nMonitorHost = atoi(namevalue.c_str());
	}
	if (xmlpack.GetNodeValueByPath("Monitor/SetMonitor/监控公共节点号",false,namevalue) != NULL)
	{
		if (savemode == 1) //更新配置文件
		{
			sprintf(tmpcharconf,"package/head/监控配置/监控公共节点号");
			m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str(),false);
		}
		//更新内存
		m_pRes->g_nMonitorDrebId = atoi(namevalue.c_str());
	}
	if (xmlpack.GetNodeValueByPath("Monitor/SetMonitor/监控私有节点号",false,namevalue) != NULL)
	{
		if (savemode == 1) //更新配置文件
		{
			sprintf(tmpcharconf,"package/head/监控配置/监控私有节点号");
			m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str(),false);
		}
		//更新内存
		m_pRes->g_nMonitorDrebPrivateId = atoi(namevalue.c_str());
	}
	if (xmlpack.GetNodeValueByPath("Monitor/SetMonitor/监控公共服务号",false,namevalue) != NULL)
	{
		if (savemode == 1) //更新配置文件
		{
			sprintf(tmpcharconf,"package/head/监控配置/监控公共服务号");
			m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str(),false);
		}
		//更新内存
		m_pRes->g_nMonitorSvrId = atoi(namevalue.c_str());
	}
	if (xmlpack.GetNodeValueByPath("Monitor/SetMonitor/监控私有服务号",false,namevalue) != NULL)
	{
		if (savemode == 1) //更新配置文件
		{
			sprintf(tmpcharconf,"package/head/监控配置/监控私有服务号");
			m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str(),false);
		}
		//更新内存
		m_pRes->g_nMonitorSvrPrivateId = atoi(namevalue.c_str());
	}
	if (xmlpack.GetNodeValueByPath("Monitor/SetMonitor/监控报告交易码",false,namevalue) != NULL)
	{
		if (savemode == 1) //更新配置文件
		{
			sprintf(tmpcharconf,"package/head/监控配置/监控报告交易码");
			m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str(),false);
		}
		//更新内存
		m_pRes->g_nMonitorTxCode = atoi(namevalue.c_str());
	}
	if (savemode == 1)
	{
		if (!m_pRes->m_xmlconf.ToFile(m_pRes->m_sBpcParaFileName.c_str()))
		{
			m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"更新配置文件[%s]失败",m_pRes->m_sBpcParaFileName.c_str());
			sprintf(rcvdata.sMsgBuf->sBuffer,"更新配置文件[%s]失败",m_pRes->m_sBpcParaFileName.c_str());
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
			return;
		}
	}
	sprintf(rcvdata.sMsgBuf->sBuffer,"设置参数成功");
	rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
	rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
	rcvdata.sMsgBuf->sDBHead.cZip = 0;
	m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
	return;
}

void CLinkThread::MonitorStopBpu(S_BPC_RSMSG &rcvdata)
{
	if (rcvdata.sMsgBuf->sDBHead.cZip != 0)
	{
		if (!UnzipBuf(*rcvdata.sMsgBuf))
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"数据解压缩出错");
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
			return ;
		}
	}
	CBF_Xml  xmlpack;
	if (!xmlpack.FromBuffer(rcvdata.sMsgBuf->sBuffer))
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"请求数据非xml");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
		return ;
	}
	CXmlNode *stopnode = xmlpack.GetNodeByPath("Monitor/StopBpu",false);
	if (stopnode == NULL)
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"xml无[Monitor/StopBpu]节点");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
		return ;
	}
	int i;
	int bpuindex=-1;
	std::string bpugroupname;
	int bpugnum=0;
	S_PROCESS_NODE node;
	bool isfind = false;
	char errmsg[200];
	CXmlNode *bpunode = (CXmlNode *)stopnode->GetFirstChild();
	while (bpunode != NULL)
	{
		bpugroupname = bpunode->GetNodeName();
		isfind=false;
		for (i=0; i< m_pRes->g_vBpuGroupInfo.size() ; i++)
		{
			if (bpugroupname.compare(m_pRes->g_vBpuGroupInfo[i].g_sBpuGroupName) == 0)
			{
				isfind = true;
				bpuindex = i;
				break;
			}
		}
		if (isfind)
		{
			node.prgname = m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuPrgName;
			node.prgpara = m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuPrgPara;
			node.prgpath = m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuPrgPath;
			node.runpath = m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuRunPath;
			node.nodeid = m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName;
			
			if (m_pRes->g_vBpuGroupInfo[bpuindex].g_nDispatchMode == 1) //java版本
			{
				m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"查找原BPU[%s]进程是否存在[%d]",\
					m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(),m_pRes->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid);
				if (m_pRes->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid > 0) //查看进程是否存在
				{
					node.pid = m_pRes->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid;
					if (m_pMgrBpu.FindProcess(&node,errmsg) == 0) //有此进程
					{
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"原BPU[%s]进程[%d]已存在，停止",\
							m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(),m_pRes->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid);
						if (m_pMgrBpu.StopProcess(&node,errmsg)!=0)
						{
							m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"BPU[%s]进程[%d]停止失败",\
								m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(),m_pRes->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid);
						}
					}
				}
			}
			else
			{
				for (int j=m_pRes->g_vBpuGroupInfo[bpuindex].g_nBeginSocketIndex ; j<m_pRes->g_vBpuGroupInfo[bpuindex].g_nEndSocketIndex ;j++)
				{
					if (m_pSockMgr->at(j)->m_sock != INVALID_SOCKET)
					{
						node.pid = m_pSockMgr->at(j)->m_nPid;
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"停止BPU[%s]进程[%d]",\
							m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(),node.pid);
						if (m_pMgrBpu.StopProcess(&node,errmsg)!=0)
						{
							m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"BPU[%s]进程[%d]停止失败",\
								m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(),node.pid);
						}
					}
				}
			}
			
		}
		bpunode = (CXmlNode *)bpunode->getNextSibling();
	}
	sprintf(rcvdata.sMsgBuf->sBuffer,"停止BPU成功");
	rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
	rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
	rcvdata.sMsgBuf->sDBHead.cZip = 0;
	m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
	return;
}

void CLinkThread::MonitorStartBpu(S_BPC_RSMSG &rcvdata)
{
	if (rcvdata.sMsgBuf->sDBHead.cZip != 0)
	{
		if (!UnzipBuf(*rcvdata.sMsgBuf))
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"数据解压缩出错");
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
			return ;
		}
	}
	CBF_Xml  xmlpack;
	if (!xmlpack.FromBuffer(rcvdata.sMsgBuf->sBuffer))
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"请求数据非xml");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
		return ;
	}
	CXmlNode *stopnode = xmlpack.GetNodeByPath("Monitor/StartBpu",false);
	if (stopnode == NULL)
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"xml无[Monitor/StartBpu]节点");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
		return ;
	}
	int i;
	int bpuindex=-1;
	std::string bpugroupname;
	int bpugnum=0;
	S_PROCESS_NODE node;
	bool isfind = false;
	int k;
	int connNum=0;
	char errmsg[200];
	CXmlNode *bpunode = (CXmlNode *)stopnode->GetFirstChild();
	while (bpunode != NULL)
	{
		bpugroupname = bpunode->GetNodeName();
		isfind=false;
		for (i=0; i< m_pRes->g_vBpuGroupInfo.size() ; i++)
		{
			if (bpugroupname.compare(m_pRes->g_vBpuGroupInfo[i].g_sBpuGroupName) == 0)
			{
				isfind = true;
				bpuindex = i;
				break;
			}
		}
		if (isfind)
		{
			node.prgname = m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuPrgName;
			node.prgpara = m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuPrgPara;
			node.prgpath = m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuPrgPath;
			node.runpath = m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuRunPath;
			node.nodeid = m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName;

			connNum=0;
			for (k=m_pRes->g_vBpuGroupInfo[bpuindex].g_nBeginSocketIndex ; k < m_pRes->g_vBpuGroupInfo[bpuindex].g_nEndSocketIndex; k++)
			{
				if (m_pSockMgr->at(k)->m_sock != INVALID_SOCKET)
				{
					connNum++;
				}
			}
			if (!m_pRes->g_vBpuGroupInfo[bpuindex].g_bIsAutoLoadBu) //不是自动重启才进入启动
			{
				if (m_pRes->g_vBpuGroupInfo[bpuindex].g_nDispatchMode == 1) //java版本
				{
//					bpugnum = m_pRes->g_vBpuGroupInfo[bpuindex].g_nBpuNum - m_pRes->g_vBpuGroupInfo[bpuindex].g_nStartBpuNum;
					bpugnum = m_pRes->g_vBpuGroupInfo[bpuindex].g_nBpuNum - connNum;
					if (bpugnum >0)
					{
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"查找原BPU[%s]进程是否存在[%d]",\
							m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(),m_pRes->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid);
						if (m_pRes->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid > 0) //查看进程是否存在
						{
							node.pid = m_pRes->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid;
							if (m_pMgrBpu.FindProcess(&node,errmsg) == 0) //有此进程
							{
								m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"原BPU[%s]进程[%d]已存在，停止",\
									m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(),m_pRes->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid);
								if (m_pMgrBpu.StopProcess(&node,errmsg)!=0)
								{
									m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"BPU[%s]进程[%d]停止失败",\
										m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(),m_pRes->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid);
								}
							}
							m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"BPU[%s]重启",\
								m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str());
							if (m_pMgrBpu.StopProcess(&node,errmsg)!=0)
							{
								m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"BPU[%s]进程[%d]停止失败",\
									m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(),m_pRes->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid);
							}
						}
					}
				}
				else
				{
//					bpugnum = m_pRes->g_vBpuGroupInfo[bpuindex].g_nBpuNum - m_pRes->g_vBpuGroupInfo[bpuindex].g_nStartBpuNum;
					bpugnum = m_pRes->g_vBpuGroupInfo[bpuindex].g_nBpuNum - connNum;
					for (int j=0; j<bpugnum;j++)
					{
						if (m_pMgrBpu.StartProcess(&node,errmsg)!=0)
						{
							m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"BPU[%s]启动失败",\
								m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str());
						}
					}
				}
			}
		}
		bpunode = (CXmlNode *)bpunode->getNextSibling();
	}
	sprintf(rcvdata.sMsgBuf->sBuffer,"启动BPU成功");
	rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
	rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
	rcvdata.sMsgBuf->sDBHead.cZip = 0;
	m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
	return;
}
void CLinkThread::OnBpuIsReg(int index,S_BPC_RSMSG &rcvdata)
{
	bool bRes=false;
	
	if (rcvdata.sMsgBuf->sDBHead.nLen <1)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"BPU注册时无bpu组名称 index=%d",index);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPC_NOGROUP;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		rcvdata.sMsgBuf->sDBHead.nLen = 0;
		rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
		m_pSockMgr->at(index)->SendMsg(rcvdata);
		OnClose(index,"BPU注册时无bpu组名称",__FILE__,__LINE__);
		return;
	}
	rcvdata.sMsgBuf->sBuffer[rcvdata.sMsgBuf->sDBHead.nLen]=0;
	bool isfind=false;
	int  bpugroupindex=0;
	for (int i=0;i<m_pRes->g_vBpuGroupInfo.size();i++)
	{
		if (strcmp(m_pRes->g_vBpuGroupInfo[i].g_sBpuGroupName.c_str(),rcvdata.sMsgBuf->sBuffer) == 0) //找到此BPU组
		{
			bpugroupindex = i;
			isfind = true;
			break;
		}
	}
	int connbpu=0;
	int connNum=0;
	int k;
	if (isfind) //有此bpu组
	{
		//查找已有此BPU组的连接数
		connNum=0;
		for (k=m_pRes->g_vBpuGroupInfo[bpugroupindex].g_nBeginSocketIndex ; k < m_pRes->g_vBpuGroupInfo[bpugroupindex].g_nEndSocketIndex; k++)
		{
			if (m_pSockMgr->at(k)->m_sock != INVALID_SOCKET)
			{
				connNum++;
			}
		}
		if (connNum >= m_pRes->g_vBpuGroupInfo[bpugroupindex].g_nBpuNum)
		{
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.nLen = 0;
			rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPC_BPULIMIT;
			m_pSockMgr->at(index)->SendMsg(rcvdata);
			m_pSockMgr->at(index)->OnClose("此组BPU连接已满",__FILE__,__LINE__);
			return ;
		}
		bool isfree = false;
		int bpuindex;
		//查找一个空闲的BPU连接，然后更新
		for (int j=m_pRes->g_vBpuGroupInfo[bpugroupindex].g_nBeginSocketIndex ; j<m_pRes->g_vBpuGroupInfo[bpugroupindex].g_nEndSocketIndex ; j++)
		{
			if (m_pSockMgr->at(j)->m_sock == INVALID_SOCKET)
			{
				isfree = true;
				bpuindex = j;
				break;
			}
		}
		if (!isfree)
		{
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.nLen = 0;
			rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPC_BPULIMIT;
			m_pSockMgr->at(index)->SendMsg(rcvdata);
			m_pSockMgr->at(index)->OnClose("此组BPU连接已满",__FILE__,__LINE__);
			return ;
		}
		//有可能有多收的数据
		if (m_pSockMgr->at(index)->m_nRcvBufLen>0)
		{
			memcpy(m_pSockMgr->at(bpuindex)->m_sRcvBuffer,m_pSockMgr->at(index)->m_sRcvBuffer,m_pSockMgr->at(index)->m_nRcvBufLen);
			m_pSockMgr->at(bpuindex)->m_nRcvBufLen = m_pSockMgr->at(index)->m_nRcvBufLen;
		}
		
		m_pSockMgr->at(bpuindex)->SetAccept(m_pSockMgr->at(index)->m_sock,m_pSockMgr->at(index)->m_sAddress);

		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"连接ip[%s] index[%d] socket[%d]转为BPU连接[%d] BPU组[%s]",\
			m_pSockMgr->at(index)->m_sAddress,index,m_pSockMgr->at(index)->m_sock,bpuindex,rcvdata.sMsgBuf->sBuffer);
		//置当前这个连接为空闲，accept可以加放了
		m_pSockMgr->at(index)->m_nRcvBufLen=0;
		m_pSockMgr->at(index)->m_sock = INVALID_SOCKET;


		//将组名及组名索引放到连接类中并置启动的BPU数+1
		
		m_pSockMgr->at(bpuindex)->m_sBpuGroupName = rcvdata.sMsgBuf->sBuffer;
		m_pSockMgr->at(bpuindex)->m_nBpuGroupIndex = bpugroupindex;
		m_pRes->g_vBpuGroupInfo[bpugroupindex].g_nStartBpuNum++;
		//将bpu的进程ID放到连接类中
		m_pSockMgr->at(bpuindex)->m_nPid = rcvdata.sMsgBuf->sBpcHead.nRtime;
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"连接过来的BPU组[%s] bpuindex[%d] BPU组index[%d] BPU启动数[%d]",\
			m_pSockMgr->at(bpuindex)->m_sBpuGroupName.c_str(),bpuindex,m_pSockMgr->at(bpuindex)->m_nBpuGroupIndex,\
			m_pRes->g_vBpuGroupInfo[bpugroupindex].g_nStartBpuNum);
		
		m_pSockMgr->at(bpuindex)->SetBpuFreeFlag(false);
//		m_pSockMgr->at(bpuindex)->m_bIsBuGetFlag = false;

//		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"BPU index[%d] 状态为忙注册",bpuindex);
		//BPC配置是否要注册
		if (m_pRes->g_nRegisterMode == REGISTERMODE_NONE || m_pRes->g_nRegisterMode == REGISTERMODE_STATIC)
		{
			m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"BPC配置无须BPU注册");
			m_pSockMgr->at(bpuindex)->m_bChecked = true;//置连接为已检查
			rcvdata.sMsgBuf->sDBHead.nLen = 0;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPC_NOREGISTER;
			m_pSockMgr->at(bpuindex)->SendMsg(rcvdata);
			return;
		}
		else //返回是否要注册
		{
			bRes = m_pRes->g_vBpuGroupInfo[bpugroupindex].g_bIsReg;
			if (bRes)
			{
				m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"BPU之前已经注册过");
			}
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			sprintf(rcvdata.sMsgBuf->sBuffer,"%d",bRes);  //1已经注册过,0未注册 需要注册
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN + rcvdata.sMsgBuf->sDBHead.nLen;
			m_pSockMgr->at(bpuindex)->SendMsg(rcvdata);
		}
	}
	else //无此bpu组
	{
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.nLen = 0;
		rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPC_NOGROUP;
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"BPU注册时无bpu组名称 %s",rcvdata.sMsgBuf->sBuffer);
		m_pSockMgr->at(index)->SendMsg(rcvdata);
		m_pSockMgr->at(index)->OnClose("BPU注册时无bpu组名称",__FILE__,__LINE__);
	}
}
void CLinkThread::StartTimer()
{
	m_pTimer.Start();
}

void CLinkThread::StopTimer()
{
	m_pTimer.Stop();
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
	m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"定时器");
	m_bIsPingTimer = true;
}

void CLinkThread::OnMonitor()
{
	if (m_pRes->g_nUseMonitor != 1) 
	{
		return;
	}
	//监控信息
	int i,k;
	CBF_Xml        pXml;
	pXml.SetNodeValueByPath("Monitor/公共节点",false,(int)m_pRes->g_nSvrMainId);
	pXml.SetNodeValueByPath("Monitor/私有节点",false,(int)m_pRes->g_nSvrPrivateId);
	pXml.SetNodeValueByPath("Monitor/节点类型",false,1);//bpc
	pXml.SetNodeValueByPath("Monitor/报告类型",false,0);//正常
	pXml.SetNodeValueByPath("Monitor/启动时间",false,m_pRes->g_sStartDateTime,false);
	int total=0;
	int used=0;
	m_pMemPool->PoolInfo(total,used);
	pXml.SetNodeAttribute("Monitor/内存缓冲池",false,"总分配",false,total );
	pXml.SetNodeAttribute("Monitor/内存缓冲池",false,"未使用",false,used );
	pXml.SetNodeAttribute("Monitor/内存缓冲池",false,"每块大小",false,(int)BPCBUFSIZE );
	

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
		connnode->SetAttribute("index",false,i+2);
		connnode->SetAttribute("公共节点",false,m_pSockMgr->at(i+2)->m_nDrebId);
		connnode->SetAttribute("私有节点",false,m_pSockMgr->at(i+2)->m_nDrebPrivateId);
		if (m_pSockMgr->at(i+2)->m_sock == INVALID_SOCKET)
		{
			connnode->SetAttribute("status",false,"未连接");
		}
		else if (m_pSockMgr->at(i+2)->m_bChecked)
		{
			connnode->SetAttribute("status",false,"正常");
		}
		else
		{
			connnode->SetAttribute("status",false,"正在连接");
		}
	}
	int connNum=0;
	//BPC信息
	pXml.AddNodeByPath("Monitor/BPU信息",false,"BPULIST",false,"");
	for (i=0 ; i<m_pRes->g_vBpuGroupInfo.size(); i++)
	{
		connNum=0;
		for (k=m_pRes->g_vBpuGroupInfo[i].g_nBeginSocketIndex ; k < m_pRes->g_vBpuGroupInfo[i].g_nEndSocketIndex; k++)
		{
			if (m_pSockMgr->at(k)->m_sock != INVALID_SOCKET)
			{
				connNum++;
			}
		}
		CXmlNode *bpunode = pXml.AddNodeByPath("Monitor/BPU信息/BPULIST",false,m_pRes->g_vBpuGroupInfo[i].g_sBpuGroupName.c_str(),true,"");
		if (bpunode != NULL)
		{
			bpunode->SetAttribute("名称",false,m_pRes->g_vBpuGroupInfo[i].g_sBpuGroupName);
			bpunode->SetAttribute("数目",false,m_pRes->g_vBpuGroupInfo[i].g_nBpuNum);
			bpunode->SetAttribute("实际数",false,connNum);
			bpunode->SetAttribute("心跳应答",false,m_pRes->g_vBpuGroupInfo[i].g_bIsAnsPing);
			bpunode->SetAttribute("分配模式",false,m_pRes->g_vBpuGroupInfo[i].g_nDispatchMode);
			bpunode->SetAttribute("自动启动",false,m_pRes->g_vBpuGroupInfo[i].g_bIsAutoLoadBu);
			bpunode->SetAttribute("程序名",false,m_pRes->g_vBpuGroupInfo[i].g_sBuPrgName);
			bpunode->SetAttribute("程序目录",false,m_pRes->g_vBpuGroupInfo[i].g_sBuPrgPath);
			bpunode->SetAttribute("运行目录",false,m_pRes->g_vBpuGroupInfo[i].g_sBuRunPath);
			bpunode->SetAttribute("参数",false,m_pRes->g_vBpuGroupInfo[i].g_sBuPrgPara);
		}
	}

	S_BPC_RSMSG  mdata;//接收数据结构
//	bzero(&mdata,sizeof(S_BPC_RSMSG));
	mdata.sMsgBuf = (PBPCCOMMSTRU )m_pMemPool->PoolMalloc();
	if (mdata.sMsgBuf == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"分配消息空间出错");
		return ;
	}
	int len = DREBDATASIZE;
	if (!pXml.ToBuffer(mdata.sMsgBuf->sBuffer,len))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ToBuffer出错");
		m_pMemPool->PoolFree(mdata.sMsgBuf);
		return;
	}
	mdata.sMsgBuf->sDBHead.nLen = len;
	
	m_pLog->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"监控通知",mdata.sMsgBuf->sBuffer,mdata.sMsgBuf->sDBHead.nLen);
	
	
	mdata.sMsgBuf->sDBHead.cRaflag = 0;
	mdata.sMsgBuf->sDBHead.cDrebAffirm = 0;
	mdata.sMsgBuf->sDBHead.cZip = 0;
	mdata.sMsgBuf->sDBHead.cNextFlag = 0;
	
	mdata.sMsgBuf->sDBHead.cCmd = CMD_DPPUSH;
	mdata.sMsgBuf->sDBHead.d_Dinfo.d_nNodeId = m_pRes->g_nMonitorDrebId;
	mdata.sMsgBuf->sDBHead.d_Dinfo.d_cNodePrivateId = m_pRes->g_nMonitorDrebPrivateId;
	mdata.sMsgBuf->sDBHead.d_Dinfo.d_nSvrMainId = m_pRes->g_nMonitorSvrId;
	mdata.sMsgBuf->sDBHead.d_Dinfo.d_cSvrPrivateId = m_pRes->g_nMonitorSvrPrivateId;
	mdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo = m_pRes->g_nMonitorTxCode;
	mdata.sMsgBuf->sBpcHead.cMsgType = MSG_BPCMONITOR;
	mdata.nRtime = time(NULL);
	S_FUNCINFO_TBL fn;
	int flag = 0;
	fn = m_pFuncTbl->FindFunc(mdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,flag);
	if (flag == 1) 
	{
		fn.nCallNum++;
		mdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN+mdata.sMsgBuf->sDBHead.nLen;
		strcpy(mdata.sBpuGroupName,fn.sBpuGroupName);//本 BPC下面的监控服务
		//20151102 直接放到BPU对应的线程队列
		if (m_pRes->PushData(fn.nBpuGroupIndex,mdata,fn.cPrio) != 0)
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"入BPU组队列出错 消息[%s]  DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 源[%d %d] cZip[%d] 业务数据长度[%d]",\
				m_pRes->GetBpcMsgType(mdata.sMsgBuf->sBpcHead.cMsgType).c_str(),\
				m_pRes->GetDrebCmdType(mdata.sMsgBuf->sDBHead.cCmd).c_str(),mdata.sMsgBuf->sDBHead.cNextFlag,\
				mdata.sMsgBuf->sDBHead.cRaflag,mdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,mdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
				mdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,mdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
				mdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,mdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
				mdata.sMsgBuf->sDBHead.cZip,mdata.sMsgBuf->sDBHead.nLen);
			m_pMemPool->PoolFree(mdata.sMsgBuf);
			mdata.sMsgBuf = NULL;
		}
	}
	else
	{
		bzero(mdata.sBpuGroupName,sizeof(mdata.sBpuGroupName));
		m_pPoolData->PushData(mdata);
	}
	
	return;
}

bool CLinkThread::UnzipBuf(BPCCOMMSTRU &data)
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
				if (m_pDes.UncryptString(data.sBuffer,ddlen,DESENCRYPTKEY)<=0)
				{
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解密出错");
					return false;
				}
				data.sDBHead.nLen = ddlen;
				data.sDBHead.cZip =0;
				break;
			case 2:	
				memcpy(zipDataBuf,data.sBuffer,data.sDBHead.nLen);
				if (!CBF_Tools::Uncompress((unsigned char *)(data.sBuffer),dlen,(unsigned char *)zipDataBuf,data.sDBHead.nLen))
				{
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解压缩出错");
					return false;
				}
				data.sDBHead.nLen = dlen;
				data.sDBHead.cZip =0;
				break;
			case 3:
				ddlen = data.sDBHead.nLen;
				if (m_pDes.UncryptString(data.sBuffer,ddlen,DESENCRYPTKEY3)<=0)
				{
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解密出错");
					return false;
				}
				data.sDBHead.nLen = ddlen;
				data.sDBHead.cZip =0;
				break;
			case 4:
				ddlen = data.sDBHead.nLen;
				m_pLog->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"解密前数据",data.sBuffer,data.sDBHead.nLen);
				if (m_pDes.UncryptString(data.sBuffer,ddlen,DESENCRYPTKEY)<=0)
				{
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解密出错");
					return false;
				}
				data.sDBHead.nLen = ddlen;
				m_pLog->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"解密后数据",data.sBuffer,data.sDBHead.nLen);
				memcpy(zipDataBuf,data.sBuffer,data.sDBHead.nLen);
				if (!CBF_Tools::Uncompress((unsigned char *)(data.sBuffer),dlen,(unsigned char *)zipDataBuf,data.sDBHead.nLen))
				{
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解压缩出错");
					return false;
				}
				data.sDBHead.nLen = dlen;
				m_pLog->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"解压后数据",data.sBuffer,data.sDBHead.nLen);
				data.sDBHead.cZip =0;
				break;
			default:
				return false;
		}
	}
	
	return true;
}

void CLinkThread::Download(S_BPC_RSMSG &rcvdata)
{
	if (rcvdata.sMsgBuf->sDBHead.cZip != 0)
	{
		if (!UnzipBuf(*rcvdata.sMsgBuf))
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"数据解压缩出错");
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
			return ;
		}
	}
	S_BPC_DUFILE sfile;
	if (rcvdata.sMsgBuf->sDBHead.nLen  < sizeof(S_BPC_DUFILE))
	{
		m_pLog->LogBin(LOG_ERROR,__FILE__,__LINE__,"请求数据非下载数据",\
			rcvdata.sMsgBuf->sBuffer,rcvdata.sMsgBuf->sDBHead.nLen);
		sprintf(rcvdata.sMsgBuf->sBuffer,"请求数据非下载数据");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_MONITOR_DOWNLOAD;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
		return ;
	}
	memcpy(&sfile,rcvdata.sMsgBuf->sBuffer,sizeof(S_BPC_DUFILE));
	struct_stat fstat;
	FSTAT(sfile.sFileName,&fstat);
	if (fstat.st_size <= 0)
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"文件[%s]不存在",sfile.sFileName);
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_MONITOR_DOWNLOAD;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
		return ;
	}
	int offset = atol(sfile.sFileOffset);
	if (offset >= fstat.st_size)
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"文件偏移量[%d]已大于等于文件大小",offset);
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_MONITOR_DOWNLOAD;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
		return ;
	}
	sprintf(sfile.sFileSize,"%011ld",fstat.st_size);
	FILE *fp = fopen(sfile.sFileName,"rb");
	if (fp == NULL)
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"文件[%s]不存在",sfile.sFileName);
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_MONITOR_DOWNLOAD;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
		return ;
	}
	fseek(fp,offset,1);
	int getlen = fread(rcvdata.sMsgBuf->sBuffer+sizeof(S_BPC_DUFILE),1,50*1024,fp);
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
	memcpy(rcvdata.sMsgBuf->sBuffer,&sfile,sizeof(S_BPC_DUFILE));
	rcvdata.sMsgBuf->sDBHead.nLen = sizeof(S_BPC_DUFILE)+getlen;
	rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
	rcvdata.sMsgBuf->sDBHead.cZip = 0;
	m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);

}

void CLinkThread::Upload(S_BPC_RSMSG &rcvdata)
{
	if (rcvdata.sMsgBuf->sDBHead.cZip != 0)
	{
		if (!UnzipBuf(*rcvdata.sMsgBuf))
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"数据解压缩出错");
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
			return ;
		}
	}
	S_BPC_DUFILE sfile;
	if (rcvdata.sMsgBuf->sDBHead.nLen  <= sizeof(S_BPC_DUFILE))
	{
		m_pLog->LogBin(LOG_ERROR,__FILE__,__LINE__,"请求数据非上传数据",\
			rcvdata.sMsgBuf->sBuffer,rcvdata.sMsgBuf->sDBHead.nLen);
		sprintf(rcvdata.sMsgBuf->sBuffer,"请求数据非上传数据");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_MONITOR_UPLOAD;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
		return ;
	}
	memcpy(&sfile,rcvdata.sMsgBuf->sBuffer,sizeof(S_BPC_DUFILE));
	m_pLog->LogBin(LOG_DEBUG,__FILE__,__LINE__,"上传文件",rcvdata.sMsgBuf->sBuffer,rcvdata.sMsgBuf->sDBHead.nLen );
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
		sprintf(rcvdata.sMsgBuf->sBuffer,"文件[%s]打开出错",sfile.sFileName);
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_MONITOR_UPLOAD;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
		return ;
	}
	fwrite(rcvdata.sMsgBuf->sBuffer+sizeof(S_BPC_DUFILE),1,rcvdata.sMsgBuf->sDBHead.nLen-sizeof(S_BPC_DUFILE),fp);
	fclose(fp);
	if (sfile.cEndFlag == '1')
	{
		struct_stat fstat;
		FSTAT(sfile.sFileName,&fstat);
		if (atol(sfile.sFileSize) != fstat.st_size)
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"上传文件[%s]失败，大小[%d]不等于[%s]",sfile.sFileName,fstat.st_size,sfile.sFileSize);
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_MONITOR_UPLOAD;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
			return ;
		}
		sprintf(rcvdata.sMsgBuf->sBuffer,"上传文件[%s]成功",sfile.sFileName);
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
		return ;
	}
	else
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"写入文件[%s]成功",sfile.sFileName);
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
		return ;
	}
}
