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
	m_nSerial = 0;
	m_pLoginData = NULL;
}

CTestThread::~CTestThread()
{
	if (m_pSendData != NULL)
	{
		free(m_pSendData);
		m_pSendData = NULL;
	}
	if (m_pLoginData != NULL)
	{
		free(m_pLoginData);
		m_pLoginData = NULL;
	}
}

bool CTestThread::Init()
{
	return(true);
}

void CTestThread::ExitThreadInstance()
{
	m_pGetPush.m_bisRun = false;
}
bool CTestThread::InitThreadInstance()
{
    return true;
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
	int sleeptime ;
	char rcvData[DREBBUFSIZE];
	unsigned int rcvDataLen;
	int serial = 0;
	memset(rcvData,0,sizeof(rcvData));
	m_log.LogMp(LOG_DEBUG,__FILE__,__LINE__,"%d 线程启动",m_nID);

	//启动接收推送线程
	m_pGetPush.m_bisRun = true;
	m_pGetPush.m_log = &m_log;
	//m_pGetPush.CreateThread();

	while (!m_pShareData->m_bIsExit)
	{
		if (!m_pShareData->m_bIsRun) //是否开始发送
		{
			SLEEP(10);
			continue;
		}
		if (!Login())
		{
			m_pShareData->Run(true,0,0,0,0,1,0,0);
			return -1;
		}
		//若本线程开始时间 大于当前时间减去运行开始时间的话则SLEEP,即在本线程开始时间后才运行
		if (CBF_Date_Time::GetTickCount() - m_pShareData->m_nRunTime < m_pDataPara.nStartTime)
		{
			
			SLEEP(10);
			continue;
		}
		break;
	}
	CGATE_COMMSTRU data;
	bzero(&data,sizeof(CGATE_COMMSTRU));
	while (!m_pShareData->m_bIsExit)
	{
		//连接服务器
		if (!Login())
		{
			nConnNum++;
			if (nConnNum >5)
			{
				m_pShareData->Run(true,0,0,0,0,1,0,0);
				m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"%d 连接网关次数超限，退出线程 ",m_nID);
				return -1;
			}
			SLEEP(500);
		}
		nConnNum = 0;
		ResetBuf();
		
		memcpy(&data.buffer,m_pSendData,m_nSendDataLen);
		data.head.nLen = m_nLoginDataLen;
		data.head.stComm.cMsgType = MSG_GATEREQ;
		data.head.stComm.cCmd = CMD_DPCALL;
		
		data.head.stComm.cNextFlag = 0;
		data.head.stComm.cRaflag = 0;
		data.head.stComm.cZip = 0;
		data.head.stDest.nServiceNo = m_pDataPara.nServiceNo;
		lasttime = CBF_Date_Time::GetTickCount();
		ret = m_pGateLink.SendReqData(&data,m_pShareData->m_pServerPara.timeout/1000);
		if (ret <= 0)
		{
			m_pGateLink.Logout();
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"%d 发送请求出错  接收应答超时 退出线程",m_nID);
			m_pShareData->Run(true,0,0,0,0,1,0,0);
			return -1;
		}
		curtime = CBF_Date_Time::GetTickCount();
		septime = curtime - lasttime; 
		if (septime > m_pShareData->m_pServerPara.rrtime)
		{
			m_log.LogMp(LOG_PROMPT,__FILE__,__LINE__,"%d 标识[%d] 请求点[%ld] 应答点[%ld] 响应时间[%ld] 用时较长",\
				m_nID,data.head.stDest.nSerial,lasttime,curtime,septime);
		}
		

		if (data.head.nRetCode != 0)
		{
			m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"%d  后台应答出错 返回码[%d] 退出线程",m_nID,data.head.nRetCode);
			m_pShareData->Run(false,1,data.head.nLen+CGATEHEADLEN,1,data.head.nLen+CGATEHEADLEN,0,septime,1);
			m_pGateLink.Logout();
			continue;
		}
		if (data.head.stComm.cNextFlag == 0)
		{
			m_pShareData->Run(false,1,data.head.nLen+CGATEHEADLEN,1,data.head.nLen+CGATEHEADLEN,0,septime,1);
			sleeptime = m_pShareData->m_pServerPara.timesep - CBF_Date_Time::GetTickCount()+lasttime ;//计算间隔
			if (sleeptime >0)
			{
				SLEEP(sleeptime);//等待间隔后再发送
			}
			continue;
		}

		m_pShareData->Run(false,1,data.head.nLen+CGATEHEADLEN,1,data.head.nLen+CGATEHEADLEN,0,septime,1);

		int packnum =0;
		int totalpack = data.head.stNext.n_nNextNo;
		//后台主动推送，不停的收
		packnum++;
		int datalen = 0;
		while (packnum < totalpack)
		{
			ret = m_pGateLink.GetAnsData(&data,m_pShareData->m_pServerPara.timeout/1000);
			if (ret <1)
			{
				m_pShareData->RunPush(m_pCgateLogin.head.nLen+CGATEHEADLEN,packnum);
				m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"%d 接收应答超时 退出线程 ",m_nID);
				m_pGateLink.Logout();
				m_pShareData->Run(true,0,0,0,0,1,0,0);
				return -1;
			}
			packnum++;
			datalen= datalen+m_pCgateLogin.head.nLen+CGATEHEADLEN;
		}
		m_pShareData->Run(false,0,0,0,datalen,0,0,packnum);
		if (m_pShareData->m_bIsExit) 
		{
			break;
		}
		sleeptime = m_pShareData->m_pServerPara.timesep - CBF_Date_Time::GetTickCount()+lasttime ;//计算间隔
		if (sleeptime >0)
		{
			SLEEP(sleeptime);//等待间隔后再发送
		}
	}
	m_pShareData->Run(true,0,0,0,0,0,0,0);
	m_log.LogMp(LOG_DEBUG,__FILE__,__LINE__,"%d 线程停止",m_nID);

	m_pGateLink.Logout();

	return 0;
}

bool CTestThread::SetData(const char *data,unsigned int datalen,const char *logindata,unsigned int logindatalen,CShareData *pData,_THREAD_DATA_PARA para)
{
	if (m_pSendData != NULL)
	{
		free(m_pSendData);
		m_pSendData = NULL;
	}
	m_pSendData = (char *)malloc(datalen+1);
	memcpy(m_pSendData,data,datalen);
	m_nSendDataLen = datalen;


	if (m_pLoginData != NULL)
	{
		free(m_pLoginData);
		m_pLoginData = NULL;
	}
	m_pLoginData = (char *)malloc(logindatalen+1);
	memcpy(m_pLoginData,logindata,logindatalen);
	m_nLoginDataLen = logindatalen;

	if (pData == NULL)
	{
		return false;
	}
	m_pShareData = pData;
	m_pDataPara = para;

	m_pGetPush.m_pShareData = m_pShareData;
	m_pGetPush.m_nID = m_nID;
	m_pGetPush.m_log = &m_log;
	m_pGetPush.m_pDataPara =  m_pDataPara;


	m_pGetPush.m_pGateLink = &m_pGateLink;


	return true;
}

int CTestThread::ThreadInit()
{
	if (m_pShareData == NULL)
	{
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"%d m_pShareData未设置",m_nID);
		return -1;
	}

	m_log.SetLogPara(m_pShareData->m_pServerPara.loglevel,"","gatepmt.log");
	
	if (!m_pGateLink.InitApi(m_pDataPara.sApiXml.c_str()))
	{
		return -1;
	}
	int level;
	std::string logfilepath;
	std::string logfilename;
	int pid;
	m_pGateLink.m_pRes.g_pLog.GetLogPara(level,logfilepath,logfilename,pid);
	char tmpchar[100];
	sprintf(tmpchar,"%s.%d",logfilename.c_str(),m_nID);

	m_pGateLink.m_pRes.g_pLog.SetLogPara(level,logfilepath.c_str(),tmpchar,m_nID);

	sprintf(tmpchar,"cgate.%d.dat",m_nID);
	m_pGateLink.SetDatLogName(tmpchar);
	
	int ret;
	//变量初始化,查找配置的变量在sendData是否有，若有则将位置、变量处理指针交给线程
	m_varList.clear();
	for (int i=0;i<m_pShareData->m_varList.size();i++)
	{
		ret = GetVarOffset(m_pShareData->m_varList[i], m_pSendData,m_nSendDataLen,m_varList);
	}

	//准备登录数据包
	bzero(&m_pCgateLogin,sizeof(CGATE_COMMSTRU));

	m_pCgateLogin.head.nLen = m_nLoginDataLen;
	m_pCgateLogin.head.stComm.cMsgType = MSG_GATELOGIN;
	m_pCgateLogin.head.stComm.cCmd = CMD_DPCALL;
	m_pCgateLogin.head.stComm.cNextFlag = 0;
	m_pCgateLogin.head.stComm.cRaflag = 0;
	m_pCgateLogin.head.stComm.cZip = 0;
	m_pCgateLogin.head.stDest.nServiceNo = m_pDataPara.nLoginTx;
	memcpy(m_pCgateLogin.buffer,m_pLoginData,m_nLoginDataLen);
	
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

bool CTestThread::Login()
{
	int nConnNum = 0;
	int ret;
	CGATE_COMMSTRU data;
	memcpy(&data,&m_pCgateLogin,sizeof(CGATE_COMMSTRU));
	while(m_pGateLink.GetConnectStatus() !=1 )
	{
		if (!m_pGateLink.ConnectGate())
		{
			nConnNum++;
			if (nConnNum >= m_pShareData->m_pServerPara.connectNum) //连接超过次数，退出线程
			{
				m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"%d %s",m_nID,"连接网关出错");
				m_pGateLink.Logout();
				return false;
			}
			SLEEP(10);
			continue;
		}
		else
		{
			data.head.stDest.nSerial = GetSerial();
			data.head.stComm.cMsgType = MSG_GATELOGIN;
			data.head.stComm.cCmd = CMD_DPCALL;
			data.head.stComm.cNextFlag = 0;
			data.head.stComm.cRaflag = 0;
			data.head.stComm.cZip = 0;
			ret = m_pGateLink.SendLogin(&data,m_pShareData->m_pServerPara.timeout/1000);
			if (ret <= 0)
			{
				m_pGateLink.Logout();
				m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"%d 登录网关出错  接收应答超时",m_nID);
				return false;
			}
			
			if (data.head.nRetCode != 0)
			{
				m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"%d 登录网关出错  返回码[%d]",m_nID,data.head.nRetCode);
				m_pShareData->Run(false,1,m_pCgateLogin.head.nLen+CGATEHEADLEN,1,data.head.nLen+CGATEHEADLEN,0,0,1);
				m_pGateLink.Logout();
				return false;
			}

			if (data.head.stComm.cNextFlag == 0)
			{
				m_pShareData->Run(false,1,m_pCgateLogin.head.nLen+CGATEHEADLEN,1,data.head.nLen+CGATEHEADLEN,0,0,1);
				return true;
			}
			int packnum =0;
			int totalpack = data.head.stNext.n_nNextNo;
			//后台主动推送，不停的收
			packnum++;
			int datalen = 0;
			while (packnum < totalpack)
			{
				ret = m_pGateLink.GetAnsData(&data,m_pShareData->m_pServerPara.timeout/1000);
				if (ret <1)
				{
					m_pShareData->Run(false,0,0,0,data.head.nLen+CGATEHEADLEN,0,0,packnum);
					m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"%d 接收应答 ",m_nID);
					m_pGateLink.Logout();
					return false;
				}
				packnum++;
				datalen= datalen+m_pCgateLogin.head.nLen+CGATEHEADLEN;
			}
			m_pShareData->Run(false,0,0,1,datalen,0,0,packnum);
			return true;
		}
	}
	return true;
}

int CTestThread::GetSerial()
{
	if (m_nSerial >= 65534)
	{
		m_nSerial = 1;
	}
	else
	{
		m_nSerial++;
	}
	return m_nSerial;
}
