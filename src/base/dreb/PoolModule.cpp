// PoolModule.cpp: implementation of the CPoolModule class.
//
//////////////////////////////////////////////////////////////////////

#include "PoolModule.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern char g_VERSION[20];




CPoolModule::CPoolModule()
{
	m_sThreadName = "CPoolModule";
	m_bIsInit = false;
	m_nEnd = 0;
	m_nDrebHeadLen = sizeof(DREB_HEAD);
	m_nBegin=0;
	m_nEnd=1;
	m_pRes=NULL;
	m_pSocketMgr=NULL;
	m_pMemPool=NULL;
	m_pMsgProc = NULL;
	m_bIsPingTimer= false;
	m_bIsRouteTimer=false;
	m_bIsServiceTimer=false;

}

CPoolModule::~CPoolModule()
{
	m_pRes=NULL;
	m_pSocketMgr=NULL;
	m_pMemPool=NULL;
}
void CPoolModule::ExitThreadInstance()
{
	return ;
}
bool CPoolModule::InitThreadInstance()
{
	return true;
}
int CPoolModule::OnTimer(unsigned int eventid, void *p)
{
	CPoolModule *pp = (CPoolModule *)p;
	if (eventid == 0)
	{
		//�ڵ�·��
		pp->RouteTimer();
	}
	else if (eventid == 1 )
	{
		//����·��
		pp->ServiceRouteTimer();
	}
	else if (eventid == 2)
	{
		pp->PingTimer();
	}

	return 0;
}
bool CPoolModule::SetGlobalVar(CGlobalVar *pVar, CSocketMgr *sockmgr, CMemDB *pMemDb, CBF_BufferPool *pool,CPoolData *msgqueue)
{
	m_pRes=pVar;
	m_log = &(m_pRes->g_pLog);
	//m_pMemDb=pMemDb;
	m_pSocketMgr=sockmgr;
	m_pMemPool=pool;
	m_pMsgQueue = msgqueue;
	

	m_pTimer.Init(100,true);
	//�ڵ�·�ɶ�ʱ��
	m_pTimer.SetTimer(0,m_pRes->g_nDrebRouteTime*1000,&CPoolModule::OnTimer,this); //���ö�ʱ�� 
	//����·�ɶ�ʱ��
	m_pTimer.SetTimer(1,m_pRes->g_nServiceRouteTime*1000,&CPoolModule::OnTimer,this);
	//PING��ʱ��
	m_pTimer.SetTimer(2,BEATHEARTTICK*1000,&CPoolModule::OnTimer,this);

	return true;
}
int CPoolModule::Run()
{
	struct timeval tv;
	int result;
	int maxfd=0;
	int lasttime=time(NULL);
	int lasttime2=time(NULL);
	int curtime;
	m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"���������߳� %d",m_pRes->g_bIsExit);
	while (!m_pRes->g_bIsExit)
	{
		FD_ZERO(&m_rset);
		FD_ZERO(&m_wset);
#if defined(_WINDOWS)
		FD_ZERO(&m_eset);
#endif
		tv.tv_sec = 0;
		tv.tv_usec = 50;
		//ȡ������socket����socket���뵽fd_set����
		GetMaxfdAddSelect(maxfd);
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

		if (result != 0) 
		{
			//���¼�����������
			OnEvent();
		}
		if (m_bIsPingTimer)
		{
			OnNoEvent();
			OnMonitor();
			m_bIsPingTimer = false;

		}
		if (m_bIsRouteTimer)
		{
//			m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"��������ROUTE");
			OnRoute();
			m_bIsRouteTimer = false;
		}
		if (m_bIsServiceTimer)
		{
			if (m_pRes->g_nRegOtherDreb == 1) //ע��
			{
//				m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"��������SERVICE ROUTE");
				OnServiceRoute();
			}
			m_bIsServiceTimer = false;
		}

	}

	m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"ֹͣ�����߳� %d",m_pRes->g_bIsExit);
	return 0;
}

int CPoolModule::GetMaxfdAddSelect(int &maxfd)
{
	int ret=0;
	maxfd = 0;
	for (int i=m_nBegin;i<m_nEnd;i++)
	{
		if (!m_pSocketMgr->at(i)->IsCanSelect())
		{
			ret++;
		}
		else
		{
			//��Ҫ������
			if (m_pSocketMgr->at(i)->NeedRead())
			{
				FD_SET(m_pSocketMgr->at(i)->m_sock, &m_rset);//���뵽��select����
			}
			//��Ҫ�������ݻ���������ʱ
			if (m_pSocketMgr->at(i)->NeedSend() || m_pSocketMgr->at(i)->NeedConnect())
			{
				FD_SET(m_pSocketMgr->at(i)->m_sock, &m_wset);//���뵽дselect����
			}
#if defined(_WINDOWS)
			if (m_pSocketMgr->at(i)->NeedConnect())
			{
				FD_SET(m_pSocketMgr->at(i)->m_sock, &m_eset);//���뵽�����쳣����
			}
#endif
			//�õ�����socket 
			if (m_pSocketMgr->at(i)->m_sock > maxfd)
			{
				maxfd=m_pSocketMgr->at(i)->m_sock; 
			}
		}
	}
	return ret;
}

void CPoolModule::OnNoEvent()
{
	int j;
	for (j=1; j<m_nEnd; j++)
	{
		if (m_pRes->g_bIsExit)
		{
			return;
		}
		if (m_pRes->g_nBeatHeart == 1) //������⣬��Է���������ڵ��������ģ��Ƿ��������������
		{
			//����������Ҫ�������
			if (m_pSocketMgr->at(j)->m_sock != INVALID_SOCKET && m_pSocketMgr->at(j)->m_bChecked && m_pSocketMgr->at(j)->NeedRead())
			{
				//����������ڵ�������
				if (m_pSocketMgr->at(j)->m_nType == SOCK_DREB || (m_pSocketMgr->at(j)->m_nType == SOCK_SRV  ))
				{
					//û�ж���������ʱ�䳬���������
					if (time(NULL)-m_pSocketMgr->at(j)->m_nReadtime >= BEATHEARTTICK*(m_pSocketMgr->at(j)->m_nBeatheartcount+1)) //�೤ʱ��û�ж������ݾͷ���ping��ر�
					{
						//������������û�г���ָ����������������
						if (m_pSocketMgr->at(j)->m_nBeatheartcount < (m_pRes->g_nHeartCount-1))
						{
							m_pSocketMgr->at(j)->m_nBeatheartcount++;
							OnPing(j);//��������
						}
						else
						{
							OnClose(j,"������",__FILE__,__LINE__);
						}
					}
					else   //�Ƿ�û�з�������
					{
						//����ָ��ʱ����û�з������ݾͷ�������
						if (time(NULL)-m_pSocketMgr->at(j)->m_nSendtime >= BEATHEARTTICK) 
						{
							OnPing(j);
						}
					}
				}  //end if (m_pSocketMgr->at(j)->m_nType == SOCK_DREB || (m_pSocketMgr->at(j)->m_nType == SOCK_SRV  ))
			}  // end if (m_pSocketMgr->at(j)->m_sock != INVALID_SOCKET && m_pSocketMgr->at(j)->m_bChecked && m_pSocketMgr->at(j)->m_bNeedread)
		}
		//����������Ϊ�������߽ڵ㻥��
		if (m_pSocketMgr->at(j)->m_nType == SOCK_TODREB  && m_pSocketMgr->at(j)->m_sock != INVALID_SOCKET)
		{
			//����û�м��ͨ��
			if (!m_pSocketMgr->at(j)->m_bChecked) 
			{
				if (time(NULL)-m_pSocketMgr->at(j)->m_nReadtime >= m_pRes->g_nHeartCount*BEATHEARTTICK)
				{
					if (m_pSocketMgr->at(j)->m_s_center.bNeedConnect)
					{
						m_log->LogMp(2,__FILE__,__LINE__,"�������ӵ�%s:%dδ�ɹ� index=%d",m_pSocketMgr->at(j)->m_s_center.sAddress,m_pSocketMgr->at(j)->m_s_center.nPort,j);
						closesocket(m_pSocketMgr->at(j)->m_sock);
						m_pSocketMgr->at(j)->m_sock = INVALID_SOCKET;
					}
					else  //����SOCK_TODREB��Ҫ�������ӵ����
					{
						OnClose(j,"Э�̳�ʱ",__FILE__,__LINE__);
					}
				}
			}
		}
		if (m_pSocketMgr->at(j)->m_sock != INVALID_SOCKET &&  !m_pSocketMgr->at(j)->m_bChecked)
		{
			if (time(NULL)-m_pSocketMgr->at(j)->m_nReadtime >= m_pRes->g_nHeartCount*BEATHEARTTICK)
			{
				OnClose(j,"���ӷǷ�",__FILE__,__LINE__);
			} 
		}
		if (m_pSocketMgr->at(j)->m_sock != INVALID_SOCKET &&  m_pSocketMgr->at(j)->m_nType == SOCK_CLI)
		{
			if (time(NULL)-m_pSocketMgr->at(j)->m_nReadtime >= m_pRes->g_nDisconnectTime)
			{
				OnClose(j,"���ӳ�ʱ��δ�ã���ʱ�Ͽ�",__FILE__,__LINE__);
			} 
		}
		//�����������߽ڵ����ӣ���û��������ȥ����
		if (m_pSocketMgr->at(j)->m_nType == SOCK_TODREB )
		{
			if (m_pSocketMgr->at(j)->m_sock == INVALID_SOCKET)
			{
				OnConnect(j);
			}
		}
	}
	
}

void CPoolModule::OnEvent()
{
	int ret;
	int i;
	if (m_nBegin == 0)
	{
		ret = FD_ISSET(m_pSocketMgr->at(0)->m_sock,&m_rset);
		if (ret != 0) //����������˿��ж����¼�����ȥ����accept
		{
			OnAccept(0);
		}
	}
	for (i= 1; i<m_nEnd; i++)
    {
		if (m_pRes->g_bIsExit)
		{
			return;
		}
		if (m_pSocketMgr->at(i)->m_sock == INVALID_SOCKET)//δ����
		{
			if (m_pSocketMgr->at(i)->m_nType == SOCK_TODREB   ) //�����������߽ڵ����ӣ���ȥ����
			{
				OnConnect(i);
			}
			continue;
		}
		else
		{
			//��Ӧ���¼�
			OnReadEvent(i);
			if (m_pSocketMgr->at(i)->m_sock != INVALID_SOCKET)//δ����
			{
				//��Ӧд�¼�
				OnWriteEvent(i);
			}
		}
	}
}



void CPoolModule::OnReadEvent(int conindex)
{
	int ret;
	ret = FD_ISSET(m_pSocketMgr->at(conindex)->m_sock,&m_rset);
	if (ret != 0) //�ж��¼�
	{
		OnRecv(conindex);
	}
}

void CPoolModule::OnWriteEvent(int conindex)
{
	int ret;
	if (m_pSocketMgr->at(conindex)->m_sock == INVALID_SOCKET)
	{
		return;
	}
#if defined(_WINDOWS)
	ret = FD_ISSET(m_pSocketMgr->at(conindex)->m_sock, &m_eset);
	if (ret != 0)
	{
		//��������������DREB
		if (m_pSocketMgr->at(conindex)->m_nType == SOCK_TODREB)
		{
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"�������ӵ�%s:%dδ�ɹ� index=%d",m_pSocketMgr->at(conindex)->m_s_center.sAddress,m_pSocketMgr->at(conindex)->m_s_center.nPort,conindex);
			closesocket(m_pSocketMgr->at(conindex)->m_sock);
			m_pSocketMgr->at(conindex)->m_sock = INVALID_SOCKET;
		}
		else 
		{
			OnClose(conindex,"Other connect fail",__FILE__,__LINE__);
		}
	}
	else if (FD_ISSET(m_pSocketMgr->at(conindex)->m_sock, &m_wset) !=0 )
	{
		if (m_pSocketMgr->at(conindex)->NeedConnect())
		{
			if (m_pSocketMgr->at(conindex)->m_nType == SOCK_TODREB ) 
			{
				OnConnected(conindex);
			}
		}
		else if (m_pSocketMgr->at(conindex)->m_bChecked)
		{
			OnSend(conindex);//��������
		}
	}
	else  if (m_pSocketMgr->at(conindex)->m_bChecked)//socket��д�¼�
	{
//		OnSend(conindex);//��������
	}
#else
	
	int ok;
	int oklen;
	if (FD_ISSET(m_pSocketMgr->at(conindex)->m_sock, &m_wset) !=0)
	{
		if (m_pSocketMgr->at(conindex)->NeedConnect())
		{
			oklen=sizeof(ok);
#if defined(HP_UX)
			getsockopt(m_pSocketMgr->at(conindex)->m_sock,SOL_SOCKET,SO_ERROR,(char *)&ok,&oklen);
#else	
			getsockopt(m_pSocketMgr->at(conindex)->m_sock,SOL_SOCKET,SO_ERROR,(char *)&ok,(socklen_t *)&oklen);
#endif
			if (ok == 0)
			{
				if (m_pSocketMgr->at(conindex)->m_nType == SOCK_TODREB)
				{
					OnConnected(conindex);
				}
			}
			else
			{
				if (m_pSocketMgr->at(conindex)->m_nType == SOCK_TODREB)
				{
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"�������ӵ�%s:%dδ�ɹ� index=%d",m_pSocketMgr->at(conindex)->m_s_center.sAddress,m_pSocketMgr->at(conindex)->m_s_center.nPort,conindex);
					closesocket(m_pSocketMgr->at(conindex)->m_sock);
					m_pSocketMgr->at(conindex)->m_sock = INVALID_SOCKET;
				}
			} // end if (ok == 0)
		}
		else if (m_pSocketMgr->at(conindex)->m_bChecked)
		{
			OnSend(conindex);//��������
		} // end if (g_connInfoList[conindex]->m_bNeedconnect)
	}
	else  if (m_pSocketMgr->at(conindex)->m_bChecked)
	{
//		OnSend(conindex);//��������
	}
#endif

				
}

void CPoolModule::OnPing(int conindex)
{
	if (!m_pSocketMgr->at(conindex)->m_bChecked)//����δȷ�ϣ����÷�����
	{
		return;
	}
// 	if (time(NULL) - m_pSocketMgr->at(conindex)->m_nPingTime < BEATHEARTTICK) //С��5�벻�÷�ping
// 	{
// 		return ;
// 	}

	m_pSocketMgr->at(conindex)->m_nPingTime = time(NULL);

	S_DREB_RSMSG *recvmessage = (S_DREB_RSMSG *)m_pMemPool->PoolMalloc();
	if (recvmessage == NULL)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"�����ڴ�ռ����");
		return ;
	}
	bzero(&(recvmessage->message.head), sizeof(DREB_HEAD));
	recvmessage->msghead.msgtype=MSG_PING;
	recvmessage->msghead.index=conindex;
	recvmessage->msghead.connecttime=m_pSocketMgr->at(conindex)->m_nConntime;
	recvmessage->msghead.socket=m_pSocketMgr->at(conindex)->m_sock;
	recvmessage->msghead.sockettype = m_pSocketMgr->at(conindex)->m_nType;
	recvmessage->msghead.nTotalLen = 0;
	//����ping���ݰ�
	
	
	recvmessage->message.head.cRaflag = 0;
	recvmessage->message.head.cZip = 0;
	recvmessage->message.head.nLen = 0;
	recvmessage->message.head.cCmd = CMD_PING;
	recvmessage->message.head.s_Sinfo.s_nIndex = conindex;
	recvmessage->message.head.s_Sinfo.s_cNodePrivateId = m_pRes->g_nDrebPrivateId;	
	recvmessage->message.head.s_Sinfo.s_nNodeId = m_pRes->g_nDrebId;
	recvmessage->message.head.s_Sinfo.s_nTimestamp = recvmessage->msghead.connecttime;
	

	//20140515 ������̴߳����˴�SendMsg��Ӧ��Ҫ�ӻ���
	if (m_pSocketMgr->at(conindex)->SendMsg(recvmessage) <0 )
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"����CMD_PING���� %d",conindex);
	}
}

void CPoolModule::OnClose(int conindex,const char *msg,const char *filename,int fileline)
{
	if (m_pSocketMgr->at(conindex)->m_sock == INVALID_SOCKET)
	{
		return;
	}
	m_log->LogMp(LOG_INFO,filename,fileline,"�ر�����%s:%d index=%d type=%d %s",\
		m_pSocketMgr->at(conindex)->m_sAddress,m_pSocketMgr->at(conindex)->m_sock,\
		conindex,m_pSocketMgr->at(conindex)->m_nType,msg);
	if (m_pSocketMgr->at(conindex)->m_nType == SOCK_CLI || m_pSocketMgr->at(conindex)->m_nType== SOCK_SLOT \
		|| m_pSocketMgr->at(conindex)->m_nType == SOCK_UNKNOW)
	{
		//20140515 ������̴߳����˴�OnClose��Ӧ��Ҫ�ӻ��⣬��ֹ�����߳����ڷ���
		m_pSocketMgr->at(conindex)->OnClose(msg,filename,fileline);
		return;
	}
	S_DREB_RSMSG *recvmessage = (S_DREB_RSMSG *)m_pMemPool->PoolMalloc();
	if (recvmessage == NULL)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"�����ڴ�ռ����");
		return ;
	}
	bzero(&(recvmessage->message.head), sizeof(DREB_HEAD));
	recvmessage->msghead.index=conindex;
	recvmessage->msghead.msgtype=MSG_CLOSE;
	recvmessage->msghead.socket=m_pSocketMgr->at(conindex)->m_sock;
	recvmessage->msghead.connecttime=m_pSocketMgr->at(conindex)->m_nConntime;
	recvmessage->msghead.sockettype = m_pSocketMgr->at(conindex)->m_nType;
	recvmessage->msghead.nTotalLen = 0;
	
	if (recvmessage->msghead.sockettype == SOCK_TODREB)
	{
		recvmessage->msghead.closeinfo.d_nNodeId = m_pSocketMgr->at(conindex)->m_s_center.nNodeId;
		recvmessage->msghead.closeinfo.d_cNodePrivateId = m_pSocketMgr->at(conindex)->m_s_center.cNodePrivateId;
	}
	else if (recvmessage->msghead.sockettype == SOCK_DREB)
	{
		recvmessage->msghead.closeinfo.d_nNodeId = m_pSocketMgr->at(conindex)->m_s_conn.nNodeId;
		recvmessage->msghead.closeinfo.d_cNodePrivateId = m_pSocketMgr->at(conindex)->m_s_conn.cNodePrivateId;
	}
	else if (recvmessage->msghead.sockettype == SOCK_SRV)
	{
		recvmessage->msghead.closeinfo.d_cSvrPrivateId = m_pSocketMgr->at(conindex)->m_s_srv.cSvrPrivateId;
		recvmessage->msghead.closeinfo.d_nSvrMainId = m_pSocketMgr->at(conindex)->m_s_srv.nSvrMainId;
	}
	//֪ͨ��Ϣ�����̣߳���ЩҪ����
	OnMsgProc(recvmessage);
}
void CPoolModule::OnConnected(int conindex)
{
	//������dreb������
	if (m_pSocketMgr->at(conindex)->m_sock != INVALID_SOCKET) //���ӳɹ�
	{
		//���Է���������
		m_pSocketMgr->at(conindex)->m_bNeedread = true;
		m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"������%s:%d�����ӳɹ� index=%d",m_pSocketMgr->at(conindex)->m_s_center.sAddress,m_pSocketMgr->at(conindex)->m_s_center.nPort,conindex);
		//���ӳɹ�����CMD_CONNECT��Ϣ��ȷ�����ӵ�����
		m_pSocketMgr->at(conindex)->m_s_center.bNeedConnect = false;
		m_pSocketMgr->at(conindex)->m_nPingTime = time(NULL);
		//����������Ϣ
		S_DREB_RSMSG *recvmessage = (S_DREB_RSMSG *)m_pMemPool->PoolMalloc();
		if (recvmessage == NULL)
		{
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"�����ڴ�ռ����");
			return ;
		}
		bzero(&(recvmessage->message.head), sizeof(DREB_HEAD));
		recvmessage->msghead.msgtype = MSG_CONNECT;
		recvmessage->msghead.index=conindex;
		recvmessage->msghead.connecttime= m_pSocketMgr->at(conindex)->m_nConntime;
		recvmessage->msghead.socket=m_pSocketMgr->at(conindex)->m_sock;
		recvmessage->msghead.sockettype = m_pSocketMgr->at(conindex)->m_nType;
		
		if (m_pSocketMgr->at(conindex)->m_pSendQueue == NULL)
		{
			m_pSocketMgr->at(conindex)->m_pSendQueue = new CSendQueue();
			if (m_pSocketMgr->at(conindex)->m_pSendQueue == NULL)
			{
				m_pSocketMgr->at(conindex)->OnClose("���䷢�Ͷ��пռ����",__FILE__,__LINE__);
				m_pMemPool->PoolFree(recvmessage);
				return;
			}
		}

		recvmessage->message.head.cRaflag = 0;
		recvmessage->message.head.cZip = 0;
		recvmessage->message.head.cCmd = CMD_CONNECT;
		recvmessage->message.head.cDrebAffirm = 0;
		recvmessage->message.head.s_Sinfo.s_nIndex = conindex;
		recvmessage->message.head.s_Sinfo.s_cNodePrivateId = m_pRes->g_nDrebPrivateId;	
		recvmessage->message.head.s_Sinfo.s_nNodeId = m_pRes->g_nDrebId;
		recvmessage->message.head.s_Sinfo.s_nTimestamp = recvmessage->msghead.connecttime;
		
		recvmessage->message.head.nLen = sizeof(S_CONNECT_DATA);
		S_CONNECT_DATA *data = (S_CONNECT_DATA *)recvmessage->message.buffer;
		data->cConnectType = 2;
		data->cNodePrivateId = m_pRes->g_nDrebPrivateId;
		data->nNodeId = m_pRes->g_nDrebId;
		data->nBandWidth = m_pSocketMgr->at(conindex)->m_s_center.nbandwidth;
		sprintf(data->sVersion,"%s",g_VERSION);
		
		m_pDrebEndian.Endian2Comm((unsigned char *)&(data->nNodeId),sizeof(data->nNodeId));
		m_pDrebEndian.Endian2Comm((unsigned char *)&(data->nBandWidth),sizeof(data->nBandWidth));
		
		//20140515 �������ϣ�����ֱ�ӷ������û���Ҳ����
		if (m_pSocketMgr->at(conindex)->SendMsg(recvmessage,true)< 0 )
		{
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"����CMD_CONNECT���� %d",conindex);
		}
		return;
	}
	m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"��ô���ܣ����ӳɹ���SOCKET�Ƿ�");
	return;
}

void CPoolModule::OnConnect(int conindex)
{
	//ȥ����
	if (time(NULL) - m_pSocketMgr->at(conindex)->m_nConntime < 5 ) //��10���ڲ�������
	{
		return;
	}
	int ret;
	m_pSocketMgr->at(conindex)->m_nConntime = time(NULL);
	m_tcpSocket.Create(AF_INET,false);//�첽������ʽ
	m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"�������ӵ��������߽ڵ�%s:%d:%d index=%d",m_pSocketMgr->at(conindex)->m_s_center.sAddress,m_pSocketMgr->at(conindex)->m_s_center.nPort,m_pSocketMgr->at(conindex)->m_sock,conindex);
	if (m_tcpSocket.ConnectServer(m_pSocketMgr->at(conindex)->m_s_center.sAddress,m_pSocketMgr->at(conindex)->m_s_center.nPort))
	{
		
		m_pSocketMgr->at(conindex)->m_nReadtime = time(NULL);
		m_pSocketMgr->at(conindex)->m_sock = m_tcpSocket.GetSocket();
	}
	else
	{
		if ((m_tcpSocket.GetErrorNo() == EWOULDBLOCK) || (m_tcpSocket.GetErrorNo() == EINPROGRESS))
		{
			m_pSocketMgr->at(conindex)->m_nReadtime = time(NULL);
			m_pSocketMgr->at(conindex)->m_sock = m_tcpSocket.GetSocket();
			m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"�ȴ�������%s:%d������ index=%d",m_pSocketMgr->at(conindex)->m_s_center.sAddress,m_pSocketMgr->at(conindex)->m_s_center.nPort,conindex);
			
			return;
		}
		else
		{
			m_pSocketMgr->at(conindex)->m_sock = INVALID_SOCKET;
			m_tcpSocket.Close();
		}
	}
	return;

}

void CPoolModule::OnSend(int conindex)
{
	//20140515 ������̴߳����˴�OnSend��Ӧ��Ҫ�ӻ��⣬��ֹ�����߳����ڷ���
	m_pSocketMgr->at(conindex)->OnSend();
}

void CPoolModule::OnAccept(int conindex)
{
	char address[32];
	SOCKET_HANDLE tsock=INVALID_SOCKET;
	int i;
	bool havefree=false;
	m_tcpSocket.AttachSocket(m_pSocketMgr->at(conindex)->m_sock,"127.0.0.1");
	tsock= m_tcpSocket.Accept(address,false);
	if (tsock != INVALID_SOCKET)
	{
		for (i=1; i<m_pRes->g_nMaxConnCount; i++)
		{
			//�����������߽ڵ������������δʹ��
			if ((m_pSocketMgr->at(i)->m_sock == INVALID_SOCKET) && (m_pSocketMgr->at(i)->m_nType != SOCK_TODREB))
			{
				havefree = true;
				//�������ӵĲ���
				m_pSocketMgr->at(i)->SetAccept(tsock,address);//�ɶ���δ���
				break;
			}
		}
		if (havefree)
		{
			m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"���մ�%s������socket[%d] index[%d]",address,tsock,i);
		}
		else
		{
			m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"�ﵽ������ӹ�����%s������socket[%d]���ܾ�",address,tsock);
			closesocket(tsock);
		}
	}
}

void CPoolModule::OnRecv(int conindex)
{
	int ret=0;
	char tmpchar[200];
	//�ж��¼��Ž��뵽����
	ret = m_pSocketMgr->at(conindex)->OnRecv();	
	if (ret <=0 )
	{
		//�϶������ӹر���
		sprintf(tmpchar,"%s Զ�˹رգ��������ݲ����� ret=[%d]",m_pSocketMgr->at(conindex)->m_sAddress,ret);
		OnClose(conindex,tmpchar,__FILE__,__LINE__);
		return;
	}
	//�н��յ���������������Ϊ0
	m_pSocketMgr->at(conindex)->m_nBeatheartcount=0;
	//�յ������ˣ�ѭ�����յ������ݴ�����
	while(m_pSocketMgr->at(conindex)->m_nRcvBufLen >= DREBHEADLEN)
	{
		S_DREB_RSMSG *recvmessage = (S_DREB_RSMSG *) m_pMemPool->PoolMalloc();
		if (recvmessage == NULL)
		{
			m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"������Ϣ�ռ�ʱ�����ڴ治��");
			return ;
		}
		ret = m_pSocketMgr->at(conindex)->GetRecvData(recvmessage);
		if (ret < 0) 
		{
			m_pMemPool->PoolFree(recvmessage);
			OnClose(conindex,"CRCУ���",__FILE__,__LINE__);
//			m_pRes->LogTime();
			return;
		}
		else if (ret == 0) //����δ��������
		{
			m_pMemPool->PoolFree(recvmessage);
			m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"����δ�������� �������ݳ���[%d]",m_pSocketMgr->at(conindex)->m_nRcvBufLen);
//			m_pRes->LogTime();
			return;
		}
		m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"��ʼ�������� �������ݳ���[%d]",m_pSocketMgr->at(conindex)->m_nRcvBufLen);
		if (recvmessage->message.head.cRaflag == 1 && (recvmessage->message.head.cCmd == CMD_DPABC || recvmessage->message.head.cCmd == CMD_DPPUSH))
		{
			m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"CMD_DPABC��CMD_DPPUSH��Ӧ��,����");
			m_pMemPool->PoolFree(recvmessage);
//			m_pRes->LogTime();
			continue;
		}
		if (recvmessage->message.head.cRaflag == 1)
		{
			//һЩӦ����cmd_connect�ȣ����ø���Ϣ�����߳���
			// #define  CMD_ROUTER      1  //�������߽ڵ㷢��·��֪ͨ��������
			// #define  CMD_DEL_NODE    2  //�������߽ڵ�֮��ȡ���������߽ڵ��������,ֻ�������������ӵ��������߽ڵ�
			// #define  CMD_DEL_SVR     3  //�����ȡ��ע������������
			// #define  CMD_PING        4  //���������������
			// #define  CMD_CONNECT     5  //����ע��
			// #define  CMD_SERVICE     6  //����·��
			// #define  CMD_REGSERVICE  7  //ע�����
			if ( recvmessage->message.head.cCmd == CMD_ROUTER || recvmessage->message.head.cCmd == CMD_DEL_NODE || \
				recvmessage->message.head.cCmd == CMD_DEL_SVR || recvmessage->message.head.cCmd == CMD_PING || \
				recvmessage->message.head.cCmd == CMD_SERVICE || recvmessage->message.head.cCmd == CMD_REGSERVICE )
			{
				m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"Ӧ�����봦��[%s]",GetDrebCmdType(recvmessage->message.head.cCmd).c_str());
				m_pMemPool->PoolFree(recvmessage);
//				m_pRes->LogTime();
				continue;
			}
		}
		recvmessage->msghead.index = conindex;
		recvmessage->msghead.connecttime = m_pSocketMgr->at(conindex)->m_nConntime;
		recvmessage->msghead.msgtype = MSG_READ;
		recvmessage->msghead.socket = m_pSocketMgr->at(conindex)->m_sock;
		recvmessage->msghead.sockettype = m_pSocketMgr->at(conindex)->m_nType;
		m_log->LogMp(LOG_DEBUG + 1, __FILE__, __LINE__, "OnMsgProc MSG_READ cCmd[%s]", GetDrebCmdType(recvmessage->message.head.cCmd).c_str());
		OnMsgProc(recvmessage);
//		m_pRes->LogTime();
		m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"������һ������ �������ݳ���[%d]",m_pSocketMgr->at(conindex)->m_nRcvBufLen);
	}
	return ;
}


void CPoolModule::OnRoute()
{
	if (m_pRes->g_bIsExit)
	{
		return;
	}
	S_DREB_RSMSG *recvmessage = (S_DREB_RSMSG *) m_pMemPool->PoolMalloc();
	if (recvmessage == NULL)
	{
		m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"������Ϣ�ռ�ʱ�����ڴ治��");
		return ;
	}
	recvmessage->msghead.msgtype=MSG_ROUTE;
	recvmessage->msghead.index=-1;
	OnMsgProc(recvmessage);
}

void CPoolModule::OnMsgProc(S_DREB_RSMSG *msg)
{
	if (m_pRes->g_bMsgProcThread)
	{
		m_pMsgQueue->PushData(msg);
	}
	else
	{
		m_pMsgProc->Run(msg);
	}
}

void CPoolModule::PrintRoute()
{
	
}

void CPoolModule::OnServiceRoute()
{
	if (m_pRes->g_bIsExit)
	{
		return;
	}
	S_DREB_RSMSG *recvmessage = (S_DREB_RSMSG *) m_pMemPool->PoolMalloc();
	if (recvmessage == NULL)
	{
		m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"������Ϣ�ռ�ʱ�����ڴ治��");
		return ;
	}
	recvmessage->msghead.msgtype=MSG_SERVICE;
	recvmessage->msghead.index=-1;
	OnMsgProc(recvmessage);
}
std::string CPoolModule::GetDrebCmdType(int cmdtype)
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
	case  CMD_MONITOR_BPC:     //15 //�������߽ڵ���
		return "CMD_MONITOR_BPC";
	default:
		return "";
	}
}
void CPoolModule::LogDrebHead(int loglevel, DREB_HEAD head, const char *msg, const char *filename, int fileline)
{
	bzero(m_headMsg,sizeof(m_headMsg));
	sprintf(m_headMsg,"%s DREBHEAD��Ϣ cZip=%d cCmd=%s cRaflag=%d cNextFlag=%d cDrebAffirm=%d s_nNodeId=%d \
	   s_cNodePrivateId=%d s_nSvrMainId=%d s_cSvrPrivateId=%d s_nHook=%d s_nSerial=%d \
	   s_nDrebSerial=%d s_nIndex=%d d_nNodeId=%d d_cNodePrivateId=%d d_nSvrMainId=%d \
	   d_cSvrPrivateId=%d d_nServiceNo=%d a_nNodeId=%d a_cNodePrivateId=%d a_nRetCode=%d \
	   n_nNextNo=%d n_nNextOffset=%d b_nSerial=%d b_cIndex=%d nLen=%d","%s",head.cZip,GetDrebCmdType(head.cCmd).c_str(),\
	   head.cRaflag,head.cNextFlag,head.cDrebAffirm,head.s_Sinfo.s_nNodeId,head.s_Sinfo.s_cNodePrivateId,\
	   head.s_Sinfo.s_nSvrMainId,head.s_Sinfo.s_cSvrPrivateId,head.s_Sinfo.s_nHook,head.s_Sinfo.s_nSerial,\
	   head.s_Sinfo.s_nDrebSerial,head.s_Sinfo.s_nIndex,head.d_Dinfo.d_nNodeId,head.d_Dinfo.d_cNodePrivateId,\
	   head.d_Dinfo.d_nSvrMainId,head.d_Dinfo.d_cSvrPrivateId,head.d_Dinfo.d_nServiceNo,\
	   head.a_Ainfo.a_nNodeId,head.a_Ainfo.a_cNodePrivateId,head.a_Ainfo.a_nRetCode,\
	   head.n_Ninfo.n_nNextNo,head.n_Ninfo.n_nNextOffset,head.b_Cinfo.b_nSerial,\
	   head.b_Cinfo.b_cIndex,head.nLen);
	m_log->LogMp(loglevel,filename,fileline,m_headMsg,msg);
}

void CPoolModule::RouteTimer()
{
	//ͨѶ�ڵ�·��
	//OnRoute();
	m_bIsRouteTimer = true;
}

void CPoolModule::ServiceRouteTimer()
{
	//����·��
	//OnServiceRoute();
	m_bIsServiceTimer = true;
}

void CPoolModule::PingTimer()
{
	//OnNoEvent();
	m_bIsPingTimer = true;
}

void CPoolModule::StartTimer()
{
	m_pTimer.Start();
}

void CPoolModule::StopTimer()
{
	m_pTimer.Stop();
}

void CPoolModule::OnMonitor()
{
	if (m_pRes->g_nUseMonitor != 1) 
	{
		return;
	}
	if (m_pRes->g_bIsExit)
	{
		return;
	}
	S_DREB_RSMSG *recvmessage = (S_DREB_RSMSG *) m_pMemPool->PoolMalloc();
	if (recvmessage == NULL)
	{
		m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"������Ϣ�ռ�ʱ�����ڴ治��");
		return ;
	}
	bzero(&(recvmessage->message.head),sizeof(DREB_HEAD));
	recvmessage->msghead.msgtype=MSG_MONITOR;
	recvmessage->msghead.index=-1;
	OnMsgProc(recvmessage);
}
