
#include "public.h"
#include "ShareMemory.h"
#include "BPCLink.h"
#include "BF_Md5.h"

#ifdef _WINDOWS
#ifdef _DEBUG

#pragma comment(lib, "bf_bumoduled.lib") 
#pragma message("Automatically linking with  bf_bumoduled.lib")
#else
#pragma comment(lib, "bf_bumodule.lib") 
#pragma message("Automatically linking with  bf_bumodule.lib")
#endif
#endif

int  g_pRunFlag = 1;


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
			g_pRunFlag = 0;
			break;
		case SIGILL: //4  Illegal Instruction
			printf("SIGILL信号%d\n",signo);
			g_pRunFlag = 0;
			break;
		case SIGABRT: //6  Abort signal from abort(3)
			printf("SIGABRT信号%d\n",signo);
			break;
		case SIGFPE://8 Floating point exception
			printf("SIGFPE信号%d\n",signo);
			break;
		case SIGKILL: //9  kill进程   Kill signal
			printf("SIGKILL信号%d\n",signo);
			g_pRunFlag = 0;
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
			g_pRunFlag = 0;
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



int main(int argc, char *argv[])
{

	//打开一块共享内存
	char shmname[200];
	char confile[200];
	memset(shmname,0,sizeof(shmname));
	memset(confile,0,sizeof(confile));
	CShareMemory pShare;
	
	char *g_pAllBpuExitFlag=NULL;
	
// 	if (argc == 1)
// 	{
// 		printf("命令行参数说明:\n");
// 		printf("               不带参数 配置文件为bfbpu.xml \n");
// 		printf("               stop/STOP  退出bfbpu \n");
// 		printf("               -m   设置共享内存参数 \n");
// 		printf("               -ml  设置日志级别   -ml5设置日志级别为5，即调试 \n");
// 		printf("               -mq  退出正在运行的程序 \n");
// 		printf("               -f   设置配置文件 -ftmc.xml配置文件为tmc.xml  默认配置文件为bfbpu.xml \n");
// 		printf("               -q   退出 \n");
// 	}

	CBPCLink frun;

	CBF_Tools::GetModuleNamePath(argv,frun.g_sCurPath,frun.g_sModulePath,frun.g_sModuleName);

#ifdef _WINDOWS
	CBF_Md5 md5class;
	sprintf(shmname,"%s\\bfspu",frun.g_sCurPath);
	md5class.Update(shmname);
	sprintf(shmname,"bfspu_%s",md5class.ToString().c_str());
#else
	sprintf(shmname,"%s/bfspu",frun.g_sCurPath);
#endif
	
	
	g_pAllBpuExitFlag = (char *)pShare.Open(shmname,4);
	if (g_pAllBpuExitFlag == NULL)
	{
		printf("打开共享内存错\n");
		return -1;
	}
#ifdef _WINDOWS
	printf("共享内存id[%d] 缓冲大小[%ld]\n",pShare.m_shmid,BPCBUFSIZE);
#else
	printf("共享内存id[%d] 文件索引ID[%s] 缓冲大小[%ld]\n",pShare.m_shmid,pShare.m_szmsg,BPCBUFSIZE);
#endif
	
	if (argc == 2 ) //检查是否为stop，兼容之前的情况
	{
		char para1[500];
		memset(para1,0,sizeof(para1));
		strcpy(para1,argv[1]);
		if (strcmp(para1,"stop") == 0 || strcmp(para1,"STOP") == 0 )
		{
			if (g_pAllBpuExitFlag[0] == 1)
			{
				printf("退出正在运行的bfspu...\n");
				g_pAllBpuExitFlag[0] = 0;
			}
			else
			{
				printf("bfspu已退出\n");
				g_pAllBpuExitFlag[0] = 0;
			}
			return -1;
		}
	}
	//检查参数
	int i;

	for (i=1; i< argc ; i++)
	{
		
		if (strncmp(argv[i],"-mq",3) == 0 || strncmp(argv[i],"-q",2) == 0)
		{
			if (g_pAllBpuExitFlag[0]  == 1)
			{
				printf("退出正在运行的bfspu...\n");
				g_pAllBpuExitFlag[0] = 0;
			}
			else
			{
				printf("bfspu已退出\n");
				g_pAllBpuExitFlag[0] = 0;
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
			printf("当前日志级别为%d,设置日志级别为 %d\n",g_pAllBpuExitFlag[2],atoi(argv[i]+3));
			loglevel = atoi(argv[i]+3);
			g_pAllBpuExitFlag[2] = loglevel;
			g_pAllBpuExitFlag[1] = 2;
			return -1;
		}
	}
	if (strlen(confile)<1)
	{
		sprintf(confile,"%s","bfspu.xml");
	}
	
	int ret;
#ifndef _WINDOWS
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
	
	if (!frun.InitLink(confile))
	{
		return -1;
	}
	printf("启动bfspu成功 %s %d\n",frun.GetBpcIp().c_str(),frun.GetBpcPort());
	g_pAllBpuExitFlag[0] = 1;
	g_pAllBpuExitFlag[2] = frun.m_pLog->GetLogLevel();
	while (true)
	{
		if (g_pAllBpuExitFlag[0] != 1)
		{
			printf("------exit bfspu ok !\n");
			g_pAllBpuExitFlag = NULL;
			return 0;
		}
		if (g_pAllBpuExitFlag[1] >0) //日志参数有变动
		{
			if (g_pAllBpuExitFlag[1] == 2)
			{
				frun.m_pLog->SetLogLevel(g_pAllBpuExitFlag[2]);
			}
			g_pAllBpuExitFlag[1] =0;
		}
		if (g_pRunFlag == 0) //信号退出
		{
			frun.m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"退出信号退出");
			frun.Stop();
			return -1;
		}
		ret = frun.Run();
		if (ret !=0 )
		{
			frun.m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"连接出错，退出SPU");
			return -1;
		}
		
	}
	return 0;
}
