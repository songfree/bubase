// BF_AIO_Thread.cpp: implementation of the CBF_AIO_Thread class.
//
//////////////////////////////////////////////////////////////////////

#include "BF_AIO_Thread.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBF_AIO_Thread::CBF_AIO_Thread()
{
	m_pAioPool = NULL;
	m_pAioWork = NULL;
	m_pLog = NULL;
	m_pAioRes = NULL;
	bzero(m_sErrMsg,sizeof(m_sErrMsg));
	m_bIsScan = false;
}

CBF_AIO_Thread::~CBF_AIO_Thread()
{
	m_pAioPool = NULL;
	m_pAioWork = NULL;
	m_pLog = NULL;
	m_pAioRes = NULL;
}
// ������: InitThreadInstance
// ���  : ������ 2012-4-9 12:00:54
// ����  : virtual bool 
// ����  : �̳߳�ʼ�����߳�����ʱ����һ��
bool CBF_AIO_Thread::InitThreadInstance()
{
	return true;
}

// ������: ExitThreadInstance
// ���  : ������ 2012-4-9 12:01:05
// ����  : virtual void 
// ����  : �˳��߳�Ҫ���Ĺ���,�߳��˳�ʱ����һ��
void CBF_AIO_Thread::ExitThreadInstance()
{

}

// ������: Run
// ���  : ������ 2012-4-9 12:01:23
// ����  : virtual int 
// ����  : �߳����з��������˷������أ����߳��Զ���ֹ
int CBF_AIO_Thread::Run()
{
	if (m_pAioPool == NULL || m_pAioRes == NULL || m_pAioWork == NULL || m_pLog == NULL)
	{
		
		return -1;
	}
	if (m_pAioRes->g_nRunMode == MODEEPOLL)
	{
		RunEpoll();
	}
	else if (m_pAioRes->g_nRunMode == MODEIOCP)
	{
		RunIocp();
	}
	else
	{
		RunSelect();
	}
	
	return 0;
}

void CBF_AIO_Thread::SetParameter(CBF_AIO_Pool *pool, CBF_AIO_WorkBase *work,CBF_AIO_Resource  *res,unsigned int index,CIErrlog  *log)
{
	m_pAioPool = pool;
	m_pAioWork = work;
	m_pAioRes = res;
	m_nIndex = index;
	m_pLog = log;
}

void CBF_AIO_Thread::RunSelect()
{
	struct timeval tv;
	int result;
	int maxfd=0;
	int ret;
	int index;
	m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"����SELECT�߳� %d",m_nIndex);
	while (!m_pAioRes->g_bIsExit)
	{
		FD_ZERO(&m_rset);
		FD_ZERO(&m_wset);
#if defined(_WINDOWS)
		FD_ZERO(&m_eset);
#endif

		tv.tv_sec = 1;
		tv.tv_usec = 0;
		//ȡ������socket����socket���뵽fd_set����
		ret = SelectGetMaxfdAdd(maxfd);

		if (ret  < 1) //û�����Ӽ��뵽���߳̽��д���
		{
			SLEEP(5);
			continue;
		}
// 		if (ret >0)
// 		{
// 			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���߳�[%d]������[%d]",GetThreadID(),ret);
// 		}
#if defined(_WINDOWS)
		result = select(maxfd+1, &m_rset, &m_wset, &m_eset, &tv);
#else
		result = select(maxfd+1, &m_rset, &m_wset, NULL, &tv);
#endif
		if (result != 0) 
		{
//			m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"%d���� ���¼�����",ret);
			bool isdata = m_lSocketList.First(index);
			while (isdata)
			{
				PSOCKET_POOL_DATA info = m_pAioPool->GetDataByIndex(index);
				if (info != NULL)
				{
					SelectOnReadEvent(info);
					if (info->s_hSocket != INVALID_SOCKET)
					{
						SelectOnWriteEvent(info);
					}
				}
				//���ret�����ϵ��¼�
				isdata = m_lSocketList.Next(index);
			}
		}
		else
		{
//			m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"%d���� ���¼�����",ret);
		}

		//ɨ�����ӳ�
		if (m_bIsScan)
		{
			ScanPool();
			m_bIsScan = false;
		}
		
	}
	m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"ֹͣSELECT�߳� %d",m_nIndex);
	return ;
}

int CBF_AIO_Thread::SelectGetMaxfdAdd(int &maxfd)
{
	bool isadd=false;
	maxfd = 0;
	int num=0;
	int index;
	m_lSocketList.Clear();
	bool isdata = m_lIndexList.First(index);
	while (isdata)
	{
		isadd = false;
		//Ҫ���뵽���̵߳�select��
		PSOCKET_POOL_DATA info = m_pAioPool->GetDataByIndex(index);
		if (info != NULL)
		{
			if (info->s_hSocket == INVALID_SOCKET)
			{
				isdata = m_lIndexList.Next(index);
				continue;
			}
			switch (info->s_cSocketStatus)
			{
				case  SOCKSTATUS_RW:
					FD_SET(info->s_hSocket, &m_rset);//���뵽��select����
					FD_SET(info->s_hSocket, &m_wset);//���뵽дselect����
					isadd = true;
					m_pLog->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"����select SOCKSTATUS_RW index=%d sock=%d",index,info->s_hSocket);
					break;
				case  SOCKSTATUS_R:
					FD_SET(info->s_hSocket, &m_rset);//���뵽��select����
					isadd = true;
					if (info->s_pSendQueue.size()>0 || info->s_pQuoteSendQueue.size()>0)
					{
						FD_SET(info->s_hSocket, &m_wset);//���뵽дselect����
					}
					m_pLog->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"����select SOCKSTATUS_R index=%d sock=%d",index,info->s_hSocket);
					break;
				case  SOCKSTATUS_C:
					FD_SET(info->s_hSocket, &m_wset);//���뵽дselect����
	#if defined(_WINDOWS)
					FD_SET(info->s_hSocket, &m_eset);//���뵽�쳣select����
	#endif
					isadd = true;
					m_pLog->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"����select SOCKSTATUS_C index=%d sock=%d",index,info->s_hSocket);
					break;
				case  SOCKSTATUS_W:
					FD_SET(info->s_hSocket, &m_wset);//���뵽дselect����
					isadd = true;
					m_pLog->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"����select SOCKSTATUS_W index=%d sock=%d",index,info->s_hSocket);
					break;
				default:
					m_pLog->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"����select NULL index=%d sock=%d",index,info->s_hSocket);
					break;
				
			}
			if (isadd)
			{
//				m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"socket[%d] index[%d]",info->s_hSocket,info->s_nIndex);
				if (maxfd < info->s_hSocket)
				{
					maxfd = info->s_hSocket;
				}
				m_lSocketList.Add(info->s_nIndex);
				num++;
			}
		}
		else
		{
			//==NULL��ʾ�ѵ�ʵ�����������ֵ��
			break;
		}
		isdata = m_lIndexList.Next(index);
	}
	return num;
}

void CBF_AIO_Thread::SelectOnReadEvent(PSOCKET_POOL_DATA info)
{
	int ret;
	if (info->s_hSocket == INVALID_SOCKET)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"SOCKET�ѹر�");
		return;
	}
	ret = FD_ISSET(info->s_hSocket,&m_rset);
	if (ret != 0) //�ж��¼�
	{
//		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"index=%d �ж��¼�",info->s_nIndex);
		if (info->s_cSocketType == SOCK_LISTEN)
		{
			m_pSocketTcp.AttachSocket(info->s_hSocket,inet_ntoa(info->s_pSocketAddr.sin_addr));
			SOCKADDR_IN s_pSocketAddr;
			SOCKET_HANDLE tmpsock = m_pSocketTcp.Accept(s_pSocketAddr,false);
			if (tmpsock != INVALID_SOCKET)
			{
				PSOCKET_POOL_DATA data = m_pAioPool->PoolMalloc();
				if (data == NULL)
				{
					int nsize=0;
					int nouse=0;
					m_pAioPool->PoolInfo(nsize,nouse);
					char msg[500];
					bzero(msg,sizeof(msg));
					m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"����������[%d %d]��������[%s]�����ӱ��ܾ�",nsize,nouse,msg);
					closesocket(tmpsock);
					return;
				}
				data->s_hSocket = tmpsock;
				data->s_pSocketAddr = s_pSocketAddr;
				data->s_cSocketType = SOCK_CLIENT;
				m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�յ�[%s %d]������ index[%d] sock[%d]",\
					inet_ntoa(data->s_pSocketAddr.sin_addr),data->s_pSocketAddr.sin_port,\
					data->s_nIndex,data->s_hSocket);
				OnAccept(data);
				return;
			}
			return;
		}
		m_pSocketTcp.AttachSocket(info->s_hSocket,inet_ntoa(info->s_pSocketAddr.sin_addr));
		ret = m_pSocketTcp.Receive(info->s_sRealBuffer+info->s_nRealBufferLen,DREBBUFSIZE - info->s_nRealBufferLen);
		if (ret <= 0)
		{
			//�����ж��¼�����δ�յ����ݣ��Է��ر�����
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"����[%s]������index[%d]socket[%d] �Է������ر�����",\
				inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,info->s_hSocket);
			OnClose(info,__FILE__,__LINE__,"�Է������ر�����");
			return ;
		}
		//�������
		m_pAioRes->g_nRecvByteLen +=ret;
		info->s_nRealBufferLen+=ret;
		info->s_nReadTime = time(NULL);
		//ҵ�����յ�������
		OnRecvBack(info);
		return;
	}
// 	else
// 	{
// 		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"����[%s]������index[%d]socket[%d] �޶���Ϣ",\
// 				inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,info->s_hSocket);
// 	}
}

void CBF_AIO_Thread::SelectOnWriteEvent(PSOCKET_POOL_DATA info)
{
	int ret;
	if (info->s_hSocket == INVALID_SOCKET)
	{
		return;
	}
#if defined(_WINDOWS)
	ret = FD_ISSET(info->s_hSocket, &m_eset);
	if (ret != 0)
	{
		//������������
		if (info->s_cSocketStatus  == SOCKSTATUS_C)
		{
			OnConnectBack(info,false);//����ʧ��
		}
		else 
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"����[%s]������index[%d]socket[%d] windows�����쳣����",\
				inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,info->s_hSocket);
			OnClose(info,__FILE__,__LINE__,"windows�����쳣����");
		}
	}
	else if (FD_ISSET(info->s_hSocket, &m_wset) !=0 )
	{
		//������������
		if (info->s_cSocketStatus  == SOCKSTATUS_C)
		{
			OnConnectBack(info,true);//���ӳɹ���֪ͨӦ��
			return;
		}
		else //�������ӣ���д
		{
			//�Ǿͷ����ݰ�
			SelectSend(info);
		}
	}
#else
	
	int ok;
	int oklen;
	if (FD_ISSET(info->s_hSocket, &m_wset) !=0)
	{
		if (info->s_cSocketStatus  == SOCKSTATUS_C)
		{
			oklen=sizeof(ok);
#if defined(HP_UX)
			getsockopt(info->s_hSocket,SOL_SOCKET,SO_ERROR,(char *)&ok,&oklen);
#else	
			getsockopt(info->s_hSocket,SOL_SOCKET,SO_ERROR,(char *)&ok,(socklen_t *)&oklen);
#endif
			if (ok == 0)
			{
				OnConnectBack(info,true);//���ӳɹ���֪ͨӦ��
			}
			else
			{
				OnConnectBack(info,false);//����ʧ�ܣ�֪ͨӦ��
			} // end if (ok == 0)
		}
		else  //�������ӣ���д
		{
			//�Ǿͷ����ݰ�
			SelectSend(info);
		}
	}

#endif
}

void CBF_AIO_Thread::SelectSend(PSOCKET_POOL_DATA info)
{
	int ret;
	
	int sendlen=0;
	
	int onceSendNum=0;
	//�ѻ����������ȫ����ȥ��ֻ�����ܷ�
	SOCKSENDQUEUE::iterator prq,prn;
	CBF_PMutex pp(&info->s_mutex);
	m_pSocketTcp.AttachSocket(info->s_hSocket,"");
	m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"select ��ʼ�Ӷ��з�������");

	if (info->s_nSendQueueFlag == 1 || info->s_nSendQueueFlag == 0)
	{
		for (prn=info->s_pSendQueue.begin() ; prn!=info->s_pSendQueue.end() ; )
		{
			prq = prn;
			prn++;
			//һЩ�ж�
			if (prq->s_nIndex != info->s_nIndex || prq->s_nTimestamp != info->s_nTimestamp )
			{
				prq->s_nRet = -1;
				prq->s_nSendNum++;
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"����[%s]������index[%d] ��ʶ[%d] socket[%d] ���Ͷ���������ʱ�������,�Ƴ�",\
					inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,prq->s_nHook,info->s_hSocket);
				m_pAioWork->OnSendBack(prq,false);//���ظ�Ӧ�ã���Ӧ���ͷŻ����
				info->s_pSendQueue.erase(prq);//�Ӷ���ɾ��
				continue;
			}
			//��������
			//���ETģʽҪ�޸ģ�ֻ��������
			
			while (prq->s_nSendLen < prq->s_nTotalLen )
			{
				ret = m_pSocketTcp.Send(prq->s_sSendBuffer+prq->s_nSendLen,prq->s_nTotalLen-prq->s_nSendLen);
				if (ret <= 0)
				{
					m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"����[%s]������index[%d] ��ʶ[%d] socket[%d] ����ʧ��[%d] �ڶ����������",\
						inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,prq->s_nHook,info->s_hSocket,ret);
					//���Ͳ��������ñ�־
					if (prq->s_nSendLen > 0 )
					{
						info->s_nSendQueueFlag = 1;
					}
					else
					{
						info->s_nSendQueueFlag = 0;
					}
					return ;
				}
				else if (ret == (prq->s_nTotalLen-prq->s_nSendLen) ) //������
				{
					m_pAioRes->g_nSendByteLen +=ret;
					m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"����[%s]������index[%d] ��ʶ[%d] socket[%d] �Ӷ��з���������� �������ݳ���[%d]",\
						inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,prq->s_nHook,info->s_hSocket,ret);
					m_pAioWork->OnSendBack(prq,true);//���ظ�Ӧ�ã���Ӧ���ͷŻ����
					info->s_pSendQueue.erase(prq);//�Ӷ���ɾ��
					//�����������ݿ���״̬Ϊ���ȷ������ȶ�������
					info->s_nSendQueueFlag = 0;
				
					break;
					
				}
				prq->s_nSendLen +=ret;
				m_pAioRes->g_nSendByteLen +=ret;

				
			}
			onceSendNum++;
			if (onceSendNum >= m_pAioRes->g_nSendOnceNum)
			{
				return;
			}
		}  //end for ����
	}
	
	int  delaytime;
	//��ʼ����������У�ֻ������Ķ��з�����Ž���
	for (prn=info->s_pQuoteSendQueue.begin() ; prn!=info->s_pQuoteSendQueue.end() ; )
	{
		prq = prn;
		prn++;
		//��������
		delaytime = time(NULL) - prq->s_nTime ;


		if (prq->s_nSendLen == 0)  //��������û�з��͹���
		{
			//�����ڶ����ﳬ�����õ��볬ʱ����
			if ( delaytime > m_pAioRes->g_nQuoteQueueDeleteTime )
			{
				m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"����[%s]������index[%d] ��ʶ[%d] socket[%d] ���������������ʱ����� [%d > %d]��,�Ƴ�",\
					inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,prq->s_nHook,info->s_hSocket,delaytime,m_pAioRes->g_nQuoteQueueDeleteTime);
				m_pAioWork->OnSendBack(prq,false);//���ظ�Ӧ�ã���Ӧ���ͷŻ����
				info->s_pQuoteSendQueue.erase(prq);//�Ӷ���ɾ��
				continue;	
			}
		}
		
		//���ETģʽҪ�޸ģ�ֻ��������
		
		while (prq->s_nSendLen < prq->s_nTotalLen )
		{
			ret = m_pSocketTcp.Send(prq->s_sSendBuffer+prq->s_nSendLen,prq->s_nTotalLen-prq->s_nSendLen);
			if (ret <= 0)
			{
				m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"����[%s]������index[%d] ��ʶ[%d] socket[%d] ��������ʧ��[%d] �ڶ����������",\
					inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,prq->s_nHook,info->s_hSocket,ret);
				//				struct epoll_event ev;
				// 				ev.data.ptr = info;
				// 				ev.events = EPOLLPRI|EPOLLERR|EPOLLIN|EPOLLOUT|EPOLLHUP;//Ҫ����Ƿ��д������дʱ�ٷ�
				// 				epoll_ctl(m_pAioRes->g_nEpoll, EPOLL_CTL_MOD, info->s_hSocket, &ev);
				//���Ͳ��������ñ�־
				if (prq->s_nSendLen > 0 )
				{
					info->s_nSendQueueFlag = 2;
				}
				else
				{
					info->s_nSendQueueFlag = 0;
				}
				return ;
			}
			else if (ret == (prq->s_nTotalLen-prq->s_nSendLen) ) //������
			{
				m_pAioRes->g_nSendByteLen +=ret;
				m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"����[%s]������index[%d] ��ʶ[%d] socket[%d] �Ӷ��з�������������� �������ݳ���[%d]",\
					inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,prq->s_nHook,info->s_hSocket,ret);
				m_pAioWork->OnSendBack(prq,true);//���ظ�Ӧ�ã���Ӧ���ͷŻ����
				info->s_pQuoteSendQueue.erase(prq);//�Ӷ���ɾ��
				//��������������״̬Ϊ���ȷ������ȶ�������
				info->s_nSendQueueFlag = 0;
				
				break;
				
			}
			prq->s_nSendLen +=ret;
			m_pAioRes->g_nSendByteLen +=ret;
		}
		if (++onceSendNum >= m_pAioRes->g_nSendOnceNum)
		{
			return;
		}
	}  //end for ����
	//������ɣ������ټ���д
	info->s_cSocketStatus = SOCKSTATUS_R;


// 	for (prn=info->s_pSendQueue.begin() ; prn!=info->s_pSendQueue.end() ; )
// 	{
// 		prq = prn;
// 		prn++;
// 		//һЩ�ж�
// 		if ( prq->s_nIndex != info->s_nIndex || prq->s_nTimestamp != info->s_nTimestamp )
// 		{
// 			prq->s_nRet = -1;
// 			prq->s_nSendNum++;
// 			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"����[%s]������index[%d]socket[%d] ���Ͷ��������ݲ���,�Ƴ�",\
// 				inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,info->s_hSocket);
// 
// 			m_pAioWork->OnSendBack(prq,false);//���ظ�Ӧ�ã���Ӧ���ͷŻ����
// 			info->s_pSendQueue.erase(prq);//�Ӷ���ɾ��
// 			continue;
// 		}
// 		ret = SelectSend(prq);
// 		if (ret == prq->s_nTotalLen - prq->s_nSendLen)
// 		{
// 			prq->s_nRet = ret;
// 			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"������� ����index[%d] ��ʶ[%d] sock[%d] ip[%s] port[%d] ��д��������[%d]",\
// 				info->s_nIndex,prq->s_nHook,info->s_hSocket,inet_ntoa(info->s_pSocketAddr.sin_addr),\
// 				info->s_pSocketAddr.sin_port,ret);
// 			m_pAioWork->OnSendBack(prq,false);//���ظ�Ӧ�ã���Ӧ���ͷŻ����
// 			info->s_pSendQueue.erase(prq);//�Ӷ���ɾ��
// 			
// 		}
// 		else if (ret >0 ) 
// 		{
// 			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���Ͳ������� ����index[%d] ��ʶ[%d] sock[%d] ip[%s] port[%d] ��д��������[%d]",\
// 				info->s_nIndex,prq->s_nHook,info->s_hSocket,inet_ntoa(info->s_pSocketAddr.sin_addr),\
// 				info->s_pSocketAddr.sin_port,ret);
// 		}
// 	}  //end for ����
}

int CBF_AIO_Thread::SelectSend(SOCKSENDQUEUE::iterator prq)
{
	//��������
	int ret;
	int dlen=0;
	CBF_PMutex pp(&(m_pAioPool->GetDataByIndex(prq->s_nIndex)->s_mutex));
	while (prq->s_nSendLen > 0)
	{
		ret = m_pSocketTcp.Send(prq->s_sSendBuffer+prq->s_nSendLen,prq->s_nTotalLen-prq->s_nSendLen);
		if (ret < 0)
		{
			return -1;
		}
		if (ret == 0) //���ܷ���
		{
			return dlen;
		}
		dlen+=ret;
		prq->s_nSendLen +=ret;
		m_pAioRes->g_nSendByteLen +=ret;
	}
	return dlen;
	
}

void CBF_AIO_Thread::RunIocp()
{
#if defined(_WINDOWS)
	while (!m_pAioRes->g_bIsExit)
	{
		BOOL Status = 0;
        DWORD NumTransferred = 0;
		DWORD  perkey;
        PSOCKET_POOL_DATA info = NULL;
#if _MSC_VER > 1200
        Status = GetQueuedCompletionStatus( m_pAioRes->g_hIocp,&NumTransferred, (PULONG_PTR)&perkey, (LPOVERLAPPED *)&info,INFINITE );
#else
		Status = GetQueuedCompletionStatus( m_pAioRes->g_hIocp,&NumTransferred, (LPDWORD)&perkey, (LPOVERLAPPED *)&info,INFINITE );
#endif
        if (!Status)
        {
			if (info != NULL)
			{
				//˵����ɶ˿ڷ����ˣ�������ʧ��
				switch (info->s_cOperType)
				{
					case OP_READ:
						m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"����[%s]������index[%d]socket[%d] ��ɶ˿�GetQueuedCompletionStatusʧ��",\
							inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,info->s_hSocket);
						OnClose(info,__FILE__,__LINE__,"��ɶ˿�GetQueuedCompletionStatusʧ��");
						break;
					case OP_WRITE:
						m_pAioWork->OnSendBack(info->s_pSendData,false);
						break;
					default:
						m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"����[%s]������index[%d]socket[%d] ��ɶ˿�GetQueuedCompletionStatusʧ��",\
							inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,info->s_hSocket);
						OnClose(info,__FILE__,__LINE__,"��ɶ˿�GetQueuedCompletionStatusʧ��");
						break;
				}
			}
			continue;
        }
		if( NumTransferred == 0 && (info->s_cOperType == OP_READ || info->s_cOperType == OP_WRITE))
		{
			//���ӱ��ر�
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"����[%s]������index[%d]socket[%d] ���ӱ��Է��ر�",\
				inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,info->s_hSocket);
			OnClose(info,__FILE__,__LINE__,"���ӱ��Է��ر�");
			continue;

		}
        if ( 1 == perkey ) //�յ��˳���֪ͨ��
        {
			break;
        }
		WSABUF buf;
		DWORD dwRecv = 0; 
		DWORD dwFlags = 0;
		DWORD dwBytes;
		PSOCKET_POOL_DATA infotmp = NULL;

		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"����index[%d] ����Ϣ������ s_cOperType[%d]",info->s_nIndex,info->s_cOperType);
		switch (info->s_cOperType)
		{
			case OP_READ:
				//ԭ�����������ݼ������յ�����
				info->s_nRealBufferLen += info->s_nIoBufferLen;
				//��OnRecvBack����Ӧ��ȡ�����ݣ�����s_pIoBufferָ�뼰s_nIoBufferLen
				m_pAioWork->OnRecvBack(info);
				//��������һ���������ݵ�����
				info->s_cOperType = OP_READ;
				
				buf.buf = info->s_pIoBuffer;
				buf.len = info->s_nIoBufferLen;
				info->s_nReadTime = time(NULL);
				WSARecv(info->s_hSocket,&buf,1,&dwRecv,&dwFlags,(LPOVERLAPPED)info,NULL);
				break;
			case OP_WRITE:
				//���ͳɹ������ݣ�֪ͨһ��Ӧ��
				m_pAioWork->OnSendBack(info->s_pSendData,true);
				
				break;
			case OP_ACCEPT:
				//�����ӹ�����,֪ͨӦ�ý��м���
				OnAccept(info);
				//�ٷ�һ��Accept
				m_pSocketTcp.Create(AF_INET,false);//������
				//����һ���µĿյ�������Ϣ
				infotmp = m_pAioPool->PoolMalloc();
				if (infotmp == NULL)
				{
					m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�������������ܾ�����");
					//����AcceptEx���ܻᵼ�º�����������������,ͳһ��scanpool������
					break;
				}
				infotmp->s_hSocket = m_pSocketTcp.GetSocket();
				infotmp->s_cSocketType = SOCK_ACCEPT;
				infotmp->s_cOperType = OP_ACCEPT;
				infotmp->s_cSocketStatus = SOCKSTATUS_UNKNOW;
				m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"index=%d ��ʼAcceptEx ",infotmp->s_nIndex);
				AcceptEx(m_pAioPool->GetDataByIndex(m_pAioRes->g_nListenIndex)->s_hSocket,infotmp->s_hSocket,infotmp->s_pIoBuffer,0,sizeof(sockaddr_in)+16, sizeof(sockaddr_in)+16,&dwBytes, (LPOVERLAPPED)infotmp);

				break;
			case OP_CONNECT:
				//�������ӣ�֪ͨӦ�����ӳɹ�
				m_pAioWork->OnConnectBack(info,true);
				break;
			default:
				break;
		}
        
		
	}
#endif
}

void CBF_AIO_Thread::RunEpoll()
{
// 	    EPOLLIN ����ʾ��Ӧ���ļ����������Զ���
// 		EPOLLOUT����ʾ��Ӧ���ļ�����������д��
// 		EPOLLPRI�� ��ʾ��Ӧ���ļ��������н��������ɶ���
// 		
// 		EPOLLERR����ʾ��Ӧ���ļ���������������
// 		EPOLLHUP����ʾ��Ӧ���ļ����������Ҷϣ�
//      EPOLLET��    ET��epoll����ģʽ��

#if defined(LINUX)
	int ret;
	int nfds;
	struct epoll_event ev;
	struct epoll_event eopollEvents[EPOLLMAXFD];
	m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"��ʼepoll�߳� ���������%d",m_pAioRes->g_nMaxConnectNum);
	while(!m_pAioRes->g_bIsExit)
    {
        nfds = epoll_wait(m_pAioRes->g_nEpoll, eopollEvents, m_pAioRes->g_nMaxConnectNum, 1000);
		
        for(int i=0 ; i < nfds; i++)
        {
			PSOCKET_POOL_DATA data = (PSOCKET_POOL_DATA)eopollEvents[i].data.ptr;
			if (data->s_hSocket == INVALID_SOCKET)
			{
				continue;
			}
			if (eopollEvents[i].events & EPOLLIN || eopollEvents[i].events & EPOLLPRI ) //���¼�
			{
				if (data->s_cOperType == OP_LISTEN && data->s_nIndex == 0) //����
				{
					while (true)
					{
						m_pSocketTcp.AttachSocket(data->s_hSocket,"");
						m_pSocketTcp.SetBlockFlag(false);
						SOCKADDR_IN        s_pSocketAddr;
						SOCKET_HANDLE tmpsocket = m_pSocketTcp.Accept(s_pSocketAddr,false);
						if (tmpsocket != INVALID_SOCKET)
						{
							PSOCKET_POOL_DATA info = m_pAioPool->PoolMalloc();
							if (info == NULL)
							{
								int nsize=0;
								int nouse=0;
								m_pAioPool->PoolInfo(nsize,nouse);
								char msg[500];
								bzero(msg,sizeof(msg));
								m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"����������[%d %d]��������[%s]�����ӱ��ܾ�",nsize,nouse,msg);
								closesocket(tmpsocket);
								continue;
							}
							info->s_hSocket = tmpsocket;
							info->s_pSocketAddr = s_pSocketAddr;
							int nAll=0;
							int nFree=0;
							m_pAioPool->PoolInfo(nAll,nFree);
							m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���ܿͻ�����������[%s]������ index[%d] socket[%d]������  ��������[%d] ������[%d]",\
								inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,info->s_hSocket,nAll,nFree);

							OnAccept(info);
							continue;
						}
						
						break;
					}

				}
				else
				{
// 					m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"����[%s]������index[%d]socket[%d]�ж��¼�",\
// 						inet_ntoa(data->s_pSocketAddr.sin_addr),data->s_nIndex,data->s_hSocket);
					EpollRecv(data);
				}
				
			}
            if (eopollEvents[i].events & EPOLLOUT ) //д�¼�
			{
// 				m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"����[%s]������index[%d]socket[%d]��Ӧд�¼�",\
// 					inet_ntoa(data->s_pSocketAddr.sin_addr),data->s_nIndex,data->s_hSocket);
				if (data->s_hSocket != INVALID_SOCKET)
				{
					EpollSend(data);
				}
			}
			if (eopollEvents[i].events & EPOLLERR  || eopollEvents[i].events & EPOLLHUP  ) //�ر������¼�
			{
				if (data->s_hSocket != INVALID_SOCKET)
				{
// 					ev.data.ptr = data;
// 					ret = epoll_ctl(m_pAioRes->g_nEpoll, EPOLL_CTL_DEL, data->s_hSocket, &ev);
// 					if (ret != 0)
// 					{
// 						m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"����[%s]������index[%d]socket[%d] EPOLL_CTL_DEL ʧ��[%d]",\
// 							inet_ntoa(data->s_pSocketAddr.sin_addr),data->s_nIndex,data->s_hSocket,ret);
// 					}
					m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ر����� ����[%s]������index[%d]socket[%d] ���Է��ر�",\
						inet_ntoa(data->s_pSocketAddr.sin_addr),data->s_nIndex,data->s_hSocket);
					OnClose(data,__FILE__,__LINE__,"�ر����� ���ӱ��Է��ر�");
				}
			}
        }
		//ɨ�����ӳ�
		if (m_bIsScan)
		{
			ScanPool();
			m_bIsScan = false;
		}
    }
#endif
	m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"ֹͣepoll�߳�");
}

void CBF_AIO_Thread::EpollRecv(PSOCKET_POOL_DATA info)
{
#if defined(LINUX)
	int ret;
	struct epoll_event ev;
	m_pSocketTcp.AttachSocket(info->s_hSocket,"");
	//��ͣ���գ�ֻ���ѻ���������������
	
	//	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�������� offset[%d]",info->s_nRealBufferLen);
	ret = m_pSocketTcp.Receive(info->s_sRealBuffer+info->s_nRealBufferLen,DREBBUFSIZE-info->s_nRealBufferLen);
	if (ret <= 0) 
	{
//   	ev.data.ptr = info;
//   	ret = epoll_ctl(m_pAioRes->g_nEpoll, EPOLL_CTL_DEL, info->s_hSocket, &ev);
// 		if (ret != 0)
// 		{
// 			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"����[%s]������index[%d]socket[%d] EPOLL_CTL_DEL ʧ��[%d]",\
// 			inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,info->s_hSocket,ret);
// 		}
		m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"����[%s]������index[%d]socket[%d]���շ��س���[%d] ���ݳ���[%d]",\
			inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,info->s_hSocket,ret,info->s_nRealBufferLen);
 		OnClose(info,__FILE__,__LINE__,"�Է������ر�����");
		return;
	}
	info->s_nRealBufferLen+=ret;
	info->s_nReadTime = time(NULL);
	
	//�������
	m_pAioRes->g_nRecvByteLen +=ret;
//	m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"�յ�����[%s]index[%d]socket[%d]�����ݳ���[%d]",\
//		inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,info->s_hSocket,info->s_nRealBufferLen);
	//������Ϣ����
	m_pAioWork->OnRecvBack(info);
// 	ev.data.ptr = info;
// 	ev.events = EPOLLPRI|EPOLLERR|EPOLLIN|EPOLLHUP;//���ü���Ƿ��д��ֻ�е����Ͳ���ʱ�ż����д���
//     epoll_ctl(m_pAioRes->g_nEpoll, EPOLL_CTL_MOD, info->s_hSocket, &ev);
#endif
	return;
}

void CBF_AIO_Thread::EpollSend(PSOCKET_POOL_DATA info)
{
#if defined(LINUX)
	int ret;
	int sendlen=0;

	int onceSendNum=0;
	//�ѻ����������ȫ����ȥ��ֻ�����ܷ�
	SOCKSENDQUEUE::iterator prq,prn;
	CBF_PMutex pp(&info->s_mutex);
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"EpollSend��ʼ");
	m_pSocketTcp.AttachSocket(info->s_hSocket,inet_ntoa(info->s_pSocketAddr.sin_addr));

	if (info->s_nSendQueueFlag == 1 || info->s_nSendQueueFlag == 0)
	{
		for (prn=info->s_pSendQueue.begin() ; prn!=info->s_pSendQueue.end() ; )
		{
			prq = prn;
			prn++;
			//һЩ�ж�
			if (prq->s_nIndex != info->s_nIndex || prq->s_nTimestamp != info->s_nTimestamp )
			{
				prq->s_nRet = -1;
				prq->s_nSendNum++;
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"����[%s]������index[%d] ��ʶ[%d] socket[%d] ���Ͷ���������ʱ�������,�Ƴ�",\
					inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,prq->s_nHook,info->s_hSocket);
				m_pAioWork->OnSendBack(prq,false);//���ظ�Ӧ�ã���Ӧ���ͷŻ����
				info->s_pSendQueue.erase(prq);//�Ӷ���ɾ��
				continue;
			}
			//��������
			//���ETģʽҪ�޸ģ�ֻ��������
			
			while (prq->s_nSendLen < prq->s_nTotalLen )
			{
				ret = m_pSocketTcp.Send(prq->s_sSendBuffer+prq->s_nSendLen,prq->s_nTotalLen-prq->s_nSendLen);
				if (ret <= 0)
				{
					m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"����[%s]������index[%d] ��ʶ[%d] socket[%d] ����ʧ��[%d] �ڶ����������",\
						inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,prq->s_nHook,info->s_hSocket,ret);
					//���Ͳ��������ñ�־
					if (prq->s_nSendLen > 0 )
					{
						info->s_nSendQueueFlag = 1;
					}
					else
					{
						info->s_nSendQueueFlag = 0;
					}
					return ;
				}
				else if (ret == (prq->s_nTotalLen-prq->s_nSendLen) ) //������
				{
					m_pAioRes->g_nSendByteLen +=ret;
					m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"����[%s]������index[%d] ��ʶ[%d] socket[%d] �Ӷ��з���������� �������ݳ���[%d]",\
						inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,prq->s_nHook,info->s_hSocket,ret);
					m_pAioWork->OnSendBack(prq,true);//���ظ�Ӧ�ã���Ӧ���ͷŻ����
					info->s_pSendQueue.erase(prq);//�Ӷ���ɾ��
					//�����������ݿ���״̬Ϊ���ȷ������ȶ�������
					info->s_nSendQueueFlag = 0;
				
					break;
					
				}
				prq->s_nSendLen +=ret;
				m_pAioRes->g_nSendByteLen +=ret;

				
			}
			onceSendNum++;
			if (onceSendNum >= m_pAioRes->g_nSendOnceNum)
			{
				return;
			}
		}  //end for ����
	}
	
	int  delaytime;
	//��ʼ����������У�ֻ������Ķ��з�����Ž���
	for (prn=info->s_pQuoteSendQueue.begin() ; prn!=info->s_pQuoteSendQueue.end() ; )
	{
		prq = prn;
		prn++;
		//��������
		delaytime = time(NULL) - prq->s_nTime ;


		if (prq->s_nSendLen == 0)  //��������û�з��͹���
		{
			//�����ڶ����ﳬ�����õ��볬ʱ����
			if ( delaytime > m_pAioRes->g_nQuoteQueueDeleteTime )
			{
				m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"����[%s]������index[%d] ��ʶ[%d] socket[%d] ���������������ʱ����� [%d > %d]��,�Ƴ�",\
					inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,prq->s_nHook,info->s_hSocket,delaytime,m_pAioRes->g_nQuoteQueueDeleteTime);
				m_pAioWork->OnSendBack(prq,false);//���ظ�Ӧ�ã���Ӧ���ͷŻ����
				info->s_pQuoteSendQueue.erase(prq);//�Ӷ���ɾ��
				continue;	
			}
		}
		
		//���ETģʽҪ�޸ģ�ֻ��������
		
		while (prq->s_nSendLen < prq->s_nTotalLen )
		{
			ret = m_pSocketTcp.Send(prq->s_sSendBuffer+prq->s_nSendLen,prq->s_nTotalLen-prq->s_nSendLen);
			if (ret <= 0)
			{
				m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"����[%s]������index[%d] ��ʶ[%d] socket[%d] ��������ʧ��[%d] �ڶ����������",\
					inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,prq->s_nHook,info->s_hSocket,ret);
				//				struct epoll_event ev;
				// 				ev.data.ptr = info;
				// 				ev.events = EPOLLPRI|EPOLLERR|EPOLLIN|EPOLLOUT|EPOLLHUP;//Ҫ����Ƿ��д������дʱ�ٷ�
				// 				epoll_ctl(m_pAioRes->g_nEpoll, EPOLL_CTL_MOD, info->s_hSocket, &ev);
				//���Ͳ��������ñ�־
				if (prq->s_nSendLen > 0 )
				{
					info->s_nSendQueueFlag = 2;
				}
				else
				{
					info->s_nSendQueueFlag = 0;
				}
				return ;
			}
			else if (ret == (prq->s_nTotalLen-prq->s_nSendLen) ) //������
			{
				m_pAioRes->g_nSendByteLen +=ret;
				m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"����[%s]������index[%d] ��ʶ[%d] socket[%d] �Ӷ��з�������������� �������ݳ���[%d]",\
					inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,prq->s_nHook,info->s_hSocket,ret);
				m_pAioWork->OnSendBack(prq,true);//���ظ�Ӧ�ã���Ӧ���ͷŻ����
				info->s_pQuoteSendQueue.erase(prq);//�Ӷ���ɾ��
				//��������������״̬Ϊ���ȷ������ȶ�������
				info->s_nSendQueueFlag = 0;
				
				break;
				
			}
			prq->s_nSendLen +=ret;
			m_pAioRes->g_nSendByteLen +=ret;
		}
		if (++onceSendNum >= m_pAioRes->g_nSendOnceNum)
		{
			return;
		}
	}  //end for ����

	//ȫ���������ˣ����Բ��ü��д
	struct epoll_event ev;
	ev.data.ptr = info;
	ev.events = EPOLLPRI|EPOLLERR|EPOLLIN|EPOLLHUP;//Ҫ����Ƿ��д������дʱ�ٷ�
	ret = epoll_ctl(m_pAioRes->g_nEpoll, EPOLL_CTL_MOD, info->s_hSocket, &ev);
	if (ret != 0)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"����[%s]������index[%d]socket[%d] EPOLL_CTL_MOD ʧ��[%d]",\
			inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,info->s_hSocket,ret);
	}

#endif
	return;

}

void CBF_AIO_Thread::OnClose(PSOCKET_POOL_DATA info, const char *filename, int fileline, const char *msg)
{
	m_pLog->LogMp(LOG_PROMPT,filename,fileline,"�ر����� index[%d]  ����[%s]  socket[%d] %s",\
		info->s_nIndex,inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_hSocket,msg);
	m_pAioWork->OnClose(info,msg);
}

void CBF_AIO_Thread::OnRecvBack(PSOCKET_POOL_DATA data)
{
	m_pAioWork->OnRecvBack(data);
}

void CBF_AIO_Thread::OnPing(PSOCKET_POOL_DATA data)
{
	m_pAioWork->OnPing(data);
}

// void CBF_AIO_Thread::OnSendBack(SOCKET_SEND_DATA data,bool bret)
// {
// 	m_pAioWork->OnSendBack(data,bret);
// }

void CBF_AIO_Thread::OnAccept(PSOCKET_POOL_DATA data)
{
	data->s_cHBFlag = HEARTBEAT_RECV;
	data->s_nReadTime = time(NULL);
	data->s_nWriteTime = data->s_nReadTime;
	data->s_nHBCloseTime = m_pAioRes->g_nDisconnectTime;
	data->s_nHBTime = m_pAioRes->g_nHeartRun;
	data->s_nTimestamp = data->s_nReadTime;
	data->s_cSocketStatus  = SOCKSTATUS_R;
	data->s_cSocketType = SOCK_CLIENT;

	//20151119 ��iocp����accept���ñ�־Ϊ��
	data->s_cOperType = OP_READ;

	m_pAioWork->OnAccept(data);
}

void CBF_AIO_Thread::OnConnectBack(PSOCKET_POOL_DATA data, bool bret)
{
	if (bret)
	{
		m_pAioWork->OnConnectBack(data,0);
	}
	else
	{
		m_pAioWork->OnConnectBack(data,1);
	}
	
}

void CBF_AIO_Thread::AddSocketIndex(int index)
{
	m_lIndexList.Add(index);
}

void CBF_AIO_Thread::ListIndex()
{
	int ret;
	printf("id[%d]   indexlist:",GetThreadID());
	bool isdata = m_lIndexList.First(ret);
	while (isdata)
	{
		printf("%d ",ret);
		isdata = m_lIndexList.Next(ret);
	}
	printf("END \n");
}

void CBF_AIO_Thread::ScanPool()
{
	//�鿴����
	int nSize;
	int nUse;
	m_pAioPool->PoolInfo(nSize,nUse);
	PSOCKET_POOL_DATA data ;
	for (int i=1; i<nSize ;i++)
	{
		data = m_pAioPool->GetDataByIndex(i);
		if (data->s_hSocket != INVALID_SOCKET && (data->s_cSocketType != SOCK_LISTEN && data->s_cSocketType != OP_ACCEPT)) 
		{
			if (data->s_cHBFlag == HEARTBEAT_RECV)
			{
				if ((time(NULL) - data->s_nReadTime) > data->s_nHBCloseTime)
				{
					m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"����[%s]������index[%d]socket[%d]���ӳ�ʱ��δ�ã��Ͽ� ����ʱ��[%d] �ر�ʱ��[%d]",\
						inet_ntoa(data->s_pSocketAddr.sin_addr),data->s_nIndex,data->s_hSocket,data->s_nReadTime,data->s_nHBCloseTime);
					OnClose(data,__FILE__,__LINE__,"���ӳ�ʱ��δ�ã��Ͽ�");
					continue;
				}
				else if ((time(NULL) - data->s_nReadTime) > data->s_nHBTime)
				{
					m_pAioWork->OnPing(data);
				}
			}
			else if (data->s_cHBFlag == HEARTBEAT_SEND)
			{
				if ((time(NULL) - data->s_nWriteTime) > data->s_nHBCloseTime)
				{
					m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"����[%s]������index[%d]socket[%d]���ӳ�ʱ��δ�ã��Ͽ� ���дʱ��[%d] �ر�ʱ��[%d]",\
						inet_ntoa(data->s_pSocketAddr.sin_addr),data->s_nIndex,data->s_hSocket,data->s_nWriteTime,data->s_nHBCloseTime);
					OnClose(data,__FILE__,__LINE__,"���ӳ�ʱ��δ�ã��Ͽ�");
					continue;
				}
				else if ((time(NULL) - data->s_nWriteTime) > data->s_nHBTime)
				{
					m_pAioWork->OnPing(data);
				}
			}
		}
	}
}

int CBF_AIO_Thread::OnTimer(unsigned int eventid, void *p)
{
	CBF_AIO_Thread *pp = (CBF_AIO_Thread *)p;
	pp->SetScan();
	return 0;
}

void CBF_AIO_Thread::SetScan()
{
	m_bIsScan = true;
}
