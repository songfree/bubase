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
	bzero(m_szmsg, sizeof(m_szmsg));
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
	m_size = 0;
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



char* CShareMemory::GetMessage()
{
	return m_szmsg;
}

void* CShareMemory::GetSHMPtr()
{
	return m_address;
}



key_t CShareMemory::myftok(const char* path, int ID)
{
	if (path == NULL)
	{
		return -1;
	}
	int  handle = sh_open(path, O_RDWR, SH_DENYNO);
	if (handle == -1)
	{
		handle = sh_open(path, O_CREAT | O_RDWR, SH_DENYNO);
		if (handle == -1)
		{
			printf("sh_open error,please check user limit!");
			return -2;
		}
	}
	close(handle);

#ifdef _WINDOWS
	return (1);
#else
	//20151120 修改直接以文件索引号为id，因为ftok出来的ID竟然重复
	struct_stat fs;
	fs.st_ino = 0;
	fs.st_size = -1;
	FSTAT(path, &fs);
	if (fs.st_size < 0)
	{
		printf("FSTAT出错 文件[%s]不存在或无权限\n", path);
		return -1;
	}
	//20170424 rh linux7.2 文件ID超过整型值，取余
	if (fs.st_ino > 1200000000)
	{
		printf("myftok id[%lld] return [%lld]\n", (INT64_)fs.st_ino, (INT64_)(fs.st_ino % 1000000000));
		return (fs.st_ino % 1000000000);
	}
	return (key_t)fs.st_ino;
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
void* CShareMemory::Open(const char* name, INT64_ shmsize)
{
	if (name == NULL)
	{
		snprintf(m_szmsg, sizeof(m_szmsg), "Open: name is NULL");
		return NULL;
	}
	char buf[256];
	Close();
	sprintf(buf, "%s.shm", name);
	m_fileno = sh_open(buf, O_CREAT | O_RDWR, SH_DENYNO);
	if (m_fileno == -1)
	{
#ifdef _WINDOWS
		sprintf(m_szmsg, "Cannot open SHM File for '%s' size=%I64d", name, shmsize);
#else
		sprintf(m_szmsg, "Cannot open SHM File for '%s' size=%lld", name, shmsize);
#endif
		return NULL;
	}
	// 注意：fd 保持打开，供 Lock/Unlock 使用，Close 时关闭
	bool isNewOpen = false;
#ifdef _WINDOWS

	char* smname = NULL;
	GetName((char*)name, &smname);
	if (shmsize > 0)
	{
		int high = (int)((shmsize & 0xFFFFFFFF00000000LL) >> 32);
		int low = (int)(shmsize & 0xFFFFFFFFLL);
		m_shmid = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, high, low, smname);
		if (m_shmid == NULL)
		{
			// 共享内存创建错误：
			sprintf(m_szmsg, "Cannot open share memory for SHM(%s) size=%I64d -errno:%u!", smname, shmsize, GetLastError());
			Close();
			return NULL;
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
		m_shmid = OpenFileMapping(FILE_MAP_WRITE | FILE_MAP_READ, FALSE, smname);
		if (m_shmid == NULL)
		{
			sprintf(m_szmsg, "Cannot open share memory for SHM(%s) - errno:%u!", (unsigned)smname, GetLastError());
			Close();
			return NULL;
		}
	}
	// 计算映射视图大小：0 表示映射全部
	SIZE_T viewSize = 0;
	if (shmsize > 0)
	{
		if ((INT64_)shmsize > (INT64_)(SIZE_T)-1)
		{
			// 32 位进程地址空间不够，明确报错而不是静默截断
			snprintf(m_szmsg, sizeof(m_szmsg),
				"Cannot map SHM(%s) size=%I64d: exceeds address space of current process",
				smname, shmsize);
			Close();
			return NULL;
		}
		viewSize = (SIZE_T)shmsize;
	}
	m_address = MapViewOfFile(m_shmid, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, viewSize);
	if (m_address == NULL)
	{
		sprintf(m_szmsg, "MapViewOfFile fail [SHM(%s) size=%I64d] - errno:%u!", smname, shmsize, (unsigned)GetLastError());
		Close();
		return NULL;
	}
	if (shmsize > 0)
	{
		m_size = shmsize;
	}
	else
	{
		MEMORY_BASIC_INFORMATION mbi;
		if (VirtualQuery(m_address, &mbi, sizeof(mbi)))
			m_size = (INT64_)mbi.RegionSize;
		else
			m_size = 0;
	}
	if (isNewOpen && shmsize > 0) //新打开时重置为\0
	{
		printf("bzero shm\n");
		bzero(m_address, shmsize);
	}
	else
	{
		if (shmsize > 0 && shmsize != m_size)
		{
			Close();
			printf("共享内存大小不匹配 应该[%lld] 已有[%lld]\n", shmsize, m_size);
			return NULL;
		}
	}
	return m_address;
#else
	key_t shmkey = myftok(buf, BUSHM_KID);
	if (shmkey < 0)
	{
		printf("myftok [%s %d] error!  %lld\n", buf);
		Close();
		return NULL;
	}
	sprintf(m_szmsg, "shmkey[%d]", shmkey);
	//尝试打开共享
	m_shmid = shmget(shmkey, 0, BUSHM_FLAG);
	if (m_shmid == -1) //打不开则创建
	{
		m_shmid = shmget(shmkey, shmsize, SHM_RIGHT | IPC_CREAT);
		if (m_shmid == -1)
		{
			// 尝试独占创建，区分创建者，避免并发创建时误清对方数据
			m_shmid = shmget(shmkey, (size_t)shmsize, SHM_RIGHT | IPC_CREAT | IPC_EXCL);
			if (m_shmid != -1)
			{
				isNewOpen = true;
			}
			else
			{
				// 别人刚创建了，重新打开
				m_shmid = shmget(shmkey, (size_t)shmsize, SHM_RIGHT | IPC_CREAT);
				if (m_shmid == -1)
				{
					snprintf(m_szmsg, sizeof(m_szmsg),
						"Cannot open share memory for key:0x%X - errno:%d",
						(unsigned)shmkey, errno);
					printf("%s\n", m_szmsg);
					Close();
					return NULL;
				}
				isNewOpen = false;
			}
		}
	}

	m_address = shmat(m_shmid, 0, 0);
#ifdef SHM_FAILED //11:50 2006-7-4 for HP_UX64 
	if (m_address == SHM_FAILED)
#else
	if (m_address == (void*)-1L)
#endif
	{
		m_address = NULL;
		// 则表示挂接的地址出现问题，可能属于flag不正确的缘故
		sprintf(m_szmsg, "shmkey=%x:shmat(%d,0,0) 返回错误errno=%d!", shmkey, m_shmid, errno);
		printf("shmkey=%x:shmat(%d,0,0) 返回错误errno=%d!", shmkey, m_shmid, errno);
		Close();
		if (isNewOpen)
		{
			Remove(name);
		}
		return NULL;
	}
	struct shmid_ds info;
	if (shmctl(m_shmid, IPC_STAT, &info) == 0)
		m_size = (INT64_)info.shm_segsz;
	else
		m_size = shmsize;
	if (isNewOpen)
	{
		if (shmsize > 0)
		{
			//			printf("bzero shm\n");
			bzero(m_address, shmsize);
		}
	}
	else
	{
		if (shmsize > 0 && shmsize != m_size)
		{
			Close();
			printf("共享内存大小不匹配 应该[%lld] 已有[%lld]\n", shmsize, m_size);
			return NULL;
		}
	}
	return m_address;
#endif
}

bool CShareMemory::Remove(const char* name)
{
#ifdef _WINDOWS
	return true;
#else
	if (name == NULL)
		return false;
	char buf[256];
	sprintf(buf, "%s.shm", name);
	key_t shmkey = myftok(buf, BUSHM_KID);
	if (shmkey < 0)
	{
		printf("myftok [%s] error!\n", buf);
		return false;
	}
	int shmid = shmget(shmkey, 0, BUSHM_FLAG);
	if (shmid != -1)
	{
		if (shmctl(shmid, IPC_RMID, 0) == -1)
		{
			return false;
		}
	}
	else
	{
		printf("shmname[%s] shmkey[%d] shm not exist\n", name, (int)shmkey);
	}
	return true;
#endif
}

bool CShareMemory::locktype(int ltype, unsigned int offset, unsigned int len)
{
	if (m_fileno == -1)
	{
		snprintf(m_szmsg, sizeof(m_szmsg), "locktype: file not open");
		return false;
	}
#ifdef _WINDOWS
	_lseek(m_fileno, offset, SEEK_SET);
	return (_locking(m_fileno, ltype, (long)len) == 0);
#else
	struct flock lck;
	memset(&lck, 0, sizeof(lck));
	lck.l_type = (short)ltype;
	lck.l_whence = SEEK_SET;
	lck.l_start = offset;
	lck.l_len = len;
	lck.l_pid = getpid();
	return (fcntl(m_fileno, F_SETLK, &lck) == 0);
#endif
}

bool CShareMemory::Lock(int offset, int len)
{
	unsigned int loff;
	unsigned int llen;
	if (offset <= 0)
		loff = LOCK_POSITION;
	else
		loff = LOCK_POSITION + offset;
	if (len <= 0)
	{
		loff = LOCK_POSITION;
		llen = MAX_FILELEN;
	}
	else
		llen = len;
	return locktype(LT_LOCK, loff, llen);
}

bool CShareMemory::Unlock(int offset, int len)
{
	unsigned int loff;
	unsigned int llen;
	if (offset <= 0)
		loff = LOCK_POSITION;
	else
		loff = LOCK_POSITION + offset;
	if (len <= 0)
	{
		loff = LOCK_POSITION;
		llen = MAX_FILELEN;
	}
	else
		llen = len;
	return(locktype(LT_UNLOCK, loff, llen));
}
