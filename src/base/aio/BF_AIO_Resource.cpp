// BF_AIO_Resource.cpp: implementation of the CBF_AIO_Resource class.
//
//////////////////////////////////////////////////////////////////////

#include "BF_AIO_Resource.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBF_AIO_Resource::CBF_AIO_Resource()
{
	g_nIoThreadNum=1;//IO线程数
	g_nMaxConnectNum=1024;//最大连接数
	g_bIsExit=true;  //是否退出
	g_nRunMode=0;//运行模式   0-select模型  1-epoll模型(LINUX)   2-iocp模型(WINDOWS)
#if defined(_WINDOWS)
	g_hIocp = NULL;  //IOCP的HANDLE
#endif
#if defined(LINUX)
	g_nEpoll=0;  //EPOlL
#endif
	g_nStartTime = CBF_Date_Time::GetTickCount();
	g_nRecvByteLen = 0;
	g_nSendByteLen = 0;
	g_nIocpAcceptNum = 5;
	bzero(g_sCurPath,sizeof(g_sCurPath));
	bzero(g_sModuleName,sizeof(g_sModuleName));
	bzero(g_sModulePath,sizeof(g_sModulePath));
	g_nListenIndex = 0;

	g_nQuoteQueueDeleteTime =10;//默认为行情过期10秒就不发给客户
	g_nQuoteQueueNum = 100; //只保留最新的100条记录
}
void CBF_AIO_Resource::ReSetPara()
{
	g_nIoThreadNum=1;//IO线程数
	g_nMaxConnectNum=1024;//最大连接数
	g_bIsExit=true;  //是否退出
	g_nRunMode=0;//运行模式   0-select模型  1-epoll模型(LINUX)   2-iocp模型(WINDOWS)
#if defined(_WINDOWS)
	g_hIocp = NULL;  //IOCP的HANDLE
#endif
#if defined(LINUX)
	g_nEpoll=0;  //EPOlL
#endif
	g_nStartTime = CBF_Date_Time::GetTickCount();
	g_nRecvByteLen = 0;
	g_nSendByteLen = 0;
	g_nIocpAcceptNum = 5;
	bzero(g_sCurPath,sizeof(g_sCurPath));
	bzero(g_sModuleName,sizeof(g_sModuleName));
	bzero(g_sModulePath,sizeof(g_sModulePath));
	g_nListenIndex = 0;
	g_nSendOnceNum = 16;
}
CBF_AIO_Resource::~CBF_AIO_Resource()
{

}
bool CBF_AIO_Resource::SetRunMode(unsigned int mode)
{
#if defined(LINUX)
	if (mode == MODESELECT || mode == MODEEPOLL)
	{
		g_nRunMode = mode;
		if (mode == MODEEPOLL && g_nMaxConnectNum > EPOLLMAXFD)
		{
			g_nMaxConnectNum = EPOLLMAXFD;
		}
		return true;
	}
	return false;
#endif
	
#if defined(_WINDOWS)
	if (mode == MODESELECT || mode == MODEIOCP)
	{
		g_nRunMode = mode;
		return true;
	}
	return false;
#endif
	if (mode != MODESELECT)
	{
		return false;
	}
	g_nRunMode = mode;
	return true;
}

void CBF_AIO_Resource::SetIoThreadNum(unsigned int iothreadnum)
{
	if (g_nRunMode == MODESELECT)
	{
		g_nIoThreadNum = iothreadnum;
		if (g_nIoThreadNum > 20)
		{
			g_nIoThreadNum = 20;
		}
	}
	else if (g_nRunMode == MODEIOCP)
	{
#ifdef _WINDOWS
		SYSTEM_INFO si;
		GetSystemInfo(&si);
		g_nIoThreadNum = si.dwNumberOfProcessors;//IOCP线程数为cpu处理器个数
#endif
	}
	else
	{
		g_nIoThreadNum = 1;
	}
	return;
}
