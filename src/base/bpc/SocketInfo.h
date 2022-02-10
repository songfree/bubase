// SocketInfo.h: interface for the CSocketInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOCKETINFO_H__836AC69E_8563_408B_BFBD_E4195C73718A__INCLUDED_)
#define AFX_SOCKETINFO_H__836AC69E_8563_408B_BFBD_E4195C73718A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "BF_SocketTcp.h"
#include "BF_Mutex.h"
#include "BF_SimpleLog.h"
#include "BF_BufferPool.h"
#include "GResource.h"
#include "BpcHead.h"
#include "SendQueue.h"
#include "DrebEndian.h"
#include "BpcEndian.h"
#include "BF_Des.h"

#ifdef _WINDOWS
#ifdef _DEBUG
#pragma comment(lib, "bf_dbpubd.lib") 
#pragma message("Automatically linking with   bf_dbpubd.lib ")
#else
#pragma comment(lib, "bf_dbpub.lib") 
#pragma message("Automatically linking with   bf_dbpub.lib ")
#endif

#endif

class CSocketInfo  
{
public:
	bool IsBpuFree();
	void SetBpuFreeFlag(bool isfree);
	CSocketInfo();
	virtual ~CSocketInfo();

	

	std::string GetBpcMsgType(int msgtype);
	std::string GetDrebCmdType(int cmdtype);

	void SetAccept(SOCKET_HANDLE sock,const char *address);

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

	/// ������: GetRecvData
	/// ���  : ������ 2009-7-3 15:53:27
	/// ����  : int ���ݳ��� С��0���ӹر� ����0���ݲ�����������0���յ������ݳ���
	/// ����  : S_BPC_RSMSG *msg
	/// ����  : ��ȡ���յ�������
	int GetRecvData(S_BPC_RSMSG *msg);
	
	int SendMsg(S_BPC_RSMSG &msg,bool sendimmediate=false);
	
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

	void Init(CGResource *res,CBF_BufferPool *pool);

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

	std::string    m_sBpuGroupName;//bpu������
	int            m_nBpuGroupIndex;    //bpu�������

	std::string    m_sDrebIp; //dreb ip
	int            m_nDrebPort; //dreb port
	bool           m_DrebIsIpc;
	int            m_nDrebId;//�����ڵ��
	int            m_nDrebPrivateId;//˽�нڵ��

	char           m_sAddress[40]; //�ͻ���IP,���ӹ�����IP
	SOCKET_HANDLE  m_sock;                   //socket id
	int            m_nType;                  //SOCKET����
	int            m_nPort;                  //�����˿�
	bool           m_bChecked;
	bool           m_bNeedread;              //�Ƿ���Ҫ������
	bool           m_bNeedConnect;//�Ƿ���Ҫ����
    char           m_cCreateDate[9];         //���ӽ�������
	int            m_nConntime;              //���ӽ���ʱ��
	int            m_nReadtime;              //�ϴζ�����ʱ��
	int            m_nSendtime;              //�ϴη�������ʱ�� 
	char           m_cCloseDate[9];         //���ӹر�����
	int            m_nCloseTime;             //�ر�ʱ��
	int            m_nPingTime;              //onpingʱ��
	int            m_nPid;                   //����������bpu�Ľ���ID
	char m_sRcvBuffer[BPCBUFSIZE];            //�������ݻ���   
	unsigned int  m_nRcvBufLen;                //���յ����ݳ���  �ɶ�ν���һ�����ݰ�
	char m_sSendBuffer[BPCBUFSIZE];            //�������ݻ���   
	unsigned int  m_nSendBufLen;                //���͵����ݳ���  �ɶ�ν���һ�����ݰ�
	int  m_index;
	
	CSendQueue       m_pSendQueue;//���Ͷ���

	DREB_HEAD m_head;//dreb��ͷ��Ϣ

protected:
	bool      m_bIsBuGetFlag;//BU�����������Ƿ�ɷַ�����

	// ������: UnzipBuf
	// ���  : ������ 2013-5-2 16:07:55
	// ����  : bool 
	// ����  : BPCCOMMSTRU &data
	// ����  : ��cZipΪ1-4�Ľ��н��
	bool UnzipBuf(BPCCOMMSTRU &data);
	
	// ������: ZipBuf
	// ���  : ������ 2013-5-2 16:08:18
	// ����  : bool 
	// ����  : PBPCCOMMSTRU data
	// ����  : int zipflag   1-4 
	// ����  : ��czipΪ0�ĸ������ý��м��ܻ�ѹ������
	bool ZipBuf(PBPCCOMMSTRU data,int zipflag);

	/// ������: PutSendMsg
	/// ���  : ������ 2009-7-3 15:35:48
	/// ����  : int 
	/// ����  : S_BPC_RSMSG msg
	/// ����  : ����һ��Ҫ���͵����������в����跢�ͱ�־Ϊtrue
	int PutSendMsg(S_BPC_RSMSG msg);

	void ResetInfo();

	void CheckFirstData();

	char   m_errMsg[256];
	CBF_SocketTcp m_tcpSocket;
	CIErrlog   *m_log;	
	CBF_Date_Time m_datetime;
	CBF_Des        m_pDes;
	
	CBF_BufferPool *m_pMemPool;
	CGResource  *m_gRes;
	CBF_Mutex  m_mutex;
	int m_sendNum;
	unsigned int  m_nBpcHeadLen;
	unsigned int  m_nDrebHeadLen;
	CDrebEndian   m_pDrebEndian;
	CBpcEndian    m_pBpcEndian;


	CBF_Mutex  m_mutexFreeFlag;

};

#endif // !defined(AFX_SOCKETINFO_H__836AC69E_8563_408B_BFBD_E4195C73718A__INCLUDED_)
