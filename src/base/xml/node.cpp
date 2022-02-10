#include "public.h"
#include "attribute.h"
#include "node.h"

CNode::CNode()
{
	m_firstChildNode=NULL;
	m_nextNode=NULL;
	m_lastNode=NULL;
	m_parentNode=NULL;
	m_endChildNode = NULL;
	m_attribute=NULL;

}

CNode::~CNode()
{
	if (m_attribute!=NULL)
	{
		CAttribute *pattr = m_attribute;
		while (pattr!=NULL)
		{
			CAttribute *pattrtmp = pattr;
			pattr = pattr->m_next;
			delete pattrtmp;
			pattrtmp = NULL;
		}
		m_attribute = NULL;
	}
	
}

std::string CNode::getValue()
{
	std::string ss;
	ss = m_value;
	getTransSign(ss);
	return ss;
}
std::string CNode::getName()
{
	std::string ss;
	ss = m_name;
	getTransSign(ss);
	return ss;
}

CAttribute *CNode::getAttributes()
{
	return m_attribute;
}

CNode *CNode::getNextSibling()
{
	if (this != NULL)
	{
		return this->m_nextNode;
	}
	return NULL;
}

CNode *CNode::getFirstChild()
{
	if (this != NULL)
	{
		return this->m_firstChildNode;
	}

	return NULL;
}

CNode *CNode::getParentNode()
{
	if (this != NULL)
	{
		return this->m_parentNode;
	}

	return NULL;
}



CNode * CNode::getLastSibling()
{
	if (this != NULL)
	{
		return this->m_lastNode;
	}
	return NULL;
}

CNode * CNode::getEndChild()
{
	if (this != NULL)
	{
		return this->m_endChildNode;
	}
	
	return NULL;
}

CAttribute *CNode::SetAttribute(std::string name, std::string value,bool isTransSign)
{
	if (m_attribute == NULL)
	{
		m_attribute = new CAttribute();
		m_attribute->m_name = name;
		m_attribute->m_value = value;
		if (isTransSign)
		{
			putTransSign(m_attribute->m_name);
			putTransSign(m_attribute->m_value);
		}
		m_attribute->m_next = NULL;
		return m_attribute;
	}
	else
	{
		string ss = name;
		if (isTransSign)
		{
			putTransSign(ss);
		}
		CAttribute *nextattr = NULL;
		CAttribute *pp = m_attribute;
		while (pp!=NULL)
		{
			if (pp->m_name.compare(ss) == 0)
			{
				pp->m_value = value;
				putTransSign(pp->m_value);
				return pp;
			}
			nextattr = pp;
			pp = pp->m_next;
		}
		
		pp = new CAttribute();
		pp->m_name = name;
		pp->m_value = value;
		if (isTransSign)
		{
			putTransSign(m_attribute->m_name);
			putTransSign(m_attribute->m_value);
		}
		pp->m_next = NULL;
		nextattr->m_next = pp;
		return pp;
	}
}
CAttribute *CNode::SetAttribute(std::string name, int value)
{
	char tmpchar[100];
	sprintf(tmpchar,"%d",value);
	return SetAttribute(name,tmpchar,false);
}
CAttribute *CNode::SetAttribute(std::string name, long value)
{
	char tmpchar[100];
	sprintf(tmpchar,"%ld",value);
	return SetAttribute(name,tmpchar,false);
}
CAttribute *CNode::SetAttribute(std::string name, double value)
{
	char tmpchar[100];
	sprintf(tmpchar,"%lf",value);
	return SetAttribute(name,tmpchar,false);
}
CAttribute *CNode::SetNewAttribute(std::string name, std::string value,bool isTransSign)
{
	if (m_attribute == NULL)
	{
		m_attribute = new CAttribute();
		m_attribute->m_name = name;
		m_attribute->m_value = value;
		if (isTransSign)
		{
			putTransSign(m_attribute->m_name);
			putTransSign(m_attribute->m_value);
		}
		m_attribute->m_next = NULL;
		return m_attribute;
	}
	else
	{
		CAttribute *nextattr = NULL;
		CAttribute *pp = m_attribute;
		while (pp!=NULL)
		{
			nextattr = pp;
			pp = pp->m_next;
		}
		
		pp = new CAttribute();
		pp->m_name = name;
		pp->m_value = value;
		if (isTransSign)
		{
			putTransSign(pp->m_name);
			putTransSign(pp->m_value = value);
		}
		pp->m_next = NULL;
		nextattr->m_next = pp;
		return pp;
	}
}
CAttribute *CNode::SetNewAttribute(std::string name, int value)
{
	char tmpchar[100];
	sprintf(tmpchar,"%d",value);
	return SetNewAttribute(name,tmpchar,false);
}
CAttribute *CNode::SetNewAttribute(std::string name, long value)
{
	char tmpchar[100];
	sprintf(tmpchar,"%ld",value);
	return SetNewAttribute(name,tmpchar,false);
}
CAttribute *CNode::SetNewAttribute(std::string name, double value)
{
	char tmpchar[100];
	sprintf(tmpchar,"%lf",value);
	return SetNewAttribute(name,tmpchar,false);
}

int CNode::GetAttributeValue(std::string name, std::string &value)
{
	if (m_attribute == NULL)
	{
		return -1;
	}
	CAttribute *attr = m_attribute;
	while (attr != NULL)
	{
		if (attr->m_name.compare(name) == 0)
		{
			value = attr->m_value;
			return 0;
		}
		attr = attr->m_next;
	}
	return -1;
}
int CNode::GetAttributeValue(std::string name, int &value)
{
	string svalue;
	if (GetAttributeValue(name,svalue)<0)
	{
		return -1;
	}
	if (!IsNumber(svalue.c_str()))
	{
		return -1;
	}
	value = atoi(svalue.c_str());
	return 0;
}
int CNode::GetAttributeValue(std::string name, long &value)
{
	string svalue;
	if (GetAttributeValue(name,svalue)<0)
	{
		return -1;
	}
	if (!IsNumber(svalue.c_str()))
	{
		return -1;
	}
	value = atoi(svalue.c_str());
	return 0;
}
int CNode::GetAttributeValue(std::string name, double &value)
{
	string svalue;
	if (GetAttributeValue(name,svalue)<0)
	{
		return -1;
	}
	if (!IsDouble(svalue.c_str()))
	{
		return -1;
	}
	value = atof(svalue.c_str());
	return 0;
}

bool CNode::IsNumber(const char *str)
{
	int len;
	len = strlen(str);
	if (len<1) return true;
	for (int i=0;i<len;i++)
	{
		if (str[i]=='-' && i==0)
		{
			continue;
		}
		
		if (str[i]<'0' || str[i]>'9')
		{
			return false;
		}
		
	}
	return true;	
}

bool CNode::IsDouble(const char *str)
{
	int len;
	int sop=0;
	int ret=0;
	len = strlen(str);
	if (len<1) return true;
	for (int i=0;i<len;i++)
	{
		if (str[i]=='-' && i==0)
		{
			continue;
		}
		if (str[i]=='.')
		{
			ret++;
			if (ret>1)
				return false;
		}
		if ((str[i]<'0' || str[i]>'9') && str[i]!='.')
		{
			return false;
		}
		if (str[i]=='.')
		{
			if (i==0 || i==len-1)
			{
				return false;
			}
			sop++;
		}
	}
	if (sop>1)
	{
		return false;
	}
	return true;	
}
//转义符
//&quot; "
//&lt; <
//&gt; >
//&amp; &
//&apos; '
int CNode::getTransSign(std::string &value)
{
	int nLength;
	char *tmpchar = (char *)malloc(value.length()+1);
	memset(tmpchar,0,value.length()+1);
	nLength = value.length();
	char *p = tmpchar;
	bool isbegin=false;
	int  isbeginoffset=0;
	bool isquot;
	bool islt;
	bool isgt;
	bool isamp;
	bool isapos;
	for (int i=0;i<nLength;)
	{
	
		if (!isbegin) //前面不是&开始的
		{
			if (value.c_str()[i] == '&' )
			{
				isbegin = true;
				isbeginoffset=1;
				isquot= true;
				islt= true;
				isgt= true;
				isamp= true;
				isapos= true;
			}
		}
		else if (isbeginoffset == 1)   //l g a q
		{
			isbeginoffset++;
			if (value.c_str()[i] == 'l' )
			{
				isquot= false;
				islt= true;
				isgt= false;
				isamp= false;
				isapos= false;
			}
			else if (value.c_str()[i] == 'g' )
			{
				isquot= false;
				islt= false;
				isgt= true;
				isamp= false;
				isapos= false;
			}
			else if (value.c_str()[i] == 'a' )
			{
				isquot= false;
				islt= false;
				isgt= false;
				isamp= true;
				isapos= true;
			}
			else if (value.c_str()[i] == 'q' )
			{
				isquot= true;
				islt= false;
				isgt= false;
				isamp= false;
				isapos= false;
			}
			else
			{
				isbegin = false;
				isbeginoffset=0;
			}

		}
		else if (isbeginoffset == 2)   //t m u p
		{
			if (islt || isgt)
			{
				if (value.c_str()[i] == 't' )
				{
					isbeginoffset++;
				}
				else
				{
					isbegin = false;
					isbeginoffset=0;
				}
			}
			else if (isquot)
			{
				if (value.c_str()[i] == 'u' )
				{
					isbeginoffset++;
				}
				else
				{
					isbegin = false;
					isbeginoffset=0;
				}
			}
			else if (isamp || isapos)
			{
				if (value.c_str()[i] == 'm' )
				{
					isapos = false;
					isbeginoffset++;
				}
				else if (value.c_str()[i] == 'p' )
				{
					isamp = false;
					isbeginoffset++;
				}
				else
				{
					isbegin = false;
					isbeginoffset=0;
				}
			}
			
		}
		else if (isbeginoffset == 3)   //; p o
		{
			if (islt)
			{
				if (value.c_str()[i] == ';' )
				{
					//替换 <
					p=p-isbeginoffset;
					*p= '<' ;
					p++;
					i++;
					isbegin = false;
					isbeginoffset=0;
				}
				else
				{
					isbegin = false;
					isbeginoffset=0;
				}
			}
			else if (isgt)
			{
				if (value.c_str()[i] == ';' )
				{
					//替换 >
					p=p-isbeginoffset;
					*p= '>' ;
					p++;
					i++;
					isbegin = false;
					isbeginoffset=0;
				}
				else
				{
					isbegin = false;
					isbeginoffset=0;
				}
			}
			else if (isquot)
			{
				if (value.c_str()[i] == 'o' )
				{
					isbeginoffset++;
					isapos = false;
				}
				else
				{
					isbegin = false;
					isbeginoffset=0;
				}
			}
			else if (isapos)
			{
				if (value.c_str()[i] == 'o' )
				{
					isquot = false;
					isbeginoffset++;
				}
				else
				{
					isbegin = false;
					isbeginoffset=0;
				}
			}
			else if (isamp)
			{
				if (value.c_str()[i] == 'p' )
				{
					isbeginoffset++;
				}
				else
				{
					isbegin = false;
					isbeginoffset=0;
				}
			}
			else
			{
				isbegin = false;
				isbeginoffset=0;
			}
		}
		else if (isbeginoffset == 4)  //; t s
		{
			if (isamp)
			{
				if (value.c_str()[i] == ';' )
				{
					//替换 &
					p=p-isbeginoffset;
					*p= '&' ;
					p++;
					i++;
					isbegin = false;
					isbeginoffset=0;
				}
				else
				{
					isbegin = false;
					isbeginoffset=0;
				}
			}
			else if (isapos)
			{
				if (value.c_str()[i] == 's' )
				{
					isbeginoffset++;
				}
				else
				{
					isbegin = false;
					isbeginoffset=0;
				}
			}
			else if (isquot)
			{
				if (value.c_str()[i] == 't' )
				{
					isbeginoffset++;
				}
				else
				{
					isbegin = false;
					isbeginoffset=0;
				}
			}
		}
		else if (isbeginoffset == 5)  //; 
		{
			if (isapos)
			{
				if (value.c_str()[i] == ';' )
				{
					//替换 '
					p=p-isbeginoffset;
					*p= 39 ;
					p++;
					i++;
					isbegin = false;
					isbeginoffset=0;
				}
				else
				{
					isbegin = false;
					isbeginoffset=0;
				}
			}
			else if (isquot)
			{
				if (value.c_str()[i] == ';' )
				{
					//替换 "
					p=p-isbeginoffset;
					*p= '"' ;
					p++;
					i++;
					isbegin = false;
					isbeginoffset=0;
				}
				else
				{
					isbegin = false;
					isbeginoffset=0;
				}
			}
			else
			{
				isbegin = false;
				isbeginoffset=0;
			}
		}
		else
		{
			isbegin = false;
			isbeginoffset=0;
		}
		*p = value.c_str()[i];
		p++;
		i++;
// 		if (nLength >= i+4)
// 		{
// 			if (strncmp((char *)value.c_str()+i,"&lt;",4)==0)
// 			{
// 				*p= '<' ;
// 				p++;
// 				i+=4;
// 				continue;
// 			}
// 			if (strncmp((char *)value.c_str()+i,"&gt;",4)==0)
// 			{
// 				*p= '>' ;
// 				p++;
// 				i+=4;
// 				continue;
// 			}
// 			if (nLength >= i+5)
// 			{
// 				if (strncmp((char *)value.c_str()+i,"&amp;",5)==0)
// 				{
// 					*p= '&' ;
// 					p++;
// 					i+=5;
// 					continue;
// 				}
// 			}
// 			if (nLength >= i+6)
// 			{
// 				if (strncmp((char *)value.c_str()+i,"&quot;",6)==0)
// 				{
// 					*p= '"' ;
// 					p++;
// 					i+=6;
// 					continue;
// 				}
// 				if (strncmp((char *)value.c_str()+i,"&apos;",6)==0)
// 				{
// 					*p= 39 ;
// 					p++;
// 					i+=6;
// 					continue;
// 				}
// 			}
// 
// 			*p = value.c_str()[i];
// 			p++;
// 			i++;
// 		}
// 		else
// 		{
// 			*p = value.c_str()[i];
// 			p++;
// 			i++;
// 		}
	}
	value = tmpchar;
	p=NULL;
	free(tmpchar);
	return 0;
}
//转义符
//&quot; "
//&lt; <
//&gt; >
//&amp; &
//&apos; '
int CNode::putTransSign(std::string &value)
{
// 	char tmpchar[2];
// 	memset(tmpchar,0,sizeof(tmpchar));
// 	string ss = value;
// 	value = "";
// 	for (int i=0;i<ss.length();i++)
// 	{
// 		if (ss.c_str()[i] == '"' )
// 		{
// 			value +="&quot;";
// 		}
// 		else if (ss.c_str()[i] == '<' )
// 		{
// 			value +="&lt;";
// 		}
// 		else if (ss.c_str()[i] == '>' )
// 		{
// 			value +="&gt;";
// 		}
// 		else if (ss.c_str()[i] == '&' )
// 		{
// 			value +="&amp;";
// 		}
// 		else if (ss.c_str()[i] == 39 )
// 		{
// 			value +="&apos;";
// 		}
// 		else
// 		{
// 			tmpchar[0]=ss.c_str()[i];
// 			value += tmpchar;
// 		}
// 	}
	
	char *tmpchar = (char *)malloc(value.length()*3);
	memset(tmpchar,0,value.length()*3);
	
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
		else if (value.c_str()[i] == '&' )
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
	p = NULL;
	free(tmpchar);
	return 0;
	
}

void CNode::setNodeName(std::string name,bool isTransSign)
{
	m_name = name;
	if (isTransSign)
	{
		putTransSign(m_name);
	}
	return ;
}

void CNode::setNodeValue(std::string value,bool isTransSign)
{
	m_value = value;
	if (isTransSign)
	{
		putTransSign(m_value);
	}
	return ;
}

void CNode::setNodeName(int name)
{
	char tmpchar[100];
	sprintf(tmpchar,"%d",name);
	m_name = tmpchar;
	return ;
}

void CNode::setNodeName(long name)
{
	char tmpchar[100];
	sprintf(tmpchar,"%ld",name);
	m_name = tmpchar;
	return ;
}

void CNode::setNodeName(double name)
{
	char tmpchar[100];
	sprintf(tmpchar,"%lf",name);
	m_name = tmpchar;
	return ;
}

void CNode::setNodeValue(int value)
{
	char tmpchar[100];
	sprintf(tmpchar,"%d",value);
	m_value = tmpchar;
	return ;
}

void CNode::setNodeValue(long value)
{	
	char tmpchar[100];
	sprintf(tmpchar,"%ld",value);
	m_value = tmpchar;
	return ;
}

void CNode::setNodeValue(double value)
{
	char tmpchar[100];
	sprintf(tmpchar,"%lf",value);
	m_value = tmpchar;
	return ;
}
CAttribute *CNode::getAttributes(std::string attrname)
{
	CAttribute *tmpNode = m_attribute;
	while (tmpNode)
	{
		if (!strcmp(tmpNode->getName().c_str(),attrname.c_str()))
		{
			return tmpNode;
		}
		tmpNode = tmpNode->m_next;
	}
	return NULL;
}