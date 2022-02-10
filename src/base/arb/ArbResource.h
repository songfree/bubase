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


#include "BF_DrebResource.h"

class CArbResource : public CBF_DrebResource  
{
public:
	virtual bool Init(const char *confile);
	CArbResource();
	virtual ~CArbResource();

	bool          g_bToExit;
	std::string   m_sXdpDefineXml;//xdp�����ļ�
	int           m_nSwitchTime;//�л�ʱ�� �������ڴ�ʱ��δ��Ӧ�л���������ʱ��
	int           m_nSwitchSeg;//�л����   �����ʱ����ڲ��ü���Ƿ��л�

	vector<int> g_lArbTxList;//�ٲû���������ע��Ľ����б�
};

#endif // !defined(AFX_ARBRESOURCE_H__7EB044C4_3163_4702_85BF_1C99CAA5C716__INCLUDED_)
