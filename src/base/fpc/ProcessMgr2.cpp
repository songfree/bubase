// ProcessMgr.cpp: implementation of the CProcessMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "ProcessMgr.h"


//#include "ListenThread.h"

//extern CListenThread g_plisten;

#ifdef WIN32
#include "psapi.h"
#endif 
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////




typedef struct tagProcessInfo
{
	void* pvHProcess;          //进程句柄
	void* pvThread;            //线程句柄
	unsigned long ulProcessID; //进程ID
} PROCESS_INFO,*PPROCESS_INFO;

CProcessMgr::CProcessMgr()
{
	m_log.SetLogPara(5,"","monitor.log");
}

CProcessMgr::~CProcessMgr()
{

}
//创建进程
bool CProcessMgr::StartProcess(S_PROCESS_NODE *pPrgNode)
{
	CBF_PMutex pp(&(pPrgNode->mutex));
	char  curpath[300];
	memset(curpath,0,sizeof(curpath));

	//取得当前目录，并进到进程运行目录

#ifdef WIN32
	GetCurrentDirectory(sizeof(curpath)-1,curpath);
	if (!SetCurrentDirectory(pPrgNode->runpath.c_str()))
	{
		m_log.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"节点[%s]名称[%s]程序运行目录[%s]不存在",\
			pPrgNode->nodeid.c_str(),pPrgNode->processname.c_str(),pPrgNode->runpath.c_str());		
		return false;
	}
#else
	getcwd(curpath,sizeof(curpath)-1);
	if (access(pPrgNode->runpath.c_str(),0)!=0) 
	{
		m_log.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"节点[%s]名称[%s]程序运行目录[%s]不存在",\
			pPrgNode->nodeid.c_str(),pPrgNode->processname.c_str(),pPrgNode->runpath.c_str());		
		return false;
	}
	chdir(pPrgNode->runpath.c_str());
#endif

#ifdef WIN32
	unsigned long rtn;
	STARTUPINFO si;
	PROCESS_INFORMATION pidt;
	memset(&si,0,sizeof(si));
	si.cb = sizeof(si);
	char szCmdLine[1024];
	if (pPrgNode->prgpath.length()>0)
	{
		sprintf(szCmdLine,"%s\\%s %s",pPrgNode->prgpath.c_str(),pPrgNode->prgname.c_str(),pPrgNode->prgpara.c_str());
	}
	else
	{
		sprintf(szCmdLine,"%s %s",pPrgNode->prgname.c_str(),pPrgNode->prgpara.c_str());
	}
	if (!CreateProcess(
		NULL,   // No module name (use command line).
		szCmdLine,    //szBUProg,
		NULL,   // If lpProcessAttributes is NULL, the handle cannot be inherited
		NULL,   // If lpThreadAttributes is NULL, the handle cannot be inherited
		FALSE,  // If the parameter is FALSE, the handles are not inherited.
		CREATE_NEW_CONSOLE, // This flag is valid only when starting a console application. If set, the console application is run without a console window. 
		NULL,   //  Use parent's environment block. 
		pPrgNode->runpath.c_str(),  // ./bunit
		&si,
		&pidt
		))
	{
		m_log.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"启动节点[%s:%s]失败 <%s:%s>",\
			pPrgNode->nodeid.c_str(),pPrgNode->processname.c_str(),szCmdLine,pPrgNode->prgpath.c_str());
		//将当前目录设置回去
		SetCurrentDirectory(curpath);

		return false;
	}
	else
	{	
		pPrgNode->pid = pidt.dwProcessId;
		pPrgNode->status = "正常";
		CloseHandle(pidt.hThread);
		CloseHandle(pidt.hProcess);
		m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"启动节点[%s:%s]成功 pid=[%d]",\
			pPrgNode->nodeid.c_str(),pPrgNode->processname.c_str(),pidt.dwProcessId);
	}
	//将当前目录设置回去
	SetCurrentDirectory(curpath);
#else
	pid_t pid;
	char szCmdLine[1024];
	memset(szCmdLine,0,sizeof(szCmdLine));
	if (pPrgNode->prgpath.length()>0)
	{
		sprintf(szCmdLine,"%s/%s",pPrgNode->prgpath.c_str(),pPrgNode->prgname.c_str());
	}
	else
	{
		sprintf(szCmdLine,"%s",pPrgNode->prgname.c_str());
	}
	//准备参数
	char *argvs[20];
	char argvList[20][400];
	char spara[1024];
	memset(spara,0,sizeof(spara));
	memset(argvList,0,sizeof(argvList));
	
	int arg=1;
	strcpy(argvList[0],szCmdLine);
	argvs[0]=argvList[0];
	int offset=0;
	bool ispara=false;
	if (pPrgNode->prgpara.length()>0)
	{
		strcpy(spara,pPrgNode->prgpara.c_str());
		for (int i=0;i<strlen(spara);i++)
		{
			if (spara[i] == ' ')
			{
				if (!ispara)
				{
					strncpy(argvList[arg],spara+offset,i-offset);
					argvs[arg]=argvList[arg];
					arg++;
					ispara = true;
				}
				offset = i+1;
				continue;
			}
			if (i == strlen(spara)-1)
			{
				if (i-offset>=0)
				{
					strncpy(argvList[arg],spara+offset,i-offset+1);
					argvs[arg]=argvList[arg];
					arg++;
				}
			}
			ispara = false;
		}
	}
	argvs[arg]=NULL;
 	for (int j=0;j<arg;j++)
 	{
// 		printf(" argv[%d]=%s ",j,argvs[j]);
        m_log.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"argv[%d]=%s",j,argvs[j]);
 	}
// 	printf("\n");


	m_log.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"curpath=%s runpath=%s szCmdLine=%s",curpath,pPrgNode->runpath.c_str(),szCmdLine);
	pid=vfork();
	if (pid==0)
	{
		execv(szCmdLine,argvs);
//		g_plisten.CloseListen(false);
		//如果execv函数返回，表明没有正常执行命令，打印错误信息
		m_log.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"curpath=%s runpath=%s szCmdLine=%s 启动失败 %d",curpath,pPrgNode->runpath.c_str(),szCmdLine,errno);
		exit(-1);
	}
	else if (pid>0)
	{
		pPrgNode->pid = pid;
		pPrgNode->status = "正常";
	}
	m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"启动节点[%s:%s]完成 pid=[%d]",\
			pPrgNode->nodeid.c_str(),pPrgNode->processname.c_str(),pPrgNode->pid);

#endif
	//将当前目录设置回去
#ifdef WIN32
	SetCurrentDirectory(curpath);
#else
	chdir(curpath);
#endif
	return true;
}

int CProcessMgr::StopProcess(S_PROCESS_NODE *pPrgNode)
{
	CBF_PMutex pp(&pPrgNode->mutex);
	//取得当前目录，并进到进程运行目录
	char  curpath[300];
	memset(curpath,0,sizeof(curpath));	
#ifdef WIN32
	GetCurrentDirectory(sizeof(curpath)-1,curpath);
	if (!SetCurrentDirectory(pPrgNode->runpath.c_str()))
	{
		m_log.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"节点[%s]名称[%s]程序运行目录[%s]不存在",\
			pPrgNode->nodeid.c_str(),pPrgNode->processname.c_str(),pPrgNode->runpath.c_str());		
		return false;
	}
#else
	getcwd(curpath,sizeof(curpath)-1);
	if (access(pPrgNode->runpath.c_str(),0)!=0) 
	{
		m_log.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"节点[%s]名称[%s]程序运行目录[%s]不存在",\
			pPrgNode->nodeid.c_str(),pPrgNode->processname.c_str(),pPrgNode->runpath.c_str());		
		return false;
	}
	chdir(pPrgNode->runpath.c_str());
#endif

	//执行退出脚本
	if (pPrgNode->stopscript.length()>0)
	{
		system(pPrgNode->stopscript.c_str());
	}
	
#ifdef WIN32
	HANDLE h = OpenProcess(PROCESS_ALL_ACCESS, TRUE, pPrgNode->pid);
	if (NULL != h)
	{
		TerminateProcess(h, 0);
		
		pPrgNode->status = "已停止";
		m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"停止节点[%s:%s]成功 pid=[%d]",\
			pPrgNode->nodeid.c_str(),pPrgNode->processname.c_str(),pPrgNode->pid);
		pPrgNode->pid = -1;
	}
	CloseHandle(h);
	m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"节点[%s:%s]已停止 pid=[%d]",\
			pPrgNode->nodeid.c_str(),pPrgNode->processname.c_str(),pPrgNode->pid);

	//将当前目录设置回去

	SetCurrentDirectory(curpath);

	return 0;

#else
	FILE *fp;
	char execcmd[301];
	char tmpchar[200];
	char stemp1[100];
	memset(tmpchar,0,sizeof(tmpchar));
	int ret;
	ret = kill(pPrgNode->pid,0);
	if (ret == 0)
	{
		ret = kill(pPrgNode->pid,SIGKILL);
		if (ret == 0)
		{
			pPrgNode->status = "已停止";
			m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"kill SIGKILL停止节点[%s:%s]成功 pid=[%d]",\
				pPrgNode->nodeid.c_str(),pPrgNode->processname.c_str(),pPrgNode->pid);
 			pPrgNode->pid = -1;
		}
		else
		{
			sprintf(execcmd,"kill -9 %d",pPrgNode->pid);
			system(execcmd);
			pPrgNode->status = "已停止";
			m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"kill -9 停止节点[%s:%s]成功 pid=[%d]",\
				pPrgNode->nodeid.c_str(),pPrgNode->processname.c_str(),pPrgNode->pid);
 			pPrgNode->pid = -1;
		}
	}
	else
	{
		m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"节点[%s:%s]已停止 pid=[%d]",\
 			pPrgNode->nodeid.c_str(),pPrgNode->processname.c_str(),pPrgNode->pid);
	}
// #if defined(AIX) 
// 	sprintf(execcmd,"ps -ef |grep %d ",pPrgNode->pid);
// #endif
// #if defined(LINUX) 
// 	sprintf(execcmd,"ps -ef | grep %d ",pPrgNode->pid);
// #endif
// #if defined(HP_UX) 
// 	sprintf(execcmd,"ps -ef |grep %d",pPrgNode->pid);
// #endif
// 	fp = popen(execcmd,"r");
// 	if (fp == NULL)
// 	{
// 		m_log.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"执行[%s]出错",execcmd);
// 		chdir(curpath);
// 		return -1;
// 	}
// 	for(; !feof(fp);)
// 	{
// 		memset(execcmd,0,sizeof(execcmd));
// 		fgets(execcmd,300,fp);
// 		if(feof(fp))
// 		{
// 			break;
// 		}
// 		m_publog.ltrim(execcmd);
// 		memset(stemp1,0,sizeof(stemp1));
// 		memset(tmpchar,0,sizeof(tmpchar));
// 		sscanf(execcmd,"%s%s",stemp1,tmpchar);
// 
// 		if (atoi(tmpchar)== pPrgNode->pid)
// 		{
// 			kill(pPrgNode->pid,SIGKILL);
// 			pPrgNode->status = "已停止";
// 			m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"停止节点[%s:%s]成功 pid=[%d]",\
// 				pPrgNode->nodeid.c_str(),pPrgNode->processname.c_str(),pPrgNode->pid);
// 			pPrgNode->pid = -1;
// 		}
// 		
// 	}
// 	pclose(fp);
// 	m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"节点[%s:%s]已停止 pid=[%d]",\
// 			pPrgNode->nodeid.c_str(),pPrgNode->processname.c_str(),pPrgNode->pid);

	chdir(curpath);
	return 0;

#endif
	return 0;
}


int CProcessMgr::FindProcess(S_PROCESS_NODE *pPrgNode)
{
	
#ifdef WIN32
	HANDLE h = OpenProcess(PROCESS_ALL_ACCESS, TRUE,pPrgNode->pid);
	if (NULL != h)
	{
		char szFilePathName[1024] = {0};
		//获得进程的完整路径（包含可执行文件名）
		if (GetModuleFileNameEx(h, NULL, szFilePathName, 
			sizeof(szFilePathName)) <= 0)
		{				
			CloseHandle(h);
			m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"节点[%s:%s]进程不存在 pid=[%d]",\
				pPrgNode->nodeid.c_str(),pPrgNode->processname.c_str(),pPrgNode->pid);
			return -1;
		}
		
		char szFilePath[512] = {0};
		char szFileName[512] = {0};
		SeparateFilePathAndFileName(szFilePathName, szFilePath, 
			sizeof(szFilePath), szFileName, sizeof(szFileName));
		if (0 == strcmp(szFileName,pPrgNode->prgname.c_str()))
		{
			CloseHandle(h);
			return 0;
		}
		m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"节点[%s:%s]进程不存在 pid=[%d]",\
			pPrgNode->nodeid.c_str(),pPrgNode->processname.c_str(),pPrgNode->pid);
		CloseHandle(h);
		return -1;	
	}
	m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"节点[%s:%s]进程不存在 pid=[%d]",\
			pPrgNode->nodeid.c_str(),pPrgNode->processname.c_str(),pPrgNode->pid);
	return -1;
#else
	FILE *fp;
	char execcmd[300];
	char tmpchar[200];
	char stemp1[100];
	memset(tmpchar,0,sizeof(tmpchar));
	int ret;
	if (pPrgNode->pid <=0 )
	{
		return -1;
	}
	
	ret = kill(pPrgNode->pid,0);
	if (ret == 0)
	{
		
		//检查程序名，程序目录，运行目录
		if (pPrgNode->prgname.length() > 0 || pPrgNode->prgpath.length()>0 || pPrgNode->runpath.length()>0)
		{
			if (CheckPrgRunPara(pPrgNode) == 0)
			{
				return 0;
			}
			m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"检查程序信息不符合");
			return -1;
		}
		return 0;
	}
	m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"节点[%s:%s]进程不存在 pid=[%d]",\
		pPrgNode->nodeid.c_str(),pPrgNode->processname.c_str(),pPrgNode->pid);
	return -1;
// #if defined(AIX) 
// 	sprintf(execcmd,"ps -ef |grep %d",pPrgNode->pid);
// #endif
// #if defined(LINUX) 
// 	sprintf(execcmd,"ps -ef |grep %d",pPrgNode->pid);
// #endif
// #if defined(HP_UX) 
// 	sprintf(execcmd,"ps -ef |grep %d",pPrgNode->pid);
// #endif
// //	printf("exec=%s\n",execcmd);
// 	fp = popen(execcmd,"r");
// 	if (fp == NULL)
// 	{
// 		m_log.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"执行[%s]出错",execcmd);
// 		return 0;
// 	}
// 	int offset=0;
// 	bool isfind=false;
// 	for(; !feof(fp);)
// 	{
// 		memset(execcmd,0,sizeof(execcmd));
// 		fgets(execcmd,300,fp);
// 		if(feof(fp))
// 		{
// 			break;
// 		}
// 		m_publog.ltrim(execcmd);
// 		memset(stemp1,0,sizeof(stemp1));
// 		memset(tmpchar,0,sizeof(tmpchar));
// 		sscanf(execcmd,"%s%s",stemp1,tmpchar);
// 		if (atoi(tmpchar)== pPrgNode->pid)
// 		{
// 			pclose(fp);
// 			return 0;
// 		}
// 	}
// 	pclose(fp);


#endif
	return -1;

}

void CProcessMgr::Init()
{
	
}
bool CProcessMgr::SeparateFilePathAndFileName(char *pFilePathName, char *pFilePath,int iFilePathLen, char *pFileName, int iFileNameLen)
{
	char *pdest, *pTemp;
	
	pTemp = pFilePathName;
	while(1)
	{
		pdest = strchr(pTemp, '\\');
		if (NULL == pdest)
			break;
		pTemp = pdest + 1;
	}
	
	strncpy(pFileName, pTemp, iFileNameLen);
	pFileName[iFileNameLen - 1] = 0;
	if (0 == strcmp(pFilePathName, pFileName))
	{
		strncpy(pFilePath, ".\\", iFilePathLen);
		pFilePath[iFilePathLen - 1] = 0;
	}
	else
	{
		int iLen = pTemp - pFilePathName;
		if (iLen <= iFilePathLen)
		{
			strncpy(pFilePath, pFilePathName, iLen);
		}
		else
		{
			strncpy(pFilePath, pFilePathName, iFilePathLen);
		}
		
		pFilePath[iFilePathLen - 1] = 0;
	}
	
	return true;
}

bool CProcessMgr::CheckPrgRunPara(S_PROCESS_NODE *pPrgNode)
{
#ifdef LINUX
	char tmpchar[400];
	char prgpath[400];
	char prgname[400];
	char runpath[400];
	bzero(tmpchar,sizeof(tmpchar));
	bzero(prgpath,sizeof(prgpath));
	bzero(prgname,sizeof(prgname));
	bzero(runpath,sizeof(runpath));
	
	sprintf(tmpchar,"/proc/%d/exe",pPrgNode->pid);
	//程序信息
	int count = readlink(tmpchar, runpath, 300);
	if (count<0 || count >300)
	{
//		printf("进程的程序信息不存在\n");
		m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"readlink返回错[%d]",count);
		return -1;
	}
	runpath[count]=0;
	GetNamePath(runpath,prgpath,prgname);
//	printf("进程的程序信息 程序目录[%s]程序名称[%s]\n",prgpath,prgname);
	//运行目录
	bzero(runpath,sizeof(runpath));
	sprintf(tmpchar,"/proc/%d/cwd",pPrgNode->pid);
	count = readlink(tmpchar, runpath, 300);
	if (count<0 || count >300)
	{
//		printf("进程的程序信息不存在\n");
		m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"readlink返回错[%d]",count);
		return -1;
	}
	runpath[count]=0;
	m_log.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"pid=[%d] 程序目录[%s] 程序名[%s] 运行目录[%s]",\
			pPrgNode->pid,prgpath,prgname,runpath);
//	printf("进程的信息  运行目录[%s]\n",runpath);
	if (pPrgNode->prgname.length() > 0 )
	{
		if (pPrgNode->prgname.compare(prgname) != 0)
		{
			m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"程序名称不同  [%s] 程序名[%s]",\
				prgname,pPrgNode->prgname.c_str());
			return -1;
		}
	}
	if (pPrgNode->prgpath.length() > 0 )
	{
		if (pPrgNode->prgpath.compare(prgpath) != 0)
		{
			m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"程序目录不同 [%s] 程序目录[%s]",\
				prgpath,pPrgNode->prgpath.c_str());
			return -1;
		}
	}
	if (pPrgNode->runpath.length() > 0 )
	{
		if (pPrgNode->runpath.compare(runpath) != 0)
		{
			m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"运行目录不同 [%s] 目录[%s]",\
				runpath,pPrgNode->runpath.c_str());
			return -1;
		}
	}
	return 0;
	
	
#else
	return 0;
#endif
}
void CProcessMgr::GetNamePath(const char *filepathname,char *filepath,char *filename)
{
	int filelen;
	int i;
	int offset = -1;
	filelen = strlen(filepathname);
	if (filelen < 1)
	{
		return ;
	}
	for (i=filelen-1 ;i>=0 ; i--)
	{
		if (filepathname[i] == '\\' || filepathname[i] == '/' )
		{
			offset = i;
			break;
		}
	}
	if (offset > -1)
	{
		memcpy(filepath,filepathname,offset);
		filepath[offset]=0;
		filelen = filelen - offset;
		for (i=0; i< filelen; i++)
		{
			filename[i]=filepathname[i+offset+1];
		}
		filename[filelen-1]=0;
		for (i=0; i<filelen-1; i++)
		{
			if (filename[i] == ' ')
			{
				filename[i] = 0;
				return;
			}
		}
	}
	return ;
}
