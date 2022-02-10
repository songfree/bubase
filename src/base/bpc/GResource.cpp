// GResource.cpp: implementation of the CGResource class.
//
//////////////////////////////////////////////////////////////////////

#include "GResource.h"
#include "PoolData.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGResource::CGResource()
{
	g_para1 = NULL;
	g_para2 = NULL;
	g_para3 = NULL;

	g_bToExit = false;
	m_serial_connect = 0;
	g_nPort = 9001;//�����˿� Ĭ��Ϊ9001
	
	g_nLoglevel = 5;//��־���� Ĭ��Ϊ5


	g_nDisconnectTime = 600;//δʹ�öϿ�ʱ�䣬���һ�������ڴ�ʱ����һֱû��ʹ�ã��򽫴����ӶϿ�����λ��Ĭ��600�뼴10����
	g_nDispatchTime = 10;
	
	g_sRegisterFile = "";
	g_nRegisterMode = 0;

	g_nHeartRun = 5;//����ʱ��  

	g_bIsInitOnce = false; //�Ƿ��ʼ��һ�Σ�����һ���߳�����г�ʼ���󣬽��˱�־��Ϊ1�������߳��жϴ˱�־�Ƿ���г�ʼ��
	g_bIsListen = false;//ֻ������һ�Σ�������󽫴˱�־��Ϊtrue,�����߳̽���������

	m_nSerialNum = 1;
	int ap = sizeof(m_serial_connect)*8-1; 
	m_nMaxSerial = (unsigned long)pow(2.0,ap);

	g_processSucessNum=0;
	g_processNum = 0;
	
	g_bIsMonitor = false;

	m_bisSetGPara1 = false;
	m_bisSetGPara2 = false;
	m_bisSetGPara3 = false;
	m_pSerial = NULL;
	m_nSerial_getMax = 99999999;
	m_nNextFileSerial = 0;

	memset(g_sCurPath,0,sizeof(g_sCurPath));
	memset(g_sModulePath,0,sizeof(g_sModulePath));
	memset(g_sModuleName,0,sizeof(g_sModuleName));
	memset(g_sStartDateTime,0,sizeof(g_sStartDateTime));

	m_bIsRecTime=1;
}

CGResource::~CGResource()
{
	//ֹͣ��־�߳�
	m_log.StopLog();
	m_pDrebDataLog.StopLog();
	m_pBpcCallDataLog.StopLog();
	for (int i=0; i < g_vBpuGroupInfo.size() ; i++)
	{
		delete g_vBpuGroupInfo[i].pdispathQueue;
		g_vBpuGroupInfo[i].pdispathQueue=NULL;
	}
	m_pSerial= NULL;
}

unsigned long CGResource::GetSerial()
{
//	CPMUTEX pp(&m_mutex);
	if (m_serial_connect >= m_nMaxSerial)
	{
		m_serial_connect = 1;
		if (m_nSerialNum >= m_nMaxSerial)
		{
			m_nSerialNum = 1; 
		}
		else
		{
			m_nSerialNum++;
		}
	}
	else
	{
		
		m_serial_connect++;
	}
	return m_serial_connect;
}

bool CGResource::Init(char *confile)
{
	m_sBpcParaFileName = confile;
	if (confile == NULL)
	{
		m_errMsg = "�����ļ�Ϊ��";
		return false;
	}
	if (!m_xmlconf.FromFile(confile))
	{
		printf("�����ļ��Ƿ�,�����ļ�[%s]\n",confile);
		m_errMsg = "���������ļ��Ƿ�";
		return false;
	}
	m_datetime.Update();
	
	CBF_Tools::StringCopy(g_sStartDateTime,17,m_datetime.ToStringDT17().c_str());
	//��ֹͣ��־�߳�
	if (!m_log.IsStopedThread())
	{
		m_log.StopLog();
	}
	int ret=0;
	std::string stmp;
	
	if (m_xmlconf.GetNodeValueByPath("package/head/public/loglevel",false,g_nLoglevel) == NULL)
	{
		m_errMsg = "�����ļ���,��[package/head/public/loglevel]�ڵ�";
		return false;
	}
	if (g_nLoglevel <0 )
	{
		g_nLoglevel = LOG_DEBUG;//���ô���Ϊ5
	}
	if (m_xmlconf.GetNodeValueByPath("package/head/public/dataloglevel",false,g_nDrebLoglevel) == NULL)
	{
		m_errMsg = "�����ļ���,��[package/head/public/dataloglevel]�ڵ�";
		return false;
	}
	if (g_nDrebLoglevel <0 )
	{
		g_nDrebLoglevel = LOG_DEBUG;//���ô���Ϊ5
	}
	if (m_xmlconf.GetNodeValueByPath("package/head/public/datalogsize",false,g_nDrebLogSize) == NULL)
	{
		//Ĭ�ϲ�����
		g_nDrebLogSize =0;
	}


	if (m_xmlconf.GetNodeValueByPath("package/head/public/logfile",false,g_sLogFileName) == NULL)
	{
		m_errMsg = "�����ļ���,��[package/head/public/logfile]�ڵ�";
		return false;
	}
	
	if (m_xmlconf.GetNodeValueByPath("package/head/public/logdir",false,g_sLogFilePath) == NULL)
	{
		m_errMsg = "�����ļ���,��[package/head/public/logdir]�ڵ�";
		g_sLogFilePath="";
	}
	if (m_xmlconf.GetNodeValueByPath("package/head/public/CRC",false,g_nCrcFlag) == NULL)
	{
		m_errMsg = "�����ļ���,��[package/head/public/CRC]�ڵ�";
		g_nCrcFlag=0;
	}

	if (m_xmlconf.GetNodeValueByPath("package/head/public/logsize",false,g_nLogSize) == NULL)
	{
		m_errMsg = "�����ļ���,��[package/head/public/logsize]�ڵ�";
		return false;
	}
	if (g_nLogSize <0 )
	{
		g_nLogSize = 0;//0Ϊ�����ƴ�С
	}
	m_log.SetMaxLogSize(g_nLogSize);
	m_log.SetLogPara(g_nLoglevel,g_sLogFilePath.c_str(),g_sLogFileName.c_str());
	
	//������־�߳�
	m_log.StartLog();
	
	
	if (m_xmlconf.GetNodeValueByPath("package/head/bpc/svrid",false,ret) == NULL)
	{
		m_errMsg = "�����ļ���,��[package/head/bpc/svrid]�ڵ�";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"%s",m_errMsg.c_str());
		return false;
	}
	if (ret<1)
	{
		m_errMsg = "�����ļ���,��[package/head/bpc/svrid]�ڵ����ò���";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"%s",m_errMsg.c_str());
		return false;
	}
	g_nSvrMainId = ret;
	if (m_xmlconf.GetNodeValueByPath("package/head/bpc/svrpid",false,ret) == NULL)
	{
		m_errMsg = "�����ļ���,��[package/head/bpc/svrpid]�ڵ�";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"%s",m_errMsg.c_str());
		return false;
	}
	if (ret<1 || ret>254)
	{
		m_errMsg = "�����ļ���,��[package/head/bpc/svrpid]�ڵ����ò���";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"%s",m_errMsg.c_str());
		return false;
	}
	g_nSvrPrivateId = ret;
	
	char tmpchar1[100];
	char tmpchar2[100];
	char tmpchar3[100];
	char tmpchar4[100];
	sprintf(tmpchar1,"srv%d-%d.DREB2BPC.dat",g_nSvrMainId,g_nSvrPrivateId);
	sprintf(tmpchar2,"srv%d-%d.BPC2BPU.dat",g_nSvrMainId,g_nSvrPrivateId);
	sprintf(tmpchar3,"srv%d-%d.BPCCALLDREB.dat",g_nSvrMainId,g_nSvrPrivateId);
	sprintf(tmpchar4,"srv%d-%d.BPUCALLBPC.dat",g_nSvrMainId,g_nSvrPrivateId);
	
	m_pDrebDataLog.SetLogPara(g_nDrebLogSize,g_nDrebLoglevel,g_sLogFilePath.c_str(),tmpchar1,tmpchar2);
	m_pBpcCallDataLog.SetLogPara(g_nDrebLogSize,g_nDrebLoglevel,g_sLogFilePath.c_str(),tmpchar3,tmpchar4);
	

	m_pDrebDataLog.StartLog();
	m_pBpcCallDataLog.StartLog();

	if (m_xmlconf.GetNodeValueByPath("package/head/bpc/dispatchtime",false,g_nDispatchTime) == NULL)
	{
		m_errMsg = "�����ļ�,��[package/head/dispatchtime/dispatchtime]�ڵ�";
		m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"%s",m_errMsg.c_str());
		g_nDispatchTime = 10;
	}
	
	if (m_xmlconf.GetNodeValueByPath("package/head/bpc/hearttime",false,g_nHeartRun) == NULL)
	{
		m_errMsg = "�����ļ�,��[package/head/bpc/hearttime]�ڵ�";
		m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"%s",m_errMsg.c_str());
		g_nHeartRun = 5;
	}
	if (m_xmlconf.GetNodeValueByPath("package/head/bpc/due_times",false,g_nDisconnectTime) == NULL)
	{
		m_errMsg = "�����ļ�,��[package/head/bpc/due_times]�ڵ�";
		m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"%s",m_errMsg.c_str());
		g_nDisconnectTime = 600;
	}

	if (m_xmlconf.GetNodeValueByPath("package/head/bpc/regmode",false,g_nRegisterMode) == NULL)
	{
		m_errMsg = "�����ļ�,��[package/head/bpc/regmode]�ڵ�";
		m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"%s",m_errMsg.c_str());
		g_nRegisterMode = 0;
	}
	if (1 == g_nRegisterMode)
	{
		if (m_xmlconf.GetNodeValueByPath("package/head/bpc/regfile",false,g_sRegisterFile) == NULL)
		{
			m_errMsg = "�����ļ���,��[package/head/bpc/regfile]�ڵ�";
			return false;
		}
	}

	g_nUseMonitor=0;
	if (m_xmlconf.GetNodeAttribute("package/head/monitor",false,"use",false,g_nUseMonitor) == NULL)
	{
		m_errMsg = "��package/head/monitor";
		g_nUseMonitor = 0;
	}
	if (g_nUseMonitor == 1)
	{
		if (m_xmlconf.GetNodeAttribute("package/head/monitor",false,"host",false,g_nMonitorHost) == NULL)
		{
			m_errMsg = "��package/head/monitor";
			m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"%s",m_errMsg.c_str());
			g_nMonitorHost = 0;
		}
		if (m_xmlconf.GetNodeValueByPath("package/head/monitor/nodeid",false,g_nMonitorDrebId) == NULL)
		{
			m_errMsg = "�ڵ�[package/head/monitor/nodeid]û������";
			m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"%s",m_errMsg.c_str());
			g_nMonitorDrebId = 0;
		}
		if (m_xmlconf.GetNodeValueByPath("package/head/monitor/nodepid",false,g_nMonitorDrebPrivateId) == NULL)
		{
			m_errMsg = "�ڵ�[package/head/monitor/nodepid]û������";
			m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"%s",m_errMsg.c_str());
			g_nMonitorDrebPrivateId = 0;
		}
		if (m_xmlconf.GetNodeValueByPath("package/head/monitor/svrid",false,g_nMonitorSvrId) == NULL)
		{
			m_errMsg = "�ڵ�[package/head/monitor/svrid]û������";
			m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"%s",m_errMsg.c_str());
			g_nMonitorSvrId = 0;
		}
		if (m_xmlconf.GetNodeValueByPath("package/head/monitor/svrpid",false,g_nMonitorSvrPrivateId) == NULL)
		{
			m_errMsg = "�ڵ�[package/head/monitor/svrpid]û������";
			m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"%s",m_errMsg.c_str());
			g_nMonitorSvrPrivateId = 0;
		}
		if (m_xmlconf.GetNodeValueByPath("package/head/monitor/txcode",false,g_nMonitorTxCode) == NULL)
		{
			m_errMsg = "�ڵ�[package/head/monitor/txcode]û������";
			m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"%s",m_errMsg.c_str());
			return false;
		}
	}

	if (m_xmlconf.GetNodeValueByPath("package/head/bpc/maxserial",false,m_nSerial_getMax) == NULL)
	{
		m_errMsg = "�����ļ�,��[package/head/bpc/maxserial]�ڵ�";
		m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"%s",m_errMsg.c_str());
		m_nSerial_getMax = 99999999;
	}
	
	CXmlNode *drebnode = m_xmlconf.GetNodeByPath("package/head/dreb",false);
	if (drebnode == NULL)
	{
		m_errMsg = "�����ļ���,��[package/head/dreb]�ڵ�";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"%s",m_errMsg.c_str());
		return false;
	}
	CXmlNode *tmpnode = (CXmlNode *)drebnode->GetFirstChild();
	S_DREBLINK_INFO dreblinkinfo;
	while (tmpnode != NULL)
	{
		tmpnode->GetAttribute("ip",false,dreblinkinfo.sIp);
		tmpnode->GetAttribute("port",false,stmp);
		dreblinkinfo.nPort = atoi(stmp.c_str());
		tmpnode->GetAttribute("bandwidth",false,stmp);
		dreblinkinfo.nBandwidth = atoi(stmp.c_str());
		tmpnode->GetAttribute("useipc",false,stmp);
		if (atoi(stmp.c_str()) ==1 )
		{
			dreblinkinfo.bIsIpc = true;
		}
		else
		{
			dreblinkinfo.bIsIpc = false;
		}
		g_vDrebLinkInfo.push_back(dreblinkinfo);
		tmpnode = (CXmlNode *)tmpnode->getNextSibling();
	}

	if (m_xmlconf.GetNodeValueByPath("package/head/bpu/bpuport",false,g_nPort) == NULL)
	{
		m_errMsg = "�����ļ���,��[package/head/bpu/bpuport]�ڵ�";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"%s",m_errMsg.c_str());
		return false;
	}
	if (g_nPort <= 1024)
	{
		m_errMsg = "�����ļ���,[package/head/bpu/bpuport]�ڵ����ò��Ϸ�";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"%s",(char *)m_errMsg.c_str());
		return false;
	}
	
	char tmpchar[600];
	S_BPUGROUP_INFO bpugroupinfo;

	CXmlNode *bpugroupnode = m_xmlconf.GetNodeByPath("package/head/bpu/bpugroup",false);
	if (bpugroupnode == NULL)
	{
		m_errMsg = "�����ļ��� ��[package/head/bpu/bpugroup]�ڵ�";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"%s",(char *)m_errMsg.c_str());
		return false;
	}
	
	CXmlNode *gpnode   = (CXmlNode *)bpugroupnode->GetFirstChild();
	int bpugroupindex=0;
	int rowid;
	S_BPUGROUP_TBL tgtbl;
	while (gpnode !=  NULL)
	{
		bpugroupinfo.g_sBpuGroupName = gpnode->GetNodeName();
		if (bpugroupinfo.g_sBpuGroupName.length()>20)
		{
			m_errMsg = std::string("BPU�����Ƴ��Ȳ��ܴ���20�ַ�");
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ļ��� %s",(char *)m_errMsg.c_str());
			return false;
		}
		sprintf(tmpchar,"package/head/bpu/bpugroup/%s/dispatchmode",bpugroupinfo.g_sBpuGroupName.c_str());
		if (m_xmlconf.GetNodeValueByPath(tmpchar,true,stmp,true) == NULL)
		{
			m_errMsg = std::string("��[") + tmpchar +"]�ڵ�";
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ļ��� %s",(char *)m_errMsg.c_str());
			return false;
		}
		bpugroupinfo.g_nDispatchMode = atoi(stmp.c_str());

		if (1 == bpugroupinfo.g_nDispatchMode ) //����ģʽΪ1,ֻ��һ��BPU
		{
			bpugroupinfo.g_nBpuNum = 1;
		}
		else  //����ģʽΪ0,ȡBPU��
		{
			sprintf(tmpchar,"package/head/bpu/bpugroup/%s/startnum",bpugroupinfo.g_sBpuGroupName.c_str());
			if (m_xmlconf.GetNodeValueByPath(tmpchar,true,stmp,true) == NULL)
			{
				m_errMsg = std::string("��[") + tmpchar +"]�ڵ�";
				m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ļ��� %s",(char *)m_errMsg.c_str());
				return false;
			}
			bpugroupinfo.g_nBpuNum = atoi(stmp.c_str());
		}
		sprintf(tmpchar,"package/head/bpu/bpugroup/%s/endian",bpugroupinfo.g_sBpuGroupName.c_str());
		if (m_xmlconf.GetNodeValueByPath(tmpchar,true,stmp,true) == NULL)
		{
			m_errMsg = std::string("��[") + tmpchar +"]�ڵ�";
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ļ��� %s Ĭ��ʹ��������",(char *)m_errMsg.c_str());
			bpugroupinfo.g_nUseBigEndian = 0;
		}
		else
		{
			bpugroupinfo.g_nUseBigEndian = atoi(stmp.c_str());
		}
		


		sprintf(tmpchar,"package/head/bpu/bpugroup/%s/heartans",bpugroupinfo.g_sBpuGroupName.c_str());
		if (m_xmlconf.GetNodeValueByPath(tmpchar,true,stmp,true) == NULL)
		{
			m_errMsg = std::string("��[") + tmpchar +"]�ڵ�";
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ļ��� %s",(char *)m_errMsg.c_str());
			return false;
		}
		if (stmp.compare("1") == 0)
		{
			bpugroupinfo.g_bIsAnsPing = true;
		}
		else
		{
			bpugroupinfo.g_bIsAnsPing = false; 
		}
		
		sprintf(tmpchar,"package/head/bpu/bpugroup/%s/sendplain",bpugroupinfo.g_sBpuGroupName.c_str());
		if (m_xmlconf.GetNodeValueByPath(tmpchar,true,stmp,true) == NULL)
		{
			m_errMsg = std::string("��[") + tmpchar +"]�ڵ�";
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ļ��� %s",(char *)m_errMsg.c_str());
			return false;
		}
		if (stmp.compare("1") == 0)
		{
			bpugroupinfo.g_bIsUnencrypt = true;
		}
		else
		{
			bpugroupinfo.g_bIsUnencrypt = false; 
		}

		bpugroupinfo.g_nStartBpuNum = 0;
		sprintf(tmpchar,"package/head/bpu/bpugroup/%s/autoloadbpu",bpugroupinfo.g_sBpuGroupName.c_str());
		if (m_xmlconf.GetNodeAttribute(tmpchar,true,"use",false,stmp,true) == NULL)
		{
			m_errMsg = std::string("��[") + tmpchar +"]�ڵ��use����";
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ļ��� %s",(char *)m_errMsg.c_str());
			return false;
		}
		if (stmp.compare("1")== 0)
		{
			bpugroupinfo.g_bIsAutoLoadBu = true;
		}
		else
		{
			bpugroupinfo.g_bIsAutoLoadBu = false;
		}
		if (bpugroupinfo.g_bIsAutoLoadBu)
		{
			sprintf(tmpchar,"package/head/bpu/bpugroup/%s/autoloadbpu/bpuprgname",bpugroupinfo.g_sBpuGroupName.c_str());
			if (m_xmlconf.GetNodeValueByPath(tmpchar,true,bpugroupinfo.g_sBuPrgName,true) == NULL)
			{
				m_errMsg = std::string("��[") + tmpchar +"]�ڵ�";
				m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ļ��� %s",(char *)m_errMsg.c_str());
				return false;
			}
			sprintf(tmpchar,"package/head/bpu/bpugroup/%s/autoloadbpu/bpuprgpath",bpugroupinfo.g_sBpuGroupName.c_str());
			if (m_xmlconf.GetNodeValueByPath(tmpchar,true,bpugroupinfo.g_sBuPrgPath,true) == NULL)
			{
				m_errMsg = std::string("��[") + tmpchar +"]�ڵ�";
				m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ļ��� %s",(char *)m_errMsg.c_str());
				return false;
			}
			sprintf(tmpchar,"package/head/bpu/bpugroup/%s/autoloadbpu/bpurunpath",bpugroupinfo.g_sBpuGroupName.c_str());
			if (m_xmlconf.GetNodeValueByPath(tmpchar,true,bpugroupinfo.g_sBuRunPath,true) == NULL)
			{
				m_errMsg = std::string("��[") + tmpchar +"]�ڵ�";
				m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ļ��� %s",(char *)m_errMsg.c_str());
				return false;
			}
			sprintf(tmpchar,"package/head/bpu/bpugroup/%s/autoloadbpu/bpupara",bpugroupinfo.g_sBpuGroupName.c_str());
			if (m_xmlconf.GetNodeValueByPath(tmpchar,true,bpugroupinfo.g_sBuPrgPara,true) == NULL)
			{
				m_errMsg = std::string("��[") + tmpchar +"]�ڵ�";
				m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ļ��� %s",(char *)m_errMsg.c_str());
				return false;
			}
		}
		
		bzero(&tgtbl,sizeof(S_BPUGROUP_TBL));
		tgtbl.bpugroupindex = bpugroupindex;
		strcpy(tgtbl.bpugroupname,bpugroupinfo.g_sBpuGroupName.c_str());
		if (m_pkey.Find(tgtbl.bpugroupname))
		{
			m_errMsg = std::string("BPU�������ظ�:")+bpugroupinfo.g_sBpuGroupName;
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ļ��� %s",(char *)m_errMsg.c_str());
			return false;
		}
		bpugroupinfo.g_bIsReg = false;
		bpugroupinfo.g_nJavaBpuPid = 0;
		bpugroupinfo.pdispathQueue = new CPoolData();
		if (bpugroupinfo.pdispathQueue == NULL)
		{
			m_errMsg = std::string("new CPoolData����")+bpugroupinfo.g_sBpuGroupName;
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"%s",(char *)m_errMsg.c_str());
			return false;
		}

		bpugroupinfo.pdispathQueue->m_pRes = this;
		bpugroupinfo.bpugroupindex = bpugroupindex;

		rowid = m_table.Add(tgtbl);
		m_pkey.Add(rowid,tgtbl.bpugroupname);
		bpugroupindex++;
		g_vBpuGroupInfo.push_back(bpugroupinfo);
		gpnode = (CXmlNode *)gpnode->getNextSibling();
	}
	
	


	sprintf(tmpchar,"%d",g_nLoglevel);
	m_errMsg = "";
	m_errMsg = m_errMsg+ "��־����:" + tmpchar ; 
	m_errMsg = m_errMsg + " ��־�ļ�:" + g_sLogFileName ;
	sprintf(tmpchar,"%d",g_nPort);
	m_errMsg = m_errMsg + " �����˿�:" + tmpchar ;


	sprintf(tmpchar,"%d",g_nDisconnectTime);
	m_errMsg = m_errMsg +" δʹ�öϿ�ʱ��(��):" + tmpchar ;
	sprintf(tmpchar,"%d",g_nHeartRun);
	m_errMsg = m_errMsg +" ����ʱ��(��):" + tmpchar ;
	

	m_log.LogMp(LOG_INFO,__FILE__,__LINE__,"���в��� %s" ,(char *)m_errMsg.c_str());

	return true;
}

std::string CGResource::GetMsg()
{
	return m_errMsg;
}

bool CGResource::OpenDll(DLLHANDLE *pDll, const char *dllname,const char *dllpath)
{
	bool bIsNotPath=false;
	char dllpathname[801];
	memset(dllpathname,0,sizeof(dllpathname));
	if (strlen(dllpath) <1)
	{
		bIsNotPath=true;
	}
	else
	{
		bIsNotPath=false;
	}
	if (bIsNotPath)
	{
		sprintf(dllpathname,"%s",dllname);
	}
	else
	{
		sprintf(dllpathname,"%s/%s",dllpath,dllname);
	}
	
	CBF_Tools::Rtrim(dllpathname);
	
	*pDll = LOADDLL(dllpathname);
#ifdef WIN32
	if (ISNOTOPEN(*pDll))
	{
		if (bIsNotPath)
		{
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�򿪶�̬��ʧ�� %s",dllpathname);
			return false;
		}
		sprintf(dllpathname,"%s",dllname);
		CBF_Tools::Rtrim(dllpathname);
		*pDll = LOADDLL(dllpathname);
		if (ISNOTOPEN(*pDll))
		{
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�򿪶�̬��ʧ�� %s",dllpathname);
			return false;
		}
	}
#else
	char *pdllerror;
	pdllerror = dlerror();
	if (pdllerror!=NULL)
	{	
		if (bIsNotPath)
		{
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�򿪶�̬��ʧ�� %s",dllpathname);
			return false;
		}
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�򿪶�̬��ʧ�� %s %s",dllpathname,pdllerror);
		sprintf(dllpathname,"%s",dllname);
		CBF_Tools::Rtrim(dllpathname);
		*pDll = LOADDLL(dllpathname);
		pdllerror = dlerror();
		if (pdllerror!=NULL)
		{
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�򿪶�̬��ʧ�� %s %s",dllpathname,pdllerror);
			return false;
		}
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�򿪶�̬��ɹ� %s",dllpathname);
		
	}
#endif	
	return true;
}


bool CGResource::SetInitOnce()
{
	CBF_PMutex pp(&m_mutex);
	if (!g_bIsInitOnce)
	{
		g_bIsInitOnce = true;
		return g_bIsInitOnce;
	}
	else
	{
		return false;
	}
}


bool CGResource::SetListen()
{
	CBF_PMutex pp(&m_mutex);
	if (!g_bIsListen)
	{
		g_bIsListen = true;
		return g_bIsListen;
	}
	else
	{
		return false;
	}
}

unsigned long CGResource::GetGenerateSerial()
{
	return m_serial_connect*m_nSerialNum;
}


int CGResource::PutProcessNum()
{
	if (g_bIsMonitor)
	{
//		CPMUTEX pp(&m_mutex_putprocess);
		if (g_processNum >= m_nMaxSerial)
		{
			g_processNum = 1; 
		}
		else
		{
			g_processNum++;
		}
	}
	return 0;
}
int CGResource::PutProcessSucessNum()
{
	if (g_bIsMonitor)
	{
//		CPMUTEX pp(&m_mutex_putprocesssucess);
		if (g_processSucessNum >= m_nMaxSerial)
		{
			g_processSucessNum = 1; 
		}
		else
		{
			g_processSucessNum++;
		}
	}
	return 0;
}
/// ������: SetGPara1
/// ���  : ������ 2008-12-30 9:42:48
/// ����  : virtual bool 
/// ����  : bool setflag=true
/// ����  : ����ȫ�ֱ���1,���������򷵻�false
bool CGResource::SetGPara1(bool setflag)
{
	CBF_PMutex pp(&m_mutex_setgpara1);
	if (m_bisSetGPara1)//������
	{
		return false;
	}
	m_bisSetGPara1 = setflag;
	return true;
}


/// ������: isSetGPara2
/// ���  : ������ 2008-12-30 9:36:10
/// ����  : virtual bool 
/// ����  : bool setflag=true
/// ����  : ����ȫ�ֱ���2,���������򷵻�false
bool CGResource::SetGPara2(bool setflag)
{
	CBF_PMutex pp(&m_mutex_setgpara2);
	if (m_bisSetGPara2)//������
	{
		return false;
	}
	m_bisSetGPara2 = setflag;
	return true;
}

/// ������: isSetGPara3
/// ���  : ������ 2008-12-30 9:36:13
/// ����  : virtual bool 
/// ����  : bool setflag=true
/// ����  : ����ȫ�ֱ���3,���������򷵻�false
bool CGResource::SetGPara3(bool setflag)
{
	CBF_PMutex pp(&m_mutex_setgpara3);
	if (m_bisSetGPara3)//������
	{
		return false;
	}
	m_bisSetGPara3 = setflag;
	return true;
}
long CGResource::GetThreadShareSerial()
{
	CBF_PMutex pp(&m_mutex_getSerial);
	if (m_pSerial->nSerial_get>=m_nSerial_getMax)//������
	{
		m_pSerial->nSerial_get = 1;
	}
	else
	{
		m_pSerial->nSerial_get++;
	}
	return m_pSerial->nSerial_get;
	
}

void CGResource::WriteBack()
{
	m_xmlconf.SetNodeValueByPath("package/head/BPC����/������ˮ��ʼֵ",false,m_pSerial->nSerial_get);
	
	printf("������д�������ļ�[%s]\n",m_sBpcParaFileName.c_str());
	m_xmlconf.ToFile((char *)m_sBpcParaFileName.c_str());
}

std::string CGResource::GetNextFileName(unsigned int &serial)
{
	CBF_PMutex pp(&m_mutex_getnextfile);
	m_nNextFileSerial++;
	serial=m_nNextFileSerial;
	char sNextFile[400];
#ifdef _WINDOWS
	sprintf(sNextFile,"%s\\nxt\\%d.nxt",g_sCurPath,m_nNextFileSerial);
#else
	sprintf(sNextFile,"%s/nxt/%d.nxt",g_sCurPath,m_nNextFileSerial);
#endif
	return sNextFile;
}

int CGResource::PushData(unsigned int bpugroypindex,S_BPC_RSMSG data,int prio)
{
	return g_vBpuGroupInfo[bpugroypindex].pdispathQueue->PushData(data,prio);
}
// int CGResource::PushData(char *bpugroupname, S_BPC_RSMSG data, int prio)
// {
// 	CInt iset;
// 	if (!m_pkey.Select(iset,bpugroupname))
// 	{
// 		//δ�ҵ�bpu����
// 		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"δ�ҵ�bpu�� %s ",bpugroupname);
// 		return -1;
// 	}
// 	int rowid;
// 	if (!iset.First(rowid))
// 	{
// 		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"δ�ҵ�bpu�� %s �Ҳ���rowid",bpugroupname);
// 		return -1;
// 	}
// 	int bpugroupindex = m_table.m_table[rowid].bpugroupindex;
// 	return g_vBpuGroupInfo[bpugroupindex].pdispathQueue->PushData(data,prio);
// 	
// }

void CGResource::Stop()
{
	m_log.StopLog();
	m_pDrebDataLog.StopLog();
	m_pBpcCallDataLog.StopLog();
}

std::string CGResource::GetBpcMsgType(int msgtype)
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
std::string CGResource::GetDrebCmdType(int cmdtype)
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
void CGResource::PutTime()
{
	if (1 == m_bIsRecTime)
	{
		m_vTime.push_back(CBF_Date_Time::GetTickUS());
	}
	
}

void CGResource::LogTime()
{
	for (int ret=0; ret < m_vTime.size() ; ret ++)
	{
#ifdef WINDOWS
		m_log.LogMp(LOG_INFO,__FILE__,__LINE__,"��%d�� ʱ��[%I64d]",ret,m_vTime[ret]);
#else
		m_log.LogMp(LOG_INFO,__FILE__,__LINE__,"��%d�� ʱ��[%lld]",ret,m_vTime[ret]);
#endif
	}
	m_vTime.clear();
}
