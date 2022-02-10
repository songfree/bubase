// PoolModule.cpp: implementation of the CPoolModule class.
//
//////////////////////////////////////////////////////////////////////

#include "PoolModule.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPoolModule::CPoolModule()
{
	m_sThreadName = "CPoolModule";
	m_bIsInit = false;
	m_bIsRunning = false;
	memset(m_errMsg,0,sizeof(m_errMsg));
	m_log=NULL;
	m_pRcvQueue= NULL;
	
	m_pRes = NULL;
	m_bIsPingTimer=false;
	m_pMemPool = NULL;
}

CPoolModule::~CPoolModule()
{
	m_log=NULL;
	m_pRcvQueue= NULL;
	m_pRes = NULL;
	m_pMemPool = NULL;
	
}
bool CPoolModule::Terminate()
{
	m_bIsRunning = false;
	return true;
}
int CPoolModule::Run()
{
	struct timeval tv;
	int result;
	int i,j;
	int maxfd=0;
	int ret;
	int lasttime=time(NULL);
#ifdef _ENGLISH_
	m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"Start listen thread");
#else
	m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"��ʼ�����߳�");
#endif
	while (m_bIsRunning)
	{


		FD_ZERO(&m_rset);
		FD_ZERO(&m_wset);
#if defined(_WINDOWS)
		FD_ZERO(&m_eset);
#endif
		tv.tv_sec = 1;
		tv.tv_usec = 0;

		//ȡ������socket����socket���뵽fd_set����
		ret = GetMaxfdAddSelect(maxfd);
		if (maxfd  <=0)
		{
			SLEEP(10);
			continue;
		}
#if defined(_WINDOWS)
		result = select(maxfd+1, &m_rset, &m_wset, &m_eset, &tv);
#else
		result = select(maxfd+1, &m_rset, &m_wset, NULL, &tv);
#endif
		
		if (result != 0) //�¼�����
		{
			OnEvent();
		}

		if (m_bIsPingTimer)
		{
			OnNoEvent();
			m_bIsPingTimer = false;
		}
	}
#ifdef _ENGLISH_
	m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"Stop listen thread");
#else
	m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"�˳������߳�");
#endif
	return 0;
}

bool CPoolModule::InitFront(CTradeResource *res,CPoolDataMsg *que,CIErrlog *log)
{
	m_pRes = res;
	m_log = log;
	m_pRcvQueue = que;

	//��ʼ�����ӳ�
	if (!m_pSockMgr.Init(m_pRes,m_pMemPool))
	{
#ifdef _ENGLISH_
		sprintf(m_errMsg,"init error:%244s",m_pSockMgr.m_errMsg);
#else
		sprintf(m_errMsg,"��ʼ��ʧ��:%244s",m_pSockMgr.m_errMsg);
#endif
		printf("%s \n",m_errMsg);
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
		m_bIsInit = false;
		return false;
	}
	if (!m_pSockMgr.at(0)->Listen(m_pRes->g_nListenPort,false,30))
	{
#ifdef _ENGLISH_
		sprintf(m_errMsg,"init error:listen socket %d faild",m_pRes->g_nListenPort);
#else
		sprintf(m_errMsg,"��ʼ��ʧ��:����%d sokcet����ʧ��",m_pRes->g_nListenPort);
#endif
		printf("%s \n",m_errMsg);
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
		m_bIsInit = false;
		return false;
	}
#ifdef UNIX
	if (!m_pSockMgr.at(1)->ListenIpc(m_pRes->g_nListenPort,false,30))
	{
#ifdef _ENGLISH_
		sprintf(m_errMsg,"init error:listen ipc %d faild",m_pRes->g_nListenPort);
#else
		sprintf(m_errMsg,"��ʼ��ʧ��:����%d sokcet����ʧ��",m_pRes->g_nListenPort);
#endif
		printf("%s \n",m_errMsg);
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
		m_bIsInit = false;
		return false;
	}
#endif
	m_pTimer.Init(100,true);
	//�ڵ�·�ɶ�ʱ��
	m_pTimer.SetTimer(0,5000,&CPoolModule::OnTimer,this); //���ö�ʱ�� 

	
	return true;
}

void CPoolModule::StopFront()
{
	m_bIsRunning = false;
	if (m_pRes != NULL)
	{
		m_pRes->g_bToExit = true;
	}
	m_pTimer.Stop();
//	printf("��ʼ�˳�CPoolModule\n");
	Join();

}

int CPoolModule::GetMaxfdAddSelect(int &maxfd)
{
	int ret=0;
	maxfd = 0;

	for (int i=0;i<m_pRes->g_nMaxConnectNum;i++)
	{
		if (!m_pSockMgr.at(i)->IsCanSelect())
		{
			ret++;
		}
		else
		{
			if (m_pSockMgr.at(i)->m_sock != INVALID_SOCKET && m_pSockMgr.at(i)->m_bNeedread)
			{
				FD_SET(m_pSockMgr.at(i)->m_sock, &m_rset);//���뵽��select����
			}
			if (m_pSockMgr.at(i)->NeedSend())
			{
				FD_SET(m_pSockMgr.at(i)->m_sock, &m_wset);//���뵽дselect����
				//m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"����index[%d]������Ƿ��д",i);
			}
			if (m_pSockMgr.at(i)->m_sock > maxfd)
			{
				maxfd=m_pSockMgr.at(i)->m_sock; //�õ�����socket 
			}
		}
	}
	return ret;
}



void CPoolModule::OnNoEvent()
{
	int j;
	int tmptime;
	if (m_pRes == NULL)
	{
		return;
	}
	for (j=1;j<m_pRes->g_nMaxConnectNum;j++)
	{
		if (m_pRes == NULL)
		{
			return;
		}
		if (m_pSockMgr.at(j)->m_sock != INVALID_SOCKET && m_pSockMgr.at(j)->m_nType == SOCK_CLI)
		{
			tmptime = time(NULL) - m_pSockMgr.at(j)->m_nReadtime;
			if ( tmptime > m_pRes->g_nHeartRun && tmptime < m_pRes->g_nDisconnectTime)
			{
				OnPing(j);
			}
			else if (tmptime > m_pRes->g_nDisconnectTime)
			{
#ifdef _ENGLISH_
				OnClose(j,"connect unuse long time,clost it");
#else
				OnClose(j,"���ӳ�ʱ��δ�ã��Ͽ�");
#endif
			}
		}
		
		if (m_pRes->g_bToExit)
		{
			return;
		}
	}
}

void CPoolModule::OnEvent()
{
	int ret;
	int i;

	ret = FD_ISSET(m_pSockMgr.at(0)->m_sock,&m_rset);
	if (ret != 0) //����������˿��ж����¼�����ȥ����accept
	{
		OnAccept(0);
	}
	if (m_pSockMgr.at(1)->m_sock != INVALID_SOCKET && m_pSockMgr.at(1)->m_nType == SOCK_LISTEN)
	{
		ret = FD_ISSET(m_pSockMgr.at(1)->m_sock,&m_rset);
		if (ret != 0) //����������˿��ж����¼�����ȥ����accept
		{
			OnAccept(1);
		}
	}
	for (i= 2; i< m_pRes->g_nMaxConnectNum; i++)
	{
		if (m_pSockMgr.at(i)->m_sock == INVALID_SOCKET)//δ����
		{
			continue;
		}
		else
		{
			//��Ӧ���¼�
			OnReadEvent(i);
			if (m_pSockMgr.at(i)->m_sock != INVALID_SOCKET)//δ����
			{
				//��Ӧд�¼�
				OnWriteEvent(i);
			}
		}
		if (m_pRes->g_bToExit)
		{
			return;
		}
	}
}



void CPoolModule::OnReadEvent(int conindex)
{
	int ret;
	int tmptime;
	ret = FD_ISSET(m_pSockMgr.at(conindex)->m_sock,&m_rset);
	if (ret != 0) //�ж��¼�
	{
		OnRecv(conindex);
	}
	else //�������޶��¼�
	{
		if (m_pSockMgr.at(conindex)->m_sock != INVALID_SOCKET && m_pSockMgr.at(conindex)->m_nType == SOCK_CLI)
		{
			tmptime = time(NULL) - m_pSockMgr.at(conindex)->m_nReadtime;
			if ( tmptime > m_pRes->g_nHeartRun && tmptime < m_pRes->g_nDisconnectTime)
			{
				OnPing(conindex);
			}
			else if (tmptime > m_pRes->g_nDisconnectTime)
			{
#ifdef _ENGLISH_
				OnClose(conindex,"connect unuse long time,clost it");
#else
				OnClose(conindex,"���ӳ�ʱ��δ�ã��Ͽ�");
#endif
			}
		}

	}// end if (ret == 1) //�ж��¼�
}

void CPoolModule::OnWriteEvent(int conindex)
{
	int ret;
#if defined(_WINDOWS)
	if (m_pSockMgr.at(conindex)->m_sock == INVALID_SOCKET)
	{
		return;
	}
	ret = FD_ISSET(m_pSockMgr.at(conindex)->m_sock, &m_eset);
	if (ret != 0)
	{
#ifdef _ENGLISH_
		OnClose(conindex,"write socket error");
#else
		OnClose(conindex,"дsocketʧ��");
#endif
	}
	else if (FD_ISSET(m_pSockMgr.at(conindex)->m_sock, &m_wset) !=0 )
	{
		OnSend(conindex);//��������
	}
	else  
	{
		OnSend(conindex);//��������
	}
#else
	if (m_pSockMgr.at(conindex)->m_sock == INVALID_SOCKET)
	{
		return;
	}
	int ok;
	int oklen;
	if (FD_ISSET(m_pSockMgr.at(conindex)->m_sock, &m_wset) !=0)
	{
		OnSend(conindex);//��������
	}
	else  
	{
		OnSend(conindex);//��������
	}
#endif

				
}


void CPoolModule::OnPing(int conindex)
{
	if (time(NULL) - m_pSockMgr.at(conindex)->m_nPingTime < BEATHEARTTICK) //С��5�벻�÷�ping
	{
		return ;
	}
	m_pSockMgr.at(conindex)->m_nPingTime = time(NULL);
	
	//����Ҫд������
	S_TRADE_DATA data;
	if (!ResetData(conindex,&data))
	{
		return;
	}
	//ǰ���ĸ��ֽڵı���ͷ
	strcpy(data.pData->sBuffer,"0000");
	data.nWantlen = 4;

	//����ping���ݰ�
	m_pSockMgr.at(conindex)->SendMsg(data,0);
	
	
}

void CPoolModule::OnClose(int conindex,const char *msg)
{
	if (m_pSockMgr.at(conindex)->m_sock == INVALID_SOCKET)
	{
		return;
	}
//	m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ر�����%s:%d:%d %d %s",m_pSockMgr.at(conindex)->m_sAddress,\
		m_pSockMgr.at(conindex)->m_sock,conindex,m_pSockMgr.at(conindex)->m_nType,msg);
	//��socket�ر�
	m_pSockMgr.at(conindex)->OnClose(msg);
}


void CPoolModule::OnSend(int conindex)
{
	m_pSockMgr.at(conindex)->OnSend();
}

void CPoolModule::OnAccept(int conindex)
{
	char address[32];
	SOCKET_HANDLE tsock=INVALID_SOCKET;
	int i;
	bool havefree=false;
	m_tcpSocket.AttachSocket(m_pSockMgr.at(conindex)->m_sock,"");
	tsock= m_tcpSocket.Accept(address,false);
	if (tsock != INVALID_SOCKET)
	{
		for (i=1; i<m_pRes->g_nMaxConnectNum; i++)
		{
			if ( m_pSockMgr.at(i)->m_sock == INVALID_SOCKET )
			{
				havefree = true;
				m_pSockMgr.at(i)->SetAccept(tsock,address);
				m_pSockMgr.at(i)->SetClient();
				break;
			}
		}
	
		if (havefree)
		{
#ifdef _ENGLISH_
			m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"Accept %s socket[%d] index[%d]",address,tsock,i);
#else
			m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"���մ�%s������%d index[%d]",address,tsock,i);
#endif
		}
		else  //�ͻ�������
		{
#ifdef _ENGLISH_
			m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"at max connect [%d]��%s socket %d refuse",m_pRes->g_nMaxConnectNum,address,tsock);
#else
			m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"�ﵽ������ӹ���[%d]����%s������%d���ܾ�",m_pRes->g_nMaxConnectNum,address,tsock);
#endif
			closesocket(tsock);
		}
	}
}

void CPoolModule::OnRecv(int conindex)
{
	int ret=0;
	int i=0;
	
	ret = m_pSockMgr.at(conindex)->OnRecv();	
	if (ret<=0)
	{
#ifdef _ENGLISH_
		OnClose(conindex,"far-end close");
		m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"far-end close");
#else
		OnClose(conindex,"Զ�˹رգ��������ݲ�����");
		m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"Զ�˹رգ��������ݲ�����");
#endif
		return;
	}

	//	printf("OnRecv %d\n",ret);
//	m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�������� ret=%d",ret);
//	std::string gtpdata;
	do 
	{
		if (!ResetData(conindex,&m_pDataRcv))
		{
			return;
		}
		ret = m_pSockMgr.at(conindex)->GetRecvData(&m_pDataRcv);
		if (ret>0)
		{
#ifdef _ENGLISH_
			m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"index[%d] Recv data",conindex);
			m_log->LogBin(LOG_DEBUG,__FILE__,__LINE__,"Recv data",m_pDataRcv.pData->sBuffer,m_pDataRcv.nWantlen);
#else
			m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"index[%d] �յ���������",conindex);
			m_log->LogBin(LOG_DEBUG,__FILE__,__LINE__,"�յ���������",m_pDataRcv.pData->sBuffer,m_pDataRcv.nWantlen);
#endif
			m_pDataRcv.rTime = time(NULL);
			//������У�������
			m_pRcvQueue->PushData(m_pDataRcv);

	
		}
		else
		{
			m_pMemPool->PoolFree(m_pDataRcv.pData);
			m_pDataRcv.pData = NULL;
			break;
		}
	} while (ret > 0 );

	
	return ;
}


bool CPoolModule::ResetData(int index,S_TRADE_DATA *data)
{
	data->nSendlen=0;
	data->nWantlen=0;
	data->src = 1;
	data->index = index;
	data->rTime = time(NULL);
	data->pData = (PBPCCOMMSTRU)m_pMemPool->PoolMalloc();
	if (data->pData == NULL)
	{
#ifdef _ENGLISH_
		m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"PoolMalloc error");
		m_pSockMgr.at(index)->OnClose("PoolMalloc error");
#else
		m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"������Ϣ�ռ����");
		m_pSockMgr.at(index)->OnClose("������Ϣ�ռ����");
#endif
		return false;
	}
	return true;
}

void CPoolModule::Monitor()
{
	if (IsStoped())
	{
#ifdef _ENGLISH_
		printf("Restart CPoolModule thread %d\n",m_bIsRunning);
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"Restart CPoolModule thread %d",m_bIsRunning);
#else
		printf("����CPoolModule�߳� %d\n",m_bIsRunning);
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"����CPoolModule�߳� %d",m_bIsRunning);
#endif
		m_bIsRunning = true;
		CreateThread();
	}
	if (m_pTimer.IsStoped())
	{
#ifdef _ENGLISH_
		printf("Restart timer thread %d\n",m_bIsRunning);
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"Restart timer thread %d",m_bIsRunning);
#else
		printf("������ʱ���߳�\n");
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"������ʱ���߳�");
#endif
		m_pTimer.Start();
	}
}

int CPoolModule::OnTimer(unsigned int eventid, void *p)
{
	CPoolModule *pp = (CPoolModule *)p;
	if (eventid == 0)
	{
		pp->PingTimer();
	}
	return 0;
}
void CPoolModule::PingTimer()
{
	//OnNoEvent();
//	m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"��ʱ��");
	m_bIsPingTimer = true;
}

bool CPoolModule::StartFront()
{
	//�������շ����߳�
	m_bIsRunning = true;
	CreateThread();
	
	
	m_pTimer.Start();
	
	SLEEP_SECONDS(2);
	return true;
}
