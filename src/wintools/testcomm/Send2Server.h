// Send2Server.h: interface for the CSend2Server class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SEND2SERVER_H__34C40EF8_ABF3_4332_BB2E_01DB6E56302C__INCLUDED_)
#define AFX_SEND2SERVER_H__34C40EF8_ABF3_4332_BB2E_01DB6E56302C__INCLUDED_

#include "BF_Thread.h"
#include "BF_SocketTcp.h"
#include "SendData.h"
//#include "ClientApi.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


typedef struct 
{
	char ip[30];
	int  port;
	int  timeout;
	int  headlen;
	int  isinclude;
	int  zip;
	int  pwdflag;
	bool  shortconnect;
}S_COMM_PARA;

class CSend2Server : public CBF_Thread  
{
public:
	CSend2Server();
	virtual ~CSend2Server();
	virtual int Run();
	virtual bool Terminate();
	
private:

	CBF_SocketTcp m_tcpSocket;
	int m_sendNum;//发送次数
	
public:
	void SetParameter(int packtype,int sendnum,bool isrunning,CSendData *sndData,S_COMM_PARA *commpara,int threadNo=0);
	
	long m_usetime;//使用时间
	char m_errMsg[256];
	bool m_isRunning;
	S_COMM_PARA *m_commpara;
	CSendData *m_sendData;
	int m_mythreadNo;
	
	int m_packtype;
	
	bool m_bIsResult;
	
	int m_nFaildNum;
	int m_nSuccessNum;
	int m_nSendNum;
	int m_nSendErrorNum;
	int m_nConnectErrorNum;
	int m_nRecvErrorNum;

	bool m_bIsConnect;
	

//	CClientApi m_clientapi;
//	Cpubstrtime m_publog;
 	CBF_Date_Time m_publog;
 	CBF_Tools     m_pTool;
	CBF_SimpleLog m_log;
};

#endif // !defined(AFX_SEND2SERVER_H__34C40EF8_ABF3_4332_BB2E_01DB6E56302C__INCLUDED_)
