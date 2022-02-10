// BPCLink.cpp: implementation of the CBPCLink class.
//
//////////////////////////////////////////////////////////////////////

#include "BPCLink.h"
#include "BF_Zip.h"
#include "BF_Des.h"
#include "dreberrcode.h"
#include "BF_Tools.h"

#define  DESENCRYPTKEY    "rd402wms"
#define  DESENCRYPTKEY3   "wms13711songfree"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



CBPCLink::CBPCLink()
{
	m_pConfXml = NULL;
	m_pLog = NULL;
	m_nConnNum = 0;
	
	memset(m_sRcvBuffer,0,sizeof(m_sRcvBuffer));
	m_nRcvBufLen = 0;
	
	memset(m_sSendBuffer,0,sizeof(m_sSendBuffer));
	m_nSendBufLen = 0;

	bzero(&m_pHeadData,sizeof(BPCCOMMSTRU));

	m_nConntime=0;              //���ӽ���ʱ��
	m_nReadtime=0;              //�ϴζ�����ʱ��
	m_nSendtime=0;              //�ϴη�������ʱ�� 
	
	m_nHeartTime =5;
	m_nDisconnectTime = 600;

	m_bIsConnect = false;
	m_nBpcHeadLen = BPCHEADLEN+DREBHEADLEN;
	m_bIsFree = false;
	m_pDate.Update();
	m_nUseIpc = 0;
	m_nBpcPort=0;

	CBF_Tools::MakeDir("data");

	m_bIsBpuCall = false;//�Ƿ�BPU����(���̿��Ƶ���)
	m_listCall=NULL;

	bzero(g_sCurPath,sizeof(g_sCurPath));
	bzero(g_sModuleName,sizeof(g_sModuleName));
	bzero(g_sModulePath,sizeof(g_sModulePath));
}

CBPCLink::~CBPCLink()
{
	m_tcpSocket.Close();
	m_pConfXml = NULL;
	m_pLog = NULL;

}
std::string CBPCLink::GetBpcMsgType(int msgtype)
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
	default:
		return "";
	}
}
std::string CBPCLink::GetDrebCmdType(int cmdtype)
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

int CBPCLink::Run()
{
	int ret=-1;
	while (!m_bIsConnect)
	{
		ret = ConnectBPC();
		if (ret != 0 )
		{
			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"Run ����BPCʧ�� ��������[%d]",m_nConnNum);
			//����ʧ�ܴ�������5�Σ��˳�bpu
			if (m_nConnNum >5 )
			{
				m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"Run ���Ӵ���[%d]����5�Σ��˳�BPU",m_nConnNum);
				return 1;
			}
			//ÿ���������2��
			SLEEP_SECONDS(2);
		}
		break;
	}
	
	if (IsSocketEvent(1)) //1�����Ƿ����¼�
	{
		//����������m_sRcvBuffer
		ret = OnReceive();
		if (ret <=0)
		{
			//�ж��¼���δ�յ����ݣ��϶����ӳ������ˣ��ر�֮
			OnClose(__FILE__,__LINE__,"�ж��¼���δ�յ����ݣ��϶����ӳ������ˣ��ر�֮");
			return 0;
		}
		while (m_nRcvBufLen >= m_nBpcHeadLen)
		{
			//���������ݷ���m_pHeadData��
			ret = GetRcvData(m_pHeadData);
			if (ret > 0)
			{
				m_bIsFree = false;
				if (m_pHeadData.sBpcHead.cMsgType == MSG_REQ || m_pHeadData.sBpcHead.cMsgType == MSG_BPCCALL)
				{
					//m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���յ���DREB���� %s",GetDrebCmdType(m_pHeadData.sDBHead.cCmd).c_str());
					//����һ������
					//m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"������[%d]",m_pHeadData.sDBHead.d_Dinfo.d_nServiceNo);
					if (m_pHeadData.sDBHead.cRaflag == 1)
					{
						//Ӧ�����ݣ����ｫ����
						m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���յ���DREB����[%s] ������[%d]",GetDrebCmdType(m_pHeadData.sDBHead.cCmd).c_str(),m_pHeadData.sDBHead.d_Dinfo.d_nServiceNo);
						continue;
					}
					m_pCallBu.Run(&m_pHeadData);//Ӧ����callbu����ansData
					//������״̬Ϊæʱ��˵��ҵ��û����Ӧ���һ����(��ʱ���ܶ��Ӧ�����Щ����)
					OnFree();
				}
				else
				{
//					m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"���յ���BPC��Ϣ %s",GetBpcMsgType(m_pHeadData.sBpcHead.cMsgType).c_str());
				}

			}
			else
			{
				//�˳�while�����ݲ�ȫ���´�����
				break;	
			}
		}
	}
	else
	{
//		OnNoEvent();//���¼�,����,ɾ����ʱ�ļ�
		OnNoReadEvent();//�޶��¼����Ƿ�Ͽ�
	}
	
	return 0;
}

std::string CBPCLink::GetBpcIp()
{
	string sip;
	if (m_pConfXml == NULL)
	{
		return "";
	}
	if (m_pConfXml->GetNodeValueByPath("package/head/PU����/BPCIP",false,sip) == NULL)
	{
		return "";
	}
	return sip;
}

int CBPCLink::GetBpcPort()
{
	int nport;
	if (m_pConfXml == NULL)
	{
		return 0;
	}
	if (m_pConfXml->GetNodeValueByPath("package/head/PU����/BPCPORT",false,nport) == NULL)
	{
		return 0;
	}
	return nport;
}

bool CBPCLink::IsSocketEvent(int sec)
{
	struct timeval tv;
	int result;
	if (m_sock == INVALID_SOCKET)
	{
		return false;
	}
	FD_ZERO(&m_rset);
	FD_ZERO(&m_wset);
#if defined(_WINDOWS)
	FD_ZERO(&m_eset);
#endif
	tv.tv_sec = sec;
	tv.tv_usec = 0;
	FD_SET(m_sock, &m_rset);//���뵽��select����
#if defined(_WINDOWS)
	result = select(m_sock+1, &m_rset, &m_wset, &m_eset, &tv);
#else
	result = select(m_sock+1, &m_rset, &m_wset, NULL, &tv);
#endif
	if (result == 0)
	{
		return false;
	}
	else
	{
		result = FD_ISSET(m_sock,&m_rset);
		if (result != 0) //�ж��¼�
		{
			return true;
		}
		return false;
	}
}


int CBPCLink::ConnectBPC()
{
	m_tcpSocket.Close();
#ifdef UNIX
	if (m_nUseIpc == 1)
	{
		m_tcpSocket.Create(AF_UNIX,true);
	}
	else
	{
		m_tcpSocket.Create(AF_INET,true);
	}
#else
	m_tcpSocket.Create(AF_INET,true);
#endif	
	m_nConntime = time(NULL);
	if (!m_tcpSocket.ConnectServer((char *)m_sBpcIp.c_str(),m_nBpcPort))
//	if (!m_tcpSocket.ConnectServer((char *)GetBpcIp().c_str(),GetBpcPort()))
	{
		m_nConnNum++;
		m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"����BPC %s %d ʧ��",(char *)m_sBpcIp.c_str(),m_nBpcPort);
		m_sock = INVALID_SOCKET;
		m_tcpSocket.Close();
		return -1;
	}
	//������ģʽ
	m_tcpSocket.SetBlockFlag(false);
	m_sock = m_tcpSocket.GetSocket();

	//ע�Ṧ��
	int ret;
	BPCCOMMSTRU data;
	bzero(&data,sizeof(BPCCOMMSTRU));
	data.sBpcHead.nBpcLen = DREBHEADLEN + m_sBpuGroupName.length();
	data.sBpcHead.cMsgType = MSG_BPUISREG;

	data.sDBHead.nLen = 0;
	data.sDBHead.cRaflag = 0;
	data.sDBHead.cNextFlag = 0;
	data.sDBHead.cZip = 0;

	strcpy(data.sBuffer,m_sBpuGroupName.c_str()); //�����Ƹ��ں���
	data.sDBHead.nLen = m_sBpuGroupName.length();
	data.sBpcHead.nRtime = GetPid();//����ID����nRtime��
	//�����Ƿ��ע����Ϣ
	if (OnSend(&data) <0 )
	{
		m_nConnNum++;
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"����MSG_BPUISREGʧ��"); 
		OnClose(__FILE__,__LINE__,"����MSG_BPUISREGʧ��");
		return -1;
	}
	while (true)
	{
		//�Ƿ�ɶ�
		if (!IsSocketEvent(5)) 
		{
			m_nConnNum++;
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"δ�յ�BPC�Ƿ��ע����Ϣ����"); 
			OnClose(__FILE__,__LINE__,"δ�յ�BPC�Ƿ��ע����Ϣ����");
			return -1;
		}
		ret = OnReceive();
		if (ret <=0)
		{
			m_nConnNum++;
			OnClose(__FILE__,__LINE__,"Զ�˹رգ��������ݲ�����");
			return -1;
		}
		ret = GetRcvData(data);
		if (ret <1)
		{
			m_nConnNum++;
			if (m_nConnNum>5)
			{
				OnClose(__FILE__,__LINE__,"�������ݲ�����");
				return -1;
			}
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"����ע�᷵�����ݲ�����");
			continue;
		}
		if (ret > 0) //�յ�����������
		{
			data.sBuffer[data.sDBHead.nLen]=0;
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�յ�BPCע��Ӧ��[%s] ",data.sBuffer);
			if (data.sDBHead.a_Ainfo.a_nRetCode != SUCCESS )
			{
				if (data.sDBHead.a_Ainfo.a_nRetCode == ERR_BPC_NOREGISTER)
				{
					m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"ע��BpuGroup:%s ����ע��,BPC������:%d",m_sBpuGroupName.c_str(),data.sDBHead.a_Ainfo.a_nRetCode); 
					break;
				}
				m_nConnNum++;
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ע��BpuGroup:%s ʧ��,BPC������:%d",m_sBpuGroupName.c_str(),data.sDBHead.a_Ainfo.a_nRetCode); 
				OnClose(__FILE__,__LINE__,"ע��BpuGroupʧ��");
				return -1;
			}
			//�Ƿ��ע�᷵��0��ʾ��ע�ᣬ1Ϊ��ע�ᣬ2Ϊbpu�鲻��
			ret = atol(data.sBuffer);
			if (ret == 0)
			{
				m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"��ʼע�ύ�� ");
				//ע��
				if (!RegisterBuGroup())
				{
					m_nConnNum++;
					m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ע��BpuGroup:%s ʧ��",m_sBpuGroupName.c_str()); 
					OnClose(__FILE__,__LINE__,"ע��BpuGroupʧ��");
					return -1;
				}
			}
			else
			{
				if (ret == 2)
				{
					m_nConnNum++;
//					printf("BpuGroup:%s ��bpc�����в�����\n",m_sBpuGroupName.c_str());
					m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"BpuGroup:%s ��bpc�����в�����",m_sBpuGroupName.c_str()); 
					OnClose(__FILE__,__LINE__,"BpuGroup��BPC�����в�����");
					return -1;
				}
				else if (ret == 3)
				{
					m_nConnNum++;
//					printf("BpuGroup:%s ��������\n",m_sBpuGroupName.c_str());
					m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"BpuGroup:%s ��������",m_sBpuGroupName.c_str()); 
					OnClose(__FILE__,__LINE__,"BpuGroup��������");
					return -1;
				}
				else
				{
					m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"BpuGroup:%s ��ע��",m_sBpuGroupName.c_str());
				}
			}
			break;
		}
	}
	if (OnFree())
	{
		m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"����BPC %s %d �ɹ�",(char *)(char *)GetBpcIp().c_str(),GetBpcPort());
	}
	else
	{
		OnClose(__FILE__,__LINE__,"ע��ʧ��");
		m_nConnNum++;
		return -1;
	}
	m_bIsConnect = true;
	m_nConnNum = 0;
	return 0;
}

// ������: Init
// ���  : ������ 2010-7-6 17:18:12
// ����  : bool 
// ����  : const char *filepathname
// ����  : ��ʼ��BPc����
bool CBPCLink::InitLink(const char *confile)
{
	m_pHeadCheck.SetCommEndian(true);//ʹ��������

	m_sConfigXmlFile = confile;
	m_pCallBu.SetPara(this);

	if (!m_pCallBu.BuInit(m_sConfigXmlFile.c_str()))
	{
		printf("%s\n",m_pCallBu.GetMsg().c_str());
		return false;
	}
	
	m_pConfXml = m_pCallBu.GetXmlConfig();
	m_pLog = m_pCallBu.GetLogClass();
	
	//��־���� ��־Ŀ¼ ��־�ļ�
	int id;
	char tmpchar[400];
	m_pLog->GetLogPara(m_nLogLevel,m_sLogPath,m_sLogFile,id);
	m_sLogPath = std::string(g_sCurPath);
	m_pLogFile.SetLogPara(m_nLogLevel,m_sLogPath.c_str(),"dreb.dat","dreberr.dat","bpu.dat","bpuerr.dat");
	
	if (m_pConfXml->GetNodeValueByPath("package/head/bpc/bpugroup",false,m_sBpuGroupName) == NULL)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"package/head/bpc/bpugroupδ����");
		return false;
	}
	if (m_sBpuGroupName.length()<1)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"package/head/bpc/bpugroup���ô���");
		return false;
	}
	
	if (m_pConfXml->GetNodeValueByPath("package/head/bpc/disconnecttime",false,m_nDisconnectTime) == NULL)
	{
		m_nDisconnectTime = 60000;
		m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"[disconnecttime]δ���ã�Ĭ��Ϊ60000��");
	}
	if (m_pConfXml->GetNodeValueByPath("package/head/bpc/hearttime",false,m_nHeartTime) == NULL)
	{
		m_nHeartTime = 1;
		m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"[hearttime]δ���ã�Ĭ��Ϊ1��");
	}
	
	if (m_pConfXml->GetNodeAttribute("package/head/bpc/compress",false,"datalen",false,m_nZipLen) == NULL)
	{
		m_nZipLen = 200;
		m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"Ĭ�ϼ���ѹ�����ݳ���Ϊ200");
	}
	if (m_pConfXml->GetNodeAttribute("package/head/bpc/compress",false,"flag",false,m_nZipFlag) == NULL)
	{
		m_nZipFlag = 0;
		m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"Ĭ�ϼ���ѹ����־Ϊ0������");
	}
	if (m_nZipFlag <0 || m_nZipFlag >4 )
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"����ѹ����־���ò���%d",m_nZipFlag);
		return false;
	}
	
	if (m_pConfXml->GetNodeValueByPath("package/head/bpc/useipc",false,m_nUseIpc) == NULL)
	{
		m_nUseIpc = 0;
		m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"[useipc]δ���ã�Ĭ��Ϊ��ʹ��");
	}

	if (m_pConfXml->GetNodeValueByPath("package/head/bpc/bpcip",false,m_sBpcIp,false) == NULL)
	{
		m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"[bpcip]δ���ã�Ĭ��Ϊ��ʹ��");
		return false;
	}
	if (m_pConfXml->GetNodeValueByPath("package/head/bpc/bpcport",false,m_nBpcPort) == NULL)
	{
		m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"[bpcport]δ���ã�Ĭ��Ϊ��ʹ��");
		return false;
	}

// 	int ret = ConnectBPC();
// 	if (ret !=0 )
// 	{
// 		printf("����BPC %s %d �����������Ӳ��ϻ�BPUע����������鿴��־\n",(char *)(char *)GetBpcIp().c_str(),GetBpcPort());
// 		return false;
// 	}
	return true;
}

/// ������: SetGPara1
/// ���  : ������ 2008-12-30 9:42:48
/// ����  : virtual bool 
/// ����  : bool setflag=true
/// ����  : �ӿ��麯��������Ҫʵ��.����ȫ�ֱ���1,���������򷵻�false
bool CBPCLink::SetGPara1(bool setflag)
{
	int ret;
	while (!m_bIsConnect)
	{
		ret = ConnectBPC();
		if (ret != 0 )
		{
			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"����BPCʧ�� ��������[%d]",m_nConnNum);
			//����ʧ�ܴ�������5�Σ��˳�bpu
			if (m_nConnNum >5 )
			{
				m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"���ӳ���[%d]�Σ��˳�BPU");
				return 1;
			}
			//ÿ���������1��
			SLEEP_SECONDS(1);
		}
		break;
	}
	BPCCOMMSTRU data;
	bzero(&data,sizeof(BPCCOMMSTRU));
	data.sBpcHead.cMsgType = MSG_SETGPARA1;
	data.sBpcHead.nBpcLen = DREBHEADLEN;
	data.sDBHead.nLen = 0;
	data.sDBHead.cRaflag = 0;
	data.sDBHead.cNextFlag = 0;
	data.sDBHead.cZip = 0;
	
	if (OnSend(&data)<0)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"����MSG_SETGPARA1ʧ��"); 
		OnClose(__FILE__,__LINE__,"����MSG_SETGPARA1ʧ��");
		return false;
	}
	while (true)
	{
		//�Ƿ�ɶ�
		if (!IsSocketEvent(5)) 
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"δ�յ�MSG_SETGPARA1��Ϣ����"); 
			OnClose(__FILE__,__LINE__,"δ�յ�MSG_SETGPARA1��Ϣ����");
			return false;
		}
		ret = OnReceive();
		if (ret <=0)
		{
			OnClose(__FILE__,__LINE__,"Զ�˹رգ��������ݲ�����");
			return false;
		}
		ret = GetRcvData(data);
		if (ret > 0) //�յ�����������
		{
			data.sBuffer[data.sDBHead.nLen]=0;
			if (data.sDBHead.a_Ainfo.a_nRetCode != SUCCESS )
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"BpuGroup:%s  MSG_SETGPARA1 ʧ��,BPC������:%d",m_sBpuGroupName.c_str(),data.sDBHead.a_Ainfo.a_nRetCode); 
				return false;
			}
			if (atoi(data.sBuffer) == 0)
			{
				return false;
			}
			else
			{
				return true;
			}
		}
	}
	return false;
}


/// ������: isSetGPara2
/// ���  : ������ 2008-12-30 9:36:10
/// ����  : virtual bool 
/// ����  : bool setflag=true
/// ����  : ����ȫ�ֱ���2,���������򷵻�false
bool CBPCLink::SetGPara2(bool setflag)
{
	int ret;
	while (!m_bIsConnect)
	{
		ret = ConnectBPC();
		if (ret != 0 )
		{
			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"����BPCʧ�� ��������[%d]",m_nConnNum);
			//����ʧ�ܴ�������5�Σ��˳�bpu
			if (m_nConnNum >5 )
			{
				m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"���ӳ���[%d]�Σ��˳�BPU");
				return 1;
			}
			//ÿ���������1��
			SLEEP_SECONDS(1);
		}
		break;
	}
	BPCCOMMSTRU data;
	bzero(&data,sizeof(BPCCOMMSTRU));
	data.sBpcHead.cMsgType = MSG_SETGPARA2;
	data.sBpcHead.nBpcLen = DREBHEADLEN;
	data.sDBHead.nLen = 0;
	data.sDBHead.cRaflag = 0;
	data.sDBHead.cNextFlag = 0;
	data.sDBHead.cZip = 0;
	
	if (OnSend(&data)<0)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"����MSG_SETGPARA2ʧ��"); 
		OnClose(__FILE__,__LINE__,"����MSG_SETGPARA2ʧ��");
		return false;
	}
	while (true)
	{
		//�Ƿ�ɶ�
		if (!IsSocketEvent(5)) 
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"δ�յ�MSG_SETGPARA2��Ϣ����"); 
			OnClose(__FILE__,__LINE__,"δ�յ�MSG_SETGPARA2��Ϣ����");
			return false;
		}
		ret = OnReceive();
		if (ret <=0)
		{
			OnClose(__FILE__,__LINE__,"Զ�˹رգ��������ݲ�����");
			return false;
		}
		ret = GetRcvData(data);
		if (ret > 0) //�յ�����������
		{
			data.sBuffer[data.sDBHead.nLen]=0;
			if (data.sDBHead.a_Ainfo.a_nRetCode != SUCCESS )
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"BpuGroup:%s  MSG_SETGPARA2 ʧ��,BPC������:%d",m_sBpuGroupName.c_str(),data.sDBHead.a_Ainfo.a_nRetCode); 
				return false;
			}
			if (atoi(data.sBuffer) == 0)
			{
				return false;
			}
			else
			{
				return true;
			}
		}
	}
	return false;

}


/// ������: isSetGPara3
/// ���  : ������ 2008-12-30 9:36:13
/// ����  : virtual bool 
/// ����  : bool setflag=true
/// ����  : ����ȫ�ֱ���3,���������򷵻�false
bool CBPCLink::SetGPara3(bool setflag)
{
	int ret;
	while (!m_bIsConnect)
	{
		ret = ConnectBPC();
		if (ret != 0 )
		{
			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"����BPCʧ�� ��������[%d]",m_nConnNum);
			//����ʧ�ܴ�������5�Σ��˳�bpu
			if (m_nConnNum >5 )
			{
				m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"���ӳ���[%d]�Σ��˳�BPU");
				return 1;
			}
			//ÿ���������1��
			SLEEP(1000);
		}
		break;
	}
	BPCCOMMSTRU data;
	bzero(&data,sizeof(BPCCOMMSTRU));
	data.sBpcHead.cMsgType = MSG_SETGPARA3;
	data.sBpcHead.nBpcLen = DREBHEADLEN;
	data.sDBHead.nLen = 0;
	data.sDBHead.cRaflag = 0;
	data.sDBHead.cNextFlag = 0;
	data.sDBHead.cZip = 0;
	
	if (OnSend(&data)<0)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"����MSG_SETGPARA3ʧ��"); 
		OnClose(__FILE__,__LINE__,"����MSG_SETGPARA3ʧ��");
		return false;
	}
	while (true)
	{
		//�Ƿ�ɶ�
		if (!IsSocketEvent(5)) 
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"δ�յ�MSG_SETGPARA3��Ϣ����"); 
			OnClose(__FILE__,__LINE__,"δ�յ�MSG_SETGPARA3��Ϣ����");
			return false;
		}
		ret = OnReceive();
		if (ret <=0)
		{
			OnClose(__FILE__,__LINE__,"Զ�˹رգ��������ݲ�����");
			return false;
		}
		ret = GetRcvData(data);
		if (ret > 0) //�յ�����������
		{
			data.sBuffer[data.sDBHead.nLen]=0;
			if (data.sDBHead.a_Ainfo.a_nRetCode != SUCCESS )
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"BpuGroup:%s MSG_SETGPARA3 ʧ��,BPC������:%d",m_sBpuGroupName.c_str(),data.sDBHead.a_Ainfo.a_nRetCode); 
				return false;
			}
			if (atoi(data.sBuffer) == 0)
			{
				return false;
			}
			else
			{
				return true;
			}
		}
	}
	return false;

}

/// ������: GetThreadShareSerial
/// ���  : ������ 2008-12-30 9:36:13
/// ����  : virtual long 
/// ����  : ȡ��Ψһ����ˮ��
long CBPCLink::GetThreadShareSerial()
{
	int ret;
	while (!m_bIsConnect)
	{
		ret = ConnectBPC();
		if (ret != 0 )
		{
			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"����BPCʧ�� ��������[%d]",m_nConnNum);
			//����ʧ�ܴ�������5�Σ��˳�bpu
			if (m_nConnNum >5 )
			{
				m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"���ӳ���[%d]�Σ��˳�BPU");
				return 1;
			}
			//ÿ���������1��
			SLEEP(1000);
		}
		break;
	}
	BPCCOMMSTRU data;
	bzero(&data,sizeof(BPCCOMMSTRU));
	data.sBpcHead.cMsgType = MSG_GETSERIAL;
	data.sBpcHead.nBpcLen = DREBHEADLEN;
	data.sDBHead.nLen = 0;
	data.sDBHead.cRaflag = 0;
	data.sDBHead.cNextFlag = 0;
	data.sDBHead.cZip = 0;

	if (OnSend(&data)<0)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"����MSG_GETSERIALʧ��"); 
		OnClose(__FILE__,__LINE__,"����MSG_GETSERIALʧ��");
		return -1;
	}
	while (true)
	{
		//�Ƿ�ɶ�
		if (!IsSocketEvent(5)) 
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"δ�յ�MSG_GETSERIAL��Ϣ����"); 
			OnClose(__FILE__,__LINE__,"δ�յ�MSG_GETSERIAL��Ϣ����");
			return -1;
		}
		ret = OnReceive();
		if (ret <=0)
		{
			OnClose(__FILE__,__LINE__,"Զ�˹رգ��������ݲ�����");
			return ret;
		}
		ret = GetRcvData(data);
		if (ret > 0) //�յ�����������
		{
			data.sBuffer[data.sDBHead.nLen]=0;
			if (data.sDBHead.a_Ainfo.a_nRetCode != SUCCESS )
			{
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ע��BpuGroup:%s ʧ��,BPC������:%d",m_sBpuGroupName.c_str(),data.sDBHead.a_Ainfo.a_nRetCode); 
				return -1;
			}
			return atol(data.sBuffer);
		}
	}
	return -1;
}


// ������: AnsData
// ���  : ������ 2013-4-23 15:16:43
// ����  : virtual int 
// ����  : PBPCCOMMSTRU data
// ����  : Ӧ������,��BPUҵ�������ԭ�ӵ���
int CBPCLink::AnsData(PBPCCOMMSTRU data) 
{
	if (m_bIsBpuCall)
	{
		m_listCall->push_back(*data);
	}
	else
	{
		int ret;
		while (!m_bIsConnect)
		{
			ret = ConnectBPC();
			if (ret != 0 )
			{
				m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"Ӧ������  ����BPCʧ�� ��������[%d]",m_nConnNum);
				//����ʧ�ܴ�������5�Σ��˳�bpu
				if (m_nConnNum >5 )
				{
					m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"Ӧ������ ����[%d]�Σ��˳�BPU",m_nConnNum);
					return 1;
				}
				//ÿ���������1��
				SLEEP(1000);
			}
			break;
		}
		if (OnSend(data)<0)
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"����Ӧ�����");
			return -1;
		}
	
	}
	
	return 0;
}

// ������: DPTrans
// ���  : ������ 2013-4-23 15:11:48
// ����  : virtual int 
// ����  : PBPCCOMMSTRU calldata
// ����  : ת��������������
int CBPCLink::DPTrans(PBPCCOMMSTRU calldata) 
{
	int ret;
	while (!m_bIsConnect)
	{
		ret = ConnectBPC();
		if (ret != 0 )
		{
			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"DPTrans ����BPCʧ�� ��������[%d]",m_nConnNum);
			//����ʧ�ܴ�������5�Σ��˳�bpu
			if (m_nConnNum >5 )
			{
				m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"DPTrans ���ӳ���[%d]�Σ��˳�BPU",m_nConnNum);
				return 1;
			}
			//ÿ���������1��
			SLEEP(1000);
		}
		break;
	}
	calldata->sBpcHead.cMsgType = MSG_TRANS;
	calldata->sDBHead.cRaflag = 0;
	//ת������ķ�����д���
	if (OnSend(calldata)<0)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"����DPTransʧ��"); 
		OnClose(__FILE__,__LINE__,"����DPTransʧ��");
		return -1;
	}
	//���ǿ��еģ����Դ��������ҵ����
	OnFree();
	return 0;
}

// ������: BpuCall
// ���  : ������ 2013-4-23 15:09:13
// ����  : virtual int -1ʧ�� 0�ɹ�
// ����  : PBPCCOMMSTRU calldata
// ����  : LIST_BPC_RSMSG &ansdata
// ����  : ����BPU�ڵĹ��ܣ������̿���ʹ��
int CBPCLink::BpuCall(PBPCCOMMSTRU calldata,LIST_BPC_RSMSG &ansdata)
{
	m_bIsBpuCall = true;
	m_listCall = &ansdata;

	calldata->sDBHead.cRaflag = 0;
	//����һ������
	m_pCallBu.Run(calldata);

	m_bIsBpuCall = false;
	m_listCall = NULL;
	if (ansdata.size()<1)
	{
		m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"BpuCall�����޷�������");
		return -1;
	}
	return 0;
}

// ������: BpcCall
// ���  : ������ 2013-4-23 15:08:18
// ����  : virtual int 
// ����  : PBPCCOMMSTRU calldata
// ����  : LIST_BPC_RSMSG &ansdata
// ����  : ����bpc����
int CBPCLink::BpcCall(PBPCCOMMSTRU calldata,LIST_BPC_RSMSG &ansdata,unsigned int stimeout)
{
	int ret;
	while (!m_bIsConnect)
	{
		ret = ConnectBPC();
		if (ret != 0 )
		{
			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"BpcCall ����BPCʧ�� ��������[%d]",m_nConnNum);
			//����ʧ�ܴ�������5�Σ��˳�bpu
			if (m_nConnNum >5 )
			{
				m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"BpcCall ���ӳ���[%d]�Σ��˳�BPU",m_nConnNum);
				return 1;
			}
			//ÿ���������1��
			SLEEP(1000);
		}
		break;
	}
	calldata->sBpcHead.cMsgType = MSG_BPCCALL;
	calldata->sDBHead.cRaflag = 0;
	if (OnSend(calldata)<0)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"����BpcCallʧ��"); 
		OnClose(__FILE__,__LINE__,"����BpcCallʧ��");
		return -1;
	}
	ret = GetAns(calldata->sDBHead.cCmd,ansdata,stimeout);
	if (ret <0)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ȡBpcCallӦ�����");
		return -1;
	}
	return 0;
}

// ������: ExtCall
// ���  : ������ 2013-4-23 15:01:25
// ����  : virtual int 
// ����  : PBPCCOMMSTRU calldata
// ����  : vector<S_BPC_RSMSG> &ansdata
// ����  : unsigned int stimeout �����ʱʱ�� ��
// ����  : ���BPC
int CBPCLink::ExtCall(PBPCCOMMSTRU calldata,LIST_BPC_RSMSG &ansdata,unsigned int stimeout)
{
	int ret;
	while (!m_bIsConnect)
	{
		ret = ConnectBPC();
		if (ret != 0 )
		{
			m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"ExtCall ����BPCʧ�� ��������[%d]",m_nConnNum);
			//����ʧ�ܴ�������5�Σ��˳�bpu
			if (m_nConnNum >5 )
			{
				m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"ExtCall ���ӳ���[%d]�Σ��˳�BPU",m_nConnNum);
				return 1;
			}
			//ÿ���������1��
			SLEEP(1000);
		}
		break;
	}
	calldata->sBpcHead.cMsgType = MSG_EXTCALL;
	calldata->sDBHead.cRaflag = 0;
	if (OnSend(calldata)<0)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"����ExtCallʧ��"); 
		OnClose(__FILE__,__LINE__,"����ExtCallʧ��");
		return -1;
	}
	ret = GetAns(calldata->sDBHead.cCmd,ansdata,stimeout);
	if (ret <0)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ȡExtCallӦ�����");
		return -1;
	}
	return 0;
}



// ������: GetRcvData
// ���  : ������ 2013-4-23 14:57:54
// ����  : virtual int 1������������
// ����  : BPCCOMMSTRU &data
// ����  : �첽���պ�ȡ��������
int CBPCLink::GetRcvData(BPCCOMMSTRU &data)
{
	if (m_nRcvBufLen >= m_nBpcHeadLen)
	{
		//��bpchead��drebhead���ƹ�ȥ
		memcpy(&data,m_sRcvBuffer,m_nBpcHeadLen);
		//�ֽ�����,ת�ɱ����ֽ���
		m_pHeadCheck.Endian2LocalHost(&data);
		if (data.sDBHead.nLen>0)
		{
			if (m_nRcvBufLen < m_nBpcHeadLen+data.sDBHead.nLen)
			{
				//δ��ȫ
				return -1;
			}
			memcpy(data.sBuffer,m_sRcvBuffer+m_nBpcHeadLen,data.sDBHead.nLen);
		}
		//�ն��ˣ���ԭ�����¸�m_sRcvBuffer
		for (int i=0; i<m_nRcvBufLen-m_nBpcHeadLen-data.sDBHead.nLen; i++)
		{
			m_sRcvBuffer[i]=m_sRcvBuffer[i+m_nBpcHeadLen+data.sDBHead.nLen];
		}
		m_nRcvBufLen =  m_nRcvBufLen - m_nBpcHeadLen - data.sDBHead.nLen;

		if (data.sBpcHead.cMsgType != MSG_FREE && data.sBpcHead.cMsgType != MSG_BPUISREG && data.sBpcHead.cMsgType != MSG_BPUREG) //������Ϣ���ܱȽ϶࣬�Ͳ���д������
		{
			LogFile(LOG_DEBUG+1,false,&data);
		}
		//���
		if (data.sDBHead.cZip >0 && data.sDBHead.cZip < 4)
		{
			if (!UnzipBuf(data))
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"��ѹ�����ݳ���");
				return -2;
			}
		}
		if (data.sBpcHead.cMsgType != MSG_FREE && data.sBpcHead.cMsgType != MSG_BPUISREG && data.sBpcHead.cMsgType != MSG_BPUREG)
		{
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���յ���BPC����Ϣ[%s]  DREB����[%s] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] cZip[%d] ҵ�����ݳ���[%d]",\
					GetBpcMsgType(data.sBpcHead.cMsgType).c_str(),GetDrebCmdType(data.sDBHead.cCmd).c_str(),\
					data.sDBHead.cRaflag,data.sDBHead.d_Dinfo.d_nServiceNo,data.sDBHead.s_Sinfo.s_nNodeId,\
					data.sDBHead.s_Sinfo.s_cNodePrivateId,data.sDBHead.s_Sinfo.s_nDrebSerial,data.sDBHead.cZip,data.sDBHead.nLen);
		}
		else
		{
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���յ���BPC����Ϣ[%s]  RA��־[%d] ҵ�����ݳ���[%d]",\
				GetBpcMsgType(data.sBpcHead.cMsgType).c_str(),data.sDBHead.cRaflag,data.sDBHead.nLen);
		}
		return 1;

	}
	return 0;
}
	
// ������: Receive
// ���  : ������ 2013-4-23 14:47:49
// ����  : virtual int 
// ����  : ��������
int CBPCLink::OnReceive()
{
	int ret;
	m_tcpSocket.AttachSocket(m_sock,"");
	if (m_nRcvBufLen >=0 && m_nRcvBufLen< BPCBUFSIZE)
	{
		//��ͷδ������
		ret = m_tcpSocket.Receive(m_sRcvBuffer+m_nRcvBufLen,BPCBUFSIZE-m_nRcvBufLen);
		if (ret <= 0)
		{
			//OnClose("Զ�˹رգ��������ݲ�����");
			return ret;
		}
		m_nRcvBufLen+=ret;
		m_nReadtime =time(NULL);
		
	}
	return m_nRcvBufLen;
}
	
	
// ������: OnSend
// ���  : ������ 2013-4-23 14:57:08
// ����  : virtual int <0���ӳ�����   =0�Է���������   >0���ͳɹ�
// ����  : PBPCCOMMSTRU data
// ����  : �������ݸ�BPC��FPC
int CBPCLink::OnSend(PBPCCOMMSTRU data)
{
	int ret;
	if (data->sBpcHead.cMsgType != MSG_FREE && data->sBpcHead.cMsgType != MSG_BPUREG && data->sBpcHead.cMsgType!=MSG_BPUISREG)
	{
		if (!ZipBuf(data))
		{
			m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"����ѹ������ʧ��");
			return -1;
		}
	}
	if (data->sBpcHead.cMsgType != MSG_FREE && data->sBpcHead.cMsgType != MSG_BPUISREG && data->sBpcHead.cMsgType != MSG_BPUREG)
	{
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���͸�BPC����Ϣ[%s]  DREB����[%s] RA��־[%d] ������[%d]  ��ʶ[%d %d %d] cZip[%d] ҵ�����ݳ���[%d]",\
			GetBpcMsgType(data->sBpcHead.cMsgType).c_str(),GetDrebCmdType(data->sDBHead.cCmd).c_str(),\
			data->sDBHead.cRaflag,data->sDBHead.d_Dinfo.d_nServiceNo,data->sDBHead.s_Sinfo.s_nNodeId,\
			data->sDBHead.s_Sinfo.s_cNodePrivateId,data->sDBHead.s_Sinfo.s_nDrebSerial,\
			data->sDBHead.cZip,data->sDBHead.nLen);
	}
	else
	{
		m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���͸�BPC����Ϣ[%s] ҵ�����ݳ���[%d]",\
			GetBpcMsgType(data->sBpcHead.cMsgType).c_str(),data->sDBHead.nLen);
	}
	data->sBpcHead.nBpcLen = DREBHEADLEN +data->sDBHead.nLen;
	memcpy(m_sSendBuffer,data,m_nBpcHeadLen+data->sDBHead.nLen);
	m_nSendBufLen = m_nBpcHeadLen+data->sDBHead.nLen;
	PBPCCOMMSTRU dd = (PBPCCOMMSTRU)m_sSendBuffer;
	m_pHeadCheck.Endian2Comm(dd);
	ret = m_tcpSocket.Send(m_sSendBuffer,m_nSendBufLen);
	if (ret != m_nSendBufLen)
	{
		LogFile(LOG_ERROR,true,data);
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"��������ʧ�� ret[%d]!=sendlen[%d] lasterr[%d %s]",\
			ret,m_nSendBufLen,m_tcpSocket.GetErrorNo(),m_tcpSocket.GetErrorMsg().c_str());
		
		//����ʧ��
		if (ret < 0)
		{
			OnClose(__FILE__,__LINE__,"��������ʧ�� ����Ϊ�����쳣");
		}
		else
		{

		}
		return -1;
	}
	m_nSendtime = time(NULL);
	if (data->sBpcHead.cMsgType != MSG_FREE && data->sBpcHead.cMsgType != MSG_BPUREG && data->sBpcHead.cMsgType != MSG_BPUISREG) //������Ϣ���ܱȽ϶࣬�Ͳ���д������
	{
		LogFile(LOG_DEBUG+1,false,data);
	}
	return ret;
}

void CBPCLink::OnNoEvent()
{
	if (time(NULL) - m_nSendtime >= m_nHeartTime)
	{
		//5��û�з�������MSG_FREE����������
		OnFree();
	}
}

void CBPCLink::OnNoReadEvent()
{
	int tmptime;
	if (m_sock != INVALID_SOCKET)
	{
		tmptime = time(NULL) - m_nReadtime;
		if (tmptime > m_nDisconnectTime)//��ʱʱ��û���յ����ݣ��Ͽ�
		{
			OnClose(__FILE__,__LINE__,"���ӳ�ʱ��δ�ã��Ͽ�");
		}
		else if (tmptime > m_nHeartTime)
		{
			//ֱ���ÿ���״̬Ϊ����
			OnFree();
		}
	}
}
void CBPCLink::OnClose(const char *file,int fileline,const char *msg)
{
	m_tcpSocket.Close();
	m_sock = INVALID_SOCKET;
	m_pLog->LogMp(LOG_WARNNING,file,fileline,"%s",msg);
	m_bIsConnect = false;
}

bool CBPCLink::RegisterBuGroup()
{
	int ret;
	vector<S_TBL_FLOWTX> txinfo;
	int nAllTxNum = m_pCallBu.GetAllTxInfo(txinfo);
	if (nAllTxNum<1)
	{
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"�޽���ע��");
		return true;
	}
	//ÿ�����ݰ��������
	int nPackTxNum = BPUDATASIZE/sizeof(S_BPU_FUNC_REG);
	BPCCOMMSTRU data;
	//�м������ݰ�
	int nRegPackNum = (nAllTxNum/nPackTxNum)+1;
	int txnum=0;
	int datalen=0;
	S_BPU_FUNC_REG *bpugroup=NULL;
	S_BPU_FUNCINFO *fn=NULL;
	for (int i=0 ; i<nRegPackNum; i++ )
	{
		memset(&data,0,sizeof(BPCCOMMSTRU));
		data.sBpcHead.cMsgType = MSG_BPUREG;//ע��
		data.sDBHead.cRaflag = 0;
		data.sDBHead.cNextFlag = 0;
		data.sDBHead.cZip = 0;
		bpugroup=(S_BPU_FUNC_REG *)data.sBuffer;
		strcpy(bpugroup->sBpuGroupName,m_sBpuGroupName.c_str());
		fn=(S_BPU_FUNCINFO *)(data.sBuffer+25);
		txnum=0;
		datalen+=25;
		for (int j= i*nPackTxNum ; j<(i+1)*nPackTxNum && j <nAllTxNum ; j++)
		{
			fn->nFuncNo = txinfo[j].nFuncNo;
			m_pHeadCheck.Endian2Comm((unsigned char *)&(fn->nFuncNo),sizeof(fn->nFuncNo));
			fn->cPrio = txinfo[j].cPrio;
			fn->cServiceFlag = txinfo[j].cServiceFlag;
			CBF_Tools::StringCopy(fn->sFuncName,40,txinfo[j].sFuncName.c_str());
			datalen+=sizeof(S_BPU_FUNCINFO);
			txnum++;
			fn++;
		}
		bpugroup->nFuncNum = txnum;
		m_pHeadCheck.Endian2Comm((unsigned char *)&(bpugroup->nFuncNum),sizeof(bpugroup->nFuncNum));
		data.sDBHead.nLen = datalen;
		data.sBpcHead.nBpcLen = DREBHEADLEN+datalen;
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"��ʼע�Ṧ��");
		if (OnSend(&data)<0)
		{
			OnClose(__FILE__,__LINE__,"����ע����Ϣʧ��");
			return false;
		}
	}
	return true;
}

bool CBPCLink::OnFree()
{
	BPCCOMMSTRU data;
	bzero(&data,sizeof(BPCCOMMSTRU));
	data.sBpcHead.nBpcLen = DREBHEADLEN;
	data.sBpcHead.cMsgType = MSG_FREE;
	data.sDBHead.cRaflag = 0;
	data.sDBHead.nLen = 0;
	data.sDBHead.cNextFlag = 0;
	
	if (OnSend(&data)<0)
	{
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"����MSG_FREEʧ��"); 
		m_bIsFree = false;
		return false;
	}
	m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"����MSG_FREE�ɹ�"); 
	m_bIsFree = true;
	m_bIsBpuCall = false;//�Ƿ�BPU����(���̿��Ƶ���)
	m_listCall=NULL;
	return m_bIsFree;
}

int CBPCLink::LogFile(int level, bool flag, PBPCCOMMSTRU data)
{
	m_pLogFile.LogBpc(level,data,flag);
	return 0;
}

int CBPCLink::GetAns(char msgtype, LIST_BPC_RSMSG &ansdata, unsigned int stimeout)
{
	int ret;
	BPCCOMMSTRU data;
	bzero(&data,sizeof(BPCCOMMSTRU));
// #define  CMD_DPCALL      8  //�������߽ڵ�ͬ������ Ҫ�����մ�����ɺ�Ӧ��
// #define  CMD_DPACALL     9  //�������߽ڵ��첽���� Ҫ����յ����������߽ڵ㷢���������ȷ��Ӧ��
// #define  CMD_DPBC        10  //�������߽ڵ�㲥��������Ϣ����ָ���������߽ڵ�������ע��ķ���,Ҫ���������߽ڵ�Ӧ��
// #define  CMD_DPABC       11  //�������߽ڵ�㲥��������Ϣ����ָ���������߽ڵ�������ע��ķ���,��Ҫ���������߽ڵ�Ӧ��
// #define  CMD_DPPUSH      12 //�������߽ڵ����ͣ�����Ӧ��
// #define  CMD_DPPOST      13 //�������߽ڵ�Ͷ�ݣ�Ҫ����յ����������߽ڵ�Ӧ��
	switch (msgtype)
	{
		case CMD_DPCALL:
		case CMD_DPACALL:
			//����Ӧ��
			break;
		case CMD_DPPOST:
		case CMD_DPBC:
			//����ȷ��
			break;
		case CMD_DPABC:
		case CMD_DPPUSH:
			return 0;
		default:
			return 0;
	}
	while (true)
	{
		//�Ƿ�ɶ�
		if (!IsSocketEvent(stimeout)) 
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"��ʱδ�յ���Ϣ����"); 
			OnClose(__FILE__,__LINE__,"��ʱδ�յ���Ϣ����");
			return -1;
		}
		ret = OnReceive();
		if (ret <=0)
		{
			OnClose(__FILE__,__LINE__,"����Ӧ��Զ�˹رգ��������ݲ�����");
			return -1;
		}
		ret = GetRcvData(data);
		if (ret > 0) //�յ�����������
		{
			if (data.sDBHead.cDrebAffirm == 1)
			{
				m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"���յ��Զ˵�DREBȷ��");
				if (data.sDBHead.a_Ainfo.a_nRetCode != SUCCESS )
				{
					m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�Զ�drebȷ�Ϸ�����[%d]",data.sDBHead.a_Ainfo.a_nRetCode); 
					return 1;
				}
				if (msgtype ==  CMD_DPPOST || msgtype == CMD_DPBC)
				{
					return 0;
				}
				//dpacall
				continue;//��������Ӧ��
			}
			else
			{
				if (data.sDBHead.cRaflag == 0)
				{
					m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�����ˣ���Ӧ����ô�����������%d %d %d",\
						data.sDBHead.s_Sinfo.s_nNodeId,data.sDBHead.s_Sinfo.s_cNodePrivateId,data.sDBHead.s_Sinfo.s_nDrebSerial); 
					continue;
				}
				ansdata.push_back(data);
				if (data.sDBHead.cNextFlag == 0 && data.sDBHead.n_Ninfo.n_nNextNo == 0)
				{
					//ֻ��һ��Ӧ��
					return 0;
				}
				else if (data.sDBHead.n_Ninfo.n_nNextNo == ansdata.size())
				{
					//�ܰ�����Ȳŷ���
					return 0;
				}
			}
			
		}
	}
	return -1;
}

bool CBPCLink::UnzipBuf(BPCCOMMSTRU &data)
{
	//0���� 1des������Կ����16 2ѹ�� 3DES���� 4DES���ܲ�ѹ��

	if (data.sDBHead.cZip >0 && data.sDBHead.cZip <=4)
	{
		char zipDataBuf[BPUDATASIZE];
		bzero(zipDataBuf,sizeof(zipDataBuf));
		unsigned int dlen = BPUDATASIZE;
		int ddlen;
		switch (data.sDBHead.cZip)
		{
			case 1:
				ddlen = data.sDBHead.nLen;
				if (m_pDes.UncryptString(data.sBuffer,ddlen,DESENCRYPTKEY)<=0)
				{
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
					return false;
				}
				data.sDBHead.nLen = ddlen;
				data.sDBHead.cZip =0;
				break;
			case 2:	
				memcpy(zipDataBuf,data.sBuffer,data.sDBHead.nLen);
				if (!CBF_Tools::Uncompress((unsigned char *)(data.sBuffer),dlen,(unsigned char *)zipDataBuf,data.sDBHead.nLen))
				{
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"��ѹ������");
					return false;
				}
				data.sDBHead.nLen = dlen;
				data.sDBHead.cZip =0;
				break;
			case 3:
				ddlen = data.sDBHead.nLen;
				if (m_pDes.UncryptString(data.sBuffer,ddlen,DESENCRYPTKEY3)<=0)
				{
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
					return false;
				}
				data.sDBHead.nLen = ddlen;
				data.sDBHead.cZip =0;
				break;
			case 4:
				ddlen = data.sDBHead.nLen;
				if (m_pDes.UncryptString(data.sBuffer,ddlen,DESENCRYPTKEY)<=0)
				{
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
					return false;
				}
				data.sDBHead.nLen = ddlen;
				memcpy(zipDataBuf,data.sBuffer,data.sDBHead.nLen);
				if (!CBF_Tools::Uncompress((unsigned char *)(data.sBuffer),dlen,(unsigned char *)zipDataBuf,data.sDBHead.nLen))
				{
					m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"��ѹ������");
					return false;
				}
				data.sDBHead.nLen = dlen;
				data.sDBHead.cZip =0;
				break;
			default:
				return false;
		}
	}
	
	return true;
}
bool CBPCLink::ZipBuf(PBPCCOMMSTRU data)
{
	//0���� 1des������Կ����16 2ѹ�� 3DES���� 4DES���ܲ�ѹ��
	if (data->sDBHead.cZip != 0 || data->sDBHead.nLen<m_nZipLen)
	{
		return true;
	}
	char zipDataBuf[BPUDATASIZE];
	bzero(zipDataBuf,sizeof(zipDataBuf));
	unsigned int dlen = BPUDATASIZE;
	int ddlen;
	switch (m_nZipFlag)
	{
		case 1:
			ddlen = data->sDBHead.nLen;
			if (m_pDes.EncryptString(data->sBuffer,ddlen,DESENCRYPTKEY)<=0)
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
				return false;
			}
			data->sDBHead.nLen = ddlen;
			data->sDBHead.cZip =1;
			break;
		case 2:	
			memcpy(zipDataBuf,data->sBuffer,data->sDBHead.nLen);
			if (!CBF_Tools::Compress((unsigned char *)(data->sBuffer),dlen,(unsigned char *)(zipDataBuf),data->sDBHead.nLen))
			{
				m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"����ѹ��ʧ��");
				memcpy(data->sBuffer,zipDataBuf,data->sDBHead.nLen);
				data->sDBHead.cZip =0;
				break;
			}
			else
			{
				data->sDBHead.nLen = dlen;
				data->sDBHead.cZip =2;
			}
			break;
		case 3:
			ddlen = data->sDBHead.nLen;
			if (m_pDes.EncryptString(data->sBuffer,ddlen,DESENCRYPTKEY3)<=0)
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
				return false;
			}
			data->sDBHead.nLen = ddlen;
			data->sDBHead.cZip =3;
			break;
		case 4:
			memcpy(zipDataBuf,data->sBuffer,data->sDBHead.nLen);
			if (!CBF_Tools::Compress((unsigned char *)data->sBuffer,dlen,(unsigned char *)(zipDataBuf),data->sDBHead.nLen))
			{
				m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"����ѹ��ʧ��");
				memcpy(data->sBuffer,zipDataBuf,data->sDBHead.nLen);
				data->sDBHead.cZip =0;
				break;
			}
			data->sDBHead.nLen = dlen;
			ddlen = dlen;
			if (m_pDes.EncryptString(data->sBuffer,ddlen,DESENCRYPTKEY)<=0)
			{
				m_pLog->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"���ܳ���");
				return false;
			}
			data->sDBHead.nLen = ddlen;
			data->sDBHead.cZip =4;
			break;
		default:
			data->sDBHead.cZip =0;
			return true;
	}
	return true;
}

void CBPCLink::Stop()
{
	OnClose(__FILE__,__LINE__,"ֹͣBPU");
}
