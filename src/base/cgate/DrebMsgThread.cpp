// DrebMsgThread.cpp: implementation of the CDrebMsgThread class.
//
//////////////////////////////////////////////////////////////////////

#include "DrebMsgThread.h"
#include "GateResource.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern int   g_pRunFlag;

#define  DESENCRYPTKEY    "rd402wms"
#define  DESENCRYPTKEY3   "wms13711songfree"

CDrebMsgThread::CDrebMsgThread()
{
	m_pLog = NULL;
    g_pGateRes = NULL;
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


bool CDrebMsgThread::SetGlobalVar(CPoolData* pooldata,CSendData* senddata, CAIO_Work* aio)
{
    m_pPoolData = pooldata;
    m_pSendData = senddata;
    m_pAio = aio;
    
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
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"Recv DREB index[%d]connect rsp��subscribe broadcast", rcvdata.sMsgBuf->sBpcHead.nIndex);
#else
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�յ�DREB index[%d]���ӳɹ�����Ӧ �����߶��Ĺ㲥", rcvdata.sMsgBuf->sBpcHead.nIndex);
#endif
    std::vector<int> lBcRegister;
    g_pGateRes->g_lBCFuncList.Select(lBcRegister);
	m_pDrebApi->Subscribe(rcvdata.sMsgBuf->sBpcHead.nIndex, &lBcRegister);
	
}

// ������: OnMsgDrebAns
// ���  : ������ 2015-4-23 15:55:58
// ����  : virtual void 
// ����  : S_BPC_RSMSG &rcvdata
// ����  : ��Ӧ���߹�����Ӧ������
void CDrebMsgThread::OnMsgDrebAns(S_BPC_RSMSG &rcvdata)
{
	//Ӧ������
    S_CGATE_SMSG data;
    bzero(&data.data.head, CGATEHEADLEN);
    data.data.head.nLen = rcvdata.sMsgBuf->sDBHead.nLen;
    if (data.data.head.nLen > 0 && data.data.head.nLen < DREBDATASIZE)
    {
        memcpy(data.data.buffer, rcvdata.sMsgBuf->sBuffer, data.data.head.nLen);
    }
    else if (data.data.head.nLen >= DREBDATASIZE)
    {
        m_pLog->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "���ݷǷ� %d", data.data.head.nLen);
        m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
        rcvdata.sMsgBuf = NULL;
        return;
    }

    data.data.head.nRetCode = rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode;
    data.data.head.stComm.cCmd = rcvdata.sMsgBuf->sDBHead.cCmd;
    data.data.head.stComm.cMsgType = MSG_GATEREQ;
    data.data.head.stComm.cNextFlag = rcvdata.sMsgBuf->sDBHead.cNextFlag;
    data.data.head.stComm.cRaflag = rcvdata.sMsgBuf->sDBHead.cRaflag;
    data.data.head.stComm.cZip = rcvdata.sMsgBuf->sDBHead.cZip;
    data.data.head.stComm.nCrc = 0;
    data.data.head.stDest.cNodePrivateId = rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_cNodePrivateId;
    data.data.head.stDest.cSvrPrivateId = rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_cSvrPrivateId;
    data.data.head.stDest.nNodeId = rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nNodeId;
    data.data.head.stDest.nSerial = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial;
    data.data.head.stDest.nServiceNo = rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo;
    data.data.head.stDest.nSvrMainId = rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nSvrMainId;
    data.data.head.stNext.n_nNextNo = rcvdata.sMsgBuf->sDBHead.n_Ninfo.n_nNextNo;
    data.data.head.stNext.n_nNextOffset = rcvdata.sMsgBuf->sDBHead.n_Ninfo.n_nNextOffset;

    data.txcode = rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo;
    data.d_nNodeId = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId;
    data.d_cNodePrivateId = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId;
    data.s_nDrebSerial = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial;
    data.rtime = time(NULL);

    data.index = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nGateIndex;
    data.timestamp = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook;
    if (data.index == 0 && rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook == 0)
    {
        m_pLog->LogMp(LOG_WARNNING, __FILE__, __LINE__, "��Ч��Ӧ��");
        m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
        rcvdata.sMsgBuf = NULL;
        return;
    }
    data.isBC = 0;
    //��������ӦӦ������
    //ͳһ��һ���̼߳��ܲ����뷢�Ͷ���
    m_pSendData->PushData(data, 0);
    //		m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"���Ͷ�����Ŀ[%d]",m_pSendData->GetSize());
    m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
    rcvdata.sMsgBuf = NULL;
    return;
}

void CDrebMsgThread::OnMsgRequest(S_BPC_RSMSG& rcvdata)
{
    S_CGATE_SMSG data;
    bzero(&data.data.head, CGATEHEADLEN);
    data.data.head.nLen = rcvdata.sMsgBuf->sDBHead.nLen;
    if (data.data.head.nLen > 0 && data.data.head.nLen < DREBDATASIZE)
    {
        memcpy(data.data.buffer, rcvdata.sMsgBuf->sBuffer, data.data.head.nLen);
    }
    else if (data.data.head.nLen >= DREBDATASIZE)
    {
        m_pLog->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "���ݷǷ� %d", data.data.head.nLen);
        m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
        rcvdata.sMsgBuf = NULL;
        return;
    }
    
    data.s_nDrebSerial = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial;
    data.txcode = rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo;

    data.data.head.nRetCode = 0;
    data.data.head.stComm.cCmd = rcvdata.sMsgBuf->sDBHead.cCmd;
    data.data.head.stComm.cMsgType = MSG_PUSH;
    data.data.head.stComm.cNextFlag = rcvdata.sMsgBuf->sDBHead.cNextFlag;
    data.data.head.stComm.cRaflag = rcvdata.sMsgBuf->sDBHead.cRaflag;
    data.data.head.stComm.cZip = rcvdata.sMsgBuf->sDBHead.cZip;
    data.data.head.stComm.nCrc = 0;
    data.data.head.stDest.cNodePrivateId = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId;
    data.data.head.stDest.cSvrPrivateId = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cSvrPrivateId;
    data.data.head.stDest.nNodeId = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId;

    data.d_nNodeId = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId;
    data.d_cNodePrivateId = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId;
    data.s_nDrebSerial = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial;
    data.rtime = time(NULL);
    data.data.head.stDest.nServiceNo = rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo;
    data.data.head.stDest.nSvrMainId = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSvrMainId;
    data.data.head.stNext.n_nNextNo = 0;
    data.data.head.stNext.n_nNextOffset = 0;

    data.data.head.stDest.nSerial = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial; 
    data.index = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nGateIndex;
    data.timestamp = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook; 


    // #define  CMD_DPCALL      8  //�������߽ڵ�ͬ������ Ҫ�����մ�����ɺ�Ӧ��
    // #define  CMD_DPACALL     9  //�������߽ڵ��첽���� Ҫ����յ����������߽ڵ㷢���������ȷ��Ӧ��
    // #define  CMD_DPBC        10  //�������߽ڵ�㲥��������Ϣ����ָ���������߽ڵ�������ע��ķ���,Ҫ���������߽ڵ�Ӧ��
    // #define  CMD_DPABC       11  //�������߽ڵ�㲥��������Ϣ����ָ���������߽ڵ�������ע��ķ���,��Ҫ���������߽ڵ�Ӧ��
    // #define  CMD_DPPUSH      12 //�������߽ڵ����ͣ�����Ӧ��
    // #define  CMD_DPPOST      13 //�������߽ڵ�Ͷ�ݣ�Ҫ����յ����������߽ڵ�Ӧ��		
    if (rcvdata.sMsgBuf->sDBHead.cCmd == CMD_DPBC || rcvdata.sMsgBuf->sDBHead.cCmd == CMD_DPABC) //�㲥������ ������Ϣ
    {
        //����ʱs_nSerialΪ�������ڣ�s_nHookΪkey(��Լ��֤ȯid) s_nGateIndexΪ����ʱ������
        //20221211 �������õĹ㲥���𣬷����Ӧ�ķ��Ͷ��У��ɽ��ر��������ر���Ϊ1  �����Ϊ2
        int level = 2;

        S_BC_INFO* bcinfo = g_pGateRes->g_lBCFuncList.Select(rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo);
        if (bcinfo != NULL)
        {
            level = bcinfo->nLevel;
        }
        m_pLog->LogMp(LOG_DEBUG+1, __FILE__, __LINE__, "�㲥 ���� func=%d date=%d code=%d time=%d", rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial, rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook, rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nGateIndex);
        //��������ķ��ͣ�������ʱ���������ţ��󵽵Ĳ���
        if (level == 2 && g_pGateRes->g_nFilterQuo == 1)
        {
            if (m_pQuotation.IsNewQuotation(rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nGateIndex) > 0)
            {
                //������У�������
                m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "�㲥 ���� func=%d date=%d code=%d time=%d", rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo, rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial, rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook, rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nGateIndex);
                data.nkey = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook;
                data.isBC = 1;
                data.index = 0;
                data.timestamp = 0;
                m_pSendData->PushData(data, level);
                m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
                rcvdata.sMsgBuf = NULL;
                
            }
            else
            {
                m_pLog->LogMp(LOG_PROMPT, __FILE__, __LINE__, "�㲥 ���� func=%d date=%d code=%d time=%d�������£�����", rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo, rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial, rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook, rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nGateIndex);
                m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
                rcvdata.sMsgBuf = NULL;
            }
            return;
        }
        else
        {
            //������У�������
            data.nkey = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook;
            data.isBC = 1;
            data.index = 0;
            data.timestamp = 0;
            m_pSendData->PushData(data, level);
            m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
            rcvdata.sMsgBuf = NULL;
            return;
        }
        return;
    }
    else  if (rcvdata.sMsgBuf->sDBHead.cCmd == CMD_DPCALL || rcvdata.sMsgBuf->sDBHead.cCmd == CMD_DPACALL || \
        rcvdata.sMsgBuf->sDBHead.cCmd == CMD_DPPUSH || rcvdata.sMsgBuf->sDBHead.cCmd == CMD_DPPOST)
        //˽������ָ�����棬�ɽ��ر���ί�лر�  ����Ļر��Ǻ�̨���͵ģ����ǹ㲥������ǹ㲥��Ҫ�ߵ����棬�����key������������ָ���Ŀͻ�
    {
        if (data.index == 0)
        {
            m_pLog->LogMp(LOG_WARNNING, __FILE__, __LINE__, "��Ч������ index[%d] cCmd[%d]", data.index, data.data.head.stComm.cCmd);
            m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
            rcvdata.sMsgBuf = NULL;
            return;
        }
        //Դ��nGateIndex�����û���¼����������   s_nHook�������ӵ�ʱ���
        data.isBC = 0;
        //�ڶ�����Ϊ�ر���Ϣ  
        m_pSendData->PushData(data, 1);
        m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
        rcvdata.sMsgBuf = NULL;
        return;

    }
    m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
    rcvdata.sMsgBuf = NULL;
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

// ������: OnMsgReportBpc
    // ���  : ������ 2015-4-23 16:01:43
    // ����  : virtual void 
    // ����  : S_BPC_RSMSG &rcvdata
    // ����  : �����ϱ������Ϣ����������
void CDrebMsgThread::OnMsgReportBpc(S_BPC_RSMSG& rcvdata)
{
    //�����Ϣ
    CBF_Xml        pXml;

    if (!pXml.FromBuffer(rcvdata.sMsgBuf->sBuffer))
    {
        m_pDrebApi->GetLogPoint()->LogMp(LOG_ERROR, __FILE__, __LINE__, "API�����ķ�xml��ʽ");
        m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
        rcvdata.sMsgBuf = NULL;
        return;
    }
    //�����Ϣ
    int i;
    pXml.SetNodeValueByPath("Monitor/�����ڵ�", false, (int)m_pRes->g_nSvrMainId);
    pXml.SetNodeValueByPath("Monitor/˽�нڵ�", false, (int)m_pRes->g_nSvrPrivateId);
    pXml.SetNodeValueByPath("Monitor/�ڵ�����", false, 2);//��bpc
    pXml.SetNodeValueByPath("Monitor/��������", false, 0);//����
    pXml.SetNodeValueByPath("Monitor/����ʱ��", false, m_pRes->g_sStartDateTime, false);

    int nConntotal = 0;
    int bufsize=0;
    int nConnnum = m_pAio->GetConnectPoolInfo(nConntotal);

    pXml.SetNodeAttribute("Monitor/���ӳ�", false, "���������", false, nConntotal);
    pXml.SetNodeAttribute("Monitor/���ӳ�", false, "����������", false, nConnnum);
    pXml.SetNodeAttribute("Monitor/���ӳ�", false, "�ͻ�������", false, nConntotal - nConnnum - 1);
    char extdata[300];
    sprintf(extdata, "���ӳ����������:%d ����������%d �ͻ�������%d", nConntotal, nConnnum, nConntotal - nConnnum - 1);
    m_pDrebApi->GetBufferPoolInfo(nConntotal, nConnnum, bufsize);
    pXml.SetNodeAttribute("Monitor/�ڴ滺���", false, "�ܷ���", false, nConntotal);
    pXml.SetNodeAttribute("Monitor/�ڴ滺���", false, "δʹ��", false, nConnnum);
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
   
    //������Ϣ
    char sip[30];
    int port;
    int drebid;
    int drebpid;
    char status[10];

    for (i = 0; i < m_pRes->g_vDrebLinkInfo.size(); i++)
    {
        CXmlNode* connnode = pXml.AddNodeByPath("Monitor/������Ϣ", false, "����", false, "");
        if (connnode == NULL)
        {
            m_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, "AddNodeByPath ����");
            continue;
        }
        if (m_pDrebApi->GetDrebInfo(i, sip, port, drebid, drebpid, status))
        {
            connnode->SetAttribute("ip", false,sip);
            connnode->SetAttribute("port", false, port);
            connnode->SetAttribute("index", false, i);
            connnode->SetAttribute("�����ڵ�", false, drebid);
            connnode->SetAttribute("˽�нڵ�", false, drebpid);
            connnode->SetAttribute("status", false, status);
        }
    }
    pXml.SetNodeValueByPath("Monitor/��չ��Ϣ/cgate", false, extdata, false);
    pXml.SetNodeAttribute("Monitor/��չ��Ϣ/cgate", false, "status", false, 0);
    int len = BPUDATASIZE;
    if (!pXml.ToBuffer(rcvdata.sMsgBuf->sBuffer, len))
    {
        m_pDrebApi->GetLogPoint()->LogMp(LOG_ERROR, __FILE__, __LINE__, "ToBuffer����");
        m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
        rcvdata.sMsgBuf = NULL;
        return;
    }
    rcvdata.sMsgBuf->sDBHead.cCmd = CMD_DPPUSH;
    rcvdata.sMsgBuf->sDBHead.cZip = 0;
    rcvdata.sMsgBuf->sDBHead.cDrebAffirm = 0;
    rcvdata.sMsgBuf->sDBHead.cNextFlag = 0; //�Ƿ���ȡ������
    rcvdata.sMsgBuf->sDBHead.cRaflag = 0;//����Ӧ���־

    rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_cNodePrivateId = m_pRes->g_nMonitorDrebPrivateId;
    rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_cSvrPrivateId = m_pRes->g_nMonitorSvrPrivateId;
    rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nNodeId = m_pRes->g_nMonitorDrebId;
    rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo = m_pRes->g_nMonitorTxCode;
    rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nSvrMainId = m_pRes->g_nMonitorSvrId;
    rcvdata.sMsgBuf->sDBHead.nLen = len;
    m_pDrebApi->GetLogPoint()->LogBin(LOG_DEBUG + 1, __FILE__, __LINE__, "���֪ͨ", rcvdata.sMsgBuf->sBuffer, rcvdata.sMsgBuf->sDBHead.nLen);
    //��������	
    rcvdata.sMsgBuf->sBpcHead.nIndex = 100;
    m_pDrebApi->SendMsg(rcvdata);
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
            data.sDBHead.cZip = 2;
            break;
        default:
            return false;
        }
    }

    return true;
}