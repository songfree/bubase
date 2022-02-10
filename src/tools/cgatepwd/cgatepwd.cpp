// changedbpwd.cpp : Defines the entry point for the console application.
//
#include "public.h"
#include "BF_Xml.h"
#include "BF_Date_Time.h"
#include <iostream>
#include "BF_Des.h"
#ifndef WIN32
#include   <curses.h>  
#include   <stdio.h>   
#include <stdio.h>
#include <sys/ioctl.h>
#include <termios.h>
int mygetch(void) 
{ 
	struct termios tm, tm_old; 
	int fd = STDIN_FILENO, c; 
	if(tcgetattr(fd, &tm) < 0) 
		return -1; 
	memcpy(&tm_old,&tm,sizeof(tm));
	tm.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	tm.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP |IXON);
	tm.c_cflag &= ~(CSIZE | PARENB);
	tm.c_cflag |= CS8;
	tm.c_oflag &= ~(OPOST);
	tm.c_cc[VMIN] = 1;
	tm.c_cc[VTIME] = 0;
	if(tcsetattr(fd, TCSANOW, &tm) < 0)
		return -1; 
	c = fgetc(stdin); 
	if(tcsetattr(fd, TCSANOW, &tm_old) < 0) 
		return -1; 
	return c; 
} 
#define GETCH(p) mygetch(p) 
#else
#define GETCH(p) _getch(p)
#endif


#define  DBPWDKEY  "rd402wms"

int GetTermCmd(const char *prommpt,int commandlen,char *commnad,bool isPwd)
{
	int count = 0;
	char ch;
	printf("\n>%s:",prommpt);
	while ((ch=GETCH())!=13)
	{
		commnad[count]=ch;
		if (ch == 8) //backspace
		{
			if (count >0)
			{
				count--;
				printf("%c",ch);
				printf("%c",' ');
				printf("%c",ch);
			}
			continue;
		}
		count++;
		if (isPwd)
		{
			printf("*");
		}
		else
		{
			printf("%c",ch);
		}
		if (commandlen >0 && count >= commandlen)
		{
			break;
		}
	} 
	commnad[count]=0;
	return count;
	
}


int main(int argc, char* argv[])
{
	char password[500];
	char pwd[500];
	char pwdflag[20];
	memset(password,0,sizeof(password));
	memset(pwd,0,sizeof(pwd));
	memset(pwdflag,0,sizeof(pwdflag));

	if (argc <1) //修改指定文件
	{
		printf("请在命令行输入配置文件\n");
		return -1;
	}	
	CBF_Xml xml;
	
	if (!xml.FromFile(argv[1]))
	{
		printf("传入文件非xml\n");
		return -1;
	}
	bool isgate = false;
	
	CXmlNode *certnode =  xml.GetNodeByPath("/package/head/pki");
	if (certnode == NULL)
	{
		printf("传入文件xml格式非法，无[/package/head/pki]节点\n");
		isgate = true;
		certnode =  xml.GetNodeByPath("/package/head/pki");
		if (certnode == NULL)
		{
			printf("传入文件xml格式非法，无[/package/head/pki]节点\n");
			return -1;
		}
		
	}


looppwd:
	GetTermCmd("请输入证书密码",30,password,true);
	GetTermCmd("请再次输入证书密码",30,pwd,true);

	if (strcmp(pwd,password)!=0)
	{
		printf("\n两次输入密码不一致\n");
		goto looppwd;
	}

//	GetTermCmd("请选择密码是否加密1加密其它不加密",1,pwdflag,false);
	printf("\n");
	
	CBF_Des myencrypt;
	int len = strlen(pwd);
	myencrypt.EncryptString(password,len,DBPWDKEY);
	password[len]=0;

	certnode->SetAttribute("certpwd",false,password);
	certnode->SetAttribute("certpwdflag",false,"1");

	if (!xml.ToFile(argv[1]))
	{
		printf("更改XML配置失败，请确定文件是否只读\n");
		return -1;
	}
	printf("更改XML[%s]配置成功\n",argv[1]);
	return 0;
}
