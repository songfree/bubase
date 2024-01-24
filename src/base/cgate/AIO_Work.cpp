// AIO_Work.cpp: implementation of the CAIO_Work class.
//
//////////////////////////////////////////////////////////////////////

#include "AIO_Work.h"
#include "SubScribeInfo.h"

#ifdef LINUX
#include<setjmp.h>
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


#define  DESENCRYPTKEY3   "wms13711songfree"



CAIO_Work::CAIO_Work()
{
	m_pDataPool = NULL;
	m_pRes = NULL;
	bzero(&m_pRcvData,sizeof(CGATE_COMMSTRU));
	bzero(m_sDestBuffer,sizeof(m_sDestBuffer));
	bzero(m_sSrcBuffer,sizeof(m_sSrcBuffer));
	m_pLog = NULL;
	m_pMemPool = NULL;
}

CAIO_Work::~CAIO_Work()
{

}

void CAIO_Work::SetGlobalVar(CGateResource *res,CPoolData *pool,CBF_BufferPool *mempool,CSendData *senddata)
{
	m_pDataPool = pool;
	m_pRes = res;
	m_pMemPool = mempool;
	m_pLog = m_pRes->g_pLog;
	m_pSendData = senddata;

	m_pGateLog = &(m_pRes->g_pGateLog);

}


// 函数名: Connect
// 编程  : 王明松 2012-5-21 11:55:51
// 返回  : void 
// 参数  : PSOCKET_POOL_DATA *info
// 描述  : AIO通知，连接断开，要求连接
void CAIO_Work::OnConnect(PSOCKET_POOL_DATA info)
{
	//没有主动连接的
}


// 函数名: OnSendBack
// 编程  : 王明松 2012-5-17 16:38:08
// 返回  : virtual void 
// 参数  : PSOCKET_SEND_DATA data   
// 描述  : AIO通知，写入数据返回，对返回结果进行处理
void CAIO_Work::OnSendBack(PSOCKET_SEND_DATA data,bool issuccess)
{
	if (issuccess)
	{
		m_pRes->g_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"发送完成 %d",data->s_nIndex);
//		m_pRes->g_pLog.LogBin(LOG_DEBUG,__FILE__,__LINE__,"发送完成",data->s_sSendBuffer,data->s_nTotalLen);
	}
	else
	{
		m_pRes->g_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送失败 %d",data->s_nIndex);
//		m_pRes->g_pLog.LogBin(LOG_DEBUG,__FILE__,__LINE__,"发送完成",data->s_sSendBuffer,data->s_nTotalLen);
	}
}
void CAIO_Work::OnSendBack(SOCKSENDQUEUE::iterator  data,bool issuccess)
{
	if (issuccess)
	{
		m_pRes->g_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"发送完成 %d",data->s_nIndex);
//		m_pRes->g_pLog.LogBin(LOG_DEBUG,__FILE__,__LINE__,"发送完成",data->s_sSendBuffer,data->s_nTotalLen);
	}
	else
	{
		m_pRes->g_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"发送失败 %d",data->s_nIndex);
//		m_pRes->g_pLog.LogBin(LOG_DEBUG,__FILE__,__LINE__,"发送完成",data->s_sSendBuffer,data->s_nTotalLen);
	}
}

// 函数名: OnPing
// 编程  : 王明松 2012-5-21 11:54:23
// 返回  : virtual void 
// 描述  : AIO通知，要发送PING包
void CAIO_Work::OnPing(PSOCKET_POOL_DATA info)
{
	if (time(NULL) - info->s_nPingTime < m_pRes->g_nHeartRun)
	{
		return;
	}
	CGATE_COMMSTRU data;
	bzero(&data,sizeof(CGATE_COMMSTRU));
	data.head.nLen = 0;
	data.head.nRetCode = 0;
	data.head.stComm.cMsgType = MSG_GATEPING;
	data.head.stComm.cCmd = 0;
	data.head.stComm.cNextFlag = 0;
	data.head.stComm.cRaflag = 0;
	data.head.stComm.cZip =0;
	data.head.stComm.nCrc =0;
	info->s_nPingTime = time(NULL);
	
	if (info->s_cCheckFlag != CHECKFLAG_NORMAL) //未验证通过，暂不发送
	{
		//确定不加密
		SendData(&data,info->s_nIndex);
	}
	else
	{
		SendData(&data,info->s_nIndex,false,0);
	}

}

// 函数名: OnRecvBack
// 编程  : 王明松 2012-5-17 16:36:58
// 返回  : virtual void 
// 参数  : PSOCKET_POOL_DATA info
// 描述  : AIO通知，连接收到数据，对数据进行处理
void CAIO_Work::OnRecvBack(PSOCKET_POOL_DATA info)
{
	//收到数据了，取数据，放入处理队列
	//收到的数据大于包头
	info->s_nRealBufferLen += info->s_nIoBufferLen;
	info->s_nIoBufferLen = 0;
	info->s_pIoBuffer = info->s_sRealBuffer+info->s_nRealBufferLen;
	CGATE_COMMSTRU msg;
	while (info->s_nRealBufferLen >= CGATEHEADLEN )
	{
		//进行字节序处理并校验CRC
		memcpy(&(msg.head),info->s_sRealBuffer,CGATEHEADLEN);
		if (!Endian2HostCrc(msg.head))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"CRC错误");
			OnClose(info,"错误");
			return ;
		}
		if (info->s_nRealBufferLen < msg.head.nLen + CGATEHEADLEN)
		{
			//说明数据没有接收完整
			return ;
		}
		if (msg.head.nLen >0)
		{
			if (msg.head.nLen > CLIENTDATASIZE)
			{
				OnClose(info,"报文长度不符");
				return;
			}
			memcpy(msg.buffer,info->s_sRealBuffer+CGATEHEADLEN,msg.head.nLen);
		}
		
		//记入接收日志
		if (msg.head.stComm.cMsgType != MSG_GATEPING )
		{
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"接收到的数据 客户端[%s] index[%d] 标识[%d] cMsgType[%s] cCmd[%s] cRaFlag[%d] cNextFlag[%d] Node[%d %d] svr[%d %d] 交易码[%d] next[%d %d] cZip[%d] nLen[%d]",\
				inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,msg.head.stDest.nSerial,GetMsgType(msg.head.stComm.cMsgType).c_str(),\
				GetDrebCmdType(msg.head.stComm.cCmd).c_str(),msg.head.stComm.cRaflag,\
				msg.head.stComm.cNextFlag,msg.head.stDest.nNodeId,msg.head.stDest.cNodePrivateId,\
				msg.head.stDest.nSvrMainId,msg.head.stDest.cSvrPrivateId,msg.head.stDest.nServiceNo,\
				msg.head.stNext.n_nNextNo,msg.head.stNext.n_nNextOffset,msg.head.stComm.cZip,msg.head.nLen);
		}
		else
		{
			m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"接收到的MSG_GATEPING消息");
		}

		//收多了，将原包重新给m_sRcvBuffer
		for (unsigned int i=0; i<info->s_nRealBufferLen-CGATEHEADLEN-msg.head.nLen; i++)
		{
			info->s_sRealBuffer[i]=info->s_sRealBuffer[i+CGATEHEADLEN+msg.head.nLen];
		}
		info->s_nRealBufferLen=info->s_nRealBufferLen - CGATEHEADLEN-msg.head.nLen;

		info->s_pIoBuffer = info->s_sRealBuffer+info->s_nRealBufferLen;

		if (msg.head.stComm.cRaflag == 1) //客户端的应答
		{
			if (msg.head.stComm.cMsgType != MSG_GATEPING)
			{
				m_pLog->LogMp(LOG_PROMPT, __FILE__, __LINE__, "客户端index[%d] cMsgType[%s]应答 丢弃", info->s_nIndex, GetMsgType(msg.head.stComm.cMsgType).c_str());
			}
			continue;
		}
		if (msg.head.nLen >0)
		{
			//解压缩数据
			if (!UnzipBuf(info,msg))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"数据解密解压缩失败");
				msg.head.stComm.cRaflag = 1;
				msg.head.stComm.cZip =0;
				msg.head.nLen = 0;
				msg.head.nRetCode = 18;
				//确定不加密
				SendData(&msg,info->s_nIndex);
				continue ;
			}
		}
		ProcessRcvMsg(info,msg);
	}
	//框架在此函数返回后再去读数据,除非这里置状态通知框架读
	
}

// 函数名: OnConnectBack
// 编程  : 王明松 2012-5-17 16:35:34
// 返回  : virtual void 
// 参数  : PSOCKET_POOL_DATA info
// 参数  : int status 连接状态  0成功 其他失败
// 描述  : AIO通知，发现主动连接成功，对此连接进行后续处理
void CAIO_Work::OnConnectBack(PSOCKET_POOL_DATA info,int status)
{
	//置连接状态，发送连接验证信息等等
	//这里什么都不用做，因为没有主动对外连接的
}

// 函数名: OnClose
// 编程  : 王明松 2012-5-17 16:34:32
// 返回  : virtual void 
// 参数  : PSOCKET_POOL_DATA info 连接信息
// 参数  : std::string msg
// 描述  : AIO通知，发现一个连接关闭，AIO并没有对此连接进行关闭处理，要求在这里对此连接进行后续处理，然后调用Close方法通知AIO关闭等
void CAIO_Work::OnClose(PSOCKET_POOL_DATA info,std::string msg)
{
	//清空发送队列是在AIO里面进行的
 	
	//将订阅实例销毁
	CSubScribeInfo *pp = (CSubScribeInfo *)info->ptr;
	if (pp != NULL)
	{
		delete pp;
		pp = NULL;
	}
	info->ptr = NULL;
	CloseClient(info);
	m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"关闭连接 %s",msg.c_str());
	
}

// 函数名: OnAccept
// 编程  : 王明松 2012-5-17 16:33:33
// 返回  : virtual void 
// 参数  : PSOCKET_POOL_DATA info
// 描述  : AIO通知，收到一条连接，对此新连接进行处理,AIO框架不放入连接池，由应用检查通过后再显示调用AddAio
void CAIO_Work::OnAccept(PSOCKET_POOL_DATA info)
{

	//检查连接是否合法
	CGATE_COMMSTRU data;
	bzero(&data,sizeof(CGATE_COMMSTRU));
	data.head.nRetCode = 0;
	
	data.head.stComm.cCmd = 0;
	data.head.stComm.cNextFlag = 0;
	data.head.stComm.cRaflag = 0;
	data.head.stComm.cZip =0;
	data.head.stComm.nCrc =0;
	PPKI_GATESTEP23 pki = (PPKI_GATESTEP23)(data.buffer);

	
	if (m_pRes->g_nUseCert == 1)
	{
		info->s_cCheckFlag = CHECKFLAG_STEP1;
		data.head.stComm.cMsgType = MSG_GATEPKISTEP1;
		CBF_Tools::StringCopy(info->s_sSignString,48,GenSignString().c_str());
		//通过签名字符串返回连接索引
		sprintf(data.buffer,"%16s %d",info->s_sSignString,info->s_nIndex);
		data.head.nLen = strlen(info->s_sSignString);
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"返回给客户端随机签名字符串");
		//确定不加密
		SendData(&data,info->s_nIndex);
	}
	else
	{
		info->s_cCheckFlag = CHECKFLAG_NORMAL;
		//随机生成会话密钥
		data.head.stComm.cMsgType = MSG_GATEPKISTEP3;
		bzero(info->s_sSessionKey,sizeof(info->s_sSessionKey));
		CBF_Tools::StringCopy(info->s_sSessionKey,48,GenSignString().c_str());
		//通过签名字符串返回连接索引
		sprintf(pki->sSignStr,"%16s %d",info->s_sSessionKey,info->s_nIndex);
		pki->nSignStrLen = strlen(pki->sSignStr);

		pki->nSignBufferLen =0;
		pki->nRsaPubKeyLen =0;
		pki->nSignPubKeyLen =0;
		pki->nKeySerialLen = 0;
		m_pEndian.Endian2Comm((unsigned char *)&(pki->nSignBufferLen),sizeof(pki->nSignBufferLen));
		m_pEndian.Endian2Comm((unsigned char *)&(pki->nSignStrLen),sizeof(pki->nSignStrLen));
		m_pEndian.Endian2Comm((unsigned char *)&(pki->nRsaPubKeyLen),sizeof(pki->nRsaPubKeyLen));
		m_pEndian.Endian2Comm((unsigned char *)&(pki->nSignPubKeyLen),sizeof(pki->nSignPubKeyLen));
		m_pEndian.Endian2Comm((unsigned char *)&(pki->nKeySerialLen),sizeof(pki->nKeySerialLen));
		data.head.nLen = sizeof(PKI_GATESTEP23);
// 		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"返回给客户端会话密钥");
// 		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"会话密钥[%s]",info->s_sSessionKey);
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"ip[%s] index[%d] 会话密钥[%s]",\
				inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,info->s_sSessionKey);
		//确定不加密
		SendData(&data,info->s_nIndex);
	}

	//置SOCKET状态等等，是只读还是只写还是读写,主要是SELECT模型用 
	info->s_cSocketStatus = SOCKSTATUS_R;
	info->s_cSocketType = SOCK_CLIENT;

	//分配一个订阅实例
	info->ptr = new CSubScribeInfo();
	if (info->ptr == NULL)
	{
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"new CSubScribeInfo出错");
	}
	else
	{
		((CSubScribeInfo *)(info->ptr))->m_pLog = m_pLog;
	}
	AddAio(info);
}
void CAIO_Work::Endian2CommCrc(CGATE_HEAD &head)
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
	if (m_pRes->g_nCrcFlag == 1)
	{
		head.stComm.nCrc = m_pEndian.CRC(&head,sizeof(CGATE_HEAD));
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"CRC值[%d]",head.stComm.nCrc);
		m_pEndian.Endian2Comm((unsigned char *)&(head.stComm.nCrc),sizeof(head.stComm.nCrc));
	}
}
bool CAIO_Work::Endian2HostCrc(CGATE_HEAD &head)
{
	m_pEndian.Endian2LocalHost((unsigned char *)&(head.stComm.nCrc),sizeof(head.stComm.nCrc));
	if (m_pRes->g_nCrcFlag == 1)
	{
		unsigned short crc1 = head.stComm.nCrc;
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"CRC值[%d]",crc1);
		head.stComm.nCrc = 0;
		unsigned short crc2 = m_pEndian.CRC(&head,sizeof(CGATE_HEAD));
		if (crc1 != crc2)
		{
			return false;
		}
	}
	
	m_pEndian.Endian2LocalHost((unsigned char *)&(head.nLen),sizeof(head.nLen));
	m_pEndian.Endian2LocalHost((unsigned char *)&(head.nRetCode),sizeof(head.nRetCode));
	m_pEndian.Endian2LocalHost((unsigned char *)&(head.stDest.nNodeId),sizeof(head.stDest.nNodeId));
	m_pEndian.Endian2LocalHost((unsigned char *)&(head.stDest.nSerial),sizeof(head.stDest.nSerial));
	m_pEndian.Endian2LocalHost((unsigned char *)&(head.stDest.nServiceNo),sizeof(head.stDest.nServiceNo));
	m_pEndian.Endian2LocalHost((unsigned char *)&(head.stDest.nSvrMainId),sizeof(head.stDest.nSvrMainId));
	m_pEndian.Endian2LocalHost((unsigned char *)&(head.stNext.n_nNextNo),sizeof(head.stNext.n_nNextNo));
	m_pEndian.Endian2LocalHost((unsigned char *)&(head.stNext.n_nNextOffset),sizeof(head.stNext.n_nNextOffset));
	
	return true;
}

bool CAIO_Work::UnzipBuf(PSOCKET_POOL_DATA info,CGATE_COMMSTRU &data)
{
	if (data.head.nLen == 0 || data.head.stComm.cZip == 0)
	{
		return true;
	}
	m_pGateLog->LogCgate(LOG_DEBUG,&data,info->s_nIndex,info->s_sSessionKey);

// #ifdef LINUX
// 	jmp_buf sJmp;
// 	if(!setjmp(sJmp))
// 	{
// 		char* s = (char *)alloca(4096);
// 		memset(s, 0, 4096);
// 		longjmp(sJmp, 1);
// 	}
// #endif
	
	

	unsigned int ziplen = CLIENTDATASIZE;
	int deslen;
	unsigned short shortlen;
	int desnum=0;
	bool issuccess=false;
	CBF_PMutex pp(&m_pDesMutex);
	switch (data.head.stComm.cZip)
	{
		case FLAG_ZIP:
			break;
		case FLAG_ZIPSESSIONKEY:
			while(desnum <3)
			{
				memcpy(m_sSrcBuffer,data.buffer,data.head.nLen);
				deslen = data.head.nLen;
				if (m_pDes.UncryptString(m_sSrcBuffer,deslen,info->s_sSessionKey) <= 0)
				{
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解密出错 [%s] %d",info->s_sSessionKey,deslen);
					return false;
				}
				
				if (m_pRes->g_nUnCommpress == 1)
				{
					if (!CBF_Tools::Uncompress((unsigned char *)m_sDestBuffer,ziplen,(unsigned char *)m_sSrcBuffer,deslen))
					{
						m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解压缩出错 可能是解密的数据已经不对 次数[%d] [%s]",\
							desnum,info->s_sSessionKey);
						desnum++;
						continue;//再次尝试解密
					}
					issuccess = true;
					data.head.nLen = ziplen;
					data.head.stComm.cZip = 0;
					memcpy(data.buffer,m_sDestBuffer,ziplen);
					m_pGateLog->LogCgate(LOG_DEBUG,&data,info->s_nIndex,info->s_sSessionKey);
					m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"解压缩成功 次数[%d] [%s]",\
							desnum,info->s_sSessionKey);
					break;
				}
				else
				{
					issuccess = true;
					data.head.nLen = deslen;
					data.head.stComm.cZip = FLAG_ZIP;
					memcpy(data.buffer,m_sSrcBuffer,deslen);
					m_pGateLog->LogCgate(LOG_DEBUG,&data,info->s_nIndex,info->s_sSessionKey);
					break;
				}
			}
			if (!issuccess)
			{
				return false;
			}
			break;
		case FLAG_SESSIONKEY:
			deslen = data.head.nLen;
			if (m_pDes.UncryptString(data.buffer,deslen,info->s_sSessionKey) <= 0)
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解密出错 [%s] %d",info->s_sSessionKey,deslen);
				return false;
			}
			data.head.nLen = deslen;
			data.head.stComm.cZip = 0;
			break;
		case FLAG_ZIPFIXEDKEY:
			memcpy(m_sSrcBuffer,data.buffer,data.head.nLen);
			deslen = data.head.nLen; 
			if (m_pDes.UncryptString(m_sSrcBuffer,deslen,DESENCRYPTKEY3) <=0 )
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解密出错 [%s]",info->s_sSessionKey);
				return false;
			}
			data.head.nLen = deslen;
			data.head.stComm.cZip = 2;
			m_pGateLog->LogCgate(LOG_DEBUG,&data,info->s_nIndex,info->s_sSessionKey);

			ziplen = CLIENTDATASIZE;
			if (!CBF_Tools::Uncompress((unsigned char *)m_sDestBuffer,ziplen,(unsigned char *)m_sSrcBuffer,deslen))
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解压缩出错 可能是解密的数据已经不对 [%s]",\
					info->s_sSessionKey);
				return false;
			}
			data.head.nLen = ziplen;
			data.head.stComm.cZip = 0;
			memcpy(data.buffer,m_sDestBuffer,ziplen);
			break;
		case FLAG_FIXEDKEY:
			
			deslen = data.head.nLen; 
			if (m_pDes.UncryptString(data.buffer,deslen,DESENCRYPTKEY3) <=0 )
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解密出错 [%s]",info->s_sSessionKey);
				return false;
			}
			data.head.nLen = deslen;
			data.head.stComm.cZip = 0;
			break;
		case FLAG_ZIPCERTKEY:
			memcpy(m_sSrcBuffer,data.buffer,data.head.nLen);
			deslen = data.head.nLen; 
			if (m_pRes->g_nUseCert == 1)
			{
				shortlen = data.head.nLen;
				if (!m_pRes->g_pCertClass->UnEncrypt(m_sSrcBuffer,shortlen))
				{
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解密出错");
					return false;
				}
				deslen = shortlen;
				data.head.stComm.cZip = FLAG_ZIP;
			}
			ziplen = CLIENTDATASIZE;
			if (!CBF_Tools::Uncompress((unsigned char *)m_sDestBuffer,ziplen,(unsigned char *)m_sSrcBuffer,deslen))
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解压缩出错 可能是解密的数据已经不对 [%s]",\
					info->s_sSessionKey);
				return false;
			}
			data.head.nLen = ziplen;
			data.head.stComm.cZip = 0;
			memcpy(data.buffer,m_sDestBuffer,ziplen);
			break;
		case FLAG_CERTKEY:
			if (m_pRes->g_nUseCert == 1)
			{
				shortlen = data.head.nLen;
				if (!m_pRes->g_pCertClass->UnEncrypt(data.buffer,shortlen))
				{
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解密出错");
					return false;
				}
				data.head.nLen = shortlen;
				data.head.stComm.cZip = 0;
			}
			break;
		default:
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"未知的标志类型[%d]",data.head.stComm.cZip);
			return false;
	}
	return true;
}
bool CAIO_Work::ZipBuf(PSOCKET_POOL_DATA info,PCGATE_COMMSTRU  data,int nZipFlag)
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

std::string CAIO_Work::GetMsgType(int type)
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
	case MSG_GATESUBSCRIBE:
		return "MSG_GATESUBSCRIBE";
	default:
		return "";
	}
}
std::string CAIO_Work::GetDrebCmdType(int cmdtype)
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

int CAIO_Work::SendData(PCGATE_COMMSTRU data,int index,bool isimmediate,int timestamp)
{
	S_CGATE_SMSG  msg;
	msg.index = index;
	msg.timestamp = timestamp;
	msg.d_nNodeId = 0;
	msg.d_cNodePrivateId = 0;
	msg.isBC = 0;
	msg.rtime = time(NULL);
	msg.s_nDrebSerial = 0;
	msg.txcode = 0;
	msg.s_nDrebSerial = 0;
	memcpy(&(msg.data),data,sizeof(CGATE_COMMSTRU));
	m_pSendData->PushData(msg,0);
	return 0;

}
int CAIO_Work::SendData(PCGATE_COMMSTRU data,int index)
{
	PSOCKET_POOL_DATA info = GetPoolData(index);
	if (info == NULL)
	{
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"连接index[%d]非法",index);
		return -1;
	}
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"认证 开始发送数据 index[%d] 标识[%d] cMsgType[%s] cCmd[%s] cRaFlag[%d] cNextFlag[%d] Node[%d %d] svr[%d %d] 交易码[%d] next[%d %d] czip[%d] nLen[%d]",\
		index,data->head.stDest.nSerial,GetMsgType(data->head.stComm.cMsgType).c_str(),\
		GetDrebCmdType(data->head.stComm.cCmd).c_str(),data->head.stComm.cRaflag,\
		data->head.stComm.cNextFlag,data->head.stDest.nNodeId,data->head.stDest.cNodePrivateId,\
		data->head.stDest.nSvrMainId,data->head.stDest.cSvrPrivateId,data->head.stDest.nServiceNo,\
		data->head.stNext.n_nNextNo,data->head.stNext.n_nNextOffset,data->head.stComm.cZip,data->head.nLen);
	if (info->s_hSocket == INVALID_SOCKET)
	{
		m_pLog->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"连接index[%d]已关闭",index);
		return 0;
	}
	//针对不同的消息进行加密压缩
	switch (data->head.stComm.cMsgType)
	{
	case MSG_GATEPING: //数据不用加密压缩
		break;
	case MSG_GATEPKISTEP1://压缩+固定密钥
		if (!ZipBuf(info,data,FLAG_ZIPFIXEDKEY))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"压缩+固定密钥加密出错");
			return -1;
		}
		break;
	case MSG_GATEPKISTEP2://压缩+固定密钥
		if (!ZipBuf(info,data,FLAG_ZIPFIXEDKEY))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"压缩+固定密钥加密出错");
			return -1;
		}
		break;
	case MSG_GATEPKISTEP3://压缩+加密公钥
		if (!ZipBuf(info,data,FLAG_ZIPCERTKEY))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"压缩+加密公钥加密出错");
			return -1;
		}
		break;
	case MSG_GATESMS:
	case MSG_GATELOGIN:   //压缩+会话密钥DES
	case MSG_GATEREQ:
	case MSG_PUSH:
	case MSG_GATESUBSCRIBE:
		if (!ZipBuf(info,data,FLAG_ZIPSESSIONKEY))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"压缩+会话密钥DES加密出错");
			return -1;
		}
		break;
	default:
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"消息类型[%s]不用在此发送",GetMsgType(data->head.stComm.cMsgType).c_str());
		return -1;
	}
	SOCKET_SEND_DATA dd;
	dd.s_nIndex = info->s_nIndex;
	dd.s_nSendLen = 0;
	dd.s_nRet = 0;
	dd.s_nSendNum = 0;
	dd.s_nTimestamp = info->s_nTimestamp;
	dd.s_nTotalLen = CGATEHEADLEN+data->head.nLen;
	
	dd.s_nHook = data->head.stDest.nSerial;
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"认证 发送长度[%d] 数据长度[%d] 报文头长度[%d] cZip[%d] index[%d] 标识[%d]",\
		dd.s_nTotalLen,data->head.nLen,CGATEHEADLEN,data->head.stComm.cZip,info->s_nIndex,data->head.stDest.nSerial);
	//字节序转换
	CGATE_HEAD cgatehead;
	memcpy(&(cgatehead),&(data->head),CGATEHEADLEN);
	Endian2CommCrc(cgatehead);
	memcpy(dd.s_sSendBuffer,&(cgatehead),CGATEHEADLEN);
// 	Endian2CommCrc(data->head);
// 	memcpy(dd.s_sSendBuffer,&(data->head),CGATEHEADLEN);

	if (dd.s_nTotalLen > DREBBUFSIZE)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"认证 发送数据长度[%d]不符 ",dd.s_nTotalLen);
	}
	else
	{
		memcpy(dd.s_sSendBuffer+CGATEHEADLEN,data->buffer,data->head.nLen);
	}
	

	if (info->s_hSocket == INVALID_SOCKET)
	{
		m_pLog->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"认证 发送失败 连接index[%d]已关闭 标识[%d]",index,data->head.stDest.nSerial);
		return 0;
	}
	//发送数据
	if (!Send2Client(dd,true))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"认证 发送失败 index[%d] 标识[%d]",index,data->head.stDest.nSerial);
		return 0;
	}
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"认证 发送成功 index[%d] 标识[%d]",index,data->head.stDest.nSerial);
	return 1;
}
std::string CAIO_Work::GenSignString()
{
	char tmpchar[49];
	bzero(tmpchar,sizeof(tmpchar));
	srand((unsigned int)CBF_Date_Time::GetTickCount());
	sprintf(tmpchar,"%05d%04u%07u",rand()%100000,CBF_Date_Time::GetTickCount()%10000,(int)CBF_Date_Time::GetTickUS()%10000000);
	return tmpchar;
}

void CAIO_Work::ProcessRcvMsg(PSOCKET_POOL_DATA info, CGATE_COMMSTRU &data)
{
	// #define  MSG_GATEPING      1          //和网关的PING消息
	// #define  MSG_GATEREQ       2          //发给网关的请求数据
	// #define  MSG_GATELOGIN     3          //网关登录
	// #define  MSG_GATESMS       4          //网关验证
	// #define  MSG_PUSH          5          //api收到的推送数据流，公共流(行情、公告)，私有流(回报等)
	// #define  MSG_GATEPKISTEP1  6          //网关PKI认证第一步 网关返回签名串
	// #define  MSG_GATEPKISTEP2  7          //网关PKI认证第二步 API发给网关签名及证书公钥、签名字符串
	// #define  MSG_GATEPKISTEP3  8          //网关PKI认证第三步 网关发给API签名及证书公钥、会话密钥

	// #define  CHECKFLAG_UNKNOW   0   //连接未知
	// #define  CHECKFLAG_STEP1    1    //接接客户端发送签名及公钥
	// #define  CHECKFLAG_STEP2    2    //接收客户端验证及登录
	// #define  CHECKFLAG_NORMAL   5    //正常连接，登录后置为正常连接
	if (info->s_cCheckFlag == CHECKFLAG_STEP1)
	{
		if (data.head.stComm.cMsgType != MSG_GATEPKISTEP2)
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"消息非法，应为消息认证第二步 [%s]",\
				GetMsgType(data.head.stComm.cMsgType).c_str());
			return;
		}
	}

	if (info->s_cCheckFlag == CHECKFLAG_STEP2)
	{
		if (data.head.stComm.cMsgType != MSG_GATELOGIN && data.head.stComm.cMsgType != MSG_GATESMS  )
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"消息非法，应为登录或验证 [%s]",\
				GetMsgType(data.head.stComm.cMsgType).c_str());
			data.head.nRetCode = 109;
			data.head.nLen = 0;
			//确定不加密
			SendData(&data,info->s_nIndex);
			OnClose(info,"消息非法");//关闭连接
			return;
		}
	}
	

	S_BPC_RSMSG drebdata;
	drebdata.sMsgBuf = NULL;

	PKI_GATESTEP23 *pkidata=NULL;
	char keyserial[40];
	unsigned short  keyseriallen;
	unsigned short nSignBufferLen;
	unsigned short nSignPubKeyLen;
	unsigned short nRsaPubKeyLen;

	CSubScribeInfo *psub = NULL;
	char errmsg[255];
	bzero(errmsg,sizeof(errmsg));
	switch (data.head.stComm.cMsgType)
	{
		case MSG_GATEPING:   //心跳消息
			data.head.stComm.cRaflag = 1;
			data.head.nLen = 0;
			data.head.stComm.cZip = 0;
			//确定不加密
			SendData(&data,info->s_nIndex,false,0);
			break;
		case MSG_GATEPKISTEP2:
			pkidata = (PKI_GATESTEP23 *)data.buffer;
			m_pEndian.Endian2LocalHost((unsigned char *)&(pkidata->nSignBufferLen),sizeof(pkidata->nSignBufferLen));
			m_pEndian.Endian2LocalHost((unsigned char *)&(pkidata->nSignStrLen),sizeof(pkidata->nSignStrLen));
			m_pEndian.Endian2LocalHost((unsigned char *)&(pkidata->nRsaPubKeyLen),sizeof(pkidata->nRsaPubKeyLen));
			m_pEndian.Endian2LocalHost((unsigned char *)&(pkidata->nSignPubKeyLen),sizeof(pkidata->nSignPubKeyLen));
			m_pEndian.Endian2LocalHost((unsigned char *)&(pkidata->nKeySerialLen),sizeof(pkidata->nKeySerialLen));
			
			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"来自[%s] index[%d] nSignBufferLen:%d nSignPubKeyLen:%d",inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,pkidata->nSignBufferLen,pkidata->nSignPubKeyLen);
			
			if (pkidata->nSignPubKeyLen >= sizeof(pkidata->sSignPubKey) || pkidata->nRsaPubKeyLen >= sizeof(pkidata->sRsaPubKey))
			{

				m_pLog->LogBin(LOG_ERROR,__FILE__,__LINE__,"客户端公钥",pkidata->sSignPubKey,pkidata->nSignPubKeyLen);
				m_pLog->LogBin(LOG_ERROR,__FILE__,__LINE__,"客户端签名",pkidata->sSignBuffer,pkidata->nSignBufferLen);
				m_pLog->LogBin(LOG_ERROR,__FILE__,__LINE__,"签名字符串",info->s_sSignString,strlen(info->s_sSignString));
				data.head.nRetCode = 100;
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"验证签名失败 来自[%s] index[%d] ",inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex);
				data.head.nLen =0;
				data.head.stComm.cZip = 0;
				//确定不加密
				SendData(&data,info->s_nIndex);
				OnClose(info,"验证签名失败");//关闭连接
				return ;	
			}
			pkidata->sSignPubKey[pkidata->nSignPubKeyLen]=0;
			
			if (!m_pRes->g_pCertClass->CheckSign(pkidata->sSignPubKey,pkidata->nSignPubKeyLen,\
				info->s_sSignString,(unsigned short)strlen(info->s_sSignString),pkidata->sSignBuffer,pkidata->nSignBufferLen))
			{
				m_pLog->LogBin(LOG_ERROR,__FILE__,__LINE__,"客户端公钥",pkidata->sSignPubKey,pkidata->nSignPubKeyLen);
				m_pLog->LogBin(LOG_ERROR,__FILE__,__LINE__,"客户端签名",pkidata->sSignBuffer,pkidata->nSignBufferLen);
				m_pLog->LogBin(LOG_ERROR,__FILE__,__LINE__,"签名字符串",info->s_sSignString,strlen(info->s_sSignString));
				data.head.nRetCode = 100;
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"验证签名失败 %s",m_pRes->g_pCertClass->GetErrMsg().c_str());
				data.head.nLen =0;
				data.head.stComm.cZip = 0;
				//确定不加密
				SendData(&data,info->s_nIndex);
				OnClose(info,"验证签名失败");//关闭连接
				return ;
			}
			//取对端证书序列号
			bzero(keyserial,sizeof(keyserial));
			keyseriallen = sizeof(keyserial);
			if (!m_pRes->g_pCertClass->GetCertSerial(pkidata->sSignPubKey,pkidata->nSignPubKeyLen,keyserial,keyseriallen))
			{
				data.head.nRetCode = 103;
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"取对端证书序列号失败 %s",m_pRes->g_pCertClass->GetErrMsg().c_str());
				data.head.nLen =0;
				data.head.stComm.cZip = 0;
				//确定不加密
				SendData(&data,info->s_nIndex);
				OnClose(info,"取对端证书序列号失败");//关闭连接
				return ;
			}
			//若对端送过来的证书序列号和公钥里的不一致，报错
// 			if (strncmp(keyserial,pkidata->sKeySerial,keyseriallen) != 0)
// 			{
// 				data.head.nRetCode = 104;
// 				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"证书序列号不符");
// 				SendData(&data,info->s_nIndex,true);
// 				CloseClient(info);//关闭连接
// 				return ;
// 			}

			//验证签名成功 保存公钥
			memcpy(info->s_sSignPubKey,pkidata->sSignPubKey,pkidata->nSignPubKeyLen);
			info->s_nSignPubKeyLen = pkidata->nSignPubKeyLen;
			info->s_sSignPubKey[info->s_nSignPubKeyLen] =0;

			memcpy(info->s_sEncryptPubKey,pkidata->sSignPubKey,pkidata->nSignPubKeyLen);
			info->s_nEncryptPubKeyLen = pkidata->nSignPubKeyLen;
			info->s_sEncryptPubKey[info->s_nEncryptPubKeyLen] =0;
			
			//本地进行签名
			pkidata->sSignStr[pkidata->nSignStrLen]=0;
			nSignBufferLen = 500;
			if (!m_pRes->g_pCertClass->Sign(pkidata->sSignStr,pkidata->nSignStrLen,pkidata->sSignBuffer,nSignBufferLen))
			{
				data.head.nRetCode = 101;
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"签名失败 %s",m_pRes->g_pCertClass->GetErrMsg().c_str());
				data.head.nLen =0;
				data.head.stComm.cZip = 0;
				//确定不加密
				SendData(&data,info->s_nIndex);
				OnClose(info,"签名失败");//关闭连接
				return ;
			}
			pkidata->nSignBufferLen = nSignBufferLen ;
			
			//取本地公钥返回
			nRsaPubKeyLen = sizeof(pkidata->sRsaPubKey);
			nSignPubKeyLen = sizeof(pkidata->sSignPubKey);
			if (!m_pRes->g_pCertClass->GetPubicKey(pkidata->sSignPubKey,nSignPubKeyLen,pkidata->sRsaPubKey,nRsaPubKeyLen))
			{
				data.head.nRetCode = 102;
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"取本地公钥失败 %s",m_pRes->g_pCertClass->GetErrMsg().c_str());
				data.head.nLen =0;
				data.head.stComm.cZip = 0;
				//确定不加密
				SendData(&data,info->s_nIndex);
				OnClose(info,"取本地公钥失败");//关闭连接
				return ;
			}
			pkidata->nRsaPubKeyLen = nRsaPubKeyLen;
			pkidata->nSignPubKeyLen = nSignPubKeyLen;
			

			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"服务端发给[%s] index[%d]的公钥 nSignPubKeyLen:%d nRsaPubKeyLen:%d",\
				inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,pkidata->nSignPubKeyLen,pkidata->nRsaPubKeyLen);
			//随机生成会话密钥
			data.head.stComm.cMsgType = MSG_GATEPKISTEP3;
			data.head.nRetCode = 0;
			bzero(info->s_sSessionKey,sizeof(info->s_sSessionKey));
			CBF_Tools::StringCopy(info->s_sSessionKey,48,GenSignString().c_str());
			strcpy(pkidata->sSignStr,info->s_sSessionKey);
			pkidata->nSignStrLen = strlen(pkidata->sSignStr);

			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"ip[%s] index[%d] 会话密钥[%s]",\
				inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,info->s_sSessionKey);
	
			//取本地证书序列号返回
			if (!m_pRes->g_pCertClass->GetCertSerial(pkidata->sSignPubKey,pkidata->nSignPubKeyLen,pkidata->sKeySerial,keyseriallen))
			{
				data.head.nRetCode = 106;
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"取本地证书序列号失败 %s",m_pRes->g_pCertClass->GetErrMsg().c_str());
				data.head.nLen =0;
				data.head.stComm.cZip = 0;
				//确定不加密
				SendData(&data,info->s_nIndex);
				OnClose(info,"取本地证书序列号失败");//关闭连接
				return ;
			}
			pkidata->nKeySerialLen = keyseriallen;
			data.head.nLen = sizeof(PKI_GATESTEP23);
			m_pEndian.Endian2Comm((unsigned char *)&(pkidata->nSignBufferLen),sizeof(pkidata->nSignBufferLen));
			m_pEndian.Endian2Comm((unsigned char *)&(pkidata->nSignStrLen),sizeof(pkidata->nSignStrLen));
			m_pEndian.Endian2Comm((unsigned char *)&(pkidata->nRsaPubKeyLen),sizeof(pkidata->nRsaPubKeyLen));
			m_pEndian.Endian2Comm((unsigned char *)&(pkidata->nSignPubKeyLen),sizeof(pkidata->nSignPubKeyLen));
			m_pEndian.Endian2Comm((unsigned char *)&(pkidata->nKeySerialLen),sizeof(pkidata->nKeySerialLen));
			//将连接序号返回
			sprintf(pkidata->sIndex,"%d",info->s_nIndex);
			info->s_cCheckFlag = CHECKFLAG_STEP2;
			//返回网关本地公钥、证收序列号、签名结果
			//确定使用证书加密
			SendData(&data,info->s_nIndex);
			break;
		case MSG_GATESMS:
		case MSG_GATELOGIN:
		case MSG_GATEREQ:
			drebdata.index = info->s_nIndex;
			drebdata.nRtime = time(NULL);
			drebdata.sMsgBuf = (PBPCCOMMSTRU)m_pMemPool->PoolMalloc();
			if (drebdata.sMsgBuf == NULL)
			{
				data.head.nRetCode = 107;
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"分配缓冲区失败");
				if (info->s_cCheckFlag != CHECKFLAG_NORMAL) //未验证通过，暂不发送
				{
					data.head.nLen = 0;
					data.head.stComm.cZip =0;
					//确定不加密
					SendData(&data,info->s_nIndex);
				}
				else
				{
					//通过发送线程加密发送
					SendData(&data,info->s_nIndex,false,0);
				}
				
				return ;
			}
			bzero(drebdata.sMsgBuf,sizeof(PBPCCOMMSTRU));
			if (data.head.nLen>0 && data.head.nLen<DREBDATASIZE)
			{
				memcpy(drebdata.sMsgBuf->sBuffer,data.buffer,data.head.nLen);
			}
			else if  (data.head.nLen >= DREBDATASIZE)
			{
				data.head.nRetCode = 108;
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"客户端数据长度不符 [%d]",data.head.nLen);
				if (info->s_cCheckFlag != CHECKFLAG_NORMAL) //未验证通过，暂不发送
				{
					data.head.nLen = 0;
					data.head.stComm.cZip = 0;
					//确定不加密
					SendData(&data,info->s_nIndex);
				}
				else
				{
					data.head.nLen = 0;
					data.head.stComm.cZip = 0;
					//通过发送线程加密发送
					SendData(&data,info->s_nIndex,false,0);
				}
				m_pMemPool->PoolFree(drebdata.sMsgBuf);
				drebdata.sMsgBuf = NULL;
				return ;
			}
			bzero(&(drebdata.sMsgBuf->sDBHead), DREBHEADLEN);
			drebdata.nTotalLen = BPCHEADLEN+DREBHEADLEN + data.head.nLen;
			//报文头
			drebdata.sMsgBuf->sDBHead.nLen = data.head.nLen;
			drebdata.sMsgBuf->sDBHead.cCmd = data.head.stComm.cCmd;
			drebdata.sMsgBuf->sDBHead.cZip = data.head.stComm.cZip;
			drebdata.sMsgBuf->sDBHead.cDrebAffirm = 0;
			drebdata.sMsgBuf->sDBHead.cNextFlag = data.head.stComm.cNextFlag; //是否是取后续包
			drebdata.sMsgBuf->sDBHead.cRaflag = data.head.stComm.cRaflag;//请求应答标志
			
            //目的信息、服务号交易码等
			drebdata.sMsgBuf->sDBHead.d_Dinfo.d_nNodeId = data.head.stDest.nNodeId;
			drebdata.sMsgBuf->sDBHead.d_Dinfo.d_cNodePrivateId = data.head.stDest.cNodePrivateId;
            drebdata.sMsgBuf->sDBHead.d_Dinfo.d_nSvrMainId = data.head.stDest.nSvrMainId;
			drebdata.sMsgBuf->sDBHead.d_Dinfo.d_cSvrPrivateId = data.head.stDest.cSvrPrivateId;
			drebdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo = data.head.stDest.nServiceNo;
			
			//取后续包的信息
			drebdata.sMsgBuf->sDBHead.n_Ninfo.n_nNextNo = data.head.stNext.n_nNextNo;
			drebdata.sMsgBuf->sDBHead.n_Ninfo.n_nNextOffset = data.head.stNext.n_nNextOffset;
			//连接信息、客户端流水、连接时间戳
			drebdata.sMsgBuf->sDBHead.s_Sinfo.s_nGateIndex = info->s_nIndex;	//连接index
			drebdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial = data.head.stDest.nSerial;	//客户端流水
			drebdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook = info->s_nTimestamp;//时间戳

			
			//放入队列，由消息线程来处理
			m_pDataPool->PushData(drebdata);
			if (data.head.stComm.cMsgType == MSG_GATELOGIN) 
			{
				info->s_cCheckFlag = CHECKFLAG_NORMAL;
			}
			break;
		case MSG_GATESUBSCRIBE://行情或回报订阅
			if (NULL == info->ptr )
			{
				data.head.nRetCode = 110;
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"行情订阅时订阅类指针为NULL");
				if (info->s_cCheckFlag != CHECKFLAG_NORMAL) //未验证通过，暂不发送
				{
					data.head.nLen = 0;
					data.head.stComm.cZip = 0;
					data.head.stComm.cRaflag = 1;
					data.head.stComm.cMsgType = MSG_GATESUBSCRIBE;
					//确定不加密
					SendData(&data,info->s_nIndex);
				}
				else
				{
					data.head.nLen = 0;
					data.head.stComm.cZip = 0;
					data.head.stComm.cRaflag = 1;
					data.head.stComm.cMsgType = MSG_GATESUBSCRIBE;
					//通过发送线程加密发送
					SendData(&data,info->s_nIndex,false,0);
				}
				return ;
			}
			psub = (CSubScribeInfo *)info->ptr;
			if (psub == NULL)
			{
				return;
			}
			if (!psub->Subscribe(data.buffer,data.head.nLen,errmsg))
			{
				data.head.nRetCode = 111;
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"行情订阅失败 %s",errmsg);
				if (info->s_cCheckFlag != CHECKFLAG_NORMAL) //未验证通过，暂不发送
				{
					data.head.nLen = 0;
					data.head.stComm.cZip = 0;
					data.head.stComm.cRaflag = 1;
					data.head.stComm.cMsgType = MSG_GATESUBSCRIBE;
					//确定不加密
					SendData(&data,info->s_nIndex);
				}
				else
				{
					data.head.nLen = 0;
					data.head.stComm.cZip = 0;
					data.head.stComm.cRaflag = 1;
					data.head.stComm.cMsgType = MSG_GATESUBSCRIBE;
					//通过发送线程加密发送
					SendData(&data,info->s_nIndex,false,0);
				}
				return ;
			}
			data.head.nRetCode = 0;
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"行情订阅成功");
			if (info->s_cCheckFlag != CHECKFLAG_NORMAL) //未验证通过，暂不发送
			{
				data.head.nLen = 0;
				data.head.stComm.cZip = 0;
				data.head.stComm.cRaflag = 1;
				data.head.stComm.cMsgType = MSG_GATESUBSCRIBE;
				//确定不加密
				SendData(&data,info->s_nIndex);
			}
			else
			{
				data.head.nLen = 0;
				data.head.stComm.cZip = 0;
				data.head.stComm.cRaflag = 1;
				data.head.stComm.cMsgType = MSG_GATESUBSCRIBE;
				//通过发送线程加密发送
				SendData(&data,info->s_nIndex,false,0);
			}
			break;
		default:
			break;
		
	}
	return;
}

void CAIO_Work::SendBCData(CGATE_COMMSTRU &data)
{
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"开始发送广播数据 标识[%d] cMsgType[%s] cCmd[%s] cRaFlag[%d] cNextFlag[%d] Node[%d %d] svr[%d %d] 交易码[%d] next[%d %d]",\
		data.head.stDest.nSerial,GetMsgType(data.head.stComm.cMsgType).c_str(),\
		GetDrebCmdType(data.head.stComm.cCmd).c_str(),data.head.stComm.cRaflag,\
		data.head.stComm.cNextFlag,data.head.stDest.nNodeId,data.head.stDest.cNodePrivateId,\
		data.head.stDest.nSvrMainId,data.head.stDest.cSvrPrivateId,data.head.stDest.nServiceNo,\
		data.head.stNext.n_nNextNo,data.head.stNext.n_nNextOffset);

	CGATE_COMMSTRU senddata;
	bzero(&senddata,sizeof(CGATE_COMMSTRU));
	int total;
	int poolsize = GetConnectPoolInfo(total);
	PSOCKET_POOL_DATA info  = NULL;
	for (int i=1 ; i<poolsize ; i++) //0是网关的侦听
	{
		
		memcpy(&senddata,&data,sizeof(CGATE_COMMSTRU));	
		info =  GetPoolData(i);
		if (info == NULL)
		{
			continue;
		}
		if (info->s_cCheckFlag != CHECKFLAG_NORMAL) //未验证通过，暂不发送
		{
//			m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"连接index[%d]为验证通过，暂不发送数据",i);
			continue;
		}
		//通过发送线程加密发送
		SendData(&senddata,i,false,0);
	}
	return ;
}


PSOCKET_POOL_DATA CAIO_Work::GetSocketByIndex(int index)
{
	return m_pAioPool->GetDataByIndex(index);
}
