#include "public.h"
#include "attribute.h"
#include "node.h"
#include "xmlanalysis.h"
#include <stdarg.h>
//#include "Errlog.h"

CXMLAnalysis::CXMLAnalysis()
{
	m_xmldatabuf = NULL;
	memset(m_errMsg,0,sizeof(m_errMsg));
	memset(m_logFileName,0,sizeof(m_logFileName));
	memset(m_logFilePath,0,sizeof(m_logFilePath));
	strcpy(m_logFileName,"xmlparse.log");
#ifdef WIN32
	WORKPATH(m_logFilePath);
	strcat(m_logFilePath,"\\log");
	makedir(m_logFilePath);
#else
	sprintf(m_logFilePath,"%s/log",(char *)getenv("HOME"));
	makedir(m_logFilePath);
	
#endif
	m_logLevel = 5;///��־���𣬴��ڴ˼������־����д����־�ļ�0���ش���1��Ҫ����2һ�����3����4��ʾ����5������Ϣ
}
CXMLAnalysis::~CXMLAnalysis()
{
	if (m_xmldatabuf!= NULL)
	{
		free(m_xmldatabuf);
		m_xmldatabuf = NULL;
	}
}
void CXMLAnalysis::setItem(CNode *pNode, char *value)
{
	CAttribute *pAttribute,*hAttribute=NULL;
	
	char itemName[ITEMNAMELENGTH+1];

	int i,flag,j;

	memset(itemName,0,ITEMNAMELENGTH+1);
	for(i=0,flag=0,j=0;*(value+i)!=0;i++)
	{
		if(flag==0 && *(value+i)==' ')
		{
			continue;
		}
		flag=1;
		if(i==ITEMNAMELENGTH || *(value+i)==' ')
		{
			break;
		}
		itemName[j++]=*(value+i);
	}
	pNode->m_name = itemName;

	
	memset(itemName,0,ITEMNAMELENGTH+1);
	int ifg=0;
	for(flag=0,j=0;*(value+i)!=0;i++)
	{
		if(flag==0 && *(value+i)==' ')
		{
			continue;
		}
		flag=1;
		if(i==ITEMNAMELENGTH)
		{
			break;
		}
		if(*(value+i) == '=')
		{
			if(ifg == 2)
			{
				itemName[j++] = '=';
				flag=0;
				continue;
			}
			else if(ifg == 1)
			{
				ifg=2;
				flag=0;
				continue;
			}
			else
			{
				ifg=1;
			}
		}
		if((*(value+i) ==' ' && ifg == 0) || (*(value+i) == '=' && ifg == 1))	
		{
			ifg=1;
			pAttribute=new CAttribute;
			pAttribute->m_next=NULL;
			pAttribute->m_name = itemName;
			flag=0;
			if(hAttribute==NULL)
			{
				pNode->m_attribute=pAttribute;
			}
			else
			{
				hAttribute->m_next=pAttribute;
			}
			hAttribute=pAttribute;
			j=0;
			memset(itemName,0,ITEMNAMELENGTH+1);
			if(*(value+i) == '=')
			{
				ifg=2;
			}
			continue;
		}
		if(*(value+i) ==' ' && ifg == 2)
		{
			/*-------------ȥ�����Խڵ�ֵ������------------
			strcpy(hAttribute->m_value,itemName);
			----------------------------------------*/
			itemName[strlen(itemName)-1] = 0;
			hAttribute->m_value = (itemName+1);
			//----------------------------------------
			ifg=0;
			flag=0;
			j=0;
			memset(itemName,0,ITEMNAMELENGTH+1);
			continue;
		}
		itemName[j++]=*(value+i);
	}

	
	if(*itemName !=0 && ifg != 2)	
	{
		pAttribute=new CAttribute;
		pAttribute->m_name = itemName;
		if(hAttribute==NULL)
		{
			pNode->m_attribute=pAttribute;
		}
		else
		{
			hAttribute->m_next=pAttribute;
		}
		hAttribute=pAttribute;
	}
	if(*itemName !=0 && ifg == 2 )
	{
		/*------------ȥ�����Խڵ�ֵ������----------------
		strcpy(hAttribute->m_value,itemName);
		-------------------------------------------------*/
		itemName[strlen(itemName)-1] = 0;
		hAttribute->m_value = (itemName+1);
		//-------------------------------------------------
	}
}

CNode *CXMLAnalysis::analyseXML2Old(const char * buffer,int bufferlen, CNode * parentNode)
{
	CNode *curNode=NULL;//��ǰ�ڵ�
	CNode *prevNode=NULL;//��һ�ڵ�
	int    nodeCnt=NULL;//�ڵ����
	char  *pbuf=NULL;
	char  *tbuf=NULL;
	char   curN[ITEMNAMELENGTH+1];  //�ڵ����ư�������
	char   curName[ITEMNAMELENGTH+1]; //�ڵ�����<���>
	char   curEndName[ITEMNAMELENGTH+1]; //�ڵ�����</���>
	int    i=0;
	bool   isSubItem=false; //�Ƿ��ӽڵ�
	char   parentValue[ITEMVALUELENGTH+1];
	int    ivalue=0;
	int    icur=0;
	memset(curN,0,sizeof(curN));
	memset(curName,0,sizeof(curName));
	memset(curEndName,0,sizeof(curEndName));
	memset(parentValue,0,sizeof(parentValue));

	char *tmbuf=NULL; //ָ��xml�Ļ���
	tmbuf = (char *)buffer;

	//�˴����ļ�xml����� songfree 20090122
	std::string curBuffer;
	


	pbuf=tmbuf;//tmbufΪȥ��ע�ͺ������

	//��ʼ������ȥע�ͺ��xml
	for(nodeCnt=0;*pbuf!=0;)
	{
		while(*pbuf!=0)
		{
			//## �ҵ�����������
			if(*pbuf=='<')
			{
				do 
				{
					pbuf++;

				} while (*pbuf == 9 || *pbuf == 32);

				int ii=0;//songfree add 
				for(i=0; *pbuf != 0 && *pbuf != '>'; pbuf++)
				{
					if (i<ITEMNAMELENGTH)
					{
						curName[i]=*pbuf;
						ii++;
					}
					i++;
				}
				
				//-------------------------------------------------

				//�õ�һ�������� <>���
				while (curName[ii-1] == 9 || curName[ii-1] == 32)
				{
					ii--;
				}
				curName[ii] = 0;
				strcpy(curN,curName);
				int j=0;
				//�õ��ڵ�����
				for(;j<ii;j++)
				{
					if(curName[j]==' ' || curName[j]=='=')
					{
						curName[j]=0;
						break;
					}
				}
				//�����'>'ǰ����/��ʾ�õ�һ���������� songfree add
				pbuf--;
				if (*pbuf=='/')
				{
					//���ɽڵ�
					//## ����������ڵ�
					curNode=new CNode;
					curNode->m_nextNode=NULL;
					curNode->m_lastNode = NULL;
					curNode->m_parentNode=parentNode;
					curNode->m_firstChildNode=NULL;
					curNode->m_endChildNode=NULL;
					curNode->m_name = curName;
					int curNLen= strlen(curN);
					curN[curNLen-1] = 0;
					setItem(curNode,curN);

					if(parentNode->m_firstChildNode==NULL)
					{
						parentNode->m_firstChildNode=curNode;
						parentNode->m_endChildNode=curNode;
					}
					else
					{
						prevNode=parentNode->m_firstChildNode;
						while(prevNode->m_nextNode!=NULL)
							prevNode=prevNode->m_nextNode;
						prevNode->m_nextNode=curNode;
						curNode->m_lastNode = prevNode;
						parentNode->m_endChildNode=curNode;
						
					}
					parentNode->m_value = "";
					continue;	
				}
				pbuf++;

				if(*pbuf != 0) 
				{
					pbuf++;
				}
				icur=0;
				isSubItem=true;
				break;
			}
			else
			{
				parentValue[ivalue++]=*pbuf;
			} //end if(*pbuf=='<')
			
			pbuf++;
			
		} //end while(*pbuf!=0)
		while (*pbuf == 32 || *pbuf == 9) 
		{
			pbuf++;
		}
		//�˴����ļ�xml����� songfree 20090122
		curBuffer = std::string("");



		//��ȡ�ڵ�����
		while(*pbuf != 0)
		{

			tbuf = pbuf + 1;//tbuf��Ϊ�ڵ������������
			while (*tbuf == 32 || *tbuf == 9 ) 
			{
				tbuf++;
			}
			if(*pbuf == '<' && *tbuf == '/')
			{
				tbuf++;
				while (*tbuf == 32 || *tbuf == 9 ) 
				{
					tbuf++;
				}
				//ȡ�ڵ���
				for(i = 0; *tbuf !=0 && *tbuf != '>'; tbuf++)
				{
					curEndName[i++]=*tbuf;
				}
				while (curEndName[i - 1] == 9 || curEndName[i - 1] == 32) 
				{
					i--;
				}
				curEndName[i]=0;
				//## �ҵ������������
				
				if(strcmp(curName,curEndName)==0)
				{
					//## ����������ڵ�
					curNode=new CNode;
					curNode->m_nextNode=NULL;
					curNode->m_parentNode=parentNode;
					curNode->m_firstChildNode=NULL;
					curNode->m_endChildNode=NULL;
					curNode->m_lastNode = NULL;
					setItem(curNode,curN);

					if(parentNode->m_firstChildNode==NULL)
					{
						parentNode->m_firstChildNode=curNode;
						parentNode->m_endChildNode=curNode;
					}
					else
					{
						prevNode=parentNode->m_firstChildNode;
						while(prevNode->m_nextNode!=NULL)
							prevNode=prevNode->m_nextNode;
						prevNode->m_nextNode=curNode;
						curNode->m_lastNode = prevNode;
						parentNode->m_endChildNode=curNode;
					}
					//printf("%d-%s\n",strlen(curBuffer),curBuffer);
					analyseXML2Old(curBuffer.c_str(),curBuffer.length(),curNode);




					//## ������һ�����ݱ�

					if(*tbuf!=0)
						pbuf=tbuf+1;
					else 
						pbuf=tbuf;

					isSubItem=false;
					
					break;//end while(*pbuf != 0)

				} //end if(strcmp(curName,curEndName)==0)
				
			}
			if(isSubItem==false)
			{
				parentValue[ivalue]=*pbuf;
				ivalue++;
			}
			char tmpaa = *pbuf;
			//�˴����ļ�xmlʱ�����
			curBuffer +=  tmpaa;	


			icur++;
			pbuf++;
		} //end while(*pbuf != 0)


	} //end for(nodeCnt=0;*pbuf!=0;)

	parentValue[ivalue]=0;

	int ti;
	for(ti=ivalue;ti>=0;ti--)
	{
		if(parentValue[ti]!=' ')
		{
			break;
		}
	}
	if(ti<ivalue)
	{
		parentValue[ti+1]=0;
	}
	for(ti=0;parentValue[ti]==' ';ti++);

    if (parentNode->m_firstChildNode!=NULL)
	{
		parentNode->m_value = "";
	}
	else
	{
		parentNode->m_value = parentValue+ti;
	}
	
	return parentNode;
}
CNode *CXMLAnalysis::analyseXML2(int beginoffset,int endoffset, CNode * parentNode)
{
//	CErrlog publog;
//	publog.SetLogPara(5,"testxml.log");
	CNode *curNode=NULL;//��ǰ�ڵ�
	char  *pbuf=NULL;
	bool   isSubItem=true; 
	int bOffset=0;
	int eOffset=0;
	int nodenameBoffset=0;
	int nodenameEndoffset=0;
	bOffset = beginoffset;
	eOffset = endoffset;

	char sNodeNameAttr[NODENAMEATTRLEN + 1];//�ڵ����Ƽ�����
	char sNodeEndChar[NODENAMELEN+1];//�ڵ����Ƽ�����
	char sNodeName[NODEVALUELEN+1];//�ڵ�ֵ
	memset(sNodeNameAttr,0,sizeof(sNodeNameAttr));
	memset(sNodeEndChar,0,sizeof(sNodeEndChar));
	memset(sNodeName,0,sizeof(sNodeName));

	pbuf = (char *)m_xmldatabuf;//ָ��xml�Ļ���
	pbuf +=beginoffset;

	char *tmpbuf=NULL; //ָ��xml�Ļ���

//	publog.LogBin(0,__FILE__,__LINE__,"����xml",pbuf,endoffset-beginoffset);
	
	while (*pbuf != 0)
	{
		int asc = *pbuf;
		while (*pbuf == SPACE || *pbuf == TABLESPACE || *pbuf == ENTER || *pbuf== 13)
		{
			pbuf++;
			bOffset++;
			if (*pbuf == 0)
			{
				return NULL;
			}
		}
		if ( *pbuf == '<' )//�ҵ�<������
		{
			//�����ƣ���������ǿո�Ļ�һֱ������
			do 
			{
				pbuf++;
				bOffset++;
				if (*pbuf == 0 )
				{
					return NULL;
				}
				
			} while (*pbuf == SPACE || *pbuf == TABLESPACE);
			nodenameBoffset = bOffset;//�ڵ����ƿ�ʼλ��
			nodenameEndoffset = bOffset;
			//�ҵ�ƥ��� '>'�� �õ��ڵ����ƽ���λ��nodenameendoffset
			while (*pbuf != '>')
			{
				pbuf++;
				bOffset++;
				if (*pbuf == 0)
				{
					return NULL;
				}
				nodenameEndoffset++;
				if (bOffset >= endoffset)
				{
					//LogMp(10,__FILE__,__LINE__,m_xmldatabuf+nodenameBoffset,nodenameEndoffset-nodenameBoffset,"�޽ڵ����ݻ�ƥ�� λ��%d-%d֮��",nodenameBoffset,nodenameEndoffset);
					return NULL;//�޽ڵ����ݻ�ƥ��
				}
			}
			int nnodeNameLen = nodenameEndoffset - nodenameBoffset;
			if (nnodeNameLen > NODENAMEATTRLEN) //�ڵ����Ƽ����Գ�������ĳ���
			{
				//�˽ڵ㲻������,ֱ�ӽ��������
				sprintf(m_errMsg,"�ڵ����Ƽ����Գ��ȳ���%ld %ld",nodenameEndoffset - nodenameBoffset,NODENAMEATTRLEN);
				printf("%s\n",m_errMsg);
				LogMp(4,__FILE__,__LINE__,"",0,m_errMsg);
				continue;//while (*pbuf != 0)
			}
			tmpbuf = pbuf;
			tmpbuf--;
			if (*tmpbuf=='/') //����ڵ�����������/����<name/>����ʽ��Ҫȥ��/
			{
				memset(sNodeNameAttr,0,sizeof(sNodeNameAttr));
				memcpy(sNodeNameAttr,m_xmldatabuf+nodenameBoffset,nnodeNameLen-1);
			}
			else
			{
				memset(sNodeNameAttr,0,sizeof(sNodeNameAttr));
				memcpy(sNodeNameAttr,m_xmldatabuf+nodenameBoffset,nnodeNameLen);
			}
			//���ɽڵ�
			curNode=new CNode;
			if (!AnalyseNodeAttr(sNodeNameAttr,nnodeNameLen,curNode))//�����ڵ���������
			{
				printf("%s\n",m_errMsg);//�����������������
				delete curNode;
				LogMp(4,__FILE__,__LINE__,"",0,m_errMsg);
				continue;// while (*pbuf != 0)
			}
			tmpbuf = pbuf;
			tmpbuf--;
			if (*tmpbuf=='/') //�����'>'ǰ����/��ʾ�õ�һ����������
			{
				pbuf++;
				bOffset++;
				if (*pbuf == 0)
				{
					delete curNode;
					LogMp(4,__FILE__,__LINE__,"",0,"%d /��������",bOffset);
					return NULL;
				}
				//�����������Ľڵ�ӵ� parentNode �ڵ�
				if (!addNode(parentNode,curNode))
				{
					printf("%s\n",m_errMsg);//�����������������
					LogMp(4,__FILE__,__LINE__,"",0,m_errMsg);
					continue;//while (*pbuf != 0)
				}
				parentNode->m_value = "";//���ڵ��ֵ��Ϊ��
				continue;//while (*pbuf != 0)
			}
			//�Ƶ�>����
			pbuf++;
			bOffset++;
			if (*pbuf == 0)
			{
				delete curNode;
				LogMp(4,__FILE__,__LINE__,"",0,"%d >�������ݣ���ֻ��<>û��</>",bOffset);
				return NULL;
			}
			//ȥ����ƥ���</xxx>


//songfree 20100614	
			int vend=0;
			int nextnodebegin=0;
			if (!FindEndNode(bOffset,eOffset,curNode->m_name,vend,nextnodebegin))
			{
				delete curNode;
				return NULL;
			}
			//�ҵ�ƥ�����
			//�����������Ľڵ�ӵ� parentNode �ڵ�
			if (!addNode(parentNode,curNode))
			{
				printf("%s\n",m_errMsg);//�����������������
				LogMp(4,__FILE__,__LINE__,"",0,m_errMsg);
				return NULL;//while (*pbuf != 0)
			}
			if (vend - bOffset <1)
			{
				curNode->m_value = "";
				pbuf = pbuf+nextnodebegin-bOffset;
				bOffset = nextnodebegin;	
				//						publog.LogBin(0,__FILE__,__LINE__,"����xml",pbuf,endoffset-bOffset);
				isSubItem = false;
			}
			else
			{
				analyseXML2(bOffset,vend,curNode);
				pbuf = pbuf+nextnodebegin-bOffset;
				bOffset = nextnodebegin;	
				//						publog.LogBin(0,__FILE__,__LINE__,"����xml",pbuf,endoffset-bOffset);
				isSubItem = false;
			}
			continue;


// 			sprintf(sNodeEndChar,"</%s>",curNode->m_name.c_str());
// 			int nNodeValueLen=0;//�ڵ�ֵ�ĳ���(�����ӽڵ�)
// 			int nNodeoEndffset=0;//�˽ڵ�Ľ�������
// 			nNodeoEndffset = curNode->m_name.length()+3;
// 			isSubItem = true;
// //			publog.LogBin(0,__FILE__,__LINE__,"����xml",m_xmldatabuf+bOffset,endoffset-bOffset);
// //			publog.LogBin(0,__FILE__,__LINE__,"����xml",pbuf,endoffset-bOffset);
// 			
// 			tmpbuf = pbuf;
// 			
// 			for (eOffset = bOffset ;eOffset < endoffset ; eOffset++)
// 			{
// 				if (strncmp(tmpbuf,sNodeEndChar,nNodeoEndffset) == 0)
// 				{
// 					//�ҵ�ƥ�����
// 					//�����������Ľڵ�ӵ� parentNode �ڵ�
// 					if (!addNode(parentNode,curNode))
// 					{
// 						printf("%s\n",m_errMsg);//�����������������
// 						LogMp(4,__FILE__,__LINE__,"",0,m_errMsg);
// 						continue;//while (*pbuf != 0)
// 					}
// 					//�ٴη��������xml������
// //					publog.LogBin(0,__FILE__,__LINE__,"����xml",m_xmldatabuf+bOffset,eOffset-bOffset);
// //					publog.LogBin(0,__FILE__,__LINE__,"����xml",pbuf,eOffset-bOffset);
// 					if (eOffset - bOffset <1)
// 					{
// 						curNode->m_value = "";
// 						pbuf = pbuf+nNodeoEndffset;
// 						bOffset = bOffset+nNodeoEndffset;	
// //						publog.LogBin(0,__FILE__,__LINE__,"����xml",pbuf,endoffset-bOffset);
// 						isSubItem = false;
// 					}
// 					else
// 					{
// 						analyseXML2(bOffset,eOffset,curNode);
// 						pbuf = pbuf+ eOffset-bOffset+nNodeoEndffset;
// 						bOffset = eOffset+nNodeoEndffset;	
// //						publog.LogBin(0,__FILE__,__LINE__,"����xml",pbuf,endoffset-bOffset);
// 						isSubItem = false;
// 					}
// 					
// 					break;//while (*pbuf != 0)
// 				}
// 				tmpbuf++;
// 			}
// 			if (!isSubItem)
// 			{
// 				continue;//while (*pbuf != 0)	
// 			}
// 			//û���ҵ�ƥ���
// 			LogMp(10,__FILE__,__LINE__,"",0,"û�кʹ˽ڵ�ƥ�������<%s>",curNode->m_name.c_str());
// 			delete curNode;
// 			return NULL;
		}
		else //if ( *pbuf == '<' ) //˵���ǽڵ��ֵ
		{
			if (parentNode->m_parentNode == NULL)
			{
				//������ڵ�Ϊ�գ����ʾ��ڵ�Ϊ���ڵ㣬����ֱ�ӷ�����
				parentNode->m_value = "";
				return parentNode;
			}
			eOffset = bOffset;
			do 
			{
				pbuf++;
				eOffset++;
				if (*pbuf == 0)
				{
					break;
				}
				
			} while (*pbuf != '<');
			memset(sNodeNameAttr,0,sizeof(sNodeNameAttr));
			if (eOffset - bOffset > NODEVALUELEN)
			{
				sprintf(m_errMsg,"�ڵ�ֵ����%ld > %ld",eOffset - bOffset,NODEVALUELEN);
				LogMp(4,__FILE__,__LINE__,"",0,"%s %s",m_errMsg,curNode->m_name.c_str());
				return NULL;
			}
			else if (eOffset - bOffset<=0)
			{
				parentNode->m_value = "";
			}
			else
			{
				memcpy(sNodeNameAttr,m_xmldatabuf+bOffset,eOffset - bOffset);
				parentNode->m_value = sNodeNameAttr;
			}
			return parentNode;
		}
	}
	return parentNode;
}
CNode *CXMLAnalysis::analyseXML(const char * buffer,int bufferlen, CNode * parentNode)
{
	
	if (m_xmldatabuf!=NULL)
	{
		free(m_xmldatabuf);
		m_xmldatabuf = NULL;
	}
	//������ڵ�Ϊ���򷵻ؿ�
	if ( parentNode==NULL )
	{
		return NULL;
	}
	if(0 == buffer[0] || bufferlen<1) //û������
	{
		parentNode->m_value = std::string("");
		return parentNode;
	}
	m_xmldatabuf = (char *)malloc(bufferlen+1); 
	if (m_xmldatabuf == NULL)
	{
		sprintf(m_errMsg,"����%ld���ڴ����ݳ���",bufferlen+1);
		return NULL;
	}
	memset(m_xmldatabuf,0,bufferlen+1);
//	LogMp(4,__FILE__,__LINE__,buffer,bufferlen,"ȥ��ע��ǰ��xml����");
	memcpy(m_xmldatabuf,buffer,bufferlen);
	deleteRemark(m_xmldatabuf);

	int buflen = strlen(m_xmldatabuf);
//	LogMp(4,__FILE__,__LINE__,m_xmldatabuf,strlen(m_xmldatabuf),"ȥ��ע�ͺ��xml����");
	//����ȥ��ע�ͺ��xml
// 	if (buflen> 0)
// 	{
		analyseXML2(0,buflen, parentNode);
// 	}
// 	else
// 	{
// 		analyseXML2Old(m_xmldatabuf,buflen, parentNode);
// 	}
	free(m_xmldatabuf);
	m_xmldatabuf = NULL;
	return parentNode;
}

void CXMLAnalysis::deleteRemark(char *buffer)
{
	char  *pbuf;
	char  *tmpbuffer;//��ʱָ��buffer��ָ��
	//ȥ��ע�Ͳ���,��xml����ŵ�tmbuf
	pbuf=buffer;
	tmpbuffer = (char *)buffer;
	for(;*tmpbuffer!=0;tmpbuffer++)
	{
		if(*tmpbuffer != 8)
		{
			//-------ȥ��ע�Ͳ���------------------------------------
			if (*tmpbuffer == '<')
			{
				do 
				{
					tmpbuffer++;
				} while ((*tmpbuffer==8 || *tmpbuffer==32 || *tmpbuffer==9 ) && *tmpbuffer!=0); 

				if (!strncmp(tmpbuffer, "?xml", 4))
				{
					tmpbuffer += 3;
					while (*tmpbuffer!=0)
					{
						tmpbuffer++;
						if (!strncmp(tmpbuffer, "?>", 2))
						{
							//songfree add 20081125
							if (strncmp(tmpbuffer, "?><", 3)==0)
							{
								tmpbuffer += 1; //���汾����û���ַ�ֱ�Ӹ���xml����ʱ��1
							}
							else
							{
								tmpbuffer += 2;
							}
							break;
						}
					}
					continue;	
				}		
				else if (!strncmp(tmpbuffer, "!--", 3))
				{
					tmpbuffer += 2;
					while (*tmpbuffer!=0)
					{
						tmpbuffer++;
						if (!strncmp(tmpbuffer, "-->", 3))
						{
							if (strncmp(tmpbuffer, "--><", 4)==0)
							{
								tmpbuffer += 2; //��ע�ͺ���û���ַ�ֱ�Ӹ���xml����ʱ��2
							}
							else
							{
								tmpbuffer += 3;
							}
							
							break;
						}
					}
					continue;
				}
				*pbuf='<';
				pbuf++;
			} //end if (*tmpbuffer == '<')
			//-----------------------------------------------------
			*pbuf=*tmpbuffer;

			pbuf++;
		}
	}
	*pbuf=0;
}

bool CXMLAnalysis::AnalyseNodeAttr(const  char *buffer,int buflen, CNode *curnode)
{
//	CErrlog publog;
//	publog.SetLogPara(5,"testxml.log");

	
	char *pbuf=NULL;
	char nodename[NODENAMELEN+1];
	memset(nodename,0,sizeof(nodename));
	int nBoffset=0;
	int nEoffset=0;
	CAttribute *tmpattr=NULL;
	
	pbuf = (char *)buffer;
	while (*pbuf == SPACE || *pbuf == TABLESPACE) //�ҵ��ո���
	{
		pbuf++;
		nBoffset++;
		if (nBoffset>=buflen)
		{
			sprintf(m_errMsg,"0-%dȫ�ǿո񣬲��ǽڵ�����",nBoffset);
			LogMp(4,__FILE__,__LINE__,m_errMsg,strlen(m_errMsg),"");
			return false;
		}
	}
	nEoffset = nBoffset;
	for (int i=nBoffset;i<buflen;i++)
	{
		pbuf++;
		nEoffset++;
		if (*pbuf == SPACE ||  *pbuf == TABLESPACE)
		{
			break;
		}
	}
	if (nEoffset - nBoffset > NODENAMELEN)
	{
		sprintf(m_errMsg,"�ڵ����Ƴ��ȳ��ޣ�%d �����趨ֵ",nEoffset - nBoffset,NODENAMELEN);
		LogMp(4,__FILE__,__LINE__,m_errMsg,strlen(m_errMsg),"");
		return false;
	}
	memcpy(nodename,buffer+nBoffset,nEoffset - nBoffset);
	curnode->m_name = nodename;
	nBoffset = nEoffset;
	if (buflen-nBoffset<1)
	{
		return true;
	}
	return AnalyseAttr(buffer+nBoffset,buflen-nBoffset,curnode);

}

bool CXMLAnalysis::addNode(CNode *desnode, CNode *newnode,bool first)
{
	if (first)
	{
		newnode->m_lastNode = NULL;
		if (desnode->m_firstChildNode != NULL) //���ǵ�һ���ӽڵ�
		{
			desnode->m_firstChildNode->m_lastNode = newnode;//ԭ��һ�ڵ���Ͻڵ�Ϊ�˽ڵ�
			newnode->m_nextNode=desnode->m_firstChildNode; //�˽ڵ���ٽ��½ڵ�Ϊԭ��һ�ӽڵ�
		}
		else
		{
			desnode->m_endChildNode = newnode; 
			newnode->m_nextNode = NULL;
		}
		newnode->m_parentNode=desnode;//���ڵ�Ϊ��ǰ�ڵ�
		desnode->m_firstChildNode=newnode;//��ǰ�ڵ�ĵ�һ���ӽڵ�Ϊ�˽ڵ�
	}
	else
	{
		newnode->m_nextNode = NULL;
		if (desnode->m_endChildNode != NULL) //��������ӽڵ�
		{
			desnode->m_endChildNode->m_nextNode = newnode;//ԭ���һ�ڵ���½ڵ�Ϊ�˽ڵ�
			newnode->m_lastNode=desnode->m_endChildNode; //�˽ڵ���ٽ��Ͻڵ�Ϊԭ���һ�ӽڵ�
		}
		else
		{
			desnode->m_firstChildNode = newnode; 
			newnode->m_lastNode = NULL;
		}
		newnode->m_parentNode=desnode;//���ڵ�Ϊ��ǰ�ڵ�
		desnode->m_endChildNode=newnode;//��ǰ�ڵ�����һ���ӽڵ�Ϊ�˽ڵ�
	}
	
	return true;
}

bool CXMLAnalysis::AnalyseAttr(const char *buffer, int buflen, CNode *curnode)
{
//	CErrlog publog;
//	publog.SetLogPara(5,"testxml.log");

	
	char *pbuf=NULL;
	char *attrvalue=NULL;
	char *attrname=NULL;

	
	pbuf = (char *)buffer;
	//��������
	int aValueBegin=0;
	int aValueEnd =0;


//	publog.LogBin(0,__FILE__,__LINE__,"����xml",pbuf,buflen);
	//ȥ�� name=""ǰ��Ŀո��TAB
	while (*pbuf == SPACE || *pbuf == TABLESPACE) //�ҵ��ո���
	{
		pbuf++;
		aValueBegin++;
		if (aValueBegin>=buflen)
		{
			sprintf(m_errMsg,"û������");
			LogMp(4,__FILE__,__LINE__,m_errMsg,strlen(m_errMsg),"");
			return false;
		}
	}
//	publog.LogBin(0,__FILE__,__LINE__,"����xml",pbuf,buflen-aValueBegin);

	int attrnamelen=0;
	aValueEnd = aValueBegin;
	while (aValueEnd<buflen)
	{
		
		if (*pbuf == '=') //�����Ⱥ���
		{
			attrnamelen =  aValueEnd - aValueBegin;
			if (attrnamelen<1) 
			{
				//�����Ƿ��������� ֻ��=���ߴ��ڹ涨�ĳ��ȣ�Ҫȥ�������" "��Ķ���
				//����ֱ���˳��ɣ�ʡ����ȥ����
				sprintf(m_errMsg,"�����Ƿ��������� ֻ��=���ߴ��ڹ涨�ĳ���");
				LogMp(4,__FILE__,__LINE__,m_errMsg,strlen(m_errMsg),"");
				return false;
			}
			else
			{
				aValueEnd++;
				pbuf++;
				if (aValueEnd>=buflen)
				{
					if (attrname != NULL)
					{
						curnode->SetNewAttribute(attrname,"");
					}
					sprintf(m_errMsg,"�����Ƿ��������� ���Ƽ�=����û������");
					LogMp(4,__FILE__,__LINE__,m_errMsg,strlen(m_errMsg),"");
					return false;
				}
				if (*pbuf != '"') //����˫���ţ����ԷǷ�
				{
					//����ֱ���˳��ɣ�ʡ����ȥ����
					if (attrname != NULL)
					{
						curnode->SetNewAttribute(attrname,"");
					}
					sprintf(m_errMsg,"�����Ƿ��������� ���Ƽ�=���治������");
					LogMp(4,__FILE__,__LINE__,m_errMsg,strlen(m_errMsg),"");
					return false;
				}
				

				aValueEnd++;
				pbuf++;
				if (aValueEnd>=buflen)
				{
					curnode->SetNewAttribute(attrname,"");
					sprintf(m_errMsg,"�����Ƿ��������� =��������");
					LogMp(4,__FILE__,__LINE__,m_errMsg,strlen(m_errMsg),"");
					
					return false;
				}
				attrname = (char *)malloc(attrnamelen+1);
				memset(attrname,0,attrnamelen+1);
				memcpy(attrname,buffer+aValueBegin,attrnamelen);

				aValueBegin = aValueEnd;
				while (*pbuf != '"')
				{
					aValueEnd++;
					pbuf++;
					if (aValueEnd>=buflen)
					{
						curnode->SetNewAttribute(attrname,"");
						sprintf(m_errMsg,"�����Ƿ��������� =\"��������");
						LogMp(4,__FILE__,__LINE__,m_errMsg,strlen(m_errMsg),"");
						free(attrname);
						attrname = NULL;
						return false;
					}
				}
//				publog.LogBin(0,__FILE__,__LINE__,"����xml",pbuf,buflen-aValueEnd);
				int attrvaluelen =0;
				attrvaluelen = aValueEnd - aValueBegin;
				if (attrvaluelen <1) //����Ϊ��
				{
					curnode->SetNewAttribute(attrname,"");
					free(attrname);
					attrname = NULL;
				}
				else
				{
					attrvalue = (char *)malloc(attrvaluelen+1);
					memset(attrvalue,0,attrvaluelen+1);
					memcpy(attrvalue,buffer+aValueBegin,attrvaluelen);
					curnode->SetNewAttribute(attrname,attrvalue);
					free(attrname);
					attrname = NULL;
					free(attrvalue);
					attrvalue = NULL;
				}
				aValueEnd++;
				pbuf++;
				if (aValueEnd>=buflen)
				{
					return true;
				}
				if (*pbuf != SPACE)
				{
					return true;
				}
//				publog.LogBin(0,__FILE__,__LINE__,"����xml",pbuf,buflen-aValueEnd);
				if (!AnalyseAttr(buffer+aValueEnd,buflen-aValueEnd,curnode))
				{
					return false;
				}
				return true;

			}
			
		}
		aValueEnd++;
		pbuf++;
	}
	return true;
}
int CXMLAnalysis::LogMp(int level, const char *filename, int line,const char *data,int datalen,const char *fmt,...)
{
#ifdef WIN32
	
#else
	char *p = (char *)getenv("XMLLOGLEVEL");
	if (p != NULL)
	{
		m_logLevel = atoi(p);
		p=NULL;
	}
#endif
	if (level>m_logLevel)
	{
		return 0;
	}
	FILE *fp;
	int j;
	char fname[420];
	char fpathname[500];

	memset(fname,0,sizeof(fname));
	memset(fpathname,0,sizeof(fpathname));

	strcpy(fname,filename);
	getName(fname);
	char buffer[60];
	memset(buffer,0,sizeof(buffer));
	//0���ش���1��Ҫ����2һ�����3����4��ʾ����5������Ϣ
	switch (level)
	{
	case 0:
		sprintf(buffer,"���� ");
		break;
	case 1:
		sprintf(buffer,"��Ҫ ");
		break;	
	case 2:
		sprintf(buffer,"һ�� ");
		break;	
	case 3:
		sprintf(buffer,"���� ");
		break;	
	case 4:
		sprintf(buffer,"��ʾ���� ");
		break;	
	case 5:
		sprintf(buffer,"������Ϣ ");
		break;	
	default:
		sprintf(buffer,"����%d ",level);
		break;
	}
	
	sprintf(fpathname,"%s/%s%s",m_logFilePath,Today(),m_logFileName);
	fp=fopen(fpathname,"a");
	if(fp==NULL)
	{
		return -1;
	}

	fprintf(fp,"%s �ļ�[%s]��[%d] %s ",Get_Date(),fname,line,buffer);

	fprintf(fp,"\n ���ݳ���%d[",datalen);
	for (j=0;j<datalen;j++)
	{
		
		if (data[j] == 0)    
		{
			fputc('*',fp);
		}
		else
		{
			fputc(data[j],fp);
		}
	}
    fprintf(fp," ]\n");
//	fprintf(fp,"%s ",data);

	va_list ap;
	va_start(ap, fmt);
    vfprintf(fp, fmt, ap);
    va_end(ap);
    fprintf(fp, "\n");
    fflush(fp);
    fclose(fp);
	return 0;
}
char *CXMLAnalysis::Get_Date()
{
	
	time_t t;
	static char buf[22];
	
	time(&t);
#ifndef _AIX32_THREADS
	struct tm *tt;
	tt=localtime(&t);
	sprintf(buf,"%4d-%02d-%02d %02d:%02d:%02d",\
		1900+tt->tm_year,tt->tm_mon+1,tt->tm_mday,\
		tt->tm_hour,tt->tm_min,tt->tm_sec);
#else
	struct tm tt;
	localtime_r(&t,&tt);
	sprintf(buf,"%4d-%02d-%02d %02d:%02d:%02d",\
		1900+tt.tm_year,tt.tm_mon+1,tt.tm_mday,\
		tt.tm_hour,tt.tm_min,tt.tm_sec);
#endif
	
	
	return(buf);
}

int CXMLAnalysis::getName(char *filename)
{
	int ret;
	int offset;
	bool bispath;
	char fileName[600];
	memset(fileName,0,sizeof(fileName));
	ret = strlen(filename);
	if (ret<1)
	{
		return 0;
	}
	offset = 0;
	bispath = false;
	for (int i=ret-1;i>=0;)
	{
		if (filename[i] == '\\' || filename[i] == '/' )
		{
			offset = i;
			bispath = true;
			break;
		}
		i--;
	}
	if (bispath)
	{
		strncpy(fileName,filename+offset+1,ret-offset);
		fileName[ret-offset]=0;
		strcpy(filename,fileName);
	}
	return 0;
}
char *CXMLAnalysis::Today()
{
	
	time_t t;
	static char buf[9];
	
	time(&t);
#ifndef _AIX32_THREADS
	struct tm *tt;
	tt=localtime(&t);
	sprintf(buf,"%4d%02d%02d",\
		1900+tt->tm_year,tt->tm_mon+1,tt->tm_mday);
#else
	struct tm tt;
	localtime_r(&t,&tt);
	sprintf(buf,"%4d%02d%02d",\
		1900+tt.tm_year,tt.tm_mon+1,tt.tm_mday);
#endif
	
	return(buf);
}
void CXMLAnalysis::AddSlash(char *path)
{
	int len;
	
	len = strlen(path);
	if (len == 0)
	{
		strcpy(path,".");
	}
	else
	{
		if (path[len-1]=='\\'||path[len-1]=='/')
			return;
	}
#ifdef WIN32
	strcat(path,"\\");
#else
	strcat(path,"/");
#endif
}
void CXMLAnalysis::makedir(char *path)
{
	char rpath[260];
	int i,l;
	AddSlash(path);
	l = strlen(path);
	for (i=0;i<l;i++) 
	{
		if (path[i]=='\\'||path[i]=='/') 
		{
			memcpy(rpath,path,i);
			rpath[i]='\0';
			if (access(rpath,0)!=0) 
			{
#ifdef WIN32
				mkdir(rpath);
#else
				mkdir(rpath,0777);
#endif
			}
			
		}
	}
}

bool CXMLAnalysis::FindEndNode(int bOffset,int eOffset,std::string nodename, int &vend,int &nextnodebegin)
{
	int nodenameBoffset=0;
	int nodenameEndoffset=0;
	int nnodeNameLen=0;
	
	char *tmpbuf=NULL;
	
	bool isendnode;
	std::string sendflag="/";
	char *snameattr=NULL;
	std::string snodenamefind;
	char *pxmlbuf=  m_xmldatabuf+bOffset;
	int noffset=bOffset;
	int buflen= eOffset;
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
			
			snameattr = (char *)malloc(nnodeNameLen+1);
			memset(snameattr,0,nnodeNameLen+1);
			memcpy(snameattr,m_xmldatabuf+nodenameBoffset,nnodeNameLen);//�õ��ڵ����Ƽ�����
			
			if (nnodeNameLen == nodename.length()+1)
			{
				if (m_xmldatabuf[nodenameBoffset] != '/') //</xx> ������ȵ�����/��ʼ�ģ�������
				{
					//�Ƶ�>����  
					pxmlbuf++;
					noffset++;
					free(snameattr);
					snameattr = NULL;
					continue;
				}
				if (strncmp(m_xmldatabuf+nodenameBoffset+1,nodename.c_str(),nodename.length())== 0) //�ҵ�һ��������־
				{
					nNameNum--; //���Ʊ�־��������
					if (nNameNum < 0 ) //�����Ʊ�־С��0ʱ����ʱƥ�䣬�����ҵ���
					{
						vend = nodenameBoffset-1;
						nextnodebegin = nodenameEndoffset+1;
						bOffset = nextnodebegin;
						free(snameattr);
						snameattr = NULL;
						return true;
					}
				}
				//���ǽ�����־
				//�Ƶ�>����  
				pxmlbuf++;
				noffset++;
				free(snameattr);
				snameattr = NULL;
				continue;
			}
			//ȡ�ýڵ����ƿ��Ƿ�ͽڵ�������ͬ
			if (!GetNodeName(snameattr,nnodeNameLen,snodenamefind))//�õ��ڵ�����	
			{
				free(snameattr);
				snameattr = NULL;
				return false;//�޽ڵ�����
			}
			free(snameattr);
			snameattr = NULL;
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
bool CXMLAnalysis::GetNodeName(const char *buffer,int buflen, std::string &nodename)
{
	
	char *pbuf=NULL;
	char *cnodename=NULL;
	
	int nBoffset=0;
	int nEoffset=0;
	
	pbuf = (char *)buffer;
	while (*pbuf == SPACE || *pbuf == TABLESPACE) //�ҵ��ո���
	{
		pbuf++;
		nBoffset++;
		if (nBoffset>=buflen)
		{
			sprintf(m_errMsg,"0-%dȫ�ǿո񣬲��ǽڵ�����",nBoffset);
			LogMp(4,__FILE__,__LINE__,buffer,buflen,m_errMsg);
			return false;
		}
	}
	nEoffset = nBoffset;
	for (int i=nBoffset;i<buflen;i++)
	{
		pbuf++;
		nEoffset++;
		if (*pbuf == SPACE ||  *pbuf == TABLESPACE)
		{
			break;
		}
	}
	cnodename = (char *)malloc(nEoffset - nBoffset+2);
	if (cnodename ==NULL)
	{
		return false;
	}
	memset(cnodename,0,nEoffset - nBoffset+2);
	memcpy(cnodename,buffer+nBoffset,nEoffset - nBoffset);
	nodename = cnodename;
	free(cnodename);
	return true;
}