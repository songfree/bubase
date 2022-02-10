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
	OVERLAPPED         ol;      //��ɶ˿ڽṹ
	PSOCKET_POOL_DATA  pooldata;  //��ܽṹ
}PER_IO_DATA, *PPER_IO_DATA;

typedef struct _PER_HANDLE_DATA 
{ 
	SOCKET_HANDLE   socket;      // ��Ӧ���׽��־�� 
	PPER_IO_DATA    iodata;      // io��������ָ��
}PER_HANDLE_DATA, *PPER_HANDLE_DATA;   

typedef struct  _BF_IO_DATA
{
	PER_IO_DATA        iodata;    
	PER_HANDLE_DATA    sockinfo;
}BF_IO_DATA,*PBF_IO_DATA;          //����ֻ�ô�һ���ṹ


class CTestIOCP  
{
public:

	CTestIOCP()
	{
		m_pIoMemPool.Init(1000,sizeof(BF_IO_DATA));//IO�����ݶ�Ϊ1000,������Ҫ����,���ӹرջ���
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
		//��������ɶ˿�
		PBF_IO_DATA  pp = (PBF_IO_DATA)m_pIoMemPool.PoolMalloc(); 
		pp->iodata.pooldata->s_cOperType = OP_ACCEPT;
		pp->iodata.pooldata = NULL;  //���ý�������
		CreateIoCompletionPort((HANDLE) pp->sockinfo.socket, m_hIocp, (DWORD)&(pp->iodata.ol), 0 );
	}
	bool CreateIOCP()
	{
		m_hIocp = ::CreateIoCompletionPort( INVALID_HANDLE_VALUE, 0, 0, 0 ); 
		if (m_hIocp == NULL)
		{
			printf("CreateIoCompletionPort ʧ��\n");
			return false;
		}
		return true;
	}
	void Test()
	{
		char tmpchar[20];
		CBF_Tools::GetConsoleEnter("�����������˿�",5,tmpchar);
		m_nPort = atoi(tmpchar);
		if (m_nPort <1024)
		{
			printf("�����˿�[%s]�Ƿ�\n",tmpchar);
			return;
		}
		CreateIOCP();
		if (!Listen())
		{
			printf("Listenʧ��\n");
			return;
		}
		//���������߳�

	}
	unsigned short    m_nPort;
	HANDLE            m_hIocp;
	SOCKET_HANDLE     m_socketListen;
	CBF_SocketTcp     m_pTcpSocket;
	CBF_BufferPool    m_pIoMemPool;
	CBF_AIO_Pool      m_pSocketPool;
};

#endif // !defined(AFX_TESTIOCP_H__8B5ADE27_2010_4595_9A90_E749034F0C20__INCLUDED_)
