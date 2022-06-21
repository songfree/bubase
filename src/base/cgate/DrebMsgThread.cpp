// DrebMsgThread.cpp: implementation of the CDrebMsgThread class.
//
//////////////////////////////////////////////////////////////////////

#include "DrebMsgThread.h"
#include "GateResource.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern int   g_pRunFlag;

CDrebMsgThread::CDrebMsgThread()
{
	m_pLog = NULL;
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
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"Recv DREB connect rsp��subscribe broadcast");
#else
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�յ�DREB���ӳɹ�����Ӧ �����߶��Ĺ㲥");
#endif
	m_pDrebApi->Subscribe(rcvdata.index,m_lBcRegister);
	
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

    data.data.head.stDest.nSerial = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial; //����ģʽ�£���д�����˻���id (�޶������˻�Ϊ��������)
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
         //���������Ƿ������µģ�������ֱ�Ӷ���������ֱ��������������ݶ������
        if (data.data.head.stDest.nSerial <= 0)  //����������㲥��
        {
            data.isBC = 1;
            data.index = 0;
            data.timestamp = 0;
            //��������Ϊ������Ϣ     �����ж������Ƿ����£�api�Ѿ�ȥ����
            data.nVerietyId = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook;
            m_pSendData->PushData(data, 2);
            
        }
        else
        {
            data.isBC = 2;    //��ʾ���ĵ���Ϣ  �����߳�Ҫ�ж��Ƿ���
            data.index = 0;
            data.timestamp = 0;
            //��������Ϊ������Ϣ     �����ж������Ƿ����£�api�Ѿ�ȥ����
            data.nVerietyId = 0;
            m_pSendData->PushData(data, 1);
        }
        m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
        rcvdata.sMsgBuf = NULL;
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
