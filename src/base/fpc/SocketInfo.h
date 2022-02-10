// SocketInfo.h: interface for the CSocketInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOCKETINFO_H__6195E011_BD18_493A_BAA5_561125709E5C__INCLUDED_)
#define AFX_SOCKETINFO_H__6195E011_BD18_493A_BAA5_561125709E5C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "BF_SocketTcp.h"
#include "BF_Mutex.h"
#include "BF_SimpleLog.h"
#include "BF_BufferPool.h"
#include "SendQueue.h"
#include "BfDefine.h"
#include "GResource.h"


class  CSocketInfo  
{
public:
	int SendMsg(S_BF_RSMSG_FPC msg);
	void Reverse(unsigned char *buf, int len);

	void Init(CGResource *res,CBF_BufferPool *pool);


	// ������: NeedSend
	// ���  : ������ 2010-3-12 13:55:22
	// ����  : bool 
	// ����  : �Ƿ�������Ҫ����
	bool NeedSend();

	void SetAccept(SOCKET_HANDLE sock,const char *address);

	/// ������: SetClient
	/// ���  : ������ 2009-7-13 15:44:09
	/// ����  : void 
	/// ����  : ��������Ϊ�ͻ���
	void SetClient();

	// ������: SetBuClient
	// ���  : ������ 2010-3-18 11:53:23
	// ����  : void 
	// ����  : ��������BU����
	void SetBuClient();

	/// ������: GetRecvData
	/// ���  : ������ 2009-7-3 15:53:27
	/// ����  : int ���ݳ��� С��0���ӹر� ����0���ݲ�����������0���յ������ݳ���
	/// ����  : S_BF_RSMSG *msg
	/// ����  : ��ȡ���յ�������
	int GetRecvData(S_BF_RSMSG_FPC *msg);


	

	/// ������: OnSend
	/// ���  : ������ 2009-7-3 15:35:30
	/// ����  : int С��0����ʧ�� >0���͵����ݳ��� 0�����ݷ���
	/// ����  : ��������
	int OnSend();

	/// ������: OnRecv
	/// ���  : ������ 2009-7-3 15:35:23
	/// ����  : int ������С��0���ʾ���ӹرգ�С��ͨѶͷ��Ҫ�����գ�����ͨѶͷ����ȡ���ݽ��д�����
	/// ����  : ��������
	int OnRecv();
	

	/// ������: Listen
	/// ���  : ������ 2009-7-3 12:48:10
	/// ����  : bool 
	/// ����  : int port
	/// ����  : bool blockflag=false  ������ʽ trueΪ����
	/// ����  : int queuenum=5
	/// ����  : ��ʼ�����������ڲ�����
	bool Listen(int port,bool blockflag=false,int queuenum=5);

	/// ������: IsFree
	/// ���  : ������ 2009-7-3 10:18:17
	/// ����  : bool 
	/// ����  : acceptʱȡ�������Ƿ����accept
	bool IsFree();

	/// ������: OnShutdown
	/// ���  : ������ 2009-7-3 10:17:25
	/// ����  : void 
	/// ����  : ǿ�йر����ӣ��ø������Ϊ��ʼֵ
	void OnShutdown();

	/// ������: OnClose
	/// ���  : ������ 2009-7-3 10:16:06
	/// ����  : void 
	/// ����  : const char *msg
	/// ����  : �ر����ӣ��������������ø������
	void OnClose(const char *msg);

	/// ������: IsCanSelect
	/// ���  : ������ 2009-7-3 10:14:49
	/// ����  : bool 
	/// ����  : �������Ƿ����select����
	bool IsCanSelect();

	CSocketInfo();
	virtual ~CSocketInfo();



//��������
	int            m_nBeatheartcount;        //������������
	bool           m_bIsBeat;                //��ͨѶƽ̨�Ƿ�Ҫ������

	char           m_sAddress[33]; //�ͻ���IP,���ӹ�����IP
//socket��������
	SOCKET_HANDLE  m_sock;                   //socket id
	int            m_nType;                  //SOCKET����
	int            m_nPort;                  //�����˿�

	bool           m_bNeedread;              //�Ƿ���Ҫ������

    char           m_cCreateDate[9];         //���ӽ�������
	int            m_nConntime;              //���ӽ���ʱ��

	int            m_nReadtime;              //�ϴζ�����ʱ��
	int            m_nSendtime;              //�ϴη�������ʱ�� 

	char           m_cCloseDate[9];         //���ӹر�����
	int            m_nCloseTime;             //�ر�ʱ��
	int            m_nPingTime;              //onpingʱ��
	
	int            m_nPid;                   //����������fpu�Ľ���ID


//���ͽ�������	
	char *m_sRcvBuffer;            //�������ݻ���   
	unsigned int  m_nRcvBufLen;                //���յ����ݳ���  �ɶ�ν���һ�����ݰ�

	char *m_sSendBuffer;            //�������ݻ���   
	unsigned int  m_nSendBufLen;                //���͵����ݳ���  �ɶ�ν���һ�����ݰ�

	int           m_nQueueTime;                //���������ݳ�����ʱ����ɾ�������ͣ���λ��

	int  m_index;

	
	CSendQueue       m_pSendQueue;//���Ͷ���
	bool      m_bIsBuGetFlag;//BU�����������Ƿ�ɷַ�����

protected :
	int           m_sendNum;                   //���ӷ��ʹ���



	/// ������: PutSendMsg
	/// ���  : ������ 2009-7-3 15:35:48
	/// ����  : int 
	/// ����  : S_DRTP_RSMSG msg
	/// ����  : ����һ��Ҫ���͵����������в����跢�ͱ�־Ϊtrue
	int PutSendMsg(S_BF_RSMSG_FPC msg);


	void ResetInfo();
	void ClearQueue();
	

	char   m_errMsg[256];

	CBF_SocketTcp m_tcpSocket;
	CBF_Tools  m_pTool;
	CIErrlog *m_log;	
	CBF_Date_Time m_publog;

	CBF_BufferPool *m_pMemPool;
	CGResource  *m_gRes;

	
	int     m_nLogLevel;
	string  m_sLogFile;
	CBF_Mutex  m_mutex;
 	
};



#endif // !defined(AFX_SOCKETINFO_H__6195E011_BD18_493A_BAA5_561125709E5C__INCLUDED_)
