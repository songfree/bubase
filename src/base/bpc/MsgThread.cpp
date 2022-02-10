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
	m_pFuncTbl = NULL;
	m_sThreadName,"CMsgThread";
	m_nDispatchNum =0;
	m_nBegin =0;
	m_nEnd =0;

	m_pLog = NULL;
}

CMsgThread::~CMsgThread()
{
	m_pLog = NULL;
	m_pRes = NULL;
	m_pSockMgr= NULL;
	m_pMemPool= NULL;//�ڴ�����;
	m_pPoolData= NULL;//�������ݶ���
	m_pFuncTbl= NULL;
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

bool CMsgThread::SetGlobalVar(CGResource *res,CPoolDataMsg *pooldata,CBF_BufferPool *mempool,CSocketMgr *sockmgr,CFuncTbl *tbl)
{
	m_pRes = res;
	m_pSockMgr = sockmgr;
	m_pMemPool = mempool;
	m_pPoolData = pooldata;
	m_pFuncTbl = tbl;

	m_pLog = &(m_pRes->m_log);
	
	
	return true;
}
int CMsgThread::Run()
{
	while (!m_pRes->g_bToExit)
	{
		if (m_pPoolData->GetData(m_pDataBuf))
		{

			//�ַ�����
			Dispatch();
		}
	}
	m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ֹͣ��Ϣ�����߳�");
	return 0;
}
void CMsgThread::Dispatch()
{
	int flag;
	S_FUNCINFO_TBL func;
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"��ʼ����[%d]",m_pDataBuf.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo);
	switch (m_pDataBuf.sMsgBuf->sBpcHead.cMsgType)
	{
		case MSG_TRANS:
			DispatchTrans(m_pDataBuf);
			break;
		case MSG_EXTCALL://bpu��������puthread���Ѿ������������
			DispatchExtCall(m_pDataBuf);
			break;
		case MSG_BPUREG://BPUע��
			DispatchBpuReg(m_pDataBuf);
			break;
		case MSG_BPCMONITOR:
			DispatchMonitor(m_pDataBuf);
			break;
		case MSG_REQ://���߹���������
			Dispatch2BpuGroup(m_pDataBuf);
			break;
		case MSG_GETNEXT://ȥbpuȡ������
			Dispatch2BpuGroupGetNext(m_pDataBuf);
			break;
		default:
			m_pMemPool->PoolFree(m_pDataBuf.sMsgBuf);
			m_pDataBuf.sMsgBuf = NULL;
			break;
	}
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���׷������");
	return;
}

void CMsgThread::DispatchExtCall(S_BPC_RSMSG &rcvdata)
{
	m_pLog->LogBin(LOG_DEBUG+2,__FILE__,__LINE__,"�յ�EXTCALL����",rcvdata.sMsgBuf->sBuffer,rcvdata.sMsgBuf->sDBHead.nLen);
	int index=0;
	int num=0;
	if (m_pRes->g_vDrebLinkInfo.size() < 1)
	{
		m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"��DREB����");
		if (rcvdata.sMsgBuf->sBpcHead.cMsgType == MSG_BPCMONITOR) //monitor
		{
			m_pMemPool->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
		}
		else
		{
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode == ERR_DREBROUTE;
			rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
			m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->SendMsg(rcvdata);
		}
		return;
	}
	//����Ҫ�޸�  20131024 ����dreb����״̬������
	index = GetDrebIndex(rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nNodeId,rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_cNodePrivateId);
	if (index == 0)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"û��DREB����");
		if (rcvdata.sMsgBuf->sBpcHead.cMsgType == MSG_BPCMONITOR) //monitor
		{
			m_pMemPool->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
		}
		else
		{
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode == ERR_DREBROUTE;
			rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
			m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->SendMsg(rcvdata);
		}
		return;
	}
	
	if (m_pSockMgr->AffirmIndex(index))
	{
		rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId = m_pSockMgr->at(index)->m_head.s_Sinfo.s_nNodeId;
		rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId = m_pSockMgr->at(index)->m_head.s_Sinfo.s_cNodePrivateId;
		if (rcvdata.sMsgBuf->sBpcHead.cMsgType != MSG_BPCMONITOR) //monitor
		{
			//b_Cinfo��b_cIndex���bpu����index,b_nSerial���bpu����indexʱ���
			rcvdata.sMsgBuf->sDBHead.b_Cinfo.b_nSerial = m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->m_nConntime;
			rcvdata.sMsgBuf->sDBHead.b_Cinfo.b_cIndex = (int )(rcvdata.sMsgBuf->sBpcHead.nBpuIndex);
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���bpuindex[%d %d]",rcvdata.sMsgBuf->sBpcHead.nBpuIndex,rcvdata.sMsgBuf->sDBHead.b_Cinfo.b_cIndex);
		}
		else
		{
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���ͼ��֪ͨ[%d]",index);
		}
		rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nTimestamp = m_pSockMgr->at(index)->m_head.s_Sinfo.s_nTimestamp;
		m_pSockMgr->at(index)->SendMsg(rcvdata);
		return;
	}
//	m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"EXTCALLȡdreb���Ӳ���");
	m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"EXTCALLȡdreb���Ӳ��� DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Դ[%d %d] cZip[%d] ҵ�����ݳ���[%d]",\
		GetDrebCmdType(rcvdata.sMsgBuf->sDBHead.cCmd).c_str(),rcvdata.sMsgBuf->sDBHead.cNextFlag,\
		rcvdata.sMsgBuf->sDBHead.cRaflag,rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
		rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
		rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
		rcvdata.sMsgBuf->sDBHead.cZip,rcvdata.sMsgBuf->sDBHead.nLen);

	m_pRes->m_pBpcCallDataLog.LogBpc(LOG_ERROR,rcvdata.sMsgBuf,true);

	m_pMemPool->PoolFree(rcvdata.sMsgBuf);
	rcvdata.sMsgBuf = NULL;
	
}
void CMsgThread::DispatchTrans(S_BPC_RSMSG &rcvdata)
{
	m_pLog->LogBin(LOG_DEBUG+2,__FILE__,__LINE__,"�յ�TRANS����",rcvdata.sMsgBuf->sBuffer,rcvdata.sMsgBuf->sDBHead.nLen);
	int index=0;
	int num=0;
	bool isfind=false;
	//songfree 20180328 add ת��ʹ��a_cNodePrivateId����ʶ�������յ��жϴ˱�ʶ������s_info��������
	rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_cNodePrivateId = 100;
	//add end

	if (m_pRes->g_vDrebLinkInfo.size() < 1)
	{
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode == ERR_DREBROUTE;
		rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->SendMsg(rcvdata);
		return;
	}
	 //����Ҫ�޸�  20131024 ����dreb����״̬������
	index = GetDrebIndex(rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nNodeId,rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_cNodePrivateId);
	
	if (index == 0)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"û��DREB����");
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode == ERR_DREBROUTE;
		rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->SendMsg(rcvdata);
		return;
	}
	if (m_pSockMgr->AffirmIndex(index))
	{
		m_pSockMgr->at(index)->SendMsg(rcvdata);
		return;
	}
//	m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"TRANSȡdreb���Ӳ���");
	m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"TRANSȡdreb���Ӳ��� DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Դ[%d %d] cZip[%d] ҵ�����ݳ���[%d]",\
		GetDrebCmdType(rcvdata.sMsgBuf->sDBHead.cCmd).c_str(),rcvdata.sMsgBuf->sDBHead.cNextFlag,\
		rcvdata.sMsgBuf->sDBHead.cRaflag,rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
						rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
						rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
						rcvdata.sMsgBuf->sDBHead.cZip,rcvdata.sMsgBuf->sDBHead.nLen);
	m_pRes->m_pBpcCallDataLog.LogBpc(LOG_ERROR,rcvdata.sMsgBuf,true);
	m_pMemPool->PoolFree(rcvdata.sMsgBuf);
	rcvdata.sMsgBuf = NULL;
	
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

void CMsgThread::DispatchMonitor(S_BPC_RSMSG &rcvdata)
{
// 	if (strlen(rcvdata.sBpuGroupName)>0)
// 	{
// 		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���ñ�BPC�µļ�ط���");
// 		
// 		DispatchBpu(rcvdata);//��BPC�µļ�ط���
// 	}
// 	else
// 	{
		DispatchExtCall(rcvdata);
//	}
}
void CMsgThread::DispatchBpuReg(S_BPC_RSMSG &rcvdata)
{
	//ע�Ṧ��
	S_BPU_FUNC_REG *regdata=NULL;
	S_BPU_FUNCINFO *funcdata=NULL;
	
	if (m_pRes->g_nRegisterMode == REGISTERMODE_STATIC  )
	{
		//��̬ע��ֱ�ӷ���
		m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"BPC��������BPUע��");
		rcvdata.sMsgBuf->sDBHead.nLen = 0;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPC_REGTX;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->SendMsg(rcvdata);
		return;
	}
	regdata = (S_BPU_FUNC_REG *)rcvdata.sMsgBuf->sBuffer;
	if (rcvdata.sMsgBuf->sBpcHead.nMsgFlag != LMSGID)
	{
		m_pBpcEndian.SetCommEndian(false);
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"BPUΪ������");
	}
	else
	{
		m_pBpcEndian.SetCommEndian(true);
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"BPUΪ������");
	}
	if (strncmp(rcvdata.sBpuGroupName,regdata->sBpuGroupName,20)!=0)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"����ע��ʱBPU�鲻�� [%s][%s]",regdata->sBpuGroupName,rcvdata.sBpuGroupName);
		rcvdata.sMsgBuf->sDBHead.nLen = 0;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPC_REGTX;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->m_bChecked = true;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->SendMsg(rcvdata);
		return;
	}

	m_pBpcEndian.Endian2LocalHost((unsigned char *)&(regdata->nFuncNum),sizeof(regdata->nFuncNum));
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"������[%d] BPC���н�����[%d]",regdata->nFuncNum,m_pFuncTbl->Size());
	
	int datalen = sizeof(S_BPU_FUNC_REG)+(regdata->nFuncNum-1)*sizeof(S_BPU_FUNCINFO);
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"������[%d]����[%d] bpu��������[%d]",regdata->nFuncNum,datalen,rcvdata.sMsgBuf->sDBHead.nLen);
	if (datalen != rcvdata.sMsgBuf->sDBHead.nLen)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"����ע��ʱDREBHEAD nLen���Ȳ��� bpu��=%s drebnlen=%d  txnum=%d",regdata->sBpuGroupName,rcvdata.sMsgBuf->sDBHead.nLen,regdata->nFuncNum);
		rcvdata.sMsgBuf->sDBHead.nLen = 0;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPC_REGTX;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->SendMsg(rcvdata);
		return;
	}
	int len = sizeof(S_BPU_FUNC_REG)-sizeof(S_BPU_FUNCINFO);
	funcdata = (S_BPU_FUNCINFO *)(rcvdata.sMsgBuf->sBuffer+len );
	S_FUNCINFO_TBL fn;
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"ע�ύ�� BPU��[%s]",rcvdata.sBpuGroupName);
	for (int i=0 ; i<regdata->nFuncNum ; i++)
	{
		m_pBpcEndian.Endian2LocalHost((unsigned char *)&(funcdata->nFuncNo),sizeof(funcdata->nFuncNo));
		memset(&fn,0,sizeof(S_FUNCINFO_TBL));
		strcpy(fn.sBpuGroupName ,rcvdata.sBpuGroupName);
		fn.nFuncNo = funcdata->nFuncNo;
		CBF_Tools::StringCopy(fn.sFuncName,40,funcdata->sFuncName);
//		memcpy(fn.sFuncName , funcdata->sFuncName,40);
		
		fn.cServiceFlag = funcdata->cServiceFlag;
		fn.cPrio = funcdata->cPrio;
		fn.nCallNum = 0;
		fn.nBpuGroupIndex = rcvdata.nBpuGroupIndex;
//		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"ע�ύ�� %d %s %d %d",fn.nFuncNo,fn.sFuncName,fn.cPrio,fn.cServiceFlag);
		if (!m_pFuncTbl->Insert(fn))
		{
			m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"����ע��ʱ����%s:%d�Ѵ���",fn.sBpuGroupName,fn.nFuncNo);
		}
		len += sizeof(S_BPU_FUNCINFO);
		funcdata = (S_BPU_FUNCINFO *)(rcvdata.sMsgBuf->sBuffer+len);
	}
	rcvdata.sMsgBuf->sDBHead.nLen = 0;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
	rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
	rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
	m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->m_bChecked = true;
	m_pRes->g_vBpuGroupInfo[m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->m_nBpuGroupIndex].g_bIsReg = true;
	m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->SendMsg(rcvdata);
	
	
	if (regdata->nFuncNum>0)
	{
		if  (m_pRes->g_nRegisterMode != REGISTERMODE_NONE) 
		{
			//��DREB��ע��
			RegisterDreb();
		}
		
	}
	return;
}
void CMsgThread::RegisterDreb()
{
	S_BPC_RSMSG rcvdata;
	S_FUNCINFO_TBL fc;
	S_SERVICEREG_MSG *data=NULL;
	unsigned int *func=NULL;
	bool bRet;
	int index;
	int  offset;
	int j;
	vector<S_FUNCINFO_TBL>vfunclist;
	if (m_pFuncTbl->GetAllFunc(vfunclist)<1)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"û��ע�Ṧ��");
		rcvdata.sMsgBuf = NULL;
		return;
	}
	//ǰ2��Ϊ����
	for (int i=0;i<m_pRes->g_vDrebLinkInfo.size();i++)
	{
		index = i+2;
		if (m_pSockMgr->at(index)->m_sock == INVALID_SOCKET || !m_pSockMgr->at(index)->m_bChecked )
		{
			continue;
		}
		rcvdata.sMsgBuf = (PBPCCOMMSTRU )m_pMemPool->PoolMalloc();
		if (rcvdata.sMsgBuf == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"������Ϣ�ռ����");
			return ;
		}
		//
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"��ʼ��DREBע������� ip[%s] port[%d] index[%d] [%d:%d] ��������[%d]",\
			m_pSockMgr->at(index)->m_sDrebIp.c_str(),m_pSockMgr->at(index)->m_nDrebPort,index,\
			m_pSockMgr->at(index)->m_nDrebId,m_pSockMgr->at(index)->m_nDrebPrivateId,m_pFuncTbl->Size());
		rcvdata.sMsgBuf->sDBHead.cCmd = CMD_REGSERVICE;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 0;
		rcvdata.sMsgBuf->sDBHead.cNextFlag = 0;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;

		for (j=0 ; j< vfunclist.size() ; j++)
		{
			if (j == 0)
			{
				data = (S_SERVICEREG_MSG *)rcvdata.sMsgBuf->sBuffer;
				data->nSvrMainId = m_pRes->g_nSvrMainId;
				m_pDrebEndian.Endian2Comm((unsigned char *)&(data->nSvrMainId),sizeof(data->nSvrMainId));
				data->cSvrPrivateId = m_pRes->g_nSvrPrivateId;
				data->nFuncNum = 1;
				data->nFuncNo = vfunclist[j].nFuncNo;
				m_pDrebEndian.Endian2Comm((unsigned char *)&(data->nFuncNo),sizeof(data->nFuncNo));
				offset = sizeof(S_SERVICEREG_MSG);
			}
			else
			{
				func = (unsigned int *)(rcvdata.sMsgBuf->sBuffer+offset);
				*func = vfunclist[j].nFuncNo;
				m_pDrebEndian.Endian2Comm((unsigned char *)func,sizeof(unsigned int));
				data->nFuncNum++;
				offset += sizeof(unsigned int);
				if (offset> BPUDATASIZE-30)
				{
					m_pMemPool->PoolFree(rcvdata.sMsgBuf);
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"ע�Ṧ��̫��,һ�����ݰ��޷���DREB��ע�ᣬ����ٹ�����");
					rcvdata.sMsgBuf = NULL;
					return;
				}
			}
		}
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ܹ���%d������ע��",data->nFuncNum);
		m_pDrebEndian.Endian2Comm((unsigned char *)&(data->nFuncNum),sizeof(data->nFuncNum));
		rcvdata.sMsgBuf->sDBHead.nLen = offset;
		m_pSockMgr->at(index)->SendMsg(rcvdata);
	}
}

int CMsgThread::GetDrebIndex(int nodeid, int nodepid)
{
	int i,j,ret;
	int index=-1;
	int num;
	vector<int>m_vDrebIndex;

	for (i=0 ; i<m_pRes->g_vDrebLinkInfo.size() ; i++)
	{
		if (m_pSockMgr->at(i+2)->m_bChecked)
		{
			m_vDrebIndex.push_back(i+2);
		}
	}
	if (m_vDrebIndex.size()<1)
	{
		return -1;
	}
	if (m_vDrebIndex.size() == 1)
	{
		return m_vDrebIndex[0];
	}
	else
	{	
		if (nodepid != 0)
		{
			for (j=0;j<m_vDrebIndex.size();j++)
			{
				index = m_vDrebIndex[j];	
				if (m_pSockMgr->at(index)->m_head.s_Sinfo.s_nNodeId == nodeid \
					&& m_pSockMgr->at(index)->m_head.s_Sinfo.s_cNodePrivateId == nodepid)
				{
					return index;
				}
			}
		}
		else //˽�нڵ�Ϊ0��ֻҪ�ҹ����ڵ㼴��
		{
			vector<int>drebindexlist;
			for (j=0;j<m_vDrebIndex.size();j++)
			{
				index = m_vDrebIndex[j];	
				if (m_pSockMgr->at(index)->m_head.s_Sinfo.s_nNodeId == nodeid)
				{
					drebindexlist.push_back(index);
				}
			}
			num = drebindexlist.size();
			if (num<1) //�������ӵ�dreb�� ���ȡһ����
			{
				srand(m_datetime.GetTickCount());
				//i = rand() %(���-��С+1) + ��С
				index = m_vDrebIndex[rand() % (m_vDrebIndex.size())];
			}
			else if (num == 1) //����ȡһ��
			{
				index = m_vDrebIndex[0];
			}
			else
			{
				srand(m_datetime.GetTickCount());
				//i = rand() %(���-��С+1) + ��С
				index = m_vDrebIndex[rand() % (num)];
			}
		}
	}
	return index;
}
void CMsgThread::Dispatch2BpuGroupGetNext(S_BPC_RSMSG &rcvdata)
{
	int flag=0;
	S_FUNCINFO_TBL fn;
	//ȡ���׵����ȼ�,�ҵ����׶�Ӧ��BPU�飬Ȼ�󽻸���BPU��,�������
	bool isfind = m_pFuncTbl->Select(rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,fn);
	if (!isfind) //û���ҵ�
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�޴˹��� ��Ϣ[%s]  DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Դ[%d %d] cZip[%d] ҵ�����ݳ���[%d]",\
			m_pRes->GetBpcMsgType(rcvdata.sMsgBuf->sBpcHead.cMsgType).c_str(),\
			m_pRes->GetDrebCmdType(rcvdata.sMsgBuf->sDBHead.cCmd).c_str(),rcvdata.sMsgBuf->sDBHead.cNextFlag,\
			rcvdata.sMsgBuf->sDBHead.cRaflag,rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
			rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
			rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
			rcvdata.sMsgBuf->sDBHead.cZip,rcvdata.sMsgBuf->sDBHead.nLen);
		
		if (rcvdata.sMsgBuf->sDBHead.cCmd == CMD_DPPUSH || rcvdata.sMsgBuf->sDBHead.cCmd == CMD_DPABC)
		{
			m_pMemPool->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
		}
		else
		{
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_FUNCNOTFUND;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.nLen = 0;
			m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
		}
		return;
	}
	
	strcpy(rcvdata.sBpuGroupName,fn.sBpuGroupName);
	rcvdata.nRtime = time(NULL);
	//20151102 ֱ�ӷŵ�BPU��Ӧ���̶߳���
	if (m_pRes->PushData(fn.nBpuGroupIndex,rcvdata,fn.cPrio) != 0)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"��BPU����г��� ��Ϣ[%s]  DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Դ[%d %d] cZip[%d] ҵ�����ݳ���[%d]",\
			m_pRes->GetBpcMsgType(rcvdata.sMsgBuf->sBpcHead.cMsgType).c_str(),\
			m_pRes->GetDrebCmdType(rcvdata.sMsgBuf->sDBHead.cCmd).c_str(),rcvdata.sMsgBuf->sDBHead.cNextFlag,\
			rcvdata.sMsgBuf->sDBHead.cRaflag,rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
			rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
			rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
			rcvdata.sMsgBuf->sDBHead.cZip,rcvdata.sMsgBuf->sDBHead.nLen);
		m_pMemPool->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
	}
	return;
}
void CMsgThread::Dispatch2BpuGroup(S_BPC_RSMSG &rcvdata)
{
	int flag=0;
	S_FUNCINFO_TBL fn;
	//ȡ���׵����ȼ�,Ȼ��������
	fn = m_pFuncTbl->FindFunc(rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,flag);
	if (flag == 0) //û���ҵ�
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�޴˹��� ��Ϣ[%s]  DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Դ[%d %d] cZip[%d] ҵ�����ݳ���[%d]",\
			m_pRes->GetBpcMsgType(rcvdata.sMsgBuf->sBpcHead.cMsgType).c_str(),\
			m_pRes->GetDrebCmdType(rcvdata.sMsgBuf->sDBHead.cCmd).c_str(),rcvdata.sMsgBuf->sDBHead.cNextFlag,\
			rcvdata.sMsgBuf->sDBHead.cRaflag,rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
			rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
			rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
			rcvdata.sMsgBuf->sDBHead.cZip,rcvdata.sMsgBuf->sDBHead.nLen);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_FUNCNOTFUND;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.nLen = 0;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
		return;
	}
	
	rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN+rcvdata.sMsgBuf->sDBHead.nLen;
	rcvdata.sMsgBuf->sBpcHead.cMsgType = MSG_REQ;
	strcpy(rcvdata.sBpuGroupName,fn.sBpuGroupName);
	
	rcvdata.nRtime = time(NULL);
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"BPU��[%s] ��������[%s] ���ȼ�[%d] ��Ϣ[%s]  DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Դ[%d %d] cZip[%d] ҵ�����ݳ���[%d]",\
		fn.sBpuGroupName,fn.sFuncName,fn.cPrio,m_pRes->GetBpcMsgType(rcvdata.sMsgBuf->sBpcHead.cMsgType).c_str(),\
		m_pRes->GetDrebCmdType(rcvdata.sMsgBuf->sDBHead.cCmd).c_str(),rcvdata.sMsgBuf->sDBHead.cNextFlag,\
		rcvdata.sMsgBuf->sDBHead.cRaflag,rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
		rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
		rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
		rcvdata.sMsgBuf->sDBHead.cZip,rcvdata.sMsgBuf->sDBHead.nLen);
	if (fn.cPrio<0 || fn.cPrio>2)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"BPU��[%s] ��������[%s] ���ȼ�[%d] ����",\
			fn.sBpuGroupName,fn.sFuncName,fn.cPrio);
		m_pMemPool->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return ;
	}
	//20151102 ֱ�ӷŵ�BPU��Ӧ���̶߳���
	if (m_pRes->PushData(fn.nBpuGroupIndex,rcvdata,fn.cPrio) != 0)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"��BPU����г��� ��Ϣ[%s]  DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Դ[%d %d] cZip[%d] ҵ�����ݳ���[%d]",\
			m_pRes->GetBpcMsgType(rcvdata.sMsgBuf->sBpcHead.cMsgType).c_str(),\
			m_pRes->GetDrebCmdType(rcvdata.sMsgBuf->sDBHead.cCmd).c_str(),rcvdata.sMsgBuf->sDBHead.cNextFlag,\
			rcvdata.sMsgBuf->sDBHead.cRaflag,rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
			rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
			rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
			rcvdata.sMsgBuf->sDBHead.cZip,rcvdata.sMsgBuf->sDBHead.nLen);
		m_pMemPool->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
	}
	return;
}
