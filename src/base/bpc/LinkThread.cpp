// LinkThread.cpp: implementation of the CLinkThread class.
//
//////////////////////////////////////////////////////////////////////

#include "LinkThread.h"
#include "dreberrcode.h"
#include "BF_Xml.h"


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

	m_pLog = m_pRes->m_log;
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
				//不可能会走到这里了
                m_pMemPool->PoolFree(m_pDataRcv.sMsgBuf);
                m_pDataRcv.sMsgBuf = NULL;
				
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
	//bzero(&(data->sMsgBuf->sDBHead),DREBHEADLEN);
	data->sMsgBuf->sBpcHead.nConnectTime= m_pSockMgr->at(index)->m_nConntime;
	data->sMsgBuf->sBpcHead.nIndex = index;
	data->sMsgBuf->sBpcHead.nRtime = time(NULL);
	
	return true;
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
