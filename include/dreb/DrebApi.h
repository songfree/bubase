// DrebApi.h: interface for the CDrebApi class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DREBAPI_H__E74BA6E7_E5A3_4C93_8FE1_DACDB0AB6D28__INCLUDED_)
#define AFX_DREBAPI_H__E74BA6E7_E5A3_4C93_8FE1_DACDB0AB6D28__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



extern "C"
{	
	
	// ������: BFDREBAPI_InitApi
	// ���  : ������ 2013-12-4 14:20:53
	// ����  : int 0�ɹ� <0ʧ��
	// ����  : const char *apixml
	// ����  : void **pClass  ���ص�����������ָ��
	// ����  : ��ʼ��api����ȡ�����ļ����õ�֤��Ŀ¼�����ص�ַ�˿�
	int BFDREBAPI_InitApi(const char *apixml,void **pClass);
	

	// ������: BFDREBAPI_FREE
	// ���  : ������ 2015-7-23 10:07:49
	// ����  : void 
	// ����  : void *pClass
	// ����  : �����˳��ͷ���Դ
	void BFDREBAPI_Free(void *pClass);
	
	// ������: BFDREBAPI_StartApi
	// ���  : ������ 2015-7-21 11:41:58
	// ����  : int 0�ɹ� <0ʧ��
	// ����  : void *pClass
	// ����  : ����API
	int BFDREBAPI_StartApi(void *pClass);
	
	
	// ������: BFDREBAPI_StopApi
	// ���  : ������ 2015-7-21 11:42:20
	// ����  : void 
	// ����  : void *pClass
	// ����  : ֹͣAPI
	void BFDREBAPI_StopApi(void *pClass);
	
	
	// ������: BFDREBAPI_MonitorApi
	// ���  : ������ 2015-7-21 11:42:29
	// ����  : void 
	// ����  : void *pClass
	// ����  : ���API�����߳�ֹͣ������
	void BFDREBAPI_MonitorApi(void *pClass);
	
	
	// ������: int BFDREBAPI_SendMsg
	// ���  : ������ 2015-7-21 11:42:49
	// ����  : int  0�ɹ�  >0 ��������ٷ�  <0ʧ��   
	// ����  : void *pClass  ��ʼ��ʱ���ص�ָ��
	// ����  : const char *data   ����Ϊ DREBSAPI_COMMSTRU�Ľṹ
	// ����  : �������Ϸ�����Ϣ
	 int BFDREBAPI_SendMsg(void *pClass,const char *data);
	
	
	// ������: int BFDREBAPI_GetMsg
	// ���  : ������ 2015-7-21 11:43:51
	// ����  : int    0������Ҫ���� 1������   <0������
	// ����  : void *pClass ��ʼ��ʱ���ص�ָ��
	// ����  : char *data  ����Ϊ DREBSAPI_COMMSTRU�Ľṹ
	// ����  : ������API������ȡ��һ����Ϣ
	int BFDREBAPI_GetMsg(void *pClass, char *data);
	
	
	// ������: BFDREBAPI_GetErrMsg
	// ���  : ������ 2015-7-21 11:46:01
	// ����  : int
	// ����  : void *pClass ��ʼ��ʱ���ص�ָ��
	// ����  : char *errmsg
	// ����  : ȡ�ó�����Ϣ
	 int BFDREBAPI_GetErrMsg(void *pClass,char *errmsg);
	
	
	// ������: BFDREBAPI_UnRegisterDreb
	// ���  : ������ 2015-7-21 11:46:18
	// ����  : void 
	// ����  : void *pClass
	// ����  : ȡ����������ע��Ľ���
	 void BFDREBAPI_UnRegisterDreb(void *pClass);
	
	
	// ������: BFDREBAPI_RegisterDreb
	// ���  : ������ 2015-7-21 11:46:30
	// ����  : 
	// ����  : void *pClass    ��ʼ��ʱ���ص�ָ��
	// ����  : int index       ��ע���������������
	// ����  : char *txlist    ��|�ָ��Ľ����룬����  1|2|3|4|  
	// ����  : ��������ע�ύ��
	 void BFDREBAPI_RegisterDreb(void *pClass,int index,char *txlist);

     // ������: BFDREBAPI_UnSubscribe
    // ���  : ������ 2022-06-14 14:20:18
    // ����  : void 
    // ����  : void *pClass
    // ����  : ȡ���������ϵĹ㲥����
     void BFDREBAPI_UnSubscribe(void* pClass);


     // ������: BFDREBAPI_Subscribe
     // ���  : ������ 2022-06-14 14:20:25
     // ����  : 
     // ����  : void *pClass    ��ʼ��ʱ���ص�ָ��
     // ����  : int index       ��ע���������������
     // ����  : char *txlist    ��|�ָ��Ľ����룬����  1|2|3|4|  
     // ����  : �������϶��Ĺ㲥��Ϣ
     void BFDREBAPI_Subscribe(void* pClass, int index, char* txlist);
	
}


#endif // !defined(AFX_DREBAPI_H__E74BA6E7_E5A3_4C93_8FE1_DACDB0AB6D28__INCLUDED_)
