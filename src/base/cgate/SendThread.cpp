// SendThread.cpp: implementation of the CSendThread class.
//
//////////////////////////////////////////////////////////////////////

#include "SendThread.h"
#include "SubScribeInfo.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define  DESENCRYPTKEY3   "wms13711songfree"
#define  DESENCRYPTKEY    "rd402wms"

CSendThread::CSendThread()
{
	m_pSendData = NULL;
	m_pRes = NULL;
	m_pAioWork = NULL;
	bzero(m_sDestBuffer,sizeof(m_sDestBuffer));
	bzero(m_sSrcBuffer,sizeof(m_sSrcBuffer));
	m_nSendQuoteNum =0;
	m_nSendCustNum = 0;
	m_nIndex = 0;
}

CSendThread::~CSendThread()
{

}
void CSendThread::ExitThreadInstance()
{
	return ;
}
bool CSendThread::InitThreadInstance()
{
	return true;
}
bool CSendThread::SetGlobalVar(CGateResource *res,CSendData *senddata,CAIO_Work *aiowork)
{
 	m_pRes = res;
 	m_pSendData = senddata;
	m_pAioWork = aiowork;
 	m_pLog = m_pRes->g_pLog;
	m_pGateLog = &(m_pRes->g_pGateLog);
	return true;
}
int CSendThread::Run()
{

	m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"启动客户发送线程");
	int rtime = time(NULL);
	int etime;
	while (!m_pRes->g_bIsExit)
	{
		if (m_pSendData->GetData(m_pSmsg))
		{
			//对数据进行解密(若总线过来的数据是加密的情况下)
			if (!UnzipDrebBuf(m_pSmsg))
			{
				m_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, "总线数据解密失败");
				continue;
			}
			if (m_pSmsg.isBC == 0)	 //表示应答和推送
			{
				SendMsg(&m_pSmsg);
			}
			else  if (m_pSmsg.isBC == 1)	 //行情广播等公共信息
			{
                //发送广播
                SendBCMsg(&m_pSmsg);
			}
			else	//订阅的信息
			{
				SendSubscribeMsg(&m_pSmsg);
			}
			
			etime = time(NULL);
			if ((etime - rtime) > 20)
			{
				rtime = etime;
				m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"发送队列数[%d]",m_pSendData->GetSize());
			}
		}
	}
	m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"停止客户发送线程");
	return 0;
}

void CSendThread::SendMsg(S_CGATE_SMSG *msg)
{
	int nRet;
	PSOCKET_POOL_DATA info = m_pAioWork->GetPoolData(msg->index);
	if (info == NULL)
	{
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"连接index[%d]非法",msg->index);
		return ;
	}
	if (info->s_cCheckFlag != CHECKFLAG_NORMAL) //未验证通过，暂不发送
	{
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"连接index[%d] 未登录 %d",msg->index,info->s_cCheckFlag );
		return ;
	}
	if (info->s_hSocket == INVALID_SOCKET)
	{
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"连接index[%d]已关闭",msg->index);
		return ;
	}
	
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"开始发送交易数据 index[%d] 标识[%d %d %d %d] cMsgType[%s] cCmd[%s] cRaFlag[%d] cNextFlag[%d] Node[%d %d] svr[%d %d] 交易码[%d] next[%d %d] czip[%d] nLen[%d]",\
		msg->index,msg->d_nNodeId,msg->d_cNodePrivateId,msg->s_nDrebSerial,msg->data.head.stDest.nSerial,GetMsgType(msg->data.head.stComm.cMsgType).c_str(),\
		GetDrebCmdType(msg->data.head.stComm.cCmd).c_str(),msg->data.head.stComm.cRaflag,\
		msg->data.head.stComm.cNextFlag,msg->data.head.stDest.nNodeId,msg->data.head.stDest.cNodePrivateId,\
		msg->data.head.stDest.nSvrMainId,msg->data.head.stDest.cSvrPrivateId,msg->data.head.stDest.nServiceNo,\
		msg->data.head.stNext.n_nNextNo,msg->data.head.stNext.n_nNextOffset,msg->data.head.stComm.cZip,msg->data.head.nLen);
	
	if (msg->data.head.stComm.cMsgType != MSG_GATEPING && msg->data.head.stComm.cMsgType != MSG_GATESUBSCRIBE)
	{
		if (msg->timestamp == 0)
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送交易数据怎会出现推送广播数据");
			return;
		}
	}
	if (m_pRes->g_nEncryptFlag == 1  )
	{
		if (!Encrypt(msg))
		{
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"数据加密出错 index[%d] 标识[%d %d %d %d] cMsgType[%s] cCmd[%s] cRaFlag[%d] cNextFlag[%d] Node[%d %d] svr[%d %d] 交易码[%d] next[%d %d] czip[%d] nLen[%d]",\
				msg->index,msg->d_nNodeId,msg->d_cNodePrivateId,msg->s_nDrebSerial,msg->data.head.stDest.nSerial,GetMsgType(msg->data.head.stComm.cMsgType).c_str(),\
				GetDrebCmdType(msg->data.head.stComm.cCmd).c_str(),msg->data.head.stComm.cRaflag,\
				msg->data.head.stComm.cNextFlag,msg->data.head.stDest.nNodeId,msg->data.head.stDest.cNodePrivateId,\
				msg->data.head.stDest.nSvrMainId,msg->data.head.stDest.cSvrPrivateId,msg->data.head.stDest.nServiceNo,\
				msg->data.head.stNext.n_nNextNo,msg->data.head.stNext.n_nNextOffset,msg->data.head.stComm.cZip,msg->data.head.nLen);
			return ;
		}
	}
	SOCKET_SEND_DATA dd;
	dd.s_nIndex = info->s_nIndex;
	dd.s_nSendLen = 0;
	dd.s_nRet = 0;
	dd.s_nSendNum = 0;
	dd.s_nTimestamp = info->s_nTimestamp;
	dd.s_nTotalLen = CGATEHEADLEN+msg->data.head.nLen;
	dd.s_nHook = msg->data.head.stDest.nSerial;

	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"发送长度[%d] 数据长度[%d] 报文头长度[%d] cZip[%d] index[%d] 标识[%d]",\
		dd.s_nTotalLen,msg->data.head.nLen,CGATEHEADLEN,msg->data.head.stComm.cZip,info->s_nIndex,msg->data.head.stDest.nSerial);

	//字节序转换
	CGATE_HEAD cgatehead;
	memcpy(&(cgatehead),&(msg->data.head),CGATEHEADLEN);

	Endian2CommCrc(cgatehead);
	
	memcpy(dd.s_sSendBuffer,&(cgatehead),CGATEHEADLEN);
	if (dd.s_nTotalLen > DREBBUFSIZE)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"发送数据长度[%d]不符 ",dd.s_nTotalLen);
		return;
	}
	else
	{
		memcpy(dd.s_sSendBuffer+CGATEHEADLEN,msg->data.buffer,msg->data.head.nLen);
	}
	if (info->s_hSocket == INVALID_SOCKET)
	{
		m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"发送交易数据失败 连接index[%d]已关闭 标识[%d]",msg->index,msg->data.head.stDest.nSerial);
		return ;
	}
	if (msg->data.head.stComm.cMsgType != MSG_GATEPING && msg->data.head.stComm.cMsgType != MSG_GATESUBSCRIBE)
	{
		if (info->s_nTimestamp != msg->timestamp)
		{
			m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"index[%d] 连接时间戳[%d]和连接的[%d]不符 标识[%d]",\
				msg->index,msg->timestamp,info->s_nTimestamp,msg->data.head.stDest.nSerial);
			return ;
		}
	}
	dd.s_nTxCode = msg->txcode;
	dd.s_serial  = msg->s_nDrebSerial;
	dd.s_nTimestamp = msg->timestamp;
	dd.s_nSendLen = 0;
	//发送数据
	if (!m_pAioWork->Send2Client(dd))
	{
		m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"发送交易数据失败 标识[%d %d %d %d] ",msg->d_nNodeId,msg->d_cNodePrivateId,msg->s_nDrebSerial,msg->data.head.stDest.nSerial);
		return ;
	}
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"发送交易数据完成 标识[%d %d %d %d] ",msg->d_nNodeId,msg->d_cNodePrivateId,msg->s_nDrebSerial,msg->data.head.stDest.nSerial);
}


void CSendThread::SendSingleBcMsg(S_CGATE_SMSG *msg)
{
	int nRet;
	PSOCKET_POOL_DATA info = m_pAioWork->GetPoolData(msg->index);
	if (info == NULL)
	{
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"连接index[%d]非法",msg->index);
		return ;
	}
	if (info->s_hSocket == INVALID_SOCKET)
	{
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"连接index[%d]已关闭",msg->index);
		return ;
	}

	if (info->s_cCheckFlag != CHECKFLAG_NORMAL) //未验证通过，暂不发送
	{
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"连接index[%d] 未登录 %d",msg->index,info->s_cCheckFlag );
		return ;
	}
	
	m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"开始发送广播推送数据 index[%d] 标识[%d %d %d %d] cMsgType[%s] cCmd[%s] cRaFlag[%d] cNextFlag[%d] Node[%d %d] svr[%d %d] 交易码[%d] next[%d %d] czip[%d] nLen[%d]",\
		msg->index,msg->d_nNodeId,msg->d_cNodePrivateId,msg->s_nDrebSerial,msg->data.head.stDest.nSerial,GetMsgType(msg->data.head.stComm.cMsgType).c_str(),\
		GetDrebCmdType(msg->data.head.stComm.cCmd).c_str(),msg->data.head.stComm.cRaflag,\
		msg->data.head.stComm.cNextFlag,msg->data.head.stDest.nNodeId,msg->data.head.stDest.cNodePrivateId,\
		msg->data.head.stDest.nSvrMainId,msg->data.head.stDest.cSvrPrivateId,msg->data.head.stDest.nServiceNo,\
		msg->data.head.stNext.n_nNextNo,msg->data.head.stNext.n_nNextOffset,msg->data.head.stComm.cZip,msg->data.head.nLen);
	

	SOCKET_SEND_DATA dd;
	dd.s_nIndex = info->s_nIndex;
	dd.s_nSendLen = 0;
	dd.s_nRet = 0;
	dd.s_nSendNum = 0;
	dd.s_nTimestamp = info->s_nTimestamp;
	dd.s_nTotalLen = CGATEHEADLEN+msg->data.head.nLen;
	
	dd.s_nHook = msg->data.head.stDest.nSerial;

	m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"广播推送发送长度[%d] 数据长度[%d] 报文头长度[%d] cZip[%d] index[%d] 标识[%d]",\
		dd.s_nTotalLen,msg->data.head.nLen,CGATEHEADLEN,msg->data.head.stComm.cZip,info->s_nIndex,msg->data.head.stDest.nSerial);
	
	//字节序转换
	CGATE_HEAD cgatehead;
	memcpy(&(cgatehead),&(msg->data.head),CGATEHEADLEN);

	Endian2CommCrc(cgatehead);
	
	memcpy(dd.s_sSendBuffer,&(cgatehead),CGATEHEADLEN);
	if (dd.s_nTotalLen > DREBBUFSIZE)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"发送数据长度[%d]不符 ",dd.s_nTotalLen);
		return;
	}
	else
	{
		memcpy(dd.s_sSendBuffer+CGATEHEADLEN,msg->data.buffer,msg->data.head.nLen);
	}
	if (info->s_hSocket == INVALID_SOCKET)
	{
		m_pLog->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"发送广播失败 连接index[%d]已关闭 标识[%d]",msg->index,msg->data.head.stDest.nSerial);
		return ;
	}
	//发送数据
	dd.s_nTxCode = msg->txcode;
	dd.s_serial  = msg->s_nDrebSerial;
	dd.s_nTimestamp = msg->timestamp;
	dd.s_nSendLen = 0;
	if (!m_pAioWork->Send2Client(dd))
	{
		m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"发送广播失败 标识[%d %d %d %d] ",msg->d_nNodeId,msg->d_cNodePrivateId,msg->s_nDrebSerial,msg->data.head.stDest.nSerial);
		return ;
	}
	m_nSendCustNum++;
	m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"发送广播完成 标识[%d %d %d %d] ",msg->d_nNodeId,msg->d_cNodePrivateId,msg->s_nDrebSerial,msg->data.head.stDest.nSerial);
}
void CSendThread::SendBCMsg(S_CGATE_SMSG *msg)
{
	msg->timestamp = 0;
	S_CGATE_SMSG senddata;
	bzero(&senddata,sizeof(S_CGATE_SMSG));
	int total;
	char tmpchar[100];
	
	m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"开始发送广播数据 标识[%d %d %d %d] cMsgType[%s] cCmd[%s] cRaFlag[%d] cNextFlag[%d] Node[%d %d] svr[%d %d] 交易码[%d] next[%d %d]",\
		msg->d_nNodeId,msg->d_cNodePrivateId,msg->s_nDrebSerial,msg->data.head.stDest.nSerial,GetMsgType(msg->data.head.stComm.cMsgType).c_str(),\
		GetDrebCmdType(msg->data.head.stComm.cCmd).c_str(),msg->data.head.stComm.cRaflag,\
		msg->data.head.stComm.cNextFlag,msg->data.head.stDest.nNodeId,msg->data.head.stDest.cNodePrivateId,\
		msg->data.head.stDest.nSvrMainId,msg->data.head.stDest.cSvrPrivateId,msg->data.head.stDest.nServiceNo,\
		msg->data.head.stNext.n_nNextNo,msg->data.head.stNext.n_nNextOffset);
	int poolsize = m_pAioWork->GetConnectPoolInfo(total);
	PSOCKET_POOL_DATA info  = NULL;
	for (int i=1 ; i<poolsize ; i++) //0是网关的侦听
	{
		info =  m_pAioWork->GetPoolData(i);
		if (info == NULL)
		{
			continue;
		}
		if (info->s_hSocket == INVALID_SOCKET)
		{
//			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"连接index[%d]已关闭",msg->index);
			continue ;
		}
		if (info->s_cCheckFlag != CHECKFLAG_NORMAL) //未验证通过，暂不发送
		{
//			m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"连接index[%d] 未登录 %d",i,info->s_cCheckFlag );
			continue;
		}
		if (m_pRes->g_nQuoSubScribe == 1) //支持行情订阅
		{
			if (msg->nVerietyId != 0) //有合约id，表示是行情，否则是公告类的，直接发
			{
                if (NULL == info->ptr)
                {
                    continue;
                }
                CSubScribeInfo* psub = (CSubScribeInfo*)info->ptr;
                if (!psub->IsQuoSubscribe(msg->nVerietyId))
                {
                    m_pLog->LogMp(LOG_DEBUG + 1,__FILE__,__LINE__,"连接index[%d] 未订阅行情 %d",i,msg->nVerietyId);
                    continue;
                }
			}
		}
		memcpy(&senddata,msg,sizeof(S_CGATE_SMSG));	
		senddata.index = i;
		senddata.timestamp = 0;
		SendSingleBcMsg(&senddata);

	}
	m_nSendQuoteNum++;
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"发送广播数[%d],成功发给客户广播数[%d]",m_nSendQuoteNum,m_nSendCustNum);
	m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"发送广播完成 标识[%d %d %d] ",msg->d_nNodeId,msg->d_cNodePrivateId,msg->s_nDrebSerial);
}

std::string CSendThread::GetMsgType(int type)
{
	// #define  MSG_GATEPING      1          //和网关的PING消息
	// #define  MSG_GATEREQ       2          //发给网关的请求数据
	// #define  MSG_GATELOGIN     3          //网关登录
	// #define  MSG_PUSH          5          //api收到的推送数据流，公共流(行情、公告)，私有流(回报等)
	// #define  MSG_GATEPKISTEP1  6          //网关PKI认证第一步 网关返回签名串
	// #define  MSG_GATEPKISTEP2  7          //网关PKI认证第二步 API发给网关签名及证书公钥、签名字符串
	// #define  MSG_GATEPKISTEP3  8          //网关PKI认证第三步 网关发给API签名及证书公钥、会话密钥
	switch (type)
	{
	case MSG_GATEPING:
		return "MSG_GATEPING";
	case MSG_GATEREQ:
		return "MSG_GATEREQ";
	case MSG_GATELOGIN:
		return "MSG_GATELOGIN";
	case MSG_PUSH:
		return "MSG_PUSH";
	case MSG_GATEPKISTEP1:
		return "MSG_GATEPKISTEP1";
	case MSG_GATEPKISTEP2:
		return "MSG_GATEPKISTEP2";
	case MSG_GATEPKISTEP3:
		return "MSG_GATEPKISTEP3";
	default:
		return "";
	}
}
std::string CSendThread::GetDrebCmdType(int cmdtype)
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
void CSendThread::Endian2CommCrc(CGATE_HEAD &head)
{
	head.stComm.nCrc = 0;
	m_pEndian.Endian2Comm((unsigned char *)&(head.nLen),sizeof(head.nLen));
	m_pEndian.Endian2Comm((unsigned char *)&(head.nRetCode),sizeof(head.nRetCode));
	m_pEndian.Endian2Comm((unsigned char *)&(head.stDest.nNodeId),sizeof(head.stDest.nNodeId));
	m_pEndian.Endian2Comm((unsigned char *)&(head.stDest.nSerial),sizeof(head.stDest.nSerial));
	m_pEndian.Endian2Comm((unsigned char *)&(head.stDest.nServiceNo),sizeof(head.stDest.nServiceNo));
	m_pEndian.Endian2Comm((unsigned char *)&(head.stDest.nSvrMainId),sizeof(head.stDest.nSvrMainId));
	m_pEndian.Endian2Comm((unsigned char *)&(head.stNext.n_nNextNo),sizeof(head.stNext.n_nNextNo));
	m_pEndian.Endian2Comm((unsigned char *)&(head.stNext.n_nNextOffset),sizeof(head.stNext.n_nNextOffset));
	head.stComm.nCrc = m_pEndian.CRC(&head,sizeof(CGATE_HEAD));
//	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"CRC值[%d]",head.stComm.nCrc);
	m_pEndian.Endian2Comm((unsigned char *)&(head.stComm.nCrc),sizeof(head.stComm.nCrc));

}

bool CSendThread::Encrypt(S_CGATE_SMSG *msg)
{
	PSOCKET_POOL_DATA info = m_pAioWork->GetPoolData(msg->index);
	if (info == NULL)
	{
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"连接index[%d]非法",msg->index);
		return false;
	}
	//针对不同的消息进行加密压缩
	switch (msg->data.head.stComm.cMsgType)
	{
		case MSG_GATEPING: //数据不用加密压缩
			break;
		case MSG_GATEPKISTEP1://压缩+固定密钥
			if (!ZipBuf(info,&(msg->data),14))
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"压缩+固定密钥加密出错");
				return false;
			}
			break;
		case MSG_GATEPKISTEP2://压缩+固定密钥
			if (!ZipBuf(info,&(msg->data),14))
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"压缩+固定密钥加密出错");
				return false;
			}
			break;
		case MSG_GATEPKISTEP3://压缩+加密公钥
			if (!ZipBuf(info,&(msg->data),15))
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"压缩+加密公钥加密出错");
				return false;
			}
			break;
		case MSG_GATESMS:
		case MSG_GATELOGIN:   //压缩+会话密钥DES
		case MSG_GATEREQ:
		case MSG_PUSH:
			if (!ZipBuf(info,&(msg->data),4))
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"压缩+会话密钥DES加密出错");
				return false;
			}
			break;
		default:
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"消息类型[%s]不用加密压缩处理",GetMsgType(msg->data.head.stComm.cMsgType).c_str());
			break;
	}
	return true;
}
bool CSendThread::ZipBuf(PSOCKET_POOL_DATA info,PCGATE_COMMSTRU  data,int nZipFlag)
{
	//0明文 1des加密密钥长度16 2压缩 3DES加密 4DES加密并压缩
	if (data->head.stComm.cZip != 0 && data->head.stComm.cZip != FLAG_ZIP)
	{
		return false;
	}
	if (data->head.nLen == 0)
	{
		return true;
	}
	char zipDataBuf[CLIENTDATASIZE+1];
	bzero(zipDataBuf,sizeof(zipDataBuf));
	unsigned int ziplen = CLIENTDATASIZE;
	int deslen=0;
	unsigned short shortlen;

	m_pGateLog->LogCgate(LOG_DEBUG,data,info->s_nIndex,info->s_sSessionKey);
	
	
	switch (nZipFlag)
	{
		case FLAG_ZIPSESSIONKEY:
			if (m_pRes->g_nZipFlag == 1)
			{
				if (data->head.stComm.cZip == 0)
				{
					ziplen = CLIENTDATASIZE;
					memcpy(zipDataBuf,data->buffer,data->head.nLen);
					if (!CBF_Tools::Compress((unsigned char *)data->buffer,ziplen,(unsigned char *)(zipDataBuf),data->head.nLen))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"数据压缩失败");
						return false;
					}
					data->head.nLen = ziplen;
					data->head.stComm.cZip = FLAG_ZIP;
					m_pGateLog->LogCgate(LOG_DEBUG,data,info->s_nIndex,info->s_sSessionKey);
					
				}
				if (data->head.stComm.cZip != FLAG_ZIP)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"数据不是压缩数据");
					return false;
				}
				deslen = data->head.nLen;
			
				if (m_pRes->g_nEncryptFlag == 1)
				{
					if (m_pDesSend.EncryptString(data->buffer,deslen,info->s_sSessionKey)<=0)
					{
						m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"加密出错");
						return false;
					}
					if (atoi(info->s_sSessionKey) == 0)
					{
						m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"加密出错 密钥[%s]",info->s_sSessionKey);
					}
					data->head.nLen = deslen;
					data->head.stComm.cZip = FLAG_ZIPSESSIONKEY;

				}
			}
			else
			{
				deslen = data->head.nLen;
				if (m_pRes->g_nEncryptFlag == 1)
				{
					if (m_pDesSend.EncryptString(data->buffer,deslen,info->s_sSessionKey)<=0)
					{
						m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"加密出错");
						return false;
					}
					if (atoi(info->s_sSessionKey) == 0)
					{
						m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"加密出错 密钥[%s]",info->s_sSessionKey);
					}
					data->head.nLen = deslen;
					if (data->head.stComm.cZip == 0)
					{
						data->head.stComm.cZip = FLAG_SESSIONKEY;
					}
					else
					{
						data->head.stComm.cZip = FLAG_ZIPSESSIONKEY;
					}
				}
			}
			
			break;
		case FLAG_ZIPFIXEDKEY:
			if (m_pRes->g_nZipFlag == 1)
			{
				if (data->head.stComm.cZip == 0)
				{
					ziplen = CLIENTDATASIZE;
					memcpy(zipDataBuf,data->buffer,data->head.nLen);
					if (!CBF_Tools::Compress((unsigned char *)data->buffer,ziplen,(unsigned char *)(zipDataBuf),data->head.nLen))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"数据压缩失败");
						return false;
					}
					data->head.nLen = ziplen;
					data->head.stComm.cZip = FLAG_ZIP;
					m_pGateLog->LogCgate(LOG_DEBUG,data,info->s_nIndex,info->s_sSessionKey);
				}
			//不加密，多线程加密可能会导致数据解不开

// 				if (data->head.stComm.cZip != FLAG_ZIP)
// 				{
// 					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"数据不是压缩数据");
// 					return false;
// 				}
// 				deslen = data->head.nLen;
// 				if (m_pDesSend.EncryptString(data->buffer,deslen,DESENCRYPTKEY3)<=0)
// 				{
// 					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"加密出错");
// 					return false;
// 				}
// 				data->head.nLen = deslen;
//				data->head.stComm.cZip = FLAG_ZIPFIXEDKEY;
	
			}
			else
			{

			}
			break;
		case FLAG_ZIPCERTKEY:
			if (m_pRes->g_nZipFlag == 1)
			{
				if (data->head.stComm.cZip == 0)
				{
					memcpy(zipDataBuf,data->buffer,data->head.nLen);
					ziplen = CLIENTDATASIZE;
					if (!CBF_Tools::Compress((unsigned char *)data->buffer,ziplen,(unsigned char *)(zipDataBuf),data->head.nLen))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"数据压缩失败");
						return false;
					}
					data->head.nLen = ziplen;
					data->head.stComm.cZip = FLAG_ZIP;
					m_pGateLog->LogCgate(LOG_DEBUG,data,info->s_nIndex,info->s_sSessionKey);
				}
				if (data->head.stComm.cZip != FLAG_ZIP)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"数据不是压缩数据");
					return false;
				}
				if (m_pRes->g_nUseCert == 1)
				{
					shortlen = data->head.nLen;
					m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"ip[%s] index[%d] 加密公钥长度[%d]",inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,info->s_nEncryptPubKeyLen);
					if (!m_pRes->g_pCertClass->Encrypt(info->s_sEncryptPubKey,info->s_nEncryptPubKeyLen,data->buffer,shortlen))
					{
						m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"加密出错 %s",m_pRes->g_pCertClass->GetErrMsg().c_str());
						return false;
					}
					data->buffer[shortlen]=0;
					data->head.nLen = shortlen;
					data->head.stComm.cZip =FLAG_ZIPCERTKEY;

				}
			}
			else
			{
				if (m_pRes->g_nUseCert == 1)
				{
					shortlen = data->head.nLen;
					m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"ip[%s] index[%d] 加密公钥长度[%d]",inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,info->s_nEncryptPubKeyLen);
					if (!m_pRes->g_pCertClass->Encrypt(info->s_sEncryptPubKey,info->s_nEncryptPubKeyLen,data->buffer,shortlen))
					{
						m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"加密出错 %s",m_pRes->g_pCertClass->GetErrMsg().c_str());
						return false;
					}
					data->buffer[shortlen]=0;
					data->head.nLen = shortlen;
					if (data->head.stComm.cZip == 0)
					{
						data->head.stComm.cZip =FLAG_CERTKEY;
					}
					else
					{
						data->head.stComm.cZip =FLAG_ZIPCERTKEY;
					}
				}
			}
			break;
		default:
			return true;
	}
	m_pGateLog->LogCgate(LOG_DEBUG,data,info->s_nIndex,info->s_sSessionKey);
	return true;
}

bool CSendThread::UnzipDrebBuf(S_CGATE_SMSG &data)
{
    //0明文 1des加密密钥长度16 2压缩 3DES加密 4DES加密并压缩
    if (data.data.head.nLen == 0)
    {
        return true;
    }
    if (data.data.head.stComm.cZip > 0 && data.data.head.stComm.cZip <= 4)
    {
        char zipDataBuf[BPUDATASIZE];
        bzero(zipDataBuf, sizeof(zipDataBuf));
        int ddlen;
        switch (data.data.head.stComm.cZip)
        {
        case 1:
            ddlen = data.data.head.nLen;
            if (m_pDes.UncryptString(data.data.buffer, ddlen, DESENCRYPTKEY) < 1)
            {
                m_pLog->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "解密出错");
                return false;
            }
			data.data.head.nLen = ddlen;
			data.data.head.stComm.cZip = 0;
            break;
        case 2:
            break;
        case 3:
            ddlen = data.data.head.nLen;
            if (m_pDes.UncryptString(data.data.buffer, ddlen, DESENCRYPTKEY3) < 1)
            {
                m_pLog->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "解密出错");
                return false;
            }
			data.data.head.nLen = ddlen;
			data.data.head.stComm.cZip = 0;
            break;
        case 4:
            ddlen = data.data.head.nLen;
            if (m_pDes.UncryptString(data.data.buffer, ddlen, DESENCRYPTKEY3) < 1)
            {
                m_pLog->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "解密出错");
                return false;
            }
			data.data.head.nLen = ddlen;
			data.data.head.stComm.cZip = 2;
            break;
        default:
            return false;
        }
    }

    return true;
}

void CSendThread::SendSubscribeMsg(S_CGATE_SMSG* msg)
{
    msg->timestamp = 0;
    S_CGATE_SMSG senddata;
    bzero(&senddata, sizeof(S_CGATE_SMSG));
    int total;
    char tmpchar[100];

    m_pLog->LogMp(LOG_DEBUG + 1, __FILE__, __LINE__, "开始发送回报类的广播数据 标识[%d %d %d %d] cMsgType[%s] cCmd[%s] cRaFlag[%d] cNextFlag[%d] Node[%d %d] svr[%d %d] 交易码[%d] next[%d %d]", \
        msg->d_nNodeId, msg->d_cNodePrivateId, msg->s_nDrebSerial, msg->data.head.stDest.nSerial, GetMsgType(msg->data.head.stComm.cMsgType).c_str(), \
        GetDrebCmdType(msg->data.head.stComm.cCmd).c_str(), msg->data.head.stComm.cRaflag, \
        msg->data.head.stComm.cNextFlag, msg->data.head.stDest.nNodeId, msg->data.head.stDest.cNodePrivateId, \
        msg->data.head.stDest.nSvrMainId, msg->data.head.stDest.cSvrPrivateId, msg->data.head.stDest.nServiceNo, \
        msg->data.head.stNext.n_nNextNo, msg->data.head.stNext.n_nNextOffset);
    int poolsize = m_pAioWork->GetConnectPoolInfo(total);
    PSOCKET_POOL_DATA info = NULL;
    for (int i = 1; i < poolsize; i++) //0是网关的侦听
    {
        info = m_pAioWork->GetPoolData(i);
        if (info == NULL)
        {
            continue;
        }
        if (info->s_hSocket == INVALID_SOCKET)
        {
            continue;
        }
        if (info->s_cCheckFlag != CHECKFLAG_NORMAL) //未验证通过，暂不发送
        {
            continue;
        }

		//要验证key是否订阅
        if (NULL == info->ptr)
        {
            continue;
        }
        CSubScribeInfo* psub = (CSubScribeInfo*)info->ptr;
        if (!psub->IsSubscribe(msg->data.head.stDest.nSerial))
        {
            m_pLog->LogMp(LOG_DEBUG + 1, __FILE__, __LINE__, "连接index[%d] 未订阅回报 key[%d]", i, msg->data.head.stDest.nSerial);
            continue;
        }

        memcpy(&senddata, msg, sizeof(S_CGATE_SMSG));
        senddata.index = i;
        senddata.timestamp = 0;
        SendSingleBcMsg(&senddata);

    }
    m_nSendQuoteNum++;
    m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "发送回报类广播数[%d],成功发给客户广播数[%d]", m_nSendQuoteNum, m_nSendCustNum);
    m_pLog->LogMp(LOG_DEBUG + 1, __FILE__, __LINE__, "发送回报类广播完成 标识[%d %d %d] ", msg->d_nNodeId, msg->d_cNodePrivateId, msg->s_nDrebSerial);
}

