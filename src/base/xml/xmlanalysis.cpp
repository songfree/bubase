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
	m_logLevel = 5;///日志级别，大于此级别的日志将不写入日志文件0严重错误1重要错误2一般错误3警告4提示警告5调试信息
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
			/*-------------去掉属性节点值的引号------------
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
		/*------------去掉属性节点值的引号----------------
		strcpy(hAttribute->m_value,itemName);
		-------------------------------------------------*/
		itemName[strlen(itemName)-1] = 0;
		hAttribute->m_value = (itemName+1);
		//-------------------------------------------------
	}
}

CNode *CXMLAnalysis::analyseXML2Old(const char * buffer,int bufferlen, CNode * parentNode)
{
	CNode *curNode=NULL;//当前节点
	CNode *prevNode=NULL;//上一节点
	int    nodeCnt=NULL;//节点计数
	char  *pbuf=NULL;
	char  *tbuf=NULL;
	char   curN[ITEMNAMELENGTH+1];  //节点名称包含属性
	char   curName[ITEMNAMELENGTH+1]; //节点名称<里的>
	char   curEndName[ITEMNAMELENGTH+1]; //节点名称</里的>
	int    i=0;
	bool   isSubItem=false; //是否子节点
	char   parentValue[ITEMVALUELENGTH+1];
	int    ivalue=0;
	int    icur=0;
	memset(curN,0,sizeof(curN));
	memset(curName,0,sizeof(curName));
	memset(curEndName,0,sizeof(curEndName));
	memset(parentValue,0,sizeof(parentValue));

	char *tmbuf=NULL; //指向xml的缓冲
	tmbuf = (char *)buffer;

	//此处大文件xml会出错 songfree 20090122
	std::string curBuffer;
	


	pbuf=tmbuf;//tmbuf为去掉注释后的数据

	//开始分析除去注释后的xml
	for(nodeCnt=0;*pbuf!=0;)
	{
		while(*pbuf!=0)
		{
			//## 找到数据项名称
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

				//得到一个数据项 <>里的
				while (curName[ii-1] == 9 || curName[ii-1] == 32)
				{
					ii--;
				}
				curName[ii] = 0;
				strcpy(curN,curName);
				int j=0;
				//得到节点名称
				for(;j<ii;j++)
				{
					if(curName[j]==' ' || curName[j]=='=')
					{
						curName[j]=0;
						break;
					}
				}
				//如果在'>'前面有/表示得到一个空数据项 songfree add
				pbuf--;
				if (*pbuf=='/')
				{
					//生成节点
					//## 增加数据项节点
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
		//此处大文件xml会出错 songfree 20090122
		curBuffer = std::string("");



		//获取节点数据
		while(*pbuf != 0)
		{

			tbuf = pbuf + 1;//tbuf置为节点名后面的数据
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
				//取节点名
				for(i = 0; *tbuf !=0 && *tbuf != '>'; tbuf++)
				{
					curEndName[i++]=*tbuf;
				}
				while (curEndName[i - 1] == 9 || curEndName[i - 1] == 32) 
				{
					i--;
				}
				curEndName[i]=0;
				//## 找到数据项结束处
				
				if(strcmp(curName,curEndName)==0)
				{
					//## 增加数据项节点
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




					//## 分析下一层数据报

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
			//此处大文件xml时会出错
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
	CNode *curNode=NULL;//当前节点
	char  *pbuf=NULL;
	bool   isSubItem=true; 
	int bOffset=0;
	int eOffset=0;
	int nodenameBoffset=0;
	int nodenameEndoffset=0;
	bOffset = beginoffset;
	eOffset = endoffset;

	char sNodeNameAttr[NODENAMEATTRLEN + 1];//节点名称及属性
	char sNodeEndChar[NODENAMELEN+1];//节点名称及属性
	char sNodeName[NODEVALUELEN+1];//节点值
	memset(sNodeNameAttr,0,sizeof(sNodeNameAttr));
	memset(sNodeEndChar,0,sizeof(sNodeEndChar));
	memset(sNodeName,0,sizeof(sNodeName));

	pbuf = (char *)m_xmldatabuf;//指向xml的缓冲
	pbuf +=beginoffset;

	char *tmpbuf=NULL; //指向xml的缓冲

//	publog.LogBin(0,__FILE__,__LINE__,"调试xml",pbuf,endoffset-beginoffset);
	
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
		if ( *pbuf == '<' )//找到<符号了
		{
			//往后移，如果后面是空格的话一直往后移
			do 
			{
				pbuf++;
				bOffset++;
				if (*pbuf == 0 )
				{
					return NULL;
				}
				
			} while (*pbuf == SPACE || *pbuf == TABLESPACE);
			nodenameBoffset = bOffset;//节点名称开始位置
			nodenameEndoffset = bOffset;
			//找到匹配的 '>'号 得到节点名称结束位置nodenameendoffset
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
					//LogMp(10,__FILE__,__LINE__,m_xmldatabuf+nodenameBoffset,nodenameEndoffset-nodenameBoffset,"无节点数据或不匹配 位置%d-%d之间",nodenameBoffset,nodenameEndoffset);
					return NULL;//无节点数据或不匹配
				}
			}
			int nnodeNameLen = nodenameEndoffset - nodenameBoffset;
			if (nnodeNameLen > NODENAMEATTRLEN) //节点名称及属性超过定义的长度
			{
				//此节点不做解析,直接解析后面的
				sprintf(m_errMsg,"节点名称及属性长度超限%ld %ld",nodenameEndoffset - nodenameBoffset,NODENAMEATTRLEN);
				printf("%s\n",m_errMsg);
				LogMp(4,__FILE__,__LINE__,"",0,m_errMsg);
				continue;//while (*pbuf != 0)
			}
			tmpbuf = pbuf;
			tmpbuf--;
			if (*tmpbuf=='/') //如果节点名紧接着是/，即<name/>的形式，要去除/
			{
				memset(sNodeNameAttr,0,sizeof(sNodeNameAttr));
				memcpy(sNodeNameAttr,m_xmldatabuf+nodenameBoffset,nnodeNameLen-1);
			}
			else
			{
				memset(sNodeNameAttr,0,sizeof(sNodeNameAttr));
				memcpy(sNodeNameAttr,m_xmldatabuf+nodenameBoffset,nnodeNameLen);
			}
			//生成节点
			curNode=new CNode;
			if (!AnalyseNodeAttr(sNodeNameAttr,nnodeNameLen,curNode))//分析节点名及属性
			{
				printf("%s\n",m_errMsg);//分析出错，继续后面的
				delete curNode;
				LogMp(4,__FILE__,__LINE__,"",0,m_errMsg);
				continue;// while (*pbuf != 0)
			}
			tmpbuf = pbuf;
			tmpbuf--;
			if (*tmpbuf=='/') //如果在'>'前面有/表示得到一个空数据项
			{
				pbuf++;
				bOffset++;
				if (*pbuf == 0)
				{
					delete curNode;
					LogMp(4,__FILE__,__LINE__,"",0,"%d /后无数据",bOffset);
					return NULL;
				}
				//将分析出来的节点加到 parentNode 节点
				if (!addNode(parentNode,curNode))
				{
					printf("%s\n",m_errMsg);//分析出错，继续后面的
					LogMp(4,__FILE__,__LINE__,"",0,m_errMsg);
					continue;//while (*pbuf != 0)
				}
				parentNode->m_value = "";//父节点的值置为空
				continue;//while (*pbuf != 0)
			}
			//移到>后面
			pbuf++;
			bOffset++;
			if (*pbuf == 0)
			{
				delete curNode;
				LogMp(4,__FILE__,__LINE__,"",0,"%d >后无数据，即只有<>没有</>",bOffset);
				return NULL;
			}
			//去查找匹配的</xxx>


//songfree 20100614	
			int vend=0;
			int nextnodebegin=0;
			if (!FindEndNode(bOffset,eOffset,curNode->m_name,vend,nextnodebegin))
			{
				delete curNode;
				return NULL;
			}
			//找到匹配的了
			//将分析出来的节点加到 parentNode 节点
			if (!addNode(parentNode,curNode))
			{
				printf("%s\n",m_errMsg);//分析出错，继续后面的
				LogMp(4,__FILE__,__LINE__,"",0,m_errMsg);
				return NULL;//while (*pbuf != 0)
			}
			if (vend - bOffset <1)
			{
				curNode->m_value = "";
				pbuf = pbuf+nextnodebegin-bOffset;
				bOffset = nextnodebegin;	
				//						publog.LogBin(0,__FILE__,__LINE__,"调试xml",pbuf,endoffset-bOffset);
				isSubItem = false;
			}
			else
			{
				analyseXML2(bOffset,vend,curNode);
				pbuf = pbuf+nextnodebegin-bOffset;
				bOffset = nextnodebegin;	
				//						publog.LogBin(0,__FILE__,__LINE__,"调试xml",pbuf,endoffset-bOffset);
				isSubItem = false;
			}
			continue;


// 			sprintf(sNodeEndChar,"</%s>",curNode->m_name.c_str());
// 			int nNodeValueLen=0;//节点值的长度(包含子节点)
// 			int nNodeoEndffset=0;//此节点的结束长度
// 			nNodeoEndffset = curNode->m_name.length()+3;
// 			isSubItem = true;
// //			publog.LogBin(0,__FILE__,__LINE__,"调试xml",m_xmldatabuf+bOffset,endoffset-bOffset);
// //			publog.LogBin(0,__FILE__,__LINE__,"调试xml",pbuf,endoffset-bOffset);
// 			
// 			tmpbuf = pbuf;
// 			
// 			for (eOffset = bOffset ;eOffset < endoffset ; eOffset++)
// 			{
// 				if (strncmp(tmpbuf,sNodeEndChar,nNodeoEndffset) == 0)
// 				{
// 					//找到匹配的了
// 					//将分析出来的节点加到 parentNode 节点
// 					if (!addNode(parentNode,curNode))
// 					{
// 						printf("%s\n",m_errMsg);//分析出错，继续后面的
// 						LogMp(4,__FILE__,__LINE__,"",0,m_errMsg);
// 						continue;//while (*pbuf != 0)
// 					}
// 					//再次分析里面的xml子数据
// //					publog.LogBin(0,__FILE__,__LINE__,"调试xml",m_xmldatabuf+bOffset,eOffset-bOffset);
// //					publog.LogBin(0,__FILE__,__LINE__,"调试xml",pbuf,eOffset-bOffset);
// 					if (eOffset - bOffset <1)
// 					{
// 						curNode->m_value = "";
// 						pbuf = pbuf+nNodeoEndffset;
// 						bOffset = bOffset+nNodeoEndffset;	
// //						publog.LogBin(0,__FILE__,__LINE__,"调试xml",pbuf,endoffset-bOffset);
// 						isSubItem = false;
// 					}
// 					else
// 					{
// 						analyseXML2(bOffset,eOffset,curNode);
// 						pbuf = pbuf+ eOffset-bOffset+nNodeoEndffset;
// 						bOffset = eOffset+nNodeoEndffset;	
// //						publog.LogBin(0,__FILE__,__LINE__,"调试xml",pbuf,endoffset-bOffset);
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
// 			//没有找到匹配的
// 			LogMp(10,__FILE__,__LINE__,"",0,"没有和此节点匹配的数据<%s>",curNode->m_name.c_str());
// 			delete curNode;
// 			return NULL;
		}
		else //if ( *pbuf == '<' ) //说明是节点的值
		{
			if (parentNode->m_parentNode == NULL)
			{
				//如果父节点为空，则表示则节点为顶节点，可以直接返回了
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
				sprintf(m_errMsg,"节点值超限%ld > %ld",eOffset - bOffset,NODEVALUELEN);
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
	//如果父节点为空则返回空
	if ( parentNode==NULL )
	{
		return NULL;
	}
	if(0 == buffer[0] || bufferlen<1) //没有数据
	{
		parentNode->m_value = std::string("");
		return parentNode;
	}
	m_xmldatabuf = (char *)malloc(bufferlen+1); 
	if (m_xmldatabuf == NULL)
	{
		sprintf(m_errMsg,"分配%ld的内存数据出错",bufferlen+1);
		return NULL;
	}
	memset(m_xmldatabuf,0,bufferlen+1);
//	LogMp(4,__FILE__,__LINE__,buffer,bufferlen,"去掉注释前的xml数据");
	memcpy(m_xmldatabuf,buffer,bufferlen);
	deleteRemark(m_xmldatabuf);

	int buflen = strlen(m_xmldatabuf);
//	LogMp(4,__FILE__,__LINE__,m_xmldatabuf,strlen(m_xmldatabuf),"去掉注释后的xml数据");
	//分析去掉注释后的xml
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
	char  *tmpbuffer;//临时指向buffer的指针
	//去掉注释部分,把xml包体放到tmbuf
	pbuf=buffer;
	tmpbuffer = (char *)buffer;
	for(;*tmpbuffer!=0;tmpbuffer++)
	{
		if(*tmpbuffer != 8)
		{
			//-------去掉注释部分------------------------------------
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
								tmpbuffer += 1; //当版本后面没有字符直接跟着xml数据时加1
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
								tmpbuffer += 2; //当注释后面没有字符直接跟着xml数据时加2
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
	while (*pbuf == SPACE || *pbuf == TABLESPACE) //找到空格了
	{
		pbuf++;
		nBoffset++;
		if (nBoffset>=buflen)
		{
			sprintf(m_errMsg,"0-%d全是空格，不是节点名称",nBoffset);
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
		sprintf(m_errMsg,"节点名称长度超限，%d 大于设定值",nEoffset - nBoffset,NODENAMELEN);
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
		if (desnode->m_firstChildNode != NULL) //不是第一个子节点
		{
			desnode->m_firstChildNode->m_lastNode = newnode;//原第一节点的上节点为此节点
			newnode->m_nextNode=desnode->m_firstChildNode; //此节点的临近下节点为原第一子节点
		}
		else
		{
			desnode->m_endChildNode = newnode; 
			newnode->m_nextNode = NULL;
		}
		newnode->m_parentNode=desnode;//父节点为当前节点
		desnode->m_firstChildNode=newnode;//当前节点的第一个子节点为此节点
	}
	else
	{
		newnode->m_nextNode = NULL;
		if (desnode->m_endChildNode != NULL) //不是最后子节点
		{
			desnode->m_endChildNode->m_nextNode = newnode;//原最后一节点的下节点为此节点
			newnode->m_lastNode=desnode->m_endChildNode; //此节点的临近上节点为原最后一子节点
		}
		else
		{
			desnode->m_firstChildNode = newnode; 
			newnode->m_lastNode = NULL;
		}
		newnode->m_parentNode=desnode;//父节点为当前节点
		desnode->m_endChildNode=newnode;//当前节点的最后一个子节点为此节点
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
	//分析属性
	int aValueBegin=0;
	int aValueEnd =0;


//	publog.LogBin(0,__FILE__,__LINE__,"调试xml",pbuf,buflen);
	//去掉 name=""前面的空格或TAB
	while (*pbuf == SPACE || *pbuf == TABLESPACE) //找到空格了
	{
		pbuf++;
		aValueBegin++;
		if (aValueBegin>=buflen)
		{
			sprintf(m_errMsg,"没有属性");
			LogMp(4,__FILE__,__LINE__,m_errMsg,strlen(m_errMsg),"");
			return false;
		}
	}
//	publog.LogBin(0,__FILE__,__LINE__,"调试xml",pbuf,buflen-aValueBegin);

	int attrnamelen=0;
	aValueEnd = aValueBegin;
	while (aValueEnd<buflen)
	{
		
		if (*pbuf == '=') //遇到等号了
		{
			attrnamelen =  aValueEnd - aValueBegin;
			if (attrnamelen<1) 
			{
				//遇到非法的属性了 只有=或者大于规定的长度，要去掉后面的" "里的东西
				//还是直接退出吧，省得再去解析
				sprintf(m_errMsg,"遇到非法的属性了 只有=或者大于规定的长度");
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
					sprintf(m_errMsg,"遇到非法的属性了 名称及=后面没有内容");
					LogMp(4,__FILE__,__LINE__,m_errMsg,strlen(m_errMsg),"");
					return false;
				}
				if (*pbuf != '"') //不是双引号，属性非法
				{
					//还是直接退出吧，省得再去解析
					if (attrname != NULL)
					{
						curnode->SetNewAttribute(attrname,"");
					}
					sprintf(m_errMsg,"遇到非法的属性了 名称及=后面不是引号");
					LogMp(4,__FILE__,__LINE__,m_errMsg,strlen(m_errMsg),"");
					return false;
				}
				

				aValueEnd++;
				pbuf++;
				if (aValueEnd>=buflen)
				{
					curnode->SetNewAttribute(attrname,"");
					sprintf(m_errMsg,"遇到非法的属性了 =后无内容");
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
						sprintf(m_errMsg,"遇到非法的属性了 =\"后无内容");
						LogMp(4,__FILE__,__LINE__,m_errMsg,strlen(m_errMsg),"");
						free(attrname);
						attrname = NULL;
						return false;
					}
				}
//				publog.LogBin(0,__FILE__,__LINE__,"调试xml",pbuf,buflen-aValueEnd);
				int attrvaluelen =0;
				attrvaluelen = aValueEnd - aValueBegin;
				if (attrvaluelen <1) //属性为空
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
//				publog.LogBin(0,__FILE__,__LINE__,"调试xml",pbuf,buflen-aValueEnd);
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
	//0严重错误1重要错误2一般错误3警告4提示警告5调试信息
	switch (level)
	{
	case 0:
		sprintf(buffer,"严重 ");
		break;
	case 1:
		sprintf(buffer,"重要 ");
		break;	
	case 2:
		sprintf(buffer,"一般 ");
		break;	
	case 3:
		sprintf(buffer,"警告 ");
		break;	
	case 4:
		sprintf(buffer,"提示警告 ");
		break;	
	case 5:
		sprintf(buffer,"调试信息 ");
		break;	
	default:
		sprintf(buffer,"其它%d ",level);
		break;
	}
	
	sprintf(fpathname,"%s/%s%s",m_logFilePath,Today(),m_logFileName);
	fp=fopen(fpathname,"a");
	if(fp==NULL)
	{
		return -1;
	}

	fprintf(fp,"%s 文件[%s]行[%d] %s ",Get_Date(),fname,line,buffer);

	fprintf(fp,"\n 数据长度%d[",datalen);
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
			
			snameattr = (char *)malloc(nnodeNameLen+1);
			memset(snameattr,0,nnodeNameLen+1);
			memcpy(snameattr,m_xmldatabuf+nodenameBoffset,nnodeNameLen);//得到节点名称及属性
			
			if (nnodeNameLen == nodename.length()+1)
			{
				if (m_xmldatabuf[nodenameBoffset] != '/') //</xx> 长度相等但不是/开始的，重新找
				{
					//移到>后面  
					pxmlbuf++;
					noffset++;
					free(snameattr);
					snameattr = NULL;
					continue;
				}
				if (strncmp(m_xmldatabuf+nodenameBoffset+1,nodename.c_str(),nodename.length())== 0) //找到一个结束标志
				{
					nNameNum--; //名称标志计数减少
					if (nNameNum < 0 ) //当名称标志小于0时，此时匹配，终于找到了
					{
						vend = nodenameBoffset-1;
						nextnodebegin = nodenameEndoffset+1;
						bOffset = nextnodebegin;
						free(snameattr);
						snameattr = NULL;
						return true;
					}
				}
				//不是结束标志
				//移到>后面  
				pxmlbuf++;
				noffset++;
				free(snameattr);
				snameattr = NULL;
				continue;
			}
			//取得节点名称看是否和节点名称相同
			if (!GetNodeName(snameattr,nnodeNameLen,snodenamefind))//得到节点名称	
			{
				free(snameattr);
				snameattr = NULL;
				return false;//无节点名称
			}
			free(snameattr);
			snameattr = NULL;
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
bool CXMLAnalysis::GetNodeName(const char *buffer,int buflen, std::string &nodename)
{
	
	char *pbuf=NULL;
	char *cnodename=NULL;
	
	int nBoffset=0;
	int nEoffset=0;
	
	pbuf = (char *)buffer;
	while (*pbuf == SPACE || *pbuf == TABLESPACE) //找到空格了
	{
		pbuf++;
		nBoffset++;
		if (nBoffset>=buflen)
		{
			sprintf(m_errMsg,"0-%d全是空格，不是节点名称",nBoffset);
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