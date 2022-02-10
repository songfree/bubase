// ShareData.cpp: implementation of the CShareData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "drebpmt.h"
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

void CShareData::Run(bool isstop, int sendnum,int sendbyte, int rcvnum,int rcvbyte, int errnum, INT64_ resptime)
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

	if (errnum<1) //没有错的时候计算响应时间
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
	m_nAllRunTime =0;//总运行时间
	m_nCurRunTime=0;//当前运行时间
	m_nTotalThreadNum=0;//总线程数
	m_nSendNum=0;//发送笔数
	m_nRcvNum=0;//接收笔数
	m_nErrNum=0;//出错笔数
	m_nMinRespTime=9999999999;//最小响应时间
	m_nMaxRespTime=0;//最大响应时间
	m_nAvRespTime=0;//平均响应时间
	m_nCurNumSec=0;//当前每秒笔数
	m_nAvNumSec=0;//平增每秒笔数
	m_nSendByte = 0;//发送字节数
	m_nRcvByte = 0;//接收字节数
	
	m_nTotalRespTIme=0;//总响应时间
	m_bIsExit=true;//是否退出
	m_bIsRun = false;//是否开始发送

	m_nSerial = 0;
	memset(&m_pServerPara,0,sizeof(SERVER_PARA));
}

bool CShareData::InitVar(char *varxml)
{
	int type;
	int len;
	std::string initvalue;
	std::string fmt;
	std::string attrvalue;
	if (!m_varxml.FromFile(varxml))
	{
		return false;
	}
	m_varList.clear();
	CXmlNode *headnode = m_varxml.GetNodeByPath("/package/head");

	CXmlNode *tmpnode = (CXmlNode *)headnode->GetFirstChild();
	while (tmpnode != NULL)
	{
		if (tmpnode->GetAttribute("type",false,attrvalue) == NULL)
		{
			tmpnode = (CXmlNode *)tmpnode->getNextSibling();
			continue;
		}
		type = atoi(attrvalue.c_str());
		if (tmpnode->GetAttribute("len",false,attrvalue) == NULL)
		{
			tmpnode = (CXmlNode *)tmpnode->getNextSibling();
			continue;
		}
		len = atoi(attrvalue.c_str());
		if (type == 0)
		{
			if (tmpnode->GetAttribute("fmt",false,fmt) == NULL)
			{
				tmpnode = (CXmlNode *)tmpnode->getNextSibling();
				continue;
			}
			if (tmpnode->GetAttribute("initvalue",false,initvalue) == NULL)
			{
				tmpnode = (CXmlNode *)tmpnode->getNextSibling();
				continue;
			}
			CIVarDefine *var = new CIVarDefine();
			var->SetVarPara(tmpnode->GetNodeName(),type,len);
			__int64 ivalue= _atoi64(initvalue.c_str());
			var->IntVarInit(fmt,ivalue);
			m_varList.push_back(var);
			tmpnode = (CXmlNode *)tmpnode->getNextSibling();
		}
		else
		{
			CIVarDefine *var = new CIVarDefine();
			var->SetVarPara(tmpnode->GetNodeName(),type,len);
			CXmlNode *childnode = (CXmlNode *)tmpnode->GetFirstChild();
			while (childnode != NULL)
			{
				var->PushVarValue(childnode->GetNodeName());
				childnode = (CXmlNode *)childnode->getNextSibling();
			}
			m_varList.push_back(var);
			tmpnode = (CXmlNode *)tmpnode->getNextSibling();
		}
		
	}
	return true;
}

int CShareData::GetSerial()
{
	CBF_PMutex pp(&m_serialMutex);
	m_nSerial++;	
	if (m_nSerial<=0)
	{
		m_nSerial = 1;
	}
	return m_nSerial;
}
