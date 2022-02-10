// XmlNode.cpp: implementation of the CXmlNode class.
//
//////////////////////////////////////////////////////////////////////

#include "XmlNode.h"
#include "XmlHead.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXmlNode::CXmlNode()
{
	m_bIsExpand = false; //子节点是否展开
	m_nNodeType = NODETYPE_UNKNOW;  //节点类型
	
	m_nParentIndex = 0;   //本节点在父节点中的ID序号
	m_nChildIndex=0;    //子节点索引记数
	m_nChildNodeNum=0; //子节点数
	
    m_parentNode = NULL;///父节点指针
	
    m_nextNode = NULL; ///临近下节点指针
	m_lastNode = NULL; ///临近上节点指针
	
    m_firstChildNode = NULL;///第一个子节点指针
	m_endChildNode = NULL;///最后一个子节点指针
	m_parseXml = NULL;

	m_bIsUseIndex = false;
	m_index = NULL;

	m_sNodeValue="";
	m_sNodeName="";
}

CXmlNode::~CXmlNode()
{
	DeleteAllChild();
	DeletePoint();
	m_parseXml = NULL;
	if (m_index != NULL)
	{
		delete m_index;
		m_index = NULL;
	}
}
CIXmlNode * CXmlNode::NewNode()
{
	return new CXmlNode();
}
void CXmlNode::SetParseClass(void *parse)
{
	m_parseXml = (CIXmlParse *)parse;
}
std::string CXmlNode::GetNodeName(bool isTransSign)
{
	if (!isTransSign)
	{
		return m_sNodeName;
	}
	else
	{
		std::string ss = m_sNodeName;
		XmlgetTransSign(ss);
		return ss;
	}
}

std::string CXmlNode::GetNodeValue(bool isTransSign)
{
	
	if (!isTransSign)
	{
		return m_sNodeValue;
	}
	else
	{
		std::string ss = m_sNodeValue;
		XmlgetTransSign(ss);
		return ss;
	}
}



int CXmlNode::AddChildNode(CIXmlNode *anode,bool first)
{
	m_nChildIndex++;
	m_nChildNodeNum++;
	CXmlNode *addnode = (CXmlNode *)anode;
	addnode->SetParseClass(m_parseXml);
	addnode->SetParentIndex(m_nChildIndex);//设置新增节点的序号
	if (first)
	{
		addnode->m_lastNode = NULL;
		if (m_firstChildNode != NULL) //不是第一个子节点
		{
			m_firstChildNode->m_lastNode = addnode;//原第一节点的上节点为此节点
			addnode->m_nextNode=m_firstChildNode; //此节点的临近节点为原第一子节点
		}
		else
		{
			m_endChildNode = addnode; 
			m_firstChildNode = addnode; 
			addnode->m_nextNode = NULL;
		}
		addnode->m_parentNode=this;//父节点为当前节点
		m_firstChildNode=addnode;//当前节点的第一个子节点为此节点
	}
	else
	{
		addnode->m_nextNode = NULL;
		if (m_endChildNode != NULL) //不是最后子节点
		{
			m_endChildNode->m_nextNode = addnode;//原最后一节点的下节点为此节点
			addnode->m_lastNode=m_endChildNode; //此节点的临近上节点为原最后一子节点
		}
		else
		{
			m_firstChildNode = addnode; 
			m_endChildNode = addnode;
			addnode->m_lastNode = NULL;
		}
		addnode->m_parentNode=this;//父节点为当前节点
		m_endChildNode=addnode;//当前节点的最后一个子节点为此节点
	}
	//对新节点进行排序
	if (m_bIsUseIndex) //若使用索引
	{
		CreateIndex(addnode);
	}
	return m_nChildNodeNum;
}

void CXmlNode::SetNodeType(int type)
{
	m_nNodeType = type;
}

int CXmlNode::GetNodeType()
{
	return m_nNodeType;
}


CIXmlNode * CXmlNode::GetFirstChild()
{
	return m_firstChildNode;
}

CIXmlNode * CXmlNode::GetEndChild()
{
	return m_endChildNode;
}

CIXmlNode * CXmlNode::getLastSibling()
{
	return m_lastNode;
}

CIXmlNode * CXmlNode::getNextSibling()
{
	return m_nextNode;
}

CIXmlNode * CXmlNode::GetParentNode()
{
	return m_parentNode;
}
void CXmlNode::SetNodeName(std::string nodename, bool isTransSign)
{
	m_sNodeName = nodename;
	if (isTransSign)
	{
		XmlputTransSign(m_sNodeName);
	}
	if (m_sNodeName.compare(nodename)!=0)
	{
		if (m_bIsUseIndex) //若使用索引，重建索引
		{
			ReCreateIndex();
		}
		
	}
}

void CXmlNode::SetNodeValue(std::string value, bool isTransSign)
{
	m_sNodeValue = value;
	if (isTransSign)
	{
		XmlputTransSign(m_sNodeValue);
	}
	if (m_nNodeType == NODETYPE_ATTR)
	{
		if (m_sNodeValue.length()>0)
		{
			m_nNodeType = NODETYPE_VALUE;
		}
	}
}
void CXmlNode::SetNodeValue(int value)
{
	char svalue[30];
	sprintf(svalue,"%d",value);
	SetNodeValue(svalue,false);
}
void CXmlNode::SetNodeValue(long value)
{
	char svalue[30];
	sprintf(svalue,"%ld",value);
	SetNodeValue(svalue,false);
}
void CXmlNode::SetNodeValue(double value, int dec)
{
	char svalue[30];
	char sfmat[20];
	sprintf(sfmat,"%s.%d%s","%",dec,"f");
	sprintf(svalue,sfmat,value);
	SetNodeValue(svalue,false);
}

void CXmlNode::SetParseChilded()
{
	m_bIsExpand = true;
}

bool CXmlNode::ExpandValue()
{
	if (m_bIsExpand)
	{
		return m_bIsExpand;
	}
	if (m_firstChildNode!=NULL)
	{
		m_bIsExpand = true;
		return m_bIsExpand; 
	}
	m_bIsExpand = ParseChildNode(1);
	return m_bIsExpand;
}
void CXmlNode::SetParentIndex(int id)
{
	m_nParentIndex = id;
}

int CXmlNode::GetParentIndex()
{
	return m_nParentIndex;
}

bool CXmlNode::ParseChildNode(int layer)
{
	m_bIsUseIndex = false;
	if (m_index != NULL)
	{
		delete m_index;
		m_index = NULL;
	}
 	if (m_parseXml == NULL)
 	{
 		return false;
 	}
	if (m_nNodeType != NODETYPE_VALUE)
	{
		return true;
	}
 	bool bres=false;
	if (layer == 0)
	{
		bres = m_parseXml->ParseXml(m_sNodeValue,this,0);
	}
	else if (layer == 1)
	{
		bres = m_parseXml->ParseXml(m_sNodeValue,this,1);
	}
	else
	{
		bres = m_parseXml->ParseXml(m_sNodeValue,this,1);
		if (!bres)
		{
			return bres;
		}
		CXmlNode *tmpnode = m_firstChildNode;
		while (tmpnode!=NULL)
		{
			bres = tmpnode->ParseChildNode(layer-1);
			if (!bres)
			{
				return bres;
			}
			tmpnode = (CXmlNode *)tmpnode->getNextSibling();
		}
	}
	if (m_firstChildNode != NULL) //只要有子节点，那么此节点的value为空且节点类型为NODETYPE_ATTR
	{
		m_sNodeValue = "";
		m_nNodeType = NODETYPE_ATTR;
	}
	m_bIsExpand = true;
	return bres;
}


bool CXmlNode::DeleteIndex(CXmlNode *node)
{
	if (node == NULL)
	{
		return false;
	}
	if (!m_bIsUseIndex || m_index == NULL) //若没有使用索引，返回true,不用删除
	{
		return true;
	}
	return m_index->Delete(node);
}

void CXmlNode::DeletePoint()
{
	if (m_nNodeType == NODETYPE_ROOT) //不能删除根节点
	{
		return;
	}
	//去掉在父节点里的索引
	DeleteParentIndex();
	
	CXmlNode *parentNode= NULL;
	//去掉父节点
	parentNode = m_parentNode;
	if (parentNode != NULL) 
	{
		if (parentNode->m_endChildNode == this && parentNode->m_firstChildNode == this) //父节点只有它一个节点
		{
			parentNode->m_endChildNode = NULL;
			parentNode->m_firstChildNode = NULL;
			m_parentNode = NULL;
		}
		else if (parentNode->m_endChildNode == this ) //是最后一个节点
		{
			parentNode->m_endChildNode = m_lastNode;
			m_lastNode->m_nextNode = NULL;
			m_lastNode = NULL;
			m_nextNode = NULL;//最后一个节点肯定是NULL啦
		}
		else if (parentNode->m_firstChildNode == this) //是最前一个节点
		{
			parentNode->m_firstChildNode = m_nextNode;
			m_nextNode->m_lastNode = NULL;
			m_nextNode = NULL;
			m_lastNode = NULL;//最后一个节点肯定是NULL啦
		}
		else //是中间的节点
		{
			m_nextNode->m_lastNode = m_lastNode;
			m_lastNode->m_nextNode = m_nextNode;
			m_nextNode = NULL;
			m_lastNode = NULL;
		}
		m_parentNode = NULL;
	}
	else
	{
		if (m_nextNode == NULL && m_lastNode == NULL) //前后节点都为空，什么都不做
		{
			
		}
		else if (m_nextNode == NULL)//是最后一个节点
		{
			m_lastNode->m_nextNode = NULL;
			m_lastNode = NULL;
		}
		else if (m_lastNode == NULL) //是最前一个节点
		{
			m_nextNode->m_lastNode = NULL;
			m_nextNode = NULL;
		}
		else //是中间的节点
		{
			m_nextNode->m_lastNode = m_lastNode;
			m_lastNode->m_nextNode = m_nextNode;
			m_nextNode = NULL;
			m_lastNode = NULL;
		}
	}
}

bool CXmlNode::DeleteParentIndex()
{
	CXmlNode *node = m_parentNode;
	if (node == NULL)
	{
		return true;
	}
	return node->DeleteIndex(this);
	
}

bool CXmlNode::DeleteChildNode(std::string nodename, bool isTransSign)
{
	std::string nname = nodename;
	if (isTransSign)
	{
		XmlputTransSign(nname);
	}
	vector<CXmlNode *>nodelist;

	int ret= GetChildNode(nodename,isTransSign,nodelist);
	if (ret<1)
	{
		return false;
	}
	for (int i=0;i<(int)nodelist.size();i++)
	{
		DeleteIndex(nodelist[i]);
		nodelist[i]->DeletePoint();
		nodelist[i]->m_attribute.Clear();
		delete nodelist[i]; //析构里调用DeletePoint
		nodelist[i]=NULL;
		m_nChildNodeNum--;
	}
	return true;
}

void CXmlNode::DeleteAllChild()
{
	CXmlNode *tmpnode = m_firstChildNode;
	while (tmpnode != NULL)
	{
		tmpnode->DeleteAllChild();
		tmpnode->DeletePoint();
		tmpnode->m_attribute.Clear();
		delete tmpnode;//析构里调用DeletePoint
		tmpnode = NULL;
		tmpnode = (CXmlNode *)GetFirstChild();
	}
	m_firstChildNode = NULL;
	m_endChildNode = NULL;
	if (m_index != NULL)
	{
		m_index->Clear();//清除所有索引
	}
	m_nChildNodeNum = 0;

}

CXmlNode * CXmlNode::AddChildNode(std::string nodename, bool isTransSign)
{
	CXmlNode *addnode = new CXmlNode();
	if (addnode == NULL)
	{
		return NULL;
	}
	addnode->SetNodeType(NODETYPE_ATTR);
	addnode->SetNodeName(nodename,isTransSign);
	AddChildNode(addnode);
	return addnode;
}



bool CXmlNode::CreateIndex(CXmlNode *childnode)
{
	if (m_index == NULL)
	{
		return true;
	}
	if (childnode->GetParentNode() != this)
	{
		return false;
	}
	if (childnode->GetNodeType() == NODETYPE_VALUE || childnode->GetNodeType() == NODETYPE_ATTR)
	{
		m_index->Insert(childnode);
		return true;
	}
	return false;
}
int CXmlNode::GetChildNode(std::string nodename, bool isTransSign, vector<CXmlNode *> &nodelist)
{
	std::string sname=nodename;
	if (isTransSign)
	{
		XmlputTransSign(sname);
	}
	//尝试去解析节点是否有下节点
	ExpandValue();
	
	if (m_bIsUseIndex && m_index != NULL) //若使用索引
	{
		if (!m_index->SelectNode(sname.c_str(),nodelist))
		{
			return 0;
		}
	}
	else
	{
		CXmlNode *childnode = m_firstChildNode;
		while (childnode != NULL)
		{
			if (sname.compare(childnode->GetNodeName(false))==0)
			{
				nodelist.push_back(childnode);
			}
			childnode = (CXmlNode *)childnode->getNextSibling();
		}
	}
	
	return nodelist.size();
}
CXmlNode *CXmlNode::GetChildNode(std::string nodename, bool isTransSign)
{
	std::string sname=nodename;
	if (isTransSign)
	{
		XmlputTransSign(sname);
	}
	//尝试去解析节点是否有下节点
	ExpandValue();

	CXmlNode *childnode = m_firstChildNode;
	while (childnode != NULL)
	{
		if (sname.compare(childnode->GetNodeName(false))==0)
		{
			return childnode;
		}
		childnode = (CXmlNode *)childnode->getNextSibling();
	}
	return NULL;
}
void CXmlNode::ReCreateIndex()
{
	//重建索引
	//delete index
	DeleteParentIndex();
	CXmlNode *pnode  = (CXmlNode *)GetParentNode();
	if (pnode == NULL)
	{
		return ;
	}
	pnode->CreateIndex(this);
	pnode = NULL;
}

int CXmlNode::NodeToBuffer(char *buffer, int &xmldatalen, int layer)
{
	bool isend=false;
	int j;
	int lalign;
	
	
	lalign = layer*3;

	//根据节点类型来判断
	if (m_nNodeType == NODETYPE_ATTR || m_nNodeType == NODETYPE_VALUE || m_nNodeType== NODETYPE_ROOT) //正常的节点
	{
		if (m_nNodeType != NODETYPE_ROOT)
		{
			if(buffer[0] != 0) //换行
			{
				buffer[xmldatalen] = '\n';
				xmldatalen++;
			}
			for (j=0;j<lalign;j++,xmldatalen++)
			{
				buffer[xmldatalen] = ' ';
			}
			
			buffer[xmldatalen] = '<';
			xmldatalen++;
			memcpy(buffer+xmldatalen,m_sNodeName.c_str(),m_sNodeName.length());
			xmldatalen+=m_sNodeName.length();
			//属性
			CXmlAttribute *pAttribute = m_attribute.GetFirstAttribute();
			while (pAttribute != NULL)
			{
				buffer[xmldatalen] = ' ';
				xmldatalen++;
				memcpy(buffer+xmldatalen,pAttribute->GetAttrName(false).c_str(),pAttribute->GetAttrName(false).length());
				xmldatalen+=pAttribute->GetAttrName(false).length();
				buffer[xmldatalen] = '=';
				xmldatalen++;
				buffer[xmldatalen] = '\"';
				xmldatalen++;
				memcpy(buffer+xmldatalen,pAttribute->GetAttrValue(false).c_str(),pAttribute->GetAttrValue(false).length());
				xmldatalen+=pAttribute->GetAttrValue(false).length();
				buffer[xmldatalen] = '\"';
				xmldatalen++;
				pAttribute=pAttribute->getNextSibling();
			}
			if (m_sNodeValue.length() > 0) //属性结束 >
			{
				buffer[xmldatalen] = '>';
				xmldatalen++;
				memcpy(buffer+xmldatalen,m_sNodeValue.c_str(),m_sNodeValue.length());
				xmldatalen+=m_sNodeValue.length();
			}
			else //属性结束 
			{
				if (m_firstChildNode == NULL) //无子节点  />
				{
					buffer[xmldatalen] = ' ';
					xmldatalen++;
					buffer[xmldatalen] = '/';
					xmldatalen++;
					buffer[xmldatalen] = '>';
					xmldatalen++;
					isend  = true;
				}
				else //有子节点 >
				{
					buffer[xmldatalen] = '>';
					xmldatalen++;
				}
			}
		}
		//子节点
		if (m_firstChildNode != NULL)
		{
			m_firstChildNode->NodeToBuffer(buffer,xmldatalen,layer+1);
		}
		if (m_nNodeType != NODETYPE_ROOT)
		{
			if (m_sNodeValue.length() == 0)
			{
 				if (!isend)
				{
					buffer[xmldatalen] = '\n';
					xmldatalen++;
					for (j=0;j<lalign;j++,xmldatalen++)
					{
						buffer[xmldatalen] = ' ';
					}
					buffer[xmldatalen] = '<';
					xmldatalen++;
					buffer[xmldatalen] = '/';
					xmldatalen++;
					memcpy(buffer+xmldatalen,m_sNodeName.c_str(),m_sNodeName.length());
					xmldatalen+=m_sNodeName.length();
					buffer[xmldatalen] = '>';
					xmldatalen++;

				}
			}
			else
			{
				buffer[xmldatalen] = '<';
				xmldatalen++;
				buffer[xmldatalen] = '/';
				xmldatalen++;
				memcpy(buffer+xmldatalen,m_sNodeName.c_str(),m_sNodeName.length());
				xmldatalen+=m_sNodeName.length();
				buffer[xmldatalen] = '>';
				xmldatalen++;

			}
		}
	}	
	else
	{
		if(xmldatalen != 0)
		{
			buffer[xmldatalen] = '\n';
			xmldatalen++;
		}
		for (j=0;j<lalign;j++,xmldatalen++)
		{
			buffer[xmldatalen] = ' ';
		}
		if (m_nNodeType == NODETYPE_VER) // <?xml ?>
		{
			buffer[xmldatalen]='<';
			xmldatalen++;
			buffer[xmldatalen]='?';
			xmldatalen++;
			buffer[xmldatalen]='x';
			xmldatalen++;
			buffer[xmldatalen]='m';
			xmldatalen++;
			buffer[xmldatalen]='l';
			xmldatalen++;
			buffer[xmldatalen]=' ';
			xmldatalen++;
			memcpy(buffer+xmldatalen,m_sNodeValue.c_str(),m_sNodeValue.length());
			xmldatalen+=m_sNodeValue.length();
			buffer[xmldatalen]='?';
			xmldatalen++;
			buffer[xmldatalen]='>';
			xmldatalen++;
		}
		if (m_nNodeType == NODETYPE_REMARK) //<!-- xx -->
		{
			buffer[xmldatalen]='<';
			xmldatalen++;
			buffer[xmldatalen]='!';
			xmldatalen++;
			buffer[xmldatalen]='-';
			xmldatalen++;
			buffer[xmldatalen]='-';
			xmldatalen++;
			memcpy(buffer+xmldatalen,m_sNodeValue.c_str(),m_sNodeValue.length());
			xmldatalen+=m_sNodeValue.length();
			buffer[xmldatalen]='-';
			xmldatalen++;
			buffer[xmldatalen]='-';
			xmldatalen++;
			buffer[xmldatalen]='>';
			xmldatalen++;
		}
		if (m_nNodeType == NODETYPE_DOCTYPE) //<!DOCTYPE xxx>
		{
			buffer[xmldatalen]='<';
			xmldatalen++;
			buffer[xmldatalen]='!';
			xmldatalen++;
			buffer[xmldatalen]='D';
			xmldatalen++;
			buffer[xmldatalen]='O';
			xmldatalen++;
			buffer[xmldatalen]='C';
			xmldatalen++;
			buffer[xmldatalen]='T';
			xmldatalen++;
			buffer[xmldatalen]='Y';
			xmldatalen++;
			buffer[xmldatalen]='P';
			xmldatalen++;
			buffer[xmldatalen]='E';
			xmldatalen++;
			buffer[xmldatalen]=' ';
			xmldatalen++;
			memcpy(buffer+xmldatalen,m_sNodeValue.c_str(),m_sNodeValue.length());
			xmldatalen+=m_sNodeValue.length();
			buffer[xmldatalen]='>';
			xmldatalen++;
		}
	}
	if (m_nextNode != NULL)
	{
		m_nextNode->NodeToBuffer(buffer,xmldatalen,layer);
	}
	buffer[xmldatalen] =0;
	return xmldatalen;
}

int CXmlNode::GetNodeDataLen(int &xmldatalen,int layer)
{
	bool isend=false;
	int lalign;
	lalign = layer*3;
	//根据节点类型来判断
	if (m_nNodeType == NODETYPE_ATTR || m_nNodeType == NODETYPE_VALUE || m_nNodeType== NODETYPE_ROOT) //正常的节点
	{
		if (m_nNodeType != NODETYPE_ROOT)
		{
			if(xmldatalen != 0) 
			{
				xmldatalen++; //\n
			}
			//开始 <xxx
			xmldatalen +=lalign;
			xmldatalen +=1;
			xmldatalen += m_sNodeName.length();
			CXmlAttribute *pAttribute = m_attribute.GetFirstAttribute();
			while (pAttribute != NULL)
			{
				xmldatalen +=4;
				xmldatalen += pAttribute->GetAttrName(false).length();
				xmldatalen += pAttribute->GetAttrValue(false).length();
				pAttribute=pAttribute->getNextSibling();
			}
			if (m_sNodeValue.length() > 0) //属性结束 >
			{
				xmldatalen +=1;
				xmldatalen += m_sNodeValue.length();
			}
			else //属性结束 
			{
				if (m_firstChildNode == NULL) //无子节点  />
				{
					xmldatalen +=3;
					isend  = true;
				}
				else //有子节点 >
				{
					xmldatalen+=1;
				}
			}
		}
		
		if (m_firstChildNode != NULL)
		{
			m_firstChildNode->GetNodeDataLen(xmldatalen,layer+1);
		}
		if (m_nNodeType != NODETYPE_ROOT)
		{
			if (m_sNodeValue.length() == 0)
			{ 
				if (!isend) //结束 \n</xxxx>
				{
					xmldatalen +=4;
					xmldatalen += m_sNodeName.length();
					xmldatalen +=lalign;
				}
			}
			else   // <xx>??</xx>
			{
				xmldatalen +=3;
				xmldatalen += m_sNodeName.length();
			}
		}
	}
	else //NODETYPE_VER NODETYPE_REMARK NODETYPE_DOCTYPE 
	{
		if(xmldatalen != 0)
		{
			xmldatalen++;
		}
		xmldatalen +=lalign;
		if (m_nNodeType == NODETYPE_VER) // <?xml ?>
		{
			xmldatalen +=8;
			xmldatalen += m_sNodeValue.length();
		}
		if (m_nNodeType == NODETYPE_REMARK) //<!-- xx -->
		{
			xmldatalen +=9;
			xmldatalen += m_sNodeValue.length();
		}
		if (m_nNodeType == NODETYPE_DOCTYPE) //<!DOCTYPE xxx>
		{
			xmldatalen +=11;
			xmldatalen += m_sNodeValue.length();
		}
	}
	
	//取临近下节点的长度
	if (m_nextNode != NULL)
	{
		m_nextNode->GetNodeDataLen(xmldatalen,layer);
	}
	return xmldatalen;	
}

int CXmlNode::ToString(std::string &xmlstring,int layer)
{
	int ret;
	int buflen;
	char *buffer=NULL;
	ret = GetNodeDataLen(buflen,layer);
	buffer = (char *)malloc(buflen+2);
	if (buffer == NULL)
	{
	    return -1;
	}
	buflen = 0;
	NodeToBuffer(buffer,buflen,layer);
	if (buflen<1)
	{
	    free(buffer);
	    buffer = NULL;
		return buflen;
	}
	buffer[buflen]=0;
	xmlstring = buffer;
	free(buffer);
	buffer = NULL;
	return buflen;
}
CXmlAttribute * CXmlNode::SetAttribute(std::string attrname,bool isTransSign,std::string attrvlaue,bool isTransValue)
{
	return m_attribute.SetAttribute(attrname,isTransSign,attrvlaue,isTransValue);
}
CXmlAttribute * CXmlNode::AddAttribute(std::string attrname,bool isTransSign,std::string attrvlaue,bool isTransValue)
{
	return m_attribute.AddAttribute(attrname,isTransSign,attrvlaue,isTransValue);
}
CXmlAttribute * CXmlNode::SetAttribute(std::string attrname,bool isTransSign,int attrvlaue)
{
	return m_attribute.SetAttribute(attrname,isTransSign,attrvlaue);
}
CXmlAttribute * CXmlNode::SetAttribute(std::string attrname,bool isTransSign,long attrvlaue)
{
	return m_attribute.SetAttribute(attrname,isTransSign,attrvlaue);
}
CXmlAttribute * CXmlNode::SetAttribute(std::string attrname,bool isTransSign,double attrvlaue,int dec)
{
	return m_attribute.SetAttribute(attrname,isTransSign,attrvlaue,dec);
}
 
CXmlAttribute * CXmlNode::GetAttribute(std::string attrname,bool isTransSign,std::string &attrvalue,bool isTransValue)
{
	return m_attribute.GetAttribute(attrname,isTransSign,attrvalue,isTransValue);
}
CXmlAttribute * CXmlNode::GetAttribute(std::string attrname,bool isTransSign)
{
	return m_attribute.GetAttribute(attrname,isTransSign);
}
CXmlAttribute * CXmlNode::GetEndAttribute()
{
	return m_attribute.GetEndAttribute();
}
CXmlAttribute * CXmlNode::GetFirstAttribute()
{
	return m_attribute.GetFirstAttribute();
}
void CXmlNode::ClearAllAttribute()
{
	m_attribute.Clear();
}
bool CXmlNode::DeleteAttribute(std::string attrname,bool isTransSign)
{
	return m_attribute.DeleteAttribute(attrname,isTransSign);
}

bool CXmlNode::NodeCopy(CXmlNode *srcnode)
{
	DeleteAllChild();
	ClearAllAttribute();
	//复制节点名称值及类型
	m_sNodeName = srcnode->GetNodeName(false);
	m_sNodeValue = srcnode->GetNodeValue(false);
	m_nNodeType = srcnode->GetNodeType();
	m_bIsExpand = srcnode->GetNodeExpandFlag();

	//复制属性
	CXmlAttribute *tmpattr=NULL;
	tmpattr = srcnode->GetFirstAttribute();
	while (tmpattr != NULL)
	{
		SetAttribute(tmpattr->GetAttrName(false),false,tmpattr->GetAttrValue(false),false);
		tmpattr = tmpattr->getNextSibling();
	}

	//复制子节点及属性
	CXmlNode *addnode=NULL;
	CXmlNode *pChildNode = (CXmlNode *)srcnode->GetFirstChild();
	while (pChildNode != NULL)
	{
		addnode = AddChildNode("",false);
		addnode->SetParseClass(m_parseXml);
		if (addnode == NULL)
		{
			return false;
		}
		
		if (!addnode->NodeCopy(pChildNode))
		{
			return false;
		}
		pChildNode = (CXmlNode *)pChildNode->getNextSibling();
	}
	return true;
}

bool CXmlNode::GetNodeIndexFlag()
{
	return m_bIsUseIndex;
}

bool CXmlNode::GetNodeExpandFlag()
{
	return m_bIsExpand;
}

void CXmlNode::CreateNodeAttrIndex()
{
	if (m_index == NULL)
	{
		m_index = new CXmlNodeIndex();
	}
	m_bIsUseIndex = true;
	
	CXmlNode *childnode = m_firstChildNode;
	while( childnode != NULL)
	{
		CreateIndex(childnode);
		childnode->CreateNodeAttrIndex();
		childnode = (CXmlNode *)childnode->getNextSibling();
	}
	m_attribute.CreateIndex();

}
int CXmlNode::NodeToFile(FILE *fp, int layer)
{
	bool isend=false;
	int j;
	int lalign;
	
	
	lalign = layer*3;

	//根据节点类型来判断
	if (m_nNodeType == NODETYPE_ATTR || m_nNodeType == NODETYPE_VALUE || m_nNodeType== NODETYPE_ROOT) //正常的节点
	{
		if (m_nNodeType != NODETYPE_ROOT)
		{
			fprintf(fp,"\n");
			for (j=0;j<lalign;j++)
			{
				fprintf(fp," ");
			}
			fprintf(fp,"<");
			fprintf(fp,"%s",m_sNodeName.c_str());
			
			//属性
			CXmlAttribute *pAttribute = m_attribute.GetFirstAttribute();
			while (pAttribute != NULL)
			{
				fprintf(fp," ");
				fprintf(fp,"%s=\"%s\"",pAttribute->GetAttrName(false).c_str(),pAttribute->GetAttrValue(false).c_str());
				pAttribute=pAttribute->getNextSibling();
			}
			if (m_sNodeValue.length() >0 ) //属性结束 >
			{
				fprintf(fp,">");
				fprintf(fp,"%s",m_sNodeValue.c_str());
			}
			else //属性结束 
			{
				if (m_firstChildNode == NULL) //无子节点  />
				{
					fprintf(fp," />");
					isend  = true;
				}
				else //有子节点 >
				{
					fprintf(fp,">");
				}
			}
		}
		//子节点
		if (m_firstChildNode != NULL)
		{
			m_firstChildNode->NodeToFile(fp,layer+1);
		}
		if (m_nNodeType != NODETYPE_ROOT)
		{
			if (m_sNodeValue.length() == 0)
			{
 				if (!isend)
				{
					fprintf(fp,"\n");
					for (j=0;j<lalign;j++)
					{
						fprintf(fp," ");
					}
					fprintf(fp,"</%s>",m_sNodeName.c_str());
//					fprintf(fp,"\n");
				}
			}
			else
			{
				fprintf(fp,"</%s>",m_sNodeName.c_str());
//				fprintf(fp,"\n");
			}
		}
	}	
	else
	{

		for (j=0;j<lalign;j++)
		{
			fprintf(fp," ");
		}
		if (m_nNodeType == NODETYPE_VER) // <?xml ?>
		{
			fprintf(fp,"<?xml %s?>",m_sNodeValue.c_str());
		}
		if (m_nNodeType == NODETYPE_REMARK) //<!-- xx -->
		{
			fprintf(fp,"<!--%s-->",m_sNodeValue.c_str());
		}
		if (m_nNodeType == NODETYPE_DOCTYPE) //<!DOCTYPE xxx>
		{
			fprintf(fp,"<!DOCTYPE %s>",m_sNodeValue.c_str());
		}
	}
	if (m_nextNode != NULL)
	{
		m_nextNode->NodeToFile(fp,layer);
	}
	return 0;
}

