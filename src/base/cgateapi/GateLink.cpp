#include "GateLink.h"



CGateLink::CGateLink()
{
	m_bIsRunThread = false;
	m_nIsConnect = 0;
}
CGateLink::~CGateLink()
{
	m_bIsRunThread = false;
}

// 函数名: Logout
// 编程  : 王明松 2013-5-23 15:20:54
// 返回  : void 
// 描述  : 关闭连接，退出线程
void CGateLink::Logout()
{
	m_bIsRunThread = false;
	//等待线程退出
	Join();
	m_pApiSocket.OnClose(__FILE__,__LINE__,"UI主动关闭连接，退出线程",LOG_PROMPT);

}


// 函数名: InitApi
// 编程  : 王明松 2013-5-23 14:49:22
// 返回  : bool true成功
// 参数  : const char *apixml xml配置文件
// 描述  : 初始化api，读取配置文件，得到证书目录及网关地址端口
bool CGateLink::InitApi(const char *apixml)
{
	//初始化配置
	if (!m_pRes.Init(apixml))
	{
		sprintf(m_sErrMsg,m_pRes.GetMsg().c_str());
		return false;
	}
	m_pApiSocket.SetPara(&m_pRes);
	return true;
}

	
// 函数名: ExitThreadInstance
// 编程  : 王明松 2013-5-27 10:01:19
// 返回  : virtual void 
// 描述  : 退出线程
void CGateLink::ExitThreadInstance()
{

}

// 函数名: InitThreadInstance
// 编程  : 王明松 2013-5-27 10:01:23
// 返回  : virtual bool 
// 描述  : 初始线程
bool CGateLink::InitThreadInstance()
{
	return true;
}


// 函数名: Run
// 编程  : 王明松 2013-5-23 15:26:51
// 返回  : virtual int 
// 描述  : 线程主方法
int CGateLink::Run()
{
	struct timeval tv;
	int result;
	int maxfd=0;
	int ret;
	m_pRes.g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,"启动 api select线程");
	while (m_bIsRunThread)
	{
		FD_ZERO(&m_rset);
		FD_ZERO(&m_wset);
#if defined(_WINDOWS)
		FD_ZERO(&m_eset);

		tv.tv_sec = 1;
		tv.tv_usec = 0;
#else
        tv.tv_sec = 0;
        tv.tv_usec = 50;
#endif
		SOCKET_HANDLE tmpscoket = m_pApiSocket.GetSocket(); 
		if ( m_bIsRunThread &&  tmpscoket != INVALID_SOCKET)
		{
			FD_SET(tmpscoket, &m_rset);//加入到读select集合
// 			if (m_pApiSocket.NeedSend()) //需要写时
// 			{
// 				FD_SET(m_pApiSocket.GetSocket(), &m_wset);//加入到写select集合
// 			}
		}
		else
		{
			m_pRes.g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,"连接断开,停止api select线程");
			m_bIsRunThread = false;
			m_nIsConnect = 0;
			return 0;
		}
		
		maxfd = tmpscoket;
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
		if (result == 0) //无事件发生,处理心跳
		{
			OnNoEvent();
		}
		else
		{
			OnEvent();
			OnNoEvent();
		}
		//SLEEP(1);
	}
	m_pRes.g_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"连接断开,停止api select线程");
	m_bIsRunThread = false;
	m_nIsConnect = 0;
	return 0;
}

//-1连接失败   0未连接   1连接成功   
int CGateLink::ConnectGate()
{
//	CBF_PMutex pp(&m_mutex);
	m_pApiSocket.OnClose(__FILE__,__LINE__,"客户端重连先关闭连接 清空接收队列，开始连接网关",LOG_PROMPT);
	m_nIsConnect = 2;
	if (!m_pApiSocket.ConnectGate())
	{
		m_nIsConnect =0;
		return -1;
	}
	m_nIsConnect = 1;
	//启动select线程
	if (IsStoped())
	{
		m_bIsRunThread = true;
		m_pRes.g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,"启动网关连接线程");
		CreateThread();
	}
	else
	{
		m_pRes.g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"网关连接线程已在运行中");
	}
	return 1;
}

void CGateLink::OnEvent()
{
	int ret;
	SOCKET_HANDLE tmpsocket= m_pApiSocket.GetSocket();
	ret = FD_ISSET(tmpsocket,&m_rset);
	if (ret != 0) //有读事件
	{
		OnRecv();
	}
	if (tmpsocket == INVALID_SOCKET)
	{
		m_bIsRunThread = false;
		return;
	}
#if defined(_WINDOWS)
	ret = FD_ISSET(tmpsocket , &m_eset);
	if (ret != 0)
	{
		//尝试连接到未成功
	}
	else if (FD_ISSET(tmpsocket, &m_wset) !=0 )
	{
		
//		m_pApiSocket.OnSend();//发送数据
		
	}
	else  //socket无写事件
	{
//		m_pApiSocket.OnSend();//发送数据
	}
#else
	
	if (FD_ISSET(tmpsocket, &m_wset) !=0)
	{
//		m_pApiSocket.OnSend();//发送数据
	}
	else  
	{
//		m_pApiSocket.OnSend();//发送数据
	}
#endif

}

void CGateLink::OnNoEvent()
{
	int tmptime = time(NULL) ;//  - m_pApiSocket.m_nSendTime;
	int tmpwtime = tmptime -  m_pApiSocket.m_nSendTime;
	tmptime = tmptime - m_pApiSocket.m_nRcvTime;
	if ( tmptime > m_pRes.g_nHeartTime && tmptime < m_pRes.g_nCloseTime)
	{
		OnPing();
	}
	else if ( tmpwtime > m_pRes.g_nHeartTime && tmptime < m_pRes.g_nCloseTime)
	{
		OnPing();
	}
	else if (tmptime > m_pRes.g_nCloseTime)
	{
		m_pApiSocket.OnClose(__FILE__,__LINE__,"连接长时间未使用，断开",LOG_WARNNING);
	}

}
void CGateLink::OnPing()
{
	CGATE_COMMSTRU smsg;
	smsg.head.nLen = 0;
	smsg.head.nRetCode = 0;
	smsg.head.stComm.cCmd = 0;
	smsg.head.stComm.cMsgType = MSG_GATEPING;
	smsg.head.stComm.cNextFlag = 0;
	smsg.head.stComm.cRaflag = 0;
	smsg.head.stComm.cZip = 0;
	smsg.head.stComm.nCrc = 0;
	smsg.head.stDest.cNodePrivateId = 0;
	smsg.head.stDest.cSvrPrivateId = 0;
	smsg.head.stDest.nNodeId = 0;
	smsg.head.stDest.nSerial = 0;
	smsg.head.stDest.nServiceNo = 0;
	smsg.head.stDest.nSvrMainId = 0;
	
	//-1 未连接网关 =0流量控制或缓冲满 <0发送出错  1发送部分数据 2发送完成
	int nRet = m_pApiSocket.Send2Gate(&smsg);
	if (nRet == 2 )
	{
		m_pRes.g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,"发送PING数据完成");
		return ;
	}
	else
	{
		m_pRes.g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"发送PING数据失败");
		return ;
	}
	return ;
}
void CGateLink::OnRecv()
{
	int ret=0;
	int i=0;
	
//	m_pRes.g_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"准备接收数据");

	ret = m_pApiSocket.OnRcv();	
	if (ret<=0)
	{
		m_pApiSocket.OnClose(__FILE__,__LINE__,"远端关闭，接收数据不完整",LOG_WARNNING);
		return;
	}
//	m_pRes.g_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"准备接收数据 长度[%d]  已有数据长度[%d]",ret,m_pApiSocket.GetRcvDataLen());
	while (m_pApiSocket.GetRcvDataLen() >= CGATEHEADLEN )
	{
		CGATE_COMMSTRU data;
		ret = m_pApiSocket.GetRecvData(&data);
		if (ret >0 )
		{
			//接收CGATE过来的数据
			if (data.head.stComm.cMsgType == MSG_GATEPING) //响应PING应答
			{
//				m_pRes.g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"收到网关发送的PING消息，响应PING消息");
				data.head.stComm.cRaflag = 1;
				m_pApiSocket.Send2Gate(&data);
				continue; ;
			}
			ProcessMsg(data);
//			m_pRes.g_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"准备接收数据 缓冲仍有数据长度[%d]",m_pApiSocket.GetRcvDataLen());
		}
		else
		{
			break;
		}
	}
	
	return ;
}

void CGateLink::ProcessMsg(CGATE_COMMSTRU &data)
{
	if (data.head.stComm.cMsgType == MSG_PUSH) //推送消息
	{
		m_pApiSocket.m_pReqQueue.PushData(data);
	}
	else if (data.head.stComm.cMsgType == MSG_GATEREQ || data.head.stComm.cMsgType == MSG_GATESUBSCRIBE) //请求
	{
		m_pApiSocket.m_pAnsQueue.PushData(data);
	}
	else
	{
		m_pRes.g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"网关消息类型不符[%s] %d",\
			m_pApiSocket.GetMsgType(data.head.stComm.cMsgType).c_str(),data.head.stDest.nServiceNo);
		m_pRes.g_pLog.LogBin(LOG_ERROR,__FILE__,__LINE__,"网关消息类型不符",data.buffer,data.head.nLen);
	}
}
//返回 0未连接  1已连接   2正在连接
//前端调用此方法时，如果连接状态为2时，需循环sleep并检查是否连接好，如果直接调用connectgate可能会造成连接出错
int CGateLink::GetConnectStatus()
{
	return m_nIsConnect;
}

// 函数名: GetReqData
// 编程  : 王明松 2013-5-23 15:12:42
// 返回  : int -1连接关闭 =0超时无请求  >0成功取到请求
// 参数  : PCGATE_COMMSTRU data   请求数据
// 参数  : int timeout 超时时间秒
// 描述  : 取推送或广播请求
int CGateLink::GetReqData(PCGATE_COMMSTRU data,int timeout)
{
	if (m_pApiSocket.m_pReqQueue.GetData(*data,timeout*1000))
	{
        m_pRes.g_pLog.LogBin(LOG_DEBUG, __FILE__, __LINE__, "推送或广播数据",data->buffer,data->head.nLen);
		return m_pApiSocket.m_pReqQueue.GetSize()+1;
	}
	//队列没有数据了，判断连接状态
	if (!GetConnectStatus())
	{
		sprintf(m_sErrMsg,"网关未连接 广播推送队列数[%d]",m_pApiSocket.m_pReqQueue.GetSize());
		m_pRes.g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_sErrMsg);
		return -1;
	}
	return 0;
}

// 函数名: GetAnsData
// 编程  : 王明松 2013-5-23 15:14:47
// 返回  : int =0超时出错 >0取到应答，剩余数为此值减1  -1连接出错
// 参数  : PCGATE_COMMSTRU data   请求数据  data->head.stDest.nSerial为指定序号的应答
// 参数  : int timeout  超时时间秒 
// 描述  : 取得指定流水的应答
int CGateLink::GetAnsData(PCGATE_COMMSTRU data,int timeout)
{
	unsigned int serial = data->head.stDest.nSerial;
	int btime = time(NULL);
	bool bRet= true;
	m_pRes.g_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"等待取应答  index[%d] 标识[%d] 超时[%d]秒 time[%d]  应答队列数[%d]",\
		m_pApiSocket.m_nIndex,serial,timeout,btime ,m_pApiSocket.m_pAnsQueue.GetSize());
	while (bRet)
	{
		bRet = m_pApiSocket.m_pAnsQueue.GetData(*data,1000);
		if (bRet)
		{
			if (serial != 0) //接收指定序号的应答
			{
				if (data->head.stDest.nSerial != serial)
				{
					if ((time(NULL) -btime) > timeout)
					{
						break;
					}
					m_pRes.g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"请求应答不匹配  收到index[%d] 标识[%d]的应答,和标识[%d]的请求不匹配",\
						m_pApiSocket.m_nIndex,data->head.stDest.nSerial,serial);
					m_pRes.g_pLog.LogBin(LOG_WARNNING,__FILE__,__LINE__,"不匹配的数据",data->buffer,data->head.nLen);
					continue;
				}
			}
			
			m_pRes.g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,"收到应答  接收到index[%d] 标识[%d]的应答 用时[%d]秒",\
				m_pApiSocket.m_nIndex,m_nSerial,time(NULL)-btime);
			return m_pApiSocket.m_pAnsQueue.GetSize()+1;
		}
		else
		{
			if ((time(NULL) -btime) > timeout)
			{
				//超时退出
				break;
			} 
			else
			{
				//队列没有数据了，判断连接状态
				if (m_pApiSocket.m_nSocketStatus != 1)
				{
					sprintf(m_sErrMsg,"网关未连接 广播推送队列数[%d]",m_pApiSocket.m_pReqQueue.GetSize());
					m_pRes.g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_sErrMsg);
					return -1;
				}
				bRet = true;
				SLEEP(1);
			}
		}
		
	}
	int etime = time(NULL);
	m_pRes.g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"错误 未收到应答 index[%d] 标识[%d] 交易码[%d] 超时[%d]秒 time[%d] 用时[%d]秒",\
		m_pApiSocket.m_nIndex,serial,data->head.stDest.nServiceNo,timeout,etime,etime-btime);
	return 0;
}

// 函数名: SendReqData
// 编程  : 王明松 2013-5-23 15:12:27
// 返回  : int -1未连接  -2发送出错  =0 超时未收到应答 1收到应答
// 参数  : PCGATE_COMMSTRU data   请求应答数据
// 参数  : unsigned int timeout    接收应答超时时间  <=0直接返回，不接收应答
// 描述  : 发送请求并接收应答
int CGateLink::SendReqData(PCGATE_COMMSTRU data,unsigned int timeout)
{
	if (GetConnectStatus() != 1)
	{
		sprintf(m_sErrMsg,"SendReqData 网关未连接或正在连接，发送数据 index[%d] 标识[%d] 交易码[%d]",\
			m_pApiSocket.m_nIndex,data->head.stDest.nSerial,data->head.stDest.nServiceNo);
		m_pRes.g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return -1;
	}
//	CBF_PMutex pp(&m_mutex);
	if (data->head.stComm.cNextFlag == 0)
	{
		data->head.nRetCode = 0;
		data->head.stComm.cMsgType = MSG_GATEREQ;
		data->head.stComm.cRaflag = 0;
		data->head.stComm.nCrc = 0;
		m_nSerial = data->head.stDest.nSerial;
		int nRet = m_pApiSocket.Send2Gate(data);
		if (nRet != 2 )
		{
			sprintf(m_sErrMsg,"SendReqData Send2Gate出错 消息类型[MSG_GATEREQ]  index[%d] 标识[%d]",\
				m_pApiSocket.m_nIndex,data->head.stDest.nSerial);
			m_pRes.g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return -1;
		}
		else
		{
			if (data->head.stComm.cCmd == CMD_DPABC || data->head.stComm.cCmd == CMD_DPPUSH )
			{
				//这两个无须应答，直接返回0吧
				return 0;
			}
			if (timeout <=0)
			{
				return 0;
			}
			return GetAnsData(data,timeout);
		}
	}
	else
	{
		if (data->head.stComm.cNextFlag == 1 || data->head.stComm.cNextFlag == 10) //不停地收
		{
			return GetAnsData(data,timeout);
		}
		else //主动发送取后续包
		{
			data->head.nRetCode = 0;
			data->head.stComm.cMsgType = MSG_GATEREQ;
			data->head.stComm.cRaflag = 0;
			data->head.stComm.nCrc = 0;
			data->head.nLen = 0;
			int nRet = m_pApiSocket.Send2Gate(data);
			if (nRet != 2 )
			{
				sprintf(m_sErrMsg,"发送取后续包出错 index[%d] 标识[%d]",m_pApiSocket.m_nIndex,data->head.stDest.nSerial);				
				m_pRes.g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
				return -2;
			}
			else
			{
				return GetAnsData(data,timeout);
			}
		}
	}
}


// 函数名: SendLogin
// 编程  : 王明松 2013-5-23 15:12:27
// 返回  : int -1未连接  -2发送出错  =0 超时未收到应答 1收到应答
// 参数  : PCGATE_COMMSTRU data   请求应答数据
// 参数  : unsigned int timeout    接收应答超时时间 <=0直接返回0，不接收应答
// 描述  : 发送请求并接收应答
int CGateLink::SendLogin(PCGATE_COMMSTRU data,unsigned int timeout)
{
	if (GetConnectStatus() != 1)
	{
		sprintf(m_sErrMsg,"SendLogin 网关未连接或正在连接，发送数据 index[%d] 标识[%d] 交易码[%d]",\
			m_pApiSocket.m_nIndex,data->head.stDest.nSerial,data->head.stDest.nServiceNo);
		m_pRes.g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return -1;
	}
//	CBF_PMutex pp(&m_mutex);
	if (data->head.stComm.cNextFlag == 0)
	{
		data->head.nRetCode = 0;
		data->head.stComm.cMsgType = MSG_GATELOGIN;
		data->head.stComm.cRaflag = 0;
		data->head.stComm.nCrc = 0;
		m_nSerial = data->head.stDest.nSerial;
		int nRet = m_pApiSocket.Send2Gate(data);
		if (nRet != 2 )
		{
			sprintf(m_sErrMsg,"SendLogin Send2Gate出错 消息类型[MSG_GATEREQ]  index[%d] 标识[%d]",\
				m_pApiSocket.m_nIndex,data->head.stDest.nSerial);
				m_pRes.g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return -1;
		}
		else
		{
			if (timeout<=0)
			{
				return 0;
			}
			return GetAnsData(data,timeout);
		}
	}
	else
	{
		if (data->head.stComm.cNextFlag == 1 || data->head.stComm.cNextFlag == 10) //不停地收
		{
			return GetAnsData(data,timeout);
		}
		else //主动发送取后续包
		{
			data->head.nRetCode = 0;
			data->head.stComm.cMsgType = MSG_GATEREQ;
			data->head.stComm.cRaflag = 0;
			data->head.stComm.nCrc = 0;
			data->head.nLen = 0;
			int nRet = m_pApiSocket.Send2Gate(data);
			if (nRet != 2 )
			{
				sprintf(m_sErrMsg,"SendLogin 发送取后续包出错 index[%d] 标识[%d]",m_pApiSocket.m_nIndex,data->head.stDest.nSerial);				
				m_pRes.g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
				return -2;
			}
			else
			{
				return GetAnsData(data,timeout);
			}
		}
	}
}

//int CGateLink::Subscribe(int flag, std::vector<int>& keylist)
//{
//	CGATE_COMMSTRU data;
//	bzero(&(data.head),CGATEHEADLEN);
//    S_GATE_SUBSCRIBE* subscribe = (S_GATE_SUBSCRIBE*)(data.buffer);
//    subscribe->flag = flag;
//    subscribe->datanum = keylist.size();
//	if (subscribe->datanum > 0)
//	{
//        subscribe->variety = keylist[0];
//        for (int i = 1; i < subscribe->datanum; i++)
//        {
//            int* key1 = (int*)(data.buffer + sizeof(S_GATE_SUBSCRIBE) + (i - 1) * 4);
//            *key1 = keylist[i];;
//        }
//        data.head.nLen = sizeof(S_GATE_SUBSCRIBE) + (subscribe->datanum - 1) * 4;
//	}
//	else
//	{
//		data.head.nLen = sizeof(S_GATE_SUBSCRIBE);
//	}
//    
//	return Subscribe(&data,0);
//}
// 函数名: SendLogin
// 编程  : 王明松 2013-5-23 15:12:27
// 返回  : int -1未连接  -2发送出错  =0 超时未收到应答 1收到应答
// 参数  : PCGATE_COMMSTRU data   请求应答数据
// 参数  : unsigned int timeout    接收应答超时时间 <=0直接返回0，不接收应答
// 描述  : 发送订阅请求并接收应答
int CGateLink::Subscribe(PCGATE_COMMSTRU data,unsigned int timeout)
{
	if (GetConnectStatus() != 1)
	{
		sprintf(m_sErrMsg,"SendLogin 网关未连接或正在连接，发送数据 index[%d] 标识[%d] 交易码[%d]",\
			m_pApiSocket.m_nIndex,data->head.stDest.nSerial,data->head.stDest.nServiceNo);
		m_pRes.g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return -1;
	}
	//	CBF_PMutex pp(&m_mutex);
	if (data->head.stComm.cNextFlag == 0)
	{
		data->head.nRetCode = 0;
		data->head.stComm.cMsgType = MSG_GATESUBSCRIBE;
		data->head.stComm.cRaflag = 0;
		data->head.stComm.nCrc = 0;
		m_nSerial = data->head.stDest.nSerial;
		int nRet = m_pApiSocket.Send2Gate(data);
		if (nRet != 2 )
		{
			sprintf(m_sErrMsg,"SendLogin Send2Gate出错 消息类型[MSG_GATEREQ]  index[%d] 标识[%d]",\
				m_pApiSocket.m_nIndex,data->head.stDest.nSerial);
			m_pRes.g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return -1;
		}
		else
		{
			if (timeout<=0)
			{
				return 0;
			}
			return GetAnsData(data,timeout);
		}
	}
	else
	{
		if (data->head.stComm.cNextFlag == 1 || data->head.stComm.cNextFlag == 10) //不停地收
		{
			return GetAnsData(data,timeout);
		}
		else //主动发送取后续包
		{
			data->head.nRetCode = 0;
			data->head.stComm.cMsgType = MSG_GATESUBSCRIBE;
			data->head.stComm.cRaflag = 0;
			data->head.stComm.nCrc = 0;
			data->head.nLen = 0;
			int nRet = m_pApiSocket.Send2Gate(data);
			if (nRet != 2 )
			{
				sprintf(m_sErrMsg,"SendLogin 发送取后续包出错 index[%d] 标识[%d]",m_pApiSocket.m_nIndex,data->head.stDest.nSerial);				
				m_pRes.g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
				return -2;
			}
			else
			{
				return GetAnsData(data,timeout);
			}
		}
	}
}
// 函数名: SendSms
// 编程  : 王明松 2013-5-23 15:12:27
// 返回  : int -1未连接  -2发送出错  =0 超时未收到应答 1收到应答
// 参数  : PCGATE_COMMSTRU data   请求应答数据
// 参数  : unsigned int timeout    接收应答超时时间 <=0直接返回0，不接收应答
// 描述  : 发送请求并接收应答
int CGateLink::SendSms(PCGATE_COMMSTRU data,unsigned int timeout)
{
	if (GetConnectStatus() != 1)
	{
		sprintf(m_sErrMsg,"SendLogin 网关未连接或正在连接，发送数据 index[%d] 标识[%d] 交易码[%d]",\
			m_pApiSocket.m_nIndex,data->head.stDest.nSerial,data->head.stDest.nServiceNo);
		m_pRes.g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return -1;
	}
//	CBF_PMutex pp(&m_mutex);
	if (data->head.stComm.cNextFlag == 0)
	{
		data->head.nRetCode = 0;
		data->head.stComm.cMsgType = MSG_GATESMS;
		data->head.stComm.cRaflag = 0;
		data->head.stComm.nCrc = 0;
		int nRet = m_pApiSocket.Send2Gate(data);
		if (nRet != 2 )
		{
			sprintf(m_sErrMsg,"SendSms Send2Gate出错 消息类型[MSG_GATEREQ]  index[%d] 标识[%d]",\
				m_pApiSocket.m_nIndex,data->head.stDest.nSerial);
				m_pRes.g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return -1;
		}
		else
		{
			if (timeout<=0)
			{
				return 0;
			}
			return GetAnsData(data,timeout);
		}
	}
	else
	{
		if (data->head.stComm.cNextFlag == 1 || data->head.stComm.cNextFlag == 10) //不停地收
		{
			return GetAnsData(data,timeout);
		}
		else //主动发送取后续包
		{
			data->head.nRetCode = 0;
			data->head.stComm.cMsgType = MSG_GATEREQ;
			data->head.stComm.cRaflag = 0;
			data->head.stComm.nCrc = 0;
			data->head.nLen = 0;
			int nRet = m_pApiSocket.Send2Gate(data);
			if (nRet != 2 )
			{
				sprintf(m_sErrMsg,"SendSms 发送取后续包出错 index[%d] 标识[%d]",m_pApiSocket.m_nIndex,data->head.stDest.nSerial);				
				m_pRes.g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
				return -2;
			}
			else
			{
				return GetAnsData(data,timeout);
			}
		}
	}
}

void CGateLink::SetDatLogName(const char *name)
{
	
}
