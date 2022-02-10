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
	m_pConfigXml=NULL;//配置文件
	m_pExtCall=NULL;//外部调用
	m_pSqlCmd=NULL; //数据库连接
}

CBuFuncMgr::~CBuFuncMgr()
{
	Clear();
	m_pLog = NULL;
	m_pConfigXml = NULL;//配置文件
	m_pExtCall = NULL;//外部调用
	m_pSqlCmd = NULL; //数据库连接

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
		sprintf(m_errMsg,"未调用SetPara");
		return false;
	}
	//清空功能及原子交易表及实例及动态库句柄
	Clear();
	
	CXmlNode *getNode = m_pConfigXml->GetNodeByPath("/package/head/business/busidll",false);
	if (getNode == NULL)
	{
		sprintf(m_errMsg,"xml配置文件无[/package/head/business/busidll]节点");
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
		return false;
	}
	if (getNode->GetAttribute("dllpath",false,dllpath) == NULL)
	{
		sprintf(m_errMsg,"xml配置文件节点[/package/head/business/busidll]无dllpath属性");
		m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
		return false;
	}
	CXmlNode *dllNode = (CXmlNode *)getNode->GetFirstChild();
	if (dllNode == NULL)
	{
		sprintf(m_errMsg,"xml配置文件节点[/package/head/business/busidll]下没有业务动态库配置");
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
			sprintf(m_errMsg,"xml配置文件节点[/package/head/business/busidll]下的[%s]节点没有method属性",dllname.c_str());
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
			dllNode = (CXmlNode *)dllNode->getNextSibling();
			continue;
		}
		
		baseinstance = (NEWBUMODULEINSTANCE)GETINSTANCE(p,smethod.c_str());
		if (NULL == baseinstance)
		{
			sprintf(m_errMsg,"动态库[%s]实例化失败",dllname.c_str());
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,m_errMsg);
			dllNode = (CXmlNode *)dllNode->getNextSibling();
			continue;
		}
		CBuModule *myClass=NULL;
		myClass=(CBuModule *)baseinstance(); 
		
		try
		{
			//设置BU的参数
			myClass->SetPuPara(m_pConfigXml,m_pExtCall,m_pSqlCmd,m_pLog);

			//初始BU类
			if (!myClass->BuInit())
			{
				sprintf(m_errMsg,"%s BuInit失败",dllname.c_str());
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
				return false;
			}

		}
		catch (...)
		{
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"调用业务动态库%s初始化异常出错",dllname.c_str());
			return false;
		}
		m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"业务动态库[%s]初始化成功",dllname.c_str());

		m_alldllclass.push_back(myClass);
		
		bRes = myClass->FirstFuncInfo(funcinfo);
		if (!bRes)
		{
			sprintf(m_errMsg,"动态库[%s]无原子功能",dllname.c_str());
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
			dllNode = (CXmlNode *)dllNode->getNextSibling();
			continue;
		}
		while (bRes)
		{
			funcinfo.pBuInstance = myClass;
			CBF_Tools::StringCopy(funcinfo.sDllName,200,dllname.c_str());
			if (funcinfo.bIsStop) //此功能已停止
			{
				bRes = myClass->NextFuncInfo(funcinfo);
				continue;
			}
			//注册
			RegisterFunc(funcinfo);
			bRes = myClass->NextFuncInfo(funcinfo);//下一个功能
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
			sprintf(m_errMsg,"打开动态库[%s]失败",dllpathname);
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
			return false;
		}
		sprintf(dllpathname,"%s",dllname);
		CBF_Tools::LRtrim(dllpathname);
		*pDll = LOADDLL(dllpathname);
		if (ISNOTOPEN(*pDll))
		{
			sprintf(m_errMsg,"打开动态库[%s]失败 %d",dllpathname,GetLastError());
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
			sprintf(m_errMsg,"打开动态库[%s]失败",dllpathname);
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
			return false;
		}
		sprintf(dllpathname,"%s",dllname);
		CBF_Tools::LRtrim(dllpathname);
		*pDll = LOADDLL(dllpathname);
		pdllerror = dlerror();
		if (pdllerror!=NULL)
		{
			sprintf(m_errMsg,"打开动态库[%s]失败[%s]",dllpathname,pdllerror);
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
	if (funcinfo.bIsTxUnit)//原子交易
	{
		m_pLog->LogMp(LOG_DEBUG + 1, __FILE__, __LINE__, "RegisterFunc %d %s", funcinfo.nFuncNo, funcinfo.sFuncName);
		if (!m_pTxUnitTbl.Select(funcinfo.nFuncNo,queryFunc))//没找到，新增
		{
			m_pTxUnitTbl.Insert(funcinfo);
		}
		else //版本比较
		{
			if (queryFunc.dVersion < funcinfo.dVersion) //版本较新，使用新的
			{
				m_pTxUnitTbl.Update(funcinfo);
			}
			else
			{
				sprintf(m_errMsg,"动态库[%s]中原子交易[%d]版本[%f]不是最新，无法注册",funcinfo.sDllName,funcinfo.nFuncNo,funcinfo.dVersion);
				m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
				return false;
			}
		}
	}
	else //独立的功能，对外提供服务的
	{
		m_pLog->LogMp(LOG_DEBUG + 1, __FILE__, __LINE__, "RegisterFunc %d %s", funcinfo.nFuncNo,funcinfo.sFuncName);
		if (!m_pFuncTbl.Select(funcinfo.nFuncNo,queryFunc))//没找到，新增
		{
			m_pFuncTbl.Insert(funcinfo);
		}
		else //版本比较
		{
			if (queryFunc.dVersion < funcinfo.dVersion) //版本较新，使用新的
			{
				m_pFuncTbl.Update(funcinfo);
			}
			else
			{
				sprintf(m_errMsg,"动态库[%s]中功能[%d]版本[%f]不是最新，无法注册",funcinfo.sDllName,funcinfo.nFuncNo,funcinfo.dVersion);
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
