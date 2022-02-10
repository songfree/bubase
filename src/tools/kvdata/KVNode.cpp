// KVNode.cpp: implementation of the CKVNode class.
//
//////////////////////////////////////////////////////////////////////

#include "KVNode.h"

std::string g_sSepStringArray[] = {"#","=","∧","｜","ˇ"};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKVNode::CKVNode()
{
	m_nKvType = KVTYPE_POP;
	m_nLevel = 0;
	m_sKvName = "";
	m_sKvValue = "";
	m_parentNode = NULL;///父节点指针
	
    m_nextNode = NULL; ///临近下节点指针
	m_lastNode = NULL; ///临近上节点指针
	
    m_firstChildNode = NULL;///第一个子节点指针
	m_endChildNode = NULL;///最后一个子节点指针
	m_nDataCount = 0;
	m_pLog.SetLogPara(LOG_WARNNING,"","kvparse.log");
}

CKVNode::~CKVNode()
{
	DeleteAllChild();
	DeletePoint();
}

bool CKVNode::ParseNode()
{
	int i;
	std::string stmp;
	CKVNode *tmpnode=NULL;
	CBF_Slist      m_pSlist;
	DeleteAllChild();
	int nDataCount=0;
	if (m_nLevel == 0)  //"#"分隔
	{
		m_pSlist.SetSeparateString(g_sSepStringArray[0].c_str());
		m_pSlist.Clear();
		nDataCount = m_pSlist.FillChnSepString(m_sKvValue);
		if (nDataCount >0)
		{
			
			for (i=0 ; i<nDataCount ; i++ )
			{
				tmpnode = new CKVNode();
				if (tmpnode == NULL)
				{
					m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"new CKVNode出错");
					return false;
				}
				tmpnode->m_nLevel = m_nLevel+1;
				tmpnode->m_nKvType = KVTYPE_NAMEVALUE;  //置为kv的格式
				tmpnode->m_sKvValue = m_pSlist.GetAt(i,true);//value
				m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"LEVEL[%d] TYPE[%d] 节点值 %s",tmpnode->m_nLevel,tmpnode->m_nKvType,tmpnode->m_sKvValue.c_str());
				AddChildNode(tmpnode);
				if (!tmpnode->ParseNode())
				{
					m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"解析节点出错 %s",tmpnode->m_sKvValue.c_str());
					return false;
				}
			}
			//有子节点的话，此值置为空，不占用空间
			m_sKvValue = "";
		}
		else
		{
			m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"解析节点出错,无#分隔数据 %s",m_sKvValue.c_str());
			return false;
		}

	}
	else if (m_nLevel == 1) //"="分隔
	{
		m_pSlist.SetSeparateString(g_sSepStringArray[m_nLevel].c_str()); //"="分隔
		m_pSlist.Clear();
		nDataCount = m_pSlist.FillChnSepString(m_sKvValue+"=");
		if (nDataCount >= 2)
		{
			tmpnode = new CKVNode();
			if (tmpnode == NULL)
			{
				m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"new CKVNode出错");
				return false;
			}
			tmpnode->m_nLevel = m_nLevel+1;
			tmpnode->m_sKvName = m_pSlist.GetAt(0,true);//value
			tmpnode->m_sKvValue = m_pSlist.GetAt(1,true);//value
			//判断节点是什么
//			tmpnode->m_nKvType = tmpnode->GetNodeType(tmpnode->m_sKvValue);
// 			m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"LEVEL[%d] TYPE[%d] 节点名[%s]节点值[%s]",\
// 				tmpnode->m_nLevel,tmpnode->m_nKvType,tmpnode->m_sKvName.c_str(),tmpnode->m_sKvValue.c_str());
			AddChildNode(tmpnode);
			if (!tmpnode->ParseNode())
			{
				m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"解析节点出错 %s",tmpnode->m_sKvValue.c_str());
				return false;
			}
			m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"LEVEL[%d] TYPE[%d] 节点名[%s]节点数据[%d]",\
				tmpnode->m_nLevel,tmpnode->m_nKvType,tmpnode->m_sKvName.c_str(),tmpnode->m_nDataCount);
			//有子节点的话，此值置为空，不占用空间
			m_sKvValue = "";
		}
		else
		{
			m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"解析节点出错,无=分隔数据 %s",m_sKvValue.c_str());
			return false;
		}
	}
	else if (m_nLevel == 2) //"∧"分隔
	{
		m_nKvType = GetNodeType(m_sKvValue);
		m_pSlist.SetSeparateString(g_sSepStringArray[m_nLevel].c_str()); //"∧"分隔
		m_pSlist.Clear();
		nDataCount = m_pSlist.FillChnSepString(m_sKvValue);
		if (m_nKvType == KVTYPE_HASHTABLE)
		{
			m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"数据[%s]∧分隔的数据项 %d" ,m_sKvValue.c_str(),nDataCount);
		}
		
		if (nDataCount>0 && !m_pSlist.m_bNotSepString)
		{
			for (i=0 ; i<nDataCount ; i++ )
			{
				tmpnode = new CKVNode();
				if (tmpnode == NULL)
				{
					m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"new CKVNode出错");
					return false;
				}
				tmpnode->m_nLevel = m_nLevel+1;
				tmpnode->m_nKvType = KVTYPE_DATA;  //置为kv的格式
				tmpnode->m_sKvValue = m_pSlist.GetAt(i,true);//value
				m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"LEVEL[%d] TYPE[%d] 节点值 %s",tmpnode->m_nLevel,tmpnode->m_nKvType,tmpnode->m_sKvValue.c_str());
				AddChildNode(tmpnode);
				if (!tmpnode->ParseNode())
				{
					m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"解析节点出错 %s",tmpnode->m_sKvValue.c_str());
					return false;
				}
			}
			//有子节点的话，此值置为空，不占用空间
			m_sKvValue = "";
		}
		else
		{
			m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"解析节点无∧分隔数据 %s",m_sKvValue.c_str());
			return true;
		}
	}
	else if (m_nLevel == 3) //"｜"分隔
	{
		m_pSlist.SetSeparateString(g_sSepStringArray[m_nLevel].c_str()); //"｜"分隔
		m_pSlist.Clear();
		nDataCount = m_pSlist.FillChnSepString(m_sKvValue);
		if (nDataCount>0 && !m_pSlist.m_bNotSepString)
		{
			for (i=0 ; i<nDataCount ; i++ )
			{
				tmpnode = new CKVNode();
				if (tmpnode == NULL)
				{
					m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"new CKVNode出错");
					return false;
				}
				tmpnode->m_nLevel = m_nLevel+1;
				tmpnode->m_nKvType = KVTYPE_DATA;  //置为kv的格式
				tmpnode->m_sKvValue = m_pSlist.GetAt(i,true);//value
				m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"LEVEL[%d] TYPE[%d] 节点值 %s",tmpnode->m_nLevel,tmpnode->m_nKvType,tmpnode->m_sKvValue.c_str());
				AddChildNode(tmpnode);
				if (!tmpnode->ParseNode())
				{
					m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"解析节点出错 %s",tmpnode->m_sKvValue.c_str());
					return false;
				}
			}
			//有子节点的话，此值置为空，不占用空间
			m_sKvValue = "";
		}
		else
		{
			m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"解析节点无｜分隔数据 %s",m_sKvValue.c_str());
			return true;
		}
	}
	else if (m_nLevel == 4) //"ˇ"分隔
	{
		m_pSlist.SetSeparateString(g_sSepStringArray[m_nLevel].c_str()); //"ˇ"分隔
		m_pSlist.Clear();
		nDataCount = m_pSlist.FillChnSepString(m_sKvValue);
		if (nDataCount>0 && !m_pSlist.m_bNotSepString)
		{
			for (i=0 ; i<nDataCount ; i++ )
			{
				tmpnode = new CKVNode();
				if (tmpnode == NULL)
				{
					m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"new CKVNode出错");
					return false;
				}
				tmpnode->m_nLevel = m_nLevel+1;
				tmpnode->m_nKvType = KVTYPE_DATA;  //置为kv的格式
				tmpnode->m_sKvValue = m_pSlist.GetAt(i,true);//value
				m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"LEVEL[%d] TYPE[%d] 节点值 %s",tmpnode->m_nLevel,tmpnode->m_nKvType,tmpnode->m_sKvValue.c_str());
				AddChildNode(tmpnode);
				if (!tmpnode->ParseNode())
				{
					m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"解析节点出错 %s",tmpnode->m_sKvValue.c_str());
					return false;
				}
			}
			//有子节点的话，此值置为空，不占用空间
			m_sKvValue = "";
		}
		else
		{
			m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"解析节点无ˇ分隔数据 %s",m_sKvValue.c_str());
			return true;
		}
	}
	else  //就是值了
	{
		
	}
	return true;
}

void CKVNode::AddChildNode(CKVNode *addnode)
{
	addnode->m_nextNode = NULL;
	if (m_endChildNode != NULL) //不是最后子节点
	{
		m_endChildNode->m_nextNode = addnode;//原最后一节点的下节点为此节点
		addnode->m_lastNode=m_endChildNode; //此节点的临近上节点为原最后一子节点
	}
	else
	{
		m_firstChildNode = addnode; 
		m_endChildNode = addnode;
		addnode->m_lastNode = NULL;
	}
	addnode->m_parentNode=this;//父节点为当前节点
	m_endChildNode=addnode;//当前节点的最后一个子节点为此节点

	m_nDataCount++;
	addnode->m_nLevel = m_nLevel+1;
	
}

int CKVNode::GetNodeType(std::string svalue)
{
	if (m_nLevel != 2)
	{
		return KVTYPE_DATA;
	}
	if (svalue.length() <= 4)
	{
		return KVTYPE_DATALIST;
	}
	else if (svalue.length() <=6)
	{
		return KVTYPE_DATALIST;
	}
	else
	{
		if (strncmp(svalue.c_str()+svalue.length()-6,"ˇ｜∧",6) == 0)
		{
			return KVTYPE_HASHTABLE;
		}
		else if (strncmp(svalue.c_str()+svalue.length()-4,"｜∧",4) == 0)
		{
			return KVTYPE_ARRAYLIST;
		}
		else
		{
			return KVTYPE_DATALIST;
		}
	}
}

bool CKVNode::ToString(std::string &data)
{
	CKVNode *tmpnode = NULL;
	if (m_nLevel == 0)   //"#"分隔
	{
		if (m_firstChildNode != NULL)
		{
			m_firstChildNode->ToString(data);
			data = data+g_sSepStringArray[m_nLevel];
			//第一子节点的临近节点
			tmpnode = m_firstChildNode->m_nextNode;
			while (tmpnode != NULL)
			{
				tmpnode->ToString(data);
				data = data+g_sSepStringArray[m_nLevel];
				tmpnode = tmpnode->m_nextNode;
			}
		}
	}
	else if (m_nLevel == 1)  //"="分隔
	{
		if (m_firstChildNode == NULL && m_nextNode == NULL)
		{
			data = data+m_sKvValue;
		}
		else
		{
			if (m_firstChildNode != NULL)
			{
				data = data+m_firstChildNode->m_sKvName + g_sSepStringArray[m_nLevel];
				m_firstChildNode->ToString(data);

			}
			
		}
	}
	else if (m_nLevel == 2)  //"∧"分隔
	{
		if (m_firstChildNode == NULL && m_nextNode == NULL)
		{
			data = data+m_sKvValue;
		}
		else
		{
			if (m_firstChildNode != NULL)
			{
				m_firstChildNode->ToString(data);
				data = data+g_sSepStringArray[m_nLevel];
				//第一子节点的临近节点
				tmpnode = m_firstChildNode->m_nextNode;
				while (tmpnode != NULL)
				{
					tmpnode->ToString(data);
					data = data+g_sSepStringArray[m_nLevel];
					tmpnode = tmpnode->m_nextNode;
				}
			}
			else
			{
				data = data+m_sKvValue;
			}
		}
	}
	else if (m_nLevel == 3)  //"｜"分隔
	{
		if (m_firstChildNode == NULL && m_nextNode == NULL)
		{
			data = data+m_sKvValue;
		}
		else
		{
			if (m_firstChildNode != NULL)
			{
				m_firstChildNode->ToString(data);
				data = data+g_sSepStringArray[m_nLevel];
				//第一子节点的临近节点
				tmpnode = m_firstChildNode->m_nextNode;
				while (tmpnode != NULL)
				{
					tmpnode->ToString(data);
					data = data+g_sSepStringArray[m_nLevel];
					tmpnode = tmpnode->m_nextNode;

				}
			}
			else
			{
				data = data+m_sKvValue;
			}
		}
	}
	else if (m_nLevel == 4)  //"ˇ"分隔
	{
		if (m_firstChildNode == NULL && m_nextNode == NULL)
		{
			data = data+m_sKvValue;
		}
		else
		{
			if (m_firstChildNode != NULL)
			{
				m_firstChildNode->ToString(data);
				data = data+g_sSepStringArray[m_nLevel];
				//第一子节点的临近节点
				tmpnode = m_firstChildNode->m_nextNode;
				while (tmpnode != NULL)
				{
					tmpnode->ToString(data);
					data = data+g_sSepStringArray[m_nLevel];
					tmpnode = tmpnode->m_nextNode;
				}
			}
			else
			{
				data = data+m_sKvValue;
			}
		}
	}
	else 
	{
		data = data+m_sKvValue;
	}
	return true;
}
void CKVNode::DeleteAllChild()
{
	CKVNode *tmpnode = m_firstChildNode;
	while (tmpnode != NULL)
	{
		tmpnode->DeleteAllChild();
		delete tmpnode;//析构里调用DeletePoint
		tmpnode = NULL;
		tmpnode = m_firstChildNode;
	}
	m_nDataCount =0;
}
void CKVNode::DeletePoint()
{
	
	CKVNode *parentNode= NULL;
	//去掉父节点
	parentNode = m_parentNode;
	if (parentNode != NULL) 
	{
		m_parentNode->m_nDataCount--;
		if (parentNode->m_endChildNode == this && parentNode->m_firstChildNode == this) //父节点只有它一个节点
		{
			parentNode->m_endChildNode = NULL;
			parentNode->m_firstChildNode = NULL;
			m_parentNode = NULL;
		}
		else if (parentNode->m_endChildNode == this ) //是最后一个节点
		{
			parentNode->m_endChildNode = m_lastNode;
			m_lastNode->m_nextNode = NULL;
			m_lastNode = NULL;
			m_nextNode = NULL;//最后一个节点肯定是NULL啦
		}
		else if (parentNode->m_firstChildNode == this) //是最前一个节点
		{
			parentNode->m_firstChildNode = m_nextNode;
			m_nextNode->m_lastNode = NULL;
			m_nextNode = NULL;
			m_lastNode = NULL;//最后一个节点肯定是NULL啦
		}
		else //是中间的节点
		{
			m_nextNode->m_lastNode = m_lastNode;
			m_lastNode->m_nextNode = m_nextNode;
			m_nextNode = NULL;
			m_lastNode = NULL;
		}
		
		m_parentNode = NULL;
	}
	else
	{
		if (m_nextNode == NULL && m_lastNode == NULL) //前后节点都为空，什么都不做
		{
			
		}
		else if (m_nextNode == NULL)//是最后一个节点
		{
			m_lastNode->m_nextNode = NULL;
			m_lastNode = NULL;
		}
		else if (m_lastNode == NULL) //是最前一个节点
		{
			m_nextNode->m_lastNode = NULL;
			m_nextNode = NULL;
		}
		else //是中间的节点
		{
			m_nextNode->m_lastNode = m_lastNode;
			m_lastNode->m_nextNode = m_nextNode;
			m_nextNode = NULL;
			m_lastNode = NULL;
		}
	}
}


bool CKVNode::ToStringOne(std::string &data,int &col,int &line)
{
	if (m_nLevel != 2)
	{
		m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"节点不为2层节点");
		return false;
	}
	if (m_nKvType == KVTYPE_HASHTABLE)
	{
		m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"节点为哈希表");
		if (m_firstChildNode == NULL)
		{
			col = 0;
			line = 0;
			m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"空表");
		}
		else
		{
			col = m_firstChildNode->m_nDataCount;
			m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"哈希表的列%d",col);
			if (m_firstChildNode->m_nextNode != NULL)
			{
				line = m_firstChildNode->m_nextNode->m_nDataCount;
				m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"哈希表的行%d",line);
			}
			else
			{
				line = 0;
			}
			return ToStringOne(data);
		}
		
		
	}
	else if (m_nKvType == KVTYPE_ARRAYLIST )
	{
		m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"节点为列表");
		if (m_firstChildNode == NULL)
		{
			col = 0;
			line = 0;
		}
		else
		{
			col = m_firstChildNode->m_nDataCount;
			line = m_nDataCount;
			return ToStringOne(data);
		}
	}
	else
	{
		m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"节点为值");
		return ToStringOne(data);
	}

	return true;	
}
bool CKVNode::ToStringOne(std::string &data)
{
	
	CKVNode *tmpnode = NULL;
	if (m_nLevel == 0)   //"#"分隔
	{
		
	}
	else if (m_nLevel == 1)  //"="分隔
	{

	}
	else if (m_nLevel == 2)  //"∧"分隔
	{
		if (m_firstChildNode == NULL && m_nextNode == NULL)
		{
			data = data+m_sKvValue;
			data = data+ONESEPSTRING;
		}
		else
		{
			if (m_firstChildNode != NULL)
			{
				
				m_firstChildNode->ToStringOne(data);
//				m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"[%s]",data.c_str());
				//第一子节点的临近节点
				tmpnode = m_firstChildNode->m_nextNode;
				while (tmpnode != NULL)
				{
					tmpnode->ToStringOne(data);
					tmpnode = tmpnode->m_nextNode;
				}
//				m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"[%s]",data.c_str());
			}
			else
			{
				data = data+m_sKvValue;
				data = data+ONESEPSTRING;
			}
		}
	}
	else if (m_nLevel == 3)  //"｜"分隔
	{
		if (m_firstChildNode == NULL && m_nextNode == NULL)
		{
			data = data+m_sKvValue;
			data = data+ONESEPSTRING;
		}
		else
		{
			if (m_firstChildNode != NULL)
			{
				m_firstChildNode->ToStringOne(data);

				
				//第一子节点的临近节点
				tmpnode = m_firstChildNode->m_nextNode;
				while (tmpnode != NULL)
				{
					tmpnode->ToStringOne(data);
					tmpnode = tmpnode->m_nextNode;

				}
			}
			else
			{
				data = data+m_sKvValue;
				data = data+ONESEPSTRING;
			}
		}
	}
	else if (m_nLevel == 4)  //"ˇ"分隔
	{
		if (m_firstChildNode == NULL && m_nextNode == NULL)
		{
			data = data+m_sKvValue;
			data = data+ONESEPSTRING;
		}
		else
		{
			if (m_firstChildNode != NULL)
			{
				m_firstChildNode->ToStringOne(data);
//				m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"[%s]",data.c_str());
				//第一子节点的临近节点
				tmpnode = m_firstChildNode->m_nextNode;
				while (tmpnode != NULL)
				{
					tmpnode->ToStringOne(data);
//					m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"[%s]",data.c_str());
					tmpnode = tmpnode->m_nextNode;
				}
			}
			else
			{
				data = data+m_sKvValue;
				data = data+ONESEPSTRING;
			}
		}
	}
	else 
	{
		data = data+m_sKvValue;
		data = data+ONESEPSTRING;
	}
	return true;
}


