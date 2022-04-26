// DrebMsgThread.cpp: implementation of the CDrebMsgThread class.
//
//////////////////////////////////////////////////////////////////////

#include "DrebMsgThread.h"
#include "TradeResource.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern int   g_pRunFlag;

CDrebMsgThread::CDrebMsgThread()
{
	m_bIsCheck = false;
	m_pLog = NULL;
	m_pAnsProc = NULL;
}

CDrebMsgThread::~CDrebMsgThread()
{
	m_pLog = NULL;
}
bool CDrebMsgThread::Init(CIBF_DrebServer * bus,CBF_DrebResource *res)
{
	m_pDrebApi = bus;
	m_pRes = res;
	m_pLog = m_pDrebApi->GetLogPoint();
	m_pTime.Init(100,true);
	m_pTime.SetTimer(0,((CTradeResource *)m_pRes)->m_nTradeReportTime*1000,&CDrebMsgThread::OnTimer,this);
	m_pTime.SetTimer(1,5000,&CDrebMsgThread::OnTimer,this);
	m_pTime.SetTimer(2,5000,&CDrebMsgThread::OnTimer,this);
	

	char errmsg[200];
	bzero(errmsg,sizeof(errmsg));
	if (!m_pXdp.InitXdp(((CTradeResource *)m_pRes)->m_sXdpDefineXml.c_str(),errmsg))
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP Init error %s",errmsg);
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP���ĳ�ʼ������ %s",errmsg);
#endif
		return false;
	}
	if (!m_pSendXdp.XdpCopy(&m_pXdp))
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP XdpCopy error");
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP���ĳ�ʼ��XdpCopy���� ");
#endif
		return false;
	}
	return true;
}
int CDrebMsgThread::OnTimer(unsigned int event, void *p)
{
	CDrebMsgThread *pp = (CDrebMsgThread *)p;
	if (event == 0)
	{
		//��������״̬���ٲ�
		pp->Send2Arb();
	}
	else if (event == 2)
	{
		pp->PrintHostStatus();
	}
	else if (event == 1)
	{
		pp->SendQueryHostStatus();
	}
	return 0;
}
int CDrebMsgThread::Run()
{
#ifdef _ENGLISH_
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"Start DREB msg thread");
#else
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"����������Ϣ�����߳�");
#endif
	while (m_bIsRunning)
	{
		if (m_pDrebApi == NULL)
		{
			m_bIsRunning = false;
			return -1;
		}
		if (m_pDrebApi->GetMsgData(m_rMsgData,500,true))
		{
			ProcessDrebData(m_rMsgData);
		}
		else
		{
			SLEEP(1);
		}
	}
#ifdef _ENGLISH_
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"Stop DREB msg thread");
#else
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"ֹͣ������Ϣ�����߳�");
#endif
	return 0;
}


// ������: OnMsgConnectBack
// ���  : ������ 2015-4-23 15:55:29
// ����  : virtual void 
// ����  : S_BPC_RSMSG &rcvdata
// ����  : �����������߳ɹ�����Ӧ
void CDrebMsgThread::OnMsgConnectBack(S_BPC_RSMSG &rcvdata)
{
	//����ע���
#ifdef _ENGLISH_
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"Recv DREB connect rsp");
#else
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�յ�DREB���ӳɹ�����Ӧ");
#endif
	CTradeResource *res = (CTradeResource *)m_pRes;
	if (res->m_nArbMode != 1)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"Set Living mode");
#else
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"����Ϊ���ؾ���ģʽ�����������ٲ�");
#endif
		res->m_nSvrHostStatus = ARBSTATUS_MASTER;
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		//ע�ύ��
		for (int i=0; i< m_pRes->g_vDrebLinkInfo.size(); i++)
		{
			m_pDrebApi->RegisterDreb(i,&((CTradeResource *)m_pRes)->g_lArbTxList);
		}
		return;
	}
	if (((CTradeResource *)m_pRes)->m_nArbMode != 1)
	{
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	//�����ѯ���������ٲõ�״̬������״̬��ע��
	char errmsg[4096];
	bzero(errmsg,sizeof(errmsg));
	m_pXdp.ResetData();
	if (!m_pXdp.SetFieldValue("uint3",m_pRes->g_nSvrMainId,errmsg))
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"Xdp Set error");
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xdp��������״̬���� %s",errmsg);
#endif
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (!m_pXdp.SetFieldValue("uint4",(unsigned int)(m_pRes->g_nSvrPrivateId),errmsg))
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"Xdp Set error");
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xdp���÷�������� %s",errmsg);
#endif
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (m_pLog->isWrite(LOG_DEBUG+1))
	{
		m_pXdp.PrintXdp(errmsg);
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"query host status: %s",errmsg);
#else
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"Ӧ�ò�ѯ����״̬���� %s",errmsg);
#endif
	}
	unsigned int datalen = BPUDATASIZE;
	if (!m_pXdp.ToBuffer(rcvdata.sMsgBuf->sBuffer,datalen,errmsg))
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ToBuffer error %s",errmsg);
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xdp ToBuffer���� %s",errmsg);
#endif
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	rcvdata.sMsgBuf->sDBHead.nLen = datalen;
	
	rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 0;
	rcvdata.sMsgBuf->sDBHead.cCmd = CMD_DPCALL;
	rcvdata.sMsgBuf->sDBHead.cZip = 0;
	rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo = APP_QUERY_HOST;
	m_bIsQueryAns = false;//��ѯӦ��Ϊfalse����һֱΪfalseʱҪ���·����ѯ����ֹ�ٲ����⵼�·���������
	m_pDrebApi->SendMsg(rcvdata);

	
}

// ������: OnMsgDrebAns
// ���  : ������ 2015-4-23 15:55:58
// ����  : virtual void 
// ����  : S_BPC_RSMSG &rcvdata
// ����  : ��Ӧ���߹�����Ӧ������
void CDrebMsgThread::OnMsgDrebAns(S_BPC_RSMSG &rcvdata)
{
	//Ӧ������

	if (rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo == APP_QUERY_HOST)
	{
#ifdef _ENGLISH_
        m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "Recv DREB RSP");
#else
        m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "�յ�DREB��Ӧ����Ӧ");
#endif
		OnQueryArbAns(rcvdata);
		return;
	}
	else
	{
		S_TRADE_DATA data;
		data.src = 0;
		data.rTime = time(NULL);
		data.index = rcvdata.index;
		data.pData = rcvdata.sMsgBuf;
		m_pAnsProc->PushAnsQueue(data);
//		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
	}
	

}

// ������: OnMsgRequest
// ���  : ������ 2015-4-23 15:54:38
// ����  : virtual void 
// ����  : S_BPC_RSMSG &rcvdata
// ����  : �������߹�������������
void CDrebMsgThread::OnMsgRequest(S_BPC_RSMSG &rcvdata)
{
	if (rcvdata.sMsgBuf->sDBHead.cRaflag !=0) //��Ϊ����
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"not function msg[%s]  DREB[%s] next[%d] RA[%d] txcode[%d]  flag[%d %d %d] src[%d %d] cZip[%d] datalen[%d]",\
			GetBpcMsgType(rcvdata.sMsgBuf->sBpcHead.cMsgType).c_str(),\
			GetDrebCmdType(rcvdata.sMsgBuf->sDBHead.cCmd).c_str(),rcvdata.sMsgBuf->sDBHead.cNextFlag,\
			rcvdata.sMsgBuf->sDBHead.cRaflag,rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
			rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
			rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
			rcvdata.sMsgBuf->sDBHead.cZip,rcvdata.sMsgBuf->sDBHead.nLen);
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�޴˹��� ��Ϣ[%s]  DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Դ[%d %d] cZip[%d] ҵ�����ݳ���[%d]",\
			GetBpcMsgType(rcvdata.sMsgBuf->sBpcHead.cMsgType).c_str(),\
			GetDrebCmdType(rcvdata.sMsgBuf->sDBHead.cCmd).c_str(),rcvdata.sMsgBuf->sDBHead.cNextFlag,\
			rcvdata.sMsgBuf->sDBHead.cRaflag,rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
			rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
			rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
			rcvdata.sMsgBuf->sDBHead.cZip,rcvdata.sMsgBuf->sDBHead.nLen);
#endif
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	switch (rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo)
	{
		case  ARB_SEND_CONTROL:
			OnArbControl(rcvdata);//ָ������
			break;
		case  ARB_SEND_QUERY://��ѯ״̬
			OnArbQuery(rcvdata);
			break;
		default:
			//�����������������
			OnSvrRequest(rcvdata);
			break;
	}
	return;
	
}
// ������: Start
// ���  : ������ 2015-4-23 15:52:29
// ����  : virtual bool 
// ����  : ���������߳�
bool CDrebMsgThread::Start()
{
	if (NULL == m_pDrebApi || NULL == m_pRes)
	{
		return false;
	}
	if (m_pTime.IsStoped())
	{
		m_pTime.Start();
	}
	m_bIsRunning = true;
	CreateThread();
	return true;
}

// ������: Stop
// ���  : ������ 2015-4-23 15:52:43
// ����  : virtual void 
// ����  : ֹͣ�����߳�
void CDrebMsgThread::Stop()
{
	if (!m_pTime.IsStoped())
	{
		m_pTime.Stop();
	}
	m_bIsRunning = false;
	Join();
}

void CDrebMsgThread::Send2Arb()
{
	CTradeResource *arbconf = (CTradeResource *)m_pRes;
	if (arbconf->m_nSvrHostStatus == ARBSTATUS_UNKNOW)
	{
		//δ֪״̬����ȥ����
		return;
	}
	if (arbconf->m_nArbMode != 1)
	{
		//������ģʽ�����÷�
		return;
	}
	S_BPC_RSMSG data;
	data.sMsgBuf = (PBPCCOMMSTRU)m_pDrebApi->PoolMalloc();
	if (data.sMsgBuf == NULL)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"SVR[%d %d] Send to ARB PoolMalloc error",m_pRes->g_nSvrMainId,m_pRes->g_nSvrPrivateId);
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"��������״̬֪ͨʧ��[%d %d] PoolMalloc error",m_pRes->g_nSvrMainId,m_pRes->g_nSvrPrivateId);
#endif
		return;
	}
	char errmsg[250];
	bzero(errmsg,sizeof(errmsg));
	bzero(data.sMsgBuf,sizeof(BPCCOMMSTRU));
	data.sMsgBuf->sBpcHead.nIndex = 100;
	data.sMsgBuf->sDBHead.cCmd = CMD_DPPUSH;
	data.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo = APP_PUSH_STATUS;
	data.sMsgBuf->sDBHead.d_Dinfo.d_nSvrMainId = 0;
	data.sMsgBuf->sDBHead.d_Dinfo.d_cSvrPrivateId = 0;
	
	if (!m_pSendXdp.SetFieldValue("uint3",m_pRes->g_nSvrMainId,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP SetFieldValue uint3 error: %s",errmsg);
		m_pDrebApi->PoolFree(data.sMsgBuf);
		return;
	}
	if (!m_pSendXdp.SetFieldValue("uint4",(unsigned int)m_pRes->g_nSvrPrivateId,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP SetFieldValue uint4 error: %s",errmsg);
		m_pDrebApi->PoolFree(data.sMsgBuf);
		return;
	}
	if (!m_pSendXdp.SetFieldValue("uint9",((CTradeResource *)m_pRes)->m_nSvrHostStatus,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP SetFieldValue uint9 error: %s",errmsg);
		m_pDrebApi->PoolFree(data.sMsgBuf);
		return;
	}
	unsigned int len = BPUDATASIZE;
	if (!m_pSendXdp.ToBuffer(data.sMsgBuf->sBuffer,len,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP ToBuffer error: %s",errmsg);
		m_pDrebApi->PoolFree(data.sMsgBuf);
		return;
	}
	data.sMsgBuf->sDBHead.nLen = len;
//	m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"�����ٲ�֪ͨ");
	m_pDrebApi->SendMsg(data); //�����ٲ�֪ͨ
}

void CDrebMsgThread::OnQueryArbAns(S_BPC_RSMSG &rcvdata)
{
	//��ѯ����״̬�ı���
	char errmsg[4096];
	bzero(errmsg,sizeof(errmsg));
	if (!m_pXdp.FromBuffer(rcvdata.sMsgBuf->sBuffer,rcvdata.sMsgBuf->sDBHead.nLen,errmsg))
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"arb rsp non XDP %s",errmsg);
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"arb���صı��Ĳ�������XDP���� %s",errmsg);
#endif
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (m_pLog->isWrite(LOG_DEBUG+1))
	{
		m_pXdp.PrintXdp(errmsg);
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"arb rsp %s",errmsg);
#else
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"arb����Ӧ�ò�ѯ���� %s",errmsg);
#endif
	}
	unsigned int status=0;
	unsigned int retcode=0;

	if (!m_pXdp.GetFieldValue("uint9",status,errmsg))
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"arb rsp ,no uint9 field %s",errmsg);
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"arb����Ӧ�ò�ѯ���Ĳ�������uint9�ֶ� %s",errmsg);
#endif
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (!m_pXdp.GetFieldValue("uint10",retcode,errmsg))
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"arb rsp ,no uint10 field %s",errmsg);
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"arb����Ӧ�ò�ѯ���Ĳ�������uint10�ֶ� %s",errmsg);
#endif
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}	
	if (retcode != SUCCESS)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"arb rsp ,ret[%d] error",retcode);
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"arb����Ӧ�ò�ѯ �ķ�����[%d]Ϊ���ɹ�",retcode);
#endif
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
#ifdef _ENGLISH_
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"arb rsp app status[%d]",status);
#else
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"arb����Ӧ�ò�ѯ ��Ӧ��״̬[%d]",status);
#endif
	if (status == ARBSTATUS_MASTER || status == ARBSTATUS_MASTER_AYN )
	{
		//Ϊ����
		OnChangeHostStatus(status);
	}
	else if (status == ARBSTATUS_BACKUP || status == status == ARBSTATUS_BACKUP_AYN)
	{
		OnChangeHostStatus(status);
	}
	else
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"arb rsp app status [%d] illegal",status);
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"arb����Ӧ�ò�ѯ ��Ӧ��״̬[%d]����",status);
#endif
	}
	m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
	rcvdata.sMsgBuf = NULL;
	m_bIsQueryAns = true;
	return;
	
}

void CDrebMsgThread::OnChangeHostStatus(unsigned int status)
{
	CTradeResource *arbconf = (CTradeResource *)m_pRes;
#ifdef _ENGLISH_
	m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"Recv  ARB  cmd status[%d], current status[%d]",status,arbconf->m_nSvrHostStatus);
#else
	m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"�յ��ٲ�֪ͨ  ָ��״̬[%d] ��ǰ״̬[%d]",status,arbconf->m_nSvrHostStatus);
#endif
	if (status == ARBSTATUS_MASTER || status == ARBSTATUS_MASTER_AYN )
	{
		//Ϊ����
		if (arbconf->m_nSvrHostStatus != ARBSTATUS_MASTER && arbconf->m_nSvrHostStatus != ARBSTATUS_MASTER_AYN)
		{
			//תΪ����
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"SVR is HOST ,begin start");
#else
			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"������Ϊ����  ��ʼ����api");
#endif
			//ע�ύ��
			for (int i=0; i< m_pRes->g_vDrebLinkInfo.size(); i++)
			{
				m_pDrebApi->RegisterDreb(i,&((CTradeResource *)m_pRes)->g_lSvrTxList);
			}
			arbconf->m_nSvrHostStatus = ARBSTATUS_MASTER;
			//����תΪ�����Ĵ��������ӽ�������
			OnHostProc();
		}
	}
	else if (status == ARBSTATUS_BACKUP || status  == ARBSTATUS_BACKUP_AYN)
	{
		if (arbconf->m_nSvrHostStatus != ARBSTATUS_BACKUP && arbconf->m_nSvrHostStatus != ARBSTATUS_BACKUP_AYN)
		{
			if (arbconf->m_nSvrHostStatus == ARBSTATUS_UNKNOW)
			{
				arbconf->m_nSvrHostStatus = ARBSTATUS_BACKUP;
#ifdef _ENGLISH_
				m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"SVR is backup");
#else
				m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"������Ϊ����");
#endif
				return;
			}
			//תΪ����
			for (int i=0; i< m_pRes->g_vDrebLinkInfo.size(); i++)
			{
				m_pDrebApi->UnRegisterDreb();
				m_pDrebApi->RegisterDreb(i,&((CTradeResource *)m_pRes)->g_lArbTxList);
			}
			arbconf->m_nSvrHostStatus = ARBSTATUS_BACKUP;
		}
	}
}

void CDrebMsgThread::OnSvrRequest(S_BPC_RSMSG &rcvdata)
{
	S_TRADE_DATA data;
	data.src = 0;   //��ʾ�����߹�����
	data.pData = rcvdata.sMsgBuf;
	data.pData->sBpcHead.nBpuIndex = 0;//��ʾ�����߹�����
	data.rTime = time(NULL);
// 	//ȥ���㲥
// 	if (rcvdata.sMsgBuf->sDBHead.cCmd == CMD_DPABC || rcvdata.sMsgBuf->sDBHead.cCmd == CMD_DPBC)
// 	{
// 		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
// 		rcvdata.sMsgBuf = NULL;
// 		return;
// 	}

	//�������
	m_pRcvQueue->PushData(data);

	return;
}

void CDrebMsgThread::OnArbControl(S_BPC_RSMSG &rcvdata)
{
	char errmsg[4096];
	bzero(errmsg,sizeof(errmsg));
	if (!m_pXdp.FromBuffer(rcvdata.sMsgBuf->sBuffer,rcvdata.sMsgBuf->sDBHead.nLen,errmsg))
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ARB non XDP %s",errmsg);
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ٲ�ָ�����Ĳ�������XDP���� %s",errmsg);
#endif
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (m_pLog->isWrite(LOG_DEBUG+1))
	{
		m_pXdp.PrintXdp(errmsg);
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"ARB Control XDP: %s",errmsg);
#else
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ٲ�ָ������ %s",errmsg);
#endif
	}
	unsigned int id=0;
	unsigned int pid=0;
	unsigned int status=0;
	
	if (!m_pXdp.GetFieldValue("uint3",id,errmsg))
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ARB Control XDP: %s",errmsg);
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ٲ�ָ�����Ĳ��� ��uint3�ֶ� %s",errmsg);
#endif
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	
	if (!m_pXdp.GetFieldValue("uint4",pid,errmsg))
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ARB Control illegal no uint4 field %s",errmsg);
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ٲ�ָ�����Ĳ��� ��uint4�ֶ� %s",errmsg);
#endif
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	
	if (!m_pXdp.GetFieldValue("uint9",status,errmsg))
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ARB Control illegal no uint9 field %s",errmsg);
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ٲ�ָ�����Ĳ��� ��uint9�ֶ� %s",errmsg);
#endif
		
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (id != m_pRes->g_nSvrMainId || pid != m_pRes->g_nSvrPrivateId)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ARB Control illegal ,svr[%d %d] ",id,pid);
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ٲ�ָ�����Ĳ��� ����[%d %d] ���Ǳ�����",id,pid);
#endif
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
#ifdef _ENGLISH_
	m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"Arb Control status[%d]",status);
#else
	m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"ARBָ����״̬Ϊ[%d]",status);
#endif
	//�ı�״̬
	OnChangeHostStatus(status);
	CTradeResource *arbconf = (CTradeResource *)m_pRes;
	if (!m_pXdp.SetFieldValue("uint9",arbconf->m_nSvrHostStatus,errmsg))
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP  SetFieldValue error");
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP  SetFieldValue ����");
#endif
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (!m_pXdp.SetFieldValue("uint10",(unsigned int)SUCCESS,errmsg))
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP  SetFieldValue error");
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP  SetFieldValue ����");
#endif
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (m_pLog->isWrite(LOG_DEBUG+1))
	{
		m_pXdp.PrintXdp(errmsg);
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"answer to ARB: %s",errmsg);
#else
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ٲ�ָ������Ӧ���� %s",errmsg);
#endif
	}
	unsigned int datalen = BPUDATASIZE;
	if (!m_pXdp.ToBuffer(rcvdata.sMsgBuf->sBuffer,datalen,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xdp ToBuffer error %s",errmsg);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	rcvdata.sMsgBuf->sDBHead.nLen = datalen;
	
	rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
	rcvdata.sMsgBuf->sDBHead.cZip = 0;
	m_pDrebApi->SendMsg(rcvdata);
	


}

void CDrebMsgThread::OnArbQuery(S_BPC_RSMSG &rcvdata)
{
	char errmsg[4096];
	bzero(errmsg,sizeof(errmsg));
	if (!m_pXdp.FromBuffer(rcvdata.sMsgBuf->sBuffer,rcvdata.sMsgBuf->sDBHead.nLen,errmsg))
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ARB query illegal, FromBuffer error: %s",errmsg);
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ٲò�ѯ���Ĳ�������XDP���� %s",errmsg);
#endif
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (m_pLog->isWrite(LOG_DEBUG+1))
	{
		m_pXdp.PrintXdp(errmsg);
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"ARB query data: %s",errmsg);
#else
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ٲò�ѯ���� %s",errmsg);
#endif
	}
	unsigned int id=0;
	unsigned int pid=0;
	unsigned int status=0;
	
	if (!m_pXdp.GetFieldValue("uint3",id,errmsg))
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ARB query illegal ,no uint3 field");
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ٲò�ѯ���Ĳ��� ��uint3�ֶ� %s",errmsg);
#endif
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	
	if (!m_pXdp.GetFieldValue("uint4",pid,errmsg))
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ARB query illegal ,no uint4 field");
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ٲò�ѯ���Ĳ��� ��uint4�ֶ� %s",errmsg);
#endif
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	
	
	if (id != m_pRes->g_nSvrMainId || pid != m_pRes->g_nSvrPrivateId)
	{
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ARB query illegal ,svr[%d %d]",id,pid);
#else
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ٲò�ѯ���Ĳ��� ����[%d %d] ���Ǳ�����",id,pid);
#endif
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	
	CTradeResource *arbconf = (CTradeResource *)m_pRes;
	if (!m_pXdp.SetFieldValue("uint9",arbconf->m_nSvrHostStatus,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP  SetFieldValue error");
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (!m_pXdp.SetFieldValue("uint10",(unsigned int)SUCCESS,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP  SetFieldValue error");
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (m_pLog->isWrite(LOG_DEBUG+1))
	{
		m_pXdp.PrintXdp(errmsg);
#ifdef _ENGLISH_
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"ARB query answer: %s",errmsg);
#else
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ٲò�ѯӦ���� %s",errmsg);
#endif
	}
	unsigned int datalen = BPUDATASIZE;
	if (!m_pXdp.ToBuffer(rcvdata.sMsgBuf->sBuffer,datalen,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xdp ToBuffer error %s",errmsg);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	rcvdata.sMsgBuf->sDBHead.nLen = datalen;
	
	rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
	rcvdata.sMsgBuf->sDBHead.cZip = 0;
	m_pDrebApi->SendMsg(rcvdata);	
}

void CDrebMsgThread::PrintHostStatus()
{
#ifdef _ENGLISH_
	switch (((CTradeResource *)m_pRes)->m_nSvrHostStatus)
	{
	case ARBSTATUS_UNKNOW:
		printf("node[%d %d] current status [unknow]\n",m_pRes->g_nSvrMainId,m_pRes->g_nSvrPrivateId);
		
		break;
	case ARBSTATUS_MASTER:
		printf("node[%d %d] current status [master]\n",m_pRes->g_nSvrMainId,m_pRes->g_nSvrPrivateId);
		break;
	case ARBSTATUS_MASTER_AYN:
		printf("node[%d %d] current status [master-ayn]\n",m_pRes->g_nSvrMainId,m_pRes->g_nSvrPrivateId);
		break;
	case ARBSTATUS_BACKUP:
		printf("node[%d %d] current status [backup]\n",m_pRes->g_nSvrMainId,m_pRes->g_nSvrPrivateId);
		break;
	case ARBSTATUS_BACKUP_AYN:
		printf("node[%d %d] current status [backup_ayn]\n",m_pRes->g_nSvrMainId,m_pRes->g_nSvrPrivateId);
		break;
	case MASTERHOST:
		printf("node[%d %d] current status [LIVING]\n",m_pRes->g_nSvrMainId,m_pRes->g_nSvrPrivateId);
		break;
	default: 
		printf("node[%d %d] current status error %d\n",m_pRes->g_nSvrMainId,m_pRes->g_nSvrPrivateId,((CTradeResource *)m_pRes)->m_nSvrHostStatus);
		break;
		
	}
#else

	switch (((CTradeResource *)m_pRes)->m_nSvrHostStatus)
	{
		case ARBSTATUS_UNKNOW:
			printf("�ڵ�[%d %d]��ǰ����״̬Ϊ [δ֪]\n",m_pRes->g_nSvrMainId,m_pRes->g_nSvrPrivateId);

			break;
		case ARBSTATUS_MASTER:
			printf("�ڵ�[%d %d]��ǰ����״̬Ϊ [����]\n",m_pRes->g_nSvrMainId,m_pRes->g_nSvrPrivateId);
			break;
		case ARBSTATUS_MASTER_AYN:
			printf("�ڵ�[%d %d]��ǰ����״̬Ϊ [����ͬ��]\n",m_pRes->g_nSvrMainId,m_pRes->g_nSvrPrivateId);
			break;
		case ARBSTATUS_BACKUP:
			printf("�ڵ�[%d %d]��ǰ����״̬Ϊ [����]\n",m_pRes->g_nSvrMainId,m_pRes->g_nSvrPrivateId);
			break;
		case ARBSTATUS_BACKUP_AYN:
			printf("�ڵ�[%d %d]��ǰ����״̬Ϊ [����ͬ��]\n",m_pRes->g_nSvrMainId,m_pRes->g_nSvrPrivateId);
			break;
		case MASTERHOST:
			printf("�ڵ�[%d %d]��ǰ����״̬Ϊ [���ؾ�������]\n",m_pRes->g_nSvrMainId,m_pRes->g_nSvrPrivateId);
			break;
		default: 
			printf("�ڵ�[%d %d]��ǰ����״̬Ϊ����״̬ %d\n",m_pRes->g_nSvrMainId,m_pRes->g_nSvrPrivateId,((CTradeResource *)m_pRes)->m_nSvrHostStatus);
			break;

	}
#endif	
}

void CDrebMsgThread::SendQueryHostStatus()
{
	if (((CTradeResource *)m_pRes)->m_nArbMode != 1)
	{
		return;
	}
	if (!m_bIsQueryAns)
	{
		S_BPC_RSMSG rcvdata;
		rcvdata.sMsgBuf = (PBPCCOMMSTRU) m_pDrebApi->PoolMalloc();
		if (rcvdata.sMsgBuf== NULL)
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"PoolMalloc error ");
			return;
		}
		//�����ѯ���������ٲõ�״̬������״̬��ע��
		char errmsg[4096];
		bzero(errmsg,sizeof(errmsg));
		m_pSendXdp.ResetData();
		if (!m_pSendXdp.SetFieldValue("uint3",m_pRes->g_nSvrMainId,errmsg))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xdp SetFieldValue error %s",errmsg);
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			return;
		}
		if (!m_pSendXdp.SetFieldValue("uint4",(unsigned int)(m_pRes->g_nSvrPrivateId),errmsg))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xdp SetFieldValue error %s",errmsg);
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			return;
		}
		if (m_pLog->isWrite(LOG_DEBUG+1))
		{
			m_pSendXdp.PrintXdp(errmsg);
#ifdef _ENGLISH_
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"app query data: %s",errmsg);
#else
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"Ӧ�ò�ѯ����״̬���� %s",errmsg);
#endif
		}
		unsigned int datalen = BPUDATASIZE;
		if (!m_pSendXdp.ToBuffer(rcvdata.sMsgBuf->sBuffer,datalen,errmsg))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xdp ToBuffer error %s",errmsg);
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			return;
		}
		rcvdata.sMsgBuf->sDBHead.nLen = datalen;
		
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 0;
		rcvdata.sMsgBuf->sDBHead.cCmd = CMD_DPCALL;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo = APP_QUERY_HOST;
		m_bIsQueryAns = false;//��ѯӦ��Ϊfalse����һֱΪfalseʱҪ���·����ѯ����ֹ�ٲ����⵼�·���������
		m_pDrebApi->SendMsg(rcvdata);

	}
}

// ������: OnMsgReportBpc
// ���  : ������ 2015-4-23 16:01:43
// ����  : virtual void 
// ����  : S_BPC_RSMSG &rcvdata
// ����  : �����ϱ������Ϣ����������
void CDrebMsgThread::OnMsgReportBpc(S_BPC_RSMSG &rcvdata)
{
	//�����Ϣ
	CBF_Xml        pXml;
	
	if (!pXml.FromBuffer(rcvdata.sMsgBuf->sBuffer))
	{
		m_pDrebApi->GetLogPoint()->LogMp(LOG_ERROR,__FILE__,__LINE__,"API�����ķ�xml��ʽ");
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	//���汨�̻�������״̬
// 	<��չ��Ϣ name="�����ݲ�������">
//         <DATA status="0">���ӳ����������22 ����������21 �ͻ�������0 �ڴ滺����ܷ���100 δʹ��100 ÿ���С65534</DATA>
//         <DATA2 status="0">���ӳ����������22 ����������21 �ͻ�������0 �ڴ滺����ܷ���100 δʹ��100 ÿ���С65534</DATA2>
// 	</��չ��Ϣ>
	
	int ret;
	char tmpchar[200];
	char tmpchar2[200];

	pXml.SetNodeValueByPath("/Monitor/�ڵ�����",false,2);//��BPC

	pXml.SetNodeValueByPath("/Monitor/��չ��Ϣ",false,"",false);
	//ÿ��1�������ӵ�״̬��δ��������ȥ����

	sprintf(tmpchar,"/Monitor/��չ��Ϣ/%s/","tradekernel");
		

	switch (((CTradeResource *)m_pRes)->m_nSvrHostStatus)
	{
		case ARBSTATUS_UNKNOW:
			sprintf(tmpchar2,"����δ֪�������ٲ�δ���������粻ͨ");
			pXml.SetNodeValueByPath(tmpchar,false,tmpchar2,false);
			pXml.SetNodeAttribute(tmpchar,false,"status",false,1);
			break;
		case ARBSTATUS_MASTER:
		case ARBSTATUS_MASTER_AYN:
			sprintf(tmpchar2,"����״̬");
			pXml.SetNodeValueByPath(tmpchar,false,tmpchar2,false);
			pXml.SetNodeAttribute(tmpchar,false,"status",false,0);//״̬����
			break;
		case ARBSTATUS_BACKUP:
		case ARBSTATUS_BACKUP_AYN:
			sprintf(tmpchar2,"����״̬");
			pXml.SetNodeValueByPath(tmpchar,false,tmpchar2,false);
			pXml.SetNodeAttribute(tmpchar,false,"status",false,0);//״̬����
			break;
		case MASTERHOST:
			sprintf(tmpchar2,"������״̬");
			pXml.SetNodeValueByPath(tmpchar,false,tmpchar2,false);
			pXml.SetNodeAttribute(tmpchar,false,"status",false,0);//״̬����
			break;
		default: 
			printf("�ڵ�[%d %d]��ǰ����״̬Ϊ����״̬ %d\n",m_pRes->g_nSvrMainId,m_pRes->g_nSvrPrivateId,((CTradeResource *)m_pRes)->m_nSvrHostStatus);
			break;
			
	}

	int len = BPUDATASIZE;
	if (!pXml.ToBuffer(rcvdata.sMsgBuf->sBuffer,len))
	{
		m_pDrebApi->GetLogPoint()->LogMp(LOG_ERROR,__FILE__,__LINE__,"ToBuffer����");
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	rcvdata.sMsgBuf->sDBHead.nLen = len;
	m_pDrebApi->GetLogPoint()->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"���֪ͨ",rcvdata.sMsgBuf->sBuffer,rcvdata.sMsgBuf->sDBHead.nLen);
	//��������	
	rcvdata.sMsgBuf->sBpcHead.nIndex = 100;
	m_pDrebApi->SendMsg(rcvdata);
	return;
}

void CDrebMsgThread::OnHostProc()
{
	//����תΪ�����Ĵ������ӽ������ȵ�
}
