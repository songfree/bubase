// BpcLog.cpp: implementation of the CBpcLog class.
//
//////////////////////////////////////////////////////////////////////

#include "DrebBpcLog.h"
#include "BF_Tools.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CDrebBpcLog::CDrebBpcLog()
{
	m_nBpcHeadLen = sizeof(S_BPC_HEAD);
}

CDrebBpcLog::~CDrebBpcLog()
{

}

void CDrebBpcLog::LogBpc(int level, PBPCCOMMSTRU data,bool flag)
{
	if (level > m_nLogLevel)
	{
		return ;
	}
	m_pDate.Update();
	char filename[400];
	if (!flag) //正常日志
	{
#ifdef _WINDOWS
		sprintf(filename,"%s\\%s%s",m_sLogPath.c_str(),m_pDate.ToStringD8().c_str(),m_sBpcLogName.c_str());
#else
		sprintf(filename,"%s/%s%s",m_sLogPath.c_str(),m_pDate.ToStringD8().c_str(),m_sBpcLogName.c_str());
#endif
	}
	else  //出错日志
	{
#ifdef _WINDOWS
		sprintf(filename,"%s\\%s%s",m_sLogPath.c_str(),m_pDate.ToStringD8().c_str(),m_sBpcLogErrName.c_str());
#else
		sprintf(filename,"%s/%s%s",m_sLogPath.c_str(),m_pDate.ToStringD8().c_str(),m_sBpcLogErrName.c_str());
#endif

	}
	FILE *fp = fopen(filename,"ab");
	if (fp == NULL)
	{
		return ;
	}
	char tmpchar[20];
	struct_stat st;
	FSTAT(filename,&st);
	int writelen=0;
	if (st.st_size <1)
	{
		if (htonl(1) == 1)
		{
			sprintf(tmpchar,"B1");//网络序
		}
		else
		{
			sprintf(tmpchar,"L1");//
		}
		fwrite(tmpchar,1,2,fp);//写入字节序标志
	}
	S_LOG_BPCHEAD logdata;
	logdata.nTime = atoi(m_pDate.ToStringT6().c_str());
	logdata.nUs = m_pDate.GetUs();
	logdata.nId = GetPid();
	writelen +=12;
	writelen = writelen +DREBHEADLEN+m_nBpcHeadLen+data->sDBHead.nLen;
	memcpy(&(logdata.data),data,DREBHEADLEN+m_nBpcHeadLen+data->sDBHead.nLen);
	fwrite(&logdata,1,writelen,fp);
	fclose(fp);
	return ;

}

void CDrebBpcLog::SetLogPara(int level, const char *logpath,const char *dreblogname,const char *dreberrlogname,const char *bpclogname,const char *bpcerrlogname)
{
	m_nLogLevel = level;
#ifdef _WINDOWS
	m_sLogPath = std::string(logpath)+"\\data";
#else
	m_sLogPath = std::string(logpath)+"/data";
#endif
	CBF_Tools::MakeDir(m_sLogPath.c_str());
	m_sDrebLogName = dreblogname;
	m_sDrebLogErrName = dreberrlogname;
	m_sBpcLogName = bpclogname;
	m_sBpcLogErrName = bpcerrlogname;
}

void CDrebBpcLog::LogDreb(int level, PBPCCOMMSTRU data,bool flag)
{
	if (level > m_nLogLevel)
	{
		return ;
	}
	m_pDate.Update();
	char filename[400];
	if (!flag) //正常日志
	{
#ifdef _WINDOWS
		sprintf(filename,"%s\\%s%s",m_sLogPath.c_str(),m_pDate.ToStringD8().c_str(),m_sDrebLogName.c_str());
#else
		sprintf(filename,"%s/%s%s",m_sLogPath.c_str(),m_pDate.ToStringD8().c_str(),m_sDrebLogName.c_str());
#endif
	}
	else  //出错日志
	{
#ifdef _WINDOWS
		sprintf(filename,"%s\\%s%s",m_sLogPath.c_str(),m_pDate.ToStringD8().c_str(),m_sDrebLogErrName.c_str());
#else
		sprintf(filename,"%s/%s%s",m_sLogPath.c_str(),m_pDate.ToStringD8().c_str(),m_sDrebLogErrName.c_str());
#endif
	}
	
	FILE *fp = fopen(filename,"ab");
	if (fp == NULL)
	{
		return ;
	}
	char tmpchar[20];
	struct_stat st;
	FSTAT(filename,&st);
	int writelen=0;
	if (st.st_size <1)
	{
		if (htonl(1) == 1)
		{
			sprintf(tmpchar,"B0");//网络序
		}
		else
		{
			sprintf(tmpchar,"L0");//
		}
		fwrite(tmpchar,1,2,fp);//写入字节序标志
	}
	S_LOG_DREBHEAD logdata;
	logdata.nTime = atoi(m_pDate.ToStringT6().c_str());
	logdata.nUs = m_pDate.GetUs();
	logdata.nId = GetPid();
	writelen +=12;
	writelen = writelen+DREBHEADLEN+data->sDBHead.nLen;
	memcpy(&(logdata.data.head),&(data->sDBHead),DREBHEADLEN);
	if (data->sDBHead.nLen>0)
	{
		memcpy(logdata.data.buffer,data->sBuffer,data->sDBHead.nLen);
	}
	fwrite(&logdata,1,writelen,fp);
	fclose(fp);
	return ;

}
void CDrebBpcLog::LogDreb(int level, PCOMMSTRU data,bool flag)
{
	if (level > m_nLogLevel)
	{
		return ;
	}
	m_pDate.Update();
	char filename[400];
	if (!flag) //正常日志
	{
#ifdef _WINDOWS
		sprintf(filename,"%s\\%s%s",m_sLogPath.c_str(),m_pDate.ToStringD8().c_str(),m_sDrebLogName.c_str());
#else
		sprintf(filename,"%s/%s%s",m_sLogPath.c_str(),m_pDate.ToStringD8().c_str(),m_sDrebLogName.c_str());
#endif
	}
	else  //出错日志
	{
#ifdef _WINDOWS
		sprintf(filename,"%s\\%s%s",m_sLogPath.c_str(),m_pDate.ToStringD8().c_str(),m_sDrebLogErrName.c_str());
#else
		sprintf(filename,"%s/%s%s",m_sLogPath.c_str(),m_pDate.ToStringD8().c_str(),m_sDrebLogErrName.c_str());
#endif
	}
	
	FILE *fp = fopen(filename,"ab");
	if (fp == NULL)
	{
		return ;
	}
	char tmpchar[20];
	struct_stat st;
	FSTAT(filename,&st);
	int writelen=0;
	if (st.st_size <1)
	{
		if (htonl(1) == 1)
		{
			sprintf(tmpchar,"B0");//网络序
		}
		else
		{
			sprintf(tmpchar,"L0");//
		}
		fwrite(tmpchar,1,2,fp);//写入字节序标志
	}
	S_LOG_DREBHEAD logdata;
	logdata.nTime = atoi(m_pDate.ToStringT6().c_str());
	logdata.nUs = m_pDate.GetUs();
	logdata.nId = GetPid();
	writelen +=12;
	writelen = writelen+DREBHEADLEN+data->head.nLen;
	memcpy(&(logdata.data.head),&(data->head),DREBHEADLEN);
	if (data->head.nLen>0)
	{
		memcpy(logdata.data.buffer,data->buffer,data->head.nLen);
	}
	
	fwrite(&logdata,1,writelen,fp);
	fclose(fp);
	return ;
	
}
