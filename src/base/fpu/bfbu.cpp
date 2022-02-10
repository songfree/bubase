
#include "public.h"
#include "ShareMemory.h"
#include "BuLink.h"

int g_pRunFlag = 1;



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
	//��һ�鹲���ڴ�
	char shmname[200];
	char confile[200];
	memset(shmname,0,sizeof(shmname));
	memset(confile,0,sizeof(confile));
	CShareMemory pShare;
	char *pExitFlag=NULL;
	CBuLink frun;
	CBF_Tools::GetModuleNamePath(argv,frun.g_sCurPath,frun.g_sModulePath,frun.g_sModuleName);
// 	if (argc == 1)
// 	{
// 		printf("�����в���˵��:\n");
// 		printf("               �������� �����ļ�Ϊbffpu.xml \n");
// 		printf("               stop/STOP  �˳�bffpu \n");
// 		printf("               -m   ���ù����ڴ���� \n");
// 		printf("               -ml  ������־����   -ml5������־����Ϊ5�������� \n");
// 		printf("               -mq  �˳��������еĳ��� \n");
// 		printf("               -f   ���������ļ� -ftmc.xml�����ļ�Ϊtmc.xml  Ĭ�������ļ�Ϊbffpu.xml \n");
// 		printf("               -q   �˳� \n");
// 	}
	
#ifdef _WINDOWS
	CBF_Md5 md5class;
	sprintf(shmname,"%s\\BFFPU",frun.g_sCurPath);
	md5class.Update(shmname);
	strcpy(shmname,md5class.ToString().c_str());
#else
	sprintf(shmname,"%s/BFFPU",frun.g_sCurPath);
#endif
	


	pExitFlag = (char *)pShare.Open(shmname,4);
	if (pExitFlag == NULL)
	{
		printf("�򿪹����ڴ��\n");
		return -1;
	}
#ifdef _WINDOWS
	printf("�����ڴ�id[%d] �����С[%ld]\n",pShare.m_shmid,BFBUFSIZE);
#else
	printf("�����ڴ�id[%d] �ļ�����ID[%s] �����С[%ld]\n",pShare.m_shmid,pShare.m_szmsg,BFBUFSIZE);
#endif
	
	if (argc == 2 ) //����Ƿ�Ϊstop������֮ǰ�����
	{
		char para1[500];
		memset(para1,0,sizeof(para1));
		strcpy(para1,argv[1]);
		if (strcmp(para1,"stop") == 0 || strcmp(para1,"STOP") == 0 )
		{
			if (pExitFlag[0] == 1)
			{
				printf("�˳��������е�bffpu...\n");
				pExitFlag[0] = 0;
			}
			else
			{
				printf("bffpu���˳�\n");
				pExitFlag[0] = 0;
			}
			return -1;
		}
	}
	//������
	int i;
	
	for (i=1; i< argc ; i++)
	{
		
		if (strncmp(argv[i],"-mq",3) == 0 || strncmp(argv[i],"-q",2) == 0)
		{
			if (pExitFlag[0]  == 1)
			{
				printf("�˳��������е�bffpu...\n");
				pExitFlag[0] = 0;
			}
			else
			{
				printf("bffpu���˳�\n");
				pExitFlag[0] = 0;
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
			printf("��ǰ��־����Ϊ%d,������־����Ϊ %d\n",pExitFlag[2],atoi(argv[i]+3));
			loglevel = atoi(argv[i]+3);
			pExitFlag[2] = loglevel;
			pExitFlag[1] = 2;
			return -1;
		}
	}
	if (strlen(confile)<1)
	{
		sprintf(confile,"%s","bffpu.xml");
	}
	
	
	
	int ret;
#ifndef _WINDOWS
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
		
	if (!frun.InitLink(confile))
	{
		
		return -1;
	}
	printf("����bffpu�ɹ� %s %d\n",frun.m_pRes.g_sBccIp.c_str(),frun.m_pRes.g_nPort);
	pExitFlag[0] = 1;
	pExitFlag[2] = frun.m_pLog->GetLogLevel();
	while (true)
	{
		if (pExitFlag[0] != 1)
		{
			printf("------begin exit bffpu.......\n");
			frun.m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"��ʼ�˳�bffpu");
			frun.m_pRes.g_bToExit = true;
			frun.Stop();
			printf("------exit bffpu ok !\n");
			frun.m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ɹ��˳�bffpu");
			SLEEP(100);
			pExitFlag = NULL;
			return 0;
		}
		if (pExitFlag[1] >0) //��־�����б䶯
		{
			if (pExitFlag[1] == 2)
			{
				frun.m_pLog->SetLogLevel(pExitFlag[2]);
			}
			pExitFlag[1] =0;
		}
		if (g_pRunFlag == 0) //�ź��˳�
		{
			frun.m_pLog->LogMp(LOG_PROMPT,__FILE__,__LINE__,"�˳��ź��˳�");
			frun.Stop();
			return -1;
		}
		ret = frun.Run();
		if (ret != 0)
		{
			printf("------begin exit bffpu.......\n");
			frun.m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"��ʼ�˳�bffpu");
			frun.m_pRes.g_bToExit = true;
			frun.Stop();
			printf("------exit bffpu ok !\n");
			frun.m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"�ɹ��˳�bffpu");
			SLEEP(100);
			pExitFlag = NULL;
			return 0;
		}
	}

	return 0;
}
