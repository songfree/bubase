// BF_CommDataLog.cpp: implementation of the CBF_CommDataLog class.
//
//////////////////////////////////////////////////////////////////////

#include "BF_CommDataLog.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBF_CommDataLog::CBF_CommDataLog()
{
	m_nMaxLogSize=0;//��־�ļ���С 0������һֱд
	
	m_nCurDrebLogSize=0;//��ǰdreb��־��С
	m_nCurBpcLogSize=0;//��ǰBPC��־��С
	m_nCurDrebErrLogSize=0;//��ǰdreb������־��С
	m_nCurBpcErrLogSize=0;//��ǰBPC������־��С
	m_pBpcErrFile = NULL;
	m_pBpcFile = NULL;
	m_pDrebFile = NULL;
	m_pDrebErrFile = NULL;
	m_bIsExit = true;
	m_bIsCheckDate = false;
	m_bIsSetPara = false;
	m_nLogLevel = LOG_DEBUG;
	m_pMemPool.Init(100,DREBBUFSIZE+50);

	bzero(m_sDrebLogFilePathName,sizeof(m_sDrebLogFilePathName));
	bzero(m_sDrebErrLogFilePathName,sizeof(m_sDrebErrLogFilePathName));
	bzero(m_sBpcLogFilePathName,sizeof(m_sBpcLogFilePathName));
	bzero(m_sBpcErrLogFilePathName,sizeof(m_sBpcErrLogFilePathName));
	if (htonl(1) == 1)
	{
		m_bIsBigEndian = true;//������
	}
	else
	{
		m_bIsBigEndian = false;
	}
	m_pLog.SetLogPara(LOG_DEBUG,"","BF_CommDataLog.log");
}

CBF_CommDataLog::~CBF_CommDataLog()
{
	ClearQueueAndClose();
}
bool CBF_CommDataLog::InitThreadInstance()
{
	return true;
}


void CBF_CommDataLog::ExitThreadInstance()
{
	
}

int CBF_CommDataLog::Run()
{
	char *data=NULL;
	
	m_pDate.Update();
	while (!m_bIsExit)
	{
		m_pDate.Update();
		//��������Ƿ�仯 m_bIsCheckDate�ɶ�ʱ�����ı䣬������ʱ���������������
		if (m_bIsCheckDate)
		{
			if (m_sCurDate.compare(m_pDate.ToStringD8()) != 0) //���ڱ仯��
			{
				m_sCurDate = m_pDate.ToStringD8();
				m_bIsCheckDate = false;//��ʱ����ʼ
				if (m_pDrebFile != NULL)
				{
					fclose(m_pDrebFile);
					m_pDrebFile = NULL;
				}
				if (m_pDrebErrFile != NULL)
				{
					fclose(m_pDrebErrFile);
					m_pDrebErrFile = NULL;
				}
				if (m_pBpcFile != NULL)
				{
					fclose(m_pBpcFile);
					m_pBpcFile = NULL;
				}
				if (m_pBpcErrFile != NULL)
				{
					fclose(m_pBpcErrFile);
					m_pBpcErrFile = NULL;
				}
				if (!OpenDrebLogFile())
				{
					return -1;
				}
				if (!OpenDrebErrLogFile())
				{
					return -1;
				}
				if (!OpenBpcLogFile())
				{
					return -1;
				}
				if (!OpenBpcErrLogFile())
				{
					return -1;
				}
			}
		}
		if (m_pQueue.GetData(data,200))
		{
			m_pDate.Update();
			//д������
			if (!WriteLog(data))
			{
				m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"д�ļ�����");
			}
			m_pMemPool.PoolFree(data);
			data = NULL;
		}
	}
	return 0;
}

bool CBF_CommDataLog::StartLog()
{
	if (!m_bIsSetPara)
	{
		m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"δ���ò���");
		return false;
	}

	m_pDate.Update();
	m_sCurDate = m_pDate.ToStringD8();
	m_pTimer.Stop();
	m_pTimer.SetTimer(0,5000,&CBF_CommDataLog::DateChange,this,0);
	m_pTimer.Start();
	
	//���ļ�

	if (IsStoped())
	{
		m_bIsExit = false;
		CreateThread();
	}
	return true;
}

int CBF_CommDataLog::DateChange(unsigned int eventid, void *p)
{
	CBF_CommDataLog *pp = (CBF_CommDataLog *)p;
	//��ʱ��ÿ��5���鵱��ʱ���Ƿ�ֻʣ10�룬������δ��֪ͨ�����������б�־֪ͨ���߳̽�������
	//���̼߳���־���Ϊtrue�����������Ƿ����仯���������������л����ñ�־Ϊfalse
	if (eventid == 0)
	{
		if (pp->m_pDate.GetRemainSecCurDate() <10  && !pp->m_bIsCheckDate)
		{
			pp->m_bIsCheckDate = true;
		}
	}
	else
	{
		
	}
	return 0;
}


void CBF_CommDataLog::StopLog()
{
	m_bIsExit = true;
	m_pTimer.Stop();
	m_pTimer.Join();
	Join();
	ClearQueueAndClose();
}

void CBF_CommDataLog::SetLogPara(int maxlogsize,int level, const char *logpath,const char *dreblogname,const char *bpclogname)
{
	if (maxlogsize <1024)
	{
		m_nMaxLogSize = 0;
	}
	else
	{
		m_nMaxLogSize = maxlogsize;
	}
	m_bIsSetPara = true;

	m_nLogLevel = level;
	m_sLogPath = logpath;
	m_sDrebLogName = dreblogname;
	m_sBpcLogName= bpclogname;
	m_sDrebErrLogName = m_sDrebLogName + ".Err";
	m_sBpcErrLogName = m_sBpcLogName+".Err";


	if (m_sLogPath.length()<1)
	{
		char tmpchar[300];
		memset(tmpchar,0,sizeof(tmpchar));
#ifdef _WINDOWS
		WORKPATH(tmpchar);
		strcat(tmpchar,"\\data");
		if (!CBF_Tools::MakeDir(tmpchar))
		{
			m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"����Ŀ¼%sʧ��",tmpchar);
			return;
		}
		
		m_sLogPath = tmpchar;
#else
#ifdef LOGCURPATH
		WORKPATH(tmpchar);
		strcat(tmpchar,"/data");
		if (!CBF_Tools::MakeDir(tmpchar))
		{
			m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"����Ŀ¼%sʧ��",tmpchar);
			return;
		}
		m_sLogPath = tmpchar;
#else	
		sprintf(tmpchar,"%s/data",(char *)getenv("HOME"));
		if (!CBF_Tools::MakeDir(tmpchar))
		{
			m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"����Ŀ¼%sʧ��",tmpchar);
			return;
		}
		m_sLogPath = tmpchar;
#endif
#endif
	}
	else
	{
		if (!CBF_Tools::MakeDir(m_sLogPath.c_str()))
		{
			m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"����Ŀ¼%sʧ��",m_sLogPath.c_str());
			return;
		}
	}
}

int CBF_CommDataLog::LogBpc(int level, PBPCCOMMSTRU data,bool flag)
{
	if (level > m_nLogLevel)
	{
		return -1;
	}
	char *tmpdata = (char *)m_pMemPool.PoolMalloc();
	if ( NULL == tmpdata )
	{
		m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"PoolMalloc����ռ�ʧ��");
		return -1;
	} 
	//�õ�һ���ֽڱ�־
	if (flag)  
	{
		tmpdata[0] =3;
	}
	else
	{
		tmpdata[0] = 2;
	}
	S_LOG_BPCHEAD *bpcdata = (S_LOG_BPCHEAD  *)(tmpdata+1);
	memcpy(&(bpcdata->data.sBpcHead),&(data->sBpcHead),BPCHEADLEN);
	memcpy(&(bpcdata->data.sDBHead),&(data->sDBHead),DREBHEADLEN);
	if (data->sDBHead.nLen>0)
	{
		memcpy(bpcdata->data.sBuffer,data->sBuffer,data->sDBHead.nLen);
	}
	m_pQueue.PushData(tmpdata);
	return 0;

}

int CBF_CommDataLog::LogDreb(int level, PBPCCOMMSTRU data,bool flag)
{
	if (level > m_nLogLevel)
	{
		return -1;
	}
	char *tmpdata = (char *)m_pMemPool.PoolMalloc();
	if ( NULL == tmpdata )
	{
		m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"PoolMalloc����ռ�ʧ��");
		return -1;
	} 
	//�õ�һ���ֽڱ�־
	if (flag)  
	{
		tmpdata[0] =1;
	}
	else
	{
		tmpdata[0] = 0;
	}
	char *pdata= tmpdata;
	pdata+=1;
	S_LOG_DREBHEAD *drebdata = (S_LOG_DREBHEAD *)(pdata);
	memcpy(&(drebdata->data.head),&(data->sDBHead),DREBHEADLEN);
	if (data->sDBHead.nLen >0)
	{
		memcpy(drebdata->data.buffer,data->sBuffer,data->sDBHead.nLen);
	}
	else
	{
		drebdata->data.buffer[0] = 0;
	}
	m_pQueue.PushData(tmpdata);
	return 0;
}
int CBF_CommDataLog::LogDreb(int level, PCOMMSTRU data,bool flag)
{
	if (level > m_nLogLevel)
	{
		return -1;
	}
	char *tmpdata = (char *)m_pMemPool.PoolMalloc();
	if ( NULL == tmpdata )
	{
		m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"PoolMalloc����ռ�ʧ��");
		return -1;
	} 
	//�õ�һ���ֽڱ�־
	if (flag)  
	{
		tmpdata[0] =1;
	}
	else
	{
		tmpdata[0] = 0;
	}
	char *pdata= tmpdata;
	pdata+=1;
	S_LOG_DREBHEAD *drebdata = (S_LOG_DREBHEAD *)(pdata);
	memcpy(&(drebdata->data.head),&(data->head),DREBHEADLEN);
	if (data->head.nLen>0)
	{
		memcpy(drebdata->data.buffer,data->buffer,data->head.nLen);
	}
	m_pQueue.PushData(tmpdata);
	return 0;
	
}
bool CBF_CommDataLog::OpenDrebLogFile()
{
	if (m_pDrebFile != NULL)
	{
		fclose(m_pDrebFile);
		m_pDrebFile = NULL;
		//return false;
	}
	char fpathname[300];
	memset(fpathname,0,sizeof(fpathname));
	m_pDate.Update();
	if (m_nMaxLogSize < 1024)  //�����ƴ�С
	{
#ifdef _WINDOWS	
		sprintf(m_sDrebLogFilePathName,"%s\\%s%s",m_sLogPath.c_str(),m_pDate.ToStringD8().c_str(),m_sDrebLogName.c_str());
#else	
		sprintf(m_sDrebLogFilePathName,"%s/%s%s",m_sLogPath.c_str(),m_pDate.ToStringD8().c_str(),m_sDrebLogName.c_str());
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
			m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"����Ŀ¼%sʧ��",fpathname);
			return false;
		}
#ifdef _WINDOWS	
		sprintf(m_sDrebLogFilePathName,"%s\\%s",fpathname,m_sDrebLogName.c_str());
#else	
		sprintf(m_sDrebLogFilePathName,"%s/%s",fpathname,m_sDrebLogName.c_str());
#endif
	}
	struct_stat fs;
	fs.st_size = 0;
	FSTAT(m_sDrebLogFilePathName,&fs);
	if (fs.st_size>0)
	{
		m_nCurDrebLogSize = fs.st_size;
	}
	else
	{
		m_nCurDrebLogSize = 0;
	}
	m_pDrebFile = fopen(m_sDrebLogFilePathName,"ab");
	if (m_pDrebFile == NULL)
	{
		m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"����־�ļ�%sʧ��",m_sDrebLogFilePathName);	
		return false;
	}
	
	if (m_nCurDrebLogSize<1)
	{
		if (m_bIsBigEndian)
		{
			sprintf(fpathname,"B0");//������
		}
		else
		{
			sprintf(fpathname,"L0");//������
		}
		fwrite(fpathname,1,2,m_pDrebFile);//д���ֽ����־
		m_nCurDrebLogSize = 2;
	}
	return true;
}
bool CBF_CommDataLog::OpenDrebErrLogFile()
{
	if (m_pDrebErrFile != NULL)
	{
		return false;
	}
	char fpathname[300];
	memset(fpathname,0,sizeof(fpathname));
	m_pDate.Update();
	if (m_nMaxLogSize < 1024)  //�����ƴ�С
	{
#ifdef _WINDOWS	
		sprintf(m_sDrebErrLogFilePathName,"%s\\%s%s",m_sLogPath.c_str(),m_pDate.ToStringD8().c_str(),m_sDrebErrLogName.c_str());
#else	
		sprintf(m_sDrebErrLogFilePathName,"%s/%s%s",m_sLogPath.c_str(),m_pDate.ToStringD8().c_str(),m_sDrebErrLogName.c_str());
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
			m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"����Ŀ¼%sʧ��",fpathname);
			return false;
		}
#ifdef _WINDOWS	
		sprintf(m_sDrebErrLogFilePathName,"%s\\%s",fpathname,m_sDrebErrLogName.c_str());
#else	
		sprintf(m_sDrebErrLogFilePathName,"%s/%s",fpathname,m_sDrebErrLogName.c_str());
#endif
	}
	struct_stat fs;
	fs.st_size = 0;
	FSTAT(m_sDrebErrLogFilePathName,&fs);
	if (fs.st_size>0)
	{
		m_nCurDrebErrLogSize = fs.st_size;
	}
	else
	{
		m_nCurDrebErrLogSize = 0;
	}
	m_pDrebErrFile = fopen(m_sDrebErrLogFilePathName,"ab");
	if (m_pDrebErrFile == NULL)
	{
		m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"����־�ļ�%sʧ��",m_sDrebErrLogFilePathName);
		return false;
	}
	
	if (m_nCurDrebErrLogSize<1)
	{
		if (m_bIsBigEndian)
		{
			sprintf(fpathname,"B0");//������
		}
		else
		{
			sprintf(fpathname,"L0");//������
		}
		fwrite(fpathname,1,2,m_pDrebErrFile);//д���ֽ����־
		m_nCurDrebErrLogSize = 2;
	}
	return true;	
}
bool CBF_CommDataLog::OpenBpcLogFile()
{
	if (m_pBpcFile != NULL)
	{
		return false;
	}
	char fpathname[300];
	memset(fpathname,0,sizeof(fpathname));
	m_pDate.Update();
	if (m_nMaxLogSize < 1024)  //�����ƴ�С
	{
#ifdef _WINDOWS	
		sprintf(m_sBpcLogFilePathName,"%s\\%s%s",m_sLogPath.c_str(),m_pDate.ToStringD8().c_str(),m_sBpcLogName.c_str());
#else	
		sprintf(m_sBpcLogFilePathName,"%s/%s%s",m_sLogPath.c_str(),m_pDate.ToStringD8().c_str(),m_sBpcLogName.c_str());
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
			m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"����Ŀ¼%sʧ��",fpathname);
			return false;
		}
#ifdef _WINDOWS	
		sprintf(m_sBpcLogFilePathName,"%s\\%s",fpathname,m_sBpcLogName.c_str());
#else	
		sprintf(m_sBpcLogFilePathName,"%s/%s",fpathname,m_sBpcLogName.c_str());
#endif
	}
	struct_stat fs;
	fs.st_size = 0;
	FSTAT(m_sBpcLogFilePathName,&fs);
	if (fs.st_size>0)
	{
		m_nCurBpcLogSize = fs.st_size;
	}
	else
	{
		m_nCurBpcLogSize = 0;
	}
	m_pBpcFile = fopen(m_sBpcLogFilePathName,"ab");
	if (m_pBpcFile == NULL)
	{
		m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"����־�ļ�%sʧ��",m_sBpcLogFilePathName);
		return false;
	}
	
	if (m_nCurBpcLogSize<1)
	{
		if (m_bIsBigEndian)
		{
			sprintf(fpathname,"B1");//������
		}
		else
		{
			sprintf(fpathname,"L1");//������
		}
		fwrite(fpathname,1,2,m_pBpcFile);//д���ֽ����־
		m_nCurBpcLogSize =2;
	}
	return true;	
}
bool CBF_CommDataLog::OpenBpcErrLogFile()
{
	if (m_pBpcErrFile != NULL)
	{
		return false;
	}
	char fpathname[300];
	memset(fpathname,0,sizeof(fpathname));
	m_pDate.Update();
	if (m_nMaxLogSize < 1024)  //�����ƴ�С
	{
#ifdef _WINDOWS	
		sprintf(m_sBpcErrLogFilePathName,"%s\\%s%s",m_sLogPath.c_str(),m_pDate.ToStringD8().c_str(),m_sBpcErrLogName.c_str());
#else	
		sprintf(m_sBpcErrLogFilePathName,"%s/%s%s",m_sLogPath.c_str(),m_pDate.ToStringD8().c_str(),m_sBpcErrLogName.c_str());
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
			m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"����Ŀ¼%sʧ��",fpathname);
			return false;
		}
#ifdef _WINDOWS	
		sprintf(m_sBpcErrLogFilePathName,"%s\\%s",fpathname,m_sBpcErrLogName.c_str());
#else	
		sprintf(m_sBpcErrLogFilePathName,"%s/%s",fpathname,m_sBpcErrLogName.c_str());
#endif
	}
	struct_stat fs;
	fs.st_size = 0;
	FSTAT(m_sBpcErrLogFilePathName,&fs);
	if (fs.st_size>0)
	{
		m_nCurBpcErrLogSize = fs.st_size;
	}
	else
	{
		m_nCurBpcErrLogSize = 0;
	}

	m_pBpcErrFile = fopen(m_sBpcErrLogFilePathName,"ab");
	if (m_pBpcErrFile == NULL)
	{
		m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"����־�ļ�%sʧ��",m_sBpcErrLogFilePathName);
		return false;
	}
	
	if (m_nCurBpcErrLogSize<1)
	{
		if (m_bIsBigEndian)
		{
			sprintf(fpathname,"B1");//������
		}
		else
		{
			sprintf(fpathname,"L1");//������
		}
		fwrite(fpathname,1,2,m_pBpcErrFile);//д���ֽ����־
		m_nCurBpcErrLogSize= 2;
	}
	return true;
}

bool CBF_CommDataLog::WriteLog(char *logdata)
{
	switch (logdata[0])
	{
		case 0:
			return WriteDrebLog(logdata+1);
		case 1:
			return WriteDrebErrLog(logdata+1);
		case 2:
			return WriteBpcLog(logdata+1);
		case 3:
			return WriteBpcErrLog(logdata+1);
		default:
			return false;
	}
	return true;
}

bool CBF_CommDataLog::WriteDrebLog(char *logdata)
{
	S_LOG_DREBHEAD *drebdata = (S_LOG_DREBHEAD *)logdata;
	m_pDate.Update();
	drebdata->nTime = atoi(m_pDate.ToStringT6().c_str());
	drebdata->nUs = m_pDate.GetUs();
	drebdata->nId = GetPid();
	if (NULL == m_pDrebFile)
	{
		if (!OpenDrebLogFile())
		{
			return false;
		}
	}
	int nRet = fwrite(logdata,1,drebdata->data.head.nLen+DREBHEADLEN+12,m_pDrebFile);

	m_nCurDrebLogSize +=nRet;
	if (nRet != drebdata->data.head.nLen+DREBHEADLEN+12)
	{
		m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"д����־ʧ��%sʧ��",m_sDrebLogFilePathName);
		return false;
	}
	fflush(m_pDrebFile);
	if (m_nMaxLogSize >0 && m_nCurDrebLogSize > m_nMaxLogSize)
	{
		//�л���־
		m_nCurDrebLogSize = 0;
		//���ļ�
		fclose(m_pDrebFile);
		m_pDrebFile = NULL;
		char  rlogfilepathname[500];
		sprintf(rlogfilepathname,"%s.%s",m_sDrebLogFilePathName,m_pDate.ToStringT6().c_str());
		rename(m_sDrebLogFilePathName,rlogfilepathname);
		if (!OpenDrebLogFile())
		{
			return false;
		}
	}
	return true;
}

bool CBF_CommDataLog::WriteDrebErrLog(char *logdata)
{
	S_LOG_DREBHEAD *drebdata = (S_LOG_DREBHEAD *)logdata;
	m_pDate.Update();
	drebdata->nTime = atoi(m_pDate.ToStringT6().c_str());
	drebdata->nUs = m_pDate.GetUs();
	drebdata->nId = GetPid();
	if (NULL == m_pDrebErrFile)
	{
		if (!OpenDrebErrLogFile())
		{
			return false;
		}
	}
	int nRet = fwrite(logdata,1,drebdata->data.head.nLen+DREBHEADLEN+12,m_pDrebErrFile);
	m_nCurDrebErrLogSize +=nRet;
	if (nRet != drebdata->data.head.nLen+DREBHEADLEN+12)
	{
		m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"д����־ʧ��%sʧ��",m_sDrebErrLogFilePathName);
		return false;
	}
	fflush(m_pDrebErrFile);
	if (m_nMaxLogSize >0 && m_nCurDrebErrLogSize > m_nMaxLogSize)
	{
		//�л���־
		m_nCurDrebErrLogSize = 0;
		//���ļ�
		fclose(m_pDrebErrFile);
		m_pDrebErrFile = NULL;
		char  rlogfilepathname[500];
		sprintf(rlogfilepathname,"%s.%s",m_sDrebErrLogFilePathName,m_pDate.ToStringT6().c_str());
		rename(m_sDrebErrLogFilePathName,rlogfilepathname);
		if (!OpenDrebErrLogFile())
		{
			return false;
		}
	}
	return true;
}

bool CBF_CommDataLog::WriteBpcLog(char *logdata)
{
	S_LOG_BPCHEAD *bpcdata = (S_LOG_BPCHEAD *)logdata;
	m_pDate.Update();
	bpcdata->nTime = atoi(m_pDate.ToStringT6().c_str());
	bpcdata->nUs = m_pDate.GetUs();
	bpcdata->nId = GetPid();
	if (NULL == m_pBpcFile)
	{
		if (!OpenBpcLogFile())
		{
			return false;
		}
	}
	int nRet = fwrite(logdata,1,bpcdata->data.sDBHead.nLen+DREBHEADLEN+BPCHEADLEN+12,m_pBpcFile);
	m_nCurBpcLogSize+=nRet;
	if (nRet != bpcdata->data.sDBHead.nLen+DREBHEADLEN+BPCHEADLEN+12)
	{
		m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"д����־ʧ��%sʧ��",m_sBpcLogFilePathName);
		return false;
	}
	fflush(m_pBpcFile);
	if (m_nMaxLogSize >0 && m_nCurBpcLogSize > m_nMaxLogSize)
	{
		//�л���־
		m_nCurBpcLogSize = 0;
		//���ļ�
		fclose(m_pBpcFile);
		m_pBpcFile = NULL;
		char  rlogfilepathname[500];
		sprintf(rlogfilepathname,"%s.%s",m_sBpcLogFilePathName,m_pDate.ToStringT6().c_str());
		rename(m_sBpcLogFilePathName,rlogfilepathname);
		if (!OpenBpcLogFile())
		{
			return false;
		}
	}
	return true;
}

bool CBF_CommDataLog::WriteBpcErrLog(char *logdata)
{
	S_LOG_BPCHEAD *bpcdata = (S_LOG_BPCHEAD *)logdata;
	m_pDate.Update();
	bpcdata->nTime = atoi(m_pDate.ToStringT6().c_str());
	bpcdata->nUs = m_pDate.GetUs();
	bpcdata->nId = GetPid();
	if (NULL == m_pBpcErrFile)
	{
		if (!OpenBpcErrLogFile())
		{
			return false;
		}
	}
	int nRet = fwrite(logdata,1,bpcdata->data.sDBHead.nLen+DREBHEADLEN+BPCHEADLEN+12,m_pBpcErrFile);
	m_nCurBpcErrLogSize+=nRet;
	if (nRet != bpcdata->data.sDBHead.nLen+DREBHEADLEN+BPCHEADLEN+12)
	{
		m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"д����־ʧ��%sʧ��",m_sBpcLogFilePathName);
		return false;
	}
	fflush(m_pBpcErrFile);
	if (m_nMaxLogSize >0 && m_nCurBpcErrLogSize > m_nMaxLogSize)
	{
		//�л���־
		m_nCurBpcErrLogSize = 0;
		//���ļ�
		fclose(m_pBpcErrFile);
		m_pBpcErrFile = NULL;
		char  rlogfilepathname[500];
		sprintf(rlogfilepathname,"%s.%s",m_sBpcErrLogFilePathName,m_pDate.ToStringT6().c_str());
		rename(m_sBpcErrLogFilePathName,rlogfilepathname);
		if (!OpenBpcErrLogFile())
		{
			return false;
		}
	}
	return true;
}

void CBF_CommDataLog::ClearQueueAndClose()
{
	char *data= NULL;
	while (m_pQueue.GetDataCount()>0)
	{
		if (!m_pQueue.GetData(data))
		{
			return;
		}
		m_pDate.Update();
		//��������Ƿ�仯 m_bIsCheckDate�ɶ�ʱ�����ı䣬������ʱ���������������
		if (m_sCurDate.compare(m_pDate.ToStringD8()) != 0) //���ڱ仯��
		{
			m_sCurDate = m_pDate.ToStringD8();
			m_bIsCheckDate = false;//��ʱ����ʼ
			if (m_pDrebFile != NULL)
			{
				fclose(m_pDrebFile);
				m_pDrebFile = NULL;
			}
			if (m_pDrebErrFile != NULL)
			{
				fclose(m_pDrebErrFile);
				m_pDrebErrFile = NULL;
			}
			if (m_pBpcFile != NULL)
			{
				fclose(m_pBpcFile);
				m_pBpcFile = NULL;
			}
			if (m_pBpcErrFile != NULL)
			{
				fclose(m_pBpcErrFile);
				m_pBpcErrFile = NULL;
			}
			OpenDrebLogFile();
			OpenDrebErrLogFile();
			OpenBpcLogFile();
			OpenBpcErrLogFile();
			
		}
		if (m_pQueue.GetData(data))
		{
			m_pDate.Update();
			//д������
			if (!WriteLog(data))
			{
				//д�ļ�����
				m_pLog.LogMp(LOG_ERROR,__FILE__,__LINE__,"д�ļ�����");
			}
			m_pMemPool.PoolFree(data);
			data = NULL;
		}
	}
	if (m_pDrebFile != NULL)
	{
		fclose(m_pDrebFile);
		m_pDrebFile = NULL;
	}
	if (m_pDrebErrFile != NULL)
	{
		fclose(m_pDrebErrFile);
		m_pDrebErrFile = NULL;
	}
	if (m_pBpcFile != NULL)
	{
		fclose(m_pBpcFile);
		m_pBpcFile = NULL;
	}
	if (m_pBpcErrFile != NULL)
	{
		fclose(m_pBpcErrFile);
		m_pBpcErrFile = NULL;
	}
}

void CBF_CommDataLog::SetLogLevel(int loglevel)
{
	m_nLogLevel = loglevel;
}

int CBF_CommDataLog::GetLogLevel()
{
	return m_nLogLevel;
}
