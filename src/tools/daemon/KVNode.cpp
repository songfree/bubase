// KVNode.cpp: implementation of the CKVNode class.
//
//////////////////////////////////////////////////////////////////////

#include "KVNode.h"

std::string g_sSepStringArray[] = {"#","=","��","��","��"};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKVNode::CKVNode()
{
	m_nKvType = KVTYPE_POP;
	m_nLevel = 0;
	m_sKvName = "";
	m_sKvValue = "";
	m_parentNode = NULL;///���ڵ�ָ��
	
    m_nextNode = NULL; ///�ٽ��½ڵ�ָ��
	m_lastNode = NULL; ///�ٽ��Ͻڵ�ָ��
	
    m_firstChildNode = NULL;///��һ���ӽڵ�ָ��
	m_endChildNode = NULL;///���һ���ӽڵ�ָ��
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
	if (m_nLevel == 0)  //"#"�ָ�
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
					m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"new CKVNode����");
					return false;
				}
				tmpnode->m_nLevel = m_nLevel+1;
				tmpnode->m_nKvType = KVTYPE_NAMEVALUE;  //��Ϊkv�ĸ�ʽ
				tmpnode->m_sKvValue = m_pSlist.GetAt(i,true);//value
				m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"LEVEL[%d] TYPE[%d] �ڵ�ֵ %s",tmpnode->m_nLevel,tmpnode->m_nKvType,tmpnode->m_sKvValue.c_str());
				AddChildNode(tmpnode);
				if (!tmpnode->ParseNode())
				{
					m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ڵ���� %s",tmpnode->m_sKvValue.c_str());
					return false;
				}
			}
			//���ӽڵ�Ļ�����ֵ��Ϊ�գ���ռ�ÿռ�
			m_sKvValue = "";
		}
		else
		{
			m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ڵ����,��#�ָ����� %s",m_sKvValue.c_str());
			return false;
		}

	}
	else if (m_nLevel == 1) //"="�ָ�
	{
		m_pSlist.SetSeparateString(g_sSepStringArray[m_nLevel].c_str()); //"="�ָ�
		m_pSlist.Clear();
		nDataCount = m_pSlist.FillChnSepString(m_sKvValue+"=");
		if (nDataCount >= 2)
		{
			tmpnode = new CKVNode();
			if (tmpnode == NULL)
			{
				m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"new CKVNode����");
				return false;
			}
			tmpnode->m_nLevel = m_nLevel+1;
			tmpnode->m_sKvName = m_pSlist.GetAt(0,true);//value
			tmpnode->m_sKvValue = m_pSlist.GetAt(1,true);//value
			//�жϽڵ���ʲô
//			tmpnode->m_nKvType = tmpnode->GetNodeType(tmpnode->m_sKvValue);
// 			m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"LEVEL[%d] TYPE[%d] �ڵ���[%s]�ڵ�ֵ[%s]",\
// 				tmpnode->m_nLevel,tmpnode->m_nKvType,tmpnode->m_sKvName.c_str(),tmpnode->m_sKvValue.c_str());
			AddChildNode(tmpnode);
			if (!tmpnode->ParseNode())
			{
				m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ڵ���� %s",tmpnode->m_sKvValue.c_str());
				return false;
			}
			m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"LEVEL[%d] TYPE[%d] �ڵ���[%s]�ڵ�����[%d]",\
				tmpnode->m_nLevel,tmpnode->m_nKvType,tmpnode->m_sKvName.c_str(),tmpnode->m_nDataCount);
			//���ӽڵ�Ļ�����ֵ��Ϊ�գ���ռ�ÿռ�
			m_sKvValue = "";
		}
		else
		{
			m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ڵ����,��=�ָ����� %s",m_sKvValue.c_str());
			return false;
		}
	}
	else if (m_nLevel == 2) //"��"�ָ�
	{
		m_nKvType = GetNodeType(m_sKvValue);
		m_pSlist.SetSeparateString(g_sSepStringArray[m_nLevel].c_str()); //"��"�ָ�
		m_pSlist.Clear();
		nDataCount = m_pSlist.FillChnSepString(m_sKvValue);
// 		if (m_nKvType == KVTYPE_HASHTABLE)
// 		{
// 			m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"����[%s]�ķָ��������� %d" ,m_sKvValue.c_str(),nDataCount);
// 		}
		
		if (nDataCount>0 && !m_pSlist.m_bNotSepString)
		{
			for (i=0 ; i<nDataCount ; i++ )
			{
				tmpnode = new CKVNode();
				if (tmpnode == NULL)
				{
					m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"new CKVNode����");
					return false;
				}
				tmpnode->m_nLevel = m_nLevel+1;
				tmpnode->m_nKvType = KVTYPE_DATA;  //��Ϊkv�ĸ�ʽ
				tmpnode->m_sKvValue = m_pSlist.GetAt(i,true);//value
				m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"LEVEL[%d] TYPE[%d] �ڵ�ֵ %s",tmpnode->m_nLevel,tmpnode->m_nKvType,tmpnode->m_sKvValue.c_str());
				AddChildNode(tmpnode);
				if (!tmpnode->ParseNode())
				{
					m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ڵ���� %s",tmpnode->m_sKvValue.c_str());
					return false;
				}
			}
			//���ӽڵ�Ļ�����ֵ��Ϊ�գ���ռ�ÿռ�
			m_sKvValue = "";
		}
		else
		{
			m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"�����ڵ��ޡķָ����� %s",m_sKvValue.c_str());
			return true;
		}
	}
	else if (m_nLevel == 3) //"��"�ָ�
	{
		m_pSlist.SetSeparateString(g_sSepStringArray[m_nLevel].c_str()); //"��"�ָ�
		m_pSlist.Clear();
		nDataCount = m_pSlist.FillChnSepString(m_sKvValue);
		if (nDataCount>0 && !m_pSlist.m_bNotSepString)
		{
			for (i=0 ; i<nDataCount ; i++ )
			{
				tmpnode = new CKVNode();
				if (tmpnode == NULL)
				{
					m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"new CKVNode����");
					return false;
				}
				tmpnode->m_nLevel = m_nLevel+1;
				tmpnode->m_nKvType = KVTYPE_DATA;  //��Ϊkv�ĸ�ʽ
				tmpnode->m_sKvValue = m_pSlist.GetAt(i,true);//value
				m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"LEVEL[%d] TYPE[%d] �ڵ�ֵ %s",tmpnode->m_nLevel,tmpnode->m_nKvType,tmpnode->m_sKvValue.c_str());
				AddChildNode(tmpnode);
				if (!tmpnode->ParseNode())
				{
					m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ڵ���� %s",tmpnode->m_sKvValue.c_str());
					return false;
				}
			}
			//���ӽڵ�Ļ�����ֵ��Ϊ�գ���ռ�ÿռ�
			m_sKvValue = "";
		}
		else
		{
			m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"�����ڵ��ޣ��ָ����� %s",m_sKvValue.c_str());
			return true;
		}
	}
	else if (m_nLevel == 4) //"��"�ָ�
	{
		m_pSlist.SetSeparateString(g_sSepStringArray[m_nLevel].c_str()); //"��"�ָ�
		m_pSlist.Clear();
		nDataCount = m_pSlist.FillChnSepString(m_sKvValue);
		if (nDataCount>0 && !m_pSlist.m_bNotSepString)
		{
			for (i=0 ; i<nDataCount ; i++ )
			{
				tmpnode = new CKVNode();
				if (tmpnode == NULL)
				{
					m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"new CKVNode����");
					return false;
				}
				tmpnode->m_nLevel = m_nLevel+1;
				tmpnode->m_nKvType = KVTYPE_DATA;  //��Ϊkv�ĸ�ʽ
				tmpnode->m_sKvValue = m_pSlist.GetAt(i,true);//value
				m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"LEVEL[%d] TYPE[%d] �ڵ�ֵ %s",tmpnode->m_nLevel,tmpnode->m_nKvType,tmpnode->m_sKvValue.c_str());
				AddChildNode(tmpnode);
				if (!tmpnode->ParseNode())
				{
					m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�����ڵ���� %s",tmpnode->m_sKvValue.c_str());
					return false;
				}
			}
			//���ӽڵ�Ļ�����ֵ��Ϊ�գ���ռ�ÿռ�
			m_sKvValue = "";
		}
		else
		{
			m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"�����ڵ��ޡ��ָ����� %s",m_sKvValue.c_str());
			return true;
		}
	}
	else  //����ֵ��
	{
		
	}
	return true;
}

void CKVNode::AddChildNode(CKVNode *addnode)
{
	addnode->m_nextNode = NULL;
	if (m_endChildNode != NULL) //��������ӽڵ�
	{
		m_endChildNode->m_nextNode = addnode;//ԭ���һ�ڵ���½ڵ�Ϊ�˽ڵ�
		addnode->m_lastNode=m_endChildNode; //�˽ڵ���ٽ��Ͻڵ�Ϊԭ���һ�ӽڵ�
	}
	else
	{
		m_firstChildNode = addnode; 
		m_endChildNode = addnode;
		addnode->m_lastNode = NULL;
	}
	addnode->m_parentNode=this;//���ڵ�Ϊ��ǰ�ڵ�
	m_endChildNode=addnode;//��ǰ�ڵ�����һ���ӽڵ�Ϊ�˽ڵ�

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
		if (strncmp(svalue.c_str()+svalue.length()-6,"������",6) == 0)
		{
			return KVTYPE_HASHTABLE;
		}
		else if (strncmp(svalue.c_str()+svalue.length()-4,"����",4) == 0)
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
	if (m_nLevel == 0)   //"#"�ָ�
	{
		if (m_firstChildNode != NULL)
		{
			m_firstChildNode->ToString(data);
			data = data+g_sSepStringArray[m_nLevel];
			//��һ�ӽڵ���ٽ��ڵ�
			tmpnode = m_firstChildNode->m_nextNode;
			while (tmpnode != NULL)
			{
				tmpnode->ToString(data);
				data = data+g_sSepStringArray[m_nLevel];
				tmpnode = tmpnode->m_nextNode;
			}
		}
	}
	else if (m_nLevel == 1)  //"="�ָ�
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
	else if (m_nLevel == 2)  //"��"�ָ�
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
				//��һ�ӽڵ���ٽ��ڵ�
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
	else if (m_nLevel == 3)  //"��"�ָ�
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
				//��һ�ӽڵ���ٽ��ڵ�
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
	else if (m_nLevel == 4)  //"��"�ָ�
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
				//��һ�ӽڵ���ٽ��ڵ�
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
		delete tmpnode;//���������DeletePoint
		tmpnode = NULL;
		tmpnode = m_firstChildNode;
	}
	m_nDataCount =0;
}
void CKVNode::DeletePoint()
{
	
	CKVNode *parentNode= NULL;
	//ȥ�����ڵ�
	parentNode = m_parentNode;
	if (parentNode != NULL) 
	{
		m_parentNode->m_nDataCount--;
		if (parentNode->m_endChildNode == this && parentNode->m_firstChildNode == this) //���ڵ�ֻ����һ���ڵ�
		{
			parentNode->m_endChildNode = NULL;
			parentNode->m_firstChildNode = NULL;
			m_parentNode = NULL;
		}
		else if (parentNode->m_endChildNode == this ) //�����һ���ڵ�
		{
			parentNode->m_endChildNode = m_lastNode;
			m_lastNode->m_nextNode = NULL;
			m_lastNode = NULL;
			m_nextNode = NULL;//���һ���ڵ�϶���NULL��
		}
		else if (parentNode->m_firstChildNode == this) //����ǰһ���ڵ�
		{
			parentNode->m_firstChildNode = m_nextNode;
			m_nextNode->m_lastNode = NULL;
			m_nextNode = NULL;
			m_lastNode = NULL;//���һ���ڵ�϶���NULL��
		}
		else //���м�Ľڵ�
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
		if (m_nextNode == NULL && m_lastNode == NULL) //ǰ��ڵ㶼Ϊ�գ�ʲô������
		{
			
		}
		else if (m_nextNode == NULL)//�����һ���ڵ�
		{
			m_lastNode->m_nextNode = NULL;
			m_lastNode = NULL;
		}
		else if (m_lastNode == NULL) //����ǰһ���ڵ�
		{
			m_nextNode->m_lastNode = NULL;
			m_nextNode = NULL;
		}
		else //���м�Ľڵ�
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
		m_pLog.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�ڵ㲻Ϊ2��ڵ�");
		return false;
	}
	if (m_nKvType == KVTYPE_HASHTABLE)
	{
		m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ڵ�Ϊ��ϣ��");
		if (m_firstChildNode == NULL)
		{
			col = 0;
			line = 0;
			m_pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"�ձ�");
		}
		else
		{
			col = m_firstChildNode->m_nDataCount;
			m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"��ϣ�����%d",col);
			if (m_firstChildNode->m_nextNode != NULL)
			{
				line = m_firstChildNode->m_nextNode->m_nDataCount;
				m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"��ϣ�����%d",line);
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
		m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ڵ�Ϊ�б�");
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
		m_pLog.LogMp(LOG_DEBUG,__FILE__,__LINE__,"�ڵ�Ϊֵ");
		return ToStringOne(data);
	}

	return true;	
}
bool CKVNode::ToStringOne(std::string &data)
{
	
	CKVNode *tmpnode = NULL;
	if (m_nLevel == 0)   //"#"�ָ�
	{
		
	}
	else if (m_nLevel == 1)  //"="�ָ�
	{

	}
	else if (m_nLevel == 2)  //"��"�ָ�
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
				//��һ�ӽڵ���ٽ��ڵ�
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
	else if (m_nLevel == 3)  //"��"�ָ�
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

				
				//��һ�ӽڵ���ٽ��ڵ�
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
	else if (m_nLevel == 4)  //"��"�ָ�
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
				//��һ�ӽڵ���ٽ��ڵ�
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


