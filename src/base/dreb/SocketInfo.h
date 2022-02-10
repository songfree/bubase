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
#include "DrebPubDefine.h"
#include "BF_Mutex.h"
#include "IErrLog.h"
#include "BF_Tools.h"
#include "BF_BufferPool.h"
#include "DrebStatInfo.h"
#include "SendQueue.h"
#include "DrebEndian.h"
#include "GlobalVar.h"

// #define SOCK_UNKNOW   1       //δ֪
// #define SOCK_CLI      2       //�ͻ���
// #define SOCK_DREB     3       //�����������ߵ����������ߵ�����
// #define SOCK_SRV      4       //ע��ķ����
// #define SOCK_CENTER   5       //�������������߽ڵ������
// #define SOCK_SLOT	 6       //ע���ͨѶ�� 
// #define SOCK_LISTEN	 7       //�����˿�
 
typedef struct 
{
	char    sAddress[33]; //��������IP
	unsigned int nPort;   //�������߶˿�
	unsigned short nNodeId;              //�������߱��
	char           cNodePrivateId;       //��������˽�����
	bool           bNeedConnect;           //��Ҫ����
	int            nbandwidth;     //����
}DREB_CENTER; //���ӵ��������߲���

typedef struct 
{
	unsigned short nNodeId;              //�������߱��
	char           cNodePrivateId;       //��������˽�����
	int            nbandwidth;     //����
}DREB_CONN; //���ӹ������������߲���

typedef struct 
{
	unsigned int nPort;   //�����˿�
	
}DREB_LISTEN; //��������

typedef struct 
{
	unsigned int   nSvrMainId;           //ע�������������ϵ��������ܺ�   ֻ�з������ʱ������д
	char           cSvrPrivateId;        //ע�������������ϵ�˽����� 0��ʾ���� >0Ϊÿ���������ţ�s_nSvrMainId+s_cSvrPrivateId ��Ϊ˽�й��ܺ�   ֻ�з������ʱ������д
}DREB_SRV;

#define  WLOG_RCV   0
#define  WLOG_SND   1
#define  WLOG_ERR   2


class CSocketInfo  
{
public:

	// ������: GetDrebCmdType
	// ���  : ������ 2013-5-13 15:01:35
	// ����  : std::string 
	// ����  : int cmdtype
	// ����  : ȡ��CMD������
	std::string GetDrebCmdType(int cmdtype);

	// ������: GetSockType
	// ���  : ������ 2013-5-13 14:18:01
	// ����  : std::string 
	// ����  : int ntype
	// ����  : ȡsocket��������
	std::string GetSockType(int ntype);

	void ResetSInfo();

	// ������: NeedRead
	// ���  : ������ 2010-7-22 9:28:09
	// ����  : bool 
	// ����  : �����Ƿ���Ҫ��
	bool NeedRead();

	// ������: NeedConnect
	// ���  : ������ 2010-7-22 9:24:20
	// ����  : bool 
	// ����  : �Ƿ���Ҫ�����������ӣ�ֻ����������ΪSOCK_TODREBʱ����Ҫ����
	bool NeedConnect();

	// ������: NeedSend
	// ���  : ������ 2010-7-22 9:23:48
	// ����  : bool 
	// ����  : �Ƿ���Ҫ�������ݣ�������������ʱ����Ҫ����
	bool NeedSend();

	/// ������: SetConnectDreb
	/// ���  : ������ 2009-10-26 16:27:48
	/// ����  : void 
	/// ����  : SOCKET_HANDLE sock
	/// ����  : int conntime
	/// ����  : CSendQueue *sque
	/// ����  : ��������Ϊ����������������
	void SetConnectDreb(SOCKET_HANDLE sock,int conntime,CSendQueue *sque);

	/// ������: SetAccept
	/// ���  : ������ 2009-7-14 13:42:50
	/// ����  : void 
	/// ����  : SOCKET_HANDLE sock
	/// ����  : const char *address
	/// ����  : ��������Ϊ��������
	void SetAccept(SOCKET_HANDLE sock,const char *address);

	/// ������: SetSvr
	/// ���  : ������ 2009-7-13 15:44:42
	/// ����  : void 
	/// ����  : unsigned int   nSvrMainId
	/// ����  : char   cSvrPrivateId
	/// ����  : CSendQueue *sque
	/// ����  : ��������Ϊע��ķ�������
	void SetSvr(unsigned int   nSvrMainId,char   cSvrPrivateId,CSendQueue *sque);

	/// ������: SetDreb
	/// ���  : ������ 2009-7-13 15:44:23
	/// ����  : void 
	/// ����  : unsigned short NodeId
	/// ����  : char  cNodePrivateId
	/// ����  : int bdrate 
	/// ����  : CSendQueue *sque
	/// ����  : ��������Ϊ�����������ߺͱ��������ߵ�����
	void SetDreb(unsigned short NodeId,char  cNodePrivateId,int bdrate,CSendQueue *sque);


	/// ������: SetClient
	/// ���  : ������ 2009-7-13 15:44:09
	/// ����  : void 
	/// ����  : ��������Ϊ�ͻ���
	void SetClient();


	/// ������: SetToDreb
	/// ���  : ������ 2009-7-13 15:43:54
	/// ����  : void 
	/// ����  : const char *ip
	/// ����  : int port
	/// ����  : ��������Ϊ���������ߵ�����
	void SetToDreb(const char *ip,int port,int brate);

	



	/// ������: GetRecvData
	/// ���  : ������ 2009-7-3 15:53:27
	/// ����  : int ���ݳ��� С��0���ӹر� ����0���ݲ�����������0���յ������ݳ���
	/// ����  : S_DREB_RSMSG *msg
	/// ����  : ��ȡ���յ�������
	int GetRecvData(S_DREB_RSMSG *msg);

	
	/// ������: SendMsg
	/// ���  : ������ 2009-7-3 15:35:48
	/// ����  : int 
	/// ����  : S_DREB_RSMSG *msg
	/// ����  : ������Ϣ�����ɹ��������
	int SendMsg(S_DREB_RSMSG *msg,bool sendimmediate=false);

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

	bool ListenIpc(int port,bool blockflag=false,int queuenum=5);
	

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
	void OnClose(const char *msg,const char *filename,int fileline);

	/// ������: IsCanSelect
	/// ���  : ������ 2009-7-3 10:14:49
	/// ����  : bool 
	/// ����  : �������Ƿ����select����
	bool IsCanSelect();

	// ������: SetGlobalVar
	// ���  : ������ 2009-7-3 16:18:43
	// ����  : void 
	// ����  : CGlobalVar *pVar 
	// ����  : CPoolBuffer *pool 
	// ����  : int deltime �����ڶ����ﳬ����ʱ����ɾ��
	// ����  : ���ò�������ʼ��
	void SetGlobalVar(CGlobalVar *pVar,CBF_BufferPool *pool,int deltime);

	CSocketInfo();
	virtual ~CSocketInfo();


	DREB_CENTER    m_s_center;  //�����������߲���
	DREB_CONN      m_s_conn;    //���ӹ�����dreb���������߲���
	DREB_LISTEN    m_s_listen;  //��������
	DREB_SRV       m_s_srv;     //ע��ķ������
//��������
	int            m_nBeatheartcount;        //������������
	
	char           m_sAddress[33]; //�ͻ���IP,���ӹ�����IP
//socket��������
	SOCKET_HANDLE  m_sock;                   //socket id
	int            m_nType;                  //socket type SOCK_UNKNOW SOCK_CLI SOCK_DREB SOCK_SRV SOCK_TODREB
	bool           m_bChecked;               //�Ƿ�����֤������ȷ�Ϲ���������
	
	bool           m_bNeedread;              //�Ƿ���Ҫ������

    char           m_cCreateDate[9];         //���ӽ�������
	int            m_nConntime;              //���ӽ���ʱ��

	int            m_nReadtime;              //�ϴζ�����ʱ��
	int            m_nSendtime;              //�ϴη�������ʱ�� 

	char           m_cCloseDate[9];         //���ӹر�����
	int            m_nCloseTime;             //�ر�ʱ��
	int            m_nPingTime;              //onpingʱ��
	
	unsigned int     m_nSend_packs;          //�����ӽ������͵����ݰ�
	unsigned int     m_nRcv_packs;           //�����ӽ�������յ����ݰ�
	unsigned long    m_nSendBytes;           //�����ӽ������͵��ֽ���
	unsigned long    m_nRcvBytes;            //�����ӽ�������յ��ֽ���

//���ͽ�������	
	char m_sRcvBuffer[DREBBUFSIZE];            //�������ݻ���   
	unsigned int  m_nRcvBufLen;                //���յ����ݳ���  �ɶ�ν���һ�����ݰ�

	int           m_nQueueTime;                //���������ݳ�����ʱ����ɾ�������ͣ���λ��

	int  m_index;                              //���������������ӳ��е����

	
	CSendQueue       *m_pSendQueue;//���Ͷ���

protected :
	int           m_sendNum;                   //���ӷ��ʹ���

	void ResetInfo();
	void ClearQueue();
	/// ������: PutSendMsg
	/// ���  : ������ 2009-7-3 15:35:48
	/// ����  : int 
	/// ����  : S_DREB_RSMSG *msg
	/// ����  : ����һ��Ҫ���͵����������в����跢�ͱ�־Ϊtrue
	int PutSendMsg(S_DREB_RSMSG *msg);

	void CheckFirstData();

	char   m_errMsg[256];

	char m_sSendBuffer[DREBBUFSIZE];            //��ʱ����ѹ����   

	CBF_SocketTcp  m_tcpSocket;
	CIErrlog  *m_log;	
	CBF_Date_Time  m_datetime;
	CBF_BufferPool *m_pMemPool;
	CDrebStatInfo *m_pStatInfo;
	CGlobalVar    *m_pRes;
	
	int     m_nLogLevel;
	string  m_sLogFile;
	CBF_Mutex  m_mutex;

	CDrebEndian  m_pDrebEndian;

private:


};



#endif // !defined(AFX_SOCKETINFO_H__6195E011_BD18_493A_BAA5_561125709E5C__INCLUDED_)
