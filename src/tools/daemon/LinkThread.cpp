// LinkThread.cpp: implementation of the CLinkThread class.
//
//////////////////////////////////////////////////////////////////////

#include "LinkThread.h"
#include "dreberrcode.h"
#include "BpcHead.h"

#define  DESENCRYPTKEY    "rd402wms"
#define  DESENCRYPTKEY3   "wms13711songfree"
char g_VERSION[20]="1.0.2.20100724";   //版本
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


#ifdef _WINDOWS
#define   INT64PFM  "%I64d"
#else
#define   INT64PFM  "%lld"
#endif


CLinkThread::CLinkThread()
{
	m_bIsRunning = false;
	m_pRes = NULL;
	
	m_bIsInit = false;
	m_sThreadName = "CLinkThread";
	m_bIsPingTimer = false;
	m_nDrebIndex = 0;
	bzero(m_sHeartBuffer,sizeof(m_sHeartBuffer));
}

CLinkThread::~CLinkThread()
{
#ifdef _WINDOWS
	m_pRes=NULL;
	WriteFile();	
#endif
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

bool CLinkThread::SetGlobalVar(CResource *res,CSocketMgr *sockmgr)
{
	m_pRes = res;
	m_pSockMgr = sockmgr;
	m_pLog.SetLogPara(m_pRes->g_nLoglevel,m_pRes->g_sLogFilePath.c_str(),m_pRes->g_sLogFileName.c_str());
	m_bIsInit = true;

	m_pProcessMgr.SetLogPara(m_pRes->g_nLoglevel,m_pRes->g_sLogFilePath.c_str(),m_pRes->g_sLogFileName.c_str());
	
	m_pTimer.Init(100,true);
	//节点路由定时器
	m_pTimer.SetTimer(0,5000,&CLinkThread::OnTimer,this); //设置定时器 
	m_pTimer.SetTimer(1,5000,&CLinkThread::OnTimer,this); //设置定时器
	m_pTimer.SetTimer(2,300000,&CLinkThread::OnTimer,this); //写文件

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
	m_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,"启动工作线程");
	while(!m_pRes->g_bIsExit)
	{
		FD_ZERO(&m_rset);
		FD_ZERO(&m_wset);
#if defined(_WINDOWS)
		FD_ZERO(&m_eset);
#endif
		
		//取得最大的socket并将socket加入到fd_set集合
		ret = GetMaxfdAddSelect(maxfd);
		if (maxfd  <=0)
		{
			for (int i=0; i<m_pRes->g_vDrebLinkInfo.size() ;i++)
			{
				OnConnect(i);
			}
			SLEEP(5);
			m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"无dreb连接 %d",tv.tv_sec);
			continue;
		}
		tv.tv_sec = 1;
		tv.tv_usec = 0;
#if defined(_WINDOWS)
		result = select(maxfd+1, &m_rset, &m_wset, &m_eset, &tv);
#else
		result = select(maxfd+1, &m_rset, &m_wset, NULL, &tv);
#endif
		
		if (result != 0) //无事件发生,处理心跳
		{
			OnEvent();
		}
		if (m_bIsPingTimer)
		{
			m_pLog.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"监控NOEVENT");
			OnNoEvent();
			OnMonitor();
			OnCheckProcess();
			m_bIsPingTimer = false;
		}

	}
	m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"停止工作线程");
	return 0;
}
int CLinkThread::GetMaxfdAddSelect(int &maxfd)
{
	int ret=0;
	maxfd = 0;
	for (int i=0;i< m_pSockMgr->m_socketList.size() ;i++)
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
			if (m_pSockMgr->at(i)->NeedConnect() || m_pSockMgr->at(i)->NeedSend())
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

	for (j=0; j< m_pSockMgr->m_socketList.size(); j++)
	{
// 		if (m_pSockMgr->at(j)->m_sock == INVALID_SOCKET)
// 		{
// 			continue;
// 		}
		if (m_pSockMgr->at(j)->m_sock != INVALID_SOCKET && m_pSockMgr->at(j)->m_nType == MONI_SOCK_DREB)
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
		else if (m_pSockMgr->at(j)->m_sock != INVALID_SOCKET && m_pSockMgr->at(j)->m_nType != MONI_SOCK_LISTEN)
		{
			tmptime = time(NULL) - m_pSockMgr->at(j)->m_nReadtime;
			if (tmptime > m_pRes->g_nGwDisTime)
			{
				OnClose(j,"连接长时间未用，断开",__FILE__,__LINE__);
			}
		}
		if (m_pRes->g_bIsExit)
		{
			return;
		}
		//若是数据总线节点连接，且没有连上则去连接
		if (m_pSockMgr->at(j)->m_nType == MONI_SOCK_DREB )
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
	for (i= 0; i< m_pSockMgr->m_socketList.size() ; i++)
	{
		if (m_pSockMgr->at(i)->m_sock == INVALID_SOCKET)//未连接
		{
			if (m_pSockMgr->at(i)->m_nType == MONI_SOCK_DREB  ) //若是数据总线节点连接，则去连接
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
		
		if (m_pSockMgr->at(conindex)->m_nType == MONI_SOCK_LISTEN)
		{
			OnAccept(conindex);
		}
		else
		{
			OnRecv(conindex);
		}
		
	}
	else //此连接无读事件
	{
		if (m_pSockMgr->at(conindex)->m_sock != INVALID_SOCKET)
		{
			//没有检查通过
			if (!m_pSockMgr->at(conindex)->m_bChecked)
			{
				//数据总线节点连接
				if (m_pSockMgr->at(conindex)->m_nType == MONI_SOCK_DREB) 
				{
					if (time(NULL)-m_pSockMgr->at(conindex)->m_nReadtime >= m_pRes->g_nHeartRun)
					{
						if (m_pSockMgr->at(conindex)->NeedConnect())
						{
							m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"尝试连接到%s:%d未成功",m_pSockMgr->at(conindex)->m_sDrebIp.c_str(),m_pSockMgr->at(conindex)->m_nDrebPort);
							closesocket(m_pSockMgr->at(conindex)->m_sock);
							m_pSockMgr->at(conindex)->m_sock = INVALID_SOCKET;
						}
					}
				}// end if (m_pSockMgr->at(conindex)->m_nType == BPCSOCK_TODREB) //数据总线节点连接
			}
		}// end if (m_pSockMgr->at(conindex)->m_sock != INVALID_SOCKET)
	}// end if (ret == 1) //有读事件
}

void CLinkThread::OnWriteEvent(int conindex)
{
	int ret;
	if (m_pSockMgr->at(conindex)->m_sock == INVALID_SOCKET)
	{
		return;
	}
	if (m_pSockMgr->at(conindex)->m_nType == MONI_SOCK_LISTEN )
	{
		return;
	}
#if defined(_WINDOWS)
	ret = FD_ISSET(m_pSockMgr->at(conindex)->m_sock, &m_eset);
	if (ret != 0)
	{
		//主动连接其它的DREB
		if (m_pSockMgr->at(conindex)->m_nType == MONI_SOCK_DREB )
		{
			m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"尝试连接到%s:%d未成功",m_pSockMgr->at(conindex)->m_sDrebIp.c_str(),m_pSockMgr->at(conindex)->m_nDrebPort);
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
			if (m_pSockMgr->at(conindex)->m_nType == MONI_SOCK_DREB ) 
			{
				OnConnected(conindex);//通知连接已成功
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
				if (m_pSockMgr->at(conindex)->m_nType == MONI_SOCK_DREB)
				{
					OnConnected(conindex);//去连接数据总线节点
				}
			}
			else
			{
				if (m_pSockMgr->at(conindex)->m_nType == MONI_SOCK_DREB )
				{
					m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"尝试连接到%s:%d未成功",m_pSockMgr->at(conindex)->m_sDrebIp.c_str(),m_pSockMgr->at(conindex)->m_nDrebPort);
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
	else if (m_pSockMgr->at(conindex)->m_bChecked)
	{
		OnSend(conindex);//发送数据
	}
#endif

				
}

void CLinkThread::OnPing(int conindex)
{
	if (m_pSockMgr->at(conindex)->m_nType == MONI_SOCK_DREB)
	{
		if (time(NULL) - m_pSockMgr->at(conindex)->m_nPingTime < BEATHEARTTICK) //小于5秒不用发ping
		{
			return ;
		}
		m_pSockMgr->at(conindex)->m_nPingTime = time(NULL);
		if (!ResetData(conindex,&m_pDataBuf))
		{
			return;
		}
		m_pDataBuf.data.head.cRaflag = 0;
		m_pDataBuf.data.head.cZip = 0;
		m_pDataBuf.data.head.nLen = 0;
		m_pDataBuf.data.head.cCmd = CMD_PING;
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
	//发送
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
	m_pSockMgr->at(conindex)->m_nConntime = time(NULL);
	m_pSockMgr->at(conindex)->m_bNeedConnect = true;
	m_tcpSocket.Create(AF_INET,false);//异步方式
	if (m_pSockMgr->at(conindex)->m_nType == MONI_SOCK_DREB)
	{
		m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"尝试连接到数据总线节点%s:%d:%d",m_pSockMgr->at(conindex)->m_sDrebIp.c_str(),m_pSockMgr->at(conindex)->m_nDrebPort,m_pSockMgr->at(conindex)->m_sock);
	}
	else
	{
		m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"尝试连接到转出目标 %s:%d:%d",m_pSockMgr->at(conindex)->m_sDrebIp.c_str(),m_pSockMgr->at(conindex)->m_nDrebPort,m_pSockMgr->at(conindex)->m_sock);
	}
	
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
			m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"等待建立到%s:%d的连接",m_pSockMgr->at(conindex)->m_sDrebIp.c_str(),m_pSockMgr->at(conindex)->m_nDrebPort);
			
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
		m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"建立到%s:%d的连接成功",m_pSockMgr->at(conindex)->m_sDrebIp.c_str(),m_pSockMgr->at(conindex)->m_nDrebPort);
		//连接成功后发送CMD_CONNECT消息，确认连接的类型
		m_pSockMgr->at(conindex)->m_bNeedConnect = false;
		
		if (m_pSockMgr->at(conindex)->m_nType != MONI_SOCK_DREB)
		{
			m_pSockMgr->at(conindex)->m_bChecked = true;
			m_pSockMgr->at(conindex)->m_bNeedread = true;
			return;
		}
		//发送连接消息
		m_pSockMgr->at(conindex)->m_nPingTime = time(NULL);
		if (!ResetData(conindex,&m_pDataBuf))
		{
			return;
		}
		
		m_pDataBuf.data.head.cRaflag = 0;
		m_pDataBuf.data.head.cZip = 0;
		m_pDataBuf.data.head.cCmd = CMD_CONNECT;
		m_pDataBuf.data.head.cDrebAffirm = 0;
	
		
		m_pDataBuf.data.head.nLen = sizeof(S_CONNECT_DATA);
		S_CONNECT_DATA *data = (S_CONNECT_DATA *)m_pDataBuf.data.buffer;
		data->cConnectType = 1;
		data->nSvrMainId = m_pRes->g_nSvrMainId;
		data->cSvrPrivateId = m_pRes->g_nSvrPrivateId;
		sprintf(data->sVersion,"%s",g_VERSION);
		m_pDrebEndian.Endian2Comm((unsigned char *)&(data->nSvrMainId),sizeof(data->nSvrMainId));
		
		if (m_pSockMgr->at(conindex)->Send(m_pDataBuf,true) >= 0)
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
	int i=0;
	char tmpchar[200];
	if (m_pSockMgr->at(conindex)->m_nType == MONI_SOCK_LISTEN )
	{
		return;
	}
	ret = m_pSockMgr->at(conindex)->OnRecv();	
	if (ret<=0)
	{
		sprintf(tmpchar,"%s 远端关闭，接收数据不完整 ret=[%d]",m_pSockMgr->at(conindex)->m_sAddress,ret);
		OnClose(conindex,tmpchar,__FILE__,__LINE__);
		return;
	}
	if (m_pSockMgr->at(conindex)->m_nType != MONI_SOCK_DREB )
	{
		//处理接收的心跳
		int heartLen=0;
		//收到的数据大于包头
		while (m_pSockMgr->at(conindex)->m_nRcvBufLen >= HEARTHEAD )
		{
			bzero(tmpchar,sizeof(tmpchar));
			memcpy(tmpchar,m_pSockMgr->at(conindex)->m_sRcvBuffer,HEARTHEAD);
			CBF_Tools::LRtrim(tmpchar);
			heartLen = atoi(tmpchar);
			if (ret <=0 )
			{
				m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"心跳包不符[%s]",tmpchar);
				OnClose(conindex,"心跳包不符",__FILE__,__LINE__);
				return ;
			}

			if (m_pSockMgr->at(conindex)->m_nRcvBufLen < (heartLen+HEARTHEAD) )
			{
				//说明数据没有接收完整
				if (m_pSockMgr->at(conindex)->m_bIsAffirm)
				{
					m_pRes->UpdateLastTimeById(m_pSockMgr->at(conindex)->m_sNodeId);
				}
				//继续收
				return;
			}
			if (heartLen < 65534)
			{
				memcpy(m_sHeartBuffer,m_pSockMgr->at(conindex)->m_sRcvBuffer+HEARTHEAD,heartLen);
			}
			else
			{
				OnClose(conindex,"心跳包不符",__FILE__,__LINE__);
				return ;
			}
			//收多了，将原包重新给m_sRcvBuffer
			for (int i=0; i<m_pSockMgr->at(conindex)->m_nRcvBufLen-HEARTHEAD-heartLen; i++)
			{
				m_pSockMgr->at(conindex)->m_sRcvBuffer[i]=m_pSockMgr->at(conindex)->m_sRcvBuffer[i+HEARTHEAD+heartLen];
			}
			m_pSockMgr->at(conindex)->m_nRcvBufLen=m_pSockMgr->at(conindex)->m_nRcvBufLen - HEARTHEAD-heartLen;

			//处理心跳包
			m_sHeartBuffer[heartLen]=0;
			if (m_pSockMgr->at(conindex)->m_bIsAffirm) 
			{
				//之前已经收到过心跳包，已确认过进程ID,这次就不用去解析报文了
				m_pRes->UpdateLastTimeById(m_pSockMgr->at(conindex)->m_sNodeId);
				continue;
			}
			m_pLog.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"心跳包[%s]",m_sHeartBuffer);
			bool bRet=false;
			if (m_sHeartBuffer[0] == '#')
			{
				bRet = m_pKvList.ParseNode(m_sHeartBuffer+1);
			}
			else
			{
				bRet = m_pKvList.ParseNode(m_sHeartBuffer);
			}
			if (!bRet)
			{
				m_pLog.LogBin(LOG_WARNNING,__FILE__,__LINE__,"错误的心跳包",m_sHeartBuffer,heartLen);
				OnClose(conindex,"心跳包格式不符，应为#分隔的KV报文",__FILE__,__LINE__);
				return ;
			}
			std::string snodeid;
			if (m_pKvList.GetValueByName("node_id",snodeid) == NULL)
			{
				m_pLog.LogBin(LOG_WARNNING,__FILE__,__LINE__,"错误的心跳包",m_sHeartBuffer,heartLen);
				OnClose(conindex,"心跳包格式不符，没有node_id字段",__FILE__,__LINE__);
				return;
			}
			if (m_pRes->UpdateLastTimeById(snodeid))
			{
				m_pSockMgr->at(conindex)->m_bIsAffirm = true;
				m_pSockMgr->at(conindex)->m_sNodeId = snodeid;
			}
			else
			{
				m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"node_id的值[%s]在本程序中未配置",snodeid.c_str());
				OnClose(conindex,"node_id不符",__FILE__,__LINE__);
				return;
			}
		
		}
		return;
	}
	//开始接收处理
	if (m_pSockMgr->at(conindex)->m_nRcvBufLen >= DREBHEADLEN)
	{
		m_pLog.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"开始接收处理");
	}
	while (m_pSockMgr->at(conindex)->m_nRcvBufLen >= DREBHEADLEN)
	{
		if (!ResetData(conindex,&m_pDataRcv))
		{
			return;
		}
		ret = m_pSockMgr->at(conindex)->GetRecvData(&m_pDataRcv);
		if (ret >0 ) //有数据
		{
			m_pDataRcv.nIndex = conindex;
			//接收dreb过来的数据
			if (m_pDataRcv.data.head.cCmd == CMD_PING || m_pDataRcv.data.head.cCmd == CMD_CONNECT) //响应PING应答
			{
				if (m_pDataRcv.data.head.cRaflag == 0) //请求
				{
					m_pDataRcv.data.head.cRaflag = 1;
					m_pSockMgr->at(conindex)->Send(m_pDataRcv);
				}
				else //应答，什么都不用做
				{
					if ( m_pDataRcv.data.head.cCmd == CMD_CONNECT)
					{
						m_pSockMgr->at(conindex)->m_nDrebId = m_pDataRcv.data.head.a_Ainfo.a_nNodeId;
						m_pSockMgr->at(conindex)->m_nDrebPrivateId = m_pDataRcv.data.head.a_Ainfo.a_cNodePrivateId;
					}
				}
				continue; ;
			}
			m_pDataRcv.nIndex = conindex;
			ProcessDreb(m_pDataRcv);
			m_pLog.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"处理完一条数据  缓冲数据长度[%d]",m_pSockMgr->at(conindex)->m_nRcvBufLen);

		}
		else  //ret <=0出错或未接收完整
		{
			if (ret <0)
			{
				OnClose(conindex,"CRC校验错",__FILE__,__LINE__);
			}
			m_pLog.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"退出接收处理  缓冲数据长度[%d]",m_pSockMgr->at(conindex)->m_nRcvBufLen);
			break;
		}
	}
	
	return ;
}


bool CLinkThread::ResetData(int index,S_GWDREB_RSMSG *data)
{
	data->offset = 0;
	data->rtime = time(NULL);
	data->nIndex = index;
	return true;
}


void CLinkThread::ProcessDreb(S_GWDREB_RSMSG &rcvdata)
{
	if (rcvdata.data.head.cCmd == CMD_MONITOR_BPC)
	{
		if (rcvdata.data.head.cCmd != CMD_PING && rcvdata.data.head.cCmd != CMD_CONNECT && rcvdata.data.head.cCmd != CMD_REGSERVICE)
		{
			m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"从DREB[%d %d %d]收到的消息 DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 源[%d %d] 业务数据长度[%d]",\
				rcvdata.data.head.s_Sinfo.s_nNodeId,rcvdata.data.head.s_Sinfo.s_cNodePrivateId,rcvdata.nIndex,\
				GetDrebCmdType(rcvdata.data.head.cCmd).c_str(),rcvdata.data.head.cNextFlag,\
				rcvdata.data.head.cRaflag,rcvdata.data.head.d_Dinfo.d_nServiceNo,rcvdata.data.head.s_Sinfo.s_nNodeId,\
				rcvdata.data.head.s_Sinfo.s_cNodePrivateId,rcvdata.data.head.s_Sinfo.s_nDrebSerial,\
				rcvdata.data.head.s_Sinfo.s_nSerial,rcvdata.data.head.s_Sinfo.s_nHook,rcvdata.data.head.nLen);
		}
		else
		{
			m_pLog.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"接收到DREB[%d]的消息 DREB命令[%s]  业务数据长度[%d]",\
				rcvdata.nIndex,GetDrebCmdType(rcvdata.data.head.cCmd).c_str(),rcvdata.data.head.nLen);
		}
		if (!UnzipBuf(&(rcvdata.data)))
		{
			m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解密出错");
			return;
		}
		//m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"CMD_MONITOR_BPC [%d]",rcvdata.data.head.d_Dinfo.d_nServiceNo);
		CallBpcLocalFunc(rcvdata);
		return;
	}
	m_pLog.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"丢弃交易 %d",rcvdata.data.head.cCmd );
	return;
}


void CLinkThread::CallBpcLocalFunc(S_GWDREB_RSMSG &rcvdata)
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
	switch (rcvdata.data.head.d_Dinfo.d_nServiceNo)
	{
		case BPCMONI_INFO:
			MonitorInfo(rcvdata);
			break;
		case BPCMONI_HOST:
			MonitorHost(rcvdata);
			break;
			
		case BPCMONI_STOPBPU:
			MonitorStopBpu(rcvdata);
			break;
		case BPCMONI_STARTBPU:
			MonitorStartBpu(rcvdata);
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
			rcvdata.data.head.cRaflag = 1;
			rcvdata.data.head.cZip = 0;
			rcvdata.data.head.a_Ainfo.a_nRetCode = ERR_FUNCNOTFUND;
			rcvdata.data.head.nLen = 0;
			m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
			break;
	}
	

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
		char zipDataBuf[DREBDATASIZE];
		bzero(zipDataBuf,sizeof(zipDataBuf));
		unsigned int dlen = DREBDATASIZE;
		int ddlen;
		switch (data->head.cZip)
		{
			case 1:
				ddlen = data->head.nLen;
				if (m_pDes.UncryptString(data->buffer,ddlen,DESENCRYPTKEY)<1)
				{
					m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解密出错");
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
					m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解密出错");
					return false;
				}
				data->head.nLen = ddlen;
				data->head.cZip =0;
				break;
			case 4:
				ddlen = data->head.nLen ;
				if (m_pDes.UncryptString(data->buffer,ddlen,DESENCRYPTKEY3)<1)
				{
					m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解密出错");
					return false;
				}
				data->head.nLen = ddlen;
				data->head.cZip = 2;
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
}
int CLinkThread::OnTimer(unsigned int eventid, void *p)
{
	CLinkThread *pp = (CLinkThread *)p;
	if (eventid == 0)
	{
		pp->GetHostInfo();
	}
	else if (eventid == 1)
	{
		pp->PingTimer();
	}
	else
	{
		pp->WriteFile();
	}
	return 0;
}
void CLinkThread::PingTimer()
{
	//OnNoEvent();
	m_pLog.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"监控定时器");
	m_bIsPingTimer = true;
}

void CLinkThread::OnMonitor()
{
	//监控信息

	if (m_pRes->g_nUseMonitor != 1)
	{
		return;
	}
	int i;
	CBF_Xml        pXml;
	pXml.SetNodeValueByPath("Monitor/公共节点",false,(int)m_pRes->g_nSvrMainId);
	pXml.SetNodeValueByPath("Monitor/私有节点",false,(int)m_pRes->g_nSvrPrivateId);
	pXml.SetNodeValueByPath("Monitor/节点类型",false,2);//类bpc
	pXml.SetNodeValueByPath("Monitor/报告类型",false,0);//正常
	pXml.SetNodeValueByPath("Monitor/启动时间",false,m_pRes->m_sStartDate,false);
	
	
	//取主机信息
	if (m_pRes->g_nMonitorHost == 1)
	{
		pXml.SetNodeValueByPath("Monitor/主机资源/CPU",false,(int)m_sHostInfo.nCpuRate);
		pXml.SetNodeAttribute("Monitor/主机资源/内存",false,"total",false,(int)m_sHostInfo.nTotalMemory);
		pXml.SetNodeAttribute("Monitor/主机资源/内存",false,"used",false,(int)m_sHostInfo.nUsedMemory);
		for (i=0 ; i< m_vdinfo.size() ; i++)
		{
			CXmlNode *disknode = pXml.AddNodeByPath("Monitor/主机资源/磁盘",false,"磁盘",false,"");
			if (disknode != NULL)
			{
				disknode->SetAttribute("drive",false,m_vdinfo[i].cIndex,true);
				disknode->SetAttribute("total",false,(int)m_vdinfo[i].TotalSpace);
				disknode->SetAttribute("used",false,(int)m_vdinfo[i].UsedSpace);
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
	std::string extdata;
	char tmpchar[1000];
	for (i=0 ; i< m_pRes->g_vProcessInfo.size(); i++)
	{
		sprintf(tmpchar,"进程名称:%s,进程ID:%d,当前时间:%d,最后心跳时间:%d",\
			m_pRes->g_vProcessInfo[i].processname.c_str(),m_pRes->g_vProcessInfo[i].pid,\
			time(NULL),m_pRes->g_vProcessInfo[i].lastUseTIme);
		extdata = tmpchar;
		sprintf(tmpchar,"Monitor/扩展信息/%s",m_pRes->g_vProcessInfo[i].nodeid.c_str());
		pXml.SetNodeAttribute(tmpchar,false,"status",false,m_pRes->g_vProcessInfo[i].status.c_str(),false);
		pXml.SetNodeValueByPath(tmpchar,false,extdata.c_str(),false);
	}
// 	<扩展信息 name="子数据不能重名">
//         <DATA status="0">连接池最大连接数22 空闲连接数21 客户连接数0 内存缓冲池总分配100 未使用100 每块大小65534</DATA>
//         <DATA2 status="0">连接池最大连接数22 空闲连接数21 客户连接数0 内存缓冲池总分配100 未使用100 每块大小65534</DATA2>
//     </扩展信息>

	S_GWDREB_RSMSG drebdata;
	bzero(&drebdata,sizeof(S_GWDREB_RSMSG));
	
	drebdata.rtime = time(NULL);
	int len = DREBDATASIZE;
	if (!pXml.ToBuffer(drebdata.data.buffer,len))
	{
		m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"ToBuffer出错");
		return;
	}
	m_pLog.LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"监控数据内容",drebdata.data.buffer,len);
	
	//报文头
	drebdata.data.head.nLen = len;
	drebdata.data.head.cCmd = CMD_DPPUSH;
	drebdata.data.head.cZip = 0;
	drebdata.data.head.cDrebAffirm = 0;
	drebdata.data.head.cNextFlag = 0; //是否是取后续包
	drebdata.data.head.cRaflag = 0;//请求应答标志
	
	drebdata.data.head.d_Dinfo.d_cNodePrivateId = m_pRes->g_nMonitorDrebPrivateId;
	drebdata.data.head.d_Dinfo.d_cSvrPrivateId = m_pRes->g_nMonitorSvrPrivateId;
	drebdata.data.head.d_Dinfo.d_nNodeId = m_pRes->g_nMonitorDrebId;
	drebdata.data.head.d_Dinfo.d_nServiceNo = m_pRes->g_nMonitorTxCode;
	drebdata.data.head.d_Dinfo.d_nSvrMainId = m_pRes->g_nMonitorSvrId;
	//通过DREB连接发送数据
	int index;
	for ( i=0 ; i< 3 ; i++)
	{
		index = GetDrebIndex();
		if (index <0)
		{
			return ;
		}
		if (m_pSockMgr->at(index)->m_bChecked) 
		{
			m_pSockMgr->at(index)->Send(drebdata);
			return;
		}
	}
	return;
}

void CLinkThread::MonitorInfo(S_GWDREB_RSMSG &rcvdata)
{
	int i;
	CBF_Xml xmlpack;
	xmlpack.SetNodeValueByPath("Monitor/BPC公共节点",false,(int)m_pRes->g_nSvrMainId);
	xmlpack.SetNodeValueByPath("Monitor/BPC私有节点",false,(int)m_pRes->g_nSvrPrivateId);
	xmlpack.SetNodeValueByPath("Monitor/启动时间",false,m_pRes->m_sStartDate,false);
	
	//连接信息
	for (i=0 ; i < m_pRes->g_vDrebLinkInfo.size() ; i++)
	{
		CXmlNode *connnode = xmlpack.AddNodeByPath("Monitor/连接信息",false,"连接",false,"");
		if (connnode == NULL)
		{
			m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"AddNodeByPath 出错");
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
	
	int len = DREBDATASIZE;
	if (!xmlpack.ToBuffer(rcvdata.data.buffer,len))
	{
		sprintf(rcvdata.data.buffer,"缓冲不足，请下载cgateinfo.txt查看");
		rcvdata.data.head.nLen = strlen(rcvdata.data.buffer);
		m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"缓冲不足");
		xmlpack.ToFile("cgateinfo.txt");
	}
	else
	{
		rcvdata.data.head.nLen = len;
	}
	rcvdata.data.head.cRaflag = 1;
	rcvdata.data.head.cZip = 0;
	rcvdata.data.head.a_Ainfo.a_nRetCode = SUCCESS;
	m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
}

void CLinkThread::MonitorHost(S_GWDREB_RSMSG &rcvdata)
{
	int i;
	CBF_Xml xmlpack;
	xmlpack.SetNodeValueByPath("Monitor/BPC公共节点",false,(int)m_pRes->g_nSvrMainId);
	xmlpack.SetNodeValueByPath("Monitor/BPC私有节点",false,(int)m_pRes->g_nSvrPrivateId);
	xmlpack.SetNodeValueByPath("Monitor/启动时间",false,m_pRes->m_sStartDate,false);


	//取主机信息
	if (m_pRes->g_nMonitorHost == 1)
	{
		xmlpack.SetNodeValueByPath("Monitor/主机资源/CPU",false,(int)m_sHostInfo.nCpuRate);
		xmlpack.SetNodeAttribute("Monitor/主机资源/内存",false,"total",false,(int)m_sHostInfo.nTotalMemory);
		xmlpack.SetNodeAttribute("Monitor/主机资源/内存",false,"used",false,(int)m_sHostInfo.nUsedMemory);
		for (i=0 ; i< m_vdinfo.size() ; i++)
		{
			CXmlNode *disknode = xmlpack.AddNodeByPath("Monitor/主机资源/磁盘",false,"磁盘",false,"");
			if (disknode != NULL)
			{
				disknode->SetAttribute("drive",false,m_vdinfo[i].cIndex,true);
				disknode->SetAttribute("total",false,(int)m_vdinfo[i].TotalSpace);
				disknode->SetAttribute("used",false,(int)m_vdinfo[i].UsedSpace);
			}
		}
	}
	int len = DREBDATASIZE;
	if (!xmlpack.ToBuffer(rcvdata.data.buffer,len))
	{
		sprintf(rcvdata.data.buffer,"缓冲不足，请下载cgatehost.txt查看");
		rcvdata.data.head.nLen = strlen(rcvdata.data.buffer);
		m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"缓冲不足");
		xmlpack.ToFile("cgatehost.txt");
	}
	else
	{
		rcvdata.data.head.nLen = len;
	}
	rcvdata.data.head.cRaflag = 1;
	rcvdata.data.head.cZip = 0;
	rcvdata.data.head.a_Ainfo.a_nRetCode = SUCCESS;
	m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
}


void CLinkThread::MonitorReport(S_GWDREB_RSMSG &rcvdata)
{
	rcvdata.data.head.nLen =0;
	rcvdata.data.head.cRaflag = 1;
	rcvdata.data.head.cZip = 0;
	rcvdata.data.head.a_Ainfo.a_nRetCode = SUCCESS;
	m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
}

void CLinkThread::MonitorBpu(S_GWDREB_RSMSG &rcvdata)
{
	int i;
	CBF_Xml xmlpack;
	xmlpack.SetNodeValueByPath("Monitor/BPC公共节点",false,(int)m_pRes->g_nSvrMainId);
	xmlpack.SetNodeValueByPath("Monitor/BPC私有节点",false,(int)m_pRes->g_nSvrPrivateId);
	xmlpack.SetNodeValueByPath("Monitor/启动时间",false,m_pRes->m_sStartDate,false);

	char tmpchar[200];
	for ( i=0 ; i<m_pRes->g_vProcessInfo.size() ; i++ )
	{
		sprintf(tmpchar,"Monitor/进程列表/%s",m_pRes->g_vProcessInfo[i].nodeid.c_str());
		xmlpack.SetNodeAttribute(tmpchar,false,"status",false,m_pRes->g_vProcessInfo[i].status.c_str());

		sprintf(tmpchar,"Monitor/进程列表/%s/进程名称",m_pRes->g_vProcessInfo[i].nodeid.c_str());
		xmlpack.SetNodeValueByPath(tmpchar,false,m_pRes->g_vProcessInfo[i].processname.c_str(),false);

		sprintf(tmpchar,"Monitor/进程列表/%s/程序名",m_pRes->g_vProcessInfo[i].nodeid.c_str());
		xmlpack.SetNodeValueByPath(tmpchar,false,m_pRes->g_vProcessInfo[i].prgname.c_str(),false);

		sprintf(tmpchar,"Monitor/进程列表/%s/程序路径",m_pRes->g_vProcessInfo[i].nodeid.c_str());
		xmlpack.SetNodeValueByPath(tmpchar,false,m_pRes->g_vProcessInfo[i].prgpath.c_str(),false);

		sprintf(tmpchar,"Monitor/进程列表/%s/运行目录",m_pRes->g_vProcessInfo[i].nodeid.c_str());
		xmlpack.SetNodeValueByPath(tmpchar,false,m_pRes->g_vProcessInfo[i].runpath.c_str(),false);

		sprintf(tmpchar,"Monitor/进程列表/%s/启动参数",m_pRes->g_vProcessInfo[i].nodeid.c_str());
		xmlpack.SetNodeValueByPath(tmpchar,false,m_pRes->g_vProcessInfo[i].prgpara.c_str(),false);

		sprintf(tmpchar,"Monitor/进程列表/%s/停止脚本",m_pRes->g_vProcessInfo[i].nodeid.c_str());
		xmlpack.SetNodeValueByPath(tmpchar,false,m_pRes->g_vProcessInfo[i].stopscript.c_str(),false);

		sprintf(tmpchar,"Monitor/进程列表/%s/无心跳是否重启",m_pRes->g_vProcessInfo[i].nodeid.c_str());
		xmlpack.SetNodeValueByPath(tmpchar,false,m_pRes->g_vProcessInfo[i].heartstart.c_str(),false);

		sprintf(tmpchar,"Monitor/进程列表/%s/是否接受控制",m_pRes->g_vProcessInfo[i].nodeid.c_str());
		xmlpack.SetNodeValueByPath(tmpchar,false,m_pRes->g_vProcessInfo[i].acceptcontrol.c_str(),false);

		sprintf(tmpchar,"Monitor/进程列表/%s/进程ID",m_pRes->g_vProcessInfo[i].nodeid.c_str());
		xmlpack.SetNodeValueByPath(tmpchar,false,m_pRes->g_vProcessInfo[i].pid);

		sprintf(tmpchar,"Monitor/进程列表/%s/最后心跳时间",m_pRes->g_vProcessInfo[i].nodeid.c_str());
		xmlpack.SetNodeValueByPath(tmpchar,false,m_pRes->g_vProcessInfo[i].lastUseTIme);

		sprintf(tmpchar,"Monitor/进程列表/%s/当前时间",m_pRes->g_vProcessInfo[i].nodeid.c_str());
		xmlpack.SetNodeValueByPath(tmpchar,false,(int)time(NULL));
		
	}
	int len = DREBDATASIZE;
	if (!xmlpack.ToBuffer(rcvdata.data.buffer,len))
	{
		sprintf(rcvdata.data.buffer,"缓冲不足，请下载clientlist.txt查看");
		rcvdata.data.head.nLen = strlen(rcvdata.data.buffer);
		m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"缓冲不足");
		xmlpack.ToFile("clientlist.txt");
	}
	else
	{
		rcvdata.data.head.nLen = len;
	}
	rcvdata.data.head.cRaflag = 1;
	rcvdata.data.head.cZip = 0;
	rcvdata.data.head.a_Ainfo.a_nRetCode = SUCCESS;
	m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
}

void CLinkThread::Download(S_GWDREB_RSMSG &rcvdata)
{
	char databuf[DREBDATASIZE];
	
	if (rcvdata.data.head.cZip == 2)
	{
		memcpy(databuf,rcvdata.data.buffer,rcvdata.data.head.nLen);
		unsigned int dlen = DREBDATASIZE;
		if (!CBF_Tools::Uncompress((unsigned char *)rcvdata.data.buffer,dlen,(unsigned char *)databuf,rcvdata.data.head.nLen))
		{
			sprintf(rcvdata.data.buffer,"数据解压缩出错");
			rcvdata.data.head.nLen = strlen(rcvdata.data.buffer);
			rcvdata.data.head.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
			rcvdata.data.head.cRaflag = 1;
			rcvdata.data.head.cZip = 0;
			m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
			return ;
		}
	}
	S_BPC_DUFILE sfile;
	if (rcvdata.data.head.nLen  < sizeof(S_BPC_DUFILE))
	{
		m_pLog.LogBin(LOG_ERROR,__FILE__,__LINE__,"请求数据非下载数据",\
			rcvdata.data.buffer,rcvdata.data.head.nLen);
		sprintf(rcvdata.data.buffer,"请求数据非下载数据");
		rcvdata.data.head.nLen = strlen(rcvdata.data.buffer);
		rcvdata.data.head.a_Ainfo.a_nRetCode = ERR_MONITOR_DOWNLOAD;
		rcvdata.data.head.cRaflag = 1;
		rcvdata.data.head.cZip = 0;
		m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
		return ;
	}
	memcpy(&sfile,rcvdata.data.buffer,sizeof(S_BPC_DUFILE));
	struct_stat fstat;
	FSTAT(sfile.sFileName,&fstat);
	if (fstat.st_size <= 0)
	{
		sprintf(rcvdata.data.buffer,"文件[%s]不存在",sfile.sFileName);
		rcvdata.data.head.nLen = strlen(rcvdata.data.buffer);
		rcvdata.data.head.a_Ainfo.a_nRetCode = ERR_MONITOR_DOWNLOAD;
		rcvdata.data.head.cRaflag = 1;
		rcvdata.data.head.cZip = 0;
		m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
		return ;
	}
	int offset = atol(sfile.sFileOffset);
	if (offset >= fstat.st_size)
	{
		sprintf(rcvdata.data.buffer,"文件偏移量[%d]已大于等于文件大小",offset);
		rcvdata.data.head.nLen = strlen(rcvdata.data.buffer);
		rcvdata.data.head.a_Ainfo.a_nRetCode = ERR_MONITOR_DOWNLOAD;
		rcvdata.data.head.cRaflag = 1;
		rcvdata.data.head.cZip = 0;
		m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
		return ;
	}
	sprintf(sfile.sFileSize,"%011ld",fstat.st_size);
	FILE *fp = fopen(sfile.sFileName,"rb");
	if (fp == NULL)
	{
		sprintf(rcvdata.data.buffer,"文件[%s]不存在",sfile.sFileName);
		rcvdata.data.head.nLen = strlen(rcvdata.data.buffer);
		rcvdata.data.head.a_Ainfo.a_nRetCode = ERR_MONITOR_DOWNLOAD;
		rcvdata.data.head.cRaflag = 1;
		rcvdata.data.head.cZip = 0;
		m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
		return ;
	}
	fseek(fp,offset,1);
	int getlen = fread(rcvdata.data.buffer+sizeof(S_BPC_DUFILE),1,50*1024,fp);
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
	memcpy(rcvdata.data.buffer,&sfile,sizeof(S_BPC_DUFILE));
	rcvdata.data.head.nLen = sizeof(S_BPC_DUFILE)+getlen;
	
	rcvdata.data.head.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.data.head.cRaflag = 1;
	rcvdata.data.head.cZip = 0;
	m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);

}

void CLinkThread::Upload(S_GWDREB_RSMSG &rcvdata)
{
	char databuf[DREBDATASIZE];
	if (rcvdata.data.head.cZip == 2)
	{
		memcpy(databuf,rcvdata.data.buffer,rcvdata.data.head.nLen);
		unsigned int dlen = DREBDATASIZE;
		if (!CBF_Tools::Uncompress((unsigned char *)rcvdata.data.buffer,dlen,(unsigned char *)databuf,rcvdata.data.head.nLen))
		{
			sprintf(rcvdata.data.buffer,"数据解压缩出错");
			rcvdata.data.head.nLen = strlen(rcvdata.data.buffer);
			rcvdata.data.head.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
			rcvdata.data.head.cRaflag = 1;
			rcvdata.data.head.cZip = 0;
			m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
			return ;
		}
	}
	S_BPC_DUFILE sfile;
	if (rcvdata.data.head.nLen  <= sizeof(S_BPC_DUFILE))
	{
		m_pLog.LogBin(LOG_ERROR,__FILE__,__LINE__,"请求数据非上传数据",\
			rcvdata.data.buffer,rcvdata.data.head.nLen);
		sprintf(rcvdata.data.buffer,"请求数据非上传数据");
		rcvdata.data.head.nLen = strlen(rcvdata.data.buffer);
		rcvdata.data.head.a_Ainfo.a_nRetCode = ERR_MONITOR_UPLOAD;
		rcvdata.data.head.cRaflag = 1;
		rcvdata.data.head.cZip = 0;
		m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
		return ;
	}
	memcpy(&sfile,rcvdata.data.buffer,sizeof(S_BPC_DUFILE));
	m_pLog.LogBin(LOG_DEBUG,__FILE__,__LINE__,"上传文件",rcvdata.data.buffer,sizeof(S_BPC_DUFILE));
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
		sprintf(rcvdata.data.buffer,"文件[%s]打开出错",sfile.sFileName);
		rcvdata.data.head.nLen = strlen(rcvdata.data.buffer);
		rcvdata.data.head.a_Ainfo.a_nRetCode = ERR_MONITOR_UPLOAD;
		rcvdata.data.head.cRaflag = 1;
		rcvdata.data.head.cZip = 0;
		m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
		return ;
	}
	fwrite(rcvdata.data.buffer+sizeof(S_BPC_DUFILE),1,rcvdata.data.head.nLen-sizeof(S_BPC_DUFILE),fp);
	fclose(fp);
	if (sfile.cEndFlag == '1')
	{
		struct_stat fstat;
		FSTAT(sfile.sFileName,&fstat);
		if (atol(sfile.sFileSize) != fstat.st_size)
		{
			sprintf(rcvdata.data.buffer,"上传文件[%s]失败，大小[%d]不等于[%s]",sfile.sFileName,fstat.st_size,sfile.sFileSize);
			rcvdata.data.head.nLen = strlen(rcvdata.data.buffer);
			rcvdata.data.head.a_Ainfo.a_nRetCode = ERR_MONITOR_UPLOAD;
			rcvdata.data.head.cRaflag = 1;
			rcvdata.data.head.cZip = 0;
		m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
			return ;
		}
		sprintf(rcvdata.data.buffer,"上传文件[%s]成功",sfile.sFileName);
		rcvdata.data.head.nLen = strlen(rcvdata.data.buffer);
		rcvdata.data.head.a_Ainfo.a_nRetCode = SUCCESS;
		rcvdata.data.head.cRaflag = 1;
		rcvdata.data.head.cZip = 0;
		m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
		return ;
	}
	else
	{
		sprintf(rcvdata.data.buffer,"写入文件[%s]成功",sfile.sFileName);
		rcvdata.data.head.nLen = strlen(rcvdata.data.buffer);
		rcvdata.data.head.a_Ainfo.a_nRetCode = SUCCESS;
		rcvdata.data.head.cRaflag = 1;
		rcvdata.data.head.cZip = 0;
		m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
		return ;
	}
}
void CLinkThread::GetHostInfo()
{
	if (m_pRes->g_nMonitorHost !=1 )
	{
		return;
	}
	m_sHostInfo.nCpuRate = m_pHost.GetCpu();
	S_MemoryInf smInfo;
	m_pHost.GetEms(smInfo);
	m_sHostInfo.nTotalMemory = smInfo.EmsTotal;
	m_sHostInfo.nUsedMemory = smInfo.EmsUse;
	m_vdinfo.clear();
	m_pHost.GetDisk(m_vdinfo);
	m_sHostInfo.nDiskNum = m_vdinfo.size();
	
}
void CLinkThread::OnAccept(int conindex)
{
	char address[32];
	SOCKET_HANDLE tsock=INVALID_SOCKET;
	int i;
	bool havefree=false;
	m_tcpSocket.AttachSocket(m_pSockMgr->at(conindex)->m_sock,"");
	tsock= m_tcpSocket.Accept(address,false);
	if (tsock != INVALID_SOCKET)
	{
		m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"从[%d]端口接收到[%s]的连接",m_pSockMgr->at(conindex)->m_nPort,address);
		for (i=m_pRes->g_vDrebLinkInfo.size()+1; i <m_pSockMgr->m_socketList.size(); i++)
		{
			if ( m_pSockMgr->at(i)->m_sock == INVALID_SOCKET )
			{
				havefree = true;
				m_pSockMgr->at(i)->SetAccept(tsock,address);
				break;
			}
		}
		if (havefree)
		{
			m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"接收从%s的连接sockid=%d index=%d ",address,tsock,i);
		}
		else
		{
			m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"连接已满，从%s的连接%d被拒绝",address,tsock);
			closesocket(tsock);
		}
		return;
	}
	return ;
}

int CLinkThread::GetDrebIndex()
{
	if (m_pRes->g_vDrebLinkInfo.size()<1)
	{
		return -1;
	}
	m_nDrebIndex++;
	if (m_nDrebIndex >= m_pRes->g_vDrebLinkInfo.size())
	{
		m_nDrebIndex = 0;
	}
	return m_nDrebIndex;
}

void CLinkThread::WriteFile()
{
	char tmpchar[200];
	if (m_pRes == NULL)
	{
		return ;
	}
	for (int i=0; i< m_pRes->g_vProcessInfo.size() ; i++)
	{
		sprintf(tmpchar,"daemon/%s/pid",m_pRes->g_vProcessInfo[i].nodeid.c_str());
		m_pRes->g_pRunXml.SetNodeValueByPath(tmpchar,false,m_pRes->g_vProcessInfo[i].pid);
		sprintf(tmpchar,"daemon/%s/lasttime",m_pRes->g_vProcessInfo[i].nodeid.c_str());
		m_pRes->g_pRunXml.SetNodeValueByPath(tmpchar,false,m_pRes->g_vProcessInfo[i].lastUseTIme);
		sprintf(tmpchar,"daemon/%s/curtime",m_pRes->g_vProcessInfo[i].nodeid.c_str());
		m_pRes->g_pRunXml.SetNodeValueByPath(tmpchar,false,(int)time(NULL));
	}
	sprintf(tmpchar,"%s/monirun.xml",m_pRes->g_sCurPath);
	m_pRes->g_pRunXml.ToFile(tmpchar);
}

void CLinkThread::MonitorStopBpu(S_GWDREB_RSMSG &rcvdata)
{
	if (rcvdata.data.head.cZip != 0)
	{
		if (!UnzipBuf(&(rcvdata.data)))
		{
			sprintf(rcvdata.data.buffer,"数据解压缩出错");
			rcvdata.data.head.nLen = strlen(rcvdata.data.buffer);
			rcvdata.data.head.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
			rcvdata.data.head.cRaflag = 1;
			rcvdata.data.head.cZip = 0;
			m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
			return ;
		}
	}
	CBF_Xml  xmlpack;
	if (!xmlpack.FromBuffer(rcvdata.data.buffer))
	{
		sprintf(rcvdata.data.buffer,"请求数据非xml");
		rcvdata.data.head.nLen = strlen(rcvdata.data.buffer);
		rcvdata.data.head.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
		rcvdata.data.head.cRaflag = 1;
		rcvdata.data.head.cZip = 0;
		m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
		return ;
	}
	CXmlNode *stopnode = xmlpack.GetNodeByPath("Monitor/StopBpu",false);
	if (stopnode == NULL)
	{
		sprintf(rcvdata.data.buffer,"xml无[Monitor/StopBpu]节点");
		rcvdata.data.head.nLen = strlen(rcvdata.data.buffer);
		rcvdata.data.head.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
		rcvdata.data.head.cRaflag = 1;
		rcvdata.data.head.cZip = 0;
		m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
		return ;
	}
	int i;
	int bpuindex=-1;
	std::string bpugroupname;
	int bpugnum=0;
	S_PROCESS_NODE node;
	bool isfind = false;
	CXmlNode *bpunode = (CXmlNode *)stopnode->GetFirstChild();
	while (bpunode != NULL)
	{
		bpugroupname = bpunode->GetNodeName();
		isfind=false;
		for (i=0; i< m_pRes->g_vProcessInfo.size() ; i++)
		{
			if (bpugroupname.compare(m_pRes->g_vProcessInfo[i].nodeid) == 0)
			{
				isfind = true;
				bpuindex = i;
				break;
			}
		}
		if (isfind)
		{
			if (m_pRes->g_vProcessInfo[bpuindex].acceptcontrol.compare("1") != 0)
			{
				m_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,"[%s %s]进程[%d]配置为不接受控制",\
					m_pRes->g_vProcessInfo[bpuindex].processname.c_str(),\
					m_pRes->g_vProcessInfo[bpuindex].nodeid.c_str(),m_pRes->g_vProcessInfo[bpuindex].pid);
				continue;
			}
			if (m_pProcessMgr.FindProcess(&(m_pRes->g_vProcessInfo[bpuindex]),m_errMsg) == 0)
			{
				m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"原[%s %s]进程[%d]已存在，停止",\
					m_pRes->g_vProcessInfo[bpuindex].processname.c_str(),\
					m_pRes->g_vProcessInfo[bpuindex].nodeid.c_str(),m_pRes->g_vProcessInfo[bpuindex].pid);
				if (m_pRes->g_vProcessInfo[bpuindex].stopscript.length()>0) //有停止脚本
				{
					system(m_pRes->g_vProcessInfo[bpuindex].stopscript.c_str());
					if (m_pProcessMgr.FindProcess(&(m_pRes->g_vProcessInfo[bpuindex]),m_errMsg) == 0) //没有此进程
					{
						m_pRes->g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"进程[%s] PID[%d] 调用停止脚本[%s] 等待停止",\
							m_pRes->g_vProcessInfo[bpuindex].nodeid.c_str(),m_pRes->g_vProcessInfo[bpuindex].pid,\
							m_pRes->g_vProcessInfo[bpuindex].stopscript.c_str());
					}
				}
				else if (m_pProcessMgr.StopProcess(&(m_pRes->g_vProcessInfo[bpuindex]),m_errMsg)!=0)
				{
					m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"原[%s %s]进程[%d]停止失败 %s",\
						m_pRes->g_vProcessInfo[bpuindex].processname.c_str(),\
						m_pRes->g_vProcessInfo[bpuindex].nodeid.c_str(),m_pRes->g_vProcessInfo[bpuindex].pid,m_errMsg);
				}
				m_pRes->g_vProcessInfo[bpuindex].controltype = "1";
			}
		}
		bpunode = (CXmlNode *)bpunode->getNextSibling();
	}
	sprintf(rcvdata.data.buffer,"停止完成，请查看进程");
	rcvdata.data.head.nLen = strlen(rcvdata.data.buffer);
	rcvdata.data.head.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.data.head.cRaflag = 1;
	rcvdata.data.head.cZip = 0;
	m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
	return;
}

void CLinkThread::MonitorStartBpu(S_GWDREB_RSMSG &rcvdata)
{
	if (rcvdata.data.head.cZip != 0)
	{
		if (!UnzipBuf(&(rcvdata.data)))
		{
			sprintf(rcvdata.data.buffer,"数据解压缩出错");
			rcvdata.data.head.nLen = strlen(rcvdata.data.buffer);
			rcvdata.data.head.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
			rcvdata.data.head.cRaflag = 1;
			rcvdata.data.head.cZip = 0;
			m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
			return ;
		}
	}
	CBF_Xml  xmlpack;
	if (!xmlpack.FromBuffer(rcvdata.data.buffer))
	{
		sprintf(rcvdata.data.buffer,"请求数据非xml");
		rcvdata.data.head.nLen = strlen(rcvdata.data.buffer);
		rcvdata.data.head.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
		rcvdata.data.head.cRaflag = 1;
		rcvdata.data.head.cZip = 0;
		m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
		return ;
	}
	CXmlNode *stopnode = xmlpack.GetNodeByPath("Monitor/StartBpu",false);
	if (stopnode == NULL)
	{
		sprintf(rcvdata.data.buffer,"xml无[Monitor/StartBpu]节点");
		rcvdata.data.head.nLen = strlen(rcvdata.data.buffer);
		rcvdata.data.head.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
		rcvdata.data.head.cRaflag = 1;
		rcvdata.data.head.cZip = 0;
		m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
		return ;
	}
	int i;
	int bpuindex=-1;
	std::string bpugroupname;
	int bpugnum=0;
	S_PROCESS_NODE node;
	bool isfind = false;
	CXmlNode *bpunode = (CXmlNode *)stopnode->GetFirstChild();
	while (bpunode != NULL)
	{
		bpugroupname = bpunode->GetNodeName();
		isfind=false;
		for (i=0; i< m_pRes->g_vProcessInfo.size() ; i++)
		{
			if (bpugroupname.compare(m_pRes->g_vProcessInfo[i].nodeid) == 0)
			{
				isfind = true;
				bpuindex = i;
				break;
			}
		}
		if (isfind)
		{
			if (m_pProcessMgr.FindProcess(&(m_pRes->g_vProcessInfo[bpuindex]),m_errMsg) != 0)
			{
				m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"[%s %s]进程[%d]不存在 %s",\
					m_pRes->g_vProcessInfo[bpuindex].processname.c_str(),\
					m_pRes->g_vProcessInfo[bpuindex].nodeid.c_str(),m_pRes->g_vProcessInfo[bpuindex].pid,m_errMsg);
				if (!m_pProcessMgr.StartProcess(&(m_pRes->g_vProcessInfo[bpuindex]),m_errMsg))
				{
					m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"[%s %s]进程[%d]启动失败 %s",\
						m_pRes->g_vProcessInfo[bpuindex].processname.c_str(),\
						m_pRes->g_vProcessInfo[bpuindex].nodeid.c_str(),m_pRes->g_vProcessInfo[bpuindex].pid,m_errMsg);
				}
				else
				{
					m_pRes->g_vProcessInfo[bpuindex].controltype = "0";
				}
			}
		}
		bpunode = (CXmlNode *)bpunode->getNextSibling();
	}
	sprintf(rcvdata.data.buffer,"启动BPU成功");
	rcvdata.data.head.nLen = strlen(rcvdata.data.buffer);
	rcvdata.data.head.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.data.head.cRaflag = 1;
	rcvdata.data.head.cZip = 0;
	m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
	return;
}

void CLinkThread::OnCheckProcess()
{
	//查看管理的进程
	for (int i=0 ; i< m_pRes->g_vProcessInfo.size() ; i++)
	{
		if (m_pProcessMgr.FindProcess(&(m_pRes->g_vProcessInfo[i]),m_errMsg) != 0) //没有此进程
		{
			if (m_pRes->g_vProcessInfo[i].controltype.compare("1") == 0)
			{
				m_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,"[%s %s]进程 状态为客户端控制",\
					m_pRes->g_vProcessInfo[i].processname.c_str(),\
					m_pRes->g_vProcessInfo[i].nodeid.c_str());
				continue;
			}
			m_pRes->g_vProcessInfo[i].status = "1";
 			//20170223 若有停止脚本，先调用停止脚本停一次
 			if (m_pRes->g_vProcessInfo[i].stopscript.length()>0) //有停止脚本
 			{
 				system(m_pRes->g_vProcessInfo[i].stopscript.c_str());
 			}

			if (!m_pProcessMgr.StartProcess(&(m_pRes->g_vProcessInfo[i]),m_errMsg))
			{
				m_pRes->g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"启动进程[%s]失败 %s",\
					m_pRes->g_vProcessInfo[i].nodeid.c_str(),m_errMsg);
			}
			else
			{
				m_pRes->g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,"启动进程[%s]成功 PID[%d]",\
					m_pRes->g_vProcessInfo[i].nodeid.c_str(),m_pRes->g_vProcessInfo[i].pid);
				m_pRes->g_vProcessInfo[i].status = "0";
				m_pRes->g_vProcessInfo[i].controltype = "0";
				m_pRes->g_vProcessInfo[i].lastUseTIme = time(NULL)+300;
			}
		}
		else //有此进程
		{
			m_pRes->g_pLog.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"进程[%s] PID[%d] 已存在",\
					m_pRes->g_vProcessInfo[i].nodeid.c_str(),m_pRes->g_vProcessInfo[i].pid);
			if (m_pRes->g_vProcessInfo[i].heartstart.compare("1") == 0) //无心跳重启
			{
				if (time(NULL) - m_pRes->g_vProcessInfo[i].lastUseTIme > m_pRes->g_nHeartRun)
				{
					m_pRes->g_vProcessInfo[i].status = "2";
				}
				if (time(NULL) - m_pRes->g_vProcessInfo[i].lastUseTIme > m_pRes->g_nHeartRun*3)
				{
					m_pRes->g_vProcessInfo[i].status = "1";
					if (m_pRes->g_vProcessInfo[i].stopscript.length()>0) //有停止脚本
					{
						system(m_pRes->g_vProcessInfo[i].stopscript.c_str());
						if (m_pProcessMgr.FindProcess(&(m_pRes->g_vProcessInfo[i]),m_errMsg) != 0) //没有此进程
						{
							if (!m_pProcessMgr.StartProcess(&(m_pRes->g_vProcessInfo[i]),m_errMsg))
							{
								m_pRes->g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"重启进程[%s]失败 %s",\
									m_pRes->g_vProcessInfo[i].nodeid.c_str(),m_errMsg);
							}
							else
							{
								m_pRes->g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,"重启进程[%s]成功 PID[%d]",\
									m_pRes->g_vProcessInfo[i].nodeid.c_str(),m_pRes->g_vProcessInfo[i].pid);
								m_pRes->g_vProcessInfo[i].status = "0";
								m_pRes->g_vProcessInfo[i].controltype = "0";
								m_pRes->g_vProcessInfo[i].lastUseTIme = time(NULL)+300;
							}
						}
						else
						{
							m_pRes->g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,"进程[%s] PID[%d] 调用停止脚本[%s] 等待停止",\
								m_pRes->g_vProcessInfo[i].nodeid.c_str(),m_pRes->g_vProcessInfo[i].pid,m_pRes->g_vProcessInfo[i].stopscript.c_str());
						}
					}
					else
					{
						if (m_pProcessMgr.StopProcess(&(m_pRes->g_vProcessInfo[i]),m_errMsg) == 0)
						{
							m_pRes->g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,"进程[%s] PID[%d] 无心跳重启调用KILL停止成功",\
								m_pRes->g_vProcessInfo[i].nodeid.c_str(),m_pRes->g_vProcessInfo[i].pid);
							if (!m_pProcessMgr.StartProcess(&(m_pRes->g_vProcessInfo[i]),m_errMsg))
							{
								m_pRes->g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"重启进程[%s]失败 %s",\
									m_pRes->g_vProcessInfo[i].nodeid.c_str(),m_errMsg);
							}
							else
							{
								m_pRes->g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,"重启进程[%s]成功 PID[%d]",\
									m_pRes->g_vProcessInfo[i].nodeid.c_str(),m_pRes->g_vProcessInfo[i].pid);
								m_pRes->g_vProcessInfo[i].status = "0";
								m_pRes->g_vProcessInfo[i].controltype = "0";
								m_pRes->g_vProcessInfo[i].lastUseTIme = time(NULL)+300;
							}
						}
					}
				}
				else
				{
					m_pRes->g_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"进程[%s] PID[%d] 已存在 心跳已[%d]秒未收到",\
						m_pRes->g_vProcessInfo[i].nodeid.c_str(),m_pRes->g_vProcessInfo[i].pid,time(NULL) - m_pRes->g_vProcessInfo[i].lastUseTIme);
				}
			}
		}
	}
}
