// BF_Tools.cpp: implementation of the CBF_Tools class.
//
//////////////////////////////////////////////////////////////////////

#include "BF_Tools.h"
#include "BF_Zip.h"


#ifndef _WINDOWS
//#include <curses.h>  
#include <sys/ioctl.h>
#include <termios.h>

#if defined(LINUX)
#include <getopt.h>
#include <locale.h>
#endif
#endif


#ifdef _WINDOWS

/* static (global) variables that are specified as exported by getopt() */
char *optarg = NULL; /* pointer to the start of the option argument */
int optind = 1; /* number of the next argv[] to be evaluated */
int opterr = 1; /* non-zero if a question mark should be returned  when a non-valid option character is detected */
/* handle possible future character set concerns by putting this in a macro */
#define _next_char(string) (char)(*(string+1))

#endif

unsigned int CBF_Tools::m_sReverseMode=0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBF_Tools::CBF_Tools()
{
	
}

CBF_Tools::~CBF_Tools()
{
	
}

void CBF_Tools::ToUpper(char *lowerchar)
{
	int ret;
	int ichar;
	ret = strlen(lowerchar);
	for(int i=0;i< ret;i++)
	{
		ichar = lowerchar[i];
		lowerchar[i] = toupper(ichar);
	}
}

void CBF_Tools::ToLower(char *upperchar)
{
	int ret;
	int ichar;
	ret = strlen(upperchar);
	for(int i=0;i< ret;i++)
	{
		ichar = upperchar[i];
		upperchar[i] = tolower(ichar);
	}
}

char * CBF_Tools::Ltrim(char *str)
{
	char *p,*q;
	q=p=str;
	for(;*p!=0;p++)
	{
		if(*p!=' ')
		{
			break;
		}
	}
	if(p!=q)
	{
		for(;*p!=0;p++,q++)
		{
			*q=*p;
		}
		*q=0;
	}
	return(str);
}

char * CBF_Tools::Rtrim(char *str)
{
	int i,j;
	for(i=0,j=-1;*(str+i)!=0;i++)
	{
		if(*(str+i)!=' ')j=i;
	}
	*(str+j+1)=0;
	return(str);
}

char * CBF_Tools::LRtrim(char *str)
{
	Ltrim(str);
	return Rtrim(str);
}

std::string & CBF_Tools::Ltrim(std::string &str)
{
	int pos;
	int offset=0;
	pos=str.find(' ', 0);
	std::vector<int>deleteList;
	while (pos != -1 ) 	
	{
		if (pos == offset)
		{
			deleteList.push_back(pos);//记录要删除的空格位置
			offset++;
		}
		else
		{
			break;
		}
		pos=str.find(' ', offset);
	}
	//删除字符
#if defined(AIX) || defined(HP_UX) 
	if (deleteList.size()>0)
	{
		str = str.substr(deleteList[deleteList.size()-1]+1,str.length());
	}
#else
	for (int i=deleteList.size();i>0;i--) //从后向前删除
	{
		str.erase(deleteList[i-1], 1);	
	}
#endif	
	return str;
}
std::string  &CBF_Tools::Rtrim(std::string &str)
{
	int pos;
	int offset=str.length()-1;
	pos=str.rfind(' ', offset);
	std::vector<int>deleteList;
	while (pos != -1 ) 	
	{
		if (pos == offset)
		{
			deleteList.push_back(pos);//记录要删除的空格位置
			offset--;
		}
		else
		{
			break;
		}
		pos=str.rfind(' ', offset);
	}
	//删除字符
#if defined(AIX) || defined(HP_UX) 
	if (deleteList.size()>0)
	{
		str = str.substr(0,deleteList[deleteList.size()-1]);
	}
#else
	for (int i=0;i<(int)deleteList.size();i++) //从后向前删除
	{
		str.erase(deleteList[i], 1);
	}
#endif	
	return str;
}

std::string & CBF_Tools::LRtrim(std::string &str)
{
	Ltrim(str);
	return Rtrim(str);
}

double CBF_Tools::Round(double dd, unsigned int dec)
{
	double tt = 0.0;
	if (dd < 0)
	{
		tt = -0.500001;//-0.500001;
	}
	else
	{
		tt = 0.500001;// 0.500001;
	}
#if defined(_WINDOWS) && (_MSC_VER >1500)
	INT64_ power = pow((long double)10, (int)dec);
	INT64_ t = ((dd) * pow((long double)10,(int) dec) + tt);
#else
#ifdef AIX
	INT64_ power = (INT64_)pow((double)10, (double)dec);
	INT64_ t = (INT64_)((dd) * pow((double)10, (double)dec) + tt);
#else
	INT64_ power = (INT64_)pow((double)10, dec);
	INT64_ t = (INT64_)((dd) * pow((double)10, dec) + tt);
#endif
#endif
	return (t /(double) power);
}

void CBF_Tools::ToBinaryString(unsigned long data, char *str)
{
	int ret;
	char tmpchar[100];
	char tmpchar2[100];
	char tmpchar3[5];
	sprintf(tmpchar,"%X",data);
	ret = strlen(tmpchar);
	if ((ret%2) != 0)
	{
		sprintf(tmpchar2,"0%s",tmpchar);
		strcpy(tmpchar,tmpchar2);
	}
	ret = strlen(tmpchar);
	for (int i=0;i<ret;i++)
	{
		switch (tmpchar[i])
		{
			case '0':
				sprintf(tmpchar3,"0000");
				break;
			case '1':
				sprintf(tmpchar3,"0001");
				break;
			case '2':
				sprintf(tmpchar3,"0010");
				break;
			case '3':
				sprintf(tmpchar3,"0011");
				break;
			case '4':
				sprintf(tmpchar3,"0100");
				break;
			case '5':
				sprintf(tmpchar3,"0101");
				break;
			case '6':
				sprintf(tmpchar3,"0110");
				break;
			case '7':
				sprintf(tmpchar3,"0111");
				break;
			case '8':
				sprintf(tmpchar3,"1000");
				break;
			case '9':
				sprintf(tmpchar3,"1001");
				break;
			case 'A':
				sprintf(tmpchar3,"1010");
				break;
			case 'B':
				sprintf(tmpchar3,"1011");
				break;
			case 'C':
				sprintf(tmpchar3,"1100");
				break;
			case 'D':
				sprintf(tmpchar3,"1101");
				break;
			case 'E':
				sprintf(tmpchar3,"1110");
				break;
			case 'F':
				sprintf(tmpchar3,"1111");
				break;
			default:
				return ;
		}
		if (0 == i)
		{
			strcpy(str,tmpchar3);
		}
		else
		{
			strcat(str,tmpchar3);
		}
	}
	return ;
}

INT64_ CBF_Tools::StringToInteger(std::string data, unsigned int jz)
{
	int len=data.length();
	INT64_ IntValue=0;

	INT64_ bitValue =-1;
	int i;
	char ch[]="00112233445566778899aAbBcCdDeEfF";
// 	if(!IsNumberData((char *)data.c_str(),jz))
// 	{
// 		return -1;
// 	}
	for(i=0;i<len;i++)
	{
		if (i == 0 && ( data[i]== '-' || data[i] == '+' ))
		{
			continue;
		}
		bool isjz=false;
		for(int j=0; j<(int)jz; j++)
		{
			if(data[i] == ch[j*2])
			{
				isjz = true;
				break;
			}
			if(data[i] == ch[j*2+1])
			{
				isjz = true;
				break;
			}
		}
		if (!isjz)
		{
			return -1;
		}
		if(data[i]>='0' && data[i]<='9')
		{
			bitValue=data[i]-0x30;
		}
		else
		{
			switch(data[i])
			{
				case 'a':
					bitValue=0x0a;
					break;
				case 'b':
					bitValue=0x0b;
					break;
				case 'c':
					bitValue=0x0c;
					break;
				case 'd':
					bitValue=0x0d;
					break;
				case 'e':
					bitValue=0x0e;
					break;
				case 'f':
					bitValue=0x0f;
					break;
				case 'A':
					bitValue=0x0a;
					break;
				case 'B':
					bitValue=0x0b;
					break;
				case 'C':
					bitValue=0x0c;
					break;
				case 'D':
					bitValue=0x0d;
					break;
				case 'E':
					bitValue=0x0e;
					break;
				case 'F':
					bitValue=0x0f;
					break;
				default:
					break;
			}
		}
#if defined(_WINDOWS) && _MSC_VER > 1500 
		IntValue=bitValue*(INT64_)pow((long double)jz,(int)(len-i-1))+IntValue;
#else
#ifdef AIX
		IntValue=bitValue*(INT64_)pow((double)jz,(double)(len-i-1))+IntValue;
#else
		IntValue=bitValue*(INT64_)pow(jz,len-i-1)+IntValue;
#endif
#endif
	}
	return IntValue;
}

bool CBF_Tools::IsNumberData(char *c, unsigned int jz)
{
	if( (jz<2) || (jz>16) )
	{
		return false;
	}
	char ch[]="00112233445566778899aAbBcCdDeEfF";
	int len = strlen(c);
	int i=0;
	for (int k=0; k< len ; k++)
	{
		if (k == 0 && ( c[k] == '-' || c[k] == '+' ))
		{
			continue;
		}
		bool isjz=false;
		for(i=0; i<(int)jz; i++)
		{
			if(c[k] == ch[i*2])
			{
				isjz = true;
				break;
			}
			if(c[k] == ch[i*2+1])
			{
				isjz = true;
				break;
			}
		}
		if (!isjz)
		{
			return false;
		}
	}
	return true;
}

int CBF_Tools::Getch()
{
#ifndef _WINDOWS
	struct termios tm, tm_old; 
	int fd = STDIN_FILENO, c; 
	if(tcgetattr(fd, &tm) < 0) 
	{
		return -1; 
	}
	memcpy(&tm_old,&tm,sizeof(tm));
	tm.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	tm.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP |IXON);
	tm.c_cflag &= ~(CSIZE | PARENB);
	tm.c_cflag |= CS8;
	tm.c_oflag &= ~(OPOST);
	tm.c_cc[VMIN] = 1;
	tm.c_cc[VTIME] = 0;
	if(tcsetattr(fd, TCSANOW, &tm) < 0)
	{
		return -1; 
	}
	c = fgetc(stdin); 
	if(tcsetattr(fd, TCSANOW, &tm_old) < 0) 
	{
		return -1; 
	}
	return c; 
#else
	return _getch();
#endif
}

int CBF_Tools::GetConsoleEnter(const char *prompt, unsigned int getlen, char *getstr, bool ispwd)
{
	int count = 0;
	char ch;
	printf("\n>%s:",prompt);
	while ( (ch = Getch()) !=13 )
	{
		getstr[count]=ch;
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
		if (ispwd)
		{
			printf("*");
		}
		else
		{
			printf("%c",ch);
		}
		if (getlen >0 && count >= (int)getlen)
		{
			break;
		}
	} 
	getstr[count]=0;
	printf("\n");
	return count;
}

unsigned short CBF_Tools::CRC(void *pdata, int datalen)
{
	unsigned short acc = 0, i;
	unsigned char* Data = (unsigned char*) pdata;
	while(datalen--)
	{
		acc = acc ^ (*Data++ << 8);
		for(i = 0; i++ < 8; )
		{
			if(acc & 0x8000)
			{
				acc = (acc << 1) ^ 0x1021;
			}
			else
			{
				acc <<= 1;
			}
		}
	}
	i = ( (unsigned short) ( ((unsigned char)acc) << 8) ) | ((unsigned short)(acc >> 8));
	return i;
}

void CBF_Tools::Reverse(unsigned char *buf, int len)
{
	int i;
	unsigned char buf1[100];
	
	if (m_sReverseMode == 0)
	{
		if (htonl(1) == 1)
		{
			m_sReverseMode = 1;
		}
		else
		{
			m_sReverseMode = 2;
		}
	}
	if (m_sReverseMode == 1)
	{
		for(i=0;i<len;i++)
		{
			buf1[i] = buf[len-i-1];
		}
		memcpy(buf,buf1,len);
	}
}

int CBF_Tools::Getopt(int argc, char *argv[], char *optstring)
{
#if defined(_WINDOWS)
	static char *pIndexPosition = NULL; /* place inside current argv string */
	char *pArgString = NULL; /* where to start from next */
	char *pOptString; /* the string in our program */


	if (pIndexPosition != NULL) 
	{
		/* we last left off inside an argv string */
		if (*(++pIndexPosition)) 
		{
			/* there is more to come in the most recent argv */
			pArgString = pIndexPosition;
		}
	}

	if (pArgString == NULL)
	{
		/* we didn't leave off in the middle of an argv string */
		if (optind >= argc) 
		{
			/* more command-line arguments than the argument count */
			pIndexPosition = NULL; /* not in the middle of anything */
			return EOF; /* used up all command-line arguments */
		}

		/*---------------------------------------------------------------------
		* If the next argv[] is not an option, there can be no more options.
		*-------------------------------------------------------------------*/
		pArgString = argv[optind++]; /* set this to the next argument ptr */

		if (('/' != *pArgString) && /* doesn't start with a slash or a dash? */
			('-' != *pArgString)) 
		{
			--optind; /* point to current arg once we're done */
			optarg = NULL; /* no argument follows the option */
			pIndexPosition = NULL; /* not in the middle of anything */
			return EOF; /* used up all the command-line flags */
		}

		/* check for special end-of-flags markers */
		if ((strcmp(pArgString, "-") == 0) ||
		(strcmp(pArgString, "--") == 0)) 
		{
			optarg = NULL; /* no argument follows the option */
			pIndexPosition = NULL; /* not in the middle of anything */
			return EOF; /* encountered the special flag */
		}

		pArgString++; /* look past the / or - */
	}

	if (':' == *pArgString)
	{ /* is it a colon? */
		/*---------------------------------------------------------------------
		* Rare case: if opterr is non-zero, return a question mark;
		* otherwise, just return the colon we're on.
		*-------------------------------------------------------------------*/
		return (opterr ? (int)'?' : (int)':');
	}
	else if ((pOptString = strchr(optstring, *pArgString)) == 0) 
	{
		/*---------------------------------------------------------------------
		* The letter on the command-line wasn't any good.
		*-------------------------------------------------------------------*/
		optarg = NULL; /* no argument follows the option */
		pIndexPosition = NULL; /* not in the middle of anything */
		return (opterr ? (int)'?' : (int)*pArgString);
	}
	else
	{
		/*---------------------------------------------------------------------
		* The letter on the command-line matches one we expect to see
		*-------------------------------------------------------------------*/
		if (':' == _next_char(pOptString)) 
		{ /* is the next letter a colon? */
			/* It is a colon. Look for an argument string. */
			if ('\0' != _next_char(pArgString))
			{ /* argument in this argv? */
				optarg = &pArgString[1]; /* Yes, it is */
			}
			else
			{
				/*-------------------------------------------------------------
				* The argument string must be in the next argv.
				* But, what if there is none (bad input from the user)?
				* In that case, return the letter, and optarg as NULL.
				*-----------------------------------------------------------*/
				if (optind < argc)
					optarg = argv[optind++];
				else 
				{
					optarg = NULL;
					return (opterr ? (int)'?' : (int)*pArgString);
				}
			}
			pIndexPosition = NULL; /* not in the middle of anything */
		}
		else 
		{
			/* it's not a colon, so just return the letter */
			optarg = NULL; /* no argument follows the option */
			pIndexPosition = pArgString; /* point to the letter we're on */
		}
		return (int)*pArgString; /* return the letter that matched */
	}
#elif defined(LINUX)
	return getopt(argc,argv,optstring);
#else
	return getopt(argc,argv,optstring);
#endif
}

void CBF_Tools::GetModuleNamePath(char *argv[], char *curpath, char *prgpath, char *prgname)
{
#ifdef _WINDOWS
	char buffer[256];
	int len;
	bzero(buffer,sizeof(buffer));
	GetModuleFileNameA(NULL, buffer, 255);
	len=strlen(buffer);
	while (len > 0)
	{
		if (buffer[len-1] == '\\')
			break;
		else
			len--;
	}
	strcpy(prgpath,buffer);
	prgpath[len]=0;
	strcpy(prgname,buffer+len);
	
	_getcwd(buffer,sizeof(buffer));
	strcpy(curpath,buffer);
#else
	int i,l;
	char exeFullPath[512];
	char buffer[512];
	
	strcpy(exeFullPath,argv[0]);
	l=0;
	for(i=strlen(exeFullPath)-1; i>=0;i--)
	{
		if((exeFullPath[i]=='/')||(exeFullPath[i]=='\\'))
		{
			strcpy(prgname,exeFullPath+i+1);
			strcpy(prgpath,exeFullPath);
			prgpath[i]=0;
			l=1;
			break;
		}
	}
	getcwd(buffer,sizeof(buffer));
	strcpy(curpath,buffer);
	if (l == 0) //说明argv里面的prg不带路径
	{
		strcpy(prgname,exeFullPath);//程序名称
		struct_stat ft;
		ft.st_size = -1;
		sprintf(exeFullPath,"%s/%s",curpath,prgname);//去当前目录下面找prgname
//		printf("prgpathname=%s\n",exeFullPath);
		if (FSTAT(exeFullPath,&ft)<0 ) //取文件信息，返回-1出错
		{
			std::string pathstring = getenv("PATH");//去PATH里面找
			int npos=0;
			int offset=0;
			while (npos != -1)
			{
				npos = pathstring.find(':',offset);
				if (npos > offset)
				{
					strcpy(prgpath,pathstring.substr(offset,npos-offset).c_str());
					sprintf(exeFullPath,"%s/%s",prgpath,prgname);
//					printf("prgpathname=%s\n",exeFullPath);
					if (FSTAT(exeFullPath,&ft)<0 ) //取文件信息，返回-1出错
					{
						offset = npos+1;//往后找
						continue;
					}
					break;
				}
				if (npos == -1)
				{
					strcpy(prgpath,pathstring.substr(offset,pathstring.length()-offset).c_str());
					sprintf(exeFullPath,"%s/%s",prgpath,prgname);
					if (FSTAT(exeFullPath,&ft)<0 ) //取文件信息，返回-1出错
					{
						strcpy(prgpath,curpath);
					}
					break;
				}
				offset = npos+1;//往后找
			}
		}
		else //程序存在
		{
			strcpy(prgpath,curpath);
		}
	}
	chdir(curpath);
#endif
}

bool CBF_Tools::Compress(unsigned char *destbuf, unsigned int &destlen, unsigned char *srbuf, int srbuflen)
{
	unsigned int dl=destlen; 
	if (!BF_Compress(destbuf,&dl,srbuf,srbuflen))
	{
		return false;
	}
	destlen = dl;
	return true;
}

bool CBF_Tools::Uncompress(unsigned char *destbuf, unsigned int &destlen, unsigned char *srbuf, int srbuflen)
{
	unsigned int dl=destlen; 

	if (!BF_Uncompress(destbuf,&dl,srbuf,srbuflen))
	{
		return false;
	}
	destlen = dl;
	return true;
}

bool CBF_Tools::MakeDir(const char *path)
{
	assert(path != NULL );
	char cdir[300];
	char subdir[300];
	bzero(cdir,sizeof(cdir));
	bzero(subdir,sizeof(subdir));
	strcpy(cdir,path);
	int dlen;
	dlen = strlen(cdir);
	if (dlen == 0)
	{
		return false;
	}
	//最后面增加'/'或'\\'
	if (cdir[dlen-1] != '\\' && cdir[dlen-1] != '/' )
	{
#ifdef _WINDOWS
		strcat(cdir,"\\");
#else
		strcat(cdir,"/");
#endif
		dlen++;
	}
	bool iswindowsvmc=false;
#ifdef _WINDOWS
	if (cdir[0] == '\\' && cdir[1] == '\\')
	{
		//这种情况下是windows虚拟机访问本地磁盘
		iswindowsvmc = true;
	}
#endif
	unsigned int dirlay=0;
	int ret;
	if (access(cdir,0) != 0 )  //目录不存在
	{
//		fprintf(stderr,"目录[%s]不存在\n",cdir);
		for (int i=0 ; i < dlen ;i++)
		{
			if (cdir[i] == '\\' || cdir[i] == '/') 
			{
				if ( i<1 )
				{
					continue;
				}
				dirlay++;
				if (iswindowsvmc && dirlay<3)
				{
					continue;
				}
				memcpy(subdir,cdir,i);
				subdir[i] = 0;
//				fprintf(stderr,"判断目录[%s]是否存在 i=%d\n",subdir,i);
				if (access(subdir,0) != 0 ) //目录不存在
				{
//					fprintf(stderr,"目录[%s]不存在，开始创建\n",subdir);
#ifdef _WINDOWS
					ret = mkdir(subdir);
#else
					ret = mkdir(subdir,0777);
#endif
					if (ret !=0 )
					{
//						fprintf(stderr,"创建目录[%s]失败\n",subdir);
						return false;
					}
				} //end if (access(subdir,0) != 0 ) //目录不存在
			}// end if (cdir[i] == '\\' || cdir[i] == '/') 
		} // end for 
	}
	return true;

}

int CBF_Tools::RemoveOverdueFile(const char *delpath,const char *fileattr, int overduedays, bool isext)
{
	assert((delpath !=NULL));

	if (strlen(delpath)<1) 
	{
		return -1;
	}
    int nfs=0;
    char szfile[1024];
    time_t odtime;

    if (overduedays<1)
	{
		return 0;
	}
	odtime = time(NULL)-(overduedays*86400); 
	if (isext)
	{
#ifdef _WINDOWS
		sprintf(szfile,"%s\\*%s",delpath,fileattr);
#else
		sprintf(szfile,"%s/*%s",delpath,fileattr);
#endif
	}
	else
	{
#ifdef _WINDOWS
		sprintf(szfile,"%s\\%s*",delpath,fileattr);
#else
		sprintf(szfile,"%s/%s*",delpath,fileattr);
#endif
	}
   
#ifdef _WINDOWS
	struct _finddata_t fd;
	intptr_t dh;
	dh = _findfirst(szfile,&fd);
	if (dh ==-1 ) 
	{
		return 0;
	}
	do
	{
		if ( odtime > fd.time_write ) //&& strstr(fd.name,ext)!=NULL
		{
			sprintf(szfile,"%s\\%s",delpath,fd.name);
			UNLINK(szfile);
			nfs++;
		}
	}while (_findnext(dh,&fd) != -1);
	_findclose(dh);

#else
	DIR *dp;
	struct dirent *dirp;
	struct stat fst;
	dp = opendir(delpath);
	if (dp == NULL) 
	{
		return 0;
	}
	while ( ( dirp = readdir(dp) ) != NULL)
	{
		sprintf(szfile,"%s/%s",delpath,dirp->d_name);
		if ( stat(szfile,&fst) == 0  && odtime>fst.st_mtime) 
		{
			if (isext)
			{
				
				if (strstr(dirp->d_name,fileattr) != NULL)
				{
					UNLINK(szfile);
					nfs++;
				}
			}
			else
			{
				if (strncmp(dirp->d_name,fileattr,strlen(fileattr)) ==0 )
				{
					UNLINK(szfile);
					nfs++;
				}
			}
			
		}
	}	
	closedir(dp);
#endif
	return(nfs);
}

bool CBF_Tools::IsNumber(const char *str)
{
	int len;
	len = strlen(str);
	if (len<1) return true;
	for (int i=0;i<len;i++)
	{
		if (str[i]<'0' || str[i]>'9')
		{
			return false;
		}
	}
	return true;	
}


bool CBF_Tools::StringCopy(char *dest, unsigned int destlen, const char *src)
{
	assert(src);
	assert(dest);
	int len = strlen(src);
	if (len > (int)destlen)
	{
		strncpy(dest,src,destlen);
		dest[destlen] = 0;
		return false;
	}
	else
	{
		strncpy(dest,src,len);
		dest[len] = 0;
		return true;
	}
}

void CBF_Tools::DSP_2_HEX(const char *dsp,char *hex,int count)
{
    int i;
    for(i = 0; i < count; i++)
    {
		hex[i]=((dsp[i*2]<=0x39)?dsp[i*2]-0x30:dsp[i*2]-0x41+10);
        hex[i]=hex[i]<<4;
		hex[i]+=((dsp[i*2+1]<=0x39)?dsp[i*2+1]-0x30:dsp[i*2+1]-0x41+10);
    }
} 

void CBF_Tools::HEX_2_DSP(const char *hex,char *dsp,int count)
{
    int i;
    char ch;
    for(i = 0; i < count; i++)
    {
        ch=(hex[i]&0xf0)>>4;
        dsp[i*2]=(ch>9)?ch+0x41-10:ch+0x30;
        ch=hex[i]&0xf;
        dsp[i*2+1]=(ch>9)?ch+0x41-10:ch+0x30;
    }
} 
//
//int CBF_Tools::gbk2utf8(char* desStr, const char* srcStr, int desStrsize)
//{
//    if(NULL == srcStr)
//    {
//        printf("Bad Parameter\n");
//        return -1;
//    }
// 
//    //首先先将gbk编码转换为unicode编码
//    if(NULL==setlocale(LC_ALL,"zh_CN.gbk"))//设置转换为unicode前的码,当前为gbk编码
//    {
//        printf("Bad Parameter\n");
//        return -1;
//    }
// 
//    int unicodeLen=mbstowcs(NULL,srcStr,0);//计算转换后的长度
//    if(unicodeLen<=0)
//    {
//        printf("Can not Transfer!!!\n");
//        return -1;
//    }
//    wchar_t *unicodeStr=(wchar_t *)calloc(sizeof(wchar_t),unicodeLen+1);
//    mbstowcs(unicodeStr,srcStr,strlen(srcStr));//将gbk转换为unicode
//    
//    //将unicode编码转换为utf8编码
//    if(NULL==setlocale(LC_ALL,"zh_CN.utf8"))//设置unicode转换后的码,当前为utf8
//    {
//        printf("Bad Parameter\n");
//        return -1;
//    }
//    int utfLen=wcstombs(NULL,unicodeStr,0);//计算转换后的长度
//    if(utfLen<=0)
//    {
//        printf("Can not Transfer!!!\n");
//        return -1;
//    }
//    else if(utfLen>= desStrsize)//判断空间是否足够
//    {
//        printf("Dst Str memory not enough\n");
//        return -1;
//    }
//    wcstombs(desStr,unicodeStr,utfLen);
//	desStr[utfLen]=0;//添加结束符
//    free(unicodeStr);
//    return utfLen;
//}