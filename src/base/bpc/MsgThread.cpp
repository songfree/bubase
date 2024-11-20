// MsgThread.cpp: implementation of the CMsgThread class.
//
//////////////////////////////////////////////////////////////////////

#include "MsgThread.h"
#include "DrebMsgProcBase.h"

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
	m_pDrebApi=NULL;
	m_pLog = NULL;
}

CMsgThread::~CMsgThread()
{
	m_pLog = NULL;
	m_pRes = NULL;
	m_pSockMgr= NULL;
	m_pMemPool= NULL;//内存分配池;
	m_pPoolData= NULL;//处理数据队列
	m_pFuncTbl= NULL;
}

void CMsgThread::ExitThreadInstance()
{
	return ;
}
bool CMsgThread::InitThreadInstance()
{
	return true;
}

bool CMsgThread::SetGlobalVar(CGResource *res,CPoolDataMsg *pooldata,CBF_BufferPool *mempool,CSocketMgr *sockmgr,CFuncTbl *tbl,CBF_DrebServer *api)
{
	m_pRes = res;
	m_pSockMgr = sockmgr;
	m_pMemPool = mempool;
	m_pPoolData = pooldata;
	m_pFuncTbl = tbl;
	m_pLog = m_pRes->m_log;
	m_pDrebApi = api;
	return true;
}
int CMsgThread::Run()
{
	while (!m_pRes->g_bToExit)
	{
		if (m_pPoolData->GetData(m_pDataBuf))
		{
			//分发数据
			Dispatch();
		}
	}
	m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"停止消息处理线程");
	return 0;
}
void CMsgThread::Dispatch()
{
	int flag;
	S_FUNCINFO_TBL func;
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"开始分派[%d]",m_pDataBuf.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo);
	switch (m_pDataBuf.sMsgBuf->sBpcHead.cMsgType)
	{
		case MSG_TRANS:
			DispatchTrans(m_pDataBuf);
			break;
		case MSG_EXTCALL://bpu过来，在puthread里已经进行区别调用
			DispatchExtCall(m_pDataBuf);
			break;
		case MSG_BPUREG://BPU注册
			DispatchBpuReg(m_pDataBuf);
			break;
		case MSG_BPCMONITOR:
			DispatchMonitor(m_pDataBuf);
			break;
		case MSG_REQ://总线过来的请求
			Dispatch2BpuGroup(m_pDataBuf);
			break;
		case MSG_GETNEXT://去bpu取后续包
			Dispatch2BpuGroupGetNext(m_pDataBuf);
			break;
		default:
			m_pMemPool->PoolFree(m_pDataBuf.sMsgBuf);
			m_pDataBuf.sMsgBuf = NULL;
			break;
	}
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"交易分派完成");
	return;
}

void CMsgThread::DispatchExtCall(S_BPC_RSMSG &rcvdata)
{
	m_pLog->LogBin(LOG_DEBUG+2,__FILE__,__LINE__,"收到EXTCALL数据",rcvdata.sMsgBuf->sBuffer,rcvdata.sMsgBuf->sDBHead.nLen);
    if (rcvdata.sMsgBuf->sBpcHead.cMsgType != MSG_BPCMONITOR) //monitor
    {
        //b_Cinfo的b_cIndex存放bpu连接index,b_nSerial存放bpu连接index时间戳
        rcvdata.sMsgBuf->sDBHead.b_Cinfo.b_nSerial = m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->m_nConntime;
        rcvdata.sMsgBuf->sDBHead.b_Cinfo.b_cIndex = (int)(rcvdata.sMsgBuf->sBpcHead.nBpuIndex);
        m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "外调bpuindex[%d %d]", rcvdata.sMsgBuf->sBpcHead.nBpuIndex, rcvdata.sMsgBuf->sDBHead.b_Cinfo.b_cIndex);
    }
    else
    {
        m_pLog->LogMp(LOG_DEBUG, __FILE__, __LINE__, "发送监控通知");
    }
	int fundno = rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo;
	rcvdata.sMsgBuf->sBpcHead.nIndex=100;
	if (m_pDrebApi->SendMsg(rcvdata)<0)
	{
		m_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__,"发送功能号[%d]失败", fundno);
        m_pMemPool->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
	}
	return;
	
}
void CMsgThread::DispatchTrans(S_BPC_RSMSG &rcvdata)
{
	m_pLog->LogBin(LOG_DEBUG+2,__FILE__,__LINE__,"收到TRANS数据",rcvdata.sMsgBuf->sBuffer,rcvdata.sMsgBuf->sDBHead.nLen);
	rcvdata.sMsgBuf->sBpcHead.nIndex = 100;
	//songfree 20180328 add 转移使用a_cNodePrivateId来标识，总线收到判断此标识，不对s_info进行重置
	rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_cNodePrivateId = 100;
	//add end
	int fundno= rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo;
    rcvdata.sMsgBuf->sBpcHead.nIndex = 100;
    if (m_pDrebApi->SendMsg(rcvdata)<0)
    {
        m_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, "发送功能号[%d]失败", fundno);
        m_pMemPool->PoolFree(rcvdata.sMsgBuf);
        rcvdata.sMsgBuf = NULL;
    }
	
}
void CMsgThread::LogDrebHead(int loglevel, DREB_HEAD head, const char *msg, const char *filename, int fileline)
{
	bzero(m_headMsg,sizeof(m_headMsg));
	sprintf(m_headMsg,"%s DREBHEAD信息 cZip=%d cCmd=%s cRaflag=%d cNextFlag=%d cDrebAffirm=%d s_nNodeId=%d \
	   s_cNodePrivateId=%d s_nSvrMainId=%d s_cSvrPrivateId=%d s_nHook=%d s_nSerial=%d \
	   s_nDrebSerial=%d s_nIndex=%d d_nNodeId=%d d_cNodePrivateId=%d d_nSvrMainId=%d \
	   d_cSvrPrivateId=%d d_nServiceNo=%d a_nNodeId=%d a_cNodePrivateId=%d a_nRetCode=%d \
	   n_nNextNo=%d n_nNextOffset=%d b_nSerial=%d b_cIndex=%d nLen=%d","%s",head.cZip, CDrebMsgProcBase::GetDrebCmdType(head.cCmd).c_str(),\
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
// 		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"调用本BPC下的监控服务");
// 		
// 		DispatchBpu(rcvdata);//本BPC下的监控服务
// 	}
// 	else
// 	{
		DispatchExtCall(rcvdata);
//	}
}
void CMsgThread::DispatchBpuReg(S_BPC_RSMSG &rcvdata)
{
	//注册功能
	S_BPU_FUNC_REG *regdata=NULL;
	S_BPU_FUNCINFO *funcdata=NULL;
	int nRet;
	if (m_pRes->g_nRegisterMode == REGISTERMODE_STATIC  )
	{
		//静态注册直接返回
		m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"BPC配置无须BPU注册");
		rcvdata.sMsgBuf->sDBHead.nLen = 0;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPC_REGTX;
		nRet = m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->SendMsg(rcvdata);
        if (nRet == -101)
        {
            m_pMemPool->PoolFree(rcvdata.sMsgBuf);
            rcvdata.sMsgBuf = NULL;
        }
		return;
	}
	regdata = (S_BPU_FUNC_REG *)rcvdata.sMsgBuf->sBuffer;
	if (rcvdata.sMsgBuf->sBpcHead.nMsgFlag != LMSGID)
	{
		m_pBpcEndian.SetCommEndian(false);
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"BPU为网络序");
	}
	else
	{
		m_pBpcEndian.SetCommEndian(true);
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"BPU为主机序");
	}
	if (strncmp(rcvdata.sBpuGroupName,regdata->sBpuGroupName,20)!=0)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"功能注册时BPU组不符 [%s][%s]",regdata->sBpuGroupName,rcvdata.sBpuGroupName);
		rcvdata.sMsgBuf->sDBHead.nLen = 0;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPC_REGTX;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->m_bChecked = true;
		nRet = m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->SendMsg(rcvdata);
        if (nRet == -101)
        {
            m_pMemPool->PoolFree(rcvdata.sMsgBuf);
            rcvdata.sMsgBuf = NULL;
        }
		return;
	}

	m_pBpcEndian.Endian2LocalHost((unsigned char *)&(regdata->nFuncNum),sizeof(regdata->nFuncNum));
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"交易数[%d] BPC已有交易数[%d]",regdata->nFuncNum,m_pFuncTbl->Size());
	
	int datalen = sizeof(S_BPU_FUNC_REG)+(regdata->nFuncNum-1)*sizeof(S_BPU_FUNCINFO);
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"交易数[%d]长度[%d] bpu送来长度[%d]",regdata->nFuncNum,datalen,rcvdata.sMsgBuf->sDBHead.nLen);
	if (datalen != rcvdata.sMsgBuf->sDBHead.nLen)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"功能注册时DREBHEAD nLen长度不符 bpu组=%s drebnlen=%d  txnum=%d",regdata->sBpuGroupName,rcvdata.sMsgBuf->sDBHead.nLen,regdata->nFuncNum);
		rcvdata.sMsgBuf->sDBHead.nLen = 0;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPC_REGTX;
		nRet = m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->SendMsg(rcvdata);
        if (nRet == -101)
        {
            m_pMemPool->PoolFree(rcvdata.sMsgBuf);
            rcvdata.sMsgBuf = NULL;
        }
		return;
	}
	int len = sizeof(S_BPU_FUNC_REG)-sizeof(S_BPU_FUNCINFO);
	funcdata = (S_BPU_FUNCINFO *)(rcvdata.sMsgBuf->sBuffer+len );
	S_FUNCINFO_TBL fn;
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"注册交易 BPU组[%s]",rcvdata.sBpuGroupName);
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
//		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"注册交易 %d %s %d %d",fn.nFuncNo,fn.sFuncName,fn.cPrio,fn.cServiceFlag);
		if (!m_pFuncTbl->Insert(fn))
		{
			m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"交易注册时交易%s:%d已存在",fn.sBpuGroupName,fn.nFuncNo);
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
	nRet = m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->SendMsg(rcvdata);
    if (nRet == -101)
    {
        m_pMemPool->PoolFree(rcvdata.sMsgBuf);
        rcvdata.sMsgBuf = NULL;
    }
	
	if (regdata->nFuncNum>0)
	{
		if  (m_pRes->g_nRegisterMode != REGISTERMODE_NONE) 
		{
			//在DREB上注册
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
	std::vector<S_FUNCINFO_TBL>vfunclist;
	if (m_pFuncTbl->GetAllFunc(vfunclist)<1)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"没有注册功能");
		rcvdata.sMsgBuf = NULL;
		return;
	}
    char sip[40];
    char drebstatus[40];
    int  drebport;
    int  drebid;
    int  drebprivateid;
	for (int i=0;i<m_pRes->g_vDrebLinkInfo.size();i++)
	{
		index = i;
		rcvdata.sMsgBuf = (PBPCCOMMSTRU )m_pMemPool->PoolMalloc();
		if (rcvdata.sMsgBuf == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"分配消息空间出错");
			return ;
		}
        bzero(sip, sizeof(sip));
        bzero(drebstatus, sizeof(drebstatus));
        m_pDrebApi->GetDrebInfo(i, sip, drebport, drebid, drebprivateid, drebstatus);
		//if (strcmp(drebstatus, "正常") != 0)
		//{
		//	continue;
		//}
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"开始向DREB注册服务功能 ip[%s] port[%d] index[%d] [%d:%d] 交易总数[%d]",\
			sip, drebport,index,\
			drebid, drebprivateid,m_pFuncTbl->Size());
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
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"注册功能太多,一个数据包无法在DREB上注册，请减少功能数");
					rcvdata.sMsgBuf = NULL;
					return;
				}
			}
		}
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"总共有%d个功能注册",data->nFuncNum);
		m_pDrebEndian.Endian2Comm((unsigned char *)&(data->nFuncNum),sizeof(data->nFuncNum));
		rcvdata.sMsgBuf->sDBHead.nLen = offset;
		rcvdata.sMsgBuf->sBpcHead.nIndex = index;
		rcvdata.index = index;
		m_pDrebApi->SendMsg(rcvdata);
	}
}

void CMsgThread::Dispatch2BpuGroupGetNext(S_BPC_RSMSG &rcvdata)
{
	int flag=0;
	S_FUNCINFO_TBL fn;
	int nRet;
	//取交易的优先级,找到交易对应的BPU组，然后交给此BPU组,放入队列
	bool isfind = m_pFuncTbl->Select(rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,fn);
	if (!isfind) //没有找到
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"无此功能 消息[%s]  DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 源[%d %d] cZip[%d] 业务数据长度[%d]",\
			CDrebMsgProcBase::GetBpcMsgType(rcvdata.sMsgBuf->sBpcHead.cMsgType).c_str(),\
			CDrebMsgProcBase::GetDrebCmdType(rcvdata.sMsgBuf->sDBHead.cCmd).c_str(),rcvdata.sMsgBuf->sDBHead.cNextFlag,\
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
			nRet  = m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
            if (nRet == -101)
            {
                m_pMemPool->PoolFree(rcvdata.sMsgBuf);
                rcvdata.sMsgBuf = NULL;
            }
		}
		return;
	}
	
	strcpy(rcvdata.sBpuGroupName,fn.sBpuGroupName);
	rcvdata.nRtime = time(NULL);
	//20151102 直接放到BPU对应的线程队列
	if (m_pRes->PushData(fn.nBpuGroupIndex,rcvdata,fn.cPrio) != 0)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"入BPU组队列出错 消息[%s]  DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 源[%d %d] cZip[%d] 业务数据长度[%d]",\
			CDrebMsgProcBase::GetBpcMsgType(rcvdata.sMsgBuf->sBpcHead.cMsgType).c_str(),\
			CDrebMsgProcBase::GetDrebCmdType(rcvdata.sMsgBuf->sDBHead.cCmd).c_str(),rcvdata.sMsgBuf->sDBHead.cNextFlag,\
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
	int nRet ;
	//取交易的优先级,然后放入队列
	fn = m_pFuncTbl->FindFunc(rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,flag);
	if (flag == 0) //没有找到
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"无此功能 消息[%s]  DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 源[%d %d] cZip[%d] 业务数据长度[%d]",\
			CDrebMsgProcBase::GetBpcMsgType(rcvdata.sMsgBuf->sBpcHead.cMsgType).c_str(),\
			CDrebMsgProcBase::GetDrebCmdType(rcvdata.sMsgBuf->sDBHead.cCmd).c_str(),rcvdata.sMsgBuf->sDBHead.cNextFlag,\
			rcvdata.sMsgBuf->sDBHead.cRaflag,rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
			rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
			rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
			rcvdata.sMsgBuf->sDBHead.cZip,rcvdata.sMsgBuf->sDBHead.nLen);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_FUNCNOTFUND;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.nLen = 0;
		nRet = m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
        if (nRet == -101)
        {
            m_pMemPool->PoolFree(rcvdata.sMsgBuf);
            rcvdata.sMsgBuf = NULL;
        }
		return;
	}
	
	rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN+rcvdata.sMsgBuf->sDBHead.nLen;
	rcvdata.sMsgBuf->sBpcHead.cMsgType = MSG_REQ;
	strcpy(rcvdata.sBpuGroupName,fn.sBpuGroupName);
	
	rcvdata.nRtime = time(NULL);
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"BPU组[%s] 交易名称[%s] 优先级[%d] 消息[%s]  DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 源[%d %d] cZip[%d] 业务数据长度[%d]",\
		fn.sBpuGroupName,fn.sFuncName,fn.cPrio, CDrebMsgProcBase::GetBpcMsgType(rcvdata.sMsgBuf->sBpcHead.cMsgType).c_str(),\
		CDrebMsgProcBase::GetDrebCmdType(rcvdata.sMsgBuf->sDBHead.cCmd).c_str(),rcvdata.sMsgBuf->sDBHead.cNextFlag,\
		rcvdata.sMsgBuf->sDBHead.cRaflag,rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
		rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
		rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
		rcvdata.sMsgBuf->sDBHead.cZip,rcvdata.sMsgBuf->sDBHead.nLen);
	if (fn.cPrio<0 || fn.cPrio>2)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"BPU组[%s] 交易名称[%s] 优先级[%d] 不符",\
			fn.sBpuGroupName,fn.sFuncName,fn.cPrio);
		m_pMemPool->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		return ;
	}
	//20151102 直接放到BPU对应的线程队列
	if (m_pRes->PushData(fn.nBpuGroupIndex,rcvdata,fn.cPrio) != 0)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"入BPU组队列出错 消息[%s]  DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 源[%d %d] cZip[%d] 业务数据长度[%d]",\
			CDrebMsgProcBase::GetBpcMsgType(rcvdata.sMsgBuf->sBpcHead.cMsgType).c_str(),\
			CDrebMsgProcBase::GetDrebCmdType(rcvdata.sMsgBuf->sDBHead.cCmd).c_str(),rcvdata.sMsgBuf->sDBHead.cNextFlag,\
			rcvdata.sMsgBuf->sDBHead.cRaflag,rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
			rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
			rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
			rcvdata.sMsgBuf->sDBHead.cZip,rcvdata.sMsgBuf->sDBHead.nLen);
		m_pMemPool->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
	}
	return;
}
