// SendData.cpp: implementation of the CSendData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "testcomm.h"
#include "SendData.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSendData::CSendData()
{
	memset(m_errMsg,0,sizeof(m_errMsg));
	memset(m_seg,0,sizeof(m_seg));
	m_headLen = 0 ;
	m_isIncludeHead = false;
	m_isFillBlank = false;
	m_packType = -1;
	m_nSuccessNum = 0;
	m_nFaildNum = 0;
	m_bIsExit = false;
	m_bIsBegin = false;
	m_nLogLevel = 4;

}

CSendData::~CSendData()
{
	m_reqans.clear();
}

bool CSendData::GetSendBuf(char *buf,unsigned int &buflen,CXmlPack &xmlpack)
{
	CBF_PMutex pp(&m_sendDataMutex);
//	xmlpack.newPackage();
	xmlpack.XmlCopy(&m_packxml);
	int len = buflen;
	char tmpbuf[30];
	char tmpbuf2[30];
	memset(tmpbuf,0,sizeof(tmpbuf));
	memset(tmpbuf2,0,sizeof(tmpbuf2));
	sprintf(tmpbuf,"%s0%d%s","%",m_headLen,"d");
	xmlpack.tobuffer(buf+m_headLen,len);
	sprintf(tmpbuf2,tmpbuf,len);
	memcpy(buf,tmpbuf2,m_headLen);
	
	buflen = len+m_headLen;
	return true;
}
bool CSendData::GetSendBuf(char *buf,unsigned int &buflen)
{
	CBF_PMutex pp(&m_sendDataMutex);
	memset(buf,0,buflen);
	char headbuffer[8192];
	int headbuflen=sizeof(headbuffer);
	char packbuffer[8192];
	int packbuflen=sizeof(packbuffer);
	
	memset(headbuffer,0,sizeof(headbuffer));
	memset(packbuffer,0,sizeof(packbuffer));
	char tmpbuf[30];
	memset(tmpbuf,0,sizeof(tmpbuf));
	sprintf(tmpbuf,"%s0%d%s","%",m_headLen,"d");

	if (!GetHeadBuffer(headbuffer,headbuflen))
	{
		return false;
	}
	if (!GetPackBuffer(packbuffer,packbuflen))
	{
		return false;
	}
	if (buflen<=headbuflen+packbuflen)
	{
		return false;
	}
	if (m_isIncludeHead)
	{
		sprintf(buf,tmpbuf,packbuflen + headbuflen + m_headLen);
	}
	else
	{
		sprintf(buf,tmpbuf,packbuflen+headbuflen);
	}
	buflen = packbuflen + headbuflen + m_headLen;
	memcpy(buf+m_headLen,headbuffer,headbuflen);
	memcpy(buf+m_headLen+headbuflen,packbuffer,packbuflen);
	buf[buflen]=0;
	if (m_isFillBlank)
	{
		for (int i=0;i<buflen;i++)
		{
			if (buf[i]==0)
			{
				buf[i]= ' ';
			}
		}
	}
	return true;
}

bool CSendData::InitSendData(int headlen, bool isinclude,bool fill, char *headfile, char *packfile)
{
	m_headLen = headlen;
	m_isIncludeHead = isinclude;
	m_isFillBlank = fill;
	if (m_headxml.fromFile(headfile)<0)
	{
		sprintf(m_errMsg,"打开包头配置文件错[%s]",m_headxml.m_errMsg);
		return false;
	}
	if (m_packxml.fromFile(packfile)<0)
	{
		sprintf(m_errMsg,"打开包体配置文件错[%s]",m_packxml.m_errMsg);
		return false;
	}
	return true;
}

void CSendData::SetParameter(int headlen, bool isinclude,bool fill,int type, char *seg)
{
	m_headLen = headlen;
	m_isIncludeHead = isinclude;
	m_isFillBlank = fill;
	m_packType = type;
	m_pTool.StringCopy(m_seg,2,seg);
}

void CSendData::SetPackType(int type, char *seg)
{
	m_packType = type;
	m_pTool.StringCopy(m_seg,2,seg);
}

bool CSendData::GetHeadBuffer(char *buf, int &buflen)
{
	m_headxml.gotoPack();
//	CNode *tmpnode = m_headxml.m_xml.getEndChildItem();
	CNode *tmpnode = m_headxml.m_xml.getFirstChildItem();
	int length=0;
	char tmpbuf[500];
	int tmpbuflen=0;
	int destlength;
	destlength = buflen;
	if (destlength<1)
	{
		sprintf(m_errMsg,"缓冲区长度不足");
		return false;
	}
	while (tmpnode!=NULL)
	{
		memset(tmpbuf,0,sizeof(tmpbuf));
		if (!GetFieldBuffer(tmpnode,tmpbuf,tmpbuflen))
		{
			return false;
		}
		if (length >= destlength)
		{
			sprintf(m_errMsg,"缓冲区长度不足");
			return false;
		}
		memcpy(buf+length,tmpbuf,tmpbuflen);
		length += tmpbuflen;
		//tmpnode = tmpnode->getLastSibling();
		tmpnode = tmpnode->getNextSibling();
	}
	buflen = length;
	return true;
}

bool CSendData::GetPackBuffer(char *buf, int &buflen)
{
	m_packxml.gotoPack();
//	CNode *tmpnode = m_packxml.m_xml.getEndChildItem();
	CNode *tmpnode = m_packxml.m_xml.getFirstChildItem();
	int length=0;
	char tmpbuf[5000];
	int tmpbuflen=0;
	int destlength;
	destlength = buflen;
	if (destlength<1)
	{
		sprintf(m_errMsg,"缓冲区长度不足");
		return false;
	}
	while (tmpnode!=NULL)
	{
		memset(tmpbuf,0,sizeof(tmpbuf));
		if (!GetFieldBuffer(tmpnode,tmpbuf,tmpbuflen))
		{
			return false;
		}
		if (length >= destlength)
		{
			sprintf(m_errMsg,"缓冲区长度不足");
			return false;
		}
		memcpy(buf+length,tmpbuf,tmpbuflen);
		length += tmpbuflen;
		//tmpnode = tmpnode->getLastSibling();
		tmpnode = tmpnode->getNextSibling();
	}
	buflen = length;
	return true;
}

bool CSendData::GetAttributes(CNode *node, S_FIELD_ATTRIBUTE *s_attr)
{
	if (node==NULL)
	{
		sprintf(m_errMsg,"节点为空");
		return false;
	}
	CAttribute *att1=NULL;
	CAttribute *att2=NULL;
	att1 = node->getAttributes();
	att2 = att1->getNamedItem("type");
	if (att2==NULL)
	{
		sprintf(m_errMsg,"type属性没有设置");
		return false;
	}
	s_attr->type = atoi((att2->getValue()).c_str());
	att2 = att1->getNamedItem("dec");
	if (att2==NULL)
	{
		sprintf(m_errMsg,"dec属性没有设置");
		return false;
	}
	s_attr->dec = atoi((att2->getValue()).c_str());
	att2 = att1->getNamedItem("align");
	if (att2==NULL)
	{
		sprintf(m_errMsg,"align属性没有设置");
		return false;
	}
	s_attr->align = atoi((att2->getValue()).c_str());
	att2 = att1->getNamedItem("length");
	if (att2==NULL)
	{
		sprintf(m_errMsg,"length属性没有设置");
		return false;
	}
	s_attr->length = atoi((att2->getValue()).c_str());
	return true;
}

bool CSendData::GetFieldBuffer(CNode *node,char *buf,int &buflen)
{
	S_FIELD_ATTRIBUTE s_field;
	memset(&s_field,0,sizeof(S_FIELD_ATTRIBUTE));
	if (!GetAttributes(node,&s_field))
	{
		return false;
	}
	char value[5000];
	memset(value,0,sizeof(value));
	if (!GetValue(node,value))
	{
		return false;
	}
	if (m_packType==0)
	{
		GetFieldBufferByType0(value,s_field,buf,buflen);
	}
	else if (m_packType==1)
	{
		GetFieldBufferByType1(value,s_field,buf,buflen);
	}
	else if (m_packType==2)
	{
		GetFieldBufferByType2(value,s_field,buf,buflen);
	}
	else
	{
		sprintf(m_errMsg,"包类型不符[%d]",m_packType);
		return false;
	}
	return true;
}

bool CSendData::GetValue(CNode *node, char *value)
{
	strcpy(value,node->getValue().c_str());
	m_pDate.Update();
	if (value[0]=='@') //用变量代替
	{
		if (strncmp(value+1,"DATE",4)==0 || strncmp(value+1,"DATE10",6)==0)
		{
			m_pDate.ToCharDate(value);
			return true;
		}
		if (strncmp(value+1,"DATE18",6)==0 )
		{
			sprintf(value,"%s %s",m_pDate.ToStringD10().c_str(),m_pDate.ToStringT8());
			return true;
		}
		if (strncmp(value+1,"TIME",4)==0 || strncmp(value+1,"TIME8",8)==0)
		{
			m_pDate.ToCharTime8(value);
			return true;
		}
		if (strncmp(value+1,"TIME6",5)==0 )
		{
			m_pDate.ToCharTime(value);
			return true;
		}
		if (strncmp(value+1,"SEC",3)==0 )
		{
			sprintf(value,"%I64d",m_pDate.GetTickCount());
			return true;
		}
	}
	else if (value[0]=='~') //变量+1
	{
		__int64 lvalue = _atoi64(value+1);
		sprintf(value,"%I64d",lvalue+1);
		node->m_value = (string)"~"+(string)value;
	}
	else if (strncmp(value,"##",2)==0) //交易码
	{
		m_packxml.getHeadValueChar("交易码",value);
	}
	return true;
}

bool CSendData::GetFieldBufferByType0(char *value, S_FIELD_ATTRIBUTE s_field, char *buf, int &buflen)
{
	buflen = s_field.length;
	if (s_field.type == 1) //double类型
	{
		double dtmp = atof(value);
		dtmp = m_pTool.Round(dtmp,s_field.dec);
		char tmpbuf[30];
		memset(tmpbuf,0,sizeof(tmpbuf));
		sprintf(tmpbuf,"%s.%dlf","%",s_field.dec);
		sprintf(value,tmpbuf,dtmp);
	}
	if (strlen(value) >= s_field.length)
	{
		memcpy(buf,value,s_field.length);
	}
	else
	{
		if (s_field.align == 0) //左对齐
		{
			memcpy(buf,value,strlen(value));
		}
		else //右对齐
		{
			char tmpbuf[30];
			memset(tmpbuf,0,sizeof(tmpbuf));
			sprintf(tmpbuf,"%s+%d%s","%",s_field.length,"s");
			sprintf(buf,tmpbuf,value);
		}
	}
	return true;
}

bool CSendData::GetFieldBufferByType1(char *value, S_FIELD_ATTRIBUTE s_field, char *buf, int &buflen)
{
	int seglen;
	bool isasc;
	if ( m_pTool.IsNumber(m_seg))
	{
		seglen = 1;
		isasc = true;
		buflen = s_field.length+1;	
	}
	else
	{
		seglen = strlen(m_seg);
		isasc = false;
		buflen = s_field.length+seglen;
	}
	
	if (s_field.type == 1) //double类型
	{
		double dtmp = atof(value);
		dtmp = m_pTool.Round(dtmp,s_field.dec);
		char tmpbuf[30];
		memset(tmpbuf,0,sizeof(tmpbuf));
		sprintf(tmpbuf,"%s.%dlf","%",s_field.dec);
		sprintf(value,tmpbuf,dtmp);
	}
	if (strlen(value) >= s_field.length)
	{
		memcpy(buf,value,s_field.length);
	}
	else
	{
		if (s_field.align == 0) //左对齐
		{
			memcpy(buf,value,strlen(value));
		}
		else //右对齐
		{
			char tmpbuf[30];
			memset(tmpbuf,0,sizeof(tmpbuf));
			sprintf(tmpbuf,"%s+%d%s","%",s_field.length,"s");
			sprintf(buf,tmpbuf,value);
		}
	}
	if (isasc)
	{
		buf[s_field.length]=atoi(m_seg);
	}
	else
	{
		for (int i=0;i<seglen;i++)
		{
			buf[s_field.length+i] = m_seg[i];
		}
	}
	buf[s_field.length+seglen]=0;
	return true;
}

bool CSendData::GetFieldBufferByType2(char *value, S_FIELD_ATTRIBUTE s_field, char *buf, int &buflen)
{
	if (s_field.type == 1) //double类型
	{
		double dtmp = atof(value);
		dtmp = m_pTool.Round(dtmp,s_field.dec);
		char tmpbuf[30];
		memset(tmpbuf,0,sizeof(tmpbuf));
		sprintf(tmpbuf,"%s.%dlf","%",s_field.dec);
		sprintf(value,tmpbuf,dtmp);
	}
	if (m_pTool.IsNumber(m_seg))
	{
		sprintf(buf,"%s",value);
		buflen = strlen(buf)+1;
		buf[buflen-1]=atoi(m_seg);
		buf[buflen]=0;
	}
	else
	{
		sprintf(buf,"%s%s",value,m_seg);
		buflen = strlen(buf);
	}
	return true;
}

bool CSendData::PushReqAns(S_REQANS s_reqans)
{
	CBF_PMutex pp(&m_putDataMutex);
	m_reqans.push_back(s_reqans);
	return true;
}

void CSendData::putResult(bool result)
{
	CBF_PMutex pp(&m_putDataMutex);
	if (result)
	{
		m_nSuccessNum++;
	}
	else
	{
		m_nFaildNum++;
	}
}
