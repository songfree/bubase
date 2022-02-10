// main.cpp : Defines the entry point for the console application.
//



#include "ShareMemory.h"
#include "TradeMain.h"
#include "BF_Md5.h"


#ifdef _WINDOWS
#ifdef _DEBUG
#pragma comment(lib, "bf_kerneld.lib") 
#pragma comment(lib, "bf_xdpd.lib") 
#pragma comment(lib, "bf_drebapid.lib") 
#pragma comment(lib, "bf_dbapid.lib") 
#pragma comment(lib, "bf_dbpubd.lib") 
#pragma message("Automatically linking with   bf_kerneld.lib bf_xdpd.lib bf_drebapid.lib bf_dbapid.lib bf_dbpubd.lib")
#else
#pragma comment(lib, "bf_kernel.lib") 
#pragma comment(lib, "bf_xdp.lib") 
#pragma comment(lib, "bf_drebapi.lib") 
#pragma comment(lib, "bf_dbapi.lib") 
#pragma comment(lib, "bf_dbpub.lib") 
#pragma message("Automatically linking with   bf_kernel.lib bf_xdp.lib bf_drebapi.lib bf_dbapi.lib bf_dbpub.lib")
#endif

#endif



char *g_pExitFlag=NULL;
int   g_pRunFlag= 1;

CTradeMain g_pKernelMain;


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
			g_pRunFlag = 0;
			break;
		case SIGQUIT: //3  �����̰���CTRL+\��shell�з����źţ��źű����ݸ�shell��ǰ̨���еĽ��̣���Ӧ���źŵ�Ĭ�ϲ������˳� (QUIT) �ý��̡�
			printf("SIGQUIT�ź�%d\n",signo);
			g_pRunFlag = 0;
			break;
		case SIGILL: //4  Illegal Instruction
			printf("SIGILL�ź�%d\n",signo);
			g_pRunFlag = 0;
			break;
		case SIGABRT: //6  Abort signal from abort(3)
			printf("SIGABRT�ź�%d\n",signo);
			break;
		case SIGFPE://8 Floating point exception
			printf("SIGFPE�ź�%d\n",signo);
			break;
		case SIGKILL: //9  kill����   Kill signal
			printf("SIGKILL�ź�%d\n",signo);
			g_pRunFlag = 0;
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
			g_pRunFlag = 0;
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
// 		g_pRunFlag = 0;
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
	memset(confile,0,sizeof(confile));
	CShareMemory pShare;
	if (argc == 1)
	{
#ifdef _ENGLISH_
		printf("command desc:\n");
		printf("               non parameter parafile is tradekernel.xml \n");
		printf("               stop/STOP  exit tradekernel \n");
		printf("               -cf  run front \n");
		printf("               -cb  run background , default background \n");
		printf("               -m   set sharememory para \n");
		printf("               -ml  set loglevel   -ml5 set loglevel is 5-debug \n");
		printf("               -md  set dataloglevel   -md5 set datalogleve is 5-debug \n");
		printf("               -mq  exit  \n");
		printf("               -pwd password tools  \n");
		printf("               -f   set parafile  -ftmc.xml the parafile is mc.xml , default file is tradekernel.xml \n");
		printf("               -q   exit \n");
#else
		printf("�����в���˵��:\n");
		printf("               �������� ��̨���� �����ļ�Ϊtradekernel.xml \n");
		printf("               stop/STOP  �˳�tradekernel \n");
		printf("               -cf  ǰ̨���� \n");
		printf("               -cb  ��̨���� ��-cʱĬ��Ϊ��̨���� \n");
		printf("               -m   ���ù����ڴ���� \n");
		printf("               -ml  ������־����   -ml5������־����Ϊ5�������� \n");
		printf("               -md  ����DATA��־����   -md5����DATA��־����Ϊ5�������� \n");
		printf("               -mq  �˳��������еĳ��� \n");
		printf("               -pwd �������룬�õ����� \n");
		printf("               -f   ���������ļ� -ftmc.xml�����ļ�Ϊtmc.xml  Ĭ�������ļ�Ϊtradekernel.xml \n");
		printf("               -q   �˳� \n");
#endif
	}
	
	CBF_Tools::GetModuleNamePath(argv,g_pKernelMain.m_pRes.g_sCurPath,g_pKernelMain.m_pRes.g_sModulePath,g_pKernelMain.m_pRes.g_sModuleName);
	
	
#ifdef _WINDOWS
	CBF_Md5 md5class;
	sprintf(shmname,"%s\\tradekernel",g_pKernelMain.m_pRes.g_sCurPath);
	md5class.Update(shmname);
	sprintf(shmname,"tradekernel_%s",md5class.ToString().c_str());
#else
	sprintf(shmname,"%s/tradekernel",g_pKernelMain.m_pRes.g_sCurPath);
#endif
	


	g_pExitFlag = (char *)pShare.Open(shmname,5+sizeof(S_SHM_PUBLIC));
	if (g_pExitFlag == NULL)
	{
		printf("�򿪹����ڴ�� %s\n",pShare.m_szmsg);
		return -1;
	}
#ifdef _ENGLISH_
#ifdef _WINDOWS
	printf("sharememory id[%d] runflag[%d] \n",pShare.m_shmid,g_pExitFlag[0]);
#else
	printf("sharememory id[%d] fileindex[%s] runflag[%d] \n",pShare.m_shmid,pShare.m_szmsg,g_pExitFlag[0]);
#endif
#else
#ifdef _WINDOWS
	printf("�����ڴ�id[%d] ���б�־[%d] \n",pShare.m_shmid,g_pExitFlag[0]);
#else
	printf("�����ڴ�id[%d] �ļ�����ID[%s] ���б�־[%d] \n",pShare.m_shmid,pShare.m_szmsg,g_pExitFlag[0]);
#endif
#endif	
	if (argc == 2 ) //����Ƿ�Ϊstop������֮ǰ�����
	{
		char para1[500];
		memset(para1,0,sizeof(para1));
		strcpy(para1,argv[1]);
		if (strcmp(para1,"stop") == 0 || strcmp(para1,"STOP") == 0 )
		{

			if (g_pExitFlag[0] == 1)
			{
#ifdef _ENGLISH_
				printf("exiting tradekernel...\n");
#else
				printf("�˳��������е�tradekernel...\n");
#endif
				g_pExitFlag[0] = 0;
			}
			else
			{
#ifdef _ENGLISH_
				printf("tradekernel exited\n");
#else
				printf("tradekernel���˳�\n");
#endif
				
				g_pExitFlag[0] = 0;
			}

			return -1;
		}
	}
	//������
	int i;
	bool isRunFront=false; //�Ƿ�ǰ̨����
	
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
#ifdef _ENGLISH_
				printf("exiting tradekernel...\n");
#else
				printf("�˳��������е�tradekernel...\n");
#endif
				g_pExitFlag[0] = 0;
			}
			else
			{
#ifdef _ENGLISH_
				printf("tradekernel exited...\n");
#else
				printf("tradekernel���˳�\n");
#endif
				
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
#ifdef _ENGLISH_
			printf("current loglevel is %d,set loglevel is %s \n",g_pExitFlag[2],atoi(argv[i]+3));
#else
			printf("��ǰ��־����Ϊ%d,������־����Ϊ %d\n",g_pExitFlag[2],atoi(argv[i]+3));
#endif
			loglevel = atoi(argv[i]+3);
			g_pExitFlag[2] = loglevel;
			g_pExitFlag[1] = 2;
			return -1;
		}
		if (strncmp(argv[i],"-md",3) == 0)
		{
			int loglevel ;
#ifdef _ENGLISH_
			printf("current dataloglevel is %d,set dataloglevel is %s \n",g_pExitFlag[2],atoi(argv[i]+3));
#else
			printf("��ǰDATA��־����Ϊ%d,����DATA��־����Ϊ %d\n",g_pExitFlag[3],atoi(argv[i]+3));
#endif
			
			loglevel = atoi(argv[i]+3);
			g_pExitFlag[3] = loglevel;
			g_pExitFlag[1] = 3;
			return -1;
		}
		if (strncmp(argv[i],"-pwd",4) == 0)
		{
			char pwd[100];
			char pwd2[100];
			int len = CBF_Tools::GetConsoleEnter("enter password:",32,pwd,true);
			pwd[len]=0;
			int len2 = CBF_Tools::GetConsoleEnter("enter password again��",32,pwd2,true);
			pwd2[len2]=0;
			if (len != len2 || strncmp(pwd,pwd2,len) != 0)
			{
#ifdef _ENGLISH_
				printf("password illegal \n");
#else
				printf("�����������벻һ�� \n");
#endif
				return -1;
			}
			g_pKernelMain.m_pRes.m_pDes.Encryptchar(pwd,len,DBPWDKEY);
#ifdef _ENGLISH_
			printf("exchange des password [%s] \n",pwd);
#else
			printf("����������des����[%s]\n",pwd);
#endif
			len = 100;
			g_pKernelMain.m_pRes.m_p3Des.Des3Buffer(pwd2,len,DBPWD3DESKEY,pwd);
#ifdef _ENGLISH_
			printf("exchange 3des password [%s]\n",pwd2);
#else
			printf("����������3des����[%s]\n",pwd2);
#endif
			return -1;
		}
	}

	if (strlen(confile)<1)
	{
		sprintf(confile,"%s","tradekernel.xml");
	}
	if (!isRunFront) 
	{
		if (g_pExitFlag[0]  == 1)
		{
#ifdef _ENGLISH_
			printf("program is running ,please stop and start again !\n");
#else
			printf("�����������У�����ֹͣ!\n");
#endif
			return -1;
		}
		else
		{
#ifdef _ENGLISH_
			printf("no program running , begin start !\n");
#else
			printf("�޳����������У���ʼ����!\n");
#endif
			
		}
	}
	else //��ǰ̨���У������жϹ����ڴ��־
	{
#ifdef _ENGLISH_
		printf("begin start !\n");
#else
		printf("��ʼ����!\n");
#endif
		
	}
	

	bool bisMonitor = false;
	
	
#ifndef _WINDOWS
	//��unix���ں�̨����
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
		CBF_Date_Time pdate;
#ifdef _WINDOWS
		sprintf(shmname,"PID[%d] sharememory id[%d] starttime[%s]",GetPid(),pShare.m_shmid,pdate.ToStringDT17().c_str());
#else
		sprintf(shmname,"PID[%d] sharememory id[%d] fileindex[%s] starttime[%s]",GetPid(),pShare.m_shmid,pShare.m_szmsg,pdate.ToStringDT17().c_str());
#endif
		fwrite(shmname,1,strlen(shmname),fp);
		fclose(fp);
	}
	
	g_pKernelMain.m_pRes.m_pShmpub = (S_SHM_PUBLIC *)(g_pExitFlag+4);
	
	if (!g_pKernelMain.Init(confile))
	{
		SLEEP_SECONDS(2);
		return -1;
	}
	if (!g_pKernelMain.Start())
	{
		SLEEP_SECONDS(2);
		return -1;
	}
#ifdef _ENGLISH_
	printf("start success !\n");
#else
	printf("����tradekernel�ɹ� \n");
#endif
	
	
	g_pExitFlag[0] = 1;
	g_pExitFlag[2] = ((CBF_LogClient *)g_pKernelMain.m_pLog)->GetLogLevel();
	g_pExitFlag[3] = g_pKernelMain.m_pDrebApi.GetDataLogLevel();
	while (true)
	{
		if (g_pRunFlag == 0)
		{
			printf("------begin exit tradekernel by signal.......\n");
			g_pKernelMain.m_pRes.g_bToExit = true;
			fprintf(stderr,"begin exit tradekernel ");
			g_pKernelMain.Stop();
			printf("------exit tradekernel by signal ok !\n");
			fprintf(stderr,"exit tradekernel ok!");
			//�ȴ�5�룬��ֹ�����ػ��������죬���е���һ̨���������ٲ��л����л�����һ̨������
			SLEEP_SECONDS(5);
			g_pExitFlag[0] = 0;
			g_pExitFlag = NULL;
#ifdef _WINDOWS
			exit(0);
#endif
			return 0;
		}
		if (g_pExitFlag[0] != 1)
		{
			printf("------begin exit tradekernel.......\n");
			g_pKernelMain.m_pRes.g_bToExit = true;
			//g_pOfferingMain.m_pRes.WriteBack();
			fprintf(stderr,"begin exit tradekernel ");
			g_pKernelMain.Stop();
			printf("------exit tradekernel ok !\n");
			fprintf(stderr,"exit tradekernel ok!");
			//�ȴ�5�룬��ֹ�����ػ��������죬���е���һ̨���������ٲ��л����л�����һ̨������
			SLEEP_SECONDS(5);
			g_pExitFlag = NULL;
			return 0;
		}
		if (g_pExitFlag[1] >0) //��־�����б䶯
		{
			if (g_pExitFlag[1] == 2)
			{
				((CBF_LogClient *)g_pKernelMain.m_pLog)->SetLogLevel(g_pExitFlag[2]);
			}
			else if (g_pExitFlag[1] == 3)
			{
				g_pKernelMain.m_pDrebApi.SetDataLogLevel(g_pExitFlag[3]);
			}
			g_pExitFlag[1] =0;
		}
		//��ش����߳�
		SLEEP_SECONDS(5);
		g_pKernelMain.Monitor();
		
		
	}
	return 0;
}

