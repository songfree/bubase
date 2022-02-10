// PuThread.cpp: implementation of the CPuThread class.
//
//////////////////////////////////////////////////////////////////////

#include "PuThread.h"
#include "dreberrcode.h"
#include "Bpc_Timer.h"

extern CBpc_Timer  g_pBpcTime;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern char g_VERSION[20];
CPuThread::CPuThread()
{
	m_bIsRunning = false;
	m_pRes = NULL;
	m_pSockMgr = NULL;
	m_pMemPool = NULL;
	m_pPoolData = NULL;
	m_pFuncTbl = NULL;
	m_bIsInit = false;
	m_nBegin=0;
	m_nEnd=1;
	m_sThreadName= "CPuThread";

	m_pLog = NULL;
	m_bIsPingTimer = false;
}

CPuThread::~CPuThread()
{
	m_pRes = NULL;
	m_pSockMgr = NULL;
	m_pMemPool = NULL;
	m_pPoolData = NULL;
	m_pFuncTbl = NULL;
	m_pLog = NULL;
}
std::string CPuThread::GetBpcMsgType(int msgtype)
{
	switch (msgtype)
	{
	case MSG_REQ:        // 0      //��������,bpc����BPU������
		return "MSG_REQ";
	case MSG_FREE:        //1      //BPU���Ϳ�������
		return "MSG_FREE";
	case MSG_GETSERIAL:   //4      //BPUȡ����Ψһ��ˮ
		return "MSG_GETSERIAL";
	case MSG_SETGPARA1:  //5      //BPU����SETGPARA1
		return "MSG_SETGPARA1";
	case MSG_SETGPARA2:   //6      //BPU����SETGPARA2
		return "MSG_SETGPARA2";
	case MSG_SETGPARA3:   //7      //BPU����SETGPARA3
		return "MSG_SETGPARA3";	
	case MSG_EXTCALL:      //10      //BPU�����������	
		return "MSG_EXTCALL";
	case MSG_BPCCALL:      //11      //BPU����PU���������
		return "MSG_BPCCALL";
	case MSG_BPUREG:       //12      //BPU����ע������
		return "MSG_BPUREG";
	case MSG_BPUISREG:     //13      //BPU�����Ƿ�ע������
		return "MSG_BPUISREG";
	case MSG_GETNEXT:      //15      //dreb������ȡ����������Ϣ
		return "MSG_GETNEXT";
	case MSG_TRANS:      //9      //����ת��
		return "MSG_GETNEXT";
	default:
		return "";
	}
}
std::string CPuThread::GetDrebCmdType(int cmdtype)
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


bool CPuThread::SetGlobalVar(CGResource *res,CPoolDataMsg *pooldata,CBF_BufferPool *mempool,CSocketMgr *sockmgr,CFuncTbl *tbl)
{
	m_pRes = res;
	m_pSockMgr = sockmgr;
	m_pMemPool = mempool;
	m_pPoolData = pooldata;
	m_pFuncTbl = tbl;
	m_pLog = &(m_pRes->m_log);
	m_bIsInit = true;

	m_pTimer.Init(100,true);
	//�ڵ�·�ɶ�ʱ��
	m_pTimer.SetTimer(0,5000,&CPuThread::OnTimer,this); //���ö�ʱ�� 
	return true;
}

void CPuThread::ExitThreadInstance()
{
	return ;
}
bool CPuThread::InitThreadInstance()
{
	return true;
}

int CPuThread::Run()
{
	struct timeval tv;
	int result;
	int maxfd=0;
	int ret;
	int lasttime=time(NULL);
	int lasttime2=time(NULL);
	m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"%s SOCKET index:%d-%d��Χ",m_sThreadName.c_str(),m_nBegin,m_nEnd);
	while(!m_pRes->g_bToExit)
	{
		FD_ZERO(&m_rset);
		FD_ZERO(&m_wset);
#if defined(_WINDOWS)
		FD_ZERO(&m_eset);
#endif
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		maxfd=0;
		//ȡ������socket����socket���뵽fd_set����
		ret = GetMaxfdAddSelect(maxfd);
		if (maxfd  <=0 || ret<1)
		{
			SLEEP(10);
			continue;
		}
#if defined(_WINDOWS)
		result = select(maxfd+1, &m_rset, &m_wset, &m_eset, &tv);
#else
		result = select(maxfd+1, &m_rset, &m_wset, NULL, &tv);
#endif
		
		if (result != 0) //�¼�����,����
		{
			OnEvent();
			
		}
		if (m_bIsPingTimer)
		{
			OnNoEvent();
			m_bIsPingTimer= false;
		}

	}
	m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ֹͣBPU���߳�");
	return 0;
}
int CPuThread::GetMaxfdAddSelect(int &maxfd)
{
	int ret=0;
	maxfd = 0;
	for (int i=m_nBegin;i<m_nEnd;i++)
	{
		if (!m_pSockMgr->at(i)->IsCanSelect())
		{
			continue;
		}
		else
		{
			//��Ҫ������
			if (m_pSockMgr->at(i)->NeedRead())
			{
				FD_SET(m_pSockMgr->at(i)->m_sock, &m_rset);//���뵽��select����
			}
			//��Ҫ�������ݻ���������ʱ
			if (m_pSockMgr->at(i)->NeedSend())
			{
				FD_SET(m_pSockMgr->at(i)->m_sock, &m_wset);//���뵽дselect����
			}
			//�õ�����socket 
			if (m_pSockMgr->at(i)->m_sock > maxfd)
			{
				maxfd=m_pSockMgr->at(i)->m_sock; 
			}
			ret++;
		}
	}
	return ret;
}
void CPuThread::OnNoEvent()
{
	int j;
	int tmptime;
	int curtime =  time(NULL) ;
	for (j=m_nBegin; j<m_nEnd; j++)
	{
		if (m_pSockMgr->at(j)->m_sock != INVALID_SOCKET && m_pSockMgr->at(j)->m_nType == BPCSOCK_BPU)
		{
			tmptime = curtime - m_pSockMgr->at(j)->m_nReadtime;
			if (m_pSockMgr->at(j)->IsBpuFree()) //���������ӵ�BPU�ǿ��е�
			{
				//���ǿ��У���
				if (tmptime > 15)  //15��������������˵�����ӳ�������
				{
					OnClose(j,"���ӿ��е�����15�����������Ͽ�",__FILE__,__LINE__);
				}
			}
			else  //�����ڴ������󣬷�ֹһֱ����״̬�����²����á�
			{
				if (tmptime > m_pRes->g_nDisconnectTime)
				{
					OnClose(j,"�������ڴ������󣬵�����ʱ�䳬�����õĶϿ�ʱ�䣬�Ͽ�",__FILE__,__LINE__);
				}
			}
		}
		if (m_pRes->g_bToExit)
		{
			return;
		}
	}
	
}

void CPuThread::OnEvent()
{
	int ret;
	int i;
	for (i= m_nBegin; i<m_nEnd; i++)
	{
		if (m_pSockMgr->at(i)->m_sock != INVALID_SOCKET)//δ����
		{
			//��Ӧ���¼�
			OnReadEvent(i);
			if (m_pSockMgr->at(i)->m_sock != INVALID_SOCKET)
			{
				//��Ӧд�¼�
				OnWriteEvent(i);
			}
			
		}
	}
}

void CPuThread::OnReadEvent(int conindex)
{
	int ret;
	ret = FD_ISSET(m_pSockMgr->at(conindex)->m_sock,&m_rset);
	if (ret != 0) //�ж��¼�
	{
		OnRecv(conindex);
	}
	else //�������޶��¼�
	{
		if (m_pSockMgr->at(conindex)->m_sock != INVALID_SOCKET)
		{
			//û�м��ͨ��
			if (!m_pSockMgr->at(conindex)->m_bChecked)
			{
				if (m_pSockMgr->at(conindex)->m_nType == BPCSOCK_UNKNOW)
				{
					if (time(NULL)-m_pSockMgr->at(conindex)->m_nReadtime >= m_pRes->g_nHeartRun)
					{
						OnClose(conindex,"���ӷǷ�",__FILE__,__LINE__);
					} 
				}
			}
		}// end if (m_pSockMgr->at(conindex)->m_sock != INVALID_SOCKET)
	}// end if (ret == 1) //�ж��¼�
}

void CPuThread::OnWriteEvent(int conindex)
{
	int ret;
	if (m_pSockMgr->at(conindex)->m_sock == INVALID_SOCKET)
	{
		return;
	}
#if defined(_WINDOWS)
	ret = FD_ISSET(m_pSockMgr->at(conindex)->m_sock, &m_eset);
	if (ret != 0)
	{
		OnClose(conindex,"Other connect fail",__FILE__,__LINE__);
	}
	else if (FD_ISSET(m_pSockMgr->at(conindex)->m_sock, &m_wset) !=0 )
	{
		OnSend(conindex);//��������
	}

#else
	
	int ok;
	int oklen;
	if (FD_ISSET(m_pSockMgr->at(conindex)->m_sock, &m_wset) !=0)
	{
		OnSend(conindex);//��������
	}
#endif

				
}


void CPuThread::OnClose(int conindex,const char *msg,const char *filename,int fileline)
{
	if (m_pSockMgr->at(conindex)->m_sock == INVALID_SOCKET)
	{
		return;
	}
	//��socket�ر�
	m_pSockMgr->at(conindex)->OnClose(msg,filename,fileline);
}


void CPuThread::OnSend(int conindex)
{
	m_pSockMgr->at(conindex)->OnSend();
}


void CPuThread::OnRecv(int conindex)
{
	int ret=0;
	int i=0;
	
	ret = m_pSockMgr->at(conindex)->OnRecv();	
	if (ret<=0)
	{
		OnClose(conindex,"Զ�˹رգ��������ݲ�����",__FILE__,__LINE__);
		return;
	}
	if (ret == 0)
	{
		return ;	
	}
//	m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"��BPU����%d OnRecv ������%d",conindex,ret);
	while(m_pSockMgr->at(conindex)->m_nRcvBufLen>0)
	{
		if (!ResetData(conindex,&m_pDataRcv))
		{
			return;
		}
		ret = m_pSockMgr->at(conindex)->GetRecvData(&m_pDataRcv);
		if (ret>0)
		{
			
			m_pDataRcv.index = conindex;//������յ������ݵ����ӵ�����
			if (m_pSockMgr->at(conindex)->m_nType == BPCSOCK_BPU) //����BPU������
			{
// 				if (m_pDataRcv.sMsgBuf->sDBHead.nLen>0)
// 				{
// 					m_pLog->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"ҵ������",m_pDataRcv.sMsgBuf->sBuffer,m_pDataRcv.sMsgBuf->sDBHead.nLen);
// 				}
				ProcessBpu(m_pDataRcv);
			}	
			else
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���� �մ�������");
			}

		}
		else
		{
			m_pMemPool->PoolFree(m_pDataRcv.sMsgBuf);
			m_pDataRcv.sMsgBuf = NULL;
			break;
		}
	}
// 	if (m_pSockMgr->at(conindex)->m_nRcvBufLen >0 )
// 	{	
// 		m_pLog->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"���������� δȡ��",\
// 			m_pSockMgr->at(conindex)->m_sRcvBuffer,m_pSockMgr->at(conindex)->m_nRcvBufLen);
// 	}
	return ;
}


bool CPuThread::ResetData(int index,S_BPC_RSMSG *data)
{
	data->sMsgBuf = (PBPCCOMMSTRU )m_pMemPool->PoolMalloc();
	if (data->sMsgBuf == NULL)
	{
		m_pSockMgr->at(index)->OnClose("������Ϣ�ռ����",__FILE__,__LINE__);
		return false;
	}
	data->sMsgBuf->sBpcHead.nConnectTime= m_pSockMgr->at(index)->m_nConntime;
	data->sMsgBuf->sBpcHead.nIndex = index;
	data->sMsgBuf->sBpcHead.nRtime = time(NULL);
	
	return true;
}


void CPuThread::ProcessBpu(S_BPC_RSMSG &rcvdata)
{
	switch (rcvdata.sMsgBuf->sBpcHead.cMsgType)
	{
		case MSG_FREE:   //bpu֪ͨbpc�����ǿ���,�൱������
			DispatchBuFree(rcvdata);
			break;
		case MSG_GETSERIAL://BPUȡ��ˮ
			GetSerial(rcvdata);
			break;
		case MSG_SETGPARA1://BPU�ò���1
			DispatchSetPara1(rcvdata);
			break;
		case MSG_SETGPARA2://BPU�ò���2
			DispatchSetPara2(rcvdata);
			break;
		case MSG_SETGPARA3://BPU�ò���3
			DispatchSetPara3(rcvdata);
			break;
		case MSG_EXTCALL://bpu����������������Ӧ���������msgthreadֱ�ӷ���
			DispatchExtCall(rcvdata);
			break;
		case MSG_BPCCALL: //bpu����bpc�ڵĹ��ܣ����������ﴦ��
			DispatchBpcCall(rcvdata);
			break;	
		case MSG_BPUREG://bpuע��
			//�޸�Ϊ��Ϣ�����߳�������ע��
			m_pPoolData->PushData(rcvdata);
			break;
		case MSG_BPUISREG://bpu�����Ƿ��ע��
			DispatchBpuIsReg(rcvdata);
			break;
		case MSG_REQ:   //�����յ�����Ӧ��
			DispatchReq(rcvdata);
			break;
		case MSG_TRANS: //�����յ���������
			DispatchTrans(rcvdata);
			break;
		case MSG_GETNEXT://�����յ�����Ӧ��
			DispatchGetNext(rcvdata);
			break;
		default:
			m_pLog->LogBin(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ش��󣬴�������",rcvdata.sMsgBuf->sBuffer,rcvdata.sMsgBuf->sDBHead.nLen);
			m_pMemPool->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			OnClose(rcvdata.index,"���ش��� �������ݷǷ����ر�����",__FILE__,__LINE__);
			break;
	}
	return ;
}

void CPuThread::DispatchBuFree(S_BPC_RSMSG &rcvdata)
{
	if (m_pSockMgr->AffirmIndex(rcvdata.index))
	{
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->SetBpuFreeFlag(true);
//		m_pLog->LogMp(LOG_INFO,__FILE__,__LINE__,"BPU����[%d] ",rcvdata.sMsgBuf->sBpcHead.nBpuIndex);
//		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"BPU index[%d] ״̬Ϊ����",rcvdata.sMsgBuf->sBpcHead.nBpuIndex);
	}
	if (rcvdata.sMsgBuf->sDBHead.cRaflag == 1)
	{
		//bpu��PINGӦ��
		m_pMemPool->PoolFree(rcvdata.sMsgBuf);
	 	rcvdata.sMsgBuf = NULL;
		return;
	}
	if (m_pRes->g_vBpuGroupInfo[m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->m_nBpuGroupIndex].g_bIsAnsPing)
	{
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		rcvdata.sMsgBuf->sDBHead.nLen = 0;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
		rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN ;
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"Ӧ��MSG_FREE");
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->SendMsg(rcvdata);
	}
	else
	{
		m_pMemPool->PoolFree(rcvdata.sMsgBuf);
	 	rcvdata.sMsgBuf = NULL;
	}
	
}

void CPuThread::GetSerial(S_BPC_RSMSG &rcvdata)
{
	rcvdata.sMsgBuf->sDBHead.cZip = 0;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
	sprintf(rcvdata.sMsgBuf->sBuffer,"%ld",m_pRes->GetThreadShareSerial());
	rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
	rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN + rcvdata.sMsgBuf->sDBHead.nLen;
	m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->SendMsg(rcvdata);
}

void CPuThread::DispatchSetPara1(S_BPC_RSMSG &rcvdata)
{
	rcvdata.sMsgBuf->sDBHead.cZip = 0;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
	bool bRes = m_pRes->SetGPara1();
	sprintf(rcvdata.sMsgBuf->sBuffer,"%d",bRes);
	rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
	rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN + rcvdata.sMsgBuf->sDBHead.nLen;
	m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->SendMsg(rcvdata);
}

void CPuThread::DispatchSetPara2(S_BPC_RSMSG &rcvdata)
{
	rcvdata.sMsgBuf->sDBHead.cZip = 0;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
	bool bRes = m_pRes->SetGPara2();
	sprintf(rcvdata.sMsgBuf->sBuffer,"%d",bRes);
	rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
	rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
	rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN + rcvdata.sMsgBuf->sDBHead.nLen;
	m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->SendMsg(rcvdata);
}

void CPuThread::DispatchSetPara3(S_BPC_RSMSG &rcvdata)
{
	rcvdata.sMsgBuf->sDBHead.cZip = 0;
	rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
	bool bRes = m_pRes->SetGPara3();
	sprintf(rcvdata.sMsgBuf->sBuffer,"%d",bRes);
	rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
	rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN + rcvdata.sMsgBuf->sDBHead.nLen;
	rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
	m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->SendMsg(rcvdata);
	
}

void CPuThread::DispatchExtCall(S_BPC_RSMSG &rcvdata)
{
	//songfree 20130409 ��������������Ҫ���ڵ�����һ�𣬷�������ʲô��û������
	rcvdata.nRtime = time(NULL);
	//������ȼ�Ϊ0
	m_pPoolData->PushData(rcvdata);//������У�msgthread�߳̽��д���
	
}

void CPuThread::DispatchBpcCall(S_BPC_RSMSG &rcvdata)
{
	//������BPC��Ľ���
	if (rcvdata.sMsgBuf->sDBHead.cRaflag == 0) //����
	{
		S_FUNCINFO_TBL fn;
		int flag;
		//ȡ���׵����ȼ�,Ȼ��������
		fn = m_pFuncTbl->FindFunc(rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,flag);
		if (flag == 0)
		{
			//m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�޴˹��� %d",rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo);
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���� �޴˹��� ��Ϣ[%s]  DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Դ[%d %d] cZip[%d] ҵ�����ݳ���[%d]",\
				GetBpcMsgType(rcvdata.sMsgBuf->sBpcHead.cMsgType).c_str(),\
				GetDrebCmdType(rcvdata.sMsgBuf->sDBHead.cCmd).c_str(),rcvdata.sMsgBuf->sDBHead.cNextFlag,\
				rcvdata.sMsgBuf->sDBHead.cRaflag,rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
				rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
				rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
				rcvdata.sMsgBuf->sDBHead.cZip,rcvdata.sMsgBuf->sDBHead.nLen);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_FUNCNOTFUND;
			rcvdata.sMsgBuf->sDBHead.nLen = 0;
			rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
	
			m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->SendMsg(rcvdata,0);
			return;
		}

		fn.nCallNum++;
		//b_Cinfo���b_nSerial���bpu����indexʱ���
		rcvdata.sMsgBuf->sDBHead.b_Cinfo.b_nSerial = m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->m_nConntime;
		strcpy(rcvdata.sBpuGroupName ,fn.sBpuGroupName);
		rcvdata.nRtime = time(NULL);

		//20151102 ֱ�ӷŵ�BPU��Ӧ���̶߳���
		if (m_pRes->PushData(fn.nBpuGroupIndex,rcvdata,fn.cPrio) != 0)
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"��BPU����г��� ��Ϣ[%s]  DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Դ[%d %d] cZip[%d] ҵ�����ݳ���[%d]",\
				GetBpcMsgType(rcvdata.sMsgBuf->sBpcHead.cMsgType).c_str(),\
				GetDrebCmdType(rcvdata.sMsgBuf->sDBHead.cCmd).c_str(),rcvdata.sMsgBuf->sDBHead.cNextFlag,\
				rcvdata.sMsgBuf->sDBHead.cRaflag,rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
				rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
				rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
				rcvdata.sMsgBuf->sDBHead.cZip,rcvdata.sMsgBuf->sDBHead.nLen);
			m_pMemPool->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
		}
		
		//m_pPoolData->PushData(rcvdata,fn.cPrio);

	}
	else //bpccallӦ��
	{
		//b_Cinfo���b_nSerial���bpu����indexʱ���
		if (m_pSockMgr->AffirmIndex(rcvdata.sMsgBuf->sBpcHead.nBpuIndex))
		{
			if (rcvdata.sMsgBuf->sDBHead.b_Cinfo.b_nSerial == m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->m_nConntime)
			{
				rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN + rcvdata.sMsgBuf->sDBHead.nLen;
				rcvdata.sMsgBuf->sBpcHead.nConnectTime = m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->m_nConntime;
				rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
				m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->SendMsg(rcvdata);
				return;
			}
		}
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���� BPCCALLӦ��ʱ���Ӳ���������BPU������ ��Ϣ[%s]  DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Դ[%d %d] cZip[%d] ҵ�����ݳ���[%d]",\
			GetBpcMsgType(rcvdata.sMsgBuf->sBpcHead.cMsgType).c_str(),\
			GetDrebCmdType(rcvdata.sMsgBuf->sDBHead.cCmd).c_str(),rcvdata.sMsgBuf->sDBHead.cNextFlag,\
			rcvdata.sMsgBuf->sDBHead.cRaflag,rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
			rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
			rcvdata.sMsgBuf->sDBHead.cZip,rcvdata.sMsgBuf->sDBHead.nLen);
		//g_pBpcLog.LogBpc(LOG_ERROR,rcvdata.sMsgBuf,true);
		m_pRes->m_pBpcCallDataLog.LogBpc(LOG_ERROR,rcvdata.sMsgBuf,true);
		m_pMemPool->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
	}
}

void CPuThread::DispatchBpuIsReg(S_BPC_RSMSG &rcvdata)
{
	bool bRes=false;
	
	if (rcvdata.sMsgBuf->sDBHead.nLen <1)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"BPUע��ʱ��bpu������ index=%d",rcvdata.sMsgBuf->sBpcHead.nBpuIndex);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPC_NOGROUP;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		rcvdata.sMsgBuf->sDBHead.nLen = 0;
		rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->SendMsg(rcvdata);
		return;
	}
	rcvdata.sMsgBuf->sBuffer[rcvdata.sMsgBuf->sDBHead.nLen]=0;
	bool isfind=false;
	int  bpuindex=0;
	for (int i=0;i<m_pRes->g_vBpuGroupInfo.size();i++)
	{
		if (strcmp(m_pRes->g_vBpuGroupInfo[i].g_sBpuGroupName.c_str(),rcvdata.sMsgBuf->sBuffer) == 0) //�ҵ���BPU��
		{
			bpuindex = i;
			isfind = true;
			break;
		}
	}
	int connbpu=0;
	int connNum=0;
	int k;
	if (isfind) //�д�bpu��
	{
		connNum=0;
		for (k=m_pRes->g_vBpuGroupInfo[bpuindex].g_nBeginSocketIndex ; k < m_pRes->g_vBpuGroupInfo[bpuindex].g_nEndSocketIndex; k++)
		{
			if (m_pSockMgr->at(k)->m_sock != INVALID_SOCKET)
			{
				connNum++;
			}
		}
		//�жϴ�BPU���Ƿ���ȫ��������Ա
//		if (m_pRes->g_vBpuGroupInfo[bpuindex].g_nStartBpuNum >= m_pRes->g_vBpuGroupInfo[bpuindex].g_nBpuNum)
		if (connNum >= m_pRes->g_vBpuGroupInfo[bpuindex].g_nBpuNum)
		{
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.nLen = 0;
			rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPC_BPULIMIT;
			m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->OnClose("����BPU��������",__FILE__,__LINE__);
			m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->SendMsg(rcvdata);
			return ;
		}
		std::string gpname= m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName;
		//�����������������ŵ��������в���������BPU��+1
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->m_sBpuGroupName = rcvdata.sMsgBuf->sBuffer;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->m_nBpuGroupIndex = bpuindex;
		m_pRes->g_vBpuGroupInfo[bpuindex].g_nStartBpuNum++;
		//��bpu�Ľ���ID�ŵ���������
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->m_nPid = rcvdata.sMsgBuf->sBpcHead.nRtime;

		//BPC�����Ƿ�Ҫע��
		if (m_pRes->g_nRegisterMode == REGISTERMODE_NONE || m_pRes->g_nRegisterMode == REGISTERMODE_STATIC)
		{
			m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"BPC��������BPUע��");
			m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->m_bChecked = true;//������Ϊ�Ѽ��
			rcvdata.sMsgBuf->sDBHead.nLen = 0;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPC_NOREGISTER;
			m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->SendMsg(rcvdata);
			
			return;
		}
		else //�����Ƿ�Ҫע��
		{
			bRes = m_pRes->g_vBpuGroupInfo[bpuindex].g_bIsReg;
			if (bRes)
			{
				m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"BPU֮ǰ�Ѿ�ע���");
			}
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			sprintf(rcvdata.sMsgBuf->sBuffer,"%d",bRes);  //1�Ѿ�ע���,0δע�� ��Ҫע��
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN + rcvdata.sMsgBuf->sDBHead.nLen;
			m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->SendMsg(rcvdata);
		}
	}
	else //�޴�bpu��
	{
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.nLen = 0;
		rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPC_NOGROUP;
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"BPUע��ʱ��bpu������ %s",rcvdata.sMsgBuf->sBuffer);
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nBpuIndex)->SendMsg(rcvdata);
	}
}


void CPuThread::LogDrebHead(int loglevel, DREB_HEAD head, const char *msg, const char *filename, int fileline)
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

void CPuThread::DispatchTrans(S_BPC_RSMSG &rcvdata)
{
	rcvdata.nRtime = time(NULL);
	//ת�����ȼ�Ϊ0
	m_pPoolData->PushData(rcvdata);//������У�msgthread�߳̽��д���
}

void CPuThread::DispatchGetNext(S_BPC_RSMSG &rcvdata)
{
	//ȡ��������Ӧ��ֱ�ӷ���
	//DREB��Ӧ��
	if (m_pSockMgr->AffirmIndex(rcvdata.sMsgBuf->sBpcHead.nIndex))
	{
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nNodeId = m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->m_head.a_Ainfo.a_nNodeId;
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_cNodePrivateId = m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->m_head.a_Ainfo.a_cNodePrivateId;
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata,0);
		return;
	}
	m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"GETNEXTӦ�𷵻ص�dreb���ӷǷ� %d",rcvdata.sMsgBuf->sBpcHead.nIndex);
	LogDrebHead(LOG_ERROR_FAULT,rcvdata.sMsgBuf->sDBHead,"GETNEXTӦ�𷵻�dreb���ӷǷ�",__FILE__,__LINE__);
	m_pMemPool->PoolFree(rcvdata.sMsgBuf);
	rcvdata.sMsgBuf = NULL;
	return;
}

void CPuThread::DispatchReq(S_BPC_RSMSG &rcvdata)
{
	//����϶���Ӧ����
	if (rcvdata.sMsgBuf->sDBHead.cRaflag == 0)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"MSGREQӦ�𷵻ص�cRaFlag�Ƿ���ӦΪ1Ӧ��");
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"MSGREQӦ�𷵻ص�cRaFlag�Ƿ���ӦΪ1Ӧ�� ��Ϣ[%s]  DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Դ[%d %d] cZip[%d] ҵ�����ݳ���[%d]",\
			GetBpcMsgType(rcvdata.sMsgBuf->sBpcHead.cMsgType).c_str(),\
			GetDrebCmdType(rcvdata.sMsgBuf->sDBHead.cCmd).c_str(),rcvdata.sMsgBuf->sDBHead.cNextFlag,\
			rcvdata.sMsgBuf->sDBHead.cRaflag,rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
			rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
			rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
			rcvdata.sMsgBuf->sDBHead.cZip,rcvdata.sMsgBuf->sDBHead.nLen);

		rcvdata.sMsgBuf->sDBHead.cRaflag =1;
		m_pMemPool->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
		OnClose(rcvdata.index,"���ش��� �������ݷǷ����ر�����",__FILE__,__LINE__);
		return;
	}
	if (rcvdata.sMsgBuf->sDBHead.cNextFlag == 2 || rcvdata.sMsgBuf->sDBHead.cNextFlag == 20)
	{
		//�����BPC���Ӧ�� 2Ϊbpc���ɶ������,20Ϊ�������
		BpcNext(rcvdata);
		return;
	}
	if (rcvdata.sMsgBuf->sDBHead.cNextFlag == 0 || rcvdata.sMsgBuf->sDBHead.cNextFlag == 20 || \
		rcvdata.sMsgBuf->sDBHead.cNextFlag == 30 || rcvdata.sMsgBuf->sDBHead.cNextFlag == 10)
	{
		//�޺�������
		if (m_pSockMgr->AffirmIndex(rcvdata.index))
		{
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"BPU��[%s %d] BPU[%d %d]Ӧ���޺�����",\
				m_pSockMgr->at(rcvdata.index)->m_sBpuGroupName.c_str(),\
				m_pSockMgr->at(rcvdata.index)->m_nBpuGroupIndex,\
				m_pSockMgr->at(rcvdata.index)->m_index,\
				rcvdata.index);
//			m_pSockMgr->at(rcvdata.index)->m_bIsBuGetFlag = true;
		}
	}
	//DREB��Ӧ��
	if (m_pSockMgr->AffirmIndex(rcvdata.sMsgBuf->sBpcHead.nIndex))
	{
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nNodeId = m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->m_head.a_Ainfo.a_nNodeId;
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_cNodePrivateId = m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->m_head.a_Ainfo.a_cNodePrivateId;
//		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
		m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata,0);
		return;
	}
	m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"MSGREQӦ�𷵻ص�dreb���ӷǷ� ��Ϣ[%s]  DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] cZip[%d] ҵ�����ݳ���[%d]",\
		GetBpcMsgType(rcvdata.sMsgBuf->sBpcHead.cMsgType).c_str(),\
		GetDrebCmdType(rcvdata.sMsgBuf->sDBHead.cCmd).c_str(),rcvdata.sMsgBuf->sDBHead.cNextFlag,\
		rcvdata.sMsgBuf->sDBHead.cRaflag,rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
		rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
		rcvdata.sMsgBuf->sDBHead.cZip,rcvdata.sMsgBuf->sDBHead.nLen);
	
	//g_pBpcLog.LogBpc(LOG_ERROR,rcvdata.sMsgBuf,true);
	m_pRes->m_pDrebDataLog.LogBpc(LOG_ERROR,rcvdata.sMsgBuf,true);
	m_pMemPool->PoolFree(rcvdata.sMsgBuf);
	rcvdata.sMsgBuf = NULL;
	return;
}



void CPuThread::BpcNext(S_BPC_RSMSG &rcvdata)
{
	S_NEXT_TBL nb;
	char sNextFile[400];
	FILE *fp=NULL;
	if (g_pBpcTime.m_pNextTbl.Select(rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,nb))
	{
		//�ҵ��ˣ�������д���ļ�
#ifdef _WINDOWS
		sprintf(sNextFile,"%s\\nxt\\%ld.nxt",m_pRes->g_sCurPath,nb.nFileSerial);
#else
		sprintf(sNextFile,"%s/nxt/%ld.nxt",m_pRes->g_sCurPath,nb.nFileSerial);
#endif
		fp = fopen(sNextFile,"ab");
		if (fp == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ļ�[%s]����",sNextFile);
			return ;
		}
		fwrite(rcvdata.sMsgBuf->sBuffer,1,rcvdata.sMsgBuf->sDBHead.nLen,fp);
		fclose(fp);
		//g_pBpcLog.LogBpc(LOG_ERROR,rcvdata.sMsgBuf,true);
		m_pRes->m_pDrebDataLog.LogBpc(LOG_ERROR,rcvdata.sMsgBuf,true);
		m_pMemPool->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
	}
	else
	{
		//�������ļ�
		std::string sfile = m_pRes->GetNextFileName(nb.nFileSerial);
		nb.nDrebSerial = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial;
		nb.nNodeId = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId;
		nb.nNodePrivateId = rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId;
		
		fp = fopen(sfile.c_str(),"wb");
		if (fp == NULL)
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ļ�[%s]����",sfile.c_str());
			return ;
		}
		fwrite(rcvdata.sMsgBuf->sBuffer,1,rcvdata.sMsgBuf->sDBHead.nLen,fp);
		fclose(fp);
		nb.nTime = time(NULL);
		g_pBpcTime.m_pNextTbl.Insert(nb);
		//���ظ�ǰ��
		rcvdata.sMsgBuf->sDBHead.n_Ninfo.n_nNextNo = nb.nFileSerial;
		rcvdata.sMsgBuf->sDBHead.n_Ninfo.n_nNextOffset = rcvdata.sMsgBuf->sDBHead.nLen;
		//DREB��Ӧ��
		if (m_pSockMgr->AffirmIndex(rcvdata.sMsgBuf->sBpcHead.nIndex))
		{
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nNodeId = m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->m_head.a_Ainfo.a_nNodeId;
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_cNodePrivateId = m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->m_head.a_Ainfo.a_cNodePrivateId;
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
			m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata,0);
			
			return;
		}
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"Ӧ�𷵻ص�dreb���ӷǷ� ��Ϣ[%s]  DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] cZip[%d] ҵ�����ݳ���[%d]",\
			GetBpcMsgType(rcvdata.sMsgBuf->sBpcHead.cMsgType).c_str(),\
			GetDrebCmdType(rcvdata.sMsgBuf->sDBHead.cCmd).c_str(),rcvdata.sMsgBuf->sDBHead.cNextFlag,\
			rcvdata.sMsgBuf->sDBHead.cRaflag,rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
			rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
			rcvdata.sMsgBuf->sDBHead.cZip,rcvdata.sMsgBuf->sDBHead.nLen);
		
		m_pRes->m_pDrebDataLog.LogBpc(LOG_ERROR,rcvdata.sMsgBuf,true);
		//g_pBpcLog.LogBpc(LOG_ERROR,rcvdata.sMsgBuf,true);
		m_pMemPool->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;

	}
	return;
}
int CPuThread::OnTimer(unsigned int eventid, void *p)
{
	CPuThread *pp = (CPuThread *)p;
	if (eventid == 0)
	{
		pp->PingTimer();
	}
	return 0;
}
void CPuThread::PingTimer()
{
	//OnNoEvent();
	m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"��ʱ��");
	m_bIsPingTimer = true;
}
void CPuThread::StartTimer()
{
	m_pTimer.Start();
}

void CPuThread::StopTimer()
{
	m_pTimer.Stop();
}