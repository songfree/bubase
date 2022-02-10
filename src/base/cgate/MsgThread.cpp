// MsgThread.cpp: implementation of the CMsgThread class.
//
//////////////////////////////////////////////////////////////////////

#include "MsgThread.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMsgThread::CMsgThread()
{
	m_pRes = NULL;
	m_pSockMgr = NULL;
	m_pMemPool = NULL;
	m_pPoolData = NULL;
	m_sThreadName,"CMsgThread";
	m_nDispatchNum =0;
	m_nDrebNum = 0;
}

CMsgThread::~CMsgThread()
{

}
std::string CMsgThread::GetDrebCmdType(int cmdtype)
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
	case  CMD_MONITOR_BPC:     //15 //�������߽ڵ���
		return "CMD_MONITOR_BPC";
	default:
		return "";
	}
}
void CMsgThread::ExitThreadInstance()
{
	return ;
}
bool CMsgThread::InitThreadInstance()
{
	return true;
}
bool CMsgThread::SetGlobalVar(CGateResource *res,CPoolData *pooldata,CBF_BufferPool *mempool,CSocketMgr *sockmgr)
{
	m_pRes = res;
	m_pSockMgr = sockmgr;
	m_pMemPool = mempool;
	m_pPoolData = pooldata;

	m_pLog = &(m_pRes->g_pLog);
	
	m_nDrebNum = m_pRes->g_vDrebLinkInfo.size();
	m_pDesZip.SetLogPara(m_pLog);

	m_pDrebLog = &(m_pRes->g_pDrebLog);
	return true;
}
int CMsgThread::Run()
{
	m_nDispatchNum = 0;
	m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"������Ϣ�����߳�");
	while (!m_pRes->g_bIsExit)
	{
		if (m_pPoolData->GetData(m_pDataBuf))
		{
			//�ַ�����
			Dispatch();
			m_nDispatchNum++;
			if (m_nDispatchNum >= m_nDrebNum)
			{
				m_nDispatchNum = 0; 
			}
		}
	}
	m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"ֹͣ��Ϣ�����߳�");
	return 0;
}
void CMsgThread::Dispatch()
{
	int btime = time(NULL);
	while (!m_pRes->g_bIsExit)
	{
		if (m_pSockMgr->at(m_nDispatchNum)->m_sock != INVALID_SOCKET && m_pSockMgr->at(m_nDispatchNum)->m_bChecked)
		{
			m_pDataBuf.totallen = m_pDataBuf.data->head.nLen + DREBHEADLEN;
			m_pDataBuf.sendlen = m_pDataBuf.totallen;
			if (m_pDataBuf.nIndex == 0)
			{
				m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���������  ���� dreb-index[%d]",\
					m_nDispatchNum);
			}
			else
			{
				m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���ͻ������� index[%d] ��ʶ[%d] ���� dreb-index[%d]",\
				m_pDataBuf.nIndex,m_pDataBuf.serial,m_nDispatchNum);
			}
			
			if (m_pSockMgr->at(m_nDispatchNum)->Send(m_pDataBuf) == 0)
			{
				if (m_pDataBuf.nIndex == 0)
				{
					m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"��� ������󷢸� dreb-index[%d] ",\
						m_nDispatchNum);
				}
				else
				{
					m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"��� �ͻ������� index[%d] ��ʶ[%d] ���� dreb-index[%d]",\
						m_pDataBuf.nIndex,m_pDataBuf.serial,m_nDispatchNum);
				}
			}
			return ;
		}
		else
		{
			m_nDispatchNum++;
			if (m_nDispatchNum >= m_nDrebNum)
			{
				m_nDispatchNum = 0; 
			}
		}
		if (time(NULL) - btime > m_pRes->g_nDispatchTime)
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"������䳬ʱ,dreb����ȫ�Ͽ� %d",m_pDataBuf.serial);
			m_pLog->LogBin(LOG_ERROR_FAULT,__FILE__,__LINE__,"������䳬ʱ",m_pDataBuf.data->buffer,m_pDataBuf.data->head.nLen);
			m_pMemPool->PoolFree(m_pDataBuf.data);
			m_pDataBuf.data = NULL;
			return;
		}
#ifdef _WINDOWS
		SLEEP(5);
#else
		usleep(5);
#endif
	}
	return;
}

void CMsgThread::LogDrebHead(int loglevel, DREB_HEAD head, const char *msg, const char *filename, int fileline)
{
	bzero(m_headMsg,sizeof(m_headMsg));
	sprintf(m_headMsg,"%s DREBHEAD��Ϣ cZip=%d cCmd=%s cRaflag=%d cNextFlag=%d cDrebAffirm=%d s_nNodeId=%d \
	   s_cNodePrivateId=%d s_nSvrMainId=%d s_cSvrPrivateId=%d s_nHook=%d s_nSerial=%d \
	   s_nDrebSerial=%d s_nIndex=%d d_nNodeId=%d d_cNodePrivateId=%d d_nSvrMainId=%d \
	   d_cSvrPrivateId=%d d_nServiceNo=%d a_nNodeId=%d a_cNodePrivateId=%d a_nRetCode=%d \
	   n_nNextNo=%d n_nNextOffset=%d b_nSerial=%d b_cIndex=%d nLen=%d","%s",head.cZip,GetDrebCmdType(head.cCmd).c_str(),\
	   head.cRaflag,head.cNextFlag,head.cDrebAffirm,head.s_Sinfo.s_nNodeId,head.s_Sinfo.s_cNodePrivateId,\
	   head.s_Sinfo.s_nSvrMainId,head.s_Sinfo.s_cSvrPrivateId,head.s_Sinfo.s_nHook,head.s_Sinfo.s_nSerial,\
	   head.s_Sinfo.s_nDrebSerial,head.s_Sinfo.s_nIndex,head.d_Dinfo.d_nNodeId,head.d_Dinfo.d_cNodePrivateId,\
	   head.d_Dinfo.d_nSvrMainId,head.d_Dinfo.d_cSvrPrivateId,head.d_Dinfo.d_nServiceNo,\
	   head.a_Ainfo.a_nNodeId,head.a_Ainfo.a_cNodePrivateId,head.a_Ainfo.a_nRetCode,\
	   head.n_Ninfo.n_nNextNo,head.n_Ninfo.n_nNextOffset,head.b_Cinfo.b_nSerial,\
	   head.b_Cinfo.b_cIndex,head.nLen);
	m_pLog->LogMp(loglevel,filename,fileline,m_headMsg,msg);
}
