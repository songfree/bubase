// BF_AIO.h: interface for the CBF_AIO class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BF_AIO_H__2C090C2E_4D96_4548_A785_0731D797AED0__INCLUDED_)
#define AFX_BF_AIO_H__2C090C2E_4D96_4548_A785_0731D797AED0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BF_AIO_Base.h"
#include "BF_AIO_Pool.h"
#include "BF_AIO_Thread.h"
#include "BF_AIO_WorkBase.h"
#include "BF_SocketTcp.h"
#include "BF_Date_Time.h"
#include "BF_AIO_Resource.h"
#include "BF_Timer.h"
#include "IErrLog.h"
#ifdef LINUX
#include <sys/epoll.h>
#endif

#ifdef _WINDOWS
#include "mswsock.h"
#pragma comment(lib, "Mswsock.lib") 
#pragma message("Automatically linking with Mswsock.lib")
#endif

#ifdef _WINDOWS
#ifdef _DEBUG
#pragma comment(lib, "bf_kerneld.lib") 
#pragma message("Automatically linking with  bf_kerneld.lib")
#else
#pragma comment(lib, "bf_kernel.lib") 
#pragma message("Automatically linking with  bf_kernel.lib")
#endif
#endif

#ifdef AIO_EXPORTS
#define AIO_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define AIO_EXPORT __declspec(dllimport)
#else
#define AIO_EXPORT 
#endif
#endif



class AIO_EXPORT CBF_AIO : public CBF_AIO_Base  
{
public:
	

	// ������: GetErrMsg
	// ���  : ������ 2012-5-22 10:21:06
	// ����  : std::string 
	// ����  : ȡ�ó�����Ϣ
	std::string GetErrMsg();
	

	// ������: GetSocketErrNo
	// ���  : ������ 2012-5-24 15:03:40
	// ����  : int  
	// ����  : ȡ��iocp���ͳ���ʱ��ERRNO
	virtual int  GetSocketErrNo();

	// ������: Send
	// ���  : ������ 2012-5-21 16:52:24
	// ����  : bool 
	// ����  : PSOCKET_SEND_DATA senddata
	// ����  : bool isimmediate=false	 �Ƿ���������
	// ����  : bool islock=true	  �Ƿ����  
	// ����  : ��������
	virtual bool Send(SOCKET_SEND_DATA senddata,bool isimmediate=false,bool islock=true);
	
	// ������: AddAio
	// ���  : ������ 2012-5-21 10:03:52
	// ����  : bool 
	// ����  : PSOCKET_POOL_DATA info
	// ����  : ��������Ϣ���뵽AIO��
    virtual bool AddAio(PSOCKET_POOL_DATA info);

	// ������: Close
	// ���  : ������ 2012-5-22 11:16:09
	// ����  : virtual bool 
	// ����  : PSOCKET_POOL_DATA info
	// ����  : Ӧ�������ر�����
	virtual bool Close(PSOCKET_POOL_DATA info);
	
	// ������: Connect
	// ���  : ������ 2012-5-21 11:56:45
	// ����  : bool 
	// ����  : char *ip  ���ӵ�IP
	// ����  : short port  �˿�
	// ����  : unsigned  short  sockettype   ��������
	// ����  : unsigned int destindex        ����vc�Ļ�Ҫָ������������
	// ����  : unsigned int &index           AIO���ش���������
	// ����  : Ӧ��Ҫ�������ӣ�OnConnectedBack�����Ƿ�ɹ�����
	virtual bool Connect(char *ip,short port,unsigned  short  sockettype, unsigned int destindex ,unsigned int &index);


	// ������: ScanPool
	// ���  : ������ 2012-5-21 11:52:10
	// ����  : void 
	// ����  : ������ӣ��Ƿ���Ҫ����Ping����ʱ�Ͽ�
	void ScanPool();

	// ������: Stop
	// ���  : ������ 2012-5-21 11:48:32
	// ����  : bool 
	// ����  : ֹͣIOģ��
	bool Stop();

	// ������: Start
	// ���  : ������ 2012-5-21 11:48:17
	// ����  : bool 
	// ����  : ����IOģ��
	bool Start();

	
	
	CBF_AIO();
	virtual ~CBF_AIO();

	
public:
	
	// ������: SetParameter
	// ���  : ������ 2012-5-21 11:38:06
	// ����  : void 
	// ����  : unsigned int iothreadnum   IO������߳���
	// ����  : unsigned int maxfd         IO��������������
	// ����  : CBF_AIO_WorkBase *workcallback    IO������ɺ�Ļص���
	// ����  : ����AIO���в���
	void SetParameter(CBF_AIO_WorkBase *workcallback,CBF_AIO_Resource *res);
	
	

protected:
	static int OnTimer(unsigned int eventid,void *p);

	char m_sErrMsg[256];
	int  m_nSocketErrno;

	CIErrlog          *m_pLog;
	CBF_SocketTcp     m_pSocketTcp;
	CBF_Date_Time     m_pDate;
	CBF_AIO_Pool      m_pAioPool;
	
	CBF_Timer         m_pTimer;

	CBF_AIO_WorkBase  *m_pAioWork;
	CBF_AIO_Resource  *m_pAioRes;
	CBF_AIO_Thread    m_pAioThread[20];//IO�����߳��������20��

};

#endif // !defined(AFX_BF_AIO_H__2C090C2E_4D96_4548_A785_0731D797AED0__INCLUDED_)
