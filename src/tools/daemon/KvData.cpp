// KvData.cpp: implementation of the CKvData class.
//
//////////////////////////////////////////////////////////////////////

#include "KvData.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKvData::CKvData()
{
	
	bzero(m_sErrMsg,sizeof(m_sErrMsg));
	m_bIsRequestPack = true;
	m_pLog.SetLogPara(LOG_WARNNING,"","kvparse.log");
	
}

CKvData::~CKvData()
{

}

bool CKvData::FromBuffer(std::string data)
{
	if (!ParseHead(data))
	{
		return false;
	}
	if (m_bIsRequestPack)
	{
		if (data.length() > FIXEDREQHEADLEN+1)
		{
			m_pDataNode.m_sKvValue = data.substr(FIXEDREQHEADLEN+1,data.length()-FIXEDREQHEADLEN-1);
		}
		else
		{
			sprintf(m_sErrMsg,"只有固定报文头，无分隔数据");
			return false;
		}
		
	}
	else
	{
		if (data.length() > FIXEDRSPHEADLEN+1)
		{
			m_pDataNode.m_sKvValue = data.substr(FIXEDRSPHEADLEN+1,data.length()-FIXEDRSPHEADLEN-1);
		}
		else
		{
			sprintf(m_sErrMsg,"只有固定报文头，无分隔数据");
			return false;
		}
	}
	return m_pDataNode.ParseNode();
}
bool CKvData::ParseNode(std::string data)
{
	m_pDataNode.m_sKvValue = data;
	return m_pDataNode.ParseNode();
}
bool CKvData::ParseHead(std::string data)
{
	if (data.length()<FIXEDRSPHEADLEN)
	{
		sprintf(m_sErrMsg,"数据长度小于76");
		return false;
	}
	bzero(&m_kvHead,sizeof(m_kvHead));
	char *p = (char *)data.c_str();
	
	memcpy(m_kvHead.m_seq_no,p,8);
	CBF_Tools::LRtrim(m_kvHead.m_seq_no);
	
	memcpy(m_kvHead.m_msg_type,p+8,1);
	CBF_Tools::LRtrim(m_kvHead.m_msg_type);
	
	memcpy(m_kvHead.m_exch_code,p+9,10);
	CBF_Tools::LRtrim(m_kvHead.m_exch_code);
	
	memcpy(m_kvHead.m_msg_flag,p+19,1);
	CBF_Tools::LRtrim(m_kvHead.m_msg_flag);
	if (strncmp(m_kvHead.m_msg_flag,"1",1) == 0)
	{
		m_bIsRequestPack = true; //请求数据包
	}
	else if (strncmp(m_kvHead.m_msg_flag,"2",1) == 0)
	{
		m_bIsRequestPack = false; //应答数据包
	}
	else
	{
		sprintf(m_sErrMsg,"报文标识[%s]不符",m_kvHead.m_msg_flag);
		return false;
	}
	memcpy(m_kvHead.m_term_type,p+20,2);
	CBF_Tools::LRtrim(m_kvHead.m_term_type);
	
	memcpy(m_kvHead.m_user_type,p+22,2);
	CBF_Tools::LRtrim(m_kvHead.m_user_type);
	
	memcpy(m_kvHead.m_user_id,p+24,20);
	CBF_Tools::LRtrim(m_kvHead.m_user_id);
	
	memcpy(m_kvHead.m_area_code,p+44,4);
	CBF_Tools::LRtrim(m_kvHead.m_area_code);
	
	memcpy(m_kvHead.m_branch_id,p+48,20);
	CBF_Tools::LRtrim(m_kvHead.m_branch_id);
	if (m_bIsRequestPack)
	{
		if (data.length() < FIXEDREQHEADLEN)
		{
			sprintf(m_sErrMsg,"数据长度小于%d",FIXEDREQHEADLEN);
			return false;
		}
		memcpy(m_kvHead.m_c_teller_id1,p+68,20);
		CBF_Tools::LRtrim(m_kvHead.m_c_teller_id1);

		memcpy(m_kvHead.m_c_teller_id2,p+88,20);
		CBF_Tools::LRtrim(m_kvHead.m_c_teller_id2);
	}
	else
	{
		memcpy(m_kvHead.m_rsp_code,p+68,8);
		CBF_Tools::LRtrim(m_kvHead.m_rsp_code);
	}
	return true;

}

bool CKvData::ToString(std::string &data)
{
	return m_pDataNode.ToString(data);
}
CKVNode* CKvData::GetValueByName(std::string name,std::string &svalue)
{
	svalue = "";
	CKVNode *tmpnode = m_pDataNode.m_firstChildNode;
	if (tmpnode == NULL)
	{
		return NULL;
	}
	CKVNode *datanode=NULL;
	while  (tmpnode != NULL)
	{
		if (tmpnode->m_firstChildNode != NULL)
		{
			datanode = tmpnode->m_firstChildNode;
			if (datanode->m_sKvName.compare(name) == 0)
			{
				datanode->ToString(svalue);
				return datanode;
			}
		}
		
		tmpnode = tmpnode->m_nextNode;
	}
	return NULL;
}

CKVNode*  CKvData::GetNodeByName(std::string name)
{
	CKVNode *tmpnode = m_pDataNode.m_firstChildNode;
	if (tmpnode == NULL)
	{
		return NULL;
	}
	CKVNode *datanode=NULL;
	while  (tmpnode != NULL)
	{
		if (tmpnode->m_firstChildNode != NULL)
		{
			datanode = tmpnode->m_firstChildNode;
			if (datanode->m_sKvName.compare(name) == 0)
			{
				return datanode;
			}
		}
		tmpnode = tmpnode->m_nextNode;
	}
	return NULL;
}



bool CKvData::GetValueByNameOne(std::string name,std::string &svalue, int &col, int &line)
{
	svalue = "";
	CKVNode *tmpnode = m_pDataNode.m_firstChildNode;
	if (tmpnode == NULL)
	{
		m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"没有1层节点");
		return false;
	}
	CKVNode *datanode=NULL;
	while  (tmpnode != NULL)
	{
		if (tmpnode->m_firstChildNode != NULL)
		{
			datanode = tmpnode->m_firstChildNode;
			if (datanode->m_sKvName.compare(name) == 0)
			{
//				m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"找到[%s]节点 子节点数[%d] name[%s] value[%s]",\
//					name.c_str(),datanode->m_nDataCount,datanode->m_sKvName.c_str(),datanode->m_sKvValue.c_str());
				return datanode->ToStringOne(svalue,col,line);
			}
		}
		
		tmpnode = tmpnode->m_nextNode;
	}
	return false;
}

CKVNode * CKvData::SetNodeByName(std::string name,int nodetype)
{
	CKVNode *addnode = NULL;
	CKVNode *tmpnode = m_pDataNode.m_firstChildNode; //#分隔的
	if (tmpnode == NULL)
	{
		addnode = new CKVNode(); 
		addnode->m_nDataCount = 0;
		addnode->m_nLevel = 1;//#分隔
		addnode->m_nKvType = KVTYPE_NAMEVALUE;
		m_pDataNode.AddChildNode(addnode);
		tmpnode = addnode;
		addnode = new CKVNode(); 
		addnode->m_nDataCount = 0;
		addnode->m_nLevel = 2;//=分隔
		addnode->m_nKvType = nodetype;
		addnode->m_sKvName = name;
		tmpnode->AddChildNode(addnode);
		return addnode;
	}
	CKVNode *datanode=NULL;
	while  (tmpnode != NULL)
	{
		if (tmpnode->m_firstChildNode != NULL)
		{
			datanode = tmpnode->m_firstChildNode;
			if (datanode->m_sKvName.compare(name) == 0)
			{
				datanode->DeleteAllChild();
				return datanode;
			}
		}
		tmpnode = tmpnode->m_nextNode;
	}
	addnode = new CKVNode(); 
	addnode->m_nDataCount = 0;
	addnode->m_nLevel = 1;//#分隔
	addnode->m_nKvType = KVTYPE_NAMEVALUE;
	m_pDataNode.AddChildNode(addnode);
	tmpnode = addnode;
	addnode = new CKVNode(); 
	addnode->m_nDataCount = 0;
	addnode->m_nLevel = 2;//=分隔
	addnode->m_nKvType = nodetype;
	addnode->m_sKvName = name;
	tmpnode->AddChildNode(addnode);
	return addnode;
	
}

bool CKvData::fromBuffer(char *buf)
{
	std::string data(buf);
	return FromBuffer(data);
}


bool CKvData::toBuffer(char *buf, unsigned int size)
{
	memset(buf, 0, size);
	memset(buf, ' ', FIXEDREQHEADLEN);
	strncpy(buf, m_kvHead.m_seq_no, strlen(m_kvHead.m_seq_no));
	strncpy(buf+ 8, m_kvHead.m_msg_type, strlen(m_kvHead.m_msg_type));
	strncpy(buf+ 9, m_kvHead.m_exch_code, strlen(m_kvHead.m_exch_code));
	strncpy(buf+ 19, m_kvHead.m_msg_flag, strlen(m_kvHead.m_msg_flag));
	strncpy(buf+ 20, m_kvHead.m_term_type, strlen(m_kvHead.m_term_type));
	strncpy(buf+ 22, m_kvHead.m_user_type, strlen(m_kvHead.m_user_type));
	strncpy(buf+ 24, m_kvHead.m_user_id, strlen(m_kvHead.m_user_id));
	strncpy(buf+ 44, m_kvHead.m_area_code, strlen(m_kvHead.m_area_code));
	strncpy(buf+ 48, m_kvHead.m_branch_id, strlen(m_kvHead.m_branch_id));
	if (m_bIsRequestPack)
	{
		strncpy(buf+ 68, m_kvHead.m_c_teller_id1, strlen(m_kvHead.m_c_teller_id1));
		strncpy(buf+ 88, m_kvHead.m_c_teller_id2, strlen(m_kvHead.m_c_teller_id2));
	}
	else
	{
		strncpy(buf+ 68, m_kvHead.m_rsp_code, strlen(m_kvHead.m_rsp_code));
	}

	std::string sPack;
	if(!ToString(sPack))
	{
		return false;
	}
	if (m_bIsRequestPack)
	{
		if (sPack.length()+1+ FIXEDREQHEADLEN> size)
		{
			return false;
		}
		sprintf(buf+ FIXEDREQHEADLEN,"#%s",sPack.c_str());
	}
	else
	{
		if (sPack.length()+1+ FIXEDRSPHEADLEN> size)
		{
			return false;
		}
		sprintf(buf+ FIXEDRSPHEADLEN,"#%s",sPack.c_str());
	}
	return true;
}


int CKvData::setHead(std::string key, std::string svalue)
{
	unsigned int itemlen[12]= {8, 1, 10, 1, 2, 2, 20, 4, 20, 20, 20, 8};
	char* item[12]= {m_kvHead.m_seq_no, m_kvHead.m_msg_type, m_kvHead.m_exch_code, m_kvHead.m_msg_flag, 
		m_kvHead.m_term_type, m_kvHead.m_user_type, m_kvHead.m_user_id, m_kvHead.m_area_code, m_kvHead.m_branch_id, 
		m_kvHead.m_c_teller_id1, m_kvHead.m_c_teller_id2, m_kvHead.m_rsp_code};
	std::string itemstr[12]= {"seq_no", "msg_type", "exch_code", "msg_flag", "term_type", "user_type", "user_id", "area_code", "branch_id", "c_teller_id1", "c_teller_id2", "rsp_code"};
	
	//先判断消息类型是请求还是应答
	if (key.compare(itemstr[3]) == 0)
	{
		CBF_Tools::StringCopy(m_kvHead.m_msg_flag,1,svalue.c_str());
		if (strncmp(m_kvHead.m_msg_flag,"1",1) == 0)
		{
			m_bIsRequestPack = true; //请求数据包
		}
		else if (strncmp(m_kvHead.m_msg_flag,"2",1) == 0)
		{
			m_bIsRequestPack = false; //应答数据包
		}
		else
		{
			sprintf(m_sErrMsg,"报文标识[%s]不符",m_kvHead.m_msg_flag);
			return -1;
		}
		return 0;
	}
	for (unsigned int i=0; i< sizeof(itemstr)/sizeof(std::string); i++)
	{
		if (key.compare(itemstr[i])== 0)
		{
			if (strlen(svalue.c_str()) > itemlen[i]) 
			{
				return -1;
			}
			strcpy(item[i], svalue.c_str());
			return 0;
		}
	}
	return -1;
}


int CKvData::setPack(std::string key, std::string svalue, bool flag/*= true*/)
{
	CKVNode *pKVNode=  SetNodeByName(key, 20);
	if(pKVNode == NULL)
	{
		return -1;
	}
	pKVNode->m_sKvValue= svalue;
	pKVNode->ParseNode();
	return 0;
}


int CKvData::setPackX(std::string key, std::string svalue, int line/*= 0*/, int col/*= 0*/, bool flag/*= true*/)
{
	CKVNode *pKVNode= NULL;
	pKVNode= SetNodeByName(key, 20);
	if(pKVNode == NULL)
	{
		return -1;
	}
	int ret = stringConvertX2K(pKVNode,svalue, line, col);
	
	return  ret;
}


bool CKvData::getHead(std::string key, std::string &svalue)
{
	CBF_Tools tools;
	char* item[12]= {m_kvHead.m_seq_no, m_kvHead.m_msg_type, m_kvHead.m_exch_code, m_kvHead.m_msg_flag,
		m_kvHead.m_term_type, m_kvHead.m_user_type, m_kvHead.m_user_id, m_kvHead.m_area_code, 
		m_kvHead.m_branch_id, m_kvHead.m_c_teller_id1, m_kvHead.m_c_teller_id2, m_kvHead.m_rsp_code};
	std::string itemstr[12]= {"seq_no", "msg_type", "exch_code", "msg_flag", "term_type", "user_type", "user_id", "area_code", "branch_id", "c_teller_id1", "c_teller_id2", "rsp_code"};
	for (unsigned int i=0; i< sizeof(item)/sizeof(char*); i++)
	{
		if (key.compare(itemstr[i]) == 0)
		{
			std::string tempstr= std::string(item[i]);
			svalue = tools.LRtrim(tempstr);
			return true;
		}
	}
	return false;
}

bool CKvData::getPack(std::string key, std::string &svalue, int row/*= 0*/, int col/*= 0*/)
{
	int i;
	CKVNode *pNode= GetNodeByName(key);
	if(pNode== NULL)
	{
		sprintf(m_sErrMsg,"没有此节点[%s]",key.c_str());
		return false;
	}
	if (pNode->m_nKvType== KVTYPE_HASHTABLE)
	{	
		if (pNode->m_firstChildNode == NULL)
		{
			sprintf(m_sErrMsg,"[%s]哈希表没有表头",key.c_str());
			return false;
		}
		pNode= pNode->m_firstChildNode->m_nextNode;

		if(pNode == NULL)
		{
			sprintf(m_sErrMsg,"[%s]哈希表没有数据记录",key.c_str());
			return false;
		}
		pNode= pNode->m_firstChildNode;

		if (pNode == NULL)
		{
			sprintf(m_sErrMsg,"[%s]哈希表没有数据记录",key.c_str());
			return false;
		}
		for (i= 0; i< row; i++)
		{
			pNode = pNode->m_nextNode;
			if (pNode == NULL)
			{
				sprintf(m_sErrMsg,"[%s]哈希表没有指定记录[%d]的值",key.c_str(),row);
				return false;
			}
		}

		if (pNode->m_firstChildNode == NULL)
		{
			return false;
		}
		pNode= pNode->m_firstChildNode;
		for (i= 0; i< col; i++)
		{
			pNode = pNode->m_nextNode;
			if (pNode == NULL)
			{
				sprintf(m_sErrMsg,"[%s]哈希表没有指定记录[%d]对应的列[%d]值",key.c_str(),row,col);
				return false;
			}	
		}
		svalue = pNode->m_sKvValue;
		return true;
	}
	else if (pNode->m_nKvType == KVTYPE_ARRAYLIST)
	{
		if (pNode->m_firstChildNode == NULL)
		{
			sprintf(m_sErrMsg,"[%s]列表没有记录",key.c_str());
			return false;
		}
		pNode= pNode->m_firstChildNode;

		for (i= 0; i< row; i++)
		{
			pNode = pNode->m_nextNode;
			if (pNode == NULL)
			{
				sprintf(m_sErrMsg,"[%s]列表对应[%d]行的记录",key.c_str(),row);
				return false;
			}		
		}

		if (pNode->m_firstChildNode== NULL)
		{
			sprintf(m_sErrMsg,"[%s]列表对应[%d]行无数据列",key.c_str(),row);
			return false;
		}
		pNode= pNode->m_firstChildNode;

		for (i= 0; i< col; i++)
		{
			pNode = pNode->m_nextNode;
			if (pNode == NULL)
			{
				sprintf(m_sErrMsg,"[%s]列表对应[%d]行无[%d]数据列",key.c_str(),row,col);
				return false;
			}
			
		}
		svalue = pNode->m_sKvValue;
		return true;
	}
	else
	{
		svalue= pNode->m_sKvValue;
		return true;
	}
}





bool CKvData::getPackH(std::string key, std::string &svalue, int row/*= 0*/, int col/*= 0*/)
{
	CKVNode *pNode= GetNodeByName(key);
	if(pNode == NULL)
	{
		return false;
	}

	if (pNode->m_nKvType == KVTYPE_ARRAYLIST)
	{
		if(pNode->m_firstChildNode == NULL)
		{
			sprintf(m_sErrMsg,"[%s]列表对无数据项",key.c_str());
			return false;
		}
		pNode= pNode->m_firstChildNode;
	}
	else if (pNode->m_nKvType == KVTYPE_HASHTABLE)
	{
		if (row == 0)
		{
			if(pNode->m_firstChildNode== NULL)
			{
				sprintf(m_sErrMsg,"[%s]哈希表无表头",key.c_str());
				return false;
			}
			pNode= pNode->m_firstChildNode;
		}
		else
		{
			if(pNode->m_firstChildNode->m_nextNode== NULL)
			{
				sprintf(m_sErrMsg,"[%s]哈希表无记录",key.c_str());
				return false;
			}
			//PNODE为记录
			pNode= pNode->m_firstChildNode->m_nextNode;
			
			if (pNode->m_firstChildNode== NULL)
			{
				sprintf(m_sErrMsg,"[%s]哈希表无记录的行节点",key.c_str());
				return false;
			}
			//pnode指向记录第一行
			pNode= pNode->m_firstChildNode;
			row --;
		}
	}
	else
	{	
		svalue= pNode->m_sKvValue;
		return true;
	}
	//找到对应的行
	while(row>0)
	{
		row--;
		pNode= pNode->m_nextNode;
		if(pNode == NULL)
		{
			sprintf(m_sErrMsg,"[%s]哈希表或列表无指定的行[%d]",key.c_str(),row);
			return false;
		}
	}
	//pnode为指定的行
	if(pNode->m_firstChildNode == NULL)
	{
		sprintf(m_sErrMsg,"[%s]哈希表或列表指定的行[%d]无数据列",key.c_str(),row);
		return false;
	}
	//pnode为指定行的第一列
	pNode= pNode->m_firstChildNode;
	//找到指定的列
	while (col>0)
	{
		col--;
		pNode= pNode->m_nextNode;
		if (pNode == NULL)
		{
			sprintf(m_sErrMsg,"[%s]哈希表或列表指定的行[%d]无数据列[%d]",key.c_str(),row,col);
			return false;
		}
	}
	svalue = pNode->m_sKvValue;

	return true;
}


bool CKvData::getPackX(std::string key, std::string &svalue, int &row, int &col)
{
	return GetValueByNameOne(key, svalue, col, row);
}
void CKvData::clear()
{
	bzero(&m_kvHead,sizeof(m_kvHead));
	m_pDataNode.DeleteAllChild();
}

int CKvData::stringConvertX2K(CKVNode *namenode,std::string &svalue, int line, int col)
{
	CBF_Slist publog;
	int ret;
	bool ishas=false;
	int offset;
	publog.Clear();
	publog.SetSeparateString(ONESEPSTRING);
	ret = publog.FillChnSepString(svalue);
	if (ret < 1)
	{
		sprintf(m_sErrMsg,"没有数据");
		return -1;
	}
	if (line *col < ret)
	{
		ishas = true;
		if ((line+1)*col != ret )
		{
			sprintf(m_sErrMsg,"数据非法,行列值不等总的数值");
			return -1;
		}
		namenode->m_nKvType = KVTYPE_HASHTABLE;
	}
	else
	{
		if (line*col == ret)
		{
			ishas = false;
		}
		else
		{
			sprintf(m_sErrMsg,"数据非法,行列值不等总的数值");
			return -1;
		}
		namenode->m_nKvType = KVTYPE_ARRAYLIST;
	}
	CKVNode *addnode = NULL;
	CKVNode *colnode = NULL;
	CKVNode *rownode = NULL;
	if (ishas )
	{
		//加列头
		addnode = new CKVNode();
		if (addnode == NULL)
		{
			return -1;
		}
		addnode->m_nLevel = namenode->m_nLevel+1;
		addnode->m_nKvType = KVTYPE_DATA;
		namenode->AddChildNode(addnode);
		for (int i=0 ; i< col ; i++)
		{
			colnode = new CKVNode();
			colnode->m_nKvType = KVTYPE_DATA;
			colnode->m_nLevel = addnode->m_nLevel+1;
			colnode->m_sKvValue = publog.GetAt(i);
			addnode->AddChildNode(colnode);
		}
		//加记录
		addnode = new CKVNode();
		if (addnode == NULL)
		{
			return -1;
		}
		addnode->m_nLevel = namenode->m_nLevel+1;
		addnode->m_nKvType = KVTYPE_DATA;
		namenode->AddChildNode(addnode);
		for (int j=0 ; j < line ; j++)
		{
			rownode = new CKVNode();
			if (rownode == NULL)
			{
				return -1;
			}
			rownode->m_nKvType = KVTYPE_DATA;
			rownode->m_nLevel = addnode->m_nLevel+1;
			addnode->AddChildNode(rownode);
			for (int i=0 ; i< col ; i++)
			{
				colnode = new CKVNode();
				colnode->m_nKvType = KVTYPE_DATA;
				colnode->m_nLevel = rownode->m_nLevel+1;
				offset = (j+1)*col;
				colnode->m_sKvValue = publog.GetAt(offset+i);
				rownode->AddChildNode(colnode);
			}
		}
	}
	else
	{
		//加记录
		addnode = new CKVNode();
		if (addnode == NULL)
		{
			return -1;
		}
		addnode->m_nLevel = namenode->m_nLevel+1;
		addnode->m_nKvType = KVTYPE_DATA;
		namenode->AddChildNode(addnode);
		for (int j=0 ; j < line ; j++)
		{
			rownode = new CKVNode();
			if (rownode == NULL)
			{
				return -1;
			}
			rownode->m_nKvType = KVTYPE_DATA;
			rownode->m_nLevel = addnode->m_nLevel+1;
			addnode->AddChildNode(rownode);
			for (int i=0 ; i< col ; i++)
			{
				colnode = new CKVNode();
				colnode->m_nKvType = KVTYPE_DATA;
				colnode->m_nLevel = rownode->m_nLevel+1;
				colnode->m_sKvValue = publog.GetAt(j*col+i);
				rownode->AddChildNode(colnode);
			}
		}
	}
	return 0;
	
}


