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
	g_nPort = 9001;//侦听端口 默认为9001
	
	g_nLoglevel = 5;//日志级别 默认为5


	g_nDisconnectTime = 600;//未使用断开时间，如果一个连接在此时间内一直没有使用，则将此连接断开。单位秒默认600秒即10分钟
	g_nDispatchTime = 10;
	
	g_sRegisterFile = "";
	g_nRegisterMode = 0;

	g_nHeartRun = 5;//心跳时间  

	g_bIsInitOnce = false; //是否初始化一次，即在一个线程里进行初始化后，将此标志置为1，其它线程判断此标志是否进行初始化
	g_bIsListen = false;//只能侦听一次，侦听完后将此标志置为true,其它线程将不用侦听

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
	//停止日志线程
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
		m_errMsg = "传入文件为空";
		return false;
	}
	if (!m_xmlconf.FromFile(confile))
	{
		printf("传入文件非法,配置文件[%s]\n",confile);
		m_errMsg = "传入配置文件非法";
		return false;
	}
	m_datetime.Update();
	
	CBF_Tools::StringCopy(g_sStartDateTime,17,m_datetime.ToStringDT17().c_str());
	//先停止日志线程
	if (!m_log.IsStopedThread())
	{
		m_log.StopLog();
	}
	int ret=0;
	std::string stmp;
	
	if (m_xmlconf.GetNodeValueByPath("package/head/public/loglevel",false,g_nLoglevel) == NULL)
	{
		m_errMsg = "配置文件错,无[package/head/public/loglevel]节点";
		return false;
	}
	if (g_nLoglevel <0 )
	{
		g_nLoglevel = LOG_DEBUG;//配置错则为5
	}
	if (m_xmlconf.GetNodeValueByPath("package/head/public/dataloglevel",false,g_nDrebLoglevel) == NULL)
	{
		m_errMsg = "配置文件错,无[package/head/public/dataloglevel]节点";
		return false;
	}
	if (g_nDrebLoglevel <0 )
	{
		g_nDrebLoglevel = LOG_DEBUG;//配置错则为5
	}
	if (m_xmlconf.GetNodeValueByPath("package/head/public/datalogsize",false,g_nDrebLogSize) == NULL)
	{
		//默认不限制
		g_nDrebLogSize =0;
	}


	if (m_xmlconf.GetNodeValueByPath("package/head/public/logfile",false,g_sLogFileName) == NULL)
	{
		m_errMsg = "配置文件错,无[package/head/public/logfile]节点";
		return false;
	}
	
	if (m_xmlconf.GetNodeValueByPath("package/head/public/logdir",false,g_sLogFilePath) == NULL)
	{
		m_errMsg = "配置文件错,无[package/head/public/logdir]节点";
		g_sLogFilePath="";
	}
	if (m_xmlconf.GetNodeValueByPath("package/head/public/CRC",false,g_nCrcFlag) == NULL)
	{
		m_errMsg = "配置文件错,无[package/head/public/CRC]节点";
		g_nCrcFlag=0;
	}

	if (m_xmlconf.GetNodeValueByPath("package/head/public/logsize",false,g_nLogSize) == NULL)
	{
		m_errMsg = "配置文件错,无[package/head/public/logsize]节点";
		return false;
	}
	if (g_nLogSize <0 )
	{
		g_nLogSize = 0;//0为不限制大小
	}
	m_log.SetMaxLogSize(g_nLogSize);
	m_log.SetLogPara(g_nLoglevel,g_sLogFilePath.c_str(),g_sLogFileName.c_str());
	
	//启动日志线程
	m_log.StartLog();
	
	
	if (m_xmlconf.GetNodeValueByPath("package/head/bpc/svrid",false,ret) == NULL)
	{
		m_errMsg = "配置文件错,无[package/head/bpc/svrid]节点";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"%s",m_errMsg.c_str());
		return false;
	}
	if (ret<1)
	{
		m_errMsg = "配置文件错,无[package/head/bpc/svrid]节点配置不符";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"%s",m_errMsg.c_str());
		return false;
	}
	g_nSvrMainId = ret;
	if (m_xmlconf.GetNodeValueByPath("package/head/bpc/svrpid",false,ret) == NULL)
	{
		m_errMsg = "配置文件错,无[package/head/bpc/svrpid]节点";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"%s",m_errMsg.c_str());
		return false;
	}
	if (ret<1 || ret>254)
	{
		m_errMsg = "配置文件错,无[package/head/bpc/svrpid]节点配置不符";
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
		m_errMsg = "配置文件,无[package/head/dispatchtime/dispatchtime]节点";
		m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"%s",m_errMsg.c_str());
		g_nDispatchTime = 10;
	}
	
	if (m_xmlconf.GetNodeValueByPath("package/head/bpc/hearttime",false,g_nHeartRun) == NULL)
	{
		m_errMsg = "配置文件,无[package/head/bpc/hearttime]节点";
		m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"%s",m_errMsg.c_str());
		g_nHeartRun = 5;
	}
	if (m_xmlconf.GetNodeValueByPath("package/head/bpc/due_times",false,g_nDisconnectTime) == NULL)
	{
		m_errMsg = "配置文件,无[package/head/bpc/due_times]节点";
		m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"%s",m_errMsg.c_str());
		g_nDisconnectTime = 600;
	}

	if (m_xmlconf.GetNodeValueByPath("package/head/bpc/regmode",false,g_nRegisterMode) == NULL)
	{
		m_errMsg = "配置文件,无[package/head/bpc/regmode]节点";
		m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"%s",m_errMsg.c_str());
		g_nRegisterMode = 0;
	}
	if (1 == g_nRegisterMode)
	{
		if (m_xmlconf.GetNodeValueByPath("package/head/bpc/regfile",false,g_sRegisterFile) == NULL)
		{
			m_errMsg = "配置文件错,无[package/head/bpc/regfile]节点";
			return false;
		}
	}

	g_nUseMonitor=0;
	if (m_xmlconf.GetNodeAttribute("package/head/monitor",false,"use",false,g_nUseMonitor) == NULL)
	{
		m_errMsg = "无package/head/monitor";
		g_nUseMonitor = 0;
	}
	if (g_nUseMonitor == 1)
	{
		if (m_xmlconf.GetNodeAttribute("package/head/monitor",false,"host",false,g_nMonitorHost) == NULL)
		{
			m_errMsg = "无package/head/monitor";
			m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"%s",m_errMsg.c_str());
			g_nMonitorHost = 0;
		}
		if (m_xmlconf.GetNodeValueByPath("package/head/monitor/nodeid",false,g_nMonitorDrebId) == NULL)
		{
			m_errMsg = "节点[package/head/monitor/nodeid]没有配置";
			m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"%s",m_errMsg.c_str());
			g_nMonitorDrebId = 0;
		}
		if (m_xmlconf.GetNodeValueByPath("package/head/monitor/nodepid",false,g_nMonitorDrebPrivateId) == NULL)
		{
			m_errMsg = "节点[package/head/monitor/nodepid]没有配置";
			m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"%s",m_errMsg.c_str());
			g_nMonitorDrebPrivateId = 0;
		}
		if (m_xmlconf.GetNodeValueByPath("package/head/monitor/svrid",false,g_nMonitorSvrId) == NULL)
		{
			m_errMsg = "节点[package/head/monitor/svrid]没有配置";
			m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"%s",m_errMsg.c_str());
			g_nMonitorSvrId = 0;
		}
		if (m_xmlconf.GetNodeValueByPath("package/head/monitor/svrpid",false,g_nMonitorSvrPrivateId) == NULL)
		{
			m_errMsg = "节点[package/head/monitor/svrpid]没有配置";
			m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"%s",m_errMsg.c_str());
			g_nMonitorSvrPrivateId = 0;
		}
		if (m_xmlconf.GetNodeValueByPath("package/head/monitor/txcode",false,g_nMonitorTxCode) == NULL)
		{
			m_errMsg = "节点[package/head/monitor/txcode]没有配置";
			m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"%s",m_errMsg.c_str());
			return false;
		}
	}

	if (m_xmlconf.GetNodeValueByPath("package/head/bpc/maxserial",false,m_nSerial_getMax) == NULL)
	{
		m_errMsg = "配置文件,无[package/head/bpc/maxserial]节点";
		m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"%s",m_errMsg.c_str());
		m_nSerial_getMax = 99999999;
	}
	
	CXmlNode *drebnode = m_xmlconf.GetNodeByPath("package/head/dreb",false);
	if (drebnode == NULL)
	{
		m_errMsg = "配置文件错,无[package/head/dreb]节点";
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
		m_errMsg = "配置文件错,无[package/head/bpu/bpuport]节点";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"%s",m_errMsg.c_str());
		return false;
	}
	if (g_nPort <= 1024)
	{
		m_errMsg = "配置文件错,[package/head/bpu/bpuport]节点配置不合法";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"%s",(char *)m_errMsg.c_str());
		return false;
	}
	
	char tmpchar[600];
	S_BPUGROUP_INFO bpugroupinfo;

	CXmlNode *bpugroupnode = m_xmlconf.GetNodeByPath("package/head/bpu/bpugroup",false);
	if (bpugroupnode == NULL)
	{
		m_errMsg = "配置文件错 无[package/head/bpu/bpugroup]节点";
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
			m_errMsg = std::string("BPU组名称长度不能大于20字符");
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
			return false;
		}
		sprintf(tmpchar,"package/head/bpu/bpugroup/%s/dispatchmode",bpugroupinfo.g_sBpuGroupName.c_str());
		if (m_xmlconf.GetNodeValueByPath(tmpchar,true,stmp,true) == NULL)
		{
			m_errMsg = std::string("无[") + tmpchar +"]节点";
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
			return false;
		}
		bpugroupinfo.g_nDispatchMode = atoi(stmp.c_str());

		if (1 == bpugroupinfo.g_nDispatchMode ) //分配模式为1,只有一个BPU
		{
			bpugroupinfo.g_nBpuNum = 1;
		}
		else  //分配模式为0,取BPU数
		{
			sprintf(tmpchar,"package/head/bpu/bpugroup/%s/startnum",bpugroupinfo.g_sBpuGroupName.c_str());
			if (m_xmlconf.GetNodeValueByPath(tmpchar,true,stmp,true) == NULL)
			{
				m_errMsg = std::string("无[") + tmpchar +"]节点";
				m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
				return false;
			}
			bpugroupinfo.g_nBpuNum = atoi(stmp.c_str());
		}
		sprintf(tmpchar,"package/head/bpu/bpugroup/%s/endian",bpugroupinfo.g_sBpuGroupName.c_str());
		if (m_xmlconf.GetNodeValueByPath(tmpchar,true,stmp,true) == NULL)
		{
			m_errMsg = std::string("无[") + tmpchar +"]节点";
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s 默认使用主机序",(char *)m_errMsg.c_str());
			bpugroupinfo.g_nUseBigEndian = 0;
		}
		else
		{
			bpugroupinfo.g_nUseBigEndian = atoi(stmp.c_str());
		}
		


		sprintf(tmpchar,"package/head/bpu/bpugroup/%s/heartans",bpugroupinfo.g_sBpuGroupName.c_str());
		if (m_xmlconf.GetNodeValueByPath(tmpchar,true,stmp,true) == NULL)
		{
			m_errMsg = std::string("无[") + tmpchar +"]节点";
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
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
			m_errMsg = std::string("无[") + tmpchar +"]节点";
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
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
			m_errMsg = std::string("无[") + tmpchar +"]节点或use属性";
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
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
				m_errMsg = std::string("无[") + tmpchar +"]节点";
				m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
				return false;
			}
			sprintf(tmpchar,"package/head/bpu/bpugroup/%s/autoloadbpu/bpuprgpath",bpugroupinfo.g_sBpuGroupName.c_str());
			if (m_xmlconf.GetNodeValueByPath(tmpchar,true,bpugroupinfo.g_sBuPrgPath,true) == NULL)
			{
				m_errMsg = std::string("无[") + tmpchar +"]节点";
				m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
				return false;
			}
			sprintf(tmpchar,"package/head/bpu/bpugroup/%s/autoloadbpu/bpurunpath",bpugroupinfo.g_sBpuGroupName.c_str());
			if (m_xmlconf.GetNodeValueByPath(tmpchar,true,bpugroupinfo.g_sBuRunPath,true) == NULL)
			{
				m_errMsg = std::string("无[") + tmpchar +"]节点";
				m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
				return false;
			}
			sprintf(tmpchar,"package/head/bpu/bpugroup/%s/autoloadbpu/bpupara",bpugroupinfo.g_sBpuGroupName.c_str());
			if (m_xmlconf.GetNodeValueByPath(tmpchar,true,bpugroupinfo.g_sBuPrgPara,true) == NULL)
			{
				m_errMsg = std::string("无[") + tmpchar +"]节点";
				m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
				return false;
			}
		}
		
		bzero(&tgtbl,sizeof(S_BPUGROUP_TBL));
		tgtbl.bpugroupindex = bpugroupindex;
		strcpy(tgtbl.bpugroupname,bpugroupinfo.g_sBpuGroupName.c_str());
		if (m_pkey.Find(tgtbl.bpugroupname))
		{
			m_errMsg = std::string("BPU组名称重复:")+bpugroupinfo.g_sBpuGroupName;
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
			return false;
		}
		bpugroupinfo.g_bIsReg = false;
		bpugroupinfo.g_nJavaBpuPid = 0;
		bpugroupinfo.pdispathQueue = new CPoolData();
		if (bpugroupinfo.pdispathQueue == NULL)
		{
			m_errMsg = std::string("new CPoolData出错")+bpugroupinfo.g_sBpuGroupName;
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
	m_errMsg = m_errMsg+ "日志级别:" + tmpchar ; 
	m_errMsg = m_errMsg + " 日志文件:" + g_sLogFileName ;
	sprintf(tmpchar,"%d",g_nPort);
	m_errMsg = m_errMsg + " 侦听端口:" + tmpchar ;


	sprintf(tmpchar,"%d",g_nDisconnectTime);
	m_errMsg = m_errMsg +" 未使用断开时间(秒):" + tmpchar ;
	sprintf(tmpchar,"%d",g_nHeartRun);
	m_errMsg = m_errMsg +" 心跳时间(秒):" + tmpchar ;
	

	m_log.LogMp(LOG_INFO,__FILE__,__LINE__,"运行参数 %s" ,(char *)m_errMsg.c_str());

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
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"打开动态库失败 %s",dllpathname);
			return false;
		}
		sprintf(dllpathname,"%s",dllname);
		CBF_Tools::Rtrim(dllpathname);
		*pDll = LOADDLL(dllpathname);
		if (ISNOTOPEN(*pDll))
		{
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"打开动态库失败 %s",dllpathname);
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
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"打开动态库失败 %s",dllpathname);
			return false;
		}
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"打开动态库失败 %s %s",dllpathname,pdllerror);
		sprintf(dllpathname,"%s",dllname);
		CBF_Tools::Rtrim(dllpathname);
		*pDll = LOADDLL(dllpathname);
		pdllerror = dlerror();
		if (pdllerror!=NULL)
		{
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"打开动态库失败 %s %s",dllpathname,pdllerror);
			return false;
		}
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"打开动态库成功 %s",dllpathname);
		
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
/// 函数名: SetGPara1
/// 编程  : 王明松 2008-12-30 9:42:48
/// 返回  : virtual bool 
/// 参数  : bool setflag=true
/// 描述  : 设置全局变量1,若已设置则返回false
bool CGResource::SetGPara1(bool setflag)
{
	CBF_PMutex pp(&m_mutex_setgpara1);
	if (m_bisSetGPara1)//已设置
	{
		return false;
	}
	m_bisSetGPara1 = setflag;
	return true;
}


/// 函数名: isSetGPara2
/// 编程  : 王明松 2008-12-30 9:36:10
/// 返回  : virtual bool 
/// 参数  : bool setflag=true
/// 描述  : 设置全局变量2,若已设置则返回false
bool CGResource::SetGPara2(bool setflag)
{
	CBF_PMutex pp(&m_mutex_setgpara2);
	if (m_bisSetGPara2)//已设置
	{
		return false;
	}
	m_bisSetGPara2 = setflag;
	return true;
}

/// 函数名: isSetGPara3
/// 编程  : 王明松 2008-12-30 9:36:13
/// 返回  : virtual bool 
/// 参数  : bool setflag=true
/// 描述  : 设置全局变量3,若已设置则返回false
bool CGResource::SetGPara3(bool setflag)
{
	CBF_PMutex pp(&m_mutex_setgpara3);
	if (m_bisSetGPara3)//已设置
	{
		return false;
	}
	m_bisSetGPara3 = setflag;
	return true;
}
long CGResource::GetThreadShareSerial()
{
	CBF_PMutex pp(&m_mutex_getSerial);
	if (m_pSerial->nSerial_get>=m_nSerial_getMax)//已设置
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
	m_xmlconf.SetNodeValueByPath("package/head/BPC配置/本次流水开始值",false,m_pSerial->nSerial_get);
	
	printf("将配置写回配置文件[%s]\n",m_sBpcParaFileName.c_str());
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
// 		//未找到bpu组名
// 		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"未找到bpu组 %s ",bpugroupname);
// 		return -1;
// 	}
// 	int rowid;
// 	if (!iset.First(rowid))
// 	{
// 		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"未找到bpu组 %s 找不到rowid",bpugroupname);
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
	case MSG_REQ:        // 0      //接收请求,bpc发给BPU的请求
		return "MSG_REQ";
	case MSG_FREE:        //1      //BPU发送空闲请求
		return "MSG_FREE";
	case MSG_GETSERIAL:   //4      //BPU取共享唯一流水
		return "MSG_GETSERIAL";
	case MSG_SETGPARA1:  //5      //BPU发送SETGPARA1
		return "MSG_SETGPARA1";
	case MSG_SETGPARA2:   //6      //BPU发送SETGPARA2
		return "MSG_SETGPARA2";
	case MSG_SETGPARA3:   //7      //BPU发送SETGPARA3
		return "MSG_SETGPARA3";	
	case MSG_EXTCALL:      //10      //BPU发送外调请求	
		return "MSG_EXTCALL";
	case MSG_BPCCALL:      //11      //BPU发送PU组调用请求
		return "MSG_BPCCALL";
	case MSG_BPUREG:       //12      //BPU发送注册请求
		return "MSG_BPUREG";
	case MSG_BPUISREG:     //13      //BPU发送是否注册请求
		return "MSG_BPUISREG";
	case MSG_GETNEXT:      //15      //dreb过来的取后续包的消息
		return "MSG_GETNEXT";
	default:
		return "";
	}
}
std::string CGResource::GetDrebCmdType(int cmdtype)
{
	switch (cmdtype)
	{
		
	case  CMD_ROUTER:      //1  //数据总线节点发送路由通知的命令字
		return "CMD_ROUTER";
	case  CMD_DEL_NODE:    //2  //数据总线节点之间取消数据总线节点的命令字,只发送至主动连接的数据总线节点
		return "CMD_DEL_NODE";
	case  CMD_DEL_SVR:     //3  //服务端取消注册服务的命令字
		return "CMD_DEL_SVR";
	case  CMD_PING:        //4  //心跳请求的命令字
		return "CMD_PING";
	case  CMD_CONNECT:     //5  //连接注册
		return "CMD_CONNECT";
	case  CMD_SERVICE:     //6  //服务路由
		return "CMD_SERVICE";
	case  CMD_REGSERVICE:  //7  //注册服务
		return "CMD_REGSERVICE";			
	case  CMD_DPCALL:      //8  //数据总线节点同步调用 要求最终处理完成后应答
		return "CMD_DPCALL";
	case  CMD_DPACALL:     //9  //数据总线节点异步调用 要求接收到的数据总线节点发送至服务后确认应答
		return "CMD_DPACALL";
	case  CMD_DPBC:        //10  //数据总线节点广播，即将信息发给指定数据总线节点上所有注册的服务,要求数据总线节点应答
		return "CMD_DPBC";
	case  CMD_DPABC:       //11  //数据总线节点广播，即将信息发给指定数据总线节点上所有注册的服务,不要求数据总线节点应答
		return "CMD_DPABC";			
	case  CMD_DPPUSH:      //12 //数据总线节点推送，无须应答
		return "CMD_DPPUSH";
	case  CMD_DPPOST:      //13 //数据总线节点投递，要求接收到的数据总线节点应答
		return "CMD_DPPOST";			
	case  CMD_MONITOR_DREB:     //15 //数据总线节点监控
		return "CMD_MONITOR_DREB";
	case  CMD_MONITOR_BPC:     //15 //数据总线节点监控
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
		m_log.LogMp(LOG_INFO,__FILE__,__LINE__,"第%d步 时间[%I64d]",ret,m_vTime[ret]);
#else
		m_log.LogMp(LOG_INFO,__FILE__,__LINE__,"第%d步 时间[%lld]",ret,m_vTime[ret]);
#endif
	}
	m_vTime.clear();
}
