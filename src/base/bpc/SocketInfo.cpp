// SocketInfo.cpp: implementation of the CSocketInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "SocketInfo.h"
#include "Bpc_Timer.h"


extern CBpc_Timer  g_pBpcTime;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define  DESENCRYPTKEY    "rd402wms"
#define  DESENCRYPTKEY3   "wms13711songfree"

CSocketInfo::CSocketInfo()
{
	memset(m_sAddress,0,sizeof(m_sAddress));         //��������������IP
	
	ResetInfo();
	m_pMemPool = NULL;
	m_gRes = NULL;
	m_nType = BPCSOCK_UNKNOW;
	m_nCloseTime = 0;
	m_nPid = -1;
	m_sendNum = 10;
	m_bNeedConnect = false;
	m_nBpuGroupIndex = 0;
	m_nBpcHeadLen = sizeof(S_BPC_HEAD);
	m_nDrebHeadLen = DREBHEADLEN;
	m_log= NULL;
	m_DrebIsIpc = false;
	
}

CSocketInfo::~CSocketInfo()
{
	if (m_sock != INVALID_SOCKET)
	{
		closesocket(m_sock);
	}
	m_pMemPool = NULL;
	m_gRes = NULL;
	m_log= NULL;
}
std::string CSocketInfo::GetBpcMsgType(int msgtype)
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

void CSocketInfo::Init(CGResource *res,CBF_BufferPool *pool)
{
	m_pMemPool = pool;
	m_gRes = res;

	m_log= m_gRes->m_log;
	ResetInfo();
	m_nCloseTime = 0;
	m_pDrebEndian.SetCommEndian(true);//ʹ��������
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
	m_nConntime = 0;
	memset(m_cCloseDate,0,sizeof(m_cCloseDate));//���ӹر�����
	m_nCloseTime = 0;
	m_bIsBuGetFlag = 0;
	m_sBpuGroupName = "";
	m_nBpuGroupIndex = 0;
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
	if (m_sBpuGroupName.length()>0)
	{
		if (m_nBpuGroupIndex<m_gRes->g_vBpuGroupInfo.size())
		{
			m_gRes->g_vBpuGroupInfo[m_nBpuGroupIndex].g_nStartBpuNum--;
		}
	}
	ResetInfo();
	m_nCloseTime = time(NULL);
	if (m_nType == BPCSOCK_DREB) //��������������
	{
		m_bNeedConnect = true; //��Ҫ����
		CheckFirstData();

	}
	else
	{
		m_bNeedConnect = false;
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

bool CSocketInfo::Listen(int port,bool blockflag,int queuenum)
{
	if (!m_tcpSocket.Create(AF_INET,false))
	{
		CBF_Tools::StringCopy(m_errMsg,255,m_tcpSocket.GetErrorMsg().c_str());
		return false;
	}
	m_nPort = port;
	if (!m_tcpSocket.Listen(m_nPort,queuenum))
	{
		sprintf(m_errMsg,"�����˿�[%d]ʧ��,�������д�С%d",m_nPort,queuenum);
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

bool CSocketInfo::ListenIpc(int port,bool blockflag,int queuenum)
{
	if (!m_tcpSocket.Create(AF_UNIX,false))
	{
		CBF_Tools::StringCopy(m_errMsg,255,m_tcpSocket.GetErrorMsg().c_str());
		return false;
	}
	m_nPort = port;
	if (!m_tcpSocket.Listen(m_nPort,queuenum))
	{
		sprintf(m_errMsg,"�����˿�[%d]ʧ��,�������д�С%d",m_nPort,queuenum);
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
bool CSocketInfo::IsFree()
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
int CSocketInfo::OnRecv()
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
//		if (m_nType == BPCSOCK_DREB) //dreb�յ�������
//		{
//			m_gRes->PutTime();
//        }
		return ret;

	}
	return 0;
}

int CSocketInfo::OnSend()
{
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
	RDQUEUE::iterator prq,prn;
	unsigned int atimeout;
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
		m_tcpSocket.AttachSocket(m_sock,"");
		
		if (m_nType != BPCSOCK_DREB )
		{
			ret = m_tcpSocket.Send((char *)prq->sMsgBuf+(prq->nTotalLen-prq->nSendLen),prq->nSendLen);
			if (ret == prq->nSendLen)
			{
				g_pBpcTime.ReportSend(prq->nSendLen);
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"���͸�BPU[%s %d] Դ[%d %d %d] ����Ϣ�ɹ� ���ݳ���[%d]",\
					m_sBpuGroupName.c_str(),m_index,prq->s_nNodeId,prq->s_cNodePrivateId,prq->s_nDrebSerial,ret);
				m_pMemPool->PoolFree(prq->sMsgBuf);
				// ����ɹ�,ֹͣ
				m_pSendQueue.DeleteSend(prq);	
				m_nSendtime = time(NULL);
			}
			else //���Ͳ��ɹ�
			{
				if (ret >0)
				{
					g_pBpcTime.ReportSend(ret);
					prq->nSendLen = prq->nSendLen -ret;
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"���͸�BPU[%s %d] Դ[%d %d %d] ����Ϣ���ֳɹ� ���ݳ���[%d]",\
						m_sBpuGroupName.c_str(),m_index,prq->s_nNodeId,prq->s_cNodePrivateId,prq->s_nDrebSerial,ret);
				}
				//�´��ٷ���ֻ��������ɻ����ӹر�
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"���͸�BPU[%s %d] Դ[%d %d %d] ����Ϣʧ�� ret[%d]",\
					m_sBpuGroupName.c_str(),m_index,prq->s_nNodeId,prq->s_cNodePrivateId,prq->s_nDrebSerial,ret);
				//���Ͳ��ɹ����´��ٷ������Ǵ������ٷ�
				return 0;

			}
			
		}
		else   //����DREB������
		{
			ret = m_tcpSocket.Send((char *)&(prq->sMsgBuf->sDBHead)+(prq->nTotalLen-prq->nSendLen),prq->nSendLen);
			if (ret == prq->nSendLen)
			{
				//ͳ������
				g_pBpcTime.ReportSend(prq->nSendLen);
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"���͸�index[%d] DREB[%d %d] Դ[%d %d %d] ����Ϣ�ɹ� ���ݳ���[%d]",\
					m_index,m_nDrebId,m_nDrebPrivateId,prq->s_nNodeId,prq->s_cNodePrivateId,prq->s_nDrebSerial,ret);
				m_pMemPool->PoolFree(prq->sMsgBuf);
				// ����ɹ�,ֹͣ
				m_pSendQueue.DeleteSend(prq);	
				m_nSendtime = time(NULL);
				
			}
			else //ֹͣ
			{
				if (ret >0)
				{
					g_pBpcTime.ReportSend(ret);
					prq->nSendLen = prq->nSendLen -ret;
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"���͸�index[%d] DREB[%d %d] Դ[%d %d %d] ����Ϣ���ֳɹ� ���ݳ���[%d]",\
						m_index,m_nDrebId,m_nDrebPrivateId,prq->s_nNodeId,prq->s_cNodePrivateId,prq->s_nDrebSerial,ret);
				}
				else
				{
					//�´��ٷ���ֱ���ɹ������ӹر�
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"���͸�index[%d] DREB[%d %d] Դ[%d %d %d] ����Ϣʧ�� ret[%d]",\
						m_index,m_nDrebId,m_nDrebPrivateId,prq->s_nNodeId,prq->s_cNodePrivateId,prq->s_nDrebSerial,ret);
				}
				//���Ͳ��ɹ����´��ٷ������Ǵ������ٷ�
				return 0;
			}
		}
		
	}
	

	return sendNum;
}


int CSocketInfo::SendMsg(S_BPC_RSMSG &msg,bool sendimmediate)
{
	int ret=0;
	CBF_PMutex pp(&m_mutex);
	m_tcpSocket.AttachSocket(m_sock,"");
	//��DREBHEAD�е����ݷ���MSG�У�������ʱд��־���и���
	msg.s_nNodeId = msg.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId;
	msg.s_nDrebSerial = msg.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial;
	msg.s_cNodePrivateId = msg.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId;
	if (m_nType != BPCSOCK_DREB )
	{
		//��bpu��Ҫ�����ķ���
		if (m_gRes->g_vBpuGroupInfo[m_nBpuGroupIndex].g_bIsUnencrypt)
		{
			if (msg.sMsgBuf->sDBHead.nLen >0)
			{
				if (!UnzipBuf(*(msg.sMsgBuf)))
				{
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"���� ��ѹ�����ܳ��� ���͸�BPU[%s %d]����Ϣ[%s]  DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Դ[%d %d] cZip[%d] ҵ�����ݳ���[%d] ",\
						m_sBpuGroupName.c_str(),m_index,GetBpcMsgType(msg.sMsgBuf->sBpcHead.cMsgType).c_str(),\
						GetDrebCmdType(msg.sMsgBuf->sDBHead.cCmd).c_str(),msg.sMsgBuf->sDBHead.cNextFlag,\
						msg.sMsgBuf->sDBHead.cRaflag,msg.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,msg.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
						msg.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,msg.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
						msg.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,msg.sMsgBuf->sDBHead.s_Sinfo.s_nHook,msg.sMsgBuf->sDBHead.cZip,msg.sMsgBuf->sDBHead.nLen);
// 					m_pMemPool->PoolFree(msg.sMsgBuf);
// 					msg.sMsgBuf = NULL;
					return -101;
				}
			}
			
		}
		msg.sMsgBuf->sBpcHead.nBpcLen = msg.sMsgBuf->sDBHead.nLen +DREBHEADLEN;
		msg.nTotalLen = msg.sMsgBuf->sDBHead.nLen+BPCHEADLEN+DREBHEADLEN;
		msg.nSendLen = msg.nTotalLen;
		if (msg.sMsgBuf->sBpcHead.cMsgType != MSG_FREE && msg.sMsgBuf->sBpcHead.cMsgType != MSG_BPUISREG && msg.sMsgBuf->sBpcHead.cMsgType != MSG_BPUREG)
		{
			m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���͸�BPU[%s %d]����Ϣ[%s]  DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Դ[%d %d] cZip[%d] ҵ�����ݳ���[%d]",\
				m_sBpuGroupName.c_str(),m_index,GetBpcMsgType(msg.sMsgBuf->sBpcHead.cMsgType).c_str(),\
				GetDrebCmdType(msg.sMsgBuf->sDBHead.cCmd).c_str(),msg.sMsgBuf->sDBHead.cNextFlag,\
				msg.sMsgBuf->sDBHead.cRaflag,msg.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,msg.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
				msg.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,msg.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
				msg.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,msg.sMsgBuf->sDBHead.s_Sinfo.s_nHook,msg.sMsgBuf->sDBHead.cZip,msg.sMsgBuf->sDBHead.nLen);
		}
		else
		{
			m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���͸�BPU[%s %d]����Ϣ[%s] ҵ�����ݳ���[%d]",\
				m_sBpuGroupName.c_str(),m_index,GetBpcMsgType(msg.sMsgBuf->sBpcHead.cMsgType).c_str(),msg.sMsgBuf->sDBHead.nLen);
		}
		//ת����ͨѶ�ֽ���,����Ϊ�����ֽ���

		if (m_gRes->g_vBpuGroupInfo[m_nBpuGroupIndex].g_nUseBigEndian == 1) //java���bpuʹ��������ʱ
		{
			m_pBpcEndian.SetCommEndian(false);
		}
		else
		{
			m_pBpcEndian.SetCommEndian(true);
		}
		
		m_pBpcEndian.Endian2Comm(msg.sMsgBuf);
		m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"׼������BPU");
		//���Ͷ����������ݣ�����뷢�Ͷ��У���˳����
		if (NeedSend())
		{
			m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���Ͷ����������ݣ�����BPU���� index[%d] ���ݳ���[%d] ��ʶ[%d %d %d] ",\
				m_index,msg.nTotalLen,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
			PutSendMsg(msg);
			return 0;
		}
		else  //ֱ�ӷ���
		{
			if (m_sock == INVALID_SOCKET)
			{
				m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"�����ѶϿ�������BPU���� index[%d] ���ݳ���[%d] ��ʶ[%d %d %d] ",\
					m_index,msg.nTotalLen,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
				PutSendMsg(msg);
				return -2;
			}
			ret = m_tcpSocket.Send((char *)(msg.sMsgBuf),msg.nTotalLen);
			if (ret == msg.nTotalLen)
			{
				m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���͸�BPU��� index[%d] ��ʶ[%d %d %d] ",\
					m_index,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
				g_pBpcTime.ReportSend(msg.nTotalLen);
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
// 				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"���� ���͸�BPU[%d]����Ϣ����,������� ret����[%d] msg[%s] ���볤��[%d]",\
// 					m_index,ret,m_tcpSocket.GetErrorMsg().c_str(),msg.nSendLen);
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"���� ���͸�BPU index[%d]����Ϣ����,������� ret����[%d] msg[%s] ���볤��[%d] ���ݳ���[%d] ��ʶ[%d %d %d] ",\
					m_index,ret,m_tcpSocket.GetErrorMsg().c_str(),msg.nSendLen,msg.nTotalLen,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
				PutSendMsg(msg);
				return 0;
			}
		}
		return 0;
		
	}
	else   //����DREB������
	{
		msg.nTotalLen = msg.sMsgBuf->sDBHead.nLen+DREBHEADLEN;
		msg.nSendLen = msg.nTotalLen;
		if (msg.sMsgBuf->sDBHead.cCmd != CMD_PING && msg.sMsgBuf->sDBHead.cCmd != CMD_CONNECT && msg.sMsgBuf->sDBHead.cCmd != CMD_REGSERVICE)
		{
			m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���͸�DREB[%d]����Ϣ DREB����[%s] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Դ[%d %d] cZip[%d] ҵ�����ݳ���[%d]",\
				m_index,GetDrebCmdType(msg.sMsgBuf->sDBHead.cCmd).c_str(),\
				msg.sMsgBuf->sDBHead.cRaflag,msg.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,msg.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
				msg.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,msg.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
				msg.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,msg.sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
				msg.sMsgBuf->sDBHead.cZip,msg.sMsgBuf->sDBHead.nLen);
		}
		else
		{
			m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���͸�DREB[%d]����Ϣ DREB����[%s]  ҵ�����ݳ���[%d]",\
				m_index,GetDrebCmdType(msg.sMsgBuf->sDBHead.cCmd).c_str(),msg.sMsgBuf->sDBHead.nLen);
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
				m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"�����ѶϿ�������DREB���� ���ݳ���[%d] ��ʶ[%d %d %d] ",msg.nTotalLen,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
				PutSendMsg(msg);
				return -2;
			}
			ret = m_tcpSocket.Send((char *)&(msg.sMsgBuf->sDBHead),msg.nTotalLen);
			if (ret == msg.nTotalLen)
			{
				m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"������� index[%d] ��ʶ[%d %d %d] ",\
					m_index,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
				//ͳ������
				g_pBpcTime.ReportSend(msg.nTotalLen);
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
// 				m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"���� ���͸�DREB[%d]����Ϣ���� ������� ret����[%d] msg[%s] ���볤��[%d]",\
// 					m_index,ret,m_tcpSocket.GetErrorMsg().c_str(),msg.nSendLen);
				m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"���� ���͸�DREB[%d]����Ϣ����,������� ret����[%d] msg[%s] ���볤��[%d] ���ݳ���[%d] ��ʶ[%d %d %d] ",\
					m_index,ret,m_tcpSocket.GetErrorMsg().c_str(),msg.nSendLen,msg.nTotalLen,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
				//ֻҪ���ͳ���ͷŵ����У���������������ݿ��ܾͻ�������
				if (!m_pSendQueue.NeedSend())
				{
					PutSendMsg(msg);
				}
				else
				{
					OnClose("ֱ�������ӷ������ݲ��ɹ��������Ӷ�������������",__FILE__,__LINE__);
				}
				return 0;
			}
			
			
		}
		else
		{
			if (NeedSend()) //��������
			{
				m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���Ͷ����������ݣ�����DREB���� index[%d] ���ݳ���[%d] ��ʶ[%d %d %d] ",\
					m_index,msg.nTotalLen,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
				if (msg.sMsgBuf->sBpcHead.cMsgType != MSG_BPCMONITOR)
				{
					PutSendMsg(msg);
					return 0;
				}
				
			}
			else  //ֱ�ӷ���
			{
				if (m_sock == INVALID_SOCKET)
				{
					m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"�����ѶϿ�������DREB���� index[%d] ���ݳ���[%d] ��ʶ[%d %d %d] ",\
						m_index,msg.nTotalLen,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
					PutSendMsg(msg);
					return -2;
				}
				ret = m_tcpSocket.Send((char *)&(msg.sMsgBuf->sDBHead),msg.nTotalLen);
				if (ret == msg.nTotalLen)
				{
					m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"������� index[%d] ��ʶ[%d %d %d] ",\
						m_index,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
					//ͳ������
					g_pBpcTime.ReportSend(msg.nTotalLen);
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
// 					m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"���� ���͸�DREB[%d]����Ϣ���� ������� ret����[%d] msg[%s] ���볤��[%d]",\
// 						m_index,ret,m_tcpSocket.GetErrorMsg().c_str(),msg.nSendLen);
					m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"���� ���͸�DREB[%d]����Ϣ����,������� ret����[%d] msg[%s] ���볤��[%d] ���ݳ���[%d] ��ʶ[%d %d %d] ",\
						m_index,ret,m_tcpSocket.GetErrorMsg().c_str(),msg.nSendLen,msg.nTotalLen,msg.s_nNodeId,msg.s_cNodePrivateId,msg.s_nDrebSerial);
					//ֻҪ���ͳ���ͷ������
					PutSendMsg(msg);
					return 0;
				}
			}
		}
		
			
	}
	return ret;
}

int CSocketInfo::PutSendMsg(S_BPC_RSMSG msg)
{
	int ret;
	ret = m_pSendQueue.PutSendMsg(msg);
	if (ret<0)
	{
		m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"������ ���� DREB[%d]����Ϣ DREB����[%s] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Դ[%d %d] cZip[%d] ҵ�����ݳ���[%d]",\
			m_index,GetDrebCmdType(msg.sMsgBuf->sDBHead.cCmd).c_str(),\
			msg.sMsgBuf->sDBHead.cRaflag,msg.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,msg.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
			msg.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,msg.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
			msg.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,msg.sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
				msg.sMsgBuf->sDBHead.cZip,msg.sMsgBuf->sDBHead.nLen);
		m_pMemPool->PoolFree(msg.sMsgBuf);
		msg.sMsgBuf = NULL;
		return -1;
	}
	return 0;
}



int CSocketInfo::GetRecvData(S_BPC_RSMSG *msg)
{
	int txlen=0;
	char tmpchar[100];
	memset(tmpchar,0,sizeof(tmpchar));
	if (msg->sMsgBuf == NULL)
	{
		return -100;
	}
	if (m_nType == BPCSOCK_DREB) //dreb�յ�������
	{
		//�յ������ݴ��ڰ�ͷ
		if (m_nRcvBufLen >= DREBHEADLEN )
		{
//			m_gRes->PutTime();
			//�����ֽ�����У��CRC
			memcpy(&(msg->sMsgBuf->sDBHead),m_sRcvBuffer,DREBHEADLEN);
			if (m_gRes->g_nCrcFlag == 1)
			{
				if (!m_pDrebEndian.Endian2LocalHostChkCrc(&(msg->sMsgBuf->sDBHead)) )
				{
					OnClose("���� CRC��",__FILE__,__LINE__);
					return -1;
				}
			}
			else
			{
				m_pDrebEndian.Endian2LocalHost(&(msg->sMsgBuf->sDBHead));

			}
			if (m_nRcvBufLen < msg->sMsgBuf->sDBHead.nLen + DREBHEADLEN)
			{
				//˵������û�н�������
				return 0;
			}
			if (msg->sMsgBuf->sDBHead.nLen >=0 && msg->sMsgBuf->sDBHead.nLen <= BPUDATASIZE ) 
			{
				if (msg->sMsgBuf->sDBHead.nLen >0)
				{
					memcpy(msg->sMsgBuf->sBuffer,m_sRcvBuffer+DREBHEADLEN,msg->sMsgBuf->sDBHead.nLen);
				}
			}
			else
			{
				OnClose("���� ���ӷ�DREB",__FILE__,__LINE__);
				return -1;
			}
//			m_gRes->PutTime();
			
			msg->sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN+msg->sMsgBuf->sDBHead.nLen;
			msg->sMsgBuf->sBpcHead.nBpuIndex = 0;
			msg->sMsgBuf->sBpcHead.nConnectTime = m_nConntime;
			msg->sMsgBuf->sBpcHead.nIndex = m_index; 
			msg->sMsgBuf->sBpcHead.cMsgType = MSG_REQ;
			msg->sMsgBuf->sBpcHead.nRtime = time(NULL);
			
			//ͳ������
			g_pBpcTime.ReportRcv(msg->sMsgBuf->sBpcHead.nBpcLen);
			if (msg->sMsgBuf->sDBHead.cCmd != CMD_PING && msg->sMsgBuf->sDBHead.cCmd != CMD_CONNECT && msg->sMsgBuf->sDBHead.cCmd != CMD_REGSERVICE)
			{
				if (m_log->isWrite(LOG_DEBUG))
				{
					m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"��index[%d] DREB[%d %d] �յ�ԴDREB[%d %d]�յ�����Ϣ DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Դ[%d %d] cZip[%d] ҵ�����ݳ���[%d]",\
						m_index,m_nDrebId,m_nDrebPrivateId,msg->sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,msg->sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,\
						GetDrebCmdType(msg->sMsgBuf->sDBHead.cCmd).c_str(),msg->sMsgBuf->sDBHead.cNextFlag,\
						msg->sMsgBuf->sDBHead.cRaflag,msg->sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,msg->sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
						msg->sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,msg->sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
						msg->sMsgBuf->sDBHead.s_Sinfo.s_nSerial,msg->sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
						msg->sMsgBuf->sDBHead.cZip,msg->sMsgBuf->sDBHead.nLen);
				}
				
			}
			else
			{
				if (m_log->isWrite(LOG_DEBUG+1))
				{
					m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���յ�index[%d] DREB[%d %d]����Ϣ DREB����[%s]  ҵ�����ݳ���[%d]",\
						m_index,m_nDrebId,m_nDrebPrivateId,GetDrebCmdType(msg->sMsgBuf->sDBHead.cCmd).c_str(),msg->sMsgBuf->sDBHead.nLen);
				}
			}
			//����Ӧ����Ϊ���ɿ����ͻ�ȫ��㲥��Ϣ��ֱ�Ӷ���
			if (msg->sMsgBuf->sDBHead.cRaflag == 1 && (msg->sMsgBuf->sDBHead.cCmd == CMD_DPPUSH || msg->sMsgBuf->sDBHead.cCmd == CMD_DPABC))
			{
//				m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"CMD_DPPUSH��CMD_DPABC��Ӧ�𣬶���");
			}
			else
			{

			}
			

			//�ն��ˣ���ԭ�����¸�m_sRcvBuffer
			m_nRcvBufLen=m_nRcvBufLen - DREBHEADLEN-msg->sMsgBuf->sDBHead.nLen;
			for (int i=0; i<m_nRcvBufLen; i++)
			{
				m_sRcvBuffer[i]=m_sRcvBuffer[i+DREBHEADLEN+msg->sMsgBuf->sDBHead.nLen];
			}
			
//			m_gRes->PutTime();
			return DREBHEADLEN+msg->sMsgBuf->sDBHead.nLen;
		}
		else if (m_nRcvBufLen>0)
		{
			m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"���� index[%d] DREB[%d %d] type[%s] �յ�����%d,��������",\
				m_index,m_nDrebId,m_nDrebPrivateId,"BPCSOCK_DREB",m_nRcvBufLen);
		}
	}
	else  // if (m_nType == BPCSOCK_BPU || m_nType == BPCSOCK_CLI) //��BPU����
	{
	    m_log->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"BPU[%s %d %d] type[%s] �յ�����%d",\
			m_sBpuGroupName.c_str(),m_index,m_nBpuGroupIndex,"BPU_SOCKET",m_nRcvBufLen);
		//�յ������ݴ��ڰ�ͷ
		if (m_nRcvBufLen >= BPCHEADLEN+DREBHEADLEN)
		{
			memcpy(&(msg->sMsgBuf->sBpcHead),m_sRcvBuffer,BPCHEADLEN);
			if (msg->sMsgBuf->sBpcHead.nMsgFlag != LMSGID)
			{
				m_pBpcEndian.SetCommEndian(false);//�Է�ʹ��������
			}
			else
			{
				m_pBpcEndian.SetCommEndian(true);
			}
			memcpy(&(msg->sMsgBuf->sDBHead),m_sRcvBuffer+BPCHEADLEN,DREBHEADLEN);
			//�ֽ�����
			m_pBpcEndian.Endian2LocalHost(msg->sMsgBuf);

			//��sDBHead.nLenΪ׼���㳤��
			if (m_nRcvBufLen < BPCHEADLEN+DREBHEADLEN+msg->sMsgBuf->sDBHead.nLen)
			{
				if (m_nRcvBufLen >= BPCBUFSIZE || msg->sMsgBuf->sBpcHead.nBpcLen>BPUDATASIZE+DREBHEADLEN || msg->sMsgBuf->sDBHead.nLen>BPUDATASIZE)
				{
					OnClose("���� ���ӷ�BPU,����ͷ����",__FILE__,__LINE__);
					return -1;
				}
				m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"���� BPU[%s  %d] ����û�н�������[%d] < HEADLEN[%d]+nLen[%d]",\
					m_sBpuGroupName.c_str(),m_index,m_nRcvBufLen,BPCHEADLEN+DREBHEADLEN,msg->sMsgBuf->sDBHead.nLen);
				//˵������û�н�������
				return 0;
			}
			if (msg->sMsgBuf->sDBHead.nLen>0 && msg->sMsgBuf->sDBHead.nLen<BPUDATASIZE)
			{
				memcpy(msg->sMsgBuf->sBuffer,m_sRcvBuffer+(BPCHEADLEN+DREBHEADLEN),msg->sMsgBuf->sDBHead.nLen);
				msg->sMsgBuf->sBuffer[msg->sMsgBuf->sDBHead.nLen]=0;
			}
			else if (msg->sMsgBuf->sDBHead.nLen >= BPUDATASIZE)
			{
				OnClose("���� ���ӷ�BPU,���������",__FILE__,__LINE__);
				return -1;
			}
			if (msg->sMsgBuf->sBpcHead.cMsgType != MSG_FREE && msg->sMsgBuf->sBpcHead.cMsgType != MSG_BPUISREG && msg->sMsgBuf->sBpcHead.cMsgType != MSG_BPUREG)
			{
				m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���յ�BPU[%s %d]����Ϣ[%s]  DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Դ[%d %d] cZip[%d] ҵ�����ݳ���[%d]",\
					m_sBpuGroupName.c_str(),m_index,GetBpcMsgType(msg->sMsgBuf->sBpcHead.cMsgType).c_str(),\
					GetDrebCmdType(msg->sMsgBuf->sDBHead.cCmd).c_str(),msg->sMsgBuf->sDBHead.cNextFlag,\
					msg->sMsgBuf->sDBHead.cRaflag,msg->sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,\
					msg->sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
					msg->sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,msg->sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
					msg->sMsgBuf->sDBHead.s_Sinfo.s_nSerial,msg->sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
					msg->sMsgBuf->sDBHead.cZip,msg->sMsgBuf->sDBHead.nLen);
			}
			else
			{
				m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���յ�BPU[%s %d]����Ϣ[%s] ҵ�����ݳ���[%d]",\
					m_sBpuGroupName.c_str(),m_index,GetBpcMsgType(msg->sMsgBuf->sBpcHead.cMsgType).c_str(),msg->sMsgBuf->sDBHead.nLen);
			}
			CBF_Tools::StringCopy(msg->sBpuGroupName,20,m_sBpuGroupName.c_str());
			msg->nBpuGroupIndex = m_nBpuGroupIndex;
			//��������Ļ������յ����������index�ŵ�bpcͷ��nBpuIndex��
			if (msg->sMsgBuf->sDBHead.cRaflag == 0) 
			{
				msg->sMsgBuf->sBpcHead.nBpuIndex = m_index;//ֻ��������ʱ��Ҫ���£���Ϊ���ڵ����ڵ����ݴ�ֵ���ظ����õ�bpu
				//m_gRes->m_pBpcCallDataLog.LogBpc(LOG_PROMPT,msg->sMsgBuf,false);
			}
			else
			{
				//m_gRes->m_pDrebDataLog.LogBpc(LOG_PROMPT,msg->sMsgBuf,false);
			}
			//g_pBpcLog.LogBpc(LOG_PROMPT,msg->sMsgBuf,false);
			//�ն��ˣ���ԭ�����¸�m_sRcvBuffer
			for (int i=0; i<m_nRcvBufLen-BPCHEADLEN-DREBHEADLEN-msg->sMsgBuf->sDBHead.nLen; i++)
			{
				m_sRcvBuffer[i]=m_sRcvBuffer[i+BPCHEADLEN+DREBHEADLEN+msg->sMsgBuf->sDBHead.nLen];
			}
			m_nRcvBufLen = m_nRcvBufLen - BPCHEADLEN -DREBHEADLEN-msg->sMsgBuf->sDBHead.nLen;
			
			
			return BPCHEADLEN+DREBHEADLEN+msg->sMsgBuf->sDBHead.nLen;
		}
		else if (m_nRcvBufLen>0)
		{
			m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"���� BPU[%s %d] type[%s] �յ�����%d,��������",\
				m_sBpuGroupName.c_str(),m_index,"BPCSOCK_BPU",m_nRcvBufLen);
		}
	}
	return 0;
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
void CSocketInfo::SetAccept(SOCKET_HANDLE sock,const char *address)
{
	ResetInfo();
	m_sock = sock;
	m_bChecked = false;
	m_bNeedread = true;
	m_nConntime = time(NULL);
	strcpy(m_sAddress,address);
	m_datetime.Update();
	m_datetime.ToCharDate(m_cCreateDate);
	m_nType = BPCSOCK_BPU;
}

bool CSocketInfo::UnzipBuf(BPCCOMMSTRU &data)
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
					m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
					return false;
				}
				data.sDBHead.nLen = ddlen;
				data.sDBHead.cZip =0;
				break;
			case 2:	
				memcpy(zipDataBuf,data.sBuffer,data.sDBHead.nLen);
				if (!CBF_Tools::Uncompress((unsigned char *)(data.sBuffer),dlen,(unsigned char *)zipDataBuf,data.sDBHead.nLen))
				{
					m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"��ѹ������");
					return false;
				}
				data.sDBHead.nLen = dlen;
				data.sDBHead.cZip =0;
				break;
			case 3:
				ddlen = data.sDBHead.nLen;
				if (m_pDes.Uncryptchar(data.sBuffer,ddlen,DESENCRYPTKEY3)<=0)
				{
					m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
					return false;
				}
				data.sDBHead.nLen = ddlen;
				data.sDBHead.cZip =0;
				break;
			case 4:
				ddlen = data.sDBHead.nLen;
				m_log->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"����ǰ����",data.sBuffer,data.sDBHead.nLen);
				if (m_pDes.Uncryptchar(data.sBuffer,ddlen,DESENCRYPTKEY)<=0)
				{
					m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
					return false;
				}
				data.sDBHead.nLen = ddlen;
				m_log->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"���ܺ�����",data.sBuffer,data.sDBHead.nLen);
				memcpy(zipDataBuf,data.sBuffer,data.sDBHead.nLen);
				if (!CBF_Tools::Uncompress((unsigned char *)(data.sBuffer),dlen,(unsigned char *)zipDataBuf,data.sDBHead.nLen))
				{
					m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"��ѹ������");
					return false;
				}
				data.sDBHead.nLen = dlen;
				m_log->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"��ѹ������",data.sBuffer,data.sDBHead.nLen);
				data.sDBHead.cZip =0;
				break;
			default:
				return false;
		}
	}
	
	return true;
}
bool CSocketInfo::ZipBuf(PBPCCOMMSTRU data,int zipflag)
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
				m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
				return false;
			}
			data->sDBHead.nLen = ddlen;
			data->sDBHead.cZip =1;
			break;
		case 2:	
			memcpy(zipDataBuf,data->sBuffer,data->sDBHead.nLen);
			if (!CBF_Tools::Compress((unsigned char *)(data->sBuffer),dlen,(unsigned char *)(zipDataBuf),data->sDBHead.nLen))
			{
				m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"����ѹ��ʧ��");
				return false;
			}
			data->sDBHead.nLen = dlen;
			data->sDBHead.cZip =2;
			break;
		case 3:
			ddlen = data->sDBHead.nLen;
			if (m_pDes.Encryptchar(data->sBuffer,ddlen,DESENCRYPTKEY3)<=0)
			{
				m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
				return false;
			}
			data->sDBHead.nLen = ddlen;
			data->sDBHead.cZip =3;
			break;
		case 4:
			memcpy(zipDataBuf,data->sBuffer,data->sDBHead.nLen);
			m_log->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"ѹ��ǰ����",data->sBuffer,data->sDBHead.nLen);
			if (!CBF_Tools::Compress((unsigned char *)data->sBuffer,dlen,(unsigned char *)(zipDataBuf),data->sDBHead.nLen))
			{
				m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"����ѹ��ʧ��");
				return false;
			}
			data->sDBHead.nLen = dlen;
			ddlen = dlen;
			m_log->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"����ǰ����",data->sBuffer,data->sDBHead.nLen);
			if (m_pDes.Encryptchar(data->sBuffer,ddlen,DESENCRYPTKEY)<=0)
			{
				m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
				return false;
			}
			data->sDBHead.nLen = ddlen;
			m_log->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"���ܺ�����",data->sBuffer,data->sDBHead.nLen);
			data->sDBHead.cZip =4;
			break;
		default:
			data->sDBHead.cZip =0;
			return true;
	}
	return true;
}
void CSocketInfo::CheckFirstData()
{
	RDQUEUE::iterator prq,prn;
	for (prn=m_pSendQueue.m_qSendData.datas.begin(); prn!=m_pSendQueue.m_qSendData.datas.end() ; )
	{
		if (prn->nTotalLen != prn->nTotalLen)
		{
			m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"���� �����е�һ�����ݰ����Ͳ������ݣ�������ɾ�� %d",prn->index);
			m_pSendQueue.DeleteSend(prn);
		}
		break;
	}
}

void CSocketInfo::SetBpuFreeFlag(bool isfree)
{
	CBF_PMutex pp(&m_mutexFreeFlag);
	m_bIsBuGetFlag = isfree;
	return;
}

bool CSocketInfo::IsBpuFree()
{
 	return m_bIsBuGetFlag;
}
