// SocketInfo.cpp: implementation of the CSocketInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "SocketInfo.h"
#include "BF_Zip.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CSocketInfo::CSocketInfo()
{
	memset(m_sAddress,0,sizeof(m_sAddress));         //��������������IP
	memset(m_sSendBuffer,0,sizeof(m_sSendBuffer)); 
	
	ResetInfo();
	m_pMemPool = NULL;
	m_pStatInfo = NULL;
	m_nCloseTime = 0;
	m_nQueueTime = 60;
	m_pSendQueue = NULL;
	m_sendNum = 10;
	m_pRes = NULL;
}

CSocketInfo::~CSocketInfo()
{
	if (m_sock != INVALID_SOCKET)
	{
		closesocket(m_sock);
	}
	m_pMemPool = NULL;
	m_pRes = NULL;
	m_pStatInfo = NULL;
}

void CSocketInfo::SetGlobalVar(CGlobalVar *pVar,CBF_BufferPool *pool,int deltime)
{
	m_pRes = pVar;
	m_pMemPool = pool;
	m_pStatInfo = &pVar->g_pDrebStatInfo;
	m_nLogLevel = m_pRes->g_nLogLevel;
	m_sLogFile = m_pRes->g_sLogFileName;
	m_nQueueTime  = deltime;
	m_log = &(m_pRes->g_pLog);
	

	ResetInfo();
	m_nCloseTime = 0;
}

bool CSocketInfo::IsCanSelect()
{
	if (m_sock != INVALID_SOCKET)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void CSocketInfo::OnClose(const char *msg,const char *filename,int fileline)
{
	CBF_PMutex pp(&m_mutex);
	m_datetime.Update();
	m_datetime.ToCharDate(m_cCloseDate);
	if (m_nType == SOCK_CLI || m_nType == SOCK_UNKNOW)
	{
		//��ն���
		m_log->LogMp(LOG_ERROR_GENERAL,filename,fileline,"%s,�ر�����[%s:%d] index[%d] type[%s]",msg,m_sAddress,m_sock,m_index,GetSockType(m_nType).c_str());
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
		ClearQueue();
		ResetInfo();
	}
	else 
	{
		m_log->LogMp(LOG_ERROR_GENERAL,filename,fileline,"%s,�ر�����[%s:%d] index[%d] type[%s]",msg,m_sAddress,m_sock,m_index,GetSockType(m_nType).c_str());
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
		//�������е�һ�����ݰ��Ƿ��Ͳ��֣�������ɾ�������������������ݰ�
		CheckFirstData();

		//20170205 songfree add ���ǰѶ��������������ɣ��Ժ���Կ��ǰ����ݷŻط������
		ClearQueue();
		//20170205 songfree add end 

		ResetSInfo();
		if (m_nType == SOCK_TODREB) //��������������
		{
			m_s_center.bNeedConnect = true; //��Ҫ����
		}
		m_pSendQueue = NULL;

	}
	m_nCloseTime = time(NULL);
	
}

void CSocketInfo::OnShutdown()
{
	closesocket(m_sock);
	//��ն���
	ClearQueue();
	ResetInfo();
	m_nCloseTime = 0;
}

bool CSocketInfo::IsFree()
{
	if (m_sock != INVALID_SOCKET)
	{
		return false;
	}
	if (m_nType == SOCK_LISTEN)//������
	{
		return false;
	}
	if (m_nType == SOCK_TODREB ) //������ͨѶƽ̨������
	{
		return false;
	}
	return true;
}

void CSocketInfo::ClearQueue()
{
	if (m_pSendQueue== NULL)
	{
		return;
	}
	RDQUEUE::iterator prn;

	while (m_pSendQueue->m_qSendData.datas.size()>0)	
	{
		prn=m_pSendQueue->m_qSendData.datas.begin();
		if (prn == m_pSendQueue->m_qSendData.datas.end())
		{
			break;
		}
		
		if ((*prn)->message.head.cCmd != CMD_PING && (*prn)->message.head.cCmd != CMD_CONNECT && (*prn)->message.head.cCmd != CMD_REGSERVICE)
		{
			m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"��ն��� index[%d] type[%s]����Ϣ DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] ҵ�����ݳ���[%d]",\
				m_index,GetSockType(m_nType).c_str(),\
				GetDrebCmdType((*prn)->message.head.cCmd).c_str(),(*prn)->message.head.cNextFlag,\
				(*prn)->message.head.cRaflag,(*prn)->message.head.d_Dinfo.d_nServiceNo,(*prn)->message.head.s_Sinfo.s_nNodeId,\
				(*prn)->message.head.s_Sinfo.s_cNodePrivateId,(*prn)->message.head.s_Sinfo.s_nDrebSerial,(*prn)->message.head.nLen);
		}
		else
		{
			m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"��ն��� index[%d] type[%s]����Ϣ DREB����[%s]  ҵ�����ݳ���[%d]",\
				m_index,GetSockType(m_nType).c_str(),GetDrebCmdType((*prn)->message.head.cCmd).c_str(),(*prn)->message.head.nLen);
		}
		m_pRes->g_pDataLog.LogDreb(LOG_ERROR_FAULT,&((*prn)->message),true);
		m_pMemPool->PoolFree((*prn));
		m_pSendQueue->DeleteSend(prn);
	}
	
}

bool CSocketInfo::Listen(int port,bool blockflag,int queuenum)
{
//	if (!m_tcpSocket.Create(AF_UNIX,blockflag))
	if (!m_tcpSocket.Create(AF_INET,blockflag))
	{
		CBF_Tools::StringCopy(m_errMsg,255,m_tcpSocket.GetErrorMsg().c_str());
		return false;
	}
	m_s_listen.nPort = port;
	if (!m_tcpSocket.Listen(m_s_listen.nPort,queuenum))
	{
		sprintf(m_errMsg,"�����˿�[%d]ʧ��,�������д�С%d",port,queuenum);
		return false;
	}
	
	m_nType   =  SOCK_LISTEN;      //����
	m_sock = m_tcpSocket.GetSocket();

	m_bChecked = true;
	m_bNeedread = true;

	return true;
}

bool CSocketInfo::ListenIpc(int port,bool blockflag,int queuenum)
{
	if (!m_tcpSocket.Create(AF_UNIX,blockflag))
	{
		CBF_Tools::StringCopy(m_errMsg,255,m_tcpSocket.GetErrorMsg().c_str());
		return false;
	}
	m_s_listen.nPort = port;
	if (!m_tcpSocket.Listen(m_s_listen.nPort,queuenum))
	{
		sprintf(m_errMsg,"�����˿�[%d]ʧ��,�������д�С%d",port,queuenum);
		return false;
	}
	
	m_nType   =  SOCK_LISTEN;      //����
	m_sock = m_tcpSocket.GetSocket();
	
	m_bChecked = true;
	m_bNeedread = true;
	
	return true;
}

void CSocketInfo::ResetInfo()
{
	memset(m_sAddress,0,sizeof(m_sAddress));
	memset(&m_s_center,0,sizeof(DREB_CENTER));
	memset(&m_s_listen,0,sizeof(DREB_LISTEN));
	memset(&m_s_conn,0,sizeof(DREB_CONN));
	memset(&m_s_srv,0,sizeof(DREB_SRV));

	//��������
	m_nBeatheartcount=0;        //������������
	//socket��������
	m_sock = INVALID_SOCKET;                   //socket id
	m_nType = SOCK_UNKNOW;                  //socket type SOCK_UNKNOW SOCK_CLI SOCK_DREB SOCK_SRV SOCK_TODREB
	m_bChecked=false;               //�Ƿ�����֤������ȷ�Ϲ���������
	m_bNeedread=false;              //�Ƿ���Ҫ������
	
	m_nReadtime= 0;             //�ϴζ�����ʱ��
//	m_nConntime=0;              //���ӽ���ʱ��
	m_nSendtime=0;              //�ϴη�������ʱ�� 
	m_nPingTime = 0;
	
	m_nSend_packs=0;          //�����ӽ������͵����ݰ�
	m_nRcv_packs=0;           //�����ӽ�������յ����ݰ�
	m_nSendBytes=0;           //�����ӽ������͵��ֽ���
	m_nRcvBytes=0;            //�����ӽ�������յ��ֽ���
	
	//���ͽ�������	
	memset(m_sRcvBuffer,0,sizeof(m_sRcvBuffer));            //�������ݻ���   
	m_nRcvBufLen=0;                //���յ����ݳ���  �ɶ�ν���һ�����ݰ�

	memset(m_cCreateDate,0,sizeof(m_cCreateDate));//���ӽ�������
	m_nConntime = 0;
	memset(m_cCloseDate,0,sizeof(m_cCloseDate));//���ӹر�����
	m_nCloseTime = 0;
	m_sendNum = 10;
}

int CSocketInfo::OnRecv()
{
	int ret;
	m_tcpSocket.AttachSocket(m_sock,"");
	if (m_nRcvBufLen >=0 && m_nRcvBufLen<DREBBUFSIZE)
	{
		//��ͷδ������
		ret = m_tcpSocket.Receive(m_sRcvBuffer+m_nRcvBufLen,DREBBUFSIZE-m_nRcvBufLen);
		if (ret <= 0)
		{
			return ret;
		}
		m_nRcvBufLen+=ret;
		m_nReadtime =time(NULL);
		m_nPingTime = m_nReadtime;
		//��¼ͳ����Ϣ
		m_nRcvBytes+=ret;
		return ret;
	}
	return 0;
}

int CSocketInfo::OnSend()
{
	int ret;
	if (m_pSendQueue == NULL)
	{
		m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"����index=%d type=%d ���Ͷ���ΪNULL",m_index,m_nType);
		return -1;
	}
	if (m_sock == INVALID_SOCKET)
	{
		m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"�����ѶϿ�");
		return -2;
	}
	if (!m_pSendQueue->NeedSend())
	{
		return -3;
	}
	CBF_PMutex pp(&m_mutex);
	RDQUEUE::iterator prq,prn;
	int sendNum=0;
    if (m_pSendQueue == NULL)
    {
        m_log->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "����index=%d type=%d ���Ͷ���ΪNULL", m_index, m_nType);
        return -1;
    }
	for (prn=m_pSendQueue->m_qSendData.datas.begin();prn!=m_pSendQueue->m_qSendData.datas.end();)
	{
		prq = prn;
		prn++;
		if ( time(NULL) - (*prq)->msghead.rtime > m_nQueueTime && (*prq)->msghead.nTotalLen == (*prq)->msghead.nSendLen )
		{
			m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"�����ڶ��г�ʱ index[%d] type[%s]����Ϣ s_nDrebSerial[%d]  ���ݳ���[%d]",\
					m_index,GetSockType(m_nType).c_str(),(*prq)->msghead.serial,(*prq)->msghead.nTotalLen);
			m_pMemPool->PoolFree((*prq));
			m_pSendQueue->DeleteSend(prq);	
			continue;
		}
		if ((*prq)->msghead.nSendLen <=0)
		{
			m_pMemPool->PoolFree((*prq));
			m_pSendQueue->DeleteSend(prq);	
			continue;
		}
		m_tcpSocket.AttachSocket(m_sock,"");
		ret = m_tcpSocket.Send((char *)&((*prq)->message)+((*prq)->msghead.nTotalLen-(*prq)->msghead.nSendLen),(*prq)->msghead.nSendLen);
		if (ret == (*prq)->msghead.nSendLen)
		{
			//ͳ����Ϣ
			m_pStatInfo->ReportSendByte(m_nType,ret);
			m_pStatInfo->ReportSendPack(m_nType,1);
			
			m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���ͳɹ� index[%d] type[%s]����Ϣ s_nDrebSerial[%d]  ���ݳ���[%d]",\
					m_index,GetSockType(m_nType).c_str(),(*prq)->msghead.serial,(*prq)->msghead.nSendLen);
			m_pMemPool->PoolFree((*prq));
			// ����ɹ�,ֹͣ
			m_pSendQueue->DeleteSend(prq);	
			m_nSendtime = time(NULL);
			
		}
		else 
		{
			if (ret >0)
			{
				(*prq)->msghead.nSendLen = (*prq)->msghead.nSendLen -ret;
				m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���Ͳ������� [%d]��׼���ط� index[%d] type[%s]����Ϣ s_nDrebSerial[%d]  ���ݳ���[%d]",\
					ret,m_index,GetSockType(m_nType).c_str(),(*prq)->msghead.serial,(*prq)->msghead.nSendLen);
				
			}
			else
			{
				m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"����ʧ�ܣ�׼���ط� index[%d] type[%s]����Ϣ s_nDrebSerial[%d]  ���ݳ���[%d]",\
					m_index,GetSockType(m_nType).c_str(),(*prq)->msghead.serial,(*prq)->msghead.nSendLen);
			}
			return sendNum;
		}
	}
	

	return sendNum;
}
int CSocketInfo::SendMsg(S_DREB_RSMSG *msg,bool sendimmediate)
{
	int ret;
	if (m_sock == INVALID_SOCKET)
	{
		m_pMemPool->PoolFree(msg);
		m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���� [%d] �ѹر�",m_index);
		return -1;
	}
	if (m_nType == SOCK_CLI || m_nType == SOCK_UNKNOW)
	{
		//�Ƚ�����ʱ���
		if (msg->message.head.s_Sinfo.s_nTimestamp != m_nConntime)
		{
			if (msg->message.head.cCmd != CMD_PING && msg->message.head.cCmd != CMD_CONNECT && msg->message.head.cCmd != CMD_REGSERVICE)
			{
				m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"����ʱ���������ɾ�� index[%d] type[%s]����Ϣ DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] ҵ�����ݳ���[%d]",\
					m_index,GetSockType(m_nType).c_str(),\
					GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
					msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
					msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,msg->message.head.nLen);
			}
			else
			{
				m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"����ʱ���������ɾ�� index[%d] type[%s]����Ϣ DREB����[%s]  ҵ�����ݳ���[%d]",\
					m_index,GetSockType(m_nType).c_str(),GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.nLen);
			}
			m_pRes->g_pDataLog.LogDreb(LOG_ERROR_FAULT,&(msg->message),true);
			m_pMemPool->PoolFree(msg);
			//��¼��־
			return -1;
		}
	}
	
	if (msg->message.head.nLen > 200 && msg->message.head.cZip == 0 ) //��������δѹ��
	{
		if (m_pRes->g_nZip == 1)
		{
#ifndef  WIN64
			if (msg->message.head.cCmd == CMD_DPCALL || msg->message.head.cCmd == CMD_DPACALL || \
				msg->message.head.cCmd == CMD_DPPOST || msg->message.head.cCmd == CMD_DPPUSH || \
				msg->message.head.cCmd == CMD_DPBC   || msg->message.head.cCmd == CMD_DPABC || \
				msg->message.head.cCmd == CMD_MONITOR_DREB || msg->message.head.cCmd == CMD_MONITOR_BPC )
			{  //��Ϊ����������ʱ����ѹ��
				
				unsigned int dlen = sizeof(m_sSendBuffer)-1;
				if (!CBF_Tools::Compress((unsigned char *)m_sSendBuffer,dlen,(unsigned char *)(msg->message.buffer),msg->message.head.nLen))
				{
					m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"����ѹ��ʧ��");
					m_pRes->g_pDataLog.LogDreb(LOG_ERROR_FAULT,&(msg->message),true);
					m_pMemPool->PoolFree(msg);
					return -101;
				}
				else
				{
					m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"����ѹ���ɹ� %d->%d",msg->message.head.nLen,dlen);
					msg->message.head.nLen = dlen;
					memcpy(msg->message.buffer,m_sSendBuffer,dlen);
					msg->message.head.cZip = 2;
				}
				
			}
#endif
		}
		
	}
	if (msg->message.head.cCmd != CMD_PING && msg->message.head.cCmd != CMD_CONNECT && msg->message.head.cCmd != CMD_REGSERVICE)
	{
		m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���� index[%d] type[%s]����Ϣ DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] cZip[%d] ҵ�����ݳ���[%d]",\
			m_index,GetSockType(m_nType).c_str(),\
			GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
			msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
			msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
			msg->message.head.cZip,msg->message.head.nLen);
	}
	else
	{
		m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���� index[%d] type[%s]����Ϣ DREB����[%s]  ҵ�����ݳ���[%d]",\
			m_index,GetSockType(m_nType).c_str(),GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.nLen);
	}
	m_pRes->g_pDataLog.LogDreb(LOG_DEBUG,&(msg->message),false);

	msg->msghead.nTotalLen = msg->message.head.nLen+DREBHEADLEN;
	msg->msghead.nSendLen = msg->msghead.nTotalLen;
	msg->msghead.serial =    msg->message.head.s_Sinfo.s_nDrebSerial;

	if (m_pRes->g_nCrcFlag == 1)
	{
		m_pDrebEndian.Endian2CommGenCrc(&(msg->message.head));
	}
	else
	{
		m_pDrebEndian.Endian2Comm(&(msg->message.head));
	}

	m_tcpSocket.AttachSocket(m_sock,"");
	CBF_PMutex pp(&m_mutex);

	if (sendimmediate)
	{

		ret = m_tcpSocket.Send((char *)&(msg->message),msg->msghead.nTotalLen);
		if (ret == msg->msghead.nTotalLen)
		{
			
			//ͳ����Ϣ
			m_pStatInfo->ReportSendByte(m_nType,ret);
			m_pStatInfo->ReportSendPack(m_nType,1);
			m_pMemPool->PoolFree(msg);
			// ����ɹ�,ֹͣ
			m_nSendtime = time(NULL);
			m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"����s_nDrebSerial[%d]���",msg->msghead.serial);
		}
		else 
		{
			if (ret > 0)
			{
				msg->msghead.nSendLen = msg->msghead.nTotalLen - ret;
				m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"���Ͳ��ֳɹ� index[%d] type[%s]����Ϣ s_nDrebSerial[%d]  ���ݳ���[%d]",\
					m_index,GetSockType(m_nType).c_str(),msg->msghead.serial,msg->msghead.nTotalLen);
			}
			else
			{
				m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"����ʧ�� index[%d] type[%s]����Ϣ s_nDrebSerial[%d]  ���ݳ���[%d]",\
					m_index,GetSockType(m_nType).c_str(),msg->msghead.serial,msg->msghead.nTotalLen);
			}
			msg->msghead.sendnum = 0;
			//��������������ݿ��ܾͻ�������
			if (!NeedSend())
			{
				PutSendMsg(msg);
			}
			else
			{
				OnClose("ֱ�������ӷ������ݲ��ɹ��������Ӷ�������������",__FILE__,__LINE__);
			}
		}
		return 0;
	}
	else
	{
		if (NeedSend())
		{
			m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���Ͷ����������ݣ�������� ���ݳ���[%d]",msg->msghead.nTotalLen);
			PutSendMsg(msg);
		}
		else  //ֱ�ӷ���
		{
			
			ret = m_tcpSocket.Send((char *)&(msg->message),msg->msghead.nTotalLen);
			if (ret == msg->msghead.nTotalLen)
			{
				//ͳ����Ϣ
				m_pStatInfo->ReportSendByte(m_nType,ret);
				m_pStatInfo->ReportSendPack(m_nType,1);
				m_pMemPool->PoolFree(msg);
				// ����ɹ�,ֹͣ
				m_nSendtime = time(NULL);
				m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"����s_nDrebSerial[%d]���",msg->msghead.serial);
				
			}
			else 
			{
				if (ret > 0)
				{
					msg->msghead.nSendLen = msg->msghead.nTotalLen - ret;
				}
				m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"���� index[%d] type[%s]����Ϣ s_nDrebSerial[%d]  ���ݳ���[%d]",\
					m_index,GetSockType(m_nType).c_str(),msg->msghead.serial,msg->msghead.nTotalLen);
				msg->msghead.sendnum = 0;
				PutSendMsg(msg);
			}
			
// 			if (msg->message.head.cRaflag == 0)
//			{
//				m_pRes->PutTime();
//			}	
		}
	
	}
	
	return 0;	
}
int CSocketInfo::PutSendMsg(S_DREB_RSMSG *msg)
{
	if (m_pSendQueue == NULL)
	{
		return -2;
	}
	int ret;
	ret = m_pSendQueue->PutSendMsg(msg);
	if (ret<0)
	{
		m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"���������� index[%d] type[%s]����Ϣ DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] cZip[%d] ҵ�����ݳ���[%d]",\
			m_index,GetSockType(m_nType).c_str(),\
			GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
			msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
			msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
			msg->message.head.cZip,msg->message.head.nLen);
		m_pMemPool->PoolFree(msg);
		msg = NULL;
		return -1;
	}
	return 0;
}



int CSocketInfo::GetRecvData(S_DREB_RSMSG *msg)
{
	//�յ������ݴ��ڰ�ͷ
	if (m_nRcvBufLen >= DREBHEADLEN )
	{
		//��¼��ʼʱ�䣬΢��
		
//		m_pRes->PutTime();
		//�����ֽ�����У��CRC
		memcpy(&(msg->message.head),m_sRcvBuffer,DREBHEADLEN);

		if (m_pRes->g_nCrcFlag == 1)
		{
			if (!m_pDrebEndian.Endian2LocalHostChkCrc(&(msg->message.head)))
			{
				//OnClose("CRC����");
				return -1;
			}
		}
		else
		{
			m_pDrebEndian.Endian2LocalHost(&(msg->message.head));
		}
		if (msg->message.head.nLen <0 || msg->message.head.nLen> DREBDATASIZE)
		{
			m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"index[%d] ����ͷ[%d]���Ȳ���",m_index,msg->message.head.nLen);
			return -1;
		}
		if (m_nRcvBufLen < msg->message.head.nLen + DREBHEADLEN)
		{
			//˵������û�н�������
			return 0;
		}
		if (msg->message.head.nLen>0)
		{
			memcpy(msg->message.buffer,m_sRcvBuffer+DREBHEADLEN,msg->message.head.nLen);
		}
		//�ն��ˣ���ԭ�����¸�m_sRcvBuffer
		m_nRcvBufLen = m_nRcvBufLen-DREBHEADLEN-msg->message.head.nLen;
		for (int i=0; i<m_nRcvBufLen; i++)
		{
			m_sRcvBuffer[i]=m_sRcvBuffer[i+DREBHEADLEN+msg->message.head.nLen];
		}
		
//		m_pRes->PutTime();
		
		//��¼ͨѶƽ̨ͳ����Ϣ
		m_nRcv_packs++;
		m_pStatInfo->ReportRcvByte(m_nType,DREBHEADLEN+msg->message.head.nLen);
		m_pStatInfo->ReportRcvPack(m_nType,1);

		if (m_nType == SOCK_CLI || m_nType == SOCK_SRV ) //��ͨѶƽ̨��Ϣ��ֵ
		{
			if (msg->message.head.cRaflag == 0 )
			{
				msg->message.head.s_Sinfo.s_nDrebSerial = m_pRes->getTick();//ͨѶƽ̨��ˮ����־Ψһ����
				//20180328 songfree modi �ж��Ƿ�Ϊת�ƣ����ǲ������¸�ֵ
				if (msg->message.head.a_Ainfo.a_cNodePrivateId != 100)
				{
					msg->message.head.s_Sinfo.s_cNodePrivateId = m_pRes->g_nDrebPrivateId;
					msg->message.head.s_Sinfo.s_nNodeId = m_pRes->g_nDrebId;
					msg->message.head.s_Sinfo.s_cSvrPrivateId = m_s_srv.cSvrPrivateId;
					msg->message.head.s_Sinfo.s_nSvrMainId = m_s_srv.nSvrMainId;
					msg->message.head.s_Sinfo.s_nIndex = m_index;
					msg->message.head.s_Sinfo.s_nTimestamp = m_nConntime;
				}
				
				
				//�ô��ֶ���Ϊ·��·������������16����
				msg->message.head.a_Ainfo.a_nNodeId =0;
			}
			else
			{
				msg->message.head.a_Ainfo.a_nNodeId = m_pRes->g_nDrebId;
				msg->message.head.a_Ainfo.a_cNodePrivateId = m_pRes->g_nDrebPrivateId;
			}
		}
		

		//���������־
		if (msg->message.head.cCmd != CMD_PING && msg->message.head.cCmd != CMD_CONNECT && msg->message.head.cCmd != CMD_REGSERVICE)
		{
			if (m_log->isWrite(LOG_DEBUG))
			{
				m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�������� index[%d] type[%s]����Ϣ DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] cZip[%d] ҵ�����ݳ���[%d]",\
					m_index,GetSockType(m_nType).c_str(),\
					GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
					msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
					msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
					msg->message.head.cZip,msg->message.head.nLen);
			}
			
		}
		else
		{
			if (m_log->isWrite(LOG_DEBUG+1))
			{
				m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"�������� index[%d] type[%s]����Ϣ DREB����[%s]  ҵ�����ݳ���[%d]",\
					m_index,GetSockType(m_nType).c_str(),GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.nLen);
			}
		}
		
		if (msg->message.head.cRaflag == 1 && (msg->message.head.cCmd == CMD_DPABC || msg->message.head.cCmd == CMD_DPPUSH))
		{
			m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"CMD_DPABC��CMD_DPPUSH��Ӧ��,����");
		}
		else
		{

			m_pRes->g_pDataLog.LogDreb(LOG_DEBUG,&(msg->message),false);
		}
		
// 		if (msg->message.head.cRaflag == 0)
// 		{
// 			m_pRes->PutTime();
// 		}
		
		return msg->message.head.nLen+DREBHEADLEN;
	}
	return 0;
}


void CSocketInfo::SetToDreb(const char *ip, int port,int brate)
{
	memset(&m_s_center,0,sizeof(DREB_CENTER));
	strcpy(m_sAddress,ip);
	strcpy(m_s_center.sAddress,ip);
	m_s_center.bNeedConnect = true;
	m_s_center.nPort = port;
	m_s_center.nbandwidth = brate;

	m_nType = SOCK_TODREB;
	m_nConntime = 0;
	m_nReadtime = 0;
	m_nSendtime = 0;
	m_nCloseTime = 0;
	m_bChecked = false;
	m_bNeedread = false;
	m_nBeatheartcount =0;
}


void CSocketInfo::SetClient()
{
	m_nType = SOCK_CLI;
	m_bChecked = true;
	m_bNeedread = true;
	m_nSendtime = time(NULL);
	m_nReadtime = time(NULL);
	m_nBeatheartcount =0;
}

void CSocketInfo::SetDreb(unsigned short NodeId, char cNodePrivateId,int bdrate,CSendQueue *sque)
{
	m_nType = SOCK_DREB;
	m_bChecked = true;
	m_bNeedread = true;
	m_nSendtime = time(NULL);
	m_nReadtime = time(NULL);
	m_s_conn.nNodeId = NodeId;
	m_s_conn.cNodePrivateId = cNodePrivateId;
	m_s_conn.nbandwidth = bdrate;
	m_nBeatheartcount =0;
	if (m_pSendQueue != NULL)
	{
		delete m_pSendQueue;
		m_pSendQueue = NULL;
	}
	m_pSendQueue = sque;
}

void CSocketInfo::SetSvr(unsigned int nSvrMainId, char cSvrPrivateId, CSendQueue *sque)
{
	m_nType = SOCK_SRV;
	m_bChecked = true;
	m_bNeedread = true;
	m_nSendtime = time(NULL);
	m_nReadtime = time(NULL);
	m_s_srv.cSvrPrivateId = cSvrPrivateId;
	m_s_srv.nSvrMainId = nSvrMainId;
	m_nBeatheartcount =0;
	if (m_pSendQueue != NULL)
	{
		delete m_pSendQueue;
		m_pSendQueue = NULL;
	}
	m_pSendQueue = sque;
	m_pSendQueue->m_pMemPool = m_pMemPool;
}

void CSocketInfo::SetAccept(SOCKET_HANDLE sock,const char *address)
{
	ResetInfo();
	m_nType = SOCK_UNKNOW;
	m_sock = sock;
	m_bChecked = false;
	m_bNeedread = true;
	m_nConntime = time(NULL);
	m_nSendtime = time(NULL);
	m_nReadtime = time(NULL);
	strcpy(m_sAddress,address);
	m_datetime.Update();
	m_datetime.ToCharDate(m_cCreateDate);
	m_nBeatheartcount =0;
	if (m_pSendQueue == NULL)
	{
		m_pSendQueue = new CSendQueue();
		m_pSendQueue->m_pMemPool = m_pMemPool;
	}
}
void CSocketInfo::SetConnectDreb(SOCKET_HANDLE sock,int conntime,CSendQueue *sque)
{
	m_nType = SOCK_TODREB;
	m_sock = sock;
	m_bChecked = true;
	m_bNeedread = true;
	m_datetime.Update();
	m_datetime.ToCharDate(m_cCreateDate);
	m_nBeatheartcount = 0;
	if (m_pSendQueue != NULL)
	{
		delete m_pSendQueue;
		m_pSendQueue = NULL;
	}
	m_pSendQueue = sque;
	m_pSendQueue->m_pMemPool = m_pMemPool;
	
}

bool CSocketInfo::NeedSend()
{
	if (m_pSendQueue != NULL)
	{
		return m_pSendQueue->NeedSend();
	}
	else
	{
		return false;
	}
}


bool CSocketInfo::NeedConnect()
{
	return m_s_center.bNeedConnect;
}

bool CSocketInfo::NeedRead()
{
	return m_bNeedread;
}

void CSocketInfo::ResetSInfo()
{
	//��������
	m_nBeatheartcount=0;        //������������
	
	//socket��������
	m_sock = INVALID_SOCKET;                   //socket id
	if (m_nType != SOCK_TODREB)
	{
		m_nType = SOCK_UNKNOW;                  //socket type SOCK_UNKNOW SOCK_CLI SOCK_DREB SOCK_SRV SOCK_TODREB
	}
	m_bChecked=false;               //�Ƿ�����֤������ȷ�Ϲ���������
	m_bNeedread=false;              //�Ƿ���Ҫ������
	m_nReadtime= 0;             //�ϴζ�����ʱ��
	m_nConntime=0;              //���ӽ���ʱ��
	m_nSendtime=0;              //�ϴη�������ʱ�� 
	m_nSend_packs=0;          //�����ӽ������͵����ݰ�
	m_nRcv_packs=0;           //�����ӽ�������յ����ݰ�
	m_nSendBytes=0;           //�����ӽ������͵��ֽ���
	m_nRcvBytes=0;            //�����ӽ�������յ��ֽ���
	//���ͽ�������	
	memset(m_sRcvBuffer,0,sizeof(m_sRcvBuffer));            //�������ݻ���   
	m_nRcvBufLen=0;                //���յ����ݳ���  �ɶ�ν���һ�����ݰ�
	memset(m_cCreateDate,0,sizeof(m_cCreateDate));//���ӽ�������
	m_nConntime = 0;
	memset(m_cCloseDate,0,sizeof(m_cCloseDate));//���ӹر�����
	m_nCloseTime = 0;
	
	m_sendNum = 10;
}

std::string CSocketInfo::GetSockType(int ntype)
{
// #define SOCK_UNKNOW   1      //δ֪
// #define SOCK_CLI      2      //�ͻ���
// #define SOCK_DREB     3      //�����������߽ڵ㵽���������߽ڵ������
// #define SOCK_SRV      4      //ע��ķ����
// #define SOCK_TODREB   5      //�������������߽ڵ�ڵ������
// #define SOCK_SLOT	  6      //ע���ͨѶ��
// #define SOCK_LISTEN	  7      //�����˿�
	char tmpchar[30];
	bzero(tmpchar,sizeof(tmpchar));
	switch (ntype)
	{
		case 1:
			sprintf(tmpchar,"0-SOCK_UNKNOW");
			break;
		case 2:
			sprintf(tmpchar,"1-SOCK_CLI");
			break;
		case 3:
			sprintf(tmpchar,"3-SOCK_DREB");
			break;
		case 4:
			sprintf(tmpchar,"4-SOCK_SRV");
			break;
		case 5:
			sprintf(tmpchar,"5-SOCK_TODREB");
			break;
		case 6:
			sprintf(tmpchar,"6-SOCK_SLOT");
			break;
		case 7:
			sprintf(tmpchar,"7-SOCK_LISTEN");
			break;
		default:
			break;
	}
	return std::string(tmpchar);
}
std::string CSocketInfo::GetDrebCmdType(int cmdtype)
{
	switch (cmdtype)
	{
		
	case  CMD_ROUTER:      //1  //�������߽ڵ㷢��·��֪ͨ��������
		return "CMD_ROUTER";
	case  CMD_DEL_NODE:    //2  //�������߽ڵ�֮��ȡ���������߽ڵ��������,ֻ�������������ӵ��������߽ڵ�
		return "CMD_DEL_NODE";
	case  CMD_DEL_SVR:     //3  //�����ȡ��ע������������
		return "CMD_DEL_SVR";
	case  CMD_PING:        //4  //���������������
		return "CMD_PING";
	case  CMD_CONNECT:     //5  //����ע��
		return "CMD_CONNECT";
	case  CMD_SERVICE:     //6  //����·��
		return "CMD_SERVICE";
	case  CMD_REGSERVICE:  //7  //ע�����
		return "CMD_REGSERVICE";			
	case  CMD_DPCALL:      //8  //�������߽ڵ�ͬ������ Ҫ�����մ�����ɺ�Ӧ��
		return "CMD_DPCALL";
	case  CMD_DPACALL:     //9  //�������߽ڵ��첽���� Ҫ����յ����������߽ڵ㷢���������ȷ��Ӧ��
		return "CMD_DPACALL";
	case  CMD_DPBC:        //10  //�������߽ڵ�㲥��������Ϣ����ָ���������߽ڵ�������ע��ķ���,Ҫ���������߽ڵ�Ӧ��
		return "CMD_DPBC";
	case  CMD_DPABC:       //11  //�������߽ڵ�㲥��������Ϣ����ָ���������߽ڵ�������ע��ķ���,��Ҫ���������߽ڵ�Ӧ��
		return "CMD_DPABC";			
	case  CMD_DPPUSH:      //12 //�������߽ڵ����ͣ�����Ӧ��
		return "CMD_DPPUSH";
	case  CMD_DPPOST:      //13 //�������߽ڵ�Ͷ�ݣ�Ҫ����յ����������߽ڵ�Ӧ��
		return "CMD_DPPOST";			
	case  CMD_MONITOR_DREB:     //15 //�������߽ڵ���
		return "CMD_MONITOR_DREB";
	case  CMD_MONITOR_BPC:     //16 //�������߽ڵ���
		return "CMD_MONITOR_BPC";
	case  CMD_DPBC_UNREGTX:     //17 //ȡ������Ľ���ע��㲥
		return "CMD_DPBC_UNREGTX";
	default:
		return "";
	}
}
void CSocketInfo::CheckFirstData()
{
	if (m_pSendQueue == NULL)
	{
		return;
	}
	RDQUEUE::iterator prq,prn;
	for (prn=m_pSendQueue->m_qSendData.datas.begin(); prn!=m_pSendQueue->m_qSendData.datas.end() ; )
	{
		prq = prn;
		prn++;
		if ((*prq)->msghead.nSendLen != (*prq)->msghead.nTotalLen)
		{
			m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"���� �����е�һ�����ݰ����Ͳ������ݣ�������ɾ�� %d",\
				(*prq)->msghead.index);
			m_pSendQueue->DeleteSend(prq);
		}
		break;
	}
}