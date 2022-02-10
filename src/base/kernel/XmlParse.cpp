// XmlParse.cpp: implementation of the CXmlParse class.
//
//////////////////////////////////////////////////////////////////////

#include "XmlParse.h"
#include "XmlHead.h"
#include "IXmlNode.h"
#include "BF_Tools.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXmlParse::CXmlParse()
{
	memset(m_errMsg,0,sizeof(m_errMsg));
	m_nLogLevel = 5;///日志级别，大于此级别的日志将不写入日志文件0严重错误1重要错误2一般错误3警告4提示警告5调试信息
#ifdef _WINDOWS
	
#else
	char *p = (char *)getenv("XMLLOGLEVEL");
	if (p != NULL)
	{
		m_nLogLevel = atoi(p);
		p=NULL;
	}
#endif
	m_sXmlBuf = NULL;
	m_sLogName="xmlparse.log";
	m_nId = GetPid();
	m_nXmlBufLen=0;
}

CXmlParse::~CXmlParse()
{
	m_sXmlBuf = NULL;
}

bool CXmlParse::ParseXml(std::string xmlbuf, CIXmlNode *parentNode,int layer)
{
	if (layer<0)
	{
		return false;
	}
	if (layer==0)
	{
		return ParseXmlDom(xmlbuf,parentNode);
	}
	else
	{
		return ParseXmlSax(xmlbuf,parentNode,layer);
	}
	
}
void CXmlParse::GetName(char *filename)
{
	int filelen;
	int i;
	int offset = -1;
	filelen = strlen(filename);
	if (filelen < 1)
	{
		return ;
	}
	for (i=filelen-1 ;i>=0 ; i--)
	{
		if (filename[i] == '\\' || filename[i] == '/' )
		{
			offset = i;
			break;
		}
	}
	if (offset > -1)
	{
		filelen = filelen - offset;
		for (i=0; i< filelen; i++)
		{
			filename[i]=filename[i+offset+1];
		}
		filename[filelen-1]=0;
	}
	return ;
}
int CXmlParse::LogMp(int level, const char *filename, int line,const char *data,int datalen,const char *fmt,...)
{
	if (level>m_nLogLevel)
	{
		return 0;
	}
	FILE *fp;
	char fname[300];
	char fpathname[300];
	char date[20];
	memset(date,0,sizeof(date));
	memset(fname,0,sizeof(fname));
	memset(fpathname,0,sizeof(fpathname));
	strcpy(fname,filename);
	//去掉路径
	GetName(fname);
	char buffer[60];
	memset(buffer,0,sizeof(buffer));
	//0严重错误1重要错误2一般错误3警告4提示警告5调试信息
	switch (level)
	{
		case -1:
			sprintf(buffer,"info ");
			break;
		case 0:
			sprintf(buffer,"FAT ");
			break;
		case 1:
			sprintf(buffer,"ERR ");
			break;	
		case 2:
			sprintf(buffer,"GEN ");
			break;	
		case 3:
			sprintf(buffer,"WRN ");
			break;	
		case 4:
			sprintf(buffer,"TIP ");
			break;	
		case 5:
			sprintf(buffer,"DBG ");
			break;	
		default:
			sprintf(buffer,"OH:%d ",level);
			break;
	}
	m_pDate.Update();
	strcpy(date,m_pDate.ToStringDT17().c_str());
#ifdef _WINDOWS	
	sprintf(fpathname,"%s%s",m_pDate.ToStringD8().c_str(),m_sLogName.c_str());
#else	
    sprintf(fpathname,"%s%s",m_pDate.ToStringD8().c_str(),m_sLogName.c_str());
#endif
	fp=fopen(fpathname,"a");
	if(fp==NULL)
	{
		return -1;
	}
	if (m_nId >=0)
	{
		fprintf(fp,"%s ID[%d] file[%s]line[%d] %s ",date,m_nId,fname,line,buffer);
	}
	else
	{
		fprintf(fp,"%s ID[%d] file[%s]line[%d] %s ",date,GetPid(),fname,line,buffer);
	}
	
	va_list ap;
	va_start(ap, fmt);
    vfprintf(fp, fmt, ap);
    va_end(ap);
    fprintf(fp, "\n");
    fflush(fp);
    fclose(fp);
	return 0;
}

CIXmlNode * CXmlParse::ParseNodeNameAttr(CIXmlNode *parentnode, std::string nameattr,bool isEndNode)
{
//	int rtn;
	char *pbuf=NULL;
	int nBoffset=0;
	int nEoffset=0;
	int nEndLen=0;
	nEndLen = nameattr.length();
	
	pbuf = (char *)nameattr.c_str();
	while (*pbuf == SPACE || *pbuf == TABLESPACE) //找到空格了
	{
		pbuf++;
		nBoffset++;
		if (nBoffset >= nEndLen-1)
		{
			LogMp(4,__FILE__,__LINE__,nameattr.c_str(),nameattr.length(),"全为空格");
			return NULL;
		}
	}



	nEoffset = nBoffset;
	for (int i=nBoffset;i<nEndLen;i++)
	{
		pbuf++;
		nEoffset++;
		if (*pbuf == SPACE ||  *pbuf == TABLESPACE)
		{
			break;
		}
	}
	
	std::string nodename = nameattr.substr(nBoffset,nEoffset-nBoffset);
	nBoffset = nEoffset;

	// <xx 后
	CIXmlNode *tmpnode = parentnode->NewNode();
	if (tmpnode == NULL)
	{
		LogMp(4,__FILE__,__LINE__,nodename.c_str(),nodename.length(),"NewNode为NULL");
		return NULL;
	}
	tmpnode->SetParseClass(this);
	
	
	if (nodename.compare("?xml") == 0) //版本
	{
		//去掉 xx后的空格
		while (*pbuf == SPACE || *pbuf == TABLESPACE) //找到空格了
		{
			pbuf++;
			nBoffset++;
			if (nBoffset >= nEndLen-1)
			{
				LogMp(4,__FILE__,__LINE__,nameattr.c_str(),nameattr.length(),"全为空格");
				return NULL;
			}
		}
		if (nameattr.substr(nEndLen-1,1).compare("?") == 0) //版本
		{
			tmpnode->SetNodeValue(nameattr.substr(nBoffset,nEndLen-nBoffset-1),false);
			tmpnode->SetNodeType(NODETYPE_VER);
			parentnode->AddChildNode(tmpnode);
			return tmpnode;
		}
		else
		{
			delete tmpnode;
			tmpnode = NULL;
			LogMp(4,__FILE__,__LINE__,"",0,"版本节点[%s]非法",nameattr.c_str());
			return NULL;
		}
	}
	else if (nodename.compare("!DOCTYPE") == 0) //指定文档
	{
		//去掉 xx后的空格
		while (*pbuf == SPACE || *pbuf == TABLESPACE) //找到空格了
		{
			pbuf++;
			nBoffset++;
			if (nBoffset >= nEndLen-1)
			{
				LogMp(4,__FILE__,__LINE__,nameattr.c_str(),nameattr.length(),"全为空格");
				return NULL;
			}
		}
		tmpnode->SetNodeValue(nameattr.substr(nBoffset,nEndLen-nBoffset),false);
		tmpnode->SetNodeType(NODETYPE_DOCTYPE);
		parentnode->AddChildNode(tmpnode);
		return tmpnode;
	}
	else if (strncmp(nodename.c_str(),"!--",3) == 0) //注释
	{
		if (nameattr.substr(nEndLen-2,2).compare("--") == 0) //注释
		{
			if (nEndLen-3-2>0)
			{
				tmpnode->SetNodeValue(nameattr.substr(3,nEndLen-3-2),false);
			}
			tmpnode->SetNodeType(NODETYPE_REMARK);
			parentnode->AddChildNode(tmpnode);
			return tmpnode;
		}
		else
		{
			delete tmpnode;
			tmpnode = NULL;
			LogMp(4,__FILE__,__LINE__,"",0,"注释节点[%s]非法",nameattr.c_str());
			return NULL;
		}
	}
	
	else
	{
		tmpnode->SetNodeName(nodename,false);
		if (isEndNode)
		{
			tmpnode->SetNodeType(NODETYPE_ATTR);
		}
		else
		{	
			tmpnode->SetNodeType(NODETYPE_VALUE);
		}
		parentnode->AddChildNode(tmpnode);
		//解析属性
		if (ParseNodeAttribute(nameattr.substr(nBoffset,nEndLen-nBoffset),tmpnode)!=0)
		{
			LogMp(4,__FILE__,__LINE__,"",0,"属性[%s]配置出错",nameattr.substr(nBoffset,nEndLen-nBoffset).c_str());
		}
		return tmpnode;
	}
	
	return tmpnode;
}

bool CXmlParse::FindEndNode(std::string xmlbuf, int bOffset,std::string nodename, int &vend,int &nextnodebegin)
{
	int nodenameBoffset=0;
	int nodenameEndoffset=0;
	int nnodeNameLen=0;

	
	char *tmpbuf=NULL;
	
	bool isendnode;
	std::string sendflag="/";
	std::string snameattr;
	std::string snodenamefind;
	char *pxmlbuf=(char *)xmlbuf.c_str()+bOffset;
	int noffset=bOffset;
	int buflen=xmlbuf.length();
	int nNameNum=0;//节点计数
	
	while (*pxmlbuf != 0)
	{
		while (*pxmlbuf == SPACE || *pxmlbuf == TABLESPACE || *pxmlbuf == ENTER || *pxmlbuf== 13)
		{
			pxmlbuf++;
			noffset++;
			if (*pxmlbuf == 0 || noffset >= buflen)
			{
				return false;
			}
		}
		if ( *pxmlbuf == '<' )//找到<符号了
		{
			//往后移，如果后面是空格的话一直往后移
			do 
			{
				pxmlbuf++;
				noffset++;
				if (*pxmlbuf == 0 || noffset >= buflen)
				{
					return false;
				}
				
			} while (*pxmlbuf == SPACE || *pxmlbuf == TABLESPACE);

			nodenameBoffset = noffset;//节点名称开始位置
			nodenameEndoffset = noffset;
			//找到匹配的 '>'号 得到节点名称结束位置nodenameendoffset
			while (*pxmlbuf != '>')
			{
				pxmlbuf++;
				noffset++;
				if (*pxmlbuf == 0 || noffset >= buflen)
				{
					return false;
				}
				nodenameEndoffset++;
			}
			nnodeNameLen = nodenameEndoffset - nodenameBoffset;
			
			tmpbuf = pxmlbuf;
			tmpbuf--;
			isendnode=false;
			if (*tmpbuf=='/') //如果节点名紧接着是/，即<name/>的形式，要去除/
			{
				nnodeNameLen--;
				//移到>后面
				pxmlbuf++;
				noffset++;
				continue;
			}
			snameattr = xmlbuf.substr(nodenameBoffset,nnodeNameLen);//得到节点名称及属性
			if (nnodeNameLen == nodename.length()+1)
			{
				if (xmlbuf[nodenameBoffset] != '/') //</xx> 长度相等但不是/开始的，重新找
				{
					//移到>后面  
					pxmlbuf++;
					noffset++;
					continue;
				}
				if (snameattr.compare(sendflag+nodename) == 0) //找到一个结束标志
				{
					nNameNum--; //名称标志计数减少
					if (nNameNum < 0 ) //当名称标志小于0时，此时匹配，终于找到了
					{
						vend = nodenameBoffset-1;
						nextnodebegin = nodenameEndoffset+1;
						return true;
					}
				}
				//不是结束标志
				//移到>后面  
				pxmlbuf++;
				noffset++;
				continue;
			}
			//取得节点名称看是否和节点名称相同
			if (!GetNodeName(snameattr,snodenamefind))
			{
				return false;//无节点名称
			}
			if (snodenamefind.compare(nodename) == 0) //节点名称相同，名称标志计数增加
			{
				nNameNum++;
			}
			//移到>后面
			pxmlbuf++;
			noffset++;
			continue;
		}
		else //if ( *pbuf == '<' ) 
		{
			pxmlbuf++;
			noffset++;
			continue;
		}
	}
	return false;
}

int CXmlParse::ParseNodeAttribute(std::string sattr, CIXmlNode *addnode)
{
//	int rtn;
	char *pbuf=NULL;
	std::string attrvalue;
	std::string attrname;
	int attrlen=sattr.length();
	
	pbuf = (char *)sattr.c_str();
	//分析属性
	int aValueBegin=0;
	int aValueEnd =0;

	//去掉 name=""前面的空格或TAB
	while (*pbuf == SPACE || *pbuf == TABLESPACE) //找到空格了
	{
		pbuf++;
		aValueBegin++;
		if (aValueBegin>=attrlen)
		{
// 			sprintf(m_errMsg,"没有属性");
// 			LogMp(4,__FILE__,__LINE__,sattr.c_str(),sattr.length(),m_errMsg);
			return 0;
		}
	}
	int attrnamelen=0;
	aValueEnd = aValueBegin;
	while (aValueEnd<attrlen)
	{
		if (*pbuf == '=') //遇到等号了
		{
			attrnamelen =  aValueEnd - aValueBegin;
			if (attrnamelen<1) 
			{
				//遇到非法的属性了 只有=或者大于规定的长度，要去掉后面的" "里的东西
				//还是直接退出吧，省得再去解析
				sprintf(m_errMsg,"遇到非法的属性了 只有=或者大于规定的长度");
				LogMp(4,__FILE__,__LINE__,sattr.c_str(),sattr.length(),m_errMsg);
				return -1;
			}
			else
			{
				attrname = sattr.substr(aValueBegin,attrnamelen);
				aValueEnd++;
				pbuf++;
				if (aValueEnd>=attrlen)
				{
					addnode->AddAttribute(attrname,false,"",false);
					sprintf(m_errMsg,"遇到非法的属性了 只有名称没有=");
					LogMp(4,__FILE__,__LINE__,sattr.c_str(),sattr.length(),m_errMsg);
					return -1;
				}
				if (*pbuf != '"') //不是双引号，属性非法
				{
					//还是直接退出吧，省得再去解析
					//addnode->m_attribute.SetAttribute(attrname,false,"",false);
					sprintf(m_errMsg,"遇到非法的属性了 名称后面不是=");
					LogMp(4,__FILE__,__LINE__,sattr.c_str(),sattr.length(),m_errMsg);
					return -1;
				}
				aValueEnd++;
				pbuf++;
				if (aValueEnd>=attrlen)
				{
					addnode->AddAttribute(attrname,false,"",false);
					sprintf(m_errMsg,"遇到非法的属性了 =后无内容");
					LogMp(4,__FILE__,__LINE__,sattr.c_str(),sattr.length(),m_errMsg);
					return -1;
				}

				aValueBegin = aValueEnd;
				while (*pbuf != '"')
				{
					aValueEnd++;
					pbuf++;
					if (aValueEnd>=attrlen)
					{
						addnode->AddAttribute(attrname,false,"",false);
						sprintf(m_errMsg,"遇到非法的属性了 =\"后无内容");
						LogMp(4,__FILE__,__LINE__,sattr.c_str(),sattr.length(),m_errMsg);
						return -1;
					}
				}
				int attrvaluelen =0;
				attrvaluelen = aValueEnd - aValueBegin;
				if (attrvaluelen <1) //属性为空
				{
					addnode->AddAttribute(attrname,false,"",false);
				}
				else
				{
					attrvalue = sattr.substr(aValueBegin,attrvaluelen);
					addnode->AddAttribute(attrname,false,attrvalue,false);
				}
				aValueEnd++;
				pbuf++;
				if (aValueEnd>=attrlen) // 已到结束
				{
					return 0;
				}
				if (*pbuf != SPACE) // ""后不是空格，属性结束
				{
					return 0;
				}
				//解析下一个属性
				if (ParseNodeAttribute(sattr.substr(aValueEnd,attrlen-aValueEnd),addnode)!=0)
				{
					LogMp(4,__FILE__,__LINE__,"",0,"属性[%s]配置出错",sattr.substr(aValueEnd,attrlen-aValueEnd).c_str());
					return -1;
				}
				return 0;

			}
			
		}
		aValueEnd++;
		pbuf++;
	}
	return 0;
}

bool CXmlParse::GetNodeName(std::string nameattr, std::string &nodename)
{
//	int rtn;
	char *pbuf=NULL;
	int nBoffset=0;
	int nEoffset=0;
	int nEndLen=0;
	nEndLen = nameattr.length();
	
	pbuf = (char *)nameattr.c_str();
	while (*pbuf == SPACE || *pbuf == TABLESPACE) //找到空格了
	{
		pbuf++;
		nBoffset++;
		if (nBoffset>=nEndLen)
		{
			LogMp(4,__FILE__,__LINE__,nameattr.c_str(),nameattr.length(),"全为空格");
			return false;
		}
	}
	nEoffset = nBoffset;
	for (int i=nBoffset;i<nEndLen;i++)
	{
		pbuf++;
		nEoffset++;
		if (*pbuf == SPACE ||  *pbuf == TABLESPACE)
		{
			break;
		}
	}
	nodename = nameattr.substr(nBoffset,nEoffset-nBoffset);
	return true;
}

bool CXmlParse::ParseXmlDom(std::string xmlbuf, CIXmlNode *parentNode)
{
	m_nXmlBufLen = xmlbuf.length();
	m_sXmlBuf=(char *)xmlbuf.c_str();
	int beginoffset=0;
	bool bres= ParseXmlDom2(beginoffset,m_nXmlBufLen,parentNode);
	m_sXmlBuf = NULL;
	return bres;
}

bool CXmlParse::ParseXmlSax(std::string xmlbuf, CIXmlNode *parentNode, int layer)
{
	int bOffset=0;
	int eOffset=0;
	int nodenameBoffset=0;
	int nodenameEndoffset=0;
	int nnodeNameLen=0;
	char  *pbuf=NULL;
	bOffset = 0;
	eOffset = xmlbuf.length();
	pbuf = (char *)xmlbuf.c_str();//指向xml的缓冲
	char *tmpbuf=NULL; //指向xml的缓冲
	std::string snameattr;
	CIXmlNode *pChildNode=NULL;
	bool isendnode=false;

//	LogMp(4,__FILE__,__LINE__,"",0,"解析[%s]层次%d",xmlbuf.c_str(),layer);

	while (*pbuf != 0)
	{
		while (*pbuf == SPACE || *pbuf == TABLESPACE || *pbuf == ENTER || *pbuf== 13)
		{
			pbuf++;
			bOffset++;
			if (*pbuf == 0 || bOffset >= eOffset-1)
			{
				parentNode->SetNodeType(NODETYPE_VALUE);
				return true;
			}
		}
		if ( *pbuf == '<' )//找到<符号了
		{
			//往后移，如果后面是空格的话一直往后移
			do 
			{
				pbuf++;
				bOffset++;
				if (*pbuf == 0 || bOffset >= eOffset-1)
				{
					LogMp(4,__FILE__,__LINE__,"",0,"节点[%s]的值中有<但无>",parentNode->GetNodeName(false).c_str());
					return false;
				}
				
			} while (*pbuf == SPACE || *pbuf == TABLESPACE);
				
			
			nodenameBoffset = bOffset;//节点名称开始位置
			nodenameEndoffset = bOffset;

			bool isfind=false;
			if (strncmp(pbuf,"!--",3) == 0)
			{
				pbuf+=3;
				bOffset+=3;
				nodenameEndoffset+=3;
				while (!isfind)
				{
					//找到匹配的 '>'号 得到节点名称结束位置nodenameendoffset
					while (*pbuf != '>')
					{
						pbuf++;
						bOffset++;
						if (*pbuf == 0 || bOffset >= eOffset-1)
						{
							LogMp(4,__FILE__,__LINE__,"",0,"节点[%s]的值中有<但无>",parentNode->GetNodeName(false).c_str());
							return false;
						}
						nodenameEndoffset++;
					}
					tmpbuf = pbuf;
					tmpbuf--;
					tmpbuf--;
					if (strncmp(tmpbuf,"--",2)==0)
					{
						isfind = true;
						break;
					}
					pbuf++;
					bOffset++;
					nodenameEndoffset++;
					if (*pbuf == 0 || bOffset >= eOffset-1)
					{
						LogMp(4,__FILE__,__LINE__,"",0,"节点[%s]的值中有<但无>",parentNode->GetNodeName(false).c_str());
						return false;
					}
				} // while (!isfind)
			}
			else
			{
			
				//找到匹配的 '>'号 得到节点名称结束位置nodenameendoffset
				while (*pbuf != '>')
				{
					pbuf++;
					bOffset++;
					if (*pbuf == 0 || bOffset >= eOffset-1)
					{
						LogMp(4,__FILE__,__LINE__,"",0,"节点[%s]的值中有<但无>",parentNode->GetNodeName(false).c_str());
						return false;
					}
					nodenameEndoffset++;
				}
			}
			isendnode=false;
			nnodeNameLen = nodenameEndoffset - nodenameBoffset;
			
			if (!isfind)
			{
				tmpbuf = pbuf;
				tmpbuf--;
				if (*tmpbuf=='/') //如果节点名紧接着是/，即<name/>的形式，要去除/
				{
					nnodeNameLen--;
					isendnode = true;
				}
			}
			
			snameattr = xmlbuf.substr(nodenameBoffset,nnodeNameLen);//得到节点名称及属性
			//分析节点名称及属性并加到parentNode的子节点中去
			pChildNode = ParseNodeNameAttr(parentNode,snameattr,isendnode);
			if (pChildNode == NULL)
			{
				return false;
			}
			if (pChildNode->GetNodeType() != NODETYPE_VALUE) //<xx/>及版本注释的情况
			{
				//移到>后面
				pbuf++;
				bOffset++;
				continue;
			}
			//移到>后面
			pbuf++;
			bOffset++;
			if (*pbuf == 0 || bOffset >= eOffset-1 )
			{
				LogMp(4,__FILE__,__LINE__,"",0,"节点%s>后无数据，即只有<>没有</>",pChildNode->GetNodeName(false).c_str());
				return false;
			}
			//<xx></xx>的情况，查找匹配的/xx
			int nNextNodeBegin=0;//节点值的开始位置
			int nValueEnd=0;//节点值的结束位置(中间可能包含子节点)
			
//			LogMp(4,__FILE__,__LINE__,"",0,"%s从[%d,%d]开始查找匹配的[/%s]",xmlbuf.c_str(),bOffset,eOffset,pChildNode->GetNodeName(false).c_str());

			if (!FindEndNode(xmlbuf,bOffset,pChildNode->GetNodeName(false),nValueEnd,nNextNodeBegin))
			{
				LogMp(4,__FILE__,__LINE__,"",0,"无</%s>",pChildNode->GetNodeName(false).c_str());
				return false;
			}
//			LogMp(4,__FILE__,__LINE__,"",0,"%s 查找匹配的[/%s]结束[%d,%d,%d]",xmlbuf.c_str(),pChildNode->GetNodeName(false).c_str(),bOffset,nValueEnd,nNextNodeBegin);

			//设置节点值
			pChildNode->SetNodeValue(xmlbuf.substr(bOffset,nValueEnd-bOffset),false);
			//解析下层
 			if ( layer == 0 )
 			{
 				pChildNode->ParseChildNode(layer);//解析所有子节点
 			}
 			else if (layer>1)
 			{
 				pChildNode->ParseChildNode(layer-1); //解析下一层
 			}
			//pbuf及bOffset移到下一节点开始
			pbuf+=(nNextNodeBegin-bOffset);
			bOffset=nNextNodeBegin;
//			LogMp(4,__FILE__,__LINE__,"",0,"%s从[%d,%d]开始下面的解析",xmlbuf.c_str(),bOffset,eOffset);
			if (*pbuf == 0 || bOffset >= eOffset)
			{
				return true;
			}
			
		}
		else //if ( *pbuf == '<' ) //说明是节点的值，不用再解析了
		{
			//<xx><yy>aaa</yy>ddd</xx>  
			//去掉ddd，不用再管它了
			return true;
		}
	}
	return true;
}

bool CXmlParse::ParseXmlDom(int beginoffset, int endoffset, CIXmlNode *parentNode)
{
	int bOffset=0;
	int eOffset=0;
	int nodenameBoffset=0;
	int nodenameEndoffset=0;
	int nnodeNameLen=0;
	char  *pbuf=NULL;

	bOffset = beginoffset;
	eOffset = endoffset;
	pbuf =  m_sXmlBuf;//指向xml的缓冲
	pbuf += bOffset;//指向xml的缓冲

	char *tmpbuf=NULL; //指向xml的缓冲

	char *snameattr=NULL;

	CIXmlNode *pChildNode=NULL;
	bool isendnode=false;
	bool bres=false;

//	LogMp(4,__FILE__,__LINE__,"",0,"解析[%s]层次%d",xmlbuf.c_str(),layer);
	//设置此节点已解析
	parentNode->SetParseChilded();
	while (*pbuf != 0)
	{
		while (*pbuf == SPACE || *pbuf == TABLESPACE || *pbuf == ENTER || *pbuf== 13)
		{
			pbuf++;
			bOffset++;
			if (*pbuf == 0 || bOffset >= endoffset-1)
			{
				parentNode->SetNodeType(NODETYPE_VALUE);
				return true;
			}
		}
		if ( *pbuf == '<' )//找到<符号了
		{
			//往后移，如果后面是空格的话一直往后移
			do 
			{
				pbuf++;
				bOffset++;
				if (*pbuf == 0 || bOffset >= endoffset-1)
				{
					LogMp(4,__FILE__,__LINE__,"",0,"节点[%s]的值中有<但无>",parentNode->GetNodeName(false).c_str());
					return false;
				}
			} while (*pbuf == SPACE || *pbuf == TABLESPACE);

			nodenameBoffset = bOffset;//节点名称开始位置
			nodenameEndoffset = bOffset;//节点名称结束位置

			bool isfind=false;
			if (strncmp(pbuf,"!--",3) == 0)
			{
				pbuf+=3;
				bOffset+=3;
				nodenameEndoffset+=3;
				
				while (!isfind)
				{
					//找到匹配的 '>'号 得到节点名称结束位置nodenameendoffset
					while (*pbuf != '>')
					{
						pbuf++;
						bOffset++;
						if (*pbuf == 0 || bOffset >= endoffset-1)
						{
							LogMp(4,__FILE__,__LINE__,"",0,"节点[%s]的值中有<但无>",parentNode->GetNodeName(false).c_str());
							return false;
						}
						nodenameEndoffset++;
					}
					tmpbuf = pbuf;
					tmpbuf--;
					tmpbuf--;
					if (strncmp(tmpbuf,"--",2)==0)
					{
						isfind = true;
						break;
					}
					pbuf++;
					bOffset++;
					nodenameEndoffset++;
					if (*pbuf == 0 || bOffset >= endoffset-1)
					{
						LogMp(4,__FILE__,__LINE__,"",0,"节点[%s]的值中有<但无>",parentNode->GetNodeName(false).c_str());
						return false;
					}
				} // while (!isfind)
			}
			else
			{
				//找到匹配的 '>'号 得到节点名称结束位置nodenameendoffset
				while (*pbuf != '>')
				{
					pbuf++;
					bOffset++;
					if (*pbuf == 0 || bOffset >= endoffset-1)
					{
						LogMp(4,__FILE__,__LINE__,"",0,"节点[%s]的值中有<但无>",parentNode->GetNodeName(false).c_str());
						return false;
					}
					nodenameEndoffset++;
				}
			}
			
			//节点名称长度
			nnodeNameLen = nodenameEndoffset - nodenameBoffset;
			if (!isfind)
			{
				tmpbuf = pbuf;
				tmpbuf--;
				isendnode=false;
				if (*tmpbuf=='/') //如果节点名紧接着是/，即<name/>的形式，要去除/
				{
					nnodeNameLen--;
					isendnode = true;
				}
			}
			
			snameattr = (char *)malloc(nnodeNameLen+1);
			//memset(snameattr,0,nnodeNameLen+1);
			memcpy(snameattr,m_sXmlBuf+nodenameBoffset,nnodeNameLen);//得到节点名称及属性
			snameattr[nnodeNameLen]=0;
			//分析节点名称及属性并加到parentNode的子节点中去
			pChildNode = ParseNodeNameAttr(parentNode,snameattr,isendnode);
			if (pChildNode == NULL)
			{
				free(snameattr);
				snameattr = NULL;
				return false;
			}
			free(snameattr);
			snameattr = NULL;
			if (pChildNode->GetNodeType() != NODETYPE_VALUE) //<xx/>及版本注释的情况
			{
				//移到>后面
				pbuf++;
				bOffset++;
				continue;//while (*pbuf != 0)
			}
			//移到>后面
			pbuf++;
			bOffset++;
			if (*pbuf == 0 || bOffset >= endoffset-1 )
			{
				LogMp(4,__FILE__,__LINE__,"",0,"节点%s>后无数据，即只有<>没有</>",pChildNode->GetNodeName(false).c_str());
				return false;
			}
			//<xx></xx>的情况，查找匹配的/xx
			int nNextNodeBegin=0;//节点值的开始位置
			int nValueEnd=0;//节点值的结束位置(中间可能包含子节点)
			
//			LogMp(4,__FILE__,__LINE__,"",0,"%s从[%d,%d]开始查找匹配的[/%s]",xmlbuf.c_str(),bOffset,eOffset,pChildNode->GetNodeName(false).c_str());
			
			if (!FindEndNodeDom(bOffset,endoffset,pChildNode->GetNodeName(false),nValueEnd,nNextNodeBegin))
			{
				LogMp(4,__FILE__,__LINE__,"",0,"无</%s>",pChildNode->GetNodeName(false).c_str());
				return false;
			}
//			LogMp(4,__FILE__,__LINE__,"",0,"%s 查找匹配的[/%s]结束[%d,%d,%d]",xmlbuf.c_str(),pChildNode->GetNodeName(false).c_str(),bOffset,nValueEnd,nNextNodeBegin);

			//解析节点里面的
			bres = ParseXmlDom(bOffset,nValueEnd+1,pChildNode);
			if (!bres)
			{
				return false;
			}
			//pbuf及bOffset移到下一节点开始
			pbuf = m_sXmlBuf+nNextNodeBegin;
			bOffset=nNextNodeBegin;

//			LogMp(4,__FILE__,__LINE__,"",0,"%s从[%d,%d]开始下面的解析",xmlbuf.c_str(),bOffset,eOffset);
			if (*pbuf == 0 || bOffset >= endoffset-1)
			{
				return true;
			}
			
		}
		else //else if ( *pbuf == '<' ) //说明是节点的值，不用再解析了
		{
			if (parentNode->GetParentNode() == NULL)
			{
				parentNode->SetNodeValue("",false);
				return true;
			}
			eOffset = bOffset;
			do 
			{
				pbuf++;
				eOffset++;
				if (*pbuf == 0 || bOffset >= endoffset-1)
				{
					break;
				}
				
			} while (*pbuf != '<');
			if (eOffset - bOffset<=0)
			{
				parentNode->SetNodeType(NODETYPE_VALUE);
				parentNode->SetNodeValue("",false);
			}
			else
			{
				snameattr = (char *)malloc(eOffset-bOffset+1);
				memset(snameattr,0,eOffset-bOffset+1);
				memcpy(snameattr,m_sXmlBuf+bOffset,eOffset-bOffset);//得到节点名称及属性
				parentNode->SetNodeType(NODETYPE_VALUE);
				parentNode->SetNodeValue(snameattr,false);
				free(snameattr);
				snameattr = NULL;
			}
			return true;
		}

	}
	return true;
}

CIXmlNode * CXmlParse::ParseNodeNameAttrDom(CIXmlNode *parentnode, char *nameattr,bool isEndNode)
{
//	int rtn;
	char *pbuf=NULL;
	int nBoffset=0;
	int nEoffset=0;
	int nEndLen=0;
	nEndLen = strlen(nameattr);
	
	pbuf = (char *)nameattr;
	while (*pbuf == SPACE || *pbuf == TABLESPACE) //找到空格了
	{
		pbuf++;
		nBoffset++;
		if (nBoffset >= nEndLen-1)
		{
			LogMp(4,__FILE__,__LINE__,nameattr,nEndLen,"全为空格");
			return NULL;
		}
	}



	nEoffset = nBoffset;
	for (int i=nBoffset;i<nEndLen;i++)
	{
		pbuf++;
		nEoffset++;
		if (*pbuf == SPACE ||  *pbuf == TABLESPACE)
		{
			break;
		}
	}
	nameattr[nEoffset]=0;
	std::string nodename = nameattr+nBoffset;
	nBoffset = nEoffset;

	// <xx 后
	CIXmlNode *tmpnode = parentnode->NewNode();
	if (tmpnode == NULL)
	{
		LogMp(4,__FILE__,__LINE__,nodename.c_str(),nodename.length(),"NewNode为NULL");
		return NULL;
	}
	tmpnode->SetParseClass(this);
	
	
	if (nodename.compare("?xml") == 0) //版本
	{
		//去掉 xx后的空格
		while (*pbuf == SPACE || *pbuf == TABLESPACE) //找到空格了
		{
			pbuf++;
			nBoffset++;
			if (nBoffset >= nEndLen-1)
			{
				LogMp(4,__FILE__,__LINE__,nameattr,nEndLen,"全为空格");
				return NULL;
			}
		}
		if (nameattr[nEndLen-1] == '?') //版本
		{
			nameattr[nEndLen-1]=0;
			tmpnode->SetNodeValue(nameattr+nBoffset,false);
			tmpnode->SetNodeType(NODETYPE_VER);
			parentnode->AddChildNode(tmpnode);
		}
		else
		{
			delete tmpnode;
			tmpnode = NULL;
			LogMp(4,__FILE__,__LINE__,"",0,"版本节点[%s]非法",nameattr);
			return NULL;
		}
	}
	else if (nodename.compare("!DOCTYPE") == 0) //指定文档
	{
		//去掉 xx后的空格
		while (*pbuf == SPACE || *pbuf == TABLESPACE) //找到空格了
		{
			pbuf++;
			nBoffset++;
			if (nBoffset >= nEndLen-1)
			{
				LogMp(4,__FILE__,__LINE__,nameattr,nEndLen,"全为空格");
				return NULL;
			}
		}
		tmpnode->SetNodeValue(nameattr+nBoffset,false);
		tmpnode->SetNodeType(NODETYPE_DOCTYPE);
		parentnode->AddChildNode(tmpnode);
	}
	else if (strncmp(nodename.c_str(),"!--",3) == 0) //注释
	{
		if (strncmp(nameattr+(nEndLen-2),"--",2) == 0) //注释
		{
			if (nEndLen-3-2>0)
			{
				nameattr[nEndLen-2]=0;
				tmpnode->SetNodeValue(nameattr+3,false);
			}
			tmpnode->SetNodeType(NODETYPE_REMARK);
			parentnode->AddChildNode(tmpnode);
		}
		else
		{
			delete tmpnode;
			tmpnode = NULL;
			LogMp(4,__FILE__,__LINE__,"",0,"注释节点[%s]非法",nameattr);
			return NULL;
		}
	}
	
	else
	{
		tmpnode->SetNodeName(nodename,false);
		if (isEndNode)
		{
			tmpnode->SetNodeType(NODETYPE_ATTR);
		}
		else
		{	
			tmpnode->SetNodeType(NODETYPE_VALUE);
		}
		parentnode->AddChildNode(tmpnode);
		//解析属性
		if (ParseNodeAttribute(nameattr+nBoffset,tmpnode)!=0)
		{
			LogMp(4,__FILE__,__LINE__,nameattr,nEndLen,"属性配置出错");
		}
	}
	
	return tmpnode;
}

bool CXmlParse::FindEndNodeDom(int bOffset,int eOffset,std::string nodename, int &vend,int &nextnodebegin)
{
	int nodenameBoffset=0;
	int nodenameEndoffset=0;
	int nnodeNameLen=0;

	
	char *tmpbuf=NULL;
	
	bool isendnode;
	std::string sendflag="/";
	std::string snameattr;
	std::string snodenamefind;
	char *pxmlbuf=(char *)m_sXmlBuf+bOffset;
	int noffset=bOffset;
	int buflen=eOffset;
	int nNameNum=0;//节点计数
	
	while (*pxmlbuf != 0)
	{
		while (*pxmlbuf == SPACE || *pxmlbuf == TABLESPACE || *pxmlbuf == ENTER || *pxmlbuf== 13)
		{
			pxmlbuf++;
			noffset++;
			if (*pxmlbuf == 0 || noffset >= buflen)
			{
				return false;
			}
		}
		if ( *pxmlbuf == '<' )//找到<符号了
		{
			//往后移，如果后面是空格的话一直往后移
			do 
			{
				pxmlbuf++;
				noffset++;
				if (*pxmlbuf == 0 || noffset >= buflen)
				{
					return false;
				}
				
			} while (*pxmlbuf == SPACE || *pxmlbuf == TABLESPACE);

			nodenameBoffset = noffset;//节点名称开始位置
			nodenameEndoffset = noffset;
			//找到匹配的 '>'号 得到节点名称结束位置nodenameendoffset
			while (*pxmlbuf != '>')
			{
				pxmlbuf++;
				noffset++;
				if (*pxmlbuf == 0 || noffset >= buflen)
				{
					return false;
				}
				nodenameEndoffset++;
			}
			nnodeNameLen = nodenameEndoffset - nodenameBoffset;
			
			tmpbuf = pxmlbuf;
			tmpbuf--;
			isendnode=false;
			if (*tmpbuf=='/') //如果节点名紧接着是/，即<name/>的形式，要去除/
			{
				nnodeNameLen--;
				//移到>后面
				pxmlbuf++;
				noffset++;
				continue;
			}
			if (nnodeNameLen<1)
			{
				return false;
			}
			char *tmpchar = (char *)malloc(nnodeNameLen+1);
			memcpy(tmpchar,m_sXmlBuf+nodenameBoffset,nnodeNameLen);
			tmpchar[nnodeNameLen]=0;
			snameattr = tmpchar;//得到节点名称及属性
			free(tmpchar);
			if (nnodeNameLen == nodename.length()+1)
			{
				if (m_sXmlBuf[nodenameBoffset] != '/') //</xx> 长度相等但不是/开始的，重新找
				{
					//移到>后面  
					pxmlbuf++;
					noffset++;
					continue;
				}
				if (snameattr.compare(sendflag+nodename) == 0) //找到一个结束标志
				{
					nNameNum--; //名称标志计数减少
					if (nNameNum < 0 ) //当名称标志小于0时，此时匹配，终于找到了
					{
						vend = nodenameBoffset-1;
						nextnodebegin = nodenameEndoffset+1;
						return true;
					}
				}
				//不是结束标志
				//移到>后面  
				pxmlbuf++;
				noffset++;
				continue;
			}
			//取得节点名称看是否和节点名称相同
			if (!GetNodeName(snameattr,snodenamefind))
			{
				return false;//无节点名称
			}
			if (snodenamefind.compare(nodename) == 0) //节点名称相同，名称标志计数增加
			{
				nNameNum++;
			}
			//移到>后面
			pxmlbuf++;
			noffset++;
			continue;
		}
		else //if ( *pbuf == '<' ) 
		{
			pxmlbuf++;
			noffset++;
			continue;
		}
	}
	return false;
}

bool CXmlParse::ParseXmlDom2(int &beginoffset, int endoffset, CIXmlNode *parentNode)
{
	int bOffset=0;
	int eOffset=0;
	int nodenameBoffset=0;
	int nodenameEndoffset=0;
	int nnodeNameLen=0;
	char  *pbuf=NULL;

	bOffset = beginoffset;
	eOffset = endoffset;
	pbuf =  m_sXmlBuf;//指向xml的缓冲
	pbuf += bOffset;//指向xml的缓冲

	char *tmpbuf=NULL; //指向xml的缓冲

	char *snameattr=NULL;

	CIXmlNode *pChildNode=NULL;
	bool isendnode=false;
	bool bres=false;

//	LogMp(4,__FILE__,__LINE__,"",0,"解析[%s]层次%d",xmlbuf.c_str(),layer);
	//设置此节点已解析
	parentNode->SetParseChilded();
	while (*pbuf != 0)
	{
		while (*pbuf == SPACE || *pbuf == TABLESPACE || *pbuf == ENTER || *pbuf== 13)
		{
			pbuf++;
			bOffset++;
			if (*pbuf == 0 || bOffset >= endoffset-1)
			{
				parentNode->SetNodeType(NODETYPE_VALUE);
				return true;
			}
		}
		if ( *pbuf == '<' )//找到<符号了
		{
			//往后移，如果后面是空格的话一直往后移
			do 
			{
				pbuf++;
				bOffset++;
				if (*pbuf == 0 || bOffset >= endoffset-1)
				{
					LogMp(4,__FILE__,__LINE__,"",0,"节点[%s]的值中有<但无>",parentNode->GetNodeName(false).c_str());
					return false;
				}
			} while (*pbuf == SPACE || *pbuf == TABLESPACE);

			if  (*pbuf == '/')
			{
				eOffset=bOffset;
				do 
				{
					pbuf++;
					eOffset++;
					if (*pbuf == 0 || bOffset >= endoffset-1)
					{
						break;
					}
					
				} while (*pbuf != '>');
				if  (*pbuf == '>')
				{
					snameattr = (char *)malloc(eOffset-bOffset);
					memset(snameattr,0,eOffset-bOffset);
					memcpy(snameattr,m_sXmlBuf+bOffset+1,eOffset-bOffset-1);//得到节点名称
					if (strncmp(parentNode->GetNodeName(false).c_str(),snameattr,eOffset-bOffset-1) !=0)
					{
						LogMp(4,__FILE__,__LINE__,"",0,"节点<%s>后有数据，但没有匹配的</%s>,有</%s>",parentNode->GetNodeName(false).c_str(),parentNode->GetNodeName(false).c_str(),snameattr);
						free(snameattr);
						snameattr = NULL;
						return false;
					}
					free(snameattr);
					snameattr = NULL;
					beginoffset = eOffset+1;
					return true;
				}
				LogMp(4,__FILE__,__LINE__,"",0,"节点<%s>后有数据，但没有匹配的</%s>",parentNode->GetNodeName(false).c_str(),parentNode->GetNodeName(false).c_str());
				return false;
			}
			


			nodenameBoffset = bOffset;//节点名称开始位置
			nodenameEndoffset = bOffset;//节点名称结束位置

			bool isfind=false;
			if (strncmp(pbuf,"!--",3) == 0)
			{
				pbuf+=3;
				bOffset+=3;
				nodenameEndoffset+=3;
				
				while (!isfind)
				{
					//找到匹配的 '>'号 得到节点名称结束位置nodenameendoffset
					while (*pbuf != '>')
					{
						pbuf++;
						bOffset++;
						if (*pbuf == 0 || bOffset >= endoffset-1)
						{
							LogMp(4,__FILE__,__LINE__,"",0,"节点[%s]的值中有<但无>",parentNode->GetNodeName(false).c_str());
							return false;
						}
						nodenameEndoffset++;
					}
					tmpbuf = pbuf;
					tmpbuf--;
					tmpbuf--;
					if (strncmp(tmpbuf,"--",2)==0)
					{
						isfind = true;
						break;
					}
					pbuf++;
					bOffset++;
					nodenameEndoffset++;
					if (*pbuf == 0 || bOffset >= endoffset-1)
					{
						LogMp(4,__FILE__,__LINE__,"",0,"节点[%s]的值中有<但无>",parentNode->GetNodeName(false).c_str());
						return false;
					}
				} // while (!isfind)
			}
			else
			{
				//找到匹配的 '>'号 得到节点名称结束位置nodenameendoffset
				while (*pbuf != '>')
				{
					pbuf++;
					bOffset++;
					if (*pbuf == 0 || bOffset >= endoffset-1)
					{
						LogMp(4,__FILE__,__LINE__,"",0,"节点[%s]的值中有<但无>",parentNode->GetNodeName(false).c_str());
						return false;
					}
					nodenameEndoffset++;
				}
			}
			
			//节点名称长度
			nnodeNameLen = nodenameEndoffset - nodenameBoffset;
			if (!isfind)
			{
				tmpbuf = pbuf;
				tmpbuf--;
				isendnode=false;
				if (*tmpbuf=='/') //如果节点名紧接着是/，即<name/>的形式，要去除/
				{
					nnodeNameLen--;
					isendnode = true;
				}
			}
			
			snameattr = (char *)malloc(nnodeNameLen+1);
			//memset(snameattr,0,nnodeNameLen+1);
			memcpy(snameattr,m_sXmlBuf+nodenameBoffset,nnodeNameLen);//得到节点名称及属性
			snameattr[nnodeNameLen]=0;
			//分析节点名称及属性并加到parentNode的子节点中去
			pChildNode = ParseNodeNameAttr(parentNode,snameattr,isendnode);
			if (pChildNode == NULL)
			{
				free(snameattr);
				snameattr = NULL;
				return false;
			}
			free(snameattr);
			snameattr = NULL;
			if (pChildNode->GetNodeType() != NODETYPE_VALUE) //<xx/>及版本注释的情况
			{
				//移到>后面
				pbuf++;
				bOffset++;
				continue;//while (*pbuf != 0)
			}
			//移到>后面
			pbuf++;
			bOffset++;
			if (*pbuf == 0 || bOffset >= endoffset-1 )
			{
				LogMp(4,__FILE__,__LINE__,"",0,"节点%s>后无数据，即只有<>没有</>",pChildNode->GetNodeName(false).c_str());
				return false;
			}

			//解析>后面的
			int offsettmp= bOffset;
			bres = ParseXmlDom2(bOffset,endoffset,pChildNode);
			if (!bres)
			{
				return false;
			}
			//再次解析临近节点
			pbuf+=(bOffset-offsettmp);
			continue;
			
		}
		else //else if ( *pbuf == '<' ) //说明是节点的值，不用再解析了
		{
			if (parentNode->GetParentNode() == NULL)
			{
				parentNode->SetNodeValue("",false);
				return true;
			}
			eOffset = bOffset;
			do 
			{
				pbuf++;
				eOffset++;
				if (*pbuf == 0 || bOffset >= endoffset-1)
				{
					break;
				}
				
			} while (*pbuf != '<');
			if (eOffset - bOffset<=0)
			{
				parentNode->SetNodeType(NODETYPE_VALUE);
				parentNode->SetNodeValue("",false);
			}
			else
			{
				snameattr = (char *)malloc(eOffset-bOffset+1);
				memset(snameattr,0,eOffset-bOffset+1);
				memcpy(snameattr,m_sXmlBuf+bOffset,eOffset-bOffset);//得到节点值
				parentNode->SetNodeType(NODETYPE_VALUE);
				parentNode->SetNodeValue(snameattr,false);
				free(snameattr);
				snameattr = NULL;
			}
			pbuf++;
			eOffset++;
			if  (*pbuf != '/')
			{
				LogMp(4,__FILE__,__LINE__,"",0,"节点<%s>后有数据，有<但没有/结束",parentNode->GetNodeName(false).c_str());
				return false;
			}
			bOffset = eOffset;	
			do 
			{
				pbuf++;
				eOffset++;
				if (*pbuf == 0 || bOffset >= endoffset-1)
				{
					break;
				}
				
			} while (*pbuf != '>');
			if  (*pbuf == '>')
			{
				snameattr = (char *)malloc(eOffset-bOffset);
				memset(snameattr,0,eOffset-bOffset);
				memcpy(snameattr,m_sXmlBuf+bOffset+1,eOffset-bOffset-1);//得到节点名称
				if (strncmp(parentNode->GetNodeName(false).c_str(),snameattr,eOffset-bOffset-1) !=0)
				{
					LogMp(4,__FILE__,__LINE__,"",0,"节点<%s>后有数据，但没有匹配的</%s>,有</%s>",parentNode->GetNodeName(false).c_str(),parentNode->GetNodeName(false).c_str(),snameattr);
					free(snameattr);
					snameattr = NULL;
					return false;
				}
				free(snameattr);
				snameattr = NULL;
				beginoffset = eOffset+1;
				return true;
			}
			return false;
		}

	}
	return true;
}