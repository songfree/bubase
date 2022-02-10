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
	m_pMemPool = NULL;
	m_gRes = NULL;
	m_nType = SOCK_UNKNOW;
	m_nCloseTime = 0;
	m_nPid = -1;
	m_sendNum = 10;
	m_bNeedConnect = false;
	m_nDrebHeadLen = DREBHEADLEN;
	
}

CSocketInfo::~CSocketInfo()
{
	if (m_sock != INVALID_SOCKET)
	{
		closesocket(m_sock);
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

void CSocketInfo::Init(CGateResource *res,CBF_BufferPool *pool)
{
	m_pMemPool = pool;
	m_gRes = res;


	m_log = &(m_gRes->g_pLog);
	ResetInfo();
	m_nCloseTime = 0;
	m_pDrebEndian.SetCommEndian(true);//使用主机序

	
	m_pDrebLog = &(m_gRes->g_pDrebLog);
	
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
	
	m_nPingTime = 0;
	//发送接收数据	
	
	memset(m_sRcvBuffer,0,sizeof(m_sRcvBuffer));            //接收数据缓冲   
	m_nRcvBufLen=0;                //接收的数据长度  可多次接收一个数据包
	
	memset(m_cCreateDate,0,sizeof(m_cCreateDate));//连接建立日期
	memset(m_cCloseDate,0,sizeof(m_cCloseDate));//连接关闭日期
	m_nCloseTime = 0;
	
	
	
	
}

void CSocketInfo::OnClose(const char *msg,const char *filename,int fileline)
{
	CBF_PMutex pp(&m_mutex);
	m_datetime.Update();
	m_datetime.ToCharDate(m_cCloseDate);
	
	//清空队列
	m_log->LogMp(LOG_DEBUG,filename,fileline,"%s,关闭连接[%s:%d] index[%d] type[%d] sock[%d]",\
		msg,m_sAddress,m_sock,m_index,m_nType,m_sock);
	
	closesocket(m_sock);
	m_sock = INVALID_SOCKET;
	ResetInfo();
	m_nCloseTime = time(NULL);
	if (m_nType == SOCK_DREB) //是主动建立连接
	{
		m_bNeedConnect = true; //需要连接
		CheckFirstData();
	}
	else
	{
		m_bNeedConnect = false; //需要连接
		m_pSendQueue.Clear();
	}
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

int CSocketInfo::OnSend()
{
	int length;
	char stmp[511];
	int ret;
	if (m_sock == INVALID_SOCKET)
	{
		m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"连接已断开");
		return -2;
	}
	if (!m_pSendQueue.NeedSend())
	{
		return -3;
	}
	CBF_PMutex pp(&m_mutex);
	m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"开始发送队列数据");
	RDQUEUE::iterator prq,prn;
	unsigned int atimeout;
	
	for (prn=m_pSendQueue.m_qSendData.datas.begin();prn!=m_pSendQueue.m_qSendData.datas.end();)
	{
// 		sendNum++;
// 		if (m_sendNum >0 && sendNum > m_sendNum) //如果本次已发送数超过定义，重新开始
// 		{
// 			return sendNum;
// 		}
		prq = prn;
		prn++;
		m_tcpSocket.AttachSocket(m_sock,"");
		
		//发给DREB的数据
		length = prq->totallen-prq->sendlen;
		ret = m_tcpSocket.Send((char *)(prq->data)+prq->sendlen,length);
		if (ret == length)
		{
			m_log->LogMp(LOG_PROMPT,__FILE__,__LINE__,"队列发送完成 len[%d] s_nDrebSerial[%d]",length,prq->serial);
			
			m_pMemPool->PoolFree(prq->data);
			prq->data = NULL;
			// 处理成功,停止
			m_pSendQueue.DeleteSend(prq);	
			m_nSendtime = time(NULL);
			
		}
		else //停止
		{
// 			m_pMemPool->PoolFree(prq->data);
// 			m_pSendQueue.DeleteSend(prq);	
			if (ret >0)
			{
				prq->sendlen = prq->sendlen + ret;
				m_log->LogMp(LOG_PROMPT,__FILE__,__LINE__,"发送部分数据 len[%d] s_nDrebSerial[%d]",ret,prq->serial);
			}
			m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"发送队列数据停止");
			return 0;
		}
	}
	m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"发送队列数据完成");
	return 0;
}

int CSocketInfo::Send(S_GATEDREB_RSMSG &msg,bool isimmediate)
{
	CBF_PMutex pp(&m_mutex);
	m_tcpSocket.AttachSocket(m_sock,"");
	//字节序转换及生成校验位
	if (msg.data->head.cCmd != CMD_PING && msg.data->head.cCmd != CMD_CONNECT && msg.data->head.cCmd != CMD_REGSERVICE)
	{
		m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"发送给DREB[%d %s %d]的消息 DREB命令[%s] RA标志[%d] 交易码[%d]  标识[%d %d %d] 源[%d %d] cZip[%d] 业务数据长度[%d]",\
			m_index,m_sDrebIp.c_str(),m_nDrebPort,GetDrebCmdType(msg.data->head.cCmd).c_str(),\
			msg.data->head.cRaflag,msg.data->head.d_Dinfo.d_nServiceNo,msg.data->head.s_Sinfo.s_nNodeId,\
			msg.data->head.s_Sinfo.s_cNodePrivateId,msg.data->head.s_Sinfo.s_nDrebSerial,\
			msg.data->head.s_Sinfo.s_nSerial,msg.data->head.s_Sinfo.s_nHook,\
			msg.data->head.cZip,msg.data->head.nLen);
		m_pDrebLog->LogDreb(LOG_DEBUG,msg.data,false);
	}
	else
	{
		m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"发送给DREB[%d %s %d]的消息 DREB命令[%s]  业务数据长度[%d]",\
			m_index,m_sDrebIp.c_str(),m_nDrebPort,GetDrebCmdType(msg.data->head.cCmd).c_str(),msg.data->head.nLen);
	}
	msg.serial = msg.data->head.s_Sinfo.s_nDrebSerial;
	msg.totallen = msg.data->head.nLen+DREBHEADLEN;
	msg.sendlen = msg.totallen;
	msg.offset = 0;
	
	int nRet  =0;
	//字节序转换
	if (m_gRes->g_nCrcFlag == 1)
	{
		m_pDrebEndian.Endian2CommGenCrc(&(msg.data->head));
	}
	else
	{
		m_pDrebEndian.Endian2Comm(&(msg.data->head));
	}
	
	if (isimmediate)
	{
		nRet = m_tcpSocket.Send((char *)msg.data,msg.totallen);
		if (nRet == msg.totallen)
		{
			m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"发送完成 serial[%d] [send:%d %d]",msg.serial,nRet,msg.sendlen);
			m_pMemPool->PoolFree(msg.data);
			msg.data = NULL;
			return 0;
		}
		else
		{
			if (nRet >0)
			{
				msg.sendlen = msg.totallen - nRet;
				m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"只发送部分数据 serial[%d] [send:%d %d]",msg.serial,nRet,msg.sendlen);
			}
			else
			{
				m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"发送数据失败 serial[%d] [send:%d %d]",msg.serial,nRet,msg.sendlen);
			}
			//如果队列里有数据可能就会有问题
			if (!NeedSend())
			{
				PutSendMsg(msg);
				return 1;
			}
			else
			{
				OnClose("直接在连接发送数据不成功，但连接队列里仍有数据",__FILE__,__LINE__);
				m_pMemPool->PoolFree(msg.data);
				msg.data = NULL;
				return -1;
			}
		}
		return 0;
	}
	else
	{
		if (NeedSend())
		{
			m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"队列已有数据 ，放入队列下次再发 index[%d] serial[%d]",\
				m_index,msg.serial);
			PutSendMsg(msg);
			return 1;
		}
		
		nRet = m_tcpSocket.Send((char *)msg.data,msg.totallen);
		if (nRet != msg.totallen)
		{
			if (nRet <=0)
			{
				m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"发送失败 ，放入队列再发 serial[%d]",msg.serial);
				PutSendMsg(msg);
			}
			else
			{
				msg.sendlen = msg.totallen - nRet;
				PutSendMsg(msg);
				m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"阻塞模式发送怎可能只发送部分serial[%d] [send:%d %d]",msg.serial,nRet,msg.totallen);
			}
			return 1;
		}
		m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"发送完成 serial[%d] [send:%d %d]",msg.serial,nRet,msg.sendlen);
		//释放空间
		m_pMemPool->PoolFree(msg.data);
		msg.data = NULL;
	}
	
	return 0;
}
int CSocketInfo::PutSendMsg(S_GATEDREB_RSMSG msg)
{
	int ret;
	ret = m_pSendQueue.PutSendMsg(msg);
	if (ret<0)
	{
		m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"队列满 丢弃 DREB[%d %s %d]的消息 DREB命令[%s] RA标志[%d] 交易码[%d]  标识[%d %d %d] 源[%d %d] cZip[%d] 业务数据长度[%d]",\
			m_index,m_sDrebIp.c_str(),m_nDrebPort,GetDrebCmdType(msg.data->head.cCmd).c_str(),\
			msg.data->head.cRaflag,msg.data->head.d_Dinfo.d_nServiceNo,msg.data->head.s_Sinfo.s_nNodeId,\
			msg.data->head.s_Sinfo.s_cNodePrivateId,msg.data->head.s_Sinfo.s_nDrebSerial,\
			msg.data->head.s_Sinfo.s_nSerial,msg.data->head.s_Sinfo.s_nHook,\
			msg.data->head.cZip,msg.data->head.nLen);
		m_pMemPool->PoolFree(msg.data);
		msg.data = NULL;
		return -1;
	}
	return 0;
}



int CSocketInfo::GetRecvData(S_GATEDREB_RSMSG *msg)
{
	int txlen=0;
	char tmpchar[100];
	memset(tmpchar,0,sizeof(tmpchar));
	//收到的数据大于包头
	if (m_nRcvBufLen >= DREBHEADLEN )
	{
		//进行字节序处理并校验CRC
		memcpy(&(msg->data->head),m_sRcvBuffer,DREBHEADLEN);
		if (m_gRes->g_nCrcFlag == 1)
		{
			if (!m_pDrebEndian.Endian2LocalHostChkCrc(&(msg->data->head)) )
			{
				//			OnClose("CRC错",__FILE__,__LINE__);
				return -1;
			}
		}
		else
		{
			m_pDrebEndian.Endian2LocalHost(&(msg->data->head)) ;
		}
		
		if (msg->data->head.nLen<0 || msg->data->head.nLen > DREBDATASIZE)
		{
			m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"index[%d] 报文头[%d]长度不符",m_index,msg->data->head.nLen);
			return -1;
		}
		if (m_nRcvBufLen < msg->data->head.nLen + DREBHEADLEN)
		{
			//说明数据没有接收完整
			return 0;
		}
		if (msg->data->head.nLen > 0 )
		{
			memcpy(msg->data->buffer,m_sRcvBuffer+DREBHEADLEN,msg->data->head.nLen);
		}
		//统计数据
// 		if (msg->data->head.cCmd != CMD_PING && msg->data->head.cCmd != CMD_CONNECT && msg->data->head.cCmd != CMD_REGSERVICE)
// 		{
// 			m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"从DREB[%d %d %d]收到的消息 DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 源[%d %d] 业务数据长度[%d]",\
// 				msg->data->head.s_Sinfo.s_nNodeId,msg->data->head.s_Sinfo.s_cNodePrivateId,m_index,\
// 				GetDrebCmdType(msg->data->head.cCmd).c_str(),msg->data->head.cNextFlag,\
// 				msg->data->head.cRaflag,msg->data->head.d_Dinfo.d_nServiceNo,msg->data->head.s_Sinfo.s_nNodeId,\
// 				msg->data->head.s_Sinfo.s_cNodePrivateId,msg->data->head.s_Sinfo.s_nDrebSerial,\
// 				msg->data->head.s_Sinfo.s_nSerial,msg->data->head.s_Sinfo.s_nHook,msg->data->head.nLen);
// 		}
// 		else
// 		{
// 			m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"从DREB[%d]收到的消息 DREB命令[%s]  业务数据长度[%d]",\
// 				m_index,GetDrebCmdType(msg->data->head.cCmd).c_str(),msg->data->head.nLen);
// 		}
		

		//收多了，将原包重新给m_sRcvBuffer
		for (int i=0; i<m_nRcvBufLen-DREBHEADLEN-msg->data->head.nLen; i++)
		{
			m_sRcvBuffer[i]=m_sRcvBuffer[i+DREBHEADLEN+msg->data->head.nLen];
		}
		m_nRcvBufLen=m_nRcvBufLen - DREBHEADLEN-msg->data->head.nLen;
		
		return DREBHEADLEN+msg->data->head.nLen;
	}
	else if (m_nRcvBufLen>0)
	{
		m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"index[%d] type[%s] 收到数据%d,但不完整",m_index,"BPCSOCK_DREB",m_nRcvBufLen);
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

bool CSocketInfo::NeedSend()
{
	return m_pSendQueue.NeedSend();
}
void CSocketInfo::CheckFirstData()
{
	RDQUEUE::iterator prq,prn;
	for (prn=m_pSendQueue.m_qSendData.datas.begin(); prn!=m_pSendQueue.m_qSendData.datas.end() ; )
	{
		if (prn->sendlen != prn->totallen)
		{
			m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"错误 队列中第一个数据包发送部分数据，重连后删除 %d",prn->nIndex);
			m_pSendQueue.DeleteSend(prn);
		}
		break;
	}
}