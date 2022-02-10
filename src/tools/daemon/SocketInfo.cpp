// SocketInfo.cpp: implementation of the CSocketInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "SocketInfo.h"
#include "BF_Tools.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSocketInfo::CSocketInfo()
{
	memset(m_sAddress,0,sizeof(m_sAddress));         //请求连接上来的IP
	
	ResetInfo();
	
	m_gRes = NULL;
	m_nType = MONI_SOCK_UNKNOW;
	m_nCloseTime = 0;
	m_nPid = -1;
	m_sendNum = 10;
	m_bNeedConnect = false;
	m_bIsAffirm = false;

	m_sock = INVALID_SOCKET;
}

CSocketInfo::~CSocketInfo()
{
	if (m_sock != INVALID_SOCKET)
	{
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
	}
}

std::string CSocketInfo::GetDrebCmdType(int cmdtype)
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
	case  CMD_MONITOR_BPC:     //16 //数据总线节点监控
		return "CMD_MONITOR_BPC";
	default:
		return "";
	}
}

void CSocketInfo::Init(CResource *res)
{
	m_gRes = res;
	m_log.SetLogPara(m_gRes->g_nLoglevel,m_gRes->g_sLogFilePath.c_str(),m_gRes->g_sLogFileName.c_str());
	ResetInfo();
	m_nCloseTime = 0;
	g_nCrcFlag = res->g_nCrcFlag;
	m_pDrebEndian.SetCommEndian(true);//使用主机序
}
void CSocketInfo::ResetInfo()
{
	memset(m_sAddress,0,sizeof(m_sAddress));
	memset(m_cCreateDate,0,sizeof(m_cCreateDate));
	memset(m_cCloseDate,0,sizeof(m_cCloseDate));

	memset(&m_head,0,sizeof(DREB_HEAD));
	//socket公共参数
	m_sock = INVALID_SOCKET;                   //socket id
	m_bChecked = false;
	m_bNeedread=false;              //是否需要读数据
	m_nReadtime= time(NULL);              //上次读数据时间
	m_nConntime=m_nReadtime;              //连接建立时间
	m_nSendtime=m_nReadtime;              //上次发送数据时间 
	
	//发送接收数据	
	
	memset(m_sRcvBuffer,0,sizeof(m_sRcvBuffer));            //接收数据缓冲   
	m_nRcvBufLen=0;                //接收的数据长度  可多次接收一个数据包
	
	memset(m_cCreateDate,0,sizeof(m_cCreateDate));//连接建立日期
	m_nConntime = 0;
	memset(m_cCloseDate,0,sizeof(m_cCloseDate));//连接关闭日期
	m_nCloseTime = 0;
	m_bIsAffirm = false;
	
	
}

void CSocketInfo::OnClose(const char *msg,const char *filename,int fileline)
{
	CBF_PMutex pp(&m_mutex);
	m_datetime.Update();
	m_datetime.ToCharDate(m_cCloseDate);
	
	//清空队列
	m_log.LogMp(LOG_DEBUG,filename,fileline,"%s,关闭连接[%s:%d] index[%d] type[%d] sock[%d]",\
		msg,m_sAddress,m_sock,m_index,m_nType,m_sock);
	
	closesocket(m_sock);
	m_sock = INVALID_SOCKET;
	ResetInfo();
	m_nCloseTime = time(NULL);
	
		
}

void CSocketInfo::OnShutdown()
{
	closesocket(m_sock);
	//清空队列
	ResetInfo();
	m_nCloseTime = 0;
}
bool CSocketInfo::IsCanSelect()
{
	if (m_sock != INVALID_SOCKET)
	{
		return true;
	}
	else
	{
		return false;
	}
}


bool CSocketInfo::IsFree()
{
	if (m_sock != INVALID_SOCKET)
	{
		return false;
	}
	return true;
}
int CSocketInfo::OnRecv()
{
	int ret;
	m_tcpSocket.AttachSocket(m_sock,"");
	if (m_nRcvBufLen >=0 && m_nRcvBufLen< DREBBUFSIZE)
	{
		//包头未接收完
		ret = m_tcpSocket.Receive(m_sRcvBuffer+m_nRcvBufLen,DREBBUFSIZE-m_nRcvBufLen);
		if (ret <= 0)
		{
			return ret;
		}
		m_nRcvBufLen+=ret;
		m_nReadtime =time(NULL);
		return ret;

	}
	return 0;
}

int CSocketInfo::Send(S_GWDREB_RSMSG &msg,bool isimmediate)
{
	int nRet  =0;
	CBF_PMutex pp(&m_mutex);
	m_tcpSocket.AttachSocket(m_sock,"");
	if (m_nType == MONI_SOCK_DREB)
	{
		//字节序转换及生成校验位
		if (msg.data.head.cCmd != CMD_PING && msg.data.head.cCmd != CMD_CONNECT && msg.data.head.cCmd != CMD_REGSERVICE)
		{
			m_log.LogMp(LOG_DEBUG,__FILE__,__LINE__,"发送给DREB[%d %s %d]的消息 DREB命令[%s] RA标志[%d] 交易码[%d]  标识[%d %d %d] 源[%d %d] cZip[%d] 业务数据长度[%d]",\
				m_index,m_sDrebIp.c_str(),m_nDrebPort,GetDrebCmdType(msg.data.head.cCmd).c_str(),\
				msg.data.head.cRaflag,msg.data.head.d_Dinfo.d_nServiceNo,msg.data.head.s_Sinfo.s_nNodeId,\
				msg.data.head.s_Sinfo.s_cNodePrivateId,msg.data.head.s_Sinfo.s_nDrebSerial,\
				msg.data.head.s_Sinfo.s_nSerial,msg.data.head.s_Sinfo.s_nHook,\
				msg.data.head.cZip,msg.data.head.nLen);
		}
		else
		{
			m_log.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"发送给DREB[%d %s %d]的消息 DREB命令[%s]  业务数据长度[%d]",\
				m_index,m_sDrebIp.c_str(),m_nDrebPort,GetDrebCmdType(msg.data.head.cCmd).c_str(),msg.data.head.nLen);
		}
		msg.serial = msg.data.head.s_Sinfo.s_nDrebSerial;
		msg.totallen = msg.data.head.nLen+DREBHEADLEN;
		msg.sendlen = 0;
		msg.offset = 0;
		
		//字节序转换
		m_pDrebEndian.Endian2CommGenCrc(&(msg.data.head));
		nRet = m_tcpSocket.Send((char *)&(msg.data),msg.totallen);
		if (nRet == msg.totallen)
		{
			return nRet;
		}
		else
		{
			if (nRet >0 )
			{
				msg.sendlen = msg.totallen - nRet;
				m_log.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"只发送部分数据 serial[%d] [send:%d %d]",msg.serial,nRet,msg.sendlen);
			}
			else
			{
				m_log.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"发送数据失败 serial[%d] [send:%d %d]",msg.serial,nRet,msg.sendlen);
			}
			OnClose("发送失败",__FILE__,__LINE__);
			return -1;
		}
	}
	return 0;
}
int CSocketInfo::Send(const char *data,int len)
{
	int nRet  =0;
	CBF_PMutex pp(&m_mutex);
	m_tcpSocket.AttachSocket(m_sock,"");
	if (m_nType == MONI_SOCK_DREB)
	{
		return 0;
	}
	nRet = m_tcpSocket.Send((char *)data,len);
	if (nRet == len)
	{
		return nRet;
	}
	return nRet;
}
int CSocketInfo::GetRecvData(S_GWDREB_RSMSG *msg)
{
	int txlen=0;
	char tmpchar[100];
	memset(tmpchar,0,sizeof(tmpchar));
	if (m_nType != MONI_SOCK_DREB)
	{
		return -1;
	}
	//收到的数据大于包头
	if (m_nRcvBufLen >= DREBHEADLEN )
	{
		//进行字节序处理并校验CRC
		memcpy(&(msg->data.head),m_sRcvBuffer,DREBHEADLEN);
		
		if (g_nCrcFlag == 1)
		{
			if (!m_pDrebEndian.Endian2LocalHostChkCrc(&(msg->data.head)) )
			{
//			OnClose("CRC错",__FILE__,__LINE__);
				return -1;
			}
		}
		else
		{
			m_pDrebEndian.Endian2LocalHost(&(msg->data.head));
		}

		if (m_nRcvBufLen < msg->data.head.nLen + DREBHEADLEN)
		{
			//说明数据没有接收完整
			return 0;
		}
		if (msg->data.head.nLen > 0 )
		{
			memcpy(msg->data.buffer,m_sRcvBuffer+DREBHEADLEN,msg->data.head.nLen);
		}
		//收多了，将原包重新给m_sRcvBuffer
		for (int i=0; i<m_nRcvBufLen-DREBHEADLEN-msg->data.head.nLen; i++)
		{
			m_sRcvBuffer[i]=m_sRcvBuffer[i+DREBHEADLEN+msg->data.head.nLen];
		}
		m_nRcvBufLen=m_nRcvBufLen - DREBHEADLEN-msg->data.head.nLen;
		
		return DREBHEADLEN+msg->data.head.nLen;
	}
	else if (m_nRcvBufLen>0)
	{
		m_log.LogMp(LOG_DEBUG,__FILE__,__LINE__,"index[%d] type[%s] 收到数据%d,但不完整",m_index,"BPCSOCK_DREB",m_nRcvBufLen);
	}
	return -1;
}

bool CSocketInfo::NeedConnect()
{
	return m_bNeedConnect;
}

bool CSocketInfo::NeedRead()
{
	return m_bNeedread;
}
bool CSocketInfo::Listen(bool blockflag,int queuenum)
{
	if (!m_tcpSocket.Create(AF_INET,blockflag))
	{
		CBF_Tools::StringCopy(m_errMsg,255,m_tcpSocket.GetErrorMsg().c_str());
		return false;
	}
	if (!m_tcpSocket.Listen(m_nPort,queuenum))
	{
		sprintf(m_errMsg,"侦听端口[%d]失败,侦听队列大小%d",m_nPort,queuenum);
		return false;
	}
	m_sock = m_tcpSocket.GetSocket();
#ifndef _WINDOWS
	fcntl(m_sock,F_SETFD,25);
#endif
	m_bNeedread = true;
	
	return true;
}

bool CSocketInfo::NeedSend()
{
// 	if (m_sock != INVALID_SOCKET )
// 	{
// 		return true;
// 	}
	return false;
}

void CSocketInfo::SetAccept(SOCKET_HANDLE sock,const char *address)
{
	ResetInfo();
	m_nType = MONI_SOCK_UNKNOW;
	m_sock = sock;
	m_bNeedread = true;
	m_nConntime = time(NULL);
	strcpy(m_sAddress,address);
	m_datetime.Update();
	m_datetime.ToCharDate(m_cCreateDate);
	
}
