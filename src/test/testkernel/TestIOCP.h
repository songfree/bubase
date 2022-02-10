// TestIOCP.h: interface for the CTestIOCP class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTIOCP_H__8B5ADE27_2010_4595_9A90_E749034F0C20__INCLUDED_)
#define AFX_TESTIOCP_H__8B5ADE27_2010_4595_9A90_E749034F0C20__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "BF_BufferPool.h"
#include <WINSOCK2.H> 
#include <windows.h>
#include "BF_AIO_Head.h"
#include "BF_AIO_Pool.h"

typedef struct _PER_IO_DATA 
{ 
	OVERLAPPED         ol;      //完成端口结构
	PSOCKET_POOL_DATA  pooldata;  //框架结构
}PER_IO_DATA, *PPER_IO_DATA;

typedef struct _PER_HANDLE_DATA 
{ 
	SOCKET_HANDLE   socket;      // 对应的套接字句柄 
	PPER_IO_DATA    iodata;      // io操作数据指针
}PER_HANDLE_DATA, *PPER_HANDLE_DATA;   

typedef struct  _BF_IO_DATA
{
	PER_IO_DATA        iodata;    
	PER_HANDLE_DATA    sockinfo;
}BF_IO_DATA,*PBF_IO_DATA;          //可以只用此一个结构


class CTestIOCP  
{
public:

	CTestIOCP()
	{
		m_pIoMemPool.Init(1000,sizeof(BF_IO_DATA));//IO数据暂定为1000,根据需要增加,连接关闭回收
		m_pSocketPool.Init(1000);
	}
	virtual ~CTestIOCP()
	{

	}
	void AssociateIocp()
	{

	}
	bool Listen()
	{
		if (!m_pTcpSocket.Create(AF_INET,false))
		{
			printf("Create error\n");
			return false;
		}
		if (!m_pTcpSocket.Listen(m_nPort))
		{
			printf("Listen error\n");
			return false;
		}
		//关联到完成端口
		PBF_IO_DATA  pp = (PBF_IO_DATA)m_pIoMemPool.PoolMalloc(); 
		pp->iodata.pooldata->s_cOperType = OP_ACCEPT;
		pp->iodata.pooldata = NULL;  //不用接收数据
		CreateIoCompletionPort((HANDLE) pp->sockinfo.socket, m_hIocp, (DWORD)&(pp->iodata.ol), 0 );
	}
	bool CreateIOCP()
	{
		m_hIocp = ::CreateIoCompletionPort( INVALID_HANDLE_VALUE, 0, 0, 0 ); 
		if (m_hIocp == NULL)
		{
			printf("CreateIoCompletionPort 失败\n");
			return false;
		}
		return true;
	}
	void Test()
	{
		char tmpchar[20];
		CBF_Tools::GetConsoleEnter("请输入侦听端口",5,tmpchar);
		m_nPort = atoi(tmpchar);
		if (m_nPort <1024)
		{
			printf("侦听端口[%s]非法\n",tmpchar);
			return;
		}
		CreateIOCP();
		if (!Listen())
		{
			printf("Listen失败\n");
			return;
		}
		//创建工作线程

	}
	unsigned short    m_nPort;
	HANDLE            m_hIocp;
	SOCKET_HANDLE     m_socketListen;
	CBF_SocketTcp     m_pTcpSocket;
	CBF_BufferPool    m_pIoMemPool;
	CBF_AIO_Pool      m_pSocketPool;
};

#endif // !defined(AFX_TESTIOCP_H__8B5ADE27_2010_4595_9A90_E749034F0C20__INCLUDED_)
