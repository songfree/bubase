// bpc.cpp : Defines the entry point for the console application.
//



#include "ShareMemory.h"
#include "BpcRun.h"
#include "Bpc_Timer.h"
#include "DrebBpcLog.h"
#include "BpcEndian.h"
#include "DrebEndian.h"
#include "BF_Md5.h"
#include "PubCompress.h"

#ifdef _WINDOWS
#ifdef _DEBUG
#pragma comment(lib, "bf_kerneld.lib") 
#pragma message("Automatically linking with   bf_kerneld.lib")
#else
#pragma comment(lib, "bf_kernel.lib") 
#pragma message("Automatically linking with   bf_kernel.lib")
#endif

#endif



char *g_pExitFlag=NULL;
CBpcRun frun;
CBpc_Timer  g_pBpcTime;
CDrebBpcLog     g_pBpcLog;
CPubCompress  g_pCommpress;


char g_VERSION[20]="1.0.2.20100724";   //api�İ汾

#ifndef _WINDOWS

typedef void Sigfunc(int);
Sigfunc * Signal(int signo, Sigfunc *func);
Sigfunc* Signal(int signo, Sigfunc* func)
{
	struct sigaction act, oact;
	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_RESTART;
	
	if(signo == SIGALRM) 
	{
#ifdef SA_INTERRUPT
		act.sa_flags |= SA_INTERRUPT;   /* SunOS 4.x */
#endif
	} 
	else 
	{
#ifdef SA_RESTART
		act.sa_flags |= SA_RESTART;   /* SVR4, 4.4BSD */
#endif
	}
	
	if(sigaction(signo, &act, &oact) <0)
	{
		return(SIG_ERR);
	}
	return(oact.sa_handler);
}

void sig_int(int signo)
{
	switch(signo)
	{
		case SIGHUP:  //1  Hangup detected on controlling terminal or death of controlling process
			printf("SIGHUP�ź�%d\n",signo);
			break;
		case SIGINT:  //2 �����̰���CTRL+C��shell�з����źţ��źű����ݸ�shell��ǰ̨���еĽ��̣���Ӧ���źŵ�Ĭ�ϲ������ж� (INTERRUPT) �ý��̡�
			printf("SIGINT�ź�%d\n",signo);
			g_pExitFlag = 0;
			break;
		case SIGQUIT: //3  �����̰���CTRL+\��shell�з����źţ��źű����ݸ�shell��ǰ̨���еĽ��̣���Ӧ���źŵ�Ĭ�ϲ������˳� (QUIT) �ý��̡�
			printf("SIGQUIT�ź�%d\n",signo);
			g_pExitFlag = 0;
			break;
		case SIGILL: //4  Illegal Instruction
			printf("SIGILL�ź�%d\n",signo);
			g_pExitFlag = 0;
			break;
		case SIGABRT: //6  Abort signal from abort(3)
			printf("SIGABRT�ź�%d\n",signo);
			break;
		case SIGFPE://8 Floating point exception
			printf("SIGFPE�ź�%d\n",signo);
			break;
		case SIGKILL: //9  kill����   Kill signal
			printf("SIGKILL�ź�%d\n",signo);
			g_pExitFlag = 0;
			break;
		case SIGSEGV: // 11 Invalid memory reference
			printf("SIGSEGV�ź�%d\n",signo);
			break;
		case SIGPIPE:// 13 Broken pipe: write to pipe with no readers
			printf("SIGPIPE�ź�%d\n",signo);
			break;
		case SIGALRM:// 14 �𵽶�ʱ�������ã�ͨ���ǳ�����һ����ʱ��֮������ɸ��źš�
			printf("SIGALRM�ź�%d\n",signo);
			break;
		case SIGTERM:// 15 Termination signal
			printf("SIGTERM�ź�%d\n",signo);
			g_pExitFlag = 0;
			break;
		case SIGUSR1: // 30,10,16 User-defined signal 1
			printf("SIGUSR1�ź�%d\n",signo);
			break;
		case SIGUSR2:// 31,12,17 User-defined signal 2
			printf("SIGUSR2�ź�%d\n",signo);
			break;
		case SIGCHLD: // 20,17,18  Child stopped or terminated
			printf("SIGCHLD�ź�%d\n",signo);
			break;
		case SIGSTOP:// 17,19,23  Stop process
			printf("SIGSTOP�ź�%d\n",signo);
			break;
		case SIGBUS:// 10,7,10  Bus error (bad memory access)
			printf("SIGBUS�ź�%d\n",signo);
			break;
// 		case SIGCLD:  // -,-,18   A synonym for SIGCHLD
// 			printf("SIGCLD�ź�%d",signo);
// 			break;
		default:
			printf("�ź�%d",signo);
			break;
	}
// 	if ((signo == 2) || (signo == 4) || (signo == 15)  ) 
// 	{
// 		g_bIsExit = true;
// 	}
}
void sig_usr1(int signo)
{
}

void sig_hup(int signo)
{
}
#endif


int main(int argc, char* argv[])
{
	//��һ�鹲���ڴ�
	char shmname[500];
	char confile[500];
	memset(shmname,0,sizeof(shmname));
	CShareMemory pShare;
	
	
	CBF_Tools::GetModuleNamePath(argv,frun.m_pRes.g_sCurPath,frun.m_pRes.g_sModulePath,frun.m_pRes.g_sModuleName);
	

	CBF_Md5 md5class;
#ifdef _WINDOWS
	sprintf(shmname,"%s\\bfbpc",frun.m_pRes.g_sCurPath);
#else
	sprintf(shmname,"%s/bfbpc",frun.m_pRes.g_sCurPath);
#endif
	md5class.Update(shmname);
	strcpy(shmname,md5class.ToString().c_str());
	

	g_pExitFlag = (char *)pShare.Open(shmname,2+sizeof(S_SHM_PUBLIC));
	if (g_pExitFlag == NULL)
	{
		printf("�򿪹����ڴ��\n");
		return -1;
	}
#ifdef _WINDOWS
	printf("�����ڴ�id[%d] ���б�־[%d]\n",pShare.m_shmid,g_pExitFlag[0]);
#else
	printf("�����ڴ�id[%d] �ļ�����ID[%s] ���б�־[%d] \n",pShare.m_shmid,pShare.m_szmsg,g_pExitFlag[0]);
#endif
	if (argc >1 )
	{
		char para1[500];
		memset(para1,0,sizeof(para1));
		strcpy(para1,argv[1]);
		if (strcmp(para1,"stop") == 0 || strcmp(para1,"STOP") == 0 )
		{
			if (g_pExitFlag[0] == 1)
			{
				printf("�˳��������е�bfbpc...\n");
				g_pExitFlag[0] = 0;
			}
			else
			{
				printf("bfbpc���˳�\n");
				g_pExitFlag[0] = 0;
			}
			
			return -1;
		}
	}
	if (g_pExitFlag[0] == 1)
	{
		printf("�����������У�����ֹͣ!\n");
		return -1;
	}
	if (argc>1)
	{
		sprintf(confile,"%s",argv[1]);
	}
	else
	{
		sprintf(confile,"%s","bfbpc.xml");
	}

	bool bisMonitor = false;
	
#ifndef WIN32
	//��unix���ں�̨����
	int ret;

	signal(SIGTTOU,SIG_IGN); 
	signal(SIGTTIN,SIG_IGN); 
	signal(SIGTSTP,SIG_IGN);
	if ( (ret=fork()) == 0) 
	{
		signal(SIGHUP, sig_hup); //1  Hangup detected on controlling terminal or death of controlling process
    	signal(SIGINT, sig_int); //2  �����̰���CTRL+C��shell�з����źţ��źű����ݸ�shell��ǰ̨���еĽ��̣���Ӧ���źŵ�Ĭ�ϲ������ж� (INTERRUPT) �ý��̡�
    	signal(SIGQUIT,sig_int); //3  �����̰���CTRL+\��shell�з����źţ��źű����ݸ�shell��ǰ̨���еĽ��̣���Ӧ���źŵ�Ĭ�ϲ������˳� (QUIT) �ý��̡�
    	signal(SIGILL,sig_int);   //4  Illegal Instruction
    	signal(SIGABRT,SIG_IGN);  //6  Abort signal from abort(3)
    	signal(SIGFPE,SIG_IGN);   //8 Floating point exception
    	signal(SIGKILL,sig_int); //9  kill����   Kill signal
    	signal(SIGSEGV,SIG_IGN);  // 11 Invalid memory reference
    	signal(SIGPIPE,SIG_IGN);  // 13 Broken pipe: write to pipe with no readers
    	signal(SIGALRM, SIG_IGN); // 14 �𵽶�ʱ�������ã�ͨ���ǳ�����һ����ʱ��֮������ɸ��źš�
    	signal(SIGTERM,sig_int); // 15 Termination signal
    	signal(SIGUSR1, sig_usr1); // 30,10,16 User-defined signal 1
    	signal(SIGUSR2, sig_usr1); // 31,12,17 User-defined signal 2
    	signal(SIGCHLD,SIG_IGN);   // 20,17,18  Child stopped or terminated
    	signal(SIGSTOP,sig_int);   // 17,19,23  Stop process
    	signal(SIGBUS, sig_int); // 10,7,10  Bus error (bad memory access)
    	signal(SIGCLD,SIG_IGN);  // -,-,18   A synonym for SIGCHLD
#endif	
		
		//��PIDд�뵽�ļ���
		strcat(shmname,".pid");
		FILE *fp = fopen(shmname,"w");
		if (fp != NULL)
		{
#ifdef _WINDOWS
			sprintf(shmname,"PID[%d] �����ڴ�id[%d]",GetPid(),pShare.m_shmid);
#else
			sprintf(shmname,"PID[%d] �����ڴ�id[%d] �ļ�����ID[%s]",GetPid(),pShare.m_shmid,pShare.m_szmsg);
#endif
			fwrite(shmname,1,strlen(shmname),fp);
			fclose(fp);
		}

		frun.m_pRes.m_pSerial = (S_SHM_PUBLIC *)(g_pExitFlag+1);
		if (!frun.Init(confile))
		{
			return -1;
		}
		
#ifdef _WINDOWS
		g_pBpcTime.m_sNxtFilePath = std::string(frun.m_pRes.g_sCurPath)+"\\nxt";
#else
		g_pBpcTime.m_sNxtFilePath = std::string(frun.m_pRes.g_sCurPath)+"/nxt";
#endif
		printf("����bfbpc�ɹ� %d\n",frun.m_pRes.g_nPort);
		frun.Monitor();
		SLEEP_SECONDS(1);
		g_pExitFlag[0] = 1;
		while (true)
		{
			if (g_pExitFlag[0] != 1)
			{
				printf("------begin exit bfbpc.......\n");
				frun.m_pRes.g_bToExit = true;
				//frun.m_pRes.WriteBack();
				frun.Stop();
				printf("------exit bfbpc ok !\n");
				SLEEP(100);
				g_pExitFlag = NULL;
				return 0;
			}
			//��ش����߳�
			SLEEP_SECONDS(5);
			frun.Monitor();
			
			
		}
#ifndef WIN32
		exit(0);
	}
#endif
	return 0;
}

