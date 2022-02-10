// BuFuncMgr.cpp: implementation of the CBuFuncMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "BuFuncMgr.h"
#include "BF_Tools.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
typedef CBuFuncMgr *(*NEWBUMODULEINSTANCE)();

CBuFuncMgr::CBuFuncMgr()
{
	m_pLog = NULL;
	m_pConfigXml=NULL;//�����ļ�
	m_pExtCall=NULL;//�ⲿ����
	m_pSqlCmd=NULL; //���ݿ�����
}

CBuFuncMgr::~CBuFuncMgr()
{
	Clear();
	m_pLog = NULL;
	m_pConfigXml = NULL;//�����ļ�
	m_pExtCall = NULL;//�ⲿ����
	m_pSqlCmd = NULL; //���ݿ�����

}

bool CBuFuncMgr::InitFuncInfo()
{
	std::string stmp;
	std::string smethod;
	std::string dllpath;
	std::string dllname;
	bool bRes;
	
	
	NEWBUMODULEINSTANCE baseinstance; 
	if (m_pLog == NULL)
	{
		sprintf(m_errMsg,"δ����SetPara");
		return false;
	}
	//��չ��ܼ�ԭ�ӽ��ױ�ʵ������̬����
	Clear();
	
	CXmlNode *getNode = m_pConfigXml->GetNodeByPath("/package/head/business/busidll",false);
	if (getNode == NULL)
	{
		sprintf(m_errMsg,"xml�����ļ���[/package/head/business/busidll]�ڵ�");
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
		return false;
	}
	if (getNode->GetAttribute("dllpath",false,dllpath) == NULL)
	{
		sprintf(m_errMsg,"xml�����ļ��ڵ�[/package/head/business/busidll]��dllpath����");
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
		return false;
	}
	CXmlNode *dllNode = (CXmlNode *)getNode->GetFirstChild();
	if (dllNode == NULL)
	{
		sprintf(m_errMsg,"xml�����ļ��ڵ�[/package/head/business/busidll]��û��ҵ��̬������");
		m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,m_errMsg);
		return true;
	}
	
	while (dllNode != NULL)
	{
		S_TBL_FUNC funcinfo;
		if (dllNode->GetNodeType() == 4)
		{
			dllNode = (CXmlNode *)dllNode->getNextSibling();
			continue;
		}
		dllname = dllNode->GetNodeName();
		DLLHANDLE p;
		if (!OpenDll(&p,dllname.c_str(),dllpath.c_str()))
		{
			dllNode = (CXmlNode *)dllNode->getNextSibling();
			continue;
		}
		m_allhandle.push_back(p);

		if (dllNode->GetAttribute("method",false,smethod) == NULL)
		{
			sprintf(m_errMsg,"xml�����ļ��ڵ�[/package/head/business/busidll]�µ�[%s]�ڵ�û��method����",dllname.c_str());
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
			dllNode = (CXmlNode *)dllNode->getNextSibling();
			continue;
		}
		
		baseinstance = (NEWBUMODULEINSTANCE)GETINSTANCE(p,smethod.c_str());
		if (NULL == baseinstance)
		{
			sprintf(m_errMsg,"��̬��[%s]ʵ����ʧ��",dllname.c_str());
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,m_errMsg);
			dllNode = (CXmlNode *)dllNode->getNextSibling();
			continue;
		}
		CBuModule *myClass=NULL;
		myClass=(CBuModule *)baseinstance(); 
		
		try
		{
			//����BU�Ĳ���
			myClass->SetPuPara(m_pConfigXml,m_pExtCall,m_pSqlCmd,m_pLog);

			//��ʼBU��
			if (!myClass->BuInit())
			{
				sprintf(m_errMsg,"%s BuInitʧ��",dllname.c_str());
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
				return false;
			}

		}
		catch (...)
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"����ҵ��̬��%s��ʼ���쳣����",dllname.c_str());
			return false;
		}
		m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"ҵ��̬��[%s]��ʼ���ɹ�",dllname.c_str());

		m_alldllclass.push_back(myClass);
		
		bRes = myClass->FirstFuncInfo(funcinfo);
		if (!bRes)
		{
			sprintf(m_errMsg,"��̬��[%s]��ԭ�ӹ���",dllname.c_str());
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
			dllNode = (CXmlNode *)dllNode->getNextSibling();
			continue;
		}
		while (bRes)
		{
			funcinfo.pBuInstance = myClass;
			CBF_Tools::StringCopy(funcinfo.sDllName,200,dllname.c_str());
			if (funcinfo.bIsStop) //�˹�����ֹͣ
			{
				bRes = myClass->NextFuncInfo(funcinfo);
				continue;
			}
			//ע��
			RegisterFunc(funcinfo);
			bRes = myClass->NextFuncInfo(funcinfo);//��һ������
		}
		dllNode = (CXmlNode *)dllNode->getNextSibling();
	}
	return true;
}

bool CBuFuncMgr::OpenDll(DLLHANDLE *pDll, const char *dllname, const char *dllpath)
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
			sprintf(m_errMsg,"�򿪶�̬��[%s]ʧ��",dllpathname);
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
			return false;
		}
		sprintf(dllpathname,"%s",dllname);
		CBF_Tools::LRtrim(dllpathname);
		*pDll = LOADDLL(dllpathname);
		if (ISNOTOPEN(*pDll))
		{
			sprintf(m_errMsg,"�򿪶�̬��[%s]ʧ�� %d",dllpathname,GetLastError());
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
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
			sprintf(m_errMsg,"�򿪶�̬��[%s]ʧ��",dllpathname);
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
			return false;
		}
		sprintf(dllpathname,"%s",dllname);
		CBF_Tools::LRtrim(dllpathname);
		*pDll = LOADDLL(dllpathname);
		pdllerror = dlerror();
		if (pdllerror!=NULL)
		{
			sprintf(m_errMsg,"�򿪶�̬��[%s]ʧ��[%s]",dllpathname,pdllerror);
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
			return false;
		}
		
	}
#endif	
	return true;
}

void CBuFuncMgr::SetBuPara(CBF_Xml *conxml,CIPcLink *callbu,CDbApi *sqldb,CBF_SimpleLog *bulog)
{
	m_pLog = bulog;
	m_pConfigXml = conxml;
	m_pSqlCmd = sqldb;
	m_pExtCall = callbu;

}

bool CBuFuncMgr::RegisterFunc(S_TBL_FUNC funcinfo)
{
	S_TBL_FUNC queryFunc;
	if (funcinfo.bIsTxUnit)//ԭ�ӽ���
	{
		m_pLog->LogMp(LOG_DEBUG + 1, __FILE__, __LINE__, "RegisterFunc %d %s", funcinfo.nFuncNo, funcinfo.sFuncName);
		if (!m_pTxUnitTbl.Select(funcinfo.nFuncNo,queryFunc))//û�ҵ�������
		{
			m_pTxUnitTbl.Insert(funcinfo);
		}
		else //�汾�Ƚ�
		{
			if (queryFunc.dVersion < funcinfo.dVersion) //�汾���£�ʹ���µ�
			{
				m_pTxUnitTbl.Update(funcinfo);
			}
			else
			{
				sprintf(m_errMsg,"��̬��[%s]��ԭ�ӽ���[%d]�汾[%f]�������£��޷�ע��",funcinfo.sDllName,funcinfo.nFuncNo,funcinfo.dVersion);
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
				return false;
			}
		}
	}
	else //�����Ĺ��ܣ������ṩ�����
	{
		m_pLog->LogMp(LOG_DEBUG + 1, __FILE__, __LINE__, "RegisterFunc %d %s", funcinfo.nFuncNo,funcinfo.sFuncName);
		if (!m_pFuncTbl.Select(funcinfo.nFuncNo,queryFunc))//û�ҵ�������
		{
			m_pFuncTbl.Insert(funcinfo);
		}
		else //�汾�Ƚ�
		{
			if (queryFunc.dVersion < funcinfo.dVersion) //�汾���£�ʹ���µ�
			{
				m_pFuncTbl.Update(funcinfo);
			}
			else
			{
				sprintf(m_errMsg,"��̬��[%s]�й���[%d]�汾[%f]�������£��޷�ע��",funcinfo.sDllName,funcinfo.nFuncNo,funcinfo.dVersion);
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
				return false;
			}
		}
	}
	return true;	
}

void CBuFuncMgr::Clear()
{
//	printf("11111111111111111111111111\n");
	for (int i=0;i<m_alldllclass.size();i++)
	{
		delete m_alldllclass[i];
		m_alldllclass[i] = NULL;
	}
//	printf("2222222222222222222222222\n");
	for (int j=0;j<m_allhandle.size();j++)
	{
		FREEDLL(m_allhandle[j]);
		m_allhandle[j] = NULL;
	}
//	printf("3333333333333333\n");
	m_alldllclass.clear();
	m_allhandle.clear();
	m_pFuncTbl.Clear();
	m_pTxUnitTbl.Clear();
}


char * CBuFuncMgr::GetMsg()
{
	return m_errMsg;
}
