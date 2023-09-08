// BF_DrebResource.cpp: implementation of the CBF_DrebResource class.
//
//////////////////////////////////////////////////////////////////////

#include "BF_DrebResource.h"
#include "IErrLog.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBF_DrebResource::CBF_DrebResource()
{
	//日志配置
	g_nLoglevel = LOG_DEBUG;//日志级别 默认为5
	g_nDrebLoglevel = LOG_PROMPT;//写入data下面的日志级别
	g_nMaxLogSize = 0;

	g_nEndianFlag = 0;
	
	//心跳连接配置
	g_nDisconnectTime = 600;//未使用断开时间，如果一个连接在此时间内一直没有使用，则将此连接断开。单位秒默认600秒即10分钟
	g_nHeartRun = 5;//心跳时间,当发现连接超过此时间未用时，主动发送心跳，默认为5秒 
	
	//主动发送监控的信息
	g_nUseMonitor = 1;       //是否监控
	g_nMonitorHost=1;      //是否报告主机资源
	g_nMonitorDrebId=0;            //公共节点号
	g_nMonitorDrebPrivateId=0;     //私有节点号
	g_nMonitorSvrId=0;            //公共服务节点号
	g_nMonitorSvrPrivateId=0;     //私有服务节点号
	g_nMonitorTxCode=99001;           //监控交易码
	g_nQueueSize=1000;

	g_nCpuCore=0;
	//在总线上注册的公共服务号和私有服务号
	g_nSvrMainId=0;     //目标服务功能号 ,必须填写
	g_nSvrPrivateId=0;  //目标私有序号 0表示公共 >0为每个服务的序号，必须填写
	
	g_nBcSerialDeleteTime = 10;//重复的广播，10秒删除
	bzero(g_sStartDateTime,sizeof(g_sStartDateTime));  //不用填写，由程序运行的填
	
	//以下三项为调用者在main里调用GetModuleNamePath取得运行程序的信息
	bzero(g_sCurPath,sizeof(g_sCurPath));
	bzero(g_sModulePath,sizeof(g_sModulePath));
	bzero(g_sModuleName,sizeof(g_sModuleName));

	bzero(&g_DrebRcvSendStat,sizeof(g_DrebRcvSendStat));
	bzero(&g_DrebRcvSendSec,sizeof(g_DrebRcvSendSec));
	

}

CBF_DrebResource::~CBF_DrebResource()
{

}

void CBF_DrebResource::ReportRcv(unsigned int rcvbyte, unsigned int packnum)
{
	CBF_PMutex pp(&m_reportMutex);
	g_DrebRcvSendStat.g_nRcvBytes+=rcvbyte;
	g_DrebRcvSendStat.g_nRcvPackNum+=packnum;
}

void CBF_DrebResource::ReportSend(unsigned int sendbytes, unsigned int sendpack)
{
	CBF_PMutex pp(&m_reportMutex);
	g_DrebRcvSendStat.g_nSendBytes+=sendbytes;
	g_DrebRcvSendStat.g_nSendPackNum+=sendpack;
}
// bool CBF_DrebResource::Init(const char *confile)
// {
// 	if (confile == NULL)
// 	{
// 		printf("传入文件为空\n");
// 		m_errMsg = "传入文件为空";
// 		return false;
// 	}
// 	
// 	if (!m_pXml.FromFile(confile))
// 	{
// 		printf("传入文件非法,配置文件[%s]\n",confile);
// 		m_errMsg = "传入配置文件非法";
// 		return false;
// 	}
// 	CBF_Date_Time pdata;
// 	CBF_Tools::StringCopy(g_sStartDateTime,17,pdata.ToStringDT17().c_str());
// 
// 	int ret=0;
// 	std::string stmp;
// 	
// 	if (m_pXml.GetNodeValueByPath("package/head/公共配置/日志级别",false,g_nLoglevel) == NULL)
// 	{
// 		m_errMsg = "配置文件错,无[package/head/公共配置/日志级别]节点";
// 		return false;
// 	}
// 	if (g_nLoglevel <0 )
// 	{
// 		g_nLoglevel = LOG_DEBUG;//配置错则为5
// 	}
// 	if (m_pXml.GetNodeValueByPath("package/head/公共配置/DATA日志级别",false,g_nDrebLoglevel) == NULL)
// 	{
// 		m_errMsg = "配置文件错,无[package/head/公共配置/DATA日志级别]节点";
// 		return false;
// 	}
// 	if (g_nDrebLoglevel <0 )
// 	{
// 		g_nDrebLoglevel = LOG_DEBUG;//配置错则为5
// 	}
// 
// 	if (m_pXml.GetNodeValueByPath("package/head/公共配置/日志文件",false,g_sLogFileName) == NULL)
// 	{
// 		m_errMsg = "配置文件错,无[package/head/公共配置/日志文件]节点";
// 		return false;
// 	}
// 	if (m_pXml.GetNodeValueByPath("package/head/公共配置/日志目录",false,g_sLogFilePath) == NULL)
// 	{
// 		m_errMsg = "配置文件错,无[package/head/公共配置/日志目录]节点";
// 		g_sLogFilePath="";
// 	}
// 	
// 	if (m_pXml.GetNodeValueByPath("package/head/公共配置/日志大小",false,ret) == NULL)
// 	{
// 		m_errMsg = "配置文件错,无[package/head/公共配置/日志大小]节点";
// 		g_nMaxLogSize=0;
// 	}
// 	else
// 	{
// 		g_nMaxLogSize = ret;
// 	}
// 
// 	if (m_pXml.GetNodeValueByPath("package/head/公共配置/字节序处理",false,ret) == NULL)
// 	{
// 		m_errMsg = "配置文件错,无[package/head/公共配置/字节序处理]节点";
// 		g_nEndianFlag=0;
// 	}
// 	else
// 	{
// 		g_nEndianFlag = ret;
// 	}
// 	if (g_nEndianFlag != 1)
// 	{
// 		g_nEndianFlag = 0;
// 	}
// 	
// 	if (m_pXml.GetNodeValueByPath("package/head/BPC配置/公共服务号",false,ret) == NULL)
// 	{
// 		m_errMsg = "配置文件错,无[package/head/BPC配置/公共服务号]节点";
// 		return false;
// 	}
// 	if (ret<1)
// 	{
// 		m_errMsg = "配置文件错,无[package/head/BPC配置/公共服务号]节点配置不符";
// 		return false;
// 	}
// 	g_nSvrMainId = ret;
// 	if (m_pXml.GetNodeValueByPath("package/head/BPC配置/私有服务号",false,ret) == NULL)
// 	{
// 		m_errMsg = "配置文件错,无[package/head/BPC配置/私有服务号]节点";
// 		return false;
// 	}
// 	if (ret<1 || ret>254)
// 	{
// 		m_errMsg = "配置文件错,无[package/head/BPC配置/私有服务号]节点配置不符";
// 		return false;
// 	}
// 	g_nSvrPrivateId = ret;
// 
// 
// 	if (m_pXml.GetNodeValueByPath("package/head/BPC配置/DREB心跳时间",false,g_nHeartRun) == NULL)
// 	{
// 		m_errMsg = "配置文件,无[package/head/BPC配置/DREB心跳时间]节点";
// 		g_nHeartRun = 5;
// 	}
// 	if (m_pXml.GetNodeValueByPath("package/head/BPC配置/连接过期时间",false,g_nDisconnectTime) == NULL)
// 	{
// 		m_errMsg = "配置文件,无[package/head/BPC配置/连接过期时间]节点";
// 		g_nDisconnectTime = 600;
// 	}
// 	g_nUseMonitor=0;
// 	if (m_pXml.GetNodeAttribute("package/head/监控配置",false,"use",false,g_nUseMonitor) == NULL)
// 	{
// 		m_errMsg = "无package/head/监控配置";
// 		g_nUseMonitor = 0;
// 	}
// 	if (g_nUseMonitor == 1)
// 	{
// 		if (m_pXml.GetNodeAttribute("package/head/监控配置",false,"host",false,g_nMonitorHost) == NULL)
// 		{
// 			m_errMsg = "无package/head/监控配置";
// 			g_nMonitorHost = 1;
// 		}
// 		if (m_pXml.GetNodeValueByPath("package/head/监控配置/监控公共节点号",false,g_nMonitorDrebId) == NULL)
// 		{
// 			m_errMsg = "节点[package/head/监控配置/监控公共节点号]没有配置";
// 			g_nMonitorDrebId = 0;
// 		}
// 		if (m_pXml.GetNodeValueByPath("package/head/监控配置/监控私有节点号",false,g_nMonitorDrebPrivateId) == NULL)
// 		{
// 			m_errMsg = "节点[package/head/监控配置/监控私有节点号]没有配置";
// 			g_nMonitorDrebPrivateId = 0;
// 		}
// 		if (m_pXml.GetNodeValueByPath("package/head/监控配置/监控公共服务号",false,g_nMonitorSvrId) == NULL)
// 		{
// 			m_errMsg = "节点[package/head/监控配置/监控公共服务号]没有配置";
// 			g_nMonitorSvrId = 0;
// 		}
// 		if (m_pXml.GetNodeValueByPath("package/head/监控配置/监控私有服务号",false,g_nMonitorSvrPrivateId) == NULL)
// 		{
// 			m_errMsg = "节点[package/head/监控配置/监控私有服务号]没有配置";
// 			g_nMonitorSvrPrivateId = 0;
// 		}
// 		if (m_pXml.GetNodeValueByPath("package/head/监控配置/监控报告交易码",false,g_nMonitorTxCode) == NULL)
// 		{
// 			m_errMsg = "节点[package/head/监控配置/监控报告交易码]没有配置";
// 			return false;
// 		}
// 	}
// 	CXmlNode *drebnode = m_pXml.GetNodeByPath("package/head/DREB配置",false);
// 	if (drebnode == NULL)
// 	{
// 		m_errMsg = "配置文件错,无[package/head/DREB配置]节点";
// 		return false;
// 	}
// 	CXmlNode *tmpnode = (CXmlNode *)drebnode->GetFirstChild();
// 	S_DREBLINK_INFO dreblinkinfo;
// 	while (tmpnode != NULL)
// 	{
// 		tmpnode->GetAttribute("ip",false,dreblinkinfo.sIp);
// 		tmpnode->GetAttribute("port",false,stmp);
// 		dreblinkinfo.nPort = atoi(stmp.c_str());
// 		tmpnode->GetAttribute("bandwidth",false,stmp);
// 		dreblinkinfo.nBandwidth = atoi(stmp.c_str());
// 		g_vDrebLinkInfo.push_back(dreblinkinfo);
// 		tmpnode = (CXmlNode *)tmpnode->getNextSibling();
// 	}
// 	return true;
// }

void CBF_DrebResource::ComputeStat()
{
	CBF_PMutex pp(&m_reportMutex);
	//计算
	g_DrebRcvSendSec.g_nRcvBytes = g_DrebRcvSendStat.g_nRcvBytes  - m_oldDrebStatInfo.g_nRcvBytes;
	g_DrebRcvSendSec.g_nRcvPackNum =g_DrebRcvSendStat.g_nRcvPackNum  - m_oldDrebStatInfo.g_nRcvPackNum;
	g_DrebRcvSendSec.g_nSendBytes =g_DrebRcvSendStat.g_nSendBytes  - m_oldDrebStatInfo.g_nSendBytes;
	g_DrebRcvSendSec.g_nSendPackNum =g_DrebRcvSendStat.g_nSendPackNum  - m_oldDrebStatInfo.g_nSendPackNum;
	//保存到旧的结构
	memcpy(&m_oldDrebStatInfo,&(g_DrebRcvSendStat),sizeof(S_DREBSTAT_INFO));
}
