// bfdreb.cpp : Defines the entry point for the console application.
//

#include "public.h"
#include "BFDreb.h"
#include "ShareMemory.h"
#include "BF_Md5.h"
#include "Dreb_Timer.h"
#include "DrebBpcLog.h"


#ifdef _WINDOWS
#ifdef _DEBUG
#pragma comment(lib, "bf_dbpubd.lib") 
#pragma comment(lib, "bf_kerneld.lib") 
#pragma message("Automatically linking with  bf_dbpubd.lib bf_kerneld.lib ")
#else
#pragma comment(lib, "bf_kernel.lib") 
#pragma comment(lib, "bf_dbpub.lib") 
#pragma message("Automatically linking with  bf_dbpub.lib bf_kernel.lib ")
#endif

#endif


char g_VERSION[40];
CBFDreb g_pDreb;
CDreb_Timer g_pDrebTimer;

char *g_pExitFlag=NULL;
int   g_pRunFlag = 1;


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
			printf("SIGHUP信号%d\n",signo);
			break;
		case SIGINT:  //2 当键盘按下CTRL+C从shell中发出信号，信号被传递给shell中前台运行的进程，对应该信号的默认操作是中断 (INTERRUPT) 该进程。
			printf("SIGINT信号%d\n",signo);
			g_pRunFlag = 0;
			break;
		case SIGQUIT: //3  当键盘按下CTRL+\从shell中发出信号，信号被传递给shell中前台运行的进程，对应该信号的默认操作是退出 (QUIT) 该进程。
			printf("SIGQUIT信号%d\n",signo);
			g_pRunFlag  = 0;
			break;
		case SIGILL: //4  Illegal Instruction
			printf("SIGILL信号%d\n",signo);
			g_pRunFlag  = 0;
			break;
		case SIGABRT: //6  Abort signal from abort(3)
			printf("SIGABRT信号%d\n",signo);
			break;
		case SIGFPE://8 Floating point exception
			printf("SIGFPE信号%d\n",signo);
			break;
		case SIGKILL: //9  kill进程   Kill signal
			printf("SIGKILL信号%d\n",signo);
			g_pRunFlag  = 0;
			break;
		case SIGSEGV: // 11 Invalid memory reference
			printf("SIGSEGV信号%d\n",signo);
			break;
		case SIGPIPE:// 13 Broken pipe: write to pipe with no readers
			printf("SIGPIPE信号%d\n",signo);
			break;
		case SIGALRM:// 14 起到定时器的作用，通常是程序在一定的时间之后才生成该信号。
			printf("SIGALRM信号%d\n",signo);
			break;
		case SIGTERM:// 15 Termination signal
			printf("SIGTERM信号%d\n",signo);
			g_pRunFlag  = 0;
			break;
		case SIGUSR1: // 30,10,16 User-defined signal 1
			printf("SIGUSR1信号%d\n",signo);
			break;
		case SIGUSR2:// 31,12,17 User-defined signal 2
			printf("SIGUSR2信号%d\n",signo);
			break;
		case SIGCHLD: // 20,17,18  Child stopped or terminated
			printf("SIGCHLD信号%d\n",signo);
			break;
		case SIGSTOP:// 17,19,23  Stop process
			printf("SIGSTOP信号%d\n",signo);
			break;
		case SIGBUS:// 10,7,10  Bus error (bad memory access)
			printf("SIGBUS信号%d\n",signo);
			break;
// 		case SIGCLD:  // -,-,18   A synonym for SIGCHLD
// 			printf("SIGCLD信号%d",signo);
// 			break;
		default:
			printf("信号%d",signo);
			break;
	}
// 	if ((signo == 2) || (signo == 4) || (signo == 15)  ) 
// 	{
// 		g_pRunFlag = true;
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

	memset(g_VERSION,0,sizeof(g_VERSION));
	sprintf(g_VERSION,"1.0.4 %s",__DATE__);
	
	CBF_Tools::GetModuleNamePath(argv,g_pDreb.g_Vars.g_curentpath,g_pDreb.g_Vars.g_modulepath,g_pDreb.g_Vars.g_modulename);

	//打开一块共享内存
	char shmname[400];
	char confile[200];
	memset(shmname,0,sizeof(shmname));
	memset(confile,0,sizeof(confile));
	if (argc == 1)
	{
		printf("dreb build [%s %s]命令行参数说明:\n",__DATE__,__TIME__);
		printf("               不带参数 后台运行 配置文件为bfdreb.xml \n");
		printf("               stop/STOP  退出bfdreb \n");
		printf("               -cf  前台运行 \n");
		printf("               -cb  后台运行 无-c时默认为后台运行 \n");
		printf("               -m   设置共享内存参数 \n");
		printf("               -ml  设置日志级别   -ml5设置日志级别为5，即调试 \n");
		printf("               -md  设置DATA日志级别   -md5设置DATA日志级别为5，即调试 \n");
		printf("               -mq  退出正在运行的程序 \n");
		printf("               -f   设置配置文件 -ftmc.xml配置文件为tmc.xml  默认配置文件为bfdreb.xml \n");
		printf("               -q   退出 \n");
	}
	CShareMemory pShare;
	
#ifdef _WINDOWS
	CBF_Md5 md5class;
	sprintf(shmname,"%s\\bfdreb",g_pDreb.g_Vars.g_curentpath);
	md5class.Update(shmname);
	sprintf(shmname,"bfdreb_%s",md5class.ToString().c_str());
#else
	sprintf(shmname,"%s/bfdreb",g_pDreb.g_Vars.g_curentpath);
#endif
	

	g_pExitFlag = (char *)pShare.Open(shmname,sizeof(S_SHM_PUBLIC)+5);
	if (g_pExitFlag == NULL)
	{
		printf("打开共享内存错\n");
		return -1;
	}
#ifdef _WINDOWS
	printf("共享内存id[%d] 运行标志[%d] 缓冲大小[%ld]\n",pShare.m_shmid,g_pExitFlag[0],DREBBUFSIZE);
#else
	printf("共享内存id[%d] 文件索引ID[%s] 运行标志[%d] 缓冲大小[%ld]\n",pShare.m_shmid,pShare.m_szmsg,g_pExitFlag[0],DREBBUFSIZE);
#endif
	
	if (argc == 2 ) //检查是否为stop，兼容之前的情况
	{
		char para1[500];
		memset(para1,0,sizeof(para1));
		strcpy(para1,argv[1]);
		if (strcmp(para1,"stop") == 0 || strcmp(para1,"STOP") == 0 )
		{
			if (g_pExitFlag[0] == 1)
			{
				printf("退出正在运行的bfdreb...\n");
				g_pExitFlag[0] = 0;
			}
			else
			{
				printf("bfdreb已退出\n");
				g_pExitFlag[0] = 0;
			}
			return -1;
		}
	}
	//检查参数
	int i;
	bool isRunFront=false; //是否前台运行
	
	for (i=1; i< argc ; i++)
	{
		if (strncmp(argv[i],"-cf",3) == 0)
		{
			isRunFront = true;
		}
		if (strncmp(argv[i],"-cb",3) == 0)
		{
			isRunFront = false;
		}
		if (strncmp(argv[i],"-mq",3) == 0 || strncmp(argv[i],"-q",2) == 0)
		{
			if (g_pExitFlag[0]  == 1)
			{
				printf("退出正在运行的bfdreb...\n");
				g_pExitFlag[0] = 0;
			}
			else
			{
				printf("bfdreb已退出\n");
				g_pExitFlag[0] = 0;
			}
			return -1;
		}
		if (strncmp(argv[i],"-f",2) == 0)
		{
			strcpy(confile,argv[i]+2);
		}
		
		if (strncmp(argv[i],"-ml",3) == 0)
		{
			int loglevel ;
			printf("当前日志级别为%d,设置日志级别为 %d\n",g_pExitFlag[2],atoi(argv[i]+3));
			loglevel = atoi(argv[i]+3);
			g_pExitFlag[2] = loglevel;
			g_pExitFlag[1] = 2;
			return -1;
		}
		if (strncmp(argv[i],"-md",3) == 0)
		{
			int loglevel ;
			printf("当前日志级别为%d,设置日志级别为 %d\n",g_pExitFlag[3],atoi(argv[i]+3));
			loglevel = atoi(argv[i]+3);
			g_pExitFlag[3] = loglevel;
			g_pExitFlag[1] = 3;
			return -1;
		}
	}
	
	if (strlen(confile)<1)
	{
		sprintf(confile,"%s","bfdreb.xml");
	}
// 	if (g_pExitFlag[0]  == 1)
// 	{
// 		printf("程序正在运行，请先停止!\n");
// 		return -1;
// 	}
// 	else
// 	{
// 		printf("无程序正在运行，开始启动!\n");
// 	}
	if (!isRunFront) 
	{
		if (g_pExitFlag[0]  == 1)
		{
			printf("程序正在运行，请先停止!\n");
			return -1;
		}
		else
		{
			printf("无程序正在运行，开始启动!\n");
		}
	}
	else //在前台运行，不用判断共享内存标志
	{
		printf("开始启动!\n");
	}
	

#ifndef _WINDOWS
	//在unix下在后台运行
	int ret;
	
	signal(SIGTTOU,SIG_IGN); 
	signal(SIGTTIN,SIG_IGN); 
	signal(SIGTSTP,SIG_IGN);
	if (!isRunFront)
	{
		if ( (ret=fork()) != 0) 
		{
			exit(0);
		}
	}
	Signal_proc(SIGHUP, sig_hup); //1  Hangup detected on controlling terminal or death of controlling process
	Signal_proc(SIGINT, sig_int); //2  当键盘按下CTRL+C从shell中发出信号，信号被传递给shell中前台运行的进程，对应该信号的默认操作是中断 (INTERRUPT) 该进程。
	Signal_proc(SIGQUIT,sig_int); //3  当键盘按下CTRL+\从shell中发出信号，信号被传递给shell中前台运行的进程，对应该信号的默认操作是退出 (QUIT) 该进程。
	Signal_proc(SIGILL,sig_int);   //4  Illegal Instruction
	signal(SIGABRT,SIG_IGN);  //6  Abort signal from abort(3)
	signal(SIGFPE,SIG_IGN);   //8 Floating point exception
	Signal_proc(SIGKILL,sig_int); //9  kill进程   Kill signal
	signal(SIGSEGV,SIG_IGN);  // 11 Invalid memory reference
	signal(SIGPIPE,SIG_IGN);  // 13 Broken pipe: write to pipe with no readers
	signal(SIGALRM, SIG_IGN); // 14 起到定时器的作用，通常是程序在一定的时间之后才生成该信号。
	Signal_proc(SIGTERM,sig_int); // 15 Termination signal
	Signal_proc(SIGUSR1, sig_usr1); // 30,10,16 User-defined signal 1
	Signal_proc(SIGUSR2, sig_usr1); // 31,12,17 User-defined signal 2
	signal(SIGCHLD,SIG_IGN);   // 20,17,18  Child stopped or terminated
	Signal_proc(SIGSTOP,sig_int);   // 17,19,23  Stop process
	Signal_proc(SIGBUS, sig_int); // 10,7,10  Bus error (bad memory access)
	signal(SIGCLD,SIG_IGN);  // -,-,18   A synonym for SIGCHLD
#endif	

	//将PID写入到文件中
	strcat(shmname,".pid");
	FILE *fp = fopen(shmname,"w");
	if (fp != NULL)
	{
#ifdef _WINDOWS
		sprintf(shmname,"PID[%d] 共享内存id[%d] 缓冲大小[%ld]",GetPid(),pShare.m_shmid,DREBBUFSIZE);
#else
		sprintf(shmname,"PID[%d] 共享内存id[%d] 文件索引ID[%s] 缓冲大小[%ld]",GetPid(),pShare.m_shmid,pShare.m_szmsg,DREBBUFSIZE);
#endif
		fwrite(shmname,1,strlen(shmname),fp);
		fclose(fp);
	}
	g_pDreb.g_Vars.m_pShmpub = (S_SHM_PUBLIC *)(g_pExitFlag+4);
	if (!g_pDreb.Init(confile))
	{
		return -1;
	}
	g_pExitFlag[0] = 1;
	g_pExitFlag[2] = g_pDreb.g_Vars.g_pLog.GetLogLevel();
	g_pExitFlag[3] = g_pDreb.g_Vars.g_pDataLog.GetLogLevel();
	while (true)
	{
		if (g_pRunFlag == 0)
		{
			printf("------begin exit bfdreb by signal.......\n");
			g_pDreb.g_Vars.g_bIsExit = true;
			g_pDreb.OnStop();
			printf("------exit bfdreb by signal ok !\n");
			SLEEP(100);
			g_pExitFlag[0] =0;
			g_pExitFlag = NULL;
			return 0;
		}
		if (g_pExitFlag[0] != 1)
		{
			printf("------begin exit bfdreb.......\n");
			g_pDreb.g_Vars.g_bIsExit = true;
			g_pDreb.OnStop();
			printf("------exit bfdreb ok !\n");
			SLEEP(100);
			g_pExitFlag = NULL;
			return 0;
		}
		if (g_pExitFlag[1] >0) //日志参数有变动
		{
			if (g_pExitFlag[1] == 2)
			{
				printf("设置日志级别为[%d]\n",g_pExitFlag[2]);
				g_pDreb.g_Vars.g_pLog.SetLogLevel(g_pExitFlag[2]);
			}
			else if (g_pExitFlag[1] == 3)
			{
				printf("设置DATA日志级别为[%d]\n",g_pExitFlag[2]);
				g_pDreb.g_Vars.g_pDataLog.SetLogLevel(g_pExitFlag[3]);
				
			}
			g_pExitFlag[1] =0;
		}
		//监控处理
		g_pDreb.Monitor();
		SLEEP_SECONDS(4);
	}
	return 0;
}
