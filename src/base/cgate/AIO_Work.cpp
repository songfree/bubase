// AIO_Work.cpp: implementation of the CAIO_Work class.
//
//////////////////////////////////////////////////////////////////////

#include "AIO_Work.h"
#include "SubScribeInfo.h"

#ifdef LINUX
#include<setjmp.h>
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


#define  DESENCRYPTKEY3   "wms13711songfree"



CAIO_Work::CAIO_Work()
{
	m_pDataPool = NULL;
	m_pRes = NULL;
	bzero(&m_pRcvData,sizeof(CGATE_COMMSTRU));
	bzero(m_sDestBuffer,sizeof(m_sDestBuffer));
	bzero(m_sSrcBuffer,sizeof(m_sSrcBuffer));
	m_pLog = NULL;
	m_pMemPool = NULL;
}

CAIO_Work::~CAIO_Work()
{

}

void CAIO_Work::SetGlobalVar(CGateResource *res,CPoolData *pool,CBF_BufferPool *mempool,CSendData *senddata)
{
	m_pDataPool = pool;
	m_pRes = res;
	m_pMemPool = mempool;
	m_pLog = m_pRes->g_pLog;
	m_pSendData = senddata;

	m_pGateLog = &(m_pRes->g_pGateLog);

}


// ������: Connect
// ���  : ������ 2012-5-21 11:55:51
// ����  : void 
// ����  : PSOCKET_POOL_DATA *info
// ����  : AIO֪ͨ�����ӶϿ���Ҫ������
void CAIO_Work::OnConnect(PSOCKET_POOL_DATA info)
{
	//û���������ӵ�
}


// ������: OnSendBack
// ���  : ������ 2012-5-17 16:38:08
// ����  : virtual void 
// ����  : PSOCKET_SEND_DATA data   
// ����  : AIO֪ͨ��д�����ݷ��أ��Է��ؽ�����д���
void CAIO_Work::OnSendBack(PSOCKET_SEND_DATA data,bool issuccess)
{
	if (issuccess)
	{
		m_pRes->g_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"������� %d",data->s_nIndex);
//		m_pRes->g_pLog.LogBin(LOG_DEBUG,__FILE__,__LINE__,"�������",data->s_sSendBuffer,data->s_nTotalLen);
	}
	else
	{
		m_pRes->g_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"����ʧ�� %d",data->s_nIndex);
//		m_pRes->g_pLog.LogBin(LOG_DEBUG,__FILE__,__LINE__,"�������",data->s_sSendBuffer,data->s_nTotalLen);
	}
}
void CAIO_Work::OnSendBack(SOCKSENDQUEUE::iterator  data,bool issuccess)
{
	if (issuccess)
	{
		m_pRes->g_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"������� %d",data->s_nIndex);
//		m_pRes->g_pLog.LogBin(LOG_DEBUG,__FILE__,__LINE__,"�������",data->s_sSendBuffer,data->s_nTotalLen);
	}
	else
	{
		m_pRes->g_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"����ʧ�� %d",data->s_nIndex);
//		m_pRes->g_pLog.LogBin(LOG_DEBUG,__FILE__,__LINE__,"�������",data->s_sSendBuffer,data->s_nTotalLen);
	}
}

// ������: OnPing
// ���  : ������ 2012-5-21 11:54:23
// ����  : virtual void 
// ����  : AIO֪ͨ��Ҫ����PING��
void CAIO_Work::OnPing(PSOCKET_POOL_DATA info)
{
	if (time(NULL) - info->s_nPingTime < m_pRes->g_nHeartRun)
	{
		return;
	}
	CGATE_COMMSTRU data;
	bzero(&data,sizeof(CGATE_COMMSTRU));
	data.head.nLen = 0;
	data.head.nRetCode = 0;
	data.head.stComm.cMsgType = MSG_GATEPING;
	data.head.stComm.cCmd = 0;
	data.head.stComm.cNextFlag = 0;
	data.head.stComm.cRaflag = 0;
	data.head.stComm.cZip =0;
	data.head.stComm.nCrc =0;
	info->s_nPingTime = time(NULL);
	
	if (info->s_cCheckFlag != CHECKFLAG_NORMAL) //δ��֤ͨ�����ݲ�����
	{
		//ȷ��������
		SendData(&data,info->s_nIndex);
	}
	else
	{
		SendData(&data,info->s_nIndex,false,0);
	}

}

// ������: OnRecvBack
// ���  : ������ 2012-5-17 16:36:58
// ����  : virtual void 
// ����  : PSOCKET_POOL_DATA info
// ����  : AIO֪ͨ�������յ����ݣ������ݽ��д���
void CAIO_Work::OnRecvBack(PSOCKET_POOL_DATA info)
{
	//�յ������ˣ�ȡ���ݣ����봦�����
	//�յ������ݴ��ڰ�ͷ
	info->s_nRealBufferLen += info->s_nIoBufferLen;
	info->s_nIoBufferLen = 0;
	info->s_pIoBuffer = info->s_sRealBuffer+info->s_nRealBufferLen;
	CGATE_COMMSTRU msg;
	while (info->s_nRealBufferLen >= CGATEHEADLEN )
	{
		//�����ֽ�����У��CRC
		memcpy(&(msg.head),info->s_sRealBuffer,CGATEHEADLEN);
		if (!Endian2HostCrc(msg.head))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"CRC����");
			OnClose(info,"����");
			return ;
		}
		if (info->s_nRealBufferLen < msg.head.nLen + CGATEHEADLEN)
		{
			//˵������û�н�������
			return ;
		}
		if (msg.head.nLen >0)
		{
			if (msg.head.nLen > CLIENTDATASIZE)
			{
				OnClose(info,"���ĳ��Ȳ���");
				return;
			}
			memcpy(msg.buffer,info->s_sRealBuffer+CGATEHEADLEN,msg.head.nLen);
		}
		
		//���������־
		if (msg.head.stComm.cMsgType != MSG_GATEPING )
		{
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���յ������� �ͻ���[%s] index[%d] ��ʶ[%d] cMsgType[%s] cCmd[%s] cRaFlag[%d] cNextFlag[%d] Node[%d %d] svr[%d %d] ������[%d] next[%d %d] cZip[%d] nLen[%d]",\
				inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,msg.head.stDest.nSerial,GetMsgType(msg.head.stComm.cMsgType).c_str(),\
				GetDrebCmdType(msg.head.stComm.cCmd).c_str(),msg.head.stComm.cRaflag,\
				msg.head.stComm.cNextFlag,msg.head.stDest.nNodeId,msg.head.stDest.cNodePrivateId,\
				msg.head.stDest.nSvrMainId,msg.head.stDest.cSvrPrivateId,msg.head.stDest.nServiceNo,\
				msg.head.stNext.n_nNextNo,msg.head.stNext.n_nNextOffset,msg.head.stComm.cZip,msg.head.nLen);
		}
		else
		{
			m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���յ���MSG_GATEPING��Ϣ");
		}

		//�ն��ˣ���ԭ�����¸�m_sRcvBuffer
		for (unsigned int i=0; i<info->s_nRealBufferLen-CGATEHEADLEN-msg.head.nLen; i++)
		{
			info->s_sRealBuffer[i]=info->s_sRealBuffer[i+CGATEHEADLEN+msg.head.nLen];
		}
		info->s_nRealBufferLen=info->s_nRealBufferLen - CGATEHEADLEN-msg.head.nLen;

		info->s_pIoBuffer = info->s_sRealBuffer+info->s_nRealBufferLen;

		if (msg.head.stComm.cRaflag == 1) //�ͻ��˵�Ӧ��
		{
			if (msg.head.stComm.cMsgType != MSG_GATEPING)
			{
				m_pLog->LogMp(LOG_PROMPT, __FILE__, __LINE__, "�ͻ���index[%d] cMsgType[%s]Ӧ�� ����", info->s_nIndex, GetMsgType(msg.head.stComm.cMsgType).c_str());
			}
			continue;
		}
		if (msg.head.nLen >0)
		{
			//��ѹ������
			if (!UnzipBuf(info,msg))
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ݽ��ܽ�ѹ��ʧ��");
				msg.head.stComm.cRaflag = 1;
				msg.head.stComm.cZip =0;
				msg.head.nLen = 0;
				msg.head.nRetCode = 18;
				//ȷ��������
				SendData(&msg,info->s_nIndex);
				continue ;
			}
		}
		ProcessRcvMsg(info,msg);
	}
	//����ڴ˺������غ���ȥ������,����������״̬֪ͨ��ܶ�
	
}

// ������: OnConnectBack
// ���  : ������ 2012-5-17 16:35:34
// ����  : virtual void 
// ����  : PSOCKET_POOL_DATA info
// ����  : int status ����״̬  0�ɹ� ����ʧ��
// ����  : AIO֪ͨ�������������ӳɹ����Դ����ӽ��к�������
void CAIO_Work::OnConnectBack(PSOCKET_POOL_DATA info,int status)
{
	//������״̬������������֤��Ϣ�ȵ�
	//����ʲô������������Ϊû�������������ӵ�
}

// ������: OnClose
// ���  : ������ 2012-5-17 16:34:32
// ����  : virtual void 
// ����  : PSOCKET_POOL_DATA info ������Ϣ
// ����  : std::string msg
// ����  : AIO֪ͨ������һ�����ӹرգ�AIO��û�жԴ����ӽ��йرմ���Ҫ��������Դ����ӽ��к�������Ȼ�����Close����֪ͨAIO�رյ�
void CAIO_Work::OnClose(PSOCKET_POOL_DATA info,std::string msg)
{
	//��շ��Ͷ�������AIO������е�
 	
	//������ʵ������
	CSubScribeInfo *pp = (CSubScribeInfo *)info->ptr;
	if (pp != NULL)
	{
		delete pp;
		pp = NULL;
	}
	info->ptr = NULL;
	CloseClient(info);
	m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"�ر����� %s",msg.c_str());
	
}

// ������: OnAccept
// ���  : ������ 2012-5-17 16:33:33
// ����  : virtual void 
// ����  : PSOCKET_POOL_DATA info
// ����  : AIO֪ͨ���յ�һ�����ӣ��Դ������ӽ��д���,AIO��ܲ��������ӳأ���Ӧ�ü��ͨ��������ʾ����AddAio
void CAIO_Work::OnAccept(PSOCKET_POOL_DATA info)
{

	//��������Ƿ�Ϸ�
	CGATE_COMMSTRU data;
	bzero(&data,sizeof(CGATE_COMMSTRU));
	data.head.nRetCode = 0;
	
	data.head.stComm.cCmd = 0;
	data.head.stComm.cNextFlag = 0;
	data.head.stComm.cRaflag = 0;
	data.head.stComm.cZip =0;
	data.head.stComm.nCrc =0;
	PPKI_GATESTEP23 pki = (PPKI_GATESTEP23)(data.buffer);

	
	if (m_pRes->g_nUseCert == 1)
	{
		info->s_cCheckFlag = CHECKFLAG_STEP1;
		data.head.stComm.cMsgType = MSG_GATEPKISTEP1;
		CBF_Tools::StringCopy(info->s_sSignString,48,GenSignString().c_str());
		//ͨ��ǩ���ַ���������������
		sprintf(data.buffer,"%16s %d",info->s_sSignString,info->s_nIndex);
		data.head.nLen = strlen(info->s_sSignString);
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���ظ��ͻ������ǩ���ַ���");
		//ȷ��������
		SendData(&data,info->s_nIndex);
	}
	else
	{
		info->s_cCheckFlag = CHECKFLAG_NORMAL;
		//������ɻỰ��Կ
		data.head.stComm.cMsgType = MSG_GATEPKISTEP3;
		bzero(info->s_sSessionKey,sizeof(info->s_sSessionKey));
		CBF_Tools::StringCopy(info->s_sSessionKey,48,GenSignString().c_str());
		//ͨ��ǩ���ַ���������������
		sprintf(pki->sSignStr,"%16s %d",info->s_sSessionKey,info->s_nIndex);
		pki->nSignStrLen = strlen(pki->sSignStr);

		pki->nSignBufferLen =0;
		pki->nRsaPubKeyLen =0;
		pki->nSignPubKeyLen =0;
		pki->nKeySerialLen = 0;
		m_pEndian.Endian2Comm((unsigned char *)&(pki->nSignBufferLen),sizeof(pki->nSignBufferLen));
		m_pEndian.Endian2Comm((unsigned char *)&(pki->nSignStrLen),sizeof(pki->nSignStrLen));
		m_pEndian.Endian2Comm((unsigned char *)&(pki->nRsaPubKeyLen),sizeof(pki->nRsaPubKeyLen));
		m_pEndian.Endian2Comm((unsigned char *)&(pki->nSignPubKeyLen),sizeof(pki->nSignPubKeyLen));
		m_pEndian.Endian2Comm((unsigned char *)&(pki->nKeySerialLen),sizeof(pki->nKeySerialLen));
		data.head.nLen = sizeof(PKI_GATESTEP23);
// 		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���ظ��ͻ��˻Ự��Կ");
// 		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�Ự��Կ[%s]",info->s_sSessionKey);
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"ip[%s] index[%d] �Ự��Կ[%s]",\
				inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,info->s_sSessionKey);
		//ȷ��������
		SendData(&data,info->s_nIndex);
	}

	//��SOCKET״̬�ȵȣ���ֻ������ֻд���Ƕ�д,��Ҫ��SELECTģ���� 
	info->s_cSocketStatus = SOCKSTATUS_R;
	info->s_cSocketType = SOCK_CLIENT;

	//����һ������ʵ��
	info->ptr = new CSubScribeInfo();
	if (info->ptr == NULL)
	{
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"new CSubScribeInfo����");
	}
	else
	{
		((CSubScribeInfo *)(info->ptr))->m_pLog = m_pLog;
	}
	AddAio(info);
}
void CAIO_Work::Endian2CommCrc(CGATE_HEAD &head)
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
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"CRCֵ[%d]",head.stComm.nCrc);
		m_pEndian.Endian2Comm((unsigned char *)&(head.stComm.nCrc),sizeof(head.stComm.nCrc));
	}
}
bool CAIO_Work::Endian2HostCrc(CGATE_HEAD &head)
{
	m_pEndian.Endian2LocalHost((unsigned char *)&(head.stComm.nCrc),sizeof(head.stComm.nCrc));
	if (m_pRes->g_nCrcFlag == 1)
	{
		unsigned short crc1 = head.stComm.nCrc;
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"CRCֵ[%d]",crc1);
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

bool CAIO_Work::UnzipBuf(PSOCKET_POOL_DATA info,CGATE_COMMSTRU &data)
{
	if (data.head.nLen == 0 || data.head.stComm.cZip == 0)
	{
		return true;
	}
	m_pGateLog->LogCgate(LOG_DEBUG,&data,info->s_nIndex,info->s_sSessionKey);

// #ifdef LINUX
// 	jmp_buf sJmp;
// 	if(!setjmp(sJmp))
// 	{
// 		char* s = (char *)alloca(4096);
// 		memset(s, 0, 4096);
// 		longjmp(sJmp, 1);
// 	}
// #endif
	
	

	unsigned int ziplen = CLIENTDATASIZE;
	int deslen;
	unsigned short shortlen;
	int desnum=0;
	bool issuccess=false;
	CBF_PMutex pp(&m_pDesMutex);
	switch (data.head.stComm.cZip)
	{
		case FLAG_ZIP:
			break;
		case FLAG_ZIPSESSIONKEY:
			while(desnum <3)
			{
				memcpy(m_sSrcBuffer,data.buffer,data.head.nLen);
				deslen = data.head.nLen;
				if (m_pDes.UncryptString(m_sSrcBuffer,deslen,info->s_sSessionKey) <= 0)
				{
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ��� [%s] %d",info->s_sSessionKey,deslen);
					return false;
				}
				
				if (m_pRes->g_nUnCommpress == 1)
				{
					if (!CBF_Tools::Uncompress((unsigned char *)m_sDestBuffer,ziplen,(unsigned char *)m_sSrcBuffer,deslen))
					{
						m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"��ѹ������ �����ǽ��ܵ������Ѿ����� ����[%d] [%s]",\
							desnum,info->s_sSessionKey);
						desnum++;
						continue;//�ٴγ��Խ���
					}
					issuccess = true;
					data.head.nLen = ziplen;
					data.head.stComm.cZip = 0;
					memcpy(data.buffer,m_sDestBuffer,ziplen);
					m_pGateLog->LogCgate(LOG_DEBUG,&data,info->s_nIndex,info->s_sSessionKey);
					m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"��ѹ���ɹ� ����[%d] [%s]",\
							desnum,info->s_sSessionKey);
					break;
				}
				else
				{
					issuccess = true;
					data.head.nLen = deslen;
					data.head.stComm.cZip = FLAG_ZIP;
					memcpy(data.buffer,m_sSrcBuffer,deslen);
					m_pGateLog->LogCgate(LOG_DEBUG,&data,info->s_nIndex,info->s_sSessionKey);
					break;
				}
			}
			if (!issuccess)
			{
				return false;
			}
			break;
		case FLAG_SESSIONKEY:
			deslen = data.head.nLen;
			if (m_pDes.UncryptString(data.buffer,deslen,info->s_sSessionKey) <= 0)
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ��� [%s] %d",info->s_sSessionKey,deslen);
				return false;
			}
			data.head.nLen = deslen;
			data.head.stComm.cZip = 0;
			break;
		case FLAG_ZIPFIXEDKEY:
			memcpy(m_sSrcBuffer,data.buffer,data.head.nLen);
			deslen = data.head.nLen; 
			if (m_pDes.UncryptString(m_sSrcBuffer,deslen,DESENCRYPTKEY3) <=0 )
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ��� [%s]",info->s_sSessionKey);
				return false;
			}
			data.head.nLen = deslen;
			data.head.stComm.cZip = 2;
			m_pGateLog->LogCgate(LOG_DEBUG,&data,info->s_nIndex,info->s_sSessionKey);

			ziplen = CLIENTDATASIZE;
			if (!CBF_Tools::Uncompress((unsigned char *)m_sDestBuffer,ziplen,(unsigned char *)m_sSrcBuffer,deslen))
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"��ѹ������ �����ǽ��ܵ������Ѿ����� [%s]",\
					info->s_sSessionKey);
				return false;
			}
			data.head.nLen = ziplen;
			data.head.stComm.cZip = 0;
			memcpy(data.buffer,m_sDestBuffer,ziplen);
			break;
		case FLAG_FIXEDKEY:
			
			deslen = data.head.nLen; 
			if (m_pDes.UncryptString(data.buffer,deslen,DESENCRYPTKEY3) <=0 )
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ��� [%s]",info->s_sSessionKey);
				return false;
			}
			data.head.nLen = deslen;
			data.head.stComm.cZip = 0;
			break;
		case FLAG_ZIPCERTKEY:
			memcpy(m_sSrcBuffer,data.buffer,data.head.nLen);
			deslen = data.head.nLen; 
			if (m_pRes->g_nUseCert == 1)
			{
				shortlen = data.head.nLen;
				if (!m_pRes->g_pCertClass->UnEncrypt(m_sSrcBuffer,shortlen))
				{
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
					return false;
				}
				deslen = shortlen;
				data.head.stComm.cZip = FLAG_ZIP;
			}
			ziplen = CLIENTDATASIZE;
			if (!CBF_Tools::Uncompress((unsigned char *)m_sDestBuffer,ziplen,(unsigned char *)m_sSrcBuffer,deslen))
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"��ѹ������ �����ǽ��ܵ������Ѿ����� [%s]",\
					info->s_sSessionKey);
				return false;
			}
			data.head.nLen = ziplen;
			data.head.stComm.cZip = 0;
			memcpy(data.buffer,m_sDestBuffer,ziplen);
			break;
		case FLAG_CERTKEY:
			if (m_pRes->g_nUseCert == 1)
			{
				shortlen = data.head.nLen;
				if (!m_pRes->g_pCertClass->UnEncrypt(data.buffer,shortlen))
				{
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
					return false;
				}
				data.head.nLen = shortlen;
				data.head.stComm.cZip = 0;
			}
			break;
		default:
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"δ֪�ı�־����[%d]",data.head.stComm.cZip);
			return false;
	}
	return true;
}
bool CAIO_Work::ZipBuf(PSOCKET_POOL_DATA info,PCGATE_COMMSTRU  data,int nZipFlag)
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

std::string CAIO_Work::GetMsgType(int type)
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
std::string CAIO_Work::GetDrebCmdType(int cmdtype)
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

int CAIO_Work::SendData(PCGATE_COMMSTRU data,int index,bool isimmediate,int timestamp)
{
	S_CGATE_SMSG  msg;
	msg.index = index;
	msg.timestamp = timestamp;
	msg.d_nNodeId = 0;
	msg.d_cNodePrivateId = 0;
	msg.isBC = 0;
	msg.rtime = time(NULL);
	msg.s_nDrebSerial = 0;
	msg.txcode = 0;
	msg.s_nDrebSerial = 0;
	memcpy(&(msg.data),data,sizeof(CGATE_COMMSTRU));
	m_pSendData->PushData(msg,0);
	return 0;

}
int CAIO_Work::SendData(PCGATE_COMMSTRU data,int index)
{
	PSOCKET_POOL_DATA info = GetPoolData(index);
	if (info == NULL)
	{
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"����index[%d]�Ƿ�",index);
		return -1;
	}
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"��֤ ��ʼ�������� index[%d] ��ʶ[%d] cMsgType[%s] cCmd[%s] cRaFlag[%d] cNextFlag[%d] Node[%d %d] svr[%d %d] ������[%d] next[%d %d] czip[%d] nLen[%d]",\
		index,data->head.stDest.nSerial,GetMsgType(data->head.stComm.cMsgType).c_str(),\
		GetDrebCmdType(data->head.stComm.cCmd).c_str(),data->head.stComm.cRaflag,\
		data->head.stComm.cNextFlag,data->head.stDest.nNodeId,data->head.stDest.cNodePrivateId,\
		data->head.stDest.nSvrMainId,data->head.stDest.cSvrPrivateId,data->head.stDest.nServiceNo,\
		data->head.stNext.n_nNextNo,data->head.stNext.n_nNextOffset,data->head.stComm.cZip,data->head.nLen);
	if (info->s_hSocket == INVALID_SOCKET)
	{
		m_pLog->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"����index[%d]�ѹر�",index);
		return 0;
	}
	//��Բ�ͬ����Ϣ���м���ѹ��
	switch (data->head.stComm.cMsgType)
	{
	case MSG_GATEPING: //���ݲ��ü���ѹ��
		break;
	case MSG_GATEPKISTEP1://ѹ��+�̶���Կ
		if (!ZipBuf(info,data,FLAG_ZIPFIXEDKEY))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"ѹ��+�̶���Կ���ܳ���");
			return -1;
		}
		break;
	case MSG_GATEPKISTEP2://ѹ��+�̶���Կ
		if (!ZipBuf(info,data,FLAG_ZIPFIXEDKEY))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"ѹ��+�̶���Կ���ܳ���");
			return -1;
		}
		break;
	case MSG_GATEPKISTEP3://ѹ��+���ܹ�Կ
		if (!ZipBuf(info,data,FLAG_ZIPCERTKEY))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"ѹ��+���ܹ�Կ���ܳ���");
			return -1;
		}
		break;
	case MSG_GATESMS:
	case MSG_GATELOGIN:   //ѹ��+�Ự��ԿDES
	case MSG_GATEREQ:
	case MSG_PUSH:
	case MSG_GATESUBSCRIBE:
		if (!ZipBuf(info,data,FLAG_ZIPSESSIONKEY))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"ѹ��+�Ự��ԿDES���ܳ���");
			return -1;
		}
		break;
	default:
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"��Ϣ����[%s]�����ڴ˷���",GetMsgType(data->head.stComm.cMsgType).c_str());
		return -1;
	}
	SOCKET_SEND_DATA dd;
	dd.s_nIndex = info->s_nIndex;
	dd.s_nSendLen = 0;
	dd.s_nRet = 0;
	dd.s_nSendNum = 0;
	dd.s_nTimestamp = info->s_nTimestamp;
	dd.s_nTotalLen = CGATEHEADLEN+data->head.nLen;
	
	dd.s_nHook = data->head.stDest.nSerial;
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"��֤ ���ͳ���[%d] ���ݳ���[%d] ����ͷ����[%d] cZip[%d] index[%d] ��ʶ[%d]",\
		dd.s_nTotalLen,data->head.nLen,CGATEHEADLEN,data->head.stComm.cZip,info->s_nIndex,data->head.stDest.nSerial);
	//�ֽ���ת��
	CGATE_HEAD cgatehead;
	memcpy(&(cgatehead),&(data->head),CGATEHEADLEN);
	Endian2CommCrc(cgatehead);
	memcpy(dd.s_sSendBuffer,&(cgatehead),CGATEHEADLEN);
// 	Endian2CommCrc(data->head);
// 	memcpy(dd.s_sSendBuffer,&(data->head),CGATEHEADLEN);

	if (dd.s_nTotalLen > DREBBUFSIZE)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"��֤ �������ݳ���[%d]���� ",dd.s_nTotalLen);
	}
	else
	{
		memcpy(dd.s_sSendBuffer+CGATEHEADLEN,data->buffer,data->head.nLen);
	}
	

	if (info->s_hSocket == INVALID_SOCKET)
	{
		m_pLog->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"��֤ ����ʧ�� ����index[%d]�ѹر� ��ʶ[%d]",index,data->head.stDest.nSerial);
		return 0;
	}
	//��������
	if (!Send2Client(dd,true))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"��֤ ����ʧ�� index[%d] ��ʶ[%d]",index,data->head.stDest.nSerial);
		return 0;
	}
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"��֤ ���ͳɹ� index[%d] ��ʶ[%d]",index,data->head.stDest.nSerial);
	return 1;
}
std::string CAIO_Work::GenSignString()
{
	char tmpchar[49];
	bzero(tmpchar,sizeof(tmpchar));
	srand((unsigned int)CBF_Date_Time::GetTickCount());
	sprintf(tmpchar,"%05d%04u%07u",rand()%100000,CBF_Date_Time::GetTickCount()%10000,(int)CBF_Date_Time::GetTickUS()%10000000);
	return tmpchar;
}

void CAIO_Work::ProcessRcvMsg(PSOCKET_POOL_DATA info, CGATE_COMMSTRU &data)
{
	// #define  MSG_GATEPING      1          //�����ص�PING��Ϣ
	// #define  MSG_GATEREQ       2          //�������ص���������
	// #define  MSG_GATELOGIN     3          //���ص�¼
	// #define  MSG_GATESMS       4          //������֤
	// #define  MSG_PUSH          5          //api�յ���������������������(���顢����)��˽����(�ر���)
	// #define  MSG_GATEPKISTEP1  6          //����PKI��֤��һ�� ���ط���ǩ����
	// #define  MSG_GATEPKISTEP2  7          //����PKI��֤�ڶ��� API��������ǩ����֤�鹫Կ��ǩ���ַ���
	// #define  MSG_GATEPKISTEP3  8          //����PKI��֤������ ���ط���APIǩ����֤�鹫Կ���Ự��Կ

	// #define  CHECKFLAG_UNKNOW   0   //����δ֪
	// #define  CHECKFLAG_STEP1    1    //�ӽӿͻ��˷���ǩ������Կ
	// #define  CHECKFLAG_STEP2    2    //���տͻ�����֤����¼
	// #define  CHECKFLAG_NORMAL   5    //�������ӣ���¼����Ϊ��������
	if (info->s_cCheckFlag == CHECKFLAG_STEP1)
	{
		if (data.head.stComm.cMsgType != MSG_GATEPKISTEP2)
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"��Ϣ�Ƿ���ӦΪ��Ϣ��֤�ڶ��� [%s]",\
				GetMsgType(data.head.stComm.cMsgType).c_str());
			return;
		}
	}

	if (info->s_cCheckFlag == CHECKFLAG_STEP2)
	{
		if (data.head.stComm.cMsgType != MSG_GATELOGIN && data.head.stComm.cMsgType != MSG_GATESMS  )
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"��Ϣ�Ƿ���ӦΪ��¼����֤ [%s]",\
				GetMsgType(data.head.stComm.cMsgType).c_str());
			data.head.nRetCode = 109;
			data.head.nLen = 0;
			//ȷ��������
			SendData(&data,info->s_nIndex);
			OnClose(info,"��Ϣ�Ƿ�");//�ر�����
			return;
		}
	}
	

	S_BPC_RSMSG drebdata;
	drebdata.sMsgBuf = NULL;

	PKI_GATESTEP23 *pkidata=NULL;
	char keyserial[40];
	unsigned short  keyseriallen;
	unsigned short nSignBufferLen;
	unsigned short nSignPubKeyLen;
	unsigned short nRsaPubKeyLen;

	CSubScribeInfo *psub = NULL;
	char errmsg[255];
	bzero(errmsg,sizeof(errmsg));
	switch (data.head.stComm.cMsgType)
	{
		case MSG_GATEPING:   //������Ϣ
			data.head.stComm.cRaflag = 1;
			data.head.nLen = 0;
			data.head.stComm.cZip = 0;
			//ȷ��������
			SendData(&data,info->s_nIndex,false,0);
			break;
		case MSG_GATEPKISTEP2:
			pkidata = (PKI_GATESTEP23 *)data.buffer;
			m_pEndian.Endian2LocalHost((unsigned char *)&(pkidata->nSignBufferLen),sizeof(pkidata->nSignBufferLen));
			m_pEndian.Endian2LocalHost((unsigned char *)&(pkidata->nSignStrLen),sizeof(pkidata->nSignStrLen));
			m_pEndian.Endian2LocalHost((unsigned char *)&(pkidata->nRsaPubKeyLen),sizeof(pkidata->nRsaPubKeyLen));
			m_pEndian.Endian2LocalHost((unsigned char *)&(pkidata->nSignPubKeyLen),sizeof(pkidata->nSignPubKeyLen));
			m_pEndian.Endian2LocalHost((unsigned char *)&(pkidata->nKeySerialLen),sizeof(pkidata->nKeySerialLen));
			
			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"����[%s] index[%d] nSignBufferLen:%d nSignPubKeyLen:%d",inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,pkidata->nSignBufferLen,pkidata->nSignPubKeyLen);
			
			if (pkidata->nSignPubKeyLen >= sizeof(pkidata->sSignPubKey) || pkidata->nRsaPubKeyLen >= sizeof(pkidata->sRsaPubKey))
			{

				m_pLog->LogBin(LOG_ERROR,__FILE__,__LINE__,"�ͻ��˹�Կ",pkidata->sSignPubKey,pkidata->nSignPubKeyLen);
				m_pLog->LogBin(LOG_ERROR,__FILE__,__LINE__,"�ͻ���ǩ��",pkidata->sSignBuffer,pkidata->nSignBufferLen);
				m_pLog->LogBin(LOG_ERROR,__FILE__,__LINE__,"ǩ���ַ���",info->s_sSignString,strlen(info->s_sSignString));
				data.head.nRetCode = 100;
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"��֤ǩ��ʧ�� ����[%s] index[%d] ",inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex);
				data.head.nLen =0;
				data.head.stComm.cZip = 0;
				//ȷ��������
				SendData(&data,info->s_nIndex);
				OnClose(info,"��֤ǩ��ʧ��");//�ر�����
				return ;	
			}
			pkidata->sSignPubKey[pkidata->nSignPubKeyLen]=0;
			
			if (!m_pRes->g_pCertClass->CheckSign(pkidata->sSignPubKey,pkidata->nSignPubKeyLen,\
				info->s_sSignString,(unsigned short)strlen(info->s_sSignString),pkidata->sSignBuffer,pkidata->nSignBufferLen))
			{
				m_pLog->LogBin(LOG_ERROR,__FILE__,__LINE__,"�ͻ��˹�Կ",pkidata->sSignPubKey,pkidata->nSignPubKeyLen);
				m_pLog->LogBin(LOG_ERROR,__FILE__,__LINE__,"�ͻ���ǩ��",pkidata->sSignBuffer,pkidata->nSignBufferLen);
				m_pLog->LogBin(LOG_ERROR,__FILE__,__LINE__,"ǩ���ַ���",info->s_sSignString,strlen(info->s_sSignString));
				data.head.nRetCode = 100;
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"��֤ǩ��ʧ�� %s",m_pRes->g_pCertClass->GetErrMsg().c_str());
				data.head.nLen =0;
				data.head.stComm.cZip = 0;
				//ȷ��������
				SendData(&data,info->s_nIndex);
				OnClose(info,"��֤ǩ��ʧ��");//�ر�����
				return ;
			}
			//ȡ�Զ�֤�����к�
			bzero(keyserial,sizeof(keyserial));
			keyseriallen = sizeof(keyserial);
			if (!m_pRes->g_pCertClass->GetCertSerial(pkidata->sSignPubKey,pkidata->nSignPubKeyLen,keyserial,keyseriallen))
			{
				data.head.nRetCode = 103;
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ȡ�Զ�֤�����к�ʧ�� %s",m_pRes->g_pCertClass->GetErrMsg().c_str());
				data.head.nLen =0;
				data.head.stComm.cZip = 0;
				//ȷ��������
				SendData(&data,info->s_nIndex);
				OnClose(info,"ȡ�Զ�֤�����к�ʧ��");//�ر�����
				return ;
			}
			//���Զ��͹�����֤�����кź͹�Կ��Ĳ�һ�£�����
// 			if (strncmp(keyserial,pkidata->sKeySerial,keyseriallen) != 0)
// 			{
// 				data.head.nRetCode = 104;
// 				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"֤�����кŲ���");
// 				SendData(&data,info->s_nIndex,true);
// 				CloseClient(info);//�ر�����
// 				return ;
// 			}

			//��֤ǩ���ɹ� ���湫Կ
			memcpy(info->s_sSignPubKey,pkidata->sSignPubKey,pkidata->nSignPubKeyLen);
			info->s_nSignPubKeyLen = pkidata->nSignPubKeyLen;
			info->s_sSignPubKey[info->s_nSignPubKeyLen] =0;

			memcpy(info->s_sEncryptPubKey,pkidata->sSignPubKey,pkidata->nSignPubKeyLen);
			info->s_nEncryptPubKeyLen = pkidata->nSignPubKeyLen;
			info->s_sEncryptPubKey[info->s_nEncryptPubKeyLen] =0;
			
			//���ؽ���ǩ��
			pkidata->sSignStr[pkidata->nSignStrLen]=0;
			nSignBufferLen = 500;
			if (!m_pRes->g_pCertClass->Sign(pkidata->sSignStr,pkidata->nSignStrLen,pkidata->sSignBuffer,nSignBufferLen))
			{
				data.head.nRetCode = 101;
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ǩ��ʧ�� %s",m_pRes->g_pCertClass->GetErrMsg().c_str());
				data.head.nLen =0;
				data.head.stComm.cZip = 0;
				//ȷ��������
				SendData(&data,info->s_nIndex);
				OnClose(info,"ǩ��ʧ��");//�ر�����
				return ;
			}
			pkidata->nSignBufferLen = nSignBufferLen ;
			
			//ȡ���ع�Կ����
			nRsaPubKeyLen = sizeof(pkidata->sRsaPubKey);
			nSignPubKeyLen = sizeof(pkidata->sSignPubKey);
			if (!m_pRes->g_pCertClass->GetPubicKey(pkidata->sSignPubKey,nSignPubKeyLen,pkidata->sRsaPubKey,nRsaPubKeyLen))
			{
				data.head.nRetCode = 102;
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ȡ���ع�Կʧ�� %s",m_pRes->g_pCertClass->GetErrMsg().c_str());
				data.head.nLen =0;
				data.head.stComm.cZip = 0;
				//ȷ��������
				SendData(&data,info->s_nIndex);
				OnClose(info,"ȡ���ع�Կʧ��");//�ر�����
				return ;
			}
			pkidata->nRsaPubKeyLen = nRsaPubKeyLen;
			pkidata->nSignPubKeyLen = nSignPubKeyLen;
			

			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"����˷���[%s] index[%d]�Ĺ�Կ nSignPubKeyLen:%d nRsaPubKeyLen:%d",\
				inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,pkidata->nSignPubKeyLen,pkidata->nRsaPubKeyLen);
			//������ɻỰ��Կ
			data.head.stComm.cMsgType = MSG_GATEPKISTEP3;
			data.head.nRetCode = 0;
			bzero(info->s_sSessionKey,sizeof(info->s_sSessionKey));
			CBF_Tools::StringCopy(info->s_sSessionKey,48,GenSignString().c_str());
			strcpy(pkidata->sSignStr,info->s_sSessionKey);
			pkidata->nSignStrLen = strlen(pkidata->sSignStr);

			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"ip[%s] index[%d] �Ự��Կ[%s]",\
				inet_ntoa(info->s_pSocketAddr.sin_addr),info->s_nIndex,info->s_sSessionKey);
	
			//ȡ����֤�����кŷ���
			if (!m_pRes->g_pCertClass->GetCertSerial(pkidata->sSignPubKey,pkidata->nSignPubKeyLen,pkidata->sKeySerial,keyseriallen))
			{
				data.head.nRetCode = 106;
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ȡ����֤�����к�ʧ�� %s",m_pRes->g_pCertClass->GetErrMsg().c_str());
				data.head.nLen =0;
				data.head.stComm.cZip = 0;
				//ȷ��������
				SendData(&data,info->s_nIndex);
				OnClose(info,"ȡ����֤�����к�ʧ��");//�ر�����
				return ;
			}
			pkidata->nKeySerialLen = keyseriallen;
			data.head.nLen = sizeof(PKI_GATESTEP23);
			m_pEndian.Endian2Comm((unsigned char *)&(pkidata->nSignBufferLen),sizeof(pkidata->nSignBufferLen));
			m_pEndian.Endian2Comm((unsigned char *)&(pkidata->nSignStrLen),sizeof(pkidata->nSignStrLen));
			m_pEndian.Endian2Comm((unsigned char *)&(pkidata->nRsaPubKeyLen),sizeof(pkidata->nRsaPubKeyLen));
			m_pEndian.Endian2Comm((unsigned char *)&(pkidata->nSignPubKeyLen),sizeof(pkidata->nSignPubKeyLen));
			m_pEndian.Endian2Comm((unsigned char *)&(pkidata->nKeySerialLen),sizeof(pkidata->nKeySerialLen));
			//��������ŷ���
			sprintf(pkidata->sIndex,"%d",info->s_nIndex);
			info->s_cCheckFlag = CHECKFLAG_STEP2;
			//�������ر��ع�Կ��֤�����кš�ǩ�����
			//ȷ��ʹ��֤�����
			SendData(&data,info->s_nIndex);
			break;
		case MSG_GATESMS:
		case MSG_GATELOGIN:
		case MSG_GATEREQ:
			drebdata.index = info->s_nIndex;
			drebdata.nRtime = time(NULL);
			drebdata.sMsgBuf = (PBPCCOMMSTRU)m_pMemPool->PoolMalloc();
			if (drebdata.sMsgBuf == NULL)
			{
				data.head.nRetCode = 107;
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���仺����ʧ��");
				if (info->s_cCheckFlag != CHECKFLAG_NORMAL) //δ��֤ͨ�����ݲ�����
				{
					data.head.nLen = 0;
					data.head.stComm.cZip =0;
					//ȷ��������
					SendData(&data,info->s_nIndex);
				}
				else
				{
					//ͨ�������̼߳��ܷ���
					SendData(&data,info->s_nIndex,false,0);
				}
				
				return ;
			}
			bzero(drebdata.sMsgBuf,sizeof(PBPCCOMMSTRU));
			if (data.head.nLen>0 && data.head.nLen<DREBDATASIZE)
			{
				memcpy(drebdata.sMsgBuf->sBuffer,data.buffer,data.head.nLen);
			}
			else if  (data.head.nLen >= DREBDATASIZE)
			{
				data.head.nRetCode = 108;
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ͻ������ݳ��Ȳ��� [%d]",data.head.nLen);
				if (info->s_cCheckFlag != CHECKFLAG_NORMAL) //δ��֤ͨ�����ݲ�����
				{
					data.head.nLen = 0;
					data.head.stComm.cZip = 0;
					//ȷ��������
					SendData(&data,info->s_nIndex);
				}
				else
				{
					data.head.nLen = 0;
					data.head.stComm.cZip = 0;
					//ͨ�������̼߳��ܷ���
					SendData(&data,info->s_nIndex,false,0);
				}
				m_pMemPool->PoolFree(drebdata.sMsgBuf);
				drebdata.sMsgBuf = NULL;
				return ;
			}
			bzero(&(drebdata.sMsgBuf->sDBHead), DREBHEADLEN);
			drebdata.nTotalLen = BPCHEADLEN+DREBHEADLEN + data.head.nLen;
			//����ͷ
			drebdata.sMsgBuf->sDBHead.nLen = data.head.nLen;
			drebdata.sMsgBuf->sDBHead.cCmd = data.head.stComm.cCmd;
			drebdata.sMsgBuf->sDBHead.cZip = data.head.stComm.cZip;
			drebdata.sMsgBuf->sDBHead.cDrebAffirm = 0;
			drebdata.sMsgBuf->sDBHead.cNextFlag = data.head.stComm.cNextFlag; //�Ƿ���ȡ������
			drebdata.sMsgBuf->sDBHead.cRaflag = data.head.stComm.cRaflag;//����Ӧ���־
			
            //Ŀ����Ϣ������Ž������
			drebdata.sMsgBuf->sDBHead.d_Dinfo.d_nNodeId = data.head.stDest.nNodeId;
			drebdata.sMsgBuf->sDBHead.d_Dinfo.d_cNodePrivateId = data.head.stDest.cNodePrivateId;
            drebdata.sMsgBuf->sDBHead.d_Dinfo.d_nSvrMainId = data.head.stDest.nSvrMainId;
			drebdata.sMsgBuf->sDBHead.d_Dinfo.d_cSvrPrivateId = data.head.stDest.cSvrPrivateId;
			drebdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo = data.head.stDest.nServiceNo;
			
			//ȡ����������Ϣ
			drebdata.sMsgBuf->sDBHead.n_Ninfo.n_nNextNo = data.head.stNext.n_nNextNo;
			drebdata.sMsgBuf->sDBHead.n_Ninfo.n_nNextOffset = data.head.stNext.n_nNextOffset;
			//������Ϣ���ͻ�����ˮ������ʱ���
			drebdata.sMsgBuf->sDBHead.s_Sinfo.s_nGateIndex = info->s_nIndex;	//����index
			drebdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial = data.head.stDest.nSerial;	//�ͻ�����ˮ
			drebdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook = info->s_nTimestamp;//ʱ���

			
			//������У�����Ϣ�߳�������
			m_pDataPool->PushData(drebdata);
			if (data.head.stComm.cMsgType == MSG_GATELOGIN) 
			{
				info->s_cCheckFlag = CHECKFLAG_NORMAL;
			}
			break;
		case MSG_GATESUBSCRIBE://�����ر�����
			if (NULL == info->ptr )
			{
				data.head.nRetCode = 110;
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���鶩��ʱ������ָ��ΪNULL");
				if (info->s_cCheckFlag != CHECKFLAG_NORMAL) //δ��֤ͨ�����ݲ�����
				{
					data.head.nLen = 0;
					data.head.stComm.cZip = 0;
					data.head.stComm.cRaflag = 1;
					data.head.stComm.cMsgType = MSG_GATESUBSCRIBE;
					//ȷ��������
					SendData(&data,info->s_nIndex);
				}
				else
				{
					data.head.nLen = 0;
					data.head.stComm.cZip = 0;
					data.head.stComm.cRaflag = 1;
					data.head.stComm.cMsgType = MSG_GATESUBSCRIBE;
					//ͨ�������̼߳��ܷ���
					SendData(&data,info->s_nIndex,false,0);
				}
				return ;
			}
			psub = (CSubScribeInfo *)info->ptr;
			if (psub == NULL)
			{
				return;
			}
			if (!psub->Subscribe(data.buffer,data.head.nLen,errmsg))
			{
				data.head.nRetCode = 111;
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���鶩��ʧ�� %s",errmsg);
				if (info->s_cCheckFlag != CHECKFLAG_NORMAL) //δ��֤ͨ�����ݲ�����
				{
					data.head.nLen = 0;
					data.head.stComm.cZip = 0;
					data.head.stComm.cRaflag = 1;
					data.head.stComm.cMsgType = MSG_GATESUBSCRIBE;
					//ȷ��������
					SendData(&data,info->s_nIndex);
				}
				else
				{
					data.head.nLen = 0;
					data.head.stComm.cZip = 0;
					data.head.stComm.cRaflag = 1;
					data.head.stComm.cMsgType = MSG_GATESUBSCRIBE;
					//ͨ�������̼߳��ܷ���
					SendData(&data,info->s_nIndex,false,0);
				}
				return ;
			}
			data.head.nRetCode = 0;
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���鶩�ĳɹ�");
			if (info->s_cCheckFlag != CHECKFLAG_NORMAL) //δ��֤ͨ�����ݲ�����
			{
				data.head.nLen = 0;
				data.head.stComm.cZip = 0;
				data.head.stComm.cRaflag = 1;
				data.head.stComm.cMsgType = MSG_GATESUBSCRIBE;
				//ȷ��������
				SendData(&data,info->s_nIndex);
			}
			else
			{
				data.head.nLen = 0;
				data.head.stComm.cZip = 0;
				data.head.stComm.cRaflag = 1;
				data.head.stComm.cMsgType = MSG_GATESUBSCRIBE;
				//ͨ�������̼߳��ܷ���
				SendData(&data,info->s_nIndex,false,0);
			}
			break;
		default:
			break;
		
	}
	return;
}

void CAIO_Work::SendBCData(CGATE_COMMSTRU &data)
{
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"��ʼ���͹㲥���� ��ʶ[%d] cMsgType[%s] cCmd[%s] cRaFlag[%d] cNextFlag[%d] Node[%d %d] svr[%d %d] ������[%d] next[%d %d]",\
		data.head.stDest.nSerial,GetMsgType(data.head.stComm.cMsgType).c_str(),\
		GetDrebCmdType(data.head.stComm.cCmd).c_str(),data.head.stComm.cRaflag,\
		data.head.stComm.cNextFlag,data.head.stDest.nNodeId,data.head.stDest.cNodePrivateId,\
		data.head.stDest.nSvrMainId,data.head.stDest.cSvrPrivateId,data.head.stDest.nServiceNo,\
		data.head.stNext.n_nNextNo,data.head.stNext.n_nNextOffset);

	CGATE_COMMSTRU senddata;
	bzero(&senddata,sizeof(CGATE_COMMSTRU));
	int total;
	int poolsize = GetConnectPoolInfo(total);
	PSOCKET_POOL_DATA info  = NULL;
	for (int i=1 ; i<poolsize ; i++) //0�����ص�����
	{
		
		memcpy(&senddata,&data,sizeof(CGATE_COMMSTRU));	
		info =  GetPoolData(i);
		if (info == NULL)
		{
			continue;
		}
		if (info->s_cCheckFlag != CHECKFLAG_NORMAL) //δ��֤ͨ�����ݲ�����
		{
//			m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"����index[%d]Ϊ��֤ͨ�����ݲ���������",i);
			continue;
		}
		//ͨ�������̼߳��ܷ���
		SendData(&senddata,i,false,0);
	}
	return ;
}


PSOCKET_POOL_DATA CAIO_Work::GetSocketByIndex(int index)
{
	return m_pAioPool->GetDataByIndex(index);
}
