// SendThread.cpp: implementation of the CSendThread class.
//
//////////////////////////////////////////////////////////////////////

#include "SendThread.h"
#include "SubScribeInfo.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define  DESENCRYPTKEY3   "wms13711songfree"
#define  DESENCRYPTKEY    "rd402wms"

CSendThread::CSendThread()
{
	m_pSendData = NULL;
	m_pRes = NULL;
	m_pAioWork = NULL;
	bzero(m_sDestBuffer,sizeof(m_sDestBuffer));
	bzero(m_sSrcBuffer,sizeof(m_sSrcBuffer));
	m_nSendQuoteNum =0;
	m_nSendCustNum = 0;
	m_nIndex = 0;
}

CSendThread::~CSendThread()
{

}
void CSendThread::ExitThreadInstance()
{
	return ;
}
bool CSendThread::InitThreadInstance()
{
	return true;
}
bool CSendThread::SetGlobalVar(CGateResource *res,CSendData *senddata,CAIO_Work *aiowork)
{
 	m_pRes = res;
 	m_pSendData = senddata;
	m_pAioWork = aiowork;
 	m_pLog = m_pRes->g_pLog;
	m_pGateLog = &(m_pRes->g_pGateLog);
	return true;
}
int CSendThread::Run()
{

	m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"�����ͻ������߳�");
	int rtime = time(NULL);
	int etime;
	while (!m_pRes->g_bIsExit)
	{
		if (m_pSendData->GetData(m_pSmsg))
		{
			//�����ݽ��н���(�����߹����������Ǽ��ܵ������)
			if (!UnzipDrebBuf(m_pSmsg))
			{
				m_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, "�������ݽ���ʧ��");
				continue;
			}
			if (m_pSmsg.isBC == 0)	 //��ʾӦ�������
			{
				SendMsg(&m_pSmsg);
			}
			else  if (m_pSmsg.isBC == 1)	 //����㲥�ȹ�����Ϣ
			{
                //���͹㲥
                SendBCMsg(&m_pSmsg);
			}
			else	//���ĵ���Ϣ
			{
				SendSubscribeMsg(&m_pSmsg);
			}
			
			etime = time(NULL);
			if ((etime - rtime) > 20)
			{
				rtime = etime;
				m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"���Ͷ�����[%d]",m_pSendData->GetSize());
			}
		}
	}
	m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"ֹͣ�ͻ������߳�");
	return 0;
}

void CSendThread::SendMsg(S_CGATE_SMSG *msg)
{
	int nRet;
	PSOCKET_POOL_DATA info = m_pAioWork->GetPoolData(msg->index);
	if (info == NULL)
	{
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"����index[%d]�Ƿ�",msg->index);
		return ;
	}
	if (info->s_cCheckFlag != CHECKFLAG_NORMAL) //δ��֤ͨ�����ݲ�����
	{
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"����index[%d] δ��¼ %d",msg->index,info->s_cCheckFlag );
		return ;
	}
	if (info->s_hSocket == INVALID_SOCKET)
	{
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"����index[%d]�ѹر�",msg->index);
		return ;
	}
	
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"��ʼ���ͽ������� index[%d] ��ʶ[%d %d %d %d] cMsgType[%s] cCmd[%s] cRaFlag[%d] cNextFlag[%d] Node[%d %d] svr[%d %d] ������[%d] next[%d %d] czip[%d] nLen[%d]",\
		msg->index,msg->d_nNodeId,msg->d_cNodePrivateId,msg->s_nDrebSerial,msg->data.head.stDest.nSerial,GetMsgType(msg->data.head.stComm.cMsgType).c_str(),\
		GetDrebCmdType(msg->data.head.stComm.cCmd).c_str(),msg->data.head.stComm.cRaflag,\
		msg->data.head.stComm.cNextFlag,msg->data.head.stDest.nNodeId,msg->data.head.stDest.cNodePrivateId,\
		msg->data.head.stDest.nSvrMainId,msg->data.head.stDest.cSvrPrivateId,msg->data.head.stDest.nServiceNo,\
		msg->data.head.stNext.n_nNextNo,msg->data.head.stNext.n_nNextOffset,msg->data.head.stComm.cZip,msg->data.head.nLen);
	
	if (msg->data.head.stComm.cMsgType != MSG_GATEPING && msg->data.head.stComm.cMsgType != MSG_GATESUBSCRIBE)
	{
		if (msg->timestamp == 0)
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ͽ�����������������͹㲥����");
			return;
		}
	}
	if (m_pRes->g_nEncryptFlag == 1  )
	{
		if (!Encrypt(msg))
		{
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���ݼ��ܳ��� index[%d] ��ʶ[%d %d %d %d] cMsgType[%s] cCmd[%s] cRaFlag[%d] cNextFlag[%d] Node[%d %d] svr[%d %d] ������[%d] next[%d %d] czip[%d] nLen[%d]",\
				msg->index,msg->d_nNodeId,msg->d_cNodePrivateId,msg->s_nDrebSerial,msg->data.head.stDest.nSerial,GetMsgType(msg->data.head.stComm.cMsgType).c_str(),\
				GetDrebCmdType(msg->data.head.stComm.cCmd).c_str(),msg->data.head.stComm.cRaflag,\
				msg->data.head.stComm.cNextFlag,msg->data.head.stDest.nNodeId,msg->data.head.stDest.cNodePrivateId,\
				msg->data.head.stDest.nSvrMainId,msg->data.head.stDest.cSvrPrivateId,msg->data.head.stDest.nServiceNo,\
				msg->data.head.stNext.n_nNextNo,msg->data.head.stNext.n_nNextOffset,msg->data.head.stComm.cZip,msg->data.head.nLen);
			return ;
		}
	}
	SOCKET_SEND_DATA dd;
	dd.s_nIndex = info->s_nIndex;
	dd.s_nSendLen = 0;
	dd.s_nRet = 0;
	dd.s_nSendNum = 0;
	dd.s_nTimestamp = info->s_nTimestamp;
	dd.s_nTotalLen = CGATEHEADLEN+msg->data.head.nLen;
	dd.s_nHook = msg->data.head.stDest.nSerial;

	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���ͳ���[%d] ���ݳ���[%d] ����ͷ����[%d] cZip[%d] index[%d] ��ʶ[%d]",\
		dd.s_nTotalLen,msg->data.head.nLen,CGATEHEADLEN,msg->data.head.stComm.cZip,info->s_nIndex,msg->data.head.stDest.nSerial);

	//�ֽ���ת��
	CGATE_HEAD cgatehead;
	memcpy(&(cgatehead),&(msg->data.head),CGATEHEADLEN);

	Endian2CommCrc(cgatehead);
	
	memcpy(dd.s_sSendBuffer,&(cgatehead),CGATEHEADLEN);
	if (dd.s_nTotalLen > DREBBUFSIZE)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"�������ݳ���[%d]���� ",dd.s_nTotalLen);
		return;
	}
	else
	{
		memcpy(dd.s_sSendBuffer+CGATEHEADLEN,msg->data.buffer,msg->data.head.nLen);
	}
	if (info->s_hSocket == INVALID_SOCKET)
	{
		m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"���ͽ�������ʧ�� ����index[%d]�ѹر� ��ʶ[%d]",msg->index,msg->data.head.stDest.nSerial);
		return ;
	}
	if (msg->data.head.stComm.cMsgType != MSG_GATEPING && msg->data.head.stComm.cMsgType != MSG_GATESUBSCRIBE)
	{
		if (info->s_nTimestamp != msg->timestamp)
		{
			m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"index[%d] ����ʱ���[%d]�����ӵ�[%d]���� ��ʶ[%d]",\
				msg->index,msg->timestamp,info->s_nTimestamp,msg->data.head.stDest.nSerial);
			return ;
		}
	}
	dd.s_nTxCode = msg->txcode;
	dd.s_serial  = msg->s_nDrebSerial;
	dd.s_nTimestamp = msg->timestamp;
	dd.s_nSendLen = 0;
	//��������
	if (!m_pAioWork->Send2Client(dd))
	{
		m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"���ͽ�������ʧ�� ��ʶ[%d %d %d %d] ",msg->d_nNodeId,msg->d_cNodePrivateId,msg->s_nDrebSerial,msg->data.head.stDest.nSerial);
		return ;
	}
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���ͽ���������� ��ʶ[%d %d %d %d] ",msg->d_nNodeId,msg->d_cNodePrivateId,msg->s_nDrebSerial,msg->data.head.stDest.nSerial);
}


void CSendThread::SendSingleBcMsg(S_CGATE_SMSG *msg)
{
	int nRet;
	PSOCKET_POOL_DATA info = m_pAioWork->GetPoolData(msg->index);
	if (info == NULL)
	{
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"����index[%d]�Ƿ�",msg->index);
		return ;
	}
	if (info->s_hSocket == INVALID_SOCKET)
	{
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"����index[%d]�ѹر�",msg->index);
		return ;
	}

	if (info->s_cCheckFlag != CHECKFLAG_NORMAL) //δ��֤ͨ�����ݲ�����
	{
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"����index[%d] δ��¼ %d",msg->index,info->s_cCheckFlag );
		return ;
	}
	
	m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"��ʼ���͹㲥�������� index[%d] ��ʶ[%d %d %d %d] cMsgType[%s] cCmd[%s] cRaFlag[%d] cNextFlag[%d] Node[%d %d] svr[%d %d] ������[%d] next[%d %d] czip[%d] nLen[%d]",\
		msg->index,msg->d_nNodeId,msg->d_cNodePrivateId,msg->s_nDrebSerial,msg->data.head.stDest.nSerial,GetMsgType(msg->data.head.stComm.cMsgType).c_str(),\
		GetDrebCmdType(msg->data.head.stComm.cCmd).c_str(),msg->data.head.stComm.cRaflag,\
		msg->data.head.stComm.cNextFlag,msg->data.head.stDest.nNodeId,msg->data.head.stDest.cNodePrivateId,\
		msg->data.head.stDest.nSvrMainId,msg->data.head.stDest.cSvrPrivateId,msg->data.head.stDest.nServiceNo,\
		msg->data.head.stNext.n_nNextNo,msg->data.head.stNext.n_nNextOffset,msg->data.head.stComm.cZip,msg->data.head.nLen);
	

	SOCKET_SEND_DATA dd;
	dd.s_nIndex = info->s_nIndex;
	dd.s_nSendLen = 0;
	dd.s_nRet = 0;
	dd.s_nSendNum = 0;
	dd.s_nTimestamp = info->s_nTimestamp;
	dd.s_nTotalLen = CGATEHEADLEN+msg->data.head.nLen;
	
	dd.s_nHook = msg->data.head.stDest.nSerial;

	m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"�㲥���ͷ��ͳ���[%d] ���ݳ���[%d] ����ͷ����[%d] cZip[%d] index[%d] ��ʶ[%d]",\
		dd.s_nTotalLen,msg->data.head.nLen,CGATEHEADLEN,msg->data.head.stComm.cZip,info->s_nIndex,msg->data.head.stDest.nSerial);
	
	//�ֽ���ת��
	CGATE_HEAD cgatehead;
	memcpy(&(cgatehead),&(msg->data.head),CGATEHEADLEN);

	Endian2CommCrc(cgatehead);
	
	memcpy(dd.s_sSendBuffer,&(cgatehead),CGATEHEADLEN);
	if (dd.s_nTotalLen > DREBBUFSIZE)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"�������ݳ���[%d]���� ",dd.s_nTotalLen);
		return;
	}
	else
	{
		memcpy(dd.s_sSendBuffer+CGATEHEADLEN,msg->data.buffer,msg->data.head.nLen);
	}
	if (info->s_hSocket == INVALID_SOCKET)
	{
		m_pLog->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"���͹㲥ʧ�� ����index[%d]�ѹر� ��ʶ[%d]",msg->index,msg->data.head.stDest.nSerial);
		return ;
	}
	//��������
	dd.s_nTxCode = msg->txcode;
	dd.s_serial  = msg->s_nDrebSerial;
	dd.s_nTimestamp = msg->timestamp;
	dd.s_nSendLen = 0;
	if (!m_pAioWork->Send2Client(dd))
	{
		m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"���͹㲥ʧ�� ��ʶ[%d %d %d %d] ",msg->d_nNodeId,msg->d_cNodePrivateId,msg->s_nDrebSerial,msg->data.head.stDest.nSerial);
		return ;
	}
	m_nSendCustNum++;
	m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���͹㲥��� ��ʶ[%d %d %d %d] ",msg->d_nNodeId,msg->d_cNodePrivateId,msg->s_nDrebSerial,msg->data.head.stDest.nSerial);
}
void CSendThread::SendBCMsg(S_CGATE_SMSG *msg)
{
	msg->timestamp = 0;
	S_CGATE_SMSG senddata;
	bzero(&senddata,sizeof(S_CGATE_SMSG));
	int total;
	char tmpchar[100];
	
	m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"��ʼ���͹㲥���� ��ʶ[%d %d %d %d] cMsgType[%s] cCmd[%s] cRaFlag[%d] cNextFlag[%d] Node[%d %d] svr[%d %d] ������[%d] next[%d %d]",\
		msg->d_nNodeId,msg->d_cNodePrivateId,msg->s_nDrebSerial,msg->data.head.stDest.nSerial,GetMsgType(msg->data.head.stComm.cMsgType).c_str(),\
		GetDrebCmdType(msg->data.head.stComm.cCmd).c_str(),msg->data.head.stComm.cRaflag,\
		msg->data.head.stComm.cNextFlag,msg->data.head.stDest.nNodeId,msg->data.head.stDest.cNodePrivateId,\
		msg->data.head.stDest.nSvrMainId,msg->data.head.stDest.cSvrPrivateId,msg->data.head.stDest.nServiceNo,\
		msg->data.head.stNext.n_nNextNo,msg->data.head.stNext.n_nNextOffset);
	int poolsize = m_pAioWork->GetConnectPoolInfo(total);
	PSOCKET_POOL_DATA info  = NULL;
	for (int i=1 ; i<poolsize ; i++) //0�����ص�����
	{
		info =  m_pAioWork->GetPoolData(i);
		if (info == NULL)
		{
			continue;
		}
		if (info->s_hSocket == INVALID_SOCKET)
		{
//			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"����index[%d]�ѹر�",msg->index);
			continue ;
		}
		if (info->s_cCheckFlag != CHECKFLAG_NORMAL) //δ��֤ͨ�����ݲ�����
		{
//			m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"����index[%d] δ��¼ %d",i,info->s_cCheckFlag );
			continue;
		}
		if (m_pRes->g_nQuoSubScribe == 1) //֧�����鶩��
		{
			if (msg->nVerietyId != 0) //�к�Լid����ʾ�����飬�����ǹ�����ģ�ֱ�ӷ�
			{
                if (NULL == info->ptr)
                {
                    continue;
                }
                CSubScribeInfo* psub = (CSubScribeInfo*)info->ptr;
                if (!psub->IsQuoSubscribe(msg->nVerietyId))
                {
                    m_pLog->LogMp(LOG_DEBUG + 1,__FILE__,__LINE__,"����index[%d] δ�������� %d",i,msg->nVerietyId);
                    continue;
                }
			}
		}
		memcpy(&senddata,msg,sizeof(S_CGATE_SMSG));	
		senddata.index = i;
		senddata.timestamp = 0;
		SendSingleBcMsg(&senddata);

	}
	m_nSendQuoteNum++;
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���͹㲥��[%d],�ɹ������ͻ��㲥��[%d]",m_nSendQuoteNum,m_nSendCustNum);
	m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���͹㲥��� ��ʶ[%d %d %d] ",msg->d_nNodeId,msg->d_cNodePrivateId,msg->s_nDrebSerial);
}

std::string CSendThread::GetMsgType(int type)
{
	// #define  MSG_GATEPING      1          //�����ص�PING��Ϣ
	// #define  MSG_GATEREQ       2          //�������ص���������
	// #define  MSG_GATELOGIN     3          //���ص�¼
	// #define  MSG_PUSH          5          //api�յ���������������������(���顢����)��˽����(�ر���)
	// #define  MSG_GATEPKISTEP1  6          //����PKI��֤��һ�� ���ط���ǩ����
	// #define  MSG_GATEPKISTEP2  7          //����PKI��֤�ڶ��� API��������ǩ����֤�鹫Կ��ǩ���ַ���
	// #define  MSG_GATEPKISTEP3  8          //����PKI��֤������ ���ط���APIǩ����֤�鹫Կ���Ự��Կ
	switch (type)
	{
	case MSG_GATEPING:
		return "MSG_GATEPING";
	case MSG_GATEREQ:
		return "MSG_GATEREQ";
	case MSG_GATELOGIN:
		return "MSG_GATELOGIN";
	case MSG_PUSH:
		return "MSG_PUSH";
	case MSG_GATEPKISTEP1:
		return "MSG_GATEPKISTEP1";
	case MSG_GATEPKISTEP2:
		return "MSG_GATEPKISTEP2";
	case MSG_GATEPKISTEP3:
		return "MSG_GATEPKISTEP3";
	default:
		return "";
	}
}
std::string CSendThread::GetDrebCmdType(int cmdtype)
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
void CSendThread::Endian2CommCrc(CGATE_HEAD &head)
{
	head.stComm.nCrc = 0;
	m_pEndian.Endian2Comm((unsigned char *)&(head.nLen),sizeof(head.nLen));
	m_pEndian.Endian2Comm((unsigned char *)&(head.nRetCode),sizeof(head.nRetCode));
	m_pEndian.Endian2Comm((unsigned char *)&(head.stDest.nNodeId),sizeof(head.stDest.nNodeId));
	m_pEndian.Endian2Comm((unsigned char *)&(head.stDest.nSerial),sizeof(head.stDest.nSerial));
	m_pEndian.Endian2Comm((unsigned char *)&(head.stDest.nServiceNo),sizeof(head.stDest.nServiceNo));
	m_pEndian.Endian2Comm((unsigned char *)&(head.stDest.nSvrMainId),sizeof(head.stDest.nSvrMainId));
	m_pEndian.Endian2Comm((unsigned char *)&(head.stNext.n_nNextNo),sizeof(head.stNext.n_nNextNo));
	m_pEndian.Endian2Comm((unsigned char *)&(head.stNext.n_nNextOffset),sizeof(head.stNext.n_nNextOffset));
	head.stComm.nCrc = m_pEndian.CRC(&head,sizeof(CGATE_HEAD));
//	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"CRCֵ[%d]",head.stComm.nCrc);
	m_pEndian.Endian2Comm((unsigned char *)&(head.stComm.nCrc),sizeof(head.stComm.nCrc));

}

bool CSendThread::Encrypt(S_CGATE_SMSG *msg)
{
	PSOCKET_POOL_DATA info = m_pAioWork->GetPoolData(msg->index);
	if (info == NULL)
	{
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"����index[%d]�Ƿ�",msg->index);
		return false;
	}
	//��Բ�ͬ����Ϣ���м���ѹ��
	switch (msg->data.head.stComm.cMsgType)
	{
		case MSG_GATEPING: //���ݲ��ü���ѹ��
			break;
		case MSG_GATEPKISTEP1://ѹ��+�̶���Կ
			if (!ZipBuf(info,&(msg->data),14))
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"ѹ��+�̶���Կ���ܳ���");
				return false;
			}
			break;
		case MSG_GATEPKISTEP2://ѹ��+�̶���Կ
			if (!ZipBuf(info,&(msg->data),14))
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"ѹ��+�̶���Կ���ܳ���");
				return false;
			}
			break;
		case MSG_GATEPKISTEP3://ѹ��+���ܹ�Կ
			if (!ZipBuf(info,&(msg->data),15))
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"ѹ��+���ܹ�Կ���ܳ���");
				return false;
			}
			break;
		case MSG_GATESMS:
		case MSG_GATELOGIN:   //ѹ��+�Ự��ԿDES
		case MSG_GATEREQ:
		case MSG_PUSH:
			if (!ZipBuf(info,&(msg->data),4))
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"ѹ��+�Ự��ԿDES���ܳ���");
				return false;
			}
			break;
		default:
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"��Ϣ����[%s]���ü���ѹ������",GetMsgType(msg->data.head.stComm.cMsgType).c_str());
			break;
	}
	return true;
}
bool CSendThread::ZipBuf(PSOCKET_POOL_DATA info,PCGATE_COMMSTRU  data,int nZipFlag)
{
	//0���� 1des������Կ����16 2ѹ�� 3DES���� 4DES���ܲ�ѹ��
	if (data->head.stComm.cZip != 0 && data->head.stComm.cZip != FLAG_ZIP)
	{
		return false;
	}
	if (data->head.nLen == 0)
	{
		return true;
	}
	char zipDataBuf[CLIENTDATASIZE+1];
	bzero(zipDataBuf,sizeof(zipDataBuf));
	unsigned int ziplen = CLIENTDATASIZE;
	int deslen=0;
	unsigned short shortlen;

	m_pGateLog->LogCgate(LOG_DEBUG,data,info->s_nIndex,info->s_sSessionKey);
	
	
	switch (nZipFlag)
	{
		case FLAG_ZIPSESSIONKEY:
			if (m_pRes->g_nZipFlag == 1)
			{
				if (data->head.stComm.cZip == 0)
				{
					ziplen = CLIENTDATASIZE;
					memcpy(zipDataBuf,data->buffer,data->head.nLen);
					if (!CBF_Tools::Compress((unsigned char *)data->buffer,ziplen,(unsigned char *)(zipDataBuf),data->head.nLen))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"����ѹ��ʧ��");
						return false;
					}
					data->head.nLen = ziplen;
					data->head.stComm.cZip = FLAG_ZIP;
					m_pGateLog->LogCgate(LOG_DEBUG,data,info->s_nIndex,info->s_sSessionKey);
					
				}
				if (data->head.stComm.cZip != FLAG_ZIP)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"���ݲ���ѹ������");
					return false;
				}
				deslen = data->head.nLen;
			
				if (m_pRes->g_nEncryptFlag == 1)
				{
					if (m_pDesSend.EncryptString(data->buffer,deslen,info->s_sSessionKey)<=0)
					{
						m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
						return false;
					}
					if (atoi(info->s_sSessionKey) == 0)
					{
						m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ��� ��Կ[%s]",info->s_sSessionKey);
					}
					data->head.nLen = deslen;
					data->head.stComm.cZip = FLAG_ZIPSESSIONKEY;

				}
			}
			else
			{
				deslen = data->head.nLen;
				if (m_pRes->g_nEncryptFlag == 1)
				{
					if (m_pDesSend.EncryptString(data->buffer,deslen,info->s_sSessionKey)<=0)
					{
						m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
						return false;
					}
					if (atoi(info->s_sSessionKey) == 0)
					{
						m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ��� ��Կ[%s]",info->s_sSessionKey);
					}
					data->head.nLen = deslen;
					if (data->head.stComm.cZip == 0)
					{
						data->head.stComm.cZip = FLAG_SESSIONKEY;
					}
					else
					{
						data->head.stComm.cZip = FLAG_ZIPSESSIONKEY;
					}
				}
			}
			
			break;
		case FLAG_ZIPFIXEDKEY:
			if (m_pRes->g_nZipFlag == 1)
			{
				if (data->head.stComm.cZip == 0)
				{
					ziplen = CLIENTDATASIZE;
					memcpy(zipDataBuf,data->buffer,data->head.nLen);
					if (!CBF_Tools::Compress((unsigned char *)data->buffer,ziplen,(unsigned char *)(zipDataBuf),data->head.nLen))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"����ѹ��ʧ��");
						return false;
					}
					data->head.nLen = ziplen;
					data->head.stComm.cZip = FLAG_ZIP;
					m_pGateLog->LogCgate(LOG_DEBUG,data,info->s_nIndex,info->s_sSessionKey);
				}
			//�����ܣ����̼߳��ܿ��ܻᵼ�����ݽⲻ��

// 				if (data->head.stComm.cZip != FLAG_ZIP)
// 				{
// 					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"���ݲ���ѹ������");
// 					return false;
// 				}
// 				deslen = data->head.nLen;
// 				if (m_pDesSend.EncryptString(data->buffer,deslen,DESENCRYPTKEY3)<=0)
// 				{
// 					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
// 					return false;
// 				}
// 				data->head.nLen = deslen;
//				data->head.stComm.cZip = FLAG_ZIPFIXEDKEY;
	
			}
			else
			{

			}
			break;
		case FLAG_ZIPCERTKEY:
			if (m_pRes->g_nZipFlag == 1)
			{
				if (data->head.stComm.cZip == 0)
				{
					memcpy(zipDataBuf,data->buffer,data->head.nLen);
					ziplen = CLIENTDATASIZE;
					if (!CBF_Tools::Compress((unsigned char *)data->buffer,ziplen,(unsigned char *)(zipDataBuf),data->head.nLen))
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"����ѹ��ʧ��");
						return false;
					}
					data->head.nLen = ziplen;
					data->head.stComm.cZip = FLAG_ZIP;
					m_pGateLog->LogCgate(LOG_DEBUG,data,info->s_nIndex,info->s_sSessionKey);
				}
				if (data->head.stComm.cZip != FLAG_ZIP)
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"���ݲ���ѹ������");
					return false;
				}
				if (m_pRes->g_nUseCert == 1)
				{
					shortlen = data->head.nLen;
					m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"ip[%s] index[%d] ���ܹ�Կ����[%d]",inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,info->s_nEncryptPubKeyLen);
					if (!m_pRes->g_pCertClass->Encrypt(info->s_sEncryptPubKey,info->s_nEncryptPubKeyLen,data->buffer,shortlen))
					{
						m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ��� %s",m_pRes->g_pCertClass->GetErrMsg().c_str());
						return false;
					}
					data->buffer[shortlen]=0;
					data->head.nLen = shortlen;
					data->head.stComm.cZip =FLAG_ZIPCERTKEY;

				}
			}
			else
			{
				if (m_pRes->g_nUseCert == 1)
				{
					shortlen = data->head.nLen;
					m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"ip[%s] index[%d] ���ܹ�Կ����[%d]",inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,info->s_nEncryptPubKeyLen);
					if (!m_pRes->g_pCertClass->Encrypt(info->s_sEncryptPubKey,info->s_nEncryptPubKeyLen,data->buffer,shortlen))
					{
						m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ��� %s",m_pRes->g_pCertClass->GetErrMsg().c_str());
						return false;
					}
					data->buffer[shortlen]=0;
					data->head.nLen = shortlen;
					if (data->head.stComm.cZip == 0)
					{
						data->head.stComm.cZip =FLAG_CERTKEY;
					}
					else
					{
						data->head.stComm.cZip =FLAG_ZIPCERTKEY;
					}
				}
			}
			break;
		default:
			return true;
	}
	m_pGateLog->LogCgate(LOG_DEBUG,data,info->s_nIndex,info->s_sSessionKey);
	return true;
}

bool CSendThread::UnzipDrebBuf(S_CGATE_SMSG &data)
{
    //0���� 1des������Կ����16 2ѹ�� 3DES���� 4DES���ܲ�ѹ��
    if (data.data.head.nLen == 0)
    {
        return true;
    }
    if (data.data.head.stComm.cZip > 0 && data.data.head.stComm.cZip <= 4)
    {
        char zipDataBuf[BPUDATASIZE];
        bzero(zipDataBuf, sizeof(zipDataBuf));
        int ddlen;
        switch (data.data.head.stComm.cZip)
        {
        case 1:
            ddlen = data.data.head.nLen;
            if (m_pDes.UncryptString(data.data.buffer, ddlen, DESENCRYPTKEY) < 1)
            {
                m_pLog->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "���ܳ���");
                return false;
            }
			data.data.head.nLen = ddlen;
			data.data.head.stComm.cZip = 0;
            break;
        case 2:
            break;
        case 3:
            ddlen = data.data.head.nLen;
            if (m_pDes.UncryptString(data.data.buffer, ddlen, DESENCRYPTKEY3) < 1)
            {
                m_pLog->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "���ܳ���");
                return false;
            }
			data.data.head.nLen = ddlen;
			data.data.head.stComm.cZip = 0;
            break;
        case 4:
            ddlen = data.data.head.nLen;
            if (m_pDes.UncryptString(data.data.buffer, ddlen, DESENCRYPTKEY3) < 1)
            {
                m_pLog->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "���ܳ���");
                return false;
            }
			data.data.head.nLen = ddlen;
			data.data.head.stComm.cZip = 2;
            break;
        default:
            return false;
        }
    }

    return true;
}

void CSendThread::SendSubscribeMsg(S_CGATE_SMSG* msg)
{
    msg->timestamp = 0;
    S_CGATE_SMSG senddata;
    bzero(&senddata, sizeof(S_CGATE_SMSG));
    int total;
    char tmpchar[100];

    m_pLog->LogMp(LOG_DEBUG + 1, __FILE__, __LINE__, "��ʼ���ͻر���Ĺ㲥���� ��ʶ[%d %d %d %d] cMsgType[%s] cCmd[%s] cRaFlag[%d] cNextFlag[%d] Node[%d %d] svr[%d %d] ������[%d] next[%d %d]", \
        msg->d_nNodeId, msg->d_cNodePrivateId, msg->s_nDrebSerial, msg->data.head.stDest.nSerial, GetMsgType(msg->data.head.stComm.cMsgType).c_str(), \
        GetDrebCmdType(msg->data.head.stComm.cCmd).c_str(), msg->data.head.stComm.cRaflag, \
        msg->data.head.stComm.cNextFlag, msg->data.head.stDest.nNodeId, msg->data.head.stDest.cNodePrivateId, \
        msg->data.head.stDest.nSvrMainId, msg->data.head.stDest.cSvrPrivateId, msg->data.head.stDest.nServiceNo, \
        msg->data.head.stNext.n_nNextNo, msg->data.head.stNext.n_nNextOffset);
    int poolsize = m_pAioWork->GetConnectPoolInfo(total);
    PSOCKET_POOL_DATA info = NULL;
    for (int i = 1; i < poolsize; i++) //0�����ص�����
    {
        info = m_pAioWork->GetPoolData(i);
        if (info == NULL)
        {
            continue;
        }
        if (info->s_hSocket == INVALID_SOCKET)
        {
            continue;
        }
        if (info->s_cCheckFlag != CHECKFLAG_NORMAL) //δ��֤ͨ�����ݲ�����
        {
            continue;
        }

		//Ҫ��֤key�Ƿ���
        if (NULL == info->ptr)
        {
            continue;
        }
        CSubScribeInfo* psub = (CSubScribeInfo*)info->ptr;
        if (!psub->IsSubscribe(msg->data.head.stDest.nSerial))
        {
            m_pLog->LogMp(LOG_DEBUG + 1, __FILE__, __LINE__, "����index[%d] δ���Ļر� key[%d]", i, msg->data.head.stDest.nSerial);
            continue;
        }

        memcpy(&senddata, msg, sizeof(S_CGATE_SMSG));
        senddata.index = i;
        senddata.timestamp = 0;
        SendSingleBcMsg(&senddata);

    }
    m_nSendQuoteNum++;
    m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "���ͻر���㲥��[%d],�ɹ������ͻ��㲥��[%d]", m_nSendQuoteNum, m_nSendCustNum);
    m_pLog->LogMp(LOG_DEBUG + 1, __FILE__, __LINE__, "���ͻر���㲥��� ��ʶ[%d %d %d] ", msg->d_nNodeId, msg->d_cNodePrivateId, msg->s_nDrebSerial);
}

