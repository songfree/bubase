// Send2Server.cpp: implementation of the CSend2Server class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "testcomm.h"
#include "Send2Server.h"
#include "XmlPackArray.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSend2Server::CSend2Server()
{
	m_isRunning = false;
	memset(m_errMsg,0,sizeof(m_errMsg));
	m_usetime = 0;
	m_sendNum = 0;
	m_sendData = NULL;
	m_log.SetLogPara(4,"","testcomm.log");
	m_bIsResult = true;
	m_nFaildNum=0;
	m_nSuccessNum=0;
	m_nSendNum = 0;
	m_nSendErrorNum =0;
	m_nConnectErrorNum =0;
	m_nRecvErrorNum =0;
	m_bIsConnect = false;
	m_commpara = NULL;
}

CSend2Server::~CSend2Server()
{
	m_sendData = NULL;
	m_commpara = NULL;
}
int CSend2Server::Run()
{
	if (m_sendData==NULL)
	{
		return -1;
	}
	m_usetime = 0;
	int nTryConnectNum=0;
	unsigned int buflen;
	int ret;
	long usetime1,usetime2;
	S_REQANS s_reqans;
	char sendBuf[MAXREQBUF];
	char recvBuf[MAXREQBUF];
	CXmlPack xmlpack;
	CXmlPack initxmlpack;
	CXmlPackArray muxmlpack;
	m_publog.Update();
	m_log.SetLogPara(m_sendData->m_nLogLevel,"","testcomm.log");

	m_usetime = 0;
	for (int i=0;m_isRunning && !m_sendData->m_bIsExit ;i++)
	{
		if (m_sendNum < 10000)
		{
			if (i>=m_sendNum)
			{
				break;
			}
		}
		
		if (!m_sendData->m_bIsBegin)
		{
			SLEEP(1);
			continue;
		}
		if (m_packtype == 3) //xml包类型
		{
			buflen = sizeof(sendBuf);
			if (!m_sendData->GetSendBuf(sendBuf,buflen,initxmlpack))
			{
				sprintf(m_errMsg,"取包内容失败!");
				m_isRunning = false;
				if (m_bIsResult)
				{
					memset(&s_reqans,0,sizeof(S_REQANS));
					s_reqans.raflag = 2;
					s_reqans.threadNo = m_mythreadNo;
					s_reqans.bufferlen = strlen(m_errMsg);
					strcpy(s_reqans.buffer,m_errMsg);
					m_publog.ToCharTime8(s_reqans.cutime);
					if (!m_sendData->PushReqAns(s_reqans))
					{
						m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"放队列失败");
					}
				}
				m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"线程终止 %s",m_errMsg);
				return -1;
			}
			if (m_bIsResult)
			{
				m_publog.Update();
				memset(&s_reqans,0,sizeof(S_REQANS));
				s_reqans.raflag = 0;
				s_reqans.threadNo = m_mythreadNo;
				s_reqans.bufferlen = buflen-m_sendData->m_headLen;
				m_publog.ToCharTime8(s_reqans.cutime);
				if (buflen < MAXREQBUF)
				{
					memcpy(s_reqans.buffer,sendBuf+m_sendData->m_headLen,buflen-m_sendData->m_headLen);
				}
				else
				{
					xmlpack.setReturn(999,"缓冲区不够");
					s_reqans.bufferlen = sizeof(s_reqans.buffer);
					xmlpack.tobuffer(s_reqans.buffer,s_reqans.bufferlen);
				}
				if (!m_sendData->PushReqAns(s_reqans))
				{
					m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"放队列失败");
				}
			}
			//压缩选项
			if (m_commpara->zip == 2)
			{
				unsigned int dlen;
				dlen = sizeof(recvBuf);
				if (!CBF_Tools::Compress((unsigned char *)recvBuf,dlen,(unsigned char *)(sendBuf+m_commpara->headlen),buflen-m_commpara->headlen))
				{
					m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"Compress失败");
					return -1;
				}
				memcpy(sendBuf+m_commpara->headlen,recvBuf,dlen);
				buflen = dlen+m_commpara->headlen;
				char tmpchar[30];
				char tmpchar2[30];
				memset(tmpchar,0,sizeof(tmpchar));
				memset(tmpchar2,0,sizeof(tmpchar2));
				sprintf(tmpchar,"%s0%d%s","%",m_commpara->headlen,"d");
				
				sprintf(tmpchar2,tmpchar,dlen);
				memcpy(sendBuf,tmpchar2,m_commpara->headlen);
			}
		}
		else // if (m_packtype == 3) //xml包类型
		{	
			memset(sendBuf,0,sizeof(sendBuf));
			buflen = sizeof(sendBuf);
			
			if (!m_sendData->GetSendBuf(sendBuf,buflen))
			{
				m_isRunning = true;
				sprintf(m_errMsg,"取包内容失败!");
				m_isRunning = false;
				if (m_bIsResult)
				{
					memset(&s_reqans,0,sizeof(S_REQANS));
					s_reqans.raflag = 2;
					s_reqans.threadNo = m_mythreadNo;
					s_reqans.bufferlen = strlen(m_errMsg);
					strcpy(s_reqans.buffer,m_errMsg);
					m_publog.ToCharTime8(s_reqans.cutime);
					if (!m_sendData->PushReqAns(s_reqans))
					{
						m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"放队列失败");
					}
				}
				//m_sendData->putResult(false);
				m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"线程终止 %s",m_errMsg);
				break;
			}
			if (m_bIsResult)
			{
				m_publog.Update();
				memset(&s_reqans,0,sizeof(S_REQANS));
				s_reqans.raflag = 0;
				s_reqans.threadNo = m_mythreadNo;
				s_reqans.bufferlen = buflen;
				m_publog.ToCharTime8(s_reqans.cutime);
				if (buflen < MAXREQBUF)
				{
					memcpy(s_reqans.buffer,sendBuf,buflen);
				}
				else
				{
					xmlpack.setReturn(999,"缓冲区不够");
					s_reqans.bufferlen = sizeof(s_reqans.buffer);
					xmlpack.tobuffer(s_reqans.buffer,s_reqans.bufferlen);
				}
				
				if (!m_sendData->PushReqAns(s_reqans))
				{
					m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"放队列失败");
				}
			}

		}
		
		
		usetime1 = GetTickCount();
		if (!m_bIsConnect)
		{
			m_tcpSocket.Create(AF_INET);
			if (!m_tcpSocket.ConnectServer(m_commpara->ip,m_commpara->port))
			{
				sprintf(m_errMsg,"连接[%s][%d]失败!",m_commpara->ip,m_commpara->port);
				m_isRunning = true;
				if (m_bIsResult)
				{
					m_publog.Update();
					memset(&s_reqans,0,sizeof(S_REQANS));
					s_reqans.raflag = 2;
					s_reqans.threadNo = m_mythreadNo;
					s_reqans.bufferlen = strlen(m_errMsg);
					strcpy(s_reqans.buffer,m_errMsg);
					m_publog.ToCharTime8(s_reqans.cutime);
					if (!m_sendData->PushReqAns(s_reqans))
					{
						m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"放队列失败");
					}
				}
				//m_sendData->putResult(false);
				m_nConnectErrorNum++;
				m_nSendNum++;
				m_nFaildNum++;
				continue;
			}
			m_bIsConnect = true;
		}
		
		ret = m_tcpSocket.Send(sendBuf,buflen);
		if (ret!=buflen)
		{
			sprintf(m_errMsg,"发送[%s][%d]失败!",m_commpara->ip,m_commpara->port);
			m_isRunning = true;
			if (m_bIsResult)
			{
				m_publog.Update();
				memset(&s_reqans,0,sizeof(S_REQANS));
				s_reqans.raflag = 2;
				s_reqans.threadNo = m_mythreadNo;
				s_reqans.bufferlen = strlen(m_errMsg);
				strcpy(s_reqans.buffer,m_errMsg);
				m_publog.ToCharTime8(s_reqans.cutime);
				if (!m_sendData->PushReqAns(s_reqans))
				{
					m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"放队列失败");
				}
				if (ret<0) //连接已断开
				{
					m_tcpSocket.Close();
					m_bIsConnect = false;
				}
				else if (m_commpara->shortconnect) //为短连接
				{
					m_tcpSocket.Close();
					m_bIsConnect = false;
				}
				
			}
			//m_sendData->putResult(false);
			m_nFaildNum++;
			m_nSendNum++;
			m_nSendErrorNum++;
			continue;
		}
		buflen = sizeof(recvBuf);
		memset(recvBuf,0,sizeof(recvBuf));
		
		ret = m_tcpSocket.Receive(recvBuf,m_sendData->m_headLen,m_commpara->timeout);
		if (ret!=m_sendData->m_headLen)
		{
			sprintf(m_errMsg,"接收[%s][%d]应答包头失败!",m_commpara->ip,m_commpara->port);
			m_isRunning = true;
			if (m_bIsResult)
			{
				m_publog.Update();
				memset(&s_reqans,0,sizeof(S_REQANS));
				s_reqans.raflag = 2;
				s_reqans.threadNo = m_mythreadNo;
				s_reqans.bufferlen = strlen(m_errMsg);
				m_publog.ToCharTime8(s_reqans.cutime);
				strcpy(s_reqans.buffer,m_errMsg);
				if (!m_sendData->PushReqAns(s_reqans))
				{
					m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"放队列失败");
				}
			}
			if (ret<0) //连接已断开
			{
				m_tcpSocket.Close();
				m_bIsConnect = false;
			}
			else if (m_commpara->shortconnect) //为短连接
			{
				m_tcpSocket.Close();
				m_bIsConnect = false;
			}
			//m_sendData->putResult(false);
			m_nFaildNum++;
			m_nSendNum++;
			m_nRecvErrorNum++;
			continue;
		}
		m_publog.Update();
		recvBuf[ret]=0;
		m_sendData->m_pTool.LRtrim(recvBuf);
		if (!m_sendData->m_pTool.IsNumber(recvBuf))
		{
			sprintf(m_errMsg,"接收[%s][%d]应答包头不符[%s]!",m_commpara->ip,m_commpara->port,recvBuf);
			m_isRunning = true;
			if (m_bIsResult)
			{
				m_publog.Update();
				memset(&s_reqans,0,sizeof(S_REQANS));
				s_reqans.raflag = 2;
				s_reqans.threadNo = m_mythreadNo;
				s_reqans.bufferlen = strlen(m_errMsg);
				m_publog.ToCharTime8(s_reqans.cutime);
				strcpy(s_reqans.buffer,m_errMsg);
				if (!m_sendData->PushReqAns(s_reqans))
				{
					m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"放队列失败");
				}
				
			}
			if (m_commpara->shortconnect) //为短连接
			{
				m_tcpSocket.Close();
				m_bIsConnect = false;
			}
			//m_sendData->putResult(false);
			m_nFaildNum++;
			m_nSendNum++;
			m_nRecvErrorNum++;
			continue;
		}
		buflen = atoi(recvBuf);
		if (buflen > MAXREQBUF)
		{
			sprintf(m_errMsg,"接收[%s][%d]应答失败,缓冲不足!",m_commpara->ip,m_commpara->port);
			m_isRunning = true;
			if (m_bIsResult)
			{
				m_publog.Update();
				memset(&s_reqans,0,sizeof(S_REQANS));
				s_reqans.raflag = 2;
				s_reqans.threadNo = m_mythreadNo;
				s_reqans.bufferlen = strlen(m_errMsg);
				m_publog.ToCharTime8(s_reqans.cutime);
				strcpy(s_reqans.buffer,m_errMsg);
				if (!m_sendData->PushReqAns(s_reqans))
				{
					m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"放队列失败");
				}
			}
			m_tcpSocket.Close();
			m_bIsConnect = false;
			m_nFaildNum++;
			m_nSendNum++;
			m_nRecvErrorNum++;
			continue;
		}
		ret = m_tcpSocket.Receive(recvBuf,buflen,m_commpara->timeout);
		if (ret!=buflen)
		{
			sprintf(m_errMsg,"接收[%s][%d]应答包体失败!",m_commpara->ip,m_commpara->port);
			m_isRunning = true;
			if (m_bIsResult)
			{
				m_publog.Update();
				memset(&s_reqans,0,sizeof(S_REQANS));
				s_reqans.raflag = 2;
				s_reqans.threadNo = m_mythreadNo;
				s_reqans.bufferlen = strlen(m_errMsg);
				m_publog.ToCharTime8(s_reqans.cutime);
				strcpy(s_reqans.buffer,m_errMsg);
				if (!m_sendData->PushReqAns(s_reqans))
				{
					m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"放队列失败");
				}
			}
			m_tcpSocket.Close();
			m_bIsConnect = false;
// 			if (ret<0) //连接已断开
// 			{
// 				m_tcpSocket.Close();
// 				m_bIsConnect = false;
// 			}
// 			else if (m_commpara->shortconnect) //为短连接
// 			{
// 				m_tcpSocket.Close();
// 				m_bIsConnect = false;
// 			}
			//m_sendData->putResult(false);
			m_nFaildNum++;
			m_nSendNum++;
			m_nRecvErrorNum++;
			continue;
		}
		if (m_commpara->shortconnect) //为短连接
		{
			m_tcpSocket.Close();
			m_bIsConnect = false;
		}
		usetime2 = GetTickCount();
		m_usetime += usetime2 - usetime1;
		if (m_packtype == 3 && m_commpara->zip == 2) //xml包类型
		{
			//解压缩
			memcpy(sendBuf,recvBuf,buflen);
			unsigned int dlen = sizeof(sendBuf);
			if (!CBF_Tools::Uncompress((unsigned char *)sendBuf,dlen,(unsigned char *)recvBuf,buflen))
			{
				m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"解压缩失败");
			}
			else
			{
				memcpy(recvBuf,sendBuf,dlen);
				buflen = dlen;
			}
		}
		if (m_bIsResult)
		{
			m_publog.Update();
			memset(&s_reqans,0,sizeof(S_REQANS));
			s_reqans.usetime = usetime2 - usetime1;
			s_reqans.raflag = 1;
			s_reqans.threadNo = m_mythreadNo;
			m_publog.ToCharTime8(s_reqans.cutime);
			s_reqans.bufferlen = buflen;
			if (buflen < MAXREQBUF)
			{
				memcpy(s_reqans.buffer,recvBuf,buflen);
			}
			else
			{
				xmlpack.setReturn(999,"缓冲区不够");
				s_reqans.bufferlen = sizeof(s_reqans.buffer);
				xmlpack.tobuffer(s_reqans.buffer,s_reqans.bufferlen);
			}
			if (!m_sendData->PushReqAns(s_reqans))
			{
				m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"放队列失败");
			}
		}
		//m_sendData->putResult(true);
		m_nSuccessNum++;
		m_nSendNum++;
	
	}
	m_isRunning = false;
	if (m_log.isWrite(6))
	{
		sprintf(m_errMsg,"线程[%d]总发送次数[%d]成功次数[%d]失败次数[%d]",m_mythreadNo,m_nSendNum,m_nSuccessNum,m_nFaildNum);
		m_log.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"%s",m_errMsg);
	}

	m_tcpSocket.Close();

	return 0;
}
bool CSend2Server::Terminate()
{
	m_isRunning = false;
	return true;
}

void CSend2Server::SetParameter(int packtype,int sendnum, bool isrunning, CSendData *sndData,S_COMM_PARA *commpara,int threadNo)
{
	m_sendNum = sendnum;
	m_isRunning = isrunning;
	m_sendData = sndData;
	m_commpara = commpara;
	m_mythreadNo = threadNo;
	m_packtype = packtype;
	m_log.SetLogPara(5,"","testcomm.log");
	
}
