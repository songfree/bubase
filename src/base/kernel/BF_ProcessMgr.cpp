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
//��������
bool CBF_ProcessMgr::StartProcess(S_PROCESS_NODE *pPrgNode,char *errmsg,bool noConsole)
{
//	CBF_PMutex pp(&(pPrgNode->mutex));
	char  curpath[300];
	memset(curpath,0,sizeof(curpath));

	//ȡ�õ�ǰĿ¼����������������Ŀ¼

#ifdef _WINDOWS
	GetCurrentDirectory(sizeof(curpath)-1,curpath);
	if (!SetCurrentDirectory(pPrgNode->runpath.c_str()))
	{
		sprintf(errmsg,"�ڵ�[%s]����[%s]��������Ŀ¼[%s]������",\
			pPrgNode->nodeid.c_str(),pPrgNode->processname.c_str(),pPrgNode->runpath.c_str());
		m_log.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,errmsg);		
		return false;
	}
#else
	getcwd(curpath,sizeof(curpath)-1);
	if (access(pPrgNode->runpath.c_str(),0)!=0) 
	{
		sprintf(errmsg,"�ڵ�[%s]����[%s]��������Ŀ¼[%s]������",\
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
		dwCreationFlags = CREATE_NEW_CONSOLE|CREATE_DEFAULT_ERROR_MODE;//��ʾ����̨
	}
	else
	{
		if (pPrgNode->winconsole == 0)
		{
			dwCreationFlags = DETACHED_PROCESS|CREATE_DEFAULT_ERROR_MODE;//����ʾ����̨
		}
		else
		{
			dwCreationFlags = CREATE_NEW_CONSOLE|CREATE_DEFAULT_ERROR_MODE;//��ʾ����̨
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
		sprintf(errmsg,"�����ڵ�[%s:%s]ʧ�� <%s:%s>",\
			pPrgNode->nodeid.c_str(),pPrgNode->prgname.c_str(),szCmdLine,pPrgNode->prgpath.c_str());
		m_log.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,errmsg);
		//����ǰĿ¼���û�ȥ
		SetCurrentDirectory(curpath);
		return false;
	}
	else
	{	
		pPrgNode->pid = pidt.dwProcessId;
		pPrgNode->status = "����";
		CloseHandle(pidt.hThread);
		CloseHandle(pidt.hProcess);
		m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"�����ڵ�[%s:%s]�ɹ� pid=[%d]",\
			pPrgNode->nodeid.c_str(),pPrgNode->prgname.c_str(),pidt.dwProcessId);
	}
	//����ǰĿ¼���û�ȥ
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
	//׼������
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
		//���execv�������أ�����û������ִ�������ӡ������Ϣ
		sprintf(errmsg,"curpath=%s runpath=%s szCmdLine=%s ����ʧ�� %d",curpath,pPrgNode->runpath.c_str(),szCmdLine,errno);
		m_log.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,errmsg);
		_exit(0);
		return false;
	}
	else if (pid>0)
	{
		pPrgNode->pid = pid;
		pPrgNode->status = "����";
	}
	m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"�����ڵ�[%s:%s]��� pid=[%d]",pPrgNode->nodeid.c_str(),pPrgNode->processname.c_str(),pPrgNode->pid);

#endif
	//����ǰĿ¼���û�ȥ
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
	//ȡ�õ�ǰĿ¼����������������Ŀ¼
	char  curpath[300];
	memset(curpath,0,sizeof(curpath));	
#ifdef _WINDOWS
	GetCurrentDirectory(sizeof(curpath)-1,curpath);
	if (!SetCurrentDirectory(pPrgNode->runpath.c_str()))
	{
		sprintf(errmsg,"�ڵ�[%s]����[%s]��������Ŀ¼[%s]������",\
			pPrgNode->nodeid.c_str(),pPrgNode->processname.c_str(),pPrgNode->runpath.c_str());	
		m_log.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,errmsg);		
		return -1;
	}
#else
	getcwd(curpath,sizeof(curpath)-1);
	if (access(pPrgNode->runpath.c_str(),0)!=0) 
	{
		sprintf(errmsg,"�ڵ�[%s]����[%s]��������Ŀ¼[%s]������",\
			pPrgNode->nodeid.c_str(),pPrgNode->processname.c_str(),pPrgNode->runpath.c_str());
		m_log.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,errmsg);		
		return -1;
	}
	chdir(pPrgNode->runpath.c_str());
#endif

	//ִ���˳��ű�
	if (pPrgNode->stopscript.length()>0)
	{
		system(pPrgNode->stopscript.c_str());
	}
	
#ifdef _WINDOWS
	HANDLE h = OpenProcess(PROCESS_ALL_ACCESS, TRUE, pPrgNode->pid);
	if (NULL != h)
	{
		TerminateProcess(h, 0);
		
		pPrgNode->status = "��ֹͣ";
		m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"ֹͣ�ڵ�[%s:%s]�ɹ� pid=[%d]",\
			pPrgNode->nodeid.c_str(),pPrgNode->processname.c_str(),pPrgNode->pid);
		pPrgNode->pid = -1;
	}
	CloseHandle(h);
	m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"�ڵ�[%s:%s]��ֹͣ pid=[%d]",\
			pPrgNode->nodeid.c_str(),pPrgNode->processname.c_str(),pPrgNode->pid);

	//����ǰĿ¼���û�ȥ

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
			pPrgNode->status = "��ֹͣ";
			m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"kill SIGKILLֹͣ�ڵ�[%s:%s]�ɹ� pid=[%d]",\
				pPrgNode->nodeid.c_str(),pPrgNode->processname.c_str(),pPrgNode->pid);
 			pPrgNode->pid = -1;
		}
		else
		{
			sprintf(execcmd,"kill -9 %d",pPrgNode->pid);
			system(execcmd);
			pPrgNode->status = "��ֹͣ";
			m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"kill -9 ֹͣ�ڵ�[%s:%s]�ɹ� pid=[%d]",\
				pPrgNode->nodeid.c_str(),pPrgNode->processname.c_str(),pPrgNode->pid);
 			pPrgNode->pid = -1;
		}
	}
	else
	{
		m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"�ڵ�[%s:%s]��ֹͣ pid=[%d]",\
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

		//��������������Ŀ¼������Ŀ¼
		if (pPrgNode->prgname.length() > 0 || pPrgNode->prgpath.length()>0 || pPrgNode->runpath.length()>0)
		{
			if (CheckPrgRunPara(pPrgNode,errmsg) == 0)
			{
				return 0;
			}
			m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"��������Ϣ������");
			return -1;
		}
		return 0;
	}
	m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,"�ڵ�[%s:%s]���̲����� pid=[%d]",\
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
	//������Ϣ
	int count = readlink(tmpchar, runpath, 300);
	if (count<0 || count >300)
	{
		sprintf(errmsg,"readlink[%s]���ش�[%d]",tmpchar,count);
		m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,errmsg);
		return -1;
	}
	runpath[count]=0;
	GetNamePath(runpath,prgpath,prgname);
//	printf("���̵ĳ�����Ϣ ����Ŀ¼[%s]��������[%s]\n",prgpath,prgname);
	//����Ŀ¼
	bzero(runpath,sizeof(runpath));
	sprintf(tmpchar,"/proc/%d/cwd",pPrgNode->pid);
	count = readlink(tmpchar, runpath, 300);
	if (count<0 || count >300)
	{
		sprintf(errmsg,"readlink[%s]���ش�[%d]",tmpchar,count);
		m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,errmsg);
		return -1;
	}
	runpath[count]=0;
	m_log.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"pid=[%d] ����Ŀ¼[%s] ������[%s] ����Ŀ¼[%s]",\
			pPrgNode->pid,prgpath,prgname,runpath);
//	printf("���̵���Ϣ  ����Ŀ¼[%s]\n",runpath);
	if (pPrgNode->prgname.length() > 0 )
	{
		if (pPrgNode->prgname.compare(prgname) != 0)
		{
			sprintf(errmsg,"�������Ʋ�ͬ  [%s] ������[%s]",\
				prgname,pPrgNode->prgname.c_str());
			m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,errmsg);
			return -1;
		}
	}
	if (pPrgNode->prgpath.length() > 0 )
	{
		if (pPrgNode->prgpath.compare(prgpath) != 0)
		{
			sprintf(errmsg,"����Ŀ¼��ͬ [%s] ����Ŀ¼[%s]",\
				prgpath,pPrgNode->prgpath.c_str());
			m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,errmsg);
			return -1;
		}
	}
	if (pPrgNode->runpath.length() > 0 )
	{
		if (pPrgNode->runpath.compare(runpath) != 0)
		{
			sprintf(errmsg,"����Ŀ¼��ͬ [%s] Ŀ¼[%s]",\
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
		sprintf(errmsg,"�ڵ�[%s:%s]���̲����� pid=[%d] GetLastError[%d]",\
			pPrgNode->nodeid.c_str(),pPrgNode->processname.c_str(),pPrgNode->pid,GetLastError());
		m_log.LogMp(LOG_WARNNING,__FILE__,__LINE__,errmsg);
		return -1;
	
	}
#ifdef PRINTPROCESSMEM
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo( h, &pmc, sizeof(pmc));
	m_log.LogMp(LOG_PROMPT,__FILE__,__LINE__,"�ڵ�[%s:%s]  pid=[%d] ��ǰռ���ڴ�:%dK ���ռ���ڴ�:%dK",\
		pPrgNode->nodeid.c_str(),pPrgNode->processname.c_str(),pPrgNode->pid,pmc.WorkingSetSize/1024,pmc.PeakWorkingSetSize/1024);
#endif
	char szFilePathName[1024];
	bzero(szFilePathName,sizeof(szFilePathName));
	strcpy(szFilePathName,pPrgNode->prgname.c_str());
	CBF_Tools::ToUpper(szFilePathName);
	//JAVA�������ж�Ŀ¼�ͳ�����
	if (strcmp(szFilePathName,"JAVA.EXE") == 0)
	{
		CloseHandle(h);
		return 0;
	}
	bzero(szFilePathName,sizeof(szFilePathName));
	//��ý��̵�����·����������ִ���ļ�����
	if (GetModuleFileNameEx(h, NULL, szFilePathName,sizeof(szFilePathName)) <= 0)
	{				
		CloseHandle(h);
		sprintf(errmsg,"GetModuleFileNameEx �ڵ�[%s:%s]���̲����� pid=[%d] GetLastError[%d]",\
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
	m_log.LogMp(LOG_PROMPT,__FILE__,__LINE__,"�ڵ�[%s:%s]  pid=[%d]  ������[%s] ����Ŀ¼[%s]",\
			pPrgNode->nodeid.c_str(),pPrgNode->processname.c_str(),pPrgNode->pid,szFileName,szFilePath);
#endif
	if (0 == strcmp(szFileName,pPrgNode->prgname.c_str() ) )
	{
		if (strcmp(szFilePath,pPrgNode->prgpath.c_str()) == 0 )
		{
			//����Ŀ¼��ͬ���жϲ����Ƿ���ͬ
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
				sprintf(errmsg,"�ڵ�[%s:%s]���� pid=[%d]  szCommandLine[%s] ����[%s] ��������ͬ�����Խ��̲�����",\
					pPrgNode->nodeid.c_str(),pPrgNode->processname.c_str(),pPrgNode->pid,szCommandLine,pPrgNode->prgpara.c_str());
				m_log.LogMp(LOG_PROMPT,__FILE__,__LINE__,errmsg);
				CloseHandle(h);
				return -1;	
			}
			sprintf(errmsg,"CreateRemoteThread �ڵ�[%s:%s]���� pid=[%d] �������̲����� ",\
				pPrgNode->nodeid.c_str(),pPrgNode->processname.c_str(),pPrgNode->pid);
			m_log.LogMp(LOG_PROMPT,__FILE__,__LINE__,errmsg);
			CloseHandle(h);
			return -1;
		}
		else  //if (strcmp(szFilePath,pPrgNode->prgpath.c_str()) == 0 )
		{
			sprintf(errmsg,"�ڵ�[%s:%s]���� pid=[%d] prgname[%s] filename[%s] filepath[%s] ����Ŀ¼����ͬ�����Խ��̲�����",\
				pPrgNode->nodeid.c_str(),pPrgNode->processname.c_str(),pPrgNode->pid,\
				pPrgNode->prgname.c_str(),szFileName,szFilePath);
			m_log.LogMp(LOG_PROMPT,__FILE__,__LINE__,errmsg);
			CloseHandle(h);
			return -1;	
		}
		
	}
	else   //	if (0 == strcmp(szFileName,pPrgNode->prgname.c_str() ) )
	{
		sprintf(errmsg,"�ڵ�[%s:%s]���� pid=[%d] prgname[%s] filename[%s] filepath[%s] �������Ʋ���",\
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
