// FrontApi.cpp: implementation of the CFrontApi class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "FrontApi.h"
#include "BF_Mutex.h"
#include "BF_SocketTcp.h"
#include "BF_SimpleLog.h"
#include "BF_Date_Time.h"
#include "BF_Tools.h"


#define  MAXAPIBUF 1024*96
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFrontApi::CFrontApi()
{
	m_bIsConnect = false;
	memset(m_errMsg,0,sizeof(m_errMsg));
	m_nHeadLen =  5;
	m_sServerIp = "127.0.0.1";
	m_nPort = 9999;
	m_nTimeOut = 5000;
	m_nEncryptZipFlag = 0;
	m_nPwdEncryptFlag = 0;
	m_bIsIncludeSelf = false;
	memset(m_sPinKey,0,sizeof(m_sPinKey));
	memset(m_sMacKey,0,sizeof(m_sMacKey));
	
	memset(m_sDesEncryptKey,0,sizeof(m_sDesEncryptKey));
	
	strcpy(m_sPinKey,"sz123456");
	strcpy(m_sDesEncryptKey,"rd402wms");
	m_nLastUseTime=0;
	m_bIsXtBegin = false;
	m_nXtBeginTime = 0;
	m_nXTUseTIme = 600000 ;//默认心跳时间为10分钟

}

CFrontApi::~CFrontApi()
{

}

int CFrontApi::SendBuf(const char *buffer, int buflen)
{
	int ret;
	int sendlen;
	CBF_PMutex pp(&m_mutexSend);
	if (!m_bIsConnect)
	{
		sprintf(m_errMsg,"未连接服务端");
		return -3000;
	}

	if (buflen <=0 || buflen > MAXAPIBUF)
	{
		sprintf(m_errMsg,"没有要发送的数据或要发送的数据过大 buflen=[%d]",buflen);
		return -998;
	}
	if (m_nHeadLen <1)
	{
		sendlen = buflen;
		m_log.LogBin(LOG_DEBUG,__FILE__,__LINE__,"要发送的数据内容",(char *)buffer,sendlen);
		ret = m_tcpSocket.Send((char *)buffer,sendlen);
		if (ret!=sendlen)
		{
			sprintf(m_errMsg,"发送数据出错 要发送数据[%d] 实发数据[%d]",sendlen,ret);
			DisConnect();
			return -1;
		}
	}
	else
	{
		char bufhead[40];
		char tmpchar[40];
		char sendbuf[MAXAPIBUF];
		memset(bufhead,0,sizeof(bufhead));
		memset(tmpchar,0,sizeof(tmpchar));
		memset(sendbuf,0,sizeof(sendbuf));

		sendlen = buflen;
		memset(sendbuf,0,sizeof(sendbuf));
		sprintf(tmpchar,"%s0%d%s","%",m_nHeadLen,"d");
		if (m_bIsIncludeSelf)
		{
			sprintf(sendbuf,tmpchar,sendlen+m_nHeadLen);
		}
		else
		{
			sprintf(sendbuf,tmpchar,sendlen);
		}
		memcpy(sendbuf+m_nHeadLen,buffer,sendlen);
		sendlen = sendlen+m_nHeadLen;
		m_log.LogBin(LOG_DEBUG,__FILE__,__LINE__,"要发送的数据内容",sendbuf,sendlen);
		ret = m_tcpSocket.Send(sendbuf,sendlen);
		if (ret!=sendlen)
		{
			sprintf(m_errMsg,"发送数据出错 要发送数据[%d] 实发数据[%d]",sendlen,ret);
			m_log.LogMp(LOG_DEBUG-1,__FILE__,__LINE__,m_errMsg);
			DisConnect();
			return -1;
		}
	}
	return sendlen-m_nHeadLen;

}

int CFrontApi::RecvBuf(char *buffer, int &buflen,int timeout)
{
	int ret;
	unsigned int rcvlen=0;
	if (!m_bIsConnect)
	{
		sprintf(m_errMsg,"未连接服务端");
		return -3000;
	}
	CBF_PMutex pp(&m_mutexRcv);
	ret = m_tcpSocket.CheckReadEvent(timeout);
	if (ret >0)
	{
		if (m_nHeadLen<1)
		{
			rcvlen = buflen;
			buflen = m_tcpSocket.Receive(buffer,rcvlen);
			if (buflen >0)
			{
				buffer[buflen]=0;
				m_log.LogBin(LOG_DEBUG,__FILE__,__LINE__,"收到数据包",buffer,buflen);
			}
			else
			{
				sprintf(m_errMsg,"接收数据出错 %d %d %s",buflen,m_tcpSocket.GetErrorNo(),m_tcpSocket.GetErrorMsg().c_str());
				m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,m_errMsg);
				if (buflen<0)
				{
					DisConnect();
				}
			}
			return buflen;
		}
		else
		{
			char bufhead[40];
			memset(bufhead,0,sizeof(bufhead));
			ret = m_tcpSocket.Receive(bufhead,m_nHeadLen,timeout);
			if (ret <= 0)
			{
				if (ret < 0)
				{
					sprintf(m_errMsg,"接收通讯报文头出错，连接已断开 %d %d %s",ret,m_tcpSocket.GetErrorNo(),m_tcpSocket.GetErrorMsg().c_str());
					DisConnect();
				}
				else
				{
					sprintf(m_errMsg,"%d时间内未接收到数据 %d %d %s",timeout,ret,m_tcpSocket.GetErrorNo(),m_tcpSocket.GetErrorMsg().c_str());
				}
				m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,m_errMsg);
				return ret;
			}
			bufhead[ret]=0;
			if (ret != m_nHeadLen) //通讯报文头不符,连接关闭
			{
				sprintf(m_errMsg,"通讯报文头不符,连接关闭 [%s]-%d",bufhead,ret);
				m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,m_errMsg);
				DisConnect();
				return -1000;
			}
			m_pTool.LRtrim(bufhead);
			if (!m_pTool.IsNumber(bufhead))
			{
				sprintf(m_errMsg,"通讯报文头不符,报文头非法，连接关闭 [%s]-%d",bufhead,ret);
				m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,m_errMsg);
				DisConnect();
				return -1001;
			}
			rcvlen = atoi(bufhead);
			if (m_bIsIncludeSelf)
			{
				rcvlen = rcvlen -m_nHeadLen;
			}
			if (rcvlen <1)
			{
				sprintf(m_errMsg,"通讯报文头不符,后续报文长度非法[%s]-%d",bufhead,rcvlen);
				m_log.LogMp(LOG_DEBUG-1,__FILE__,__LINE__,m_errMsg);
				DisConnect();
				return -1002;
			}
			if (buflen < rcvlen )
			{
				sprintf(m_errMsg,"通讯报文头不符或传入数据缓冲不够[%d]-[%d]",rcvlen,buflen);
				m_log.LogMp(LOG_DEBUG-1,__FILE__,__LINE__,m_errMsg);
				DisConnect();
				return -1003;
			}
			ret = m_tcpSocket.Receive(buffer,rcvlen,timeout);
			if (ret != rcvlen)
			{
				if (ret>0)
				{
					m_log.LogBin(LOG_DEBUG,__FILE__,__LINE__,"收到的错误包",buffer,ret);
				}
				if (ret<0)
				{
					sprintf(m_errMsg,"接收数据出错，该收到数据长度[%d]实收到数据长度[%d] %d %s",rcvlen,ret,m_tcpSocket.GetErrorNo(),m_tcpSocket.GetErrorMsg().c_str());
					m_log.LogMp(LOG_DEBUG-1,__FILE__,__LINE__,m_errMsg);
				}
				else
				{
					sprintf(m_errMsg,"接收数据出错，该收到数据长度[%d]实收到数据长度[%d]",rcvlen,ret);
					m_log.LogMp(LOG_DEBUG-1,__FILE__,__LINE__,m_errMsg);
				}
				
				DisConnect();
				return -1004;
			}
			buffer[ret]=0;
			buflen = rcvlen;
		}
		

		m_log.LogBin(LOG_DEBUG,__FILE__,__LINE__,"接收到的数据",buffer,buflen);
		return buflen; 
	}
	else
	{
		if (ret == 0)
		{
			sprintf(m_errMsg,"%d时间内未接收到数据 %d %d %s",timeout,ret,m_tcpSocket.GetErrorNo(),m_tcpSocket.GetErrorMsg().c_str());
		}
		else
		{
			sprintf(m_errMsg,"CheckDataArrived出错 %d %s",m_tcpSocket.GetErrorNo(),m_tcpSocket.GetErrorMsg().c_str());
		}
		m_log.LogMp(LOG_DEBUG-1,__FILE__,__LINE__,m_errMsg);
		DisConnect();
		return ret;
	}

}


bool CFrontApi::Connect()
{
	m_tcpSocket.Close();
	if (!m_tcpSocket.Create(AF_INET))
	{
		sprintf(m_errMsg,"建立socket失败");
		m_log.LogMp(0,__FILE__,__LINE__,"建立socket失败");
		return false;
	}
	if (!m_tcpSocket.ConnectServer((char *)m_sServerIp.c_str(), m_nPort))
	{
		sprintf(m_errMsg,"连接远程服务端失败 %d-%s",m_nPort,m_sServerIp.c_str());
		//m_log.LogMp(0,__FILE__,__LINE__,"连接远程服务端失败 %d %s",m_nPort,(char *)m_sServerIp.c_str());
		m_log.LogMp(LOG_DEBUG-1,__FILE__,__LINE__,m_errMsg);
		return false;
	}
	m_bIsConnect = true;
	m_nLastUseTime = m_publog.GetTickCount();
	m_bIsXtBegin = false;
	return true;
}

void CFrontApi::DisConnect()
{
	m_tcpSocket.Close();
	m_bIsXtBegin = false;
	m_bIsConnect = false;
	return;
}

void CFrontApi::SetPara(const char *ip, int port,const char *logname,int loglevel,int headlen,bool isincludehead,int pwdflag,int encryptflag)
{
	m_sServerIp = ip;
	m_nPort = port;
	m_nHeadLen = headlen;
	m_log.SetLogPara(loglevel,"",logname);
	m_nPwdEncryptFlag = pwdflag;
	m_nEncryptZipFlag = encryptflag;
}

string CFrontApi::GetErrMsg()
{
	return m_errMsg;
}

bool CFrontApi::IsConnect()
{
	return m_bIsConnect;
}


void CFrontApi::SetXTSegTime(int ntime)
{
	m_nXTUseTIme = ntime;
}

