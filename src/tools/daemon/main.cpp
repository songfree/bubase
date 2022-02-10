
#include "public.h"
#include "ShareMemory.h"
#include "BF_Md5.h"
#include "BF_Tools.h"
#include "MoniRun.h"

#ifdef WIN32
#ifdef _DEBUG
#pragma comment(lib, "bf_kerneld.lib") 
#pragma message("Automatically linking with  bf_kerneld.lib")
#else
#pragma comment(lib, "bf_kernel.lib") 
#pragma message("Automatically linking with  bf_kernel.lib")
#endif
#endif

char     *g_pExitFlag;
CMoniRun    g_pMoniRun;


#ifndef _WINDOWS

typedef void Sigfunc(int);
Sigfunc * Signal_proc(int signo, Sigfunc *func);
Sigfunc* Signal_proc(int signo, Sigfunc* func)
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
			g_pExitFlag[0] = 0;
			break;
		case SIGQUIT: //3  �����̰���CTRL+\��shell�з����źţ��źű����ݸ�shell��ǰ̨���еĽ��̣���Ӧ���źŵ�Ĭ�ϲ������˳� (QUIT) �ý��̡�
			printf("SIGQUIT�ź�%d\n",signo);
			g_pExitFlag[0]  = 0;
			break;
		case SIGILL: //4  Illegal Instruction
			printf("SIGILL�ź�%d\n",signo);
			g_pExitFlag[0]  = 0;
			break;
		case SIGABRT: //6  Abort signal from abort(3)
			printf("SIGABRT�ź�%d\n",signo);
			break;
		case SIGFPE://8 Floating point exception
			printf("SIGFPE�ź�%d\n",signo);
			break;
		case SIGKILL: //9  kill����   Kill signal
			printf("SIGKILL�ź�%d\n",signo);
			g_pExitFlag[0]  = 0;
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
			g_pExitFlag[0]  = 0;
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


void StringXOR(unsigned char *str1, unsigned char *str2)
{
	int i;
	
	for (i=0; i<16; i++) 
	{
		if ( isdigit(str1[i]) ) 
		{ 
			str1[i]-='0'; 
			continue; 
		}
		if ( isxdigit(str1[i]) ) 
		{ 
			str1[i]-=('A'-10); 
			continue; 
		}
	}
	for (i=0; i<16; i++) 
	{
		if ( isdigit(str2[i]) ) 
		{ 
			str2[i]-='0'; 
			continue; 
		}
		if ( isxdigit(str2[i]) ) 
		{ 
			str2[i]-=('A'-10); 
			continue; 
		}
	}
	for (i=0; i<16; i++) 
	{
		str1[i]^=str2[i];
	}
	for (i=0; i<16; i++) 
	{
		if ( str1[i] < 10 ) 
		{ 
			str1[i]+='0'; 
			continue; 
		}
		if ( str1[i] >= 10 ) 
		{ 
			str1[i]+=('A'-10); 
			continue; 
		}
	}
}

int main(int argc, char* argv[])
{
	//��һ�鹲���ڴ�
	char shmname[200];
	char confile[200];
	memset(shmname,0,sizeof(shmname));
	CShareMemory pShare;
	

// 	CBF_Des des;
// 	char key[17];
// 	char skey[33];
// 	strcpy(skey,"1111111111111111");
// 	strcpy(key,"2222222222222222");
// 	StringXOR((unsigned char *)skey,(unsigned char *)key);
// 	
// 	strcpy(key,"1234567890987654");
// 	
// 	des.SoftEndesc((unsigned char *)skey,(unsigned char *)key,shmname);
	
	CBF_Tools::GetModuleNamePath(argv,g_pMoniRun.g_pRes.g_sCurPath,g_pMoniRun.g_pRes.g_sModulePath,g_pMoniRun.g_pRes.g_sModuleName);
	
#ifdef _WINDOWS
	CBF_Md5 md5class;
	sprintf(shmname,"%s\\bfdaemon",g_pMoniRun.g_pRes.g_sCurPath);
	md5class.Update(shmname);
	sprintf(shmname,"bfdaemon_%s",md5class.ToString().c_str());
#else	
	sprintf(shmname,"%s/bfdaemon",g_pMoniRun.g_pRes.g_sCurPath);
#endif
	

	g_pExitFlag = (char *)pShare.Open(shmname,2);
	if (g_pExitFlag == NULL)
	{
		printf("�򿪹����ڴ��\n");
		return -1;
	}
#ifdef _WINDOWS
	printf("�����ڴ�id[%d]\n",pShare.m_shmid);
#else
	printf("�����ڴ�id[%d] �ļ�����ID[%s]\n",pShare.m_shmid,pShare.m_szmsg);
#endif
	if (argc >1 )
	{
		char para1[300];
		memset(para1,0,sizeof(para1));
		strcpy(para1,argv[1]);
		if (strcmp(para1,"stop") == 0 || strcmp(para1,"STOP") == 0 )
		{
			if (g_pExitFlag[0] == 1)
			{
				printf("�˳��������е�bfdaemon...\n");
				g_pExitFlag[0] = 0;
			}
			else
			{
				printf("bfdaemon���˳�\n");
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
		sprintf(confile,"%s","bfdaemon.xml");
	}

	
	
#ifndef WIN32
	//��unix���ں�̨����
	int ret;

	signal(SIGTTOU,SIG_IGN); 
	signal(SIGTTIN,SIG_IGN); 
	signal(SIGTSTP,SIG_IGN);
	if ( (ret=fork()) == 0) 
	{
		Signal_proc(SIGHUP, sig_hup); //1  Hangup detected on controlling terminal or death of controlling process
		Signal_proc(SIGINT, sig_int); //2  �����̰���CTRL+C��shell�з����źţ��źű����ݸ�shell��ǰ̨���еĽ��̣���Ӧ���źŵ�Ĭ�ϲ������ж� (INTERRUPT) �ý��̡�
		Signal_proc(SIGQUIT,sig_int); //3  �����̰���CTRL+\��shell�з����źţ��źű����ݸ�shell��ǰ̨���еĽ��̣���Ӧ���źŵ�Ĭ�ϲ������˳� (QUIT) �ý��̡�
		Signal_proc(SIGILL,sig_int);   //4  Illegal Instruction
		signal(SIGABRT,SIG_IGN);  //6  Abort signal from abort(3)
		signal(SIGFPE,SIG_IGN);   //8 Floating point exception
		Signal_proc(SIGKILL,sig_int); //9  kill����   Kill signal
		signal(SIGSEGV,SIG_IGN);  // 11 Invalid memory reference
		signal(SIGPIPE,SIG_IGN);  // 13 Broken pipe: write to pipe with no readers
		signal(SIGALRM, SIG_IGN); // 14 �𵽶�ʱ�������ã�ͨ���ǳ�����һ����ʱ��֮������ɸ��źš�
		Signal_proc(SIGTERM,sig_int); // 15 Termination signal
		Signal_proc(SIGUSR1, sig_usr1); // 30,10,16 User-defined signal 1
		Signal_proc(SIGUSR2, sig_usr1); // 31,12,17 User-defined signal 2
		signal(SIGCHLD,SIG_IGN);   // 20,17,18  Child stopped or terminated
		Signal_proc(SIGSTOP,sig_int);   // 17,19,23  Stop process
		Signal_proc(SIGBUS, sig_int); // 10,7,10  Bus error (bad memory access)
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
		//��ȡ�����ļ�
		if (!g_pMoniRun.Init(confile))
		{
			printf("��ʼ��ʧ��\n");
			return -1;
		}
		if (!g_pMoniRun.Start())
		{
			printf("%s\n",g_pMoniRun.GetErrMsg().c_str());
			return -1;
		}

		printf("���������ػ�bfdaemon�ɹ�\n");
		g_pExitFlag[0] = 1;
		while (true)
		{
			if (g_pExitFlag[0] != 1)
			{
				printf("------begin exit bfdaemon.......\n");
				g_pMoniRun.Stop();
				printf("------exit bfdaemon ok !\n");
				SLEEP(100);
				g_pExitFlag = NULL;
				return 0;
			}
			g_pMoniRun.Monitor();
			SLEEP_SECONDS(5);
			
		}
#ifndef WIN32
		exit(0);
	}
#endif
	return 0;
}

