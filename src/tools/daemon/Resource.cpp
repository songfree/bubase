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
	g_nCrcFlag = 0;

}

CResource::~CResource()
{

}
bool CResource::Init(const char *confile)
{
	char tmpchar[300];
	sprintf(tmpchar,"%s/monirun.xml",g_sCurPath);
	g_pRunXml.FromFile(tmpchar);

	if (!g_pXml.FromFile(confile))
	{
		sprintf(m_sErrMsg,"�����ļ�[%s]�����ڻ��xml",confile);
		return false;
	}
	m_pDate.Update();
	
	CBF_Tools::StringCopy(m_sStartDate,17,m_pDate.ToStringDT17().c_str());
	
	int ret=0;
	std::string stmp;
	
	if (g_pXml.GetNodeValueByPath("package/head/public/loglevel",false,g_nLoglevel) == NULL)
	{
		sprintf(m_sErrMsg,"�����ļ���,��[package/head/public/loglevel]�ڵ�");
		return false;
	}
	if (g_nLoglevel <0 )
	{
		g_nLoglevel = LOG_DEBUG;//���ô���Ϊ5
	}
	
	if (g_pXml.GetNodeValueByPath("package/head/public/logfile",false,g_sLogFileName) == NULL)
	{
		sprintf(m_sErrMsg, "�����ļ���,��[package/head/public/logfile]�ڵ�");
		return false;
	}
	if (g_pXml.GetNodeValueByPath("package/head/public/logdir",false,g_sLogFilePath) == NULL)
	{
		sprintf(m_sErrMsg, "�����ļ���,��[package/head/public/logdir]�ڵ�");
		g_sLogFilePath="";
	}
	g_pLog.SetLogPara(g_nLoglevel,g_sLogFilePath.c_str(),g_sLogFileName.c_str());
	
	
	if (g_pXml.GetNodeValueByPath("package/head/bpc/svrid",false,ret) == NULL)
	{
		sprintf(m_sErrMsg, "�����ļ���,��[package/head/bpc/svrid]�ڵ�");
		g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	if (ret<1)
	{
		sprintf(m_sErrMsg, "�����ļ���,��[package/head/bpc/svrid]�ڵ����ò���");
		g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	g_nSvrMainId = ret;
	if (g_pXml.GetNodeValueByPath("package/head/bpc/svrpid",false,ret) == NULL)
	{
		sprintf(m_sErrMsg, "�����ļ���,��[package/head/bpc/svrpid]�ڵ�");
		g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	if (ret<1 || ret>254)
	{
		sprintf(m_sErrMsg, "�����ļ���,��[package/head/bpc/svrpid]�ڵ����ò���");
		g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	g_nSvrPrivateId = ret;
	
	
	if (g_pXml.GetNodeValueByPath("package/head/bpc/CRC",false,g_nCrcFlag) == NULL)
	{
		g_nCrcFlag=0;
	}

	if (g_pXml.GetNodeValueByPath("package/head/bpc/hearttime",false,g_nHeartRun) == NULL)
	{
		sprintf(m_sErrMsg, "�����ļ�,��[package/head/bpc/hearttime]�ڵ�");
		g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,m_sErrMsg);
		g_nHeartRun = 5;
	}
	if (g_pXml.GetNodeValueByPath("package/head/bpc/due_times",false,g_nDisconnectTime) == NULL)
	{
		sprintf(m_sErrMsg, "�����ļ�,��[package/head/bpc/due_times]�ڵ�");
		g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,m_sErrMsg);
		g_nDisconnectTime = 60;
	}
	if (g_pXml.GetNodeValueByPath("package/head/bpc/connect_timeout",false,g_nGwDisTime) == NULL)
	{
		sprintf(m_sErrMsg, "�����ļ�,��[package/head/bpc/connect_timeout]�ڵ�");
		g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,m_sErrMsg);
		g_nGwDisTime = 6000;
	}
	if (g_pXml.GetNodeValueByPath("package/head/bpc/heartport",false,g_nPort) == NULL)
	{
		sprintf(m_sErrMsg, "�����ļ�,��[package/head/bpc/heartport]�ڵ�");
		g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}

	g_nUseMonitor=0;
	if (g_pXml.GetNodeAttribute("package/head/monitor",false,"use",false,g_nUseMonitor) == NULL)
	{
		sprintf(m_sErrMsg, "��package/head/monitor");
		g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_sErrMsg);
		g_nUseMonitor = 0;
	}
	if (g_nUseMonitor == 1)
	{
		if (g_pXml.GetNodeAttribute("package/head/monitor",false,"host",false,g_nMonitorHost) == NULL)
		{
			sprintf(m_sErrMsg, "��package/head/monitor");
			g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_sErrMsg);
			g_nMonitorHost = 1;
		}
		if (g_pXml.GetNodeValueByPath("package/head/monitor/nodeid",false,g_nMonitorDrebId) == NULL)
		{
			sprintf(m_sErrMsg, "�ڵ�[package/head/monitor/nodeid]û������");
			g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_sErrMsg);
			g_nMonitorDrebId = 0;
		}
		if (g_pXml.GetNodeValueByPath("package/head/monitor/nodepid",false,g_nMonitorDrebPrivateId) == NULL)
		{
			sprintf(m_sErrMsg, "�ڵ�[package/head/monitor/nodepid]û������");
			g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_sErrMsg);
			g_nMonitorDrebPrivateId = 0;
		}
		if (g_pXml.GetNodeValueByPath("package/head/monitor/svrid",false,g_nMonitorSvrId) == NULL)
		{
			sprintf(m_sErrMsg, "�ڵ�[package/head/monitor/svrid]û������");
			g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_sErrMsg);
			g_nMonitorSvrId = 0;
		}
		if (g_pXml.GetNodeValueByPath("package/head/monitor/svrpid",false,g_nMonitorSvrPrivateId) == NULL)
		{
			sprintf(m_sErrMsg, "�ڵ�[package/head/monitor/svrpid]û������");
			g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_sErrMsg);
			g_nMonitorSvrPrivateId = 0;
		}
		if (g_pXml.GetNodeValueByPath("package/head/monitor/txcode",false,g_nMonitorTxCode) == NULL)
		{
			sprintf(m_sErrMsg, "�ڵ�[package/head/monitor/txcode]û������");
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
				g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"%s ��ip����",tmpnode->GetNodeName().c_str());
				tmpnode = (CXmlNode *)tmpnode->getNextSibling();
				continue;
			}
			if (tmpnode->GetAttribute("port",false,stmp) == NULL)
			{
				g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"%s ��port����",tmpnode->GetNodeName().c_str());
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
		sprintf(m_sErrMsg, "�ڵ�[package/head/dreb]û������");
		g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_sErrMsg);
	}
	
	S_PROCESS_NODE pnode;
	CXmlNode *mgrnode = g_pXml.GetNodeByPath("package/head/daemon",false);
	if (mgrnode != NULL)
	{
		
		tmpnode = (CXmlNode *)mgrnode->GetFirstChild();
		if (tmpnode == NULL)
		{
			sprintf(m_sErrMsg, "�ڵ�[package/head/daemon]û�����ý�����Ϣ");
			g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_sErrMsg);
			return true;
		}
		while (tmpnode != NULL)
		{
			pnode.nodeid = tmpnode->GetNodeName();//����ID
			sprintf(tmpchar,"package/head/daemon/%s/name",pnode.nodeid.c_str());
			if (g_pXml.GetNodeValueByPath(tmpchar,false,pnode.processname) == NULL)
			{
				sprintf(m_sErrMsg, "�ڵ�[%s]û������",tmpchar);
				g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_sErrMsg);
				tmpnode = (CXmlNode *)tmpnode->getNextSibling();
				continue;
			}
			sprintf(tmpchar,"package/head/daemon/%s/prgname",pnode.nodeid.c_str());
			if (g_pXml.GetNodeValueByPath(tmpchar,false,pnode.prgname) == NULL)
			{
				sprintf(m_sErrMsg, "�ڵ�[%s]û������",tmpchar);
				g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_sErrMsg);
				tmpnode = (CXmlNode *)tmpnode->getNextSibling();
				continue;
			}
			sprintf(tmpchar,"package/head/daemon/%s/prgpath",pnode.nodeid.c_str());
			if (g_pXml.GetNodeValueByPath(tmpchar,false,pnode.prgpath) == NULL)
			{
				sprintf(m_sErrMsg, "�ڵ�[%s]û������",tmpchar);
				g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_sErrMsg);
				tmpnode = (CXmlNode *)tmpnode->getNextSibling();
				continue;
			}
			sprintf(tmpchar,"package/head/daemon/%s/runpath",pnode.nodeid.c_str());
			if (g_pXml.GetNodeValueByPath(tmpchar,false,pnode.runpath) == NULL)
			{
				sprintf(m_sErrMsg, "�ڵ�[%s]û������",tmpchar);
				g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_sErrMsg);
				tmpnode = (CXmlNode *)tmpnode->getNextSibling();
				continue;
			}
			sprintf(tmpchar,"package/head/daemon/%s/startpara",pnode.nodeid.c_str());
			if (g_pXml.GetNodeValueByPath(tmpchar,false,pnode.prgpara) == NULL)
			{
				sprintf(m_sErrMsg, "�ڵ�[%s]û������",tmpchar);
				g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_sErrMsg);
				tmpnode = (CXmlNode *)tmpnode->getNextSibling();
				continue;
			}
			sprintf(tmpchar,"package/head/daemon/%s/stopshell",pnode.nodeid.c_str());
			if (g_pXml.GetNodeValueByPath(tmpchar,false,pnode.stopscript) == NULL)
			{
				sprintf(m_sErrMsg, "�ڵ�[%s]û������",tmpchar);
				g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_sErrMsg);
				tmpnode = (CXmlNode *)tmpnode->getNextSibling();
				continue;
			}
			sprintf(tmpchar,"package/head/daemon/%s/restarnoheart",pnode.nodeid.c_str());
			if (g_pXml.GetNodeValueByPath(tmpchar,false,pnode.heartstart) == NULL)
			{
				sprintf(m_sErrMsg, "�ڵ�[%s]û������",tmpchar);
				g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_sErrMsg);
				tmpnode = (CXmlNode *)tmpnode->getNextSibling();
				continue;
			}
			sprintf(tmpchar,"package/head/daemon/%s/iscontrol",pnode.nodeid.c_str());
			if (g_pXml.GetNodeValueByPath(tmpchar,false,pnode.acceptcontrol) == NULL)
			{
				sprintf(m_sErrMsg, "�ڵ�[%s]û������",tmpchar);
				g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_sErrMsg);
				tmpnode = (CXmlNode *)tmpnode->getNextSibling();
				continue;
			}
			sprintf(tmpchar,"package/head/daemon/%s/windows_console",pnode.nodeid.c_str());
			if (g_pXml.GetNodeValueByPath(tmpchar,false,pnode.winconsole) == NULL)
			{
				sprintf(m_sErrMsg, "�ڵ�[%s]û������,Ĭ����ʾ",tmpchar);
				g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_sErrMsg);
				pnode.winconsole = 1;
			}
			pnode.pid = -1;
			pnode.status = "4";
			
			//ȡ�ý�����Ϣ
			sprintf(tmpchar,"daemon/%s/pid",pnode.nodeid.c_str());
			g_pRunXml.GetNodeValueByPath(tmpchar,false,pnode.pid);
			pnode.lastUseTIme = time(NULL)+300;//�����ʹ��ʱ��Ϊ300��󣬷�ֹ��������ʱ��ϳ���ͣ������
			g_vProcessInfo.push_back(pnode);
			tmpnode = (CXmlNode *)tmpnode->getNextSibling();
		}
		
	}
	else
	{
		sprintf(m_sErrMsg, "�ڵ�[package/head/daemon]û������");
		g_pLog.LogMp(LOG_PROMPT,__FILE__,__LINE__,m_sErrMsg);
	}
	return true;
}

bool CResource::UpdateLastTimeById(std::string nodeid)
{
	for (int i=0 ; i< g_vProcessInfo.size(); i++)
	{
		if (g_vProcessInfo[i].nodeid.compare(nodeid) == 0)
		{
			g_vProcessInfo[i].lastUseTIme = time(NULL);
			return true;
		}
	}
	return false;
}
