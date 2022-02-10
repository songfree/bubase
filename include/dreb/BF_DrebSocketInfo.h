// BF_DrebSocketInfo.h: interface for the CBF_DrebSocketInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BF_DREBSOCKETINFO_H__E9295B9F_3BA5_4361_B883_30412F15E49C__INCLUDED_)
#define AFX_BF_DREBSOCKETINFO_H__E9295B9F_3BA5_4361_B883_30412F15E49C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "public.h"
#include "BF_SocketTcp.h"
#include "BF_Mutex.h"
#include "BF_SimpleLog.h"
#include "BF_BufferPool.h"
#include "BF_DrebResource.h"
#include "BpcHead.h"
#include "BF_DrebSendQueue.h"
#include "DrebEndian.h"
#include "BpcEndian.h"
#include "BF_Des.h"
#include "BF_CommDataLog.h"
#include "IErrLog.h"

#ifdef DREBAPI_EXPORTS
#define DREBAPI_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define DREBAPI_EXPORT __declspec(dllimport)
#else
#define DREBAPI_EXPORT 
#endif
#endif

class DREBAPI_EXPORT CBF_DrebSocketInfo  
{
public:
	CBF_DrebSocketInfo();
	virtual ~CBF_DrebSocketInfo();


	std::string GetBpcMsgType(int msgtype);
	std::string GetDrebCmdType(int cmdtype);

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

	// ������: GetRecvData
	// ���  : ������ 2009-7-3 15:53:27
	// ����  : int ���ݳ��� С��0���ӹر� ����0���ݲ�����������0���յ������ݳ���
	// ����  : S_BPC_RSMSG *msg
	// ����  : ��ȡ���յ�������
	int GetRecvData(S_BPC_RSMSG *msg);
	

	// ������: SendMsg
	// ���  : ������ 2014-12-10 14:47:56
	// ����  : int 
	// ����  : S_BPC_RSMSG &msg
	// ����  : bool sendimmediate=false
	// ����  : ��������
	int SendMsg(S_BPC_RSMSG &msg,bool sendimmediate=false);
	
	// ������: OnSend
	// ���  : ������ 2009-7-3 15:35:30
	// ����  : int С��0����ʧ�� >0���͵���������
	// ����  : �Ӷ����﷢������
	int OnSend();
	
	// ������: OnRecv
	// ���  : ������ 2009-7-3 15:35:23
	// ����  : int ������С��0���ʾ���ӹرգ�С��ͨѶͷ��Ҫ�����գ�����ͨѶͷ����ȡ���ݽ��д�����
	// ����  : ��������
	int OnRecv();

	// ������: OnShutdown
	// ���  : ������ 2009-7-3 10:17:25
	// ����  : void 
	// ����  : ǿ�йر����ӣ��ø������Ϊ��ʼֵ
	void OnShutdown();
	
	// ������: OnClose
	// ���  : ������ 2009-7-3 10:16:06
	// ����  : void 
	// ����  : const char *msg
	// ����  : �ر����ӣ��������������ø������
	void OnClose(const char *msg,const char *filename,int fileline);
	
	// ������: IsCanSelect
	// ���  : ������ 2009-7-3 10:14:49
	// ����  : bool 
	// ����  : �������Ƿ����select����
	bool IsCanSelect();

	// ������: Init
	// ���  : ������ 2014-12-10 14:41:34
	// ����  : void 
	// ����  : CBF_DrebResource *res
	// ����  : CBF_BufferPool *pool
	// ����  : CIErrlog *perrlog
	// ����  : CBF_CommDataLog *pdatalog
	// ����  : ��ʼ��
	void Init(CBF_DrebResource *res,CBF_BufferPool *pool,CIErrlog *perrlog,CBF_CommDataLog *pdreb2bpclog,CBF_CommDataLog *pbpc2dreblog);

	// ������: Listen
	// ���  : ������ 2009-7-3 12:48:10
	// ����  : bool 
	// ����  : int port
	// ����  : bool blockflag=false  ������ʽ trueΪ����
	// ����  : int queuenum=5
	// ����  : ��ʼ�����������ڲ�����
	bool Listen(int port,bool blockflag=false,int queuenum=5);
	
	// ������: IsFree
	// ���  : ������ 2009-7-3 10:18:17
	// ����  : bool 
	// ����  : acceptʱȡ�������Ƿ����accept
	bool IsFree();


	std::string    m_sDrebIp; //dreb ip
	int            m_nDrebPort; //dreb port
	int            m_nDrebId;//�����ڵ��
	int            m_nDrebPrivateId;//˽�нڵ��

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

	CBF_DrebSendQueue       m_pSendQueue;//���Ͷ���

	DREB_HEAD m_head;//dreb��ͷ��Ϣ

protected:

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

	// ������: PutSendMsg
	// ���  : ������ 2009-7-3 15:35:48
	// ����  : int 
	// ����  : S_BPC_RSMSG msg
	// ����  : ����һ��Ҫ���͵����������в����跢�ͱ�־Ϊtrue
	int PutSendMsg(S_BPC_RSMSG msg);

	void ResetInfo();

	void CheckFirstData();

	char   m_errMsg[256];
	CBF_SocketTcp m_tcpSocket;
	CBF_Date_Time m_datetime;
	CBF_Des        m_pDes;

	//����ļ�����ָ��
	CBF_CommDataLog *m_pDreb2BpcDataLog;
	CBF_CommDataLog *m_pBpc2DrebDataLog;
	CBF_BufferPool *m_pMemPool;
	CBF_DrebResource  *m_gRes;
	CIErrlog          *m_log;


	CBF_Mutex  m_mutex;
	int m_sendNum;
	unsigned int  m_nBpcHeadLen;
	unsigned int  m_nDrebHeadLen;
	CDrebEndian   m_pDrebEndian;
	CBpcEndian    m_pBpcEndian;
};

#endif // !defined(AFX_BF_DREBSOCKETINFO_H__E9295B9F_3BA5_4361_B883_30412F15E49C__INCLUDED_)
