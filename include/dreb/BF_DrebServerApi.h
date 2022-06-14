// BF_DrebServerApi.h: interface for the CBF_DrebServerApi class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BF_DREBSERVERAPI_H__C49125EC_277E_43F5_8EAA_7AE1400C4DE1__INCLUDED_)
#define AFX_BF_DREBSERVERAPI_H__C49125EC_277E_43F5_8EAA_7AE1400C4DE1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BF_DrebServer.h"
#include "BF_DrebResource.h"
#include "IErrLog.h"



#ifdef __cplusplus
extern "C" {
#endif
	
	//�ֽڶ���
#ifdef _WINDOWS
#define __PACKED__ 
#pragma pack(push,1)
#else
#ifdef __GNUC__
#define __PACKED__	__attribute__ ((packed))
	//#pragma pack(push,1)
#else
#define __PACKED__
#pragma options align=packed
#endif
#endif
	

	typedef struct 
	{
		char          cMsgType;        //��Ϣ����  �����߷���������Ϣ���ͣ�����ʱ���á�
		unsigned int  index;           //��dreb���߹���������������ָ��Ҫ�������ݵ���������(�������ӵ�������˳����ѡ��)
		DREB_HEAD     sDBHead;         //drebͷ
		char   sBuffer[BPUDATASIZE];  
	}__PACKED__ DREBSAPI_COMMSTRU,*PDREBSAPI_COMMSTRU; //����API����

	//ȡ���ֽڶ���
#ifdef _WINDOWS
#pragma pack(pop)
#else
#ifdef __GNUC__
	//#pragma pack(pop)
#else
#pragma options align=reset
#endif
#endif
#undef __PACKED__
	
#ifdef __cplusplus
}
#endif
#ifdef DREBAPI_EXPORTS
#define DREBAPI_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define DREBAPI_EXPORT __declspec(dllimport)
#else
#define DREBAPI_EXPORT 
#endif
#endif

class DREBAPI_EXPORT CBF_DrebServerApi  
{
public:
    // ������: Subscribe
    // ���  : ������ 2022-06-14 14:16:47
    // ����  : void 
    // ����  : int index
    // ����  : std::vector<int> &funclist
    // ����  : �������϶��Ĺ㲥��Ϣ
    void Subscribe(int index, std::vector<int>& funclist);

    // ������: UnSubscribe
    // ���  : ������ 2022-06-14 14:16:49
    // ����  : void 
    // ����  : ȡ���������ϵĹ㲥����
    void UnSubscribe();

	bool OnMsgMonitor(S_BPC_RSMSG &rcvdata);

	// ������: RegisterDreb
	// ���  : ������ 2015-7-21 11:08:47
	// ����  : void 
	// ����  : int index
	// ����  : std::vector<int> &funclist
	// ����  : ��������ע�ύ�ף�һ�������ӷ��سɹ���Ϣ��ע��
	void RegisterDreb(int index,std::vector<int> &funclist);

	// ������: UnRegisterDreb
	// ���  : ������ 2015-7-21 11:06:44
	// ����  : void 
	// ����  : ȡ���������ϵĽ���ע��
	void UnRegisterDreb();

	// ������: GetMsg
	// ���  : ������ 2015-7-21 10:08:48
	// ����  : int 
	// ����  : S_BPC_RSMSG *gmsg
	// ����  : �����߶���ȡ��Ϣ
	int GetMsg(DREBSAPI_COMMSTRU *gmsg);

	// ������: SendMsg
	// ���  : ������ 2015-7-21 10:09:11
	// ����  : int 
	// ����  : S_BPC_RSMSG smsg
	// ����  : ������Ϣ������  a_cNodePrivateId����ʶ�Ƿ�ת�ƽ��ף��������յ��жϴ˱�ʶΪ100ʱ������s_info��������
	int SendMsg(DREBSAPI_COMMSTRU smsg);


	// ������: MonitorApi
	// ���  : ������ 2015-7-21 10:09:28
	// ����  : void 
	// ����  : �������API�߳�
	void MonitorApi();

	// ������: StopApi
	// ���  : ������ 2015-7-21 10:09:50
	// ����  : void 
	// ����  : ֹͣapi�߳�
	void StopApi();

	// ������: StartApi
	// ���  : ������ 2015-7-21 10:10:04
	// ����  : bool 
	// ����  : ����api�߳�
	bool StartApi();

	// ������: Init
	// ���  : ������ 2015-7-21 9:55:28
	// ����  : bool 
	// ����  : const char *conxmlfile
	// ����  :,CDrebMsgProcBase* spi=NULL   ���лص�ʱ��api�߳�ֱ�ӻص�������������
	// ����  : ��ʼ��API
	bool Init(const char *conxmlfile, CDrebMsgProcBase* spi=NULL);

	CBF_DrebServerApi();
	virtual ~CBF_DrebServerApi();


	CBF_DrebServer     m_pDrebApi;
	CBF_DrebResource   m_pRes;

	CDrebEndian   m_pDrebEndian;

	CIErrlog           *m_pLog;

	char               m_errMsg[256];

protected:
	void Upload(S_BPC_RSMSG &rcvdata);
	void Download(S_BPC_RSMSG &rcvdata);
	bool MonitorReport(S_BPC_RSMSG &rcvdata);
	bool MonitorBpu(S_BPC_RSMSG &rcvdata);
	bool MonitorHost(S_BPC_RSMSG &rcvdata);
	bool MonitorInfo(S_BPC_RSMSG &rcvdata);

	CBF_SimpleLog      log;
	
};




extern "C"
{	
	
	// ������: BFDREBAPI_InitApi
	// ���  : ������ 2013-12-4 14:20:53
	// ����  : int 0�ɹ� <0ʧ��
	// ����  : const char *apixml
	// ����  : void **pClass  ���ص�����������ָ��
	// ����  : ��ʼ��api����ȡ�����ļ����õ�֤��Ŀ¼�����ص�ַ�˿�
	DREBAPI_EXPORT	int BFDREBAPI_InitApi(const char *apixml,void **pClass);
	
	// ������: BFDREBAPI_FREE
	// ���  : ������ 2015-7-23 10:07:49
	// ����  : void 
	// ����  : void *pClass
	// ����  : �����˳��ͷ���Դ
	DREBAPI_EXPORT void BFDREBAPI_Free(void *pClass);

	// ������: BFDREBAPI_StartApi
	// ���  : ������ 2015-7-21 11:41:58
	// ����  : int 0�ɹ� <0ʧ��
	// ����  : void *pClass
	// ����  : ����API
	DREBAPI_EXPORT	int BFDREBAPI_StartApi(void *pClass);


	// ������: BFDREBAPI_StopApi
	// ���  : ������ 2015-7-21 11:42:20
	// ����  : void 
	// ����  : void *pClass
	// ����  : ֹͣAPI
	DREBAPI_EXPORT	void BFDREBAPI_StopApi(void *pClass);


	// ������: BFDREBAPI_MonitorApi
	// ���  : ������ 2015-7-21 11:42:29
	// ����  : void 
	// ����  : void *pClass
	// ����  : ���API�����߳�ֹͣ������
	DREBAPI_EXPORT	void BFDREBAPI_MonitorApi(void *pClass);


	// ������: int BFDREBAPI_SendMsg
	// ���  : ������ 2015-7-21 11:42:49
	// ����  : int  0�ɹ�  >0 ��������ٷ�  <0ʧ��   
	// ����  : void *pClass  ��ʼ��ʱ���ص�ָ��
	// ����  : const char *data   ����Ϊ DREBSAPI_COMMSTRU�Ľṹ
	// ����  : �������Ϸ�����Ϣ  a_cNodePrivateId����ʶ�Ƿ�ת�ƽ��ף��������յ��жϴ˱�ʶΪ100ʱ������s_info��������
	DREBAPI_EXPORT  int BFDREBAPI_SendMsg(void *pClass,const char *data);


	// ������: int BFDREBAPI_GetMsg
	// ���  : ������ 2015-7-21 11:43:51
	// ����  : int    0������Ҫ���� 1������   <0������
	// ����  : void *pClass ��ʼ��ʱ���ص�ָ��
	// ����  : char *data  ����Ϊ DREBSAPI_COMMSTRU�Ľṹ
	// ����  : ������API������ȡ��һ����Ϣ
	DREBAPI_EXPORT  int BFDREBAPI_GetMsg(void *pClass, char *data);


	// ������: BFDREBAPI_GetErrMsg
	// ���  : ������ 2015-7-21 11:46:01
	// ����  : int 
	// ����  : void *pClass ��ʼ��ʱ���ص�ָ��
	// ����  : char *errmsg 
	// ����  : ȡ�ó�����Ϣ
	DREBAPI_EXPORT  int BFDREBAPI_GetErrMsg(void *pClass,char *errmsg);


	// ������: BFDREBAPI_UnRegisterDreb
	// ���  : ������ 2015-7-21 11:46:18
	// ����  : void 
	// ����  : void *pClass
	// ����  : ȡ����������ע��Ľ���
	DREBAPI_EXPORT  void BFDREBAPI_UnRegisterDreb(void *pClass);


	// ������: BFDREBAPI_RegisterDreb
	// ���  : ������ 2015-7-21 11:46:30
	// ����  : 
	// ����  : void *pClass    ��ʼ��ʱ���ص�ָ��
	// ����  : int index       ��ע���������������
	// ����  : char *txlist    ��|�ָ��Ľ����룬����  1|2|3|4|  
	// ����  : ��������ע�ύ��
	DREBAPI_EXPORT  void BFDREBAPI_RegisterDreb(void *pClass,int index,char *txlist);

    // ������: BFDREBAPI_UnSubscribe
	// ���  : ������ 2022-06-14 14:38:30
    // ����  : void 
    // ����  : void *pClass
    // ����  : ȡ���������ϵĹ㲥����
    DREBAPI_EXPORT  void BFDREBAPI_UnSubscribe(void* pClass);


    // ������: BFDREBAPI_Subscribe
    // ���  : ������ 2022-06-14 14:38:30
    // ����  : 
    // ����  : void *pClass    ��ʼ��ʱ���ص�ָ��
    // ����  : int index       ��ע���������������
    // ����  : char *txlist    ��|�ָ��Ľ����룬����  1|2|3|4|  
    // ����  : �������϶��Ĺ㲥
    DREBAPI_EXPORT  void BFDREBAPI_Subscribe(void* pClass, int index, char* txlist);

}


#endif // !defined(AFX_BF_DREBSERVERAPI_H__C49125EC_277E_43F5_8EAA_7AE1400C4DE1__INCLUDED_)
