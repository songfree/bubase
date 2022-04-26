// Resource.cpp: implementation of the CResource class.
//
//////////////////////////////////////////////////////////////////////

#include "Resource.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CResource::CResource()
{
	g_bIsExit = false;
	bzero(g_sCurPath,sizeof(g_sCurPath));
	bzero(g_sModuleName,sizeof(g_sCurPath));
	bzero(g_sModulePath,sizeof(g_sModulePath));
	bzero(m_sStartDate,sizeof(m_sStartDate));

	g_nGwDisTime = 6000;
	g_nHeartRun = 5;
	g_nDisconnectTime = 60;
	g_nPrintRSData = 0;
	g_nCrcFlag = 0;
}

CResource::~CResource()
{

}
bool CResource::Init(const char *confile)
{
	if (!g_pXml.FromFile(confile))
	{
		sprintf(m_sErrMsg,"配置文件[%s]不存在或非xml",confile);
		return false;
	}
	m_pDate.Update();
	
	CBF_Tools::StringCopy(m_sStartDate,17,m_pDate.ToStringDT17().c_str());
	
	int ret=0;
	std::string stmp;
	
	if (g_pXml.GetNodeValueByPath("package/head/public/loglevel",false,g_nLoglevel) == NULL)
	{
		sprintf(m_sErrMsg,"配置文件错,无[package/head/public/loglevel]节点");
		return false;
	}
	if (g_nLoglevel <0 )
	{
		g_nLoglevel = LOG_DEBUG;//配置错则为5
	}
	
	if (g_pXml.GetNodeValueByPath("package/head/public/logfile",false,g_sLogFileName) == NULL)
	{
		sprintf(m_sErrMsg, "配置文件错,无[package/head/public/logfile]节点");
		return false;
	}
	if (g_pXml.GetNodeValueByPath("package/head/public/logdir",false,g_sLogFilePath) == NULL)
	{
		sprintf(m_sErrMsg, "配置文件错,无[package/head/public/logdir]节点");
		g_sLogFilePath="";
	}
	g_pLog.SetLogPara(g_nLoglevel,g_sLogFilePath.c_str(),g_sLogFileName.c_str());

    if (g_pXml.GetNodeValueByPath("package/head/public/CRC", false, g_nCrcFlag) == NULL)
    {
		sprintf(m_sErrMsg, "配置文件错,无[package/head/public/CRC]节点");
        g_nCrcFlag = 0;
    }

	if (g_pXml.GetNodeValueByPath("package/head/public/logtrans",false,g_nPrintRSData) == NULL)
	{
		sprintf(m_sErrMsg, "配置文件错,无[package/head/public/logtrans]节点,默认为0不打");
		g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,m_sErrMsg);
		g_nPrintRSData = 0;
	}
	
	if (g_pXml.GetNodeValueByPath("package/head/bpc/svrid",false,ret) == NULL)
	{
		sprintf(m_sErrMsg, "配置文件错,无[package/head/bpc/svrid]节点");
		g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	if (ret<1)
	{
		sprintf(m_sErrMsg, "配置文件错,无[package/head/bpc/svrpid]节点配置不符");
		g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	g_nSvrMainId = ret;
	if (g_pXml.GetNodeValueByPath("package/head/bpc/svrpid",false,ret) == NULL)
	{
		sprintf(m_sErrMsg, "配置文件错,无[package/head/bpc/svrpid]节点");
		g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	if (ret<1 || ret>254)
	{
		sprintf(m_sErrMsg, "配置文件错,无[package/head/bpc/svrpid]节点配置不符");
		g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	g_nSvrPrivateId = ret;
	
	
	if (g_pXml.GetNodeValueByPath("package/head/bpc/hearttime",false,g_nHeartRun) == NULL)
	{
		sprintf(m_sErrMsg, "配置文件,无[package/head/bpc/hearttime]节点");
		g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,m_sErrMsg);
		g_nHeartRun = 5;
	}
	if (g_pXml.GetNodeValueByPath("package/head/bpc/due_times",false,g_nDisconnectTime) == NULL)
	{
		sprintf(m_sErrMsg, "配置文件,无[package/head/bpc/due_times]节点");
		g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,m_sErrMsg);
		g_nDisconnectTime = 60;
	}
	if (g_pXml.GetNodeValueByPath("package/head/bpc/transtimeout",false,g_nGwDisTime) == NULL)
	{
		sprintf(m_sErrMsg, "配置文件,无[package/head/bpc/transtimeout]节点");
		g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,m_sErrMsg);
		g_nGwDisTime = 60000;
	}
	g_nUseMonitor=0;
	if (g_pXml.GetNodeAttribute("package/head/monitor",false,"use",false,g_nUseMonitor) == NULL)
	{
		sprintf(m_sErrMsg, "无package/head/monitor");
		g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_sErrMsg);
		g_nUseMonitor = 0;
	}
	if (g_nUseMonitor == 1)
	{
		if (g_pXml.GetNodeAttribute("package/head/monitor",false,"host",false,g_nMonitorHost) == NULL)
		{
			sprintf(m_sErrMsg, "无package/head/monitor");
			g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_sErrMsg);
			g_nMonitorHost = 1;
		}
		if (g_pXml.GetNodeValueByPath("package/head/monitor/nodeid",false,g_nMonitorDrebId) == NULL)
		{
			sprintf(m_sErrMsg, "节点[package/head/monitor/nodeid]没有配置");
			g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_sErrMsg);
			g_nMonitorDrebId = 0;
		}
		if (g_pXml.GetNodeValueByPath("package/head/monitor/nodepid",false,g_nMonitorDrebPrivateId) == NULL)
		{
			sprintf(m_sErrMsg, "节点[package/head/monitor/nodepid]没有配置");
			g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_sErrMsg);
			g_nMonitorDrebPrivateId = 0;
		}
		if (g_pXml.GetNodeValueByPath("package/head/monitor/svrid",false,g_nMonitorSvrId) == NULL)
		{
			sprintf(m_sErrMsg, "节点[package/head/monitor/svrid]没有配置");
			g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_sErrMsg);
			g_nMonitorSvrId = 0;
		}
		if (g_pXml.GetNodeValueByPath("package/head/monitor/svrpid",false,g_nMonitorSvrPrivateId) == NULL)
		{
			sprintf(m_sErrMsg, "节点[package/head/monitor/svrpid]没有配置");
			g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_sErrMsg);
			g_nMonitorSvrPrivateId = 0;
		}
		if (g_pXml.GetNodeValueByPath("package/head/monitor/txcode",false,g_nMonitorTxCode) == NULL)
		{
			sprintf(m_sErrMsg, "节点[package/head/monitor/txcode]没有配置");
			g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
	}
	CXmlNode *tmpnode = NULL;
	CXmlNode *drebnode = g_pXml.GetNodeByPath("package/head/dreb",false);
	if (drebnode != NULL)
	{
		tmpnode = (CXmlNode *)drebnode->GetFirstChild();
		S_DREBLINK_INFO dreblinkinfo;
		while (tmpnode != NULL)
		{
			if (tmpnode->GetAttribute("ip",false,dreblinkinfo.sIp) == NULL)
			{
				g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"%s 无ip属性",tmpnode->GetNodeName().c_str());
				tmpnode = (CXmlNode *)tmpnode->getNextSibling();
				continue;
			}
			if (tmpnode->GetAttribute("port",false,stmp) == NULL)
			{
				g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"%s 无port属性",tmpnode->GetNodeName().c_str());
				tmpnode = (CXmlNode *)tmpnode->getNextSibling();
				continue;
			}
			dreblinkinfo.nPort = atoi(stmp.c_str());
			tmpnode->GetAttribute("bandwidth",false,stmp);
			dreblinkinfo.nBandwidth = atoi(stmp.c_str());
			g_vDrebLinkInfo.push_back(dreblinkinfo);
			tmpnode = (CXmlNode *)tmpnode->getNextSibling();
		}
	}
	else
	{
		sprintf(m_sErrMsg, "节点[package/head/dreb]没有配置");
		g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_sErrMsg);
	}
	CXmlNode *spnode = g_pXml.GetNodeByPath("package/head/gateway",false);
	if (spnode != NULL)
	{
		tmpnode = (CXmlNode *)spnode->GetFirstChild();
		S_TRANSCONF strans;
		while (tmpnode != NULL)
		{
			if (tmpnode->GetAttribute("INPUTPORT",false,stmp) == NULL)
			{
				g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"%s 无INPUTPORT属性",tmpnode->GetNodeName().c_str());
				tmpnode = (CXmlNode *)tmpnode->getNextSibling();
				continue;
			}
			strans.inport = atoi(stmp.c_str());
			if (tmpnode->GetAttribute("OUTPUTIP",false,stmp)== NULL)
			{
				g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"%s 无OUTPUTIP属性",tmpnode->GetNodeName().c_str());
				tmpnode = (CXmlNode *)tmpnode->getNextSibling();
				continue;
			}
			CBF_Tools::StringCopy(strans.outip,16,stmp.c_str());
			if (tmpnode->GetAttribute("OUTPUTPORT",false,stmp) == NULL)
			{
				g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"%s 无OUTPUTPORT属性",tmpnode->GetNodeName().c_str());
				tmpnode = (CXmlNode *)tmpnode->getNextSibling();
				continue;
			}
			strans.outport = atoi(stmp.c_str());
			m_gwList.push_back(strans);
			tmpnode = (CXmlNode *)tmpnode->getNextSibling();
		}
	}
	else
	{
		sprintf(m_sErrMsg, "节点[package/head/gateway]没有配置");
		g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_sErrMsg);
	}
	int nId;
	CXmlNode *refusenode = g_pXml.GetNodeByPath("package/head/blacklist",false);
	if (refusenode != NULL)
	{
		tmpnode = (CXmlNode *)refusenode->GetFirstChild();
		while (tmpnode != NULL)
		{
			if (tmpnode->GetAttribute("IP",false,stmp) == NULL)
			{
				g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"%s 无IP属性",tmpnode->GetNodeName().c_str());
				tmpnode = (CXmlNode *)tmpnode->getNextSibling();
				continue;
			}
			if (!IsRefuse(stmp.c_str()))
			{
				nId = m_table.Add(stmp);
				m_pKey.Add(nId,(char *)stmp.c_str());
			}
			else
			{
				g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"%s IP[%s]重复",tmpnode->GetNodeName().c_str(),stmp.c_str());
			}
			tmpnode = (CXmlNode *)tmpnode->getNextSibling();
		}
	}
	else
	{
		sprintf(m_sErrMsg, "节点[package/head/blacklist]转发黑名单未配置");
		g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_sErrMsg);
	}
	return true;
}

bool CResource::IsRefuse(const char *ip)
{
	CInt nSet;
	return m_pKey.Select(nSet,(char *)ip);
}
