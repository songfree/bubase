#include "GateLink.h"



CGateLink::CGateLink()
{
	m_bIsRunThread = false;
	m_nIsConnect = 0;
}
CGateLink::~CGateLink()
{
	m_bIsRunThread = false;
}

// ������: Logout
// ���  : ������ 2013-5-23 15:20:54
// ����  : void 
// ����  : �ر����ӣ��˳��߳�
void CGateLink::Logout()
{
	m_bIsRunThread = false;
	//�ȴ��߳��˳�
	Join();
	m_pApiSocket.OnClose(__FILE__,__LINE__,"UI�����ر����ӣ��˳��߳�",LOG_PROMPT);

}


// ������: InitApi
// ���  : ������ 2013-5-23 14:49:22
// ����  : bool true�ɹ�
// ����  : const char *apixml xml�����ļ�
// ����  : ��ʼ��api����ȡ�����ļ����õ�֤��Ŀ¼�����ص�ַ�˿�
bool CGateLink::InitApi(const char *apixml)
{
	//��ʼ������
	if (!m_pRes.Init(apixml))
	{
		sprintf(m_sErrMsg,m_pRes.GetMsg().c_str());
		return false;
	}
	m_pApiSocket.SetPara(&m_pRes);
	return true;
}

	
// ������: ExitThreadInstance
// ���  : ������ 2013-5-27 10:01:19
// ����  : virtual void 
// ����  : �˳��߳�
void CGateLink::ExitThreadInstance()
{

}

// ������: InitThreadInstance
// ���  : ������ 2013-5-27 10:01:23
// ����  : virtual bool 
// ����  : ��ʼ�߳�
bool CGateLink::InitThreadInstance()
{
	return true;
}


// ������: Run
// ���  : ������ 2013-5-23 15:26:51
// ����  : virtual int 
// ����  : �߳�������
int CGateLink::Run()
{
	struct timeval tv;
	int result;
	int maxfd=0;
	int ret;
	m_pRes.g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,"���� api select�߳�");
	while (m_bIsRunThread)
	{
		FD_ZERO(&m_rset);
		FD_ZERO(&m_wset);
#if defined(_WINDOWS)
		FD_ZERO(&m_eset);

		tv.tv_sec = 1;
		tv.tv_usec = 0;
#else
        tv.tv_sec = 0;
        tv.tv_usec = 50;
#endif
		SOCKET_HANDLE tmpscoket = m_pApiSocket.GetSocket(); 
		if ( m_bIsRunThread &&  tmpscoket != INVALID_SOCKET)
		{
			FD_SET(tmpscoket, &m_rset);//���뵽��select����
// 			if (m_pApiSocket.NeedSend()) //��Ҫдʱ
// 			{
// 				FD_SET(m_pApiSocket.GetSocket(), &m_wset);//���뵽дselect����
// 			}
		}
		else
		{
			m_pRes.g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,"���ӶϿ�,ֹͣapi select�߳�");
			m_bIsRunThread = false;
			m_nIsConnect = 0;
			return 0;
		}
		
		maxfd = tmpscoket;
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
		if (result == 0) //���¼�����,��������
		{
			OnNoEvent();
		}
		else
		{
			OnEvent();
			OnNoEvent();
		}
		//SLEEP(1);
	}
	m_pRes.g_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"���ӶϿ�,ֹͣapi select�߳�");
	m_bIsRunThread = false;
	m_nIsConnect = 0;
	return 0;
}

//-1����ʧ��   0δ����   1���ӳɹ�   
int CGateLink::ConnectGate()
{
//	CBF_PMutex pp(&m_mutex);
	m_pApiSocket.OnClose(__FILE__,__LINE__,"�ͻ��������ȹر����� ��ս��ն��У���ʼ��������",LOG_PROMPT);
	m_nIsConnect = 2;
	if (!m_pApiSocket.ConnectGate())
	{
		m_nIsConnect =0;
		return -1;
	}
	m_nIsConnect = 1;
	//����select�߳�
	if (IsStoped())
	{
		m_bIsRunThread = true;
		m_pRes.g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,"�������������߳�");
		CreateThread();
	}
	else
	{
		m_pRes.g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"���������߳�����������");
	}
	return 1;
}

void CGateLink::OnEvent()
{
	int ret;
	SOCKET_HANDLE tmpsocket= m_pApiSocket.GetSocket();
	ret = FD_ISSET(tmpsocket,&m_rset);
	if (ret != 0) //�ж��¼�
	{
		OnRecv();
	}
	if (tmpsocket == INVALID_SOCKET)
	{
		m_bIsRunThread = false;
		return;
	}
#if defined(_WINDOWS)
	ret = FD_ISSET(tmpsocket , &m_eset);
	if (ret != 0)
	{
		//�������ӵ�δ�ɹ�
	}
	else if (FD_ISSET(tmpsocket, &m_wset) !=0 )
	{
		
//		m_pApiSocket.OnSend();//��������
		
	}
	else  //socket��д�¼�
	{
//		m_pApiSocket.OnSend();//��������
	}
#else
	
	if (FD_ISSET(tmpsocket, &m_wset) !=0)
	{
//		m_pApiSocket.OnSend();//��������
	}
	else  
	{
//		m_pApiSocket.OnSend();//��������
	}
#endif

}

void CGateLink::OnNoEvent()
{
	int tmptime = time(NULL) ;//  - m_pApiSocket.m_nSendTime;
	int tmpwtime = tmptime -  m_pApiSocket.m_nSendTime;
	tmptime = tmptime - m_pApiSocket.m_nRcvTime;
	if ( tmptime > m_pRes.g_nHeartTime && tmptime < m_pRes.g_nCloseTime)
	{
		OnPing();
	}
	else if ( tmpwtime > m_pRes.g_nHeartTime && tmptime < m_pRes.g_nCloseTime)
	{
		OnPing();
	}
	else if (tmptime > m_pRes.g_nCloseTime)
	{
		m_pApiSocket.OnClose(__FILE__,__LINE__,"���ӳ�ʱ��δʹ�ã��Ͽ�",LOG_WARNNING);
	}

}
void CGateLink::OnPing()
{
	CGATE_COMMSTRU smsg;
	smsg.head.nLen = 0;
	smsg.head.nRetCode = 0;
	smsg.head.stComm.cCmd = 0;
	smsg.head.stComm.cMsgType = MSG_GATEPING;
	smsg.head.stComm.cNextFlag = 0;
	smsg.head.stComm.cRaflag = 0;
	smsg.head.stComm.cZip = 0;
	smsg.head.stComm.nCrc = 0;
	smsg.head.stDest.cNodePrivateId = 0;
	smsg.head.stDest.cSvrPrivateId = 0;
	smsg.head.stDest.nNodeId = 0;
	smsg.head.stDest.nSerial = 0;
	smsg.head.stDest.nServiceNo = 0;
	smsg.head.stDest.nSvrMainId = 0;
	
	//-1 δ�������� =0�������ƻ򻺳��� <0���ͳ���  1���Ͳ������� 2�������
	int nRet = m_pApiSocket.Send2Gate(&smsg);
	if (nRet == 2 )
	{
		m_pRes.g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,"����PING�������");
		return ;
	}
	else
	{
		m_pRes.g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"����PING����ʧ��");
		return ;
	}
	return ;
}
void CGateLink::OnRecv()
{
	int ret=0;
	int i=0;
	
//	m_pRes.g_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"׼����������");

	ret = m_pApiSocket.OnRcv();	
	if (ret<=0)
	{
		m_pApiSocket.OnClose(__FILE__,__LINE__,"Զ�˹رգ��������ݲ�����",LOG_WARNNING);
		return;
	}
//	m_pRes.g_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"׼���������� ����[%d]  �������ݳ���[%d]",ret,m_pApiSocket.GetRcvDataLen());
	while (m_pApiSocket.GetRcvDataLen() >= CGATEHEADLEN )
	{
		CGATE_COMMSTRU data;
		ret = m_pApiSocket.GetRecvData(&data);
		if (ret >0 )
		{
			//����CGATE����������
			if (data.head.stComm.cMsgType == MSG_GATEPING) //��ӦPINGӦ��
			{
//				m_pRes.g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"�յ����ط��͵�PING��Ϣ����ӦPING��Ϣ");
				data.head.stComm.cRaflag = 1;
				m_pApiSocket.Send2Gate(&data);
				continue; ;
			}
			ProcessMsg(data);
//			m_pRes.g_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"׼���������� �����������ݳ���[%d]",m_pApiSocket.GetRcvDataLen());
		}
		else
		{
			break;
		}
	}
	
	return ;
}

void CGateLink::ProcessMsg(CGATE_COMMSTRU &data)
{
	if (data.head.stComm.cMsgType == MSG_PUSH) //������Ϣ
	{
		m_pApiSocket.m_pReqQueue.PushData(data);
	}
	else if (data.head.stComm.cMsgType == MSG_GATEREQ || data.head.stComm.cMsgType == MSG_GATESUBSCRIBE) //����
	{
		m_pApiSocket.m_pAnsQueue.PushData(data);
	}
	else
	{
		m_pRes.g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"������Ϣ���Ͳ���[%s] %d",\
			m_pApiSocket.GetMsgType(data.head.stComm.cMsgType).c_str(),data.head.stDest.nServiceNo);
		m_pRes.g_pLog.LogBin(LOG_ERROR,__FILE__,__LINE__,"������Ϣ���Ͳ���",data.buffer,data.head.nLen);
	}
}
//���� 0δ����  1������   2��������
//ǰ�˵��ô˷���ʱ���������״̬Ϊ2ʱ����ѭ��sleep������Ƿ����Ӻã����ֱ�ӵ���connectgate���ܻ�������ӳ���
int CGateLink::GetConnectStatus()
{
	return m_nIsConnect;
}

// ������: GetReqData
// ���  : ������ 2013-5-23 15:12:42
// ����  : int -1���ӹر� =0��ʱ������  >0�ɹ�ȡ������
// ����  : PCGATE_COMMSTRU data   ��������
// ����  : int timeout ��ʱʱ����
// ����  : ȡ���ͻ�㲥����
int CGateLink::GetReqData(PCGATE_COMMSTRU data,int timeout)
{
	if (m_pApiSocket.m_pReqQueue.GetData(*data,timeout*1000))
	{
        m_pRes.g_pLog.LogBin(LOG_DEBUG, __FILE__, __LINE__, "���ͻ�㲥����",data->buffer,data->head.nLen);
		return m_pApiSocket.m_pReqQueue.GetSize()+1;
	}
	//����û�������ˣ��ж�����״̬
	if (!GetConnectStatus())
	{
		sprintf(m_sErrMsg,"����δ���� �㲥���Ͷ�����[%d]",m_pApiSocket.m_pReqQueue.GetSize());
		m_pRes.g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_sErrMsg);
		return -1;
	}
	return 0;
}

// ������: GetAnsData
// ���  : ������ 2013-5-23 15:14:47
// ����  : int =0��ʱ���� >0ȡ��Ӧ��ʣ����Ϊ��ֵ��1  -1���ӳ���
// ����  : PCGATE_COMMSTRU data   ��������  data->head.stDest.nSerialΪָ����ŵ�Ӧ��
// ����  : int timeout  ��ʱʱ���� 
// ����  : ȡ��ָ����ˮ��Ӧ��
int CGateLink::GetAnsData(PCGATE_COMMSTRU data,int timeout)
{
	unsigned int serial = data->head.stDest.nSerial;
	int btime = time(NULL);
	bool bRet= true;
	m_pRes.g_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ȴ�ȡӦ��  index[%d] ��ʶ[%d] ��ʱ[%d]�� time[%d]  Ӧ�������[%d]",\
		m_pApiSocket.m_nIndex,serial,timeout,btime ,m_pApiSocket.m_pAnsQueue.GetSize());
	while (bRet)
	{
		bRet = m_pApiSocket.m_pAnsQueue.GetData(*data,1000);
		if (bRet)
		{
			if (serial != 0) //����ָ����ŵ�Ӧ��
			{
				if (data->head.stDest.nSerial != serial)
				{
					if ((time(NULL) -btime) > timeout)
					{
						break;
					}
					m_pRes.g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"����Ӧ��ƥ��  �յ�index[%d] ��ʶ[%d]��Ӧ��,�ͱ�ʶ[%d]������ƥ��",\
						m_pApiSocket.m_nIndex,data->head.stDest.nSerial,serial);
					m_pRes.g_pLog.LogBin(LOG_WARNNING,__FILE__,__LINE__,"��ƥ�������",data->buffer,data->head.nLen);
					continue;
				}
			}
			
			m_pRes.g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,"�յ�Ӧ��  ���յ�index[%d] ��ʶ[%d]��Ӧ�� ��ʱ[%d]��",\
				m_pApiSocket.m_nIndex,m_nSerial,time(NULL)-btime);
			return m_pApiSocket.m_pAnsQueue.GetSize()+1;
		}
		else
		{
			if ((time(NULL) -btime) > timeout)
			{
				//��ʱ�˳�
				break;
			} 
			else
			{
				//����û�������ˣ��ж�����״̬
				if (m_pApiSocket.m_nSocketStatus != 1)
				{
					sprintf(m_sErrMsg,"����δ���� �㲥���Ͷ�����[%d]",m_pApiSocket.m_pReqQueue.GetSize());
					m_pRes.g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_sErrMsg);
					return -1;
				}
				bRet = true;
				SLEEP(1);
			}
		}
		
	}
	int etime = time(NULL);
	m_pRes.g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"���� δ�յ�Ӧ�� index[%d] ��ʶ[%d] ������[%d] ��ʱ[%d]�� time[%d] ��ʱ[%d]��",\
		m_pApiSocket.m_nIndex,serial,data->head.stDest.nServiceNo,timeout,etime,etime-btime);
	return 0;
}

// ������: SendReqData
// ���  : ������ 2013-5-23 15:12:27
// ����  : int -1δ����  -2���ͳ���  =0 ��ʱδ�յ�Ӧ�� 1�յ�Ӧ��
// ����  : PCGATE_COMMSTRU data   ����Ӧ������
// ����  : unsigned int timeout    ����Ӧ��ʱʱ��  <=0ֱ�ӷ��أ�������Ӧ��
// ����  : �������󲢽���Ӧ��
int CGateLink::SendReqData(PCGATE_COMMSTRU data,unsigned int timeout)
{
	if (GetConnectStatus() != 1)
	{
		sprintf(m_sErrMsg,"SendReqData ����δ���ӻ��������ӣ��������� index[%d] ��ʶ[%d] ������[%d]",\
			m_pApiSocket.m_nIndex,data->head.stDest.nSerial,data->head.stDest.nServiceNo);
		m_pRes.g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return -1;
	}
//	CBF_PMutex pp(&m_mutex);
	if (data->head.stComm.cNextFlag == 0)
	{
		data->head.nRetCode = 0;
		data->head.stComm.cMsgType = MSG_GATEREQ;
		data->head.stComm.cRaflag = 0;
		data->head.stComm.nCrc = 0;
		m_nSerial = data->head.stDest.nSerial;
		int nRet = m_pApiSocket.Send2Gate(data);
		if (nRet != 2 )
		{
			sprintf(m_sErrMsg,"SendReqData Send2Gate���� ��Ϣ����[MSG_GATEREQ]  index[%d] ��ʶ[%d]",\
				m_pApiSocket.m_nIndex,data->head.stDest.nSerial);
			m_pRes.g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return -1;
		}
		else
		{
			if (data->head.stComm.cCmd == CMD_DPABC || data->head.stComm.cCmd == CMD_DPPUSH )
			{
				//����������Ӧ��ֱ�ӷ���0��
				return 0;
			}
			if (timeout <=0)
			{
				return 0;
			}
			return GetAnsData(data,timeout);
		}
	}
	else
	{
		if (data->head.stComm.cNextFlag == 1 || data->head.stComm.cNextFlag == 10) //��ͣ����
		{
			return GetAnsData(data,timeout);
		}
		else //��������ȡ������
		{
			data->head.nRetCode = 0;
			data->head.stComm.cMsgType = MSG_GATEREQ;
			data->head.stComm.cRaflag = 0;
			data->head.stComm.nCrc = 0;
			data->head.nLen = 0;
			int nRet = m_pApiSocket.Send2Gate(data);
			if (nRet != 2 )
			{
				sprintf(m_sErrMsg,"����ȡ���������� index[%d] ��ʶ[%d]",m_pApiSocket.m_nIndex,data->head.stDest.nSerial);				
				m_pRes.g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
				return -2;
			}
			else
			{
				return GetAnsData(data,timeout);
			}
		}
	}
}


// ������: SendLogin
// ���  : ������ 2013-5-23 15:12:27
// ����  : int -1δ����  -2���ͳ���  =0 ��ʱδ�յ�Ӧ�� 1�յ�Ӧ��
// ����  : PCGATE_COMMSTRU data   ����Ӧ������
// ����  : unsigned int timeout    ����Ӧ��ʱʱ�� <=0ֱ�ӷ���0��������Ӧ��
// ����  : �������󲢽���Ӧ��
int CGateLink::SendLogin(PCGATE_COMMSTRU data,unsigned int timeout)
{
	if (GetConnectStatus() != 1)
	{
		sprintf(m_sErrMsg,"SendLogin ����δ���ӻ��������ӣ��������� index[%d] ��ʶ[%d] ������[%d]",\
			m_pApiSocket.m_nIndex,data->head.stDest.nSerial,data->head.stDest.nServiceNo);
		m_pRes.g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return -1;
	}
//	CBF_PMutex pp(&m_mutex);
	if (data->head.stComm.cNextFlag == 0)
	{
		data->head.nRetCode = 0;
		data->head.stComm.cMsgType = MSG_GATELOGIN;
		data->head.stComm.cRaflag = 0;
		data->head.stComm.nCrc = 0;
		m_nSerial = data->head.stDest.nSerial;
		int nRet = m_pApiSocket.Send2Gate(data);
		if (nRet != 2 )
		{
			sprintf(m_sErrMsg,"SendLogin Send2Gate���� ��Ϣ����[MSG_GATEREQ]  index[%d] ��ʶ[%d]",\
				m_pApiSocket.m_nIndex,data->head.stDest.nSerial);
				m_pRes.g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return -1;
		}
		else
		{
			if (timeout<=0)
			{
				return 0;
			}
			return GetAnsData(data,timeout);
		}
	}
	else
	{
		if (data->head.stComm.cNextFlag == 1 || data->head.stComm.cNextFlag == 10) //��ͣ����
		{
			return GetAnsData(data,timeout);
		}
		else //��������ȡ������
		{
			data->head.nRetCode = 0;
			data->head.stComm.cMsgType = MSG_GATEREQ;
			data->head.stComm.cRaflag = 0;
			data->head.stComm.nCrc = 0;
			data->head.nLen = 0;
			int nRet = m_pApiSocket.Send2Gate(data);
			if (nRet != 2 )
			{
				sprintf(m_sErrMsg,"SendLogin ����ȡ���������� index[%d] ��ʶ[%d]",m_pApiSocket.m_nIndex,data->head.stDest.nSerial);				
				m_pRes.g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
				return -2;
			}
			else
			{
				return GetAnsData(data,timeout);
			}
		}
	}
}

//int CGateLink::Subscribe(int flag, std::vector<int>& keylist)
//{
//	CGATE_COMMSTRU data;
//	bzero(&(data.head),CGATEHEADLEN);
//    S_GATE_SUBSCRIBE* subscribe = (S_GATE_SUBSCRIBE*)(data.buffer);
//    subscribe->flag = flag;
//    subscribe->datanum = keylist.size();
//	if (subscribe->datanum > 0)
//	{
//        subscribe->variety = keylist[0];
//        for (int i = 1; i < subscribe->datanum; i++)
//        {
//            int* key1 = (int*)(data.buffer + sizeof(S_GATE_SUBSCRIBE) + (i - 1) * 4);
//            *key1 = keylist[i];;
//        }
//        data.head.nLen = sizeof(S_GATE_SUBSCRIBE) + (subscribe->datanum - 1) * 4;
//	}
//	else
//	{
//		data.head.nLen = sizeof(S_GATE_SUBSCRIBE);
//	}
//    
//	return Subscribe(&data,0);
//}
// ������: SendLogin
// ���  : ������ 2013-5-23 15:12:27
// ����  : int -1δ����  -2���ͳ���  =0 ��ʱδ�յ�Ӧ�� 1�յ�Ӧ��
// ����  : PCGATE_COMMSTRU data   ����Ӧ������
// ����  : unsigned int timeout    ����Ӧ��ʱʱ�� <=0ֱ�ӷ���0��������Ӧ��
// ����  : ���Ͷ������󲢽���Ӧ��
int CGateLink::Subscribe(PCGATE_COMMSTRU data,unsigned int timeout)
{
	if (GetConnectStatus() != 1)
	{
		sprintf(m_sErrMsg,"SendLogin ����δ���ӻ��������ӣ��������� index[%d] ��ʶ[%d] ������[%d]",\
			m_pApiSocket.m_nIndex,data->head.stDest.nSerial,data->head.stDest.nServiceNo);
		m_pRes.g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return -1;
	}
	//	CBF_PMutex pp(&m_mutex);
	if (data->head.stComm.cNextFlag == 0)
	{
		data->head.nRetCode = 0;
		data->head.stComm.cMsgType = MSG_GATESUBSCRIBE;
		data->head.stComm.cRaflag = 0;
		data->head.stComm.nCrc = 0;
		m_nSerial = data->head.stDest.nSerial;
		int nRet = m_pApiSocket.Send2Gate(data);
		if (nRet != 2 )
		{
			sprintf(m_sErrMsg,"SendLogin Send2Gate���� ��Ϣ����[MSG_GATEREQ]  index[%d] ��ʶ[%d]",\
				m_pApiSocket.m_nIndex,data->head.stDest.nSerial);
			m_pRes.g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return -1;
		}
		else
		{
			if (timeout<=0)
			{
				return 0;
			}
			return GetAnsData(data,timeout);
		}
	}
	else
	{
		if (data->head.stComm.cNextFlag == 1 || data->head.stComm.cNextFlag == 10) //��ͣ����
		{
			return GetAnsData(data,timeout);
		}
		else //��������ȡ������
		{
			data->head.nRetCode = 0;
			data->head.stComm.cMsgType = MSG_GATESUBSCRIBE;
			data->head.stComm.cRaflag = 0;
			data->head.stComm.nCrc = 0;
			data->head.nLen = 0;
			int nRet = m_pApiSocket.Send2Gate(data);
			if (nRet != 2 )
			{
				sprintf(m_sErrMsg,"SendLogin ����ȡ���������� index[%d] ��ʶ[%d]",m_pApiSocket.m_nIndex,data->head.stDest.nSerial);				
				m_pRes.g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
				return -2;
			}
			else
			{
				return GetAnsData(data,timeout);
			}
		}
	}
}
// ������: SendSms
// ���  : ������ 2013-5-23 15:12:27
// ����  : int -1δ����  -2���ͳ���  =0 ��ʱδ�յ�Ӧ�� 1�յ�Ӧ��
// ����  : PCGATE_COMMSTRU data   ����Ӧ������
// ����  : unsigned int timeout    ����Ӧ��ʱʱ�� <=0ֱ�ӷ���0��������Ӧ��
// ����  : �������󲢽���Ӧ��
int CGateLink::SendSms(PCGATE_COMMSTRU data,unsigned int timeout)
{
	if (GetConnectStatus() != 1)
	{
		sprintf(m_sErrMsg,"SendLogin ����δ���ӻ��������ӣ��������� index[%d] ��ʶ[%d] ������[%d]",\
			m_pApiSocket.m_nIndex,data->head.stDest.nSerial,data->head.stDest.nServiceNo);
		m_pRes.g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return -1;
	}
//	CBF_PMutex pp(&m_mutex);
	if (data->head.stComm.cNextFlag == 0)
	{
		data->head.nRetCode = 0;
		data->head.stComm.cMsgType = MSG_GATESMS;
		data->head.stComm.cRaflag = 0;
		data->head.stComm.nCrc = 0;
		int nRet = m_pApiSocket.Send2Gate(data);
		if (nRet != 2 )
		{
			sprintf(m_sErrMsg,"SendSms Send2Gate���� ��Ϣ����[MSG_GATEREQ]  index[%d] ��ʶ[%d]",\
				m_pApiSocket.m_nIndex,data->head.stDest.nSerial);
				m_pRes.g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return -1;
		}
		else
		{
			if (timeout<=0)
			{
				return 0;
			}
			return GetAnsData(data,timeout);
		}
	}
	else
	{
		if (data->head.stComm.cNextFlag == 1 || data->head.stComm.cNextFlag == 10) //��ͣ����
		{
			return GetAnsData(data,timeout);
		}
		else //��������ȡ������
		{
			data->head.nRetCode = 0;
			data->head.stComm.cMsgType = MSG_GATEREQ;
			data->head.stComm.cRaflag = 0;
			data->head.stComm.nCrc = 0;
			data->head.nLen = 0;
			int nRet = m_pApiSocket.Send2Gate(data);
			if (nRet != 2 )
			{
				sprintf(m_sErrMsg,"SendSms ����ȡ���������� index[%d] ��ʶ[%d]",m_pApiSocket.m_nIndex,data->head.stDest.nSerial);				
				m_pRes.g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
				return -2;
			}
			else
			{
				return GetAnsData(data,timeout);
			}
		}
	}
}

void CGateLink::SetDatLogName(const char *name)
{
	
}
