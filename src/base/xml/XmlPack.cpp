// XmlPack.cpp: implementation of the CXmlPack class.
//
//////////////////////////////////////////////////////////////////////
#include "XmlPack.h"
#include "XmlPackArray.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CXmlPack::CXmlPack()
{
	m_rootnode = NULL;
	m_headnode = NULL;
	m_packnode = NULL;
	m_pRecNode = NULL;
	memset(m_errMsg,0,sizeof(m_errMsg));
	m_rowbuflen = 0;
	m_xml.setCurrentNode(m_xml.m_firstNode);
	CNode *tmpnode = m_xml.addItemN("package","");
	if (tmpnode==NULL)
	{
		sprintf(m_errMsg,"新建<package>节点失败");
		return ;
	}
	m_xml.setCurrentNode(tmpnode);
	tmpnode = m_xml.addItemN("head","");
	if (tmpnode==NULL)
	{
		sprintf(m_errMsg,"新建<head>节点失败");
		return ;
	}
	m_headnode = tmpnode;
	tmpnode = m_xml.addItemN("pack","");
	if (tmpnode==NULL)
	{
		sprintf(m_errMsg,"新建<pack>节点失败");
		return ;
	}
	m_packnode = tmpnode;
	
	m_rowbuf = NULL;
}
CXmlPack::CXmlPack(char *xmlBuf)
{
	if (m_xml.fromBuffer(xmlBuf))
	{
		m_rootnode = m_xml.getRoot();
	}
	else
	{
		m_rootnode = NULL;
	}
	
	m_rowbuf = NULL;
	memset(m_errMsg,0,sizeof(m_errMsg));
	m_rowbuflen = 0;
	readPack();
	if (isBin())
	{
		getHeadValue("BINLENGTH",m_rowbuflen);
	}
	if (m_rowbuflen>0)
	{
		int ret= strlen(xmlBuf);
		m_rowbuf = (char *)malloc(m_rowbuflen+1);
		if (m_rowbuf == NULL)
		{
			return ;
		}
		memset(m_rowbuf,0,m_rowbuflen+1);
		memcpy(m_rowbuf,xmlBuf+ret+1,m_rowbuflen);
	}
}

CXmlPack::~CXmlPack()
{
	m_rootnode = NULL;
	m_headnode = NULL;
	m_packnode = NULL;
	m_pRecNode = NULL;
	if (m_rowbuf != NULL)
	{
		free(m_rowbuf);
		m_rowbuf = NULL;
	}
	m_rowbuflen = 0;
}

bool CXmlPack::fromBuf(char *xmlBuf)
{
	m_rootnode = NULL;
	m_headnode = NULL;
	m_packnode = NULL;
	m_pRecNode = NULL;
	if (m_xml.fromBuffer(xmlBuf))
	{
		m_rootnode = m_xml.getRoot();
	}
	else
	{
		m_rootnode = NULL;
		return false;
	}
	if (m_rowbuf != NULL)
	{
		free(m_rowbuf);
		m_rowbuf = NULL;
	}
	m_rowbuflen = 0;
	if (!readPack())
	{
		return false;
	}
	if (isBin())
	{
		getHeadValue("BINLENGTH",m_rowbuflen);
	}
	if (m_rowbuflen>0)
	{
		int ret= strlen(xmlBuf);
		m_rowbuf = (char *)malloc(m_rowbuflen+1);
		if (m_rowbuf == NULL)
		{
			return false;
		}
		memset(m_rowbuf,0,m_rowbuflen+1);
		memcpy(m_rowbuf,xmlBuf+ret+1,m_rowbuflen);
	}
	return true;
}

bool CXmlPack::readXmlPack(char *xmlBuf)
{
	m_headnode = NULL;
	m_packnode = NULL;
	m_pRecNode = NULL;
	m_rootnode = m_xml.getRoot();
	if (m_rootnode==NULL)
	{
		return false;
	}
	
	m_rowbuflen = 0;
	if (!readPack())
	{
		return false;
	}
	if (isBin())
	{
		if (m_rowbuf != NULL)
		{
			free(m_rowbuf);
			m_rowbuf = NULL;
		}
		getHeadValue("BINLENGTH",m_rowbuflen);
		if (m_rowbuflen >0)
		{
			int ret = strlen(xmlBuf);
			m_rowbuf = (char *)malloc(m_rowbuflen+1);
			if (m_rowbuf == NULL)
			{
				return false;
			}
			memset(m_rowbuf,0,m_rowbuflen+1);
			memcpy(m_rowbuf,xmlBuf+ret+1,m_rowbuflen);
		}
	}
	return true;
}

bool CXmlPack::toXmlBuf(char *xmlBuf)
{
	m_xml.toBuffer(xmlBuf);
	if (strlen(xmlBuf)<1)
	{
		return false;
	}
	return true;
}
bool CXmlPack::toString(std::string &buffer)
{
	return m_xml.toString(buffer);
}
bool CXmlPack::readPack()
{
	//回到根节点
	m_rootnode = m_xml.getRoot();
	m_headnode = NULL;
	m_packnode = NULL;
	m_pRecNode = NULL;
	//获得第一个子节点，当前节点为子节点
	CNode *tmpnode = m_xml.getFirstChildItem();
	if (tmpnode==NULL)
	{
		sprintf(m_errMsg,"xml数据包没有数据项");
		return false;
	}
	//第一个子节点是否为package,不是则表示xml包不符

	if (strcmp(tmpnode->getName().c_str(),"package")==0) 
	{
		//确定是合法xml包信息，取包头及包体
		//取得当前节点的子节点里名为"head"的节点,当前节点为head
		m_headnode = m_xml.getFirstChildItem("head");
		if (m_headnode==NULL)
		{
			sprintf(m_errMsg,"xml数据包没有head数据项");
			return false;
		}
		m_xml.getParentItem();
		m_packnode = m_xml.getFirstChildItem("pack");
		if (m_packnode==NULL)
		{
			sprintf(m_errMsg,"xml数据包没有pack数据项");
			return false;
		}
		return true;
	}
	sprintf(m_errMsg,"第一个子节点不是package,%s",tmpnode->getName().c_str());
	return false;
}

int CXmlPack::setHeadNameValue(std::string name, std::string value,bool isupdate,bool istrans)
{
	CNode *tmpnode;
	if (m_headnode==NULL)
	{
		sprintf(m_errMsg,"包头节点为空");
		return -1;
	}
	tmpnode = m_xml.setCurrentNode(m_headnode);
	if (tmpnode !=m_headnode)
	{
		sprintf(m_errMsg,"设置包头节点出错");
		return -1;
	}
	if (isupdate)
	{
		tmpnode = m_xml.getFirstChildItem(name);
		if (tmpnode==NULL) //增加
		{
			tmpnode = m_xml.setCurrentNode(m_headnode);
			if (tmpnode !=m_headnode)
			{
				sprintf(m_errMsg,"设置包头节点出错");
				return -1;
			}
			if (istrans)
			{
				tmpnode = m_xml.addItem(name,value);
			}
			else
			{
				tmpnode = m_xml.addItemN(name,value);
			}
			if (tmpnode==NULL)
			{
				sprintf(m_errMsg,"设置包头节点(增加)出错");
				return -1;
			}
			m_xml.setCurrentNode(tmpnode);
		}
		else //修改
		{
			if (istrans)
			{
				if (!m_xml.editItem(value))
				{
					sprintf(m_errMsg,"设置包头节点(修改)出错");
					return -1;
				}
			}
			else
			{
				if (!m_xml.editItemN(value))
				{
					sprintf(m_errMsg,"设置包头节点(修改)出错");
					return -1;
				}
			}
			
		}
	}
	else
	{
		if (istrans)
		{
			tmpnode = m_xml.addItem(name,value);
		}
		else
		{
			tmpnode = m_xml.addItemN(name,value);
		}
		if (tmpnode==NULL)
		{
			sprintf(m_errMsg,"设置包头节点(增加)出错");
			return -1;
		}
		m_xml.setCurrentNode(tmpnode);
	}
	
	return 0;
}
int CXmlPack::setHeadNameValue(std::string name, int value,bool isupdate)
{
	char tmpchar[30];
	sprintf(tmpchar,"%d",value);
	return setHeadNameValue(name, tmpchar,isupdate,false);
}
int CXmlPack::setHeadNameValue(std::string name, long value,bool isupdate)
{
	char tmpchar[30];
	sprintf(tmpchar,"%ld",value);
	return setHeadNameValue(name, tmpchar,isupdate,false);
}
int CXmlPack::setHeadNameValue(std::string name, double value,bool isupdate)
{
	char tmpchar[30];
	sprintf(tmpchar,"%lf",value);
	return setHeadNameValue(name, tmpchar,isupdate,false);
}
int CXmlPack::setHeadNameValue(std::string name, double value,int dec,bool isupdate)
{
	char tmpchar[30];
	char fmtchar[30];
	sprintf(fmtchar,"%s%d%s","%.0",dec,"f");
	sprintf(tmpchar,fmtchar,value);
	return setHeadNameValue(name, tmpchar,isupdate,false);
}
int CXmlPack::setPackNameValue(std::string name, int value,bool isupdate)
{
	char tmpchar[30];
	sprintf(tmpchar,"%d",value);
	return setPackNameValue(name, tmpchar,isupdate,false);
}
int CXmlPack::setPackNameValue(std::string name, long value,bool isupdate)
{
	char tmpchar[30];
	sprintf(tmpchar,"%ld",value);
	return setPackNameValue(name, tmpchar,isupdate,false);
}
int CXmlPack::setPackNameValue(std::string name, double value,bool isupdate)
{
	char tmpchar[30];
	sprintf(tmpchar,"%lf",value);
	return setPackNameValue(name, tmpchar,isupdate,false);
}
int CXmlPack::setPackNameValue(std::string name, double value,int dec,bool isupdate)
{
	char tmpchar[30];
	char fmtchar[30];
	sprintf(fmtchar,"%s%d%s","%.0",dec,"f");
	sprintf(tmpchar,fmtchar,value);
	return setPackNameValue(name, tmpchar,isupdate,false);
}

int CXmlPack::setPackNameValue(std::string name, std::string value,bool isupdate,bool istrans)
{
	CNode *tmpnode;
	if (m_packnode==NULL)
	{
		sprintf(m_errMsg,"包体节点为空");
		return -1;
	}
	tmpnode = m_xml.setCurrentNode(m_packnode);
	if (tmpnode !=m_packnode)
	{
		sprintf(m_errMsg,"设置包体节点出错");
		return -1;
	}
	if (isupdate) //查找节点进行更新或增加
	{
		tmpnode = m_xml.getFirstChildItem(name);
		if (tmpnode==NULL) //增加
		{
			tmpnode = m_xml.setCurrentNode(m_packnode);
			if (tmpnode !=m_packnode)
			{
				sprintf(m_errMsg,"设置包体节点出错");
				return -1;
			}
			if (istrans)
			{
				tmpnode = m_xml.addItem(name,value);
			}
			else
			{
				tmpnode = m_xml.addItemN(name,value);
			}
			if (tmpnode==NULL)
			{
				sprintf(m_errMsg,"设置包体节点(增加)出错");
				return -1;
			}
			m_xml.setCurrentNode(tmpnode);
		}
		else //修改
		{
			if (istrans)
			{
				if (!m_xml.editItem(value))
				{
					sprintf(m_errMsg,"设置包体节点(修改)出错");
					return -1;
				}
			}
			else
			{	
				if (!m_xml.editItemN(value))
				{
					sprintf(m_errMsg,"设置包体节点(修改)出错");
					return -1;
				}
			}
			
		}
	}
	else //增加节点
	{
		if (istrans)
		{
			tmpnode = m_xml.addItem(name,value);
		}
		else
		{
			tmpnode = m_xml.addItemN(name,value);
		}
		if (tmpnode==NULL)
		{
			sprintf(m_errMsg,"设置包体节点(增加)出错");
			return -1;
		}
		m_xml.setCurrentNode(tmpnode);
	}
	return 0;
}

int CXmlPack::setPackNameValue(const std::string item, std::string name, std::string value,bool isupdate,bool istrans)
{
	CNode *tmpnode;
	if (m_packnode==NULL)
	{
		sprintf(m_errMsg,"包体节点为空");
		return -1;
	}
	tmpnode = m_xml.setCurrentNode(m_packnode);
	if (tmpnode !=m_packnode)
	{
		sprintf(m_errMsg,"设置包体节点出错");
		return -1;
	}
	tmpnode = m_xml.getParentItem();
	if (tmpnode == NULL)
	{
		sprintf(m_errMsg,"pack父节点为空");
		return -1;
	}
	CNode *tmpnode2 = m_xml.getFirstChildItem(item);
	if (tmpnode2 == NULL)
	{
		sprintf(m_errMsg,"节点<%s>不存在",item.c_str());
		return -1;
	}
	if (isupdate)
	{
		tmpnode = m_xml.getFirstChildItem(name);
		if (tmpnode==NULL) //增加
		{
			tmpnode = m_xml.setCurrentNode(tmpnode2);
			if (tmpnode !=tmpnode2)
			{
				sprintf(m_errMsg,"设置包体节点出错");
				return -1;
			}
			if (istrans)
			{
				tmpnode = m_xml.addItem(name,value);
			}
			else
			{
				tmpnode = m_xml.addItemN(name,value);
			}
			if (tmpnode==NULL)
			{
				sprintf(m_errMsg,"设置包体节点(增加)出错");
				return -1;
			}
			m_xml.setCurrentNode(tmpnode);
		}
		else //修改
		{
			if (istrans)
			{
				if (!m_xml.editItem(value))
				{
					sprintf(m_errMsg,"设置包体节点(修改)出错");
					return -1;
			}
			}
			else
			{
				if (!m_xml.editItemN(value))
				{
					sprintf(m_errMsg,"设置包体节点(修改)出错");
					return -1;
				}
			}
			
		}
	}
	else
	{
		if (istrans)
		{
			tmpnode = m_xml.addItem(name,value);
		}
		else
		{
			tmpnode = m_xml.addItemN(name,value);
		}
		if (tmpnode==NULL)
		{
			sprintf(m_errMsg,"设置包体节点(增加)出错");
			return -1;
		}
		m_xml.setCurrentNode(tmpnode);
	}
	return 0;
}
int CXmlPack::setPackNameValueI(int packno, std::string name, int value,bool isupdate)
{
	char tmpchar[30];
	sprintf(tmpchar,"%d",value);
	return setPackNameValueI(packno,name,tmpchar,isupdate,false);
}
int CXmlPack::setPackNameValueI(int packno, std::string name, long value,bool isupdate)
{
	char tmpchar[30];
	sprintf(tmpchar,"%ld",value);
	return setPackNameValueI(packno,name,tmpchar,isupdate,false);
}
int CXmlPack::setPackNameValueI(int packno, std::string name, double value,bool isupdate)
{
	char tmpchar[30];
	sprintf(tmpchar,"%lf",value);
	return setPackNameValueI(packno,name,tmpchar,isupdate,false);
}
int CXmlPack::setPackNameValueI(int packno, std::string name, std::string value,bool isupdate,bool istrans)
{
	
	char item[20];
	if (packno<1)
	{
		sprintf(m_errMsg,"记录数不能小于1");
		return -1;
	}
	if (m_packnode==NULL)
	{
		sprintf(m_errMsg,"包体节点为空");
		return -1;
	}
	if (packno==1)
	{
		sprintf(item,"pack");
	}
	else
	{
		sprintf(item,"pack%d",packno-1);
	}
	return setPackNameValue(item,name,value,isupdate,istrans);
}
char *CXmlPack::getMsg()
{
	return m_errMsg;
}

int CXmlPack::fromFile(char *filename)
{
	m_rootnode = NULL;
	m_headnode = NULL;
	m_packnode = NULL;
	m_pRecNode = NULL;
	int ret = m_xml.fromFile(filename);
	if (0==ret )
	{
		if (!readPack())
		{
			return -1;
		}
		return 0;
	}
	
	return ret;
}

int CXmlPack::toFile(char *filename)
{
	FILE *fp;
	int ret;
	int datalen = getXmlDataLen();
	if (datalen>0)
	{
		fp = fopen(filename,"wb");
		if (fp==NULL)
		{
			sprintf(m_errMsg,"打开文件[%s]失败",filename);
			return -1;
		}
		fprintf(fp,"<?xml version=%c1.0%c encoding=%cGB2312%c?>\n",'"','"','"','"');
		char *buffer = (char *)malloc(datalen+1);
		if (buffer == NULL)
		{
			sprintf(m_errMsg,"分配内存空间失败");
			return -1;
		}
		if (!toBuf(buffer,ret))
		{
			free(buffer) ;
			fclose(fp);
			return -1;
		}
		fwrite(buffer,1,ret,fp);
		free(buffer) ;
		fflush(fp);
		fclose(fp);
		return ret;
	}
	sprintf(m_errMsg,"XML无数据");
	return -1;
}


int CXmlPack::trim(std::string &s)
{	
	int pos;
	while ((pos=s.find(' ', 0)) != -1)	
	{
		s.erase(pos, 1);	
	}
	return 0;
}
int CXmlPack::trim(char *s)
{
	int i;
	char tm[8192],*str;

	str=s;
	for(i=0;*str!=0;str++)
	{
		if(*str!=' '&&*str!=10&&*str!=13)
		{
			tm[i++]=*str;
		}
	}
	tm[i]=0;
	strcpy(s,tm);
	return(0);
}
char *CXmlPack::rtrim(char *str)
{
	int i,j;
	for(i=0,j=-1;*(str+i)!=0;i++)
	{
		if(*(str+i)!=' ')j=i;
	}
	*(str+j+1)=0;
	return(str);
}

char *CXmlPack::ltrim(char *str)
{
	char *p,*q;
	q=p=str;
	for(;*p!=0;p++)
	{
		if(*p!=' ')
		{
			break;
		}
	}
	if(p!=q)
	{
		for(;*p!=0;p++,q++)
		{
			*q=*p;
		}
		*q=0;
	}
	return(str);
}	

int CXmlPack::newPackage()
{
	if (m_xml.m_firstNode!=NULL)
	{
		if (m_xml.m_firstNode->m_firstChildNode!=NULL)
		{
			if (!m_xml.deleteNode(m_xml.m_firstNode->m_firstChildNode))
			{
				sprintf(m_errMsg,"删除所有节点失败");
				return -1;
			}
		}
	}
	m_pRecNode = NULL;
	m_headnode = NULL;
	m_packnode = NULL;
	m_xml.setCurrentNode(m_xml.m_firstNode);
	CNode *tmpnode = m_xml.addItemN("package","");
	if (tmpnode==NULL)
	{
		sprintf(m_errMsg,"新建<package>节点失败");
		return -1;
	}
	m_xml.setCurrentNode(tmpnode);
	tmpnode = m_xml.addItemN("head","");
	if (tmpnode==NULL)
	{
		sprintf(m_errMsg,"新建<head>节点失败");
		return -1;
	}
	m_headnode = tmpnode;
	m_xml.setCurrentNode(tmpnode);
	if (m_xml.addItemN("交易码","0") == NULL)
	{
		sprintf(m_errMsg,"新建<head>节点的<交易码>节点失败");
		return -1;
	}
	if (m_xml.addItemN("返回码","0") == NULL)
	{
		sprintf(m_errMsg,"新建<head>节点的<返回码>节点失败");
		return -1;
	}
	if (m_xml.addItemN("NEXTFLAG","0") == NULL)
	{
		sprintf(m_errMsg,"新建<head>节点的<NEXTFLAG>节点失败");
		return -1;
	}
	if (m_xml.addItemN("FIRSTFLAG","1") == NULL)
	{
		sprintf(m_errMsg,"新建<head>节点的<FIRSTFLAG>节点失败");
		return -1;
	}
	if (m_xml.addItemN("总记录数","1") == NULL)
	{
		sprintf(m_errMsg,"新建<head>节点的<总记录数>节点失败");
		return -1;
	}
	m_pRecNode = m_xml.addItemN("本包记录数","1");
	if ( m_pRecNode == NULL)
	{
		sprintf(m_errMsg,"新建<head>节点的<本包记录数>节点失败");
		return -1;
	}
	if (m_xml.addItemN("总包数","1") == NULL)
	{
		sprintf(m_errMsg,"新建<head>节点的<总包数>节点失败");
		return -1;
	}
	if (m_xml.addItemN("BINFLAG","0") == NULL)
	{
		sprintf(m_errMsg,"新建<head>节点的<BINFLAG>节点失败");
		return -1;
	}
	if (m_xml.addItemN("业务步骤","0") == NULL)
	{
		sprintf(m_errMsg,"新建<head>节点的<业务步骤>节点失败");
		return -1;
	}
	if (m_xml.addItemN("业务控制流水","0") == NULL)
	{
		sprintf(m_errMsg,"新建<head>节点的<业务控制流水>节点失败");
		return -1;
	}
	
	m_xml.setCurrentNode(m_headnode);
	m_xml.getParentItem();
	tmpnode = m_xml.addItemN("pack","");
	if (tmpnode==NULL)
	{
		sprintf(m_errMsg,"新建<pack>节点失败");
		return -1;
	}
	m_packnode = tmpnode;
	
	return 0;
}

int CXmlPack::addPack(std::string &packname)
{
	
	char packrec[100];
	int rec;
	CNode *tmpnode;
	//取本包记录数
	if (!getRecCount(rec))
	{
		rec = 1;
	}
	sprintf(packrec,"pack%d",rec);
	if (m_packnode == NULL)
	{
		sprintf(m_errMsg,"无pack节点,增加失败");
		return -1;
	}
	//回到pack节点的父节点，以便增加
	tmpnode = m_xml.setCurrentNode(m_packnode);
	if (tmpnode != m_packnode)
	{
		sprintf(m_errMsg,"设置包体节点失败,增加失败");
		return -1;
	}
	CNode *tmpnode2 = m_xml.getParentItem();
	if (tmpnode2 == NULL)
	{
		sprintf(m_errMsg,"返回包体节点父节点失败,增加失败");
		return -1;
	}
	//取此节点是否存在，存在则置head返回
	tmpnode = m_xml.getFirstItem(packrec);
	if (tmpnode!=NULL) //已有此节点，直接置head返回
	{
		packname = packrec ;
		rec++;
		setRecCount(rec);
		return rec;
	}
	//再次回到父节点
	tmpnode = m_xml.setCurrentNode(tmpnode2);
	if (tmpnode != tmpnode2)
	{
		sprintf(m_errMsg,"设置包体节点失败,增加失败");
		return -1;
	}
	//增加
	tmpnode = m_xml.addItemN(packrec,"");
	if (tmpnode == NULL)
	{
		sprintf(m_errMsg,"增加节点<%s>失败",packrec);
		return -1;
	}
	packname = packrec;
	rec++;
	setRecCount(rec);
	return rec;
}
bool CXmlPack::toBuf(char *Buf,int &buflen)
{
	if(!m_xml.toBuffer(Buf))
	{
		return false;
	}
	buflen = strlen(Buf);
	if (isBin())
	{
		if (m_rowbuflen>0)
		{
			memcpy(Buf+buflen+1,m_rowbuf,m_rowbuflen);
			buflen = buflen+m_rowbuflen+1;
		}
	}
	return true;
}
bool CXmlPack::toBufOld(char *Buf,int &buflen)
{
	if(!m_xml.toBufferOld(Buf))
	{
		return false;
	}
	buflen = strlen(Buf);
	if (isBin())
	{
		if (m_rowbuflen>0)
		{
			memcpy(Buf+buflen+1,m_rowbuf,m_rowbuflen);
			buflen = buflen+m_rowbuflen+1;
		}
	}
	return true;
}
bool CXmlPack::tobuffer(char *buffer,int &bufsize)
{
	int bsize = bufsize;
	if(!m_xml.toBuffer(buffer,bufsize))
	{
		return false;
	}
	if (isBin())
	{
		if (m_rowbuflen>0)
		{
			if ((bufsize + m_rowbuflen +1) > bsize)
			{
				return false;
			}
			memcpy(buffer+bufsize+1,m_rowbuf,m_rowbuflen);
			bufsize = bufsize+m_rowbuflen+1;
		}
	}
	return true;
}
int CXmlPack::getHeadValue(const std::string name, std::string &value,bool istrim)
{
	value = "";
	CNode *tmpnode;
	if (m_headnode==NULL)
	{
		sprintf(m_errMsg,"包头节点为空");
		return -1;
	}
	tmpnode = m_xml.setCurrentNode(m_headnode);
	if (tmpnode !=m_headnode)
	{
		sprintf(m_errMsg,"设置包头节点出错");
		return -1;
	}
	tmpnode = m_xml.getFirstChildItem(name);
	if (tmpnode==NULL)
	{
		sprintf(m_errMsg,"head无<%s>的值",name.c_str());
		return -1;
	}
	
	value = m_xml.getNodeValue();
	if (istrim)
	{
		trim(value);
	}
	return 0;
}

int CXmlPack::getPackValue(const std::string name, std::string &value,bool istrim)
{
	value = "";
	CNode *tmpnode;
	if (m_packnode==NULL)
	{
		sprintf(m_errMsg,"包头节点为空");
		return -1;
	}
	tmpnode = m_xml.setCurrentNode(m_packnode);
	if (tmpnode !=m_packnode)
	{
		sprintf(m_errMsg,"设置包体节点出错");
		return -1;
	}
	tmpnode = m_xml.getFirstChildItem(name);
	if (tmpnode==NULL)
	{
		sprintf(m_errMsg,"pack无<%s>的值",name.c_str());
		return -1;
	}
	if (!istrim)
	{
		value =  m_xml.getNodeValue();
	}
	else
	{
		std::string ss ;
		ss =  m_xml.getNodeValue();
		
		trim(ss);
		value = ss;
	}
	
	return 0;
}
int CXmlPack::getHeadValueChar(const char *name, char *value)
{
	CNode *tmpnode;
	if (m_headnode==NULL)
	{
		sprintf(m_errMsg,"包头节点为空");
		return -1;
	}
	tmpnode = m_xml.setCurrentNode(m_headnode);
	if (tmpnode !=m_headnode)
	{
		sprintf(m_errMsg,"设置包头节点出错");
		return -1;
	}
	tmpnode = m_xml.getFirstChildItem(name);
	if (tmpnode==NULL)
	{
		sprintf(m_errMsg,"head无<%s>的值",name);
		return -1;
	}
	std::string ss;
	ss = m_xml.getNodeValue();
	strcpy(value,ss.c_str());
	ltrim(value);
	rtrim(value);
	return 0;
}

int CXmlPack::getPackValueChar(const char *name, char *value)
{
	CNode *tmpnode;
	if (m_packnode==NULL)
	{
		sprintf(m_errMsg,"包头节点为空");
		return -1;
	}
	tmpnode = m_xml.setCurrentNode(m_packnode);
	if (tmpnode !=m_packnode)
	{
		sprintf(m_errMsg,"设置包体节点出错");
		return -1;
	}
	tmpnode = m_xml.getFirstChildItem(name);
	if (tmpnode==NULL)
	{
		sprintf(m_errMsg,"pack无<%s>的值",name);
		return -1;
	}
	std::string ss;
	ss = m_xml.getNodeValue();
	strcpy(value,ss.c_str());
	ltrim(value);
	rtrim(value);
	return 0;
}
int CXmlPack::getPackValue(const std::string name, int &value)
{
	int ret;
	std::string ss;
	ret = getPackValue(name, ss);
	if (ret<0) return ret;
	value = atoi(ss.c_str());
	return 0;
}

int CXmlPack::getPackValue(const std::string name, double &value)
{
	int ret;
	std::string ss;
	ret = getPackValue(name, ss);
	if (ret<0) return ret;
	value = atof(ss.c_str());
	return 0;

}
int CXmlPack::getPackValueI(const std::string name)
{
	int ret;
	std::string ss;
	ret = getPackValue(name, ss);
	if (ret<0) return -9999;
	return atoi(ss.c_str());
}
double CXmlPack::getPackValueD(const std::string name)
{
	int ret;
	std::string ss;
	ret = getPackValue(name, ss);
	if (ret<0) return -9999;
	return atof(ss.c_str());
}
char * CXmlPack::getPackValue(const std::string name)
{
	int ret;
	static char tmpbuf[4096];
	std::string ss;
	memset(tmpbuf,0,sizeof(tmpbuf));
	ret = getPackValue(name,ss);
	if (ret<0) return tmpbuf;
	memcpy(tmpbuf,ss.c_str(),ss.length());
	return tmpbuf;
}
std::string CXmlPack::getPackValueS(const std::string name,bool istrim)
{
	int ret;
	std::string ss;
	ret = getPackValue(name, ss,istrim);
	if (istrim)
	{
		trim(ss);
	}
	return ss;
}	

	

int CXmlPack::getHeadValue(const std::string name, int &value)
{
	int ret;
	std::string ss;
	ret = getHeadValue(name, ss);
	if (ret<0) return ret;
	value = atoi(ss.c_str());
	return 0;
}

int CXmlPack::getHeadValue(const std::string name, double &value)
{
	int ret;
	std::string ss;
	ret = getHeadValue(name, ss);
	if (ret<0) return ret;
	value = atof(ss.c_str());
	return 0;
}
int CXmlPack::getHeadValueI(const std::string name)
{
	int ret;
	std::string ss;
	ret = getHeadValue(name, ss);
	if (ret<0) return -9999;
	return atoi(ss.c_str());
}
double CXmlPack::getHeadValueD(const std::string name)
{
	int ret;
	std::string ss;
	ret = getHeadValue(name, ss);
	if (ret<0) return -9999;
	return atof(ss.c_str());
}
char * CXmlPack::getHeadValue(const std::string name)
{
	int ret;
	std::string ss;
	ret = getHeadValue(name, ss);
	if (ret<0) return (char*)"";
	trim(ss);
	return (char *)ss.c_str();
}
std::string CXmlPack::getHeadValueS(const std::string name,bool istrim)
{
	int ret;
	std::string ss;
	ret = getHeadValue(name, ss,istrim);
	if (ret<0) return ss;
	if (istrim)
	{
		trim(ss);
	}
	return ss;
}
int CXmlPack::gotoHead()
{
	if (m_headnode==NULL)
	{
		return -1;
	}
	m_xml.setCurrentNode(m_headnode);
	return 0;
}
int CXmlPack::gotoPack()
{
	if (m_packnode==NULL)
	{
		return -1;
	}
	m_xml.setCurrentNode(m_packnode);
	return 0;
}

int CXmlPack::deletePack()
{
	if (gotoPack()<0)
	{
		sprintf(m_errMsg,"无pack节点");
		return -1;
	}
	CNode *tmpnode = m_xml.getParentItem();
	int rec=0;
	if (!getRecCount(rec)) //没有本包记录数
	{
		//只删除pack即可,不用删除pack%d
	}
	else //删除多pack
	{
		CNode *deletenode;
		char packname[20];
		for (int i=1;i<rec;i++)
		{
			sprintf(packname,"pack%d",i);
			m_xml.setCurrentNode(tmpnode);
			deletenode = m_xml.getFirstChildItem(packname);
			if (!m_xml.deleteNode(deletenode))
			{
				sprintf(m_errMsg,"删除%s节点出错",packname);
				return -1;
			}
		}
		
	}
	if (gotoPack()<0)
	{
		sprintf(m_errMsg,"无pack节点");
		return -1;
	}
	if (!m_xml.deleteItem())
	{
		sprintf(m_errMsg,"删除pack节点出错");
		return -1;
	}
	
	m_xml.setCurrentNode(tmpnode);
	tmpnode = m_xml.addItemN("pack","");
	if (tmpnode==NULL)
	{
		return -1;
	}
	m_packnode = tmpnode;
	return 0;
}
bool CXmlPack::isBin()
{
	int ret;
	std::string flag;
	ret = getHeadValue("BINFLAG",flag);
	if (ret<0) return false;
	if (1==atoi(flag.c_str()))
	{
		return true;
	}
	return false;
}
bool CXmlPack::setBin(bool binflag)
{
	int ret;
	if (binflag)
	{
		ret = setHeadNameValue("BINFLAG","1",true,false);
	}
	else
	{
		ret = setHeadNameValue("BINFLAG","0",true,false);
	}
	if (ret<0) return false;
	return true;
}

bool CXmlPack::xmlCopyNV(CXmlPack *xmlpack)
{
	bool isbin = false;
	if (xmlpack->isBin())
	{
		if (m_rowbuf != NULL)
		{
			free(m_rowbuf);
			m_rowbuf = NULL;
		}
		m_rowbuf = (char *)malloc(xmlpack->m_rowbuflen+1);
		if (m_rowbuf == NULL)
		{
			sprintf(m_errMsg,"分配内存空间失败");
			return false;
		}
		memset(m_rowbuf,0,xmlpack->m_rowbuflen);
		memcpy(m_rowbuf,xmlpack->m_rowbuf,xmlpack->m_rowbuflen);
		m_rowbuflen = xmlpack->m_rowbuflen;
		setBin(true);
		isbin = true;
	}
	int ret;
	newPackage();
	ret = xmlpack->gotoHead();
	if (ret<0)
	{
		if (isbin) return true;
		return false;
	}
	CNode *tmpnode = xmlpack->m_xml.getFirstChildItem();
	while (tmpnode !=NULL)
	{
		setHeadNameValue(tmpnode->m_name,tmpnode->m_value,true,false);
		tmpnode = xmlpack->m_xml.getNextSibling();
	}
	ret = xmlpack->gotoPack();
	if (ret<0)
	{
		if (isbin) return true;
		return false;
	}
	tmpnode = xmlpack->m_xml.getFirstChildItem();
	while (tmpnode !=NULL)
	{
		setPackNameValue(tmpnode->m_name,tmpnode->m_value,true,false);
		tmpnode = xmlpack->m_xml.getNextSibling();
	}
	return true;
}

bool CXmlPack::modiPack(CXmlPack *xmlpack,bool isUpdate)
{
	if (xmlpack->gotoPack()<0)
	{
		return false;
	}
	CNode *node = xmlpack->m_xml.getFirstChildItem();
	while (node !=NULL)
	{
		setPackNameValue(node->m_name,node->m_value,isUpdate,false);
		node = node->getNextSibling();
	}
	return true;
}

bool CXmlPack::setBinData(char *data, int datalen)
{
	if (datalen>0)
	{
		if (m_rowbuf != NULL)
		{
			free(m_rowbuf);
			m_rowbuf = NULL;
		}
		m_rowbuf = (char *)malloc(datalen+1);
		if (m_rowbuf == NULL)
		{
			sprintf(m_errMsg,"分配内存空间失败");
			return false;
		}
		memset(m_rowbuf,0,datalen+1);
		memcpy(m_rowbuf,data,datalen);
		m_rowbuflen = datalen;
	}
	else
	{
		m_rowbuflen = 0;
	}
	setBin(true);
	setHeadNameValue("BINLENGTH",m_rowbuflen,true);
	return true;
}

bool CXmlPack::getBinData(char *data, int &datalen)
{
	if (!isBin())
	{
		datalen =0;
		return false;
	}
	if (m_rowbuflen>0)
	{
		memcpy(data,m_rowbuf,m_rowbuflen);
		datalen = m_rowbuflen;
	}
	else
	{
		datalen = 0;
	}
	return true;
}

bool CXmlPack::setRetCode(int retCode)
{
	if (setHeadNameValue("返回码",retCode,true) < 0 )
	{
		return false;
	}
	return true;
}

bool CXmlPack::setRetCode(const char *retCode)
{
	if (setHeadNameValue("返回码",retCode,true,true) < 0 )
	{
		return false;
	}
	return true;
}
bool CXmlPack::setRetMsg(const char *msg)
{
	if (setPackNameValue("返回信息",msg,true,true) < 0 )
	{
		return false;
	}
	return true;
}

bool CXmlPack::setReturn(int retCode, const char *msg)
{
	if (setHeadNameValue("返回码",retCode,true)  < 0 )
	{
		return false;
	}
	if (setPackNameValue("返回信息",msg,true,true) < 0 )
	{
		return false;
	}
	return true;
}

bool CXmlPack::setReturn(const char *retCode, const char *msg)
{
	if (setHeadNameValue("返回码",retCode,true,true)  < 0 )
	{
		return false;
	}
	if (setPackNameValue("返回信息",msg,true,true) < 0 )
	{
		return false;
	}
	return true;
}
bool CXmlPack::getRetCode(int &retCode)
{
	char ret[20];
	memset(ret,0,sizeof(ret));
	if (getHeadValueChar("返回码",ret)<0)
//	if (getHeadValue("返回码",retCode) <0)
	{
		retCode = -1;
		return false;
	}
	int len;
	len = strlen(ret);
	if (len<1) 
	{
		retCode = 100;
		return false;
	}
	for (int i=0;i<len;i++)
	{
		if (ret[i]<'0' || ret[i]>'9')
		{
			retCode = -100;
			return false;
		}
	}
	retCode = atoi(ret);
	return true;
}
bool CXmlPack::getRetCode(char *retCode)
{
	char ret[20];
	memset(ret,0,sizeof(ret));
	if (getHeadValueChar("返回码",retCode)<0)
	{
		return false;
	}
	return true;
}
bool CXmlPack::getRetMsg(char *msg)
{
	if (getPackValueChar("返回信息",msg) <0)
	{
		return false;
	}
	return true;
}

bool CXmlPack::setRecCount(int rec)
{
	if (m_pRecNode != NULL)
	{
		char tmpchar[40];
		sprintf(tmpchar,"%d",rec);
		m_pRecNode->m_value = tmpchar;
		return true;
	}
	if (setHeadNameValue("本包记录数",rec,true)<0)
	{
		return false;
	}
	m_pRecNode = m_xml.getCurrentNode();
	return true;
}

bool CXmlPack::getRecCount(int &rec)
{
	if (m_pRecNode != NULL)
	{
		rec = atoi(m_pRecNode->m_value.c_str());
		return true;
	}
	if (getHeadValue("本包记录数",rec)<0)
	{
		return false;
	}
	m_pRecNode = m_xml.getCurrentNode();
	return true;
}

bool CXmlPack::setTotalRec(int rec)
{
	if (setHeadNameValue("总记录数",rec,true)<0)
	{
		return false;
	}
	return true;
}

bool CXmlPack::getTotalRec(int &rec)
{
	if (getHeadValue("总记录数",rec)<0)
	{
		return false;
	}
	return true;
}

bool CXmlPack::setFirstFlag(bool isfirst)
{
	if (isfirst)
	{
		if (setHeadNameValue("FIRSTFLAG","1",true,false) <0)
		{
			return false;
		}
	}
	else
	{
		if (setHeadNameValue("FIRSTFLAG","0",true,false) <0)
		{
			return false;
		}
	}
	return true;
}

bool CXmlPack::getFirstFlag()
{
	int ret;
	int flag;
	ret = getHeadValue("FIRSTFLAG",flag);
	if (ret<0)
	{
		return true;
	}
	if (flag==0)
	{
		return false;
	}
	return true;
}

bool CXmlPack::setNextFlag(bool isnext)
{
	if (isnext)
	{
		if (setHeadNameValue("NEXTFLAG","1",true,false) <0)
		{
			return false;
		}
	}
	else
	{
		if (setHeadNameValue("NEXTFLAG","0",true,false) <0)
		{
			return false;
		}
	}
	return true;
}

bool CXmlPack::getNextFlag()
{
	int ret;
	int flag;
	ret = getHeadValue("NEXTFLAG",flag);
	if (ret<0)
	{
		return false;
	}
	if (flag==1)
	{
		return true;
	}
	return false;
}

int CXmlPack::addPack(CXmlPack *xmlpack)
{
	int rec=0;
	std::string name;
	CNode *curtmpnode;
	rec  = addPack(name);
	if (rec <1)
	{
		return -1;
	}
	if (gotoPack()!=0)
	{
		sprintf(m_errMsg,"无pack节点");
		return -1;
	}
	m_xml.getParentItem();
	curtmpnode = m_xml.getFirstChildItem(name);
	if (curtmpnode==NULL)
	{
		//sprintf(m_errMsg,"增加新的记录节点失败");
		return -1;
	}

	m_xml.setCurrentNode(curtmpnode);
	if (xmlpack->gotoPack()!=0)
	{
		sprintf(m_errMsg,"源xml无pack节点");
		return -1;
	}
	xmlpack->m_xml.getParentItem();
	curtmpnode = xmlpack->m_xml.getFirstChildItem("pack");	
	curtmpnode = xmlpack->m_xml.getFirstChildItem();
	if (!m_xml.XmlCopyNode(curtmpnode))
	{
		sprintf(m_errMsg,"复制xml节点出错");
		return -1;
	}
	return 0;
}
int CXmlPack::addPackPoint(CXmlPack *xmlpack)
{
	
	char packrec[100];
	int rec=0;
	std::string name;
	CNode *curtmpnode=NULL;
	
	if (xmlpack->gotoPack()!=0)
	{
		sprintf(m_errMsg,"源xmlpack无pack节点");
		return -1;
	}
	curtmpnode = xmlpack->m_xml.getCurrentNode();
	xmlpack->m_xml.deleteNodePoint(curtmpnode);
	CNode *packnode = new CNode();
	packnode->m_name = "pack";
	if (!xmlpack->SetPackNode(packnode))
	{
		delete packnode;
		packnode = NULL;
		return -1;
	}
	packnode = NULL;
	//取本包记录数
	if (!getRecCount(rec))
	{
		rec =1;
	}
	sprintf(packrec,"pack%d",rec);
	curtmpnode->m_name = packrec;
	if (!SetPackNode(curtmpnode))
	{
		delete curtmpnode;
		curtmpnode = NULL;
		return -1;
	}
// 	
// 	gotoPack();
// 	m_xml.getParentItem();
// 	if (!m_xml.AddNode(curtmpnode))
// 	{
// 		m_xml.deleteNode(curtmpnode);
// 		sprintf(m_errMsg,"增加节点出错");
// 		return -1;
// 	}
	setRecCount(rec+1);
	return 0;
}
bool CXmlPack::getPack(int recNo,CXmlPack *xmlpack)
{
	int rec;
	int ret;
	char packname[20];
	getRecCount(rec);
	if (recNo>rec)
	{
		sprintf(m_errMsg,"recNo大于本包记录数");
		return false;
	}
	xmlpack->newPackage();
	getRetCode(ret);
	xmlpack->setRetCode(ret);

	if (gotoPack()!=0)
	{
		sprintf(m_errMsg,"源XML无pack节点");
		return false;
	}
	CNode *tmpnode=NULL;
	if (recNo==1)
	{
		xmlpack->gotoPack();
		tmpnode = m_xml.getFirstChildItem();
		if (tmpnode != NULL)
		{
			if  (!xmlpack->m_xml.XmlCopyNode(tmpnode))
			{
				strcpy(m_errMsg,xmlpack->m_errMsg);
				return false;
			}
		}
		return true;
		
	}
	else
	{
		sprintf(packname,"pack%d",recNo-1);
		m_xml.getParentItem();
		tmpnode = m_xml.getFirstChildItem(packname);
		if (tmpnode ==NULL)
		{
			sprintf(m_errMsg,"无[%s]节点",packname);
			return false;
		}
		xmlpack->gotoPack();
		tmpnode = m_xml.getFirstChildItem();
		if (tmpnode != NULL)
		{
			if  (!xmlpack->m_xml.XmlCopyNode(tmpnode))
			{
				strcpy(m_errMsg,xmlpack->m_errMsg);
				return false;
			}
		}
		return true;
	}
	return true;
}
bool CXmlPack::getPackPoint(int recNo,CXmlPack *xmlpack)
{
	
	
	char packname[20];
	
//	getRetCode(ret);
	if (gotoPack()!=0)
	{
		sprintf(m_errMsg,"源XML无pack节点");
		return false;
	}
	CNode *srcpacknode=NULL;
	CNode *packnode = NULL;
	if (recNo==1)
	{
		srcpacknode = m_xml.getCurrentNode();
		m_xml.getParentItem();
		packnode = new CNode();
		packnode->m_name = "pack";
	}
	else
	{
		sprintf(packname,"pack%d",recNo-1);
		m_xml.getParentItem();
		srcpacknode = m_xml.getFirstChildItem(packname);
		if (srcpacknode ==NULL)
		{
			sprintf(m_errMsg,"无[%s]节点",packname);
			return false;
		}
		packnode = new CNode();
		packnode->m_name = packname;
	}
	if (!m_xml.deleteNodePoint(srcpacknode)) //在xml链表中删除pack节点
	{
		sprintf(m_errMsg,"deleteNodePoint失败");
		delete packnode;
		packnode = NULL;
		return false;
	}
	
	if (!SetPackNode(packnode))//将新节点作为原pack节点
	{
		delete packnode;
		delete srcpacknode;
		srcpacknode = NULL;
		packnode = NULL;
		return false;
	}
	packnode = NULL;
	xmlpack->gotoPack();
	CNode *ttxmlnode= xmlpack->m_xml.getCurrentNode();
	xmlpack->m_xml.getParentItem();
	if (!xmlpack->m_xml.deleteNode(ttxmlnode)) //删除xmlpack的pack节点
	{
		ttxmlnode = NULL;
		sprintf(m_errMsg,"%s",xmlpack->m_errMsg);
		return false;
	}
	ttxmlnode = NULL;
	srcpacknode->m_name = "pack";
	if (!xmlpack->SetPackNode(srcpacknode))//将源节点作为原pack节点
	{
		delete srcpacknode;
		srcpacknode = NULL;
		return false;
	}
	return true;
}
bool CXmlPack::getMuPack(CXmlPackArray *muxmlpack)
{
	int rec;
	if (!getRecCount(rec))
	{
		rec = 1;
	}
	CXmlPack xmlpack;
	for (int i=0;i<rec;i++)
	{
		if (!getPack(i+1,&xmlpack))
		{
			//sprintf(m_errMsg,"取pack%d出错",i+1);
			return false;
		}
		muxmlpack->push_back(&xmlpack);
	}
	return true;
}
bool CXmlPack::getMuPackPoint(CXmlPackArray *muxmlpack)
{
	int rec;
	if (!getRecCount(rec))
	{
		rec = 1;
	}
	CXmlPack *xmlpack=NULL;
	for (int i=0;i<rec;i++)
	{
		xmlpack = muxmlpack->push_back();
		if (!getPackPoint(i+1,xmlpack))
		{
			//sprintf(m_errMsg,"取pack%d出错",i+1);
			return false;
		}
	}
	return true;
}
bool CXmlPack::addMuPack(CXmlPackArray muxmlpack)
{
	int ret;
	CXmlPack *xmlpack;
	if (muxmlpack.size()<1)
	{
		sprintf(m_errMsg,"参数无记录");
		return false;
	}
	for (int i=0;i<muxmlpack.size();i++)
	{
		xmlpack = muxmlpack.GetAt(i);
		ret = addPack(xmlpack);
	}
	return true;
}
bool CXmlPack::addMuPackPoint(CXmlPackArray muxmlpack)
{
	int ret;
	CXmlPack *xmlpack=NULL;
	if (muxmlpack.size()<1)
	{
		sprintf(m_errMsg,"参数无记录");
		return false;
	}
	for (int i=0;i<muxmlpack.size();i++)
	{
		xmlpack = muxmlpack.GetAt(i);
		ret = addPackPoint(xmlpack);
		if (ret!=0)
		{
			return false;
		}
	}
	return true;
}
bool CXmlPack::newPackFromMuPack(CXmlPackArray muxmlpack)
{
	newPackage();
	int ret;
	CXmlPack *xmlpack;
	if (muxmlpack.size()<1)
	{
		sprintf(m_errMsg,"参数无记录");
		return false;
	}
	xmlpack = muxmlpack.GetAt(0);
	XmlCopy(xmlpack);
	for (int i=1;i<muxmlpack.size();i++)
	{
		xmlpack = muxmlpack.GetAt(i);
		ret = addPack(xmlpack);
	}
	return true;
}
bool CXmlPack::newPackFromMuPackPoint(CXmlPackArray muxmlpack)
{
	newPackage();
	int ret;
	CXmlPack *xmlpack=NULL;
	if (muxmlpack.size()<1)
	{
		sprintf(m_errMsg,"参数无记录");
		return false;
	}
	xmlpack = muxmlpack.GetAt(0);
	XmlCopy(xmlpack);
	for (int i=1;i<muxmlpack.size();i++)
	{
		xmlpack = muxmlpack.GetAt(i);
		ret = addPackPoint(xmlpack);
	}
	return true;
}
bool CXmlPack::XmlCopy(CXmlPack *xmlpack)
{
	if  (!m_xml.XmlCopy(&(xmlpack->m_xml)))
	{
		return false;
	}
	m_rootnode = m_xml.m_firstNode;
	if (!readPack())
	{
		return false;
	}
	if (xmlpack->isBin())
	{
		if (m_rowbuf != NULL)
		{
			free(m_rowbuf);
			m_rowbuf = NULL;
			
		}
		m_rowbuf = (char *)malloc(xmlpack->m_rowbuflen+1);
		memset(m_rowbuf,0,xmlpack->m_rowbuflen+1);
		xmlpack->getBinData(m_rowbuf,m_rowbuflen);
		setBin(true);
	}
	else
	{
		m_rowbuflen = 0;
	}
	return true;
}
bool CXmlPack::XmlPointCopy(CXmlPack *xmlpack)
{
	if  (!m_xml.XmlPointCopy(&(xmlpack->m_xml)))
	{
		sprintf(m_errMsg,"XmlPointCopy出错");
		return false;
	}
	xmlpack->readPack();
	m_rootnode = m_xml.m_firstNode;
	if (!readPack())
	{
		return false;
	}
	if (xmlpack->isBin())
	{
		if (m_rowbuf != NULL)
		{
			free(m_rowbuf);
			m_rowbuf = NULL;
			
		}
		m_rowbuf = (char *)malloc(xmlpack->m_rowbuflen+1);
		memset(m_rowbuf,0,xmlpack->m_rowbuflen+1);
		xmlpack->getBinData(m_rowbuf,m_rowbuflen);
		setBin(true);
	}
	else
	{
		m_rowbuflen = 0;
	}
	return true;
}
int CXmlPack::getXmlDataLen()
{
	int ret ;
	ret = m_xml.GetXmlDatalen();
	if (ret<1)
	{
		return ret;
	}
	if (isBin())
	{
		ret = ret + m_rowbuflen+1;
	}
	return ret;
}

int CXmlPack::GetPackMuValue(std::string name, int recNo, std::string &value)
{
	int ret;
	std::string tmpstring;
	ret = getPackValue(name,tmpstring);
	if (ret != 0)
	{
		return -1;
	}
	ret = 0;
	CNode *tmpnode = m_xml.getEndChildItem();
	while (tmpnode != NULL)
	{
		if (recNo == ret )
		{
			value = tmpnode->getValue();
			return 0;
		}
		ret ++;
		tmpnode = m_xml.getLastSibling();
	}
	return -1;
}

int CXmlPack::GetPackMuValueCount(std::string name)
{
	int ret;
	std::string tmpstring;
	ret = getPackValue(name,tmpstring);
	if (ret != 0)
	{
		return -1;
	}
	ret = 0;
	CNode *tmpnode = m_xml.getFirstChildItem();
	while (tmpnode != NULL)
	{
		ret ++;
		tmpnode = m_xml.getNextSibling();
	}
	return ret;
}
CNode * CXmlPack::SetPackMuValue(std::string name, std::string value,bool istrans)
{
	int ret;
	std::string tmpstring;
	CNode *rtNode=NULL;
	ret = getPackValue(name,tmpstring);
	if (ret != 0)
	{
		//增加
		if (setPackNameValue(name,"",true,false)<0)
		{
			sprintf(m_errMsg,"增加节点[%s]失败",name.c_str());
			return NULL;
		}
		if (istrans)
		{
			rtNode = m_xml.addItem("RECORD",value);
		}
		else
		{
			rtNode = m_xml.addItemN("RECORD",value);
		}
	}
	else
	{
		if (tmpstring.length()>0)
		{
			m_xml.addItem("RECORD",tmpstring);
		}
		if (istrans)
		{
			rtNode = m_xml.addItem("RECORD",value);
		}
		else
		{
			rtNode = m_xml.addItemN("RECORD",value);
		}
	}
	
	return rtNode;
}

bool CXmlPack::XmlCopyHead(CXmlPack *xmlpack)
{
	if (gotoHead()!=0)
	{
		newPackage();
		gotoHead();
	}
	m_xml.deleteItem();
	
	m_pRecNode = NULL;
	CNode *curtmpnode = m_xml.addItemN("head","");
	m_xml.setCurrentNode(curtmpnode);
	if (xmlpack->gotoHead()!=0)
	{
		sprintf(m_errMsg,"源xml无head节点");
		return false;
	}
	xmlpack->m_xml.getParentItem();
	curtmpnode = xmlpack->m_xml.getFirstChildItem("head");	
	curtmpnode = xmlpack->m_xml.getFirstChildItem();
	if (!m_xml.XmlCopyNode(curtmpnode))
	{
		sprintf(m_errMsg,"复制xml的head节点出错");
		return false;
	}
	return readPack();
}

bool CXmlPack::XmlCopyPack(CXmlPack *xmlpack)
{
	if (gotoPack()!=0)
	{
		newPackage();
		gotoPack();
	}
	m_xml.deleteItem();
	CNode *curtmpnode = m_xml.addItemN("pack","");
	m_xml.setCurrentNode(curtmpnode);
	if (xmlpack->gotoHead()!=0)
	{
		sprintf(m_errMsg,"源xml无pack节点");
		return false;
	}
	xmlpack->m_xml.getParentItem();
	curtmpnode = xmlpack->m_xml.getFirstChildItem("pack");	
	curtmpnode = xmlpack->m_xml.getFirstChildItem();
	if (!m_xml.XmlCopyNode(curtmpnode))
	{
		sprintf(m_errMsg,"复制xml的pack节点出错");
		return false;
	}
	return readPack();
}

bool CXmlPack::SetPackNode(CNode *pnode)
{
	
	if (pnode == NULL)
	{
		m_packnode = NULL;
		return true;
	}
	m_rootnode = m_xml.getRoot();
	if (m_rootnode != NULL) //根
	{
		CNode *fcnode = m_rootnode->getFirstChild();//package
		if (fcnode != NULL)
		{
			m_xml.setCurrentNode(fcnode);
			m_xml.AddNode(pnode);
			if (strcmp(pnode->m_name.c_str(),"pack")==0)
			{
				m_packnode = pnode;
			}
			
			return true;
		}
	}
	m_packnode = NULL;
	return false;
}
