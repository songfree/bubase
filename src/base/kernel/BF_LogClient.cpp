// BF_LogClient.cpp: implementation of the CBF_LogClient class.
//
//////////////////////////////////////////////////////////////////////

#include "BF_LogClient.h"
#include "BF_Tools.h"
#include "BF_Slist.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define  LOGSERVER_HEADLEN   4   //报文头长度


CBF_LogClient::CBF_LogClient()
{
	m_nLogLevel = LOG_DEBUG;
	m_nId = -1;
	char tmpchar[300];
	memset(tmpchar,0,sizeof(tmpchar));
	WORKPATH(tmpchar);
	m_sLogPath = tmpchar;
	m_sLogName = "logclient.log";
	m_bIsSetPara = false;
	m_pFp=NULL;
	m_bIsLocal = true;
	m_bIsExit = true;
	m_pMemPool.Init(1000,LOG_BUFFERSIZE+54);
	bzero(m_sIp,sizeof(m_sIp));
	m_nPort = 0;
	
	m_bIsCheckDate  = false;
	m_nMaxLogSize = 0;
	m_nCurLogSize = 0;
	bzero(m_sLogFilePathName,sizeof(m_sLogFilePathName));
	m_sock = INVALID_SOCKET;
	m_nConntime = 0;
	m_nReadtime = 0;
	m_nWriteTime = 0;
	m_bIsPingTimer = false;
	m_bIsConnect = false;
	m_pTimer.Init(1000);
	m_nRcvLen = 0;
	m_nSendLen = 0;
	bzero(m_sRcvBuffer,sizeof(m_sRcvBuffer));
	bzero(m_sSendBuffer,sizeof(m_sSendBuffer));
	m_nPort=0;
	m_nPingTime = 0;
}

CBF_LogClient::~CBF_LogClient()
{
	char *data= NULL;
	while (m_pQueue.GetData(data))
	{
		m_pDate.Update();
		//检查日期是否变化 m_bIsCheckDate由定时器来改变，不用随时来根据日期来检查
		if (m_bIsCheckDate)
		{
			if (m_sCurDate.compare(m_pDate.ToStringD8()) != 0)
			{
				m_sCurDate = m_pDate.ToStringD8();
				//设置定时器 为下一个日期的前5秒时间
				//m_pTimer.SetTimer(0,(m_pDate.GetRemainSecCurDate()-5)*1000,&CBF_LogClient::DateChange,this,0);
				m_bIsCheckDate = false;
				if (m_pFp != NULL)
				{
					fclose(m_pFp);
					m_pFp = NULL;
				}
				if (!OpenLogFile())
				{
					fprintf(stderr,"打开日志文件错\n");
					m_pMemPool.PoolFree(data);
					continue; ;
				}
			}
			
		}
		if (m_pFp == NULL)
		{
			if (!OpenLogFile())
			{
				fprintf(stderr,"打开日志文件错\n");
				m_pMemPool.PoolFree(data);
				continue; ;
			}
		}
		//写入日期
		WriteLog(data);
		m_pMemPool.PoolFree(data);
	}
	if (m_pFp != NULL)
	{
		fclose(m_pFp);
		m_pFp = NULL;
	}
}
void CBF_LogClient::SetLogPara(int level,const char *logfilepath,const char *logfilename, int id)
{
	m_nId = id;
	m_nLogLevel = level;
	if (logfilepath == NULL && logfilename == NULL)
	{
		return;
	}
	m_sLogName = logfilename;
	m_sLogPath = logfilepath;
	if (m_sLogPath.length()<1)
	{
		char tmpchar[300];
		memset(tmpchar,0,sizeof(tmpchar));
#ifdef _WINDOWS
		WORKPATH(tmpchar);
		strcat(tmpchar,"\\log");
		if (!CBF_Tools::MakeDir(tmpchar))
		{
			fprintf(stderr,"建立目录%s失败\n",tmpchar);
			return;
		}

		m_sLogPath = tmpchar;
#else
#ifdef LOGCURPATH
		WORKPATH(tmpchar);
		strcat(tmpchar,"/log");
		if (!CBF_Tools::MakeDir(tmpchar))
		{
			fprintf(stderr,"建立目录%s失败\n",tmpchar);
			return;
		}
		m_sLogPath = tmpchar;
#else	
		sprintf(tmpchar,"%s/log",(char *)getenv("HOME"));
		if (!CBF_Tools::MakeDir(tmpchar))
		{
			fprintf(stderr,"建立目录%s失败\n",tmpchar);
			return;
		}
		m_sLogPath = tmpchar;
#endif
#endif
		//写入本地
		m_bIsLocal = true;
		m_pDate.Update();
		if (m_pFp != NULL)
		{
			fclose(m_pFp);
			m_pFp = NULL;
		}
		if (!OpenLogFile())
		{
			fprintf(stderr,"打开日志文件错\n");
		}
	}
	else
	{
		//分析是本地还是远程
		CBF_Slist psep("@");
		if (psep.FillSepString(logfilepath)<2) 
		{
			//写入本地
			m_bIsLocal = true;
			CBF_Tools::MakeDir(m_sLogPath.c_str());
			m_pDate.Update();
			if (m_pFp != NULL)
			{
				fclose(m_pFp);
				m_pFp = NULL;
			}
			if (!OpenLogFile())
			{
				fprintf(stderr,"打开日志文件错\n");
			}
			
		}
		else
		{
			//远程服务器
			m_bIsLocal = false;
			strcpy(m_sIp,psep.GetAt(0).c_str());
			m_nPort = atoi(psep.GetAt(1).c_str());
			m_log.SetLogPara(LOG_DEBUG,"","logclient.log");

		}
	}
	m_bIsSetPara = true;
}
int CBF_LogClient::GetName(char *filename,char **pfn)
{
	int filelen;
	int i;
	int offset = -1;
	filelen = strlen(filename);
	if (filelen < 1)
	{
		*pfn=filename;
		return filelen;
	}
	for (i=filelen-1 ;i>=0 ; i--)
	{
		if (filename[i] == '\\' || filename[i] == '/' )
		{
			offset = i;
			break;
		}
	}
    *pfn = filename+offset+1;
	return filelen-offset;
// 	if (offset > -1)
// 	{
// 		filelen = filelen - offset;
// 		for (i=0; i< filelen; i++)
// 		{
// 			filename[i]=filename[i+offset+1];
// 		}
// 		filename[filelen-1]=0;
// 	}
// 	return ;
}


int CBF_LogClient::LogMp(int level, const char *filename, int line,const char *fmt,...)
{
	if (level>m_nLogLevel)
	{
		return 0;
	}
	m_pDate.Update();
//	INT64_ btime0=CBF_Date_Time::GetTickUS();
//	INT64_ btime1=CBF_Date_Time::GetTickUS();
	char *fname=NULL;
	//去掉路径
	int fnamelen = GetName((char *)filename,&fname);
	
//	INT64_ etime1=CBF_Date_Time::GetTickUS();
	S_LOG_DATA *data = (S_LOG_DATA *)m_pMemPool.PoolMalloc();
	if ( NULL == data )
	{
		return -1;
	} 
	bzero(data,sizeof(S_LOG_DATA)-1);
	data->logus = m_pDate.GetUs();
	data->fileline = line;
	data->loglevel = level;
	data->wtype = 0;
	data->threadid = GetThreadID();
	if (fnamelen>40)
	{
		memcpy(data->filename,fname,40);
	}
	else
	{
		strcpy(data->filename,fname);
	}
//	INT64_ etime2=CBF_Date_Time::GetTickUS();
	va_list ap;
	va_start(ap, fmt);
    vsprintf(data->data, fmt, ap);
    va_end(ap);
	
//	INT64_ etime3=CBF_Date_Time::GetTickUS();
	data->datalen = strlen(data->data);
	data->data[data->datalen] = 0;
	m_pQueue.PushData((char *)data);
//	INT64_ etime4=CBF_Date_Time::GetTickUS();

//	printf("耗时  %lld %lld %lld %lld %lld\n",etime4-etime3-(btime1-btime0),etime3-etime2-(btime1-btime0),etime2-etime1-(btime1-btime0),etime1-btime1-(btime1-btime0),btime1-btime0);
	return 0;
}

int CBF_LogClient::LogBin(int level, const char *filename, int line,const char *title, char *msg,int msglen)
{
	if (level>m_nLogLevel)
	{
		return 0;
	}
	if (msglen <=0 )
	{
		return 0;
	}
	m_pDate.Update();
	char *fname=NULL;
	//去掉路径
	GetName((char *)filename,&fname);
	S_LOG_DATA *data = (S_LOG_DATA *)m_pMemPool.PoolMalloc();
	if ( NULL == data )
	{
		return -1;
	}

	bzero(data,sizeof(S_LOG_DATA));
	data->logus = m_pDate.GetUs();
	data->fileline = line;
	data->loglevel = level;
	data->wtype = 1;
	data->threadid = GetThreadID();
	if (strlen(fname)>40)
	{
		memcpy(data->filename,fname,40);
	}
	else
	{
		strcpy(data->filename,fname);
	}
	
	
	int ttlen= strlen(title);
#ifdef _ENGLISH_
	sprintf(data->data,"TITLE[%s]\n[",title);
	
	data->datalen = msglen+ttlen+9;
#else
	sprintf(data->data,"标题[%s]\n[",title);
	data->datalen = msglen+ttlen+8;
#endif
	
	if (data->datalen >LOG_BUFFERSIZE)
	{
		data->datalen = LOG_BUFFERSIZE;
	}
	
	memcpy(data->data+ttlen+8,msg,data->datalen-ttlen-8);

	m_pQueue.PushData((char *)data);
	
	return 0;
}

bool CBF_LogClient::isWrite(int level)
{
	if (level > m_nLogLevel)
	{
		return false;
	}
	return true;
}
void CBF_LogClient::GetLogPara(int &level,std::string &logfilepath, std::string &logfilename,int &id)
{
	level = m_nLogLevel;
	logfilepath = m_sLogPath;
	logfilename = m_sLogName;
	id = m_nId;
}
bool CBF_LogClient::InitThreadInstance()
{
	return true;
}


void CBF_LogClient::ExitThreadInstance()
{

}

int CBF_LogClient::Run()
{
	if (m_bIsLocal)
	{
		return RunLocal();
	}
	else
	{
		return RunServer();
	}
}
bool CBF_LogClient::WriteLog(char *logdata)
{
	S_LOG_DATA * pp = (S_LOG_DATA *)logdata;
	char buffer[60];
	char headbuffer[100];
	memset(buffer,0,sizeof(buffer));
	//0严重错误1重要错误2一般错误3警告4提示警告5调试信息
#ifdef _ENGLISH_
	switch (pp->loglevel)
	{
		case -1:
			sprintf(buffer,"INFO ");
			break;
		case 0:
			sprintf(buffer,"ERROR_0 ");
			break;
		case 1:
			sprintf(buffer,"ERROR ");
			break;	
		case 2:
			sprintf(buffer,"ERROR_2 ");
			break;	
		case 3:
			sprintf(buffer,"WARNNING ");
			break;	
		case 4:
			sprintf(buffer,"PROMPT ");
			break;	
		case 5:
			sprintf(buffer,"DEBUG ");
			break;	
		default:
			sprintf(buffer,"OH:%d ",pp->loglevel);
			break;
	}
	if (m_nId >=0)
	{
		//sprintf(headbuffer,"%s %06ld %06ld ID[%d] FILE[%s]LINE[%d] %s ",m_pDate.ToStringT6().c_str(),pp->logus,m_pDate.GetUs(),m_nId,pp->filename,pp->fileline,buffer);
		sprintf(headbuffer,"%s %06ld ID[%d] FILE[%s]LINE[%d] %s ",m_pDate.ToStringT6().c_str(),pp->logus,m_nId,pp->filename,pp->fileline,buffer);
		//sprintf(headbuffer,"%s %d ID[%d] FILE[%s]LINE[%d] %s ",m_pDate.ToStringT6().c_str(),m_pDate.GetUs(),m_nId,pp->filename,pp->fileline,buffer);
		fprintf(m_pFp,headbuffer);
	}
	else
	{
		//sprintf(headbuffer,"%s %06ld %06ld ID[%d] FILE[%s]LINE[%d] %s ",m_pDate.ToStringT6().c_str(),pp->logus,m_pDate.GetUs(),GetPid(),pp->filename,pp->fileline,buffer);
		sprintf(headbuffer,"%s %06ld ID[%d] FILE[%s]LINE[%d] %s ",m_pDate.ToStringT6().c_str(),pp->logus,pp->threadid,pp->filename,pp->fileline,buffer);
		fprintf(m_pFp,headbuffer);
	}
#else
	switch (pp->loglevel)
	{
	case -1:
		sprintf(buffer,"info ");
		break;
	case 0:
		sprintf(buffer,"严重 ");
		break;
	case 1:
		sprintf(buffer,"错误 ");
		break;	
	case 2:
		sprintf(buffer,"一般 ");
		break;	
	case 3:
		sprintf(buffer,"警告 ");
		break;	
	case 4:
		sprintf(buffer,"提示 ");
		break;	
	case 5:
		sprintf(buffer,"调试 ");
		break;	
	default:
		sprintf(buffer,"其它:%d ",pp->loglevel);
		break;
	}
	
	if (m_nId >=0)
	{
		sprintf(headbuffer,"%s %06ld ID[%d] 文件[%s]行[%d] %s ",m_pDate.ToStringT6().c_str(),pp->logus,m_nId,pp->filename,pp->fileline,buffer);
		//sprintf(headbuffer,"%s %d ID[%d] 文件[%s]行[%d] %s ",m_pDate.ToStringT6().c_str(),m_pDate.GetUs(),m_nId,pp->filename,pp->fileline,buffer);
		fprintf(m_pFp,headbuffer);
	}
	else
	{
		sprintf(headbuffer,"%s %06ld ID[%d] 文件[%s]行[%d] %s ",m_pDate.ToStringT6().c_str(),pp->logus, pp->threadid,pp->filename,pp->fileline,buffer);
		//sprintf(headbuffer,"%s %d ID[%d] 文件[%s]行[%d] %s ",m_pDate.ToStringT6().c_str(),m_pDate.GetUs(),GetPid(),pp->filename,pp->fileline,buffer);
		fprintf(m_pFp,headbuffer);
	}
#endif
	if (m_nMaxLogSize >=1024 )
	{
		m_nCurLogSize += strlen(buffer);
		m_nCurLogSize += strlen(headbuffer);
	}
	if (pp->wtype == 1)
	{
		fprintf(m_pFp,"\n[");
		for (int j=0 ;j<pp->datalen ;j++)
		{
			
			if (pp->data[j] == 0)    
			{
				fputc('*',m_pFp);
			}
			else
			{
				fputc(pp->data[j],m_pFp);
			}
		}
		fprintf(m_pFp,"]\n");
		if (m_nMaxLogSize >=1024 )
		{
			m_nCurLogSize += pp->datalen;
			m_nCurLogSize += 4;
		}
	}
	else  // LOGMP
	{
		
		fprintf(m_pFp,"%s \n",pp->data);
		if (m_nMaxLogSize >=1024 )
		{
			m_nCurLogSize += pp->datalen;
			m_nCurLogSize += 2;
		}
	}
	fflush(m_pFp);
	if (m_nCurLogSize > m_nMaxLogSize && m_nMaxLogSize != 0)
	{
		m_nCurLogSize = 0;
		//切文件
		fclose(m_pFp);
		m_pFp = NULL;
		char  rlogfilepathname[500];
		sprintf(rlogfilepathname,"%s.%s",m_sLogFilePathName,m_pDate.ToStringT6().c_str());
		rename(m_sLogFilePathName,rlogfilepathname);
		if (!OpenLogFile())
		{
			return false;
		}
	}
	return true;
}



bool CBF_LogClient::OpenLogFile()
{
	if (m_pFp != NULL)
	{
		return false;
	}
	char fpathname[300];
	memset(fpathname,0,sizeof(fpathname));
	m_pDate.Update();
	if (m_nMaxLogSize < 1024)  //不限制大小
	{
#ifdef _WINDOWS	
		sprintf(m_sLogFilePathName,"%s\\%s%s",m_sLogPath.c_str(),m_pDate.ToStringD8().c_str(),m_sLogName.c_str());
#else	
		sprintf(m_sLogFilePathName,"%s/%s%s",m_sLogPath.c_str(),m_pDate.ToStringD8().c_str(),m_sLogName.c_str());
#endif
	}
	else //限制文件大小
	{
#ifdef _WINDOWS	
		sprintf(fpathname,"%s\\%s",m_sLogPath.c_str(),m_pDate.ToStringD8().c_str());
#else	
		sprintf(fpathname,"%s/%s",m_sLogPath.c_str(),m_pDate.ToStringD8().c_str());
#endif
		if (!CBF_Tools::MakeDir(fpathname))
		{
			printf("建立目录[%s]失败",fpathname);
			fprintf(stderr,"%s 建立目录[%s]失败\n",m_pDate.ToStringDT17().c_str(),fpathname);
			return false;
		}
#ifdef _WINDOWS	
		sprintf(m_sLogFilePathName,"%s\\%s",fpathname,m_sLogName.c_str());
#else	
		sprintf(m_sLogFilePathName,"%s/%s",fpathname,m_sLogName.c_str());
#endif
	}

	m_pFp = fopen(m_sLogFilePathName,"a");
	if (m_pFp == NULL)
	{
		printf("打开日志文件失败[%s]",m_sLogFilePathName);
		fprintf(stderr,"%s 打开日志文件失败 [%s]\n",m_pDate.ToStringDT17().c_str(),m_sLogFilePathName);
		return false;
	}
	if (m_nMaxLogSize >=1024)  //限制大小
	{
		struct_stat fs;
		FSTAT(m_sLogFilePathName,&fs);
		m_nCurLogSize = fs.st_size;
	}
	return true;
}

bool CBF_LogClient::StartLog()
{
	if (!m_bIsSetPara)
	{
		return false;
	}
	if (m_bIsLocal)
	{
		m_pDate.Update();
		m_sCurDate = m_pDate.ToStringD8();
		m_pTimer.Stop();
		m_pTimer.SetTimer(0,5000,&CBF_LogClient::DateChange,this,0);
//		m_pTimer.SetTimer(1,5000,&CBF_LogClient::DateChange,this,0);
//		m_pTimer.Start();
	}
	if (IsStoped())
	{
		m_bIsExit = false;
		CreateThread();
	}
	return true;
}

int CBF_LogClient::DateChange(unsigned int eventid, void *p)
{
	CBF_LogClient *pp = (CBF_LogClient *)p;
	//若当天还剩下小于10秒时间且日切检查状态仍为未检查时，置状态为检查，
	//由主线程检查日期是否变化，若变化则处理日切并置状态为未检查时
	if (eventid == 0)
	{
		if (pp->m_pDate.GetRemainSecCurDate() <10  && !pp->m_bIsCheckDate)
		{
			pp->m_bIsCheckDate = true;
		}
		pp->m_bIsPingTimer = true;
	}
	else
	{
		
	}
	return 0;
}


void CBF_LogClient::StopLog()
{
	m_bIsExit = true;
//	m_pTimer.Stop();
	Join();
}
bool CBF_LogClient::IsStopedThread()
{
	return IsStoped();
}
void CBF_LogClient::SetMaxLogSize(int size)
{
	if (size <1024)
	{
		m_nMaxLogSize = 0;
	}
	else
	{
		m_nMaxLogSize = size;
	}
}

int CBF_LogClient::RunLocal()
{
	char *data=NULL;
	
	m_pDate.Update();
	while (!m_bIsExit)
	{
		m_pDate.Update();
		if (m_sCurDate.compare(m_pDate.ToStringD8()) != 0) //日期变化了
		{
			m_sCurDate = m_pDate.ToStringD8();
			m_bIsCheckDate = false;//定时器开始
			if (m_pFp != NULL)
			{
				fclose(m_pFp);
				m_pFp = NULL;
			}
			if (!OpenLogFile())
			{
				return -1;
			}
		}
		
		if (m_pQueue.GetData(data))
		{
			m_pDate.Update();
			if (m_sCurDate.compare(m_pDate.ToStringD8()) != 0) //日期变化了
			{
				m_sCurDate = m_pDate.ToStringD8();
				m_bIsCheckDate = false;//定时器开始
				if (m_pFp != NULL)
				{
					fclose(m_pFp);
					m_pFp = NULL;
				}
				if (!OpenLogFile())
				{
					return -1;
				}
			}
			if (NULL == m_pFp )
			{
				if (!OpenLogFile())
				{
					fprintf(stderr,"打开日志文件错\n");
					m_pMemPool.PoolFree(data);
					return -1;
				}
			}
			//写入日期
			WriteLog(data);
			m_pMemPool.PoolFree(data);
			data = NULL;
		}
		else
		{
			m_pDate.Update();
			if (m_sCurDate.compare(m_pDate.ToStringD8()) != 0) //日期变化了
			{
				m_sCurDate = m_pDate.ToStringD8();
				m_bIsCheckDate = false;//定时器开始
				if (m_pFp != NULL)
				{
					fclose(m_pFp);
					m_pFp = NULL;
				}
				if (!OpenLogFile())
				{
					return -1;
				}
			}
		}
	}
	return 0;
}

int CBF_LogClient::RunServer()
{
	struct timeval tv;
	int result;
	int maxfd=0;
	int lasttime=time(NULL);
	int lasttime2=time(NULL);
	while (!m_bIsExit)
	{
		FD_ZERO(&m_rset);
		FD_ZERO(&m_wset);
#if defined(_WINDOWS)
		FD_ZERO(&m_eset);
#endif
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		//取得最大的socket并将socket加入到fd_set集合
		if (INVALID_SOCKET == m_sock )
		{
			OnConnect();
			continue;
		}
		maxfd = m_sock;

#if defined(_WINDOWS)
		result = select(maxfd+1, &m_rset, &m_wset, &m_eset, &tv);
#else
		result = select(maxfd+1, &m_rset, &m_wset, NULL, &tv);
#endif
		
		if (result != 0) 
		{
			//有事件发生，处理
			OnEvent();
		}
		else
		{
			OnNoEvent();
		}
		if (m_bIsPingTimer)
		{
			OnPing();
			m_bIsPingTimer = false;
		}

	}
//	m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"停止日志服务器线程 %d",m_bIsExit);
	return 0;
}

void CBF_LogClient::OnConnect()
{
	//去连接
	if (time(NULL) - m_nConntime < 5 ) //在5秒内不用重连
	{
		return;
	}
	m_nConntime = time(NULL);
	m_bIsConnect = true;
	m_pTcpSocket.Create(AF_INET,false);//异步阻塞方式
#ifdef _ENGLISH_
	m_log.LogMp(LOG_PROMPT,__FILE__,__LINE__,"begin connect %s:%d",m_sIp,m_nPort);
#else
	m_log.LogMp(LOG_PROMPT,__FILE__,__LINE__,"开始连接%s:%d",m_sIp,m_nPort);
#endif
	if (m_pTcpSocket.ConnectServer(m_sIp,m_nPort))
	{
		
		m_nReadtime = time(NULL);
		m_sock = m_pTcpSocket.GetSocket();
	}
	else
	{
		if ((m_pTcpSocket.GetErrorNo() == EWOULDBLOCK) || (m_pTcpSocket.GetErrorNo() == EINPROGRESS))
		{
			m_nReadtime = time(NULL);
			m_sock = m_pTcpSocket.GetSocket();
#ifdef _ENGLISH_
			m_log.LogMp(LOG_PROMPT,__FILE__,__LINE__,"wait connect %s:%d",m_sIp,m_nPort);
#else
			m_log.LogMp(LOG_PROMPT,__FILE__,__LINE__,"等待建立到%s:%d的连接",m_sIp,m_nPort);
#endif
	
			return;
		}
		else
		{
			m_sock = INVALID_SOCKET;
			m_pTcpSocket.Close();
		}
	}
	return;
}

void CBF_LogClient::OnEvent()
{
	int ret;
	ret = FD_ISSET(m_sock,&m_rset);
	if (ret != 0) //有读事件
	{
		OnRecv();
	}
	if (m_sock != INVALID_SOCKET)
	{
#if defined(_WINDOWS)
		ret = FD_ISSET(m_sock, &m_eset);
		if (ret != 0)
		{
#ifdef _ENGLISH_
			m_log.LogMp(LOG_ERROR,__FILE__,__LINE__,"try connect %s:%d faild",m_sIp,m_nPort);
#else
			m_log.LogMp(LOG_ERROR,__FILE__,__LINE__,"尝试连接到%s:%d未成功",m_sIp,m_nPort);
#endif
			closesocket(m_sock);
			m_sock = INVALID_SOCKET;
		}
		else if (FD_ISSET(m_sock, &m_wset) !=0 )
		{
			//可写
			if (m_bIsConnect)
			{
#ifdef _ENGLISH_
				m_log.LogMp(LOG_PROMPT,__FILE__,__LINE__,"connect %s:%d success",m_sIp,m_nPort);
#else
				m_log.LogMp(LOG_PROMPT,__FILE__,__LINE__,"建立到%s:%d的连接成功",m_sIp,m_nPort);
#endif
				m_nPingTime = time(NULL);
				m_nReadtime = time(NULL);
				m_bIsConnect = false;
			}
			else
			{
				OnSend();//发送数据
			}
		}
// 		else  
// 		{
// 			OnSend();//发送数据
// 		}
#else
		
		int ok;
		int oklen;
		if (FD_ISSET(m_sock, &m_wset) !=0)
		{
			if (m_bIsConnect)
			{
				oklen=sizeof(ok);
#if defined(HP_UX)
				getsockopt(m_sock,SOL_SOCKET,SO_ERROR,(char *)&ok,&oklen);
#else	
				getsockopt(m_sock,SOL_SOCKET,SO_ERROR,(char *)&ok,(socklen_t *)&oklen);
#endif
				if (ok == 0)
				{
#ifdef _ENGLISH_
					m_log.LogMp(LOG_PROMPT,__FILE__,__LINE__,"connect %s:%d success",m_sIp,m_nPort);
#else
					m_log.LogMp(LOG_PROMPT,__FILE__,__LINE__,"建立到%s:%d的连接成功",m_sIp,m_nPort);
#endif
					m_nPingTime = time(NULL);
					m_bIsConnect = false;
					m_nReadtime = time(NULL);
				}
				else
				{
#ifdef _ENGLISH_
					m_log.LogMp(LOG_PROMPT,__FILE__,__LINE__,"try connect %s:%d faild",m_sIp,m_nPort);
#else
					m_log.LogMp(LOG_ERROR,__FILE__,__LINE__,"尝试连接到%s:%d未成功",m_sIp,m_nPort);
#endif
					closesocket(m_sock);
					m_sock = INVALID_SOCKET;
				} // end if (ok == 0)
			}
			else
			{
				OnSend();//发送数据
			} // end if (m_bIsConnect)
		}
// 		else
// 		{
// 			OnSend(conindex);//发送数据
// 		}
#endif
	}
}

void CBF_LogClient::OnNoEvent()
{
	int delnum = m_pQueue.GetDataCount() - LOG_QUEUEMAXNUM;
	char *data= NULL;
	for (int i=0 ; i < delnum ; i++)
	{
		if (m_pQueue.GetData(data,0))
		{
			S_LOG_DATA *logdata = (S_LOG_DATA *)data;
#ifdef _ENGLISH_
			m_log.LogBin(LOG_WARNNING,logdata->filename,logdata->fileline,"not send to server",logdata->data,logdata->datalen);
#else
			m_log.LogBin(LOG_WARNNING,logdata->filename,logdata->fileline,"未发至日志服务器的日志",logdata->data,logdata->datalen);
#endif
			m_pMemPool.PoolFree(data);
			data = NULL;
		}
	}
	return;
}

void CBF_LogClient::OnPing()
{
	int curtime =  time(NULL);
	if (curtime - m_nPingTime < 10) //发完心跳10秒内不用再发了
	{
		return;
	}
	if (curtime - m_nReadtime > 10 || curtime - m_nWriteTime >10) //10秒
	{
		//发送心跳
		m_nPingTime = time(NULL);
	}
	if (curtime - m_nReadtime > 30) //30秒仍未收到数据，说明连接出问题了，关闭之
	{
		OnClose();
	}
}

void CBF_LogClient::OnRecv()
{
	int wantlen = sizeof(m_sRcvBuffer) - m_nRcvLen;
	m_pTcpSocket.AttachSocket(m_sock,m_sIp);
	int ret = m_pTcpSocket.Receive(m_sRcvBuffer+m_nRcvLen,wantlen);
	if (ret <=0)
	{
#ifdef _ENGLISH_
		m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"%s client close  ret=[%d]",m_sIp,ret);
#else
		m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"%s 远端关闭，接收数据不完整 ret=[%d]",m_sIp,ret);
#endif
		OnClose();
		return;
	}
	m_nRcvLen += ret;
	//接收处理
	m_nReadtime = time(NULL);
	int packlen=0;
	char head[LOGSERVER_HEADLEN+1];
	
	while (m_nRcvLen >= LOGSERVER_HEADLEN)
	{
		bzero(head,sizeof(head));
		memcpy(head,m_sRcvBuffer,LOGSERVER_HEADLEN);
		packlen = atoi(head);
		if (packlen == 0)
		{
			packlen = LOGSERVER_HEADLEN;
		}
		else
		{

			packlen = packlen+  LOGSERVER_HEADLEN;
			if (packlen > (int)m_nRcvLen) //数据未收完
			{
				break;
			}
		}
		if (packlen > LOGSERVER_HEADLEN)
		{
//			memcpy(pbuf,m_sRcvBuffer,packlen -LOGSERVER_HEADLEN )
			//处理日志服务器的数据
		}
		

		//收多了，将原包重新给m_sRcvBuffer
		for (int i=0; i<(int)m_nRcvLen-packlen; i++)
		{
			m_sRcvBuffer[i]=m_sRcvBuffer[i+packlen];
		}
		m_nRcvLen=m_nRcvLen - packlen;

	}


}

void CBF_LogClient::OnSend()
{
	m_pTcpSocket.AttachSocket(m_sock,m_sIp);
	int nRet=0;
	//先发送上次未发送的数据
	if (m_nSendLen >0)
	{
		nRet = m_pTcpSocket.Send(m_sSendBuffer,m_nSendLen);
		if (nRet <=0 ) //发送不成功
		{
			return;
		}
		if (nRet != m_nSendLen) //发送部分
		{
			//数据前移，以备下次再发
			for (int i=0; i<(int)m_nSendLen-nRet; i++)
			{
				m_sSendBuffer[i]=m_sSendBuffer[i+nRet];
			}
			m_nSendLen=m_nSendLen - nRet;
			return;
		}
		m_nSendLen = 0;
	}
	//开始发送队列里的数据
	int queuesize = m_pQueue.GetDataCount();
	int sendnum=0;
	//发送窗口，最大16，即最多发16条后去收数据
	if (queuesize > 16)
	{
		sendnum = 16;
	}
	else
	{
		sendnum = queuesize;
	}
	char *data = NULL;
	for (int i=0; i< sendnum ; i++)
	{
		if (!m_pQueue.GetData(data,0))
		{
		 return;
		}
		S_LOG_DATA *logdata  = (S_LOG_DATA *)data;
		m_nSendLen = S_LOG_HEADLEN + LOGSERVER_HEADLEN + logdata->datalen;
		sprintf(m_sSendBuffer,"%04d",logdata->datalen+S_LOG_HEADLEN);
		//字节序转换，统一转为LE  当本机为BE时反转
		CBF_Tools::Reverse((unsigned char *)logdata->datalen,sizeof(logdata->datalen));
		CBF_Tools::Reverse((unsigned char *)logdata->fileline,sizeof(logdata->fileline));
		memcpy(m_sSendBuffer+LOGSERVER_HEADLEN,data,m_nSendLen-LOGSERVER_HEADLEN);
		m_pMemPool.PoolFree(data);
		data = NULL;
		nRet = m_pTcpSocket.Send(m_sSendBuffer,m_nSendLen);
		if (nRet <=0 ) //发送不成功
		{
			return;
		}
		if (nRet != m_nSendLen) //发送部分
		{
			//数据前移，以备下次再发
			for (int i=0; i<(int)m_nSendLen-nRet; i++)
			{
				m_sSendBuffer[i]=m_sSendBuffer[i+nRet];
			}
			m_nSendLen=m_nSendLen - nRet;
			return;
		}
		m_nSendLen = 0;
	}
}

void CBF_LogClient::OnClose()
{
	closesocket(m_sock);
	m_sock = INVALID_SOCKET;
	m_nRcvLen = 0;
	m_nSendLen = 0;
	return;
}

int CBF_LogClient::GetLogLevel()
{
	return m_nLogLevel;
}

void CBF_LogClient::SetLogLevel(int loglevel)
{
	m_nLogLevel = loglevel;
}
