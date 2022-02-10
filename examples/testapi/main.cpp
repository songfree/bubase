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

void *g_pCgateClass=NULL;  //api��
int  g_nSerailRequest=0;   //�����������ˮ���
int  g_nTimeOut=5;         //��ʱʱ��
unsigned int  g_nSendRequestNum=0;  //����������
unsigned int  g_nGetAnsDataNum=0;   //����Ӧ����
unsigned int  g_nGetPushDataNum=0;  //����������Ϣ��

bool    g_bExit;//�Ƿ��˳�


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
				//����
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
			printf("δ��������\n");
			return false;
		case -2:
			printf("�����������\n");
			return false;
		case 0:
			printf("����Ӧ��ʱ\n");
			return false;
		case 1:
			printf("�յ���̨Ӧ�� nRet[%d] nLen[%d]\n",data.head.nRetCode,data.head.nLen);
			g_nGetAnsDataNum++;
			if (data.head.stComm.cNextFlag == 0)
			{
				printf("�޺�����\n");
				return true;
			}
			//��̨�������ͣ���ͣ����
			if (data.head.stComm.cNextFlag == 1 || data.head.stComm.cNextFlag == 10)
			{
				packnum++;
				while (true)
				{
					printf("�յ�Ӧ������ݰ��� %d",packnum);
					if (packnum >= data.head.stNext.n_nNextNo) //�ܰ���
					{
						printf("Ӧ�������ݰ���Ŀ %d\n",packnum);
						return true;
					}
					ret = BFCGATE_TS_SendReqData(g_pCgateClass,&data,g_nTimeOut);
					if (ret != 1)
					{
						printf("����Ӧ����� %d",ret);
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
					printf("�յ�Ӧ������ݰ��� %d",packnum);
					ret = BFCGATE_TS_SendReqData(g_pCgateClass,&data,g_nTimeOut);
					if (ret != 1)
					{
						printf("����Ӧ����� %d",ret);
						return false;
					}
					packnum++;
					g_nGetAnsDataNum++;
					g_nSendRequestNum++;
				}
				printf("�յ�Ӧ������ݰ��� %d",packnum);
				return true;
			}
			return true;
		default:
			printf("δ֪����\n");
			return false;
	}	
}


int main(int argc,char *argv[])
{
	int nRet;
	char databuffer[CLIENTDATASIZE+1];
	bzero(databuffer,sizeof(databuffer));
	nRet = CBF_Tools::GetConsoleEnter(">������ѡ��\n>---0 : ��ʼ����\n>---1 : �˳�\n>",1,databuffer);
	if (databuffer[0] == '1')
	{
		printf("�˳�!\n");
		return -1;
	}
	
	if (BFCGATE_TS_InitApi("cgateapi.xml",&g_pCgateClass)!=1)
	{
		printf("��ʼ������ʧ��!\n");
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
				printf("��������ʧ��!\n");
				BFCGATE_TS_Logout(g_pCgateClass);
				return -1;
			}
			continue;
		}
		break;
	}
	nRet = CBF_Tools::GetConsoleEnter(">��������ճ�ʱʱ��(��)>",2,databuffer);
	databuffer[nRet]=0;
	g_nTimeOut = atoi(databuffer);
	if (g_nTimeOut <5 )
	{
		g_nTimeOut = 5;
	}
	printf("��ʱʱ��(��) %d",g_nTimeOut);
	CGATE_COMMSTRU   data;

	CGATE_COMMSTRU   sLoginData;//�����¼����
	CGATE_COMMSTRU   sSubscribeData;//�����¼����

	//��¼����
	bzero(&data,sizeof(CGATE_COMMSTRU));
	nRet = CBF_Tools::GetConsoleEnter(">�������¼����\n>",CLIENTDATASIZE,databuffer);
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
	
	//���Ľ���
	bzero(&data,sizeof(CGATE_COMMSTRU));
	nRet = CBF_Tools::GetConsoleEnter(">�����붩�ı���\n>",CLIENTDATASIZE,databuffer);
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
	//���������߳�
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
					printf("��������ʧ��!\n");
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
		nRet = CBF_Tools::GetConsoleEnter(">������ѡ��\n>---0 : ��������\n>---1 : �˳�\n>",1,databuffer);
		if (databuffer[0] == '1')
		{
			printf("�˳�!\n");
			BFCGATE_TS_Logout(g_pCgateClass);
			return -1;
		}
		nRet = CBF_Tools::GetConsoleEnter(">�����뽻����\n",8,databuffer);
		databuffer[nRet]=0;
		nTxCode = atol(databuffer);
		nRet = CBF_Tools::GetConsoleEnter(">�����뽻�ױ���\n>",CLIENTDATASIZE,databuffer);
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