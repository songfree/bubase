// changedbpwd.cpp : Defines the entry point for the console application.
//
#include "public.h"
#include "BF_SocketTcp.h"


#ifdef _WINDOWS
#ifdef _DEBUG
#pragma comment(lib, "bf_kerneld.lib") 
#pragma message("Automatically linking with  bf_kerneld.lib ")
#else
#pragma comment(lib, "bf_kernel.lib") 
#pragma message("Automatically linking with  bf_kernel.lib ")
#endif
#endif

int main(int argc, char* argv[])
{
	CBF_SocketTcp  pSocket;
	pSocket.Create(AF_INET,false);
	if (!pSocket.ConnectServer("127.0.0.1",3001,10000))
	{
		printf("连接网关失败\n");
		return -1;
	}
	pSocket.SetBlockFlag(false);
	char sendbuf[8192];
	bzero(sendbuf,sizeof(sendbuf));
	
	FILE *fp;
	int sendLen;
	int nRet;
// 	fp = fopen("sendpack.txt","rb");
// 	if (fp == NULL)
// 	{
// 		printf("sendpack.txt不存在\n");
// 		return -1;
// 	}
// 	sendLen = fread(sendbuf,1,8192,fp);
// 	if (sendLen <1)
// 	{
// 		printf("sendpack.txt无数据\n");
// 		return -1;
// 	}
// 	fclose(fp);
// 	
// 	nRet = pSocket.Send(sendbuf,sendLen);
// 	if (nRet != sendLen)
// 	{
// 		printf("发送sendpack.txt内空出错\n");
// 		return -1;
// 	}
// 	
	fp = fopen("getfile.xml","wb");
	if (fp == NULL)
	{
		return -1;
	}
	char databuf[8192];
	int  datasize= sizeof(databuf)-1;

	int getlen = 0;
	int wantlen=0;
	bool isbegin = true;
	char headbuf[100];
	bzero(headbuf,sizeof(headbuf));
	
	while (true)
	{
		if (pSocket.CheckReadEvent(1000) == 1)
		{
			nRet = pSocket.Receive(databuf,datasize);
			if (nRet <= 0)
			{
				printf("连接已关闭\n");
				fclose(fp);
				pSocket.Close();
				return -1;
			}
			getlen +=nRet;
			if (isbegin)
			{
				memcpy(headbuf,databuf,8);
				wantlen = atol(headbuf);
				isbegin = false;
			}
			fwrite(databuf,1,nRet,fp);
			if (getlen >= wantlen)
			{
				printf("接收文件成功\n");
				fclose(fp);
				SLEEP(10000);
				pSocket.Close();
				return -1;
			}
			SLEEP(1);
		}
	}
	return 0;
}
