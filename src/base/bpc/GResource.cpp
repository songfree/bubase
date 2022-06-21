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
	m_log = NULL;
	g_bToExit = false;
	m_serial_connect = 0;
	g_nPort = 9001;//侦听端口 默认为9001
	
	g_nDispatchTime = 10;
	
	g_sRegisterFile = "";
	g_nRegisterMode = 0;

	g_bIsInitOnce = false; //是否初始化一次，即在一个线程里进行初始化后，将此标志置为1，其它线程判断此标志是否进行初始化
	g_bIsListen = false;//只能侦听一次，侦听完后将此标志置为true,其它线程将不用侦听

	m_nSerialNum = 1;
	int ap = sizeof(m_serial_connect)*8-1; 
	m_nMaxSerial = (unsigned long)pow(2.0,ap);

	g_processSucessNum=0;
	g_processNum = 0;

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

bool CGResource::Init(char *confile, CIErrlog* log)
{
	m_sBpcParaFileName = confile;
	m_log = log;
	if (confile == NULL)
	{
		m_errMsg = "传入文件为空";
		return false;
	}
	if (!CBF_DrebResource::Init(confile))
	{
		printf("总线配置初始化出错 %s\n", confile);
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
	int ret=0;
	std::string stmp;

	if (m_xmlconf.GetNodeValueByPath("package/head/bpc/dispatchtime",false,g_nDispatchTime) == NULL)
	{
		m_errMsg = "配置文件,无[package/head/dispatchtime/dispatchtime]节点";
		m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"%s",m_errMsg.c_str());
		g_nDispatchTime = 10;
	}
	if (m_xmlconf.GetNodeValueByPath("package/head/bpc/regmode",false,g_nRegisterMode) == NULL)
	{
		m_errMsg = "配置文件,无[package/head/bpc/regmode]节点";
		m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"%s",m_errMsg.c_str());
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

	if (m_xmlconf.GetNodeValueByPath("package/head/bpc/maxserial",false,m_nSerial_getMax) == NULL)
	{
		m_errMsg = "配置文件,无[package/head/bpc/maxserial]节点";
		m_log->LogMp(LOG_WARNNING,__FILE__,__LINE__,"%s",m_errMsg.c_str());
		m_nSerial_getMax = 99999999;
	}
	
	CXmlNode *drebnode = m_xmlconf.GetNodeByPath("package/head/dreb",false);
	if (drebnode == NULL)
	{
		m_errMsg = "配置文件错,无[package/head/dreb]节点";
		m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"%s",m_errMsg.c_str());
		return false;
	}
	
	if (m_xmlconf.GetNodeValueByPath("package/head/bpu/bpuport",false,g_nPort) == NULL)
	{
		m_errMsg = "配置文件错,无[package/head/bpu/bpuport]节点";
		m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"%s",m_errMsg.c_str());
		return false;
	}
	if (g_nPort <= 1024 || g_nPort>65534)
	{
		m_errMsg = "配置文件错,[package/head/bpu/bpuport]节点配置不合法";
		m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"%s",(char *)m_errMsg.c_str());
		return false;
	}
	
	char tmpchar[600];
	S_BPUGROUP_INFO bpugroupinfo;

	CXmlNode *bpugroupnode = m_xmlconf.GetNodeByPath("package/head/bpu/bpugroup",false);
	if (bpugroupnode == NULL)
	{
		m_errMsg = "配置文件错 无[package/head/bpu/bpugroup]节点";
		m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"%s",(char *)m_errMsg.c_str());
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
			m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
			return false;
		}
		sprintf(tmpchar,"package/head/bpu/bpugroup/%s/dispatchmode",bpugroupinfo.g_sBpuGroupName.c_str());
		if (m_xmlconf.GetNodeValueByPath(tmpchar,true,stmp,true) == NULL)
		{
			m_errMsg = std::string("无[") + tmpchar +"]节点";
			m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
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
				m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
				return false;
			}
			bpugroupinfo.g_nBpuNum = atoi(stmp.c_str());
		}
		sprintf(tmpchar,"package/head/bpu/bpugroup/%s/endian",bpugroupinfo.g_sBpuGroupName.c_str());
		if (m_xmlconf.GetNodeValueByPath(tmpchar,true,stmp,true) == NULL)
		{
			m_errMsg = std::string("无[") + tmpchar +"]节点";
			m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s 默认使用主机序",(char *)m_errMsg.c_str());
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
			m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
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
			m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
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
			m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
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
				m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
				return false;
			}
			sprintf(tmpchar,"package/head/bpu/bpugroup/%s/autoloadbpu/bpuprgpath",bpugroupinfo.g_sBpuGroupName.c_str());
			if (m_xmlconf.GetNodeValueByPath(tmpchar,true,bpugroupinfo.g_sBuPrgPath,true) == NULL)
			{
				m_errMsg = std::string("无[") + tmpchar +"]节点";
				m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
				return false;
			}
			sprintf(tmpchar,"package/head/bpu/bpugroup/%s/autoloadbpu/bpurunpath",bpugroupinfo.g_sBpuGroupName.c_str());
			if (m_xmlconf.GetNodeValueByPath(tmpchar,true,bpugroupinfo.g_sBuRunPath,true) == NULL)
			{
				m_errMsg = std::string("无[") + tmpchar +"]节点";
				m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
				return false;
			}
			sprintf(tmpchar,"package/head/bpu/bpugroup/%s/autoloadbpu/bpupara",bpugroupinfo.g_sBpuGroupName.c_str());
			if (m_xmlconf.GetNodeValueByPath(tmpchar,true,bpugroupinfo.g_sBuPrgPara,true) == NULL)
			{
				m_errMsg = std::string("无[") + tmpchar +"]节点";
				m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
				return false;
			}
		}
		
		bzero(&tgtbl,sizeof(S_BPUGROUP_TBL));
		tgtbl.bpugroupindex = bpugroupindex;
		strcpy(tgtbl.bpugroupname,bpugroupinfo.g_sBpuGroupName.c_str());
		if (m_pkey.Find(tgtbl.bpugroupname))
		{
			m_errMsg = std::string("BPU组名称重复:")+bpugroupinfo.g_sBpuGroupName;
			m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
			return false;
		}
		bpugroupinfo.g_bIsReg = false;
		bpugroupinfo.g_nJavaBpuPid = 0;
		bpugroupinfo.pdispathQueue = new CPoolData();
		if (bpugroupinfo.pdispathQueue == NULL)
		{
			m_errMsg = std::string("new CPoolData出错")+bpugroupinfo.g_sBpuGroupName;
			m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"%s",(char *)m_errMsg.c_str());
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
    //广播配置

    CXmlNode* bcnode = m_xmlconf.GetNodeByPath("package/head/broadcast", false);
    if (bcnode != NULL)
    {
        CXmlNode* bcfuncnode = (CXmlNode*)bcnode->GetFirstChild();
        while (bcfuncnode != NULL)
        {
            g_lBCFuncList.push_back(atoi(bcfuncnode->GetNodeValue().c_str()));
            bcfuncnode = (CXmlNode*)bcfuncnode->getNextSibling();
        }
    }
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
			m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"打开动态库失败 %s",dllpathname);
			return false;
		}
		sprintf(dllpathname,"%s",dllname);
		CBF_Tools::Rtrim(dllpathname);
		*pDll = LOADDLL(dllpathname);
		if (ISNOTOPEN(*pDll))
		{
			m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"打开动态库失败 %s",dllpathname);
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
			m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"打开动态库失败 %s",dllpathname);
			return false;
		}
		m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"打开动态库失败 %s %s",dllpathname,pdllerror);
		sprintf(dllpathname,"%s",dllname);
		CBF_Tools::Rtrim(dllpathname);
		*pDll = LOADDLL(dllpathname);
		pdllerror = dlerror();
		if (pdllerror!=NULL)
		{
			m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"打开动态库失败 %s %s",dllpathname,pdllerror);
			return false;
		}
		m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"打开动态库成功 %s",dllpathname);
		
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
    if (1 == g_nUseMonitor)
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
	if (1 == g_nUseMonitor)
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

void CGResource::Stop()
{
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
		m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"第%d步 时间[%I64d]",ret,m_vTime[ret]);
#else
		m_log->LogMp(LOG_INFO,__FILE__,__LINE__,"第%d步 时间[%lld]",ret,m_vTime[ret]);
#endif
	}
	m_vTime.clear();
}
