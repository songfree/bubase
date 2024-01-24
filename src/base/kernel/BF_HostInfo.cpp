// HostInfo.cpp: implementation of the CHostInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "BF_HostInfo.h"
#include "BF_Tools.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#ifdef _WINDOWS
#include <windows.h>

#define SystemBasicInformation       0   //系统基本信息
#define SystemPerformanceInformation 2   //系统运行信息
#define SystemTimeInformation        3   //系统时间信息
#define SystemProcessInformation     5    //处理器信息
#define SystemProcessorPerformanceInformation  8   //处理器运行信息


#define Li2Double(x) ((double)((x).HighPart) * 4.294967296E9 + (double)((x).LowPart))   //???????????

typedef LONG (WINAPI *PROCNTQSI)(UINT,PVOID,ULONG,PULONG);
PROCNTQSI NtQuerySystemInformation;

typedef BOOL (WINAPI *MYGETSYSTIMES)(LPFILETIME ,LPFILETIME ,LPFILETIME );
MYGETSYSTIMES my_NtQuerySystemInformation;

typedef struct
{
    DWORD   dwUnknown1;
    ULONG   uKeMaximumIncrement;
    ULONG   uPageSize;
    ULONG   uMmNumberOfPhysicalPages;
    ULONG   uMmLowestPhysicalPage;
    ULONG   uMmHighestPhysicalPage;
    ULONG   uAllocationGranularity;
    PVOID   pLowestUserAddress;
    PVOID   pMmHighestUserAddress;
    ULONG   uKeActiveProcessors;
    BYTE    bKeNumberProcessors;
    BYTE    bUnknown2;
    WORD    wUnknown3;
} SYSTEM_BASIC_INFORMATION;         //系统基本信息结构   对应 SystemBasicInformation



typedef struct __SYSTEM_PROCESSOR_TIMES
{
	LARGE_INTEGER IdleTime;               //空闲时间；
	LARGE_INTEGER KernelTime;             //内核模式时间；
	LARGE_INTEGER UserTime;               //用户模式时间；
	LARGE_INTEGER DpcTime;                //延迟过程调用时间；
	LARGE_INTEGER InterruptTime;          //中断时间；
	ULONG         InterruptCount;         //中断次数；
 }SYSTEM_PROCESSOR_TIMES,*PSYSTEM_PROCESSOR_TIMES;  //对应SystemProcessInformation

typedef struct _SYSTEM_PERFORMANCE_INFORMATION
{
	LARGE_INTEGER  IdleTime;                    //CPU空闲时间；
	LARGE_INTEGER  ReadTransferCount;           //I/O读操作数目；
	LARGE_INTEGER  WriteTransferCount;          //I/O写操作数目；
	LARGE_INTEGER  OtherTransferCount;          //I/O其他操作数目；
	ULONG          ReadOperationCount;          //I/O读数据数目；
	ULONG          WriteOperationCount;         //I/O写数据数目；
	ULONG          OtherOperationCount;         //I/O其他操作数据数目；
	ULONG          AvailablePages;              //可获得的页数目；
	ULONG          TotalCommittedPages;         //总共提交页数目；
	ULONG          TotalCommitLimit;            //已提交页数目；
	ULONG          PeakCommitment;              //页提交峰值；
	ULONG          PageFaults;                  //页故障数目；
	ULONG          WriteCopyFaults;             //Copy-On-Write故障数目；
	ULONG          TransitionFaults;            //软页故障数目；
	ULONG          Reserved1;
	ULONG          DemandZeroFaults;            //需求0故障数；
	ULONG          PagesRead;                   //读页数目；
	ULONG          PageReadIos;                 //读页I/O操作数；
	ULONG          Reserved2[2];
	ULONG          PagefilePagesWritten;        //已写页文件页数；
	ULONG          PagefilePageWriteIos;        //已写页文件操作数；
	ULONG          MappedFilePagesWritten;      //已写映射文件页数；
	ULONG          MappedFileWriteIos;          //已写映射文件操作数；
	ULONG          PagedPoolUsage;              //分页池使用；
	ULONG          NonPagedPoolUsage;           //非分页池使用；
	ULONG          PagedPoolAllocs;             //分页池分配情况；
	ULONG          PagedPoolFrees;              //分页池释放情况；
	ULONG          NonPagedPoolAllocs;          //非分页池分配情况；
	ULONG          NonPagedPoolFress;           //非分页池释放情况；
	ULONG          TotalFreeSystemPtes;         //系统页表项释放总数；
	ULONG          SystemCodePage;              //操作系统代码页数；
	ULONG          TotalSystemDriverPages;      //可分页驱动程序页数；
	ULONG          TotalSystemCodePages;        //操作系统代码页总数；
	ULONG          SmallNonPagedLookasideListAllocateHits; //
	ULONG          SmallPagedLookasideListAllocateHits;
	ULONG          Reserved3;                   
	ULONG          MmSystemCachePage;          //系统缓存页数；
	ULONG          PagedPoolPage;              //分页池页数；
	ULONG          SystemDriverPage;           //可分页驱动页数；
	ULONG          FastReadNoWait;             //异步快速读数目；
	ULONG          FastReadWait;               //同步快速读数目；
	ULONG          FastReadResourceMiss;       //快速读资源冲突数；
	ULONG          FastReadNotPossible;        //快速读失败数；
	ULONG          FastMdlReadNoWait;          //异步MDL快速读数目；
	ULONG          FastMdlReadWait;            //同步MDL快速读数目；
	ULONG          FastMdlReadResourceMiss;    //MDL读资源冲突数；
	ULONG          FastMdlReadNotPossible;     //MDL读失败数；
	ULONG          MapDataNoWait;              //异步映射数据次数；
	ULONG          MapDataWait;                //同步映射数据次数；
	ULONG          MapDataNoWaitMiss;          //异步映射数据冲突次数；
	ULONG          MapDataWaitMiss;            //同步映射数据冲突次数；
	ULONG          PinMappedDataCount;         //牵制映射数据数目；
	ULONG          PinReadNoWait;              //牵制异步读数目；
	ULONG          PinReadWait;                //牵制同步读数目；
	ULONG          PinReadNoWaitMiss;          //牵制异步读冲突数目；
	ULONG          PinReadWaitMiss;            //牵制同步读冲突数目；
	ULONG          CopyReadNoWait;             //异步拷贝读次数；
	ULONG          CopyReadWait;               //同步拷贝读次数；
	ULONG          CopyReadNoWaitMiss;         //异步拷贝读故障次数；
	ULONG          CopyReadWaitMiss;           //同步拷贝读故障次数；
	ULONG          MdlReadNoWait;              //异步MDL读次数；
	ULONG          MdlReadWait;                //同步MDL读次数；
	ULONG          MdlReadNoWaitMiss;          //异步MDL读故障次数；
	ULONG          MdlReadWaitMiss;            //同步MDL读故障次数；
	ULONG          ReadAheadIos;               //向前读操作数目；
	ULONG          LazyWriteIos;               //LAZY写操作数目；
	ULONG          LazyWritePages;             //LAZY写页文件数目；
	ULONG          DataFlushes;                //缓存刷新次数；
	ULONG          DataPages;                  //缓存刷新页数；
	ULONG          ContextSwitches;            //环境切换数目；
	ULONG          FirstLevelTbFills;          //第一层缓冲区填充次数；
	ULONG          SecondLevelTbFills;         //第二层缓冲区填充次数；
	ULONG          SystemCall;                 //系统调用次数；
}SYSTEM_PERFORMANCE_INFORMATION,*PSYSTEM_PERFORMANCE_INFORMATION; //对应SystemPerformanceInformation


typedef struct
{
    LARGE_INTEGER liKeBootTime;
    LARGE_INTEGER liKeSystemTime;
    LARGE_INTEGER liExpTimeZoneBias;
    ULONG         uCurrentTimeZoneId;
    DWORD         dwReserved;
} SYSTEM_TIME_INFORMATION;      //系统时间信息结构 对应SystemTimeInformation

typedef struct
{
    LARGE_INTEGER IdleTime;
    LARGE_INTEGER KernelTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER Reserved1[2];
    ULONG Reserved2;
} SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION;  //对应SystemProcessorPerformanceInformation

// 
// #define FT2D(x) ((double)((x).dwHighDateTime) * 4.294967296E9 + (double)((x).dwLowDateTime))
// 
// bool GetCPUIdelTmAndSysTm(double& dIdelTm, double& dSysTm)
// {
// 	FILETIME  stIdelTm = {0};
// 	FILETIME  stSysTm = {0};
// 	
// 	//获取cpu空闲时间
// 	if(!GetSystemTimes(&stIdelTm, NULL, NULL))
// 	{
// 		return false;
// 	}
// 	
// 	//获取cpu系统时间
// 	GetSystemTimeAsFileTime(&stSysTm);
// 	
// 	dIdelTm = FT2D(stIdelTm);
// 	dSysTm  = FT2D(stSysTm);
// 	return true;
// }


#endif

CBF_HostInfo::CBF_HostInfo()
{
	m_log.SetLogPara(LOG_PROMPT,"","hostinfo.log");
}

void CBF_HostInfo::SetLogPara(int level,const char *logfilepath,const char *logfilename,int id)
{
	m_log.SetLogPara(level,logfilepath,logfilename,id);
}
CBF_HostInfo::~CBF_HostInfo()
{

}
#ifdef _WINDOWS
__int64 CBF_HostInfo::CompareFileTime ( FILETIME time1, FILETIME time2 )  
{  
	__int64 a = time1.dwHighDateTime << 32 | time1.dwLowDateTime ;  
	__int64 b = time2.dwHighDateTime << 32 | time2.dwLowDateTime ;  
	return   (b - a);  
}  
long CBF_HostInfo::GetCpuSpeed(double &tCpuUsed)  
{   
	HANDLE hEvent;  
	BOOL res ;  
	LONG status = 0;   
	//  
	FILETIME preidleTime;  
	FILETIME prekernelTime;  
	FILETIME preuserTime;  
	//  
	FILETIME idleTime;  
	FILETIME kernelTime;  
	FILETIME userTime;  
	HINSTANCE  hKernel = LoadLibrary("Kernel32.dll");  
	if (hKernel == NULL)  
	{  
		return (GetLastError());  
	}  
	my_NtQuerySystemInformation = (MYGETSYSTIMES)GetProcAddress(hKernel,"GetSystemTimes");  
	if( my_NtQuerySystemInformation == NULL )  
	{  
		return (GetLastError());  
	}  
	res = my_NtQuerySystemInformation( &idleTime, &kernelTime, &userTime );  
	if (!res)  
	{  
		return GetLastError();  
	}  
	preidleTime = idleTime;  
	prekernelTime = kernelTime;  
	preuserTime = userTime ;  
	SLEEP(200);
	res = my_NtQuerySystemInformation( &idleTime, &kernelTime, &userTime );  
	if (!res)  
	{  
//		printf("GetSystemTimes error %d\n",GetLastError());  
		return GetLastError();
	}  
	__int64 idle = CompareFileTime( preidleTime,idleTime);  
	__int64 kernel = CompareFileTime( prekernelTime, kernelTime);  
	__int64 user = CompareFileTime(preuserTime, userTime);  

	tCpuUsed = (kernel +user - idle) *100/(kernel+user);//（总的时间-空闲时间）/总的时间=占用cpu的时间就是使用率
	if(tCpuUsed > 100)  
	{  
		tCpuUsed = 100;  
	}  
	if(tCpuUsed < 0)  
	{  
		tCpuUsed = 1;  
	}  
	return status;  
 }  

#endif
int CBF_HostInfo::GetCpu()
{
#ifdef _WINDOWS
	SYSTEM_PERFORMANCE_INFORMATION SysPerfInfo;  //系统运行信息结构
    SYSTEM_TIME_INFORMATION        SysTimeInfo;  //系统时间信息结构
    SYSTEM_BASIC_INFORMATION       SysBaseInfo;  //系统基本信息结构
	SYSTEM_PROCESSOR_TIMES         cputimes;
    double                         dbIdleTime;
    double                         dbSystemTime;
    LONG                           status;
    LARGE_INTEGER                  liOldIdleTime = {0,0};
    LARGE_INTEGER                  liOldSystemTime = {0,0};
	int UsageCpu = 0;

    NtQuerySystemInformation = (PROCNTQSI)GetProcAddress(GetModuleHandle("ntdll"),"NtQuerySystemInformation");
	
    if (NULL == NtQuerySystemInformation)
	{
		m_log.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"取得ntdll的NtQuerySystemInformation方法出错");
        return 0;
	}
    status = NtQuerySystemInformation(SystemBasicInformation,&SysBaseInfo,sizeof(SysBaseInfo),NULL);
    if (status != NO_ERROR)
	{
		m_log.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Get CPURadio Get SystemBasicInformation error!");
        return 0;
    }
	status = NtQuerySystemInformation(SystemProcessInformation,&cputimes,sizeof(cputimes),NULL);
    if (status != NO_ERROR)
	{
		m_log.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"WIN2008 Get CPURadio Get SystemProcessInformation error!");
		double cpurate=0.0;
		GetCpuSpeed(cpurate);
		return (int)cpurate;
	}
	INT64_ alltime = cputimes.IdleTime.QuadPart+cputimes.UserTime.QuadPart+cputimes.InterruptTime.QuadPart+cputimes.KernelTime.QuadPart;
	INT64_ usetime = cputimes.UserTime.QuadPart+cputimes.InterruptTime.QuadPart+cputimes.KernelTime.QuadPart;
	UsageCpu =100*usetime/alltime;
	m_log.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"CPU使用率 %d%s",UsageCpu,"%");
	return UsageCpu;
#else
	char sWorkBuffer[500];
	memset(sWorkBuffer, 0, sizeof(sWorkBuffer));
	int iCount = 0;
	char sTemp[20][100];
	memset(sTemp,0,sizeof(sTemp));
	FILE * fp = popen("vmstat 1 2","r");
	if (fp == NULL)
	{
		m_log.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Get CPURadio error");
		return -1;
	}
	else
	{		
		while(!feof(fp))
		{
			memset(sWorkBuffer,0,sizeof(sWorkBuffer));
			fgets(sWorkBuffer, sizeof(sWorkBuffer)-1, fp);
#ifdef LINUX
			if(iCount == 3)
			{
				sscanf(sWorkBuffer,"%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",sTemp[0],sTemp[1],sTemp[2],sTemp[3],sTemp[4],sTemp[5],\
					sTemp[6],sTemp[7],sTemp[8],sTemp[9],sTemp[10],sTemp[11],sTemp[12],sTemp[13],sTemp[14],\
					sTemp[15],sTemp[16]);	
				
				pclose(fp);
				m_log.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"CPU使用率 %d%s",100-atoi(sTemp[14]),"%");
				return 100-atoi(sTemp[14]);
			}
#elif AIX
			if(iCount == 6)
			{
				sscanf(sWorkBuffer,"%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",sTemp[0],sTemp[1],sTemp[2],sTemp[3],sTemp[4],sTemp[5],\
					sTemp[6],sTemp[7],sTemp[8],sTemp[9],sTemp[10],sTemp[11],sTemp[12],sTemp[13],sTemp[14],\
					sTemp[15],sTemp[16]);	

				pclose(fp);
				m_log.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"CPU使用率 %d%s",100-atoi(sTemp[15]),"%");
				
				return 100-atoi(sTemp[15]);
			}
#else  //HP_UX
			if(iCount == 3)
			{
				sscanf(sWorkBuffer,"%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",sTemp[0],sTemp[1],sTemp[2],sTemp[3],sTemp[4],sTemp[5],\
					sTemp[6],sTemp[7],sTemp[8],sTemp[9],sTemp[10],sTemp[11],sTemp[12],sTemp[13],sTemp[14],\
					sTemp[15],sTemp[16],sTemp[17]);	
				
				pclose(fp);
				m_log.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"CPU使用率 %d%s",100-atoi(sTemp[17]),"%");
	
				return 100-atoi(sTemp[17]);
				
			}
#endif
			iCount++;		
		}
		pclose(fp);
	}
	return 	iCount;	
#endif

}

int CBF_HostInfo::GetEms(S_MemoryInf &smInfo)
{
#ifdef _WINDOWS
	



	SYSTEM_PERFORMANCE_INFORMATION SysPerfInfo;  //系统运行信息结构
    SYSTEM_TIME_INFORMATION        SysTimeInfo;  //系统时间信息结构
    SYSTEM_BASIC_INFORMATION       SysBaseInfo;  //系统基本信息结构
    SYSTEM_INFO sys_info;
	LONG  status;
    NtQuerySystemInformation = (PROCNTQSI)GetProcAddress(GetModuleHandle("ntdll"),"NtQuerySystemInformation");
	
    if (NULL == NtQuerySystemInformation)
	{
		m_log.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"取得ntdll的NtQuerySystemInformation方法出错");
        return -1;
	}
	GetSystemInfo(&sys_info);
	
	status = NtQuerySystemInformation(SystemBasicInformation,&SysBaseInfo,sizeof(SYSTEM_BASIC_INFORMATION),NULL);
	if (status != NO_ERROR)
	{
		m_log.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Get EMS error win7/2008 ");
		return -1;
	}
	smInfo.EmsTotal = (INT64_)SysBaseInfo.uMmNumberOfPhysicalPages*SysBaseInfo.uPageSize/1024/1024; //M
//	printf("%d %d total=%d\n",SysBaseInfo.uMmNumberOfPhysicalPages,SysBaseInfo.uPageSize,smInfo.EmsTotal);

	MEMORYSTATUS memStatus;
	GlobalMemoryStatus(&memStatus);
	INT64_ tom=memStatus.dwTotalPhys/1024/1024;
	INT64_ mem=memStatus.dwAvailPhys/1024/1024;
	//	DWORD res=memStatus.dwAvailVirtual/1024;
	
	//	printf("dwTotalPhys=%dMB dwAvailPhys=%dMB dwMemoryLoad=%d%s\n",tom/1024,mem/1024,memStatus.dwMemoryLoad,"%");
	
	smInfo.EmsUse = smInfo.EmsTotal*memStatus.dwMemoryLoad/100;
	smInfo.EmsUseRadio = memStatus.dwMemoryLoad;
	
	//	printf("总内存=%dM 使用内存=%dM 内存使用率%d%s\n",smInfo.EmsTotal,smInfo.EmsUse,smInfo.EmsUseRadio,"%");
	m_log.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"总内存=%dM 使用内存=%dM 内存使用率%d%s",smInfo.EmsTotal,smInfo.EmsUse,smInfo.EmsUseRadio,"%");
	return 0;

// 
// 	status = NtQuerySystemInformation(SystemPerformanceInformation,&SysPerfInfo,sizeof(SYSTEM_PERFORMANCE_INFORMATION),NULL);
// 	if (status != NO_ERROR)
// 	{
// 		m_log.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Get EMS error win7/2008 ");
// 		return -1;
// 	}
// 	
// 	INT64_ unusemem = SysPerfInfo.AvailablePages*SysBaseInfo.uPageSize/1024/1024;//mb
// 	smInfo.EmsUse = smInfo.EmsTotal - unusemem;
// 	smInfo.EmsUseRadio = 100*smInfo.EmsUse/smInfo.EmsTotal;
// 	return 0;

#elif LINUX
	char sWorkBuffer[500];
	memset(sWorkBuffer, 0, sizeof(sWorkBuffer));

	int iCount = 0;
	char sTemp1[100];
	char sTemp2[100];
	char sTemp3[100];
	char sTemp4[100];
	char sTemp5[100];
	char sTemp6[100];
	char sTemp7[100];
	memset(sTemp1,0,sizeof(sTemp1));
	memset(sTemp2,0,sizeof(sTemp2));
	memset(sTemp3,0,sizeof(sTemp3));
	memset(sTemp4,0,sizeof(sTemp4));
	memset(sTemp5,0,sizeof(sTemp5));
	memset(sTemp6,0,sizeof(sTemp6));
	memset(sTemp7,0,sizeof(sTemp7));

	FILE * fp = popen("free","r");

	if (fp == NULL)
	{
		return -1;
	}
	else
	{
		while(!feof(fp))
		{
			memset(sWorkBuffer,0,sizeof(sWorkBuffer));
			fgets(sWorkBuffer, sizeof(sWorkBuffer)-1, fp);
			if(iCount>0 && strlen(sWorkBuffer)>1)
			{
				//memset(sWorkBuffer,0,sizeof(sWorkBuffer));
				memset(sTemp1,0,sizeof(sTemp1));
				memset(sTemp2,0,sizeof(sTemp2));
				memset(sTemp3,0,sizeof(sTemp3));
				memset(sTemp4,0,sizeof(sTemp4));
				memset(sTemp5,0,sizeof(sTemp5));
				memset(sTemp6,0,sizeof(sTemp6));
				memset(sTemp7,0,sizeof(sTemp7));
				sscanf(sWorkBuffer,"%s%s%s%s%s%s%s",sTemp1,sTemp2,sTemp3,sTemp4,sTemp5,sTemp6,sTemp7);
				//printf("sTemp1=%s\nsTemp2=%s\nsTemp3=%s\nsTemp4=%s\nsTemp5=%s\nsTemp6=%s\nsTemp7=%s\n"
				//	,sTemp1,sTemp2,sTemp3,sTemp4,sTemp5,sTemp6,sTemp7);
				smInfo.EmsTotal = atoi(sTemp2)/1024;
				
				
				//20151230 modi 改为可用=free +buffer +cache
				//smInfo.EmsUse = atoi(sTemp3)/1024;
				//smInfo.EmsUse = smInfo.EmsTotal - atoi(sTemp4)/1024 - atoi(sTemp6)/1024 -atoi(sTemp7)/1024;
				smInfo.EmsUse = smInfo.EmsTotal - atoi(sTemp7) / 1024;

				smInfo.EmsUseRadio = 100*(smInfo.EmsUse)/(smInfo.EmsTotal);
//				printf("总内存=%dM 使用内存=%dM 内存使用率%d%s\n",smInfo.EmsTotal,smInfo.EmsUse,smInfo.EmsUseRadio,"%");
				m_log.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"总内存=%dM 使用内存=%dM 内存使用率%d%s",smInfo.EmsTotal,smInfo.EmsUse,smInfo.EmsUseRadio,"%");
				
				break;
			}
			iCount++;
		}	
	}
	pclose(fp);
#elif  AIX   //AIX
	char sWorkBuffer[500];
	memset(sWorkBuffer, 0, sizeof(sWorkBuffer));
	int iCount = 0;
	char sTemp[20][100];
	char tmpchar[100];
	memset(tmpchar,0,sizeof(tmpchar));
	memset(sTemp,0,sizeof(sTemp));
	FILE * fp = popen("vmstat 1 1","r");
	if (fp == NULL)
	{
//		printf("Get CPURadio error!\n");
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"vmstat 1 1 错误");
		return -1;
	}
	else
	{		
		while(!feof(fp))
		{
			memset(sWorkBuffer,0,sizeof(sWorkBuffer));
			fgets(sWorkBuffer, sizeof(sWorkBuffer)-1, fp);
			//ltrim(sWorkBuffer);
			//printf("line%d = %s\n",iCount,sWorkBuffer);
			if(iCount == 1)
			{
				sscanf(sWorkBuffer,"%s%s%s%s",sTemp[0],sTemp[1],sTemp[2],sTemp[3]);	
				memset(tmpchar,0,sizeof(tmpchar));
				CBF_Tools::LRtrim(sTemp[2]);
				CBF_Tools::LRtrim(sTemp[3]);
				if (strlen(sTemp[3])<6)
				{
					m_log.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"vmstat 1 1 总内存内容 %s",sWorkBuffer);
					if (strlen(sTemp[2])<6)
					{
						if (strlen(sTemp[1])<6)
						{
//							printf("Get CPURadio error %s!\n",sWorkBuffer);
							m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"vmstat 1 1 错误 %s",sWorkBuffer);
							pclose(fp);
							return -1;
						}
						strncpy(tmpchar,sTemp[1]+4,strlen(sTemp[1])-6);
					}
					else
					{
						strncpy(tmpchar,sTemp[2]+4,strlen(sTemp[2])-6);
					}
				}
				else
				{
					strncpy(tmpchar,sTemp[3]+4,strlen(sTemp[3])-6);
				}
				
				//printf("总内存%s %sM",sTemp[3],tmpchar);
				smInfo.EmsTotal = atoi(tmpchar);
				if (smInfo.EmsTotal == 0)
				{
					m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"总内存错误 %s",tmpchar);
					pclose(fp);
					return -1;
				}
			}
			if(iCount == 6)
			{
				//printf("line%d = %s\n",iCount,sWorkBuffer);
				sscanf(sWorkBuffer,"%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",sTemp[0],sTemp[1],sTemp[2],sTemp[3],sTemp[4],sTemp[5],\
					sTemp[6],sTemp[7],sTemp[8],sTemp[9],sTemp[10],sTemp[11],sTemp[12],sTemp[13],sTemp[14],\
					sTemp[15],sTemp[16]);	
				int freemem= atoi(sTemp[3])/1024*4;
				smInfo.EmsUse = smInfo.EmsTotal - freemem;
				smInfo.EmsUseRadio = 100*(smInfo.EmsUse)/smInfo.EmsTotal;
				m_log.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"总内存=%dM 使用内存=%dM 内存使用率%d%s",smInfo.EmsTotal,smInfo.EmsUse,smInfo.EmsUseRadio,"%");
				pclose(fp);
				return 0;
			}
			iCount++;		
		}
		pclose(fp);
	}
#else
	
	char sWorkBuffer[500];
	memset(sWorkBuffer, 0, sizeof(sWorkBuffer));
	
	int iCount = 0;
	char sTemp[20][100];
	char tmpchar[100];
	memset(tmpchar,0,sizeof(tmpchar));
	memset(sTemp,0,sizeof(sTemp));

	FILE * fp = popen("swapinfo -m","r");
	
	if (fp == NULL)
	{
		m_log.LogMp(LOG_ERROR,__FILE__,__LINE__,"Get CPURadio error");
		return -1;
	}
	else
	{		
		while(!feof(fp))
		{
			memset(sWorkBuffer,0,sizeof(sWorkBuffer));
			fgets(sWorkBuffer, sizeof(sWorkBuffer)-1, fp);
			if(iCount == 4)
			{
				sscanf(sWorkBuffer,"%s%s%s%s%s",sTemp[0],sTemp[1],sTemp[2],sTemp[3],sTemp[4]);	
				smInfo.EmsTotal = atoi(sTemp[1]);

				int freemem= atoi(sTemp[3]);
				smInfo.EmsUse = atoi(sTemp[2]);
				smInfo.EmsUseRadio = 100*(smInfo.EmsUse)/smInfo.EmsTotal;
//				printf("使用内存 %dM",smInfo.EmsUse);
//				printf("内存使用率 %d%s\n",100*(smInfo.EmsUse)/smInfo.EmsTotal,"%");
				m_log.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"总内存=%dM 使用内存=%dM 内存使用率%d%s",smInfo.EmsTotal,smInfo.EmsUse,smInfo.EmsUseRadio,"%");
				pclose(fp);
				return 0;
			}
			iCount++;		
		}
		pclose(fp);
	}
#endif
	return 0;
}

/*
功能:得到硬盘使用情况
参数:无
返回值:硬盘信息结构体
*/
int CBF_HostInfo::GetDisk(V_DiskInfo &vmDisINFO)
{
	S_DiskInf tmp;
#ifdef _WINDOWS
	ULARGE_INTEGER FreeAvailable,TotalNum,TotalFreeNum;
	
	char p[3];
	bool b_flag;
	
	tmp.TotalSpace = 0;
	tmp.UsedSpace = 0;

//	vDiskInfoStr.clear();
	//得到有效的驱动器名,即盘符 
	for( int drive = 1; drive <= 26; drive++ )
	{
		if( !_chdrive( drive ) )
		{
			memset( p , 0 , sizeof(p));
			p[0] = drive + 'A' - 1;
			p[1] = ':';
			p[2] = '\0';
			b_flag = GetDiskFreeSpaceEx( p ,&FreeAvailable,&TotalNum,&TotalFreeNum );
			if( b_flag )
			{
				tmp.cIndex[0] = p[0];
				tmp.cIndex[1] = p[2];
				tmp.TotalSpace = (int)(TotalNum.QuadPart/1024/(1024));
				int FreeSpace  = (int)(FreeAvailable.QuadPart/1024/(1024));
				tmp.UsedSpace  = tmp.TotalSpace - FreeSpace;
				tmp.DiskUseRadio = 100.00*tmp.UsedSpace/tmp.TotalSpace;
//				printf("磁盘情况 %s:总大小%dM 使用大小%dM 使用率%.2f%s \n",tmp.cIndex,tmp.TotalSpace,tmp.UsedSpace,tmp.DiskUseRadio,"%");
				m_log.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"磁盘情况 %s:总大小%dM 使用大小%dM 使用率%.2f%s ",tmp.cIndex,tmp.TotalSpace,tmp.UsedSpace,tmp.DiskUseRadio,"%");
				
				vmDisINFO.push_back(tmp);
			}
		}
	}
#elif LINUX
	char sWorkBuffer[500];
	memset(sWorkBuffer, 0, sizeof(sWorkBuffer));
	int iCount = 0;
	char sTemp1[100];
	char sTemp2[100];
	char sTemp3[100];
	char sTemp4[100];
	char sTemp5[100];
	char sTemp6[100];
	memset(sTemp1,0,sizeof(sTemp1));
	memset(sTemp2,0,sizeof(sTemp2));
	memset(sTemp3,0,sizeof(sTemp3));
	memset(sTemp4,0,sizeof(sTemp4));
	memset(sTemp5,0,sizeof(sTemp5));
	memset(sTemp6,0,sizeof(sTemp6));

	FILE * fp = popen("df -m","r");
	if (fp == NULL)
	{
//		printf("popen df -m error!\n");
		m_log.LogMp(LOG_ERROR,__FILE__,__LINE__,"popen df -m error!");
		return -1;
	}
	else
	{
		while(!feof(fp))
		{
			memset(sWorkBuffer,0,sizeof(sWorkBuffer));
			fgets(sWorkBuffer, sizeof(sWorkBuffer)-1, fp);

			if(iCount>0 && strlen(sWorkBuffer)>1)
			{
				memset(sTemp1,0,sizeof(sTemp1));
				memset(sTemp2,0,sizeof(sTemp2));
				memset(sTemp3,0,sizeof(sTemp3));
				memset(sTemp4,0,sizeof(sTemp4));
				memset(sTemp5,0,sizeof(sTemp5));
				memset(sTemp6,0,sizeof(sTemp6));
				CBF_Tools::Ltrim(sWorkBuffer);
				sscanf(sWorkBuffer,"%s%s%s%s%s%s",sTemp1,sTemp2,sTemp3,sTemp4,sTemp5,sTemp6);
				while(strcmp(sTemp2, "") == 0)
				{
					//printf("11111111111");
					memset(sWorkBuffer,0,sizeof(sWorkBuffer));
					fgets(sWorkBuffer, sizeof(sWorkBuffer), fp);
					//char Temp[100];
					memset(sTemp2,0,sizeof(sTemp2));
					memset(sTemp3,0,sizeof(sTemp3));
					memset(sTemp4,0,sizeof(sTemp4));
					memset(sTemp5,0,sizeof(sTemp5));
					memset(sTemp6,0,sizeof(sTemp6));
					//memset(Temp,0,sizeof(Temp));
					CBF_Tools::Ltrim(sWorkBuffer);
					sscanf(sWorkBuffer,"%s%s%s%s%s",sTemp2,sTemp3,sTemp4,sTemp5,sTemp6);
				}

				//	printf("sTemp1=%s\nsTemp2=%s\nsTemp3=%s\nsTemp4=%s\nsTemp5=%s\nsTemp6=%s\n"
				//	,sTemp1,sTemp2,sTemp3,sTemp4,sTemp5,sTemp6);

				strcpy(tmp.cIndex, sTemp6);
				tmp.TotalSpace = atoll(sTemp2);
				tmp.UsedSpace = atoll(sTemp3);

				int len =strlen(sTemp5);
				sTemp5[len-1]='\0';

				tmp.DiskUseRadio = atof(sTemp5);
//				printf("磁盘情况 %s:总大小%dM 使用大小%dM 使用率%.2f%s \n",tmp.cIndex,tmp.TotalSpace,tmp.UsedSpace,tmp.DiskUseRadio,"%");
				m_log.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"磁盘情况 %s:总大小%dM 使用大小%dM 使用率%.2f%s ",tmp.cIndex,tmp.TotalSpace,tmp.UsedSpace,tmp.DiskUseRadio,"%");
				vmDisINFO.push_back(tmp);

			}
			iCount++;
		}
		pclose(fp);
	}
#elif AIX //AIX
	char sWorkBuffer[500];
	memset(sWorkBuffer, 0, sizeof(sWorkBuffer));
	int iCount = 0;
	char sTemp1[100];
	char sTemp2[100];
	char sTemp3[100];
	char sTemp4[100];
	char sTemp5[100];
	char sTemp6[100];
	char sTemp7[100];
	memset(sTemp1,0,sizeof(sTemp1));
	memset(sTemp2,0,sizeof(sTemp2));
	memset(sTemp3,0,sizeof(sTemp3));
	memset(sTemp4,0,sizeof(sTemp4));
	memset(sTemp5,0,sizeof(sTemp5));
	memset(sTemp6,0,sizeof(sTemp6));
	memset(sTemp7,0,sizeof(sTemp7));
	FILE * fp = popen("df -m","r");
	if (fp == NULL)
	{
//		printf("popen df -m error!\n");
		m_log.LogMp(LOG_ERROR,__FILE__,__LINE__,"popen df -m error!");
		return -1;
	}
	else
	{
		while(!feof(fp))
		{
			memset(sWorkBuffer,0,sizeof(sWorkBuffer));
			fgets(sWorkBuffer, sizeof(sWorkBuffer)-1, fp);
			
			if(iCount>0 && strlen(sWorkBuffer)>1)
			{
				memset(sTemp1,0,sizeof(sTemp1));
				memset(sTemp2,0,sizeof(sTemp2));
				memset(sTemp3,0,sizeof(sTemp3));
				memset(sTemp4,0,sizeof(sTemp4));
				memset(sTemp5,0,sizeof(sTemp5));
				memset(sTemp6,0,sizeof(sTemp6));
				memset(sTemp7,0,sizeof(sTemp7));
				CBF_Tools::Ltrim(sWorkBuffer);
				sscanf(sWorkBuffer,"%s%s%s%s%s%s%s",sTemp1,sTemp2,sTemp3,sTemp4,sTemp5,sTemp6,sTemp7);
				strcpy(tmp.cIndex, sTemp7);
				tmp.TotalSpace = atoll(sTemp2);
				tmp.UsedSpace = tmp.TotalSpace - atoll(sTemp3);

				int len =strlen(sTemp4);
				sTemp4[len-1]='\0';

				tmp.DiskUseRadio = atof(sTemp4);
//				printf("磁盘情况 %s:总大小%dM 使用大小%dM 使用率%.2f%s \n",tmp.cIndex,tmp.TotalSpace,tmp.UsedSpace,tmp.DiskUseRadio,"%");
				m_log.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"磁盘情况 %s:总大小%dM 使用大小%dM 使用率%.2f%s ",tmp.cIndex,tmp.TotalSpace,tmp.UsedSpace,tmp.DiskUseRadio,"%");
				vmDisINFO.push_back(tmp);
			}
			iCount++;
		}
		pclose(fp);
	}
#else  //HP_UX
	char sWorkBuffer[500];
	memset(sWorkBuffer, 0, sizeof(sWorkBuffer));
	
	int iCount = 0;
	char sTemp1[100];
	char sTemp2[100];
	char sTemp3[100];
	char sTemp4[100];
	char sTemp5[100];
	char sTemp6[100];
	char sTemp7[100];
	memset(sTemp1,0,sizeof(sTemp1));
	memset(sTemp2,0,sizeof(sTemp2));
	memset(sTemp3,0,sizeof(sTemp3));
	memset(sTemp4,0,sizeof(sTemp4));
	memset(sTemp5,0,sizeof(sTemp5));
	memset(sTemp6,0,sizeof(sTemp6));
	memset(sTemp7,0,sizeof(sTemp7));
	
	FILE * fp = popen("df -k","r");
	
	if (fp == NULL)
	{
		m_log.LogMp(LOG_ERROR,__FILE__,__LINE__,"popen df -k error!");
//		printf("popen df -k error!\n");
		return -1;
	}
	else
	{
		bool isinfo=false;
		while(!feof(fp))
		{
			memset(sWorkBuffer,0,sizeof(sWorkBuffer));
			fgets(sWorkBuffer, sizeof(sWorkBuffer)-1, fp);
			CBF_Tools::Ltrim(sWorkBuffer);
			if (sWorkBuffer[0] == '/')
			{
				isinfo = true;
				iCount=0;
			}
			if(isinfo)
			{
				memset(sTemp1,0,sizeof(sTemp1));
				memset(sTemp2,0,sizeof(sTemp2));
				memset(sTemp3,0,sizeof(sTemp3));
				memset(sTemp4,0,sizeof(sTemp4));
				memset(sTemp5,0,sizeof(sTemp5));
				if (iCount == 0)
				{
					sscanf(sWorkBuffer,"%s%s%s%s%s",sTemp1,sTemp2,sTemp3,sTemp4,sTemp5);
					strcpy(tmp.cIndex, sTemp1);
					tmp.TotalSpace = atoll(sTemp5)/1024;
				}
				else if (iCount == 1) //空闲
				{
	
				}
				else if (iCount == 2) //使用
				{
					sscanf(sWorkBuffer,"%s",sTemp1);
					tmp.UsedSpace = atoll(sTemp1)/1024;
				}
				else //使用率
				{
					sscanf(sWorkBuffer,"%s",sTemp1);
					tmp.DiskUseRadio = atof(sTemp1);
					if (tmp.TotalSpace>0)
					{
						vmDisINFO.push_back(tmp);
					}
					
//					printf("磁盘情况 %s:总大小%dM 使用大小%dM 使用率%.2f%s \n",tmp.cIndex,tmp.TotalSpace,tmp.UsedSpace,tmp.DiskUseRadio,"%");
					m_log.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"磁盘情况 %s:总大小%dM 使用大小%dM 使用率%.2f%s ",tmp.cIndex,tmp.TotalSpace,tmp.UsedSpace,tmp.DiskUseRadio,"%");
					isinfo = false;
				}
				iCount++;
			}
		}
		pclose(fp);
	}

#endif
	return 0;
}
