#include "xml.h"
//#include "pubstrtime.h"
CXML::CXML()
{
	m_firstNode=NULL;
	m_curNode=NULL;
	memset(m_errMsg,0,sizeof(m_errMsg));
}
CXML::~CXML()
{
	if(m_firstNode != NULL)
	{
		deleteNode(m_firstNode);
		m_firstNode = NULL;
	}
}
CXML::CXML(char *filename)
{
	
 	FILE *fp;
	char c = 0;
	char c1 = 0;
	char *tbuf=NULL;
	long  filelen=0;
	if(m_firstNode != NULL)
	{
		deleteNode(m_firstNode);
		m_firstNode = NULL;
	}
	m_curNode = NULL;
	memset(m_errMsg,0,sizeof(m_errMsg));
	fp = fopen(filename,"rb");
	if (fp != NULL)
	{
		struct stat f_stat;
		stat(filename,&f_stat);
		filelen=f_stat.st_size;//文件长度
	    char *buffer = (char *) malloc(filelen+2);
		if (buffer == NULL)
		{
			fclose(fp);
			sprintf(m_errMsg,"分配文件大小%ld+2的内存空间失败",filelen+2);
			return ;
		}
		memset(buffer,0,filelen+2);
		tbuf = buffer;
		for (; !feof(fp); )
		{
			c = fgetc(fp);
			if (c1 == ' ' && c == ' ')
			{
				//如果空格后仍是空格，则相当于去掉后面的空格
				continue;
			}
			else
			{
				c1 = c;
				*tbuf = c;
				tbuf++;
			}
		}
		*tbuf = 0;
		fclose(fp);
		filelen = strlen(buffer);
		if (filelen>0)
		{
			if (!analysisXML(buffer,filelen))
			{
				sprintf(m_errMsg,"解析文件%s出错",filename);
			}
		}
		else
		{
			sprintf(m_errMsg,"文件不符");
		}
		free(buffer) ;
	}
}
bool CXML::fromBuffer(char *buffer)
{
	int ret = strlen(buffer);
	if(m_firstNode != NULL)
	{
		if (!deleteNode(m_firstNode))
		{
			return false;
		}
		m_firstNode = NULL;
	}	
	if (!analysisXML(buffer,ret))
	{
		return false;
	}
	return true;
}

bool CXML::toBuffer(char *buffer)
{
	if (m_firstNode == NULL)
	{
		return false;
	}
	memset(buffer,0,20);//只是置头为0
	int xmldatalen=0;
	nodeToBuffer(m_firstNode->m_firstChildNode,buffer,xmldatalen,0);
	return true;
}
bool CXML::toBufferOld(char *buffer)
{
	if (m_firstNode == NULL)
	{
		return false;
	}
	memset(buffer,0,20);//只是置头为0
	int xmldatalen=0;
	nodeToBufferOld(m_firstNode->m_firstChildNode,buffer,xmldatalen,0);
	return true;
}
bool CXML::toBuffer(char *buffer,int &bufsize)
{
	if (m_firstNode == NULL)
	{
		return false;
	}
	int xmldatalen=0;
	GetXmlDatalen(m_firstNode->m_firstChildNode,xmldatalen,0);
	if (bufsize <= xmldatalen)
	{
		sprintf(m_errMsg,"toBuffer的缓冲大小不足");
		return false;
	}
	memset(buffer,0,20);//只是置头为0
	xmldatalen=0;
	nodeToBuffer(m_firstNode->m_firstChildNode,buffer,xmldatalen,0);
	bufsize = xmldatalen;
	return true;
}

bool CXML::toFile(char *filename)
{
	FILE *fp;
	fp = fopen(filename,"wb");
	if (fp==NULL)
	{
		return false;
	}
//  *** Updated by songfree at 2008-3-12 13:24:20 ****** 
	int xmldatalen = GetXmlDatalen();
	if (xmldatalen<1)
	{
		fclose(fp);
		return false;
	}
	char *buffer = (char *)malloc (xmldatalen+1);
	if (buffer == NULL)
	{
		return false;
	}
	memset(buffer,0,sizeof(xmldatalen+1));
	if (!toBuffer(buffer))
	{
		free(buffer);
		fclose(fp);
		return false;
	}
	fprintf(fp,"<?xml version=%c1.0%c encoding=%cGB2312%c?>\n",'"','"','"','"');
	fprintf(fp,buffer);
	free(buffer);
	fflush(fp);
	fclose(fp);
	return true;
}
int CXML::nodeToBuffer(CNode *pnode,char *buffer,int &xmldatalen,int tier)
{
	int ret;
	char tm[80];
	bool isend=false;
//	Cpubstrtime publog;
	int i;
	int j;
	if (pnode == NULL)
	{
		return xmldatalen;
	}

	for(i=0, tm[0]=0; i<tier; i++)
	{
		sprintf(tm,"%s%s",tm,TAB);
	}
	if(buffer[0] == 0)
	{
		ret = strlen(tm);
		for (j=0;j<ret;j++,xmldatalen++)
		{
			buffer[xmldatalen] = tm[j];
		}
		buffer[xmldatalen] = '<' ;
		xmldatalen +=1;
		for (j=0;j< (int)pnode->m_name.length();j++,xmldatalen++)
		{
			buffer[xmldatalen] = pnode->m_name.c_str()[j];
		}

	}
	else
	{
		buffer[xmldatalen] = '\n';
		xmldatalen++;
		ret = strlen(tm);
		for (j=0;j<ret;j++,xmldatalen++)
		{
			buffer[xmldatalen] = tm[j];
		}
		buffer[xmldatalen] = '<';
		xmldatalen++;
		for (j=0;j<(int)pnode->m_name.length();j++,xmldatalen++)
		{
			buffer[xmldatalen] = pnode->m_name.c_str()[j];
		}

	}
//	publog.errlog("test",__FILE__,__LINE__,0,buffer);
	CAttribute *pAttribute;
	pAttribute = pnode->m_attribute;

	while (pAttribute != NULL)
	{
		/*---------属性值增加引号------------------------------
		sprintf(buffer,"%s %s=%s ",\
			buffer,pAttribute->m_name,pAttribute->m_value);
		------------------------------------------------------*/
/* *** Updated by songfree at 2008-6-19 17:19:20 ****** 
		sprintf(buffer,"%s %s=\"%s\"",\
			buffer,pAttribute->m_name.c_str(),\
			pAttribute->m_value.c_str());
*******************************************************/
		buffer[xmldatalen] = ' ';
		xmldatalen++;
		for (j=0;j<(int)pAttribute->m_name.length();j++,xmldatalen++)
		{
			buffer[xmldatalen] = pAttribute->m_name.c_str()[j];
		}
		buffer[xmldatalen] = '=';
		xmldatalen++;
		buffer[xmldatalen] = '\"';
		xmldatalen++;
		for (j=0;j<(int)pAttribute->m_value.length();j++,xmldatalen++)
		{
			buffer[xmldatalen] = pAttribute->m_value.c_str()[j];
		}
		buffer[xmldatalen] = '\"';
		xmldatalen++;


		//-----------------------------------------------------
		pAttribute=pAttribute->m_next;
	}
//	publog.errlog("test",__FILE__,__LINE__,0,buffer);
	if (pnode->m_value.length() != 0)
	{
/* *** Updated by songfree at 2008-6-19 17:22:04 ****** 
		sprintf(buffer,"%s>%s", buffer, \
			pnode->m_value.c_str());
*******************************************************/
		buffer[xmldatalen] = '>';
		xmldatalen++;
		for (j=0;j<(int)pnode->m_value.length();j++,xmldatalen++)
		{
			buffer[xmldatalen] = pnode->m_value.c_str()[j];
		}

	}
	else
	{
/* *** Updated by songfree at 2006-12-22 13:23:17 ****** 
		sprintf(buffer,"%s>", buffer);
*******************************************************/

 
		if (pnode->m_firstChildNode == NULL)
		{
/* *** Updated by songfree at 2008-6-19 17:23:06 ****** 
			sprintf(buffer,"%s />", buffer);
*******************************************************/
			buffer[xmldatalen] = ' ';
			xmldatalen++;
			buffer[xmldatalen] = '/';
			xmldatalen++;
			buffer[xmldatalen] = '>';
			xmldatalen++;
			isend  = true;
		}
		else
		{
/* *** Updated by songfree at 2008-6-19 17:23:43 ****** 
			sprintf(buffer,"%s>", buffer);
*******************************************************/
			buffer[xmldatalen] = '>';
			xmldatalen++;
		}

	
	}


	nodeToBuffer(pnode->m_firstChildNode,buffer,xmldatalen,tier+1);


	if (pnode->m_value.length() == 0)
	{
/* *** Updated by songfree at 2006-12-22 13:23:03 ****** 
		sprintf(buffer,"%s\n%s</%s>",buffer, tm, \
			pnode->m_name.c_str());
*******************************************************/
 
		if (!isend)
		{
/* *** Updated by songfree at 2008-6-19 17:24:57 ****** 
			sprintf(buffer,"%s\n%s</%s>",buffer, tm, \
				pnode->m_name.c_str());
*******************************************************/
			buffer[xmldatalen] = '\n';
			xmldatalen++;
			ret = strlen(tm);
			for (j=0;j<ret;j++,xmldatalen++)
			{
				buffer[xmldatalen] = tm[j];
			}
			buffer[xmldatalen] = '<';
			xmldatalen++;
			buffer[xmldatalen] = '/';
			xmldatalen++;
			for (j=0;j<(int)pnode->m_name.length();j++,xmldatalen++)
			{
				buffer[xmldatalen] = pnode->m_name.c_str()[j];
			}
			buffer[xmldatalen] = '>';
			xmldatalen++;

		}
		
	}
	else
	{
/* *** Updated by songfree at 2008-6-19 17:27:12 ****** 
		sprintf(buffer,"%s</%s>",buffer, pnode->m_name.c_str());
*******************************************************/
		buffer[xmldatalen] = '<';
		xmldatalen++;
		buffer[xmldatalen] = '/';
		xmldatalen++;
		for (j=0;j<(int)pnode->m_name.length();j++,xmldatalen++)
		{
			buffer[xmldatalen] = pnode->m_name.c_str()[j];
		}
		buffer[xmldatalen] = '>';
		xmldatalen++;

	}

	nodeToBuffer(pnode->m_nextNode,buffer,xmldatalen,tier);
	buffer[xmldatalen] =0;
	return xmldatalen;
}
int CXML::nodeToBufferOld(CNode *pnode,char *buffer,int &xmldatalen,int tier)
{
	int ret;
	char tm[80];
	bool isend=false;
//	Cpubstrtime publog;
	int i;
	int j;
	if (pnode == NULL)
	{
		return xmldatalen;
	}

	for(i=0, tm[0]=0; i<tier; i++)
		sprintf(tm,"%s%s",tm,TAB);
	if(buffer[0] == 0)
	{
		ret = strlen(tm);
		for (j=0;j<ret;j++,xmldatalen++)
		{
			buffer[xmldatalen] = tm[j];
		}
		buffer[xmldatalen] = '<' ;
		xmldatalen +=1;
		for (j=0;j<(int)pnode->m_name.length();j++,xmldatalen++)
		{
			buffer[xmldatalen] = pnode->m_name.c_str()[j];
		}

	}
	else
	{

		buffer[xmldatalen] = '\n';
		xmldatalen++;
		ret = strlen(tm);
		for (j=0;j<ret;j++,xmldatalen++)
		{
			buffer[xmldatalen] = tm[j];
		}
		buffer[xmldatalen] = '<';
		xmldatalen++;
		for (j=0;j<(int)pnode->m_name.length();j++,xmldatalen++)
		{
			buffer[xmldatalen] = pnode->m_name.c_str()[j];
		}

	}

	CAttribute *pAttribute;
	pAttribute = pnode->m_attribute;

	while (pAttribute != NULL)
	{
		buffer[xmldatalen] = ' ';
		xmldatalen++;
		for (j=0;j<(int)pAttribute->m_name.length();j++,xmldatalen++)
		{
			buffer[xmldatalen] = pAttribute->m_name.c_str()[j];
		}
		buffer[xmldatalen] = '=';
		xmldatalen++;
		buffer[xmldatalen] = '\"';
		xmldatalen++;
		for (j=0;j<(int)pAttribute->m_value.length();j++,xmldatalen++)
		{
			buffer[xmldatalen] = pAttribute->m_value.c_str()[j];
		}
		buffer[xmldatalen] = '\"';
		xmldatalen++;


		//-----------------------------------------------------
		pAttribute=pAttribute->m_next;
	}


	if (pnode->m_value.length() != 0)
	{

		buffer[xmldatalen] = '>';
		xmldatalen++;
		for (j=0;j<(int)pnode->m_value.length();j++,xmldatalen++)
		{
			buffer[xmldatalen] = pnode->m_value.c_str()[j];
		}


	}
	else
	{
		if (pnode->m_firstChildNode == NULL)
		{
// 			buffer[xmldatalen] = ' ';
// 			xmldatalen++;
// 			buffer[xmldatalen] = '/';
// 			xmldatalen++;
			buffer[xmldatalen] = '>';
			xmldatalen++;
			isend  = true;
		}
		else
		{
			buffer[xmldatalen] = '>';
			xmldatalen++;
		}
	
	}




	nodeToBufferOld(pnode->m_firstChildNode,buffer,xmldatalen,tier+1);



	if (pnode->m_value.length() == 0)
	{
 
		if (!isend)
		{
			buffer[xmldatalen] = '\n';
			xmldatalen++;
			ret = strlen(tm);
			for (j=0;j<ret;j++,xmldatalen++)
			{
				buffer[xmldatalen] = tm[j];
			}
			buffer[xmldatalen] = '<';
			xmldatalen++;
			buffer[xmldatalen] = '/';
			xmldatalen++;
			for (j=0;j<(int)pnode->m_name.length();j++,xmldatalen++)
			{
				buffer[xmldatalen] = pnode->m_name.c_str()[j];
			}
			buffer[xmldatalen] = '>';
			xmldatalen++;

		}
		else
		{
			buffer[xmldatalen] = '<';
			xmldatalen++;
			buffer[xmldatalen] = '/';
			xmldatalen++;
			for (j=0;j<(int)pnode->m_name.length();j++,xmldatalen++)
			{
				buffer[xmldatalen] = pnode->m_name.c_str()[j];
			}
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
		for (j=0;j<(int)pnode->m_name.length();j++,xmldatalen++)
		{
			buffer[xmldatalen] = pnode->m_name.c_str()[j];
		}
		buffer[xmldatalen] = '>';
		xmldatalen++;


	}
	nodeToBufferOld(pnode->m_nextNode,buffer,xmldatalen,tier);
	buffer[xmldatalen] =0;
	return xmldatalen;
}
//##ModelId=3EDC16120074
//## 分析XML数据报
bool CXML::analysisXML(const char *buffer,int bufferlen)
{
	CNode *headNode;


	headNode= new CNode;
	headNode->m_nextNode=NULL;
	headNode->m_parentNode=NULL;
	headNode->m_firstChildNode=NULL;

	m_xmlAnalysis.analyseXML(buffer,bufferlen,headNode);
	if (m_firstNode != NULL)
	{
		deleteNode(m_firstNode);
		m_firstNode = NULL;
	}
	m_firstNode=headNode;
	m_curNode=headNode;
	return true;
}

CNode *CXML::getRoot()
{
	m_curNode=m_firstNode;
	return m_firstNode;
}

CNode *CXML::getFirstChildItem(const std::string item)
{
	CNode  *pnode;

	if(m_curNode == NULL)
	{
		return NULL;
	}
	std::string ss = item;
	putTransSign(ss) ;
	pnode=m_curNode->m_firstChildNode;
	while (pnode != NULL)
	{
		if (pnode->m_name.compare(ss)==0)
		{
			break;
		}
		pnode=pnode->m_nextNode;
	}
	if (pnode != NULL)
	{
		m_curNode=pnode;
	}
	return pnode;
}

CNode *CXML::getFirstChildItem()
{
	if(m_curNode==NULL)
	{
		return m_curNode;
	}

	m_curNode = m_curNode->getFirstChild();

	return m_curNode;
}

CNode *CXML::getFirstItem(const std::string item)
{
	CNode *pnode;

	if (m_curNode == NULL)
	{
		return NULL;
	}

	pnode=m_curNode->m_parentNode;
	std::string ss = item;
	putTransSign(ss);
	pnode=getFirstChildItem(ss);
	if (pnode != NULL)
	{
		m_curNode=pnode;
	}

	return pnode;
}

CNode *CXML::getFirstItem()
{
	if (m_curNode == NULL)
	{
		return NULL;
	}
	m_curNode=m_curNode->m_parentNode->m_firstChildNode;
	return m_curNode;
}

//##ModelId=3EDC254301A2
CNode *CXML::getNextSibling(const std::string item)
{
	CNode *pnode;

	if (m_curNode == NULL)
	{
		return NULL;
	}

	pnode=m_curNode->m_nextNode;
	std::string ss = item;
	putTransSign(ss);
	while (pnode != NULL)
	{
		if (pnode->m_name.compare(ss)==0)
		{
			break;
		}
/* *** Updated by songfree at 2006-6-2 9:46:32 ****** 
		if (!strcmp(pnode->m_name.c_str(), item.c_str()))
		{
			break;
		}
*******************************************************/

		pnode=pnode->m_nextNode;
	}
	if (pnode != NULL)
	{
		m_curNode=pnode;
	}

	return pnode;
}

CNode *CXML::getNextSibling()
{
	m_curNode = m_curNode->getNextSibling();

	return m_curNode;
}

bool CXML::getAttribute(const std::string name, std::string &value)
{
	CAttribute *pAttribute;
	if (m_curNode == NULL)
	{
		return false;
	}
	pAttribute=m_curNode->m_attribute;
	std::string ss = name;
	putTransSign(ss);
	while (pAttribute != NULL)
	{
		if (pAttribute->m_name.compare(ss)==0)
		{
			value = pAttribute->m_value;
			getTransSign(value);
			break;
		}
/* *** Updated by songfree at 2006-6-2 9:49:11 ****** 
		if (!strcmp(name.c_str(), pAttribute->m_name.c_str()))
		{
			value = pAttribute->m_value;
			break;
		}
*******************************************************/

		pAttribute=pAttribute->m_next;
	}
	if (pAttribute == NULL)
	{
		return false;
	}

	return true;
}

CAttribute *CXML::getAttributes()
{
	return m_curNode->getAttributes();
}
	
bool CXML::setAttribute(const std::string name,const std::string value)
{
	CAttribute *pAttribute,*last;
	if (m_curNode == NULL)
	{
		return false;
	}
	pAttribute=m_curNode->m_attribute;
	std::string ss = name;
	putTransSign(ss);
	while (pAttribute != NULL)
	{
		if (pAttribute->m_name.compare(ss)==0)
		{
			pAttribute->m_value = value;
			putTransSign(pAttribute->m_value);
			break;
		}
/* *** Updated by songfree at 2006-6-2 9:50:20 ****** 
		if (!strcmp(name.c_str(), pAttribute->m_name.c_str()))
		{
			pAttribute->m_value = value;
			break;
		}
*******************************************************/

		pAttribute=pAttribute->m_next;
	}
	if(pAttribute == NULL)
	{
		pAttribute=new CAttribute;
		pAttribute->m_name.empty();
		pAttribute->m_value.empty();
		pAttribute->m_next=NULL;
		pAttribute->m_name = ss;
		pAttribute->m_value = value;
		putTransSign(pAttribute->m_value);
		last=m_curNode->m_attribute;
		if (last == NULL)
		{
			m_curNode->m_attribute=pAttribute;
		}
		else
		{
			while (last->m_next != NULL)
			{
				last=last->m_next;
			}
			last->m_next=pAttribute;
		}
	}
	return true;
}

bool CXML::delAttribute(const std::string name)
{
	CAttribute *pAttribute,*prev;
	if(m_curNode == NULL)
	{
		return false;
	}
	pAttribute=m_curNode->m_attribute;
	prev=NULL;
	std::string ss = name;
	putTransSign(ss);
	while (pAttribute != NULL)
	{
		if (pAttribute->m_name.compare(ss)==0)
/* *** Updated by songfree at 2006-6-2 9:54:29 ****** 
		if (!strcmp(name.c_str(), pAttribute->m_name.c_str()))
*******************************************************/
		{
			if (prev != NULL)
			{
				prev->m_next=pAttribute->m_next;
			}
			else
			{
				m_curNode->m_attribute=NULL;
			}
			delete pAttribute;
			break;
		}
		prev=pAttribute;
		pAttribute=pAttribute->m_next;
	}
	return true;
}

bool CXML::delAttribute(CNode *pNode)
{
	if (this==NULL) return false;
	CAttribute *pAttribute,*cur;
	if (pNode == NULL)
	{
		return false;
	}
	pAttribute=pNode->m_attribute;
	cur=NULL;
	while (pAttribute != NULL)
	{
		cur=pAttribute;
		pAttribute=pAttribute->m_next;
		delete cur;
	}
	pNode->m_attribute=NULL;
	return true;
}
//##ModelId=3EDC258A02C7
CNode *CXML::addItem(const std::string name, const std::string value)
{
	CNode *pnode;
	if (m_firstNode == NULL)
	{
		m_firstNode=new CNode;
		m_firstNode->m_nextNode=NULL;
		m_firstNode->m_parentNode=NULL;
		m_firstNode->m_firstChildNode=NULL;
		m_firstNode->m_endChildNode = NULL;
		m_firstNode->m_lastNode = NULL;
		m_curNode=m_firstNode;
	}

	if(m_curNode == NULL)
	{
		return NULL;
	}
	pnode=new CNode;
	pnode->m_name = name;
	putTransSign(pnode->m_name);
	pnode->m_value = value;
	putTransSign(pnode->m_value);
	if (!AddNode(pnode))
	{
		delete pnode;
		pnode = NULL;
	}
	return pnode;
}
CNode *CXML::addItemN(const std::string name, const std::string value)
{
	CNode *pnode;
	if (m_firstNode == NULL)
	{
		m_firstNode=new CNode;
		m_firstNode->m_nextNode=NULL;
		m_firstNode->m_parentNode=NULL;
		m_firstNode->m_firstChildNode=NULL;
		m_firstNode->m_endChildNode = NULL;
		m_firstNode->m_lastNode = NULL;

		m_curNode=m_firstNode;
	}

	if(m_curNode == NULL)
	{
		return NULL;
	}

	pnode=new CNode;
	pnode->m_name = name;
	pnode->m_value = value;
	if (!AddNode(pnode))
	{
		delete pnode;
		pnode = NULL;
	}
	return pnode;
}
CNode *CXML::addItem(const std::string name, const std::string value,int i)
{
	CNode *pnode;

	if(m_firstNode==NULL)
	{
		m_firstNode=new CNode;
		m_firstNode->m_nextNode=NULL;
		m_firstNode->m_parentNode=NULL;
		m_firstNode->m_firstChildNode=NULL;
		m_firstNode->m_endChildNode = NULL;
		m_firstNode->m_lastNode = NULL;
		m_curNode=m_firstNode;
	}

	if (m_curNode == NULL)
	{
		return NULL;
	}

	pnode=new CNode;
	pnode->m_name = name;
	putTransSign(pnode->m_name);
	pnode->m_value = value.substr(0,i);
	putTransSign(pnode->m_value);

	if (!AddNode(pnode))
	{
		delete pnode;
		pnode = NULL;
	}

	return pnode;
}

//##ModelId=3EDC26B501A4
bool CXML::editItem(const std::string value)
{
	if (m_curNode == NULL)
	{
		return false;
	}
	m_curNode->m_value = value;
	putTransSign(m_curNode->m_value);
	return true;
}
bool CXML::editItemN(const std::string value)
{
	if (m_curNode == NULL)
	{
		return false;
	}
	m_curNode->m_value = value;

	return true;
}
bool CXML::editItem(const std::string value,int i)
{
	if (m_curNode == NULL)
	{
		return false;
	}
	m_curNode->m_value = value.substr(0,i);
	putTransSign(m_curNode->m_value);
	return true;
}


bool CXML::deleteItem()
{
	CNode *pnode=NULL;
	if (m_curNode == NULL)
	{
		return false;
	}
	pnode=m_curNode;
	getParentItem();
	deleteNode(pnode);
	pnode = NULL;
	return true;
}

bool CXML::deleteNode(CNode *pnode)
{
	if (pnode == NULL)
	{
		return true;
	}
	if (m_curNode == pnode)
	{
		m_curNode = m_firstNode;
	}
	
	delAttribute(pnode);//删除此节点的属性
	deleteNodePoint(pnode);//删除此节点在链表的关系
	while (pnode->m_firstChildNode != NULL) //删除此节点所有子节点
	{
		deleteNode(pnode->m_firstChildNode);//将第一个子节点删除，那么第二个就成为第一个，循环一直删除
	}
	delete pnode;
	pnode = NULL;
	
	return true;
}
bool CXML::deleteNodePoint(CNode *pnode)
{
	if (pnode == NULL)
	{
		return true;
	}
	if (m_curNode == pnode )
	{
		m_curNode = m_firstNode;
	}
	if (m_firstNode == pnode)
	{
		m_curNode = NULL;
		m_firstNode = NULL;
	}
	CNode *parentNode= NULL;
	//去掉父节点
	parentNode = pnode->m_parentNode;
	if (parentNode != NULL) 
	{
		if (parentNode->m_endChildNode == pnode && parentNode->m_firstChildNode == pnode) //父节点只有它一个节点
		{
			parentNode->m_endChildNode = NULL;
			parentNode->m_firstChildNode = NULL;
			pnode->m_parentNode = NULL;
		}
		else if (parentNode->m_endChildNode == pnode ) //是最后一个节点
		{
			parentNode->m_endChildNode = pnode->m_lastNode;
			pnode->m_lastNode->m_nextNode = NULL;
			pnode->m_lastNode = NULL;
			pnode->m_nextNode = NULL;//最后一个节点肯定是NULL啦
		}
		else if (parentNode->m_firstChildNode == pnode) //是最前一个节点
		{
			parentNode->m_firstChildNode = pnode->m_nextNode;
			pnode->m_nextNode->m_lastNode = NULL;
			pnode->m_nextNode = NULL;
			pnode->m_lastNode = NULL;//最后一个节点肯定是NULL啦
		}
		else //是中间的节点
		{
			pnode->m_nextNode->m_lastNode = pnode->m_lastNode;
			pnode->m_lastNode->m_nextNode = pnode->m_nextNode;
			pnode->m_nextNode = NULL;
			pnode->m_lastNode = NULL;
		}
		pnode->m_parentNode = NULL;
	}
	else
	{
		if (pnode->m_nextNode == NULL && pnode->m_lastNode == NULL) //前后节点都为空，什么都不做
		{

		}
		else if (pnode->m_nextNode == NULL)//是最后一个节点
		{
			pnode->m_lastNode->m_nextNode = NULL;
			pnode->m_lastNode = NULL;
		}
		else if (pnode->m_lastNode == NULL) //是最前一个节点
		{
			pnode->m_nextNode->m_lastNode = NULL;
			pnode->m_nextNode = NULL;
		}
		else //是中间的节点
		{
			pnode->m_nextNode->m_lastNode = pnode->m_lastNode;
			pnode->m_lastNode->m_nextNode = pnode->m_nextNode;
			pnode->m_nextNode = NULL;
			pnode->m_lastNode = NULL;
		}
	}
	
	return true;
}
CNode *CXML::getParentItem()
{
	if (m_curNode == NULL)
	{
		return NULL;
	}
	if (m_curNode->m_parentNode==NULL)
	{
		return NULL;
	}
	m_curNode=m_curNode->m_parentNode;
	return m_curNode;
}

CNode *CXML::getCurrentNode()
{
	return m_curNode;
}

CNode *CXML::setCurrentNode(CNode *pNode)
{
	m_curNode = pNode;

	return m_curNode;
}

std::string CXML::getNodeName()
{
	std::string ss;
	ss = m_curNode->getName();
	return ss;
}
std::string CXML::getNodeValue()
{
	std::string ss;
	ss = m_curNode->getValue();
	return ss;
}

int CXML::fromFile(char *filename)
{
	FILE *fp;
	char c,c1 = 0;
	char *tbuf;
	int filelen;
	
	if (m_firstNode!=NULL)
	{
		deleteNode(m_firstNode);
		m_firstNode = NULL;
	}
	m_curNode = NULL;
	fp = fopen(filename,"rb");
	if (fp != NULL)
	{
		struct stat f_stat;
		stat(filename,&f_stat);
		filelen=f_stat.st_size;//文件长度
	    char *buffer = (char *) malloc(filelen+2);
		if (buffer == NULL)
		{
			fclose(fp);
			return -1;
		}
		memset(buffer,0,filelen+2);
		for (tbuf = buffer; !feof(fp); )
		{
			c = fgetc(fp);
// 			if (c1 == ' ' && c == ' ')
// 			{
// 				//如果空格后仍是空格，则相当于去掉后面的空格
// 				continue;
// 			}
// 			else
// 			{
				c1 = c;
				*tbuf = c;
				tbuf++;
//			}
		}
		*tbuf = 0;
		fclose(fp);
//		printf("%s %d\n",filename,strlen(m_buffer));
		filelen = strlen(buffer);
		if (filelen >0)
		{
			if (!analysisXML(buffer,filelen))
			{
				free(buffer) ;
				return -1;
			}
			free(buffer) ;
			return 0;
		}
		free(buffer) ;
		return -1;
	}
	return -1;
}

CNode * CXML::getEndChildItem()
{
	if(m_curNode==NULL)
	{
		return NULL;
	}
	m_curNode = m_curNode->getEndChild();
	
	return m_curNode;
}

CNode * CXML::getLastSibling()
{
	m_curNode = m_curNode->getLastSibling();
	
	return m_curNode;
}
//转义符
//&quot; "
//&lt; <
//&gt; >
//&amp; &
//&apos; '
int CXML::putTransSign(std::string &value)
{
// 	char *tmpchar = (char *)malloc(value.length()*2);
// 	memset(tmpchar,0,value.length()*2);
	char tmpchar[NODEVALUELEN];
	memset(tmpchar,0,sizeof(tmpchar));
	char *p = tmpchar;
	for (int i=0;i<(int)value.length();i++)
	{
		if (value.c_str()[i] == '"' )
		{
			strcpy(p,"&quot;");
			p+=6;
		}
		else if (value.c_str()[i] == '<' )
		{
			strcpy(p,"&lt;");
			p+=4;
		}
		else if (value.c_str()[i] == '>' )
		{
			strcpy(p,"&gt;");
			p+=4;
		}
		else if (value.c_str()[i] == '&' && strncmp(value.c_str()+i,"&quot;",6)!=0 \
			&& strncmp(value.c_str()+i,"&lt;",4)!=0 && strncmp(value.c_str()+i,"&gt;",4)!=0 \
			&& strncmp(value.c_str()+i,"&amp;",5)!=0 && strncmp(value.c_str()+i,"&apos",5)!=0)
		{
			strcpy(p,"&amp;");
			p+=5;
		}
		else if (value.c_str()[i] == 39 )
		{
			strcpy(p,"&apos;");
			p+=6;
		}
		else
		{
			*p = value.c_str()[i];
			p++;
		}
	}
	*(p+1)= 0;
	value = tmpchar;
//	free(tmpchar);
//	tmpchar = NULL;
	return 0;
	
}
//转义符
//&quot; "
//&lt; <
//&gt; >
//&amp; &
//&apos; '
int CXML::getTransSign(std::string &value)
{
	int nLength;
//	char *tmpchar = (char *)malloc(value.length()*2);
//	memset(tmpchar,0,value.length()*2);
	char tmpchar[NODEVALUELEN];
	memset(tmpchar,0,sizeof(tmpchar));
	nLength = value.length();
	char *p = tmpchar;
	for (int i=0;i<nLength;)
	{
		if (nLength >= i+4)
		{
			if (strncmp((char *)value.c_str()+i,"&lt;",4)==0)
			{
				*p= '<' ;
				p++;
				i+=4;
				continue;
			}
			if (strncmp((char *)value.c_str()+i,"&gt;",4)==0)
			{
				*p= '>' ;
				p++;
				i+=4;
				continue;
			}
			if (nLength >= i+5)
			{
				if (strncmp((char *)value.c_str()+i,"&amp;",5)==0)
				{
					*p= '&' ;
					p++;
					i+=5;
					continue;
				}
			}
			if (nLength >= i+6)
			{
				if (strncmp((char *)value.c_str()+i,"&quot;",6)==0)
				{
					*p= '"' ;
					p++;
					i+=6;
					continue;
				}
				if (strncmp((char *)value.c_str()+i,"&apos;",6)==0)
				{
					*p= 39 ;
					p++;
					i+=6;
					continue;
				}
			}

			*p = value.c_str()[i];
			//*p = '1';
			p++;
			i++;
		}
		else
		{
			*p = value.c_str()[i];
			p++;
			i++;
		}
	}
	value = tmpchar;
//	free(tmpchar);
//	tmpchar = NULL;
	return 0;
}

int CXML::GetXmlDatalen()
{
	int datalen=0;
	int flag=0;
	if (m_firstNode == NULL)
	{
		return 0;
	}
	GetXmlDatalen(m_firstNode->m_firstChildNode,datalen,0);
	return datalen;
}
int CXML::GetXmlDatalen(CNode *pnode,int &xmldatalen, int tier)
{
	int ret;
	char tm[80];
	bool isend=false;
	int i;
	if (pnode == NULL)
	{
		return xmldatalen;
	}

	for(i=0, tm[0]=0; i<tier; i++)
	{
		sprintf(tm,"%s%s",tm,TAB);
	}
	if(xmldatalen == 0)
	{
		ret = strlen(tm);
		xmldatalen +=ret;
		xmldatalen +=1;
		xmldatalen += pnode->m_name.length();
	}
	else
	{
		ret = pnode->m_name.length();
		ret = strlen(tm);
		xmldatalen +=ret;
		xmldatalen +=2;
		xmldatalen += pnode->m_name.length();
	}

	CAttribute *pAttribute;
	pAttribute = pnode->m_attribute;

	while (pAttribute != NULL)
	{
		xmldatalen +=4;
		ret = pAttribute->m_name.length();
		xmldatalen += pAttribute->m_name.length();
		ret = pAttribute->m_value.length();
		xmldatalen += pAttribute->m_value.length();
		pAttribute=pAttribute->m_next;
	}
	if (pnode->m_value.length() > 0)
	{
		xmldatalen +=1;
		xmldatalen += pnode->m_value.length();
	}
	else
	{
		if (pnode->m_firstChildNode == NULL)
		{
			xmldatalen +=3;
			isend  = true;
		}
		else
		{
			xmldatalen+=1;
		}
	}
	GetXmlDatalen(pnode->m_firstChildNode,xmldatalen,tier+1);
	if (pnode->m_value.length() == 0)
	{ 
		if (!isend)
		{
			xmldatalen +=4;
			xmldatalen += pnode->m_name.length();
			ret = strlen(tm);
			xmldatalen +=ret;
		}
	}
	else
	{
		xmldatalen +=3;
		xmldatalen += pnode->m_name.length();
	}
	GetXmlDatalen(pnode->m_nextNode,xmldatalen,tier);
	return xmldatalen;
}

bool CXML::toString(std::string &buffer)
{
	if (m_firstNode == NULL)
	{
		return false;
	}
	buffer ="";
	int datalen = GetXmlDatalen();
	if (datalen<1)
	{
		return false;
	}
	datalen = datalen+10;
	char *xmldata = (char *)malloc(datalen);
	if (xmldata == NULL)
	{
		return false;
	}
	memset(xmldata,0,datalen);
	if (!toBuffer(xmldata,datalen))
	{
		free(xmldata);
		return false;
	}
	buffer = xmldata;
	free(xmldata);
	return true;
}

bool CXML::XmlCopy(CXML *srcxml)
{
	if (!deleteNode(m_firstNode))
	{
		return false;
	}
	m_firstNode = NULL;	
	if (srcxml->m_firstNode != NULL)
	{
		XmlCopyNode(srcxml->m_firstNode->getFirstChild());
	}
	return true;
}
bool CXML::XmlPointCopy(CXML *srcxml)
{
	if (m_firstNode!= NULL)
	{
		deleteNode(m_firstNode);
		m_firstNode = NULL;
	}
	m_firstNode = srcxml->m_firstNode;
	srcxml->setCurrentNode(NULL);
	srcxml->m_firstNode = NULL;
	m_curNode = m_firstNode;
	return true;
}
bool CXML::XmlCopyNode(CNode *srcnode,bool iscopysrc)
{
	if (srcnode == NULL)
	{
		return true;
	}
	CNode *curnode=NULL;
	if (iscopysrc)
	{
		//copy节点 无面进行转义符转换
		curnode = addItemN(srcnode->getName(),srcnode->getValue());
		if (curnode == NULL)
		{
			return false;
		}
		//copy节点属性
		CAttribute *pattr = srcnode->getAttributes();
		while(pattr!=NULL)
		{
			curnode->SetAttribute(pattr->getName(),pattr->getValue());
			pattr = pattr->m_next;
		}
	}
	else
	{
		curnode = m_curNode;
	}
	

	//保存当前节点
	CNode *lastNode = m_curNode;
	setCurrentNode(curnode);
	CNode *stmpnode = srcnode->getFirstChild();
	CNode *dtmpnode = NULL;	
	if (stmpnode != NULL)
	{
		if (!XmlCopyNode(stmpnode))
		{
			return false;
		}
	}
	//恢复当前节点
	setCurrentNode(lastNode);
	stmpnode = srcnode->getNextSibling();
	if (stmpnode != NULL)
	{
		if (!XmlCopyNode(stmpnode))
		{
			return false;
		}
		stmpnode = stmpnode->getNextSibling();
	}
	//恢复当前节点
	setCurrentNode(lastNode);
	return true;
}
bool CXML::AddNode(CNode *anode,bool first)
{
	if (m_firstNode == NULL)
	{
		m_firstNode= anode;
		m_firstNode->m_nextNode=NULL;
		m_firstNode->m_parentNode=NULL;
		m_firstNode->m_lastNode = NULL;
		m_curNode=m_firstNode;
		return true;
	}
	
	if(m_curNode == NULL)
	{
		sprintf(m_errMsg,"当前节点为空");
		return false;
	}
	if (first)
	{
		anode->m_lastNode = NULL;
		if (m_curNode->m_firstChildNode != NULL) //不是第一个子节点
		{
			m_curNode->m_firstChildNode->m_lastNode = anode;//原第一节点的上节点为此节点
			anode->m_nextNode=m_curNode->m_firstChildNode; //此节点的临近节点为原第一子节点
		}
		else
		{
			m_curNode->m_endChildNode = anode; 
			anode->m_nextNode = NULL;
		}
		anode->m_parentNode=m_curNode;//父节点为当前节点
		m_curNode->m_firstChildNode=anode;//当前节点的第一个子节点为此节点
	}
	else
	{
		anode->m_nextNode = NULL;
		if (m_curNode->m_endChildNode != NULL) //不是最后子节点
		{
			m_curNode->m_endChildNode->m_nextNode = anode;//原最后一节点的下节点为此节点
			anode->m_lastNode=m_curNode->m_endChildNode; //此节点的临近上节点为原最后一子节点
		}
		else
		{
			m_curNode->m_firstChildNode = anode; 
			anode->m_lastNode = NULL;
		}
		anode->m_parentNode=m_curNode;//父节点为当前节点
		m_curNode->m_endChildNode=anode;//当前节点的最后一个子节点为此节点
	}

	return true;
}

bool CXML::ReplaceNode(CNode *newnode, CNode *oldnode)
{
	newnode->m_parentNode = oldnode->m_parentNode;
	if (newnode->m_parentNode != NULL)
	{
		if (newnode->m_parentNode->m_firstChildNode == oldnode )
		{
			newnode->m_parentNode->m_firstChildNode = newnode;
		}
		if (newnode->m_parentNode->m_endChildNode == oldnode)
		{
			newnode->m_parentNode->m_endChildNode = newnode;
		}
		oldnode->m_parentNode = NULL;
	}
	
	newnode->m_lastNode = oldnode->m_lastNode;
	oldnode->m_lastNode = NULL;
	newnode->m_nextNode = oldnode->m_nextNode;
	oldnode->m_nextNode = NULL;
	return true;
}
CNode *CXML::LocateRoot()
{
	m_curNode=m_firstNode;
	return m_firstNode;
}
CNode *CXML::LocateFirstByName(char *pString)
{

	if(m_curNode == NULL || strlen(pString)==0 )
	{
		return NULL;
	}

	CNode  *pNode;
	char   *pNodeName;
	std::string  name;
	CNode *pRootNode=LocateRoot();
	pNode=pRootNode;
	CNode *pTmpNode=NULL;
	for (int i=1;;i++)
	{
		pNodeName=GetCol(pString, i);
		if (strlen(pNodeName) == 0)
			break;
		pNode=pNode->getFirstChild();
		name = pNodeName;
		if (pTmpNode=CompLayerNode(pNode,name))
		{
			pNode=pTmpNode;
		}
		else
			return NULL;
	}
	if (pNode==pRootNode)
		return NULL;
	else
		return pNode;
}

char *CXML::GetCol(char *src, int index)
{
	memset(m_buf,0,sizeof(m_buf));
	int i,j,k;
	i=0;
	j=0;
	while (src[i]!='\0')
	{
		if (src[i]=='.') j++;
		if (j==index)break;
		i++;
	}
	if (i==0) return (char *)"";
	if (j==index || j==index-1&&src[i]=='\0')
	{
		k=i-1;
		while(src[k]!='.' &&k>=0) k--;
		if (k+1==i) return (char*)"";
		memset (m_buf,0,sizeof(m_buf));
		memcpy(m_buf, &src[k+1], i-k-1);
		return m_buf;
	}
	return (char*)"";
}


CNode *CXML::CompLayerNode(CNode *pFirst,const std::string item)
{
	CNode *pTmpNode=NULL;
	if (pFirst)
		pTmpNode=pFirst;
	else
		return NULL;
	do
	{
		if(!strcmp(pTmpNode->m_name.c_str(),item.c_str()))
			return pTmpNode;
	}while(pTmpNode=pTmpNode->getNextSibling());
	return NULL;
}