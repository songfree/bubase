#include "GateLink.h"


CGateLink g_pCgateApiClass;

//1���ӳɹ�  0δ����   2��������
int BFCGATE_GetConnectStatus()
{
	return g_pCgateApiClass.GetConnectStatus();
}

// ������: ConnectGate
// ���  : ������ 2013-5-29 15:27:27
// ����  : int  1�ɹ�  -1����ʧ�� 
// ����  : ��������
int BFCGATE_ConnectGate()
{
	return g_pCgateApiClass.ConnectGate();
}

// ������: Logout
// ���  : ������ 2013-5-23 15:20:54
// ����  : void 
// ����  : �رձ߽�,�˳��߳�
void BFCGATE_Logout()
{
	g_pCgateApiClass.Logout();
	
}

// ������: InitApi
// ���  : ������ 2013-5-23 14:49:22
// ����  : int 1�ɹ�
// ����  : const char *apixml xml�����ļ�
// ����  : ��ʼ��api����ȡ�����ļ����õ�֤��Ŀ¼�����ص�ַ�˿�
int BFCGATE_InitApi(const char *apixml)
{
	return g_pCgateApiClass.InitApi(apixml);

}

// ������: SendReqData
// ���  : ������ 2013-5-23 15:12:27
// ����  : int -1δ����  -2���ͳ���  =0 ��ʱδ�յ�Ӧ�� 1�յ�Ӧ��
// ����  : PCGATE_COMMSTRU data   ����Ӧ������
// ����  : unsigned int timeout    ����Ӧ��ʱʱ�� <=0ֱ�ӷ��أ�������Ӧ��
// ����  : �������󲢽���Ӧ��
int BFCGATE_SendReqData(void *data,unsigned int timeout)
{
	return g_pCgateApiClass.SendReqData((PCGATE_COMMSTRU)data,timeout);
}

int BFCGATE_Subscribe(void *data,unsigned int timeout)
{
	return g_pCgateApiClass.Subscribe((PCGATE_COMMSTRU)data,timeout);
}

// ������: SendLogin
// ���  : ������ 2013-5-23 15:12:27
// ����  : int -1δ����  -2���ͳ���  =0 ��ʱδ�յ�Ӧ�� 1�յ�Ӧ��
// ����  : PCGATE_COMMSTRU data   ����Ӧ������
// ����  : unsigned int timeout    ����Ӧ��ʱʱ��
// ����  : �������󲢽���Ӧ��
int BFCGATE_SendLogin(void *data,unsigned int timeout)
{
	return g_pCgateApiClass.SendLogin((PCGATE_COMMSTRU)data,timeout);
}

// ������: SendSms
// ���  : ������ 2013-5-23 15:12:27
// ����  : int -1δ����  -2���ͳ���  =0 ��ʱδ�յ�Ӧ�� 1�յ�Ӧ��
// ����  : PCGATE_COMMSTRU data   ����Ӧ������
// ����  : unsigned int timeout    ����Ӧ��ʱʱ��
// ����  : ���Ͷ������󲢽���Ӧ��
int BFCGATE_SendSms(void *data,unsigned int timeout)
{
	return g_pCgateApiClass.SendSms((PCGATE_COMMSTRU)data,timeout);
}

// ������: GetReqData
// ���  : ������ 2013-5-23 15:12:42
// ����  : int <0���� =0��ʱ������  >0��������
// ����  : char *data   ��������
// ����  : int *datalen ����Ϊdata�Ĵ�С�����Ϊ���ݵĴ�С
// ����  : int *txcode  ������
// ����  : int timeout ��ʱʱ����
// ����  : ȡ���ͻ�㲥����
int BFCGATE_GetReqData(void *data,unsigned int timeout)
{
	return g_pCgateApiClass.GetReqData((PCGATE_COMMSTRU)data,timeout);
}

// ������: GetAnsData
// ���  : ������ 2013-5-23 15:14:47
// ����  : int <0���ӳ��� >0ȡ��Ӧ�� =0��ʱ��Ӧ��
// ����  : PCGATE_COMMSTRU data   ����Ӧ������ 
// ����  : int timeout  ��ʱʱ����
// ����  : ȡ��ָ����ˮ��Ӧ��
int BFCGATE_GetAnsData(void *data,unsigned int timeout)
{
	return g_pCgateApiClass.GetAnsData((PCGATE_COMMSTRU)data,timeout);
}




//1�ɹ�  0δ����   2��������
int BFCGATE_TS_GetConnectStatus(void *pClass)
{
	CGateLink *pGl= (CGateLink *)pClass;
	return pGl->GetConnectStatus();
}

// ������: BFCGATE_TS_ConnectGate
// ���  : ������ 2013-5-29 15:27:27
// ����  : int  �ɹ�  0δ����   2�������� 
// ����  : ��������
int BFCGATE_TS_ConnectGate(void *pClass)
{
	CGateLink *pGl= (CGateLink *)pClass;
	return pGl->ConnectGate();
}

// ������: BFCGATE_TS_Logout
// ���  : ������ 2013-5-23 15:20:54
// ����  : void 
// ����  : �رձ߽�,�˳��߳�
void BFCGATE_TS_Logout(void *pClass)
{
	CGateLink *pGl= (CGateLink *)pClass;
	pGl->Logout();
	delete pGl;
	pGl = NULL;
}

// ������: BFCGATE_TS_InitApi
// ���  : ������ 2013-5-23 14:49:22
// ����  : int 1�ɹ�
// ����  : const char *apixml xml�����ļ�
// ����  : ��ʼ��api����ȡ�����ļ����õ�֤��Ŀ¼�����ص�ַ�˿ڣ�����������
int BFCGATE_TS_InitApi(const char *apixml,void **pClass)
{
	*pClass = new CGateLink();
	if (*pClass == NULL)
	{
		return -1;
	}
	return ((CGateLink *)(*pClass))->InitApi(apixml);
}

// ������: BFCGATE_TS_SendReqData
// ���  : ������ 2013-5-23 15:12:27
// ����  : int -1δ����  -2���ͳ���  =0 ��ʱδ�յ�Ӧ�� 1�յ�Ӧ��
// ����  : PCGATE_COMMSTRU data   ��������
// ����  : unsigned int timeout    ����Ӧ��ʱʱ�� <=0ֱ�ӷ��أ�������Ӧ��
// ����  : �������󲢽���Ӧ��
int BFCGATE_TS_SendReqData(void *pClass,void *data,unsigned int timeout)
{
	CGateLink *pGl= (CGateLink *)pClass;
	return pGl->SendReqData((PCGATE_COMMSTRU)data,timeout);
}


int BFCGATE_TS_Subscribe(void *pClass,void *data,unsigned int timeout)
{
	CGateLink *pGl= (CGateLink *)pClass;
	return pGl->Subscribe((PCGATE_COMMSTRU)data,timeout);
}

// ������: BFCGATE_TS_SendLogin
// ���  : ������ 2013-5-23 15:12:27
// ����  : int -1δ����  -2���ͳ���  =0 ��ʱδ�յ�Ӧ�� 1�յ�Ӧ��
// ����  : PCGATE_COMMSTRU data   ����Ӧ������
// ����  : unsigned int timeout    ����Ӧ��ʱʱ��
// ����  : �������󲢽���Ӧ��
int BFCGATE_TS_SendLogin(void *pClass,void *data,unsigned int timeout)
{
	CGateLink *pGl= (CGateLink *)pClass;
	return pGl->SendLogin((PCGATE_COMMSTRU)data,timeout);
}

// ������: BFCGATE_TS_SendSms
// ���  : ������ 2013-5-23 15:12:27
// ����  : int -1δ����  -2���ͳ���  =0 ��ʱδ�յ�Ӧ�� 1�յ�Ӧ��
// ����  : PCGATE_COMMSTRU data   ����Ӧ������
// ����  : unsigned int timeout    ����Ӧ��ʱʱ��
// ����  : ���Ͷ������󲢽���Ӧ��
int BFCGATE_TS_SendSms(void *pClass,void *data,unsigned int timeout)
{
	CGateLink *pGl= (CGateLink *)pClass;
	return pGl->SendSms((PCGATE_COMMSTRU)data,timeout);
}

// ������: BFCGATE_TS_GetReqData
// ���  : ������ 2013-5-23 15:12:42
// ����  : int <0���� =0��ʱ������  >0��������
// ����  : char *data   ��������
// ����  : int *datalen ����Ϊdata�Ĵ�С�����Ϊ���ݵĴ�С
// ����  : int *txcode  ������
// ����  : int timeout ��ʱʱ����
// ����  : ȡ���ͻ�㲥����
int BFCGATE_TS_GetReqData(void *pClass,void *data,int timeout)
{
	CGateLink *pGl= (CGateLink *)pClass;
	return pGl->GetReqData((PCGATE_COMMSTRU)data,timeout);
}

// ������: BFCGATE_TS_GetAnsData
// ���  : ������ 2013-5-23 15:14:47
// ����  : int <0���ӳ��� >0ȡ��Ӧ�� =0��ʱ��Ӧ��
// ����  : PCGATE_COMMSTRU data   ����Ӧ������ 
// ����  : int timeout  ��ʱʱ����
// ����  : ȡ��ָ����ˮ��Ӧ��
int BFCGATE_TS_GetAnsData(void *pClass,void *data,int timeout)
{
	CGateLink *pGl= (CGateLink *)pClass;
	return pGl->GetAnsData((PCGATE_COMMSTRU)data,timeout);
}
