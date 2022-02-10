// DrebMsgProcBase.h: interface for the CDrebMsgProcBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DREBMSGPROCBASE_H__EC38AEB7_E135_4E09_92E4_4D13D9D38982__INCLUDED_)
#define AFX_DREBMSGPROCBASE_H__EC38AEB7_E135_4E09_92E4_4D13D9D38982__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "BF_DrebServer.h"

//��ΪDREBAPIʹ�õ�һ�����ӣ��ɼ̳���ʵ��
//���̴߳�api������ȡ���ݽ��д���

class CDrebMsgProcBase : public CBF_Thread  
{
public:
	CDrebMsgProcBase()
	{
		m_bIsRunning = false;
		m_pDrebApi = NULL;
		m_pRes = NULL;
	}
	virtual ~CDrebMsgProcBase()
	{

	}


	// ������: Start
	// ���  : ������ 2015-4-23 15:52:29
	// ����  : virtual bool 
	// ����  : ���������߳�
	virtual bool Start()
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
	virtual void Stop()
	{
		m_bIsRunning = false;
		Join();
	}

	// ������: bool Init
	// ���  : ������ 2015-4-23 15:52:55
	// ����  : virtual  
	// ����  : CBF_DrebServer   *bus
	// ����  : CBF_DrebResource *res
	// ����  : �����̳߳�ʼ��
	virtual  bool Init(CBF_DrebServer   *bus,CBF_DrebResource *res)
	{
		m_pDrebApi = bus;
		m_pRes = res;
		return true;
	}
	CBF_DrebServer   *m_pDrebApi;//apiʵ��
	CBF_DrebResource *m_pRes;    //������

	S_BPC_RSMSG      m_rMsgData;
	
	virtual std::string GetBpcMsgType(int msgtype)
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
		case MSG_BPCMONITOR:      //16      //�����ļ�ر��棬��ʱ����
			return "MSG_BPCMONITOR";
		case MSG_GETBPCNEXT:      //17      //dreb������ȡ��BPC����ĺ���������Ϣ
			return "MSG_GETBPCNEXT";
		case MSG_CONNECTDREB:      //18      //dreb������������DREB�ɹ�����Ϣ����������Ϣ�����ע���(����ѯ�ʣ�ע��)
			return "MSG_CONNECTDREB";
		case MSG_MONITOR_SAP:      //19      //dreb��������������Ϣ��DREB������ΪCMD_MONITOR_BPC,����ļ������service_no����DrebPubDefine.h���BPCMONI_INFO����
			return "MSG_MONITOR_SAP";
		case MSG_DREBANS:      //20      //DREB������Ӧ����Ϣ
			return "MSG_DREBANS";
		default:
			return "";
		}
	}
	virtual std::string GetDrebCmdType(int cmdtype)
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
		case  CMD_UNREGSERVICE:      //14 //ȡ������ע��
			return "CMD_UNREGSERVICE";
		case  CMD_MONITOR_DREB:     //15 //�������߽ڵ���
			return "CMD_MONITOR_DREB";
		case  CMD_MONITOR_BPC:     //16 //�������߽ڵ���
			return "CMD_MONITOR_BPC";
		default:
			return "";
		}
	}
protected:
	bool  m_bIsRunning;

	// ������: Run
	// ���  : ������ 2015-4-23 15:53:15
	// ����  : virtual int 
	// ����  : �����߳����
	virtual int Run()
	{
		while (m_bIsRunning)
		{
			if (m_pDrebApi->GetMsgData(m_rMsgData))
			{
				ProcessDrebData(m_rMsgData);
			}
		}
		return 0;
	}
	virtual bool InitThreadInstance()
	{
		return true;
	}
	virtual void ExitThreadInstance()
	{

	}

	// ������: ProcessDrebData
	// ���  : ������ 2015-4-23 15:53:34
	// ����  : virtual void 
	// ����  : S_BPC_RSMSG &rcvdata
	// ����  : ��������߽��չ���������
	virtual void ProcessDrebData(S_BPC_RSMSG &rcvdata)
	{
	    if (rcvdata.sMsgBuf->sDBHead.cZip != 0)
		{
			if (!m_pDrebApi->UnzipBuf(*rcvdata.sMsgBuf))
			{
				sprintf(rcvdata.sMsgBuf->sBuffer,"���ݽ�ѹ������");
				rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
				rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
				rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
				rcvdata.sMsgBuf->sDBHead.cZip = 0;
				m_pDrebApi->SendMsg(rcvdata);
				return ;
			}
		}
		switch (rcvdata.sMsgBuf->sBpcHead.cMsgType)
		{
			case MSG_GETBPCNEXT://��bpc��ȡ������
				OnGetBpcNextPack(rcvdata);
				break;
			case MSG_GETNEXT://��BPU��ȡ��������Ҫ����BPU����
				OnGetBpuNextPack(rcvdata);
				break;
			case MSG_REQ://DREB���յ�������
				OnMsgRequest(rcvdata);
				break;
			case MSG_MONITOR_SAP://DREB���յ��ļ��ָ��
				OnMsgMonitor(rcvdata);
				break;
			case MSG_CONNECTDREB://DREB�������ӳɹ����������������ע�ύ�׵�
				OnMsgConnectBack(rcvdata);
				break;
			case MSG_DREBANS://DREB��������ȵ�Ӧ��
				OnMsgDrebAns(rcvdata);
				break;	
			case MSG_BPCMONITOR://�����ϱ������Ϣ
				OnMsgReportBpc(rcvdata);
				break;	
			default:
				m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
				rcvdata.sMsgBuf = NULL;
				break;
		}
		return;
	}

	// ������: OnGetBpcNextPack
	// ���  : ������ 2015-4-23 15:53:52
	// ����  : virtual void 
	// ����  : S_BPC_RSMSG &rcvdata
	// ����  : ȡ���صĺ�����,(���������ڱ���)
	virtual void OnGetBpcNextPack(S_BPC_RSMSG &rcvdata)
	{
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
	}

	// ������: OnGetBpuNextPack
	// ���  : ������ 2015-4-23 15:54:14
	// ����  : virtual void 
	// ����  : S_BPC_RSMSG &rcvdata
	// ����  : ȡBPU�ϵĺ�������Ҫ����BPU����
	virtual void OnGetBpuNextPack(S_BPC_RSMSG &rcvdata)
	{
		m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
		rcvdata.sMsgBuf = NULL;
	}

	// ������: OnMsgRequest
	// ���  : ������ 2015-4-23 15:54:38
	// ����  : virtual void 
	// ����  : S_BPC_RSMSG &rcvdata
	// ����  : �������߹�������������
	virtual void OnMsgRequest(S_BPC_RSMSG &rcvdata)
	{
		if (rcvdata.sMsgBuf->sDBHead.cRaflag !=0) //��Ϊ����
		{
			m_pDrebApi->GetLogPoint()->LogMp(LOG_ERROR,__FILE__,__LINE__,"�޴˹��� ��Ϣ[%s]  DREB����[%s] ����[%d] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] Դ[%d %d] cZip[%d] ҵ�����ݳ���[%d]",\
				GetBpcMsgType(rcvdata.sMsgBuf->sBpcHead.cMsgType).c_str(),\
				GetDrebCmdType(rcvdata.sMsgBuf->sDBHead.cCmd).c_str(),rcvdata.sMsgBuf->sDBHead.cNextFlag,\
				rcvdata.sMsgBuf->sDBHead.cRaflag,rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nNodeId,\
				rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_cNodePrivateId,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nDrebSerial,\
				rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nSerial,rcvdata.sMsgBuf->sDBHead.s_Sinfo.s_nHook,\
						rcvdata.sMsgBuf->sDBHead.cZip,rcvdata.sMsgBuf->sDBHead.nLen);
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			return;
		}
		//���ݲ�ͬ����Ϣ���ͽ��д���
		
	}

	// ������: OnMsgMonitor
	// ���  : ������ 2015-4-23 15:54:57
	// ����  : virtual void 
	// ����  : S_BPC_RSMSG &rcvdata
	// ����  : �������߹����ļ��ָ��
	virtual void OnMsgMonitor(S_BPC_RSMSG &rcvdata)
	{
		//DREB�����ĵ���
		switch (rcvdata.sMsgBuf->sDBHead.d_Dinfo.d_nServiceNo)
		{
			case BPCMONI_INFO:
				MonitorInfo(rcvdata);
				break;
			case BPCMONI_HOST:
				MonitorHost(rcvdata);
				break;
			case BPCMONI_BPU:
				MonitorBpu(rcvdata);
				break;
			case BPCMONI_REPORT:
				MonitorReport(rcvdata);
				break;
			case BPCMONI_SETBPU:
				MonitorSetBpu(rcvdata);
				break;
			case BPCMONI_SETMODE:
				MonitorSetMode(rcvdata);
				break;
			case BPCMONI_STOPBPU:
				MonitorStopBpu(rcvdata);
				break;
			case BPCMONI_STARTBPU:
				MonitorStartBpu(rcvdata);
				break;
			case BPCMONI_DOWNLOAD:
				Download(rcvdata);
				break;
			case BPCMONI_UPLOAD:
				Upload(rcvdata);
				break;
			default:
				rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
				rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_FUNCNOTFUND;
				rcvdata.sMsgBuf->sDBHead.nLen = 0;
				rcvdata.sMsgBuf->sBpcHead.nBpcLen = DREBHEADLEN;
				m_pDrebApi->SendMsg(rcvdata);
				break;
		}
		return;
	}

	// ������: OnMsgConnectBack
	// ���  : ������ 2015-4-23 15:55:29
	// ����  : virtual void 
	// ����  : S_BPC_RSMSG &rcvdata
	// ����  : �����������߳ɹ�����Ӧ
	virtual void OnMsgConnectBack(S_BPC_RSMSG &rcvdata)
	{
		//����ע���
		m_pDrebApi->GetLogPoint()->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�յ�DREB���ӳɹ�����Ӧ");
	}

	// ������: OnMsgDrebAns
	// ���  : ������ 2015-4-23 15:55:58
	// ����  : virtual void 
	// ����  : S_BPC_RSMSG &rcvdata
	// ����  : ��Ӧ���߹�����Ӧ������
	virtual void OnMsgDrebAns(S_BPC_RSMSG &rcvdata)
	{
		//Ӧ������
		m_pDrebApi->GetLogPoint()->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�յ�DREB��Ӧ����Ӧ");
	}

	// ������: OnMsgReportBpc
	// ���  : ������ 2015-4-23 16:01:43
	// ����  : virtual void 
	// ����  : S_BPC_RSMSG &rcvdata
	// ����  : �����ϱ������Ϣ����������
	virtual void OnMsgReportBpc(S_BPC_RSMSG &rcvdata)
	{
		//�����Ϣ
		CBF_Xml        pXml;

		if (!pXml.FromBuffer(rcvdata.sMsgBuf->sBuffer))
		{
			m_pDrebApi->GetLogPoint()->LogMp(LOG_ERROR,__FILE__,__LINE__,"API�����ķ�xml��ʽ");
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			return;
		}
//		int i,k;
// 		int connNum=0;
// 		//BPC��Ϣ
// 		pXml.AddNodeByPath("Monitor/BPU��Ϣ",false,"BPULIST",false,"");
// 		for (i=0 ; i<m_pRes->g_vBpuGroupInfo.size(); i++)
// 		{
// 			connNum=0;
// 			for (k=m_pRes->g_vBpuGroupInfo[i].g_nBeginSocketIndex ; k < m_pRes->g_vBpuGroupInfo[i].g_nEndSocketIndex; k++)
// 			{
// 				if (m_pSockMgr->at(k)->m_sock != INVALID_SOCKET)
// 				{
// 					connNum++;
// 				}
// 			}
// 			CXmlNode *bpunode = pXml.AddNodeByPath("Monitor/BPU��Ϣ/BPULIST",false,m_pRes->g_vBpuGroupInfo[i].g_sBpuGroupName.c_str(),true,"");
// 			if (bpunode != NULL)
// 			{
// 				bpunode->SetAttribute("����",false,m_pRes->g_vBpuGroupInfo[i].g_sBpuGroupName);
// 				bpunode->SetAttribute("��Ŀ",false,m_pRes->g_vBpuGroupInfo[i].g_nBpuNum);
// 				bpunode->SetAttribute("ʵ����",false,connNum);
// 				bpunode->SetAttribute("����Ӧ��",false,m_pRes->g_vBpuGroupInfo[i].g_bIsAnsPing);
// 				bpunode->SetAttribute("����ģʽ",false,m_pRes->g_vBpuGroupInfo[i].g_nDispatchMode);
// 				bpunode->SetAttribute("�Զ�����",false,m_pRes->g_vBpuGroupInfo[i].g_bIsAutoLoadBu);
// 				bpunode->SetAttribute("������",false,m_pRes->g_vBpuGroupInfo[i].g_sBuPrgName);
// 				bpunode->SetAttribute("����Ŀ¼",false,m_pRes->g_vBpuGroupInfo[i].g_sBuPrgPath);
// 				bpunode->SetAttribute("����Ŀ¼",false,m_pRes->g_vBpuGroupInfo[i].g_sBuRunPath);
// 				bpunode->SetAttribute("����",false,m_pRes->g_vBpuGroupInfo[i].g_sBuPrgPara);
// 			}
// 		}

		int len = BPUDATASIZE;
		if (!pXml.ToBuffer(rcvdata.sMsgBuf->sBuffer,len))
		{
			m_pDrebApi->GetLogPoint()->LogMp(LOG_ERROR,__FILE__,__LINE__,"ToBuffer����");
			m_pDrebApi->PoolFree(rcvdata.sMsgBuf);
			rcvdata.sMsgBuf = NULL;
			return;
		}
		rcvdata.sMsgBuf->sDBHead.nLen = len;
		m_pDrebApi->GetLogPoint()->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"���֪ͨ",rcvdata.sMsgBuf->sBuffer,rcvdata.sMsgBuf->sDBHead.nLen);
		//��������	
		rcvdata.sMsgBuf->sBpcHead.nIndex = 100;
		m_pDrebApi->SendMsg(rcvdata);
		return;
	}
	
	virtual void MonitorInfo(S_BPC_RSMSG &rcvdata)
	{
		int i;
		CBF_Xml xmlpack;
		xmlpack.SetNodeValueByPath("Monitor/BPC�����ڵ�",false,(int)m_pRes->g_nSvrMainId);
		xmlpack.SetNodeValueByPath("Monitor/BPC˽�нڵ�",false,(int)m_pRes->g_nSvrPrivateId);
		xmlpack.SetNodeValueByPath("Monitor/����ʱ��",false,m_pRes->g_sStartDateTime,false);
		char sip[40];
		char drebstatus[40];
		int  drebport;
		int  drebid;
		int  drebprivateid;
		
		//������Ϣ
		for (i=0 ; i < m_pRes->g_vDrebLinkInfo.size() ; i++)
		{
			CXmlNode *connnode = xmlpack.AddNodeByPath("Monitor/DREB������Ϣ",false,"����",false,"");
			if (connnode == NULL)
			{
				m_pDrebApi->GetLogPoint()->LogMp(LOG_ERROR,__FILE__,__LINE__,"AddNodeByPath ����");
				continue;
			}
			bzero(sip,sizeof(sip));
			bzero(drebstatus,sizeof(drebstatus));
			m_pDrebApi->GetDrebInfo(i,sip,drebport,drebid,drebprivateid,drebstatus);
			connnode->SetAttribute("ip",false,sip);
			connnode->SetAttribute("port",false,drebport);
			connnode->SetAttribute("index",false,i);
			connnode->SetAttribute("�����ڵ�",false,drebid);
			connnode->SetAttribute("˽�нڵ�",false,drebprivateid);
			connnode->SetAttribute("status",false,drebstatus);
			
		}
		
		//BPU��Ϣ
// 		int k;
// 		int connNum=0;
// 		xmlpack.AddNodeByPath("Monitor",false,"BPU�б�",false,"");
// 		for (i=0 ; i<m_pRes->g_vBpuGroupInfo.size(); i++)
// 		{
// 			connNum=0;
// 			for (k=m_pRes->g_vBpuGroupInfo[i].g_nBeginSocketIndex ; k < m_pRes->g_vBpuGroupInfo[i].g_nEndSocketIndex; k++)
// 			{
// 				if (m_pSockMgr->at(k)->m_sock != INVALID_SOCKET)
// 				{
// 					connNum++;
// 				}
// 			}
// 			CXmlNode *bpunode = xmlpack.AddNodeByPath("Monitor/BPU�б�",false,m_pRes->g_vBpuGroupInfo[i].g_sBpuGroupName.c_str(),true,"");
// 			bpunode->SetAttribute("����BPU��",false,m_pRes->g_vBpuGroupInfo[i].g_nBpuNum);
// 			bpunode->SetAttribute("ʵ��BPU��",false,connNum);
// 			bpunode->SetAttribute("����ģʽ",false,m_pRes->g_vBpuGroupInfo[i].g_nDispatchMode);
// 			bpunode->SetAttribute("�Զ�����",false,m_pRes->g_vBpuGroupInfo[i].g_bIsAutoLoadBu);
// 			bpunode->SetAttribute("��ʼ��������",false,m_pRes->g_vBpuGroupInfo[i].g_nBeginSocketIndex);
// 			bpunode->SetAttribute("������������",false,m_pRes->g_vBpuGroupInfo[i].g_nEndSocketIndex);
// 		}
		
		int len = BPUDATASIZE;
		if (!xmlpack.ToBuffer(rcvdata.sMsgBuf->sBuffer,len))
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"���岻�㣬������bpcinfo.txt�鿴");
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			m_pDrebApi->GetLogPoint()->LogMp(LOG_ERROR,__FILE__,__LINE__,"MonitorInfo ���岻��");
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
	}
	
	virtual void MonitorHost(S_BPC_RSMSG &rcvdata)
	{
		CBF_Xml xmlpack;
		xmlpack.SetNodeValueByPath("Monitor/BPC�����ڵ�",false,(int)m_pRes->g_nSvrMainId);
		xmlpack.SetNodeValueByPath("Monitor/BPC˽�нڵ�",false,(int)m_pRes->g_nSvrPrivateId);
		xmlpack.SetNodeValueByPath("Monitor/����ʱ��",false,m_pRes->g_sStartDateTime,false);
		
		int total;
		int unused;
		int bufsize;
		m_pDrebApi->GetBufferPoolInfo(total,unused,bufsize);
		xmlpack.SetNodeAttribute("Monitor/�ڴ滺���",false,"�ܷ���",false,total );
		xmlpack.SetNodeAttribute("Monitor/�ڴ滺���",false,"δʹ��",false,unused );
		xmlpack.SetNodeAttribute("Monitor/�ڴ滺���",false,"ÿ���С",false,bufsize );
		//ȡ������Ϣ
		S_MONITOR_HOST hf;
		vector<S_MONITOR_DISK> di;
		m_pDrebApi->GetHostInfo(hf,di);
		
		xmlpack.SetNodeValueByPath("Monitor/������Դ/CPU",false,(int)hf.nCpuRate);
		
		
		xmlpack.SetNodeValueByPath("Monitor/������Դ/�ڴ�",false,"",false);
		xmlpack.SetNodeAttribute("Monitor/������Դ/�ڴ�",false,"�ܴ�СM",false,(int)hf.nTotalMemory);
		xmlpack.SetNodeAttribute("Monitor/������Դ/�ڴ�",false,"ʹ��M",false,(int)hf.nUsedMemory);
		
		for (int i=0 ; i<di.size() ; i++)
		{
			CXmlNode *disknode = xmlpack.AddNodeByPath("Monitor/������Դ/�����б�",false,"����",false,"");
			if (disknode != NULL)
			{
				disknode->SetAttribute("����",false,di[i].sDiskName);
				disknode->SetAttribute("�ܴ�СM",false,(int)di[i].nTotalSpace);
				disknode->SetAttribute("ʹ�ô�СM",false,(int)di[i].nUsedSpace);
			}
		}
		
		int len = BPUDATASIZE;
		if (!xmlpack.ToBuffer(rcvdata.sMsgBuf->sBuffer,len))
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"���岻�㣬������bpchost.txt�鿴");
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			m_pDrebApi->GetLogPoint()->LogMp(LOG_ERROR,__FILE__,__LINE__,"MonitorHost ���岻��");
			xmlpack.ToFile("bpchost.txt");
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

	virtual void MonitorBpu(S_BPC_RSMSG &rcvdata)
	{
		
		int connNum=0;
		CBF_Xml xmlpack;
		xmlpack.SetNodeValueByPath("Monitor/BPC�����ڵ�",false,(int)m_pRes->g_nSvrMainId);
		xmlpack.SetNodeValueByPath("Monitor/BPC˽�нڵ�",false,(int)m_pRes->g_nSvrPrivateId);
		xmlpack.SetNodeValueByPath("Monitor/����ʱ��",false,m_pRes->g_sStartDateTime,false);
		//BPU��Ϣ
		xmlpack.AddNodeByPath("Monitor",false,"BPU�б�",false,"");
//		int i,k;
// 		for (i=0 ; i<m_pRes->g_vBpuGroupInfo.size(); i++)
// 		{
// 			connNum=0;
// 			for (k=m_pRes->g_vBpuGroupInfo[i].g_nBeginSocketIndex ; k < m_pRes->g_vBpuGroupInfo[i].g_nEndSocketIndex; k++)
// 			{
// 				if (m_pSockMgr->at(k)->m_sock != INVALID_SOCKET)
// 				{
// 					connNum++;
// 				}
// 			}
// 			CXmlNode *bpunode = xmlpack.AddNodeByPath("Monitor/BPU�б�",false,m_pRes->g_vBpuGroupInfo[i].g_sBpuGroupName.c_str(),true,"");
// 			if (bpunode != NULL)
// 			{
// 				bpunode->SetAttribute("����",false,m_pRes->g_vBpuGroupInfo[i].g_sBpuGroupName);
// 				bpunode->SetAttribute("����BPU��",false,m_pRes->g_vBpuGroupInfo[i].g_nBpuNum);
// 				bpunode->SetAttribute("ʵ��BPU��",false,connNum);
// 				bpunode->SetAttribute("����Ӧ��",false,m_pRes->g_vBpuGroupInfo[i].g_bIsAnsPing);
// 				bpunode->SetAttribute("����ģʽ",false,m_pRes->g_vBpuGroupInfo[i].g_nDispatchMode);
// 				bpunode->SetAttribute("�Զ�����",false,m_pRes->g_vBpuGroupInfo[i].g_bIsAutoLoadBu);
// 				bpunode->SetAttribute("������",false,m_pRes->g_vBpuGroupInfo[i].g_sBuPrgName);
// 				bpunode->SetAttribute("����Ŀ¼",false,m_pRes->g_vBpuGroupInfo[i].g_sBuPrgPath);
// 				bpunode->SetAttribute("����Ŀ¼",false,m_pRes->g_vBpuGroupInfo[i].g_sBuRunPath);
// 				bpunode->SetAttribute("���в���",false,m_pRes->g_vBpuGroupInfo[i].g_sBuPrgPara);
// 				bpunode->SetAttribute("��ʼ��������",false,m_pRes->g_vBpuGroupInfo[i].g_nBeginSocketIndex);
// 				bpunode->SetAttribute("������������",false,m_pRes->g_vBpuGroupInfo[i].g_nEndSocketIndex);
// 				
// 			}
// 		}
		
		int len = BPUDATASIZE;
		if (!xmlpack.ToBuffer(rcvdata.sMsgBuf->sBuffer,len))
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"���岻�㣬������bpcbpu.txt�鿴");
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			m_pDrebApi->GetLogPoint()->LogMp(LOG_ERROR,__FILE__,__LINE__,"MonitorBpu ���岻��");
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

	virtual void MonitorReport(S_BPC_RSMSG &rcvdata)
	{
		CBF_Xml xmlpack;
		xmlpack.SetNodeValueByPath("Monitor/BPC�����ڵ�",false,(int)m_pRes->g_nSvrMainId);
		xmlpack.SetNodeValueByPath("Monitor/BPC˽�нڵ�",false,(int)m_pRes->g_nSvrPrivateId);
		xmlpack.SetNodeValueByPath("Monitor/����ʱ��",false,m_pRes->g_sStartDateTime,false);
		char tmpchar[100];
		char nodename[200];
// 		vector<S_FUNCINFO_TBL> funclist;
// 		m_pFuncTbl->GetAllFuncByGroup(funclist);
// 		
// 		xmlpack.SetNodeValueByPath("Monitor/BPU����/������",false,(int)funclist.size());
// 
// 		std::string bpugroup;
// 		int txnum=0;
// 		bzero(nodename,sizeof(nodename));
// 		for (int i=0 ; i<funclist.size() ; i++)
// 		{
// 			if (bpugroup.compare(funclist[i].sBpuGroupName) !=0)
// 			{
// 				if (strlen(nodename) >0)
// 				{
// 					xmlpack.SetNodeAttribute(nodename,false,"������",false,txnum);
// 				}
// 				sprintf(nodename,"Monitor/BPU����/%s",funclist[i].sBpuGroupName);
// 				bpugroup = funclist[i].sBpuGroupName;
// 				txnum = 0;
// 			}
// 			txnum++;
// 			sprintf(tmpchar,"%d",funclist[i].nFuncNo);
// 			CXmlNode *bpunode = xmlpack.AddNodeByPath(nodename,false,tmpchar,false,"",false);
// 			if (bpunode != NULL)
// 			{
// 				bpunode->SetAttribute("����",false,funclist[i].sFuncName);
// 				bpunode->SetAttribute("���ȼ�",false,(int)funclist[i].cPrio);
// 				bpunode->SetAttribute("���ô���",false,(int)funclist[i].nCallNum);
// 			}
// 		}
// 		xmlpack.SetNodeAttribute(nodename,false,"������",false,txnum);

		int len = BPUDATASIZE;
		if (!xmlpack.ToBuffer(rcvdata.sMsgBuf->sBuffer,len))
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"���岻�㣬������bpureport.txt�鿴");
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			m_pDrebApi->GetLogPoint()->LogMp(LOG_ERROR,__FILE__,__LINE__,"MonitorReport ���岻��");
			xmlpack.ToFile("bpureport.txt");
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

	virtual void MonitorSetBpu(S_BPC_RSMSG &rcvdata)
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
			if (!m_pDrebApi->UnzipBuf(*rcvdata.sMsgBuf))
			{
				sprintf(rcvdata.sMsgBuf->sBuffer,"���ݽ�ѹ������");
				rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
				rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
				rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
				rcvdata.sMsgBuf->sDBHead.cZip = 0;
				m_pDrebApi->SendMsg(rcvdata);
				return ;
			}
		}
		CBF_Xml  xmlpack;
		if (!xmlpack.FromBuffer(rcvdata.sMsgBuf->sBuffer))
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"�������ݷ�xml");
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pDrebApi->SendMsg(rcvdata);
			return ;
		}
		CXmlNode *setnode = xmlpack.GetNodeByPath("Monitor/SetBpu",false);
		if (setnode == NULL)
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"xml��[Monitor/SetBpu]�ڵ�");
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pDrebApi->SendMsg(rcvdata);
			return ;
		}
		int savemode=0;
		if (xmlpack.GetNodeAttribute("Monitor/SetBpu",false,"mode",false,savemode) == NULL)
		{
			savemode =0;//������Ĭ��Ϊ0
		}
		std::string bpugroupname;
		int i;
		int bpuindex=-1;
		char tmpchar[200];
		char tmpcharconf[200];
		bool isfind=false;
		std::string namevalue;
		if (savemode == 1)
		{
// 			CBF_Date_Time dt;
// 			sprintf(tmpchar,"%s.%s%s",m_pRes->m_sBpcParaFileName.c_str(),dt.ToStringD8().c_str(),dt.ToStringT6().c_str());
// 			m_pRes->m_xmlconf.ToFile(tmpchar);
		}
		
		
// 		CXmlNode *bpugnode = (CXmlNode *)setnode->GetFirstChild();
// 		while (bpugnode != NULL)
// 		{
// 			bpugroupname = bpugnode->GetNodeName();
// 			isfind=false;
// 			for (i=0; i< m_pRes->g_vBpuGroupInfo.size() ; i++)
// 			{
// 				if (bpugroupname.compare(m_pRes->g_vBpuGroupInfo[i].g_sBpuGroupName) == 0)
// 				{
// 					isfind = true;
// 					bpuindex = i;
// 					break;
// 				}
// 			}
// 			if (isfind)
// 			{
// 				//�ҵ���
// 				sprintf(tmpchar,"Monitor/SetBpu/%s/��������",bpugroupname.c_str());
// 				if (xmlpack.GetNodeValueByPath(tmpchar,false,namevalue) != NULL)
// 				{
// 					if (savemode == 1) //���������ļ�
// 					{
// 						sprintf(tmpcharconf,"package/head/BPU����/BPU��/%s/��������",bpugroupname.c_str());
// 						m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str());
// 					}
// 					//�����ڴ�
// 					if (atoi(namevalue.c_str()) == 1)
// 					{
// 						m_pRes->g_vBpuGroupInfo[bpuindex].g_bIsUnencrypt  = true;
// 					}
// 					else
// 					{
// 						m_pRes->g_vBpuGroupInfo[bpuindex].g_bIsUnencrypt  = false;
// 					}
// 				}
// 				sprintf(tmpchar,"Monitor/SetBpu/%s/����Ӧ��",bpugroupname.c_str());
// 				if (xmlpack.GetNodeValueByPath(tmpchar,false,namevalue) != NULL)
// 				{
// 					if (savemode == 1) //���������ļ�
// 					{
// 						sprintf(tmpcharconf,"package/head/BPU����/BPU��/%s/����Ӧ��",bpugroupname.c_str());
// 						m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str());
// 					}
// 					//�����ڴ�
// 					if (atoi(namevalue.c_str()) == 1)
// 					{
// 						m_pRes->g_vBpuGroupInfo[bpuindex].g_bIsAnsPing  = true;
// 					}
// 					else
// 					{
// 						m_pRes->g_vBpuGroupInfo[bpuindex].g_bIsAnsPing  = false;
// 					}
// 				}
// 				sprintf(tmpchar,"Monitor/SetBpu/%s/����ģʽ",bpugroupname.c_str());
// 				if (xmlpack.GetNodeValueByPath(tmpchar,false,namevalue) != NULL)
// 				{
// 					if (savemode == 1) //���������ļ�
// 					{
// 						sprintf(tmpcharconf,"package/head/BPU����/BPU��/%s/����ģʽ",bpugroupname.c_str());
// 						m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str());
// 					}
// 					//�����ڴ�
// 					m_pRes->g_vBpuGroupInfo[bpuindex].g_nDispatchMode = atoi(namevalue.c_str());
// 				}
// 				sprintf(tmpchar,"Monitor/SetBpu/%s/��������BPU��",bpugroupname.c_str());
// 				if (xmlpack.GetNodeValueByPath(tmpchar,false,namevalue) != NULL)
// 				{
// 					if (m_pRes->g_vBpuGroupInfo[bpuindex].g_nDispatchMode == 1)
// 					{
// 						namevalue = "1";
// 					}
// 					//�����ڴ�
// 					m_pRes->g_vBpuGroupInfo[bpuindex].g_nBpuNum  = atoi(namevalue.c_str());
// 					
// 					if (savemode == 1) //���������ļ�
// 					{
// 						sprintf(tmpcharconf,"package/head/BPU����/BPU��/%s/��������BPU��",bpugroupname.c_str());
// 						m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str());
// 					}
// 				}
// 				
// 				sprintf(tmpchar,"Monitor/SetBpu/%s/AUTOLOADBPU",bpugroupname.c_str());
// 				if (xmlpack.GetNodeAttribute(tmpchar,false,"use",false,namevalue) != NULL)
// 				{
// 					if (atoi(namevalue.c_str()) == 1)
// 					{
// 						m_pRes->g_vBpuGroupInfo[bpuindex].g_bIsAutoLoadBu = true;
// 					}
// 					else
// 					{
// 						m_pRes->g_vBpuGroupInfo[bpuindex].g_bIsAutoLoadBu = false;
// 					}
// 
// 					if (savemode == 1) //���������ļ�
// 					{
// 						sprintf(tmpcharconf,"package/head/BPU����/BPU��/%s/AUTOLOADBPU",bpugroupname.c_str());
// 						m_pRes->m_xmlconf.SetNodeAttribute(tmpcharconf,false,"use",false,namevalue.c_str());
// 					}
// 					if (m_pRes->g_vBpuGroupInfo[bpuindex].g_bIsAutoLoadBu)//�Զ�����BPU
// 					{
// 						sprintf(tmpchar,"Monitor/SetBpu/%s/AUTOLOADBPU/BPU������",bpugroupname.c_str());
// 						if (xmlpack.GetNodeValueByPath(tmpchar,false,namevalue) != NULL)
// 						{
// 							m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuPrgName = namevalue;
// 							if (savemode == 1) //���������ļ�
// 							{
// 								sprintf(tmpcharconf,"package/head/BPU����/BPU��/%s/AUTOLOADBPU/BPU������",bpugroupname.c_str());
// 								m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str());
// 							}
// 						}
// 						sprintf(tmpchar,"Monitor/SetBpu/%s/AUTOLOADBPU/BPU����·��",bpugroupname.c_str());
// 						if (xmlpack.GetNodeValueByPath(tmpchar,false,namevalue) != NULL)
// 						{
// 							m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuPrgPath = namevalue;
// 							if (savemode == 1) //���������ļ�
// 							{
// 								sprintf(tmpcharconf,"package/head/BPU����/BPU��/%s/AUTOLOADBPU/BPU����·��",bpugroupname.c_str());
// 								m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str());
// 							}
// 						}
// 						sprintf(tmpchar,"Monitor/SetBpu/%s/AUTOLOADBPU/BPU����Ŀ¼",bpugroupname.c_str());
// 						if (xmlpack.GetNodeValueByPath(tmpchar,false,namevalue) != NULL)
// 						{
// 							m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuRunPath = namevalue;
// 							if (savemode == 1) //���������ļ�
// 							{
// 								sprintf(tmpcharconf,"package/head/BPU����/BPU��/%s/AUTOLOADBPU/BPU����Ŀ¼",bpugroupname.c_str());
// 								m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str());
// 							}
// 						}
// 						sprintf(tmpchar,"Monitor/SetBpu/%s/AUTOLOADBPU/BPU��������",bpugroupname.c_str());
// 						if (xmlpack.GetNodeValueByPath(tmpchar,false,namevalue) != NULL)
// 						{
// 							m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuPrgPara = namevalue;
// 							if (savemode == 1) //���������ļ�
// 							{
// 								sprintf(tmpcharconf,"package/head/BPU����/BPU��/%s/AUTOLOADBPU/BPU��������",bpugroupname.c_str());
// 								m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str());
// 							}
// 						}
// 					}
// 				}
// 				if (savemode == 1)
// 				{
// 					if (!m_pRes->m_xmlconf.ToFile(m_pRes->m_sBpcParaFileName.c_str()))
// 					{
// 						m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"���������ļ�[%s]ʧ��",m_pRes->m_sBpcParaFileName.c_str());
// 					}
// 				}
// 			}
// 			else
// 			{
// 				m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"bpu��[%s]��BPC�����в�����",bpugroupname.c_str());
// 			}
// 			bpugnode = (CXmlNode *)bpugnode->getNextSibling();
// 		}
		sprintf(rcvdata.sMsgBuf->sBuffer,"���ò����ɹ�");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pDrebApi->SendMsg(rcvdata);
		return ;
	}

	virtual  void MonitorSetMode(S_BPC_RSMSG &rcvdata)
	{
		if (rcvdata.sMsgBuf->sDBHead.cZip != 0)
		{
			if (!m_pDrebApi->UnzipBuf(*rcvdata.sMsgBuf))
			{
				sprintf(rcvdata.sMsgBuf->sBuffer,"���ݽ�ѹ������");
				rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
				rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
				rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
				rcvdata.sMsgBuf->sDBHead.cZip = 0;
				m_pDrebApi->SendMsg(rcvdata);
				return ;
			}
		}
		CBF_Xml  xmlpack;
		if (!xmlpack.FromBuffer(rcvdata.sMsgBuf->sBuffer))
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"�������ݷ�xml");
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pDrebApi->SendMsg(rcvdata);
			return ;
		}
		CXmlNode *setnode = xmlpack.GetNodeByPath("Monitor/SetBpu",false);
		if (setnode == NULL)
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"xml��[Monitor/SetBpu]�ڵ�");
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pDrebApi->SendMsg(rcvdata);
			return ;
		}
		int savemode=0;
		if (xmlpack.GetNodeAttribute("Monitor/SetMonitor",false,"mode",false,savemode) == NULL)
		{
			savemode =0;//������Ĭ��Ϊ0
		}
		std::string bpugroupname;
		int i;
		
		char tmpchar[200];
		char tmpcharconf[200];
		bool isfind=false;
		std::string namevalue;
		if (savemode == 1)
		{
// 			CBF_Date_Time dt;
// 			sprintf(tmpchar,"%s.%s%s",m_pRes->m_sBpcParaFileName.c_str(),dt.ToStringD8().c_str(),dt.ToStringT6().c_str());
// 			m_pRes->m_xmlconf.ToFile(tmpchar);
		}
	// 	int   g_nUseMonitor;       //�Ƿ���
	// 	int   g_nMonitorHost;      //�Ƿ񱨸�������Դ
	// 	int   g_nMonitorDrebId;            //�����ڵ��
	// 	int   g_nMonitorDrebPrivateId;     //˽�нڵ��
	// 	int   g_nMonitorSvrId;            //��������ڵ��
	// 	int   g_nMonitorSvrPrivateId;     //˽�з���ڵ��
	// 	int   g_nMonitorTxCode;           //��ؽ�����
// 		if (xmlpack.GetNodeAttribute("Monitor/SetMonitor/�������",false,"use",false,namevalue) != NULL)
// 		{
// 			if (savemode == 1) //���������ļ�
// 			{
// 				sprintf(tmpcharconf,"package/head/�������");
// 				m_pRes->m_xmlconf.SetNodeAttribute(tmpcharconf,false,"use",false,namevalue.c_str(),false);
// 			}
// 			//�����ڴ�
// 			m_pRes->g_nUseMonitor = atoi(namevalue.c_str());
// 		}
// 		if (xmlpack.GetNodeAttribute("Monitor/SetMonitor/�������",false,"host",false,namevalue) != NULL)
// 		{
// 			if (savemode == 1) //���������ļ�
// 			{
// 				sprintf(tmpcharconf,"package/head/�������");
// 				m_pRes->m_xmlconf.SetNodeAttribute(tmpcharconf,false,"host",false,namevalue.c_str(),false);
// 			}
// 			//�����ڴ�
// 			m_pRes->g_nMonitorHost = atoi(namevalue.c_str());
// 		}
// 		if (xmlpack.GetNodeValueByPath("Monitor/SetMonitor/��ع����ڵ��",false,namevalue) != NULL)
// 		{
// 			if (savemode == 1) //���������ļ�
// 			{
// 				sprintf(tmpcharconf,"package/head/�������/��ع����ڵ��");
// 				m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str(),false);
// 			}
// 			//�����ڴ�
// 			m_pRes->g_nMonitorDrebId = atoi(namevalue.c_str());
// 		}
// 		if (xmlpack.GetNodeValueByPath("Monitor/SetMonitor/���˽�нڵ��",false,namevalue) != NULL)
// 		{
// 			if (savemode == 1) //���������ļ�
// 			{
// 				sprintf(tmpcharconf,"package/head/�������/���˽�нڵ��");
// 				m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str(),false);
// 			}
// 			//�����ڴ�
// 			m_pRes->g_nMonitorDrebPrivateId = atoi(namevalue.c_str());
// 		}
// 		if (xmlpack.GetNodeValueByPath("Monitor/SetMonitor/��ع��������",false,namevalue) != NULL)
// 		{
// 			if (savemode == 1) //���������ļ�
// 			{
// 				sprintf(tmpcharconf,"package/head/�������/��ع��������");
// 				m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str(),false);
// 			}
// 			//�����ڴ�
// 			m_pRes->g_nMonitorSvrId = atoi(namevalue.c_str());
// 		}
// 		if (xmlpack.GetNodeValueByPath("Monitor/SetMonitor/���˽�з����",false,namevalue) != NULL)
// 		{
// 			if (savemode == 1) //���������ļ�
// 			{
// 				sprintf(tmpcharconf,"package/head/�������/���˽�з����");
// 				m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str(),false);
// 			}
// 			//�����ڴ�
// 			m_pRes->g_nMonitorSvrPrivateId = atoi(namevalue.c_str());
// 		}
// 		if (xmlpack.GetNodeValueByPath("Monitor/SetMonitor/��ر��潻����",false,namevalue) != NULL)
// 		{
// 			if (savemode == 1) //���������ļ�
// 			{
// 				sprintf(tmpcharconf,"package/head/�������/��ر��潻����");
// 				m_pRes->m_xmlconf.SetNodeValueByPath(tmpcharconf,false,namevalue.c_str(),false);
// 			}
// 			//�����ڴ�
// 			m_pRes->g_nMonitorTxCode = atoi(namevalue.c_str());
// 		}
// 		if (savemode == 1)
// 		{
// 			if (!m_pRes->m_xmlconf.ToFile(m_pRes->m_sBpcParaFileName.c_str()))
// 			{
// 				m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"���������ļ�[%s]ʧ��",m_pRes->m_sBpcParaFileName.c_str());
// 				sprintf(rcvdata.sMsgBuf->sBuffer,"���������ļ�[%s]ʧ��",m_pRes->m_sBpcParaFileName.c_str());
// 				rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
// 				rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
// 				rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
// 				rcvdata.sMsgBuf->sDBHead.cZip = 0;
// 				m_pSockMgr->at(rcvdata.sMsgBuf->sBpcHead.nIndex)->SendMsg(rcvdata);
// 				return;
// 			}
// 		}
		sprintf(rcvdata.sMsgBuf->sBuffer,"���ò����ɹ�");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pDrebApi->SendMsg(rcvdata);
		return;
	}

	virtual  void MonitorStopBpu(S_BPC_RSMSG &rcvdata)
	{
		if (rcvdata.sMsgBuf->sDBHead.cZip != 0)
		{
			if (!m_pDrebApi->UnzipBuf(*rcvdata.sMsgBuf))
			{
				sprintf(rcvdata.sMsgBuf->sBuffer,"���ݽ�ѹ������");
				rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
				rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
				rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
				rcvdata.sMsgBuf->sDBHead.cZip = 0;
				m_pDrebApi->SendMsg(rcvdata);
				return ;
			}
		}
		CBF_Xml  xmlpack;
		if (!xmlpack.FromBuffer(rcvdata.sMsgBuf->sBuffer))
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"�������ݷ�xml");
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pDrebApi->SendMsg(rcvdata);
			return ;
		}
		CXmlNode *stopnode = xmlpack.GetNodeByPath("Monitor/StopBpu",false);
		if (stopnode == NULL)
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"xml��[Monitor/StopBpu]�ڵ�");
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pDrebApi->SendMsg(rcvdata);
			return ;
		}
		int i;
		int bpuindex=-1;
		std::string bpugroupname;
		int bpugnum=0;
//		S_PROCESS_NODE node;
//		bool isfind = false;
// 		CXmlNode *bpunode = (CXmlNode *)stopnode->GetFirstChild();
// 		while (bpunode != NULL)
// 		{
// 			bpugroupname = bpunode->GetNodeName();
// 			isfind=false;
// 			for (i=0; i< m_pRes->g_vBpuGroupInfo.size() ; i++)
// 			{
// 				if (bpugroupname.compare(m_pRes->g_vBpuGroupInfo[i].g_sBpuGroupName) == 0)
// 				{
// 					isfind = true;
// 					bpuindex = i;
// 					break;
// 				}
// 			}
// 			if (isfind)
// 			{
// 				node.prgname = m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuPrgName;
// 				node.prgpara = m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuPrgPara;
// 				node.prgpath = m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuPrgPath;
// 				node.runpath = m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuRunPath;
// 				node.nodeid = m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName;
// 				
// 				if (m_pRes->g_vBpuGroupInfo[bpuindex].g_nDispatchMode == 1) //java�汾
// 				{
// 					m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"����ԭBPU[%s]�����Ƿ����[%d]",\
// 						m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(),m_pRes->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid);
// 					if (m_pRes->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid > 0) //�鿴�����Ƿ����
// 					{
// 						node.pid = m_pRes->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid;
// 						if (m_pMgrBpu.FindProcess(&node) == 0) //�д˽���
// 						{
// 							m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"ԭBPU[%s]����[%d]�Ѵ��ڣ�ֹͣ",\
// 								m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(),m_pRes->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid);
// 							if (m_pMgrBpu.StopProcess(&node)!=0)
// 							{
// 								m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"BPU[%s]����[%d]ֹͣʧ��",\
// 									m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(),m_pRes->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid);
// 							}
// 						}
// 					}
// 				}
// 				else
// 				{
// 					for (int j=m_pRes->g_vBpuGroupInfo[bpuindex].g_nBeginSocketIndex ; j<m_pRes->g_vBpuGroupInfo[bpuindex].g_nEndSocketIndex ;j++)
// 					{
// 						if (m_pSockMgr->at(j)->m_sock != INVALID_SOCKET)
// 						{
// 							node.pid = m_pSockMgr->at(j)->m_nPid;
// 							m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"ֹͣBPU[%s]����[%d]",\
// 								m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(),node.pid);
// 							if (m_pMgrBpu.StopProcess(&node)!=0)
// 							{
// 								m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"BPU[%s]����[%d]ֹͣʧ��",\
// 									m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(),node.pid);
// 							}
// 						}
// 					}
// 				}
// 				
// 			}
// 			bpunode = (CXmlNode *)bpunode->getNextSibling();
// 		}
		sprintf(rcvdata.sMsgBuf->sBuffer,"ֹͣBPU�ɹ�");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pDrebApi->SendMsg(rcvdata);
		return;
	}

	virtual void MonitorStartBpu(S_BPC_RSMSG &rcvdata)
	{
		if (rcvdata.sMsgBuf->sDBHead.cZip != 0)
		{
			if (!m_pDrebApi->UnzipBuf(*rcvdata.sMsgBuf))
			{
				sprintf(rcvdata.sMsgBuf->sBuffer,"���ݽ�ѹ������");
				rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
				rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
				rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
				rcvdata.sMsgBuf->sDBHead.cZip = 0;
				m_pDrebApi->SendMsg(rcvdata);
				return ;
			}
		}
		CBF_Xml  xmlpack;
		if (!xmlpack.FromBuffer(rcvdata.sMsgBuf->sBuffer))
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"�������ݷ�xml");
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pDrebApi->SendMsg(rcvdata);
			return ;
		}
		CXmlNode *stopnode = xmlpack.GetNodeByPath("Monitor/StartBpu",false);
		if (stopnode == NULL)
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"xml��[Monitor/StartBpu]�ڵ�");
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pDrebApi->SendMsg(rcvdata);
			return ;
		}
//		int i;
//		int bpuindex=-1;
//		std::string bpugroupname;
//		int bpugnum=0;
//		S_PROCESS_NODE node;
//		bool isfind = false;
//		int k;
//		int connNum=0;
// 		CXmlNode *bpunode = (CXmlNode *)stopnode->GetFirstChild();
// 		while (bpunode != NULL)
// 		{
// 			bpugroupname = bpunode->GetNodeName();
// 			isfind=false;
// 			for (i=0; i< m_pRes->g_vBpuGroupInfo.size() ; i++)
// 			{
// 				if (bpugroupname.compare(m_pRes->g_vBpuGroupInfo[i].g_sBpuGroupName) == 0)
// 				{
// 					isfind = true;
// 					bpuindex = i;
// 					break;
// 				}
// 			}
// 			if (isfind)
// 			{
// 				node.prgname = m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuPrgName;
// 				node.prgpara = m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuPrgPara;
// 				node.prgpath = m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuPrgPath;
// 				node.runpath = m_pRes->g_vBpuGroupInfo[bpuindex].g_sBuRunPath;
// 				node.nodeid = m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName;
// 
// 				connNum=0;
// 				for (k=m_pRes->g_vBpuGroupInfo[bpuindex].g_nBeginSocketIndex ; k < m_pRes->g_vBpuGroupInfo[bpuindex].g_nEndSocketIndex; k++)
// 				{
// 					if (m_pSockMgr->at(k)->m_sock != INVALID_SOCKET)
// 					{
// 						connNum++;
// 					}
// 				}
// 				if (!m_pRes->g_vBpuGroupInfo[bpuindex].g_bIsAutoLoadBu) //�����Զ������Ž�������
// 				{
// 					if (m_pRes->g_vBpuGroupInfo[bpuindex].g_nDispatchMode == 1) //java�汾
// 					{
// 	//					bpugnum = m_pRes->g_vBpuGroupInfo[bpuindex].g_nBpuNum - m_pRes->g_vBpuGroupInfo[bpuindex].g_nStartBpuNum;
// 						bpugnum = m_pRes->g_vBpuGroupInfo[bpuindex].g_nBpuNum - connNum;
// 						if (bpugnum >0)
// 						{
// 							m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"����ԭBPU[%s]�����Ƿ����[%d]",\
// 								m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(),m_pRes->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid);
// 							if (m_pRes->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid > 0) //�鿴�����Ƿ����
// 							{
// 								node.pid = m_pRes->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid;
// 								if (m_pMgrBpu.FindProcess(&node) == 0) //�д˽���
// 								{
// 									m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"ԭBPU[%s]����[%d]�Ѵ��ڣ�ֹͣ",\
// 										m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(),m_pRes->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid);
// 									if (m_pMgrBpu.StopProcess(&node)!=0)
// 									{
// 										m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"BPU[%s]����[%d]ֹͣʧ��",\
// 											m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(),m_pRes->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid);
// 									}
// 								}
// 								m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"BPU[%s]����",\
// 									m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str());
// 								if (m_pMgrBpu.StopProcess(&node)!=0)
// 								{
// 									m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"BPU[%s]����[%d]ֹͣʧ��",\
// 										m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str(),m_pRes->g_vBpuGroupInfo[bpuindex].g_nJavaBpuPid);
// 								}
// 							}
// 						}
// 					}
// 					else
// 					{
// 	//					bpugnum = m_pRes->g_vBpuGroupInfo[bpuindex].g_nBpuNum - m_pRes->g_vBpuGroupInfo[bpuindex].g_nStartBpuNum;
// 						bpugnum = m_pRes->g_vBpuGroupInfo[bpuindex].g_nBpuNum - connNum;
// 						for (int j=0; j<bpugnum;j++)
// 						{
// 							if (m_pMgrBpu.StartProcess(&node)!=0)
// 							{
// 								m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"BPU[%s]����ʧ��",\
// 									m_pRes->g_vBpuGroupInfo[bpuindex].g_sBpuGroupName.c_str());
// 							}
// 						}
// 					}
// 				}
// 			}
// 			bpunode = (CXmlNode *)bpunode->getNextSibling();
// 		}
		sprintf(rcvdata.sMsgBuf->sBuffer,"����BPU�ɹ�");
		rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pDrebApi->SendMsg(rcvdata);
		return;
	}
	
	virtual void Download(S_BPC_RSMSG &rcvdata)
	{
		if (rcvdata.sMsgBuf->sDBHead.cZip != 0)
		{
			if (!m_pDrebApi->UnzipBuf(*rcvdata.sMsgBuf))
			{
				sprintf(rcvdata.sMsgBuf->sBuffer,"���ݽ�ѹ������");
				rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
				rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
				rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
				rcvdata.sMsgBuf->sDBHead.cZip = 0;
				m_pDrebApi->SendMsg(rcvdata);
				return ;
			}
		}
		S_BPC_DUFILE sfile;
		if (rcvdata.sMsgBuf->sDBHead.nLen  < sizeof(S_BPC_DUFILE))
		{
			m_pDrebApi->GetLogPoint()->LogBin(LOG_ERROR,__FILE__,__LINE__,"�������ݷ���������",\
				rcvdata.sMsgBuf->sBuffer,rcvdata.sMsgBuf->sDBHead.nLen);
			sprintf(rcvdata.sMsgBuf->sBuffer,"�������ݷ���������");
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_MONITOR_DOWNLOAD;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pDrebApi->SendMsg(rcvdata);
			return ;
		}
		memcpy(&sfile,rcvdata.sMsgBuf->sBuffer,sizeof(S_BPC_DUFILE));
		struct_stat fstat;
		FSTAT(sfile.sFileName,&fstat);
		if (fstat.st_size <= 0)
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"�ļ�[%s]������",sfile.sFileName);
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_MONITOR_DOWNLOAD;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pDrebApi->SendMsg(rcvdata);
			return ;
		}
		int offset = atol(sfile.sFileOffset);
		if (offset >= fstat.st_size)
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"�ļ�ƫ����[%d]�Ѵ��ڵ����ļ���С",offset);
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_MONITOR_DOWNLOAD;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pDrebApi->SendMsg(rcvdata);
			return ;
		}
		sprintf(sfile.sFileSize,"%011ld",fstat.st_size);
		FILE *fp = fopen(sfile.sFileName,"rb");
		if (fp == NULL)
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"�ļ�[%s]������",sfile.sFileName);
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_MONITOR_DOWNLOAD;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pDrebApi->SendMsg(rcvdata);
			return ;
		}
		fseek(fp,offset,1);
		int getlen = fread(rcvdata.sMsgBuf->sBuffer+sizeof(S_BPC_DUFILE),1,50*1024,fp);
		fclose(fp);
		sprintf(sfile.sFileOffset,"%011ld",getlen + offset);
		if ((getlen + offset) >= fstat.st_size)
		{
			sfile.cEndFlag = '1';
		}
		else
		{
			sfile.cEndFlag = '0';
		}
		memcpy(rcvdata.sMsgBuf->sBuffer,&sfile,sizeof(S_BPC_DUFILE));
		rcvdata.sMsgBuf->sDBHead.nLen = sizeof(S_BPC_DUFILE)+getlen;
		rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
		rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
		rcvdata.sMsgBuf->sDBHead.cZip = 0;
		m_pDrebApi->SendMsg(rcvdata);
		return;

	}

	virtual void Upload(S_BPC_RSMSG &rcvdata)
	{
		if (rcvdata.sMsgBuf->sDBHead.cZip != 0)
		{
			if (!m_pDrebApi->UnzipBuf(*rcvdata.sMsgBuf))
			{
				sprintf(rcvdata.sMsgBuf->sBuffer,"���ݽ�ѹ������");
				rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
				rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_BPU_MONITORDATA;
				rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
				rcvdata.sMsgBuf->sDBHead.cZip = 0;
				m_pDrebApi->SendMsg(rcvdata);
				return ;
			}
		}
		S_BPC_DUFILE sfile;
		if (rcvdata.sMsgBuf->sDBHead.nLen  <= sizeof(S_BPC_DUFILE))
		{
			m_pDrebApi->GetLogPoint()->LogBin(LOG_ERROR,__FILE__,__LINE__,"�������ݷ��ϴ�����",\
				rcvdata.sMsgBuf->sBuffer,rcvdata.sMsgBuf->sDBHead.nLen);
			sprintf(rcvdata.sMsgBuf->sBuffer,"�������ݷ��ϴ�����");
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_MONITOR_UPLOAD;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pDrebApi->SendMsg(rcvdata);
			return ;
		}
		memcpy(&sfile,rcvdata.sMsgBuf->sBuffer,sizeof(S_BPC_DUFILE));
		m_pDrebApi->GetLogPoint()->LogBin(LOG_DEBUG,__FILE__,__LINE__,"�ϴ��ļ�",rcvdata.sMsgBuf->sBuffer,rcvdata.sMsgBuf->sDBHead.nLen );
		FILE *fp=NULL;
		if (sfile.cBeginFlag == '1')
		{
			char filenamepath[301];
			sprintf(filenamepath,"%s",sfile.sFileName);
			int j= strlen(filenamepath)-1;
			for (;j>0;j--)
			{
				if ((filenamepath[j] == '/') ||  (filenamepath[j] == '\\'))
				{
					filenamepath[j] =0;
					CBF_Tools::MakeDir(filenamepath);
					break;
				}
			}
			fp = fopen(sfile.sFileName,"wb");
		}
		else
		{
			fp = fopen(sfile.sFileName,"ab");
		}
		if (fp == NULL)
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"�ļ�[%s]�򿪳���",sfile.sFileName);
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_MONITOR_UPLOAD;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pDrebApi->SendMsg(rcvdata);
			return ;
		}
		fwrite(rcvdata.sMsgBuf->sBuffer+sizeof(S_BPC_DUFILE),1,rcvdata.sMsgBuf->sDBHead.nLen-sizeof(S_BPC_DUFILE),fp);
		fclose(fp);
		if (sfile.cEndFlag == '1')
		{
			struct_stat fstat;
			FSTAT(sfile.sFileName,&fstat);
			if (atol(sfile.sFileSize) != fstat.st_size)
			{
				sprintf(rcvdata.sMsgBuf->sBuffer,"�ϴ��ļ�[%s]ʧ�ܣ���С[%d]������[%s]",sfile.sFileName,fstat.st_size,sfile.sFileSize);
				rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
				rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = ERR_MONITOR_UPLOAD;
				rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
				rcvdata.sMsgBuf->sDBHead.cZip = 0;
				m_pDrebApi->SendMsg(rcvdata);
				return ;
			}
			sprintf(rcvdata.sMsgBuf->sBuffer,"�ϴ��ļ�[%s]�ɹ�",sfile.sFileName);
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pDrebApi->SendMsg(rcvdata);
			return ;
		}
		else
		{
			sprintf(rcvdata.sMsgBuf->sBuffer,"д���ļ�[%s]�ɹ�",sfile.sFileName);
			rcvdata.sMsgBuf->sDBHead.nLen = strlen(rcvdata.sMsgBuf->sBuffer);
			rcvdata.sMsgBuf->sDBHead.a_Ainfo.a_nRetCode = SUCCESS;
			rcvdata.sMsgBuf->sDBHead.cRaflag = 1;
			rcvdata.sMsgBuf->sDBHead.cZip = 0;
			m_pDrebApi->SendMsg(rcvdata);
			return ;
		}
	}

};

#endif // !defined(AFX_DREBMSGPROCBASE_H__EC38AEB7_E135_4E09_92E4_4D13D9D38982__INCLUDED_)
