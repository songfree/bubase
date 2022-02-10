// TestThread.cpp: implementation of the CTestThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "drebpmt.h"
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
	m_nID=0;//线程的ID
	m_pSendData=NULL;//发送的脚本数据内容
	m_nSendDataLen=0;//发送的脚本数据长度
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
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"%d m_pShareData未设置",m_nID);
		return -1;
	}
	int ret=0;
	int nConnNum=0;
	INT64_ lasttime;
	INT64_ septime;
	INT64_ curtime;
	char rcvData[DREBBUFSIZE];
	unsigned int rcvDataLen;
	int serial = 0;
	memset(rcvData,0,sizeof(rcvData));
	m_log.LogMp(LOG_DEBUG,__FILE__,__LINE__,"%d 线程启动",m_nID);

	while (!m_pShareData->m_bIsExit)
	{
		if (!m_pShareData->m_bIsRun) //是否开始发送
		{
			SLEEP(10);
			continue;
		}
		//若本线程开始时间 大于当前时间减去运行开始时间的话则SLEEP,即在本线程开始时间后才运行
		if (CBF_Date_Time::GetTickCount() - m_pShareData->m_nRunTime < m_pDataPara.nStartTime)
		{
			SLEEP(10);
			if (m_pDataPara.nConnectType == 1)
			{
				if (!m_pFrontApi.IsConnect())
				{
					m_pFrontApi.Close();
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
		lasttime = CBF_Date_Time::GetTickCount();
		//连接服务器
		if (!m_pFrontApi.IsConnect())
		{
			if (!m_pFrontApi.Connect())
			{
				nConnNum++;
				if (nConnNum >= m_pShareData->m_pServerPara.connectNum) //连接超过次数，退出线程
				{
					m_pShareData->Run(true,0,0,0,0,1,0);
					m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"%d %s",m_nID,m_pFrontApi.m_errMsg);
					return -1;
				}
				SLEEP(50);
				continue;
			}
		}
		ResetBuf();
		nConnNum=0;

		serial = m_pShareData->GetSerial();

		ret = m_pFrontApi.SendData(serial,serial,0,m_pDataPara.nCmd,m_destNode,m_pSendData,m_nSendDataLen,m_pDataPara.cZip);
		if (ret != m_nSendDataLen)
		{
			m_pShareData->Run(false,0,0,0,0,1,0);
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"%d %s",m_nID,m_pFrontApi.m_errMsg);
			if (m_pDataPara.nConnectType == 0)
			{
				m_pFrontApi.Close();
			}
			continue;
		}
//		m_pShareData->Run(false,1,ret+DREBHEADLEN,0,0,0,0);
		if (m_pShareData->m_bIsExit) 
		{
			break;
		}
		if (m_pDataPara.nCmd != CMD_DPABC && m_pDataPara.nCmd != CMD_DPPUSH)
		{

		
			rcvDataLen = sizeof(rcvData)-1;
			ret = m_pFrontApi.ReceveData(m_head,(unsigned char *)rcvData,rcvDataLen,m_pShareData->m_pServerPara.timeout);
			if (ret<1)
			{
				m_pShareData->Run(false,1,m_nSendDataLen,0,0,1,0);
				m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"%d %s",m_nID,m_pFrontApi.m_errMsg);
				if (m_pDataPara.nConnectType == 0)
				{
					m_pFrontApi.Close();
				}
				continue;
			}
			curtime = CBF_Date_Time::GetTickCount();
			septime = curtime - lasttime; 
			if (septime > m_pShareData->m_pServerPara.rrtime)
			{
				m_log.LogMp(LOG_PROMPT,__FILE__,__LINE__,"请求点[%ld] 应答点[%ld] 响应时间[%ld] serial[%d] hook[%d] 标识[%d %d %d]应答时间较长",\
					lasttime,curtime,septime,m_head.s_Sinfo.s_nSerial,m_head.s_Sinfo.s_nHook,m_head.s_Sinfo.s_nNodeId,m_head.s_Sinfo.s_cNodePrivateId,m_head.s_Sinfo.s_nDrebSerial);
			}

			m_pShareData->Run(false,1,m_nSendDataLen+DREBHEADLEN,1,ret,0,septime);
		}
		else
		{
			m_pShareData->Run(false,1,m_nSendDataLen+DREBHEADLEN,1,ret,0,0);
		}
		if (m_pDataPara.nConnectType == 0)
		{
			m_pFrontApi.Close();
		}
		//int sleeptime = m_pShareData->m_pServerPara.timesep - CBF_Date_Time::GetTickCount()+lasttime ;//计算间隔
		int sleeptime = m_pDataPara.nSendSep - CBF_Date_Time::GetTickCount()+lasttime ;//计算间隔
		if (sleeptime >0)
		{
			SLEEP(sleeptime);//等待间隔后再发送
		}
	}
	m_pShareData->Run(true,0,0,0,0,0,0);
	m_log.LogMp(LOG_DEBUG,__FILE__,__LINE__,"%d 线程停止",m_nID);

	m_pFrontApi.Close();

	return 0;
}

bool CTestThread::SetData(const char *data,unsigned int datalen,CShareData *pData,_THREAD_DATA_PARA para)
{
	if (m_pSendData != NULL)
	{
		free(m_pSendData);
		m_pSendData = NULL;
	}
	m_pSendData = (char *)malloc(datalen+1);
	memcpy(m_pSendData,data,datalen);
	m_nSendDataLen = datalen;
	if (pData == NULL)
	{
		return false;
	}
	m_pShareData = pData;
	m_pDataPara = para;
	m_destNode.d_nNodeId = m_pDataPara.nNodeId;
	m_destNode.d_cNodePrivateId = m_pDataPara.cNodePrivate;
	m_destNode.d_nSvrMainId = m_pDataPara.nSvrId;
	m_destNode.d_cSvrPrivateId = m_pDataPara.cSvrPrivate;
	m_destNode.d_nServiceNo = m_pDataPara.nServiceNo;
	return true;
}

int CTestThread::ThreadInit()
{
	if (m_pShareData == NULL)
	{
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"%d m_pShareData未设置",m_nID);
		return -1;
	}
	m_log.SetLogPara(m_pShareData->m_pServerPara.loglevel,"","drebpmt.log");
	m_pFrontApi.Init(m_pDataPara.sdrebip.c_str(),m_pDataPara.nPort,m_pShareData->m_pServerPara.timeout);
	int ret;
	//变量初始化,查找配置的变量在sendData是否有，若有则将位置、变量处理指针交给线程
	m_varList.clear();
	for (int i=0;i<m_pShareData->m_varList.size();i++)
	{
		ret = GetVarOffset(m_pShareData->m_varList[i], m_pSendData,m_nSendDataLen,m_varList);
	}

	if (m_pDataPara.nConnectType == 0)
	{
		return 0;
	}
	int nConnNum = 0;
	while(!m_pFrontApi.IsConnect())
	{
		if (!m_pFrontApi.Connect())
		{
			nConnNum++;
			if (nConnNum >= m_pShareData->m_pServerPara.connectNum) //连接超过次数，退出线程
			{
				m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"%d %s",m_nID,m_pFrontApi.m_errMsg);
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
					if (strncmp(var->GetVarName().c_str(),databuf+nbegin+1,namelen) == 0)//找到了
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
			if (lastvname.compare(varname) != 0) //上一个变量和当前变量不同需重取变量值
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
			if (lastvname.compare(varname) != 0) //上一个变量和当前变量不同需重取变量值
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
