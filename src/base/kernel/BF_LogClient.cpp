// BF_LogClient.cpp: implementation of the CBF_LogClient class.
//
//////////////////////////////////////////////////////////////////////

#include "BF_LogClient.h"
#include "BF_Tools.h"
#include "BF_Slist.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define  LOGSERVER_HEADLEN   4   //����ͷ����


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
		//��������Ƿ�仯 m_bIsCheckDate�ɶ�ʱ�����ı䣬������ʱ���������������
		if (m_bIsCheckDate)
		{
			if (m_sCurDate.compare(m_pDate.ToStringD8()) != 0)
			{
				m_sCurDate = m_pDate.ToStringD8();
				//���ö�ʱ�� Ϊ��һ�����ڵ�ǰ5��ʱ��
				//m_pTimer.SetTimer(0,(m_pDate.GetRemainSecCurDate()-5)*1000,&CBF_LogClient::DateChange,this,0);
				m_bIsCheckDate = false;
				if (m_pFp != NULL)
				{
					fclose(m_pFp);
					m_pFp = NULL;
				}
				if (!OpenLogFile())
				{
					fprintf(stderr,"����־�ļ���\n");
					m_pMemPool.PoolFree(data);
					continue; ;
				}
			}
			
		}
		if (m_pFp == NULL)
		{
			if (!OpenLogFile())
			{
				fprintf(stderr,"����־�ļ���\n");
				m_pMemPool.PoolFree(data);
				continue; ;
			}
		}
		//д������
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
			fprintf(stderr,"����Ŀ¼%sʧ��\n",tmpchar);
			return;
		}

		m_sLogPath = tmpchar;
#else
#ifdef LOGCURPATH
		WORKPATH(tmpchar);
		strcat(tmpchar,"/log");
		if (!CBF_Tools::MakeDir(tmpchar))
		{
			fprintf(stderr,"����Ŀ¼%sʧ��\n",tmpchar);
			return;
		}
		m_sLogPath = tmpchar;
#else	
		sprintf(tmpchar,"%s/log",(char *)getenv("HOME"));
		if (!CBF_Tools::MakeDir(tmpchar))
		{
			fprintf(stderr,"����Ŀ¼%sʧ��\n",tmpchar);
			return;
		}
		m_sLogPath = tmpchar;
#endif
#endif
		//д�뱾��
		m_bIsLocal = true;
		m_pDate.Update();
		if (m_pFp != NULL)
		{
			fclose(m_pFp);
			m_pFp = NULL;
		}
		if (!OpenLogFile())
		{
			fprintf(stderr,"����־�ļ���\n");
		}
	}
	else
	{
		//�����Ǳ��ػ���Զ��
		CBF_Slist psep("@");
		if (psep.FillSepString(logfilepath)<2) 
		{
			//д�뱾��
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
				fprintf(stderr,"����־�ļ���\n");
			}
			
		}
		else
		{
			//Զ�̷�����
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
	//ȥ��·��
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

//	printf("��ʱ  %lld %lld %lld %lld %lld\n",etime4-etime3-(btime1-btime0),etime3-etime2-(btime1-btime0),etime2-etime1-(btime1-btime0),etime1-btime1-(btime1-btime0),btime1-btime0);
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
	//ȥ��·��
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
	sprintf(data->data,"����[%s]\n[",title);
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
	//0���ش���1��Ҫ����2һ�����3����4��ʾ����5������Ϣ
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
		sprintf(buffer,"���� ");
		break;
	case 1:
		sprintf(buffer,"���� ");
		break;	
	case 2:
		sprintf(buffer,"һ�� ");
		break;	
	case 3:
		sprintf(buffer,"���� ");
		break;	
	case 4:
		sprintf(buffer,"��ʾ ");
		break;	
	case 5:
		sprintf(buffer,"���� ");
		break;	
	default:
		sprintf(buffer,"����:%d ",pp->loglevel);
		break;
	}
	
	if (m_nId >=0)
	{
		sprintf(headbuffer,"%s %06ld ID[%d] �ļ�[%s]��[%d] %s ",m_pDate.ToStringT6().c_str(),pp->logus,m_nId,pp->filename,pp->fileline,buffer);
		//sprintf(headbuffer,"%s %d ID[%d] �ļ�[%s]��[%d] %s ",m_pDate.ToStringT6().c_str(),m_pDate.GetUs(),m_nId,pp->filename,pp->fileline,buffer);
		fprintf(m_pFp,headbuffer);
	}
	else
	{
		sprintf(headbuffer,"%s %06ld ID[%d] �ļ�[%s]��[%d] %s ",m_pDate.ToStringT6().c_str(),pp->logus, pp->threadid,pp->filename,pp->fileline,buffer);
		//sprintf(headbuffer,"%s %d ID[%d] �ļ�[%s]��[%d] %s ",m_pDate.ToStringT6().c_str(),m_pDate.GetUs(),GetPid(),pp->filename,pp->fileline,buffer);
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
		//���ļ�
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
	if (m_nMaxLogSize < 1024)  //�����ƴ�С
	{
#ifdef _WINDOWS	
		sprintf(m_sLogFilePathName,"%s\\%s%s",m_sLogPath.c_str(),m_pDate.ToStringD8().c_str(),m_sLogName.c_str());
#else	
		sprintf(m_sLogFilePathName,"%s/%s%s",m_sLogPath.c_str(),m_pDate.ToStringD8().c_str(),m_sLogName.c_str());
#endif
	}
	else //�����ļ���С
	{
#ifdef _WINDOWS	
		sprintf(fpathname,"%s\\%s",m_sLogPath.c_str(),m_pDate.ToStringD8().c_str());
#else	
		sprintf(fpathname,"%s/%s",m_sLogPath.c_str(),m_pDate.ToStringD8().c_str());
#endif
		if (!CBF_Tools::MakeDir(fpathname))
		{
			printf("����Ŀ¼[%s]ʧ��",fpathname);
			fprintf(stderr,"%s ����Ŀ¼[%s]ʧ��\n",m_pDate.ToStringDT17().c_str(),fpathname);
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
		printf("����־�ļ�ʧ��[%s]",m_sLogFilePathName);
		fprintf(stderr,"%s ����־�ļ�ʧ�� [%s]\n",m_pDate.ToStringDT17().c_str(),m_sLogFilePathName);
		return false;
	}
	if (m_nMaxLogSize >=1024)  //���ƴ�С
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
	//�����컹ʣ��С��10��ʱ�������м��״̬��Ϊδ���ʱ����״̬Ϊ��飬
	//�����̼߳�������Ƿ�仯�����仯�������в���״̬Ϊδ���ʱ
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
		if (m_sCurDate.compare(m_pDate.ToStringD8()) != 0) //���ڱ仯��
		{
			m_sCurDate = m_pDate.ToStringD8();
			m_bIsCheckDate = false;//��ʱ����ʼ
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
			if (m_sCurDate.compare(m_pDate.ToStringD8()) != 0) //���ڱ仯��
			{
				m_sCurDate = m_pDate.ToStringD8();
				m_bIsCheckDate = false;//��ʱ����ʼ
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
					fprintf(stderr,"����־�ļ���\n");
					m_pMemPool.PoolFree(data);
					return -1;
				}
			}
			//д������
			WriteLog(data);
			m_pMemPool.PoolFree(data);
			data = NULL;
		}
		else
		{
			m_pDate.Update();
			if (m_sCurDate.compare(m_pDate.ToStringD8()) != 0) //���ڱ仯��
			{
				m_sCurDate = m_pDate.ToStringD8();
				m_bIsCheckDate = false;//��ʱ����ʼ
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
		//ȡ������socket����socket���뵽fd_set����
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
			//���¼�����������
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
//	m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"ֹͣ��־�������߳� %d",m_bIsExit);
	return 0;
}

void CBF_LogClient::OnConnect()
{
	//ȥ����
	if (time(NULL) - m_nConntime < 5 ) //��5���ڲ�������
	{
		return;
	}
	m_nConntime = time(NULL);
	m_bIsConnect = true;
	m_pTcpSocket.Create(AF_INET,false);//�첽������ʽ
#ifdef _ENGLISH_
	m_log.LogMp(LOG_PROMPT,__FILE__,__LINE__,"begin connect %s:%d",m_sIp,m_nPort);
#else
	m_log.LogMp(LOG_PROMPT,__FILE__,__LINE__,"��ʼ����%s:%d",m_sIp,m_nPort);
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
			m_log.LogMp(LOG_PROMPT,__FILE__,__LINE__,"�ȴ�������%s:%d������",m_sIp,m_nPort);
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
	if (ret != 0) //�ж��¼�
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
			m_log.LogMp(LOG_ERROR,__FILE__,__LINE__,"�������ӵ�%s:%dδ�ɹ�",m_sIp,m_nPort);
#endif
			closesocket(m_sock);
			m_sock = INVALID_SOCKET;
		}
		else if (FD_ISSET(m_sock, &m_wset) !=0 )
		{
			//��д
			if (m_bIsConnect)
			{
#ifdef _ENGLISH_
				m_log.LogMp(LOG_PROMPT,__FILE__,__LINE__,"connect %s:%d success",m_sIp,m_nPort);
#else
				m_log.LogMp(LOG_PROMPT,__FILE__,__LINE__,"������%s:%d�����ӳɹ�",m_sIp,m_nPort);
#endif
				m_nPingTime = time(NULL);
				m_nReadtime = time(NULL);
				m_bIsConnect = false;
			}
			else
			{
				OnSend();//��������
			}
		}
// 		else  
// 		{
// 			OnSend();//��������
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
					m_log.LogMp(LOG_PROMPT,__FILE__,__LINE__,"������%s:%d�����ӳɹ�",m_sIp,m_nPort);
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
					m_log.LogMp(LOG_ERROR,__FILE__,__LINE__,"�������ӵ�%s:%dδ�ɹ�",m_sIp,m_nPort);
#endif
					closesocket(m_sock);
					m_sock = INVALID_SOCKET;
				} // end if (ok == 0)
			}
			else
			{
				OnSend();//��������
			} // end if (m_bIsConnect)
		}
// 		else
// 		{
// 			OnSend(conindex);//��������
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
			m_log.LogBin(LOG_WARNNING,logdata->filename,logdata->fileline,"δ������־����������־",logdata->data,logdata->datalen);
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
	if (curtime - m_nPingTime < 10) //��������10���ڲ����ٷ���
	{
		return;
	}
	if (curtime - m_nReadtime > 10 || curtime - m_nWriteTime >10) //10��
	{
		//��������
		m_nPingTime = time(NULL);
	}
	if (curtime - m_nReadtime > 30) //30����δ�յ����ݣ�˵�����ӳ������ˣ��ر�֮
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
		m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"%s Զ�˹رգ��������ݲ����� ret=[%d]",m_sIp,ret);
#endif
		OnClose();
		return;
	}
	m_nRcvLen += ret;
	//���մ���
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
			if (packlen > (int)m_nRcvLen) //����δ����
			{
				break;
			}
		}
		if (packlen > LOGSERVER_HEADLEN)
		{
//			memcpy(pbuf,m_sRcvBuffer,packlen -LOGSERVER_HEADLEN )
			//������־������������
		}
		

		//�ն��ˣ���ԭ�����¸�m_sRcvBuffer
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
	//�ȷ����ϴ�δ���͵�����
	if (m_nSendLen >0)
	{
		nRet = m_pTcpSocket.Send(m_sSendBuffer,m_nSendLen);
		if (nRet <=0 ) //���Ͳ��ɹ�
		{
			return;
		}
		if (nRet != m_nSendLen) //���Ͳ���
		{
			//����ǰ�ƣ��Ա��´��ٷ�
			for (int i=0; i<(int)m_nSendLen-nRet; i++)
			{
				m_sSendBuffer[i]=m_sSendBuffer[i+nRet];
			}
			m_nSendLen=m_nSendLen - nRet;
			return;
		}
		m_nSendLen = 0;
	}
	//��ʼ���Ͷ����������
	int queuesize = m_pQueue.GetDataCount();
	int sendnum=0;
	//���ʹ��ڣ����16������෢16����ȥ������
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
		//�ֽ���ת����ͳһתΪLE  ������ΪBEʱ��ת
		CBF_Tools::Reverse((unsigned char *)logdata->datalen,sizeof(logdata->datalen));
		CBF_Tools::Reverse((unsigned char *)logdata->fileline,sizeof(logdata->fileline));
		memcpy(m_sSendBuffer+LOGSERVER_HEADLEN,data,m_nSendLen-LOGSERVER_HEADLEN);
		m_pMemPool.PoolFree(data);
		data = NULL;
		nRet = m_pTcpSocket.Send(m_sSendBuffer,m_nSendLen);
		if (nRet <=0 ) //���Ͳ��ɹ�
		{
			return;
		}
		if (nRet != m_nSendLen) //���Ͳ���
		{
			//����ǰ�ƣ��Ա��´��ٷ�
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
