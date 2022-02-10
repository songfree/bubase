// changedbpwd.cpp : Defines the entry point for the console application.
//
#include "public.h"
#include "XmlPack.h"
#include "BF_Date_Time.h"
#include <iostream>
#include "BF_3Des.h"
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

int modidbconfig(CXmlPack *xmlpack,const char *dbname,const char *dbuser,const char *pwd,const char *pwdflag,const char *dbtype)
{
	bool iswrite = false;
	int ret;
	std::string ss;
	CNode *pNode =NULL;
	CNode *pChildNode =NULL;
	if (xmlpack->gotoPack()<0)
	{
		return -1;
	}
	//�޸�������pack������ݿ�����  package->pack
	if (xmlpack->gotoPack() == 0)
	{
		ret = xmlpack->getPackValue("passwdtype",ss);
		if (ret == 0)
		{
			if (pwdflag[0] != '0') //����Ҫ���м���
			{
				xmlpack->setPackNameValue("passwdtype",pwdflag);
				iswrite = true;
			}
			else
			{
				xmlpack->setPackNameValue("passwdtype","0");
				iswrite = true;
			}
		}
		xmlpack->gotoPack();
		ret = xmlpack->getPackValue("userpasswd",ss);
		if (ret == 0)
		{
			xmlpack->setPackNameValue("userpasswd",pwd);
			iswrite = true;
		}
		ret = xmlpack->getPackValue("databaseserver",ss);
		if (ret == 0)
		{
			xmlpack->setPackNameValue("databaseserver",dbname);
			iswrite = true;
		}
		ret = xmlpack->getPackValue("databaseuser",ss);
		if (ret == 0)
		{
			xmlpack->setPackNameValue("databaseuser",dbuser);
			iswrite = true;
		}
		ret = xmlpack->getPackValue("databasetype",ss);
		if (ret == 0)
		{
			xmlpack->setPackNameValue("databasetype",dbtype);
			iswrite = true;
		}
	}
	else
	{
	    printf("xml�ļ�����package/head��������\n");
	    return -1;
	}

	//�޸�������DATABASE������ݿ����� package->DATABASE
	if (xmlpack->gotoPack() == 0)
	{
//		xmlpack->m_xml.getParentItem();
		pNode = xmlpack->m_xml.getFirstChildItem("DATABASE");
		if (pNode != NULL)
		{
			pChildNode = xmlpack->m_xml.getFirstChildItem("passwdtype");
			if (pChildNode != NULL)
			{
				if (pwdflag[0] != '0') //����Ҫ���м���
				{
					pChildNode->setNodeValue(pwdflag);
					iswrite = true;
				}
				else
				{
					pChildNode->setNodeValue("0");
					iswrite = true;
				}
			}
			xmlpack->m_xml.setCurrentNode(pNode);
			pChildNode = xmlpack->m_xml.getFirstChildItem("userpasswd");
			if (pChildNode != NULL)
			{
				pChildNode->setNodeValue(pwd);
				iswrite = true;
			}
			xmlpack->m_xml.setCurrentNode(pNode);
			pChildNode = xmlpack->m_xml.getFirstChildItem("databaseserver");
			if (pChildNode != NULL)
			{
				pChildNode->setNodeValue(dbname);
				iswrite = true;
			}
			xmlpack->m_xml.setCurrentNode(pNode);
			pChildNode = xmlpack->m_xml.getFirstChildItem("databaseuser");
			if (pChildNode != NULL)
			{
				pChildNode->setNodeValue(dbuser);
				iswrite = true;
			}
			xmlpack->m_xml.setCurrentNode(pNode);
			pChildNode = xmlpack->m_xml.getFirstChildItem("dbtype");
			if (pChildNode != NULL)
			{
				pChildNode->setNodeValue(dbtype);
				iswrite = true;
			}
			xmlpack->m_xml.setCurrentNode(pNode);
			pChildNode = xmlpack->m_xml.getFirstChildItem("databasetype");
			if (pChildNode != NULL)
			{
				pChildNode->setNodeValue(dbtype);
				iswrite = true;
			}
			printf("xml������package/pack/DATABASE\n");
		}
		else
		{
		    printf("xml���ò���package/pack/DATABASE ����������  \n");
	    }    
	}
	else
	{
	    printf("xml�ļ�����package/head��������\n");
	    return -1;
	}

	//�޸�������package->head->ҵ������->���ݿ�����
	if (xmlpack->gotoHead() == 0)
	{
		pNode = xmlpack->m_xml.getFirstChildItem("ҵ������");	
		if (pNode != NULL)
		{
			pNode = xmlpack->m_xml.getFirstChildItem("���ݿ�����");
			if (pNode != NULL)
			{
				pChildNode = xmlpack->m_xml.getFirstChildItem("passwdtype");
				if (pChildNode != NULL)
				{
					if (pwdflag[0] != '0') //����Ҫ���м���
					{
						pChildNode->setNodeValue(pwdflag);
						iswrite = true;
					}
					else
					{
						pChildNode->setNodeValue("0");
						iswrite = true;
					}
				}
				xmlpack->m_xml.setCurrentNode(pNode);
				pChildNode = xmlpack->m_xml.getFirstChildItem("userpasswd");
				if (pChildNode != NULL)
				{
					pChildNode->setNodeValue(pwd);
					iswrite = true;
				}
				xmlpack->m_xml.setCurrentNode(pNode);
				pChildNode = xmlpack->m_xml.getFirstChildItem("databaseserver");
				if (pChildNode != NULL)
				{
					pChildNode->setNodeValue(dbname);
					iswrite = true;
				}
				xmlpack->m_xml.setCurrentNode(pNode);
				pChildNode = xmlpack->m_xml.getFirstChildItem("databaseuser");
				if (pChildNode != NULL)
				{
					pChildNode->setNodeValue(dbuser);
					iswrite = true;
				}
				xmlpack->m_xml.setCurrentNode(pNode);
				pChildNode = xmlpack->m_xml.getFirstChildItem("dbtype");
				if (pChildNode != NULL)
				{
					pChildNode->setNodeValue(dbtype);
					iswrite = true;
				}
				xmlpack->m_xml.setCurrentNode(pNode);
			    pChildNode = xmlpack->m_xml.getFirstChildItem("databasetype");
			    if (pChildNode != NULL)
			    {
				    pChildNode->setNodeValue(dbtype);
				    iswrite = true;
			    }
			}
		}
	}
	if (iswrite)
	{
		return 0;
	}
	else
	{
		return 1;
	}
	
}
int modifile(const char *filename,const char *dbname,const char *dbuser,const char *pwd,const char *pwdflag,const char *dbtype)
{
	int ret;
	CXmlPack xmlpack;
	char encrypwd[300];
	CBF_Des myencrypt;
	CBF_3Des p3des;
	char pwdkey[9];
	strcpy(pwdkey,"rd402wms");
	char p3deskey[25];
	strcpy(p3deskey,"rd402wms20092017Ylink123");
	memset(encrypwd,0,sizeof(encrypwd));
	if (xmlpack.fromFile((char *)filename) < 0)
	{
		printf("�ļ�[%s]��xml�ļ��򲻴���\n",filename);
		return -1;
	}
	strcpy(encrypwd,pwd);	
	if (pwdflag[0] != '0') //����Ҫ���м���
	{
		int len = strlen(encrypwd);
		if (pwdflag[0] =='2')
		{
			char errmsg[200];
			len = 300;
			p3des.Des3Buffer(encrypwd,len,p3deskey,errmsg);
		}
		else
		{
			myencrypt.EncryptString(encrypwd,len,pwdkey);
			encrypwd[len]=0;
		}
		
	}
	ret = modidbconfig(&xmlpack,dbname,dbuser,encrypwd,pwdflag,dbtype);
	if (ret == 0)
	{
		UNLINK(filename);
		xmlpack.toFile((char *)filename);
		return 0;
	}
	return ret;
}


int modixml(const char *path_mark,const char *dbname,const char *dbuser,const char *pwd,const char *pwdflag,const char *dbtype,std::vector<string> *chgfile)
{
	if (strlen(path_mark)<1) return -1;

	CXmlPack xmlpack;
    int nfs=0;
	string ss;
	bool iswrite = false;
	CNode *tmpnode = NULL;
	int ret;
    char szfile[1024];
	char encrypwd[300];
	memset(encrypwd,0,sizeof(encrypwd));
	sprintf(szfile,"%s/*.%s",path_mark,"xml");
	CBF_Des myencrypt;
	char pwdkey[9];
	strcpy(pwdkey,"rd402wms");
#ifdef WIN32
	struct _finddata_t fd;
	int dh;
	dh = _findfirst(szfile,&fd);
	if (dh==-1) 
		return(0);
	do
	{
		iswrite = false;
		sprintf(szfile,"%s/%s",path_mark,fd.name);
		ret = modifile(szfile,dbname,dbuser,pwd,pwdflag,dbtype);
		if (ret == 0)
		{
			chgfile->push_back(szfile);
		}
	}while (_findnext(dh,&fd)!=-1);
	_findclose(dh);
#else
	DIR *dp;
	struct dirent *dirp;
	struct stat fst;
	//printf("��ʼ�޸�111111\n");
	dp = opendir(path_mark);
	if (dp==NULL) 
		return(0);
	while ((dirp=readdir(dp))!=NULL)
	{
		//printf("��ʼ�޸�111111\n");
		iswrite = false;
		sprintf(szfile,"%s/%s",path_mark,dirp->d_name);
		if ( stat(szfile,&fst)==0 )
		{
			if (strstr(dirp->d_name,"xml")!=NULL)
			{
				ret = modifile(szfile,dbname,dbuser,pwd,pwdflag,dbtype);
				if (ret == 0)
				{
					chgfile->push_back(szfile);
				}
			}
		}
	}	
	closedir(dp);
#endif
	return(nfs);
}

int main(int argc, char* argv[])
{
	char dbname[500];
	char dbtype[500];
	char dbuser[500];
	char password[500];
	char pwd[500];
	char pwdflag[20];
	memset(dbname,0,sizeof(dbname));
	memset(dbtype,0,sizeof(dbtype));
	memset(dbuser,0,sizeof(dbuser));

	
	memset(password,0,sizeof(password));
	memset(pwd,0,sizeof(pwd));
	memset(pwdflag,0,sizeof(pwdflag));

    printf("�޸����ݿ�����xml�ļ�    \n     ��������Ϊ�޸�$HOME/etc�����е�xml�ļ���\n     ��������Ϊ�޸Ĳ�����Ӧ��xml�ļ�\n\n");
	GetTermCmd("���������ݿ�����",20,dbname,false);
	GetTermCmd("���������ݿ��û�",30,dbuser,false);
looppwd:
	GetTermCmd("���������ݿ��û�������",30,password,true);
	GetTermCmd("���ٴ��������ݿ��û�������",30,pwd,true);

	if (strcmp(pwd,password)!=0)
	{
		printf("\n�����������벻һ��\n");
		goto looppwd;
	}

	GetTermCmd("��ѡ�������Ƿ���� 1-des���� 2-3DES���� 0-������",1,pwdflag,false);
	GetTermCmd("���������ݿ����� 0-oracle1-sybase2-sqlserver3-infomix4-db25-odbc6-mysql",1,dbtype,false);

	printf("\n");
	

	char xmlpath[500];
	memset(xmlpath,0,sizeof(xmlpath));
	
	std::vector<string>chgfile;
	if (dbtype[0]<'0' || dbtype[0] >'6')
	{
		printf("\n���ݿ����ʹ�\n");
		return -1;
	}
	if (argc >1) //�޸�ָ���ļ�
	{
		strcpy(xmlpath,argv[1]);
		if (modifile(xmlpath,dbname,dbuser,pwd,pwdflag,dbtype) == 0)
		{
			printf("\n�޸ĳɹ�\n");
		}
		else
		{
			printf("\n�޸�ʧ��\n");
		}
	}
	else //�޸�ָ��Ŀ¼�µ����������ļ� windowsΪ��ǰĿ¼��linuxΪetcĿ¼
	{
#ifdef WIN32
		WORKPATH(xmlpath);
#else
		sprintf(xmlpath,"%s/etc",(char *)getenv("HOME"));
#endif

		//printf("��ʼ�޸�\n");
		modixml(xmlpath,dbname,dbuser,pwd,pwdflag,dbtype,&chgfile);
		
		// 	printf("�޸��ļ���%d\n",chgfile.size());
		// 	for (int ii=0;ii<chgfile.size();ii++)
		// 	{
		// 		printf("   �޸��ļ��� [%s]\n",chgfile[ii].c_str());
		// 	}
		printf("\n�޸������ļ������ݿ��û�����ɹ�\n");

	}

	return 0;
}
