// TestThread.cpp: implementation of the CTestThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "testfront.h"
#include "TestThread.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTestThread::CTestThread()
{
	m_pShareData = NULL;
	m_nBeginTime=0;//���߳̿�ʼ����ʱ��,���
	m_nID=0;//�̵߳�ID
	
	m_pSendData=NULL;//���͵Ľű���������
	m_nSendDataLen=0;//���͵Ľű����ݳ���
	m_nConnType = 0;//��������
}

CTestThread::~CTestThread()
{
	if (m_pSendData != NULL)
	{
		free(m_pSendData);
		m_pSendData = NULL;
	}
}

bool CTestThread::Init()
{
	return(true);
}
bool CTestThread::Terminate()
{
	return(true);
}

int CTestThread::Run()
{
	if (m_pShareData == NULL)
	{
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"%d m_pShareDataδ����",m_nID);
		return -1;
	}
	int ret=0;
	int nConnNum=0;
	int lasttime;
	int septime;
	int curtime;
	char rcvData[20*1024];
	int rcvDataLen;
	memset(rcvData,0,sizeof(rcvData));
	m_log.LogMp(LOG_DEBUG,__FILE__,__LINE__,"%d �߳�����",m_nID);

	while (!m_pShareData->m_bIsExit)
	{
		if (!m_pShareData->m_bIsRun) //�Ƿ�ʼ����
		{
			SLEEP(10);
			continue;
		}
		//�����߳̿�ʼʱ�� ���ڵ�ǰʱ���ȥ���п�ʼʱ��Ļ���SLEEP,���ڱ��߳̿�ʼʱ��������
		if (m_pDate.GetTickCount() - m_pShareData->m_nRunTime < m_nBeginTime)
		{
			SLEEP(10);
			if (m_nConnType == 1)
			{
				if (!m_pFrontApi.IsConnect())
				{
					m_pFrontApi.DisConnect();
					if (!m_pFrontApi.Connect())
					{
						
					}
				}
				
			}
			continue;
		}
		break;
	}
	while (!m_pShareData->m_bIsExit)
	{
		lasttime = m_pDate.GetTickCount();
		//���ӷ�����
		if (!m_pFrontApi.IsConnect())
		{
			if (!m_pFrontApi.Connect())
			{
				nConnNum++;
				if (nConnNum >= m_pShareData->m_nConnNum) //���ӳ����������˳��߳�
				{
					m_pShareData->Run(true,0,0,0,0,1,0);
					m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"%d %s",m_nID,m_pFrontApi.GetErrMsg().c_str());
					//m_log.LogMp(LOG_DEBUG-1,__FILE__,__LINE__,"%d �߳�ֹͣ",m_nID);
					return -1;
				}
				SLEEP(50);
				continue;
			}
		}
		ResetBuf();
		nConnNum=0;
		ret = m_pFrontApi.SendBuf(m_pSendData,m_nSendDataLen);
		if (ret != m_nSendDataLen)
		{
			m_pShareData->Run(false,0,0,0,0,1,0);
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"%d %s",m_nID,m_pFrontApi.GetErrMsg().c_str());
			if (m_nConnType == 0)
			{
				m_pFrontApi.DisConnect();
			}
			continue;
		}
		if (m_pShareData->m_bIsExit) 
		{
			break;
		}
		if (m_pShareData->m_pServerPara.timeout>0) //����0��ȥȡ��Ӧ��
		{
			rcvDataLen = sizeof(rcvData)-1;
			ret = m_pFrontApi.RecvBuf(rcvData,rcvDataLen,m_pShareData->m_pServerPara.timeout);
			if (ret<1)
			{
				m_pShareData->Run(false,1,m_nSendDataLen,0,0,1,0);
				m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"%d %s",m_nID,m_pFrontApi.GetErrMsg().c_str());
				if (m_nConnType == 0)
				{
					m_pFrontApi.DisConnect();
				}
				continue;
			}
			curtime = m_pDate.GetTickCount();
			septime = curtime - lasttime; 
			m_pShareData->Run(false,1,m_nSendDataLen,1,ret,0,septime);
		}
		else
		{
			curtime = m_pDate.GetTickCount();
			septime = curtime - lasttime; 
			m_pShareData->Run(false,1,m_nSendDataLen,0,0,0,septime);
		}
		
		if (m_nConnType == 0)
		{
			m_pFrontApi.DisConnect();
		}
		int sleeptime = m_pShareData->m_pServerPara.timesep - m_pDate.GetTickCount()+lasttime ;//������
		if (sleeptime >0)
		{
			SLEEP(sleeptime);//�ȴ�������ٷ���
		}
	}
	m_pShareData->Run(true,0,0,0,0,0,0);
	m_log.LogMp(LOG_DEBUG,__FILE__,__LINE__,"%d �߳�ֹͣ",m_nID);

	m_pFrontApi.DisConnect();

	return 0;
}

bool CTestThread::SetData(const char *data,unsigned int datalen,CShareData *pData,unsigned int begintime,int conntype)
{
	if (m_pSendData != NULL)
	{
		free(m_pSendData);
		m_pSendData = NULL;
	}
	m_pSendData = (char *)malloc(datalen);
	memcpy(m_pSendData,data,datalen);
	m_nSendDataLen = datalen;
	if (pData == NULL)
	{
		return false;
	}
	m_pShareData = pData;
	m_nBeginTime = begintime;
	m_nConnType = conntype;
	return true;
}

int CTestThread::ThreadInit()
{
	if (m_pShareData == NULL)
	{
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"%d m_pShareDataδ����",m_nID);
		return -1;
	}
	m_log.SetLogPara(m_pShareData->m_pServerPara.loglevel,"","TestFront.log");
	m_pFrontApi.SetPara(m_pShareData->m_pServerPara.sip,m_pShareData->m_pServerPara.port,\
		"TestFrontApi.log",m_pShareData->m_pServerPara.loglevel,m_pShareData->m_pServerPara.headlen,false,0,0);
	int ret;
	//������ʼ��,�������õı�����sendData�Ƿ��У�������λ�á���������ָ�뽻���߳�
	m_varList.clear();
	for (int i=0;i<m_pShareData->m_varList.size();i++)
	{
		ret = GetVarOffset(m_pShareData->m_varList[i], m_pSendData,m_nSendDataLen,m_varList);
	}

	if (m_nConnType == 0)
	{
		return 0;
	}
	int nConnNum = 0;
	while(!m_pFrontApi.IsConnect())
	{
		if (!m_pFrontApi.Connect())
		{
			nConnNum++;
			if (nConnNum >= m_pShareData->m_nConnNum) //���ӳ����������˳��߳�
			{
				m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"%d %s",m_nID,m_pFrontApi.GetErrMsg().c_str());
				//m_log.LogMp(LOG_DEBUG-1,__FILE__,__LINE__,"%d �߳�ֹͣ",m_nID);
				return -1;
			}
			SLEEP(10);
			continue;
		}
	}
	return 0;
}

int CTestThread::GetVarOffset(CIVarDefine *var, const char *databuf,int datalen,vector<_SVAROFFSET>&result)
{
	
	int namelen;
	int nbegin=0;
	int nend=0;
	_SVAROFFSET sof;
	bool isfindbegin=false;
	namelen= var->GetVarName().length();
	char *p = (char *)databuf;
	for (int i=0;i<datalen;i++,p++)
	{
		if ( *p == '<' )
		{
			isfindbegin = true;
			nbegin = i;
		}
		if (isfindbegin)
		{
			if (*p == '>')
			{
				nend = i;
				if (nend - nbegin -1 == namelen)
				{
					if (strncmp(var->GetVarName().c_str(),databuf+nbegin+1,namelen) == 0)//�ҵ���
					{
						sof.begin = nbegin;
						sof.end = nend;
						sof.pVarPoint = var;
						result.push_back(sof);
					}
				}
				isfindbegin  = false;
			}
		}
	}
	return result.size();
}

void CTestThread::ResetBuf()
{
	string varname;
	string lastvname;
	char svalue[100];
	string ssvalue;
	int vartype;

	for (int i=0;i<m_varList.size();i++)
	{
		lastvname = varname;
		varname = m_varList[i].pVarPoint->GetVarName();
		
		vartype = m_varList[i].pVarPoint->GetVarType();
		if ( vartype == 0)
		{
			if (lastvname.compare(varname) != 0) //��һ�������͵�ǰ������ͬ����ȡ����ֵ
			{
				memset(svalue,0,sizeof(svalue));
				sprintf(svalue,m_varList[i].pVarPoint->GetFmt().c_str(),m_varList[i].pVarPoint->GetVarValue());
			}
			if (strlen(svalue)<m_varList[i].pVarPoint->GetVarLen() && strlen(svalue) >m_varList[i].pVarPoint->GetVarName().length() )
			{
				memcpy(m_pSendData+m_varList[i].begin,svalue,strlen(svalue));
			}
			else
			{
				memcpy(m_pSendData+m_varList[i].begin,svalue,m_varList[i].pVarPoint->GetVarLen());
			}
			
		}
		if (vartype > 0)
		{
			if (lastvname.compare(varname) != 0) //��һ�������͵�ǰ������ͬ����ȡ����ֵ
			{
				ssvalue = m_varList[i].pVarPoint->GetVarValueS();
			}
			if (ssvalue.length() < m_varList[i].pVarPoint->GetVarLen() && ssvalue.length() > m_varList[i].pVarPoint->GetVarName().length())
			{
				memcpy(m_pSendData+m_varList[i].begin,ssvalue.c_str(),ssvalue.length());
			}
			else
			{
				memcpy(m_pSendData+m_varList[i].begin,ssvalue.c_str(),m_varList[i].pVarPoint->GetVarLen());
			}
			
		}
	}
}
