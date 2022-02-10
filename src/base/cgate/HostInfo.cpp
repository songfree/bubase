// HostInfo.cpp: implementation of the CHostInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "HostInfo.h"
#include "BF_Tools.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#ifdef _WINDOWS
#include <windows.h>

#define SystemBasicInformation       0   //系统基本信息
#define SystemPerformanceInformation 2   //系统运行信息
#define SystemTimeInformation        3   //系统时间信息
#define Li2Double(x) ((double)((x).HighPart) * 4.294967296E9 + (double)((x).LowPart))   //???????????

typedef LONG (WINAPI *PROCNTQSI)(UINT,PVOID,ULONG,PULONG);
PROCNTQSI NtQuerySystemInformation;

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
} SYSTEM_BASIC_INFORMATION;         //系统基本信息结构

typedef struct
{
    LARGE_INTEGER   liIdleTime;
    DWORD           dwSpare[76];
} SYSTEM_PERFORMANCE_INFORMATION;    //系统运行信息结构

typedef struct
{
    LARGE_INTEGER liKeBootTime;
    LARGE_INTEGER liKeSystemTime;
    LARGE_INTEGER liExpTimeZoneBias;
    ULONG         uCurrentTimeZoneId;
    DWORD         dwReserved;
} SYSTEM_TIME_INFORMATION;      //系统时间信息结构

#endif

CHostInfo::CHostInfo()
{
	m_log = NULL;
	
}

CHostInfo::~CHostInfo()
{

}
int CHostInfo::GetCpu()
{
#ifdef _WINDOWS
	SYSTEM_PERFORMANCE_INFORMATION SysPerfInfo;  //系统运行信息结构
    SYSTEM_TIME_INFORMATION        SysTimeInfo;  //系统时间信息结构
    SYSTEM_BASIC_INFORMATION       SysBaseInfo;  //系统基本信息结构
    double                         dbIdleTime;
    double                         dbSystemTime;
    LONG                           status;
    LARGE_INTEGER                  liOldIdleTime = {0,0};
    LARGE_INTEGER                  liOldSystemTime = {0,0};
	int UsageCpu = 0;

    NtQuerySystemInformation = (PROCNTQSI)GetProcAddress(GetModuleHandle("ntdll"),"NtQuerySystemInformation");
	
    if (NULL == NtQuerySystemInformation)
	{
		m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"取得ntdll的NtQuerySystemInformation方法出错");
        return 0;
	}
    status = NtQuerySystemInformation(SystemBasicInformation,&SysBaseInfo,sizeof(SysBaseInfo),NULL);
    if (status != NO_ERROR)
	{
        return 0;
    }
	for( int t = 0 ; t < 2 ; t++ )
    {
		status = NtQuerySystemInformation(SystemTimeInformation,&SysTimeInfo,sizeof(SysTimeInfo),0);
        if (status!=NO_ERROR)
		{
            return 0;
		}
        status = NtQuerySystemInformation(SystemPerformanceInformation,&SysPerfInfo,sizeof(SysPerfInfo),NULL);
        if (status != NO_ERROR)
		{
            return 0;
		}
		if (liOldIdleTime.QuadPart != 0)
		{
            dbIdleTime = Li2Double(SysPerfInfo.liIdleTime) - Li2Double(liOldIdleTime);
            dbSystemTime = Li2Double(SysTimeInfo.liKeSystemTime) - Li2Double(liOldSystemTime);
            dbIdleTime = dbIdleTime / dbSystemTime;
            dbIdleTime = 100.0 - dbIdleTime * 100.0 / (double)SysBaseInfo.bKeNumberProcessors + 0.5;
			UsageCpu = (int)dbIdleTime;
		}
        // store new CPU's idle and system time
        liOldIdleTime = SysPerfInfo.liIdleTime;
        liOldSystemTime = SysTimeInfo.liKeSystemTime;
        // wait one second
        SLEEP(500);
    }
//	printf("CPU使用率 %d%s\n",UsageCpu,"%");
	m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"CPU使用率 %d%s",UsageCpu,"%");
	return UsageCpu;
#else
	char sWorkBuffer[500];
	memset(sWorkBuffer, 0, sizeof(sWorkBuffer));
	int iCount = 0;
	char sTemp[20][100];
	memset(sTemp,0,sizeof(sTemp));
	FILE * fp = popen("vmstat 1 1","r");
	if (fp == NULL)
	{
//		printf("Get CPURadio error!\n");
		m_log->LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"Get CPURadio error");
		return -1;
	}
	else
	{		
		while(!feof(fp))
		{
			memset(sWorkBuffer,0,sizeof(sWorkBuffer));
			fgets(sWorkBuffer, sizeof(sWorkBuffer)-1, fp);
#ifdef LINUX
			if(iCount == 2)
			{
				sscanf(sWorkBuffer,"%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",sTemp[0],sTemp[1],sTemp[2],sTemp[3],sTemp[4],sTemp[5],\
					sTemp[6],sTemp[7],sTemp[8],sTemp[9],sTemp[10],sTemp[11],sTemp[12],sTemp[13],sTemp[14],\
					sTemp[15],sTemp[16]);	
				
				pclose(fp);
//				printf("CPU使用率 %d%s\n",100-atoi(sTemp[14]),"%");
				m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"CPU使用率 %d%s",100-atoi(sTemp[14]),"%");
				return 100-atoi(sTemp[14]);
			}
#elif AIX
			if(iCount == 6)
			{
				sscanf(sWorkBuffer,"%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",sTemp[0],sTemp[1],sTemp[2],sTemp[3],sTemp[4],sTemp[5],\
					sTemp[6],sTemp[7],sTemp[8],sTemp[9],sTemp[10],sTemp[11],sTemp[12],sTemp[13],sTemp[14],\
					sTemp[15],sTemp[16]);	

				pclose(fp);
//				printf("CPU使用率 %d%s\n",100-atoi(sTemp[15]),"%");
				m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"CPU使用率 %d%s",100-atoi(sTemp[15]),"%");
				
				return 100-atoi(sTemp[15]);
			}
#else  //HP_UX
			if(iCount == 2)
			{
				sscanf(sWorkBuffer,"%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",sTemp[0],sTemp[1],sTemp[2],sTemp[3],sTemp[4],sTemp[5],\
					sTemp[6],sTemp[7],sTemp[8],sTemp[9],sTemp[10],sTemp[11],sTemp[12],sTemp[13],sTemp[14],\
					sTemp[15],sTemp[16],sTemp[17]);	
				
				pclose(fp);
//				printf("CPU使用率 %d%s\n",100-atoi(sTemp[17]),"%");
				m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"CPU使用率 %d%s",100-atoi(sTemp[17]),"%");
				
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

int CHostInfo::GetEms(S_MemoryInf &smInfo)
{
#ifdef _WINDOWS
	MEMORYSTATUS memStatus;
	GlobalMemoryStatus(&memStatus);
	DWORD tom=memStatus.dwTotalPhys/1024;
	DWORD mem=memStatus.dwAvailPhys/1024;
//	DWORD res=memStatus.dwAvailVirtual/1024;

	smInfo.EmsTotal = (int)tom/1024;
	smInfo.EmsUse = smInfo.EmsTotal-(int)mem/1024;  //使用等于可用减去还能使用的
	smInfo.EmsUseRadio = 100*(smInfo.EmsUse)/(smInfo.EmsTotal);

//	printf("总内存=%dM 使用内存=%dM 内存使用率%d%s\n",smInfo.EmsTotal,smInfo.EmsUse,smInfo.EmsUseRadio,"%");
	m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"总内存=%dM 使用内存=%dM 内存使用率%d%s",smInfo.EmsTotal,smInfo.EmsUse,smInfo.EmsUseRadio,"%");
	
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
				smInfo.EmsUse = atoi(sTemp3)/1024;

				smInfo.EmsUseRadio = 100*(smInfo.EmsUse)/(smInfo.EmsTotal);
//				printf("总内存=%dM 使用内存=%dM 内存使用率%d%s\n",smInfo.EmsTotal,smInfo.EmsUse,smInfo.EmsUseRadio,"%");
				m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"总内存=%dM 使用内存=%dM 内存使用率%d%s",smInfo.EmsTotal,smInfo.EmsUse,smInfo.EmsUseRadio,"%");
				
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
		m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"vmstat 1 1 错误");
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
					m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"vmstat 1 1 总内存内容 %s",sWorkBuffer);
					if (strlen(sTemp[2])<6)
					{
						if (strlen(sTemp[1])<6)
						{
//							printf("Get CPURadio error %s!\n",sWorkBuffer);
							m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"vmstat 1 1 错误 %s",sWorkBuffer);
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
					m_log->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"总内存错误 %s",tmpchar);
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
// 				if (m_log.isWrite(LOG_DEBUG))
// 				{
// 					printf("总内存%dM",smInfo.EmsTotal);
// 					printf("空闲内存%dM",freemem);
// 					
// 				}
				smInfo.EmsUse = smInfo.EmsTotal - freemem;
				smInfo.EmsUseRadio = 100*(smInfo.EmsUse)/smInfo.EmsTotal;
//				printf("使用内存 %dM",smInfo.EmsUse);
//				printf("内存使用率 %d%s\n",100*(smInfo.EmsUse)/smInfo.EmsTotal,"%");
				m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"总内存=%dM 使用内存=%dM 内存使用率%d%s",smInfo.EmsTotal,smInfo.EmsUse,smInfo.EmsUseRadio,"%");
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
//		printf("Get CPURadio error!\n");
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"Get CPURadio error");
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
// 				if (m_log.isWrite(LOG_DEBUG))
// 				{
// 					printf("总内存%dM",smInfo.EmsTotal);
// 					printf("空闲内存%dM",freemem);
// 				}
				smInfo.EmsUse = atoi(sTemp[2]);
				smInfo.EmsUseRadio = 100*(smInfo.EmsUse)/smInfo.EmsTotal;
//				printf("使用内存 %dM",smInfo.EmsUse);
//				printf("内存使用率 %d%s\n",100*(smInfo.EmsUse)/smInfo.EmsTotal,"%");
				m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"总内存=%dM 使用内存=%dM 内存使用率%d%s",smInfo.EmsTotal,smInfo.EmsUse,smInfo.EmsUseRadio,"%");
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
int CHostInfo::GetDisk(V_DiskInfo &vmDisINFO)
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
				m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"磁盘情况 %s:总大小%dM 使用大小%dM 使用率%.2f%s ",tmp.cIndex,tmp.TotalSpace,tmp.UsedSpace,tmp.DiskUseRadio,"%");
				
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
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"popen df -m error!");
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
				tmp.TotalSpace = atoi(sTemp2);
				tmp.UsedSpace = atoi(sTemp3);

				int len =strlen(sTemp5);
				sTemp5[len-1]='\0';

				tmp.DiskUseRadio = atof(sTemp5);
//				printf("磁盘情况 %s:总大小%dM 使用大小%dM 使用率%.2f%s \n",tmp.cIndex,tmp.TotalSpace,tmp.UsedSpace,tmp.DiskUseRadio,"%");
				m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"磁盘情况 %s:总大小%dM 使用大小%dM 使用率%.2f%s ",tmp.cIndex,tmp.TotalSpace,tmp.UsedSpace,tmp.DiskUseRadio,"%");
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
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"popen df -m error!");
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
				tmp.TotalSpace = atoi(sTemp2);
				tmp.UsedSpace = tmp.TotalSpace - atoi(sTemp3);

				int len =strlen(sTemp4);
				sTemp4[len-1]='\0';

				tmp.DiskUseRadio = atof(sTemp4);
//				printf("磁盘情况 %s:总大小%dM 使用大小%dM 使用率%.2f%s \n",tmp.cIndex,tmp.TotalSpace,tmp.UsedSpace,tmp.DiskUseRadio,"%");
				m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"磁盘情况 %s:总大小%dM 使用大小%dM 使用率%.2f%s ",tmp.cIndex,tmp.TotalSpace,tmp.UsedSpace,tmp.DiskUseRadio,"%");
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
		m_log->LogMp(LOG_ERROR,__FILE__,__LINE__,"popen df -k error!");
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
					tmp.TotalSpace = atoi(sTemp5)/1024;
				}
				else if (iCount == 1) //空闲
				{
	
				}
				else if (iCount == 2) //使用
				{
					sscanf(sWorkBuffer,"%s",sTemp1);
					tmp.UsedSpace = atoi(sTemp1)/1024;
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
					m_log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"磁盘情况 %s:总大小%dM 使用大小%dM 使用率%.2f%s ",tmp.cIndex,tmp.TotalSpace,tmp.UsedSpace,tmp.DiskUseRadio,"%");
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
