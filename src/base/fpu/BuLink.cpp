// BuLink.cpp: implementation of the CBuLink class.
//
//////////////////////////////////////////////////////////////////////

#include "BuLink.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBuLink::CBuLink()
{
	m_nConnNum = 0;
	
	m_nRcvBufLen = 0;
	nRcvBufLen = 0;
	
	m_nDataLen = 0;
	m_nConntime=0;              //连接建立时间
	m_nReadtime=0;              //上次读数据时间
	m_nSendtime=0;              //上次发送数据时间 
	m_pBuProc.SetPara(this);
	m_bIsConnect = false;
	
	m_sRcvBuffer = (char *)malloc(BFBUFSIZE);            //接收数据缓冲   
	
	sRcvBuffer = (char *)malloc(BFBUFSIZE);            //接收数据缓冲   
	
	m_sReqAnsData=(char *)malloc(BFBUFSIZE);            //发送数据缓冲   
	
	m_sSendBuffer=(char *)malloc(BFBUFSIZE);            //发送数据缓冲   

}

CBuLink::~CBuLink()
{
	free(m_sRcvBuffer);
	free(sRcvBuffer);
	free(m_sSendBuffer);
	free(m_sReqAnsData);
	m_sRcvBuffer = NULL;
	sRcvBuffer = NULL;
	m_sSendBuffer = NULL;
	m_sReqAnsData = NULL;
	m_tcpSocket.Close();
}


bool CBuLink::IsSocketEvent(int sec)
{
	struct timeval tv;
	int result;
	if (m_sock == INVALID_SOCKET)
	{
		return false;
	}
	FD_ZERO(&m_rset);

	tv.tv_sec = sec;
	tv.tv_usec = 0;
	
	FD_SET(m_sock, &m_rset);//加入到读select集合

#if defined(_WINDOWS)
	result = select(m_sock+1, &m_rset, NULL, NULL, &tv);
#else
	result = select(m_sock+1, &m_rset, NULL, NULL, &tv);
#endif
	if (result == 0)
	{
		return false;
	}
	else
	{
		result = FD_ISSET(m_sock,&m_rset);
		if (result != 0) //有读事件
		{
			return true;
		}
		return false;
	}
}

long CBuLink::GetThreadShareSerial()
{
	int ret;
	S_BF_RSMSG_FPC data;
	char databuf[BFBUFSIZE];
	memset(&data,0,sizeof(S_BF_RSMSG_FPC));
	data.msgbuf.message = (PCOMMSTRU_FPC)databuf;
	data.msgbuf.head.len = 0;
	data.msgbuf.head.msgtype = MSG_FREE;
	if (OnSend(&data)<0)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送MSG_FREE失败"); 
		return -1;
	}
	int lasttime = time(NULL);
	if (IsSocketEvent(5))
	{
		ret = OnRecv(&data);
		if (ret <=0)
		{
			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"连接有事件但无数据，关闭连接 [%d]",ret);
			OnClose("连接有事件但无数据，关闭连接");
			m_nConnNum = 6;//退出主程序，重启BPU,因为不知道是连接多少次了
			return -1;
		}
		if (ret>=sizeof(S_MSG_HEAD_FPC))
		{
			data.msgbuf.message->buffer[data.msgbuf.head.len]=0;
			return atol(data.msgbuf.message->buffer);
		}
	}
	return -1;
}
int CBuLink::Connect()
{
	m_tcpSocket.Create(AF_INET);
	m_nConntime = time(NULL);
	if (!m_tcpSocket.ConnectServer((char *)m_pRes.g_sBccIp.c_str(),m_pRes.g_nPort))
	{
		m_nConnNum++;
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"连接BFFPC %s %d 失败",(char *)m_pRes.g_sBccIp.c_str(),m_pRes.g_nPort);
		m_bIsConnect = false;
		return -1;
	}
	//非阻塞模式
	m_tcpSocket.SetBlockFlag(false);
	m_sock = m_tcpSocket.GetSocket();
	
	m_nReadtime = m_nConntime;
	m_nSendtime = m_nConntime;
	S_BF_RSMSG_FPC data;
	memset(&data,0,sizeof(S_BF_RSMSG_FPC));
	data.msgbuf.message = NULL;
	data.msgbuf.head.len = 0;
	data.msgbuf.head.msgtype = MSG_FREE;
	data.msgbuf.head.rtime = GetPid();
	if (OnSend(&data) != 0)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送MSG_FREE失败");
		m_tcpSocket.Close();
		m_bIsConnect = true;
		m_nConnNum++;
		return -1;
	}
	else
	{
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"连接BFFPC %s %d 成功",(char *)m_pRes.g_sBccIp.c_str(),m_pRes.g_nPort);
	}
	m_nConnNum = 0;
	m_bIsConnect = true;
	return 0;
}


void CBuLink::Stop()
{
	m_tcpSocket.Close();
}

int CBuLink::Run()
{
	struct timeval tv;
	int result;
	int i,j;
	int maxfd=0;
	int ret=0;
	int lasttime=time(NULL);
	if (m_nConnNum >5 )
	{
		m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"连接次数[%d]超过5次，退出BPU",m_nConnNum);
		return 1;
	}
	while (!m_bIsConnect)
	{
		ret = Connect();
		if (ret != 0 )
		{
			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"连接BPC失败 重连次数[%d]",m_nConnNum);
			//连接失败次数超过5次，退出bpu
			if (m_nConnNum >5 )
			{
				m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"连接次数[%d]超过5次，退出BPU",m_nConnNum);
				return 1;
			}
			//每次重连间隔2秒
			SLEEP_SECONDS(2);
		}
		break;
	}
	
	if (IsSocketEvent(1))
	{
		OnRecv();
	}
	else //无事件发生,处理心跳
	{
		OnNoEvent();
	}
	return 0;
}

void CBuLink::OnClose(const char *msg)
{
	m_tcpSocket.Close();
	m_sock = INVALID_SOCKET;
	m_bIsConnect = false;
	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"%s",msg);
}
void CBuLink::Reverse(unsigned char *buf, int len)
{
	int i;
	unsigned char buf1[100];
	if (htonl(1) == 1)
	{
		for(i=0;i<len;i++)
		{
			buf1[i] = buf[len-i-1];
		}
		memcpy(buf,buf1,len);
	}
}
void CBuLink::OnRecv()
{
	int ret;
	int txlen=0;
	char tmpchar[100];
	memset(tmpchar,0,sizeof(tmpchar));
	m_tcpSocket.AttachSocket(m_sock,"");
	if (m_nRcvBufLen >=0 && m_nRcvBufLen< BFBUFSIZE)
	{
		//包头未接收完
		ret = m_tcpSocket.Receive(m_sRcvBuffer+m_nRcvBufLen,BFBUFSIZE-m_nRcvBufLen);
		if (ret <= 0)
		{
			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"连接有事件但无数据，关闭连接 [%d]",ret);
			OnClose("连接有事件但无数据，关闭连接");
			m_nConnNum = 6;//退出主程序，重启BPU,因为不知道是连接多少次了
			return ;
		}
		m_nRcvBufLen+=ret;
		m_nReadtime =time(NULL);
		
	}
	ResetData();

	//收到的数据大于包头
	while (m_nRcvBufLen >= sizeof(S_MSG_HEAD_FPC))
	{
		memcpy(&(m_pHeadData.msgbuf.head),m_sRcvBuffer,sizeof(S_MSG_HEAD_FPC));
		Reverse((unsigned char *)&(m_pHeadData.msgbuf.head.msgtype),sizeof(m_pHeadData.msgbuf.head.msgtype));
		Reverse((unsigned char *)&(m_pHeadData.msgbuf.head.connecttime),sizeof(m_pHeadData.msgbuf.head.connecttime));
		Reverse((unsigned char *)&(m_pHeadData.msgbuf.head.index),sizeof(m_pHeadData.msgbuf.head.index));
		Reverse((unsigned char *)&(m_pHeadData.msgbuf.head.len),sizeof(m_pHeadData.msgbuf.head.len));
		Reverse((unsigned char *)&(m_pHeadData.msgbuf.head.buindex),sizeof(m_pHeadData.msgbuf.head.buindex));
		Reverse((unsigned char *)&(m_pHeadData.msgbuf.head.rtime),sizeof(m_pHeadData.msgbuf.head.rtime));
		if (m_nRcvBufLen < sizeof(S_MSG_HEAD_FPC)+m_pHeadData.msgbuf.head.len)
		{
			if (m_nRcvBufLen >= BFBUFSIZE || m_pHeadData.msgbuf.head.len>=BFBUFSIZE )
			{
				OnClose("连接发送数据非法，非BFFPC连接");
				return ;
			}
			//说明数据没有接收完整
			return ;
		}
		//记录交易信息
//		m_pLog->LogSysBin(LOG_DEBUG+1,m_sRcvBuffer,m_pHeadData.msgbuf.head.len+sizeof(S_MSG_HEAD));
		if (m_pHeadData.msgbuf.message == NULL)
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"m_pHeadData.msgbuf.message为NULL");
			return ;
		}
		memcpy(m_pHeadData.msgbuf.message->buffer,m_sRcvBuffer+sizeof(S_MSG_HEAD_FPC),m_pHeadData.msgbuf.head.len);
		m_pHeadData.msgbuf.message->buffer[m_pHeadData.msgbuf.head.len]=0;
		m_nDataLen = m_pHeadData.msgbuf.head.len;
		//记入接收日志

		m_pLog->LogBin(LOG_DEBUG-1,__FILE__,__LINE__,"接收到的BFFPC请求",m_pHeadData.msgbuf.message->buffer,m_pHeadData.msgbuf.head.len);

		
		//收多了，将原包重新给m_sRcvBuffer
		for (int i=0; i<m_nRcvBufLen-sizeof(S_MSG_HEAD_FPC)-m_pHeadData.msgbuf.head.len; i++)
		{
			m_sRcvBuffer[i]=m_sRcvBuffer[i+sizeof(S_MSG_HEAD_FPC)+m_pHeadData.msgbuf.head.len];
		}
		m_nRcvBufLen-=sizeof(S_MSG_HEAD_FPC)+m_pHeadData.msgbuf.head.len;
		CallService();
	}
	return ;
}

void CBuLink::OnSend()
{
	int ret;
	char stmp[511];
	int length = m_pHeadData.msgbuf.head.len+sizeof(S_MSG_HEAD_FPC);
	memcpy(m_sSendBuffer,&(m_pHeadData.msgbuf.head),sizeof(S_MSG_HEAD_FPC));
	memcpy(m_sSendBuffer+sizeof(S_MSG_HEAD_FPC),m_pHeadData.msgbuf.message->buffer,m_pHeadData.msgbuf.head.len);
	S_MSG_HEAD_FPC *tmphead = (S_MSG_HEAD_FPC *)m_sSendBuffer;
	Reverse((unsigned char *)&(tmphead->msgtype),sizeof(tmphead->msgtype));
	Reverse((unsigned char *)&(tmphead->connecttime),sizeof(tmphead->connecttime));
	Reverse((unsigned char *)&(tmphead->index),sizeof(tmphead->index));
	Reverse((unsigned char *)&(tmphead->len),sizeof(tmphead->len));
	Reverse((unsigned char *)&(tmphead->buindex),sizeof(tmphead->buindex));
	Reverse((unsigned char *)&(tmphead->rtime),sizeof(tmphead->rtime));
	ret = m_tcpSocket.Send(m_sSendBuffer,length);
	m_pLog->LogBin(LOG_DEBUG-1,__FILE__,__LINE__,"应答给BFFPC的数据",m_pHeadData.msgbuf.message->buffer,m_pHeadData.msgbuf.head.len);				
	if (ret == length)
	{
		m_nSendtime = time(NULL);
	}
	else //停止
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"发送数据失败 m_sock=%d errmsg=%d %s",m_sock,m_tcpSocket.GetErrorNo(),m_tcpSocket.GetErrorMsg().c_str());

	}
}

void CBuLink::OnNoEvent()
{
	if (time(NULL) - m_nSendtime >= 5)
	{
		//5秒没有发送则发送MSG_FREE，类似心跳
		S_BF_RSMSG_FPC data;
		memset(&data,0,sizeof(S_BF_RSMSG_FPC));
		data.msgbuf.message = NULL;
		data.msgbuf.head.len = 0;
		data.msgbuf.head.rtime = GetPid();
		data.msgbuf.head.msgtype = MSG_FREE;
		if (OnSend(&data)<0)
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送MSG_FREE失败"); 
		}
	}
}

void CBuLink::OnEvent()
{
	int ret;
	int tmptime;
	ret = FD_ISSET(m_sock,&m_rset);
	if (ret != 0) //有读事件
	{
		OnRecv();
	}
	else //此连接无读事件
	{
		if (m_sock != INVALID_SOCKET)
		{
			tmptime = time(NULL) - m_nReadtime;
			if (tmptime > m_pRes.g_nDisconnectTime)
			{
				OnClose("连接长时间未用，断开");
			}
		}
	}// end if (ret == 1) //有读事件
}

void CBuLink::ResetData()
{
	memset(&m_pHeadData,0,sizeof(S_BF_RSMSG_FPC));
	m_pHeadData.msgbuf.message = (PCOMMSTRU_FPC)m_sReqAnsData;
}

void CBuLink::CallService()
{
	//处理请求
//	m_pBuProc.ProcessTx(m_sReqAnsData,m_nDataLen);
	
	BPCCOMMSTRU data;
	bzero(&data,sizeof(BPCCOMMSTRU));
	unsigned int dlen=BFBUFSIZE;
	if (m_pRes.g_IsZipEncrypt == 2)
	{
		if (!CBF_Tools::Uncompress((unsigned char *)data.sBuffer,dlen,(unsigned char *)m_sReqAnsData,m_nDataLen))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据解压缩出错 ");
			m_pHeadData.msgbuf.head.msgtype = MSG_FREE;
			m_pHeadData.msgbuf.head.len  = 0;
			OnSend();
			return;
		}
		data.sDBHead.nLen = dlen;
	}
	else
	{
		memcpy(data.sBuffer,m_sReqAnsData,m_nDataLen);
		data.sDBHead.nLen = m_nDataLen;
	}
	m_listAnsData.clear();


	CXmlPack xmlpack;
	if (xmlpack.fromBuf(data.sBuffer))
	{
		data.sDBHead.d_Dinfo.d_nServiceNo = xmlpack.getHeadValueI("交易码");
	}
	else
	{
		xmlpack.newPackage();
		if (!xmlpack.setBinData(data.sBuffer,data.sDBHead.nLen))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setBinData出错");
			return ;
		}
		int len = BFBUFSIZE;
		xmlpack.tobuffer(data.sBuffer,len);
		data.sDBHead.nLen = len;
		data.sDBHead.d_Dinfo.d_nServiceNo = atoi(m_pRes.g_sMainFunc.c_str());
	}
	m_pLog->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"请求数据",data.sBuffer,data.sDBHead.nLen);
	m_pBuProc.Run(&data);

	

	int datalen;
	
	if (m_listAnsData.size()>1)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"FPU应答有多记录，组成一条记录返回 %d",m_listAnsData.size());
		//若是交易统一入口，取bindata返回
		//多条记录，就只要把bindata组合到一起发给fpc
		char *tmpbuffer = (char *)malloc(BFBUFSIZE);
		if (tmpbuffer== NULL)
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"malloc出错 ");
			return;
		}
		bzero(tmpbuffer,BFBUFSIZE);
		int  tmpbufferlen=0;
		if (m_pRes.g_bIsTxMainFunc)
		{
			for (int k=0; k<m_listAnsData.size() ; k++)
			{
				if (xmlpack.fromBuf(m_listAnsData[k].sBuffer))
				{
					datalen = BPUDATASIZE;
					if (xmlpack.getBinData(m_listAnsData[k].sBuffer,datalen))
					{
						memcpy(tmpbuffer+tmpbufferlen,m_listAnsData[k].sBuffer,datalen);
						tmpbufferlen+=datalen;
					}

				}
			}
		}
		dlen = BFBUFSIZE;
		if (m_pRes.g_IsZipEncrypt == 2)
		{
			if (!CBF_Tools::Compress((unsigned char *)(m_pHeadData.msgbuf.message->buffer),dlen,(unsigned char *)(tmpbuffer),tmpbufferlen))
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据压缩出错 ");
				m_pHeadData.msgbuf.head.msgtype = MSG_FREE;
				m_pHeadData.msgbuf.head.len  = 0;
				OnSend();
				free(tmpbuffer);
				tmpbuffer= NULL;
				return;
			}
			m_pHeadData.msgbuf.head.len = dlen;
		}
		else 
		{
			if (tmpbufferlen <= BFBUFSIZE)
			{
				memcpy(m_pHeadData.msgbuf.message->buffer,tmpbuffer,tmpbufferlen);
				m_pHeadData.msgbuf.head.len = tmpbufferlen;
			}
			else
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"FPU应答数据太大 %d",tmpbufferlen);
				m_pLog->LogBin(LOG_ERROR_FAULT,__FILE__,__LINE__,"FPU应答数据太大",tmpbuffer,tmpbufferlen);
				m_pHeadData.msgbuf.head.msgtype = MSG_FREE;
				m_pHeadData.msgbuf.head.len  = 0;
				OnSend();
				free(tmpbuffer);
				tmpbuffer= NULL;
				return;
			}
		}
		free(tmpbuffer);
		tmpbuffer= NULL;
	}
	else if (m_listAnsData.size()<1)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"FPU无应答 %d",m_listAnsData.size());
		m_pHeadData.msgbuf.head.msgtype = MSG_FREE;
		m_pHeadData.msgbuf.head.len  = 0;
		OnSend();
		return;
	}
	else
	{
		//若是交易统一入口，取bindata返回
		if (m_pRes.g_bIsTxMainFunc)
		{
			if (xmlpack.fromBuf(m_listAnsData[0].sBuffer))
			{
				datalen = BPUDATASIZE;
				if (xmlpack.getBinData(m_listAnsData[0].sBuffer,datalen))
				{
					m_listAnsData[0].sDBHead.nLen = datalen;
				}
			}
			
		}
		dlen = BFBUFSIZE;
		if (m_pRes.g_IsZipEncrypt == 2)
		{
			if (!CBF_Tools::Compress((unsigned char *)(m_pHeadData.msgbuf.message->buffer),dlen,(unsigned char *)(m_listAnsData[0].sBuffer),m_listAnsData[0].sDBHead.nLen))
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"数据压缩出错 ");
				m_pHeadData.msgbuf.head.msgtype = MSG_FREE;
				m_pHeadData.msgbuf.head.len  = 0;
				OnSend();
				return;
			}
			m_pHeadData.msgbuf.head.len = dlen;
		}
		else 
		{
			if (m_listAnsData[0].sDBHead.nLen <= BFBUFSIZE)
			{
				memcpy(m_pHeadData.msgbuf.message->buffer,m_listAnsData[0].sBuffer,m_listAnsData[0].sDBHead.nLen);
				m_pHeadData.msgbuf.head.len = m_listAnsData[0].sDBHead.nLen;
			}
			else
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"FPU应答数据太大 %d",m_listAnsData[0].sDBHead.nLen);
				m_pLog->LogBin(LOG_ERROR_FAULT,__FILE__,__LINE__,"FPU应答数据太大",m_listAnsData[0].sBuffer,m_listAnsData[0].sDBHead.nLen);
				m_pHeadData.msgbuf.head.msgtype = MSG_FREE;
				m_pHeadData.msgbuf.head.len  = 0;
				OnSend();
				return;
			}
		}
	}
	m_pHeadData.msgbuf.head.msgtype = MSG_BUANSFREE;
	OnSend();
}

int CBuLink::OnSend(S_BF_RSMSG_FPC *data)
{
	int ret;
	char stmp[511];
	int length = data->msgbuf.head.len+sizeof(S_MSG_HEAD_FPC);
	memcpy(m_sSendBuffer,&(data->msgbuf.head),sizeof(S_MSG_HEAD_FPC));
	if (data->msgbuf.head.len>0)
	{
		memcpy(m_sSendBuffer+sizeof(S_MSG_HEAD_FPC),data->msgbuf.message->buffer,data->msgbuf.head.len);
	}
	
	S_MSG_HEAD_FPC *tmphead = (S_MSG_HEAD_FPC *)m_sSendBuffer;
	Reverse((unsigned char *)&(tmphead->msgtype),sizeof(tmphead->msgtype));
	Reverse((unsigned char *)&(tmphead->connecttime),sizeof(tmphead->connecttime));
	Reverse((unsigned char *)&(tmphead->index),sizeof(tmphead->index));
	Reverse((unsigned char *)&(tmphead->len),sizeof(tmphead->len));
	Reverse((unsigned char *)&(tmphead->buindex),sizeof(tmphead->buindex));
	Reverse((unsigned char *)&(tmphead->rtime),sizeof(tmphead->rtime));
	ret = m_tcpSocket.Send(m_sSendBuffer,length);
	if (data->msgbuf.head.len>0)
	{
		m_pLog->LogBin(LOG_DEBUG,__FILE__,__LINE__,"发送数据",data->msgbuf.message->buffer,data->msgbuf.head.len);	
	}
	else
	{
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"发送MSG_FREE");	
	}
	if (ret == length)
	{
		m_nSendtime = time(NULL);
		return 0;
	}
	else //停止
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"发送数据失败 m_sock=%d errmsg=%d %s",\
			m_sock,m_tcpSocket.GetErrorNo(),m_tcpSocket.GetErrorMsg().c_str());
	}
	return -1;
}
int CBuLink::OnRecv(S_BF_RSMSG_FPC *data)
{
	int ret;
	int txlen=0;
	char tmpchar[100];
	
	memset(tmpchar,0,sizeof(tmpchar));
	memset(sRcvBuffer,0,sizeof(sRcvBuffer));
	
	m_tcpSocket.AttachSocket(m_sock,"");
	if (nRcvBufLen >=0 && nRcvBufLen< BFBUFSIZE)
	{
		//包头未接收完
		ret = m_tcpSocket.Receive(sRcvBuffer+nRcvBufLen,BFBUFSIZE-nRcvBufLen);
		if (ret <= 0)
		{
			return ret;
		}
		nRcvBufLen+=ret;
		m_nReadtime =time(NULL);
		
	}
	
	//收到的数据大于包头
	if (nRcvBufLen >= sizeof(S_MSG_HEAD_FPC))
	{
		memcpy(&(data->msgbuf.head),sRcvBuffer,sizeof(S_MSG_HEAD_FPC));
		Reverse((unsigned char *)&(data->msgbuf.head.msgtype),sizeof(data->msgbuf.head.msgtype));
		Reverse((unsigned char *)&(data->msgbuf.head.connecttime),sizeof(data->msgbuf.head.connecttime));
		Reverse((unsigned char *)&(data->msgbuf.head.index),sizeof(data->msgbuf.head.index));
		Reverse((unsigned char *)&(data->msgbuf.head.len),sizeof(data->msgbuf.head.len));
		if (nRcvBufLen < sizeof(S_MSG_HEAD_FPC)+data->msgbuf.head.len)
		{
			if (nRcvBufLen >= BFBUFSIZE || data->msgbuf.head.len>=BFBUFSIZE )
			{
				OnClose("连接发送数据非法，非BFFPC连接");
				return -1;
			}
			//说明数据没有接收完整
			return 0;
		}
		//记录交易信息
//		m_pLog->LogSysBin(LOG_DEBUG+1,sRcvBuffer,data->msgbuf.head.len+sizeof(S_MSG_HEAD));
		if (data->msgbuf.message == NULL)
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"data->msgbuf.message为NULL");
			return -1;
		}
		if (data->msgbuf.head.len>0)
		{
			memcpy(data->msgbuf.message->buffer,sRcvBuffer+sizeof(S_MSG_HEAD_FPC),data->msgbuf.head.len);
			data->msgbuf.message->buffer[data->msgbuf.head.len]=0;
		}
		
		//记入接收日志
		if (m_pLog->isWrite(LOG_PROMPT))
		{
			m_pLog->LogBin(LOG_WARNNING,__FILE__,__LINE__,"接收到的BFFPC请求",data->msgbuf.message->buffer,data->msgbuf.head.len);
		}
		//收多了，将原包重新给m_sRcvBuffer
		for (int i=0; i<nRcvBufLen-sizeof(S_MSG_HEAD_FPC)-data->msgbuf.head.len; i++)
		{
			sRcvBuffer[i]=sRcvBuffer[i+sizeof(S_MSG_HEAD_FPC)+data->msgbuf.head.len];
		}
		nRcvBufLen-=sizeof(S_MSG_HEAD_FPC)+data->msgbuf.head.len;
		return sizeof(S_MSG_HEAD_FPC)+data->msgbuf.head.len;
	}
	return 0;
}

bool CBuLink::SetGPara1(bool setflag)
{
	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"无此功能");
	return false;
}
/// 函数名: isSetGPara2
/// 编程  : 王明松 2008-12-30 9:36:10
/// 返回  : virtual bool true成功false失败
/// 参数  : bool setflag=true
/// 描述  : 设置全局变量2,若已设置则返回false
bool CBuLink::SetGPara2(bool setflag)
{
	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"无此功能");
	return false;
}


/// 函数名: isSetGPara3
/// 编程  : 王明松 2008-12-30 9:36:13
/// 返回  : virtual bool true成功false失败
/// 参数  : bool setflag=true
/// 描述  : 设置全局变量3,若已设置则返回false
bool CBuLink::SetGPara3(bool setflag)
{
	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"无此功能");
	return false;
}



// 函数名: AnsData
// 编程  : 王明松 2013-4-23 15:16:43
// 返回  : virtual int -1失败 0成功
// 参数  : S_BPC_RSMSG *data
// 描述  : 应答数据,由BPU业务里面的原子调用
int CBuLink::AnsData(PBPCCOMMSTRU data)
{
	m_listAnsData.push_back(*data);
	return 0;
}

// 函数名: DPTrans
// 编程  : 王明松 2013-4-23 15:11:48
// 返回  : virtual int -1失败 0成功
// 参数  : PBPCCOMMSTRU calldata
// 描述  : 转移至其它服务处理
int CBuLink::DPTrans(PBPCCOMMSTRU calldata)
{
	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"无此功能");
	return -1;
}

// 函数名: BpuCall
// 编程  : 王明松 2013-4-23 15:09:13
// 返回  : virtual int -1失败 0成功
// 参数  : PBPCCOMMSTRU calldata
// 参数  : LIST_BPC_RSMSG &ansdata
// 描述  : 调用BPU内的功能，给流程控制使用
int CBuLink::BpuCall(PBPCCOMMSTRU calldata,LIST_BPC_RSMSG &ansdata)
{
	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"无此功能");
	return -1;
}

// 函数名: BpcCall
// 编程  : 王明松 2013-4-23 15:08:18
// 返回  : virtual int -1失败 0成功
// 参数  : PBPCCOMMSTRU calldata
// 参数  : LIST_BPC_RSMSG &ansdata
// 描述  : 调用bpc功能
int CBuLink::BpcCall(PBPCCOMMSTRU calldata,LIST_BPC_RSMSG &ansdata,unsigned int stimeout)
{
	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"无此功能");
	return -1;
}

// 函数名: ExtCall
// 编程  : 王明松 2013-4-23 15:01:25
// 返回  : virtual int -1失败 0成功
// 参数  : PBPCCOMMSTRU calldata
// 参数  : vector<S_BPC_RSMSG> &ansdata
// 描述  : 外调BPC
int CBuLink::ExtCall(PBPCCOMMSTRU calldata,LIST_BPC_RSMSG &ansdata,unsigned int stimeout)
{
	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"无此功能");
	return -1;
}


// 函数名:  InitLink
// 编程  : 王明松 2013-4-23 14:44:14
// 返回  : virtual bool true成功 false失败
// 参数  : const char *confile
// 描述  : 初始化连接配置 
bool  CBuLink::InitLink(const char *confile)
{
	if (!m_pRes.Init((char *)confile))
	{
		printf("初始化失败 %s\n",m_pRes.m_errMsg.c_str());
		return false;
	}
//	m_pLog = &(m_pRes.m_log);
	m_pBuProc.SetPara(this);
	if (!m_pBuProc.BuInit(confile))
	{
		return false;
	}
	m_pLog = m_pBuProc.GetLogClass();
	return true;

}
