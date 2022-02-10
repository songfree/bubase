#include "public.h"
#include "BF_Tools.h"


void GetNamePath(const char *filepathname,char *filepath,char *filename)
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
		memcpy(filepath,filepathname,offset);
		filepath[offset]=0;
		filelen = filelen - offset;
		for (i=0; i< filelen; i++)
		{
			filename[i]=filepathname[i+offset+1];
		}
		filename[filelen-1]=0;
		
	}
	return ;
}


int main(int argc, char* argv[])
{
    char databuffer[8192];
    bzero(databuffer,sizeof(databuffer));
    int nRet = CBF_Tools::GetConsoleEnter(">请输入进程id\n>",10,databuffer);
    databuffer[nRet]=0;  
    nRet = kill(atol(databuffer),0);
	printf("kill 0 返回[%d]\n",nRet);
	if (nRet == 0)
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
    	
    	sprintf(tmpchar,"/proc/%d/exe",atoi(databuffer));
    	//程序信息
        int count = readlink(tmpchar, runpath, 300);
        if (count<0 || count >300)
        {
            printf("进程的程序信息不存在\n");
            return 0;
        }
        GetNamePath(runpath,prgpath,prgname);
        printf("进程的程序信息 程序目录[%s]程序名称[%s]\n",prgpath,prgname);
    	//运行目录
    	sprintf(tmpchar,"/proc/%d/cwd",atoi(databuffer));
    	count = readlink(tmpchar, runpath, 300);
        printf("进程的信息  运行目录[%s]\n",runpath);
        return 0;

#else
	return 0;
#endif
    }
	nRet = CBF_Tools::GetConsoleEnter(">按任意键退出\n>",1,databuffer);
}