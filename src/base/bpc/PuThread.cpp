// PuThread.cpp: implementation of the CPuThread class.
//
//////////////////////////////////////////////////////////////////////

#include "PuThread.h"
#include "dreberrcode.h"
#include "Bpc_Timer.h"

extern CBpc_Timer  g_pBpcTime;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern char g_VERSION[20];
CPuThread::CPuThread()
{
	m_bIsRunning = false;
	m_pRes = NULL;
	m_pSockMgr = NULL;
	m_pMemPool = NULL;
	m_pPoolData = NULL;
	m_pFuncTbl = NULL;
	m_bIsInit = false;
	m_nBegin=0;
	m_nEnd=1;
	m_sThreadName= "CPuThread";

	m_pLog = NULL;
	m_bIsPingTimer = false;
}

CPuThread::~CPuThread()
{
	m_pRes = NULL;
	m_pSockMgr = NULL;
	m_pMemPool = NULL;
	m_pPoolData = NULL;
	m_pFuncTbl = NULL;
	m_pLog = NULL;
}
std::string CPuThread::GetBpcMsgType(int msgtype)
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
	case MSG_TRANS:      //9      //交易转移
		return "MSG_GETNEXT";
	default:
		return "";
	}
}
std::string CPuThread::GetDrebCmdType(int cmdtype)
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


bool CPuThread::SetGlobalVar(CGResource *res,CPoolDataMsg *pooldata,CBF_BufferPool *mempool,CSocketMgr *sockmgr,CFuncTbl *tbl)
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
	m_pTimer.SetTimer(0,5000,&CPuThread::OnTimer,this); //设置定时器 
	return true;
}

void CPuThread::ExitThreadInstance()
{
	return ;
}
bool CPuThread::InitThreadInstance()
{
	return true;
}

int CPuThread::Run()
{
	struct timeval tv;
	int result;
	int maxfd=0;
	int ret;
	int lasttime=time(NULL);
	int lasttime2=time(NULL);
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"%s SOCKET index:%d-%d范围",m_sThreadName.c_str(),m_nBegin,m_nEnd);
	while(!m_pRes->g_bToExit)
	{
		FD_ZERO(&m_rset);
		FD_ZERO(&m_wset);
#if defined(_WINDOWS)
		FD_ZERO(&m_eset);
#endif
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		maxfd=0;
		//取得最大的socket并将socket加入到fd_set集合
		ret = GetMaxfdAddSelect(maxfd);
		if (maxfd  <=0 || ret<1)
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
			m_bIsPingTimer= false;
		}

	}
	m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"停止BPU组线程");
	return 0;
}
int CPuThread::GetMaxfdAddSelect(int &maxfd)
{
	int ret=0;
	maxfd = 0;
	for (int i=m_nBegin;i<m_nEnd;i++)
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
			if (m_pSockMgr->at(i)->NeedSend())
			{
				FD_SET(m_pSockMgr->at(i)->m_sock, &m_wset);//加入到写select集合
			}
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
void CPuThread::OnNoEvent()
{
	int j;
	int tmptime;
	int curtime =  time(NULL) ;
	for (j=m_nBegin; j<m_nEnd; j++)
	{
		if (m_pSockMgr->at(j)->m_sock != INVALID_SOCKET && m_pSockMgr->at(j)->m_nType == BPCSOCK_BPU)
		{
			tmptime = curtime - m_pSockMgr->at(j)->m_nReadtime;
			if (m_pSockMgr->at(j)->IsBpuFree()) //表明此连接的BPU是空闲的
			{
				//若是空闲，且
				if (tmptime > 15)  //15秒无心跳过来，说明连接出问题了
				{
					OnClose(j,"连接空闲但超过15秒无心跳，断开",__FILE__,__LINE__);
				}
			}
			else  //连接在处理请求，防止一直在这状态，导致不可用。
			{
				if (tmptime > m_pRes->g_nDisconnectTime)
				{
					OnClose(j,"连接正在处理请求，但处理时间超过配置的断开时间，断开",__FILE__,__LINE__);
				}
			}
		}
		if (m_pRes->g_bToExit)
		{
			return;
		}
	}
	
}

void CPuThread::OnEvent()
{
	int ret;
	int i;
	for (i= m_nBegin; i<m_nEnd; i++)
	{
		if (m_pSockMgr->at(i)->m_sock != INVALID_SOCKET)//未连接
		{
			//响应读事件
			OnReadEvent(i);
			if (m_pSockMgr->at(i)->m_sock != INVALID_SOCKET)
			{
				//响应写事件
				OnWriteEvent(i);
			}
			
		}
	}
}

void CPuThread::OnReadEvent(int conindex)
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
				if (m_pSockMgr->at(conindex)->m_nType == BPCSOCK_UNKNOW)
				{
					if (time(NULL)-m_pSockMgr->at(conindex)->m_nReadtime >= m_pRes->g_nHeartRun)
					{
						OnClose(conindex,"连接非法",__FILE__,__LINE__);
					} 
				}
			}
		}// end if (m_pSockMgr->at(conindex)->m_sock != INVALID_SOCKET)
	}// end if (ret == 1) //有读事件
}

void CPuThread::OnWriteEvent(int conindex)
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
		OnClose(conindex,"Other connect fail",__FILE__,__LINE__);
	}
	else if (FD_ISSET(m_pSockMgr->at(conindex)->m_sock, &m_wset) !=0 )
	{
		OnSend(conindex);//发送数据
	}

#else
	
	int ok;
	int oklen;
	if (FD_ISSET(m_pSockMgr->at(conindex)->m_sock, &m_wset) !=0)
	{
		OnSend(conindex);//发送数据
	}
#endif

				
}


void CPuThread::OnClose(int conindex,const char *msg,const char *filename,int fileline)
{
	if (m_pSockMgr->at(conindex)->m_sock == INVALID_SOCKET)
	{
		return;
	}
	//将socket关闭
	m_pSockMgr->at(conindex)->OnClose(msg,filename,fileline);
}


void CPuThread::OnSend(int conindex)
{
	m_pSockMgr->at(conindex)->OnSend();
}


void CPuThread::OnRecv(int conindex)
{
	int ret=0;
	int i=0;
	
	ret = m_pSockMgr->at(conindex)->OnRecv();	
	if (ret<=0)
	{
		OnClose(conindex,"远端关闭，接收数据不完整",__FILE__,__LINE__);
		return;
	}
	if (ret == 0)
	{
		return ;	
	}
//	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"从BPU连接%d OnRecv 到数据%d",conindex,ret);
	while(m_pSockMgr->at(conindex)->m_nRcvBufLen>0)
	{
		if (!ResetData(conindex,&m_pDataRcv))
		{
			return;
		}
		ret = m_pSockMgr->at(conindex)->GetRecvData(&m_pDataRcv);
		if (ret>0)
		{
			
			m_pDataRcv.index = conindex;//保存接收到的数据的连接的索引
			if (m_pSockMgr->at(conindex)->m_nType == BPCSOCK_BPU) //接收BPU的数据
			{
// 				if (m_pDataRcv.sMsgBuf->sDBHead.nLen>0)
// 				{
// 					m_pLog->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"业务数据",m_pDataRcv.sMsgBuf->sBuffer,m_pDataRcv.sMsgBuf->sDBHead.nLen);
// 				}
				ProcessBpu(m_pDataRcv);
			}	
			else
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"错误 收错数据了");
			}

		}
		else
		{
			m_pMemPool->PoolFree(m_pDataRcv.sMsgBuf);
			m_pDataRcv.sMsgBuf = NULL;
			break;
		}
	}
// 	if (m_pSockMgr->at(conindex)->m_nRcvBufLen >0 )
// 	{	
// 		m_pLog->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"缓冲区数据 未取走",\
// 			m_pSockMgr->at(conindex)->m_sRcvBuffer,m_pSockMgr->at(conindex)->m_nRcvBufLen);
// 	}
	return ;
}


bool CPuThread::ResetData(int index,S_BPC_RSMSG *data)
{
	data->sMsgBuf = (PBPCCOMMSTRU )m_pMemPool->PoolMalloc();
	if (data->sMsgBuf == NULL)
	{
		m_pSockMgr->at(index)->OnClose("分配消息空间出错",__FILE__,__LINE__);
		return false;
	}
	data->sMsgBuf->sBpcHead.nConnectTime= m_pSockMgr->at(index)->m_nConntime;
	data->sMsgBuf->sBpcHead.nIndex = index;
	data->sMsgBuf->sBpcHead.nRtime = time(NULL);
	
	return true;
}


void CPuThread::ProcessBpu(S_BPC_RSMSG &rcvdata)
{
	switch (rcvdata.sMsgBuf->sBpcHead.cMsgType)
	{
		case MSG_FREE:   //bpu通知bpc，我是空闲,相当于心跳
			DispatchBuFree(rcvdata);
			break;
		case MSG_GETSERIAL://BPU取流水
			GetSerial(rcvdata);
			break;
		case MSG_SETGPARA1://BPU置参数1
			DispatchSetPara1(rcvdata);
			break;
		case MSG_SETGPARA2://BPU置参数2
			DispatchSetPara2(rcvdata);
			break;
		case MSG_SETGPARA3://BPU置参数3
			DispatchSetPara3(rcvdata);
			break;
		case MSG_EXTCALL://bpu外调，这里仅是请求，应答的数据由msgthread直接返回
			DispatchExtCall(rcvdata);
			break;
		case MSG_BPCCALL: //bpu调用bpc内的功能，请求在这里处理
			DispatchBpcCall(rcvdata);
			break;	
		case MSG_BPUREG://bpu注册
			//修改为消息处理线程来处理注册
			m_pPoolData->PushData(rcvdata);
			break;
		case MSG_BPUISREG://bpu发送是否可注册
			DispatchBpuIsReg(rcvdata);
			break;
		case MSG_REQ:   //这里收到的是应答
			DispatchReq(rcvdata);
			break;
		case MSG_TRANS: //这里收到的是请求
			DispatchTrans(rcvdata);
			break;
		case MSG_GETNEXT://这里收到的是应答
			DispatchGetNext(rcvdata);
			break;
		default:
			m_pLog->LogBin(LOG_ERROR_FAULT,__FILE__,__LINE__,"严重错误，错误数据",rcvdata.sMsgBuf->sBuffer,rcvdata.sMsgBuf->sDBHead.nLen);
			m_pMemPool->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			OnClose(rcvdata.index,"严重错误 接收数据非法，关闭连接",__FILE__,__LINE__);
			break;
	}
	return ;
}

void CPuThread::DispatchBuFree(S_BPC_RSMSG &rcvdata)
{
	if (m_pSockMgr->AffirmIndex(rcvdata.index))
	{
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->SetBpuFreeFlag(true);
//		m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"BPU连接[%d] ",rcvdata.sMsgBuf->sBpcHead.nBpuIndex);
//		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"BPU index[%d] 状态为空闲",rcvdata.sMsgBuf->sBpcHead.nBpuIndex);
	}
	if (rcvdata.sMsgBuf->sDBHead.cRaflag == 1)
	{
		//bpu的PING应答
		m_pMemPool->PoolFree(rcvdata.sMsgBuf);
	 	rcvdata.sMsgBuf = NULL;
		return;
	}
	if (m_pRes->g_vBpuGroupInfo[m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->m_nBpuGroupIndex].g_bIsAnsPing)
	{
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		rcvdata.sMsgBuf->sDBHead.nLen = 0;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
		rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN ;
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"应答MSG_FREE");
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->SendMsg(rcvdata);
	}
	else
	{
		m_pMemPool->PoolFree(rcvdata.sMsgBuf);
	 	rcvdata.sMsgBuf = NULL;
	}
	
}

void CPuThread::GetSerial(S_BPC_RSMSG &rcvdata)
{
	rcvdata.sMsgBuf->sDBHead.cZip = 0;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
	sprintf(rcvdata.sMsgBuf->sBuffer,"%ld",m_pRes->GetThreadShareSerial());
	rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
	rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN + rcvdata.sMsgBuf->sDBHead.nLen;
	m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->SendMsg(rcvdata);
}

void CPuThread::DispatchSetPara1(S_BPC_RSMSG &rcvdata)
{
	rcvdata.sMsgBuf->sDBHead.cZip = 0;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
	bool bRes = m_pRes->SetGPara1();
	sprintf(rcvdata.sMsgBuf->sBuffer,"%d",bRes);
	rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
	rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN + rcvdata.sMsgBuf->sDBHead.nLen;
	m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->SendMsg(rcvdata);
}

void CPuThread::DispatchSetPara2(S_BPC_RSMSG &rcvdata)
{
	rcvdata.sMsgBuf->sDBHead.cZip = 0;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
	bool bRes = m_pRes->SetGPara2();
	sprintf(rcvdata.sMsgBuf->sBuffer,"%d",bRes);
	rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
	rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN + rcvdata.sMsgBuf->sDBHead.nLen;
	m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->SendMsg(rcvdata);
}

void CPuThread::DispatchSetPara3(S_BPC_RSMSG &rcvdata)
{
	rcvdata.sMsgBuf->sDBHead.cZip = 0;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
	bool bRes = m_pRes->SetGPara3();
	sprintf(rcvdata.sMsgBuf->sBuffer,"%d",bRes);
	rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
	rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN + rcvdata.sMsgBuf->sDBHead.nLen;
	rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
	m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->SendMsg(rcvdata);
	
}

void CPuThread::DispatchExtCall(S_BPC_RSMSG &rcvdata)
{
	//songfree 20130409 外调就是外调，不要和内调混在一起，否则推送什么的没法区分
	rcvdata.nRtime = time(NULL);
	//外调优先级为0
	m_pPoolData->PushData(rcvdata);//放入队列，msgthread线程进行处理
	
}

void CPuThread::DispatchBpcCall(S_BPC_RSMSG &rcvdata)
{
	//即调用BPC里的交易
	if (rcvdata.sMsgBuf->sDBHead.cRaflag == 0) //请求
	{
		S_FUNCINFO_TBL fn;
		int flag;
		//取交易的优先级,然后放入队列
		fn = m_pFuncTbl->FindFunc(rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,flag);
		if (flag == 0)
		{
			//m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"无此功能 %d",rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo);
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"错误 无此功能 消息[%s]  DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 源[%d %d] cZip[%d] 业务数据长度[%d]",\
				GetBpcMsgType(rcvdata.sMsgBuf->sBpcHead.cMsgType).c_str(),\
				GetDrebCmdType(rcvdata.sMsgBuf->sDBHead.cCmd).c_str(),rcvdata.sMsgBuf->sDBHead.cNextFlag,\
				rcvdata.sMsgBuf->sDBHead.cRaflag,rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
				rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
				rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
				rcvdata.sMsgBuf->sDBHead.cZip,rcvdata.sMsgBuf->sDBHead.nLen);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_FUNCNOTFUND;
			rcvdata.sMsgBuf->sDBHead.nLen = 0;
			rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
	
			m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->SendMsg(rcvdata,0);
			return;
		}

		fn.nCallNum++;
		//b_Cinfo里的b_nSerial存放bpu连接index时间戳
		rcvdata.sMsgBuf->sDBHead.b_Cinfo.b_nSerial = m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->m_nConntime;
		strcpy(rcvdata.sBpuGroupName ,fn.sBpuGroupName);
		rcvdata.nRtime = time(NULL);

		//20151102 直接放到BPU对应的线程队列
		if (m_pRes->PushData(fn.nBpuGroupIndex,rcvdata,fn.cPrio) != 0)
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"入BPU组队列出错 消息[%s]  DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 源[%d %d] cZip[%d] 业务数据长度[%d]",\
				GetBpcMsgType(rcvdata.sMsgBuf->sBpcHead.cMsgType).c_str(),\
				GetDrebCmdType(rcvdata.sMsgBuf->sDBHead.cCmd).c_str(),rcvdata.sMsgBuf->sDBHead.cNextFlag,\
				rcvdata.sMsgBuf->sDBHead.cRaflag,rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
				rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
				rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
				rcvdata.sMsgBuf->sDBHead.cZip,rcvdata.sMsgBuf->sDBHead.nLen);
			m_pMemPool->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
		}
		
		//m_pPoolData->PushData(rcvdata,fn.cPrio);

	}
	else //bpccall应答
	{
		//b_Cinfo里的b_nSerial存放bpu连接index时间戳
		if (m_pSockMgr->AffirmIndex(rcvdata.sMsgBuf->sBpcHead.nBpuIndex))
		{
			if (rcvdata.sMsgBuf->sDBHead.b_Cinfo.b_nSerial == m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->m_nConntime)
			{
				rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN + rcvdata.sMsgBuf->sDBHead.nLen;
				rcvdata.sMsgBuf->sBpcHead.nConnectTime = m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->m_nConntime;
				rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
				m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->SendMsg(rcvdata);
				return;
			}
		}
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"错误 BPCCALL应答时连接不符，可能BPU已重启 消息[%s]  DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 源[%d %d] cZip[%d] 业务数据长度[%d]",\
			GetBpcMsgType(rcvdata.sMsgBuf->sBpcHead.cMsgType).c_str(),\
			GetDrebCmdType(rcvdata.sMsgBuf->sDBHead.cCmd).c_str(),rcvdata.sMsgBuf->sDBHead.cNextFlag,\
			rcvdata.sMsgBuf->sDBHead.cRaflag,rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
			rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
			rcvdata.sMsgBuf->sDBHead.cZip,rcvdata.sMsgBuf->sDBHead.nLen);
		//g_pBpcLog.LogBpc(LOG_ERROR,rcvdata.sMsgBuf,true);
		m_pRes->m_pBpcCallDataLog.LogBpc(LOG_ERROR,rcvdata.sMsgBuf,true);
		m_pMemPool->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
	}
}

void CPuThread::DispatchBpuIsReg(S_BPC_RSMSG &rcvdata)
{
	bool bRes=false;
	
	if (rcvdata.sMsgBuf->sDBHead.nLen <1)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"BPU注册时无bpu组名称 index=%d",rcvdata.sMsgBuf->sBpcHead.nBpuIndex);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPC_NOGROUP;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		rcvdata.sMsgBuf->sDBHead.nLen = 0;
		rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->SendMsg(rcvdata);
		return;
	}
	rcvdata.sMsgBuf->sBuffer[rcvdata.sMsgBuf->sDBHead.nLen]=0;
	bool isfind=false;
	int  bpuindex=0;
	for (int i=0;i<m_pRes->g_vBpuGroupInfo.size();i++)
	{
		if (strcmp(m_pRes->g_vBpuGroupInfo[i].g_sBpuGroupName.c_str(),rcvdata.sMsgBuf->sBuffer) == 0) //找到此BPU组
		{
			bpuindex = i;
			isfind = true;
			break;
		}
	}
	int connbpu=0;
	int connNum=0;
	int k;
	if (isfind) //有此bpu组
	{
		connNum=0;
		for (k=m_pRes->g_vBpuGroupInfo[bpuindex].g_nBeginSocketIndex ; k < m_pRes->g_vBpuGroupInfo[bpuindex].g_nEndSocketIndex; k++)
		{
			if (m_pSockMgr->at(k)->m_sock != INVALID_SOCKET)
			{
				connNum++;
			}
		}
		//判断此BPU组是否已全部连接满员
//		if (m_pRes->g_vBpuGroupInfo[bpuindex].g_nStartBpuNum >= m_pRes->g_vBpuGroupInfo[bpuindex].g_nBpuNum)
		if (connNum >= m_pRes->g_vBpuGroupInfo[bpuindex].g_nBpuNum)
		{
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.nLen = 0;
			rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPC_BPULIMIT;
			m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->OnClose("此组BPU连接已满",__FILE__,__LINE__);
			m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->SendMsg(rcvdata);
			return ;
		}
		std::string gpname= m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName;
		//将组名及组名索引放到连接类中并置启动的BPU数+1
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->m_sBpuGroupName = rcvdata.sMsgBuf->sBuffer;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->m_nBpuGroupIndex = bpuindex;
		m_pRes->g_vBpuGroupInfo[bpuindex].g_nStartBpuNum++;
		//将bpu的进程ID放到连接类中
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->m_nPid = rcvdata.sMsgBuf->sBpcHead.nRtime;

		//BPC配置是否要注册
		if (m_pRes->g_nRegisterMode == REGISTERMODE_NONE || m_pRes->g_nRegisterMode == REGISTERMODE_STATIC)
		{
			m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"BPC配置无须BPU注册");
			m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->m_bChecked = true;//置连接为已检查
			rcvdata.sMsgBuf->sDBHead.nLen = 0;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPC_NOREGISTER;
			m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->SendMsg(rcvdata);
			
			return;
		}
		else //返回是否要注册
		{
			bRes = m_pRes->g_vBpuGroupInfo[bpuindex].g_bIsReg;
			if (bRes)
			{
				m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"BPU之前已经注册过");
			}
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			sprintf(rcvdata.sMsgBuf->sBuffer,"%d",bRes);  //1已经注册过,0未注册 需要注册
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN + rcvdata.sMsgBuf->sDBHead.nLen;
			m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->SendMsg(rcvdata);
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
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->SendMsg(rcvdata);
	}
}


void CPuThread::LogDrebHead(int loglevel, DREB_HEAD head, const char *msg, const char *filename, int fileline)
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

void CPuThread::DispatchTrans(S_BPC_RSMSG &rcvdata)
{
	rcvdata.nRtime = time(NULL);
	//转移优先级为0
	m_pPoolData->PushData(rcvdata);//放入队列，msgthread线程进行处理
}

void CPuThread::DispatchGetNext(S_BPC_RSMSG &rcvdata)
{
	//取后续包的应答，直接返回
	//DREB的应答
	if (m_pSockMgr->AffirmIndex(rcvdata.sMsgBuf->sBpcHead.nIndex))
	{
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nNodeId = m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->m_head.a_Ainfo.a_nNodeId;
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_cNodePrivateId = m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->m_head.a_Ainfo.a_cNodePrivateId;
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata,0);
		return;
	}
	m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"GETNEXT应答返回的dreb连接非法 %d",rcvdata.sMsgBuf->sBpcHead.nIndex);
	LogDrebHead(LOG_ERROR_FAULT,rcvdata.sMsgBuf->sDBHead,"GETNEXT应答返回dreb连接非法",__FILE__,__LINE__);
	m_pMemPool->PoolFree(rcvdata.sMsgBuf);
	rcvdata.sMsgBuf = NULL;
	return;
}

void CPuThread::DispatchReq(S_BPC_RSMSG &rcvdata)
{
	//这里肯定是应答了
	if (rcvdata.sMsgBuf->sDBHead.cRaflag == 0)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"MSGREQ应答返回的cRaFlag非法，应为1应答");
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"MSGREQ应答返回的cRaFlag非法，应为1应答 消息[%s]  DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 源[%d %d] cZip[%d] 业务数据长度[%d]",\
			GetBpcMsgType(rcvdata.sMsgBuf->sBpcHead.cMsgType).c_str(),\
			GetDrebCmdType(rcvdata.sMsgBuf->sDBHead.cCmd).c_str(),rcvdata.sMsgBuf->sDBHead.cNextFlag,\
			rcvdata.sMsgBuf->sDBHead.cRaflag,rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
			rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
			rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
			rcvdata.sMsgBuf->sDBHead.cZip,rcvdata.sMsgBuf->sDBHead.nLen);

		rcvdata.sMsgBuf->sDBHead.cRaflag =1;
		m_pMemPool->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		OnClose(rcvdata.index,"严重错误 接收数据非法，关闭连接",__FILE__,__LINE__);
		return;
	}
	if (rcvdata.sMsgBuf->sDBHead.cNextFlag == 2 || rcvdata.sMsgBuf->sDBHead.cNextFlag == 20)
	{
		//处理多BPC多包应答 2为bpc生成多包缓冲,20为多包结束
		BpcNext(rcvdata);
		return;
	}
	if (rcvdata.sMsgBuf->sDBHead.cNextFlag == 0 || rcvdata.sMsgBuf->sDBHead.cNextFlag == 20 || \
		rcvdata.sMsgBuf->sDBHead.cNextFlag == 30 || rcvdata.sMsgBuf->sDBHead.cNextFlag == 10)
	{
		//无后续包了
		if (m_pSockMgr->AffirmIndex(rcvdata.index))
		{
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"BPU组[%s %d] BPU[%d %d]应答无后续包",\
				m_pSockMgr->at(rcvdata.index)->m_sBpuGroupName.c_str(),\
				m_pSockMgr->at(rcvdata.index)->m_nBpuGroupIndex,\
				m_pSockMgr->at(rcvdata.index)->m_index,\
				rcvdata.index);
//			m_pSockMgr->at(rcvdata.index)->m_bIsBuGetFlag = true;
		}
	}
	//DREB的应答
	if (m_pSockMgr->AffirmIndex(rcvdata.sMsgBuf->sBpcHead.nIndex))
	{
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nNodeId = m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->m_head.a_Ainfo.a_nNodeId;
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_cNodePrivateId = m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->m_head.a_Ainfo.a_cNodePrivateId;
//		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata,0);
		return;
	}
	m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"MSGREQ应答返回的dreb连接非法 消息[%s]  DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] cZip[%d] 业务数据长度[%d]",\
		GetBpcMsgType(rcvdata.sMsgBuf->sBpcHead.cMsgType).c_str(),\
		GetDrebCmdType(rcvdata.sMsgBuf->sDBHead.cCmd).c_str(),rcvdata.sMsgBuf->sDBHead.cNextFlag,\
		rcvdata.sMsgBuf->sDBHead.cRaflag,rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
		rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
		rcvdata.sMsgBuf->sDBHead.cZip,rcvdata.sMsgBuf->sDBHead.nLen);
	
	//g_pBpcLog.LogBpc(LOG_ERROR,rcvdata.sMsgBuf,true);
	m_pRes->m_pDrebDataLog.LogBpc(LOG_ERROR,rcvdata.sMsgBuf,true);
	m_pMemPool->PoolFree(rcvdata.sMsgBuf);
	rcvdata.sMsgBuf = NULL;
	return;
}



void CPuThread::BpcNext(S_BPC_RSMSG &rcvdata)
{
	S_NEXT_TBL nb;
	char sNextFile[400];
	FILE *fp=NULL;
	if (g_pBpcTime.m_pNextTbl.Select(rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,nb))
	{
		//找到了，将内容写到文件
#ifdef _WINDOWS
		sprintf(sNextFile,"%s\\nxt\\%ld.nxt",m_pRes->g_sCurPath,nb.nFileSerial);
#else
		sprintf(sNextFile,"%s/nxt/%ld.nxt",m_pRes->g_sCurPath,nb.nFileSerial);
#endif
		fp = fopen(sNextFile,"ab");
		if (fp == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"打开文件[%s]出错",sNextFile);
			return ;
		}
		fwrite(rcvdata.sMsgBuf->sBuffer,1,rcvdata.sMsgBuf->sDBHead.nLen,fp);
		fclose(fp);
		//g_pBpcLog.LogBpc(LOG_ERROR,rcvdata.sMsgBuf,true);
		m_pRes->m_pDrebDataLog.LogBpc(LOG_ERROR,rcvdata.sMsgBuf,true);
		m_pMemPool->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
	}
	else
	{
		//新生成文件
		std::string sfile = m_pRes->GetNextFileName(nb.nFileSerial);
		nb.nDrebSerial = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial;
		nb.nNodeId = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId;
		nb.nNodePrivateId = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId;
		
		fp = fopen(sfile.c_str(),"wb");
		if (fp == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"打开文件[%s]出错",sfile.c_str());
			return ;
		}
		fwrite(rcvdata.sMsgBuf->sBuffer,1,rcvdata.sMsgBuf->sDBHead.nLen,fp);
		fclose(fp);
		nb.nTime = time(NULL);
		g_pBpcTime.m_pNextTbl.Insert(nb);
		//返回给前端
		rcvdata.sMsgBuf->sDBHead.n_Ninfo.n_nNextNo = nb.nFileSerial;
		rcvdata.sMsgBuf->sDBHead.n_Ninfo.n_nNextOffset = rcvdata.sMsgBuf->sDBHead.nLen;
		//DREB的应答
		if (m_pSockMgr->AffirmIndex(rcvdata.sMsgBuf->sBpcHead.nIndex))
		{
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nNodeId = m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->m_head.a_Ainfo.a_nNodeId;
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_cNodePrivateId = m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->m_head.a_Ainfo.a_cNodePrivateId;
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
			m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata,0);
			
			return;
		}
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"应答返回的dreb连接非法 消息[%s]  DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] cZip[%d] 业务数据长度[%d]",\
			GetBpcMsgType(rcvdata.sMsgBuf->sBpcHead.cMsgType).c_str(),\
			GetDrebCmdType(rcvdata.sMsgBuf->sDBHead.cCmd).c_str(),rcvdata.sMsgBuf->sDBHead.cNextFlag,\
			rcvdata.sMsgBuf->sDBHead.cRaflag,rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
			rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
			rcvdata.sMsgBuf->sDBHead.cZip,rcvdata.sMsgBuf->sDBHead.nLen);
		
		m_pRes->m_pDrebDataLog.LogBpc(LOG_ERROR,rcvdata.sMsgBuf,true);
		//g_pBpcLog.LogBpc(LOG_ERROR,rcvdata.sMsgBuf,true);
		m_pMemPool->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;

	}
	return;
}
int CPuThread::OnTimer(unsigned int eventid, void *p)
{
	CPuThread *pp = (CPuThread *)p;
	if (eventid == 0)
	{
		pp->PingTimer();
	}
	return 0;
}
void CPuThread::PingTimer()
{
	//OnNoEvent();
	m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"定时器");
	m_bIsPingTimer = true;
}
void CPuThread::StartTimer()
{
	m_pTimer.Start();
}

void CPuThread::StopTimer()
{
	m_pTimer.Stop();
}