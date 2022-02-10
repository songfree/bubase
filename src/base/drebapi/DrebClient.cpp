// DrebClient.cpp: implementation of the CDrebClient class.
//
//////////////////////////////////////////////////////////////////////

#include "DrebClient.h"
#include "BF_Tools.h"
#include "BF_SimpleLog.h"
#include "BF_Zip.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

char g_VERSION[20]="1.0.2.20100724";   //api的版本



CDrebClient::CDrebClient()
{
	m_bIsInit = false;
	m_bIsConnect = false;
	memset(&m_head,0,sizeof(DREB_HEAD));
	memset(m_errMsg,0,sizeof(m_errMsg));
	memset(m_sRcvBuffer,0,sizeof(m_sRcvBuffer));
	memset(m_sSendBuffer,0,sizeof(m_sSendBuffer));
	m_nTimeOut = 5000;
	bzero(m_tmpdata,sizeof(m_tmpdata));
	m_tmpdatalen = sizeof(m_tmpdata);
	
	m_pLog.SetLogPara(LOG_DEBUG,"","deszip.log");
	m_pDesZip.SetLogPara(&m_pLog);
}

CDrebClient::~CDrebClient()
{

}

bool CDrebClient::Init(const char *ip, short port,int timeout)
{
	m_sIp = ip;
	m_nPort = port;
	m_nTimeOut = timeout;
	m_bIsInit = true;
	return true;
}

bool CDrebClient::Connect()
{
	m_bIsConnect = false;
	m_tcpSocket.Create(AF_INET,true);//阻塞模式
	if (!m_tcpSocket.ConnectServer((char *)m_sIp.c_str(),m_nPort))
	{
		sprintf(m_errMsg,"连接DREB%s %d失败 %s",m_sIp.c_str(),m_nPort,m_tcpSocket.GetErrorMsg().c_str());
		Close();
		return false;
	}
	int ret;
	unsigned int datalen;
 	char reginfo[8192];
	char tmpchar[256];
	//发送注册信息
	memset(reginfo,0,sizeof(reginfo));
	S_CONNECT_DATA *conData = (S_CONNECT_DATA *)reginfo;
	conData->cConnectType = 0;
	sprintf(conData->sVersion,"%s",g_VERSION);
	DREB_D_NODEINFO dest;
	memset(&dest,0,sizeof(DREB_D_NODEINFO));
	ret = SendData(0,0,0,CMD_CONNECT,dest,reginfo,sizeof(S_CONNECT_DATA),0);
	if (ret != sizeof(S_CONNECT_DATA))
	{
		sprintf(m_errMsg,"连接DREB%s %d时发送CMD_CONNECT数据出错 %s",m_sIp.c_str(),m_nPort,m_tcpSocket.GetErrorMsg().c_str());
		Close();
		return false;
	}
	m_bIsConnect = true;
	datalen = sizeof(reginfo);
	ret = ReceveData(m_head,(unsigned char *)reginfo,datalen,5000);
	if (ret <0)
	{
		Close();
//		sprintf(m_errMsg,"连接DREB%s %d时接收CMD_CONNECT应答出错 %s",m_sIp.c_str(),m_nPort,m_tcpSocket.GetErrorMsg().c_str());
		return false;
	}
	m_bIsConnect = false;
	if (m_head.a_Ainfo.a_nRetCode != SUCCESS)
	{
		Close();
		sprintf(m_errMsg,"连接DREB%s %d时DREB返回 %d 错",m_sIp.c_str(),m_nPort,m_head.a_Ainfo.a_nRetCode);
		return false;
	}
	m_bIsConnect = true;
	return true;
}

void CDrebClient::Close()
{
	m_tcpSocket.Close();
	m_bIsConnect = false;
}

int CDrebClient::ReceveData(DREB_HEAD &head,unsigned char *data,unsigned int &datalen,int timeout)
{
	int ret;
	int rcvlen=0;
	if (!m_bIsConnect)
	{
		sprintf(m_errMsg,"未连接DREB");
		return -100;
	}
	unsigned int wantlen = DREBHEADLEN;
	ret = m_tcpSocket.Receive(m_sRcvBuffer,wantlen,timeout);
	if (ret != DREBHEADLEN)
	{
		if (ret == 0)
		{
			sprintf(m_errMsg,"在指定时间内未收到数据");
			return 0;
		}
		else if (ret <0)
		{
			sprintf(m_errMsg,"连接已被DREB断开 %s",m_tcpSocket.GetErrorMsg().c_str());
			Close();
			return -1;
		}
		else
		{
			sprintf(m_errMsg,"DREB应答数据不符");
			Close();
			return -2;
		}
	}
	//进行字节序处理并校验CRC
	memcpy(&head,m_sRcvBuffer,DREBHEADLEN);
	m_pCheckHead.Endian2LocalHost(&head);
// 	{
// 		sprintf(m_errMsg,"CRC错误");
// 		Close();
// 		return -3;
// 	}
	if (head.nLen >0 )
	{
		wantlen = head.nLen;
		ret = m_tcpSocket.Receive(m_sRcvBuffer,wantlen,timeout);
		if (ret != head.nLen)
		{
			if (ret == 0)
			{
				sprintf(m_errMsg,"在指定时间内未收到包体数据");
				return -4;
			}
			else if (ret <0)
			{
				sprintf(m_errMsg,"接收包体时连接已被DREB断开 %s",m_tcpSocket.GetErrorMsg().c_str());
				Close();
				return -1;
			}
			else
			{
				sprintf(m_errMsg,"接收包体时DREB应答数据不符");
				Close();
				return -2;
			}
		}
		rcvlen = head.nLen;
		ret = head.nLen;
		COMMSTRU drebdata;
		memcpy(&(drebdata.head),&head,DREBHEADLEN);
		memcpy(drebdata.buffer,m_sRcvBuffer,head.nLen);
		
		if (m_pDesZip.DrebZipDesProcess(&drebdata,0))
		{
			memcpy(data,drebdata.buffer,drebdata.head.nLen);
			head.nLen = drebdata.head.nLen;
			data[head.nLen]=0;
			datalen = head.nLen;
			return datalen+DREBHEADLEN;
		}
		else
		{
			memcpy(data,m_sRcvBuffer,head.nLen);
			data[head.nLen]=0;
			datalen = head.nLen;
			return datalen+DREBHEADLEN;
		}
	}
	return DREBHEADLEN;
}

int CDrebClient::SendData(int hook,int serial,int gateindex,int cmd,DREB_D_NODEINFO dest,const char *data, int datalen,int cZip,int nextno)
{
	int ret;
	memset(m_sSendBuffer,0,sizeof(m_sSendBuffer));
	memcpy(m_sSendBuffer,&m_head,DREBHEADLEN);
	PCOMMSTRU databuf = (PCOMMSTRU)m_sSendBuffer;
	databuf->head.cDrebAffirm = 0;
	databuf->head.cNextFlag = 0;
	databuf->head.cRaflag = 0;
	databuf->head.cZip = cZip;
	databuf->head.cCmd = cmd;
	databuf->head.nLen = datalen;
	
	databuf->head.d_Dinfo.d_cNodePrivateId = dest.d_cNodePrivateId;
	databuf->head.d_Dinfo.d_cSvrPrivateId = dest.d_cSvrPrivateId;
	databuf->head.d_Dinfo.d_nNodeId = dest.d_nNodeId;
	databuf->head.d_Dinfo.d_nSvrMainId = dest.d_nSvrMainId;
	databuf->head.d_Dinfo.d_nServiceNo = dest.d_nServiceNo;
	databuf->head.s_Sinfo.s_nHook = hook;
	databuf->head.s_Sinfo.s_nSerial = serial;
	databuf->head.n_Ninfo.n_nNextNo = nextno;
	databuf->head.s_Sinfo.s_nGateIndex = gateindex;
	m_pCheckHead.Endian2CommGenCrc(&(databuf->head));

	if (datalen >= DREBBUFSIZE-DREBHEADLEN)
	{
		sprintf(m_errMsg,"发送数据太大 %d>=%d",datalen,DREBBUFSIZE-DREBHEADLEN);
		return -2;
	}
	if (datalen >0 )
	{
		memcpy(m_sSendBuffer+DREBHEADLEN,data,datalen);
	}
	
	ret = m_tcpSocket.Send(m_sSendBuffer,DREBHEADLEN+datalen);
	if (ret != DREBHEADLEN+datalen)
	{
		sprintf(m_errMsg,"发送数据出错 %s",m_tcpSocket.GetErrorMsg().c_str());
		Close();
		return -1;
	}
	
	return ret - DREBHEADLEN;
}

bool CDrebClient::IsConnect()
{
	if (!m_bIsConnect)
	{
		return m_bIsConnect;
	}
	
	//检查连接是否正常
	int nRet = m_tcpSocket.CheckReadEvent(0);
	if (nRet < 0 ) 
	{
		Close();//关闭连接，置状态
	}
	if (nRet == 1)
	{
		char buf[2];
		int bufsize=sizeof(buf);
		nRet = m_tcpSocket.Receive(buf,bufsize);
		if (nRet == 0) //有读但无数据，对端主动关闭
		{
			Close();//关闭连接，置状态
		}
	}
	return m_bIsConnect;
}

int CDrebClient::DPCall(int hook, int serial, DREB_D_NODEINFO dest,const char *data, int datalen,int cZip, DREB_HEAD &head, char *ansdata)
{
	int ret;
	ret = SendData(hook,serial,0,CMD_DPCALL,dest,data,datalen,cZip);
	if (ret != datalen)
	{
// 		if (!IsConnect())
// 		{
// 			Connect();
// 		}
		return -1;
	}
	while (true)
	{
		m_nRcvLen = sizeof(m_sRcvBuffer);
		ret = ReceveData(head,(unsigned char *)m_sRcvBuffer,m_nRcvLen,m_nTimeOut);
		if (ret <=0)
		{
			return ret;
		}
		if (head.a_Ainfo.a_nRetCode == SUCCESS && head.cDrebAffirm == 1)
		{
			continue;
		}
		else if (head.a_Ainfo.a_nRetCode != SUCCESS)
		{
			if (head.nLen>0)
			{
				memcpy(ansdata,m_sRcvBuffer,head.nLen);
			}
			return head.a_Ainfo.a_nRetCode;
		}
		memcpy(ansdata,m_sRcvBuffer,m_nRcvLen);
		return ret;
	}
	return ret;
}

int CDrebClient::DPACall(int hook, int serial, DREB_D_NODEINFO dest, const char *data, int datalen,int cZip)
{
	int ret;
	ret = SendData(hook,serial,0,CMD_DPACALL,dest,data,datalen,cZip);
	if (ret != datalen)
	{
// 		if (!IsConnect())
// 		{
// 			Connect();
// 		}
		return -1;
	}
	DREB_HEAD head;
	m_nRcvLen = sizeof(m_sRcvBuffer);
	ret = ReceveData(head,(unsigned char *)m_sRcvBuffer,m_nRcvLen,m_nTimeOut);
	if (ret <=0)
	{
		return ret;
	}
	if (head.a_Ainfo.a_nRetCode == SUCCESS && head.cDrebAffirm == 1)
	{
		return 100;
	}
	else if (head.a_Ainfo.a_nRetCode != SUCCESS)
	{
		return head.a_Ainfo.a_nRetCode;
	}
	return ret;
}

int CDrebClient::DPPush(int hook, int serial, DREB_D_NODEINFO dest, const char *data, int datalen,int cZip)
{
	int ret;
	ret = SendData(hook,serial,0,CMD_DPPUSH,dest,data,datalen,cZip);
	if (ret != datalen)
	{
// 		if (!IsConnect())
// 		{
// 			Connect();
// 		}
		return -1;
	}
	return ret;
}

int CDrebClient::DPPost(int hook, int serial, DREB_D_NODEINFO dest, const char *data, int datalen,int cZip)
{
	int ret;
	ret = SendData(hook,serial,0,CMD_DPPOST,dest,data,datalen,cZip);
	if (ret != datalen)
	{
// 		if (!IsConnect())
// 		{
// 			Connect();
// 		}
		return -1;
	}
	DREB_HEAD head;
	m_nRcvLen = sizeof(m_sRcvBuffer);
	ret = ReceveData(head,(unsigned char *)m_sRcvBuffer,m_nRcvLen,m_nTimeOut);
	if (ret <=0)
	{
		return ret;
	}
	if (head.a_Ainfo.a_nRetCode == SUCCESS && head.cDrebAffirm == 1)
	{
		return 100;
	}
	else if (head.a_Ainfo.a_nRetCode != SUCCESS)
	{
		return head.a_Ainfo.a_nRetCode;
	}
	return ret;
}

int CDrebClient::DPBC(int hook, int serial,DREB_D_NODEINFO dest, const char *data, int datalen,int cZip)
{
	int ret;
	DREB_D_NODEINFO destdreb;
	destdreb.d_nNodeId = dest.d_nNodeId;
	destdreb.d_cNodePrivateId = dest.d_cNodePrivateId;
	destdreb.d_nSvrMainId = dest.d_nSvrMainId;
	destdreb.d_cSvrPrivateId = dest.d_cSvrPrivateId;
	destdreb.d_nServiceNo = dest.d_nServiceNo;
	ret = SendData(hook,serial,0,CMD_DPBC,destdreb,data,datalen,cZip);
	if (ret != datalen)
	{
// 		if (!IsConnect())
// 		{
// 			Connect();
// 		}
		return -1;
	}
	DREB_HEAD head;
	m_nRcvLen = sizeof(m_sRcvBuffer);
	ret = ReceveData(head,(unsigned char *)m_sRcvBuffer,m_nRcvLen,m_nTimeOut);
	if (ret <=0)
	{
		return ret;
	}
	if (head.a_Ainfo.a_nRetCode == SUCCESS && head.cDrebAffirm == 1)
	{
		return 100;
	}
	else if (head.a_Ainfo.a_nRetCode != SUCCESS)
	{
		return head.a_Ainfo.a_nRetCode;
	}
	return ret;
}

int CDrebClient::DPABC(int hook, int serial,int gateindex,DREB_D_NODEINFO dest, const char *data, int datalen,int cZip,int nextno)
{
	int ret;
	DREB_D_NODEINFO destdreb;
	destdreb.d_nNodeId = dest.d_nNodeId;
	destdreb.d_cNodePrivateId = dest.d_cNodePrivateId;
	destdreb.d_nSvrMainId = dest.d_nSvrMainId;
	destdreb.d_cSvrPrivateId = dest.d_cSvrPrivateId;
	destdreb.d_nServiceNo = dest.d_nServiceNo;
	ret = SendData(hook,serial,gateindex,CMD_DPABC,destdreb,data,datalen,cZip,nextno);
	if (ret != datalen)
	{
// 		if (!IsConnect())
// 		{
// 			Connect();
// 		}
		return -1;
	}
	return ret;
}

void CDrebClient::GetDrebHead(DREB_HEAD &head)
{
	memcpy(&head,&m_head,sizeof(DREB_HEAD));
}
// bool CDrebClient::UnzipBuf(int IsZipEncrypt, char *buf,int &buflen)
// {
// 	char tmpbuf[DREBBUFSIZE];
// 	unsigned int dlen;
// 	
// 	memset(tmpbuf,0,sizeof(tmpbuf));
// 	
// 	switch (IsZipEncrypt)
// 	{
// 		case 1:
// 			if (encode.unencryptchar(buf,buflen,ENCODEKEY)<0)
// 			{
// 				return false;
// 			}
// 			break;
// 		case 2:
// 			dlen= sizeof(tmpbuf);
// 			if (!Uncompress((unsigned char *)tmpbuf,&dlen,(unsigned char *)buf,buflen))
// 			{
// 				return false;
// 			}
// 			memcpy(buf,tmpbuf,dlen);
// 			buflen = dlen;
// 			buf[buflen]=0;
// 			break;
// 		case 3:
// 			if (encrypt.unencrypt64(buf,buflen,DESENCRYPTKEY)<0)
// 			{
// 				return false;
// 			}
// 			break;
// 		case 4:
// 			dlen= sizeof(tmpbuf);
// 			if (!Uncompress((unsigned char *)tmpbuf,&dlen,(unsigned char *)buf,buflen))
// 			{
// 				return false;
// 			}
// 			memcpy(buf,tmpbuf,dlen);
// 			buflen = dlen;
// 			buf[buflen]=0;
// 			if (encrypt.unencrypt64(buf,buflen,DESENCRYPTKEY)<0)
// 			{
// 				return false;
// 			}
// 			break;
// 		default:
// 			break;
// 	}
// 	return true;
// }
// bool CDrebClient::ZipBuf(int IsZipEncrypt,char *buf,int &buflen)
// {
// 	CEncode encode;
// 	Cencrypt encrypt;
// 	char tmpbuf[DREBBUFSIZE];
// 	
// 	unsigned int dlen;
// 	
// 	memset(tmpbuf,0,sizeof(tmpbuf));
// 	
// 	switch (IsZipEncrypt)
// 	{
// 	case 1:
// 		if (encode.encryptchar(buf,buflen,ENCODEKEY)<0)
// 		{
// 			return false;
// 		}
// 		break;
// 	case 2:
// 		memcpy(tmpbuf,buf,buflen);
// 		dlen = sizeof(tmpbuf);
// 		if (!Compress((unsigned char *)buf,&dlen,(unsigned char *)tmpbuf,buflen))
// 		{
// 			return false;
// 		}
// 		buflen = dlen;
// 		break;
// 	case 3:
// 		if (encrypt.encrypt64(buf,buflen,DESENCRYPTKEY)<0)
// 		{
// 			return false;
// 		}
// 		break;
// 	case 4:
// 		if (encrypt.encrypt64(buf,buflen,DESENCRYPTKEY)<0)
// 		{
// 			return false;
// 		}
// 		memcpy(tmpbuf,buf,buflen);
// 		dlen = sizeof(tmpbuf);
// 		if (!Compress((unsigned char *)buf,&dlen,(unsigned char *)tmpbuf,buflen))
// 		{
// 			return false;
// 		}
// 		buflen = dlen;
// 		break;
// 	default:
// 		break;
// 	}
// 	return true;
// }

int CDrebClient::Monitor_Dreb(int hook, int serial, DREB_D_NODEINFO dest,const char *data, int datalen,int cZip, DREB_HEAD &head, char *ansdata)
{
	int ret;
	ret = SendData(hook,serial,0,CMD_MONITOR_DREB,dest,data,datalen,cZip);
	if (ret != datalen)
	{
// 		if (!IsConnect())
// 		{
// 			Connect();
// 		}
		return -1;
	}
	while (true)
	{
		m_nRcvLen = sizeof(m_sRcvBuffer);
		ret = ReceveData(head,(unsigned char *)m_sRcvBuffer,m_nRcvLen,m_nTimeOut);
		if (ret <=0)
		{
			return ret;
		}
		if (head.a_Ainfo.a_nRetCode == SUCCESS && head.cDrebAffirm == 1)
		{
			continue;
		}
		else if (head.a_Ainfo.a_nRetCode != SUCCESS)
		{
			if (head.nLen>0)
			{
				memcpy(ansdata,m_sRcvBuffer,head.nLen);
			}
			return head.a_Ainfo.a_nRetCode;
		}
		memcpy(ansdata,m_sRcvBuffer,m_nRcvLen);
		return ret;
	}
	return ret;
}
int CDrebClient::Monitor_Bpc(int hook, int serial, DREB_D_NODEINFO dest,const char *data, int datalen,int cZip, DREB_HEAD &head, char *ansdata)
{
	int ret;
	ret = SendData(hook,serial,0,CMD_MONITOR_BPC,dest,data,datalen,cZip);
	if (ret != datalen)
	{
// 		if (!IsConnect())
// 		{
// 			Connect();
// 		}
		return -1;
	}
	while (true)
	{
		m_nRcvLen = sizeof(m_sRcvBuffer);
		ret = ReceveData(head,(unsigned char *)m_sRcvBuffer,m_nRcvLen,m_nTimeOut);
		if (ret <=0)
		{
			return ret;
		}
		if (head.a_Ainfo.a_nRetCode == SUCCESS && head.cDrebAffirm == 1)
		{
			continue;
		}
		else if (head.a_Ainfo.a_nRetCode != SUCCESS)
		{
			if (head.nLen>0)
			{
				memcpy(ansdata,m_sRcvBuffer,head.nLen);
			}
			return head.a_Ainfo.a_nRetCode;
		}
		memcpy(ansdata,m_sRcvBuffer,m_nRcvLen);
		return ret;
	}
	return ret;
}
