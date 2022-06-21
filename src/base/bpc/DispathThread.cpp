// DispathThread.cpp: implementation of the CDispathThread class.
//
//////////////////////////////////////////////////////////////////////

#include "DispathThread.h"
#include "DrebMsgProcBase.h"


CDispathThread::CDispathThread()
{
	m_pRes = NULL;
	m_pSockMgr = NULL;
	m_pMemPool = NULL;
	m_pPoolData = NULL;
	m_pFuncTbl = NULL;
	m_sThreadName,"CDispathThread";
	m_nDispatchNum =0;
	m_nBegin =0;
	m_nEnd =0;

	m_pLog = NULL;
	m_nCurBpuIndex = 0;
}

CDispathThread::~CDispathThread()
{
	m_pLog = NULL;
	m_pRes = NULL;
	m_pSockMgr= NULL;
	m_pMemPool= NULL;//内存分配池;
	m_pPoolData= NULL;//处理数据队列
	m_pFuncTbl= NULL;
}

void CDispathThread::ExitThreadInstance()
{
	return ;
}
bool CDispathThread::InitThreadInstance()
{
	return true;
}

bool CDispathThread::SetGlobalVar(CGResource *res,CPoolData *pooldata,CBF_BufferPool *mempool,CSocketMgr *sockmgr,CFuncTbl *tbl)
{
	m_pRes = res;
	m_pSockMgr = sockmgr;
	m_pMemPool = mempool;
	m_pPoolData = pooldata;
	m_pFuncTbl = tbl;
	m_pLog = m_pRes->m_log;
	return true;
}
int CDispathThread::Run()
{
	while (!m_pRes->g_bToExit)
	{
		if (m_pPoolData->GetData(m_pDataBuf))
		{

			//分发数据
			Dispatch();
		}
	}
	m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"停止BPU组分配线程");
	return 0;
}
void CDispathThread::Dispatch()
{
	int flag;
	S_FUNCINFO_TBL func;
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"开始分派[%d]",m_pDataBuf.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo);
	switch (m_pDataBuf.sMsgBuf->sBpcHead.cMsgType)
	{
		case MSG_BPCCALL://bpu过来
		case MSG_REQ://dreb过来
		case MSG_BPCMONITOR: //监控
			DispatchBpu(m_pDataBuf);
			break;
		case MSG_GETNEXT://dreb过来
			DispatchBpu(m_pDataBuf);
			break;
		default:
			m_pMemPool->PoolFree(m_pDataBuf.sMsgBuf);
			m_pDataBuf.sMsgBuf = NULL;
			break;
	}
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"交易分派完成");
	return;
}
void CDispathThread::DispatchBpu(S_BPC_RSMSG &rcvdata)
{
	int nRet;
	int rtime= time(NULL);
	if (rtime - rcvdata.nRtime > m_pRes->g_nDispatchTime)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"请求在队列里超时[%d]秒 DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 源[%d %d] cZip[%d] 业务数据长度[%d]",\
			m_pRes->g_nDispatchTime, CDrebMsgProcBase::GetDrebCmdType(rcvdata.sMsgBuf->sDBHead.cCmd).c_str(),rcvdata.sMsgBuf->sDBHead.cNextFlag,\
			rcvdata.sMsgBuf->sDBHead.cRaflag,rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
						rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
						rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook,rcvdata.sMsgBuf->sDBHead.cZip,rcvdata.sMsgBuf->sDBHead.nLen);
		if (rcvdata.sMsgBuf->sBpcHead.cMsgType == MSG_REQ) //drebcall
		{
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cNextFlag = 0;
			rcvdata.sMsgBuf->sDBHead.nLen = 0;
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_TIMEOUT;
			m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
		}
		else if (rcvdata.sMsgBuf->sBpcHead.cMsgType == MSG_BPCMONITOR) //monitor
		{
			m_pMemPool->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
		}
		else //bpc call
		{
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cNextFlag = 0;
			rcvdata.sMsgBuf->sDBHead.nLen = 0;
			rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_TIMEOUT;
			m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->SendMsg(rcvdata);
		}

		return;
	}
	
	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"消息所在BPU组[%s %d] BPU连接范围[%d %d] BPU数目[%d] DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 源[%d %d] cZip[%d] 业务数据长度[%d]",\
		m_pRes->g_vBpuGroupInfo[m_nBpugroupindex].g_sBpuGroupName.c_str(),m_nBpugroupindex,\
		m_nBegin,m_nEnd,m_pRes->g_vBpuGroupInfo[m_nBpugroupindex].g_nBpuNum,\
		CDrebMsgProcBase::GetDrebCmdType(rcvdata.sMsgBuf->sDBHead.cCmd).c_str(),rcvdata.sMsgBuf->sDBHead.cNextFlag,\
		rcvdata.sMsgBuf->sDBHead.cRaflag,rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
		rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
		rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
		rcvdata.sMsgBuf->sDBHead.cZip,rcvdata.sMsgBuf->sDBHead.nLen);
	//分派
	if (m_pRes->g_vBpuGroupInfo[m_nBpugroupindex].g_nDispatchMode != 0)
	{
		while (time(NULL)-rtime < m_pRes->g_nDispatchTime)
		{
			if (m_pRes->g_bToExit)
			{
				return;
			}
			int i;
			//分派模式，一直分派，不管空不空闲
			for (i=m_nBegin;i<m_nEnd;i++)
			{
				if (m_pSockMgr->at(i)->m_sock != INVALID_SOCKET)
				{
					nRet = m_pSockMgr->at(i)->SendMsg(rcvdata,0);
					if (nRet == -101)  //数据解密解压缩出错
					{
						//应答回去
						if (rcvdata.sMsgBuf->sBpcHead.cMsgType == MSG_REQ)
						{
							rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
							rcvdata.sMsgBuf->sDBHead.cNextFlag = 0;
							rcvdata.sMsgBuf->sDBHead.nLen = 0;
							rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_DATA_ENCRYPTZIP;
							m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
						}
						else if (rcvdata.sMsgBuf->sBpcHead.cMsgType == MSG_BPCMONITOR) //monitor
						{
							m_pMemPool->PoolFree(rcvdata.sMsgBuf);
							rcvdata.sMsgBuf = NULL;
						}
						else //bpc call
						{
							rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
							rcvdata.sMsgBuf->sDBHead.cNextFlag = 0;
							rcvdata.sMsgBuf->sDBHead.nLen = 0;
							rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
							rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_DATA_ENCRYPTZIP;
							m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->SendMsg(rcvdata);
						}
					}
					return;
				}
			}
		}
	}
	else  //分派模式为哪个空闲分哪个
	{
		while (time(NULL)-rtime < m_pRes->g_nDispatchTime)
		{
			if (m_nCurBpuIndex< m_nBegin)
			{
				m_nCurBpuIndex = m_nBegin;
			}
			if (m_nCurBpuIndex >= m_nEnd)
			{
				m_nCurBpuIndex = m_nBegin;
			}
            if (m_pRes->g_bToExit)
            {
                return;
            }
			//分派模式，空闲分派
			for ( ; m_nCurBpuIndex <m_nEnd ; m_nCurBpuIndex++)
			{
				if (m_pSockMgr->at(m_nCurBpuIndex)->m_sock != INVALID_SOCKET && m_pSockMgr->at(m_nCurBpuIndex)->IsBpuFree())
				{
//					m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"发给连接[%d]的BPU处理",m_nCurBpuIndex);
					//发送成功，置状态为忙
					m_pSockMgr->at(m_nCurBpuIndex)->SetBpuFreeFlag(false);

					nRet = m_pSockMgr->at(m_nCurBpuIndex)->SendMsg(rcvdata);
					if (nRet == -101) //数据解密解压缩出错
					{
						//应答回去
						if (rcvdata.sMsgBuf->sBpcHead.cMsgType == MSG_REQ)
						{
							rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
							rcvdata.sMsgBuf->sDBHead.cNextFlag = 0;
							rcvdata.sMsgBuf->sDBHead.nLen = 0;
							rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_DATA_ENCRYPTZIP;
							m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
						}
						else if (rcvdata.sMsgBuf->sBpcHead.cMsgType == MSG_BPCMONITOR) //monitor
						{
							m_pMemPool->PoolFree(rcvdata.sMsgBuf);
							rcvdata.sMsgBuf = NULL;
						}
						else //bpc call
						{
							rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
							rcvdata.sMsgBuf->sDBHead.cNextFlag = 0;
							rcvdata.sMsgBuf->sDBHead.nLen = 0;
							rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
							rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_DATA_ENCRYPTZIP;
							m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->SendMsg(rcvdata);
						}
					}
					
//					m_pSockMgr->at(m_nCurBpuIndex)->m_bIsBuGetFlag = false;
					m_nCurBpuIndex++;
//					m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"BPU index[%d] 状态为忙",i);
					return;
				} //end if 连接正常且是空闲
// 				if (m_pSockMgr->at(m_nCurBpuIndex)->m_sock != INVALID_SOCKET)
// 				{
// 					m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"连接[%d]忙，查找下一个",m_nCurBpuIndex);
// 				}
				

			} //end for
			

		} //end while
	}
	m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"BPU组[%s %d]在指定的时间内无空闲BPU来处理[%d]秒 DREB命令[%s] 后续[%d] RA标志[%d] 交易码[%d]  标识[%d %d %d] 源[%d %d] cZip[%d] 业务数据长度[%d]",\
		m_pRes->g_vBpuGroupInfo[m_nBpugroupindex].g_sBpuGroupName.c_str(),m_nBpugroupindex,m_pRes->g_nDispatchTime, CDrebMsgProcBase::GetDrebCmdType(rcvdata.sMsgBuf->sDBHead.cCmd).c_str(),rcvdata.sMsgBuf->sDBHead.cNextFlag,\
		rcvdata.sMsgBuf->sDBHead.cRaflag,rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
		rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
		rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
		rcvdata.sMsgBuf->sDBHead.cZip,rcvdata.sMsgBuf->sDBHead.nLen);

	if (rcvdata.sMsgBuf->sBpcHead.cMsgType == MSG_REQ)
	{
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cNextFlag = 0;
		rcvdata.sMsgBuf->sDBHead.nLen = 0;
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_NOBPUFREE;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
	}
	else if (rcvdata.sMsgBuf->sBpcHead.cMsgType == MSG_BPCMONITOR) //monitor
	{
		m_pMemPool->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
	}
	else
	{
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cNextFlag = 0;
		rcvdata.sMsgBuf->sDBHead.nLen = 0;
		rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_NOBPUFREE;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->SendMsg(rcvdata);
	}

	return;
}

void CDispathThread::LogDrebHead(int loglevel, DREB_HEAD head, const char *msg, const char *filename, int fileline)
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
