// LinkThread.cpp: implementation of the CLinkThread class.
//
//////////////////////////////////////////////////////////////////////

#include "LinkThread.h"
#include "dreberrcode.h"
#include "Bpc_Timer.h"

#define  DESENCRYPTKEY    "rd402wms"
#define  DESENCRYPTKEY3   "wms13711songfree"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern char g_VERSION[20];
extern CBpc_Timer      g_pBpcTime;



#ifdef _WINDOWS
#define   INT64PFM  "%I64d"
#else
#define   INT64PFM  "%lld"
#endif

#define  CHECKFLAG_UNKNOW   0   //����δ֪
#define  CHECKFLAG_STEP1    1    //�ӽӿͻ��˷���ǩ������Կ
#define  CHECKFLAG_STEP2    2    //���տͻ�����֤����¼
#define  CHECKFLAG_NORMAL   5    //�������ӣ���¼����Ϊ��������

CLinkThread::CLinkThread()
{
	m_bIsRunning = false;
	m_pRes = NULL;
	m_pMemPool = NULL;
	m_bIsInit = false;
	m_nBegin=0;
	m_nScanBegin = 0;
	m_nEnd=1;
	m_sThreadName = "CLinkThread";
	CBF_Tools::MakeDir("data");
	m_bIsPingTimer = false;

	bzero(&(m_pDataBuf.data->head),DREBHEADLEN);
	m_pSendData = NULL;
}

CLinkThread::~CLinkThread()
{

}

std::string CLinkThread::GetBpcMsgType(int msgtype)
{
	switch (msgtype)
	{
	case MSG_REQ:        // 0      //��������,bpc����BPU������
		return "MSG_REQ";
	case MSG_FREE:        //1      //BPU���Ϳ�������
		return "MSG_FREE";
	case MSG_GETSERIAL:   //4      //BPUȡ����Ψһ��ˮ
		return "MSG_GETSERIAL";
	case MSG_SETGPARA1:  //5      //BPU����SETGPARA1
		return "MSG_SETGPARA1";
	case MSG_SETGPARA2:   //6      //BPU����SETGPARA2
		return "MSG_SETGPARA2";
	case MSG_SETGPARA3:   //7      //BPU����SETGPARA3
		return "MSG_SETGPARA3";	
	case MSG_EXTCALL:      //10      //BPU�����������	
		return "MSG_EXTCALL";
	case MSG_BPCCALL:      //11      //BPU����PU���������
		return "MSG_BPCCALL";
	case MSG_BPUREG:       //12      //BPU����ע������
		return "MSG_BPUREG";
	case MSG_BPUISREG:     //13      //BPU�����Ƿ�ע������
		return "MSG_BPUISREG";
	case MSG_GETNEXT:      //15      //dreb������ȡ����������Ϣ
		return "MSG_GETNEXT";
	default:
		return "";
	}
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

bool CLinkThread::SetGlobalVar(CGateResource *res,CPoolData *pooldata,CBF_BufferPool *mempool,CSocketMgr *sockmgr,CSendData *senddata,CAIO_Work *aio)
{
	m_pRes = res;
	m_pSockMgr = sockmgr;
	m_pMemPool = mempool;
	m_pPoolData = pooldata;
	m_pSendData = senddata;
	
	m_pAio = aio;
	m_pLog  = &(m_pRes->g_pLog);
	m_bIsInit = true;

	m_pDesZip.SetLogPara(m_pLog);

	
	m_pDrebLog = &(m_pRes->g_pDrebLog);
	

	m_pTimer.Init(100,true);
	//�ڵ�·�ɶ�ʱ��
	m_pTimer.SetTimer(0,5000,&CLinkThread::OnTimer,this); //���ö�ʱ�� 

	return true;
}
int CLinkThread::Run()
{
	struct timeval tv;
	int result;
	unsigned int i;
	int maxfd=0;
	//int lasttime=time(NULL);
	//int lasttime2=time(NULL);
	m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"����dreb�߳�");
	while(!m_pRes->g_bIsExit)
	{
		FD_ZERO(&m_rset);
		FD_ZERO(&m_wset);
#if defined(_WINDOWS)
		FD_ZERO(&m_eset);
#endif
		
		m_nScanBegin = m_nBegin;
		//ȡ������socket����socket���뵽fd_set����
		GetMaxfdAddSelect(maxfd);
		if (maxfd  <=0)
		{
			for (i=0; i<m_pRes->g_vDrebLinkInfo.size() ;i++)
			{
				OnConnect(i);
			}
			SLEEP(5);
//			m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"��dreb���� %d",tv.tv_sec);
			continue;
		}
		tv.tv_sec = 1;
		tv.tv_usec = 0;
#if defined(_WINDOWS)
		result = select(maxfd+1, &m_rset, &m_wset, &m_eset, &tv);
#else
		result = select(maxfd+1, &m_rset, &m_wset, NULL, &tv);
#endif
		
		if (result != 0) //���¼�����,����
		{
			OnEvent();
		}
		if (m_bIsPingTimer)
		{
//			m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���NOEVENT");
			OnNoEvent();
			if (m_pRes->g_nUseMonitor == 1)
			{
				OnMonitor();
			}
			
			m_bIsPingTimer = false;
		}

	}
	m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"ֹͣdreb�߳�");
	return 0;
}
int CLinkThread::GetMaxfdAddSelect(int &maxfd)
{
	int ret=0;
	maxfd = 0;
	for (unsigned int i=0;i< m_pRes->g_vDrebLinkInfo.size() ;i++)
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
	unsigned int j;
	int tmptime;

	for (j=0; j< m_pRes->g_vDrebLinkInfo.size(); j++)
	{
		if (m_pSockMgr->at(j)->m_sock != INVALID_SOCKET && m_pSockMgr->at(j)->m_nType == SOCK_DREB)
		{
			tmptime = time(NULL) - m_pSockMgr->at(j)->m_nReadtime;
			if ( tmptime > m_pRes->g_nHeartRun && tmptime < m_pRes->g_nDisconnectTime)
			{
				OnPing(j);
			}
			else if (tmptime > m_pRes->g_nDisconnectTime)
			{
				m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"����index[%d] ��ǰʱ��[%d] �������¶�ʱ��[%d] ���öϿ�ʱ��[%d] ���ӳ�ʱ��δ�ã��Ͽ�",\
					j,time(NULL),m_pSockMgr->at(j)->m_nReadtime,m_pRes->g_nDisconnectTime);
				OnClose(j,"���ӳ�ʱ��δ�ã��Ͽ�",__FILE__,__LINE__);
			}
		}
		if (m_pRes->g_bIsExit)
		{
			return;
		}
		//�����������߽ڵ����ӣ���û��������ȥ����
		if (m_pSockMgr->at(j)->m_nType == SOCK_DREB )
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
	unsigned int i;
	for (i= 0; i< m_pRes->g_vDrebLinkInfo.size() ; i++)
	{
		if (m_pSockMgr->at(i)->m_sock == INVALID_SOCKET)//δ����
		{
			if (m_pSockMgr->at(i)->m_nType == SOCK_DREB   ) //�����������߽ڵ����ӣ���ȥ����
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
	else //�������޶��¼�
	{
		if (m_pSockMgr->at(conindex)->m_sock != INVALID_SOCKET)
		{
			//û�м��ͨ��
			if (!m_pSockMgr->at(conindex)->m_bChecked)
			{
				//�������߽ڵ�����
				if (m_pSockMgr->at(conindex)->m_nType == SOCK_DREB) 
				{
					if (time(NULL)-m_pSockMgr->at(conindex)->m_nReadtime >= m_pRes->g_nHeartRun)
					{
						if (m_pSockMgr->at(conindex)->NeedConnect())
						{
							m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�������ӵ�%s:%dδ�ɹ�",m_pSockMgr->at(conindex)->m_sDrebIp.c_str(),m_pSockMgr->at(conindex)->m_nDrebPort);
							closesocket(m_pSockMgr->at(conindex)->m_sock);
							m_pSockMgr->at(conindex)->m_sock = INVALID_SOCKET;
						}
					}
				}// end if (m_pSockMgr->at(conindex)->m_nType == BPCSOCK_TODREB) //�������߽ڵ�����
				else if (m_pSockMgr->at(conindex)->m_nType == SOCK_UNKNOW)
				{
					if (time(NULL)-m_pSockMgr->at(conindex)->m_nReadtime >= m_pRes->g_nHeartRun)
					{
						OnClose(conindex,"���ӷǷ�",__FILE__,__LINE__);
					} 
				}
				else 
				{
					if (time(NULL)-m_pSockMgr->at(conindex)->m_nReadtime >= m_pRes->g_nHeartRun)
					{
						OnClose(conindex,"Э�̳�ʱ",__FILE__,__LINE__);
					} 
				}
			}
		}// end if (m_pSockMgr->at(conindex)->m_sock != INVALID_SOCKET)
	}// end if (ret == 1) //�ж��¼�
}

void CLinkThread::OnWriteEvent(int conindex)
{
	
	if (m_pSockMgr->at(conindex)->m_sock == INVALID_SOCKET)
	{
		return;
	}
#if defined(_WINDOWS)
	int ret;
	ret = FD_ISSET(m_pSockMgr->at(conindex)->m_sock, &m_eset);
	if (ret != 0)
	{
		//��������������DREB
		if (m_pSockMgr->at(conindex)->m_nType == SOCK_DREB)
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
			if (m_pSockMgr->at(conindex)->m_nType == SOCK_DREB ) 
			{
				OnConnected(conindex);//ȥ�����������߽ڵ�
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
				if (m_pSockMgr->at(conindex)->m_nType == SOCK_DREB)
				{
					OnConnected(conindex);//ȥ�����������߽ڵ�
				}
			}
			else
			{
				if (m_pSockMgr->at(conindex)->m_nType == SOCK_DREB)
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
	if (m_pSockMgr->at(conindex)->m_nType == SOCK_DREB)
	{
		m_pSockMgr->at(conindex)->m_nPingTime = time(NULL);
		if (!ResetData(conindex,&m_pDataBuf))
		{
			return;
		}
		m_pDataBuf.data->head.cRaflag = 0;
		m_pDataBuf.data->head.cZip = 0;
		m_pDataBuf.data->head.nLen = 0;
		m_pDataBuf.data->head.cCmd = CMD_PING;
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

	m_pSockMgr->at(conindex)->m_nConntime = time(NULL);
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
		
		m_pDataBuf.data->head.cRaflag = 0;
		m_pDataBuf.data->head.cZip = 0;
		m_pDataBuf.data->head.cCmd = CMD_CONNECT;
		m_pDataBuf.data->head.cDrebAffirm = 0;
	
		
		m_pDataBuf.data->head.nLen = sizeof(S_CONNECT_DATA);
		S_CONNECT_DATA *data = (S_CONNECT_DATA *)m_pDataBuf.data->buffer;
		data->cConnectType = 1;
		data->nSvrMainId = m_pRes->g_nSvrMainId;
		data->cSvrPrivateId = m_pRes->g_nSvrPrivateId;
		sprintf(data->sVersion,"%s",g_VERSION);
		m_pDrebEndian.Endian2Comm((unsigned char *)&(data->nSvrMainId),sizeof(data->nSvrMainId));
		
		if (m_pSockMgr->at(conindex)->Send(m_pDataBuf,true) == 0)
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
	char tmpchar[200];
	ret = m_pSockMgr->at(conindex)->OnRecv();	
	if (ret<=0)
	{
		sprintf(tmpchar,"%s Զ�˹رգ��������ݲ����� ret=[%d]",m_pSockMgr->at(conindex)->m_sAddress,ret);
		OnClose(conindex,tmpchar,__FILE__,__LINE__);
		return;
	}
	//��ʼ���մ���
	if (m_pSockMgr->at(conindex)->m_nRcvBufLen >= DREBHEADLEN)
	{
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"��ʼ���մ���  ���ݳ���%d",m_pSockMgr->at(conindex)->m_nRcvBufLen );
	}
	while (m_pSockMgr->at(conindex)->m_nRcvBufLen >= DREBHEADLEN)
	{
		if (!ResetData(conindex,&m_pDataRcv))
		{
			return;
		}
		ret = m_pSockMgr->at(conindex)->GetRecvData(&m_pDataRcv);
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"index[%d] GetRecvData ����[%d]",conindex,ret);
		if (ret >0 ) //������
		{
			m_pDataRcv.nIndex = conindex;
			//����dreb����������
			if (m_pDataRcv.data->head.cCmd == CMD_PING || m_pDataRcv.data->head.cCmd == CMD_CONNECT) //��ӦPINGӦ��
			{
				if (m_pDataRcv.data->head.cRaflag == 0) //����
				{
					m_pDataRcv.data->head.cRaflag = 1;
					m_pSockMgr->at(conindex)->Send(m_pDataRcv);
					m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"index[%d]�յ�CMD_PING,����Ӧ��",conindex);
				}
				else //Ӧ��ʲô��������
				{
					if ( m_pDataRcv.data->head.cCmd == CMD_CONNECT)
					{
						m_pSockMgr->at(conindex)->m_nDrebId = m_pDataRcv.data->head.a_Ainfo.a_nNodeId;
						m_pSockMgr->at(conindex)->m_nDrebPrivateId = m_pDataRcv.data->head.a_Ainfo.a_cNodePrivateId;
						m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"index[%d]�յ�CMD_CONNECTӦ��",conindex);
					}
					else
					{
						m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"index[%d]�յ�CMD_PINGӦ��",conindex);
					}
					
					m_pMemPool->PoolFree(m_pDataRcv.data);
					m_pDataRcv.data = NULL;
				}
				m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"index[%d]��ʼ���մ���  ���ݳ���%d",conindex,m_pSockMgr->at(conindex)->m_nRcvBufLen );
				continue; ;
			}
			m_pDataRcv.nIndex = conindex;
			ProcessDreb(m_pDataRcv);
			m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"index[%d]������һ������  �������ݳ���[%d]",conindex,m_pSockMgr->at(conindex)->m_nRcvBufLen);

		}
		else  //ret <=0�����δ��������
		{
			if (ret <0)
			{
				OnClose(conindex,"CRCУ���",__FILE__,__LINE__);
			}
			m_pMemPool->PoolFree(m_pDataRcv.data);
			m_pDataRcv.data = NULL;
			m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"�˳����մ���  �������ݳ���[%d]",m_pSockMgr->at(conindex)->m_nRcvBufLen);
			break;
		}
	}
	
	return ;
}


bool CLinkThread::ResetData(int index,S_GATEDREB_RSMSG *data)
{
	data->data = (PCOMMSTRU )m_pMemPool->PoolMalloc();
	if (data->data == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"������Ϣ�ռ����");
		m_pSockMgr->at(index)->OnClose("������Ϣ�ռ����",__FILE__,__LINE__);
		return false;
	}
	data->offset = 0;
	data->rtime = time(NULL);
	data->nIndex = index;
	return true;
}


void CLinkThread::ProcessDreb(S_GATEDREB_RSMSG &rcvdata)
{
	
	if (rcvdata.data->head.cCmd != CMD_PING && rcvdata.data->head.cCmd != CMD_CONNECT && rcvdata.data->head.cCmd != CMD_REGSERVICE)
	{
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"��DREB[%d %d %d]�յ�����Ϣ DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Դ[%d %d] ҵ�����ݳ���[%d]",\
			rcvdata.data->head.s_Sinfo.s_nNodeId,rcvdata.data->head.s_Sinfo.s_cNodePrivateId,rcvdata.nIndex,\
			GetDrebCmdType(rcvdata.data->head.cCmd).c_str(),rcvdata.data->head.cNextFlag,\
			rcvdata.data->head.cRaflag,rcvdata.data->head.d_Dinfo.d_nServiceNo,rcvdata.data->head.s_Sinfo.s_nNodeId,\
			rcvdata.data->head.s_Sinfo.s_cNodePrivateId,rcvdata.data->head.s_Sinfo.s_nDrebSerial,\
			rcvdata.data->head.s_Sinfo.s_nSerial,rcvdata.data->head.s_Sinfo.s_nHook,rcvdata.data->head.nLen);
	}
	else
	{
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���յ�DREB[%d]����Ϣ DREB����[%s]  ҵ�����ݳ���[%d]",\
			rcvdata.nIndex,GetDrebCmdType(rcvdata.data->head.cCmd).c_str(),rcvdata.data->head.nLen);
	}

	if (!UnzipBuf(rcvdata.data))
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
		m_pMemPool->PoolFree(rcvdata.data);
		rcvdata.data = NULL;
		return;
	}
	if (rcvdata.data->head.cCmd != CMD_PING && rcvdata.data->head.cCmd != CMD_CONNECT && rcvdata.data->head.cCmd != CMD_REGSERVICE)
	{
		m_pDrebLog->LogDreb(LOG_DEBUG,rcvdata.data,false);
	}
	if (rcvdata.data->head.cCmd == CMD_MONITOR_BPC)
	{
		//m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"CMD_MONITOR_BPC [%d]",rcvdata.data->head.d_Dinfo.d_nServiceNo);
		CallBpcLocalFunc(rcvdata);
		return;
	}
	S_CGATE_SMSG data;
	bzero(&data,sizeof(S_CGATE_SMSG));
	data.data.head.nLen = rcvdata.data->head.nLen;
	if (data.data.head.nLen>0 && data.data.head.nLen<DREBDATASIZE)
	{
		memcpy(data.data.buffer,rcvdata.data->buffer,data.data.head.nLen);
	}
	else if (data.data.head.nLen >=DREBDATASIZE)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ݷǷ� %d",data.data.head.nLen);
		m_pMemPool->PoolFree(rcvdata.data);
		rcvdata.data = NULL;
		return;
	}
	data.s_nDrebSerial = rcvdata.data->head.s_Sinfo.s_nDrebSerial;
	data.txcode = rcvdata.data->head.d_Dinfo.d_nServiceNo;
	if (rcvdata.data->head.cRaflag == 0) //����
	{
		data.data.head.nRetCode = 0;
		data.data.head.stComm.cCmd = rcvdata.data->head.cCmd;
		data.data.head.stComm.cMsgType = MSG_PUSH;
		data.data.head.stComm.cNextFlag = rcvdata.data->head.cNextFlag;
		data.data.head.stComm.cRaflag = rcvdata.data->head.cRaflag;
		data.data.head.stComm.cZip = rcvdata.data->head.cZip;
		data.data.head.stComm.nCrc = 0;
		data.data.head.stDest.cNodePrivateId = rcvdata.data->head.s_Sinfo.s_cNodePrivateId;
		data.data.head.stDest.cSvrPrivateId = rcvdata.data->head.s_Sinfo.s_cSvrPrivateId;
		data.data.head.stDest.nNodeId = rcvdata.data->head.s_Sinfo.s_nNodeId;
		
		data.d_nNodeId = rcvdata.data->head.s_Sinfo.s_nNodeId;
		data.d_cNodePrivateId = rcvdata.data->head.s_Sinfo.s_cNodePrivateId;
		data.s_nDrebSerial = rcvdata.data->head.s_Sinfo.s_nDrebSerial;
		data.rtime = time(NULL);

// 		PS_GATE_SERIAL gserial = (PS_GATE_SERIAL)&(rcvdata.data->head.s_Sinfo.s_nSerial);//��ŵ�����������+������ˮ
// 		
// 
// 		data.data.head.stDest.nSerial = gserial->index;//��������

		data.data.head.stDest.nServiceNo = rcvdata.data->head.d_Dinfo.d_nServiceNo;
		data.data.head.stDest.nSvrMainId = rcvdata.data->head.s_Sinfo.s_nSvrMainId;
		data.data.head.stNext.n_nNextNo = 0;
		data.data.head.stNext.n_nNextOffset = 0;

		data.data.head.stDest.nSerial = rcvdata.data->head.s_Sinfo.s_nSerial;
		data.index = rcvdata.data->head.s_Sinfo.s_nGateIndex;
		data.timestamp = rcvdata.data->head.s_Sinfo.s_nHook;
		

// #define  CMD_DPCALL      8  //�������߽ڵ�ͬ������ Ҫ�����մ�����ɺ�Ӧ��
// #define  CMD_DPACALL     9  //�������߽ڵ��첽���� Ҫ����յ����������߽ڵ㷢���������ȷ��Ӧ��
// #define  CMD_DPBC        10  //�������߽ڵ�㲥��������Ϣ����ָ���������߽ڵ�������ע��ķ���,Ҫ���������߽ڵ�Ӧ��
// #define  CMD_DPABC       11  //�������߽ڵ�㲥��������Ϣ����ָ���������߽ڵ�������ע��ķ���,��Ҫ���������߽ڵ�Ӧ��
// #define  CMD_DPPUSH      12 //�������߽ڵ����ͣ�����Ӧ��
// #define  CMD_DPPOST      13 //�������߽ڵ�Ͷ�ݣ�Ҫ����յ����������߽ڵ�Ӧ��		
		if (rcvdata.data->head.cCmd == CMD_DPBC || rcvdata.data->head.cCmd == CMD_DPABC ) //�㲥������
		{
			if (m_pRes->g_nBroadCastMode != 0)
			{
				if (m_pRes->g_nBroadCastMode == 1)
				{
					//��ֹ����
					if (m_pRes->g_lBCFuncList.Find(rcvdata.data->head.d_Dinfo.d_nServiceNo))
					{
						m_pMemPool->PoolFree(rcvdata.data);
						rcvdata.data = NULL;
						return;
					}
				}
				else
				{	
					//������
					if (!m_pRes->g_lBCFuncList.Find(rcvdata.data->head.d_Dinfo.d_nServiceNo))
					{
						m_pMemPool->PoolFree(rcvdata.data);
						rcvdata.data = NULL;
						return;
					}
				}
			}

			//���������Ƿ������µģ�������ֱ�Ӷ���������ֱ��������������ݶ������
			data.isBC = 1;
			data.index = 0;
			data.timestamp = 0;
			//��������Ϊ������Ϣ

			//��̨�͹����Ľ�����9999999Ϊ��������
			if ( 9999999 == rcvdata.data->head.d_Dinfo.d_nServiceNo)
			{
				//�ж������Ƿ�Ϊ����
				//rcvdata.data->head.s_Sinfo.s_nHook Ϊ��Լ���
				//rcvdata.data->head.s_Sinfo.s_nSerial Ϊ��������
				//rcvdata.data->head.s_Sinfo.s_nGateIndex Ϊ�������
				data.nVerietyId = rcvdata.data->head.s_Sinfo.s_nHook;
				ProcessQuota(rcvdata,data);
			}
			else
			{ 
				if ( 9999998 == rcvdata.data->head.d_Dinfo.d_nServiceNo)
				{
					data.nVerietyId = rcvdata.data->head.s_Sinfo.s_nHook;
				}
				m_pSendData->PushData(data,2);
				m_pMemPool->PoolFree(rcvdata.data);
				rcvdata.data = NULL;
			}
			return;
		}
		else  if (rcvdata.data->head.cCmd == CMD_DPCALL || rcvdata.data->head.cCmd == CMD_DPACALL || \
			rcvdata.data->head.cCmd == CMD_DPPUSH || rcvdata.data->head.cCmd == CMD_DPPOST )
		//˽������ָ�����棬�ɽ��ر���ί�лر�
		{
			if (data.index == 0)
			{
				m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"��Ч������ index[%d] cCmd[%d]",data.index,data.data.head.stComm.cCmd);
				m_pMemPool->PoolFree(rcvdata.data);
				rcvdata.data = NULL;
				return;
			}
			//Դ��nGateIndex�����û���¼����������   s_nHook�������ӵ�ʱ���
			data.isBC = 0;
			//�ڶ�����Ϊ�ر���Ϣ  
			m_pSendData->PushData(data,1);
//			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"���Ͷ�������Ŀ[%d]",m_pSendData->GetSize());

			m_pMemPool->PoolFree(rcvdata.data);
			rcvdata.data = NULL;
			return;

		}
		m_pMemPool->PoolFree(rcvdata.data);
		
		rcvdata.data = NULL;
		return;	
	}
	else  //��DREB��Ӧ��
	{
		if (rcvdata.data->head.cCmd == CMD_MONITOR_BPC)
		{
			m_pMemPool->PoolFree(rcvdata.data);
			rcvdata.data = NULL;
			return;
		}
//		PS_GATE_SERIAL gserial = (PS_GATE_SERIAL)&(rcvdata.data->head.s_Sinfo.s_nSerial);

		data.data.head.nRetCode = rcvdata.data->head.a_Ainfo.a_nRetCode;
		data.data.head.stComm.cCmd = rcvdata.data->head.cCmd;
		data.data.head.stComm.cMsgType = MSG_GATEREQ;
		data.data.head.stComm.cNextFlag = rcvdata.data->head.cNextFlag;
		data.data.head.stComm.cRaflag = rcvdata.data->head.cRaflag;
		data.data.head.stComm.cZip = rcvdata.data->head.cZip;
		data.data.head.stComm.nCrc = 0;
		data.data.head.stDest.cNodePrivateId = rcvdata.data->head.d_Dinfo.d_cNodePrivateId;
		data.data.head.stDest.cSvrPrivateId = rcvdata.data->head.d_Dinfo.d_cSvrPrivateId;
		data.data.head.stDest.nNodeId = rcvdata.data->head.d_Dinfo.d_nNodeId;
		data.data.head.stDest.nSerial = rcvdata.data->head.s_Sinfo.s_nSerial;
		data.data.head.stDest.nServiceNo = rcvdata.data->head.d_Dinfo.d_nServiceNo;
		data.data.head.stDest.nSvrMainId = rcvdata.data->head.d_Dinfo.d_nSvrMainId;
		data.data.head.stNext.n_nNextNo = rcvdata.data->head.n_Ninfo.n_nNextNo;
		data.data.head.stNext.n_nNextOffset = rcvdata.data->head.n_Ninfo.n_nNextOffset;
		
		data.d_nNodeId = rcvdata.data->head.s_Sinfo.s_nNodeId;
		data.d_cNodePrivateId = rcvdata.data->head.s_Sinfo.s_cNodePrivateId;
		data.s_nDrebSerial = rcvdata.data->head.s_Sinfo.s_nDrebSerial;
		data.rtime = time(NULL);

		data.index = rcvdata.data->head.s_Sinfo.s_nGateIndex;
		data.timestamp = rcvdata.data->head.s_Sinfo.s_nHook;
		if (data.index == 0 && rcvdata.data->head.s_Sinfo.s_nHook == 0)
		{
			m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"��Ч��Ӧ��");
			m_pMemPool->PoolFree(rcvdata.data);
			rcvdata.data = NULL;
			return;
		}
		data.isBC = 0;
		//��������ӦӦ������
     	//ͳһ��һ���̼߳��ܲ����뷢�Ͷ���
		m_pSendData->PushData(data,0);
//		m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"���Ͷ�����Ŀ[%d]",m_pSendData->GetSize());
	

		m_pMemPool->PoolFree(rcvdata.data);
		rcvdata.data = NULL;
		return;
		
	}
	m_pMemPool->PoolFree(rcvdata.data);
	rcvdata.data = NULL;
	return;
}


void CLinkThread::CallBpcLocalFunc(S_GATEDREB_RSMSG &rcvdata)
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
	switch (rcvdata.data->head.d_Dinfo.d_nServiceNo)
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
		case BPCMONI_DOWNLOAD:
			Download(rcvdata);
			break;
		case BPCMONI_UPLOAD:
			Upload(rcvdata);
			break;
		default:
			rcvdata.data->head.cRaflag = 1;
			rcvdata.data->head.cZip = 0;
			rcvdata.data->head.a_Ainfo.a_nRetCode = ERR_FUNCNOTFUND;
			rcvdata.data->head.nLen = 0;
			m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
			break;
	}
	

}


void CLinkThread::LogDrebHead(int loglevel, DREB_HEAD head, const char *msg, const char *filename, int fileline)
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
	m_pLog->LogMp(loglevel,filename,fileline,m_headMsg,msg);
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
		char zipDataBuf[BPUDATASIZE];
		bzero(zipDataBuf,sizeof(zipDataBuf));
		int ddlen;
		switch (data->head.cZip)
		{
			case 1:
				ddlen = data->head.nLen;
				if (m_pDes.UncryptString(data->buffer,ddlen,DESENCRYPTKEY)<1)
				{
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
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
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
					return false;
				}
				data->head.nLen = ddlen;
				data->head.cZip =0;
				break;
			case 4:
				ddlen = data->head.nLen ;
				if (m_pDes.UncryptString(data->buffer,ddlen,DESENCRYPTKEY3)<1)
				{
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
					return false;
				}
				data->head.nLen = ddlen;
				data->head.cZip =2;
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
	m_pTimer.Join();
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
//	m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"��ض�ʱ��");
	m_bIsPingTimer = true;
}

void CLinkThread::OnMonitor()
{
	//�����Ϣ
	int i;
	CBF_Xml        pXml;
	pXml.SetNodeValueByPath("Monitor/�����ڵ�",false,(int)m_pRes->g_nSvrMainId);
	pXml.SetNodeValueByPath("Monitor/˽�нڵ�",false,(int)m_pRes->g_nSvrPrivateId);
	pXml.SetNodeValueByPath("Monitor/�ڵ�����",false,2);//��bpc
	pXml.SetNodeValueByPath("Monitor/��������",false,0);//����
	pXml.SetNodeValueByPath("Monitor/����ʱ��",false,m_pRes->m_sStartDate,false);
	
	int nConntotal=0;
	int nConnnum = m_pAio->GetConnectPoolInfo(nConntotal);

	pXml.SetNodeAttribute("Monitor/���ӳ�",false,"���������",false,nConntotal );
	pXml.SetNodeAttribute("Monitor/���ӳ�",false,"����������",false,nConnnum );
	pXml.SetNodeAttribute("Monitor/���ӳ�",false,"�ͻ�������",false,nConntotal-nConnnum-1 );
	char extdata[300];
	sprintf(extdata,"���ӳ����������:%d ����������%d �ͻ�������%d",nConntotal,nConnnum,nConntotal-nConnnum-1);
	m_pMemPool->PoolInfo(nConntotal,nConnnum);
	pXml.SetNodeAttribute("Monitor/�ڴ滺���",false,"�ܷ���",false,nConntotal );
	pXml.SetNodeAttribute("Monitor/�ڴ滺���",false,"δʹ��",false,nConnnum );
	pXml.SetNodeAttribute("Monitor/�ڴ滺���",false,"ÿ���С",false,(int)DREBBUFSIZE );

	
	//ȡ������Ϣ
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
	pXml.SetNodeValueByPath("Monitor/��չ��Ϣ/cgate",false,extdata,false);
	pXml.SetNodeAttribute("Monitor/��չ��Ϣ/cgate",false,"status",false,0);

	S_GATEDREB_RSMSG drebdata;
	bzero(&drebdata,sizeof(S_GATEDREB_RSMSG));
	drebdata.data = (PCOMMSTRU)m_pMemPool->PoolMalloc();
	if (drebdata.data == NULL) 
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"PoolMalloc����");
		return;
	}
	
	drebdata.rtime = time(NULL);
	bzero(drebdata.data,sizeof(COMMSTRU));
	int len = DREBDATASIZE;
	if (!pXml.ToBuffer(drebdata.data->buffer,len))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ToBuffer����");
		m_pMemPool->PoolFree(drebdata.data);
		drebdata.data = NULL;
		return;
	}
	m_pLog->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"�����������",drebdata.data->buffer,len);
	
	//����ͷ
	drebdata.data->head.nLen = len;
	drebdata.data->head.cCmd = CMD_DPPUSH;
	drebdata.data->head.cZip = 0;
	drebdata.data->head.cDrebAffirm = 0;
	drebdata.data->head.cNextFlag = 0; //�Ƿ���ȡ������
	drebdata.data->head.cRaflag = 0;//����Ӧ���־
	
	drebdata.data->head.d_Dinfo.d_cNodePrivateId = m_pRes->g_nMonitorDrebPrivateId;
	drebdata.data->head.d_Dinfo.d_cSvrPrivateId = m_pRes->g_nMonitorSvrPrivateId;
	drebdata.data->head.d_Dinfo.d_nNodeId = m_pRes->g_nMonitorDrebId;
	drebdata.data->head.d_Dinfo.d_nServiceNo = m_pRes->g_nMonitorTxCode;
	drebdata.data->head.d_Dinfo.d_nSvrMainId = m_pRes->g_nMonitorSvrId;
		
	//������У�����Ϣ�߳�������
	m_pPoolData->PushData(drebdata);
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"������[%d] �ܷ����ڴ��[%d] δʹ���ڴ��[%d]",\
		m_pPoolData->GetSize(),nConntotal,nConnnum);

	return;
}

void CLinkThread::MonitorInfo(S_GATEDREB_RSMSG &rcvdata)
{
	int i;
	CBF_Xml xmlpack;
	xmlpack.SetNodeValueByPath("Monitor/BPC�����ڵ�",false,(int)m_pRes->g_nSvrMainId);
	xmlpack.SetNodeValueByPath("Monitor/BPC˽�нڵ�",false,(int)m_pRes->g_nSvrPrivateId);
	xmlpack.SetNodeValueByPath("Monitor/����ʱ��",false,m_pRes->m_sStartDate,false);
	int nConntotal=0;
	int nConnnum = m_pAio->GetConnectPoolInfo(nConntotal);
	
	xmlpack.SetNodeAttribute("Monitor/���ӳ�",false,"���������",false,nConntotal );
	xmlpack.SetNodeAttribute("Monitor/���ӳ�",false,"����������",false,nConnnum );
	xmlpack.SetNodeAttribute("Monitor/���ӳ�",false,"�ͻ�������",false,nConntotal-nConnnum-1 );
	
	m_pMemPool->PoolInfo(nConntotal,nConnnum);
	xmlpack.SetNodeAttribute("Monitor/�ڴ滺���",false,"�ܷ���",false,nConntotal );
	xmlpack.SetNodeAttribute("Monitor/�ڴ滺���",false,"δʹ��",false,nConnnum );
	xmlpack.SetNodeAttribute("Monitor/�ڴ滺���",false,"ÿ���С",false,(int)DREBBUFSIZE );
	//������Ϣ
	for (i=0 ; i < m_pRes->g_vDrebLinkInfo.size() ; i++)
	{
		CXmlNode *connnode = xmlpack.AddNodeByPath("Monitor/������Ϣ",false,"����",false,"");
		if (connnode == NULL)
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"AddNodeByPath ����");
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
	
	//BPU��Ϣ

	
	int len = DREBDATASIZE;
	if (!xmlpack.ToBuffer(rcvdata.data->buffer,len))
	{
		sprintf(rcvdata.data->buffer,"���岻�㣬������cgateinfo.txt�鿴");
		rcvdata.data->head.nLen = strlen(rcvdata.data->buffer);
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���岻��");
		xmlpack.ToFile("cgateinfo.txt");
	}
	else
	{
		rcvdata.data->head.nLen = len;
	}
	rcvdata.data->head.cRaflag = 1;
	rcvdata.data->head.cZip = 0;
	rcvdata.data->head.a_Ainfo.a_nRetCode = SUCCESS;
	m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
}

void CLinkThread::MonitorHost(S_GATEDREB_RSMSG &rcvdata)
{
	unsigned int i;
	CBF_Xml xmlpack;
	xmlpack.SetNodeValueByPath("Monitor/BPC�����ڵ�",false,(int)m_pRes->g_nSvrMainId);
	xmlpack.SetNodeValueByPath("Monitor/BPC˽�нڵ�",false,(int)m_pRes->g_nSvrPrivateId);
	xmlpack.SetNodeValueByPath("Monitor/����ʱ��",false,m_pRes->m_sStartDate,false);
	int nConntotal=0;
	int nConnnum = m_pAio->GetConnectPoolInfo(nConntotal);
	
	xmlpack.SetNodeAttribute("Monitor/���ӳ�",false,"���������",false,nConntotal );
	xmlpack.SetNodeAttribute("Monitor/���ӳ�",false,"����������",false,nConnnum );
	xmlpack.SetNodeAttribute("Monitor/���ӳ�",false,"�ͻ�������",false,nConntotal-nConnnum-1 );
	
	m_pMemPool->PoolInfo(nConntotal,nConnnum);
	xmlpack.SetNodeAttribute("Monitor/�ڴ滺���",false,"�ܷ���",false,nConntotal );
	xmlpack.SetNodeAttribute("Monitor/�ڴ滺���",false,"δʹ��",false,nConnnum );
	xmlpack.SetNodeAttribute("Monitor/�ڴ滺���",false,"ÿ���С",false,(int)DREBBUFSIZE );


	//ȡ������Ϣ
	if (m_pRes->g_nMonitorHost == 1)
	{
		S_MONITOR_HOST hostinfo;
		V_MonitorDiskInfo  diskinfo;
		g_pBpcTime.GetHostInfo(hostinfo,diskinfo);
		xmlpack.SetNodeValueByPath("Monitor/������Դ/CPU",false,(int)hostinfo.nCpuRate);
		xmlpack.SetNodeAttribute("Monitor/������Դ/�ڴ�",false,"total",false,(int)hostinfo.nTotalMemory);
		xmlpack.SetNodeAttribute("Monitor/������Դ/�ڴ�",false,"used",false,(int)hostinfo.nUsedMemory);
		for (i=0 ; i< diskinfo.size() ; i++)
		{
			CXmlNode *disknode = xmlpack.AddNodeByPath("Monitor/������Դ/����",false,"����",false,"");
			if (disknode != NULL)
			{
				disknode->SetAttribute("drive",false,diskinfo[i].sDiskName,true);
				disknode->SetAttribute("total",false,(int)diskinfo[i].nTotalSpace);
				disknode->SetAttribute("used",false,(int)diskinfo[i].nUsedSpace);
			}
		}
	}
	int len = DREBDATASIZE;
	if (!xmlpack.ToBuffer(rcvdata.data->buffer,len))
	{
		sprintf(rcvdata.data->buffer,"���岻�㣬������cgatehost.txt�鿴");
		rcvdata.data->head.nLen = strlen(rcvdata.data->buffer);
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���岻��");
		xmlpack.ToFile("cgatehost.txt");
	}
	else
	{
		rcvdata.data->head.nLen = len;
	}
	rcvdata.data->head.cRaflag = 1;
	rcvdata.data->head.cZip = 0;
	rcvdata.data->head.a_Ainfo.a_nRetCode = SUCCESS;
	m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
}


void CLinkThread::MonitorReport(S_GATEDREB_RSMSG &rcvdata)
{
	CBF_Xml xmlpack;
	xmlpack.SetNodeValueByPath("Monitor/BPC�����ڵ�",false,(int)m_pRes->g_nSvrMainId);
	xmlpack.SetNodeValueByPath("Monitor/BPC˽�нڵ�",false,(int)m_pRes->g_nSvrPrivateId);
	xmlpack.SetNodeValueByPath("Monitor/����ʱ��",false,m_pRes->m_sStartDate,false);
	int nConntotal=0;
	int nConnnum = m_pAio->GetConnectPoolInfo(nConntotal);
	
	xmlpack.SetNodeAttribute("Monitor/���ӳ�",false,"���������",false,nConntotal );
	xmlpack.SetNodeAttribute("Monitor/���ӳ�",false,"����������",false,nConnnum );
	xmlpack.SetNodeAttribute("Monitor/���ӳ�",false,"�ͻ�������",false,nConntotal-nConnnum-1 );
	
	m_pMemPool->PoolInfo(nConntotal,nConnnum);
	xmlpack.SetNodeAttribute("Monitor/�ڴ滺���",false,"�ܷ���",false,nConntotal );
	xmlpack.SetNodeAttribute("Monitor/�ڴ滺���",false,"δʹ��",false,nConnnum );
	xmlpack.SetNodeAttribute("Monitor/�ڴ滺���",false,"ÿ���С",false,(int)DREBBUFSIZE );
	
	char tmpchar[100];
	sprintf(tmpchar,INT64PFM,m_pRes->g_nSendByteLen);
	xmlpack.SetNodeValueByPath("Monitor/�����ֽ���",false,tmpchar,false);
	sprintf(tmpchar,INT64PFM,m_pRes->g_nRecvByteLen);
	xmlpack.SetNodeValueByPath("Monitor/�����ֽ���",false,tmpchar,false);
	int len = DREBDATASIZE;
	if (!xmlpack.ToBuffer(rcvdata.data->buffer,len))
	{
		sprintf(rcvdata.data->buffer,"���岻�㣬������cgatereport.txt�鿴");
		rcvdata.data->head.nLen = strlen(rcvdata.data->buffer);
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���岻��");
		xmlpack.ToFile("cgatereport.txt");
	}
	else
	{
		rcvdata.data->head.nLen = len;
	}
	rcvdata.data->head.cRaflag = 1;
	rcvdata.data->head.cZip = 0;
	rcvdata.data->head.a_Ainfo.a_nRetCode = SUCCESS;
	m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
}

void CLinkThread::MonitorBpu(S_GATEDREB_RSMSG &rcvdata)
{
	int i;
	CBF_Xml xmlpack;
	xmlpack.SetNodeValueByPath("Monitor/BPC�����ڵ�",false,(int)m_pRes->g_nSvrMainId);
	xmlpack.SetNodeValueByPath("Monitor/BPC˽�нڵ�",false,(int)m_pRes->g_nSvrPrivateId);
	xmlpack.SetNodeValueByPath("Monitor/����ʱ��",false,m_pRes->m_sStartDate,false);
	int nConntotal=0;
	int nConnnum =0;
	m_pMemPool->PoolInfo(nConntotal,nConnnum);
	xmlpack.SetNodeAttribute("Monitor/�ڴ滺���",false,"�ܷ���",false,nConntotal );
	xmlpack.SetNodeAttribute("Monitor/�ڴ滺���",false,"δʹ��",false,nConnnum );
	xmlpack.SetNodeAttribute("Monitor/�ڴ滺���",false,"ÿ���С",false,(int)DREBBUFSIZE );

	nConnnum = m_pAio->GetConnectPoolInfo(nConntotal);
	xmlpack.SetNodeAttribute("Monitor/���ӳ�",false,"���������",false,nConntotal );
	xmlpack.SetNodeAttribute("Monitor/���ӳ�",false,"����������",false,nConnnum );
	xmlpack.SetNodeAttribute("Monitor/���ӳ�",false,"�ͻ�������",false,nConntotal-nConnnum-1 );
	
	

	xmlpack.SetNodeValueByPath("Monitor/�����б�",false,"",false);
	char tmpchar[100];
	for ( i=1 ; i<nConntotal ; i++ )
	{
		PSOCKET_POOL_DATA info = m_pAio->GetSocketByIndex(i);
		if (info == NULL)
		{
			continue;
		}
		if (info->s_hSocket != INVALID_SOCKET)
		{
			sprintf(tmpchar,"%s",inet_ntoa(info->s_pSocketAddr.sin_addr));
			CXmlNode *disknode = xmlpack.AddNodeByPath("Monitor/�����б�",false,tmpchar,false,"");
			if (disknode != NULL)
			{
				disknode->SetAttribute("index",false,i);
				disknode->SetAttribute("socket",false,(int)info->s_hSocket);
				disknode->SetAttribute("flag",false,(int)info->s_cCheckFlag);
			}
		}
	}
	int len = DREBDATASIZE;
	if (!xmlpack.ToBuffer(rcvdata.data->buffer,len))
	{
		sprintf(rcvdata.data->buffer,"���岻�㣬������clientlist.txt�鿴");
		rcvdata.data->head.nLen = strlen(rcvdata.data->buffer);
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���岻��");
		xmlpack.ToFile("clientlist.txt");
	}
	else
	{
		rcvdata.data->head.nLen = len;
	}
	rcvdata.data->head.cRaflag = 1;
	rcvdata.data->head.cZip = 0;
	rcvdata.data->head.a_Ainfo.a_nRetCode = SUCCESS;
	m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
}

void CLinkThread::Download(S_GATEDREB_RSMSG &rcvdata)
{
	char databuf[DREBDATASIZE];
	
	if (rcvdata.data->head.cZip == 2)
	{
		memcpy(databuf,rcvdata.data->buffer,rcvdata.data->head.nLen);
		unsigned int dlen = DREBDATASIZE;
		if (!CBF_Tools::Uncompress((unsigned char *)rcvdata.data->buffer,dlen,(unsigned char *)databuf,rcvdata.data->head.nLen))
		{
			sprintf(rcvdata.data->buffer,"���ݽ�ѹ������");
			rcvdata.data->head.nLen = strlen(rcvdata.data->buffer);
			rcvdata.data->head.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
			rcvdata.data->head.cRaflag = 1;
			rcvdata.data->head.cZip = 0;
			m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
			return ;
		}
	}
	S_BPC_DUFILE sfile;
	if (rcvdata.data->head.nLen  < sizeof(S_BPC_DUFILE))
	{
		m_pLog->LogBin(LOG_ERROR,__FILE__,__LINE__,"�������ݷ���������",\
			rcvdata.data->buffer,rcvdata.data->head.nLen);
		sprintf(rcvdata.data->buffer,"�������ݷ���������");
		rcvdata.data->head.nLen = strlen(rcvdata.data->buffer);
		rcvdata.data->head.a_Ainfo.a_nRetCode = ERR_MONITOR_DOWNLOAD;
		rcvdata.data->head.cRaflag = 1;
		rcvdata.data->head.cZip = 0;
		m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
		return ;
	}
	memcpy(&sfile,rcvdata.data->buffer,sizeof(S_BPC_DUFILE));
	struct_stat fstat;
	FSTAT(sfile.sFileName,&fstat);
	if (fstat.st_size <= 0)
	{
		sprintf(rcvdata.data->buffer,"�ļ�[%s]������",sfile.sFileName);
		rcvdata.data->head.nLen = strlen(rcvdata.data->buffer);
		rcvdata.data->head.a_Ainfo.a_nRetCode = ERR_MONITOR_DOWNLOAD;
		rcvdata.data->head.cRaflag = 1;
		rcvdata.data->head.cZip = 0;
		m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
		return ;
	}
	int offset = atol(sfile.sFileOffset);
	if (offset >= fstat.st_size)
	{
		sprintf(rcvdata.data->buffer,"�ļ�ƫ����[%d]�Ѵ��ڵ����ļ���С",offset);
		rcvdata.data->head.nLen = strlen(rcvdata.data->buffer);
		rcvdata.data->head.a_Ainfo.a_nRetCode = ERR_MONITOR_DOWNLOAD;
		rcvdata.data->head.cRaflag = 1;
		rcvdata.data->head.cZip = 0;
		m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
		return ;
	}
	sprintf(sfile.sFileSize,"%011ld",fstat.st_size);
	FILE *fp = fopen(sfile.sFileName,"rb");
	if (fp == NULL)
	{
		sprintf(rcvdata.data->buffer,"�ļ�[%s]������",sfile.sFileName);
		rcvdata.data->head.nLen = strlen(rcvdata.data->buffer);
		rcvdata.data->head.a_Ainfo.a_nRetCode = ERR_MONITOR_DOWNLOAD;
		rcvdata.data->head.cRaflag = 1;
		rcvdata.data->head.cZip = 0;
		m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
		return ;
	}
	fseek(fp,offset,1);
	int getlen = fread(rcvdata.data->buffer+sizeof(S_BPC_DUFILE),1,50*1024,fp);
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
	memcpy(rcvdata.data->buffer,&sfile,sizeof(S_BPC_DUFILE));
	rcvdata.data->head.nLen = sizeof(S_BPC_DUFILE)+getlen;
	
	rcvdata.data->head.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.data->head.cRaflag = 1;
	rcvdata.data->head.cZip = 0;
	m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);

}

void CLinkThread::Upload(S_GATEDREB_RSMSG &rcvdata)
{
	char databuf[DREBDATASIZE];
	if (rcvdata.data->head.cZip == 2)
	{
		memcpy(databuf,rcvdata.data->buffer,rcvdata.data->head.nLen);
		unsigned int dlen = DREBDATASIZE;
		if (!CBF_Tools::Uncompress((unsigned char *)rcvdata.data->buffer,dlen,(unsigned char *)databuf,rcvdata.data->head.nLen))
		{
			sprintf(rcvdata.data->buffer,"���ݽ�ѹ������");
			rcvdata.data->head.nLen = strlen(rcvdata.data->buffer);
			rcvdata.data->head.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
			rcvdata.data->head.cRaflag = 1;
			rcvdata.data->head.cZip = 0;
			m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
			return ;
		}
	}
	S_BPC_DUFILE sfile;
	if (rcvdata.data->head.nLen  <= sizeof(S_BPC_DUFILE))
	{
		m_pLog->LogBin(LOG_ERROR,__FILE__,__LINE__,"�������ݷ��ϴ�����",\
			rcvdata.data->buffer,rcvdata.data->head.nLen);
		sprintf(rcvdata.data->buffer,"�������ݷ��ϴ�����");
		rcvdata.data->head.nLen = strlen(rcvdata.data->buffer);
		rcvdata.data->head.a_Ainfo.a_nRetCode = ERR_MONITOR_UPLOAD;
		rcvdata.data->head.cRaflag = 1;
		rcvdata.data->head.cZip = 0;
		m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
		return ;
	}
	memcpy(&sfile,rcvdata.data->buffer,sizeof(S_BPC_DUFILE));
	m_pLog->LogBin(LOG_DEBUG,__FILE__,__LINE__,"�ϴ��ļ�",rcvdata.data->buffer,sizeof(S_BPC_DUFILE));
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
		sprintf(rcvdata.data->buffer,"�ļ�[%s]�򿪳���",sfile.sFileName);
		rcvdata.data->head.nLen = strlen(rcvdata.data->buffer);
		rcvdata.data->head.a_Ainfo.a_nRetCode = ERR_MONITOR_UPLOAD;
		rcvdata.data->head.cRaflag = 1;
		rcvdata.data->head.cZip = 0;
		m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
		return ;
	}
	fwrite(rcvdata.data->buffer+sizeof(S_BPC_DUFILE),1,rcvdata.data->head.nLen-sizeof(S_BPC_DUFILE),fp);
	fclose(fp);
	if (sfile.cEndFlag == '1')
	{
		struct_stat fstat;
		FSTAT(sfile.sFileName,&fstat);
		if (atol(sfile.sFileSize) != fstat.st_size)
		{
			sprintf(rcvdata.data->buffer,"�ϴ��ļ�[%s]ʧ�ܣ���С[%d]������[%s]",sfile.sFileName,fstat.st_size,sfile.sFileSize);
			rcvdata.data->head.nLen = strlen(rcvdata.data->buffer);
			rcvdata.data->head.a_Ainfo.a_nRetCode = ERR_MONITOR_UPLOAD;
			rcvdata.data->head.cRaflag = 1;
			rcvdata.data->head.cZip = 0;
		m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
			return ;
		}
		sprintf(rcvdata.data->buffer,"�ϴ��ļ�[%s]�ɹ�",sfile.sFileName);
		rcvdata.data->head.nLen = strlen(rcvdata.data->buffer);
		rcvdata.data->head.a_Ainfo.a_nRetCode = SUCCESS;
		rcvdata.data->head.cRaflag = 1;
		rcvdata.data->head.cZip = 0;
		m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
		return ;
	}
	else
	{
		sprintf(rcvdata.data->buffer,"д���ļ�[%s]�ɹ�",sfile.sFileName);
		rcvdata.data->head.nLen = strlen(rcvdata.data->buffer);
		rcvdata.data->head.a_Ainfo.a_nRetCode = SUCCESS;
		rcvdata.data->head.cRaflag = 1;
		rcvdata.data->head.cZip = 0;
		m_pSockMgr->at(rcvdata.nIndex)->Send(rcvdata);
		return ;
	}
}

void CLinkThread::ProcessQuota(S_GATEDREB_RSMSG &rcvdata, S_CGATE_SMSG &data)
{
	S_CGATE_QUOTATION quo;
	quo.nVerietyId = rcvdata.data->head.s_Sinfo.s_nHook;
	quo.nDate = rcvdata.data->head.s_Sinfo.s_nSerial;
	//quo.nSerial = rcvdata.data->head.n_Ninfo.n_nNextNo;
	//n_nNextNo��short���ͣ������û�s_nGateIndex�ֶΰ�
	quo.nSerial =  rcvdata.data->head.s_Sinfo.s_nGateIndex;
	
	if (m_pQuotation.IsNewQuotation(quo) >0 )
	{
		//������У�������
		m_pSendData->PushData(data,2);
		
	}
	else
	{
		m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"����[%d %d %d]�������£�����",quo.nVerietyId,quo.nDate,quo.nSerial);
	}
	m_pMemPool->PoolFree(rcvdata.data);
	rcvdata.data = NULL;
}
