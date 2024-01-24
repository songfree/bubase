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

#define SystemBasicInformation       0   //ϵͳ������Ϣ
#define SystemPerformanceInformation 2   //ϵͳ������Ϣ
#define SystemTimeInformation        3   //ϵͳʱ����Ϣ
#define SystemProcessInformation     5    //��������Ϣ
#define SystemProcessorPerformanceInformation  8   //������������Ϣ


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
} SYSTEM_BASIC_INFORMATION;         //ϵͳ������Ϣ�ṹ   ��Ӧ SystemBasicInformation



typedef struct __SYSTEM_PROCESSOR_TIMES
{
	LARGE_INTEGER IdleTime;               //����ʱ�䣻
	LARGE_INTEGER KernelTime;             //�ں�ģʽʱ�䣻
	LARGE_INTEGER UserTime;               //�û�ģʽʱ�䣻
	LARGE_INTEGER DpcTime;                //�ӳٹ��̵���ʱ�䣻
	LARGE_INTEGER InterruptTime;          //�ж�ʱ�䣻
	ULONG         InterruptCount;         //�жϴ�����
 }SYSTEM_PROCESSOR_TIMES,*PSYSTEM_PROCESSOR_TIMES;  //��ӦSystemProcessInformation

typedef struct _SYSTEM_PERFORMANCE_INFORMATION
{
	LARGE_INTEGER  IdleTime;                    //CPU����ʱ�䣻
	LARGE_INTEGER  ReadTransferCount;           //I/O��������Ŀ��
	LARGE_INTEGER  WriteTransferCount;          //I/Oд������Ŀ��
	LARGE_INTEGER  OtherTransferCount;          //I/O����������Ŀ��
	ULONG          ReadOperationCount;          //I/O��������Ŀ��
	ULONG          WriteOperationCount;         //I/Oд������Ŀ��
	ULONG          OtherOperationCount;         //I/O��������������Ŀ��
	ULONG          AvailablePages;              //�ɻ�õ�ҳ��Ŀ��
	ULONG          TotalCommittedPages;         //�ܹ��ύҳ��Ŀ��
	ULONG          TotalCommitLimit;            //���ύҳ��Ŀ��
	ULONG          PeakCommitment;              //ҳ�ύ��ֵ��
	ULONG          PageFaults;                  //ҳ������Ŀ��
	ULONG          WriteCopyFaults;             //Copy-On-Write������Ŀ��
	ULONG          TransitionFaults;            //��ҳ������Ŀ��
	ULONG          Reserved1;
	ULONG          DemandZeroFaults;            //����0��������
	ULONG          PagesRead;                   //��ҳ��Ŀ��
	ULONG          PageReadIos;                 //��ҳI/O��������
	ULONG          Reserved2[2];
	ULONG          PagefilePagesWritten;        //��дҳ�ļ�ҳ����
	ULONG          PagefilePageWriteIos;        //��дҳ�ļ���������
	ULONG          MappedFilePagesWritten;      //��дӳ���ļ�ҳ����
	ULONG          MappedFileWriteIos;          //��дӳ���ļ���������
	ULONG          PagedPoolUsage;              //��ҳ��ʹ�ã�
	ULONG          NonPagedPoolUsage;           //�Ƿ�ҳ��ʹ�ã�
	ULONG          PagedPoolAllocs;             //��ҳ�ط��������
	ULONG          PagedPoolFrees;              //��ҳ���ͷ������
	ULONG          NonPagedPoolAllocs;          //�Ƿ�ҳ�ط��������
	ULONG          NonPagedPoolFress;           //�Ƿ�ҳ���ͷ������
	ULONG          TotalFreeSystemPtes;         //ϵͳҳ�����ͷ�������
	ULONG          SystemCodePage;              //����ϵͳ����ҳ����
	ULONG          TotalSystemDriverPages;      //�ɷ�ҳ��������ҳ����
	ULONG          TotalSystemCodePages;        //����ϵͳ����ҳ������
	ULONG          SmallNonPagedLookasideListAllocateHits; //
	ULONG          SmallPagedLookasideListAllocateHits;
	ULONG          Reserved3;                   
	ULONG          MmSystemCachePage;          //ϵͳ����ҳ����
	ULONG          PagedPoolPage;              //��ҳ��ҳ����
	ULONG          SystemDriverPage;           //�ɷ�ҳ����ҳ����
	ULONG          FastReadNoWait;             //�첽���ٶ���Ŀ��
	ULONG          FastReadWait;               //ͬ�����ٶ���Ŀ��
	ULONG          FastReadResourceMiss;       //���ٶ���Դ��ͻ����
	ULONG          FastReadNotPossible;        //���ٶ�ʧ������
	ULONG          FastMdlReadNoWait;          //�첽MDL���ٶ���Ŀ��
	ULONG          FastMdlReadWait;            //ͬ��MDL���ٶ���Ŀ��
	ULONG          FastMdlReadResourceMiss;    //MDL����Դ��ͻ����
	ULONG          FastMdlReadNotPossible;     //MDL��ʧ������
	ULONG          MapDataNoWait;              //�첽ӳ�����ݴ�����
	ULONG          MapDataWait;                //ͬ��ӳ�����ݴ�����
	ULONG          MapDataNoWaitMiss;          //�첽ӳ�����ݳ�ͻ������
	ULONG          MapDataWaitMiss;            //ͬ��ӳ�����ݳ�ͻ������
	ULONG          PinMappedDataCount;         //ǣ��ӳ��������Ŀ��
	ULONG          PinReadNoWait;              //ǣ���첽����Ŀ��
	ULONG          PinReadWait;                //ǣ��ͬ������Ŀ��
	ULONG          PinReadNoWaitMiss;          //ǣ���첽����ͻ��Ŀ��
	ULONG          PinReadWaitMiss;            //ǣ��ͬ������ͻ��Ŀ��
	ULONG          CopyReadNoWait;             //�첽������������
	ULONG          CopyReadWait;               //ͬ��������������
	ULONG          CopyReadNoWaitMiss;         //�첽���������ϴ�����
	ULONG          CopyReadWaitMiss;           //ͬ�����������ϴ�����
	ULONG          MdlReadNoWait;              //�첽MDL��������
	ULONG          MdlReadWait;                //ͬ��MDL��������
	ULONG          MdlReadNoWaitMiss;          //�첽MDL�����ϴ�����
	ULONG          MdlReadWaitMiss;            //ͬ��MDL�����ϴ�����
	ULONG          ReadAheadIos;               //��ǰ��������Ŀ��
	ULONG          LazyWriteIos;               //LAZYд������Ŀ��
	ULONG          LazyWritePages;             //LAZYдҳ�ļ���Ŀ��
	ULONG          DataFlushes;                //����ˢ�´�����
	ULONG          DataPages;                  //����ˢ��ҳ����
	ULONG          ContextSwitches;            //�����л���Ŀ��
	ULONG          FirstLevelTbFills;          //��һ�㻺������������
	ULONG          SecondLevelTbFills;         //�ڶ��㻺������������
	ULONG          SystemCall;                 //ϵͳ���ô�����
}SYSTEM_PERFORMANCE_INFORMATION,*PSYSTEM_PERFORMANCE_INFORMATION; //��ӦSystemPerformanceInformation


typedef struct
{
    LARGE_INTEGER liKeBootTime;
    LARGE_INTEGER liKeSystemTime;
    LARGE_INTEGER liExpTimeZoneBias;
    ULONG         uCurrentTimeZoneId;
    DWORD         dwReserved;
} SYSTEM_TIME_INFORMATION;      //ϵͳʱ����Ϣ�ṹ ��ӦSystemTimeInformation

typedef struct
{
    LARGE_INTEGER IdleTime;
    LARGE_INTEGER KernelTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER Reserved1[2];
    ULONG Reserved2;
} SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION;  //��ӦSystemProcessorPerformanceInformation

// 
// #define FT2D(x) ((double)((x).dwHighDateTime) * 4.294967296E9 + (double)((x).dwLowDateTime))
// 
// bool GetCPUIdelTmAndSysTm(double& dIdelTm, double& dSysTm)
// {
// 	FILETIME  stIdelTm = {0};
// 	FILETIME  stSysTm = {0};
// 	
// 	//��ȡcpu����ʱ��
// 	if(!GetSystemTimes(&stIdelTm, NULL, NULL))
// 	{
// 		return false;
// 	}
// 	
// 	//��ȡcpuϵͳʱ��
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

	tCpuUsed = (kernel +user - idle) *100/(kernel+user);//���ܵ�ʱ��-����ʱ�䣩/�ܵ�ʱ��=ռ��cpu��ʱ�����ʹ����
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
	SYSTEM_PERFORMANCE_INFORMATION SysPerfInfo;  //ϵͳ������Ϣ�ṹ
    SYSTEM_TIME_INFORMATION        SysTimeInfo;  //ϵͳʱ����Ϣ�ṹ
    SYSTEM_BASIC_INFORMATION       SysBaseInfo;  //ϵͳ������Ϣ�ṹ
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
		m_log.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"ȡ��ntdll��NtQuerySystemInformation��������");
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
	m_log.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"CPUʹ���� %d%s",UsageCpu,"%");
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
				m_log.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"CPUʹ���� %d%s",100-atoi(sTemp[14]),"%");
				return 100-atoi(sTemp[14]);
			}
#elif AIX
			if(iCount == 6)
			{
				sscanf(sWorkBuffer,"%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",sTemp[0],sTemp[1],sTemp[2],sTemp[3],sTemp[4],sTemp[5],\
					sTemp[6],sTemp[7],sTemp[8],sTemp[9],sTemp[10],sTemp[11],sTemp[12],sTemp[13],sTemp[14],\
					sTemp[15],sTemp[16]);	

				pclose(fp);
				m_log.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"CPUʹ���� %d%s",100-atoi(sTemp[15]),"%");
				
				return 100-atoi(sTemp[15]);
			}
#else  //HP_UX
			if(iCount == 3)
			{
				sscanf(sWorkBuffer,"%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",sTemp[0],sTemp[1],sTemp[2],sTemp[3],sTemp[4],sTemp[5],\
					sTemp[6],sTemp[7],sTemp[8],sTemp[9],sTemp[10],sTemp[11],sTemp[12],sTemp[13],sTemp[14],\
					sTemp[15],sTemp[16],sTemp[17]);	
				
				pclose(fp);
				m_log.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"CPUʹ���� %d%s",100-atoi(sTemp[17]),"%");
	
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
	



	SYSTEM_PERFORMANCE_INFORMATION SysPerfInfo;  //ϵͳ������Ϣ�ṹ
    SYSTEM_TIME_INFORMATION        SysTimeInfo;  //ϵͳʱ����Ϣ�ṹ
    SYSTEM_BASIC_INFORMATION       SysBaseInfo;  //ϵͳ������Ϣ�ṹ
    SYSTEM_INFO sys_info;
	LONG  status;
    NtQuerySystemInformation = (PROCNTQSI)GetProcAddress(GetModuleHandle("ntdll"),"NtQuerySystemInformation");
	
    if (NULL == NtQuerySystemInformation)
	{
		m_log.LogMp(LOG_ERROR_FAULT,__FILE__,__LINE__,"ȡ��ntdll��NtQuerySystemInformation��������");
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
	
	//	printf("���ڴ�=%dM ʹ���ڴ�=%dM �ڴ�ʹ����%d%s\n",smInfo.EmsTotal,smInfo.EmsUse,smInfo.EmsUseRadio,"%");
	m_log.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���ڴ�=%dM ʹ���ڴ�=%dM �ڴ�ʹ����%d%s",smInfo.EmsTotal,smInfo.EmsUse,smInfo.EmsUseRadio,"%");
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
				
				
				//20151230 modi ��Ϊ����=free +buffer +cache
				//smInfo.EmsUse = atoi(sTemp3)/1024;
				//smInfo.EmsUse = smInfo.EmsTotal - atoi(sTemp4)/1024 - atoi(sTemp6)/1024 -atoi(sTemp7)/1024;
				smInfo.EmsUse = smInfo.EmsTotal - atoi(sTemp7) / 1024;

				smInfo.EmsUseRadio = 100*(smInfo.EmsUse)/(smInfo.EmsTotal);
//				printf("���ڴ�=%dM ʹ���ڴ�=%dM �ڴ�ʹ����%d%s\n",smInfo.EmsTotal,smInfo.EmsUse,smInfo.EmsUseRadio,"%");
				m_log.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���ڴ�=%dM ʹ���ڴ�=%dM �ڴ�ʹ����%d%s",smInfo.EmsTotal,smInfo.EmsUse,smInfo.EmsUseRadio,"%");
				
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
		m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"vmstat 1 1 ����");
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
					m_log.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"vmstat 1 1 ���ڴ����� %s",sWorkBuffer);
					if (strlen(sTemp[2])<6)
					{
						if (strlen(sTemp[1])<6)
						{
//							printf("Get CPURadio error %s!\n",sWorkBuffer);
							m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"vmstat 1 1 ���� %s",sWorkBuffer);
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
				
				//printf("���ڴ�%s %sM",sTemp[3],tmpchar);
				smInfo.EmsTotal = atoi(tmpchar);
				if (smInfo.EmsTotal == 0)
				{
					m_log.LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"���ڴ���� %s",tmpchar);
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
				m_log.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���ڴ�=%dM ʹ���ڴ�=%dM �ڴ�ʹ����%d%s",smInfo.EmsTotal,smInfo.EmsUse,smInfo.EmsUseRadio,"%");
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
//				printf("ʹ���ڴ� %dM",smInfo.EmsUse);
//				printf("�ڴ�ʹ���� %d%s\n",100*(smInfo.EmsUse)/smInfo.EmsTotal,"%");
				m_log.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���ڴ�=%dM ʹ���ڴ�=%dM �ڴ�ʹ����%d%s",smInfo.EmsTotal,smInfo.EmsUse,smInfo.EmsUseRadio,"%");
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
����:�õ�Ӳ��ʹ�����
����:��
����ֵ:Ӳ����Ϣ�ṹ��
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
	//�õ���Ч����������,���̷� 
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
//				printf("������� %s:�ܴ�С%dM ʹ�ô�С%dM ʹ����%.2f%s \n",tmp.cIndex,tmp.TotalSpace,tmp.UsedSpace,tmp.DiskUseRadio,"%");
				m_log.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"������� %s:�ܴ�С%dM ʹ�ô�С%dM ʹ����%.2f%s ",tmp.cIndex,tmp.TotalSpace,tmp.UsedSpace,tmp.DiskUseRadio,"%");
				
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
//				printf("������� %s:�ܴ�С%dM ʹ�ô�С%dM ʹ����%.2f%s \n",tmp.cIndex,tmp.TotalSpace,tmp.UsedSpace,tmp.DiskUseRadio,"%");
				m_log.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"������� %s:�ܴ�С%dM ʹ�ô�С%dM ʹ����%.2f%s ",tmp.cIndex,tmp.TotalSpace,tmp.UsedSpace,tmp.DiskUseRadio,"%");
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
//				printf("������� %s:�ܴ�С%dM ʹ�ô�С%dM ʹ����%.2f%s \n",tmp.cIndex,tmp.TotalSpace,tmp.UsedSpace,tmp.DiskUseRadio,"%");
				m_log.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"������� %s:�ܴ�С%dM ʹ�ô�С%dM ʹ����%.2f%s ",tmp.cIndex,tmp.TotalSpace,tmp.UsedSpace,tmp.DiskUseRadio,"%");
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
				else if (iCount == 1) //����
				{
	
				}
				else if (iCount == 2) //ʹ��
				{
					sscanf(sWorkBuffer,"%s",sTemp1);
					tmp.UsedSpace = atoll(sTemp1)/1024;
				}
				else //ʹ����
				{
					sscanf(sWorkBuffer,"%s",sTemp1);
					tmp.DiskUseRadio = atof(sTemp1);
					if (tmp.TotalSpace>0)
					{
						vmDisINFO.push_back(tmp);
					}
					
//					printf("������� %s:�ܴ�С%dM ʹ�ô�С%dM ʹ����%.2f%s \n",tmp.cIndex,tmp.TotalSpace,tmp.UsedSpace,tmp.DiskUseRadio,"%");
					m_log.LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"������� %s:�ܴ�С%dM ʹ�ô�С%dM ʹ����%.2f%s ",tmp.cIndex,tmp.TotalSpace,tmp.UsedSpace,tmp.DiskUseRadio,"%");
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
