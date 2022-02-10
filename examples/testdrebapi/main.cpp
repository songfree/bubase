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
	if (htonl(1) == 1) //����Ϊ������ AIX HP_UX
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
		return val;//����Ϊ������ AIX HP_UX
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
// 	INT64_   d1 = aa%10000000000; //��λ
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
		printf("BFDREB_CLIENT_NewInstanceʧ��\n");
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
		printf("��ʼ��ʧ�� %s\n",errmsg);
		BFDREBAPI_Free(api);
		return -1;
	}
	if (BFDREBAPI_StartApi(api) <0)
	{
		BFDREBAPI_GetErrMsg(api,errmsg);
		printf("����apiʧ�� %s\n",errmsg);
		BFDREBAPI_Free(api);
		return -1;
	}
	//��������
	DREBSAPI_COMMSTRU gdata;
	int nRet;
	bzero(&gdata,sizeof(DREBSAPI_COMMSTRU));
	bool issend = false;
	while (true)
	{
		nRet = BFDREBAPI_GetMsg(api,(char *)&gdata);
		if (nRet <0)
		{
			printf("ȡ���ݳ��� %d\n",nRet);
		}
		else if (nRet == 0)
		{
			printf("�����ݽ���\n");
		}
		else
		{
			switch (gdata.cMsgType)
			{
				case MSG_REQ: //���߹���������  ������������͡��㲥
					printf("�յ�����������\n");
					break;
				case MSG_DREBANS:  //���߹�����Ӧ������  ���ӱ�API����ȥ�������Ӧ��
					printf("�յ�Ӧ��\n");
					break;
				case MSG_MONITOR_SAP:  //���߹����ļ���������ݣ�buffer��Ϊapi��õ�xml���ģ��ɸ�����Ҫ���Ӻ���÷��ͻ�ȥ����
					printf("��ر���%d %s \n",gdata.sDBHead.nLen,gdata.sBuffer);
					//Ӧ���ȥ
					gdata.sDBHead.cRaflag = 1;
					BFDREBAPI_SendMsg(api,(const char *)&gdata);
					break;
				case MSG_CONNECTDREB:  //api����������Ϣ��֪ͨ���÷���API�Ѿ����������ˣ�����ע��ȵ�
					printf("���������ϵ��������� %d \n",gdata.index);
					//ע�����һ�ν���һ��������ע�ᣬ�������Ҫ���ö��
					BFDREBAPI_RegisterDreb(api,gdata.index,"1|2|3|4|");//�����ӵ�������ע��1��2��3��4�ĸ�������
					break;
				case MSG_BPCMONITOR:  //api����������Ϣ��֪ͨ���÷����������������ϱ������Ϣ��,buffer��Ϊapi��õ�xml���ģ��ɸ�����Ҫ���Ӻ���÷��ͻ�ȥ����
					gdata.sDBHead.cRaflag = 0;
					gdata.sDBHead.cCmd = CMD_DPPUSH;
					printf("�ϱ�����%d %s \n",gdata.sDBHead.nLen,gdata.sBuffer);
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