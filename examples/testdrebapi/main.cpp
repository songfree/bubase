#include "public.h"
#include "DrebApi.h"
#include "BF_DrebServerApi.h"
#include "Bfdefine.h"
#include "DrebClient_Java.h"

#ifdef _WINDOWS
#ifdef _DEBUG
#pragma comment(lib, "bf_dbpubd.lib")
#pragma comment(lib, "bf_drebapid.lib")
#pragma comment(lib, "bf_kerneld.lib") 
#pragma comment(lib, "bf_drebapi_javad.lib") 
#pragma message("Automatically linking with  bf_kerneld.lib bf_drebapid.lib")
#else
#pragma comment(lib, "bf_dbpub.lib")
#pragma comment(lib, "bf_kernel.lib")
#pragma comment(lib, "bf_drebapi.lib") 
#pragma comment(lib, "bf_drebapi_java.lib") 
#pragma message("Automatically linking with  bf_kernel.lib bf_drebapi.lib")
#endif
#endif

UINT64_ ntohll(UINT64_ val)
{
	if (htonl(1) == 1) //本机为网络序 AIX HP_UX
	{
		return val;
	}
	else
	{
		return (((UINT64_)htonl((int)((val << 32) >> 32))) << 32) | (unsigned int)htonl((int)(val >> 32));
	}
 }

UINT64_ htonll(UINT64_ val)
{
	if (htonl(1) == 1)
	{
		return val;//本机为网络序 AIX HP_UX
	}
    else
	{
        return (((UINT64_)htonl((int)((val << 32) >> 32))) << 32) | (unsigned int)htonl((int)(val >> 32));
    }
}



int main(int argc,char *argv[])
{


// 	printf("INT64 LENGTH %d\n",sizeof(INT64_));
// 	INT64_  aa =2345678901234567890;
// 	FILE *fp= fopen("tint64_l.dat","wb");
// 	if (fp != NULL)
// 	{
// 		fwrite(&aa,1,8,fp);
// 		fclose(fp);
// 	}
// 	CDrebEndian  endian;
// 	endian.Reverse((unsigned char *)&aa,8);
// 	//aa = htonll(aa);
// 	fp= fopen("tint64_b.dat","wb");
// 	if (fp != NULL)
// 	{
// 		fwrite(&aa,1,8,fp);
// 		fclose(fp);
// 	}
// 
// 	printf("%020I64u\n",aa);
// 
// 	INT64_   d1 = aa%10000000000; //高位
// 	printf("%020I64u\n",d1);
// 	INT64_ d2 = aa-d1*10000000000;
// 
// 	
// 	printf("%020I64u\n",d2);
// 


	int port=9021;
	int timeout = 6000;
	int aa = BFDREB_CLIENT_NewInstance("172.168.65.107",(char *)&port,(char *)&timeout);
	if (aa >120)
	{
		printf("BFDREB_CLIENT_NewInstance失败\n");
		return -1;
	}
	char index= aa;
	BFDREB_CLIENT_FreeInstance(aa);

	return -1;

	char errmsg[256];
	void *api=NULL;
	if (BFDREBAPI_InitApi("bfbpc.xml",&api)<0)
	{
		BFDREBAPI_GetErrMsg(api,errmsg);
		printf("初始化失败 %s\n",errmsg);
		BFDREBAPI_Free(api);
		return -1;
	}
	if (BFDREBAPI_StartApi(api) <0)
	{
		BFDREBAPI_GetErrMsg(api,errmsg);
		printf("启动api失败 %s\n",errmsg);
		BFDREBAPI_Free(api);
		return -1;
	}
	//发送数据
	DREBSAPI_COMMSTRU gdata;
	int nRet;
	bzero(&gdata,sizeof(DREBSAPI_COMMSTRU));
	bool issend = false;
	while (true)
	{
		nRet = BFDREBAPI_GetMsg(api,(char *)&gdata);
		if (nRet <0)
		{
			printf("取数据出错 %d\n",nRet);
		}
		else if (nRet == 0)
		{
			printf("无数据接收\n");
		}
		else
		{
			switch (gdata.cMsgType)
			{
				case MSG_REQ: //总线过来的请求  包含外调、推送、广播
					printf("收到待处理请求\n");
					break;
				case MSG_DREBANS:  //总线过来的应答数据  即从本API发出去的请求的应答
					printf("收到应答\n");
					break;
				case MSG_MONITOR_SAP:  //总线过来的监控请求数据，buffer里为api组好的xml报文，可根据需要增加后调用发送回去即可
					printf("监控报文%d %s \n",gdata.sDBHead.nLen,gdata.sBuffer);
					//应答回去
					gdata.sDBHead.cRaflag = 1;
					BFDREBAPI_SendMsg(api,(const char *)&gdata);
					break;
				case MSG_CONNECTDREB:  //api发过来的消息，通知调用方本API已经连上总线了，可以注册等等
					printf("本次连接上的总线索引 %d \n",gdata.index);
					//注意调用一次仅在一个总线上注册，多个总线要调用多次
					BFDREBAPI_RegisterDreb(api,gdata.index,"1|2|3|4|");//在连接的总线上注册1、2、3、4四个交易码
					break;
				case MSG_BPCMONITOR:  //api发过来的消息，通知调用方本该向总线主动上报监控信息了,buffer里为api组好的xml报文，可根据需要增加后调用发送回去即可
					gdata.sDBHead.cRaflag = 0;
					gdata.sDBHead.cCmd = CMD_DPPUSH;
					printf("上报报文%d %s \n",gdata.sDBHead.nLen,gdata.sBuffer);
					BFDREBAPI_SendMsg(api,(const char *)&gdata);
					break;
				default:
					break;
			}
		}

	}
	BFDREBAPI_Free(api);
	return 0;

}