// InterfacePack.cpp: implementation of the CInterfacePack class.
//
//////////////////////////////////////////////////////////////////////

#include "InterfacePack.h"
#include "XmlPack.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInterfacePack::CInterfacePack()
{
	m_headnode = NULL;
	m_funclistnode = NULL;
	m_rootnode = NULL;
	m_transnode = NULL;
	m_txcodenode = NULL;
	memset(m_errMsg,0,sizeof(m_errMsg));
	m_publog.SetLogPara(LOG_DEBUG,"","packconv.log");
}

CInterfacePack::~CInterfacePack()
{
	
}
bool CInterfacePack::fromBuf(char *xmlBuf)
{
	if (m_xml.fromBuffer(xmlBuf))
	{
		m_rootnode = m_xml.getRoot();
		if (!readPack())
		{
			m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"数据非xml接口包格式");
			return false;
		}
		return true;
	}
	else
	{
		m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"数据非xml格式");
		m_rootnode = NULL;
		return false;
	}
}

bool CInterfacePack::readPack()
{
	//回到根节点
	m_xml.getRoot();
	//获得第一个子节点，当前节点为子节点
	CNode *tmpnode = m_xml.getFirstChildItem();
	if (tmpnode==NULL)
	{
		m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"xml数据包没有数据项");
		return false;
	}
	//第一个子节点是否为interface,不是则表示xml包不符
	if (strcmp(tmpnode->getName().c_str(),"interface")!=0) 
	{
		m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"xml数据包没有interface项");
		return false;
	}
	//确定是合法xml包信息，取包头及包体
	//取得当前节点的子节点里名为"func_list"的节点,当前节点为func_list
	m_funclistnode = m_xml.getFirstChildItem("func_list");
	if (m_funclistnode==NULL)
	{
		m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"xml数据包没有func_list数据项");
		return false;
	}
	m_xml.getParentItem();
	m_transnode =   m_xml.getFirstChildItem("翻译方法");
	
	m_xml.getParentItem();
	m_headnode = m_xml.getFirstChildItem("包头");

	m_xml.getParentItem();
	m_txcodenode = m_xml.getFirstChildItem("交易码");

	//取交易码节点
	resetField(&m_txcode_info);
	if (m_txcodenode!=NULL)
	{
		getField(m_txcodenode,&m_txcode_info);
	}
	if (m_transnode!=NULL)
	{
		//翻译节点
		if(!readTrans(m_transnode))
		{
			m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"翻译节点出错");
			return false;
		}
	}
	if 	(m_headnode!=NULL)
	{
		//包头节点
		if(!readHead(m_headnode))
		{
			m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"include节点出错");
			return false;
		}
	}
	if 	(m_funclistnode!=NULL)
	{
		//功能节点
		if (!readFunc(m_funclistnode))
		{
			m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"功能节点出错");
			return false;
		}	
	}
	return true;
}
char *CInterfacePack::getMsg()
{
	return m_errMsg;
}

int CInterfacePack::fromFile(char *filename)
{
	if (m_xml.fromFile(filename)<0)
	{
		m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"文件[%s]非xml格式",filename);
		return -1;
	}
	if (!readPack())
	{
		m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"xml文件[%s]非接口转换xml格式",filename);
		return -1;
	}
	return 0;
}

int CInterfacePack::trim(char *s)
{
	int i;
	char tm[4096],*str;

	str=s;
	for(i=0;*str!=0;str++)
		if(*str!=' '&&*str!=10&&*str!=13)
			tm[i++]=*str;
	tm[i]=0;
	strcpy(s,tm);
	return(0);
}
char *CInterfacePack::rtrim(char *str)
{
	int i,j;
	for(i=0,j=-1;*(str+i)!=0;i++)
	{
		if(*(str+i)!=' ')j=i;
	}
	*(str+j+1)=0;
	return(str);
}

char *CInterfacePack::ltrim(char *str)
{
	char *p,*q;
	q=p=str;
	for(;*p!=0;p++)
		if(*p!=' ')break;
	if(p!=q)
	{
		for(;*p!=0;p++,q++)
			*q=*p;
		*q=0;
	}
	return(str);
}	
int CInterfacePack::trim(std::string &s)
{	
	int pos;
	while ((pos=s.find(' ', 0)) != -1)	
	{
		s.erase(pos, 1);	
	}
	return 0;
}

bool CInterfacePack::readFunc(CNode *funclistnode)
{
	if (funclistnode==NULL)
	{
		sprintf(m_errMsg,"功能节点为空");
		return false;
	}
	std::string value;
	//转到功能节点
	m_xml.setCurrentNode(funclistnode);
	CNode *funcNode = m_xml.getFirstChildItem();
	while (funcNode != NULL)
	{
		//读一个功能配置
		S_FUNC_CONF_INFO funcinfo;
		funcinfo.funcAttr.conv_type = 0;
		funcinfo.funcAttr.funcNo = 0;
		memset(funcinfo.funcAttr.separator,0,sizeof(funcinfo.funcAttr.separator));
		//读功能号的信息
		std::string ss = funcNode->getName();
		funcinfo.funcAttr.funcNo = atoi(ss.c_str()+1);
		CAttribute *attr = funcNode->getAttributes();
		if (attr==NULL)
		{
			sprintf(m_errMsg,"属性为空");
			return false;
		}
		
		//转换类型
		if (funcNode->GetAttributeValue("conv",value) !=0)
		{
			sprintf(m_errMsg,"功能[%s]转换属性conv没有设置",ss.c_str());
			return false;
		}
		if (strncmp(value.c_str(),"xml",3)==0)
		{
			funcinfo.funcAttr.conv_type = 0;
		}
		else if (strncmp(value.c_str(),"xpack",5)==0)
		{
			funcinfo.funcAttr.conv_type = 1;
		}
		else if (strncmp(value.c_str(),"separator",9)==0)
		{
			funcinfo.funcAttr.conv_type = 2;
		}
		else if (strncmp(value.c_str(),"standard",8)==0)
		{
			funcinfo.funcAttr.conv_type = 3;
		}
		else
		{
			funcinfo.funcAttr.conv_type =0 ;
		}
		if (funcinfo.funcAttr.conv_type==2)
		{
			if (funcNode->GetAttributeValue("separator",value) !=0)
			{
				sprintf(m_errMsg,"功能号[%d]分隔符未定义",funcinfo.funcAttr.funcNo);
				return false;
			}
			if (value.length()<1)
			{
				sprintf(m_errMsg,"功能号[%d]分隔符未定义",funcinfo.funcAttr.funcNo);
				return false;
			}
			if (value.length()>2)
			{
				strncpy(funcinfo.funcAttr.separator,value.c_str(),2);
			}
			else 
			{
				strcpy(funcinfo.funcAttr.separator,value.c_str());
			}
			
		}
		
		
		//请求段
		//到功能号节点
		m_xml.setCurrentNode(funcNode);

		CNode *reqNode = m_xml.getFirstChildItem("请求");
		if (reqNode!=NULL)
		{
			m_xml.setCurrentNode(reqNode);
			CNode *fieldNode = m_xml.getFirstChildItem();
			while (fieldNode!=NULL)
			{
				S_FUNC_FIELD_INFO tmpfield;
				resetField(&tmpfield);
				if (!getField(fieldNode,&tmpfield))
				{
					sprintf(m_errMsg,"读取节点[%s]错",reqNode->m_name.c_str());
					return false;
				}
				funcinfo.funcReq.funcReq.push_back(tmpfield);
				fieldNode = fieldNode->getNextSibling();
			}
		}
		//返回段
		//到功能号节点
		m_xml.setCurrentNode(funcNode);
		CNode *resNode = m_xml.getFirstChildItem("请求");
		resNode = resNode->getNextSibling();
		while(resNode!=NULL)
		{
			S_FUNC_RES_FIELD_LIST resList;
			memset(&resList,0,sizeof(S_FUNC_RES_FIELD_LIST));
			CAttribute *tmpatt = resNode->getAttributes();
			if (tmpatt != NULL)
			{
				if (resNode->GetAttributeValue("code",value) == 0)
				{
					resList.retCode = atoi(value.c_str());
				}
				
/* *** Updated by songfree at 2006-8-1 17:21:09 ****** 
	增加返回段的包转换类型,即返回包可能是另外一种包。
*******************************************************/
				if (resNode->GetAttributeValue("conv",value) == 0)
				{
					if (strncmp(value.c_str(),"xml",3)==0)
					{
						resList.conv_type  = 0;
					}
					else if (strncmp(value.c_str(),"xpack",5)==0)
					{
						resList.conv_type = 1;
					}
					else if (strncmp(value.c_str(),"separator",9)==0)
					{
						resList.conv_type = 2;
					}
					else if (strncmp(value.c_str(),"standard",8)==0)
					{
						resList.conv_type = 3;
					}
					else
					{
						resList.conv_type =0 ;
					}
				}
				else //如果返回段没有定义，则和交易定义一致
				{
					resList.conv_type = funcinfo.funcAttr.conv_type ;
				}
				if (resList.conv_type == 2)
				{
					//如果分隔的话，取分隔符
					if (resNode->GetAttributeValue("separator",value) == 0)
					{
						if (value.length()<1)
						{
							sprintf(m_errMsg,"功能号[%d]分隔符未定义",funcinfo.funcAttr.funcNo);
							return false;
						}
						if (value.length()>2)
						{
							strncpy(resList.separator,value.c_str(),2);
						}
						else 
						{
							strcpy(resList.separator,value.c_str());
						}
					}
				}
				//后续包是否写到文件
				if (resNode->GetAttributeValue("file",value) == 0)
				{
					resList.fileFlag = atoi(value.c_str());
				}
				//两种返回的处理所包含的包头
				if (resNode->GetAttributeValue("include",value) == 0)
				{
					strcpy(resList.include,value.c_str());
				}

	/* *** Updated by songfree at 2006-8-1 17:29:42 end****** 
		增加返回段的包转换类型,即返回包可能是另外一种包   end  
	*******************************************************/
				
			}
			
		

			m_xml.setCurrentNode(resNode);
			CNode *fieldNode = m_xml.getFirstChildItem();
			while (fieldNode!=NULL)
			{
				S_FUNC_FIELD_INFO tmpfield;
				resetField(&tmpfield);
				if (!getField(fieldNode,&tmpfield))
				{
					sprintf(m_errMsg,"读取节点[%s]错",reqNode->m_name.c_str());
					return false;
				}
				resList.funcRes.push_back(tmpfield);
				fieldNode = fieldNode->getNextSibling();
			}

			resNode = resNode->getNextSibling();
			funcinfo.funcRes.push_back(resList);
		}
		funcinfo.funcReq.retNum = funcinfo.funcRes.size();
		m_func_conf_info.push_back(funcinfo);
		funcNode = funcNode->getNextSibling();
	}
	return true;
}

bool CInterfacePack::readHead(CNode *readnode)
{
	m_xml.setCurrentNode(readnode);
	//取此包头下一子节点
	CNode *tmpheadnode = m_xml.getFirstChildItem();
	while (tmpheadnode!=NULL)
	{
		S_PACKHEAD_INFO tmpheadinfo;//头信息
		//取包头的名字
		tmpheadinfo.headName = tmpheadnode->getName();
		m_xml.setCurrentNode(tmpheadnode);
		//取包头的定义
		CNode *tmpnode = m_xml.getFirstChildItem();
		S_FUNC_FIELD_INFO tmppack;

		while (tmpnode!=NULL)
		{
			tmppack.fieldlength = 0;
			tmppack.fieldtype =0;
			tmppack.left = 0;
			resetField(&tmppack);
			if (!getField(tmpnode,&tmppack))
			{
				sprintf(m_errMsg,"读取节点[%s]错",tmpheadinfo.headName.c_str());
				return false;
			}
			tmpheadinfo.headLength += tmppack.fieldlength ;
			tmpnode = tmpnode->getNextSibling();
			tmpheadinfo.headinfo.push_back(tmppack);
		}
		tmpheadnode = tmpheadnode->getNextSibling();
		m_include_head.push_back(tmpheadinfo);
	}
	return true;
}
bool CInterfacePack::readTrans(CNode *transnode)
{
	std::string value;
	m_xml.setCurrentNode(transnode);
	//取此包头下一子节点
	CNode *tmpheadnode = m_xml.getFirstChildItem();
	while (tmpheadnode!=NULL)
	{
		S_TRANS_INFO tmptransinfo;//翻译信息
		//取翻译的名字
		tmptransinfo.transName = tmpheadnode->getName();
		if (tmpheadnode->GetAttributeValue("conv",value) == 0)
		{
			tmptransinfo.conv = value;
		}
		else
		{
			tmptransinfo.conv = "";
		}
		
		
		m_xml.setCurrentNode(tmpheadnode);
		//取翻译的定义
		CNode *tmpnode = m_xml.getFirstChildItem();
		S_FUNC_FIELD_INFO tmppack;
		while (tmpnode!=NULL)
		{
			resetField(&tmppack);
			if (!getField(tmpnode,&tmppack))
			{
				sprintf(m_errMsg,"读取节点[%s]错",tmptransinfo.transName.c_str());
				return false;
			}
			tmpnode = tmpnode->getNextSibling();
			tmptransinfo.transinfo.push_back(tmppack);
		}
		//目前计算方法只支持两个值的加减乘除
		if (strncmp(tmptransinfo.conv.c_str(),"1",1)==0)
		{
			if (tmptransinfo.transinfo.size()==3)
			{
				m_trans_info.push_back(tmptransinfo);
			}
			else
			{
				tmptransinfo.transinfo.clear();
			}
		}
		tmpheadnode = tmpheadnode->getNextSibling();
		m_trans_info.push_back(tmptransinfo);
	}
	return true;
}
bool CInterfacePack::getField(CNode *fieldnode,S_FUNC_FIELD_INFO *fieldpack)
{
	if (fieldnode==NULL)
	{
		sprintf(m_errMsg,"节点为空");
		return false;
	}
	fieldpack->name = fieldnode->getName();
	fieldpack->value = fieldnode->getValue();
	CAttribute *attr;
	attr = fieldnode->getAttributes();
	if (attr==NULL)
	{
		sprintf(m_errMsg,"属性为空");
		fieldpack->fieldtype =-1;
		fieldpack->fieldlength = -1;
		fieldpack->left = -1;
		fieldpack->isTrans = false;
		fieldpack->isupdate = false;
		fieldpack->isVar = false;
		return true;
	}
	std::string ss;
	if (fieldnode->GetAttributeValue("type",ss)== 0)
	{
		if (strcmp(ss.c_str(),"char")==0)
		{
			fieldpack->fieldtype = 0;
		}
		else if (strcmp(ss.c_str(),"integer")==0)
		{
			fieldpack->fieldtype = 1;
		}
		else if (strcmp(ss.c_str(),"double")==0)
		{
			fieldpack->fieldtype = 2;
		}
		else
		{
			fieldpack->fieldtype = 0;
		}
	}
	if (fieldnode->GetAttributeValue("length",ss)== 0)
	{
		fieldpack->fieldlength = atoi(ss.c_str());
	}
	if (fieldnode->GetAttributeValue("left",ss)== 0)
	{
		if (strncmp(ss.c_str(),"yes",3)==0)
		{
			fieldpack->left = 0;
		}
		else if (strncmp(ss.c_str(),"no",2)==0)
		{
			fieldpack->left = 1;
		}
		else
		{
			fieldpack->left = 0;
		}
	}
	if (fieldnode->GetAttributeValue("trans",ss)== 0)
	{
		if (strncmp(ss.c_str(),"1",1)==0)
		{
			fieldpack->isTrans = true;
		}
		else
		{
			fieldpack->isTrans = false;
		}
	}
	else
	{
		fieldpack->isTrans = false;
	}
	if (fieldnode->GetAttributeValue("var",ss)== 0)
	{
		if (strncmp(ss.c_str(),"~",1)==0)
		{
			fieldpack->isVar = false;
			fieldpack->transValue = (ss.c_str()+1);
		}
		else if (strncmp(ss.c_str(),"@",1)==0)
		{
			fieldpack->isVar = true;
			fieldpack->transValue = (ss.c_str()+1);
		}
		else
		{
			fieldpack->isVar = false;
		}
	}
	else
	{
		fieldpack->isTrans = false;
	}
	if (fieldnode->GetAttributeValue("update",ss)== 0)
	{
		if (strncmp(ss.c_str(),"y",1)==0)
		{
			fieldpack->isupdate = true;
		}
		else if (strncmp(ss.c_str(),"n",1)==0)
		{
			fieldpack->isupdate = false;
		}
		else
		{
			fieldpack->isupdate = false;
		}
	}
	else
	{
		fieldpack->isupdate = false;
	}
/* *** Updated by songfree at 2006-8-1 17:02:15 ****** 
	增加定长包的位移定义
*******************************************************/

	if (fieldnode->GetAttributeValue("offset",ss)== 0)
	{
		fieldpack->fieldoffset = atoi(ss.c_str());
	}
	else
	{
		fieldpack->fieldoffset = 0;
	}
	//转换类型

	if (fieldnode->GetAttributeValue("conv",ss)== 0)
	{
		if (strncmp(ss.c_str(),"xml",3)==0)
		{
			fieldpack->conv_type = 0;
		}
		else if (strncmp(ss.c_str(),"xpack",5)==0)
		{
			fieldpack->conv_type = 1;
		}
		else if (strncmp(ss.c_str(),"separator",9)==0)
		{
			fieldpack->conv_type = 2;
		}
		else if (strncmp(ss.c_str(),"standard",8)==0)
		{
			fieldpack->conv_type = 3;
		}
	}
	//分隔符
	if (fieldnode->GetAttributeValue("separator",ss)== 0)
	{
		if (ss.length()>2)
		{
			strncpy(fieldpack->separator,ss.c_str(),2);
		}
		else
		{
			strcpy(fieldpack->separator,ss.c_str());
		}
	}
	//去除空格
	if (fieldnode->GetAttributeValue("trim",ss)== 0)
	{
		if (atoi(ss.c_str())==1)
		{
			fieldpack->isTrim = true;
		}
		else
		{
			fieldpack->isTrim = false;
		}
	}
	//小数位数
	if (fieldnode->GetAttributeValue("dec",ss)== 0)
	{
		fieldpack->dec = atoi(ss.c_str());
	}
	//include段
	if (fieldnode->GetAttributeValue("include",ss)== 0)
	{
		fieldpack->include = ss;
	}
	return true;
}

S_FUNC_CONF_INFO *CInterfacePack::getFuncInfo(int txcode)
{
	for (int i=0;i<m_func_conf_info.size();i++)
	{
		if (m_func_conf_info[i].funcAttr.funcNo == txcode)
		{
			return &(m_func_conf_info[i]);
		}
	}
	return NULL;
}

S_PACKHEAD_INFO *CInterfacePack::getHeadInfo(std::string headname)
{
	for (int i=0;i<m_include_head.size();i++)
	{
		if (m_include_head[i].headName.compare(headname)==0)
		{
			return &(m_include_head[i]);
		}
	}
	return NULL;
}

std::string CInterfacePack::getTransString(std::string transname, std::string sourceValue)
{
	std::string ss;
	for (int i=0;i<m_trans_info.size();i++)
	{
		if (m_trans_info[i].transName.compare(transname)==0)
		{
			if (atoi(m_trans_info[i].conv.c_str())==0)
			{
				for (int j=0;j<m_trans_info[i].transinfo.size();j++)
				{
					if (m_trans_info[i].transinfo[j].name.compare(sourceValue)==0)
					{
						return m_trans_info[i].transinfo[j].value;
					}
					if (m_trans_info[i].transinfo[j].name.compare("default")==0)
					{
						ss = m_trans_info[i].transinfo[j].value;
					}
				}
			}
			
		}
	}
	return ss;
}

std::string CInterfacePack::getTransConv(string transname)
{
	for (int i=0;i<m_trans_info.size();i++)
	{
		if (m_trans_info[i].transName.compare(transname)==0)
		{
			return m_trans_info[i].conv;
		}
	}
	return "";
}

int CInterfacePack::getTxCode(char *buf, int buflen, int &txcode)
{
	CBF_Slist publog;
	if (m_txcode_info.fieldoffset <0) 
	{
		sprintf(m_errMsg,"取交易码定义出错，offset不能小于0");
		return -1;
	}	
	if (m_txcode_info.conv_type == 2) //分隔符
	{
		char separator[3];
		bzero(separator,sizeof(separator));
		if (CBF_Tools::IsNumber(m_txcode_info.separator))
		{
			separator[0] = atoi(m_txcode_info.separator);
		}
		else
		{
			separator[0] = m_txcode_info.separator[0];
		}
		publog.SetSeparateString(separator);
		publog.FillSepString(buf);
		if (m_txcode_info.fieldoffset >= publog.GetCount())
		{
			sprintf(m_errMsg,"数据包无交易码[%d]",m_txcode_info.fieldoffset);
			return -1;
		}
		txcode = atoi(publog.GetAt(m_txcode_info.fieldoffset,true).c_str());
	}
	if (m_txcode_info.conv_type == 3) //定长
	{
		if (m_txcode_info.fieldlength<1)
		{
			sprintf(m_errMsg,"交易码长度[%d]不符",m_txcode_info.fieldlength);
			return -1;
		}
		char tmpchar[200];
		memset(tmpchar,0,sizeof(tmpchar));
		memcpy(tmpchar,buf+m_txcode_info.fieldoffset,m_txcode_info.fieldlength);
		txcode = atoi(tmpchar);
	}
	return 0;
}

bool CInterfacePack::resetField(S_FUNC_FIELD_INFO *field)
{
	field->conv_type = 0;
	field->dec = 0;
	field->fieldlength = 0;
	field->fieldoffset = 0;
	field->fieldtype = 0;
	field->isTrans = false;
	field->isTrim = true;
	field->isupdate = false;
	field->isVar = false;
	field->left = 0;
	memset(field->separator,0,sizeof(field->separator));
	field->name = "";
	field->value = "";
	field->transValue = "";
	return true;
}
