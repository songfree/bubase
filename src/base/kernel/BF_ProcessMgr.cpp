// ProcessMgr.cpp: implementation of the CProcessMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "BF_ProcessMgr.h"
#include "BF_Tools.h"

//#include "ListenThread.h"

//extern CListenThread g_plisten;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CBF_ProcessMgr::CBF_ProcessMgr()
{
	m_log.SetLogPara(LOG_PROMPT,"","monitor.log");
}

CBF_ProcessMgr::~CBF_ProcessMgr()
{

}
//创建进程
bool CBF_ProcessMgr::StartProcess(S_PROCESS_NODE *pPrgNode,char *errmsg,bool noConsole)
{
//	CBF_PMutex pp(&(pPrgNode->mutex));
	char  curpath[300];
	memset(curpath,0,sizeof(curpath));

	//取得当前目录，并进到进程运行目录

#ifdef _WINDOWS
	GetCurrentDirectory(sizeof(curpath)-1,curpath);
	if (!SetCurrentDirectory(pPrgNode->runpath.c_str()))
	{
		sprintf(errmsg,"节点[%s]名称[%s]程序运行目录[%s]不存在",\
			pPrgNode->nodeid.c_str(),pPrgNode->processname.c_str(),pPrgNode->runpath.c_str());
		m_log.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,errmsg);		
		return false;
	}
#else
	getcwd(curpath,sizeof(curpath)-1);
	if (access(pPrgNode->runpath.c_str(),0)!=0) 
	{
		sprintf(errmsg,"节点[%s]名称[%s]程序运行目录[%s]不存在",\
			pPrgNode->nodeid.c_str(),pPrgNode->processname.c_str(),pPrgNode->runpath.c_str());
		m_log.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,errmsg);		
		return false;
	}
	chdir(pPrgNode->runpath.c_str());
#endif

#ifdef _WINDOWS
//	unsigned long rtn;
	STARTUPINFO si;
	PROCESS_INFORMATION pidt;
	memset(&si,0,sizeof(si));
	si.cb = sizeof(si);
	char szCmdLine[20480];
	if (pPrgNode->prgpath.length()>0)
	{
		sprintf(szCmdLine,"%s\\%s %s",pPrgNode->prgpath.c_str(),pPrgNode->prgname.c_str(),pPrgNode->prgpara.c_str());
	}
	else
	{
		sprintf(szCmdLine,"%s %s",pPrgNode->prgname.c_str(),pPrgNode->prgpara.c_str());
	}
	DWORD dwCreationFlags;  
	if (noConsole)
	{
		dwCreationFlags = CREATE_NEW_CONSOLE|CREATE_DEFAULT_ERROR_MODE;//显示控制台
	}
	else
	{
		if (pPrgNode->winconsole == 0)
		{
			dwCreationFlags = DETACHED_PROCESS|CREATE_DEFAULT_ERROR_MODE;//不显示控制台
		}
		else
		{
			dwCreationFlags = CREATE_NEW_CONSOLE|CREATE_DEFAULT_ERROR_MODE;//显示控制台
		}
	}
	if (!CreateProcess(
		NULL,   // No module name (use command line).
		szCmdLine,    //szBUProg,
		NULL,   // If lpProcessAttributes is NULL, the handle cannot be inherited
		NULL,   // If lpThreadAttributes is NULL, the handle cannot be inherited
		FALSE,  // If the parameter is FALSE, the handles are not inherited.
		dwCreationFlags, // This flag is valid only when starting a console application. If set, the console application is run without a console window. 
		NULL,   //  Use parent's environment block. 
		pPrgNode->runpath.c_str(),  // ./bunit
		&si,
		&pidt
		))
	{
		sprintf(errmsg,"启动节点[%s:%s]失败 <%s:%s>",\
			pPrgNode->nodeid.c_str(),pPrgNode->prgname.c_str(),szCmdLine,pPrgNode->prgpath.c_str());
		m_log.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,errmsg);
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
			pPrgNode->nodeid.c_str(),pPrgNode->prgname.c_str(),pidt.dwProcessId);
	}
	//将当前目录设置回去
	SetCurrentDirectory(curpath);
#else
	pid_t pid;
	char szCmdLine[2048];
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
	char argvList[50][2000];
	char spara[10240];
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
	if (pid == 0)
	{
		execv(szCmdLine,argvs);
//		g_plisten.CloseListen(false);
		//如果execv函数返回，表明没有正常执行命令，打印错误信息
		sprintf(errmsg,"curpath=%s runpath=%s szCmdLine=%s 启动失败 %d",curpath,pPrgNode->runpath.c_str(),szCmdLine,errno);
		m_log.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,errmsg);
		_exit(0);
		return false;
	}
	else if (pid>0)
	{
		pPrgNode->pid = pid;
		pPrgNode->status = "正常";
	}
	m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"启动节点[%s:%s]完成 pid=[%d]",pPrgNode->nodeid.c_str(),pPrgNode->processname.c_str(),pPrgNode->pid);

#endif
	//将当前目录设置回去
#ifdef _WINDOWS
	SetCurrentDirectory(curpath);
#else
	chdir(curpath);
#endif
	return true;
}

int CBF_ProcessMgr::StopProcess(S_PROCESS_NODE *pPrgNode,char *errmsg)
{
//	CBF_PMutex pp(&pPrgNode->mutex);
	//取得当前目录，并进到进程运行目录
	char  curpath[300];
	memset(curpath,0,sizeof(curpath));	
#ifdef _WINDOWS
	GetCurrentDirectory(sizeof(curpath)-1,curpath);
	if (!SetCurrentDirectory(pPrgNode->runpath.c_str()))
	{
		sprintf(errmsg,"节点[%s]名称[%s]程序运行目录[%s]不存在",\
			pPrgNode->nodeid.c_str(),pPrgNode->processname.c_str(),pPrgNode->runpath.c_str());	
		m_log.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,errmsg);		
		return -1;
	}
#else
	getcwd(curpath,sizeof(curpath)-1);
	if (access(pPrgNode->runpath.c_str(),0)!=0) 
	{
		sprintf(errmsg,"节点[%s]名称[%s]程序运行目录[%s]不存在",\
			pPrgNode->nodeid.c_str(),pPrgNode->processname.c_str(),pPrgNode->runpath.c_str());
		m_log.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,errmsg);		
		return -1;
	}
	chdir(pPrgNode->runpath.c_str());
#endif

	//执行退出脚本
	if (pPrgNode->stopscript.length()>0)
	{
		system(pPrgNode->stopscript.c_str());
	}
	
#ifdef _WINDOWS
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
	memset(execcmd,0,sizeof(execcmd));
	memset(tmpchar,0,sizeof(tmpchar));
	memset(stemp1,0,sizeof(stemp1));
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
	chdir(curpath);
	return 0;

#endif
	return 0;
}
void CBF_ProcessMgr::SetLogPara(int level,const char *logfilepath,const char *logfilename,int id)
{
	m_log.SetLogPara(level,logfilepath,logfilename,id);
}

int CBF_ProcessMgr::FindProcess(S_PROCESS_NODE *pPrgNode,char *errmsg)
{
	
#ifdef _WINDOWS
	return CheckPrgRunPara(pPrgNode,errmsg);
#else
	FILE *fp;
	char execcmd[300];
	char tmpchar[200];
	char stemp1[100];
	
	memset(execcmd,0,sizeof(execcmd));
	memset(tmpchar,0,sizeof(tmpchar));
	memset(stemp1,0,sizeof(stemp1));
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
			if (CheckPrgRunPara(pPrgNode,errmsg) == 0)
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

#endif
	return -1;

}

bool CBF_ProcessMgr::SeparateFilePathAndFileName(char *pFilePathName, char *pFilePath,int iFilePathLen, char *pFileName, int iFileNameLen)
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

int CBF_ProcessMgr::CheckPrgRunPara(S_PROCESS_NODE *pPrgNode,char *errmsg)
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
		sprintf(errmsg,"readlink[%s]返回错[%d]",tmpchar,count);
		m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,errmsg);
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
		sprintf(errmsg,"readlink[%s]返回错[%d]",tmpchar,count);
		m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,errmsg);
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
			sprintf(errmsg,"程序名称不同  [%s] 程序名[%s]",\
				prgname,pPrgNode->prgname.c_str());
			m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,errmsg);
			return -1;
		}
	}
	if (pPrgNode->prgpath.length() > 0 )
	{
		if (pPrgNode->prgpath.compare(prgpath) != 0)
		{
			sprintf(errmsg,"程序目录不同 [%s] 程序目录[%s]",\
				prgpath,pPrgNode->prgpath.c_str());
			m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,errmsg);
			return -1;
		}
	}
	if (pPrgNode->runpath.length() > 0 )
	{
		if (pPrgNode->runpath.compare(runpath) != 0)
		{
			sprintf(errmsg,"运行目录不同 [%s] 目录[%s]",\
				runpath,pPrgNode->runpath.c_str());
			m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,errmsg);
			return -1;
		}
	}
	return 0;
	
	
#elif _WINDOWS
	HANDLE h = OpenProcess(PROCESS_ALL_ACCESS, TRUE,pPrgNode->pid);
	//HANDLE h = OpenProcess(PROCESS_ALL_ACCESS, FALSE,pPrgNode->pid);
	if (NULL == h)
	{
		sprintf(errmsg,"节点[%s:%s]进程不存在 pid=[%d] GetLastError[%d]",\
			pPrgNode->nodeid.c_str(),pPrgNode->processname.c_str(),pPrgNode->pid,GetLastError());
		m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,errmsg);
		return -1;
	
	}
#ifdef PRINTPROCESSMEM
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo( h, &pmc, sizeof(pmc));
	m_log.LogMp(LOG_PROMPT,__FILE__,__LINE__,"节点[%s:%s]  pid=[%d] 当前占用内存:%dK 最高占用内存:%dK",\
		pPrgNode->nodeid.c_str(),pPrgNode->processname.c_str(),pPrgNode->pid,pmc.WorkingSetSize/1024,pmc.PeakWorkingSetSize/1024);
#endif
	char szFilePathName[1024];
	bzero(szFilePathName,sizeof(szFilePathName));
	strcpy(szFilePathName,pPrgNode->prgname.c_str());
	CBF_Tools::ToUpper(szFilePathName);
	//JAVA程序不用判断目录和程序名
	if (strcmp(szFilePathName,"JAVA.EXE") == 0)
	{
		CloseHandle(h);
		return 0;
	}
	bzero(szFilePathName,sizeof(szFilePathName));
	//获得进程的完整路径（包含可执行文件名）
	if (GetModuleFileNameEx(h, NULL, szFilePathName,sizeof(szFilePathName)) <= 0)
	{				
		CloseHandle(h);
		sprintf(errmsg,"GetModuleFileNameEx 节点[%s:%s]进程不存在 pid=[%d] GetLastError[%d]",\
			pPrgNode->nodeid.c_str(),pPrgNode->processname.c_str(),pPrgNode->pid,GetLastError());
		m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,errmsg);
		return -1;
	}

	char szFilePath[512];
	char szFileName[512];
	bzero(szFilePath,sizeof(szFilePath));
	bzero(szFileName,sizeof(szFileName));
	GetNamePath(szFilePathName,szFilePath,szFileName);
#ifdef _DEBUG
	m_log.LogMp(LOG_PROMPT,__FILE__,__LINE__,"节点[%s:%s]  pid=[%d]  程序名[%s] 程序目录[%s]",\
			pPrgNode->nodeid.c_str(),pPrgNode->processname.c_str(),pPrgNode->pid,szFileName,szFilePath);
#endif
	if (0 == strcmp(szFileName,pPrgNode->prgname.c_str() ) )
	{
		if (strcmp(szFilePath,pPrgNode->prgpath.c_str()) == 0 )
		{
			//程序目录相同，判断参数是否相同
			DWORD dwThreadId = 0;
			DWORD dwExitCode = 0;
			//DWORD dwReaded = 0;
			SIZE_T dwReaded = 0;
			char  szCommandLine[500];
			char  szparaLine[500];
			bzero(szparaLine,sizeof(szparaLine));
			bzero(szCommandLine,sizeof(szCommandLine));
			HANDLE hThread  = ::CreateRemoteThread(h, NULL, NULL, (LPTHREAD_START_ROUTINE)GetCommandLine, NULL, 0, &dwThreadId);
			
			if (hThread != NULL)
			{
				::WaitForSingleObject(hThread, INFINITE);
				::GetExitCodeThread(hThread, &dwExitCode);
				::ReadProcessMemory(h, (LPCVOID)dwExitCode, szCommandLine, sizeof(szCommandLine)-1, &dwReaded);
				int len = strlen(szCommandLine);
				int spaceoffset = 0;
				for (int k=0;k<len;k++)
				{
					if (szCommandLine[k] == ' ')
					{
						spaceoffset = k;
						break;
					}
				}
				if (spaceoffset !=0 )
				{
					memcpy(szparaLine,szCommandLine+spaceoffset,len-spaceoffset);
					CBF_Tools::LRtrim(szparaLine);
				}
				if (strcmp(szparaLine,pPrgNode->prgpara.c_str()) == 0)
				{
					CloseHandle(h);
					return 0;
				}
				sprintf(errmsg,"节点[%s:%s]进程 pid=[%d]  szCommandLine[%s] 参数[%s] 参数不相同，所以进程不存在",\
					pPrgNode->nodeid.c_str(),pPrgNode->processname.c_str(),pPrgNode->pid,szCommandLine,pPrgNode->prgpara.c_str());
				m_log.LogMp(LOG_PROMPT,__FILE__,__LINE__,errmsg);
				CloseHandle(h);
				return -1;	
			}
			sprintf(errmsg,"CreateRemoteThread 节点[%s:%s]进程 pid=[%d] 出错，进程不存在 ",\
				pPrgNode->nodeid.c_str(),pPrgNode->processname.c_str(),pPrgNode->pid);
			m_log.LogMp(LOG_PROMPT,__FILE__,__LINE__,errmsg);
			CloseHandle(h);
			return -1;
		}
		else  //if (strcmp(szFilePath,pPrgNode->prgpath.c_str()) == 0 )
		{
			sprintf(errmsg,"节点[%s:%s]进程 pid=[%d] prgname[%s] filename[%s] filepath[%s] 程序目录不相同，所以进程不存在",\
				pPrgNode->nodeid.c_str(),pPrgNode->processname.c_str(),pPrgNode->pid,\
				pPrgNode->prgname.c_str(),szFileName,szFilePath);
			m_log.LogMp(LOG_PROMPT,__FILE__,__LINE__,errmsg);
			CloseHandle(h);
			return -1;	
		}
		
	}
	else   //	if (0 == strcmp(szFileName,pPrgNode->prgname.c_str() ) )
	{
		sprintf(errmsg,"节点[%s:%s]进程 pid=[%d] prgname[%s] filename[%s] filepath[%s] 程序名称不符",\
				pPrgNode->nodeid.c_str(),pPrgNode->processname.c_str(),pPrgNode->pid,\
				pPrgNode->prgname.c_str(),szFileName,szFilePath);
		m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,errmsg);
		CloseHandle(h);
		return -1;	
	}
	
#endif
	return 0;
}
void CBF_ProcessMgr::GetNamePath(const char *filepathname,char *filepath,char *filename)
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
		if (0 == offset)
		{
			
		}
		else
		{
			memcpy(filepath,filepathname,offset);
		}
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
