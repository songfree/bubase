// GateApi.h: interface for the CGateApi class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GATEAPI_H__38F43A80_B734_4386_A0E1_1A8E395F79EC__INCLUDED_)
#define AFX_GATEAPI_H__38F43A80_B734_4386_A0E1_1A8E395F79EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

extern "C"
{	
	// ������: ConnectGate
	// ���  : ������ 2013-5-29 15:27:27
	// ����  : int  1�ɹ� 0 δ����  2��������
	// ����  : ��������
	int BFCGATE_GetConnectStatus();
	
	// ������: ConnectGate
	// ���  : ������ 2013-5-29 15:27:27
	// ����  : int  1�ɹ�  -1����ʧ��  
	// ����  : ��������
	int BFCGATE_ConnectGate();
	
	// ������: Logout
	// ���  : ������ 2013-5-23 15:20:54
	// ����  : void 
	// ����  : �ر�����,�˳��߳�
	void BFCGATE_Logout();
	
	// ������: InitApi
	// ���  : ������ 2013-5-23 14:49:22
	// ����  : int 1�ɹ�
	// ����  : const char *apixml xml�����ļ�
	// ����  : ��ʼ��api����ȡ�����ļ����õ�֤��Ŀ¼�����ص�ַ�˿�
	int BFCGATE_InitApi(const char *apixml);
	
	
	// ������: SendReqData
	// ���  : ������ 2013-5-23 15:12:27
	// ����  : int -1δ����  -2���ͳ���  =0 ��ʱδ�յ�Ӧ�� >=1�յ�Ӧ�� �������ֵΪ��ֵ��1
	// ����  : PCGATE_COMMSTRU data   ����Ӧ������
	// ����  : unsigned int timeout    ����Ӧ��ʱʱ�� <=0ֱ�ӷ���0��������Ӧ��
	// ����  : �������󲢽���Ӧ��
	int BFCGATE_SendReqData(void *data,unsigned int timeout);
	
		// ������: BFCGATE_Subscribe
	// ���  : ������ 2014-10-21 15:51:02
	// ����  : int  -1δ����  -2���ͳ���  =0 ��ʱδ�յ�Ӧ�� >=1�յ�Ӧ�� �������ֵΪ��ֵ��1
	// ����  : void *pClass ����������ָ��
	// ����  : void *data ����Ӧ������
	// ����  : unsigned int timeout ����Ӧ��ʱʱ�� <=0ֱ�ӷ���0��������Ӧ��
	// ����  : �������ض�������
	int BFCGATE_Subscribe(void *data,unsigned int timeout);
	
	// ������: GetReqData
	// ���  : ������ 2013-5-23 15:12:42
	// ����  : int <0���� =0��ʱ������  >=1�ɹ�ȡ���������� �������ֵΪ��ֵ��1
	// ����  : PCGATE_COMMSTRU data   ��������
	// ����  : int timeout ��ʱʱ����
	// ����  : ȡ���ͻ�㲥����
	int BFCGATE_GetReqData(void *data,int timeout);
	
	// ������: GetAnsData
	// ���  : ������ 2013-5-23 15:14:47
	// ����  : int <0���� =0��ʱ������  >1�ɹ�ȡ���������� �������ֵΪ��ֵ��1
	// ����  : PCGATE_COMMSTRU data   ��������
	// ����  : int timeout ��ʱʱ���� 
	// ����  : ȡ��ָ����ˮ��Ӧ��
	int BFCGATE_GetAnsData(void *data,int timeout);
	
	// ������: SendLogin
	// ���  : ������ 2013-5-23 15:12:27
	// ����  : int -1δ����  -2���ͳ���  =0 ��ʱδ�յ�Ӧ�� >1�յ�Ӧ�� �������ֵΪ��ֵ��1
	// ����  : PCGATE_COMMSTRU data   ����Ӧ������
	// ����  : unsigned int timeout    ����Ӧ��ʱʱ�� <=0ֱ�ӷ���0��������Ӧ��
	// ����  : �������󲢽���Ӧ��
	int BFCGATE_SendLogin(void *data,unsigned int timeout);
	
	// ������: SendSms
	// ���  : ������ 2013-5-23 15:12:27
	// ����  : int -1δ����  -2���ͳ���  =0 ��ʱδ�յ�Ӧ�� >1�յ�Ӧ�� �������ֵΪ��ֵ��1
	// ����  : PCGATE_COMMSTRU data   ����Ӧ������
	// ����  : unsigned int timeout    ����Ӧ��ʱʱ�� <=0ֱ�ӷ���0��������Ӧ��
	// ����  : �������󲢽���Ӧ��
	int BFCGATE_SendSms(void *data,unsigned int timeout);
	

	
	// ������: BFCGATE_TS_GetConnectStatus
	// ���  : ������ 2013-12-4 14:19:35
	// ����  : int 
	// ����  : void *pClass  ����������ָ��
	// ����  : ȡ����������״̬
	int BFCGATE_TS_GetConnectStatus(void *pClass);
	
	// ������: BFCGATE_TS_ConnectGate
	// ���  : ������ 2013-12-4 14:20:06
	// ����  : int 0�ɹ�
	// ����  : void *pClass ����������ָ��
	// ����  : ��������
	int BFCGATE_TS_ConnectGate(void *pClass);
	
	// ������: BFCGATE_TS_Logout
	// ���  : ������ 2013-12-4 14:20:32
	// ����  : void 
	// ����  : void *pClass ����������ָ��
	// ����  : �ر�����,�˳��̣߳��ͷ�ָ��ռ�
	void BFCGATE_TS_Logout(void *pClass);
	
	// ������: BFCGATE_TS_InitApi
	// ���  : ������ 2013-12-4 14:20:53
	// ����  : int 1�ɹ�
	// ����  : const char *apixml
	// ����  : void **pClass  ���ص�����������ָ��
	// ����  : ��ʼ��api����ȡ�����ļ����õ�֤��Ŀ¼�����ص�ַ�˿�
	int BFCGATE_TS_InitApi(const char *apixml,void **pClass);
	
	
	// ������: BFCGATE_TS_SendReqData
	// ���  : ������ 2013-5-23 15:12:27
	// ����  : int -1δ����  -2���ͳ���  =0 ��ʱδ�յ�Ӧ�� >=1�յ�Ӧ�� �������ֵΪ��ֵ��1
	// ����  : void *pClass  ����������ָ��
	// ����  : PCGATE_COMMSTRU data   ����Ӧ������
	// ����  : unsigned int timeout    ����Ӧ��ʱʱ�� <=0ֱ�ӷ���0��������Ӧ��
	// ����  : �������󲢽���Ӧ��
	int BFCGATE_TS_SendReqData(void *pClass,void *data,unsigned int timeout);
	
	// ������: BFCGATE_TS_Subscribe
	// ���  : ������ 2014-10-21 15:51:02
	// ����  : int  -1δ����  -2���ͳ���  =0 ��ʱδ�յ�Ӧ�� >=1�յ�Ӧ�� �������ֵΪ��ֵ��1
	// ����  : void *pClass ����������ָ��
	// ����  : void *data ����Ӧ������
	// ����  : unsigned int timeout ����Ӧ��ʱʱ�� <=0ֱ�ӷ���0��������Ӧ��
	// ����  : �������ض�������
	int BFCGATE_TS_Subscribe(void *pClass,void *data,unsigned int timeout);
	
	// ������: BFCGATE_TS_GetReqData
	// ���  : ������ 2013-5-23 15:12:42
	// ����  : int <0���� =0��ʱ������  >=1�ɹ�ȡ���������� �������ֵΪ��ֵ��1
	// ����  : void *pClass          ����������ָ��
	// ����  : PCGATE_COMMSTRU data   ��������
	// ����  : int timeout ��ʱʱ����
	// ����  : ȡ���ͻ�㲥����
	int BFCGATE_TS_GetReqData(void *pClass,void *data,int timeout);

	// ������: BFCGATE_TS_GetAnsData
	// ���  : ������ 2013-5-23 15:14:47
	// ����  : int <0���� =0��ʱ������  >=1�ɹ�ȡ���������� �������ֵΪ��ֵ��1
	// ����  : void *pClass          ����������ָ��
	// ����  : PCGATE_COMMSTRU data   ��������
	// ����  : int timeout ��ʱʱ����
	// ����  : ȡ��ָ����ˮ��Ӧ��
	int BFCGATE_TS_GetAnsData(void *pClass,void *data,int timeout);
	
	// ������: BFCGATE_TS_SendLogin
	// ���  : ������ 2013-5-23 15:12:27
	// ����  : int -1δ����  -2���ͳ���  =0 ��ʱδ�յ�Ӧ�� >=1�յ�Ӧ�� �������ֵΪ��ֵ��1
	// ����  : void *pClass          ����������ָ��
	// ����  : PCGATE_COMMSTRU *data            ����Ӧ������
	// ����  : unsigned int timeout    ����Ӧ��ʱʱ�� <=0ֱ�ӷ���0��������Ӧ��
	// ����  : �������󲢽���Ӧ��
	int BFCGATE_TS_SendLogin(void *pClass,void *data,unsigned int timeout);
	
	// ������: BFCGATE_TS_SendSms
	// ���  : ������ 2013-5-23 15:12:27
	// ����  : int -1δ����  -2���ͳ���  =0 ��ʱδ�յ�Ӧ�� >=1�յ�Ӧ�� �������ֵΪ��ֵ��1
	// ����  : void *pClass          ����������ָ��
	// ����  : PCGATE_COMMSTRU data             ����Ӧ������
	// ����  : unsigned int timeout    ����Ӧ��ʱʱ�� <=0ֱ�ӷ���0��������Ӧ��
	// ����  : �������󲢽���Ӧ��
	int BFCGATE_TS_SendSms(void *pClass,void *data,unsigned int timeout);


	// ������: BFCGATE_TS_GetConnectStatus
	// ���  : ������ 2013-12-4 14:19:35
	// ����  : int 
	// ����  : void *pClass  ����������ָ��
	// ����  : ȡ����������״̬
	int BFCGATE_TS_GetConnectStatus(void *pClass);
	
	// ������: BFCGATE_TS_ConnectGate
	// ���  : ������ 2013-12-4 14:20:06
	// ����  : int 0�ɹ�
	// ����  : void *pClass ����������ָ��
	// ����  : ��������
	int BFCGATE_TS_ConnectGate(void *pClass);
	
	// ������: BFCGATE_TS_Logout
	// ���  : ������ 2013-12-4 14:20:32
	// ����  : void 
	// ����  : void *pClass ����������ָ��
	// ����  : �ر�����,�˳��̣߳��ͷ�ָ��ռ�
	void BFCGATE_TS_Logout(void *pClass);
	
	// ������: BFCGATE_TS_InitApi
	// ���  : ������ 2013-12-4 14:20:53
	// ����  : int 1�ɹ�
	// ����  : const char *apixml
	// ����  : void **pClass  ���ص�����������ָ��
	// ����  : ��ʼ��api����ȡ�����ļ����õ�֤��Ŀ¼�����ص�ַ�˿�
	int BFCGATE_TS_InitApi(const char *apixml,void **pClass);
	
	
	// ������: BFCGATE_TS_SendReqData
	// ���  : ������ 2013-5-23 15:12:27
	// ����  : int -1δ����  -2���ͳ���  =0 ��ʱδ�յ�Ӧ�� >=1�յ�Ӧ�� �������ֵΪ��ֵ��1
	// ����  : void *pClass  ����������ָ��
	// ����  : PCGATE_COMMSTRU data   ����Ӧ������
	// ����  : unsigned int timeout    ����Ӧ��ʱʱ�� <=0ֱ�ӷ���0��������Ӧ��
	// ����  : �������󲢽���Ӧ��
	int BFCGATE_TS_SendReqData(void *pClass,void *data,unsigned int timeout);
	
	// ������: BFCGATE_TS_Subscribe
	// ���  : ������ 2014-10-21 15:51:02
	// ����  : int  -1δ����  -2���ͳ���  =0 ��ʱδ�յ�Ӧ�� >=1�յ�Ӧ�� �������ֵΪ��ֵ��1
	// ����  : void *pClass ����������ָ��
	// ����  : void *data ����Ӧ������
	// ����  : unsigned int timeout ����Ӧ��ʱʱ�� <=0ֱ�ӷ���0��������Ӧ��
	// ����  : �������ض�������
	int BFCGATE_TS_Subscribe(void *pClass,void *data,unsigned int timeout);
	
	// ������: BFCGATE_TS_GetReqData
	// ���  : ������ 2013-5-23 15:12:42
	// ����  : int <0���� =0��ʱ������  >=1�ɹ�ȡ���������� �������ֵΪ��ֵ��1
	// ����  : void *pClass          ����������ָ��
	// ����  : PCGATE_COMMSTRU data   ��������
	// ����  : int timeout ��ʱʱ����
	// ����  : ȡ���ͻ�㲥����
	int BFCGATE_TS_GetReqData(void *pClass,void *data,int timeout);

	// ������: BFCGATE_TS_GetAnsData
	// ���  : ������ 2013-5-23 15:14:47
	// ����  : int <0���� =0��ʱ������  >=1�ɹ�ȡ���������� �������ֵΪ��ֵ��1
	// ����  : void *pClass          ����������ָ��
	// ����  : PCGATE_COMMSTRU data   ��������
	// ����  : int timeout ��ʱʱ����
	// ����  : ȡ��ָ����ˮ��Ӧ��
	int BFCGATE_TS_GetAnsData(void *pClass,void *data,int timeout);
	
	// ������: BFCGATE_TS_SendLogin
	// ���  : ������ 2013-5-23 15:12:27
	// ����  : int -1δ����  -2���ͳ���  =0 ��ʱδ�յ�Ӧ�� >=1�յ�Ӧ�� �������ֵΪ��ֵ��1
	// ����  : void *pClass          ����������ָ��
	// ����  : PCGATE_COMMSTRU *data            ����Ӧ������
	// ����  : unsigned int timeout    ����Ӧ��ʱʱ�� <=0ֱ�ӷ���0��������Ӧ��
	// ����  : �������󲢽���Ӧ��
	int BFCGATE_TS_SendLogin(void *pClass,void *data,unsigned int timeout);
	
	// ������: BFCGATE_TS_SendSms
	// ���  : ������ 2013-5-23 15:12:27
	// ����  : int -1δ����  -2���ͳ���  =0 ��ʱδ�յ�Ӧ�� >=1�յ�Ӧ�� �������ֵΪ��ֵ��1
	// ����  : void *pClass          ����������ָ��
	// ����  : PCGATE_COMMSTRU data             ����Ӧ������
	// ����  : unsigned int timeout    ����Ӧ��ʱʱ�� <=0ֱ�ӷ���0��������Ӧ��
	// ����  : �������󲢽���Ӧ��
	int BFCGATE_TS_SendSms(void *pClass,void *data,unsigned int timeout);



	//��c#���õķ���

    // ������: BFCGATE_TSI_GetConnectStatus
    // ���  : ������ 2013-12-4 14:19:35
    // ����  : int 
    // ����  : unsigned int clientid  �����������±�
    // ����  : ȡ����������״̬
    int BFCGATE_TSI_GetConnectStatus(unsigned int clientid);

    // ������: BFCGATE_TSI_ConnectGate
    // ���  : ������ 2013-12-4 14:20:06
    // ����  : int 0�ɹ�
    // ����  : unsigned int clientid  �����������±�
    // ����  : ��������
    int BFCGATE_TSI_ConnectGate(unsigned int clientid);

    // ������: BFCGATE_TSI_Logout
    // ���  : ������ 2013-12-4 14:20:32
    // ����  : void 
    // ����  : unsigned int clientid  �����������±�
    // ����  : �ر�����,�˳��̣߳��ͷ�ָ��ռ�
    void BFCGATE_TSI_Logout(unsigned int clientid);

    // ������: BFCGATE_TSI_InitApi
    // ���  : ������ 2013-12-4 14:20:53
    // ����  : int >=0�ɹ� �����������±�  <0ʧ��
    // ����  : const char *apixml
    // ����  : ��ʼ��api����ȡ�����ļ����õ�֤��Ŀ¼�����ص�ַ�˿�
    int BFCGATE_TSI_InitApi(const char* apixml);


    // ������: BFCGATE_TSI_SendReqData
    // ���  : ������ 2013-5-23 15:12:27
    // ����  : int -1δ����  -2���ͳ���  =0 ��ʱδ�յ�Ӧ�� >=1�յ�Ӧ�� �������ֵΪ��ֵ��1
    // ����  : unsigned int clientid  �����������±�
    // ����  : PCGATE_COMMSTRU data   ����Ӧ������
    // ����  : unsigned int timeout    ����Ӧ��ʱʱ�� <=0ֱ�ӷ���0��������Ӧ��
    // ����  : �������󲢽���Ӧ��
    int BFCGATE_TSI_SendReqData(unsigned int clientid, void* data, unsigned int timeout);

    // ������: BFCGATE_TSI_Subscribe
    // ���  : ������ 2014-10-21 15:51:02
    // ����  : int  -1δ����  -2���ͳ���  =0 ��ʱδ�յ�Ӧ�� >=1�յ�Ӧ�� �������ֵΪ��ֵ��1
    // ����  : unsigned int clientid  �����������±�
    // ����  : void *data ����Ӧ������
    // ����  : unsigned int timeout ����Ӧ��ʱʱ�� <=0ֱ�ӷ���0��������Ӧ��
    // ����  : �������ض�������
    int BFCGATE_TSI_Subscribe(unsigned int clientid, void* data, unsigned int timeout);

    // ������: BFCGATE_TSI_GetReqData
    // ���  : ������ 2013-5-23 15:12:42
    // ����  : int <0���� =0��ʱ������  >=1�ɹ�ȡ���������� �������ֵΪ��ֵ��1
    // ����  : unsigned int clientid  �����������±�
    // ����  : PCGATE_COMMSTRU data   ��������
    // ����  : int timeout ��ʱʱ����
    // ����  : ȡ���ͻ�㲥����
    int BFCGATE_TSI_GetReqData(unsigned int clientid, void* data, int timeout);

    // ������: BFCGATE_TSI_GetAnsData
    // ���  : ������ 2013-5-23 15:14:47
    // ����  : int <0���� =0��ʱ������  >=1�ɹ�ȡ���������� �������ֵΪ��ֵ��1
    // ����  : unsigned int clientid  �����������±�
    // ����  : PCGATE_COMMSTRU data   ��������
    // ����  : int timeout ��ʱʱ����
    // ����  : ȡ��ָ����ˮ��Ӧ��
    int BFCGATE_TSI_GetAnsData(unsigned int clientid, void* data, int timeout);

    // ������: BFCGATE_TSI_SendLogin
    // ���  : ������ 2013-5-23 15:12:27
    // ����  : int -1δ����  -2���ͳ���  =0 ��ʱδ�յ�Ӧ�� >=1�յ�Ӧ�� �������ֵΪ��ֵ��1
    // ����  : unsigned int clientid  �����������±�
    // ����  : PCGATE_COMMSTRU *data            ����Ӧ������
    // ����  : unsigned int timeout    ����Ӧ��ʱʱ�� <=0ֱ�ӷ���0��������Ӧ��
    // ����  : �������󲢽���Ӧ��
    int BFCGATE_TSI_SendLogin(unsigned int clientid, void* data, unsigned int timeout);

    // ������: BFCGATE_TSI_SendSms
    // ���  : ������ 2013-5-23 15:12:27
    // ����  : int -1δ����  -2���ͳ���  =0 ��ʱδ�յ�Ӧ�� >=1�յ�Ӧ�� �������ֵΪ��ֵ��1
    // ����  : unsigned int clientid  �����������±�
    // ����  : PCGATE_COMMSTRU data             ����Ӧ������
    // ����  : unsigned int timeout    ����Ӧ��ʱʱ�� <=0ֱ�ӷ���0��������Ӧ��
    // ����  : �������󲢽���Ӧ��
    int BFCGATE_TSI_SendSms(unsigned int clientid, void* data, unsigned int timeout);
	
}

#endif // !defined(AFX_GATEAPI_H__38F43A80_B734_4386_A0E1_1A8E395F79EC__INCLUDED_)
