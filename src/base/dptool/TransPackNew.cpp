// TransPack.cpp: implementation of the CTransPack class.
//
//////////////////////////////////////////////////////////////////////

#include "TransPackNew.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTransPackNew::CTransPackNew()
{
	m_bisUse = false;
}

CTransPackNew::~CTransPackNew()
{
	if (m_transList.size()>0)
	{
		for (int j=0 ;j<m_serverList.size() ;j++)
		{
			if (m_transList[j].packTrans != NULL)
			{
				delete m_transList[j].packTrans;	
				m_transList[j].packTrans= NULL;
			}
			
		}
		
	}
}

int CTransPackNew::Init(CXmlPack *xmlpack)
{
	if (xmlpack == NULL)
	{
		sprintf(m_errMsg,"初始化传入XMLPACK类参数为NULL");
		return -1;
	}
	if (xmlpack->gotoHead()<0)
	{
		sprintf(m_errMsg,"初始化传入的XML节点head为NULL");
		return -1;
	}
	int ret=0;
	std::string stmp;
	CNode *pNode =NULL;
	CNode *pChildNode =NULL;
	pNode = xmlpack->m_xml.getFirstChildItem("前置配置");
	if (pNode == NULL)
	{
		sprintf(m_errMsg,"配置文件错,head节点下无[前置配置]节点");
		return -1;
	}
	pNode = xmlpack->m_xml.getFirstChildItem("协议转换配置");
	if (pNode == NULL)
	{
		sprintf(m_errMsg,"配置文件错,head节点下的前置配置节点无[协议转换配置]节点");
		return -1;
	}
	pNode->GetAttributeValue("use",stmp);
	if (atoi(stmp.c_str()) == 1)
	{
		m_bisUse = true;
	}
	else
	{
		m_bisUse = false;
		return 0;
	}
	pChildNode = xmlpack->m_xml.getFirstChildItem("转换配置");
	if (pChildNode == NULL)
	{
		sprintf(m_errMsg,"转换配置没有配置");
		return -1;
	} 
	pChildNode = xmlpack->m_xml.getFirstChildItem();
	while (pChildNode != NULL)
	{
		_S_TRANS_INFO_ tinfo;
		pChildNode->GetAttributeValue("filename",tinfo.sfilename);	
		if (tinfo.sfilename.length()<1)
		{
			sprintf(m_errMsg,"配置文件没有配置");
			return -1;
		}
		tinfo.stransname = pChildNode->getName();
		tinfo.packTrans = new CPackConvert();
		if (tinfo.packTrans->Init((char *)tinfo.sfilename.c_str())<0)
		{
			sprintf(m_errMsg,"%s初始化失败[%s]",tinfo.stransname.c_str(),tinfo.packTrans->m_errMsg);
			return -1;
		}
		m_transList.push_back(tinfo);
		pChildNode = pChildNode->getNextSibling();
	}
	xmlpack->m_xml.setCurrentNode(pNode);
	pChildNode = xmlpack->m_xml.getFirstChildItem("接入配置");
	if (pChildNode == NULL)
	{
		sprintf(m_errMsg,"接入配置没有配置");
		return -1;
	} 
	pChildNode = xmlpack->m_xml.getFirstChildItem();
	while (pChildNode != NULL)
	{
		_S_SERVER_INFO_ sinfo;
		sinfo.bisTrans = false;
		sinfo.sname = pChildNode->getName();
		CAttribute *attr = pChildNode->getAttributes();
		if (attr == NULL)
		{
			sprintf(m_errMsg,"配置[%s]没有属性",sinfo.sname.c_str());
			return -1;
		}
		pChildNode->GetAttributeValue("IP",sinfo.sip);
		pChildNode->GetAttributeValue("Port",sinfo.nport);
		pChildNode->GetAttributeValue("id",sinfo.sid);
		pChildNode->GetAttributeValue("transname",sinfo.stransname);
		pChildNode->GetAttributeValue("trans",stmp);
		if (atoi(stmp.c_str()) == 1)
		{
			sinfo.bisTrans = true;
		}
		
		if (IsPut(sinfo))
		{
			if (sinfo.bisTrans)
			{
				if (sinfo.stransname.length()<1)
				{
					sprintf(m_errMsg,"配置名称没有配置");
					return -1;
				}
				bool isfind=false;
				for (int j=0;j<m_transList.size();j++)
				{
					if (m_transList[j].stransname.compare(sinfo.stransname) == 0)
					{
						isfind = true;
						break;
					}
				}
				if (!isfind)
				{
					sprintf(m_errMsg,"配置名称在转换配置中没有配置");
					return -1;
				}
			}
			m_serverList.push_back(sinfo);
		}
		pChildNode = pChildNode->getNextSibling();
		
	}
	return 0;
}

CPackConvert * CTransPackNew::GetTransByIp(std::string sip,int &istrans)
{
	if (!m_bisUse)
	{
		istrans = 0;
		return NULL;
	}
	istrans = -1;
	for (int i=0;i<m_serverList.size();i++)
	{
		_S_SERVER_INFO_ tmp = m_serverList[i];
		if ( strcmp(m_serverList[i].sip.c_str(),sip.c_str()) == 0 )
		{
			istrans = m_serverList[i].bisTrans;
			if (istrans == 1)
			{
				for (int j=0;j<m_transList.size();j++)
				{
					if (m_transList[j].stransname.compare(m_serverList[i].stransname) == 0)
					{
						return m_transList[j].packTrans;
					}
				}
				return NULL;
			}
			else
			{
				return NULL;
			}
		}
	}
	return NULL;
}

CPackConvert * CTransPackNew::GetTransByServerId(std::string sid,int &istrans)
{
	if (!m_bisUse)
	{
		istrans = 0;
		return NULL;
	}
	istrans = -1;
	for (int i=0;i<m_serverList.size();i++)
	{
		if (strcmp(m_serverList[i].sid.c_str(),sid.c_str()) == 0 )
		{
			istrans = m_serverList[i].bisTrans;
			if (istrans == 1)
			{
				for (int j=0;j<m_transList.size();j++)
				{
					if (m_transList[j].stransname.compare(m_serverList[i].stransname) == 0)
					{
						return m_transList[j].packTrans;
					}
				}
				return NULL;
			}
			else
			{
				return NULL;
			}
		}
	}
	return NULL;
}

int CTransPackNew::GetServerPara(std::string sid, std::string &ip, int &port)
{
	for (int i=0;i<m_serverList.size();i++)
	{
		if (m_serverList[i].sid.compare(sid) == 0 )
		{
			ip = m_serverList[i].sip;
			port = m_serverList[i].nport;
			return 0;
		}
	}
	return -1;
}

int CTransPackNew::GetServerId(std::string ip, std::string &id)
{
	for (int i=0;i<m_serverList.size();i++)
	{
		if (m_serverList[i].sip.compare(ip) == 0 )
		{
			id = m_serverList[i].sid;
			return 0;
		}
	}
	return -1;
}

bool CTransPackNew::IsPut(_S_SERVER_INFO_ sinfo)
{
	if (m_serverList.size()<1)
	{
		return true;
	}
	for (int i=0;i<m_serverList.size();i++)
	{
		if (m_serverList[i].sip.compare(sinfo.sip) == 0 || m_serverList[i].sid.compare(sinfo.sid)==0)
		{
			sprintf(m_errMsg,"配置文件已有[%s]或[%s]",sinfo.sip.c_str(),sinfo.sid.c_str());
			return false;
		}
	}
	return true;
}
