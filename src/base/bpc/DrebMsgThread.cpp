// DrebMsgThread.cpp: implementation of the CDrebMsgThread class.
//
//////////////////////////////////////////////////////////////////////

#include "DrebMsgThread.h"
#include "GResource.h"

#define  DESENCRYPTKEY    "rd402wms"
#define  DESENCRYPTKEY3   "wms13711songfree"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CDrebMsgThread::CDrebMsgThread()
{
	m_pLog = NULL;
    m_pPoolData = NULL;//�������ݶ���
    m_pFuncTbl = NULL;//���ܱ�
    m_pSockMgr = NULL;//bpu���ӹ���
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
    
	return true;
}


bool CDrebMsgThread::SetGlobalVar(CPoolDataMsg* pooldata, CFuncTbl* tbl, CSocketMgr* sockmgr)
{
    m_pPoolData = pooldata;
    m_pFuncTbl = tbl;
    m_pSockMgr = sockmgr;
    return true;
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
    m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "Recv DREB index[%d]connect rsp��subscribe broadcast", rcvdata.sMsgBuf->sBpcHead.nIndex);
#else
    m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "�յ�DREB index[%d]���ӳɹ�����Ӧ �����߶��Ĺ㲥", rcvdata.sMsgBuf->sBpcHead.nIndex);
#endif
    std::vector<S_FUNCINFO_TBL> funclist;
    m_pFuncTbl->GetAllFunc(funclist);
    std::vector<int>txlist;
    for (int i = 0; i < funclist.size(); i++)
    {
        txlist.push_back(funclist[i].nFuncNo);
    }
    m_pDrebApi->RegisterDreb(rcvdata.sMsgBuf->sBpcHead.nIndex, &txlist);
	m_pDrebApi->Subscribe(rcvdata.sMsgBuf->sBpcHead.nIndex,m_lBcRegister);
	
}

// ������: OnMsgDrebAns
// ���  : ������ 2015-4-23 15:55:58
// ����  : virtual void 
// ����  : S_BPC_RSMSG &rcvdata
// ����  : ��Ӧ���߹�����Ӧ������
void CDrebMsgThread::OnMsgDrebAns(S_BPC_RSMSG &rcvdata)
{
    switch (rcvdata.sMsgBuf->sDBHead.cCmd)
    {
    
    case CMD_DPCALL:
    case CMD_DPACALL:
    case CMD_DPBC:
    case CMD_DPABC:
    case CMD_DPPUSH:
    case CMD_DPPOST:
        //bpc�����ֱ��Ӧ���bpu
        if (m_pSockMgr->AffirmIndex(rcvdata.sMsgBuf->sDBHead.b_Cinfo.b_cIndex))
        {
            //�ж�����ʱ����Ƿ���ͬ ,b_Cinfo���b_cIndex���bpu����index,b_nSerial���bpu����indexʱ���
            if (rcvdata.sMsgBuf->sDBHead.b_Cinfo.b_nSerial == m_pSockMgr->at(rcvdata.sMsgBuf->sDBHead.b_Cinfo.b_cIndex)->m_nConntime)
            {
                rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN + rcvdata.sMsgBuf->sDBHead.nLen;
                rcvdata.sMsgBuf->sBpcHead.nBpuIndex = rcvdata.sMsgBuf->sDBHead.b_Cinfo.b_cIndex;
                rcvdata.sMsgBuf->sBpcHead.nConnectTime = m_pSockMgr->at(rcvdata.sMsgBuf->sDBHead.b_Cinfo.b_cIndex)->m_nConntime;
                //Ӧ��ص�BPC���϶���extcall��
                rcvdata.sMsgBuf->sBpcHead.cMsgType = MSG_EXTCALL;
                m_pSockMgr->at(rcvdata.sMsgBuf->sDBHead.b_Cinfo.b_cIndex)->SendMsg(rcvdata);
                return;
            }
            m_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, "BPU���Ӧ��ʱ����ʱ������� index[%d]����Ϣ[%s]  DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Դ[%d %d] czip[%d] ҵ�����ݳ���[%d]", \
                rcvdata.sMsgBuf->sDBHead.b_Cinfo.b_cIndex, CDrebMsgProcBase::GetBpcMsgType(rcvdata.sMsgBuf->sBpcHead.cMsgType).c_str(), \
                CDrebMsgProcBase::GetDrebCmdType(rcvdata.sMsgBuf->sDBHead.cCmd).c_str(), rcvdata.sMsgBuf->sDBHead.cNextFlag, \
                rcvdata.sMsgBuf->sDBHead.cRaflag, rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo, rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId, \
                rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId, rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial, \
                rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial, rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook, \
                rcvdata.sMsgBuf->sDBHead.cZip, rcvdata.sMsgBuf->sDBHead.nLen);
        }
        else
        {
            m_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, "BPU���Ӧ��ʱ����index�Ƿ� index[%d]����Ϣ[%s]  DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Դ[%d %d] cZip[%d] ҵ�����ݳ���[%d]", \
                rcvdata.sMsgBuf->sDBHead.b_Cinfo.b_cIndex, CDrebMsgProcBase::GetBpcMsgType(rcvdata.sMsgBuf->sBpcHead.cMsgType).c_str(), \
                CDrebMsgProcBase::GetDrebCmdType(rcvdata.sMsgBuf->sDBHead.cCmd).c_str(), rcvdata.sMsgBuf->sDBHead.cNextFlag, \
                rcvdata.sMsgBuf->sDBHead.cRaflag, rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo, rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId, \
                rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId, rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial, \
                rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial, rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook, \
                rcvdata.sMsgBuf->sDBHead.cZip, rcvdata.sMsgBuf->sDBHead.nLen);
        }
        m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
        rcvdata.sMsgBuf = NULL;
        break;
    default:
        m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
        rcvdata.sMsgBuf = NULL;
        break;
    }
    return;
}

void CDrebMsgThread::OnMsgRequest(S_BPC_RSMSG& rcvdata)
{
    switch (rcvdata.sMsgBuf->sDBHead.cCmd)
    {
    case CMD_DPCALL:
    case CMD_DPACALL:
    case CMD_DPBC:
    case CMD_DPABC:
    case CMD_DPPUSH:
    case CMD_DPPOST:
        m_pLog->LogBin(LOG_DEBUG + 2, __FILE__, __LINE__, "�յ���������", rcvdata.sMsgBuf->sBuffer, rcvdata.sMsgBuf->sDBHead.nLen);
//				m_pRes->PutTime();

// 				//���Դ���
// 				rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
// 				sprintf(rcvdata.sMsgBuf->sBuffer,"����bpcֱ�ӷ���");
// 				rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
// 				rcvdata.sMsgBuf->sDBHead.cZip=0;
// 				m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
// 
// 				return;
                //������У�����Ϣ�߳�ȡ��������Ϣ��������Ӧ��BPU��
        m_pPoolData->PushData(rcvdata);
        break;
    default:
        m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
        rcvdata.sMsgBuf = NULL;
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
	m_bIsRunning = false;
	Join();
}

void CDrebMsgThread::OnGetBpuNextPack(S_BPC_RSMSG& rcvdata)
{
    m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
    rcvdata.sMsgBuf = NULL;
}
void CDrebMsgThread::OnGetBpcNextPack(S_BPC_RSMSG& rcvdata)
{
    m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
    rcvdata.sMsgBuf = NULL;
}
void CDrebMsgThread::MonitorInfo(S_BPC_RSMSG& rcvdata)
{
    int i;
    CBF_Xml xmlpack;
    xmlpack.SetNodeValueByPath("Monitor/BPC�����ڵ�", false, (int)m_pRes->g_nSvrMainId);
    xmlpack.SetNodeValueByPath("Monitor/BPC˽�нڵ�", false, (int)m_pRes->g_nSvrPrivateId);
    xmlpack.SetNodeValueByPath("Monitor/����ʱ��", false, m_pRes->g_sStartDateTime, false);
    char sip[40];
    char drebstatus[40];
    int  drebport;
    int  drebid;
    int  drebprivateid;

    //������Ϣ
    for (i = 0; i < m_pRes->g_vDrebLinkInfo.size(); i++)
    {
        CXmlNode* connnode = xmlpack.AddNodeByPath("Monitor/DREB������Ϣ", false, "����", false, "");
        if (connnode == NULL)
        {
            m_pDrebApi->GetLogPoint()->LogMp(LOG_ERROR, __FILE__, __LINE__, "AddNodeByPath ����");
            continue;
        }
        bzero(sip, sizeof(sip));
        bzero(drebstatus, sizeof(drebstatus));
        m_pDrebApi->GetDrebInfo(i, sip, drebport, drebid, drebprivateid, drebstatus);
        connnode->SetAttribute("ip", false, sip);
        connnode->SetAttribute("port", false, drebport);
        connnode->SetAttribute("index", false, i);
        connnode->SetAttribute("�����ڵ�", false, drebid);
        connnode->SetAttribute("˽�нڵ�", false, drebprivateid);
        connnode->SetAttribute("status", false, drebstatus);

    }
    CGResource *res = (CGResource*)m_pRes;
    //BPU��Ϣ
    int k;
    int connNum = 0;
    xmlpack.AddNodeByPath("Monitor", false, "BPU�б�", false, "");
    for (i = 0; i < res->g_vBpuGroupInfo.size(); i++)
    {
        connNum = 0;
        for (k = res->g_vBpuGroupInfo[i].g_nBeginSocketIndex; k < res->g_vBpuGroupInfo[i].g_nEndSocketIndex; k++)
        {
            if (m_pSockMgr->at(k)->m_sock != INVALID_SOCKET)
            {
                connNum++;
            }
        }
        CXmlNode* bpunode = xmlpack.AddNodeByPath("Monitor/BPU�б�", false, res->g_vBpuGroupInfo[i].g_sBpuGroupName.c_str(), true, "");
        bpunode->SetAttribute("����BPU��", false, res->g_vBpuGroupInfo[i].g_nBpuNum);
        bpunode->SetAttribute("ʵ��BPU��", false, connNum);
        bpunode->SetAttribute("����ģʽ", false, res->g_vBpuGroupInfo[i].g_nDispatchMode);
        bpunode->SetAttribute("�Զ�����", false, res->g_vBpuGroupInfo[i].g_bIsAutoLoadBu);
        bpunode->SetAttribute("��ʼ��������", false, res->g_vBpuGroupInfo[i].g_nBeginSocketIndex);
        bpunode->SetAttribute("������������", false, res->g_vBpuGroupInfo[i].g_nEndSocketIndex);
    }

    int len = DREBDATASIZE;
    if (!xmlpack.ToBuffer(rcvdata.sMsgBuf->sBuffer, len))
    {
        sprintf(rcvdata.sMsgBuf->sBuffer, "���岻�㣬������bpcinfo.txt�鿴");
        rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
        m_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, "���岻��");
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
    return;
}
void CDrebMsgThread::MonitorBpu(S_BPC_RSMSG& rcvdata)
{
    int i, k;
    int connNum = 0;
    CBF_Xml xmlpack;
    xmlpack.SetNodeValueByPath("Monitor/BPC�����ڵ�", false, (int)m_pRes->g_nSvrMainId);
    xmlpack.SetNodeValueByPath("Monitor/BPC˽�нڵ�", false, (int)m_pRes->g_nSvrPrivateId);
    xmlpack.SetNodeValueByPath("Monitor/����ʱ��", false, m_pRes->g_sStartDateTime, false);
    //BPU��Ϣ
    CGResource* res = (CGResource*)m_pRes;
    xmlpack.AddNodeByPath("Monitor", false, "BPU�б�", false, "");
    for (i = 0; i < res->g_vBpuGroupInfo.size(); i++)
    {
        connNum = 0;
        for (k = res->g_vBpuGroupInfo[i].g_nBeginSocketIndex; k < res->g_vBpuGroupInfo[i].g_nEndSocketIndex; k++)
        {
            if (m_pSockMgr->at(k)->m_sock != INVALID_SOCKET)
            {
                connNum++;
            }
        }
        CXmlNode* bpunode = xmlpack.AddNodeByPath("Monitor/BPU�б�", false, res->g_vBpuGroupInfo[i].g_sBpuGroupName.c_str(), true, "");
        if (bpunode != NULL)
        {
            bpunode->SetAttribute("����", false, res->g_vBpuGroupInfo[i].g_sBpuGroupName);
            bpunode->SetAttribute("����BPU��", false, res->g_vBpuGroupInfo[i].g_nBpuNum);
            bpunode->SetAttribute("ʵ��BPU��", false, connNum);
            bpunode->SetAttribute("����Ӧ��", false, res->g_vBpuGroupInfo[i].g_bIsAnsPing);
            bpunode->SetAttribute("����ģʽ", false, res->g_vBpuGroupInfo[i].g_nDispatchMode);
            bpunode->SetAttribute("�Զ�����", false, res->g_vBpuGroupInfo[i].g_bIsAutoLoadBu);
            bpunode->SetAttribute("������", false, res->g_vBpuGroupInfo[i].g_sBuPrgName);
            bpunode->SetAttribute("����Ŀ¼", false, res->g_vBpuGroupInfo[i].g_sBuPrgPath);
            bpunode->SetAttribute("����Ŀ¼", false, res->g_vBpuGroupInfo[i].g_sBuRunPath);
            bpunode->SetAttribute("���в���", false, res->g_vBpuGroupInfo[i].g_sBuPrgPara);
            bpunode->SetAttribute("��ʼ��������", false, res->g_vBpuGroupInfo[i].g_nBeginSocketIndex);
            bpunode->SetAttribute("������������", false, res->g_vBpuGroupInfo[i].g_nEndSocketIndex);

        }
    }

    int len = DREBDATASIZE;
    if (!xmlpack.ToBuffer(rcvdata.sMsgBuf->sBuffer, len))
    {
        sprintf(rcvdata.sMsgBuf->sBuffer, "���岻�㣬������bpcbpu.txt�鿴");
        rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
        m_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, "���岻��");
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
void CDrebMsgThread::OnMsgReportBpc(S_BPC_RSMSG& rcvdata)
{
    MonitorReport(rcvdata);
}
void CDrebMsgThread::MonitorReport(S_BPC_RSMSG& rcvdata)
{
    if (m_pRes->g_nUseMonitor != 1)
    {
        return;
    }
    //�����Ϣ
    int i, k;
    CBF_Xml        pXml;
    if (!pXml.FromBuffer(rcvdata.sMsgBuf->sBuffer))
    {
        pXml.SetNodeValueByPath("Monitor/�����ڵ�", false, (int)m_pRes->g_nSvrMainId);
        pXml.SetNodeValueByPath("Monitor/˽�нڵ�", false, (int)m_pRes->g_nSvrPrivateId);
        pXml.SetNodeValueByPath("Monitor/�ڵ�����", false, 1);//bpc
        pXml.SetNodeValueByPath("Monitor/��������", false, 0);//����
        pXml.SetNodeValueByPath("Monitor/����ʱ��", false, m_pRes->g_sStartDateTime, false);
        int total = 0;
        int used = 0;
        int bufsize = 0;
        m_pDrebApi->GetBufferPoolInfo(total, used, bufsize);
        pXml.SetNodeAttribute("Monitor/�ڴ滺���", false, "�ܷ���", false, total);
        pXml.SetNodeAttribute("Monitor/�ڴ滺���", false, "δʹ��", false, used);
        pXml.SetNodeAttribute("Monitor/�ڴ滺���", false, "ÿ���С", false, bufsize);
        if (m_pRes->g_nMonitorHost == 1)
        {

            S_MONITOR_HOST hostinfo;
            std::vector<S_MONITOR_DISK>  diskinfo;
            m_pDrebApi->GetHostInfo(hostinfo, diskinfo);
            pXml.SetNodeValueByPath("Monitor/������Դ/CPU", false, (int)hostinfo.nCpuRate);
            pXml.SetNodeAttribute("Monitor/������Դ/�ڴ�", false, "total", false, (int)hostinfo.nTotalMemory);
            pXml.SetNodeAttribute("Monitor/������Դ/�ڴ�", false, "used", false, (int)hostinfo.nUsedMemory);
            for (i = 0; i < diskinfo.size(); i++)
            {
                CXmlNode* disknode = pXml.AddNodeByPath("Monitor/������Դ/����", false, "����", false, "");
                if (disknode != NULL)
                {
                    disknode->SetAttribute("drive", false, diskinfo[i].sDiskName, true);
                    disknode->SetAttribute("total", false, (int)diskinfo[i].nTotalSpace);
                    disknode->SetAttribute("used", false, (int)diskinfo[i].nUsedSpace);
                }
            }
        }
        char sip[40];
        char drebstatus[40];
        int  drebport;
        int  drebid;
        int  drebprivateid;

        //������Ϣ
        for (i = 0; i < m_pRes->g_vDrebLinkInfo.size(); i++)
        {
            CXmlNode* connnode = pXml.AddNodeByPath("Monitor/DREB������Ϣ", false, "����", false, "");
            if (connnode == NULL)
            {
                m_pDrebApi->GetLogPoint()->LogMp(LOG_ERROR, __FILE__, __LINE__, "AddNodeByPath ����");
                continue;
            }
            bzero(sip, sizeof(sip));
            bzero(drebstatus, sizeof(drebstatus));
            m_pDrebApi->GetDrebInfo(i, sip, drebport, drebid, drebprivateid, drebstatus);
            connnode->SetAttribute("ip", false, sip);
            connnode->SetAttribute("port", false, drebport);
            connnode->SetAttribute("index", false, i);
            connnode->SetAttribute("�����ڵ�", false, drebid);
            connnode->SetAttribute("˽�нڵ�", false, drebprivateid);
            connnode->SetAttribute("status", false, drebstatus);

        }
    }
    CGResource* res = (CGResource*)m_pRes;
    int connNum = 0;
    //BPC��Ϣ
    pXml.AddNodeByPath("Monitor/BPU��Ϣ", false, "BPULIST", false, "");
    for (i = 0; i < res->g_vBpuGroupInfo.size(); i++)
    {
        connNum = 0;
        for (k = res->g_vBpuGroupInfo[i].g_nBeginSocketIndex; k < res->g_vBpuGroupInfo[i].g_nEndSocketIndex; k++)
        {
            if (m_pSockMgr->at(k)->m_sock != INVALID_SOCKET)
            {
                connNum++;
            }
        }
        CXmlNode* bpunode = pXml.AddNodeByPath("Monitor/BPU��Ϣ/BPULIST", false, res->g_vBpuGroupInfo[i].g_sBpuGroupName.c_str(), true, "");
        if (bpunode != NULL)
        {
            bpunode->SetAttribute("����", false, res->g_vBpuGroupInfo[i].g_sBpuGroupName);
            bpunode->SetAttribute("��Ŀ", false, res->g_vBpuGroupInfo[i].g_nBpuNum);
            bpunode->SetAttribute("ʵ����", false, connNum);
            bpunode->SetAttribute("����Ӧ��", false, res->g_vBpuGroupInfo[i].g_bIsAnsPing);
            bpunode->SetAttribute("����ģʽ", false, res->g_vBpuGroupInfo[i].g_nDispatchMode);
            bpunode->SetAttribute("�Զ�����", false, res->g_vBpuGroupInfo[i].g_bIsAutoLoadBu);
            bpunode->SetAttribute("������", false, res->g_vBpuGroupInfo[i].g_sBuPrgName);
            bpunode->SetAttribute("����Ŀ¼", false, res->g_vBpuGroupInfo[i].g_sBuPrgPath);
            bpunode->SetAttribute("����Ŀ¼", false, res->g_vBpuGroupInfo[i].g_sBuRunPath);
            bpunode->SetAttribute("����", false, res->g_vBpuGroupInfo[i].g_sBuPrgPara);
        }
    }

    S_BPC_RSMSG  mdata;//�������ݽṹ
//	bzero(&mdata,sizeof(S_BPC_RSMSG));
    mdata.sMsgBuf = (PBPCCOMMSTRU)m_pDrebApi->PoolMalloc();
    if (mdata.sMsgBuf == NULL)
    {
        m_pLog->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "������Ϣ�ռ����");
        return;
    }
    int len = DREBDATASIZE;
    if (!pXml.ToBuffer(mdata.sMsgBuf->sBuffer, len))
    {
        m_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, "ToBuffer����");
        m_pDrebApi->PoolFree(mdata.sMsgBuf);
        return;
    }
    mdata.sMsgBuf->sDBHead.nLen = len;

    m_pLog->LogBin(LOG_DEBUG + 1, __FILE__, __LINE__, "���֪ͨ", mdata.sMsgBuf->sBuffer, mdata.sMsgBuf->sDBHead.nLen);


    mdata.sMsgBuf->sDBHead.cRaflag = 0;
    mdata.sMsgBuf->sDBHead.cDrebAffirm = 0;
    mdata.sMsgBuf->sDBHead.cZip = 0;
    mdata.sMsgBuf->sDBHead.cNextFlag = 0;

    mdata.sMsgBuf->sDBHead.cCmd = CMD_DPPUSH;
    mdata.sMsgBuf->sDBHead.d_Dinfo.d_nNodeId = m_pRes->g_nMonitorDrebId;
    mdata.sMsgBuf->sDBHead.d_Dinfo.d_cNodePrivateId = m_pRes->g_nMonitorDrebPrivateId;
    mdata.sMsgBuf->sDBHead.d_Dinfo.d_nSvrMainId = m_pRes->g_nMonitorSvrId;
    mdata.sMsgBuf->sDBHead.d_Dinfo.d_cSvrPrivateId = m_pRes->g_nMonitorSvrPrivateId;
    mdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo = m_pRes->g_nMonitorTxCode;
    mdata.sMsgBuf->sBpcHead.cMsgType = MSG_BPCMONITOR;
    mdata.nRtime = time(NULL);
    S_FUNCINFO_TBL fn;
    int flag = 0;
    fn = m_pFuncTbl->FindFunc(mdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo, flag);
    if (flag == 1)
    {
        fn.nCallNum++;
        mdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN + mdata.sMsgBuf->sDBHead.nLen;
        strcpy(mdata.sBpuGroupName, fn.sBpuGroupName);//�� BPC����ļ�ط���
        //20151102 ֱ�ӷŵ�BPU��Ӧ���̶߳���
        if (res->PushData(fn.nBpuGroupIndex, mdata, fn.cPrio) != 0)
        {
            m_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, "��BPU����г��� ��Ϣ[%s]  DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Դ[%d %d] cZip[%d] ҵ�����ݳ���[%d]", \
                CDrebMsgProcBase::GetBpcMsgType(mdata.sMsgBuf->sBpcHead.cMsgType).c_str(), \
                CDrebMsgProcBase::GetDrebCmdType(mdata.sMsgBuf->sDBHead.cCmd).c_str(), mdata.sMsgBuf->sDBHead.cNextFlag, \
                mdata.sMsgBuf->sDBHead.cRaflag, mdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo, mdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId, \
                mdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId, mdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial, \
                mdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial, mdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook, \
                mdata.sMsgBuf->sDBHead.cZip, mdata.sMsgBuf->sDBHead.nLen);
            m_pDrebApi->PoolFree(mdata.sMsgBuf);
            mdata.sMsgBuf = NULL;
        }
    }
    else
    {
        bzero(mdata.sBpuGroupName, sizeof(mdata.sBpuGroupName));
        m_pPoolData->PushData(mdata);
    }

    return;
}
bool CDrebMsgThread::UnzipBuf(BPCCOMMSTRU& data)
{
    //0���� 1des������Կ����16 2ѹ�� 3DES���� 4DES���ܲ�ѹ��
    if (data.sDBHead.nLen == 0)
    {
        data.sDBHead.cZip = 0;
        return true;
    }
    if (data.sDBHead.cZip > 0 && data.sDBHead.cZip <= 4)
    {
        char zipDataBuf[BPUDATASIZE];
        bzero(zipDataBuf, sizeof(zipDataBuf));
        unsigned int dlen = BPUDATASIZE;
        int ddlen;
        switch (data.sDBHead.cZip)
        {
        case 1:
            ddlen = data.sDBHead.nLen;
            if (m_pDes.UncryptString(data.sBuffer, ddlen, DESENCRYPTKEY) <= 0)
            {
                m_pLog->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "���ܳ���");
                return false;
            }
            data.sDBHead.nLen = ddlen;
            data.sDBHead.cZip = 0;
            break;
        case 2:
            memcpy(zipDataBuf, data.sBuffer, data.sDBHead.nLen);
            if (!CBF_Tools::Uncompress((unsigned char*)(data.sBuffer), dlen, (unsigned char*)zipDataBuf, data.sDBHead.nLen))
            {
                m_pLog->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "��ѹ������");
                return false;
            }
            data.sDBHead.nLen = dlen;
            data.sDBHead.cZip = 0;
            break;
        case 3:
            ddlen = data.sDBHead.nLen;
            if (m_pDes.UncryptString(data.sBuffer, ddlen, DESENCRYPTKEY3) <= 0)
            {
                m_pLog->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "���ܳ���");
                return false;
            }
            data.sDBHead.nLen = ddlen;
            data.sDBHead.cZip = 0;
            break;
        case 4:
            ddlen = data.sDBHead.nLen;
            m_pLog->LogBin(LOG_DEBUG + 1, __FILE__, __LINE__, "����ǰ����", data.sBuffer, data.sDBHead.nLen);
            if (m_pDes.UncryptString(data.sBuffer, ddlen, DESENCRYPTKEY) <= 0)
            {
                m_pLog->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "���ܳ���");
                return false;
            }
            data.sDBHead.nLen = ddlen;
            m_pLog->LogBin(LOG_DEBUG + 1, __FILE__, __LINE__, "���ܺ�����", data.sBuffer, data.sDBHead.nLen);
            memcpy(zipDataBuf, data.sBuffer, data.sDBHead.nLen);
            if (!CBF_Tools::Uncompress((unsigned char*)(data.sBuffer), dlen, (unsigned char*)zipDataBuf, data.sDBHead.nLen))
            {
                m_pLog->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "��ѹ������");
                return false;
            }
            data.sDBHead.nLen = dlen;
            m_pLog->LogBin(LOG_DEBUG + 1, __FILE__, __LINE__, "��ѹ������", data.sBuffer, data.sDBHead.nLen);
            data.sDBHead.cZip = 0;
            break;
        default:
            return false;
        }
    }

    return true;
}
void CDrebMsgThread::MonitorSetBpu(S_BPC_RSMSG& rcvdata)
{
    //����BPU��Ŀ������ģʽ���Ƿ񱣴��������ļ�
// 	<Monitor>
// 		<SetBpu mode="0" remark="0ֻ�����ڴ�����1ͬʱ���µ������ļ�">
//         <bpu1>
// 				<�������� remark="��BPU��ѹ������0,���ķ���BPU1">0</��������>
// 				<����Ӧ�� remark="��Ϊc/c++��bpuʱ������Ӧ��������Ϣ">0</����Ӧ��>
// 				<����ģʽ remark="0��ʾ���з��䣬1��ʾBPU��ֻ��Ϊ1��һֱ����">0</����ģʽ>
// 				<��������BPU�� remark="�����Դ���ҵ��ĵ�Ԫ��Ŀ�����������ģʽΪ1ʱֻ��Ϊ1">1</��������BPU��>
// 				<AUTOLOADBPU use="0" remark="�Զ���BPU��������1������Ϊ0">
// 					<BPU������ remark="BU��������ƣ�����·��">bfBPUd.exe</BPU������>
// 					<BPU����·�� remark="���Ի����·��">D:\BUBASE\bin</BPU����·��>
// 					<BPU����Ŀ¼ remark="���Ի����Ŀ¼">D:\BUBASE\bin</BPU����Ŀ¼>
// 					<BPU�������� remark="����BPU����Ĳ���">D:\BUBASE\bin\bfBPU.xml</BPU��������>
// 				</AUTOLOADBPU>
//         </bpu1>
// 		</SetBpu>
// 	</Monitor>
    if (rcvdata.sMsgBuf->sDBHead.cZip != 0)
    {
        if (!UnzipBuf(*rcvdata.sMsgBuf))
        {
            sprintf(rcvdata.sMsgBuf->sBuffer, "���ݽ�ѹ������");
            rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
            rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
            rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
            rcvdata.sMsgBuf->sDBHead.cZip = 0;
            m_pDrebApi->SendMsg(rcvdata);
            return;
        }
    }
    CBF_Xml  xmlpack;
    if (!xmlpack.FromBuffer(rcvdata.sMsgBuf->sBuffer))
    {
        sprintf(rcvdata.sMsgBuf->sBuffer, "�������ݷ�xml");
        rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
        rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
        rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
        rcvdata.sMsgBuf->sDBHead.cZip = 0;
        m_pDrebApi->SendMsg(rcvdata);
        return;
    }
    CXmlNode* setnode = xmlpack.GetNodeByPath("Monitor/SetBpu", false);
    if (setnode == NULL)
    {
        sprintf(rcvdata.sMsgBuf->sBuffer, "xml��[Monitor/SetBpu]�ڵ�");
        rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
        rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
        rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
        rcvdata.sMsgBuf->sDBHead.cZip = 0;
        m_pDrebApi->SendMsg(rcvdata);
        return;
    }
    int savemode = 0;
    if (xmlpack.GetNodeAttribute("Monitor/SetBpu", false, "mode", false, savemode) == NULL)
    {
        savemode = 0;//������Ĭ��Ϊ0
    }
    CGResource* res = (CGResource*)m_pRes;
    std::string bpugroupname;
    int i;
    int bpuindex = -1;
    char tmpchar[200];
    char tmpcharconf[200];
    bool isfind = false;
    std::string namevalue;
    if (savemode == 1)
    {
        CBF_Date_Time dt;
        sprintf(tmpchar, "%s.%s%s", res->m_sBpcParaFileName.c_str(), dt.ToStringD8().c_str(), dt.ToStringT6().c_str());
        res->m_xmlconf.ToFile(tmpchar);
    }

    
    CXmlNode* bpugnode = (CXmlNode*)setnode->GetFirstChild();
    while (bpugnode != NULL)
    {
        bpugroupname = bpugnode->GetNodeName();
        isfind = false;
        for (i = 0; i < res->g_vBpuGroupInfo.size(); i++)
        {
            if (bpugroupname.compare(res->g_vBpuGroupInfo[i].g_sBpuGroupName) == 0)
            {
                isfind = true;
                bpuindex = i;
                break;
            }
        }
        if (isfind)
        {
            //�ҵ���
            sprintf(tmpchar, "Monitor/SetBpu/%s/��������", bpugroupname.c_str());
            if (xmlpack.GetNodeValueByPath(tmpchar, false, namevalue) != NULL)
            {
                if (savemode == 1) //���������ļ�
                {
                    sprintf(tmpcharconf, "package/head/BPU����/BPU��/%s/��������", bpugroupname.c_str());
                    res->m_xmlconf.SetNodeValueByPath(tmpcharconf, false, namevalue.c_str());
                }
                //�����ڴ�
                if (atoi(namevalue.c_str()) == 1)
                {
                    res->g_vBpuGroupInfo[bpuindex].g_bIsUnencrypt = true;
                }
                else
                {
                    res->g_vBpuGroupInfo[bpuindex].g_bIsUnencrypt = false;
                }
            }
            sprintf(tmpchar, "Monitor/SetBpu/%s/����Ӧ��", bpugroupname.c_str());
            if (xmlpack.GetNodeValueByPath(tmpchar, false, namevalue) != NULL)
            {
                if (savemode == 1) //���������ļ�
                {
                    sprintf(tmpcharconf, "package/head/BPU����/BPU��/%s/����Ӧ��", bpugroupname.c_str());
                    res->m_xmlconf.SetNodeValueByPath(tmpcharconf, false, namevalue.c_str());
                }
                //�����ڴ�
                if (atoi(namevalue.c_str()) == 1)
                {
                    res->g_vBpuGroupInfo[bpuindex].g_bIsAnsPing = true;
                }
                else
                {
                    res->g_vBpuGroupInfo[bpuindex].g_bIsAnsPing = false;
                }
            }
            sprintf(tmpchar, "Monitor/SetBpu/%s/����ģʽ", bpugroupname.c_str());
            if (xmlpack.GetNodeValueByPath(tmpchar, false, namevalue) != NULL)
            {
                if (savemode == 1) //���������ļ�
                {
                    sprintf(tmpcharconf, "package/head/BPU����/BPU��/%s/����ģʽ", bpugroupname.c_str());
                    res->m_xmlconf.SetNodeValueByPath(tmpcharconf, false, namevalue.c_str());
                }
                //�����ڴ�
                res->g_vBpuGroupInfo[bpuindex].g_nDispatchMode = atoi(namevalue.c_str());
            }
            sprintf(tmpchar, "Monitor/SetBpu/%s/��������BPU��", bpugroupname.c_str());
            if (xmlpack.GetNodeValueByPath(tmpchar, false, namevalue) != NULL)
            {
                if (res->g_vBpuGroupInfo[bpuindex].g_nDispatchMode == 1)
                {
                    namevalue = "1";
                }
                //�����ڴ�
                res->g_vBpuGroupInfo[bpuindex].g_nBpuNum = atoi(namevalue.c_str());

                if (savemode == 1) //���������ļ�
                {
                    sprintf(tmpcharconf, "package/head/BPU����/BPU��/%s/��������BPU��", bpugroupname.c_str());
                    res->m_xmlconf.SetNodeValueByPath(tmpcharconf, false, namevalue.c_str());
                }
            }

            sprintf(tmpchar, "Monitor/SetBpu/%s/AUTOLOADBPU", bpugroupname.c_str());
            if (xmlpack.GetNodeAttribute(tmpchar, false, "use", false, namevalue) != NULL)
            {
                if (atoi(namevalue.c_str()) == 1)
                {
                    res->g_vBpuGroupInfo[bpuindex].g_bIsAutoLoadBu = true;
                }
                else
                {
                    res->g_vBpuGroupInfo[bpuindex].g_bIsAutoLoadBu = false;
                }

                if (savemode == 1) //���������ļ�
                {
                    sprintf(tmpcharconf, "package/head/BPU����/BPU��/%s/AUTOLOADBPU", bpugroupname.c_str());
                    res->m_xmlconf.SetNodeAttribute(tmpcharconf, false, "use", false, namevalue.c_str());
                }
                if (res->g_vBpuGroupInfo[bpuindex].g_bIsAutoLoadBu)//�Զ�����BPU
                {
                    sprintf(tmpchar, "Monitor/SetBpu/%s/AUTOLOADBPU/BPU������", bpugroupname.c_str());
                    if (xmlpack.GetNodeValueByPath(tmpchar, false, namevalue) != NULL)
                    {
                        res->g_vBpuGroupInfo[bpuindex].g_sBuPrgName = namevalue;
                        if (savemode == 1) //���������ļ�
                        {
                            sprintf(tmpcharconf, "package/head/BPU����/BPU��/%s/AUTOLOADBPU/BPU������", bpugroupname.c_str());
                            res->m_xmlconf.SetNodeValueByPath(tmpcharconf, false, namevalue.c_str());
                        }
                    }
                    sprintf(tmpchar, "Monitor/SetBpu/%s/AUTOLOADBPU/BPU����·��", bpugroupname.c_str());
                    if (xmlpack.GetNodeValueByPath(tmpchar, false, namevalue) != NULL)
                    {
                        res->g_vBpuGroupInfo[bpuindex].g_sBuPrgPath = namevalue;
                        if (savemode == 1) //���������ļ�
                        {
                            sprintf(tmpcharconf, "package/head/BPU����/BPU��/%s/AUTOLOADBPU/BPU����·��", bpugroupname.c_str());
                            res->m_xmlconf.SetNodeValueByPath(tmpcharconf, false, namevalue.c_str());
                        }
                    }
                    sprintf(tmpchar, "Monitor/SetBpu/%s/AUTOLOADBPU/BPU����Ŀ¼", bpugroupname.c_str());
                    if (xmlpack.GetNodeValueByPath(tmpchar, false, namevalue) != NULL)
                    {
                        res->g_vBpuGroupInfo[bpuindex].g_sBuRunPath = namevalue;
                        if (savemode == 1) //���������ļ�
                        {
                            sprintf(tmpcharconf, "package/head/BPU����/BPU��/%s/AUTOLOADBPU/BPU����Ŀ¼", bpugroupname.c_str());
                            res->m_xmlconf.SetNodeValueByPath(tmpcharconf, false, namevalue.c_str());
                        }
                    }
                    sprintf(tmpchar, "Monitor/SetBpu/%s/AUTOLOADBPU/BPU��������", bpugroupname.c_str());
                    if (xmlpack.GetNodeValueByPath(tmpchar, false, namevalue) != NULL)
                    {
                        res->g_vBpuGroupInfo[bpuindex].g_sBuPrgPara = namevalue;
                        if (savemode == 1) //���������ļ�
                        {
                            sprintf(tmpcharconf, "package/head/BPU����/BPU��/%s/AUTOLOADBPU/BPU��������", bpugroupname.c_str());
                            res->m_xmlconf.SetNodeValueByPath(tmpcharconf, false, namevalue.c_str());
                        }
                    }
                }
            }
            if (savemode == 1)
            {
                if (!res->m_xmlconf.ToFile(res->m_sBpcParaFileName.c_str()))
                {
                    m_pLog->LogMp(LOG_WARNNING, __FILE__, __LINE__, "���������ļ�[%s]ʧ��", res->m_sBpcParaFileName.c_str());
                }
            }
        }
        else
        {
            m_pLog->LogMp(LOG_WARNNING, __FILE__, __LINE__, "bpu��[%s]��BPC�����в�����", bpugroupname.c_str());
        }
        bpugnode = (CXmlNode*)bpugnode->getNextSibling();
    }
    sprintf(rcvdata.sMsgBuf->sBuffer, "���ò����ɹ�");
    rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
    rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
    rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
    rcvdata.sMsgBuf->sDBHead.cZip = 0;
    m_pDrebApi->SendMsg(rcvdata);
    return;
}
void CDrebMsgThread::MonitorSetMode(S_BPC_RSMSG& rcvdata)
{
    if (rcvdata.sMsgBuf->sDBHead.cZip != 0)
    {
        if (!UnzipBuf(*rcvdata.sMsgBuf))
        {
            sprintf(rcvdata.sMsgBuf->sBuffer, "���ݽ�ѹ������");
            rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
            rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
            rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
            rcvdata.sMsgBuf->sDBHead.cZip = 0;
            m_pDrebApi->SendMsg(rcvdata);
            return;
        }
    }
    CBF_Xml  xmlpack;
    if (!xmlpack.FromBuffer(rcvdata.sMsgBuf->sBuffer))
    {
        sprintf(rcvdata.sMsgBuf->sBuffer, "�������ݷ�xml");
        rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
        rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
        rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
        rcvdata.sMsgBuf->sDBHead.cZip = 0;
        m_pDrebApi->SendMsg(rcvdata);
        return;
    }
    CXmlNode* setnode = xmlpack.GetNodeByPath("Monitor/SetBpu", false);
    if (setnode == NULL)
    {
        sprintf(rcvdata.sMsgBuf->sBuffer, "xml��[Monitor/SetBpu]�ڵ�");
        rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
        rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
        rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
        rcvdata.sMsgBuf->sDBHead.cZip = 0;
        m_pDrebApi->SendMsg(rcvdata);
        return;
    }
    int savemode = 0;
    if (xmlpack.GetNodeAttribute("Monitor/SetMonitor", false, "mode", false, savemode) == NULL)
    {
        savemode = 0;//������Ĭ��Ϊ0
    }
    CGResource* res = (CGResource*)m_pRes;
    std::string bpugroupname;
    int i;

    char tmpchar[200];
    char tmpcharconf[200];
    bool isfind = false;
    std::string namevalue;
    if (savemode == 1)
    {
        CBF_Date_Time dt;
        sprintf(tmpchar, "%s.%s%s", res->m_sBpcParaFileName.c_str(), dt.ToStringD8().c_str(), dt.ToStringT6().c_str());
        res->m_xmlconf.ToFile(tmpchar);
    }
    // 	int   g_nUseMonitor;       //�Ƿ���
    // 	int   g_nMonitorHost;      //�Ƿ񱨸�������Դ
    // 	int   g_nMonitorDrebId;            //�����ڵ��
    // 	int   g_nMonitorDrebPrivateId;     //˽�нڵ��
    // 	int   g_nMonitorSvrId;            //��������ڵ��
    // 	int   g_nMonitorSvrPrivateId;     //˽�з���ڵ��
    // 	int   g_nMonitorTxCode;           //��ؽ�����
    if (xmlpack.GetNodeAttribute("Monitor/SetMonitor/�������", false, "use", false, namevalue) != NULL)
    {
        if (savemode == 1) //���������ļ�
        {
            sprintf(tmpcharconf, "package/head/�������");
            res->m_xmlconf.SetNodeAttribute(tmpcharconf, false, "use", false, namevalue.c_str(), false);
        }
        //�����ڴ�
        m_pRes->g_nUseMonitor = atoi(namevalue.c_str());
    }
    if (xmlpack.GetNodeAttribute("Monitor/SetMonitor/�������", false, "host", false, namevalue) != NULL)
    {
        if (savemode == 1) //���������ļ�
        {
            sprintf(tmpcharconf, "package/head/�������");
            res->m_xmlconf.SetNodeAttribute(tmpcharconf, false, "host", false, namevalue.c_str(), false);
        }
        //�����ڴ�
        m_pRes->g_nMonitorHost = atoi(namevalue.c_str());
    }
    if (xmlpack.GetNodeValueByPath("Monitor/SetMonitor/��ع����ڵ��", false, namevalue) != NULL)
    {
        if (savemode == 1) //���������ļ�
        {
            sprintf(tmpcharconf, "package/head/�������/��ع����ڵ��");
            res->m_xmlconf.SetNodeValueByPath(tmpcharconf, false, namevalue.c_str(), false);
        }
        //�����ڴ�
        m_pRes->g_nMonitorDrebId = atoi(namevalue.c_str());
    }
    if (xmlpack.GetNodeValueByPath("Monitor/SetMonitor/���˽�нڵ��", false, namevalue) != NULL)
    {
        if (savemode == 1) //���������ļ�
        {
            sprintf(tmpcharconf, "package/head/�������/���˽�нڵ��");
            res->m_xmlconf.SetNodeValueByPath(tmpcharconf, false, namevalue.c_str(), false);
        }
        //�����ڴ�
        m_pRes->g_nMonitorDrebPrivateId = atoi(namevalue.c_str());
    }
    if (xmlpack.GetNodeValueByPath("Monitor/SetMonitor/��ع��������", false, namevalue) != NULL)
    {
        if (savemode == 1) //���������ļ�
        {
            sprintf(tmpcharconf, "package/head/�������/��ع��������");
            res->m_xmlconf.SetNodeValueByPath(tmpcharconf, false, namevalue.c_str(), false);
        }
        //�����ڴ�
        m_pRes->g_nMonitorSvrId = atoi(namevalue.c_str());
    }
    if (xmlpack.GetNodeValueByPath("Monitor/SetMonitor/���˽�з����", false, namevalue) != NULL)
    {
        if (savemode == 1) //���������ļ�
        {
            sprintf(tmpcharconf, "package/head/�������/���˽�з����");
            res->m_xmlconf.SetNodeValueByPath(tmpcharconf, false, namevalue.c_str(), false);
        }
        //�����ڴ�
        m_pRes->g_nMonitorSvrPrivateId = atoi(namevalue.c_str());
    }
    if (xmlpack.GetNodeValueByPath("Monitor/SetMonitor/��ر��潻����", false, namevalue) != NULL)
    {
        if (savemode == 1) //���������ļ�
        {
            sprintf(tmpcharconf, "package/head/�������/��ر��潻����");
            res->m_xmlconf.SetNodeValueByPath(tmpcharconf, false, namevalue.c_str(), false);
        }
        //�����ڴ�
        m_pRes->g_nMonitorTxCode = atoi(namevalue.c_str());
    }
    if (savemode == 1)
    {
        if (!res->m_xmlconf.ToFile(res->m_sBpcParaFileName.c_str()))
        {
            m_pLog->LogMp(LOG_WARNNING, __FILE__, __LINE__, "���������ļ�[%s]ʧ��", res->m_sBpcParaFileName.c_str());
            sprintf(rcvdata.sMsgBuf->sBuffer, "���������ļ�[%s]ʧ��", res->m_sBpcParaFileName.c_str());
            rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
            rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
            rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
            rcvdata.sMsgBuf->sDBHead.cZip = 0;
            m_pDrebApi->SendMsg(rcvdata);
            return;
        }
    }
    sprintf(rcvdata.sMsgBuf->sBuffer, "���ò����ɹ�");
    rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
    rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
    rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
    rcvdata.sMsgBuf->sDBHead.cZip = 0;
    m_pDrebApi->SendMsg(rcvdata);
    return;
}
void CDrebMsgThread::MonitorStopBpu(S_BPC_RSMSG& rcvdata)
{
    if (rcvdata.sMsgBuf->sDBHead.cZip != 0)
    {
        if (!UnzipBuf(*rcvdata.sMsgBuf))
        {
            sprintf(rcvdata.sMsgBuf->sBuffer, "���ݽ�ѹ������");
            rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
            rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
            rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
            rcvdata.sMsgBuf->sDBHead.cZip = 0;
            m_pDrebApi->SendMsg(rcvdata);
            return;
        }
    }
    CBF_Xml  xmlpack;
    if (!xmlpack.FromBuffer(rcvdata.sMsgBuf->sBuffer))
    {
        sprintf(rcvdata.sMsgBuf->sBuffer, "�������ݷ�xml");
        rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
        rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
        rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
        rcvdata.sMsgBuf->sDBHead.cZip = 0;
        m_pDrebApi->SendMsg(rcvdata);
        return;
    }
    CXmlNode* stopnode = xmlpack.GetNodeByPath("Monitor/StopBpu", false);
    if (stopnode == NULL)
    {
        sprintf(rcvdata.sMsgBuf->sBuffer, "xml��[Monitor/StopBpu]�ڵ�");
        rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
        rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
        rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
        rcvdata.sMsgBuf->sDBHead.cZip = 0;
        m_pDrebApi->SendMsg(rcvdata);
        return;
    }
    CGResource* res = (CGResource*)m_pRes;
    int i;
    int bpuindex = -1;
    std::string bpugroupname;
    int bpugnum = 0;
    S_PROCESS_NODE node;
    bool isfind = false;
    char errmsg[200];
    CXmlNode* bpunode = (CXmlNode*)stopnode->GetFirstChild();
    while (bpunode != NULL)
    {
        bpugroupname = bpunode->GetNodeName();
        isfind = false;
        for (i = 0; i < res->g_vBpuGroupInfo.size(); i++)
        {
            if (bpugroupname.compare(res->g_vBpuGroupInfo[i].g_sBpuGroupName) == 0)
            {
                isfind = true;
                bpuindex = i;
                break;
            }
        }
        if (isfind)
        {
            node.prgname = res->g_vBpuGroupInfo[bpuindex].g_sBuPrgName;
            node.prgpara = res->g_vBpuGroupInfo[bpuindex].g_sBuPrgPara;
            node.prgpath = res->g_vBpuGroupInfo[bpuindex].g_sBuPrgPath;
            node.runpath = res->g_vBpuGroupInfo[bpuindex].g_sBuRunPath;
            node.nodeid = res->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName;

            if (res->g_vBpuGroupInfo[bpuindex].g_nDispatchMode == 1) //java�汾
            {
                m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "����ԭBPU[%s]�����Ƿ����[%d]", \
                    res->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(), res->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid);
                if (res->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid > 0) //�鿴�����Ƿ����
                {
                    node.pid = res->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid;
                    if (m_pMgrBpu.FindProcess(&node, errmsg) == 0) //�д˽���
                    {
                        m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "ԭBPU[%s]����[%d]�Ѵ��ڣ�ֹͣ", \
                            res->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(), res->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid);
                        if (m_pMgrBpu.StopProcess(&node, errmsg) != 0)
                        {
                            m_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, "BPU[%s]����[%d]ֹͣʧ��", \
                                res->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(), res->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid);
                        }
                    }
                }
            }
            else
            {
                for (int j = res->g_vBpuGroupInfo[bpuindex].g_nBeginSocketIndex; j < res->g_vBpuGroupInfo[bpuindex].g_nEndSocketIndex; j++)
                {
                    if (m_pSockMgr->at(j)->m_sock != INVALID_SOCKET)
                    {
                        node.pid = m_pSockMgr->at(j)->m_nPid;
                        m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "ֹͣBPU[%s]����[%d]", \
                            res->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(), node.pid);
                        if (m_pMgrBpu.StopProcess(&node, errmsg) != 0)
                        {
                            m_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, "BPU[%s]����[%d]ֹͣʧ��", \
                                res->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(), node.pid);
                        }
                    }
                }
            }

        }
        bpunode = (CXmlNode*)bpunode->getNextSibling();
    }
    sprintf(rcvdata.sMsgBuf->sBuffer, "ֹͣBPU�ɹ�");
    rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
    rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
    rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
    rcvdata.sMsgBuf->sDBHead.cZip = 0;
    m_pDrebApi->SendMsg(rcvdata);
    return;
}
void CDrebMsgThread::MonitorStartBpu(S_BPC_RSMSG& rcvdata)
{
    if (rcvdata.sMsgBuf->sDBHead.cZip != 0)
    {
        if (!UnzipBuf(*rcvdata.sMsgBuf))
        {
            sprintf(rcvdata.sMsgBuf->sBuffer, "���ݽ�ѹ������");
            rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
            rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
            rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
            rcvdata.sMsgBuf->sDBHead.cZip = 0;
            m_pDrebApi->SendMsg(rcvdata);
            return;
        }
    }
    CBF_Xml  xmlpack;
    if (!xmlpack.FromBuffer(rcvdata.sMsgBuf->sBuffer))
    {
        sprintf(rcvdata.sMsgBuf->sBuffer, "�������ݷ�xml");
        rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
        rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
        rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
        rcvdata.sMsgBuf->sDBHead.cZip = 0;
        m_pDrebApi->SendMsg(rcvdata);
        return;
    }
    CXmlNode* stopnode = xmlpack.GetNodeByPath("Monitor/StartBpu", false);
    if (stopnode == NULL)
    {
        sprintf(rcvdata.sMsgBuf->sBuffer, "xml��[Monitor/StartBpu]�ڵ�");
        rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
        rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
        rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
        rcvdata.sMsgBuf->sDBHead.cZip = 0;
        m_pDrebApi->SendMsg(rcvdata);
        return;
    }
    CGResource* res = (CGResource*)m_pRes;
    int i;
    int bpuindex = -1;
    std::string bpugroupname;
    int bpugnum = 0;
    S_PROCESS_NODE node;
    bool isfind = false;
    int k;
    int connNum = 0;
    char errmsg[200];
    CXmlNode* bpunode = (CXmlNode*)stopnode->GetFirstChild();
    while (bpunode != NULL)
    {
        bpugroupname = bpunode->GetNodeName();
        isfind = false;
        for (i = 0; i < res->g_vBpuGroupInfo.size(); i++)
        {
            if (bpugroupname.compare(res->g_vBpuGroupInfo[i].g_sBpuGroupName) == 0)
            {
                isfind = true;
                bpuindex = i;
                break;
            }
        }
        if (isfind)
        {
            node.prgname = res->g_vBpuGroupInfo[bpuindex].g_sBuPrgName;
            node.prgpara = res->g_vBpuGroupInfo[bpuindex].g_sBuPrgPara;
            node.prgpath = res->g_vBpuGroupInfo[bpuindex].g_sBuPrgPath;
            node.runpath = res->g_vBpuGroupInfo[bpuindex].g_sBuRunPath;
            node.nodeid = res->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName;

            connNum = 0;
            for (k = res->g_vBpuGroupInfo[bpuindex].g_nBeginSocketIndex; k < res->g_vBpuGroupInfo[bpuindex].g_nEndSocketIndex; k++)
            {
                if (m_pSockMgr->at(k)->m_sock != INVALID_SOCKET)
                {
                    connNum++;
                }
            }
            if (!res->g_vBpuGroupInfo[bpuindex].g_bIsAutoLoadBu) //�����Զ������Ž�������
            {
                if (res->g_vBpuGroupInfo[bpuindex].g_nDispatchMode == 1) //java�汾
                {
                    //					bpugnum = m_pRes->g_vBpuGroupInfo[bpuindex].g_nBpuNum - m_pRes->g_vBpuGroupInfo[bpuindex].g_nStartBpuNum;
                    bpugnum = res->g_vBpuGroupInfo[bpuindex].g_nBpuNum - connNum;
                    if (bpugnum > 0)
                    {
                        m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "����ԭBPU[%s]�����Ƿ����[%d]", \
                            res->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(), res->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid);
                        if (res->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid > 0) //�鿴�����Ƿ����
                        {
                            node.pid = res->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid;
                            if (m_pMgrBpu.FindProcess(&node, errmsg) == 0) //�д˽���
                            {
                                m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "ԭBPU[%s]����[%d]�Ѵ��ڣ�ֹͣ", \
                                    res->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(), res->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid);
                                if (m_pMgrBpu.StopProcess(&node, errmsg) != 0)
                                {
                                    m_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, "BPU[%s]����[%d]ֹͣʧ��", \
                                        res->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(), res->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid);
                                }
                            }
                            m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "BPU[%s]����", \
                                res->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str());
                            if (m_pMgrBpu.StopProcess(&node, errmsg) != 0)
                            {
                                m_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, "BPU[%s]����[%d]ֹͣʧ��", \
                                    res->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(), res->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid);
                            }
                        }
                    }
                }
                else
                {
                    //					bpugnum = m_pRes->g_vBpuGroupInfo[bpuindex].g_nBpuNum - m_pRes->g_vBpuGroupInfo[bpuindex].g_nStartBpuNum;
                    bpugnum = res->g_vBpuGroupInfo[bpuindex].g_nBpuNum - connNum;
                    for (int j = 0; j < bpugnum; j++)
                    {
                        if (m_pMgrBpu.StartProcess(&node, errmsg) != 0)
                        {
                            m_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, "BPU[%s]����ʧ��", \
                                res->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str());
                        }
                    }
                }
            }
        }
        bpunode = (CXmlNode*)bpunode->getNextSibling();
    }
    sprintf(rcvdata.sMsgBuf->sBuffer, "����BPU�ɹ�");
    rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
    rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
    rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
    rcvdata.sMsgBuf->sDBHead.cZip = 0;
    m_pDrebApi->SendMsg(rcvdata);
    return;
}


