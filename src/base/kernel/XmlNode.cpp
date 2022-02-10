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
	m_bIsExpand = false; //�ӽڵ��Ƿ�չ��
	m_nNodeType = NODETYPE_UNKNOW;  //�ڵ�����
	
	m_nParentIndex = 0;   //���ڵ��ڸ��ڵ��е�ID���
	m_nChildIndex=0;    //�ӽڵ���������
	m_nChildNodeNum=0; //�ӽڵ���
	
    m_parentNode = NULL;///���ڵ�ָ��
	
    m_nextNode = NULL; ///�ٽ��½ڵ�ָ��
	m_lastNode = NULL; ///�ٽ��Ͻڵ�ָ��
	
    m_firstChildNode = NULL;///��һ���ӽڵ�ָ��
	m_endChildNode = NULL;///���һ���ӽڵ�ָ��
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
	addnode->SetParentIndex(m_nChildIndex);//���������ڵ�����
	if (first)
	{
		addnode->m_lastNode = NULL;
		if (m_firstChildNode != NULL) //���ǵ�һ���ӽڵ�
		{
			m_firstChildNode->m_lastNode = addnode;//ԭ��һ�ڵ���Ͻڵ�Ϊ�˽ڵ�
			addnode->m_nextNode=m_firstChildNode; //�˽ڵ���ٽ��ڵ�Ϊԭ��һ�ӽڵ�
		}
		else
		{
			m_endChildNode = addnode; 
			m_firstChildNode = addnode; 
			addnode->m_nextNode = NULL;
		}
		addnode->m_parentNode=this;//���ڵ�Ϊ��ǰ�ڵ�
		m_firstChildNode=addnode;//��ǰ�ڵ�ĵ�һ���ӽڵ�Ϊ�˽ڵ�
	}
	else
	{
		addnode->m_nextNode = NULL;
		if (m_endChildNode != NULL) //��������ӽڵ�
		{
			m_endChildNode->m_nextNode = addnode;//ԭ���һ�ڵ���½ڵ�Ϊ�˽ڵ�
			addnode->m_lastNode=m_endChildNode; //�˽ڵ���ٽ��Ͻڵ�Ϊԭ���һ�ӽڵ�
		}
		else
		{
			m_firstChildNode = addnode; 
			m_endChildNode = addnode;
			addnode->m_lastNode = NULL;
		}
		addnode->m_parentNode=this;//���ڵ�Ϊ��ǰ�ڵ�
		m_endChildNode=addnode;//��ǰ�ڵ�����һ���ӽڵ�Ϊ�˽ڵ�
	}
	//���½ڵ��������
	if (m_bIsUseIndex) //��ʹ������
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
		if (m_bIsUseIndex) //��ʹ���������ؽ�����
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
	if (m_firstChildNode != NULL) //ֻҪ���ӽڵ㣬��ô�˽ڵ��valueΪ���ҽڵ�����ΪNODETYPE_ATTR
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
	if (!m_bIsUseIndex || m_index == NULL) //��û��ʹ������������true,����ɾ��
	{
		return true;
	}
	return m_index->Delete(node);
}

void CXmlNode::DeletePoint()
{
	if (m_nNodeType == NODETYPE_ROOT) //����ɾ�����ڵ�
	{
		return;
	}
	//ȥ���ڸ��ڵ��������
	DeleteParentIndex();
	
	CXmlNode *parentNode= NULL;
	//ȥ�����ڵ�
	parentNode = m_parentNode;
	if (parentNode != NULL) 
	{
		if (parentNode->m_endChildNode == this && parentNode->m_firstChildNode == this) //���ڵ�ֻ����һ���ڵ�
		{
			parentNode->m_endChildNode = NULL;
			parentNode->m_firstChildNode = NULL;
			m_parentNode = NULL;
		}
		else if (parentNode->m_endChildNode == this ) //�����һ���ڵ�
		{
			parentNode->m_endChildNode = m_lastNode;
			m_lastNode->m_nextNode = NULL;
			m_lastNode = NULL;
			m_nextNode = NULL;//���һ���ڵ�϶���NULL��
		}
		else if (parentNode->m_firstChildNode == this) //����ǰһ���ڵ�
		{
			parentNode->m_firstChildNode = m_nextNode;
			m_nextNode->m_lastNode = NULL;
			m_nextNode = NULL;
			m_lastNode = NULL;//���һ���ڵ�϶���NULL��
		}
		else //���м�Ľڵ�
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
		if (m_nextNode == NULL && m_lastNode == NULL) //ǰ��ڵ㶼Ϊ�գ�ʲô������
		{
			
		}
		else if (m_nextNode == NULL)//�����һ���ڵ�
		{
			m_lastNode->m_nextNode = NULL;
			m_lastNode = NULL;
		}
		else if (m_lastNode == NULL) //����ǰһ���ڵ�
		{
			m_nextNode->m_lastNode = NULL;
			m_nextNode = NULL;
		}
		else //���м�Ľڵ�
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
		delete nodelist[i]; //���������DeletePoint
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
		delete tmpnode;//���������DeletePoint
		tmpnode = NULL;
		tmpnode = (CXmlNode *)GetFirstChild();
	}
	m_firstChildNode = NULL;
	m_endChildNode = NULL;
	if (m_index != NULL)
	{
		m_index->Clear();//�����������
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
	//����ȥ�����ڵ��Ƿ����½ڵ�
	ExpandValue();
	
	if (m_bIsUseIndex && m_index != NULL) //��ʹ������
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
	//����ȥ�����ڵ��Ƿ����½ڵ�
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
	//�ؽ�����
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

	//���ݽڵ��������ж�
	if (m_nNodeType == NODETYPE_ATTR || m_nNodeType == NODETYPE_VALUE || m_nNodeType== NODETYPE_ROOT) //�����Ľڵ�
	{
		if (m_nNodeType != NODETYPE_ROOT)
		{
			if(buffer[0] != 0) //����
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
			//����
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
			if (m_sNodeValue.length() > 0) //���Խ��� >
			{
				buffer[xmldatalen] = '>';
				xmldatalen++;
				memcpy(buffer+xmldatalen,m_sNodeValue.c_str(),m_sNodeValue.length());
				xmldatalen+=m_sNodeValue.length();
			}
			else //���Խ��� 
			{
				if (m_firstChildNode == NULL) //���ӽڵ�  />
				{
					buffer[xmldatalen] = ' ';
					xmldatalen++;
					buffer[xmldatalen] = '/';
					xmldatalen++;
					buffer[xmldatalen] = '>';
					xmldatalen++;
					isend  = true;
				}
				else //���ӽڵ� >
				{
					buffer[xmldatalen] = '>';
					xmldatalen++;
				}
			}
		}
		//�ӽڵ�
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
	//���ݽڵ��������ж�
	if (m_nNodeType == NODETYPE_ATTR || m_nNodeType == NODETYPE_VALUE || m_nNodeType== NODETYPE_ROOT) //�����Ľڵ�
	{
		if (m_nNodeType != NODETYPE_ROOT)
		{
			if(xmldatalen != 0) 
			{
				xmldatalen++; //\n
			}
			//��ʼ <xxx
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
			if (m_sNodeValue.length() > 0) //���Խ��� >
			{
				xmldatalen +=1;
				xmldatalen += m_sNodeValue.length();
			}
			else //���Խ��� 
			{
				if (m_firstChildNode == NULL) //���ӽڵ�  />
				{
					xmldatalen +=3;
					isend  = true;
				}
				else //���ӽڵ� >
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
				if (!isend) //���� \n</xxxx>
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
	
	//ȡ�ٽ��½ڵ�ĳ���
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
	//���ƽڵ�����ֵ������
	m_sNodeName = srcnode->GetNodeName(false);
	m_sNodeValue = srcnode->GetNodeValue(false);
	m_nNodeType = srcnode->GetNodeType();
	m_bIsExpand = srcnode->GetNodeExpandFlag();

	//��������
	CXmlAttribute *tmpattr=NULL;
	tmpattr = srcnode->GetFirstAttribute();
	while (tmpattr != NULL)
	{
		SetAttribute(tmpattr->GetAttrName(false),false,tmpattr->GetAttrValue(false),false);
		tmpattr = tmpattr->getNextSibling();
	}

	//�����ӽڵ㼰����
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

	//���ݽڵ��������ж�
	if (m_nNodeType == NODETYPE_ATTR || m_nNodeType == NODETYPE_VALUE || m_nNodeType== NODETYPE_ROOT) //�����Ľڵ�
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
			
			//����
			CXmlAttribute *pAttribute = m_attribute.GetFirstAttribute();
			while (pAttribute != NULL)
			{
				fprintf(fp," ");
				fprintf(fp,"%s=\"%s\"",pAttribute->GetAttrName(false).c_str(),pAttribute->GetAttrValue(false).c_str());
				pAttribute=pAttribute->getNextSibling();
			}
			if (m_sNodeValue.length() >0 ) //���Խ��� >
			{
				fprintf(fp,">");
				fprintf(fp,"%s",m_sNodeValue.c_str());
			}
			else //���Խ��� 
			{
				if (m_firstChildNode == NULL) //���ӽڵ�  />
				{
					fprintf(fp," />");
					isend  = true;
				}
				else //���ӽڵ� >
				{
					fprintf(fp,">");
				}
			}
		}
		//�ӽڵ�
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

