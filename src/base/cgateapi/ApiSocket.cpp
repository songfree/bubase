// ApiSocket.cpp: implementation of the CApiSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "ApiSocket.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define  DESENCRYPTKEY3   "wms13711songfree"


#define  FLAG_FIXEDKEY            94  //�̶�key����
#define  FLAG_SESSIONKEY          84   //�Ựkey����
#define  FLAG_CERTKEY             95  //֤��key����

#define  FLAG_ZIPFIXEDKEY         14  //ѹ�����̶�key����
#define  FLAG_ZIPSESSIONKEY       4   //ѹ�����Ựkey����
#define  FLAG_ZIPCERTKEY          15  //ѹ����֤��key����
#define  FLAG_ZIP                 2   //ѹ��

CApiSocket::CApiSocket()
{

	bzero(m_sRcvBuffer,sizeof(m_sRcvBuffer));  //�������ݻ���
	bzero(m_sSendBuffer,sizeof(m_sSendBuffer));
	m_nRcvBufferLen=0;//�������ݻ����С
	
	m_pLog  = NULL;
	
	bzero(m_sSessionKey,sizeof(m_sSessionKey));
	bzero(&m_head,sizeof(CGATE_HEAD));

	bzero(m_sGatePubSignKey,sizeof(m_sGatePubSignKey));
	bzero(m_sGatePubRsaKey,sizeof(m_sGatePubRsaKey));
	m_nGatePubSignKeyLen = 0;
	m_nGatePubRsaKeyLen = 0;
	m_nIndex = 0;

	m_nSocketStatus =0;
}

CApiSocket::~CApiSocket()
{
	m_pLog  = NULL;
	m_pRes = NULL;
	
	m_nSocketStatus = 0;
}

int CApiSocket::Send2Gate(PCGATE_COMMSTRU data)
{
	int nRet;
	S_CGATEAPI_SMSG  smsg;
	bzero(&smsg,sizeof(S_CGATEAPI_SMSG));
	smsg.offset = 0;
	memcpy(&(smsg.message),data,sizeof(CGATE_COMMSTRU));
	
// 	if (m_pRes->IsControlBytes())
// 	{
// 		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�������� ÿ���ֽ���");
// //		m_pSendQueue.PutSendMsg(smsg);
// 		return 0;
// 	}
// 	if (m_pRes->IsControlTx())
// 	{
// 		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�������� ÿ�뽻����");
// //		m_pSendQueue.PutSendMsg(smsg);
// 		return 0;
// 	}
	if (data->head.stComm.cMsgType == MSG_GATEPING)
	{
		m_pLog->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"��ʼ�������� index[%d] ��ʶ[%d] cMsgType[%s] cCmd[%s] cRaFlag[%d] cNextFlag[%d] Node[%d %d] svr[%d %d] ������[%d] next[%d %d]",\
			m_nIndex,data->head.stDest.nSerial,GetMsgType(data->head.stComm.cMsgType).c_str(),\
			GetDrebCmdType(data->head.stComm.cCmd).c_str(),data->head.stComm.cRaflag,\
			data->head.stComm.cNextFlag,data->head.stDest.nNodeId,data->head.stDest.cNodePrivateId,\
			data->head.stDest.nSvrMainId,data->head.stDest.cSvrPrivateId,data->head.stDest.nServiceNo,\
		data->head.stNext.n_nNextNo,data->head.stNext.n_nNextOffset);
	}
	else
	{
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"��ʼ�������� index[%d] ��ʶ[%d] cMsgType[%s] cCmd[%s] cRaFlag[%d] cNextFlag[%d] Node[%d %d] svr[%d %d] ������[%d] next[%d %d]",\
			m_nIndex,data->head.stDest.nSerial,GetMsgType(data->head.stComm.cMsgType).c_str(),\
			GetDrebCmdType(data->head.stComm.cCmd).c_str(),data->head.stComm.cRaflag,\
			data->head.stComm.cNextFlag,data->head.stDest.nNodeId,data->head.stDest.cNodePrivateId,\
			data->head.stDest.nSvrMainId,data->head.stDest.cSvrPrivateId,data->head.stDest.nServiceNo,\
			data->head.stNext.n_nNextNo,data->head.stNext.n_nNextOffset);
	}
	smsg.message.head.stComm.cZip = 0;
	//��Բ�ͬ����Ϣ���м���ѹ��
	switch (smsg.message.head.stComm.cMsgType)
	{
		case MSG_GATEPING: //���ݲ��ü���ѹ��
			break;
		case MSG_GATEPKISTEP1://���ݲ��ü���ѹ��
			break;
		case MSG_GATEPKISTEP2://ѹ��+�̶���Կ3DES
			ZipBuf(&(smsg.message),14);
			break;
		case MSG_GATEPKISTEP3://ѹ��+API��Կ����
			break;
		case MSG_GATESMS:
		case MSG_GATELOGIN:   //ѹ��+�Ự��ԿDES
		case MSG_GATEREQ:
			ZipBuf(&(smsg.message),4);
			break;
		default:
			break;
	}
	smsg.offset=0;
	smsg.serial = smsg.message.head.stDest.nSerial;
	smsg.totallen = smsg.message.head.nLen+CGATEHEADLEN;
	smsg.sendlen = 0;
	//�ֽ���ת��
	Endian2CommCrc(smsg.message.head);
	//��������
	CBF_PMutex pp(&m_mutex);
	if (m_pSocketTcp.GetSocket() == INVALID_SOCKET)
	{
		m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"��������ʱ�����ѱ��ر� index[%d] ��ʶ[%d] cMsgType[%s] cCmd[%s] cRaFlag[%d] cNextFlag[%d] Node[%d %d] svr[%d %d] ������[%d] next[%d %d]",\
			m_nIndex,data->head.stDest.nSerial,GetMsgType(data->head.stComm.cMsgType).c_str(),\
			GetDrebCmdType(data->head.stComm.cCmd).c_str(),data->head.stComm.cRaflag,\
			data->head.stComm.cNextFlag,data->head.stDest.nNodeId,data->head.stDest.cNodePrivateId,\
			data->head.stDest.nSvrMainId,data->head.stDest.cSvrPrivateId,data->head.stDest.nServiceNo,\
			data->head.stNext.n_nNextNo,data->head.stNext.n_nNextOffset);
		return -1;
	}
	nRet = m_pSocketTcp.Send( (char *)&(smsg.message),smsg.totallen);
	if (nRet != smsg.totallen) //û�з������
	{
		if (nRet >0)
		{
			//���ͳ���
			smsg.offset+=nRet;
			smsg.sendlen +=nRet;
			m_nSendTime = time(NULL);
		}
		if (m_pSocketTcp.CheckWriteEvent(1000) == 1)
		{
			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"socket��д�ٴη���,Ҫ�������ݳ���[%d]",smsg.totallen - smsg.sendlen);
			nRet = m_pSocketTcp.Send( ((char *)&(smsg.message))+smsg.sendlen,smsg.totallen - smsg.sendlen);
			if (nRet == (smsg.totallen - smsg.sendlen) )
			{
				m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"�ٴη��ͳɹ�,�������ݳ���[%d]",nRet);
				m_pRes->g_pApiFlowInfo.ReportStat(smsg.totallen);
				return 2;
			}
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��д����������ʧ�ܣ�����ֵ[%d] index[%d] ��ʶ[%d] �ر����� [%d %s]",\
				nRet,m_nIndex,smsg.serial,m_pSocketTcp.GetErrorNo(),m_pSocketTcp.GetErrorMsg().c_str());
			OnClose(__FILE__,__LINE__,"��д����������ʧ��,�ر�����",LOG_ERROR_GENERAL);
			return 1;
		}
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��1���������Բ���д��index[%d] ��ʶ[%d] �ر����� [%d %s]",\
			m_nIndex,smsg.serial,m_pSocketTcp.GetErrorNo(),m_pSocketTcp.GetErrorMsg().c_str());
		OnClose(__FILE__,__LINE__,"��1���������Բ���д,�ر�����",LOG_ERROR_GENERAL);
		return 1;
	}
	else //���ͳɹ�
	{
		smsg.offset+=nRet;
		smsg.sendlen=0;
		m_pRes->g_pApiFlowInfo.ReportStat(smsg.totallen);
		if (smsg.serial !=0 )
		{
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�������� index[%d] ��ʶ[%d]�ɹ� ����[%d]",\
				m_nIndex,smsg.serial,nRet);
		}
		else
		{
			m_pLog->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"�������� index[%d] ��ʶ[%d]�ɹ� ����[%d]",\
				m_nIndex,smsg.serial,nRet);
		}
		
		m_nSendTime = time(NULL);
		return 2;
	}
	return 2;
}
// 
// int CApiSocket::OnSend()
// {
// 	if (!m_pSendQueue.NeedSend())
// 	{
// 		return 0;
// 	}
// 	if (m_pRes->IsControlBytes())
// 	{
// 		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�������� ÿ���ֽ���");
// 		return 0;
// 	}
// 	if (m_pRes->IsControlTx())
// 	{
// 		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�������� ÿ�뽻����");
// 		return 0;
// 	}
// 	CGATE_SQUEUE::iterator prq,prn;
// 	unsigned int atimeout;
// 	int nRet;
// 
// 	for (prn=m_pSendQueue.m_qSendData.datas.begin(); prn!=m_pSendQueue.m_qSendData.datas.end() ;)
// 	{
// 		prq = prn;
// 		prn++;
// 		if (prq->sendlen  ==  prq->totallen ) //ȫ�����ݣ�Ҫ�жϳ�ʱ
// 		{
// 			if (time(NULL) - prq->rtime> m_pRes->g_nTimeOutSend)
// 			{
// 				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�����ڶ��г�ʱ %d ��",m_pRes->g_nTimeOutSend);
// 				m_pLog->LogBin(LOG_ERROR,__FILE__,__LINE__,"�����ڶ��г�ʱ",prq->message.buffer,prq->totallen);
// 				m_pSendQueue.DeleteSend(prq);
// 				continue;
// 			}
// 			if (prq->sendnum > m_pRes->g_nDataSendNum)
// 			{
// 				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�����ʹ������� %d",m_pRes->g_nDataSendNum);
// 				m_pLog->LogBin(LOG_ERROR,__FILE__,__LINE__,"�����ʹ�������",prq->message.buffer,prq->totallen);
// 				m_pSendQueue.DeleteSend(prq);
// 				continue;
// 			}
// 		}
// 		Send(prq);
// 	}
// 
// }

void CApiSocket::OnClose(const char *filename, int fileline, const char *msg,int loglevel)
{
	m_pSocketTcp.Close();
	m_pLog->LogMp(loglevel,filename,fileline,"�ر����� %s",msg);
	bzero(m_sRcvBuffer,sizeof(m_sRcvBuffer));  //�������ݻ���
	bzero(m_sSendBuffer,sizeof(m_sSendBuffer));
	m_nRcvBufferLen=0;//�������ݻ����С
	bzero(m_sSessionKey,sizeof(m_sSessionKey));
	bzero(&m_head,sizeof(CGATE_HEAD));
	
	bzero(m_sGatePubSignKey,sizeof(m_sGatePubSignKey));
	bzero(m_sGatePubRsaKey,sizeof(m_sGatePubRsaKey));
	m_nGatePubSignKeyLen = 0;
	m_nGatePubRsaKeyLen = 0;
	m_nIndex = 0;

	m_nSocketStatus = 0;//δ����
	m_pAnsQueue.Clear();
	m_pReqQueue.Clear();
}


void CApiSocket::SetPara(CGlobalResource *res)
{
	m_pRes = res;
	m_pLog = &(res->g_pLog);
	m_pGateLog = &(m_pRes->g_pCgateLog);
	if (m_pRes->g_nUseCert == 1)
	{
		m_pRes->g_pCertClass->GetPubicKey(m_sLocalPubSignKey,m_nLocalPubSignKeyLen,m_sLocalPubRsaKey,m_nLocalPubRsaKeyLen);
	}
}

bool CApiSocket::UnzipBuf(CGATE_COMMSTRU &data)
{
	if (data.head.nLen == 0)
	{
		return true;
	}
	char zipDataBuf[CLIENTDATASIZE];
	bzero(zipDataBuf,sizeof(zipDataBuf));
	unsigned int ziplen = CLIENTDATASIZE;
	int deslen;
	unsigned short rsalen;
	m_pGateLog->LogCgate(LOG_DEBUG+1,&data,m_nIndex,m_sSessionKey);
	switch (data.head.stComm.cZip)
	{
		case FLAG_ZIP:
			memcpy(zipDataBuf,data.buffer,data.head.nLen);
			ziplen = CLIENTDATASIZE;
			
			if (!Uncompress((unsigned char *)(data.buffer),ziplen,(unsigned char *)zipDataBuf,data.head.nLen))
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"��ѹ������ index[%d] ��ʶ[%d]",\
					m_nIndex,data.head.stDest.nSerial);
				m_pLog->LogBin(LOG_ERROR_FAULT,__FILE__,__LINE__,"��ѹ������",zipDataBuf,data.head.nLen);
				memcpy(data.buffer,zipDataBuf,data.head.nLen);
				m_pGateLog->LogCgate(LOG_ERROR_FAULT,&data,m_nIndex,"999999999");
				return false;
			}
			data.head.nLen = ziplen;
			data.head.stComm.cZip =0;
			break;
		case FLAG_ZIPSESSIONKEY:
			deslen = data.head.nLen;
			if (m_pDes.UncryptString(data.buffer,deslen,m_sSessionKey)<1)
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ�����Կ[%d]",m_sSessionKey);
				return false;
			}
			data.head.nLen = deslen ;
			data.head.stComm.cZip =FLAG_ZIP;
			memcpy(zipDataBuf,data.buffer,data.head.nLen);
//			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���ܺ����ݳ���[%d]",data.head.nLen);
			ziplen = CLIENTDATASIZE;
			m_pGateLog->LogCgate(LOG_DEBUG,&data,m_nIndex,m_sSessionKey);
			if (!Uncompress((unsigned char *)(data.buffer),ziplen,(unsigned char *)zipDataBuf,data.head.nLen))
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"��ѹ������ index[%d] ��ʶ[%d]",\
					m_nIndex,data.head.stDest.nSerial);
				m_pLog->LogBin(LOG_ERROR_FAULT,__FILE__,__LINE__,"��ѹ������",zipDataBuf,data.head.nLen);
				memcpy(data.buffer,zipDataBuf,data.head.nLen);
				m_pGateLog->LogCgate(LOG_ERROR_FAULT,&data,m_nIndex,"999999999");
				return false;
			}
			data.head.nLen = ziplen;
			data.head.stComm.cZip =0;
			break;
		case FLAG_SESSIONKEY:
			deslen = data.head.nLen;
			if (m_pDes.UncryptString(data.buffer,deslen,m_sSessionKey)<1)
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ�����Կ[%d]",m_sSessionKey);
				return false;
			}
			data.head.nLen = deslen ;
			data.head.stComm.cZip =0;
			break;
		case FLAG_ZIPFIXEDKEY:
			deslen = data.head.nLen;
			if (m_pDes.UncryptString(data.buffer,deslen,DESENCRYPTKEY3)<1)
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"�̶���ԿDESENCRYPTKEY3���ܳ���");
				return false;
			}

			data.head.nLen = deslen;
			data.head.stComm.cZip =FLAG_ZIP;
			memcpy(zipDataBuf,data.buffer,data.head.nLen);
			ziplen = CLIENTDATASIZE;
			

			m_pGateLog->LogCgate(LOG_DEBUG,&data,m_nIndex,m_sSessionKey);
			if (!Uncompress((unsigned char *)(data.buffer),ziplen,(unsigned char *)zipDataBuf,data.head.nLen))
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"��ѹ������ index[%d] ��ʶ[%d]",\
					m_nIndex,data.head.stDest.nSerial);
				m_pLog->LogBin(LOG_ERROR_FAULT,__FILE__,__LINE__,"��ѹ������",zipDataBuf,data.head.nLen);
				memcpy(data.buffer,zipDataBuf,data.head.nLen);
				m_pGateLog->LogCgate(LOG_ERROR_FAULT,&data,m_nIndex,"999999999");
				return false;
			}
			data.head.nLen = ziplen;
			data.head.stComm.cZip =0;
			break;
		case FLAG_FIXEDKEY:
			deslen = data.head.nLen;
			if (m_pDes.UncryptString(data.buffer,deslen,DESENCRYPTKEY3)<1)
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"�̶���ԿDESENCRYPTKEY3���ܳ���");
				return false;
			}
			
			data.head.nLen = deslen;
			data.head.stComm.cZip =0;
			break;
		case FLAG_ZIPCERTKEY:
			if (m_pRes->g_nUseCert == 1)
			{
				rsalen = data.head.nLen;
				if (!m_pRes->g_pCertClass->UnEncrypt(data.buffer,rsalen))
				{
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
					return false;
				}
				data.buffer[rsalen]=0;
				data.head.nLen = rsalen;
				data.head.stComm.cZip =FLAG_ZIP;
			}
			
			memcpy(zipDataBuf,data.buffer,data.head.nLen);
			ziplen = CLIENTDATASIZE;
			m_pGateLog->LogCgate(LOG_DEBUG,&data,m_nIndex,m_sSessionKey);
			if (!Uncompress((unsigned char *)(data.buffer),ziplen,(unsigned char *)zipDataBuf,data.head.nLen))
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"��ѹ������ index[%d] ��ʶ[%d]",\
					m_nIndex,data.head.stDest.nSerial);
				m_pLog->LogBin(LOG_ERROR_FAULT,__FILE__,__LINE__,"��ѹ������",zipDataBuf,data.head.nLen);
				return false;
			}
			data.head.nLen = ziplen;
			data.head.stComm.cZip =0;
			break;
		case FLAG_CERTKEY:
			if (m_pRes->g_nUseCert == 1)
			{
				rsalen = data.head.nLen;
				if (!m_pRes->g_pCertClass->UnEncrypt(data.buffer,rsalen))
				{
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
					return false;
				}
				data.buffer[rsalen]=0;
				data.head.nLen = rsalen;
				data.head.stComm.cZip = 0;
			}
			break;
		default:
			return true;
	}
	return true;
}
bool CApiSocket::ZipBuf(PCGATE_COMMSTRU  data,int nZipFlag)
{
	//0���� 1des������Կ����16 2ѹ�� 3DES���� 4DES���ܲ�ѹ��
	if (data->head.stComm.cZip != 0)
	{
		return true;
	}
	if (data->head.nLen == 0)
	{
		return true;
	}
	char zipDataBuf[CLIENTDATASIZE];
	bzero(zipDataBuf,sizeof(zipDataBuf));
	unsigned int ziplen = CLIENTDATASIZE;
	int deslen;
	unsigned short rsalen;
	m_pGateLog->LogCgate(LOG_DEBUG+1,data,m_nIndex,m_sSessionKey);
	switch (nZipFlag)
	{
		case FLAG_ZIPSESSIONKEY:
			if (m_pRes->g_nZipFlag == 1)
			{
				ziplen = CLIENTDATASIZE;
				memcpy(zipDataBuf,data->buffer,data->head.nLen);
				if (!Compress((unsigned char *)data->buffer,ziplen,(unsigned char *)(zipDataBuf),data->head.nLen))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"����ѹ��ʧ��");
					return false;
				}
				data->head.nLen = ziplen;
				//дdata��־
				deslen = ziplen;
				data->head.stComm.cZip =FLAG_ZIP;
				m_pGateLog->LogCgate(LOG_DEBUG,data,m_nIndex,m_sSessionKey);
				if (m_pDes.EncryptString(data->buffer,deslen,m_sSessionKey)<1)
				{
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
					return false;
				}
				
				data->head.nLen = deslen;
				data->head.stComm.cZip =FLAG_ZIPSESSIONKEY;
			}
			else
			{
				deslen = data->head.nLen;
				if (m_pDes.EncryptString(data->buffer,deslen,m_sSessionKey)<1)
				{
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
					return false;
				}
				
				data->head.nLen = deslen;
				data->head.stComm.cZip =FLAG_SESSIONKEY;
			}
			break;
		case FLAG_ZIPFIXEDKEY:
			if (m_pRes->g_nZipFlag == 1)
			{
				memcpy(zipDataBuf,data->buffer,data->head.nLen);
				ziplen = CLIENTDATASIZE;
				if (!Compress((unsigned char *)data->buffer,ziplen,(unsigned char *)(zipDataBuf),data->head.nLen))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"����ѹ��ʧ��");
					return false;
				}
				data->head.nLen = ziplen;
				deslen = ziplen;
				data->head.stComm.cZip =FLAG_ZIP;
				m_pGateLog->LogCgate(LOG_DEBUG,data,m_nIndex,m_sSessionKey);
				if (m_pDes.EncryptString(data->buffer,deslen,DESENCRYPTKEY3)<1)
				{
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
					return false;
				}

				data->head.nLen = deslen;
				data->head.stComm.cZip =FLAG_ZIPFIXEDKEY;
			}
			else
			{
				deslen = data->head.nLen;
				if (m_pDes.EncryptString(data->buffer,deslen,DESENCRYPTKEY3)<1)
				{
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
					return false;
				}
				
				data->head.nLen = deslen;
				data->head.stComm.cZip =FLAG_FIXEDKEY;
			}
			break;
		case FLAG_ZIPCERTKEY:
			if (m_pRes->g_nZipFlag == 1)
			{
				memcpy(zipDataBuf,data->buffer,data->head.nLen);
				ziplen = CLIENTDATASIZE;
				if (!Compress((unsigned char *)data->buffer,ziplen,(unsigned char *)(zipDataBuf),data->head.nLen))
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"����ѹ��ʧ��");
					return false;
				}
				data->head.nLen = ziplen;
				data->head.stComm.cZip =FLAG_ZIP;
				m_pGateLog->LogCgate(LOG_DEBUG,data,m_nIndex,m_sSessionKey);
				if (m_pRes->g_nUseCert == 1)
				{
					rsalen = data->head.nLen;
					if (!m_pRes->g_pCertClass->Encrypt(m_sLocalPubRsaKey,m_nLocalPubRsaKeyLen,data->buffer,rsalen ))
					{
						m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
						return false;
					}
					data->buffer[rsalen]=0;
					data->head.nLen = rsalen;
					data->head.stComm.cZip =FLAG_ZIPCERTKEY;
				}
			}
			else
			{
				if (m_pRes->g_nUseCert == 1)
				{
					rsalen = data->head.nLen;
					if (!m_pRes->g_pCertClass->Encrypt(m_sLocalPubRsaKey,m_nLocalPubRsaKeyLen,data->buffer,rsalen ))
					{
						m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
						return false;
					}
					data->buffer[rsalen]=0;
					data->head.nLen = rsalen;
					data->head.stComm.cZip = FLAG_CERTKEY;
				}
			}
			
			break;
		default:
			return true;
	}
	m_pGateLog->LogCgate(LOG_DEBUG,data,m_nIndex,m_sSessionKey);
	return true;
}

std::string CApiSocket::GetMsgType(int type)
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
		case MSG_GATESUBSCRIBE:
			return "MSG_GATESUBSCRIBE";
		default:
			return "";
	}
}
std::string CApiSocket::GetDrebCmdType(int cmdtype)
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

void CApiSocket::Endian2CommCrc(CGATE_HEAD &head)
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
	if (m_pRes->g_nCrcFlag == 1)
	{
		head.stComm.nCrc = m_pEndian.CRC(&head,sizeof(CGATE_HEAD));
		//	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"CRCֵ[%d]",head.stComm.nCrc );
		m_pEndian.Endian2Comm((unsigned char *)&(head.stComm.nCrc),sizeof(head.stComm.nCrc));
	}
	
}
bool CApiSocket::Endian2HostCrc(CGATE_HEAD &head)
{
	m_pEndian.Endian2LocalHost((unsigned char *)&(head.stComm.nCrc),sizeof(head.stComm.nCrc));
	if (m_pRes->g_nCrcFlag == 1)
	{ 
		unsigned short crc1 = head.stComm.nCrc;
	//	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"CRCֵ[%d]",head.stComm.nCrc );
		head.stComm.nCrc = 0;
		unsigned short crc2 = m_pEndian.CRC(&head,sizeof(CGATE_HEAD));
		if (crc1 != crc2)
		{
			return false;
		}
	}
	m_pEndian.Endian2LocalHost((unsigned char *)&(head.nLen),sizeof(head.nLen));
	m_pEndian.Endian2LocalHost((unsigned char *)&(head.nRetCode),sizeof(head.nRetCode));
	m_pEndian.Endian2LocalHost((unsigned char *)&(head.stDest.nNodeId),sizeof(head.stDest.nNodeId));
	m_pEndian.Endian2LocalHost((unsigned char *)&(head.stDest.nSerial),sizeof(head.stDest.nSerial));
	m_pEndian.Endian2LocalHost((unsigned char *)&(head.stDest.nServiceNo),sizeof(head.stDest.nServiceNo));
	m_pEndian.Endian2LocalHost((unsigned char *)&(head.stDest.nSvrMainId),sizeof(head.stDest.nSvrMainId));
	m_pEndian.Endian2LocalHost((unsigned char *)&(head.stNext.n_nNextNo),sizeof(head.stNext.n_nNextNo));
	m_pEndian.Endian2LocalHost((unsigned char *)&(head.stNext.n_nNextOffset),sizeof(head.stNext.n_nNextOffset));
	return true;
}

int CApiSocket::OnRcv()
{
	int ret;
	if (m_nRcvBufferLen >=0 && m_nRcvBufferLen<DREBBUFSIZE)
	{
		//��ͷδ������
		ret = m_pSocketTcp.Receive(m_sRcvBuffer+m_nRcvBufferLen,DREBBUFSIZE-m_nRcvBufferLen);
		if (ret <= 0)
		{
			return ret;
		}
		m_nRcvTime =time(NULL);
		m_nPingTime = m_nRcvTime;
		m_nRcvBufferLen+=ret;
		//��¼ͳ����Ϣ
		return ret;
	}
	return 0;
}
int CApiSocket::GetRecvData(CGATE_COMMSTRU *msg)
{
	//�յ������ݴ��ڰ�ͷ
	if (m_nRcvBufferLen >= CGATEHEADLEN )
	{
		//�����ֽ�����У��CRC
		memcpy(&m_head,m_sRcvBuffer,CGATEHEADLEN);
		if (!Endian2HostCrc(m_head))
		{
			OnClose(__FILE__,__LINE__,"CRC����",LOG_ERROR);
			return -1;
		}
		if (m_nRcvBufferLen < m_head.nLen + CGATEHEADLEN)
		{
			//˵������û�н�������
//			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"����δ�������� index[%d] ��ʶ[%d] ���ݳ���[%d] nLen[%d]  msgtype[%d] raflag[%d]",\
//				m_nIndex,msg->head.stDest.nSerial,m_nRcvBufferLen,m_head.nLen ,m_head.stComm.cMsgType,m_head.stComm.cRaflag);
			return 0;
		}
		memcpy(&(msg->head),&m_head,CGATEHEADLEN);
		if (m_head.nLen>0)
		{
			memcpy(&(msg->buffer),m_sRcvBuffer+CGATEHEADLEN,m_head.nLen);
		}
		
		//���������־
		if (msg->head.stComm.cMsgType != MSG_GATEPING)
		{
			if (msg->head.stComm.cMsgType == MSG_PUSH)
			{
				m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���յ������� �������� index[%d] cMsgType[%s] cCmd[%s] cRaFlag[%d] cNextFlag[%d] Node[%d %d] svr[%d %d] ������[%d] next[%d %d] cZip[%d] nLen[%d]",\
					m_nIndex,GetMsgType(msg->head.stComm.cMsgType).c_str(),\
					GetDrebCmdType(msg->head.stComm.cCmd).c_str(),msg->head.stComm.cRaflag,\
					msg->head.stComm.cNextFlag,msg->head.stDest.nNodeId,msg->head.stDest.cNodePrivateId,\
					msg->head.stDest.nSvrMainId,msg->head.stDest.cSvrPrivateId,msg->head.stDest.nServiceNo,\
					msg->head.stNext.n_nNextNo,msg->head.stNext.n_nNextOffset,msg->head.stComm.cZip,msg->head.nLen);
			}
			else
			{
				m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���յ������� index[%d] ��ʶ[%d] cMsgType[%s] cCmd[%s] cRaFlag[%d] cNextFlag[%d] Node[%d %d] svr[%d %d] ������[%d] next[%d %d] cZip[%d] nLen[%d]",\
					m_nIndex,msg->head.stDest.nSerial,GetMsgType(msg->head.stComm.cMsgType).c_str(),\
					GetDrebCmdType(msg->head.stComm.cCmd).c_str(),msg->head.stComm.cRaflag,\
					msg->head.stComm.cNextFlag,msg->head.stDest.nNodeId,msg->head.stDest.cNodePrivateId,\
					msg->head.stDest.nSvrMainId,msg->head.stDest.cSvrPrivateId,msg->head.stDest.nServiceNo,\
					msg->head.stNext.n_nNextNo,msg->head.stNext.n_nNextOffset,msg->head.stComm.cZip,msg->head.nLen);
			}
			
		}
		else
		{
			m_pLog->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"���յ������� index[%d] ��ʶ[%d] cMsgType[%s] cCmd[%s] cRaFlag[%d] cNextFlag[%d] Node[%d %d] svr[%d %d] ������[%d] next[%d %d] cZip[%d] nLen[%d]",\
				m_nIndex,msg->head.stDest.nSerial,GetMsgType(msg->head.stComm.cMsgType).c_str(),\
				GetDrebCmdType(msg->head.stComm.cCmd).c_str(),msg->head.stComm.cRaflag,\
				msg->head.stComm.cNextFlag,msg->head.stDest.nNodeId,msg->head.stDest.cNodePrivateId,\
				msg->head.stDest.nSvrMainId,msg->head.stDest.cSvrPrivateId,msg->head.stDest.nServiceNo,\
				msg->head.stNext.n_nNextNo,msg->head.stNext.n_nNextOffset,msg->head.stComm.cZip,msg->head.nLen);
		}
		

		//�ն��ˣ���ԭ�����¸�m_sRcvBuffer
		for (int i=0; i<m_nRcvBufferLen-CGATEHEADLEN-m_head.nLen; i++)
		{
			m_sRcvBuffer[i]=m_sRcvBuffer[i+CGATEHEADLEN+m_head.nLen];
		}
		m_nRcvBufferLen=m_nRcvBufferLen - CGATEHEADLEN-m_head.nLen;
		//��ѹ������
		if (!UnzipBuf(*msg))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ݽ�ѹ������ʧ�� index[%d] ��ʶ[%d]",\
				m_nIndex,msg->head.stDest.nSerial);
			OnClose(__FILE__,__LINE__,"���ݽ�ѹ������ʧ��",LOG_ERROR);
			return -1;
// 			msg->head.nRetCode = 18;
// 			return m_head.nLen+CGATEHEADLEN;
		}
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���� nLen[%d] cZip[%d]",msg->head.nLen,msg->head.stComm.cZip);
		m_pLog->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"���յ�������",msg->buffer,msg->head.nLen);
		return m_head.nLen+CGATEHEADLEN;
	}
	return 0;
}

bool CApiSocket::ConnectGate()
{
	m_nIndex = 0;
	bool bisConnected=false;
	for (unsigned int i=0 ; i<m_pRes->g_lGateList.size() ; i++)
	{
		m_pSocketTcp.Close();
		m_pSocketTcp.Create(AF_INET,false);
		if (m_pSocketTcp.ConnectServer(m_pRes->g_lGateList[i].sIp.c_str(),m_pRes->g_lGateList[i].nPort,m_pRes->g_nConnGateTimeOut*1000))
		{
			bisConnected = true;
			m_pSocketTcp.SetBlockFlag(false);
			m_pRes->g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"��������[%s %d]�ɹ� %d",m_pRes->g_lGateList[i].sIp.c_str(),m_pRes->g_lGateList[i].nPort,m_pRes->g_nConnGateTimeOut*1000);
			break;
		}
		m_pRes->g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"��������[%s %d]ʧ�� %d",m_pRes->g_lGateList[i].sIp.c_str(),m_pRes->g_lGateList[i].nPort,m_pRes->g_nConnGateTimeOut*1000);
	}
	if (!bisConnected)
	{
		OnClose(__FILE__,__LINE__,"�������е�����IP��ʧ��",LOG_ERROR);
		return false;
	}
	//PKI��֤
	if (m_pRes->g_nUseCert == 1)
	{
		if (!Pki())
		{
			OnClose(__FILE__,__LINE__,"PKI��ȫ��֤ʧ��",LOG_ERROR);
			return false;
		}
		m_pRes->g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,"PKI��ȫ��֤�ɹ�");
	}
	else
	{
		int nRet;
		CGATE_COMMSTRU data;
		bzero(&data,sizeof(CGATE_COMMSTRU));
		while (true)
		{
			nRet = m_pSocketTcp.CheckReadEvent(m_pRes->g_nConnGateTimeOut*1000);
			if (nRet <= 0)
			{
				OnClose(__FILE__,__LINE__,"�������� ��ָ��ʱ��δ�յ�������Ӧ����",LOG_ERROR);
				return false;
			}
			nRet = OnRcv();
			if (nRet < 0 )
			{
				OnClose(__FILE__,__LINE__,"�������� ���������ѹرգ���������ʧ��",LOG_WARNNING);
				return false;
			}
			
			nRet = GetRecvData(&data);
			if (nRet == 0)
			{
				//������
				continue;
			}
			if (nRet <0)
			{
				OnClose(__FILE__,__LINE__,"�������� GetRecvData ʧ��",LOG_ERROR);
				return false;
			}
			break;
		}
		if (data.head.stComm.cMsgType != MSG_GATEPKISTEP3)
		{
			OnClose(__FILE__,__LINE__,"�������� ���ط��������ݷ�MSG_GATEPKISTEP3",LOG_ERROR);
			return false;
		}
		PPKI_GATESTEP23 s2 = (PPKI_GATESTEP23)data.buffer;
		m_pEndian.Endian2LocalHost((unsigned char *)&(s2->nSignBufferLen),sizeof(s2->nSignBufferLen));
		m_pEndian.Endian2LocalHost((unsigned char *)&(s2->nSignStrLen),sizeof(s2->nSignStrLen));
		m_pEndian.Endian2LocalHost((unsigned char *)&(s2->nRsaPubKeyLen),sizeof(s2->nRsaPubKeyLen));
		m_pEndian.Endian2LocalHost((unsigned char *)&(s2->nSignPubKeyLen),sizeof(s2->nSignPubKeyLen));
		bzero(m_sSessionKey,sizeof(m_sSessionKey));
		//�Ự����Ϊ16λ����
		memcpy(m_sSessionKey,s2->sSignStr,16);
		m_nIndex = atoi(s2->sSignStr+17);

		m_pRes->g_pLog.LogBin(LOG_PROMPT,__FILE__,__LINE__,"���ط������������",s2->sSignStr,sizeof(s2->sSignStr));
		if (m_nIndex <1)
		{
			OnClose(__FILE__,__LINE__,"�������� ���ط�����������ŷǷ�",LOG_ERROR);
			return false;
		}
		m_pRes->g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,"��֤����֤ͨ�� index[%d]",m_nIndex);
	}
	m_pRes->g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,"���ӳɹ� index[%d]�����Է���¼������",m_nIndex);
	m_nSocketStatus = 1;
	return true;
}

SOCKET_HANDLE CApiSocket::GetSocket()
{
	return m_pSocketTcp.GetSocket();
}

bool CApiSocket::Pki()
{
	if (m_pRes->g_pCertClass == NULL)
	{
		m_pRes->g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"֤�鶯̬��ʵ����ΪNULL");
		return false;
	}
	int nRet;
	CGATE_COMMSTRU data;
	bzero(&data,sizeof(CGATE_COMMSTRU));
	while (true)
	{
		nRet = m_pSocketTcp.CheckReadEvent(m_pRes->g_nConnGateTimeOut*1000);
		if (nRet <= 0)
		{
			OnClose(__FILE__,__LINE__,"��ȫ��֤ ��ָ��ʱ��δ�յ�������Ӧ����",LOG_ERROR);
			return false;
		}
		nRet = OnRcv();
		if (nRet < 0 )
		{
			OnClose(__FILE__,__LINE__,"��ȫ��֤ ���������ѹرգ���������ʧ��",LOG_ERROR);
			return false;
		}
		m_pRes->g_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"��ȫ��֤ ���յ����� %d",nRet);
		nRet = GetRecvData(&data);
		if (nRet == 0)
		{
			//����û����ȫ��������
			continue;
		}
		if (nRet <0 )
		{
			OnClose(__FILE__,__LINE__,"��ȫ��֤ GetRecvData ʧ��",LOG_ERROR);
			return false;
		}
		break;
	}
	
	if (data.head.stComm.cMsgType != MSG_GATEPKISTEP1)
	{
		OnClose(__FILE__,__LINE__,"��ȫ��֤ ���ط��������ݷ�MSG_GATEPKISTEP1",LOG_ERROR);
		return false;
	}
	//�õ����ط��ص��ַ�����Ҫ�Դ��ַ�������ǩ��������Կ��������
	//ǩ��
	char  signstr[4048];
	unsigned short   signstrlen= 16;//�̶�Ϊ16λ����
	bzero(signstr,sizeof(signstr));
	memcpy(signstr,data.buffer,16);
	//�ӵ�17���ַ�֮��ʹ���������������������
	m_nIndex = atoi(data.buffer+17);

	//������ݣ�׼�����ظ�����ǩ������Կ
	bzero(&data,sizeof(CGATE_COMMSTRU));
	data.head.nRetCode = 0;
	data.head.stComm.cMsgType = MSG_GATEPKISTEP2;

	PPKI_GATESTEP23 pkidata = (PPKI_GATESTEP23)data.buffer;
	pkidata->nSignBufferLen = sizeof(pkidata->sSignBuffer);

	unsigned short clen;
	clen = pkidata->nSignBufferLen;
	//�����ط������ַ�������ǩ��
	if (!m_pRes->g_pCertClass->Sign(signstr,signstrlen,pkidata->sSignBuffer,clen))
	{
		OnClose(__FILE__,__LINE__,"��ȫ��֤ ������ǩ��ʧ��",LOG_ERROR);
		m_pLog->LogBin(LOG_ERROR,__FILE__,__LINE__,"Ҫǩ��������",signstr,signstrlen);
		return false;
	}
	pkidata->nSignBufferLen = clen;
	//���ɸ�����˵�ǩ���ַ��� 16λ����
	pkidata->nSignStrLen = sizeof(pkidata->sSignStr);
	memcpy(pkidata->sSignStr,GenSignString().c_str(),16);
	pkidata->nSignStrLen = 16;
	
	//���淢�����ص��ַ�������ǩ����֤
	bzero(signstr,sizeof(signstr));
	memcpy(signstr,pkidata->sSignStr,16);
	signstrlen = 16;

	//ȡ��Կ
	pkidata->nSignPubKeyLen = sizeof(pkidata->sSignPubKey);//ǩ����Կ
	pkidata->nRsaPubKeyLen = sizeof(pkidata->sRsaPubKey);//���ܹ�Կ
	unsigned short dlen = pkidata->nSignPubKeyLen;
	clen = pkidata->nRsaPubKeyLen;
	if (!m_pRes->g_pCertClass->GetPubicKey(pkidata->sSignPubKey,dlen,pkidata->sRsaPubKey,clen))
	{
		OnClose(__FILE__,__LINE__,"��ȫ��֤ ȡ���ع�Կʧ��",LOG_ERROR);
		return false;
	}
	pkidata->nSignPubKeyLen = dlen;
	pkidata->nRsaPubKeyLen = clen;
	m_pRes->g_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ͻ��˹�Կ���� %d %d ",pkidata->nSignPubKeyLen,pkidata->nRsaPubKeyLen );
	m_pRes->g_pLog.LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"�ͻ��˹�Կ",pkidata->sSignPubKey,pkidata->nSignPubKeyLen);
	m_pRes->g_pLog.LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"ǩ���ַ���",signstr,strlen(signstr));
	m_pRes->g_pLog.LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"�ͻ���ǩ��",pkidata->sSignBuffer,pkidata->nSignBufferLen);

	m_pEndian.Endian2Comm((unsigned char *)&(pkidata->nSignBufferLen),sizeof(pkidata->nSignBufferLen));//ǩ��
	m_pEndian.Endian2Comm((unsigned char *)&(pkidata->nSignStrLen),sizeof(pkidata->nSignStrLen));//Ҫǩ�����ַ���
	m_pEndian.Endian2Comm((unsigned char *)&(pkidata->nRsaPubKeyLen),sizeof(pkidata->nRsaPubKeyLen));//���ܹ�Կ
	m_pEndian.Endian2Comm((unsigned char *)&(pkidata->nSignPubKeyLen),sizeof(pkidata->nSignPubKeyLen));//ǩ����Կ

	data.head.nLen = sizeof(PKI_GATESTEP23);
	m_pRes->g_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"���͸�����ǩ��������֤�鹫Կ MSG_GATEPKISTEP2  nLen:%d",data.head.nLen);
	nRet = Send2Gate(&data);
	if (nRet != 2)
	{
		OnClose(__FILE__,__LINE__,"��ȫ��֤ ����ǩ�������ع�Կʧ��",LOG_ERROR);
		return false;
	}
	m_pRes->g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,"׼���������ص�Ӧ�� MSG_GATEPKISTEP3");

	while (true)
	{
		nRet = m_pSocketTcp.CheckReadEvent(m_pRes->g_nConnGateTimeOut*1000);
		if (nRet <= 0)
		{
			OnClose(__FILE__,__LINE__,"��ȫ��֤ ��ָ��ʱ��δ�յ�������Ӧ����",LOG_ERROR);
			return false;
		}
		m_pRes->g_pLog.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"CheckReadEvent %d ",nRet );
		nRet = OnRcv();
		if (nRet < 0 )
		{
			OnClose(__FILE__,__LINE__,"��ȫ��֤ ���������ѹرգ���������ʧ��",LOG_ERROR);
			return false;
		}
		m_pRes->g_pLog.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���յ������ݳ��� %d ",nRet );
		bzero(&data,sizeof(CGATE_COMMSTRU));
		nRet = GetRecvData(&data);
		if (nRet == 0)
		{
			//����û����ȫ��������
			continue;
		}
		if (nRet <0 )
		{
			OnClose(__FILE__,__LINE__,"��ȫ��֤ �������ݲ����������عر�����",LOG_ERROR);
			return false;
		}
		break;
	}
	
	m_pRes->g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,"��ȫ��֤ ���յ������ݳ���:%d msgtype:%d nlen:%d retcode:%d",\
		nRet,data.head.stComm.cMsgType,data.head.nLen,data.head.nRetCode);
	
	if (data.head.nRetCode != 0)
	{
		m_pRes->g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"PKI��֤ʧ�ܣ����ط��� %d ",data.head.nRetCode);
		OnClose(__FILE__,__LINE__,"��ȫ��֤ ��̨����PKI��֤ʧ��",LOG_ERROR);
		return false;
	}
	
	if (data.head.stComm.cMsgType != MSG_GATEPKISTEP3)
	{
		OnClose(__FILE__,__LINE__,"��ȫ��֤ ���ط��������ݷ�MSG_GATEPKISTEP3",LOG_ERROR);
		return false;
	}
	if (data.head.nLen != sizeof(PKI_GATESTEP23))
	{
		OnClose(__FILE__,__LINE__,"��ȫ��֤ ���ط��������ݷ�MSG_GATEPKISTEP3",LOG_ERROR);
		return false;
	}
	//�������ع�Կ���Ự��Կ
	pkidata = (PPKI_GATESTEP23)data.buffer;
	m_pEndian.Endian2LocalHost((unsigned char *)&(pkidata->nSignBufferLen),sizeof(pkidata->nSignBufferLen));
	m_pEndian.Endian2LocalHost((unsigned char *)&(pkidata->nSignStrLen),sizeof(pkidata->nSignStrLen));
	m_pEndian.Endian2LocalHost((unsigned char *)&(pkidata->nRsaPubKeyLen),sizeof(pkidata->nRsaPubKeyLen));
	m_pEndian.Endian2LocalHost((unsigned char *)&(pkidata->nSignPubKeyLen),sizeof(pkidata->nSignPubKeyLen));

	bzero(m_sGatePubRsaKey,sizeof(m_sGatePubRsaKey));
	bzero(m_sGatePubSignKey,sizeof(m_sGatePubSignKey));
	memcpy(m_sGatePubRsaKey,pkidata->sRsaPubKey,pkidata->nRsaPubKeyLen);
	m_nGatePubRsaKeyLen = pkidata->nRsaPubKeyLen;
	memcpy(m_sGatePubSignKey,pkidata->sSignPubKey,pkidata->nSignPubKeyLen);
	m_nGatePubSignKeyLen = pkidata->nSignPubKeyLen;
	
	
// 	//ǩ���ַ���������ǻỰ��Կ��ʹ�ñ��ع�Կ����
// 	if (!m_pRes->g_pCertClass->UnEncrypt(pkidata->sSignStr,pkidata->nSignStrLen))
// 	{
// 		m_pSocketTcp.Close();
// 		m_pRes->g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"���ܻỰ��Կʧ��");
// 		return false;
// 	}
// 	if (pkidata->nSignStrLen >= sizeof(m_sSessionKey))
// 	{
// 		m_pSocketTcp.Close();
// 		m_pRes->g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"�Ự��Կ����[%d]����",pkidata->nSignStrLen);
// 		return false;
// 	}

	bzero(m_sSessionKey,sizeof(m_sSessionKey));
	memcpy(m_sSessionKey,pkidata->sSignStr,pkidata->nSignStrLen);
//	m_pRes->g_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"�Ự��Կ[%s]",m_sSessionKey);

	//�����ص�ǩ��������֤
	if (!m_pRes->g_pCertClass->CheckSign(m_sGatePubSignKey,m_nGatePubSignKeyLen,signstr,signstrlen,pkidata->sSignBuffer,pkidata->nSignBufferLen))
	{
		OnClose(__FILE__,__LINE__,"��ȫ��֤ ��֤����ǩ��ʧ��",LOG_ERROR);
		return false;
	}
	m_nIndex = atoi(pkidata->sIndex);
	return true;
}
int CApiSocket::GetRcvDataLen()
{
	return m_nRcvBufferLen;
}

std::string CApiSocket::GenSignString()
{
	char tmpchar[49];
	bzero(tmpchar,sizeof(tmpchar));
	srand((unsigned int)CBF_Date_Time::GetTickCount());
	sprintf(tmpchar,"%05d%04u%07u",rand()%100000,(int)CBF_Date_Time::GetTickCount()%10000,(int)CBF_Date_Time::GetTickUS()%10000000);
	return tmpchar;
}
bool CApiSocket::Compress(unsigned char *destbuf, unsigned int &destlen, unsigned char *srbuf, int srbuflen)
{
	CBF_PMutex pp(&m_mutexCompress);
	return CBF_Tools::Compress(destbuf,destlen,srbuf,srbuflen);
}

bool CApiSocket::Uncompress(unsigned char *destbuf, unsigned int &destlen, unsigned char *srbuf, int srbuflen)
{
	CBF_PMutex pp(&m_mutexCompress);
	return CBF_Tools::Uncompress(destbuf,destlen,srbuf,srbuflen);
}

