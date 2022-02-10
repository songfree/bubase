// DrebClientApi.cpp: implementation of the CDrebClientApi class.
//
//////////////////////////////////////////////////////////////////////

#include "DrebClientApi.h"
#include "DrebClient.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


// ������: BFDREBCLIENT_API_Monitor_Dreb
// ���  : ������ 2009-12-2 15:23:40
// ����  : int >0<DREBHEADLENͨѶƽ̨���ش� >DREBHEADLEN�յ������ݳ��� =0��ָ��ʱ����δ�յ�����  <0ʧ�� -100δ���� -1���ӶϿ� -2ͨѶƽ̨Ӧ�����ݲ��� -3CRC����
// ����  : void *pClass    �ͻ�����ָ��
// ����  : int hook
// ����  : int serial
// ����  : DREB_D_NODEINFO dest
// ����  : const char *data  ��ӦcZip������(���ļ��ܻ�ѹ��)
// ����  : int datalen ��ӦcZip�����ݳ���
// ����  : int cZip
// ����  : DREB_HEAD &head
// ����  : char *ansdata   ��Ӧhead->cZip�⿪�����������,�Զ������
// ����  : ͨѶƽ̨��� Ҫ�����մ�����ɺ�Ӧ��
int BFDREBCLIENT_API_Monitor_Dreb(void *pClass,int hook,int serial,DREB_D_NODEINFO dest, const char *data, int datalen,int cZip,DREB_HEAD &head,char *ansdata)
{
	if (pClass == NULL)
	{
		return -3;
	}
	CDrebClient *cliapi = (CDrebClient *)pClass;
	return cliapi->Monitor_Dreb(hook,serial,dest,data,datalen,cZip,head,ansdata);
}

// ������: BFDREBCLIENT_API_Monitor_Bpc
// ���  : ������ 2009-12-2 15:23:40
// ����  : int >0<DREBHEADLENͨѶƽ̨���ش� >DREBHEADLEN�յ������ݳ��� =0��ָ��ʱ����δ�յ�����  <0ʧ�� -100δ���� -1���ӶϿ� -2ͨѶƽ̨Ӧ�����ݲ��� -3CRC����
// ����  : void *pClass    �ͻ�����ָ��
// ����  : int hook
// ����  : int serial
// ����  : DREB_D_NODEINFO dest
// ����  : const char *data  ��ӦcZip������(���ļ��ܻ�ѹ��)
// ����  : int datalen ��ӦcZip�����ݳ���
// ����  : int cZip
// ����  : DREB_HEAD &head
// ����  : char *ansdata   ��Ӧhead->cZip�⿪�����������,�Զ������
// ����  : BPC��� Ҫ�����մ�����ɺ�Ӧ��
int BFDREBCLIENT_API_Monitor_Bpc(void *pClass,int hook,int serial,DREB_D_NODEINFO dest, const char *data, int datalen,int cZip,DREB_HEAD &head,char *ansdata)
{
	if (pClass == NULL)
	{
		return -3;
	}
	CDrebClient *cliapi = (CDrebClient *)pClass;
	return cliapi->Monitor_Bpc(hook,serial,dest,data,datalen,cZip,head,ansdata);
}

// ������: BFDREBCLIENT_API_GetDrebHead
// ���  : ������ 2010-9-28 9:18:45
// ����  : void 
// ����  : DREB_HEAD &head
// ����  : ��ȡ����ʱ���ص�drebͷ
void BFDREBCLIENT_API_GetDrebHead(void *pClass,DREB_HEAD &head)
{
	if (pClass == NULL)
	{
		return ;
	}
	CDrebClient *cliapi = (CDrebClient *)pClass;
	cliapi->GetDrebHead(head);
	return;
}

// ������: BFDREBCLIENT_API_DPABC
// ���  : ������ 2009-12-2 15:25:00
// ����  : int <0����ʧ�ܣ�>=���͵����ݳ���
// ����  : void *pClass    �ͻ�����ָ��
// ����  : int hook
// ����  : int serial
// ����  : DREB_D_NODEINFO dest
// ����  : const char *data  ��ӦcZip������(���ļ��ܻ�ѹ��)
// ����  : int datalen ��ӦcZip�����ݳ���
// ����  : int cZip
// ����  : ͨѶƽ̨�㲥��������Ϣ����ָ��ͨѶƽ̨������ע��ķ���,��Ҫ��ͨѶƽ̨Ӧ��
int BFDREBCLIENT_API_DPABC(void *pClass,int hook, int serial,int gateindex,DREB_D_NODEINFO dest, const char *data, int datalen,int cZip,int nextno)
{
	if (pClass == NULL)
	{
		return -3;
	}
	CDrebClient *cliapi = (CDrebClient *)pClass;
	return cliapi->DPABC(hook,serial,gateindex,dest,data,datalen,cZip,nextno);
}

// ������: BFDREBCLIENT_API_DPBC
// ���  : ������ 2009-12-2 15:24:42
// ����  : int =100�ɹ� >0��<100dreb���ش� <0����ʧ�� =0δ�յ�ȷ����Ϣ
// ����  : void *pClass    �ͻ�����ָ��
// ����  : int hook
// ����  : int serial
// ����  : DREB_D_NODEINFO dest
// ����  : const char *data  ��ӦcZip������(���ļ��ܻ�ѹ��)
// ����  : int datalen ��ӦcZip�����ݳ���
// ����  : int cZip
// ����  : ͨѶƽ̨�㲥��������Ϣ����ָ��ͨѶƽ̨������ע��ķ���,Ҫ��ͨѶƽ̨Ӧ��
int BFDREBCLIENT_API_DPBC(void *pClass,int hook, int serial,DREB_D_NODEINFO dest, const char *data, int datalen,int cZip)
{
	if (pClass == NULL)
	{
		return -3;
	}
	CDrebClient *cliapi = (CDrebClient *)pClass;
	return cliapi->DPBC(hook,serial,dest,data,datalen,cZip);
}

// ������: BFDREBCLIENT_API_DPPost
// ���  : ������ 2009-12-2 15:24:28
// ����  : int =100�ɹ� >0��<100dreb���ش� <0����ʧ�� =0δ�յ�ȷ����Ϣ
// ����  : void *pClass    �ͻ�����ָ��
// ����  : int hook
// ����  : int serial
// ����  : DREB_D_NODEINFO dest
// ����  : const char *data  ��ӦcZip������(���ļ��ܻ�ѹ��)
// ����  : int datalen ��ӦcZip�����ݳ���
// ����  : int cZip
// ����  : ͨѶƽ̨Ͷ�ݣ�Ҫ����յ���ͨѶƽ̨Ӧ��
int BFDREBCLIENT_API_DPPost(void *pClass,int hook, int serial, DREB_D_NODEINFO dest, const char *data, int datalen,int cZip)
{
	if (pClass == NULL)
	{
		return -3;
	}
	CDrebClient *cliapi = (CDrebClient *)pClass;
	return cliapi->DPPost(hook,serial,dest,data,datalen,cZip);
}

// ������: BFDREBCLIENT_API_DPPush
// ���  : ������ 2009-12-2 15:24:13
// ����  : int <0����ʧ�ܣ�>=���͵����ݳ���
// ����  : void *pClass    �ͻ�����ָ��
// ����  : int hook
// ����  : int serial
// ����  : DREB_D_NODEINFO dest
// ����  : const char *data  ��ӦcZip������(���ļ��ܻ�ѹ��)
// ����  : int datalen ��ӦcZip�����ݳ���
// ����  : int cZip
// ����  : ͨѶƽ̨���ͣ�����Ӧ��
int BFDREBCLIENT_API_DPPush(void *pClass,int hook, int serial, DREB_D_NODEINFO dest, const char *data, int datalen,int cZip)
{
	if (pClass == NULL)
	{
		return -3;
	}
	CDrebClient *cliapi = (CDrebClient *)pClass;
	return cliapi->DPPush(hook,serial,dest,data,datalen,cZip);
}

// ������: BFDREBCLIENT_API_DPACall
// ���  : ������ 2009-12-2 15:24:06
// ����  : int =100�ɹ� >0��<100dreb���ش� <0����ʧ�� =0δ�յ�ȷ����Ϣ
// ����  : void *pClass    �ͻ�����ָ��
// ����  : int hook
// ����  : int serial
// ����  : DREB_D_NODEINFO dest
// ����  : const char *data  ��ӦcZip������(���ļ��ܻ�ѹ��)
// ����  : int datalen ��ӦcZip�����ݳ���
// ����  : int cZip
// ����  : ͨѶƽ̨�첽���� Ҫ����յ���ͨѶƽ̨�����������ȷ��Ӧ��
int BFDREBCLIENT_API_DPACall(void *pClass,int hook, int serial, DREB_D_NODEINFO dest, const char *data, int datalen,int cZip)
{
	if (pClass == NULL)
	{
		return -3;
	}
	CDrebClient *cliapi = (CDrebClient *)pClass;
	return cliapi->DPACall(hook,serial,dest,data,datalen,cZip);
}

// ������: BFDREBCLIENT_API_DPCall
// ���  : ������ 2009-12-2 15:23:40
// ����  : int >0<DREBHEADLENͨѶƽ̨���ش� >DREBHEADLEN�յ������ݳ��� =0��ָ��ʱ����δ�յ�����  <0ʧ�� -100δ���� -1���ӶϿ� -2ͨѶƽ̨Ӧ�����ݲ��� -3CRC����
// ����  : void *pClass    �ͻ�����ָ��
// ����  : int hook
// ����  : int serial
// ����  : DREB_D_NODEINFO dest
// ����  : const char *data  ��ӦcZip������(���ļ��ܻ�ѹ��)
// ����  : int datalen ��ӦcZip�����ݳ���
// ����  : int cZip
// ����  : DREB_HEAD &head
// ����  : char *ansdata   ��Ӧhead->cZip�⿪�����������,�Զ������
// ����  : ͨѶƽ̨ͬ������ Ҫ�����մ�����ɺ�Ӧ��
int BFDREBCLIENT_API_DPCall(void *pClass,int hook,int serial,DREB_D_NODEINFO dest, const char *data, int datalen,int cZip,DREB_HEAD &head,char *ansdata)
{
	if (pClass == NULL)
	{
		return -3;
	}
	CDrebClient *cliapi = (CDrebClient *)pClass;
	return cliapi->DPCall(hook,serial,dest,data,datalen,cZip,head,ansdata);
}

// ������: BFDREBCLIENT_API_IsConnect
// ���  : ������ 2009-12-2 15:13:30
// ����  : int    1����  0δ���� 
// ����  : void *pClass    �ͻ�����ָ��
// ����  : �Ƿ�����
int BFDREBCLIENT_API_IsConnect(void *pClass)
{
	if (pClass == NULL)
	{
		return -3;
	}
	CDrebClient *cliapi = (CDrebClient *)pClass;
	if (cliapi->IsConnect())
	{
		return 1;
	}
	return 0;
}

// ������: BFDREBCLIENT_API_ReceveData
// ���  : ������ 2009-11-26 16:16:19
// ����  : int >0ʵ���յ������ݳ��� =0��ָ��ʱ����δ�յ�����  <0ʧ�� -100δ���� -1���ӶϿ� -2ͨѶƽ̨Ӧ�����ݲ��� -3CRC����
// ����  : void *pClass    �ͻ�����ָ��
// ����  : DREB_HEAD &head  ͨѶƽ̨ͨѶͷ 
// ����  : char *data       ���յ������� ��Ӧhead->cZip�⿪�����������,����Ϊhead.nLen,�Զ������
// ����  : int timeout      �������ݳ�ʱʱ��
// ����  : �������ݣ�ע�ⷵ��ֵ���ܺ�head.nLen���ȣ�head.nLen�ǽ⿪�����������
int BFDREBCLIENT_API_ReceveData(void *pClass,DREB_HEAD &head ,unsigned char *data,unsigned int &datalen,int timeout)
{
	if (pClass == NULL)
	{
		return -3;
	}
	CDrebClient *cliapi = (CDrebClient *)pClass;
	return cliapi->ReceveData(head,data,datalen,timeout);
}


// ������: BFDREBCLIENT_API_Close
// ���  : ������ 2009-11-26 16:16:10
// ����  : void 
// ����  : void *pClass    �ͻ�����ָ��
// ����  : �ر�����
void BFDREBCLIENT_API_Close(void *pClass)
{
	if (pClass == NULL)
	{
		return ;
	}
	CDrebClient *cliapi = (CDrebClient *)pClass;
	cliapi->Close();
}

// ������: BFDREBCLIENT_API_Connect
// ���  : ������ 2009-11-26 16:15:58
// ����  : int   0�ɹ� �������ɹ� 
// ����  : void *pClass    �ͻ�����ָ��
// ����  : ����ͨѶƽ̨
int BFDREBCLIENT_API_Connect(void *pClass)
{
	if (pClass == NULL)
	{
		return -3;
	}
	CDrebClient *cliapi = (CDrebClient *)pClass;
	if (cliapi->Connect())
	{
		return 0;
	}
	return -1;
}

// ������: BFDREBCLIENT_API_Init
// ���  : ������ 2009-11-26 14:45:45
// ����  : int  0�ɹ�  ����ʧ��
// ����  : void *pClass    �ͻ�����ָ��
// ����  : const  char *ip Ҫ���ӵ�ͨѶƽ̨ip
// ����  : short port Ҫ���ӵ�ͨѶƽ̨�˿�
// ����  : int   timeout ���ճ�ʱ
// ����  : �������ӵ�ͨѶƽ̨����
int BFDREBCLIENT_API_Init(void *pClass,const  char *ip,short port,int timeout)
{
	if (pClass == NULL)
	{
		return -3;
	}
	CDrebClient *cliapi = (CDrebClient *)pClass;
	if (cliapi->Init(ip,port,timeout))
	{
		return 0;
	}
	return -1;
}


// ������: BFDREBCLIENT_API_SendData
// ���  : ������ 2009-11-26 16:16:28
// ����  : int <0����ʧ�� -1���ͳ��� -2����̫�� >0 Ϊdatalen��ֵ
// ����  : void *pClass    �ͻ�����ָ��
// ����  : int hook    �ͻ���ʹ�õ��ֶΣ�ͨѶƽ̨���ã�ԭ�����ء�
// ����  : int serial  �첽ʱ��д����ˮ��ͨѶƽ̨���ã�ԭ������
// ����  : int cmd   ������ CMD_DPCALL CMD_DPACALL CMD_DPBC CMD_DPABC CMD_DPPUSH CMD_DPPOST
// ����  : DREB_D_NODEINFO dest  Ŀ����Ϣ  
// ����  : char *data  ����
// ����  : int datalen ���ݳ���
// ����  : int cZip 0�������� 13DES����,����16λ���� 2ѹ�� 3DES����,����8λ���� 4DES���ܲ�ѹ��
// ����  : ��������
int BFDREBCLIENT_API_SendData(void *pClass,int hook,int serial,int gateindex,int cmd,DREB_D_NODEINFO dest,const char *data, int datalen,int cZip,int nextno)
{
	if (pClass == NULL)
	{
		return -3;
	}
	CDrebClient *cliapi = (CDrebClient *)pClass;
	return cliapi->SendData(hook,serial,gateindex,cmd,dest,data,datalen,cZip,nextno);
}


// ������: BFDREBCLIENT_API_NewApi
// ���  : ������ 2013-12-4 14:20:53
// ����  : int 0�ɹ� <0ʧ��
// ����  : void **pClass  ���ص�����������ָ��
// ����  : ��ʼ��api���õ�������ָ��
int BFDREBCLIENT_API_NewApi(void **pClass)
{
	*pClass = new CDrebClient();
	if (*pClass == NULL)
	{
		return -1;
	}
	return 0;

}
void BFDREBCLIENT_API_FreeApi(void *pClass)
{
	if (pClass != NULL)
	{
		delete pClass;
		pClass = NULL;
	}
	return ;
}