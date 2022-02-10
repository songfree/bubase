// GResource.cpp: implementation of the CGResource class.
//
//////////////////////////////////////////////////////////////////////

#include "BuRes.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBuRes::CBuRes()
{

	g_nPort = 9001;//�����˿� Ĭ��Ϊ9001
	g_nLoglevel = 5;//��־���� Ĭ��Ϊ5
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
		m_errMsg = "�����ļ�Ϊ��";
		return false;
	}
	if (m_xmlconf.fromFile(confile)<0)
	{
		printf("�����ļ��Ƿ�,�����ļ�[%s]\n",confile);
		m_errMsg = "���������ļ��Ƿ�";
		return false;
	}

	
	if (m_xmlconf.gotoHead()<0)
	{
		m_errMsg = "��ʼ�������XML�ڵ�headΪNULL";
		return false;
	}

	int ret=0;
	std::string stmp;
	CNode *pNode =NULL;
	CNode *pChildNode =NULL;
	pNode = m_xmlconf.m_xml.getFirstChildItem("public");
	if (pNode == NULL)
	{
		m_errMsg = "�����ļ���,head�ڵ�����[public]�ڵ�";
		return false;
	}
	pChildNode = m_xmlconf.m_xml.getFirstChildItem("loglevel");
	if (pChildNode == NULL)
	{
		m_errMsg = "�����ļ���,head�ڵ��µ�public�ڵ���[loglevel]�ڵ�";
		return false;
	}
	g_nLoglevel = atoi(pChildNode->getValue().c_str());
	if (g_nLoglevel <0 )
	{
		g_nLoglevel = LOG_DEBUG;//���ô���Ϊ5
	}
	m_xmlconf.m_xml.setCurrentNode(pNode);
	pChildNode = m_xmlconf.m_xml.getFirstChildItem("logfile");
	if (pChildNode == NULL)
	{
		m_errMsg = "�����ļ���,head�ڵ��µ�public�ڵ���[logfile]�ڵ�";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ļ��� %s",(char *)m_errMsg.c_str());
		return false;
	}
	g_sLogFileName = pChildNode->getValue();
	if (g_sLogFileName.length()<1)
	{
		m_errMsg = "�����ļ���,head�ڵ��µ�public�ڵ���[logfile]�ڵ�û������";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ļ��� %s",(char *)m_errMsg.c_str());
		return false;
	}

	m_xmlconf.m_xml.setCurrentNode(pNode);
	pChildNode = m_xmlconf.m_xml.getFirstChildItem("logdir");
	if (pChildNode == NULL)
	{
		m_errMsg = "�����ļ���,head�ڵ��µ�public�ڵ���[logdir]�ڵ�";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ļ��� %s",(char *)m_errMsg.c_str());
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
		m_errMsg = "�����ļ���,head�ڵ�����[fpu]�ڵ�";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ļ��� %s",(char *)m_errMsg.c_str());
		return false;
	}
	pChildNode = m_xmlconf.m_xml.getFirstChildItem("fpcport");
	if (pChildNode == NULL)
	{
		m_errMsg = "�����ļ���,head�ڵ��µ�fpu�ڵ���[fpcport]�ڵ�";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ļ��� %s",(char *)m_errMsg.c_str());
		return false;
	}
	g_nPort = atoi(pChildNode->getValue().c_str());
	if (g_nPort <= 1024)
	{
		m_errMsg = "�����ļ���,head�ڵ��µ�fpu�ڵ�[fpcport]�ڵ����ò��Ϸ�";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ļ��� %s",(char *)m_errMsg.c_str());
		return false;
	}
		
	
	m_xmlconf.m_xml.setCurrentNode(pNode);
	pChildNode = m_xmlconf.m_xml.getFirstChildItem("fpcip");
	if (pChildNode == NULL)
	{
		m_errMsg = "�����ļ���,head�ڵ��µ�fpu�ڵ���[fpcip]�ڵ�";
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ļ��� %s",(char *)m_errMsg.c_str());
		return false;
	}
	g_sBccIp = pChildNode->getValue();

	m_xmlconf.m_xml.setCurrentNode(pNode);
	pChildNode = m_xmlconf.m_xml.getFirstChildItem("compress");
	if (pChildNode == NULL)
	{
		m_errMsg = "�����ļ���,head�ڵ��µ�fpu�ڵ���[compress]�ڵ�";
		ret =0;
	}
	else
	{
		ret = atoi(pChildNode->getValue().c_str());
	}
	//	m_xmlconf.getHeadValue("�Ƿ����ѹ��",ret);
	if (1 == ret)
	{
		g_IsZipEncrypt = 1;
//		g_log.Log(LOG_ERROR_GENERAL,__FILE__,__LINE__,"���ݰ�����ѹ������",0,"���ݰ�Ҫ�����");
	}
	else if (2 == ret)
	{
		g_IsZipEncrypt = 2;
//		g_log.Log(LOG_ERROR_GENERAL,__FILE__,__LINE__,"���ݰ�����ѹ������",0,"���ݰ�Ҫ��ѹ��");
	}
	else if (3 == ret)
	{
		g_IsZipEncrypt = 3;
//		g_log.Log(LOG_ERROR_GENERAL,__FILE__,__LINE__,"���ݰ�����ѹ������",0,"���ݰ�Ҫ��des����");
	}
	else if (4 == ret)
	{
		g_IsZipEncrypt = 4;
//		g_log.Log(LOG_ERROR_GENERAL,__FILE__,__LINE__,"���ݰ�����ѹ������",0,"���ݰ�Ҫ��des���ܲ�ѹ��");
	}
	else if (5 == ret)
	{
		g_IsZipEncrypt = 5;
//		g_log.Log(LOG_ERROR_GENERAL,__FILE__,__LINE__,"���ݰ�����ѹ������",0,"���ݰ�Ҫ��zlibѹ��");
	}
	else
	{
		g_IsZipEncrypt = 0;
//		g_log.Log(LOG_ERROR_GENERAL,__FILE__,__LINE__,"���ݰ�����ѹ������",0,"���ݰ������ܲ�ѹ��");
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
					m_errMsg = "�����ļ���,head�ڵ��µ�fpu�ڵ㽻��ͳһ�������[txcode]�ڵ����ò��Ϸ�";
					m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ļ��� %s",(char *)m_errMsg.c_str());
					return false;
				}
				m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ʹ�ý���ͳһ��� %s",g_sMainFunc.c_str());
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
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�򿪶�̬��ʧ�� %s",dllpathname);
			return false;
		}
		sprintf(dllpathname,"%s",dllname);
		m_publog.LRtrim(dllpathname);
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
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�򿪶�̬��ʧ�� %s",pdllerror);
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�򿪶�̬��ʧ�� %s",dllpathname);
		sprintf(dllpathname,"%s",dllname);
		m_publog.LRtrim(dllpathname);
		*pDll = LOADDLL(dllpathname);
		pdllerror = dlerror();
		if (pdllerror!=NULL)
		{
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�򿪶�̬��ʧ�� %s",pdllerror);
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�򿪶�̬��ʧ�� %s",dllpathname);
			return false;
		}
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�򿪶�̬��ɹ� %s",dllpathname);
		
	}
#endif	
	return true;
}


/// ������: SetGPara1
/// ���  : ������ 2008-12-30 9:42:48
/// ����  : virtual bool 
/// ����  : bool setflag=true
/// ����  : ����ȫ�ֱ���1,���������򷵻�false
bool CBuRes::SetGPara1(bool setflag)
{
	return false;
}


/// ������: isSetGPara2
/// ���  : ������ 2008-12-30 9:36:10
/// ����  : virtual bool 
/// ����  : bool setflag=true
/// ����  : ����ȫ�ֱ���2,���������򷵻�false
bool CBuRes::SetGPara2(bool setflag)
{
	
	return false;
}

/// ������: isSetGPara3
/// ���  : ������ 2008-12-30 9:36:13
/// ����  : virtual bool 
/// ����  : bool setflag=true
/// ����  : ����ȫ�ֱ���3,���������򷵻�false
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
