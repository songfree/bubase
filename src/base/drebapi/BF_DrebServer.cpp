// BF_DrebServer.cpp: implementation of the CBF_DrebServer class.
//
//////////////////////////////////////////////////////////////////////

#include "BF_DrebServer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
char g_DrebApiVERSION[20]="2.0.2.20220616";   //api的版本

#define  DESENCRYPTKEY    "rd402wms"
#define  DESENCRYPTKEY3   "wms13711songfree"

CBF_DrebServer::CBF_DrebServer()
{
	m_nBegin =0;
	m_nEnd = 0;
	m_pRes = NULL;
	m_bIsRunning = false;
	m_bIsInit = false;
	m_sThreadName = "CBF_DrebServer";
	m_bIsPingTimer = false;
	
	bzero(&m_sHostInfo,sizeof(m_sHostInfo));
	m_nCurIndex = 0;
	m_spi = NULL;
}

CBF_DrebServer::~CBF_DrebServer()
{
	
}

void * CBF_DrebServer::PoolMalloc()
{
	return m_pMemPool.PoolMalloc();
}

void CBF_DrebServer::PoolFree(void *ppoint)
{
	m_pMemPool.PoolFree(ppoint);
}

bool CBF_DrebServer::Init(CBF_DrebResource *res, CDrebMsgProcBase* spi)
{
	if (m_bIsInit)
	{
		sprintf(m_errMsg,"已经初始化过");
		return false;
	}
	
	m_pRes = res;
	m_pLog.SetMaxLogSize(m_pRes->g_nMaxLogSize);
	m_pLog.SetLogPara(m_pRes->g_nLoglevel,m_pRes->g_sLogFilePath.c_str(),m_pRes->g_sLogFileName.c_str());
	
	
	char tmpchar1[100];
	char tmpchar2[100];
	char tmpchar3[100];
	char tmpchar4[100];
	sprintf(tmpchar1,"srv%d-%d.DREB2BPC.dat",m_pRes->g_nSvrMainId,m_pRes->g_nSvrPrivateId);
	sprintf(tmpchar2,"srv%d-%d.BPC2BPU.dat",m_pRes->g_nSvrMainId,m_pRes->g_nSvrPrivateId);
	sprintf(tmpchar3,"srv%d-%d.BPC2DREB.dat",m_pRes->g_nSvrMainId,m_pRes->g_nSvrPrivateId);
	sprintf(tmpchar4,"srv%d-%d.BPU2BPC.dat",m_pRes->g_nSvrMainId,m_pRes->g_nSvrPrivateId);

	m_pDrebDataLog.SetLogPara(m_pRes->g_nMaxLogSize,m_pRes->g_nDrebLoglevel,m_pRes->g_sLogFilePath.c_str(),tmpchar1,tmpchar2);
	m_pBpcCallDataLog.SetLogPara(m_pRes->g_nMaxLogSize,m_pRes->g_nDrebLoglevel,m_pRes->g_sLogFilePath.c_str(),tmpchar3,tmpchar4);

	if (!m_pSockMgr.Init(m_pRes,&m_pMemPool,&m_pLog,&m_pDrebDataLog,&m_pBpcCallDataLog))
	{
		return false;
	}
	if (!m_pMemPool.Init(10,BPCBUFSIZE))
	{
		return false;
	}
	m_nBegin = 0;
	m_nEnd = m_pRes->g_vDrebLinkInfo.size();

	//设置定时器
	m_pTimer.Init(100,true);
	//计算每秒数据量的定时器
	m_pTimer.SetTimer(0,1000,&CBF_DrebServer::OnTimer,this); //设置定时器 每隔1秒计算流量
	//取主机资源信息的定时器及连接noevent定时器
	m_pTimer.SetTimer(1,5000,&CBF_DrebServer::OnTimer,this);//每隔5秒去取一次cpu、内存、磁盘

	if (!m_pLog.StartLog())
	{
		sprintf(m_errMsg,"启动日志线程失败");
		return false;
	}
	m_spi = spi;
	m_bIsInit = true;
	return true;
}
void CBF_DrebServer::ExitThreadInstance()
{
	return ;
}
bool CBF_DrebServer::InitThreadInstance()
{
	return true;
}

int CBF_DrebServer::Run()
{
	struct timeval tv;
	int result;
	int i,j;
	int maxfd=0;
	int ret;
	int lasttime=time(NULL);
	int lasttime2=time(NULL);
	if (!m_bIsInit)
	{
		sprintf(m_errMsg,"未初始化，退出线程");
		m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
		return -1;
	}
#ifdef _ENGLISH_
	m_pLog.LogMp(LOG_INFO,__FILE__,__LINE__,"Start DrebServer thread");
#else
	m_pLog.LogMp(LOG_INFO,__FILE__,__LINE__,"启动DrebServer线程");
#endif
	while(m_bIsRunning)
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
			for (int i=0; i<m_pRes->g_vDrebLinkInfo.size() ;i++)
			{
				OnConnect(i);
			}
			SLEEP(5);
//			m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"无dreb连接 %d",tv.tv_sec);
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
#ifdef _ENGLISH_
	m_pLog.LogMp(LOG_INFO,__FILE__,__LINE__,"Stop DrebServer thread");
#else
	m_pLog.LogMp(LOG_INFO,__FILE__,__LINE__,"停止DrebServer线程");
#endif
	return 0;
}
int CBF_DrebServer::GetMaxfdAddSelect(int &maxfd)
{
	int ret=0;
	maxfd = 0;
	for (int i=m_nBegin;i<m_nEnd;i++)
	{
		if (!m_pSockMgr.at(i)->IsCanSelect())
		{
			continue;
		}
		else
		{
			//需要读数据
			if (m_pSockMgr.at(i)->NeedRead())
			{
				FD_SET(m_pSockMgr.at(i)->m_sock, &m_rset);//加入到读select集合
			}
			//需要发送数据或主动连接时
			if (m_pSockMgr.at(i)->NeedSend() || m_pSockMgr.at(i)->NeedConnect())
			{
				FD_SET(m_pSockMgr.at(i)->m_sock, &m_wset);//加入到写select集合
			}
#if defined(_WINDOWS)
			if (m_pSockMgr.at(i)->NeedConnect())
			{
				FD_SET(m_pSockMgr.at(i)->m_sock, &m_eset);//加入到连接异常集合
			}
#endif
			//得到最大的socket 
			if (m_pSockMgr.at(i)->m_sock > maxfd)
			{
				maxfd=m_pSockMgr.at(i)->m_sock; 
			}
			ret++;
		}
	}
	return ret;
}
void CBF_DrebServer::OnConnect(int conindex)
{
	if (m_pSockMgr.at(conindex)->m_sock != INVALID_SOCKET) //连接成功
	{
		return ;
	}
	if (!m_bIsRunning)
	{
		return;
	}
	//去连接
	if (time(NULL) - m_pSockMgr.at(conindex)->m_nConntime < 5 ) //在10秒内不用重连
	{
		return;
	}
	int ret;
	m_pSockMgr.at(conindex)->m_bChecked = false;
	m_pSockMgr.at(conindex)->m_nType = BPCSOCK_DREB;
	
	m_pSockMgr.at(conindex)->m_nConntime = time(NULL);
	m_tcpSocket.Create(AF_INET,false);//异步方式
#ifdef _ENGLISH_
	m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"try to connect DREB %s:%d:%d",\
		m_pSockMgr.at(conindex)->m_sDrebIp.c_str(),m_pSockMgr.at(conindex)->m_nDrebPort,\
		m_pSockMgr.at(conindex)->m_sock);
#else
	m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"尝试连接到DREB节点%s:%d:%d",\
		m_pSockMgr.at(conindex)->m_sDrebIp.c_str(),m_pSockMgr.at(conindex)->m_nDrebPort,\
		m_pSockMgr.at(conindex)->m_sock);
#endif
	if (m_tcpSocket.ConnectServer((char *)m_pSockMgr.at(conindex)->m_sDrebIp.c_str(),m_pSockMgr.at(conindex)->m_nDrebPort))
	{
		
		m_pSockMgr.at(conindex)->m_nReadtime = time(NULL);
		m_pSockMgr.at(conindex)->m_sock = m_tcpSocket.GetSocket();
	}
	else
	{
		if ((m_tcpSocket.GetErrorNo() == EWOULDBLOCK) || (m_tcpSocket.GetErrorNo() == EINPROGRESS))
		{
			m_pSockMgr.at(conindex)->m_nReadtime = time(NULL);
			m_pSockMgr.at(conindex)->m_sock = m_tcpSocket.GetSocket();
#ifdef _ENGLISH_
			m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"wait for connect DREB %s:%d",m_pSockMgr.at(conindex)->m_sDrebIp.c_str(),m_pSockMgr.at(conindex)->m_nDrebPort);
#else
			m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"等待建立到DREB %s:%d的连接",m_pSockMgr.at(conindex)->m_sDrebIp.c_str(),m_pSockMgr.at(conindex)->m_nDrebPort);
#endif
			return;
		}
		else
		{
			m_pSockMgr.at(conindex)->m_sock = INVALID_SOCKET;
			m_tcpSocket.Close();
#ifdef _ENGLISH_
			m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"connect to DREB %s:%d error, errno:%d",m_pSockMgr.at(conindex)->m_sDrebIp.c_str(),m_pSockMgr.at(conindex)->m_nDrebPort,m_tcpSocket.GetErrorNo());
#else
			m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"建立到DREB %s:%d的连接失败 errno:%d",m_pSockMgr.at(conindex)->m_sDrebIp.c_str(),m_pSockMgr.at(conindex)->m_nDrebPort,m_tcpSocket.GetErrorNo());
#endif
		}
	}
	return;
	
}
void CBF_DrebServer::OnConnected(int conindex)
{
	if (m_pSockMgr.at(conindex)->m_sock != INVALID_SOCKET) //连接成功
	{
		//可以发送数据了
		m_pSockMgr.at(conindex)->m_bNeedread = true;
#ifdef _ENGLISH_
		m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"connect DREB %s:%d success",m_pSockMgr.at(conindex)->m_sDrebIp.c_str(),m_pSockMgr.at(conindex)->m_nDrebPort);
#else
		m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"建立到DREB %s:%d的连接成功",m_pSockMgr.at(conindex)->m_sDrebIp.c_str(),m_pSockMgr.at(conindex)->m_nDrebPort);
#endif
		//连接成功后发送CMD_CONNECT消息，确认连接的类型
		m_pSockMgr.at(conindex)->m_bNeedConnect = false;
		//发送连接消息
		m_pSockMgr.at(conindex)->m_nPingTime = time(NULL);
		if (!ReMallocData(conindex,&m_pDataBuf))
		{
			return;
		}
		//发送连接指令给DREB
		m_pDataBuf.sMsgBuf->sDBHead.cRaflag = 0;
		m_pDataBuf.sMsgBuf->sDBHead.cZip = 0;
		m_pDataBuf.sMsgBuf->sDBHead.cCmd = CMD_CONNECT;
		m_pDataBuf.sMsgBuf->sDBHead.cDrebAffirm = 0;
		m_pDataBuf.sMsgBuf->sDBHead.nLen = sizeof(S_CONNECT_DATA);
		S_CONNECT_DATA *data = (S_CONNECT_DATA *)m_pDataBuf.sMsgBuf->sBuffer;
		data->cConnectType = 1;
		data->nSvrMainId = m_pRes->g_nSvrMainId;
		data->cSvrPrivateId = m_pRes->g_nSvrPrivateId;
		sprintf(data->sVersion,"%s",g_DrebApiVERSION);
		m_pDrebEndian.Endian2Comm((unsigned char *)&(data->nSvrMainId),sizeof(data->nSvrMainId));
		m_pDataBuf.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN+m_pDataBuf.sMsgBuf->sDBHead.nLen;
		m_pSockMgr.at(conindex)->SendMsg(m_pDataBuf,true);
		//置状态统一在此消息的返回处理中设置
		
		return ;
	}
	return;
	
}
void CBF_DrebServer::OnEvent()
{
	int ret;
	int i;
	for (i= m_nBegin; i<m_nEnd; i++)
	{
		if (m_pSockMgr.at(i)->m_sock == INVALID_SOCKET)//未连接
		{
			if (m_pSockMgr.at(i)->m_nType == BPCSOCK_DREB   ) //若是数据总线节点连接，则去连接
			{
				OnConnect(i);
			}
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
	}
}
void CBF_DrebServer::OnNoEvent()
{
	int j;
	int tmptime;
	for (j=m_nBegin; j<m_nEnd; j++)
	{
		if (m_pSockMgr.at(j)->m_sock != INVALID_SOCKET && m_pSockMgr.at(j)->m_nType == BPCSOCK_DREB)
		{
			tmptime = time(NULL) - m_pSockMgr.at(j)->m_nReadtime;
			if ( tmptime > m_pRes->g_nHeartRun && tmptime < m_pRes->g_nDisconnectTime)
			{
				OnPing(j);
			}
			else if (tmptime > m_pRes->g_nDisconnectTime)
			{
#ifdef _ENGLISH_
				OnClose(j,"connect unuse long time,close it",__FILE__,__LINE__);
#else
				OnClose(j,"连接长时间未用，断开",__FILE__,__LINE__);
#endif
			}
		}
		if (!m_bIsRunning)
		{
			return;
		}
		if (m_pSockMgr.at(j)->m_sock != INVALID_SOCKET)
		{
			//没有检查通过
			if (!m_pSockMgr.at(j)->m_bChecked)
			{
				//数据总线节点连接
				if (m_pSockMgr.at(j)->m_nType == BPCSOCK_DREB) 
				{
					if (time(NULL)-m_pSockMgr.at(j)->m_nReadtime >= m_pRes->g_nHeartRun)
					{
						if (m_pSockMgr.at(j)->NeedConnect())
						{
#ifdef _ENGLISH_
							m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"connect DREB %s:%d fail",m_pSockMgr.at(j)->m_sDrebIp.c_str(),m_pSockMgr.at(j)->m_nDrebPort);
#else
							m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"尝试连接到DREB %s:%d未成功",m_pSockMgr.at(j)->m_sDrebIp.c_str(),m_pSockMgr.at(j)->m_nDrebPort);
#endif
							closesocket(m_pSockMgr.at(j)->m_sock);
							m_pSockMgr.at(j)->m_sock = INVALID_SOCKET;
						}
					}
				}// end if (m_pSockMgr.at(conindex)->m_nType == BPCSOCK_TODREB) //数据总线节点连接
				else if (m_pSockMgr.at(j)->m_nType == BPCSOCK_UNKNOW)
				{
					if (time(NULL)-m_pSockMgr.at(j)->m_nReadtime >= m_pRes->g_nHeartRun)
					{
#ifdef _ENGLISH_
						OnClose(j,"connect illegal",__FILE__,__LINE__);
#else
						OnClose(j,"连接非法",__FILE__,__LINE__);
#endif
					} 
				}
				else 
				{
#ifdef _ENGLISH_
					m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"connect index[%d] type[%d] sock[%d]",\
						j,m_pSockMgr.at(j)->m_nType,m_pSockMgr.at(j)->m_sock);
					if (time(NULL)-m_pSockMgr.at(j)->m_nReadtime >= m_pRes->g_nHeartRun)
					{
						OnClose(j,"connect timeout",__FILE__,__LINE__);
					} 
#else
					m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"连接index[%d] type[%d] sock[%d]",\
						j,m_pSockMgr.at(j)->m_nType,m_pSockMgr.at(j)->m_sock);
					if (time(NULL)-m_pSockMgr.at(j)->m_nReadtime >= m_pRes->g_nHeartRun)
					{
						OnClose(j,"协商超时",__FILE__,__LINE__);
					} 
#endif
				}
			}
		}// end if (m_pSockMgr.at(conindex)->m_sock != INVALID_SOCKET)
		if (!m_bIsRunning)
		{
			return;
		}
		//若是数据总线节点连接，且没有连上则去连接
		if (m_pSockMgr.at(j)->m_nType == BPCSOCK_DREB )
		{
			if (m_pSockMgr.at(j)->m_sock == INVALID_SOCKET)
			{
				OnConnect(j);
			}
		}
	}
	
}
bool CBF_DrebServer::ReMallocData(int index,S_BPC_RSMSG *data)
{
	data->sMsgBuf = (PBPCCOMMSTRU )m_pMemPool.PoolMalloc();
	if (data->sMsgBuf == NULL)
	{
#ifdef _ENGLISH_
		m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"malloc msgdata error");
		m_pSockMgr.at(index)->OnClose("malloc msgdata error",__FILE__,__LINE__);
#else
		m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"分配消息空间出错");
		m_pSockMgr.at(index)->OnClose("分配消息空间出错",__FILE__,__LINE__);
#endif
		return false;
	}
	bzero(&(data->sMsgBuf->sDBHead), sizeof(DREB_HEAD));
	bzero(&(data->sMsgBuf->sBpcHead), sizeof(S_BPC_HEAD));
	data->sMsgBuf->sBpcHead.nConnectTime= m_pSockMgr.at(index)->m_nConntime;
	data->sMsgBuf->sBpcHead.nIndex = index;
	data->sMsgBuf->sBpcHead.nRtime = time(NULL);
	
	return true;
}
void CBF_DrebServer::OnReadEvent(int conindex)
{
	int ret;
	ret = FD_ISSET(m_pSockMgr.at(conindex)->m_sock,&m_rset);
	if (ret != 0) //有读事件
	{
		OnRecv(conindex);
	}
}
void CBF_DrebServer::OnWriteEvent(int conindex)
{
	int ret;
	if (m_pSockMgr.at(conindex)->m_sock == INVALID_SOCKET)
	{
		return;
	}
#if defined(_WINDOWS)
	ret = FD_ISSET(m_pSockMgr.at(conindex)->m_sock, &m_eset);
	if (ret != 0)
	{
		//主动连接其它的DREB
		if (m_pSockMgr.at(conindex)->m_nType == BPCSOCK_DREB)
		{
#ifdef _ENGLISH_
			m_pLog.LogMp(2,__FILE__,__LINE__,"connect DREB %s:%d fail",m_pSockMgr.at(conindex)->m_sDrebIp.c_str(),m_pSockMgr.at(conindex)->m_nDrebPort);
#else
			m_pLog.LogMp(2,__FILE__,__LINE__,"尝试连接到DREB %s:%d未成功",m_pSockMgr.at(conindex)->m_sDrebIp.c_str(),m_pSockMgr.at(conindex)->m_nDrebPort);
#endif
			closesocket(m_pSockMgr.at(conindex)->m_sock);
			m_pSockMgr.at(conindex)->m_sock = INVALID_SOCKET;
		}
		else 
		{
#ifdef _ENGLISH_
			OnClose(conindex,"connect DREB Other fail",__FILE__,__LINE__);
#else
			OnClose(conindex,"尝试连接到DREB Other connect fail",__FILE__,__LINE__);
#endif
		}
	}
	else if (FD_ISSET(m_pSockMgr.at(conindex)->m_sock, &m_wset) !=0 )
	{
		if (m_pSockMgr.at(conindex)->NeedConnect())
		{
			if (m_pSockMgr.at(conindex)->m_nType == BPCSOCK_DREB ) 
			{
				OnConnected(conindex);//去连接数据总线节点
			}
		}
		else if (m_pSockMgr.at(conindex)->m_bChecked)
		{
			OnSend(conindex);//发送数据
		}
	}
	else  if (m_pSockMgr.at(conindex)->m_bChecked) //socket无写事件
	{
//		OnSend(conindex);//发送数据
	}
#else
	
	int ok;
	int oklen;
	if (FD_ISSET(m_pSockMgr.at(conindex)->m_sock, &m_wset) !=0)
	{
		if (m_pSockMgr.at(conindex)->NeedConnect())
		{
			oklen=sizeof(ok);
#if defined(HP_UX)
			getsockopt(m_pSockMgr.at(conindex)->m_sock,SOL_SOCKET,SO_ERROR,(char *)&ok,&oklen);
#else
			getsockopt(m_pSockMgr.at(conindex)->m_sock,SOL_SOCKET,SO_ERROR,(char *)&ok,(socklen_t *)&oklen);
#endif
			if (ok == 0)
			{
				if (m_pSockMgr.at(conindex)->m_nType == BPCSOCK_DREB)
				{
					OnConnected(conindex);//去连接数据总线节点
				}
			}
			else
			{
				if (m_pSockMgr.at(conindex)->m_nType == BPCSOCK_DREB)
				{
#ifdef _ENGLISH_
					m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"connect DREB %s:%d fail",m_pSockMgr.at(conindex)->m_sDrebIp.c_str(),m_pSockMgr.at(conindex)->m_nDrebPort);
#else
					m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"尝试连接到DREB %s:%d未成功",m_pSockMgr.at(conindex)->m_sDrebIp.c_str(),m_pSockMgr.at(conindex)->m_nDrebPort);
#endif
					closesocket(m_pSockMgr.at(conindex)->m_sock);
					m_pSockMgr.at(conindex)->m_sock = INVALID_SOCKET;
				}
			} // end if (ok == 0)
		}
		else if (m_pSockMgr.at(conindex)->m_bChecked)// else if (g_connInfoList[conindex]->m_bNeedconnect)
		{
			OnSend(conindex);//发送数据
		} // end if (g_connInfoList[conindex]->m_bNeedconnect)
	}
	else  if (m_pSockMgr.at(conindex)->m_bChecked)
	{
//		OnSend(conindex);//发送数据
	}
#endif
	
				
}
void CBF_DrebServer::OnPing(int conindex)
{
	if (time(NULL) - m_pSockMgr.at(conindex)->m_nPingTime < BEATHEARTTICK) //小于5秒不用发ping
	{
		return ;
	}
	if (!m_pSockMgr.at(conindex)->m_bChecked)
	{
		//没有连接上，不用发
		return;
	}
	if (m_pSockMgr.at(conindex)->m_nType == BPCSOCK_DREB)
	{
		m_pSockMgr.at(conindex)->m_nPingTime = time(NULL);
		if (!ReMallocData(conindex,&m_pDataBuf))
		{
			return;
		}
		m_pDataBuf.sMsgBuf->sDBHead.cRaflag = 0;
		m_pDataBuf.sMsgBuf->sDBHead.cZip = 0;
		m_pDataBuf.sMsgBuf->sDBHead.nLen = 0;
		m_pDataBuf.sMsgBuf->sDBHead.cCmd = CMD_PING;
		m_pDataBuf.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
		//发送ping数据包
		m_pSockMgr.at(conindex)->SendMsg(m_pDataBuf);
	}
	
}
void CBF_DrebServer::OnClose(int conindex,const char *msg,const char *filename,int fileline)
{
	if (m_pSockMgr.at(conindex)->m_sock == INVALID_SOCKET)
	{
		return;
	}
	//将socket关闭
	m_pSockMgr.at(conindex)->OnClose(msg,filename,fileline);
}
void CBF_DrebServer::OnRecv(int conindex)
{
	int ret=0;
	int i=0;
	
	ret = m_pSockMgr.at(conindex)->OnRecv();	
	if (ret<=0)
	{
#ifdef _ENGLISH_
		OnClose(conindex,"far-end close",__FILE__,__LINE__);
#else
		OnClose(conindex,"远端关闭，接收数据不完整",__FILE__,__LINE__);
#endif
		return;
	}
	while (m_pSockMgr.at(conindex)->m_nRcvBufLen>0)
	{
		if (!ReMallocData(conindex,&m_pDataRcv))
		{
			return;
		}
		ret = m_pSockMgr.at(conindex)->GetRecvData(&m_pDataRcv);
		if (ret>0)
		{
			if (m_pSockMgr.at(conindex)->m_nType == BPCSOCK_DREB)
			{
				//若是应答且为不可靠推送或全域广播消息，直接丢弃
				if (m_pDataRcv.sMsgBuf->sDBHead.cRaflag == 1 && (m_pDataRcv.sMsgBuf->sDBHead.cCmd == CMD_DPPUSH || m_pDataRcv.sMsgBuf->sDBHead.cCmd == CMD_DPABC))
				{
#ifdef _ENGLISH_
					m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"CMD_DPPUSH or CMD_DPABC answer,discard");
#else
					m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"CMD_DPPUSH或CMD_DPABC的应答，丢弃");
#endif
					m_pMemPool.PoolFree(m_pDataRcv.sMsgBuf);
					m_pDataRcv.sMsgBuf = NULL;
					continue;
				}
				ProcessDreb(m_pDataRcv);
			}
			else
			{
				m_pMemPool.PoolFree(m_pDataRcv.sMsgBuf);
				m_pDataRcv.sMsgBuf = NULL;
			}
		}
		else
		{
			m_pMemPool.PoolFree(m_pDataRcv.sMsgBuf);
			m_pDataRcv.sMsgBuf = NULL;
			break;
		}
	}
	
	return ;
}
void CBF_DrebServer::OnSend(int conindex)
{
	m_pSockMgr.at(conindex)->OnSend();
}

void CBF_DrebServer::ProcessDreb(S_BPC_RSMSG &rcvdata)
{
	int flag=0;
	if (rcvdata.sMsgBuf->sDBHead.cRaflag == 0) //请求
	{
		if (rcvdata.sMsgBuf->sDBHead.cNextFlag == 2 ) //取后续数据
		{
			if (rcvdata.sMsgBuf->sDBHead.cCmd != CMD_DPCALL && rcvdata.sMsgBuf->sDBHead.cCmd != CMD_DPACALL)
			{
				//不是这两种，就不是取后续包
				m_pMemPool.PoolFree(rcvdata.sMsgBuf);
				rcvdata.sMsgBuf = NULL;
				return;
			}
			rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN+rcvdata.sMsgBuf->sDBHead.nLen;
			rcvdata.sMsgBuf->sBpcHead.cMsgType = MSG_GETBPCNEXT;
			rcvdata.nRtime = time(NULL);
            if (m_spi != NULL) //有设定回调
            {
                m_spi->ProcessDrebData(rcvdata);
            }
            else //放入队列
            {
                m_qRcvQueue.PushData(rcvdata);
            }
			return;
		}
		if (rcvdata.sMsgBuf->sDBHead.cNextFlag == 3) //bpu取后续包
		{
#ifdef _ENGLISH_
			
#else
			m_pLog.LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"收到BPU取后续包请求数据");
#endif

			if (rcvdata.sMsgBuf->sDBHead.cCmd != CMD_DPCALL && rcvdata.sMsgBuf->sDBHead.cCmd != CMD_DPACALL)
			{
				//不是这两种，就不是取后续包
				m_pMemPool.PoolFree(rcvdata.sMsgBuf);
				rcvdata.sMsgBuf = NULL;
				return;
			}
			rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN+rcvdata.sMsgBuf->sDBHead.nLen;
			rcvdata.sMsgBuf->sBpcHead.cMsgType = MSG_GETNEXT;
			rcvdata.nRtime = time(NULL);
			if (m_spi != NULL) //有设定回调
			{
				m_spi->ProcessDrebData(rcvdata);
			}
			else //放入队列
			{
				m_qRcvQueue.PushData(rcvdata);
			}
			return;
		}
		switch (rcvdata.sMsgBuf->sDBHead.cCmd)
		{
			case CMD_PING:
				rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
				rcvdata.sMsgBuf->sDBHead.nLen = 0;
				rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
				m_pSockMgr.at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
				break;
            case CMD_DPBC:
            case CMD_DPABC:	//广播信息，需要过滤重复的
				if (NULL != m_bcSerial.Select(rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId, rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial))
				{
					//重复
                    m_pLog.LogMp(LOG_DEBUG+1, __FILE__, __LINE__, "收到重复的广播 %d %d %d", rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId, rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId, rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial);
                    m_pMemPool.PoolFree(rcvdata.sMsgBuf);
                    rcvdata.sMsgBuf = NULL;
					break;
				}
				S_BC_SERIAL bcserial;
				bcserial.node_id = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId;
				bcserial.node_pid = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId;
				bcserial.serial = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial;
				bcserial.timestamp = time(NULL);
				m_bcSerial.Insert(bcserial);
                rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN + rcvdata.sMsgBuf->sDBHead.nLen;
                rcvdata.sMsgBuf->sBpcHead.cMsgType = MSG_REQ;
                rcvdata.nRtime = time(NULL);
                if (m_spi != NULL) //有设定回调
                {
                    m_spi->ProcessDrebData(rcvdata);
                }
                else //放入队列
                {
                    m_qRcvQueue.PushData(rcvdata);
                }
				break;
			case CMD_DPCALL:
			case CMD_DPACALL:
			case CMD_DPPUSH:
			case CMD_DPPOST:
				rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN+rcvdata.sMsgBuf->sDBHead.nLen;
				rcvdata.sMsgBuf->sBpcHead.cMsgType = MSG_REQ;
				rcvdata.nRtime = time(NULL);
                if (m_spi != NULL) //有设定回调
                {
                    m_spi->ProcessDrebData(rcvdata);
                }
                else //放入队列
                {
                    m_qRcvQueue.PushData(rcvdata);
                }
				break;	
			case CMD_MONITOR_BPC:
				rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN+rcvdata.sMsgBuf->sDBHead.nLen;
				rcvdata.sMsgBuf->sBpcHead.cMsgType = MSG_MONITOR_SAP;
				rcvdata.nRtime = time(NULL);
                if (m_spi != NULL) //有设定回调
                {
                    m_spi->ProcessDrebData(rcvdata);
                }
                else //放入队列
                {
                    m_qRcvQueue.PushData(rcvdata);
                }
				break;	
			default:
				rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN+rcvdata.sMsgBuf->sDBHead.nLen;
				rcvdata.sMsgBuf->sBpcHead.cMsgType = MSG_REQ;
				rcvdata.nRtime = time(NULL);
                if (m_spi != NULL) //有设定回调
                {
                    m_spi->ProcessDrebData(rcvdata);
                }
                else //放入队列
                {
                    m_qRcvQueue.PushData(rcvdata);
                }
		}
		return;
		
	}
	else  //是DREB的应答
	{
		switch (rcvdata.sMsgBuf->sDBHead.cCmd)
		{
			case CMD_PING:
				m_pMemPool.PoolFree(rcvdata.sMsgBuf);
				rcvdata.sMsgBuf = NULL;
				break;
			case CMD_CONNECT:
				if (rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode != SUCCESS)
				{
#ifdef _ENGLISH_
					m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"DREB connect fail, retcode[%d]",rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode);
					m_pSockMgr.at(rcvdata.sMsgBuf->sBpcHead.nIndex)->OnClose("DREB connect fail",__FILE__,__LINE__);
#else
					m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"DREB服务连接失败 返回码[%d]",rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode);
					m_pSockMgr.at(rcvdata.sMsgBuf->sBpcHead.nIndex)->OnClose("DREB返回服务连接失败",__FILE__,__LINE__);
#endif
					m_pSockMgr.at(rcvdata.sMsgBuf->sBpcHead.nIndex)->m_nConntime = time(NULL);
					m_pMemPool.PoolFree(rcvdata.sMsgBuf);
					rcvdata.sMsgBuf = NULL;
					return;
				}
				m_pSockMgr.at(rcvdata.sMsgBuf->sBpcHead.nIndex)->m_bChecked = true;
				m_pSockMgr.at(rcvdata.sMsgBuf->sBpcHead.nIndex)->m_nDrebId = rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nNodeId;
				m_pSockMgr.at(rcvdata.sMsgBuf->sBpcHead.nIndex)->m_nDrebPrivateId = rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_cNodePrivateId;
				
				memcpy(&(m_pSockMgr.at(rcvdata.sMsgBuf->sBpcHead.nIndex)->m_head),&(rcvdata.sMsgBuf->sDBHead),DREBHEADLEN);
				rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN+rcvdata.sMsgBuf->sDBHead.nLen;
				rcvdata.sMsgBuf->sBpcHead.cMsgType = MSG_CONNECTDREB;
				rcvdata.nRtime = time(NULL);
                if (m_spi != NULL) //有设定回调
                {
                    m_spi->ProcessDrebData(rcvdata);
                }
                else //放入队列
                {
                    m_qRcvQueue.PushData(rcvdata);
                }
				break;
			case CMD_DEL_SVR:
				m_pMemPool.PoolFree(rcvdata.sMsgBuf);
				rcvdata.sMsgBuf = NULL;
				break;
			case CMD_REGSERVICE:
				m_pMemPool.PoolFree(rcvdata.sMsgBuf);
				rcvdata.sMsgBuf = NULL;
				break;	
			case CMD_DPCALL:
			case CMD_DPACALL:
			case CMD_DPBC:
			case CMD_DPABC:
			case CMD_DPPUSH:
			case CMD_DPPOST:
				//bpc外调，直接应答给bpu
				rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN+rcvdata.sMsgBuf->sDBHead.nLen;
				rcvdata.sMsgBuf->sBpcHead.cMsgType = MSG_DREBANS;
				rcvdata.nRtime = time(NULL);
                if (m_spi != NULL) //有设定回调
                {
                    m_spi->ProcessDrebData(rcvdata);
                }
                else //放入队列
                {
                    m_qRcvQueue.PushData(rcvdata);
                }
				break;	
			default:
				m_pMemPool.PoolFree(rcvdata.sMsgBuf);
				rcvdata.sMsgBuf = NULL;
				break;
		}
		return;
	}
}

void CBF_DrebServer::OnMonitor()
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
	m_pMemPool.PoolInfo(total,used);
	pXml.SetNodeAttribute("Monitor/内存缓冲池",false,"总分配",false,total );
	pXml.SetNodeAttribute("Monitor/内存缓冲池",false,"未使用",false,used );
	pXml.SetNodeAttribute("Monitor/内存缓冲池",false,"每块大小",false,(int)BPCBUFSIZE );
	

	if (m_pRes->g_nMonitorHost == 1)
	{
		S_MONITOR_HOST hostinfo;
		std::vector<S_MONITOR_DISK> diskinfo;
		//取得主机信息
		GetHostInfo(hostinfo,diskinfo);
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
			m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"AddNodeByPath 出错");
			continue;
		}
		connnode->SetAttribute("ip",false,m_pRes->g_vDrebLinkInfo[i].sIp);
		connnode->SetAttribute("port",false,(int)m_pRes->g_vDrebLinkInfo[i].nPort);
		connnode->SetAttribute("index",false,i+1);
		connnode->SetAttribute("公共节点",false,m_pSockMgr.at(i)->m_nDrebId);
		connnode->SetAttribute("私有节点",false,m_pSockMgr.at(i)->m_nDrebPrivateId);
		if (m_pSockMgr.at(i)->m_sock == INVALID_SOCKET)
		{
			connnode->SetAttribute("status",false,"未连接");
		}
		else if (m_pSockMgr.at(i)->m_bChecked)
		{
			connnode->SetAttribute("status",false,"正常");
		}
		else
		{
			connnode->SetAttribute("status",false,"正在连接");
		}
	}


	S_BPC_RSMSG  mdata;//接收数据结构
	mdata.sMsgBuf = (PBPCCOMMSTRU )m_pMemPool.PoolMalloc();
	if (mdata.sMsgBuf == NULL)
	{
		m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"分配消息空间出错");
		return ;
	}
    bzero(&(mdata.sMsgBuf->sDBHead), sizeof(DREB_HEAD));
    bzero(&(mdata.sMsgBuf->sBpcHead), sizeof(S_BPC_HEAD));
	int len = DREBDATASIZE;
	if (!pXml.ToBuffer(mdata.sMsgBuf->sBuffer,len))
	{
		m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"ToBuffer出错");
		m_pMemPool.PoolFree(mdata.sMsgBuf);
		return;
	}
	mdata.sMsgBuf->sDBHead.nLen = len;
	
	m_pLog.LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"监控通知",mdata.sMsgBuf->sBuffer,mdata.sMsgBuf->sDBHead.nLen);
	
	
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
	
    if (m_spi != NULL) //有设定回调
    {
        m_spi->ProcessDrebData(mdata);
    }
    else //放入队列
    {
        m_qRcvQueue.PushData(mdata);
    }
	return;
}
int CBF_DrebServer::OnTimer(unsigned int event, void *p)
{
	CBF_DrebServer *pp = (CBF_DrebServer *)p;
	if (event == 0)
	{
		//计算每秒接收发放
		pp->ComputeStat();
	}
	else if (event == 1 )
	{
		pp->m_bIsPingTimer = true;
		pp->GetHostInfo();
	}
	return 0;
}

void CBF_DrebServer::ComputeStat()
{
	//计算
	m_pRes->ComputeStat();
}

void CBF_DrebServer::GetHostInfo()
{
	CBF_PMutex pp(&m_pHostMutex);
	m_sHostInfo.nCpuRate = m_pHostInfo.GetCpu();
	S_MemoryInf smInfo;
	m_pHostInfo.GetEms(smInfo);
	m_sHostInfo.nTotalMemory = smInfo.EmsTotal;
	m_sHostInfo.nUsedMemory = smInfo.EmsUse;
	m_vdinfo.clear();
	m_pHostInfo.GetDisk(m_vdinfo);
	m_sHostInfo.nDiskNum = m_vdinfo.size();
	//删除过期的广播信息
	m_bcSerial.Delete();
}

bool CBF_DrebServer::Start()
{
	if (m_bIsRunning)
	{
		sprintf(m_errMsg,"线程运行标志为正在运行，请Stop");
		return false;
	}
	if (!m_bIsInit)
	{
		sprintf(m_errMsg,"未初始化");
		return false;
	}
	//启动定时器
	if (!m_pTimer.Start())
	{
		sprintf(m_errMsg,"定时器启动失败");
		return false;
	}
	if (m_pLog.IsStopedThread())
	{
		if (!m_pLog.StartLog())
		{
			sprintf(m_errMsg,"启动日志线程失败");
			m_pTimer.Stop();
			return false;
		}
	}
	
	if (!m_pDrebDataLog.StartLog())
	{
		sprintf(m_errMsg,"启动总线报文日志线程失败");
		m_pTimer.Stop();
		m_pLog.StopLog();
		return false;
	}
	if (!m_pBpcCallDataLog.StartLog())
	{
		sprintf(m_errMsg,"启动外调报文日志线程失败");
		m_pTimer.Stop();
		m_pLog.StopLog();
		m_pDrebDataLog.StopLog();
		return false;
	}
	m_bIsRunning = true;
	CreateThread();
	return true;
}

void CBF_DrebServer::MonitorThread()
{
	if (m_pTimer.IsStoped())
	{
#ifdef _ENGLISH_
		m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"restart timer thread");
#else
		m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"重启定时器线程");
#endif
		m_pTimer.Start();
	}
	if (m_pLog.IsStopedThread())
	{
		m_pLog.StartLog();
#ifdef _ENGLISH_
		m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"restart log thread");
#else
		m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"重启日志线程");
#endif
	}
	if (m_pBpcCallDataLog.IsStoped())
	{
		m_pBpcCallDataLog.StartLog();
#ifdef _ENGLISH_
		m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"restart bpc datalog thread");
#else
		m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"重启BPC调用报文日志线程");
#endif
	}
	if (m_pDrebDataLog.IsStoped())
	{
		m_pDrebDataLog.StartLog();
#ifdef _ENGLISH_
		m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"restart dreb datalog thread");
#else
		m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"重启DREB报文日志线程");
#endif
	}
	if (IsStoped())
	{
#ifdef _ENGLISH_
		m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"restart api thread");
#else
		m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"重启DREBAPI线程");
#endif
		CreateThread();
	}
}
void CBF_DrebServer::Stop()
{
	m_bIsRunning = false;
	m_pTimer.Stop();
//	printf("开始退出日志线程\n");
	m_pLog.StopLog();
//	printf("结束退出日志线程\n");

//	printf("开始退出BpcCallData日志线程\n");
	m_pBpcCallDataLog.StopLog();
//	printf("开始退出DrebData日志线程\n");
	m_pDrebDataLog.StopLog();
//	printf("结束退出DATA日志线程\n");
	Join();
	if (m_pRes != NULL)
	{
		int num =m_pRes->g_vDrebLinkInfo.size();
		for (int i=0; i< num;i++)
		{
#ifdef _ENGLISH_
			m_pSockMgr.at(i)->OnClose("exit and close connect",__FILE__,__LINE__);
#else
			m_pSockMgr.at(i)->OnClose("退出关闭连接",__FILE__,__LINE__);
#endif
		}
	}
	
	
}

void CBF_DrebServer::GetHostInfo(S_MONITOR_HOST &host, std::vector<S_MONITOR_DISK>&disk)
{
	CBF_PMutex ppmutex(&m_pHostMutex);
	host.nCpuRate = m_sHostInfo.nCpuRate;
	host.nDiskNum = m_sHostInfo.nDiskNum;
	host.nTotalMemory = m_sHostInfo.nTotalMemory;
	host.nUsedMemory = m_sHostInfo.nUsedMemory;
	
	S_MONITOR_DISK pp;
	for (int i=0;i<m_sHostInfo.nDiskNum;i++)
	{
		pp.nTotalSpace = m_vdinfo[i].TotalSpace;
		pp.nUsedSpace = m_vdinfo[i].UsedSpace;
		CBF_Tools::StringCopy(pp.sDiskName,199,m_vdinfo[i].cIndex);
//		printf("name:%s Total:%d used:%d\n",pp.sDiskName,pp.nTotalSpace,pp.nUsedSpace);
		disk.push_back(pp);
	}
}

int CBF_DrebServer::SendMsg(S_BPC_RSMSG &sdata)
{
	if (!m_pSockMgr.AffirmIndex(sdata.sMsgBuf->sBpcHead.nIndex))
	{
		sdata.sMsgBuf->sBpcHead.nIndex = m_nCurIndex;
		m_nCurIndex++;
		if (m_nCurIndex >= m_nEnd)
		{
			m_nCurIndex = m_nBegin;
		}
	}
	else if (m_pSockMgr.at(sdata.sMsgBuf->sBpcHead.nIndex)->m_bChecked) //原连接正常，可以发送
	{
		return m_pSockMgr.at(sdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(sdata);
	}
	//原连接不正常，选一个正常的发送，否则还是放到原连接的队列里
	int nRet=0;
	for (int i=m_nBegin ; i< m_nEnd; i++)
	{
		if (m_pSockMgr.at(m_nCurIndex)->m_bChecked)
		{
			nRet =  m_pSockMgr.at(m_nCurIndex)->SendMsg(sdata);
			m_nCurIndex++;
			if (m_nCurIndex >= m_nEnd)
			{
				m_nCurIndex = m_nBegin;
			}
			return nRet;
		}
	}
	//没有找到合适的连接，还是用原来的连接发吧
	if (m_pSockMgr.at(sdata.sMsgBuf->sBpcHead.nIndex)->m_sock == INVALID_SOCKET || !m_pSockMgr.at(sdata.sMsgBuf->sBpcHead.nIndex)->m_bChecked )
	{
#ifdef _ENGLISH_
		m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"no DREB connect ,discard %d",sdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo);
#else
		m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"无总线连接，丢弃 %d",sdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo);
#endif
		m_pMemPool.PoolFree(sdata.sMsgBuf);
		sdata.sMsgBuf = NULL;
		return -1;
	}
	else
	{
		return m_pSockMgr.at(sdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(sdata);
	}
	
	

}

bool CBF_DrebServer::GetMsgData(S_BPC_RSMSG &rdata,unsigned int waitetms,bool get_front)
{
	return m_qRcvQueue.GetData(rdata,waitetms,get_front);
}
void CBF_DrebServer::RegisterDreb(int index, std::vector<int> *funclist)
{
	S_BPC_RSMSG rcvdata;
	S_SERVICEREG_MSG *data=NULL;
	bool bRet;
	int  offset;
	int funcNo;
	unsigned int *func=NULL;
	if (!m_pSockMgr.AffirmIndex(index))
	{
#ifdef _ENGLISH_
		m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"connect %d illegal",index);
#else
		m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"连接索引非法 %d",index);
#endif
		return;
	}

	if (m_pSockMgr.at(index)->m_sock == INVALID_SOCKET || !m_pSockMgr.at(index)->m_bChecked )
	{
#ifdef _ENGLISH_
		m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"connect [%d] not affirmed ",index);
#else
		m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"连接索引[%d]的连接未连上总线或正在连接确认 ",index);
#endif
		return;
	}
	rcvdata.sMsgBuf = (PBPCCOMMSTRU )m_pMemPool.PoolMalloc();
	if (rcvdata.sMsgBuf == NULL)
	{
#ifdef _ENGLISH_
		m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"malloc msgdata fail");
#else
		m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"分配消息空间出错");
#endif
		return ;
	}
	memset(rcvdata.sMsgBuf,0,sizeof(BPCCOMMSTRU));

	CInt listint;
	for (int i=0; i< funclist->size(); i++)
	{
		listint.Add(funclist->at(i));
	}
	if (listint.Size()<1)
	{
#ifdef _ENGLISH_
		m_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,"no txcode register");
#else
		m_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,"无交易注册");
#endif
		data = (S_SERVICEREG_MSG *)rcvdata.sMsgBuf->sBuffer;
		data->nSvrMainId = m_pRes->g_nSvrMainId;
		m_pDrebEndian.Endian2Comm((unsigned char *)&(data->nSvrMainId),sizeof(data->nSvrMainId));
		data->cSvrPrivateId = m_pRes->g_nSvrPrivateId;
		data->nFuncNum = 0;
		data->nFuncNo = 0;
		m_pDrebEndian.Endian2Comm((unsigned char *)&(data->nFuncNo),sizeof(data->nFuncNo));
		rcvdata.sMsgBuf->sDBHead.nLen = sizeof(S_SERVICEREG_MSG);
		rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN + rcvdata.sMsgBuf->sDBHead.nLen;
#ifdef _ENGLISH_
		m_pLog.LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"register data len=%d",rcvdata.sMsgBuf->sDBHead.nLen);
#else
		m_pLog.LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"连接注册发送的数据 长度=%d",rcvdata.sMsgBuf->sDBHead.nLen);
#endif
		rcvdata.sMsgBuf->sDBHead.cCmd = CMD_REGSERVICE;
		m_pSockMgr.at(index)->SendMsg(rcvdata);
		return ;
	}
	bRet = listint.First(funcNo);
	while (bRet)
	{
		bRet = listint.Next(funcNo);
	}
#ifdef _ENGLISH_
	m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"DREB index[%d] [%d %d][%s-%d] register",\
		index,m_pSockMgr.at(index)->m_nDrebId,m_pSockMgr.at(index)->m_nDrebPrivateId,\
		m_pSockMgr.at(index)->m_sDrebIp.c_str(),m_pSockMgr.at(index)->m_nDrebPort);
#else	
	m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"开始向DREB index[%d] [%d %d][%s-%d] 注册服务功能",\
		index,m_pSockMgr.at(index)->m_nDrebId,m_pSockMgr.at(index)->m_nDrebPrivateId,\
		m_pSockMgr.at(index)->m_sDrebIp.c_str(),m_pSockMgr.at(index)->m_nDrebPort);
#endif
	rcvdata.sMsgBuf->sDBHead.cCmd = CMD_REGSERVICE;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 0;
	rcvdata.sMsgBuf->sDBHead.cNextFlag = 0;
	rcvdata.sMsgBuf->sDBHead.cZip = 0;
	
	if (listint.First(funcNo))
	{
		data = (S_SERVICEREG_MSG *)rcvdata.sMsgBuf->sBuffer;
		data->nSvrMainId = m_pRes->g_nSvrMainId;
		m_pDrebEndian.Endian2Comm((unsigned char *)&(data->nSvrMainId),sizeof(data->nSvrMainId));
		data->cSvrPrivateId = m_pRes->g_nSvrPrivateId;
		data->nFuncNum = 1;
		data->nFuncNo = funcNo;
		m_pDrebEndian.Endian2Comm((unsigned char *)&(data->nFuncNo),sizeof(data->nFuncNo));
		offset = sizeof(S_SERVICEREG_MSG);
	}
	bRet = listint.Next(funcNo);
	while (bRet)
	{
		func = (unsigned int *)(rcvdata.sMsgBuf->sBuffer+offset);
		*func = funcNo;
		m_pDrebEndian.Endian2Comm((unsigned char *)func,sizeof(unsigned int));
		data->nFuncNum++;
		offset += sizeof(unsigned int);
		if (offset> BPUDATASIZE-30)
		{
			m_pMemPool.PoolFree(rcvdata.sMsgBuf);
#ifdef _ENGLISH_
			m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"too many txcode");
#else
			m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"注册功能太多,一个数据包无法在DREB上注册，请减少功能数");
#endif
			rcvdata.sMsgBuf = NULL;
			return;
		}
		bRet = listint.Next(funcNo);
	}
#ifdef _ENGLISH_
	m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"total %d function",data->nFuncNum);
#else	
	m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"总共有%d个功能注册",data->nFuncNum);
#endif
	m_pDrebEndian.Endian2Comm((unsigned char *)&(data->nFuncNum),sizeof(data->nFuncNum));
	rcvdata.sMsgBuf->sDBHead.nLen = offset;
	rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN + rcvdata.sMsgBuf->sDBHead.nLen;
#ifdef _ENGLISH_
	m_pLog.LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"register data len=%d",rcvdata.sMsgBuf->sDBHead.nLen);
#else
	m_pLog.LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"连接注册发送的数据 长度=%d",rcvdata.sMsgBuf->sDBHead.nLen);
#endif

	m_pSockMgr.at(index)->SendMsg(rcvdata);
}

void CBF_DrebServer::UnRegisterDreb()
{
	S_BPC_RSMSG rcvdata;
	S_SERVICEREG_MSG *data=NULL;

	for (int i= 0 ; i< m_pRes->g_vDrebLinkInfo.size(); i++)
	{
		if (m_pSockMgr.at(i)->m_sock == INVALID_SOCKET || !m_pSockMgr.at(i)->m_bChecked )
		{
			continue;
		}
		rcvdata.sMsgBuf = (PBPCCOMMSTRU )m_pMemPool.PoolMalloc();
		if (rcvdata.sMsgBuf == NULL)
		{
			m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"分配消息空间出错");
			return ;
		}
		memset(rcvdata.sMsgBuf,0,sizeof(BPCCOMMSTRU));
#ifdef _ENGLISH_
		m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"DREB index[%d] [%d %d][%s-%d] cancel register",\
			i,m_pSockMgr.at(i)->m_nDrebId,m_pSockMgr.at(i)->m_nDrebPrivateId,\
			m_pSockMgr.at(i)->m_sDrebIp.c_str(),m_pSockMgr.at(i)->m_nDrebPort);
#else
		m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"开始向DREB index[%d] [%d %d][%s-%d] 取消注册交易",\
			i,m_pSockMgr.at(i)->m_nDrebId,m_pSockMgr.at(i)->m_nDrebPrivateId,\
			m_pSockMgr.at(i)->m_sDrebIp.c_str(),m_pSockMgr.at(i)->m_nDrebPort);
#endif
		rcvdata.sMsgBuf->sDBHead.cCmd = CMD_UNREGSERVICE;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 0;
		rcvdata.sMsgBuf->sDBHead.cNextFlag = 0;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		data = (S_SERVICEREG_MSG *)rcvdata.sMsgBuf->sBuffer;
		data->nSvrMainId = m_pRes->g_nSvrMainId;
		m_pDrebEndian.Endian2Comm((unsigned char *)&(data->nSvrMainId),sizeof(data->nSvrMainId));
		data->cSvrPrivateId = m_pRes->g_nSvrPrivateId;
		data->nFuncNum = 1;
		data->nFuncNo = 0;
		m_pDrebEndian.Endian2Comm((unsigned char *)&(data->nFuncNo),sizeof(data->nFuncNo));
		m_pDrebEndian.Endian2Comm((unsigned char *)&(data->nFuncNum),sizeof(data->nFuncNum));
		rcvdata.sMsgBuf->sDBHead.nLen = sizeof(S_SERVICEREG_MSG);
		rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN + rcvdata.sMsgBuf->sDBHead.nLen;
		m_pSockMgr.at(i)->SendMsg(rcvdata);

	}
	return;
}
void CBF_DrebServer::GetBufferPoolInfo(int &total, int &unused, int &bufsize)
{
	m_pMemPool.PoolInfo(total,unused);
	bufsize = BPCBUFSIZE;
}

bool CBF_DrebServer::GetDrebInfo(int index, char *sip, int &port, int &drebid, int &privateid, char *status)
{
	if (!m_pSockMgr.AffirmIndex(index))
	{
		return false;
	}
	strcpy(sip,m_pSockMgr.at(index)->m_sDrebIp.c_str());
	port = m_pSockMgr.at(index)->m_nDrebPort;
	drebid = m_pSockMgr.at(index)->m_nDrebId;
	privateid = m_pSockMgr.at(index)->m_nDrebPrivateId;

	if (m_pSockMgr.at(index)->m_sock == INVALID_SOCKET)
	{
		strcpy(status,"未连接");
	}
	else if (m_pSockMgr.at(index)->m_bChecked)
	{
		strcpy(status,"正常");
	}
	else
	{
		strcpy(status,"正在连接");
	}
	return true;
}

CIErrlog * CBF_DrebServer::GetLogPoint()
{
	return &m_pLog;
}
bool CBF_DrebServer::UnzipBuf(BPCCOMMSTRU &data)
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
#ifdef _ENGLISH_
				m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Uncrypt fail");
#else
				m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解密出错");
#endif
				return false;
			}
			data.sDBHead.nLen = ddlen;
			data.sDBHead.cZip =0;
			break;
		case 2:	
			memcpy(zipDataBuf,data.sBuffer,data.sDBHead.nLen);
			if (!CBF_Tools::Uncompress((unsigned char *)(data.sBuffer),dlen,(unsigned char *)zipDataBuf,data.sDBHead.nLen))
			{
#ifdef _ENGLISH_
				m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Uncrypt fail");
#else
				m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解压缩出错");
#endif
				return false;
			}
			data.sDBHead.nLen = dlen;
			data.sDBHead.cZip =0;
			break;
		case 3:
			ddlen = data.sDBHead.nLen;
			if (m_pDes.UncryptString(data.sBuffer,ddlen,DESENCRYPTKEY3)<=0)
			{
#ifdef _ENGLISH_
				m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Uncrypt fail");
#else
				m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解密出错");
#endif
				return false;
			}
			data.sDBHead.nLen = ddlen;
			data.sDBHead.cZip =0;
			break;
		case 4:
			ddlen = data.sDBHead.nLen;
#ifdef _ENGLISH_
			m_pLog.LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"uncrypt data",data.sBuffer,data.sDBHead.nLen);
#else
			m_pLog.LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"解密前数据",data.sBuffer,data.sDBHead.nLen);
#endif
			if (m_pDes.UncryptString(data.sBuffer,ddlen,DESENCRYPTKEY)<=0)
			{
#ifdef _ENGLISH_
				m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"uncrypt fail");
#else
				m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解密出错");
#endif
				return false;
			}
			data.sDBHead.nLen = ddlen;
#ifdef _ENGLISH_
			m_pLog.LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"uncrypted data",data.sBuffer,data.sDBHead.nLen);
#else
			m_pLog.LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"解密后数据",data.sBuffer,data.sDBHead.nLen);
#endif
			memcpy(zipDataBuf,data.sBuffer,data.sDBHead.nLen);
			if (!CBF_Tools::Uncompress((unsigned char *)(data.sBuffer),dlen,(unsigned char *)zipDataBuf,data.sDBHead.nLen))
			{
#ifdef _ENGLISH_
				m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Uncompress fail");
#else
				m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解压缩出错");
#endif
				return false;
			}
			data.sDBHead.nLen = dlen;
#ifdef _ENGLISH_
			m_pLog.LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"Uncompressed data",data.sBuffer,data.sDBHead.nLen);
#else
			m_pLog.LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"解压后数据",data.sBuffer,data.sDBHead.nLen);
#endif
			data.sDBHead.cZip =0;
			break;
		default:
			return false;
		}
	}
	
	return true;
}

void CBF_DrebServer::SetDataLogLevel(int loglevel)
{
	m_pDrebDataLog.SetLogLevel(loglevel);
	m_pBpcCallDataLog.SetLogLevel(loglevel);
}

std::string CBF_DrebServer::GetErrMsg()
{
	return m_errMsg;
}

int CBF_DrebServer::GetDataLogLevel()
{
	return m_pDrebDataLog.GetLogLevel();
}
CBF_BufferPool *CBF_DrebServer::GetBufferPool()
{
	return &m_pMemPool;
}

void CBF_DrebServer::Subscribe(int index, std::vector<int>* funclist)
{
    S_BPC_RSMSG rcvdata;
    S_SERVICEREG_MSG* data = NULL;
    bool bRet;
    int  offset;
    int funcNo;
    unsigned int* func = NULL;
    if (!m_pSockMgr.AffirmIndex(index))
    {
#ifdef _ENGLISH_
        m_pLog.LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "connect %d illegal", index);
#else
        m_pLog.LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "连接索引非法 %d", index);
#endif
        return;
    }

    if (m_pSockMgr.at(index)->m_sock == INVALID_SOCKET || !m_pSockMgr.at(index)->m_bChecked)
    {
#ifdef _ENGLISH_
        m_pLog.LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "connect [%d] not affirmed ", index);
#else
        m_pLog.LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "连接索引[%d]的连接未连上总线或正在连接确认 ", index);
#endif
        return;
    }
    rcvdata.sMsgBuf = (PBPCCOMMSTRU)m_pMemPool.PoolMalloc();
    if (rcvdata.sMsgBuf == NULL)
    {
#ifdef _ENGLISH_
        m_pLog.LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "malloc msgdata fail");
#else
        m_pLog.LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "分配消息空间出错");
#endif
        return;
    }
    memset(rcvdata.sMsgBuf, 0, sizeof(BPCCOMMSTRU));

    CInt listint;
    for (int i = 0; i < funclist->size(); i++)
    {
        listint.Add(funclist->at(i));
    }
    if (listint.Size() < 1)
    {
#ifdef _ENGLISH_
        m_pLog.LogMp(LOG_PROMPT, __FILE__, __LINE__, "no broadcast subscribe");
#else
        m_pLog.LogMp(LOG_PROMPT, __FILE__, __LINE__, "无广播订阅");
#endif
        data = (S_SERVICEREG_MSG*)rcvdata.sMsgBuf->sBuffer;
        data->nSvrMainId = m_pRes->g_nSvrMainId;
        m_pDrebEndian.Endian2Comm((unsigned char*)&(data->nSvrMainId), sizeof(data->nSvrMainId));
        data->cSvrPrivateId = m_pRes->g_nSvrPrivateId;
        data->nFuncNum = 0;
        data->nFuncNo = 0;
        m_pDrebEndian.Endian2Comm((unsigned char*)&(data->nFuncNo), sizeof(data->nFuncNo));
        rcvdata.sMsgBuf->sDBHead.nLen = sizeof(S_SERVICEREG_MSG);
        rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN + rcvdata.sMsgBuf->sDBHead.nLen;
#ifdef _ENGLISH_
        m_pLog.LogMp(LOG_DEBUG + 2, __FILE__, __LINE__, "subscribe data len=%d", rcvdata.sMsgBuf->sDBHead.nLen);
#else
        m_pLog.LogMp(LOG_DEBUG + 2, __FILE__, __LINE__, "广播订阅发送的数据 长度=%d", rcvdata.sMsgBuf->sDBHead.nLen);
#endif
		rcvdata.sMsgBuf->sDBHead.cCmd = CMD_SUBSCRIBE;
        m_pSockMgr.at(index)->SendMsg(rcvdata);
        return;
    }
    bRet = listint.First(funcNo);
    while (bRet)
    {
        bRet = listint.Next(funcNo);
    }
#ifdef _ENGLISH_
    m_pLog.LogMp(LOG_DEBUG, __FILE__, __LINE__, "DREB index[%d] [%d %d][%s-%d] register", \
        index, m_pSockMgr.at(index)->m_nDrebId, m_pSockMgr.at(index)->m_nDrebPrivateId, \
        m_pSockMgr.at(index)->m_sDrebIp.c_str(), m_pSockMgr.at(index)->m_nDrebPort);
#else	
    m_pLog.LogMp(LOG_DEBUG, __FILE__, __LINE__, "开始向DREB index[%d] [%d %d][%s-%d] 订阅广播功能", \
        index, m_pSockMgr.at(index)->m_nDrebId, m_pSockMgr.at(index)->m_nDrebPrivateId, \
        m_pSockMgr.at(index)->m_sDrebIp.c_str(), m_pSockMgr.at(index)->m_nDrebPort);
#endif
    rcvdata.sMsgBuf->sDBHead.cCmd = CMD_SUBSCRIBE;
    rcvdata.sMsgBuf->sDBHead.cRaflag = 0;
    rcvdata.sMsgBuf->sDBHead.cNextFlag = 0;
    rcvdata.sMsgBuf->sDBHead.cZip = 0;

    if (listint.First(funcNo))
    {
        data = (S_SERVICEREG_MSG*)rcvdata.sMsgBuf->sBuffer;
        data->nSvrMainId = m_pRes->g_nSvrMainId;
        m_pDrebEndian.Endian2Comm((unsigned char*)&(data->nSvrMainId), sizeof(data->nSvrMainId));
        data->cSvrPrivateId = m_pRes->g_nSvrPrivateId;
        data->nFuncNum = 1;
        data->nFuncNo = funcNo;
        m_pDrebEndian.Endian2Comm((unsigned char*)&(data->nFuncNo), sizeof(data->nFuncNo));
        offset = sizeof(S_SERVICEREG_MSG);
    }
    bRet = listint.Next(funcNo);
    while (bRet)
    {
        func = (unsigned int*)(rcvdata.sMsgBuf->sBuffer + offset);
        *func = funcNo;
        m_pDrebEndian.Endian2Comm((unsigned char*)func, sizeof(unsigned int));
        data->nFuncNum++;
        offset += sizeof(unsigned int);
        if (offset > BPUDATASIZE - 30)
        {
            m_pMemPool.PoolFree(rcvdata.sMsgBuf);
#ifdef _ENGLISH_
            m_pLog.LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "too many txcode");
#else
            m_pLog.LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "注册功能太多,一个数据包无法在DREB上注册，请减少功能数");
#endif
            rcvdata.sMsgBuf = NULL;
            return;
        }
        bRet = listint.Next(funcNo);
    }
#ifdef _ENGLISH_
    m_pLog.LogMp(LOG_DEBUG, __FILE__, __LINE__, "total %d function subscribe", data->nFuncNum);
#else	
    m_pLog.LogMp(LOG_DEBUG, __FILE__, __LINE__, "总共有%d个功能广播订阅", data->nFuncNum);
#endif
    m_pDrebEndian.Endian2Comm((unsigned char*)&(data->nFuncNum), sizeof(data->nFuncNum));
    rcvdata.sMsgBuf->sDBHead.nLen = offset;
    rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN + rcvdata.sMsgBuf->sDBHead.nLen;
#ifdef _ENGLISH_
    m_pLog.LogMp(LOG_DEBUG + 2, __FILE__, __LINE__, "register data len=%d", rcvdata.sMsgBuf->sDBHead.nLen);
#else
    m_pLog.LogMp(LOG_DEBUG + 2, __FILE__, __LINE__, "广播订阅发送的数据 长度=%d", rcvdata.sMsgBuf->sDBHead.nLen);
#endif

    m_pSockMgr.at(index)->SendMsg(rcvdata);
}

void CBF_DrebServer::UnSubscribe()
{
    S_BPC_RSMSG rcvdata;
    S_SERVICEREG_MSG* data = NULL;

    for (int i = 0; i < m_pRes->g_vDrebLinkInfo.size(); i++)
    {
        if (m_pSockMgr.at(i)->m_sock == INVALID_SOCKET || !m_pSockMgr.at(i)->m_bChecked)
        {
            continue;
        }
        rcvdata.sMsgBuf = (PBPCCOMMSTRU)m_pMemPool.PoolMalloc();
        if (rcvdata.sMsgBuf == NULL)
        {
            m_pLog.LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "分配消息空间出错");
            return;
        }
        memset(rcvdata.sMsgBuf, 0, sizeof(BPCCOMMSTRU));
#ifdef _ENGLISH_
        m_pLog.LogMp(LOG_DEBUG, __FILE__, __LINE__, "DREB index[%d] [%d %d][%s-%d] unsubscribe", \
            i, m_pSockMgr.at(i)->m_nDrebId, m_pSockMgr.at(i)->m_nDrebPrivateId, \
            m_pSockMgr.at(i)->m_sDrebIp.c_str(), m_pSockMgr.at(i)->m_nDrebPort);
#else
        m_pLog.LogMp(LOG_DEBUG, __FILE__, __LINE__, "开始向DREB index[%d] [%d %d][%s-%d] 取消广播订阅", \
            i, m_pSockMgr.at(i)->m_nDrebId, m_pSockMgr.at(i)->m_nDrebPrivateId, \
            m_pSockMgr.at(i)->m_sDrebIp.c_str(), m_pSockMgr.at(i)->m_nDrebPort);
#endif
        rcvdata.sMsgBuf->sDBHead.cCmd = CMD_UNSUBSCRIBE;
        rcvdata.sMsgBuf->sDBHead.cRaflag = 0;
        rcvdata.sMsgBuf->sDBHead.cNextFlag = 0;
        rcvdata.sMsgBuf->sDBHead.cZip = 0;
        data = (S_SERVICEREG_MSG*)rcvdata.sMsgBuf->sBuffer;
        data->nSvrMainId = m_pRes->g_nSvrMainId;
        m_pDrebEndian.Endian2Comm((unsigned char*)&(data->nSvrMainId), sizeof(data->nSvrMainId));
        data->cSvrPrivateId = m_pRes->g_nSvrPrivateId;
        data->nFuncNum = 1;
        data->nFuncNo = 0;
        m_pDrebEndian.Endian2Comm((unsigned char*)&(data->nFuncNo), sizeof(data->nFuncNo));
        m_pDrebEndian.Endian2Comm((unsigned char*)&(data->nFuncNum), sizeof(data->nFuncNum));
        rcvdata.sMsgBuf->sDBHead.nLen = sizeof(S_SERVICEREG_MSG);
        rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN + rcvdata.sMsgBuf->sDBHead.nLen;
        m_pSockMgr.at(i)->SendMsg(rcvdata);

    }
    return;
}
