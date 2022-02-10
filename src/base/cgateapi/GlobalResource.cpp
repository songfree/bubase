// GlobalResource.cpp: implementation of the CGlobalResource class.
//
//////////////////////////////////////////////////////////////////////

#include "GlobalResource.h"
#include "XmlHead.h"
#include "BF_Des.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define  DBPWDKEY  "rd402wms"
typedef CGlobalResource *(*NEWBUMODULEINSTANCE)();

CGlobalResource::CGlobalResource()
{
	g_nTimeOutSend = 10; //�������ݳ�ʱʱ�� �� �������ڶ����ﳬ����ʱ��󲻷���
	g_nDataSendNum = 10; //���ݷ��ʹ���
	g_nFlowControlSendBytes = 1024*1024*1; //1M
	g_nFlowControlSendTxNum = 100;
	g_nUseCert = 0;
	g_pCertClass = NULL;
	m_pDll = NULL;
	g_nHeartTime = 10; //����ʱ��  ���೤ʱ��δ�յ���������������������
	g_nLogLevel = 5;//��־����
	bzero(g_sCurPath,sizeof(g_sCurPath));
	g_nCrcFlag=0;
	g_nConnGateTimeOut = 5;
	g_nZipFlag = 1;
}

CGlobalResource::~CGlobalResource()
{
	if (m_pDll != NULL)
	{
		FREEDLL(m_pDll);
		m_pDll = NULL;
	}
	if (g_pCertClass != NULL)
	{
		delete g_pCertClass;
		g_pCertClass = NULL;
	}
	g_pLog.StopLog();
	g_pCgateLog.StopLog();
}

bool CGlobalResource::Init(const char *apixml)
{
	WORKPATH(g_sCurPath);
	if (!g_pXml.FromFile(apixml))
	{
		sprintf(m_sErrMsg,"����[%s]�ļ������ڻ��xml��ʽ",apixml);
		return false;
	}
// <?xml version="1.0" encoding="GB2312"?>
// <����API>
// <��־>
// <��־���� remark="��־���ټ���0���ش���1��Ҫ����2һ�����3����4��ʾ����5������Ϣ">5</��־����>
// <��־�ļ��� remark="��¼��־��Ϣ���ļ���,ǰ������">cgateapi.log</��־�ļ���>
// <��־Ŀ¼ remark="�����־��Ŀ¼"></��־Ŀ¼>
// </��־>
// <����  ����ʱ��="30" remark="ͨѶʱ���������ļ�� ��">
// <gate1  ip="" port="" remark="���ط�������IP���˿�" />
// <gate1  ip="" port="" remark="���ط�������IP���˿�" />
// </����>
// <֤�� use="0" dllpath="/home/songfree/lib" dllname="shcaapi.dll" method="getInstance" remark="�Ƿ�ʹ��֤�鼰֤�鶯̬�����ڵ�Ŀ¼,methodΪʵ����������">
// </֤��>
// </����API>
	g_pLog.StopLog();
	g_pCgateLog.StopLog();
	if (g_pXml.GetNodeValueByPath("/package/head/public/loglevel",false,g_nLogLevel) == NULL)
	{
		g_nLogLevel = 5;
	}
	if (g_pXml.GetNodeValueByPath("/package/head/public/logfile",false,g_sLogName,false) == NULL) 
	{
		sprintf(m_sErrMsg,"xml�����ļ��ڵ�[/package/head/public/logfile]δ����");
		return false;
	}
	if (g_pXml.GetNodeValueByPath("//package/head/public/logdir",false,g_sLogPath,false) == NULL) 
	{
		sprintf(m_sErrMsg,"xml�����ļ��ڵ�[/package/head/public/logdir]δ����");
		return false;
	}
	if (g_pXml.GetNodeValueByPath("/package/head/public/crc",false,g_nCrcFlag) == NULL)
	{
		g_nCrcFlag=0;
	}
	if (g_pXml.GetNodeValueByPath("/package/head/public/compress",false,g_nZipFlag) == NULL)
	{
		g_nZipFlag=1;
	}

	if (g_pXml.GetNodeValueByPath("/package/head/public/logsize",false,g_nMaxLogSize) == NULL)
	{
		g_nMaxLogSize = 0;
	}
	g_pLog.SetMaxLogSize(g_nMaxLogSize);
	g_pLog.SetLogPara(g_nLogLevel,g_sLogPath.c_str(),g_sLogName.c_str());
	g_pLog.StartLog();

	if (g_pXml.GetNodeValueByPath("/package/head/public/datalogsize",false,g_nMaxDataLogSize) == NULL)
	{
		g_nMaxLogSize = 0;
	}
	if (g_pXml.GetNodeValueByPath("/package/head/public/dataloglevel",false,g_nDataLogLevel) == NULL)
	{
		sprintf(m_sErrMsg,"xml�����ļ��ڵ�[/package/head/public/dataloglevel]δ����");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	g_pCgateLog.SetLogPara(g_nMaxDataLogSize,g_nDataLogLevel,g_sLogPath.c_str(),"CGATE.dat");
	g_pCgateLog.StartLog();

	CXmlNode *gatenode = g_pXml.GetNodeByPath("/package/head/gate",false);
	if (gatenode == NULL)
	{
		sprintf(m_sErrMsg,"xml�����ļ��ڵ�[/package/head/gate]δ����");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	std::string strtmp;
	if (gatenode->GetAttribute("hearttime",false,strtmp,false) == NULL)
	{
		sprintf(m_sErrMsg,"xml�����ļ��ڵ�[/package/head/gate]������[hearttime]δ����");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	g_nHeartTime = atoi(strtmp.c_str());
	if (g_nHeartTime < 5)
	{
		g_nHeartTime = 5;
		sprintf(m_sErrMsg,"xml�����ļ��ڵ�[/package/head/gate]������[hearttime]���ô���ȡĬ����Сֵ5��");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
	}
	if (gatenode->GetAttribute("disconnecttime",false,strtmp,false) == NULL)
	{
		sprintf(m_sErrMsg,"xml�����ļ��ڵ�[/package/head/gate]������[disconnecttime]δ����");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	g_nCloseTime = atoi(strtmp.c_str());
	if (g_nCloseTime < g_nHeartTime+5)
	{
		g_nCloseTime = g_nHeartTime+5;
		g_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"�Ͽ�ʱ�����ô�ȡĬ��ֵ:[%d]����ʱ��+5��",g_nCloseTime);
	}
	
	if (gatenode->GetAttribute("timeout",false,strtmp,false) == NULL)
	{
		sprintf(m_sErrMsg,"xml�����ļ��ڵ�[/package/head/gate]������[timeout]δ���ã�Ĭ��5��");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		g_nConnGateTimeOut = 5;
	}
	else
	{
		g_nConnGateTimeOut = atoi(strtmp.c_str());
		if (g_nConnGateTimeOut < 3)
		{
			g_nConnGateTimeOut = 3;
		}
	}
	

	
	CXmlNode *gateipportnode = (CXmlNode *)gatenode->GetFirstChild();
	if (gateipportnode == NULL)
	{
		sprintf(m_sErrMsg,"xml�����ļ��ڵ�[/package/head/gate]δ��������IP���˿��ӽڵ�");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	GATE_IPPORT oneip;
	g_lGateList.clear();
	while (gateipportnode != NULL)
	{
		if (gateipportnode->GetNodeType() != NODETYPE_VALUE && gateipportnode->GetNodeType() != NODETYPE_ATTR)
		{
			gateipportnode = (CXmlNode *)gateipportnode->getNextSibling();
			continue;
		}
		if (gateipportnode->GetAttribute("ip",false,oneip.sIp,false) == NULL)
		{
			sprintf(m_sErrMsg,"xml�����ļ��ڵ�[/package/head/gate/%s]δ����[ip]����",gateipportnode->GetNodeName().c_str());
			g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
		if (gateipportnode->GetAttribute("port",false,strtmp,false) == NULL)
		{
			sprintf(m_sErrMsg,"xml�����ļ��ڵ�[/package/head/gate/%s]δ����[port]����",gateipportnode->GetNodeName().c_str());
			g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
		oneip.nPort = atoi(strtmp.c_str());
		g_lGateList.push_back(oneip);
		gateipportnode = (CXmlNode *)gateipportnode->getNextSibling();
	}
	if (g_lGateList.size() <1 )
	{
		sprintf(m_sErrMsg,"xml�����ļ��ڵ�[/package/head/gate]δ��������IP���˿��ӽڵ�");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	//��ȡ֤����Ϣ
	if (g_pXml.GetNodeAttribute("/package/head/pki",false,"use",false,g_nUseCert) == NULL) 
	{
		sprintf(m_sErrMsg,"xml�����ļ��ڵ�[/package/head/pki]��������[use]δ����");
		g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	//ʹ��֤��
	if (g_nUseCert == 1)
	{
		std::string sdllname;
		std::string sdllpath;
		std::string sdllmethod;
		std::string scertpath;
		std::string scertpwd;
		std::string scertpwdflag;
		char  certpwd[40];
		bzero(certpwd,sizeof(certpwd));
		if (g_pXml.GetNodeAttribute("/package/head/pki",false,"dllpath",false,sdllpath) == NULL) 
		{
			sprintf(m_sErrMsg,"xml�����ļ��ڵ�[/package/head/pki]��������[dllpath]δ����");
			g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
		if (g_pXml.GetNodeAttribute("/package/head/pki",false,"dllname",false,sdllname) == NULL) 
		{
			sprintf(m_sErrMsg,"xml�����ļ��ڵ�[/package/head/pki]��������[dllname]δ����");
			g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
		if (g_pXml.GetNodeAttribute("/package/head/pki",false,"method",false,sdllmethod) == NULL) 
		{
			sprintf(m_sErrMsg,"xml�����ļ��ڵ�[/package/head/pki]��������[method]δ����");
			g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
		if (g_pXml.GetNodeAttribute("/package/head/pki",false,"certpath",false,scertpath) == NULL) 
		{
			sprintf(m_sErrMsg,"xml�����ļ��ڵ�[/package/head/pki]��������[certpath]δ����");
			g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
		if (g_pXml.GetNodeAttribute("/package/head/pki",false,"certpwd",false,scertpwd) == NULL) 
		{
			sprintf(m_sErrMsg,"xml�����ļ��ڵ�[/package/head/pki]��������[certpwd]δ����");
			g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
		if (g_pXml.GetNodeAttribute("/package/head/pki",false,"certpwdflag",false,scertpwdflag) == NULL) 
		{
			sprintf(m_sErrMsg,"xml�����ļ��ڵ�[/package/head/pki]��������[certpwdflag]δ����");
			g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
		strcpy(certpwd,scertpwd.c_str());
		if (atoi(scertpwdflag.c_str()) ==1 )
		{
			CBF_Des pEncrypt;
			int len = strlen(certpwd);
			pEncrypt.UncryptString(certpwd,len,DBPWDKEY);
		}
		if (!OpenDll(&m_pDll,sdllname.c_str(),sdllpath.c_str()) )
		{
			sprintf(m_sErrMsg,"�򿪶�̬��[%s %s]ʧ��",sdllpath.c_str(),sdllname.c_str());
			g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
		NEWBUMODULEINSTANCE baseinstance = (NEWBUMODULEINSTANCE)GETINSTANCE(m_pDll,sdllmethod.c_str());
		if (NULL == baseinstance)
		{
			sprintf(m_sErrMsg,"��̬��[%s]ʵ����ʧ��",sdllname.c_str());
			g_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
		
		g_pCertClass=(CICertApiBase *)baseinstance(); 
		if (g_pCertClass == NULL)
		{
			sprintf(m_sErrMsg,"��̬��[%s]ʵ����ʧ��",sdllname.c_str());
			g_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
		try
		{
			//��֤��̬���ʼ��
			if (!g_pCertClass->InitCertApi(scertpath.c_str(),certpwd)  )
			{
				sprintf(m_sErrMsg,g_pCertClass->GetErrMsg().c_str());
				g_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,m_sErrMsg);
				return false;
			}
		}
		catch (...)
		{
			g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"������֤��̬��[%s]��ʼ���쳣����",sdllname.c_str());
			return false;
		}
		
	}
	return true;
}

bool CGlobalResource::IsControlBytes()
{
	if (g_pApiFlowInfo.GetSpeedRateBytes() > g_nFlowControlSendBytes)
	{
		return true;
	}
	return false;
}

bool CGlobalResource::IsControlTx()
{
	if (g_pApiFlowInfo.GetSpeedRateTx() > g_nFlowControlSendTxNum)
	{
		return true;
	}
	return false;
}

std::string CGlobalResource::GetMsg()
{
	return m_sErrMsg;
}
bool CGlobalResource::OpenDll(DLLHANDLE *pDll, const char *dllname, const char *dllpath)
{
	bool bIsNotPath=false;
	char dllpathname[401];
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
#ifdef _WINDOWS
		sprintf(dllpathname,"%s\\%s",dllpath,dllname);
#else
		sprintf(dllpathname,"%s/%s",dllpath,dllname);
#endif
	}
	
	CBF_Tools::LRtrim(dllpathname);
	
	*pDll = LOADDLL(dllpathname);
#ifdef WIN32
	if (ISNOTOPEN(*pDll))
	{
		if (bIsNotPath)
		{
			sprintf(m_sErrMsg,"�򿪶�̬��[%s]ʧ��",dllpathname);
			g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
		sprintf(dllpathname,"%s",dllname);
		CBF_Tools::LRtrim(dllpathname);
		*pDll = LOADDLL(dllpathname);
		if (ISNOTOPEN(*pDll))
		{
			sprintf(m_sErrMsg,"�򿪶�̬��[%s]ʧ��",dllpathname);
			g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
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
			sprintf(m_sErrMsg,"�򿪶�̬��[%s]ʧ��",dllpathname);
			g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
		sprintf(dllpathname,"%s",dllname);
		CBF_Tools::LRtrim(dllpathname);
		*pDll = LOADDLL(dllpathname);
		pdllerror = dlerror();
		if (pdllerror!=NULL)
		{
			sprintf(m_sErrMsg,"�򿪶�̬��[%s]ʧ��[%s]",dllpathname,pdllerror);
			g_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
		
	}
#endif	
	return true;
}