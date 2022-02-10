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
	memset(&m_pDataBuf,0,sizeof(S_BF_RSMSG_FPC));
	memset(&m_pDataRcv,0,sizeof(S_BF_RSMSG_FPC));
	
	m_pPara = NULL;
	m_log=NULL;
}

CPoolModule::~CPoolModule()
{
	m_pTimer.Stop();
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
		

		FD_SET(m_pSockMgr.at(0)->m_sock, &m_rset);//���뵽��select����
		if (m_pSockMgr.at(0)->m_sock > maxfd)
		{
			maxfd = m_pSockMgr.at(0)->m_sock;
		}
		FD_SET(m_pSockMgr.at(1)->m_sock, &m_rset);//���뵽��select����
		if (m_pSockMgr.at(1)->m_sock > maxfd)
		{
			maxfd = m_pSockMgr.at(1)->m_sock;
		}
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
		
		if (m_pRes.g_bToExit)
		{
			break;
		}

	}
	m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"�˳����շ����߳�");
	return 0;
}

bool CPoolModule::InitFront(char *confile,void *para)
{
	m_pPara = para;
	if (!m_pRes.Init(confile))
	{
		printf("ǰ�ó�ʼ��ʧ�� %s\n",m_pRes.GetMsg().c_str());
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"ǰ�ó�ʼ��ʧ�� %s",m_pRes.GetMsg().c_str());
		return false;
	}
	m_log = &(m_pRes.m_log);
	
	m_pPoolData.m_pRes = &m_pRes;
	//�ڴ����ش�С�������С
	m_pMemPool.Init(100,BFBUFSIZE);
	//��ʼ�����ӳ�
	if (!m_pSockMgr.Init(&m_pRes,&m_pMemPool))
	{
		sprintf(m_errMsg,"��ʼ��ʧ��:%s",m_pSockMgr.m_errMsg);
		printf("%s \n",m_errMsg);
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
		m_bIsInit = false;
		return false;
	}
	if (!m_pSockMgr.at(0)->Listen(m_pRes.g_nPort,false,30))
	{
		sprintf(m_errMsg,"��ʼ��ʧ��:����%d sokcet����ʧ��",m_pRes.g_nPort);
		printf("%s \n",m_errMsg);
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
		m_bIsInit = false;
		return false;
	}
	if (!m_pSockMgr.at(1)->Listen(m_pRes.g_nBuPort,false,30))
	{
		sprintf(m_errMsg,"��ʼ��ʧ��:����%d BUsokcet����ʧ��",m_pRes.g_nBuPort);
		printf("%s \n",m_errMsg);
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
		m_bIsInit = false;
		return false;
	}
	if (!m_pProcessMgr.SetGlobalVar(&m_pRes,&m_pPoolData,&m_pMemPool,&m_pSockMgr,m_pPara))
	{
		sprintf(m_errMsg,"%s",m_pProcessMgr.GetErrMsg().c_str());
		printf("%s \n",m_errMsg);
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
		m_bIsInit = false;
		return false;
	}

	//��ʼ�����������߳�
	if (!m_pProcessMgr.InitPmgr())
	{
		sprintf(m_errMsg,"%s",m_pProcessMgr.GetErrMsg().c_str());
		printf("%s \n",m_errMsg);
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
		m_bIsInit = false;
		return false;
	}
	//�������շ����߳�
	m_bIsRunning = true;
	CreateThread();

	m_pTimer.Init(100,true);
	//�ڵ�·�ɶ�ʱ��
	m_pTimer.SetTimer(0,5000,&CPoolModule::OnTimer,this); //���ö�ʱ�� 
	m_pTimer.Start();

	SLEEP_SECONDS(2);
//	m_pProcessMgr.Monitor();//����BU
	return true;
}

void CPoolModule::StopFront()
{
	printf("StopFront!\n");
	m_bIsRunning = false;
	m_pRes.g_bToExit = true;
	printf("��ʼֹͣ�����߳�!\n");
	m_pProcessMgr.Stop();
	m_pRes.m_log.StopLog();
	printf("�����߳���ֹͣ!\n");
}

int CPoolModule::GetMaxfdAddSelect(int &maxfd)
{
	int ret=0;
	maxfd = 0;

	for (int i=2;i<m_pRes.g_nMaxConnectNum+m_pRes.g_nProcessThreadNum;i++)
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
			if (m_pSockMgr.at(i)->m_sock != INVALID_SOCKET && m_pSockMgr.at(i)->NeedSend())
			{
				FD_SET(m_pSockMgr.at(i)->m_sock, &m_wset);//���뵽дselect����
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
	for (j=2;j<m_pRes.g_nMaxConnectNum+m_pRes.g_nProcessThreadNum;j++)
	{
		if (m_pSockMgr.at(j)->m_sock != INVALID_SOCKET && m_pSockMgr.at(j)->m_nType == SOCK_CLI)
		{
			tmptime = time(NULL) - m_pSockMgr.at(j)->m_nReadtime;
			if ( tmptime > m_pRes.g_nHeartRun && tmptime < m_pRes.g_nDisconnectTime)
			{
				OnPing(j);
			}
			else if (tmptime > m_pRes.g_nDisconnectTime)
			{
				OnClose(j,"���ӳ�ʱ��δ�ã��Ͽ�");
			}
		}
		if (m_pSockMgr.at(j)->m_sock != INVALID_SOCKET && m_pSockMgr.at(j)->m_nType == SOCK_BU)
		{
			tmptime = time(NULL) - m_pSockMgr.at(j)->m_nReadtime;
			if (tmptime > m_pRes.g_nDisconnectTime)
			{
				OnClose(j,"���ӳ�ʱ��δ�ã��Ͽ�");
			}
		}
		if (m_pRes.g_bToExit)
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
	ret = FD_ISSET(m_pSockMgr.at(1)->m_sock,&m_rset);
	if (ret != 0) //����������˿��ж����¼�����ȥ����accept
	{
		OnAccept(1);
	}
	for (i= 2; i< m_pRes.g_nMaxConnectNum+m_pRes.g_nProcessThreadNum; i++)
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
		if (m_pRes.g_bToExit)
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
			if ( tmptime > m_pRes.g_nHeartRun && tmptime < m_pRes.g_nDisconnectTime)
			{
				OnPing(conindex);
			}
			else if (tmptime > m_pRes.g_nDisconnectTime)
			{
				OnClose(conindex,"���ӳ�ʱ��δ�ã��Ͽ�");
			}
		}
		if (m_pSockMgr.at(conindex)->m_sock != INVALID_SOCKET && m_pSockMgr.at(conindex)->m_nType == SOCK_BU)
		{
			tmptime = time(NULL) - m_pSockMgr.at(conindex)->m_nReadtime;
			if (tmptime > m_pRes.g_nDisconnectTime)
			{
				OnClose(conindex,"���ӳ�ʱ��δ�ã��Ͽ�");
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
		OnClose(conindex,"дsocketʧ��");
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
	if (!ResetData(conindex,&m_pDataBuf))
	{
		return;
	}
	strcpy(m_pDataBuf.msgbuf.message->buffer,"xt");
	m_pDataBuf.msgbuf.head.len = 2;
	//����ping���ݰ�
	m_pSockMgr.at(conindex)->SendMsg(m_pDataBuf);
	
	
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
		if (conindex == 0) //�ͻ�������
		{
			for (i=2; i<m_pRes.g_nMaxConnectNum; i++)
			{
				if ( m_pSockMgr.at(i)->m_sock == INVALID_SOCKET )
				{
					havefree = true;
					m_pSockMgr.at(i)->SetAccept(tsock,address);
					m_pSockMgr.at(i)->SetClient();
					break;
				}
			}
		}
		else //BU����
		{
			for (i=m_pRes.g_nMaxConnectNum; i<m_pRes.g_nMaxConnectNum+m_pRes.g_nProcessThreadNum; i++)
			{
				if ( m_pSockMgr.at(i)->m_sock == INVALID_SOCKET )
				{
					havefree = true;
					m_pSockMgr.at(i)->SetAccept(tsock,address);
					m_pSockMgr.at(i)->SetBuClient();
					break;
				}
			}
		}
		
		if (havefree)
		{
			if (conindex == 0) 
			{
				m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���մ�%s������%d:%d",address,tsock,i);
			}
			else
			{
				m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���մ�%s��BU����%d:%d",address,tsock,i);
			}
		}
		else if (conindex == 0) //�ͻ�������
		{
			m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"�ﵽ������ӹ�����%s������%d���ܾ�",address,tsock);
			closesocket(tsock);
		}
		else
		{
			m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"BU������������%s������%d���ܾ�",address,tsock);
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
		OnClose(conindex,"Զ�˹رգ��������ݲ�����");
		m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"Զ�˹رգ��������ݲ�����");
		return;
	}

	//	printf("OnRecv %d\n",ret);

	do 
	{
		if (!ResetData(conindex,&m_pDataRcv))
		{
			return;
		}
		ret = m_pSockMgr.at(conindex)->GetRecvData(&m_pDataRcv);
		m_log->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"GetRecvData���� %d",ret);
		if (ret>0)
		{
			if (m_pSockMgr.at(conindex)->m_nType == SOCK_BU) //����FPU������
			{
				m_pDataRcv.msgbuf.head.buindex = conindex;
				m_log->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"BUͷ��Ϣ index=%d conntime=%d msgtype=%d",conindex,\
					m_pDataRcv.msgbuf.head.connecttime,m_pDataRcv.msgbuf.head.msgtype);
				m_log->LogBin(LOG_DEBUG+2,__FILE__,__LINE__,"�յ�BU����������",m_pDataRcv.msgbuf.message->buffer,m_pDataRcv.msgbuf.head.len);
				ProcessFpu(m_pDataRcv);
			}
			else  //���յ���������  ͨ�����н��������߳����ַ���FPU
			{
				m_log->LogBin(LOG_DEBUG+2,__FILE__,__LINE__,"�յ���������",m_pDataRcv.msgbuf.message->buffer,m_pDataRcv.msgbuf.head.len);
				m_pDataRcv.msgbuf.head.rtime = time(NULL);
				m_pPoolData.PushData(m_pDataRcv);
			}
			
		}
		else
		{
			m_pMemPool.PoolFree(m_pDataRcv.msgbuf.message);
			m_pDataRcv.msgbuf.message = NULL;
			break;
		}
	} while (ret <=0 );

	
	return ;
}


bool CPoolModule::ResetData(int index,S_BF_RSMSG_FPC *data)
{
	memset(data,0,sizeof(S_BF_RSMSG_FPC));
	data->msgbuf.head.connecttime = m_pSockMgr.at(index)->m_nConntime;
	data->msgbuf.head.index = index;
	data->msgbuf.head.rtime = time(NULL);
	data->msgbuf.message = (COMMSTRU_FPC *)m_pMemPool.PoolMalloc();
	if (data->msgbuf.message == NULL)
	{
		m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"������Ϣ�ռ����");
		m_pSockMgr.at(index)->OnClose("������Ϣ�ռ����");
		return false;
	}
	return true;
}

void CPoolModule::Monitor()
{
	if (IsStoped())
	{
		printf("����CPoolModule�߳� %d\n",m_bIsRunning);
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"����CPoolModule�߳� %d",m_bIsRunning);
		m_bIsRunning = true;
		CreateThread();
	}
	m_pProcessMgr.Monitor();
	if (m_pRes.g_bIsMonitor)
	{
		int allsize=0;
		int unuse=0;
		m_pMemPool.PoolInfo(allsize,unuse);
		m_log->LogMp(LOG_PROMPT,__FILE__,__LINE__,"�ڴ滺����[%d]  ������[%d] ������[%d]",allsize,unuse,m_pPoolData.GetSize());
	}
	
}

void CPoolModule::ProcessFpu(S_BF_RSMSG_FPC &rcvdata)
{
	switch (rcvdata.msgbuf.head.msgtype)
	{
	case MSG_FREE:
		DispatchBuFree(rcvdata);
		break;
	case MSG_BUANS:
		DispatchBuAns(rcvdata);
		//m_pPoolData.PushData(rcvdata);
		break;
	case MSG_BUANSFREE:
		DispatchBuAnsFree(rcvdata);
		break;
	case MSG_GETSERIAL:
		GetSerial(rcvdata);
		break;
	default:
		m_pMemPool.PoolFree(rcvdata.msgbuf.message);
		rcvdata.msgbuf.message = NULL;
		break;
	}
	return ;
}
void CPoolModule::DispatchBuAns(S_BF_RSMSG_FPC &rcvdata)
{
	if (m_pSockMgr.AffirmIndex(rcvdata.msgbuf.head.index))
	{
		if (m_pSockMgr.at(rcvdata.msgbuf.head.index)->m_sock != INVALID_SOCKET )
		{
			m_pSockMgr.at(rcvdata.msgbuf.head.index)->SendMsg(rcvdata);
			return;
		}
	}
	m_pMemPool.PoolFree(rcvdata.msgbuf.message);
	rcvdata.msgbuf.message = NULL;
	return;
}

void CPoolModule::DispatchBuFree(S_BF_RSMSG_FPC &rcvdata)
{
	if (m_pSockMgr.AffirmIndex(rcvdata.msgbuf.head.buindex))
	{
		m_pSockMgr.at(rcvdata.msgbuf.head.buindex)->m_bIsBuGetFlag = true;
		m_pSockMgr.at(rcvdata.msgbuf.head.buindex)->m_nPid = rcvdata.msgbuf.head.rtime;
	}
	
	m_pMemPool.PoolFree(rcvdata.msgbuf.message);
	rcvdata.msgbuf.message = NULL;
}

void CPoolModule::DispatchBuAnsFree(S_BF_RSMSG_FPC &rcvdata)
{
	if (m_pSockMgr.AffirmIndex(rcvdata.msgbuf.head.buindex))
	{
		m_pSockMgr.at(rcvdata.msgbuf.head.buindex)->m_bIsBuGetFlag = true;
	}
	if (m_pSockMgr.AffirmIndex(rcvdata.msgbuf.head.index))
	{
		if (m_pSockMgr.at(rcvdata.msgbuf.head.index)->m_sock != INVALID_SOCKET )
		{
			m_pSockMgr.at(rcvdata.msgbuf.head.index)->SendMsg(rcvdata);
			return;
		}
	}
	m_pMemPool.PoolFree(rcvdata.msgbuf.message);
	rcvdata.msgbuf.message = NULL;
	return;
}

void CPoolModule::GetSerial(S_BF_RSMSG_FPC &rcvdata)
{
	sprintf(rcvdata.msgbuf.message->buffer,"%ld",m_pRes.GetThreadShareSerial());
	rcvdata.msgbuf.head.len = strlen(m_pDataRcv.msgbuf.message->buffer);
	m_pSockMgr.at(rcvdata.msgbuf.head.index)->SendMsg(rcvdata);
	
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
	m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"��ʱ��");
	m_bIsPingTimer = true;
}
