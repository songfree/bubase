// BF_DrebServer.cpp: implementation of the CBF_DrebServer class.
//
//////////////////////////////////////////////////////////////////////

#include "BF_DrebServer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
char g_DrebApiVERSION[20]="2.0.2.20220616";   //api�İ汾

#define  DESENCRYPTKEY    "rd402wms"
#define  DESENCRYPTKEY3   "wms13711songfree"

CBF_DrebServer::CBF_DrebServer()
{
	m_nBegin =0;
	m_nEnd = 0;
	m_pRes = NULL;
	m_bIsRunning = false;
	m_bIsInit = false;
	m_sThreadName = "CBF_DrebServer";
	m_bIsPingTimer = false;
	
	bzero(&m_sHostInfo,sizeof(m_sHostInfo));
	m_nCurIndex = 0;
	m_spi = NULL;
}

CBF_DrebServer::~CBF_DrebServer()
{
	
}

void * CBF_DrebServer::PoolMalloc()
{
	return m_pMemPool.PoolMalloc();
}

void CBF_DrebServer::PoolFree(void *ppoint)
{
	m_pMemPool.PoolFree(ppoint);
}

bool CBF_DrebServer::Init(CBF_DrebResource *res, CDrebMsgProcBase* spi)
{
	if (m_bIsInit)
	{
		sprintf(m_errMsg,"�Ѿ���ʼ����");
		return false;
	}
	
	m_pRes = res;
	m_pLog.SetMaxLogSize(m_pRes->g_nMaxLogSize);
	m_pLog.SetLogPara(m_pRes->g_nLoglevel,m_pRes->g_sLogFilePath.c_str(),m_pRes->g_sLogFileName.c_str());
	
	
	char tmpchar1[100];
	char tmpchar2[100];
	char tmpchar3[100];
	char tmpchar4[100];
	sprintf(tmpchar1,"srv%d-%d.DREB2BPC.dat",m_pRes->g_nSvrMainId,m_pRes->g_nSvrPrivateId);
	sprintf(tmpchar2,"srv%d-%d.BPC2BPU.dat",m_pRes->g_nSvrMainId,m_pRes->g_nSvrPrivateId);
	sprintf(tmpchar3,"srv%d-%d.BPC2DREB.dat",m_pRes->g_nSvrMainId,m_pRes->g_nSvrPrivateId);
	sprintf(tmpchar4,"srv%d-%d.BPU2BPC.dat",m_pRes->g_nSvrMainId,m_pRes->g_nSvrPrivateId);

	m_pDrebDataLog.SetLogPara(m_pRes->g_nMaxLogSize,m_pRes->g_nDrebLoglevel,m_pRes->g_sLogFilePath.c_str(),tmpchar1,tmpchar2);
	m_pBpcCallDataLog.SetLogPara(m_pRes->g_nMaxLogSize,m_pRes->g_nDrebLoglevel,m_pRes->g_sLogFilePath.c_str(),tmpchar3,tmpchar4);

	if (!m_pSockMgr.Init(m_pRes,&m_pMemPool,&m_pLog,&m_pDrebDataLog,&m_pBpcCallDataLog))
	{
		return false;
	}
	if (!m_pMemPool.Init(10,BPCBUFSIZE))
	{
		return false;
	}
	m_nBegin = 0;
	m_nEnd = m_pRes->g_vDrebLinkInfo.size();

	//���ö�ʱ��
	m_pTimer.Init(100,true);
	//����ÿ���������Ķ�ʱ��
	m_pTimer.SetTimer(0,1000,&CBF_DrebServer::OnTimer,this); //���ö�ʱ�� ÿ��1���������
	//ȡ������Դ��Ϣ�Ķ�ʱ��������noevent��ʱ��
	m_pTimer.SetTimer(1,5000,&CBF_DrebServer::OnTimer,this);//ÿ��5��ȥȡһ��cpu���ڴ桢����

	if (!m_pLog.StartLog())
	{
		sprintf(m_errMsg,"������־�߳�ʧ��");
		return false;
	}
	m_spi = spi;
	m_bIsInit = true;
	return true;
}
void CBF_DrebServer::ExitThreadInstance()
{
	return ;
}
bool CBF_DrebServer::InitThreadInstance()
{
	return true;
}

int CBF_DrebServer::Run()
{
	struct timeval tv;
	int result;
	int i,j;
	int maxfd=0;
	int ret;
	int lasttime=time(NULL);
	int lasttime2=time(NULL);
	if (!m_bIsInit)
	{
		sprintf(m_errMsg,"δ��ʼ�����˳��߳�");
		m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
		return -1;
	}
#ifdef _ENGLISH_
	m_pLog.LogMp(LOG_INFO,__FILE__,__LINE__,"Start DrebServer thread");
#else
	m_pLog.LogMp(LOG_INFO,__FILE__,__LINE__,"����DrebServer�߳�");
#endif
	while(m_bIsRunning)
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
			for (int i=0; i<m_pRes->g_vDrebLinkInfo.size() ;i++)
			{
				OnConnect(i);
			}
			SLEEP(5);
//			m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"��dreb���� %d",tv.tv_sec);
			continue;
		}

#if defined(_WINDOWS)
		result = select(maxfd+1, &m_rset, &m_wset, &m_eset, &tv);
#else
		result = select(maxfd+1, &m_rset, &m_wset, NULL, &tv);
#endif
		
		if (result != 0) //�¼�����,����
		{
			OnEvent();
		}
		
		if (m_bIsPingTimer)
		{
			OnNoEvent();
			OnMonitor();
			m_bIsPingTimer = false;
		}
		
	}
#ifdef _ENGLISH_
	m_pLog.LogMp(LOG_INFO,__FILE__,__LINE__,"Stop DrebServer thread");
#else
	m_pLog.LogMp(LOG_INFO,__FILE__,__LINE__,"ֹͣDrebServer�߳�");
#endif
	return 0;
}
int CBF_DrebServer::GetMaxfdAddSelect(int &maxfd)
{
	int ret=0;
	maxfd = 0;
	for (int i=m_nBegin;i<m_nEnd;i++)
	{
		if (!m_pSockMgr.at(i)->IsCanSelect())
		{
			continue;
		}
		else
		{
			//��Ҫ������
			if (m_pSockMgr.at(i)->NeedRead())
			{
				FD_SET(m_pSockMgr.at(i)->m_sock, &m_rset);//���뵽��select����
			}
			//��Ҫ�������ݻ���������ʱ
			if (m_pSockMgr.at(i)->NeedSend() || m_pSockMgr.at(i)->NeedConnect())
			{
				FD_SET(m_pSockMgr.at(i)->m_sock, &m_wset);//���뵽дselect����
			}
#if defined(_WINDOWS)
			if (m_pSockMgr.at(i)->NeedConnect())
			{
				FD_SET(m_pSockMgr.at(i)->m_sock, &m_eset);//���뵽�����쳣����
			}
#endif
			//�õ�����socket 
			if (m_pSockMgr.at(i)->m_sock > maxfd)
			{
				maxfd=m_pSockMgr.at(i)->m_sock; 
			}
			ret++;
		}
	}
	return ret;
}
void CBF_DrebServer::OnConnect(int conindex)
{
	if (m_pSockMgr.at(conindex)->m_sock != INVALID_SOCKET) //���ӳɹ�
	{
		return ;
	}
	if (!m_bIsRunning)
	{
		return;
	}
	//ȥ����
	if (time(NULL) - m_pSockMgr.at(conindex)->m_nConntime < 5 ) //��10���ڲ�������
	{
		return;
	}
	int ret;
	m_pSockMgr.at(conindex)->m_bChecked = false;
	m_pSockMgr.at(conindex)->m_nType = BPCSOCK_DREB;
	
	m_pSockMgr.at(conindex)->m_nConntime = time(NULL);
	m_tcpSocket.Create(AF_INET,false);//�첽��ʽ
#ifdef _ENGLISH_
	m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"try to connect DREB %s:%d:%d",\
		m_pSockMgr.at(conindex)->m_sDrebIp.c_str(),m_pSockMgr.at(conindex)->m_nDrebPort,\
		m_pSockMgr.at(conindex)->m_sock);
#else
	m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"�������ӵ�DREB�ڵ�%s:%d:%d",\
		m_pSockMgr.at(conindex)->m_sDrebIp.c_str(),m_pSockMgr.at(conindex)->m_nDrebPort,\
		m_pSockMgr.at(conindex)->m_sock);
#endif
	if (m_tcpSocket.ConnectServer((char *)m_pSockMgr.at(conindex)->m_sDrebIp.c_str(),m_pSockMgr.at(conindex)->m_nDrebPort))
	{
		
		m_pSockMgr.at(conindex)->m_nReadtime = time(NULL);
		m_pSockMgr.at(conindex)->m_sock = m_tcpSocket.GetSocket();
	}
	else
	{
		if ((m_tcpSocket.GetErrorNo() == EWOULDBLOCK) || (m_tcpSocket.GetErrorNo() == EINPROGRESS))
		{
			m_pSockMgr.at(conindex)->m_nReadtime = time(NULL);
			m_pSockMgr.at(conindex)->m_sock = m_tcpSocket.GetSocket();
#ifdef _ENGLISH_
			m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"wait for connect DREB %s:%d",m_pSockMgr.at(conindex)->m_sDrebIp.c_str(),m_pSockMgr.at(conindex)->m_nDrebPort);
#else
			m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ȴ�������DREB %s:%d������",m_pSockMgr.at(conindex)->m_sDrebIp.c_str(),m_pSockMgr.at(conindex)->m_nDrebPort);
#endif
			return;
		}
		else
		{
			m_pSockMgr.at(conindex)->m_sock = INVALID_SOCKET;
			m_tcpSocket.Close();
#ifdef _ENGLISH_
			m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"connect to DREB %s:%d error, errno:%d",m_pSockMgr.at(conindex)->m_sDrebIp.c_str(),m_pSockMgr.at(conindex)->m_nDrebPort,m_tcpSocket.GetErrorNo());
#else
			m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"������DREB %s:%d������ʧ�� errno:%d",m_pSockMgr.at(conindex)->m_sDrebIp.c_str(),m_pSockMgr.at(conindex)->m_nDrebPort,m_tcpSocket.GetErrorNo());
#endif
		}
	}
	return;
	
}
void CBF_DrebServer::OnConnected(int conindex)
{
	if (m_pSockMgr.at(conindex)->m_sock != INVALID_SOCKET) //���ӳɹ�
	{
		//���Է���������
		m_pSockMgr.at(conindex)->m_bNeedread = true;
#ifdef _ENGLISH_
		m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"connect DREB %s:%d success",m_pSockMgr.at(conindex)->m_sDrebIp.c_str(),m_pSockMgr.at(conindex)->m_nDrebPort);
#else
		m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"������DREB %s:%d�����ӳɹ�",m_pSockMgr.at(conindex)->m_sDrebIp.c_str(),m_pSockMgr.at(conindex)->m_nDrebPort);
#endif
		//���ӳɹ�����CMD_CONNECT��Ϣ��ȷ�����ӵ�����
		m_pSockMgr.at(conindex)->m_bNeedConnect = false;
		//����������Ϣ
		m_pSockMgr.at(conindex)->m_nPingTime = time(NULL);
		if (!ReMallocData(conindex,&m_pDataBuf))
		{
			return;
		}
		//��������ָ���DREB
		m_pDataBuf.sMsgBuf->sDBHead.cRaflag = 0;
		m_pDataBuf.sMsgBuf->sDBHead.cZip = 0;
		m_pDataBuf.sMsgBuf->sDBHead.cCmd = CMD_CONNECT;
		m_pDataBuf.sMsgBuf->sDBHead.cDrebAffirm = 0;
		m_pDataBuf.sMsgBuf->sDBHead.nLen = sizeof(S_CONNECT_DATA);
		S_CONNECT_DATA *data = (S_CONNECT_DATA *)m_pDataBuf.sMsgBuf->sBuffer;
		data->cConnectType = 1;
		data->nSvrMainId = m_pRes->g_nSvrMainId;
		data->cSvrPrivateId = m_pRes->g_nSvrPrivateId;
		sprintf(data->sVersion,"%s",g_DrebApiVERSION);
		m_pDrebEndian.Endian2Comm((unsigned char *)&(data->nSvrMainId),sizeof(data->nSvrMainId));
		m_pDataBuf.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN+m_pDataBuf.sMsgBuf->sDBHead.nLen;
		m_pSockMgr.at(conindex)->SendMsg(m_pDataBuf,true);
		//��״̬ͳһ�ڴ���Ϣ�ķ��ش���������
		
		return ;
	}
	return;
	
}
void CBF_DrebServer::OnEvent()
{
	int ret;
	int i;
	for (i= m_nBegin; i<m_nEnd; i++)
	{
		if (m_pSockMgr.at(i)->m_sock == INVALID_SOCKET)//δ����
		{
			if (m_pSockMgr.at(i)->m_nType == BPCSOCK_DREB   ) //�����������߽ڵ����ӣ���ȥ����
			{
				OnConnect(i);
			}
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
	}
}
void CBF_DrebServer::OnNoEvent()
{
	int j;
	int tmptime;
	for (j=m_nBegin; j<m_nEnd; j++)
	{
		if (m_pSockMgr.at(j)->m_sock != INVALID_SOCKET && m_pSockMgr.at(j)->m_nType == BPCSOCK_DREB)
		{
			tmptime = time(NULL) - m_pSockMgr.at(j)->m_nReadtime;
			if ( tmptime > m_pRes->g_nHeartRun && tmptime < m_pRes->g_nDisconnectTime)
			{
				OnPing(j);
			}
			else if (tmptime > m_pRes->g_nDisconnectTime)
			{
#ifdef _ENGLISH_
				OnClose(j,"connect unuse long time,close it",__FILE__,__LINE__);
#else
				OnClose(j,"���ӳ�ʱ��δ�ã��Ͽ�",__FILE__,__LINE__);
#endif
			}
		}
		if (!m_bIsRunning)
		{
			return;
		}
		if (m_pSockMgr.at(j)->m_sock != INVALID_SOCKET)
		{
			//û�м��ͨ��
			if (!m_pSockMgr.at(j)->m_bChecked)
			{
				//�������߽ڵ�����
				if (m_pSockMgr.at(j)->m_nType == BPCSOCK_DREB) 
				{
					if (time(NULL)-m_pSockMgr.at(j)->m_nReadtime >= m_pRes->g_nHeartRun)
					{
						if (m_pSockMgr.at(j)->NeedConnect())
						{
#ifdef _ENGLISH_
							m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"connect DREB %s:%d fail",m_pSockMgr.at(j)->m_sDrebIp.c_str(),m_pSockMgr.at(j)->m_nDrebPort);
#else
							m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"�������ӵ�DREB %s:%dδ�ɹ�",m_pSockMgr.at(j)->m_sDrebIp.c_str(),m_pSockMgr.at(j)->m_nDrebPort);
#endif
							closesocket(m_pSockMgr.at(j)->m_sock);
							m_pSockMgr.at(j)->m_sock = INVALID_SOCKET;
						}
					}
				}// end if (m_pSockMgr.at(conindex)->m_nType == BPCSOCK_TODREB) //�������߽ڵ�����
				else if (m_pSockMgr.at(j)->m_nType == BPCSOCK_UNKNOW)
				{
					if (time(NULL)-m_pSockMgr.at(j)->m_nReadtime >= m_pRes->g_nHeartRun)
					{
#ifdef _ENGLISH_
						OnClose(j,"connect illegal",__FILE__,__LINE__);
#else
						OnClose(j,"���ӷǷ�",__FILE__,__LINE__);
#endif
					} 
				}
				else 
				{
#ifdef _ENGLISH_
					m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"connect index[%d] type[%d] sock[%d]",\
						j,m_pSockMgr.at(j)->m_nType,m_pSockMgr.at(j)->m_sock);
					if (time(NULL)-m_pSockMgr.at(j)->m_nReadtime >= m_pRes->g_nHeartRun)
					{
						OnClose(j,"connect timeout",__FILE__,__LINE__);
					} 
#else
					m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"����index[%d] type[%d] sock[%d]",\
						j,m_pSockMgr.at(j)->m_nType,m_pSockMgr.at(j)->m_sock);
					if (time(NULL)-m_pSockMgr.at(j)->m_nReadtime >= m_pRes->g_nHeartRun)
					{
						OnClose(j,"Э�̳�ʱ",__FILE__,__LINE__);
					} 
#endif
				}
			}
		}// end if (m_pSockMgr.at(conindex)->m_sock != INVALID_SOCKET)
		if (!m_bIsRunning)
		{
			return;
		}
		//�����������߽ڵ����ӣ���û��������ȥ����
		if (m_pSockMgr.at(j)->m_nType == BPCSOCK_DREB )
		{
			if (m_pSockMgr.at(j)->m_sock == INVALID_SOCKET)
			{
				OnConnect(j);
			}
		}
	}
	
}
bool CBF_DrebServer::ReMallocData(int index,S_BPC_RSMSG *data)
{
	data->sMsgBuf = (PBPCCOMMSTRU )m_pMemPool.PoolMalloc();
	if (data->sMsgBuf == NULL)
	{
#ifdef _ENGLISH_
		m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"malloc msgdata error");
		m_pSockMgr.at(index)->OnClose("malloc msgdata error",__FILE__,__LINE__);
#else
		m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"������Ϣ�ռ����");
		m_pSockMgr.at(index)->OnClose("������Ϣ�ռ����",__FILE__,__LINE__);
#endif
		return false;
	}
	bzero(&(data->sMsgBuf->sDBHead), sizeof(DREB_HEAD));
	bzero(&(data->sMsgBuf->sBpcHead), sizeof(S_BPC_HEAD));
	data->sMsgBuf->sBpcHead.nConnectTime= m_pSockMgr.at(index)->m_nConntime;
	data->sMsgBuf->sBpcHead.nIndex = index;
	data->sMsgBuf->sBpcHead.nRtime = time(NULL);
	
	return true;
}
void CBF_DrebServer::OnReadEvent(int conindex)
{
	int ret;
	ret = FD_ISSET(m_pSockMgr.at(conindex)->m_sock,&m_rset);
	if (ret != 0) //�ж��¼�
	{
		OnRecv(conindex);
	}
}
void CBF_DrebServer::OnWriteEvent(int conindex)
{
	int ret;
	if (m_pSockMgr.at(conindex)->m_sock == INVALID_SOCKET)
	{
		return;
	}
#if defined(_WINDOWS)
	ret = FD_ISSET(m_pSockMgr.at(conindex)->m_sock, &m_eset);
	if (ret != 0)
	{
		//��������������DREB
		if (m_pSockMgr.at(conindex)->m_nType == BPCSOCK_DREB)
		{
#ifdef _ENGLISH_
			m_pLog.LogMp(2,__FILE__,__LINE__,"connect DREB %s:%d fail",m_pSockMgr.at(conindex)->m_sDrebIp.c_str(),m_pSockMgr.at(conindex)->m_nDrebPort);
#else
			m_pLog.LogMp(2,__FILE__,__LINE__,"�������ӵ�DREB %s:%dδ�ɹ�",m_pSockMgr.at(conindex)->m_sDrebIp.c_str(),m_pSockMgr.at(conindex)->m_nDrebPort);
#endif
			closesocket(m_pSockMgr.at(conindex)->m_sock);
			m_pSockMgr.at(conindex)->m_sock = INVALID_SOCKET;
		}
		else 
		{
#ifdef _ENGLISH_
			OnClose(conindex,"connect DREB Other fail",__FILE__,__LINE__);
#else
			OnClose(conindex,"�������ӵ�DREB Other connect fail",__FILE__,__LINE__);
#endif
		}
	}
	else if (FD_ISSET(m_pSockMgr.at(conindex)->m_sock, &m_wset) !=0 )
	{
		if (m_pSockMgr.at(conindex)->NeedConnect())
		{
			if (m_pSockMgr.at(conindex)->m_nType == BPCSOCK_DREB ) 
			{
				OnConnected(conindex);//ȥ�����������߽ڵ�
			}
		}
		else if (m_pSockMgr.at(conindex)->m_bChecked)
		{
			OnSend(conindex);//��������
		}
	}
	else  if (m_pSockMgr.at(conindex)->m_bChecked) //socket��д�¼�
	{
//		OnSend(conindex);//��������
	}
#else
	
	int ok;
	int oklen;
	if (FD_ISSET(m_pSockMgr.at(conindex)->m_sock, &m_wset) !=0)
	{
		if (m_pSockMgr.at(conindex)->NeedConnect())
		{
			oklen=sizeof(ok);
#if defined(HP_UX)
			getsockopt(m_pSockMgr.at(conindex)->m_sock,SOL_SOCKET,SO_ERROR,(char *)&ok,&oklen);
#else
			getsockopt(m_pSockMgr.at(conindex)->m_sock,SOL_SOCKET,SO_ERROR,(char *)&ok,(socklen_t *)&oklen);
#endif
			if (ok == 0)
			{
				if (m_pSockMgr.at(conindex)->m_nType == BPCSOCK_DREB)
				{
					OnConnected(conindex);//ȥ�����������߽ڵ�
				}
			}
			else
			{
				if (m_pSockMgr.at(conindex)->m_nType == BPCSOCK_DREB)
				{
#ifdef _ENGLISH_
					m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"connect DREB %s:%d fail",m_pSockMgr.at(conindex)->m_sDrebIp.c_str(),m_pSockMgr.at(conindex)->m_nDrebPort);
#else
					m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"�������ӵ�DREB %s:%dδ�ɹ�",m_pSockMgr.at(conindex)->m_sDrebIp.c_str(),m_pSockMgr.at(conindex)->m_nDrebPort);
#endif
					closesocket(m_pSockMgr.at(conindex)->m_sock);
					m_pSockMgr.at(conindex)->m_sock = INVALID_SOCKET;
				}
			} // end if (ok == 0)
		}
		else if (m_pSockMgr.at(conindex)->m_bChecked)// else if (g_connInfoList[conindex]->m_bNeedconnect)
		{
			OnSend(conindex);//��������
		} // end if (g_connInfoList[conindex]->m_bNeedconnect)
	}
	else  if (m_pSockMgr.at(conindex)->m_bChecked)
	{
//		OnSend(conindex);//��������
	}
#endif
	
				
}
void CBF_DrebServer::OnPing(int conindex)
{
	if (time(NULL) - m_pSockMgr.at(conindex)->m_nPingTime < BEATHEARTTICK) //С��5�벻�÷�ping
	{
		return ;
	}
	if (!m_pSockMgr.at(conindex)->m_bChecked)
	{
		//û�������ϣ����÷�
		return;
	}
	if (m_pSockMgr.at(conindex)->m_nType == BPCSOCK_DREB)
	{
		m_pSockMgr.at(conindex)->m_nPingTime = time(NULL);
		if (!ReMallocData(conindex,&m_pDataBuf))
		{
			return;
		}
		m_pDataBuf.sMsgBuf->sDBHead.cRaflag = 0;
		m_pDataBuf.sMsgBuf->sDBHead.cZip = 0;
		m_pDataBuf.sMsgBuf->sDBHead.nLen = 0;
		m_pDataBuf.sMsgBuf->sDBHead.cCmd = CMD_PING;
		m_pDataBuf.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
		//����ping���ݰ�
		m_pSockMgr.at(conindex)->SendMsg(m_pDataBuf);
	}
	
}
void CBF_DrebServer::OnClose(int conindex,const char *msg,const char *filename,int fileline)
{
	if (m_pSockMgr.at(conindex)->m_sock == INVALID_SOCKET)
	{
		return;
	}
	//��socket�ر�
	m_pSockMgr.at(conindex)->OnClose(msg,filename,fileline);
}
void CBF_DrebServer::OnRecv(int conindex)
{
	int ret=0;
	int i=0;
	
	ret = m_pSockMgr.at(conindex)->OnRecv();	
	if (ret<=0)
	{
#ifdef _ENGLISH_
		OnClose(conindex,"far-end close",__FILE__,__LINE__);
#else
		OnClose(conindex,"Զ�˹رգ��������ݲ�����",__FILE__,__LINE__);
#endif
		return;
	}
	while (m_pSockMgr.at(conindex)->m_nRcvBufLen>0)
	{
		if (!ReMallocData(conindex,&m_pDataRcv))
		{
			return;
		}
		ret = m_pSockMgr.at(conindex)->GetRecvData(&m_pDataRcv);
		if (ret>0)
		{
			if (m_pSockMgr.at(conindex)->m_nType == BPCSOCK_DREB)
			{
				//����Ӧ����Ϊ���ɿ����ͻ�ȫ��㲥��Ϣ��ֱ�Ӷ���
				if (m_pDataRcv.sMsgBuf->sDBHead.cRaflag == 1 && (m_pDataRcv.sMsgBuf->sDBHead.cCmd == CMD_DPPUSH || m_pDataRcv.sMsgBuf->sDBHead.cCmd == CMD_DPABC))
				{
#ifdef _ENGLISH_
					m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"CMD_DPPUSH or CMD_DPABC answer,discard");
#else
					m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"CMD_DPPUSH��CMD_DPABC��Ӧ�𣬶���");
#endif
					m_pMemPool.PoolFree(m_pDataRcv.sMsgBuf);
					m_pDataRcv.sMsgBuf = NULL;
					continue;
				}
				ProcessDreb(m_pDataRcv);
			}
			else
			{
				m_pMemPool.PoolFree(m_pDataRcv.sMsgBuf);
				m_pDataRcv.sMsgBuf = NULL;
			}
		}
		else
		{
			m_pMemPool.PoolFree(m_pDataRcv.sMsgBuf);
			m_pDataRcv.sMsgBuf = NULL;
			break;
		}
	}
	
	return ;
}
void CBF_DrebServer::OnSend(int conindex)
{
	m_pSockMgr.at(conindex)->OnSend();
}

void CBF_DrebServer::ProcessDreb(S_BPC_RSMSG &rcvdata)
{
	int flag=0;
	if (rcvdata.sMsgBuf->sDBHead.cRaflag == 0) //����
	{
		if (rcvdata.sMsgBuf->sDBHead.cNextFlag == 2 ) //ȡ��������
		{
			if (rcvdata.sMsgBuf->sDBHead.cCmd != CMD_DPCALL && rcvdata.sMsgBuf->sDBHead.cCmd != CMD_DPACALL)
			{
				//���������֣��Ͳ���ȡ������
				m_pMemPool.PoolFree(rcvdata.sMsgBuf);
				rcvdata.sMsgBuf = NULL;
				return;
			}
			rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN+rcvdata.sMsgBuf->sDBHead.nLen;
			rcvdata.sMsgBuf->sBpcHead.cMsgType = MSG_GETBPCNEXT;
			rcvdata.nRtime = time(NULL);
            if (m_spi != NULL) //���趨�ص�
            {
                m_spi->ProcessDrebData(rcvdata);
            }
            else //�������
            {
                m_qRcvQueue.PushData(rcvdata);
            }
			return;
		}
		if (rcvdata.sMsgBuf->sDBHead.cNextFlag == 3) //bpuȡ������
		{
#ifdef _ENGLISH_
			
#else
			m_pLog.LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"�յ�BPUȡ��������������");
#endif

			if (rcvdata.sMsgBuf->sDBHead.cCmd != CMD_DPCALL && rcvdata.sMsgBuf->sDBHead.cCmd != CMD_DPACALL)
			{
				//���������֣��Ͳ���ȡ������
				m_pMemPool.PoolFree(rcvdata.sMsgBuf);
				rcvdata.sMsgBuf = NULL;
				return;
			}
			rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN+rcvdata.sMsgBuf->sDBHead.nLen;
			rcvdata.sMsgBuf->sBpcHead.cMsgType = MSG_GETNEXT;
			rcvdata.nRtime = time(NULL);
			if (m_spi != NULL) //���趨�ص�
			{
				m_spi->ProcessDrebData(rcvdata);
			}
			else //�������
			{
				m_qRcvQueue.PushData(rcvdata);
			}
			return;
		}
		switch (rcvdata.sMsgBuf->sDBHead.cCmd)
		{
			case CMD_PING:
				rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
				rcvdata.sMsgBuf->sDBHead.nLen = 0;
				rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
				m_pSockMgr.at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
				break;
            case CMD_DPBC:
            case CMD_DPABC:	//�㲥��Ϣ����Ҫ�����ظ���
				if (NULL != m_bcSerial.Select(rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId, rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial))
				{
					//�ظ�
                    m_pLog.LogMp(LOG_DEBUG+1, __FILE__, __LINE__, "�յ��ظ��Ĺ㲥 %d %d %d", rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId, rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId, rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial);
                    m_pMemPool.PoolFree(rcvdata.sMsgBuf);
                    rcvdata.sMsgBuf = NULL;
					break;
				}
				S_BC_SERIAL bcserial;
				bcserial.node_id = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId;
				bcserial.node_pid = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId;
				bcserial.serial = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial;
				bcserial.timestamp = time(NULL);
				m_bcSerial.Insert(bcserial);
                rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN + rcvdata.sMsgBuf->sDBHead.nLen;
                rcvdata.sMsgBuf->sBpcHead.cMsgType = MSG_REQ;
                rcvdata.nRtime = time(NULL);
                if (m_spi != NULL) //���趨�ص�
                {
                    m_spi->ProcessDrebData(rcvdata);
                }
                else //�������
                {
                    m_qRcvQueue.PushData(rcvdata);
                }
				break;
			case CMD_DPCALL:
			case CMD_DPACALL:
			case CMD_DPPUSH:
			case CMD_DPPOST:
				rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN+rcvdata.sMsgBuf->sDBHead.nLen;
				rcvdata.sMsgBuf->sBpcHead.cMsgType = MSG_REQ;
				rcvdata.nRtime = time(NULL);
                if (m_spi != NULL) //���趨�ص�
                {
                    m_spi->ProcessDrebData(rcvdata);
                }
                else //�������
                {
                    m_qRcvQueue.PushData(rcvdata);
                }
				break;	
			case CMD_MONITOR_BPC:
				rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN+rcvdata.sMsgBuf->sDBHead.nLen;
				rcvdata.sMsgBuf->sBpcHead.cMsgType = MSG_MONITOR_SAP;
				rcvdata.nRtime = time(NULL);
                if (m_spi != NULL) //���趨�ص�
                {
                    m_spi->ProcessDrebData(rcvdata);
                }
                else //�������
                {
                    m_qRcvQueue.PushData(rcvdata);
                }
				break;	
			default:
				rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN+rcvdata.sMsgBuf->sDBHead.nLen;
				rcvdata.sMsgBuf->sBpcHead.cMsgType = MSG_REQ;
				rcvdata.nRtime = time(NULL);
                if (m_spi != NULL) //���趨�ص�
                {
                    m_spi->ProcessDrebData(rcvdata);
                }
                else //�������
                {
                    m_qRcvQueue.PushData(rcvdata);
                }
		}
		return;
		
	}
	else  //��DREB��Ӧ��
	{
		switch (rcvdata.sMsgBuf->sDBHead.cCmd)
		{
			case CMD_PING:
				m_pMemPool.PoolFree(rcvdata.sMsgBuf);
				rcvdata.sMsgBuf = NULL;
				break;
			case CMD_CONNECT:
				if (rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode != SUCCESS)
				{
#ifdef _ENGLISH_
					m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"DREB connect fail, retcode[%d]",rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode);
					m_pSockMgr.at(rcvdata.sMsgBuf->sBpcHead.nIndex)->OnClose("DREB connect fail",__FILE__,__LINE__);
#else
					m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"DREB��������ʧ�� ������[%d]",rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode);
					m_pSockMgr.at(rcvdata.sMsgBuf->sBpcHead.nIndex)->OnClose("DREB���ط�������ʧ��",__FILE__,__LINE__);
#endif
					m_pSockMgr.at(rcvdata.sMsgBuf->sBpcHead.nIndex)->m_nConntime = time(NULL);
					m_pMemPool.PoolFree(rcvdata.sMsgBuf);
					rcvdata.sMsgBuf = NULL;
					return;
				}
				m_pSockMgr.at(rcvdata.sMsgBuf->sBpcHead.nIndex)->m_bChecked = true;
				m_pSockMgr.at(rcvdata.sMsgBuf->sBpcHead.nIndex)->m_nDrebId = rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nNodeId;
				m_pSockMgr.at(rcvdata.sMsgBuf->sBpcHead.nIndex)->m_nDrebPrivateId = rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_cNodePrivateId;
				
				memcpy(&(m_pSockMgr.at(rcvdata.sMsgBuf->sBpcHead.nIndex)->m_head),&(rcvdata.sMsgBuf->sDBHead),DREBHEADLEN);
				rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN+rcvdata.sMsgBuf->sDBHead.nLen;
				rcvdata.sMsgBuf->sBpcHead.cMsgType = MSG_CONNECTDREB;
				rcvdata.nRtime = time(NULL);
                if (m_spi != NULL) //���趨�ص�
                {
                    m_spi->ProcessDrebData(rcvdata);
                }
                else //�������
                {
                    m_qRcvQueue.PushData(rcvdata);
                }
				break;
			case CMD_DEL_SVR:
				m_pMemPool.PoolFree(rcvdata.sMsgBuf);
				rcvdata.sMsgBuf = NULL;
				break;
			case CMD_REGSERVICE:
				m_pMemPool.PoolFree(rcvdata.sMsgBuf);
				rcvdata.sMsgBuf = NULL;
				break;	
			case CMD_DPCALL:
			case CMD_DPACALL:
			case CMD_DPBC:
			case CMD_DPABC:
			case CMD_DPPUSH:
			case CMD_DPPOST:
				//bpc�����ֱ��Ӧ���bpu
				rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN+rcvdata.sMsgBuf->sDBHead.nLen;
				rcvdata.sMsgBuf->sBpcHead.cMsgType = MSG_DREBANS;
				rcvdata.nRtime = time(NULL);
                if (m_spi != NULL) //���趨�ص�
                {
                    m_spi->ProcessDrebData(rcvdata);
                }
                else //�������
                {
                    m_qRcvQueue.PushData(rcvdata);
                }
				break;	
			default:
				m_pMemPool.PoolFree(rcvdata.sMsgBuf);
				rcvdata.sMsgBuf = NULL;
				break;
		}
		return;
	}
}

void CBF_DrebServer::OnMonitor()
{
	if (m_pRes->g_nUseMonitor != 1) 
	{
		return;
	}
	//�����Ϣ
	int i,k;
	CBF_Xml        pXml;
	pXml.SetNodeValueByPath("Monitor/�����ڵ�",false,(int)m_pRes->g_nSvrMainId);
	pXml.SetNodeValueByPath("Monitor/˽�нڵ�",false,(int)m_pRes->g_nSvrPrivateId);
	pXml.SetNodeValueByPath("Monitor/�ڵ�����",false,1);//bpc
	pXml.SetNodeValueByPath("Monitor/��������",false,0);//����
	pXml.SetNodeValueByPath("Monitor/����ʱ��",false,m_pRes->g_sStartDateTime,false);
	int total=0;
	int used=0;
	m_pMemPool.PoolInfo(total,used);
	pXml.SetNodeAttribute("Monitor/�ڴ滺���",false,"�ܷ���",false,total );
	pXml.SetNodeAttribute("Monitor/�ڴ滺���",false,"δʹ��",false,used );
	pXml.SetNodeAttribute("Monitor/�ڴ滺���",false,"ÿ���С",false,(int)BPCBUFSIZE );
	

	if (m_pRes->g_nMonitorHost == 1)
	{
		S_MONITOR_HOST hostinfo;
		std::vector<S_MONITOR_DISK> diskinfo;
		//ȡ��������Ϣ
		GetHostInfo(hostinfo,diskinfo);
		pXml.SetNodeValueByPath("Monitor/������Դ/CPU",false,(int)hostinfo.nCpuRate);
		pXml.SetNodeAttribute("Monitor/������Դ/�ڴ�",false,"total",false,(int)hostinfo.nTotalMemory);
		pXml.SetNodeAttribute("Monitor/������Դ/�ڴ�",false,"used",false,(int)hostinfo.nUsedMemory);
		for (i=0 ; i< diskinfo.size() ; i++)
		{
			CXmlNode *disknode = pXml.AddNodeByPath("Monitor/������Դ/����",false,"����",false,"");
			if (disknode != NULL)
			{
				disknode->SetAttribute("drive",false,diskinfo[i].sDiskName,true);
				disknode->SetAttribute("total",false,(int)diskinfo[i].nTotalSpace);
				disknode->SetAttribute("used",false,(int)diskinfo[i].nUsedSpace);
			}
		}
	}
	//������Ϣ
	for (i=0 ; i < m_pRes->g_vDrebLinkInfo.size() ; i++)
	{
		CXmlNode *connnode = pXml.AddNodeByPath("Monitor/������Ϣ",false,"����",false,"");
		if (connnode == NULL)
		{
			m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"AddNodeByPath ����");
			continue;
		}
		connnode->SetAttribute("ip",false,m_pRes->g_vDrebLinkInfo[i].sIp);
		connnode->SetAttribute("port",false,(int)m_pRes->g_vDrebLinkInfo[i].nPort);
		connnode->SetAttribute("index",false,i+1);
		connnode->SetAttribute("�����ڵ�",false,m_pSockMgr.at(i)->m_nDrebId);
		connnode->SetAttribute("˽�нڵ�",false,m_pSockMgr.at(i)->m_nDrebPrivateId);
		if (m_pSockMgr.at(i)->m_sock == INVALID_SOCKET)
		{
			connnode->SetAttribute("status",false,"δ����");
		}
		else if (m_pSockMgr.at(i)->m_bChecked)
		{
			connnode->SetAttribute("status",false,"����");
		}
		else
		{
			connnode->SetAttribute("status",false,"��������");
		}
	}


	S_BPC_RSMSG  mdata;//�������ݽṹ
	mdata.sMsgBuf = (PBPCCOMMSTRU )m_pMemPool.PoolMalloc();
	if (mdata.sMsgBuf == NULL)
	{
		m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"������Ϣ�ռ����");
		return ;
	}
    bzero(&(mdata.sMsgBuf->sDBHead), sizeof(DREB_HEAD));
    bzero(&(mdata.sMsgBuf->sBpcHead), sizeof(S_BPC_HEAD));
	int len = DREBDATASIZE;
	if (!pXml.ToBuffer(mdata.sMsgBuf->sBuffer,len))
	{
		m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"ToBuffer����");
		m_pMemPool.PoolFree(mdata.sMsgBuf);
		return;
	}
	mdata.sMsgBuf->sDBHead.nLen = len;
	
	m_pLog.LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"���֪ͨ",mdata.sMsgBuf->sBuffer,mdata.sMsgBuf->sDBHead.nLen);
	
	
	mdata.sMsgBuf->sDBHead.cRaflag = 0;
	mdata.sMsgBuf->sDBHead.cDrebAffirm = 0;
	mdata.sMsgBuf->sDBHead.cZip = 0;
	mdata.sMsgBuf->sDBHead.cNextFlag = 0;
	
	mdata.sMsgBuf->sDBHead.cCmd = CMD_DPPUSH;
	mdata.sMsgBuf->sDBHead.d_Dinfo.d_nNodeId = m_pRes->g_nMonitorDrebId;
	mdata.sMsgBuf->sDBHead.d_Dinfo.d_cNodePrivateId = m_pRes->g_nMonitorDrebPrivateId;
	mdata.sMsgBuf->sDBHead.d_Dinfo.d_nSvrMainId = m_pRes->g_nMonitorSvrId;
	mdata.sMsgBuf->sDBHead.d_Dinfo.d_cSvrPrivateId = m_pRes->g_nMonitorSvrPrivateId;
	mdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo = m_pRes->g_nMonitorTxCode;
	mdata.sMsgBuf->sBpcHead.cMsgType = MSG_BPCMONITOR;
	
    if (m_spi != NULL) //���趨�ص�
    {
        m_spi->ProcessDrebData(mdata);
    }
    else //�������
    {
        m_qRcvQueue.PushData(mdata);
    }
	return;
}
int CBF_DrebServer::OnTimer(unsigned int event, void *p)
{
	CBF_DrebServer *pp = (CBF_DrebServer *)p;
	if (event == 0)
	{
		//����ÿ����շ���
		pp->ComputeStat();
	}
	else if (event == 1 )
	{
		pp->m_bIsPingTimer = true;
		pp->GetHostInfo();
	}
	return 0;
}

void CBF_DrebServer::ComputeStat()
{
	//����
	m_pRes->ComputeStat();
}

void CBF_DrebServer::GetHostInfo()
{
	CBF_PMutex pp(&m_pHostMutex);
	m_sHostInfo.nCpuRate = m_pHostInfo.GetCpu();
	S_MemoryInf smInfo;
	m_pHostInfo.GetEms(smInfo);
	m_sHostInfo.nTotalMemory = smInfo.EmsTotal;
	m_sHostInfo.nUsedMemory = smInfo.EmsUse;
	m_vdinfo.clear();
	m_pHostInfo.GetDisk(m_vdinfo);
	m_sHostInfo.nDiskNum = m_vdinfo.size();
	//ɾ�����ڵĹ㲥��Ϣ
	m_bcSerial.Delete();
}

bool CBF_DrebServer::Start()
{
	if (m_bIsRunning)
	{
		sprintf(m_errMsg,"�߳����б�־Ϊ�������У���Stop");
		return false;
	}
	if (!m_bIsInit)
	{
		sprintf(m_errMsg,"δ��ʼ��");
		return false;
	}
	//������ʱ��
	if (!m_pTimer.Start())
	{
		sprintf(m_errMsg,"��ʱ������ʧ��");
		return false;
	}
	if (m_pLog.IsStopedThread())
	{
		if (!m_pLog.StartLog())
		{
			sprintf(m_errMsg,"������־�߳�ʧ��");
			m_pTimer.Stop();
			return false;
		}
	}
	
	if (!m_pDrebDataLog.StartLog())
	{
		sprintf(m_errMsg,"�������߱�����־�߳�ʧ��");
		m_pTimer.Stop();
		m_pLog.StopLog();
		return false;
	}
	if (!m_pBpcCallDataLog.StartLog())
	{
		sprintf(m_errMsg,"�������������־�߳�ʧ��");
		m_pTimer.Stop();
		m_pLog.StopLog();
		m_pDrebDataLog.StopLog();
		return false;
	}
	m_bIsRunning = true;
	CreateThread();
	return true;
}

void CBF_DrebServer::MonitorThread()
{
	if (m_pTimer.IsStoped())
	{
#ifdef _ENGLISH_
		m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"restart timer thread");
#else
		m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"������ʱ���߳�");
#endif
		m_pTimer.Start();
	}
	if (m_pLog.IsStopedThread())
	{
		m_pLog.StartLog();
#ifdef _ENGLISH_
		m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"restart log thread");
#else
		m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"������־�߳�");
#endif
	}
	if (m_pBpcCallDataLog.IsStoped())
	{
		m_pBpcCallDataLog.StartLog();
#ifdef _ENGLISH_
		m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"restart bpc datalog thread");
#else
		m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"����BPC���ñ�����־�߳�");
#endif
	}
	if (m_pDrebDataLog.IsStoped())
	{
		m_pDrebDataLog.StartLog();
#ifdef _ENGLISH_
		m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"restart dreb datalog thread");
#else
		m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"����DREB������־�߳�");
#endif
	}
	if (IsStoped())
	{
#ifdef _ENGLISH_
		m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"restart api thread");
#else
		m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"����DREBAPI�߳�");
#endif
		CreateThread();
	}
}
void CBF_DrebServer::Stop()
{
	m_bIsRunning = false;
	m_pTimer.Stop();
//	printf("��ʼ�˳���־�߳�\n");
	m_pLog.StopLog();
//	printf("�����˳���־�߳�\n");

//	printf("��ʼ�˳�BpcCallData��־�߳�\n");
	m_pBpcCallDataLog.StopLog();
//	printf("��ʼ�˳�DrebData��־�߳�\n");
	m_pDrebDataLog.StopLog();
//	printf("�����˳�DATA��־�߳�\n");
	Join();
	if (m_pRes != NULL)
	{
		int num =m_pRes->g_vDrebLinkInfo.size();
		for (int i=0; i< num;i++)
		{
#ifdef _ENGLISH_
			m_pSockMgr.at(i)->OnClose("exit and close connect",__FILE__,__LINE__);
#else
			m_pSockMgr.at(i)->OnClose("�˳��ر�����",__FILE__,__LINE__);
#endif
		}
	}
	
	
}

void CBF_DrebServer::GetHostInfo(S_MONITOR_HOST &host, std::vector<S_MONITOR_DISK>&disk)
{
	CBF_PMutex ppmutex(&m_pHostMutex);
	host.nCpuRate = m_sHostInfo.nCpuRate;
	host.nDiskNum = m_sHostInfo.nDiskNum;
	host.nTotalMemory = m_sHostInfo.nTotalMemory;
	host.nUsedMemory = m_sHostInfo.nUsedMemory;
	
	S_MONITOR_DISK pp;
	for (int i=0;i<m_sHostInfo.nDiskNum;i++)
	{
		pp.nTotalSpace = m_vdinfo[i].TotalSpace;
		pp.nUsedSpace = m_vdinfo[i].UsedSpace;
		CBF_Tools::StringCopy(pp.sDiskName,199,m_vdinfo[i].cIndex);
//		printf("name:%s Total:%d used:%d\n",pp.sDiskName,pp.nTotalSpace,pp.nUsedSpace);
		disk.push_back(pp);
	}
}

int CBF_DrebServer::SendMsg(S_BPC_RSMSG &sdata)
{
	if (!m_pSockMgr.AffirmIndex(sdata.sMsgBuf->sBpcHead.nIndex))
	{
		sdata.sMsgBuf->sBpcHead.nIndex = m_nCurIndex;
		m_nCurIndex++;
		if (m_nCurIndex >= m_nEnd)
		{
			m_nCurIndex = m_nBegin;
		}
	}
	else if (m_pSockMgr.at(sdata.sMsgBuf->sBpcHead.nIndex)->m_bChecked) //ԭ�������������Է���
	{
		return m_pSockMgr.at(sdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(sdata);
	}
	//ԭ���Ӳ�������ѡһ�������ķ��ͣ������Ƿŵ�ԭ���ӵĶ�����
	int nRet=0;
	for (int i=m_nBegin ; i< m_nEnd; i++)
	{
		if (m_pSockMgr.at(m_nCurIndex)->m_bChecked)
		{
			nRet =  m_pSockMgr.at(m_nCurIndex)->SendMsg(sdata);
			m_nCurIndex++;
			if (m_nCurIndex >= m_nEnd)
			{
				m_nCurIndex = m_nBegin;
			}
			return nRet;
		}
	}
	//û���ҵ����ʵ����ӣ�������ԭ�������ӷ���
	if (m_pSockMgr.at(sdata.sMsgBuf->sBpcHead.nIndex)->m_sock == INVALID_SOCKET || !m_pSockMgr.at(sdata.sMsgBuf->sBpcHead.nIndex)->m_bChecked )
	{
#ifdef _ENGLISH_
		m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"no DREB connect ,discard %d",sdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo);
#else
		m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"���������ӣ����� %d",sdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo);
#endif
		m_pMemPool.PoolFree(sdata.sMsgBuf);
		sdata.sMsgBuf = NULL;
		return -1;
	}
	else
	{
		return m_pSockMgr.at(sdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(sdata);
	}
	
	

}

bool CBF_DrebServer::GetMsgData(S_BPC_RSMSG &rdata,unsigned int waitetms,bool get_front)
{
	return m_qRcvQueue.GetData(rdata,waitetms,get_front);
}
void CBF_DrebServer::RegisterDreb(int index, std::vector<int> *funclist)
{
	S_BPC_RSMSG rcvdata;
	S_SERVICEREG_MSG *data=NULL;
	bool bRet;
	int  offset;
	int funcNo;
	unsigned int *func=NULL;
	if (!m_pSockMgr.AffirmIndex(index))
	{
#ifdef _ENGLISH_
		m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"connect %d illegal",index);
#else
		m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���������Ƿ� %d",index);
#endif
		return;
	}

	if (m_pSockMgr.at(index)->m_sock == INVALID_SOCKET || !m_pSockMgr.at(index)->m_bChecked )
	{
#ifdef _ENGLISH_
		m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"connect [%d] not affirmed ",index);
#else
		m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"��������[%d]������δ�������߻���������ȷ�� ",index);
#endif
		return;
	}
	rcvdata.sMsgBuf = (PBPCCOMMSTRU )m_pMemPool.PoolMalloc();
	if (rcvdata.sMsgBuf == NULL)
	{
#ifdef _ENGLISH_
		m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"malloc msgdata fail");
#else
		m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"������Ϣ�ռ����");
#endif
		return ;
	}
	memset(rcvdata.sMsgBuf,0,sizeof(BPCCOMMSTRU));

	CInt listint;
	for (int i=0; i< funclist->size(); i++)
	{
		listint.Add(funclist->at(i));
	}
	if (listint.Size()<1)
	{
#ifdef _ENGLISH_
		m_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,"no txcode register");
#else
		m_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,"�޽���ע��");
#endif
		data = (S_SERVICEREG_MSG *)rcvdata.sMsgBuf->sBuffer;
		data->nSvrMainId = m_pRes->g_nSvrMainId;
		m_pDrebEndian.Endian2Comm((unsigned char *)&(data->nSvrMainId),sizeof(data->nSvrMainId));
		data->cSvrPrivateId = m_pRes->g_nSvrPrivateId;
		data->nFuncNum = 0;
		data->nFuncNo = 0;
		m_pDrebEndian.Endian2Comm((unsigned char *)&(data->nFuncNo),sizeof(data->nFuncNo));
		rcvdata.sMsgBuf->sDBHead.nLen = sizeof(S_SERVICEREG_MSG);
		rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN + rcvdata.sMsgBuf->sDBHead.nLen;
#ifdef _ENGLISH_
		m_pLog.LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"register data len=%d",rcvdata.sMsgBuf->sDBHead.nLen);
#else
		m_pLog.LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"����ע�ᷢ�͵����� ����=%d",rcvdata.sMsgBuf->sDBHead.nLen);
#endif
		rcvdata.sMsgBuf->sDBHead.cCmd = CMD_REGSERVICE;
		m_pSockMgr.at(index)->SendMsg(rcvdata);
		return ;
	}
	bRet = listint.First(funcNo);
	while (bRet)
	{
		bRet = listint.Next(funcNo);
	}
#ifdef _ENGLISH_
	m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"DREB index[%d] [%d %d][%s-%d] register",\
		index,m_pSockMgr.at(index)->m_nDrebId,m_pSockMgr.at(index)->m_nDrebPrivateId,\
		m_pSockMgr.at(index)->m_sDrebIp.c_str(),m_pSockMgr.at(index)->m_nDrebPort);
#else	
	m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"��ʼ��DREB index[%d] [%d %d][%s-%d] ע�������",\
		index,m_pSockMgr.at(index)->m_nDrebId,m_pSockMgr.at(index)->m_nDrebPrivateId,\
		m_pSockMgr.at(index)->m_sDrebIp.c_str(),m_pSockMgr.at(index)->m_nDrebPort);
#endif
	rcvdata.sMsgBuf->sDBHead.cCmd = CMD_REGSERVICE;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 0;
	rcvdata.sMsgBuf->sDBHead.cNextFlag = 0;
	rcvdata.sMsgBuf->sDBHead.cZip = 0;
	
	if (listint.First(funcNo))
	{
		data = (S_SERVICEREG_MSG *)rcvdata.sMsgBuf->sBuffer;
		data->nSvrMainId = m_pRes->g_nSvrMainId;
		m_pDrebEndian.Endian2Comm((unsigned char *)&(data->nSvrMainId),sizeof(data->nSvrMainId));
		data->cSvrPrivateId = m_pRes->g_nSvrPrivateId;
		data->nFuncNum = 1;
		data->nFuncNo = funcNo;
		m_pDrebEndian.Endian2Comm((unsigned char *)&(data->nFuncNo),sizeof(data->nFuncNo));
		offset = sizeof(S_SERVICEREG_MSG);
	}
	bRet = listint.Next(funcNo);
	while (bRet)
	{
		func = (unsigned int *)(rcvdata.sMsgBuf->sBuffer+offset);
		*func = funcNo;
		m_pDrebEndian.Endian2Comm((unsigned char *)func,sizeof(unsigned int));
		data->nFuncNum++;
		offset += sizeof(unsigned int);
		if (offset> BPUDATASIZE-30)
		{
			m_pMemPool.PoolFree(rcvdata.sMsgBuf);
#ifdef _ENGLISH_
			m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"too many txcode");
#else
			m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"ע�Ṧ��̫��,һ�����ݰ��޷���DREB��ע�ᣬ����ٹ�����");
#endif
			rcvdata.sMsgBuf = NULL;
			return;
		}
		bRet = listint.Next(funcNo);
	}
#ifdef _ENGLISH_
	m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"total %d function",data->nFuncNum);
#else	
	m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ܹ���%d������ע��",data->nFuncNum);
#endif
	m_pDrebEndian.Endian2Comm((unsigned char *)&(data->nFuncNum),sizeof(data->nFuncNum));
	rcvdata.sMsgBuf->sDBHead.nLen = offset;
	rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN + rcvdata.sMsgBuf->sDBHead.nLen;
#ifdef _ENGLISH_
	m_pLog.LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"register data len=%d",rcvdata.sMsgBuf->sDBHead.nLen);
#else
	m_pLog.LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"����ע�ᷢ�͵����� ����=%d",rcvdata.sMsgBuf->sDBHead.nLen);
#endif

	m_pSockMgr.at(index)->SendMsg(rcvdata);
}

void CBF_DrebServer::UnRegisterDreb()
{
	S_BPC_RSMSG rcvdata;
	S_SERVICEREG_MSG *data=NULL;

	for (int i= 0 ; i< m_pRes->g_vDrebLinkInfo.size(); i++)
	{
		if (m_pSockMgr.at(i)->m_sock == INVALID_SOCKET || !m_pSockMgr.at(i)->m_bChecked )
		{
			continue;
		}
		rcvdata.sMsgBuf = (PBPCCOMMSTRU )m_pMemPool.PoolMalloc();
		if (rcvdata.sMsgBuf == NULL)
		{
			m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"������Ϣ�ռ����");
			return ;
		}
		memset(rcvdata.sMsgBuf,0,sizeof(BPCCOMMSTRU));
#ifdef _ENGLISH_
		m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"DREB index[%d] [%d %d][%s-%d] cancel register",\
			i,m_pSockMgr.at(i)->m_nDrebId,m_pSockMgr.at(i)->m_nDrebPrivateId,\
			m_pSockMgr.at(i)->m_sDrebIp.c_str(),m_pSockMgr.at(i)->m_nDrebPort);
#else
		m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"��ʼ��DREB index[%d] [%d %d][%s-%d] ȡ��ע�ύ��",\
			i,m_pSockMgr.at(i)->m_nDrebId,m_pSockMgr.at(i)->m_nDrebPrivateId,\
			m_pSockMgr.at(i)->m_sDrebIp.c_str(),m_pSockMgr.at(i)->m_nDrebPort);
#endif
		rcvdata.sMsgBuf->sDBHead.cCmd = CMD_UNREGSERVICE;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 0;
		rcvdata.sMsgBuf->sDBHead.cNextFlag = 0;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		data = (S_SERVICEREG_MSG *)rcvdata.sMsgBuf->sBuffer;
		data->nSvrMainId = m_pRes->g_nSvrMainId;
		m_pDrebEndian.Endian2Comm((unsigned char *)&(data->nSvrMainId),sizeof(data->nSvrMainId));
		data->cSvrPrivateId = m_pRes->g_nSvrPrivateId;
		data->nFuncNum = 1;
		data->nFuncNo = 0;
		m_pDrebEndian.Endian2Comm((unsigned char *)&(data->nFuncNo),sizeof(data->nFuncNo));
		m_pDrebEndian.Endian2Comm((unsigned char *)&(data->nFuncNum),sizeof(data->nFuncNum));
		rcvdata.sMsgBuf->sDBHead.nLen = sizeof(S_SERVICEREG_MSG);
		rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN + rcvdata.sMsgBuf->sDBHead.nLen;
		m_pSockMgr.at(i)->SendMsg(rcvdata);

	}
	return;
}
void CBF_DrebServer::GetBufferPoolInfo(int &total, int &unused, int &bufsize)
{
	m_pMemPool.PoolInfo(total,unused);
	bufsize = BPCBUFSIZE;
}

bool CBF_DrebServer::GetDrebInfo(int index, char *sip, int &port, int &drebid, int &privateid, char *status)
{
	if (!m_pSockMgr.AffirmIndex(index))
	{
		return false;
	}
	strcpy(sip,m_pSockMgr.at(index)->m_sDrebIp.c_str());
	port = m_pSockMgr.at(index)->m_nDrebPort;
	drebid = m_pSockMgr.at(index)->m_nDrebId;
	privateid = m_pSockMgr.at(index)->m_nDrebPrivateId;

	if (m_pSockMgr.at(index)->m_sock == INVALID_SOCKET)
	{
		strcpy(status,"δ����");
	}
	else if (m_pSockMgr.at(index)->m_bChecked)
	{
		strcpy(status,"����");
	}
	else
	{
		strcpy(status,"��������");
	}
	return true;
}

CIErrlog * CBF_DrebServer::GetLogPoint()
{
	return &m_pLog;
}
bool CBF_DrebServer::UnzipBuf(BPCCOMMSTRU &data)
{
	//0���� 1des������Կ����16 2ѹ�� 3DES���� 4DES���ܲ�ѹ��
	if (data.sDBHead.nLen == 0)
	{
		data.sDBHead.cZip = 0;
		return true;
	}
	if (data.sDBHead.cZip >0 && data.sDBHead.cZip <=4)
	{
		char zipDataBuf[BPUDATASIZE];
		bzero(zipDataBuf,sizeof(zipDataBuf));
		unsigned int dlen = BPUDATASIZE;
		int ddlen;
		switch (data.sDBHead.cZip)
		{
		case 1:
			ddlen = data.sDBHead.nLen;
			if (m_pDes.UncryptString(data.sBuffer,ddlen,DESENCRYPTKEY)<=0)
			{
#ifdef _ENGLISH_
				m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Uncrypt fail");
#else
				m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
#endif
				return false;
			}
			data.sDBHead.nLen = ddlen;
			data.sDBHead.cZip =0;
			break;
		case 2:	
			memcpy(zipDataBuf,data.sBuffer,data.sDBHead.nLen);
			if (!CBF_Tools::Uncompress((unsigned char *)(data.sBuffer),dlen,(unsigned char *)zipDataBuf,data.sDBHead.nLen))
			{
#ifdef _ENGLISH_
				m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Uncrypt fail");
#else
				m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"��ѹ������");
#endif
				return false;
			}
			data.sDBHead.nLen = dlen;
			data.sDBHead.cZip =0;
			break;
		case 3:
			ddlen = data.sDBHead.nLen;
			if (m_pDes.UncryptString(data.sBuffer,ddlen,DESENCRYPTKEY3)<=0)
			{
#ifdef _ENGLISH_
				m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Uncrypt fail");
#else
				m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
#endif
				return false;
			}
			data.sDBHead.nLen = ddlen;
			data.sDBHead.cZip =0;
			break;
		case 4:
			ddlen = data.sDBHead.nLen;
#ifdef _ENGLISH_
			m_pLog.LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"uncrypt data",data.sBuffer,data.sDBHead.nLen);
#else
			m_pLog.LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"����ǰ����",data.sBuffer,data.sDBHead.nLen);
#endif
			if (m_pDes.UncryptString(data.sBuffer,ddlen,DESENCRYPTKEY)<=0)
			{
#ifdef _ENGLISH_
				m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"uncrypt fail");
#else
				m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
#endif
				return false;
			}
			data.sDBHead.nLen = ddlen;
#ifdef _ENGLISH_
			m_pLog.LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"uncrypted data",data.sBuffer,data.sDBHead.nLen);
#else
			m_pLog.LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"���ܺ�����",data.sBuffer,data.sDBHead.nLen);
#endif
			memcpy(zipDataBuf,data.sBuffer,data.sDBHead.nLen);
			if (!CBF_Tools::Uncompress((unsigned char *)(data.sBuffer),dlen,(unsigned char *)zipDataBuf,data.sDBHead.nLen))
			{
#ifdef _ENGLISH_
				m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Uncompress fail");
#else
				m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"��ѹ������");
#endif
				return false;
			}
			data.sDBHead.nLen = dlen;
#ifdef _ENGLISH_
			m_pLog.LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"Uncompressed data",data.sBuffer,data.sDBHead.nLen);
#else
			m_pLog.LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"��ѹ������",data.sBuffer,data.sDBHead.nLen);
#endif
			data.sDBHead.cZip =0;
			break;
		default:
			return false;
		}
	}
	
	return true;
}

void CBF_DrebServer::SetDataLogLevel(int loglevel)
{
	m_pDrebDataLog.SetLogLevel(loglevel);
	m_pBpcCallDataLog.SetLogLevel(loglevel);
}

std::string CBF_DrebServer::GetErrMsg()
{
	return m_errMsg;
}

int CBF_DrebServer::GetDataLogLevel()
{
	return m_pDrebDataLog.GetLogLevel();
}
CBF_BufferPool *CBF_DrebServer::GetBufferPool()
{
	return &m_pMemPool;
}

void CBF_DrebServer::Subscribe(int index, std::vector<int>* funclist)
{
    S_BPC_RSMSG rcvdata;
    S_SERVICEREG_MSG* data = NULL;
    bool bRet;
    int  offset;
    int funcNo;
    unsigned int* func = NULL;
    if (!m_pSockMgr.AffirmIndex(index))
    {
#ifdef _ENGLISH_
        m_pLog.LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "connect %d illegal", index);
#else
        m_pLog.LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "���������Ƿ� %d", index);
#endif
        return;
    }

    if (m_pSockMgr.at(index)->m_sock == INVALID_SOCKET || !m_pSockMgr.at(index)->m_bChecked)
    {
#ifdef _ENGLISH_
        m_pLog.LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "connect [%d] not affirmed ", index);
#else
        m_pLog.LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "��������[%d]������δ�������߻���������ȷ�� ", index);
#endif
        return;
    }
    rcvdata.sMsgBuf = (PBPCCOMMSTRU)m_pMemPool.PoolMalloc();
    if (rcvdata.sMsgBuf == NULL)
    {
#ifdef _ENGLISH_
        m_pLog.LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "malloc msgdata fail");
#else
        m_pLog.LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "������Ϣ�ռ����");
#endif
        return;
    }
    memset(rcvdata.sMsgBuf, 0, sizeof(BPCCOMMSTRU));

    CInt listint;
    for (int i = 0; i < funclist->size(); i++)
    {
        listint.Add(funclist->at(i));
    }
    if (listint.Size() < 1)
    {
#ifdef _ENGLISH_
        m_pLog.LogMp(LOG_PROMPT, __FILE__, __LINE__, "no broadcast subscribe");
#else
        m_pLog.LogMp(LOG_PROMPT, __FILE__, __LINE__, "�޹㲥����");
#endif
        data = (S_SERVICEREG_MSG*)rcvdata.sMsgBuf->sBuffer;
        data->nSvrMainId = m_pRes->g_nSvrMainId;
        m_pDrebEndian.Endian2Comm((unsigned char*)&(data->nSvrMainId), sizeof(data->nSvrMainId));
        data->cSvrPrivateId = m_pRes->g_nSvrPrivateId;
        data->nFuncNum = 0;
        data->nFuncNo = 0;
        m_pDrebEndian.Endian2Comm((unsigned char*)&(data->nFuncNo), sizeof(data->nFuncNo));
        rcvdata.sMsgBuf->sDBHead.nLen = sizeof(S_SERVICEREG_MSG);
        rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN + rcvdata.sMsgBuf->sDBHead.nLen;
#ifdef _ENGLISH_
        m_pLog.LogMp(LOG_DEBUG + 2, __FILE__, __LINE__, "subscribe data len=%d", rcvdata.sMsgBuf->sDBHead.nLen);
#else
        m_pLog.LogMp(LOG_DEBUG + 2, __FILE__, __LINE__, "�㲥���ķ��͵����� ����=%d", rcvdata.sMsgBuf->sDBHead.nLen);
#endif
		rcvdata.sMsgBuf->sDBHead.cCmd = CMD_SUBSCRIBE;
        m_pSockMgr.at(index)->SendMsg(rcvdata);
        return;
    }
    bRet = listint.First(funcNo);
    while (bRet)
    {
        bRet = listint.Next(funcNo);
    }
#ifdef _ENGLISH_
    m_pLog.LogMp(LOG_DEBUG, __FILE__, __LINE__, "DREB index[%d] [%d %d][%s-%d] register", \
        index, m_pSockMgr.at(index)->m_nDrebId, m_pSockMgr.at(index)->m_nDrebPrivateId, \
        m_pSockMgr.at(index)->m_sDrebIp.c_str(), m_pSockMgr.at(index)->m_nDrebPort);
#else	
    m_pLog.LogMp(LOG_DEBUG, __FILE__, __LINE__, "��ʼ��DREB index[%d] [%d %d][%s-%d] ���Ĺ㲥����", \
        index, m_pSockMgr.at(index)->m_nDrebId, m_pSockMgr.at(index)->m_nDrebPrivateId, \
        m_pSockMgr.at(index)->m_sDrebIp.c_str(), m_pSockMgr.at(index)->m_nDrebPort);
#endif
    rcvdata.sMsgBuf->sDBHead.cCmd = CMD_SUBSCRIBE;
    rcvdata.sMsgBuf->sDBHead.cRaflag = 0;
    rcvdata.sMsgBuf->sDBHead.cNextFlag = 0;
    rcvdata.sMsgBuf->sDBHead.cZip = 0;

    if (listint.First(funcNo))
    {
        data = (S_SERVICEREG_MSG*)rcvdata.sMsgBuf->sBuffer;
        data->nSvrMainId = m_pRes->g_nSvrMainId;
        m_pDrebEndian.Endian2Comm((unsigned char*)&(data->nSvrMainId), sizeof(data->nSvrMainId));
        data->cSvrPrivateId = m_pRes->g_nSvrPrivateId;
        data->nFuncNum = 1;
        data->nFuncNo = funcNo;
        m_pDrebEndian.Endian2Comm((unsigned char*)&(data->nFuncNo), sizeof(data->nFuncNo));
        offset = sizeof(S_SERVICEREG_MSG);
    }
    bRet = listint.Next(funcNo);
    while (bRet)
    {
        func = (unsigned int*)(rcvdata.sMsgBuf->sBuffer + offset);
        *func = funcNo;
        m_pDrebEndian.Endian2Comm((unsigned char*)func, sizeof(unsigned int));
        data->nFuncNum++;
        offset += sizeof(unsigned int);
        if (offset > BPUDATASIZE - 30)
        {
            m_pMemPool.PoolFree(rcvdata.sMsgBuf);
#ifdef _ENGLISH_
            m_pLog.LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "too many txcode");
#else
            m_pLog.LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "ע�Ṧ��̫��,һ�����ݰ��޷���DREB��ע�ᣬ����ٹ�����");
#endif
            rcvdata.sMsgBuf = NULL;
            return;
        }
        bRet = listint.Next(funcNo);
    }
#ifdef _ENGLISH_
    m_pLog.LogMp(LOG_DEBUG, __FILE__, __LINE__, "total %d function subscribe", data->nFuncNum);
#else	
    m_pLog.LogMp(LOG_DEBUG, __FILE__, __LINE__, "�ܹ���%d�����ܹ㲥����", data->nFuncNum);
#endif
    m_pDrebEndian.Endian2Comm((unsigned char*)&(data->nFuncNum), sizeof(data->nFuncNum));
    rcvdata.sMsgBuf->sDBHead.nLen = offset;
    rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN + rcvdata.sMsgBuf->sDBHead.nLen;
#ifdef _ENGLISH_
    m_pLog.LogMp(LOG_DEBUG + 2, __FILE__, __LINE__, "register data len=%d", rcvdata.sMsgBuf->sDBHead.nLen);
#else
    m_pLog.LogMp(LOG_DEBUG + 2, __FILE__, __LINE__, "�㲥���ķ��͵����� ����=%d", rcvdata.sMsgBuf->sDBHead.nLen);
#endif

    m_pSockMgr.at(index)->SendMsg(rcvdata);
}

void CBF_DrebServer::UnSubscribe()
{
    S_BPC_RSMSG rcvdata;
    S_SERVICEREG_MSG* data = NULL;

    for (int i = 0; i < m_pRes->g_vDrebLinkInfo.size(); i++)
    {
        if (m_pSockMgr.at(i)->m_sock == INVALID_SOCKET || !m_pSockMgr.at(i)->m_bChecked)
        {
            continue;
        }
        rcvdata.sMsgBuf = (PBPCCOMMSTRU)m_pMemPool.PoolMalloc();
        if (rcvdata.sMsgBuf == NULL)
        {
            m_pLog.LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "������Ϣ�ռ����");
            return;
        }
        memset(rcvdata.sMsgBuf, 0, sizeof(BPCCOMMSTRU));
#ifdef _ENGLISH_
        m_pLog.LogMp(LOG_DEBUG, __FILE__, __LINE__, "DREB index[%d] [%d %d][%s-%d] unsubscribe", \
            i, m_pSockMgr.at(i)->m_nDrebId, m_pSockMgr.at(i)->m_nDrebPrivateId, \
            m_pSockMgr.at(i)->m_sDrebIp.c_str(), m_pSockMgr.at(i)->m_nDrebPort);
#else
        m_pLog.LogMp(LOG_DEBUG, __FILE__, __LINE__, "��ʼ��DREB index[%d] [%d %d][%s-%d] ȡ���㲥����", \
            i, m_pSockMgr.at(i)->m_nDrebId, m_pSockMgr.at(i)->m_nDrebPrivateId, \
            m_pSockMgr.at(i)->m_sDrebIp.c_str(), m_pSockMgr.at(i)->m_nDrebPort);
#endif
        rcvdata.sMsgBuf->sDBHead.cCmd = CMD_UNSUBSCRIBE;
        rcvdata.sMsgBuf->sDBHead.cRaflag = 0;
        rcvdata.sMsgBuf->sDBHead.cNextFlag = 0;
        rcvdata.sMsgBuf->sDBHead.cZip = 0;
        data = (S_SERVICEREG_MSG*)rcvdata.sMsgBuf->sBuffer;
        data->nSvrMainId = m_pRes->g_nSvrMainId;
        m_pDrebEndian.Endian2Comm((unsigned char*)&(data->nSvrMainId), sizeof(data->nSvrMainId));
        data->cSvrPrivateId = m_pRes->g_nSvrPrivateId;
        data->nFuncNum = 1;
        data->nFuncNo = 0;
        m_pDrebEndian.Endian2Comm((unsigned char*)&(data->nFuncNo), sizeof(data->nFuncNo));
        m_pDrebEndian.Endian2Comm((unsigned char*)&(data->nFuncNum), sizeof(data->nFuncNum));
        rcvdata.sMsgBuf->sDBHead.nLen = sizeof(S_SERVICEREG_MSG);
        rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN + rcvdata.sMsgBuf->sDBHead.nLen;
        m_pSockMgr.at(i)->SendMsg(rcvdata);

    }
    return;
}
