// TibSendMgr.h: interface for the CTibSendMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIBSENDMGR_H__576E2AA0_6032_4FB4_A588_6CF88018B3BA__INCLUDED_)
#define AFX_TIBSENDMGR_H__576E2AA0_6032_4FB4_A588_6CF88018B3BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "public.h"
#include "BF_Xml.h"
#include "BF_SocketUdp.h"
#include "IErrLog.h"
#include "TibHead.h"
#include "PortTbl.h"
#include "UdpSocketInfo.h"

#ifdef TIB_EXPORTS
#define TIB_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define TIB_EXPORT __declspec(dllimport)
#else
#define TIB_EXPORT 
#endif
#endif

typedef struct 
{
	int  msgtype;
	CUdpSocketInfo *pUdpInfo;
} S_TIB_SEND_TBL;

//���͹���
class TIB_EXPORT CTibSendMgr  
{
public:

	// ������: IsMsgtype
	// ���  : ������ 2015-7-16 16:25:11
	// ����  : bool 
	// ����  : int msgtype
	// ����  : ��Ϣ�����Ƿ����
	bool IsMsgtype(int msgtype);

	// ������: SendData
	// ���  : ������ 2015-7-16 16:25:00
	// ����  : int 
	// ����  : TIBCOMMSTRU data
	// ����  : ��������
	int SendData(TIBCOMMSTRU data);
	

	// ������: Init
	// ���  : ������ 2015-6-30 14:42:16
	// ����  : bool 
	// ����  : CPortTbl *portlist
	// ����  : CIErrlog *errlog
	// ����  : CBF_Xml *tibxml
	// ����  : ��ʼ��
	bool Init(CPortTbl *portlist,CIErrlog *errlog,CBF_Xml *tibxml);

	CTibSendMgr();
	virtual ~CTibSendMgr();

protected:

	//��Ϣ���Ͷ�Ӧ�Ķ˿�
	bool     m_bIsInit;
	
	
	CPortTbl   *m_pListPort;//�˿��б�
	CIErrlog   *m_pLog;//��־��
	std::string m_sSendAddr;//���͵�ַ����172.168.255.255  ���㲥��ַ
	
	std::vector<CUdpSocketInfo *> m_pSendInfo;//Ҫ���͵Ķ˿��б�

	CMemTable<S_TIB_SEND_TBL> m_pMsgTbl;//��Ϣ���Ͷ�Ӧ�Ķ˿����
	CPkeyInt<1>               m_pMsgKey;

	TIBCOMMSTRU               m_sSendData;
	
	CBF_Mutex                 m_mutex;
private:
	CUdpSocketInfo * SelectByMsgtype(int msgtype);
	bool Insert(S_TIB_SEND_TBL tbl);
};

#endif // !defined(AFX_TIBSENDMGR_H__576E2AA0_6032_4FB4_A588_6CF88018B3BA__INCLUDED_)
