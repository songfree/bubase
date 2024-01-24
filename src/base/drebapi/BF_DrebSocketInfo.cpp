// BF_DrebSocketInfo.cpp: implementation of the CBF_DrebSocketInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "BF_DrebSocketInfo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define  DESENCRYPTKEY    "rd402wms"
#define  DESENCRYPTKEY3   "wms13711songfree"

CBF_DrebSocketInfo::CBF_DrebSocketInfo()
{
	ResetInfo();
	m_pMemPool = NULL;
	m_gRes = NULL;
	m_nType = BPCSOCK_UNKNOW;
	m_nCloseTime = 0;
	m_nPid = -1;
	m_sendNum = 10;
	m_bNeedConnect = false;
	m_nBpcHeadLen = sizeof(S_BPC_HEAD);
	m_nDrebHeadLen = DREBHEADLEN;
}

CBF_DrebSocketInfo::~CBF_DrebSocketInfo()
{
	if (m_sock != INVALID_SOCKET)
	{
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
	}
	RDQUEUE::iterator prq,prn;
	for (prn=m_pSendQueue.m_qSendData.datas.begin();prn!=m_pSendQueue.m_qSendData.datas.end();)
	{
		prq = prn;
		prn++;
		m_pMemPool->PoolFree(prq->sMsgBuf);
		m_pSendQueue.DeleteSend(prq);	
	}
}
std::string CBF_DrebSocketInfo::GetBpcMsgType(int msgtype)
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
	case MSG_BPCMONITOR:      //16      //�����ļ�ر��棬��ʱ����
		return "MSG_BPCMONITOR";
	case MSG_GETBPCNEXT:      //17      //dreb������ȡ��BPC����ĺ���������Ϣ
		return "MSG_GETBPCNEXT";
	case MSG_CONNECTDREB:      //18      //dreb������������DREB�ɹ�����Ϣ����������Ϣ�����ע���(����ѯ�ʣ�ע��)
		return "MSG_CONNECTDREB";
	case MSG_MONITOR_SAP:      //19      //dreb��������������Ϣ��DREB������ΪCMD_MONITOR_BPC,����ļ������service_no����DrebPubDefine.h���BPCMONI_INFO����
		return "MSG_MONITOR_SAP";
	case MSG_DREBANS:      //20      //DREB������Ӧ����Ϣ
		return "MSG_DREBANS";
	default:
		return "";
	}
}
std::string CBF_DrebSocketInfo::GetDrebCmdType(int cmdtype)
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
	case  CMD_UNREGSERVICE:      //14 //ȡ������ע��
		return "CMD_UNREGSERVICE";
	case  CMD_MONITOR_DREB:     //15 //�������߽ڵ���
		return "CMD_MONITOR_DREB";
	case  CMD_MONITOR_BPC:     //16 //�������߽ڵ���
		return "CMD_MONITOR_BPC";
	default:
		return "";
	}
}

void CBF_DrebSocketInfo::Init(CBF_DrebResource *res,CBF_BufferPool *pool,CIErrlog *perrlog,CBF_CommDataLog *pdreb2bpclog,CBF_CommDataLog *pbpc2dreblog)
{
	m_pMemPool = pool;
	m_gRes = res;
	m_log = perrlog;
	m_pDreb2BpcDataLog = pdreb2bpclog;
	m_pBpc2DrebDataLog = pbpc2dreblog;
	m_pSendQueue.m_nQueueSize = m_gRes->g_nQueueSize;
	ResetInfo();
	m_nCloseTime = 0;
	m_pDrebEndian.SetCommEndian(true);//ʹ��������
}
void CBF_DrebSocketInfo::ResetInfo()
{
	
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
	m_nConntime = 0;
	memset(m_cCloseDate,0,sizeof(m_cCloseDate));//���ӹر�����
	m_nCloseTime = 0;
}

void CBF_DrebSocketInfo::OnClose(const char *msg,const char *filename,int fileline)
{
	if (m_sock == INVALID_SOCKET)
	{
		return;
	}
	CBF_PMutex pp(&m_mutex);
	m_datetime.Update();
	m_datetime.ToCharDate(m_cCloseDate);
#ifdef _ENGLISH_
	m_log->LogMp(LOG_DEBUG,filename,fileline,"%s,close DREB [%s:%d %d] index[%d] type[%d] sock[%d]",\
		msg,m_sDrebIp.c_str(),m_nDrebPort,m_sock,m_index,m_nType,m_sock);
#else
	m_log->LogMp(LOG_DEBUG,filename,fileline,"%s,�ر�DREB����[%s:%d %d] index[%d] type[%d] sock[%d]",\
		msg,m_sDrebIp.c_str(),m_nDrebPort,m_sock,m_index,m_nType,m_sock);
#endif
	
	closesocket(m_sock);
	m_sock = INVALID_SOCKET;
	ResetInfo();
	m_nCloseTime = time(NULL);
	
	m_bNeedConnect = true; //��Ҫ����
	//������һ�������ɾ���������������ķ��ڶ�����
	CheckFirstData();
}

void CBF_DrebSocketInfo::OnShutdown()
{
	closesocket(m_sock);
	ResetInfo();
	m_nCloseTime = 0;
}
bool CBF_DrebSocketInfo::IsCanSelect()
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

bool CBF_DrebSocketInfo::Listen(int port,bool blockflag,int queuenum)
{
	if (!m_tcpSocket.Create())
	{
		CBF_Tools::StringCopy(m_errMsg,255,m_tcpSocket.GetErrorMsg().c_str());
		return false;
	}
	m_nPort = port;
	if (!m_tcpSocket.Listen(m_nPort,queuenum))
	{
#ifdef _ENGLISH_
		sprintf(m_errMsg,"Listen Port [%d] fail, queue size %d",m_nPort,queuenum);
#else
		sprintf(m_errMsg,"�����˿�[%d]ʧ��,�������д�С%d",m_nPort,queuenum);
#endif
		return false;
	}
	if (!blockflag)
	{
		m_tcpSocket.SetBlockFlag(false);
	}
	m_nType   =  BPCSOCK_LISTEN;      //����
	m_sock = m_tcpSocket.GetSocket();
#ifndef _WINDOWS
	fcntl(m_sock,F_SETFD,25);
#endif
	m_bNeedread = true;
	
	return true;
}

bool CBF_DrebSocketInfo::IsFree()
{
	if (m_sock != INVALID_SOCKET)
	{
		return false;
	}
	if (m_nType == BPCSOCK_LISTEN)//������
	{
		return false;
	}
	return true;
}
int CBF_DrebSocketInfo::OnRecv()
{
	int ret;
	m_tcpSocket.AttachSocket(m_sock,"");
	if (m_nRcvBufLen >=0 && m_nRcvBufLen< BPCBUFSIZE)
	{
		//��ͷδ������
		ret = m_tcpSocket.Receive(m_sRcvBuffer+m_nRcvBufLen,BPCBUFSIZE-m_nRcvBufLen);
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

int CBF_DrebSocketInfo::OnSend()
{
	int ret;
	if (m_sock == INVALID_SOCKET)
	{
#ifdef _ENGLISH_
		m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"connect closed");
#else
		m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�����ѶϿ�");
#endif
		return -2;
	}
	if (!m_pSendQueue.NeedSend())
	{
		return -3;
	}
	CBF_PMutex pp(&m_mutex);
	RDQUEUE::iterator prq,prn;
	unsigned int atimeout= time(NULL);
	int sendNum=0;
	for (prn=m_pSendQueue.m_qSendData.datas.begin();prn!=m_pSendQueue.m_qSendData.datas.end();)
	{
		sendNum++;
		if (m_sendNum >0 && sendNum > m_sendNum) //��������ѷ������������壬���¿�ʼ
		{
			return sendNum;
		}
		prq = prn;
		prn++;
		if (atimeout - prq->nRtime > 300)
		{
#ifdef _ENGLISH_
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__," index[%d] DREB[%d %d] src[%d %d %d] msg timeout,  datalen[%d]",\
				m_index,m_nDrebId,m_nDrebPrivateId,prq->s_nNodeId,prq->s_cNodePrivateId,prq->s_nDrebSerial,ret);
#else
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"�Ӷ����﷢�͸�index[%d] DREB[%d %d] Դ[%d %d %d] ����Ϣ ��ʱ5���Ӷ���  ���ݳ���[%d]",\
				m_index,m_nDrebId,m_nDrebPrivateId,prq->s_nNodeId,prq->s_cNodePrivateId,prq->s_nDrebSerial,ret);
#endif
			m_pMemPool->PoolFree(prq->sMsgBuf);
			m_pSendQueue.DeleteSend(prq);	
			continue;
		}
		m_tcpSocket.AttachSocket(m_sock,"");

		ret = m_tcpSocket.Send((char *)&(prq->sMsgBuf->sDBHead)+(prq->nTotalLen-prq->nSendLen),prq->nSendLen);
		if (ret == prq->nSendLen)
		{
			//ͳ������
			m_gRes->ReportSend(prq->nSendLen);
#ifdef _ENGLISH_
			m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"index[%d] DREB[%d %d] src[%d %d %d] send msg success datalen[%d]",\
				m_index,m_nDrebId,m_nDrebPrivateId,prq->s_nNodeId,prq->s_cNodePrivateId,prq->s_nDrebSerial,ret);
#else
			m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�Ӷ����﷢�͸�index[%d] DREB[%d %d] Դ[%d %d %d] ����Ϣ�ɹ� ���ݳ���[%d]",\
				m_index,m_nDrebId,m_nDrebPrivateId,prq->s_nNodeId,prq->s_cNodePrivateId,prq->s_nDrebSerial,ret);
#endif
			m_pMemPool->PoolFree(prq->sMsgBuf);
			// ����ɹ�,ֹͣ
			m_pSendQueue.DeleteSend(prq);	
			m_nSendtime = time(NULL);
			
		}
		else //ֹͣ
		{
			if (ret >0)
			{
				m_gRes->ReportSend(ret);
				prq->nSendLen = prq->nSendLen -ret;
#ifdef _ENGLISH_
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"index[%d] DREB[%d %d] src[%d %d %d] send part success datalen[%d]",\
					m_index,m_nDrebId,m_nDrebPrivateId,prq->s_nNodeId,prq->s_cNodePrivateId,prq->s_nDrebSerial,ret);
#else
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"�Ӷ����﷢�͸�index[%d] DREB[%d %d] Դ[%d %d %d] ����Ϣ���ֳɹ� ���ݳ���[%d]",\
					m_index,m_nDrebId,m_nDrebPrivateId,prq->s_nNodeId,prq->s_cNodePrivateId,prq->s_nDrebSerial,ret);
#endif
			}
			else
			{
				//�´��ٷ���ֱ���ɹ������ӹر�
#ifdef _ENGLISH_
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"index[%d] DREB[%d %d] src[%d %d %d] send fail, ret[%d]",\
					m_index,m_nDrebId,m_nDrebPrivateId,prq->s_nNodeId,prq->s_cNodePrivateId,prq->s_nDrebSerial,ret);
#else
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"�Ӷ����﷢�͸�index[%d] DREB[%d %d] Դ[%d %d %d] ����Ϣʧ�� ret[%d]",\
					m_index,m_nDrebId,m_nDrebPrivateId,prq->s_nNodeId,prq->s_cNodePrivateId,prq->s_nDrebSerial,ret);
#endif
			}
			//���Ͳ��ɹ����´��ٷ������Ǵ������ٷ�
			return 0;
		}
	}
	return sendNum;
}


int CBF_DrebSocketInfo::SendMsg(S_BPC_RSMSG &msg,bool sendimmediate)
{
	int ret=0;
	CBF_PMutex pp(&m_mutex);
	m_tcpSocket.AttachSocket(m_sock,"");
	//��DREBHEAD�е����ݷ���MSG�У�������ʱд��־���и���
	msg.s_nNodeId = msg.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId;
	msg.s_nDrebSerial = msg.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial;
	msg.s_cNodePrivateId = msg.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId;

	msg.nTotalLen = msg.sMsgBuf->sDBHead.nLen+DREBHEADLEN;
	msg.nSendLen = msg.nTotalLen;
	if (msg.sMsgBuf->sDBHead.cCmd != CMD_PING && msg.sMsgBuf->sDBHead.cCmd != CMD_CONNECT && msg.sMsgBuf->sDBHead.cCmd != CMD_REGSERVICE)
	{
		if (m_log->isWrite(LOG_DEBUG+1))
		{
#ifdef _ENGLISH_
			m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"send to DREB[%d] msg DREB[%s] RA[%d] txcode[%d]  flag[%d %d %d] src[%d %d] cZip[%d] datalen[%d]",\
				m_index,GetDrebCmdType(msg.sMsgBuf->sDBHead.cCmd).c_str(),\
				msg.sMsgBuf->sDBHead.cRaflag,msg.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,msg.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
				msg.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,msg.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
				msg.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,msg.sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
				msg.sMsgBuf->sDBHead.cZip,msg.sMsgBuf->sDBHead.nLen);
#else
			m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���͸�DREB[%d]����Ϣ DREB����[%s] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Դ[%d %d] cZip[%d] ҵ�����ݳ���[%d]",\
				m_index,GetDrebCmdType(msg.sMsgBuf->sDBHead.cCmd).c_str(),\
				msg.sMsgBuf->sDBHead.cRaflag,msg.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,msg.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
				msg.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,msg.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
				msg.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,msg.sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
				msg.sMsgBuf->sDBHead.cZip,msg.sMsgBuf->sDBHead.nLen);
#endif
		}
		
	}
	else
	{
		if (m_log->isWrite(LOG_DEBUG+2))
		{
#ifdef _ENGLISH_
			m_log->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"send to DREB[%d]  DREB[%s]  datalen[%d]",\
				m_index,GetDrebCmdType(msg.sMsgBuf->sDBHead.cCmd).c_str(),msg.sMsgBuf->sDBHead.nLen);
#else
			m_log->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"���͸�DREB[%d]����Ϣ DREB����[%s]  ҵ�����ݳ���[%d]",\
				m_index,GetDrebCmdType(msg.sMsgBuf->sDBHead.cCmd).c_str(),msg.sMsgBuf->sDBHead.nLen);
#endif
		}
	}
	if (1 == msg.sMsgBuf->sDBHead.cRaflag)
	{
		//д��DREB��������������־
		m_pDreb2BpcDataLog->LogDreb(LOG_PROMPT,msg.sMsgBuf,false);
	}
	else
	{
		//д��BPC�������ⷢ����־
		m_pBpc2DrebDataLog->LogDreb(LOG_PROMPT,msg.sMsgBuf,false);
	}
	if (m_gRes->g_nCrcFlag == 1)
	{
		m_pDrebEndian.Endian2CommGenCrc(&(msg.sMsgBuf->sDBHead));
	}
	else
	{
		m_pDrebEndian.Endian2Comm(&(msg.sMsgBuf->sDBHead));
	}
	//���Ͷ����������ݣ�����뷢�Ͷ��У���˳����
	if (sendimmediate)
	{
		if (m_sock == INVALID_SOCKET)
		{
#ifdef _ENGLISH_
			m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"connect closed��push queue datalen[%d] flag[%d %d %d] ",msg.nTotalLen,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
#else
			m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"�����ѶϿ�������DREB���� ���ݳ���[%d] ��ʶ[%d %d %d] ",msg.nTotalLen,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
#endif
			PutSendMsg(msg);
			return 1;
		}
		ret = m_tcpSocket.Send((char *)&(msg.sMsgBuf->sDBHead),msg.nTotalLen);
		if (ret == msg.nTotalLen)
		{
#ifdef _ENGLISH_
			m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"send finish index[%d] flag[%d %d %d] ",\
				m_index,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
#else
			m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"������� index[%d] ��ʶ[%d %d %d] ",\
				m_index,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
#endif
			//ͳ������
			m_gRes->ReportSend(msg.nTotalLen);
			m_pMemPool->PoolFree(msg.sMsgBuf);
			msg.sMsgBuf = NULL;
			m_nSendtime = time(NULL);
			return 0;
			
		}
		else //ֹͣ
		{
			if (ret >0)
			{
				msg.nSendLen = msg.nTotalLen - ret;
			}
#ifdef _ENGLISH_
			m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"warn,send to DREB[%d] failed and push queue, retcode[%d] msg[%s] push len [%d] datalen[%d] flag[%d %d %d] ",\
				m_index,ret,m_tcpSocket.GetErrorMsg().c_str(),msg.nSendLen,msg.nTotalLen,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
#else
			m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"���� ���͸�DREB[%d]����Ϣ����,������� ret����[%d] msg[%s] ���볤��[%d] ���ݳ���[%d] ��ʶ[%d %d %d] ",\
				m_index,ret,m_tcpSocket.GetErrorMsg().c_str(),msg.nSendLen,msg.nTotalLen,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
#endif
			//ֻҪ���ͳ���ͷŵ����У���������������ݿ��ܾͻ�������
			if (!m_pSendQueue.NeedSend())
			{
				if (PutSendMsg(msg) < 0)
				{
					return 100;
				}
				return 1;
			}
			else
			{
#ifdef _ENGLISH_
				OnClose("send fail,queue has data",__FILE__,__LINE__);
#else
				OnClose("ֱ�������ӷ������ݲ��ɹ��������Ӷ�������������",__FILE__,__LINE__);
#endif
				return -1;
			}
			
		}
	}
	else
	{
		if (NeedSend()) //��������
		{
			if (msg.sMsgBuf->sBpcHead.cMsgType != MSG_BPCMONITOR)
			{
#ifdef _ENGLISH_
				m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"queue has data,push DREB queue  index[%d] datalen[%d] flag[%d %d %d] cMsgType[%d] txcode[%d]",\
					m_index,msg.nTotalLen,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial,msg.sMsgBuf->sBpcHead.cMsgType,msg.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo);
#else
				m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���Ͷ����������ݣ�����DREB���� index[%d] ���ݳ���[%d] ��ʶ[%d %d %d] cMsgType[%d] txcode[%d]",\
					m_index,msg.nTotalLen,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial,msg.sMsgBuf->sBpcHead.cMsgType,msg.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo);
#endif
                if (PutSendMsg(msg) < 0)
                {
                    return 100;
                }
				return 1;
			}
		}
		else  //ֱ�ӷ���
		{
			if (m_sock == INVALID_SOCKET )
			{
#ifdef _ENGLISH_
				m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"connect closed,push DREB queue, index[%d] datalen[%d] flag[%d %d %d] ",\
					m_index,msg.nTotalLen,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
#else
				m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"�����ѶϿ�������DREB���� index[%d] ���ݳ���[%d] ��ʶ[%d %d %d] ",\
					m_index,msg.nTotalLen,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
#endif
                if (PutSendMsg(msg) < 0)
                {
                    return 100;
                }
				return 1;
			}
			if (!m_bChecked)
			{
#ifdef _ENGLISH_
				m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"connect not affirm,push DREB queue, index[%d] datalen[%d] flag[%d %d %d] ",\
					m_index,msg.nTotalLen,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
#else
				m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"����δ��֤ͨ��������DREB���� index[%d] ���ݳ���[%d] ��ʶ[%d %d %d] ",\
					m_index,msg.nTotalLen,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
#endif
                if (PutSendMsg(msg) < 0)
                {
                    return 100;
                }
				return 1;
			}
			ret = m_tcpSocket.Send((char *)&(msg.sMsgBuf->sDBHead),msg.nTotalLen);
			if (ret == msg.nTotalLen)
			{
#ifdef _ENGLISH_
				m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"send finish, index[%d] flag[%d %d %d] ",\
					m_index,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
#else
				m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"������� index[%d] ��ʶ[%d %d %d] ",\
					m_index,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
#endif
				//ͳ������
				m_gRes->ReportSend(msg.nTotalLen);
				m_pMemPool->PoolFree(msg.sMsgBuf);
				msg.sMsgBuf = NULL;
				m_nSendtime = time(NULL);
				return 0;
				
			}
			else //ֹͣ
			{
				if (ret >0)
				{
					msg.nSendLen = msg.nTotalLen - ret;
				}
#ifdef _ENGLISH_
				m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"warn,send to DREB[%d] failed,push queue, ret[%d] msg[%s] pushlen[%d] datalen[%d] flag[%d %d %d] ",\
					m_index,ret,m_tcpSocket.GetErrorMsg().c_str(),msg.nSendLen,msg.nTotalLen,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
#else
				m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"���� ���͸�DREB[%d]����Ϣ����,������� ret����[%d] msg[%s] ���볤��[%d] ���ݳ���[%d] ��ʶ[%d %d %d] ",\
					m_index,ret,m_tcpSocket.GetErrorMsg().c_str(),msg.nSendLen,msg.nTotalLen,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
#endif
				//ֻҪ���ͳ���ͷ������
                if (PutSendMsg(msg) < 0)
                {
                    return 100;
                }
				return 2;
			}
		}
	}
	return ret;
}

int CBF_DrebSocketInfo::PutSendMsg(S_BPC_RSMSG msg)
{
	int ret;
	
	ret = m_pSendQueue.PutSendMsg(msg);
	if (ret <0 )
	{
#ifdef _ENGLISH_
		m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"queue already full,discard, index[%d] flag[%d %d %d] queuesize[%d]",\
				m_index,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial, m_pSendQueue.m_nQueueSize);
#else
		m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�������������� index[%d] ��ʶ[%d %d %d]  ���д�С[%d]",\
				m_index,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial, m_pSendQueue.m_nQueueSize);
#endif
		m_pMemPool->PoolFree(msg.sMsgBuf);
		msg.sMsgBuf = NULL;
		return ret;
	}
	return 0;
}



int CBF_DrebSocketInfo::GetRecvData(S_BPC_RSMSG *msg)
{
	int txlen=0;
	char tmpchar[100];
	memset(tmpchar,0,sizeof(tmpchar));
	if (msg->sMsgBuf == NULL)
	{
		return -100;
	}
	//�յ������ݴ��ڰ�ͷ
	if (m_nRcvBufLen >= DREBHEADLEN )
	{
		//�����ֽ�����У��CRC
		memcpy(&(msg->sMsgBuf->sDBHead),m_sRcvBuffer,DREBHEADLEN);
		if (m_gRes->g_nCrcFlag ==1 )
		{
			if (!m_pDrebEndian.Endian2LocalHostChkCrc(&(msg->sMsgBuf->sDBHead)) )
			{
#ifdef _ENGLISH_
				OnClose("CRC error",__FILE__,__LINE__);
#else
				OnClose("���� CRC��",__FILE__,__LINE__);
#endif
				return -1;
			}
		}
		else
		{
			m_pDrebEndian.Endian2LocalHost(&(msg->sMsgBuf->sDBHead)) ;
		}
		
		if (m_nRcvBufLen < msg->sMsgBuf->sDBHead.nLen + DREBHEADLEN)
		{
			//˵������û�н�������
			return 0;
		}
		if (msg->sMsgBuf->sDBHead.nLen >=0 && msg->sMsgBuf->sDBHead.nLen <= DREBDATASIZE )
		{
			memcpy(msg->sMsgBuf->sBuffer,m_sRcvBuffer+DREBHEADLEN,msg->sMsgBuf->sDBHead.nLen);
		}
		else
		{
#ifdef _ENGLISH_
			OnClose("connect not DREB",__FILE__,__LINE__);
#else
			OnClose("���� ���ӷ�DREB",__FILE__,__LINE__);
#endif
			return -1;
		}
		
		msg->sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN+msg->sMsgBuf->sDBHead.nLen;
		msg->sMsgBuf->sBpcHead.nBpuIndex = 0;
		msg->sMsgBuf->sBpcHead.nConnectTime = m_nConntime;
		msg->sMsgBuf->sBpcHead.nIndex = m_index; 
		msg->sMsgBuf->sBpcHead.cMsgType = MSG_REQ;
		msg->sMsgBuf->sBpcHead.nRtime = time(NULL);
		
		//ͳ������
		m_gRes->ReportRcv(msg->sMsgBuf->sBpcHead.nBpcLen);
		if (msg->sMsgBuf->sDBHead.cCmd != CMD_PING && msg->sMsgBuf->sDBHead.cCmd != CMD_CONNECT && msg->sMsgBuf->sDBHead.cCmd != CMD_REGSERVICE)
		{
#ifdef _ENGLISH_
			m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"index[%d] DREB[%d %d] reveive: DREB[%d %d] msg DREB[%s] next[%d] RA[%d] txcode[%d]  flag[%d %d %d] src[%d %d] cZip[%d] dest[%d %d %d %d] datalen[%d]",\
				m_index,m_nDrebId,m_nDrebPrivateId,msg->sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,msg->sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,\
				GetDrebCmdType(msg->sMsgBuf->sDBHead.cCmd).c_str(),msg->sMsgBuf->sDBHead.cNextFlag,\
				msg->sMsgBuf->sDBHead.cRaflag,msg->sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,msg->sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
				msg->sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,msg->sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
				msg->sMsgBuf->sDBHead.s_Sinfo.s_nSerial,msg->sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
				msg->sMsgBuf->sDBHead.cZip,msg->sMsgBuf->sDBHead.d_Dinfo.d_nNodeId,\
				msg->sMsgBuf->sDBHead.d_Dinfo.d_cNodePrivateId,msg->sMsgBuf->sDBHead.d_Dinfo.d_nSvrMainId,\
				msg->sMsgBuf->sDBHead.d_Dinfo.d_cSvrPrivateId,msg->sMsgBuf->sDBHead.nLen);
#else
			m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"��index[%d] DREB[%d %d] �յ�ԴDREB[%d %d]�յ�����Ϣ DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Դ[%d %d] cZip[%d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]",\
				m_index,m_nDrebId,m_nDrebPrivateId,msg->sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,msg->sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,\
				GetDrebCmdType(msg->sMsgBuf->sDBHead.cCmd).c_str(),msg->sMsgBuf->sDBHead.cNextFlag,\
				msg->sMsgBuf->sDBHead.cRaflag,msg->sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,msg->sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
				msg->sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,msg->sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
				msg->sMsgBuf->sDBHead.s_Sinfo.s_nSerial,msg->sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
				msg->sMsgBuf->sDBHead.cZip,msg->sMsgBuf->sDBHead.d_Dinfo.d_nNodeId,\
				msg->sMsgBuf->sDBHead.d_Dinfo.d_cNodePrivateId,msg->sMsgBuf->sDBHead.d_Dinfo.d_nSvrMainId,\
				msg->sMsgBuf->sDBHead.d_Dinfo.d_cSvrPrivateId,msg->sMsgBuf->sDBHead.nLen);
#endif
		}
		else
		{
#ifdef _ENGLISH_
			m_log->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"index[%d] DREB[%d %d] receive  DREB[%s]  datalen[%d]",\
				m_index,m_nDrebId,m_nDrebPrivateId,GetDrebCmdType(msg->sMsgBuf->sDBHead.cCmd).c_str(),msg->sMsgBuf->sDBHead.nLen);
#else
			m_log->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"���յ�index[%d] DREB[%d %d]����Ϣ DREB����[%s]  ҵ�����ݳ���[%d]",\
				m_index,m_nDrebId,m_nDrebPrivateId,GetDrebCmdType(msg->sMsgBuf->sDBHead.cCmd).c_str(),msg->sMsgBuf->sDBHead.nLen);
#endif
		}
		//����Ӧ����Ϊ���ɿ����ͻ�ȫ��㲥��Ϣ��ֱ�Ӷ���
		if (msg->sMsgBuf->sDBHead.cRaflag == 1 && (msg->sMsgBuf->sDBHead.cCmd == CMD_DPPUSH || msg->sMsgBuf->sDBHead.cCmd == CMD_DPABC))
		{
//				m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"CMD_DPPUSH��CMD_DPABC��Ӧ�𣬶���");
		}
		else
		{
			if (1 == msg->sMsgBuf->sDBHead.cRaflag)
			{
				//д��BPC�������ⷢ����־
				m_pBpc2DrebDataLog->LogDreb(LOG_PROMPT,msg->sMsgBuf,false);
			}
			else
			{
				//д��DREB��������������־
				m_pDreb2BpcDataLog->LogDreb(LOG_PROMPT,msg->sMsgBuf,false);
			}
		}
		

		//�ն��ˣ���ԭ�����¸�m_sRcvBuffer
		for (int i=0; i<m_nRcvBufLen-DREBHEADLEN-msg->sMsgBuf->sDBHead.nLen; i++)
		{
			m_sRcvBuffer[i]=m_sRcvBuffer[i+DREBHEADLEN+msg->sMsgBuf->sDBHead.nLen];
		}
		m_nRcvBufLen=m_nRcvBufLen - DREBHEADLEN-msg->sMsgBuf->sDBHead.nLen;
		
		return DREBHEADLEN+msg->sMsgBuf->sDBHead.nLen;
	}
	else if (m_nRcvBufLen>0)
	{
#ifdef _ENGLISH_
		m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"warn, index[%d] DREB[%d %d] type[%s] receive part data %d",\
			m_index,m_nDrebId,m_nDrebPrivateId,"BPCSOCK_DREB",m_nRcvBufLen);
#else
		m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���� index[%d] DREB[%d %d] type[%s] �յ�����%d,��������",\
			m_index,m_nDrebId,m_nDrebPrivateId,"BPCSOCK_DREB",m_nRcvBufLen);
#endif
	}
	
	return 0;
}

bool CBF_DrebSocketInfo::NeedConnect()
{
	return m_bNeedConnect;
}

bool CBF_DrebSocketInfo::NeedRead()
{
	return m_bNeedread;
}

bool CBF_DrebSocketInfo::NeedSend()
{
	if (m_sock == INVALID_SOCKET)
	{
		return false;
	}
	return m_pSendQueue.NeedSend();
}

bool CBF_DrebSocketInfo::UnzipBuf(BPCCOMMSTRU &data)
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
				if (m_pDes.Uncryptchar(data.sBuffer,ddlen,DESENCRYPTKEY)<=0)
				{
					m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Uncryptchar error");
					return false;
				}
				data.sDBHead.nLen = ddlen;
				data.sDBHead.cZip =0;
				break;
			case 2:	
				memcpy(zipDataBuf,data.sBuffer,data.sDBHead.nLen);
				if (!CBF_Tools::Uncompress((unsigned char *)(data.sBuffer),dlen,(unsigned char *)zipDataBuf,data.sDBHead.nLen))
				{
					m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Uncompress error");
					return false;
				}
				data.sDBHead.nLen = dlen;
				data.sDBHead.cZip =0;
				break;
			case 3:
				ddlen = data.sDBHead.nLen;
				if (m_pDes.Uncryptchar(data.sBuffer,ddlen,DESENCRYPTKEY3)<=0)
				{
					m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Uncryptchar error");
					return false;
				}
				data.sDBHead.nLen = ddlen;
				data.sDBHead.cZip =0;
				break;
			case 4:
				ddlen = data.sDBHead.nLen;
//				m_log->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"����ǰ����",data.sBuffer,data.sDBHead.nLen);
				if (m_pDes.Uncryptchar(data.sBuffer,ddlen,DESENCRYPTKEY)<=0)
				{
					m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Uncryptchar error");
					return false;
				}
				data.sDBHead.nLen = ddlen;
//				m_log->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"���ܺ�����",data.sBuffer,data.sDBHead.nLen);
				memcpy(zipDataBuf,data.sBuffer,data.sDBHead.nLen);
				if (!CBF_Tools::Uncompress((unsigned char *)(data.sBuffer),dlen,(unsigned char *)zipDataBuf,data.sDBHead.nLen))
				{
					m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Uncompress error");
					return false;
				}
				data.sDBHead.nLen = dlen;
//				m_log->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"��ѹ������",data.sBuffer,data.sDBHead.nLen);
				data.sDBHead.cZip =0;
				break;
			default:
				return false;
		}
	}
	
	return true;
}
bool CBF_DrebSocketInfo::ZipBuf(PBPCCOMMSTRU data,int zipflag)
{
	//0���� 1des������Կ����16 2ѹ�� 3DES���� 4DES���ܲ�ѹ��
	if (data->sDBHead.cZip != 0 || data->sDBHead.nLen<20)
	{
		return true;
	}
	char zipDataBuf[BPUDATASIZE];
	bzero(zipDataBuf,sizeof(zipDataBuf));
	unsigned int dlen = BPUDATASIZE;
	int ddlen;
	switch (zipflag)
	{
		case 1:
			ddlen = data->sDBHead.nLen;
			if (m_pDes.Encryptchar(data->sBuffer,ddlen,DESENCRYPTKEY)<=0)
			{
				m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Encryptchar error");
				return false;
			}
			data->sDBHead.nLen = ddlen;
			data->sDBHead.cZip =1;
			break;
		case 2:	
			memcpy(zipDataBuf,data->sBuffer,data->sDBHead.nLen);
			if (!CBF_Tools::Compress((unsigned char *)(data->sBuffer),dlen,(unsigned char *)(zipDataBuf),data->sDBHead.nLen))
			{
				m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"Compress error");
				return false;
			}
			data->sDBHead.nLen = dlen;
			data->sDBHead.cZip =2;
			break;
		case 3:
			ddlen = data->sDBHead.nLen;
			if (m_pDes.Encryptchar(data->sBuffer,ddlen,DESENCRYPTKEY3)<=0)
			{
				m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Encryptchar error");
				return false;
			}
			data->sDBHead.nLen = ddlen;
			data->sDBHead.cZip =3;
			break;
		case 4:
			memcpy(zipDataBuf,data->sBuffer,data->sDBHead.nLen);
//			m_log->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"ѹ��ǰ����",data->sBuffer,data->sDBHead.nLen);
			if (!CBF_Tools::Compress((unsigned char *)data->sBuffer,dlen,(unsigned char *)(zipDataBuf),data->sDBHead.nLen))
			{
				m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"Compress error");
				return false;
			}
			data->sDBHead.nLen = dlen;
			ddlen = dlen;
//			m_log->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"����ǰ����",data->sBuffer,data->sDBHead.nLen);
			if (m_pDes.Encryptchar(data->sBuffer,ddlen,DESENCRYPTKEY)<=0)
			{
				m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Encryptchar error");
				return false;
			}
			data->sDBHead.nLen = ddlen;
//			m_log->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"���ܺ�����",data->sBuffer,data->sDBHead.nLen);
			data->sDBHead.cZip =4;
			break;
		default:
			data->sDBHead.cZip =0;
			return true;
	}
	return true;
}
void CBF_DrebSocketInfo::CheckFirstData()
{
	RDQUEUE::iterator prq,prn;
	for (prn=m_pSendQueue.m_qSendData.datas.begin(); prn!=m_pSendQueue.m_qSendData.datas.end() ; )
	{
		if (prn->nTotalLen != prn->nTotalLen)
		{
#ifdef _ENGLISH_
			m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"error, the first send part data,delete it %d",prn->index);
#else
			m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"���� �����е�һ�����ݰ����Ͳ������ݣ�������ɾ�� %d",prn->index);
#endif
			m_pSendQueue.DeleteSend(prn);
		}
		break;
	}
}
