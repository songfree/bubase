// MsgProcThread.cpp: implementation of the CMsgProcThread class.
//
//////////////////////////////////////////////////////////////////////

#include "MsgProcThread.h"
#include "GlobalVar.h"
#include "SocketMgr.h"
#include "BF_BufferPool.h"
#include "MemDB.h"
#include "dreberrcode.h"
#include "Dreb_Timer.h"

extern char g_VERSION[20];
extern CDreb_Timer g_pDrebTimer;

#ifdef _WINDOWS
#define   INT64PFM  "%I64d"
#else
#define   INT64PFM  "%lld"
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



CMsgProcThread::CMsgProcThread()
{
	memset(m_errMsg,0,sizeof(m_errMsg));
	m_pRes=NULL;
	m_pMemDb=NULL;
	m_pSocketMgr=NULL;
	m_pMemPool=NULL;
	m_pMsgQueue = NULL;
//	memset(&m_pRsMsgHead,0,sizeof(DREB_HEAD));
	m_nIndex =0;
}

CMsgProcThread::~CMsgProcThread()
{
	m_pRes=NULL;
	m_pMemDb=NULL;
	m_pSocketMgr=NULL;
	m_pMemPool=NULL;
	m_pMsgQueue = NULL;
}
void CMsgProcThread::ExitThreadInstance()
{
	return ;
}
bool CMsgProcThread::InitThreadInstance()
{
	return true;
}
int CMsgProcThread::Run()
{
	S_DREB_RSMSG *data=NULL;
	m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"������Ϣ�����߳� index[%d] g_bIsExit[%d]",m_nIndex,m_pRes->g_bIsExit);
	while (!m_pRes->g_bIsExit)
	{
		data = m_pMsgQueue->GetData();
		if (data != NULL)
		{
			Run(data);
		}
	}
	m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"ֹͣ��Ϣ�����߳� index[%d] g_bIsExit[%d]",m_nIndex,m_pRes->g_bIsExit);
	return 0;
}
std::string CMsgProcThread::GetDrebCmdType(int cmdtype)
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
	case  CMD_UNREGSERVICE:     //14 //ȡ������ע��
		return "CMD_UNREGSERVICE";
	case  CMD_MONITOR_DREB:     //15 //�������߽ڵ���
		return "CMD_MONITOR_DREB";
	case  CMD_MONITOR_BPC:     //16 //BPC�ڵ���
		return "CMD_MONITOR_BPC";
	case  CMD_DPBC_UNREGTX:     //17 //ȡ������Ľ���ע��㲥
		return "CMD_DPBC_UNREGTX";
	default:
		return "";
	}
}
std::string CMsgProcThread::GetDrebSockType(int socktype)
{
	switch (socktype)
	{
		case  SOCK_CLI:      //2  //�ͻ���
			return "SOCK_CLI";
		case SOCK_DREB:     //3      //�����������߽ڵ㵽���������߽ڵ������
			return "SOCK_DREB";
		case SOCK_SRV:      //4      //ע��ķ����
			return "SOCK_SRV";
		case SOCK_TODREB:   //5      //�������������߽ڵ�ڵ������
			return "SOCK_TODREB";
		case SOCK_SLOT:	  //6      //ע���ͨѶ��
			return "SOCK_SLOT";
		case SOCK_LISTEN:	  //7      //�����˿�
			return "SOCK_LISTEN";
		default:
			return "";
	}
}

std::string CMsgProcThread::GetIndexFlag(int index)
{
	if (!m_pSocketMgr->AffirmIndex(index))
	{
		return "";
	}
	char tmpchar[40];
	switch (m_pSocketMgr->at(index)->m_nType)
	{
		case SOCK_SRV:
			sprintf(tmpchar,"SVR=%d,%d",m_pSocketMgr->at(index)->m_s_srv.nSvrMainId,m_pSocketMgr->at(index)->m_s_srv.cSvrPrivateId);
			return tmpchar;
		case SOCK_TODREB:
			sprintf(tmpchar,"DREB=%d,%d",m_pSocketMgr->at(index)->m_s_center.nNodeId,m_pSocketMgr->at(index)->m_s_center.cNodePrivateId);
			return tmpchar;
		case SOCK_DREB:
			sprintf(tmpchar,"DREB=%d,%d",m_pSocketMgr->at(index)->m_s_conn.nNodeId,m_pSocketMgr->at(index)->m_s_conn.cNodePrivateId);
			return tmpchar;
		default:
			return "";
	}
}

int CMsgProcThread::Run(S_DREB_RSMSG *msg)
{
	//������Ϣ
	switch (msg->msghead.msgtype)
	{
		case MSG_READ://��socket�յ������ݣ����������ֱַ���д���
			m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"����һ��MSG_READ��Ϣ");
			OnMsgRead(msg);
			break;
		case MSG_CLOSE://���ӶϿ�����Ϣ
			m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"����һ��MSG_CLOSE��Ϣ");
			OnMsgClose(msg);
			break;
		case MSG_ROUTE://����dreb�ڵ�·�ɵ���Ϣ
			m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"����һ��MSG_ROUTE��Ϣ");
			OnMsgRoute(msg);
			break;
		case MSG_SERVICE://���ͽ���·�ɵ���Ϣ
			m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"����һ��MSG_SERVICE��Ϣ");
			OnMsgServiceRoute(msg);
			break;
		case MSG_MONITOR://���ͼ����Ϣ����Ϣ
			m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"����һ��MSG_MONITOR��Ϣ");
			OnMsgMonitor(msg);
			break;
		default:
			m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"��Ϣ�����ֲ��� %d",msg->msghead.msgtype);
			m_pMemPool->PoolFree(msg);
			break;
	}
	return 0;
}


void CMsgProcThread::OnMsgClose(S_DREB_RSMSG *msg)
{
	if (m_pSocketMgr->at(msg->msghead.index)->m_nConntime != msg->msghead.connecttime)
	{
		m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"�ر����ӱ�ʶ���� index=%d %d!=%d",msg->msghead.index,msg->msghead.connecttime,m_pSocketMgr->at(msg->msghead.index)->m_nConntime);
		m_pMemPool->PoolFree(msg);
		return;
	}
	switch (m_pSocketMgr->at(msg->msghead.index)->m_nType)
	{
		case SOCK_SRV:
			//�رշ���
			if (!m_pMemDb->CloseSvr(m_pSocketMgr->at(msg->msghead.index)->m_s_srv.nSvrMainId,m_pSocketMgr->at(msg->msghead.index)->m_s_srv.cSvrPrivateId))
			{
				m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"�ر�ʱ����[%d %d]������",\
					m_pSocketMgr->at(msg->msghead.index)->m_s_srv.nSvrMainId,\
					m_pSocketMgr->at(msg->msghead.index)->m_s_srv.cSvrPrivateId);
			}
			//ȡ�����ط����ܱ�
			m_pMemDb->UnRegisterService(m_pRes->g_nDrebId,m_pRes->g_nDrebPrivateId,m_pSocketMgr->at(msg->msghead.index)->m_s_srv.nSvrMainId,m_pSocketMgr->at(msg->msghead.index)->m_s_srv.cSvrPrivateId);
			//ȡ���㲥����
			m_pMemDb->m_subscribe.UnSubscribe(msg->msghead.index);
			m_pSocketMgr->at(msg->msghead.index)->OnClose("�Ͽ�����",__FILE__,__LINE__);
			if (m_pRes->g_nRegOtherDreb == 1) //����������ע��
			{
				OnMsgServiceRoute(msg);
			}
			return;
		case SOCK_TODREB:
			if (!m_pMemDb->CloseDreb(m_pSocketMgr->at(msg->msghead.index)->m_s_center.nNodeId,m_pSocketMgr->at(msg->msghead.index)->m_s_center.cNodePrivateId,msg->msghead.index))
			{
				m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"�رձ����������ӵ��������߽ڵ�[%d %d]������",\
					m_pSocketMgr->at(msg->msghead.index)->m_s_center.nNodeId,\
					m_pSocketMgr->at(msg->msghead.index)->m_s_center.cNodePrivateId);
			}
			//ɾ�������ӹ����Ľ���·��
			m_pMemDb->DeleteServiceByIndex(msg->msghead.index);	
			m_pSocketMgr->at(msg->msghead.index)->OnClose("�Ͽ�����",__FILE__,__LINE__);
			OnMsgRoute(msg);
			return;
		case SOCK_DREB:
			if (!m_pMemDb->CloseDreb(m_pSocketMgr->at(msg->msghead.index)->m_s_conn.nNodeId,m_pSocketMgr->at(msg->msghead.index)->m_s_conn.cNodePrivateId,msg->msghead.index))
			{
				m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"�رնԶ��������ӵ��������߽ڵ�[%d %d]������",\
					m_pSocketMgr->at(msg->msghead.index)->m_s_center.nNodeId,\
					m_pSocketMgr->at(msg->msghead.index)->m_s_center.cNodePrivateId);
			}
			//ɾ�������ӹ����Ľ���·��
			m_pMemDb->DeleteServiceByIndex(msg->msghead.index);
			m_pSocketMgr->at(msg->msghead.index)->OnClose("�Ͽ�����",__FILE__,__LINE__);
			OnMsgRoute(msg);
			return;
		case SOCK_LISTEN:
			break;
		default:
			m_pSocketMgr->at(msg->msghead.index)->OnClose("�Ͽ�����",__FILE__,__LINE__);
			break;

	}
	m_pMemPool->PoolFree(msg);
	return;
}


void CMsgProcThread::OnMsgRoute(S_DREB_RSMSG *msg)
{
	S_DREB_ROUTE *data=NULL;
	std::vector<S_DREB_ROUTE *>rtlist;
	std::vector<S_DREB_ROUTE *> dp;

	std::string msginfo;
	char tmpchar[500];
	char msghead[500];
	//ȡ���е����ӵ�DREB��Ϣ���������ڵ�
	m_pMemDb->GetAllDreb(dp);
	if (dp.size()<1)
	{
		m_pMemPool->PoolFree(msg);
		m_log->LogMp(LOG_PROMPT,__FILE__,__LINE__,"�޶������������");
		return;
	}
	m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"��������ROUTE");
	S_DREB_RSMSG *rsmsg = NULL;
	for (unsigned int i=0;i<dp.size();i++)
	{
		data = dp[i];
		if (!data->bIsClose)  //����δ�ر�
		{
			rtlist.clear();
			//����dreb���ӵ�����ȡ��Ҫ����������Ϸ��͵�·����Ϣ
			m_pMemDb->GetRouteByIndex(data->nIndex,rtlist);
			if (m_pRes->g_nPrintRoute == 1)
			{
				unsigned short nodeid;
				int   privateid;
				if (m_pSocketMgr->at(dp[i]->nIndex)->m_nType == SOCK_TODREB) //��dreb�������ӵ�
				{
					nodeid = m_pSocketMgr->at(dp[i]->nIndex)->m_s_center.nNodeId;
					privateid = m_pSocketMgr->at(dp[i]->nIndex)->m_s_center.cNodePrivateId;
				}
				else //�Է�dreb���ӹ�����
				{
					nodeid = m_pSocketMgr->at(dp[i]->nIndex)->m_s_conn.nNodeId;
					privateid = m_pSocketMgr->at(dp[i]->nIndex)->m_s_conn.cNodePrivateId;
				}
				msginfo="";
				
				bzero(tmpchar,sizeof(tmpchar));
				
				bzero(msghead,sizeof(msghead));
				sprintf(msghead,"����index[%d] dreb[%d %d]�Ľڵ�·����Ϣ",dp[i]->nIndex,nodeid,privateid);
				for (unsigned int j=0;j<rtlist.size();j++)
				{
					sprintf(tmpchar,"\n   DREB[%d %d] STEP[%d] ",\
						rtlist[j]->nNodeId,rtlist[j]->cNodePrivateId,\
						rtlist[j]->nStep);
					msginfo = msginfo + tmpchar;
				}
				msginfo = msginfo + "\n";
				m_pRes->g_pRouteLog.LogBin(LOG_INFO,__FILE__,__LINE__,msghead,(char *)msginfo.c_str(),msginfo.length());
			}
			
			if (rtlist.size()>0)
			{
				//����·�ɷ���
				rsmsg = (S_DREB_RSMSG *) m_pMemPool->PoolMalloc();
				if (rsmsg == NULL)
				{
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ڴ治�㣬�����������߽ڵ����ݿռ����");
					m_pMemPool->PoolFree(msg);
					return;
				}
				rsmsg->msghead.connecttime = m_pSocketMgr->at(data->nIndex)->m_nConntime;
				rsmsg->msghead.index = data->nIndex;
				bzero(&(rsmsg->message.head),sizeof(DREB_HEAD));
				rsmsg->message.head.cRaflag = 0;
				rsmsg->message.head.cNextFlag = 0;
				rsmsg->message.head.cZip = 0;
				rsmsg->message.head.nLen = 0;
				rsmsg->message.head.cCmd = CMD_ROUTER;
				rsmsg->message.head.s_Sinfo.s_nIndex = data->nIndex;
				rsmsg->message.head.s_Sinfo.s_cNodePrivateId = m_pRes->g_nDrebPrivateId;	
				rsmsg->message.head.s_Sinfo.s_nNodeId = m_pRes->g_nDrebId;
				rsmsg->message.head.s_Sinfo.s_nTimestamp = m_pSocketMgr->at(data->nIndex)->m_nConntime;
				if (rtlist.size()*sizeof(S_ROUTE_MSG)> DREBDATASIZE)
				{
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"·�ɱ�̫�󣬳������");
					m_pMemPool->PoolFree(rsmsg);
					m_pMemPool->PoolFree(msg);
					return;
				}
				rsmsg->message.head.nLen = rtlist.size()*sizeof(S_ROUTE_MSG); 
				for (unsigned k=0;k<rtlist.size();k++)
				{
					S_ROUTE_MSG *tbl = (S_ROUTE_MSG *)(rsmsg->message.buffer+k*sizeof(S_ROUTE_MSG));
					tbl->nNodeId = rtlist[k]->nNodeId;
					m_pDrebEndian.Endian2Comm((unsigned char *)&(tbl->nNodeId),sizeof(tbl->nNodeId));
					tbl->cNodePrivateId = rtlist[k]->cNodePrivateId;
					tbl->nStep = rtlist[k]->nStep;
					tbl->nBandWidth = rtlist[k]->nbandwidth;
					m_pDrebEndian.Endian2Comm((unsigned char *)&(tbl->nBandWidth),sizeof(tbl->nBandWidth));
					m_log->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"���͸�index=%d nNodeId=%d cNodePrivateId=%d��·��nNodeId=%d cNodePrivateId=%d nStep=%d",\
						data->nIndex,data->nNodeId,data->cNodePrivateId,tbl->nNodeId,tbl->cNodePrivateId,tbl->nStep);
				}
				if (m_pSocketMgr->at(data->nIndex)->SendMsg(rsmsg)!=0)
				{
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"����CMD_ROUTER���� %d",data->nIndex);
				}
				
			}
			else //û������ҲҪ����һ���յ�
			{
				//����·�ɷ���
				rsmsg = (S_DREB_RSMSG *) m_pMemPool->PoolMalloc();
				if (rsmsg == NULL)
				{
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ڴ治�㣬�����������߽ڵ����ݿռ����");
					m_pMemPool->PoolFree(msg);
					return;
				}
				bzero(&(rsmsg->message.head), sizeof(DREB_HEAD));
				rsmsg->message.head.cRaflag = 0;
				rsmsg->message.head.cZip = 0;
				rsmsg->message.head.cNextFlag = 0;
				rsmsg->message.head.nLen = 0;
				rsmsg->message.head.cCmd = CMD_ROUTER;
				rsmsg->message.head.s_Sinfo.s_nIndex = data->nIndex;
				rsmsg->message.head.s_Sinfo.s_cNodePrivateId = m_pRes->g_nDrebPrivateId;	
				rsmsg->message.head.s_Sinfo.s_nNodeId = m_pRes->g_nDrebId;
				rsmsg->message.head.s_Sinfo.s_nTimestamp = m_pSocketMgr->at(data->nIndex)->m_nConntime;
				if (m_pSocketMgr->at(data->nIndex)->SendMsg(rsmsg)!=0)
				{
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"����CMD_ROUTER���� %d",data->nIndex);
				}
			}
		} //end if (!data.bIsClose)
	}
	m_pMemPool->PoolFree(msg);
	return ;
}

void CMsgProcThread::OnMsgServiceRoute(S_DREB_RSMSG *msg)
{
	std::vector<S_SERVICE_ROUTE *> rtlist;
	std::vector<S_DREB_ROUTE *> dp;
	//ȡ���е�DREB��Ϣ����������
	m_pMemDb->GetAllDreb(dp);
	if (dp.size()<1)
	{
		m_pMemPool->PoolFree(msg);
		m_log->LogMp(LOG_PROMPT,__FILE__,__LINE__,"�޶������������");
		return;
	}
	
	unsigned short nodeid;
	int   privateid;
	std::string msginfo;
	char tmpchar[500];
	char msghead[500];


	int index;
	unsigned short wnodeid;
	int wpid;
	unsigned int wsvr;
	int wpsvr;
	for (unsigned int i=0;i<dp.size();i++)
	{
		if (!dp[i]->bIsClose)
		{
			rtlist.clear();
			//Ϊ�˼����������������Ӷ�Ӧ��dreb���б���ȥ��
			if (m_pSocketMgr->at(dp[i]->nIndex)->m_nType == SOCK_TODREB) //��dreb�������ӵ�
			{
				nodeid = m_pSocketMgr->at(dp[i]->nIndex)->m_s_center.nNodeId;
				privateid = m_pSocketMgr->at(dp[i]->nIndex)->m_s_center.cNodePrivateId;
			}
			else //�Է�dreb���ӹ�����
			{
				nodeid = m_pSocketMgr->at(dp[i]->nIndex)->m_s_conn.nNodeId;
				privateid = m_pSocketMgr->at(dp[i]->nIndex)->m_s_conn.cNodePrivateId;
			}
			if (!m_pMemDb->GetRouteServiceByIndex(nodeid,privateid,dp[i]->nIndex,rtlist))
			{
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"GetRouteServiceByIndex����");
				continue;
			}
			if (m_pRes->g_nPrintRoute == 1)
			{
				msginfo="";
				bzero(tmpchar,sizeof(tmpchar));
				bzero(msghead,sizeof(msghead));
				sprintf(msghead,"����index[%d] dreb[%d %d]�Ľ���·����Ϣ",dp[i]->nIndex,nodeid,privateid);
				index=-1;
				wnodeid=0;
				wpid=-1;
				wsvr=0;
				wpsvr=-1;
				for (unsigned int j=0;j<rtlist.size();j++)
				{
					if (index != rtlist[j]->nIndex || wnodeid!= rtlist[j]->nNodeId ||\
						wpid!=rtlist[j]->cNodePrivateId || wsvr != rtlist[j]->nSvrMainId ||\
						wpsvr != rtlist[j]->cSvrPrivateId)
					{
						index = rtlist[j]->nIndex;
						wnodeid = rtlist[j]->nNodeId;
						wpid =rtlist[j]->cNodePrivateId;
						wsvr = rtlist[j]->nSvrMainId;
						wpsvr = rtlist[j]->cSvrPrivateId;
						sprintf(tmpchar,"\n   INDEX[%d] DREB[%d %d] SVR[%d %d] STEP[%d] FUNCNO=",\
							index,rtlist[j]->nNodeId,rtlist[j]->cNodePrivateId,rtlist[j]->nSvrMainId,\
							rtlist[j]->cSvrPrivateId,rtlist[j]->nStep);
						msginfo = msginfo + tmpchar;
					}
					sprintf(tmpchar," %d",rtlist[j]->nFuncNo);
					msginfo = msginfo + tmpchar;
					
				}
				msginfo = msginfo + "\n";
				m_pRes->g_pRouteLog.LogBin(LOG_INFO,__FILE__,__LINE__,msghead,(char *)msginfo.c_str(),msginfo.length());
			}
			m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"��������SERVICE ROUTE index[%d] ������[%d]",dp[i]->nIndex,rtlist.size());
			SendService(dp[i]->nIndex,rtlist);
		}
	}
	m_pMemPool->PoolFree(msg);
	return;
}
void CMsgProcThread::OnMsgRead(S_DREB_RSMSG *msg)
{
	
	char cmd = msg->message.head.cCmd;
	int index;
	unsigned short s_nNodeId;
	char s_cNodePrivateId;
	if (m_log->isWrite(LOG_DEBUG))
    {
		index = msg->msghead.index;
        s_nNodeId = msg->message.head.s_Sinfo.s_nNodeId;
		s_cNodePrivateId = msg->message.head.s_Sinfo.s_cNodePrivateId;
		m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�������߽ڵ�����[%s] index[%d] ����DREB[%d %d]",\
			GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->msghead.index,s_nNodeId,\
			s_cNodePrivateId);
	}
	
	switch (msg->message.head.cCmd)
	{
		case CMD_PING:  //����
			AnsMsg(msg,SUCCESS,"Ӧ������"); 
			break;
		case CMD_CONNECT:  //����
			OnCmdConnect(msg); //�������ӵ���֤��
			break;
		case CMD_ROUTER:  //·��֪ͨ
			OnCmdRoute(msg);
			break;
		case CMD_SERVICE:  //������֪ͨ
			OnCmdService(msg);
			break;	
		case CMD_REGSERVICE:
			OnCmdServiceReg(msg);//�����ܺ�ע��
			break;
		case CMD_UNREGSERVICE:
			OnCmdUnServiceReg(msg);//�����ܺ�ȡ��ע��
			break;
		case CMD_DEL_NODE: //ͨѶ�ڵ�ȡ��
			OnCmdDelNode(msg);
			break;
		case CMD_DEL_SVR:  //����ע��
			OnCmdDelSvr(msg);
			break;
		case CMD_DPCALL:   //ͬ������ ���շ���Ӧ��
			OnCmdDpCall(msg);
			break;
		case CMD_DPACALL:  //�첽���ã��������߽ڵ�Ӧ��
			OnCmdDpACall(msg);	
			break;
		case CMD_DPBC:     //�㲥��Ӧ��
			OnCmdDpBc(msg);
			break;
		case CMD_DPABC:    //�㲥��Ӧ��
			OnCmdDpABc(msg);
			break;
		case CMD_DPPOST:    //Ͷ�ݣ��������߽ڵ�Ӧ��
			OnCmdDpPost(msg);
			break;
		case CMD_DPPUSH:    //���ͣ���Ӧ��
			OnCmdDpPush(msg);
			break;
		case CMD_MONITOR_DREB://DREB��ع���
			OnCmdMonitorDreb(msg);
			break;
		case CMD_MONITOR_BPC://BPC��ع���
			OnCmdMonitorBpc(msg);
			break;
		case CMD_DPBC_UNREGTX://�㲥ȡ������Ľ���ע��
			OnCmdUnReqTx(msg);
			break;
		case CMD_SUBSCRIBE:		//�����Ĺ㲥
			OnCmdSubscribe(msg);
			break;
        case CMD_UNSUBSCRIBE:		//����ȡ�����Ĺ㲥
            OnCmdUnSubscribe(msg);
			break;
		default:
			m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"�������߽ڵ������ֲ��� %d index[%d] ����DREB[%d %d]",\
				msg->message.head.cCmd,msg->msghead.index,msg->message.head.s_Sinfo.s_nNodeId,\
				msg->message.head.s_Sinfo.s_cNodePrivateId);
			m_pMemPool->PoolFree(msg);
			return;
	}
	if (m_log->isWrite(LOG_DEBUG))
	{
		m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�������߽ڵ�����[%s] ������� index[%d] ����DREB[%d %d ]",\
			GetDrebCmdType(cmd).c_str(),msg->msghead.index,s_nNodeId,\
			s_cNodePrivateId);
	}
	return ;
}


void CMsgProcThread::AcceptClient(S_DREB_RSMSG *msg)
{
	m_pSocketMgr->at(msg->msghead.index)->SetClient();
	//Ӧ���ȥ
	AnsConnect(msg,SUCCESS,"�ͻ�������");
}

void CMsgProcThread::AcceptSvr(S_DREB_RSMSG *msg)
{
	int index = msg->msghead.index;
	S_CONNECT_DATA *conData = (S_CONNECT_DATA *)msg->message.buffer;
	if (conData->nSvrMainId<1)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"index[%d] �����%d����",msg->msghead.index,conData->nSvrMainId);
		AnsConnect(msg,ERR_SVRID,"����Ų���");
		m_pSocketMgr->at(index)->OnClose("����Ų���",__FILE__,__LINE__);
		return;
	}
	if (conData->cSvrPrivateId<1)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"index[%d] ����˽�к�%d����",msg->msghead.index,conData->cSvrPrivateId);
		AnsConnect(msg,ERR_PRIVATEID,"����˽�кŲ���");
		m_pSocketMgr->at(index)->OnClose("����˽�кŲ���",__FILE__,__LINE__);
		return;
	}
	CSendQueue *psque=NULL;
	//ע�����
	if (!m_pMemDb->RegisterSvr(conData->nSvrMainId,conData->cSvrPrivateId,msg->msghead.index,&psque))
	{
		sprintf(m_errMsg,"index[%d] ����[%d %d]��ע��",msg->msghead.index,conData->nSvrMainId,conData->cSvrPrivateId);
		AnsConnect(msg,ERR_REGISTER,m_errMsg);
		m_pSocketMgr->at(index)->OnClose(m_errMsg,__FILE__,__LINE__);
		return;
	}
	m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"index[%d] ����[%d %d]��Ӧ�Ķ��е�ַ[%d]",\
		msg->msghead.index,conData->nSvrMainId,conData->cSvrPrivateId,psque);

	m_pSocketMgr->at(msg->msghead.index)->SetSvr(conData->nSvrMainId,conData->cSvrPrivateId,psque);
	//20130304 songfree add ��������ʱ����������뵽����·�ɱ���������·�ɣ�������Ĭ��Ϊ0
	S_SERVICE_ROUTE rt;
	bzero(&rt,sizeof(S_SERVICE_ROUTE));
	rt.cNodePrivateId = m_pRes->g_nDrebPrivateId;
	rt.cSvrPrivateId = conData->cSvrPrivateId;
	rt.nFuncNo = 0;
	//nIndexΪ���������������ӵ�index
	rt.nIndex = msg->msghead.index;
	rt.nNodeId = m_pRes->g_nDrebId;
	rt.nStep = 0;
	rt.nSvrMainId = conData->nSvrMainId;
	rt.nUpdateTime = time(NULL);
	m_pMemDb->AddServiceRoute(rt);
	//ADD END

	m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"index[%d] ���ܷ���ڵ�[%d %d]���ӳɹ�",\
		msg->msghead.index,conData->nSvrMainId,conData->cSvrPrivateId);
	AnsConnect(msg,SUCCESS,"���������");
	return;

}

void CMsgProcThread::AcceptDreb(S_DREB_RSMSG *msg)
{
	int index = msg->msghead.index;
	S_CONNECT_DATA *conData = (S_CONNECT_DATA *)msg->message.buffer;
	if (conData->nNodeId<1)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"index[%d] �������߽ڵ�ID[%d]����",msg->msghead.index,conData->nNodeId);
		AnsConnect(msg,ERR_DREBID,"�������߽ڵ�ID����");
		m_pSocketMgr->at(index)->OnClose("�������߽ڵ�ID����",__FILE__,__LINE__);
		return;
	}
	if (conData->cNodePrivateId<1)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"index[%d] �������߽ڵ�˽��ID[%d]����",msg->msghead.index,conData->cNodePrivateId);
		AnsConnect(msg,ERR_DREBPRIVATEID,"�������߽ڵ�ID����");
		m_pSocketMgr->at(index)->OnClose("�������߽ڵ�˽��ID����",__FILE__,__LINE__);
		return;
	}
	if (m_pRes->g_nDrebId == conData->nNodeId  && m_pRes->g_nDrebPrivateId == conData->cNodePrivateId )
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"index[%d] Ҫע���DREB�ڵ�ͱ��ڵ���ͬ[%d %d]",\
			msg->msghead.index,conData->nNodeId,conData->cNodePrivateId);
		AnsConnect(msg,ERR_DREBNODE,"Ҫע���DREB�ڵ�ͱ��ڵ���ͬ");
		m_pSocketMgr->at(index)->OnClose("Ҫע���DREB�ڵ�ͱ��ڵ���ͬ",__FILE__,__LINE__);
		return;
	}
	CSendQueue *psque;
	if (!m_pMemDb->RegisterDreb(conData->nNodeId,conData->cNodePrivateId,index,conData->nBandWidth,&psque))
	{
		sprintf(m_errMsg,"index[%d] �������߽ڵ�[%d %d]��ע��",msg->msghead.index,conData->nNodeId,conData->cNodePrivateId);
		AnsConnect(msg,ERR_DREBREGISTER,m_errMsg);
		m_pSocketMgr->at(index)->OnClose(m_errMsg,__FILE__,__LINE__);
		return;
	}
	
	m_pSocketMgr->at(index)->SetDreb(conData->nNodeId,conData->cNodePrivateId,conData->nBandWidth,psque);
	m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"index[%d] �����������߽ڵ�nodeid=%d privateid=%d index=%d ���ӳɹ�",\
		msg->msghead.index,conData->nNodeId,conData->cNodePrivateId,index);
	AnsConnect(msg,SUCCESS,"�������߽ڵ����ӳɹ�");	
	return;
}

void CMsgProcThread::OnCmdConnect(S_DREB_RSMSG *msg)
{
	int index = msg->msghead.index;
	if (m_pSocketMgr->at(index)->m_bChecked)
	{
		m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"������ȷ��");
		m_pMemPool->PoolFree(msg);
		return;
	}

	if (msg->message.head.cRaflag == 0) //�ͻ��ˡ�����ˡ�ͨѶ�ڵ㷢����������
	{
		if (msg->message.head.nLen < sizeof(S_CONNECT_DATA))
		{
			m_pSocketMgr->at(index)->OnClose("������Ϣ���Ȳ���",__FILE__,__LINE__);
			m_pMemPool->PoolFree(msg);
			return ;
		}
		S_CONNECT_DATA *conData = (S_CONNECT_DATA *)(msg->message.buffer);
		m_pDrebEndian.Endian2Comm((unsigned char *)&(conData->nNodeId),sizeof(conData->nNodeId));
		m_pDrebEndian.Endian2Comm((unsigned char *)&(conData->nSvrMainId),sizeof(conData->nSvrMainId));
		switch (conData->cConnectType)
		{
			case 0://�ͻ���
				AcceptClient(msg);
				break;
			case 1: //�����
				AcceptSvr(msg);
				break;
			case 2://�������߽ڵ�
				AcceptDreb(msg);
				break;
			default:
				m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"index[%d] �������ʹ��� %d",msg->msghead.index,conData->cConnectType);
				AnsConnect(msg,ERR_VERSION,"�������ʹ���");
				m_pSocketMgr->at(index)->OnClose("�������ʹ���",__FILE__,__LINE__);
				break;	
		}
		return;
	}
	else //Ӧ��,�������ӵ�
	{
		if (m_pSocketMgr->at(index)->m_nType != SOCK_TODREB)
		{
			m_log->LogMp(LOG_PROMPT,__FILE__,__LINE__,"index[%d] �Զ�[%d]��Ӧ�𣬶���",\
				msg->msghead.index,m_pSocketMgr->at(index)->m_nType);
			m_pMemPool->PoolFree(msg);
			return;
		}
		if (msg->message.head.a_Ainfo.a_nRetCode != SUCCESS)
		{
			m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"index[%d] �Զ�dreb���� %d",msg->msghead.index,msg->message.head.a_Ainfo.a_nRetCode);
			m_pSocketMgr->at(index)->OnClose("�Զ�DREB�ܾ�����",__FILE__,__LINE__);
			m_pMemPool->PoolFree(msg);
			return;
		}
		CSendQueue *psque;
		if (!m_pMemDb->RegisterDreb(msg->message.head.a_Ainfo.a_nNodeId,msg->message.head.a_Ainfo.a_cNodePrivateId,msg->msghead.index,m_pSocketMgr->at(msg->msghead.index)->m_s_center.nbandwidth,&psque))
		{
			m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"index[%d] ע���������߽ڵ�ʧ��[%d %d]",\
				msg->msghead.index,msg->message.head.a_Ainfo.a_nNodeId,msg->message.head.a_Ainfo.a_cNodePrivateId);
			m_pSocketMgr->at(index)->OnClose("ע���������߽ڵ�ʧ��",__FILE__,__LINE__);
			m_pMemPool->PoolFree(msg);
			return;
		}
		m_pSocketMgr->at(index)->m_s_center.cNodePrivateId = msg->message.head.a_Ainfo.a_cNodePrivateId;
		m_pSocketMgr->at(index)->m_s_center.nNodeId = msg->message.head.a_Ainfo.a_nNodeId;
		m_pSocketMgr->at(index)->SetConnectDreb(msg->msghead.socket,msg->msghead.connecttime,psque);
		m_pMemPool->PoolFree(msg);
		return;
	}
	
}
void CMsgProcThread::OnCmdRoute(S_DREB_RSMSG *msg)
{
	std::vector<S_DREB_ROUTE>rtlist;
	S_DREB_ROUTE data;
	bzero(&data,sizeof(S_DREB_ROUTE));
	int rtnum = msg->message.head.nLen/sizeof(S_ROUTE_MSG);

	unsigned short nodeid;
	int  privateid;
	if (m_pSocketMgr->at(msg->msghead.index)->m_nType == SOCK_TODREB)
	{
		nodeid= m_pSocketMgr->at(msg->msghead.index)->m_s_center.nNodeId;
		privateid = m_pSocketMgr->at(msg->msghead.index)->m_s_center.cNodePrivateId;
	}
	else
	{
		nodeid= m_pSocketMgr->at(msg->msghead.index)->m_s_conn.nNodeId;
		privateid = m_pSocketMgr->at(msg->msghead.index)->m_s_conn.cNodePrivateId;
	}
	std::string msginfo;
	char tmpchar[400];
	bzero(tmpchar,sizeof(tmpchar));
	//��ɾ������������ϵ���������·��
	m_pMemDb->DeleteRouteByIndex(msg->msghead.index);
	
	for (int i=0;i<rtnum;i++)
	{
		S_ROUTE_MSG *rt = (S_ROUTE_MSG *)(msg->message.buffer+i*sizeof(S_ROUTE_MSG));
		data.cNodePrivateId = rt->cNodePrivateId;
		m_pDrebEndian.Endian2Comm((unsigned char *)&(rt->nNodeId),sizeof(rt->nNodeId));
		data.nNodeId = rt->nNodeId;
		data.nStep = rt->nStep+1;
		data.nIndex = msg->msghead.index;
		m_pDrebEndian.Endian2Comm((unsigned char *)&(rt->nBandWidth),sizeof(rt->nBandWidth));
		if (m_pSocketMgr->at(msg->msghead.index)->m_nType == SOCK_TODREB && \
			m_pSocketMgr->at(msg->msghead.index)->m_s_center.nbandwidth < rt->nBandWidth)
		{
			data.nbandwidth = m_pSocketMgr->at(msg->msghead.index)->m_s_center.nbandwidth;
		}
		else if (m_pSocketMgr->at(msg->msghead.index)->m_nType == SOCK_DREB && \
			m_pSocketMgr->at(msg->msghead.index)->m_s_conn.nbandwidth < rt->nBandWidth)
		{
			data.nbandwidth = m_pSocketMgr->at(msg->msghead.index)->m_s_conn.nbandwidth;
		}
		else
		{
			data.nbandwidth = rt->nBandWidth;
		}
		if (m_pRes->g_nPrintRoute == 1)
		{
			sprintf(tmpchar,"\n   DREB[%d %d] STEP[%d]",data.nNodeId,data.cNodePrivateId,data.nStep);
			msginfo = msginfo +tmpchar;
		}

		//songfree 20210126 �޸���󲽽�Ϊ����
		if (data.nStep <= m_pRes->g_nMaxRouteStep )//��������16����Ϊ��ѭ��·�ɣ�ȥ��
		{			
			if (data.nNodeId == m_pRes->g_nDrebId && data.cNodePrivateId == m_pRes->g_nDrebPrivateId)
			{
				//�Ǳ��ڵ��·�ɣ�ȥ��
			}
			else
			{
				rtlist.push_back(data);
			}
		}
	}
	m_log->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"�յ�index=%d ������·������[%d]",msg->msghead.index,rtlist.size());
	//����·��,�����ֱ����dreb�����ü���·�ɱ�
	if (m_pRes->g_nPrintRoute == 1)
	{
		msginfo = msginfo +"\n";
		sprintf(tmpchar,"�յ�index[%d] DREB[%d %d] �Ľڵ�·��",msg->msghead.index,nodeid,privateid);
		m_pRes->g_pRouteLog.LogBin(LOG_INFO,__FILE__,__LINE__,tmpchar,(char *)msginfo.c_str(),msginfo.length());
	}
	m_pMemDb->AddRoute(msg->msghead.index,rtlist);
	PrintRoute();
	m_pMemPool->PoolFree(msg);

}

void CMsgProcThread::OnCmdDelSvr(S_DREB_RSMSG *msg)
{
	if (m_pSocketMgr->at(msg->msghead.index)->m_nType != SOCK_SRV)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"OnCmdDelSvr �������Ͳ��� %d",m_pSocketMgr->at(msg->msghead.index)->m_nType);
		m_pMemPool->PoolFree(msg);
		return;
	}
	//ȡ�����ط����
	m_pMemDb->UnRegisterSvr(m_pSocketMgr->at(msg->msghead.index)->m_s_srv.nSvrMainId,m_pSocketMgr->at(msg->msghead.index)->m_s_srv.cSvrPrivateId);
	//ȡ�����ط����ܱ�
	m_pMemDb->UnRegisterService(m_pRes->g_nDrebId,m_pRes->g_nDrebPrivateId,\
		m_pSocketMgr->at(msg->msghead.index)->m_s_srv.nSvrMainId,m_pSocketMgr->at(msg->msghead.index)->m_s_srv.cSvrPrivateId);
	m_pSocketMgr->at(msg->msghead.index)->OnClose("ע������",__FILE__,__LINE__);
 	m_pSocketMgr->at(msg->msghead.index)->OnShutdown();
	m_pMemPool->PoolFree(msg);
}

void CMsgProcThread::OnCmdDelNode(S_DREB_RSMSG *msg)
{
	if (m_pSocketMgr->at(msg->msghead.index)->m_nType != SOCK_DREB)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"OnCmdDelNode �������Ͳ��� %d",m_pSocketMgr->at(msg->msghead.index)->m_nType);
		m_pMemPool->PoolFree(msg);
		return;
	}
	m_pMemDb->UnRegisterDreb(m_pSocketMgr->at(msg->msghead.index)->m_s_conn.nNodeId,\
		m_pSocketMgr->at(msg->msghead.index)->m_s_conn.cNodePrivateId,msg->msghead.index);
	
	m_pSocketMgr->at(msg->msghead.index)->OnShutdown();
	m_pMemPool->PoolFree(msg);
}


bool CMsgProcThread::AnsMsg(S_DREB_RSMSG *msg, unsigned short errcode,const char *errmsg)
{
	if (errcode != SUCCESS)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,errmsg);
	}
	else
	{
		m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,errmsg);
	}
	if (msg->msghead.msgtype == MSG_MONITOR)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ͼ��֪ͨ���� %d",msg->message.head.cCmd);
		m_pMemPool->PoolFree(msg);
		return true;
	}
	if (msg->message.head.cCmd == CMD_DPPUSH || msg->message.head.cCmd == CMD_DPABC)
	{
		m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"DPPUSH��DPABC����Ӧ�� %d",msg->message.head.cCmd);
		m_pMemPool->PoolFree(msg);
		return true;
	}
	msg->message.head.cRaflag = 1;
	msg->message.head.cDrebAffirm = 1;
	msg->message.head.cZip = 0;
	msg->message.head.nLen = 0;
	
	msg->message.head.a_Ainfo.a_cNodePrivateId = m_pRes->g_nDrebPrivateId;	
	msg->message.head.a_Ainfo.a_nNodeId = m_pRes->g_nDrebId;
	msg->message.head.a_Ainfo.a_nRetCode = errcode;

	m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"Ӧ�� %d",msg->msghead.index);

	if (m_pSocketMgr->at(msg->msghead.index)->SendMsg(msg)!=0)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"����Ӧ�����");
	}
	return true;
}

void CMsgProcThread::OnCmdDpCall(S_DREB_RSMSG *msg)
{
	if (msg->message.head.cRaflag == 0)
	{
// #ifdef _WINDOWS
// 		m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"��ʼת�� %I64d",CBF_Date_Time::GetTickUS());
// #else
// 		m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"��ʼת�� %lld",CBF_Date_Time::GetTickUS());
// #endif
		TransMsgRequest(msg,false);
// #ifdef _WINDOWS
// 		m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"ת����� %I64d",CBF_Date_Time::GetTickUS());
// #else
// 		m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"ת����� %lld",CBF_Date_Time::GetTickUS());
// #endif
		
	}
	else
	{
// #ifdef _WINDOWS
// 		m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"��ʼӦ�� %I64d",CBF_Date_Time::GetTickUS());
// #else
// 		m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"��ʼӦ�� %lld",CBF_Date_Time::GetTickUS());
// #endif
		TransMsgAns(msg);
// #ifdef _WINDOWS
// 		m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"Ӧ����� %I64d",CBF_Date_Time::GetTickUS());
// #else
// 		m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"Ӧ����� %lld",CBF_Date_Time::GetTickUS());
// #endif
	}
	return;
}

void CMsgProcThread::OnCmdDpACall(S_DREB_RSMSG *msg)
{
	if (msg->message.head.cRaflag == 0)
	{
		TransMsgRequest(msg,true);
	}
	else
	{
		TransMsgAns(msg);
	}
}
void CMsgProcThread::OnCmdDpPush(S_DREB_RSMSG *msg)
{
	if (msg->message.head.cRaflag == 0)
	{
		TransMsgRequest(msg,false);
	}
	else
	{
		TransMsgAns(msg);
	}
}

void CMsgProcThread::OnCmdDpPost(S_DREB_RSMSG *msg)
{
	if (msg->message.head.cRaflag == 0)
	{
		TransMsgRequest(msg,true);
	}
	else
	{
		TransMsgAns(msg);
	}
}
void CMsgProcThread::OnCmdDpBc(S_DREB_RSMSG *msg)
{
	if (msg->message.head.cRaflag == 0)
	{
		TransDestBc(msg);
		TransBroadCast(msg,true);
	}
	else
	{
		TransMsgAns(msg);
	}
}

void CMsgProcThread::OnCmdDpABc(S_DREB_RSMSG *msg)
{
	if (msg->message.head.cRaflag == 0)
	{
		TransDestBc(msg);
		TransBroadCast(msg,false);
	}
	else
	{
		TransMsgAns(msg);
	}
}



bool CMsgProcThread::SetGlobalVar(CGlobalVar *pVar, CSocketMgr *sockmgr, CMemDB *pMemDb, CBF_BufferPool *pool,CPoolData *msgqueue)
{
	m_pRes = pVar;
	m_log = &(m_pRes->g_pLog);
	m_pMemDb=pMemDb;
	m_pSocketMgr=sockmgr;
	m_pMemPool=pool;
	m_pMsgQueue = msgqueue;
	
//	m_pLogRoute.SetLogPara(m_pRes->g_nLogLevel+1,m_pRes->g_sLogFilePath.c_str(),"drebroute.log");
	return true;
}
bool CMsgProcThread::AnsConnect(S_DREB_RSMSG *msg, unsigned short errcode,const char *errmsg)
{
	if (errcode != SUCCESS)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,errmsg);
	}
	else
	{
		m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,errmsg);
	}
	msg->message.head.cRaflag = 1;
	msg->message.head.cDrebAffirm = 1;
	msg->message.head.cZip = 0;
	msg->message.head.nLen = 0;
	msg->message.head.cNextFlag = 0;
	
	msg->message.head.a_Ainfo.a_cNodePrivateId = m_pRes->g_nDrebPrivateId;	
	msg->message.head.a_Ainfo.a_nNodeId = m_pRes->g_nDrebId;
	msg->message.head.a_Ainfo.a_nRetCode = errcode;
	
	msg->message.head.d_Dinfo.d_cNodePrivateId = msg->message.head.s_Sinfo.s_cNodePrivateId;
	msg->message.head.d_Dinfo.d_cSvrPrivateId = msg->message.head.s_Sinfo.s_cSvrPrivateId;
	msg->message.head.d_Dinfo.d_nNodeId = msg->message.head.s_Sinfo.s_nNodeId;
	msg->message.head.d_Dinfo.d_nSvrMainId = msg->message.head.s_Sinfo.s_nSvrMainId;

	msg->message.head.s_Sinfo.s_nNodeId = m_pRes->g_nDrebId;
	msg->message.head.s_Sinfo.s_cNodePrivateId = m_pRes->g_nDrebPrivateId;
	msg->message.head.s_Sinfo.s_nIndex = msg->msghead.index;
	msg->message.head.s_Sinfo.s_nTimestamp = msg->msghead.connecttime;
	msg->message.head.s_Sinfo.s_nSvrMainId = 0;
	msg->message.head.s_Sinfo.s_cSvrPrivateId = 0;

	if (m_pSocketMgr->at(msg->msghead.index)->SendMsg(msg,true)!=0)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"����Ӧ�����");
		return false;
	}
	return true;
}


void CMsgProcThread::SendService(int index, std::vector<S_SERVICE_ROUTE *> rtlist)
{
	S_DREB_RSMSG *rsmsg=NULL;
	if (rtlist.size()<1)
	{
		rsmsg = (S_DREB_RSMSG *) m_pMemPool->PoolMalloc();
		if (rsmsg == NULL)
		{
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ڴ治�㣬�����������߽ڵ����ݿռ����");
			return;
		}
		bzero(&(rsmsg->message.head), sizeof(DREB_HEAD));
		rsmsg->msghead.connecttime = m_pSocketMgr->at(index)->m_nConntime;
		rsmsg->msghead.index = index;
		rsmsg->message.head.cRaflag = 0;
		rsmsg->message.head.cNextFlag = 0;
		rsmsg->message.head.cZip = 0;
		rsmsg->message.head.cCmd = CMD_SERVICE;
		rsmsg->message.head.s_Sinfo.s_nIndex = index;
		rsmsg->message.head.s_Sinfo.s_cNodePrivateId = m_pRes->g_nDrebPrivateId;	
		rsmsg->message.head.s_Sinfo.s_nNodeId = m_pRes->g_nDrebId;
		rsmsg->message.head.s_Sinfo.s_nTimestamp = m_pSocketMgr->at(index)->m_nConntime;
		rsmsg->message.head.nLen = 0;
		if (m_pSocketMgr->at(index)->SendMsg(rsmsg)!=0)
		{
			if (m_pSocketMgr->at(index)->m_nType == SOCK_DREB)
			{
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ͷ���·��SOCK_DREB����");
			}
			else if (m_pSocketMgr->at(index)->m_nType == SOCK_TODREB)
			{
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ͷ���·��SOCK_TODREB ����");
			}
			
		}
		return;
	}
	bool isnext=false;
	int len=0;
	int allfuncnum=0;
	S_ROUTESVR_MSG *prs=NULL;
	rsmsg = (S_DREB_RSMSG *) m_pMemPool->PoolMalloc();
	if (rsmsg == NULL)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ڴ治�㣬�����������߽ڵ����ݿռ����");
		return;
	}
	bzero(&(rsmsg->message.head), sizeof(DREB_HEAD));
	rsmsg->msghead.connecttime = m_pSocketMgr->at(index)->m_nConntime;
	rsmsg->msghead.index = index;
	rsmsg->message.head.cRaflag = 0;
	rsmsg->message.head.cNextFlag = 0;
	rsmsg->message.head.cZip = 0;
	rsmsg->message.head.cCmd = CMD_SERVICE;
	rsmsg->message.head.s_Sinfo.s_nIndex = index;
	rsmsg->message.head.s_Sinfo.s_cNodePrivateId = m_pRes->g_nDrebPrivateId;	
	rsmsg->message.head.s_Sinfo.s_nNodeId = m_pRes->g_nDrebId;
	rsmsg->message.head.s_Sinfo.s_nTimestamp = m_pSocketMgr->at(index)->m_nConntime;
	rsmsg->message.head.nLen = 0;
	bool isbegin = false;
	for (unsigned int i=0;i<rtlist.size();i++)
	{
		if (rtlist[i]->nStep >= m_pRes->g_nMaxRouteStep-1) //�����������Ĳ���-1�������ٷ��ˣ� ����Ҳû��
		{
			continue;
		}
		if (isbegin) //һ���°���ʼ
		{
			prs = (S_ROUTESVR_MSG *)(rsmsg->message.buffer+len);
			prs->cNodePrivateId = rtlist[i]->cNodePrivateId;
			prs->nNodeId = rtlist[i]->nNodeId;
			m_pDrebEndian.Endian2Comm((unsigned char *)&(prs->nNodeId),sizeof(prs->nNodeId));
			prs->nSvrMainId = rtlist[i]->nSvrMainId;
			m_pDrebEndian.Endian2Comm((unsigned char *)&(prs->nSvrMainId),sizeof(prs->nSvrMainId));
			prs->cSvrPrivateId = rtlist[i]->cSvrPrivateId;
			prs->nFuncNo = rtlist[i]->nFuncNo;
			m_pDrebEndian.Endian2Comm((unsigned char *)&(prs->nFuncNo),sizeof(prs->nFuncNo));
			prs->nFuncNum = 1;
			allfuncnum++;
			prs->cStep = rtlist[i]->nStep;
			len = sizeof(S_ROUTESVR_MSG);
			isbegin = false;
		}
		else
		{
			if (i==0)
			{
				//��һ�����ݰ�
				prs = (S_ROUTESVR_MSG *)rsmsg->message.buffer;
				prs->cNodePrivateId = rtlist[i]->cNodePrivateId;
				prs->nNodeId = rtlist[i]->nNodeId;
				m_pDrebEndian.Endian2Comm((unsigned char *)&(prs->nNodeId),sizeof(prs->nNodeId));
				prs->nSvrMainId = rtlist[i]->nSvrMainId;
				m_pDrebEndian.Endian2Comm((unsigned char *)&(prs->nSvrMainId),sizeof(prs->nSvrMainId));
				prs->cSvrPrivateId = rtlist[i]->cSvrPrivateId;
				prs->nFuncNo = rtlist[i]->nFuncNo;
				m_pDrebEndian.Endian2Comm((unsigned char *)&(prs->nFuncNo),sizeof(prs->nFuncNo));
				prs->nFuncNum = 1;
				allfuncnum++;
				prs->cStep = rtlist[i]->nStep;
				len = sizeof(S_ROUTESVR_MSG);
			}
			else
			{
				if (rtlist[i]->nNodeId == rtlist[i-1]->nNodeId && rtlist[i]->cNodePrivateId == rtlist[i-1]->cNodePrivateId \
					&& rtlist[i]->nSvrMainId == rtlist[i-1]->nSvrMainId && rtlist[i]->cSvrPrivateId == rtlist[i-1]->cSvrPrivateId)
				{ //������һ������
					//20160108moid HP_UX
// 					unsigned int *func = (unsigned int *)(rsmsg->message.buffer+len);
// 					*func = rtlist[i].nFuncNo;
					memcpy(rsmsg->message.buffer+len,&(rtlist[i]->nFuncNo),sizeof(unsigned int));
					m_pDrebEndian.Endian2Comm((unsigned char *)(rsmsg->message.buffer+len),sizeof(unsigned int));
					//20160108MODI END 
					prs->nFuncNum++;
					allfuncnum++;
					len +=sizeof(unsigned int);
				}
				else //����һ��������
				{
					//����һ������Ľ����������ֽ�����
					m_pDrebEndian.Endian2Comm((unsigned char *)&(prs->nFuncNum),sizeof(prs->nFuncNum));
					//��һ���������
					prs = (S_ROUTESVR_MSG *)(rsmsg->message.buffer+len);
					prs->cNodePrivateId = rtlist[i]->cNodePrivateId;
					prs->nNodeId = rtlist[i]->nNodeId;
					m_pDrebEndian.Endian2Comm((unsigned char *)&(prs->nNodeId),sizeof(prs->nNodeId));
					prs->nSvrMainId = rtlist[i]->nSvrMainId;
					m_pDrebEndian.Endian2Comm((unsigned char *)&(prs->nSvrMainId),sizeof(prs->nSvrMainId));
					prs->cSvrPrivateId = rtlist[i]->cSvrPrivateId;
					prs->nFuncNo = rtlist[i]->nFuncNo;
					m_pDrebEndian.Endian2Comm((unsigned char *)&(prs->nFuncNo),sizeof(prs->nFuncNo));
					prs->nFuncNum = 1;
					allfuncnum++;
					prs->cStep = rtlist[i]->nStep;
					len += sizeof(S_ROUTESVR_MSG);
				}
			}
		}
		
		if (i == rtlist.size()-1) //���һ��������
		{
			rsmsg->message.head.nLen = len;
			
			m_log->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"index=%d ����service·����Ŀ%d ���ݳ���%d",index,allfuncnum,len);
			m_pDrebEndian.Endian2Comm((unsigned char *)&(prs->nFuncNum),sizeof(prs->nFuncNum));
			if (m_pSocketMgr->at(index)->SendMsg(rsmsg)!=0)
			{
				if (m_pSocketMgr->at(index)->m_nType == SOCK_DREB)
				{
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ͷ���·��SOCK_DREB����");
				}
				else if (m_pSocketMgr->at(index)->m_nType == SOCK_TODREB)
				{
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ͷ���·��SOCK_TODREB ����");
				}
			}
			return ;
		}
		
		if (len >= DREBDATASIZE-100) //���񹻶࣬Ҫ�������
		{
			rsmsg->message.head.nLen = len;
			rsmsg->message.head.cNextFlag = 1;
			isnext = true;
			m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�����ݰ����ͽ���·�� index=%d ����service·����Ŀ%d ���ݳ���%d",index,allfuncnum,len);
			m_pDrebEndian.Endian2Comm((unsigned char *)&(prs->nFuncNum),sizeof(prs->nFuncNum));
			if (m_pSocketMgr->at(index)->SendMsg(rsmsg)!=0)
			{
				if (m_pSocketMgr->at(index)->m_nType == SOCK_DREB)
				{
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ͷ���·��SOCK_DREB����");
				}
				else if (m_pSocketMgr->at(index)->m_nType == SOCK_TODREB)
				{
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ͷ���·��SOCK_TODREB ����");
				}
			}
			rsmsg = (S_DREB_RSMSG *) m_pMemPool->PoolMalloc();
			if (rsmsg == NULL)
			{
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ڴ治�㣬�����������߽ڵ����ݿռ����");
				return;
			}
			bzero(&(rsmsg->message.head), sizeof(DREB_HEAD));
			rsmsg->msghead.connecttime = m_pSocketMgr->at(index)->m_nConntime;
			rsmsg->msghead.index = index;
			rsmsg->message.head.cRaflag = 0;
			if (isnext) //�к����� 
			{
				rsmsg->message.head.cNextFlag = 1;
			}
			else
			{
				rsmsg->message.head.cNextFlag = 0;
			}
			
			rsmsg->message.head.cZip = 0;
			rsmsg->message.head.cCmd = CMD_SERVICE;
			rsmsg->message.head.s_Sinfo.s_nIndex = index;
			rsmsg->message.head.s_Sinfo.s_cNodePrivateId = m_pRes->g_nDrebPrivateId;	
			rsmsg->message.head.s_Sinfo.s_nNodeId = m_pRes->g_nDrebId;
			rsmsg->message.head.s_Sinfo.s_nTimestamp = m_pSocketMgr->at(index)->m_nConntime;
			rsmsg->message.head.nLen = 0;
			len = 0;
			isbegin = true;  //��ʾ����һ�����ݰ���ʼ
			allfuncnum =0;
		}
	} // end for (int i=0;i<rtlist.size();i++)
	m_pMemPool->PoolFree(rsmsg);
	return;
}


// ������: CMsgProcThread::OnCmdService
// ���  : ������ 2013-6-24 16:21:33
// ����  : void 
// ����  : S_DREB_RSMSG *msg
// ����  : ���յ��ķ���·�ɣ�����·�ɱ�
void CMsgProcThread::OnCmdService(S_DREB_RSMSG *msg)
{
	int snum=0;
	PCOMMSTRU databuf = &(msg->message);
	S_ROUTESVR_MSG *prs=NULL;
	S_SERVICE_ROUTE data;
	int nFuncNum=0;
	unsigned int  bufferlen=0;

	std::string routemsg;
	char tmpchar[300];
	std::vector<S_SERVICE_ROUTE *>servicelist;
	unsigned short nodeid;
	int  privateid;
	if (m_pSocketMgr->at(msg->msghead.index)->m_nType == SOCK_TODREB)
	{
		nodeid= m_pSocketMgr->at(msg->msghead.index)->m_s_center.nNodeId;
		privateid = m_pSocketMgr->at(msg->msghead.index)->m_s_center.cNodePrivateId;
	}
	else
	{
		nodeid= m_pSocketMgr->at(msg->msghead.index)->m_s_conn.nNodeId;
		privateid = m_pSocketMgr->at(msg->msghead.index)->m_s_conn.cNodePrivateId;
	}
	PrintServiceRoute();
	//20210126 songfree add ���Զ��͹�����·�ɣ���ʾû�д˽ڵ�û�й���ע�ᣬɾ��ԭ����
	if (databuf->head.cNextFlag == 0 && databuf->head.nLen == 0 ) 
	{
		//��ʾ�Զ���������һ���յģ�û�н���
		m_pMemDb->DeleteServiceByIndex(msg->msghead.index);
		return ;
	}
	//20210126 songfree add end

	//�ȸ���һ�·�����·�ɣ�ɾ�����ڵĹ���
	if (databuf->head.cNextFlag == 0) //ֻ��һ�����ݰ��Ľ���·��
	{
		m_pMemDb->UpdateRouteServiceByIndex(msg->msghead.index);
	}
	m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,\
		"�յ�����index=%d %s ��������service·�����ݳ���%d",msg->msghead.index,\
		GetIndexFlag(msg->msghead.index).c_str(),databuf->head.nLen);
	while (bufferlen<databuf->head.nLen)
	{
		if (databuf->head.nLen-bufferlen <sizeof(S_ROUTESVR_MSG))
		{
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"����·�ɲ���");
			break;
		}
		prs = (S_ROUTESVR_MSG *)(databuf->buffer+bufferlen);
		m_pDrebEndian.Endian2LocalHost((unsigned char *)&(prs->nNodeId),sizeof(prs->nNodeId));
		m_pDrebEndian.Endian2LocalHost((unsigned char *)&(prs->nSvrMainId),sizeof(prs->nSvrMainId));
		m_pDrebEndian.Endian2LocalHost((unsigned char *)&(prs->nFuncNo),sizeof(prs->nFuncNo));
		m_pDrebEndian.Endian2LocalHost((unsigned char *)&(prs->nFuncNum),sizeof(prs->nFuncNum));
		if (prs->nFuncNum*sizeof(unsigned int) > (databuf->head.nLen-bufferlen-11))
		{
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"�յ�����index=%d ��������service·����Ϣ�з�������Ŀ���� %d",\
				msg->msghead.index,prs->nFuncNum);
			m_pMemPool->PoolFree(msg);
			return;
		}
		if (prs->nFuncNum<1)
		{
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"�յ�����index=%d ��������service·����Ϣ�з�������Ŀ���� %d",\
				msg->msghead.index,prs->nFuncNum);
			m_pMemPool->PoolFree(msg);
			return ;
		}
		m_log->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"�յ�����index=%d ��������service·����Ŀ%d",msg->msghead.index,prs->nFuncNum);

		data.nNodeId = prs->nNodeId;
		data.nSvrMainId = prs->nSvrMainId;
		data.nFuncNo = prs->nFuncNo;
		data.cSvrPrivateId = prs->cSvrPrivateId;
		data.cNodePrivateId = prs->cNodePrivateId;
		data.nIndex = msg->msghead.index;
		data.nStep = prs->cStep+1;
		if (m_pSocketMgr->at(data.nIndex)->m_nType == SOCK_TODREB)
		{
			data.nbandwidth = m_pSocketMgr->at(data.nIndex)->m_s_center.nbandwidth;
		}
		else if (m_pSocketMgr->at(data.nIndex)->m_nType == SOCK_DREB)
		{
			data.nbandwidth = m_pSocketMgr->at(data.nIndex)->m_s_conn.nbandwidth;
		}
		
		if (m_pRes->g_nPrintRoute == 1)
		{
			sprintf(tmpchar,"\n   DREB[%d %d] SVR[%d %d] STEP[%d] FUNCNO[%d",\
				data.nNodeId,data.cNodePrivateId,data.nSvrMainId,data.cSvrPrivateId,data.nStep,data.nFuncNo);
			routemsg = routemsg+tmpchar;
		}
		nFuncNum++;
		m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"�յ�index=%d��service·��nNodeId=%d cNodePrivateId=%d nSvrMainId=%d cSvrPrivateId=%d nStep=%d nFuncNo=%d",\
			data.nIndex,data.nNodeId,data.cNodePrivateId,data.nSvrMainId,data.cSvrPrivateId,data.nStep,data.nFuncNo);
		//songfree 20210126 �޸���󲽽�Ϊ����
		if (data.nStep<= m_pRes->g_nMaxRouteStep)//��������16����Ϊ��ѭ��·�ɣ�ȥ��
		{
			if (data.nNodeId == m_pRes->g_nDrebId && data.cNodePrivateId == m_pRes->g_nDrebPrivateId)
			{
				//�Ǳ��ڵ��·�ɣ�ȥ��
			}
			else
			{
				m_pMemDb->AddServiceRoute(data);
			}
			
		}
		snum = prs->nFuncNum -1;
		bufferlen +=sizeof(S_ROUTESVR_MSG);;

		//20161014 �������Ǳ��صģ�����������
		if (prs->nNodeId == m_pRes->g_nDrebId && prs->cNodePrivateId == m_pRes->g_nDrebPrivateId)
		{
			m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"�������ڵ��·��  index=%d��service·��nNodeId=%d cNodePrivateId=%d nSvrMainId=%d cSvrPrivateId=%d nStep=%d nFuncNo=%d",\
			data.nIndex,data.nNodeId,data.cNodePrivateId,data.nSvrMainId,data.cSvrPrivateId,data.nStep,data.nFuncNo);
			continue;
		}

		for (int i=0;i <snum ; i++)
		{
			memcpy(&(data.nFuncNo),databuf->buffer+bufferlen,sizeof(unsigned int ));
			m_pDrebEndian.Endian2LocalHost((unsigned char *)&(data.nFuncNo),sizeof(unsigned int ));
			bufferlen +=sizeof(unsigned int );
			if (m_pRes->g_nPrintRoute == 1)
			{
				sprintf(tmpchar,",%d",data.nFuncNo);
				routemsg = routemsg+tmpchar;
			}
			//songfree 20210126 �޸���󲽽�Ϊ����
			if (data.nStep<=m_pRes->g_nMaxRouteStep)//��������16����Ϊ��ѭ��·�ɣ�ȥ��
			{
				if (data.nNodeId == m_pRes->g_nDrebId && data.cNodePrivateId == m_pRes->g_nDrebPrivateId)
				{
					//�Ǳ��ڵ��·�ɣ�ȥ��
				}
				else
				{
					if (!m_pMemDb->AddServiceRoute(data))
					{
						m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ӻ����·�ɳ��� �յ�index=%d��service·��nNodeId=%d cNodePrivateId=%d nSvrMainId=%d cSvrPrivateId=%d nStep=%d nFuncNo=%d",\
							data.nIndex,data.nNodeId,data.cNodePrivateId,data.nSvrMainId,data.cSvrPrivateId,data.nStep,data.nFuncNo);
						m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ӻ����·�ɳ���");
					}
				}
			}
			nFuncNum++;
		}
		if (m_pRes->g_nPrintRoute == 1)
		{
			sprintf(tmpchar,"]\n");
			routemsg = routemsg+tmpchar;
		}
	}
	if (databuf->head.cNextFlag == 0)
	{
		m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"INDEX=%d %s ��%d�������ܺ�·��",\
			msg->msghead.index,GetIndexFlag(msg->msghead.index).c_str(),nFuncNum);
	}
	else
	{
		m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"INDEX=%d %s ������%d�������ܺ�·��",\
			msg->msghead.index,GetIndexFlag(msg->msghead.index).c_str(),nFuncNum);
	}
	
	if (m_pRes->g_nPrintRoute == 1)
	{
		sprintf(tmpchar,"�յ�index[%d] DREB[%d %d] �Ľ���·��",msg->msghead.index,nodeid,privateid);
		m_pRes->g_pRouteLog.LogBin(LOG_INFO,__FILE__,__LINE__,tmpchar,(char *)routemsg.c_str(),routemsg.length());
	}

	m_pMemPool->PoolFree(msg);
	PrintServiceRoute();
	return ;
}	

void CMsgProcThread::TransMsgAns(S_DREB_RSMSG *msg)
{
	if (msg->message.head.s_Sinfo.s_nNodeId<1)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"����ʱ��DREB�ڵ㲻�� %d",msg->message.head.s_Sinfo.s_nNodeId);
		m_pMemPool->PoolFree(msg);
		return ;
	}
	if (msg->message.head.s_Sinfo.s_nNodeId == m_pRes->g_nDrebId && \
		msg->message.head.s_Sinfo.s_cNodePrivateId == m_pRes->g_nDrebPrivateId) //���Ǳ��������߽ڵ���
	{
		//ת��������
		if (m_pSocketMgr->at(msg->message.head.s_Sinfo.s_nIndex)->m_nType == SOCK_CLI && \
			m_pSocketMgr->at(msg->message.head.s_Sinfo.s_nIndex)->m_nConntime != msg->message.head.s_Sinfo.s_nTimestamp)
		{
			//�ͻ��������ѶϿ�������Ӧ����
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"Ӧ��ʱ�ͻ��������ѶϿ�[%d %s] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]",\
				msg->message.head.s_Sinfo.s_nIndex,GetIndexFlag(msg->message.head.s_Sinfo.s_nIndex).c_str(),\
				GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
				msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
				msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
				msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		        msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
			m_pMemPool->PoolFree(msg);
			return;
		}
		if (m_pSocketMgr->at(msg->message.head.s_Sinfo.s_nIndex)->m_nType == SOCK_CLI)
		{    //���ͻ��˵�Ӧ��
			msg->msghead.connecttime = msg->message.head.s_Sinfo.s_nTimestamp;
			if (m_log->isWrite(LOG_DEBUG))
			{
				m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"������Ӧ����ͻ���[%d %s] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]",\
					msg->message.head.s_Sinfo.s_nIndex,GetIndexFlag(msg->message.head.s_Sinfo.s_nIndex).c_str(),\
					GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
					msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
					msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
					msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
					msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
			}

			
			if (m_pSocketMgr->at(msg->message.head.s_Sinfo.s_nIndex)->SendMsg(msg)!=0)
			{
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"����ʧ��[%d %s] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]",\
					msg->message.head.s_Sinfo.s_nIndex,GetIndexFlag(msg->message.head.s_Sinfo.s_nIndex).c_str(),\
					GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
					msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
					msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
					msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		            msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
				return;
			}
			
		}
		else //�������Ӧ��
		{
			S_SVR_ROUTE svr;
			if (!m_pMemDb->SelectSvr(msg->message.head.s_Sinfo.s_nSvrMainId,msg->message.head.s_Sinfo.s_cSvrPrivateId,svr))
			{
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"Ӧ��ʱ����˷�����ע��[%d %s] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]",\
					msg->message.head.s_Sinfo.s_nIndex,GetIndexFlag(msg->message.head.s_Sinfo.s_nIndex).c_str(),\
					GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
					msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
					msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
					msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		            msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
				m_pMemPool->PoolFree(msg);
				return;
			}
			if (m_log->isWrite(LOG_DEBUG))
			{
				m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"������Ӧ��������[%d %s] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]",\
					msg->message.head.s_Sinfo.s_nIndex,GetIndexFlag(msg->message.head.s_Sinfo.s_nIndex).c_str(),\
					GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
					msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
					msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
					msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		            msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
			}
			
			//ת��������

			if (m_pSocketMgr->at(msg->message.head.s_Sinfo.s_nIndex)->SendMsg(msg)!=0)
			{
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"����ʧ��");
				return;
			}
		}
		return ;
		
	}
	//ͨ��·��ת��
	S_DREB_ROUTE dreb;
	if (msg->message.head.s_Sinfo.s_cNodePrivateId <1)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"Ӧ��ʱ�������߽ڵ�˽�нڵ�[%d] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]",\
			msg->message.head.s_Sinfo.s_cNodePrivateId,\
			GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
			msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
					msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
					msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		            msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
		m_pMemPool->PoolFree(msg);
		return;
	}
	else
	{
		if (!m_pMemDb->SelectRoute(msg->message.head.s_Sinfo.s_nNodeId,msg->message.head.s_Sinfo.s_cNodePrivateId,dreb))
		{
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"Ӧ��ʱ�������߽ڵ�·�ɲ�����[%d %d] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]",\
				msg->message.head.s_Sinfo.s_nNodeId,msg->message.head.s_Sinfo.s_cNodePrivateId,\
				GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
				msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
					msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
					msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		            msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
			m_pMemPool->PoolFree(msg);
			return;
		}
	}
	if (m_log->isWrite(LOG_DEBUG))
	{
		m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"������Ӧ���·��ת��[%d %d] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]",\
			dreb.nIndex,GetIndexFlag(dreb.nIndex).c_str(),\
			GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
			msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
			msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
			msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
			msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
	
	}
	
	if (m_pSocketMgr->at(dreb.nIndex)->SendMsg(msg)!=0)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"����ʧ��");
		return;
	}
	return;	
}

void CMsgProcThread::OnCmdServiceReg(S_DREB_RSMSG *msg)
{
	int snum=0;
	PCOMMSTRU databuf = &(msg->message);
	S_SERVICEREG_MSG *prs=NULL;
	S_SERVICE_ROUTE data;

	prs = (S_SERVICEREG_MSG *)(databuf->buffer);
	m_pDrebEndian.Endian2LocalHost((unsigned char *)&(prs->nSvrMainId),sizeof(prs->nSvrMainId));
	m_pDrebEndian.Endian2LocalHost((unsigned char *)&(prs->nFuncNo),sizeof(prs->nFuncNo));
	m_pDrebEndian.Endian2LocalHost((unsigned char *)&(prs->nFuncNum),sizeof(prs->nFuncNum));
	
	m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"����[%d %d]ע��   ���׸���[%d] len=[%d]",\
		prs->nSvrMainId,prs->cSvrPrivateId,prs->nFuncNum,msg->message.head.nLen);
	if (prs->nFuncNum <1)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"����[%d %d]ע�� ���׸���[%d]����",prs->nSvrMainId,prs->cSvrPrivateId,prs->nFuncNum);
		m_pMemPool->PoolFree(msg);
		return ;
	}
	if (msg->message.head.nLen <sizeof(S_SERVICEREG_MSG) + (prs->nFuncNum -1)*sizeof(unsigned int ))
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"����[%d %d]ע�� ���ĳ���[%d]���� ӦΪ[%d]",\
			prs->nSvrMainId,prs->cSvrPrivateId,prs->nFuncNum,msg->message.head.nLen,\
			sizeof(S_SERVICEREG_MSG) + (prs->nFuncNum -1)*sizeof(unsigned int));
		m_pMemPool->PoolFree(msg);
		return ;
	}

	data.nNodeId = m_pRes->g_nDrebId;
	data.nSvrMainId = prs->nSvrMainId;
	data.nFuncNo = prs->nFuncNo;
	data.cSvrPrivateId = prs->cSvrPrivateId;
	data.cNodePrivateId = m_pRes->g_nDrebPrivateId;
	data.nStep = 0;
	//20160112modi begin ��ע��ʱ�������������ŷŽ�����·�ɱ�
	//data.nIndex=0;
	data.nIndex = msg->msghead.index;
	data.nbandwidth = 10240000;
	//20160112modi end

	S_SVR_ROUTE svr;
	if (!m_pMemDb->SelectSvr(data.nSvrMainId,data.cSvrPrivateId,svr))
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"����ע��ʱ�ķ���[%d %d]�ڷ�����в�����",data.nSvrMainId,data.cSvrPrivateId);
		m_pMemPool->PoolFree(msg);
		return ;
	}
	if (svr.nIndex != data.nIndex)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"����ע��ʱ�ķ���[%d %d]���������[%d]��ע�������������[%d]��һ����",\
			data.nSvrMainId,data.cSvrPrivateId,data.nIndex,svr.nIndex);
		m_pMemPool->PoolFree(msg);
		return ;
	}
    //20130304 songfree add ���н���ע��ʱ��ɾ������ʱд��·�ɱ�ķ���·�� 
    if (prs->nFuncNum > 0)
    {
        m_log->LogMp(LOG_DEBUG + 1, __FILE__, __LINE__, "m_serviceTbl Delete   begin");
        m_pMemDb->m_serviceTbl.Delete(data.nIndex, m_pRes->g_nDrebId, m_pRes->g_nDrebPrivateId, data.nSvrMainId, data.cSvrPrivateId, 0);
        m_log->LogMp(LOG_DEBUG + 1, __FILE__, __LINE__, "m_serviceTbl Delete   end");
        m_log->LogMp(LOG_DEBUG + 1, __FILE__, __LINE__, "������[%d]���뽻�ױ�", data.nFuncNo);
        m_pMemDb->AddServiceRoute(data);
        m_log->LogMp(LOG_DEBUG + 1, __FILE__, __LINE__, "������[%d]���뽻�ױ����", data.nFuncNo);
    }
	//add end
	
	
	snum = prs->nFuncNum -1;
	
	int nFuncNum =  prs->nFuncNum;
	for (int i=0; i< snum; i++)
	{
		//20160107�޸�  data.nFuncNo = *func;��HP_UX������
		memcpy(&(data.nFuncNo),databuf->buffer+sizeof(S_SERVICEREG_MSG)+i*sizeof(unsigned int),sizeof(unsigned int));
		m_log->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"Endian2LocalHost begin" );
		m_pDrebEndian.Endian2LocalHost((unsigned char *)&(data.nFuncNo),sizeof(unsigned int ));
		m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"������[%d]���뽻�ױ�",data.nFuncNo );
		m_pMemDb->AddServiceRoute(data);
		m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"������[%d]���뽻�ױ����",data.nFuncNo );
	}

	m_log->LogMp(LOG_DEBUG+2,__FILE__,__LINE__,"����%d-%d�й���%d��ע��",data.nSvrMainId,data.cSvrPrivateId,nFuncNum);
	if (m_pRes->g_nRegOtherDreb == 1) //����������ע��
	{
		OnMsgServiceRoute(msg);
	}
	return ;
}

void CMsgProcThread::OnCmdUnServiceReg(S_DREB_RSMSG *msg)
{
	PCOMMSTRU databuf = &(msg->message);
	S_SERVICEREG_MSG *prs=NULL;
	S_SERVICE_ROUTE data;

	prs = (S_SERVICEREG_MSG *)(databuf->buffer);
	m_pDrebEndian.Endian2LocalHost((unsigned char *)&(prs->nSvrMainId),sizeof(prs->nSvrMainId));
	m_pDrebEndian.Endian2LocalHost((unsigned char *)&(prs->nFuncNo),sizeof(prs->nFuncNo));
	m_pDrebEndian.Endian2LocalHost((unsigned char *)&(prs->nFuncNum),sizeof(prs->nFuncNum));
	data.nNodeId = m_pRes->g_nDrebId;
	data.nSvrMainId = prs->nSvrMainId;
	data.nFuncNo = prs->nFuncNo;
	data.cSvrPrivateId = prs->cSvrPrivateId;
	data.cNodePrivateId = m_pRes->g_nDrebPrivateId;
	data.nStep = 0;
	data.nIndex = 0;
	S_SVR_ROUTE svr;
	if (!m_pMemDb->SelectSvr(data.nSvrMainId,data.cSvrPrivateId,svr))
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"ȡ������ע��ʱ�ķ���[%d %d]�ڷ�����в�����",data.nSvrMainId,data.cSvrPrivateId);
		m_pMemPool->PoolFree(msg);
		return ;
	}
	//ȡ��ע��
	m_pMemDb->UnRegisterService(data.nNodeId,data.cNodePrivateId,data.nSvrMainId,data.cSvrPrivateId);

	if (!svr.bIsClose)
	{
		//20130304 songfree add ��������ʱ����������뵽����·�ɱ���������·�ɣ�������Ĭ��Ϊ0
		S_SERVICE_ROUTE rt;
		bzero(&rt,sizeof(S_SERVICE_ROUTE));
		rt.cNodePrivateId = m_pRes->g_nDrebPrivateId;
		rt.cSvrPrivateId = data.cSvrPrivateId;
		rt.nFuncNo = 0;
		rt.nIndex = svr.nIndex;
		rt.nNodeId = m_pRes->g_nDrebId;
		rt.nStep = 0;
		rt.nSvrMainId = data.nSvrMainId;
		rt.nUpdateTime = time(NULL);
		m_pMemDb->AddServiceRoute(rt);
	//ADD END
	}
	
	return ;
}
void CMsgProcThread::OnCmdMonitorDreb(S_DREB_RSMSG *msg)
{
	if (msg->message.head.cRaflag == 1) //Ӧ��
	{
		TransMsgAns(msg);
		return;
	}
	if (msg->message.head.d_Dinfo.d_nNodeId >0) // ����ָ����dreb�ڵ�ģ��ж��Ƿ�ͨ��·��ת��
	{
		if (msg->message.head.d_Dinfo.d_nNodeId == m_pRes->g_nDrebId && msg->message.head.d_Dinfo.d_cNodePrivateId == m_pRes->g_nDrebPrivateId)
		{

		}
		else //���Ǳ��ڵ������·��ת��
		{
			S_DREB_ROUTE dreb;
			if (!m_pMemDb->SelectRoute(msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,dreb))
			{
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"·��[%d %d]�����ڣ��������������߽ڵ�δע��",msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId);
				AnsMsg(msg,ERR_DREBROUTE,"·�ɲ����ڣ��������������߽ڵ�δע��");
				//WriteErrLog(8,msg->message);
				return;
			}
		
			if (dreb.nIndex <1)
			{
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"��ô�����·��ָ������");
				m_pMemPool->PoolFree(msg);
				return ;
			}
			if (m_log->isWrite(LOG_DEBUG))
			{
				m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�����ݷ���·�ɴ���nIndex=%d %s nCmd=%d %s cRaFlag=%d cZip=%d s_nHook=%d s_nSerial=%d nLen=%d ",\
					dreb.nIndex,GetIndexFlag(dreb.nIndex).c_str(),msg->message.head.cCmd,\
					GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cRaflag,msg->message.head.cZip,\
				msg->message.head.s_Sinfo.s_nHook,msg->message.head.s_Sinfo.s_nSerial,msg->message.head.nLen);
			}
			
			if (!dreb.bIsClose)
			{
				m_pSocketMgr->at(dreb.nIndex)->SendMsg(msg);
			}
			else if (dreb.pSendQueue->PutSendMsg(msg)!=0)
			{
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"�뷢�Ͷ���ʧ��");
				return;
			}
			return;
		}
		
	}
// #define  DREBMONI_INFO          1     //dreb��Ϣ ��ǰ�ڵ㡢��ǰ�ڵ����
// #define  DREBMONI_HOST          2     //������Ϣ CPU ���� �ڴ�
// #define  DREBMONI_CURTXLIST     3     //dreb��Ϣ ��ǰ�ڵ㽻���б�
// #define  DREBMONI_ROUTEDREB     4     //dreb·����Ϣ �ڵ�·�ɱ�
// #define  DREBMONI_ROUTESERVICE  5     //dreb·����Ϣ ������·�ɱ�
// #define  DREBMONI_REPORT        6     //dreb����ͳ����Ϣ
// #define  DREBMONI_CONNINFO      7     //dreb������Ϣ
// #define  DREBMONI_REPORTSTART   8     //dreb��ʼͳ����Ϣ pack/s byte/s
// #define  DREBMONI_REPORTSTOP    9     //drebֹͣͳ����Ϣ
	switch (msg->message.head.d_Dinfo.d_nServiceNo)
	{
		case DREBMONI_INFO:
			Monitor_info(msg);
			break;
		case DREBMONI_HOST:
			Monitor_Host(msg);
			break;
		case DREBMONI_CURTXLIST:
			Monitor_CurTxList(msg);
			break;
		case DREBMONI_ROUTEDREB:
			Monitor_RouteDreb(msg);
			break;
		case DREBMONI_ROUTESERVICE:
			Monitor_RouteService(msg);
			break;
		case DREBMONI_REPORT:
			Monitor_Report(msg);
			break;
		case DREBMONI_CONNINFO:
			Monitor_ConnInfo(msg);
			break;
		case DREBMONI_REPORTSTART:
			Monitor_ReportStart(msg);
			break;
		case DREBMONI_REPORTSTOP:
			Monitor_ReportStop(msg);
			break;
		default:
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"�޴˼�ؽ������� %d",msg->message.head.d_Dinfo.d_nServiceNo);
			AnsMsg(msg,ERR_FUNCNOTFUND,"�޴˼�ؽ�������");
			//WriteErrLog(8,msg->message);
			break;
	}
	return;
}

void CMsgProcThread::OnCmdMonitorBpc(S_DREB_RSMSG *msg)
{
	if (msg->message.head.cRaflag == 1) //Ӧ��
	{
		TransMsgAns(msg);
		return;
	}
	if (msg->message.head.d_Dinfo.d_nSvrMainId<1)
	{
		//����δ��д
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"δָ�����������[%d]",msg->message.head.d_Dinfo.d_nSvrMainId);
		AnsMsg(msg,ERR_SVRNOTREGISTER,"δָ�����������");
		return ;
	}
	bool bRet;
	if (msg->message.head.d_Dinfo.d_nNodeId>0)
	{
		//�����ж��Ƿ����߽ڵ�
		if (msg->message.head.d_Dinfo.d_nNodeId == m_pRes->g_nDrebId)
		{
			//�����ڵ���ͬ
			if (0 == msg->message.head.d_Dinfo.d_cNodePrivateId || msg->message.head.d_Dinfo.d_cNodePrivateId == m_pRes->g_nDrebPrivateId)
			{
				S_SERVICE_ROUTE rt;
				//�����ڵ���ͬ��˽�нڵ�Ϊ0����Ǳ��ڵ�
				//�������ӵķ���
				if (0 == msg->message.head.d_Dinfo.d_cNodePrivateId )
				{
					bRet = m_pMemDb->SelectLocalBySvr(msg->message.head.d_Dinfo.d_nSvrMainId,rt);
				}
				else
				{
					bRet = m_pMemDb->SelectLocalBySvr(msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cNodePrivateId,rt);
				}
				if (!bRet)
				{
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"·�ɴ��� �������߽ڵ�-����·�ɣ��Ҳ������� [%d %d %d %d] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d]  ҵ�����ݳ���[%d]",\
						msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
						msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
						GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
						msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
						msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
						msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		        msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
					AnsMsg(msg,ERR_SVRNOTREGISTER,"�������߽ڵ�-����·�ɣ��Ҳ�������");
					return ;
				}
				//����ָ������
				//ת��������
				if (m_log->isWrite(LOG_DEBUG))
				{
					m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�����ݷ������ط����� [%d %s] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d]  ҵ�����ݳ���[%d]",\
						rt.nIndex,GetIndexFlag(rt.nIndex).c_str(),\
						GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
						msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
						msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
						msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
						msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
				}
				
				m_pSocketMgr->at(rt.nIndex)->SendMsg(msg);

				return ;
			}
			//�����ڵ���ͬ��˽�нڵ㲻��ͬ������ͨ�������·�ɷ�ʽ
		}
		
		S_DREB_ROUTE  dreb;
		//���Ǳ��ڵ㣬Ҫͨ��·��ת��
		if (0 == msg->message.head.d_Dinfo.d_cNodePrivateId)
		{
			bRet =m_pMemDb->SelectRoute(msg->message.head.d_Dinfo.d_nNodeId,dreb);
		}
		else
		{
			bRet =m_pMemDb->SelectRoute(msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,dreb);
		}
		if (!bRet)
		{
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"·�ɴ��� �Ҳ������߽ڵ�[%d %d]��·��",\
				msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId);
			AnsMsg(msg,ERR_DREBROUTE,"·�ɴ���");
			return;
		}
		if (0 == dreb.nStep)
		{
			//��ô�����أ����Ǳ��ڵ�Ӧ��������Ŷ�
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"·�ɴ��� [%d %d %d %d] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d]  ҵ�����ݳ���[%d]",\
				msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
				msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
				GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
				msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
				msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
				msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		        msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"·�ɴ��� ���߽ڵ�·�ɳ��� ��ǰ�ڵ�[%d %d] ·��Ŀ��ڵ�[%d %d] nStep[%d]",\
				m_pRes->g_nDrebId,m_pRes->g_nDrebPrivateId,dreb.nNodeId,dreb.cNodePrivateId,dreb.nStep);
			AnsMsg(msg,ERR_DREBROUTE,"·�ɴ���");
			return ;
		}
		if (m_log->isWrite(LOG_DEBUG))
		{
			m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�����ݷ���·�ɴ��� [%d %s] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d]  ҵ�����ݳ���[%d]",\
				dreb.nIndex,GetIndexFlag(dreb.nIndex).c_str(),\
				GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
				msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
				msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
				msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		        msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
		}
		
		if (!dreb.bIsClose)
		{
			m_pSocketMgr->at(dreb.nIndex)->SendMsg(msg);
		}
		else 
		{
			if (m_log->isWrite(LOG_WARNNING))
			{
				m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"·�������ѶϿ����ݻ�����[%d %d %d %d] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d]  ҵ�����ݳ���[%d]",\
					msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
					msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
					GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
					msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
					msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
					msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		        msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
			}
			
			if (dreb.pSendQueue->PutSendMsg(msg) !=0 )
			{
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"�뷢�Ͷ���ʧ��");
			}
		}
		return ;
	}
	else
	{
		RouteMsgRequestBySvr(msg,false);
	}
}

//DREBMONI_INFO
void CMsgProcThread::Monitor_info(S_DREB_RSMSG *msg)
{
	msg->message.head.cRaflag = 1;
	msg->message.head.cDrebAffirm = 0;
	msg->message.head.cZip = 0;
	msg->message.head.nLen = 0;
	msg->message.head.cNextFlag = 0;
	
	msg->message.head.a_Ainfo.a_cNodePrivateId = m_pRes->g_nDrebPrivateId;	
	msg->message.head.a_Ainfo.a_nNodeId = m_pRes->g_nDrebId;
	msg->message.head.a_Ainfo.a_nRetCode = SUCCESS;

	CBF_Xml xmlpack;
	xmlpack.SetNodeValueByPath("Monitor/�����ڵ�",false,m_pRes->g_nDrebId);
	xmlpack.SetNodeValueByPath("Monitor/˽�нڵ�",false,m_pRes->g_nDrebPrivateId);
	xmlpack.SetNodeValueByPath("Monitor/����ʱ��",false,m_pRes->g_pDrebStatInfo.m_sStartDate,false);
	//ȡ����ע��ķ����ڱ�dreb��ע���
	std::vector<S_SVR_ROUTE>svrlist;
	m_pMemDb->GetAllSvr(svrlist);
	for (unsigned int i=0 ; i<svrlist.size() ; i++)
	{
		CXmlNode *svrnode = xmlpack.AddNodeByPath("Monitor/�����б�",false,"����",false,"");
		if (svrnode != NULL)
		{
			svrnode->SetAttribute("���������",false,(int)svrlist[i].nSvrMainId);
			svrnode->SetAttribute("˽�з����",false,(int)svrlist[i].cSvrPrivateId);
			svrnode->SetAttribute("״̬",false,(int)svrlist[i].bIsClose);
			svrnode->SetAttribute("����",false,(int)svrlist[i].nIndex);
		}
	}
	int len = DREBDATASIZE;
	if (!xmlpack.ToBuffer(msg->message.buffer,len))
	{
		sprintf(msg->message.buffer,"���岻�㣬������dreb.txt�鿴");
		msg->message.head.nLen = strlen(msg->message.buffer);
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"���岻��");
		xmlpack.ToFile("dreb.txt");
	}
	else
	{
		msg->message.head.nLen = len;
	}

	
	if (m_pSocketMgr->at(msg->msghead.index)->SendMsg(msg)!=0)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"����Ӧ�����");
	}
	return ;
	
}
//DREBMONI_HOST
void CMsgProcThread::Monitor_Host(S_DREB_RSMSG *msg)
{
	msg->message.head.cRaflag = 1;
	msg->message.head.cDrebAffirm = 0;
	msg->message.head.cZip = 0;
	msg->message.head.nLen = 0;
	msg->message.head.cNextFlag = 0;
	
	msg->message.head.a_Ainfo.a_cNodePrivateId = m_pRes->g_nDrebPrivateId;	
	msg->message.head.a_Ainfo.a_nNodeId = m_pRes->g_nDrebId;
	msg->message.head.a_Ainfo.a_nRetCode = SUCCESS;
	
	CBF_Xml xmlpack;
	xmlpack.SetNodeValueByPath("Monitor/�����ڵ�",false,m_pRes->g_nDrebId);
	xmlpack.SetNodeValueByPath("Monitor/˽�нڵ�",false,m_pRes->g_nDrebPrivateId);
	xmlpack.SetNodeValueByPath("Monitor/����ʱ��",false,m_pRes->g_pDrebStatInfo.m_sStartDate,false);

	int total;
	int unused;
	m_pMemPool->PoolInfo(total,unused);
	xmlpack.SetNodeAttribute("Monitor/�ڴ滺���",false,"�ܷ���",false,total );
	xmlpack.SetNodeAttribute("Monitor/�ڴ滺���",false,"δʹ��",false,unused );
	xmlpack.SetNodeAttribute("Monitor/�ڴ滺���",false,"ÿ���С",false,(int)(sizeof(DREBQUEUEMSG_HEAD)+DREBBUFSIZE+1) );
	//ȡ������Ϣ
	S_MONITOR_HOST hf;
	V_MonitorDiskInfo di;
	g_pDrebTimer.GetHostInfo(hf,di);
	
	xmlpack.SetNodeValueByPath("Monitor/������Դ/CPU",false,(int)hf.nCpuRate);
	

	xmlpack.SetNodeValueByPath("Monitor/������Դ/�ڴ�",false,"",false);
	xmlpack.SetNodeAttribute("Monitor/������Դ/�ڴ�",false,"�ܴ�СM",false,(int)hf.nTotalMemory);
	xmlpack.SetNodeAttribute("Monitor/������Դ/�ڴ�",false,"ʹ��M",false,(int)hf.nUsedMemory);
	
	for (unsigned int i=0 ; i<di.size() ; i++)
	{
		CXmlNode *disknode = xmlpack.AddNodeByPath("Monitor/������Դ/�����б�",false,"����",false,"");
		if (disknode != NULL)
		{
			disknode->SetAttribute("����",false,di[i].sDiskName);
			disknode->SetAttribute("�ܴ�СM",false,(int)di[i].nTotalSpace);
			disknode->SetAttribute("ʹ�ô�СM",false,(int)di[i].nUsedSpace);
		}
	}
	int len = DREBDATASIZE;
	if (!xmlpack.ToBuffer(msg->message.buffer,len))
	{
		sprintf(msg->message.buffer,"���岻�㣬������host.txt�鿴");
		msg->message.head.nLen = strlen(msg->message.buffer);
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"���岻��");
		xmlpack.ToFile("host.txt");
	}
	else
	{
		msg->message.head.nLen = len;
	}

	if (m_pSocketMgr->at(msg->msghead.index)->SendMsg(msg)!=0)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"����Ӧ�����");
	}
	
	return ;
	
}
//DREBMONI_CURTXLIST
void CMsgProcThread::Monitor_CurTxList(S_DREB_RSMSG *msg)
{
	msg->message.head.cRaflag = 1;
	msg->message.head.cDrebAffirm = 0;
	msg->message.head.cZip = 0;
	msg->message.head.nLen = 0;
	msg->message.head.cNextFlag = 0;
	
	msg->message.head.a_Ainfo.a_cNodePrivateId = m_pRes->g_nDrebPrivateId;	
	msg->message.head.a_Ainfo.a_nNodeId = m_pRes->g_nDrebId;
	msg->message.head.a_Ainfo.a_nRetCode = SUCCESS;
	

	//ȡ���н���
	std::vector<S_SERVICE_ROUTE *> slist;
	m_pMemDb->GetAllServiceOrder(slist);
	CBF_Xml monixml;
	monixml.SetNodeValueByPath("Monitor/�����ڵ�",false,m_pRes->g_nDrebId);
	monixml.SetNodeValueByPath("Monitor/˽�нڵ�",false,m_pRes->g_nDrebPrivateId);
	monixml.SetNodeValueByPath("Monitor/����ʱ��",false,m_pRes->g_pDrebStatInfo.m_sStartDate,false);
	monixml.SetNodeValueByPath("Monitor/�����б�",false,"",false);
	char tmpchar[30];
	int nSvrMainId=0;
	int cSvrPrivateId=0;


	char svrname[40];
	char txnodename[100];
	bzero(svrname,sizeof(svrname));
	bzero(txnodename,sizeof(txnodename));
	for (unsigned int i=0 ; i<slist.size() ; i++)
	{
		if (slist[i]->nStep  != 0) //ȥ���Ǳ��صĽ�����Ϣ
		{
			continue;
		}
		if (nSvrMainId != slist[i]->nSvrMainId || cSvrPrivateId != slist[i]->cSvrPrivateId)
		{
			//һ���µķ���
			sprintf(svrname,"%d_%d",slist[i]->nSvrMainId,slist[i]->cSvrPrivateId);
			monixml.AddNodeByPath("Monitor/�����б�",false,svrname,false,"",false);
			nSvrMainId = slist[i]->nSvrMainId ;
			cSvrPrivateId = slist[i]->cSvrPrivateId;
		}
		sprintf(txnodename,"Monitor/�����б�/%s",svrname);
		sprintf(tmpchar,"%d",slist[i]->nFuncNo);
		monixml.AddNodeByPath(txnodename,false,tmpchar,false,"",false);
		
	}
	int ret = DREBDATASIZE;
	if (!monixml.ToBuffer(msg->message.buffer,ret))
	{
		sprintf(msg->message.buffer,"������%d̫�࣬���岻�㣬������curtxlist.txt�鿴",slist.size());
		msg->message.head.nLen = strlen(msg->message.buffer);
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"������̫�࣬���岻��");
		monixml.ToFile("curtxlist.txt");

	}
	else
	{
		msg->message.head.nLen = ret;
	}
	if (m_pSocketMgr->at(msg->msghead.index)->SendMsg(msg)!=0)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"����Ӧ�����");
	}
	
	return ;
	
}
//DREBMONI_ROUTEDREB
void CMsgProcThread::Monitor_RouteDreb(S_DREB_RSMSG *msg)
{
	msg->message.head.cRaflag = 1;
	msg->message.head.cDrebAffirm = 0;
	msg->message.head.cZip = 0;
	msg->message.head.nLen = 0;
	msg->message.head.cNextFlag = 0;
	
	msg->message.head.a_Ainfo.a_cNodePrivateId = m_pRes->g_nDrebPrivateId;	
	msg->message.head.a_Ainfo.a_nNodeId = m_pRes->g_nDrebId;
	msg->message.head.a_Ainfo.a_nRetCode = SUCCESS;
	
	CBF_Xml xmlpack;
	xmlpack.SetNodeValueByPath("Monitor/�����ڵ�",false,m_pRes->g_nDrebId);
	xmlpack.SetNodeValueByPath("Monitor/˽�нڵ�",false,m_pRes->g_nDrebPrivateId);
	xmlpack.SetNodeValueByPath("Monitor/����ʱ��",false,m_pRes->g_pDrebStatInfo.m_sStartDate,false);
	xmlpack.SetNodeValueByPath("Monitor/·�ɱ�",false,"",false);
	unsigned int i;
	std::vector<S_DREB_ROUTE *>dreblist;
	
	//ȡ����ֱ���Ľڵ���Ϣ
	m_pMemDb->GetAllDreb(dreblist);

	//ȡ���з�ֱ����DREB·��
	m_pMemDb->GetRouteList(dreblist);

	for (i=0 ; i<dreblist.size() ; i++)
	{
		CXmlNode *routenode = xmlpack.AddNodeByPath("Monitor/·�ɱ�",false,"·��",false,"");
		if (routenode != NULL)
		{
			routenode->SetAttribute("�����ڵ�",false,(int)dreblist[i]->nNodeId);
			routenode->SetAttribute("˽�нڵ�",false,(int)dreblist[i]->cNodePrivateId);
			routenode->SetAttribute("����",false,(int)dreblist[i]->nIndex);
			routenode->SetAttribute("����",false,(int)dreblist[i]->nStep);
			routenode->SetAttribute("����",false,(int)dreblist[i]->nbandwidth);
		}
	}


	int ret = DREBDATASIZE;
	if (!xmlpack.ToBuffer(msg->message.buffer,ret))
	{
		sprintf(msg->message.buffer,"���岻�㣬������drebroute.txt�鿴");
		msg->message.head.nLen = strlen(msg->message.buffer);
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"���岻��");
		xmlpack.ToFile("drebroute.txt");
		
	}
	else
	{
		msg->message.head.nLen = ret;
	}
	if (m_pSocketMgr->at(msg->msghead.index)->SendMsg(msg)!=0)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"����Ӧ�����");
	}
	
	return ;
	
}
//DREBMONI_ROUTESERVICE
void CMsgProcThread::Monitor_RouteService(S_DREB_RSMSG *msg)
{
	msg->message.head.cRaflag = 1;
	msg->message.head.cDrebAffirm = 0;
	msg->message.head.cZip = 0;
	msg->message.head.nLen = 0;
	msg->message.head.cNextFlag = 0;
	
	msg->message.head.a_Ainfo.a_cNodePrivateId = m_pRes->g_nDrebPrivateId;	
	msg->message.head.a_Ainfo.a_nNodeId = m_pRes->g_nDrebId;
	msg->message.head.a_Ainfo.a_nRetCode = SUCCESS;
	
	CBF_Xml xmlpack;
	xmlpack.SetNodeValueByPath("Monitor/�����ڵ�",false,m_pRes->g_nDrebId);
	xmlpack.SetNodeValueByPath("Monitor/˽�нڵ�",false,m_pRes->g_nDrebPrivateId);
	xmlpack.SetNodeValueByPath("Monitor/����ʱ��",false,m_pRes->g_pDrebStatInfo.m_sStartDate,false);
//	xmlpack.SetNodeValueByPath("Monitor/����·�ɱ�",false,"",false);
	
	unsigned int i;
	std::string routemsg;
	char tmpchar[DREBBUFSIZE];
	//ȡ���н���
	std::vector<S_SERVICE_ROUTE *> slist;
	m_pMemDb->GetAllServiceOrder(slist);

	sprintf(tmpchar,"\n  DREB SERVICE LIST,  TOTAL SERVICE:%d \n",slist.size());
	routemsg = routemsg+tmpchar;
	int index;
	int nodeid;
	int nodeprivateid;
	int svrid;
	int svrprivateid;
	
	for (i=0;i<slist.size();i++)
	{
		if (i == 0)
		{
			index = slist[i]->nIndex;
			nodeid = slist[i]->nNodeId;
			nodeprivateid = slist[i]->cNodePrivateId;
			svrid = slist[i]->nSvrMainId;
			svrprivateid = slist[i]->cSvrPrivateId;
			sprintf(tmpchar,"    INDEX=%d DREB[%d %d] SVR[%d %d] STEP=%d FUNC=",index,nodeid,nodeprivateid,svrid,svrprivateid,slist[i]->nStep);
			routemsg= routemsg+tmpchar;
		}
		if (index != slist[i]->nIndex || nodeid != slist[i]->nNodeId  || nodeprivateid != slist[i]->cNodePrivateId \
			|| svrid != slist[i]->nSvrMainId || svrprivateid != slist[i]->cSvrPrivateId)
		{
			index = slist[i]->nIndex;
			nodeid = slist[i]->nNodeId;
			nodeprivateid = slist[i]->cNodePrivateId;
			svrid = slist[i]->nSvrMainId;
			svrprivateid = slist[i]->cSvrPrivateId;
			sprintf(tmpchar,"\n    INDEX=%d DREB[%d %d] SVR[%d %d] STEP=%d FUNC=",index,nodeid,nodeprivateid,svrid,svrprivateid,slist[i]->nStep);
			routemsg= routemsg+tmpchar;
		}
		//sprintf(tmpchar," %d-i%d",servicelist[i]->nFuncNo,servicelist[i]->nIndex);
		sprintf(tmpchar," %d",slist[i]->nFuncNo);
		routemsg= routemsg+tmpchar;
	}
	routemsg= routemsg+"\n";

	xmlpack.AddNodeByPath("Monitor/",false,"����·�ɱ�",false,routemsg.c_str(),false);

	int ret = DREBDATASIZE;
	if (!xmlpack.ToBuffer(msg->message.buffer,ret))
	{
		sprintf(msg->message.buffer,"���岻��,������txroute.txt�ļ��鿴");
		msg->message.head.nLen = strlen(msg->message.buffer);
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"���岻��");
		xmlpack.ToFile("txroute.txt");
		
	}
	else
	{
		msg->message.head.nLen = ret;
	}
	if (m_pSocketMgr->at(msg->msghead.index)->SendMsg(msg)!=0)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"����Ӧ�����");
	}
	return;
	
}
//DREBMONI_REPORT
void CMsgProcThread::Monitor_Report(S_DREB_RSMSG *msg)
{
	msg->message.head.cRaflag = 1;
	msg->message.head.cDrebAffirm = 0;
	msg->message.head.cZip = 0;
	msg->message.head.nLen = 0;
	msg->message.head.cNextFlag = 0;
	
	msg->message.head.a_Ainfo.a_cNodePrivateId = m_pRes->g_nDrebPrivateId;	
	msg->message.head.a_Ainfo.a_nNodeId = m_pRes->g_nDrebId;
	msg->message.head.a_Ainfo.a_nRetCode = SUCCESS;
	

	CBF_Xml xmlpack;
	xmlpack.SetNodeValueByPath("Monitor/�����ڵ�",false,m_pRes->g_nDrebId);
	xmlpack.SetNodeValueByPath("Monitor/˽�нڵ�",false,m_pRes->g_nDrebPrivateId);
	xmlpack.SetNodeValueByPath("Monitor/����ʱ��",false,m_pRes->g_pDrebStatInfo.m_sStartDate,false);
	xmlpack.SetNodeValueByPath("Monitor/����ͳ��",false,"",false);

	xmlpack.AddNodeByPath("Monitor/����ͳ��",false,"��������",false,m_pRes->g_pDrebStatInfo.m_sStartDate,false);
	char tmpchar[100];
	sprintf(tmpchar,INT64PFM,g_pDrebTimer.m_sResultDrebInfo.m_nRcvPackClient);
	xmlpack.AddNodeByPath("Monitor/����ͳ��/ƽ��ÿ��",false,"���տͻ������ݰ���",false,tmpchar,false);

	sprintf(tmpchar,INT64PFM,g_pDrebTimer.m_sResultDrebInfo.m_nRcvByteClient);
	xmlpack.AddNodeByPath("Monitor/����ͳ��/ƽ��ÿ��",false,"���տͻ����ֽ���",false,tmpchar,false);

	sprintf(tmpchar,INT64PFM,g_pDrebTimer.m_sResultDrebInfo.m_nAnsPackClient);
	xmlpack.AddNodeByPath("Monitor/����ͳ��/ƽ��ÿ��",false,"Ӧ��ͻ������ݰ���",false,tmpchar,false);

	sprintf(tmpchar,INT64PFM,g_pDrebTimer.m_sResultDrebInfo.m_nAnsByteClient);
	xmlpack.AddNodeByPath("Monitor/����ͳ��/ƽ��ÿ��",false,"Ӧ��ͻ����ֽ���",false,tmpchar,false);

	sprintf(tmpchar,INT64PFM,g_pDrebTimer.m_sResultDrebInfo.m_nTransOutNum);
	xmlpack.AddNodeByPath("Monitor/����ͳ��/ƽ��ÿ��",false,"ת�����ݰ���",false,tmpchar,false);

	sprintf(tmpchar,INT64PFM,g_pDrebTimer.m_sResultDrebInfo.m_nTransOutByte);
	xmlpack.AddNodeByPath("Monitor/����ͳ��/ƽ��ÿ��",false,"ת�������ֽ���",false,tmpchar,false);

	sprintf(tmpchar,INT64PFM,g_pDrebTimer.m_sResultDrebInfo.m_nTransInNum);
	xmlpack.AddNodeByPath("Monitor/����ͳ��/ƽ��ÿ��",false,"ת�����ݰ���",false,tmpchar,false);

	sprintf(tmpchar,INT64PFM,g_pDrebTimer.m_sResultDrebInfo.m_nTransInByte);
	xmlpack.AddNodeByPath("Monitor/����ͳ��/ƽ��ÿ��",false,"ת�������ֽ���",false,tmpchar,false);

	sprintf(tmpchar,INT64PFM,g_pDrebTimer.m_sResultDrebInfo.m_nCallSrvNum);
	xmlpack.AddNodeByPath("Monitor/����ͳ��/ƽ��ÿ��",false,"���÷������",false,tmpchar,false);

	sprintf(tmpchar,INT64PFM,g_pDrebTimer.m_sResultDrebInfo.m_nCallSrvByte);
	xmlpack.AddNodeByPath("Monitor/����ͳ��/ƽ��ÿ��",false,"���÷����ֽ���",false,tmpchar,false);

	sprintf(tmpchar,INT64PFM,g_pDrebTimer.m_sResultDrebInfo.m_nSrvAnsNum);
	xmlpack.AddNodeByPath("Monitor/����ͳ��/ƽ��ÿ��",false,"����Ӧ����",false,tmpchar,false);

	sprintf(tmpchar,INT64PFM,g_pDrebTimer.m_sResultDrebInfo.m_nSrvAnsByte);
	xmlpack.AddNodeByPath("Monitor/����ͳ��/ƽ��ÿ��",false,"����Ӧ���ֽ���",false,tmpchar,false);



	sprintf(tmpchar,INT64PFM,m_pRes->g_pDrebStatInfo.m_drebinfo.m_nRcvPackClient);
	xmlpack.AddNodeByPath("Monitor/����ͳ��/�ۼ�",false,"���տͻ������ݰ���",false,tmpchar,false);
	
	sprintf(tmpchar,INT64PFM,m_pRes->g_pDrebStatInfo.m_drebinfo.m_nRcvByteClient);
	xmlpack.AddNodeByPath("Monitor/����ͳ��/�ۼ�",false,"���տͻ����ֽ���",false,tmpchar,false);
	
	sprintf(tmpchar,INT64PFM,m_pRes->g_pDrebStatInfo.m_drebinfo.m_nAnsPackClient);
	xmlpack.AddNodeByPath("Monitor/����ͳ��/�ۼ�",false,"Ӧ��ͻ������ݰ���",false,tmpchar,false);
	
	sprintf(tmpchar,INT64PFM,m_pRes->g_pDrebStatInfo.m_drebinfo.m_nAnsByteClient);
	xmlpack.AddNodeByPath("Monitor/����ͳ��/�ۼ�",false,"Ӧ��ͻ����ֽ���",false,tmpchar,false);
	
	sprintf(tmpchar,INT64PFM,m_pRes->g_pDrebStatInfo.m_drebinfo.m_nTransOutNum);
	xmlpack.AddNodeByPath("Monitor/����ͳ��/�ۼ�",false,"ת�����ݰ���",false,tmpchar,false);
	
	sprintf(tmpchar,INT64PFM,m_pRes->g_pDrebStatInfo.m_drebinfo.m_nTransOutByte);
	xmlpack.AddNodeByPath("Monitor/����ͳ��/�ۼ�",false,"ת�������ֽ���",false,tmpchar,false);
	
	sprintf(tmpchar,INT64PFM,m_pRes->g_pDrebStatInfo.m_drebinfo.m_nTransInNum);
	xmlpack.AddNodeByPath("Monitor/����ͳ��/�ۼ�",false,"ת�����ݰ���",false,tmpchar,false);
	
	sprintf(tmpchar,INT64PFM,m_pRes->g_pDrebStatInfo.m_drebinfo.m_nTransInByte);
	xmlpack.AddNodeByPath("Monitor/����ͳ��/�ۼ�",false,"ת�������ֽ���",false,tmpchar,false);
	
	sprintf(tmpchar,INT64PFM,m_pRes->g_pDrebStatInfo.m_drebinfo.m_nCallSrvNum);
	xmlpack.AddNodeByPath("Monitor/����ͳ��/�ۼ�",false,"���÷������",false,tmpchar,false);
	
	sprintf(tmpchar,INT64PFM,m_pRes->g_pDrebStatInfo.m_drebinfo.m_nCallSrvByte);
	xmlpack.AddNodeByPath("Monitor/����ͳ��/�ۼ�",false,"���÷����ֽ���",false,tmpchar,false);
	
	sprintf(tmpchar,INT64PFM,m_pRes->g_pDrebStatInfo.m_drebinfo.m_nSrvAnsNum);
	xmlpack.AddNodeByPath("Monitor/����ͳ��/�ۼ�",false,"����Ӧ����",false,tmpchar,false);
	
	sprintf(tmpchar,INT64PFM,m_pRes->g_pDrebStatInfo.m_drebinfo.m_nSrvAnsByte);
	xmlpack.AddNodeByPath("Monitor/����ͳ��/�ۼ�",false,"����Ӧ���ֽ���",false,tmpchar,false);

	int ret = DREBDATASIZE;
	if (!xmlpack.ToBuffer(msg->message.buffer,ret))
	{
		sprintf(msg->message.buffer,"���岻��,������drebstat.txt�ļ��鿴");
		msg->message.head.nLen = strlen(msg->message.buffer);
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"���岻��");
		xmlpack.ToFile("drebstat.txt");
		
	}
	else
	{
		msg->message.head.nLen = ret;
	}
	if (m_pSocketMgr->at(msg->msghead.index)->SendMsg(msg)!=0)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"����Ӧ�����");
	}

	return ;
	
}
//DREBMONI_CONNINFO
void CMsgProcThread::Monitor_ConnInfo(S_DREB_RSMSG *msg)
{
	msg->message.head.cRaflag = 1;
	msg->message.head.cDrebAffirm = 0;
	msg->message.head.cZip = 0;
	msg->message.head.nLen = 0;
	msg->message.head.cNextFlag = 0;
	
	msg->message.head.a_Ainfo.a_cNodePrivateId = m_pRes->g_nDrebPrivateId;	
	msg->message.head.a_Ainfo.a_nNodeId = m_pRes->g_nDrebId;
	msg->message.head.a_Ainfo.a_nRetCode = SUCCESS;
	
	CBF_Xml xmlpack;
	xmlpack.SetNodeValueByPath("Monitor/�����ڵ�",false,m_pRes->g_nDrebId);
	xmlpack.SetNodeValueByPath("Monitor/˽�нڵ�",false,m_pRes->g_nDrebPrivateId);
	xmlpack.SetNodeValueByPath("Monitor/����ʱ��",false,m_pRes->g_pDrebStatInfo.m_sStartDate,false);
	xmlpack.SetNodeValueByPath("Monitor/������Ϣ",false,"",false);
	unsigned int i;
	int svrconnnum=0;
	int drebnum=0;
	int cliconnnum=0;
	for ( i=m_pRes->g_pCenterList.size()+1 ; i<m_pRes->g_nMaxConnCount ; i++)
	{
		if (m_pSocketMgr->at(i)->m_sock != INVALID_SOCKET && m_pSocketMgr->at(i)->m_bChecked)
		{
			if (m_pSocketMgr->at(i)->m_nType == SOCK_DREB)
			{
				drebnum++;
			}
			if (m_pSocketMgr->at(i)->m_nType == SOCK_SRV)
			{
				svrconnnum++;
			}
			if (m_pSocketMgr->at(i)->m_nType == SOCK_CLI)
			{
				cliconnnum++;
			}
		}
	}
	xmlpack.SetNodeAttribute("Monitor/������Ϣ",false,"���ӹ�����DREB��",false,drebnum);
	xmlpack.SetNodeAttribute("Monitor/������Ϣ",false,"�ͻ�����Ŀ",false,cliconnnum);
	xmlpack.SetNodeAttribute("Monitor/������Ϣ",false,"������Ŀ",false,svrconnnum);
	for ( i=0 ; i<m_pRes->g_pCenterList.size() ; i++)
	{
		CXmlNode *tmpnode = xmlpack.AddNodeByPath("Monitor/������Ϣ",false,"DREB����",false,"",false);
		if (tmpnode != NULL)
		{
			tmpnode->SetAttribute("ip",false,m_pSocketMgr->at(i+1)->m_s_center.sAddress,false);
			tmpnode->SetAttribute("port",false,(int)m_pSocketMgr->at(i+1)->m_s_center.nPort);
			if (m_pSocketMgr->at(i+1)->m_s_center.bNeedConnect)
			{
				tmpnode->SetAttribute("����״̬",false,"��Ҫ����",false);
			}
			else
			{
				tmpnode->SetAttribute("����״̬",false,"����",false);
			}
			tmpnode->SetAttribute("�Զ˹����ڵ�",false,(int)m_pSocketMgr->at(i+1)->m_s_center.nNodeId);
			tmpnode->SetAttribute("�Զ�˽�нڵ�",false,(int)m_pSocketMgr->at(i+1)->m_s_center.cNodePrivateId);
			tmpnode->SetAttribute("����",false,m_pSocketMgr->at(i+1)->m_s_center.nbandwidth);
		}
	}
	
	int ret = DREBDATASIZE;
	if (!xmlpack.ToBuffer(msg->message.buffer,ret))
	{
		sprintf(msg->message.buffer,"���岻��");
		msg->message.head.nLen = strlen(msg->message.buffer);
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"���岻��");
		xmlpack.ToFile("connstatus.txt");
		
	}
	else
	{
		msg->message.head.nLen = ret;
	}
	if (m_pSocketMgr->at(msg->msghead.index)->SendMsg(msg)!=0)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"����Ӧ�����");
	}
	
	return ;
	
}

void CMsgProcThread::Monitor_ReportStart(S_DREB_RSMSG *msg)
{
	msg->message.head.cRaflag = 1;
	msg->message.head.cDrebAffirm = 0;
	msg->message.head.cZip = 0;
	msg->message.head.nLen = 0;
	msg->message.head.cNextFlag = 0;
	
	msg->message.head.a_Ainfo.a_cNodePrivateId = m_pRes->g_nDrebPrivateId;	
	msg->message.head.a_Ainfo.a_nNodeId = m_pRes->g_nDrebId;
	msg->message.head.a_Ainfo.a_nRetCode = SUCCESS;
	
	if (g_pDrebTimer.IsStoped())
	{
		if (g_pDrebTimer.StartTimer())
		{
			//������DREB��ض�ʱ��
			sprintf(msg->message.buffer,"����DREB��ض�ʱ���ɹ�");
			msg->message.head.nLen = strlen(msg->message.buffer);
		}
		else
		{
			msg->message.head.a_Ainfo.a_nRetCode = ERR_START_DREBTIMER;
			sprintf(msg->message.buffer,"����DREB��ض�ʱ��ʧ��");
			msg->message.head.nLen = strlen(msg->message.buffer);
		}
	}
	else
	{
		msg->message.head.a_Ainfo.a_nRetCode = ERR_START_DREBTIMER;
		sprintf(msg->message.buffer,"DREB��ض�ʱ���Ѿ�����,�޷�����");
		msg->message.head.nLen = strlen(msg->message.buffer);
	}
	
	if (m_pSocketMgr->at(msg->msghead.index)->SendMsg(msg)!=0)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"����Ӧ�����");
	}
	
	return ;
	
}

void CMsgProcThread::Monitor_ReportStop(S_DREB_RSMSG *msg)
{
	msg->message.head.cRaflag = 1;
	msg->message.head.cDrebAffirm = 0;
	msg->message.head.cZip = 0;
	msg->message.head.nLen = 0;
	msg->message.head.cNextFlag = 0;
	
	msg->message.head.a_Ainfo.a_cNodePrivateId = m_pRes->g_nDrebPrivateId;	
	msg->message.head.a_Ainfo.a_nNodeId = m_pRes->g_nDrebId;
	msg->message.head.a_Ainfo.a_nRetCode = SUCCESS;
	
	if (g_pDrebTimer.IsStoped())
	{
		msg->message.head.a_Ainfo.a_nRetCode = ERR_STOP_DREBTIMER;
		sprintf(msg->message.buffer,"DREB��ض�ʱ���Ѿ�ֹͣ,�޷���ͣ");
		msg->message.head.nLen = strlen(msg->message.buffer);
	}
	else
	{
		if (g_pDrebTimer.StopTimer())
		{
			//������DREB��ض�ʱ��
			sprintf(msg->message.buffer,"ֹͣDREB��ض�ʱ���ɹ�");
			msg->message.head.nLen = strlen(msg->message.buffer);
		}
		else
		{
			msg->message.head.a_Ainfo.a_nRetCode = ERR_STOP_DREBTIMER;
			sprintf(msg->message.buffer,"ֹͣDREB��ض�ʱ��ʧ��");
			msg->message.head.nLen = strlen(msg->message.buffer);
		}
	}
	
	if (m_pSocketMgr->at(msg->msghead.index)->SendMsg(msg)!=0)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"����Ӧ�����");
	}
	return ;
	
}
void CMsgProcThread::LogDrebHead(int loglevel, DREB_HEAD head, const char *msg, const char *filename, int fileline)
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
	m_log->LogMp(loglevel,filename,fileline,m_headMsg,msg);
}
void CMsgProcThread::TransBroadCast(S_DREB_RSMSG *msg, bool isaffirm)
{
	bool issendaffirm=false;//�Ƿ��ѷ���Ӧ��
	bool baffrim=false; //�Ƿ��ɱ��ڷ���Ӧ��
	if (isaffirm && msg->message.head.s_Sinfo.s_nNodeId == m_pRes->g_nDrebId && \
		msg->message.head.s_Sinfo.s_cNodePrivateId == m_pRes->g_nDrebPrivateId)
	{
		baffrim = true;
	}
	S_DREB_RSMSG *data=NULL;
	if (msg->message.head.d_Dinfo.d_nNodeId<1)
	{
		//���͸����е�ͨѶƽ̨
		std::vector<S_DREB_ROUTE *>sendtlist;
		if (!m_pMemDb->GetAllRouteBc(sendtlist))
		{
			m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"TransBroadCast ȡ����Ҫ���㲥��DREB�ڵ���Ϣ����");
			m_pMemPool->PoolFree(msg);
			return;
		}
		for (unsigned int i=0;i<sendtlist.size();i++)
		{
			data = (S_DREB_RSMSG *) m_pMemPool->PoolMalloc();
			if (data == NULL)
			{
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ڴ治�㣬�����������߽ڵ����ݿռ����");
				m_pMemPool->PoolFree(msg);
				return;
			}
			memcpy(data,msg,sizeof(S_DREB_RSMSG));
			data->message.head.d_Dinfo.d_nNodeId = sendtlist[i]->nNodeId;
			data->message.head.d_Dinfo.d_cNodePrivateId = sendtlist[i]->cNodePrivateId;
			if (m_log->isWrite(LOG_DEBUG+1))
			{
				m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"�������� [%d %d %d %d] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]",\
					data->message.head.d_Dinfo.d_nNodeId,data->message.head.d_Dinfo.d_cNodePrivateId,\
					data->message.head.d_Dinfo.d_nSvrMainId,data->message.head.d_Dinfo.d_cSvrPrivateId,\
					GetDrebCmdType(data->message.head.cCmd).c_str(),data->message.head.cNextFlag,\
					data->message.head.cRaflag,data->message.head.d_Dinfo.d_nServiceNo,data->message.head.s_Sinfo.s_nNodeId,\
					data->message.head.s_Sinfo.s_cNodePrivateId,data->message.head.s_Sinfo.s_nDrebSerial,\
					data->message.head.d_Dinfo.d_nNodeId, data->message.head.d_Dinfo.d_cNodePrivateId,\
					data->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,data->message.head.nLen);
			}
			
			if (m_pSocketMgr->at(sendtlist[i]->nIndex)->SendMsg(data)!=0)
			{
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"TransBroadCast ����ʧ��");
				m_pMemPool->PoolFree(msg);
				return;
			}
		}

	}
	if (msg->message.head.d_Dinfo.d_nNodeId == m_pRes->g_nDrebId)  //�ڵ���ͬ
	{
		if (msg->message.head.d_Dinfo.d_cNodePrivateId <1 || msg->message.head.d_Dinfo.d_cNodePrivateId == m_pRes->g_nDrebPrivateId) //���Ǳ��������߽ڵ���
		{
			//����˽�нڵ���ͬ��ָ��˽�нڵ�
			if (msg->message.head.d_Dinfo.d_nSvrMainId<1) //û��ָ������
			{
				if (0 != m_pRes->g_nSupportSubscribe)
				{
					 std::vector<S_SUBSCRIBE> subcribelist;
					 if (m_pMemDb->m_subscribe.GetIndex(msg->message.head.d_Dinfo.d_nServiceNo, subcribelist))
					 {
                         S_DREB_RSMSG* data = NULL;
                         for (unsigned int i = 0; i < subcribelist.size(); i++)
                         {
                             data = (S_DREB_RSMSG*)m_pMemPool->PoolMalloc();
                             if (data == NULL)
                             {
                                 m_log->LogMp(LOG_ERROR, __FILE__, __LINE__, "�ڴ治�㣬�����������߽ڵ����ݿռ����");
                                 m_pMemPool->PoolFree(msg);
                                 return;
                             }
                             memcpy(data, msg, sizeof(S_DREB_RSMSG));
                          
							 if (m_log->isWrite(LOG_DEBUG + 1))
							 {
								 m_log->LogMp(LOG_DEBUG + 1, __FILE__, __LINE__, "�������� [%d %d ]���ĵĹ㲥��Ϣ DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]", \
									 subcribelist[i].nSvrMainId, subcribelist[i].cSvrPrivateId, \
									GetDrebCmdType(data->message.head.cCmd).c_str(), data->message.head.cNextFlag, \
									data->message.head.cRaflag, data->message.head.d_Dinfo.d_nServiceNo, data->message.head.s_Sinfo.s_nNodeId, \
									data->message.head.s_Sinfo.s_cNodePrivateId, data->message.head.s_Sinfo.s_nDrebSerial, \
									data->message.head.d_Dinfo.d_nNodeId, data->message.head.d_Dinfo.d_cNodePrivateId, \
									data->message.head.d_Dinfo.d_nSvrMainId, data->message.head.d_Dinfo.d_cSvrPrivateId, data->message.head.nLen);
							 }

							 m_pSocketMgr->at(subcribelist[i].nIndex)->SendMsg(data);
                            
                         }
						 //���������˹㲥�ķ���
					 }
				}
				else
				{
                    //���͸����еķ���
					std::vector<S_SVR_ROUTE>svrlist;
                    if (!m_pMemDb->GetAllSvr(svrlist))
                    {
                        m_pMemPool->PoolFree(msg);
                        m_log->LogMp(LOG_ERROR, __FILE__, __LINE__, "TransBroadCast GetAllSvrʧ��");
                        return;
                    }
                    S_DREB_RSMSG* data = NULL;
                    for (unsigned int i = 0; i < svrlist.size(); i++)
                    {
                        data = (S_DREB_RSMSG*)m_pMemPool->PoolMalloc();
                        if (data == NULL)
                        {
                            m_log->LogMp(LOG_ERROR, __FILE__, __LINE__, "�ڴ治�㣬�����������߽ڵ����ݿռ����");
                            m_pMemPool->PoolFree(msg);
                            return;
                        }
                        memcpy(data, msg, sizeof(S_DREB_RSMSG));
                        if (!svrlist[i].bIsClose)
                        {
                            if (m_log->isWrite(LOG_DEBUG + 1))
                            {
                                m_log->LogMp(LOG_DEBUG + 1, __FILE__, __LINE__, "�������� [%d %d ] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]", \
                                    svrlist[i].nSvrMainId, svrlist[i].cSvrPrivateId, \
                                    GetDrebCmdType(data->message.head.cCmd).c_str(), data->message.head.cNextFlag, \
                                    data->message.head.cRaflag, data->message.head.d_Dinfo.d_nServiceNo, data->message.head.s_Sinfo.s_nNodeId, \
                                    data->message.head.s_Sinfo.s_cNodePrivateId, data->message.head.s_Sinfo.s_nDrebSerial, \
									data->message.head.d_Dinfo.d_nNodeId, data->message.head.d_Dinfo.d_cNodePrivateId, \
									data->message.head.d_Dinfo.d_nSvrMainId, data->message.head.d_Dinfo.d_cSvrPrivateId, data->message.head.nLen);
                            }

                            m_pSocketMgr->at(svrlist[i].nIndex)->SendMsg(data);
                        }
                        else
                        {
                            m_log->LogMp(LOG_ERROR, __FILE__, __LINE__, "���������ѶϿ����ݻ�����[%d %d %d %d] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] ����[%d %d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]", \
                                data->message.head.d_Dinfo.d_nNodeId, data->message.head.d_Dinfo.d_cNodePrivateId, \
                                data->message.head.d_Dinfo.d_nSvrMainId, data->message.head.d_Dinfo.d_cSvrPrivateId, \
                                GetDrebCmdType(data->message.head.cCmd).c_str(), data->message.head.cNextFlag, \
                                data->message.head.cRaflag, data->message.head.d_Dinfo.d_nServiceNo, data->message.head.s_Sinfo.s_nNodeId, \
                                data->message.head.s_Sinfo.s_cNodePrivateId, msg->message.head.s_Sinfo.s_nDrebSerial, \
                                svrlist[i].nSvrMainId, svrlist[i].cSvrPrivateId, \
								data->message.head.d_Dinfo.d_nNodeId, data->message.head.d_Dinfo.d_cNodePrivateId, \
								data->message.head.d_Dinfo.d_nSvrMainId, data->message.head.d_Dinfo.d_cSvrPrivateId, data->message.head.nLen);
                            if (svrlist[i].pSendQueue->PutSendMsg(data) != 0)
                            {
                                m_log->LogMp(LOG_ERROR, __FILE__, __LINE__, "�뷢�Ͷ���ʧ��");
                                m_pMemPool->PoolFree(msg);
                                return;
                            }

                        }
                    }
					//���͸����з��񷵻�
				}
				
				if (baffrim && !issendaffirm)
				{
					AnsMsg(msg,SUCCESS,"SUCCESS");
					issendaffirm = true;
				}
				else
				{
					m_pMemPool->PoolFree(msg);
				}
				//���͸����з�������˴˹㲥�ķ���
				return;
			}
			S_SVR_ROUTE svr;
			//ָ���˹��������
			if (msg->message.head.d_Dinfo.d_cSvrPrivateId <1) //δָ��˽�з����
			{
				if (!m_pMemDb->SelectASvr(msg->message.head.d_Dinfo.d_nSvrMainId,svr))
				{
					//�޴˷���
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"�޴˷���[%d %d %d %d] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]",\
						msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
						msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
						GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
						msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
						msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
						msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
						msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
					AnsMsg(msg,ERR_SVRNOTREGISTER,"�޴˷���");
					return ;
				}
			}
			else
			{
				if (!m_pMemDb->SelectSvr(msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,svr))
				{
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"�޴˷���[%d %d %d %d] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]",\
						msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
						msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
						GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
						msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
						msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
						msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
						msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
					AnsMsg(msg,ERR_SVRNOTREGISTER,"�޴˷���");
					return;
				}
			}
			
			data = (S_DREB_RSMSG *) m_pMemPool->PoolMalloc();
			if (data == NULL)
			{
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ڴ治�㣬�����������߽ڵ����ݿռ����");
				m_pMemPool->PoolFree(msg);
				return;
			}
			memcpy(&(data->msghead),&(msg->msghead),sizeof(DREBQUEUEMSG_HEAD));
			memcpy(&(data->message),&(msg->message),sizeof(COMMSTRU));
			//ת��������
			if (!svr.bIsClose)
			{
				if (m_log->isWrite(LOG_DEBUG+1))
				{
					m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"�������� [%d %d ] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]",\
						svr.nSvrMainId,svr.cSvrPrivateId,\
						GetDrebCmdType(data->message.head.cCmd).c_str(),data->message.head.cNextFlag,\
						data->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,data->message.head.s_Sinfo.s_nNodeId,\
						data->message.head.s_Sinfo.s_cNodePrivateId,data->message.head.s_Sinfo.s_nDrebSerial,\
						data->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
						data->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,data->message.head.nLen);
				}
				
				m_pSocketMgr->at(svr.nIndex)->SendMsg(data);
			}
			else 
			{
				if (m_log->isWrite(LOG_WARNNING))
				{
					m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"���������ѶϿ����ݻ�����[%d %d %d %d] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]",\
						data->message.head.d_Dinfo.d_nNodeId,data->message.head.d_Dinfo.d_cNodePrivateId,\
						data->message.head.d_Dinfo.d_nSvrMainId,data->message.head.d_Dinfo.d_cSvrPrivateId,\
						GetDrebCmdType(data->message.head.cCmd).c_str(),data->message.head.cNextFlag,\
						data->message.head.cRaflag,data->message.head.d_Dinfo.d_nServiceNo,data->message.head.s_Sinfo.s_nNodeId,\
						data->message.head.s_Sinfo.s_cNodePrivateId,data->message.head.s_Sinfo.s_nDrebSerial,\
						data->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
						data->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,data->message.head.nLen);
				}
				
				if (svr.pSendQueue->PutSendMsg(data)!=0)
				{
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"�뷢�Ͷ���ʧ��");
				}
			}
			if (baffrim && !issendaffirm)
			{
				AnsMsg(msg,SUCCESS,"SUCCESS");
				issendaffirm = true;
			}
			else
			{
				m_pMemPool->PoolFree(msg);
			}
			//���͸�ָ���˷��񷵻�
			return ;
		} // end 	if (msg->message.head.d_Dinfo.d_cNodePrivateId <1 || msg->message.head.d_Dinfo.d_cNodePrivateId == m_pRes->g_nDrebPrivateId) //���Ǳ��������߽ڵ���
		else 
		{
			//���Ǳ����ݽڵ㣬�����ڵ����ͬ��˽�в�ͬ����������·��ת��
			
		}
	}
	else if (msg->message.head.d_Dinfo.d_nNodeId <1)  //û��ָ���ڵ�
	{
		if (msg->message.head.d_Dinfo.d_cNodePrivateId <1 || msg->message.head.d_Dinfo.d_cNodePrivateId == m_pRes->g_nDrebPrivateId) //���Ǳ��������߽ڵ���
		{
			//����˽�нڵ���ͬ��ָ��˽�нڵ�
			if (msg->message.head.d_Dinfo.d_nSvrMainId<1) //û��ָ������
			{
                if (0 != m_pRes->g_nSupportSubscribe)
                {
                    std::vector<S_SUBSCRIBE> subcribelist;
                    if (m_pMemDb->m_subscribe.GetIndex(msg->message.head.d_Dinfo.d_nServiceNo, subcribelist))
                    {
                        S_DREB_RSMSG* data = NULL;
                        for (unsigned int i = 0; i < subcribelist.size(); i++)
                        {
                            data = (S_DREB_RSMSG*)m_pMemPool->PoolMalloc();
                            if (data == NULL)
                            {
                                m_log->LogMp(LOG_ERROR, __FILE__, __LINE__, "�ڴ治�㣬�����������߽ڵ����ݿռ����");
                                m_pMemPool->PoolFree(msg);
                                return;
                            }
                            memcpy(data, msg, sizeof(S_DREB_RSMSG));

                            if (m_log->isWrite(LOG_DEBUG + 1))
                            {
                                m_log->LogMp(LOG_DEBUG + 1, __FILE__, __LINE__, "�������� [%d %d ]���ĵĹ㲥��Ϣ DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]", \
                                    subcribelist[i].nSvrMainId, subcribelist[i].cSvrPrivateId, \
                                    GetDrebCmdType(data->message.head.cCmd).c_str(), data->message.head.cNextFlag, \
                                    data->message.head.cRaflag, data->message.head.d_Dinfo.d_nServiceNo, data->message.head.s_Sinfo.s_nNodeId, \
                                    data->message.head.s_Sinfo.s_cNodePrivateId, data->message.head.s_Sinfo.s_nDrebSerial, \
									data->message.head.d_Dinfo.d_nNodeId, data->message.head.d_Dinfo.d_cNodePrivateId, \
									data->message.head.d_Dinfo.d_nSvrMainId, data->message.head.d_Dinfo.d_cSvrPrivateId, data->message.head.nLen);
                            }

                            m_pSocketMgr->at(subcribelist[i].nIndex)->SendMsg(data);

                        }
                        //���������˹㲥�ķ���
                    }
                }
                else
                {
					//���͸����еķ���
					std::vector<S_SVR_ROUTE>svrlist;
					if (!m_pMemDb->GetAllSvr(svrlist))
					{
						m_pMemPool->PoolFree(msg);
						m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"TransBroadCast GetAllSvrʧ��");
						return;
					}
					S_DREB_RSMSG *data= NULL;
					for (unsigned int i=0;i<svrlist.size();i++)
					{
						data  = (S_DREB_RSMSG *) m_pMemPool->PoolMalloc();
						if (data == NULL)
						{
							m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ڴ治�㣬�����������߽ڵ����ݿռ����");
							m_pMemPool->PoolFree(msg);
							return;
						}
						memcpy(data,msg,sizeof(S_DREB_RSMSG));
						if (!svrlist[i].bIsClose)
						{
							if (m_log->isWrite(LOG_DEBUG+1))
							{
								m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"�������� [%d %d ] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]",\
									svrlist[i].nSvrMainId,svrlist[i].cSvrPrivateId,\
									GetDrebCmdType(data->message.head.cCmd).c_str(),data->message.head.cNextFlag,\
									data->message.head.cRaflag,data->message.head.d_Dinfo.d_nServiceNo,data->message.head.s_Sinfo.s_nNodeId,\
									data->message.head.s_Sinfo.s_cNodePrivateId,data->message.head.s_Sinfo.s_nDrebSerial,\
									data->message.head.d_Dinfo.d_nNodeId, data->message.head.d_Dinfo.d_cNodePrivateId,\
									data->message.head.d_Dinfo.d_nSvrMainId, data->message.head.d_Dinfo.d_cSvrPrivateId,data->message.head.nLen);
							}
						
							m_pSocketMgr->at(svrlist[i].nIndex)->SendMsg(data);
						}
						else
						{
							m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"���������ѶϿ����ݻ�����[%d %d %d %d] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] ����[%d %d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]",\
								data->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
								data->message.head.d_Dinfo.d_nSvrMainId,data->message.head.d_Dinfo.d_cSvrPrivateId,\
								GetDrebCmdType(data->message.head.cCmd).c_str(),data->message.head.cNextFlag,\
								data->message.head.cRaflag,data->message.head.d_Dinfo.d_nServiceNo,data->message.head.s_Sinfo.s_nNodeId,\
								data->message.head.s_Sinfo.s_cNodePrivateId,data->message.head.s_Sinfo.s_nDrebSerial,\
								svrlist[i].nSvrMainId,svrlist[i].cSvrPrivateId,\
								data->message.head.d_Dinfo.d_nNodeId, data->message.head.d_Dinfo.d_cNodePrivateId,\
								data->message.head.d_Dinfo.d_nSvrMainId, data->message.head.d_Dinfo.d_cSvrPrivateId,data->message.head.nLen);
							if (svrlist[i].pSendQueue->PutSendMsg(data)!=0)
							{
								m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"�뷢�Ͷ���ʧ��");
								m_pMemPool->PoolFree(msg);
								return;
							}
						
						}
					}
					//���͸����з��񷵻�
				}
				if (baffrim && !issendaffirm)
				{
					AnsMsg(msg,SUCCESS,"SUCCESS");
					issendaffirm = true;
				}
				else
				{
					m_pMemPool->PoolFree(msg);
				}
				//���͸����з�������˴˹㲥�ķ���
				return;
			}
			S_SVR_ROUTE svr;
			//ָ���˹��������
			if (msg->message.head.d_Dinfo.d_cSvrPrivateId <1) //δָ��˽�з����
			{
				if (!m_pMemDb->SelectASvr(msg->message.head.d_Dinfo.d_nSvrMainId,svr))
				{
					//�޴˷���
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"�޴˷���[%d %d %d %d] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]",\
						msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
						msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
						GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
						msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
						msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
						msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
						msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
					m_pMemPool->PoolFree(msg);
					return ;
				}
			}
			else
			{
				if (!m_pMemDb->SelectSvr(msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,svr))
				{
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"�޴˷���[%d %d %d %d] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]",\
						msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
						msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
						GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
						msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
						msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
						msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
						msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
					m_pMemPool->PoolFree(msg);
					return;
				}
			}
			
			data = (S_DREB_RSMSG *) m_pMemPool->PoolMalloc();
			if (data == NULL)
			{
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ڴ治�㣬�����������߽ڵ����ݿռ����");
				m_pMemPool->PoolFree(msg);
				return;
			}
			memcpy(&(data->msghead),&(msg->msghead),sizeof(DREBQUEUEMSG_HEAD));
			memcpy(&(data->message),&(msg->message),sizeof(COMMSTRU));
			//ת��������
			if (!svr.bIsClose)
			{
				if (m_log->isWrite(LOG_DEBUG+1))
				{
					m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"�������� [%d %d ] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]",\
						svr.nSvrMainId,svr.cSvrPrivateId,\
						GetDrebCmdType(data->message.head.cCmd).c_str(),data->message.head.cNextFlag,\
						data->message.head.cRaflag,data->message.head.d_Dinfo.d_nServiceNo,data->message.head.s_Sinfo.s_nNodeId,\
						data->message.head.s_Sinfo.s_cNodePrivateId,data->message.head.s_Sinfo.s_nDrebSerial,\
						data->message.head.d_Dinfo.d_nNodeId, data->message.head.d_Dinfo.d_cNodePrivateId,\
						data->message.head.d_Dinfo.d_nSvrMainId, data->message.head.d_Dinfo.d_cSvrPrivateId,data->message.head.nLen);
				}
				
				m_pSocketMgr->at(svr.nIndex)->SendMsg(data);
			}
			else 
			{
				if (m_log->isWrite(LOG_WARNNING))
				{
					m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"���������ѶϿ����ݻ�����[%d %d %d %d] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]",\
						data->message.head.d_Dinfo.d_nNodeId,data->message.head.d_Dinfo.d_cNodePrivateId,\
						data->message.head.d_Dinfo.d_nSvrMainId,data->message.head.d_Dinfo.d_cSvrPrivateId,\
						GetDrebCmdType(data->message.head.cCmd).c_str(),data->message.head.cNextFlag,\
						data->message.head.cRaflag,data->message.head.d_Dinfo.d_nServiceNo,data->message.head.s_Sinfo.s_nNodeId,\
						data->message.head.s_Sinfo.s_cNodePrivateId,data->message.head.s_Sinfo.s_nDrebSerial,\
						data->message.head.d_Dinfo.d_nNodeId, data->message.head.d_Dinfo.d_cNodePrivateId,\
						data->message.head.d_Dinfo.d_nSvrMainId, data->message.head.d_Dinfo.d_cSvrPrivateId,data->message.head.nLen);
				}
				
				if (svr.pSendQueue->PutSendMsg(data)!=0)
				{
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"�뷢�Ͷ���ʧ��");
				}
			}
			if (baffrim && !issendaffirm)
			{
				AnsMsg(msg,SUCCESS,"SUCCESS");
				issendaffirm = true;
			}
			else
			{
				m_pMemPool->PoolFree(msg);
			}
			return ;
		} // end if (msg->message.head.d_Dinfo.d_cNodePrivateId <1 || msg->message.head.d_Dinfo.d_cNodePrivateId == m_pRes->g_nDrebPrivateId) //���Ǳ��������߽ڵ���
		//ָ����˽�нڵ㣬��Ϊǰ����ת������dreb�ڵ㣬��������ֱ�ӷ���
		m_log->LogMp(LOG_PROMPT,__FILE__,__LINE__,"�����ڵ㲻��ͬ��δָ�������ڵ㣬��ָ����˽�нڵ㣬��Ϊ���ڵ㴦�����ڵ㴦�����");
		m_pMemPool->PoolFree(msg);
		return ;
	}
	//ͨ��·��ת��
	S_DREB_ROUTE dreb;
	if (msg->message.head.d_Dinfo.d_cNodePrivateId <1)
	{
		if (!m_pMemDb->SelectRoute(msg->message.head.d_Dinfo.d_nNodeId,dreb))
		{
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"·�ɲ����ڣ��������������߽ڵ�δע��[%d %d %d %d] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]",\
				msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
				msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
				GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
				msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
				msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
				msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		        msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
			AnsMsg(msg,ERR_DREBROUTE,"·�ɲ����ڣ��������������߽ڵ�δע��");	
			return;
		}
	}
	else
	{
		if (!m_pMemDb->SelectRoute(msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,dreb))
		{
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"·�ɲ����ڣ��������������߽ڵ�δע��[%d %d %d %d] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]",\
				msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
				msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
				GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
				msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
				msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
				msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		        msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
				
			AnsMsg(msg,ERR_DREBROUTE,"·�ɲ����ڣ��������������߽ڵ�δע��");
			return;
		}
	}
	if (dreb.nIndex <1)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"��ô�����·��ָ������[%d %d %d %d] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]",\
			msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
			msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
			GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
			msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
			msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
			msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		        msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
		m_pMemPool->PoolFree(msg);		
		return ;
	}
	//songfree modi 20160607  
// 	if (baffrim && !issendaffirm)
// 	{
		data = (S_DREB_RSMSG *) m_pMemPool->PoolMalloc();
		if (data == NULL)
		{
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ڴ治�㣬�����������߽ڵ����ݿռ����");
			m_pMemPool->PoolFree(msg);
			return;
		}
		memcpy(&(data->msghead),&(msg->msghead),sizeof(DREBQUEUEMSG_HEAD));
		memcpy(&(data->message),&(msg->message),sizeof(COMMSTRU));
//	}
	//songfree modi 20160607  end 
	if (!dreb.bIsClose)
	{
		if (m_log->isWrite(LOG_DEBUG+1))
		{
			m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"�������� [%d %d] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]",\
				dreb.nNodeId,dreb.cNodePrivateId,\
				GetDrebCmdType(data->message.head.cCmd).c_str(),data->message.head.cNextFlag,\
				data->message.head.cRaflag,data->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
				data->message.head.s_Sinfo.s_cNodePrivateId,data->message.head.s_Sinfo.s_nDrebSerial,\
				msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		        msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,data->message.head.nLen);
		}
		
		m_pSocketMgr->at(dreb.nIndex)->SendMsg(data);
	}
	else 
	{
		if (m_log->isWrite(LOG_WARNNING))
		{
			m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"·�������ѶϿ����ݻ�����[%d %d %d %d] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]",\
				msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
				msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
				GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
				msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
				msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
				msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		        msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
		}
		
		if (dreb.pSendQueue->PutSendMsg(msg)!=0)
		{
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"�뷢�Ͷ���ʧ��");
		}
	}
	if (baffrim && !issendaffirm)
	{
		AnsMsg(msg,SUCCESS,"SUCCESS");
		issendaffirm = true;
	}
	else
	{
		m_pMemPool->PoolFree(msg);
	}
	return ;
}


void CMsgProcThread::TransMsgRequest(S_DREB_RSMSG *msg, bool isaffirm)
{
	RouteMsgRequest(msg,isaffirm);
	return;
}


void CMsgProcThread::OnMsgMonitor(S_DREB_RSMSG *msg)
{
	int i;
	CBF_Xml        pXml;
	pXml.SetNodeValueByPath("Monitor/�����ڵ�",false,m_pRes->g_nDrebId);
	pXml.SetNodeValueByPath("Monitor/˽�нڵ�",false,m_pRes->g_nDrebPrivateId);
	pXml.SetNodeValueByPath("Monitor/�ڵ�����",false,0);//dreb
	pXml.SetNodeValueByPath("Monitor/��������",false,0);//����
	pXml.SetNodeValueByPath("Monitor/����ʱ��",false,m_pRes->g_pDrebStatInfo.m_sStartDate,false);
	
	int total=0;
	int used=0;
	m_pMemPool->PoolInfo(total,used);
	pXml.SetNodeAttribute("Monitor/�ڴ滺���",false,"�ܷ���",false,total );
	pXml.SetNodeAttribute("Monitor/�ڴ滺���",false,"δʹ��",false,used );
	pXml.SetNodeAttribute("Monitor/�ڴ滺���",false,"ÿ���С",false,(int)(DREBBUFSIZE+sizeof(DREBQUEUEMSG_HEAD)+1));

	if (m_pRes->g_nMonitorHost == 1)
	{
		S_MONITOR_HOST hostinfo;
		V_MonitorDiskInfo  diskinfo;
		g_pDrebTimer.GetHostInfo(hostinfo,diskinfo);
		pXml.SetNodeValueByPath("Monitor/������Դ/CPU",false,(int)hostinfo.nCpuRate);
		pXml.SetNodeAttribute("Monitor/������Դ/�ڴ�",false,"total",false,(int)hostinfo.nTotalMemory);
		pXml.SetNodeAttribute("Monitor/������Դ/�ڴ�",false,"used",false,(int)hostinfo.nUsedMemory);
		for (i=0 ; i< diskinfo.size() ; i++)
		{
			CXmlNode *disknode = pXml.AddNodeByPath("Monitor/������Դ/����",false,"����",false,"");
			if (disknode != NULL)
			{
				disknode->SetAttribute("drive",false,diskinfo[i].sDiskName,true);
				disknode->SetAttribute("total",false,(int)diskinfo[i].nTotalSpace);
				disknode->SetAttribute("used",false,(int)diskinfo[i].nUsedSpace);
			}
		}
	}
	//������Ϣ
	for (i=0 ; i < m_pRes->g_pCenterList.size() ; i++)
	{
		CXmlNode *connnode = pXml.AddNodeByPath("Monitor/������Ϣ",false,m_pRes->g_pCenterList[i].sname.c_str(),false,"");
		if (connnode == NULL)
		{
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"AddNodeByPath ����");
			continue;
		}
		connnode->SetAttribute("ip",false,m_pRes->g_pCenterList[i].sIp);
		connnode->SetAttribute("port",false,(int)m_pRes->g_pCenterList[i].nPort);
		connnode->SetAttribute("index",false,i+1);
		connnode->SetAttribute("�����ڵ�",false,m_pSocketMgr->at(i+1)->m_s_center.nNodeId);
		connnode->SetAttribute("˽�нڵ�",false,m_pSocketMgr->at(i+1)->m_s_center.cNodePrivateId);
		if (m_pSocketMgr->at(i+1)->m_sock == INVALID_SOCKET)
		{
			connnode->SetAttribute("status",false,"δ����");
		}
		else if (m_pSocketMgr->at(i+1)->m_bChecked)
		{
			connnode->SetAttribute("status",false,"����");
		}
		else
		{
			connnode->SetAttribute("status",false,"��������");
		}
	}
	//DREB��Ϣ
// 	<DREB��Ϣ>
//     <DREBLIST name="ֱ�����ӵ�dreb�б�">
// 		  <DREB index="1" id="" pid=""  />
// 		  <DREB index="1" id="" pid=""  />
// 		  <DREB index="1" id="" pid=""  />
// 		  <DREB index="1" id="" pid=""  />
//     </DREBLIST>
//     <SVRLIST name="ע��ķ����б�">
// 		  <SVR index="1" id="" pid="" status="" />
// 		  <SVR index="1" id="" pid="" status="" />
//     </SVRLIST>
//  </DREB��Ϣ>
	//dreb�����б�
	pXml.AddNodeByPath("Monitor/DREB��Ϣ",false,"DREBLIST",false,"");
	for (i=1 ; i< m_pRes->g_nMaxConnCount ; i++)
	{
		if (m_pSocketMgr->at(i)->m_nType == SOCK_DREB)
		{
			CXmlNode *ddnode = pXml.AddNodeByPath("Monitor/DREB��Ϣ/DREBLIST",false,"SOCK_DREB",false,"");
			if (ddnode == NULL)
			{
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"AddNodeByPath ����");
				continue;
			}
			ddnode->SetAttribute("index",false,i);
			ddnode->SetAttribute("id",false,(int)m_pSocketMgr->at(i)->m_s_conn.nNodeId);
			ddnode->SetAttribute("pid",false,(int)m_pSocketMgr->at(i)->m_s_conn.cNodePrivateId);
		}
	}
	//dreb�����б�
	pXml.AddNodeByPath("Monitor/DREB��Ϣ",false,"SVRLIST",false,"");
	std::vector<S_SVR_ROUTE>svrlist;
	m_pMemDb->GetAllSvr(svrlist);
	for (i=0 ; i<svrlist.size() ; i++)
	{
		CXmlNode *svrnode = pXml.AddNodeByPath("Monitor/DREB��Ϣ/SVRLIST",false,"SVR",false,"");
		if (svrnode == NULL)
		{
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"AddNodeByPath ����");
			continue;
		}
		svrnode->SetAttribute("index",false,(int)svrlist[i].nIndex);
		svrnode->SetAttribute("id",false,(int)svrlist[i].nSvrMainId);
		svrnode->SetAttribute("pid",false,(int)svrlist[i].cSvrPrivateId);
		if (svrlist[i].bIsClose)
		{
			svrnode->SetAttribute("status",false,"�Ͽ�",false);
		}
		else
		{
			svrnode->SetAttribute("status",false,"����",false);
		}
	}
	int len = DREBDATASIZE;
	if (!pXml.ToBuffer(msg->message.buffer,len))
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"ToBuffer����");
		m_pMemPool->PoolFree(msg);
		return;
	}
	msg->message.head.nLen = len;

	m_log->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"���֪ͨ",msg->message.buffer,msg->message.head.nLen);

	m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���֪ͨ �ڴ����[%d] δʹ��[%d]",total,used);
	msg->message.head.cRaflag = 0;
	msg->message.head.cDrebAffirm = 0;
	msg->message.head.cZip = 0;
	msg->message.head.cNextFlag = 0;

	msg->message.head.cCmd = CMD_DPPUSH;
	msg->message.head.d_Dinfo.d_nNodeId = m_pRes->g_nMonitorDrebId;
	msg->message.head.d_Dinfo.d_cNodePrivateId = m_pRes->g_nMonitorDrebPrivateId;
	msg->message.head.d_Dinfo.d_nSvrMainId = m_pRes->g_nMonitorSvrId;
	msg->message.head.d_Dinfo.d_cSvrPrivateId = m_pRes->g_nMonitorSvrPrivateId;
	msg->message.head.d_Dinfo.d_nServiceNo = m_pRes->g_nMonitorTxCode;
	TransMsgRequest(msg,false);

}

void CMsgProcThread::OnCmdUnReqTx(S_DREB_RSMSG *msg)
{
	
	bool baffrim=false; //�Ƿ��ɱ��ڷ���Ӧ��
	
	S_DREB_RSMSG *data=NULL;
	if (msg->message.head.d_Dinfo.d_nNodeId<1)
	{
		//���͸����е�ͨѶƽ̨
		std::vector<S_DREB_ROUTE *>sendtlist;
		if (!m_pMemDb->GetAllRouteBc(sendtlist))
		{
			m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"OnCmdUnReqTx ȡ����Ҫ���㲥��DREB�ڵ���Ϣ����");
			m_pMemPool->PoolFree(msg);
			return;
		}
		for (unsigned int i=0;i<sendtlist.size();i++)
		{
			data = (S_DREB_RSMSG *) m_pMemPool->PoolMalloc();
			if (data == NULL)
			{
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ڴ治�㣬�����������߽ڵ����ݿռ����");
				m_pMemPool->PoolFree(msg);
				return;
			}
			memcpy(data,msg,sizeof(S_DREB_RSMSG));
			data->message.head.d_Dinfo.d_nNodeId = sendtlist[i]->nNodeId;
			data->message.head.d_Dinfo.d_cNodePrivateId = sendtlist[i]->cNodePrivateId;
			if (m_pSocketMgr->at(sendtlist[i]->nIndex)->SendMsg(data)!=0)
			{
				m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"OnCmdUnReqTx ����ʧ��");
				m_pMemPool->PoolFree(msg);
				return;
			}
		}
	}
	if (msg->message.head.d_Dinfo.d_nNodeId == m_pRes->g_nDrebId || msg->message.head.d_Dinfo.d_nNodeId <1)  //�ڵ���ͬ
	{
		S_SERVICE_ROUTE data;
		//ȡ������Ľ���ע��
		data.nNodeId = m_pRes->g_nDrebId;
		data.nSvrMainId = msg->message.head.d_Dinfo.d_nSvrMainId;
		data.nFuncNo = msg->message.head.d_Dinfo.d_nServiceNo;
		data.cSvrPrivateId = msg->message.head.d_Dinfo.d_cSvrPrivateId;
		data.cNodePrivateId = m_pRes->g_nDrebPrivateId;
		data.nStep = 0;
		data.nIndex = 0;
		S_SVR_ROUTE svr;
		if (!m_pMemDb->SelectSvr(data.nSvrMainId,data.cSvrPrivateId,svr))
		{
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"ȡ������ע��ʱ�ķ���[%d %d]�ڷ�����в�����",data.nSvrMainId,data.cSvrPrivateId);
			m_pMemPool->PoolFree(msg);
			return ;
		}
		m_log->LogMp(LOG_PROMPT,__FILE__,__LINE__,"ȡ������[%d %d] �Ľ���ע��",data.nSvrMainId,data.cSvrPrivateId);
		//ȡ��ע��
		m_pMemDb->UnRegisterService(data.nNodeId,data.cNodePrivateId,data.nSvrMainId,data.cSvrPrivateId);
		
		if (!svr.bIsClose)
		{
	    //20130304 songfree add ��������ʱ����������뵽����·�ɱ���������·�ɣ�������Ĭ��Ϊ0
			S_SERVICE_ROUTE rt;
			bzero(&rt,sizeof(S_SERVICE_ROUTE));
			rt.cNodePrivateId = m_pRes->g_nDrebPrivateId;
			rt.cSvrPrivateId = data.cSvrPrivateId;
			rt.nFuncNo = 0;
			rt.nIndex = svr.nIndex;
			rt.nNodeId = m_pRes->g_nDrebId;
			rt.nStep = 0;
			rt.nSvrMainId = data.nSvrMainId;
			rt.nUpdateTime = time(NULL);
			m_pMemDb->AddServiceRoute(rt);
		//ADD END
		}

	}
	m_pMemPool->PoolFree(msg);	
	return ;
}

void CMsgProcThread::RouteMsgRequest(S_DREB_RSMSG *msg,bool isaffirm)
{
	if (msg->message.head.d_Dinfo.d_nNodeId>0)
	{
		 RouteMsgRequestByDreb(msg,isaffirm);
		 return;
	}
	else if (msg->message.head.d_Dinfo.d_nSvrMainId > 0) 
	{
		 RouteMsgRequestBySvr(msg,isaffirm);
		 return;
	}
	else
	{
		RouteMsgRequestByFunc(msg,isaffirm);
		return;
	}
}

void CMsgProcThread::RouteMsgRequestByDreb(S_DREB_RSMSG *msg,bool isaffirm)
{
	bool bRet;
	//�����ж��Ƿ����߽ڵ�
	if (msg->message.head.d_Dinfo.d_nNodeId == m_pRes->g_nDrebId)
	{
		//�����ڵ���ͬ
		if (0 == msg->message.head.d_Dinfo.d_cNodePrivateId || msg->message.head.d_Dinfo.d_cNodePrivateId == m_pRes->g_nDrebPrivateId)
		{
			S_SERVICE_ROUTE rt;
			//�����ڵ���ͬ��˽�нڵ�Ϊ0����Ǳ��ڵ�
			if (msg->message.head.d_Dinfo.d_nSvrMainId >0)  //���ұ��صķ���
			{
				//�������ӵķ���
				if (0 == msg->message.head.d_Dinfo.d_cSvrPrivateId )
				{
					if (msg->message.head.d_Dinfo.d_nServiceNo>0)
					{
						//20181019 ������+������+�������	
						bRet = m_pMemDb->SelectRouteByDrebFuncSvr(msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.d_Dinfo.d_nSvrMainId,rt);
					}
					else
					{
						bRet = m_pMemDb->SelectLocalBySvr(msg->message.head.d_Dinfo.d_nSvrMainId,rt);
					}
					
				}
				else
				{
					bRet = m_pMemDb->SelectLocalBySvr(msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,rt);
				}
				if (!bRet)
				{
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"·�ɴ��� �������߽ڵ�-����·�ɣ��Ҳ������� [%d %d %d %d] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]",\
						msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
						msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
						GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
						msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
						msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
						msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		                msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
					AnsMsg(msg,ERR_SVRNOTREGISTER,"�������߽ڵ�-����·�ɣ��Ҳ�������");
					return ;
				}
			}
			else  //���ݽ�����������
			{
				
				if (!m_pMemDb->SelectLocalByFunc(msg->message.head.d_Dinfo.d_nServiceNo,rt))
				{
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"·�ɴ��� �������߽ڵ�·�ɣ��Ҳ��������� [%d %d %d %d] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]",\
						msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
						msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
						GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
						msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
						msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
						msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		                msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
					AnsMsg(msg,ERR_FUNCNOTFUND,"���߽ڵ�·���޽�����");
					return ;
				}

			}
			
			S_DREB_RSMSG *data=NULL;
			if (isaffirm) //Ҫȷ��Ӧ��
			{
				data = (S_DREB_RSMSG *)m_pMemPool->PoolMalloc();
				if (data == NULL)
				{
					m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"�ڴ�������");
					m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ڴ�������[%d %d %d %d] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]",\
						msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
						msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
						GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
						msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
						msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
						msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		                msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
					AnsMsg(msg,ERR_SYSTEM,"�ڴ�������");
					return;
				}
				
				memcpy(&(data->msghead),&(msg->msghead),sizeof(DREBQUEUEMSG_HEAD));
				memcpy(&(data->message),&(msg->message),sizeof(COMMSTRU));
				//����ΪҪȷ��Ӧ���ȸ���һ��
			}
			//����ָ������
			//ת��������
			if (m_log->isWrite(LOG_DEBUG))
			{
				m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�����ݷ������ط����� [%d %s] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]",\
					rt.nIndex,GetIndexFlag(rt.nIndex).c_str(),\
					GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
					msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
					msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
					msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		        msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
			}
			if (m_pSocketMgr->at(rt.nIndex) != NULL)
			{
				m_pSocketMgr->at(rt.nIndex)->SendMsg(msg);
			}
			else
			{
				m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"���ط���[%d %d]�ѶϿ�",rt.nSvrMainId,rt.cSvrPrivateId);
			}
			if (isaffirm)
			{
				AnsMsg(data,SUCCESS,"��ת��������");
			}
			return ;
		}
		//�����ڵ���ͬ��˽�нڵ㲻��ͬ������ͨ�������·�ɷ�ʽ
	}
	
	S_DREB_ROUTE  dreb;
	//���Ǳ��ڵ㣬Ҫͨ��·��ת��
	if (0 == msg->message.head.d_Dinfo.d_cNodePrivateId)
	{
		bRet =m_pMemDb->SelectRoute(msg->message.head.d_Dinfo.d_nNodeId,dreb);
	}
	else
	{
		bRet =m_pMemDb->SelectRoute(msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,dreb);
	}
	if (!bRet)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"·�ɴ��� �Ҳ������߽ڵ�[%d %d]��·��",\
			msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId);
		AnsMsg(msg,ERR_DREBROUTE,"·�ɴ���");
		return;
	}
	if (0 == dreb.nStep)
	{
		//��ô�����أ����Ǳ��ڵ�Ӧ��������Ŷ�
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"·�ɴ��� [%d %d %d %d] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]",\
			msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
			msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
			GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
			msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
			msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
			msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		        msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"·�ɴ��� ���߽ڵ�·�ɳ��� ��ǰ�ڵ�[%d %d] ·��Ŀ��ڵ�[%d %d] nStep[%d]",\
			m_pRes->g_nDrebId,m_pRes->g_nDrebPrivateId,dreb.nNodeId,dreb.cNodePrivateId,dreb.nStep);
		AnsMsg(msg,ERR_DREBROUTE,"·�ɴ���");
		return ;
	}
	//�ô��ֶ���Ϊ·��·������������16����
	msg->message.head.a_Ainfo.a_nNodeId++;
	if (dreb.nNodeId == msg->message.head.s_Sinfo.s_nNodeId && dreb.cNodePrivateId == msg->message.head.s_Sinfo.s_cNodePrivateId)	
	{
		//���ؽ�����������ߣ��������ѭ����ֱ�Ӷ���
		m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"����δע��");
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"����δע��[%d %d %d %d] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]",\
			msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
			msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
			GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
			msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
			msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
			msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
			msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
		AnsMsg(msg,ERR_FUNCNOTFUND,"����δע��");
		return;
	}
	//songfree 20210126 �޸���󲽽�Ϊ����
	if (msg->message.head.a_Ainfo.a_nNodeId >=m_pRes->g_nMaxRouteStep)
	{
		//���ؽ�����������ߣ��������ѭ����ֱ�Ӷ���
		m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"·��ѭ���������߿��ܶϿ�");
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"·��ѭ���������߿��ܶϿ�[%d %d %d %d] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]",\
			msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
			msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
			GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
			msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
			msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
			msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
			msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
		AnsMsg(msg,ERR_FUNCNOTFUND,"·��ѭ���������߿��ܶϿ�");
		return;
	}
	if (m_log->isWrite(LOG_DEBUG))
	{
		m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�����ݷ���·�ɴ��� [%d %s] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]",\
			dreb.nIndex,GetIndexFlag(dreb.nIndex).c_str(),\
			GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
			msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
			msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
			msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
			msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
	}
	
	if (!dreb.bIsClose)
	{
		m_pSocketMgr->at(dreb.nIndex)->SendMsg(msg);
	}
	else 
	{
		if (m_log->isWrite(LOG_WARNNING))
		{
			m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"·�������ѶϿ����ݻ�����[%d %d %d %d] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]",\
				msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
				msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
				GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
				msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
				msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
				msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		    msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
		}
		
		if (dreb.pSendQueue->PutSendMsg(msg) !=0 )
		{
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"�뷢�Ͷ���ʧ��");
		}
	}
	return ;
}

void CMsgProcThread::RouteMsgRequestBySvr(S_DREB_RSMSG *msg,bool isaffirm)
{
	bool bRet;
	S_DREB_ROUTE rt;
	if (0 == msg->message.head.d_Dinfo.d_cSvrPrivateId)
	{
		if (msg->message.head.d_Dinfo.d_nServiceNo>0)
		{
			//20181019���Ӱ�������ͷ����·��
			//������û��ע�ύ����Ĳ��ᷢ�����ڷ�������
			bRet = m_pMemDb->SelectRouteByFuncSvr(msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.d_Dinfo.d_nSvrMainId,rt);
			if (!bRet) // 20190311û���ҵ���������Ų�
			{
				bRet = m_pMemDb->SelectRouteBySvr(msg->message.head.d_Dinfo.d_nSvrMainId,rt);
			}
		}
		else //û������룬��������
		{
			bRet = m_pMemDb->SelectRouteBySvr(msg->message.head.d_Dinfo.d_nSvrMainId,rt);
		}
	}
	else
	{
		bRet = m_pMemDb->SelectRouteBySvr(msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,rt);
	}
	if (!bRet)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ݷ���·�� �Ҳ������� [%d %d %d %d] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]",\
			msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
			msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
			GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
			msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
			msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
			msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		    msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
			AnsMsg(msg,ERR_SVRNOTREGISTER,"����δע��");
		return;
	}
	//�ô��ֶ���Ϊ·��·������������16����
	msg->message.head.a_Ainfo.a_nNodeId++;
	//songfree 20210126 �޸���󲽽�Ϊ����
	if (msg->message.head.a_Ainfo.a_nNodeId >=m_pRes->g_nMaxRouteStep)
	{
		//���ؽ�����������ߣ��������ѭ����ֱ�Ӷ���
		m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"·��ѭ�����󣬷�����ܶϿ�");
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"·��ѭ�����󣬷�����ܶϿ�[%d %d %d %d] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]",\
			msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
			msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
			GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
			msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
			msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
			msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
			msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
		AnsMsg(msg,ERR_FUNCNOTFUND,"·��ѭ�����󣬷�����ܶϿ�");
		return;
	}

	S_DREB_RSMSG *data=NULL;
	if (isaffirm) //Ҫȷ��Ӧ��
	{
		data = (S_DREB_RSMSG *)m_pMemPool->PoolMalloc();
		if (data == NULL)
		{
			m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"�ڴ�������");
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ڴ�������[%d %d %d %d] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]",\
				msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
				msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
				GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
				msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
				msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
				msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		        msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
			AnsMsg(msg,ERR_SYSTEM,"�ڴ�������");
			return;
		}
		
		memcpy(&(data->msghead),&(msg->msghead),sizeof(DREBQUEUEMSG_HEAD));
		memcpy(&(data->message),&(msg->message),sizeof(COMMSTRU));
		//����ΪҪȷ��Ӧ���ȸ���һ��
	}
	//ת��������
	if (m_log->isWrite(LOG_DEBUG))
	{
		m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�����ݷ��������� [%d %s] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]",\
			rt.nIndex,GetIndexFlag(rt.nIndex).c_str(),\
			GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
			msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
			msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
			msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
	}
	if (m_pSocketMgr->at(rt.nIndex) != NULL)
	{
		m_pSocketMgr->at(rt.nIndex)->SendMsg(msg);
	}
	else
	{
		m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"�����ѶϿ�");
	}
	if (isaffirm)
	{
		AnsMsg(data,SUCCESS,"��ת��������");
	}
	return ;
}

void CMsgProcThread::RouteMsgRequestByFunc(S_DREB_RSMSG *msg,bool isaffirm)
{
	bool bRet;
	S_DREB_ROUTE rt;
//	m_pRes->PutTime();
	bRet = m_pMemDb->SelectRouteByFunc(msg->message.head.d_Dinfo.d_nServiceNo,rt);
	if (!bRet)
	{
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ݽ���·�� �Ҳ����˽��� [%d %d %d %d] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]",\
			msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
			msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
			GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
			msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
			msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
			msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		        msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
		AnsMsg(msg,ERR_FUNCNOTFUND,"������δע��");
		return;
	}
	//�ô��ֶ���Ϊ·��·������������16����
	msg->message.head.a_Ainfo.a_nNodeId++;

	if (rt.nStep >0) //˵�����Ǳ���
	{
		if (rt.nNodeId == msg->message.head.s_Sinfo.s_nNodeId && rt.cNodePrivateId == msg->message.head.s_Sinfo.s_cNodePrivateId)	
		{
			//���ؽ�����������ߣ��������ѭ����ֱ�Ӷ���
			m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"����δע��");
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"����δע��[%d %d %d %d] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]",\
				msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
				msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
				GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
				msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
				msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
				msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
				msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
			AnsMsg(msg,ERR_FUNCNOTFUND,"����δע��");
			return;
		}
		//songfree 20210126 �޸���󲽽�Ϊ����
		if (msg->message.head.a_Ainfo.a_nNodeId >=m_pRes->g_nMaxRouteStep)
		{
			//���ؽ�����������ߣ��������ѭ����ֱ�Ӷ���
			m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"·��ѭ�����󣬽���δע��");
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"·��ѭ�����󣬽���δע��[%d %d %d %d] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]",\
				msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
				msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
				GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
				msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
				msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
				msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
				msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
			AnsMsg(msg,ERR_FUNCNOTFUND,"·��ѭ�����󣬽���δע��");
			return;
		}
	}
//	m_pRes->PutTime();
	S_DREB_RSMSG *data=NULL;
	if (isaffirm) //Ҫȷ��Ӧ��
	{
		data = (S_DREB_RSMSG *)m_pMemPool->PoolMalloc();
		if (data == NULL)
		{
			m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"�ڴ�������");
			m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ڴ�������[%d %d %d %d] DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]",\
				msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
				msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,\
				GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
				msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
				msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
				msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		        msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
			AnsMsg(msg,ERR_SYSTEM,"�ڴ�������");
			return;
		}
		
		memcpy(&(data->msghead),&(msg->msghead),sizeof(DREBQUEUEMSG_HEAD));
		memcpy(&(data->message),&(msg->message),sizeof(COMMSTRU));
		//����ΪҪȷ��Ӧ���ȸ���һ��
	}
	//ת�������������·��
	if (m_log->isWrite(LOG_DEBUG))
	{
		m_log->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�����ݷ���[%d %s] ���� DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Ŀ��[%d %d %d %d] ҵ�����ݳ���[%d]",\
			rt.nIndex,GetIndexFlag(rt.nIndex).c_str(),\
			GetDrebCmdType(msg->message.head.cCmd).c_str(),msg->message.head.cNextFlag,\
			msg->message.head.cRaflag,msg->message.head.d_Dinfo.d_nServiceNo,msg->message.head.s_Sinfo.s_nNodeId,\
			msg->message.head.s_Sinfo.s_cNodePrivateId,msg->message.head.s_Sinfo.s_nDrebSerial,\
			msg->message.head.d_Dinfo.d_nNodeId,msg->message.head.d_Dinfo.d_cNodePrivateId,\
		        msg->message.head.d_Dinfo.d_nSvrMainId,msg->message.head.d_Dinfo.d_cSvrPrivateId,msg->message.head.nLen);
	}
	if (m_pSocketMgr->at(rt.nIndex) != NULL)
	{
		m_pSocketMgr->at(rt.nIndex)->SendMsg(msg);
	}
	else
	{
		m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"�����ѶϿ�");
	}
	
//	m_pRes->PutTime();
	if (isaffirm)
	{
		AnsMsg(data,SUCCESS,"��ת��");
	}
	return ;
}

void CMsgProcThread::PrintRoute()
{
	if (m_pRes->g_nPrintRoute != 1)
	{
		return;
	}
	unsigned int i;
	std::string routemsg;
	char tmpchar[400];
	std::vector<S_DREB_ROUTE *> dp;
	sprintf(tmpchar,"\n  DREB LOCAL INFO\n");
	routemsg = routemsg+tmpchar;
	sprintf(tmpchar,"    nIndex=%d nStep=%d nNodeId=%d cNodePrivateId=%d nbandwidth=%d\n",\
		0,0,m_pRes->g_nDrebId,m_pRes->g_nDrebPrivateId,1024000);
	routemsg = routemsg+tmpchar;
	m_pMemDb->GetAllDreb(dp);
	sprintf(tmpchar,"  DREB CONNECT LIST\n");
	routemsg = routemsg+tmpchar;
	for (i=0;i<dp.size();i++)
	{
		sprintf(tmpchar,"    nIndex=%d nStep=%d nNodeId=%d cNodePrivateId=%d bIsClose=%d nbandwidth=%d\n",\
			dp[i]->nIndex,dp[i]->nStep,dp[i]->nNodeId,dp[i]->cNodePrivateId,dp[i]->bIsClose,dp[i]->nbandwidth);
		routemsg = routemsg+tmpchar;
	}
	std::vector<S_SVR_ROUTE>svrlist;
	m_pMemDb->GetAllSvr(svrlist);
	sprintf(tmpchar,"  DREB LOCAL SERVICE LIST\n");
	routemsg = routemsg+tmpchar;
	for (i=0;i<svrlist.size();i++)
	{
		sprintf(tmpchar,"    nSvrMainId=%d cSvrPrivateId=%d bIsClose=%d\n",\
			svrlist[i].nSvrMainId,svrlist[i].cSvrPrivateId,svrlist[i].bIsClose);
		routemsg = routemsg+tmpchar;
	}
	
	std::vector<S_DREB_ROUTE *> rt;
	m_pMemDb->GetRouteList(rt);
	sprintf(tmpchar,"  DREB ROUTE LIST\n");
	routemsg = routemsg+tmpchar;
	for (i=0;i<rt.size();i++)
	{
		sprintf(tmpchar,"    nIndex=%d nStep=%d nNodeId=%d cNodePrivateId=%d nbandwidth=%d\n",\
			rt[i]->nIndex,rt[i]->nStep,rt[i]->nNodeId,rt[i]->cNodePrivateId,rt[i]->nbandwidth);
		routemsg = routemsg+tmpchar;
	}
	m_pRes->g_pRouteLog.LogBin(LOG_INFO,__FILE__,__LINE__,"�ڵ���񼰽ڵ�·����Ϣ",\
		(char *)routemsg.c_str(),routemsg.length());
}

void CMsgProcThread::PrintServiceRoute()
{
	if (m_pRes->g_nPrintRoute != 1)
	{
		return;
	}
	unsigned int i;
	std::string routemsg;
	char tmpchar[DREBBUFSIZE];

	std::vector<S_SERVICE_ROUTE *>servicelist;
	m_pMemDb->GetAllServiceOrder(servicelist);
	sprintf(tmpchar,"\n  DREB SERVICE LIST,  TOTAL SERVICE:%d \n",servicelist.size());
	routemsg = routemsg+tmpchar;
	int index;
	int nodeid;
	int nodeprivateid;
	int svrid;
	int svrprivateid;
	
	for (i=0;i<servicelist.size();i++)
	{
		if (i == 0)
		{
			index = servicelist[i]->nIndex;
			nodeid = servicelist[i]->nNodeId;
			nodeprivateid = servicelist[i]->cNodePrivateId;
			svrid = servicelist[i]->nSvrMainId;
			svrprivateid = servicelist[i]->cSvrPrivateId;
			sprintf(tmpchar,"    INDEX=%d DREB[%d %d] SVR[%d %d] STEP=%d FUNC=",index,nodeid,nodeprivateid,svrid,svrprivateid,servicelist[i]->nStep);
			routemsg= routemsg+tmpchar;
		}
		if (index != servicelist[i]->nIndex || nodeid != servicelist[i]->nNodeId  || nodeprivateid != servicelist[i]->cNodePrivateId \
			|| svrid != servicelist[i]->nSvrMainId || svrprivateid != servicelist[i]->cSvrPrivateId)
		{
			index = servicelist[i]->nIndex;
			nodeid = servicelist[i]->nNodeId;
			nodeprivateid = servicelist[i]->cNodePrivateId;
			svrid = servicelist[i]->nSvrMainId;
			svrprivateid = servicelist[i]->cSvrPrivateId;
			sprintf(tmpchar,"\n    INDEX=%d DREB[%d %d] SVR[%d %d] STEP=%d FUNC=",index,nodeid,nodeprivateid,svrid,svrprivateid,servicelist[i]->nStep);
			routemsg= routemsg+tmpchar;
		}
		//sprintf(tmpchar," %d-i%d",servicelist[i]->nFuncNo,servicelist[i]->nIndex);
		sprintf(tmpchar," %d",servicelist[i]->nFuncNo);
		routemsg= routemsg+tmpchar;
	}
	routemsg= routemsg+"\n";
	m_pRes->g_pRouteLog.LogBin(LOG_INFO,__FILE__,__LINE__,"����·����Ϣ",(char *)routemsg.c_str(),routemsg.length());
}


void CMsgProcThread::OnCmdSubscribe(S_DREB_RSMSG* msg)
{
    int snum = 0;
    PCOMMSTRU databuf = &(msg->message);
    S_SERVICEREG_MSG* prs = NULL;
	S_SUBSCRIBE data;

    prs = (S_SERVICEREG_MSG*)(databuf->buffer);
    m_pDrebEndian.Endian2LocalHost((unsigned char*)&(prs->nSvrMainId), sizeof(prs->nSvrMainId));
    m_pDrebEndian.Endian2LocalHost((unsigned char*)&(prs->nFuncNo), sizeof(prs->nFuncNo));
    m_pDrebEndian.Endian2LocalHost((unsigned char*)&(prs->nFuncNum), sizeof(prs->nFuncNum));

    m_log->LogMp(LOG_DEBUG, __FILE__, __LINE__, "����[%d %d]�㲥����   �㲥����[%d] len=[%d]", \
        prs->nSvrMainId, prs->cSvrPrivateId, prs->nFuncNum, msg->message.head.nLen);
    if (prs->nFuncNum < 1)
    {
        m_log->LogMp(LOG_ERROR, __FILE__, __LINE__, "����[%d %d]�㲥���� �㲥����[%d]Ϊ0��ȡ������", prs->nSvrMainId, prs->cSvrPrivateId, prs->nFuncNum);
		m_pMemDb->m_subscribe.UnSubscribe(msg->msghead.index);
		m_pMemPool->PoolFree(msg);
        return;
    }
    if (msg->message.head.nLen < sizeof(S_SERVICEREG_MSG) + (prs->nFuncNum - 1) * sizeof(unsigned int))
    {
        m_log->LogMp(LOG_ERROR, __FILE__, __LINE__, "����[%d %d]�㲥���� ���ĳ���[%d]���� ӦΪ[%d]", \
            prs->nSvrMainId, prs->cSvrPrivateId, prs->nFuncNum, msg->message.head.nLen, \
            sizeof(S_SERVICEREG_MSG) + (prs->nFuncNum - 1) * sizeof(unsigned int));
		m_pMemPool->PoolFree(msg);
        return;
    }
    data.nSvrMainId = prs->nSvrMainId;
    data.nFuncNo = prs->nFuncNo;
    data.cSvrPrivateId = prs->cSvrPrivateId;
    data.nIndex = msg->msghead.index;

	std::vector<unsigned int> subscribelist;
	subscribelist.push_back(data.nFuncNo);
    snum = prs->nFuncNum - 1;

    int nFuncNum = prs->nFuncNum;
    for (int i = 0; i < snum; i++)
    {
        memcpy(&(data.nFuncNo), databuf->buffer + sizeof(S_SERVICEREG_MSG) + i * sizeof(unsigned int), sizeof(unsigned int));
        m_pDrebEndian.Endian2LocalHost((unsigned char*)&(data.nFuncNo), sizeof(unsigned int));
		subscribelist.push_back(data.nFuncNo);
    }

    m_log->LogMp(LOG_DEBUG + 2, __FILE__, __LINE__, "����%d-%d��%d���㲥����", data.nSvrMainId, data.cSvrPrivateId, nFuncNum);
	m_pMemDb->m_subscribe.Subscribe(data.nIndex,subscribelist, data.nSvrMainId, data.cSvrPrivateId);
	m_pMemPool->PoolFree(msg);
    return;
}
void CMsgProcThread::OnCmdUnSubscribe(S_DREB_RSMSG* msg)
{
    m_log->LogMp(LOG_ERROR, __FILE__, __LINE__, "index[%d] ȡ���㲥����", msg->msghead.index);
    m_pMemDb->m_subscribe.UnSubscribe(msg->msghead.index);
    m_pMemPool->PoolFree(msg);
    return;
}

void CMsgProcThread::TransDestBc(S_DREB_RSMSG* msg)
{
	//m_log->LogMp(LOG_DEBUG, __FILE__, __LINE__, "TransDestBc ��ʼ m_vBcDest size[%d] dest[%d %d %d %d] local[%d %d]", m_pRes->m_vBcDest.size(),\
	//	msg->message.head.d_Dinfo.d_nNodeId, msg->message.head.d_Dinfo.d_cNodePrivateId, msg->message.head.d_Dinfo.d_nSvrMainId, msg->message.head.d_Dinfo.d_cSvrPrivateId, \
	//	m_pRes->g_nDrebId,m_pRes->g_nDrebPrivateId);
	if (m_pRes->m_vBcDest.size()>0 && \
		msg->message.head.d_Dinfo.d_nNodeId == m_pRes->g_nDrebId && \
		msg->message.head.d_Dinfo.d_cNodePrivateId == m_pRes->g_nDrebPrivateId && \
		msg->message.head.d_Dinfo.d_nSvrMainId == 0 && \
		msg->message.head.d_Dinfo.d_cSvrPrivateId == 0)
	{
		S_DREB_RSMSG* data = NULL;
		S_DREB_ROUTE dreb;
		//Ϊ�������ڵ�Ĺ㲥���鿴�Ƿ������ã�������ת����ȥ
		for (unsigned int i = 0; i < m_pRes->m_vBcDest.size(); i++)
		{
			if (m_pMemDb->SelectRoute(m_pRes->m_vBcDest[i].nNodeId, m_pRes->m_vBcDest[i].cNodePrivateId, dreb))
			{
                data = (S_DREB_RSMSG*)m_pMemPool->PoolMalloc();
                if (data == NULL)
                {
                    m_log->LogMp(LOG_ERROR_FAULT, __FILE__, __LINE__, "TransDestBc �ڴ�������");
                    return;
                }
                memcpy(&(data->msghead), &(msg->msghead), sizeof(DREBQUEUEMSG_HEAD));
                memcpy(&(data->message), &(msg->message), sizeof(COMMSTRU)); 
                data->message.head.d_Dinfo.d_nNodeId = m_pRes->m_vBcDest[i].nNodeId;
                data->message.head.d_Dinfo.d_cNodePrivateId = m_pRes->m_vBcDest[i].cNodePrivateId;
                if (m_pSocketMgr->at(dreb.nIndex)->SendMsg(data) != 0)
                {
                    m_log->LogMp(LOG_ERROR, __FILE__, __LINE__, "TransDestBc ����ʧ��");
                }
				//else
				//{
				//	m_log->LogMp(LOG_DEBUG, __FILE__, __LINE__, "�㲥ת�����߽ڵ�[%d %d]���", m_pRes->m_vBcDest[i].nNodeId, m_pRes->m_vBcDest[i].cNodePrivateId);
				//}
			}
			//else
			//{
			//	m_log->LogMp(LOG_WARNNING, __FILE__, __LINE__, "�޹㲥ת�������߽ڵ�[%d %d]·��", m_pRes->m_vBcDest[i].nNodeId, m_pRes->m_vBcDest[i].cNodePrivateId);
			//}
		}
	}
	return;
}