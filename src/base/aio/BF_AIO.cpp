// BF_AIO.cpp: implementation of the CBF_AIO class.
//
//////////////////////////////////////////////////////////////////////

#include "BF_AIO.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBF_AIO::CBF_AIO()
{
	m_pAioWork = NULL;
	m_pAioRes=NULL;
	bzero(m_sErrMsg,sizeof(m_sErrMsg));
	m_pLog = NULL;
}

CBF_AIO::~CBF_AIO()
{
	m_pAioWork=NULL;
	m_pAioRes=NULL;
}
void CBF_AIO::SetParameter(CBF_AIO_WorkBase *workcallback,CBF_AIO_Resource *res)
{
	m_pAioWork = workcallback;
	m_pAioRes = res;
	m_pAioWork->SetParameter(this,&m_pAioPool);

	m_pAioPool.SetMaxNum(m_pAioRes->g_nMaxConnectNum);
	//���������������2000ʱ���ȷ���2000������
	if (m_pAioRes->g_nMaxConnectNum >2000)
	{
		//����������ʱ2000��С��g_nMaxConnectNumʱ����m_pAioPool��������
		m_pAioPool.Init(2000);
	}
	else
	{
		//С��2000��ֱ�ӷ��䵽���
		m_pAioPool.Init(m_pAioRes->g_nMaxConnectNum);
	}
	m_pLog = &(m_pAioRes->g_pLog);
}



bool CBF_AIO::Start()
{
	int ret;
	if (m_pLog == NULL)
	{
		return false;
	}
	m_pAioRes->g_bIsExit = false;
	if (!m_pSocketTcp.Create(AF_INET,false))
	{
		sprintf(m_sErrMsg,"�����˿�Createʧ��");
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		m_pAioRes->g_bIsExit = true;
		return false;
	}
	if (!m_pSocketTcp.Listen(m_pAioRes->g_nPort))
	{
		sprintf(m_sErrMsg,"�����˿�%d Listenʧ��",m_pAioRes->g_nPort);
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		m_pAioRes->g_bIsExit = true;
		return false;
	}


	PSOCKET_POOL_DATA info = m_pAioPool.PoolMalloc();
	if (info == NULL)
	{
		sprintf(m_sErrMsg,"�������ӿռ�ʧ��");
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		m_pAioRes->g_bIsExit = true;
		return false;
	}
	info->s_hSocket = m_pSocketTcp.GetSocket();
	info->s_cOperType = OP_LISTEN;
	info->s_cSocketType = SOCK_LISTEN;
	info->s_cSocketStatus = SOCKSTATUS_R; //ֻ�ö�������
	//��������������������ȫ�֣������߳�ʹ��
	m_pAioRes->g_nListenIndex = info->s_nIndex;
	
	m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"���� socket[%d] index[%d]",info->s_hSocket,info->s_nIndex);

	if (m_pAioRes->g_nRunMode == MODEIOCP )
	{
#if defined(_WINDOWS)
		//IOCPģʽ
		m_pAioRes->g_hIocp = ::CreateIoCompletionPort( INVALID_HANDLE_VALUE, 0, 0, 0 );
		if (m_pAioRes->g_hIocp == NULL)
		{
			sprintf(m_sErrMsg,"������ɶ˿�ʧ��");
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			m_pAioRes->g_bIsExit = true;
			return false;
		}
		CreateIoCompletionPort((HANDLE)info->s_hSocket, m_pAioRes->g_hIocp, NULL, 0 );

		PSOCKET_POOL_DATA infotmp=NULL;
		//���Ͷ��AcceptEx����,���������ݣ�ÿ��IO�߳�һ��acceptex
		CBF_SocketTcp ptcp;
		for (int k=0 ; k<m_pAioRes->g_nIoThreadNum ; k++)
		{
			ptcp.Create(AF_INET,false);//������
			//����һ���µĿյ�������Ϣ
			infotmp = m_pAioPool.PoolMalloc();
			if (infotmp == NULL)
			{
				sprintf(m_sErrMsg,"�������ӿռ�ʧ��");
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
				m_pAioRes->g_bIsExit = true;
				return false;
			}
			infotmp->s_hSocket = ptcp.GetSocket();

			infotmp->s_cOperType = OP_ACCEPT;
			infotmp->s_cSocketType = SOCK_ACCEPT;
			infotmp->s_cSocketStatus = SOCKSTATUS_R; //ֻ�ö�������
			infotmp->s_hSocket = ptcp.GetSocket();
			DWORD dwBytes;
			m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"index=%d ��ʼAcceptEx ",infotmp->s_nIndex);

			bool bret = AcceptEx(info->s_hSocket,infotmp->s_hSocket,infotmp->s_pIoBuffer,0,sizeof(sockaddr_in)+16, sizeof(sockaddr_in)+16,&dwBytes, (LPOVERLAPPED)infotmp);
			if (!bret) 
			{
				int iocperrno = WSAGetLastError();
				if  (iocperrno != ERROR_IO_PENDING)
				{
					sprintf(m_sErrMsg,"AcceptEx failed with error: %u\n", iocperrno);
					m_pAioPool.CloseFree(info);
					m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
					m_pAioRes->g_bIsExit = true;
					return false;
				}
			}
			ptcp.AttachSocket(INVALID_SOCKET,"");
		}
#else
		sprintf(m_sErrMsg,"IOCPֻ����windows������\n");
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		m_pAioRes->g_bIsExit = true;
		return false;
#endif

	}
	else if (m_pAioRes->g_nRunMode == MODEEPOLL )
	{
#if defined(LINUX)
		//EPOLLģʽ
		
		m_pAioRes->g_nEpoll = epoll_create(m_pAioRes->g_nMaxConnectNum);
		if (m_pAioRes->g_nEpoll <0 )
		{
			sprintf(m_sErrMsg,"epoll_createʧ��");
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			m_pAioRes->g_bIsExit = true;
			return false;
		}
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"����epoll handle[%d]",m_pAioRes->g_nEpoll);
		//���뵽EPOLL
		struct epoll_event ev;
		ev.events = EPOLLPRI|EPOLLERR|EPOLLIN|EPOLLOUT|EPOLLHUP;
		ev.data.ptr = info;
// 	    EPOLLIN ����ʾ��Ӧ���ļ����������Զ���
// 		EPOLLOUT����ʾ��Ӧ���ļ�����������д��
// 		EPOLLPRI�� ��ʾ��Ӧ���ļ��������н��������ɶ���
// 		EPOLLERR����ʾ��Ӧ���ļ���������������
// 		EPOLLHUP����ʾ��Ӧ���ļ����������Ҷϣ�
//      EPOLLET��    ET��epoll����ģʽ��
		
        if (epoll_ctl(m_pAioRes->g_nEpoll, EPOLL_CTL_ADD, ((PSOCKET_POOL_DATA)(ev.data.ptr))->s_hSocket, &ev)<0)
		{
			sprintf(m_sErrMsg,"�������˿ڼ���epollʧ�� [%d]",((PSOCKET_POOL_DATA)(ev.data.ptr))->s_hSocket);
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			m_pAioRes->g_bIsExit = true;
			return false;
		}
        m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���� epoll socket:[%d]",info->s_hSocket);
#else
		sprintf(m_sErrMsg,"EPOLLֻ����linux������\n");
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		m_pAioRes->g_bIsExit = true;
		return false;
#endif
	}
	else if (m_pAioRes->g_nRunMode == MODESELECT )
	{
		int id;
		//��ÿ��io�߳���Զ������߳�IDƽ���������ӽ���select����
		m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"�ܹ�%d��select�̣߳���%d����ƽ������ ",m_pAioRes->g_nIoThreadNum,m_pAioRes->g_nMaxConnectNum);
		for (int j=0;j < m_pAioRes->g_nMaxConnectNum ; j++)
		{
			id = j%m_pAioRes->g_nIoThreadNum;
			m_pAioThread[id].AddSocketIndex(j);
		}
	}
	else
	{
		m_pAioRes->g_bIsExit = true;
		m_pSocketTcp.Close();
		sprintf(m_sErrMsg,"����ģʽ���� %d\n",m_pAioRes->g_nRunMode);
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	int i;
    //IO�����̵߳ĳ�ʼ
	for (i=0 ; i< m_pAioRes->g_nIoThreadNum ; i++)
	{
		m_pAioThread[i].SetParameter(&m_pAioPool,m_pAioWork,m_pAioRes,i+1,m_pLog);
//		m_pAioThread[i].ListIndex();
		m_pAioThread[i].CreateThread();//����IO�߳�
	}

	//������ʱ��

 	m_pAioThread[0].m_pTimer.Init(100);
 	m_pAioThread[0].m_pTimer.SetTimer(0,5000,&CBF_AIO::OnTimer,this); //ÿ��5����һ��
 	m_pAioThread[0].m_pTimer.Start();
	return true;
}

bool CBF_AIO::Stop()
{
	m_pAioRes->g_bIsExit = true;
	m_pSocketTcp.Close();
 	//m_pAioThread[0].m_pTimer.KillTimer(0);
	m_pAioThread[0].m_pTimer.Stop();
	m_pAioThread[0].m_pTimer.Join();

 	m_pTimer.Stop();
	int i;
	if (m_pAioRes->g_nRunMode == MODEIOCP)
	{
#if defined(_WINDOWS)
		for (i=0; i<m_pAioRes->g_nIoThreadNum ; i++)  //֪ͨIO�߳�ֹͣ
		{
			PostQueuedCompletionStatus( m_pAioRes->g_hIocp, 0, 1, 0 );
		}
#endif
	}
	for (i=0 ; i< m_pAioRes->g_nIoThreadNum ; i++)
	{
		m_pAioThread[i].Join();
	}
	m_pTimer.Join();
	return true;
}

void CBF_AIO::ScanPool()
{
	//�鿴����
	int nSize;
	int nUse;
	m_pAioPool.PoolInfo(nSize,nUse);
	PSOCKET_POOL_DATA data ;
	m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"��ʼɨ�����ӳ�");
	for (int i=1; i<nSize ;i++)
	{
		data = m_pAioPool.GetDataByIndex(i);
		if (data->s_hSocket != INVALID_SOCKET && (data->s_cSocketType != SOCK_LISTEN && data->s_cSocketType != OP_ACCEPT)) 
		{
			//iocp���ü��acceptex��������������ڵȿͻ���������
			if (m_pAioRes->g_nRunMode == MODEIOCP )
			{
				if (data->s_cOperType == OP_ACCEPT)
				{
					continue;
				}
			}
			
			if (data->s_cHBFlag == HEARTBEAT_RECV)
			{
				if ((time(NULL) - data->s_nReadTime) > data->s_nHBCloseTime)
				{
					m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"����[%s]������index[%d]socket[%d]���ӳ�ʱ��δ�ã��Ͽ� rtime[%d] closetime[%d]",\
						inet_ntoa(data->s_pSocketAddr.sin_addr),data->s_nIndex,data->s_hSocket,data->s_nReadTime,data->s_nHBCloseTime);
					m_pAioWork->OnClose(data,"���ӳ�ʱ��δ�ã��Ͽ�");
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
					m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"����[%s]������index[%d]socket[%d]���ӳ�ʱ��δ�ã��Ͽ� wtime[%d] closetime[%d]",\
						inet_ntoa(data->s_pSocketAddr.sin_addr),data->s_nIndex,data->s_hSocket,data->s_nWriteTime,data->s_nHBCloseTime);
					m_pAioWork->OnClose(data,"���ӳ�ʱ��δ�ã��Ͽ�");
					continue;
				}
				else if ((time(NULL) - data->s_nWriteTime) > data->s_nHBTime)
				{
					m_pAioWork->OnPing(data);
				}
			}
		}
	}
	m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"ɨ�����ӳ����");
}

bool CBF_AIO::Send(SOCKET_SEND_DATA senddata,bool isimmediate)
{
	int ret=0;
	senddata.s_nSendLen = 0;
	PSOCKET_POOL_DATA info = m_pAioPool.GetDataByIndex(senddata.s_nIndex);
	if (info == NULL || info->s_hSocket == INVALID_SOCKET)
	{
		senddata.s_nRet = -1;
		sprintf(m_sErrMsg,"����ʧ�ܣ������ѹرջ�ָ������������[%d]������ ��ʶ[%d]",senddata.s_nIndex,senddata.s_nHook);
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	CBF_PMutex pp(&info->s_mutex);
	if (senddata.s_nTimestamp != 0)
	{
		if (info->s_nTimestamp != senddata.s_nTimestamp)
		{
			senddata.s_nRet = -1;
			sprintf(m_sErrMsg,"����ʧ�ܣ�����ʱ������� ��������[%d] ��ʶ[%d]",senddata.s_nIndex,senddata.s_nHook);
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
	}
	if (info->s_hSocket == INVALID_SOCKET)
	{
		senddata.s_nRet = -1;
		sprintf(m_sErrMsg,"����ʧ�ܣ������ѹر� ��������[%d] ��ʶ[%d]",senddata.s_nIndex,senddata.s_nHook);
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	
	senddata.s_nTime = time(NULL);
	CBF_SocketTcp pTcpSocket;
	if (isimmediate) //�������ͣ���¼��֤��
	{
		pTcpSocket.AttachSocket(info->s_hSocket,inet_ntoa(info->s_pSocketAddr.sin_addr));
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"��ʼ�������� %d",senddata.s_nTotalLen);
		ret = pTcpSocket.Send(senddata.s_sSendBuffer,senddata.s_nTotalLen);
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"����Send ���� %d",ret);
		if (ret == senddata.s_nTotalLen)
		{
			//�������
//			m_pAioRes->g_nSendByteLen +=ret;
			senddata.s_nRet = ret;
			info->s_nWriteTime = time(NULL);
//			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"������� index[%d] ��ʶ[%d] length[%d]",info->s_nIndex,senddata.s_nHook,ret);
			return true;
		}
		else
		{
			senddata.s_nRet = ret;
			if (ret >0)
			{
				senddata.s_nSendLen +=ret;	
			}
			if (info->s_pSendQueue.size() > 0)
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�����ܣ�����������֤��Ϣʱ�������������� index[%d] ��ʶ[%d] length[%d]",info->s_nIndex,senddata.s_nHook,ret);
				return false;
			}
			else //������У��ݻ�����
			{
				senddata.s_nTime = time(NULL);
				info->s_pSendQueue.push_back(senddata);
				m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"������У��´��ٷ� index[%d] ��ʶ[%d] ������ˮ[%d] ������[%d] ���η���length[%d] ʣ�೤��[%d] ���׶�����[%d] ���������[%d]",\
					info->s_nIndex,senddata.s_nHook,senddata.s_serial,senddata.s_nTxCode,ret,senddata.s_nTotalLen-senddata.s_nSendLen,info->s_pSendQueue.size(),info->s_pQuoteSendQueue.size());
				//���ݲ�ͬ��ģ�ͽ��д���
				if (m_pAioRes->g_nRunMode == MODESELECT ) //selectģ�ͣ�socket�Ƕ�д��Ҫ���
				{
					info->s_cSocketStatus = SOCKSTATUS_RW;
				}
				else if (m_pAioRes->g_nRunMode == MODEEPOLL)
				{
#if defined(LINUX)
					//Ҫ�鿴״̬
					struct epoll_event ev;
					ev.data.ptr = info;
					m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"��index[%d] socket[%d] g_nEpoll[%d] ����epoll ��д",info->s_nIndex,info->s_hSocket,m_pAioRes->g_nEpoll);
					ev.events = EPOLLPRI|EPOLLERR|EPOLLIN|EPOLLHUP|EPOLLOUT;//Ҫ����Ƿ��д������дʱ�ٷ�
					int ret = epoll_ctl(m_pAioRes->g_nEpoll, EPOLL_CTL_MOD,info->s_hSocket, &ev);
					if (ret != 0)
					{
						m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"epoll_ctl ret[%d] errno[%d] ʧ��",ret,errno);
					
					}
// 					else
// 					{
// 						m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"epoll_ctl ret[%d] �ɹ�",ret);
// 					}
					
#endif
				}
				else 
				{
#if defined(_WINDOWS)
					
					//����һ�������ݵ�����
					///�˴��д�ע��ָ�벻��
					info->s_cOperType = OP_WRITE;
					info->s_pSendData = &(senddata);
					WSABUF buf;
					buf.buf = senddata.s_sSendBuffer+senddata.s_nSendLen;
					buf.len = senddata.s_nTotalLen - senddata.s_nSendLen;
					DWORD dwSend = 0; 
					DWORD dwFlags = 0;
					ret = WSASend(info->s_hSocket,&buf,1,&dwSend,dwFlags,(LPOVERLAPPED)info,NULL);
#endif 
				}
			}
		}
		return true;
	}
	
	//�������չ���������
	

	int delaytime=0;
	int deletenum=0;//ɾ������
	int delaynum=0;//��������
	//Ҫ�ж϶����Ƿ�������
	if (info->s_pSendQueue.size() > 0 || info->s_pQuoteSendQueue.size() >0 )
	{
		senddata.s_nTime = time(NULL);
		if (senddata.s_nTimestamp != 0)
		{
			info->s_pSendQueue.push_back(senddata);
			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"������У��´��ٷ� index[%d] ��ʶ[%d] ������ˮ[%d] ������[%d] ���η���length[%d] ʣ�೤��[%d]  ���׶��д�С[%d] ������д�С[%d]",\
				info->s_nIndex,senddata.s_nHook,senddata.s_serial,senddata.s_nTxCode,0,senddata.s_nTotalLen-senddata.s_nSendLen,info->s_pSendQueue.size(),info->s_pQuoteSendQueue.size());
		}
		else
		{
			info->s_pQuoteSendQueue.push_back(senddata);
			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"����������У��´��ٷ� index[%d] ��ʶ[%d] ������ˮ[%d] ������[%d] ���η���length[%d] ʣ�೤��[%d]�����׶��д�С[%d] ������д�С[%d]",\
				info->s_nIndex,senddata.s_nHook,senddata.s_serial,senddata.s_nTxCode,0,senddata.s_nTotalLen-senddata.s_nSendLen,info->s_pSendQueue.size(),info->s_pQuoteSendQueue.size());
			//�Ƴ����ڵ���������
			SOCKSENDQUEUE::iterator prq,prn;
			bool bMoved = false;
			int  quoteNum = info->s_pQuoteSendQueue.size();
			for (prn=info->s_pQuoteSendQueue.begin() ; prn!=info->s_pQuoteSendQueue.end() ; )
			{
				prq = prn;
				prn++;
				if (info->s_nSendQueueFlag == 2 && bMoved==false)
				{
					quoteNum--;
					bMoved = true;
					continue;//���鷢�Ͳ��֣�������������Ƿ����
				}
				if (quoteNum > m_pAioRes->g_nQuoteQueueNum )
				{
					//�Ƴ�����
					info->s_pQuoteSendQueue.erase(prq);//�Ӷ���ɾ��
					quoteNum--;
					deletenum++;
					continue;
				}
				//������С�ڱ��������鿴����ʱ��
				delaytime = time(NULL) - prq->s_nTime ;
				//�����ڶ����ﳬ�����õ��볬ʱ����
				if ( delaytime > m_pAioRes->g_nQuoteQueueDeleteTime )
				{
					delaynum++;
					info->s_pQuoteSendQueue.erase(prq);//�Ӷ���ɾ��
					continue;	
				}
			}
			if (delaynum >0 || deletenum >0)
			{
				m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"����[%s]������index[%d] ��ʶ[%d] socket[%d] �����������  �Ƴ�������Ŀ[%d] ������Ŀ[%d] ����ʱ��[%d]  ���׶��д�С[%d] ������д�С[%d] ",\
					inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,prq->s_nHook,\
					info->s_hSocket,deletenum,delaynum,m_pAioRes->g_nQuoteQueueDeleteTime,\
					info->s_pSendQueue.size(),info->s_pQuoteSendQueue.size());
			}
			
		}
		//���ݲ�ͬ��ģ�ͽ��д���
		if (m_pAioRes->g_nRunMode == MODESELECT ) //selectģ�ͣ�socket�Ƕ�д��Ҫ���
		{
			info->s_cSocketStatus = SOCKSTATUS_RW;
		}
		else if (m_pAioRes->g_nRunMode == MODEEPOLL)
		{
#if defined(LINUX)
			//Ҫ�鿴״̬
			struct epoll_event ev;
			ev.data.ptr = info;
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"��index[%d] socket[%d] g_nEpoll[%d] ����epoll ��д",info->s_nIndex,info->s_hSocket,m_pAioRes->g_nEpoll);
			ev.events = EPOLLPRI|EPOLLERR|EPOLLIN|EPOLLOUT|EPOLLHUP;//Ҫ����Ƿ��д������дʱ�ٷ�
			int ret = epoll_ctl(m_pAioRes->g_nEpoll, EPOLL_CTL_MOD, info->s_hSocket, &ev);
			if (ret != 0)
			{
				m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"epoll_ctl ret[%d] errno[%d] ʧ��",ret,errno);
		
			}
// 			else
// 			{
// 				m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"epoll_ctl ret[%d] �ɹ�",ret);
// 			}
#endif
		}
		else 
		{
#if defined(_WINDOWS)
			//����һ�������ݵ�����
			//�˴�ָ���д�ע���޸�

			info->s_cOperType = OP_WRITE;
			info->s_pSendData = &senddata;
			WSABUF buf;
			buf.buf = senddata.s_sSendBuffer+senddata.s_nSendLen;
			buf.len = senddata.s_nTotalLen - senddata.s_nSendLen;
			DWORD dwSend = 0; 
			DWORD dwFlags = 0;
			ret = WSASend(info->s_hSocket,&buf,1,&dwSend,dwFlags,(LPOVERLAPPED)info,NULL);
#endif 
		}
	}
	else  //ֱ�ӷ���
	{
		info->s_nSendQueueFlag = 0;
//		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"��ʼ����  ��������[%d] ��ʶ[%d]",senddata.s_nIndex,senddata.s_nHook);
		pTcpSocket.AttachSocket(info->s_hSocket,inet_ntoa(info->s_pSocketAddr.sin_addr));
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"��ʼ�������� ����[%d]  socket[%d] ip[%s]",senddata.s_nTotalLen,info->s_hSocket,inet_ntoa(info->s_pSocketAddr.sin_addr));
		ret = pTcpSocket.Send(senddata.s_sSendBuffer,senddata.s_nTotalLen);
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"����Send ���� %d",ret);
		if (ret == senddata.s_nTotalLen)
		{
			//�������
//			m_pAioRes->g_nSendByteLen +=ret;
			senddata.s_nRet = ret;
			info->s_nWriteTime = time(NULL);
//			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"������� index[%d] ��ʶ[%d] length[%d]",info->s_nIndex,senddata.s_nHook,ret);
			return true;
		}
		else
		{
			senddata.s_nRet = ret;
			if(ret > 0)
			{
				senddata.s_nSendLen +=ret;
				info->s_nWriteTime = time(NULL);
			}
			senddata.s_nTime = time(NULL);
			if (senddata.s_nTimestamp != 0)
			{				
				info->s_pSendQueue.push_back(senddata);
				m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"������У��´��ٷ� index[%d] ��ʶ[%d] ������ˮ[%d] ������[%d] ���η���length[%d] ʣ�೤��[%d] ���׶��д�С[%d] ������д�С[%d]",\
					info->s_nIndex,senddata.s_nHook,senddata.s_serial,senddata.s_nTxCode,ret,\
					senddata.s_nTotalLen-senddata.s_nSendLen,info->s_pSendQueue.size(),\
					info->s_pQuoteSendQueue.size());
				if (senddata.s_nSendLen>0)
				{
					info->s_nSendQueueFlag = 1;//���Ͳ�������
				}
			}
			else
			{
				info->s_pQuoteSendQueue.push_back(senddata);
				m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"����������У��´��ٷ� index[%d] ��ʶ[%d] ������ˮ[%d] ������[%d] ���η���length[%d] ʣ�೤��[%d] ���׶��д�С[%d] ������д�С[%d]",\
					info->s_nIndex,senddata.s_nHook,senddata.s_serial,senddata.s_nTxCode,ret,senddata.s_nTotalLen-senddata.s_nSendLen,info->s_pSendQueue.size(),info->s_pQuoteSendQueue.size());
				if (senddata.s_nSendLen>0)
				{
					info->s_nSendQueueFlag = 2;//���鷢�Ͳ�������
				}
			}
			
			//���ݲ�ͬ��ģ�ͽ��д���
			if (m_pAioRes->g_nRunMode == MODESELECT ) //selectģ�ͣ�socket�Ƕ�д��Ҫ���
			{
				info->s_cSocketStatus = SOCKSTATUS_RW;
			}
			else if (m_pAioRes->g_nRunMode == MODEEPOLL)
			{
#if defined(LINUX)
				//Ҫ�鿴״̬
				struct epoll_event ev;
				ev.data.ptr = info;
				m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"��index[%d] socket[%d] g_nEpoll[%d] ����epoll ��д",info->s_nIndex,info->s_hSocket,m_pAioRes->g_nEpoll);
				ev.events = EPOLLPRI|EPOLLERR|EPOLLIN|EPOLLOUT|EPOLLHUP;//Ҫ����Ƿ��д������дʱ�ٷ�
//				ev.events = EPOLLOUT;//Ҫ����Ƿ��д������дʱ�ٷ�
				int ret = epoll_ctl(m_pAioRes->g_nEpoll, EPOLL_CTL_MOD,info->s_hSocket, &ev);
				if (ret != 0)
				{
					m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"epoll_ctl ret[%d] errno[%d] ʧ��",ret,errno);

				}
// 				else
// 				{
// 					m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"epoll_ctl ret[%d] �ɹ�",ret);
// 				}
#endif
			}
			else 
			{
#if defined(_WINDOWS)
				
				//����һ�������ݵ�����
				//�˴��д�ע���޸�
				info->s_cOperType = OP_WRITE;
				info->s_pSendData = &senddata;
				WSABUF buf;
				buf.buf = senddata.s_sSendBuffer+senddata.s_nSendLen;
				buf.len = senddata.s_nTotalLen - senddata.s_nSendLen;
				DWORD dwSend = 0; 
				DWORD dwFlags = 0;
				ret = WSASend(info->s_hSocket,&buf,1,&dwSend,dwFlags,(LPOVERLAPPED)info,NULL);
#endif 
			}
		}
	}
	return true;
}

bool CBF_AIO::AddAio(PSOCKET_POOL_DATA info)
{
	if (m_pAioRes->g_nRunMode == MODESELECT ) //selectģ�ͣ�socket�Ƕ�д��Ҫ���
	{
		//SELECTģ�͵ı�־��Ӧ������
//		info->s_cSocketStatus = SOCKSTATUS_R;
	}
	else if (m_pAioRes->g_nRunMode == MODEEPOLL )
	{
#if defined(LINUX)
		struct epoll_event ev;
		ev.data.ptr = info;
		ev.events = EPOLLPRI|EPOLLERR|EPOLLIN|EPOLLHUP;
        int ret = epoll_ctl(m_pAioRes->g_nEpoll, EPOLL_CTL_ADD, info->s_hSocket, &ev); 
		if (ret != 0)
		{
			m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"epoll_ctl add ret[%d] errno[%d] ʧ��",ret,errno);
			
		}
		else
		{
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"������ index[%d] socket[%d]����epoll",info->s_nIndex,info->s_hSocket);
		}
		
#else
		return false;
#endif
	}
	else
	{
#if defined(_WINDOWS)
		int ret;
		CreateIoCompletionPort((HANDLE)info->s_hSocket, m_pAioRes->g_hIocp, NULL, 0 );
		//����һ���������ݵ�����
		info->s_cOperType = OP_READ;
		WSABUF buf;
		buf.buf = info->s_pIoBuffer;
		buf.len = info->s_nIoBufferLen;
		DWORD dwRecv = 0; 
        DWORD dwFlags = 0;
		WSARecv(info->s_hSocket,&buf,1,&dwRecv,&dwFlags,(LPOVERLAPPED)info,NULL);
#else
		return false;
#endif 
	}

	return true;
}
bool CBF_AIO::Close(PSOCKET_POOL_DATA info)
{
	int ret;
	if (info == NULL)
	{
		return false;
	}
//	CBF_SocketTcp pTcpSocket;
	CBF_PMutex pp(&info->s_mutex);
	if (m_pAioRes->g_nRunMode == MODESELECT ) 
	{
		m_pAioPool.CloseFree(info);
	}
	else if (m_pAioRes->g_nRunMode == MODEEPOLL )
	{
#if defined(LINUX)
		struct epoll_event ev;
		ev.data.ptr = info;
        ret = epoll_ctl(m_pAioRes->g_nEpoll, EPOLL_CTL_DEL, info->s_hSocket , &ev); 
		if (ret != 0)
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"epoll_ctl del ret[%d] errno[%d] socket[%d] index[%d] ʧ��",\
				ret,errno,info->s_hSocket,info->s_nIndex);
		}
		m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"�ر����� index[%d]",info->s_nIndex);
		m_pAioPool.CloseFree(info);
		
#endif
	}
	else
	{
#if defined(_WINDOWS)
		m_pAioPool.CloseFree(info);
#endif 
	}
	return true;
}
bool CBF_AIO::Connect(char *ip,short port,unsigned  short  sockettype, unsigned int destindex ,unsigned int &index)
{
	int ret;
	CBF_SocketTcp pSocketTcp;
	if (!pSocketTcp.Create(AF_INET,false))
	{
		sprintf(m_sErrMsg,"Createʧ��");
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	if (m_pAioRes->g_nRunMode == MODEIOCP )
	{
#ifdef _WINDOWS
		SOCKADDR_IN temp;
		temp.sin_family = AF_INET;
		temp.sin_port = htons(0);
		temp.sin_addr.s_addr = htonl(ADDR_ANY);
		ret = bind(pSocketTcp.GetSocket(), (struct sockaddr*)&temp, sizeof(temp));
		if(ret<0) 
		{
			m_nSocketErrno = GET_LAST_SOCK_ERROR();
			return false;
		}
#endif
	}

	bool bret  = pSocketTcp.ConnectServer(ip,port,0);
	
	PSOCKET_POOL_DATA info = m_pAioPool.PoolMalloc();
	if (info == NULL)
	{
		sprintf(m_sErrMsg,"�������ӿռ�ʧ��");
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	unsigned int inaddr;
	memset(&(info->s_pSocketAddr), 0, sizeof(info->s_pSocketAddr));
	info->s_pSocketAddr.sin_family = AF_INET;
	inaddr=inet_addr(ip);

#if defined(_WINDOWS)
	info->s_pSocketAddr.sin_addr.S_un.S_addr = inaddr;
#endif
		
#if defined(AIX) || defined(LINUX) || defined(SOLARIS) || defined(HP_UX) 
	info->s_pSocketAddr.sin_addr.s_addr = inaddr;
#endif		
	info->s_pSocketAddr.sin_port = htons(port);
	
	info->s_hSocket = pSocketTcp.GetSocket();
	info->s_cOperType   = OP_CONNECT;
	info->s_cSocketType = sockettype;
	info->s_nDestIndex = destindex;
	index = info->s_nIndex;
	if (bret)
	{
		//���ӳɹ�
		info->s_cSocketStatus = SOCKSTATUS_C;//���ӵ�socket
	}
	//δ�ɹ�Ҫ���
	info->s_cSocketStatus = SOCKSTATUS_C;//���ӵ�socket

	return bret;
}


std::string CBF_AIO::GetErrMsg()
{
	return m_sErrMsg;
}
int  CBF_AIO::GetSocketErrNo()
{
	return m_nSocketErrno;
}

int CBF_AIO::OnTimer(unsigned int eventid, void *p)
{
	CBF_AIO *pp = (CBF_AIO *)p;
	pp->ScanPool();
	return 0;
}
