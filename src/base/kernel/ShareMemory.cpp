// ShareMemory.cpp: implementation of the CShareMemory class.
//
//////////////////////////////////////////////////////////////////////

#include "ShareMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////




#ifdef _WINDOWS
#include <windows.h>
#include <share.h>
#include <sys/locking.h>

#define sh_open(a,b,c)  _sopen(a,b|O_BINARY,c,S_IREAD | S_IWRITE)
#define LT_LOCK      _LK_NBLCK
#define LT_UNLOCK    _LK_UNLCK
#else
#include <unistd.h>
#include <sys/ipc.h>

#define SH_DENYNO    0x40
#define SH_DENYRW    0x10
#define sh_open(a,b,c)  open(a,b,S_IREAD | S_IWRITE)
#define LT_LOCK   F_WRLCK
#define LT_UNLOCK F_UNLCK
#endif



#define BUSHM_KID 16
#define BUSHM_FLAG 0
#define SHM_RIGHT 0777

#define LOCK_POSITION 0x40000000L
#define MAX_FILELEN   0x3FFFFFFFL


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CShareMemory::CShareMemory()
{
	m_address = NULL;
	m_fileno = -1;
	m_shmid = INVALID_ID;
	m_size = -1;
	bzero(m_szmsg,sizeof(m_szmsg));
}

CShareMemory::~CShareMemory()
{
	Close();
}

void CShareMemory::Close()
{
#ifdef _WINDOWS
   if (m_address != NULL)
   {
      UnmapViewOfFile(m_address);
      m_address = NULL;
   }
   if (m_shmid != INVALID_ID)
   {
      CloseHandle(m_shmid);
      m_shmid = INVALID_ID;
   }
#else
   if (m_address != NULL)
   {
      // 先detach
      shmdt(m_address);
   }
   m_address = NULL; 
   m_shmid = INVALID_ID;
   m_size = 0;
#endif
   if (m_fileno != -1)
   {
      close(m_fileno);
      m_fileno = -1;
   }
}



char * CShareMemory::GetMessage()
{
   return(m_szmsg);
}

void * CShareMemory::GetSHMPtr()
{
   return(m_address);
}



key_t CShareMemory::myftok(const char *path, int ID)
{
	int  handle;
	//handle=open(buffer,O_RDWR);
    handle = sh_open(path,O_RDWR,SH_DENYNO);
	if (handle == -1)
	{
        handle = sh_open(path,O_CREAT|O_RDWR,SH_DENYNO);
		if (handle == -1)
		{
			printf("sh_open error,please check user limit!");
			return -2;
		}
		else
		{
			close(handle);
		}
	}
	else
	{
		close(handle);
	}
#ifdef _WINDOWS
    return (1);
#else
	//20151120 修改直接以文件索引号为id，因为ftok出来的ID竟然重复
    struct_stat fs;
	fs.st_ino =0;
	fs.st_size=-1;
    FSTAT(path,&fs);
	if (fs.st_size <0)
	{
		printf("FSTAT出错 文件[%s]不存在或无权限\n",path);
		return -1;
	}
	//20170424 rh linux7.2 文件ID超过整型值，取余
	if (fs.st_ino > 1200000000)
	{
		printf("myftok id[%lld] return [%lld]\n",fs.st_ino,(fs.st_ino % 1000000000));
		return (fs.st_ino % 1000000000);
	}
	else
	{
		return fs.st_ino;
	}
	

// 	if (fs.st_ino>100000000)
// 	{
// 		return fs.st_ino;
// 	}
// 	else
// 	{
// 		return (u_long)((u_long)1000000000+fs.st_ino);
// 	}
	
//     printf("st_dev: %u, st_ino: %u, key: %u \n", 
// 	   (u_long) fs.st_dev,(u_long) fs.st_ino, 
//               ftok(tmpfile, 16));
//	return ftok(path,ID);
#endif
}
int GetName(char* filename, char** pfn)
{
    int filelen;
    int i;
    int offset = -1;
    filelen = strlen(filename);
    if (filelen < 1)
    {
        *pfn = filename;
        return filelen;
    }
    for (i = filelen - 1; i >= 0; i--)
    {
        if (filename[i] == '\\' || filename[i] == '/')
        {
            offset = i;
            break;
        }
    }
    *pfn = filename + offset + 1;
    return filelen - offset;
}
void * CShareMemory::Open(const char *name, int shmsize)
{
	char buf[256];
	Close();
	sprintf(buf,"%s.shm",name);
	m_fileno = sh_open(buf,O_CREAT|O_RDWR,SH_DENYNO);
	if (m_fileno == -1)
	{
		sprintf(m_szmsg,"Cannot open SHM File for '%s' size=%d",name,shmsize);
		return(NULL);
	}
	else
	{
		close(m_fileno);
		m_fileno = -1;
	}
	bool isNewOpen=false;
#ifdef _WINDOWS
	
	char *smname=NULL;
	GetName((char *)name,&smname);
	if (shmsize>0)
	{
		m_shmid = CreateFileMapping(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,0,shmsize, smname);
		if (m_shmid == NULL)
		{
			// 共享内存创建错误：
			sprintf(m_szmsg,"Cannot open share memory for SHM(%s) size=%d -errno:%u!", smname,shmsize,GetLastError());
			Close();
			return(NULL);
		}
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			//打开已有的共享内存
			isNewOpen = false;
		}
		else
		{
			//新打开的共享内存
			isNewOpen = true;
		}
	}
	else
	{
		m_shmid = OpenFileMapping(FILE_MAP_WRITE|FILE_MAP_READ,FALSE, smname);
		if (m_shmid == NULL)
		{
			sprintf(m_szmsg,"Cannot open share memory for SHM(%s) - errno:%u!", smname,GetLastError());
			Close();
			return(NULL);
		}
	}
	m_address = MapViewOfFile(m_shmid,FILE_MAP_WRITE|FILE_MAP_READ,0,0,0);
	if (m_address == NULL)
	{
		sprintf(m_szmsg,"MapViewOfFile fail [SHM(%s) size=%d] - errno:%u!", smname,shmsize,GetLastError());
		Close();
		return(NULL);
	}
	if (isNewOpen) //新打开时重置为\0
	{
		if (shmsize>0)
		{
			printf("bzero shm\n");
			bzero(m_address,shmsize);
		}
	}
	BY_HANDLE_FILE_INFORMATION finfo;
	GetFileInformationByHandle(m_shmid,&finfo);
	m_size = finfo.nFileSizeLow;
	return(m_address);
#else
	isNewOpen = false;
	key_t shmkey = myftok(buf,BUSHM_KID);
	if (shmkey<0)
	{
		printf("myftok [%s %d] error!  %lld\n",buf);
		return (NULL);
	}

	sprintf(m_szmsg,"shmkey[%d]",shmkey);
	//尝试打开共享
	m_shmid = shmget(shmkey,0,BUSHM_FLAG);
	if (m_shmid == -1) //打不开则创建
	{
		m_shmid = shmget(shmkey,shmsize,SHM_RIGHT|IPC_CREAT);
		isNewOpen = true;
		if (m_shmid == -1)
		{
			// 共享内存创建错误：
			sprintf(m_szmsg,"Cannot open share memory for key:0x%X-errno:%d!",shmkey,errno);
			printf("Cannot open share memory for key:0x%X-errno:%d!\n",shmkey,errno);
			Close();
			return(NULL);
		}
	}
	
	m_address = shmat(m_shmid,0,0);
#ifdef SHM_FAILED //11:50 2006-7-4 for HP_UX64 
	if (m_address == SHM_FAILED)
#else
	if (m_address == (void *)-1L)
#endif
	{
		m_address = NULL;
		// 则表示挂接的地址出现问题，可能属于flag不正确的缘故
		sprintf(m_szmsg,"shmkey=%x:shmat(%d,0,0) 返回错误errno=%d!",shmkey,m_shmid,errno);
		printf("shmkey=%x:shmat(%d,0,0) 返回错误errno=%d!",shmkey,m_shmid,errno);
		Close();
		if (isNewOpen)
		{
			Remove(name);
		}
	}
	if (isNewOpen)
	{
		if (shmsize>0)
		{
//			printf("bzero shm\n");
			bzero(m_address,shmsize);
		}
	}
	struct shmid_ds info;
	shmctl(m_shmid,IPC_STAT,&info);
	m_size = info.shm_segsz;
	return(m_address);
#endif
}

bool CShareMemory::Remove(const char *name)
{
#ifdef _WINDOWS
	return(true);
#else
	char buf[256];
	sprintf(buf,"%s.shm",name);
	key_t shmkey = myftok(buf,BUSHM_KID);
	int shmid = shmget(shmkey,0,BUSHM_FLAG);
	if (shmid != -1)
	{
		if (shmctl(shmid,IPC_RMID,0) == -1)
		{
			return(false);
		}
	}
	return(true);
#endif
}

bool CShareMemory::locktype(int ltype, unsigned int offset, unsigned int len)
{
#ifdef _WINDOWS
   _lseek(m_fileno,offset,SEEK_SET);
   return (_locking(m_fileno,ltype,len)==0);
#else
   struct flock lck;
   lck.l_type = ltype;
   lck.l_whence = SEEK_SET;
   lck.l_start = offset;
   lck.l_len = len;
   lck.l_pid = getpid();
   return (fcntl(m_fileno,F_SETLK,&lck)==0);
#endif
}

bool CShareMemory::Lock(int offset, int len)
{
   unsigned int loff;
   unsigned int llen;
   if (offset<=0)
      loff = LOCK_POSITION;
   else
      loff = LOCK_POSITION+offset;
   if (len<=0)
   {
      loff = LOCK_POSITION;
      llen = MAX_FILELEN;
   }
   else
      llen = len;
   return(locktype(LT_LOCK,loff,llen));
}

bool CShareMemory::Unlock(int offset, int len)
{
   unsigned int loff;
   unsigned int llen;
   if (offset<=0)
      loff = LOCK_POSITION;
   else
      loff = LOCK_POSITION+offset;
   if (len<=0)
   {
      loff = LOCK_POSITION;
      llen = MAX_FILELEN;
   }
   else
      llen = len;
   return(locktype(LT_UNLOCK,loff,llen));
}
