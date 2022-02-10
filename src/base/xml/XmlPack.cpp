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
		sprintf(m_errMsg,"�½�<package>�ڵ�ʧ��");
		return ;
	}
	m_xml.setCurrentNode(tmpnode);
	tmpnode = m_xml.addItemN("head","");
	if (tmpnode==NULL)
	{
		sprintf(m_errMsg,"�½�<head>�ڵ�ʧ��");
		return ;
	}
	m_headnode = tmpnode;
	tmpnode = m_xml.addItemN("pack","");
	if (tmpnode==NULL)
	{
		sprintf(m_errMsg,"�½�<pack>�ڵ�ʧ��");
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
	//�ص����ڵ�
	m_rootnode = m_xml.getRoot();
	m_headnode = NULL;
	m_packnode = NULL;
	m_pRecNode = NULL;
	//��õ�һ���ӽڵ㣬��ǰ�ڵ�Ϊ�ӽڵ�
	CNode *tmpnode = m_xml.getFirstChildItem();
	if (tmpnode==NULL)
	{
		sprintf(m_errMsg,"xml���ݰ�û��������");
		return false;
	}
	//��һ���ӽڵ��Ƿ�Ϊpackage,�������ʾxml������

	if (strcmp(tmpnode->getName().c_str(),"package")==0) 
	{
		//ȷ���ǺϷ�xml����Ϣ��ȡ��ͷ������
		//ȡ�õ�ǰ�ڵ���ӽڵ�����Ϊ"head"�Ľڵ�,��ǰ�ڵ�Ϊhead
		m_headnode = m_xml.getFirstChildItem("head");
		if (m_headnode==NULL)
		{
			sprintf(m_errMsg,"xml���ݰ�û��head������");
			return false;
		}
		m_xml.getParentItem();
		m_packnode = m_xml.getFirstChildItem("pack");
		if (m_packnode==NULL)
		{
			sprintf(m_errMsg,"xml���ݰ�û��pack������");
			return false;
		}
		return true;
	}
	sprintf(m_errMsg,"��һ���ӽڵ㲻��package,%s",tmpnode->getName().c_str());
	return false;
}

int CXmlPack::setHeadNameValue(std::string name, std::string value,bool isupdate,bool istrans)
{
	CNode *tmpnode;
	if (m_headnode==NULL)
	{
		sprintf(m_errMsg,"��ͷ�ڵ�Ϊ��");
		return -1;
	}
	tmpnode = m_xml.setCurrentNode(m_headnode);
	if (tmpnode !=m_headnode)
	{
		sprintf(m_errMsg,"���ð�ͷ�ڵ����");
		return -1;
	}
	if (isupdate)
	{
		tmpnode = m_xml.getFirstChildItem(name);
		if (tmpnode==NULL) //����
		{
			tmpnode = m_xml.setCurrentNode(m_headnode);
			if (tmpnode !=m_headnode)
			{
				sprintf(m_errMsg,"���ð�ͷ�ڵ����");
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
				sprintf(m_errMsg,"���ð�ͷ�ڵ�(����)����");
				return -1;
			}
			m_xml.setCurrentNode(tmpnode);
		}
		else //�޸�
		{
			if (istrans)
			{
				if (!m_xml.editItem(value))
				{
					sprintf(m_errMsg,"���ð�ͷ�ڵ�(�޸�)����");
					return -1;
				}
			}
			else
			{
				if (!m_xml.editItemN(value))
				{
					sprintf(m_errMsg,"���ð�ͷ�ڵ�(�޸�)����");
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
			sprintf(m_errMsg,"���ð�ͷ�ڵ�(����)����");
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
		sprintf(m_errMsg,"����ڵ�Ϊ��");
		return -1;
	}
	tmpnode = m_xml.setCurrentNode(m_packnode);
	if (tmpnode !=m_packnode)
	{
		sprintf(m_errMsg,"���ð���ڵ����");
		return -1;
	}
	if (isupdate) //���ҽڵ���и��»�����
	{
		tmpnode = m_xml.getFirstChildItem(name);
		if (tmpnode==NULL) //����
		{
			tmpnode = m_xml.setCurrentNode(m_packnode);
			if (tmpnode !=m_packnode)
			{
				sprintf(m_errMsg,"���ð���ڵ����");
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
				sprintf(m_errMsg,"���ð���ڵ�(����)����");
				return -1;
			}
			m_xml.setCurrentNode(tmpnode);
		}
		else //�޸�
		{
			if (istrans)
			{
				if (!m_xml.editItem(value))
				{
					sprintf(m_errMsg,"���ð���ڵ�(�޸�)����");
					return -1;
				}
			}
			else
			{	
				if (!m_xml.editItemN(value))
				{
					sprintf(m_errMsg,"���ð���ڵ�(�޸�)����");
					return -1;
				}
			}
			
		}
	}
	else //���ӽڵ�
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
			sprintf(m_errMsg,"���ð���ڵ�(����)����");
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
		sprintf(m_errMsg,"����ڵ�Ϊ��");
		return -1;
	}
	tmpnode = m_xml.setCurrentNode(m_packnode);
	if (tmpnode !=m_packnode)
	{
		sprintf(m_errMsg,"���ð���ڵ����");
		return -1;
	}
	tmpnode = m_xml.getParentItem();
	if (tmpnode == NULL)
	{
		sprintf(m_errMsg,"pack���ڵ�Ϊ��");
		return -1;
	}
	CNode *tmpnode2 = m_xml.getFirstChildItem(item);
	if (tmpnode2 == NULL)
	{
		sprintf(m_errMsg,"�ڵ�<%s>������",item.c_str());
		return -1;
	}
	if (isupdate)
	{
		tmpnode = m_xml.getFirstChildItem(name);
		if (tmpnode==NULL) //����
		{
			tmpnode = m_xml.setCurrentNode(tmpnode2);
			if (tmpnode !=tmpnode2)
			{
				sprintf(m_errMsg,"���ð���ڵ����");
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
				sprintf(m_errMsg,"���ð���ڵ�(����)����");
				return -1;
			}
			m_xml.setCurrentNode(tmpnode);
		}
		else //�޸�
		{
			if (istrans)
			{
				if (!m_xml.editItem(value))
				{
					sprintf(m_errMsg,"���ð���ڵ�(�޸�)����");
					return -1;
			}
			}
			else
			{
				if (!m_xml.editItemN(value))
				{
					sprintf(m_errMsg,"���ð���ڵ�(�޸�)����");
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
			sprintf(m_errMsg,"���ð���ڵ�(����)����");
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
		sprintf(m_errMsg,"��¼������С��1");
		return -1;
	}
	if (m_packnode==NULL)
	{
		sprintf(m_errMsg,"����ڵ�Ϊ��");
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
			sprintf(m_errMsg,"���ļ�[%s]ʧ��",filename);
			return -1;
		}
		fprintf(fp,"<?xml version=%c1.0%c encoding=%cGB2312%c?>\n",'"','"','"','"');
		char *buffer = (char *)malloc(datalen+1);
		if (buffer == NULL)
		{
			sprintf(m_errMsg,"�����ڴ�ռ�ʧ��");
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
	sprintf(m_errMsg,"XML������");
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
				sprintf(m_errMsg,"ɾ�����нڵ�ʧ��");
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
		sprintf(m_errMsg,"�½�<package>�ڵ�ʧ��");
		return -1;
	}
	m_xml.setCurrentNode(tmpnode);
	tmpnode = m_xml.addItemN("head","");
	if (tmpnode==NULL)
	{
		sprintf(m_errMsg,"�½�<head>�ڵ�ʧ��");
		return -1;
	}
	m_headnode = tmpnode;
	m_xml.setCurrentNode(tmpnode);
	if (m_xml.addItemN("������","0") == NULL)
	{
		sprintf(m_errMsg,"�½�<head>�ڵ��<������>�ڵ�ʧ��");
		return -1;
	}
	if (m_xml.addItemN("������","0") == NULL)
	{
		sprintf(m_errMsg,"�½�<head>�ڵ��<������>�ڵ�ʧ��");
		return -1;
	}
	if (m_xml.addItemN("NEXTFLAG","0") == NULL)
	{
		sprintf(m_errMsg,"�½�<head>�ڵ��<NEXTFLAG>�ڵ�ʧ��");
		return -1;
	}
	if (m_xml.addItemN("FIRSTFLAG","1") == NULL)
	{
		sprintf(m_errMsg,"�½�<head>�ڵ��<FIRSTFLAG>�ڵ�ʧ��");
		return -1;
	}
	if (m_xml.addItemN("�ܼ�¼��","1") == NULL)
	{
		sprintf(m_errMsg,"�½�<head>�ڵ��<�ܼ�¼��>�ڵ�ʧ��");
		return -1;
	}
	m_pRecNode = m_xml.addItemN("������¼��","1");
	if ( m_pRecNode == NULL)
	{
		sprintf(m_errMsg,"�½�<head>�ڵ��<������¼��>�ڵ�ʧ��");
		return -1;
	}
	if (m_xml.addItemN("�ܰ���","1") == NULL)
	{
		sprintf(m_errMsg,"�½�<head>�ڵ��<�ܰ���>�ڵ�ʧ��");
		return -1;
	}
	if (m_xml.addItemN("BINFLAG","0") == NULL)
	{
		sprintf(m_errMsg,"�½�<head>�ڵ��<BINFLAG>�ڵ�ʧ��");
		return -1;
	}
	if (m_xml.addItemN("ҵ����","0") == NULL)
	{
		sprintf(m_errMsg,"�½�<head>�ڵ��<ҵ����>�ڵ�ʧ��");
		return -1;
	}
	if (m_xml.addItemN("ҵ�������ˮ","0") == NULL)
	{
		sprintf(m_errMsg,"�½�<head>�ڵ��<ҵ�������ˮ>�ڵ�ʧ��");
		return -1;
	}
	
	m_xml.setCurrentNode(m_headnode);
	m_xml.getParentItem();
	tmpnode = m_xml.addItemN("pack","");
	if (tmpnode==NULL)
	{
		sprintf(m_errMsg,"�½�<pack>�ڵ�ʧ��");
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
	//ȡ������¼��
	if (!getRecCount(rec))
	{
		rec = 1;
	}
	sprintf(packrec,"pack%d",rec);
	if (m_packnode == NULL)
	{
		sprintf(m_errMsg,"��pack�ڵ�,����ʧ��");
		return -1;
	}
	//�ص�pack�ڵ�ĸ��ڵ㣬�Ա�����
	tmpnode = m_xml.setCurrentNode(m_packnode);
	if (tmpnode != m_packnode)
	{
		sprintf(m_errMsg,"���ð���ڵ�ʧ��,����ʧ��");
		return -1;
	}
	CNode *tmpnode2 = m_xml.getParentItem();
	if (tmpnode2 == NULL)
	{
		sprintf(m_errMsg,"���ذ���ڵ㸸�ڵ�ʧ��,����ʧ��");
		return -1;
	}
	//ȡ�˽ڵ��Ƿ���ڣ���������head����
	tmpnode = m_xml.getFirstItem(packrec);
	if (tmpnode!=NULL) //���д˽ڵ㣬ֱ����head����
	{
		packname = packrec ;
		rec++;
		setRecCount(rec);
		return rec;
	}
	//�ٴλص����ڵ�
	tmpnode = m_xml.setCurrentNode(tmpnode2);
	if (tmpnode != tmpnode2)
	{
		sprintf(m_errMsg,"���ð���ڵ�ʧ��,����ʧ��");
		return -1;
	}
	//����
	tmpnode = m_xml.addItemN(packrec,"");
	if (tmpnode == NULL)
	{
		sprintf(m_errMsg,"���ӽڵ�<%s>ʧ��",packrec);
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
		sprintf(m_errMsg,"��ͷ�ڵ�Ϊ��");
		return -1;
	}
	tmpnode = m_xml.setCurrentNode(m_headnode);
	if (tmpnode !=m_headnode)
	{
		sprintf(m_errMsg,"���ð�ͷ�ڵ����");
		return -1;
	}
	tmpnode = m_xml.getFirstChildItem(name);
	if (tmpnode==NULL)
	{
		sprintf(m_errMsg,"head��<%s>��ֵ",name.c_str());
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
		sprintf(m_errMsg,"��ͷ�ڵ�Ϊ��");
		return -1;
	}
	tmpnode = m_xml.setCurrentNode(m_packnode);
	if (tmpnode !=m_packnode)
	{
		sprintf(m_errMsg,"���ð���ڵ����");
		return -1;
	}
	tmpnode = m_xml.getFirstChildItem(name);
	if (tmpnode==NULL)
	{
		sprintf(m_errMsg,"pack��<%s>��ֵ",name.c_str());
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
		sprintf(m_errMsg,"��ͷ�ڵ�Ϊ��");
		return -1;
	}
	tmpnode = m_xml.setCurrentNode(m_headnode);
	if (tmpnode !=m_headnode)
	{
		sprintf(m_errMsg,"���ð�ͷ�ڵ����");
		return -1;
	}
	tmpnode = m_xml.getFirstChildItem(name);
	if (tmpnode==NULL)
	{
		sprintf(m_errMsg,"head��<%s>��ֵ",name);
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
		sprintf(m_errMsg,"��ͷ�ڵ�Ϊ��");
		return -1;
	}
	tmpnode = m_xml.setCurrentNode(m_packnode);
	if (tmpnode !=m_packnode)
	{
		sprintf(m_errMsg,"���ð���ڵ����");
		return -1;
	}
	tmpnode = m_xml.getFirstChildItem(name);
	if (tmpnode==NULL)
	{
		sprintf(m_errMsg,"pack��<%s>��ֵ",name);
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
		sprintf(m_errMsg,"��pack�ڵ�");
		return -1;
	}
	CNode *tmpnode = m_xml.getParentItem();
	int rec=0;
	if (!getRecCount(rec)) //û�б�����¼��
	{
		//ֻɾ��pack����,����ɾ��pack%d
	}
	else //ɾ����pack
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
				sprintf(m_errMsg,"ɾ��%s�ڵ����",packname);
				return -1;
			}
		}
		
	}
	if (gotoPack()<0)
	{
		sprintf(m_errMsg,"��pack�ڵ�");
		return -1;
	}
	if (!m_xml.deleteItem())
	{
		sprintf(m_errMsg,"ɾ��pack�ڵ����");
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
			sprintf(m_errMsg,"�����ڴ�ռ�ʧ��");
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
			sprintf(m_errMsg,"�����ڴ�ռ�ʧ��");
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
	if (setHeadNameValue("������",retCode,true) < 0 )
	{
		return false;
	}
	return true;
}

bool CXmlPack::setRetCode(const char *retCode)
{
	if (setHeadNameValue("������",retCode,true,true) < 0 )
	{
		return false;
	}
	return true;
}
bool CXmlPack::setRetMsg(const char *msg)
{
	if (setPackNameValue("������Ϣ",msg,true,true) < 0 )
	{
		return false;
	}
	return true;
}

bool CXmlPack::setReturn(int retCode, const char *msg)
{
	if (setHeadNameValue("������",retCode,true)  < 0 )
	{
		return false;
	}
	if (setPackNameValue("������Ϣ",msg,true,true) < 0 )
	{
		return false;
	}
	return true;
}

bool CXmlPack::setReturn(const char *retCode, const char *msg)
{
	if (setHeadNameValue("������",retCode,true,true)  < 0 )
	{
		return false;
	}
	if (setPackNameValue("������Ϣ",msg,true,true) < 0 )
	{
		return false;
	}
	return true;
}
bool CXmlPack::getRetCode(int &retCode)
{
	char ret[20];
	memset(ret,0,sizeof(ret));
	if (getHeadValueChar("������",ret)<0)
//	if (getHeadValue("������",retCode) <0)
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
	if (getHeadValueChar("������",retCode)<0)
	{
		return false;
	}
	return true;
}
bool CXmlPack::getRetMsg(char *msg)
{
	if (getPackValueChar("������Ϣ",msg) <0)
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
	if (setHeadNameValue("������¼��",rec,true)<0)
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
	if (getHeadValue("������¼��",rec)<0)
	{
		return false;
	}
	m_pRecNode = m_xml.getCurrentNode();
	return true;
}

bool CXmlPack::setTotalRec(int rec)
{
	if (setHeadNameValue("�ܼ�¼��",rec,true)<0)
	{
		return false;
	}
	return true;
}

bool CXmlPack::getTotalRec(int &rec)
{
	if (getHeadValue("�ܼ�¼��",rec)<0)
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
		sprintf(m_errMsg,"��pack�ڵ�");
		return -1;
	}
	m_xml.getParentItem();
	curtmpnode = m_xml.getFirstChildItem(name);
	if (curtmpnode==NULL)
	{
		//sprintf(m_errMsg,"�����µļ�¼�ڵ�ʧ��");
		return -1;
	}

	m_xml.setCurrentNode(curtmpnode);
	if (xmlpack->gotoPack()!=0)
	{
		sprintf(m_errMsg,"Դxml��pack�ڵ�");
		return -1;
	}
	xmlpack->m_xml.getParentItem();
	curtmpnode = xmlpack->m_xml.getFirstChildItem("pack");	
	curtmpnode = xmlpack->m_xml.getFirstChildItem();
	if (!m_xml.XmlCopyNode(curtmpnode))
	{
		sprintf(m_errMsg,"����xml�ڵ����");
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
		sprintf(m_errMsg,"Դxmlpack��pack�ڵ�");
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
	//ȡ������¼��
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
// 		sprintf(m_errMsg,"���ӽڵ����");
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
		sprintf(m_errMsg,"recNo���ڱ�����¼��");
		return false;
	}
	xmlpack->newPackage();
	getRetCode(ret);
	xmlpack->setRetCode(ret);

	if (gotoPack()!=0)
	{
		sprintf(m_errMsg,"ԴXML��pack�ڵ�");
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
			sprintf(m_errMsg,"��[%s]�ڵ�",packname);
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
		sprintf(m_errMsg,"ԴXML��pack�ڵ�");
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
			sprintf(m_errMsg,"��[%s]�ڵ�",packname);
			return false;
		}
		packnode = new CNode();
		packnode->m_name = packname;
	}
	if (!m_xml.deleteNodePoint(srcpacknode)) //��xml������ɾ��pack�ڵ�
	{
		sprintf(m_errMsg,"deleteNodePointʧ��");
		delete packnode;
		packnode = NULL;
		return false;
	}
	
	if (!SetPackNode(packnode))//���½ڵ���Ϊԭpack�ڵ�
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
	if (!xmlpack->m_xml.deleteNode(ttxmlnode)) //ɾ��xmlpack��pack�ڵ�
	{
		ttxmlnode = NULL;
		sprintf(m_errMsg,"%s",xmlpack->m_errMsg);
		return false;
	}
	ttxmlnode = NULL;
	srcpacknode->m_name = "pack";
	if (!xmlpack->SetPackNode(srcpacknode))//��Դ�ڵ���Ϊԭpack�ڵ�
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
			//sprintf(m_errMsg,"ȡpack%d����",i+1);
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
			//sprintf(m_errMsg,"ȡpack%d����",i+1);
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
		sprintf(m_errMsg,"�����޼�¼");
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
		sprintf(m_errMsg,"�����޼�¼");
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
		sprintf(m_errMsg,"�����޼�¼");
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
		sprintf(m_errMsg,"�����޼�¼");
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
		sprintf(m_errMsg,"XmlPointCopy����");
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
		//����
		if (setPackNameValue(name,"",true,false)<0)
		{
			sprintf(m_errMsg,"���ӽڵ�[%s]ʧ��",name.c_str());
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
		sprintf(m_errMsg,"Դxml��head�ڵ�");
		return false;
	}
	xmlpack->m_xml.getParentItem();
	curtmpnode = xmlpack->m_xml.getFirstChildItem("head");	
	curtmpnode = xmlpack->m_xml.getFirstChildItem();
	if (!m_xml.XmlCopyNode(curtmpnode))
	{
		sprintf(m_errMsg,"����xml��head�ڵ����");
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
		sprintf(m_errMsg,"Դxml��pack�ڵ�");
		return false;
	}
	xmlpack->m_xml.getParentItem();
	curtmpnode = xmlpack->m_xml.getFirstChildItem("pack");	
	curtmpnode = xmlpack->m_xml.getFirstChildItem();
	if (!m_xml.XmlCopyNode(curtmpnode))
	{
		sprintf(m_errMsg,"����xml��pack�ڵ����");
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
	if (m_rootnode != NULL) //��
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
