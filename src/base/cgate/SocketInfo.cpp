// SocketInfo.cpp: implementation of the CSocketInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "SocketInfo.h"
#include "BF_Tools.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSocketInfo::CSocketInfo()
{
	memset(m_sAddress,0,sizeof(m_sAddress));         //��������������IP
	
	ResetInfo();
	m_pMemPool = NULL;
	m_gRes = NULL;
	m_nType = SOCK_UNKNOW;
	m_nCloseTime = 0;
	m_nPid = -1;
	m_sendNum = 10;
	m_bNeedConnect = false;
	m_nDrebHeadLen = DREBHEADLEN;
	
}

CSocketInfo::~CSocketInfo()
{
	if (m_sock != INVALID_SOCKET)
	{
		closesocket(m_sock);
	}
}

std::string CSocketInfo::GetDrebCmdType(int cmdtype)
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
	case  CMD_MONITOR_BPC:     //16 //�������߽ڵ���
		return "CMD_MONITOR_BPC";
	default:
		return "";
	}
}

void CSocketInfo::Init(CGateResource *res,CBF_BufferPool *pool)
{
	m_pMemPool = pool;
	m_gRes = res;


	m_log = &(m_gRes->g_pLog);
	ResetInfo();
	m_nCloseTime = 0;
	m_pDrebEndian.SetCommEndian(true);//ʹ��������

	
	m_pDrebLog = &(m_gRes->g_pDrebLog);
	
}
void CSocketInfo::ResetInfo()
{
	memset(m_sAddress,0,sizeof(m_sAddress));
	memset(m_cCreateDate,0,sizeof(m_cCreateDate));
	memset(m_cCloseDate,0,sizeof(m_cCloseDate));

	memset(&m_head,0,sizeof(DREB_HEAD));
	//socket��������
	m_sock = INVALID_SOCKET;                   //socket id
	m_bChecked = false;
	m_bNeedread=false;              //�Ƿ���Ҫ������
	m_nReadtime= time(NULL);              //�ϴζ�����ʱ��
	m_nConntime=m_nReadtime;              //���ӽ���ʱ��
	m_nSendtime=m_nReadtime;              //�ϴη�������ʱ�� 
	
	m_nPingTime = 0;
	//���ͽ�������	
	
	memset(m_sRcvBuffer,0,sizeof(m_sRcvBuffer));            //�������ݻ���   
	m_nRcvBufLen=0;                //���յ����ݳ���  �ɶ�ν���һ�����ݰ�
	
	memset(m_cCreateDate,0,sizeof(m_cCreateDate));//���ӽ�������
	memset(m_cCloseDate,0,sizeof(m_cCloseDate));//���ӹر�����
	m_nCloseTime = 0;
	
	
	
	
}

void CSocketInfo::OnClose(const char *msg,const char *filename,int fileline)
{
	CBF_PMutex pp(&m_mutex);
	m_datetime.Update();
	m_datetime.ToCharDate(m_cCloseDate);
	
	//��ն���
	m_log->LogMp(LOG_DEBUG,filename,fileline,"%s,�ر�����[%s:%d] index[%d] type[%d] sock[%d]",\
		msg,m_sAddress,m_sock,m_index,m_nType,m_sock);
	
	closesocket(m_sock);
	m_sock = INVALID_SOCKET;
	ResetInfo();
	m_nCloseTime = time(NULL);
	if (m_nType == SOCK_DREB) //��������������
	{
		m_bNeedConnect = true; //��Ҫ����
		CheckFirstData();
	}
	else
	{
		m_bNeedConnect = false; //��Ҫ����
		m_pSendQueue.Clear();
	}
}

void CSocketInfo::OnShutdown()
{
	closesocket(m_sock);
	//��ն���
	ResetInfo();
	m_nCloseTime = 0;
}
bool CSocketInfo::IsCanSelect()
{
	if (m_sock != INVALID_SOCKET)
	{
		return true;
	}
	else
	{
		return false;
	}
}


bool CSocketInfo::IsFree()
{
	if (m_sock != INVALID_SOCKET)
	{
		return false;
	}
	return true;
}
int CSocketInfo::OnRecv()
{
	int ret;
	m_tcpSocket.AttachSocket(m_sock,"");
	if (m_nRcvBufLen >=0 && m_nRcvBufLen< DREBBUFSIZE)
	{
		//��ͷδ������
		ret = m_tcpSocket.Receive(m_sRcvBuffer+m_nRcvBufLen,DREBBUFSIZE-m_nRcvBufLen);
		if (ret <= 0)
		{
			return ret;
		}
		m_nRcvBufLen+=ret;
		m_nReadtime =time(NULL);
		return ret;

	}
	return 0;
}

int CSocketInfo::OnSend()
{
	int length;
	char stmp[511];
	int ret;
	if (m_sock == INVALID_SOCKET)
	{
		m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�����ѶϿ�");
		return -2;
	}
	if (!m_pSendQueue.NeedSend())
	{
		return -3;
	}
	CBF_PMutex pp(&m_mutex);
	m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"��ʼ���Ͷ�������");
	RDQUEUE::iterator prq,prn;
	unsigned int atimeout;
	
	for (prn=m_pSendQueue.m_qSendData.datas.begin();prn!=m_pSendQueue.m_qSendData.datas.end();)
	{
// 		sendNum++;
// 		if (m_sendNum >0 && sendNum > m_sendNum) //��������ѷ������������壬���¿�ʼ
// 		{
// 			return sendNum;
// 		}
		prq = prn;
		prn++;
		m_tcpSocket.AttachSocket(m_sock,"");
		
		//����DREB������
		length = prq->totallen-prq->sendlen;
		ret = m_tcpSocket.Send((char *)(prq->data)+prq->sendlen,length);
		if (ret == length)
		{
			m_log->LogMp(LOG_PROMPT,__FILE__,__LINE__,"���з������ len[%d] s_nDrebSerial[%d]",length,prq->serial);
			
			m_pMemPool->PoolFree(prq->data);
			prq->data = NULL;
			// ����ɹ�,ֹͣ
			m_pSendQueue.DeleteSend(prq);	
			m_nSendtime = time(NULL);
			
		}
		else //ֹͣ
		{
// 			m_pMemPool->PoolFree(prq->data);
// 			m_pSendQueue.DeleteSend(prq);	
			if (ret >0)
			{
				prq->sendlen = prq->sendlen + ret;
				m_log->LogMp(LOG_PROMPT,__FILE__,__LINE__,"���Ͳ������� len[%d] s_nDrebSerial[%d]",ret,prq->serial);
			}
			m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���Ͷ�������ֹͣ");
			return 0;
		}
	}
	m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���Ͷ����������");
	return 0;
}

int CSocketInfo::Send(S_GATEDREB_RSMSG &msg,bool isimmediate)
{
	CBF_PMutex pp(&m_mutex);
	m_tcpSocket.AttachSocket(m_sock,"");
	//�ֽ���ת��������У��λ
	if (msg.data->head.cCmd != CMD_PING && msg.data->head.cCmd != CMD_CONNECT && msg.data->head.cCmd != CMD_REGSERVICE)
	{
		m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���͸�DREB[%d %s %d]����Ϣ DREB����[%s] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Դ[%d %d] cZip[%d] ҵ�����ݳ���[%d]",\
			m_index,m_sDrebIp.c_str(),m_nDrebPort,GetDrebCmdType(msg.data->head.cCmd).c_str(),\
			msg.data->head.cRaflag,msg.data->head.d_Dinfo.d_nServiceNo,msg.data->head.s_Sinfo.s_nNodeId,\
			msg.data->head.s_Sinfo.s_cNodePrivateId,msg.data->head.s_Sinfo.s_nDrebSerial,\
			msg.data->head.s_Sinfo.s_nSerial,msg.data->head.s_Sinfo.s_nHook,\
			msg.data->head.cZip,msg.data->head.nLen);
		m_pDrebLog->LogDreb(LOG_DEBUG,msg.data,false);
	}
	else
	{
		m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���͸�DREB[%d %s %d]����Ϣ DREB����[%s]  ҵ�����ݳ���[%d]",\
			m_index,m_sDrebIp.c_str(),m_nDrebPort,GetDrebCmdType(msg.data->head.cCmd).c_str(),msg.data->head.nLen);
	}
	msg.serial = msg.data->head.s_Sinfo.s_nDrebSerial;
	msg.totallen = msg.data->head.nLen+DREBHEADLEN;
	msg.sendlen = msg.totallen;
	msg.offset = 0;
	
	int nRet  =0;
	//�ֽ���ת��
	if (m_gRes->g_nCrcFlag == 1)
	{
		m_pDrebEndian.Endian2CommGenCrc(&(msg.data->head));
	}
	else
	{
		m_pDrebEndian.Endian2Comm(&(msg.data->head));
	}
	
	if (isimmediate)
	{
		nRet = m_tcpSocket.Send((char *)msg.data,msg.totallen);
		if (nRet == msg.totallen)
		{
			m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"������� serial[%d] [send:%d %d]",msg.serial,nRet,msg.sendlen);
			m_pMemPool->PoolFree(msg.data);
			msg.data = NULL;
			return 0;
		}
		else
		{
			if (nRet >0)
			{
				msg.sendlen = msg.totallen - nRet;
				m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"ֻ���Ͳ������� serial[%d] [send:%d %d]",msg.serial,nRet,msg.sendlen);
			}
			else
			{
				m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"��������ʧ�� serial[%d] [send:%d %d]",msg.serial,nRet,msg.sendlen);
			}
			//��������������ݿ��ܾͻ�������
			if (!NeedSend())
			{
				PutSendMsg(msg);
				return 1;
			}
			else
			{
				OnClose("ֱ�������ӷ������ݲ��ɹ��������Ӷ�������������",__FILE__,__LINE__);
				m_pMemPool->PoolFree(msg.data);
				msg.data = NULL;
				return -1;
			}
		}
		return 0;
	}
	else
	{
		if (NeedSend())
		{
			m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"������������ ����������´��ٷ� index[%d] serial[%d]",\
				m_index,msg.serial);
			PutSendMsg(msg);
			return 1;
		}
		
		nRet = m_tcpSocket.Send((char *)msg.data,msg.totallen);
		if (nRet != msg.totallen)
		{
			if (nRet <=0)
			{
				m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"����ʧ�� ����������ٷ� serial[%d]",msg.serial);
				PutSendMsg(msg);
			}
			else
			{
				msg.sendlen = msg.totallen - nRet;
				PutSendMsg(msg);
				m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"����ģʽ����������ֻ���Ͳ���serial[%d] [send:%d %d]",msg.serial,nRet,msg.totallen);
			}
			return 1;
		}
		m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"������� serial[%d] [send:%d %d]",msg.serial,nRet,msg.sendlen);
		//�ͷſռ�
		m_pMemPool->PoolFree(msg.data);
		msg.data = NULL;
	}
	
	return 0;
}
int CSocketInfo::PutSendMsg(S_GATEDREB_RSMSG msg)
{
	int ret;
	ret = m_pSendQueue.PutSendMsg(msg);
	if (ret<0)
	{
		m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"������ ���� DREB[%d %s %d]����Ϣ DREB����[%s] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Դ[%d %d] cZip[%d] ҵ�����ݳ���[%d]",\
			m_index,m_sDrebIp.c_str(),m_nDrebPort,GetDrebCmdType(msg.data->head.cCmd).c_str(),\
			msg.data->head.cRaflag,msg.data->head.d_Dinfo.d_nServiceNo,msg.data->head.s_Sinfo.s_nNodeId,\
			msg.data->head.s_Sinfo.s_cNodePrivateId,msg.data->head.s_Sinfo.s_nDrebSerial,\
			msg.data->head.s_Sinfo.s_nSerial,msg.data->head.s_Sinfo.s_nHook,\
			msg.data->head.cZip,msg.data->head.nLen);
		m_pMemPool->PoolFree(msg.data);
		msg.data = NULL;
		return -1;
	}
	return 0;
}



int CSocketInfo::GetRecvData(S_GATEDREB_RSMSG *msg)
{
	int txlen=0;
	char tmpchar[100];
	memset(tmpchar,0,sizeof(tmpchar));
	//�յ������ݴ��ڰ�ͷ
	if (m_nRcvBufLen >= DREBHEADLEN )
	{
		//�����ֽ�����У��CRC
		memcpy(&(msg->data->head),m_sRcvBuffer,DREBHEADLEN);
		if (m_gRes->g_nCrcFlag == 1)
		{
			if (!m_pDrebEndian.Endian2LocalHostChkCrc(&(msg->data->head)) )
			{
				//			OnClose("CRC��",__FILE__,__LINE__);
				return -1;
			}
		}
		else
		{
			m_pDrebEndian.Endian2LocalHost(&(msg->data->head)) ;
		}
		
		if (msg->data->head.nLen<0 || msg->data->head.nLen > DREBDATASIZE)
		{
			m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"index[%d] ����ͷ[%d]���Ȳ���",m_index,msg->data->head.nLen);
			return -1;
		}
		if (m_nRcvBufLen < msg->data->head.nLen + DREBHEADLEN)
		{
			//˵������û�н�������
			return 0;
		}
		if (msg->data->head.nLen > 0 )
		{
			memcpy(msg->data->buffer,m_sRcvBuffer+DREBHEADLEN,msg->data->head.nLen);
		}
		//ͳ������
// 		if (msg->data->head.cCmd != CMD_PING && msg->data->head.cCmd != CMD_CONNECT && msg->data->head.cCmd != CMD_REGSERVICE)
// 		{
// 			m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"��DREB[%d %d %d]�յ�����Ϣ DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Դ[%d %d] ҵ�����ݳ���[%d]",\
// 				msg->data->head.s_Sinfo.s_nNodeId,msg->data->head.s_Sinfo.s_cNodePrivateId,m_index,\
// 				GetDrebCmdType(msg->data->head.cCmd).c_str(),msg->data->head.cNextFlag,\
// 				msg->data->head.cRaflag,msg->data->head.d_Dinfo.d_nServiceNo,msg->data->head.s_Sinfo.s_nNodeId,\
// 				msg->data->head.s_Sinfo.s_cNodePrivateId,msg->data->head.s_Sinfo.s_nDrebSerial,\
// 				msg->data->head.s_Sinfo.s_nSerial,msg->data->head.s_Sinfo.s_nHook,msg->data->head.nLen);
// 		}
// 		else
// 		{
// 			m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"��DREB[%d]�յ�����Ϣ DREB����[%s]  ҵ�����ݳ���[%d]",\
// 				m_index,GetDrebCmdType(msg->data->head.cCmd).c_str(),msg->data->head.nLen);
// 		}
		

		//�ն��ˣ���ԭ�����¸�m_sRcvBuffer
		for (int i=0; i<m_nRcvBufLen-DREBHEADLEN-msg->data->head.nLen; i++)
		{
			m_sRcvBuffer[i]=m_sRcvBuffer[i+DREBHEADLEN+msg->data->head.nLen];
		}
		m_nRcvBufLen=m_nRcvBufLen - DREBHEADLEN-msg->data->head.nLen;
		
		return DREBHEADLEN+msg->data->head.nLen;
	}
	else if (m_nRcvBufLen>0)
	{
		m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"index[%d] type[%s] �յ�����%d,��������",m_index,"BPCSOCK_DREB",m_nRcvBufLen);
	}
	return -1;
}

bool CSocketInfo::NeedConnect()
{
	return m_bNeedConnect;
}

bool CSocketInfo::NeedRead()
{
	return m_bNeedread;
}

bool CSocketInfo::NeedSend()
{
	return m_pSendQueue.NeedSend();
}
void CSocketInfo::CheckFirstData()
{
	RDQUEUE::iterator prq,prn;
	for (prn=m_pSendQueue.m_qSendData.datas.begin(); prn!=m_pSendQueue.m_qSendData.datas.end() ; )
	{
		if (prn->sendlen != prn->totallen)
		{
			m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"���� �����е�һ�����ݰ����Ͳ������ݣ�������ɾ�� %d",prn->nIndex);
			m_pSendQueue.DeleteSend(prn);
		}
		break;
	}
}