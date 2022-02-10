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
	m_nConntime=0;              //���ӽ���ʱ��
	m_nReadtime=0;              //�ϴζ�����ʱ��
	m_nSendtime=0;              //�ϴη�������ʱ�� 
	m_pBuProc.SetPara(this);
	m_bIsConnect = false;
	
	m_sRcvBuffer = (char *)malloc(BFBUFSIZE);            //�������ݻ���   
	
	sRcvBuffer = (char *)malloc(BFBUFSIZE);            //�������ݻ���   
	
	m_sReqAnsData=(char *)malloc(BFBUFSIZE);            //�������ݻ���   
	
	m_sSendBuffer=(char *)malloc(BFBUFSIZE);            //�������ݻ���   

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
	
	FD_SET(m_sock, &m_rset);//���뵽��select����

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
		if (result != 0) //�ж��¼�
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
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"����MSG_FREEʧ��"); 
		return -1;
	}
	int lasttime = time(NULL);
	if (IsSocketEvent(5))
	{
		ret = OnRecv(&data);
		if (ret <=0)
		{
			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"�������¼��������ݣ��ر����� [%d]",ret);
			OnClose("�������¼��������ݣ��ر�����");
			m_nConnNum = 6;//�˳�����������BPU,��Ϊ��֪�������Ӷ��ٴ���
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
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"����BFFPC %s %d ʧ��",(char *)m_pRes.g_sBccIp.c_str(),m_pRes.g_nPort);
		m_bIsConnect = false;
		return -1;
	}
	//������ģʽ
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
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"����MSG_FREEʧ��");
		m_tcpSocket.Close();
		m_bIsConnect = true;
		m_nConnNum++;
		return -1;
	}
	else
	{
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"����BFFPC %s %d �ɹ�",(char *)m_pRes.g_sBccIp.c_str(),m_pRes.g_nPort);
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
		m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"���Ӵ���[%d]����5�Σ��˳�BPU",m_nConnNum);
		return 1;
	}
	while (!m_bIsConnect)
	{
		ret = Connect();
		if (ret != 0 )
		{
			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"����BPCʧ�� ��������[%d]",m_nConnNum);
			//����ʧ�ܴ�������5�Σ��˳�bpu
			if (m_nConnNum >5 )
			{
				m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"���Ӵ���[%d]����5�Σ��˳�BPU",m_nConnNum);
				return 1;
			}
			//ÿ���������2��
			SLEEP_SECONDS(2);
		}
		break;
	}
	
	if (IsSocketEvent(1))
	{
		OnRecv();
	}
	else //���¼�����,��������
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
		//��ͷδ������
		ret = m_tcpSocket.Receive(m_sRcvBuffer+m_nRcvBufLen,BFBUFSIZE-m_nRcvBufLen);
		if (ret <= 0)
		{
			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"�������¼��������ݣ��ر����� [%d]",ret);
			OnClose("�������¼��������ݣ��ر�����");
			m_nConnNum = 6;//�˳�����������BPU,��Ϊ��֪�������Ӷ��ٴ���
			return ;
		}
		m_nRcvBufLen+=ret;
		m_nReadtime =time(NULL);
		
	}
	ResetData();

	//�յ������ݴ��ڰ�ͷ
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
				OnClose("���ӷ������ݷǷ�����BFFPC����");
				return ;
			}
			//˵������û�н�������
			return ;
		}
		//��¼������Ϣ
//		m_pLog->LogSysBin(LOG_DEBUG+1,m_sRcvBuffer,m_pHeadData.msgbuf.head.len+sizeof(S_MSG_HEAD));
		if (m_pHeadData.msgbuf.message == NULL)
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"m_pHeadData.msgbuf.messageΪNULL");
			return ;
		}
		memcpy(m_pHeadData.msgbuf.message->buffer,m_sRcvBuffer+sizeof(S_MSG_HEAD_FPC),m_pHeadData.msgbuf.head.len);
		m_pHeadData.msgbuf.message->buffer[m_pHeadData.msgbuf.head.len]=0;
		m_nDataLen = m_pHeadData.msgbuf.head.len;
		//���������־

		m_pLog->LogBin(LOG_DEBUG-1,__FILE__,__LINE__,"���յ���BFFPC����",m_pHeadData.msgbuf.message->buffer,m_pHeadData.msgbuf.head.len);

		
		//�ն��ˣ���ԭ�����¸�m_sRcvBuffer
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
	m_pLog->LogBin(LOG_DEBUG-1,__FILE__,__LINE__,"Ӧ���BFFPC������",m_pHeadData.msgbuf.message->buffer,m_pHeadData.msgbuf.head.len);				
	if (ret == length)
	{
		m_nSendtime = time(NULL);
	}
	else //ֹͣ
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��������ʧ�� m_sock=%d errmsg=%d %s",m_sock,m_tcpSocket.GetErrorNo(),m_tcpSocket.GetErrorMsg().c_str());

	}
}

void CBuLink::OnNoEvent()
{
	if (time(NULL) - m_nSendtime >= 5)
	{
		//5��û�з�������MSG_FREE����������
		S_BF_RSMSG_FPC data;
		memset(&data,0,sizeof(S_BF_RSMSG_FPC));
		data.msgbuf.message = NULL;
		data.msgbuf.head.len = 0;
		data.msgbuf.head.rtime = GetPid();
		data.msgbuf.head.msgtype = MSG_FREE;
		if (OnSend(&data)<0)
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"����MSG_FREEʧ��"); 
		}
	}
}

void CBuLink::OnEvent()
{
	int ret;
	int tmptime;
	ret = FD_ISSET(m_sock,&m_rset);
	if (ret != 0) //�ж��¼�
	{
		OnRecv();
	}
	else //�������޶��¼�
	{
		if (m_sock != INVALID_SOCKET)
		{
			tmptime = time(NULL) - m_nReadtime;
			if (tmptime > m_pRes.g_nDisconnectTime)
			{
				OnClose("���ӳ�ʱ��δ�ã��Ͽ�");
			}
		}
	}// end if (ret == 1) //�ж��¼�
}

void CBuLink::ResetData()
{
	memset(&m_pHeadData,0,sizeof(S_BF_RSMSG_FPC));
	m_pHeadData.msgbuf.message = (PCOMMSTRU_FPC)m_sReqAnsData;
}

void CBuLink::CallService()
{
	//��������
//	m_pBuProc.ProcessTx(m_sReqAnsData,m_nDataLen);
	
	BPCCOMMSTRU data;
	bzero(&data,sizeof(BPCCOMMSTRU));
	unsigned int dlen=BFBUFSIZE;
	if (m_pRes.g_IsZipEncrypt == 2)
	{
		if (!CBF_Tools::Uncompress((unsigned char *)data.sBuffer,dlen,(unsigned char *)m_sReqAnsData,m_nDataLen))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ݽ�ѹ������ ");
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
		data.sDBHead.d_Dinfo.d_nServiceNo = xmlpack.getHeadValueI("������");
	}
	else
	{
		xmlpack.newPackage();
		if (!xmlpack.setBinData(data.sBuffer,data.sDBHead.nLen))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"setBinData����");
			return ;
		}
		int len = BFBUFSIZE;
		xmlpack.tobuffer(data.sBuffer,len);
		data.sDBHead.nLen = len;
		data.sDBHead.d_Dinfo.d_nServiceNo = atoi(m_pRes.g_sMainFunc.c_str());
	}
	m_pLog->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"��������",data.sBuffer,data.sDBHead.nLen);
	m_pBuProc.Run(&data);

	

	int datalen;
	
	if (m_listAnsData.size()>1)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"FPUӦ���ж��¼�����һ����¼���� %d",m_listAnsData.size());
		//���ǽ���ͳһ��ڣ�ȡbindata����
		//������¼����ֻҪ��bindata��ϵ�һ�𷢸�fpc
		char *tmpbuffer = (char *)malloc(BFBUFSIZE);
		if (tmpbuffer== NULL)
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"malloc���� ");
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
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"����ѹ������ ");
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
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"FPUӦ������̫�� %d",tmpbufferlen);
				m_pLog->LogBin(LOG_ERROR_FAULT,__FILE__,__LINE__,"FPUӦ������̫��",tmpbuffer,tmpbufferlen);
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
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"FPU��Ӧ�� %d",m_listAnsData.size());
		m_pHeadData.msgbuf.head.msgtype = MSG_FREE;
		m_pHeadData.msgbuf.head.len  = 0;
		OnSend();
		return;
	}
	else
	{
		//���ǽ���ͳһ��ڣ�ȡbindata����
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
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"����ѹ������ ");
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
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"FPUӦ������̫�� %d",m_listAnsData[0].sDBHead.nLen);
				m_pLog->LogBin(LOG_ERROR_FAULT,__FILE__,__LINE__,"FPUӦ������̫��",m_listAnsData[0].sBuffer,m_listAnsData[0].sDBHead.nLen);
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
		m_pLog->LogBin(LOG_DEBUG,__FILE__,__LINE__,"��������",data->msgbuf.message->buffer,data->msgbuf.head.len);	
	}
	else
	{
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"����MSG_FREE");	
	}
	if (ret == length)
	{
		m_nSendtime = time(NULL);
		return 0;
	}
	else //ֹͣ
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��������ʧ�� m_sock=%d errmsg=%d %s",\
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
		//��ͷδ������
		ret = m_tcpSocket.Receive(sRcvBuffer+nRcvBufLen,BFBUFSIZE-nRcvBufLen);
		if (ret <= 0)
		{
			return ret;
		}
		nRcvBufLen+=ret;
		m_nReadtime =time(NULL);
		
	}
	
	//�յ������ݴ��ڰ�ͷ
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
				OnClose("���ӷ������ݷǷ�����BFFPC����");
				return -1;
			}
			//˵������û�н�������
			return 0;
		}
		//��¼������Ϣ
//		m_pLog->LogSysBin(LOG_DEBUG+1,sRcvBuffer,data->msgbuf.head.len+sizeof(S_MSG_HEAD));
		if (data->msgbuf.message == NULL)
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"data->msgbuf.messageΪNULL");
			return -1;
		}
		if (data->msgbuf.head.len>0)
		{
			memcpy(data->msgbuf.message->buffer,sRcvBuffer+sizeof(S_MSG_HEAD_FPC),data->msgbuf.head.len);
			data->msgbuf.message->buffer[data->msgbuf.head.len]=0;
		}
		
		//���������־
		if (m_pLog->isWrite(LOG_PROMPT))
		{
			m_pLog->LogBin(LOG_WARNNING,__FILE__,__LINE__,"���յ���BFFPC����",data->msgbuf.message->buffer,data->msgbuf.head.len);
		}
		//�ն��ˣ���ԭ�����¸�m_sRcvBuffer
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
	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�޴˹���");
	return false;
}
/// ������: isSetGPara2
/// ���  : ������ 2008-12-30 9:36:10
/// ����  : virtual bool true�ɹ�falseʧ��
/// ����  : bool setflag=true
/// ����  : ����ȫ�ֱ���2,���������򷵻�false
bool CBuLink::SetGPara2(bool setflag)
{
	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�޴˹���");
	return false;
}


/// ������: isSetGPara3
/// ���  : ������ 2008-12-30 9:36:13
/// ����  : virtual bool true�ɹ�falseʧ��
/// ����  : bool setflag=true
/// ����  : ����ȫ�ֱ���3,���������򷵻�false
bool CBuLink::SetGPara3(bool setflag)
{
	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�޴˹���");
	return false;
}



// ������: AnsData
// ���  : ������ 2013-4-23 15:16:43
// ����  : virtual int -1ʧ�� 0�ɹ�
// ����  : S_BPC_RSMSG *data
// ����  : Ӧ������,��BPUҵ�������ԭ�ӵ���
int CBuLink::AnsData(PBPCCOMMSTRU data)
{
	m_listAnsData.push_back(*data);
	return 0;
}

// ������: DPTrans
// ���  : ������ 2013-4-23 15:11:48
// ����  : virtual int -1ʧ�� 0�ɹ�
// ����  : PBPCCOMMSTRU calldata
// ����  : ת��������������
int CBuLink::DPTrans(PBPCCOMMSTRU calldata)
{
	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�޴˹���");
	return -1;
}

// ������: BpuCall
// ���  : ������ 2013-4-23 15:09:13
// ����  : virtual int -1ʧ�� 0�ɹ�
// ����  : PBPCCOMMSTRU calldata
// ����  : LIST_BPC_RSMSG &ansdata
// ����  : ����BPU�ڵĹ��ܣ������̿���ʹ��
int CBuLink::BpuCall(PBPCCOMMSTRU calldata,LIST_BPC_RSMSG &ansdata)
{
	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�޴˹���");
	return -1;
}

// ������: BpcCall
// ���  : ������ 2013-4-23 15:08:18
// ����  : virtual int -1ʧ�� 0�ɹ�
// ����  : PBPCCOMMSTRU calldata
// ����  : LIST_BPC_RSMSG &ansdata
// ����  : ����bpc����
int CBuLink::BpcCall(PBPCCOMMSTRU calldata,LIST_BPC_RSMSG &ansdata,unsigned int stimeout)
{
	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�޴˹���");
	return -1;
}

// ������: ExtCall
// ���  : ������ 2013-4-23 15:01:25
// ����  : virtual int -1ʧ�� 0�ɹ�
// ����  : PBPCCOMMSTRU calldata
// ����  : vector<S_BPC_RSMSG> &ansdata
// ����  : ���BPC
int CBuLink::ExtCall(PBPCCOMMSTRU calldata,LIST_BPC_RSMSG &ansdata,unsigned int stimeout)
{
	m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�޴˹���");
	return -1;
}


// ������:  InitLink
// ���  : ������ 2013-4-23 14:44:14
// ����  : virtual bool true�ɹ� falseʧ��
// ����  : const char *confile
// ����  : ��ʼ���������� 
bool  CBuLink::InitLink(const char *confile)
{
	if (!m_pRes.Init((char *)confile))
	{
		printf("��ʼ��ʧ�� %s\n",m_pRes.m_errMsg.c_str());
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
