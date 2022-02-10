// DesApi.cpp: implementation of the CDesApi class.
//
//////////////////////////////////////////////////////////////////////

#include "DesApi.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDesApi::CDesApi()
{
	memset(m_sRSBuffer,0,sizeof(m_sRSBuffer));
	m_bIsConnect = false;
	m_bIsSet = false;
	m_pLog.SetLogPara(LOG_DEBUG,"","hsmapi.log");
}

CDesApi::~CDesApi()
{
	
}

void CDesApi::SetHostPara(const char *ip, int port, int timeout)
{
	m_sIp = ip;
	m_nPort = port;
	m_nTimeOut = timeout;
	m_bIsSet = true;
}

bool CDesApi::Connect()
{
	if (!m_bIsConnect)
	{
		if (!m_bIsSet)
		{
			m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"未设置加密机参数");
			return false;
		}
		m_pSocket.Create();
		if (!m_pSocket.ConnectTcp((char *)m_sIp.c_str(),m_nPort))
		{
			m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"连接加密机出错 %s %d",m_sIp.c_str(),m_nPort);
			return false;
		}
		m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"连接加密机%s %d 成功",m_sIp.c_str(),m_nPort);
		m_bIsConnect = true;
	}
	return true;
}

bool CDesApi::Connect(const char *ip, int port, int timeout)
{
	SetHostPara(ip,port,timeout);
	return Connect();
}

void CDesApi::DisConnect()
{
	m_pSocket.Close();
	m_bIsConnect = false;
}

int CDesApi::CallSrv(const char *data, int datalen, char *rdata, int &rlen)
{
	if (!Connect())
	{
		return -2;
	}
	int ret;
	char tmpchar[256];
	memset(tmpchar,0,sizeof(tmpchar));
	memset(m_sRSBuffer,0,sizeof(m_sRSBuffer));
	m_sRSBuffer[0] = (datalen+8) / 256;
	m_sRSBuffer[1] = (datalen+8) % 256;
	strncpy(m_sRSBuffer+2,"12345678",8);
	memcpy(m_sRSBuffer+10,data,datalen);

	m_pLog.LogBin(LOG_DEBUG,__FILE__,__LINE__,"发送给加密机的数据",m_sRSBuffer,datalen+10);
	ret = m_pSocket.Send(m_sRSBuffer,datalen+10);
	if (ret != datalen +10)
	{
		m_pSocket.GetLastError(tmpchar,255);
		m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"发送加密机出错 %s ",tmpchar);
		DisConnect();
		return -3;
	}
	ret = m_pSocket.Receive(m_sRSBuffer,2,m_nTimeOut);
	if (ret != 2)
	{
		m_pSocket.GetLastError(tmpchar,255);
		m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"接收加密机出错%d %s ",ret,tmpchar);
		DisConnect();
		return -4;
	}
	rlen = m_sRSBuffer[0]*256+m_sRSBuffer[1];
	ret = m_pSocket.Receive(m_sRSBuffer,rlen,m_nTimeOut);
	if (ret != rlen)
	{
		m_pSocket.GetLastError(tmpchar,255);
		m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"接收加密机出错%d %s ",ret,tmpchar);
		DisConnect();
		return -5;
	}
	m_pLog.LogBin(LOG_DEBUG,__FILE__,__LINE__,"接收加密机应答",m_sRSBuffer,rlen);
	rlen-=12;
	if (rlen>0)
	{
		memcpy(rdata,m_sRSBuffer+12,rlen);
		rdata[rlen]=0;
	}
	m_sRSBuffer[12]=0;
	DisConnect();
	return atoi(m_sRSBuffer+10);
}

int CDesApi::ConvertPinBlock(const char *srcpin, const char *srcpinkey, const char *destpinkey, char *despin,const char *bankacc)
{
	char sDataBuffer[8192];
	
	memset(sDataBuffer,0,sizeof(sDataBuffer));
	char *pBuf=sDataBuffer;
	memcpy(pBuf,"Y1",2);
	pBuf+=2;
	int nsrckeyLen= strlen(srcpinkey);
	if (nsrckeyLen <16 || nsrckeyLen%16>0)
	{
		m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"源密钥长度[%d]不符，应当为16的倍数",nsrckeyLen);
		return -1;
	}
	//AA833914642093B67C8482CF9951AF40
	
	//Y1 X673505CD5562C222D4548572BFCA9E08 8B4ECCAE01B4B17A F2A9B25141B68DF62C1639E40F7D5939 900000002070 | Y200 E8696663B0B82B26
	switch (nsrckeyLen)
	{
		case 16:
			memcpy(pBuf,srcpinkey,nsrckeyLen);
			pBuf+=nsrckeyLen;
			break;
		case 32:
			memcpy(pBuf,"X",1);
			pBuf+=1;
			memcpy(pBuf,srcpinkey,nsrckeyLen);
			pBuf+=nsrckeyLen;
			break;
		case 48:
			memcpy(pBuf,"Y",1);
			pBuf+=1;
			memcpy(pBuf,srcpinkey,nsrckeyLen);
			pBuf+=nsrckeyLen;
			break;
		default :
			m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"源密钥长度[%d]不符，应当为16的倍数且不大于48",nsrckeyLen);
			return -1;
	}
	int ndestLen = strlen(destpinkey);
	switch (ndestLen)
	{
		case 16:
			memcpy(pBuf,destpinkey,ndestLen);
			pBuf+=ndestLen;
			break;
		case 32:
			memcpy(pBuf,"X",1);
			pBuf+=1;
			memcpy(pBuf,destpinkey,ndestLen);
			pBuf+=ndestLen;
			break;
		case 48:
			memcpy(pBuf,"Y",1);
			pBuf+=1;
			memcpy(pBuf,destpinkey,ndestLen);
			pBuf+=ndestLen;
			break;
		default :
			m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"目的密钥长度[%d]不符，应当为16的倍数且不大于48",destpinkey);
			return -1;
	}
	int nsrcpinLen= strlen(srcpin);
	if (nsrcpinLen!=32)
	{
		m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"源PINBLOCK长度[%d]不符，应为32",nsrcpinLen);
		return -1;
	}
	memcpy(pBuf,srcpin,nsrcpinLen);
	pBuf+=nsrcpinLen;
	if (strlen(bankacc)<12)
	{
		memcpy(pBuf,"000000000000",12);
	}
	else
	{
		int acclen= strlen(bankacc);
		memcpy(pBuf,bankacc+(acclen-13),12);
	}
	pBuf+=12;
	int sendlen= pBuf - sDataBuffer;
	int ret = CallSrv(sDataBuffer,sendlen,despin,sendlen);
	if (ret != 0)
	{
		m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"加密机返回错[%d] %s",ret,despin);
		return ret;
	}
	return 0;
}
