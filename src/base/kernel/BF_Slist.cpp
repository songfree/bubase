// BF_Slist.cpp: implementation of the CBF_Slist class.
//
//////////////////////////////////////////////////////////////////////

#include "BF_Slist.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define  SPEP 9       //非汉字全角后添加的字符


CBF_Slist::CBF_Slist(const char *sep)
{
	m_sSepstr = sep;
	m_count = 0;
	m_bNotSepString = false;
}

CBF_Slist::~CBF_Slist()
{
	Clear();
}
void CBF_Slist::Add(std::string data)
{
	m_buffer_list.push_back(data);
	m_count++;
}

void CBF_Slist::Add(int data)
{
	char buffer[60];
	memset(buffer,0,sizeof(buffer));
	sprintf(buffer,"%d",data);
	Add(buffer);
}
void CBF_Slist::Add(long data)
{
	char buffer[60];
	memset(buffer,0,sizeof(buffer));
	sprintf(buffer,"%ld",data);
	Add(buffer);
}
void CBF_Slist::Add(double data,int dec)
{
	char buffer[60];
	memset(buffer,0,sizeof(buffer));
	char tmpstr[20];
	memset(tmpstr,0,sizeof(tmpstr));
	sprintf(tmpstr,"%s.%d%s","%",dec,"lf");
	sprintf(buffer,tmpstr,CBF_Tools::Round(data,dec));
	Add(buffer);
}
bool CBF_Slist::GetAt(int nIndex,std::string &data,bool flag)
{
	if (nIndex<0 || nIndex>=m_count) 
	{
		data = "";
		return false; 
	}
	STRINGSLIST::iterator plist;
    plist=m_buffer_list.begin();
	for(int i=0;i<nIndex;i++) 
	{
		plist++;
	}
	data = *plist;
	if (flag) 
	{
		CBF_Tools::LRtrim(data); 
	}
    return true;
}
std::string CBF_Slist::GetAt(int nIndex,bool flag)
{
	if (nIndex<0 || nIndex>=m_count) 
	{
		return ""; 
	}
	STRINGSLIST::iterator plist;
    plist=m_buffer_list.begin();
	for(int i=0;i<nIndex;i++) 
	{
		plist++;
	}
	
	if (flag) 
	{
		std::string data = *plist;
		CBF_Tools::LRtrim(data); 
		return data;
	}
    return *plist;
}

bool CBF_Slist::SetAt(std::string buffer,int nIndex)
{
	if(nIndex<0 || nIndex>=m_count) 
	{
		return false;
	}
    STRINGSLIST::iterator plist;
    plist=m_buffer_list.begin();
    for(int i=0;i<nIndex;i++)
	{
		plist++;
	}
	m_buffer_list.insert(plist,buffer);
    m_buffer_list.erase(plist);
	return true;
}

int CBF_Slist::GetCount()
{
    return m_count;
}

void CBF_Slist::Clear()
{
	m_count=0;
	m_buffer_list.clear();
}


void CBF_Slist::SetSeparateString(const char *sepstr)
{
	m_sSepstr = sepstr;
}

int CBF_Slist::FillSepString(std::string datastring)
{
	int nPos=0;
	unsigned int offset=0;
	m_bNotSepString = false;
	while (nPos != -1)
	{
		nPos = datastring.find(m_sSepstr,offset);//找到分隔符
		if (nPos == -1) //没有找到
		{
		    if (offset == 0)
		    {
		        Add(datastring);
				m_bNotSepString = true;
		    }
			else if  (offset <  datastring.length()-1)
			{
				Add(datastring.substr(offset,datastring.length()-offset));
			}
			break;
		}
		else //找到了
		{
			if (offset == nPos)
			{
				Add("");
			}
			else
			{
// #ifdef _DEBUG
// 				printf("data=[%s]\n",datastring.substr(offset,nPos-offset).c_str());
// #endif
				Add(datastring.substr(offset,nPos-offset));
			}
			offset = nPos + m_sSepstr.length();
		}
	}
	return m_count;
}

std::string CBF_Slist::ToString(unsigned int flag)
{
	int i;
	if (m_count<1)
	{
		return "";
	}
	char flagstr[11];
	bzero(flagstr,sizeof(flagstr));
	switch (flag)
	{
		case 0:
		case 1:
		case 2:
		case 3:
			CBF_Tools::ToBinaryString(flag,flagstr);
			break;
		default:
			return "";
	}
	std::string tmpstr;

	if (flagstr[6] == '1')
	{
		tmpstr = m_sSepstr; //前面要加上分隔符
	}
	for(i=0; i<m_count; i++)
	{
		tmpstr = tmpstr+ GetAt(i);
		if (i == m_count -1)
		{
			if (flagstr[7] == '1') //最后面要加上分隔符
			{
				tmpstr = tmpstr + m_sSepstr;
			}
		}
		else
		{
			tmpstr = tmpstr + m_sSepstr;
		}
	}
	return tmpstr;
}

std::string CBF_Slist::ToStringException(unsigned int index, unsigned int flag)
{
	unsigned int i;
	if (m_count<1)
	{
		return "";
	}
	char flagstr[11];
	bzero(flagstr,sizeof(flagstr));
	switch (flag)
	{
	case 0:
	case 1:
	case 2:
	case 3:
		CBF_Tools::ToBinaryString(flag,flagstr);
		break;
	default:
		return "";
	}
	std::string tmpstr;
	if (flagstr[6] == '1')
	{
		tmpstr = m_sSepstr; //前面要加上分隔符
	}
	for(i=0; i<(unsigned int)m_count; i++)
	{
		if (i == index)
		{
			continue;
		}
		tmpstr = tmpstr+ GetAt(i);
		if (i == m_count -1)
		{
			if (flagstr[7] == '1') //最后面要加上分隔符
			{
				tmpstr = tmpstr + m_sSepstr;
			}
		}
		else
		{
			tmpstr = tmpstr + m_sSepstr;
		}
	}
	return tmpstr;
}
bool CBF_Slist::fastDectGB(const char * src)
{
	int i, len;
	unsigned char *p;
	
	p = (unsigned char *)src;
	len = strlen(src);
	for(i = 0 ; i < len; i++)
	{
		if(*p >= 128)
		{
			return true;
		}
		p++;
	}
	return false;
}
//非全角、汉字后添加SPEP
//调用者必须保证destbuf有足够的空间去保存数据
void CBF_Slist::bufFillSP(const char* databuf,  char *destbuf)
{
	int i, dlen;
	unsigned char *p, *q;
	dlen = strlen(databuf);
	p = (unsigned char *)databuf;
	q = (unsigned char *)destbuf;
	int nlen=0;
	if(fastDectGB(databuf))
	{
		for(i = 0 ; i < dlen; i++ , p++,q++)
		{
	
			*q = *p;  //复制
			if(*q < 128)
			{
				q++;
				nlen++;
				*q = SPEP;// 占位
			}
			else //后移两位，中间加
			{
				p++;
				q++;
				*q = *p;  //复制
			}
			
		}
		destbuf[dlen+nlen]=0;//加结束符
	}
	else
	{
        strcpy(destbuf, databuf);
    }
	return ;
}
//去掉SPEP
void  CBF_Slist::bufRemoveSP(const char* databuf,  char *destbuf)
{
	int i, dlen;
	unsigned char *p, *q;
	dlen = strlen(databuf);
	p = (unsigned char *)databuf;
	q = (unsigned char *)destbuf;
	
	for(i = 0 ; i < dlen; i++ , p++)
	{
		if(*p !=  SPEP)
		{
			*q = *p;  //复制
			q++;
		}
	}
	*q = 0;//加结束符
	return ;
}

int CBF_Slist::FillChnSepString(std::string datastring)
{
	int nPos=0;
	unsigned int offset=0;
	unsigned int dataoffset=0;
	//非汉字全解分隔符还是走原来的
	if (!fastDectGB(m_sSepstr.c_str()))
	{
		return FillSepString(datastring);
	}

	char *pData = (char *)malloc(datastring.length()*2+100);
	if (pData == NULL)
	{
		return -1;
	}
	bzero(pData,datastring.length()*2+100);
	bufFillSP(datastring.c_str(),pData);
	std::string pDataString = pData;
	m_bNotSepString = false;
	while (nPos != -1)
	{
		nPos = pDataString.find(m_sSepstr,offset);//找到分隔符
		if (nPos == -1) //没有找到
		{
			if (offset == 0)
			{
				bufRemoveSP(pDataString.c_str(),pData);
				m_bNotSepString = true;
				Add(pData);
			}
			else if  (offset <  datastring.length()-1)
			{
				bufRemoveSP(pDataString.substr(dataoffset,pDataString.length()-dataoffset).c_str(),pData);
				Add(pData);
			}
			break;
		}
		else //找到了
		{
			
			if (nPos % 2 !=0)
			{
				offset = nPos+1;
				continue;
			}
			if (offset == nPos)
			{
				Add("");
			}
			else
			{
				bufRemoveSP(pDataString.substr(dataoffset,nPos-dataoffset).c_str(),pData);
				Add(pData);
			}
			offset = nPos + m_sSepstr.length();
			dataoffset = offset;
		}
	}
	free(pData);
	pData = NULL;
	return m_count;
}
