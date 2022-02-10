// BF_SimpleLog.cpp: implementation of the CBF_SimpleLog class.
//
//////////////////////////////////////////////////////////////////////

#include "BF_SimpleLog.h"
#include "BF_Tools.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBF_SimpleLog::CBF_SimpleLog()
{
	m_nLogLevel = LOG_DEBUG;
	m_nId = -1;
	char tmpchar[300];
	memset(tmpchar,0,sizeof(tmpchar));
	WORKPATH(tmpchar);
	strcat(tmpchar,"/log");
	CBF_Tools::MakeDir(tmpchar);
	m_sLogPath = tmpchar;
	m_sLogName = "simplelog.log";
	
}

CBF_SimpleLog::~CBF_SimpleLog()
{

}
void CBF_SimpleLog::SetLogPara(int level,const char *logfilepath,const char *logfilename, int id)
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
		CBF_Tools::MakeDir(tmpchar);
		m_sLogPath = tmpchar;
#else
#ifdef LOGCURPATH
		WORKPATH(tmpchar);
		strcat(tmpchar,"/log");
		CBF_Tools::MakeDir(tmpchar);
		m_sLogPath = tmpchar;
#else	
		sprintf(tmpchar,"%s/log",(char *)getenv("HOME"));
		CBF_Tools::MakeDir(tmpchar);
		m_sLogPath = tmpchar;
#endif
#endif
	}
	else
	{
		CBF_Tools::MakeDir(m_sLogPath.c_str());
	}
}
void CBF_SimpleLog::GetName(char *filename)
{
	int filelen;
	int i;
	int offset = -1;
	filelen = strlen(filename);
	if (filelen < 1)
	{
		return ;
	}
	for (i=filelen-1 ;i>=0 ; i--)
	{
		if (filename[i] == '\\' || filename[i] == '/' )
		{
			offset = i;
			break;
		}
	}
	if (offset > -1)
	{
		filelen = filelen - offset;
		for (i=0; i< filelen; i++)
		{
			filename[i]=filename[i+offset+1];
		}
		filename[filelen-1]=0;
	}
	return ;
}


int CBF_SimpleLog::LogMp(int level, const char *filename, int line,const char *fmt,...)
{
	if (level>m_nLogLevel)
	{
		return 0;
	}
	FILE *fp;
	char fname[300];
	char fpathname[300];
	char date[30];
	memset(date,0,sizeof(date));
	memset(fname,0,sizeof(fname));
	memset(fpathname,0,sizeof(fpathname));
	strcpy(fname,filename);
	//去掉路径
	GetName(fname);
	char buffer[60];
	memset(buffer,0,sizeof(buffer));
	//0严重错误1重要错误2一般错误3警告4提示警告5调试信息
#ifdef _ENGLISH_
	switch (level)
	{
	case -1:
		sprintf(buffer,"info ");
		break;
	case 0:
		sprintf(buffer,"FAT ");
		break;
	case 1:
		sprintf(buffer,"ERR ");
		break;	
	case 2:
		sprintf(buffer,"GEN ");
		break;	
	case 3:
		sprintf(buffer,"WRN ");
		break;	
	case 4:
		sprintf(buffer,"TIP ");
		break;	
	case 5:
		sprintf(buffer,"DBG ");
		break;	
	default:
		sprintf(buffer,"OH:%d ",level);
		break;
	}
	m_pDate.Update();
	//	strcpy(date,m_pDate.ToStringDT17().c_str());
	sprintf(date,"%s %ld", m_pDate.ToStringDT17().c_str(),m_pDate.GetUs());
#ifdef _WINDOWS	
	sprintf(fpathname,"%s\\%s%s",m_sLogPath.c_str(),m_pDate.ToStringD8().c_str(),m_sLogName.c_str());
#else	
    sprintf(fpathname,"%s/%s%s",m_sLogPath.c_str(),m_pDate.ToStringD8().c_str(),m_sLogName.c_str());
#endif
	fp=fopen(fpathname,"a");
	if(fp==NULL)
	{
		return -1;
	}
	if (m_nId >=0)
	{
		fprintf(fp,"%s ID[%d] file[%s]line[%d] %s ",date,m_nId,fname,line,buffer);
	}
	else
	{
		fprintf(fp,"%s ID[%d] file[%s]line[%d] %s ",date,GetPid(),fname,line,buffer);
	}

#else
	switch (level)
	{
	case -1:
		sprintf(buffer,"info ");
		break;
	case 0:
		sprintf(buffer,"严重错 ");
		break;
	case 1:
		sprintf(buffer,"重大错 ");
		break;	
	case 2:
		sprintf(buffer,"错误 ");
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
		sprintf(buffer,"其它:%d ",level);
		break;
	}
	m_pDate.Update();
//	strcpy(date,m_pDate.ToStringDT17().c_str());
	sprintf(date,"%s %ld", m_pDate.ToStringDT17().c_str(),m_pDate.GetUs());
#ifdef _WINDOWS	
	sprintf(fpathname,"%s\\%s%s",m_sLogPath.c_str(),m_pDate.ToStringD8().c_str(),m_sLogName.c_str());
#else	
    sprintf(fpathname,"%s/%s%s",m_sLogPath.c_str(),m_pDate.ToStringD8().c_str(),m_sLogName.c_str());
#endif
	fp=fopen(fpathname,"a");
	if(fp==NULL)
	{
		return -1;
	}
	if (m_nId >=0)
	{
		fprintf(fp,"%s ID[%d] 文件[%s]行[%d] %s ",date,m_nId,fname,line,buffer);
	}
	else
	{
		fprintf(fp,"%s ID[%d] 文件[%s]行[%d] %s ",date,GetPid(),fname,line,buffer);
	}

#endif
	
	va_list ap;
	va_start(ap, fmt);
    vfprintf(fp, fmt, ap);
    va_end(ap);
    fprintf(fp, "\n");
    fflush(fp);
    fclose(fp);
	return 0;
}

int CBF_SimpleLog::LogBin(int level, const char *filename, int line,const char *title, char *msg,int msglen)
{
	if (level>m_nLogLevel)
	{
		return 0;
	}
	FILE *fp;
	char fname[300];
	char fpathname[300];
	char date[30];
	memset(date,0,sizeof(date));
	memset(fname,0,sizeof(fname));
	memset(fpathname,0,sizeof(fpathname));
	strcpy(fname,filename);
	//去掉路径
	GetName(fname);
	char buffer[60];
	memset(buffer,0,sizeof(buffer));
	//0严重错误1重要错误2一般错误3警告4提示警告5调试信息
#ifdef _ENGLISH_
	switch (level)
	{
	case -1:
		sprintf(buffer,"info ");
		break;
	case 0:
		sprintf(buffer,"FAT ");
		break;
	case 1:
		sprintf(buffer,"ERR ");
		break;	
	case 2:
		sprintf(buffer,"GEN ");
		break;	
	case 3:
		sprintf(buffer,"WRN ");
		break;	
	case 4:
		sprintf(buffer,"TIP ");
		break;	
	case 5:
		sprintf(buffer,"DBG ");
		break;	
	default:
		sprintf(buffer,"OH:%d ",level);
		break;
	}
	m_pDate.Update();
	//strcpy(date,m_pDate.ToStringDT17().c_str());
	sprintf(date,"%s %ld", m_pDate.ToStringDT17().c_str(),m_pDate.GetUs());
#ifdef _WINDOWS	
	sprintf(fpathname,"%s\\%s%s",m_sLogPath.c_str(),m_pDate.ToStringD8().c_str(),m_sLogName.c_str());
#else	
    sprintf(fpathname,"%s/%s%s",m_sLogPath.c_str(),m_pDate.ToStringD8().c_str(),m_sLogName.c_str());
#endif
	fp=fopen(fpathname,"a");
    if(fp == NULL)
	{
        return -1;
	}
	if (m_nId >=0)
	{
		fprintf(fp,"%s ID[%d] file[%s]line[%d] title[%s] datalen[%d]\n",date,m_nId,\
			fname,line,title,msglen);
	}
	else
	{
		fprintf(fp,"%s ID[%d] file[%s]line[%d] title[%s] datalen[%d]\n",date,GetPid(),\
			fname,line,title,msglen);
	}
	
#else
	switch (level)
	{
	case -1:
		sprintf(buffer,"info ");
		break;
	case 0:
		sprintf(buffer,"严重错 ");
		break;
	case 1:
		sprintf(buffer,"重大错 ");
		break;	
	case 2:
		sprintf(buffer,"错误 ");
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
		sprintf(buffer,"其它:%d ",level);
		break;
	}
	m_pDate.Update();
	//strcpy(date,m_pDate.ToStringDT17().c_str());
	sprintf(date,"%s %ld", m_pDate.ToStringDT17().c_str(),m_pDate.GetUs());
#ifdef _WINDOWS	
	sprintf(fpathname,"%s\\%s%s",m_sLogPath.c_str(),m_pDate.ToStringD8().c_str(),m_sLogName.c_str());
#else	
    sprintf(fpathname,"%s/%s%s",m_sLogPath.c_str(),m_pDate.ToStringD8().c_str(),m_sLogName.c_str());
#endif
	fp=fopen(fpathname,"a");
    if(fp == NULL)
	{
        return -1;
	}
	if (m_nId >=0)
	{
		fprintf(fp,"%s ID[%d] 文件[%s]行[%d] 标题[%s] 数据长度[%d]\n",date,m_nId,\
			fname,line,title,msglen);
	}
	else
	{
		fprintf(fp,"%s ID[%d] 文件[%s]行[%d] 标题[%s] 数据长度[%d]\n",date,GetPid(),\
			fname,line,title,msglen);
	}
#endif
	fprintf(fp,"[");
    for (int j=0 ;j<msglen ;j++)
	{
		
		if (msg[j] == 0)    
		{
			fputc('*',fp);
		}
		else
		{
			fputc(msg[j],fp);
		}
	}
    fprintf(fp,"]\n");
	fflush(fp);
    fclose(fp);
	return 0;
}

bool CBF_SimpleLog::isWrite(int level)
{
	if (level > m_nLogLevel)
	{
		return false;
	}
	return true;
}
void CBF_SimpleLog::GetLogPara(int &level,std::string &logfilepath, std::string &logfilename,int &id)
{
	level = m_nLogLevel;
	logfilepath = m_sLogPath;
	logfilename = m_sLogName;
	id = m_nId;
}
void CBF_SimpleLog::SetMaxLogSize(int size)
{
	
	return;
}
void  CBF_SimpleLog::SetLogLevel(int level)
{
	m_nLogLevel = level;
}
int  CBF_SimpleLog::GetLogLevel()
{
	return m_nLogLevel ;
}