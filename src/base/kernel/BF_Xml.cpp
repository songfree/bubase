// BFXml.cpp: implementation of the CBFXml class.
//
//////////////////////////////////////////////////////////////////////

#include "BF_Xml.h"
#include "XmlHead.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBF_Xml::CBF_Xml()
{
	m_pRootNode.SetNodeType(NODETYPE_ATTR);
	m_pRootNode.SetParseClass(&m_parseXml);
}

CBF_Xml::~CBF_Xml()
{
	DeleteAllNode();
}

bool CBF_Xml::FromBuffer(const char *xmlbuf,int layer)
{
	DeleteAllNode();
	if (layer <1)
	{
		m_pRootNode.SetNodeValue(xmlbuf,false);
		return m_pRootNode.ParseChildNode(0);
	}
	else
	{
		m_pRootNode.SetNodeValue(xmlbuf,false);
		return m_pRootNode.ParseChildNode(layer);
	}
	
}

bool CBF_Xml::FromFile(const char *xmlfile, int layer)
{
	FILE *fp;
	int filelen;
	bool bres=false;
	int buflen;
	fp = fopen(xmlfile,"rb");
	if (fp != NULL)
	{
		struct_stat f_stat;
		FSTAT(xmlfile,&f_stat);
		filelen=f_stat.st_size;//文件长度
		char *buffer = (char *) malloc(filelen+2);
		if (buffer == NULL)
		{
			fclose(fp);
			return false;
		}
		buflen = fread(buffer,1,filelen,fp);
		fclose(fp);
		buffer[buflen]=0;	
		if (buflen != filelen)
		{
			free(buffer) ;
			return false;
		}
		if (buflen >0)
		{
			bres= FromBuffer(buffer,layer);
		}
		free(buffer) ;
		return bres;
	}
	return bres;
}

bool CBF_Xml::ToString(std::string &xmlstring)
{
	int ret=0;
	CXmlNode *tmpnode = (CXmlNode *)m_pRootNode.GetFirstChild();
	if (tmpnode != NULL)
	{
		ret = tmpnode->ToString(xmlstring,0);
		if (ret<1)
		{
			return false;
		}
	}
	return true;
}

bool CBF_Xml::ToFile(const char *filename)
{
	FILE *fp;
	fp = fopen(filename,"wb");
	if (fp==NULL)
	{
		return false;
	}

	CXmlNode *tmpnode = (CXmlNode *)m_pRootNode.GetFirstChild();
	if (tmpnode != NULL)
	{
		tmpnode->NodeToFile(fp,0);
	}
	fflush(fp);
	fclose(fp);
	return true;
}

bool CBF_Xml::ToBuffer(char *buffer, int &buflen)
{
	int xmldatalen=0;
	xmldatalen = GetXmlDataLen();
	if (xmldatalen>=buflen)
	{
		return false;
	}
	buffer[0]=0;
	buflen = 0;
	CXmlNode *tmpnode = (CXmlNode *)m_pRootNode.GetFirstChild();
	if (tmpnode != NULL)
	{
		tmpnode->NodeToBuffer(buffer,buflen,0);
	}
	return true;
}

int CBF_Xml::GetXmlDataLen()
{
	int datalen=0;
	CXmlNode *tmpnode = (CXmlNode *)m_pRootNode.GetFirstChild();
	if (tmpnode != NULL)
	{
		tmpnode->GetNodeDataLen(datalen,0);
	}	
	return datalen;
}


void CBF_Xml::DeleteAllNode()
{
	m_pRootNode.DeleteAllChild();
}

CXmlNode * CBF_Xml::GetNodeByPath(const char *xmlpathname,bool isTransSign)
{
	std::vector<std::string> namelist;
	std::vector<CXmlNode *> nodelist;
	int ret;
	ret = GetNodeName(xmlpathname,&namelist);
	if (ret<1)
	{
		return NULL;
	}
	CXmlNode *getNode = &m_pRootNode;
	for (int i=0;i<(int)namelist.size();i++)
	{
		nodelist.clear();
		ret = getNode->GetChildNode(namelist[i],isTransSign,nodelist);
		if (ret != 1)
		{
			return NULL;
		}
		getNode = nodelist[0];
	}
	return getNode;
}

int CBF_Xml::GetNodeName(std::string xmlpathname, std::vector<std::string> *namelist)
{
	int bOffset;
	int eOffset;
	char *pbuf = (char *)xmlpathname.c_str();
	int buflen=xmlpathname.length();
	bOffset=0;
	eOffset=0;
	while (*pbuf != 0)
	{
		if (*pbuf == '/')
		{
			if (bOffset == eOffset)
			{
				pbuf++;
				bOffset++;
				eOffset++;
				continue;
			}
			else
			{
				std::string nodename=xmlpathname.substr(bOffset,eOffset-bOffset);
				namelist->push_back(nodename);
				pbuf++;
				eOffset++;
				bOffset=eOffset;
				continue;
			}
		}
		if (eOffset >= buflen-1 && buflen-bOffset>0)
		{
			std::string nodename=xmlpathname.substr(bOffset,buflen-bOffset);
			namelist->push_back(nodename);
			pbuf = NULL;
			return namelist->size();
		}
		pbuf++;
		eOffset++;
	}
	pbuf = NULL;
	return namelist->size();
}

bool CBF_Xml::XmlCopy(CBF_Xml *srcxml)
{
	DeleteAllNode();
	CXmlNode *srcnode = srcxml->GetRoot();
	return m_pRootNode.NodeCopy(srcnode);
}

CXmlNode * CBF_Xml::GetRoot()
{
	return &m_pRootNode;
}

CXmlNode * CBF_Xml::GetNodeValueByPath(const char *xmlpathname, bool isTransSign, std::string &value,bool isTransValue)
{
	CXmlNode *getNode = GetNodeByPath(xmlpathname,isTransSign);
	if (getNode == NULL)
	{
		return NULL;
	}
	value = getNode->GetNodeValue(isTransValue);
	return getNode;
}

CXmlNode * CBF_Xml::GetNodeValueByPath(const char *xmlpathname, bool isTransSign, int &value)
{
	std::string svalue;
	CXmlNode *getnode = GetNodeValueByPath(xmlpathname,isTransSign,svalue,false);
	if (getnode == NULL)
	{
		return NULL;
	}
	value = atoi(svalue.c_str());
	return getnode;
}
CXmlNode * CBF_Xml::GetNodeValueByPath(const char *xmlpathname, bool isTransSign, long &value)
{
	std::string svalue;
	CXmlNode *getnode = GetNodeValueByPath(xmlpathname,isTransSign,svalue,false);
	if (getnode == NULL)
	{
		return NULL;
	}
	value = atol(svalue.c_str());
	return getnode;
}
CXmlNode * CBF_Xml::GetNodeValueByPath(const char *xmlpathname, bool isTransSign, double &value)
{
	std::string svalue;
	CXmlNode *getnode = GetNodeValueByPath(xmlpathname,isTransSign,svalue,false);
	if (getnode == NULL)
	{
		return NULL;
	}
	value = atof(svalue.c_str());
	return getnode;
}
CXmlNode * CBF_Xml::SetNodeValueByPath(const char *xmlpathname, bool isTransSign,const char *value, bool isTransValue)
{
	std::vector<std::string> namelist;
	std::vector<CXmlNode *> nodelist;
	int ret;
	ret = GetNodeName(xmlpathname,&namelist);
	if (ret<1)
	{
		return NULL;
	}
	CXmlNode *getNode = &m_pRootNode;
	for (int i=0;i<(int)namelist.size();i++)
	{
		nodelist.clear();
		ret = getNode->GetChildNode(namelist[i],isTransSign,nodelist);
		if (ret >1) //大于1个节点出错
		{
			return NULL;
		}
		else if (ret <1) //没有则增加
		{
			getNode = getNode->AddChildNode(namelist[i],isTransSign);
		}
		else
		{
			getNode = nodelist[0];
		}
	}
	getNode->SetNodeValue(value,isTransValue);
	return getNode;
}

CXmlNode * CBF_Xml::SetNodeValueByPath(const char *xmlpathname, bool isTransSign, int value)
{
	char svalue[30];
	sprintf(svalue,"%d",value);
	return SetNodeValueByPath(xmlpathname,isTransSign,svalue,false);
}
CXmlNode * CBF_Xml::SetNodeValueByPath(const char *xmlpathname, bool isTransSign, long value)
{
	char svalue[30];
	sprintf(svalue,"%ld",value);
	return SetNodeValueByPath(xmlpathname,isTransSign,svalue,false);
}
CXmlNode * CBF_Xml::SetNodeValueByPath(const char *xmlpathname, bool isTransSign, double value, int dec)
{
	char svalue[30];
	char sfmat[20];
	sprintf(sfmat,"%s.%d%s","%",dec,"f");
	sprintf(svalue,sfmat,value);
	return SetNodeValueByPath(xmlpathname,isTransSign,svalue,false);
}


bool CBF_Xml::ToBuffer(char *buffer)
{
	buffer[0]=0;
	int buflen = 0;
	CXmlNode *tmpnode = (CXmlNode *)m_pRootNode.GetFirstChild();
	if (tmpnode != NULL)
	{
		tmpnode->NodeToBuffer(buffer,buflen,0);
	}
	return true;
}

CXmlAttribute * CBF_Xml::SetNodeAttribute(const char *xmlpathname, bool isTransSign, const char *attrname, bool isTransAttrName, const char *value, bool isTransValue)
{
	std::vector<std::string> namelist;
	std::vector<CXmlNode *> nodelist;
	int ret;
	ret = GetNodeName(xmlpathname,&namelist);
	if (ret<1)
	{
		return NULL;
	}
	CXmlNode *getNode = &m_pRootNode;
	for (int i=0;i<(int)namelist.size();i++)
	{
		nodelist.clear();
		ret = getNode->GetChildNode(namelist[i],isTransSign,nodelist);
		if (ret >1) //大于1个节点出错
		{
			return NULL;
		}
		else if (ret <1) //没有则增加
		{
			getNode = getNode->AddChildNode(namelist[i],isTransSign);
		}
		else
		{
			getNode = nodelist[0];
		}
	}
	return getNode->SetAttribute(attrname,isTransAttrName,value,isTransValue);
}
CXmlAttribute * CBF_Xml::SetNodeAttribute(const char *xmlpathname, bool isTransSign, const char *attrname, bool isTransAttrName, int value)
{
	char svalue[30];
	sprintf(svalue,"%d",value);
	return SetNodeAttribute(xmlpathname,isTransSign,attrname,isTransAttrName,svalue,false);
}
CXmlAttribute * CBF_Xml::SetNodeAttribute(const char *xmlpathname, bool isTransSign, const char *attrname, bool isTransAttrName, long value)
{
	char svalue[30];
	sprintf(svalue,"%ld",value);
	return SetNodeAttribute(xmlpathname,isTransSign,attrname,isTransAttrName,svalue,false);
}
CXmlAttribute * CBF_Xml::SetNodeAttribute(const char *xmlpathname, bool isTransSign, const char *attrname, bool isTransAttrName, double value,int dec)
{
	char svalue[30];
	char sfmat[20];
	sprintf(sfmat,"%s.%d%s","%",dec,"f");
	sprintf(svalue,sfmat,value);
	return SetNodeAttribute(xmlpathname,isTransSign,attrname,isTransAttrName,svalue,false);
}
CXmlAttribute * CBF_Xml::GetNodeAttribute(const char *xmlpathname, bool isTransSign, const char *attrname, bool isTransAttrName, std::string &value, bool isTransValue)
{
	CXmlNode *getNode = GetNodeByPath(xmlpathname,isTransSign);
	if (getNode == NULL)
	{
		return NULL;
	}
	return getNode->GetAttribute(attrname,isTransAttrName,value,isTransValue);
}
CXmlAttribute * CBF_Xml::GetNodeAttribute(const char *xmlpathname, bool isTransSign, const char *attrname, bool isTransAttrName, int &value)
{
	std::string svalue;
	CXmlAttribute *attr = GetNodeAttribute(xmlpathname,isTransSign,attrname,isTransAttrName,svalue,false);
	if (attr == NULL)
	{
		return NULL;
	}
	value = atoi(svalue.c_str());
	return attr;
}
CXmlAttribute * CBF_Xml::GetNodeAttribute(const char *xmlpathname, bool isTransSign, const char *attrname, bool isTransAttrName, long &value)
{
	std::string svalue;
	CXmlAttribute *attr = GetNodeAttribute(xmlpathname,isTransSign,attrname,isTransAttrName,svalue,false);
	if (attr == NULL)
	{
		return NULL;
	}
	value = atol(svalue.c_str());
	return attr;
}
CXmlAttribute * CBF_Xml::GetNodeAttribute(const char *xmlpathname, bool isTransSign, const char *attrname, bool isTransAttrName, double &value)
{
	std::string svalue;
	CXmlAttribute *attr = GetNodeAttribute(xmlpathname,isTransSign,attrname,isTransAttrName,svalue,false);
	if (attr == NULL)
	{
		return NULL;
	}
	value = atof(svalue.c_str());
	return attr;
}

CXmlNode * CBF_Xml::AddNodeByPath(const char *xmlpathname, bool isTransSign, const char *nodename, bool isTransName, const char *nodevalue, bool isTransValue)
{
	CXmlNode *parentnode = SetNodeValueByPath(xmlpathname,isTransSign,"",false);
	if (parentnode == NULL)
	{
		return NULL;
	}
	CXmlNode *addnode = parentnode->AddChildNode(nodename,isTransName);
	if (addnode == NULL)
	{
		return NULL;
	}
	addnode->SetNodeValue(nodevalue,isTransValue);
	return addnode;
}
CXmlNode * CBF_Xml::AddNodeByPath(const char *xmlpathname, bool isTransSign, const char *nodename, bool isTransName,int nodevalue)
{
	char svalue[30];
	sprintf(svalue,"%d",nodevalue);
	return AddNodeByPath(xmlpathname,isTransSign,nodename,isTransName,svalue,false);
}
CXmlNode * CBF_Xml::AddNodeByPath(const char *xmlpathname, bool isTransSign, const char *nodename, bool isTransName,long nodevalue)
{
	char svalue[30];
	sprintf(svalue,"%ld",nodevalue);
	return AddNodeByPath(xmlpathname,isTransSign,nodename,isTransName,svalue,false);
}
CXmlNode * CBF_Xml::AddNodeByPath(const char *xmlpathname, bool isTransSign, const char *nodename, bool isTransName,double nodevalue,int dec)
{
	char svalue[30];
	char sfmat[20];
	sprintf(sfmat,"%s.%d%s","%",dec,"f");
	sprintf(svalue,sfmat,nodevalue);
	return AddNodeByPath(xmlpathname,isTransSign,nodename,isTransName,svalue,false);
}

void CBF_Xml::CreateIndex()
{
	m_pRootNode.CreateNodeAttrIndex();
}
