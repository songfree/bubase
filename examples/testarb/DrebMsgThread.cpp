// DrebMsgThread.cpp: implementation of the CDrebMsgThread class.
//
//////////////////////////////////////////////////////////////////////

#include "DrebMsgThread.h"
#include "ArbResource.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDrebMsgThread::CDrebMsgThread()
{
	m_bIsCheck = false;
	m_pLog = NULL;
}

CDrebMsgThread::~CDrebMsgThread()
{

}
bool CDrebMsgThread::Init(CBF_DrebServer   *bus,CBF_DrebResource *res)
{
	m_pDrebApi = bus;
	m_pRes = res;
	m_pLog = m_pDrebApi->GetLogPoint();
	m_pTime.Init(100,true);
	m_pTime.SetTimer(0,((CArbResource *)m_pRes)->m_nArbReportTime*1000,&CDrebMsgThread::OnTimer,this);
	m_pTime.SetTimer(1,5000,&CDrebMsgThread::OnTimer,this);
	m_pTime.SetTimer(2,5000,&CDrebMsgThread::OnTimer,this);
	

	char errmsg[200];
	bzero(errmsg,sizeof(errmsg));
	if (!m_pXdp.InitXdp(((CArbResource *)m_pRes)->m_sXdpDefineXml.c_str(),errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP���ĳ�ʼ������ %s",errmsg);
		return false;
	}
	if (!m_pSendXdp.XdpCopy(&m_pXdp))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP���ĳ�ʼ��XdpCopy���� %s",errmsg);
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
	while (m_bIsRunning)
	{
		if (m_pDrebApi->GetMsgData(m_rMsgData))
		{
			ProcessDrebData(m_rMsgData);
		}
	}
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
	m_pDrebApi->GetLogPoint()->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�յ�DREB���ӳɹ�����Ӧ");
	//�����ѯ���������ٲõ�״̬������״̬��ע��
	char errmsg[4096];
	bzero(errmsg,sizeof(errmsg));
	m_pXdp.ResetData();
	if (!m_pXdp.SetFieldValue("uint3",m_pRes->g_nSvrMainId,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xdp��������״̬���� %s",errmsg);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (!m_pXdp.SetFieldValue("uint4",(unsigned int)(m_pRes->g_nSvrPrivateId),errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xdp���÷�������� %s",errmsg);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (m_pLog->isWrite(LOG_DEBUG+1))
	{
		m_pXdp.PrintXdp(errmsg);
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"Ӧ�ò�ѯ����״̬���� %s",errmsg);
	}
	unsigned int datalen = BPUDATASIZE;
	if (!m_pXdp.ToBuffer(rcvdata.sMsgBuf->sBuffer,datalen,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xdp ToBuffer���� %s",errmsg);
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
	m_pDrebApi->GetLogPoint()->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�յ�DREB��Ӧ����Ӧ");
	if (rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo == APP_QUERY_HOST)
	{
		OnQueryArbAns(rcvdata);
		return;
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
		m_pDrebApi->GetLogPoint()->LogMp(LOG_ERROR,__FILE__,__LINE__,"�޴˹��� ��Ϣ[%s]  DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Դ[%d %d] cZip[%d] ҵ�����ݳ���[%d]",\
			GetBpcMsgType(rcvdata.sMsgBuf->sBpcHead.cMsgType).c_str(),\
			GetDrebCmdType(rcvdata.sMsgBuf->sDBHead.cCmd).c_str(),rcvdata.sMsgBuf->sDBHead.cNextFlag,\
			rcvdata.sMsgBuf->sDBHead.cRaflag,rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
			rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
			rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
			rcvdata.sMsgBuf->sDBHead.cZip,rcvdata.sMsgBuf->sDBHead.nLen);
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
	CArbResource *arbconf = (CArbResource *)m_pRes;
	if (arbconf->m_nSvrHostStatus == ARBSTATUS_UNKNOW)
	{
		//δ֪״̬����ȥ����
		return;
	}
	S_BPC_RSMSG data;
	data.sMsgBuf = (PBPCCOMMSTRU)m_pDrebApi->PoolMalloc();
	if (data.sMsgBuf == NULL)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"��������״̬֪ͨʧ��[%d %d] PoolMalloc error",m_pRes->g_nSvrMainId,m_pRes->g_nSvrPrivateId);
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
	if (!m_pSendXdp.SetFieldValue("uint9",((CArbResource *)m_pRes)->m_nSvrHostStatus,errmsg))
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
	m_pDrebApi->SendMsg(data); //�����ٲ�֪ͨ
}

void CDrebMsgThread::OnQueryArbAns(S_BPC_RSMSG &rcvdata)
{
	//��ѯ����״̬�ı���
	char errmsg[4096];
	bzero(errmsg,sizeof(errmsg));
	if (!m_pXdp.FromBuffer(rcvdata.sMsgBuf->sBuffer,rcvdata.sMsgBuf->sDBHead.nLen,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"arb���صı��Ĳ�������XDP���� %s",errmsg);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (m_pLog->isWrite(LOG_DEBUG+1))
	{
		m_pXdp.PrintXdp(errmsg);
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"arb����Ӧ�ò�ѯ���� %s",errmsg);
	}
	unsigned int status=0;
	unsigned int retcode=0;

	if (!m_pXdp.GetFieldValue("uint9",status,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"arb����Ӧ�ò�ѯ���Ĳ�������uint9�ֶ� %s",errmsg);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (!m_pXdp.GetFieldValue("uint10",retcode,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"arb����Ӧ�ò�ѯ���Ĳ�������uint10�ֶ� %s",errmsg);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}	
	if (retcode != SUCCESS)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"arb����Ӧ�ò�ѯ �ķ�����[%d]Ϊ���ɹ�",retcode);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"arb����Ӧ�ò�ѯ ��Ӧ��״̬[%d]",status);
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
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"arb����Ӧ�ò�ѯ ��Ӧ��״̬[%d]����",status);
	}
	m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
	rcvdata.sMsgBuf = NULL;
	m_bIsQueryAns = true;
	return;
	
}

void CDrebMsgThread::OnChangeHostStatus(unsigned int status)
{
	CArbResource *arbconf = (CArbResource *)m_pRes;
	if (status == ARBSTATUS_MASTER || status == ARBSTATUS_MASTER_AYN )
	{
		//Ϊ����
		if (arbconf->m_nSvrHostStatus != ARBSTATUS_MASTER && arbconf->m_nSvrHostStatus != ARBSTATUS_MASTER_AYN)
		{
			//תΪ����
			arbconf->m_nSvrHostStatus = ARBSTATUS_MASTER;
			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"������Ϊ����");
			//ע�ύ��
			for (int i=0; i< m_pRes->g_vDrebLinkInfo.size(); i++)
			{
				m_pDrebApi->RegisterDreb(i,&((CArbResource *)m_pRes)->g_lSvrTxList);
			}
		}
	}
	else if (status == ARBSTATUS_BACKUP || status == status == ARBSTATUS_BACKUP_AYN)
	{
		if (arbconf->m_nSvrHostStatus != ARBSTATUS_BACKUP && arbconf->m_nSvrHostStatus != ARBSTATUS_BACKUP_AYN)
		{
			//תΪ����
			arbconf->m_nSvrHostStatus = ARBSTATUS_BACKUP;
			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"������Ϊ����");
		}
	}
}

void CDrebMsgThread::OnSvrRequest(S_BPC_RSMSG &rcvdata)
{
	m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
	rcvdata.sMsgBuf = NULL;
	return;
}

void CDrebMsgThread::OnArbControl(S_BPC_RSMSG &rcvdata)
{
	char errmsg[4096];
	bzero(errmsg,sizeof(errmsg));
	if (!m_pXdp.FromBuffer(rcvdata.sMsgBuf->sBuffer,rcvdata.sMsgBuf->sDBHead.nLen,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ٲ�ָ�����Ĳ�������XDP���� %s",errmsg);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (m_pLog->isWrite(LOG_DEBUG+1))
	{
		m_pXdp.PrintXdp(errmsg);
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ٲ�ָ������ %s",errmsg);
	}
	unsigned int id=0;
	unsigned int pid=0;
	unsigned int status=0;
	
	if (!m_pXdp.GetFieldValue("uint3",id,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ٲ�ָ�����Ĳ��� ��uint3�ֶ� %s",errmsg);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	
	if (!m_pXdp.GetFieldValue("uint4",pid,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ٲ�ָ�����Ĳ��� ��uint4�ֶ� %s",errmsg);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	
	if (!m_pXdp.GetFieldValue("uint9",status,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ٲ�ָ�����Ĳ��� ��uint9�ֶ� %s",errmsg);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (id != m_pRes->g_nSvrMainId || pid != m_pRes->g_nSvrPrivateId)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ٲ�ָ�����Ĳ��� ����[%d %d] ���Ǳ�����",id,pid);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"����ָ����״̬Ϊ[%d]",status);
	//�ı�״̬
	OnChangeHostStatus(status);
	CArbResource *arbconf = (CArbResource *)m_pRes;
	if (!m_pXdp.SetFieldValue("uint9",arbconf->m_nSvrHostStatus,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP  SetFieldValue ����");
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (!m_pXdp.SetFieldValue("uint10",(unsigned int)SUCCESS,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP  SetFieldValue ����");
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (m_pLog->isWrite(LOG_DEBUG+1))
	{
		m_pXdp.PrintXdp(errmsg);
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ٲ�ָ������Ӧ���� %s",errmsg);
	}
	unsigned int datalen = BPUDATASIZE;
	if (!m_pXdp.ToBuffer(rcvdata.sMsgBuf->sBuffer,datalen,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xdp ToBuffer���� %s",errmsg);
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
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ٲò�ѯ���Ĳ�������XDP���� %s",errmsg);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (m_pLog->isWrite(LOG_DEBUG+1))
	{
		m_pXdp.PrintXdp(errmsg);
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ٲò�ѯ���� %s",errmsg);
	}
	unsigned int id=0;
	unsigned int pid=0;
	unsigned int status=0;
	
	if (!m_pXdp.GetFieldValue("uint3",id,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ٲò�ѯ���Ĳ��� ��uint3�ֶ� %s",errmsg);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	
	if (!m_pXdp.GetFieldValue("uint4",pid,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ٲò�ѯ���Ĳ��� ��uint4�ֶ� %s",errmsg);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	
	
	if (id != m_pRes->g_nSvrMainId || pid != m_pRes->g_nSvrPrivateId)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ٲò�ѯ���Ĳ��� ����[%d %d] ���Ǳ�����",id,pid);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	
	CArbResource *arbconf = (CArbResource *)m_pRes;
	if (!m_pXdp.SetFieldValue("uint9",arbconf->m_nSvrHostStatus,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP  SetFieldValue ����");
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (!m_pXdp.SetFieldValue("uint10",(unsigned int)SUCCESS,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP  SetFieldValue ����");
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (m_pLog->isWrite(LOG_DEBUG+1))
	{
		m_pXdp.PrintXdp(errmsg);
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ٲò�ѯӦ���� %s",errmsg);
	}
	unsigned int datalen = BPUDATASIZE;
	if (!m_pXdp.ToBuffer(rcvdata.sMsgBuf->sBuffer,datalen,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xdp ToBuffer���� %s",errmsg);
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
	switch (((CArbResource *)m_pRes)->m_nSvrHostStatus)
	{
		case ARBSTATUS_UNKNOW:
			printf("������״̬Ϊ [δ֪]\n");

			break;
		case ARBSTATUS_MASTER:
			printf("������״̬Ϊ [����]\n");
			break;
		case ARBSTATUS_MASTER_AYN:
			printf("������״̬Ϊ [����ͬ��]\n");
			break;
		case ARBSTATUS_BACKUP:
			printf("������״̬Ϊ [����]\n");
			break;
		case ARBSTATUS_BACKUP_AYN:
			printf("������״̬Ϊ [����ͬ��]\n");
			break;
		default: 
			printf("������״̬Ϊ����״̬ %d\n",((CArbResource *)m_pRes)->m_nSvrHostStatus);
			break;

	}
	
}

void CDrebMsgThread::SendQueryHostStatus()
{
	if (!m_bIsQueryAns)
	{
		S_BPC_RSMSG rcvdata;
		rcvdata.sMsgBuf = (PBPCCOMMSTRU) m_pDrebApi->PoolMalloc();
		if (rcvdata.sMsgBuf== NULL)
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"PoolMalloc���� ");
			return;
		}
		//�����ѯ���������ٲõ�״̬������״̬��ע��
		char errmsg[4096];
		bzero(errmsg,sizeof(errmsg));
		m_pSendXdp.ResetData();
		if (!m_pSendXdp.SetFieldValue("uint3",m_pRes->g_nSvrMainId,errmsg))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xdp��������״̬���� %s",errmsg);
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			return;
		}
		if (!m_pSendXdp.SetFieldValue("uint4",(unsigned int)(m_pRes->g_nSvrPrivateId),errmsg))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xdp���÷�������� %s",errmsg);
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			return;
		}
		if (m_pLog->isWrite(LOG_DEBUG+1))
		{
			m_pSendXdp.PrintXdp(errmsg);
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"Ӧ�ò�ѯ����״̬���� %s",errmsg);
		}
		unsigned int datalen = BPUDATASIZE;
		if (!m_pSendXdp.ToBuffer(rcvdata.sMsgBuf->sBuffer,datalen,errmsg))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xdp ToBuffer���� %s",errmsg);
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
