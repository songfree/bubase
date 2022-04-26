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
bool CDrebMsgThread::Init(CIBF_DrebServer* bus,CBF_DrebResource *res)
{
	m_pDrebApi = bus;
	m_pRes = res;
	m_tArb.m_pLog = m_pDrebApi->GetLogPoint();
	m_pLog = m_tArb.m_pLog;
	m_pTime.Init(100,true);
	m_pTime.SetTimer(0,1000,&CDrebMsgThread::OnTimer,this);
	char errmsg[200];
	bzero(errmsg,sizeof(errmsg));
	if (!m_pXdp.InitXdp(((CArbResource *)m_pRes)->m_sXdpDefineXml.c_str(),errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP���ĳ�ʼ������ %s",errmsg);
		return false;
	}
	return true;
}
int CDrebMsgThread::OnTimer(unsigned int event, void *p)
{
	CDrebMsgThread *pp = (CDrebMsgThread *)p;
	if (event == 0)
	{
		//�ü��Ӧ��״̬���Խ����л�
		pp->OnCheck();
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
		if (m_bIsCheck)
		{
			//���Ӧ�õ�״̬�����л����л�
			CheckArb();
			m_bIsCheck = false;
		}
	}
	return 0;
}
// ������: ProcessDrebData
// ���  : ������ 2015-4-23 15:53:34
// ����  : virtual void 
// ����  : S_BPC_RSMSG &rcvdata
// ����  : ��������߽��չ���������
void CDrebMsgThread::ProcessDrebData(S_BPC_RSMSG &rcvdata)
{
	if (rcvdata.sMsgBuf->sDBHead.cZip != 0)
	{
		if (!m_pDrebApi->UnzipBuf(*rcvdata.sMsgBuf))
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"���ݽ�ѹ������");
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pDrebApi->SendMsg(rcvdata);
			return ;
		}
	}
	switch (rcvdata.sMsgBuf->sBpcHead.cMsgType)
	{
		case MSG_GETBPCNEXT://��bpc��ȡ������
			OnGetBpcNextPack(rcvdata);
			break;
		case MSG_GETNEXT://��BPU��ȡ��������Ҫ����BPU����
			OnGetBpuNextPack(rcvdata);
			break;
		case MSG_REQ://DREB���յ�������
			OnMsgRequest(rcvdata);
			break;
		case MSG_MONITOR_SAP://DREB���յ��ļ��ָ��
			OnMsgMonitor(rcvdata);
			break;
		case MSG_CONNECTDREB://DREB�������ӳɹ����������������ע�ύ�׵�
			OnMsgConnectBack(rcvdata);
			break;
		case MSG_DREBANS://DREB��������ȵ�Ӧ��
			OnMsgDrebAns(rcvdata);
			break;	
		case MSG_BPCMONITOR://�����ϱ������Ϣ
			OnMsgReportBpc(rcvdata);
			break;	
		default:
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			break;
	}
	return;
}

// ������: OnGetBpcNextPack
// ���  : ������ 2015-4-23 15:53:52
// ����  : virtual void 
// ����  : S_BPC_RSMSG &rcvdata
// ����  : ȡ���صĺ�����,(���������ڱ���)
void CDrebMsgThread::OnGetBpcNextPack(S_BPC_RSMSG &rcvdata)
{
	m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
	rcvdata.sMsgBuf = NULL;
}

// ������: OnGetBpuNextPack
// ���  : ������ 2015-4-23 15:54:14
// ����  : virtual void 
// ����  : S_BPC_RSMSG &rcvdata
// ����  : ȡBPU�ϵĺ�������Ҫ����BPU����
void CDrebMsgThread::OnGetBpuNextPack(S_BPC_RSMSG &rcvdata)
{
	m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
	rcvdata.sMsgBuf = NULL;
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
	//���ݲ�ͬ����Ϣ���ͽ��д���
	if (rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo != APP_QUERY_HOST && rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo != APP_PUSH_STATUS)
	{
// 		m_pDrebApi->GetLogPoint()->LogMp(LOG_ERROR,__FILE__,__LINE__,"�����ٲý��� ��Ϣ[%s]  DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Դ[%d %d] cZip[%d] ҵ�����ݳ���[%d]",\
// 			GetBpcMsgType(rcvdata.sMsgBuf->sBpcHead.cMsgType).c_str(),\
// 			GetDrebCmdType(rcvdata.sMsgBuf->sDBHead.cCmd).c_str(),rcvdata.sMsgBuf->sDBHead.cNextFlag,\
// 			rcvdata.sMsgBuf->sDBHead.cRaflag,rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
// 			rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
// 			rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
// 			rcvdata.sMsgBuf->sDBHead.cZip,rcvdata.sMsgBuf->sDBHead.nLen);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	switch (rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo)
	{
		case APP_QUERY_HOST:
			OnAppQueryHost(rcvdata);//Ӧ�ò�ѯ����
			break;
		case APP_PUSH_STATUS: //Ӧ������״̬֪ͨ
			OnAppPushStatus(rcvdata);
			break;
		default:
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			break;
	}

	
}

// ������: OnMsgMonitor
// ���  : ������ 2015-4-23 15:54:57
// ����  : virtual void 
// ����  : S_BPC_RSMSG &rcvdata
// ����  : �������߹����ļ��ָ��
void CDrebMsgThread::OnMsgMonitor(S_BPC_RSMSG &rcvdata)
{
	//DREB�����ĵ���
	switch (rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo)
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
		case BPCMONI_SETBPU:
			MonitorSetBpu(rcvdata);
			break;
		case BPCMONI_SETMODE:
			MonitorSetMode(rcvdata);
			break;
		case BPCMONI_STOPBPU:
			MonitorStopBpu(rcvdata);
			break;
		case BPCMONI_STARTBPU:
			MonitorStartBpu(rcvdata);
			break;
		case BPCMONI_DOWNLOAD:
			Download(rcvdata);
			break;
		case BPCMONI_UPLOAD:
			Upload(rcvdata);
			break;
		default:
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_FUNCNOTFUND;
			rcvdata.sMsgBuf->sDBHead.nLen = 0;
			rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
			m_pDrebApi->SendMsg(rcvdata);
			break;
	}
	return;
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
	//ע��arb����
	m_pDrebApi->RegisterDreb(rcvdata.sMsgBuf->sBpcHead.nIndex,&(((CArbResource *)m_pRes)->g_lArbTxList));
	m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
	rcvdata.sMsgBuf = NULL;
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
	char errmsg[4096];
	bzero(errmsg,sizeof(errmsg));
	if (rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo == ARB_SEND_QUERY)
	{
		if (rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode != SUCCESS)
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"��ѯӦ��[%d %d]״̬ ���߷��ش� a_nRetCode[%d]",\
				rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nSvrMainId,rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_cSvrPrivateId,\
				rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode);
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			return;
		}
		if (!m_pXdp.FromBuffer(rcvdata.sMsgBuf->sBuffer,rcvdata.sMsgBuf->sDBHead.nLen,errmsg))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"Ӧ�÷��ر��Ĳ�������XDP���� %s",errmsg);
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			return;
		}
		if (m_pLog->isWrite(LOG_DEBUG+1))
		{
			m_pXdp.PrintXdp(errmsg);
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"Ӧ�÷���״̬��ѯ���� %s",errmsg);
		}
		unsigned int id=0;
		unsigned pid=0;
		unsigned status=0;
		unsigned int retcode=0;
		
		if (!m_pXdp.GetFieldValue("uint3",id,errmsg))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"Ӧ�÷���״̬��ѯ���Ĳ�������uint3�ֶ� %s",errmsg);
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			return;
		}
		
		if (!m_pXdp.GetFieldValue("uint4",pid,errmsg))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"Ӧ�÷���״̬��ѯ���Ĳ�������uint4�ֶ� %s",errmsg);
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			return;
		}
		
		if (!m_pXdp.GetFieldValue("uint9",status,errmsg))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"Ӧ�÷���״̬��ѯ���Ĳ�������uint9�ֶ� %s",errmsg);
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			return;
		}
		if (!m_pXdp.GetFieldValue("uint10",retcode,errmsg))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"Ӧ�÷���״̬��ѯ���Ĳ�������uint10�ֶ� %s",errmsg);
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			return;
		}
		
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"Ӧ�÷���״̬ app[%d %d] status[%d]",id,pid,status);
		if (status == ARBSTATUS_UNKNOW)
		{
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			return;
		}
		ARB_S_APPINFO *appinfo = m_tArb.UpdateArb(id,pid,status,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId);
		if (appinfo == NULL)
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"UpdateArb����");
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			return;
		}
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo == ARB_SEND_CONTROL)
	{
		if (rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode != SUCCESS)
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"������������ Ӧ��[%d %d] ���߷��ش� a_nRetCode[%d]",\
				rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nSvrMainId,rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_cSvrPrivateId,\
				rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode);
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			return;
		}
		if (!m_pXdp.FromBuffer(rcvdata.sMsgBuf->sBuffer,rcvdata.sMsgBuf->sDBHead.nLen,errmsg))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"Ӧ�÷��ر��Ĳ�������XDP���� %s",errmsg);
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			return;
		}
		if (m_pLog->isWrite(LOG_DEBUG+1))
		{
			m_pXdp.PrintXdp(errmsg);
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"Ӧ�÷���ָ���������� %s",errmsg);
		}
		unsigned int id=0;
		unsigned pid=0;
		unsigned status=0;
		unsigned int retcode=0;
		
		if (!m_pXdp.GetFieldValue("uint3",id,errmsg))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"Ӧ�÷���ָ���������Ĳ�������uint3�ֶ� %s",errmsg);
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			return;
		}
		
		if (!m_pXdp.GetFieldValue("uint4",pid,errmsg))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"Ӧ�÷���ָ���������Ĳ�������uint4�ֶ� %s",errmsg);
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			return;
		}
		
		if (!m_pXdp.GetFieldValue("uint9",status,errmsg))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"Ӧ�÷���ָ���������Ĳ�������uint9�ֶ� %s",errmsg);
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			return;
		}
		if (!m_pXdp.GetFieldValue("uint10",retcode,errmsg))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"Ӧ�÷���ָ���������Ĳ�������uint10�ֶ� %s",errmsg);
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			return;
		}
		
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"Ӧ�÷���״̬ app[%d %d] status[%d]",id,pid,status);
		ARB_S_APPINFO *appinfo = m_tArb.UpdateArb(id,pid,status,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId);
		if (appinfo == NULL)
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"UpdateArb����");
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			return;
		}
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}

	m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
	rcvdata.sMsgBuf = NULL;
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
	//��Ӧ����Ϣ������������
	CXmlNode *arbnode =  pXml.AddNodeByPath("Monitor",false,"ARB��Ϣ",false,"");
	
	if (arbnode != NULL)
	{
		std::vector<ARB_S_APPINFO> applist;
		m_tArb.GetAppList(applist);
		char tmpchar [500];
		for (int i=0; i< applist.size(); i++)
		{
			bzero(tmpchar,sizeof(tmpchar));
			sprintf(tmpchar,"app[%d %d] status[%d] uptime[%d]",applist[i].nSvrMainId,applist[i].cSvrPrivateId,applist[i].cStatus,\
				applist[i].nUpdateTime);
			CXmlNode *tmpnode = arbnode->AddChildNode("ARBLIST");
			if (tmpnode != NULL)
			{
				tmpnode->SetNodeValue(tmpchar);
			}
		}
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
	
void CDrebMsgThread::MonitorInfo(S_BPC_RSMSG &rcvdata)
{
	int i;
	CBF_Xml xmlpack;
	xmlpack.SetNodeValueByPath("Monitor/BPC�����ڵ�",false,(int)m_pRes->g_nSvrMainId);
	xmlpack.SetNodeValueByPath("Monitor/BPC˽�нڵ�",false,(int)m_pRes->g_nSvrPrivateId);
	xmlpack.SetNodeValueByPath("Monitor/����ʱ��",false,m_pRes->g_sStartDateTime,false);
	char sip[40];
	char drebstatus[40];
	int  drebport;
	int  drebid;
	int  drebprivateid;
	
	//������Ϣ
	for (i=0 ; i < m_pRes->g_vDrebLinkInfo.size() ; i++)
	{
		CXmlNode *connnode = xmlpack.AddNodeByPath("Monitor/DREB������Ϣ",false,"����",false,"");
		if (connnode == NULL)
		{
			m_pDrebApi->GetLogPoint()->LogMp(LOG_ERROR,__FILE__,__LINE__,"AddNodeByPath ����");
			continue;
		}
		bzero(sip,sizeof(sip));
		bzero(drebstatus,sizeof(drebstatus));
		m_pDrebApi->GetDrebInfo(i,sip,drebport,drebid,drebprivateid,drebstatus);
		connnode->SetAttribute("ip",false,sip);
		connnode->SetAttribute("port",false,drebport);
		connnode->SetAttribute("index",false,i);
		connnode->SetAttribute("�����ڵ�",false,drebid);
		connnode->SetAttribute("˽�нڵ�",false,drebprivateid);
		connnode->SetAttribute("status",false,drebstatus);
		
	}

	int len = BPUDATASIZE;
	if (!xmlpack.ToBuffer(rcvdata.sMsgBuf->sBuffer,len))
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"���岻�㣬������bpcinfo.txt�鿴");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		m_pDrebApi->GetLogPoint()->LogMp(LOG_ERROR,__FILE__,__LINE__,"MonitorInfo ���岻��");
		xmlpack.ToFile("bpcinfo.txt");
	}
	else
	{
		rcvdata.sMsgBuf->sDBHead.nLen = len;
	}
	
	rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
	rcvdata.sMsgBuf->sDBHead.cZip = 0;
	m_pDrebApi->SendMsg(rcvdata);
}
	
void CDrebMsgThread::MonitorHost(S_BPC_RSMSG &rcvdata)
{
	CBF_Xml xmlpack;
	xmlpack.SetNodeValueByPath("Monitor/BPC�����ڵ�",false,(int)m_pRes->g_nSvrMainId);
	xmlpack.SetNodeValueByPath("Monitor/BPC˽�нڵ�",false,(int)m_pRes->g_nSvrPrivateId);
	xmlpack.SetNodeValueByPath("Monitor/����ʱ��",false,m_pRes->g_sStartDateTime,false);
	
	int total;
	int unused;
	int bufsize;
	m_pDrebApi->GetBufferPoolInfo(total,unused,bufsize);
	xmlpack.SetNodeAttribute("Monitor/�ڴ滺���",false,"�ܷ���",false,total );
	xmlpack.SetNodeAttribute("Monitor/�ڴ滺���",false,"δʹ��",false,unused );
	xmlpack.SetNodeAttribute("Monitor/�ڴ滺���",false,"ÿ���С",false,bufsize );
	//ȡ������Ϣ
	S_MONITOR_HOST hf;
	vector<S_MONITOR_DISK> di;
	m_pDrebApi->GetHostInfo(hf,di);
	
	xmlpack.SetNodeValueByPath("Monitor/������Դ/CPU",false,(int)hf.nCpuRate);
	
	
	xmlpack.SetNodeValueByPath("Monitor/������Դ/�ڴ�",false,"",false);
	xmlpack.SetNodeAttribute("Monitor/������Դ/�ڴ�",false,"�ܴ�СM",false,(int)hf.nTotalMemory);
	xmlpack.SetNodeAttribute("Monitor/������Դ/�ڴ�",false,"ʹ��M",false,(int)hf.nUsedMemory);
	
	for (int i=0 ; i<di.size() ; i++)
	{
		CXmlNode *disknode = xmlpack.AddNodeByPath("Monitor/������Դ/�����б�",false,"����",false,"");
		if (disknode != NULL)
		{
			disknode->SetAttribute("����",false,di[i].sDiskName);
			disknode->SetAttribute("�ܴ�СM",false,(int)di[i].nTotalSpace);
			disknode->SetAttribute("ʹ�ô�СM",false,(int)di[i].nUsedSpace);
		}
	}
	
	int len = BPUDATASIZE;
	if (!xmlpack.ToBuffer(rcvdata.sMsgBuf->sBuffer,len))
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"���岻�㣬������bpchost.txt�鿴");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		m_pDrebApi->GetLogPoint()->LogMp(LOG_ERROR,__FILE__,__LINE__,"MonitorHost ���岻��");
		xmlpack.ToFile("bpchost.txt");
	}
	else
	{
		rcvdata.sMsgBuf->sDBHead.nLen = len;
	}
	
	rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
	rcvdata.sMsgBuf->sDBHead.cZip = 0;
	m_pDrebApi->SendMsg(rcvdata);
}

void CDrebMsgThread::MonitorBpu(S_BPC_RSMSG &rcvdata)
{
	
	int connNum=0;
	CBF_Xml xmlpack;
	xmlpack.SetNodeValueByPath("Monitor/BPC�����ڵ�",false,(int)m_pRes->g_nSvrMainId);
	xmlpack.SetNodeValueByPath("Monitor/BPC˽�нڵ�",false,(int)m_pRes->g_nSvrPrivateId);
	xmlpack.SetNodeValueByPath("Monitor/����ʱ��",false,m_pRes->g_sStartDateTime,false);
	//BPU��Ϣ
	
	CXmlNode *arbnode =  xmlpack.AddNodeByPath("Monitor",false,"ARB��Ϣ",false,"");
	
	if (arbnode != NULL)
	{
		std::vector<ARB_S_APPINFO> applist;
		m_tArb.GetAppList(applist);
		char tmpchar [500];
		for (int i=0; i< applist.size(); i++)
		{
			bzero(tmpchar,sizeof(tmpchar));
			sprintf(tmpchar,"app[%d %d] status[%d] uptime[%d]",applist[i].nSvrMainId,applist[i].cSvrPrivateId,applist[i].cStatus,\
				applist[i].nUpdateTime);
			CXmlNode *tmpnode = arbnode->AddChildNode("ARBLIST");
			if (tmpnode != NULL)
			{
				tmpnode->SetNodeValue(tmpchar);
			}
		}
	}

	
	int len = BPUDATASIZE;
	if (!xmlpack.ToBuffer(rcvdata.sMsgBuf->sBuffer,len))
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"���岻�㣬������bpcbpu.txt�鿴");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		m_pDrebApi->GetLogPoint()->LogMp(LOG_ERROR,__FILE__,__LINE__,"MonitorBpu ���岻��");
		xmlpack.ToFile("bpcbpu.txt");
	}
	else
	{
		rcvdata.sMsgBuf->sDBHead.nLen = len;
	}
	
	rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
	rcvdata.sMsgBuf->sDBHead.cZip = 0;
	m_pDrebApi->SendMsg(rcvdata);
}

void CDrebMsgThread::MonitorReport(S_BPC_RSMSG &rcvdata)
{
	CBF_Xml xmlpack;
	xmlpack.SetNodeValueByPath("Monitor/BPC�����ڵ�",false,(int)m_pRes->g_nSvrMainId);
	xmlpack.SetNodeValueByPath("Monitor/BPC˽�нڵ�",false,(int)m_pRes->g_nSvrPrivateId);
	xmlpack.SetNodeValueByPath("Monitor/����ʱ��",false,m_pRes->g_sStartDateTime,false);
	char tmpchar[100];
	char nodename[200];


	int len = BPUDATASIZE;
	if (!xmlpack.ToBuffer(rcvdata.sMsgBuf->sBuffer,len))
	{
		sprintf(rcvdata.sMsgBuf->sBuffer,"���岻�㣬������bpureport.txt�鿴");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		m_pDrebApi->GetLogPoint()->LogMp(LOG_ERROR,__FILE__,__LINE__,"MonitorReport ���岻��");
		xmlpack.ToFile("bpureport.txt");
	}
	else
	{
		rcvdata.sMsgBuf->sDBHead.nLen = len;
	}
	rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
	rcvdata.sMsgBuf->sDBHead.cZip = 0;
	m_pDrebApi->SendMsg(rcvdata);
}

void CDrebMsgThread::OnAppQueryHost(S_BPC_RSMSG &rcvdata)
{
	char errmsg[4096];
	bzero(errmsg,sizeof(errmsg));
	if (!m_pXdp.FromBuffer(rcvdata.sMsgBuf->sBuffer,rcvdata.sMsgBuf->sDBHead.nLen,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"Ӧ�ò�ѯ����״̬���Ĳ�������XDP���� %s",errmsg);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (m_pLog->isWrite(LOG_DEBUG+1))
	{
		m_pXdp.PrintXdp(errmsg);
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"Ӧ�ò�ѯ����״̬���� %s",errmsg);
	}
	unsigned int id;
	unsigned int pid;
	
	if (!m_pXdp.GetFieldValue("uint3",id,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"Ӧ�ò�ѯ����״̬���Ĳ�������uint3�ֶ� %s",errmsg);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	
	if (!m_pXdp.GetFieldValue("uint4",pid,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"Ӧ�ò�ѯ����״̬���Ĳ�������uint4�ֶ� %s",errmsg);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	
	m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"Ӧ�ò�ѯ����״̬ app[%d %d]",id,pid);

	ARB_S_APPINFO *appinfo = m_tArb.UpdateArb(id,pid,ARBSTATUS_UNKNOW,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId);
	if (appinfo == NULL)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"UpdateArb����");
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (!m_pXdp.SetFieldValue("uint9",(unsigned int)(appinfo->cStatus),errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xdp��������״̬���� %s",errmsg);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (!m_pXdp.SetFieldValue("uint10",(unsigned int)SUCCESS,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"xdp���÷�������� %s",errmsg);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (m_pLog->isWrite(LOG_DEBUG+1))
	{
		m_pXdp.PrintXdp(errmsg);
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"Ӧ�ò�ѯ����״̬Ӧ���� %s",errmsg);
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

void CDrebMsgThread::OnAppPushStatus(S_BPC_RSMSG &rcvdata)
{
	char errmsg[4096];
	bzero(errmsg,sizeof(errmsg));
	if (!m_pXdp.FromBuffer(rcvdata.sMsgBuf->sBuffer,rcvdata.sMsgBuf->sDBHead.nLen,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"Ӧ������״̬���Ĳ�������XDP���� %s",errmsg);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	if (m_pLog->isWrite(LOG_DEBUG+1))
	{
		m_pXdp.PrintXdp(errmsg);
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"Ӧ������״̬���� %s",errmsg);
	}
	unsigned int id=0;
	unsigned int pid=0;
	unsigned int status=0;
	
	if (!m_pXdp.GetFieldValue("uint3",id,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"Ӧ������״̬���Ĳ�������uint3�ֶ� %s",errmsg);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	
	if (!m_pXdp.GetFieldValue("uint4",pid,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"Ӧ������״̬���Ĳ�������uint4�ֶ� %s",errmsg);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	
	if (!m_pXdp.GetFieldValue("uint9",status,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"Ӧ������״̬���Ĳ�������uint9�ֶ� %s",errmsg);
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	

	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"Ӧ������״̬ app[%d %d] status[%d]",id,pid,status);
	ARB_S_APPINFO *appinfo = m_tArb.UpdateArb(id,pid,status,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId);
	if (appinfo == NULL)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"UpdateArb����");
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return;
	}
	m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
	rcvdata.sMsgBuf = NULL;
	return;
	
}

void CDrebMsgThread::OnCheck()
{
	m_bIsCheck = true;	
}

void CDrebMsgThread::CheckArb()
{
	//ȡ���е�Ӧ�ã���ÿ��Ӧ�ý���ɨ��
	std::vector<int> applist;
	if (!m_tArb.GetDistinctApp(applist))
	{
		m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"GetDistinctApp���� ��Ӧ������");
		return ;
	}
	std::vector<ARB_S_APPINFO *> appinfo;
	int i;
	
	for (i=0; i<applist.size(); i++)
	{
		appinfo.clear();
		if (!m_tArb.GetAppInfo(applist[i],appinfo))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"û��[%d]��Ӧ����Ϣ",applist[i]);
			continue;
		}
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"��� Ӧ��[%d]������״̬  Ӧ����[%d]",applist[i],appinfo.size());
		CheckArbByApp(appinfo);
	}

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

void CDrebMsgThread::CheckArbByApp(std::vector<ARB_S_APPINFO *> appinfo)
{
	if (appinfo.size()<1)
	{
		return;
	}
	if (time(NULL) - appinfo[0]->nLastSwtichTime <((CArbResource *)m_pRes)->m_nSwitchSeg)
	{
		//���л������ü����
		return;
	}
	bool isSwitch=false;//�Ƿ������л�
	bool isMaster=false;//�Ƿ�������
	bool isBackup=false;//�Ƿ��б���
	int  nMasterId=-1;//������ID
	int  nBackupId=-1;//������ID  ��Ҫ��Ϊ���ı���ID
	//������ʱ�� �Ƿ�Ҫ����ѯ
	
	unsigned int curtime = time(NULL);
	unsigned int utime=curtime;
	for (int j=0 ; j<appinfo.size() ; j++)
	{
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"APP[%d %d]״̬[%d]",\
			appinfo[j]->nSvrMainId,appinfo[j]->cSvrPrivateId,appinfo[j]->cStatus);
		if (appinfo[j]->cStatus == ARBSTATUS_MASTER || appinfo[j]->cStatus == ARBSTATUS_MASTER_AYN)
		{
			if (nMasterId >=0)
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"APP[%d]�ж������ [%d %d] [%d %d]",\
					appinfo[nMasterId]->nSvrMainId,appinfo[nMasterId]->nSvrMainId,appinfo[nMasterId]->cSvrPrivateId,\
					appinfo[j]->nSvrMainId,appinfo[j]->cSvrPrivateId);
				m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"APP[%d]�ж������,��[%d %d] ��Ϊ����",appinfo[j]->nSvrMainId,appinfo[j]->cSvrPrivateId);
				SwitchHost(appinfo[j]->nNodeId,appinfo[j]->nSvrMainId,appinfo[j]->cSvrPrivateId,0);
				SetSwitchTime(appinfo);
				continue ;
			}
			//����
			isMaster = true;
			nMasterId = j;
			//���������������ʱ��
			if (curtime - appinfo[j]->nUpdateTime > ((CArbResource *)m_pRes)->m_nSwitchTime)
			{
				isSwitch = true; //��Ҫ�л�
				//��ѭ�����ҵ����¸���ʱ��ı����������л�
			}
			else  //�������л�
			{
				if (((CArbResource *)m_pRes)->m_nSwitchTime>2)
				{
					if (curtime - appinfo[j]->nUpdateTime > ((CArbResource *)m_pRes)->m_nSwitchTime-2)
					{
						//��Ҫ���Ͳ�ѯ��
						appinfo[j]->nQueryStatusTime = curtime;
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"APP [%d %d] ��ʱ���в�ѯ",\
							appinfo[j]->nSvrMainId,appinfo[j]->cSvrPrivateId);
						SendQueryHostStatus(appinfo[j]->nSvrMainId,appinfo[j]->cSvrPrivateId);
						
					}
				}
				//����ѭ��
				continue ;
			}
			
		}
		if (appinfo[j]->cStatus != ARBSTATUS_MASTER && appinfo[j]->cStatus != ARBSTATUS_MASTER_AYN)
		{
			
			//��2����δ���£���ɾ��
			if (curtime - appinfo[j]->nUpdateTime > 60*2)
			{
				m_tArb.DeleteApp(appinfo[j]->nSvrMainId,appinfo[j]->cSvrPrivateId);
			}
			else
			{
				if (appinfo[j]->cStatus == ARBSTATUS_UNKNOW)
				{
					continue;
				}
				//ǰ��û�б�������Ϊ��
				if (!isBackup)
				{
					isBackup = true;
					nBackupId = j;
					utime = appinfo[j]->nAddTime;
				}
				//�ҳ�����ʱ����m_nSwitchTime-2֮�ڲ��Ҽ���ʱ�������һ��
				if (curtime - appinfo[j]->nUpdateTime < ((CArbResource *)m_pRes)->m_nSwitchTime-2)
				{
					//������һ�������ļ���ʱ����磬����Ϊ�˱���
					if (utime > appinfo[j]->nAddTime)
					{
						utime = appinfo[j]->nAddTime;
						nBackupId = j;
					}
				}
			}
			
		}
	}
	
	if (isSwitch) //���л�
	{
		if (isBackup)//�б���
		{
			appinfo[nMasterId]->cStatus = ARBSTATUS_UNKNOW;
			appinfo[nBackupId]->cStatus = ARBSTATUS_MASTER;
			m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"����[%d %d]��ΪARBSTATUS_UNKNOW ����[%d %d]��Ϊ��",\
				appinfo[nMasterId]->nSvrMainId,appinfo[nMasterId]->cSvrPrivateId,\
				appinfo[nBackupId]->nSvrMainId,appinfo[nBackupId]->cSvrPrivateId);
			m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"���������л� [%d %d->%d]",\
				appinfo[nMasterId]->nSvrMainId,appinfo[nMasterId]->cSvrPrivateId,appinfo[nBackupId]->cSvrPrivateId);
			SwitchHost(appinfo[nMasterId]->nNodeId,appinfo[nMasterId]->nSvrMainId,appinfo[nMasterId]->cSvrPrivateId,appinfo[nBackupId]->cSvrPrivateId);
			SetSwitchTime(appinfo);
		}
		else
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"APP[%d] ����[%d %d]Ҫ�����л�ΪARBSTATUS_UNKNOW�����ޱ��������л�",\
				appinfo[nMasterId]->nSvrMainId,appinfo[nMasterId]->nSvrMainId,appinfo[nMasterId]->cSvrPrivateId);
			appinfo[nMasterId]->cStatus = ARBSTATUS_UNKNOW;//��Ϊδ֪
			SwitchHost(appinfo[nMasterId]->nNodeId,appinfo[nMasterId]->nSvrMainId,appinfo[nMasterId]->cSvrPrivateId,0);
			SetSwitchTime(appinfo);
			return ;
		}
	}
	else
	{
		//û������
		if (!isMaster)
		{
			if (isBackup) //�б���
			{
				appinfo[nBackupId]->cStatus = ARBSTATUS_MASTER;
				m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"û���������л� [%d %d]Ϊ����",\
					appinfo[nBackupId]->nSvrMainId,appinfo[nBackupId]->cSvrPrivateId);
				SwitchHost(appinfo[nBackupId]->nNodeId,appinfo[nBackupId]->nSvrMainId,0,appinfo[nBackupId]->cSvrPrivateId);
				SetSwitchTime(appinfo);
			}
			
		}
	}
}

void CDrebMsgThread::SendQueryHostStatus(unsigned int svr, char pid)
{
	S_BPC_RSMSG data;
	data.sMsgBuf = (PBPCCOMMSTRU)m_pDrebApi->PoolMalloc();
	if (data.sMsgBuf == NULL)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"��������״̬��ѯʧ�� PoolMalloc error");
		return;
	}
	char errmsg[250];
	bzero(errmsg,sizeof(errmsg));
	bzero(data.sMsgBuf,sizeof(BPCCOMMSTRU));
	data.sMsgBuf->sBpcHead.nIndex = 100;
	data.sMsgBuf->sDBHead.cCmd = CMD_DPCALL;
	data.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo = ARB_SEND_QUERY;
	data.sMsgBuf->sDBHead.d_Dinfo.d_nSvrMainId = svr;
	data.sMsgBuf->sDBHead.d_Dinfo.d_cSvrPrivateId = pid;
	m_pXdp.ResetData();
	if (!m_pXdp.SetFieldValue("uint3",svr,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP SetFieldValue uint3 error: %s",errmsg);
		m_pDrebApi->PoolFree(data.sMsgBuf);
		return;
	}
	if (!m_pXdp.SetFieldValue("uint4",(unsigned int)pid,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP SetFieldValue uint4 error: %s",errmsg);
		m_pDrebApi->PoolFree(data.sMsgBuf);
		return;
	}
	unsigned int len = BPUDATASIZE;
	if (!m_pXdp.ToBuffer(data.sMsgBuf->sBuffer,len,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP ToBuffer error: %s",errmsg);
		m_pDrebApi->PoolFree(data.sMsgBuf);
		return;
	}
	data.sMsgBuf->sDBHead.nLen = len;
	m_pDrebApi->SendMsg(data);
}

void CDrebMsgThread::SwitchHost(unsigned short drebid,unsigned int svr, char m2b, char b2m)
{
	//���������л�Ϊ����
	S_BPC_RSMSG data;
	char errmsg[250];
	unsigned int len = BPUDATASIZE;
	if (m2b >0)
	{
		data.sMsgBuf = (PBPCCOMMSTRU)m_pDrebApi->PoolMalloc();
		if (data.sMsgBuf == NULL)
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���������л�ʧ��[%d %d->%d] PoolMalloc error",svr,m2b,b2m);
			return;
		}
		
		bzero(errmsg,sizeof(errmsg));
		bzero(data.sMsgBuf,sizeof(BPCCOMMSTRU));
		data.sMsgBuf->sBpcHead.nIndex = 100;
		data.sMsgBuf->sDBHead.cCmd = CMD_DPCALL;
		data.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo = ARB_SEND_CONTROL;
		data.sMsgBuf->sDBHead.d_Dinfo.d_nNodeId = drebid;
		data.sMsgBuf->sDBHead.d_Dinfo.d_nSvrMainId = svr;
		data.sMsgBuf->sDBHead.d_Dinfo.d_cSvrPrivateId = m2b;
		m_pXdp.ResetData();
		if (!m_pXdp.SetFieldValue("uint3",svr,errmsg))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP SetFieldValue uint3 error: %s",errmsg);
			m_pDrebApi->PoolFree(data.sMsgBuf);
			return;
		}
		if (!m_pXdp.SetFieldValue("uint4",(unsigned int)m2b,errmsg))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP SetFieldValue uint4 error: %s",errmsg);
			m_pDrebApi->PoolFree(data.sMsgBuf);
			return;
		}
		if (!m_pXdp.SetFieldValue("uint9",(unsigned int)ARBSTATUS_BACKUP,errmsg))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP SetFieldValue uint9 error: %s",errmsg);
			m_pDrebApi->PoolFree(data.sMsgBuf);
			return;
		}
		
		if (!m_pXdp.ToBuffer(data.sMsgBuf->sBuffer,len,errmsg))
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP ToBuffer error: %s",errmsg);
			m_pDrebApi->PoolFree(data.sMsgBuf);
			return;
		}
		data.sMsgBuf->sDBHead.nLen = len;
		m_pDrebApi->SendMsg(data); //���������л�Ϊ����

		SLEEP(100);
		//��������ȡ�������Ľ���ע��
		data.sMsgBuf = (PBPCCOMMSTRU)m_pDrebApi->PoolMalloc();
		if (data.sMsgBuf == NULL)
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���������л�ʧ��[%d %d->%d] PoolMalloc error",svr,m2b,b2m);
			return;
		}
		bzero(data.sMsgBuf,sizeof(BPCCOMMSTRU));
		data.sMsgBuf->sBpcHead.nIndex = 100;
		data.sMsgBuf->sDBHead.cCmd = CMD_DPBC_UNREGTX;
		data.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo = ARB_SEND_UNREG_SVR;
		data.sMsgBuf->sDBHead.d_Dinfo.d_nNodeId = drebid;
		data.sMsgBuf->sDBHead.d_Dinfo.d_nSvrMainId = svr;
		data.sMsgBuf->sDBHead.d_Dinfo.d_cSvrPrivateId = m2b;
		data.sMsgBuf->sDBHead.nLen = 0;
		m_pDrebApi->SendMsg(data);//��������ȡ�������Ľ���ע��

	}
	//���������л�Ϊ����
	if (b2m<1)
	{
		m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"����[%d %d]Ϊ���������÷��������л�Ϊ����",svr,m2b);
		return;
	}

	data.sMsgBuf = (PBPCCOMMSTRU)m_pDrebApi->PoolMalloc();
	if (data.sMsgBuf == NULL)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���������л�ʧ��[%d %d->%d] PoolMalloc error",svr,m2b,b2m);
		return;
	}
	bzero(data.sMsgBuf,sizeof(BPCCOMMSTRU));
	data.sMsgBuf->sBpcHead.nIndex = 100;
	data.sMsgBuf->sDBHead.cCmd = CMD_DPCALL;
	data.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo = ARB_SEND_CONTROL;
	data.sMsgBuf->sDBHead.d_Dinfo.d_nNodeId = drebid;
	data.sMsgBuf->sDBHead.d_Dinfo.d_nSvrMainId = svr;
	data.sMsgBuf->sDBHead.d_Dinfo.d_cSvrPrivateId = b2m;
	m_pXdp.ResetData();
	if (!m_pXdp.SetFieldValue("uint3",svr,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP SetFieldValue uint3 error: %s",errmsg);
		m_pDrebApi->PoolFree(data.sMsgBuf);
		return;
	}
	if (!m_pXdp.SetFieldValue("uint4",(unsigned int)b2m,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP SetFieldValue uint4 error: %s",errmsg);
		m_pDrebApi->PoolFree(data.sMsgBuf);
		return;
	}
	if (!m_pXdp.SetFieldValue("uint9",(unsigned int)ARBSTATUS_MASTER,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP SetFieldValue uint9 error: %s",errmsg);
		m_pDrebApi->PoolFree(data.sMsgBuf);
		return;
	}
	len = BPUDATASIZE;
	if (!m_pXdp.ToBuffer(data.sMsgBuf->sBuffer,len,errmsg))
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"XDP ToBuffer error: %s",errmsg);
		m_pDrebApi->PoolFree(data.sMsgBuf);
		return;
	}
	data.sMsgBuf->sDBHead.nLen = len;
	m_pDrebApi->SendMsg(data);//���������л�Ϊ����
	return;
}

void CDrebMsgThread::SetSwitchTime(std::vector<ARB_S_APPINFO *> &appinfo)
{
	unsigned int curtime = time(NULL);
	for (int j=0 ; j<appinfo.size() ; j++)
	{
		appinfo[j]->nLastSwtichTime = curtime;
	}
}
