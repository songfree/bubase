#include "public.h"
#include "GateApi.h"
#include "BF_Tools.h"
#include "GateHead.h"
#include "BF_Thread.h"

#ifdef WIN32
#ifdef _DEBUG
#pragma comment(lib, "bf_cgateapid.lib")
#pragma comment(lib, "bf_kerneld.lib") 
#pragma message("Automatically linking with  bf_kerneld.lib bf_cgateapidd.lib")
#else
#pragma comment(lib, "bf_kerneld.lib")
#pragma comment(lib, "bf_cgateapi.lib") 
#pragma message("Automatically linking with  bf_kernel.lib bf_cgateapi.lib")
#endif
#endif

void *g_pCgateClass=NULL;  //api类
int  g_nSerailRequest=0;   //发送请求的流水序号
int  g_nTimeOut=5;         //超时时间
unsigned int  g_nSendRequestNum=0;  //发送请求数
unsigned int  g_nGetAnsDataNum=0;   //接收应答数
unsigned int  g_nGetPushDataNum=0;  //接收推送消息数

bool    g_bExit;//是否退出


class TestApiReqThread : public CBF_Thread
{
public:
	TestApiReqThread()
	{
		
	}
	~TestApiReqThread()
	{
		
	}
private:
	virtual int Run()
	{
		int ret;
		CGATE_COMMSTRU data;
		bzero(&data,sizeof(CGATE_COMMSTRU));
		while(!g_bExit)
		{
			ret = BFCGATE_TS_GetReqData(g_pCgateClass,&data,g_nTimeOut);
			if (ret >0)
			{
				//处理
				g_nGetPushDataNum ++;
			}
		}
		return 0;
	}
};

int GetSerial()
{
	g_nSerailRequest++;
	if (g_nSerailRequest > 65534 )
	{
		g_nSerailRequest = 0;
	}
	return g_nSerailRequest;
}
bool GetNextPack(int ret,CGATE_COMMSTRU &data)
{
	int packnum=0;
	switch (ret)
	{
		case -1:
			printf("未连接网关\n");
			return false;
		case -2:
			printf("发送请求出错\n");
			return false;
		case 0:
			printf("接收应答超时\n");
			return false;
		case 1:
			printf("收到后台应答 nRet[%d] nLen[%d]\n",data.head.nRetCode,data.head.nLen);
			g_nGetAnsDataNum++;
			if (data.head.stComm.cNextFlag == 0)
			{
				printf("无后续包\n");
				return true;
			}
			//后台主动推送，不停的收
			if (data.head.stComm.cNextFlag == 1 || data.head.stComm.cNextFlag == 10)
			{
				packnum++;
				while (true)
				{
					printf("收到应答的数据包数 %d",packnum);
					if (packnum >= data.head.stNext.n_nNextNo) //总包数
					{
						printf("应答总数据包数目 %d\n",packnum);
						return true;
					}
					ret = BFCGATE_TS_SendReqData(g_pCgateClass,&data,g_nTimeOut);
					if (ret != 1)
					{
						printf("接收应答出错 %d",ret);
						return false;
					}
					packnum++;
					g_nGetAnsDataNum++;
				}

			}
			else
			{
				packnum++;
				while (data.head.stComm.cNextFlag <10)
				{
					printf("收到应答的数据包数 %d",packnum);
					ret = BFCGATE_TS_SendReqData(g_pCgateClass,&data,g_nTimeOut);
					if (ret != 1)
					{
						printf("接收应答出错 %d",ret);
						return false;
					}
					packnum++;
					g_nGetAnsDataNum++;
					g_nSendRequestNum++;
				}
				printf("收到应答的数据包数 %d",packnum);
				return true;
			}
			return true;
		default:
			printf("未知返回\n");
			return false;
	}	
}


int main(int argc,char *argv[])
{
	int nRet;
	char databuffer[CLIENTDATASIZE+1];
	bzero(databuffer,sizeof(databuffer));
	nRet = CBF_Tools::GetConsoleEnter(">请输入选项\n>---0 : 开始测试\n>---1 : 退出\n>",1,databuffer);
	if (databuffer[0] == '1')
	{
		printf("退出!\n");
		return -1;
	}
	
	if (BFCGATE_TS_InitApi("cgateapi.xml",&g_pCgateClass)!=1)
	{
		printf("初始化网关失败!\n");
		return false;
	}
	int connNum=0;
	while (BFCGATE_TS_GetConnectStatus(g_pCgateClass)!=1)
	{
		if (BFCGATE_TS_ConnectGate(g_pCgateClass)!=0)
		{
			SLEEP(2000);
			connNum++;
			if (connNum>10)
			{
				printf("连接网关失败!\n");
				BFCGATE_TS_Logout(g_pCgateClass);
				return -1;
			}
			continue;
		}
		break;
	}
	nRet = CBF_Tools::GetConsoleEnter(">请输入接收超时时间(秒)>",2,databuffer);
	databuffer[nRet]=0;
	g_nTimeOut = atoi(databuffer);
	if (g_nTimeOut <5 )
	{
		g_nTimeOut = 5;
	}
	printf("超时时间(秒) %d",g_nTimeOut);
	CGATE_COMMSTRU   data;

	CGATE_COMMSTRU   sLoginData;//保存登录报文
	CGATE_COMMSTRU   sSubscribeData;//保存登录报文

	//登录交易
	bzero(&data,sizeof(CGATE_COMMSTRU));
	nRet = CBF_Tools::GetConsoleEnter(">请输入登录报文\n>",CLIENTDATASIZE,databuffer);
	data.head.nLen = nRet;
	data.head.stComm.cMsgType  = MSG_GATELOGIN;
	data.head.stComm.cCmd = CMD_DPCALL;
	data.head.stComm.cZip = 0;
	data.head.stDest.nSerial = GetSerial();
	data.head.stDest.nServiceNo = 8004001;
	if (nRet >0)
	{
		memcpy(data.buffer,databuffer,nRet);
	}
	memcpy(&sLoginData,&data,sizeof(CGATE_COMMSTRU));
	nRet =  BFCGATE_TS_SendLogin(g_pCgateClass,&data,g_nTimeOut);
	
	if (!GetNextPack(nRet,data))
	{
		BFCGATE_TS_Logout(g_pCgateClass);
		return -1;
	}
	
	//订阅交易
	bzero(&data,sizeof(CGATE_COMMSTRU));
	nRet = CBF_Tools::GetConsoleEnter(">请输入订阅报文\n>",CLIENTDATASIZE,databuffer);
	data.head.nLen = nRet;
	data.head.stComm.cMsgType  = MSG_GATEREQ;
	data.head.stComm.cCmd = CMD_DPCALL;
	data.head.stComm.cZip = 0;
	data.head.stDest.nSerial = GetSerial();
	data.head.stDest.nServiceNo = 1903;
	if (nRet >0)
	{
		memcpy(data.buffer,databuffer,nRet);
	}
	memcpy(&sSubscribeData,&data,sizeof(CGATE_COMMSTRU));
	nRet = BFCGATE_TS_SendReqData(g_pCgateClass,&data,g_nTimeOut);
	if (!GetNextPack(nRet,data))
	{
		BFCGATE_TS_Logout(g_pCgateClass);
		return -1;
	}
	//启动接收线程
	g_bExit = false;
	TestApiReqThread pushThread;
	pushThread.CreateThread();
	int nTxCode;
	while (!g_bExit)
	{
		connNum = 0;
		while (BFCGATE_TS_GetConnectStatus(g_pCgateClass)!=1)
		{
			if (BFCGATE_TS_ConnectGate(g_pCgateClass)!=0)
			{
				SLEEP(2000);
				connNum++;
				if (connNum>10)
				{
					printf("连接网关失败!\n");
					BFCGATE_TS_Logout(g_pCgateClass);
					return -1;
				}
				continue;
			}
			memcpy(&data,&sLoginData,sizeof(CGATE_COMMSTRU));
			nRet =  BFCGATE_TS_SendLogin(g_pCgateClass,&data,g_nTimeOut);
			GetNextPack(nRet,data);
			memcpy(&data,&sSubscribeData,sizeof(CGATE_COMMSTRU));
			nRet = BFCGATE_TS_SendReqData(g_pCgateClass,&data,g_nTimeOut);
			GetNextPack(nRet,data);
		}
		nRet = CBF_Tools::GetConsoleEnter(">请输入选项\n>---0 : 继续测试\n>---1 : 退出\n>",1,databuffer);
		if (databuffer[0] == '1')
		{
			printf("退出!\n");
			BFCGATE_TS_Logout(g_pCgateClass);
			return -1;
		}
		nRet = CBF_Tools::GetConsoleEnter(">请输入交易码\n",8,databuffer);
		databuffer[nRet]=0;
		nTxCode = atol(databuffer);
		nRet = CBF_Tools::GetConsoleEnter(">请输入交易报文\n>",CLIENTDATASIZE,databuffer);
		bzero(&data,sizeof(CGATE_COMMSTRU));
		data.head.stDest.nServiceNo = 
		data.head.nLen = nRet;
		data.head.stComm.cMsgType  = MSG_GATEREQ;
		data.head.stComm.cCmd = CMD_DPCALL;
		data.head.stComm.cZip = 0;
		data.head.stDest.nSerial = GetSerial();
		data.head.stDest.nServiceNo = nTxCode;
		if (nRet >0)
		{
			memcpy(data.buffer,databuffer,nRet);
		}
		nRet = BFCGATE_TS_SendReqData(g_pCgateClass,&data,g_nTimeOut);
		GetNextPack(nRet,data);
	}
	return 0;

}