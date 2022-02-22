// LinkThread.cpp: implementation of the CLinkThread class.
//
//////////////////////////////////////////////////////////////////////

#include "LinkThread.h"
#include "dreberrcode.h"
#include "Bpc_Timer.h"
#include "BF_Xml.h"

extern CBpc_Timer      g_pBpcTime;
#define  DESENCRYPTKEY    "rd402wms"
#define  DESENCRYPTKEY3   "wms13711songfree"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern char g_VERSION[20];
CLinkThread::CLinkThread()
{
	m_bIsRunning = false;
	m_pRes = NULL;
	m_pSockMgr = NULL;
	m_pMemPool = NULL;
	m_pPoolData = NULL;
	m_pFuncTbl = NULL;
	m_bIsInit = false;
	m_nBegin=0;
	m_nScanBegin = 0;
	m_nEnd=1;
	m_sThreadName = "CLinkThread";
	m_bIsPingTimer = false;
	m_pLog = NULL;
}

CLinkThread::~CLinkThread()
{
	m_pRes = NULL;
	m_pSockMgr = NULL;
	m_pMemPool = NULL;
	m_pPoolData = NULL;
	m_pFuncTbl = NULL;
	m_pLog = NULL;
}

void CLinkThread::ExitThreadInstance()
{
	return ;
}
bool CLinkThread::InitThreadInstance()
{
	return true;
}

bool CLinkThread::SetGlobalVar(CGResource *res,CPoolDataMsg *pooldata,CBF_BufferPool *mempool,CSocketMgr *sockmgr,CFuncTbl *tbl)
{
	m_pRes = res;
	m_pSockMgr = sockmgr;
	m_pMemPool = mempool;
	m_pPoolData = pooldata;
	m_pFuncTbl = tbl;

	m_pLog = &(m_pRes->m_log);
	m_bIsInit = true;

	m_pTimer.Init(100,true);
	//�ڵ�·�ɶ�ʱ��
	m_pTimer.SetTimer(0,5000,&CLinkThread::OnTimer,this); //���ö�ʱ�� 
	
	

	return true;
}
int CLinkThread::Run()
{
	struct timeval tv;
	int result;
	int i,j;
	int maxfd=0;
	int ret;
	int lasttime=time(NULL);
	int lasttime2=time(NULL);
	while(!m_pRes->g_bToExit)
	{
		FD_ZERO(&m_rset);
		FD_ZERO(&m_wset);
#if defined(_WINDOWS)
		FD_ZERO(&m_eset);
#endif
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		m_nScanBegin = m_nBegin;
		//ȡ������socket����socket���뵽fd_set����
		ret = GetMaxfdAddSelect(maxfd);
		
		if (m_nBegin == 0) //��������socket����������
		{
			FD_SET(m_pSockMgr->at(0)->m_sock, &m_rset);//���뵽��select����
			if (m_pSockMgr->at(0)->m_sock > maxfd)
			{
				maxfd = m_pSockMgr->at(0)->m_sock;
			}
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
	m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ֹͣ�����߳�");
	return 0;
}
int CLinkThread::GetMaxfdAddSelect(int &maxfd)
{
	int ret=0;
	maxfd = 0;
	m_nScanBegin = m_nBegin;
	if (m_nBegin == 0) //������������1��ʼ
	{
		m_nScanBegin = 1;
	}
	for (int i=m_nScanBegin;i<m_nEnd;i++)
	{
		if (!m_pSockMgr->at(i)->IsCanSelect())
		{
			continue;
		}
		else
		{
			//��Ҫ������
			if (m_pSockMgr->at(i)->NeedRead())
			{
				FD_SET(m_pSockMgr->at(i)->m_sock, &m_rset);//���뵽��select����
			}
			//��Ҫ�������ݻ���������ʱ
			if (m_pSockMgr->at(i)->NeedSend() || m_pSockMgr->at(i)->NeedConnect())
			{
				FD_SET(m_pSockMgr->at(i)->m_sock, &m_wset);//���뵽дselect����
			}
#if defined(_WINDOWS)
			if (m_pSockMgr->at(i)->NeedConnect())
			{
				FD_SET(m_pSockMgr->at(i)->m_sock, &m_eset);//���뵽�����쳣����
			}
#endif
			//�õ�����socket 
			if (m_pSockMgr->at(i)->m_sock > maxfd)
			{
				maxfd=m_pSockMgr->at(i)->m_sock; 
			}
			ret++;
		}
	}
	return ret;
}
void CLinkThread::OnNoEvent()
{
	int j;
	int tmptime;
	if (m_nBegin == 0)
	{
		m_nScanBegin = 2;//�Թ������˿�
	}
	for (j=m_nScanBegin; j<m_nEnd; j++)
	{
		if (m_pSockMgr->at(j)->m_sock != INVALID_SOCKET && m_pSockMgr->at(j)->m_nType == BPCSOCK_DREB)
		{
			tmptime = time(NULL) - m_pSockMgr->at(j)->m_nReadtime;
			if ( tmptime > m_pRes->g_nHeartRun && tmptime < m_pRes->g_nDisconnectTime)
			{
				OnPing(j);
			}
			else if (tmptime > m_pRes->g_nDisconnectTime)
			{
				OnClose(j,"���ӳ�ʱ��δ�ã��Ͽ�",__FILE__,__LINE__);
			}
		}
		if (m_pSockMgr->at(j)->m_sock != INVALID_SOCKET && m_pSockMgr->at(j)->m_nType == BPCSOCK_BPU)
		{
			tmptime = time(NULL) - m_pSockMgr->at(j)->m_nReadtime;
			if (tmptime > m_pRes->g_nDisconnectTime)
			{
				OnClose(j,"���ӳ�ʱ��δ�ã��Ͽ�",__FILE__,__LINE__);
			}
		}
		if (m_pSockMgr->at(j)->m_sock != INVALID_SOCKET)
		{
			//û�м��ͨ��
			if (!m_pSockMgr->at(j)->m_bChecked)
			{
				//�������߽ڵ�����
				if (m_pSockMgr->at(j)->m_nType == BPCSOCK_DREB) 
				{
					if (time(NULL)-m_pSockMgr->at(j)->m_nReadtime >= m_pRes->g_nHeartRun)
					{
						if (m_pSockMgr->at(j)->NeedConnect())
						{
							m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�������ӵ�%s:%dδ�ɹ�",m_pSockMgr->at(j)->m_sDrebIp.c_str(),m_pSockMgr->at(j)->m_nDrebPort);
							closesocket(m_pSockMgr->at(j)->m_sock);
							m_pSockMgr->at(j)->m_sock = INVALID_SOCKET;
						}
					}
				}// end if (m_pSockMgr->at(conindex)->m_nType == BPCSOCK_TODREB) //�������߽ڵ�����
				else if (m_pSockMgr->at(j)->m_nType == BPCSOCK_UNKNOW)
				{
					if (time(NULL)-m_pSockMgr->at(j)->m_nReadtime >= m_pRes->g_nHeartRun)
					{
						OnClose(j,"���ӷǷ�",__FILE__,__LINE__);
					} 
				}
				else 
				{
					if (time(NULL)-m_pSockMgr->at(j)->m_nReadtime >= m_pRes->g_nHeartRun)
					{
						OnClose(j,"Э�̳�ʱ",__FILE__,__LINE__);
					} 
				}
			}
		}// end if (m_pSockMgr->at(conindex)->m_sock != INVALID_SOCKET)
		if (m_pRes->g_bToExit)
		{
			return;
		}
		//�����������߽ڵ����ӣ���û��������ȥ����
		if (m_pSockMgr->at(j)->m_nType == BPCSOCK_DREB )
		{
			if (m_pSockMgr->at(j)->m_sock == INVALID_SOCKET)
			{
				OnConnect(j);
			}
		}
	}
	
}

void CLinkThread::OnEvent()
{
	int ret;
	int i;
	m_nScanBegin = m_nBegin;
	if (m_nBegin == 0)
	{
		ret = FD_ISSET(m_pSockMgr->at(0)->m_sock,&m_rset);
		if (ret != 0) //����������˿��ж����¼�����ȥ����accept
		{
			OnAccept(0);
		}
#ifdef UNIX

		ret = FD_ISSET(m_pSockMgr->at(1)->m_sock,&m_rset);
		if (ret != 0) //����������˿��ж����¼�����ȥ����accept
		{
			OnAccept(1);
		}
		
#endif
		m_nScanBegin = m_nBegin + 2;
	}

	for (i= m_nScanBegin; i<m_nEnd; i++)
	{
		if (m_pSockMgr->at(i)->m_sock == INVALID_SOCKET)//δ����
		{
			if (m_pSockMgr->at(i)->m_nType == BPCSOCK_DREB   ) //�����������߽ڵ����ӣ���ȥ����
			{
				OnConnect(i);
			}
			continue;
		}
		else
		{
			//��Ӧ���¼�
			OnReadEvent(i);
			if (m_pSockMgr->at(i)->m_sock != INVALID_SOCKET)//δ����
			{
				//��Ӧд�¼�
				OnWriteEvent(i);
			}
		}
	}
}

void CLinkThread::OnReadEvent(int conindex)
{
	int ret;
	ret = FD_ISSET(m_pSockMgr->at(conindex)->m_sock,&m_rset);
	if (ret != 0) //�ж��¼�
	{
		OnRecv(conindex);
	}
}

void CLinkThread::OnWriteEvent(int conindex)
{
	int ret;
	if (m_pSockMgr->at(conindex)->m_sock == INVALID_SOCKET)
	{
		return;
	}
#if defined(_WINDOWS)
	ret = FD_ISSET(m_pSockMgr->at(conindex)->m_sock, &m_eset);
	if (ret != 0)
	{
		//��������������DREB
		if (m_pSockMgr->at(conindex)->m_nType == BPCSOCK_DREB)
		{
			m_pLog->LogMp(2,__FILE__,__LINE__,"�������ӵ�%s:%dδ�ɹ�",m_pSockMgr->at(conindex)->m_sDrebIp.c_str(),m_pSockMgr->at(conindex)->m_nDrebPort);
			closesocket(m_pSockMgr->at(conindex)->m_sock);
			m_pSockMgr->at(conindex)->m_sock = INVALID_SOCKET;
		}
		else 
		{
			OnClose(conindex,"Other connect fail",__FILE__,__LINE__);
		}
	}
	else if (FD_ISSET(m_pSockMgr->at(conindex)->m_sock, &m_wset) !=0 )
	{
		if (m_pSockMgr->at(conindex)->NeedConnect())
		{
			if (m_pSockMgr->at(conindex)->m_nType == BPCSOCK_DREB ) 
			{
				OnConnected(conindex);//ȥ�����������߽ڵ�
			}
		}
		else if (m_pSockMgr->at(conindex)->m_bChecked)
		{
			OnSend(conindex);//��������
		}
	}
	else  if (m_pSockMgr->at(conindex)->m_bChecked) //socket��д�¼�
	{
		OnSend(conindex);//��������
	}
#else
	
	int ok;
	int oklen;
	if (FD_ISSET(m_pSockMgr->at(conindex)->m_sock, &m_wset) !=0)
	{
		if (m_pSockMgr->at(conindex)->NeedConnect())
		{
			oklen=sizeof(ok);
#if defined(HP_UX)
			getsockopt(m_pSockMgr->at(conindex)->m_sock,SOL_SOCKET,SO_ERROR,(char *)&ok,&oklen);
#else
			getsockopt(m_pSockMgr->at(conindex)->m_sock,SOL_SOCKET,SO_ERROR,(char *)&ok,(socklen_t *)&oklen);
#endif
			if (ok == 0)
			{
				if (m_pSockMgr->at(conindex)->m_nType == BPCSOCK_DREB)
				{
					OnConnected(conindex);//ȥ�����������߽ڵ�
				}
			}
			else
			{
				if (m_pSockMgr->at(conindex)->m_nType == BPCSOCK_DREB)
				{
					m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�������ӵ�%s:%dδ�ɹ�",m_pSockMgr->at(conindex)->m_sDrebIp.c_str(),m_pSockMgr->at(conindex)->m_nDrebPort);
					closesocket(m_pSockMgr->at(conindex)->m_sock);
					m_pSockMgr->at(conindex)->m_sock = INVALID_SOCKET;
				}
			} // end if (ok == 0)
		}
		else if (m_pSockMgr->at(conindex)->m_bChecked)// else if (g_connInfoList[conindex]->m_bNeedconnect)
		{
			OnSend(conindex);//��������
		} // end if (g_connInfoList[conindex]->m_bNeedconnect)
	}
	else  if (m_pSockMgr->at(conindex)->m_bChecked)
	{
		OnSend(conindex);//��������
	}
#endif

				
}


void CLinkThread::OnPing(int conindex)
{
	if (time(NULL) - m_pSockMgr->at(conindex)->m_nPingTime < BEATHEARTTICK) //С��5�벻�÷�ping
	{
		return ;
	}
	if (m_pSockMgr->at(conindex)->m_nType == BPCSOCK_DREB)
	{
		m_pSockMgr->at(conindex)->m_nPingTime = time(NULL);
		if (!ResetData(conindex,&m_pDataBuf))
		{
			return;
		}
		m_pDataBuf.sMsgBuf->sDBHead.cRaflag = 0;
		m_pDataBuf.sMsgBuf->sDBHead.cZip = 0;
		m_pDataBuf.sMsgBuf->sDBHead.nLen = 0;
		m_pDataBuf.sMsgBuf->sDBHead.cCmd = CMD_PING;
		m_pDataBuf.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
		//����ping���ݰ�
		m_pSockMgr->at(conindex)->SendMsg(m_pDataBuf);
	}
	
}

void CLinkThread::OnClose(int conindex,const char *msg,const char *filename,int fileline)
{
	if (m_pSockMgr->at(conindex)->m_sock == INVALID_SOCKET)
	{
		return;
	}
	//��socket�ر�
	m_pSockMgr->at(conindex)->OnClose(msg,filename,fileline);
}


void CLinkThread::OnSend(int conindex)
{
	m_pSockMgr->at(conindex)->OnSend();
}
void CLinkThread::OnConnect(int conindex)
{
	if (m_pSockMgr->at(conindex)->m_sock != INVALID_SOCKET) //���ӳɹ�
	{
		return ;
	}
	//ȥ����
	if (time(NULL) - m_pSockMgr->at(conindex)->m_nConntime < 5 ) //��10���ڲ�������
	{
		return;
	}
	int ret;
	m_pSockMgr->at(conindex)->m_bChecked = false;

	m_pSockMgr->at(conindex)->m_nConntime = time(NULL);


//	m_tcpSocket.Create(AF_UNIX,false);//�첽��ʽ
	m_tcpSocket.Create(AF_INET,false);//�첽��ʽ
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�������ӵ��������߽ڵ�%s:%d:%d",m_pSockMgr->at(conindex)->m_sDrebIp.c_str(),m_pSockMgr->at(conindex)->m_nDrebPort,m_pSockMgr->at(conindex)->m_sock);
	if (m_tcpSocket.ConnectServer((char *)m_pSockMgr->at(conindex)->m_sDrebIp.c_str(),m_pSockMgr->at(conindex)->m_nDrebPort))
	{
		
		m_pSockMgr->at(conindex)->m_nReadtime = time(NULL);
		m_pSockMgr->at(conindex)->m_sock = m_tcpSocket.GetSocket();
	}
	else
	{
		if ((m_tcpSocket.GetErrorNo() == EWOULDBLOCK) || (m_tcpSocket.GetErrorNo() == EINPROGRESS))
		{
			m_pSockMgr->at(conindex)->m_nReadtime = time(NULL);
			m_pSockMgr->at(conindex)->m_sock = m_tcpSocket.GetSocket();
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ȴ�������%s:%d������",m_pSockMgr->at(conindex)->m_sDrebIp.c_str(),m_pSockMgr->at(conindex)->m_nDrebPort);
			
			return;
		}
		else
		{
			m_pSockMgr->at(conindex)->m_sock = INVALID_SOCKET;
		}
	}
	return;
	
}
void CLinkThread::OnConnected(int conindex)
{
	if (m_pSockMgr->at(conindex)->m_sock != INVALID_SOCKET) //���ӳɹ�
	{
		//���Է���������
		m_pSockMgr->at(conindex)->m_bNeedread = true;
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"������%s:%d�����ӳɹ�",m_pSockMgr->at(conindex)->m_sDrebIp.c_str(),m_pSockMgr->at(conindex)->m_nDrebPort);
		//���ӳɹ�����CMD_CONNECT��Ϣ��ȷ�����ӵ�����
		m_pSockMgr->at(conindex)->m_bNeedConnect = false;
		//����������Ϣ
		m_pSockMgr->at(conindex)->m_nPingTime = time(NULL);
		if (!ResetData(conindex,&m_pDataBuf))
		{
			return;
		}
		
		m_pDataBuf.sMsgBuf->sDBHead.cRaflag = 0;
		m_pDataBuf.sMsgBuf->sDBHead.cZip = 0;
		m_pDataBuf.sMsgBuf->sDBHead.cCmd = CMD_CONNECT;
		m_pDataBuf.sMsgBuf->sDBHead.cDrebAffirm = 0;
	
		
		m_pDataBuf.sMsgBuf->sDBHead.nLen = sizeof(S_CONNECT_DATA);
		S_CONNECT_DATA *data = (S_CONNECT_DATA *)m_pDataBuf.sMsgBuf->sBuffer;
		data->cConnectType = 1;

		data->nSvrMainId = m_pRes->g_nSvrMainId;
		data->cSvrPrivateId = m_pRes->g_nSvrPrivateId;
		sprintf(data->sVersion,"%s",g_VERSION);
		m_pDrebEndian.Endian2Comm((unsigned char *)&(data->nSvrMainId),sizeof(data->nSvrMainId));
		m_pDataBuf.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN+m_pDataBuf.sMsgBuf->sDBHead.nLen;
		m_pSockMgr->at(conindex)->SendMsg(m_pDataBuf,true);
		m_pSockMgr->at(conindex)->m_bChecked = true;
		return ;
	}
	return;
	
}
void CLinkThread::OnAccept(int conindex)
{
	char address[40];
	bzero(address,sizeof(address));
	SOCKET_HANDLE tsock=INVALID_SOCKET;
	int i;
	bool havefree=false;
	m_tcpSocket.AttachSocket(m_pSockMgr->at(conindex)->m_sock,"");
	tsock= m_tcpSocket.Accept(address,false);
	int num;
	num = 31;
	
	if (tsock != INVALID_SOCKET)
	{

		for (i=m_pRes->g_vDrebLinkInfo.size()+2; i<num; i++)
		{
			if ( m_pSockMgr->at(i)->m_sock == INVALID_SOCKET )
			{
				havefree = true;
				m_pSockMgr->at(i)->SetAccept(tsock,address);
				m_pSockMgr->at(i)->m_nType = SOCK_CLI;
				break;
			}
		}
		if (havefree)
		{
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���մ�%s��BPU����socket[%d] index[%d]",address,tsock,i);
		}
		else
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"BPU���ӹ��࣬��%s������socket%d���ܾ�",address,tsock);
			closesocket(tsock);
		}
	}
}

void CLinkThread::OnRecv(int conindex)
{
	int ret=0;
	int i=0;
	
	ret = m_pSockMgr->at(conindex)->OnRecv();	
	if (ret<=0)
	{
		OnClose(conindex,"Զ�˹رգ��������ݲ�����",__FILE__,__LINE__);
		return;
	}
	while (m_pSockMgr->at(conindex)->m_nRcvBufLen>0)
	{
//		m_pRes->PutTime();
		if (!ResetData(conindex,&m_pDataRcv))
		{
			return;
		}
		ret = m_pSockMgr->at(conindex)->GetRecvData(&m_pDataRcv);
		if (ret>0)
		{
			if (m_pSockMgr->at(conindex)->m_nType == BPCSOCK_DREB)
			{
				//����dreb����������
				if (m_pDataRcv.sMsgBuf->sDBHead.cCmd == CMD_PING) //��ӦPINGӦ��
				{
					m_pDataRcv.sMsgBuf->sDBHead.cRaflag = 1;
					m_pSockMgr->at(conindex)->SendMsg(m_pDataRcv);
//					m_pRes->LogTime();
					return ;
				}
				//����Ӧ����Ϊ���ɿ����ͻ�ȫ��㲥��Ϣ��ֱ�Ӷ���
				if (m_pDataRcv.sMsgBuf->sDBHead.cRaflag == 1 && (m_pDataRcv.sMsgBuf->sDBHead.cCmd == CMD_DPPUSH || m_pDataRcv.sMsgBuf->sDBHead.cCmd == CMD_DPABC))
				{
					m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"CMD_DPPUSH��CMD_DPABC��Ӧ�𣬶���");
					m_pMemPool->PoolFree(m_pDataRcv.sMsgBuf);
					m_pDataRcv.sMsgBuf = NULL;
//					m_pRes->LogTime();
					continue;
				}
				ProcessDreb(m_pDataRcv);
//				m_pRes->PutTime();
//				m_pRes->LogTime();
			}
			else //bpu���ӹ�����bpuisreqע����Ϣ
			{
				if (m_pDataRcv.sMsgBuf->sBpcHead.cMsgType != MSG_BPUISREG)
				{
					OnClose(conindex,"BPU����Ӧ�ȷ�MSG_BPUISREG��Ϣ",__FILE__,__LINE__);
//					m_pRes->LogTime();
					return ;
				}
				OnBpuIsReg(conindex,m_pDataRcv);
//				m_pRes->LogTime();
			}
			
		}
		else
		{
			m_pMemPool->PoolFree(m_pDataRcv.sMsgBuf);
			m_pDataRcv.sMsgBuf = NULL;
			break;
		}
	}
	
	return ;
}


bool CLinkThread::ResetData(int index,S_BPC_RSMSG *data)
{
	data->sMsgBuf = (PBPCCOMMSTRU )m_pMemPool->PoolMalloc();
	if (data->sMsgBuf == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"������Ϣ�ռ����");
		m_pSockMgr->at(index)->OnClose("������Ϣ�ռ����",__FILE__,__LINE__);
		return false;
	}
	data->sMsgBuf->sBpcHead.nConnectTime= m_pSockMgr->at(index)->m_nConntime;
	data->sMsgBuf->sBpcHead.nIndex = index;
	data->sMsgBuf->sBpcHead.nRtime = time(NULL);
	
	return true;
}


void CLinkThread::ProcessDreb(S_BPC_RSMSG &rcvdata)
{
	int flag=0;
	if (rcvdata.sMsgBuf->sDBHead.cRaflag == 0) //����
	{
		S_FUNCINFO_TBL fn;
		if (rcvdata.sMsgBuf->sDBHead.cNextFlag == 2 ) //ȡ��������
		{
			GetBpcNext(rcvdata);
			return;
		}
		if (rcvdata.sMsgBuf->sDBHead.cNextFlag == 3) //bpuȡ������
		{
			m_pLog->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"�յ�BPUȡ��������������");
			rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN+rcvdata.sMsgBuf->sDBHead.nLen;
			rcvdata.sMsgBuf->sBpcHead.cMsgType = MSG_GETNEXT;
			m_pPoolData->PushData(rcvdata);
			return ;
		}
		switch (rcvdata.sMsgBuf->sDBHead.cCmd)
		{
			case CMD_PING:
				rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
				rcvdata.sMsgBuf->sDBHead.nLen = 0;
				rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
				m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
				break;
			case CMD_DPCALL:
			case CMD_DPACALL:
			case CMD_DPBC:
			case CMD_DPABC:
			case CMD_DPPUSH:
			case CMD_DPPOST:
				m_pLog->LogBin(LOG_DEBUG+2,__FILE__,__LINE__,"�յ���������",rcvdata.sMsgBuf->sBuffer,rcvdata.sMsgBuf->sDBHead.nLen);
//				m_pRes->PutTime();

// 				//���Դ���
// 				rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
// 				sprintf(rcvdata.sMsgBuf->sBuffer,"����bpcֱ�ӷ���");
// 				rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
// 				rcvdata.sMsgBuf->sDBHead.cZip=0;
// 				m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
// 
// 				return;
				//������У�����Ϣ�߳�ȡ��������Ϣ��������Ӧ��BPU��
				m_pPoolData->PushData(rcvdata);
				break;	
			case CMD_MONITOR_BPC:
				CallBpcLocalFunc(rcvdata);
				break;	
			default:
				m_pMemPool->PoolFree(rcvdata.sMsgBuf);
				rcvdata.sMsgBuf = NULL;
		}
		return;
		
	}
	else  //��DREB��Ӧ��
	{
		switch (rcvdata.sMsgBuf->sDBHead.cCmd)
		{
			case CMD_PING:
				m_pMemPool->PoolFree(rcvdata.sMsgBuf);
				rcvdata.sMsgBuf = NULL;
				break;
			case CMD_CONNECT:
				if (rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode != SUCCESS)
				{
					m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"��������ʧ�� %d",rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode);
					m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->OnClose("DREB���ط�������ʧ��",__FILE__,__LINE__);
					m_pMemPool->PoolFree(rcvdata.sMsgBuf);
					rcvdata.sMsgBuf = NULL;
					return;
				}
				m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->m_bChecked = true;
				m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->m_nDrebId = rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nNodeId;
				m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->m_nDrebPrivateId = rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_cNodePrivateId;
				
				memcpy(&(m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->m_head),&(rcvdata.sMsgBuf->sDBHead),DREBHEADLEN);
				if (m_pRes->g_nRegisterMode != REGISTERMODE_NONE)
				{
					RegisterDreb(rcvdata.sMsgBuf->sBpcHead.nIndex);
				}
				
				m_pMemPool->PoolFree(rcvdata.sMsgBuf);
				rcvdata.sMsgBuf = NULL;
				
				break;
			case CMD_DEL_SVR:
				m_pMemPool->PoolFree(rcvdata.sMsgBuf);
				rcvdata.sMsgBuf = NULL;
				break;
			case CMD_REGSERVICE:
				m_pMemPool->PoolFree(rcvdata.sMsgBuf);
				rcvdata.sMsgBuf = NULL;
				break;	
			case CMD_DPCALL:
			case CMD_DPACALL:
			case CMD_DPBC:
			case CMD_DPABC:
			case CMD_DPPUSH:
			case CMD_DPPOST:
				//bpc�����ֱ��Ӧ���bpu
				if (m_pSockMgr->AffirmIndex(rcvdata.sMsgBuf->sDBHead.b_Cinfo.b_cIndex))
				{
					//�ж�����ʱ����Ƿ���ͬ ,b_Cinfo���b_cIndex���bpu����index,b_nSerial���bpu����indexʱ���
					if (rcvdata.sMsgBuf->sDBHead.b_Cinfo.b_nSerial == m_pSockMgr->at(rcvdata.sMsgBuf->sDBHead.b_Cinfo.b_cIndex)->m_nConntime)
					{
						rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN + rcvdata.sMsgBuf->sDBHead.nLen;
						rcvdata.sMsgBuf->sBpcHead.nBpuIndex = rcvdata.sMsgBuf->sDBHead.b_Cinfo.b_cIndex;
						rcvdata.sMsgBuf->sBpcHead.nConnectTime = m_pSockMgr->at(rcvdata.sMsgBuf->sDBHead.b_Cinfo.b_cIndex)->m_nConntime;
						//Ӧ��ص�BPC���϶���extcall��
						rcvdata.sMsgBuf->sBpcHead.cMsgType = MSG_EXTCALL;
						m_pSockMgr->at(rcvdata.sMsgBuf->sDBHead.b_Cinfo.b_cIndex)->SendMsg(rcvdata);
						return;
					}
					m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"BPU���Ӧ��ʱ����ʱ������� index[%d]����Ϣ[%s]  DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Դ[%d %d] czip[%d] ҵ�����ݳ���[%d]",\
						rcvdata.sMsgBuf->sDBHead.b_Cinfo.b_cIndex,m_pRes->GetBpcMsgType(rcvdata.sMsgBuf->sBpcHead.cMsgType).c_str(),\
						m_pRes->GetDrebCmdType(rcvdata.sMsgBuf->sDBHead.cCmd).c_str(),rcvdata.sMsgBuf->sDBHead.cNextFlag,\
						rcvdata.sMsgBuf->sDBHead.cRaflag,rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
							rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
							rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
							rcvdata.sMsgBuf->sDBHead.cZip,rcvdata.sMsgBuf->sDBHead.nLen);
					m_pRes->m_pBpcCallDataLog.LogDreb(LOG_ERROR,rcvdata.sMsgBuf,true);
//					g_pBpcLog.LogBpc(LOG_ERROR,rcvdata.sMsgBuf,true);
				}
				else
				{
					m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"BPU���Ӧ��ʱ����index�Ƿ� index[%d]����Ϣ[%s]  DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Դ[%d %d] cZip[%d] ҵ�����ݳ���[%d]",\
						rcvdata.sMsgBuf->sDBHead.b_Cinfo.b_cIndex,m_pRes->GetBpcMsgType(rcvdata.sMsgBuf->sBpcHead.cMsgType).c_str(),\
						m_pRes->GetDrebCmdType(rcvdata.sMsgBuf->sDBHead.cCmd).c_str(),rcvdata.sMsgBuf->sDBHead.cNextFlag,\
						rcvdata.sMsgBuf->sDBHead.cRaflag,rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
						rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
						rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
						rcvdata.sMsgBuf->sDBHead.cZip,rcvdata.sMsgBuf->sDBHead.nLen);
					m_pRes->m_pBpcCallDataLog.LogDreb(LOG_ERROR,rcvdata.sMsgBuf,true);
//					g_pBpcLog.LogBpc(LOG_ERROR,rcvdata.sMsgBuf,true);
				}
				m_pMemPool->PoolFree(rcvdata.sMsgBuf);
				rcvdata.sMsgBuf = NULL;
				break;	
			default:
				m_pMemPool->PoolFree(rcvdata.sMsgBuf);
				rcvdata.sMsgBuf = NULL;
				break;
		}
		return;
	}
}


void CLinkThread::CallBpcLocalFunc(S_BPC_RSMSG &rcvdata)
{
// #define  BPCMONI_INFO       11     //bpc��Ϣ   �ڵ� bpu��
// #define  BPCMONI_HOST       12     //������Ϣ  CPU ���� �ڴ�
// #define  BPCMONI_BPU        13     //·����Ϣ  ����bpu��
// #define  BPCMONI_REPORT     14     //����ͳ����Ϣ  ���������� ������  
// #define  BPCMONI_SETBPU     15     //����BPU��Ŀ
// #define  BPCMONI_SETMODE    16     //����bpc����ģʽ
// #define  BPCMONI_STOPBPU    17     //ֹͣBPU
// #define  BPCMONI_STARTBPU   18     //����BPU
	//DREB�����ĵ���
	switch (rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo)
	{
		case BPCMONI_INFO:
			MonitorInfo(rcvdata);
			break;
		case BPCMONI_HOST:
			MonitorHost(rcvdata);
			break;
		case BPCMONI_BPU:
			MonitorBpu(rcvdata);
			break;
		case BPCMONI_REPORT:
			MonitorReport(rcvdata);
			break;
		case BPCMONI_SETBPU:
			MonitorSetBpu(rcvdata);
			break;
		case BPCMONI_SETMODE:
			MonitorSetMode(rcvdata);
			break;
		case BPCMONI_STOPBPU:
			MonitorStopBpu(rcvdata);
			break;
		case BPCMONI_STARTBPU:
			MonitorStartBpu(rcvdata);
			break;
		case BPCMONI_DOWNLOAD:
			Download(rcvdata);
			break;
		case BPCMONI_UPLOAD:
			Upload(rcvdata);
			break;
		default:
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_FUNCNOTFUND;
			rcvdata.sMsgBuf->sDBHead.nLen = 0;
			rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
			m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
			break;
	}
	

}
void CLinkThread::RegisterDreb(int index)
{
	S_BPC_RSMSG rcvdata;
	S_FUNCINFO_TBL fc;
	S_SERVICEREG_MSG *data=NULL;
	bool bRet;
	int  offset;
	unsigned int *func=NULL;
	if (m_pSockMgr->at(index)->m_sock == INVALID_SOCKET || !m_pSockMgr->at(index)->m_bChecked )
	{
		return;
	}
	rcvdata.sMsgBuf = (PBPCCOMMSTRU )m_pMemPool->PoolMalloc();
	if (rcvdata.sMsgBuf == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"������Ϣ�ռ����");
		return ;
	}
	//
	memset(rcvdata.sMsgBuf,0,sizeof(BPCCOMMSTRU));
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"��ʼ��DREBע������� %s-%d",m_pSockMgr->at(index)->m_sDrebIp.c_str(),m_pSockMgr->at(index)->m_nDrebPort);
	rcvdata.sMsgBuf->sDBHead.cCmd = CMD_REGSERVICE;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 0;
	rcvdata.sMsgBuf->sDBHead.cNextFlag = 0;
	rcvdata.sMsgBuf->sDBHead.cZip = 0;

	vector<S_FUNCINFO_TBL>vfunclist;
	if (m_pFuncTbl->GetAllFunc(vfunclist)<1)
	{
		m_pMemPool->PoolFree(rcvdata.sMsgBuf);
		m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"û��ע�Ṧ��");
		rcvdata.sMsgBuf = NULL;
		return;
	}
	
	for (int i=0 ; i< vfunclist.size() ; i++)
	{
		if (i == 0)
		{
			data = (S_SERVICEREG_MSG *)rcvdata.sMsgBuf->sBuffer;
			data->nSvrMainId = m_pRes->g_nSvrMainId;
			m_pDrebEndian.Endian2Comm((unsigned char *)&(data->nSvrMainId),sizeof(data->nSvrMainId));
			data->cSvrPrivateId = m_pRes->g_nSvrPrivateId;
			data->nFuncNum = 1;
			data->nFuncNo = vfunclist[i].nFuncNo;
			m_pDrebEndian.Endian2Comm((unsigned char *)&(data->nFuncNo),sizeof(data->nFuncNo));
			offset = sizeof(S_SERVICEREG_MSG);
		}
		else
		{
			func = (unsigned int *)(rcvdata.sMsgBuf->sBuffer+offset);
			*func = vfunclist[i].nFuncNo;
			m_pDrebEndian.Endian2Comm((unsigned char *)func,sizeof(unsigned int));
			data->nFuncNum++;
			offset += sizeof(unsigned int);
			if (offset> BPUDATASIZE-30)
			{
				m_pMemPool->PoolFree(rcvdata.sMsgBuf);
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"ע�Ṧ��̫��,һ�����ݰ��޷���DREB��ע�ᣬ����ٹ�����");
				rcvdata.sMsgBuf = NULL;
				return;
			}
		}
	}
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ܹ���%d������ע��",data->nFuncNum);
	m_pDrebEndian.Endian2Comm((unsigned char *)&(data->nFuncNum),sizeof(data->nFuncNum));
	rcvdata.sMsgBuf->sDBHead.nLen = offset;
	rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN + rcvdata.sMsgBuf->sDBHead.nLen;

	m_pLog->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"����ע�ᷢ�͵����� ����=%d",rcvdata.sMsgBuf->sDBHead.nLen);

	m_pSockMgr->at(index)->SendMsg(rcvdata);
}

void CLinkThread::GetBpcNext(S_BPC_RSMSG &rcvdata)
{
	char sNextFile[400];
#ifdef _WINDOWS
	sprintf(sNextFile,"%s\\nxt\\%ld.nxt",m_pRes->g_sCurPath,rcvdata.sMsgBuf->sDBHead.n_Ninfo.n_nNextNo);
#else
	sprintf(sNextFile,"%s/nxt/%ld.nxt",m_pRes->g_sCurPath,rcvdata.sMsgBuf->sDBHead.n_Ninfo.n_nNextNo);
#endif
	FILE *fp = fopen(sNextFile,"rb");
	if (fp == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"�������ļ�[%s]������",sNextFile);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_NEXTFILENOT;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cNextFlag = 0;
		rcvdata.sMsgBuf->sDBHead.nLen = 0;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
		return;
	}
	fseek(fp,rcvdata.sMsgBuf->sDBHead.n_Ninfo.n_nNextOffset,1);
	int ret =  fread(rcvdata.sMsgBuf->sBuffer,1,BPUDATASIZE,fp);
	fclose(fp);
	struct_stat fs;
	FSTAT(sNextFile,&fs);
	
	rcvdata.sMsgBuf->sDBHead.nLen = ret;
	rcvdata.sMsgBuf->sDBHead.n_Ninfo.n_nNextOffset+=ret;
	if (ret < BPUDATASIZE)
	{
		rcvdata.sMsgBuf->sDBHead.cNextFlag = 0; //û�к�������
	}
	else
	{
		if (rcvdata.sMsgBuf->sDBHead.n_Ninfo.n_nNextOffset == fs.st_size) //������β
		{
			rcvdata.sMsgBuf->sDBHead.cNextFlag = 0;//û�к�������
		}
		else
		{
			rcvdata.sMsgBuf->sDBHead.cNextFlag = 2;//�к���
		}
	}

	rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
	rcvdata.sMsgBuf->sDBHead.cZip = 0;
	m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
	return;
}

void CLinkThread::LogDrebHead(int loglevel, DREB_HEAD head, const char *msg, const char *filename, int fileline)
{
	bzero(m_headMsg,sizeof(m_headMsg));
	sprintf(m_headMsg,"%s DREBHEAD��Ϣ cZip=%d cCmd=%s cRaflag=%d cNextFlag=%d cDrebAffirm=%d s_nNodeId=%d \
		s_cNodePrivateId=%d s_nSvrMainId=%d s_cSvrPrivateId=%d s_nHook=%d s_nSerial=%d \
		s_nDrebSerial=%d s_nIndex=%d d_nNodeId=%d d_cNodePrivateId=%d d_nSvrMainId=%d \
		d_cSvrPrivateId=%d d_nServiceNo=%d a_nNodeId=%d a_cNodePrivateId=%d a_nRetCode=%d \
		n_nNextNo=%d n_nNextOffset=%d b_nSerial=%d b_cIndex=%d nLen=%d","%s",head.cZip,m_pRes->GetDrebCmdType(head.cCmd).c_str(),\
		head.cRaflag,head.cNextFlag,head.cDrebAffirm,head.s_Sinfo.s_nNodeId,head.s_Sinfo.s_cNodePrivateId,\
		head.s_Sinfo.s_nSvrMainId,head.s_Sinfo.s_cSvrPrivateId,head.s_Sinfo.s_nHook,head.s_Sinfo.s_nSerial,\
		head.s_Sinfo.s_nDrebSerial,head.s_Sinfo.s_nIndex,head.d_Dinfo.d_nNodeId,head.d_Dinfo.d_cNodePrivateId,\
		head.d_Dinfo.d_nSvrMainId,head.d_Dinfo.d_cSvrPrivateId,head.d_Dinfo.d_nServiceNo,\
		head.a_Ainfo.a_nNodeId,head.a_Ainfo.a_cNodePrivateId,head.a_Ainfo.a_nRetCode,\
		head.n_Ninfo.n_nNextNo,head.n_Ninfo.n_nNextOffset,head.b_Cinfo.b_nSerial,\
		head.b_Cinfo.b_cIndex,head.nLen);
	m_pLog->LogMp(loglevel,filename,fileline,m_headMsg,msg);
}

void CLinkThread::MonitorInfo(S_BPC_RSMSG &rcvdata)
{
	int i;
	CBF_Xml xmlpack;
	xmlpack.SetNodeValueByPath("Monitor/BPC�����ڵ�",false,(int)m_pRes->g_nSvrMainId);
	xmlpack.SetNodeValueByPath("Monitor/BPC˽�нڵ�",false,(int)m_pRes->g_nSvrPrivateId);
	xmlpack.SetNodeValueByPath("Monitor/����ʱ��",false,m_pRes->g_sStartDateTime,false);
	//������Ϣ
	for (i=0 ; i < m_pRes->g_vDrebLinkInfo.size() ; i++)
	{
		CXmlNode *connnode = xmlpack.AddNodeByPath("Monitor/DREB������Ϣ",false,"����",false,"");
		if (connnode == NULL)
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"AddNodeByPath ����");
			continue;
		}
		connnode->SetAttribute("ip",false,m_pRes->g_vDrebLinkInfo[i].sIp);
		connnode->SetAttribute("port",false,(int)m_pRes->g_vDrebLinkInfo[i].nPort);
		connnode->SetAttribute("index",false,i+2);
		connnode->SetAttribute("�����ڵ�",false,m_pSockMgr->at(i+2)->m_nDrebId);
		connnode->SetAttribute("˽�нڵ�",false,m_pSockMgr->at(i+2)->m_nDrebPrivateId);
		if (m_pSockMgr->at(i+2)->m_sock == INVALID_SOCKET)
		{
			connnode->SetAttribute("status",false,"δ����");
		}
		else if (m_pSockMgr->at(i+2)->m_bChecked)
		{
			connnode->SetAttribute("status",false,"����");
		}
		else
		{
			connnode->SetAttribute("status",false,"��������");
		}
	}
	
	//BPU��Ϣ
	int k;
	int connNum=0;
	xmlpack.AddNodeByPath("Monitor",false,"BPU�б�",false,"");
	for (i=0 ; i<m_pRes->g_vBpuGroupInfo.size(); i++)
	{
		connNum=0;
		for (k=m_pRes->g_vBpuGroupInfo[i].g_nBeginSocketIndex ; k < m_pRes->g_vBpuGroupInfo[i].g_nEndSocketIndex; k++)
		{
			if (m_pSockMgr->at(k)->m_sock != INVALID_SOCKET)
			{
				connNum++;
			}
		}
		CXmlNode *bpunode = xmlpack.AddNodeByPath("Monitor/BPU�б�",false,m_pRes->g_vBpuGroupInfo[i].g_sBpuGroupName.c_str(),true,"");
		bpunode->SetAttribute("����BPU��",false,m_pRes->g_vBpuGroupInfo[i].g_nBpuNum);
		bpunode->SetAttribute("ʵ��BPU��",false,connNum);
		bpunode->SetAttribute("����ģʽ",false,m_pRes->g_vBpuGroupInfo[i].g_nDispatchMode);
		bpunode->SetAttribute("�Զ�����",false,m_pRes->g_vBpuGroupInfo[i].g_bIsAutoLoadBu);
		bpunode->SetAttribute("��ʼ��������",false,m_pRes->g_vBpuGroupInfo[i].g_nBeginSocketIndex);
		bpunode->SetAttribute("������������",false,m_pRes->g_vBpuGroupInfo[i].g_nEndSocketIndex);
	}

	int len = DREBDATASIZE;
	if (!xmlpack.ToBuffer(rcvdata.sMsgBuf->sBuffer,len))
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"���岻�㣬������bpcinfo.txt�鿴");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���岻��");
		xmlpack.ToFile("bpcinfo.txt");
	}
	else
	{
		rcvdata.sMsgBuf->sDBHead.nLen = len;
	}

	rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
	rcvdata.sMsgBuf->sDBHead.cZip = 0;
	m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
}

void CLinkThread::MonitorHost(S_BPC_RSMSG &rcvdata)
{
	CBF_Xml xmlpack;
	xmlpack.SetNodeValueByPath("Monitor/BPC�����ڵ�",false,(int)m_pRes->g_nSvrMainId);
	xmlpack.SetNodeValueByPath("Monitor/BPC˽�нڵ�",false,(int)m_pRes->g_nSvrPrivateId);
	xmlpack.SetNodeValueByPath("Monitor/����ʱ��",false,m_pRes->g_sStartDateTime,false);
	
	int total;
	int unused;
	m_pMemPool->PoolInfo(total,unused);
	xmlpack.SetNodeAttribute("Monitor/�ڴ滺���",false,"�ܷ���",false,total );
	xmlpack.SetNodeAttribute("Monitor/�ڴ滺���",false,"δʹ��",false,unused );
	xmlpack.SetNodeAttribute("Monitor/�ڴ滺���",false,"ÿ���С",false,(int)BPCBUFSIZE );
	//ȡ������Ϣ
	S_MONITOR_HOST hf;
	V_MonitorDiskInfo di;
	g_pBpcTime.GetHostInfo(hf,di);
	
	xmlpack.SetNodeValueByPath("Monitor/������Դ/CPU",false,(int)hf.nCpuRate);
	
	
	xmlpack.SetNodeValueByPath("Monitor/������Դ/�ڴ�",false,"",false);
	xmlpack.SetNodeAttribute("Monitor/������Դ/�ڴ�",false,"�ܴ�СM",false,(int)hf.nTotalMemory);
	xmlpack.SetNodeAttribute("Monitor/������Դ/�ڴ�",false,"ʹ��M",false,(int)hf.nUsedMemory);
	
	for (int i=0 ; i<di.size() ; i++)
	{
		CXmlNode *disknode = xmlpack.AddNodeByPath("Monitor/������Դ/�����б�",false,"����",false,"");
		if (disknode != NULL)
		{
			disknode->SetAttribute("����",false,di[i].sDiskName);
			disknode->SetAttribute("�ܴ�СM",false,(int)di[i].nTotalSpace);
			disknode->SetAttribute("ʹ�ô�СM",false,(int)di[i].nUsedSpace);
		}
	}
	
	int len = DREBDATASIZE;
	if (!xmlpack.ToBuffer(rcvdata.sMsgBuf->sBuffer,len))
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"���岻�㣬������bpchost.txt�鿴");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���岻��");
		xmlpack.ToFile("bpchost.txt");
	}
	else
	{
		rcvdata.sMsgBuf->sDBHead.nLen = len;
	}
	
	rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
	rcvdata.sMsgBuf->sDBHead.cZip = 0;
	m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
}

void CLinkThread::MonitorBpu(S_BPC_RSMSG &rcvdata)
{
	int i,k;
	int connNum=0;
	CBF_Xml xmlpack;
	xmlpack.SetNodeValueByPath("Monitor/BPC�����ڵ�",false,(int)m_pRes->g_nSvrMainId);
	xmlpack.SetNodeValueByPath("Monitor/BPC˽�нڵ�",false,(int)m_pRes->g_nSvrPrivateId);
	xmlpack.SetNodeValueByPath("Monitor/����ʱ��",false,m_pRes->g_sStartDateTime,false);
	//BPU��Ϣ
	xmlpack.AddNodeByPath("Monitor",false,"BPU�б�",false,"");
	for (i=0 ; i<m_pRes->g_vBpuGroupInfo.size(); i++)
	{
		connNum=0;
		for (k=m_pRes->g_vBpuGroupInfo[i].g_nBeginSocketIndex ; k < m_pRes->g_vBpuGroupInfo[i].g_nEndSocketIndex; k++)
		{
			if (m_pSockMgr->at(k)->m_sock != INVALID_SOCKET)
			{
				connNum++;
			}
		}
		CXmlNode *bpunode = xmlpack.AddNodeByPath("Monitor/BPU�б�",false,m_pRes->g_vBpuGroupInfo[i].g_sBpuGroupName.c_str(),true,"");
		if (bpunode != NULL)
		{
			bpunode->SetAttribute("����",false,m_pRes->g_vBpuGroupInfo[i].g_sBpuGroupName);
			bpunode->SetAttribute("����BPU��",false,m_pRes->g_vBpuGroupInfo[i].g_nBpuNum);
			bpunode->SetAttribute("ʵ��BPU��",false,connNum);
			bpunode->SetAttribute("����Ӧ��",false,m_pRes->g_vBpuGroupInfo[i].g_bIsAnsPing);
			bpunode->SetAttribute("����ģʽ",false,m_pRes->g_vBpuGroupInfo[i].g_nDispatchMode);
			bpunode->SetAttribute("�Զ�����",false,m_pRes->g_vBpuGroupInfo[i].g_bIsAutoLoadBu);
			bpunode->SetAttribute("������",false,m_pRes->g_vBpuGroupInfo[i].g_sBuPrgName);
			bpunode->SetAttribute("����Ŀ¼",false,m_pRes->g_vBpuGroupInfo[i].g_sBuPrgPath);
			bpunode->SetAttribute("����Ŀ¼",false,m_pRes->g_vBpuGroupInfo[i].g_sBuRunPath);
			bpunode->SetAttribute("���в���",false,m_pRes->g_vBpuGroupInfo[i].g_sBuPrgPara);
			bpunode->SetAttribute("��ʼ��������",false,m_pRes->g_vBpuGroupInfo[i].g_nBeginSocketIndex);
			bpunode->SetAttribute("������������",false,m_pRes->g_vBpuGroupInfo[i].g_nEndSocketIndex);
			
		}
	}
	
	int len = DREBDATASIZE;
	if (!xmlpack.ToBuffer(rcvdata.sMsgBuf->sBuffer,len))
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"���岻�㣬������bpcbpu.txt�鿴");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���岻��");
		xmlpack.ToFile("bpcbpu.txt");
	}
	else
	{
		rcvdata.sMsgBuf->sDBHead.nLen = len;
	}
	
	rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
	rcvdata.sMsgBuf->sDBHead.cZip = 0;
	m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
}

void CLinkThread::MonitorReport(S_BPC_RSMSG &rcvdata)
{
	CBF_Xml xmlpack;
	xmlpack.SetNodeValueByPath("Monitor/BPC�����ڵ�",false,(int)m_pRes->g_nSvrMainId);
	xmlpack.SetNodeValueByPath("Monitor/BPC˽�нڵ�",false,(int)m_pRes->g_nSvrPrivateId);
	xmlpack.SetNodeValueByPath("Monitor/����ʱ��",false,m_pRes->g_sStartDateTime,false);
	char tmpchar[100];
	char nodename[200];
	vector<S_FUNCINFO_TBL> funclist;
	m_pFuncTbl->GetAllFuncByGroup(funclist);
	
	xmlpack.SetNodeValueByPath("Monitor/BPU����/������",false,(int)funclist.size());

	std::string bpugroup;
	int txnum=0;
	bzero(nodename,sizeof(nodename));
	for (int i=0 ; i<funclist.size() ; i++)
	{
		if (bpugroup.compare(funclist[i].sBpuGroupName) !=0)
		{
			if (strlen(nodename) >0)
			{
				xmlpack.SetNodeAttribute(nodename,false,"������",false,txnum);
			}
			sprintf(nodename,"Monitor/BPU����/%s",funclist[i].sBpuGroupName);
			bpugroup = funclist[i].sBpuGroupName;
			txnum = 0;
		}
		txnum++;
		sprintf(tmpchar,"%d",funclist[i].nFuncNo);
		CXmlNode *bpunode = xmlpack.AddNodeByPath(nodename,false,tmpchar,false,"",false);
		if (bpunode != NULL)
		{
			bpunode->SetAttribute("����",false,funclist[i].sFuncName);
			bpunode->SetAttribute("���ȼ�",false,(int)funclist[i].cPrio);
			bpunode->SetAttribute("���ô���",false,(int)funclist[i].nCallNum);
		}
	}
	xmlpack.SetNodeAttribute(nodename,false,"������",false,txnum);

	int len = DREBDATASIZE;
	if (!xmlpack.ToBuffer(rcvdata.sMsgBuf->sBuffer,len))
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"���岻�㣬������bpureport.txt�鿴");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���岻��");
		xmlpack.ToFile("bpureport.txt");
	}
	else
	{
		rcvdata.sMsgBuf->sDBHead.nLen = len;
	}
	rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
	rcvdata.sMsgBuf->sDBHead.cZip = 0;
	m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
}

void CLinkThread::MonitorSetBpu(S_BPC_RSMSG &rcvdata)
{
	//����BPU��Ŀ������ģʽ���Ƿ񱣴��������ļ�
// 	<Monitor>
// 		<SetBpu mode="0" remark="0ֻ�����ڴ�����1ͬʱ���µ������ļ�">
//         <bpu1>
// 				<�������� remark="��BPU��ѹ������0,���ķ���BPU1">0</��������>
// 				<����Ӧ�� remark="��Ϊc/c++��bpuʱ������Ӧ��������Ϣ">0</����Ӧ��>
// 				<����ģʽ remark="0��ʾ���з��䣬1��ʾBPU��ֻ��Ϊ1��һֱ����">0</����ģʽ>
// 				<��������BPU�� remark="�����Դ���ҵ��ĵ�Ԫ��Ŀ�����������ģʽΪ1ʱֻ��Ϊ1">1</��������BPU��>
// 				<AUTOLOADBPU use="0" remark="�Զ���BPU��������1������Ϊ0">
// 					<BPU������ remark="BU��������ƣ�����·��">bfBPUd.exe</BPU������>
// 					<BPU����·�� remark="���Ի����·��">D:\BUBASE\bin</BPU����·��>
// 					<BPU����Ŀ¼ remark="���Ի����Ŀ¼">D:\BUBASE\bin</BPU����Ŀ¼>
// 					<BPU�������� remark="����BPU����Ĳ���">D:\BUBASE\bin\bfBPU.xml</BPU��������>
// 				</AUTOLOADBPU>
//         </bpu1>
// 		</SetBpu>
// 	</Monitor>
	if (rcvdata.sMsgBuf->sDBHead.cZip != 0)
	{
		if (!UnzipBuf(*rcvdata.sMsgBuf))
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"���ݽ�ѹ������");
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
			return ;
		}
	}
	CBF_Xml  xmlpack;
	if (!xmlpack.FromBuffer(rcvdata.sMsgBuf->sBuffer))
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"�������ݷ�xml");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
		return ;
	}
	CXmlNode *setnode = xmlpack.GetNodeByPath("Monitor/SetBpu",false);
	if (setnode == NULL)
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"xml��[Monitor/SetBpu]�ڵ�");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
		return ;
	}
	int savemode=0;
	if (xmlpack.GetNodeAttribute("Monitor/SetBpu",false,"mode",false,savemode) == NULL)
	{
		savemode =0;//������Ĭ��Ϊ0
	}
	std::string bpugroupname;
	int i;
	int bpuindex=-1;
	char tmpchar[200];
	char tmpcharconf[200];
	bool isfind=false;
	std::string namevalue;
	if (savemode == 1)
	{
		CBF_Date_Time dt;
		sprintf(tmpchar,"%s.%s%s",m_pRes->m_sBpcParaFileName.c_str(),dt.ToStringD8().c_str(),dt.ToStringT6().c_str());
		m_pRes->m_xmlconf.ToFile(tmpchar);
	}
	
	
	CXmlNode *bpugnode = (CXmlNode *)setnode->GetFirstChild();
	while (bpugnode != NULL)
	{
		bpugroupname = bpugnode->GetNodeName();
		isfind=false;
		for (i=0; i< m_pRes->g_vBpuGroupInfo.size() ; i++)
		{
			if (bpugroupname.compare(m_pRes->g_vBpuGroupInfo[i].g_sBpuGroupName) == 0)
			{
				isfind = true;
				bpuindex = i;
				break;
			}
		}
		if (isfind)
		{
			//�ҵ���
			sprintf(tmpchar,"Monitor/SetBpu/%s/��������",bpugroupname.c_str());
			if (xmlpack.GetNodeValueByPath(tmpchar,false,namevalue) != NULL)
			{
				if (savemode == 1) //���������ļ�
				{
					sprintf(tmpcharconf,"package/head/BPU����/BPU��/%s/��������",bpugroupname.c_str());
					m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str());
				}
				//�����ڴ�
				if (atoi(namevalue.c_str()) == 1)
				{
					m_pRes->g_vBpuGroupInfo[bpuindex].g_bIsUnencrypt  = true;
				}
				else
				{
					m_pRes->g_vBpuGroupInfo[bpuindex].g_bIsUnencrypt  = false;
				}
			}
			sprintf(tmpchar,"Monitor/SetBpu/%s/����Ӧ��",bpugroupname.c_str());
			if (xmlpack.GetNodeValueByPath(tmpchar,false,namevalue) != NULL)
			{
				if (savemode == 1) //���������ļ�
				{
					sprintf(tmpcharconf,"package/head/BPU����/BPU��/%s/����Ӧ��",bpugroupname.c_str());
					m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str());
				}
				//�����ڴ�
				if (atoi(namevalue.c_str()) == 1)
				{
					m_pRes->g_vBpuGroupInfo[bpuindex].g_bIsAnsPing  = true;
				}
				else
				{
					m_pRes->g_vBpuGroupInfo[bpuindex].g_bIsAnsPing  = false;
				}
			}
			sprintf(tmpchar,"Monitor/SetBpu/%s/����ģʽ",bpugroupname.c_str());
			if (xmlpack.GetNodeValueByPath(tmpchar,false,namevalue) != NULL)
			{
				if (savemode == 1) //���������ļ�
				{
					sprintf(tmpcharconf,"package/head/BPU����/BPU��/%s/����ģʽ",bpugroupname.c_str());
					m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str());
				}
				//�����ڴ�
				m_pRes->g_vBpuGroupInfo[bpuindex].g_nDispatchMode = atoi(namevalue.c_str());
			}
			sprintf(tmpchar,"Monitor/SetBpu/%s/��������BPU��",bpugroupname.c_str());
			if (xmlpack.GetNodeValueByPath(tmpchar,false,namevalue) != NULL)
			{
				if (m_pRes->g_vBpuGroupInfo[bpuindex].g_nDispatchMode == 1)
				{
					namevalue = "1";
				}
				//�����ڴ�
				m_pRes->g_vBpuGroupInfo[bpuindex].g_nBpuNum  = atoi(namevalue.c_str());
				
				if (savemode == 1) //���������ļ�
				{
					sprintf(tmpcharconf,"package/head/BPU����/BPU��/%s/��������BPU��",bpugroupname.c_str());
					m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str());
				}
			}
			
			sprintf(tmpchar,"Monitor/SetBpu/%s/AUTOLOADBPU",bpugroupname.c_str());
			if (xmlpack.GetNodeAttribute(tmpchar,false,"use",false,namevalue) != NULL)
			{
				if (atoi(namevalue.c_str()) == 1)
				{
					m_pRes->g_vBpuGroupInfo[bpuindex].g_bIsAutoLoadBu = true;
				}
				else
				{
					m_pRes->g_vBpuGroupInfo[bpuindex].g_bIsAutoLoadBu = false;
				}

				if (savemode == 1) //���������ļ�
				{
					sprintf(tmpcharconf,"package/head/BPU����/BPU��/%s/AUTOLOADBPU",bpugroupname.c_str());
					m_pRes->m_xmlconf.SetNodeAttribute(tmpcharconf,false,"use",false,namevalue.c_str());
				}
				if (m_pRes->g_vBpuGroupInfo[bpuindex].g_bIsAutoLoadBu)//�Զ�����BPU
				{
					sprintf(tmpchar,"Monitor/SetBpu/%s/AUTOLOADBPU/BPU������",bpugroupname.c_str());
					if (xmlpack.GetNodeValueByPath(tmpchar,false,namevalue) != NULL)
					{
						m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuPrgName = namevalue;
						if (savemode == 1) //���������ļ�
						{
							sprintf(tmpcharconf,"package/head/BPU����/BPU��/%s/AUTOLOADBPU/BPU������",bpugroupname.c_str());
							m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str());
						}
					}
					sprintf(tmpchar,"Monitor/SetBpu/%s/AUTOLOADBPU/BPU����·��",bpugroupname.c_str());
					if (xmlpack.GetNodeValueByPath(tmpchar,false,namevalue) != NULL)
					{
						m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuPrgPath = namevalue;
						if (savemode == 1) //���������ļ�
						{
							sprintf(tmpcharconf,"package/head/BPU����/BPU��/%s/AUTOLOADBPU/BPU����·��",bpugroupname.c_str());
							m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str());
						}
					}
					sprintf(tmpchar,"Monitor/SetBpu/%s/AUTOLOADBPU/BPU����Ŀ¼",bpugroupname.c_str());
					if (xmlpack.GetNodeValueByPath(tmpchar,false,namevalue) != NULL)
					{
						m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuRunPath = namevalue;
						if (savemode == 1) //���������ļ�
						{
							sprintf(tmpcharconf,"package/head/BPU����/BPU��/%s/AUTOLOADBPU/BPU����Ŀ¼",bpugroupname.c_str());
							m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str());
						}
					}
					sprintf(tmpchar,"Monitor/SetBpu/%s/AUTOLOADBPU/BPU��������",bpugroupname.c_str());
					if (xmlpack.GetNodeValueByPath(tmpchar,false,namevalue) != NULL)
					{
						m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuPrgPara = namevalue;
						if (savemode == 1) //���������ļ�
						{
							sprintf(tmpcharconf,"package/head/BPU����/BPU��/%s/AUTOLOADBPU/BPU��������",bpugroupname.c_str());
							m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str());
						}
					}
				}
			}
			if (savemode == 1)
			{
				if (!m_pRes->m_xmlconf.ToFile(m_pRes->m_sBpcParaFileName.c_str()))
				{
					m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"���������ļ�[%s]ʧ��",m_pRes->m_sBpcParaFileName.c_str());
				}
			}
		}
		else
		{
			m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"bpu��[%s]��BPC�����в�����",bpugroupname.c_str());
		}
		bpugnode = (CXmlNode *)bpugnode->getNextSibling();
	}
	sprintf(rcvdata.sMsgBuf->sBuffer,"���ò����ɹ�");
	rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
	rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
	rcvdata.sMsgBuf->sDBHead.cZip = 0;
	m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
	return ;
}

void CLinkThread::MonitorSetMode(S_BPC_RSMSG &rcvdata)
{
	if (rcvdata.sMsgBuf->sDBHead.cZip != 0)
	{
		if (!UnzipBuf(*rcvdata.sMsgBuf))
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"���ݽ�ѹ������");
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
			return ;
		}
	}
	CBF_Xml  xmlpack;
	if (!xmlpack.FromBuffer(rcvdata.sMsgBuf->sBuffer))
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"�������ݷ�xml");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
		return ;
	}
	CXmlNode *setnode = xmlpack.GetNodeByPath("Monitor/SetBpu",false);
	if (setnode == NULL)
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"xml��[Monitor/SetBpu]�ڵ�");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
		return ;
	}
	int savemode=0;
	if (xmlpack.GetNodeAttribute("Monitor/SetMonitor",false,"mode",false,savemode) == NULL)
	{
		savemode =0;//������Ĭ��Ϊ0
	}
	std::string bpugroupname;
	int i;
	
	char tmpchar[200];
	char tmpcharconf[200];
	bool isfind=false;
	std::string namevalue;
	if (savemode == 1)
	{
		CBF_Date_Time dt;
		sprintf(tmpchar,"%s.%s%s",m_pRes->m_sBpcParaFileName.c_str(),dt.ToStringD8().c_str(),dt.ToStringT6().c_str());
		m_pRes->m_xmlconf.ToFile(tmpchar);
	}
// 	int   g_nUseMonitor;       //�Ƿ���
// 	int   g_nMonitorHost;      //�Ƿ񱨸�������Դ
// 	int   g_nMonitorDrebId;            //�����ڵ��
// 	int   g_nMonitorDrebPrivateId;     //˽�нڵ��
// 	int   g_nMonitorSvrId;            //��������ڵ��
// 	int   g_nMonitorSvrPrivateId;     //˽�з���ڵ��
// 	int   g_nMonitorTxCode;           //��ؽ�����
	if (xmlpack.GetNodeAttribute("Monitor/SetMonitor/�������",false,"use",false,namevalue) != NULL)
	{
		if (savemode == 1) //���������ļ�
		{
			sprintf(tmpcharconf,"package/head/�������");
			m_pRes->m_xmlconf.SetNodeAttribute(tmpcharconf,false,"use",false,namevalue.c_str(),false);
		}
		//�����ڴ�
		m_pRes->g_nUseMonitor = atoi(namevalue.c_str());
	}
	if (xmlpack.GetNodeAttribute("Monitor/SetMonitor/�������",false,"host",false,namevalue) != NULL)
	{
		if (savemode == 1) //���������ļ�
		{
			sprintf(tmpcharconf,"package/head/�������");
			m_pRes->m_xmlconf.SetNodeAttribute(tmpcharconf,false,"host",false,namevalue.c_str(),false);
		}
		//�����ڴ�
		m_pRes->g_nMonitorHost = atoi(namevalue.c_str());
	}
	if (xmlpack.GetNodeValueByPath("Monitor/SetMonitor/��ع����ڵ��",false,namevalue) != NULL)
	{
		if (savemode == 1) //���������ļ�
		{
			sprintf(tmpcharconf,"package/head/�������/��ع����ڵ��");
			m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str(),false);
		}
		//�����ڴ�
		m_pRes->g_nMonitorDrebId = atoi(namevalue.c_str());
	}
	if (xmlpack.GetNodeValueByPath("Monitor/SetMonitor/���˽�нڵ��",false,namevalue) != NULL)
	{
		if (savemode == 1) //���������ļ�
		{
			sprintf(tmpcharconf,"package/head/�������/���˽�нڵ��");
			m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str(),false);
		}
		//�����ڴ�
		m_pRes->g_nMonitorDrebPrivateId = atoi(namevalue.c_str());
	}
	if (xmlpack.GetNodeValueByPath("Monitor/SetMonitor/��ع��������",false,namevalue) != NULL)
	{
		if (savemode == 1) //���������ļ�
		{
			sprintf(tmpcharconf,"package/head/�������/��ع��������");
			m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str(),false);
		}
		//�����ڴ�
		m_pRes->g_nMonitorSvrId = atoi(namevalue.c_str());
	}
	if (xmlpack.GetNodeValueByPath("Monitor/SetMonitor/���˽�з����",false,namevalue) != NULL)
	{
		if (savemode == 1) //���������ļ�
		{
			sprintf(tmpcharconf,"package/head/�������/���˽�з����");
			m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str(),false);
		}
		//�����ڴ�
		m_pRes->g_nMonitorSvrPrivateId = atoi(namevalue.c_str());
	}
	if (xmlpack.GetNodeValueByPath("Monitor/SetMonitor/��ر��潻����",false,namevalue) != NULL)
	{
		if (savemode == 1) //���������ļ�
		{
			sprintf(tmpcharconf,"package/head/�������/��ر��潻����");
			m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str(),false);
		}
		//�����ڴ�
		m_pRes->g_nMonitorTxCode = atoi(namevalue.c_str());
	}
	if (savemode == 1)
	{
		if (!m_pRes->m_xmlconf.ToFile(m_pRes->m_sBpcParaFileName.c_str()))
		{
			m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"���������ļ�[%s]ʧ��",m_pRes->m_sBpcParaFileName.c_str());
			sprintf(rcvdata.sMsgBuf->sBuffer,"���������ļ�[%s]ʧ��",m_pRes->m_sBpcParaFileName.c_str());
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
			return;
		}
	}
	sprintf(rcvdata.sMsgBuf->sBuffer,"���ò����ɹ�");
	rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
	rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
	rcvdata.sMsgBuf->sDBHead.cZip = 0;
	m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
	return;
}

void CLinkThread::MonitorStopBpu(S_BPC_RSMSG &rcvdata)
{
	if (rcvdata.sMsgBuf->sDBHead.cZip != 0)
	{
		if (!UnzipBuf(*rcvdata.sMsgBuf))
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"���ݽ�ѹ������");
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
			return ;
		}
	}
	CBF_Xml  xmlpack;
	if (!xmlpack.FromBuffer(rcvdata.sMsgBuf->sBuffer))
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"�������ݷ�xml");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
		return ;
	}
	CXmlNode *stopnode = xmlpack.GetNodeByPath("Monitor/StopBpu",false);
	if (stopnode == NULL)
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"xml��[Monitor/StopBpu]�ڵ�");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
		return ;
	}
	int i;
	int bpuindex=-1;
	std::string bpugroupname;
	int bpugnum=0;
	S_PROCESS_NODE node;
	bool isfind = false;
	char errmsg[200];
	CXmlNode *bpunode = (CXmlNode *)stopnode->GetFirstChild();
	while (bpunode != NULL)
	{
		bpugroupname = bpunode->GetNodeName();
		isfind=false;
		for (i=0; i< m_pRes->g_vBpuGroupInfo.size() ; i++)
		{
			if (bpugroupname.compare(m_pRes->g_vBpuGroupInfo[i].g_sBpuGroupName) == 0)
			{
				isfind = true;
				bpuindex = i;
				break;
			}
		}
		if (isfind)
		{
			node.prgname = m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuPrgName;
			node.prgpara = m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuPrgPara;
			node.prgpath = m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuPrgPath;
			node.runpath = m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuRunPath;
			node.nodeid = m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName;
			
			if (m_pRes->g_vBpuGroupInfo[bpuindex].g_nDispatchMode == 1) //java�汾
			{
				m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"����ԭBPU[%s]�����Ƿ����[%d]",\
					m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(),m_pRes->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid);
				if (m_pRes->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid > 0) //�鿴�����Ƿ����
				{
					node.pid = m_pRes->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid;
					if (m_pMgrBpu.FindProcess(&node,errmsg) == 0) //�д˽���
					{
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"ԭBPU[%s]����[%d]�Ѵ��ڣ�ֹͣ",\
							m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(),m_pRes->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid);
						if (m_pMgrBpu.StopProcess(&node,errmsg)!=0)
						{
							m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"BPU[%s]����[%d]ֹͣʧ��",\
								m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(),m_pRes->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid);
						}
					}
				}
			}
			else
			{
				for (int j=m_pRes->g_vBpuGroupInfo[bpuindex].g_nBeginSocketIndex ; j<m_pRes->g_vBpuGroupInfo[bpuindex].g_nEndSocketIndex ;j++)
				{
					if (m_pSockMgr->at(j)->m_sock != INVALID_SOCKET)
					{
						node.pid = m_pSockMgr->at(j)->m_nPid;
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"ֹͣBPU[%s]����[%d]",\
							m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(),node.pid);
						if (m_pMgrBpu.StopProcess(&node,errmsg)!=0)
						{
							m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"BPU[%s]����[%d]ֹͣʧ��",\
								m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(),node.pid);
						}
					}
				}
			}
			
		}
		bpunode = (CXmlNode *)bpunode->getNextSibling();
	}
	sprintf(rcvdata.sMsgBuf->sBuffer,"ֹͣBPU�ɹ�");
	rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
	rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
	rcvdata.sMsgBuf->sDBHead.cZip = 0;
	m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
	return;
}

void CLinkThread::MonitorStartBpu(S_BPC_RSMSG &rcvdata)
{
	if (rcvdata.sMsgBuf->sDBHead.cZip != 0)
	{
		if (!UnzipBuf(*rcvdata.sMsgBuf))
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"���ݽ�ѹ������");
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
			return ;
		}
	}
	CBF_Xml  xmlpack;
	if (!xmlpack.FromBuffer(rcvdata.sMsgBuf->sBuffer))
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"�������ݷ�xml");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
		return ;
	}
	CXmlNode *stopnode = xmlpack.GetNodeByPath("Monitor/StartBpu",false);
	if (stopnode == NULL)
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"xml��[Monitor/StartBpu]�ڵ�");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
		return ;
	}
	int i;
	int bpuindex=-1;
	std::string bpugroupname;
	int bpugnum=0;
	S_PROCESS_NODE node;
	bool isfind = false;
	int k;
	int connNum=0;
	char errmsg[200];
	CXmlNode *bpunode = (CXmlNode *)stopnode->GetFirstChild();
	while (bpunode != NULL)
	{
		bpugroupname = bpunode->GetNodeName();
		isfind=false;
		for (i=0; i< m_pRes->g_vBpuGroupInfo.size() ; i++)
		{
			if (bpugroupname.compare(m_pRes->g_vBpuGroupInfo[i].g_sBpuGroupName) == 0)
			{
				isfind = true;
				bpuindex = i;
				break;
			}
		}
		if (isfind)
		{
			node.prgname = m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuPrgName;
			node.prgpara = m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuPrgPara;
			node.prgpath = m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuPrgPath;
			node.runpath = m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuRunPath;
			node.nodeid = m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName;

			connNum=0;
			for (k=m_pRes->g_vBpuGroupInfo[bpuindex].g_nBeginSocketIndex ; k < m_pRes->g_vBpuGroupInfo[bpuindex].g_nEndSocketIndex; k++)
			{
				if (m_pSockMgr->at(k)->m_sock != INVALID_SOCKET)
				{
					connNum++;
				}
			}
			if (!m_pRes->g_vBpuGroupInfo[bpuindex].g_bIsAutoLoadBu) //�����Զ������Ž�������
			{
				if (m_pRes->g_vBpuGroupInfo[bpuindex].g_nDispatchMode == 1) //java�汾
				{
//					bpugnum = m_pRes->g_vBpuGroupInfo[bpuindex].g_nBpuNum - m_pRes->g_vBpuGroupInfo[bpuindex].g_nStartBpuNum;
					bpugnum = m_pRes->g_vBpuGroupInfo[bpuindex].g_nBpuNum - connNum;
					if (bpugnum >0)
					{
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"����ԭBPU[%s]�����Ƿ����[%d]",\
							m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(),m_pRes->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid);
						if (m_pRes->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid > 0) //�鿴�����Ƿ����
						{
							node.pid = m_pRes->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid;
							if (m_pMgrBpu.FindProcess(&node,errmsg) == 0) //�д˽���
							{
								m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"ԭBPU[%s]����[%d]�Ѵ��ڣ�ֹͣ",\
									m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(),m_pRes->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid);
								if (m_pMgrBpu.StopProcess(&node,errmsg)!=0)
								{
									m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"BPU[%s]����[%d]ֹͣʧ��",\
										m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(),m_pRes->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid);
								}
							}
							m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"BPU[%s]����",\
								m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str());
							if (m_pMgrBpu.StopProcess(&node,errmsg)!=0)
							{
								m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"BPU[%s]����[%d]ֹͣʧ��",\
									m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(),m_pRes->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid);
							}
						}
					}
				}
				else
				{
//					bpugnum = m_pRes->g_vBpuGroupInfo[bpuindex].g_nBpuNum - m_pRes->g_vBpuGroupInfo[bpuindex].g_nStartBpuNum;
					bpugnum = m_pRes->g_vBpuGroupInfo[bpuindex].g_nBpuNum - connNum;
					for (int j=0; j<bpugnum;j++)
					{
						if (m_pMgrBpu.StartProcess(&node,errmsg)!=0)
						{
							m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"BPU[%s]����ʧ��",\
								m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str());
						}
					}
				}
			}
		}
		bpunode = (CXmlNode *)bpunode->getNextSibling();
	}
	sprintf(rcvdata.sMsgBuf->sBuffer,"����BPU�ɹ�");
	rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
	rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
	rcvdata.sMsgBuf->sDBHead.cZip = 0;
	m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
	return;
}
void CLinkThread::OnBpuIsReg(int index,S_BPC_RSMSG &rcvdata)
{
	bool bRes=false;
	
	if (rcvdata.sMsgBuf->sDBHead.nLen <1)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"BPUע��ʱ��bpu������ index=%d",index);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPC_NOGROUP;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		rcvdata.sMsgBuf->sDBHead.nLen = 0;
		rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
		m_pSockMgr->at(index)->SendMsg(rcvdata);
		OnClose(index,"BPUע��ʱ��bpu������",__FILE__,__LINE__);
		return;
	}
	rcvdata.sMsgBuf->sBuffer[rcvdata.sMsgBuf->sDBHead.nLen]=0;
	bool isfind=false;
	int  bpugroupindex=0;
	for (int i=0;i<m_pRes->g_vBpuGroupInfo.size();i++)
	{
		if (strcmp(m_pRes->g_vBpuGroupInfo[i].g_sBpuGroupName.c_str(),rcvdata.sMsgBuf->sBuffer) == 0) //�ҵ���BPU��
		{
			bpugroupindex = i;
			isfind = true;
			break;
		}
	}
	int connbpu=0;
	int connNum=0;
	int k;
	if (isfind) //�д�bpu��
	{
		//�������д�BPU���������
		connNum=0;
		for (k=m_pRes->g_vBpuGroupInfo[bpugroupindex].g_nBeginSocketIndex ; k < m_pRes->g_vBpuGroupInfo[bpugroupindex].g_nEndSocketIndex; k++)
		{
			if (m_pSockMgr->at(k)->m_sock != INVALID_SOCKET)
			{
				connNum++;
			}
		}
		if (connNum >= m_pRes->g_vBpuGroupInfo[bpugroupindex].g_nBpuNum)
		{
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.nLen = 0;
			rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPC_BPULIMIT;
			m_pSockMgr->at(index)->SendMsg(rcvdata);
			m_pSockMgr->at(index)->OnClose("����BPU��������",__FILE__,__LINE__);
			return ;
		}
		bool isfree = false;
		int bpuindex;
		//����һ�����е�BPU���ӣ�Ȼ�����
		for (int j=m_pRes->g_vBpuGroupInfo[bpugroupindex].g_nBeginSocketIndex ; j<m_pRes->g_vBpuGroupInfo[bpugroupindex].g_nEndSocketIndex ; j++)
		{
			if (m_pSockMgr->at(j)->m_sock == INVALID_SOCKET)
			{
				isfree = true;
				bpuindex = j;
				break;
			}
		}
		if (!isfree)
		{
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.nLen = 0;
			rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPC_BPULIMIT;
			m_pSockMgr->at(index)->SendMsg(rcvdata);
			m_pSockMgr->at(index)->OnClose("����BPU��������",__FILE__,__LINE__);
			return ;
		}
		//�п����ж��յ�����
		if (m_pSockMgr->at(index)->m_nRcvBufLen>0)
		{
			memcpy(m_pSockMgr->at(bpuindex)->m_sRcvBuffer,m_pSockMgr->at(index)->m_sRcvBuffer,m_pSockMgr->at(index)->m_nRcvBufLen);
			m_pSockMgr->at(bpuindex)->m_nRcvBufLen = m_pSockMgr->at(index)->m_nRcvBufLen;
		}
		
		m_pSockMgr->at(bpuindex)->SetAccept(m_pSockMgr->at(index)->m_sock,m_pSockMgr->at(index)->m_sAddress);

		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"����ip[%s] index[%d] socket[%d]תΪBPU����[%d] BPU��[%s]",\
			m_pSockMgr->at(index)->m_sAddress,index,m_pSockMgr->at(index)->m_sock,bpuindex,rcvdata.sMsgBuf->sBuffer);
		//�õ�ǰ�������Ϊ���У�accept���Լӷ���
		m_pSockMgr->at(index)->m_nRcvBufLen=0;
		m_pSockMgr->at(index)->m_sock = INVALID_SOCKET;


		//�����������������ŵ��������в���������BPU��+1
		
		m_pSockMgr->at(bpuindex)->m_sBpuGroupName = rcvdata.sMsgBuf->sBuffer;
		m_pSockMgr->at(bpuindex)->m_nBpuGroupIndex = bpugroupindex;
		m_pRes->g_vBpuGroupInfo[bpugroupindex].g_nStartBpuNum++;
		//��bpu�Ľ���ID�ŵ���������
		m_pSockMgr->at(bpuindex)->m_nPid = rcvdata.sMsgBuf->sBpcHead.nRtime;
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���ӹ�����BPU��[%s] bpuindex[%d] BPU��index[%d] BPU������[%d]",\
			m_pSockMgr->at(bpuindex)->m_sBpuGroupName.c_str(),bpuindex,m_pSockMgr->at(bpuindex)->m_nBpuGroupIndex,\
			m_pRes->g_vBpuGroupInfo[bpugroupindex].g_nStartBpuNum);
		
		m_pSockMgr->at(bpuindex)->SetBpuFreeFlag(false);
//		m_pSockMgr->at(bpuindex)->m_bIsBuGetFlag = false;

//		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"BPU index[%d] ״̬Ϊæע��",bpuindex);
		//BPC�����Ƿ�Ҫע��
		if (m_pRes->g_nRegisterMode == REGISTERMODE_NONE || m_pRes->g_nRegisterMode == REGISTERMODE_STATIC)
		{
			m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"BPC��������BPUע��");
			m_pSockMgr->at(bpuindex)->m_bChecked = true;//������Ϊ�Ѽ��
			rcvdata.sMsgBuf->sDBHead.nLen = 0;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPC_NOREGISTER;
			m_pSockMgr->at(bpuindex)->SendMsg(rcvdata);
			return;
		}
		else //�����Ƿ�Ҫע��
		{
			bRes = m_pRes->g_vBpuGroupInfo[bpugroupindex].g_bIsReg;
			if (bRes)
			{
				m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"BPU֮ǰ�Ѿ�ע���");
			}
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			sprintf(rcvdata.sMsgBuf->sBuffer,"%d",bRes);  //1�Ѿ�ע���,0δע�� ��Ҫע��
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN + rcvdata.sMsgBuf->sDBHead.nLen;
			m_pSockMgr->at(bpuindex)->SendMsg(rcvdata);
		}
	}
	else //�޴�bpu��
	{
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.nLen = 0;
		rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPC_NOGROUP;
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"BPUע��ʱ��bpu������ %s",rcvdata.sMsgBuf->sBuffer);
		m_pSockMgr->at(index)->SendMsg(rcvdata);
		m_pSockMgr->at(index)->OnClose("BPUע��ʱ��bpu������",__FILE__,__LINE__);
	}
}
void CLinkThread::StartTimer()
{
	m_pTimer.Start();
}

void CLinkThread::StopTimer()
{
	m_pTimer.Stop();
}
int CLinkThread::OnTimer(unsigned int eventid, void *p)
{
	CLinkThread *pp = (CLinkThread *)p;
	if (eventid == 0)
	{
		pp->PingTimer();
	}
	return 0;
}
void CLinkThread::PingTimer()
{
	//OnNoEvent();
	m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"��ʱ��");
	m_bIsPingTimer = true;
}

void CLinkThread::OnMonitor()
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
	m_pMemPool->PoolInfo(total,used);
	pXml.SetNodeAttribute("Monitor/�ڴ滺���",false,"�ܷ���",false,total );
	pXml.SetNodeAttribute("Monitor/�ڴ滺���",false,"δʹ��",false,used );
	pXml.SetNodeAttribute("Monitor/�ڴ滺���",false,"ÿ���С",false,(int)BPCBUFSIZE );
	

	if (m_pRes->g_nMonitorHost == 1)
	{
		S_MONITOR_HOST hostinfo;
		V_MonitorDiskInfo  diskinfo;
		g_pBpcTime.GetHostInfo(hostinfo,diskinfo);
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
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"AddNodeByPath ����");
			continue;
		}
		connnode->SetAttribute("ip",false,m_pRes->g_vDrebLinkInfo[i].sIp);
		connnode->SetAttribute("port",false,(int)m_pRes->g_vDrebLinkInfo[i].nPort);
		connnode->SetAttribute("index",false,i+2);
		connnode->SetAttribute("�����ڵ�",false,m_pSockMgr->at(i+2)->m_nDrebId);
		connnode->SetAttribute("˽�нڵ�",false,m_pSockMgr->at(i+2)->m_nDrebPrivateId);
		if (m_pSockMgr->at(i+2)->m_sock == INVALID_SOCKET)
		{
			connnode->SetAttribute("status",false,"δ����");
		}
		else if (m_pSockMgr->at(i+2)->m_bChecked)
		{
			connnode->SetAttribute("status",false,"����");
		}
		else
		{
			connnode->SetAttribute("status",false,"��������");
		}
	}
	int connNum=0;
	//BPC��Ϣ
	pXml.AddNodeByPath("Monitor/BPU��Ϣ",false,"BPULIST",false,"");
	for (i=0 ; i<m_pRes->g_vBpuGroupInfo.size(); i++)
	{
		connNum=0;
		for (k=m_pRes->g_vBpuGroupInfo[i].g_nBeginSocketIndex ; k < m_pRes->g_vBpuGroupInfo[i].g_nEndSocketIndex; k++)
		{
			if (m_pSockMgr->at(k)->m_sock != INVALID_SOCKET)
			{
				connNum++;
			}
		}
		CXmlNode *bpunode = pXml.AddNodeByPath("Monitor/BPU��Ϣ/BPULIST",false,m_pRes->g_vBpuGroupInfo[i].g_sBpuGroupName.c_str(),true,"");
		if (bpunode != NULL)
		{
			bpunode->SetAttribute("����",false,m_pRes->g_vBpuGroupInfo[i].g_sBpuGroupName);
			bpunode->SetAttribute("��Ŀ",false,m_pRes->g_vBpuGroupInfo[i].g_nBpuNum);
			bpunode->SetAttribute("ʵ����",false,connNum);
			bpunode->SetAttribute("����Ӧ��",false,m_pRes->g_vBpuGroupInfo[i].g_bIsAnsPing);
			bpunode->SetAttribute("����ģʽ",false,m_pRes->g_vBpuGroupInfo[i].g_nDispatchMode);
			bpunode->SetAttribute("�Զ�����",false,m_pRes->g_vBpuGroupInfo[i].g_bIsAutoLoadBu);
			bpunode->SetAttribute("������",false,m_pRes->g_vBpuGroupInfo[i].g_sBuPrgName);
			bpunode->SetAttribute("����Ŀ¼",false,m_pRes->g_vBpuGroupInfo[i].g_sBuPrgPath);
			bpunode->SetAttribute("����Ŀ¼",false,m_pRes->g_vBpuGroupInfo[i].g_sBuRunPath);
			bpunode->SetAttribute("����",false,m_pRes->g_vBpuGroupInfo[i].g_sBuPrgPara);
		}
	}

	S_BPC_RSMSG  mdata;//�������ݽṹ
//	bzero(&mdata,sizeof(S_BPC_RSMSG));
	mdata.sMsgBuf = (PBPCCOMMSTRU )m_pMemPool->PoolMalloc();
	if (mdata.sMsgBuf == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"������Ϣ�ռ����");
		return ;
	}
	int len = DREBDATASIZE;
	if (!pXml.ToBuffer(mdata.sMsgBuf->sBuffer,len))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ToBuffer����");
		m_pMemPool->PoolFree(mdata.sMsgBuf);
		return;
	}
	mdata.sMsgBuf->sDBHead.nLen = len;
	
	m_pLog->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"���֪ͨ",mdata.sMsgBuf->sBuffer,mdata.sMsgBuf->sDBHead.nLen);
	
	
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
	mdata.nRtime = time(NULL);
	S_FUNCINFO_TBL fn;
	int flag = 0;
	fn = m_pFuncTbl->FindFunc(mdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,flag);
	if (flag == 1) 
	{
		fn.nCallNum++;
		mdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN+mdata.sMsgBuf->sDBHead.nLen;
		strcpy(mdata.sBpuGroupName,fn.sBpuGroupName);//�� BPC����ļ�ط���
		//20151102 ֱ�ӷŵ�BPU��Ӧ���̶߳���
		if (m_pRes->PushData(fn.nBpuGroupIndex,mdata,fn.cPrio) != 0)
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"��BPU����г��� ��Ϣ[%s]  DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Դ[%d %d] cZip[%d] ҵ�����ݳ���[%d]",\
				m_pRes->GetBpcMsgType(mdata.sMsgBuf->sBpcHead.cMsgType).c_str(),\
				m_pRes->GetDrebCmdType(mdata.sMsgBuf->sDBHead.cCmd).c_str(),mdata.sMsgBuf->sDBHead.cNextFlag,\
				mdata.sMsgBuf->sDBHead.cRaflag,mdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,mdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
				mdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,mdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
				mdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,mdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
				mdata.sMsgBuf->sDBHead.cZip,mdata.sMsgBuf->sDBHead.nLen);
			m_pMemPool->PoolFree(mdata.sMsgBuf);
			mdata.sMsgBuf = NULL;
		}
	}
	else
	{
		bzero(mdata.sBpuGroupName,sizeof(mdata.sBpuGroupName));
		m_pPoolData->PushData(mdata);
	}
	
	return;
}

bool CLinkThread::UnzipBuf(BPCCOMMSTRU &data)
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
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
					return false;
				}
				data.sDBHead.nLen = ddlen;
				data.sDBHead.cZip =0;
				break;
			case 2:	
				memcpy(zipDataBuf,data.sBuffer,data.sDBHead.nLen);
				if (!CBF_Tools::Uncompress((unsigned char *)(data.sBuffer),dlen,(unsigned char *)zipDataBuf,data.sDBHead.nLen))
				{
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"��ѹ������");
					return false;
				}
				data.sDBHead.nLen = dlen;
				data.sDBHead.cZip =0;
				break;
			case 3:
				ddlen = data.sDBHead.nLen;
				if (m_pDes.UncryptString(data.sBuffer,ddlen,DESENCRYPTKEY3)<=0)
				{
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
					return false;
				}
				data.sDBHead.nLen = ddlen;
				data.sDBHead.cZip =0;
				break;
			case 4:
				ddlen = data.sDBHead.nLen;
				m_pLog->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"����ǰ����",data.sBuffer,data.sDBHead.nLen);
				if (m_pDes.UncryptString(data.sBuffer,ddlen,DESENCRYPTKEY)<=0)
				{
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
					return false;
				}
				data.sDBHead.nLen = ddlen;
				m_pLog->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"���ܺ�����",data.sBuffer,data.sDBHead.nLen);
				memcpy(zipDataBuf,data.sBuffer,data.sDBHead.nLen);
				if (!CBF_Tools::Uncompress((unsigned char *)(data.sBuffer),dlen,(unsigned char *)zipDataBuf,data.sDBHead.nLen))
				{
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"��ѹ������");
					return false;
				}
				data.sDBHead.nLen = dlen;
				m_pLog->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"��ѹ������",data.sBuffer,data.sDBHead.nLen);
				data.sDBHead.cZip =0;
				break;
			default:
				return false;
		}
	}
	
	return true;
}

void CLinkThread::Download(S_BPC_RSMSG &rcvdata)
{
	if (rcvdata.sMsgBuf->sDBHead.cZip != 0)
	{
		if (!UnzipBuf(*rcvdata.sMsgBuf))
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"���ݽ�ѹ������");
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
			return ;
		}
	}
	S_BPC_DUFILE sfile;
	if (rcvdata.sMsgBuf->sDBHead.nLen  < sizeof(S_BPC_DUFILE))
	{
		m_pLog->LogBin(LOG_ERROR,__FILE__,__LINE__,"�������ݷ���������",\
			rcvdata.sMsgBuf->sBuffer,rcvdata.sMsgBuf->sDBHead.nLen);
		sprintf(rcvdata.sMsgBuf->sBuffer,"�������ݷ���������");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_MONITOR_DOWNLOAD;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
		return ;
	}
	memcpy(&sfile,rcvdata.sMsgBuf->sBuffer,sizeof(S_BPC_DUFILE));
	struct_stat fstat;
	FSTAT(sfile.sFileName,&fstat);
	if (fstat.st_size <= 0)
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"�ļ�[%s]������",sfile.sFileName);
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_MONITOR_DOWNLOAD;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
		return ;
	}
	int offset = atol(sfile.sFileOffset);
	if (offset >= fstat.st_size)
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"�ļ�ƫ����[%d]�Ѵ��ڵ����ļ���С",offset);
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_MONITOR_DOWNLOAD;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
		return ;
	}
	sprintf(sfile.sFileSize,"%011ld",fstat.st_size);
	FILE *fp = fopen(sfile.sFileName,"rb");
	if (fp == NULL)
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"�ļ�[%s]������",sfile.sFileName);
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_MONITOR_DOWNLOAD;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
		return ;
	}
	fseek(fp,offset,1);
	int getlen = fread(rcvdata.sMsgBuf->sBuffer+sizeof(S_BPC_DUFILE),1,50*1024,fp);
	fclose(fp);
	sprintf(sfile.sFileOffset,"%011ld",getlen + offset);
	if ((getlen + offset) >= fstat.st_size)
	{
		sfile.cEndFlag = '1';
	}
	else
	{
		sfile.cEndFlag = '0';
	}
	memcpy(rcvdata.sMsgBuf->sBuffer,&sfile,sizeof(S_BPC_DUFILE));
	rcvdata.sMsgBuf->sDBHead.nLen = sizeof(S_BPC_DUFILE)+getlen;
	rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
	rcvdata.sMsgBuf->sDBHead.cZip = 0;
	m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);

}

void CLinkThread::Upload(S_BPC_RSMSG &rcvdata)
{
	if (rcvdata.sMsgBuf->sDBHead.cZip != 0)
	{
		if (!UnzipBuf(*rcvdata.sMsgBuf))
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"���ݽ�ѹ������");
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
			return ;
		}
	}
	S_BPC_DUFILE sfile;
	if (rcvdata.sMsgBuf->sDBHead.nLen  <= sizeof(S_BPC_DUFILE))
	{
		m_pLog->LogBin(LOG_ERROR,__FILE__,__LINE__,"�������ݷ��ϴ�����",\
			rcvdata.sMsgBuf->sBuffer,rcvdata.sMsgBuf->sDBHead.nLen);
		sprintf(rcvdata.sMsgBuf->sBuffer,"�������ݷ��ϴ�����");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_MONITOR_UPLOAD;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
		return ;
	}
	memcpy(&sfile,rcvdata.sMsgBuf->sBuffer,sizeof(S_BPC_DUFILE));
	m_pLog->LogBin(LOG_DEBUG,__FILE__,__LINE__,"�ϴ��ļ�",rcvdata.sMsgBuf->sBuffer,rcvdata.sMsgBuf->sDBHead.nLen );
	FILE *fp=NULL;
	if (sfile.cBeginFlag == '1')
	{
		char filenamepath[301];
		sprintf(filenamepath,"%s",sfile.sFileName);
		int j= strlen(filenamepath)-1;
		for (;j>0;j--)
		{
			if ((filenamepath[j] == '/') ||  (filenamepath[j] == '\\'))
			{
				filenamepath[j] =0;
				CBF_Tools::MakeDir(filenamepath);
				break;
			}
		}
		fp = fopen(sfile.sFileName,"wb");
	}
	else
	{
		fp = fopen(sfile.sFileName,"ab");
	}
	if (fp == NULL)
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"�ļ�[%s]�򿪳���",sfile.sFileName);
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_MONITOR_UPLOAD;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
		return ;
	}
	fwrite(rcvdata.sMsgBuf->sBuffer+sizeof(S_BPC_DUFILE),1,rcvdata.sMsgBuf->sDBHead.nLen-sizeof(S_BPC_DUFILE),fp);
	fclose(fp);
	if (sfile.cEndFlag == '1')
	{
		struct_stat fstat;
		FSTAT(sfile.sFileName,&fstat);
		if (atol(sfile.sFileSize) != fstat.st_size)
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"�ϴ��ļ�[%s]ʧ�ܣ���С[%d]������[%s]",sfile.sFileName,fstat.st_size,sfile.sFileSize);
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_MONITOR_UPLOAD;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
			return ;
		}
		sprintf(rcvdata.sMsgBuf->sBuffer,"�ϴ��ļ�[%s]�ɹ�",sfile.sFileName);
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
		return ;
	}
	else
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"д���ļ�[%s]�ɹ�",sfile.sFileName);
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
		return ;
	}
}
