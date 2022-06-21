// ArbResource.h: interface for the CArbResource class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARBRESOURCE_H__7EB044C4_3163_4702_85BF_1C99CAA5C716__INCLUDED_)
#define AFX_ARBRESOURCE_H__7EB044C4_3163_4702_85BF_1C99CAA5C716__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define  APP_QUERY_HOST    9100001   //Ӧ�ò�ѯ״̬
#define  APP_PUSH_STATUS   9100003   //Ӧ������״̬

#define  ARB_SEND_QUERY    9100002  //arb��������״̬��ѯ
#define  ARB_SEND_CONTROL  9100004  //arb����ָ������
#define  ARB_SEND_UNREG_SVR    9100005  //arb����ȡ�������������ϵ�ע��

#define   ARBSTATUS_UNKNOW       0  //δ֪״̬
#define   ARBSTATUS_MASTER       1  //����״̬
#define   ARBSTATUS_MASTER_AYN   3  //����ͬ��״̬  3-4��Ӧ��˵���������ںͱ�����ͬ������ 
#define   ARBSTATUS_BACKUP       2  //����״̬
#define   ARBSTATUS_BACKUP_AYN   4  //����ͬ��״̬


#include "BF_DrebResource.h"

class CArbResource : public CBF_DrebResource  
{
public:
	virtual bool Init(const char *confile);
	CArbResource();
	virtual ~CArbResource();

	bool          g_bToExit;
	std::string   m_sXdpDefineXml;//xdp�����ļ�
	int           m_nArbReportTime;//�л�ʱ�� �������ϱ����ٲõ�ʱ��
	unsigned  int m_nSvrHostStatus;//���������״̬   

	std::vector<int> g_lSvrTxList;//������������ע��Ľ����б�
};

#endif // !defined(AFX_ARBRESOURCE_H__7EB044C4_3163_4702_85BF_1C99CAA5C716__INCLUDED_)
