// ShareData.cpp: implementation of the CShareData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "testfront.h"
#include "ShareData.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShareData::CShareData()
{
	Clear();
}

CShareData::~CShareData()
{
	for (int i=0;i<m_varList.size();i++)
	{
		delete m_varList[i];
	}
	m_varList.clear();
}

void CShareData::Run(bool isstop, int sendnum,int sendbyte, int rcvnum,int rcvbyte, int errnum, int resptime)
{
	CBF_PMutex pp(&m_runMutex);
	if (isstop)
	{
		m_nTotalThreadNum--;
	}
	m_nSendNum+=sendnum;
	m_nRcvNum+=rcvnum;
	m_nErrNum+=errnum;
	m_nRcvByte+=rcvbyte;
	m_nSendByte+=sendbyte;

	if (errnum<1) //û�д��ʱ�������Ӧʱ��
	{
		if (resptime < m_nMinRespTime)
		{
			m_nMinRespTime = resptime;
		}
		if (resptime > m_nMaxRespTime)
		{
			m_nMaxRespTime = resptime;
		}
		m_nTotalRespTIme+=resptime;
	}
	
}

void CShareData::Clear()
{
	m_nAllRunTime =0;//������ʱ��
	m_nCurRunTime=0;//��ǰ����ʱ��
	m_nTotalThreadNum=0;//���߳���
	m_nSendNum=0;//���ͱ���
	m_nRcvNum=0;//���ձ���
	m_nErrNum=0;//�������
	m_nMinRespTime=9999999;//��С��Ӧʱ��
	m_nMaxRespTime=0;//�����Ӧʱ��
	m_nAvRespTime=0;//ƽ����Ӧʱ��
	m_nCurNumSec=0;//��ǰÿ�����
	m_nAvNumSec=0;//ƽ��ÿ�����
	m_nSendByte = 0;//�����ֽ���
	m_nRcvByte = 0;//�����ֽ���
	
	m_nTotalRespTIme=0;//����Ӧʱ��
	m_bIsExit=true;//�Ƿ��˳�
	m_bIsRun = false;//�Ƿ�ʼ����
	m_nConnNum = 2;//���ӷ���������
	memset(&m_pServerPara,0,sizeof(SERVER_PARA));
}

bool CShareData::InitVar(char *varxml)
{
	int type;
	int len;
	string initvalue;
	string fmt;
	if (m_varxml.fromFile(varxml)<0)
	{
		return false;
	}
	m_varList.clear();
	m_varxml.gotoHead();
	CNode *headnode = m_varxml.m_xml.getCurrentNode();
	CNode *tmpnode = headnode->getFirstChild();
	while (tmpnode != NULL)
	{
		if (tmpnode->GetAttributeValue("type",type)<0)
		{
			tmpnode = tmpnode->getNextSibling();
			continue;
		}
		if (tmpnode->GetAttributeValue("len",len)<0)
		{
			tmpnode = tmpnode->getNextSibling();
			continue;
		}
		if (type == 0)
		{
			if (tmpnode->GetAttributeValue("fmt",fmt)<0)
			{
				tmpnode = tmpnode->getNextSibling();
				continue;
			}
			if (tmpnode->GetAttributeValue("initvalue",initvalue)<0)
			{
				tmpnode = tmpnode->getNextSibling();
				continue;
			}
			CIVarDefine *var = new CIVarDefine();
			var->SetVarPara(tmpnode->getName(),type,len);
			__int64 ivalue= _atoi64(initvalue.c_str());
			var->IntVarInit(fmt,ivalue);
			m_varList.push_back(var);
			tmpnode = tmpnode->getNextSibling();
		}
		else
		{
			CIVarDefine *var = new CIVarDefine();
			var->SetVarPara(tmpnode->getName(),type,len);
			CNode *childnode = tmpnode->getFirstChild();
			while (childnode != NULL)
			{
				var->PushVarValue(childnode->getName());
				childnode = childnode->getNextSibling();
			}
			m_varList.push_back(var);
			tmpnode = tmpnode->getNextSibling();
		}
		
	}
	m_varxml.newPackage();
	return true;
}
