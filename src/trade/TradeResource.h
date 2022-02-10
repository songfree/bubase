// ArbResource.h: interface for the CArbResource class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRADERESOURCE_H__7EB044C4_3163_4702_85BF_1C99CAA5C716__INCLUDED_)
#define AFX_TRADERESOURCE_H__7EB044C4_3163_4702_85BF_1C99CAA5C716__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define  APP_QUERY_HOST    9100001   //Ӧ�ò�ѯ״̬
#define  APP_PUSH_STATUS   9100003   //Ӧ������״̬

#define  ARB_SEND_QUERY    9100002  //arb��������״̬��ѯ
#define  ARB_SEND_CONTROL  9100004  //arb����ָ������
#define  ARB_SEND_UNREG_SVR    9100005  //arb����ȡ�������������ϵ�ע��

#define  TRADEFRONT_BAEAKPOINT 70000030  //���̻��ϵ�����������


#define   ARBSTATUS_UNKNOW       0  //δ֪״̬
#define   ARBSTATUS_MASTER       1  //����״̬
#define   ARBSTATUS_MASTER_AYN   3  //����ͬ��״̬  3-4��Ӧ��˵���������ںͱ�����ͬ������ 
#define   ARBSTATUS_BACKUP       2  //����״̬
#define   ARBSTATUS_BACKUP_AYN   4  //����ͬ��״̬
#define   MASTERHOST             10  //���ؾ�������״̬

#include "BF_DrebResource.h"
#include "BF_Queue.h"
#include "BF_3Des.h"

#define  DBPWDKEY   "rd402wms"
#define  DBPWD3DESKEY   "rd402wms20092017Ylink123" 

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
	long           nSerial_get;   //���ر�����
	unsigned int   serial;        //�������ˮ��

}__PACKED__ S_SHM_PUBLIC;
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

class CTradeResource : public CBF_DrebResource  
{
public:
	int  GetSerial();
	long GetLocalNo();

	CIErrlog  *m_log;

	virtual bool Init(const char *confile);
	CTradeResource();
	virtual ~CTradeResource();


	bool          g_bToExit;
	std::string   m_sXdpDefineXml;//xdp�����ļ�
	int           m_nTradeReportTime;//�л�ʱ�� �������ϱ����ٲõ�ʱ��
	unsigned  int m_nSvrHostStatus;//���������״̬ 
	int           m_nArbMode;//�ٲ�ģʽ  1Ϊ����  
	
	int            g_nListenPort;//�����˿�
	int            g_nMaxConnectNum;//����������
	
	int            g_nProcTheadNum;//ҵ�����߳���


	std::vector<int> g_lSvrTxList;//������������ע��Ľ����б�

	std::vector<int> g_lArbTxList;//������������ע����ٲý���

	CBF_Des       m_pDes;
	CBF_3Des      m_p3Des;

	int           g_nStatus;//����״̬ 0���ڳ�ʼ��  1��������    3�������������

	S_SHM_PUBLIC *m_pShmpub;//�����ڴ��е�����

	CBF_Mutex   m_mutex;
};

#endif // !defined(AFX_TRADERESOURCE_H__7EB044C4_3163_4702_85BF_1C99CAA5C716__INCLUDED_)
