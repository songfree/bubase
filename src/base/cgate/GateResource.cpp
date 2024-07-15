// GateResource.cpp: implementation of the CGateResource class.
//
//////////////////////////////////////////////////////////////////////

#include "GateResource.h"
#include "BF_Tools.h"
#include "XmlHead.h"
#include "BF_Des.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define  DBPWDKEY  "rd402wms"
typedef CGateResource *(*NEWBUMODULEINSTANCE)();

CGateResource::CGateResource()
{
	ReSetPara();
	m_pDll  = NULL;
	g_pCertClass = NULL;
	g_nUnCommpress = 0;
	g_nEncryptFlag = 1;
	bzero(m_sStartDate,sizeof(m_sStartDate));

	g_nQuoteQueueDeleteTime = 10;
	g_nSendQueThreadNum  = 2;
	g_nQuoSubScribe = 1;
	g_nCloseBc =0;
	g_nCloseBcFunc = 99002;
}

CGateResource::~CGateResource()
{
	if (g_pCertClass != NULL)
	{
		delete g_pCertClass;
		g_pCertClass = NULL;
	}
	if (m_pDll != NULL)
	{
		FREEDLL(m_pDll);
		m_pDll = NULL;
	}
	g_pGateLog.StopLog();

	
}
std::string CGateResource::GetMsg()
{
	return m_sErrMsg;
}
bool CGateResource::OpenDll(DLLHANDLE *pDll, const char *dllname, const char *dllpath)
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
			g_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
		sprintf(dllpathname,"%s",dllname);
		CBF_Tools::LRtrim(dllpathname);
		*pDll = LOADDLL(dllpathname);
		if (ISNOTOPEN(*pDll))
		{
			sprintf(m_sErrMsg,"�򿪶�̬��[%s]ʧ��",dllpathname);
			g_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
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
			g_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
		sprintf(dllpathname,"%s",dllname);
		CBF_Tools::LRtrim(dllpathname);
		*pDll = LOADDLL(dllpathname);
		pdllerror = dlerror();
		if (pdllerror!=NULL)
		{
			sprintf(m_sErrMsg,"�򿪶�̬��[%s]ʧ��[%s]",dllpathname,pdllerror);
			g_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
		
	}
#endif	
	return true;
}
bool CGateResource::Init(const char *confile, CIErrlog* log)
{
// 	unsigned   int    g_nIoThreadNum;//IO�߳���
// 	unsigned   int    g_nMaxConnectNum;//���������
// 	unsigned   int    g_nRunMode;//����ģʽ   0-selectģ��  1-epollģ��(LINUX)   2-iocpģ��(WINDOWS)
// 	unsigned   int    g_nPort;//IOģ�͵������˿�
// 	int               g_nIocpAcceptNum;//iocpһ�η�AcceptEx����Ŀ
// 	int g_nHeartRun;//����ʱ��,���������ӳ�����ʱ��δ��ʱ��������������
// 	int g_nDisconnectTime;//δʹ�öϿ�ʱ�䣬���һ�������ڴ�ʱ����һֱû��ʹ�ã��򽫴����ӶϿ�����λ��Ĭ��600�뼴10����
// 	int g_nDispatchTime;//�������ʱ��
	if (!CBF_AIO_Resource::Init(confile, log))
	{
		printf("CBF_AIO_Resource Init error !\n");
		return false;
	}
	if (!g_pXml.FromFile(confile))
	{
		sprintf(m_sErrMsg,"����[%s]�ļ������ڻ��xml��ʽ",confile);
		return false;
	}
	//��ֹͣ��־�߳�
	g_pGateLog.StopLog();

	int tmpint=0;
	if (g_pXml.GetNodeValueByPath("/package/head/public/encrypt",false,g_nEncryptFlag) == NULL) 
	{
		sprintf(m_sErrMsg,"xml�����ļ��ڵ�[/package/head/public/encrypt]δ����");
		g_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	g_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"���ܱ�־Ϊ%d",g_nEncryptFlag);
	if (g_pXml.GetNodeValueByPath("/package/head/public/compress",false,g_nZipFlag) == NULL) 
	{
		sprintf(m_sErrMsg,"xml�����ļ��ڵ�[/package/head/public/compress]δ����");
		g_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		return false;
	}
	g_pLog->LogMp(LOG_PROMPT, __FILE__, __LINE__, "ѹ����־Ϊ%d", g_nZipFlag);
    if (g_pXml.GetNodeValueByPath("/package/head/public/filterquo", false, g_nFilterQuo) == NULL)
    {
        sprintf(m_sErrMsg, "xml�����ļ��ڵ�[/package/head/public/filterquo]δ����");
        g_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, m_sErrMsg);
        return false;
    }
	g_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"���������־Ϊ%d", g_nFilterQuo);
	if (g_pXml.GetNodeValueByPath("/package/head/public/uncompress",false,g_nUnCommpress) == NULL) 
	{
		sprintf(m_sErrMsg,"xml�����ļ��ڵ�[/package/head/public/uncompress]δ����");
		g_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		g_nUnCommpress = 0;
	}
	if (g_pXml.GetNodeValueByPath("/package/head/public/quotsubscribe",false,g_nQuoSubScribe) == NULL) 
	{
		sprintf(m_sErrMsg,"xml�����ļ��ڵ�[/package/head/public/quotsubscribe]δ���� Ĭ��1֧��");
		g_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		g_nQuoSubScribe = 1;
	}
    if (g_pXml.GetNodeValueByPath("/package/head/public/closebc2dreb", false, g_nCloseBc) == NULL)
    {
        sprintf(m_sErrMsg, "xml�����ļ��ڵ�[/package/head/public/closebc2dreb]δ���� Ĭ�ϲ�����");
        g_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, m_sErrMsg);
		g_nCloseBc = 0;
    }
	if (g_nCloseBc != 1)
	{
		g_nCloseBc = 0;
	}
	else
	{
        if (g_pXml.GetNodeValueByPath("/package/head/public/closebcfunc", false, g_nCloseBcFunc) == NULL)
        {
            sprintf(m_sErrMsg, "xml�����ļ��ڵ�[/package/head/public/closebcfunc]δ���� Ĭ��99002");
            g_pLog->LogMp(LOG_ERROR, __FILE__, __LINE__, m_sErrMsg);
			g_nCloseBcFunc = 99002;
        }
	}
	if (g_pXml.GetNodeValueByPath("/package/head/public/clientsendthreadnum",false,g_nSendQueThreadNum) == NULL) 
	{
		sprintf(m_sErrMsg,"xml�����ļ��ڵ�[/package/head/public/clientsendthreadnum]δ���� Ĭ��1�������50��");
		g_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
		g_nSendQueThreadNum = 1;
	}
	if (g_nSendQueThreadNum >50 )
	{
		g_nSendQueThreadNum = 50;
	}

	if (!g_pDrebRes.Init(confile))
	{
		printf("�������� Init error !\n");
		return false;
	}
	char  tmpchar1[100];
	sprintf(tmpchar1,"CGATE%d-%d.dat", g_pDrebRes.g_nSvrMainId, g_pDrebRes.g_nSvrPrivateId);
	g_pGateLog.SetLogPara(g_pDrebRes.g_nMaxLogSize, g_pDrebRes.g_nDrebLoglevel, g_pDrebRes.g_sLogFilePath.c_str(),tmpchar1);
	g_pGateLog.StartLog();

	
	//��ȡ֤����Ϣ
	if (g_pXml.GetNodeAttribute("/package/head/pki",false,"use",false,g_nUseCert) == NULL) 
	{
		sprintf(m_sErrMsg,"xml�����ļ��ڵ�[/package/head/pki]��������[use]δ����");
		g_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
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
			g_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
		if (g_pXml.GetNodeAttribute("/package/head/pki",false,"dllname",false,sdllname) == NULL) 
		{
			sprintf(m_sErrMsg,"xml�����ļ��ڵ�[/package/head/pki]��������[dllname]δ����");
			g_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
		if (g_pXml.GetNodeAttribute("/package/head/pki",false,"method",false,sdllmethod) == NULL) 
		{
			sprintf(m_sErrMsg,"xml�����ļ��ڵ�[/package/head/pki]��������[method]δ����");
			g_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
		if (g_pXml.GetNodeAttribute("/package/head/pki",false,"certpath",false,scertpath) == NULL) 
		{
			sprintf(m_sErrMsg,"xml�����ļ��ڵ�[/package/head/pki]��������[certpath]δ����");
			g_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
		if (g_pXml.GetNodeAttribute("/package/head/pki",false,"certpwd",false,scertpwd) == NULL) 
		{
			sprintf(m_sErrMsg,"xml�����ļ��ڵ�[/package/head/pki]��������[certpwd]δ����");
			g_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
		if (g_pXml.GetNodeAttribute("/package/head/pki",false,"certpwdflag",false,scertpwdflag) == NULL) 
		{
			sprintf(m_sErrMsg,"xml�����ļ��ڵ�[/package/head/pki]��������[certpwdflag]δ����");
			g_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
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
			g_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
		NEWBUMODULEINSTANCE baseinstance = (NEWBUMODULEINSTANCE)GETINSTANCE(m_pDll,sdllmethod.c_str());
		if (NULL == baseinstance)
		{
			sprintf(m_sErrMsg,"��̬��[%s]ʵ����ʧ��",sdllname.c_str());
			g_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
		
		g_pCertClass=(CICertApiBase *)baseinstance(); 
		if (g_pCertClass == NULL)
		{
			sprintf(m_sErrMsg,"��̬��[%s]ʵ����ʧ��",sdllname.c_str());
			g_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
		try
		{
			//��֤��̬���ʼ��
			if (!g_pCertClass->InitCertApi(scertpath.c_str(),certpwd)  )
			{
				sprintf(m_sErrMsg,"%s",g_pCertClass->GetErrMsg().c_str());
				g_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,m_sErrMsg);
				return false;
			}
		}
		catch (...)
		{
			sprintf(m_sErrMsg,"������֤��̬��[%s]��ʼ���쳣����",sdllname.c_str());
			g_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_sErrMsg);
			return false;
		}
		
	}

	//�㲥����

	CXmlNode *bcnode = g_pXml.GetNodeByPath("package/head/broadcast",false);
	if (bcnode != NULL)
	{
		S_BC_INFO bcinfo;
		CXmlNode *bcfuncnode = (CXmlNode *)bcnode->GetFirstChild();
		while (bcfuncnode != NULL)
		{
			bcinfo.nServiceNo = atoi(bcfuncnode->GetNodeValue().c_str());
			CXmlAttribute* attr = bcfuncnode->GetAttribute("level");
			if (attr == NULL)
			{
				bcinfo.nLevel =2;//Ĭ��������㲥
			}
			else
			{
				bcinfo.nLevel = atoi(attr->GetAttrValue().c_str());
				if (bcinfo.nLevel != 1 && bcinfo.nLevel != 2)
				{
					bcinfo.nLevel =2;
				}
			}
			g_lBCFuncList.Insert(bcinfo);
			bcfuncnode = (CXmlNode *)bcfuncnode->getNextSibling();
		}
	}
	
	CBF_Date_Time pDate;
	std::string ss = pDate.ToStringDT17();
	sprintf(m_sStartDate,"%s",ss.c_str());
	return true;
}


