// ApiSocket.cpp: implementation of the CApiSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "ApiSocket.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define  DESENCRYPTKEY3   "wms13711songfree"


#define  FLAG_FIXEDKEY            94  //固定key加密
#define  FLAG_SESSIONKEY          84   //会话key加密
#define  FLAG_CERTKEY             95  //证书key加密

#define  FLAG_ZIPFIXEDKEY         14  //压缩并固定key加密
#define  FLAG_ZIPSESSIONKEY       4   //压缩并会话key加密
#define  FLAG_ZIPCERTKEY          15  //压缩并证书key加密
#define  FLAG_ZIP                 2   //压缩

CApiSocket::CApiSocket()
{

	bzero(m_sRcvBuffer,sizeof(m_sRcvBuffer));  //接收数据缓冲
	bzero(m_sSendBuffer,sizeof(m_sSendBuffer));
	m_nRcvBufferLen=0;//接收数据缓冲大小
	
	m_pLog  = NULL;
	
	bzero(m_sSessionKey,sizeof(m_sSessionKey));
	bzero(&m_head,sizeof(CGATE_HEAD));

	bzero(m_sGatePubSignKey,sizeof(m_sGatePubSignKey));
	bzero(m_sGatePubRsaKey,sizeof(m_sGatePubRsaKey));
	m_nGatePubSignKeyLen = 0;
	m_nGatePubRsaKeyLen = 0;
	m_nIndex = 0;

	m_nSocketStatus =0;
}

CApiSocket::~CApiSocket()
{
	m_pLog  = NULL;
	m_pRes = NULL;
	
	m_nSocketStatus = 0;
}

int CApiSocket::Send2Gate(PCGATE_COMMSTRU data)
{
	int nRet;
	S_CGATEAPI_SMSG  smsg;
	bzero(&smsg,sizeof(S_CGATEAPI_SMSG));
	smsg.offset = 0;
	memcpy(&(smsg.message),data,sizeof(CGATE_COMMSTRU));
	
// 	if (m_pRes->IsControlBytes())
// 	{
// 		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"流量控制 每秒字节数");
// //		m_pSendQueue.PutSendMsg(smsg);
// 		return 0;
// 	}
// 	if (m_pRes->IsControlTx())
// 	{
// 		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"流量控制 每秒交易数");
// //		m_pSendQueue.PutSendMsg(smsg);
// 		return 0;
// 	}
	if (data->head.stComm.cMsgType == MSG_GATEPING)
	{
		m_pLog->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"开始发送数据 index[%d] 标识[%d] cMsgType[%s] cCmd[%s] cRaFlag[%d] cNextFlag[%d] Node[%d %d] svr[%d %d] 交易码[%d] next[%d %d]",\
			m_nIndex,data->head.stDest.nSerial,GetMsgType(data->head.stComm.cMsgType).c_str(),\
			GetDrebCmdType(data->head.stComm.cCmd).c_str(),data->head.stComm.cRaflag,\
			data->head.stComm.cNextFlag,data->head.stDest.nNodeId,data->head.stDest.cNodePrivateId,\
			data->head.stDest.nSvrMainId,data->head.stDest.cSvrPrivateId,data->head.stDest.nServiceNo,\
		data->head.stNext.n_nNextNo,data->head.stNext.n_nNextOffset);
	}
	else
	{
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"开始请求数据 index[%d] 标识[%d] cMsgType[%s] cCmd[%s] cRaFlag[%d] cNextFlag[%d] Node[%d %d] svr[%d %d] 交易码[%d] next[%d %d]",\
			m_nIndex,data->head.stDest.nSerial,GetMsgType(data->head.stComm.cMsgType).c_str(),\
			GetDrebCmdType(data->head.stComm.cCmd).c_str(),data->head.stComm.cRaflag,\
			data->head.stComm.cNextFlag,data->head.stDest.nNodeId,data->head.stDest.cNodePrivateId,\
			data->head.stDest.nSvrMainId,data->head.stDest.cSvrPrivateId,data->head.stDest.nServiceNo,\
			data->head.stNext.n_nNextNo,data->head.stNext.n_nNextOffset);
	}
	smsg.message.head.stComm.cZip = 0;
	//针对不同的消息进行加密压缩
	switch (smsg.message.head.stComm.cMsgType)
	{
		case MSG_GATEPING: //数据不用加密压缩
			break;
		case MSG_GATEPKISTEP1://数据不用加密压缩
			break;
		case MSG_GATEPKISTEP2://压缩+固定密钥3DES
			ZipBuf(&(smsg.message),14);
			break;
		case MSG_GATEPKISTEP3://压缩+API公钥加密
			break;
		case MSG_GATESMS:
		case MSG_GATELOGIN:   //压缩+会话密钥DES
		case MSG_GATEREQ:
			ZipBuf(&(smsg.message),4);
			break;
		default:
			break;
	}
	smsg.offset=0;
	smsg.serial = smsg.message.head.stDest.nSerial;
	smsg.totallen = smsg.message.head.nLen+CGATEHEADLEN;
	smsg.sendlen = 0;
	//字节序转换
	Endian2CommCrc(smsg.message.head);
	//发送数据
	CBF_PMutex pp(&m_mutex);
	if (m_pSocketTcp.GetSocket() == INVALID_SOCKET)
	{
		m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"发送数据时连接已被关闭 index[%d] 标识[%d] cMsgType[%s] cCmd[%s] cRaFlag[%d] cNextFlag[%d] Node[%d %d] svr[%d %d] 交易码[%d] next[%d %d]",\
			m_nIndex,data->head.stDest.nSerial,GetMsgType(data->head.stComm.cMsgType).c_str(),\
			GetDrebCmdType(data->head.stComm.cCmd).c_str(),data->head.stComm.cRaflag,\
			data->head.stComm.cNextFlag,data->head.stDest.nNodeId,data->head.stDest.cNodePrivateId,\
			data->head.stDest.nSvrMainId,data->head.stDest.cSvrPrivateId,data->head.stDest.nServiceNo,\
			data->head.stNext.n_nNextNo,data->head.stNext.n_nNextOffset);
		return -1;
	}
	nRet = m_pSocketTcp.Send( (char *)&(smsg.message),smsg.totallen);
	if (nRet != smsg.totallen) //没有发送完成
	{
		if (nRet >0)
		{
			//发送出错
			smsg.offset+=nRet;
			smsg.sendlen +=nRet;
			m_nSendTime = time(NULL);
		}
		if (m_pSocketTcp.CheckWriteEvent(1000) == 1)
		{
			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"socket可写再次发送,要发送数据长度[%d]",smsg.totallen - smsg.sendlen);
			nRet = m_pSocketTcp.Send( ((char *)&(smsg.message))+smsg.sendlen,smsg.totallen - smsg.sendlen);
			if (nRet == (smsg.totallen - smsg.sendlen) )
			{
				m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"再次发送成功,发送数据长度[%d]",nRet);
				m_pRes->g_pApiFlowInfo.ReportStat(smsg.totallen);
				return 2;
			}
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"可写但发送数据失败，返回值[%d] index[%d] 标识[%d] 关闭连接 [%d %s]",\
				nRet,m_nIndex,smsg.serial,m_pSocketTcp.GetErrorNo(),m_pSocketTcp.GetErrorMsg().c_str());
			OnClose(__FILE__,__LINE__,"可写但发送数据失败,关闭连接",LOG_ERROR_GENERAL);
			return 1;
		}
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"在1秒内连接仍不可写，index[%d] 标识[%d] 关闭连接 [%d %s]",\
			m_nIndex,smsg.serial,m_pSocketTcp.GetErrorNo(),m_pSocketTcp.GetErrorMsg().c_str());
		OnClose(__FILE__,__LINE__,"在1秒内连接仍不可写,关闭连接",LOG_ERROR_GENERAL);
		return 1;
	}
	else //发送成功
	{
		smsg.offset+=nRet;
		smsg.sendlen=0;
		m_pRes->g_pApiFlowInfo.ReportStat(smsg.totallen);
		if (smsg.serial !=0 )
		{
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"发送数据 index[%d] 标识[%d]成功 长度[%d]",\
				m_nIndex,smsg.serial,nRet);
		}
		else
		{
			m_pLog->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"发送数据 index[%d] 标识[%d]成功 长度[%d]",\
				m_nIndex,smsg.serial,nRet);
		}
		
		m_nSendTime = time(NULL);
		return 2;
	}
	return 2;
}
// 
// int CApiSocket::OnSend()
// {
// 	if (!m_pSendQueue.NeedSend())
// 	{
// 		return 0;
// 	}
// 	if (m_pRes->IsControlBytes())
// 	{
// 		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"流量控制 每秒字节数");
// 		return 0;
// 	}
// 	if (m_pRes->IsControlTx())
// 	{
// 		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"流量控制 每秒交易数");
// 		return 0;
// 	}
// 	CGATE_SQUEUE::iterator prq,prn;
// 	unsigned int atimeout;
// 	int nRet;
// 
// 	for (prn=m_pSendQueue.m_qSendData.datas.begin(); prn!=m_pSendQueue.m_qSendData.datas.end() ;)
// 	{
// 		prq = prn;
// 		prn++;
// 		if (prq->sendlen  ==  prq->totallen ) //全新数据，要判断超时
// 		{
// 			if (time(NULL) - prq->rtime> m_pRes->g_nTimeOutSend)
// 			{
// 				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"请求在队列超时 %d 秒",m_pRes->g_nTimeOutSend);
// 				m_pLog->LogBin(LOG_ERROR,__FILE__,__LINE__,"请求在队列超时",prq->message.buffer,prq->totallen);
// 				m_pSendQueue.DeleteSend(prq);
// 				continue;
// 			}
// 			if (prq->sendnum > m_pRes->g_nDataSendNum)
// 			{
// 				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"请求发送次数超限 %d",m_pRes->g_nDataSendNum);
// 				m_pLog->LogBin(LOG_ERROR,__FILE__,__LINE__,"请求发送次数超限",prq->message.buffer,prq->totallen);
// 				m_pSendQueue.DeleteSend(prq);
// 				continue;
// 			}
// 		}
// 		Send(prq);
// 	}
// 
// }

void CApiSocket::OnClose(const char *filename, int fileline, const char *msg,int loglevel)
{
	m_pSocketTcp.Close();
	m_pLog->LogMp(loglevel,filename,fileline,"关闭连接 %s",msg);
	bzero(m_sRcvBuffer,sizeof(m_sRcvBuffer));  //接收数据缓冲
	bzero(m_sSendBuffer,sizeof(m_sSendBuffer));
	m_nRcvBufferLen=0;//接收数据缓冲大小
	bzero(m_sSessionKey,sizeof(m_sSessionKey));
	bzero(&m_head,sizeof(CGATE_HEAD));
	
	bzero(m_sGatePubSignKey,sizeof(m_sGatePubSignKey));
	bzero(m_sGatePubRsaKey,sizeof(m_sGatePubRsaKey));
	m_nGatePubSignKeyLen = 0;
	m_nGatePubRsaKeyLen = 0;
	m_nIndex = 0;

	m_nSocketStatus = 0;//未连接
	m_pAnsQueue.Clear();
	m_pReqQueue.Clear();
}


void CApiSocket::SetPara(CGlobalResource *res)
{
	m_pRes = res;
	m_pLog = &(res->g_pLog);
	m_pGateLog = &(m_pRes->g_pCgateLog);
	if (m_pRes->g_nUseCert == 1)
	{
		m_pRes->g_pCertClass->GetPubicKey(m_sLocalPubSignKey,m_nLocalPubSignKeyLen,m_sLocalPubRsaKey,m_nLocalPubRsaKeyLen);
	}
}

bool CApiSocket::UnzipBuf(CGATE_COMMSTRU &data)
{
	if (data.head.nLen == 0)
	{
		return true;
	}
	char zipDataBuf[CLIENTDATASIZE];
	bzero(zipDataBuf,sizeof(zipDataBuf));
	unsigned int ziplen = CLIENTDATASIZE;
	int deslen;
	unsigned short rsalen;
	m_pGateLog->LogCgate(LOG_DEBUG+1,&data,m_nIndex,m_sSessionKey);
	switch (data.head.stComm.cZip)
	{
		case FLAG_ZIP:
			memcpy(zipDataBuf,data.buffer,data.head.nLen);
			ziplen = CLIENTDATASIZE;
			
			if (!Uncompress((unsigned char *)(data.buffer),ziplen,(unsigned char *)zipDataBuf,data.head.nLen))
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解压缩出错 index[%d] 标识[%d]",\
					m_nIndex,data.head.stDest.nSerial);
				m_pLog->LogBin(LOG_ERROR_FAULT,__FILE__,__LINE__,"解压缩出错",zipDataBuf,data.head.nLen);
				memcpy(data.buffer,zipDataBuf,data.head.nLen);
				m_pGateLog->LogCgate(LOG_ERROR_FAULT,&data,m_nIndex,"999999999");
				return false;
			}
			data.head.nLen = ziplen;
			data.head.stComm.cZip =0;
			break;
		case FLAG_ZIPSESSIONKEY:
			deslen = data.head.nLen;
			if (m_pDes.UncryptString(data.buffer,deslen,m_sSessionKey)<1)
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解密出错密钥[%d]",m_sSessionKey);
				return false;
			}
			data.head.nLen = deslen ;
			data.head.stComm.cZip =FLAG_ZIP;
			memcpy(zipDataBuf,data.buffer,data.head.nLen);
//			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"解密后数据长度[%d]",data.head.nLen);
			ziplen = CLIENTDATASIZE;
			m_pGateLog->LogCgate(LOG_DEBUG,&data,m_nIndex,m_sSessionKey);
			if (!Uncompress((unsigned char *)(data.buffer),ziplen,(unsigned char *)zipDataBuf,data.head.nLen))
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解压缩出错 index[%d] 标识[%d]",\
					m_nIndex,data.head.stDest.nSerial);
				m_pLog->LogBin(LOG_ERROR_FAULT,__FILE__,__LINE__,"解压缩出错",zipDataBuf,data.head.nLen);
				memcpy(data.buffer,zipDataBuf,data.head.nLen);
				m_pGateLog->LogCgate(LOG_ERROR_FAULT,&data,m_nIndex,"999999999");
				return false;
			}
			data.head.nLen = ziplen;
			data.head.stComm.cZip =0;
			break;
		case FLAG_SESSIONKEY:
			deslen = data.head.nLen;
			if (m_pDes.UncryptString(data.buffer,deslen,m_sSessionKey)<1)
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解密出错密钥[%d]",m_sSessionKey);
				return false;
			}
			data.head.nLen = deslen ;
			data.head.stComm.cZip =0;
			break;
		case FLAG_ZIPFIXEDKEY:
			deslen = data.head.nLen;
			if (m_pDes.UncryptString(data.buffer,deslen,DESENCRYPTKEY3)<1)
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"固定密钥DESENCRYPTKEY3解密出错");
				return false;
			}

			data.head.nLen = deslen;
			data.head.stComm.cZip =FLAG_ZIP;
			memcpy(zipDataBuf,data.buffer,data.head.nLen);
			ziplen = CLIENTDATASIZE;
			

			m_pGateLog->LogCgate(LOG_DEBUG,&data,m_nIndex,m_sSessionKey);
			if (!Uncompress((unsigned char *)(data.buffer),ziplen,(unsigned char *)zipDataBuf,data.head.nLen))
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解压缩出错 index[%d] 标识[%d]",\
					m_nIndex,data.head.stDest.nSerial);
				m_pLog->LogBin(LOG_ERROR_FAULT,__FILE__,__LINE__,"解压缩出错",zipDataBuf,data.head.nLen);
				memcpy(data.buffer,zipDataBuf,data.head.nLen);
				m_pGateLog->LogCgate(LOG_ERROR_FAULT,&data,m_nIndex,"999999999");
				return false;
			}
			data.head.nLen = ziplen;
			data.head.stComm.cZip =0;
			break;
		case FLAG_FIXEDKEY:
			deslen = data.head.nLen;
			if (m_pDes.UncryptString(data.buffer,deslen,DESENCRYPTKEY3)<1)
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"固定密钥DESENCRYPTKEY3解密出错");
				return false;
			}
			
			data.head.nLen = deslen;
			data.head.stComm.cZip =0;
			break;
		case FLAG_ZIPCERTKEY:
			if (m_pRes->g_nUseCert == 1)
			{
				rsalen = data.head.nLen;
				if (!m_pRes->g_pCertClass->UnEncrypt(data.buffer,rsalen))
				{
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解密出错");
					return false;
				}
				data.buffer[rsalen]=0;
				data.head.nLen = rsalen;
				data.head.stComm.cZip =FLAG_ZIP;
			}
			
			memcpy(zipDataBuf,data.buffer,data.head.nLen);
			ziplen = CLIENTDATASIZE;
			m_pGateLog->LogCgate(LOG_DEBUG,&data,m_nIndex,m_sSessionKey);
			if (!Uncompress((unsigned char *)(data.buffer),ziplen,(unsigned char *)zipDataBuf,data.head.nLen))
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解压缩出错 index[%d] 标识[%d]",\
					m_nIndex,data.head.stDest.nSerial);
				m_pLog->LogBin(LOG_ERROR_FAULT,__FILE__,__LINE__,"解压缩出错",zipDataBuf,data.head.nLen);
				return false;
			}
			data.head.nLen = ziplen;
			data.head.stComm.cZip =0;
			break;
		case FLAG_CERTKEY:
			if (m_pRes->g_nUseCert == 1)
			{
				rsalen = data.head.nLen;
				if (!m_pRes->g_pCertClass->UnEncrypt(data.buffer,rsalen))
				{
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"解密出错");
					return false;
				}
				data.buffer[rsalen]=0;
				data.head.nLen = rsalen;
				data.head.stComm.cZip = 0;
			}
			break;
		default:
			return true;
	}
	return true;
}
bool CApiSocket::ZipBuf(PCGATE_COMMSTRU  data,int nZipFlag)
{
	//0明文 1des加密密钥长度16 2压缩 3DES加密 4DES加密并压缩
	if (data->head.stComm.cZip != 0)
	{
		return true;
	}
	if (data->head.nLen == 0)
	{
		return true;
	}
	char zipDataBuf[CLIENTDATASIZE];
	bzero(zipDataBuf,sizeof(zipDataBuf));
	unsigned int ziplen = CLIENTDATASIZE;
	int deslen;
	unsigned short rsalen;
	m_pGateLog->LogCgate(LOG_DEBUG+1,data,m_nIndex,m_sSessionKey);
	switch (nZipFlag)
	{
		case FLAG_ZIPSESSIONKEY:
			if (m_pRes->g_nZipFlag == 1)
			{
				ziplen = CLIENTDATASIZE;
				memcpy(zipDataBuf,data->buffer,data->head.nLen);
				if (!Compress((unsigned char *)data->buffer,ziplen,(unsigned char *)(zipDataBuf),data->head.nLen))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"数据压缩失败");
					return false;
				}
				data->head.nLen = ziplen;
				//写data日志
				deslen = ziplen;
				data->head.stComm.cZip =FLAG_ZIP;
				m_pGateLog->LogCgate(LOG_DEBUG,data,m_nIndex,m_sSessionKey);
				if (m_pDes.EncryptString(data->buffer,deslen,m_sSessionKey)<1)
				{
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"加密出错");
					return false;
				}
				
				data->head.nLen = deslen;
				data->head.stComm.cZip =FLAG_ZIPSESSIONKEY;
			}
			else
			{
				deslen = data->head.nLen;
				if (m_pDes.EncryptString(data->buffer,deslen,m_sSessionKey)<1)
				{
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"加密出错");
					return false;
				}
				
				data->head.nLen = deslen;
				data->head.stComm.cZip =FLAG_SESSIONKEY;
			}
			break;
		case FLAG_ZIPFIXEDKEY:
			if (m_pRes->g_nZipFlag == 1)
			{
				memcpy(zipDataBuf,data->buffer,data->head.nLen);
				ziplen = CLIENTDATASIZE;
				if (!Compress((unsigned char *)data->buffer,ziplen,(unsigned char *)(zipDataBuf),data->head.nLen))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"数据压缩失败");
					return false;
				}
				data->head.nLen = ziplen;
				deslen = ziplen;
				data->head.stComm.cZip =FLAG_ZIP;
				m_pGateLog->LogCgate(LOG_DEBUG,data,m_nIndex,m_sSessionKey);
				if (m_pDes.EncryptString(data->buffer,deslen,DESENCRYPTKEY3)<1)
				{
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"加密出错");
					return false;
				}

				data->head.nLen = deslen;
				data->head.stComm.cZip =FLAG_ZIPFIXEDKEY;
			}
			else
			{
				deslen = data->head.nLen;
				if (m_pDes.EncryptString(data->buffer,deslen,DESENCRYPTKEY3)<1)
				{
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"加密出错");
					return false;
				}
				
				data->head.nLen = deslen;
				data->head.stComm.cZip =FLAG_FIXEDKEY;
			}
			break;
		case FLAG_ZIPCERTKEY:
			if (m_pRes->g_nZipFlag == 1)
			{
				memcpy(zipDataBuf,data->buffer,data->head.nLen);
				ziplen = CLIENTDATASIZE;
				if (!Compress((unsigned char *)data->buffer,ziplen,(unsigned char *)(zipDataBuf),data->head.nLen))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"数据压缩失败");
					return false;
				}
				data->head.nLen = ziplen;
				data->head.stComm.cZip =FLAG_ZIP;
				m_pGateLog->LogCgate(LOG_DEBUG,data,m_nIndex,m_sSessionKey);
				if (m_pRes->g_nUseCert == 1)
				{
					rsalen = data->head.nLen;
					if (!m_pRes->g_pCertClass->Encrypt(m_sLocalPubRsaKey,m_nLocalPubRsaKeyLen,data->buffer,rsalen ))
					{
						m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"加密出错");
						return false;
					}
					data->buffer[rsalen]=0;
					data->head.nLen = rsalen;
					data->head.stComm.cZip =FLAG_ZIPCERTKEY;
				}
			}
			else
			{
				if (m_pRes->g_nUseCert == 1)
				{
					rsalen = data->head.nLen;
					if (!m_pRes->g_pCertClass->Encrypt(m_sLocalPubRsaKey,m_nLocalPubRsaKeyLen,data->buffer,rsalen ))
					{
						m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"加密出错");
						return false;
					}
					data->buffer[rsalen]=0;
					data->head.nLen = rsalen;
					data->head.stComm.cZip = FLAG_CERTKEY;
				}
			}
			
			break;
		default:
			return true;
	}
	m_pGateLog->LogCgate(LOG_DEBUG,data,m_nIndex,m_sSessionKey);
	return true;
}

std::string CApiSocket::GetMsgType(int type)
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
std::string CApiSocket::GetDrebCmdType(int cmdtype)
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

void CApiSocket::Endian2CommCrc(CGATE_HEAD &head)
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
		//	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"CRC值[%d]",head.stComm.nCrc );
		m_pEndian.Endian2Comm((unsigned char *)&(head.stComm.nCrc),sizeof(head.stComm.nCrc));
	}
	
}
bool CApiSocket::Endian2HostCrc(CGATE_HEAD &head)
{
	m_pEndian.Endian2LocalHost((unsigned char *)&(head.stComm.nCrc),sizeof(head.stComm.nCrc));
	if (m_pRes->g_nCrcFlag == 1)
	{ 
		unsigned short crc1 = head.stComm.nCrc;
	//	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"CRC值[%d]",head.stComm.nCrc );
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

int CApiSocket::OnRcv()
{
	int ret;
	if (m_nRcvBufferLen >=0 && m_nRcvBufferLen<DREBBUFSIZE)
	{
		//包头未接收完
		ret = m_pSocketTcp.Receive(m_sRcvBuffer+m_nRcvBufferLen,DREBBUFSIZE-m_nRcvBufferLen);
		if (ret <= 0)
		{
			return ret;
		}
		m_nRcvTime =time(NULL);
		m_nPingTime = m_nRcvTime;
		m_nRcvBufferLen+=ret;
		//记录统计信息
		return ret;
	}
	return 0;
}
int CApiSocket::GetRecvData(CGATE_COMMSTRU *msg)
{
	//收到的数据大于包头
	if (m_nRcvBufferLen >= CGATEHEADLEN )
	{
		//进行字节序处理并校验CRC
		memcpy(&m_head,m_sRcvBuffer,CGATEHEADLEN);
		if (!Endian2HostCrc(m_head))
		{
			OnClose(__FILE__,__LINE__,"CRC错误",LOG_ERROR);
			return -1;
		}
		if (m_nRcvBufferLen < m_head.nLen + CGATEHEADLEN)
		{
			//说明数据没有接收完整
//			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"数据未接收完整 index[%d] 标识[%d] 数据长度[%d] nLen[%d]  msgtype[%d] raflag[%d]",\
//				m_nIndex,msg->head.stDest.nSerial,m_nRcvBufferLen,m_head.nLen ,m_head.stComm.cMsgType,m_head.stComm.cRaflag);
			return 0;
		}
		memcpy(&(msg->head),&m_head,CGATEHEADLEN);
		if (m_head.nLen>0)
		{
			memcpy(&(msg->buffer),m_sRcvBuffer+CGATEHEADLEN,m_head.nLen);
		}
		
		//记入接收日志
		if (msg->head.stComm.cMsgType != MSG_GATEPING)
		{
			if (msg->head.stComm.cMsgType == MSG_PUSH)
			{
				m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"接收到的数据 推送数据 index[%d] cMsgType[%s] cCmd[%s] cRaFlag[%d] cNextFlag[%d] Node[%d %d] svr[%d %d] 交易码[%d] next[%d %d] cZip[%d] nLen[%d]",\
					m_nIndex,GetMsgType(msg->head.stComm.cMsgType).c_str(),\
					GetDrebCmdType(msg->head.stComm.cCmd).c_str(),msg->head.stComm.cRaflag,\
					msg->head.stComm.cNextFlag,msg->head.stDest.nNodeId,msg->head.stDest.cNodePrivateId,\
					msg->head.stDest.nSvrMainId,msg->head.stDest.cSvrPrivateId,msg->head.stDest.nServiceNo,\
					msg->head.stNext.n_nNextNo,msg->head.stNext.n_nNextOffset,msg->head.stComm.cZip,msg->head.nLen);
			}
			else
			{
				m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"接收到的数据 index[%d] 标识[%d] cMsgType[%s] cCmd[%s] cRaFlag[%d] cNextFlag[%d] Node[%d %d] svr[%d %d] 交易码[%d] next[%d %d] cZip[%d] nLen[%d]",\
					m_nIndex,msg->head.stDest.nSerial,GetMsgType(msg->head.stComm.cMsgType).c_str(),\
					GetDrebCmdType(msg->head.stComm.cCmd).c_str(),msg->head.stComm.cRaflag,\
					msg->head.stComm.cNextFlag,msg->head.stDest.nNodeId,msg->head.stDest.cNodePrivateId,\
					msg->head.stDest.nSvrMainId,msg->head.stDest.cSvrPrivateId,msg->head.stDest.nServiceNo,\
					msg->head.stNext.n_nNextNo,msg->head.stNext.n_nNextOffset,msg->head.stComm.cZip,msg->head.nLen);
			}
			
		}
		else
		{
			m_pLog->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"接收到的数据 index[%d] 标识[%d] cMsgType[%s] cCmd[%s] cRaFlag[%d] cNextFlag[%d] Node[%d %d] svr[%d %d] 交易码[%d] next[%d %d] cZip[%d] nLen[%d]",\
				m_nIndex,msg->head.stDest.nSerial,GetMsgType(msg->head.stComm.cMsgType).c_str(),\
				GetDrebCmdType(msg->head.stComm.cCmd).c_str(),msg->head.stComm.cRaflag,\
				msg->head.stComm.cNextFlag,msg->head.stDest.nNodeId,msg->head.stDest.cNodePrivateId,\
				msg->head.stDest.nSvrMainId,msg->head.stDest.cSvrPrivateId,msg->head.stDest.nServiceNo,\
				msg->head.stNext.n_nNextNo,msg->head.stNext.n_nNextOffset,msg->head.stComm.cZip,msg->head.nLen);
		}
		

		//收多了，将原包重新给m_sRcvBuffer
		for (int i=0; i<m_nRcvBufferLen-CGATEHEADLEN-m_head.nLen; i++)
		{
			m_sRcvBuffer[i]=m_sRcvBuffer[i+CGATEHEADLEN+m_head.nLen];
		}
		m_nRcvBufferLen=m_nRcvBufferLen - CGATEHEADLEN-m_head.nLen;
		//解压缩数据
		if (!UnzipBuf(*msg))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"数据解压缩解密失败 index[%d] 标识[%d]",\
				m_nIndex,msg->head.stDest.nSerial);
			OnClose(__FILE__,__LINE__,"数据解压缩解密失败",LOG_ERROR);
			return -1;
// 			msg->head.nRetCode = 18;
// 			return m_head.nLen+CGATEHEADLEN;
		}
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"数据 nLen[%d] cZip[%d]",msg->head.nLen,msg->head.stComm.cZip);
		m_pLog->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"接收到的数据",msg->buffer,msg->head.nLen);
		return m_head.nLen+CGATEHEADLEN;
	}
	return 0;
}

bool CApiSocket::ConnectGate()
{
	m_nIndex = 0;
	bool bisConnected=false;
	for (unsigned int i=0 ; i<m_pRes->g_lGateList.size() ; i++)
	{
		m_pSocketTcp.Close();
		m_pSocketTcp.Create(AF_INET,false);
		if (m_pSocketTcp.ConnectServer(m_pRes->g_lGateList[i].sIp.c_str(),m_pRes->g_lGateList[i].nPort,m_pRes->g_nConnGateTimeOut*1000))
		{
			bisConnected = true;
			m_pSocketTcp.SetBlockFlag(false);
			m_pRes->g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"连接网关[%s %d]成功 %d",m_pRes->g_lGateList[i].sIp.c_str(),m_pRes->g_lGateList[i].nPort,m_pRes->g_nConnGateTimeOut*1000);
			break;
		}
		m_pRes->g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"连接网关[%s %d]失败 %d",m_pRes->g_lGateList[i].sIp.c_str(),m_pRes->g_lGateList[i].nPort,m_pRes->g_nConnGateTimeOut*1000);
	}
	if (!bisConnected)
	{
		OnClose(__FILE__,__LINE__,"连接所有的网关IP皆失败",LOG_ERROR);
		return false;
	}
	//PKI认证
	if (m_pRes->g_nUseCert == 1)
	{
		if (!Pki())
		{
			OnClose(__FILE__,__LINE__,"PKI安全认证失败",LOG_ERROR);
			return false;
		}
		m_pRes->g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,"PKI安全认证成功");
	}
	else
	{
		int nRet;
		CGATE_COMMSTRU data;
		bzero(&data,sizeof(CGATE_COMMSTRU));
		while (true)
		{
			nRet = m_pSocketTcp.CheckReadEvent(m_pRes->g_nConnGateTimeOut*1000);
			if (nRet <= 0)
			{
				OnClose(__FILE__,__LINE__,"连接网关 在指定时间未收到网关响应数据",LOG_ERROR);
				return false;
			}
			nRet = OnRcv();
			if (nRet < 0 )
			{
				OnClose(__FILE__,__LINE__,"连接网关 网关连接已关闭，接收数据失败",LOG_WARNNING);
				return false;
			}
			
			nRet = GetRecvData(&data);
			if (nRet == 0)
			{
				//继续收
				continue;
			}
			if (nRet <0)
			{
				OnClose(__FILE__,__LINE__,"连接网关 GetRecvData 失败",LOG_ERROR);
				return false;
			}
			break;
		}
		if (data.head.stComm.cMsgType != MSG_GATEPKISTEP3)
		{
			OnClose(__FILE__,__LINE__,"连接网关 网关发来的数据非MSG_GATEPKISTEP3",LOG_ERROR);
			return false;
		}
		PPKI_GATESTEP23 s2 = (PPKI_GATESTEP23)data.buffer;
		m_pEndian.Endian2LocalHost((unsigned char *)&(s2->nSignBufferLen),sizeof(s2->nSignBufferLen));
		m_pEndian.Endian2LocalHost((unsigned char *)&(s2->nSignStrLen),sizeof(s2->nSignStrLen));
		m_pEndian.Endian2LocalHost((unsigned char *)&(s2->nRsaPubKeyLen),sizeof(s2->nRsaPubKeyLen));
		m_pEndian.Endian2LocalHost((unsigned char *)&(s2->nSignPubKeyLen),sizeof(s2->nSignPubKeyLen));
		bzero(m_sSessionKey,sizeof(m_sSessionKey));
		//会话密码为16位长度
		memcpy(m_sSessionKey,s2->sSignStr,16);
		m_nIndex = atoi(s2->sSignStr+17);

		m_pRes->g_pLog.LogBin(LOG_PROMPT,__FILE__,__LINE__,"网关发来的连接序号",s2->sSignStr,sizeof(s2->sSignStr));
		if (m_nIndex <1)
		{
			OnClose(__FILE__,__LINE__,"连接网关 网关发来的连接序号非法",LOG_ERROR);
			return false;
		}
		m_pRes->g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,"无证书验证通过 index[%d]",m_nIndex);
	}
	m_pRes->g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,"连接成功 index[%d]，可以发登录请求了",m_nIndex);
	m_nSocketStatus = 1;
	return true;
}

SOCKET_HANDLE CApiSocket::GetSocket()
{
	return m_pSocketTcp.GetSocket();
}

bool CApiSocket::Pki()
{
	if (m_pRes->g_pCertClass == NULL)
	{
		m_pRes->g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"证书动态库实例类为NULL");
		return false;
	}
	int nRet;
	CGATE_COMMSTRU data;
	bzero(&data,sizeof(CGATE_COMMSTRU));
	while (true)
	{
		nRet = m_pSocketTcp.CheckReadEvent(m_pRes->g_nConnGateTimeOut*1000);
		if (nRet <= 0)
		{
			OnClose(__FILE__,__LINE__,"安全认证 在指定时间未收到网关响应数据",LOG_ERROR);
			return false;
		}
		nRet = OnRcv();
		if (nRet < 0 )
		{
			OnClose(__FILE__,__LINE__,"安全认证 网关连接已关闭，接收数据失败",LOG_ERROR);
			return false;
		}
		m_pRes->g_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"安全认证 接收到数据 %d",nRet);
		nRet = GetRecvData(&data);
		if (nRet == 0)
		{
			//数据没有收全，继续收
			continue;
		}
		if (nRet <0 )
		{
			OnClose(__FILE__,__LINE__,"安全认证 GetRecvData 失败",LOG_ERROR);
			return false;
		}
		break;
	}
	
	if (data.head.stComm.cMsgType != MSG_GATEPKISTEP1)
	{
		OnClose(__FILE__,__LINE__,"安全认证 网关发来的数据非MSG_GATEPKISTEP1",LOG_ERROR);
		return false;
	}
	//得到网关返回的字符串，要对此字符串进行签名并将公钥发给网关
	//签名
	char  signstr[4048];
	unsigned short   signstrlen= 16;//固定为16位长度
	bzero(signstr,sizeof(signstr));
	memcpy(signstr,data.buffer,16);
	//从第17个字符之后就此连接在网关里的连接序号
	m_nIndex = atoi(data.buffer+17);

	//清空数据，准备返回给网关签名及公钥
	bzero(&data,sizeof(CGATE_COMMSTRU));
	data.head.nRetCode = 0;
	data.head.stComm.cMsgType = MSG_GATEPKISTEP2;

	PPKI_GATESTEP23 pkidata = (PPKI_GATESTEP23)data.buffer;
	pkidata->nSignBufferLen = sizeof(pkidata->sSignBuffer);

	unsigned short clen;
	clen = pkidata->nSignBufferLen;
	//对网关发来的字符串进行签名
	if (!m_pRes->g_pCertClass->Sign(signstr,signstrlen,pkidata->sSignBuffer,clen))
	{
		OnClose(__FILE__,__LINE__,"安全认证 对数据签名失败",LOG_ERROR);
		m_pLog->LogBin(LOG_ERROR,__FILE__,__LINE__,"要签名的数据",signstr,signstrlen);
		return false;
	}
	pkidata->nSignBufferLen = clen;
	//生成给服务端的签名字符串 16位长度
	pkidata->nSignStrLen = sizeof(pkidata->sSignStr);
	memcpy(pkidata->sSignStr,GenSignString().c_str(),16);
	pkidata->nSignStrLen = 16;
	
	//保存发给网关的字符串，以签名验证
	bzero(signstr,sizeof(signstr));
	memcpy(signstr,pkidata->sSignStr,16);
	signstrlen = 16;

	//取公钥
	pkidata->nSignPubKeyLen = sizeof(pkidata->sSignPubKey);//签名公钥
	pkidata->nRsaPubKeyLen = sizeof(pkidata->sRsaPubKey);//加密公钥
	unsigned short dlen = pkidata->nSignPubKeyLen;
	clen = pkidata->nRsaPubKeyLen;
	if (!m_pRes->g_pCertClass->GetPubicKey(pkidata->sSignPubKey,dlen,pkidata->sRsaPubKey,clen))
	{
		OnClose(__FILE__,__LINE__,"安全认证 取本地公钥失败",LOG_ERROR);
		return false;
	}
	pkidata->nSignPubKeyLen = dlen;
	pkidata->nRsaPubKeyLen = clen;
	m_pRes->g_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"客户端公钥长度 %d %d ",pkidata->nSignPubKeyLen,pkidata->nRsaPubKeyLen );
	m_pRes->g_pLog.LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"客户端公钥",pkidata->sSignPubKey,pkidata->nSignPubKeyLen);
	m_pRes->g_pLog.LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"签名字符串",signstr,strlen(signstr));
	m_pRes->g_pLog.LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"客户端签名",pkidata->sSignBuffer,pkidata->nSignBufferLen);

	m_pEndian.Endian2Comm((unsigned char *)&(pkidata->nSignBufferLen),sizeof(pkidata->nSignBufferLen));//签名
	m_pEndian.Endian2Comm((unsigned char *)&(pkidata->nSignStrLen),sizeof(pkidata->nSignStrLen));//要签名的字符串
	m_pEndian.Endian2Comm((unsigned char *)&(pkidata->nRsaPubKeyLen),sizeof(pkidata->nRsaPubKeyLen));//加密公钥
	m_pEndian.Endian2Comm((unsigned char *)&(pkidata->nSignPubKeyLen),sizeof(pkidata->nSignPubKeyLen));//签名公钥

	data.head.nLen = sizeof(PKI_GATESTEP23);
	m_pRes->g_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"发送给网关签名及本地证书公钥 MSG_GATEPKISTEP2  nLen:%d",data.head.nLen);
	nRet = Send2Gate(&data);
	if (nRet != 2)
	{
		OnClose(__FILE__,__LINE__,"安全认证 发送签名，本地公钥失败",LOG_ERROR);
		return false;
	}
	m_pRes->g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,"准备接收网关的应答 MSG_GATEPKISTEP3");

	while (true)
	{
		nRet = m_pSocketTcp.CheckReadEvent(m_pRes->g_nConnGateTimeOut*1000);
		if (nRet <= 0)
		{
			OnClose(__FILE__,__LINE__,"安全认证 在指定时间未收到网关响应数据",LOG_ERROR);
			return false;
		}
		m_pRes->g_pLog.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"CheckReadEvent %d ",nRet );
		nRet = OnRcv();
		if (nRet < 0 )
		{
			OnClose(__FILE__,__LINE__,"安全认证 网关连接已关闭，接收数据失败",LOG_ERROR);
			return false;
		}
		m_pRes->g_pLog.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"接收到的数据长度 %d ",nRet );
		bzero(&data,sizeof(CGATE_COMMSTRU));
		nRet = GetRecvData(&data);
		if (nRet == 0)
		{
			//数据没有收全，继续收
			continue;
		}
		if (nRet <0 )
		{
			OnClose(__FILE__,__LINE__,"安全认证 接收数据不完整或网关关闭连接",LOG_ERROR);
			return false;
		}
		break;
	}
	
	m_pRes->g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,"安全认证 接收到的数据长度:%d msgtype:%d nlen:%d retcode:%d",\
		nRet,data.head.stComm.cMsgType,data.head.nLen,data.head.nRetCode);
	
	if (data.head.nRetCode != 0)
	{
		m_pRes->g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"PKI认证失败，网关返回 %d ",data.head.nRetCode);
		OnClose(__FILE__,__LINE__,"安全认证 后台返回PKI认证失败",LOG_ERROR);
		return false;
	}
	
	if (data.head.stComm.cMsgType != MSG_GATEPKISTEP3)
	{
		OnClose(__FILE__,__LINE__,"安全认证 网关发来的数据非MSG_GATEPKISTEP3",LOG_ERROR);
		return false;
	}
	if (data.head.nLen != sizeof(PKI_GATESTEP23))
	{
		OnClose(__FILE__,__LINE__,"安全认证 网关发来的数据非MSG_GATEPKISTEP3",LOG_ERROR);
		return false;
	}
	//保存网关公钥及会话密钥
	pkidata = (PPKI_GATESTEP23)data.buffer;
	m_pEndian.Endian2LocalHost((unsigned char *)&(pkidata->nSignBufferLen),sizeof(pkidata->nSignBufferLen));
	m_pEndian.Endian2LocalHost((unsigned char *)&(pkidata->nSignStrLen),sizeof(pkidata->nSignStrLen));
	m_pEndian.Endian2LocalHost((unsigned char *)&(pkidata->nRsaPubKeyLen),sizeof(pkidata->nRsaPubKeyLen));
	m_pEndian.Endian2LocalHost((unsigned char *)&(pkidata->nSignPubKeyLen),sizeof(pkidata->nSignPubKeyLen));

	bzero(m_sGatePubRsaKey,sizeof(m_sGatePubRsaKey));
	bzero(m_sGatePubSignKey,sizeof(m_sGatePubSignKey));
	memcpy(m_sGatePubRsaKey,pkidata->sRsaPubKey,pkidata->nRsaPubKeyLen);
	m_nGatePubRsaKeyLen = pkidata->nRsaPubKeyLen;
	memcpy(m_sGatePubSignKey,pkidata->sSignPubKey,pkidata->nSignPubKeyLen);
	m_nGatePubSignKeyLen = pkidata->nSignPubKeyLen;
	
	
// 	//签名字符串保存的是会话密钥，使用本地公钥加密
// 	if (!m_pRes->g_pCertClass->UnEncrypt(pkidata->sSignStr,pkidata->nSignStrLen))
// 	{
// 		m_pSocketTcp.Close();
// 		m_pRes->g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"解密会话密钥失败");
// 		return false;
// 	}
// 	if (pkidata->nSignStrLen >= sizeof(m_sSessionKey))
// 	{
// 		m_pSocketTcp.Close();
// 		m_pRes->g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"会话密钥长度[%d]不符",pkidata->nSignStrLen);
// 		return false;
// 	}

	bzero(m_sSessionKey,sizeof(m_sSessionKey));
	memcpy(m_sSessionKey,pkidata->sSignStr,pkidata->nSignStrLen);
//	m_pRes->g_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"会话密钥[%s]",m_sSessionKey);

	//对网关的签名进行验证
	if (!m_pRes->g_pCertClass->CheckSign(m_sGatePubSignKey,m_nGatePubSignKeyLen,signstr,signstrlen,pkidata->sSignBuffer,pkidata->nSignBufferLen))
	{
		OnClose(__FILE__,__LINE__,"安全认证 验证网关签名失败",LOG_ERROR);
		return false;
	}
	m_nIndex = atoi(pkidata->sIndex);
	return true;
}
int CApiSocket::GetRcvDataLen()
{
	return m_nRcvBufferLen;
}

std::string CApiSocket::GenSignString()
{
	char tmpchar[49];
	bzero(tmpchar,sizeof(tmpchar));
	srand((unsigned int)CBF_Date_Time::GetTickCount());
	sprintf(tmpchar,"%05d%04u%07u",rand()%100000,(int)CBF_Date_Time::GetTickCount()%10000,(int)CBF_Date_Time::GetTickUS()%10000000);
	return tmpchar;
}
bool CApiSocket::Compress(unsigned char *destbuf, unsigned int &destlen, unsigned char *srbuf, int srbuflen)
{
	CBF_PMutex pp(&m_mutexCompress);
	return CBF_Tools::Compress(destbuf,destlen,srbuf,srbuflen);
}

bool CApiSocket::Uncompress(unsigned char *destbuf, unsigned int &destlen, unsigned char *srbuf, int srbuflen)
{
	CBF_PMutex pp(&m_mutexCompress);
	return CBF_Tools::Uncompress(destbuf,destlen,srbuf,srbuflen);
}

