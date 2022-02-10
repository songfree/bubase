// IProcessMgr.cpp: implementation of the CIProcessMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "ProcessThread.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProcessThread::CProcessThread()
{
	m_sThreadName = "CProcessThread";
	m_pRes = NULL;
	m_bIsSetGlobalVar = false;
	m_pPara = NULL;
	m_bIsInit = false;
	m_bIsRunning = false;
	m_log = NULL;
}

CProcessThread::~CProcessThread()
{
}



std::string CProcessThread::GetErrMsg()
{
	return m_errMsg;
}

bool CProcessThread::SetGlobalVar(CGResource *res,CPoolData *pooldata,CBF_BufferPool *mempool,CSocketMgr *sockmgr,void *para)
{
	if (res == NULL)
	{
		//m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"设置出错 %s %s","SetGlobalVar","res为空");
		return false;
	}
	m_pRes = res;
	m_log = &(m_pRes->m_log);
	if (pooldata == NULL)
	{
		m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"设置出错 %s %s","SetGlobalVar","pooldata为空");
		return false;
	}
	if (mempool == NULL)
	{
		m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"设置出错 %s %s","SetGlobalVar","mempool为空");
		return false;
	}
	if (sockmgr == NULL)
	{
		m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"设置出错 %s %s","SetGlobalVar","sockmgr为空");
		return false;
	}
	m_pPara = para;
	m_pMemPool = mempool;
	m_pPoolData = pooldata;
	m_pSockMgr = sockmgr;
	
	m_bIsSetGlobalVar = true;
	return true;	
}

bool CProcessThread::InitPmgr()
{
	if (m_bIsInit)
	{
		return true;
	}
	if (!m_bIsSetGlobalVar)
	{
		m_errMsg = "未设置全局资源";
		m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"NOT SetGlobalVar %s","未设置全局资源");
		return false;
	} 
	
	m_bIsRunning = true;
	CreateThread();
	
	m_bIsInit = true;
	return true;
}


int CProcessThread::Monitor()
{
	if (m_pRes->g_bIsAutoLoadBu) //自动加栽BU
	{
		S_PROCESS_NODE node;
		node.prgname = m_pRes->g_sBuPrgName;
		node.prgpara = m_pRes->g_sBuPrgPara;
		node.prgpath = m_pRes->g_sBuPrgPath;
		node.runpath = m_pRes->g_sBuRunPath;
		node.processname = "BU";

// 
// 		for (int i=m_pRes->g_nMaxConnectNum; i<m_pRes->g_nMaxConnectNum+m_pRes->g_nProcessThreadNum; i++)
// 		{
// 			if ( m_pSockMgr->at(i)->m_sock == INVALID_SOCKET )
// 			{
// 				if (m_procMgr.StartProcess(&node))
// 				{
// 					
// 				}
// 			}
// 		}
		for (int i=0;i<m_pRes->g_nProcessThreadNum;i++)
		{
			node.pid = m_pSockMgr->at(i+m_pRes->g_nMaxConnectNum)->m_nPid;
			if (node.pid >0 ) 
			{
				if (m_procMgr.FindProcess(&node,errmsg) == 0) //进程仍在，不用重启
				{
					continue;
				}
			}
			//重启bu进程
			m_pSockMgr->at(i+m_pRes->g_nMaxConnectNum)->m_nPid = -1;
			//启动BU
			if (m_procMgr.StartProcess(&node,errmsg))
			{
				//将进程ID保存
				m_pSockMgr->at(i+m_pRes->g_nMaxConnectNum)->m_nPid = node.pid;
			}
			SLEEP(400);
		}
	}
	if (IsStoped())
	{
		printf("重启CProcessThread线程 %d\n",m_bIsRunning);
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"重启CProcessThread线程 %d",m_bIsRunning);
		m_bIsRunning = true;
		CreateThread();
	}
	return 0;
}
void CProcessThread::Stop()
{
	m_bIsRunning = false;
	for (int i=0;i<m_pRes->g_nProcessThreadNum;i++)
	{
		m_pSockMgr->at(i+m_pRes->g_nMaxConnectNum)->OnClose("程序退出");
	}
	SLEEP_SECONDS(2);
}
bool CProcessThread::Terminate()
{
	m_bIsRunning = false;
	return true;
}
int CProcessThread::Run()
{
	while (m_bIsRunning)
	{
		if (m_pPoolData->GetData(m_pReqAnsData))
		{
//			m_log->LogBin(LOG_DEBUG,__FILE__,__LINE__,"处理线程取得的数据",m_pReqAnsData.msgbuf.message->buffer,m_pReqAnsData.msgbuf.head.len);
			if (time(NULL) - m_pReqAnsData.msgbuf.head.rtime > m_pRes->g_nQueueDelayTime)
			{
				m_log->LogBin(LOG_DEBUG,__FILE__,__LINE__,"在队列里超时的数据",m_pReqAnsData.msgbuf.message->buffer,m_pReqAnsData.msgbuf.head.len);
				continue;
			}
			//分发数据
			Dispatch();
		}
	}
	return 0;
}

void CProcessThread::Dispatch()
{
// #define MSG_REQ       0      //接收请求
// #define MSG_FREE      1      //BU发送空闲请求
// #define MSG_BUANS     2      //BU应答
// #define MSG_BUANSFREE 3      //BU应答并空闲请求
	switch (m_pReqAnsData.msgbuf.head.msgtype)
	{
		case MSG_REQ:
			DispatchBu();
			break;
		case MSG_FREE:
			DispatchBuFree();
			break;
		case MSG_BUANS:
			DispatchBuAns();
			break;
		case MSG_BUANSFREE:
			DispatchBuAnsFree();
			break;
		case MSG_GETSERIAL:
			GetSerial();
			break;
		default:
			m_pMemPool->PoolFree(m_pReqAnsData.msgbuf.message);
			break;
	}
	m_pReqAnsData.msgbuf.message = NULL;
}

void CProcessThread::DispatchBu()
{
	int rtime= time(NULL);
//	m_log->LogBin(LOG_ERROR,__FILE__,__LINE__,"收到的数据请求",m_pReqAnsData.msgbuf.message->buffer,m_pReqAnsData.msgbuf.head.len);
	while (time(NULL)-rtime < m_pRes->g_nDispatchTime)
	{
		for (int i=m_pRes->g_nMaxConnectNum;i<m_pRes->g_nMaxConnectNum+m_pRes->g_nProcessThreadNum;i++)
		{
			if (m_pSockMgr->at(i)->m_sock != INVALID_SOCKET && m_pSockMgr->at(i)->m_bIsBuGetFlag)
			{
				m_log->LogBin(LOG_DEBUG,__FILE__,__LINE__,"DispatchBu数据",m_pReqAnsData.msgbuf.message->buffer,m_pReqAnsData.msgbuf.head.len);
				m_pSockMgr->at(i)->SendMsg(m_pReqAnsData);
				m_pSockMgr->at(i)->m_bIsBuGetFlag = false;
				return;
			}
		}
		SLEEP(10);
	}
	m_log->LogBin(LOG_ERROR,__FILE__,__LINE__,"在指定的时间内无空闲BU来处理",m_pReqAnsData.msgbuf.message->buffer,m_pReqAnsData.msgbuf.head.len);
	m_pMemPool->PoolFree(m_pReqAnsData.msgbuf.message);
	m_pReqAnsData.msgbuf.message = NULL;
}

void CProcessThread::DispatchBuAns()
{
	if (m_pSockMgr->AffirmIndex(m_pReqAnsData.msgbuf.head.index))
	{
		if (m_pSockMgr->at(m_pReqAnsData.msgbuf.head.index)->m_sock != INVALID_SOCKET )
		{
			m_pSockMgr->at(m_pReqAnsData.msgbuf.head.index)->SendMsg(m_pReqAnsData);
			return;
		}
	}
	m_pMemPool->PoolFree(m_pReqAnsData.msgbuf.message);
	return;
}

void CProcessThread::DispatchBuFree()
{
	if (m_pSockMgr->AffirmIndex(m_pReqAnsData.msgbuf.head.buindex))
	{
		m_pSockMgr->at(m_pReqAnsData.msgbuf.head.buindex)->m_bIsBuGetFlag = true;
	}
}

void CProcessThread::DispatchBuAnsFree()
{
	DispatchBuAns();
	DispatchBuFree();
}

void CProcessThread::GetSerial()
{
	sprintf(m_pReqAnsData.msgbuf.message->buffer,"%ld",m_pRes->GetThreadShareSerial());
	m_pReqAnsData.msgbuf.head.len = strlen(m_pReqAnsData.msgbuf.message->buffer);
	m_pSockMgr->at(m_pReqAnsData.msgbuf.head.index)->SendMsg(m_pReqAnsData);
}
