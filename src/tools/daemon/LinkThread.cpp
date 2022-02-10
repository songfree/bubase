// LinkThread.cpp: implementation of the CLinkThread class.
//
//////////////////////////////////////////////////////////////////////

#include "LinkThread.h"
#include "dreberrcode.h"
#include "BpcHead.h"

#define  DESENCRYPTKEY    "rd402wms"
#define  DESENCRYPTKEY3   "wms13711songfree"
char g_VERSION[20]="1.0.2.20100724";   //�汾
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


#ifdef _WINDOWS
#define   INT64PFM  "%I64d"
#else
#define   INT64PFM  "%lld"
#endif


CLinkThread::CLinkThread()
{
	m_bIsRunning = false;
	m_pRes = NULL;
	
	m_bIsInit = false;
	m_sThreadName = "CLinkThread";
	m_bIsPingTimer = false;
	m_nDrebIndex = 0;
	bzero(m_sHeartBuffer,sizeof(m_sHeartBuffer));
}

CLinkThread::~CLinkThread()
{
#ifdef _WINDOWS
	m_pRes=NULL;
	WriteFile();	
#endif
}

std::string CLinkThread::GetDrebCmdType(int cmdtype)
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
bool CLinkThread::Terminate()
{
	m_bIsRunning = false;
	return true;
}

bool CLinkThread::SetGlobalVar(CResource *res,CSocketMgr *sockmgr)
{
	m_pRes = res;
	m_pSockMgr = sockmgr;
	m_pLog.SetLogPara(m_pRes->g_nLoglevel,m_pRes->g_sLogFilePath.c_str(),m_pRes->g_sLogFileName.c_str());
	m_bIsInit = true;

	m_pProcessMgr.SetLogPara(m_pRes->g_nLoglevel,m_pRes->g_sLogFilePath.c_str(),m_pRes->g_sLogFileName.c_str());
	
	m_pTimer.Init(100,true);
	//�ڵ�·�ɶ�ʱ��
	m_pTimer.SetTimer(0,5000,&CLinkThread::OnTimer,this); //���ö�ʱ�� 
	m_pTimer.SetTimer(1,5000,&CLinkThread::OnTimer,this); //���ö�ʱ��
	m_pTimer.SetTimer(2,300000,&CLinkThread::OnTimer,this); //д�ļ�

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
	m_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,"���������߳�");
	while(!m_pRes->g_bIsExit)
	{
		FD_ZERO(&m_rset);
		FD_ZERO(&m_wset);
#if defined(_WINDOWS)
		FD_ZERO(&m_eset);
#endif
		
		//ȡ������socket����socket���뵽fd_set����
		ret = GetMaxfdAddSelect(maxfd);
		if (maxfd  <=0)
		{
			for (int i=0; i<m_pRes->g_vDrebLinkInfo.size() ;i++)
			{
				OnConnect(i);
			}
			SLEEP(5);
			m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"��dreb���� %d",tv.tv_sec);
			continue;
		}
		tv.tv_sec = 1;
		tv.tv_usec = 0;
#if defined(_WINDOWS)
		result = select(maxfd+1, &m_rset, &m_wset, &m_eset, &tv);
#else
		result = select(maxfd+1, &m_rset, &m_wset, NULL, &tv);
#endif
		
		if (result != 0) //���¼�����,��������
		{
			OnEvent();
		}
		if (m_bIsPingTimer)
		{
			m_pLog.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���NOEVENT");
			OnNoEvent();
			OnMonitor();
			OnCheckProcess();
			m_bIsPingTimer = false;
		}

	}
	m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"ֹͣ�����߳�");
	return 0;
}
int CLinkThread::GetMaxfdAddSelect(int &maxfd)
{
	int ret=0;
	maxfd = 0;
	for (int i=0;i< m_pSockMgr->m_socketList.size() ;i++)
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
			if (m_pSockMgr->at(i)->NeedConnect() || m_pSockMgr->at(i)->NeedSend())
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

	for (j=0; j< m_pSockMgr->m_socketList.size(); j++)
	{
// 		if (m_pSockMgr->at(j)->m_sock == INVALID_SOCKET)
// 		{
// 			continue;
// 		}
		if (m_pSockMgr->at(j)->m_sock != INVALID_SOCKET && m_pSockMgr->at(j)->m_nType == MONI_SOCK_DREB)
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
		else if (m_pSockMgr->at(j)->m_sock != INVALID_SOCKET && m_pSockMgr->at(j)->m_nType != MONI_SOCK_LISTEN)
		{
			tmptime = time(NULL) - m_pSockMgr->at(j)->m_nReadtime;
			if (tmptime > m_pRes->g_nGwDisTime)
			{
				OnClose(j,"���ӳ�ʱ��δ�ã��Ͽ�",__FILE__,__LINE__);
			}
		}
		if (m_pRes->g_bIsExit)
		{
			return;
		}
		//�����������߽ڵ����ӣ���û��������ȥ����
		if (m_pSockMgr->at(j)->m_nType == MONI_SOCK_DREB )
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
	for (i= 0; i< m_pSockMgr->m_socketList.size() ; i++)
	{
		if (m_pSockMgr->at(i)->m_sock == INVALID_SOCKET)//δ����
		{
			if (m_pSockMgr->at(i)->m_nType == MONI_SOCK_DREB  ) //�����������߽ڵ����ӣ���ȥ����
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
		
		if (m_pSockMgr->at(conindex)->m_nType == MONI_SOCK_LISTEN)
		{
			OnAccept(conindex);
		}
		else
		{
			OnRecv(conindex);
		}
		
	}
	else //�������޶��¼�
	{
		if (m_pSockMgr->at(conindex)->m_sock != INVALID_SOCKET)
		{
			//û�м��ͨ��
			if (!m_pSockMgr->at(conindex)->m_bChecked)
			{
				//�������߽ڵ�����
				if (m_pSockMgr->at(conindex)->m_nType == MONI_SOCK_DREB) 
				{
					if (time(NULL)-m_pSockMgr->at(conindex)->m_nReadtime >= m_pRes->g_nHeartRun)
					{
						if (m_pSockMgr->at(conindex)->NeedConnect())
						{
							m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"�������ӵ�%s:%dδ�ɹ�",m_pSockMgr->at(conindex)->m_sDrebIp.c_str(),m_pSockMgr->at(conindex)->m_nDrebPort);
							closesocket(m_pSockMgr->at(conindex)->m_sock);
							m_pSockMgr->at(conindex)->m_sock = INVALID_SOCKET;
						}
					}
				}// end if (m_pSockMgr->at(conindex)->m_nType == BPCSOCK_TODREB) //�������߽ڵ�����
			}
		}// end if (m_pSockMgr->at(conindex)->m_sock != INVALID_SOCKET)
	}// end if (ret == 1) //�ж��¼�
}

void CLinkThread::OnWriteEvent(int conindex)
{
	int ret;
	if (m_pSockMgr->at(conindex)->m_sock == INVALID_SOCKET)
	{
		return;
	}
	if (m_pSockMgr->at(conindex)->m_nType == MONI_SOCK_LISTEN )
	{
		return;
	}
#if defined(_WINDOWS)
	ret = FD_ISSET(m_pSockMgr->at(conindex)->m_sock, &m_eset);
	if (ret != 0)
	{
		//��������������DREB
		if (m_pSockMgr->at(conindex)->m_nType == MONI_SOCK_DREB )
		{
			m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�������ӵ�%s:%dδ�ɹ�",m_pSockMgr->at(conindex)->m_sDrebIp.c_str(),m_pSockMgr->at(conindex)->m_nDrebPort);
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
			if (m_pSockMgr->at(conindex)->m_nType == MONI_SOCK_DREB ) 
			{
				OnConnected(conindex);//֪ͨ�����ѳɹ�
			}
		}
		else if (m_pSockMgr->at(conindex)->m_bChecked)
		{
			OnSend(conindex);//��������
		}
	}
	else  if (m_pSockMgr->at(conindex)->m_bChecked)//socket��д�¼�
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
				if (m_pSockMgr->at(conindex)->m_nType == MONI_SOCK_DREB)
				{
					OnConnected(conindex);//ȥ�����������߽ڵ�
				}
			}
			else
			{
				if (m_pSockMgr->at(conindex)->m_nType == MONI_SOCK_DREB )
				{
					m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"�������ӵ�%s:%dδ�ɹ�",m_pSockMgr->at(conindex)->m_sDrebIp.c_str(),m_pSockMgr->at(conindex)->m_nDrebPort);
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
	else if (m_pSockMgr->at(conindex)->m_bChecked)
	{
		OnSend(conindex);//��������
	}
#endif

				
}

void CLinkThread::OnPing(int conindex)
{
	if (m_pSockMgr->at(conindex)->m_nType == MONI_SOCK_DREB)
	{
		if (time(NULL) - m_pSockMgr->at(conindex)->m_nPingTime < BEATHEARTTICK) //С��5�벻�÷�ping
		{
			return ;
		}
		m_pSockMgr->at(conindex)->m_nPingTime = time(NULL);
		if (!ResetData(conindex,&m_pDataBuf))
		{
			return;
		}
		m_pDataBuf.data.head.cRaflag = 0;
		m_pDataBuf.data.head.cZip = 0;
		m_pDataBuf.data.head.nLen = 0;
		m_pDataBuf.data.head.cCmd = CMD_PING;
		//����ping���ݰ�
		m_pSockMgr->at(conindex)->Send(m_pDataBuf);
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
	//����
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
	m_pSockMgr->at(conindex)->m_nConntime = time(NULL);
	m_pSockMgr->at(conindex)->m_bNeedConnect = true;
	m_tcpSocket.Create(AF_INET,false);//�첽��ʽ
	if (m_pSockMgr->at(conindex)->m_nType == MONI_SOCK_DREB)
	{
		m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"�������ӵ��������߽ڵ�%s:%d:%d",m_pSockMgr->at(conindex)->m_sDrebIp.c_str(),m_pSockMgr->at(conindex)->m_nDrebPort,m_pSockMgr->at(conindex)->m_sock);
	}
	else
	{
		m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"�������ӵ�ת��Ŀ�� %s:%d:%d",m_pSockMgr->at(conindex)->m_sDrebIp.c_str(),m_pSockMgr->at(conindex)->m_nDrebPort,m_pSockMgr->at(conindex)->m_sock);
	}
	
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
			m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ȴ�������%s:%d������",m_pSockMgr->at(conindex)->m_sDrebIp.c_str(),m_pSockMgr->at(conindex)->m_nDrebPort);
			
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
		m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"������%s:%d�����ӳɹ�",m_pSockMgr->at(conindex)->m_sDrebIp.c_str(),m_pSockMgr->at(conindex)->m_nDrebPort);
		//���ӳɹ�����CMD_CONNECT��Ϣ��ȷ�����ӵ�����
		m_pSockMgr->at(conindex)->m_bNeedConnect = false;
		
		if (m_pSockMgr->at(conindex)->m_nType != MONI_SOCK_DREB)
		{
			m_pSockMgr->at(conindex)->m_bChecked = true;
			m_pSockMgr->at(conindex)->m_bNeedread = true;
			return;
		}
		//����������Ϣ
		m_pSockMgr->at(conindex)->m_nPingTime = time(NULL);
		if (!ResetData(conindex,&m_pDataBuf))
		{
			return;
		}
		
		m_pDataBuf.data.head.cRaflag = 0;
		m_pDataBuf.data.head.cZip = 0;
		m_pDataBuf.data.head.cCmd = CMD_CONNECT;
		m_pDataBuf.data.head.cDrebAffirm = 0;
	
		
		m_pDataBuf.data.head.nLen = sizeof(S_CONNECT_DATA);
		S_CONNECT_DATA *data = (S_CONNECT_DATA *)m_pDataBuf.data.buffer;
		data->cConnectType = 1;
		data->nSvrMainId = m_pRes->g_nSvrMainId;
		data->cSvrPrivateId = m_pRes->g_nSvrPrivateId;
		sprintf(data->sVersion,"%s",g_VERSION);
		m_pDrebEndian.Endian2Comm((unsigned char *)&(data->nSvrMainId),sizeof(data->nSvrMainId));
		
		if (m_pSockMgr->at(conindex)->Send(m_pDataBuf,true) >= 0)
		{
			m_pSockMgr->at(conindex)->m_bChecked = true;
		}
		return ;
	}
	return;
	
}

void CLinkThread::OnRecv(int conindex)
{
	int ret=0;
	int i=0;
	char tmpchar[200];
	if (m_pSockMgr->at(conindex)->m_nType == MONI_SOCK_LISTEN )
	{
		return;
	}
	ret = m_pSockMgr->at(conindex)->OnRecv();	
	if (ret<=0)
	{
		sprintf(tmpchar,"%s Զ�˹رգ��������ݲ����� ret=[%d]",m_pSockMgr->at(conindex)->m_sAddress,ret);
		OnClose(conindex,tmpchar,__FILE__,__LINE__);
		return;
	}
	if (m_pSockMgr->at(conindex)->m_nType != MONI_SOCK_DREB )
	{
		//������յ�����
		int heartLen=0;
		//�յ������ݴ��ڰ�ͷ
		while (m_pSockMgr->at(conindex)->m_nRcvBufLen >= HEARTHEAD )
		{
			bzero(tmpchar,sizeof(tmpchar));
			memcpy(tmpchar,m_pSockMgr->at(conindex)->m_sRcvBuffer,HEARTHEAD);
			CBF_Tools::LRtrim(tmpchar);
			heartLen = atoi(tmpchar);
			if (ret <=0 )
			{
				m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"����������[%s]",tmpchar);
				OnClose(conindex,"����������",__FILE__,__LINE__);
				return ;
			}

			if (m_pSockMgr->at(conindex)->m_nRcvBufLen < (heartLen+HEARTHEAD) )
			{
				//˵������û�н�������
				if (m_pSockMgr->at(conindex)->m_bIsAffirm)
				{
					m_pRes->UpdateLastTimeById(m_pSockMgr->at(conindex)->m_sNodeId);
				}
				//������
				return;
			}
			if (heartLen < 65534)
			{
				memcpy(m_sHeartBuffer,m_pSockMgr->at(conindex)->m_sRcvBuffer+HEARTHEAD,heartLen);
			}
			else
			{
				OnClose(conindex,"����������",__FILE__,__LINE__);
				return ;
			}
			//�ն��ˣ���ԭ�����¸�m_sRcvBuffer
			for (int i=0; i<m_pSockMgr->at(conindex)->m_nRcvBufLen-HEARTHEAD-heartLen; i++)
			{
				m_pSockMgr->at(conindex)->m_sRcvBuffer[i]=m_pSockMgr->at(conindex)->m_sRcvBuffer[i+HEARTHEAD+heartLen];
			}
			m_pSockMgr->at(conindex)->m_nRcvBufLen=m_pSockMgr->at(conindex)->m_nRcvBufLen - HEARTHEAD-heartLen;

			//����������
			m_sHeartBuffer[heartLen]=0;
			if (m_pSockMgr->at(conindex)->m_bIsAffirm) 
			{
				//֮ǰ�Ѿ��յ�������������ȷ�Ϲ�����ID,��ξͲ���ȥ����������
				m_pRes->UpdateLastTimeById(m_pSockMgr->at(conindex)->m_sNodeId);
				continue;
			}
			m_pLog.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"������[%s]",m_sHeartBuffer);
			bool bRet=false;
			if (m_sHeartBuffer[0] == '#')
			{
				bRet = m_pKvList.ParseNode(m_sHeartBuffer+1);
			}
			else
			{
				bRet = m_pKvList.ParseNode(m_sHeartBuffer);
			}
			if (!bRet)
			{
				m_pLog.LogBin(LOG_WARNNING,__FILE__,__LINE__,"�����������",m_sHeartBuffer,heartLen);
				OnClose(conindex,"��������ʽ������ӦΪ#�ָ���KV����",__FILE__,__LINE__);
				return ;
			}
			std::string snodeid;
			if (m_pKvList.GetValueByName("node_id",snodeid) == NULL)
			{
				m_pLog.LogBin(LOG_WARNNING,__FILE__,__LINE__,"�����������",m_sHeartBuffer,heartLen);
				OnClose(conindex,"��������ʽ������û��node_id�ֶ�",__FILE__,__LINE__);
				return;
			}
			if (m_pRes->UpdateLastTimeById(snodeid))
			{
				m_pSockMgr->at(conindex)->m_bIsAffirm = true;
				m_pSockMgr->at(conindex)->m_sNodeId = snodeid;
			}
			else
			{
				m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"node_id��ֵ[%s]�ڱ�������δ����",snodeid.c_str());
				OnClose(conindex,"node_id����",__FILE__,__LINE__);
				return;
			}
		
		}
		return;
	}
	//��ʼ���մ���
	if (m_pSockMgr->at(conindex)->m_nRcvBufLen >= DREBHEADLEN)
	{
		m_pLog.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"��ʼ���մ���");
	}
	while (m_pSockMgr->at(conindex)->m_nRcvBufLen >= DREBHEADLEN)
	{
		if (!ResetData(conindex,&m_pDataRcv))
		{
			return;
		}
		ret = m_pSockMgr->at(conindex)->GetRecvData(&m_pDataRcv);
		if (ret >0 ) //������
		{
			m_pDataRcv.nIndex = conindex;
			//����dreb����������
			if (m_pDataRcv.data.head.cCmd == CMD_PING || m_pDataRcv.data.head.cCmd == CMD_CONNECT) //��ӦPINGӦ��
			{
				if (m_pDataRcv.data.head.cRaflag == 0) //����
				{
					m_pDataRcv.data.head.cRaflag = 1;
					m_pSockMgr->at(conindex)->Send(m_pDataRcv);
				}
				else //Ӧ��ʲô��������
				{
					if ( m_pDataRcv.data.head.cCmd == CMD_CONNECT)
					{
						m_pSockMgr->at(conindex)->m_nDrebId = m_pDataRcv.data.head.a_Ainfo.a_nNodeId;
						m_pSockMgr->at(conindex)->m_nDrebPrivateId = m_pDataRcv.data.head.a_Ainfo.a_cNodePrivateId;
					}
				}
				continue; ;
			}
			m_pDataRcv.nIndex = conindex;
			ProcessDreb(m_pDataRcv);
			m_pLog.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"������һ������  �������ݳ���[%d]",m_pSockMgr->at(conindex)->m_nRcvBufLen);

		}
		else  //ret <=0�����δ��������
		{
			if (ret <0)
			{
				OnClose(conindex,"CRCУ���",__FILE__,__LINE__);
			}
			m_pLog.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"�˳����մ���  �������ݳ���[%d]",m_pSockMgr->at(conindex)->m_nRcvBufLen);
			break;
		}
	}
	
	return ;
}


bool CLinkThread::ResetData(int index,S_GWDREB_RSMSG *data)
{
	data->offset = 0;
	data->rtime = time(NULL);
	data->nIndex = index;
	return true;
}


void CLinkThread::ProcessDreb(S_GWDREB_RSMSG &rcvdata)
{
	if (rcvdata.data.head.cCmd == CMD_MONITOR_BPC)
	{
		if (rcvdata.data.head.cCmd != CMD_PING && rcvdata.data.head.cCmd != CMD_CONNECT && rcvdata.data.head.cCmd != CMD_REGSERVICE)
		{
			m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"��DREB[%d %d %d]�յ�����Ϣ DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Դ[%d %d] ҵ�����ݳ���[%d]",\
				rcvdata.data.head.s_Sinfo.s_nNodeId,rcvdata.data.head.s_Sinfo.s_cNodePrivateId,rcvdata.nIndex,\
				GetDrebCmdType(rcvdata.data.head.cCmd).c_str(),rcvdata.data.head.cNextFlag,\
				rcvdata.data.head.cRaflag,rcvdata.data.head.d_Dinfo.d_nServiceNo,rcvdata.data.head.s_Sinfo.s_nNodeId,\
				rcvdata.data.head.s_Sinfo.s_cNodePrivateId,rcvdata.data.head.s_Sinfo.s_nDrebSerial,\
				rcvdata.data.head.s_Sinfo.s_nSerial,rcvdata.data.head.s_Sinfo.s_nHook,rcvdata.data.head.nLen);
		}
		else
		{
			m_pLog.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���յ�DREB[%d]����Ϣ DREB����[%s]  ҵ�����ݳ���[%d]",\
				rcvdata.nIndex,GetDrebCmdType(rcvdata.data.head.cCmd).c_str(),rcvdata.data.head.nLen);
		}
		if (!UnzipBuf(&(rcvdata.data)))
		{
			m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
			return;
		}
		//m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"CMD_MONITOR_BPC [%d]",rcvdata.data.head.d_Dinfo.d_nServiceNo);
		CallBpcLocalFunc(rcvdata);
		return;
	}
	m_pLog.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"�������� %d",rcvdata.data.head.cCmd );
	return;
}


void CLinkThread::CallBpcLocalFunc(S_GWDREB_RSMSG &rcvdata)
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
	switch (rcvdata.data.head.d_Dinfo.d_nServiceNo)
	{
		case BPCMONI_INFO:
			MonitorInfo(rcvdata);
			break;
		case BPCMONI_HOST:
			MonitorHost(rcvdata);
			break;
			
		case BPCMONI_STOPBPU:
			MonitorStopBpu(rcvdata);
			break;
		case BPCMONI_STARTBPU:
			MonitorStartBpu(rcvdata);
			break;
		case BPCMONI_BPU:
			MonitorBpu(rcvdata);
			break;
		case BPCMONI_REPORT:
			MonitorReport(rcvdata);
			break;
		case BPCMONI_DOWNLOAD:
			Download(rcvdata);
			break;
		case BPCMONI_UPLOAD:
			Upload(rcvdata);
			break;
		default:
			rcvdata.data.head.cRaflag = 1;
			rcvdata.data.head.cZip = 0;
			rcvdata.data.head.a_Ainfo.a_nRetCode = ERR_FUNCNOTFUND;
			rcvdata.data.head.nLen = 0;
			m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
			break;
	}
	

}


bool CLinkThread::UnzipBuf(PCOMMSTRU data)
{
	//0���� 1des������Կ����16 2ѹ�� 3DES���� 4DES���ܲ�ѹ��
	if (data->head.nLen == 0)
	{
		return true;
	}
	if (data->head.cZip >0 && data->head.cZip <=4)
	{
		char zipDataBuf[DREBDATASIZE];
		bzero(zipDataBuf,sizeof(zipDataBuf));
		unsigned int dlen = DREBDATASIZE;
		int ddlen;
		switch (data->head.cZip)
		{
			case 1:
				ddlen = data->head.nLen;
				if (m_pDes.UncryptString(data->buffer,ddlen,DESENCRYPTKEY)<1)
				{
					m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
					return false;
				}
				data->head.nLen = ddlen;
				data->head.cZip =0;
				break;
			case 2:	
				break;
			case 3:
				ddlen = data->head.nLen ;
				if (m_pDes.UncryptString(data->buffer,ddlen,DESENCRYPTKEY3)<1)
				{
					m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
					return false;
				}
				data->head.nLen = ddlen;
				data->head.cZip =0;
				break;
			case 4:
				ddlen = data->head.nLen ;
				if (m_pDes.UncryptString(data->buffer,ddlen,DESENCRYPTKEY3)<1)
				{
					m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
					return false;
				}
				data->head.nLen = ddlen;
				data->head.cZip = 2;
				break;
			default:
				return false;
		}
	}
	
	return true;
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
		pp->GetHostInfo();
	}
	else if (eventid == 1)
	{
		pp->PingTimer();
	}
	else
	{
		pp->WriteFile();
	}
	return 0;
}
void CLinkThread::PingTimer()
{
	//OnNoEvent();
	m_pLog.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"��ض�ʱ��");
	m_bIsPingTimer = true;
}

void CLinkThread::OnMonitor()
{
	//�����Ϣ

	if (m_pRes->g_nUseMonitor != 1)
	{
		return;
	}
	int i;
	CBF_Xml        pXml;
	pXml.SetNodeValueByPath("Monitor/�����ڵ�",false,(int)m_pRes->g_nSvrMainId);
	pXml.SetNodeValueByPath("Monitor/˽�нڵ�",false,(int)m_pRes->g_nSvrPrivateId);
	pXml.SetNodeValueByPath("Monitor/�ڵ�����",false,2);//��bpc
	pXml.SetNodeValueByPath("Monitor/��������",false,0);//����
	pXml.SetNodeValueByPath("Monitor/����ʱ��",false,m_pRes->m_sStartDate,false);
	
	
	//ȡ������Ϣ
	if (m_pRes->g_nMonitorHost == 1)
	{
		pXml.SetNodeValueByPath("Monitor/������Դ/CPU",false,(int)m_sHostInfo.nCpuRate);
		pXml.SetNodeAttribute("Monitor/������Դ/�ڴ�",false,"total",false,(int)m_sHostInfo.nTotalMemory);
		pXml.SetNodeAttribute("Monitor/������Դ/�ڴ�",false,"used",false,(int)m_sHostInfo.nUsedMemory);
		for (i=0 ; i< m_vdinfo.size() ; i++)
		{
			CXmlNode *disknode = pXml.AddNodeByPath("Monitor/������Դ/����",false,"����",false,"");
			if (disknode != NULL)
			{
				disknode->SetAttribute("drive",false,m_vdinfo[i].cIndex,true);
				disknode->SetAttribute("total",false,(int)m_vdinfo[i].TotalSpace);
				disknode->SetAttribute("used",false,(int)m_vdinfo[i].UsedSpace);
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
		connnode->SetAttribute("index",false,i);
		connnode->SetAttribute("�����ڵ�",false,m_pSockMgr->at(i)->m_nDrebId);
		connnode->SetAttribute("˽�нڵ�",false,m_pSockMgr->at(i)->m_nDrebPrivateId);
		if (m_pSockMgr->at(i)->m_sock == INVALID_SOCKET)
		{
			connnode->SetAttribute("status",false,"δ����");
		}
		else if (m_pSockMgr->at(i)->m_bChecked)
		{
			connnode->SetAttribute("status",false,"����");
		}
		else
		{
			connnode->SetAttribute("status",false,"��������");
		}
	}
	std::string extdata;
	char tmpchar[1000];
	for (i=0 ; i< m_pRes->g_vProcessInfo.size(); i++)
	{
		sprintf(tmpchar,"��������:%s,����ID:%d,��ǰʱ��:%d,�������ʱ��:%d",\
			m_pRes->g_vProcessInfo[i].processname.c_str(),m_pRes->g_vProcessInfo[i].pid,\
			time(NULL),m_pRes->g_vProcessInfo[i].lastUseTIme);
		extdata = tmpchar;
		sprintf(tmpchar,"Monitor/��չ��Ϣ/%s",m_pRes->g_vProcessInfo[i].nodeid.c_str());
		pXml.SetNodeAttribute(tmpchar,false,"status",false,m_pRes->g_vProcessInfo[i].status.c_str(),false);
		pXml.SetNodeValueByPath(tmpchar,false,extdata.c_str(),false);
	}
// 	<��չ��Ϣ name="�����ݲ�������">
//         <DATA status="0">���ӳ����������22 ����������21 �ͻ�������0 �ڴ滺����ܷ���100 δʹ��100 ÿ���С65534</DATA>
//         <DATA2 status="0">���ӳ����������22 ����������21 �ͻ�������0 �ڴ滺����ܷ���100 δʹ��100 ÿ���С65534</DATA2>
//     </��չ��Ϣ>

	S_GWDREB_RSMSG drebdata;
	bzero(&drebdata,sizeof(S_GWDREB_RSMSG));
	
	drebdata.rtime = time(NULL);
	int len = DREBDATASIZE;
	if (!pXml.ToBuffer(drebdata.data.buffer,len))
	{
		m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"ToBuffer����");
		return;
	}
	m_pLog.LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"�����������",drebdata.data.buffer,len);
	
	//����ͷ
	drebdata.data.head.nLen = len;
	drebdata.data.head.cCmd = CMD_DPPUSH;
	drebdata.data.head.cZip = 0;
	drebdata.data.head.cDrebAffirm = 0;
	drebdata.data.head.cNextFlag = 0; //�Ƿ���ȡ������
	drebdata.data.head.cRaflag = 0;//����Ӧ���־
	
	drebdata.data.head.d_Dinfo.d_cNodePrivateId = m_pRes->g_nMonitorDrebPrivateId;
	drebdata.data.head.d_Dinfo.d_cSvrPrivateId = m_pRes->g_nMonitorSvrPrivateId;
	drebdata.data.head.d_Dinfo.d_nNodeId = m_pRes->g_nMonitorDrebId;
	drebdata.data.head.d_Dinfo.d_nServiceNo = m_pRes->g_nMonitorTxCode;
	drebdata.data.head.d_Dinfo.d_nSvrMainId = m_pRes->g_nMonitorSvrId;
	//ͨ��DREB���ӷ�������
	int index;
	for ( i=0 ; i< 3 ; i++)
	{
		index = GetDrebIndex();
		if (index <0)
		{
			return ;
		}
		if (m_pSockMgr->at(index)->m_bChecked) 
		{
			m_pSockMgr->at(index)->Send(drebdata);
			return;
		}
	}
	return;
}

void CLinkThread::MonitorInfo(S_GWDREB_RSMSG &rcvdata)
{
	int i;
	CBF_Xml xmlpack;
	xmlpack.SetNodeValueByPath("Monitor/BPC�����ڵ�",false,(int)m_pRes->g_nSvrMainId);
	xmlpack.SetNodeValueByPath("Monitor/BPC˽�нڵ�",false,(int)m_pRes->g_nSvrPrivateId);
	xmlpack.SetNodeValueByPath("Monitor/����ʱ��",false,m_pRes->m_sStartDate,false);
	
	//������Ϣ
	for (i=0 ; i < m_pRes->g_vDrebLinkInfo.size() ; i++)
	{
		CXmlNode *connnode = xmlpack.AddNodeByPath("Monitor/������Ϣ",false,"����",false,"");
		if (connnode == NULL)
		{
			m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"AddNodeByPath ����");
			continue;
		}
		connnode->SetAttribute("ip",false,m_pRes->g_vDrebLinkInfo[i].sIp);
		connnode->SetAttribute("port",false,(int)m_pRes->g_vDrebLinkInfo[i].nPort);
		connnode->SetAttribute("index",false,i);
		connnode->SetAttribute("�����ڵ�",false,m_pSockMgr->at(i)->m_nDrebId);
		connnode->SetAttribute("˽�нڵ�",false,m_pSockMgr->at(i)->m_nDrebPrivateId);
		if (m_pSockMgr->at(i)->m_sock == INVALID_SOCKET)
		{
			connnode->SetAttribute("status",false,"δ����");
		}
		else if (m_pSockMgr->at(i)->m_bChecked)
		{
			connnode->SetAttribute("status",false,"����");
		}
		else
		{
			connnode->SetAttribute("status",false,"��������");
		}
	}
	
	int len = DREBDATASIZE;
	if (!xmlpack.ToBuffer(rcvdata.data.buffer,len))
	{
		sprintf(rcvdata.data.buffer,"���岻�㣬������cgateinfo.txt�鿴");
		rcvdata.data.head.nLen = strlen(rcvdata.data.buffer);
		m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"���岻��");
		xmlpack.ToFile("cgateinfo.txt");
	}
	else
	{
		rcvdata.data.head.nLen = len;
	}
	rcvdata.data.head.cRaflag = 1;
	rcvdata.data.head.cZip = 0;
	rcvdata.data.head.a_Ainfo.a_nRetCode = SUCCESS;
	m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
}

void CLinkThread::MonitorHost(S_GWDREB_RSMSG &rcvdata)
{
	int i;
	CBF_Xml xmlpack;
	xmlpack.SetNodeValueByPath("Monitor/BPC�����ڵ�",false,(int)m_pRes->g_nSvrMainId);
	xmlpack.SetNodeValueByPath("Monitor/BPC˽�нڵ�",false,(int)m_pRes->g_nSvrPrivateId);
	xmlpack.SetNodeValueByPath("Monitor/����ʱ��",false,m_pRes->m_sStartDate,false);


	//ȡ������Ϣ
	if (m_pRes->g_nMonitorHost == 1)
	{
		xmlpack.SetNodeValueByPath("Monitor/������Դ/CPU",false,(int)m_sHostInfo.nCpuRate);
		xmlpack.SetNodeAttribute("Monitor/������Դ/�ڴ�",false,"total",false,(int)m_sHostInfo.nTotalMemory);
		xmlpack.SetNodeAttribute("Monitor/������Դ/�ڴ�",false,"used",false,(int)m_sHostInfo.nUsedMemory);
		for (i=0 ; i< m_vdinfo.size() ; i++)
		{
			CXmlNode *disknode = xmlpack.AddNodeByPath("Monitor/������Դ/����",false,"����",false,"");
			if (disknode != NULL)
			{
				disknode->SetAttribute("drive",false,m_vdinfo[i].cIndex,true);
				disknode->SetAttribute("total",false,(int)m_vdinfo[i].TotalSpace);
				disknode->SetAttribute("used",false,(int)m_vdinfo[i].UsedSpace);
			}
		}
	}
	int len = DREBDATASIZE;
	if (!xmlpack.ToBuffer(rcvdata.data.buffer,len))
	{
		sprintf(rcvdata.data.buffer,"���岻�㣬������cgatehost.txt�鿴");
		rcvdata.data.head.nLen = strlen(rcvdata.data.buffer);
		m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"���岻��");
		xmlpack.ToFile("cgatehost.txt");
	}
	else
	{
		rcvdata.data.head.nLen = len;
	}
	rcvdata.data.head.cRaflag = 1;
	rcvdata.data.head.cZip = 0;
	rcvdata.data.head.a_Ainfo.a_nRetCode = SUCCESS;
	m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
}


void CLinkThread::MonitorReport(S_GWDREB_RSMSG &rcvdata)
{
	rcvdata.data.head.nLen =0;
	rcvdata.data.head.cRaflag = 1;
	rcvdata.data.head.cZip = 0;
	rcvdata.data.head.a_Ainfo.a_nRetCode = SUCCESS;
	m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
}

void CLinkThread::MonitorBpu(S_GWDREB_RSMSG &rcvdata)
{
	int i;
	CBF_Xml xmlpack;
	xmlpack.SetNodeValueByPath("Monitor/BPC�����ڵ�",false,(int)m_pRes->g_nSvrMainId);
	xmlpack.SetNodeValueByPath("Monitor/BPC˽�нڵ�",false,(int)m_pRes->g_nSvrPrivateId);
	xmlpack.SetNodeValueByPath("Monitor/����ʱ��",false,m_pRes->m_sStartDate,false);

	char tmpchar[200];
	for ( i=0 ; i<m_pRes->g_vProcessInfo.size() ; i++ )
	{
		sprintf(tmpchar,"Monitor/�����б�/%s",m_pRes->g_vProcessInfo[i].nodeid.c_str());
		xmlpack.SetNodeAttribute(tmpchar,false,"status",false,m_pRes->g_vProcessInfo[i].status.c_str());

		sprintf(tmpchar,"Monitor/�����б�/%s/��������",m_pRes->g_vProcessInfo[i].nodeid.c_str());
		xmlpack.SetNodeValueByPath(tmpchar,false,m_pRes->g_vProcessInfo[i].processname.c_str(),false);

		sprintf(tmpchar,"Monitor/�����б�/%s/������",m_pRes->g_vProcessInfo[i].nodeid.c_str());
		xmlpack.SetNodeValueByPath(tmpchar,false,m_pRes->g_vProcessInfo[i].prgname.c_str(),false);

		sprintf(tmpchar,"Monitor/�����б�/%s/����·��",m_pRes->g_vProcessInfo[i].nodeid.c_str());
		xmlpack.SetNodeValueByPath(tmpchar,false,m_pRes->g_vProcessInfo[i].prgpath.c_str(),false);

		sprintf(tmpchar,"Monitor/�����б�/%s/����Ŀ¼",m_pRes->g_vProcessInfo[i].nodeid.c_str());
		xmlpack.SetNodeValueByPath(tmpchar,false,m_pRes->g_vProcessInfo[i].runpath.c_str(),false);

		sprintf(tmpchar,"Monitor/�����б�/%s/��������",m_pRes->g_vProcessInfo[i].nodeid.c_str());
		xmlpack.SetNodeValueByPath(tmpchar,false,m_pRes->g_vProcessInfo[i].prgpara.c_str(),false);

		sprintf(tmpchar,"Monitor/�����б�/%s/ֹͣ�ű�",m_pRes->g_vProcessInfo[i].nodeid.c_str());
		xmlpack.SetNodeValueByPath(tmpchar,false,m_pRes->g_vProcessInfo[i].stopscript.c_str(),false);

		sprintf(tmpchar,"Monitor/�����б�/%s/�������Ƿ�����",m_pRes->g_vProcessInfo[i].nodeid.c_str());
		xmlpack.SetNodeValueByPath(tmpchar,false,m_pRes->g_vProcessInfo[i].heartstart.c_str(),false);

		sprintf(tmpchar,"Monitor/�����б�/%s/�Ƿ���ܿ���",m_pRes->g_vProcessInfo[i].nodeid.c_str());
		xmlpack.SetNodeValueByPath(tmpchar,false,m_pRes->g_vProcessInfo[i].acceptcontrol.c_str(),false);

		sprintf(tmpchar,"Monitor/�����б�/%s/����ID",m_pRes->g_vProcessInfo[i].nodeid.c_str());
		xmlpack.SetNodeValueByPath(tmpchar,false,m_pRes->g_vProcessInfo[i].pid);

		sprintf(tmpchar,"Monitor/�����б�/%s/�������ʱ��",m_pRes->g_vProcessInfo[i].nodeid.c_str());
		xmlpack.SetNodeValueByPath(tmpchar,false,m_pRes->g_vProcessInfo[i].lastUseTIme);

		sprintf(tmpchar,"Monitor/�����б�/%s/��ǰʱ��",m_pRes->g_vProcessInfo[i].nodeid.c_str());
		xmlpack.SetNodeValueByPath(tmpchar,false,(int)time(NULL));
		
	}
	int len = DREBDATASIZE;
	if (!xmlpack.ToBuffer(rcvdata.data.buffer,len))
	{
		sprintf(rcvdata.data.buffer,"���岻�㣬������clientlist.txt�鿴");
		rcvdata.data.head.nLen = strlen(rcvdata.data.buffer);
		m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"���岻��");
		xmlpack.ToFile("clientlist.txt");
	}
	else
	{
		rcvdata.data.head.nLen = len;
	}
	rcvdata.data.head.cRaflag = 1;
	rcvdata.data.head.cZip = 0;
	rcvdata.data.head.a_Ainfo.a_nRetCode = SUCCESS;
	m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
}

void CLinkThread::Download(S_GWDREB_RSMSG &rcvdata)
{
	char databuf[DREBDATASIZE];
	
	if (rcvdata.data.head.cZip == 2)
	{
		memcpy(databuf,rcvdata.data.buffer,rcvdata.data.head.nLen);
		unsigned int dlen = DREBDATASIZE;
		if (!CBF_Tools::Uncompress((unsigned char *)rcvdata.data.buffer,dlen,(unsigned char *)databuf,rcvdata.data.head.nLen))
		{
			sprintf(rcvdata.data.buffer,"���ݽ�ѹ������");
			rcvdata.data.head.nLen = strlen(rcvdata.data.buffer);
			rcvdata.data.head.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
			rcvdata.data.head.cRaflag = 1;
			rcvdata.data.head.cZip = 0;
			m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
			return ;
		}
	}
	S_BPC_DUFILE sfile;
	if (rcvdata.data.head.nLen  < sizeof(S_BPC_DUFILE))
	{
		m_pLog.LogBin(LOG_ERROR,__FILE__,__LINE__,"�������ݷ���������",\
			rcvdata.data.buffer,rcvdata.data.head.nLen);
		sprintf(rcvdata.data.buffer,"�������ݷ���������");
		rcvdata.data.head.nLen = strlen(rcvdata.data.buffer);
		rcvdata.data.head.a_Ainfo.a_nRetCode = ERR_MONITOR_DOWNLOAD;
		rcvdata.data.head.cRaflag = 1;
		rcvdata.data.head.cZip = 0;
		m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
		return ;
	}
	memcpy(&sfile,rcvdata.data.buffer,sizeof(S_BPC_DUFILE));
	struct_stat fstat;
	FSTAT(sfile.sFileName,&fstat);
	if (fstat.st_size <= 0)
	{
		sprintf(rcvdata.data.buffer,"�ļ�[%s]������",sfile.sFileName);
		rcvdata.data.head.nLen = strlen(rcvdata.data.buffer);
		rcvdata.data.head.a_Ainfo.a_nRetCode = ERR_MONITOR_DOWNLOAD;
		rcvdata.data.head.cRaflag = 1;
		rcvdata.data.head.cZip = 0;
		m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
		return ;
	}
	int offset = atol(sfile.sFileOffset);
	if (offset >= fstat.st_size)
	{
		sprintf(rcvdata.data.buffer,"�ļ�ƫ����[%d]�Ѵ��ڵ����ļ���С",offset);
		rcvdata.data.head.nLen = strlen(rcvdata.data.buffer);
		rcvdata.data.head.a_Ainfo.a_nRetCode = ERR_MONITOR_DOWNLOAD;
		rcvdata.data.head.cRaflag = 1;
		rcvdata.data.head.cZip = 0;
		m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
		return ;
	}
	sprintf(sfile.sFileSize,"%011ld",fstat.st_size);
	FILE *fp = fopen(sfile.sFileName,"rb");
	if (fp == NULL)
	{
		sprintf(rcvdata.data.buffer,"�ļ�[%s]������",sfile.sFileName);
		rcvdata.data.head.nLen = strlen(rcvdata.data.buffer);
		rcvdata.data.head.a_Ainfo.a_nRetCode = ERR_MONITOR_DOWNLOAD;
		rcvdata.data.head.cRaflag = 1;
		rcvdata.data.head.cZip = 0;
		m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
		return ;
	}
	fseek(fp,offset,1);
	int getlen = fread(rcvdata.data.buffer+sizeof(S_BPC_DUFILE),1,50*1024,fp);
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
	memcpy(rcvdata.data.buffer,&sfile,sizeof(S_BPC_DUFILE));
	rcvdata.data.head.nLen = sizeof(S_BPC_DUFILE)+getlen;
	
	rcvdata.data.head.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.data.head.cRaflag = 1;
	rcvdata.data.head.cZip = 0;
	m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);

}

void CLinkThread::Upload(S_GWDREB_RSMSG &rcvdata)
{
	char databuf[DREBDATASIZE];
	if (rcvdata.data.head.cZip == 2)
	{
		memcpy(databuf,rcvdata.data.buffer,rcvdata.data.head.nLen);
		unsigned int dlen = DREBDATASIZE;
		if (!CBF_Tools::Uncompress((unsigned char *)rcvdata.data.buffer,dlen,(unsigned char *)databuf,rcvdata.data.head.nLen))
		{
			sprintf(rcvdata.data.buffer,"���ݽ�ѹ������");
			rcvdata.data.head.nLen = strlen(rcvdata.data.buffer);
			rcvdata.data.head.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
			rcvdata.data.head.cRaflag = 1;
			rcvdata.data.head.cZip = 0;
			m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
			return ;
		}
	}
	S_BPC_DUFILE sfile;
	if (rcvdata.data.head.nLen  <= sizeof(S_BPC_DUFILE))
	{
		m_pLog.LogBin(LOG_ERROR,__FILE__,__LINE__,"�������ݷ��ϴ�����",\
			rcvdata.data.buffer,rcvdata.data.head.nLen);
		sprintf(rcvdata.data.buffer,"�������ݷ��ϴ�����");
		rcvdata.data.head.nLen = strlen(rcvdata.data.buffer);
		rcvdata.data.head.a_Ainfo.a_nRetCode = ERR_MONITOR_UPLOAD;
		rcvdata.data.head.cRaflag = 1;
		rcvdata.data.head.cZip = 0;
		m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
		return ;
	}
	memcpy(&sfile,rcvdata.data.buffer,sizeof(S_BPC_DUFILE));
	m_pLog.LogBin(LOG_DEBUG,__FILE__,__LINE__,"�ϴ��ļ�",rcvdata.data.buffer,sizeof(S_BPC_DUFILE));
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
		sprintf(rcvdata.data.buffer,"�ļ�[%s]�򿪳���",sfile.sFileName);
		rcvdata.data.head.nLen = strlen(rcvdata.data.buffer);
		rcvdata.data.head.a_Ainfo.a_nRetCode = ERR_MONITOR_UPLOAD;
		rcvdata.data.head.cRaflag = 1;
		rcvdata.data.head.cZip = 0;
		m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
		return ;
	}
	fwrite(rcvdata.data.buffer+sizeof(S_BPC_DUFILE),1,rcvdata.data.head.nLen-sizeof(S_BPC_DUFILE),fp);
	fclose(fp);
	if (sfile.cEndFlag == '1')
	{
		struct_stat fstat;
		FSTAT(sfile.sFileName,&fstat);
		if (atol(sfile.sFileSize) != fstat.st_size)
		{
			sprintf(rcvdata.data.buffer,"�ϴ��ļ�[%s]ʧ�ܣ���С[%d]������[%s]",sfile.sFileName,fstat.st_size,sfile.sFileSize);
			rcvdata.data.head.nLen = strlen(rcvdata.data.buffer);
			rcvdata.data.head.a_Ainfo.a_nRetCode = ERR_MONITOR_UPLOAD;
			rcvdata.data.head.cRaflag = 1;
			rcvdata.data.head.cZip = 0;
		m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
			return ;
		}
		sprintf(rcvdata.data.buffer,"�ϴ��ļ�[%s]�ɹ�",sfile.sFileName);
		rcvdata.data.head.nLen = strlen(rcvdata.data.buffer);
		rcvdata.data.head.a_Ainfo.a_nRetCode = SUCCESS;
		rcvdata.data.head.cRaflag = 1;
		rcvdata.data.head.cZip = 0;
		m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
		return ;
	}
	else
	{
		sprintf(rcvdata.data.buffer,"д���ļ�[%s]�ɹ�",sfile.sFileName);
		rcvdata.data.head.nLen = strlen(rcvdata.data.buffer);
		rcvdata.data.head.a_Ainfo.a_nRetCode = SUCCESS;
		rcvdata.data.head.cRaflag = 1;
		rcvdata.data.head.cZip = 0;
		m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
		return ;
	}
}
void CLinkThread::GetHostInfo()
{
	if (m_pRes->g_nMonitorHost !=1 )
	{
		return;
	}
	m_sHostInfo.nCpuRate = m_pHost.GetCpu();
	S_MemoryInf smInfo;
	m_pHost.GetEms(smInfo);
	m_sHostInfo.nTotalMemory = smInfo.EmsTotal;
	m_sHostInfo.nUsedMemory = smInfo.EmsUse;
	m_vdinfo.clear();
	m_pHost.GetDisk(m_vdinfo);
	m_sHostInfo.nDiskNum = m_vdinfo.size();
	
}
void CLinkThread::OnAccept(int conindex)
{
	char address[32];
	SOCKET_HANDLE tsock=INVALID_SOCKET;
	int i;
	bool havefree=false;
	m_tcpSocket.AttachSocket(m_pSockMgr->at(conindex)->m_sock,"");
	tsock= m_tcpSocket.Accept(address,false);
	if (tsock != INVALID_SOCKET)
	{
		m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"��[%d]�˿ڽ��յ�[%s]������",m_pSockMgr->at(conindex)->m_nPort,address);
		for (i=m_pRes->g_vDrebLinkInfo.size()+1; i <m_pSockMgr->m_socketList.size(); i++)
		{
			if ( m_pSockMgr->at(i)->m_sock == INVALID_SOCKET )
			{
				havefree = true;
				m_pSockMgr->at(i)->SetAccept(tsock,address);
				break;
			}
		}
		if (havefree)
		{
			m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"���մ�%s������sockid=%d index=%d ",address,tsock,i);
		}
		else
		{
			m_pLog.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"������������%s������%d���ܾ�",address,tsock);
			closesocket(tsock);
		}
		return;
	}
	return ;
}

int CLinkThread::GetDrebIndex()
{
	if (m_pRes->g_vDrebLinkInfo.size()<1)
	{
		return -1;
	}
	m_nDrebIndex++;
	if (m_nDrebIndex >= m_pRes->g_vDrebLinkInfo.size())
	{
		m_nDrebIndex = 0;
	}
	return m_nDrebIndex;
}

void CLinkThread::WriteFile()
{
	char tmpchar[200];
	if (m_pRes == NULL)
	{
		return ;
	}
	for (int i=0; i< m_pRes->g_vProcessInfo.size() ; i++)
	{
		sprintf(tmpchar,"daemon/%s/pid",m_pRes->g_vProcessInfo[i].nodeid.c_str());
		m_pRes->g_pRunXml.SetNodeValueByPath(tmpchar,false,m_pRes->g_vProcessInfo[i].pid);
		sprintf(tmpchar,"daemon/%s/lasttime",m_pRes->g_vProcessInfo[i].nodeid.c_str());
		m_pRes->g_pRunXml.SetNodeValueByPath(tmpchar,false,m_pRes->g_vProcessInfo[i].lastUseTIme);
		sprintf(tmpchar,"daemon/%s/curtime",m_pRes->g_vProcessInfo[i].nodeid.c_str());
		m_pRes->g_pRunXml.SetNodeValueByPath(tmpchar,false,(int)time(NULL));
	}
	sprintf(tmpchar,"%s/monirun.xml",m_pRes->g_sCurPath);
	m_pRes->g_pRunXml.ToFile(tmpchar);
}

void CLinkThread::MonitorStopBpu(S_GWDREB_RSMSG &rcvdata)
{
	if (rcvdata.data.head.cZip != 0)
	{
		if (!UnzipBuf(&(rcvdata.data)))
		{
			sprintf(rcvdata.data.buffer,"���ݽ�ѹ������");
			rcvdata.data.head.nLen = strlen(rcvdata.data.buffer);
			rcvdata.data.head.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
			rcvdata.data.head.cRaflag = 1;
			rcvdata.data.head.cZip = 0;
			m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
			return ;
		}
	}
	CBF_Xml  xmlpack;
	if (!xmlpack.FromBuffer(rcvdata.data.buffer))
	{
		sprintf(rcvdata.data.buffer,"�������ݷ�xml");
		rcvdata.data.head.nLen = strlen(rcvdata.data.buffer);
		rcvdata.data.head.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
		rcvdata.data.head.cRaflag = 1;
		rcvdata.data.head.cZip = 0;
		m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
		return ;
	}
	CXmlNode *stopnode = xmlpack.GetNodeByPath("Monitor/StopBpu",false);
	if (stopnode == NULL)
	{
		sprintf(rcvdata.data.buffer,"xml��[Monitor/StopBpu]�ڵ�");
		rcvdata.data.head.nLen = strlen(rcvdata.data.buffer);
		rcvdata.data.head.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
		rcvdata.data.head.cRaflag = 1;
		rcvdata.data.head.cZip = 0;
		m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
		return ;
	}
	int i;
	int bpuindex=-1;
	std::string bpugroupname;
	int bpugnum=0;
	S_PROCESS_NODE node;
	bool isfind = false;
	CXmlNode *bpunode = (CXmlNode *)stopnode->GetFirstChild();
	while (bpunode != NULL)
	{
		bpugroupname = bpunode->GetNodeName();
		isfind=false;
		for (i=0; i< m_pRes->g_vProcessInfo.size() ; i++)
		{
			if (bpugroupname.compare(m_pRes->g_vProcessInfo[i].nodeid) == 0)
			{
				isfind = true;
				bpuindex = i;
				break;
			}
		}
		if (isfind)
		{
			if (m_pRes->g_vProcessInfo[bpuindex].acceptcontrol.compare("1") != 0)
			{
				m_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,"[%s %s]����[%d]����Ϊ�����ܿ���",\
					m_pRes->g_vProcessInfo[bpuindex].processname.c_str(),\
					m_pRes->g_vProcessInfo[bpuindex].nodeid.c_str(),m_pRes->g_vProcessInfo[bpuindex].pid);
				continue;
			}
			if (m_pProcessMgr.FindProcess(&(m_pRes->g_vProcessInfo[bpuindex]),m_errMsg) == 0)
			{
				m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"ԭ[%s %s]����[%d]�Ѵ��ڣ�ֹͣ",\
					m_pRes->g_vProcessInfo[bpuindex].processname.c_str(),\
					m_pRes->g_vProcessInfo[bpuindex].nodeid.c_str(),m_pRes->g_vProcessInfo[bpuindex].pid);
				if (m_pRes->g_vProcessInfo[bpuindex].stopscript.length()>0) //��ֹͣ�ű�
				{
					system(m_pRes->g_vProcessInfo[bpuindex].stopscript.c_str());
					if (m_pProcessMgr.FindProcess(&(m_pRes->g_vProcessInfo[bpuindex]),m_errMsg) == 0) //û�д˽���
					{
						m_pRes->g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"����[%s] PID[%d] ����ֹͣ�ű�[%s] �ȴ�ֹͣ",\
							m_pRes->g_vProcessInfo[bpuindex].nodeid.c_str(),m_pRes->g_vProcessInfo[bpuindex].pid,\
							m_pRes->g_vProcessInfo[bpuindex].stopscript.c_str());
					}
				}
				else if (m_pProcessMgr.StopProcess(&(m_pRes->g_vProcessInfo[bpuindex]),m_errMsg)!=0)
				{
					m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"ԭ[%s %s]����[%d]ֹͣʧ�� %s",\
						m_pRes->g_vProcessInfo[bpuindex].processname.c_str(),\
						m_pRes->g_vProcessInfo[bpuindex].nodeid.c_str(),m_pRes->g_vProcessInfo[bpuindex].pid,m_errMsg);
				}
				m_pRes->g_vProcessInfo[bpuindex].controltype = "1";
			}
		}
		bpunode = (CXmlNode *)bpunode->getNextSibling();
	}
	sprintf(rcvdata.data.buffer,"ֹͣ��ɣ���鿴����");
	rcvdata.data.head.nLen = strlen(rcvdata.data.buffer);
	rcvdata.data.head.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.data.head.cRaflag = 1;
	rcvdata.data.head.cZip = 0;
	m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
	return;
}

void CLinkThread::MonitorStartBpu(S_GWDREB_RSMSG &rcvdata)
{
	if (rcvdata.data.head.cZip != 0)
	{
		if (!UnzipBuf(&(rcvdata.data)))
		{
			sprintf(rcvdata.data.buffer,"���ݽ�ѹ������");
			rcvdata.data.head.nLen = strlen(rcvdata.data.buffer);
			rcvdata.data.head.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
			rcvdata.data.head.cRaflag = 1;
			rcvdata.data.head.cZip = 0;
			m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
			return ;
		}
	}
	CBF_Xml  xmlpack;
	if (!xmlpack.FromBuffer(rcvdata.data.buffer))
	{
		sprintf(rcvdata.data.buffer,"�������ݷ�xml");
		rcvdata.data.head.nLen = strlen(rcvdata.data.buffer);
		rcvdata.data.head.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
		rcvdata.data.head.cRaflag = 1;
		rcvdata.data.head.cZip = 0;
		m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
		return ;
	}
	CXmlNode *stopnode = xmlpack.GetNodeByPath("Monitor/StartBpu",false);
	if (stopnode == NULL)
	{
		sprintf(rcvdata.data.buffer,"xml��[Monitor/StartBpu]�ڵ�");
		rcvdata.data.head.nLen = strlen(rcvdata.data.buffer);
		rcvdata.data.head.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
		rcvdata.data.head.cRaflag = 1;
		rcvdata.data.head.cZip = 0;
		m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
		return ;
	}
	int i;
	int bpuindex=-1;
	std::string bpugroupname;
	int bpugnum=0;
	S_PROCESS_NODE node;
	bool isfind = false;
	CXmlNode *bpunode = (CXmlNode *)stopnode->GetFirstChild();
	while (bpunode != NULL)
	{
		bpugroupname = bpunode->GetNodeName();
		isfind=false;
		for (i=0; i< m_pRes->g_vProcessInfo.size() ; i++)
		{
			if (bpugroupname.compare(m_pRes->g_vProcessInfo[i].nodeid) == 0)
			{
				isfind = true;
				bpuindex = i;
				break;
			}
		}
		if (isfind)
		{
			if (m_pProcessMgr.FindProcess(&(m_pRes->g_vProcessInfo[bpuindex]),m_errMsg) != 0)
			{
				m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"[%s %s]����[%d]������ %s",\
					m_pRes->g_vProcessInfo[bpuindex].processname.c_str(),\
					m_pRes->g_vProcessInfo[bpuindex].nodeid.c_str(),m_pRes->g_vProcessInfo[bpuindex].pid,m_errMsg);
				if (!m_pProcessMgr.StartProcess(&(m_pRes->g_vProcessInfo[bpuindex]),m_errMsg))
				{
					m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"[%s %s]����[%d]����ʧ�� %s",\
						m_pRes->g_vProcessInfo[bpuindex].processname.c_str(),\
						m_pRes->g_vProcessInfo[bpuindex].nodeid.c_str(),m_pRes->g_vProcessInfo[bpuindex].pid,m_errMsg);
				}
				else
				{
					m_pRes->g_vProcessInfo[bpuindex].controltype = "0";
				}
			}
		}
		bpunode = (CXmlNode *)bpunode->getNextSibling();
	}
	sprintf(rcvdata.data.buffer,"����BPU�ɹ�");
	rcvdata.data.head.nLen = strlen(rcvdata.data.buffer);
	rcvdata.data.head.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.data.head.cRaflag = 1;
	rcvdata.data.head.cZip = 0;
	m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
	return;
}

void CLinkThread::OnCheckProcess()
{
	//�鿴����Ľ���
	for (int i=0 ; i< m_pRes->g_vProcessInfo.size() ; i++)
	{
		if (m_pProcessMgr.FindProcess(&(m_pRes->g_vProcessInfo[i]),m_errMsg) != 0) //û�д˽���
		{
			if (m_pRes->g_vProcessInfo[i].controltype.compare("1") == 0)
			{
				m_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,"[%s %s]���� ״̬Ϊ�ͻ��˿���",\
					m_pRes->g_vProcessInfo[i].processname.c_str(),\
					m_pRes->g_vProcessInfo[i].nodeid.c_str());
				continue;
			}
			m_pRes->g_vProcessInfo[i].status = "1";
 			//20170223 ����ֹͣ�ű����ȵ���ֹͣ�ű�ͣһ��
 			if (m_pRes->g_vProcessInfo[i].stopscript.length()>0) //��ֹͣ�ű�
 			{
 				system(m_pRes->g_vProcessInfo[i].stopscript.c_str());
 			}

			if (!m_pProcessMgr.StartProcess(&(m_pRes->g_vProcessInfo[i]),m_errMsg))
			{
				m_pRes->g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"��������[%s]ʧ�� %s",\
					m_pRes->g_vProcessInfo[i].nodeid.c_str(),m_errMsg);
			}
			else
			{
				m_pRes->g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,"��������[%s]�ɹ� PID[%d]",\
					m_pRes->g_vProcessInfo[i].nodeid.c_str(),m_pRes->g_vProcessInfo[i].pid);
				m_pRes->g_vProcessInfo[i].status = "0";
				m_pRes->g_vProcessInfo[i].controltype = "0";
				m_pRes->g_vProcessInfo[i].lastUseTIme = time(NULL)+300;
			}
		}
		else //�д˽���
		{
			m_pRes->g_pLog.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"����[%s] PID[%d] �Ѵ���",\
					m_pRes->g_vProcessInfo[i].nodeid.c_str(),m_pRes->g_vProcessInfo[i].pid);
			if (m_pRes->g_vProcessInfo[i].heartstart.compare("1") == 0) //����������
			{
				if (time(NULL) - m_pRes->g_vProcessInfo[i].lastUseTIme > m_pRes->g_nHeartRun)
				{
					m_pRes->g_vProcessInfo[i].status = "2";
				}
				if (time(NULL) - m_pRes->g_vProcessInfo[i].lastUseTIme > m_pRes->g_nHeartRun*3)
				{
					m_pRes->g_vProcessInfo[i].status = "1";
					if (m_pRes->g_vProcessInfo[i].stopscript.length()>0) //��ֹͣ�ű�
					{
						system(m_pRes->g_vProcessInfo[i].stopscript.c_str());
						if (m_pProcessMgr.FindProcess(&(m_pRes->g_vProcessInfo[i]),m_errMsg) != 0) //û�д˽���
						{
							if (!m_pProcessMgr.StartProcess(&(m_pRes->g_vProcessInfo[i]),m_errMsg))
							{
								m_pRes->g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"��������[%s]ʧ�� %s",\
									m_pRes->g_vProcessInfo[i].nodeid.c_str(),m_errMsg);
							}
							else
							{
								m_pRes->g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,"��������[%s]�ɹ� PID[%d]",\
									m_pRes->g_vProcessInfo[i].nodeid.c_str(),m_pRes->g_vProcessInfo[i].pid);
								m_pRes->g_vProcessInfo[i].status = "0";
								m_pRes->g_vProcessInfo[i].controltype = "0";
								m_pRes->g_vProcessInfo[i].lastUseTIme = time(NULL)+300;
							}
						}
						else
						{
							m_pRes->g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,"����[%s] PID[%d] ����ֹͣ�ű�[%s] �ȴ�ֹͣ",\
								m_pRes->g_vProcessInfo[i].nodeid.c_str(),m_pRes->g_vProcessInfo[i].pid,m_pRes->g_vProcessInfo[i].stopscript.c_str());
						}
					}
					else
					{
						if (m_pProcessMgr.StopProcess(&(m_pRes->g_vProcessInfo[i]),m_errMsg) == 0)
						{
							m_pRes->g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,"����[%s] PID[%d] ��������������KILLֹͣ�ɹ�",\
								m_pRes->g_vProcessInfo[i].nodeid.c_str(),m_pRes->g_vProcessInfo[i].pid);
							if (!m_pProcessMgr.StartProcess(&(m_pRes->g_vProcessInfo[i]),m_errMsg))
							{
								m_pRes->g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"��������[%s]ʧ�� %s",\
									m_pRes->g_vProcessInfo[i].nodeid.c_str(),m_errMsg);
							}
							else
							{
								m_pRes->g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,"��������[%s]�ɹ� PID[%d]",\
									m_pRes->g_vProcessInfo[i].nodeid.c_str(),m_pRes->g_vProcessInfo[i].pid);
								m_pRes->g_vProcessInfo[i].status = "0";
								m_pRes->g_vProcessInfo[i].controltype = "0";
								m_pRes->g_vProcessInfo[i].lastUseTIme = time(NULL)+300;
							}
						}
					}
				}
				else
				{
					m_pRes->g_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"����[%s] PID[%d] �Ѵ��� ������[%d]��δ�յ�",\
						m_pRes->g_vProcessInfo[i].nodeid.c_str(),m_pRes->g_vProcessInfo[i].pid,time(NULL) - m_pRes->g_vProcessInfo[i].lastUseTIme);
				}
			}
		}
	}
}
