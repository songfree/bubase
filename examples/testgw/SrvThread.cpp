// SrvThread.cpp: implementation of the CSrvThread class.
//
//////////////////////////////////////////////////////////////////////

#include "SrvThread.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CSrvThread::CSrvThread()
{

}

CSrvThread::~CSrvThread()
{

}

int CSrvThread::RunSelect()
{
	int nRet;
	int nLen;
	char sip[100];
	int offset=0;
	char databuffer[200*1024+1];
	int nDatalen= sizeof(databuffer)-1;
	CBF_SocketTcp  pSocketCli;
	if ( m_pSocket.CheckReadEvent(1000)==1)
	{
		SOCKET_HANDLE clisocket = m_pSocket.Accept(sip,false);
		if (clisocket != NULL)
		{
			printf("接收到%s的连接\n",sip);
			pSocketCli.AttachSocket(clisocket,"");
			FILE *fp;
			fp = fopen("bfbpcd.exe","rb");
			if (fp == NULL)
			{
				printf("打开文件错\n");
				pSocketCli.Close();
				return -1;
			}
			struct_stat fszie;
			FSTAT("bfbpcd.exe",&fszie);
			sprintf(sip,"%08d",fszie.st_size);
			nRet = pSocketCli.Send(sip,8);
			if (nRet !=8 )
			{
				printf("Send出错 %d\n",nRet);
				pSocketCli.Close();
				return -1;
			}

			bool isend=false;
			nLen = fread(databuffer,1,nDatalen,fp);
			if (nLen <1)
			{
				printf("文件无数据\n");
				fclose(fp);
				return -1;
			}
			if (nLen<nDatalen)
			{
				isend = true;
			}
			while (true)
			{
				printf("开始发送数据 offset[%d]  nLen[%d]\n",offset,nLen);
				nRet = pSocketCli.Send(databuffer+offset,nLen);
				if (nRet <0)
				{
					printf("Send出错 %d\n",nRet);
					pSocketCli.Close();
					return -1;
				}
				if (nRet != nLen)
				{
					printf("发送部分数据 %d\n",nRet);
					nLen = nLen - nRet;
					offset = nRet;
					while (pSocketCli.CheckWriteEvent(20000) != 1)
					{
						printf("等待连接可写 \n");
						SLEEP(1000);	
					}
					printf("继续发数据 %d\n",nLen);
					continue;
				}
				else
				{
					printf("发送完数据 %d\n",nLen);
					if (isend)
					{
						printf("发送完成\n");
						SLEEP(100000);
						pSocketCli.Close();
						fclose(fp);
						return 0;
					}
					offset = 0;
					nLen = fread(databuffer,1,nDatalen,fp);
					if (nLen <0)
					{
						printf("发送完成\n");
						pSocketCli.Close();
						fclose(fp);
						return 0;
					}
					printf("要发送数据 offset[0] nLen[%d]\n",nLen);
					if (nLen<nDatalen)
					{
						isend = true;
					}
				}
				
				
				
			}
		}
	}
	return 0;
}
// 函数名: InitThreadInstance
// 编程  : 王明松 2012-4-9 12:00:54
// 返回  : virtual bool 
// 描述  : 线程初始化，线程启动时调用一次
bool CSrvThread::InitThreadInstance()
{
	return true;
}

// 函数名: ExitThreadInstance
// 编程  : 王明松 2012-4-9 12:01:05
// 返回  : virtual void 
// 描述  : 退出线程要做的工作,线程退出时调用一次
void CSrvThread::ExitThreadInstance()
{
	return;
}
int CSrvThread::Run()
{
	return 0;
}
bool CSrvThread::Listen(unsigned short port)
{
	m_pSocket.Create(AF_INET,false);
	if (!m_pSocket.Listen(port,5))
	{
		printf("Listen失败 %d\n",port);
		return false;
	}
	printf("开始[%d]侦听\n",port);
	return true;
}
