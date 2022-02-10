// GResource.cpp: implementation of the CGResource class.
//
//////////////////////////////////////////////////////////////////////

#include "BuRes.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBuRes::CBuRes()
{

	g_nPort = 9001;//侦听端口 默认为9001
	g_nLoglevel = 5;//日志级别 默认为5
	m_pLink = NULL;
	g_bIsTxMainFunc = false;
	g_nDisconnectTime = 60000;
	g_nXmlNodeMode = 0;
}

CBuRes::~CBuRes()
{
	WriteBack();
}


bool CBuRes::Init(char *confile)
{
	if (confile == NULL)
	{
		m_errMsg = "传入文件为空";
		return false;
	}
	if (m_xmlconf.fromFile(confile)<0)
	{
		printf("传入文件非法,配置文件[%s]\n",confile);
		m_errMsg = "传入配置文件非法";
		return false;
	}

	
	if (m_xmlconf.gotoHead()<0)
	{
		m_errMsg = "初始化传入的XML节点head为NULL";
		return false;
	}

	int ret=0;
	std::string stmp;
	CNode *pNode =NULL;
	CNode *pChildNode =NULL;
	pNode = m_xmlconf.m_xml.getFirstChildItem("public");
	if (pNode == NULL)
	{
		m_errMsg = "配置文件错,head节点下无[public]节点";
		return false;
	}
	pChildNode = m_xmlconf.m_xml.getFirstChildItem("loglevel");
	if (pChildNode == NULL)
	{
		m_errMsg = "配置文件错,head节点下的public节点无[loglevel]节点";
		return false;
	}
	g_nLoglevel = atoi(pChildNode->getValue().c_str());
	if (g_nLoglevel <0 )
	{
		g_nLoglevel = LOG_DEBUG;//配置错则为5
	}
	m_xmlconf.m_xml.setCurrentNode(pNode);
	pChildNode = m_xmlconf.m_xml.getFirstChildItem("logfile");
	if (pChildNode == NULL)
	{
		m_errMsg = "配置文件错,head节点下的public节点无[logfile]节点";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
		return false;
	}
	g_sLogFileName = pChildNode->getValue();
	if (g_sLogFileName.length()<1)
	{
		m_errMsg = "配置文件错,head节点下的public节点下[logfile]节点没有配置";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
		return false;
	}

	m_xmlconf.m_xml.setCurrentNode(pNode);
	pChildNode = m_xmlconf.m_xml.getFirstChildItem("logdir");
	if (pChildNode == NULL)
	{
		m_errMsg = "配置文件错,head节点下的public节点无[logdir]节点";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
		g_sLogFilePath ="";
	}
	else
	{
		g_sLogFilePath = pChildNode->getValue();
	}
	

	m_log.SetLogPara(g_nLoglevel,g_sLogFilePath.c_str(),g_sLogFileName.c_str());
	m_xmlconf.gotoHead();
	pNode = m_xmlconf.m_xml.getFirstChildItem("fpu");
	if (pNode == NULL)
	{
		m_errMsg = "配置文件错,head节点下无[fpu]节点";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
		return false;
	}
	pChildNode = m_xmlconf.m_xml.getFirstChildItem("fpcport");
	if (pChildNode == NULL)
	{
		m_errMsg = "配置文件错,head节点下的fpu节点无[fpcport]节点";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
		return false;
	}
	g_nPort = atoi(pChildNode->getValue().c_str());
	if (g_nPort <= 1024)
	{
		m_errMsg = "配置文件错,head节点下的fpu节点[fpcport]节点配置不合法";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
		return false;
	}
		
	
	m_xmlconf.m_xml.setCurrentNode(pNode);
	pChildNode = m_xmlconf.m_xml.getFirstChildItem("fpcip");
	if (pChildNode == NULL)
	{
		m_errMsg = "配置文件错,head节点下的fpu节点无[fpcip]节点";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
		return false;
	}
	g_sBccIp = pChildNode->getValue();

	m_xmlconf.m_xml.setCurrentNode(pNode);
	pChildNode = m_xmlconf.m_xml.getFirstChildItem("compress");
	if (pChildNode == NULL)
	{
		m_errMsg = "配置文件错,head节点下的fpu节点无[compress]节点";
		ret =0;
	}
	else
	{
		ret = atoi(pChildNode->getValue().c_str());
	}
	//	m_xmlconf.getHeadValue("是否加密压缩",ret);
	if (1 == ret)
	{
		g_IsZipEncrypt = 1;
//		g_log.Log(LOG_ERROR_GENERAL,__FILE__,__LINE__,"数据包加密压缩设置",0,"数据包要求加密");
	}
	else if (2 == ret)
	{
		g_IsZipEncrypt = 2;
//		g_log.Log(LOG_ERROR_GENERAL,__FILE__,__LINE__,"数据包加密压缩设置",0,"数据包要求压缩");
	}
	else if (3 == ret)
	{
		g_IsZipEncrypt = 3;
//		g_log.Log(LOG_ERROR_GENERAL,__FILE__,__LINE__,"数据包加密压缩设置",0,"数据包要求des加密");
	}
	else if (4 == ret)
	{
		g_IsZipEncrypt = 4;
//		g_log.Log(LOG_ERROR_GENERAL,__FILE__,__LINE__,"数据包加密压缩设置",0,"数据包要求des加密并压缩");
	}
	else if (5 == ret)
	{
		g_IsZipEncrypt = 5;
//		g_log.Log(LOG_ERROR_GENERAL,__FILE__,__LINE__,"数据包加密压缩设置",0,"数据包要求zlib压缩");
	}
	else
	{
		g_IsZipEncrypt = 0;
//		g_log.Log(LOG_ERROR_GENERAL,__FILE__,__LINE__,"数据包加密压缩设置",0,"数据包不加密不压缩");
	}
	m_xmlconf.m_xml.setCurrentNode(pNode);
	pChildNode = m_xmlconf.m_xml.getFirstChildItem("xmlnodemode");
	if (pChildNode == NULL)
	{
		g_nXmlNodeMode = 0;
	}
	else
	{
		if (atoi(pChildNode->getValue().c_str()) == 1)
		{
			g_nXmlNodeMode = 1;
		}
		else
		{
			g_nXmlNodeMode = 0;
		}
	}
	m_xmlconf.m_xml.setCurrentNode(pNode);
	pChildNode = m_xmlconf.m_xml.getFirstChildItem("oneentry");
	if (pChildNode == NULL)
	{
		g_bIsTxMainFunc = false;
	}
	else
	{
		if (pChildNode->GetAttributeValue("use",g_sMainFunc)<0)
		{
			g_bIsTxMainFunc = false;
		}
		else
		{
			if (atoi(g_sMainFunc.c_str())==1)
			{
				g_bIsTxMainFunc = true;
				if (pChildNode->GetAttributeValue("txcode",g_sMainFunc)<0)
				{
					m_errMsg = "配置文件错,head节点下的fpu节点交易统一入口属性[txcode]节点配置不合法";
					m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"配置文件错 %s",(char *)m_errMsg.c_str());
					return false;
				}
				m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"使用交易统一入口 %s",g_sMainFunc.c_str());
			}
		}
	}
	return true;
}

std::string CBuRes::GetMsg()
{
	return m_errMsg;
}

bool CBuRes::OpenDll(DLLHANDLE *pDll, const char *dllname,const char *dllpath)
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
	
	m_publog.LRtrim(dllpathname);
	
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
		m_publog.LRtrim(dllpathname);
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
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"打开动态库失败 %s",pdllerror);
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"打开动态库失败 %s",dllpathname);
		sprintf(dllpathname,"%s",dllname);
		m_publog.LRtrim(dllpathname);
		*pDll = LOADDLL(dllpathname);
		pdllerror = dlerror();
		if (pdllerror!=NULL)
		{
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"打开动态库失败 %s",pdllerror);
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"打开动态库失败 %s",dllpathname);
			return false;
		}
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"打开动态库成功 %s",dllpathname);
		
	}
#endif	
	return true;
}


/// 函数名: SetGPara1
/// 编程  : 王明松 2008-12-30 9:42:48
/// 返回  : virtual bool 
/// 参数  : bool setflag=true
/// 描述  : 设置全局变量1,若已设置则返回false
bool CBuRes::SetGPara1(bool setflag)
{
	return false;
}


/// 函数名: isSetGPara2
/// 编程  : 王明松 2008-12-30 9:36:10
/// 返回  : virtual bool 
/// 参数  : bool setflag=true
/// 描述  : 设置全局变量2,若已设置则返回false
bool CBuRes::SetGPara2(bool setflag)
{
	
	return false;
}

/// 函数名: isSetGPara3
/// 编程  : 王明松 2008-12-30 9:36:13
/// 返回  : virtual bool 
/// 参数  : bool setflag=true
/// 描述  : 设置全局变量3,若已设置则返回false
bool CBuRes::SetGPara3(bool setflag)
{

	return false;
}
long CBuRes::GetThreadShareSerial()
{
	int ret;
	char data[8*1024];
	int datalen;
	if (m_pLink !=NULL)
	{
		return m_pLink->GetThreadShareSerial();
	}
	return -1;
}

void CBuRes::WriteBack()
{
	
}
