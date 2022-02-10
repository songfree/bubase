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
	m_nLogLevel = 5;///��־���𣬴��ڴ˼������־����д����־�ļ�0���ش���1��Ҫ����2һ�����3����4��ʾ����5������Ϣ
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
	//ȥ��·��
	GetName(fname);
	char buffer[60];
	memset(buffer,0,sizeof(buffer));
	//0���ش���1��Ҫ����2һ�����3����4��ʾ����5������Ϣ
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
	while (*pbuf == SPACE || *pbuf == TABLESPACE) //�ҵ��ո���
	{
		pbuf++;
		nBoffset++;
		if (nBoffset >= nEndLen-1)
		{
			LogMp(4,__FILE__,__LINE__,nameattr.c_str(),nameattr.length(),"ȫΪ�ո�");
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

	// <xx ��
	CIXmlNode *tmpnode = parentnode->NewNode();
	if (tmpnode == NULL)
	{
		LogMp(4,__FILE__,__LINE__,nodename.c_str(),nodename.length(),"NewNodeΪNULL");
		return NULL;
	}
	tmpnode->SetParseClass(this);
	
	
	if (nodename.compare("?xml") == 0) //�汾
	{
		//ȥ�� xx��Ŀո�
		while (*pbuf == SPACE || *pbuf == TABLESPACE) //�ҵ��ո���
		{
			pbuf++;
			nBoffset++;
			if (nBoffset >= nEndLen-1)
			{
				LogMp(4,__FILE__,__LINE__,nameattr.c_str(),nameattr.length(),"ȫΪ�ո�");
				return NULL;
			}
		}
		if (nameattr.substr(nEndLen-1,1).compare("?") == 0) //�汾
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
			LogMp(4,__FILE__,__LINE__,"",0,"�汾�ڵ�[%s]�Ƿ�",nameattr.c_str());
			return NULL;
		}
	}
	else if (nodename.compare("!DOCTYPE") == 0) //ָ���ĵ�
	{
		//ȥ�� xx��Ŀո�
		while (*pbuf == SPACE || *pbuf == TABLESPACE) //�ҵ��ո���
		{
			pbuf++;
			nBoffset++;
			if (nBoffset >= nEndLen-1)
			{
				LogMp(4,__FILE__,__LINE__,nameattr.c_str(),nameattr.length(),"ȫΪ�ո�");
				return NULL;
			}
		}
		tmpnode->SetNodeValue(nameattr.substr(nBoffset,nEndLen-nBoffset),false);
		tmpnode->SetNodeType(NODETYPE_DOCTYPE);
		parentnode->AddChildNode(tmpnode);
		return tmpnode;
	}
	else if (strncmp(nodename.c_str(),"!--",3) == 0) //ע��
	{
		if (nameattr.substr(nEndLen-2,2).compare("--") == 0) //ע��
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
			LogMp(4,__FILE__,__LINE__,"",0,"ע�ͽڵ�[%s]�Ƿ�",nameattr.c_str());
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
		//��������
		if (ParseNodeAttribute(nameattr.substr(nBoffset,nEndLen-nBoffset),tmpnode)!=0)
		{
			LogMp(4,__FILE__,__LINE__,"",0,"����[%s]���ó���",nameattr.substr(nBoffset,nEndLen-nBoffset).c_str());
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
	int nNameNum=0;//�ڵ����
	
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
		if ( *pxmlbuf == '<' )//�ҵ�<������
		{
			//�����ƣ���������ǿո�Ļ�һֱ������
			do 
			{
				pxmlbuf++;
				noffset++;
				if (*pxmlbuf == 0 || noffset >= buflen)
				{
					return false;
				}
				
			} while (*pxmlbuf == SPACE || *pxmlbuf == TABLESPACE);

			nodenameBoffset = noffset;//�ڵ����ƿ�ʼλ��
			nodenameEndoffset = noffset;
			//�ҵ�ƥ��� '>'�� �õ��ڵ����ƽ���λ��nodenameendoffset
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
			if (*tmpbuf=='/') //����ڵ�����������/����<name/>����ʽ��Ҫȥ��/
			{
				nnodeNameLen--;
				//�Ƶ�>����
				pxmlbuf++;
				noffset++;
				continue;
			}
			snameattr = xmlbuf.substr(nodenameBoffset,nnodeNameLen);//�õ��ڵ����Ƽ�����
			if (nnodeNameLen == nodename.length()+1)
			{
				if (xmlbuf[nodenameBoffset] != '/') //</xx> ������ȵ�����/��ʼ�ģ�������
				{
					//�Ƶ�>����  
					pxmlbuf++;
					noffset++;
					continue;
				}
				if (snameattr.compare(sendflag+nodename) == 0) //�ҵ�һ��������־
				{
					nNameNum--; //���Ʊ�־��������
					if (nNameNum < 0 ) //�����Ʊ�־С��0ʱ����ʱƥ�䣬�����ҵ���
					{
						vend = nodenameBoffset-1;
						nextnodebegin = nodenameEndoffset+1;
						return true;
					}
				}
				//���ǽ�����־
				//�Ƶ�>����  
				pxmlbuf++;
				noffset++;
				continue;
			}
			//ȡ�ýڵ����ƿ��Ƿ�ͽڵ�������ͬ
			if (!GetNodeName(snameattr,snodenamefind))
			{
				return false;//�޽ڵ�����
			}
			if (snodenamefind.compare(nodename) == 0) //�ڵ�������ͬ�����Ʊ�־��������
			{
				nNameNum++;
			}
			//�Ƶ�>����
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
	//��������
	int aValueBegin=0;
	int aValueEnd =0;

	//ȥ�� name=""ǰ��Ŀո��TAB
	while (*pbuf == SPACE || *pbuf == TABLESPACE) //�ҵ��ո���
	{
		pbuf++;
		aValueBegin++;
		if (aValueBegin>=attrlen)
		{
// 			sprintf(m_errMsg,"û������");
// 			LogMp(4,__FILE__,__LINE__,sattr.c_str(),sattr.length(),m_errMsg);
			return 0;
		}
	}
	int attrnamelen=0;
	aValueEnd = aValueBegin;
	while (aValueEnd<attrlen)
	{
		if (*pbuf == '=') //�����Ⱥ���
		{
			attrnamelen =  aValueEnd - aValueBegin;
			if (attrnamelen<1) 
			{
				//�����Ƿ��������� ֻ��=���ߴ��ڹ涨�ĳ��ȣ�Ҫȥ�������" "��Ķ���
				//����ֱ���˳��ɣ�ʡ����ȥ����
				sprintf(m_errMsg,"�����Ƿ��������� ֻ��=���ߴ��ڹ涨�ĳ���");
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
					sprintf(m_errMsg,"�����Ƿ��������� ֻ������û��=");
					LogMp(4,__FILE__,__LINE__,sattr.c_str(),sattr.length(),m_errMsg);
					return -1;
				}
				if (*pbuf != '"') //����˫���ţ����ԷǷ�
				{
					//����ֱ���˳��ɣ�ʡ����ȥ����
					//addnode->m_attribute.SetAttribute(attrname,false,"",false);
					sprintf(m_errMsg,"�����Ƿ��������� ���ƺ��治��=");
					LogMp(4,__FILE__,__LINE__,sattr.c_str(),sattr.length(),m_errMsg);
					return -1;
				}
				aValueEnd++;
				pbuf++;
				if (aValueEnd>=attrlen)
				{
					addnode->AddAttribute(attrname,false,"",false);
					sprintf(m_errMsg,"�����Ƿ��������� =��������");
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
						sprintf(m_errMsg,"�����Ƿ��������� =\"��������");
						LogMp(4,__FILE__,__LINE__,sattr.c_str(),sattr.length(),m_errMsg);
						return -1;
					}
				}
				int attrvaluelen =0;
				attrvaluelen = aValueEnd - aValueBegin;
				if (attrvaluelen <1) //����Ϊ��
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
				if (aValueEnd>=attrlen) // �ѵ�����
				{
					return 0;
				}
				if (*pbuf != SPACE) // ""���ǿո����Խ���
				{
					return 0;
				}
				//������һ������
				if (ParseNodeAttribute(sattr.substr(aValueEnd,attrlen-aValueEnd),addnode)!=0)
				{
					LogMp(4,__FILE__,__LINE__,"",0,"����[%s]���ó���",sattr.substr(aValueEnd,attrlen-aValueEnd).c_str());
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
	while (*pbuf == SPACE || *pbuf == TABLESPACE) //�ҵ��ո���
	{
		pbuf++;
		nBoffset++;
		if (nBoffset>=nEndLen)
		{
			LogMp(4,__FILE__,__LINE__,nameattr.c_str(),nameattr.length(),"ȫΪ�ո�");
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
	pbuf = (char *)xmlbuf.c_str();//ָ��xml�Ļ���
	char *tmpbuf=NULL; //ָ��xml�Ļ���
	std::string snameattr;
	CIXmlNode *pChildNode=NULL;
	bool isendnode=false;

//	LogMp(4,__FILE__,__LINE__,"",0,"����[%s]���%d",xmlbuf.c_str(),layer);

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
		if ( *pbuf == '<' )//�ҵ�<������
		{
			//�����ƣ���������ǿո�Ļ�һֱ������
			do 
			{
				pbuf++;
				bOffset++;
				if (*pbuf == 0 || bOffset >= eOffset-1)
				{
					LogMp(4,__FILE__,__LINE__,"",0,"�ڵ�[%s]��ֵ����<����>",parentNode->GetNodeName(false).c_str());
					return false;
				}
				
			} while (*pbuf == SPACE || *pbuf == TABLESPACE);
				
			
			nodenameBoffset = bOffset;//�ڵ����ƿ�ʼλ��
			nodenameEndoffset = bOffset;

			bool isfind=false;
			if (strncmp(pbuf,"!--",3) == 0)
			{
				pbuf+=3;
				bOffset+=3;
				nodenameEndoffset+=3;
				while (!isfind)
				{
					//�ҵ�ƥ��� '>'�� �õ��ڵ����ƽ���λ��nodenameendoffset
					while (*pbuf != '>')
					{
						pbuf++;
						bOffset++;
						if (*pbuf == 0 || bOffset >= eOffset-1)
						{
							LogMp(4,__FILE__,__LINE__,"",0,"�ڵ�[%s]��ֵ����<����>",parentNode->GetNodeName(false).c_str());
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
						LogMp(4,__FILE__,__LINE__,"",0,"�ڵ�[%s]��ֵ����<����>",parentNode->GetNodeName(false).c_str());
						return false;
					}
				} // while (!isfind)
			}
			else
			{
			
				//�ҵ�ƥ��� '>'�� �õ��ڵ����ƽ���λ��nodenameendoffset
				while (*pbuf != '>')
				{
					pbuf++;
					bOffset++;
					if (*pbuf == 0 || bOffset >= eOffset-1)
					{
						LogMp(4,__FILE__,__LINE__,"",0,"�ڵ�[%s]��ֵ����<����>",parentNode->GetNodeName(false).c_str());
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
				if (*tmpbuf=='/') //����ڵ�����������/����<name/>����ʽ��Ҫȥ��/
				{
					nnodeNameLen--;
					isendnode = true;
				}
			}
			
			snameattr = xmlbuf.substr(nodenameBoffset,nnodeNameLen);//�õ��ڵ����Ƽ�����
			//�����ڵ����Ƽ����Բ��ӵ�parentNode���ӽڵ���ȥ
			pChildNode = ParseNodeNameAttr(parentNode,snameattr,isendnode);
			if (pChildNode == NULL)
			{
				return false;
			}
			if (pChildNode->GetNodeType() != NODETYPE_VALUE) //<xx/>���汾ע�͵����
			{
				//�Ƶ�>����
				pbuf++;
				bOffset++;
				continue;
			}
			//�Ƶ�>����
			pbuf++;
			bOffset++;
			if (*pbuf == 0 || bOffset >= eOffset-1 )
			{
				LogMp(4,__FILE__,__LINE__,"",0,"�ڵ�%s>�������ݣ���ֻ��<>û��</>",pChildNode->GetNodeName(false).c_str());
				return false;
			}
			//<xx></xx>�����������ƥ���/xx
			int nNextNodeBegin=0;//�ڵ�ֵ�Ŀ�ʼλ��
			int nValueEnd=0;//�ڵ�ֵ�Ľ���λ��(�м���ܰ����ӽڵ�)
			
//			LogMp(4,__FILE__,__LINE__,"",0,"%s��[%d,%d]��ʼ����ƥ���[/%s]",xmlbuf.c_str(),bOffset,eOffset,pChildNode->GetNodeName(false).c_str());

			if (!FindEndNode(xmlbuf,bOffset,pChildNode->GetNodeName(false),nValueEnd,nNextNodeBegin))
			{
				LogMp(4,__FILE__,__LINE__,"",0,"��</%s>",pChildNode->GetNodeName(false).c_str());
				return false;
			}
//			LogMp(4,__FILE__,__LINE__,"",0,"%s ����ƥ���[/%s]����[%d,%d,%d]",xmlbuf.c_str(),pChildNode->GetNodeName(false).c_str(),bOffset,nValueEnd,nNextNodeBegin);

			//���ýڵ�ֵ
			pChildNode->SetNodeValue(xmlbuf.substr(bOffset,nValueEnd-bOffset),false);
			//�����²�
 			if ( layer == 0 )
 			{
 				pChildNode->ParseChildNode(layer);//���������ӽڵ�
 			}
 			else if (layer>1)
 			{
 				pChildNode->ParseChildNode(layer-1); //������һ��
 			}
			//pbuf��bOffset�Ƶ���һ�ڵ㿪ʼ
			pbuf+=(nNextNodeBegin-bOffset);
			bOffset=nNextNodeBegin;
//			LogMp(4,__FILE__,__LINE__,"",0,"%s��[%d,%d]��ʼ����Ľ���",xmlbuf.c_str(),bOffset,eOffset);
			if (*pbuf == 0 || bOffset >= eOffset)
			{
				return true;
			}
			
		}
		else //if ( *pbuf == '<' ) //˵���ǽڵ��ֵ�������ٽ�����
		{
			//<xx><yy>aaa</yy>ddd</xx>  
			//ȥ��ddd�������ٹ�����
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
	pbuf =  m_sXmlBuf;//ָ��xml�Ļ���
	pbuf += bOffset;//ָ��xml�Ļ���

	char *tmpbuf=NULL; //ָ��xml�Ļ���

	char *snameattr=NULL;

	CIXmlNode *pChildNode=NULL;
	bool isendnode=false;
	bool bres=false;

//	LogMp(4,__FILE__,__LINE__,"",0,"����[%s]���%d",xmlbuf.c_str(),layer);
	//���ô˽ڵ��ѽ���
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
		if ( *pbuf == '<' )//�ҵ�<������
		{
			//�����ƣ���������ǿո�Ļ�һֱ������
			do 
			{
				pbuf++;
				bOffset++;
				if (*pbuf == 0 || bOffset >= endoffset-1)
				{
					LogMp(4,__FILE__,__LINE__,"",0,"�ڵ�[%s]��ֵ����<����>",parentNode->GetNodeName(false).c_str());
					return false;
				}
			} while (*pbuf == SPACE || *pbuf == TABLESPACE);

			nodenameBoffset = bOffset;//�ڵ����ƿ�ʼλ��
			nodenameEndoffset = bOffset;//�ڵ����ƽ���λ��

			bool isfind=false;
			if (strncmp(pbuf,"!--",3) == 0)
			{
				pbuf+=3;
				bOffset+=3;
				nodenameEndoffset+=3;
				
				while (!isfind)
				{
					//�ҵ�ƥ��� '>'�� �õ��ڵ����ƽ���λ��nodenameendoffset
					while (*pbuf != '>')
					{
						pbuf++;
						bOffset++;
						if (*pbuf == 0 || bOffset >= endoffset-1)
						{
							LogMp(4,__FILE__,__LINE__,"",0,"�ڵ�[%s]��ֵ����<����>",parentNode->GetNodeName(false).c_str());
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
						LogMp(4,__FILE__,__LINE__,"",0,"�ڵ�[%s]��ֵ����<����>",parentNode->GetNodeName(false).c_str());
						return false;
					}
				} // while (!isfind)
			}
			else
			{
				//�ҵ�ƥ��� '>'�� �õ��ڵ����ƽ���λ��nodenameendoffset
				while (*pbuf != '>')
				{
					pbuf++;
					bOffset++;
					if (*pbuf == 0 || bOffset >= endoffset-1)
					{
						LogMp(4,__FILE__,__LINE__,"",0,"�ڵ�[%s]��ֵ����<����>",parentNode->GetNodeName(false).c_str());
						return false;
					}
					nodenameEndoffset++;
				}
			}
			
			//�ڵ����Ƴ���
			nnodeNameLen = nodenameEndoffset - nodenameBoffset;
			if (!isfind)
			{
				tmpbuf = pbuf;
				tmpbuf--;
				isendnode=false;
				if (*tmpbuf=='/') //����ڵ�����������/����<name/>����ʽ��Ҫȥ��/
				{
					nnodeNameLen--;
					isendnode = true;
				}
			}
			
			snameattr = (char *)malloc(nnodeNameLen+1);
			//memset(snameattr,0,nnodeNameLen+1);
			memcpy(snameattr,m_sXmlBuf+nodenameBoffset,nnodeNameLen);//�õ��ڵ����Ƽ�����
			snameattr[nnodeNameLen]=0;
			//�����ڵ����Ƽ����Բ��ӵ�parentNode���ӽڵ���ȥ
			pChildNode = ParseNodeNameAttr(parentNode,snameattr,isendnode);
			if (pChildNode == NULL)
			{
				free(snameattr);
				snameattr = NULL;
				return false;
			}
			free(snameattr);
			snameattr = NULL;
			if (pChildNode->GetNodeType() != NODETYPE_VALUE) //<xx/>���汾ע�͵����
			{
				//�Ƶ�>����
				pbuf++;
				bOffset++;
				continue;//while (*pbuf != 0)
			}
			//�Ƶ�>����
			pbuf++;
			bOffset++;
			if (*pbuf == 0 || bOffset >= endoffset-1 )
			{
				LogMp(4,__FILE__,__LINE__,"",0,"�ڵ�%s>�������ݣ���ֻ��<>û��</>",pChildNode->GetNodeName(false).c_str());
				return false;
			}
			//<xx></xx>�����������ƥ���/xx
			int nNextNodeBegin=0;//�ڵ�ֵ�Ŀ�ʼλ��
			int nValueEnd=0;//�ڵ�ֵ�Ľ���λ��(�м���ܰ����ӽڵ�)
			
//			LogMp(4,__FILE__,__LINE__,"",0,"%s��[%d,%d]��ʼ����ƥ���[/%s]",xmlbuf.c_str(),bOffset,eOffset,pChildNode->GetNodeName(false).c_str());
			
			if (!FindEndNodeDom(bOffset,endoffset,pChildNode->GetNodeName(false),nValueEnd,nNextNodeBegin))
			{
				LogMp(4,__FILE__,__LINE__,"",0,"��</%s>",pChildNode->GetNodeName(false).c_str());
				return false;
			}
//			LogMp(4,__FILE__,__LINE__,"",0,"%s ����ƥ���[/%s]����[%d,%d,%d]",xmlbuf.c_str(),pChildNode->GetNodeName(false).c_str(),bOffset,nValueEnd,nNextNodeBegin);

			//�����ڵ������
			bres = ParseXmlDom(bOffset,nValueEnd+1,pChildNode);
			if (!bres)
			{
				return false;
			}
			//pbuf��bOffset�Ƶ���һ�ڵ㿪ʼ
			pbuf = m_sXmlBuf+nNextNodeBegin;
			bOffset=nNextNodeBegin;

//			LogMp(4,__FILE__,__LINE__,"",0,"%s��[%d,%d]��ʼ����Ľ���",xmlbuf.c_str(),bOffset,eOffset);
			if (*pbuf == 0 || bOffset >= endoffset-1)
			{
				return true;
			}
			
		}
		else //else if ( *pbuf == '<' ) //˵���ǽڵ��ֵ�������ٽ�����
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
				memcpy(snameattr,m_sXmlBuf+bOffset,eOffset-bOffset);//�õ��ڵ����Ƽ�����
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
	while (*pbuf == SPACE || *pbuf == TABLESPACE) //�ҵ��ո���
	{
		pbuf++;
		nBoffset++;
		if (nBoffset >= nEndLen-1)
		{
			LogMp(4,__FILE__,__LINE__,nameattr,nEndLen,"ȫΪ�ո�");
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

	// <xx ��
	CIXmlNode *tmpnode = parentnode->NewNode();
	if (tmpnode == NULL)
	{
		LogMp(4,__FILE__,__LINE__,nodename.c_str(),nodename.length(),"NewNodeΪNULL");
		return NULL;
	}
	tmpnode->SetParseClass(this);
	
	
	if (nodename.compare("?xml") == 0) //�汾
	{
		//ȥ�� xx��Ŀո�
		while (*pbuf == SPACE || *pbuf == TABLESPACE) //�ҵ��ո���
		{
			pbuf++;
			nBoffset++;
			if (nBoffset >= nEndLen-1)
			{
				LogMp(4,__FILE__,__LINE__,nameattr,nEndLen,"ȫΪ�ո�");
				return NULL;
			}
		}
		if (nameattr[nEndLen-1] == '?') //�汾
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
			LogMp(4,__FILE__,__LINE__,"",0,"�汾�ڵ�[%s]�Ƿ�",nameattr);
			return NULL;
		}
	}
	else if (nodename.compare("!DOCTYPE") == 0) //ָ���ĵ�
	{
		//ȥ�� xx��Ŀո�
		while (*pbuf == SPACE || *pbuf == TABLESPACE) //�ҵ��ո���
		{
			pbuf++;
			nBoffset++;
			if (nBoffset >= nEndLen-1)
			{
				LogMp(4,__FILE__,__LINE__,nameattr,nEndLen,"ȫΪ�ո�");
				return NULL;
			}
		}
		tmpnode->SetNodeValue(nameattr+nBoffset,false);
		tmpnode->SetNodeType(NODETYPE_DOCTYPE);
		parentnode->AddChildNode(tmpnode);
	}
	else if (strncmp(nodename.c_str(),"!--",3) == 0) //ע��
	{
		if (strncmp(nameattr+(nEndLen-2),"--",2) == 0) //ע��
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
			LogMp(4,__FILE__,__LINE__,"",0,"ע�ͽڵ�[%s]�Ƿ�",nameattr);
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
		//��������
		if (ParseNodeAttribute(nameattr+nBoffset,tmpnode)!=0)
		{
			LogMp(4,__FILE__,__LINE__,nameattr,nEndLen,"�������ó���");
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
	int nNameNum=0;//�ڵ����
	
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
		if ( *pxmlbuf == '<' )//�ҵ�<������
		{
			//�����ƣ���������ǿո�Ļ�һֱ������
			do 
			{
				pxmlbuf++;
				noffset++;
				if (*pxmlbuf == 0 || noffset >= buflen)
				{
					return false;
				}
				
			} while (*pxmlbuf == SPACE || *pxmlbuf == TABLESPACE);

			nodenameBoffset = noffset;//�ڵ����ƿ�ʼλ��
			nodenameEndoffset = noffset;
			//�ҵ�ƥ��� '>'�� �õ��ڵ����ƽ���λ��nodenameendoffset
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
			if (*tmpbuf=='/') //����ڵ�����������/����<name/>����ʽ��Ҫȥ��/
			{
				nnodeNameLen--;
				//�Ƶ�>����
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
			snameattr = tmpchar;//�õ��ڵ����Ƽ�����
			free(tmpchar);
			if (nnodeNameLen == nodename.length()+1)
			{
				if (m_sXmlBuf[nodenameBoffset] != '/') //</xx> ������ȵ�����/��ʼ�ģ�������
				{
					//�Ƶ�>����  
					pxmlbuf++;
					noffset++;
					continue;
				}
				if (snameattr.compare(sendflag+nodename) == 0) //�ҵ�һ��������־
				{
					nNameNum--; //���Ʊ�־��������
					if (nNameNum < 0 ) //�����Ʊ�־С��0ʱ����ʱƥ�䣬�����ҵ���
					{
						vend = nodenameBoffset-1;
						nextnodebegin = nodenameEndoffset+1;
						return true;
					}
				}
				//���ǽ�����־
				//�Ƶ�>����  
				pxmlbuf++;
				noffset++;
				continue;
			}
			//ȡ�ýڵ����ƿ��Ƿ�ͽڵ�������ͬ
			if (!GetNodeName(snameattr,snodenamefind))
			{
				return false;//�޽ڵ�����
			}
			if (snodenamefind.compare(nodename) == 0) //�ڵ�������ͬ�����Ʊ�־��������
			{
				nNameNum++;
			}
			//�Ƶ�>����
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
	pbuf =  m_sXmlBuf;//ָ��xml�Ļ���
	pbuf += bOffset;//ָ��xml�Ļ���

	char *tmpbuf=NULL; //ָ��xml�Ļ���

	char *snameattr=NULL;

	CIXmlNode *pChildNode=NULL;
	bool isendnode=false;
	bool bres=false;

//	LogMp(4,__FILE__,__LINE__,"",0,"����[%s]���%d",xmlbuf.c_str(),layer);
	//���ô˽ڵ��ѽ���
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
		if ( *pbuf == '<' )//�ҵ�<������
		{
			//�����ƣ���������ǿո�Ļ�һֱ������
			do 
			{
				pbuf++;
				bOffset++;
				if (*pbuf == 0 || bOffset >= endoffset-1)
				{
					LogMp(4,__FILE__,__LINE__,"",0,"�ڵ�[%s]��ֵ����<����>",parentNode->GetNodeName(false).c_str());
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
					memcpy(snameattr,m_sXmlBuf+bOffset+1,eOffset-bOffset-1);//�õ��ڵ�����
					if (strncmp(parentNode->GetNodeName(false).c_str(),snameattr,eOffset-bOffset-1) !=0)
					{
						LogMp(4,__FILE__,__LINE__,"",0,"�ڵ�<%s>�������ݣ���û��ƥ���</%s>,��</%s>",parentNode->GetNodeName(false).c_str(),parentNode->GetNodeName(false).c_str(),snameattr);
						free(snameattr);
						snameattr = NULL;
						return false;
					}
					free(snameattr);
					snameattr = NULL;
					beginoffset = eOffset+1;
					return true;
				}
				LogMp(4,__FILE__,__LINE__,"",0,"�ڵ�<%s>�������ݣ���û��ƥ���</%s>",parentNode->GetNodeName(false).c_str(),parentNode->GetNodeName(false).c_str());
				return false;
			}
			


			nodenameBoffset = bOffset;//�ڵ����ƿ�ʼλ��
			nodenameEndoffset = bOffset;//�ڵ����ƽ���λ��

			bool isfind=false;
			if (strncmp(pbuf,"!--",3) == 0)
			{
				pbuf+=3;
				bOffset+=3;
				nodenameEndoffset+=3;
				
				while (!isfind)
				{
					//�ҵ�ƥ��� '>'�� �õ��ڵ����ƽ���λ��nodenameendoffset
					while (*pbuf != '>')
					{
						pbuf++;
						bOffset++;
						if (*pbuf == 0 || bOffset >= endoffset-1)
						{
							LogMp(4,__FILE__,__LINE__,"",0,"�ڵ�[%s]��ֵ����<����>",parentNode->GetNodeName(false).c_str());
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
						LogMp(4,__FILE__,__LINE__,"",0,"�ڵ�[%s]��ֵ����<����>",parentNode->GetNodeName(false).c_str());
						return false;
					}
				} // while (!isfind)
			}
			else
			{
				//�ҵ�ƥ��� '>'�� �õ��ڵ����ƽ���λ��nodenameendoffset
				while (*pbuf != '>')
				{
					pbuf++;
					bOffset++;
					if (*pbuf == 0 || bOffset >= endoffset-1)
					{
						LogMp(4,__FILE__,__LINE__,"",0,"�ڵ�[%s]��ֵ����<����>",parentNode->GetNodeName(false).c_str());
						return false;
					}
					nodenameEndoffset++;
				}
			}
			
			//�ڵ����Ƴ���
			nnodeNameLen = nodenameEndoffset - nodenameBoffset;
			if (!isfind)
			{
				tmpbuf = pbuf;
				tmpbuf--;
				isendnode=false;
				if (*tmpbuf=='/') //����ڵ�����������/����<name/>����ʽ��Ҫȥ��/
				{
					nnodeNameLen--;
					isendnode = true;
				}
			}
			
			snameattr = (char *)malloc(nnodeNameLen+1);
			//memset(snameattr,0,nnodeNameLen+1);
			memcpy(snameattr,m_sXmlBuf+nodenameBoffset,nnodeNameLen);//�õ��ڵ����Ƽ�����
			snameattr[nnodeNameLen]=0;
			//�����ڵ����Ƽ����Բ��ӵ�parentNode���ӽڵ���ȥ
			pChildNode = ParseNodeNameAttr(parentNode,snameattr,isendnode);
			if (pChildNode == NULL)
			{
				free(snameattr);
				snameattr = NULL;
				return false;
			}
			free(snameattr);
			snameattr = NULL;
			if (pChildNode->GetNodeType() != NODETYPE_VALUE) //<xx/>���汾ע�͵����
			{
				//�Ƶ�>����
				pbuf++;
				bOffset++;
				continue;//while (*pbuf != 0)
			}
			//�Ƶ�>����
			pbuf++;
			bOffset++;
			if (*pbuf == 0 || bOffset >= endoffset-1 )
			{
				LogMp(4,__FILE__,__LINE__,"",0,"�ڵ�%s>�������ݣ���ֻ��<>û��</>",pChildNode->GetNodeName(false).c_str());
				return false;
			}

			//����>�����
			int offsettmp= bOffset;
			bres = ParseXmlDom2(bOffset,endoffset,pChildNode);
			if (!bres)
			{
				return false;
			}
			//�ٴν����ٽ��ڵ�
			pbuf+=(bOffset-offsettmp);
			continue;
			
		}
		else //else if ( *pbuf == '<' ) //˵���ǽڵ��ֵ�������ٽ�����
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
				memcpy(snameattr,m_sXmlBuf+bOffset,eOffset-bOffset);//�õ��ڵ�ֵ
				parentNode->SetNodeType(NODETYPE_VALUE);
				parentNode->SetNodeValue(snameattr,false);
				free(snameattr);
				snameattr = NULL;
			}
			pbuf++;
			eOffset++;
			if  (*pbuf != '/')
			{
				LogMp(4,__FILE__,__LINE__,"",0,"�ڵ�<%s>�������ݣ���<��û��/����",parentNode->GetNodeName(false).c_str());
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
				memcpy(snameattr,m_sXmlBuf+bOffset+1,eOffset-bOffset-1);//�õ��ڵ�����
				if (strncmp(parentNode->GetNodeName(false).c_str(),snameattr,eOffset-bOffset-1) !=0)
				{
					LogMp(4,__FILE__,__LINE__,"",0,"�ڵ�<%s>�������ݣ���û��ƥ���</%s>,��</%s>",parentNode->GetNodeName(false).c_str(),parentNode->GetNodeName(false).c_str(),snameattr);
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